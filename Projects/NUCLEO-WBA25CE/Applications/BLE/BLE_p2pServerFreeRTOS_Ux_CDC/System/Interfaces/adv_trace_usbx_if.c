/**
  ******************************************************************************
  * @file    adv_trace_usbx_if.c
  * @author  MCD Application Team
  * @brief : Source file for interfacing the stm32_adv_trace to USBX stack
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "app_conf.h"
#include "stm32_adv_trace.h"
#include "adv_trace_usbx_if.h"
#include "ux_device_cdc_acm.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* External variables --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* list all the driver interface used by the trace application. */
const UTIL_ADV_TRACE_Driver_s UTIL_TraceDriver =
{
  USBX_If_Init,
  USBX_If_Deinit,
  USBX_If_StartRx,
  USBX_If_Transmit
};

/* Private variables ---------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static void (*TxCpltCallback)       ( void * );
static void (*RxCpltCallback)       ( uint8_t * pData, uint16_t iSize, uint8_t cError );

/* Private user code ---------------------------------------------------------*/

/**
 *
 */
UTIL_ADV_TRACE_Status_t USBX_If_Init(  void (*pCallbackFunction)(void *))
{
  TxCpltCallback = pCallbackFunction;

  return UTIL_ADV_TRACE_OK;
}

/**
 *
 */
UTIL_ADV_TRACE_Status_t USBX_If_Deinit( void )
{
  return UTIL_ADV_TRACE_OK;
}

/**
 *
 */
UTIL_ADV_TRACE_Status_t USBX_If_StartRx( void (*pCallbackFunction)(uint8_t * pData, uint16_t iSize, uint8_t cError ) )
{
  /* Configure USBX stack in Receive mode */
  //HAL_UART_Receive_IT( &LOG_UART_HANDLER, &cCharRx, 1 );
  //LOG_UART_HANDLER.RxCpltCallback = &USBXIf_RxCpltCallback;

  if ( pCallbackFunction != NULL )
  {
    RxCpltCallback = pCallbackFunction;
  }

  return UTIL_ADV_TRACE_OK;
}

/**
 *
 */
UTIL_ADV_TRACE_Status_t USBX_If_Transmit ( uint8_t * pData, uint16_t iSize )
{
  UTIL_ADV_TRACE_Status_t eStatus = UTIL_ADV_TRACE_OK;

  USBD_CDC_ACM_WriteString((char*) pData, iSize);

  return eStatus;
}

/**
 *
 */
void USBXIf_TxCpltCallback(void)
{
  /* ADV Trace callback */
  TxCpltCallback(NULL);
}

/**
 *
 */
void USBXIf_RxCpltCallback(char *str, uint16_t str_len)
{
  RxCpltCallback((uint8_t *)str, str_len, 0);
}

