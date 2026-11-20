/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_device_audio_record.c
  * @author  MCD Application Team
  * @brief   USBX Device Audio applicative source file
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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
extern TX_SEMAPHORE semaphore_half_transfer;
extern TX_SEMAPHORE semaphore_full_transfer;

/* Double BUFFER for Input Audio stream */
__ALIGN_BEGIN AUDIO_IN_BufferCtl_t BufferCtl __ALIGN_END;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  if (alternate_setting == 0U)
  {
    BufferCtl.buffer_offset = BUFFER_OFFSET_NONE;
  }

  BufferCtl.state = AUDIOMIC_START;

  ux_device_class_audio_frame_write(audio_record_stream, (UCHAR *)BufferCtl.alt_buffer, PACKET_LENGTH);

  /* Start sending valid frames in the audio class */
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
  if (!BufferCtl.ready)
  {
    /* Buffer is not ready */
    ux_device_class_audio_frame_write(audio_record_stream,
                                      (UCHAR *)BufferCtl.alt_buffer,
                                      PACKET_LENGTH);
    return;
  }
  /* Use internal buffer with an index */
  if (BufferCtl.buffer_offset == BUFFER_OFFSET_HALF || BufferCtl.buffer_offset == BUFFER_OFFSET_FULL)
  {
    ux_device_class_audio_frame_write(audio_record_stream,
                                      (UCHAR *)(BufferCtl.internal_buffer + BufferCtl.write_ptr),
                                      PACKET_LENGTH);
    BufferCtl.write_ptr += PACKET_LENGTH / 2U; /* 12 bytes / 2 bytes per sample = 6 samples */
    if (BufferCtl.write_ptr >= AUDIO_BLOCK_SIZE)
    {
      BufferCtl.write_ptr = 0U;
    }
  }
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

/**
  * @brief  Function implementing the audio recording thread.
  * @param  arg: Not used
  * @retval None
  */
VOID usbx_audio_record_app_thread(ULONG arg)
{
  while (1U)
  {
    if (BufferCtl.state == AUDIOMIC_START)
    {
      /* Start Recording: Size in number of bytes */
      if (BSP_AUDIO_IN_Record(0U, (uint8_t*)BufferCtl.internal_buffer, 2U * AUDIO_BLOCK_SIZE) != BSP_ERROR_NONE)
      {
        BufferCtl.state = AUDIOMIC_ERROR;
      }

      while (BufferCtl.state == AUDIOMIC_START)
      {
        /* Wait for half block recording to complete */
        tx_semaphore_get(&semaphore_half_transfer, TX_WAIT_FOREVER);
        if (BufferCtl.state == AUDIOMIC_START)
        {
          BufferCtl.buffer_offset = BUFFER_OFFSET_HALF;
          BufferCtl.ready = true;
        }

        /* Wait for full block recording to complete */
        tx_semaphore_get(&semaphore_full_transfer, TX_WAIT_FOREVER);
        if (BufferCtl.state == AUDIOMIC_START)
        {
          BufferCtl.buffer_offset = BUFFER_OFFSET_FULL;
          BufferCtl.ready = true;
        }
      }
    }
    else
    {
      tx_thread_sleep(1U);
    }
  }
}

/**
  * @brief  Callback for transfer complete.
  * @param  Instance: Audio instance.
  * @retval None
  */
void BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance)
{
  BufferCtl.buffer_offset = BUFFER_OFFSET_FULL;
  tx_semaphore_put(&semaphore_full_transfer);
}

/**
  * @brief  Callback for half transfer complete.
  * @param  Instance: Audio instance.
  * @retval None
  */
void BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance)
{
  BufferCtl.buffer_offset = BUFFER_OFFSET_HALF;
  tx_semaphore_put(&semaphore_half_transfer);
}
/* USER CODE END 1 */
