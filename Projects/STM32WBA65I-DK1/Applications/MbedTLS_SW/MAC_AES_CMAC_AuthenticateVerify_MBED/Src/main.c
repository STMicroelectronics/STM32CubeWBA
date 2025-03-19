/**
  ******************************************************************************
  * @file    MbedTLS_SW/MAC_AES_CMAC_AuthenticateVerify_MBED/Src/main.c
  * @author  MCD Application Team
  * @brief   This example provides a short description of how to use the
  *          mbeb-crypto with no alt function.
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
/** Extract from NIST Special Publication 800-38A
  * Example #4

Key is
 2B7E1516 28AED2A6 ABF71588 09CF4F3C
Mlen=64

PT is
6BC1BEE2 2E409F96 E93D7E11 7393172A
AE2D8A57 1E03AC9C 9EB76FAC 45AF8E51
30C81C46 A35CE411 E5FBC119 1A0A52EF
F69F2445 DF4F9B17 AD2B417B E66C3710

...

Tag is
51F0BEBF 7E3B9D92 FC497417 79363CFE
  */
const uint8_t Key[] =
{
  0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
};
const uint8_t Message[] =
{
  0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96, 0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A,
  0xAE, 0x2D, 0x8A, 0x57, 0x1E, 0x03, 0xAC, 0x9C, 0x9E, 0xB7, 0x6F, 0xAC, 0x45, 0xAF, 0x8E, 0x51,
  0x30, 0xC8, 0x1C, 0x46, 0xA3, 0x5C, 0xE4, 0x11, 0xE5, 0xFB, 0xC1, 0x19, 0x1A, 0x0A, 0x52, 0xEF,
  0xF6, 0x9F, 0x24, 0x45, 0xDF, 0x4F, 0x9B, 0x17, 0xAD, 0x2B, 0x41, 0x7B, 0xE6, 0x6C, 0x37, 0x10
};
const uint8_t Expected_Tag[] =
{
  0x51, 0xF0, 0xBE, 0xBF, 0x7E, 0x3B, 0x9D, 0x92, 0xFC, 0x49, 0x74, 0x17, 0x79, 0x36, 0x3C, 0xFE
};

/* Computed data buffer */
uint8_t Computed_Tag[sizeof(Expected_Tag)];

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
  psa_mac_operation_t operation;
  /* Key attributes */
  psa_key_attributes_t key_attributes;
  psa_key_handle_t key_handle;
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


  /* Configure LD5 */
  BSP_LED_Init(LD5);

  /* --------------------------------------------------------------------------
   * Create the PSA key
   * --------------------------------------------------------------------------
  */

  /* Init the key attributes */
  key_attributes = psa_key_attributes_init();

  /* Init the PSA */
  retval = psa_crypto_init();
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Setup the key policy */
  psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH);
  psa_set_key_algorithm(&key_attributes, PSA_ALG_CMAC);
  psa_set_key_type(&key_attributes, PSA_KEY_TYPE_AES);
  psa_set_key_bits(&key_attributes, 8U*sizeof(Key));

  /* Import a key */
  retval = psa_import_key(&key_attributes, Key, sizeof(Key), &key_handle);
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Reset the key attribute */
  psa_reset_key_attributes(&key_attributes);

  /* --------------------------------------------------------------------------
   * SINGLE CALL USAGE
   * --------------------------------------------------------------------------
   */

  /* Compute directly the authentication tag passing all the needed parameters */
  retval = psa_mac_compute(key_handle,               /* The key id */
                           PSA_ALG_CMAC,             /* Algorithm type */
                           Message, sizeof(Message), /* Message to authenticate */
                           Computed_Tag,             /* Data buffer to receive generated authentication tag */
                           sizeof(Computed_Tag),     /* Size of buffer to receive authentication tag */
                           &computed_size);          /* Size of computed authentication tag */

  /* Verify API returned value */
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Verify generated data size is the expected one */
  if (computed_size != sizeof(Expected_Tag))
  {
    Error_Handler();
  }

  /* Verify generated data are the expected ones */
  if (memcmp(Expected_Tag, Computed_Tag, sizeof(Expected_Tag)) != 0)
  {
    Error_Handler();
  }

  /* Verify directly the message passing all the needed parameters */
  retval = psa_mac_verify(key_handle,                /* The key id */
                          PSA_ALG_CMAC,              /* Algorithm type */
                          Message, sizeof(Message),  /* Message to authenticate */
                          Expected_Tag,              /* Authentication tag */
                          sizeof(Expected_Tag));     /* tag size */

  /* Verify API returned value */
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* --------------------------------------------------------------------------
   * MULTIPLE CALLS USAGE
   * --------------------------------------------------------------------------
   */

  operation = psa_mac_operation_init();

  /* Setup the context for sign operation */
  retval = psa_mac_sign_setup(&operation,           /* The current operation */
                              key_handle,           /* The key id */
                              PSA_ALG_CMAC);        /* Algorithm type */

  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Process with the authentication of the message by chunk of CHUNK_SIZE Bytes */
  for (index = 0; index < (sizeof(Message) - CHUNK_SIZE); index += CHUNK_SIZE)
  {
    retval = psa_mac_update(&operation, &Message[index], CHUNK_SIZE); /* Chunk of data to authenticate */

    /* Verify API returned value */
    if (retval != PSA_SUCCESS)
    {
      Error_Handler();
    }
  }
  /* Process with the authentication of the last part if needed */
  if (index < sizeof(Message))
  {
    retval = psa_mac_update(&operation, &Message[index], sizeof(Message) - index); /* Last part of data to authen */

    /* Verify API returned value */
    if (retval != PSA_SUCCESS)
    {
      Error_Handler();
    }
  }

  /* Generate the authentication tag */
  retval = psa_mac_sign_finish(&operation,
                               Computed_Tag,          /* Data buffer to receive the computed tag */
                               sizeof(Computed_Tag),  /* Size of buffer to receive the computed tag */
                               &computed_size);       /* Size of computed tag */

  /* Verify API returned value */
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Verify generated data size is the expected one */
  if (computed_size != sizeof(Expected_Tag))
  {
    Error_Handler();
  }

  /* Verify generated data are the expected ones */
  if (memcmp(Expected_Tag, Computed_Tag, computed_size) != 0)
  {
    Error_Handler();
  }

  /* Free resources */
  retval = psa_mac_abort(&operation);
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }


  /* Setup the context for verify operation */
  retval = psa_mac_verify_setup(&operation,           /* The current operation */
                                key_handle,           /* The key id */
                                PSA_ALG_CMAC);        /* Algorithm type */

  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Process with the authentication of the message by chunk of CHUNK_SIZE Bytes */
  for (index = 0; index < (sizeof(Message) - CHUNK_SIZE); index += CHUNK_SIZE)
  {
    retval = psa_mac_update(&operation, &Message[index], CHUNK_SIZE); /* Chunk of data to authenticate */

    /* Verify API returned value */
    if (retval != PSA_SUCCESS)
    {
      Error_Handler();
    }
  }
  /* Process with the authentication of the last part if needed */
  if (index < sizeof(Message))
  {
    retval = psa_mac_update(&operation, &Message[index], sizeof(Message) - index);

    /* Verify API returned value */
    if (retval != PSA_SUCCESS)
    {
      Error_Handler();
    }
  }

  /* Verify authentication tag */
  retval = psa_mac_verify_finish(&operation, Expected_Tag, sizeof(Expected_Tag));

  /* Verify API returned value */
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Free resources */
  retval = psa_mac_abort(&operation);
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* --------------------------------------------------------------------------
   * Destroy the PSA key and clear all data
   * --------------------------------------------------------------------------
  */
  /* Destroy the key */
  retval = psa_destroy_key(key_handle);
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Clear all data associated with the PSA layer */
  mbedtls_psa_crypto_free();

  /* Turn on LD5 in an infinite loop in case of AES CMAC operations are successful */
  BSP_LED_On(LD5);
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
  /** At reset, the regulator is the LDO, in range 2
     Need to move to range 1 to reach 100 MHz
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    /* Initialization error */
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
  RCC_OscInitStruct.PLL1.PLLN = 25U;   /* VCO = HSE/M * N = 32 / 2 * 25 = 400 MHz */
  RCC_OscInitStruct.PLL1.PLLR = 4U;    /* PLLSYS = 400 MHz / 4 = 100 MHz */
  RCC_OscInitStruct.PLL1.PLLP = 4U;
  RCC_OscInitStruct.PLL1.PLLQ = 4U;
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
  /* Toggle LD5 @2Hz to notify error condition */
  while (1)
  {
    BSP_LED_Toggle(LD5);
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
