/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_usbx_device.c
  * @author  MCD Application Team
  * @brief   USBX Device applicative file
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
#include "app_usbx_device.h"
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

static ULONG audio_interface_number;
static ULONG audio_configuration_number;
static UX_DEVICE_CLASS_AUDIO_PARAMETER audio_parameter;
static UX_DEVICE_CLASS_AUDIO_STREAM_PARAMETER audio_stream_parameter[USBD_AUDIO_STREAM_NUMBER];
static uint8_t audio_stream_index = 0U;
static TX_THREAD ux_device_app_thread;
extern PCD_HandleTypeDef hpcd_USB_OTG_HS;

/* USER CODE BEGIN PV */

static TX_THREAD ux_audio_record_thread;
TX_SEMAPHORE     semaphore_half_transfer;
TX_SEMAPHORE     semaphore_full_transfer;
BSP_AUDIO_Init_t AudioInInit;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static VOID app_ux_device_thread_entry(ULONG thread_input);
static UINT USBD_ChangeFunction(ULONG Device_State);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/**
  * @brief  Application USBX Device Initialization.
  * @param  memory_ptr: memory pointer
  * @retval status
  */

UINT MX_USBX_Device_Init(VOID *memory_ptr)
{
  UINT ret = UX_SUCCESS;
  UCHAR *pointer;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;
  /* USER CODE BEGIN MX_USBX_Device_Init 0 */
  /* USER CODE END MX_USBX_Device_Init 0 */

  /* USER CODE BEGIN MX_USBX_Device_Init 1 */
  /* USER CODE END MX_USBX_Device_Init 1 */

  /* Allocate the stack for device application main thread */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, UX_DEVICE_APP_THREAD_STACK_SIZE,
                       TX_NO_WAIT) != TX_SUCCESS)
  {
    /* USER CODE BEGIN MAIN_THREAD_ALLOCATE_STACK_ERROR */
    return TX_POOL_ERROR;
    /* USER CODE END MAIN_THREAD_ALLOCATE_STACK_ERROR */
  }

  /* Create the device application main thread */
  if (tx_thread_create(&ux_device_app_thread, UX_DEVICE_APP_THREAD_NAME, app_ux_device_thread_entry,
                       0, pointer, UX_DEVICE_APP_THREAD_STACK_SIZE, UX_DEVICE_APP_THREAD_PRIO,
                       UX_DEVICE_APP_THREAD_PREEMPTION_THRESHOLD, UX_DEVICE_APP_THREAD_TIME_SLICE,
                       UX_DEVICE_APP_THREAD_START_OPTION) != TX_SUCCESS)
  {
    /* USER CODE BEGIN MAIN_THREAD_CREATE_ERROR */
    return TX_THREAD_ERROR;
    /* USER CODE END MAIN_THREAD_CREATE_ERROR */
  }

  /* USER CODE BEGIN MX_USBX_Device_Init 2 */

  /* Allocate the stack for audio record thread.  */
  if(tx_byte_allocate(byte_pool, (VOID **) &pointer, UX_DEVICE_APP_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    Error_Handler();
  }

  /* Create the audio record thread.  */
  if(tx_thread_create(&ux_audio_record_thread, "usbx_audio_app_thread_entry",
                      usbx_audio_record_app_thread, 0, pointer, 1024,
                      20, 20, 1, TX_AUTO_START) != TX_SUCCESS)
  {
    Error_Handler();
  }

  /* Create the semaphore_half_transfer */
  if (tx_semaphore_create(&semaphore_half_transfer, "Half Transfer Semaphore", 0) != TX_SUCCESS)
  {
    Error_Handler();
  }

  /* Create the semaphore_full_transfer */
  if (tx_semaphore_create(&semaphore_full_transfer, "Full Transfer Semaphore", 0) != TX_SUCCESS)
  {
    Error_Handler();
  }

  /* USER CODE END MX_USBX_Device_Init 2 */

  return ret;
}

/**
  * @brief  MX_USBX_Device_Stack_Init
  *         Initialization of the USB Device.
  *         Initialize the device stack, register the device class stack
  *         register the USB device controller.
  * @param  None
  * @retval ret
  */
UINT MX_USBX_Device_Stack_Init(void)
{
  UINT ret = UX_SUCCESS;
  UCHAR *device_framework_high_speed;
  UCHAR *device_framework_full_speed;
  ULONG device_framework_hs_length;
  ULONG device_framework_fs_length;
  ULONG string_framework_length;
  ULONG language_id_framework_length;
  UCHAR *string_framework;
  UCHAR *language_id_framework;

  /* USER CODE BEGIN MX_USBX_Device_Stack_Init 0 */

  /* USER CODE END MX_USBX_Device_Stack_Init 0 */
  /* Get Device Framework High Speed and get the length */
  device_framework_high_speed = USBD_Get_Device_Framework_Speed(USBD_HIGH_SPEED,
                                                                &device_framework_hs_length);

  /* Get Device Framework Full Speed and get the length */
  device_framework_full_speed = USBD_Get_Device_Framework_Speed(USBD_FULL_SPEED,
                                                                &device_framework_fs_length);

  /* Get String Framework and get the length */
  string_framework = USBD_Get_String_Framework(&string_framework_length);

  /* Get Language Id Framework and get the length */
  language_id_framework = USBD_Get_Language_Id_Framework(&language_id_framework_length);

  /* Install the device portion of USBX */
  if (ux_device_stack_initialize(device_framework_high_speed,
                                 device_framework_hs_length,
                                 device_framework_full_speed,
                                 device_framework_fs_length,
                                 string_framework,
                                 string_framework_length,
                                 language_id_framework,
                                 language_id_framework_length,
                                 USBD_ChangeFunction) != UX_SUCCESS)
  {
    /* USER CODE BEGIN USBX_DEVICE_INITIALIZE_ERROR */
    return UX_ERROR;
    /* USER CODE END USBX_DEVICE_INITIALIZE_ERROR */
  }

  /* Initialize audio playback control values */
  USBD_AUDIO_SetControlValues();

  /* Initialize the audio class parameters for audio recording */
  audio_stream_parameter[audio_stream_index].ux_device_class_audio_stream_parameter_callbacks.ux_device_class_audio_stream_change
    = USBD_AUDIO_RecordingStreamChange;

  audio_stream_parameter[audio_stream_index].ux_device_class_audio_stream_parameter_callbacks.ux_device_class_audio_stream_frame_done
    = USBD_AUDIO_RecordingStreamFrameDone;

  audio_stream_parameter[audio_stream_index].ux_device_class_audio_stream_parameter_max_frame_buffer_nb
    = USBD_AUDIO_RecordingStreamGetMaxFrameBufferNumber();

  audio_stream_parameter[audio_stream_index].ux_device_class_audio_stream_parameter_max_frame_buffer_size
    = USBD_AUDIO_RecordingStreamGetMaxFrameBufferSize();

  audio_stream_parameter[audio_stream_index].ux_device_class_audio_stream_parameter_thread_entry
    = ux_device_class_audio_write_thread_entry;

  /* Set the parameters for audio device */
  audio_parameter.ux_device_class_audio_parameter_streams_nb  = USBD_AUDIO_STREAM_NUMBER;
  audio_parameter.ux_device_class_audio_parameter_streams     = audio_stream_parameter;

  audio_parameter.ux_device_class_audio_parameter_callbacks.ux_slave_class_audio_instance_activate
    = USBD_AUDIO_Activate;

  audio_parameter.ux_device_class_audio_parameter_callbacks.ux_slave_class_audio_instance_deactivate
    = USBD_AUDIO_Deactivate;

 audio_parameter.ux_device_class_audio_parameter_callbacks.ux_device_class_audio_control_process
    = USBD_AUDIO_ControlProcess;

  /* Get audio configuration number */
  audio_configuration_number = USBD_Get_Configuration_Number(CLASS_TYPE_AUDIO_20, 0);

  /* Find audio interface number */
  audio_interface_number = USBD_Get_Interface_Number(CLASS_TYPE_AUDIO_20, 0);
  /* Initialize the device audio class */
  if (ux_device_stack_class_register(_ux_system_slave_class_audio_name,
                                     ux_device_class_audio_entry,
                                     audio_configuration_number,
                                     audio_interface_number,
                                     &audio_parameter) != UX_SUCCESS)
  {
    /* USER CODE BEGIN USBX_DEVICE_AUDIO_REGISTER_ERROR */
    return UX_ERROR;
    /* USER CODE END USBX_DEVICE_AUDIO_REGISTER_ERROR */
  }

  /* Initialize and link controller HAL driver */
  ux_dcd_stm32_initialize((ULONG)USB_OTG_HS, (ULONG)&hpcd_USB_OTG_HS);
  /* USER CODE BEGIN MX_USBX_Device_Stack_Init_PostTreatment */

  /* USER CODE END MX_USBX_Device_Stack_Init_PostTreatment */

  /* USER CODE BEGIN MX_USBX_Device_Stack_Init 1 */

  /* USER CODE END MX_USBX_Device_Stack_Init 1 */

  return ret;
}

/**
  * @brief  Function implementing app_ux_device_thread_entry.
  * @param  thread_input: User thread input parameter.
  * @retval none
  */
static VOID app_ux_device_thread_entry(ULONG thread_input)
{
  /* USER CODE BEGIN app_ux_device_thread_entry 0*/
  MX_USB_OTG_HS_PCD_Init();

  AudioInInit.Device = AUDIO_IN_DEVICE_DIGITAL_MIC;
  AudioInInit.SampleRate = AUDIO_FREQUENCY_48K;
  AudioInInit.BitsPerSample = AUDIO_RESOLUTION_16B;
  AudioInInit.ChannelsNbr = 1U;
  AudioInInit.Volume = 80U;

  if (BSP_AUDIO_IN_Init(0U, &AudioInInit) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* USER CODE END app_ux_device_thread_entry 0*/

  /* Initialize the Stack USB Device */
  if (MX_USBX_Device_Stack_Init() != UX_SUCCESS)
  {
    /* USER CODE BEGIN MAIN_INITIALIZE_DEVICE_STACK_ERROR */
    Error_Handler();
    /* USER CODE END MAIN_INITIALIZE_DEVICE_STACK_ERROR */
  }
  /* USER CODE BEGIN app_ux_device_thread_entry 1*/
  /* Start the USB device */
  HAL_PCD_Start(&hpcd_USB_OTG_HS);
  /* USER CODE END app_ux_device_thread_entry 1*/
}

/**
  * @brief  MX_USBX_Device_Stack_DeInit
  *         Uninitialization of the USB Device.
  *         Uninitialize the device stack, unregister the device class stack
  *         unregister the USB device controller.
  * @param  None
  * @retval ret
  */
UINT MX_USBX_Device_Stack_DeInit(void)
{
  UINT ret = UX_SUCCESS;

  /* USER CODE BEGIN MX_USBX_Device_Stack_DeInit_PreTreatment_0 */
  /* USER CODE END MX_USBX_Device_Stack_DeInit_PreTreatment_0 */

  /* Unregister USB device controller. */
  if (ux_dcd_stm32_uninitialize((ULONG)USB_OTG_HS, (ULONG)&hpcd_USB_OTG_HS) != UX_SUCCESS)
  {
    return UX_ERROR;
  }

  /* Unregister audio class. */
  if (ux_device_stack_class_unregister(_ux_system_slave_class_audio_name,
                                     ux_device_class_audio_entry) != UX_SUCCESS)
  {
    return UX_ERROR;
  }

  /* The code below is required for uninstalling the device portion of USBX.  */
  if (ux_device_stack_uninitialize() != UX_SUCCESS)
  {
    return UX_ERROR;
  }

  /* USER CODE BEGIN MX_USBX_Device_Stack_DeInit_PreTreatment_1 */
  /* USER CODE END MX_USBX_Device_Stack_DeInit_PreTreatment_1 */

  /* USER CODE BEGIN MX_USBX_Device_Stack_DeInit_PostTreatment */
  /* USER CODE END MX_USBX_Device_Stack_DeInit_PostTreatment */

  return ret;
}

/**
  * @brief  USBD_ChangeFunction
  *         This function is called when the device state changes.
  * @param  Device_State: USB Device State
  * @retval status
  */
static UINT USBD_ChangeFunction(ULONG Device_State)
{
   UINT status = UX_SUCCESS;

  /* USER CODE BEGIN USBD_ChangeFunction0 */

  /* USER CODE END USBD_ChangeFunction0 */

  switch (Device_State)
  {
    case UX_DEVICE_ATTACHED:

      /* USER CODE BEGIN UX_DEVICE_ATTACHED */

      /* USER CODE END UX_DEVICE_ATTACHED */

      break;

    case UX_DEVICE_REMOVED:

      /* USER CODE BEGIN UX_DEVICE_REMOVED */

      /* USER CODE END UX_DEVICE_REMOVED */

      break;

    case UX_DCD_STM32_DEVICE_CONNECTED:

      /* USER CODE BEGIN UX_DCD_STM32_DEVICE_CONNECTED */

      /* USER CODE END UX_DCD_STM32_DEVICE_CONNECTED */

      break;

    case UX_DCD_STM32_DEVICE_DISCONNECTED:

      /* USER CODE BEGIN UX_DCD_STM32_DEVICE_DISCONNECTED */

      /* USER CODE END UX_DCD_STM32_DEVICE_DISCONNECTED */

      break;

    case UX_DCD_STM32_DEVICE_SUSPENDED:

      /* USER CODE BEGIN UX_DCD_STM32_DEVICE_SUSPENDED */

      /* USER CODE END UX_DCD_STM32_DEVICE_SUSPENDED */

      break;

    case UX_DCD_STM32_DEVICE_RESUMED:

      /* USER CODE BEGIN UX_DCD_STM32_DEVICE_RESUMED */

      /* USER CODE END UX_DCD_STM32_DEVICE_RESUMED */

      break;

    case UX_DCD_STM32_SOF_RECEIVED:

      /* USER CODE BEGIN UX_DCD_STM32_SOF_RECEIVED */

      /* USER CODE END UX_DCD_STM32_SOF_RECEIVED */

      break;

    default:

      /* USER CODE BEGIN DEFAULT */

      /* USER CODE END DEFAULT */

      break;

  }

  /* USER CODE BEGIN USBD_ChangeFunction1 */

  /* USER CODE END USBD_ChangeFunction1 */

  return status;
}
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
