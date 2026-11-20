/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    wav_flash.h
  * @brief   WAV reader from internal Flash memory.
  *
  *          This module reads a PCM WAV file stored as a constant array
  *          in internal Flash memory.
  *
  *          Assumptions:
  *          - Standard 44-byte RIFF/WAVE PCM header.
  *          - No additional chunks between "fmt " and "data".
  *          - WAV data declared in audio_data.c / audio_data.h.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __WAV_FLASH_H__
#define __WAV_FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "tx_api.h"

/* Standard PCM WAV header size */
#define WAV_DATA_OFFSET  44U

/**
  * @brief  Application-level WAV file information.
  */
typedef struct
{
  ULONG  FileSize;       /*!< Total file size in bytes         */
  ULONG  SampleRate;     /*!< Sample rate in Hz                */
  ULONG  ByteRate;       /*!< Byte rate                        */
  ULONG  SubChunk2Size;  /*!< Audio data payload size in bytes */
  USHORT NbrChannels;    /*!< Number of channels               */
  USHORT BitPerSample;   /*!< Number of bits per sample        */
} WAV_InfoTypedef;

/**
  * @brief  Get WAV header information from Flash memory.
  * @param  info  Pointer to a WAV_InfoTypedef structure to fill.
  */
void WAV_Flash_GetInfo(WAV_InfoTypedef *info);

/**
  * @brief  Read audio data from Flash memory.
  * @param  buf     Pointer to destination buffer.
  * @param  size    Number of bytes to read.
  * @param  offset  Pointer to current offset in WAV data.
  * @retval Number of bytes read.
  */
ULONG WAV_Flash_Read(UCHAR *buf, ULONG size, ULONG *offset);

#ifdef __cplusplus
}
#endif

#endif /* __WAV_FLASH_H__ */
