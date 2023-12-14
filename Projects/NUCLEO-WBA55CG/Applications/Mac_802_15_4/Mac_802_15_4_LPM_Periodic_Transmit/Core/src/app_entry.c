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
#include "app_mac.h"
#include "ll_sys_startup.h"
#include "stm32_seq.h"
#if (CFG_LPM_SUPPORTED == 1)
#include "stm32_lpm.h"
#endif /* CFG_LPM_SUPPORTED */
#include "stm32_timer.h"
#if (CFG_LOG_SUPPORTED != 0)
#include "serial_cmd_interpreter.h"
#include "usart_if.h"
#endif /* CFG_LOG_SUPPORTED */
#include "ll_sys_if.h"
#include "app_sys.h"
#include "otp.h"
#include "scm.h"
#include "bpka.h"
#include "ll_sys.h"
#include "app_debug.h"
#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
#include "adc_ctrl.h"
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */

/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32wbaxx_nucleo.h"
#include "hw_if.h"

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

/* USER CODE BEGIN PD */
#if (CFG_BUTTON_SUPPORTED == 1)
#define BUTTON_LONG_PRESS_THRESHOLD_MS   (500u)
#define BUTTON_NB_MAX                    (B3 + 1u)
#endif

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

/* USER CODE BEGIN PV */
#if (CFG_BUTTON_SUPPORTED == 1)
/* Button management */
static ButtonDesc_t buttonDesc[BUTTON_NB_MAX];
#endif

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
static void Led_Init(void);
#endif
#if (CFG_BUTTON_SUPPORTED == 1)
static void Button_Init(void);
static void Button_TriggerActions(void *arg);
#endif
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

  UNUSED(p_param);

  /* System initialization */
  System_Init();

  /* Configure the system Power Mode */
  SystemPower_Config();


  /* USER CODE BEGIN APPE_Init_1 */
#if (CFG_LED_SUPPORTED == 1)
  Led_Init();
#endif
#if (CFG_BUTTON_SUPPORTED == 1)
  Button_Init();
#endif
  /* USER CODE END APPE_Init_1 */
  UTIL_SEQ_RegTask(1U << CFG_TASK_BPKA, UTIL_SEQ_RFU, BPKA_BG_Process);

  BPKA_Reset( );

  RNG_Init();


  ll_sys_mac_cntrl_init();
  
/* MAC Abstraction_Layer Init */
  APP_MAC_Init();  

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
#if (CFG_LOG_SUPPORTED != 0)
  Ifhuart_s sIfUartInit;
#endif
  /* Clear RCC RESET flag */
  LL_RCC_ClearResetFlags();

  UTIL_TIMER_Init();

  /* Enable wakeup out of standby from RTC ( UTIL_TIMER )*/
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN7_HIGH_3);

#if (CFG_LOG_SUPPORTED != 0)
    /*Initialize the terminal using the LPUART */
  sIfUartInit.IfhuartTx = &IF_USART_TX;
  sIfUartInit.IfhuartRx = &IF_USART_RX;
  UartIf_Init(sIfUartInit);
  /* Initialize the logs ( using the USART ) */
  Log_Module_Init( Log_Module_Config );

  /* Initialize the Command Interpreter */
  Serial_CMD_Interpreter_Init();
#endif  /* (CFG_LOG_SUPPORTED != 0) */

#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
  adc_ctrl_init();
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */

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
  /* Set VOS (Range 1) */
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);

  /* Wait until VOS has changed */
  while (LL_PWR_IsActiveFlag_VOS() == 0);

  scm_init();
  /* Set the HSE clock to 32MHz */
  scm_setsystemclock(SCM_USER_APP, HSE_32MHZ);
#if (CFG_LPM_SUPPORTED == 1)
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
#endif /* (CFG_LPM_SUPPORTED == 1)  */
}

/**
 * @brief Initialize Random Number Generator module
 */
static void RNG_Init(void)
{
  HW_RNG_Start();

  UTIL_SEQ_RegTask(1U << CFG_TASK_HW_RNG, UTIL_SEQ_RFU, (void (*)(void))HW_RNG_Process);

  return;
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

  /* Register tasks associated to buttons */
  UTIL_SEQ_RegTask(1U << TASK_BUTTON_1, UTIL_SEQ_RFU, APPE_Button1Action);
  UTIL_SEQ_RegTask(1U << TASK_BUTTON_2, UTIL_SEQ_RFU, APPE_Button2Action);
  UTIL_SEQ_RegTask(1U << TASK_BUTTON_3, UTIL_SEQ_RFU, APPE_Button3Action);

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

  LOG_INFO_APP("Button %d pressed\r\n", (p_buttonDesc->button + 1));
  switch (p_buttonDesc->button)
  {
    case B1:
      UTIL_SEQ_SetTask(1U << TASK_BUTTON_1, CFG_SEQ_PRIO_0);
      break;
    case B2:
      UTIL_SEQ_SetTask(1U << TASK_BUTTON_2, CFG_SEQ_PRIO_0);
      break;
    case B3:
      UTIL_SEQ_SetTask(1U << TASK_BUTTON_3, CFG_SEQ_PRIO_0);
      break;
    default:
      break;
  }

  return;
}

#endif

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

/**
  * @brief  This function is called by the scheduler each time an event
  *         is pending.
  *
  * @param  evt_waited_bm : Event pending.
  * @retval None
  */
void UTIL_SEQ_EvtIdle( UTIL_SEQ_bm_t task_id_bm, UTIL_SEQ_bm_t evt_waited_bm )
{
  UTIL_SEQ_Run( UTIL_SEQ_DEFAULT );

  return;
}

void UTIL_SEQ_Idle( void )
{
#if ( CFG_LPM_SUPPORTED == 1)
  HAL_SuspendTick();
  UTIL_LPM_EnterLowPower();
  HAL_ResumeTick();
#endif /* CFG_LPM_SUPPORTED */
  return;
}

void UTIL_SEQ_PreIdle( void )
{
  /* USER CODE BEGIN UTIL_SEQ_PreIdle_1 */
  
  /* USER CODE END UTIL_SEQ_PreIdle_1 */
#if ( CFG_LPM_SUPPORTED == 1)
  LL_PWR_ClearFlag_STOP();

  if(system_startup_done && UTIL_LPM_GetMode() == UTIL_LPM_OFFMODE)
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
#endif /* CFG_LPM_SUPPORTED */
  /* USER CODE BEGIN UTIL_SEQ_PreIdle_2 */

  /* USER CODE END UTIL_SEQ_PreIdle_2 */
  return;
}

void UTIL_SEQ_PostIdle( void )
{
  /* USER CODE BEGIN UTIL_SEQ_PostIdle_1 */

  /* USER CODE END UTIL_SEQ_PostIdle_1 */
#if ( CFG_LPM_SUPPORTED == 1)
  LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_RADIO);
  ll_sys_dp_slp_exit();
#endif /* CFG_LPM_SUPPORTED */
  /* USER CODE BEGIN UTIL_SEQ_PostIdle_2 */

  /* USER CODE END UTIL_SEQ_PostIdle_2 */
  return;
}

void BPKACB_Process( void )
{
  UTIL_SEQ_SetTask(1U << CFG_TASK_BPKA, CFG_SEQ_PRIO_0);
}

/**
 * @brief Callback used by 'Random Generator' to launch Task to generate Random Numbers
 */
void HWCB_RNG_Process( void )
{
  UTIL_SEQ_SetTask(1U << CFG_TASK_HW_RNG, CFG_TASK_PRIO_HW_RNG);
}

void AMM_ProcessRequest (void)
{
  /* Ask for AMM background task scheduling */
  UTIL_SEQ_SetTask(1U << CFG_TASK_AMM_BCKGND, CFG_SEQ_PRIO_0);
}

void FM_ProcessRequest (void)
{
  /* Schedule the background process */
  UTIL_SEQ_SetTask(1U << CFG_TASK_FLASH_MANAGER_BCKGND, CFG_SEQ_PRIO_0);
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

#endif /* (CFG_LOG_SUPPORTED != 0) */

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
