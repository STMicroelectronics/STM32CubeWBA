/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Examples_LL/SPI/SPI_TwoBoards_FullDuplex_DMA_Slave_Init/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to send/receive bytes over SPI IP using
  *          the STM32WBAxx SPI LL API.
  *          Peripheral initialization done using LL unitary services functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

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

/* USER CODE BEGIN PV */

__IO uint8_t ubButtonPress = 0;

/* Buffer used for transmission */
uint8_t aTxBuffer[] = "**** SPI_TwoBoards_FullDuplex_DMA communication **** SPI_TwoBoards_FullDuplex_DMA communication **** SPI_TwoBoards_FullDuplex_DMA communication ****";
uint8_t ubNbDataToTransmit = sizeof(aTxBuffer);
__IO uint8_t ubTransmissionComplete = 0;

/* Buffer used for reception */
uint8_t aRxBuffer[sizeof(aTxBuffer)];
uint8_t ubNbDataToReceive  = sizeof(aTxBuffer);
__IO uint8_t ubReceptionComplete = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_GPDMA1_Init(void);
static void MX_ICACHE_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

void     SystemClock_Config(void);
void     Activate_SPI(void);
void     LED_On(void);
void     LED_Blinking(uint32_t Period);
void     WaitAndCheckEndOfTransfer(void);
uint8_t  Buffercmp8(uint8_t *pBuffer1, uint8_t *pBuffer2, uint8_t BufferLength);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  /* System interrupt init*/
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* Enable PWR clock interface */

  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_PWR);

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_GPDMA1_Init();
  MX_ICACHE_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  /* Configure the DMA1_Channel2 functional parameters */
  LL_DMA_ConfigAddresses(GPDMA1, LL_DMA_CHANNEL_2, (uint32_t)aTxBuffer, (uint32_t)&(SPI1->TXDR));
  LL_DMA_SetBlkDataLength(GPDMA1, LL_DMA_CHANNEL_2, ubNbDataToReceive);
  LL_DMA_SetPeriphRequest(GPDMA1, LL_DMA_CHANNEL_2, LL_GPDMA1_REQUEST_SPI1_TX);


  /* Configure the DMA1_Channel1 functional parameters */
  LL_DMA_ConfigAddresses(GPDMA1, LL_DMA_CHANNEL_1, (uint32_t)&(SPI1->RXDR), (uint32_t)aRxBuffer);
  LL_DMA_SetBlkDataLength(GPDMA1, LL_DMA_CHANNEL_1, ubNbDataToTransmit);

  LL_DMA_SetPeriphRequest(GPDMA1, LL_DMA_CHANNEL_1, LL_GPDMA1_REQUEST_SPI1_RX);

  /* Enable DMA interrupts complete/error */
  LL_DMA_EnableIT_TC(GPDMA1, LL_DMA_CHANNEL_2);
  LL_DMA_EnableIT_DTE(GPDMA1, LL_DMA_CHANNEL_2);
  LL_DMA_EnableIT_TC(GPDMA1, LL_DMA_CHANNEL_1);
  LL_DMA_EnableIT_DTE(GPDMA1, LL_DMA_CHANNEL_1);

  /* Initialize FFIFO Threshold */
  LL_SPI_SetFIFOThreshold(SPI1, LL_SPI_FIFO_TH_01DATA);

  /* Configure SPI1 DMA transfer interrupts */
  /* Enable DMA TX Interrupt */
  LL_SPI_EnableDMAReq_TX(SPI1);

  /* Configure SPI1 DMA transfer interrupts */
  /* Enable DMA RX Interrupt */
  LL_SPI_EnableDMAReq_RX(SPI1);

  /* Enable the SPI1 peripheral */
  Activate_SPI();

  /* Wait for the end of the transfer and check received data */
  /* LED blinking FAST during waiting time */
  WaitAndCheckEndOfTransfer();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_3);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_3)
  {
  }

  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {
  }

  LL_RCC_PLL1_ConfigDomain_SYS(LL_RCC_PLL1SOURCE_HSE, 4, 25, 2);
  LL_RCC_PLL1_EnableDomain_PLL1R();
  LL_RCC_PLL1_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_8_16);
  LL_RCC_PLL1_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL1_IsReady() != 1)
  {
  }

  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1R);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1R)
  {
  }

  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAHB5Prescaler(LL_RCC_AHB5_DIV_4);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetAPB7Prescaler(LL_RCC_APB7_DIV_1);

  LL_Init1msTick(100000000);

  LL_SetSystemCoreClock(100000000);
}

/**
  * @brief GPDMA1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPDMA1_Init(void)
{

  /* USER CODE BEGIN GPDMA1_Init 0 */

  /* USER CODE END GPDMA1_Init 0 */

  /* Peripheral clock enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPDMA1);

  /* GPDMA1 interrupt Init */
  NVIC_SetPriority(GPDMA1_Channel1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(GPDMA1_Channel1_IRQn);
  NVIC_SetPriority(GPDMA1_Channel2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(GPDMA1_Channel2_IRQn);

  /* USER CODE BEGIN GPDMA1_Init 1 */

  /* USER CODE END GPDMA1_Init 1 */
  /* USER CODE BEGIN GPDMA1_Init 2 */

  /* USER CODE END GPDMA1_Init 2 */

}

/**
  * @brief ICACHE Initialization Function
  * @param None
  * @retval None
  */
static void MX_ICACHE_Init(void)
{

  /* USER CODE BEGIN ICACHE_Init 0 */

  /* USER CODE END ICACHE_Init 0 */

  /* USER CODE BEGIN ICACHE_Init 1 */

  /* USER CODE END ICACHE_Init 1 */

  /** Enable instruction cache in 1-way (direct mapped cache)
  */
  LL_ICACHE_SetMode(LL_ICACHE_1WAY);
  LL_ICACHE_Enable();
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  LL_DMA_InitTypeDef DMA_InitStruct = {0};

  LL_RCC_SetSPIClockSource(LL_RCC_SPI1_CLKSOURCE_PCLK2);

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
  /**SPI1 GPIO Configuration
  PB4 (NJTRST)   ------> SPI1_SCK
  PB3 (JTDO/TRACESWO)   ------> SPI1_MISO
  PA15 (JTDI)   ------> SPI1_MOSI
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_4;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_15;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* SPI1 DMA Init */

  /* GPDMA1_REQUEST_SPI1_TX Init */
  DMA_InitStruct.SrcAddress = 0x00000000U;
  DMA_InitStruct.DestAddress = 0x00000000U;
  DMA_InitStruct.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
  DMA_InitStruct.BlkHWRequest = LL_DMA_HWREQUEST_SINGLEBURST;
  DMA_InitStruct.DataAlignment = LL_DMA_DATA_ALIGN_ZEROPADD;
  DMA_InitStruct.SrcBurstLength = 1;
  DMA_InitStruct.DestBurstLength = 1;
  DMA_InitStruct.SrcDataWidth = LL_DMA_SRC_DATAWIDTH_BYTE;
  DMA_InitStruct.DestDataWidth = LL_DMA_DEST_DATAWIDTH_BYTE;
  DMA_InitStruct.SrcIncMode = LL_DMA_SRC_INCREMENT;
  DMA_InitStruct.DestIncMode = LL_DMA_DEST_FIXED;
  DMA_InitStruct.Priority = LL_DMA_LOW_PRIORITY_LOW_WEIGHT;
  DMA_InitStruct.BlkDataLength = 0x00000000U;
  DMA_InitStruct.TriggerMode = LL_DMA_TRIGM_BLK_TRANSFER;
  DMA_InitStruct.TriggerPolarity = LL_DMA_TRIG_POLARITY_MASKED;
  DMA_InitStruct.TriggerSelection = 0x00000000U;
  DMA_InitStruct.Request = LL_GPDMA1_REQUEST_SPI1_TX;
  DMA_InitStruct.TransferEventMode = LL_DMA_TCEM_BLK_TRANSFER;
  DMA_InitStruct.SrcAllocatedPort = LL_DMA_SRC_ALLOCATED_PORT0;
  DMA_InitStruct.DestAllocatedPort = LL_DMA_DEST_ALLOCATED_PORT0;
  DMA_InitStruct.LinkAllocatedPort = LL_DMA_LINK_ALLOCATED_PORT1;
  DMA_InitStruct.LinkStepMode = LL_DMA_LSM_FULL_EXECUTION;
  DMA_InitStruct.LinkedListBaseAddr = 0x00000000U;
  DMA_InitStruct.LinkedListAddrOffset = 0x00000000U;
  LL_DMA_Init(GPDMA1, LL_DMA_CHANNEL_2, &DMA_InitStruct);

  /* GPDMA1_REQUEST_SPI1_RX Init */
  DMA_InitStruct.SrcAddress = 0x00000000U;
  DMA_InitStruct.DestAddress = 0x00000000U;
  DMA_InitStruct.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
  DMA_InitStruct.BlkHWRequest = LL_DMA_HWREQUEST_SINGLEBURST;
  DMA_InitStruct.DataAlignment = LL_DMA_DATA_ALIGN_ZEROPADD;
  DMA_InitStruct.SrcBurstLength = 1;
  DMA_InitStruct.DestBurstLength = 1;
  DMA_InitStruct.SrcDataWidth = LL_DMA_SRC_DATAWIDTH_BYTE;
  DMA_InitStruct.DestDataWidth = LL_DMA_DEST_DATAWIDTH_BYTE;
  DMA_InitStruct.SrcIncMode = LL_DMA_SRC_FIXED;
  DMA_InitStruct.DestIncMode = LL_DMA_DEST_INCREMENT;
  DMA_InitStruct.Priority = LL_DMA_LOW_PRIORITY_LOW_WEIGHT;
  DMA_InitStruct.BlkDataLength = 0x00000000U;
  DMA_InitStruct.TriggerMode = LL_DMA_TRIGM_BLK_TRANSFER;
  DMA_InitStruct.TriggerPolarity = LL_DMA_TRIG_POLARITY_MASKED;
  DMA_InitStruct.TriggerSelection = 0x00000000U;
  DMA_InitStruct.Request = LL_GPDMA1_REQUEST_SPI1_RX;
  DMA_InitStruct.TransferEventMode = LL_DMA_TCEM_BLK_TRANSFER;
  DMA_InitStruct.SrcAllocatedPort = LL_DMA_SRC_ALLOCATED_PORT0;
  DMA_InitStruct.DestAllocatedPort = LL_DMA_DEST_ALLOCATED_PORT0;
  DMA_InitStruct.LinkAllocatedPort = LL_DMA_LINK_ALLOCATED_PORT1;
  DMA_InitStruct.LinkStepMode = LL_DMA_LSM_FULL_EXECUTION;
  DMA_InitStruct.LinkedListBaseAddr = 0x00000000U;
  DMA_InitStruct.LinkedListAddrOffset = 0x00000000U;
  LL_DMA_Init(GPDMA1, LL_DMA_CHANNEL_1, &DMA_InitStruct);

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_SLAVE;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 0x7;
  LL_SPI_Init(SPI1, &SPI_InitStruct);
  LL_SPI_SetStandard(SPI1, LL_SPI_PROTOCOL_MOTOROLA);
  LL_SPI_DisableNSSPulseMgt(SPI1);
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

  /**/
  LL_GPIO_SetOutputPin(LD2_GPIO_Port, LD2_Pin);

  /**/
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief  This function Activate SPI1
  * @param  None
  * @retval None
  */
void Activate_SPI(void)
{
  /* Enable SPI1 */
  LL_SPI_Enable(SPI1);

  /* Enable DMA Channels */
  LL_DMA_EnableChannel(GPDMA1, LL_DMA_CHANNEL_2);
  LL_DMA_EnableChannel(GPDMA1, LL_DMA_CHANNEL_1);
}

/**
  * @brief  Turn-on LD1.
  * @param  None
  * @retval None
  */
void LED_On(void)
{
  /* Turn LD1 on */
  LL_GPIO_ResetOutputPin(LD1_GPIO_Port, LD1_Pin);
}


/**
  * @brief  Set LD1 to Blinking mode for an infinite loop (toggle period based on value provided as input parameter).
  * @param  Period : Period of time (in ms) between each toggling of LED
  *   This parameter can be user defined values. Pre-defined values used in that example are :
  *     @arg LED_BLINK_FAST : Fast Blinking
  *     @arg LED_BLINK_SLOW : Slow Blinking
  *     @arg LED_BLINK_ERROR : Error specific Blinking
  * @retval None
  */
void LED_Blinking(uint32_t Period)
{
  /* Toggle LD1 in an infinite loop */
  while (1)
  {
    LL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
    LL_mDelay(Period);
  }
}


/**
  * @brief  Wait end of transfer and check if received Data are well.
  * @param  None
  * @retval None
  */
void WaitAndCheckEndOfTransfer(void)
{
  /* 1 - Wait end of transmission */
  while (ubTransmissionComplete != 1)
  {
  }
  /* Disable GPDMA1 Tx Channel */
  LL_DMA_DisableChannel(GPDMA1, LL_DMA_CHANNEL_1);
  /* 2 - Wait end of reception */
  while (ubReceptionComplete != 1)
  {
  }
  /* Disable GPDMA1 Rx Channel */
  LL_DMA_DisableChannel(GPDMA1, LL_DMA_CHANNEL_2);
  /* 3 - Compare Transmit data to receive data */
  if (Buffercmp8((uint8_t *)aTxBuffer, (uint8_t *)aRxBuffer, ubNbDataToTransmit))
  {
    /* Processing Error */
    LED_Blinking(LED_BLINK_ERROR);
  }
  else
  {
    /* Turn On Led if data are well received */
    LED_On();
  }
}

/**
* @brief Compares two 8-bit buffers and returns the comparison result.
* @param pBuffer1: pointer to the source buffer to be compared to.
* @param pBuffer2: pointer to the second source buffer to be compared to the first.
* @param BufferLength: buffer's length.
* @retval   0: Comparison is OK (the two Buffers are identical)
*           Value different from 0: Comparison is NOK (Buffers are different)
*/
uint8_t Buffercmp8(uint8_t *pBuffer1, uint8_t *pBuffer2, uint8_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return 1;
    }

    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}


/******************************************************************************/
/*   USER IRQ HANDLER TREATMENT Functions                                     */
/******************************************************************************/
/**
  * @brief  Function to manage USER push-button
  * @param  None
  * @retval None
  */
void UserButton_Callback(void)
{
  /* Update USER push-button variable : to be checked in waiting loop in main program */
  ubButtonPress = 1;
}

/**
  * @brief  Function called from GPDMA1 IRQ Handler when Rx transfer is completed
  * @param  None
  * @retval None
  */
void GPDMA1_ReceiveComplete_Callback(void)
{
  /* DMA Rx transfer completed */
  ubReceptionComplete = 1;
}

/**
  * @brief  Function called from GPDMA1 IRQ Handler when Tx transfer is completed
  * @param  None
  * @retval None
  */
void GPDMA1_TransmitComplete_Callback(void)
{
  /* DMA Tx transfer completed */
  ubTransmissionComplete = 1;
}

/**
  * @brief  Function called in case of error detected in SPI IT Handler
  * @param  None
  * @retval None
  */
void SPI1_TransferError_Callback(void)
{
  /* Disable GPDMA1 Rx Channel */
  LL_DMA_DisableChannel(GPDMA1, LL_DMA_CHANNEL_2);

  /* Disable GPDMA1 Tx Channel */
  LL_DMA_DisableChannel(GPDMA1, LL_DMA_CHANNEL_1);
  /* Set LD1 to Blinking mode to indicate error occurs */
  LED_Blinking(LED_BLINK_ERROR);
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
