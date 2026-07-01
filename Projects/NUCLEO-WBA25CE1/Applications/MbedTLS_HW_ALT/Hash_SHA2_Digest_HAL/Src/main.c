/**
  ******************************************************************************
  * @file    MbedTLS_HW_ALT/Hash_SHA2_Digest_HAL/Src/main.c
  * @author  MCD Application Team
  * @brief   This example provides a short description of how to use the
  *          mbeb-crypto with alt function define on HW accelerator
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "main.h"
#include "crypto.h"

/* Global variables ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef enum
{
  FAILED = 0,
  PASSED = 1
} TestStatus;
/* Private defines -----------------------------------------------------------*/
#define CHUNK_SIZE  48u
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** Extract from SHA256LongMsg.rsp
  *
Len = 2096
Msg = 6b918fb1a5ad1f9c5e5dbdf10a93a9c8f6bca89f37e79c9fe12a57227941b173ac79d8d440cde8c64c4ebc84a4c803
d198a296f3de060900cc427f58ca6ec373084f95dd6c7c427ecfbf781f68be572a88dbcbb188581ab200bfb99a3a816407e7
dd6dd21003554d4f7a99c93ebfce5c302ff0e11f26f83fe669acefb0c1bbb8b1e909bd14aa48ba3445c88b0e1190eef765ad
898ab8ca2fe507015f1578f10dce3c11a55fb9434ee6e9ad6cc0fdc4684447a9b3b156b908646360f24fec2d8fa69e2c93db
78708fcd2eef743dcb9353819b8d667c48ed54cd436fb1476598c4a1d7028e6f2ff50751db36ab6bc32435152a00abd3d58d
9a8770d9a3e52d5a3628ae3c9e0325
MD = 46500b6ae1ab40bde097ef168b0f3199049b55545a1588792d39d594f493dca7

  */
const uint8_t Message[] =
{
  0x6b, 0x91, 0x8f, 0xb1, 0xa5, 0xad, 0x1f, 0x9c, 0x5e, 0x5d, 0xbd, 0xf1, 0x0a, 0x93, 0xa9, 0xc8,
  0xf6, 0xbc, 0xa8, 0x9f, 0x37, 0xe7, 0x9c, 0x9f, 0xe1, 0x2a, 0x57, 0x22, 0x79, 0x41, 0xb1, 0x73,
  0xac, 0x79, 0xd8, 0xd4, 0x40, 0xcd, 0xe8, 0xc6, 0x4c, 0x4e, 0xbc, 0x84, 0xa4, 0xc8, 0x03, 0xd1,
  0x98, 0xa2, 0x96, 0xf3, 0xde, 0x06, 0x09, 0x00, 0xcc, 0x42, 0x7f, 0x58, 0xca, 0x6e, 0xc3, 0x73,
  0x08, 0x4f, 0x95, 0xdd, 0x6c, 0x7c, 0x42, 0x7e, 0xcf, 0xbf, 0x78, 0x1f, 0x68, 0xbe, 0x57, 0x2a,
  0x88, 0xdb, 0xcb, 0xb1, 0x88, 0x58, 0x1a, 0xb2, 0x00, 0xbf, 0xb9, 0x9a, 0x3a, 0x81, 0x64, 0x07,
  0xe7, 0xdd, 0x6d, 0xd2, 0x10, 0x03, 0x55, 0x4d, 0x4f, 0x7a, 0x99, 0xc9, 0x3e, 0xbf, 0xce, 0x5c,
  0x30, 0x2f, 0xf0, 0xe1, 0x1f, 0x26, 0xf8, 0x3f, 0xe6, 0x69, 0xac, 0xef, 0xb0, 0xc1, 0xbb, 0xb8,
  0xb1, 0xe9, 0x09, 0xbd, 0x14, 0xaa, 0x48, 0xba, 0x34, 0x45, 0xc8, 0x8b, 0x0e, 0x11, 0x90, 0xee,
  0xf7, 0x65, 0xad, 0x89, 0x8a, 0xb8, 0xca, 0x2f, 0xe5, 0x07, 0x01, 0x5f, 0x15, 0x78, 0xf1, 0x0d,
  0xce, 0x3c, 0x11, 0xa5, 0x5f, 0xb9, 0x43, 0x4e, 0xe6, 0xe9, 0xad, 0x6c, 0xc0, 0xfd, 0xc4, 0x68,
  0x44, 0x47, 0xa9, 0xb3, 0xb1, 0x56, 0xb9, 0x08, 0x64, 0x63, 0x60, 0xf2, 0x4f, 0xec, 0x2d, 0x8f,
  0xa6, 0x9e, 0x2c, 0x93, 0xdb, 0x78, 0x70, 0x8f, 0xcd, 0x2e, 0xef, 0x74, 0x3d, 0xcb, 0x93, 0x53,
  0x81, 0x9b, 0x8d, 0x66, 0x7c, 0x48, 0xed, 0x54, 0xcd, 0x43, 0x6f, 0xb1, 0x47, 0x65, 0x98, 0xc4,
  0xa1, 0xd7, 0x02, 0x8e, 0x6f, 0x2f, 0xf5, 0x07, 0x51, 0xdb, 0x36, 0xab, 0x6b, 0xc3, 0x24, 0x35,
  0x15, 0x2a, 0x00, 0xab, 0xd3, 0xd5, 0x8d, 0x9a, 0x87, 0x70, 0xd9, 0xa3, 0xe5, 0x2d, 0x5a, 0x36,
  0x28, 0xae, 0x3c, 0x9e, 0x03, 0x25
};
const uint8_t Expected_Hash[] =
{
  0x46, 0x50, 0x0b, 0x6a, 0xe1, 0xab, 0x40, 0xbd, 0xe0, 0x97, 0xef, 0x16, 0x8b, 0x0f, 0x31, 0x99,
  0x04, 0x9b, 0x55, 0x54, 0x5a, 0x15, 0x88, 0x79, 0x2d, 0x39, 0xd5, 0x94, 0xf4, 0x93, 0xdc, 0xa7
};

/* Computed data buffer */
uint8_t Computed_Hash[sizeof(Expected_Hash)];

__IO TestStatus glob_status = FAILED;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
void Error_Handler(void);
/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  psa_status_t retval;
  size_t computed_size;
  /* General operation */
  psa_hash_operation_t operation;
  /* Index for piecemeal processing */
  uint32_t index;

  /* STM32WBAxx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the System clock */
  SystemClock_Config();


  /* Configure LD3 */
  BSP_LED_Init(LD3);


  /* Init the PSA */
  retval = psa_crypto_init();
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* --------------------------------------------------------------------------
   * SINGLE CALL USAGE
   * --------------------------------------------------------------------------
   */
  /* Compute directly the digest passing all the needed parameters */
  retval = psa_hash_compute(PSA_ALG_SHA_256,
                            Message, sizeof(Message), /* Message to digest */
                            Computed_Hash,            /* Data buffer to receive digest data */
                            sizeof(Computed_Hash),    /* Size of hash buffer */
                            &computed_size);          /* Size of computed digest */

  /* Verify API returned value */
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Verify generated data size is the expected one */
  if (computed_size != sizeof(Expected_Hash))
  {
    Error_Handler();
  }

  /* Verify generated data are the expected ones */
  if (memcmp(Expected_Hash, Computed_Hash, computed_size) != 0)
  {
    Error_Handler();
  }

  /* --------------------------------------------------------------------------
   * MULTIPLE CALLS USAGE
   * --------------------------------------------------------------------------
   */

  /* Reset Computed_Hash value */
  (void) memset(Computed_Hash, 0, sizeof(Computed_Hash));

  operation = psa_hash_operation_init();

  /* Initialize the hash context */
  retval = psa_hash_setup(&operation, PSA_ALG_SHA_256);
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Process with the hashing of the message by chunk of CHUNK_SIZE Bytes */
  for (index = 0; index < (sizeof(Message) - CHUNK_SIZE); index += CHUNK_SIZE)
  {
    retval = psa_hash_update(&operation, &Message[index], CHUNK_SIZE); /* Chunk of data to digest */

    /* Verify API returned value */
    if (retval != PSA_SUCCESS)
    {
      Error_Handler();
    }
  }

  /* Process with hashing of the last part if needed */
  if (index < sizeof(Message))
  {
    /* Last part of data to digest */
    retval = psa_hash_update(&operation, &Message[index], sizeof(Message) - index);

    /* Verify API returned value */
    if (retval != PSA_SUCCESS)
    {
      Error_Handler();
    }
  }

  /* Generate the digest data */
  retval = psa_hash_finish(&operation,
                           Computed_Hash,            /* Computed hash */
                           sizeof(Computed_Hash),    /* Size of hash buffer */
                           &computed_size);          /* Size of computed digest */

  /* Verify API returned value */
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Verify generated data size is the expected one */
  if (computed_size != sizeof(Expected_Hash))
  {
    Error_Handler();
  }

  /* Verify generated data are the expected ones */
  if (memcmp(Expected_Hash, Computed_Hash, computed_size) != 0)
  {
    Error_Handler();
  }

  /* Clean up hash operation context */
  retval = psa_hash_abort(&operation);
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Clear all data associated with the PSA layer */
  mbedtls_psa_crypto_free();

  /* Turn on LD3 in an infinite loop in case of AES CBC operations are successful */
  BSP_LED_On(LD3);
  glob_status = PASSED;
  while (1)
  {}
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 100000000
  *            HCLK(Hz)                       = 100000000
  *            AHB Prescaler                  = 1
  *            AHB5 Prescaler                 = 4
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            APB7 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 32000000
  *            PLL_M                          = 2
  *            PLL_N                          = 25
  *            PLL_P                          = 4
  *            PLL_Q                          = 4
  *            PLL_R                          = 4
  *            Flash Latency(WS)              = 3
  *            Voltage range                  = 1
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate PLL with HSE as source
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEDiv = RCC_HSE_DIV1;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL1.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL1.PLLFractional = 0U;
  RCC_OscInitStruct.PLL1.PLLM = 2U;
  RCC_OscInitStruct.PLL1.PLLN = 8U;    /* VCO = HSE/M * N = 32 / 2 * 8 = 128 MHz */
  RCC_OscInitStruct.PLL1.PLLR = 2U;    /* PLLSYS = 128 MHz / 2 = 64 MHz */
  RCC_OscInitStruct.PLL1.PLLP = 2U;
  RCC_OscInitStruct.PLL1.PLLQ = 2U;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization error */
    Error_Handler();
  }

  /** Select PLL as system clock source and configure the HCLK, PCLK1, PCLK2 and PCLK7
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 \
                               | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK7 | RCC_CLOCKTYPE_HCLK5);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB7CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHB5_PLL1_CLKDivider = RCC_SYSCLK_PLL1_DIV4;
  RCC_ClkInitStruct.AHB5_HSEHSI_CLKDivider = RCC_SYSCLK_HSEHSI_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    /* Initialization error */
    Error_Handler();
  }

  /** Deactivate HSI
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization error */
    Error_Handler();
  }
}


/**
  * @brief  This function is executed in case of error occurrence
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* User may add here some code to deal with this error */
  /* Toggle LD3 @2Hz to notify error condition */
  while (1)
  {
    BSP_LED_Toggle(LD3);
    HAL_Delay(250);
  }
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif /* USE_FULL_ASSERT */
