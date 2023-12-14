/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    zigbee_plat.c
  * @author  MCD Application Team
  * @brief   Source file for the Zigbee platform adaptation layer
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

#include "app_common.h"
#include "hw.h"
#include "baes.h"

/*****************************************************************************/

void ZIGBEE_PLAT_Init( void )
{
  //TIMER_Init( );
}

/*****************************************************************************/

void ZIGBEE_PLAT_RngInit( void )
{
  // -- Init random HW driver --
  HW_RNG_Start( );
}

/*****************************************************************************/

void ZIGBEE_PLAT_RngProcess( void )
{
  // -- Generate new Random numbers --
  HW_RNG_Process();
}

/*****************************************************************************/

void ZIGBEE_PLAT_RngGet( uint8_t cNumberOfBytes, uint8_t * pValue )
{
  uint8_t   cNbRemainingBytes = cNumberOfBytes;
  uint8_t   *pData = pValue;
  uint32_t  lGeneratedRandom;

  /* Get the requested RNGs (4 bytes by 4bytes) */
  while ( cNbRemainingBytes >= 4u )
  {
    HW_RNG_Get(1, &lGeneratedRandom);
    memcpy( pData, &lGeneratedRandom, 4);
    cNbRemainingBytes -= 4u;
    pData += 4u;
  }

  /* Get the remaining number of RNGs */
  if ( cNbRemainingBytes > 0u )
  {
    HW_RNG_Get(1, &lGeneratedRandom);
    memcpy( pData, &lGeneratedRandom, cNbRemainingBytes);
  }
}

/*****************************************************************************/

void ZIGBEE_PLAT_AesEcbEncrypt( const uint8_t * pKey, const uint8_t * pInput, uint8_t * pOutput )
{
  BAES_EcbCrypt( pKey, pInput, pOutput, HW_AES_ENC );
}

/*****************************************************************************/

void ZIGBEE_PLAT_AesCmacSetKey( const uint8_t * pKey )
{
  BAES_CmacSetKey( pKey );
}

/*****************************************************************************/

void ZIGBEE_PLAT_AesCmacSetVector( const uint8_t * pIV )
{
  BAES_CmacSetVector( pIV );
}

/*****************************************************************************/

void ZIGBEE_PLAT_AesCrypt32( const uint32_t * pInput, uint32_t * pOutput )
{
  HW_AES_Crypt( pInput, pOutput );
}

/*****************************************************************************/

void ZIGBEE_PLAT_AesCrypt( const uint8_t * pInput, uint8_t * pOutput )
{
  HW_AES_Crypt8( pInput, pOutput );
}

/*****************************************************************************/

void ZIGBEE_PLAT_AesCmacCompute( const uint8_t * pInput, uint32_t lInputLength, uint8_t * pOutputTag )
{
  BAES_CmacCompute( pInput, lInputLength, pOutputTag );
}

/*****************************************************************************/
