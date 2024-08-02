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

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

#include "app_common.h"
#include "hw.h"
#include "baes.h"
#include "stm32_mm.h"

/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/
#define C_SYS_MEMORY_HEAP_SIZE_BYTES          CFG_MM_POOL_SIZE

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* round a size in bytes to a size in words (32 bits) */
#define BYTES_TO_WORD32(a) (((a) + 3) >> 2)

/* Private constants ---------------------------------------------------------*/
/* USER CODE BEGIN PC */

/* USER CODE END PC */

/* Private variables ---------------------------------------------------------*/
static uint8_t        SYS_MEMORY_HEAP[C_SYS_MEMORY_HEAP_SIZE_BYTES];

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/
/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private functions prototypes-----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/

/**
 *
 */
void ZIGBEE_PLAT_Init( void )
{
  //TIMER_Init( );
}

/**
 *
 */
void ZIGBEE_PLAT_RngInit( void )
{
  // -- Init random HW driver --
  HW_RNG_Start( );
}

/**
 *
 */
void ZIGBEE_PLAT_RngProcess( void )
{
  // -- Generate new Random numbers --
  HW_RNG_Process();
}

/**
 *
 */
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

/**
 *
 */
void ZIGBEE_PLAT_AesEcbEncrypt( const uint8_t * pKey, const uint8_t * pInput, uint8_t * pOutput )
{
  BAES_EcbCrypt( pKey, pInput, pOutput, HW_AES_ENC );
}

/**
 *
 */
void ZIGBEE_PLAT_AesCmacSetKey( const uint8_t * pKey )
{
  BAES_CmacSetKey( pKey );
}

/**
 *
 */
void ZIGBEE_PLAT_AesCmacSetVector( const uint8_t * pIV )
{
  BAES_CmacSetVector( pIV );
}

/**
 *
 */
void ZIGBEE_PLAT_AesCrypt32( const uint32_t * pInput, uint32_t * pOutput )
{
  HW_AES_Crypt( pInput, pOutput );
}

/**
 *
 */
void ZIGBEE_PLAT_AesCrypt( const uint8_t * pInput, uint8_t * pOutput )
{
  HW_AES_Crypt8( pInput, pOutput );
}

/**
 *
 */
void ZIGBEE_PLAT_AesCmacCompute( const uint8_t * pInput, uint32_t lInputLength, uint8_t * pOutputTag )
{
  BAES_CmacCompute( pInput, lInputLength, pOutputTag );
}

/**
 *
 */
bool ZIGBEE_PLAT_ZbHeapInit( void )
{
  static bool  bHeapAlreadyDone = false;

  /* Initialize System Heap used by Zigbee Stack */
  if ( bHeapAlreadyDone == false )
  {
    UTIL_MM_Init( SYS_MEMORY_HEAP, C_SYS_MEMORY_HEAP_SIZE_BYTES );
    bHeapAlreadyDone = true;
  }

  return true;
}

/**
 *
 */
void * ZIGBEE_PLAT_ZbHeapMalloc( uint32_t iSize )
{
  void  *ptr;

  ptr = UTIL_MM_GetBuffer( iSize );

  return ptr;
}

/**
 *
 */
void ZIGBEE_PLAT_ZbHeapFree( void * ptr )
{
  if ( ptr != NULL )
  {
    UTIL_MM_ReleaseBuffer( ptr );
  }
  else
  {
    /* Intentional INFINITE_LOOP */
    /* coverity[no_escape] */
    while(1) {}
  }
}

/**
 *
 */
unsigned int ZIGBEE_PLAT_ZbHeapMallocCurrentSize()
{
  return 0U;
}

/**
 *
 */
bool ZIGBEE_PLAT_HeapInit( void )
{
  return ZIGBEE_PLAT_ZbHeapInit();
}

/**
 *
 */
void * ZIGBEE_PLAT_HeapMalloc( uint32_t iSize )
{
  void  *ptr;

  ptr = UTIL_MM_GetBuffer( iSize );

  return ptr;
}

/**
 *
 */
void ZIGBEE_PLAT_HeapFree( void * ptr )
{
  if ( ptr != NULL )
  {
    UTIL_MM_ReleaseBuffer( ptr );
  }
  else
  {
    /* Intentional INFINITE_LOOP */
    /* coverity[no_escape] */
    while(1) {}
  }
}

/**
 *
 */
unsigned int ZIGBEE_PLAT_HeapMallocCurrentSize( void )
{
  return 0U;
}

/**
 *
 */
unsigned long ZIGBEE_PLAT_HeapAvailable( void )
{
    return 0U;
}

/**
 *
 */
bool ZIGBEE_PLAT_HeapCheckAlloc( unsigned int iSize )
{
    /* Just assume we have enough memory in the system heap */
    return true;
}

/**
 *
 */
unsigned long ZIGBEE_PLAT_HeapUsed( void )
{
    /*  ZIGBEE_PLAT_HeapUSed is not updated in this file, because we're not tracking memory usage. */
    return 0U;
}

/**
 *
 */
unsigned long ZIGBEE_PLAT_HeapHighWaterMark( void )
{
    /* ZIGBEE_PLAT_HeapHighWaterMark is not updated in this file. */
    return 0U;
}
