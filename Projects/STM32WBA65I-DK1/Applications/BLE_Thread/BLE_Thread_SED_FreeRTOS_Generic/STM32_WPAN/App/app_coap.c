/* app_coap.c
 ******************************************************************************
 * CoAP application implementation
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdbool.h>
#include "app_coap.h"
#include "app_thread.h"
#include "log_module.h"
#include "app_bsp.h"
#include "stm32_lcd.h"
#include "stm32wba65i_discovery_bus.h"
#include "cmsis_os2.h"
#include "coap.h"
#include "thread.h"

/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private defines -----------------------------------------------------------*/
#define C_RESSOURCE             "light"

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private variables ---------------------------------------------------------*/
static otInstance     *sOtInstance = NULL;

/* CoAP resources*/
static void APP_COAP_RequestHandler(void                * pContext,
                                    otMessage           * pMessage,
                                    const otMessageInfo * pMessageInfo);

static otCoapResource sCoapResource =
{
  C_RESSOURCE,                 /* mUriPath    */
  APP_COAP_RequestHandler,     /* mHandler    */
  "myContext",                 /* mContext    */
  NULL                         /* mNext       */
};

static otMessageInfo sMessageInfo = {0};

/* Payload */
static uint8_t  sPayloadWrite[COAP_PAYLOAD_LENGTH] = {0};
static uint8_t  sReceivedCommand = 0;
static otMessage *sMessage = NULL;
static bool     sCoapPeriodicStarted = false;

/* RTOS objects -------------------------------------------------------------*/
static osTimerId_t    sCoapSendTimer = NULL;
static void           coapSendTimerCallback(void *arg);

static osSemaphoreId_t sSendCoapMsgSemaphore = NULL;
static void            Send_Coap_Msg_Task(void *argument);

static const osThreadAttr_t stSendCoapMsgTaskAttributes =
{
  .name       = "SendCoapMsg Task",
  .priority   = TASK_PRIO_SEND_COAP_MSG,
  .stack_size = TASK_SEND_COAP_MSG_STACK_SIZE
};

static osThreadId_t   sSendCoapMsgTaskId = NULL;

/* Private function prototypes ----------------------------------------------*/
static void APP_COAP_SendRequest(otCoapResource        *aCoapResource,
                                 otCoapType            aCoapType,
                                 otCoapCode            aCoapCode,
                                 const char           *aStringAddress,
                                 const otIp6Address   *aPeerAddress,
                                 uint8_t              *aPayload,
                                 uint16_t              size,
                                 otCoapResponseHandler aHandler,
                                 void                 *aContext);

static void APP_COAP_SendMulticastRequest(void);

/* Exported functions --------------------------------------------------------*/

void APP_COAP_Init(otInstance *aInstance)
{
  otError error = OT_ERROR_NONE;

  sOtInstance = aInstance;

  /* Start the COAP server */
  error = otCoapStart(sOtInstance, OT_DEFAULT_COAP_PORT);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_COAP_START, error);
  }

  /* Add COAP resources */
  otCoapAddResource(sOtInstance, &sCoapResource);

  /* Create semaphore to control send task */
  sSendCoapMsgSemaphore = osSemaphoreNew(1, 0, NULL);
  if (sSendCoapMsgSemaphore == NULL)
  {
    APP_DBG("ERROR FREERTOS : SEND COAP MESSAGES SEMAPHORE CREATION FAILED");
    while (1) { }
  }

  /* Create thread for multicast request CoAp msg transmission */
  sSendCoapMsgTaskId = osThreadNew(Send_Coap_Msg_Task, NULL, &stSendCoapMsgTaskAttributes);
  if (sSendCoapMsgTaskId == NULL)
  {
    APP_DBG("ERROR FREERTOS : SEND COAP MESSAGES TASK CREATION FAILED");
    while (1) { }
  }

  /* Create timer to handle COAP request sending */
  sCoapSendTimer = osTimerNew(coapSendTimerCallback, osTimerPeriodic, NULL, NULL);
}

void APP_COAP_OnThreadRoleChanged(otDeviceRole aRole)
{
  if (aRole == OT_DEVICE_ROLE_DETACHED)
  {
    if (sCoapPeriodicStarted == true && sCoapSendTimer != NULL)
    {
      osTimerStop(sCoapSendTimer);
      sCoapPeriodicStarted = false;
    }
  }
}

void APP_COAP_StartPeriodic(void)
{
  if ((sCoapPeriodicStarted == false) && (sCoapSendTimer != NULL))
  {
    osTimerStart(sCoapSendTimer, pdMS_TO_TICKS(COAP_SEND_TIMEOUT));
    sCoapPeriodicStarted = true;
  }
}

void APP_COAP_StopPeriodic(void)
{
  if ((sCoapPeriodicStarted == true) && (sCoapSendTimer != NULL))
  {
    osTimerStop(sCoapSendTimer);
    sCoapPeriodicStarted = false;
  }
}

bool APP_COAP_IsPeriodicRunning(void)
{
  return sCoapPeriodicStarted;
}

uint8_t APP_COAP_GetLastReceivedCommand(void)
{
  return sReceivedCommand;
}

/* Private functions ---------------------------------------------------------*/

static void APP_COAP_SendRequest(otCoapResource        *aCoapResource,
                                 otCoapType            aCoapType,
                                 otCoapCode            aCoapCode,
                                 const char           *aStringAddress,
                                 const otIp6Address   *aPeerAddress,
                                 uint8_t              *aPayload,
                                 uint16_t              size,
                                 otCoapResponseHandler aHandler,
                                 void                 *aContext)
{
  otError error = OT_ERROR_NONE;

  do
  {
    sMessage = otCoapNewMessage(sOtInstance, NULL);
    if (sMessage == NULL)
    {
      APP_THREAD_Error(ERR_THREAD_COAP_NEW_MSG, error);
      break;
    }

    otCoapMessageInit(sMessage, aCoapType, aCoapCode);
    (void)otCoapMessageAppendUriPathOptions(sMessage, aCoapResource->mUriPath);
    (void)otCoapMessageSetPayloadMarker(sMessage);

    if ((aPayload != NULL) && (size > 0))
    {
      error = otMessageAppend(sMessage, aPayload, size);
      if (error != OT_ERROR_NONE)
      {
        APP_THREAD_Error(ERR_THREAD_COAP_APPEND, error);
        break;
      }
    }
    else
    {
      LOG_ERROR_APP("APP_COAP_SendRequest: No payload passed");
    }

    memset(&sMessageInfo, 0, sizeof(sMessageInfo));
    memcpy(&sMessageInfo.mSockAddr,
           otThreadGetLinkLocalIp6Address(sOtInstance),
           sizeof(otIp6Address));
    sMessageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;

    if ((aPeerAddress == NULL) && (aStringAddress != NULL))
    {
      LOG_INFO_APP("Use String Address : %s", aStringAddress);
      (void)otIp6AddressFromString(aStringAddress, &sMessageInfo.mPeerAddr);
    }
    else
    {
      if (aPeerAddress != NULL)
      {
        LOG_INFO_APP("Use Peer Address");
        memcpy(&sMessageInfo.mPeerAddr, aPeerAddress, sizeof(sMessageInfo.mPeerAddr));
      }
      else
      {
        LOG_ERROR_APP("ERROR: Address string and Peer Address not defined");
        APP_THREAD_Error(ERR_THREAD_COAP_ADDRESS_NOT_DEFINED, 0);
      }
    }

    if (aCoapType == OT_COAP_TYPE_NON_CONFIRMABLE)
    {
      LOG_INFO_APP("\r\naCoapType == OT_COAP_TYPE_NON_CONFIRMABLE");
      error = otCoapSendRequest(sOtInstance, sMessage, &sMessageInfo, NULL, NULL);
    }
    else if (aCoapType == OT_COAP_TYPE_CONFIRMABLE)
    {
      LOG_INFO_APP("\r\naCoapType == OT_COAP_TYPE_CONFIRMABLE");
      error = otCoapSendRequest(sOtInstance, sMessage, &sMessageInfo, aHandler, aContext);
    }
  }
  while (false);

  if ((error != OT_ERROR_NONE) && (sMessage != NULL))
  {
    otMessageFree(sMessage);
    APP_THREAD_Error(ERR_THREAD_COAP_SEND_REQUEST, error);
  }
}

/**
 * @brief   Background Task for Send Coap Messages.
 */
static void Send_Coap_Msg_Task(void *argument)
{
  UNUSED(argument);

  while (1)
  {
    /* Wait for task semaphore to be released */
    osSemaphoreAcquire(sSendCoapMsgSemaphore, osWaitForever);

    APP_COAP_SendMulticastRequest();
    osThreadYield();
  }
}

static void coapSendTimerCallback(void *arg)
{
  (void)arg;
  osSemaphoreRelease(sSendCoapMsgSemaphore);
  return;
}

/**
  * @brief Send a COAP multicast request to all the devices which are connected
  *        on the Thread network
  */
static void APP_COAP_SendMulticastRequest(void)
{
  sPayloadWrite[0] = 0xff;
  sPayloadWrite[1] = 0xff;

  APP_COAP_SendRequest(&sCoapResource,
                       OT_COAP_TYPE_NON_CONFIRMABLE,
                       OT_COAP_CODE_PUT,
                       MULTICAST_FTD_MED,
                       NULL,
                       &sPayloadWrite[0],
                       sizeof(sPayloadWrite),
                       NULL,
                       NULL);
}

/**
 * @brief Handler called when the server receives a COAP request.
 */
static void APP_COAP_RequestHandler(void                 * pContext,
                                    otMessage            * pMessage,
                                    const otMessageInfo  * pMessageInfo)
{
  LOG_INFO_APP("Received CoAP request (context = %s)", (const char *)pContext);

  do
  {
    if (otCoapMessageGetType(pMessage) != OT_COAP_TYPE_NON_CONFIRMABLE)
    {
      break;
    }

    if (otCoapMessageGetCode(pMessage) != OT_COAP_CODE_PUT)
    {
      break;
    }

    if (otMessageRead(pMessage,
                      otMessageGetOffset(pMessage),
                      &sReceivedCommand,
                      1U) != 1U)
    {
      APP_THREAD_Error(ERR_THREAD_MESSAGE_READ, 0);
    }
  }
  while (false);
}


#if (CFG_JOYSTICK_SUPPORTED == 1)
static void APP_COAP_JoystickTogglePeriodicWithLabel(const char *directionString)
{
  if (APP_COAP_IsPeriodicRunning() == false)
  {
    APP_COAP_StartPeriodic();
#if (CFG_LCD_SUPPORTED == 1)
    BSP_SPI3_Init();
    UTIL_LCD_DisplayStringAt(0, 0, (uint8_t *)"periodic coap", CENTER_MODE);
    UTIL_LCD_DisplayStringAt(0, 20, (uint8_t *)"start", CENTER_MODE);
#endif
  }
  else
  {
    APP_COAP_StopPeriodic();
#if (CFG_LCD_SUPPORTED == 1)
    BSP_SPI3_Init();
    UTIL_LCD_DisplayStringAt(0, 0, (uint8_t *)"periodic coap", CENTER_MODE);
    UTIL_LCD_DisplayStringAt(0, 20, (uint8_t *)"stop", CENTER_MODE);
#endif
  }

#if (CFG_LCD_SUPPORTED == 1)
  UTIL_LCD_DisplayStringAt(0, 40, (uint8_t *)directionString, CENTER_MODE);
  BSP_LCD_Refresh(0);
  osDelay(100);
  UTIL_LCD_Clear(LCD_COLOR_BLACK);
  BSP_LCD_Refresh(0);
  BSP_SPI3_DeInit();
#endif
}

void APP_BSP_JoystickDownAction(void)
{
  APP_COAP_JoystickTogglePeriodicWithLabel("Joystick down");
}

/**
 * @brief  Management of the Joystick Right button
 */
void APP_BSP_JoystickRightAction(void)
{
  APP_COAP_JoystickTogglePeriodicWithLabel("Joystick right");
}

/**
 * @brief  Management of the Joystick Up button
 */
void APP_BSP_JoystickUpAction(void)
{
  APP_COAP_JoystickTogglePeriodicWithLabel("Joystick up");
}

/**

 * @brief  Management of the Joystick Left button : Triggers ble adv
 * @param  None
 * @retval None
 */
void APP_BSP_JoystickLeftAction(void)
{
  uint16_t adv_cmd = 0;
  osMessageQueuePut(advertisingCmdQueueHandle, &adv_cmd, 0U, 0U);
#if (CFG_LCD_SUPPORTED == 1)
  BSP_SPI3_Init();
  UTIL_LCD_DisplayStringAt(0, 0, (uint8_t *)"BLE adv start", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 40, (uint8_t *)"Joystick left", CENTER_MODE);
  BSP_LCD_Refresh(0);
  osDelay(100);
  UTIL_LCD_Clear( LCD_COLOR_BLACK );
  BSP_LCD_Refresh(0);
  BSP_SPI3_DeInit();
#endif
}

/**

 * @brief  Management of the Joystick Select button : Stops ble adv
 * @param  None
 * @retval None
 */
void APP_BSP_JoystickSelectAction(void)
{
  uint16_t adv_cmd = 2;
  osMessageQueuePut(advertisingCmdQueueHandle, &adv_cmd, 0U, 0U);
#if (CFG_LCD_SUPPORTED == 1)
  BSP_SPI3_Init();
  UTIL_LCD_DisplayStringAt(0, 0, (uint8_t *)"BLE adv stop", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 40, (uint8_t *)"Joystick select", CENTER_MODE);
  BSP_LCD_Refresh(0);
  osDelay(100);
  UTIL_LCD_Clear( LCD_COLOR_BLACK );
  BSP_LCD_Refresh(0);
  BSP_SPI3_DeInit();
#endif
}

#endif /* CFG_JOYSTICK_SUPPORTED == 1 */