/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_device_cdc_acm.c
  * @author  MCD Application Team
  * @brief   USBX Device CDC ACM applicative source file
  ******************************************************************************
    * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "ux_device_cdc_acm.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "app_ble.h"
#include "log_module.h"
#include "adv_trace_usbx_if.h"
#include "app_freertos.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
static UINT write_state = UX_STATE_RESET;

#define APP_RX_DATA_SIZE   HCI_DATA_MAX_SIZE

/* Rx/TX flag */
#define RX_NEW_RECEIVED_DATA      0x01
#define TX_NEW_TRANSMITTED_DATA   0x02

/* Data length for vcp */
#define VCP_WORDLENGTH8  8
#define VCP_WORDLENGTH9  9

/* the minimum baudrate */
#define MIN_BAUDRATE     9600

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

UX_SLAVE_CLASS_CDC_ACM  *cdc_acm;

UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_PARAMETER CDC_VCP_LineCoding =
{
  115200, /* baud rate */
  0x00,   /* stop bits-1 */
  0x00,   /* parity - none */
  0x08    /* nb. of bits 8 */
};

/* USB buffers */
char *wr_buf = 0;
uint16_t wr_len = 0;

char rd_buf[512];
uint16_t rd_len;

extern osThreadId_t USBXTaskHandle;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  USBD_CDC_ACM_Activate
  *         This function is called when insertion of a CDC ACM device.
  * @param  cdc_acm_instance: Pointer to the cdc acm class instance.
  * @retval none
  */
VOID USBD_CDC_ACM_Activate(VOID *cdc_acm_instance)
{
  /* USER CODE BEGIN USBD_CDC_ACM_Activate */
  LOG_INFO_APP("[USB] USB CDC ACM Activation\n");

  /* Save the CDC instance */
  cdc_acm = (UX_SLAVE_CLASS_CDC_ACM*) cdc_acm_instance;

  /* Set device class_cdc_acm with default parameters */
  if (ux_device_class_cdc_acm_ioctl(cdc_acm, UX_SLAVE_CLASS_CDC_ACM_IOCTL_SET_LINE_CODING,
                                    &CDC_VCP_LineCoding) != UX_SUCCESS)
  {
    Error_Handler();
  }

  /* USER CODE END USBD_CDC_ACM_Activate */

  return;
}

/**
  * @brief  USBD_CDC_ACM_Deactivate
  *         This function is called when extraction of a CDC ACM device.
  * @param  cdc_acm_instance: Pointer to the cdc acm class instance.
  * @retval none
  */
VOID USBD_CDC_ACM_Deactivate(VOID *cdc_acm_instance)
{
  /* USER CODE BEGIN USBD_CDC_ACM_Deactivate */
  LOG_INFO_APP("[USB] USB CDC ACM Deactivation\n");
  UX_PARAMETER_NOT_USED(cdc_acm_instance);

  /* Reset the cdc acm instance */
  cdc_acm = UX_NULL;

  /* USER CODE END USBD_CDC_ACM_Deactivate */

  return;
}

/**
  * @brief  USBD_CDC_ACM_ParameterChange
  *         This function is invoked to manage the CDC ACM class requests.
  * @param  cdc_acm_instance: Pointer to the cdc acm class instance.
  * @retval none
  */
VOID USBD_CDC_ACM_ParameterChange(VOID *cdc_acm_instance)
{
  /* USER CODE BEGIN USBD_CDC_ACM_ParameterChange */
  LOG_INFO_APP("[USB] USB CDC ACM Parameter Change\n");

  /* USER CODE END USBD_CDC_ACM_ParameterChange */

  return;
}

/* USER CODE BEGIN 1 */
/**
  * @brief Process data received from USB and send it the the Transparent Mode app
  * @retval none
  */
VOID USBD_CDC_ACM_Read_Task(VOID)
{
  UX_SLAVE_DEVICE *device;
  UINT  status;
  ULONG read_length;
  static ULONG actual_length = 0;

  /* Get device */
  device = &_ux_system_slave->ux_system_slave_device;

  /* Check if device is configured */
  if (device->ux_slave_device_state != UX_DEVICE_CONFIGURED)
  {
    return;
  }

  /* Read data from USB */
  read_length = (_ux_system_slave->ux_system_slave_speed == UX_HIGH_SPEED_DEVICE) ? 512 : 64;
  status = ux_device_class_cdc_acm_read_run(cdc_acm,
                                            (UCHAR *)rd_buf, read_length,
                                            &actual_length);

  /* Error.  */
  if (status <= UX_STATE_ERROR)
  {
    return;
  }
  else if (status == UX_STATE_NEXT)
  {
    uint8_t i;

    /* Read is complete */
    rd_len = actual_length;

    /* Send packet byte per byte to match UART behavior */
    for (i = 0; i < rd_len; i++)
    {
      USBXIf_RxCpltCallback(&rd_buf[i], 1);
    }
  }
}

/**
  * @brief Process data received from Transparent Mode app and send it through USB
  * @retval none
  */
VOID USBD_CDC_ACM_Write_Task(VOID)
{
  UX_SLAVE_DEVICE    *device;
  ULONG actual_length;
  UINT ux_status = UX_SUCCESS;

  /* Get device */
  device = &_ux_system_slave->ux_system_slave_device;

  /* Check if device is configured */
  if (device->ux_slave_device_state != UX_DEVICE_CONFIGURED)
  {
    return;
  }

  switch(write_state)
  {
    case UX_STATE_RESET:
    {
      if (wr_buf != 0)
      {
        /* Send data over the class cdc_acm_write */
        ux_status = ux_device_class_cdc_acm_write_run(cdc_acm,
                                                      (UCHAR *)wr_buf,
                                                      wr_len, &actual_length);
        if (ux_status != UX_STATE_WAIT)
        {
          return;
        }
        write_state = UX_STATE_WAIT;
        /* Continue to run USBX Task */
        osThreadFlagsSet(USBXTaskHandle, 0x01);
        return;
      }
      return;
    }

    case UX_STATE_WAIT:
      /* Continue to run state machine.  */
      ux_status = ux_device_class_cdc_acm_write_run(cdc_acm, UX_NULL, 0, &actual_length);
      /* Check if there is  fatal error.  */
      if (ux_status < UX_STATE_IDLE)
      {
        return;
      }
      /* Check if dataset is transmitted */
      if (ux_status <= UX_STATE_NEXT)
      {
        /* Buffer has been fully sent */
        wr_buf = 0;
        USBXIf_TxCpltCallback();

        /* Reset state */
        write_state = UX_STATE_RESET;
      }
      /* Continue to run USBX Task */
      osThreadFlagsSet(USBXTaskHandle, 0x01);
      return;
    default:
      return;
  }
}

/**
  * @brief Set string to be sent through USB
  * @param str: string to send
  * @param str_len: length of the string to send
  * @retval none
  */
VOID USBD_CDC_ACM_WriteString(char* str, uint16_t str_len)
{
  wr_buf = str;
  wr_len = str_len;
}
/* USER CODE END 1 */
