/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_entry.c
  * @author  MCD Application Team
  * @brief   Entry point of the application
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "app_entry.h"
#include "stm32_rtos.h"
#if (CFG_LPM_LEVEL != 0)
#include "stm32_lpm.h"
#endif /* (CFG_LPM_LEVEL != 0) */
#include "stm32_timer.h"
#include "advanced_memory_manager.h"
#include "stm32_mm.h"
#if (CFG_LOG_SUPPORTED != 0)
#include "stm32_adv_trace.h"
#include "serial_cmd_interpreter.h"
#endif /* CFG_LOG_SUPPORTED */
#include "app_ble.h"
#include "ll_sys.h"
#include "ll_sys_if.h"
#include "app_sys.h"
#include "otp.h"
#include "scm.h"
#include "bpka.h"
#include "flash_driver.h"
#include "flash_manager.h"
#include "simple_nvm_arbiter.h"
#include "app_debug.h"
#include "crc_ctrl.h"
#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
#include "adc_ctrl.h"
#include "temp_measurement.h"
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */
#if(CFG_RT_DEBUG_DTB == 1)
#include "RTDebug_dtb.h"
#endif /* CFG_RT_DEBUG_DTB */
#include "timer_if.h"
extern void xPortSysTickHandler (void);
extern void vPortSetupTimerInterrupt(void);

/* Private includes -----------------------------------------------------------*/
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
    .Id = CFG_AMM_VIRTUAL_STACK_BLE,
    .BufferSize = CFG_AMM_VIRTUAL_STACK_BLE_BUFFER_SIZE
  },
  /* Virtual Memory #2 */
  {
    .Id = CFG_AMM_VIRTUAL_APP_BLE,
    .BufferSize = CFG_AMM_VIRTUAL_APP_BLE_BUFFER_SIZE
  },
};

static AMM_InitParameters_t ammInitConfig =
{
  .p_PoolAddr = AMM_Pool,
  .PoolSize = CFG_AMM_POOL_SIZE,
  .VirtualMemoryNumber = CFG_AMM_VIRTUAL_MEMORY_NUMBER,
  .p_VirtualMemoryConfigList = vmConfig
};

/* Timer for HAL tick declaration */
static UTIL_TIMER_Object_t  TimerHALtick_Id;

/* Timer for OS tick declaration */
static UTIL_TIMER_Object_t  TimerOStick_Id;

#if ( CFG_LPM_LEVEL != 0)
/* Holds maximum number of FreeRTOS tick periods that can be suppressed */
static uint32_t maximumPossibleSuppressedTicks = 0;

/* Timer OS wakeup low power declaration */
static UTIL_TIMER_Object_t  TimerOSwakeup_Id;

/* Time remaining variables to correct next OS tick */
static uint32_t timeDiffRemaining = 0;
static uint32_t lowPowerTimeDiffRemaining = 0;
#endif /* ( CFG_LPM_LEVEL != 0) */

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

static osThreadId_t     FlashManagerTaskHandle;
static osSemaphoreId_t  FlashManagerSemaphore;

static const osThreadAttr_t FlashManagerTask_attributes = {
  .name         = "FLASH Manager Task",
  .priority     = TASK_PRIO_FLASH_MANAGER,
  .stack_size   = TASK_STACK_SIZE_FLASH_MANAGER,
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE,
  .stack_mem    = TASK_DEFAULT_STACK_MEM
};

static const osSemaphoreAttr_t FlashManagerSemaphore_attributes = {
  .name         = "FLASH Manager Semaphore",
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE
};

static osThreadId_t     BpkaTaskHandle;
static osSemaphoreId_t  BpkaSemaphore;

static const osThreadAttr_t BpkaTask_attributes = {
  .name         = "BPKA Task",
  .priority     = TASK_PRIO_BPKA,
  .stack_size   = TASK_STACK_SIZE_BPKA,
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE,
  .stack_mem    = TASK_DEFAULT_STACK_MEM
};

static const osSemaphoreAttr_t BpkaSemaphore_attributes = {
  .name         = "BPKA Semaphore",
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE
};

static osMutexId_t      crcCtrlMutex;

static const osMutexAttr_t crcCtrlMutex_attributes = {
  .name         = "CRC CTRL Mutex",
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE
};

#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
static osMutexId_t      adcCtrlMutex;

static const osMutexAttr_t adcCtrlMutex_attributes = {
  .name         = "ADC CTRL Mutex",
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE
};
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */

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

static void RNG_Task_Entry(void* argument);

static void APPE_FLASH_MANAGER_Init( void );
static void FLASH_Manager_Task_Entry(void* argument);

static void APPE_BPKA_Init( void );
static void BPKA_Task_Entry(void* argument);

static void TimerOStickCB(void *arg);
static void TimerHALtickCB(void *arg);
#if ( CFG_LPM_LEVEL != 0)
static void TimerOSwakeupCB(void *arg);
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

#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
  /* Initialize the Temperature measurement */
  TEMPMEAS_Init ();
#endif /* (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1) */

  /* Initialize the Advance Memory Manager module */
  APPE_AMM_Init();

  /* Initialize the Random Number Generator module */
  APPE_RNG_Init();

  /* Initialize the Flash Manager module */
  APPE_FLASH_MANAGER_Init();

  /* USER CODE BEGIN APPE_Init_1 */
#if (CFG_LED_SUPPORTED == 1)  
  APP_BSP_LedInit();
#endif /* (CFG_LED_SUPPORTED == 1) */
#if (CFG_BUTTON_SUPPORTED == 1)
  APP_BSP_ButtonInit();
#endif /* (CFG_BUTTON_SUPPORTED == 1) */

  /* USER CODE END APPE_Init_1 */

  crcCtrlMutex = osMutexNew(&crcCtrlMutex_attributes);
  if (crcCtrlMutex == NULL)
  {
    LOG_ERROR_APP( "CRC CTRL FreeRTOS objects creation FAILED");
    Error_Handler();
  }

#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
  adcCtrlMutex = osMutexNew(&adcCtrlMutex_attributes);
  if (adcCtrlMutex == NULL)
  {
    LOG_ERROR_APP( "ADC CTRL FreeRTOS objects creation FAILED");
    Error_Handler();
  }
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */

  /* Initialize the Ble Public Key Accelerator module */
  APPE_BPKA_Init();

  /* Initialize the Simple Non Volatile Memory Arbiter */
  if( SNVMA_Init((uint32_t *)CFG_SNVMA_START_ADDRESS) != SNVMA_ERROR_OK )
  {
    Error_Handler();
  }

  APP_BLE_Init();

  /* Disable RFTS Bypass for flash operation - Since LL has not started yet */
  FD_SetStatus (FD_FLASHACCESS_RFTS_BYPASS, LL_FLASH_DISABLE);

#if ( CFG_LPM_LEVEL != 0)
  /* create a SW timer to wakeup system from low power */
  UTIL_TIMER_Create(&TimerOSwakeup_Id,
                    0,
                    UTIL_TIMER_ONESHOT,
                    &TimerOSwakeupCB, 0);

  maximumPossibleSuppressedTicks = UINT32_MAX;
#endif /* ( CFG_LPM_LEVEL != 0) */

  /* Create an RTC based timer to trigger HAL tick increment */
  UTIL_TIMER_Create(&TimerHALtick_Id,
                    HAL_TICK_FREQ_100HZ,
                    UTIL_TIMER_PERIODIC,
                    &TimerHALtickCB, 0);
  uwTickFreq = HAL_TICK_FREQ_100HZ;

  /* Start HAL tick timer */
  UTIL_TIMER_StartWithPeriod(&TimerHALtick_Id, HAL_TICK_FREQ_100HZ);

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

  /* Initialize the Command Interpreter */
  Serial_CMD_Interpreter_Init();
#endif  /* (CFG_LOG_SUPPORTED != 0) */

#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
  ADCCTRL_Init ();
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */

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
  /* Setup GPIOA 13, 14, 15 in Analog no pull */
  if(__HAL_RCC_GPIOA_IS_CLK_ENABLED() == 0)
  {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIOA->PUPDR &= ~0xFC000000;
    GPIOA->MODER |= 0xFC000000;
    __HAL_RCC_GPIOA_CLK_DISABLE();
  }
  else
  {
    GPIOA->PUPDR &= ~0xFC000000;
    GPIOA->MODER |= 0xFC000000;
  }

  /* Setup GPIOB 3, 4 in Analog no pull */
  if(__HAL_RCC_GPIOB_IS_CLK_ENABLED() == 0)
  {
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIOB->PUPDR &= ~0x3C0;
    GPIOB->MODER |= 0x3C0;
    __HAL_RCC_GPIOB_CLK_DISABLE();
  }
  else
  {
    GPIOB->PUPDR &= ~0x3C0;
    GPIOB->MODER |= 0x3C0;
  }
#endif /* CFG_DEBUGGER_LEVEL */

#if (CFG_LPM_LEVEL != 0)
  /* Initialize low Power Manager. By default enabled */
  UTIL_LPM_Init();

#if (CFG_LPM_STDBY_SUPPORTED > 0)
  /* Enable SRAM1, SRAM2 and RADIO retention*/
  LL_PWR_SetSRAM1SBRetention(LL_PWR_SRAM1_SB_FULL_RETENTION);
  LL_PWR_SetSRAM2SBRetention(LL_PWR_SRAM2_SB_FULL_RETENTION);
  LL_PWR_SetRadioSBRetention(LL_PWR_RADIO_SB_FULL_RETENTION); /* Retain sleep timer configuration */

#else /* (CFG_LPM_STDBY_SUPPORTED > 0) */
  UTIL_LPM_SetOffMode(1U << CFG_LPM_APP, UTIL_LPM_DISABLE);
#endif /* (CFG_LPM_STDBY_SUPPORTED > 0) */
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
  }
}

/**
 * @brief Initialize Random Number Generator module
 */
static void APPE_RNG_Init(void)
{
  HW_RNG_SetPoolThreshold(CFG_HW_RNG_POOL_THRESHOLD);
  HW_RNG_Init();
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

/**
 * @brief Initialize Flash Manager module
 */
static void APPE_FLASH_MANAGER_Init(void)
{
  /* Create Flash Manager FreeRTOS objects */

  FlashManagerSemaphore = osSemaphoreNew(1U, 0U, &FlashManagerSemaphore_attributes);

  FlashManagerTaskHandle = osThreadNew(FLASH_Manager_Task_Entry, NULL, &FlashManagerTask_attributes);

  if ((FlashManagerTaskHandle == NULL) || (FlashManagerSemaphore == NULL))
  {
    LOG_ERROR_APP( "FLASH FreeRTOS objects creation FAILED");
    Error_Handler();
  }

  /* Disable flash before any use - RFTS */
  FD_SetStatus (FD_FLASHACCESS_RFTS, LL_FLASH_DISABLE);
  /* Enable RFTS Bypass for flash operation - Since LL has not started yet */
  FD_SetStatus (FD_FLASHACCESS_RFTS_BYPASS, LL_FLASH_ENABLE);
  /* Enable flash system flag */
  FD_SetStatus (FD_FLASHACCESS_SYSTEM, LL_FLASH_ENABLE);

  return;
}

/**
 * @brief Initialize Ble Public Key Accelerator module
 */
static void APPE_BPKA_Init(void)
{
  /* Create Ble Public Key Accelerator FreeRTOS objects */

  BpkaSemaphore = osSemaphoreNew(1U, 0U, &BpkaSemaphore_attributes);

  BpkaTaskHandle = osThreadNew(BPKA_Task_Entry, NULL, &BpkaTask_attributes);

  if ((BpkaTaskHandle == NULL) || (BpkaSemaphore == NULL))
  {
    LOG_ERROR_APP( "BPKA FreeRTOS objects creation FAILED");
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
  }
}

static void FLASH_Manager_Task_Entry(void* argument)
{
  UNUSED(argument);

  for(;;)
  {
    osSemaphoreAcquire(FlashManagerSemaphore, osWaitForever);
    FM_BackgroundProcess();
  }
}

static void BPKA_Task_Entry(void *argument)
{
  UNUSED(argument);

  for(;;)
  {
    osSemaphoreAcquire(BpkaSemaphore, osWaitForever);
    BPKA_BG_Process();
  }
}

/* Timer OS tick callback */
static void TimerOStickCB(void *arg)
{
  xPortSysTickHandler();
  /* USER CODE BEGIN TimerOStickCB */

  /* USER CODE END TimerOStickCB */
  return;
}

/* Timer HAL tick callback */
static void TimerHALtickCB(void *arg)
{
  HAL_IncTick();
  /* USER CODE BEGIN TimerHALtickCB */

  /* USER CODE END TimerHALtickCB */
  return;
}

#if ( CFG_LPM_LEVEL != 0)
/* OS wakeup callback */
static void TimerOSwakeupCB(void *arg)
{
  /* USER CODE BEGIN TimerOSwakeupCB */

  /* USER CODE END TimerOSwakeupCB */
  return;
}

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

void BPKACB_Process( void )
{
  osSemaphoreRelease(BpkaSemaphore);
}

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

void FM_ProcessRequest(void)
{
  /* Trigger to call Flash Manager process function */
  osSemaphoreRelease(FlashManagerSemaphore);
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
  
  /* Simulate button press from UART commands. */
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
  uint32_t lowPowerTimeBeforeSleep, lowPowerTimeAfterSleep, lowPowerTimeDiff, timeDiff;
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

    LL_RCC_ClearResetFlags();

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

    /* Compute time spent in low power state and report precision loss */
    lowPowerTimeDiff = (lowPowerTimeAfterSleep - lowPowerTimeBeforeSleep) + lowPowerTimeDiffRemaining;
    /* Store precision loss during RTC time conversion to report it for next OS tick. */
    timeDiff = TIMER_IF_Convert_Tick2ms(lowPowerTimeDiff);
    lowPowerTimeDiffRemaining = lowPowerTimeDiff - TIMER_IF_Convert_ms2Tick(timeDiff);
    /* Report precision loss */
    timeDiff += timeDiffRemaining;
    /* Store precision loss during OS tick time conversion to report it for next OS tick. */
    timeDiffRemaining = timeDiff % portTICK_PERIOD_MS;

    /* Correct the kernel tick count to account for the time spent in its low power state. */
    vTaskStepTick( timeDiff / portTICK_PERIOD_MS );

    /* Re-enable interrupts to allow the interrupt that brought the MCU
     * out of sleep mode to execute immediately.  See comments above
     * the cpsid instruction above. */
    __asm volatile ( "cpsie i" ::: "memory" );
    __asm volatile ( "dsb" );
    __asm volatile ( "isb" );

    /* Put the radio in active state */
    if( system_startup_done != FALSE )
    {
      LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_RADIO);
      (void)ll_sys_dp_slp_exit();
    }
    UTIL_LPM_SetOffMode(1U << CFG_LPM_LL_DEEPSLEEP, UTIL_LPM_ENABLE);

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

CRCCTRL_Cmd_Status_t CRCCTRL_MutexTake(void)
{
  osStatus_t os_status;
  CRCCTRL_Cmd_Status_t crc_status;
  /* USER CODE BEGIN CRCCTRL_MutexTake_0 */

  /* USER CODE END CRCCTRL_MutexTake_0 */
  os_status = osMutexAcquire(crcCtrlMutex, osWaitForever);

  if(os_status != osOK)
  {
    crc_status = CRCCTRL_NOK;
  }
  else
  {
    crc_status = CRCCTRL_OK;
  }
  /* USER CODE BEGIN CRCCTRL_MutexTake_1 */

  /* USER CODE END CRCCTRL_MutexTake_1 */
  return crc_status;
}

CRCCTRL_Cmd_Status_t CRCCTRL_MutexRelease(void)
{
  osStatus_t os_status;
  CRCCTRL_Cmd_Status_t crc_status;
  /* USER CODE BEGIN CRCCTRL_MutexRelease_0 */

  /* USER CODE END CRCCTRL_MutexRelease_0 */
  os_status = osMutexRelease(crcCtrlMutex);

  if(os_status != osOK)
  {
    crc_status = CRCCTRL_NOK;
  }
  else
  {
    crc_status = CRCCTRL_OK;
  }
  /* USER CODE BEGIN CRCCTRL_MutexRelease_1 */

  /* USER CODE END CRCCTRL_MutexRelease_1 */
  return crc_status;
}

#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
ADCCTRL_Cmd_Status_t ADCCTRL_MutexTake(void)
{
  osStatus_t os_status;
  ADCCTRL_Cmd_Status_t adc_status;
  /* USER CODE BEGIN ADCCTRL_MutexTake_0 */

  /* USER CODE END ADCCTRL_MutexTake_0 */
  os_status = osMutexAcquire(adcCtrlMutex, osWaitForever);

  if(os_status != osOK)
  {
    adc_status = ADCCTRL_NOK;
  }
  else
  {
    adc_status = ADCCTRL_OK;
  }
  /* USER CODE BEGIN ADCCTRL_MutexTake_1 */

  /* USER CODE END ADCCTRL_MutexTake_1 */
  return adc_status;
}

ADCCTRL_Cmd_Status_t ADCCTRL_MutexRelease(void)
{
  osStatus_t os_status;
  ADCCTRL_Cmd_Status_t adc_status;
  /* USER CODE BEGIN ADCCTRL_MutexRelease_0 */

  /* USER CODE END ADCCTRL_MutexRelease_0 */
  os_status = osMutexRelease(adcCtrlMutex);

  if(os_status != osOK)
  {
    adc_status = ADCCTRL_NOK;
  }
  else
  {
    adc_status = ADCCTRL_OK;
  }
  /* USER CODE BEGIN ADCCTRL_MutexRelease_1 */

  /* USER CODE END ADCCTRL_MutexRelease_1 */
  return adc_status;
}
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */

/* USER CODE BEGIN FD_WRAP_FUNCTIONS */

/* Overwrite HAL_InitTick to not use sysTick in this project */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  return HAL_OK;
}

void HAL_SuspendTick(void)
{
  UTIL_TIMER_Stop(&TimerHALtick_Id);
  return;
}

void HAL_ResumeTick(void)
{
  UTIL_TIMER_Start(&TimerHALtick_Id);
  return;
}
/* USER CODE END FD_WRAP_FUNCTIONS */
