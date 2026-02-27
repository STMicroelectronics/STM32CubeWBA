/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * File Name          : app_thread.c
  * Description        : Thread Application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32_lpm_if.h"

/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_bsp.h"
#include "stm32_lcd.h"
#include "stm32wba65i_discovery_bus.h"
#include "app_coap.h"
#ifdef OT_CLI_USE
#include "uart.h"
#endif /* OT_CLI_USE */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/
#define C_PANID                 0xBA98U
#define C_CHANNEL_NB            16U
#define WAIT_TIMEOUT           (5000U)    /**< 5s */
#define C_CCA_THRESHOLD         (-70)

/* USER CODE BEGIN PD */
#define THREAD_LINK_POLL_PERIOD_MS (5000) /**< 5s */
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
void APP_THREAD_ProcessUart(void *argument);
#endif // OT_CLI_USE

static void APP_THREAD_PersistenceStartup(void);

/* USER CODE BEGIN PFP */


/* USER CODE END PFP */

/* Private variables -----------------------------------------------*/
static otInstance * PtOpenThreadInstance;

/* USER CODE BEGIN PV */


/* USER CODE END PV */

/* Functions Definition ------------------------------------------------------*/

void APP_THREAD_ProcessAlarm(void *argument)
{
  UNUSED(argument);

  arcAlarmProcess(PtOpenThreadInstance);
}

void APP_THREAD_ProcessUsAlarm(void *argument)
{
  UNUSED(argument);

  arcUsAlarmProcess(PtOpenThreadInstance);
}

/**
 * @brief  APP_THREAD_ProcessOpenThreadTasklets.
 * @param  ULONG lArgument (unused)
 * @param  None
 * @retval None
 */
void APP_THREAD_ProcessOpenThreadTasklets(void *argument)
{
  UNUSED(argument);

  /* process the tasklet */
  otTaskletsProcess(PtOpenThreadInstance);
}

/**
 * OpenThread calls this function when the tasklet queue transitions from empty to non-empty.
 *
 * @param[in] aInstance A pointer to an OpenThread instance.
 */
void otTaskletsSignalPending(otInstance *aInstance)
{
  UNUSED(aInstance);

  osThreadFlagsSet(WpanTaskHandle, 1U << CFG_RTOS_FLAG_OT_Tasklet);
}

void APP_THREAD_ScheduleAlarm(void)
{
  osThreadFlagsSet(WpanTaskHandle, 1U << CFG_RTOS_FLAG_OT_Alarm_ms);
}

void APP_THREAD_ScheduleUsAlarm(void)
{
  osThreadFlagsSet(WpanTaskHandle, 1U << CFG_RTOS_FLAG_OT_Alarm_us);
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

  ll_sys_thread_init();

  /* USER CODE BEGIN INIT TASKS */
  
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
  otLinkModeConfig OT_LinkMode;
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

    /* After reaching the child or router state, the system
   *   a) sets the 'sleepy end device' mode
   *   b) perform a Thread stop
   *   c) perform a Thread start.
   *
   *  NOTE : According to the Thread specification, it is necessary to set the
   *         mode before starting Thread.
   *
   * A Child that has attached to its Parent indicating it is an FTD MUST NOT use Child UpdateRequest
   * to modify its mode to MTD.
   * As a result, you need to first detach from the network before switching from FTD to MTD at runtime,
   * then reattach.
   *
   */
  /* Set the pool period . It means that when the device will enter in 'sleepy
   * end device' mode, it will send an ACK_Request every
   * WAIT_TIMEOUT.
   * This message will act as keep alive message.
   */
  /* Set the sleepy end device mode */
  OT_LinkMode.mRxOnWhenIdle = 0;
  OT_LinkMode.mDeviceType = 0; /* 0: MTD, 1: FTD */
  OT_LinkMode.mNetworkData = 1U;

  error = otThreadSetLinkMode(PtOpenThreadInstance,OT_LinkMode);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_LINK_MODE,error);
  }
  error = otLinkSetPollPeriod(PtOpenThreadInstance, WAIT_TIMEOUT);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_POLL_MODE,error);
  }
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
  APP_COAP_Init(PtOpenThreadInstance);

#if (CFG_LPM_LEVEL == 2)
  UTIL_LPM_SetMaxMode(1 << CFG_LPM_APP, UTIL_LPM_MAX_MODE);
#endif // CFG_LPM_LEVEL
#if (CFG_LPM_LEVEL == 1)
  UTIL_LPM_SetMaxMode(1 << CFG_LPM_APP, UTIL_LPM_STOP1_MODE);
#endif // CFG_LPM_LEVEL
  /* USER CODE END DEVICECONFIG */
}

void APP_THREAD_Init( void )
{
#if (CFG_LPM_LEVEL != 0)
  UTIL_LPM_SetMaxMode(1 << CFG_LPM_APP, UTIL_LPM_SLEEP_MODE);
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

  while (1)
  {
#if CFG_LED_SUPPORTED 
    APP_BSP_LED_Toggle(LED_GREEN);
    HAL_Delay(500U);
    APP_BSP_LED_Toggle(LED_RED);
    HAL_Delay(500U);
#endif
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
#if (CFG_LED_SUPPORTED == 1)
      	  APP_BSP_LED_Off(LED_GREEN);
          APP_BSP_LED_Off(LED_RED);
#endif
          /* USER CODE END OT_DEVICE_ROLE_DISABLED */
          break;

      case OT_DEVICE_ROLE_DETACHED:
          /* USER CODE BEGIN OT_DEVICE_ROLE_DETACHED */
#if (CFG_LED_SUPPORTED == 1)
          APP_BSP_LED_Off(LED_GREEN);
          APP_BSP_LED_Off(LED_RED);
#endif
          /* USER CODE END OT_DEVICE_ROLE_DETACHED */
          break;

      case OT_DEVICE_ROLE_CHILD:
          /* USER CODE BEGIN OT_DEVICE_ROLE_CHILD */
#if (CFG_LED_SUPPORTED == 1)
          APP_BSP_LED_Off(LED_GREEN);
          APP_BSP_LED_On(LED_RED);
#endif
          /* USER CODE END OT_DEVICE_ROLE_CHILD */
          break;

      case OT_DEVICE_ROLE_ROUTER :
          /* USER CODE BEGIN OT_DEVICE_ROLE_ROUTER */
#if (CFG_LED_SUPPORTED == 1)
          APP_BSP_LED_Off(LED_GREEN);
          APP_BSP_LED_On(LED_RED);
#endif
          /* USER CODE END OT_DEVICE_ROLE_ROUTER */
          break;

      case OT_DEVICE_ROLE_LEADER :
          /* USER CODE BEGIN OT_DEVICE_ROLE_LEADER */
#if (CFG_LED_SUPPORTED == 1)
          APP_BSP_LED_On(LED_GREEN);
          APP_BSP_LED_Off(LED_RED);
#endif
          /* USER CODE END OT_DEVICE_ROLE_LEADER */
          break;

      default:
          /* USER CODE BEGIN DEFAULT */
#if (CFG_LED_SUPPORTED == 1)
          APP_BSP_LED_Off(LED_GREEN);
          APP_BSP_LED_Off(LED_RED);
#endif
          /* USER CODE END DEFAULT */
          break;
    }
  }
}

#if (OT_CLI_USE == 1)
/* OT CLI UART functions */
void APP_THREAD_ProcessUart(void *argument)
{
  UNUSED(argument);

  arcUartProcess();
}

void APP_THREAD_ScheduleUART(void)
{
  osThreadFlagsSet(WpanTaskHandle, 1U << CFG_RTOS_FLAG_OT_CLIuart);
}

static void APP_THREAD_CliInit(otInstance *aInstance)
{

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

/* USER CODE END FD_LOCAL_FUNCTIONS */

