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
#include "stm32_seq.h"
#if (CFG_LPM_LEVEL != 0)
#include "stm32_lpm.h"
#endif /* (CFG_LPM_LEVEL != 0) */
#include "stm32_timer.h"
#include "stm32_mm.h"
#if (CFG_LOG_SUPPORTED != 0)
#include "stm32_adv_trace.h"
#include "serial_cmd_interpreter.h"
#endif /* CFG_LOG_SUPPORTED */
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

/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "codec_mngr.h"
#include "codec_if.h"
#include "stm32wba55g_discovery.h"
#include "stm32wba55g_discovery_audio.h"
#include "stm32wba55g_discovery_bus.h"
#include "stm32wba55g_discovery_lcd.h"
#include "stm32_lpm.h"
#include "stm32_lcd.h"
#include "pbp_app.h"
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
/* Log configuration */
static Log_Module_t Log_Module_Config = { .verbose_level = APPLI_CONFIG_LOG_LEVEL, .region = LOG_REGION_ALL_REGIONS };
#endif /* (CFG_LOG_SUPPORTED != 0) */

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

/* USER CODE BEGIN PV */
#if (CFG_JOYSTICK_SUPPORTED == 1)
static JOYPin_TypeDef Joystick_Event;
#endif /* (CFG_JOYSTICK_SUPPORTED == 1) */
#if (CFG_LCD_SUPPORTED == 1)
static uint8_t mute;
#endif /* (CFG_LCD_SUPPORTED == 1) */

static uint32_t frame_byte_size = 0;
static uint32_t PLL_Target_Clock_Freq=0;
static uint8_t MxAudioInit_Flag = 0;
/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/
/* USER CODE BEGIN GV */
#if (CFG_JOYSTICK_SUPPORTED == 1)
extern uint8_t adc_cleared_flag;
uint32_t ADC_High_Threshold;
uint32_t ADC_Low_Threshold;
#endif /* (CFG_JOYSTICK_SUPPORTED == 1) */
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

/* USER CODE BEGIN PFP */
#if (CFG_LED_SUPPORTED == 1)
static void Led_Init(void);
#endif /* CFG_LED_SUPPORTED */
#if (CFG_JOYSTICK_SUPPORTED == 1)
static void Joystick_Init( uint8_t wkup_mode );
static void Joystick_ActionHandle(void);
#endif /* CFG_JOYSTICK_SUPPORTED */
#if (CFG_LCD_SUPPORTED == 1)
static void LCD_Init(void);
static int32_t LCD_DrawBitmapArray(uint8_t xpos, uint8_t ypos, uint8_t xlen, uint8_t ylen, uint8_t *data);
static void SourceIDToString(uint32_t SourceID, uint8_t *pString);
static void DrawSpeakerStateTask(void);
static void DrawSpeakerState(void);
#endif /* CFG_LCD_SUPPORTED */
static void Init_AudioBuffer(uint8_t *pRecBuff, uint16_t RecBuffLen,uint8_t *pPlayBuff, uint16_t PlayBuffLen);
static void AudioClock_Deinit(void);
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

  /* Initialize the Advance Memory Manager */
  AMM_Init (&ammInitConfig);

  /* Register the AMM background task */
  UTIL_SEQ_RegTask(1U << CFG_TASK_AMM_BCKGND, UTIL_SEQ_RFU, AMM_BackgroundProcess);
  /* Initialize the Simple NVM Arbiter */
  SNVMA_Init ((uint32_t *)CFG_SNVMA_START_ADDRESS);

  /* Register the flash manager task */
  UTIL_SEQ_RegTask(1U << CFG_TASK_FLASH_MANAGER_BCKGND, UTIL_SEQ_RFU, FM_BackgroundProcess);

  /* USER CODE BEGIN APPE_Init_1 */
#if (CFG_LED_SUPPORTED == 1)
  Led_Init();
#endif /* (CFG_LED_SUPPORTED == 1) */
#if (CFG_JOYSTICK_SUPPORTED == 1)
  Joystick_Init(0);
#endif /* (CFG_JOYSTICK_SUPPORTED == 1) */
#if (CFG_LCD_SUPPORTED == 1)
  LCD_Init();
  UTIL_SEQ_RegTask(1U << CFG_TASK_DRAW_SPEAKER_ID, UTIL_SEQ_RFU, DrawSpeakerStateTask);
#endif /* CFG_LCD_SUPPORTED */
  /* USER CODE END APPE_Init_1 */
  UTIL_SEQ_RegTask(1U << CFG_TASK_BPKA, UTIL_SEQ_RFU, BPKA_BG_Process);

  BPKA_Reset( );

  RNG_Init();

  /* Disable flash before any use - RFTS */
  FD_SetStatus (FD_FLASHACCESS_RFTS, LL_FLASH_DISABLE);
  /* Enable RFTS Bypass for flash operation - Since LL has not started yet */
  FD_SetStatus (FD_FLASHACCESS_RFTS_BYPASS, LL_FLASH_ENABLE);
  /* Enable flash system flag */
  FD_SetStatus (FD_FLASHACCESS_SYSTEM, LL_FLASH_ENABLE);

  APP_BLE_Init();
  /* Disable RFTS Bypass for flash operation - Since LL has not started yet */
  FD_SetStatus (FD_FLASHACCESS_RFTS_BYPASS, LL_FLASH_DISABLE);

  /* USER CODE BEGIN APPE_Init_2 */
#if (CFG_JOYSTICK_SUPPORTED == 1)
  /* Register Button Tasks */
  UTIL_SEQ_RegTask(1U << CFG_TASK_JOYSTICK_ID, UTIL_SEQ_RFU, Joystick_ActionHandle);
#endif /* CFG_JOYSTICK_SUPPORTED */
  /* USER CODE END APPE_Init_2 */
  APP_DEBUG_SIGNAL_RESET(APP_APPE_INIT);
  return WPAN_SUCCESS;
}

/* USER CODE BEGIN FD */
#if (CFG_JOYSTICK_SUPPORTED == 1)
void BSP_JOY_Callback(JOY_TypeDef JOY, JOYPin_TypeDef JoyPin)
{
  Joystick_Event = JoyPin;
  UTIL_SEQ_SetTask( 1U << CFG_TASK_JOYSTICK_ID, CFG_SEQ_PRIO_0);
}
#endif /* CFG_JOYSTICK_SUPPORTED */
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
}

/**
 * @brief Initialize Random Number Generator module
 */
static void RNG_Init(void)
{
  HW_RNG_Start();

  UTIL_SEQ_RegTask(1U << CFG_TASK_HW_RNG, UTIL_SEQ_RFU, (void (*)(void))HW_RNG_Process);
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
  BSP_LED_Init(LD3);
  BSP_LED_On(LD3);

  return;
}
#endif /* CFG_LED_SUPPORTED */

#if (CFG_JOYSTICK_SUPPORTED == 1)
static void Joystick_Init( uint8_t wkup_mode )
{
  if (wkup_mode == 1)
  {
    /* configuration as a WKUP Pin */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin       = JOY1_CHANNEL_GPIO_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(JOY1_CHANNEL_GPIO_PORT, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI1_IRQn, 15, 0);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
  }
  else
  {
    BSP_JOY_Init(JOY1, JOY_MODE_IT, JOY_ALL);
    /* reconfiguration of the ADC4 interrupt priority */
    HAL_NVIC_DisableIRQ(ADC4_IRQn);
    HAL_NVIC_SetPriority(ADC4_IRQn, 15, 0);
    HAL_NVIC_EnableIRQ(ADC4_IRQn);

    /* remove end of conversion interrupt */
    LL_ADC_REG_StopConversion(hjoy_adc[JOY1].Instance);
    while(LL_ADC_REG_IsConversionOngoing(hjoy_adc[JOY1].Instance) != 0);
    __HAL_ADC_DISABLE_IT(&hjoy_adc[JOY1], ADC_IT_EOC);
    LL_ADC_REG_StartConversion(hjoy_adc[JOY1].Instance);
  }
}

static void Joystick_ActionHandle(void)
{
  if(Joystick_Event == JOY_SEL)
  {
  }
}
#endif  /* CFG_JOYSTICK_SUPPORTED */

#if (CFG_LCD_SUPPORTED == 1)

static void LCD_Init( void )
{
  extern uint8_t wav_2[];
  uint8_t source_id_string[19] = "source ID 0xXXXXXX";
  /* BSP init LCD*/
  BSP_LCD_Init(0, LCD_ORIENTATION_LANDSCAPE);

  /* Set LCD Foreground Layer  */
  UTIL_LCD_SetFuncDriver(&LCD_Driver); /* SetFunc before setting device */
  UTIL_LCD_SetDevice(0);               /* SetDevice after funcDriver is set */

  BSP_LCD_DisplayOn(0);

  BSP_LCD_Clear(0,SSD1315_COLOR_BLACK);
  BSP_LCD_Refresh(0);

  /* Set the LCD Text Color */
  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_SetTextColor(SSD1315_COLOR_WHITE);
  UTIL_LCD_SetBackColor(SSD1315_COLOR_BLACK);
  BSP_LCD_Refresh(0);

  LCD_DrawBitmapArray(0 , 32, 128, 32, &wav_2[0]);

  mute = 1;
  DrawSpeakerState();

  UTIL_LCD_DisplayStringAt(0, 5, (uint8_t *)"AURACAST DEMO", CENTER_MODE);

  SourceIDToString(BAP_BROADCAST_SOURCE_ID, &(source_id_string[12]));
  UTIL_LCD_DisplayStringAt(0, 5+12+2, source_id_string, CENTER_MODE);

  BSP_LCD_Refresh(0);

  /* release bus for power optimisation */
  BSP_SPI3_DeInit();
}

/* draw an array of bits at the specified offsets starting from corner top left. Ensure xlen is multiple of 8*/
static int32_t LCD_DrawBitmapArray(uint8_t xpos, uint8_t ypos, uint8_t xlen, uint8_t ylen, uint8_t *data){

  int32_t i,j,k;
  uint8_t mask;
  uint8_t* pdata = data;

  if (((xpos+xlen) > 128) || ((ypos+ylen) > 64))
  {
    /*out of screen*/
    return -1;
  }

  for (j=0 ; j < ylen ; j++)
  {
    for (i=0 ; i < xlen/8 ; i++)
    {
      mask = 0x80;
      for (k=0 ; k < 8 ; k++)
      {
        if ( mask & *pdata)
        {
          UTIL_LCD_SetPixel(i*8+k+xpos, j+ypos, SSD1315_COLOR_WHITE);
        }
        else
        {
          UTIL_LCD_SetPixel(i*8+k+xpos, j+ypos, SSD1315_COLOR_BLACK);
        }
        mask = mask >> 1;
      }
      pdata++;
    }
  }

  BSP_LCD_Refresh(0);

  return 0;
}

static void SourceIDToString(uint32_t SourceID, uint8_t *pString)
{
  uint8_t i;
  for (i = 0; i < 6; i++)
  {
    uint8_t digit = (SourceID >> ((5-i)*4)) & 0xF;
    if (digit < 0xA)
    {
      pString[i] = digit + 0x30;
    }
    else
    {
      pString[i] = digit + 0x37;
    }
  }
}
static void DrawSpeakerStateTask(void)
{
  BSP_SPI3_Init();
  DrawSpeakerState();
  BSP_SPI3_DeInit();
}

static void DrawSpeakerState(void)
{
  extern uint8_t volume_up[];
  extern uint8_t volume_mute[];
  uint8_t *pvolume;

  if (mute == 1)
  {
    /* Draw volume mute icon */
    pvolume = &volume_mute[0];
  }
  else
  {
    /* Draw volume up icon */
    pvolume = &volume_up[0];
  }

  LCD_DrawBitmapArray(108, 38, 16, 16, pvolume);
}
#endif  /* CFG_LCD_SUPPORTED */

/**
  * @brief Configure PLL for the audio frequency, PLL output is used for the SAI peripheral and as the core clock
  * @retval None
  */
void AudioClock_Init(uint32_t audio_frequency_type)
{
  uint32_t target_freq;
  if (audio_frequency_type == SAMPLE_FREQ_44100_HZ)
  {
    target_freq = 90316800;
  }
  else
  {
    /* valid for 8k, 16k, 24k, 32k or 48k */
    target_freq = 98304000;
  }

  if ( target_freq !=  PLL_Target_Clock_Freq )
  {
    scm_pll_config_t pll_config;

    pll_config.pll_mode = PLL_FRACTIONAL_MODE;
    pll_config.PLLM = 6;
    pll_config.PLLP = 4;
    pll_config.PLLQ = 4;
    pll_config.PLLR = 4;

    /* VCO frequency should be in range 128 to 544 MHz */
    /* PLL fractionnal should allows to cover +/-550ppm around the targeted frequency*/
    if (target_freq == 90316800)
    {
      pll_config.PLLN = 67;  /* VCO = HSE/M * N * (1 + Nfrac / 8192) < 32 / 6 * (67 + 1) = 362.7 MHz */
      pll_config.PLLFractional = 6042;
      pll_config.AHB5_PLL1_CLKDivider = 3;
      /* PLLSYS = 90.31673 MHz */
    }
    else
    {
      pll_config.PLLN = 73;  /* VCO = HSE/M * N * (1 + Nfrac / 8192) < 32 / 6 * (73 + 1) = 394.7 MHz */
      pll_config.PLLFractional = 5964;
      pll_config.AHB5_PLL1_CLKDivider = 4;
      /* PLLSYS = 98.30396 MHz */
    }

    UTIL_LPM_SetStopMode(1 << CFG_LPM_AUDIO, UTIL_LPM_DISABLE);

    scm_pll_setconfig(&pll_config);

    scm_setsystemclock(SCM_USER_APP, SYS_PLL);

    /* enable P output used for the audio timer */
    LL_RCC_PLL1_EnableDomain_PLL1P();

    PLL_Target_Clock_Freq = target_freq;

    CODEC_CLK_Init();

    AUDIO_PLLConfig_t corrector_pll_config;
    corrector_pll_config.PLLTargetFreq = target_freq;
    corrector_pll_config.VCOInputFreq = (32000000.0f / pll_config.PLLM);
    corrector_pll_config.PLLOutputDiv = pll_config.PLLP;
    AUDIO_InitializeClockCorrector(&corrector_pll_config, 500000, 4000000);
  }
}


static void AudioClock_Deinit( void )
{
  scm_setsystemclock(SCM_USER_APP, NO_CLOCK_CONFIG);

  PLL_Target_Clock_Freq = 0;

  AUDIO_DeinitializeClockCorrector();

  UTIL_LPM_SetStopMode(1 << CFG_LPM_AUDIO, UTIL_LPM_ENABLE);
}

HAL_StatusTypeDef MX_SAI1_ClockConfig(SAI_HandleTypeDef *hsai, uint32_t SampleRate)
{
  /* PLL already configured */

  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
  PeriphClkInit.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL1P;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

  __HAL_RCC_SAI1_CLK_ENABLE();

  return HAL_OK;
}

void MX_AudioInit(Audio_Role_t role,
                  Sampling_Freq_t sampling_frequency,
                  Frame_Duration_t frame_duration,
                  uint8_t *pRecBuff,
                  uint8_t *pPlayBuff,
                  AudioDriverConfig driver_config)
{
  uint32_t sample_per_frame;
  uint32_t audioFrequency;
  uint16_t rec_buff_length = 0u;
  uint16_t play_buff_length = 0u;
  BSP_AUDIO_Init_t audio_conf;

  switch (sampling_frequency)
  {
    case SAMPLE_FREQ_8000_HZ:
      audioFrequency = SAI_AUDIO_FREQUENCY_8K;
      break;

    case SAMPLE_FREQ_16000_HZ:
      audioFrequency = SAI_AUDIO_FREQUENCY_16K;
      break;

    case SAMPLE_FREQ_22050_HZ:
      audioFrequency = SAI_AUDIO_FREQUENCY_22K;
      break;

    case SAMPLE_FREQ_24000_HZ:
      audioFrequency = SAI_AUDIO_FREQUENCY_24K;
      break;

    case SAMPLE_FREQ_32000_HZ:
      audioFrequency = SAI_AUDIO_FREQUENCY_32K;
      break;

    case SAMPLE_FREQ_44100_HZ:
      audioFrequency = SAI_AUDIO_FREQUENCY_44K;
      break;

    case SAMPLE_FREQ_48000_HZ:
      audioFrequency = SAI_AUDIO_FREQUENCY_48K;
      break;
  }
  switch(frame_duration)
  {
    case FRAME_DURATION_7_5_MS:
      sample_per_frame = audioFrequency * 75 / 10000;
      break;

    case FRAME_DURATION_10_MS:
      sample_per_frame = audioFrequency * 100 / 10000;
      break;

  }

  if (sampling_frequency == SAMPLE_FREQ_44100_HZ)
  {
    /* LC3 need setup like 48Khz for this frequency */
    switch(frame_duration)
    {
    case FRAME_DURATION_7_5_MS:
      sample_per_frame = 360;
      break;

    case FRAME_DURATION_10_MS:
      sample_per_frame = 480;
      break;
    }
  }


  audio_conf.Device = AUDIO_IN_DEVICE_LINE_IN;
  audio_conf.SampleRate = audioFrequency;
  audio_conf.BitsPerSample = AUDIO_RESOLUTION_16B;
  audio_conf.ChannelsNbr = 2;
  audio_conf.Volume = 50;

  if (BSP_AUDIO_IN_Init(0x00, &audio_conf) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  audio_conf.Device = AUDIO_OUT_DEVICE_HEADPHONE;
  audio_conf.ChannelsNbr = 2;
  audio_conf.Volume = 90;
  if (BSP_AUDIO_OUT_Init(0x00, &audio_conf) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* Start SAI clock without DMA interrupt */
  uint8_t channel_per_buf = 2; /* SAI is in stereo */
  uint8_t buffer_nb = 2; /* double buffer strategie for minimum latency*/
  uint8_t bytes_per_sample = 2;
  frame_byte_size = sample_per_frame * channel_per_buf * buffer_nb;
  if (pRecBuff != 0)
  {
    rec_buff_length = (sample_per_frame * channel_per_buf * buffer_nb * bytes_per_sample);
  }
  if (pPlayBuff != 0)
  {
    play_buff_length = (sample_per_frame * channel_per_buf * buffer_nb * bytes_per_sample);
  }
  /* Start SAI clock without DMA interrupt */
  Init_AudioBuffer((uint8_t *)pRecBuff,
                   rec_buff_length,
                   (uint8_t *)pPlayBuff,
                   play_buff_length);

  /* Release Bus for power consumption optimisation */
  __HAL_RCC_I2C3_CLK_DISABLE();
  MxAudioInit_Flag = 1;

}

void MX_AudioDeInit(void)
{
  if (MxAudioInit_Flag == 1)
  {
    MxAudioInit_Flag = 0;

    __HAL_RCC_I2C3_CLK_ENABLE();

    if (BSP_AUDIO_IN_DeInit(0x00) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }

    if (BSP_AUDIO_OUT_DeInit(0x00) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }

    BSP_I2C3_DeInit();
  }

  AudioClock_Deinit();

#if (CFG_LCD_SUPPORTED == 1)
  mute = 1;
  UTIL_SEQ_SetTask( 1U << CFG_TASK_DRAW_SPEAKER_ID, CFG_SEQ_PRIO_0);
#endif /* (CFG_LCD_SUPPORTED == 1) */
}

static void Init_AudioBuffer(uint8_t *pRecBuff, uint16_t RecBuffLen,uint8_t *pPlayBuff, uint16_t PlayBuffLen)
{
  if ((RecBuffLen  > 0) && (pRecBuff != NULL))
  {
    if (BSP_AUDIO_OUT_Play(0, (uint8_t *)pRecBuff, RecBuffLen) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }
    if (BSP_AUDIO_OUT_Pause(0) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }
  }
  if ((PlayBuffLen  > 0) && (pPlayBuff != NULL))
  {
    if (BSP_AUDIO_IN_Record(0, (uint8_t *)pPlayBuff, PlayBuffLen) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }
    if (BSP_AUDIO_IN_Pause(0) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }
  }
}

void Start_TxAudio(void)
{
  /* Initialize Bus which bas been released for Power consumption optimisation */
  __HAL_RCC_I2C3_CLK_ENABLE();

  APP_NotifyTxAudioCplt(frame_byte_size);
  if (BSP_AUDIO_OUT_Resume(0) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* Release Bus for power consumption optimisation */
  __HAL_RCC_I2C3_CLK_DISABLE();
}

void Stop_TxAudio(void)
{
  /* Initialize Bus which bas been released for Power consumption optimisation */
 __HAL_RCC_I2C3_CLK_ENABLE();

  if (BSP_AUDIO_OUT_Stop(0) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* Release Bus for power consumption optimisation */
  __HAL_RCC_I2C3_CLK_DISABLE();
}

void BSP_AUDIO_OUT_TransferComplete_CallBack(uint32_t instance)
{
  APP_NotifyTxAudioCplt(frame_byte_size);
}

void BSP_AUDIO_OUT_HalfTransfer_CallBack(uint32_t instance)
{
  APP_NotifyTxAudioHalfCplt();
}

void Start_RxAudio(void)
{
  /* Initialize Bus which bas been released for Power consumption optimisation */
  __HAL_RCC_I2C3_CLK_ENABLE();

  if (BSP_AUDIO_IN_Resume(0) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* Release Bus for power consumption optimisation */
  __HAL_RCC_I2C3_CLK_DISABLE();

#if (CFG_LCD_SUPPORTED == 1)
  mute = 0;
  UTIL_SEQ_SetTask( 1U << CFG_TASK_DRAW_SPEAKER_ID, CFG_SEQ_PRIO_0);
#endif /* (CFG_LCD_SUPPORTED == 1) */
}

void Stop_RxAudio(void)
{
  /* Initialize Bus which bas been released for Power consumption optimisation */
  __HAL_RCC_I2C3_CLK_ENABLE();

  if (BSP_AUDIO_IN_Stop(0) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* Release Bus for power consumption optimisation */
  __HAL_RCC_I2C3_CLK_DISABLE();
}

void BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t instance)
{
  APP_NotifyRxAudioCplt(frame_byte_size);
}

void BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t instance)
{
  APP_NotifyRxAudioHalfCplt();
}
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
    APP_SYS_BLE_EnterDeepSleep();
  }

  LL_RCC_ClearResetFlags();

#if defined(STM32WBAXX_SI_CUT1_0)
  /* Wait until HSE is ready */
  while (LL_RCC_HSE_IsReady() == 0);

  UTILS_ENTER_LIMITED_CRITICAL_SECTION(RCC_INTR_PRIO << 4U);
  scm_hserdy_isr();
  UTILS_EXIT_LIMITED_CRITICAL_SECTION();
#endif /* STM32WBAXX_SI_CUT1_0 */
#endif /* CFG_LPM_LEVEL */
  /* USER CODE BEGIN UTIL_SEQ_PreIdle_2 */
#if (CFG_LPM_STDBY_SUPPORTED == 1)
#if (CFG_JOYSTICK_SUPPORTED == 1)
  ADC_Low_Threshold = LL_ADC_GetAnalogWDThresholds(ADC4, LL_ADC_AWD1, LL_ADC_AWD_THRESHOLD_LOW);
  ADC_High_Threshold = LL_ADC_GetAnalogWDThresholds(ADC4, LL_ADC_AWD1, LL_ADC_AWD_THRESHOLD_HIGH);
#endif /* CFG_JOYSTICK_SUPPORTED */
#endif /* CFG_LPM_STDBY_SUPPORTED */
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
#if (CFG_LPM_STDBY_SUPPORTED == 1)
#if (CFG_JOYSTICK_SUPPORTED == 1)
  if (adc_cleared_flag == 1){
    BSP_JOY_DeInit(JOY1, JOY_ALL);
    Joystick_Init(0);

    /* re set threshold */
    LL_ADC_ConfigAnalogWDThresholds(ADC4, LL_ADC_AWD1, ADC_High_Threshold, ADC_Low_Threshold);
    adc_cleared_flag=0;
  }
#endif /* CFG_JOYSTICK_SUPPORTED */
#endif /* CFG_LPM_STDBY_SUPPORTED */
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
#if (CFG_LOG_SUPPORTED == 0)
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

#endif /* (CFG_LOG_SUPPORTED == 0) */

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
  HAL_GPIO_EXTI_Rising_Callback(GPIO_Pin);
}

/* USER CODE END FD_WRAP_FUNCTIONS */
