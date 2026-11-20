/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    audio_data.h
  * @brief   External declarations for the WAV audio array stored in Flash memory.
  *
  *          The WAV data array is defined in audio_data.c

  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __AUDIO_DATA_H__
#define __AUDIO_DATA_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "tx_api.h"

/** @brief Pointer to the start of the embedded WAV file in Flash. */
extern const UCHAR  audio_wav_data[];

/** @brief Total size of the embedded WAV file in bytes (header + payload). */
extern const ULONG  audio_wav_size;

#ifdef __cplusplus
}
#endif

#endif /* __AUDIO_DATA_H__ */
