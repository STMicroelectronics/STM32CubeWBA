/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_device_audio_record.c
  * @author  MCD Application Team
  * @brief   USBX Device Audio Recording applicative source file
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
#include "ux_device_audio_record.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ux_device_audio_play.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define USBD_AUDIO_RECORD_NOMINAL_PACKET_SIZE_FS      ((USBD_AUDIO_RECORD_DEFAULT_FREQ * \
                                                        USBD_AUDIO_RECORD_CHANNEL_COUNT * \
                                                        USBD_AUDIO_RECORD_RES_BYTE) / \
                                                       (1000U / \
                                                        (1UL << (USBD_AUDIO_RECORD_EPIN_FS_BINTERVAL - 1U))))

#define USBD_AUDIO_RECORD_NOMINAL_PACKET_SIZE_HS      ((USBD_AUDIO_RECORD_DEFAULT_FREQ * \
                                                        USBD_AUDIO_RECORD_CHANNEL_COUNT * \
                                                        USBD_AUDIO_RECORD_RES_BYTE) / \
                                                       (8000U / \
                                                        (1UL << (USBD_AUDIO_RECORD_EPIN_HS_BINTERVAL - 1U))))

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
extern AUDIO_OUT_BufferTypeDef BufferCtl;
__ALIGN_BEGIN static UCHAR loopback_out_frame[USBD_AUDIO_PLAY_MAX_PACKET_SIZE] __ALIGN_END;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static VOID USBD_AUDIO_RecordingResetState(VOID);
static ULONG USBD_AUDIO_RecordingGetMaxPacketSize(VOID);
static ULONG USBD_AUDIO_RecordingGetNominalPacketSize(uint8_t speed);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
  * @brief  USBD_AUDIO_RecordingResetState
  *         This function resets the loopback ring buffer state used by the recording stream.
  * @param  none
  * @retval none
  */
static VOID USBD_AUDIO_RecordingResetState(VOID)
{
  BufferCtl.rd_ptr = 0U;
  BufferCtl.wr_ptr = 0U;
  ux_utility_memory_set(loopback_out_frame, 0, sizeof(loopback_out_frame));
}

/**
  * @brief  USBD_AUDIO_RecordingGetMaxPacketSize
  *         This function returns the largest recording packet size across USB speeds.
  * @param  none
  * @retval Largest recording packet size in bytes
  */
static ULONG USBD_AUDIO_RecordingGetMaxPacketSize(VOID)
{
  ULONG max_packet_size;

  if (USBD_AUDIO_RECORD_EPIN_HS_MPS > USBD_AUDIO_RECORD_EPIN_FS_MPS)
  {
    max_packet_size = USBD_AUDIO_RECORD_EPIN_HS_MPS;
  }
  else
  {
    max_packet_size = USBD_AUDIO_RECORD_EPIN_FS_MPS;
  }

  return max_packet_size;
}

/**
  * @brief  USBD_AUDIO_RecordingGetNominalPacketSize
  *         This function returns the nominal loopback payload size for the active USB speed.
  * @param  speed: USB device speed.
  * @retval Nominal payload size in bytes
  */
static ULONG USBD_AUDIO_RecordingGetNominalPacketSize(uint8_t speed)
{
  ULONG nominal_packet_size;

  if (speed == UX_HIGH_SPEED_DEVICE)
  {
    nominal_packet_size = (ULONG)USBD_AUDIO_RECORD_NOMINAL_PACKET_SIZE_HS;
  }
  else
  {
    nominal_packet_size = (ULONG)USBD_AUDIO_RECORD_NOMINAL_PACKET_SIZE_FS;
  }

  return nominal_packet_size;
}

/**
  * @brief  Store one received playback frame in the loopback ring buffer.
  * @param  data: Pointer to source frame data to store.
  * @param  length: Number of bytes to push into the ring buffer.
  * @retval UX_SUCCESS on success, UX_ERROR if parameters or buffer state are invalid.
  */
UINT USBD_AUDIO_Loopback_PushFrame(const UCHAR *data, ULONG length)
{
  ULONG filled_size;
  ULONG tail_length;
  ULONG head_length;

  if ((data == UX_NULL) || (length == 0U))
  {
    return UX_ERROR;
  }

  if ((BufferCtl.size == 0U) || (length > BufferCtl.size))
  {
    return UX_ERROR;
  }

  filled_size = USB_AUDIO_BUFFER_Filled_SIZE(BufferCtl);

  if (length >= (BufferCtl.size - filled_size))
  {
    /* Reject only when the total free space is exhausted.
       Crossing the buffer end is handled below by splitting the copy, but one byte must remain
       reserved so wr_ptr == rd_ptr always keeps its empty-buffer meaning. */
    return UX_ERROR;
  }

  /* Bytes available before reaching the buffer end. */
  tail_length = BufferCtl.size - BufferCtl.wr_ptr;

  if (length <= tail_length)
  {
    ux_utility_memory_copy(&BufferCtl.buff[BufferCtl.wr_ptr], (VOID *)data, length);
  }

  else
  {
    /* Finish at the buffer end, then continue from index 0. */
    head_length = length - tail_length;

    ux_utility_memory_copy(&BufferCtl.buff[BufferCtl.wr_ptr], (VOID *)data, tail_length);
    ux_utility_memory_copy(&BufferCtl.buff[0], (VOID *)&data[tail_length], head_length);
  }

  BufferCtl.wr_ptr += length;
  if (BufferCtl.wr_ptr >= BufferCtl.size)
  {
    BufferCtl.wr_ptr -= BufferCtl.size;
  }

  return UX_SUCCESS;
}

/**
  * @brief  Read one continuous memory block from the loopback ring buffer.
  * @param  data: Pointer to destination buffer for popped data.
  * @param  length: Pointer updated with number of bytes actually popped.
  * @param  max_length: Maximum number of bytes allowed to pop.
  * @retval UX_SUCCESS when data is available, UX_ERROR when buffer is empty or invalid.
  */
UINT USBD_AUDIO_Loopback_PopFrame(UCHAR *data, ULONG *length, ULONG max_length)
{
  ULONG filled_size;
  ULONG actual_length;
  ULONG tail_length;
  ULONG head_length;

  if ((data == UX_NULL) || (length == UX_NULL) || (max_length == 0U))
  {
    return UX_ERROR;
  }

  if (BufferCtl.size == 0U)
  {
    return UX_ERROR;
  }

  filled_size = USB_AUDIO_BUFFER_Filled_SIZE(BufferCtl);

  /* Nothing is buffered yet, so there is nothing to read. */
  if (filled_size == 0U)
  {
    return UX_ERROR;
  }

  actual_length = filled_size;
  if (actual_length > max_length)
  {
    actual_length = max_length;
  }

  /* Bytes available before reaching the buffer end. */
  tail_length = BufferCtl.size - BufferCtl.rd_ptr;

  if (actual_length <= tail_length)
  {
    ux_utility_memory_copy(data, &BufferCtl.buff[BufferCtl.rd_ptr], actual_length);
  }

  else
  {
    /* Read to the buffer end, then continue from index 0. */
    head_length = actual_length - tail_length;

    ux_utility_memory_copy(data, &BufferCtl.buff[BufferCtl.rd_ptr], tail_length);
    ux_utility_memory_copy(&data[tail_length], &BufferCtl.buff[0], head_length);
  }

  *length = actual_length;
  BufferCtl.rd_ptr += actual_length;
  if (BufferCtl.rd_ptr >= BufferCtl.size)
  {
    BufferCtl.rd_ptr -= BufferCtl.size;
  }

  return UX_SUCCESS;
}

/* USER CODE END 0 */

/**
  * @brief  USBD_AUDIO_RecordingStreamChange
  *         This function is invoked to inform application that the
  *         alternate setting are changed.
  * @param  audio_record_stream: Pointer to audio recording class stream instance.
  * @param  alternate_setting: interface alternate setting.
  * @retval none
  */
VOID USBD_AUDIO_RecordingStreamChange(UX_DEVICE_CLASS_AUDIO_STREAM *audio_record_stream,
                                      ULONG alternate_setting)
{
  /* USER CODE BEGIN USBD_AUDIO_RecordingStreamChange */
  ULONG nominal_packet_size;
  uint8_t speed;

  if (alternate_setting == 0U)
  {
    USBD_AUDIO_RecordingResetState();
    return;
  }

  USBD_AUDIO_RecordingResetState();

  speed = ux_device_class_audio_speed_get(audio_record_stream);
  nominal_packet_size = USBD_AUDIO_RecordingGetNominalPacketSize(speed);

  /* Prime the first IN packet before starting transmission. */
  ux_device_class_audio_frame_write(audio_record_stream, loopback_out_frame, nominal_packet_size);
  ux_device_class_audio_transmission_start(audio_record_stream);

  /* USER CODE END USBD_AUDIO_RecordingStreamChange */

  return;
}

/**
  * @brief  USBD_AUDIO_RecordingStreamFrameDone
  *         This function is invoked whenever a USB packet (audio frame) is received
  *         from the host.
  * @param  audio_record_stream: Pointer to audio recording class stream instance.
  * @param  length: transfer length.
  * @retval none
  */
VOID USBD_AUDIO_RecordingStreamFrameDone(UX_DEVICE_CLASS_AUDIO_STREAM *audio_record_stream,
                                        ULONG length)
{
  /* USER CODE BEGIN USBD_AUDIO_RecordingStreamFrameDone */
  ULONG data_length = 0U;
  ULONG nominal_packet_size;
  ULONG max_packet_size;
  UINT pop_status;
  uint8_t speed;

  UX_PARAMETER_NOT_USED(length);

  speed = ux_device_class_audio_speed_get(audio_record_stream);
  max_packet_size = USBD_AUDIO_RecordingGetMaxPacketSize();
  nominal_packet_size = USBD_AUDIO_RecordingGetNominalPacketSize(speed);

  pop_status = USBD_AUDIO_Loopback_PopFrame(loopback_out_frame, &data_length, max_packet_size);

  if (pop_status != UX_SUCCESS)
  {
    /* Send one nominal silent packet when no loopback data is available. */
    ux_utility_memory_set(loopback_out_frame, 0, nominal_packet_size);
    data_length = nominal_packet_size;
  }

  ux_device_class_audio_frame_write(audio_record_stream, loopback_out_frame, data_length);

  /* USER CODE END USBD_AUDIO_RecordingStreamFrameDone */

  return;
}

/**
  * @brief  USBD_AUDIO_RecordingStreamGetMaxFrameBufferNumber
  *         This function is invoked to set audio recording stream max frame buffer number.
  * @param  none
  * @retval max frame buffer number
  */
ULONG USBD_AUDIO_RecordingStreamGetMaxFrameBufferNumber(VOID)
{
  ULONG max_frame_buffer_number = 0U;

  /* USER CODE BEGIN USBD_AUDIO_RecordingStreamGetMaxFrameBufferNumber */
  max_frame_buffer_number = 3U;

  /* USER CODE END USBD_AUDIO_RecordingStreamGetMaxFrameBufferNumber */

  return max_frame_buffer_number;
}

/**
  * @brief  USBD_AUDIO_RecordingStreamGetMaxFrameBufferSize
  *         This function is invoked to set audio recording stream max frame buffer size.
  * @param  none
  * @retval max frame buffer size
  */
ULONG USBD_AUDIO_RecordingStreamGetMaxFrameBufferSize(VOID)
{
  ULONG max_frame_buffer_size = 0U;

  /* USER CODE BEGIN USBD_AUDIO_RecordingStreamGetMaxFrameBufferSize */
  max_frame_buffer_size = USBD_AUDIO_RECORD_EPIN_HS_MPS;

  /* USER CODE END USBD_AUDIO_RecordingStreamGetMaxFrameBufferSize */

  return max_frame_buffer_size;
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
