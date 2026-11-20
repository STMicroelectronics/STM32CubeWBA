/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_usbx_host.c
  * @author  MCD Application Team
  * @brief   USBX host applicative file
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
#include "app_usbx_host.h"

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
static TX_THREAD ux_host_app_thread;

/* USER CODE BEGIN PV */
TX_THREAD ux_audio_playback_thread;
TX_SEMAPHORE data_send_semaphore;
UX_HOST_CLASS_AUDIO *audio_playback;

extern audio_playback_stateTypeDef audio_playback_state;
extern audio_playback_stateTypeDef audio_playback_prev_state;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static VOID app_ux_host_thread_entry(ULONG thread_input);
static UINT ux_host_event_callback(ULONG event, UX_HOST_CLASS *current_class, VOID *current_instance);
static VOID ux_host_error_callback(UINT system_level, UINT system_context, UINT error_code);

extern HCD_HandleTypeDef hhcd_USB_OTG_HS;

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/**
  * @brief  Application USBX Host Initialization.
  * @param  memory_ptr: memory pointer
  * @retval status
  */
UINT MX_USBX_Host_Init(VOID *memory_ptr)
{
  UINT ret = UX_SUCCESS;

  UCHAR *pointer;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

  /* USER CODE BEGIN MX_USBX_Host_Init0 */
  /* USER CODE END MX_USBX_Host_Init0 */

  /* USER CODE BEGIN MX_USBX_Host_Init 1 */

  /* USER CODE END MX_USBX_Host_Init 1 */

  /* Allocate the stack for host application main thread */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, UX_HOST_APP_THREAD_STACK_SIZE,
                       TX_NO_WAIT) != TX_SUCCESS)
  {
    /* USER CODE BEGIN MAIN_THREAD_ALLOCATE_STACK_ERROR */
    return TX_POOL_ERROR;
    /* USER CODE END MAIN_THREAD_ALLOCATE_STACK_ERROR */
  }

  /* Create the host application main thread */
  if (tx_thread_create(&ux_host_app_thread, UX_HOST_APP_THREAD_NAME, app_ux_host_thread_entry,
                       0, pointer, UX_HOST_APP_THREAD_STACK_SIZE, UX_HOST_APP_THREAD_PRIO,
                       UX_HOST_APP_THREAD_PREEMPTION_THRESHOLD, UX_HOST_APP_THREAD_TIME_SLICE,
                       UX_HOST_APP_THREAD_START_OPTION) != TX_SUCCESS)
  {
    /* USER CODE BEGIN MAIN_THREAD_CREATE_ERROR */
    return TX_THREAD_ERROR;
    /* USER CODE END MAIN_THREAD_CREATE_ERROR */
  }

  /* USER CODE BEGIN MX_USBX_Host_Init 2 */

  /* Allocate the stack for audio playback thread */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, UX_HOST_APP_THREAD_STACK_SIZE * 2, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the audio playback thread */
  if (tx_thread_create(&ux_audio_playback_thread, "USBX Audio Playback Thread", audio_playback_thread_entry,
                       0, pointer, UX_HOST_APP_THREAD_STACK_SIZE * 2, 30, 30, 1, TX_AUTO_START) != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  /* Create the semaphore to synch up */
  if (tx_semaphore_create(&data_send_semaphore, "data_send_semaphore", 0) != TX_SUCCESS)
  {
    return TX_SEMAPHORE_ERROR;
  }

  /* USER CODE END MX_USBX_Host_Init 2 */

  return ret;
}

/**
  * @brief  Function implementing app_ux_host_thread_entry.
  * @param  thread_input: User thread input parameter.
  * @retval none
  */
static VOID app_ux_host_thread_entry(ULONG thread_input)
{
  /* USER CODE BEGIN app_ux_host_thread_entry 0*/
  /* Start Application Message */
  USBH_UsrLog("**** USB OTG HS Audio Host **** \n");
  USBH_UsrLog("USB Host Audio library started.\n");

  /* Wait for Device to be attached */
  USBH_UsrLog("Starting Audio Application");
  USBH_UsrLog("Connect an Audio Speaker Device");

  /* Initialization of USB host */
  MX_USB_OTG_HS_HCD_Init();

  /* USER CODE END app_ux_host_thread_entry 0*/

  /* Initialize the Stack Host USB*/
  if (MX_USBX_Host_Stack_Init() != UX_SUCCESS)
  {
    /* USER CODE BEGIN MAIN_INITIALIZE_HOST_STACK_ERROR */
    Error_Handler();
    /* USER CODE END MAIN_INITIALIZE_HOST_STACK_ERROR */
  }
  /* USER CODE BEGIN app_ux_host_thread_entry 1*/
  /* Start USB Host */
  HAL_HCD_Start(&hhcd_USB_OTG_HS);
  /* USER CODE END app_ux_host_thread_entry 1*/
}

/**
  * @brief  ux_host_event_callback
  *         This callback is invoked to notify application of instance changes.
  * @param  event: event code.
  * @param  current_class: Pointer to class.
  * @param  current_instance: Pointer to class instance.
  * @retval status
  */
UINT ux_host_event_callback(ULONG event, UX_HOST_CLASS *current_class, VOID *current_instance)
{
  UINT status = UX_SUCCESS;

  /* USER CODE BEGIN ux_host_event_callback0 */

  /* USER CODE END ux_host_event_callback0 */

  switch (event)
  {
    case UX_DEVICE_INSERTION:

      /* USER CODE BEGIN UX_DEVICE_INSERTION */

      /* Get current audio class */
      if (current_class->ux_host_class_entry_function == ux_host_class_audio_entry)
      {
        if ((audio_playback == UX_NULL) &&
            (ux_host_class_audio_subclass_get((UX_HOST_CLASS_AUDIO *) current_instance) == UX_HOST_CLASS_AUDIO_SUBCLASS_STREAMING))
        {

          if (ux_host_class_audio_type_get((UX_HOST_CLASS_AUDIO *) current_instance) == UX_HOST_CLASS_AUDIO_OUTPUT)
          {
            /* Get current audio speaker instance */
            audio_playback = (UX_HOST_CLASS_AUDIO *) current_instance;

            USBH_UsrLog("\nAUDIO Speaker Device Found");
          }
        }
      }

      /* USER CODE END UX_DEVICE_INSERTION */

      break;

    case UX_DEVICE_REMOVAL:

      /* USER CODE BEGIN UX_DEVICE_REMOVAL */

      if ((VOID*)audio_playback == current_instance)
      {
        /* Clear audio playback instance */
        audio_playback = UX_NULL;

        /* Flush pending semaphore events */
        while (tx_semaphore_get(&data_send_semaphore, TX_NO_WAIT) == TX_SUCCESS) { }

        /* Wake up playback thread if streaming is active */
        if (audio_playback_state == AUDIO_PLAYBACK_PLAY)
        {
          tx_semaphore_put(&data_send_semaphore);
        }

        /* Reset state so the next connection starts cleanly from IDLE */
        audio_playback_state = AUDIO_PLAYBACK_IDLE;

        USBH_UsrLog("\nUSB Audio Speaker Device Removal");
      }

      /* USER CODE END UX_DEVICE_REMOVAL */

      break;

    case UX_DEVICE_CONNECTION:

      /* USER CODE BEGIN UX_DEVICE_CONNECTION */

      if (audio_playback != UX_NULL)
      {
        USBH_UsrLog("\nPID: %#x ", (UINT)_ux_system_host->ux_system_host_device_array->ux_device_descriptor.idProduct);
        USBH_UsrLog("VID: %#x ", (UINT)_ux_system_host->ux_system_host_device_array->ux_device_descriptor.idVendor);
        USBH_UsrLog("USB Audio Host App...");

        /* Check USB Audio class */
        if (ux_host_class_audio_protocol_get(audio_playback) == UX_HOST_CLASS_AUDIO_PROTOCOL_IP_VERSION_01_00)
        {
          USBH_UsrLog("Audio 1.0 Speaker Device is ready...\n");
        }
        else
        {
          USBH_UsrLog("Audio 2.0 Speaker Device is ready...\n");
        }

        audio_playback_state = AUDIO_PLAYBACK_IDLE;
      }

      /* USER CODE END UX_DEVICE_CONNECTION */

      break;

    case UX_DEVICE_DISCONNECTION:

      /* USER CODE BEGIN UX_DEVICE_DISCONNECTION */

      /* USER CODE END UX_DEVICE_DISCONNECTION */

      break;

    default:

      /* USER CODE BEGIN EVENT_DEFAULT */

      /* USER CODE END EVENT_DEFAULT */

      break;
  }

  /* USER CODE BEGIN ux_host_event_callback1 */

  /* USER CODE END ux_host_event_callback1 */

  return status;
}

/**
  * @brief  ux_host_error_callback
  *         This callback is invoked to notify application of error changes.
  * @param  system_level: system level parameter.
  * @param  system_context: system context code.
  * @param  error_code: error event code.
  * @retval None
  */
VOID ux_host_error_callback(UINT system_level, UINT system_context, UINT error_code)
{
  /* USER CODE BEGIN ux_host_error_callback0 */

  /* USER CODE END ux_host_error_callback0 */

  switch (error_code)
  {
    case UX_DEVICE_ENUMERATION_FAILURE:

      /* USER CODE BEGIN UX_DEVICE_ENUMERATION_FAILURE */
      USBH_UsrLog("USB Device Enumeration Failure");
      /* USER CODE END UX_DEVICE_ENUMERATION_FAILURE */

      break;

    case  UX_NO_DEVICE_CONNECTED:

      /* USER CODE BEGIN UX_NO_DEVICE_CONNECTED */
      USBH_UsrLog("USB Device disconnected");
      /* USER CODE END UX_NO_DEVICE_CONNECTED */

      break;

    default:

      /* USER CODE BEGIN ERROR_DEFAULT */

      /* USER CODE END ERROR_DEFAULT */

      break;
  }

  /* USER CODE BEGIN ux_host_error_callback1 */

  /* USER CODE END ux_host_error_callback1 */
}

/**
  * @brief  MX_USBX_Host_Stack_Init
  *         Initialization of the USB Host stack.
  *         Initialize the host stack, register the host class stack.
  * @param  None
  * @retval ret
  */
UINT MX_USBX_Host_Stack_Init(void)
{
  UINT ret = UX_SUCCESS;
  /* USER CODE BEGIN MX_USBX_Host_Stack_Init_PreTreatment_0 */
  /* USER CODE END MX_USBX_Host_Stack_Init_PreTreatment_0 */

  /* The code below is required for installing the host portion of USBX.  */
  if (ux_host_stack_initialize(ux_host_event_callback) != UX_SUCCESS)
  {
    return UX_ERROR;
  }

  /* Register a callback error function */
  ux_utility_error_callback_register(&ux_host_error_callback);

  /* Initialize the host audio class */
  if (ux_host_stack_class_register(_ux_system_host_class_audio_name,
                                   ux_host_class_audio_entry) != UX_SUCCESS)
  {
    /* USER CODE BEGIN USBX_HOST_AUDIO_REGISTER_ERROR */
    return UX_ERROR;
    /* USER CODE END USBX_HOST_AUDIO_REGISTER_ERROR */
  }

  /* Register all the USB host controllers available in this system. */
  if (ux_host_stack_hcd_register(_ux_system_host_hcd_stm32_name,
                                 _ux_hcd_stm32_initialize, USB_OTG_HS_BASE,
                                 (ULONG)&hhcd_USB_OTG_HS)!= UX_SUCCESS)
  {
    /* USER CODE BEGIN USBX_HOST_STACK_HCD_REGISTER_ERROR */
    return UX_ERROR;
    /* USER CODE END USBX_HOST_STACK_HCD_REGISTER_ERROR */
  }
  /* USER CODE BEGIN MX_USBX_Host_Stack_Init_PreTreatment_1 */
  /* USER CODE END MX_USBX_Host_Stack_Init_PreTreatment_1 */

  /* USER CODE BEGIN MX_USBX_Host_Stack_Init_PostTreatment */
  /* USER CODE END MX_USBX_Host_Stack_Init_PostTreatment */
  return ret ;
}

/**
  * @brief  MX_USBX_Host_Stack_DeInit
  *         Uninitialization of the USB Host stack.
  *         Uninitialize the host stack, unregister the host class stack and
  *         unregister the USB host controllers.
  * @param  None
  * @retval ret
  */
UINT MX_USBX_Host_Stack_DeInit(void)
{
  UINT ret = UX_SUCCESS;

  /* USER CODE BEGIN MX_USBX_Host_Stack_DeInit_PreTreatment_0 */

  /* USER CODE END MX_USBX_Host_Stack_DeInit_PreTreatment_0 */

  /* Unregister all the USB host controllers available in this system. */
  if (ux_host_stack_hcd_unregister(_ux_system_host_hcd_stm32_name,
                                   USB_OTG_HS_BASE,
                                   (ULONG)&hhcd_USB_OTG_HS)!= UX_SUCCESS)
  {
    return UX_ERROR;
  }

  /* The code below is required for uninstalling the host portion of USBX.  */
  if (ux_host_stack_uninitialize() != UX_SUCCESS)
  {
    return UX_ERROR;
  }

  /* USER CODE BEGIN MX_USBX_Host_Stack_DeInit_PreTreatment_1 */
  /* USER CODE END MX_USBX_Host_Stack_DeInit_PreTreatment_1 */

  /* USER CODE BEGIN MX_USBX_Host_Stack_DeInit_PostTreatment */
  /* USER CODE END MX_USBX_Host_Stack_DeInit_PostTreatment */
  return ret ;
}
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
