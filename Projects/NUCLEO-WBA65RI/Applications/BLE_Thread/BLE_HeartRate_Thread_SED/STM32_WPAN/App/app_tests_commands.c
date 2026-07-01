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
#ifdef CFG_BLE_ADV_CHANNEL_MAP
#include "app_ble.h"
#endif // CFG_BLE_ADV_CHANNEL_MAP
#include "log_module.h"
#include "common_types.h"
#include "cli.h"
#include "coap.h"
#include "tasklet.h"
#include "thread.h"
#include "joiner.h"
#include "alarm.h"
#include "stm32_timer.h"

/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_bsp.h"

#ifdef CFG_COAP_MSG
extern uint32_t             APP_Thread_TransmitPeriod_ms, APP_Thread_CoapPayloadLength_byte;
extern otCoapType           APP_Thread_CoapType;
#endif /* CFG_COAP_MSG */
#ifdef APP_THREAD_PERIODIC_TRANSMIT
extern UTIL_TIMER_Object_t  APP_Thread_transmitTimerId;
#endif /* APP_THREAD_PERIODIC_TRANSMIT */
#ifdef CFG_BLE_ADV_CHANNEL_MAP
extern uint8_t              APP_BLE_AdvChannelMap;
#endif /* CFG_BLE_ADV_CHANNEL_MAP */

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
#ifdef CFG_COAP_MSG
static void  AppTestCommand_CoapMsgRateAction        ( void );
#endif /* CFG_COAP_MSG */
#ifdef CFG_BLE_ADV_CHANNEL_MAP
static void  AppTestCommand_BleAdvChannelMapAction   ( void );
#endif /* CFG_BLE_ADV_CHANNEL_MAP */

/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private variables -----------------------------------------------*/

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Functions Definition ------------------------------------------------------*/

/**
 * @brief   Initialisation of CLI commands tasks.
 */
void AppTestCommand_Init( void )
{
#ifdef CFG_COAP_MSG
  /* Task associated with COAP message rate change */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_COAP_MSG_RATE, UTIL_SEQ_RFU, AppTestCommand_CoapMsgRateAction );
#endif /* CFG_COAP_MSG */
#ifdef CFG_BLE_ADV_CHANNEL_MAP
  UTIL_SEQ_RegTask( 1U << CFG_TASK_BLE_ADV_CHANNEL_MAP, UTIL_SEQ_RFU, AppTestCommand_BleAdvChannelMapAction );
#endif /* CFG_BLE_ADV_CHANNEL_MAP */
}

/**
 *
 */
bool AppTestCommand_Dispatch( uint8_t * pRxBuffer, uint16_t iRxBufferSize )
{
  bool      bReturn = false;
#if defined(CFG_COAP_MSG) || defined(CFG_BLE_ADV_CHANNEL_MAP)
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

#ifdef CFG_COAP_MSG
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
        APP_Thread_CoapType = OT_COAP_TYPE_NON_CONFIRMABLE;

        LOG_INFO_APP( "Coap type has been changed to OT_COAP_TYPE_NON_CONFIRMABLE" );
        bReturn = true;
      }
    }
#endif /* CFG_COAP_MSG */
#ifdef CFG_BLE_ADV_CHANNEL_MAP
    if ( strcmp( szConfigType, "ble adv channel bitmap " ) == 0 )
    {
      iLength = iRxBufferSize - ( (pszDelimiter + 2u) - (char const*)pRxBuffer );
      strncpy( szValue, ( pszDelimiter + 2u ), iLength );
      szValue[iLength] = '\0';
      
      APP_BLE_AdvChannelMap = (uint8_t)strtol( szValue, NULL, 16 );
      
      LOG_INFO_APP( "BLE adv channel bitmap has been changed to 0x%02X", APP_BLE_AdvChannelMap );
      bReturn = true;

      UTIL_SEQ_SetTask( 1U << CFG_TASK_BLE_ADV_CHANNEL_MAP, CFG_SEQ_PRIO_0 );
    }
#endif /* CFG_BLE_ADV_CHANNEL_MAP */
  }
#endif /* defined(CFG_COAP_MSG) || defined(CFG_BLE_ADV_CHANNEL_MAP) */

  return bReturn;
}


/**
 * @brief Task associated to the COAP message rate change command.
 * @param  None
 * @retval None
 */
#ifdef CFG_COAP_MSG

static void AppTestCommand_CoapMsgRateAction( void )
{
#ifdef APP_THREAD_PERIODIC_TRANSMIT
  /* If timer for periodic CoAp msg transmission is running, then restart it with the new configured period */
  if ( UTIL_TIMER_IsRunning( &APP_Thread_transmitTimerId ) == 1U )
  {
    UTIL_TIMER_Stop( &APP_Thread_transmitTimerId );
    UTIL_TIMER_StartWithPeriod( &APP_Thread_transmitTimerId, APP_Thread_TransmitPeriod_ms );
  }
#endif
}

#endif /* CFG_COAP_MSG */
#ifdef CFG_BLE_ADV_CHANNEL_MAP

/**
 * @brief Task associated to the BLE advertising channel map change command.
 * @param  None
 * @retval None
 */
static void AppTestCommand_BleAdvChannelMapAction(void)
{
  uint16_t Advertising_Interval_Min = 0;
  uint16_t Advertising_Interval_Max = 0;
  APP_BLE_ConnStatus_t  eStatus;

  eStatus = APP_BLE_Get_Server_Connection_Status();
  if ( ( eStatus == APP_BLE_ADV_FAST ) || ( eStatus == APP_BLE_ADV_LP ) )
  {
    if ( eStatus == APP_BLE_ADV_FAST )
    {
      LOG_INFO_APP("Fast advertising is ongoing\n");
      Advertising_Interval_Min = ADV_INTERVAL_MIN;
      Advertising_Interval_Max = ADV_INTERVAL_MAX;
    }
    else
    {
      if ( eStatus == APP_BLE_ADV_LP )
      {
        LOG_INFO_APP("LP advertising is ongoing\n");
        Advertising_Interval_Min = ADV_LP_INTERVAL_MIN;
        Advertising_Interval_Max = ADV_LP_INTERVAL_MAX;
      }
    }
    
    APP_BLE_ChangeAdvChannelMap( Advertising_Interval_Min, Advertising_Interval_Max );
  }
}

#endif /* CFG_BLE_ADV_CHANNEL_MAP */

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */


/* USER CODE END FD_LOCAL_FUNCTIONS */

