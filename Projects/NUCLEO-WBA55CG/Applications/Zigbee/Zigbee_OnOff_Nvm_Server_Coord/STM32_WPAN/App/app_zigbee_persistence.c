/**
  ******************************************************************************
  * File Name          : app_zigbee_persistence.c
  * Description        : Zigbee Persistence Management for all Zigbee Application.
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

/* Includes ------------------------------------------------------------------*/
#include <assert.h>

#include "app_conf.h"
#include "app_common.h"
#include "log_module.h"
#include "app_zigbee.h"
#include "dbg_trace.h"

#include "stm32_rtos.h"
#include "stm32_timer.h"
#include "stm32wbaxx_nucleo.h"
#include "flash_manager.h"
#include "flash_driver.h"
#include "simple_nvm_arbiter.h"

#include "zigbee.h"


/* Private defines -----------------------------------------------------------*/
#define PERSISTENCE_ENABLED                     (1u)

#define PERSISTENCE_CACHE_SIZE_IN_WORDS         1000u /* Arbitrary Choice for RAM cache size in U32/Word */
#define PERSISTENCE_CACHE_SIZE_IN_BYTES         ( 4u * PERSISTENCE_CACHE_SIZE_IN_WORDS ) /* Max size of the RAM cache in bytes */

#define PERSISTENCE_LENGTH_OFFSET_IN_WORDS      0u                                              /* Offset in words for Persistence Length */
#define PERSISTENCE_LENGTH_OFFSET_IN_BYTES      ( 4u * PERSISTENCE_LENGTH_OFFSET_IN_WORDS )     /* Offset in bytes for Persistence Length */
#define PERSISTENCE_DATA_OFFSET_IN_WORDS        1u                                              /* Offset in words for Persistence Data */
#define PERSISTENCE_DATA_OFFSET_IN_BYTES        ( 4u * PERSISTENCE_DATA_OFFSET_IN_WORDS)        /* Offset in bytes for Persistence Data */

/* Private Structure Definition ------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/


/* Private variables -----------------------------------------------*/

/* Cache in uninitialized RAM to store/retrieve persistent data */
union cache
{
  uint8_t       cData[PERSISTENCE_CACHE_SIZE_IN_BYTES]; // in bytes
  uint32_t 	lData[PERSISTENCE_CACHE_SIZE_IN_WORDS]; // in U32 words
};

__attribute__ ((section(".noinit"))) union cache sCachePersistentData;
__attribute__ ((section(".noinit"))) uint8_t szCacheBuffer[PERSISTENCE_CACHE_SIZE_IN_BYTES - PERSISTENCE_DATA_OFFSET_IN_BYTES];

static enum ZbStatusCodeT       eStartupPersistStatus;


/* Functions Definition ------------------------------------------------------*/

/**
 * @brief  Load persitent data
 * @param  None
 * @retval 'true' if successful, else 'false'
 */
static bool APP_ZIGBEE_Persistence_Load( void )
{
  uint32_t              lDataLength;
#ifdef PERSISTENCE_ENABLED
  SNVMA_Cmd_Status_t    eStatus;
#endif /* PERSISTENCE_ENABLED */

#ifdef PERSISTENCE_ENABLED
  /* Load the persistent data */
  LOG_INFO_APP( "Loading persistent data" );
  eStatus = SNVMA_Restore( APP_ZIGBEE_NvmBuffer );
  if ( eStatus != SNVMA_ERROR_OK )
  {
    LOG_ERROR_APP( "Error, failed to load persistent data (0x%02" PRIX8 ").", eStatus );
    memset( sCachePersistentData.cData, 0x00, PERSISTENCE_CACHE_SIZE_IN_BYTES );
    return false;
  }
#endif /* PERSISTENCE_ENABLED */

  /* Get the length of the persistent data */
  lDataLength = sCachePersistentData.lData[PERSISTENCE_LENGTH_OFFSET_IN_WORDS];

  /* Check if the length of the persistent data is zero */
  if ( lDataLength == 0u )
  {
    LOG_ERROR_APP( "Error, persistent data length is zero." );
    return false;
  }

  /* Check if the length of the persistent data exceeds the cache size */
  if ( lDataLength > PERSISTENCE_CACHE_SIZE_IN_BYTES )
  {
    LOG_ERROR_APP( "Error, persistent data length greater than cache size (%d).", lDataLength );
    return false;
  }

#if 0 /* Debug */
  for ( uint32_t lIndex = 0u; lIndex < PERSISTENCE_CACHE_SIZE_IN_WORDS; lIndex += 4u )
  {
    LOG_DEBUG_APP( "0x%08" PRIX32 " | 0x%08" PRIX32 " | 0x%08" PRIX32 " | 0x%08" PRIX32,
                   sCachePersistentData.lData[lIndex],
                   sCachePersistentData.lData[lIndex + 1u],
                   sCachePersistentData.lData[lIndex + 2u],
                   sCachePersistentData.lData[lIndex + 3u]);
  }
#endif

  LOG_INFO_APP( "Persistent data loaded (%d bytes)", lDataLength );

  return true;
}


#ifdef PERSISTENCE_ENABLED
/**
 * @brief  SNVMA write callback function
 * @param  eStatus Callback status
 * @retval none
 */
static void APP_ZIGBEE_Persistence_SnvmaCallback( SNVMA_Callback_Status_t eStatus )
{
  if ( eStatus != SNVMA_OPERATION_COMPLETE )
  {
    /* Retry the write operation */
    SNVMA_Write( APP_ZIGBEE_NvmBuffer, APP_ZIGBEE_Persistence_SnvmaCallback );
  }
  else
  {
    UTIL_SEQ_SetEvt( EVENT_ZIGBEE_SNVMA_WRITE_ENDED );
  }
}
#endif /* PERSISTENCE_ENABLED */


/**
 * @brief  Save persistent data
 * @param  pstZigbee Zigbee stack info
 * @retval 'true' if successful, else 'false'
 */
static bool APP_ZIGBEE_Persistence_Save( struct ZigBeeT * pstZigbee )
{
  uint32_t              lDataLength;
#ifdef PERSISTENCE_ENABLED
  SNVMA_Cmd_Status_t    eStatus;
#endif /* PERSISTENCE_ENABLED */

  /* Get the length of the stack persistent data */
  lDataLength = ZbPersistGet( pstZigbee, NULL, 0u );

  /* Check if the length of the persistent data is zero */
  if ( lDataLength == 0u )
  {
    LOG_ERROR_APP( "Error, persistent data length is zero." );
    return false;
  }

  /* Check if the length of the persistent data exceeds the cache size */
  if ( lDataLength > PERSISTENCE_CACHE_SIZE_IN_BYTES )
  {
    LOG_ERROR_APP( "Error, persistent data length greater than cache size (%d).", lDataLength );
    return false;
  }

  /* Get the stack persistent data */
  ( void )ZbPersistGet( pstZigbee, szCacheBuffer, lDataLength );

  if ( lDataLength != sCachePersistentData.lData[PERSISTENCE_LENGTH_OFFSET_IN_WORDS] ||
       memcmp( szCacheBuffer, &sCachePersistentData.cData[PERSISTENCE_DATA_OFFSET_IN_BYTES], lDataLength ) != 0 )
  {
    /* Copy the stack persistent data to cache */
    memset( sCachePersistentData.cData, 0x00, PERSISTENCE_CACHE_SIZE_IN_BYTES );
    sCachePersistentData.lData[PERSISTENCE_LENGTH_OFFSET_IN_WORDS] = lDataLength;
    memcpy( &sCachePersistentData.cData[PERSISTENCE_DATA_OFFSET_IN_BYTES], szCacheBuffer, lDataLength );

#ifdef PERSISTENCE_ENABLED
    /* Save the persistent data */
    LOG_INFO_APP( "Saving persistent data" );
    UTIL_SEQ_ClrEvt( EVENT_ZIGBEE_SNVMA_WRITE_ENDED );
    eStatus = SNVMA_Write( APP_ZIGBEE_NvmBuffer, APP_ZIGBEE_Persistence_SnvmaCallback );
    if ( eStatus != SNVMA_ERROR_OK )
    {
      LOG_ERROR_APP( "Error, failed to save persistent data (0x%02" PRIX8 ").", eStatus );
      memset( sCachePersistentData.cData, 0x00, PERSISTENCE_CACHE_SIZE_IN_BYTES );
      return false;
    }
#endif /* PERSISTENCE_ENABLED */
  }

#if 0 /* Debug */
  for ( uint32_t lIndex = 0u; lIndex < PERSISTENCE_CACHE_SIZE_IN_WORDS; lIndex += 4u )
  {
    LOG_DEBUG_APP( "0x%08" PRIX32 " | 0x%08" PRIX32 " | 0x%08" PRIX32 " | 0x%08" PRIX32,
                   sCachePersistentData.lData[lIndex],
                   sCachePersistentData.lData[lIndex + 1u],
                   sCachePersistentData.lData[lIndex + 2u],
                   sCachePersistentData.lData[lIndex + 3u]);
  }
#endif

  LOG_INFO_APP( "Persistent data saved (%d bytes)", lDataLength );

  return true;
}


/**
 * @brief  Initialize persistence
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_Persistence_Init( void )
{
#ifdef PERSISTENCE_ENABLED
  /* Register the cache buffer to SNVMA */
  ( void )SNVMA_Register( APP_ZIGBEE_NvmBuffer, sCachePersistentData.lData, PERSISTENCE_CACHE_SIZE_IN_WORDS );
#endif /* PERSISTENCE_ENABLED */

  /* Clear persistence cache */
  memset( sCachePersistentData.cData, 0x00, PERSISTENCE_CACHE_SIZE_IN_BYTES );
}


/**
 * @brief  Startup persistence callback function
 * @param  eStatus      Startup status
 * @param  pArg         Callback arguments
 * @retval None
 */
static void APP_ZIGBEE_Persistence_StartupCallback( enum ZbStatusCodeT eStatus, void * pArg )
{
  /* Save the callback status */
  eStartupPersistStatus = eStatus;
  UTIL_SEQ_SetEvt( EVENT_ZIGBEE_STARTUP_PERSISTENCE_ENDED );
}


/**
 * @brief  Notify to save persitent data callback function
 * @param  pstZigbee    Zigbee stack info
 * @param  pArg         Callback arguments
 * @retval None
 */
static void APP_ZIGBEE_Persistence_NotifyCallback( struct ZigBeeT * pstZigbee, void * pArg )
{
  /* Save the persistent data */
  ( void )APP_ZIGBEE_Persistence_Save( pstZigbee );
}


/**
 * @brief  Start Zigbee Network from persistent data
 * @param  pstZigbeeAppInfo     Zigbee application info
 * @param  bSavePersistence     Save persistent data if 'true'
 * @retval 'true' if successful, else 'false'
 */
bool APP_ZIGBEE_Persistence_StartupNwk( ZigbeeAppInfo_t * pstZigbeeAppInfo, bool bSavePersistence )
{
  bool                  bStatus;
  uint64_t              llEpid = 0u;
  uint32_t              lDataLength;
  bool                  bReturn = false;

  /* Clear the persistence callback */
  ( void )ZbPersistNotifyRegister( pstZigbeeAppInfo->pstZigbee, NULL, NULL );

  /* Restore persistence */
  bStatus = APP_ZIGBEE_Persistence_Load();
  if ( bStatus != false )
  {
    /* Make sure the EPID is cleared before starting the stack from persistence */
    ( void )ZbNwkSet( pstZigbeeAppInfo->pstZigbee, ZB_NWK_NIB_ID_ExtendedPanId, &llEpid, sizeof( llEpid ) );

    /* Attempt to start the stack from persistence */
    lDataLength = sCachePersistentData.lData[PERSISTENCE_LENGTH_OFFSET_IN_WORDS];
    pstZigbeeAppInfo->eJoinStatus = ZbStartupPersist( pstZigbeeAppInfo->pstZigbee, &sCachePersistentData.cData[PERSISTENCE_DATA_OFFSET_IN_BYTES], lDataLength, NULL, APP_ZIGBEE_Persistence_StartupCallback, NULL );
    if ( pstZigbeeAppInfo->eJoinStatus == ZB_STATUS_SUCCESS )
    {
      UTIL_SEQ_WaitEvt( EVENT_ZIGBEE_STARTUP_PERSISTENCE_ENDED );

      if ( eStartupPersistStatus == ZB_STATUS_SUCCESS )
      {
        LOG_INFO_APP( "Startup from persistence successful" );
        pstZigbeeAppInfo->bNwkStartup = false;
        bReturn = true;
      }
      else
      {
        LOG_ERROR_APP( "Error, startup from persistence failed (0x%02" PRIX8 ").", eStartupPersistStatus );
      }
    }
    else
    {
      LOG_ERROR_APP( "Error, startup from persistence failed (0x%02" PRIX8 ").", pstZigbeeAppInfo->eJoinStatus );
    }
  }

  if ( bSavePersistence )
  {
    /* Register the persistence callback */
    ( void )ZbPersistNotifyRegister( pstZigbeeAppInfo->pstZigbee, APP_ZIGBEE_Persistence_NotifyCallback, NULL );

    /* Call the callback once here to save persistence data */
    APP_ZIGBEE_Persistence_NotifyCallback( pstZigbeeAppInfo->pstZigbee, NULL );
  }

  return bReturn;
}


/**
 * @brief  Delete persistent data
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_Persistence_Delete( void )
{
  /* Clear persistence cache */
  memset( sCachePersistentData.cData, 0x00, PERSISTENCE_CACHE_SIZE_IN_BYTES );

#ifdef PERSISTENCE_ENABLED
  /* Clear flash */
  UTIL_SEQ_ClrEvt( EVENT_ZIGBEE_SNVMA_WRITE_ENDED );
  SNVMA_Write( APP_ZIGBEE_NvmBuffer, APP_ZIGBEE_Persistence_SnvmaCallback );
#endif /* PERSISTENCE_ENABLED */
}
