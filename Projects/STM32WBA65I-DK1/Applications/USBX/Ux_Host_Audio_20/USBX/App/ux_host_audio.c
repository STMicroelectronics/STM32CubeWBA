/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_host_audio.c
  * @author  MCD Application Team
  * @brief   USBX Host Audio applicative source file
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
#include "ux_host_audio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_usbx_host.h"
#include "wav_flash.h"
#include "stm32wba65i_discovery.h"
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
extern UX_HOST_CLASS_AUDIO *audio_playback;
extern TX_SEMAPHORE data_send_semaphore;

UINT joystick_state;
UINT joystick_pressed = 0U;
audio_playback_stateTypeDef audio_playback_state;
audio_playback_stateTypeDef audio_playback_prev_state;

UX_HOST_CLASS_AUDIO_CONTROL audio_speaker_control1;
UX_HOST_CLASS_AUDIO_CONTROL audio_speaker_control2;
UX_HOST_CLASS_AUDIO_CONTROL audio_speaker_mute;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* USER CODE BEGIN 1 */
/**
  * @brief  Function implementing audio_playback_thread_entry.
  * @param  thread_input: not used
  * @retval none
  */
VOID audio_playback_thread_entry(ULONG thread_input)
{
  UINT status;
  SHORT min, max;
  ULONG duration;
  WAV_InfoTypedef WavPlayInfo;
  static UINT double_buffer_index;
  UCHAR audio_play_feature_unit_id;
  UCHAR audio_frame_length;
  ULONG audio_requested_length;
  ULONG audio_data_offset;
  ULONG audio_buffer_size;
  UCHAR *local_frame_buffer1;
  UCHAR *local_frame_buffer2;
  UCHAR *actual_frame_buffer;
  UX_HOST_CLASS_AUDIO_SAMPLING audio_play_sampling;
  UX_HOST_CLASS_AUDIO_TRANSFER_REQUEST transfer_request1;
  UX_HOST_CLASS_AUDIO_TRANSFER_REQUEST transfer_request2;
  UX_HOST_CLASS_AUDIO_TRANSFER_REQUEST *actual_transfer_request;
  UX_HOST_CLASS_AUDIO_TRANSFER_REQUEST *previous_transfer_request;
  UX_SEMAPHORE host_audio_transfer_request1_semaphore;
  UX_SEMAPHORE host_audio_transfer_request2_semaphore;
  UX_PARAMETER_NOT_USED(thread_input);

  while(1)
  {
    /* Start if the audio device speaker is connected */
    if ((audio_playback != NULL) &&
        (audio_playback->ux_host_class_audio_state == UX_HOST_CLASS_INSTANCE_LIVE))
    {

      switch(audio_playback_state)
      {
        case AUDIO_PLAYBACK_IDLE:

          /* Select Audio Streaming alternate 0 */
          ux_host_stack_interface_setting_select(audio_playback->ux_host_class_audio_streaming_interface);

          USBH_UsrLog("\nTo start Playback, please press the Joystick select.\n");

          /* Store current JOY_SEL state to detect next press */
          joystick_state = BSP_JOY_GetState(JOY1);
          if ((joystick_state & JOY_SEL) != 0U)
          {
              joystick_pressed = 1U;
          }
          else
          {
              joystick_pressed = 0U;
          }

          audio_playback_state = AUDIO_PLAYBACK_WAIT;
          audio_playback_prev_state = AUDIO_PLAYBACK_IDLE;

          break;

        case AUDIO_PLAYBACK_CONFIG:

          /* Find the right feature unit for audio playback stream */
          audio_play_feature_unit_id = audio_playback->ux_host_class_audio_feature_unit_id;

          /* Read the static volume values for speaker channel 1 and 2 */
          audio_speaker_control1.ux_host_class_audio_control_entity = audio_play_feature_unit_id;
          audio_speaker_control1.ux_host_class_audio_control_size = 2;
          audio_speaker_control1.ux_host_class_audio_control = UX_HOST_CLASS_AUDIO_VOLUME_CONTROL;
          audio_speaker_control1.ux_host_class_audio_control_channel = 1;

          audio_speaker_control2.ux_host_class_audio_control_entity = audio_play_feature_unit_id;
          audio_speaker_control2.ux_host_class_audio_control_size = 2;
          audio_speaker_control2.ux_host_class_audio_control = UX_HOST_CLASS_AUDIO_VOLUME_CONTROL;
          audio_speaker_control2.ux_host_class_audio_control_channel = 2;

          ux_host_class_audio_entity_control_get(audio_playback, &audio_speaker_control1);
          ux_host_class_audio_entity_control_get(audio_playback, &audio_speaker_control2);

          /* Set the current volume values for speaker channel 1 and 2 */
          min = (SHORT)audio_speaker_control1.ux_host_class_audio_control_min;
          max = (SHORT)audio_speaker_control1.ux_host_class_audio_control_max;
          audio_speaker_control1.ux_host_class_audio_control_cur = (ULONG)((min + max) / 2);
          ux_host_class_audio_entity_control_value_set(audio_playback, &audio_speaker_control1);
          ux_host_class_audio_entity_control_value_get(audio_playback, &audio_speaker_control1);

          min = (SHORT)audio_speaker_control2.ux_host_class_audio_control_min;
          max = (SHORT)audio_speaker_control2.ux_host_class_audio_control_max;
          audio_speaker_control2.ux_host_class_audio_control_cur = (ULONG)((min + max) / 2);
          ux_host_class_audio_entity_control_value_set(audio_playback, &audio_speaker_control2);
          ux_host_class_audio_entity_control_value_get(audio_playback, &audio_speaker_control2);

          audio_speaker_mute.ux_host_class_audio_control_entity = audio_play_feature_unit_id;
          audio_speaker_mute.ux_host_class_audio_control_size = 1;
          audio_speaker_mute.ux_host_class_audio_control = UX_HOST_CLASS_AUDIO_MUTE_CONTROL;
          audio_speaker_mute.ux_host_class_audio_control_channel = 0;
          audio_speaker_mute.ux_host_class_audio_control_cur = 0;
          ux_host_class_audio_entity_control_value_set(audio_playback, &audio_speaker_mute);

          /* Update audio demo state machine */
          audio_playback_state = AUDIO_PLAYBACK_START;
          audio_playback_prev_state = AUDIO_PLAYBACK_CONFIG;

          break;

        case AUDIO_PLAYBACK_START:

          /* Parse the WAV header from the embedded Flash array */
          WAV_Flash_GetInfo(&WavPlayInfo);

          /* Print wav info */
          USBH_UsrLog("Playing embedded WAV from Flash");
          USBH_UsrLog("Sample rate : %lu Hz", WavPlayInfo.SampleRate);
          USBH_UsrLog("Channels    : %d", WavPlayInfo.NbrChannels);
          duration = WavPlayInfo.SubChunk2Size / WavPlayInfo.ByteRate;
          USBH_UsrLog("File size   : %lu bytes [%02lu:%02lu]",
                      WavPlayInfo.FileSize, duration / 60U, duration % 60U);

          /* Get audio frame length */
          if (ux_host_class_audio_speed_get(audio_playback) == UX_HIGH_SPEED_DEVICE)
          {
            audio_frame_length = ((WavPlayInfo.SampleRate * WavPlayInfo.NbrChannels * (WavPlayInfo.BitPerSample / 8U)) / 8000U);
          }
          else
          {
            audio_frame_length = ((WavPlayInfo.SampleRate * WavPlayInfo.NbrChannels * (WavPlayInfo.BitPerSample / 8U)) / 1000U);
          }

          /* Get audio buffer size */
          audio_buffer_size = audio_frame_length * 8U;

          /* Allocate local buffer */
          local_frame_buffer1 = ux_utility_memory_allocate(UX_SAFE_ALIGN, UX_CACHE_SAFE_MEMORY, audio_buffer_size);
          local_frame_buffer2 = ux_utility_memory_allocate(UX_SAFE_ALIGN, UX_CACHE_SAFE_MEMORY, audio_buffer_size);

          /* Reset local buffer memory */
          ux_utility_memory_set(local_frame_buffer1, 0, audio_buffer_size);
          ux_utility_memory_set(local_frame_buffer2, 0, audio_buffer_size);

          audio_data_offset = 0U;

          /* Check chunk size (not too small) */
          if (WavPlayInfo.SubChunk2Size < (audio_buffer_size * 3U))
          {
            USBH_UsrLog("ERROR: WAV payload too small (%lu bytes, need >= %lu).",
                        WavPlayInfo.SubChunk2Size, audio_buffer_size * 3U);
            USBH_UsrLog("Replace audio_data.c with a real WAV file");

            /* Release already-allocated buffers before bailing out */
            ux_utility_memory_free(local_frame_buffer1);
            ux_utility_memory_free(local_frame_buffer2);
            audio_frame_length = 0U;
            audio_buffer_size  = 0U;

            audio_playback_state = AUDIO_PLAYBACK_WAIT;
            break;
          }

          audio_data_offset = WAV_DATA_OFFSET;

          /* Update audio demo state machine */
          audio_playback_state = AUDIO_PLAYBACK_PLAY;
          audio_playback_prev_state = AUDIO_PLAYBACK_START;

          break;

        case AUDIO_PLAYBACK_PLAY:

          /* Fill audio sampling struct */
          audio_play_sampling.ux_host_class_audio_sampling_channels = WavPlayInfo.NbrChannels ;
          audio_play_sampling.ux_host_class_audio_sampling_frequency = WavPlayInfo.SampleRate;
          audio_play_sampling.ux_host_class_audio_sampling_resolution = WavPlayInfo.BitPerSample;


          /* Set the alternate setting interface of a stereo USB speaker */
          if (ux_host_class_audio_streaming_sampling_set(audio_playback, &audio_play_sampling) != UX_SUCCESS)
          {
            USBH_UsrLog("\nThe frequency or resolution of the WAV is not supported by this device.\n");
            USBH_UsrLog("Check audio_data.c: SampleRate and BitPerSample must match the speaker.\n");

            /* Update audio demo state machine */
            audio_playback_state = AUDIO_PLAYBACK_WAIT;
            audio_playback_prev_state = AUDIO_PLAYBACK_PLAY;

            break;
          }

          /* Prepare the 2 audio transfer_requests */
          transfer_request1.ux_host_class_audio_transfer_request_completion_function = playback_transfer_request_completion;
          transfer_request2.ux_host_class_audio_transfer_request_completion_function = playback_transfer_request_completion;
          transfer_request1.ux_host_class_audio_transfer_request_class_instance = audio_playback;
          transfer_request2.ux_host_class_audio_transfer_request_class_instance = audio_playback;
          transfer_request1.ux_host_class_audio_transfer_request_next_audio_transfer_request = &transfer_request2;
          transfer_request2.ux_host_class_audio_transfer_request_next_audio_transfer_request = UX_NULL;
          /* Create and link semaphore for host audio transfer request 1 */
          if (tx_semaphore_create(&host_audio_transfer_request1_semaphore, "host audio transfer request1 semaphore", 0) == TX_SUCCESS)
          {
            transfer_request1.ux_host_class_audio_transfer_request.ux_transfer_request_semaphore = host_audio_transfer_request1_semaphore;
          }
          /* Create and link semaphore for host audio transfer request 2 */
          if (tx_semaphore_create(&host_audio_transfer_request2_semaphore, "host audio transfer request2 semaphore", 0) == TX_SUCCESS)
          {
            transfer_request2.ux_host_class_audio_transfer_request.ux_transfer_request_semaphore = host_audio_transfer_request2_semaphore;
          }
          /* Set audio packet size */
          transfer_request1.ux_host_class_audio_transfer_request_packet_size = ux_host_class_audio_packet_size_get(audio_playback);
          transfer_request2.ux_host_class_audio_transfer_request_packet_size = ux_host_class_audio_packet_size_get(audio_playback);

          /* Read first block from Flash */
          audio_requested_length = WAV_Flash_Read(local_frame_buffer1, audio_buffer_size, &audio_data_offset);

          /* Set the buffer and length in the audio transfer_request */
          transfer_request1.ux_host_class_audio_transfer_request_data_pointer = local_frame_buffer1;
          transfer_request1.ux_host_class_audio_transfer_request_requested_length = audio_requested_length;

          /* Read second block from Flash */
          audio_requested_length = WAV_Flash_Read(local_frame_buffer2, audio_buffer_size, &audio_data_offset);

          /* Set the buffer and length in the audio transfer_request */
          transfer_request2.ux_host_class_audio_transfer_request_data_pointer = local_frame_buffer2;
          transfer_request2.ux_host_class_audio_transfer_request_requested_length = audio_requested_length;

          /* Send the first 4 transfer_requests */
          status = ux_host_class_audio_write(audio_playback, &transfer_request1);
          status = ux_host_class_audio_write(audio_playback, &transfer_request2);

          /* Update audio demo state machine */
          audio_playback_prev_state = AUDIO_PLAYBACK_PLAY;

          /* First sent should be request 1 */
          double_buffer_index = 0;

          do
          {
            /* Wait for the transfer to be completed */
            status = tx_semaphore_get(&data_send_semaphore, TX_WAIT_FOREVER);

            /* Check device removal */
            if (audio_playback == UX_NULL)
            {
              /* Flush pending semaphore count */
              while (tx_semaphore_get(&data_send_semaphore, TX_NO_WAIT) == TX_SUCCESS) { }

              ux_utility_memory_free(local_frame_buffer1);
              ux_utility_memory_free(local_frame_buffer2);
              audio_frame_length = 0U;
              audio_buffer_size  = 0U;
              break;
            }

            if (double_buffer_index == 0)
            {
              actual_frame_buffer = local_frame_buffer1;
              actual_transfer_request = &transfer_request1;
              double_buffer_index = 1;
              previous_transfer_request = &transfer_request2;
            }
            else
            {
              actual_frame_buffer = local_frame_buffer2;
              actual_transfer_request = &transfer_request2;
              double_buffer_index = 0;
              previous_transfer_request = &transfer_request1;
            }

            /* Read next block from Flash */
            audio_requested_length = WAV_Flash_Read(actual_frame_buffer, audio_buffer_size, &audio_data_offset);

            if (audio_requested_length != audio_buffer_size)
            {

              /* Stop playback at end of data */
              ux_host_class_audio_stop(audio_playback);

              /* Free allocated file */
              ux_utility_memory_free(local_frame_buffer1);
              ux_utility_memory_free(local_frame_buffer2);

              ux_utility_memory_set(&audio_play_sampling,0, sizeof(UX_HOST_CLASS_AUDIO_SAMPLING));
              ux_utility_memory_set(&transfer_request1,0, sizeof(UX_HOST_CLASS_AUDIO_TRANSFER_REQUEST));
              ux_utility_memory_set(&transfer_request2,0, sizeof(UX_HOST_CLASS_AUDIO_TRANSFER_REQUEST));

              audio_frame_length = 0U;
              audio_buffer_size  = 0U;

              USBH_UsrLog("\nPlayback complete.");

              /* Update audio demo state machine */
              audio_playback_state      = AUDIO_PLAYBACK_WAIT;
              audio_playback_prev_state = AUDIO_PLAYBACK_PLAY;

              break;
            }

            /* Prepare next transfer request */
            previous_transfer_request->ux_host_class_audio_transfer_request_next_audio_transfer_request = actual_transfer_request;

            /* The next transfer request is null */
            actual_transfer_request->ux_host_class_audio_transfer_request_next_audio_transfer_request = UX_NULL;

            /* Send the Audio transfer request */
            status = ux_host_class_audio_write(audio_playback, actual_transfer_request);

          }while (status == UX_SUCCESS);

          /* Flush pending completion events */
          while (tx_semaphore_get(&data_send_semaphore, TX_NO_WAIT) == TX_SUCCESS) { }

          /* Release buffers on write error */
          if (audio_buffer_size != 0U)
          {
            if (audio_playback != UX_NULL)
            {
              ux_host_class_audio_stop(audio_playback);
            }
            ux_utility_memory_free(local_frame_buffer1);
            ux_utility_memory_free(local_frame_buffer2);
            audio_frame_length = 0U;
            audio_buffer_size  = 0U;
            audio_playback_state = AUDIO_PLAYBACK_WAIT;
          }

          break;

        case AUDIO_PLAYBACK_WAIT:

          /* Wait for JOY_SEL press */
          joystick_state = BSP_JOY_GetState(JOY1);

          if ((joystick_state & JOY_SEL) != 0U)
          {
            if (joystick_pressed == 0U)
            {
              joystick_pressed = 1U;

              /* The first or third press starts playback and recording together. */
              if (audio_playback != UX_NULL)
              {
                 audio_playback_state = AUDIO_PLAYBACK_CONFIG;
                 audio_playback_prev_state = AUDIO_PLAYBACK_WAIT;
                 USBH_UsrLog("\nStarting playback.\n");
              }
            }
          }
          else
          {
            joystick_pressed = 0U;
          }

          /* Sleep thread for 20 ms */
          tx_thread_sleep(MS_TO_TICK(20));

          break;

        default:

          /* Update audio demo state machine */
          audio_playback_state = AUDIO_PLAYBACK_WAIT;
          break;
      }
    }
    else
    {
      /* Sleep thread for 10 ms */
      tx_thread_sleep(MS_TO_TICK(10));
    }
  }
}

/**
  * @brief  playback_transfer_request_completion
  *         This function receives a completion call back on a isoch transfer request.
  * @param  transfer_request: Pointer to transfer request
  * @retval none
  */
VOID playback_transfer_request_completion(UX_HOST_CLASS_AUDIO_TRANSFER_REQUEST *transfer_request)
{
  /* Release the semaphore */
  tx_semaphore_put(&data_send_semaphore);
}

/* USER CODE END 1 */
