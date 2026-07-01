/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * File Name          : app_tests_commands.c
  * Description        : CLI COmmands to test Concurrents Modes (BLE/Thread/Zigbee)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include <stdint.h>
#include <stdbool.h>

#include "app_conf.h"
#include "app_common.h"
#include "app_entry.h"
#include "app_thread.h"
#include "log_module.h"
#include "common_types.h"
#include "cli.h"
#include "coap.h"
#include "tasklet.h"
#include "thread.h"
#include "joiner.h"
#include "alarm.h"

/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_bsp.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private variables -----------------------------------------------*/

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Functions Definition ------------------------------------------------------*/

/**
 *
 */
bool AppTestCommand_Dispatch( uint8_t * pRxBuffer, uint16_t iRxBufferSize )
{
  bool      bReturn = false;
#if (defined(CFG_COAP_MSG) && (CFG_COAP_MSG == 1)) || (defined(CFG_BLE_ADV_CHANNEL_MAP) && (CFG_BLE_ADV_CHANNEL_MAP == 1))
  /* Coap message configuration */
  char      * pszDelimiter = NULL;
  char      szDelimiter[] = "=";
  char      szConfigType[20] = {0};
  char      szValue[30] = {0};
  uint32_t  iLength;

  /* Extract the configuration type */
  LOG_INFO_APP( "\"%s\" is received by serial command.", pRxBuffer );
      
  pszDelimiter = strstr( (char const*)pRxBuffer, szDelimiter );
  if ( pszDelimiter != NULL )
  {
    iLength = ( pszDelimiter - (char const*)pRxBuffer );
    strncpy( szConfigType, (char const*)pRxBuffer, iLength );
    szConfigType[iLength] = '\0';

#if (defined(CFG_COAP_MSG) && (CFG_COAP_MSG == 1))
    if (strcmp( szConfigType, "coap msg rate " ) == 0 )
    {
      iLength = iRxBufferSize - ( ( pszDelimiter + 2u ) - (char const*)pRxBuffer );
      APP_Thread_TransmitPeriod_ms = atoi( strncpy(szValue, (pszDelimiter + 2u), iLength ) );
      
      LOG_INFO_APP( "Transmission rate has been changed to 1msg/%dms", APP_Thread_TransmitPeriod_ms );
      bReturn = true;
      
      UTIL_SEQ_SetTask( 1U << CFG_TASK_COAP_MSG_RATE, CFG_SEQ_PRIO_0 );
    }
    else if ( (strcmp( szConfigType, "coap msg size " ) == 0) )
    {
      iLength = iRxBufferSize - ( ( pszDelimiter + 2u ) - (char const*)pRxBuffer );
      APP_Thread_CoapPayloadLength_byte = atoi( strncpy( szValue, ( pszDelimiter + 2u ), iLength ) );

      LOG_INFO_APP( "Coap payload length has been changed to %d", APP_Thread_CoapPayloadLength_byte );
      bReturn = true;
    }
    else if ( strcmp( szConfigType, "coap msg type " ) == 0 )
    {
      iLength = ( iRxBufferSize - ( ( pszDelimiter + 2u ) - (char const*)pRxBuffer ) ) + 1u;
      strncpy(szValue, (pszDelimiter + 2), (iRxBufferSize - ( ( pszDelimiter + 2u ) - (char const*)pRxBuffer ) ) );
      szValue[iLength] = '\0';
      
      if (strcmp( szValue, "OT_COAP_TYPE_CONFIRMABLE" ) == 0 )
      {
        APP_Thread_CoapType = OT_COAP_TYPE_CONFIRMABLE;
        
        LOG_INFO_APP( "Coap type has been changed to OT_COAP_TYPE_CONFIRMABLE" );
        bReturn = true;
      }
      else if ( strcmp(szValue, "OT_COAP_TYPE_NON_CONFIRMABLE") == 0 )
      {
        APP_Thread_CoapType =  OT_COAP_TYPE_NON_CONFIRMABLE;

        LOG_INFO_APP( "Coap type has been changed to OT_COAP_TYPE_NON_CONFIRMABLE" );
        bReturn = true;
      }
    }
#endif /* defined(CFG_COAP_MSG) && (CFG_COAP_MSG == 1) */
#if (defined(CFG_BLE_ADV_CHANNEL_MAP) && (CFG_BLE_ADV_CHANNEL_MAP == 1))
    if ( strcmp( szConfigType, "ble adv channel bitmap " ) == 0 )
    {
      iLength = iRxBufferSize - ( (pszDelimiter + 2u) - (char const*)pRxBuffer );
      strncpy( szValue, ( pszDelimiter + 2u ), iLength );
      szValue[iLength] = '\0';
      
      APP_BLE_AdvChannelMap = strtol( szValue, NULL, 16 );
      
      LOG_INFO_APP( "BLE adv channel bitmap has been changed to 0x%02X", APP_BLE_AdvChannelMap );
      bReturn = true;

      UTIL_SEQ_SetTask( 1U << CFG_TASK_BLE_ADV_CHANNEL_MAP, CFG_SEQ_PRIO_0 );
    }
#endif /* defined(CFG_BLE_ADV_CHANNEL_MAP) && (CFG_BLE_ADV_CHANNEL_MAP == 1) */
  }
#endif /* (defined(CFG_COAP_MSG) && (CFG_COAP_MSG == 1)) || (defined(CFG_BLE_ADV_CHANNEL_MAP) && (CFG_BLE_ADV_CHANNEL_MAP == 1)) */

  return bReturn;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */


/* USER CODE END FD_LOCAL_FUNCTIONS */

