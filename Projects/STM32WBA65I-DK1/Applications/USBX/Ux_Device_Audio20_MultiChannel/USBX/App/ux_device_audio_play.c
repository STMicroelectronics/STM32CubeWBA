/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_device_audio_play.c
  * @author  MCD Application Team
  * @brief   USBX Device Audio Playback applicative source file
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

/* Includes ------------------------------------------------------------------*/
#include "ux_device_audio_play.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ux_device_audio_record.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#ifdef UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT
#define BUF_OVERRUN_THRESHOLD                            ((3U * AUDIO_TOTAL_BUF_SIZE) / 4U)
#define BUF_UNDERRUN_THRESHOLD                           (AUDIO_TOTAL_BUF_SIZE / 4U)
#endif /* UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT */
#define AUDIO_HS_SERVICE_INTERVALS_PER_SECOND           (8000U / (1U << (USBD_AUDIO_PLAY_EPOUT_HS_BINTERVAL - 1U)))
#define AUDIO_FS_SERVICE_INTERVALS_PER_SECOND           (1000U / (1U << (USBD_AUDIO_PLAY_EPOUT_FS_BINTERVAL - 1U)))
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
/* Single circular buffer for Output Audio stream */
__ALIGN_BEGIN AUDIO_OUT_BufferTypeDef BufferCtl __ALIGN_END;
AUDIO_DescriptionTypeDef PlaybackAudioDescription;
#ifdef UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT
static uint8_t feedback_buffer[4];
#endif /* UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static VOID USBD_AUDIO_PlaybackResetState(VOID);
#ifdef UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT
static uint32_t Audio_get_feedback(uint8_t speed);
static VOID USBD_AUDIO_PlaybackFeedbackEncode(uint32_t feedback_data_rate, uint8_t *feedback_buffer);
#endif /* UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
  * @brief  USBD_AUDIO_PlaybackResetState
  *         This function resets the playback loopback buffer state.
  * @param  none
  * @retval none
  */
static VOID USBD_AUDIO_PlaybackResetState(VOID)
{
  BufferCtl.rd_ptr = 0U;
  BufferCtl.wr_ptr = 0U;
  ux_utility_memory_set(BufferCtl.buff, 0, sizeof(BufferCtl.buff));
}

/* USER CODE END 0 */

/**
  * @brief  USBD_AUDIO_PlaybackStreamChange
  *         This function is invoked to inform application that the
  *         alternate setting are changed.
  * @param  audio_play_stream: Pointer to audio playback class stream instance.
  * @param  alternate_setting: interface alternate setting.
  * @retval none
  */
VOID USBD_AUDIO_PlaybackStreamChange(UX_DEVICE_CLASS_AUDIO_STREAM *audio_play_stream,
                                     ULONG alternate_setting)
{
  /* USER CODE BEGIN USBD_AUDIO_PlaybackStreamChange */
  USBD_AUDIO_PlaybackResetState();

  if (alternate_setting == 0U)
  {
    return;
  }

  /* Start reception when the streaming alternate setting opens. */
  ux_device_class_audio_reception_start(audio_play_stream);

  /* USER CODE END USBD_AUDIO_PlaybackStreamChange */

  return;
}

/**
  * @brief  USBD_AUDIO_PlaybackStreamFrameDone
  *         This function is invoked whenever a USB packet (audio frame) is received
  *         from the host.
  * @param  audio_play_stream: Pointer to audio playback class stream instance.
  * @param  length: transfer length.
  * @retval none
  */
VOID USBD_AUDIO_PlaybackStreamFrameDone(UX_DEVICE_CLASS_AUDIO_STREAM *audio_play_stream,
                                        ULONG length)
{
  /* USER CODE BEGIN USBD_AUDIO_PlaybackStreamFrameDone */
  UCHAR *frame_buffer;
  ULONG frame_length;

#ifdef UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT
  uint32_t feedback_data_rate;
  uint8_t speed;
#endif /* UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT */

  /* Get access to the first audio OUT frame. */
  ux_device_class_audio_read_frame_get(audio_play_stream, &frame_buffer, &frame_length);

  if (length != 0U)
  {
    /* Store each received OUT packet in the shared loopback buffer. */
    USBD_AUDIO_Loopback_PushFrame(frame_buffer, frame_length);
  }

  /* Release the frame so USBX can reuse its buffer. */
  ux_device_class_audio_read_frame_free(audio_play_stream);

#ifdef UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT

  /* Get the active USB bus speed. */
  speed = ux_device_class_audio_speed_get(audio_play_stream);

  /* Derive the feedback value from the current buffer level. */
  feedback_data_rate = Audio_get_feedback(speed);

  /* Prepare the feedback buffer here. */
  USBD_AUDIO_PlaybackFeedbackEncode(feedback_data_rate, feedback_buffer);

  /* Submit the feedback payload for the next IN transaction. */
  ux_device_class_audio_feedback_set(audio_play_stream, feedback_buffer);
#endif /* UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT */

  /* USER CODE END USBD_AUDIO_PlaybackStreamFrameDone */

  return;
}

/**
  * @brief  USBD_AUDIO_PlaybackStreamGetMaxFrameBufferNumber
  *         This function is invoked to set audio playback stream max frame buffer number.
  * @param  none
  * @retval max frame buffer number
  */
ULONG USBD_AUDIO_PlaybackStreamGetMaxFrameBufferNumber(VOID)
{
  ULONG max_frame_buffer_number = 0U;

  /* USER CODE BEGIN USBD_AUDIO_PlaybackStreamGetMaxFrameBufferNumber */

  max_frame_buffer_number = 3U;

  /* USER CODE END USBD_AUDIO_PlaybackStreamGetMaxFrameBufferNumber */

  return max_frame_buffer_number;
}

/**
  * @brief  USBD_AUDIO_PlaybackStreamGetMaxFrameBufferSize
  *         This function is invoked to set audio playback stream max frame buffer size.
  * @param  none
  * @retval max frame buffer size
  */
ULONG USBD_AUDIO_PlaybackStreamGetMaxFrameBufferSize(VOID)
{
  ULONG max_frame_buffer_size = 0U;

  /* USER CODE BEGIN USBD_AUDIO_PlaybackStreamGetMaxFrameBufferSize */

  max_frame_buffer_size = USBD_AUDIO_PLAY_EPOUT_HS_MPS;

  /* USER CODE END USBD_AUDIO_PlaybackStreamGetMaxFrameBufferSize */

  return max_frame_buffer_size;
}

/* USER CODE BEGIN 1 */

/**
  * @brief  USBD_AUDIO_PlaybackInit
  *         Initializes playback stream runtime parameters.
  * @param  none
  * @retval status
  */
UINT USBD_AUDIO_PlaybackInit(VOID)
{
  PlaybackAudioDescription.audio_frequency      = USBD_AUDIO_PLAY_DEFAULT_FREQ;
  PlaybackAudioDescription.audio_channels_count = USBD_AUDIO_PLAY_CHANNEL_COUNT;
  PlaybackAudioDescription.audio_resolution     = USBD_AUDIO_PLAY_RES_BYTE;
  BufferCtl.size                                = AUDIO_TOTAL_BUF_SIZE;

  return UX_SUCCESS;
}

#ifdef UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT
/**
  * @brief  Calculates the playback feedback payload value from the ring buffer level.
  * @param  speed: USB device speed.
  * @retval Feedback payload value in fixed-point samples per service interval.
  */
static uint32_t Audio_get_feedback(uint8_t speed)
{
  uint32_t filled_size;
  uint32_t distance_size;
  uint32_t frame_size_byte;
  uint32_t correction_hz;
  uint32_t feedback_data_rate;
  uint32_t feedback_frequency_hz;

  /* Compute the current loopback buffer occupancy. */
  filled_size = USB_AUDIO_BUFFER_Filled_SIZE(BufferCtl);

  /* Convert the byte occupancy into audio frames before deriving a frequency correction. */
  frame_size_byte = PlaybackAudioDescription.audio_channels_count * PlaybackAudioDescription.audio_resolution;
  feedback_frequency_hz = PlaybackAudioDescription.audio_frequency;

  if (filled_size < BUF_UNDERRUN_THRESHOLD)
  {
    /* Increase the feedback frequency when the buffer level drops below the target threshold. */
    distance_size = BUF_UNDERRUN_THRESHOLD - filled_size;
    correction_hz = distance_size / frame_size_byte;

    feedback_frequency_hz += correction_hz;
  }
  else if (filled_size > BUF_OVERRUN_THRESHOLD)
  {
    /* Decrease the feedback frequency when the buffer level rises above the target threshold. */
    distance_size = filled_size - BUF_OVERRUN_THRESHOLD;
    correction_hz = distance_size / frame_size_byte;

    feedback_frequency_hz -= correction_hz;
  }

  if (speed == UX_HIGH_SPEED_DEVICE)
  {
    /* High-speed feedback always uses 16.16 sample frames per service interval. */
    feedback_data_rate = (uint32_t)(((((uint64_t)feedback_frequency_hz) << 16)
                                     + (AUDIO_HS_SERVICE_INTERVALS_PER_SECOND / 2U))
                                    / AUDIO_HS_SERVICE_INTERVALS_PER_SECOND);
  }
  else if (USBD_AUDIO_PLAY_EP_FEEDBACK_FS_MPS > 3U)
  {
    /* Keep full-speed feedback in 16.16 by default to work around the Windows
    Audio 2.0 full-speed feedback handling issue. */
    feedback_data_rate = (uint32_t)(((((uint64_t)feedback_frequency_hz) << 16)
                                     + (AUDIO_FS_SERVICE_INTERVALS_PER_SECOND / 2U))
                                    / AUDIO_FS_SERVICE_INTERVALS_PER_SECOND);
  }
  else
  {
    /* Switch back to 10.14 only when the user explicitly sets the FS feedback MPS to 3 bytes. */
    feedback_data_rate = (uint32_t)(((((uint64_t)feedback_frequency_hz) << 14)
                                     + (AUDIO_FS_SERVICE_INTERVALS_PER_SECOND / 2U))
                                    / AUDIO_FS_SERVICE_INTERVALS_PER_SECOND);
  }

  return feedback_data_rate;
}

/**
  * @brief  USBD_AUDIO_PlaybackFeedbackEncode
  *         This function always prepares the 4-byte local feedback buffer.
  *         The middleware uses the configured endpoint request length to send 3 or 4 bytes.
  * @param  feedback_data_rate: Feedback data rate in fixed-point format.
  * @param  feedback_buffer: Pointer to feedback payload buffer.
  * @retval none
  */
static VOID USBD_AUDIO_PlaybackFeedbackEncode(uint32_t feedback_data_rate, uint8_t *feedback_buffer)
{
  feedback_buffer[0] = (uint8_t)(feedback_data_rate & 0xFFU);
  feedback_buffer[1] = (uint8_t)((feedback_data_rate >> 8) & 0xFFU);
  feedback_buffer[2] = (uint8_t)((feedback_data_rate >> 16) & 0xFFU);
  feedback_buffer[3] = (uint8_t)((feedback_data_rate >> 24) & 0xFFU);
}
#endif /* UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT */
/* USER CODE END 1 */
