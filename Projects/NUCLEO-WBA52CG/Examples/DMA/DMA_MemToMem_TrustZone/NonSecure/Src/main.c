/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    NonSecure/Src/main.c
  * @author  MCD Application Team
  * @brief   Non-secure main application
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>     /* for memcmp() */

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

DMA_HandleTypeDef handle_GPDMA1_Channel3;
DMA_HandleTypeDef handle_GPDMA1_Channel2;
DMA_HandleTypeDef handle_GPDMA1_Channel1;
DMA_HandleTypeDef handle_GPDMA1_Channel0;

/* USER CODE BEGIN PV */

static uint32_t aSRC_Const_Buffer[BUFFER_SIZE] =
{
  0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10,
  0x11121314, 0x15161718, 0x191A1B1C, 0x1D1E1F20,
  0x21222324, 0x25262728, 0x292A2B2C, 0x2D2E2F30,
  0x31323334, 0x35363738, 0x393A3B3C, 0x3D3E3F40,
  0x41424344, 0x45464748, 0x494A4B4C, 0x4D4E4F50,
  0x51525354, 0x55565758, 0x595A5B5C, 0x5D5E5F60,
  0x61626364, 0x65666768, 0x696A6B6C, 0x6D6E6F70,
  0x71727374, 0x75767778, 0x797A7B7C, 0x7D7E7F80
};

static uint32_t aDST_Buffer1[BUFFER_SIZE];   /* Non-secure buffer to receive secure data */
static uint32_t aDST_Buffer2[BUFFER_SIZE];   /* Non-secure buffer copy */

static __IO uint32_t transferCompleteDetected; /* Set to 1 if transfer is correctly completed */
static __IO uint32_t transferErrorDetected; /* Set to 1 if an error transfer is detected */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPDMA1_Init(void);
static void MX_ICACHE_Init(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
static void NonSecureTransferComplete(DMA_HandleTypeDef *hdma_memtomem_dma1_channel1);
static void NonSecureTransferError(DMA_HandleTypeDef *hdma_memtomem_dma1_channel1);
/* Following callbacks are called by secure */
static void SecureNonSecureTransferCompleteCallback(DMA_HandleTypeDef *hdma_memtomem_dma1_channelx);
static void NonSecureSecureTransferCompleteCallback(DMA_HandleTypeDef *hdma_memtomem_dma1_channelx);
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

  /* STM32WBAxx **NON-SECURE** HAL library initialization:
       - Non-secure Systick timer is configured by default as source of time base,
         but user can eventually implement his proper time base source (a general
         purpose timer for example or other time source), keeping in mind that
         Time base duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined
         and handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
  */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPDMA1_Init();
  MX_ICACHE_Init();
  /* USER CODE BEGIN 2 */
  /* Initialize LEDs */
  BSP_LED_Init(LD3);

  /* Step 1 */
  /* Get secure data into non-secure RAM area */
  /* through secure DMA channels via Non-Secure Callable secure service */

  /* Reset transferCompleteDetected to 0, it will be set to 1 if a transfer is correctly completed */
  transferCompleteDetected = 0;

  if (SECURE_DMA_Secure_To_NonSecure((uint32_t *)aDST_Buffer1,
                                     BUFFER_SIZE,
                                     (void *)SecureNonSecureTransferCompleteCallback) == ERROR)
  {
    Error_Handler();
  }

  /* Wait for notification completion */
  while (transferCompleteDetected == 0);

  /* Check copied RAM buffer expected content */
  /* Compare the secure destination verus the source */
  if (memcmp((uint8_t *)aDST_Buffer1,
             (uint8_t *)aSRC_Const_Buffer,
             sizeof(uint32_t) * BUFFER_SIZE) != 0)
  {
    Error_Handler();   /* Infinite loop */
  }

  /* Step 2 */
  /* Copy data into second non-secure RAM area */

  /* Reset transferCompleteDetected to 0, it will be set to 1 if a transfer is correctly completed */
  transferCompleteDetected = 0;
  /* Reset transferErrorDetected to 0, it will be set to 1 if a transfer error is detected */
  transferErrorDetected = 0;

  /* Select Callbacks functions called after Transfer complete and Transfer error */
  HAL_DMA_RegisterCallback(&handle_GPDMA1_Channel3, HAL_DMA_XFER_CPLT_CB_ID, NonSecureTransferComplete);
  HAL_DMA_RegisterCallback(&handle_GPDMA1_Channel3, HAL_DMA_XFER_ERROR_CB_ID, NonSecureTransferError);

  /* Configure the source, destination and buffer size DMA fields and Start DMA channel transfer */
  /* Enable DMA TC and TE interrupts */
  if (HAL_DMA_Start_IT(&handle_GPDMA1_Channel3,
                       (uint32_t)&aDST_Buffer1,
                       (uint32_t)&aDST_Buffer2,
                       sizeof(uint32_t) * BUFFER_SIZE) != HAL_OK)
  {
    /* Transfer Error */
    Error_Handler();
  }

  /* Wait for end of DMA transfer */
  while ((transferCompleteDetected == 0) &&
         (transferErrorDetected == 0));

  if (transferErrorDetected == 1)
  {
    Error_Handler();  /* Infinite loop */
  }

  /* Check copied RAM buffer expected content */
  /* Compare the secure destination verus the source */
  if (memcmp((uint8_t *)aDST_Buffer2,
             (uint8_t *)aDST_Buffer1,
             sizeof(uint32_t) * BUFFER_SIZE) != 0)
  {
    Error_Handler();   /* Infinite loop */
  }

  /* Step 3 */
  /* Provide non-secure data to secure */
  /* through secure DMA channels via Non-Secure Callable secure service */

  /* Reset transferCompleteDetected to 0, it will be set to 1 if a transfer is correctly completed */
  transferCompleteDetected = 0;

  if (SECURE_DMA_NonSecure_To_Secure((uint32_t *)aDST_Buffer2,
                                     BUFFER_SIZE,
                                     (void *)NonSecureSecureTransferCompleteCallback) == ERROR)
  {
    Error_Handler();
  }

  /* Wait for notification completion */
  while (transferCompleteDetected == 0);

  /* Step 4 */
  /* Secure data coherency check */
  if (SECURE_DATA_Last_Buffer_Compare() == SUCCESS)
  {
    /* LD3 is on for end of test */
    BSP_LED_On(LD3);
  }
  else
  {
    Error_Handler();
  }


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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL1.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL1.PLLM = 1;
  RCC_OscInitStruct.PLL1.PLLN = 12;
  RCC_OscInitStruct.PLL1.PLLP = 2;
  RCC_OscInitStruct.PLL1.PLLQ = 2;
  RCC_OscInitStruct.PLL1.PLLR = 2;
  RCC_OscInitStruct.PLL1.PLLFractional = 4096;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK7|RCC_CLOCKTYPE_HCLK5;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB7CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHB5_PLL1_CLKDivider = RCC_SYSCLK_PLL1_DIV4;
  RCC_ClkInitStruct.AHB5_HSEHSI_CLKDivider = RCC_SYSCLK_HSEHSI_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
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
  __HAL_RCC_GPDMA1_CLK_ENABLE();

  /* GPDMA1 interrupt Init */
    HAL_NVIC_SetPriority(GPDMA1_Channel3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel3_IRQn);

  /* USER CODE BEGIN GPDMA1_Init 1 */

  /* USER CODE END GPDMA1_Init 1 */
  handle_GPDMA1_Channel3.Instance = GPDMA1_Channel3;
  handle_GPDMA1_Channel3.Init.Request = DMA_REQUEST_SW;
  handle_GPDMA1_Channel3.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
  handle_GPDMA1_Channel3.Init.Direction = DMA_MEMORY_TO_MEMORY;
  handle_GPDMA1_Channel3.Init.SrcInc = DMA_SINC_INCREMENTED;
  handle_GPDMA1_Channel3.Init.DestInc = DMA_DINC_INCREMENTED;
  handle_GPDMA1_Channel3.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
  handle_GPDMA1_Channel3.Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
  handle_GPDMA1_Channel3.Init.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;
  handle_GPDMA1_Channel3.Init.SrcBurstLength = 1;
  handle_GPDMA1_Channel3.Init.DestBurstLength = 1;
  handle_GPDMA1_Channel3.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0|DMA_DEST_ALLOCATED_PORT1;
  handle_GPDMA1_Channel3.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
  handle_GPDMA1_Channel3.Init.Mode = DMA_NORMAL;
  if (HAL_DMA_Init(&handle_GPDMA1_Channel3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel3, DMA_CHANNEL_PRIV) != HAL_OK)
  {
    Error_Handler();
  }
  handle_GPDMA1_Channel2.Instance = GPDMA1_Channel2;
  handle_GPDMA1_Channel2.Init.Request = DMA_REQUEST_SW;
  handle_GPDMA1_Channel2.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
  handle_GPDMA1_Channel2.Init.Direction = DMA_MEMORY_TO_MEMORY;
  handle_GPDMA1_Channel2.Init.SrcInc = DMA_SINC_INCREMENTED;
  handle_GPDMA1_Channel2.Init.DestInc = DMA_DINC_INCREMENTED;
  handle_GPDMA1_Channel2.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
  handle_GPDMA1_Channel2.Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
  handle_GPDMA1_Channel2.Init.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;
  handle_GPDMA1_Channel2.Init.SrcBurstLength = 1;
  handle_GPDMA1_Channel2.Init.DestBurstLength = 1;
  handle_GPDMA1_Channel2.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0|DMA_DEST_ALLOCATED_PORT1;
  handle_GPDMA1_Channel2.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
  handle_GPDMA1_Channel2.Init.Mode = DMA_NORMAL;
  if (HAL_DMA_Init(&handle_GPDMA1_Channel2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel2, DMA_CHANNEL_PRIV) != HAL_OK)
  {
    Error_Handler();
  }
  handle_GPDMA1_Channel1.Instance = GPDMA1_Channel1;
  handle_GPDMA1_Channel1.Init.Request = DMA_REQUEST_SW;
  handle_GPDMA1_Channel1.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
  handle_GPDMA1_Channel1.Init.Direction = DMA_MEMORY_TO_MEMORY;
  handle_GPDMA1_Channel1.Init.SrcInc = DMA_SINC_INCREMENTED;
  handle_GPDMA1_Channel1.Init.DestInc = DMA_DINC_INCREMENTED;
  handle_GPDMA1_Channel1.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
  handle_GPDMA1_Channel1.Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
  handle_GPDMA1_Channel1.Init.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;
  handle_GPDMA1_Channel1.Init.SrcBurstLength = 1;
  handle_GPDMA1_Channel1.Init.DestBurstLength = 1;
  handle_GPDMA1_Channel1.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0|DMA_DEST_ALLOCATED_PORT1;
  handle_GPDMA1_Channel1.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
  handle_GPDMA1_Channel1.Init.Mode = DMA_NORMAL;
  if (HAL_DMA_Init(&handle_GPDMA1_Channel1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel1, DMA_CHANNEL_PRIV) != HAL_OK)
  {
    Error_Handler();
  }
  handle_GPDMA1_Channel0.Instance = GPDMA1_Channel0;
  handle_GPDMA1_Channel0.Init.Request = DMA_REQUEST_SW;
  handle_GPDMA1_Channel0.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
  handle_GPDMA1_Channel0.Init.Direction = DMA_MEMORY_TO_MEMORY;
  handle_GPDMA1_Channel0.Init.SrcInc = DMA_SINC_INCREMENTED;
  handle_GPDMA1_Channel0.Init.DestInc = DMA_DINC_INCREMENTED;
  handle_GPDMA1_Channel0.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
  handle_GPDMA1_Channel0.Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
  handle_GPDMA1_Channel0.Init.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;
  handle_GPDMA1_Channel0.Init.SrcBurstLength = 1;
  handle_GPDMA1_Channel0.Init.DestBurstLength = 1;
  handle_GPDMA1_Channel0.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0|DMA_DEST_ALLOCATED_PORT1;
  handle_GPDMA1_Channel0.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
  handle_GPDMA1_Channel0.Init.Mode = DMA_NORMAL;
  if (HAL_DMA_Init(&handle_GPDMA1_Channel0) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel0, DMA_CHANNEL_PRIV) != HAL_OK)
  {
    Error_Handler();
  }
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
  if (HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */

}

/* USER CODE BEGIN 4 */
/**
  * @brief  DMA non-secure transfer complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated
  * @retval None
  */
static void NonSecureTransferComplete(DMA_HandleTypeDef *hdma_memtomem_dma1_channel1)
{
  transferCompleteDetected = 1;
}

/**
  * @brief  DMA non-secure transfer error callback
  * @note   This function is executed when the transfer error interrupt
  *         is generated during DMA transfer
  * @retval None
  */
static void NonSecureTransferError(DMA_HandleTypeDef *hdma_memtomem_dma1_channel1)
{
  transferErrorDetected = 1;
}

/**
  * @brief  DMA secure to non-secure transfer complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated
  * @retval None
  */
static void SecureNonSecureTransferCompleteCallback(DMA_HandleTypeDef *hdma_memtomem_dma1_channelx)
{
  transferCompleteDetected = 1;
}

/**
  * @brief  DMA non-secure to secure transfer complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated
  * @retval None
  */
static void NonSecureSecureTransferCompleteCallback(DMA_HandleTypeDef *hdma_memtomem_dma1_channelx)
{
  transferCompleteDetected = 1;
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
  while (1)
  {
    BSP_LED_Toggle(LD3);
    HAL_Delay(500);
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)
  {}
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
