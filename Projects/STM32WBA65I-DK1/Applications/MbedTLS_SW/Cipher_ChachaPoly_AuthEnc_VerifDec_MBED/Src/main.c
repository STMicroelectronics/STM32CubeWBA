/**
  ******************************************************************************
  * @file    MbedTLS_SW/Cipher_ChachaPoly_AuthEnc_VerifDec_MBED/Src/main.c
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
#define CHUNK_SIZE  64u   /* Chacha-Poly1305 chunks must be multiple of 64 bytes */
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** Extract from IETF
  * A.5.  ChaCha20-Poly1305 AEAD Decryption

   Below we see decrypting a message.  We receive a ciphertext, a nonce,
   and a tag.  We know the key.  We will check the tag and then
   (assuming that it validates) decrypt the ciphertext.  In this
   particular protocol, we'll assume that there is no padding of the
   plaintext.

  The ChaCha20 Key
  000  1c 92 40 a5 eb 55 d3 8a f3 33 88 86 04 f6 b5 f0  ..@..U...3......
  016  47 39 17 c1 40 2b 80 09 9d ca 5c bc 20 70 75 c0  G9..@+....\. pu.

  Ciphertext:
  000  64 a0 86 15 75 86 1a f4 60 f0 62 c7 9b e6 43 bd  d...u...`.b...C.
  016  5e 80 5c fd 34 5c f3 89 f1 08 67 0a c7 6c 8c b2  ^.\.4\....g..l..
  032  4c 6c fc 18 75 5d 43 ee a0 9e e9 4e 38 2d 26 b0  Ll..u]C....N8-&.
  048  bd b7 b7 3c 32 1b 01 00 d4 f0 3b 7f 35 58 94 cf  ...<2.....;.5X..
  064  33 2f 83 0e 71 0b 97 ce 98 c8 a8 4a bd 0b 94 81  3/..q......J....
  080  14 ad 17 6e 00 8d 33 bd 60 f9 82 b1 ff 37 c8 55  ...n..3.`....7.U
  096  97 97 a0 6e f4 f0 ef 61 c1 86 32 4e 2b 35 06 38  ...n...a..2N+5.8
  112  36 06 90 7b 6a 7c 02 b0 f9 f6 15 7b 53 c8 67 e4  6..{j|.....{S.g.
  128  b9 16 6c 76 7b 80 4d 46 a5 9b 52 16 cd e7 a4 e9  ..lv{.MF..R.....
  144  90 40 c5 a4 04 33 22 5e e2 82 a1 b0 a0 6c 52 3e  .@...3"^.....lR>
  160  af 45 34 d7 f8 3f a1 15 5b 00 47 71 8c bc 54 6a  .E4..?..[.Gq..Tj
  176  0d 07 2b 04 b3 56 4e ea 1b 42 22 73 f5 48 27 1a  ..+..VN..B"s.H'.
  192  0b b2 31 60 53 fa 76 99 19 55 eb d6 31 59 43 4e  ..1`S.v..U..1YCN
  208  ce bb 4e 46 6d ae 5a 10 73 a6 72 76 27 09 7a 10  ..NFm.Z.s.rv'.z.
  224  49 e6 17 d9 1d 36 10 94 fa 68 f0 ff 77 98 71 30  I....6...h..w.q0
  240  30 5b ea ba 2e da 04 df 99 7b 71 4d 6c 6f 2c 29  0[.......{qMlo,)
  256  a6 ad 5c b4 02 2b 02 70 9b                       ..\..+.p.

  The nonce:
  000  00 00 00 00 01 02 03 04 05 06 07 08              ............

  The AAD:
  000  f3 33 88 86 00 00 00 00 00 00 4e 91              .3........N.

  Received Tag:
  000  ee ad 9d 67 89 0c bb 22 39 23 36 fe a1 85 1f 38  ...g..."9#6....8

  ...

  Plaintext::
  000  49 6e 74 65 72 6e 65 74 2d 44 72 61 66 74 73 20  Internet-Drafts
  016  61 72 65 20 64 72 61 66 74 20 64 6f 63 75 6d 65  are draft docume
  032  6e 74 73 20 76 61 6c 69 64 20 66 6f 72 20 61 20  nts valid for a
  048  6d 61 78 69 6d 75 6d 20 6f 66 20 73 69 78 20 6d  maximum of six m
  064  6f 6e 74 68 73 20 61 6e 64 20 6d 61 79 20 62 65  onths and may be
  080  20 75 70 64 61 74 65 64 2c 20 72 65 70 6c 61 63   updated, replac
  096  65 64 2c 20 6f 72 20 6f 62 73 6f 6c 65 74 65 64  ed, or obsoleted
  112  20 62 79 20 6f 74 68 65 72 20 64 6f 63 75 6d 65   by other docume
  128  6e 74 73 20 61 74 20 61 6e 79 20 74 69 6d 65 2e  nts at any time.
  144  20 49 74 20 69 73 20 69 6e 61 70 70 72 6f 70 72   It is inappropr
  160  69 61 74 65 20 74 6f 20 75 73 65 20 49 6e 74 65  iate to use Inte
  176  72 6e 65 74 2d 44 72 61 66 74 73 20 61 73 20 72  rnet-Drafts as r
  192  65 66 65 72 65 6e 63 65 20 6d 61 74 65 72 69 61  eference materia
  208  6c 20 6f 72 20 74 6f 20 63 69 74 65 20 74 68 65  l or to cite the
  224  6d 20 6f 74 68 65 72 20 74 68 61 6e 20 61 73 20  m other than as
  240  2f e2 80 9c 77 6f 72 6b 20 69 6e 20 70 72 6f 67  /...work in prog
  256  72 65 73 73 2e 2f e2 80 9d                       ress./...


  */
const uint8_t Key[] =
{
  0x1c, 0x92, 0x40, 0xa5, 0xeb, 0x55, 0xd3, 0x8a, 0xf3, 0x33, 0x88, 0x86, 0x04, 0xf6, 0xb5, 0xf0,
  0x47, 0x39, 0x17, 0xc1, 0x40, 0x2b, 0x80, 0x09, 0x9d, 0xca, 0x5c, 0xbc, 0x20, 0x70, 0x75, 0xc0
};
const uint8_t IV[] =
{
  0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
};
const uint8_t Plaintext[] =
{
  0x49, 0x6e, 0x74, 0x65, 0x72, 0x6e, 0x65, 0x74, 0x2d, 0x44, 0x72, 0x61, 0x66, 0x74, 0x73, 0x20,
  0x61, 0x72, 0x65, 0x20, 0x64, 0x72, 0x61, 0x66, 0x74, 0x20, 0x64, 0x6f, 0x63, 0x75, 0x6d, 0x65,
  0x6e, 0x74, 0x73, 0x20, 0x76, 0x61, 0x6c, 0x69, 0x64, 0x20, 0x66, 0x6f, 0x72, 0x20, 0x61, 0x20,
  0x6d, 0x61, 0x78, 0x69, 0x6d, 0x75, 0x6d, 0x20, 0x6f, 0x66, 0x20, 0x73, 0x69, 0x78, 0x20, 0x6d,
  0x6f, 0x6e, 0x74, 0x68, 0x73, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x6d, 0x61, 0x79, 0x20, 0x62, 0x65,
  0x20, 0x75, 0x70, 0x64, 0x61, 0x74, 0x65, 0x64, 0x2c, 0x20, 0x72, 0x65, 0x70, 0x6c, 0x61, 0x63,
  0x65, 0x64, 0x2c, 0x20, 0x6f, 0x72, 0x20, 0x6f, 0x62, 0x73, 0x6f, 0x6c, 0x65, 0x74, 0x65, 0x64,
  0x20, 0x62, 0x79, 0x20, 0x6f, 0x74, 0x68, 0x65, 0x72, 0x20, 0x64, 0x6f, 0x63, 0x75, 0x6d, 0x65,
  0x6e, 0x74, 0x73, 0x20, 0x61, 0x74, 0x20, 0x61, 0x6e, 0x79, 0x20, 0x74, 0x69, 0x6d, 0x65, 0x2e,
  0x20, 0x49, 0x74, 0x20, 0x69, 0x73, 0x20, 0x69, 0x6e, 0x61, 0x70, 0x70, 0x72, 0x6f, 0x70, 0x72,
  0x69, 0x61, 0x74, 0x65, 0x20, 0x74, 0x6f, 0x20, 0x75, 0x73, 0x65, 0x20, 0x49, 0x6e, 0x74, 0x65,
  0x72, 0x6e, 0x65, 0x74, 0x2d, 0x44, 0x72, 0x61, 0x66, 0x74, 0x73, 0x20, 0x61, 0x73, 0x20, 0x72,
  0x65, 0x66, 0x65, 0x72, 0x65, 0x6e, 0x63, 0x65, 0x20, 0x6d, 0x61, 0x74, 0x65, 0x72, 0x69, 0x61,
  0x6c, 0x20, 0x6f, 0x72, 0x20, 0x74, 0x6f, 0x20, 0x63, 0x69, 0x74, 0x65, 0x20, 0x74, 0x68, 0x65,
  0x6d, 0x20, 0x6f, 0x74, 0x68, 0x65, 0x72, 0x20, 0x74, 0x68, 0x61, 0x6e, 0x20, 0x61, 0x73, 0x20,
  0x2f, 0xe2, 0x80, 0x9c, 0x77, 0x6f, 0x72, 0x6b, 0x20, 0x69, 0x6e, 0x20, 0x70, 0x72, 0x6f, 0x67,
  0x72, 0x65, 0x73, 0x73, 0x2e, 0x2f, 0xe2, 0x80, 0x9d
};
const uint8_t AddData[] =
{
  0xf3, 0x33, 0x88, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4e, 0x91
};
const uint8_t Expected_Ciphertext[] =
{
  0x64, 0xa0, 0x86, 0x15, 0x75, 0x86, 0x1a, 0xf4, 0x60, 0xf0, 0x62, 0xc7, 0x9b, 0xe6, 0x43, 0xbd,
  0x5e, 0x80, 0x5c, 0xfd, 0x34, 0x5c, 0xf3, 0x89, 0xf1, 0x08, 0x67, 0x0a, 0xc7, 0x6c, 0x8c, 0xb2,
  0x4c, 0x6c, 0xfc, 0x18, 0x75, 0x5d, 0x43, 0xee, 0xa0, 0x9e, 0xe9, 0x4e, 0x38, 0x2d, 0x26, 0xb0,
  0xbd, 0xb7, 0xb7, 0x3c, 0x32, 0x1b, 0x01, 0x00, 0xd4, 0xf0, 0x3b, 0x7f, 0x35, 0x58, 0x94, 0xcf,
  0x33, 0x2f, 0x83, 0x0e, 0x71, 0x0b, 0x97, 0xce, 0x98, 0xc8, 0xa8, 0x4a, 0xbd, 0x0b, 0x94, 0x81,
  0x14, 0xad, 0x17, 0x6e, 0x00, 0x8d, 0x33, 0xbd, 0x60, 0xf9, 0x82, 0xb1, 0xff, 0x37, 0xc8, 0x55,
  0x97, 0x97, 0xa0, 0x6e, 0xf4, 0xf0, 0xef, 0x61, 0xc1, 0x86, 0x32, 0x4e, 0x2b, 0x35, 0x06, 0x38,
  0x36, 0x06, 0x90, 0x7b, 0x6a, 0x7c, 0x02, 0xb0, 0xf9, 0xf6, 0x15, 0x7b, 0x53, 0xc8, 0x67, 0xe4,
  0xb9, 0x16, 0x6c, 0x76, 0x7b, 0x80, 0x4d, 0x46, 0xa5, 0x9b, 0x52, 0x16, 0xcd, 0xe7, 0xa4, 0xe9,
  0x90, 0x40, 0xc5, 0xa4, 0x04, 0x33, 0x22, 0x5e, 0xe2, 0x82, 0xa1, 0xb0, 0xa0, 0x6c, 0x52, 0x3e,
  0xaf, 0x45, 0x34, 0xd7, 0xf8, 0x3f, 0xa1, 0x15, 0x5b, 0x00, 0x47, 0x71, 0x8c, 0xbc, 0x54, 0x6a,
  0x0d, 0x07, 0x2b, 0x04, 0xb3, 0x56, 0x4e, 0xea, 0x1b, 0x42, 0x22, 0x73, 0xf5, 0x48, 0x27, 0x1a,
  0x0b, 0xb2, 0x31, 0x60, 0x53, 0xfa, 0x76, 0x99, 0x19, 0x55, 0xeb, 0xd6, 0x31, 0x59, 0x43, 0x4e,
  0xce, 0xbb, 0x4e, 0x46, 0x6d, 0xae, 0x5a, 0x10, 0x73, 0xa6, 0x72, 0x76, 0x27, 0x09, 0x7a, 0x10,
  0x49, 0xe6, 0x17, 0xd9, 0x1d, 0x36, 0x10, 0x94, 0xfa, 0x68, 0xf0, 0xff, 0x77, 0x98, 0x71, 0x30,
  0x30, 0x5b, 0xea, 0xba, 0x2e, 0xda, 0x04, 0xdf, 0x99, 0x7b, 0x71, 0x4d, 0x6c, 0x6f, 0x2c, 0x29,
  0xa6, 0xad, 0x5c, 0xb4, 0x02, 0x2b, 0x02, 0x70, 0x9b
};

const uint8_t Expected_Tag[] =
{
  0xee, 0xad, 0x9d, 0x67, 0x89, 0x0c, 0xbb, 0x22, 0x39, 0x23, 0x36, 0xfe, 0xa1, 0x85, 0x1f, 0x38
};

const uint8_t Expected_Ciphertext_and_Tag[] =
{
  0x64, 0xa0, 0x86, 0x15, 0x75, 0x86, 0x1a, 0xf4, 0x60, 0xf0, 0x62, 0xc7, 0x9b, 0xe6, 0x43, 0xbd,
  0x5e, 0x80, 0x5c, 0xfd, 0x34, 0x5c, 0xf3, 0x89, 0xf1, 0x08, 0x67, 0x0a, 0xc7, 0x6c, 0x8c, 0xb2,
  0x4c, 0x6c, 0xfc, 0x18, 0x75, 0x5d, 0x43, 0xee, 0xa0, 0x9e, 0xe9, 0x4e, 0x38, 0x2d, 0x26, 0xb0,
  0xbd, 0xb7, 0xb7, 0x3c, 0x32, 0x1b, 0x01, 0x00, 0xd4, 0xf0, 0x3b, 0x7f, 0x35, 0x58, 0x94, 0xcf,
  0x33, 0x2f, 0x83, 0x0e, 0x71, 0x0b, 0x97, 0xce, 0x98, 0xc8, 0xa8, 0x4a, 0xbd, 0x0b, 0x94, 0x81,
  0x14, 0xad, 0x17, 0x6e, 0x00, 0x8d, 0x33, 0xbd, 0x60, 0xf9, 0x82, 0xb1, 0xff, 0x37, 0xc8, 0x55,
  0x97, 0x97, 0xa0, 0x6e, 0xf4, 0xf0, 0xef, 0x61, 0xc1, 0x86, 0x32, 0x4e, 0x2b, 0x35, 0x06, 0x38,
  0x36, 0x06, 0x90, 0x7b, 0x6a, 0x7c, 0x02, 0xb0, 0xf9, 0xf6, 0x15, 0x7b, 0x53, 0xc8, 0x67, 0xe4,
  0xb9, 0x16, 0x6c, 0x76, 0x7b, 0x80, 0x4d, 0x46, 0xa5, 0x9b, 0x52, 0x16, 0xcd, 0xe7, 0xa4, 0xe9,
  0x90, 0x40, 0xc5, 0xa4, 0x04, 0x33, 0x22, 0x5e, 0xe2, 0x82, 0xa1, 0xb0, 0xa0, 0x6c, 0x52, 0x3e,
  0xaf, 0x45, 0x34, 0xd7, 0xf8, 0x3f, 0xa1, 0x15, 0x5b, 0x00, 0x47, 0x71, 0x8c, 0xbc, 0x54, 0x6a,
  0x0d, 0x07, 0x2b, 0x04, 0xb3, 0x56, 0x4e, 0xea, 0x1b, 0x42, 0x22, 0x73, 0xf5, 0x48, 0x27, 0x1a,
  0x0b, 0xb2, 0x31, 0x60, 0x53, 0xfa, 0x76, 0x99, 0x19, 0x55, 0xeb, 0xd6, 0x31, 0x59, 0x43, 0x4e,
  0xce, 0xbb, 0x4e, 0x46, 0x6d, 0xae, 0x5a, 0x10, 0x73, 0xa6, 0x72, 0x76, 0x27, 0x09, 0x7a, 0x10,
  0x49, 0xe6, 0x17, 0xd9, 0x1d, 0x36, 0x10, 0x94, 0xfa, 0x68, 0xf0, 0xff, 0x77, 0x98, 0x71, 0x30,
  0x30, 0x5b, 0xea, 0xba, 0x2e, 0xda, 0x04, 0xdf, 0x99, 0x7b, 0x71, 0x4d, 0x6c, 0x6f, 0x2c, 0x29,
  0xa6, 0xad, 0x5c, 0xb4, 0x02, 0x2b, 0x02, 0x70, 0x9b,
  0xee, 0xad, 0x9d, 0x67, 0x89, 0x0c, 0xbb, 0x22, 0x39, 0x23, 0x36, 0xfe, 0xa1, 0x85, 0x1f, 0x38
};

/* Computed data buffer */
uint8_t Computed_Ciphertext[sizeof(Expected_Ciphertext) + sizeof(Expected_Tag)];
uint8_t Computed_Plaintext[sizeof(Plaintext)];
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
  /* Key attributes */
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

  /* Setup the key policy */
  psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
  psa_set_key_algorithm(&key_attributes, PSA_ALG_CHACHA20_POLY1305);
  psa_set_key_type(&key_attributes, PSA_KEY_TYPE_CHACHA20);
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

  /* Compute directly the ciphertext passing all the needed parameters */
  retval = psa_aead_encrypt(key_handle,                         /* The key id */
                            PSA_ALG_CHACHA20_POLY1305,          /* Algorithm type */
                            IV, sizeof(IV),                     /* Initialization vector */
                            AddData, sizeof(AddData),           /* Additional authenticated data */
                            Plaintext, sizeof(Plaintext),       /* Plaintext to encrypt and authenticate */
                            Computed_Ciphertext,                /* Data buffer to receive ciphertext and auth tag */
                            sizeof(Computed_Ciphertext),        /* Size of buffer to receive ciphertext and tag */
                            &computed_size);                    /* Size of computed ciphertext */

  /* Verify API returned value */
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Verify generated data size is the expected one */
  if (computed_size != (sizeof(Expected_Ciphertext) + sizeof(Expected_Tag)))
  {
    Error_Handler();
  }

  /* Verify generated data are the expected ones */
  if (memcmp(Expected_Ciphertext, Computed_Ciphertext, sizeof(Expected_Ciphertext)) != 0)
  {
    Error_Handler();
  }

  /* Verify generated authentication tag is the expected one */
  if (memcmp(Expected_Tag, &Computed_Ciphertext[sizeof(Expected_Ciphertext)], sizeof(Expected_Tag)) != 0)
  {
    Error_Handler();
  }

  /* Decrypt and verify directly ciphertext and tag passing all the needed parameters */
  retval = psa_aead_decrypt(key_handle,                           /* The key id */
                            PSA_ALG_CHACHA20_POLY1305,            /* Algorithm type */
                            IV, sizeof(IV),                       /* Initialization vector */
                            AddData, sizeof(AddData),             /* Additional authenticated data */
                            Expected_Ciphertext_and_Tag,          /* Ciphertext + tag to decrypt and verify */
                            sizeof(Expected_Ciphertext_and_Tag),  /* Sizeof Ciphertext + tag to decrypt and verify */
                            Computed_Plaintext,                   /* Data buffer to receive generated plaintext */
                            sizeof(Computed_Plaintext),           /* Size of data buff to receive generated plaintext */
                            &computed_size);                      /* Size of computed plaintext */

  /* Verify API returned value */
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Verify generated data size is the expected one */
  if (computed_size != sizeof(Plaintext))
  {
    Error_Handler();
  }

  /* Verify generated data are the expected ones */
  if (memcmp(Plaintext, Computed_Plaintext, computed_size) != 0)
  {
    Error_Handler();
  }

  /* Turn on LD5 in an infinite loop in case of Chacha-Poly1305 operations are successful */
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
