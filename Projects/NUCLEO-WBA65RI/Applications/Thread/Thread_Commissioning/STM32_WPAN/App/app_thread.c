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
#include "stm32_lpm_if.h"

/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_bsp.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
  NO_TOGGLING,
  FAST_TOGGLING,
  SLOW_TOGGLING
} APP_THREAD_LedTogglingMode_t;
/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/
#define C_CCA_THRESHOLD         (-70)

/* USER CODE BEGIN PD */
#define C_PANID                 0x2345U
#define C_CHANNEL_NB            17U
#define C_PASSWORD              "PASSPSKD" // MUST NOT BE USING "I" or "O" or "Q" or "Z" letters
#define LED_TIMEOUT             (300) /**< 300 ms */
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
static void APP_THREAD_ProcessUart(void);
#endif // OT_CLI_USE

static void APP_THREAD_PersistenceStartup(void);

/* USER CODE BEGIN PFP */
static void APP_THREAD_JoinerHandler(otError OtError, void *pContext);
static void APP_THREAD_ConfigJoiner(void);
static void APP_THREAD_ConfigLeaderDevice(void);
static void APP_THREAD_StartCommissioner(void);
static void APP_THREAD_AddJoiner(void);
static void APP_THREAD_TimingElapsed(void *args);

/* USER CODE END PFP */

/* Private variables -----------------------------------------------*/
static otInstance * PtOpenThreadInstance;

/* USER CODE BEGIN PV */
static APP_THREAD_LedTogglingMode_t   ToggleBlueLedMode = SLOW_TOGGLING;
static uint32_t   APP_THREAD_DeviceIsLeader = FALSE;
static UTIL_TIMER_Object_t timerThreadApp_Id;
/* USER CODE END PV */

/* Functions Definition ------------------------------------------------------*/

void ProcessAlarm(void)
{
  arcAlarmProcess(PtOpenThreadInstance);
}

void ProcessUsAlarm(void)
{
  arcUsAlarmProcess(PtOpenThreadInstance);
}

void ProcessTasklets(void)
{
  if (otTaskletsArePending(PtOpenThreadInstance) == TRUE)
  {
    UTIL_SEQ_SetTask(1U << CFG_TASK_OT_TASKLETS, TASK_PRIO_TASKLETS);
  }
}

/**
 * @brief  ProcessOpenThreadTasklets.
 * @param  None
 * @param  None
 * @retval None
 */
void ProcessOpenThreadTasklets(void)
{
  /* process the tasklet */
  otTaskletsProcess(PtOpenThreadInstance);

  /* reschedule the tasklets if any */
  ProcessTasklets();
}

/**
 * OpenThread calls this function when the tasklet queue transitions from empty to non-empty.
 *
 * @param[in] aInstance A pointer to an OpenThread instance.
 */
void otTaskletsSignalPending(otInstance *aInstance)
{
  UTIL_SEQ_SetTask(1U << CFG_TASK_OT_TASKLETS, TASK_PRIO_TASKLETS);
}

void APP_THREAD_ScheduleAlarm(void)
{
  UTIL_SEQ_SetTask(1U << CFG_TASK_OT_ALARM, TASK_PRIO_ALARM);
}

void APP_THREAD_ScheduleUsAlarm(void)
{
  UTIL_SEQ_SetTask(1U << CFG_TASK_OT_US_ALARM, TASK_PRIO_US_ALARM);
}

static void APP_THREAD_AlarmsInit(void)
{
  UTIL_SEQ_RegTask(1U << CFG_TASK_OT_ALARM, UTIL_SEQ_RFU, ProcessAlarm);
  UTIL_SEQ_RegTask(1U << CFG_TASK_OT_US_ALARM, UTIL_SEQ_RFU, ProcessUsAlarm);

  /* Run first time */
  UTIL_SEQ_SetTask(1U << CFG_TASK_OT_ALARM, TASK_PRIO_ALARM);
}

static void APP_THREAD_TaskletsInit(void)
{
  UTIL_SEQ_RegTask(1U << CFG_TASK_OT_TASKLETS, UTIL_SEQ_RFU, ProcessOpenThreadTasklets);
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

  /* Register tasks */
  APP_THREAD_AlarmsInit();
  APP_THREAD_TaskletsInit();

  ll_sys_thread_init();

  /* USER CODE BEGIN INIT TASKS */
  /* Set the commissioning task */
  UTIL_SEQ_RegTask(1<< CFG_TASK_START_COMMISSIONER, UTIL_SEQ_RFU, APP_THREAD_StartCommissioner);
    /**
   * Create timer to handle LED toggling
   */
  UTIL_TIMER_Create(&timerThreadApp_Id,
                    LED_TIMEOUT,
                    UTIL_TIMER_PERIODIC,
                    &APP_THREAD_TimingElapsed,
                    NULL);

  UTIL_TIMER_Start(&timerThreadApp_Id);
  
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

  otPlatRadioEnableSrcMatch(PtOpenThreadInstance, true);

  error = otIp6SetEnabled(PtOpenThreadInstance, true);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_IPV6_ENABLE,error);
  }
  error = otThreadSetEnabled(PtOpenThreadInstance, false);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_START,error);
  }
  /* USER CODE BEGIN DEVICECONFIG */


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
    case ERR_TIMER_INIT :
        APP_THREAD_TraceError("ERROR : ERR_TIMER_INIT ", ErrCode);
        break;
    case ERR_TIMER_START :
        APP_THREAD_TraceError("ERROR : ERR_TIMER_START ", ErrCode);
        break;
    case ERR_COMMISSIONER_START :
        APP_THREAD_TraceError("ERROR : ERR_COMMISSIONER_START ", ErrCode);
        break;
    case ERR_COMMISSIONER_CONFIG :
        APP_THREAD_TraceError("ERROR : ERR_COMMISSIONER_CONFIG ", ErrCode);
        break;
    case ERR_THREAD_JOINER_CB :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_JOINER_CB ", ErrCode);
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
          ToggleBlueLedMode = NO_TOGGLING;
          BSP_LED_On(LD1);
          APP_DBG("ROLE CHILD");
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
          if (APP_THREAD_DeviceIsLeader == TRUE)
          {
            UTIL_SEQ_SetTask(TASK_START_COMMISSIONER, CFG_SEQ_PRIO_1);
          }
          ToggleBlueLedMode = NO_TOGGLING;
          BSP_LED_On(LD1);
          APP_DBG("ROLE LEADER");
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
static void APP_THREAD_ProcessUart(void)
{
  arcUartProcess();
}

void APP_THREAD_ScheduleUART(void)
{
  UTIL_SEQ_SetTask(1U << CFG_TASK_OT_UART, TASK_PRIO_UART);
}

static void APP_THREAD_CliInit(otInstance *aInstance)
{
  UTIL_SEQ_RegTask(1 << CFG_TASK_OT_UART, UTIL_SEQ_RFU, APP_THREAD_ProcessUart);
  /* run first time */
  UTIL_SEQ_SetTask(1U << CFG_TASK_OT_UART, TASK_PRIO_UART);

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
 * @brief  Period elapsed callback
 * @param  none
 * @retval None
 */
static void APP_THREAD_TimingElapsed(void *args)
{
  static uint32_t internalFlag = 0;

  UNUSED(args);
  internalFlag++;
  if (internalFlag >= 4U)
    internalFlag = 0;

  switch(ToggleBlueLedMode)
  {
  case FAST_TOGGLING : BSP_LED_Toggle(LD1);
  break;

  case SLOW_TOGGLING : if (internalFlag == 0)
    BSP_LED_Toggle(LD1);
  break;
  default : break;
  }
}

static void CommissionerStateCallback(otCommissionerState aState, void *aContext)
{
  APP_DBG("otCommissionerStateCallback");

  switch (aState)
  {
  case OT_COMMISSIONER_STATE_DISABLED:
    APP_DBG("Commissioner : disabled\r\n");
    break;
  case OT_COMMISSIONER_STATE_PETITION:
    APP_DBG("Commissioner : petitioning\r\n");
    break;
  case OT_COMMISSIONER_STATE_ACTIVE:
    APP_DBG("Commissioner : active\r\n");
    APP_THREAD_AddJoiner();
    break;
  }
}

static void OutputBytes(const uint8_t *aBytes, uint8_t aLength)
{
  for (int i = 0; i < aLength; i++)
  {
    APP_DBG("%02x", aBytes[i]);
  }
}

static void CommissionerJoinerCallback(otCommissionerJoinerEvent aEvent, const otJoinerInfo * aJoinerInfo, const otExtAddress *aJoinerId, void *aContext)
{
  APP_DBG("Commissioner: Joiner ");

  switch (aEvent)
  {
  case OT_COMMISSIONER_JOINER_START:
      APP_DBG("start ");
      break;
  case OT_COMMISSIONER_JOINER_CONNECTED:
      APP_DBG("connect ");
      break;
  case OT_COMMISSIONER_JOINER_FINALIZE:
      APP_DBG("finalize ");
      break;
  case OT_COMMISSIONER_JOINER_END:
      APP_DBG("end ");
      break;
  case OT_COMMISSIONER_JOINER_REMOVED:
      APP_DBG("remove ");
      break;
  }

  OutputBytes(aJoinerId->m8, sizeof(*aJoinerId));
}


/**
 * @brief Commissioner configuration.
 *        This function starts and configure the commissioner.
 * @param  None
 * @retval None
 */
static void APP_THREAD_StartCommissioner(void)
{
  otError error;
  
  APP_DBG("APP_THREAD_StartCommissioner");
  /* Start the commissioner */
  error = otCommissionerStart(PtOpenThreadInstance,
      CommissionerStateCallback,
      CommissionerJoinerCallback,
      NULL);

  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_COMMISSIONER_START, error);
  }

}

/**
 * @brief Add Joiner.
 *        This function authorizes external devices to join the network.
 * @param  None
 * @retval None
 */
static void APP_THREAD_AddJoiner(void)
{
   /* Authorize external devices to join */
  otError error = otCommissionerAddJoiner(PtOpenThreadInstance,  /* otInstance */
  NULL,        /* aEui64 */
  C_PASSWORD,  /* aPskd */
  120U);       /* aTimeout */
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_COMMISSIONER_CONFIG, error);
  }
}

/**
 * @brief Commissioner configuration.
 *        This function starts and configures the commissioner.
 * @param  None
 * @retval None
 */
static void APP_THREAD_ConfigLeaderDevice(void)
{
  otError error;

  APP_DBG("APP_THREAD_ConfigLeaderDevice");

  error = otPlatRadioSetCcaEnergyDetectThreshold(PtOpenThreadInstance, C_CCA_THRESHOLD);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_SET_THRESHOLD,error);
  }

  error = otLinkSetChannel(PtOpenThreadInstance, C_CHANNEL_NB);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_SET_CHANNEL, error);
  }
  error = otLinkSetPanId(PtOpenThreadInstance, C_PANID);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_SET_PANID, error);
  }

  error = otIp6SetEnabled(PtOpenThreadInstance, true);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_IPV6_ENABLE, error);
  }
  error = otThreadSetEnabled(PtOpenThreadInstance, true);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_START, error);
  }
  APP_THREAD_DeviceIsLeader = TRUE;
}



/**
 * @brief Joiner configuration.
 *        This function starts and configures the joiner.
 * @param  None
 * @retval None
 */
static void APP_THREAD_ConfigJoiner(void)
{
  otError error;

  APP_DBG("APP_THREAD_ConfigJoiner");
  ToggleBlueLedMode = FAST_TOGGLING;
  
  error = otLinkSetChannel(PtOpenThreadInstance, C_CHANNEL_NB);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_SET_CHANNEL, error);
  }
  
  error = otIp6SetEnabled(PtOpenThreadInstance, true);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_IPV6_ENABLE, error);
  }
  
  /* Start the commissioner */
  error = otJoinerStart(PtOpenThreadInstance,   /* otInstance */
      C_PASSWORD,               /* aPskd */
      NULL,                     /* aProvisioningUrl */
      "OT",                     /* aVendorName */
      "TestAppli",              /* aVendorModel */
      "NA",                     /* aVendorSwVersion */
      NULL,                     /* aVendorData */
      APP_THREAD_JoinerHandler, /* otJoinerCallback aCallback */
      NULL);                    /* aContext */

  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_JOINER_START, error);
  }
}

/**
 * @brief Dummy request handler
 * @param
 * @retval None
 */
static void APP_THREAD_JoinerHandler(otError OtError, void *pContext)
{
  if (OtError == OT_ERROR_NONE)
  {
    OtError = otThreadSetEnabled(PtOpenThreadInstance, true);
    if (OtError != OT_ERROR_NONE)
    {
      APP_THREAD_Error(ERR_THREAD_START, OtError);
    }
    APP_DBG("JOIN SUCCESS!");
  }
  else
  {
    APP_THREAD_Error(ERR_THREAD_JOINER_CB, OtError);
  }
    
}

/**
 * @brief Task associated to the push button 1.
 * @param  None
 * @retval None
 */
void APP_BSP_Button1Action(void)
{
  LOG_INFO_APP("Config Leader Device");
  
  APP_THREAD_ConfigLeaderDevice();
}

/**
 * @brief Task associated to the push button 3.
 * @param  None
 * @retval None
 */
void APP_BSP_Button3Action(void)
{
  LOG_INFO_APP("Config Joiner");
  
  APP_THREAD_ConfigJoiner();
}
/* USER CODE END FD_LOCAL_FUNCTIONS */

