/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_entry.c
  * @author  MCD Application Team
  * @brief   Entry point of the application
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
#include "app_common.h"
#include "log_module.h"
#include "app_conf.h"
#include "main.h"
#include "app_zigbee.h"
#include "app_entry.h"
#include "stm32_rtos.h"
#if (CFG_LPM_LEVEL != 0)
#include "app_sys.h"
#include "stm32_lpm.h"
#endif /* (CFG_LPM_LEVEL != 0) */
#include "stm32_timer.h"
#include "advanced_memory_manager.h"
#include "stm32_mm.h"
#if (CFG_LOG_SUPPORTED != 0)
#include "stm32_adv_trace.h"
#include "serial_cmd_interpreter.h"
#endif /* CFG_LOG_SUPPORTED */
#include "otp.h"
#include "scm.h"
#include "stm32wbaxx_ll_rcc.h"
#include "timer_if.h"
extern void xPortSysTickHandler (void);
extern void vPortSetupTimerInterrupt(void);
#include "assert.h"

/* Private includes -----------------------------------------------------------*/
extern void ll_sys_mac_cntrl_init( void );
/* USER CODE BEGIN Includes */
#include "app_bsp.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private constants ---------------------------------------------------------*/
/* USER CODE BEGIN PC */

/* USER CODE END PC */

/* Private variables ---------------------------------------------------------*/
#if ( CFG_LPM_LEVEL != 0)
static bool system_startup_done = FALSE;
/* Holds maximum number of FreeRTOS tick periods that can be suppressed */
static uint32_t maximumPossibleSuppressedTicks = 0;
#endif /* ( CFG_LPM_LEVEL != 0) */

#if (CFG_LOG_SUPPORTED != 0)
/* Log configuration
 * .verbose_level can be any value of the Log_Verbose_Level_t enum.
 * .region_mask can either be :
 * - LOG_REGION_ALL_REGIONS to enable all regions
 * or
 * - One or several specific regions (any value except LOG_REGION_ALL_REGIONS)
 *   from the Log_Region_t enum and matching the mask value.
 *
 *   For example, to enable both LOG_REGION_BLE and LOG_REGION_APP,
 *   the value assigned to the define is :
 *   (1U << LOG_REGION_BLE | 1U << LOG_REGION_APP)
 */
static Log_Module_t Log_Module_Config = { .verbose_level = APPLI_CONFIG_LOG_LEVEL, .region_mask = APPLI_CONFIG_LOG_REGION };
#endif /* (CFG_LOG_SUPPORTED != 0) */

/* AMM configuration */
static uint32_t AMM_Pool[CFG_AMM_POOL_SIZE];
static AMM_VirtualMemoryConfig_t vmConfig[CFG_AMM_VIRTUAL_MEMORY_NUMBER] =
{
  /* Virtual Memory #1 */
  {
    .Id = CFG_AMM_VIRTUAL_STACK_ZIGBEE_INIT,
    .BufferSize = CFG_AMM_VIRTUAL_STACK_ZIGBEE_INIT_BUFFER_SIZE
  },
  /* Virtual Memory #2 */
  {
    .Id = CFG_AMM_VIRTUAL_STACK_ZIGBEE_HEAP,
    .BufferSize = CFG_AMM_VIRTUAL_STACK_ZIGBEE_HEAP_BUFFER_SIZE
  },
};

static AMM_InitParameters_t ammInitConfig =
{
  .p_PoolAddr = AMM_Pool,
  .PoolSize = CFG_AMM_POOL_SIZE,
  .VirtualMemoryNumber = CFG_AMM_VIRTUAL_MEMORY_NUMBER,
  .p_VirtualMemoryConfigList = vmConfig
};

/* Timers for FreeRTOS declaration */

static UTIL_TIMER_Object_t  TimerOStick_Id;
static UTIL_TIMER_Object_t  TimerOSwakeup_Id;

/* FreeRTOS objects declaration */

static osThreadId_t     AmmTaskHandle;
static osSemaphoreId_t  AmmSemaphore;

static const osThreadAttr_t AmmTask_attributes = {
  .name         = "AMM Task",
  .priority     = TASK_PRIO_AMM,
  .stack_size   = TASK_STACK_SIZE_AMM,
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE,
  .stack_mem    = TASK_DEFAULT_STACK_MEM
};

static const osSemaphoreAttr_t AmmSemaphore_attributes = {
  .name         = "AMM Semaphore",
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE
};

static osThreadId_t     RngTaskHandle;
static osSemaphoreId_t  RngSemaphore;

static const osThreadAttr_t RngTask_attributes = {
  .name         = "Random Number Generator Task",
  .priority     = TASK_PRIO_RNG,
  .stack_size   = TASK_STACK_SIZE_RNG,
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE,
  .stack_mem    = TASK_DEFAULT_STACK_MEM
};

static const osSemaphoreAttr_t RngSemaphore_attributes = {
  .name         = "Random Number Generator Semaphore",
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE
};

osThreadId_t          AppliTaskHandle;

const osThreadAttr_t AppliTask_attributes = 
{
  .name         = "Application Task",
  .priority     = TASK_PRIO_ZIGBEE_APP_START,
  .stack_size   = TASK_STACK_SIZE_ZIGBEE_APP_START,
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE,
  .stack_mem    = TASK_DEFAULT_STACK_MEM,
};


/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/
/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private functions prototypes-----------------------------------------------*/
static void System_Init( void );
static void SystemPower_Config( void );
static void Config_HSE(void);
static void APPE_RNG_Init( void );

static void APPE_AMM_Init(void);
static void AMM_Task_Entry(void* argument);
static void AMM_WrapperInit(uint32_t * const p_PoolAddr, const uint32_t PoolSize);
static uint32_t * AMM_WrapperAllocate(const uint32_t BufferSize);
static void AMM_WrapperFree(uint32_t * const p_BufferAddr);

void MX_APPE_InitTask(void* argument);
static void RNG_Task_Entry(void* argument);

static void TimerOStickCB(void *arg);
static void TimerOSwakeupCB(void *arg);

#if ( CFG_LPM_LEVEL != 0)
static uint32_t getCurrentTime(void);
#endif /* CFG_LPM_LEVEL */

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Functions Definition ------------------------------------------------------*/
/**
 * @brief   Wireless Private Area Network configuration.
 */
void MX_APPE_Config(void)
{
  /* Configure HSE Tuning */
  Config_HSE();
}

/**
 * @brief   LinkLayer & MAC Initialisation.
 */
void MX_APPE_LinkLayerInit(void)
{
  /* Initialization of the low level : link layer and MAC */
  ll_sys_mac_cntrl_init();

}

/**
 * @brief   System Tasks Initialisations
 */
void MX_APPE_InitTask( void * argument )
{
  /* USER CODE BEGIN APPE_Init_Task_1 */
  /* Initialize Peripherals */
  APP_BSP_Init();

  /* USER CODE END APPE_Init_Task_1 */

  /* Initialization of the low level : link layer and MAC */
  MX_APPE_LinkLayerInit();

  /* Initialization of the Zigbee Application */
  /* Must be called in RTOS context
     due to dependency of ZbInit() on MAC layer semaphore */
  APP_ZIGBEE_ApplicationInit();

  /* USER CODE BEGIN APPE_Init_Task_2 */
  /* USER CODE END APPE_Init_Task_2 */

  /* FreeRTOS hard fault if task simply returns... */
  osThreadExit();
}

/**
 * @brief   Wireless Private Area Network initialisation.
 */
uint32_t MX_APPE_Init(void *p_param)
{
  APP_DEBUG_SIGNAL_SET(APP_APPE_INIT);

  UNUSED(p_param);

  /* System initialization */
  System_Init();

  /* Configure the system Power Mode */
  SystemPower_Config();

  /* Initialize the Advance Memory Manager module */
  APPE_AMM_Init();

  /* Initialize the Random Number Generator module */
  APPE_RNG_Init();

  /* USER CODE BEGIN APPE_Init_1 */

  /* USER CODE END APPE_Init_1 */

  /* create a SW timer to wakeup system from low power */
  UTIL_TIMER_Create(&TimerOSwakeup_Id,
                    0,
                    UTIL_TIMER_ONESHOT,
                    &TimerOSwakeupCB, 0);
#if ( CFG_LPM_LEVEL != 0)
  maximumPossibleSuppressedTicks = UINT32_MAX;
#endif /* ( CFG_LPM_LEVEL != 0) */

  /* Create the Appli Task Thread  */
  AppliTaskHandle = osThreadNew( MX_APPE_InitTask, NULL, &AppliTask_attributes );
  if ( AppliTaskHandle == NULL )
  {
    APP_DBG( "ERROR FREERTOS : APPLICATION START THREAD CREATION FAILED" );
    while(1);
  }

  /* USER CODE BEGIN APPE_Init_2 */

  /* USER CODE END APPE_Init_2 */

  APP_DEBUG_SIGNAL_RESET(APP_APPE_INIT);

  return WPAN_SUCCESS;
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

/**
 * @brief Configure HSE by read this Tuning from OTP
 *
 */
static void Config_HSE(void)
{
  OTP_Data_s* otp_ptr = NULL;

  /* Read HSE_Tuning from OTP */
  if (OTP_Read(DEFAULT_OTP_IDX, &otp_ptr) != HAL_OK)
  {
    /* OTP no present in flash, apply default gain */
    HAL_RCCEx_HSESetTrimming(0x0C);
  }
  else
  {
    HAL_RCCEx_HSESetTrimming(otp_ptr->hsetune);
  }
}

/**
 *
 */
static void System_Init( void )
{
  /* Clear RCC RESET flag */
  LL_RCC_ClearResetFlags();

  /* Initialize the Timer Server */
  UTIL_TIMER_Init();

  /* Enable wakeup out of standby from RTC ( UTIL_TIMER )*/
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN7_HIGH_3);

#if (CFG_LOG_SUPPORTED != 0)
  MX_USART1_UART_Init();

  /* Initialize the logs ( using the USART ) */
  Log_Module_Init( Log_Module_Config );
  Log_Module_Set_Region( LOG_REGION_APP );
  Log_Module_Add_Region( LOG_REGION_ZIGBEE );

  /* Initialize the Command Interpreter */
  Serial_CMD_Interpreter_Init();
#endif  /* (CFG_LOG_SUPPORTED != 0) */

#if(CFG_RT_DEBUG_DTB == 1)
  /* DTB initialization and configuration */
  RT_DEBUG_DTBInit();
  RT_DEBUG_DTBConfig();
#endif /* CFG_RT_DEBUG_DTB */
#if(CFG_RT_DEBUG_GPIO_MODULE == 1)
  /* RT DEBUG GPIO_Init */
  RT_DEBUG_GPIO_Init();
#endif /* (CFG_RT_DEBUG_GPIO_MODULE == 1) */

#if ( CFG_LPM_LEVEL != 0)
  system_startup_done = TRUE;
  UNUSED(system_startup_done);
#endif /* ( CFG_LPM_LEVEL != 0) */

  return;
}

/**
 * @brief  Configure the system for power optimization
 *
 * @note  This API configures the system to be ready for low power mode
 *
 * @param  None
 * @retval None
 */
static void SystemPower_Config(void)
{
#if (CFG_SCM_SUPPORTED == 1)
  /* Initialize System Clock Manager */
  scm_init();
#endif /* CFG_SCM_SUPPORTED */

#if (CFG_DEBUGGER_LEVEL == 0)
  /* Pins used by SerialWire Debug are now analog input */
  GPIO_InitTypeDef DbgIOsInit = {0};
  DbgIOsInit.Mode = GPIO_MODE_ANALOG;
  DbgIOsInit.Pull = GPIO_NOPULL;
  DbgIOsInit.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  __HAL_RCC_GPIOA_CLK_ENABLE();
  HAL_GPIO_Init(GPIOA, &DbgIOsInit);

  DbgIOsInit.Mode = GPIO_MODE_ANALOG;
  DbgIOsInit.Pull = GPIO_NOPULL;
  DbgIOsInit.Pin = GPIO_PIN_3|GPIO_PIN_4;
  __HAL_RCC_GPIOB_CLK_ENABLE();
  HAL_GPIO_Init(GPIOB, &DbgIOsInit);
#endif /* CFG_DEBUGGER_LEVEL */

#if (CFG_SCM_SUPPORTED == 1)
  /* Set the HSE clock to 32MHz */
  scm_setsystemclock(SCM_USER_APP, HSE_32MHZ);
#endif /* CFG_SCM_SUPPORTED */

#if (CFG_LPM_LEVEL != 0)
  /* Initialize low Power Manager. By default enabled */
  UTIL_LPM_Init();

#if (CFG_LPM_STDBY_SUPPORTED > 0)
  /* Enable SRAM1, SRAM2 and RADIO retention*/
  LL_PWR_SetSRAM1SBRetention(LL_PWR_SRAM1_SB_FULL_RETENTION);
  LL_PWR_SetSRAM2SBRetention(LL_PWR_SRAM2_SB_FULL_RETENTION);
  LL_PWR_SetRadioSBRetention(LL_PWR_RADIO_SB_FULL_RETENTION); /* Retain sleep timer configuration */

#endif /* (CFG_LPM_STDBY_SUPPORTED > 0) */

  /* Disable LowPower during Init */
  UTIL_LPM_SetStopMode(1U << CFG_LPM_APP, UTIL_LPM_DISABLE);
  UTIL_LPM_SetOffMode(1U << CFG_LPM_APP, UTIL_LPM_DISABLE);

#endif /* (CFG_LPM_LEVEL != 0)  */

  /* USER CODE BEGIN SystemPower_Config */

  /* USER CODE END SystemPower_Config */
}

static void RNG_Task_Entry(void *argument)
{
  UNUSED(argument);

  for(;;)
  {
    osSemaphoreAcquire(RngSemaphore, osWaitForever);
    HW_RNG_Process();
    osThreadYield();
  }
}

/**
 * @brief Initialize Random Number Generator module
 */
static void APPE_RNG_Init(void)
{
  HW_RNG_Start();

  /* Create Random Number Generator FreeRTOS objects */

  RngSemaphore = osSemaphoreNew(1U, 0U, &RngSemaphore_attributes);

  RngTaskHandle = osThreadNew(RNG_Task_Entry, NULL, &RngTask_attributes);

  if ((RngTaskHandle == NULL) || (RngSemaphore == NULL))
  {
    LOG_ERROR_APP( "RNG FreeRTOS objects creation FAILED");
    Error_Handler();
  }

}

static void APPE_AMM_Init(void)
{
  /* Initialize the Advance Memory Manager */
  if( AMM_Init(&ammInitConfig) != AMM_ERROR_OK )
  {
    Error_Handler();
  }

  /* Create Advance Memory Manager FreeRTOS objects */

  AmmSemaphore = osSemaphoreNew(1U, 0U, &AmmSemaphore_attributes);

  AmmTaskHandle = osThreadNew(AMM_Task_Entry, NULL, &AmmTask_attributes);

  if ((AmmTaskHandle == NULL) || (AmmSemaphore == NULL))
  {
    LOG_ERROR_APP( "AMM FreeRTOS objects creation FAILED");
    Error_Handler();
  }
}

static void AMM_Task_Entry(void* argument)
{
  UNUSED(argument);

  for(;;)
  {
    osSemaphoreAcquire(AmmSemaphore, osWaitForever);
    AMM_BackgroundProcess();
    osThreadYield();
  }
}

/* OS tick callback */
static void TimerOStickCB(void *arg)
{
  xPortSysTickHandler();
  /* USER CODE BEGIN TimerOStickCB */

  /* USER CODE END TimerOStickCB */

  return;
}

/* OS wakeup callback */
static void TimerOSwakeupCB(void *arg)
{
  /* USER CODE BEGIN TimerOSwakeupCB */

  /* USER CODE END TimerOSwakeupCB */
  return;
}

#if ( CFG_LPM_LEVEL != 0)
/* return current time since boot, continue to count in standby low power mode */
static uint32_t getCurrentTime(void)
{
  return TIMER_IF_GetTimerValue();
}

#endif /* ( CFG_LPM_LEVEL != 0) */

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

/* USER CODE END FD_LOCAL_FUNCTIONS */

/*************************************************************
 *
 * WRAP FUNCTIONS
 *
 *************************************************************/

/**
 * @brief Callback used by Random Number Generator to launch Task to generate Random Numbers
 */
void HWCB_RNG_Process( void )
{
  osSemaphoreRelease(RngSemaphore);
}

void AMM_RegisterBasicMemoryManager (AMM_BasicMemoryManagerFunctions_t * const p_BasicMemoryManagerFunctions)
{
  /* Fulfill the function handle */
  p_BasicMemoryManagerFunctions->Init = AMM_WrapperInit;
  p_BasicMemoryManagerFunctions->Allocate = AMM_WrapperAllocate;
  p_BasicMemoryManagerFunctions->Free = AMM_WrapperFree;
}

void AMM_ProcessRequest(void)
{
  /* Trigger to call Advance Memory Manager process function */
  osSemaphoreRelease(AmmSemaphore);
}

static void AMM_WrapperInit(uint32_t * const p_PoolAddr, const uint32_t PoolSize)
{
  UTIL_MM_Init ((uint8_t *)p_PoolAddr, ((size_t)PoolSize * sizeof(uint32_t)));
}

static uint32_t * AMM_WrapperAllocate(const uint32_t BufferSize)
{
  return (uint32_t *)UTIL_MM_GetBuffer (((size_t)BufferSize * sizeof(uint32_t)));
}

static void AMM_WrapperFree (uint32_t * const p_BufferAddr)
{
  UTIL_MM_ReleaseBuffer ((void *)p_BufferAddr);
}

#if ((CFG_LOG_SUPPORTED == 0) && (CFG_LPM_LEVEL != 0))
/* RNG module turn off HSI clock when traces are not used and low power used */
void RNG_KERNEL_CLK_OFF(void)
{
  /* USER CODE BEGIN RNG_KERNEL_CLK_OFF_1 */

  /* USER CODE END RNG_KERNEL_CLK_OFF_1 */
  LL_RCC_HSI_Disable();
  /* USER CODE BEGIN RNG_KERNEL_CLK_OFF_2 */

  /* USER CODE END RNG_KERNEL_CLK_OFF_2 */
}

#if (CFG_SCM_SUPPORTED == 1)
/* SCM module turn off HSI clock when traces are not used and low power used */
void SCM_HSI_CLK_OFF(void)
{
  /* USER CODE BEGIN SCM_HSI_CLK_OFF_1 */

  /* USER CODE END SCM_HSI_CLK_OFF_1 */
  LL_RCC_HSI_Disable();
  /* USER CODE BEGIN SCM_HSI_CLK_OFF_2 */

  /* USER CODE END SCM_HSI_CLK_OFF_2 */
}
#endif /* CFG_SCM_SUPPORTED */
#endif /* ((CFG_LOG_SUPPORTED == 0) && (CFG_LPM_LEVEL != 0)) */

#if (CFG_LOG_SUPPORTED != 0)
void UTIL_ADV_TRACE_PreSendHook(void)
{
#if (CFG_LPM_LEVEL != 0)
  /* Disable Stop mode before sending a LOG message over UART */
  UTIL_LPM_SetStopMode(1U << CFG_LPM_LOG, UTIL_LPM_DISABLE);
#endif /* (CFG_LPM_LEVEL != 0) */
  /* USER CODE BEGIN UTIL_ADV_TRACE_PreSendHook */

  /* USER CODE END UTIL_ADV_TRACE_PreSendHook */
}

void UTIL_ADV_TRACE_PostSendHook(void)
{
#if (CFG_LPM_LEVEL != 0)
  /* Enable Stop mode after LOG message over UART sent */
  UTIL_LPM_SetStopMode(1U << CFG_LPM_LOG, UTIL_LPM_ENABLE);
#endif /* (CFG_LPM_LEVEL != 0) */
  /* USER CODE BEGIN UTIL_ADV_TRACE_PostSendHook */

  /* USER CODE END UTIL_ADV_TRACE_PostSendHook */
}

/**
 * @brief  Treat Serial commands.
 *
 * @param  pRxBuffer      Pointer on received data from USART.
 * @param  iRxBufferSize  Number of received data.
 * @retval None
 */
void Serial_CMD_Interpreter_CmdExecute( uint8_t * pRxBuffer, uint16_t iRxBufferSize )
{
  /* USER CODE BEGIN Serial_CMD_Interpreter_CmdExecute_1 */
  
  /* Threat USART Command to simulate button press for instance. */
  (void)APP_BSP_SerialCmdExecute( pRxBuffer, iRxBufferSize );

  /* USER CODE END Serial_CMD_Interpreter_CmdExecute_1 */
}

#endif /* (CFG_LOG_SUPPORTED != 0) */

/* Implement weak function to setup a timer that will trig OS ticks */
void vPortSetupTimerInterrupt( void )
{
  UTIL_TIMER_Create(&TimerOStick_Id,
                    portTICK_PERIOD_MS,
                    UTIL_TIMER_PERIODIC,
                    &TimerOStickCB, 0);

  UTIL_TIMER_StartWithPeriod(&TimerOStick_Id, portTICK_PERIOD_MS);
  /* USER CODE BEGIN vPortSetupTimerInterrupt */

  /* USER CODE END vPortSetupTimerInterrupt */
  return;
}

#if ( CFG_LPM_LEVEL != 0)
void vPortSuppressTicksAndSleep( uint32_t xExpectedIdleTime )
{
  uint32_t lowPowerTimeBeforeSleep, lowPowerTimeAfterSleep;
  eSleepModeStatus eSleepStatus;

  /* Stop the timer that is generating the OS tick interrupt. */
  UTIL_TIMER_Stop(&TimerOStick_Id);

  /* Make sure the SysTick reload value does not overflow the counter. */
  if( xExpectedIdleTime > maximumPossibleSuppressedTicks )
  {
    xExpectedIdleTime = maximumPossibleSuppressedTicks;
  }

  /* Enter a critical section but don't use the taskENTER_CRITICAL()
   * method as that will mask interrupts that should exit sleep mode. */
  __asm volatile ( "cpsid i" ::: "memory" );
  __asm volatile ( "dsb" );
  __asm volatile ( "isb" );

  eSleepStatus = eTaskConfirmSleepModeStatus();

  /* If a context switch is pending or a task is waiting for the scheduler
   * to be unsuspended then abandon the low power entry. */
  if( eSleepStatus == eAbortSleep )
  {
    /* Restart the timer that is generating the OS tick interrupt. */
    UTIL_TIMER_StartWithPeriod(&TimerOStick_Id, portTICK_PERIOD_MS);

    /* Re-enable interrupts - see comments above the cpsid instruction above. */
    __asm volatile ( "cpsie i" ::: "memory" );
  }
  else
  {
    if( eSleepStatus != eNoTasksWaitingTimeout )
    {
      /* Configure an interrupt to bring the microcontroller out of its low
         power state at the time the kernel next needs to execute. The
         interrupt must be generated from a source that remains operational
         when the microcontroller is in a low power state. */
      UTIL_TIMER_StartWithPeriod(&TimerOSwakeup_Id, (xExpectedIdleTime - 1) * portTICK_PERIOD_MS);
    }

    /* Read the current time from RTC, maintainned in standby */
    lowPowerTimeBeforeSleep = getCurrentTime();

    /* Enter the low power state. */

    LL_PWR_ClearFlag_STOP();
    if ( ( system_startup_done != FALSE ) && ( UTIL_LPM_GetMode() == UTIL_LPM_OFFMODE ) )
    {
      APP_SYS_BLE_EnterDeepSleep();
    }

    LL_RCC_ClearResetFlags();/* TODO check if its necessary -----------------------------------------------------*/

    HAL_SuspendTick();
    UTIL_LPM_EnterLowPower(); /* WFI instruction call is inside this API */
    HAL_ResumeTick();

    /* Stop the timer that may wakeup us as wakeup source can be another one */
    UTIL_TIMER_Stop(&TimerOSwakeup_Id);

    /* Determine how long the microcontroller was actually in a low power
     state for, which will be less than xExpectedIdleTime if the
     microcontroller was brought out of low power mode by an interrupt
     other than that configured by the vSetWakeTimeInterrupt() call.
     Note that the scheduler is suspended before
     portSUPPRESS_TICKS_AND_SLEEP() is called, and resumed when
     portSUPPRESS_TICKS_AND_SLEEP() returns.  Therefore no other tasks will
     execute until this function completes. */
    lowPowerTimeAfterSleep = getCurrentTime();

    /* Correct the kernel tick count to account for the time spent in its low power state. */
    vTaskStepTick( TIMER_IF_Convert_Tick2ms(lowPowerTimeAfterSleep - lowPowerTimeBeforeSleep) / portTICK_PERIOD_MS );

    /* Re-enable interrupts to allow the interrupt that brought the MCU
     * out of sleep mode to execute immediately.  See comments above
     * the cpsid instruction above. */
    __asm volatile ( "cpsie i" ::: "memory" );
    __asm volatile ( "dsb" );
    __asm volatile ( "isb" );

    /* Put the radio in active state */
    if ( ( system_startup_done != FALSE ) && ( UTIL_LPM_GetMode() == UTIL_LPM_OFFMODE ) )
    {
      LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_RADIO);
      ll_sys_dp_slp_exit();
    }

    /* Restart the timer that is generating the OS tick interrupt. */
    UTIL_TIMER_StartWithPeriod(&TimerOStick_Id, portTICK_PERIOD_MS);
  }
  return;
}
#else
void vPortSuppressTicksAndSleep( uint32_t xExpectedIdleTime )
{
  return;
}
#endif /* ( CFG_LPM_LEVEL != 0) */


/**
 * @brief Function Assert AEABI in case of not described on 'libc' libraries.
 */
__WEAK void __aeabi_assert(const char * szExpression, const char * szFile, int iLine)
{
  Error_Handler();
  __builtin_unreachable();
}

/* USER CODE BEGIN FD_WRAP_FUNCTIONS */

/* USER CODE END FD_WRAP_FUNCTIONS */
