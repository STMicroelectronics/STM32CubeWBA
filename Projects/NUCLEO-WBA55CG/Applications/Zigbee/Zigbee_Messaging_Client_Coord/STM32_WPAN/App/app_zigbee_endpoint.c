/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_zigbee_endpoint.c
  * Description        : Zigbee Application to manage endpoints and these clusters.
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
#include <stdint.h>

#include "app_common.h"
#include "app_conf.h"
#include "log_module.h"
#include "app_entry.h"
#include "app_zigbee.h"
#include "dbg_trace.h"
#include "ieee802154_enums.h"
#include "mcp_enums.h"

#include "stm32_lpm.h"
#include "stm32_rtos.h"
#include "stm32_timer.h"

#include "zigbee.h"
#include "zigbee.nwk.h"
#include "zigbee.security.h"

/* Private includes -----------------------------------------------------------*/
#include "zcl/zcl.h"
#include "zcl/se/zcl.message.h"

/* USER CODE BEGIN PI */
#include "app_bsp.h"

/* USER CODE END PI */

/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_CHANNEL                13u
#define APP_ZIGBEE_CHANNEL_MASK           ( 1u << APP_ZIGBEE_CHANNEL )
#define APP_ZIGBEE_TX_POWER               ((int8_t) 10)    /* TX-Power is at +10 dBm. */

#define APP_ZIGBEE_ENDPOINT               17u
#define APP_ZIGBEE_PROFILE_ID             ZCL_PROFILE_HOME_AUTOMATION
#define APP_ZIGBEE_DEVICE_ID              ZCL_DEVICE_METER_INTERFACE
#define APP_ZIGBEE_GROUP_ADDRESS          0x0001u

#define APP_ZIGBEE_CLUSTER_ID             ZCL_CLUSTER_MESSAGING
#define APP_ZIGBEE_CLUSTER_NAME           "Messaging Client"

/* USER CODE BEGIN PD */
#define APP_ZIGBEE_STARTUP_FAIL_DELAY     500u

#define APP_ZIGBEE_APPLICATION_NAME       APP_ZIGBEE_CLUSTER_NAME
#define APP_ZIGBEE_APPLICATION_OS_NAME    "."

#define CFG_TASK_ZIGBEE_APP_MESSAGE_CONFIRMATION         CFG_TASK_ZIGBEE_APP1
#define TASK_ZIGBEE_APP_MESSAGE_CONFIRMATION_PRIORITY    CFG_SEQ_PRIO_1

/* USER CODE END PD */

// -- Redefine Clusters to better code read --
#define MessagingClient                   pstZbCluster[0]

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private constants ---------------------------------------------------------*/

/* USER CODE BEGIN PC */

/* USER CODE END PC */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
static struct ZbZclMsgMessageT      stCurrentReceivedMessage;
static struct ZbZclAddrInfoT        stCurrentSourceMessage;
static struct ZbZclMsgConfirmT      stCurrentMsgConfirm;
static UTIL_TIMER_Object_t          stDisplayTimerId;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* Messaging Client Callbacks */
static enum ZclStatusCodeT  APP_ZIGBEE_MessagingClientDisplayMessageCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclMsgMessageT * pstMessage, struct ZbZclAddrInfoT * pstSrcInfo );
static enum ZclStatusCodeT  APP_ZIGBEE_MessagingClientCancelMessageCallback ( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclMsgMessageCancelT * pstMessageCancel, struct ZbZclAddrInfoT * pstSrcInfo );
static enum ZclStatusCodeT  APP_ZIGBEE_MessagingClientCancelAllMessagesCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclMsgMessageCancelAllT * pstMessageCancelAll, struct ZbZclAddrInfoT * pstSrcInfo );
static enum ZclStatusCodeT  APP_ZIGBEE_MessagingClientDisplayProtectedMessageCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclMsgMessageT * pstMessage, struct ZbZclAddrInfoT * pstSrcInfo );

static struct ZbZclMsgClientCallbacksT stMessagingClientCallbacks =
{
  .display_message = APP_ZIGBEE_MessagingClientDisplayMessageCallback,
  .cancel_message = APP_ZIGBEE_MessagingClientCancelMessageCallback,
  .cancel_all_messages = APP_ZIGBEE_MessagingClientCancelAllMessagesCallback,
  .display_protected_message = APP_ZIGBEE_MessagingClientDisplayProtectedMessageCallback,
};

/* USER CODE BEGIN PFP */
static void APP_ZIGBEE_ApplicationTaskInit        ( void );
static void APP_ZIGBEE_DisplayDelayingMessage     ( void * arg );
static void APP_ZIGBEE_MessageConfirmation        ( void );

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/

/**
 * @brief  Zigbee application initialization
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_ApplicationInit(void)
{
  LOG_INFO_APP( "ZIGBEE Application Init" );

  /* Initialization of the Zigbee stack */
  APP_ZIGBEE_Init();

  /* Configure Application Form/Join parameters : Startup, Persistence and Start with/without Form/Join */
  stZigbeeAppInfo.eStartupControl = ZbStartTypeForm;
  stZigbeeAppInfo.bPersistNotification = false;
  stZigbeeAppInfo.bNwkStartup = true;

  /* USER CODE BEGIN APP_ZIGBEE_ApplicationInit */
  /* Initialization of used Tasks */
  APP_ZIGBEE_ApplicationTaskInit();
  
  /* USER CODE END APP_ZIGBEE_ApplicationInit */

  /* Initialize Zigbee stack layers */
  APP_ZIGBEE_StackLayersInit();
}

/**
 * @brief  Zigbee application start
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_ApplicationStart( void )
{
  /* USER CODE BEGIN APP_ZIGBEE_ApplicationStart */
  /* Display Short Address */
  LOG_INFO_APP( "Use Short Address : 0x%04X", ZbShortAddress( stZigbeeAppInfo.pstZigbee ) );
  LOG_INFO_APP( "%s ready to work !", APP_ZIGBEE_APPLICATION_NAME );

  /* USER CODE END APP_ZIGBEE_ApplicationStart */

#if ( CFG_LPM_LEVEL != 0)
  /* Authorize LowPower now */
  UTIL_LPM_SetStopMode( 1 << CFG_LPM_APP, UTIL_LPM_ENABLE );
#if (CFG_LPM_STDBY_SUPPORTED > 0)
  UTIL_LPM_SetOffMode( 1 << CFG_LPM_APP, UTIL_LPM_ENABLE );
#endif /* CFG_LPM_STDBY_SUPPORTED */
#endif /* CFG_LPM_LEVEL */
}

/**
 * @brief  Configure Zigbee application endpoints
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_ConfigEndpoints(void)
{
  struct ZbApsmeAddEndpointReqT   stRequest;
  struct ZbApsmeAddEndpointConfT  stConfig;
  /* USER CODE BEGIN APP_ZIGBEE_ConfigEndpoints1 */

  /* USER CODE END APP_ZIGBEE_ConfigEndpoints1 */

  /* Add EndPoint */
  memset( &stRequest, 0, sizeof( stRequest ) );
  memset( &stConfig, 0, sizeof( stConfig ) );

  stRequest.profileId = APP_ZIGBEE_PROFILE_ID;
  stRequest.deviceId = APP_ZIGBEE_DEVICE_ID;
  stRequest.endpoint = APP_ZIGBEE_ENDPOINT;
  ZbZclAddEndpoint( stZigbeeAppInfo.pstZigbee, &stRequest, &stConfig );
  assert( stConfig.status == ZB_STATUS_SUCCESS );

  /* Add Messaging Client Cluster */
  stZigbeeAppInfo.MessagingClient = ZbZclMsgClientAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT, &stMessagingClientCallbacks, NULL );
  assert( stZigbeeAppInfo.MessagingClient != NULL );
  if ( ZbZclClusterEndpointRegister( stZigbeeAppInfo.MessagingClient ) == false )
  {
    LOG_ERROR_APP( "Error during Messaging Client Endpoint Register." );
  }

  /* USER CODE BEGIN APP_ZIGBEE_ConfigEndpoints2 */

  /* USER CODE END APP_ZIGBEE_ConfigEndpoints2 */
}

/**
 * @brief  Set Group Addressing mode (if used)
 * @param  None
 * @retval 'true' if Group Address used else 'false'.
 */
bool APP_ZIGBEE_ConfigGroupAddr( void )
{
  struct ZbApsmeAddGroupReqT  stRequest;
  struct ZbApsmeAddGroupConfT stConfig;

  memset( &stRequest, 0, sizeof( stRequest ) );

  stRequest.endpt = APP_ZIGBEE_ENDPOINT;
  stRequest.groupAddr = APP_ZIGBEE_GROUP_ADDRESS;
  ZbApsmeAddGroupReq( stZigbeeAppInfo.pstZigbee, &stRequest, &stConfig );
  
  return true;
}

/**
 * @brief  Return the Startup Configuration
 * @param  pstConfig  Configuration structure to fill
 * @retval None
 */
void APP_ZIGBEE_GetStartupConfig( struct ZbStartupT * pstConfig )
{
  /* Attempt to join a zigbee network */
  ZbStartupConfigGetProDefaults( pstConfig );

  /* Using the default HA preconfigured Link Key */
  memcpy( pstConfig->security.preconfiguredLinkKey, sec_key_ha, ZB_SEC_KEYSIZE );

  /* Setting up additional startup configuration parameters */
  pstConfig->startupControl = stZigbeeAppInfo.eStartupControl;
  pstConfig->channelList.count = 1;
  pstConfig->channelList.list[0].page = 0;
  pstConfig->channelList.list[0].channelMask = APP_ZIGBEE_CHANNEL_MASK;

  /* Set the TX-Power */
  if ( APP_ZIGBEE_SetTxPower( APP_ZIGBEE_TX_POWER ) == false )
  {
    LOG_ERROR_APP( "Switching to %d dB failed.", APP_ZIGBEE_TX_POWER );
    return;
  }

  /* USER CODE BEGIN APP_ZIGBEE_GetStartupConfig */

  /* USER CODE END APP_ZIGBEE_GetStartupConfig */
}

/**
 * @brief  Manage a New Device on Network (called only if Coord or Router).
 * @param  iShortAddress      Short Address of new Device
 * @param  dlExtendedAddress  Extended Address of new Device
 * @param  cCapability        Capability of new Device
 * @retval Group Address
 */
void APP_ZIGBEE_SetNewDevice( uint16_t iShortAddress, uint64_t dlExtendedAddress, uint8_t cCapability )
{
  LOG_INFO_APP( "New Device (%d) on Network : with Extended ( " LOG_DISPLAY64() " ) and Short ( 0x%04X ) Address.", cCapability, LOG_NUMBER64( dlExtendedAddress ), iShortAddress );

  /* USER CODE BEGIN APP_ZIGBEE_SetNewDevice */

  /* USER CODE END APP_ZIGBEE_SetNewDevice */
}

/**
 * @brief  Print application information to the console
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_PrintApplicationInfo(void)
{
  LOG_INFO_APP( "**********************************************************" );
  LOG_INFO_APP( "Network config : CENTRALIZED COORDINATOR" );

  /* USER CODE BEGIN APP_ZIGBEE_PrintApplicationInfo1 */
  LOG_INFO_APP( "Application Flashed : Zigbee %s %s", APP_ZIGBEE_APPLICATION_NAME, APP_ZIGBEE_APPLICATION_OS_NAME );

  /* USER CODE END APP_ZIGBEE_PrintApplicationInfo1 */
  LOG_INFO_APP( "Channel used: %d.", APP_ZIGBEE_CHANNEL );

  APP_ZIGBEE_PrintGenericInfo();

  LOG_INFO_APP( "Clusters allocated are:" );
  LOG_INFO_APP( "%s on Endpoint %d.", APP_ZIGBEE_CLUSTER_NAME, APP_ZIGBEE_ENDPOINT );

  /* USER CODE BEGIN APP_ZIGBEE_PrintApplicationInfo2 */

  /* USER CODE END APP_ZIGBEE_PrintApplicationInfo2 */

  LOG_INFO_APP( "**********************************************************" );
}

/**
 * @brief  Messaging Client 'DisplayMessage' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_MessagingClientDisplayMessageCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclMsgMessageT * pstMessage, struct ZbZclAddrInfoT * pstSrcInfo )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_MessagingClientDisplayMessageCallback */
  uint8_t * szCurrentMessage = NULL;

  LOG_INFO_APP( "Message ID '%d' received from Server.", pstMessage->message_id );
  
  /* If message already received and in waiting or display, do not display it */
  if ( stCurrentReceivedMessage.message_id == pstMessage->message_id )
  {
    LOG_INFO_APP( "Message cannot be treated because already received and in waiting of display." );
  }
  else
  {
    /* If a message already received is in waiting or confirmation, do not display this */
    if ( ( ( ( pstMessage->message_control & ZCL_MESSAGE_CTRL_CONF ) != 0u ) && ( stCurrentMsgConfirm.message_id != 0 ) )
      || ( stCurrentMsgConfirm.message_id == pstMessage->message_id ) )
    {
      LOG_INFO_APP( "Message cannot be treated because message ID '%d' already in waiting of confirmation. ", stCurrentMsgConfirm.message_id );
    }
    else
    {
      /* Treatment if Confirmation is required */
      if ( ( pstMessage->message_control & ZCL_MESSAGE_CTRL_CONF ) != 0u )
      {
        /* Copy structure SourceInfo */
        memcpy( &stCurrentSourceMessage, pstSrcInfo, sizeof( struct ZbZclAddrInfoT ) );
        
        /* Set Confirm Message */
        stCurrentMsgConfirm.message_id = pstMessage->message_id;
        stCurrentMsgConfirm.confirm_time = 0x00;
      }

      /* Treatment if Start Time is delayed. We have no RTC so StartTime is the number of ms wait */
      if ( pstMessage->start_time != 0u )
      {
        /* Copy structure Message, message itself  & start Timer */
        memcpy( &stCurrentReceivedMessage, pstMessage, sizeof( struct ZbZclMsgMessageT ) );
        if ( stCurrentReceivedMessage.message_len != 0 )
        {
          szCurrentMessage = malloc( stCurrentReceivedMessage.message_len );
          memcpy( szCurrentMessage, stCurrentReceivedMessage.message_str, stCurrentReceivedMessage.message_len );
        }
        stCurrentReceivedMessage.message_str = szCurrentMessage;

        UTIL_TIMER_StartWithPeriod( &stDisplayTimerId, pstMessage->start_time );
        
        LOG_INFO_APP( "Message will be display in %d ms.", pstMessage->start_time );
      }
      else
      {
        /* Display Message directly */
        LOG_INFO_APP( "Message from Server : %s.", pstMessage->message_str );
        
        if ( stCurrentMsgConfirm.message_id == pstMessage->message_id )
        {
          LOG_INFO_APP( "Message request Confirmation. Please press Button SW1 to confirm." );
          stCurrentMsgConfirm.confirm_time = 0x01;
        }
      }
    }
  }

  /* USER CODE END APP_ZIGBEE_MessagingClientDisplayMessageCallback */
  return eStatus;
}

/**
 * @brief  Messaging Client 'CancelMessage' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_MessagingClientCancelMessageCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclMsgMessageCancelT * pstMessageCancel, struct ZbZclAddrInfoT * pstSrcInfo )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_MessagingClientCancelMessageCallback */

  /* Verify if Message to cancel is in waiting */
  if ( pstMessageCancel->message_id == stCurrentReceivedMessage.message_id )
  {
    /* If it's the case, Stop Timer & remove message */
    UTIL_TIMER_Stop( &stDisplayTimerId );
    free( stCurrentReceivedMessage.message_str );
    memset( &stCurrentReceivedMessage, 0, sizeof( struct ZbZclMsgMessageT ) );
    memset( &stCurrentMsgConfirm, 0, sizeof( struct ZbZclMsgConfirmT ) );

    LOG_INFO_APP( "Cancel Message ID '%d'.", pstMessageCancel->message_id );
  }
  else
  {
    LOG_INFO_APP( "Message ID '%d' is not present.", pstMessageCancel->message_id );            
  }
  
  /* USER CODE END APP_ZIGBEE_MessagingClientCancelMessageCallback */
  return eStatus;
}

/**
 * @brief  Messaging Client 'CancelAllMessages' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_MessagingClientCancelAllMessagesCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclMsgMessageCancelAllT * pstMessageCancelAll, struct ZbZclAddrInfoT * pstSrcInfo )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_MessagingClientCancelAllMessagesCallback */

  /* USER CODE END APP_ZIGBEE_MessagingClientCancelAllMessagesCallback */
  return eStatus;
}

/**
 * @brief  Messaging Client 'DisplayProtectedMessage' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_MessagingClientDisplayProtectedMessageCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclMsgMessageT * pstMessage, struct ZbZclAddrInfoT * pstSrcInfo )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_MessagingClientDisplayProtectedMessageCallback */

  /* USER CODE END APP_ZIGBEE_MessagingClientDisplayProtectedMessageCallback */
  return eStatus;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

/**
 * @brief  Zigbee application Task initialization
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_ApplicationTaskInit( void )
{
  /* Task associated with Door Lock passage mode */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_ZIGBEE_APP_MESSAGE_CONFIRMATION, UTIL_SEQ_RFU, APP_ZIGBEE_MessageConfirmation );
  
  /* Create Timer used if a message is delayed */
  UTIL_TIMER_Create( &stDisplayTimerId, 0, UTIL_TIMER_ONESHOT, &APP_ZIGBEE_DisplayDelayingMessage, NULL );
}

/**
 *
 */
static void APP_ZIGBEE_DisplayDelayingMessage( void * arg )
{
  /* Display delayed Message and remove it */
  LOG_INFO_APP( "Delayed message from Server : %s.", stCurrentReceivedMessage.message_str );
  if ( stCurrentMsgConfirm.message_id == stCurrentReceivedMessage.message_id )
  {
    LOG_INFO_APP( "Message request Confirmation. Please press Button SW1 to confirm." );
    stCurrentMsgConfirm.confirm_time = 0x01;
  }

  /* Remove message after display */
  free( stCurrentReceivedMessage.message_str );
  memset( &stCurrentReceivedMessage, 0, sizeof( struct ZbZclMsgMessageT ) );
}

/**
 *
 */
static void APP_ZIGBEE_MessageConfirmation( void )
{
  struct ZbApsAddrT             stDest;
  enum ZclStatusCodeT           eStatus;
     
  /* Set destination (Sender) */
  memset( &stDest, 0, sizeof( stDest ) );
  stDest.mode = ZB_APSDE_ADDRMODE_SHORT;
  stDest.endpoint = APP_ZIGBEE_ENDPOINT;
  stDest.nwkAddr = stCurrentSourceMessage.addr.nwkAddr;
  
  eStatus = ZbZclMsgClientConfReq( stZigbeeAppInfo.MessagingClient, &stDest, &stCurrentMsgConfirm, NULL, NULL);
  if ( eStatus != ZCL_STATUS_SUCCESS) 
  {
    LOG_ERROR_APP( "Error, ZbZclMsgClientConfReq failed (0x%02X)", eStatus );
  }
}

/**
 * @brief If a message with confirmation was occurs, send confirmation.
 * @param  None
 * @retval None
 */
void APP_BSP_Button1Action( void )
{
  /* First, verify if Appli has already Join a Network  */ 
  if ( APP_ZIGBEE_IsAppliJoinNetwork() != false )
  {
    if ( ( stCurrentMsgConfirm.message_id != 0u ) && ( stCurrentMsgConfirm.confirm_time != 0u ) )
    {
      LOG_INFO_APP( "SW1 PUSHED : Send confirmation for message ID '%d'.", stCurrentMsgConfirm.message_id );
      stCurrentMsgConfirm.confirm_time = 0;
      APP_ZIGBEE_MessageConfirmation();
      
      memset( &stCurrentMsgConfirm, 0, sizeof( struct ZbZclMsgConfirmT ) );
    }
  }
}

/* USER CODE END FD_LOCAL_FUNCTIONS */
