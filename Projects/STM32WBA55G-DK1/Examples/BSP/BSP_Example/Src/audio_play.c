/**
  ******************************************************************************
  * @file    BSP/Src/audio_play.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the BSP AUDIO play feature
  *          in the STM32WBAxx_discovery driver
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
#include <stdio.h>
#include "audio_16khz_wav.h"

/** @addtogroup STM32H7xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

extern __IO JOYPin_TypeDef JoyPinPressed;
extern __ALIGN_END const uint8_t st_logo_small[];
/* Private typedef -----------------------------------------------------------*/
typedef enum
{
  AUDIO_ERROR_NONE = 0,
  AUDIO_ERROR_NOTREADY,
  AUDIO_ERROR_IO,
  AUDIO_ERROR_EOF,
} AUDIO_ErrorTypeDef;
/* Private define ------------------------------------------------------------*/
#define AUDIO_BUFFER_SIZE            4096
/* Audio file size */
#define AUDIO_HEADER_FILE_SIZE       10
#define AUDIO_FILE_SIZE              BYTES_IN_AUDIO_WAV - AUDIO_HEADER_FILE_SIZE
#define AUDIO_VOL_MAX                100
#define AUDIO_VOL_MIN                0
#define AUDIO_VOL_INC                5

/* Private typedef -----------------------------------------------------------*/
typedef enum
{
  AUDIO_STATE_IDLE = 0,
  AUDIO_STATE_INIT,
  AUDIO_STATE_PLAYING,
} AUDIO_PLAYBACK_StateTypeDef;

typedef enum
{
  BUFFER_OFFSET_NONE = 0,
  BUFFER_OFFSET_HALF,
  BUFFER_OFFSET_FULL,
} BUFFER_StateTypeDef;

typedef struct
{
  int16_t buff[AUDIO_BUFFER_SIZE];
  uint32_t fptr;
  BUFFER_StateTypeDef state;
  uint32_t AudioFileSize;
  uint32_t *SrcAddress;
} AUDIO_BufferTypeDef;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static AUDIO_BufferTypeDef  buffer_ctl;
static AUDIO_PLAYBACK_StateTypeDef  audio_state;
__IO uint32_t uwVolume;
uint32_t PauseEnabledStatus = 0U;
uint32_t AudioInstance = 0U;
char FreqStr[20] = {0U};

/* Private function prototypes -----------------------------------------------*/
static void Audio_SetHint(uint32_t Index);
static uint32_t GetData(void *pdata, uint32_t offset, uint8_t *pbuf, uint32_t NbrOfData);
AUDIO_ErrorTypeDef AUDIO_Start(uint32_t *psrc_address, uint32_t file_size);
AUDIO_ErrorTypeDef AUDIO_Stop(void);
uint8_t AUDIO_Process(void);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Audio Play demo
  * @param  None
  * @retval None
  */
int32_t AudioPlay_demo(void)
{
  BSP_AUDIO_Init_t AudioPlayInit;

  uwVolume = 70;

  /* Display test information */
  Audio_SetHint(0);

  /* Delay to Display information */
  HAL_Delay(1000);

  AudioPlayInit.Device        = AUDIO_OUT_DEVICE_HEADPHONE;
  AudioPlayInit.ChannelsNbr   = 2;
  AudioPlayInit.SampleRate    = AUDIO_FREQUENCY_16K;
  AudioPlayInit.BitsPerSample = AUDIO_RESOLUTION_16B;
  AudioPlayInit.Volume        = uwVolume;

  if (BSP_AUDIO_OUT_Init(AudioInstance, &AudioPlayInit) != BSP_ERROR_NONE)
  {
    Display_string("Audio Codec Fail");
    while (JoyPinPressed != JOY_SEL)
    {
    }
    return 0;
  }
  else
  {
    /*
    Start playing the file from a circular buffer, once the DMA is enabled, it is
    always in running state. Application has to fill the buffer with the audio data
    using Transfer complete and/or half transfer complete interrupts callbacks
    (BSP_AUDIO_OUT_TransferComplete_CallBack() or BSP_AUDIO_OUT_HalfTransfer_CallBack()...
    */
    if (AUDIO_Start((uint32_t *)audio_wav + AUDIO_HEADER_FILE_SIZE, (uint32_t)AUDIO_FILE_SIZE) != AUDIO_ERROR_NONE)
    {
      Display_string("Audio Start Fail");
      while (JoyPinPressed != JOY_SEL)
      {
      }
      return 0;
    }

    Display_string("PLAYING...");

    /* Infinite loop */
    while (1)
    {
      /* IMPORTANT: AUDIO_Process() should be called within a periodic process */
      AUDIO_Process();

      if (JoyPinPressed != JOY_NONE)
      {
        /* Get the TouchScreen State */
        switch (JoyPinPressed)
        {

        case JOY_UP:
          /* Increase volume by 5% */
          if (uwVolume < (AUDIO_VOL_MAX - AUDIO_VOL_INC))
          {
            uwVolume += AUDIO_VOL_INC;
          }
          else
          {
            uwVolume = AUDIO_VOL_MAX;
          }
          sprintf((char *)FreqStr, "VOL:%3ld", uwVolume);
          if (BSP_AUDIO_OUT_SetVolume(AudioInstance, uwVolume) != BSP_ERROR_NONE)
          {
            Display_string("Audio Vol Fail");
            while (JoyPinPressed != JOY_SEL)
            {
            }
            return 0;
          }
          Display_string(FreqStr);
          /* Reset Joystick state */
          JoyPinPressed  = JOY_NONE;
          break;

        case JOY_DOWN:
          /* Decrease volume by 5% */
          if (uwVolume > AUDIO_VOL_INC)
          {
            uwVolume -= AUDIO_VOL_INC;
          }
          else
          {
            uwVolume = AUDIO_VOL_MIN;
          }
          sprintf((char *)FreqStr, "VOL:%3ld", uwVolume);
          if (BSP_AUDIO_OUT_SetVolume(AudioInstance, uwVolume) != BSP_ERROR_NONE)
          {
            Display_string("Audio Vol Fail");
            while (JoyPinPressed != JOY_SEL)
            {
            }
            return 0;
          }
          Display_string(FreqStr);
          /* Reset Joystick state */
          JoyPinPressed  = JOY_NONE;
          break;

        case JOY_RIGHT:
          /* Set Resume */
          if (PauseEnabledStatus == 1)
          {
            /* Pause is enabled, call Resume */
            if (BSP_AUDIO_OUT_Resume(AudioInstance) != BSP_ERROR_NONE)
            {
              Display_string("Resume Fail");
              while (JoyPinPressed != JOY_SEL)
              {
              }
              return 0;
            }
            PauseEnabledStatus = 0;
            Display_string("PLAYING...");
            /* Reset Joystick state */
            JoyPinPressed  = JOY_NONE;
          }
          break;

        case JOY_LEFT:
          if (PauseEnabledStatus != 1)
          {
            /* Pause the playback */
            if (BSP_AUDIO_OUT_Pause(AudioInstance) != BSP_ERROR_NONE)
            {
              Display_string("Pause Fail");
              while (JoyPinPressed != JOY_SEL)
              {
              }
              return 0;
            }
            PauseEnabledStatus = 1;
            Display_string("PAUSE...");
            /* Reset Joystick state */
            JoyPinPressed  = JOY_NONE;
          }
          break;

        default:
          break;
        }
      }

      if (JoyPinPressed == JOY_SEL)
      {
        if (BSP_AUDIO_OUT_Stop(AudioInstance) != BSP_ERROR_NONE)
        {
          Display_string("Audio Stop Fail");
        }
        if (BSP_AUDIO_OUT_DeInit(AudioInstance) != BSP_ERROR_NONE)
        {
          Display_string("Aud. DeInit Fail");
        }
        Display_string("Exit");
        HAL_Delay(200);
        /* Reset Joystick state */
        JoyPinPressed  = JOY_NONE;
        return 0;
      }
    }
  }
}

/**
  * @brief  Display Audio demo hint
  * @param  None
  * @retval None
  */
static void Audio_SetHint(uint32_t Index)
{
  /* Clear the LCD */
  BSP_LCD_Clear(0, LCD_COLOR_BLACK);

  UTIL_LCD_DisplayStringAt(40, 0, (uint8_t *)"Audio Play", LEFT_MODE);
  BSP_LCD_DrawBitmap(0,  0,  0, (uint8_t *)st_logo_small);
  UTIL_LCD_DrawHLine(32, 13, 96, SSD1315_COLOR_WHITE);
  UTIL_LCD_DrawHLine(32, 14, 96, SSD1315_COLOR_WHITE);
  UTIL_LCD_DrawHLine(32, 15, 96, SSD1315_COLOR_WHITE);
  UTIL_LCD_DrawRect(0, 16, 128, 48, SSD1315_COLOR_WHITE);
  UTIL_LCD_DrawHLine(1, 16, 126, SSD1315_COLOR_BLACK);
  UTIL_LCD_SetFont(&Font8);
  UTIL_LCD_DisplayStringAt(71, 19, (uint8_t *)"Vol+", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(50, 28, (uint8_t *)"Pause   Resume", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(73, 36, (uint8_t *)"Vol-", LEFT_MODE);
  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_DisplayStringAt(5, 26, (uint8_t *)"Dir:", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(0, 50, (uint8_t *)"JOY SEL to Exit", CENTER_MODE);

  /* Refresh LCD to Display FrameBuffer */
  BSP_LCD_Refresh(0);
}


/**
  * @brief  Starts Audio streaming.
  * @param  None
  * @retval Audio error
  */
AUDIO_ErrorTypeDef AUDIO_Start(uint32_t *psrc_address, uint32_t file_size)
{
  uint32_t bytesread;

  buffer_ctl.state = BUFFER_OFFSET_NONE;
  buffer_ctl.AudioFileSize = file_size;
  buffer_ctl.SrcAddress = psrc_address;

  bytesread = GetData((void *)psrc_address,
                      0,
                      (uint8_t *)&buffer_ctl.buff[0],
                      AUDIO_BUFFER_SIZE * 2);
  if (bytesread > 0)
  {
    BSP_AUDIO_OUT_Play(AudioInstance, (uint8_t *)&buffer_ctl.buff[0], AUDIO_BUFFER_SIZE * 2);
    audio_state = AUDIO_STATE_PLAYING;
    buffer_ctl.fptr = bytesread;
    return AUDIO_ERROR_NONE;
  }
  return AUDIO_ERROR_IO;
}

/**
  * @brief  Manages Audio process.
  * @param  None
  * @retval Audio error
  */
uint8_t AUDIO_Process(void)
{
  uint32_t bytesread;
  AUDIO_ErrorTypeDef error_state = AUDIO_ERROR_NONE;

  switch (audio_state)
  {
    case AUDIO_STATE_PLAYING:

      if ((buffer_ctl.fptr + AUDIO_BUFFER_SIZE) >= buffer_ctl.AudioFileSize)
      {
        /* Play audio sample again ... */
        buffer_ctl.fptr = 0U;
        error_state = AUDIO_ERROR_EOF;
      }

      /* 1st half buffer played; so fill it and continue playing from bottom*/
      if (buffer_ctl.state == BUFFER_OFFSET_HALF)
      {
        bytesread = GetData((void *)buffer_ctl.SrcAddress,
                            buffer_ctl.fptr,
                            (uint8_t *)&buffer_ctl.buff[0],
                            AUDIO_BUFFER_SIZE);

        if (bytesread > 0)
        {
          buffer_ctl.state = BUFFER_OFFSET_NONE;
          buffer_ctl.fptr += bytesread;
        }
      }

      /* 2nd half buffer played; so fill it and continue playing from top */
      if (buffer_ctl.state == BUFFER_OFFSET_FULL)
      {
        bytesread = GetData((void *)buffer_ctl.SrcAddress,
                            buffer_ctl.fptr,
                            (uint8_t *)&buffer_ctl.buff[AUDIO_BUFFER_SIZE / 2],
                            AUDIO_BUFFER_SIZE);
        if (bytesread > 0)
        {
          buffer_ctl.state = BUFFER_OFFSET_NONE;
          buffer_ctl.fptr += bytesread;
        }
      }
      break;

    default:
      error_state = AUDIO_ERROR_NOTREADY;
      break;
  }
  return (uint8_t) error_state;
}

/**
  * @brief  Gets Data from storage unit.
  * @param  None
  * @retval None
  */
static uint32_t GetData(void *pdata, uint32_t offset, uint8_t *pbuf, uint32_t NbrOfData)
{
  uint8_t *lptr = pdata;
  uint32_t ReadDataNbr;

  ReadDataNbr = 0U;
  while (((offset + ReadDataNbr) < buffer_ctl.AudioFileSize) && (ReadDataNbr < NbrOfData))
  {
    pbuf[ReadDataNbr] = lptr [offset + ReadDataNbr];
    ReadDataNbr++;
  }
  return ReadDataNbr;
}

/*------------------------------------------------------------------------------
       Callbacks implementation:
           the callbacks API are defined __weak in the stm32h735g_discovery_audio.c file
           and their implementation should be done the user code if they are needed.
           Below some examples of callback implementations.
  ----------------------------------------------------------------------------*/
/**
  * @brief  Manages the full Transfer complete event.
  * @param  None
  * @retval None
  */
void BSP_AUDIO_OUT_TransferComplete_CallBack(uint32_t Instance)
{
  if (audio_state == AUDIO_STATE_PLAYING)
  {
    /* allows AUDIO_Process() to refill 2nd part of the buffer  */
    buffer_ctl.state = BUFFER_OFFSET_FULL;
  }
}

/**
  * @brief  Manages the DMA Half Transfer complete event.
  * @param  None
  * @retval None
  */
void BSP_AUDIO_OUT_HalfTransfer_CallBack(uint32_t Instance)
{
  if (audio_state == AUDIO_STATE_PLAYING)
  {
    /* allows AUDIO_Process() to refill 1st part of the buffer  */
    buffer_ctl.state = BUFFER_OFFSET_HALF;
  }
}

/**
  * @brief  Manages the DMA FIFO error event.
  * @param  None
  * @retval None
  */
void BSP_AUDIO_OUT_Error_CallBack(uint32_t Instance)
{
  Display_string("DMA  ERROR");

  /* Stop the program with an infinite loop */
  while (BSP_JOY_GetState(JOY1) != JOY_NONE)
  {
    return;
  }

  /* could also generate a system reset to recover from the error */
  /* .... */
}


/**
  * @}
  */

/**
  * @}
  */

