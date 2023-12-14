/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart_if.c
  * @author  MCD Application Team
  * @brief : Source file for interfacing the stm32_adv_trace to hardware
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32_adv_trace.h"
#include "usart_if.h"

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
/**
  * @brief USART1 handle
  */
extern UART_HandleTypeDef huart2;

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
static void USART2_DMA_MspDeInit(void);
static void UsartIf_TxCpltCallback(UART_HandleTypeDef *huart);
static void UsartIf_RxCpltCallback(UART_HandleTypeDef *huart);

/* USER CODE END Private_Function_Prototypes */
/* Private typedef -----------------------------------------------------------*/
#if (CFG_HW_USART2_ENABLED == 1)
extern UART_HandleTypeDef huart2;
#if (CFG_HW_USART2_DMA_TX_SUPPORTED == 1)
extern DMA_HandleTypeDef hdma_usart2_tx;
#endif /*(CFG_HW_USART2_DMA_TX_SUPPORTED == 1)*/
#endif /* (CFG_HW_USART2_ENABLED == 1) */

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

UTIL_ADV_TRACE_Status_t UART_Init(  void (*cb)(void *))
{
  /*
   * It is assumed that UART1 MSP Init is done by
   * CubeMX layer. As such, no need to call
   * MX_USART1_UART_Init & HAL_UART_MspInit.
   */

  /* Two layer callbacks: perhaps smarter way to do it? */
  TxCpltCallback = cb;
  huart2.TxCpltCallback = UsartIf_TxCpltCallback;

  return UTIL_ADV_TRACE_OK;
}

UTIL_ADV_TRACE_Status_t UART_TransmitDMA ( uint8_t *pdata, uint16_t size )
{
  /* USER CODE BEGIN UART_TransmitDMA 1 */

  /* USER CODE END UART_TransmitDMA 1 */

  UTIL_ADV_TRACE_Status_t status = UTIL_ADV_TRACE_OK;

  /* USER CODE BEGIN UART_TransmitDMA 2 */

  /* USER CODE END UART_TransmitDMA 2 */

  HAL_StatusTypeDef result = HAL_UART_Transmit_DMA(&huart2, pdata, size);

  if (result != HAL_OK)
    status = UTIL_ADV_TRACE_HW_ERROR;

  /* Check whether the UART should return in Receiver mode */
  if(receive_after_transmit)
  {
    HAL_UART_Receive_IT(&huart2, &charRx, 1);
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

  USART2_DMA_MspDeInit();

  result = HAL_UART_DeInit(&huart2);
  if (result != HAL_OK)
  {
    huart2.TxCpltCallback = NULL;
    return UTIL_ADV_TRACE_UNKNOWN_ERROR;
  }

  result = HAL_DMA_DeInit(huart2.hdmatx);
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
  HAL_UART_Receive_IT(&huart2, &charRx, 1);
  huart2.RxCpltCallback = &UsartIf_RxCpltCallback;

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

/* USER CODE BEGIN Private_Functions */
static void USART2_DMA_MspDeInit(void)
{
  /* USER CODE BEGIN USART2_DMA_MspDeInit 1 */

  /* USER CODE END USART2_DMA_MspDeInit 1 */

  /* Disable USART2 clock */
  __HAL_RCC_USART2_CLK_DISABLE();

  /* Disable interrupts for USART2 */
  HAL_NVIC_DisableIRQ(USART2_IRQn);

  /* GPDMA1 controller clock disable */
  //__HAL_RCC_GPDMA1_CLK_DISABLE();

  /* DMA interrupt init */
  HAL_NVIC_DisableIRQ(GPDMA1_Channel1_IRQn);

  /* USER CODE BEGIN USART1_DMA_MspDeInit 2 */

  /* USER CODE END USART1_DMA_MspDeInit 2 */
}

static void UsartIf_TxCpltCallback(UART_HandleTypeDef *huart)
{
  /* ADV Trace callback */
  TxCpltCallback(NULL);
}

static void UsartIf_RxCpltCallback(UART_HandleTypeDef *huart)
{
   RxCpltCallback(&charRx, 1, 0);
   HAL_UART_Receive_IT(&huart2, &charRx, 1);
}

/* USER CODE END Private_Functions */