/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart_if.c
  * @author  MCD Application Team
  * @brief : Source file for interfacing the stm32_adv_trace to hardware
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "main.h"
#include "stm32_adv_trace.h"
#include "usart_if.h"
#include "app_conf.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/

/* Check if we need UsartIf */
#if (IF_USART_USE == 1)

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/

/**
 *  @brief  trace tracer definition.
 *
 *  list all the driver interface used by the trace application.
 */
const UTIL_ADV_TRACE_Driver_s UTIL_TraceDriver =
{
  UART_Init,
  UART_DeInit,
  UART_StartRx,
  UART_TransmitDMA
};

/* USER CODE BEGIN EC */


/* USER CODE END EC */

/* Private variables ---------------------------------------------------------*/
static Ifhuart_s sIfhuart;

uint8_t receive_after_transmit = 0; /* Whether the UART should be in RX after a Transmit */

/**
  * @brief buffer to receive 1 character
  */
uint8_t charRx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  TX complete callback
  * @return none
  */
static void (*TxCpltCallback)(void *);
static void (*RxCpltCallback)(uint8_t *pdata, uint16_t size, uint8_t error);
static void UsartIf_TxCpltCallback(UART_HandleTypeDef *huart);
static void UsartIf_RxCpltCallback(UART_HandleTypeDef *huart);

/* USER CODE END Private_Function_Prototypes */
/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN Private_Typedef */

/* USER CODE END Private_Typedef */
/* Private define ------------------------------------------------------------*/


/* USER CODE END Private_Define */
/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Private_Macro */

/* USER CODE END Private_Macro */
/* Private variables ---------------------------------------------------------*/
/**
  * @brief buffer to receive 1 character
  */
uint8_t charRx;
/* USER CODE BEGIN Private_Variables */

/* USER CODE END Private_Variables */

void UartIf_Init(Ifhuart_s sUartIf)
{
  sIfhuart.IfhuartTx = sUartIf.IfhuartTx;
  sIfhuart.IfhuartRx = sUartIf.IfhuartRx;
}

UTIL_ADV_TRACE_Status_t UART_Init(  void (*cb)(void *))
{
  /*
   * It is assumed that UART1 MSP Init is done by
   * CubeMX layer. As such, no need to call
   * MX_USART1_UART_Init & HAL_UART_MspInit.
   */

  /* Two layer callbacks: perhaps smarter way to do it? */
  TxCpltCallback = cb;
  sIfhuart.IfhuartTx->TxCpltCallback = UsartIf_TxCpltCallback;

  return UTIL_ADV_TRACE_OK;
}

UTIL_ADV_TRACE_Status_t UART_TransmitDMA ( uint8_t *pdata, uint16_t size )
{
  /* USER CODE BEGIN UART_TransmitDMA 1 */

  /* USER CODE END UART_TransmitDMA 1 */

  UTIL_ADV_TRACE_Status_t status = UTIL_ADV_TRACE_OK;

  /* USER CODE BEGIN UART_TransmitDMA 2 */

  /* USER CODE END UART_TransmitDMA 2 */

  HAL_StatusTypeDef result = HAL_UART_Transmit_DMA(sIfhuart.IfhuartTx, pdata, size);

  if (result != HAL_OK)
    status = UTIL_ADV_TRACE_HW_ERROR;

  /* Check whether the UART should return in Receiver mode */
  if(receive_after_transmit)
  {
    HAL_UART_Receive_IT(sIfhuart.IfhuartTx, &charRx, 1);
  }

  /* USER CODE BEGIN UART_TransmitDMA 3 */

  /* USER CODE END UART_TransmitDMA 3 */

  return status;

  /* USER CODE BEGIN UART_TransmitDMA 4 */

  /* USER CODE END UART_TransmitDMA 4 */
}

UTIL_ADV_TRACE_Status_t UART_DeInit( void )
{
  HAL_StatusTypeDef result;

  result = HAL_UART_DeInit(sIfhuart.IfhuartTx);
  if (result != HAL_OK)
  {
    sIfhuart.IfhuartTx->TxCpltCallback = NULL;
    return UTIL_ADV_TRACE_UNKNOWN_ERROR;
  }

  result = HAL_DMA_DeInit(sIfhuart.IfhuartTx->hdmatx);
  if (result != HAL_OK)
  {
    return UTIL_ADV_TRACE_UNKNOWN_ERROR;
  }
  
  result = HAL_UART_DeInit(sIfhuart.IfhuartRx);
  if (result != HAL_OK)
  {
    return UTIL_ADV_TRACE_UNKNOWN_ERROR;
  }

  return UTIL_ADV_TRACE_OK;

}

UTIL_ADV_TRACE_Status_t UART_StartRx(void (*cb)(uint8_t *pdata, uint16_t size, uint8_t error))
{
  /* USER CODE BEGIN UART_StartRx 1 */

  /* USER CODE END UART_StartRx 1 */

  /* Configure UART1 in Receive mode */
  HAL_UART_Receive_IT(sIfhuart.IfhuartRx, &charRx, 1);
  sIfhuart.IfhuartRx->RxCpltCallback = &UsartIf_RxCpltCallback;

  if (cb != NULL)
  {
    RxCpltCallback = cb;
  }

  /* USER CODE BEGIN UART_StartRx 2 */

  /* USER CODE END UART_StartRx 2 */

  return UTIL_ADV_TRACE_OK;

  /* USER CODE BEGIN UART_StartRx 3 */

  /* USER CODE END UART_StartRx 3 */
}


static void UsartIf_TxCpltCallback(UART_HandleTypeDef *huart)
{
  /* ADV Trace callback */
  TxCpltCallback(NULL);
}

static void UsartIf_RxCpltCallback(UART_HandleTypeDef *huart)
{
   RxCpltCallback(&charRx, 1, 0);
   HAL_UART_Receive_IT(sIfhuart.IfhuartRx, &charRx, 1);
}

/* USER CODE END Private_Functions */

#else /* (IF_USART_USE != 1), initialize with empty function to avoid error */


/**
 *  @brief  trace tracer definition.
 *
 *  list all the driver interface used by the trace application.
 */
const UTIL_ADV_TRACE_Driver_s UTIL_TraceDriver =
{
  UART_Init,
  UART_DeInit,
  UART_StartRx,
  UART_TransmitDMA
};


void UartIf_Init(Ifhuart_s sUartIf)
{
  (void)sUartIf;
  return;
}

UTIL_ADV_TRACE_Status_t UART_Init(  void (*cb)(void *))
{
  (void)cb;
  return UTIL_ADV_TRACE_INVALID_PARAM;
}

UTIL_ADV_TRACE_Status_t UART_TransmitDMA ( uint8_t *pdata, uint16_t size )
{
  (void)pdata;
  (void)size;
  return UTIL_ADV_TRACE_INVALID_PARAM;
}

UTIL_ADV_TRACE_Status_t UART_DeInit( void )
{
  return UTIL_ADV_TRACE_INVALID_PARAM;
}


UTIL_ADV_TRACE_Status_t UART_StartRx(void (*cb)(uint8_t *pdata, uint16_t size, uint8_t error))
{
  (void)cb;
  return UTIL_ADV_TRACE_INVALID_PARAM;
}

#endif /* (IF_USART_USE == 1) */
