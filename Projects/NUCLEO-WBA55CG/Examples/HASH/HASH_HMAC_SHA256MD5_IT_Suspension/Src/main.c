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
#define LOW_PRIO_INPUT_TAB_SIZE    ((uint32_t) 261)/* The size of the input data "aInput_LowPrio" */
#define HIGH_PRIO_INPUT_TAB_SIZE   ((uint32_t) 264)/* The size of the input data "aInput_HighPrio" */

#define MD5_DIGEST_LENGTH    16
#define SHA256_DIGEST_LENGTH 32
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

HASH_HandleTypeDef hhash;
__ALIGN_BEGIN static const uint8_t pKeyHASH[261] __ALIGN_END = {
                            0x54,0x68,0x65,0x20,0x68,0x61,0x73,0x68,0x20,0x70,
                            0x72,0x6f,0x63,0x65,0x73,0x73,0x6f,0x72,0x20,0x69,
                            0x73,0x20,0x61,0x20,0x66,0x75,0x6c,0x6c,0x79,0x20,
                            0x63,0x6f,0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,
                            0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,0x74,0x61,
                            0x74,0x69,0x6f,0x6e,0x20,0x6f,0x66,0x20,0x74,0x68,
                            0x65,0x20,0x73,0x65,0x63,0x75,0x72,0x65,0x20,0x68,
                            0x61,0x73,0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,
                            0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,0x2d,0x31,
                            0x29,0x2c,0x20,0x74,0x68,0x65,0x20,0x4d,0x44,0x35,
                            0x20,0x28,0x6d,0x65,0x73,0x73,0x61,0x67,0x65,0x2d,
                            0x64,0x69,0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,
                            0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,0x29,0x20,
                            0x68,0x61,0x73,0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,
                            0x69,0x74,0x68,0x6d,0x20,0x61,0x6e,0x64,0x20,0x74,
                            0x68,0x65,0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,
                            0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,0x68,0x20,
                            0x6d,0x65,0x73,0x73,0x61,0x67,0x65,0x20,0x61,0x75,
                            0x74,0x68,0x65,0x6e,0x74,0x69,0x63,0x61,0x74,0x69,
                            0x6f,0x6e,0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,
                            0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x73,
                            0x75,0x69,0x74,0x61,0x62,0x6c,0x65,0x20,0x66,0x6f,
                            0x72,0x20,0x61,0x20,0x76,0x61,0x72,0x69,0x65,0x74,
                            0x79,0x20,0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,
                            0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,0x2a,0x2a,
                            0x2a,0x20,0x53,0x54,0x4d,0x33,0x32,0x20,0x2a,0x2a,
                            0x2a};

/* USER CODE BEGIN PV */
/*
  "The hash processor is a fully compliant implementation of the secure
   hash algorithm (SHA-1), the MD5 (message-digest algorithm 5) hash
   algorithm and the HMAC (keyed-hash message authentication code)
   algorithm suitable for a variety of applications.*** STM32 ***"
*/
__ALIGN_BEGIN uint8_t aInput_LowPrio[LOW_PRIO_INPUT_TAB_SIZE] __ALIGN_END =
                        {0x54,0x68,0x65,0x20,0x68,0x61,0x73,0x68,
                         0x20,0x70,0x72,0x6f,0x63,0x65,0x73,0x73,
                         0x6f,0x72,0x20,0x69,0x73,0x20,0x61,0x20,
                         0x66,0x75,0x6c,0x6c,0x79,0x20,0x63,0x6f,
                         0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,
                         0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,
                         0x74,0x61,0x74,0x69,0x6f,0x6e,0x20,0x6f,
                         0x66,0x20,0x74,0x68,0x65,0x20,0x73,0x65,
                         0x63,0x75,0x72,0x65,0x20,0x68,0x61,0x73,
                         0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,
                         0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,
                         0x2d,0x31,0x29,0x2c,0x20,0x74,0x68,0x65,
                         0x20,0x4d,0x44,0x35,0x20,0x28,0x6d,0x65,
                         0x73,0x73,0x61,0x67,0x65,0x2d,0x64,0x69,
                         0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,
                         0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,
                         0x29,0x20,0x68,0x61,0x73,0x68,0x20,0x61,
                         0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                         0x20,0x61,0x6e,0x64,0x20,0x74,0x68,0x65,
                         0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,
                         0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,
                         0x68,0x20,0x6d,0x65,0x73,0x73,0x61,0x67,
                         0x65,0x20,0x61,0x75,0x74,0x68,0x65,0x6e,
                         0x74,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,
                         0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,
                         0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                         0x20,0x73,0x75,0x69,0x74,0x61,0x62,0x6c,
                         0x65,0x20,0x66,0x6f,0x72,0x20,0x61,0x20,
                         0x76,0x61,0x72,0x69,0x65,0x74,0x79,0x20,
                         0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,
                         0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,
                         0x2a,0x2a,0x2a,0x20,0x53,0x54,0x4d,0x33,
                         0x32,0x20,0x2a,0x2a,0x2a};

__ALIGN_BEGIN uint8_t aInput_HighPrio[HIGH_PRIO_INPUT_TAB_SIZE] __ALIGN_END =                         
                        {0x68,0x54,0x20,0x65,0x61,0x68,0x68,0x73,
                         0x70,0x20,0x6f,0x72,0x65,0x63,0x73,0x73,
                         0x72,0x6f,0x69,0x20,0x20,0x73,0x20,0x61,
                         0x75,0x66,0x6c,0x6c,0x20,0x79,0x6f,0x63,
                         0x70,0x6d,0x69,0x6c,0x6e,0x61,0x20,0x74,
                         0x6d,0x69,0x6c,0x70,0x6d,0x65,0x6e,0x65,
                         0x61,0x74,0x69,0x74,0x6e,0x6f,0x6f,0x20,
                         0x20,0x66,0x68,0x74,0x20,0x65,0x65,0x73,
                         0x75,0x63,0x65,0x72,0x68,0x20,0x73,0x61,
                         0x20,0x68,0x6c,0x61,0x6f,0x67,0x69,0x72,
                         0x68,0x74,0x20,0x6d,0x53,0x28,0x41,0x48,
                         0x31,0x2d,0x2c,0x29,0x74,0x20,0x65,0x68,
                         0x4d,0x20,0x35,0x44,0x28,0x20,0x65,0x6d,
                         0x73,0x73,0x67,0x61,0x2d,0x65,0x69,0x64,
                         0x65,0x67,0x74,0x73,0x61,0x20,0x67,0x6c,
                         0x72,0x6f,0x74,0x69,0x6d,0x68,0x35,0x20,
                         0x20,0x29,0x61,0x68,0x68,0x73,0x61,0x20,
                         0x67,0x6c,0x72,0x6f,0x74,0x69,0x6d,0x68,
                         0x61,0x20,0x64,0x6e,0x74,0x20,0x65,0x68,
                         0x48,0x20,0x41,0x4d,0x20,0x43,0x6b,0x28,
                         0x79,0x65,0x64,0x65,0x68,0x2d,0x73,0x61,
                         0x20,0x68,0x65,0x6d,0x73,0x73,0x67,0x61,
                         0x20,0x65,0x75,0x61,0x68,0x74,0x6e,0x65,
                         0x69,0x74,0x61,0x63,0x69,0x74,0x6e,0x6f,
                         0x63,0x20,0x64,0x6f,0x29,0x65,0x61,0x20,
                         0x67,0x6c,0x72,0x6f,0x74,0x69,0x6d,0x68,
                         0x73,0x20,0x69,0x75,0x61,0x74,0x6c,0x62,
                         0x20,0x65,0x6f,0x66,0x20,0x72,0x20,0x61,
                         0x61,0x76,0x69,0x72,0x74,0x65,0x20,0x79,
                         0x66,0x6f,0x61,0x20,0x70,0x70,0x69,0x6c,
                         0x61,0x63,0x69,0x74,0x6e,0x6f,0x2e,0x73,
                         0x2a,0x2a,0x20,0x2a,0x54,0x53,0x33,0x4d,
                         0x20,0x32,0x2a,0x2a,0x00,0x2a,0x81,0x62};

__ALIGN_BEGIN static uint8_t aSHA256Digest[SHA256_DIGEST_LENGTH] __ALIGN_END; 
__ALIGN_BEGIN static uint8_t aMD5Digest[MD5_DIGEST_LENGTH] __ALIGN_END;                                                    


__ALIGN_BEGIN static uint8_t aExpectSHA256Digest[SHA256_DIGEST_LENGTH] __ALIGN_END = 
  {0x62, 0x46, 0xc3, 0x1d, 0x8b, 0x8b, 0x0b, 0x39, 
   0x5e, 0xcf, 0xd1, 0xe9, 0xf5, 0x68, 0x7e, 0x48, 
   0x5c, 0xe4, 0xe3, 0xc9, 0x02, 0xcb, 0x75, 0xc7, 
   0xb1, 0x11, 0xc0, 0xb2, 0xdf, 0xc2, 0x70, 0xb6};    
             
__ALIGN_BEGIN static uint8_t aExpectMD5Digest[MD5_DIGEST_LENGTH] __ALIGN_END =                                                   
                             {0x87, 0x7e, 0xbb, 0xbf, 0xc8, 0xa9, 0xc0, 0x85,
                             0x78, 0xa2, 0xc3, 0x31, 0xd6, 0x7e, 0xc7, 0x0e};                                                                                                

  uint8_t Storage_Buffer[57 * 4] = {0}; /* 57 32-bit words are saved               */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_ICACHE_Init(void);
static void MX_HASH_Init(void);
/* USER CODE BEGIN PFP */
static void HigherPriorityBlock_Processing(void);
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
  BSP_LED_Init(LD1);
  BSP_LED_Init(LD3);
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_ICACHE_Init();
  MX_HASH_Init();
  /* USER CODE BEGIN 2 */

  /***************************************************/
  /*                                                 */
  /* Start HMAC MD5 processing of low priority block */
  /*                                                 */ 
  /***************************************************/  
  /* For testing purposes, request suspension now */
  /*  this API should be called by priority */
  HAL_HASH_ProcessSuspend(&hhash);

  /* At first call or to resume processing after suspension, the API is invoked with
  the same handle and same parameters */  
  if (HAL_HASH_HMAC_Start_IT(&hhash, aInput_LowPrio, LOW_PRIO_INPUT_TAB_SIZE, aMD5Digest) != HAL_OK)
  {
    Error_Handler();
  }

  /* Wait until processing is suspended (state is HAL_HASH_STATE_SUSPENDED) */
  while (HAL_HASH_GetState(&hhash) != HAL_HASH_STATE_SUSPENDED);

  /* Low priority block context saving operation */
  HAL_HASH_Suspend(&hhash, Storage_Buffer);

  /* Process high priority block */
  HigherPriorityBlock_Processing();

  /***************************************************/  
  /*                                                 */
  /* Low priority block processing resumption        */
  /*                                                 */
  /***************************************************/  
  /* Low priority block context restoring operation */
  HAL_HASH_Resume(&hhash, Storage_Buffer); 

  if (HAL_HASH_HMAC_Start_IT(&hhash, aInput_LowPrio, LOW_PRIO_INPUT_TAB_SIZE, aMD5Digest) != HAL_OK)
  {
    Error_Handler();
  }

  /* Check low priority block digest computation */
  if (memcmp(aMD5Digest, aExpectMD5Digest, MD5_DIGEST_LENGTH) != 0)
  {
    Error_Handler();
  }
  else
  {
    BSP_LED_On(LD1);
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
  hhash.Init.KeySize = 261;
  hhash.Init.pKey = (uint8_t *)pKeyHASH;
  hhash.Init.Algorithm = HASH_ALGOSELECTION_MD5;
  if (HAL_HASH_Init(&hhash) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN HASH_Init 2 */

  /* USER CODE END HASH_Init 2 */

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

  /** Enable instruction cache (default 2-ways set associative cache)
  */
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */

}

/* USER CODE BEGIN 4 */
/**
  * @brief Once low priority block HMAC MD5 processing is suspended, computes high priority block digest.
  *        In the example at hand, the high priority block processing is SHA-256
  *        where data are fed to the IP in interrupt mode.          
  * @param  None
  * @retval None
  */
static void HigherPriorityBlock_Processing(void)
{
  
  /* hhash has been saved previously in HashHandle_saved. 
     For the high priority block, all the initialization parameters are the 
     same but the Data Type */ 
  hhash.Init.DataType  = HASH_HALFWORD_SWAP;
  hhash.Init.Algorithm = HASH_ALGOSELECTION_SHA256;  
  if (HAL_HASH_Init(&hhash) != HAL_OK)
  {
    Error_Handler();
  }  

  /* Computes SHA-256 digest */
  if (HAL_HASH_Start_IT(&hhash, aInput_HighPrio, HIGH_PRIO_INPUT_TAB_SIZE, aSHA256Digest) != HAL_OK)
  {
    Error_Handler();
  }
  /* Wait until processing is over */
  while (HAL_HASH_GetState(&hhash) != HAL_HASH_STATE_READY); 
  
  
  /* Check high priority block digest computation */
  if (memcmp(aSHA256Digest, aExpectSHA256Digest, SHA256_DIGEST_LENGTH) != 0)
  {
    Error_Handler();
  }
  
  return;
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
  __disable_irq();

  while (1)
  {
    BSP_LED_On(LD3);
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
