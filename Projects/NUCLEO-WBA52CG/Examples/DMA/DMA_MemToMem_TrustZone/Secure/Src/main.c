/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Secure/Src/main.c
  * @author  MCD Application Team
  * @brief   Secure main application
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
#include <string.h>    /* for memcmp() */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* Non-secure Vector table to jump to (internal Flash Bank2 here)             */
/* Caution: address must correspond to non-secure internal Flash where is     */
/*          mapped in the non-secure vector table                             */
#define VTOR_TABLE_NS_START_ADDR  0x08080000UL
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

uint32_t aSRC_SEC_ROM_Buffer[BUFFER_SIZE] =
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

uint32_t aDST_SEC_RAM_Buffer[BUFFER_SIZE];
uint32_t aDST_SEC_RAM_COPY_Buffer[BUFFER_SIZE];

static __IO uint32_t SecureToSecureTransferErrorDetected;    /* Set to 1 if an error transfer is detected */
static __IO uint32_t SecureToSecureTransferCompleteDetected; /* Set to 1 if transfer is correctly completed */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void NonSecure_Init(void);
static void MX_GPDMA1_Init(void);
static void MX_GTZC_S_Init(void);
/* USER CODE BEGIN PFP */
static void SecureToSecureTransferComplete(DMA_HandleTypeDef *hdma_memtomem_dma1_channel1);
static void SecureToSecureTransferError(DMA_HandleTypeDef *hdma_memtomem_dma1_channel1);
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

  /* STM32WBAxx **SECURE** HAL library initialization:
       - Secure Systick timer is configured by default as source of time base,
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

  /* GTZC initialisation */
  MX_GTZC_S_Init();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPDMA1_Init();
  /* USER CODE BEGIN 2 */
  /* Initialize LEDs for secure */
  BSP_LED_Init(LD2);
  /* Allocate LD3 (PB.08) to non-secure (reminder all IOs are secure by default */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  HAL_GPIO_ConfigPinAttributes(GPIOB, GPIO_PIN_8, GPIO_PIN_NSEC);

  /* DMA1 Channel1: Select Callbacks functions called after Transfer complete and Transfer error */
  HAL_DMA_RegisterCallback(&handle_GPDMA1_Channel0, HAL_DMA_XFER_CPLT_CB_ID, SecureToSecureTransferComplete);
  HAL_DMA_RegisterCallback(&handle_GPDMA1_Channel0, HAL_DMA_XFER_ERROR_CB_ID, SecureToSecureTransferError);

  /* DMA1 Channel2: Select Callbacks functions called after Transfer complete and Transfer error */
  HAL_DMA_RegisterCallback(&handle_GPDMA1_Channel1, HAL_DMA_XFER_CPLT_CB_ID, SecureToNonSecureTransferComplete);
  HAL_DMA_RegisterCallback(&handle_GPDMA1_Channel1, HAL_DMA_XFER_ERROR_CB_ID, SecureToNonSecureTransferError);

  /* DMA1 Channel3: Select Callbacks functions called after Transfer complete and Transfer error */
  HAL_DMA_RegisterCallback(&handle_GPDMA1_Channel2, HAL_DMA_XFER_CPLT_CB_ID, NonSecureToSecureTransferComplete);
  HAL_DMA_RegisterCallback(&handle_GPDMA1_Channel2, HAL_DMA_XFER_ERROR_CB_ID, NonSecureToSecureTransferError);

  /* Configure the source, destination and buffer size DMA fields and Start DMA channel transfer */
  /* Enable DMA TC and TE interrupts */
  if (HAL_DMA_Start_IT(&handle_GPDMA1_Channel0, (uint32_t)&aSRC_SEC_ROM_Buffer, (uint32_t)&aDST_SEC_RAM_Buffer, sizeof(uint32_t) * BUFFER_SIZE) != HAL_OK)
  {
    /* Transfer Error */
    Error_Handler();
  }

  /* Wait for end of DMA transfer */
  while ((SecureToSecureTransferCompleteDetected == 0) &&
         (SecureToSecureTransferErrorDetected == 0));

  if (SecureToSecureTransferErrorDetected == 1)
  {
    Error_Handler();   /* Infinite loop */
  }

  /* Compare the secure destination versus the source */
  if (memcmp((uint8_t *)aDST_SEC_RAM_Buffer,
             (uint8_t *)aSRC_SEC_ROM_Buffer,
             sizeof(uint32_t) * BUFFER_SIZE) == 0)
  {
    /* Turn LD2 on*/
    BSP_LED_On(LD2);
  }
  else
  {
    Error_Handler();   /* Infinite loop */
  }

  /* Secure SysTick should rather be suspended before calling non-secure  */
  /* in order to avoid wake-up from sleep mode entered by non-secure      */
  /* The Secure SysTick shall be resumed on non-secure callable functions */
  HAL_SuspendTick();

  /* USER CODE END 2 */

  /*************** Setup and jump to non-secure *******************************/

  NonSecure_Init();

  /* Non-secure software does not return, this code is not executed */
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
  * @brief  Non-secure call function
  *         This function is responsible for Non-secure initialization and switch
  *         to non-secure state
  * @retval None
  */
static void NonSecure_Init(void)
{
  funcptr_NS NonSecure_ResetHandler;

  SCB_NS->VTOR = VTOR_TABLE_NS_START_ADDR;

  /* Set non-secure main stack (MSP_NS) */
  __TZ_set_MSP_NS((*(uint32_t *)VTOR_TABLE_NS_START_ADDR));

  /* Get non-secure reset handler */
  NonSecure_ResetHandler = (funcptr_NS)(*((uint32_t *)((VTOR_TABLE_NS_START_ADDR) + 4U)));

  /* Start non-secure state software application */
  NonSecure_ResetHandler();
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
    HAL_NVIC_SetPriority(GPDMA1_Channel0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel0_IRQn);
    HAL_NVIC_SetPriority(GPDMA1_Channel1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel1_IRQn);
    HAL_NVIC_SetPriority(GPDMA1_Channel2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel2_IRQn);

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
  if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel3, DMA_CHANNEL_PRIV|DMA_CHANNEL_NSEC
                              |DMA_CHANNEL_SRC_NSEC|DMA_CHANNEL_DEST_NSEC) != HAL_OK)
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
  if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel2, DMA_CHANNEL_PRIV|DMA_CHANNEL_SEC
                              |DMA_CHANNEL_SRC_NSEC|DMA_CHANNEL_DEST_SEC) != HAL_OK)
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
  if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel1, DMA_CHANNEL_PRIV|DMA_CHANNEL_SEC
                              |DMA_CHANNEL_SRC_SEC|DMA_CHANNEL_DEST_NSEC) != HAL_OK)
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
  if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel0, DMA_CHANNEL_PRIV|DMA_CHANNEL_SEC
                              |DMA_CHANNEL_SRC_SEC|DMA_CHANNEL_DEST_SEC) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN GPDMA1_Init 2 */

  /* USER CODE END GPDMA1_Init 2 */

}

/**
  * @brief GTZC_S Initialization Function
  * @param None
  * @retval None
  */
static void MX_GTZC_S_Init(void)
{

  /* USER CODE BEGIN GTZC_S_Init 0 */

  /* USER CODE END GTZC_S_Init 0 */

  MPCBB_ConfigTypeDef MPCBB_Area_Desc = {0};

  /* USER CODE BEGIN GTZC_S_Init 1 */

  /* USER CODE END GTZC_S_Init 1 */
  MPCBB_Area_Desc.SecureRWIllegalMode = GTZC_MPCBB_SRWILADIS_ENABLE;
  MPCBB_Area_Desc.InvertSecureState = GTZC_MPCBB_INVSECSTATE_NOT_INVERTED;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[0] =   0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[1] =   0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[2] =   0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[3] =   0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[0] =   0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[1] =   0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[2] =   0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[3] =   0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_LockConfig_array[0] =   0x00000000;
  if (HAL_GTZC_MPCBB_ConfigMem(SRAM2_BASE, &MPCBB_Area_Desc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN GTZC_S_Init 2 */

  /* USER CODE END GTZC_S_Init 2 */

}

/* USER CODE BEGIN 4 */
/**
  * @brief  DMA conversion complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated
  * @retval None
  */
static void SecureToSecureTransferComplete(DMA_HandleTypeDef *hdma_memtomem_dma1_channel1)
{
  SecureToSecureTransferCompleteDetected = 1;
}

/**
  * @brief  DMA conversion error callback
  * @note   This function is executed when the transfer error interrupt
  *         is generated during DMA transfer
  * @retval None
  */
static void SecureToSecureTransferError(DMA_HandleTypeDef *hdma_memtomem_dma1_channel1)
{
  SecureToSecureTransferErrorDetected = 1;
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

  /* Toggle LD2 on in an infinite loop */
  while (1)
  {
    BSP_LED_Toggle(LD2);
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
