/**
  ******************************************************************************
  * @file    MbedTLS_HW_ALT/RSA_PKCS1v1.5_SignVerifyCRT_HAL/Src/main.c
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
/** Extract from psa_arch_tests\api-tests\dev_apis\crypto\common\test_crypto_common.c

# Public key
# ----------

# Modulus:
af 05 7d 39 6e e8 4f b7 5f db b5 c2 b1 3c 7f e5
a6 54 aa 8a a2 47 0b 54 1e e1 fe b0 b1 2d 25 c7
97 11 53 12 49 e1 12 96 28 04 2d bb b6 c1 20 d1
44 35 24 ef 4c 0e 6e 1d 89 56 ee b2 07 7a f1 23
49 dd ee e5 44 83 bc 06 c2 c6 19 48 cd 02 b2 02
e7 96 ae bd 94 d3 a7 cb f8 59 c2 c1 81 9c 32 4c
b8 2b 9c d3 4e de 26 3a 2a bf fe 47 33 f0 77 86
9e 86 60 f7 d6 83 4d a5 3d 69 0e f7 98 5f 6b c3

# Exponent:
01 00 01

# Private key
# -----------

# Modulus:
af 05 7d 39 6e e8 4f b7 5f db b5 c2 b1 3c 7f e5
a6 54 aa 8a a2 47 0b 54 1e e1 fe b0 b1 2d 25 c7
97 11 53 12 49 e1 12 96 28 04 2d bb b6 c1 20 d1
44 35 24 ef 4c 0e 6e 1d 89 56 ee b2 07 7a f1 23
49 dd ee e5 44 83 bc 06 c2 c6 19 48 cd 02 b2 02
e7 96 ae bd 94 d3 a7 cb f8 59 c2 c1 81 9c 32 4c
b8 2b 9c d3 4e de 26 3a 2a bf fe 47 33 f0 77 86
9e 86 60 f7 d6 83 4d a5 3d 69 0e f7 98 5f 6b c3

# Public exponent:
01 00 01

# Prime1
e2 ab 76 08 41 bb 9d 30 a8 1d 22 2d e1 eb 73 81
d8 22 14 40 7f 1b 97 5c bb fe 4e 1a 94 67 fd 98
ad bd 78 f6 07 83 6c a5 be 19 28 b9 d1 60 d9 7f
d4 5c 12 d6 b5 2e 2c 98 71 a1 74 c6 6b 48 81 13

# Prime2
c5 ab 27 60 21 59 ae 7d 6f 20 c3 c2 ee 85 1e 46
dc 11 2e 68 9e 28 d5 fc bb f9 90 a9 9e f8 a9 0b
8b b4 4f d3 64 67 e7 fc 17 89 ce b6 63 ab da 33
86 52 c3 c7 3f 11 17 74 90 2e 84 05 65 92 70 91

# Exponent1
b6 cd bd 35 4f 7d f5 79 a6 3b 48 b3 64 3e 35 3b
84 89 87 77 b4 8b 15 f9 4e 0b fc 05 67 a6 ae 59
11 d5 7a d6 40 9c f7 64 7b f9 62 64 e9 bd 87 eb
95 e2 63 b7 11 0b 9a 1f 9f 94 ac ce d0 fa fa 4d

# Exponent2
71 19 5e ec 37 e8 d2 57 de cf c6 72 b0 7a e6 39
f1 0c bb 9b 0c 73 9d 0c 80 99 68 d6 44 a9 4e 3f
d6 ed 92 87 07 7a 14 58 3f 37 90 58 f7 6a 8a ec
d4 3c 62 dc 8c 0f 41 76 66 50 d7 25 27 5a c4 a1

# Coefficient
bb 32 d1 33 ed c2 e0 48 d4 63 38 8b 7b e9 cb 4b
e2 9f 4b 62 50 be 60 3e 70 e3 64 75 01 c9 7d dd
e2 0a 4e 71 be 95 fd 5e 71 78 4e 25 ac a4 ba f2
5b e5 73 8a ae 59 bb fe 1c 99 77 81 44 7a 2b 24

# PKCS#1 v1.5 Signature Example

# -----------------

# Message to be signed:
BD

# Signature:
62 F7 CB DC 60 2B 9F C2 7D 96 4F D1 B3 59 52 68
CB 51 EF DF 05 DB B8 3D 7B 87 3C 08 2D 4A 93 A1
D3 7B A5 4F 42 46 85 9E 16 BD 9B F4 AF 97 68 62
F4 EC 7F 72 2B 4F 95 EB 7D A5 F5 E2 0E 7B 43 93
6F FD 7E FC 5D 74 88 2B EA 02 64 0D 12 76 98 43
E7 D5 FF 94 81 AD 58 87 13 B4 EE 98 41 A4 DE 30
84 C1 B1 04 85 5B CA 86 A4 F3 64 E5 20 21 1B E0
55 40 24 74 95 2A 34 F1 C2 79 6A 6D 3C AB BD A0

  */
const uint8_t Message[] =
{
 0xbd
};

const uint8_t Known_Signature[] =
{
 0x62, 0xF7, 0xCB, 0xDC, 0x60, 0x2B, 0x9F, 0xC2, 0x7D, 0x96, 0x4F, 0xD1, 0xB3, 0x59, 0x52, 0x68,
 0xCB, 0x51, 0xEF, 0xDF, 0x05, 0xDB, 0xB8, 0x3D, 0x7B, 0x87, 0x3C, 0x08, 0x2D, 0x4A, 0x93, 0xA1,
 0xD3, 0x7B, 0xA5, 0x4F, 0x42, 0x46, 0x85, 0x9E, 0x16, 0xBD, 0x9B, 0xF4, 0xAF, 0x97, 0x68, 0x62,
 0xF4, 0xEC, 0x7F, 0x72, 0x2B, 0x4F, 0x95, 0xEB, 0x7D, 0xA5, 0xF5, 0xE2, 0x0E, 0x7B, 0x43, 0x93,
 0x6F, 0xFD, 0x7E, 0xFC, 0x5D, 0x74, 0x88, 0x2B, 0xEA, 0x02, 0x64, 0x0D, 0x12, 0x76, 0x98, 0x43,
 0xE7, 0xD5, 0xFF, 0x94, 0x81, 0xAD, 0x58, 0x87, 0x13, 0xB4, 0xEE, 0x98, 0x41, 0xA4, 0xDE, 0x30,
 0x84, 0xC1, 0xB1, 0x04, 0x85, 0x5B, 0xCA, 0x86, 0xA4, 0xF3, 0x64, 0xE5, 0x20, 0x21, 0x1B, 0xE0,
 0x55, 0x40, 0x24, 0x74, 0x95, 0x2A, 0x34, 0xF1, 0xC2, 0x79, 0x6A, 0x6D, 0x3C, 0xAB, 0xBD, 0xA0
};

const uint8_t rsa_128_key_pair[] =
{
0x30, 0x82, 0x02, 0x5e, 0x02, 0x01, 0x00, 0x02, 0x81, 0x81, 0x00, 0xaf, 0x05,
0x7d, 0x39, 0x6e, 0xe8, 0x4f, 0xb7, 0x5f, 0xdb, 0xb5, 0xc2, 0xb1, 0x3c, 0x7f,
0xe5, 0xa6, 0x54, 0xaa, 0x8a, 0xa2, 0x47, 0x0b, 0x54, 0x1e, 0xe1, 0xfe, 0xb0,
0xb1, 0x2d, 0x25, 0xc7, 0x97, 0x11, 0x53, 0x12, 0x49, 0xe1, 0x12, 0x96, 0x28,
0x04, 0x2d, 0xbb, 0xb6, 0xc1, 0x20, 0xd1, 0x44, 0x35, 0x24, 0xef, 0x4c, 0x0e,
0x6e, 0x1d, 0x89, 0x56, 0xee, 0xb2, 0x07, 0x7a, 0xf1, 0x23, 0x49, 0xdd, 0xee,
0xe5, 0x44, 0x83, 0xbc, 0x06, 0xc2, 0xc6, 0x19, 0x48, 0xcd, 0x02, 0xb2, 0x02,
0xe7, 0x96, 0xae, 0xbd, 0x94, 0xd3, 0xa7, 0xcb, 0xf8, 0x59, 0xc2, 0xc1, 0x81,
0x9c, 0x32, 0x4c, 0xb8, 0x2b, 0x9c, 0xd3, 0x4e, 0xde, 0x26, 0x3a, 0x2a, 0xbf,
0xfe, 0x47, 0x33, 0xf0, 0x77, 0x86, 0x9e, 0x86, 0x60, 0xf7, 0xd6, 0x83, 0x4d,
0xa5, 0x3d, 0x69, 0x0e, 0xf7, 0x98, 0x5f, 0x6b, 0xc3, 0x02, 0x03, 0x01, 0x00,
0x01, 0x02, 0x81, 0x81, 0x00, 0x87, 0x4b, 0xf0, 0xff, 0xc2, 0xf2, 0xa7, 0x1d,
0x14, 0x67, 0x1d, 0xdd, 0x01, 0x71, 0xc9, 0x54, 0xd7, 0xfd, 0xbf, 0x50, 0x28,
0x1e, 0x4f, 0x6d, 0x99, 0xea, 0x0e, 0x1e, 0xbc, 0xf8, 0x2f, 0xaa, 0x58, 0xe7,
0xb5, 0x95, 0xff, 0xb2, 0x93, 0xd1, 0xab, 0xe1, 0x7f, 0x11, 0x0b, 0x37, 0xc4,
0x8c, 0xc0, 0xf3, 0x6c, 0x37, 0xe8, 0x4d, 0x87, 0x66, 0x21, 0xd3, 0x27, 0xf6,
0x4b, 0xbe, 0x08, 0x45, 0x7d, 0x3e, 0xc4, 0x09, 0x8b, 0xa2, 0xfa, 0x0a, 0x31,
0x9f, 0xba, 0x41, 0x1c, 0x28, 0x41, 0xed, 0x7b, 0xe8, 0x31, 0x96, 0xa8, 0xcd,
0xf9, 0xda, 0xa5, 0xd0, 0x06, 0x94, 0xbc, 0x33, 0x5f, 0xc4, 0xc3, 0x22, 0x17,
0xfe, 0x04, 0x88, 0xbc, 0xe9, 0xcb, 0x72, 0x02, 0xe5, 0x94, 0x68, 0xb1, 0xea,
0xd1, 0x19, 0x00, 0x04, 0x77, 0xdb, 0x2c, 0xa7, 0x97, 0xfa, 0xc1, 0x9e, 0xda,
0x3f, 0x58, 0xc1, 0x02, 0x41, 0x00, 0xe2, 0xab, 0x76, 0x08, 0x41, 0xbb, 0x9d,
0x30, 0xa8, 0x1d, 0x22, 0x2d, 0xe1, 0xeb, 0x73, 0x81, 0xd8, 0x22, 0x14, 0x40,
0x7f, 0x1b, 0x97, 0x5c, 0xbb, 0xfe, 0x4e, 0x1a, 0x94, 0x67, 0xfd, 0x98, 0xad,
0xbd, 0x78, 0xf6, 0x07, 0x83, 0x6c, 0xa5, 0xbe, 0x19, 0x28, 0xb9, 0xd1, 0x60,
0xd9, 0x7f, 0xd4, 0x5c, 0x12, 0xd6, 0xb5, 0x2e, 0x2c, 0x98, 0x71, 0xa1, 0x74,
0xc6, 0x6b, 0x48, 0x81, 0x13, 0x02, 0x41, 0x00, 0xc5, 0xab, 0x27, 0x60, 0x21,
0x59, 0xae, 0x7d, 0x6f, 0x20, 0xc3, 0xc2, 0xee, 0x85, 0x1e, 0x46, 0xdc, 0x11,
0x2e, 0x68, 0x9e, 0x28, 0xd5, 0xfc, 0xbb, 0xf9, 0x90, 0xa9, 0x9e, 0xf8, 0xa9,
0x0b, 0x8b, 0xb4, 0x4f, 0xd3, 0x64, 0x67, 0xe7, 0xfc, 0x17, 0x89, 0xce, 0xb6,
0x63, 0xab, 0xda, 0x33, 0x86, 0x52, 0xc3, 0xc7, 0x3f, 0x11, 0x17, 0x74, 0x90,
0x2e, 0x84, 0x05, 0x65, 0x92, 0x70, 0x91, 0x02, 0x41, 0x00, 0xb6, 0xcd, 0xbd,
0x35, 0x4f, 0x7d, 0xf5, 0x79, 0xa6, 0x3b, 0x48, 0xb3, 0x64, 0x3e, 0x35, 0x3b,
0x84, 0x89, 0x87, 0x77, 0xb4, 0x8b, 0x15, 0xf9, 0x4e, 0x0b, 0xfc, 0x05, 0x67,
0xa6, 0xae, 0x59, 0x11, 0xd5, 0x7a, 0xd6, 0x40, 0x9c, 0xf7, 0x64, 0x7b, 0xf9,
0x62, 0x64, 0xe9, 0xbd, 0x87, 0xeb, 0x95, 0xe2, 0x63, 0xb7, 0x11, 0x0b, 0x9a,
0x1f, 0x9f, 0x94, 0xac, 0xce, 0xd0, 0xfa, 0xfa, 0x4d, 0x02, 0x40, 0x71, 0x19,
0x5e, 0xec, 0x37, 0xe8, 0xd2, 0x57, 0xde, 0xcf, 0xc6, 0x72, 0xb0, 0x7a, 0xe6,
0x39, 0xf1, 0x0c, 0xbb, 0x9b, 0x0c, 0x73, 0x9d, 0x0c, 0x80, 0x99, 0x68, 0xd6,
0x44, 0xa9, 0x4e, 0x3f, 0xd6, 0xed, 0x92, 0x87, 0x07, 0x7a, 0x14, 0x58, 0x3f,
0x37, 0x90, 0x58, 0xf7, 0x6a, 0x8a, 0xec, 0xd4, 0x3c, 0x62, 0xdc, 0x8c, 0x0f,
0x41, 0x76, 0x66, 0x50, 0xd7, 0x25, 0x27, 0x5a, 0xc4, 0xa1, 0x02, 0x41, 0x00,
0xbb, 0x32, 0xd1, 0x33, 0xed, 0xc2, 0xe0, 0x48, 0xd4, 0x63, 0x38, 0x8b, 0x7b,
0xe9, 0xcb, 0x4b, 0xe2, 0x9f, 0x4b, 0x62, 0x50, 0xbe, 0x60, 0x3e, 0x70, 0xe3,
0x64, 0x75, 0x01, 0xc9, 0x7d, 0xdd, 0xe2, 0x0a, 0x4e, 0x71, 0xbe, 0x95, 0xfd,
0x5e, 0x71, 0x78, 0x4e, 0x25, 0xac, 0xa4, 0xba, 0xf2, 0x5b, 0xe5, 0x73, 0x8a,
0xae, 0x59, 0xbb, 0xfe, 0x1c, 0x99, 0x77, 0x81, 0x44, 0x7a, 0x2b, 0x24
};

/* Computed data buffer */
uint8_t Computed_Signature[sizeof(Known_Signature)];

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
  psa_key_handle_t key_handle;

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

  /* Setup the key policy for the key pair */
  psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE);
  psa_set_key_algorithm(&key_attributes, PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256));
  psa_set_key_type(&key_attributes, PSA_KEY_TYPE_RSA_KEY_PAIR);
  psa_set_key_bits(&key_attributes, 1024); /* Key length = modulus size */

  /* Import a key pair */
  retval = psa_import_key(&key_attributes, rsa_128_key_pair, sizeof(rsa_128_key_pair), &key_handle);
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Reset the key attribute */
  psa_reset_key_attributes(&key_attributes);


  /* --------------------------------------------------------------------------
   * USING CHINESE REMAINDER THEOREM (CRT) PRIVATE KEY REPRESENTATION WITH FACM
   * --------------------------------------------------------------------------
   */

  /* Compute directly the signature passing all the needed parameters */
  retval = psa_sign_message(key_handle,
                            PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256),     /* Algorithm type */
                            Message, sizeof(Message),                       /* Message to sign */
                            Computed_Signature, sizeof(Computed_Signature), /*Data buffer to receive signature */
                            &computed_size);                                /* Size of computed signature */

  /* Verify API returned value */
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Verify generated data size is the expected one */
  if (computed_size != sizeof(Known_Signature))
  {
    Error_Handler();
  }

  /* Verify generated data are the expected ones */
  if (memcmp(Computed_Signature, Known_Signature, computed_size) != 0)
  {
    Error_Handler();
  }


  /* Check the signature passing all the needed parameters */
  retval = psa_verify_message(key_handle,
                              PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256),   /* Algorithm type */
                              Message, sizeof(Message),                     /* Message to sign */
                              Known_Signature, sizeof(Known_Signature));    /* Signature to verify */


  /* Verify API returned value */
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

  /* Turn on LD5 in an infinite loop in case of RSA operations are successful */
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
