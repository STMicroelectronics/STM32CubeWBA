/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_entry.c
  * @author  MCD Application Team
  * @brief   Entry point of the application
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
#include "app_common.h"
#include "app_conf.h"
#include "main.h"
#include "app_entry.h"
#if (CFG_LPM_SUPPORTED == 1)
#include "stm32_lpm.h"
#endif /* CFG_LPM_SUPPORTED */
#include "stm32_timer.h"
#if (CFG_LOG_SUPPORTED != 0)
#include "serial_cmd_interpreter.h"
#endif /* CFG_LOG_SUPPORTED */
#include "app_thread.h"
#include "otp.h"
#include "scm.h"
#include "stm32_rtos.h"
#include "ll_sys.h"

/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32wbaxx_nucleo.h"
#include "serial_cmd_interpreter.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */
#if (CFG_BUTTON_SUPPORTED == 1)
typedef struct
{
  Button_TypeDef      button;
  UTIL_TIMER_Object_t longTimerId;
  uint8_t             longPressed;
  uint32_t            waitingTime;
} ButtonDesc_t;
#endif /* (CFG_BUTTON_SUPPORTED == 1) */
/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN PD */
#if (CFG_BUTTON_SUPPORTED == 1)

#define BUTTON_LONG_PRESS_SAMPLE_MS           (50u)         // Sample Button every 50ms.
#define BUTTON_LONG_PRESS_THRESHOLD_MS        (500u)        // Normally 500ms if we use 'Long pression' on button.
#define BUTTON_NB_MAX                         (B3 + 1u)

/* Push Button SW1 Task related defines */
#define TASK_BUTTON_SW1_STACK_SIZE            RTOS_STACK_SIZE_NORMAL
#define TASK_BUTTON_SW1_PRIORITY              CFG_TASK_PRIO_BUTTON_SWx
/* Push Button SW2 Task related defines */
#define TASK_BUTTON_SW2_STACK_SIZE            RTOS_STACK_SIZE_NORMAL
#define TASK_BUTTON_SW2_PRIORITY              CFG_TASK_PRIO_BUTTON_SWx

/* Push Button SW3 Task related defines */
#define TASK_BUTTON_SW3_STACK_SIZE            RTOS_STACK_SIZE_NORMAL
#define TASK_BUTTON_SW3_PRIORITY              CFG_TASK_PRIO_BUTTON_SWx

#endif /* (CFG_BUTTON_SUPPORTED == 1) */
/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private constants ---------------------------------------------------------*/

/* USER CODE BEGIN PC */

/* USER CODE END PC */

/* Private variables ---------------------------------------------------------*/
#if ( CFG_LPM_SUPPORTED == 1)
static bool system_startup_done = FALSE;
#endif /* ( CFG_LPM_SUPPORTED == 1) */

#if (CFG_LOG_SUPPORTED != 0)
/* Log configuration */
static Log_Module_t Log_Module_Config = { .verbose_level = APPLI_CONFIG_LOG_LEVEL, .region = LOG_REGION_ALL_REGIONS };
#endif /* (CFG_LOG_SUPPORTED != 0) */

TX_SEMAPHORE          RandomProcessSemaphore;
TX_THREAD             RandomProcessThread;

/* USER CODE BEGIN PV */
#if (CFG_BUTTON_SUPPORTED == 1)
/* Button management */
TX_SEMAPHORE          ButtonSw1Semaphore, ButtonSw2Semaphore, ButtonSw3Semaphore;
TX_THREAD             ButtonSw1Thread, ButtonSw2Thread, ButtonSw3Thread;
static ButtonDesc_t   buttonDesc[BUTTON_NB_MAX] = { { B1, 0, 0 }, { B2, 0, 0 }, { B3, 0, 0 } } ;
#endif /* (CFG_BUTTON_SUPPORTED == 1) */

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/
/* ThreadX byte pool pointer for whole WPAN middleware */
TX_BYTE_POOL  * pBytePool;
CHAR          * pStack;

/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private functions prototypes-----------------------------------------------*/
static void Config_HSE(void);
static void RNG_Init( void );
static void System_Init( void );
static void SystemPower_Config( void );

/* USER CODE BEGIN PFP */
#if (CFG_LED_SUPPORTED == 1)
static void Led_Init                      ( void );
#endif /* (CFG_LED_SUPPORTED == 1) */
#if (CFG_BUTTON_SUPPORTED == 1)
static void Button_Init                   ( void );
static void Button_TriggerActions         ( void * arg );
#endif /* (CFG_BUTTON_SUPPORTED == 1) */
/* USER CODE END PFP */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */
/* USER CODE END EV */

/* Functions Definition ------------------------------------------------------*/
/**
 * @brief   System Initialisation.
 */
void MX_APPE_Config(void)
{
  /* Configure HSE Tuning */
  Config_HSE();
}

/**
 * @brief   System Initialisation.
 */
uint32_t MX_APPE_Init(void *p_param)
{ 
  APP_DEBUG_SIGNAL_SET(APP_APPE_INIT);

  /* System initialization */
  System_Init();

  /* Configure the system Power Mode */
  SystemPower_Config();
  
  /* Save ThreadX byte pool for whole WPAN middleware */
  pBytePool = p_param;

  /* USER CODE BEGIN APPE_Init_1 */
#if (CFG_LED_SUPPORTED == 1)
  Led_Init();
#endif /* (CFG_LED_SUPPORTED == 1) */
#if (CFG_BUTTON_SUPPORTED == 1)
  Button_Init();
#endif /* (CFG_BUTTON_SUPPORTED == 1) */

  /* USER CODE END APPE_Init_1 */

  RNG_Init();

  /* Thread Initialisation */
  APP_THREAD_Init();
  ll_sys_config_params();

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
  /* Initialize the logs ( using the USART ) */
  Log_Module_Init( Log_Module_Config );
  Log_Module_Set_Region( LOG_REGION_APP );
  Log_Module_Add_Region( LOG_REGION_THREAD );

  /* Initialize the Command Interpreter */
  Serial_CMD_Interpreter_Init();
#endif  /* (CFG_LOG_SUPPORTED != 0) */

#if ( CFG_LPM_SUPPORTED == 1)
  system_startup_done = TRUE;
#endif /* ( CFG_LPM_SUPPORTED == 1) */

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
  /* Initialize System Clock Manager */
  scm_init();

#if (CFG_LPM_SUPPORTED == 1)
  /* Initialize low Power Manager. By default enabled */
  UTIL_LPM_Init();

#if (CFG_LPM_STDBY_SUPPORTED == 1)
  /* Enable SRAM1, SRAM2 and RADIO retention*/
  LL_PWR_SetSRAM1SBRetention(LL_PWR_SRAM1_SB_FULL_RETENTION);
  LL_PWR_SetSRAM2SBRetention(LL_PWR_SRAM2_SB_FULL_RETENTION);
  LL_PWR_SetRadioSBRetention(LL_PWR_RADIO_SB_FULL_RETENTION); /* Retain sleep timer configuration */

#endif /* (CFG_LPM_STDBY_SUPPORTED == 1) */

  /* Disable LowPower during Init */
  UTIL_LPM_SetStopMode(1U << CFG_LPM_APP, UTIL_LPM_DISABLE);
  UTIL_LPM_SetOffMode(1U << CFG_LPM_APP, UTIL_LPM_DISABLE);
#endif /* (CFG_LPM_SUPPORTED == 1)  */
}


static void HW_RNG_Process_Task( ULONG lArgument )
{
  UNUSED( lArgument );
  
  for(;;)
  {
    tx_semaphore_get( &RandomProcessSemaphore, TX_WAIT_FOREVER );
    HW_RNG_Process();
  }
}

/**
 * @brief Initialize Random Number Generator module
 */
static void RNG_Init(void)
{
  UINT        ThreadXStatus;
  
  HW_RNG_Start( );
  
  /* Register Semaphore to launch the Random Process */
  ThreadXStatus = tx_semaphore_create( &RandomProcessSemaphore, "RandomProcess Semaphore", 0 );
  
  /* Create the Random Process Thread and this Stack */
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus |= tx_byte_allocate( pBytePool, (VOID**) &pStack, TASK_HW_RNG_STACK_SIZE, TX_NO_WAIT);
  }
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus |= tx_thread_create( &RandomProcessThread, "RandomProcess Thread", HW_RNG_Process_Task, 0, pStack,
                                       TASK_HW_RNG_STACK_SIZE, CFG_TASK_PRIO_HW_RNG, CFG_TASK_PRIO_HW_RNG,
                                       TX_NO_TIME_SLICE, TX_AUTO_START);
  }
  
  /* Verify if it's OK */
  if ( ThreadXStatus != TX_SUCCESS )
  { 
    APP_DBG( "ERROR THREADX : RANDOM PROCESS THREAD CREATION FAILED (%d)", ThreadXStatus );
    while(1);
  }
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */
#if (CFG_LED_SUPPORTED == 1)
static void Led_Init( void )
{
  /* Leds Initialization */
  BSP_LED_Init(LED_BLUE);
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_RED);

  return;
}
#endif /* CFG_LED_SUPPORTED */

#if (CFG_BUTTON_SUPPORTED == 1)

/**
 * @brief  Management of the SW1 pushbutton task
 * @param  lArgument  Not used.
 * @retval None
 */
static void ButtonSw1Task( ULONG lArgument )
{
  UNUSED( lArgument );
  
  for(;;)
  {
    tx_semaphore_get( &ButtonSw1Semaphore, TX_WAIT_FOREVER );
    APP_THREAD_Button1Action();
  }
}
    
/**
 * @brief  Management of the SW2 pushbutton task
 * @param  lArgument  Not used.
 * @retval None
 */
static void ButtonSw2Task( ULONG lArgument )
{
  UNUSED( lArgument );
  
  for(;;)
  {
    tx_semaphore_get( &ButtonSw2Semaphore, TX_WAIT_FOREVER );
    APP_THREAD_Button2Action();
  }
}

/**
 * @brief  Management of the SW3 pushbutton task
 * @param  lArgument  Not used.
 * @retval None
 */
static void ButtonSw3Task( ULONG lArgument )
{
  UNUSED( lArgument );
  
  for(;;)
  {
    tx_semaphore_get( &ButtonSw3Semaphore, TX_WAIT_FOREVER );
    APP_THREAD_Button3Action();
  }
}

static void Button_InitTask( void )
{
  UINT        ThreadXStatus;
  
  /* Register Semaphore to launch the pushbutton SW1 Task */
  ThreadXStatus = tx_semaphore_create( &ButtonSw1Semaphore, "ButtonSw1 Semaphore", 0 );
  
  /* Create the pushbutton SWx Thread and this Stack */
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus = tx_byte_allocate( pBytePool, (VOID**) &pStack, TASK_BUTTON_SW1_STACK_SIZE, TX_NO_WAIT);
  }
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus = tx_thread_create(  &ButtonSw1Thread, "ButtonSw1 Thread", ButtonSw1Task, 0, pStack,
                                        TASK_BUTTON_SW1_STACK_SIZE, TASK_BUTTON_SW1_PRIORITY, TASK_BUTTON_SW1_PRIORITY,
                                        TX_NO_TIME_SLICE, TX_AUTO_START );
  }
  
  /* Verify if it's OK */
  if ( ThreadXStatus != TX_SUCCESS )
  { 
    APP_DBG( "ERROR THREADX : PUSH BUTTON SW1 THREAD CREATION FAILED (0x%04X)", ThreadXStatus );
    while(1);
  }
  
  
  /* Register Semaphore to launch the pushbutton SW2 Task */
  ThreadXStatus = tx_semaphore_create( &ButtonSw2Semaphore, "ButtonSw2 Semaphore", 0 );
  
  /* Create the pushbutton SW2 Thread and this Stack */
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus = tx_byte_allocate( pBytePool, (VOID**) &pStack, TASK_BUTTON_SW2_STACK_SIZE, TX_NO_WAIT);
  }
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus = tx_thread_create(  &ButtonSw2Thread, "ButtonSw2 Thread", ButtonSw2Task, 0, pStack,
                                        TASK_BUTTON_SW2_STACK_SIZE, TASK_BUTTON_SW2_PRIORITY, TASK_BUTTON_SW2_PRIORITY,
                                        TX_NO_TIME_SLICE, TX_AUTO_START );
  }
  
  /* Verify if it's OK */
  if ( ThreadXStatus != TX_SUCCESS )
  { 
    APP_DBG( "ERROR THREADX : PUSH BUTTON SW2 THREAD CREATION FAILED (0x%04X)", ThreadXStatus );
    while(1);
  }
  
  
  /* Register Semaphore to launch the pushbutton SW3 Task */
  ThreadXStatus = tx_semaphore_create( &ButtonSw3Semaphore, "ButtonSw3 Semaphore", 0 );
  
  /* Create the pushbutton SW3 Thread and this Stack */
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus = tx_byte_allocate( pBytePool, (VOID**) &pStack, TASK_BUTTON_SW3_STACK_SIZE, TX_NO_WAIT);
  }
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus = tx_thread_create(  &ButtonSw3Thread, "ButtonSw3 Thread", ButtonSw3Task, 0, pStack,
                                        TASK_BUTTON_SW3_STACK_SIZE, TASK_BUTTON_SW3_PRIORITY, TASK_BUTTON_SW3_PRIORITY,
                                        TX_NO_TIME_SLICE, TX_AUTO_START );
  }
  
  /* Verify if it's OK */
  if ( ThreadXStatus != TX_SUCCESS )
  { 
    APP_DBG( "ERROR THREADX : PUSH BUTTON SW3 THREAD CREATION FAILED (0x%04X)", ThreadXStatus );
    while(1);
  }
}

static void Button_Init( void )
{
  Button_TypeDef  buttonIndex;
  
  /* Button HW Initialization */
  BSP_PB_Init(B1, BUTTON_MODE_EXTI);
  BSP_PB_Init(B2, BUTTON_MODE_EXTI);
  BSP_PB_Init(B3, BUTTON_MODE_EXTI);

  /* Button task initialisation */
  Button_InitTask();
  
  /* Button timers initialisation (one for each button) */
  for ( buttonIndex = B1; buttonIndex < BUTTON_NB_MAX; buttonIndex++ )
  { 
    UTIL_TIMER_Create( &buttonDesc[buttonIndex].longTimerId, 0, (UTIL_TIMER_Mode_t)hw_ts_Repeated, &Button_TriggerActions, &buttonDesc[buttonIndex] ); 
  }
}

/**
 *
 */
static void Button_TriggerActions( void * arg )
{
  ButtonDesc_t  * p_buttonDesc = arg;
  int32_t       buttonState;

  buttonState = BSP_PB_GetState( p_buttonDesc->button );

  /* If Button pressed and Threshold time not finish, continue waiting */
  p_buttonDesc->waitingTime += BUTTON_LONG_PRESS_SAMPLE_MS;
  if ( ( buttonState == 1 ) && ( p_buttonDesc->waitingTime < BUTTON_LONG_PRESS_THRESHOLD_MS ) )
  {
    return;
  }

  /* Save button state */
  p_buttonDesc->longPressed = buttonState;

  /* Stop Timer */
  UTIL_TIMER_Stop( &p_buttonDesc->longTimerId );
  
  switch ( p_buttonDesc->button )
  {
    case B1:
        tx_semaphore_put(&ButtonSw1Semaphore);
        break;
    
    case B2:
        tx_semaphore_put(&ButtonSw2Semaphore);
        break;
    
    case B3:
        tx_semaphore_put(&ButtonSw3Semaphore);
        break;
        
    default:
        break;
  }
}

#endif /* (CFG_BUTTON_SUPPORTED == 1) */

/* USER CODE END FD_LOCAL_FUNCTIONS */

/*************************************************************
 *
 * WRAP FUNCTIONS
 *
 *************************************************************/
/**
 * @brief Callback used by 'Random Generator' to launch Task to generate Random Numbers
 */
void HWCB_RNG_Process( void )
{
  if (RandomProcessSemaphore.tx_semaphore_count == 0)
  {
    tx_semaphore_put(&RandomProcessSemaphore);
  }
}

#if (CFG_LOG_SUPPORTED != 0)
/**
 *
 */
void RNG_KERNEL_CLK_OFF(void)
{
  /* RNG module may not switch off HSI clock when traces are used */

  /* USER CODE BEGIN RNG_KERNEL_CLK_OFF */

  /* USER CODE END RNG_KERNEL_CLK_OFF */
}

#endif /* (CFG_LOG_SUPPORTED != 0) */

/* USER CODE BEGIN FD_WRAP_FUNCTIONS */
#if ( CFG_BUTTON_SUPPORTED == 1 ) 

/**
 *
 */
void BSP_PB_Callback( Button_TypeDef button )
{
  buttonDesc[button].longPressed = 0;
  buttonDesc[button].waitingTime = 0;
  UTIL_TIMER_StartWithPeriod( &buttonDesc[button].longTimerId, BUTTON_LONG_PRESS_SAMPLE_MS );
}

#endif /* ( CFG_BUTTON_SUPPORTED == 1 )  */

/* USER CODE END FD_WRAP_FUNCTIONS */
