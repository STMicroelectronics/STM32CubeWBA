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
#if (CFG_LPM_SUPPORTED == 1)
#include "stm32_lpm.h"
#endif /* CFG_LPM_SUPPORTED */
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
#if (CFG_BUTTON_SUPPORTED == 1)
typedef struct
{
  Button_TypeDef      button;
  UTIL_TIMER_Object_t longTimerId;
  uint8_t             longPressed;
} ButtonDesc_t;
#endif /* (CFG_BUTTON_SUPPORTED == 1) */

/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/
/* AMM_BCKGND_TASK related defines */
#define AMM_BCKGND_TASK_STACK_SIZE    (256*7)
#define AMM_BCKGND_TASK_PRIO          (15)
#define AMM_BCKGND_TASK_PREEM_TRES    (0)

/* BPKA_TASK related defines */
#define BPKA_TASK_STACK_SIZE    (256*7)
#define BPKA_TASK_PRIO          (15)
#define BPKA_TASK_PREEM_TRES    (0)

/* HW_RNG_TASK related defines */
#define HW_RNG_TASK_STACK_SIZE    (256*7)
#define HW_RNG_TASK_PRIO          (15)
#define HW_RNG_TASK_PREEM_TRES    (0)

/* FLASH_MANAGER_BCKGND_TASK related defines */
#define FLASH_MANAGER_BCKGND_TASK_STACK_SIZE    (256*7)
#define FLASH_MANAGER_BCKGND_TASK_PRIO          (15)
#define FLASH_MANAGER_BCKGND_TASK_PREEM_TRES    (0)

/* USER CODE BEGIN PD */
#if (CFG_BUTTON_SUPPORTED == 1)
#define BUTTON_LONG_PRESS_THRESHOLD_MS   (500u)
#define BUTTON_NB_MAX                    (B3 + 1u)
#endif
/* Section specific to button management using UART */
#define C_SIZE_CMD_STRING       256U

/* PB1_BUTTON_PUSHED_TASK related defines */
#define PB1_BUTTON_PUSHED_TASK_STACK_SIZE    (256*7)
#define PB1_BUTTON_PUSHED_TASK_PRIO          (15)
#define PB1_BUTTON_PUSHED_TASK_PREEM_TRES    (0)

/* PB2_BUTTON_PUSHED_TASK related defines */
#define PB2_BUTTON_PUSHED_TASK_STACK_SIZE    (256*7)
#define PB2_BUTTON_PUSHED_TASK_PRIO          (15)
#define PB2_BUTTON_PUSHED_TASK_PREEM_TRES    (0)

/* PB3_BUTTON_PUSHED_TASK related defines */
#define PB3_BUTTON_PUSHED_TASK_STACK_SIZE    (256*7)
#define PB3_BUTTON_PUSHED_TASK_PRIO          (15)
#define PB3_BUTTON_PUSHED_TASK_PREEM_TRES    (0)

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

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
#if (CFG_BUTTON_SUPPORTED == 1)
/* Button management */
static ButtonDesc_t buttonDesc[BUTTON_NB_MAX];

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
/* Section specific to button management using UART */
static uint8_t CommandString[C_SIZE_CMD_STRING];
static uint16_t indexReceiveChar = 0;
EXTI_HandleTypeDef exti_handle;

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/
/* ThreadX byte pool pointer for whole WPAN middleware */
TX_BYTE_POOL *pBytePool;
/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private functions prototypes-----------------------------------------------*/
static void Config_HSE(void);
static void RNG_Init( void );
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
static void Button_TriggerActions(void *arg);
static void APPE_Button1Action_Entry(unsigned long thread_input);
static void APPE_Button2Action_Entry(unsigned long thread_input);
static void APPE_Button3Action_Entry(unsigned long thread_input);
#endif
/* Section specific to button management using UART */
static void RxUART_Init(void);
static void RxCpltCallback(uint8_t *pdata, uint16_t size, uint8_t error);
static void UartCmdExecute(void);
/* USER CODE END PFP */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */
/* Section specific to button management using UART */
extern uint8_t charRx;
/* USER CODE END EV */

/* Functions Definition ------------------------------------------------------*/
void MX_APPE_Config(void)
{
  /* Configure HSE Tuning */
  Config_HSE();
}

uint32_t MX_APPE_Init(void *p_param)
{
  APP_DEBUG_SIGNAL_SET(APP_APPE_INIT);

  /* save ThreadX byte pool for whole WPAN middleware */
  pBytePool = p_param;

  /* System initialization */
  System_Init();

  /* Configure the system Power Mode */
  SystemPower_Config();

  /* Initialize the Advance Memory Manager */
  AMM_Init (&ammInitConfig);

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
  /* Section specific to button management using UART */
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

  RNG_Init();

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

  /* Disable RFTS Bypass for flash operation - Since LL has not started yet */
  FD_SetStatus (FD_FLASHACCESS_RFTS_BYPASS, LL_FLASH_DISABLE);

/* USER CODE BEGIN APPE_Init_2 */

/* USER CODE END APPE_Init_2 */
  APP_DEBUG_SIGNAL_RESET(APP_APPE_INIT);
  return WPAN_SUCCESS;
}

/* USER CODE BEGIN FD */
#if (CFG_BUTTON_SUPPORTED == 1)
/**
 * @brief   Indicate if the selected button was pressedn during a 'long time' or not.
 *
 * @param   btnIdx    Button to test, listed in enum Button_TypeDef
 * @return  '1' if pressed during a 'long time', else '0'.
 */
uint8_t APPE_ButtonIsLongPressed(uint16_t btnIdx)
{
  uint8_t pressStatus;

  if ( btnIdx < BUTTON_NB_MAX )
  {
    pressStatus = buttonDesc[btnIdx].longPressed;
  }
  else
  {
    pressStatus = 0;
  }

  return pressStatus;
}

/**
 * @brief  Action of button 1 when pressed, to be implemented by user.
 * @param  None
 * @retval None
 */
__WEAK void APPE_Button1Action(void)
{
}

/**
 * @brief  Action of button 2 when pressed, to be implemented by user.
 * @param  None
 * @retval None
 */
__WEAK void APPE_Button2Action(void)
{
}

/**
 * @brief  Action of button 3 when pressed, to be implemented by user.
 * @param  None
 * @retval None
 */
__WEAK void APPE_Button3Action(void)
{
}
#endif

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

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
  UTIL_ADV_TRACE_SetVerboseLevel(VLEVEL_L); /* functional traces*/
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

#if (CFG_LPM_SUPPORTED == 1)
 /* Initialize low power manager */
  UTIL_LPM_Init();

#if (CFG_LPM_STDBY_SUPPORTED == 1)
  /* Enable SRAM1, SRAM2 and RADIO retention*/
  LL_PWR_SetSRAM1SBRetention(LL_PWR_SRAM1_SB_FULL_RETENTION);
  LL_PWR_SetSRAM2SBRetention(LL_PWR_SRAM2_SB_FULL_RETENTION);
  LL_PWR_SetRadioSBRetention(LL_PWR_RADIO_SB_FULL_RETENTION); /* Retain sleep timer configuration */
#else
  UTIL_LPM_SetOffMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);
#endif /* CFG_LPM_STDBY_SUPPORTED */
#endif /* CFG_LPM_SUPPORTED */
}

/**
 * @brief Initialize Random Number Generator module
 */
static void RNG_Init(void)
{
  CHAR * pStack;

  HW_RNG_Start();

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

  return;
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
  buttonDesc[B1].button = B1;
  buttonDesc[B2].button = B2;
  buttonDesc[B3].button = B3;
  BSP_PB_Init(B1, BUTTON_MODE_EXTI);
  BSP_PB_Init(B2, BUTTON_MODE_EXTI);
  BSP_PB_Init(B3, BUTTON_MODE_EXTI);

  CHAR * pStack;

  /* Register tasks associated to buttons */
  if (tx_byte_allocate(pBytePool, (void **) &pStack, PB1_BUTTON_PUSHED_TASK_STACK_SIZE,TX_NO_WAIT) != TX_SUCCESS)
  {
    Error_Handler();
  }
  if (tx_semaphore_create(&PB1_BUTTON_PUSHED_Thread_Sem, "PB1_BUTTON_PUSHED_Thread_Sem", 0)!= TX_SUCCESS )
  {
    Error_Handler();
  }
  if (tx_thread_create(&PB1_BUTTON_PUSHED_Thread, "PB1_BUTTON_PUSHED Thread", APPE_Button1Action_Entry, 0,
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
  if (tx_thread_create(&PB2_BUTTON_PUSHED_Thread, "PB2_BUTTON_PUSHED Thread", APPE_Button2Action_Entry, 0,
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
  if (tx_thread_create(&PB3_BUTTON_PUSHED_Thread, "PB3_BUTTON_PUSHED Thread", APPE_Button3Action_Entry, 0,
                         pStack, PB3_BUTTON_PUSHED_TASK_STACK_SIZE,
                         PB3_BUTTON_PUSHED_TASK_PRIO, PB3_BUTTON_PUSHED_TASK_PREEM_TRES,
                         TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
  {
    Error_Handler();
  }

  /* Create timers to detect button long press (one for each button) */
  Button_TypeDef buttonIndex;
  for ( buttonIndex = B1; buttonIndex < BUTTON_NB_MAX; buttonIndex++ )
  {
    UTIL_TIMER_Create( &buttonDesc[buttonIndex].longTimerId,
                       0,
                       (UTIL_TIMER_Mode_t)hw_ts_SingleShot,
                       &Button_TriggerActions,
                       &buttonDesc[buttonIndex] );
  }

  return;
}

static void Button_TriggerActions(void *arg)
{
  ButtonDesc_t *p_buttonDesc = arg;

  p_buttonDesc->longPressed = BSP_PB_GetState(p_buttonDesc->button);

  APP_DBG_MSG("Button %d pressed\n", (p_buttonDesc->button + 1));
  switch (p_buttonDesc->button)
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

static void APPE_Button1Action_Entry(unsigned long thread_input)
{
  (void)(thread_input);
  
  while(1)
  {
    tx_semaphore_get(&PB1_BUTTON_PUSHED_Thread_Sem, TX_WAIT_FOREVER);
    APPE_Button1Action();
    tx_thread_relinquish();
  }
}

static void APPE_Button2Action_Entry(unsigned long thread_input)
{
  (void)(thread_input);
  
  while(1)
  {
    tx_semaphore_get(&PB2_BUTTON_PUSHED_Thread_Sem, TX_WAIT_FOREVER);
    APPE_Button2Action();
    tx_thread_relinquish();
  }
}

static void APPE_Button3Action_Entry(unsigned long thread_input)
{
  (void)(thread_input);
  
  while(1)
  {
    tx_semaphore_get(&PB3_BUTTON_PUSHED_Thread_Sem, TX_WAIT_FOREVER);
    APPE_Button3Action();
    tx_thread_relinquish();
  }
}
#endif

/* Section specific to button management using UART */
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
#if (CFG_BUTTON_SUPPORTED == 1)
    buttonDesc[B1].longPressed = 0;
    tx_semaphore_put(&PB1_BUTTON_PUSHED_Thread_Sem);
#endif
  }
  else if (strcmp((char const*)CommandString, "SW2") == 0)
  {
    APP_DBG_MSG("SW2 OK\n");
#if (CFG_BUTTON_SUPPORTED == 1)
    buttonDesc[B2].longPressed = 0;
    tx_semaphore_put(&PB2_BUTTON_PUSHED_Thread_Sem);
#endif
  }
  else if (strcmp((char const*)CommandString, "SW3") == 0)
  {
    APP_DBG_MSG("SW3 OK\n");
#if (CFG_BUTTON_SUPPORTED == 1)
    buttonDesc[B3].longPressed = 0;
    tx_semaphore_put(&PB3_BUTTON_PUSHED_Thread_Sem);
#endif
  }
  else if(strcmp((char const*)CommandString, "SW1_LONG") == 0)
  {
    APP_DBG_MSG("SW1_LONG OK\n");
#if (CFG_BUTTON_SUPPORTED == 1)
    buttonDesc[B1].longPressed = 1;
    tx_semaphore_put(&PB1_BUTTON_PUSHED_Thread_Sem);
#endif
  }
  else if (strcmp((char const*)CommandString, "SW2_LONG") == 0)
  {
    APP_DBG_MSG("SW2_LONG OK\n");
#if (CFG_BUTTON_SUPPORTED == 1)
    buttonDesc[B2].longPressed = 1;
    tx_semaphore_put(&PB2_BUTTON_PUSHED_Thread_Sem);
#endif
  }
  else if (strcmp((char const*)CommandString, "SW3_LONG") == 0)
  {
    APP_DBG_MSG("SW3_LONG OK\n");
#if (CFG_BUTTON_SUPPORTED == 1)
    buttonDesc[B3].longPressed = 1;
    tx_semaphore_put(&PB3_BUTTON_PUSHED_Thread_Sem);
#endif
  }
  else
  {
    APP_DBG_MSG("NOT RECOGNIZED COMMAND : %s\n", CommandString);
  }
}
/* USER CODE END FD_LOCAL_FUNCTIONS */

/*************************************************************
 *
 * WRAP FUNCTIONS
 *
 *************************************************************/
void ThreadXLowPowerUserEnter( void )
{
#if ( CFG_LPM_SUPPORTED == 1)
  LL_PWR_ClearFlag_STOP();

  LL_RCC_ClearResetFlags();

  /* Wait until HSE is ready */
  while (LL_RCC_HSE_IsReady() == 0);

  UTILS_ENTER_LIMITED_CRITICAL_SECTION(RCC_INTR_PRIO<<4);
  scm_hserdy_isr();
  UTILS_EXIT_LIMITED_CRITICAL_SECTION();
  HAL_SuspendTick();
  /* Disable SysTick Interrupt */
  SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
  UTIL_LPM_EnterLowPower();
#endif /* CFG_LPM_SUPPORTED */
  return;
}

void ThreadXLowPowerUserExit( void )
{
#if ( CFG_LPM_SUPPORTED == 1)
  HAL_ResumeTick();
  /* Enable SysTick Interrupt */
  SysTick->CTRL  |= SysTick_CTRL_TICKINT_Msk;
  LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_RADIO);
  ll_sys_dp_slp_exit();
#endif /* CFG_LPM_SUPPORTED */
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
    tx_thread_relinquish();
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
    tx_thread_relinquish();
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
    tx_thread_relinquish();
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
    tx_thread_relinquish();
  }
}

#if (CFG_DEBUG_APP_TRACE != 0)
void RNG_KERNEL_CLK_OFF(void)
{
  /* RNG module may not switch off HSI clock when traces are used */

  /* USER CODE BEGIN RNG_KERNEL_CLK_OFF */

  /* USER CODE END RNG_KERNEL_CLK_OFF */
}

#endif

/* USER CODE BEGIN FD_WRAP_FUNCTIONS */
#if (CFG_BUTTON_SUPPORTED == 1)
void BSP_PB_Callback(Button_TypeDef Button)
{
  buttonDesc[Button].longPressed = 0;
  UTIL_TIMER_StartWithPeriod(&buttonDesc[Button].longTimerId, BUTTON_LONG_PRESS_THRESHOLD_MS);

  return;
}
#endif

/* USER CODE END FD_WRAP_FUNCTIONS */
