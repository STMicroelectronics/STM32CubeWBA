/**
  ******************************************************************************
  * @file    adv_trace_usart_if.h
  * @author  MCD Application Team
  * @brief : Header file for stm32_adv_trace interface file
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef USART_IF_H
#define USART_IF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32_adv_trace.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* External variables --------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
/**
* @brief  Init the UART and associated DMA.
* @param  cb tx function callback.
* @return @ref UTIL_ADV_TRACE_Status_t
*/
UTIL_ADV_TRACE_Status_t UART_Init(void (*cb)(void *));

/**
* @brief  DeInit the UART and associated DMA.
* @return @ref UTIL_ADV_TRACE_Status_t
*/
UTIL_ADV_TRACE_Status_t UART_DeInit(void);

/**
* @brief  send buffer to UART using DMA
* @param  pdata data to be sent
* @param  size of buffer p_data to be sent
* @return @ref UTIL_ADV_TRACE_Status_t
*/
UTIL_ADV_TRACE_Status_t UART_TransmitDMA(uint8_t *pdata, uint16_t size);

/**
* @brief  start Rx process
* @param  cb callback to receive the data
* @return @ref UTIL_ADV_TRACE_Status_t
*/
UTIL_ADV_TRACE_Status_t UART_StartRx(void (*cb)(uint8_t *pdata, uint16_t size, uint8_t error));


#ifdef __cplusplus
}
#endif

#endif /* USART_IF_H */
