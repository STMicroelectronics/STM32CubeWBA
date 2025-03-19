/**
  ******************************************************************************
  * @file    MbedTLS_HW_ALT/Encrypted_ITS_KeyImport/Src/main.c
  * @author  MCD Application Team
  * @brief   This example provides a short description of how to use
  *          PSA Encryptes ITS alternative implementation to store AES-CBC key
  *          in encrypted storage and use the encrypted key to perform AES-CBC
  *          Encryption and dycreption operation.
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
#include "psa_its_alt.h"

/* Global variables ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef enum
{
  FAILED = 0,
  PASSED = 1
} TestStatus;
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/

/* Define ITS encryption secret key ID
 * ITS_ENCRYPTION_SECRET_KEY_ID can be updated by user
 * in storage_interface files.
 */
#define ITS_ENCRYPTION_SECRET_KEY_ID_USER  ITS_ENCRYPTION_SECRET_KEY_ID

/* Define user AES-CBC key ID */
/* User key ID should be between PSA_KEY_ID_USER_MIN and PSA_KWE_KEY_ID_RSSE_MIN
 * Key IDs from PSA_KWE_KEY_ID_RSSE_MIN to PSA_KEY_ID_USER_MAX are reserved for
 * RSSE keys.
 */
#define PSA_AES_CBC_KEY_ID_USER  ((psa_key_id_t)0x1fff0001)

/* Private variables ---------------------------------------------------------*/

/* AES CBC to be stored using encrypted ITS  */
/** Extract from NIST Special Publication 800-38A
  * F.2.1 CBC-AES128.Encrypt
  * Key 2b7e151628aed2a6abf7158809cf4f3c
  * IV 000102030405060708090a0b0c0d0e0f
  * Block #1
  * Plaintext 6bc1bee22e409f96e93d7e117393172a
  * Input Block 6bc0bce12a459991e134741a7f9e1925
  * Output Block 7649abac8119b246cee98e9b12e9197d
  * Ciphertext 7649abac8119b246cee98e9b12e9197d
  * Block #2
  * Plaintext ae2d8a571e03ac9c9eb76fac45af8e51
  * Input Block d86421fb9f1a1eda505ee1375746972c
  * Output Block 5086cb9b507219ee95db113a917678b2
  * Ciphertext 5086cb9b507219ee95db113a917678b2
  * Block #3
  * Plaintext 30c81c46a35ce411e5fbc1191a0a52ef
  * Input Block 604ed7ddf32efdff7020d0238b7c2a5d
  * Output Block 73bed6b8e3c1743b7116e69e22229516
  * Ciphertext 73bed6b8e3c1743b7116e69e22229516
  * Block #4
  * Plaintext f69f2445df4f9b17ad2b417be66c3710
  * Input Block 8521f2fd3c8eef2cdc3da7e5c44ea206
  * Output Block 3ff1caa1681fac09120eca307586e1a7
  * Ciphertext 3ff1caa1681fac09120eca307586e1a7
  */
const uint8_t Aes_CBC_Key[] =
{
  0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};
const uint8_t IV_CBC[] =
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};
const uint8_t Plaintext_CBC[] =
{
  0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
  0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
  0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
  0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
};
const uint8_t Expected_Ciphertext_CBC[] =
{
  0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46, 0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d,
  0x50, 0x86, 0xcb, 0x9b, 0x50, 0x72, 0x19, 0xee, 0x95, 0xdb, 0x11, 0x3a, 0x91, 0x76, 0x78, 0xb2,
  0x73, 0xbe, 0xd6, 0xb8, 0xe3, 0xc1, 0x74, 0x3b, 0x71, 0x16, 0xe6, 0x9e, 0x22, 0x22, 0x95, 0x16,
  0x3f, 0xf1, 0xca, 0xa1, 0x68, 0x1f, 0xac, 0x09, 0x12, 0x0e, 0xca, 0x30, 0x75, 0x86, 0xe1, 0xa7
};

const uint8_t IV_And_Expected_Ciphertext_CBC[] =
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46, 0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d,
  0x50, 0x86, 0xcb, 0x9b, 0x50, 0x72, 0x19, 0xee, 0x95, 0xdb, 0x11, 0x3a, 0x91, 0x76, 0x78, 0xb2,
  0x73, 0xbe, 0xd6, 0xb8, 0xe3, 0xc1, 0x74, 0x3b, 0x71, 0x16, 0xe6, 0x9e, 0x22, 0x22, 0x95, 0x16,
  0x3f, 0xf1, 0xca, 0xa1, 0x68, 0x1f, 0xac, 0x09, 0x12, 0x0e, 0xca, 0x30, 0x75, 0x86, 0xe1, 0xa7
};

/* Computed data buffer */
uint8_t Computed_Ciphertext_CBC[sizeof(Expected_Ciphertext_CBC)+sizeof(IV_CBC)];
uint8_t Computed_Plaintext_CBC[sizeof(Plaintext_CBC)];

/* AES GCM key to be impoted as ITS encryption secret key */
/** Extract from NIST Special Publication 800-38D
  * gcmEncryptExtIV256.rsp
  * [Keylen = 256]
  * aes_key = 463b412911767d57a0b33969e674ffe7845d313b88c6fe312f3d724be68e1fca
  */
const uint8_t Aes_GCM_Key[] =
{
  0x46, 0x3b, 0x41, 0x29, 0x11, 0x76, 0x7d, 0x57, 0xa0, 0xb3, 0x39, 0x69, 0xe6, 0x74, 0xff, 0xe7,
  0x84, 0x5d, 0x31, 0x3b, 0x88, 0xc6, 0xfe, 0x31, 0x2f, 0x3d, 0x72, 0x4b, 0xe6, 0x8e, 0x1f, 0xca
};

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
  /* Define key attributes */
  psa_key_attributes_t key_attributes;
#if !defined(USE_HUK)
  psa_key_handle_t key_handle_aes_gcm_its;
#endif /* ! USE_HUK */
  psa_key_handle_t key_handle_aes_cbc;

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
   *                    PSA Crypto library Initialization
   * --------------------------------------------------------------------------
   */
  retval = psa_crypto_init();
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

#if !defined(USE_HUK)
  /* --------------------------------------------------------------------------
   *                      AES GCM Key to encrypt ITS storage
   *   PSA Persistent Private Key Import using PSA Key Import
   *   The imported key is stored in secret storage location defined by
   *   the user in storage_interface files.
   *   When USE_HUK is enabled, ITS storage is encrypted uing the device
   *   hardware unique key.
   * --------------------------------------------------------------------------
   */

  /* Setup the key policy for the private key */
  psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_DERIVE);
  psa_set_key_algorithm(&key_attributes, PSA_ALG_HKDF(PSA_ALG_SHA_256));
  psa_set_key_type(&key_attributes, PSA_KEY_TYPE_DERIVE);
  psa_set_key_bits(&key_attributes, 8U*sizeof(Aes_GCM_Key));
  /* Set up the key location using PSA_KEY_LOCATION_LOCAL_STORAGE */
  psa_set_key_lifetime(&key_attributes, PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_DEFAULT, PSA_KEY_LOCATION_LOCAL_STORAGE));
  /* Set up persistent Key ID */
  psa_set_key_id(&key_attributes, ITS_ENCRYPTION_SECRET_KEY_ID_USER);
  /* Import the private key */
  retval = psa_import_key(&key_attributes, Aes_GCM_Key, sizeof(Aes_GCM_Key), &key_handle_aes_gcm_its);
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Reset the key attribute */
  psa_reset_key_attributes(&key_attributes);
#endif /* ! USE_HUK */

  /* --------------------------------------------------------------------------
   *                      AES CBC Key
   *   PSA Persistent Private Key Import using PSA Key Import
   *   The imported key is encrypted and stored in ITS Persistent storage
   * --------------------------------------------------------------------------
   */

  /* Init the key attributes */
  key_attributes = psa_key_attributes_init();

  /* Setup the key policy for the private key */
  psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
  psa_set_key_algorithm(&key_attributes, PSA_ALG_CBC_NO_PADDING);
  psa_set_key_type(&key_attributes, PSA_KEY_TYPE_AES);
  psa_set_key_bits(&key_attributes, 8U*sizeof(Aes_CBC_Key));
  /* Set up the key location using PSA_KEY_LOCATION_LOCAL_STORAGE */
  psa_set_key_lifetime(&key_attributes, PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_DEFAULT, PSA_KEY_LOCATION_LOCAL_STORAGE));
  /* Set up persistent Key ID */
  psa_set_key_id(&key_attributes, PSA_AES_CBC_KEY_ID_USER);
  /* Import the private key */
  retval = psa_import_key(&key_attributes, Aes_CBC_Key, sizeof(Aes_CBC_Key), &key_handle_aes_cbc);
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Reset the key attribute */
  psa_reset_key_attributes(&key_attributes);

  /* Close the key to remove it from volatile memory */
  retval = psa_close_key(key_handle_aes_cbc);
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Open the key to load it from persistent storage to PSA slot
   * in vloatile memory, the key is decrypted during this operation
   */
  retval = psa_open_key(PSA_AES_CBC_KEY_ID_USER, &key_handle_aes_cbc);
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* --------------------------------------------------------------------------
   *              AES CBC Encryption and Decryption
   *   Use the encrypted key to encrypt and dycrypt text and check that the
   *   generated data are the expected ones
   * --------------------------------------------------------------------------
   */

  /* Compute directly the ciphertext passing all the needed parameters */
  /* This function use a random IV, data verification is not possible */
  retval = psa_cipher_encrypt(key_handle_aes_cbc,                      /* The key id */
                              PSA_ALG_CBC_NO_PADDING,                  /* Algorithm type */
                              Plaintext_CBC, sizeof(Plaintext_CBC),    /* Plaintext to encrypt */
                              Computed_Ciphertext_CBC,                 /* Data buffer to receive generated ciphertext */
                              sizeof(Computed_Ciphertext_CBC),         /* Size of buffer to receive ciphertext */
                              &computed_size);                         /* Size of computed ciphertext */

  /* Verify API returned value */
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Verify generated data size is the expected one. We can only verify the size of the data.*/
  if (computed_size != sizeof(IV_And_Expected_Ciphertext_CBC))
  {
    Error_Handler();
  }

  /* Decrypt directly ciphertext passing all the needed parameters   */
  retval= psa_cipher_decrypt(key_handle_aes_cbc,                      /* The key id */
                             PSA_ALG_CBC_NO_PADDING,                  /* Algorithm type */
                             Computed_Ciphertext_CBC,                 /* IV + Ciphertext to decrypt */
                             sizeof(IV_And_Expected_Ciphertext_CBC),  /* Size of IV + Ciphertext to decrypt */
                             Computed_Plaintext_CBC,                  /* Data buffer to receive generated plaintext */
                             sizeof(Computed_Plaintext_CBC),          /* Size of buffer to receive plaintext */
                             &computed_size);                         /* Size of computed plaintext */

  /* Verify API returned value */
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Verify generated data size is the expected one */
  if (computed_size != sizeof(Plaintext_CBC))
  {
    Error_Handler();
  }

  /* Verify generated data are the expected ones */
  if (memcmp(Plaintext_CBC, Computed_Plaintext_CBC, computed_size) != 0)
  {
    Error_Handler();
  }

  /* --------------------------------------------------------------------------
   * Destroy the PSA key and clear all data
   * --------------------------------------------------------------------------
  */

  /* Destroy the private keys */
#if !defined(USE_HUK)
  retval = psa_destroy_key(key_handle_aes_gcm_its);
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }
#endif /* ! USE_HUK */

  retval = psa_destroy_key(key_handle_aes_cbc);
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Clear all data associated with the PSA layer */
  mbedtls_psa_crypto_free();

  /* Turn on LD5 in an infinite loop in case of Key Wrap operations are successful */
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
