/**
  ******************************************************************************
  * File Name          : app_thread_persistence.c
  * Description        : Persistence management for thread applications
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
#include "dbg_trace.h"

#include "stm32_rtos.h"
#include "stm32_timer.h"
#include "stm32wbaxx_nucleo.h"
#include "flash_manager.h"
#include "flash_driver.h"
#include "simple_nvm_arbiter.h"
#include "thread.h"
#include "app_thread_persistence.h"
#include "app_thread.h"
#include "flash.h"


/* Private defines -----------------------------------------------------------*/
#define PERSISTENCE_ENABLED                     (1u)

#define DEBUG_NVM_BUFFER                        (0u)

#define NVM_TIMER_PERIOD                        (1000 * 60 * 60) /* Take a backup every 1 hour */

/* Arbitrary Choice for RAM cache size in U32/Word */
/* this buffer should be big enough to save the THREAD_SETTINGS_BUFFER_SIZE + 1 word to save the length of the old data*/
#define PERSISTENCE_CACHE_SIZE_IN_WORDS         257u 
#define PERSISTENCE_CACHE_SIZE_IN_BYTES         ( 4u * PERSISTENCE_CACHE_SIZE_IN_WORDS ) /* Max size of the RAM cache in bytes */

#define PERSISTENCE_LENGTH_OFFSET_IN_WORDS      0u                                              /* Offset in words for Persistence Length */
#define PERSISTENCE_LENGTH_OFFSET_IN_BYTES      ( 4u * PERSISTENCE_LENGTH_OFFSET_IN_WORDS )     /* Offset in bytes for Persistence Length */
#define PERSISTENCE_DATA_OFFSET_IN_WORDS        1u                                              /* Offset in words for Persistence Data */
#define PERSISTENCE_DATA_OFFSET_IN_BYTES        ( 4u * PERSISTENCE_DATA_OFFSET_IN_WORDS)        /* Offset in bytes for Persistence Data */

/* Private Structure Definition ------------------------------------------------------*/

typedef struct {
  CRC_HandleTypeDef crcHandle;
  uint16_t iCrcValue;
} CacheCrc_t;

/* Private function prototypes -----------------------------------------------*/

static void processNVM(void * arg);
static void APP_THREAD_OTNvmInit(void);

#if (DEBUG_NVM_BUFFER == 1)
static void printThreadPersistentBuf(void);
#endif /*(DEBUG_NVM_BUFFER == 1) */

/* Private variables -----------------------------------------------*/

/* Cache in uninitialized RAM to store/retrieve persistent data */
union cache
{
  uint8_t       cData[PERSISTENCE_CACHE_SIZE_IN_BYTES]; // in bytes
  uint32_t 	lData[PERSISTENCE_CACHE_SIZE_IN_WORDS]; // in U32 words
};

__attribute__ ((section(".noinit"))) union cache sCachePersistentData;
__attribute__ ((section(".noinit"))) uint8_t sCacheBuffer[PERSISTENCE_CACHE_SIZE_IN_BYTES - PERSISTENCE_DATA_OFFSET_IN_BYTES];

UTIL_TIMER_Object_t nvmTimerId;

static volatile bool otThreadSettingsChanged = false;

/* Functions Definition ------------------------------------------------------*/

/**
 * @brief  Load persitent data
 * @param  None
 * @retval 'true' if successful, else 'false'
 */
bool APP_THREAD_Persistence_Load(void)
{
  uint32_t              lDataLength;
#ifdef PERSISTENCE_ENABLED
  SNVMA_Cmd_Status_t    eStatus;
#endif /* PERSISTENCE_ENABLED */

#ifdef PERSISTENCE_ENABLED
  /* Load the persistent data */
  LOG_INFO_APP( "Loading persistent data" );
  eStatus = SNVMA_Restore( APP_THREAD_NvmBuffer );
  if ( eStatus != SNVMA_ERROR_OK )
  {
    LOG_ERROR_APP( "Error, failed to load persistent data (0x%02X).", eStatus );
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
static void APP_THREAD_Persistence_SnvmaCallback( SNVMA_Callback_Status_t eStatus )
{
  if ( eStatus != SNVMA_OPERATION_COMPLETE )
  {
    /* Retry the write operation */
    SNVMA_Write( APP_THREAD_NvmBuffer, APP_THREAD_Persistence_SnvmaCallback );
  }
  else
  {
     UTIL_SEQ_SetEvt( EVENT_THREAD_SNVMA_WRITE_ENDED );
  }
}
#endif /* PERSISTENCE_ENABLED */


/**
 * @brief  Save 'Persistent' data
 * @param  None
 * @retval true if success , false if fail
 */
void APP_THREAD_Persistence_Save(void)
{
#ifdef PERSISTENCE_ENABLED
  SNVMA_Cmd_Status_t    eStatus;
#endif /* PERSISTENCE_ENABLED */
  
  uint32_t settingBufferSize;
  uint8_t* settingBufferPtr;
  
  /* If there is no change in the settings buffer so we don't need to back up*/
  if (otThreadSettingsChanged == false)
  {
    return ;
  }
  
  settingBufferPtr = GetSettingBuffer(&settingBufferSize);
  

  /* Check if the length of the persistent data is zero */
  if ( settingBufferSize == 0u )
  {
    LOG_INFO_APP( "persistent data length is zero." );
    return ;
  }

  /* Check if the length of the persistent data exceeds the cache size */
  if ( settingBufferSize > PERSISTENCE_CACHE_SIZE_IN_BYTES )
  {
    LOG_ERROR_APP( "Error, persistent data length greater than cache size (%d).", settingBufferSize );
    return ;
  }
  
  if ( settingBufferSize != sCachePersistentData.lData[PERSISTENCE_LENGTH_OFFSET_IN_WORDS] ||
       memcmp( sCacheBuffer, &sCachePersistentData.cData[PERSISTENCE_DATA_OFFSET_IN_BYTES], settingBufferSize ) != 0 )
  {
    /* Copy the stack persistent data to cache */
    memset( sCachePersistentData.cData, 0x00, PERSISTENCE_CACHE_SIZE_IN_BYTES );
    sCachePersistentData.lData[PERSISTENCE_LENGTH_OFFSET_IN_WORDS] = settingBufferSize;
    memcpy( &sCachePersistentData.cData[PERSISTENCE_DATA_OFFSET_IN_BYTES], settingBufferPtr, settingBufferSize );

#ifdef PERSISTENCE_ENABLED
    /* Save the persistent data */
    LOG_INFO_APP("Saving persistent data");
    UTIL_SEQ_ClrEvt( EVENT_THREAD_SNVMA_WRITE_ENDED );
    eStatus = SNVMA_Write( APP_THREAD_NvmBuffer, APP_THREAD_Persistence_SnvmaCallback );
    if ( eStatus != SNVMA_ERROR_OK )
    {
      LOG_ERROR_APP( "Error, failed to save persistent data 0x%02X", eStatus );
      memset( sCachePersistentData.cData, 0x00, PERSISTENCE_CACHE_SIZE_IN_BYTES );
      return ;
    }
#endif /* PERSISTENCE_ENABLED */
  }
  UTIL_SEQ_WaitEvt(EVENT_THREAD_SNVMA_WRITE_ENDED);
  LOG_INFO_APP( "Persistent data saved (%d bytes)", settingBufferSize );

  /* Toggle the Blue LED */
  for (int var = 0; var < 5; ++var) {
    BSP_LED_Toggle(LED_BLUE);
    HAL_Delay(50U);
  }
  BSP_LED_Off(LED_BLUE);
  

}


/**
 * @brief  Initialize persistence
 * @param  None
 * @retval None
 */
void APP_THREAD_Persistence_Init(void)
{
#ifdef PERSISTENCE_ENABLED
  /* Register the cache buffer to SNVMA */
  ( void )SNVMA_Register( APP_THREAD_NvmBuffer, sCachePersistentData.lData, PERSISTENCE_CACHE_SIZE_IN_WORDS );
#endif /* PERSISTENCE_ENABLED */

  /* Clear persistence cache */
  memset( sCachePersistentData.cData, 0x00, PERSISTENCE_CACHE_SIZE_IN_BYTES );
  
  APP_THREAD_OTNvmInit();
}

/**
 * @brief  Thread persistence restore
 * @param  none 
 * @retval 'true' if startup from persistence is successful, else 'false'
 */
 void APP_THREAD_Persistence_Restore(void)
{
  bool status = false;
  otError error;
  uint32_t settingBufferSize;
  uint32_t savedBufferSize;
  uint8_t* settingBufferPtr = (uint8_t*) GetSettingBuffer(&settingBufferSize);

  /* Attempt to read the persistent data */
  status = APP_THREAD_Persistence_Load();
  if (status != true)
  {
    APP_DBG("\r\nAPP_Thread_NVM_Load Failed");
    return;
  }
  
  savedBufferSize = sCachePersistentData.lData[PERSISTENCE_LENGTH_OFFSET_IN_WORDS];
  if ( savedBufferSize > 0)
  {
    memcpy(settingBufferPtr, &sCachePersistentData.cData[PERSISTENCE_DATA_OFFSET_IN_BYTES], savedBufferSize);
    APP_DBG("\r\nAPP_Thread_NVM_Load Success, the buffer length = %d\r\n", savedBufferSize);
    

    error = FillSettingBuffer(settingBufferPtr, savedBufferSize);
    if (error != OT_ERROR_NONE)
    {
      APP_THREAD_Error(ERR_THREAD_RECOVER_PERSISTENT_INFO, error);
    }
    /* Toggle the Green LED */
    for (int var = 0; var < 5; ++var) {
      BSP_LED_Toggle(LED_GREEN);
      HAL_Delay(50U);
    }
    BSP_LED_Off(LED_GREEN);
  }
}


/**
 * @brief  Delete part of the persistent data
 * @param  None
 * @retval 'true' if successful, else 'false'
 */
bool APP_THREAD_Persistence_Delete(uint32_t pOtBufferOffset, uint32_t pOtBufferLen)
{
  
  if ((pOtBufferOffset + pOtBufferLen) > PERSISTENCE_CACHE_SIZE_IN_BYTES)
  {
    return false;
    
  }
  /* Clear persistence cache */
  memset(sCachePersistentData.cData + pOtBufferOffset, 0x00, pOtBufferLen);

#ifdef PERSISTENCE_ENABLED
  /* Clear flash */
  UTIL_SEQ_ClrEvt( EVENT_THREAD_SNVMA_WRITE_ENDED );
  SNVMA_Write( APP_THREAD_NvmBuffer, APP_THREAD_Persistence_SnvmaCallback );
#endif /* PERSISTENCE_ENABLED */
  
  return true;
}

/**
 * @brief  Delete persistent data
 * @param  None
 * @retval None
 */
void APP_THREAD_Persistence_Earse( void )
{
  /* Clear persistence cache */
  memset( sCachePersistentData.cData, 0x00, PERSISTENCE_CACHE_SIZE_IN_BYTES );

#ifdef PERSISTENCE_ENABLED
  /* Clear flash */
  UTIL_SEQ_ClrEvt( EVENT_THREAD_SNVMA_WRITE_ENDED );
  SNVMA_Write( APP_THREAD_NvmBuffer, APP_THREAD_Persistence_SnvmaCallback );
#endif /* PERSISTENCE_ENABLED */
  
  UTIL_SEQ_WaitEvt(EVENT_THREAD_SNVMA_WRITE_ENDED);
  LOG_INFO_APP( "Persistent data Erased");
}

static void processNVM(void * arg)
{
  UTIL_SEQ_SetTask(1U << CFG_TASK_OT_NVM, TASK_PRIO_NVM);
  
#if (DEBUG_NVM_BUFFER == 1)
  printThreadPersistentBuf();
#endif /*(DEBUG_NVM_BUFFER == 1) */
}

static void APP_THREAD_OTNvmInit(void)
{
  UTIL_SEQ_RegTask(1U << CFG_TASK_OT_NVM, UTIL_SEQ_RFU, APP_THREAD_Persistence_Save);
  
  UTIL_TIMER_Create(&nvmTimerId, NVM_TIMER_PERIOD, UTIL_TIMER_PERIODIC, processNVM, NULL); 
  
  UTIL_TIMER_Start( &nvmTimerId);
}

/**
 * @brief Notification that the settings is updated (Callback from the Openthread Flash driver).
 * @param  type of settings change
 * @retval None
 */
void APP_THREAD_SettingsUpdated(settings_type_t SettingType)
{
   
   if (SettingType == SETTINGS_MASSERASE)
   {
     APP_THREAD_Persistence_Earse();
   }
   else
   {
     otThreadSettingsChanged = true;
   }
}

#if (DEBUG_NVM_BUFFER == 1)
/**
 * @brief Print the Openthread NVM Buffer from Flash.
 * @param  None
 * @retval None
 */
static void printThreadPersistentBuf(void)
{
  uint32_t settingBufferSize;
  char logBuff[4000];
  uint32_t logBuffSize = 0;
  uint8_t* settingBufferPtr = (uint8_t*) GetSettingBuffer(&settingBufferSize);
  uint32_t idx;

  /* settingBufferSize will be 0, but we need to reterive all the buffer */
  settingBufferSize = GetSettingsBuffer_MaxSize();
  
  APP_DBG("-------------- Persistent Buffer --------------\n");
  
  for ( idx = 0 ; idx < settingBufferSize; idx++)
  {
    logBuffSize += sprintf(logBuff + logBuffSize, "%x ", settingBufferPtr[idx]);
  }
  logBuff[logBuffSize] = '\0';
  APP_DBG("%s\n------------------------------------------------\n",logBuff);
}
#endif /* (DEBUG_NVM_BUFFER == 1) */