/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_zigbee_persistence_nvm.c
  * @author  MCD Application Team
  * @brief   Library to simpliest manage persistence on NVM.
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

/* Includes ------------------------------------------------------------------*/
#include <assert.h>

#include "app_conf.h"
#include "app_common.h"
#include "log_module.h"
#include "app_zigbee.h"
#include "app_zigbee_persistence_nvm.h"
#include "dbg_trace.h"

#include "stm32_rtos.h"
#include "stm32_timer.h"
#include "stm32wbaxx_nucleo.h"
#include "stm32wbaxx_ll_icache.h"

#include "zigbee.h"


/* Private includes ---------------------------------------------------------*/
extern void Error_Handler         ( void );

/* USER CODE BEGIN PI */

/* USER CODE END PI */

/* Private types ------------------------------------------------------------*/
typedef struct PersistHeader_t
{
  uint16_t  iCount;
  uint16_t  iDataSize;
  uint16_t  iNvmSize;
  uint16_t  iDataCrc;
  uint16_t  iHeaderCrc;
} PersistHeader_st;

typedef struct PersistInfo_t
{
  uint32_t  lBufferAddress;
  uint16_t  iBufferLength;
} PersistInfo_st;

/* USER CODE BEGIN PT */

/* USER CODE END PT */

/* Private constants --------------------------------------------------------*/
/* USER CODE BEGIN PC */

/* USER CODE END PC */

/* Private defines ------------------------------------------------------------*/
#define PERSISTENCE_NB_PAGES        ( 4u )
//#define PERSISTENCE_END_ADDRESS     ( 0x08100000u )     // For WBA55 - 1MB
#define PERSISTENCE_END_ADDRESS     ( 0x08080000u )     // For WBA55 - 512KB
#define PERSISTENCE_START_ADDRESS   ( PERSISTENCE_END_ADDRESS - ( PERSISTENCE_NB_PAGES * FLASH_PAGE_SIZE ) )

/* Define Polynoms for CRC */
#define CRC16_CCITT             0x1021u         // X.25, V.41, HDLC FCS, Bluetooth, ...
#define CRC16_ISO_13239         0x8408u         // ISO/IEC 13239  ...

#define CRC16_POLY              CRC16_ISO_13239 // Define the used Polynom
#define CRC16_START             0xFFFFu         // Define the used Start CRC.

//#define PERSISTANCE_TESTS

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private variables ---------------------------------------------------------*/
static uint32_t           lPersistenceCurrentAddress;
static uint16_t           iPersistenceCurrentCount;
static PersistInfo_st     stLastPersistInfo;

static enum ZbStatusCodeT eStartupStatus;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Functions Definition ------------------------------------------------------*/

/**
 *
 * @param pstPersistHeader
 * @return
 */
static uint16_t AppZbPersistence_CalcCrcHeader( PersistHeader_st * pstPersistHeader )
{
  return( pstPersistHeader->iCount ^ pstPersistHeader->iDataSize ^ pstPersistHeader->iNvmSize ^ pstPersistHeader->iDataCrc );
}

/**
 *
 * @param pData
 * @param iLength
 * @return
 */
static uint16_t AppZbPersistence_CalcCrc16( uint8_t * pData, uint16_t iLength )
{
  uint8_t     cLoop;
  uint16_t    iCrcValue = CRC16_START;
  uint16_t    iIndex = 0x00;

  // -- Sanity check --
  if( pData != NULL )
  {
      while( iIndex < iLength )
      {
          iCrcValue = iCrcValue ^ (uint16_t)( pData[iIndex] );
          for ( cLoop = 0; cLoop < 8u; cLoop++ )
          {
              if ( ( iCrcValue & 0x0001u ) == 0x0001u )
                  { iCrcValue = ( iCrcValue >> 1u )  ^ CRC16_POLY; }
              else
                  { iCrcValue = ( iCrcValue >> 1u ); }
          }
          iIndex++;
      }
  }

  return ~iCrcValue;
}

/**
 *
 */
bool AppZbPersistence_EraseAllData( void )
{
  bool                    bStatus = true;
  uint32_t                lPageError;
  FLASH_EraseInitTypeDef  stEraseInit;

  /* Determine the page, the number of pages to delete, etc. */
  stEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
  stEraseInit.Page = ( PERSISTENCE_START_ADDRESS - FLASH_BASE ) / FLASH_PAGE_SIZE;
  stEraseInit.NbPages = PERSISTENCE_NB_PAGES;

  /* Delete the flash sectors */
  HAL_FLASH_Unlock();
  if ( HAL_FLASHEx_Erase( &stEraseInit, &lPageError ) != HAL_OK )
  {
    bStatus = false;
  }
  HAL_FLASH_Lock();

  /* Reset parameters */
  lPersistenceCurrentAddress = PERSISTENCE_START_ADDRESS;
  iPersistenceCurrentCount = 1;

  return bStatus;
}

/**
*
*/
static bool AppZbPersistence_ErasePage( uint32_t lFlashAddress )
{
  bool                    bStatus = true;
  uint32_t                lPageError;
  FLASH_EraseInitTypeDef  stEraseInit;

  /* Determine the page, the number of pages to delete, etc. */
  stEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
  stEraseInit.Page = ( lFlashAddress - FLASH_BASE ) / FLASH_PAGE_SIZE;
  stEraseInit.NbPages = 1;

  /* Delete the flash sectors */
  HAL_FLASH_Unlock();
  if ( HAL_FLASHEx_Erase( &stEraseInit, &lPageError ) != HAL_OK )
  {
    bStatus = false;
  }
  HAL_FLASH_Lock();

  return bStatus;
}

/**
 *
 * @param pBuffer
 * @param iBufferLength
 */
bool AppZbPersistence_SaveData( uint8_t * pBuffer, uint16_t iBufferLength )
{
  bool                bStatus = true;
  uint8_t             szFlashData[HW_FLASH_WIDTH];
  uint16_t            iFinalBufferSize, iNbCopy, iPage, iBufferIndex, iNbRetry = 0;
  uint32_t            lFinalAddress, lTempAddress, lPersistenceStartAddress;
  PersistHeader_st    stPersistHeader;

  /* Persistence is Flashed by block, preceded by Persistence definition (Count, Size & CRC )C. */
  /* A save is always a multiple of HW_FLASH_WIDTH */

  /* First, verify if new data are different of previous. If not, Save is not needed */
  if ( iBufferLength == stLastPersistInfo.iBufferLength )
  {
    if ( memcmp( pBuffer, (uint8_t *)stLastPersistInfo.lBufferAddress, iBufferLength ) == 0u )
    {
      LOG_INFO_APP( "  Same data as previous ..." );
      return bStatus;
    }
  }

  /* Verify if Buffer Size if a multiple of Flash Width */
  iFinalBufferSize = iBufferLength + sizeof(stPersistHeader);
  if ( iFinalBufferSize % HW_FLASH_WIDTH != 0u )
  {
    iFinalBufferSize = ( ( iFinalBufferSize / HW_FLASH_WIDTH ) * HW_FLASH_WIDTH ) + HW_FLASH_WIDTH;
  }

  /* Verify if Current Address is not to High and we have an OverFlow of NVM */
  lFinalAddress = lPersistenceCurrentAddress + iFinalBufferSize;
  if ( lFinalAddress >= PERSISTENCE_END_ADDRESS )
  {
    /* Erase First Page */
    lPersistenceCurrentAddress = PERSISTENCE_START_ADDRESS;
    LOG_DEBUG_APP( "  Erase First Page." );

    bStatus = AppZbPersistence_ErasePage(lPersistenceCurrentAddress);
    if ( bStatus == false )
    {
      LOG_ERROR_APP( "  Error, Cannot erase First Page." );
    }
  }
  else
  {
    /* Verify if Current Address is not to High and we continue to another page */
    lTempAddress = ( lPersistenceCurrentAddress & ~( FLASH_PAGE_SIZE - 1u ) ) + FLASH_PAGE_SIZE;
    if ( lFinalAddress >= lTempAddress )
    {
      lPersistenceCurrentAddress = lTempAddress;
      iPage = ( ( lPersistenceCurrentAddress - PERSISTENCE_START_ADDRESS ) / FLASH_PAGE_SIZE );
      LOG_DEBUG_APP( "  Erase Page %d.", iPage );

      bStatus = AppZbPersistence_ErasePage(lPersistenceCurrentAddress);
      if ( bStatus == false )
      {
        LOG_ERROR_APP( "  Error, Cannot erase Page %d.", iPage );
      }
    }
  }

  if ( bStatus != false )
  {
    /* First Data are Count + Size + Data */
    stPersistHeader.iCount = iPersistenceCurrentCount;
    stPersistHeader.iDataSize = iBufferLength;
    stPersistHeader.iNvmSize = iFinalBufferSize;
    stPersistHeader.iDataCrc = AppZbPersistence_CalcCrc16( pBuffer, iBufferLength );
    stPersistHeader.iHeaderCrc = AppZbPersistence_CalcCrcHeader( &stPersistHeader );
    memcpy( szFlashData, &stPersistHeader, sizeof(stPersistHeader) );

    /* Update Data */
    iPersistenceCurrentCount++;

    /* Disable ICache */
    LL_ICACHE_Disable();

    do
    {
      bStatus = true;
      iBufferIndex = 0;

      /* Copy first Persistence Data */
      iNbCopy = HW_FLASH_WIDTH - sizeof(stPersistHeader);
      if ( iNbCopy > iBufferLength )
      {
        iNbCopy = iBufferLength - sizeof(stPersistHeader);
      }
      memcpy( &szFlashData[sizeof(stPersistHeader)], pBuffer, iNbCopy );

      /* Flash NVM with Persistence Data */
      LOG_INFO_APP( "  Save %d data on NVM @ 0x%08X ...", iBufferLength, lPersistenceCurrentAddress );

      lPersistenceStartAddress = lPersistenceCurrentAddress;

      /* Unlock the Flash to enable the flash control register access */
      HAL_FLASH_Unlock();

      do
      {
        if ( HAL_FLASH_Program( FLASH_TYPEPROGRAM_QUADWORD, lPersistenceCurrentAddress, ( uint32_t )&szFlashData ) != HAL_OK )
        {
          bStatus = false;
          LOG_ERROR_APP( "  Error, Flash Data failed @ 0x%08X ...", lPersistenceCurrentAddress );
        }
        else
        {
          /* Read back the value for verification */
          if ( memcmp( ( void * )(lPersistenceCurrentAddress), &szFlashData, HW_FLASH_WIDTH ) != 0x00u )
          {
            LOG_ERROR_APP( "  Error, Flash verification failed @ 0x%08X ...", lPersistenceCurrentAddress );
            bStatus = false;
          }
          else
          {
            /* Update Data for next iteration */
            lPersistenceCurrentAddress += HW_FLASH_WIDTH;
            iBufferIndex += iNbCopy;

            iNbCopy = HW_FLASH_WIDTH;
            if ( ( iBufferIndex + iNbCopy ) > iBufferLength )
            {
              iNbCopy = iBufferLength - iBufferIndex;
            }
            memcpy( szFlashData, &pBuffer[iBufferIndex], iNbCopy );
            if ( iNbCopy < HW_FLASH_WIDTH )
            {
              memset( &szFlashData[iNbCopy], 0, (HW_FLASH_WIDTH - iNbCopy ) );
            }
          }
        }
      }
      while( ( iBufferIndex < iBufferLength ) && ( bStatus != false ) );

      /* Lock the Flash to disable the flash control register access */
      HAL_FLASH_Lock();

      /* In case or error, retry at least ( PERSISTENCE_NB_PAGES - 2 ) times */
      if ( bStatus == false )
      {
        iNbRetry++;

        /* Go to the next page */
        lPersistenceCurrentAddress = ( lPersistenceCurrentAddress & ~( FLASH_PAGE_SIZE - 1u ) ) + FLASH_PAGE_SIZE;
        if ( lPersistenceCurrentAddress >= PERSISTENCE_END_ADDRESS )
        {
          lPersistenceCurrentAddress = PERSISTENCE_START_ADDRESS;
        }

        iPage = ( ( lPersistenceCurrentAddress - PERSISTENCE_START_ADDRESS ) / FLASH_PAGE_SIZE );
        LOG_DEBUG_APP( "  Erase Page %d.\n", iPage );

        if ( AppZbPersistence_ErasePage( lPersistenceCurrentAddress ) == false )
        {
          LOG_ERROR_APP( "  Error, Cannot erase Page %d.", iPage );
        }
      }
    }
    while ( ( bStatus == false ) && ( iNbRetry <= ( PERSISTENCE_NB_PAGES - 2u ) ) );

    /* Re-Enable ICache */
    LL_ICACHE_Enable();
  }

  if ( bStatus != false )
  {
    /* If OK, update Last Persist Informations */
    stLastPersistInfo.lBufferAddress = lPersistenceStartAddress + + sizeof(stPersistHeader);
    stLastPersistInfo.iBufferLength = iBufferLength;
  }

  return( bStatus );
}

/**
 *
 * @param pstZigbee
 * @param arg
 */
static void AppZbPersistence_SaveCallback( struct ZigBeeT * pstZigbee, void *arg)
{
  bool        bStatus;
  uint16_t    iPersistLength;
  uint8_t     *pszPersistData;

  LOG_INFO_APP( "Persistence Callback ..." );
  iPersistLength = ZbPersistGet( pstZigbee, NULL, 0 );

#ifdef PERSISTANCE_TESTS
  iPersistLength = 2560;    // 2.5k buffer size
#endif // PERSISTANCE_TESTS

  /* First verify if Persist is not more that Flash Page */
  if ( iPersistLength >= ( FLASH_PAGE_SIZE - sizeof( PersistHeader_st ) - HW_FLASH_WIDTH  ) )
  {
    LOG_ERROR_APP( "  Error : Persistence to big for Flash (%d bytes).", iPersistLength );
  }
  else
  {
    pszPersistData = malloc( iPersistLength );
    if ( pszPersistData != NULL )
    {
      LOG_DEBUG_APP( "  Need %d data to save.", iPersistLength );
      (void)ZbPersistGet( pstZigbee, pszPersistData, iPersistLength );

      bStatus = AppZbPersistence_SaveData( pszPersistData, iPersistLength );
      if ( bStatus != false )
      {
        LOG_INFO_APP( "  Done." );
      }
      else
      {
        LOG_ERROR_APP( "  Error during Write Page on NVM." );
      }
      free( pszPersistData );
    }
    else
    {
      LOG_ERROR_APP( "  Error : No free memory for malloc (%d bytes).", iPersistLength );
    }
  }
}

/**
 *
 * @param pBuffer
 * @param iBufferLength
 */
static bool AppZbPersistence_SearchLatestData( PersistInfo_st * pstLastPersistInfo )
{
  bool                bFinish = false, bFound = false, bHeaderError;
  uint16_t            iPage, iDataCrc;
  uint32_t            lCurrentAddress;
  PersistHeader_st    stPersistHeader;

  /* Persistence is Flashed by block, preceded by Persistence definition (Count, Size & CRC )C. */
  /* A save is always a multiple of HW_FLASH_WIDTH */

  /* Reset parameters */
  lCurrentAddress = PERSISTENCE_START_ADDRESS;
  lPersistenceCurrentAddress = lCurrentAddress;
  iPersistenceCurrentCount = 1;
  memset( pstLastPersistInfo, 0, sizeof(PersistInfo_st) );

  do
  {
    /* Retrieve Header */
    memcpy( &stPersistHeader, ( void * )lCurrentAddress, sizeof(stPersistHeader) );

    /* Search if Header have an error */
    bHeaderError = true;
    if ( stPersistHeader.iHeaderCrc == AppZbPersistence_CalcCrcHeader( &stPersistHeader ) )
    {
      if ( ( stPersistHeader.iCount != 0xFFFFu ) && ( stPersistHeader.iCount != 0x0u ) )
      {
        bHeaderError = false;

        if ( stPersistHeader.iCount >= iPersistenceCurrentCount )
        {
          /* Verify CRC */
          iDataCrc = AppZbPersistence_CalcCrc16( (uint8_t *)( lCurrentAddress + sizeof(stPersistHeader) ), stPersistHeader.iDataSize );
          if ( iDataCrc == stPersistHeader.iDataCrc )
          {
            pstLastPersistInfo->lBufferAddress = lCurrentAddress + sizeof(stPersistHeader);
            pstLastPersistInfo->iBufferLength = stPersistHeader.iDataSize;
            lPersistenceCurrentAddress = lCurrentAddress + stPersistHeader.iNvmSize;
            iPersistenceCurrentCount = stPersistHeader.iCount + 1u;
            bFound = true;

            LOG_DEBUG_APP( "Found %d data on NVM @ 0x%08X.", stLastPersistInfo.iBufferLength, lCurrentAddress );
            LOG_DEBUG_APP( "Next Data are on NVM @ 0x%08X.", lPersistenceCurrentAddress );
          }
          else
          {
            LOG_DEBUG_APP( "CRC Buffer Error on NVM @ 0x%08X.", lCurrentAddress );
          }
        }

        lCurrentAddress += stPersistHeader.iNvmSize;
      }
    }

    /* If Header Error, search on another Page */
    if ( bHeaderError == true )
    {
      iPage = ( ( lCurrentAddress - PERSISTENCE_START_ADDRESS ) / FLASH_PAGE_SIZE );
      if ( iPage < ( PERSISTENCE_NB_PAGES - 1u ) )
      {
        iPage++;
        lCurrentAddress = PERSISTENCE_START_ADDRESS + ( iPage * FLASH_PAGE_SIZE );
      }
      else
      {
        bFinish = true;
      }
    }
  }
  while ( ( bFinish == false ) && ( lCurrentAddress < PERSISTENCE_END_ADDRESS ) );

  return( bFound );
}

/**
 *
 * @param eStatus
 * @param arg
 */
static void AppZbPersistence_StartupCallback(enum ZbStatusCodeT eStatus, void *arg)
{
  eStartupStatus = eStatus;
  LOG_INFO_APP( "Persistence : Startup Callback with Status : 0x%02X", eStartupStatus );

  UTIL_SEQ_SetEvt( TASK_ZIGBEE_PERSISTENCE );
}

/**
 *
 * @param pszPersistData
 * @param iPersistLength
 * @return
 */
static HAL_StatusTypeDef AppZbPersistence_Startup( struct ZigBeeT * pstZigbee, struct ZbStartupT * pstConfig, PersistInfo_st * pstPersistInfo )
{
  HAL_StatusTypeDef   eStatus = HAL_ERROR;
  enum ZbStatusCodeT  eZbStatus;

  LOG_INFO_APP( "Persistence : startup with Persistence data (%d data @ 0x%08X).", pstPersistInfo->iBufferLength, pstPersistInfo->lBufferAddress );
  UTIL_SEQ_ClrEvt( TASK_ZIGBEE_PERSISTENCE );

  eZbStatus = ZbStartupPersist( pstZigbee, (uint8_t *)pstPersistInfo->lBufferAddress, pstPersistInfo->iBufferLength, pstConfig, AppZbPersistence_StartupCallback, NULL );
  if ( eZbStatus == ZB_STATUS_SUCCESS )
  {
    UTIL_SEQ_WaitEvt( TASK_ZIGBEE_PERSISTENCE );

    if ( eStartupStatus == ZB_STATUS_SUCCESS )
    {
      eStatus = HAL_OK;
    }
    else
    {
      LOG_ERROR_APP( "Persistence : Startup with Persistence callback FAIL (0x%02X)", eStartupStatus );
    }
  }
  else
  {
    LOG_ERROR_APP( "Persistence : ZbStartupPersist FAIL (0x%02X)", eZbStatus );
  }

  return eStatus;
}

/**
 * @brief   Initialize Persistence. If Persistance can be retrieve, load this setting.
 *
 * @return
 */
HAL_StatusTypeDef AppZbPersistence_Init( struct ZigBeeT * pstZigbee, struct ZbStartupT * pstConfig, uint8_t cPersistenceType )
{
  HAL_StatusTypeDef   eStatus = HAL_OK;
  uint32_t            lTimeout;

  /* If needed, erase Persistence Informations */
  if ( eStatus == HAL_OK )
  {
    if ( cPersistenceType == APP_ZB_PERSISTENCE_TYPE_ENABLED_ERASED )
    {
      /* Erase used Flash */
      LOG_INFO_APP( "Persistence : Erase Flash on Host demand." );
      if ( AppZbPersistence_EraseAllData() != false )
      {
        cPersistenceType = APP_ZB_PERSISTENCE_TYPE_ENABLED;
      }
      else
      {
        LOG_INFO_APP( "Persistence : error during Flash Erase." );
      }
    }
  }

  if ( eStatus == HAL_OK )
  {
    if ( cPersistenceType == APP_ZB_PERSISTENCE_TYPE_ENABLED )
    {
      /* Start by clearing the persistence callback */
      if ( ZbPersistNotifyRegister( pstZigbee, NULL, NULL) == false )
      {
        LOG_INFO_APP( "Persistence : error during 'ZbPersistNotifyRegister'." );
      }
      else
      {
        /* Continue by search the latest persistent data saved */
        if ( AppZbPersistence_SearchLatestData( &stLastPersistInfo ) != false )
        {
          HAL_Delay(200);
          LOG_INFO_APP( "Persistence : retrieve %d data on NVM.", stLastPersistInfo.iBufferLength );
          eStatus = AppZbPersistence_Startup( pstZigbee, pstConfig, &stLastPersistInfo );
        }
        else
        {
          eStatus = HAL_ERROR;
        }

        /* In all case, start Persistence Callback */
        if ( ZbBdbGet( pstZigbee, ZB_BDB_PersistTimeoutMs, &lTimeout, sizeof( lTimeout ) ) == ZB_STATUS_SUCCESS )
        {
          LOG_INFO_APP( "Persistence : TimeOut is : %ld.", lTimeout );
        }
        else
        {
          LOG_INFO_APP( "Error during call of ZbBdbGet with ZB_BDB_PersistTimeoutMs" );
        }

        if ( ZbPersistNotifyRegister( pstZigbee, AppZbPersistence_SaveCallback, NULL) == false )
        {
          LOG_INFO_APP( "Persistence : error during 'ZbPersistNotifyRegister' for Callback." );
        }
      }
    }
  }

  return eStatus;
}
