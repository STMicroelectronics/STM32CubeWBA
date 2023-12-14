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
#include "app_zigbee.h"
#include "app_entry.h"
#if (CFG_LPM_LEVEL != 0)
#include "app_sys.h"
#include "stm32_lpm.h"
#endif /* (CFG_LPM_LEVEL != 0) */
#include "stm32_timer.h"
#include "stm32_mm.h"
#if (CFG_LOG_SUPPORTED != 0)
#include "stm32_adv_trace.h"
#include "serial_cmd_interpreter.h"
#endif /* CFG_LOG_SUPPORTED */
#include "otp.h"
#include "scm.h"
#include "stm32_rtos.h"
#include "stm32wbaxx_ll_rcc.h"

/* Private includes -----------------------------------------------------------*/
extern void ll_sys_mac_cntrl_init( void );
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
/* Heap size for System (used by Low-Layers) */
#define C_SYS_MEMORY_HEAP_SIZE_BYTES          60000U

/* USER CODE BEGIN PD */
#if (CFG_BUTTON_SUPPORTED == 1)
#define BUTTON_LONG_PRESS_SAMPLE_MS           (50u)         // Sample Button every 50ms.
#define BUTTON_LONG_PRESS_THRESHOLD_MS        (500u)        // Normally 500ms if we use 'Long pression' on button.
#define BUTTON_NB_MAX                         (B3 + 1u)
#endif /* (CFG_BUTTON_SUPPORTED == 1) */

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private constants ---------------------------------------------------------*/
/* USER CODE BEGIN PC */

/* USER CODE END PC */

/* Private variables ---------------------------------------------------------*/
static uint8_t        SYS_MEMORY_HEAP[C_SYS_MEMORY_HEAP_SIZE_BYTES];

#if ( CFG_LPM_LEVEL != 0)
static bool system_startup_done = FALSE;
#endif /* ( CFG_LPM_LEVEL != 0) */

#if (CFG_LOG_SUPPORTED != 0)
/* Log configuration */
static Log_Module_t Log_Module_Config = { .verbose_level = APPLI_CONFIG_LOG_LEVEL, .region = LOG_REGION_ALL_REGIONS };
#endif /* (CFG_LOG_SUPPORTED != 0) */

/* USER CODE BEGIN PV */
#if (CFG_BUTTON_SUPPORTED == 1)
/* Button management */
static ButtonDesc_t   buttonDesc[BUTTON_NB_MAX] = { { B1, { 0 }, 0, 0 } , { B2, { 0 } , 0, 0 }, { B3, { 0 }, 0, 0 } };
#endif /* (CFG_BUTTON_SUPPORTED == 1) */

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/
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
 *
 */
void MX_APPE_LinkLayerInit(void)
{
  /* Initialization of the low level : link layer and MAC */
  ll_sys_mac_cntrl_init();

}

/**
 * @brief   System Initialisation.
 */
uint32_t MX_APPE_Init(void *p_param)
{
  APP_DEBUG_SIGNAL_SET(APP_APPE_INIT);

  UNUSED(p_param);

  /* System initialization */
  System_Init();

  /* Configure the system Power Mode */
  SystemPower_Config();

  /* USER CODE BEGIN APPE_Init_1 */
#if (CFG_LED_SUPPORTED == 1)  
  Led_Init();
#endif /* (CFG_LED_SUPPORTED == 1) */
#if (CFG_BUTTON_SUPPORTED == 1)
  Button_Init();
#endif /* (CFG_BUTTON_SUPPORTED == 1) */

  /* USER CODE END APPE_Init_1 */

  RNG_Init();

  /* Initialization of the low level : link layer and MAC */
  MX_APPE_LinkLayerInit();

  /* Initialization of the Zigbee Application */
  APP_ZIGBEE_ApplicationInit();

  /* USER CODE BEGIN APPE_Init_2 */

  /* USER CODE END APPE_Init_2 */
  APP_DEBUG_SIGNAL_RESET(APP_APPE_INIT);
  return WPAN_SUCCESS;
}

/* USER CODE BEGIN FD */
#if ( CFG_BUTTON_SUPPORTED == 1 ) 

/**
 * @brief   Indicate if the selected button was pressedn during a 'long time' or not.
 *
 * @param   btnIdx    Button to test, listed in enum Button_TypeDef
 * @return  '1' if pressed during a 'long time', else '0'.
 */
uint8_t APPE_ButtonIsLongPressed( uint16_t btnIdx )
{
  uint8_t pressStatus = 0;

  if ( btnIdx < BUTTON_NB_MAX )
  {
    pressStatus = buttonDesc[btnIdx].longPressed;
  }

  return pressStatus;
}

/**
 * @brief  Action of button 1 when pressed, to be implemented by user.
 * @param  None
 * @retval None
 */
__WEAK void APPE_Button1Action( void )
{
}

/**
 * @brief  Action of button 2 when pressed, to be implemented by user.
 * @param  None
 * @retval None
 */
__WEAK void APPE_Button2Action( void )
{
}

/**
 * @brief  Action of button 3 when pressed, to be implemented by user.
 * @param  None
 * @retval None
 */
__WEAK void APPE_Button3Action( void )
{
}

#endif /* ( CFG_BUTTON_SUPPORTED == 1 )  */

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
  /* Initialize System Heap used by Zigbee Stack */
  UTIL_MM_Init( SYS_MEMORY_HEAP, C_SYS_MEMORY_HEAP_SIZE_BYTES );

  /* Clear RCC RESET flag */
  LL_RCC_ClearResetFlags();

  UTIL_TIMER_Init();

  /* Enable wakeup out of standby from RTC ( UTIL_TIMER )*/
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN7_HIGH_3);

#if (CFG_LOG_SUPPORTED != 0)
  /* Initialize the logs ( using the USART ) */
  Log_Module_Init( Log_Module_Config );
  Log_Module_Set_Region( LOG_REGION_APP );
  Log_Module_Add_Region( LOG_REGION_ZIGBEE );

  /* Initialize the Command Interpreter */
  Serial_CMD_Interpreter_Init();
#endif  /* (CFG_LOG_SUPPORTED != 0) */

#if ( CFG_LPM_LEVEL != 0)
  system_startup_done = TRUE;
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
  HAL_GPIO_Init(GPIOA, &DbgIOsInit);

  DbgIOsInit.Mode = GPIO_MODE_ANALOG;
  DbgIOsInit.Pull = GPIO_NOPULL;
  DbgIOsInit.Pin = GPIO_PIN_3|GPIO_PIN_4;
  HAL_GPIO_Init(GPIOB, &DbgIOsInit);
#endif /* CFG_DEBUGGER_LEVEL */

  /* Configure Vcore supply */
  if ( HAL_PWREx_ConfigSupply( CFG_CORE_SUPPLY ) != HAL_OK )
  {
    Error_Handler();
  }

#if (CFG_SCM_SUPPORTED == 1)
  /* Set the HSE clock to 32MHz */
  scm_setsystemclock(SCM_USER_APP, HSE_32MHZ);
#endif /* CFG_SCM_SUPPORTED */

#if (CFG_LPM_LEVEL != 0)
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
#endif /* (CFG_LPM_LEVEL != 0)  */
}

/**
 * @brief Initialize Random Number Generator module
 */
static void RNG_Init(void)
{
  HW_RNG_Start();

  UTIL_SEQ_RegTask(1U << CFG_TASK_HW_RNG, UTIL_SEQ_RFU, (void (*)(void))HW_RNG_Process);
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */
#if ( CFG_LED_SUPPORTED == 1 )

static void Led_Init( void )
{
  /* Leds Initialization */
  BSP_LED_Init(LED_BLUE);
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_RED);

  APP_LED_ON(LED_GREEN);
}

#endif // (CFG_LED_SUPPORTED == 1)
#if ( CFG_BUTTON_SUPPORTED == 1 )

static void Button_InitTask( void )
{
  /* Task associated with push button SW1 */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_BUTTON_SW1, UTIL_SEQ_RFU, APPE_Button1Action );
  
  /* Task associated with push button SW2 */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_BUTTON_SW2, UTIL_SEQ_RFU, APPE_Button2Action );
    
  /* Task associated with push button SW3 */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_BUTTON_SW3, UTIL_SEQ_RFU, APPE_Button3Action );
}


static void Button_Init( void )
{
  Button_TypeDef  buttonIndex;
  
  /* Buttons HW Initialization */
  BSP_PB_Init( B1, BUTTON_MODE_EXTI );
  BSP_PB_Init( B2, BUTTON_MODE_EXTI );
  BSP_PB_Init( B3, BUTTON_MODE_EXTI );

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
        UTIL_SEQ_SetTask( 1U << CFG_TASK_BUTTON_SW1, CFG_TASK_PRIO_BUTTON_SWx );
        break;
    
    case B2:
        UTIL_SEQ_SetTask( 1U << CFG_TASK_BUTTON_SW2, CFG_TASK_PRIO_BUTTON_SWx );
        break;
    
    case B3:
        UTIL_SEQ_SetTask( 1U << CFG_TASK_BUTTON_SW3, CFG_TASK_PRIO_BUTTON_SWx );
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
void MX_APPE_Process(void)
{
  /* USER CODE BEGIN MX_APPE_Process_1 */

  /* USER CODE END MX_APPE_Process_1 */
  UTIL_SEQ_Run(UTIL_SEQ_DEFAULT);
  /* USER CODE BEGIN MX_APPE_Process_2 */

  /* USER CODE END MX_APPE_Process_2 */
}

void UTIL_SEQ_Idle( void )
{
#if ( CFG_LPM_LEVEL != 0)
  HAL_SuspendTick();
  UTIL_LPM_EnterLowPower();
  HAL_ResumeTick();
#endif /* CFG_LPM_LEVEL */
  return;
}

void UTIL_SEQ_PreIdle( void )
{
  /* USER CODE BEGIN UTIL_SEQ_PreIdle_1 */

  /* USER CODE END UTIL_SEQ_PreIdle_1 */
#if ( CFG_LPM_LEVEL != 0)
  LL_PWR_ClearFlag_STOP();

  if ( ( system_startup_done != FALSE ) && ( UTIL_LPM_GetMode() == UTIL_LPM_OFFMODE ) )
  {
    APP_SYS_LPM_EnterLowPowerMode();
  }

  LL_RCC_ClearResetFlags();

  /* Wait until System clock is not on HSI */
  while (LL_RCC_GetSysClkSource() == LL_RCC_SYS_CLKSOURCE_STATUS_HSI);

#if defined(STM32WBAXX_SI_CUT1_0)
  /* Wait until HSE is ready */
  while (LL_RCC_HSE_IsReady() == 0);

  UTILS_ENTER_LIMITED_CRITICAL_SECTION(RCC_INTR_PRIO << 4U);
  scm_hserdy_isr();
  UTILS_EXIT_LIMITED_CRITICAL_SECTION();
#endif /* STM32WBAXX_SI_CUT1_0 */
#endif /* CFG_LPM_LEVEL */
  /* USER CODE BEGIN UTIL_SEQ_PreIdle_2 */

  /* USER CODE END UTIL_SEQ_PreIdle_2 */
  return;
}

void UTIL_SEQ_PostIdle( void )
{
  /* USER CODE BEGIN UTIL_SEQ_PostIdle_1 */

  /* USER CODE END UTIL_SEQ_PostIdle_1 */
#if ( CFG_LPM_LEVEL != 0)
  LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_RADIO);
  ll_sys_dp_slp_exit();
#endif /* CFG_LPM_LEVEL */
  /* USER CODE BEGIN UTIL_SEQ_PostIdle_2 */
#if ( (CFG_LPM_LEVEL != 0) && ( CFG_LPM_STDBY_SUPPORTED == 1 ) )
  /* Treatment of WakeUp Button */
  if ( ( PWR->WUSR & PWR_WAKEUP_PIN2 ) != 0 )
  {
    PWR->WUSCR = PWR_WAKEUP_PIN2;
    BSP_PB_Callback( B1 );
  }
#endif /* ( CFG_LPM_LEVEL && CFG_LPM_STDBY_SUPPORTED ) */

  /* USER CODE END UTIL_SEQ_PostIdle_2 */
  return;
}

/**
 * @brief Callback used by 'Random Generator' to launch Task to generate Random Numbers
 */
void HWCB_RNG_Process( void )
{
  UTIL_SEQ_SetTask(1U << CFG_TASK_HW_RNG, CFG_TASK_PRIO_HW_RNG);
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

void SCM_HSI_CLK_OFF(void)
{
  /* SCM module may not switch off HSI clock when traces are used */

  /* USER CODE BEGIN SCM_HSI_CLK_OFF */

  /* USER CODE END SCM_HSI_CLK_OFF */
}

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
