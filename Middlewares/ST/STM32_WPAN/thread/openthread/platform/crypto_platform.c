/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    crypto_platform.c
  * @author  MCD Application Team
  * @brief   This file implements part of the OpenThread crypto platform abstraction
  * Functions replace weak function of crypto_platform.cpp file
  *          UART communication.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#include "openthread-core-config.h"
#include "mbedtls-config.h"
#include <string.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>

#include "error.h"
#include "platform/misc.h"
#include "platform/entropy.h"
#include "platform/crypto.h"

#if OPENTHREAD_CONFIG_CRYPTO_LIB == OPENTHREAD_CONFIG_CRYPTO_LIB_MBEDTLS

#if (!defined(MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES) && \
     (!defined(MBEDTLS_NO_PLATFORM_ENTROPY) || defined(MBEDTLS_HAVEGE_C) || defined(MBEDTLS_ENTROPY_HARDWARE_ALT)))
#define OT_MBEDTLS_STRONG_DEFAULT_ENTROPY_PRESENT
#endif

#if !OPENTHREAD_RADIO
/**
 * Below context structures depend of mbedtls config they cannot be integrated in openthread lib 
 * For example matter has capability MBEDTLS_THREADING_C where as OT hasn't by default
 *
 */
static mbedtls_ctr_drbg_context sCtrDrbgContext;
static mbedtls_entropy_context  sEntropyContext;
#ifndef OT_MBEDTLS_STRONG_DEFAULT_ENTROPY_PRESENT
static uint16_t kEntropyMinThreshold = 16;
#endif
#endif

#ifndef OT_MBEDTLS_STRONG_DEFAULT_ENTROPY_PRESENT

static int handleMbedtlsEntropyPoll(void *aData, unsigned char *aOutput, size_t aInLen, size_t *aOutLen)
{
    int rval = MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;

    if (otPlatEntropyGet((uint8_t *)(aOutput), (uint16_t)(aInLen)) == 0)
    {
      rval = 0;
    }
    
    if (aOutLen != NULL)
    {
      *aOutLen = aInLen;
    }

    OT_UNUSED_VARIABLE(aData);
    return rval;
}

#endif // OT_MBEDTLS_STRONG_DEFAULT_ENTROPY_PRESENT

void otPlatCryptoRandomInit(void)
{
    mbedtls_entropy_init(&sEntropyContext);

#ifndef OT_MBEDTLS_STRONG_DEFAULT_ENTROPY_PRESENT
    mbedtls_entropy_add_source(&sEntropyContext, handleMbedtlsEntropyPoll, NULL, kEntropyMinThreshold,
                               MBEDTLS_ENTROPY_SOURCE_STRONG);
#endif

    mbedtls_ctr_drbg_init(&sCtrDrbgContext);

    int rval = mbedtls_ctr_drbg_seed(&sCtrDrbgContext, mbedtls_entropy_func, &sEntropyContext, NULL, 0);
    if (rval != 0)
    {
      otPlatAssertFail(NULL,0);
    }
}

void otPlatCryptoRandomDeinit(void)
{
    mbedtls_entropy_free(&sEntropyContext);
    mbedtls_ctr_drbg_free(&sCtrDrbgContext);
}

otError otPlatCryptoRandomGet(uint8_t *aBuffer, uint16_t aSize)
{
  otError error = OT_ERROR_NONE;

  if (mbedtls_ctr_drbg_random(&sCtrDrbgContext, aBuffer, (size_t)(aSize)) != 0)
  {
    error = OT_ERROR_FAILED;
  }

  return error;
}

#endif // #if OPENTHREAD_CONFIG_CRYPTO_LIB == OPENTHREAD_CONFIG_CRYPTO_LIB_MBEDTLS