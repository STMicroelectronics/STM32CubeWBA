/**
  ******************************************************************************
  * @file    audio_record.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the BSP AUDIO record feature
  *          in the STM32WBAxx_Nucleo driver
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Include PDM to PCM lib header file */
#include "pdm2pcm_glo.h"

/** @addtogroup STM32L5xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */
extern __IO JOYPin_TypeDef JoyPinPressed;
extern __ALIGN_END const uint8_t st_logo_small[];
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define PDM_SAMPLES_AT_EACH_CALL 48*20
#define AUDIO_REC_BUFFER_SIZE   (PDM_SAMPLES_AT_EACH_CALL * 8 * 2)
#define AUDIO_PLAY_BUFFER_SIZE  (PDM_SAMPLES_AT_EACH_CALL * 4)
#define AUDIO_HIGH_PASS_TAP     2122358088
#define AUDIO_STOP              0
#define AUDIO_STARTED           1
#define AUDIO_PLAYING           2
#define AUDIO_PAUSE             3

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t   RecordBuff[AUDIO_REC_BUFFER_SIZE];
uint16_t  PlaybackBuff[AUDIO_PLAY_BUFFER_SIZE];
uint32_t  RecHalfBuffCplt = 0;
uint32_t  RecBuffCplt     = 0;
uint32_t  PlaybackStatus  = AUDIO_STOP;
uint32_t  playbackPtr     = 0;
/* PDM filters params */
static PDM_Filter_Handler_t            PDM_FilterHandler;
static PDM_Filter_Config_t             PDM_FilterConfig;

/* Private function prototypes -----------------------------------------------*/
static void AudioRecord_SetHint(void);
static void Record_Init(void);
static void Playback_Init(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Test BSP AUDIO for record.
  * @param  None
  * @retval None
  */
int32_t AudioRecord_demo(void)
{
  /* Display test information */
  AudioRecord_SetHint();

  /* Delay to Display information */
  HAL_Delay(1000);

  /* Initialize record */
  Record_Init();

  /* Initialize playback */
  Playback_Init();

  /* Enable CRC peripheral to unlock the PDM library */
  __HAL_RCC_CRC_CLK_ENABLE();

  /* Init PDM filters */
  PDM_FilterHandler.bit_order  = PDM_FILTER_BIT_ORDER_MSB;
  PDM_FilterHandler.endianness = PDM_FILTER_ENDIANNESS_LE;
  PDM_FilterHandler.high_pass_tap = AUDIO_HIGH_PASS_TAP;
  PDM_FilterHandler.out_ptr_channels = 1;
  PDM_FilterHandler.in_ptr_channels  = 1;
  PDM_Filter_Init((PDM_Filter_Handler_t *)(&PDM_FilterHandler));

  /* PDM lib config phase */
  PDM_FilterConfig.mic_gain = 24;
  PDM_FilterConfig.output_samples_number = PDM_SAMPLES_AT_EACH_CALL;
  PDM_FilterConfig.decimation_factor = PDM_FILTER_DEC_FACTOR_64;
  PDM_Filter_setConfig((PDM_Filter_Handler_t *)&PDM_FilterHandler, &PDM_FilterConfig);

  /* Start record */
  if (BSP_AUDIO_IN_RecordPDM(1, (uint8_t *) RecordBuff, AUDIO_REC_BUFFER_SIZE) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  PlaybackStatus  = AUDIO_STARTED;

  /* Start loopback */
  while (JoyPinPressed != JOY_SEL)
  {
    if (RecHalfBuffCplt == 1)
    {
      /* Store values on Play buff */
      PDM_Filter(&RecordBuff[0], &PlaybackBuff[playbackPtr], &PDM_FilterHandler);
      playbackPtr += PDM_SAMPLES_AT_EACH_CALL;
      if (playbackPtr >= AUDIO_PLAY_BUFFER_SIZE)
      {
        playbackPtr = 0;
      }
      RecHalfBuffCplt = 0;
    }
    if (RecBuffCplt == 1)
    {
      /* Store values on Play buff */
      PDM_Filter(&RecordBuff[(AUDIO_REC_BUFFER_SIZE / 2)], &PlaybackBuff[playbackPtr], &PDM_FilterHandler);
      playbackPtr += PDM_SAMPLES_AT_EACH_CALL;
      if (playbackPtr >= AUDIO_PLAY_BUFFER_SIZE)
      {
        playbackPtr = 0;
      }

      if ((PlaybackStatus == AUDIO_STARTED) && (playbackPtr >= AUDIO_PLAY_BUFFER_SIZE / 2))
      {
        if (BSP_ERROR_NONE != BSP_AUDIO_OUT_Play(0, (uint8_t *) PlaybackBuff, AUDIO_PLAY_BUFFER_SIZE * 2))
        {
          Error_Handler();
        }
        Display_string("Recording");
        PlaybackStatus = AUDIO_PLAYING;
      }
      RecBuffCplt  = 0;
    }
    /* Pause */
    if ((JoyPinPressed == JOY_LEFT) && (PlaybackStatus == AUDIO_PLAYING))
    {
      /* Pause record and stop playback */
      if (BSP_ERROR_NONE != BSP_AUDIO_OUT_Stop(0))
      {
        Error_Handler();
      }
      if (BSP_ERROR_NONE != BSP_AUDIO_IN_Pause(1))
      {
        Error_Handler();
      }
      Display_string("Pause");
      /* Reset global variable */
      PlaybackStatus = AUDIO_PAUSE;
      RecBuffCplt = 0;
      RecHalfBuffCplt = 0;
      /* Reset Joystick state */
      JoyPinPressed = JOY_NONE;

    }
    /* Resume */
    if ((JoyPinPressed == JOY_RIGHT) && (PlaybackStatus == AUDIO_PAUSE))
    {
      /* Resume record */
      if (BSP_ERROR_NONE != BSP_AUDIO_IN_Resume(1))
      {
        Error_Handler();
      }
      Display_string("Resume");
      PlaybackStatus = AUDIO_STARTED;
      /* Reset Joystick state */
      JoyPinPressed = JOY_NONE;
    }
    /* Stop */
    if ((JoyPinPressed == JOY_DOWN) && (PlaybackStatus == AUDIO_PLAYING))
    {
      /* Stop record and stop playback */
      if (BSP_ERROR_NONE != BSP_AUDIO_OUT_Stop(0))
      {
        Error_Handler();
      }
      if (BSP_ERROR_NONE != BSP_AUDIO_IN_Stop(1))
      {
        Error_Handler();
      }
      /* Wait a little delay for pending dma */
      HAL_Delay(500);
      /* Reset global variable */
      PlaybackStatus = AUDIO_STOP;
      RecBuffCplt = 0;
      RecHalfBuffCplt = 0;
      Display_string("Stop");
      /* Reset Joystick state */
      JoyPinPressed = JOY_NONE;
    }
    /* Re-Start */
    if ((JoyPinPressed == JOY_UP) && (PlaybackStatus == AUDIO_STOP))
    {
      /* Restart record */
      if (BSP_ERROR_NONE != BSP_AUDIO_IN_RecordPDM(1, (uint8_t *) RecordBuff, AUDIO_REC_BUFFER_SIZE))
      {
        Error_Handler();
      }
      PlaybackStatus = AUDIO_STARTED;
      Display_string("Re-Start");
      /* Reset Joystick state */
      JoyPinPressed = JOY_NONE;
    }
  }

  Display_string("Exit");

  /* Stop record */
  if (BSP_AUDIO_IN_Stop(1) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* Stop playback */
  if (BSP_AUDIO_OUT_Stop(0) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* De-initialize record */
  if (BSP_AUDIO_IN_DeInit(1) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* De-initialize playback */
  if (BSP_AUDIO_OUT_DeInit(0) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  HAL_Delay(200);
  /* Reset Joystick state */
  JoyPinPressed = JOY_NONE;

  /* Reset global variables */
  RecHalfBuffCplt  = 0;
  RecBuffCplt      = 0;
  PlaybackStatus  = 0;

  return 0;
}

/**
  * @brief  Display audio record demo hint
  * @param  None
  * @retval None
  */
static void AudioRecord_SetHint(void)
{
  /* Clear the LCD */
  BSP_LCD_Clear(0, LCD_COLOR_BLACK);

  UTIL_LCD_DisplayStringAt(40, 0, (uint8_t *)"Audio Record", LEFT_MODE);
  BSP_LCD_DrawBitmap(0,  0,  0, (uint8_t *)st_logo_small);
  UTIL_LCD_DrawHLine(32, 13, 96, SSD1315_COLOR_WHITE);
  UTIL_LCD_DrawHLine(32, 14, 96, SSD1315_COLOR_WHITE);
  UTIL_LCD_DrawHLine(32, 15, 96, SSD1315_COLOR_WHITE);
  UTIL_LCD_DrawRect(0, 16, 128, 48, SSD1315_COLOR_WHITE);
  UTIL_LCD_DrawHLine(1, 16, 126, SSD1315_COLOR_BLACK);
  UTIL_LCD_SetFont(&Font8);
  UTIL_LCD_DisplayStringAt(71, 19, (uint8_t *)"Start", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(50, 28, (uint8_t *)"Pause   Resume", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(73, 36, (uint8_t *)"Stop", LEFT_MODE);
  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_DisplayStringAt(5, 26, (uint8_t *)"Dir:", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(0, 50, (uint8_t *)"JOY SEL to Exit", CENTER_MODE);

  /* Refresh LCD to Display FrameBuffer */
  BSP_LCD_Refresh(0);
}

/**
  * @brief  Record initialization
  * @param  None
  * @retval None
  */
static void Record_Init(void)
{
  BSP_AUDIO_Init_t AudioInit;

  AudioInit.Device        = AUDIO_IN_DEVICE_DIGITAL_MIC;
  AudioInit.SampleRate    = AUDIO_FREQUENCY_48K;
  AudioInit.BitsPerSample = AUDIO_RESOLUTION_8B;
  AudioInit.ChannelsNbr   = 1;
  AudioInit.Volume        = 80; /* Not used */
  if (BSP_AUDIO_IN_Init(1, &AudioInit) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  if (BSP_ERROR_NONE != BSP_AUDIO_IN_SetDevice(1, AUDIO_IN_DEVICE_DIGITAL_MIC)) { Error_Handler(); }
  if (BSP_ERROR_NONE != BSP_AUDIO_IN_SetBitsPerSample(1, AUDIO_RESOLUTION_8B)) { Error_Handler(); }
  if (BSP_ERROR_NONE != BSP_AUDIO_IN_SetChannelsNbr(1, 1)) { Error_Handler(); }
  if (BSP_ERROR_NONE != BSP_AUDIO_IN_SetSampleRate(1, AudioInit.SampleRate)) { Error_Handler(); }
}

/**
  * @brief  Playback initialization
  * @param  None
  * @retval None
  */
static void Playback_Init(void)
{
  BSP_AUDIO_Init_t AudioInit;

  AudioInit.Device        = AUDIO_OUT_DEVICE_HEADPHONE;
  AudioInit.SampleRate    = AUDIO_FREQUENCY_48K;
  AudioInit.BitsPerSample = AUDIO_RESOLUTION_16B;
  AudioInit.ChannelsNbr   = 1;
  AudioInit.Volume        = 80;
  if (BSP_AUDIO_OUT_Init(0, &AudioInit) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
}

/**
  * @brief  Manage the BSP audio in half transfer complete event.
  * @param  Instance Audio in instance.
  * @retval None.
  */
void BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance)
{
  if (Instance == 0U)
  {
    RecHalfBuffCplt = 1;
  }
}

/**
  * @brief  Manage the BSP audio in transfer complete event.
  * @param  Instance Audio in instance.
  * @retval None.
  */
void BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance)
{
  if (Instance == 0U)
  {
    RecBuffCplt = 1;
  }
}

/**
  * @brief  Manages the BSP audio in error event.
  * @param  Instance Audio in instance.
  * @retval None.
  */
void BSP_AUDIO_IN_Error_CallBack(uint32_t Instance)
{
  if (Instance == 0U)
  {
    Error_Handler();
  }
}

/**
  * @}
  */

/**
  * @}
  */
