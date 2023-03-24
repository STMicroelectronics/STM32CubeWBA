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
#include "app_conf.h"
#include "main.h"
#include "app_entry.h"
#include "app_threadx.h"
#include "stm32_lpm.h"
#include "stm32_timer.h"
#include "stm32_mm.h"
#include "stm32_adv_trace.h"
#include "app_ble.h"
#include "ll_sys_if.h"
#include "app_sys.h"
#include "otp.h"
#include "scm.h"
#include "bpka.h"
#include "ll_sys.h"
#include "advanced_memory_manager.h"
#include "flash_driver.h"
#include "flash_manager.h"
#include "simple_nvm_arbiter.h"
#include "app_debug.h"
#include "adc_ctrl.h"

/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32wbaxx_nucleo.h"
#include "usart_if.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */
EXTI_HandleTypeDef exti_handle;

/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/
/* AMM_BCKGND_TASK related defines */
#define AMM_BCKGND_TASK_STACK_SIZE    (256*7)
#define AMM_BCKGND_TASK_PRIO          (9)
#define AMM_BCKGND_TASK_PREEM_TRES    (0)

/* BPKA_TASK related defines */
#define BPKA_TASK_STACK_SIZE    (256*7)
#define BPKA_TASK_PRIO          (5)
#define BPKA_TASK_PREEM_TRES    (0)

/* HW_RNG_TASK related defines */
#define HW_RNG_TASK_STACK_SIZE    (256*7)
#define HW_RNG_TASK_PRIO          (5)
#define HW_RNG_TASK_PREEM_TRES    (0)

/* FLASH_MANAGER_BCKGND_TASK related defines */
#define FLASH_MANAGER_BCKGND_TASK_STACK_SIZE    (256*7)
#define FLASH_MANAGER_BCKGND_TASK_PRIO          (9)
#define FLASH_MANAGER_BCKGND_TASK_PREEM_TRES    (0)

/* USER CODE BEGIN PD */
/* Section specific to button management using UART */
#define C_SIZE_CMD_STRING       256U
/* PB1_BUTTON_PUSHED_TASK related defines */
#define PB1_BUTTON_PUSHED_TASK_STACK_SIZE    (256*7)
#define PB1_BUTTON_PUSHED_TASK_PRIO          (8)
#define PB1_BUTTON_PUSHED_TASK_PREEM_TRES    (0)

/* PB2_BUTTON_PUSHED_TASK related defines */
#define PB2_BUTTON_PUSHED_TASK_STACK_SIZE    (256*7)
#define PB2_BUTTON_PUSHED_TASK_PRIO          (8)
#define PB2_BUTTON_PUSHED_TASK_PREEM_TRES    (0)

/* PB3_BUTTON_PUSHED_TASK related defines */
#define PB3_BUTTON_PUSHED_TASK_STACK_SIZE    (256*7)
#define PB3_BUTTON_PUSHED_TASK_PRIO          (8)
#define PB3_BUTTON_PUSHED_TASK_PREEM_TRES    (0)

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

static uint32_t AMM_Pool[CFG_AMM_POOL_SIZE];
static AMM_InitParameters_t ammInitConfig =
{
  .p_PoolAddr = AMM_Pool,
  .PoolSize = CFG_AMM_POOL_SIZE,
  .VirtualMemoryNumber = CFG_AMM_VIRTUAL_MEMORY_NUMBER,
  .a_VirtualMemoryConfigList =
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
  }
};

/* AMM_BCKGND_TASK related resources */
TX_THREAD AMM_BCKGND_Thread;
TX_SEMAPHORE AMM_BCKGND_Thread_Sem;

/* BPKA_TASK related resources */
TX_THREAD BPKA_Thread;
TX_SEMAPHORE BPKA_Thread_Sem;

/* HW_RNG_TASK related resources */
TX_THREAD HW_RNG_Thread;
TX_SEMAPHORE HW_RNG_Thread_Sem;

/* FLASH_MANAGER_BCKGND_TASK related resources */
TX_THREAD FLASH_MANAGER_BCKGND_Thread;
TX_SEMAPHORE FLASH_MANAGER_BCKGND_Thread_Sem;

/* USER CODE BEGIN PV */
/* Section specific to button management using UART */
static uint8_t CommandString[C_SIZE_CMD_STRING];
static uint16_t indexReceiveChar = 0;

#if (CFG_BUTTON_SUPPORTED == 1)
/* PB1_BUTTON_PUSHED_TASK related resources */
TX_THREAD PB1_BUTTON_PUSHED_Thread;
TX_SEMAPHORE PB1_BUTTON_PUSHED_Thread_Sem;

/* PB2_BUTTON_PUSHED_TASK related resources */
TX_THREAD PB2_BUTTON_PUSHED_Thread;
TX_SEMAPHORE PB2_BUTTON_PUSHED_Thread_Sem;

/* PB3_BUTTON_PUSHED_TASK related resources */
TX_THREAD PB3_BUTTON_PUSHED_Thread;
TX_SEMAPHORE PB3_BUTTON_PUSHED_Thread_Sem;
#endif

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/
/* ThreadX byte pool pointer for whole WPAN middleware */
TX_BYTE_POOL *pBytePool;

/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private functions prototypes-----------------------------------------------*/
static void Config_HSE(void);
static void System_Init( void );
static void SystemPower_Config( void );

/**
 * @brief Wrapper for init function of the MM for the AMM
 *
 * @param p_PoolAddr: Address of the pool to use - Not use -
 * @param PoolSize: Size of the pool - Not use -
 *
 * @return None
 */
static void AMM_WrapperInit (uint32_t * const p_PoolAddr, const uint32_t PoolSize);

/**
 * @brief Wrapper for allocate function of the MM for the AMM
 *
 * @param BufferSize
 *
 * @return Allocated buffer
 */
static uint32_t * AMM_WrapperAllocate (const uint32_t BufferSize);

/**
 * @brief Wrapper for free function of the MM for the AMM
 *
 * @param p_BufferAddr
 *
 * @return None
 */
static void AMM_WrapperFree (uint32_t * const p_BufferAddr);
static void BPKA_BG_Process_Entry(unsigned long thread_input);
static void HW_RNG_Process_Entry(unsigned long thread_input);
static void AMM_BackgroundProcess_Entry(unsigned long thread_input);
static void FM_BackgroundProcess_Entry(unsigned long thread_input);

/* USER CODE BEGIN PFP */
#if (CFG_LED_SUPPORTED == 1)
static void Led_Init(void);
#endif
#if (CFG_BUTTON_SUPPORTED == 1)
static void Button_Init(void);
static void APP_ENTRY_Key_Button1_Entry(unsigned long thread_input);
static void APP_ENTRY_Key_Button2_Entry(unsigned long thread_input);
static void APP_ENTRY_Key_Button3_Entry(unsigned long thread_input);
#endif
/* Section specific to button management using UART */
static void RxUART_Init(void);
static void RxCpltCallback(uint8_t *pdata, uint16_t size, uint8_t error);
static void UartCmdExecute(void);

/* External variables --------------------------------------------------------*/
extern uint8_t charRx;
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void MX_APPE_Config(void)
{
  /* Configure HSE Tuning */
  Config_HSE();
}

uint32_t MX_APPE_Init(void *p_param)
{
  APP_DEBUG_SIGNAL_SET(APP_APPE_INIT);

  /* System initialization */
  System_Init();

  /* Configure the system Power Mode */
  SystemPower_Config();

  /* Initialize the Advance Memory Manager */
  AMM_Init (&ammInitConfig);

  /* save ThreadX byte pool for whole WPAN middleware */
  pBytePool = p_param;

  /* Register the AMM background task */
  CHAR * pStack;

  if (tx_byte_allocate(pBytePool, (void **) &pStack, AMM_BCKGND_TASK_STACK_SIZE,TX_NO_WAIT) != TX_SUCCESS)
  {
    Error_Handler();
  }
  if (tx_semaphore_create(&AMM_BCKGND_Thread_Sem, "AMM_BCKGND_Thread_Sem", 0)!= TX_SUCCESS )
  {
    Error_Handler();
  }
  if (tx_thread_create(&AMM_BCKGND_Thread, "AMM_BCKGND Thread", AMM_BackgroundProcess_Entry, 0,
                         pStack, AMM_BCKGND_TASK_STACK_SIZE,
                         AMM_BCKGND_TASK_PRIO, AMM_BCKGND_TASK_PREEM_TRES,
                         TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
  {
    Error_Handler();
  }

  /* Initialize the Simple NVM Arbiter */
  SNVMA_Init ((uint32_t *)CFG_SNVMA_START_ADDRESS);

  /* Register the flash manager task */
  if (tx_byte_allocate(pBytePool, (void **) &pStack, FLASH_MANAGER_BCKGND_TASK_STACK_SIZE,TX_NO_WAIT) != TX_SUCCESS)
  {
    Error_Handler();
  }
  if (tx_semaphore_create(&FLASH_MANAGER_BCKGND_Thread_Sem, "FLASH_MANAGER_BCKGND_Thread_Sem", 0)!= TX_SUCCESS )
  {
    Error_Handler();
  }
  if (tx_thread_create(&FLASH_MANAGER_BCKGND_Thread, "FLASH_MANAGER_BCKGND Thread", FM_BackgroundProcess_Entry, 0,
                         pStack, FLASH_MANAGER_BCKGND_TASK_STACK_SIZE,
                         FLASH_MANAGER_BCKGND_TASK_PRIO, FLASH_MANAGER_BCKGND_TASK_PREEM_TRES,
                         TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
  {
    Error_Handler();
  }

/* USER CODE BEGIN APPE_Init_1 */
#if (CFG_LED_SUPPORTED == 1)
  Led_Init();
#endif
#if (CFG_BUTTON_SUPPORTED == 1)
  Button_Init();
#endif

  RxUART_Init();
/* USER CODE END APPE_Init_1 */
  if (tx_byte_allocate(pBytePool, (void **) &pStack, BPKA_TASK_STACK_SIZE,TX_NO_WAIT) != TX_SUCCESS)
  {
    Error_Handler();
  }
  if (tx_semaphore_create(&BPKA_Thread_Sem, "BPKA_Thread_Sem", 0)!= TX_SUCCESS )
  {
    Error_Handler();
  }
  if (tx_thread_create(&BPKA_Thread, "BPKA Thread", BPKA_BG_Process_Entry, 0,
                         pStack, BPKA_TASK_STACK_SIZE,
                         BPKA_TASK_PRIO, BPKA_TASK_PREEM_TRES,
                         TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
  {
    Error_Handler();
  }
  BPKA_Reset( );

  if (tx_byte_allocate(pBytePool, (void **) &pStack, HW_RNG_TASK_STACK_SIZE,TX_NO_WAIT) != TX_SUCCESS)
  {
    Error_Handler();
  }
  if (tx_semaphore_create(&HW_RNG_Thread_Sem, "HW_RNG_Thread_Sem", 0)!= TX_SUCCESS )
  {
    Error_Handler();
  }
  if (tx_thread_create(&HW_RNG_Thread, "HW_RNG Thread", HW_RNG_Process_Entry, 0,
                         pStack, HW_RNG_TASK_STACK_SIZE,
                         HW_RNG_TASK_PRIO, HW_RNG_TASK_PREEM_TRES,
                         TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
  {
    Error_Handler();
  }
  HW_RNG_Start( );

  /* Disable flash before any use - RFTS */
  FD_SetStatus (FD_FLASHACCESS_RFTS, LL_FLASH_DISABLE);
  /* Enable RFTS Bypass for flash operation - Since LL has not started yet */
  FD_SetStatus (FD_FLASHACCESS_RFTS_BYPASS, LL_FLASH_ENABLE);
  /* Enable flash system flag */
  FD_SetStatus (FD_FLASHACCESS_SYSTEM, LL_FLASH_ENABLE);

  if (tx_semaphore_create(&PROC_GAP_COMPLETE_Sem, "PROC_GAP_COMPLETE_Sem", 0) != TX_SUCCESS )
  {
    Error_Handler();
  }
  APP_BLE_Init();
  ll_sys_config_params();
  /* Disable RFTS Bypass for flash operation - Since LL has not started yet */
  FD_SetStatus (FD_FLASHACCESS_RFTS_BYPASS, LL_FLASH_DISABLE);

/* USER CODE BEGIN APPE_Init_2 */

#if (CFG_BUTTON_SUPPORTED == 1)
  /* Register Button Tasks */
  if (tx_byte_allocate(pBytePool, (void **) &pStack, PB1_BUTTON_PUSHED_TASK_STACK_SIZE,TX_NO_WAIT) != TX_SUCCESS)
  {
    Error_Handler();
  }
  if (tx_semaphore_create(&PB1_BUTTON_PUSHED_Thread_Sem, "PB1_BUTTON_PUSHED_Thread_Sem", 0)!= TX_SUCCESS )
  {
    Error_Handler();
  }
  if (tx_thread_create(&PB1_BUTTON_PUSHED_Thread, "PB1_BUTTON_PUSHED Thread", APP_ENTRY_Key_Button1_Entry, 0,
                         pStack, PB1_BUTTON_PUSHED_TASK_STACK_SIZE,
                         PB1_BUTTON_PUSHED_TASK_PRIO, PB1_BUTTON_PUSHED_TASK_PREEM_TRES,
                         TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
  {
    Error_Handler();
  }
  
  if (tx_byte_allocate(pBytePool, (void **) &pStack, PB2_BUTTON_PUSHED_TASK_STACK_SIZE,TX_NO_WAIT) != TX_SUCCESS)
  {
    Error_Handler();
  }
  if (tx_semaphore_create(&PB2_BUTTON_PUSHED_Thread_Sem, "PB2_BUTTON_PUSHED_Thread_Sem", 0)!= TX_SUCCESS )
  {
    Error_Handler();
  }
  if (tx_thread_create(&PB2_BUTTON_PUSHED_Thread, "PB2_BUTTON_PUSHED Thread", APP_ENTRY_Key_Button2_Entry, 0,
                         pStack, PB2_BUTTON_PUSHED_TASK_STACK_SIZE,
                         PB2_BUTTON_PUSHED_TASK_PRIO, PB2_BUTTON_PUSHED_TASK_PREEM_TRES,
                         TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
  {
    Error_Handler();
  }
  
  if (tx_byte_allocate(pBytePool, (void **) &pStack, PB3_BUTTON_PUSHED_TASK_STACK_SIZE,TX_NO_WAIT) != TX_SUCCESS)
  {
    Error_Handler();
  }
  if (tx_semaphore_create(&PB3_BUTTON_PUSHED_Thread_Sem, "PB3_BUTTON_PUSHED_Thread_Sem", 0)!= TX_SUCCESS )
  {
    Error_Handler();
  }
  if (tx_thread_create(&PB3_BUTTON_PUSHED_Thread, "PB3_BUTTON_PUSHED Thread", APP_ENTRY_Key_Button3_Entry, 0,
                         pStack, PB3_BUTTON_PUSHED_TASK_STACK_SIZE,
                         PB3_BUTTON_PUSHED_TASK_PRIO, PB3_BUTTON_PUSHED_TASK_PREEM_TRES,
                         TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
  {
    Error_Handler();
  }
#endif

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

static void Config_HSE(void)
{
  OTP_Data_s* otp_ptr = NULL;

  /**
   * Read HSE_Tuning from OTP
   */
  if (OTP_Read(DEFAULT_OTP_IDX, &otp_ptr) != HAL_OK) {
    /* OTP no present in flash, apply default gain */
    HAL_RCCEx_HSESetTrimming(0x0C);
  }
  else
  {
    HAL_RCCEx_HSESetTrimming(otp_ptr->hsetune);
  }
}

static void System_Init( void )
{
  /* Clear RCC RESET flag */
  LL_RCC_ClearResetFlags();

  UTIL_TIMER_Init();
  /* Enable wakeup out of standby from RTC ( UTIL_TIMER )*/
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN7_HIGH_3);

#if (CFG_DEBUG_APP_TRACE != 0)
  /*Initialize the terminal using the USART2 */
  UTIL_ADV_TRACE_Init();
  UTIL_ADV_TRACE_SetVerboseLevel(VLEVEL_L); /*!< functional traces*/
  UTIL_ADV_TRACE_SetRegion(~0x0);
#endif

#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
  adc_ctrl_init();
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */

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
  scm_init();

 /* Initialize low power manager */
  UTIL_LPM_Init();

#if (CFG_LPM_STDBY_SUPPORTED == 1)
  /* Enable SRAM1, SRAM2 and RADIO retention*/
  LL_PWR_SetSRAM1SBRetention(LL_PWR_SRAM1_SB_FULL_RETENTION);
  LL_PWR_SetSRAM2SBRetention(LL_PWR_SRAM2_SB_FULL_RETENTION);
  LL_PWR_SetRadioSBRetention(LL_PWR_RADIO_SB_FULL_RETENTION); /* Retain sleep timer configuration */
#else
  UTIL_LPM_SetOffMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);
#endif
}

static void AMM_WrapperInit (uint32_t * const p_PoolAddr, const uint32_t PoolSize)
{
  UTIL_MM_Init ((uint8_t *)p_PoolAddr, ((size_t)PoolSize * sizeof(uint32_t)));
}

static uint32_t * AMM_WrapperAllocate (const uint32_t BufferSize)
{
  return (uint32_t *)UTIL_MM_GetBuffer (((size_t)BufferSize * sizeof(uint32_t)));
}

static void AMM_WrapperFree (uint32_t * const p_BufferAddr)
{
  UTIL_MM_ReleaseBuffer ((void *)p_BufferAddr);
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */
#if (CFG_LED_SUPPORTED == 1)
static void Led_Init( void )
{
  /* Leds Initialization */
  BSP_LED_Init(LED_BLUE);
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_RED);

  BSP_LED_On(LED_GREEN);

  return;
}
#endif

#if (CFG_BUTTON_SUPPORTED == 1)
static void Button_Init( void )
{
  /* Button Initialization */
  BSP_PB_Init(B1, BUTTON_MODE_EXTI);
  BSP_PB_Init(B2, BUTTON_MODE_EXTI);
  BSP_PB_Init(B3, BUTTON_MODE_EXTI);

  return;
}

static void APP_ENTRY_Key_Button1_Entry(unsigned long thread_input)
{
  (void)(thread_input);
  
  while(1)
  {
    tx_semaphore_get(&PB1_BUTTON_PUSHED_Thread_Sem, TX_WAIT_FOREVER);
    APP_BLE_Key_Button1_Action();
  }
}

static void APP_ENTRY_Key_Button2_Entry(unsigned long thread_input)
{
  (void)(thread_input);
  
  while(1)
  {
    tx_semaphore_get(&PB2_BUTTON_PUSHED_Thread_Sem, TX_WAIT_FOREVER);
    APP_BLE_Key_Button2_Action();
  }
}

static void APP_ENTRY_Key_Button3_Entry(unsigned long thread_input)
{
  (void)(thread_input);
  
  while(1)
  {
    tx_semaphore_get(&PB3_BUTTON_PUSHED_Thread_Sem, TX_WAIT_FOREVER);
    APP_BLE_Key_Button3_Action();
  }
}
#endif
/* USER CODE END FD_LOCAL_FUNCTIONS */

/*************************************************************
 *
 * WRAP FUNCTIONS
 *
 *************************************************************/
void ThreadXLowPowerUserEnter( void )
{
  LL_PWR_ClearFlag_STOP();

  LL_RCC_ClearResetFlags();

  /* Wait until System clock is not on HSI */
  while (LL_RCC_GetSysClkSource() == LL_RCC_SYS_CLKSOURCE_STATUS_HSI);

  HAL_SuspendTick();

  UTIL_LPM_EnterLowPower();

  return;
}

void ThreadXLowPowerUserExit( void )
{
  HAL_ResumeTick();
  LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_RADIO);
  ll_sys_dp_slp_exit();

  return;
}

void BPKACB_Process( void )
{
  tx_semaphore_put(&BPKA_Thread_Sem);
}

void BPKA_BG_Process_Entry(unsigned long thread_input)
{
  (void)(thread_input);

  while(1)
  {
    tx_semaphore_get(&BPKA_Thread_Sem, TX_WAIT_FOREVER);
    tx_mutex_get(&LINK_LAYER_Thread_Mutex, TX_WAIT_FOREVER);
    BPKA_BG_Process();
    tx_mutex_put(&LINK_LAYER_Thread_Mutex);
  }
}

void HWCB_RNG_Process( void )
{
  tx_semaphore_put(&HW_RNG_Thread_Sem);
}

void HW_RNG_Process_Entry(unsigned long thread_input)
{
  (void)(thread_input);

  while(1)
  {
    tx_semaphore_get(&HW_RNG_Thread_Sem, TX_WAIT_FOREVER);
     tx_mutex_get(&LINK_LAYER_Thread_Mutex, TX_WAIT_FOREVER);
    HW_RNG_Process();
    tx_mutex_put(&LINK_LAYER_Thread_Mutex);
  }
}

void AMM_RegisterBasicMemoryManager (AMM_BasicMemoryManagerFunctions_t * const p_BasicMemoryManagerFunctions)
{
  /* Fulfill the function handle */
  p_BasicMemoryManagerFunctions->Init = AMM_WrapperInit;
  p_BasicMemoryManagerFunctions->Allocate = AMM_WrapperAllocate;
  p_BasicMemoryManagerFunctions->Free = AMM_WrapperFree;
}

void AMM_ProcessRequest (void)
{
  /* Ask for AMM background task scheduling */
  tx_semaphore_put(&AMM_BCKGND_Thread_Sem);
}

void AMM_BackgroundProcess_Entry(unsigned long thread_input)
{
  (void)(thread_input);

  while(1)
  {
    tx_semaphore_get(&AMM_BCKGND_Thread_Sem, TX_WAIT_FOREVER);
    tx_mutex_get(&LINK_LAYER_Thread_Mutex, TX_WAIT_FOREVER);
    AMM_BackgroundProcess();
    tx_mutex_put(&LINK_LAYER_Thread_Mutex);
  }
}

void FM_ProcessRequest (void)
{
  /* Schedule the background process */
  tx_semaphore_put(&FLASH_MANAGER_BCKGND_Thread_Sem);
}

void FM_BackgroundProcess_Entry(unsigned long thread_input)
{
  (void)(thread_input);

  while(1)
  {
    tx_semaphore_get(&FLASH_MANAGER_BCKGND_Thread_Sem, TX_WAIT_FOREVER);
    tx_mutex_get(&LINK_LAYER_Thread_Mutex, TX_WAIT_FOREVER);
    FM_BackgroundProcess();
    tx_mutex_put(&LINK_LAYER_Thread_Mutex);
  }
}

/* USER CODE BEGIN FD_WRAP_FUNCTIONS */
void BSP_PB_Callback(Button_TypeDef Button)
{
  switch (Button)
  {
    case B1:
      tx_semaphore_put(&PB1_BUTTON_PUSHED_Thread_Sem);
      break;
    case B2:
      tx_semaphore_put(&PB2_BUTTON_PUSHED_Thread_Sem);
      break;
    case B3:
      tx_semaphore_put(&PB3_BUTTON_PUSHED_Thread_Sem);
      break;
    default:
      break;
  }

  return;
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == B1_PIN)
  {
    BSP_PB_Callback(B1);
  }
  if (GPIO_Pin == B2_PIN)
  {
    BSP_PB_Callback(B2);
  }
  if (GPIO_Pin == B3_PIN)
  {
    BSP_PB_Callback(B3);
  }

  return;
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
  HAL_GPIO_EXTI_Rising_Callback(GPIO_Pin);
}

static void RxUART_Init(void)
{
  UART_StartRx(RxCpltCallback);
}

static void RxCpltCallback(uint8_t *pdata, uint16_t size, uint8_t error)
{
  /* Filling buffer and wait for '\r' char */
  if (indexReceiveChar < C_SIZE_CMD_STRING)
  {
    if (charRx == '\r')
    {
      APP_DBG_MSG("received %s\n", CommandString);

      UartCmdExecute();

      /* Clear receive buffer and character counter*/
      indexReceiveChar = 0;
      memset(CommandString, 0, C_SIZE_CMD_STRING);
    }
    else
    {
      CommandString[indexReceiveChar++] = charRx;
    }
  }

  /* Once a character has been sent, put back the device in reception mode */
  UART_StartRx(RxCpltCallback);
}

static void UartCmdExecute(void)
{
  /* Parse received CommandString */
  if(strcmp((char const*)CommandString, "SW1") == 0)
  {
    APP_DBG_MSG("SW1 OK\n");
    exti_handle.Line = B1_EXTI_LINE;
    HAL_EXTI_GenerateSWI(&exti_handle);
  }
  else if (strcmp((char const*)CommandString, "SW2") == 0)
  {
    APP_DBG_MSG("SW2 OK\n");
    exti_handle.Line = B2_EXTI_LINE;
    HAL_EXTI_GenerateSWI(&exti_handle);
  }
  else if (strcmp((char const*)CommandString, "SW3") == 0)
  {
    APP_DBG_MSG("SW3 OK\n");
    exti_handle.Line = B3_EXTI_LINE;
    HAL_EXTI_GenerateSWI(&exti_handle);
  }
  else
  {
    APP_DBG_MSG("NOT RECOGNIZED COMMAND : %s\n", CommandString);
  }
}

#if (CFG_DEBUG_APP_TRACE != 0)
void RNG_KERNEL_CLK_OFF(void)
{
  /* Do not switch off HSI clock as it is used for traces */
}
#endif
/* USER CODE END FD_WRAP_FUNCTIONS */
