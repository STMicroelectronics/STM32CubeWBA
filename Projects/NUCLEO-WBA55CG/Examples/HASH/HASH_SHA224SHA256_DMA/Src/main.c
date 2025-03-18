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
#include "stdio.h"
#include "string.h"
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

HASH_HandleTypeDef hhash;
DMA_HandleTypeDef handle_GPDMA1_Channel0;

/* USER CODE BEGIN PV */
__ALIGN_BEGIN const __IO uint8_t aInput[] __ALIGN_END = "The STM32WBA52xx multiprotocol wireless and ultra-low-power devices embed a powerful and ultra-low-power radio compliant with the Bluetooth(R) SIG Low Energy specification 5.3";

__ALIGN_BEGIN static uint8_t aSHA224Digest[28] __ALIGN_END;
__ALIGN_BEGIN static uint8_t aExpectSHA224Digest[28] __ALIGN_END = {0x1c, 0x55, 0x4c, 0x3e, 0x1c, 0xe7, 0x1d, 0x99,
                                                                    0xcb, 0x2a, 0x35, 0x23, 0xd4, 0x68, 0x50, 0x20,
                                                                    0x2b, 0x6a, 0x6c, 0xab, 0x0d, 0xf5, 0x34, 0xba,
                                                                    0x10, 0x9b, 0xc3, 0xac
                                                                    };
__ALIGN_BEGIN static uint8_t aSHA256Digest[32] __ALIGN_END;
__ALIGN_BEGIN static uint8_t aExpectSHA256Digest[32] __ALIGN_END = {0x9a, 0xd0, 0x96, 0x3d, 0x57, 0x2f, 0x89, 0xaa,
                                                                    0xa6, 0x52, 0x30, 0x36, 0x45, 0x19, 0xf5, 0x36,
                                                                    0x56, 0x96, 0xbd, 0x1d, 0x0e, 0xac, 0x90, 0x5c,
                                                                    0xa5, 0x8a, 0x2f, 0xaa, 0x4b, 0xa9, 0x90, 0xb3
							            };
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPDMA1_Init(void);
static void MX_HASH_Init(void);
/* USER CODE BEGIN PFP */

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
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* Configure LED1, LED2 and  LED3  */
  BSP_LED_Init(LD1);
  BSP_LED_Init(LD2);
  BSP_LED_Init(LD3);
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPDMA1_Init();
  MX_HASH_Init();
  /* USER CODE BEGIN 2 */
  /****************************************************************************/
  /****************************** SHA224 **************************************/
  /****************************************************************************/

  /* Start HASH computation using DMA transfer */
  if (HAL_HASH_Start_DMA(&hhash, (uint8_t*)aInput, strlen((char const*)aInput), aSHA224Digest) != HAL_OK)
  {
    Error_Handler();
  }
  /* Wait for DMA transfer to complete */ 
  while (HAL_HASH_GetState(&hhash) == HAL_HASH_STATE_BUSY);

 /* Compare computed digest with expected one */
  if(memcmp(aSHA224Digest, aExpectSHA224Digest, sizeof(aExpectSHA224Digest)/sizeof(aExpectSHA224Digest[0])) != 0)
  {
    BSP_LED_On(LD3);
  }
  else
  {
    BSP_LED_On(LD1);
  }
  /****************************************************************************/
  /***************************** SHA256 ***************************************/
  /****************************************************************************/
  if(HAL_HASH_DeInit(&hhash) != HAL_OK)
  {
    Error_Handler();
  }
  hhash.Init.DataType = HASH_BYTE_SWAP;
  hhash.Init.Algorithm = HASH_ALGOSELECTION_SHA256;
    
  if (HAL_HASH_Init(&hhash) != HAL_OK)
  {
    Error_Handler();
  }

  /* Start HASH computation using DMA transfer */
  if (HAL_HASH_Start_DMA(&hhash, (uint8_t*)aInput, strlen((char const*)aInput), aSHA256Digest) != HAL_OK)
  {
    Error_Handler();
  }
  /* Wait for DMA transfer to complete */ 
  while (HAL_HASH_GetState(&hhash) == HAL_HASH_STATE_BUSY);

 /* Compare computed digest with expected one */
  if(memcmp(aSHA256Digest, aExpectSHA256Digest, sizeof(aExpectSHA256Digest)/sizeof(aExpectSHA256Digest[0])) != 0)
  {
    BSP_LED_On(LD3);
  }
  else
  {
    BSP_LED_On(LD2);
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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK7|RCC_CLOCKTYPE_HCLK5;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB7CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHB5_PLL1_CLKDivider = RCC_SYSCLK_PLL1_DIV1;
  RCC_ClkInitStruct.AHB5_HSEHSI_CLKDivider = RCC_SYSCLK_HSEHSI_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
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
    HAL_NVIC_SetPriority(GPDMA1_Channel0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel0_IRQn);

  /* USER CODE BEGIN GPDMA1_Init 1 */

  /* USER CODE END GPDMA1_Init 1 */
  /* USER CODE BEGIN GPDMA1_Init 2 */

  /* USER CODE END GPDMA1_Init 2 */

}

/**
  * @brief HASH Initialization Function
  * @param None
  * @retval None
  */
static void MX_HASH_Init(void)
{

  /* USER CODE BEGIN HASH_Init 0 */

  /* USER CODE END HASH_Init 0 */

  /* USER CODE BEGIN HASH_Init 1 */

  /* USER CODE END HASH_Init 1 */
  hhash.Instance = HASH;
  hhash.Init.DataType = HASH_BYTE_SWAP;
  hhash.Init.Algorithm = HASH_ALGOSELECTION_SHA224;
  if (HAL_HASH_Init(&hhash) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN HASH_Init 2 */

  /* USER CODE END HASH_Init 2 */

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
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
