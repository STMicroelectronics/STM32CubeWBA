/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    wav_flash.c
  * @brief   Flash-based WAV file reader implementation.
  *          Reads a standard 44-byte PCM WAV stored as a const array in
  *          internal Flash (see audio_data.c / audio_data.h).
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

#include "wav_flash.h"
#include "audio_data.h"
#include <string.h>

typedef struct __attribute__((packed))
{
  char   ChunkID[4];       /* "RIFF"                          */
  ULONG  ChunkSize;        /* File size - 8                   */
  char   Format[4];        /* "WAVE"                          */
  char   Subchunk1ID[4];   /* "fmt "                          */
  ULONG  Subchunk1Size;    /* 16 for PCM                      */
  USHORT AudioFormat;      /* 1 = PCM                         */
  USHORT NumChannels;      /* 1 = mono, 2 = stereo            */
  ULONG  SampleRate;       /* e.g. 44100, 48000               */
  ULONG  ByteRate;         /* SampleRate * NumChannels * bps/8*/
  USHORT BlockAlign;       /* NumChannels * bps / 8           */
  USHORT BitsPerSample;    /* 8, 16, 24, 32                   */
  char   Subchunk2ID[4];   /* "data"                          */
  ULONG  Subchunk2Size;    /* Audio payload size in bytes     */
} WAV_HeaderTypeDef;

/* Compile-time check: the header struct must be exactly 44 bytes */
typedef char _wav_header_size_check[(sizeof(WAV_HeaderTypeDef) == WAV_DATA_OFFSET) ? 1 : -1];

void WAV_Flash_GetInfo(WAV_InfoTypedef *info)
{
  const WAV_HeaderTypeDef *hdr = (const WAV_HeaderTypeDef *)audio_wav_data;

  info->SampleRate    = hdr->SampleRate;
  info->NbrChannels   = hdr->NumChannels;
  info->BitPerSample  = hdr->BitsPerSample;
  info->ByteRate      = hdr->ByteRate;
  info->SubChunk2Size = hdr->Subchunk2Size;
  info->FileSize      = audio_wav_size;
}

ULONG WAV_Flash_Read(UCHAR *buf, ULONG size, ULONG *offset)
{
  ULONG available;
  ULONG to_read;

  if (*offset >= (ULONG)audio_wav_size)
  {
    return 0U;
  }

  available = (ULONG)audio_wav_size - *offset;
  to_read   = (size <= available) ? size : available;

  memcpy(buf, audio_wav_data + *offset, (size_t)to_read);
  *offset += to_read;

  return to_read;
}
