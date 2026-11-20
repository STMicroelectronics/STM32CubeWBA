/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    bleplat.c
  * @author  MCD Application Team
  * @brief   This file implements the platform functions for BLE stack library.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#include "app_common.h"
#include "bleplat.h"
#include "app_ble.h"
#include "psa_plat_crypto.h"
#include "ble_timer.h"
#include "blestack.h"
#include "host_stack_if.h"

#include "ble_wrap.c"

/*****************************************************************************/

void BLEPLAT_Init( void )
{

  return;
}

/*****************************************************************************/

void BLEPLAT_NvmStore( const uint64_t* ptr,
                              uint16_t size )
{
  UNUSED(ptr);
  UNUSED(size);

  APP_BLE_HostNvmStore();
  return;
}

/*****************************************************************************/

void BLEPLAT_RngGet( uint8_t n,
                     uint32_t* val )
{
  if (psa_generate_random((uint8_t*)val, n << 2) != PSA_SUCCESS)
  {
    assert_param(1);
  }
}

/*****************************************************************************/

void BLEPLAT_AesEcbEncrypt( const uint8_t* key,
                            const uint8_t* input,
                            uint8_t* output )
{
  PSA_PLAT_CRYPTO_AesEcbEncryt(key, input, output);
}

/*****************************************************************************/

void BLEPLAT_AesCmacSetKey( const uint8_t* key )
{
  PSA_PLAT_CRYPTO_AesCmacSetKey(key);
}

/*****************************************************************************/

void BLEPLAT_AesCmacCompute( const uint8_t* input,
                             uint32_t input_length,
                             uint8_t* output_tag )
{
  PSA_PLAT_CRYPTO_AesCmacCompute(input, input_length, output_tag);
}

/*****************************************************************************/

int BLEPLAT_AesCcmCrypt( uint8_t mode,
                         const uint8_t* key,
                         uint8_t iv_length,
                         const uint8_t* iv,
                         uint16_t add_length,
                         const uint8_t* add,
                         uint32_t input_length,
                         const uint8_t* input,
                         uint8_t tag_length,
                         uint8_t* tag,
                         uint8_t* output )
{
  return PSA_PLAT_CRYPTO_CcmCrypt( mode, key, iv_length, iv, add_length, add,
                                  input_length, input, tag_length, tag, output );
}

/*****************************************************************************/

int BLEPLAT_PkaStartP256Key( const uint32_t* local_private_key )
{
  return PSA_PLAT_CRYPTO_PkaStartP256Key(local_private_key);
}

/*****************************************************************************/

void BLEPLAT_PkaReadP256Key( uint32_t* local_public_key )
{
  PSA_PLAT_CRYPTO_PkaReadP256Key(local_public_key);
}

/*****************************************************************************/

int BLEPLAT_PkaStartDhKey( const uint32_t* local_private_key,
                           const uint32_t* remote_public_key )
{
  int ret = PSA_PLAT_CRYPTO_PkaStartDhKey(local_private_key, remote_public_key);

  if (ret == PSA_ERROR_INVALID_ARGUMENT)
  {
    return BLEPLAT_ERROR;
  }
  else if (ret == PSA_ERROR_INSUFFICIENT_MEMORY)
  {
    return BLEPLAT_FULL;
  }
  else if (ret != PSA_SUCCESS)
  {
    return BLEPLAT_BUSY;
  }

  return ret;
}

/*****************************************************************************/

int BLEPLAT_PkaReadDhKey( uint32_t* dh_key )
{
  return PSA_PLAT_CRYPTO_PkaReadDhKey(dh_key);
}

/*****************************************************************************/

uint8_t BLEPLAT_TimerStart( uint16_t id,
                            uint32_t timeout )
{
  return BLE_TIMER_Start( id, timeout );
}

/*****************************************************************************/

void BLEPLAT_TimerStop( uint16_t id )
{
  BLE_TIMER_Stop( id );
}
/*****************************************************************************/
