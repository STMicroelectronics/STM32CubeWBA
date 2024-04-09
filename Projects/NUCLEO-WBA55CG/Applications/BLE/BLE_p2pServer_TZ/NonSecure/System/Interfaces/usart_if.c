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
#define IRQ_BADIRQ       ((IRQn_Type)(-666))
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/**
  * @brief USART1 handle
  */
extern UART_HandleTypeDef huart1;

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

static void USART1_DMA_MspDeInit(void);

static void UsartIf_TxCpltCallback(UART_HandleTypeDef *huart);
static void UsartIf_RxCpltCallback(UART_HandleTypeDef *huart);
static IRQn_Type get_IRQn_Type_from_DMA_HandleTypeDef(DMA_HandleTypeDef * dma_handler);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

UTIL_ADV_TRACE_Status_t UART_Init(  void (*cb)(void *))
{
  /* USER CODE BEGIN UART_Init 1 */

  /* USER CODE END UART_Init 1 */

  MX_USART1_UART_Init();

  /* USER CODE BEGIN UART_Init 2 */

  /* USER CODE END UART_Init 2 */

  /* Two layer callbacks: perhaps smarter way to do it? */
  TxCpltCallback = cb;

  huart1.TxCpltCallback = UsartIf_TxCpltCallback;

  /* USER CODE BEGIN UART_Init 3 */

  /* USER CODE END UART_Init 3 */

  /* USER CODE BEGIN UART_Init 4 */

  /* USER CODE END UART_Init 4 */

  return UTIL_ADV_TRACE_OK;
}

UTIL_ADV_TRACE_Status_t UART_DeInit( void )
{
  IRQn_Type use_dma;
  HAL_StatusTypeDef result;

  /* USER CODE BEGIN UART_DeInit 1 */

  /* USER CODE END UART_DeInit 1 */

  USART1_DMA_MspDeInit();

  /* USER CODE BEGIN UART_DeInit 2 */

  /* USER CODE END UART_DeInit 2 */

  result = HAL_UART_DeInit(&huart1);
  if (result != HAL_OK)
  {
    huart1.TxCpltCallback = NULL;
    return UTIL_ADV_TRACE_UNKNOWN_ERROR;
  }

  /* USER CODE BEGIN UART_DeInit 3 */

  /* USER CODE END UART_DeInit 3 */

  use_dma = get_IRQn_Type_from_DMA_HandleTypeDef(huart1.hdmatx);

  if (use_dma == GPDMA1_Channel0_IRQn || use_dma == GPDMA1_Channel1_IRQn
      || use_dma == GPDMA1_Channel2_IRQn || use_dma == GPDMA1_Channel3_IRQn
      || use_dma == GPDMA1_Channel4_IRQn || use_dma == GPDMA1_Channel5_IRQn
      || use_dma == GPDMA1_Channel6_IRQn || use_dma == GPDMA1_Channel7_IRQn)
  {
    result = HAL_DMA_DeInit(huart1.hdmatx);
    if (result != HAL_OK)
    {
      return UTIL_ADV_TRACE_UNKNOWN_ERROR;
    }
  }

  /* USER CODE BEGIN UART_DeInit 4 */

  /* USER CODE END UART_DeInit 4 */

  use_dma = get_IRQn_Type_from_DMA_HandleTypeDef(huart1.hdmarx);

  if (use_dma == GPDMA1_Channel0_IRQn || use_dma == GPDMA1_Channel1_IRQn
      || use_dma == GPDMA1_Channel2_IRQn || use_dma == GPDMA1_Channel3_IRQn
      || use_dma == GPDMA1_Channel4_IRQn || use_dma == GPDMA1_Channel5_IRQn
      || use_dma == GPDMA1_Channel6_IRQn || use_dma == GPDMA1_Channel7_IRQn)
  {
    result = HAL_DMA_DeInit(huart1.hdmarx);
    if (result != HAL_OK)
    {
      return UTIL_ADV_TRACE_UNKNOWN_ERROR;
    }
  }

  /* USER CODE BEGIN UART_DeInit 5 */

  /* USER CODE END UART_DeInit 5 */

  /* USER CODE BEGIN UART_DeInit 6 */

  /* USER CODE END UART_DeInit 6 */

  return UTIL_ADV_TRACE_OK;
}

UTIL_ADV_TRACE_Status_t UART_StartRx(void (*cb)(uint8_t *pdata, uint16_t size, uint8_t error))
{
  /* USER CODE BEGIN UART_StartRx 1 */

  /* USER CODE END UART_StartRx 1 */

  /* Configure UART1 in Receive mode */
  HAL_UART_Receive_IT(&huart1, &charRx, 1);
  huart1.RxCpltCallback = &UsartIf_RxCpltCallback;

  if (cb != NULL)
  {
    RxCpltCallback = cb;
  }

  /* USER CODE BEGIN UART_StartRx 2 */

  /* USER CODE END UART_StartRx 2 */

  /* USER CODE BEGIN UART_StartRx 3 */

  /* USER CODE END UART_StartRx 3 */

  return UTIL_ADV_TRACE_OK;
}

UTIL_ADV_TRACE_Status_t UART_TransmitDMA ( uint8_t *pdata, uint16_t size )
{
  HAL_StatusTypeDef result;
  IRQn_Type use_dma_tx;
  UTIL_ADV_TRACE_Status_t status = UTIL_ADV_TRACE_OK;

  /* USER CODE BEGIN UART_TransmitDMA 1 */

  /* USER CODE END UART_TransmitDMA 1 */

  /* USER CODE BEGIN UART_TransmitDMA 2 */

  /* USER CODE END UART_TransmitDMA 2 */

  use_dma_tx = get_IRQn_Type_from_DMA_HandleTypeDef(huart1.hdmatx);

  if ( (use_dma_tx == GPDMA1_Channel0_IRQn ) || ( use_dma_tx == GPDMA1_Channel1_IRQn )
      || ( use_dma_tx == GPDMA1_Channel2_IRQn ) || ( use_dma_tx == GPDMA1_Channel3_IRQn )
      || ( use_dma_tx == GPDMA1_Channel4_IRQn ) || ( use_dma_tx == GPDMA1_Channel5_IRQn )
      || ( use_dma_tx == GPDMA1_Channel6_IRQn ) || ( use_dma_tx == GPDMA1_Channel7_IRQn ) )
  {
    result = HAL_UART_Transmit_DMA(&huart1, pdata, size);
  }
  else
  {
    result = HAL_UART_Transmit_IT(&huart1, pdata, size);
  }

  if (result != HAL_OK)
  {
    status = UTIL_ADV_TRACE_HW_ERROR;
  }

  /* Check whether the UART should return in Receiver mode */
  if(receive_after_transmit)
  {
    HAL_UART_Receive_IT(&huart1, &charRx, 1);
  }

  /* USER CODE BEGIN UART_TransmitDMA 3 */

  /* USER CODE END UART_TransmitDMA 3 */

  /* USER CODE BEGIN UART_TransmitDMA 4 */

  /* USER CODE END UART_TransmitDMA 4 */

  return status;
}

static void USART1_DMA_MspDeInit(void)
{
  IRQn_Type use_dma_tx;

  /* USER CODE BEGIN USART1_DMA_MspDeInit 1 */

  /* USER CODE END USART1_DMA_MspDeInit 1 */

  /* Disable USART1 clock */
  __HAL_RCC_USART1_CLK_DISABLE();

  /* Disable interrupts for USART1 */
  HAL_NVIC_DisableIRQ(USART1_IRQn);

  /* GPDMA1 controller clock disable */
  __HAL_RCC_GPDMA1_CLK_DISABLE();

  /* DMA interrupt init */
  use_dma_tx = get_IRQn_Type_from_DMA_HandleTypeDef(huart1.hdmatx);

  if ( use_dma_tx != IRQ_BADIRQ )
  {
    HAL_NVIC_DisableIRQ(use_dma_tx);
  }

  /* USER CODE BEGIN USART1_DMA_MspDeInit 2 */

  /* USER CODE END USART1_DMA_MspDeInit 2 */
}

static void UsartIf_TxCpltCallback(UART_HandleTypeDef *huart)
{
  /* USER CODE BEGIN UsartIf_TxCpltCallback 1 */

  /* USER CODE END UsartIf_TxCpltCallback 1 */

  /* ADV Trace callback */
  TxCpltCallback(NULL);

  /* USER CODE BEGIN UsartIf_TxCpltCallback 2 */

  /* USER CODE END UsartIf_TxCpltCallback 2 */
}

static void UsartIf_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* USER CODE BEGIN UsartIf_RxCpltCallback 1 */

  /* USER CODE END UsartIf_RxCpltCallback 1 */

  RxCpltCallback(&charRx, 1, 0);
  HAL_UART_Receive_IT(&huart1, &charRx, 1);

  /* USER CODE BEGIN UsartIf_RxCpltCallback 2 */

  /* USER CODE END UsartIf_RxCpltCallback 2 */
}

/**
  * The purpose of this function is to match a DMA_HandleTypeDef as key with the corresponding IRQn_Type as value.
  *
  * TAKE CARE : in case of an invalid parameter or e.g. an usart/lpuart not initialized, this will lead to hard fault.
  *             it is up to the user to ensure the serial link is in a valid state.
  */
static IRQn_Type get_IRQn_Type_from_DMA_HandleTypeDef(DMA_HandleTypeDef * dma_handler)
{
  if (dma_handler->Instance == GPDMA1_Channel0) { return GPDMA1_Channel0_IRQn; }
  if (dma_handler->Instance == GPDMA1_Channel1) { return GPDMA1_Channel1_IRQn; }
  if (dma_handler->Instance == GPDMA1_Channel2) { return GPDMA1_Channel2_IRQn; }
  if (dma_handler->Instance == GPDMA1_Channel3) { return GPDMA1_Channel3_IRQn; }
  if (dma_handler->Instance == GPDMA1_Channel4) { return GPDMA1_Channel4_IRQn; }
  if (dma_handler->Instance == GPDMA1_Channel5) { return GPDMA1_Channel5_IRQn; }
  if (dma_handler->Instance == GPDMA1_Channel6) { return GPDMA1_Channel6_IRQn; }
  if (dma_handler->Instance == GPDMA1_Channel7) { return GPDMA1_Channel7_IRQn; }

  /* Values from (-1) to (-15) are already in used. This value isn't used so it should be safe.
     So, if you see this value, it means you used an invalid DMA handler as input. */
  return IRQ_BADIRQ;
}

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
