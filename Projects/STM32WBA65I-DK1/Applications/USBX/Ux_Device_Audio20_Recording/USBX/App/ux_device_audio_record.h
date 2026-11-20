/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_device_audio_record.h
  * @author  MCD Application Team
  * @brief   USBX Device audio recording applicative header file
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UX_DEVICE_AUDIO_RECORD_H__
#define __UX_DEVICE_AUDIO_RECORD_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ux_api.h"
#include "ux_device_class_audio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ux_device_descriptors.h"
#include "stm32wba65i_discovery_audio.h"
#include <stdbool.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
VOID USBD_AUDIO_RecordingStreamChange(UX_DEVICE_CLASS_AUDIO_STREAM *audio_record_stream,
                                     ULONG alternate_setting);
VOID USBD_AUDIO_RecordingStreamFrameDone(UX_DEVICE_CLASS_AUDIO_STREAM *audio_record_stream,
                                        ULONG length);
ULONG USBD_AUDIO_RecordingStreamGetMaxFrameBufferNumber(VOID);
ULONG USBD_AUDIO_RecordingStreamGetMaxFrameBufferSize(VOID);

/* USER CODE BEGIN EFP */
VOID usbx_audio_record_app_thread(ULONG arg);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define AUDIO_BLOCK_SIZE   3840U
#define PACKET_LENGTH      12U

typedef enum
{
  AUDIOMIC_STOP = 0,
  AUDIOMIC_Init,
  AUDIOMIC_START,
  AUDIOMIC_PLAY,
  AUDIOMIC_PAUSE,
  AUDIOMIC_EOF,
  AUDIOMIC_ERROR,
}AUDIO_MicStateTypeDef ;

typedef enum
{
  BUFFER_OFFSET_NONE = 0,
  BUFFER_OFFSET_HALF = 1,
  BUFFER_OFFSET_FULL = 2,
}BUFFER_StateTypeDef;

/* Audio buffer control structure */
typedef struct
{
  volatile AUDIO_MicStateTypeDef state;
  volatile BUFFER_StateTypeDef   buffer_offset;  /* empty (none), half, full*/
  volatile uint32_t              write_ptr;
  volatile bool                  ready;
  uint16_t              internal_buffer[AUDIO_BLOCK_SIZE];
  uint16_t              alt_buffer[PACKET_LENGTH];
} AUDIO_IN_BufferCtl_t;
/* USER CODE END PD */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

#ifdef __cplusplus
}
#endif
#endif  /* __UX_DEVICE_AUDIO_RECORD_H__ */
