/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32wbaxx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
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
#include "main.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */
extern DMA_HandleTypeDef handle_LPDMA1_Channel0;

extern DMA_HandleTypeDef handle_LPDMA1_Channel1;

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{

  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_PWR_CLK_ENABLE();

  /* System interrupt init*/

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/**
  * @brief XSPI MSP Initialization
  * This function configures the hardware resources used in this example
  * @param hxspi: XSPI handle pointer
  * @retval None
  */
void HAL_XSPI_MspInit(XSPI_HandleTypeDef* hxspi)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hxspi->Instance==XSPI1)
  {
    /* USER CODE BEGIN XSPI1_MspInit 0 */

    /* USER CODE END XSPI1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_XSPI1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**XSPI1 GPIO Configuration
    PA2     ------> XSPI1_NCS
    PB6     ------> XSPI1_IO3
    PB5     ------> XSPI1_IO2
    PB4 (NJTRST)     ------> XSPI1_IO1
    PB3 (JTDO/TRACESWO)     ------> XSPI1_IO0
    PA15 (JTDI)     ------> XSPI1_CLK
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_XSPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_XSPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_XSPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF13_XSPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF13_XSPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* XSPI1 DMA Init */
    /* LPDMA1_REQUEST_XSPI1 Init */
    handle_LPDMA1_Channel0.Instance = LPDMA1_Channel0;
    handle_LPDMA1_Channel0.Init.Request = LPDMA1_REQUEST_XSPI1;
    handle_LPDMA1_Channel0.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
    handle_LPDMA1_Channel0.Init.Direction = DMA_MEMORY_TO_PERIPH;
    handle_LPDMA1_Channel0.Init.SrcInc = DMA_SINC_INCREMENTED;
    handle_LPDMA1_Channel0.Init.DestInc = DMA_DINC_FIXED;
    handle_LPDMA1_Channel0.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
    handle_LPDMA1_Channel0.Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
    handle_LPDMA1_Channel0.Init.Priority = DMA_LOW_PRIORITY_HIGH_WEIGHT;
    handle_LPDMA1_Channel0.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    handle_LPDMA1_Channel0.Init.Mode = DMA_NORMAL;
    if (HAL_DMA_Init(&handle_LPDMA1_Channel0) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hxspi, hdmatx, handle_LPDMA1_Channel0);

    if (HAL_DMA_ConfigChannelAttributes(&handle_LPDMA1_Channel0, DMA_CHANNEL_NPRIV) != HAL_OK)
    {
      Error_Handler();
    }

    /* LPDMA1_REQUEST_XSPI1 Init */
    handle_LPDMA1_Channel1.Instance = LPDMA1_Channel1;
    handle_LPDMA1_Channel1.Init.Request = LPDMA1_REQUEST_XSPI1;
    handle_LPDMA1_Channel1.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
    handle_LPDMA1_Channel1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    handle_LPDMA1_Channel1.Init.SrcInc = DMA_SINC_FIXED;
    handle_LPDMA1_Channel1.Init.DestInc = DMA_DINC_INCREMENTED;
    handle_LPDMA1_Channel1.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
    handle_LPDMA1_Channel1.Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
    handle_LPDMA1_Channel1.Init.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;
    handle_LPDMA1_Channel1.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    handle_LPDMA1_Channel1.Init.Mode = DMA_NORMAL;
    if (HAL_DMA_Init(&handle_LPDMA1_Channel1) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hxspi, hdmarx, handle_LPDMA1_Channel1);

    if (HAL_DMA_ConfigChannelAttributes(&handle_LPDMA1_Channel1, DMA_CHANNEL_NPRIV) != HAL_OK)
    {
      Error_Handler();
    }

    /* XSPI1 interrupt Init */
    HAL_NVIC_SetPriority(XSPI1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(XSPI1_IRQn);
    /* USER CODE BEGIN XSPI1_MspInit 1 */

    /* USER CODE END XSPI1_MspInit 1 */

  }

}

/**
  * @brief XSPI MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param hxspi: XSPI handle pointer
  * @retval None
  */
void HAL_XSPI_MspDeInit(XSPI_HandleTypeDef* hxspi)
{
  if(hxspi->Instance==XSPI1)
  {
    /* USER CODE BEGIN XSPI1_MspDeInit 0 */

    /* USER CODE END XSPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_XSPI1_CLK_DISABLE();

    /**XSPI1 GPIO Configuration
    PA2     ------> XSPI1_NCS
    PB6     ------> XSPI1_IO3
    PB5     ------> XSPI1_IO2
    PB4 (NJTRST)     ------> XSPI1_IO1
    PB3 (JTDO/TRACESWO)     ------> XSPI1_IO0
    PA15 (JTDI)     ------> XSPI1_CLK
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_15);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_3);

    /* XSPI1 DMA DeInit */
    HAL_DMA_DeInit(hxspi->hdmatx);
    HAL_DMA_DeInit(hxspi->hdmarx);

    /* XSPI1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(XSPI1_IRQn);
    /* USER CODE BEGIN XSPI1_MspDeInit 1 */

    /* USER CODE END XSPI1_MspDeInit 1 */
  }

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
