/**
  ******************************************************************************
  * @file    MbedTLS_HW_ALT/ECC_ECDH_SharedSecretGeneration_HAL/Src/main.c
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
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** Extract from KAS_ECC_CDH_PrimitiveTest.txt
  * [P-256]

COUNT = 0
QCAVSx = 700c48f77f56584c5cc632ca65640db91b6bacce3a4df6b42ce7cc838833d287
QCAVSy = db71e509e3fd9b060ddb20ba5c51dcc5948d46fbf640dfe0441782cab85fa4ac
dIUT = 7d7dc5f71eb29ddaf80d6214632eeae03d9058af1fb6d22ed80badb62bc1a534
QIUTx = ead218590119e8876b29146ff89ca61770c4edbbf97d38ce385ed281d8a6b230
QIUTy = 28af61281fd35e2fa7002523acc85a429cb06ee6648325389f59edfce1405141
ZIUT = 46fc62106420ff012e54a434fbdd2d25ccc5852060561e68040dd7778997bd7b

  */
const uint8_t Private_Key[] =
{
  0x7d, 0x7d, 0xc5, 0xf7, 0x1e, 0xb2, 0x9d, 0xda, 0xf8, 0x0d, 0x62, 0x14, 0x63, 0x2e, 0xea, 0xe0,
  0x3d, 0x90, 0x58, 0xaf, 0x1f, 0xb6, 0xd2, 0x2e, 0xd8, 0x0b, 0xad, 0xb6, 0x2b, 0xc1, 0xa5, 0x34
};
const uint8_t Remote_Public_Key[] =
{
  0x04, /* This octet is for a Weierstrass public key representation */
  0x70, 0x0c, 0x48, 0xf7, 0x7f, 0x56, 0x58, 0x4c, 0x5c, 0xc6, 0x32, 0xca, 0x65, 0x64, 0x0d, 0xb9,
  0x1b, 0x6b, 0xac, 0xce, 0x3a, 0x4d, 0xf6, 0xb4, 0x2c, 0xe7, 0xcc, 0x83, 0x88, 0x33, 0xd2, 0x87,
  0xdb, 0x71, 0xe5, 0x09, 0xe3, 0xfd, 0x9b, 0x06, 0x0d, 0xdb, 0x20, 0xba, 0x5c, 0x51, 0xdc, 0xc5,
  0x94, 0x8d, 0x46, 0xfb, 0xf6, 0x40, 0xdf, 0xe0, 0x44, 0x17, 0x82, 0xca, 0xb8, 0x5f, 0xa4, 0xac
};
const uint8_t Expected_SecretX[] =
{
  0x46, 0xfc, 0x62, 0x10, 0x64, 0x20, 0xff, 0x01, 0x2e, 0x54, 0xa4, 0x34, 0xfb, 0xdd, 0x2d, 0x25,
  0xcc, 0xc5, 0x85, 0x20, 0x60, 0x56, 0x1e, 0x68, 0x04, 0x0d, 0xd7, 0x77, 0x89, 0x97, 0xbd, 0x7b
};

/* Computed data buffer */
uint8_t Computed_Secret[sizeof(Expected_SecretX)];

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
  /* Key definition */
  psa_key_attributes_t key_attributes;
  psa_key_handle_t key_handle_private;      /* Private key */
  psa_key_handle_t key_handle_public;       /* Public key */

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
  /* Setup the key policy for private key */
  psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_DERIVE);
  psa_set_key_algorithm(&key_attributes, PSA_ALG_ECDH);
  psa_set_key_type(&key_attributes, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));

  /* Import a private key */
  retval = psa_import_key(&key_attributes, Private_Key, sizeof(Private_Key), &key_handle_private);
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Reset the key attribute */
  psa_reset_key_attributes(&key_attributes);

  /* Setup the key policy for public key */
  psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_DERIVE);
  psa_set_key_algorithm(&key_attributes, PSA_ALG_ECDH);
  psa_set_key_type(&key_attributes, PSA_KEY_TYPE_DERIVE);

  /* Import a public key */
  retval = psa_import_key(&key_attributes, Remote_Public_Key, sizeof(Remote_Public_Key), &key_handle_public);
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Reset the key attribute */
  psa_reset_key_attributes(&key_attributes);

  /* --------------------------------------------------------------------------
   * Diffie-Hellman
   * --------------------------------------------------------------------------
  */

  /* Derive a key */
  retval = psa_raw_key_agreement(PSA_ALG_ECDH,
                                 key_handle_private,        /* Private key to use to compute the secret */
                                 Remote_Public_Key,         /* Public key to use to compute the secret */
                                 sizeof(Remote_Public_Key), /* Size of public key */
                                 Computed_Secret,           /* Data buffer to receive shared secret */
                                 sizeof(Computed_Secret),   /* Size of buffer to receive shared secret */
                                 &computed_size);           /* Size of computed secret */

  /* Verify API returned value */
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Verify generated data size is the expected one */
  if (computed_size != sizeof(Computed_Secret))
  {
    Error_Handler();
  }

  /* Verify generated data are the expected ones */
  if (memcmp(Computed_Secret, Expected_SecretX, sizeof(Expected_SecretX)) != 0)
  {
    Error_Handler();
  }

  /* --------------------------------------------------------------------------
   * Destroy the PSA keys and clear all data
   * --------------------------------------------------------------------------
  */
  /* Destroy the public key */
  retval = psa_destroy_key(key_handle_public);
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Destroy the private key */
  retval = psa_destroy_key(key_handle_private);
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Clear all data associated with the PSA layer */
  mbedtls_psa_crypto_free();

  /* Turn on LD5 in an infinite loop in case of ECC ECDH operations are successful */
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
