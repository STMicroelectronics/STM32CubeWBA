/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * File Name          : app_thread.c
  * Description        : Thread Application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include <assert.h>
#include <stdint.h>

#include "app_conf.h"
#include "app_common.h"
#include "app_entry.h"
#include "log_module.h"
#include "app_thread.h"
#include "dbg_trace.h"
#include "stm32_rtos.h"
#include "stm32_timer.h"
#if (CFG_LPM_LEVEL != 0)
#include "stm32_lpm.h"
#endif // CFG_LPM_LEVEL
#include "common_types.h"
#include "instance.h"
#include "radio.h"
#include "platform.h"
#include "ll_sys_startup.h"
#include "event_manager.h"
#include "platform_wba.h"
#include "link.h"
#include "cli.h"
#include "coap.h"
#include "tasklet.h"
#include "thread.h"
#if (OT_CLI_USE == 1)
#include "uart.h"
#endif
#include "joiner.h"
#include "alarm.h"
#include OPENTHREAD_CONFIG_FILE

/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_bsp.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/
#define C_PANID                 0xBA98U
#define C_CHANNEL_NB            16U
#define C_CCA_THRESHOLD         (-70)

/* USER CODE BEGIN PD */
#define C_RESSOURCE                   "light"
#define COAP_PAYLOAD_LENGTH           (2U)
/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private function prototypes -----------------------------------------------*/
static void APP_THREAD_DeviceConfig(void);
static void APP_THREAD_StateNotif(uint32_t NotifFlags, void *pContext);
static void APP_THREAD_TraceError(const char * pMess, uint32_t ErrCode);

#if (OT_CLI_USE == 1)
static void APP_THREAD_CliInit(otInstance *aInstance);
static void APP_THREAD_ProcessUart(ULONG lArgument);
#endif // OT_CLI_USE

static void APP_THREAD_PersistenceStartup(void);

/* USER CODE BEGIN PFP */
static void APP_THREAD_AppInit(void);
static void APP_THREAD_CoapSendRequest( otCoapResource        * aCoapRessource,
                                        otCoapType            aCoapType,
                                        otCoapCode            aCoapCode,
                                        const char            * aStringAddress,
                                        const otIp6Address    * aPeerAddress,
                                        uint8_t               * aPayload,
                                        uint16_t              Size,
                                        otCoapResponseHandler aHandler,
                                        void                  * aContext);

static void APP_THREAD_CoapRequestHandler(  void                * pContext,
                                            otMessage           * pMessage,
                                            const otMessageInfo * pMessageInfo);

static void APP_THREAD_CoapSendDataResponse(otMessage           * pMessage,
                                            const otMessageInfo * pMessageInfo);

static void APP_THREAD_InitPayloadWrite(void);
static bool APP_THREAD_CheckMsgValidity(void);
static void APP_THREAD_CoapDataRespHandler( void                * aContext,
                                            otMessage           * pMessage,
                                            const otMessageInfo * pMessageInfo,
                                            otError             result);
/* USER CODE END PFP */

/* Private variables -----------------------------------------------*/
static otInstance * PtOpenThreadInstance;

TX_THREAD           AlarmTask, TaskletsTask;
TX_SEMAPHORE        AlarmSemaphore, TaskletSemaphore;
TX_THREAD           AlarmUsTask;
TX_SEMAPHORE        AlarmUsSemaphore;
#if (OT_CLI_USE == 1)
TX_THREAD           CliUartTask;
TX_SEMAPHORE        CliUartSemaphore;
#endif // OT_CLI_USE
/* USER CODE BEGIN PV */
static otCoapResource OT_Ressource = {C_RESSOURCE, APP_THREAD_CoapRequestHandler, "MyOwnContext", NULL};
static otMessageInfo OT_MessageInfo = {0};
static otMessage* pOT_Message = NULL;
static otMessage* pOT_MessageResponse = NULL;

static uint8_t PayloadWrite[COAP_PAYLOAD_LENGTH]= {0};
static uint8_t PayloadRead[COAP_PAYLOAD_LENGTH]= {0};
/* USER CODE END PV */

/* Functions Definition ------------------------------------------------------*/

static void APP_THREAD_ProcessAlarm(ULONG lArgument)
{
  UNUSED(lArgument);

  for(;;)
  {
    tx_semaphore_get(&AlarmSemaphore, TX_WAIT_FOREVER);
    arcAlarmProcess(PtOpenThreadInstance);
  }
}

static void APP_THREAD_ProcessUsAlarm(ULONG lArgument)
{
  UNUSED(lArgument);

  for(;;)
  {
    tx_semaphore_get(&AlarmUsSemaphore, TX_WAIT_FOREVER);
    arcUsAlarmProcess(PtOpenThreadInstance);
  }
}

/**
 * @brief  APP_THREAD_ProcessOpenThreadTasklets.
 * @param  ULONG lArgument (unused)
 * @param  None
 * @retval None
 */
static void APP_THREAD_ProcessOpenThreadTasklets(ULONG lArgument)
{
  UNUSED(lArgument);

  for(;;)
  {
    tx_semaphore_get(&TaskletSemaphore, TX_WAIT_FOREVER);

    /* wakeUp the system */
    //ll_sys_radio_hclk_ctrl_req(LL_SYS_RADIO_HCLK_LL_BG, LL_SYS_RADIO_HCLK_ON);
    //ll_sys_dp_slp_exit();

    /* process the tasklet */
    otTaskletsProcess(PtOpenThreadInstance);

    /* put the IP802_15_4 back to sleep mode */
    //ll_sys_radio_hclk_ctrl_req(LL_SYS_RADIO_HCLK_LL_BG, LL_SYS_RADIO_HCLK_OFF);

  }
}

/**
 * OpenThread calls this function when the tasklet queue transitions from empty to non-empty.
 *
 * @param[in] aInstance A pointer to an OpenThread instance.
 */
void otTaskletsSignalPending(otInstance *aInstance)
{
  tx_semaphore_put(&TaskletSemaphore);
}

void APP_THREAD_ScheduleAlarm(void)
{
  if (AlarmSemaphore.tx_semaphore_count == 0)
  {
    tx_semaphore_put(&AlarmSemaphore);
  }
}

void APP_THREAD_ScheduleUsAlarm(void)
{
  if (AlarmUsSemaphore.tx_semaphore_count == 0)
  {
    tx_semaphore_put(&AlarmUsSemaphore);
  }
}

static void APP_THREAD_AlarmsInit(void)
{
  UINT ThreadXStatus;
  CHAR *pStack;

  /* Register semaphores to launch tasks */
  ThreadXStatus = tx_semaphore_create(&AlarmSemaphore, "AlarmSemaphore", 0);

  if (ThreadXStatus == TX_SUCCESS)
  {
    ThreadXStatus = tx_semaphore_create(&AlarmUsSemaphore, "AlarmUsSemaphore", 0);
  }

  /* Create associated tasks */
  if (ThreadXStatus == TX_SUCCESS)
  {
    ThreadXStatus = tx_byte_allocate(pBytePool, (VOID**) &pStack, TASK_STACK_SIZE_ALARM, TX_NO_WAIT);
  }
  if (ThreadXStatus == TX_SUCCESS)
  {
    ThreadXStatus = tx_thread_create( &AlarmTask, "AlarmTask", APP_THREAD_ProcessAlarm, 0, pStack,
                                      TASK_STACK_SIZE_ALARM, TASK_PRIO_ALARM, TASK_PREEMP_ALARM,
                                      TX_NO_TIME_SLICE, TX_AUTO_START );
  }

  if (ThreadXStatus == TX_SUCCESS)
  {
    ThreadXStatus = tx_byte_allocate(pBytePool, (VOID**) &pStack, TASK_STACK_SIZE_ALARM_US, TX_NO_WAIT);
  }
  if (ThreadXStatus == TX_SUCCESS)
  {
    ThreadXStatus = tx_thread_create( &AlarmUsTask, "AlarmUsTask", APP_THREAD_ProcessUsAlarm, 0, pStack,
                                      TASK_STACK_SIZE_ALARM_US, TASK_PRIO_US_ALARM, TASK_PREEMP_US_ALARM,
                                      TX_NO_TIME_SLICE, TX_AUTO_START );
  }

  /* Verify if it's OK */
  if (ThreadXStatus != TX_SUCCESS)
  {
    LOG_ERROR_APP( "ERROR THREADX : ALARMS THREAD CREATION FAILED (%d)", ThreadXStatus );
    while(1);
  }
}

static void APP_THREAD_TaskletsInit(void)
{
  UINT ThreadXStatus;
  CHAR *pStack;

  /* Semaphore already created */

  /* Create associated task */
  ThreadXStatus = tx_byte_allocate(pBytePool, (VOID**) &pStack, TASK_STACK_SIZE_TASKLETS, TX_NO_WAIT);

  if (ThreadXStatus == TX_SUCCESS)
  {
    ThreadXStatus |= tx_thread_create(&TaskletsTask, "TaskletsTask", APP_THREAD_ProcessOpenThreadTasklets, 0, pStack,
                                      TASK_STACK_SIZE_TASKLETS, TASK_PRIO_TASKLETS, TASK_PREEMP_TASKLETS,
                                      TX_NO_TIME_SLICE, TX_AUTO_START);
  }

  /* Verify if it's OK */
  if (ThreadXStatus != TX_SUCCESS)
  {
    LOG_ERROR_APP( "ERROR THREADX : TASKLETS THREAD CREATION FAILED (%d)", ThreadXStatus );
    while(1);
  }
}

/**
 *
 */
void Thread_Init(void)
{
#if OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
  size_t otInstanceBufferLength = 0;
  uint8_t *otInstanceBuffer = NULL;
#endif // OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE

  APP_THREAD_PersistenceStartup();

  otSysInit(0, NULL);

 /* Semaphore used by otTaskletsSignalPending() that is called by otInstanceInit() */
  tx_semaphore_create(&TaskletSemaphore, "TaskletSemaphore", 0);

#if OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
  // Call to query the buffer size
  (void)otInstanceInit(NULL, &otInstanceBufferLength);

  // Call to allocate the buffer
  otInstanceBuffer = (uint8_t *)malloc(otInstanceBufferLength);
  assert(otInstanceBuffer);

  // Initialize OpenThread with the buffer
  PtOpenThreadInstance = otInstanceInit(otInstanceBuffer, &otInstanceBufferLength);
#else // OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
  PtOpenThreadInstance = otInstanceInitSingle();
#endif // OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE

  assert(PtOpenThreadInstance);

#if (OT_CLI_USE == 1)
  APP_THREAD_CliInit(PtOpenThreadInstance);
#endif // OT_CLI_USE
  otDispatch_tbl_init(PtOpenThreadInstance);

  /* Register tasks */
  APP_THREAD_AlarmsInit();
  APP_THREAD_TaskletsInit();

  ll_sys_thread_init();

  /* USER CODE BEGIN INIT TASKS */
  APP_THREAD_AppInit();
  /* USER CODE END INIT TASKS */
}

/**
 * @brief Thread initialization.
 * @param  None
 * @retval None
 */
static void APP_THREAD_DeviceConfig(void)
{
  otError error = OT_ERROR_NONE;
  otNetworkKey networkKey = {{0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00}};

  error = otSetStateChangedCallback(PtOpenThreadInstance, APP_THREAD_StateNotif, NULL);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_SET_STATE_CB,error);
  }

  error = otPlatRadioSetCcaEnergyDetectThreshold(PtOpenThreadInstance, C_CCA_THRESHOLD);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_SET_THRESHOLD,error);
  }

  error = otLinkSetChannel(PtOpenThreadInstance, C_CHANNEL_NB);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_SET_CHANNEL,error);
  }

  error = otLinkSetPanId(PtOpenThreadInstance, C_PANID);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_SET_PANID,error);
  }

  error = otThreadSetNetworkKey(PtOpenThreadInstance, &networkKey);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_SET_NETWORK_KEY,error);
  }

  otPlatRadioEnableSrcMatch(PtOpenThreadInstance, true);

  error = otIp6SetEnabled(PtOpenThreadInstance, true);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_IPV6_ENABLE,error);
  }
  error = otThreadSetEnabled(PtOpenThreadInstance, true);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_START,error);
  }
  /* USER CODE BEGIN DEVICECONFIG */
  /* Start the COAP server */
  error = otCoapStart(PtOpenThreadInstance, OT_DEFAULT_COAP_PORT);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_COAP_START,error);
  }

  /* Add COAP resources */
  otCoapAddResource(PtOpenThreadInstance, &OT_Ressource);

  APP_THREAD_InitPayloadWrite();

  /* USER CODE END DEVICECONFIG */
}

void APP_THREAD_Init( void )
{
#if (CFG_LPM_LEVEL != 0)
  UTIL_LPM_SetStopMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);
  UTIL_LPM_SetOffMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);
#endif // CFG_LPM_LEVEL

  Thread_Init();

  APP_THREAD_DeviceConfig();
}

/**
 * @brief  Warn the user that an error has occurred.
 *
 * @param  pMess  : Message associated to the error.
 * @param  ErrCode: Error code associated to the module (OpenThread or other module if any)
 * @retval None
 */
static void APP_THREAD_TraceError(const char * pMess, uint32_t ErrCode)
{
  /* USER CODE BEGIN TRACE_ERROR */
  LOG_ERROR_APP("**** FATAL ERROR = %s (Err = %d)", pMess, ErrCode);
  /* In this case, the LEDs on the Board will start blinking. */

  /* HAL_Delay() requires TIM2 interrupts to work 
     During ThreadX initialization, all interrupts are disabled
     As this function may be called during this phase, 
     interrupts need to be re-enabled to make LEDs blinking    
  */
  if (__get_PRIMASK())
  {
    __enable_irq();
  }
  
  while (1)
  {
    APP_LED_TOGGLE(LD1);
    HAL_Delay(500U);
    APP_LED_TOGGLE(LD2);
    HAL_Delay(500U);
    APP_LED_TOGGLE(LD3);
    HAL_Delay(500U);
  }

  /* USER CODE END TRACE_ERROR */
}

/**
 * @brief  Trace the error or the warning reported.
 * @param  ErrId :
 * @param  ErrCode
 * @retval None
 */
void APP_THREAD_Error(uint32_t ErrId, uint32_t ErrCode)
{
  /* USER CODE BEGIN APP_THREAD_Error_1 */

  /* USER CODE END APP_THREAD_Error_1 */

  switch(ErrId)
  {
    case ERR_THREAD_SET_STATE_CB :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_SET_STATE_CB ",ErrCode);
        break;

    case ERR_THREAD_SET_CHANNEL :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_SET_CHANNEL ",ErrCode);
        break;

    case ERR_THREAD_SET_PANID :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_SET_PANID ",ErrCode);
        break;

    case ERR_THREAD_IPV6_ENABLE :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_IPV6_ENABLE ",ErrCode);
        break;

    case ERR_THREAD_START :
        APP_THREAD_TraceError("ERROR: ERR_THREAD_START ", ErrCode);
        break;

    case ERR_THREAD_ERASE_PERSISTENT_INFO :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_ERASE_PERSISTENT_INFO ",ErrCode);
        break;

    case ERR_THREAD_SET_NETWORK_KEY :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_SET_NETWORK_KEY ",ErrCode);
        break;

    case ERR_THREAD_CHECK_WIRELESS :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_CHECK_WIRELESS ",ErrCode);
        break;

    case ERR_THREAD_SET_THRESHOLD:
        APP_THREAD_TraceError("ERROR : ERR_THREAD_SET_THRESHOLD", ErrCode);
        break;

    /* USER CODE BEGIN APP_THREAD_Error_2 */
    case ERR_THREAD_COAP_START :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_COAP_START ",ErrCode);
        break;

    case ERR_THREAD_COAP_ADD_RESSOURCE :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_COAP_ADD_RESSOURCE ",ErrCode);
        break;

    case ERR_THREAD_MESSAGE_READ :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_MESSAGE_READ ",ErrCode);
        break;

    case ERR_THREAD_COAP_SEND_RESPONSE :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_COAP_SEND_RESPONSE ",ErrCode);
        break;

    case ERR_THREAD_COAP_APPEND :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_COAP_APPEND ",ErrCode);
        break;

    case ERR_THREAD_COAP_SEND_REQUEST :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_COAP_SEND_REQUEST ",ErrCode);
        break;

    case ERR_THREAD_MSG_COMPARE_FAILED:
        APP_THREAD_TraceError("ERROR : ERR_THREAD_MSG_COMPARE_FAILED ",ErrCode);
        break;
        
    /* USER CODE END APP_THREAD_Error_2 */
    default :
        APP_THREAD_TraceError("ERROR Unknown ", 0);
        break;
  }
}

/**
 * @brief Thread notification when the state changes.
 * @param  aFlags  : Define the item that has been modified
 *         aContext: Context
 *
 * @retval None
 */
static void APP_THREAD_StateNotif(uint32_t NotifFlags, void *pContext)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(pContext);

  /* USER CODE BEGIN APP_THREAD_STATENOTIF */

  /* USER CODE END APP_THREAD_STATENOTIF */

  if ((NotifFlags & (uint32_t)OT_CHANGED_THREAD_ROLE) == (uint32_t)OT_CHANGED_THREAD_ROLE)
  {
    switch (otThreadGetDeviceRole(PtOpenThreadInstance))
    {
      case OT_DEVICE_ROLE_DISABLED:
          /* USER CODE BEGIN OT_DEVICE_ROLE_DISABLED */
      	  APP_LED_OFF(LD2);
          APP_LED_OFF(LD3);
          /* USER CODE END OT_DEVICE_ROLE_DISABLED */
          break;

      case OT_DEVICE_ROLE_DETACHED:
          /* USER CODE BEGIN OT_DEVICE_ROLE_DETACHED */
          APP_LED_OFF(LD2);
          APP_LED_OFF(LD3);
          /* USER CODE END OT_DEVICE_ROLE_DETACHED */
          break;

      case OT_DEVICE_ROLE_CHILD:
          /* USER CODE BEGIN OT_DEVICE_ROLE_CHILD */
          APP_LED_OFF(LD2);
          APP_LED_ON(LD3);
          /* USER CODE END OT_DEVICE_ROLE_CHILD */
          break;

      case OT_DEVICE_ROLE_ROUTER :
          /* USER CODE BEGIN OT_DEVICE_ROLE_ROUTER */
          APP_LED_OFF(LD2);
          APP_LED_ON(LD3);
          /* USER CODE END OT_DEVICE_ROLE_ROUTER */
          break;

      case OT_DEVICE_ROLE_LEADER :
          /* USER CODE BEGIN OT_DEVICE_ROLE_LEADER */
          APP_LED_ON(LD2);
          APP_LED_OFF(LD3);
          /* USER CODE END OT_DEVICE_ROLE_LEADER */
          break;

      default:
          /* USER CODE BEGIN DEFAULT */
          APP_LED_OFF(LD2);
          APP_LED_OFF(LD3);
          /* USER CODE END DEFAULT */
          break;
    }
  }
}

#if (OT_CLI_USE == 1)
/* OT CLI UART functions */
static void APP_THREAD_ProcessUart(ULONG lArgument)
{
  UNUSED(lArgument);

  for(;;)
  {
    tx_semaphore_get(&CliUartSemaphore, TX_WAIT_FOREVER);
    arcUartProcess();
  }
}

void APP_THREAD_ScheduleUART(void)
{
  tx_semaphore_put(&CliUartSemaphore);
}

static void APP_THREAD_CliInit(otInstance *aInstance)
{
  UINT ThreadXStatus;
  CHAR *pStack;

  /* Register semaphore to launch task */
  ThreadXStatus = tx_semaphore_create(&CliUartSemaphore, "CliUartSemaphore", 0);

  /* Create CLI UART thread with this stack */
  if (ThreadXStatus == TX_SUCCESS)
  {
    ThreadXStatus |= tx_byte_allocate(pBytePool, (VOID**) &pStack, TASK_CLI_UART_STACK_SIZE, TX_NO_WAIT);
  }
  if (ThreadXStatus == TX_SUCCESS)
  {
    ThreadXStatus |= tx_thread_create(&CliUartTask, "CliUartTask", APP_THREAD_ProcessUart, 0, pStack,
                                      TASK_CLI_UART_STACK_SIZE, TASK_PRIO_CLI_UART, TASK_PREEMP_CLI_UART,
                                      TX_NO_TIME_SLICE, TX_AUTO_START);
  }

  /* Verify if it's OK */
  if (ThreadXStatus != TX_SUCCESS)
  {
    LOG_ERROR_APP( "ERROR THREADX : CLI UART THREAD CREATION FAILED (%d)", ThreadXStatus );
    while(1);
  }

  (void)otPlatUartEnable();
  otCliInit(aInstance, CliUartOutput, aInstance);
}
#endif /* OT_CLI_USE */

/**
 * @brief  Thread persistence startup
 * @param  None
 * @retval None
 */
static void APP_THREAD_PersistenceStartup(void)
{
  /* USER CODE BEGIN APP_THREAD_PersistenceStartup */

  /* USER CODE END APP_THREAD_PersistenceStartup */
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */
/**
 * @brief Send a CoAP request with defined parameters.
 *
 * @param[in]  aCoapRessource   A pointer to a otCoapResource.
 * @param[in]  aCoapType        otCoapType.
 * @param[in]  aCoapCode        otCoapCode.
 * @param[in]  aStringAddress   A pointer to a NULL-terminated string representing the address. Example: "FF03::1" for Multicast.
 * @param[in]  aPeerAddress     A pointer to otIp6Address Peer Address.
 * @param[in]  aPayload         A pointer to payload.
 * @param[in]  aHandler         A pointer to CoAP response handler.
 * @param[in]  aContext         A pointer to application specific context.
 *
 * @retval none.
 */
static void APP_THREAD_CoapSendRequest( otCoapResource        * aCoapRessource,
                                        otCoapType            aCoapType,
                                        otCoapCode            aCoapCode,
                                        const char            * aStringAddress,
                                        const otIp6Address    * aPeerAddress,
                                        uint8_t               * aPayload,
                                        uint16_t              Size,
                                        otCoapResponseHandler aHandler,
                                        void                  * aContext)
{
  otError error = OT_ERROR_NONE;

  do
  {
    pOT_Message = otCoapNewMessage(PtOpenThreadInstance, NULL);
    if (pOT_Message == NULL)
    {
      APP_THREAD_Error(ERR_THREAD_COAP_NEW_MSG,error);
      break;
    }

    otCoapMessageInit(pOT_Message, aCoapType, aCoapCode);
    (void)otCoapMessageAppendUriPathOptions(pOT_Message, aCoapRessource->mUriPath);
    (void)otCoapMessageSetPayloadMarker(pOT_Message);

    if((aPayload != NULL) && (Size > 0))
    {
      error = otMessageAppend(pOT_Message, aPayload, Size);
      if (error != OT_ERROR_NONE)
      {
        APP_THREAD_Error(ERR_THREAD_COAP_APPEND,error);
        break;
      }
    }
    else
    {
      LOG_ERROR_APP("APP_THREAD_CoapSendRequest: No payload passed");
    }

    memset(&OT_MessageInfo, 0, sizeof(OT_MessageInfo));
    OT_MessageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;

    if((aPeerAddress == NULL) && (aStringAddress != NULL))
    {
      LOG_INFO_APP("Use String Address : %s", aStringAddress);
      (void)otIp6AddressFromString(aStringAddress, &OT_MessageInfo.mPeerAddr);
    }
    else
    {
      if (aPeerAddress != NULL)
      {
        LOG_INFO_APP("Use Peer Address");
        memcpy(&OT_MessageInfo.mPeerAddr, aPeerAddress, sizeof(OT_MessageInfo.mPeerAddr));
      }
      else
      {
        LOG_ERROR_APP("ERROR: Address string and Peer Address not defined");
        APP_THREAD_Error(ERR_THREAD_COAP_ADDRESS_NOT_DEFINED, 0);
      }
    }
  
    if(aCoapType == OT_COAP_TYPE_NON_CONFIRMABLE)
    {
      LOG_INFO_APP("\r\naCoapType == OT_COAP_TYPE_NON_CONFIRMABLE");
      error = otCoapSendRequest(PtOpenThreadInstance, pOT_Message, &OT_MessageInfo, NULL, NULL);
    }
    
    if(aCoapType == OT_COAP_TYPE_CONFIRMABLE)
    {
      LOG_INFO_APP("\r\naCoapType == OT_COAP_TYPE_CONFIRMABLE");
      error = otCoapSendRequest(PtOpenThreadInstance, pOT_Message, &OT_MessageInfo, aHandler, aContext);
    }
  }
  while(false);
  
  if (error != OT_ERROR_NONE && pOT_Message != NULL)
  {
    otMessageFree(pOT_Message);
    APP_THREAD_Error(ERR_THREAD_COAP_SEND_REQUEST,error);
  }
}

/**
 * @brief Handler called when the server receives a COAP request.
 *
 * @param pContext : Context
 * @param pHeader : Header
 * @param pMessage : Message
 * @param pMessageInfo : Message information
 * @retval None
 */
static void APP_THREAD_CoapRequestHandler(void                 * pContext,
                                          otMessage            * pMessage,
                                          const otMessageInfo  * pMessageInfo)

{
  LOG_INFO_APP("Received CoAP request (context = %s)", pContext);
  
  if (otMessageRead(pMessage, otMessageGetOffset(pMessage), &PayloadRead, sizeof(PayloadRead)) != sizeof(PayloadRead))
  {
    APP_THREAD_Error(ERR_THREAD_MESSAGE_READ, 0);
  }

  if (APP_THREAD_CheckMsgValidity() == true)
  {
    APP_LED_TOGGLE(LD1);
  }
  
  /* If Message is Confirmable, send response */
  if (otCoapMessageGetType(pMessage) == OT_COAP_TYPE_CONFIRMABLE)
  {
    APP_THREAD_CoapSendDataResponse(pMessage, pMessageInfo);
  }
}

/**
 * @brief This function acknowledges the data reception by sending an ACK
 *        back to the sender.
 * @param  pMessage coap message
 * @param  pMessageInfo message info pointer
 * @retval None
 */
static void APP_THREAD_CoapSendDataResponse(otMessage  * pMessage, const otMessageInfo * pMessageInfo)
{
  otError  error = OT_ERROR_NONE;

  do
  {
    LOG_INFO_APP("APP_THREAD_CoapSendDataResponse");

    pOT_MessageResponse = otCoapNewMessage(PtOpenThreadInstance, NULL);
    if (pOT_MessageResponse == NULL)
    {
      LOG_WARNING_APP("WARNING : pOT_MessageResponse = NULL ! -> exit now");
      break;
    }

    (void)otCoapMessageInitResponse(pOT_MessageResponse, pMessage, OT_COAP_TYPE_ACKNOWLEDGMENT, OT_COAP_CODE_VALID);

    error = otCoapSendResponse(PtOpenThreadInstance, pOT_MessageResponse, pMessageInfo);
    if (error != OT_ERROR_NONE && pOT_MessageResponse != NULL)
    {
      otMessageFree(pOT_MessageResponse);
      APP_THREAD_Error(ERR_THREAD_COAP_DATA_RESPONSE,error);
    }
  }
  while(false);
}

/**
 * @brief Initialize CoAP write buffer.
 * @param  None
 * @retval None
 */
static void APP_THREAD_InitPayloadWrite(void)
{
  uint8_t i;
  
  for(i = 0; i < COAP_PAYLOAD_LENGTH; i++)
  {
    PayloadWrite[i] = 0xFF;
  }
}

/**
 * @brief  Compare the message received versus the original message.
 * @param  None
 * @retval None
 */
static bool APP_THREAD_CheckMsgValidity(void)
{
  bool valid = true;
  uint32_t i;

  for(i = 0; i < COAP_PAYLOAD_LENGTH; i++)
  {
    if(PayloadRead[i] != PayloadWrite[i])
    {
      valid = false;
    }
  }

  if(valid == true)
  {
    LOG_INFO_APP("PAYLOAD Comparison OK!");
  }
  else
  {
    APP_THREAD_Error(ERR_THREAD_MSG_COMPARE_FAILED, 0);
  }
  
  return valid;
}

/**
 * @brief Task associated to the push button 1.
 * @param  None
 * @retval None
 */
void APP_BSP_Button1Action(void)
{
  LOG_INFO_APP("Send a CoAP NON-CONFIRMABLE PUT Request");
  
  /* Send a NON-CONFIRMABLE PUT Request */
  APP_THREAD_CoapSendRequest(&OT_Ressource, OT_COAP_TYPE_NON_CONFIRMABLE, OT_COAP_CODE_PUT, MULTICAST_FTD_MED,
                              NULL, PayloadWrite, sizeof(PayloadWrite), NULL, NULL);

}


static void APP_THREAD_AppInit(void)
{
}

/**
 * @brief This function manages the data response handler.
 *
 * @param pHeader  context
 * @param pMessage message pointer
 * @param pMessageInfo message info pointer
 * @param result error code
 * @retval None
 */
static void APP_THREAD_CoapDataRespHandler( void                * pContext,
                                            otMessage           * pMessage,
                                            const otMessageInfo * pMessageInfo,
                                            otError               result)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(pMessage);
  UNUSED(pMessageInfo);

  if (result == OT_ERROR_NONE)
  {
    LOG_INFO_APP("APP_THREAD_CoapDataRespHandler : NO ERROR");
  }
  else
  {
    LOG_WARNING_APP("APP_THREAD_CoapDataRespHandler : WARNING. Result = %d",result);
  }
}

/**
 * @brief Task associated to the push button 2.
 * @param  None
 * @retval None
 */
void APP_BSP_Button2Action(void)
{
  LOG_INFO_APP("Send a CoAP CONFIRMABLE PUT Request");
  
  /* Send a CONFIRMABLE PUT Request */
  APP_THREAD_CoapSendRequest(&OT_Ressource, OT_COAP_TYPE_CONFIRMABLE, OT_COAP_CODE_PUT, MULTICAST_FTD_MED,
                             NULL, PayloadWrite, sizeof(PayloadWrite), APP_THREAD_CoapDataRespHandler, NULL);
}



/* USER CODE END FD_LOCAL_FUNCTIONS */

