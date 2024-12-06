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

#if (CFG_LOG_SUPPORTED != 0)
/* Log configuration */
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

/* ThreadX objects declaration */

static TX_THREAD      AmmTaskHandle;
static TX_SEMAPHORE   AmmSemaphore;

static TX_THREAD      RngTaskHandle;
static TX_SEMAPHORE   RngSemaphore;

static TX_THREAD      FlashManagerTaskHandle;
static TX_SEMAPHORE   FlashManagerSemaphore;

static TX_THREAD      BpkaTaskHandle;
static TX_SEMAPHORE   BpkaSemaphore;

static TX_MUTEX       crcCtrlMutex;
#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
static TX_MUTEX       adcCtrlMutex;
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/
/* ThreadX byte pool pointer for whole WPAN middleware */
TX_BYTE_POOL  * pBytePool;
CHAR          * pStack;

/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private functions prototypes-----------------------------------------------*/
static void System_Init( void );
static void SystemPower_Config( void );
static void Config_HSE(void);
static void APPE_RNG_Init( void );

static void APPE_AMM_Init(void);
static void AMM_Task_Entry(ULONG lArgument);
static void AMM_WrapperInit(uint32_t * const p_PoolAddr, const uint32_t PoolSize);
static uint32_t * AMM_WrapperAllocate(const uint32_t BufferSize);
static void AMM_WrapperFree(uint32_t * const p_BufferAddr);

static void RNG_Task_Entry(ULONG lArgument);

static void APPE_FLASH_MANAGER_Init( void );
static void FLASH_Manager_Task_Entry(ULONG lArgument);

static void APPE_BPKA_Init( void );
static void BPKA_Task_Entry(ULONG lArgument);

#ifndef TX_LOW_POWER_USER_ENTER
void ThreadXLowPowerUserEnter( void );
#endif
#ifndef TX_LOW_POWER_USER_EXIT
void ThreadXLowPowerUserExit( void );
#endif

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
  UINT TXstatus;
  APP_DEBUG_SIGNAL_SET(APP_APPE_INIT);

  /* Save ThreadX byte pool for whole WPAN middleware */
  pBytePool = p_param;

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

  /* Disable flash before any use - RFTS */
  FD_SetStatus (FD_FLASHACCESS_RFTS, LL_FLASH_DISABLE);
  /* Enable RFTS Bypass for flash operation - Since LL has not started yet */
  FD_SetStatus (FD_FLASHACCESS_RFTS_BYPASS, LL_FLASH_ENABLE);
  /* Enable flash system flag */
  FD_SetStatus (FD_FLASHACCESS_SYSTEM, LL_FLASH_ENABLE);

  /* USER CODE BEGIN APPE_Init_1 */
#if (CFG_LED_SUPPORTED == 1)  
  APP_BSP_LedInit();
#endif /* (CFG_LED_SUPPORTED == 1) */
#if (CFG_BUTTON_SUPPORTED == 1)
  APP_BSP_ButtonInit();
#endif /* (CFG_BUTTON_SUPPORTED == 1) */

  /* USER CODE END APPE_Init_1 */

  TXstatus = tx_mutex_create(&crcCtrlMutex, "CRC CTRL Mutex", 0 );

  if( TXstatus != TX_SUCCESS )
  {
    LOG_ERROR_APP( "CRC CTRL ThreadX objects creation FAILED, status: %d", TXstatus);
    Error_Handler();
  }
#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
  TXstatus = tx_mutex_create(&adcCtrlMutex, "ADC CTRL Mutex", 0 );

  if( TXstatus != TX_SUCCESS )
  {
    LOG_ERROR_APP( "ADC CTRL ThreadX objects creation FAILED, status: %d", TXstatus);
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

#if (CFG_LPM_LEVEL != 0)
  /* Initialize low Power Manager. By default enabled */
  UTIL_LPM_Init();

#if (CFG_LPM_STDBY_SUPPORTED == 1)
  /* Enable SRAM1, SRAM2 and RADIO retention*/
  LL_PWR_SetSRAM1SBRetention(LL_PWR_SRAM1_SB_FULL_RETENTION);
  LL_PWR_SetSRAM2SBRetention(LL_PWR_SRAM2_SB_FULL_RETENTION);
  LL_PWR_SetRadioSBRetention(LL_PWR_RADIO_SB_FULL_RETENTION); /* Retain sleep timer configuration */

#else /* (CFG_LPM_STDBY_SUPPORTED == 1) */
  UTIL_LPM_SetOffMode(1U << CFG_LPM_APP, UTIL_LPM_DISABLE);
#endif /* (CFG_LPM_STDBY_SUPPORTED == 1) */
#endif /* (CFG_LPM_LEVEL != 0)  */

  /* USER CODE BEGIN SystemPower_Config */

  /* USER CODE END SystemPower_Config */
}

static void RNG_Task_Entry(ULONG lArgument)
{
  UNUSED(lArgument);

  for(;;)
  {
    tx_semaphore_get( &RngSemaphore, TX_WAIT_FOREVER );
    HW_RNG_Process();
    tx_thread_relinquish();
  }
}

/**
 * @brief Initialize Random Number Generator module
 */
static void APPE_RNG_Init(void)
{
  UINT TXstatus;
  CHAR *pStack;

  HW_RNG_Start();

  /* Create Random Number Generator ThreadX objects */

  TXstatus = tx_byte_allocate(pBytePool, (void **)&pStack, TASK_STACK_SIZE_RNG, TX_NO_WAIT);

  if( TXstatus == TX_SUCCESS )
  {
    TXstatus = tx_thread_create(&RngTaskHandle, "RNG Task", RNG_Task_Entry, 0,
                                 pStack, TASK_STACK_SIZE_RNG,
                                 TASK_PRIO_RNG, TASK_PREEMP_RNG,
                                 TX_NO_TIME_SLICE, TX_AUTO_START);

    TXstatus |= tx_semaphore_create(&RngSemaphore, "RNG Semaphore", 0);
  }

  if( TXstatus != TX_SUCCESS )
  {
    LOG_ERROR_APP( "RNG ThreadX objects creation FAILED, status: %d", TXstatus);
    Error_Handler();
  }
}

/**
 * @brief Initialize Flash Manager module
 */
static void APPE_FLASH_MANAGER_Init(void)
{
  UINT TXstatus;
  CHAR *pStack;

  /* Create Flash Manager ThreadX objects */

  TXstatus = tx_byte_allocate(pBytePool, (void **)&pStack, TASK_STACK_SIZE_FLASH_MANAGER, TX_NO_WAIT);

  if( TXstatus == TX_SUCCESS )
  {
    TXstatus = tx_thread_create(&FlashManagerTaskHandle, "FLASH Manager Task", FLASH_Manager_Task_Entry, 0,
                                 pStack, TASK_STACK_SIZE_FLASH_MANAGER,
                                 TASK_PRIO_FLASH_MANAGER, TASK_PREEMP_FLASH_MANAGER,
                                 TX_NO_TIME_SLICE, TX_AUTO_START);

    TXstatus |= tx_semaphore_create(&FlashManagerSemaphore, "FLASH Manager Semaphore", 0);
  }

  if( TXstatus != TX_SUCCESS )
  {
    LOG_ERROR_APP( "FLASH ThreadX objects creation FAILED, status: %d", TXstatus);
    Error_Handler();
  }
}

/**
 * @brief Initialize Ble Public Key Accelerator module
 */
static void APPE_BPKA_Init(void)
{
  UINT TXstatus;
  CHAR *pStack;

  /* Create Ble Public Key Accelerator ThreadX objects */

  TXstatus = tx_byte_allocate(pBytePool, (void **)&pStack, TASK_STACK_SIZE_BPKA, TX_NO_WAIT);

  if( TXstatus == TX_SUCCESS )
  {
    TXstatus = tx_thread_create(&BpkaTaskHandle, "BPKA Task", BPKA_Task_Entry, 0,
                                 pStack, TASK_STACK_SIZE_BPKA,
                                 TASK_PRIO_BPKA, TASK_PREEMP_BPKA,
                                 TX_NO_TIME_SLICE, TX_AUTO_START);

    TXstatus |= tx_semaphore_create(&BpkaSemaphore, "BPKA Semaphore", 0);
  }

  if( TXstatus != TX_SUCCESS )
  {
    LOG_ERROR_APP( "BPKA ThreadX objects creation FAILED, status: %d", TXstatus);
    Error_Handler();
  }
}

static void APPE_AMM_Init(void)
{
  UINT TXstatus;
  CHAR *pStack;

  /* Initialize the Advance Memory Manager */
  if( AMM_Init(&ammInitConfig) != AMM_ERROR_OK )
  {
    Error_Handler();
  }

  /* Create Advance Memory Manager ThreadX objects */

  TXstatus = tx_byte_allocate(pBytePool, (void **)&pStack, TASK_STACK_SIZE_AMM, TX_NO_WAIT);

  if( TXstatus == TX_SUCCESS )
  {
    TXstatus = tx_thread_create(&AmmTaskHandle, "AMM Task", AMM_Task_Entry, 0,
                                 pStack, TASK_STACK_SIZE_AMM,
                                 TASK_PRIO_AMM, TASK_PREEMP_AMM,
                                 TX_NO_TIME_SLICE, TX_AUTO_START);

    TXstatus |= tx_semaphore_create(&AmmSemaphore, "AMM Semaphore", 0);
  }

  if( TXstatus != TX_SUCCESS )
  {
    LOG_ERROR_APP( "AMM ThreadX objects creation FAILED, status: %d", TXstatus);
    Error_Handler();
  }
}

static void AMM_Task_Entry(ULONG lArgument)
{
  UNUSED(lArgument);

  for(;;)
  {
    tx_semaphore_get(&AmmSemaphore, TX_WAIT_FOREVER);
    AMM_BackgroundProcess();
    tx_thread_relinquish();
  }
}

static void FLASH_Manager_Task_Entry(ULONG lArgument)
{
  UNUSED(lArgument);

  for(;;)
  {
    tx_semaphore_get(&FlashManagerSemaphore, TX_WAIT_FOREVER);
    FM_BackgroundProcess();
    tx_thread_relinquish();
  }
}

static void BPKA_Task_Entry(ULONG lArgument)
{
  UNUSED(lArgument);

  for(;;)
  {
    tx_semaphore_get(&BpkaSemaphore, TX_WAIT_FOREVER);
    BPKA_BG_Process();
    tx_thread_relinquish();
  }
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

/* USER CODE END FD_LOCAL_FUNCTIONS */

/*************************************************************
 *
 * WRAP FUNCTIONS
 *
 *************************************************************/

void BPKACB_Process( void )
{
  tx_semaphore_put(&BpkaSemaphore);
}

/**
 * @brief Callback used by Random Number Generator to launch Task to generate Random Numbers
 */
void HWCB_RNG_Process( void )
{
  tx_semaphore_put(&RngSemaphore);
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
  tx_semaphore_put(&AmmSemaphore);
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
  tx_semaphore_put(&FlashManagerSemaphore);
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

/**
 * @brief   Enter in LowPower Mode after a ThreadX call
 */
void ThreadXLowPowerUserEnter( void )
{
  /* USER CODE BEGIN ThreadXLowPowerUserEnter_1 */

  /* USER CODE END ThreadXLowPowerUserEnter_1 */

#if ( CFG_LPM_LEVEL != 0 )
  LL_PWR_ClearFlag_STOP();

  LL_RCC_ClearResetFlags();

  /* Wait until HSE is ready */
  while ( LL_RCC_HSE_IsReady() == 0 );

  UTILS_ENTER_LIMITED_CRITICAL_SECTION( RCC_INTR_PRIO << 4U );
  scm_hserdy_isr();
  UTILS_EXIT_LIMITED_CRITICAL_SECTION();
  HAL_SuspendTick();

  /* Disable SysTick Interrupt */
  SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
  UTIL_LPM_EnterLowPower();
#endif /* CFG_LPM_LEVEL */

  /* USER CODE BEGIN ThreadXLowPowerUserEnter_2 */

  /* USER CODE END ThreadXLowPowerUserEnter_2 */
  return;
}

/**
 * @brief   Exit of LowPower Mode after a ThreadX call
 */
void ThreadXLowPowerUserExit( void )
{
  /* USER CODE BEGIN ThreadXLowPowerUserExit_1 */

  /* USER CODE END ThreadXLowPowerUserExit_1 */

#if ( CFG_LPM_LEVEL != 0 )
  HAL_ResumeTick();

  /* Enable SysTick Interrupt */
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
  LL_AHB5_GRP1_EnableClock( LL_AHB5_GRP1_PERIPH_RADIO );
  ll_sys_dp_slp_exit();
  UTIL_LPM_SetOffMode(1U << CFG_LPM_LL_DEEPSLEEP, UTIL_LPM_ENABLE);
#endif /* CFG_LPM_LEVEL */

  /* USER CODE BEGIN ThreadXLowPowerUserExit_2 */

  /* USER CODE END ThreadXLowPowerUserExit_2 */
  return;
}

CRCCTRL_Cmd_Status_t CRCCTRL_MutexTake(void)
{
  UINT TXstatus;
  CRCCTRL_Cmd_Status_t crc_status;
  /* USER CODE BEGIN CRCCTRL_MutexTake_0 */

  /* USER CODE END CRCCTRL_MutexTake_0 */
  if(TX_THREAD_GET_SYSTEM_STATE() == TX_INITIALIZE_IS_FINISHED)
  {
    TXstatus = tx_mutex_get(&crcCtrlMutex, TX_WAIT_FOREVER);
  }
  else
  {
    TXstatus = TX_SUCCESS;
  }

  if(TXstatus != TX_SUCCESS)
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
  UINT TXstatus;
  CRCCTRL_Cmd_Status_t crc_status;
  /* USER CODE BEGIN CRCCTRL_MutexRelease_0 */

  /* USER CODE END CRCCTRL_MutexRelease_0 */
  if(TX_THREAD_GET_SYSTEM_STATE() == TX_INITIALIZE_IS_FINISHED)
  {
    TXstatus = tx_mutex_put(&crcCtrlMutex);
  }
  else
  {
    TXstatus = TX_SUCCESS;
  }

  if(TXstatus != TX_SUCCESS)
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
  UINT TXstatus;
  ADCCTRL_Cmd_Status_t adc_status;
  /* USER CODE BEGIN ADCCTRL_MutexTake_0 */

  /* USER CODE END ADCCTRL_MutexTake_0 */
  if(TX_THREAD_GET_SYSTEM_STATE() == TX_INITIALIZE_IS_FINISHED)
  {
    TXstatus = tx_mutex_get(&adcCtrlMutex, TX_WAIT_FOREVER);
  }
  else
  {
    TXstatus = TX_SUCCESS;
  }

  if(TXstatus != TX_SUCCESS)
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
  UINT TXstatus;
  ADCCTRL_Cmd_Status_t adc_status;
  /* USER CODE BEGIN ADCCTRL_MutexRelease_0 */

  /* USER CODE END ADCCTRL_MutexRelease_0 */
  if(TX_THREAD_GET_SYSTEM_STATE() == TX_INITIALIZE_IS_FINISHED)
  {
    TXstatus = tx_mutex_put(&adcCtrlMutex);
  }
  else
  {
    TXstatus = TX_SUCCESS;
  }

  if(TXstatus != TX_SUCCESS)
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

/* USER CODE END FD_WRAP_FUNCTIONS */
