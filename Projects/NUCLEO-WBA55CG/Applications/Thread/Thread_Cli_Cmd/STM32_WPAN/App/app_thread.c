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
#include "threadplat_pka.h"
#include "joiner.h"
#include OPENTHREAD_CONFIG_FILE

/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32wbaxx_nucleo.h"
#include "app_thread_data_transfer.h"
#include "udp.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/
#define C_CCA_THRESHOLD         (-70)

/* USER CODE BEGIN PD */
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
static void APP_THREAD_ProcessPka(void);

/* USER CODE BEGIN PFP */
static void APP_THREAD_AppInit(void);
/* USER CODE END PFP */

/* Private variables -----------------------------------------------*/
static otInstance * PtOpenThreadInstance;

/* USER CODE BEGIN PV */
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
  /* wakeUp the system */
  //ll_sys_radio_hclk_ctrl_req(LL_SYS_RADIO_HCLK_LL_BG, LL_SYS_RADIO_HCLK_ON);
  //ll_sys_dp_slp_exit();

  /* process the tasklet */
  otTaskletsProcess(PtOpenThreadInstance);

  /* put the IP802_15_4 back to sleep mode */
  //ll_sys_radio_hclk_ctrl_req(LL_SYS_RADIO_HCLK_LL_BG, LL_SYS_RADIO_HCLK_OFF);

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

static void APP_THREAD_PkaInit(void)
{
  UTIL_SEQ_RegTask(1U << CFG_TASK_PKA, UTIL_SEQ_RFU, APP_THREAD_ProcessPka);
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
  APP_THREAD_PkaInit();

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
  
  while(1U == 1U)
  {
    BSP_LED_Toggle(LD1);
    HAL_Delay(500U);
    BSP_LED_Toggle(LD2);
    HAL_Delay(500U);
    BSP_LED_Toggle(LD3);
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
          BSP_LED_Off(LD2);
          BSP_LED_Off(LD3);
          /* USER CODE END OT_DEVICE_ROLE_DISABLED */
          break;

      case OT_DEVICE_ROLE_DETACHED:
          /* USER CODE BEGIN OT_DEVICE_ROLE_DETACHED */
          BSP_LED_Off(LD2);
          BSP_LED_Off(LD3);
          /* USER CODE END OT_DEVICE_ROLE_DETACHED */
          break;

      case OT_DEVICE_ROLE_CHILD:
          /* USER CODE BEGIN OT_DEVICE_ROLE_CHILD */
          BSP_LED_Off(LD2);
          BSP_LED_On(LD3);
          /* USER CODE END OT_DEVICE_ROLE_CHILD */
          break;

      case OT_DEVICE_ROLE_ROUTER :
          /* USER CODE BEGIN OT_DEVICE_ROLE_ROUTER */
          BSP_LED_Off(LD2);
          BSP_LED_On(LD3);
          /* USER CODE END OT_DEVICE_ROLE_ROUTER */
          break;

      case OT_DEVICE_ROLE_LEADER :
          /* USER CODE BEGIN OT_DEVICE_ROLE_LEADER */
          BSP_LED_On(LD2);
          BSP_LED_Off(LD3);
          /* USER CODE END OT_DEVICE_ROLE_LEADER */
          break;

      default:
          /* USER CODE BEGIN DEFAULT */
          BSP_LED_Off(LD2);
          BSP_LED_Off(LD3);
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

  otPlatUartEnable();
  otCliInit(aInstance, CliUartOutput, aInstance);
}
#endif /* OT_CLI_USE */

static void APP_THREAD_ProcessPka(void)
{
  otPlatPkaProccessLoop();
}

void APP_THREAD_SchedulePka(void)
{
  UTIL_SEQ_SetTask(1U << CFG_TASK_PKA, TASK_PRIO_PKA);
}

void APP_THREAD_WaitPkaEndOfOperation(void)
{
  /* Wait for event CFG_EVENT_PKA_COMPLETED */
  UTIL_SEQ_WaitEvt(1U << CFG_EVENT_PKA_COMPLETED);
}

void APP_THREAD_PostPkaEndOfOperation(void)
{
  /* Pka operation ended, set CFG_EVENT_PKA_COMPLETED event */
  UTIL_SEQ_SetEvt(1U << CFG_EVENT_PKA_COMPLETED);
}

void app_logger_write(uint8_t *buffer, uint32_t size)
{
  //UTIL_ADV_TRACE_COND_Send(VLEVEL_ALWAYS, ~0x0, 0, buffer, (uint16_t)size);
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

static void APP_THREAD_AppInit(void)
{
}




/* USER CODE END FD_LOCAL_FUNCTIONS */

