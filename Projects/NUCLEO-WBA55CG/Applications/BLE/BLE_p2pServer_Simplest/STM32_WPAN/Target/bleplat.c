/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    bleplat.c
  * @author  MCD Application Team
  * @brief   This file implements the platform functions for BLE stack library.
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
/* USER CODE END Header */

#include "app_common.h"
#include "bleplat.h"
#include "stm32wbaxx_ll_rng.h"
#include "stm32wbaxx_ll_bus.h"

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

  return;
}

/*****************************************************************************/

void BLEPLAT_RngGet( uint8_t n,
                     uint32_t* val )
{
  LL_AHB2_GRP1_EnableClock( LL_AHB2_GRP1_PERIPH_RNG );
  LL_RNG_Enable( RNG );
  while ( n-- )
  {
    *val++=LL_RNG_ReadRandData32( RNG );
  }
}

/*****************************************************************************/

void BLEPLAT_AesEcbEncrypt( const uint8_t* key,
                            const uint8_t* input,
                            uint8_t* output )
{
  return;
}

/*****************************************************************************/

void BLEPLAT_AesCmacSetKey( const uint8_t* key )
{
  return;
}

/*****************************************************************************/

void BLEPLAT_AesCmacCompute( const uint8_t* input,
                             uint32_t input_length,
                             uint8_t* output_tag )
{
  return;
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
  return 0;
}

/*****************************************************************************/

int BLEPLAT_PkaStartP256Key( const uint32_t* local_private_key )
{
  return 0;
}

/*****************************************************************************/

void BLEPLAT_PkaReadP256Key( uint32_t* local_public_key )
{
  return;
}

/*****************************************************************************/

int BLEPLAT_PkaStartDhKey( const uint32_t* local_private_key,
                           const uint32_t* remote_public_key )
{
  return 0;
}

/*****************************************************************************/

int BLEPLAT_PkaReadDhKey( uint32_t* dh_key )
{
  return 0;
}

/*****************************************************************************/

uint8_t BLEPLAT_TimerStart( uint16_t id,
                            uint32_t timeout )
{
  return 0;
}

/*****************************************************************************/

void BLEPLAT_TimerStop( uint16_t id )
{
  return;
}

/*****************************************************************************/
