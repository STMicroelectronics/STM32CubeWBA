/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_zigbee_endpoint.c
  * Description        : Zigbee Application to manage endpoints and these clusters.
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
#include <assert.h>
#include <stdint.h>

#include "app_conf.h"
#include "app_common.h"
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
#include "stm32wbaxx_nucleo.h"

/* USER CODE END PI */

/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_CHANNEL                13u
#define APP_ZIGBEE_CHANNEL_MASK           ( 1u << APP_ZIGBEE_CHANNEL )
#define APP_ZIGBEE_TX_POWER               ((int8_t) 10)    /* TX-Power is at +10 dBm. */

#define APP_ZIGBEE_ENDPOINT               17u
#define APP_ZIGBEE_PROFILE_ID             ZCL_PROFILE_HOME_AUTOMATION
#define APP_ZIGBEE_DEVICE_ID              ZCL_DEVICE_HOME_GATEWAY
#define APP_ZIGBEE_GROUP_ADDRESS          0x0001u

#define APP_ZIGBEE_CLUSTER_ID             ZCL_CLUSTER_MESSAGING
#define APP_ZIGBEE_CLUSTER_NAME           "Messaging Server"

/* USER CODE BEGIN PD */
#define APP_ZIGBEE_STARTUP_FAIL_DELAY     500u

#define APP_ZIGBEE_APPLICATION_NAME       APP_ZIGBEE_CLUSTER_NAME
#define APP_ZIGBEE_APPLICATION_OS_NAME    "."

#define MESSAGE_IMMEDIATE_ID              0x01u
#define MESSAGE_DELAYED_ID                0x02u
#define MESSAGE_MAX_ID                    ( MESSAGE_DELAYED_ID + 1u )
/* USER CODE END PD */

// -- Redefine Clusters to better code read --
#define MessagingServer                   pstZbCluster[0]

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private constants ---------------------------------------------------------*/
/* USER CODE BEGIN PC */
const uint8_t  szMessageImmediate[] = "Hello";
const uint8_t  szMessageDelayed[]   = "GoodBye";

/* USER CODE END PC */

/* Private variables ---------------------------------------------------------*/
static uint16_t   iDeviceShortAddress;

/* USER CODE BEGIN PV */
static struct ZbZclMsgMessageT          stImmediateMessage;
static struct ZbZclMsgMessageT          stDelayedMessage;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* Messaging Server Callbacks */
static enum ZclStatusCodeT APP_ZIGBEE_MessagingServerGetLastMessageCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclAddrInfoT * pstSrcInfo );
static enum ZclStatusCodeT APP_ZIGBEE_MessagingServerMessageConfirmationCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclMsgMessageConfT * pstMessageConfirm, struct ZbZclAddrInfoT * pstSrcInfo);
static enum ZclStatusCodeT APP_ZIGBEE_MessagingServerGetMessageCancellationCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclMsgGetMsgCancellationT * pstRequest, struct ZbZclAddrInfoT * pstSrcInfo );

static struct ZbZclMsgServerCallbacksT stMessagingServerCallbacks =
{
  .get_last_message = APP_ZIGBEE_MessagingServerGetLastMessageCallback,
  .message_confirmation = APP_ZIGBEE_MessagingServerMessageConfirmationCallback,
  .get_message_cancellation = APP_ZIGBEE_MessagingServerGetMessageCancellationCallback,
};

/* USER CODE BEGIN PFP */
static void APP_ZIGBEE_MessagingServerStart         ( void );

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
  stZigbeeAppInfo.eStartupControl = ZbStartTypeJoin;
  stZigbeeAppInfo.bPersistNotification = false;
  stZigbeeAppInfo.bNwkStartup = true;

  /* USER CODE BEGIN APP_ZIGBEE_ApplicationInit */

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
  uint16_t  iShortAddress;

  /* Start Messaging Server */
  APP_ZIGBEE_MessagingServerStart();

  /* Display Short Address */
  iShortAddress = ZbShortAddress( stZigbeeAppInfo.pstZigbee );
  LOG_INFO_APP( "Use Short Address : 0x%04X", iShortAddress );

  LOG_INFO_APP( "%s ready to work !", APP_ZIGBEE_APPLICATION_NAME );
  /* USER CODE END APP_ZIGBEE_ApplicationStart */

#if ( CFG_LPM_LEVEL != 0)
  /* Authorize LowPower now */
  UTIL_LPM_SetStopMode( 1 << CFG_LPM_APP, UTIL_LPM_ENABLE );
#if (CFG_LPM_STDBY_SUPPORTED == 1)
  UTIL_LPM_SetOffMode( 1 << CFG_LPM_APP, UTIL_LPM_ENABLE );
#endif /* CFG_LPM_STDBY_SUPPORTED */
#endif /* CFG_LPM_LEVEL */
}

/**
 * @brief  Zigbee persistence startup
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_PersistenceStartup(void)
{
  /* Not used */
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

  /* Add Messaging Server Cluster */
  stZigbeeAppInfo.MessagingServer = ZbZclMsgServerAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT, &stMessagingServerCallbacks, NULL );
  assert( stZigbeeAppInfo.MessagingServer != NULL );
  ZbZclClusterEndpointRegister( stZigbeeAppInfo.MessagingServer );

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
  iDeviceShortAddress = iShortAddress;
  LOG_INFO_APP( "New Device (%d) on Network : with Extended ( 0x%016" PRIX64 " ) and Short ( 0x%04" PRIX16 " ) Address.", cCapability, dlExtendedAddress, iDeviceShortAddress );

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
  LOG_INFO_APP( "Network config : CENTRALIZED ROUTER" );

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
 * @brief  Messaging Server 'GetLastMessage' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_MessagingServerGetLastMessageCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclAddrInfoT * pstSrcInfo )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_MessagingServerGetLastMessageCallback */

  /* USER CODE END APP_ZIGBEE_MessagingServerGetLastMessageCallback */
  return eStatus;
}

/**
 * @brief  Messaging Server 'MessageConfirmation' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_MessagingServerMessageConfirmationCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclMsgMessageConfT * pstMessageConfirm, struct ZbZclAddrInfoT * pstSrcInfo)
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_MessagingServerMessageConfirmationCallback */

  /* Display Confirmation Message */
  LOG_INFO_APP( "Message ID '%d' is confirmed.", pstMessageConfirm->message_id );
      
  /* USER CODE END APP_ZIGBEE_MessagingServerMessageConfirmationCallback */
  return eStatus;
}

/**
 * @brief  Messaging Server 'GetMessageCancellation' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_MessagingServerGetMessageCancellationCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclMsgGetMsgCancellationT * pstRequest, struct ZbZclAddrInfoT * pstSrcInfo )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_MessagingServerGetMessageCancellationCallback */

  /* USER CODE END APP_ZIGBEE_MessagingServerGetMessageCancellationCallback */
  return eStatus;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

/**
 * @brief  MeterId Server Start
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_MessagingServerStart(void)
{
  /* Initialize Messages. Note : Message length is limited at 59 bytes */
  
  /* First message is displayed immediately without confirmation */
  stImmediateMessage.message_id = MESSAGE_IMMEDIATE_ID;
  stImmediateMessage.message_control = ZCL_MESSAGE_CTRL_IMPORT_HIGH;    /* High priority message */
  stImmediateMessage.start_time = 0x00;         /* Display message now */
  stImmediateMessage.duration=  1;              /* Display message for 1 mn */
  stImmediateMessage.extended_control = 0x00;
  stImmediateMessage.message_str = (uint8_t *)szMessageImmediate;
  stImmediateMessage.message_len = sizeof(szMessageImmediate);
    
  /* Second message is displayed 10 second after reception (we have no RTC so it's a delay time and not an absolute time) without confirmation */
  stDelayedMessage.message_id = MESSAGE_DELAYED_ID;
  stDelayedMessage.message_control = ZCL_MESSAGE_CTRL_IMPORT_HIGH;   /* High priority message */
  stDelayedMessage.start_time = 10000;       /* Display message 10 second after reception  */
  stDelayedMessage.duration = 1;             /* Display message for 1 mn */
  stDelayedMessage.extended_control = 0x00;
  stDelayedMessage.message_str = (uint8_t *)szMessageDelayed;
  stDelayedMessage.message_len = sizeof(szMessageDelayed);
}

/**
 * @brief Send an 'immediate message' with a confirmation.
 * @param  None
 * @retval None
 */
void APPE_Button1Action( void )
{
  struct ZbApsAddrT   stDest;
  enum ZclStatusCodeT eStatus;
  
  /* First, verify if Appli has already Join a Network  */ 
  if ( APP_ZIGBEE_IsAppliJoinNetwork() != false )
  {
    /* Set destination (Coordinator) */
    memset( &stDest, 0, sizeof( stDest ) );
    stDest.mode = ZB_APSDE_ADDRMODE_SHORT;
    stDest.endpoint = APP_ZIGBEE_ENDPOINT;
    stDest.nwkAddr = 0x0000;
    
    /* Update Confirmation field in function of if button long pressed or not */
    if ( APPE_ButtonIsLongPressed( B1 ) != 0u )
    {
      stImmediateMessage.message_control |= ZCL_MESSAGE_CTRL_CONF;
      LOG_INFO_APP( "SW1 PUSHED : Sending '%s' message (with confirmation) to Client", stImmediateMessage.message_str );
    }
    else
    {
      stImmediateMessage.message_control &= ~ZCL_MESSAGE_CTRL_CONF;
      LOG_INFO_APP( "SW1 PUSHED : Sending '%s' message to Client", stImmediateMessage.message_str );
    }
    
    eStatus = ZbZclMsgServerDisplayMessageUnsolic( stZigbeeAppInfo.MessagingServer, &stDest, &stImmediateMessage, NULL, NULL);
    if ( eStatus != ZCL_STATUS_SUCCESS ) 
    {
      LOG_ERROR_APP( "Error, ZbZclMsgServerDisplayMessageUnsolic failed (0x%02X)", eStatus );
    }
  }
}

/**
 * @brief Send a 'delayed message' without a confirmation.
 * @param  None
 * @retval None
 */
void APPE_Button2Action( void )
{
  struct ZbApsAddrT   stDest;
  enum ZclStatusCodeT eStatus;
  
  /* First, verify if Appli has already Join a Network  */ 
  if ( APP_ZIGBEE_IsAppliJoinNetwork() != false )
  {
    /* Set destination (Coordinator) */
    memset( &stDest, 0, sizeof( stDest ) );
    stDest.mode = ZB_APSDE_ADDRMODE_SHORT;
    stDest.endpoint = APP_ZIGBEE_ENDPOINT;
    stDest.nwkAddr = 0x0000;
    
    if ( APPE_ButtonIsLongPressed( B2 ) != 0 )
    {
      stDelayedMessage.message_control |= ZCL_MESSAGE_CTRL_CONF;
      LOG_INFO_APP( "SW2 PUSHED : Sending '%s' message (with confirmation) to Client (display in %d ms)", stDelayedMessage.message_str, stDelayedMessage.start_time );
    }
    else
    {
      stDelayedMessage.message_control &= ~ZCL_MESSAGE_CTRL_CONF;
      LOG_INFO_APP( "SW2 PUSHED : Sending '%s' message to Client (display in %d ms)", stDelayedMessage.message_str, stDelayedMessage.start_time );
    }

    eStatus = ZbZclMsgServerDisplayMessageUnsolic( stZigbeeAppInfo.MessagingServer, &stDest, &stDelayedMessage, NULL, NULL);
    if ( eStatus != ZCL_STATUS_SUCCESS) 
    {
      LOG_ERROR_APP( "Error, ZbZclMsgServerDisplayMessageReq failed (0x%02X)", eStatus );
    }
  }
}


/**
 * @brief Cancel the 'delayed message' (if possible).
 * @param  None
 * @retval None
 */
void APPE_Button3Action( void )
{
  struct ZbApsAddrT             stDest;
  struct ZbZclMsgMessageCancelT stCancelMessage; 
  enum ZclStatusCodeT           eStatus;
  
  /* First, verify if Appli has already Join a Network  */ 
  if ( APP_ZIGBEE_IsAppliJoinNetwork() != false )
  {
    /* Set destination (Coordinator) */
    memset( &stDest, 0, sizeof( stDest ) );
    stDest.mode = ZB_APSDE_ADDRMODE_SHORT;
    stDest.endpoint = APP_ZIGBEE_ENDPOINT;
    stDest.nwkAddr = 0x0000;
    
    /* Set Cancel Message */
    stCancelMessage.message_id = stDelayedMessage.message_id;
    stCancelMessage.control = 0;

    LOG_INFO_APP( "SW3 PUSHED : Cancel '%s' message to Client", stDelayedMessage.message_str );
    eStatus = ZbZclMsgServerCancelMessageReq( stZigbeeAppInfo.MessagingServer, &stDest, &stCancelMessage, NULL, NULL);
    if ( eStatus != ZCL_STATUS_SUCCESS) 
    {
      LOG_ERROR_APP( "Error, ZbZclMsgServerCancelMessageReq failed (0x%02X)", eStatus );
    }
  }
}


/* USER CODE END FD_LOCAL_FUNCTIONS */
