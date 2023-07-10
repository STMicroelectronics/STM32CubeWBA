/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct
{
  uint32_t Memory_address;
  uint32_t Mask_error;
  uint32_t Byte_position;
  uint32_t Access_type;
}RAMCFG_ParityErrorGenTypeDef;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DATA_VALUE_32                  (0xAA55AA55U)
#define SRAM2_DATA_SIZE_32             (SRAM2_SIZE / 4U)

#define TOGGLE_COUNT                   (10U)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

RAMCFG_HandleTypeDef hramcfg_SRAM2;

RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN PV */
FLASH_OBProgramInitTypeDef FLASH_OBInitStruct = {0};

/* Handler declaration */
DMA_HandleTypeDef    hdma;

RAMCFG_ParityErrorGenTypeDef const ArraySRAMParityError[] =
{
 /* Memory_address, Mask_error, Byte_position, Access_type */
 {     0x20014000, 0xFFFFF7FF,             2, RAMCFG_PARITY_ERROR_CPU_ACCESS},       /* Parity Error generated @ 0x20014000 with CPU read access             */
 {     0x2001FFF0, 0xFFEFFFFF,             4, RAMCFG_PARITY_ERROR_DMA_PORT0_ACCESS}, /* Parity Error generated @ 0x2001FFF0 with DMA read access with port 0 */
 {     0x20018654, 0xFFFFFFFE,             1, RAMCFG_PARITY_ERROR_DMA_PORT1_ACCESS}, /* Parity Error generated @ 0x20018654 with DMA read access with port 1 */

  /* Dummy last element */
  {0U,0U,0U,0U},
};

uint32_t Read_data;
__IO uint32_t Parity_error_interrupt_count  = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RAMCFG_Init(void);
static void MX_ICACHE_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */
static void DMA_Init(uint32_t AllocationPort);
static void Toggle_LED(Led_TypeDef Led_id, uint32_t Wait_time, uint32_t Count);
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
  RAMCFG_PEAddressTypeDef sPEAddress;
  uint32_t step_idx = 0;
  uint32_t loop = 0;

  /* STM32WBAxx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
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
  /* Initialize LD1, LD2 and LD3 */
  BSP_LED_Init(LD1);
  BSP_LED_Init(LD2);
  BSP_LED_Init(LD3);

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_RAMCFG_Init();
  MX_ICACHE_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

  /* Allow Access to Flash control registers and user Flash */
  if (HAL_FLASH_Unlock() != HAL_OK)
  {
    Error_Handler();
  }

  /* Allow Access to option bytes */
  if (HAL_FLASH_OB_Unlock() != HAL_OK)
  {
    Error_Handler();
  }

  /* Get the Option bytes configuration */
  HAL_FLASHEx_OBGetConfig(&FLASH_OBInitStruct);

  /* Restore contexte */
  step_idx = HAL_RTCEx_BKUPRead(&hrtc, 0);

  switch (step_idx)
  {
    case 0:

      /* Check if SRAM2_PARITY_DISABLE OB is set */
      if ((FLASH_OBInitStruct.USERConfig & OB_SRAM2_PARITY_DISABLE) == OB_SRAM2_PARITY_DISABLE)
      {

        /* Update BKUP register */
        HAL_RTCEx_BKUPWrite(&hrtc, 0, 1);
        Toggle_LED(LD1, 100, TOGGLE_COUNT);

        /* Enable Parity error detection on SRAM2 */
        FLASH_OBInitStruct.USERType   = OB_USER_SRAM2_PE;
        FLASH_OBInitStruct.USERConfig = OB_SRAM2_PARITY_ENABLE;
        HAL_FLASHEx_OBProgram(&FLASH_OBInitStruct);

        /* Launch Option bytes loading */
        HAL_FLASH_OB_Launch();
      }

    case 1:
      /* Erase the SRAM memory */
      if (HAL_RAMCFG_Erase(&hramcfg_SRAM2) != HAL_OK)
      {
        Error_Handler();
      }

      /* Write DATA to the SRAM2 memory */
      for (uint32_t i = 0; i < SRAM2_DATA_SIZE_32; i++)
      {
        *(__IO uint32_t *)(SRAM2_BASE + (i * 4U)) = DATA_VALUE_32;
      }

      /* Check the SRAM2 memory content correctness */
      for (uint32_t i = 0; i < SRAM2_DATA_SIZE_32; i++)
      {
        if (*(__IO uint32_t *)(SRAM2_BASE + (i * 4U)) != DATA_VALUE_32)
        {
          Error_Handler();
        }
      }

      /* Update BKUP register */
      HAL_RTCEx_BKUPWrite(&hrtc, 0, 2);
      Toggle_LED(LD1, 100, TOGGLE_COUNT);

      /* Disable Parity error detection on SRAM2 */
      FLASH_OBInitStruct.USERType   = OB_USER_SRAM2_PE;
      FLASH_OBInitStruct.USERConfig = OB_SRAM2_PARITY_DISABLE;
      HAL_FLASHEx_OBProgram(&FLASH_OBInitStruct);

      /* Launch Option bytes loading */
      HAL_FLASH_OB_Launch();

      break;

    case 2:
      /* SRAM2 parity error detection is disabled here */
      loop = 0;
      do
      {
        /* Parity error creation in word aligned address */
        *(__IO uint32_t *)(ArraySRAMParityError[loop].Memory_address) = (DATA_VALUE_32 & ArraySRAMParityError[loop].Mask_error);

        if (*(__IO uint32_t *)(ArraySRAMParityError[loop].Memory_address) != (DATA_VALUE_32 & ArraySRAMParityError[loop].Mask_error))
        {
          Error_Handler();
        }
        loop++;
      }while(ArraySRAMParityError[loop].Memory_address != 0U );

      /* Update BKUP register */
      HAL_RTCEx_BKUPWrite(&hrtc, 0, 3);
      Toggle_LED(LD1, 100, TOGGLE_COUNT);

      /* Enable Parity error detection */
      FLASH_OBInitStruct.USERType   = OB_USER_SRAM2_PE;
      FLASH_OBInitStruct.USERConfig = OB_SRAM2_PARITY_ENABLE;
      HAL_FLASHEx_OBProgram(&FLASH_OBInitStruct);

      /* Launch Option bytes loading */
      HAL_FLASH_OB_Launch();

    case 3:
      /* SRAM2 parity error detection is enabled here */

      /* Reset parity error interrupt count */
      Parity_error_interrupt_count = 0;

      if (HAL_RAMCFG_LatchParityErrorAddress(&hramcfg_SRAM2) != HAL_OK)
      {
        Error_Handler();
      }

      /* Re-enable Parity Error Detection for SRAM2 */
      if (HAL_RAMCFG_EnableNotification(&hramcfg_SRAM2, RAMCFG_IT_PARITYERR) != HAL_OK)
      {
        Error_Handler();
      }

      loop = 0;
      do
      {
        /* Reset callback counter */
        Parity_error_interrupt_count = 0;

        if(ArraySRAMParityError[loop].Access_type == RAMCFG_PARITY_ERROR_CPU_ACCESS)
        {
          /* CPU access */

          /* Read the SRAM 2 content to generate single order error */
          Read_data = *(__IO uint32_t *)(ArraySRAMParityError[loop].Memory_address);

        }else if( (ArraySRAMParityError[loop].Access_type == RAMCFG_PARITY_ERROR_DMA_PORT0_ACCESS) ||
                  (ArraySRAMParityError[loop].Access_type == RAMCFG_PARITY_ERROR_DMA_PORT1_ACCESS))
        {

          /* Init DMA */
          DMA_Init(ArraySRAMParityError[loop].Access_type);

          /* Start DMA channel transfer */
          if (HAL_DMA_Start(&hdma, ArraySRAMParityError[loop].Memory_address, (uint32_t)&Read_data, 4) != HAL_OK)
          {
            Error_Handler();
          }

          /* Polling for transfer complete */
          if (HAL_DMA_PollForTransfer(&hdma, HAL_DMA_FULL_TRANSFER, 0xFFFFU) != HAL_OK)
          {
            Error_Handler();
          }
        }

        /* Wait 1 ms for Parity Error Interruption */
        HAL_Delay(1);

        /* Check if Parity error occurred */
        if(Parity_error_interrupt_count == 0U)
        {
          Error_Handler();
        }

        /* Parity error must have been detected here */
        HAL_RAMCFG_GetParityErrorAddress(&hramcfg_SRAM2, &sPEAddress);
        if (sPEAddress.AddressOffset != (ArraySRAMParityError[loop].Memory_address & 0xFFFF))
        {
          Error_Handler();
        }
        if (sPEAddress.Byte != ArraySRAMParityError[loop].Byte_position)
        {
          Error_Handler();
        }
        if (sPEAddress.BusMasterId != ArraySRAMParityError[loop].Access_type)
        {
          Error_Handler();
        }

        loop++;
      }while(ArraySRAMParityError[loop].Memory_address != 0U );
      break;
  }

  if(step_idx != 3U)
  {
    Error_Handler();
  }

  /* Reset step index */
  HAL_RTCEx_BKUPWrite(&hrtc, 0, 0);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    HAL_Delay(250);
    BSP_LED_Toggle(LD2);
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

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_MEDIUMLOW);

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
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

/**
  * @brief RAMCFG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RAMCFG_Init(void)
{

  /* USER CODE BEGIN RAMCFG_Init 0 */

  /* USER CODE END RAMCFG_Init 0 */

  /* USER CODE BEGIN RAMCFG_Init 1 */

  /* USER CODE END RAMCFG_Init 1 */

  /** Initialize RAMCFG SRAM2
  */
  hramcfg_SRAM2.Instance = RAMCFG_SRAM2;
  if (HAL_RAMCFG_Init(&hramcfg_SRAM2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RAMCFG_Init 2 */

  /* USER CODE END RAMCFG_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_PrivilegeStateTypeDef privilegeState = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  hrtc.Init.BinMode = RTC_BINARY_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  privilegeState.rtcPrivilegeFull = RTC_PRIVILEGE_FULL_NO;
  privilegeState.backupRegisterPrivZone = RTC_PRIVILEGE_BKUP_ZONE_NONE;
  privilegeState.backupRegisterStartZone2 = RTC_BKP_DR0;
  privilegeState.backupRegisterStartZone3 = RTC_BKP_DR0;
  if (HAL_RTCEx_PrivilegeModeSet(&hrtc, &privilegeState) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void DMA_Init(uint32_t AllocationPort)
{
  /* Enable DMA clock */
  __HAL_RCC_GPDMA1_CLK_ENABLE();

  /* Set parameters to be configured */
  hdma.Init.Request               = DMA_REQUEST_SW;
  hdma.Init.Direction             = DMA_MEMORY_TO_MEMORY;
  hdma.Init.SrcInc                = DMA_SINC_INCREMENTED;
  hdma.Init.DestInc               = DMA_DINC_INCREMENTED;
  hdma.Init.SrcBurstLength        = 1;
  hdma.Init.DestBurstLength       = 1;
  hdma.Init.SrcDataWidth          = DMA_SRC_DATAWIDTH_WORD;
  hdma.Init.DestDataWidth         = DMA_DEST_DATAWIDTH_WORD;
  hdma.Init.Priority              = DMA_HIGH_PRIORITY;
  hdma.Init.TransferEventMode     = DMA_TCEM_BLOCK_TRANSFER;
  hdma.Init.BlkHWRequest          = DMA_BREQ_SINGLE_BURST;
  hdma.Init.TransferEventMode     = DMA_TCEM_LAST_LL_ITEM_TRANSFER;
  hdma.Init.Mode                  = DMA_NORMAL;

  if(AllocationPort == RAMCFG_PARITY_ERROR_DMA_PORT0_ACCESS)
  {
    hdma.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT1;
  }
  else if(AllocationPort == RAMCFG_PARITY_ERROR_DMA_PORT1_ACCESS)
  {
    hdma.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT1 | DMA_DEST_ALLOCATED_PORT0;
  }else
  {
    Error_Handler();
  }

  /* Set instance to be tested */
  hdma.Instance = GPDMA1_Channel0;

  /* Initialize the DMA Channel */
  if (HAL_DMA_Init(&hdma) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  RAMCFG parity error detection callback.
  * @param  hramcfg : Pointer to a RAMCFG_HandleTypeDef structure that contains
  *                   the configuration information for the specified RAMCFG.
  * @retval None.
  */
void HAL_RAMCFG_DetectParityErrorCallback(RAMCFG_HandleTypeDef *hramcfg)
{
  Parity_error_interrupt_count++;
}

/**
  * @brief  This function toggle a led.
  * @retval None
  */
static void Toggle_LED(Led_TypeDef Led_id, uint32_t Wait_time, uint32_t Count)
{
  for (uint32_t i = 0; i < Count; i++)
  {
    /* Toggle the Led */
    BSP_LED_Toggle(Led_id);

    HAL_Delay (Wait_time);
  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */

  /* Reset step index */
  HAL_RTCEx_BKUPWrite(&hrtc, 0, 0);

  /* User can add his own implementation to report the HAL error return state */
  /* Turn LD3 on */
  BSP_LED_On(LD3);
  __disable_irq();
  while (1)
  {
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
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
