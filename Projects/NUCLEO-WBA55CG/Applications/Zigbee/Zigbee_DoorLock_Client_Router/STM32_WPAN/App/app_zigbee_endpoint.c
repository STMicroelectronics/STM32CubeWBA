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
#include "zcl/general/zcl.alarm.h"
#include "zcl/general/zcl.doorlock.h"

/* USER CODE BEGIN PI */
#include "app_bsp.h"

/* USER CODE END PI */

/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_CHANNEL                13u
#define APP_ZIGBEE_CHANNEL_MASK           ( 1u << APP_ZIGBEE_CHANNEL )
#define APP_ZIGBEE_TX_POWER               ((int8_t) 10)    /* TX-Power is at +10 dBm. */

#define APP_ZIGBEE_ENDPOINT               17u
#define APP_ZIGBEE_PROFILE_ID             ZCL_PROFILE_HOME_AUTOMATION
#define APP_ZIGBEE_DEVICE_ID              ZCL_DEVICE_DOOR_LOCK
#define APP_ZIGBEE_GROUP_ADDRESS          0x0001u

#define APP_ZIGBEE_CLUSTER1_ID            ZCL_CLUSTER_ALARM
#define APP_ZIGBEE_CLUSTER1_NAME          "Alarm Client"

#define APP_ZIGBEE_CLUSTER2_ID            ZCL_CLUSTER_DOOR_LOCK
#define APP_ZIGBEE_CLUSTER2_NAME          "DoorLock Client"

/* USER CODE BEGIN PD */
#define APP_ZIGBEE_APPLICATION_NAME       APP_ZIGBEE_CLUSTER2_NAME
#define APP_ZIGBEE_APPLICATION_OS_NAME    "."

/* DoorLock specific defines -------------------------------------------------*/
#define USER_ID                           0x1u
#define USER_STATUS                       ZCL_DRLK_USER_STATUS_OCC_ENABLED
#define USER_TYPE                         ZCL_DRLK_USER_TYPE_UNRESTRICTED
#define PIN_CODE                          "PWD"

/* USER CODE END PD */

// -- Redefine Clusters to better code read --
#define AlarmClient                       pstZbCluster[0]
#define DoorLockClient                    pstZbCluster[1]

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private constants ---------------------------------------------------------*/
/* USER CODE BEGIN PC */

/* USER CODE END PC */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static uint16_t   iUserId, iPinCodeLength;
static uint8_t    szPinCode[2u * ZCL_DRLK_MAX_PIN_LEN];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* Alarm Client Callbacks */
static void APP_ZIGBEE_AlarmClientCallback( void * arg, uint16_t iNetworkAddress, uint8_t cEndpoint, uint8_t cAlarmCode, uint16_t iClusterId );

/* USER CODE BEGIN PFP */
static void APP_ZIGBEE_DoorLockClientStart		            ( uint16_t iShortAddress );
static void APP_ZIGBEE_GenericCommandResponseCallback	    ( struct ZbZclCommandRspT *zcl_rsp, void *arg );

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

  /* Read Short Address */
  iShortAddress = ZbShortAddress( stZigbeeAppInfo.pstZigbee );

  /* Start DoorLock Client */
  APP_ZIGBEE_DoorLockClientStart( iShortAddress );

  /* Display Informations */
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
  /* USER CODE BEGIN APP_ZIGBEE_PersistenceStartup */

  /* USER CODE END APP_ZIGBEE_PersistenceStartup */
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

  /* Add Alarm Client Cluster */
  stZigbeeAppInfo.AlarmClient = ZbZclAlarmClientAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT, APP_ZIGBEE_AlarmClientCallback, NULL );
  assert( stZigbeeAppInfo.AlarmClient != NULL );
  ZbZclClusterEndpointRegister( stZigbeeAppInfo.AlarmClient );

  /* Add DoorLock Client Cluster */
  stZigbeeAppInfo.DoorLockClient = ZbZclDoorLockClientAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT );
  assert( stZigbeeAppInfo.DoorLockClient != NULL );
  ZbZclClusterEndpointRegister( stZigbeeAppInfo.DoorLockClient );

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
  LOG_INFO_APP( "Network config : CENTRALIZED ROUTER" );

  /* USER CODE BEGIN APP_ZIGBEE_PrintApplicationInfo1 */
  LOG_INFO_APP( "Application Flashed : Zigbee %s %s", APP_ZIGBEE_APPLICATION_NAME, APP_ZIGBEE_APPLICATION_OS_NAME );

  /* USER CODE END APP_ZIGBEE_PrintApplicationInfo1 */
  LOG_INFO_APP( "Channel used: %d.", APP_ZIGBEE_CHANNEL );

  APP_ZIGBEE_PrintGenericInfo();

  LOG_INFO_APP( "Clusters allocated are:" );
  LOG_INFO_APP( "%s on Endpoint %d.", APP_ZIGBEE_CLUSTER1_NAME, APP_ZIGBEE_ENDPOINT );
  LOG_INFO_APP( "%s on Endpoint %d.", APP_ZIGBEE_CLUSTER2_NAME, APP_ZIGBEE_ENDPOINT );

  /* USER CODE BEGIN APP_ZIGBEE_PrintApplicationInfo2 */

  /* USER CODE END APP_ZIGBEE_PrintApplicationInfo2 */

  LOG_INFO_APP( "**********************************************************" );
}

/**
 * @brief  Alarm Client '' command Callback
 */
static void APP_ZIGBEE_AlarmClientCallback( void * arg, uint16_t iNetworkAddress, uint8_t cEndpoint, uint8_t cAlarmCode, uint16_t iClusterId )
{

  /* USER CODE BEGIN APP_ZIGBEE_AlarmClientCallback */
  UNUSED(arg);

  switch( iClusterId )
  {
  	  case ZCL_CLUSTER_DOOR_LOCK:
          LOG_INFO_APP( "[ALARM] Alarm received from 'DoorLock' cluster (code 0x%02X) from Address 0x%04X.", cAlarmCode, iNetworkAddress );

          /* RED led on during 500ms */
          APP_LED_ON(LED_RED);
          LOG_INFO_APP("Red Led 'ON'");
          HAL_Delay(500);
          APP_LED_OFF(LED_RED);
          LOG_INFO_APP("Red Led 'OFF'");
          break;

  	  default:
          LOG_ERROR_APP( "[ALARM] Error, alarm from an unexpected cluster received (0x%04X).", iClusterId );
          break;
  }
  
  /* USER CODE END APP_ZIGBEE_AlarmClientCallback */

}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

/**
 * @brief  DoorLock client start
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_DoorLockClientStart( uint16_t iShortAddress )
{
  struct ZbApsAddrT 	  stDest;
  struct ZbZclDoorLockSetPinReqT stDoorLockSetPinReq;
  enum ZclStatusCodeT 	eStatus;
  
  LOG_INFO_APP( "[DOORLOCK] Requesting PIN code setting for the User." );

  /* Setting up the addressing mode */
  memset( &stDest, 0, sizeof( stDest ) );
  stDest.mode = ZB_APSDE_ADDRMODE_SHORT;
  stDest.endpoint = APP_ZIGBEE_ENDPOINT;
  stDest.nwkAddr = 0x0;
  
  /* Create User-ID and Pin-Code based on Short Address */
  iUserId = iShortAddress;
  snprintf( (char *)szPinCode, sizeof(szPinCode), "%s%04X", PIN_CODE, iShortAddress );
  iPinCodeLength = sizeof(szPinCode);
  if ( iPinCodeLength > ZCL_DRLK_MAX_PIN_LEN )
    { iPinCodeLength = ZCL_DRLK_MAX_PIN_LEN; }

  /* Creating a request for new user and its associated pin */
  memset( &stDoorLockSetPinReq, 0, sizeof( stDoorLockSetPinReq ) );
  stDoorLockSetPinReq.user_id = iUserId;
  stDoorLockSetPinReq.user_status = USER_STATUS;
  stDoorLockSetPinReq.user_type = USER_TYPE;
  stDoorLockSetPinReq.pin_len = iPinCodeLength;
  memcpy(stDoorLockSetPinReq.pin, szPinCode, iPinCodeLength );

  /* Send the request */
  eStatus = ZbZclDoorLockClientSetPinReq( stZigbeeAppInfo.DoorLockClient, &stDest, &stDoorLockSetPinReq, &APP_ZIGBEE_GenericCommandResponseCallback, NULL );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "Error during the send of 'Pin Code' Request (0x%02X)", eStatus );
    return;
  }
}


/**
 * @brief  DoorLock client lock command
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_DoorLockClientSendLock(void)
{
  struct ZbApsAddrT   stDest;
  struct ZbZclDoorLockLockDoorReqT stDoorLockLockDoorReq;
  enum ZclStatusCodeT eStatus;

  LOG_INFO_APP( "[DOORLOCK] Requesting a 'Lock'." );

  /* Setting up the addressing mode */
  memset( &stDest, 0, sizeof( stDest ) );
  stDest.mode = ZB_APSDE_ADDRMODE_SHORT;
  stDest.endpoint = APP_ZIGBEE_ENDPOINT;
  stDest.nwkAddr = 0x0;

  /* Creating a request for the unlock cmd */
  memset( &stDoorLockLockDoorReq, 0, sizeof( stDoorLockLockDoorReq ) );
  stDoorLockLockDoorReq.pin_len = iPinCodeLength;
  memcpy( stDoorLockLockDoorReq.pin, szPinCode, iPinCodeLength );

  /* Send the request */
  eStatus = ZbZclDoorLockClientLockReq( stZigbeeAppInfo.DoorLockClient, &stDest, &stDoorLockLockDoorReq, &APP_ZIGBEE_GenericCommandResponseCallback, NULL );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "Error during the send of 'Door Lock' request (0x%02X)", eStatus );
    return;
  }
}


/**
 * @brief  DoorLock client unlock command
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_DoorLockClientSendUnlock(void)
{
  struct ZbApsAddrT   stDest;
  struct ZbZclDoorLockUnlockDoorReqT stDoorLockUnlockDoorReq;
  enum ZclStatusCodeT eStatus;

  LOG_INFO_APP( "[DOORLOCK] Requesting an 'Unlock'." );

  /* Setting up the addressing mode */
  memset( &stDest, 0, sizeof( stDest ) );
  stDest.mode = ZB_APSDE_ADDRMODE_SHORT;
  stDest.endpoint = APP_ZIGBEE_ENDPOINT;
  stDest.nwkAddr = 0x0;

  /* Creating a request for the unlock cmd */
  memset( &stDoorLockUnlockDoorReq, 0, sizeof( stDoorLockUnlockDoorReq ) );
  stDoorLockUnlockDoorReq.pin_len = iPinCodeLength;
  memcpy( stDoorLockUnlockDoorReq.pin, szPinCode, iPinCodeLength );

  /* Send the request */
  eStatus = ZbZclDoorLockClientUnlockReq( stZigbeeAppInfo.DoorLockClient, &stDest, &stDoorLockUnlockDoorReq, &APP_ZIGBEE_GenericCommandResponseCallback, NULL );
  if ( eStatus != ZCL_STATUS_SUCCESS ) 
  {
    LOG_ERROR_APP( "Error during the send of 'Door Unlock' request (0x%02X)", eStatus );
    return;
  }
}


/**
 * @brief  DoorLock client unlock command but with a Wrong Pin
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_DoorLockClientSendUnlockWithWrongPIN(void)
{
  struct ZbApsAddrT   stDest;
  struct ZbZclDoorLockUnlockDoorReqT stDoorLockUnlockDoorReq;
  enum ZclStatusCodeT eStatus;

  LOG_INFO_APP( "[DOORLOCK] Requesting an 'Unlock' with a wrong PIN." );

  /* Setting up the addressing mode */
  memset( &stDest, 0, sizeof( stDest ) );
  stDest.mode = ZB_APSDE_ADDRMODE_SHORT;
  stDest.endpoint = APP_ZIGBEE_ENDPOINT;
  stDest.nwkAddr = 0x0;

  /* Creating a request for the unlock cmd  with a wrong pin (set to 0) */
  stDoorLockUnlockDoorReq.pin_len = 1;
  memcpy( stDoorLockUnlockDoorReq.pin, PIN_CODE, stDoorLockUnlockDoorReq.pin_len );
  memset( stDoorLockUnlockDoorReq.pin, 0, ZCL_DRLK_MAX_PIN_LEN );

  /* Send the request */
  eStatus = ZbZclDoorLockClientUnlockReq( stZigbeeAppInfo.DoorLockClient, &stDest, &stDoorLockUnlockDoorReq, &APP_ZIGBEE_GenericCommandResponseCallback, NULL );
  if ( eStatus != ZCL_STATUS_SUCCESS ) 
  {
    LOG_ERROR_APP("Error during the send of 'Door Unlock with Wrong PIN' request (0x%02X)", eStatus);
    return;
  }
}


/**
 * @brief  DoorLock client command response management
 * @param  pstResponse  ZCL response
 * @retval None
 */
static void APP_ZIGBEE_DoorLockCommandResponseManagement(struct ZbZclCommandRspT * pstResponse)
{
  uint8_t   cResponseStatus = 0;

  /* Checking response consistency */
  assert( pstResponse != NULL );
  assert( pstResponse->payload != NULL);

  memcpy( &cResponseStatus, pstResponse->payload, sizeof(cResponseStatus) );
  switch( pstResponse->hdr.cmdId )
  {
  	  case ZCL_DRLK_SVR_LOCK_RSP:
  		  /* DoorLock lock cmd response */
  		  if (cResponseStatus != ZCL_DRLK_STATUS_SUCCESS)
  		    { LOG_ERROR_APP( "[DOORLOCK] Door 'Lock' response: the server returned an error: 0x%02x.", cResponseStatus ); }
  		  else
  		  	{ LOG_INFO_APP( "[DOORLOCK] Door 'Lock' response: success!" ); }
  		  break;

      case ZCL_DRLK_SVR_UNLOCK_RSP:
    	  /* DoorLock unlock cmd response */
    	  if ( cResponseStatus != ZCL_DRLK_STATUS_SUCCESS )
    	    { LOG_ERROR_APP( "[DOORLOCK] Door 'Unlock' response: the server returned an error: 0x%02x.", cResponseStatus ); }
    	  else
    	    { LOG_INFO_APP( "[DOORLOCK] Door 'Unlock' response: success!" ); }
    	  break;

      case ZCL_DRLK_SVR_SETPIN_RSP:
    	  /* DoorLock set_pin cmd response */
    	  if ( cResponseStatus != ZCL_DRLK_STATUS_SUCCESS )
    	  	{ LOG_ERROR_APP( "[DOORLOCK] 'Set Pin Code' response: the server returned an error: 0x%02x.", cResponseStatus ); }
    	  else
    	    { LOG_INFO_APP( "[DOORLOCK] 'Set Pin Code' response: success!" ); }
    	  break;

      default:
    	  LOG_ERROR_APP( "Error: unsupported general command type." );
    	  break;
  }
}


/**
 * @brief  Device generic command response callback
 * @param  zcl_rsp: ZCL response
 * @param  arg: passed arguments
 * @retval None
 */
static void APP_ZIGBEE_GenericCommandResponseCallback(struct ZbZclCommandRspT * pstResponse, void * arg)
{
  UNUSED(arg);

  /* Checking response consistency */
  assert( pstResponse != NULL );

  switch( pstResponse->hdr.frameCtrl.frameType )
  {
  	  /* General frame command response */
  	  case ZCL_FRAMETYPE_PROFILE:
  		  switch( pstResponse->hdr.cmdId )
  		  {
  		  	  case ZCL_COMMAND_DEFAULT_RESPONSE:
  		  		  if ( pstResponse->status != ZCL_STATUS_SUCCESS )
  		  		  	{ LOG_ERROR_APP( "Default response: the server returned an error: 0x%02x.", pstResponse->status ); }
  		  		  else
                { LOG_INFO_APP( "Default response: success!" ); }
  		  		  break;

  		  	  default:
  		  		  LOG_ERROR_APP( "Error: unsupported general cmd type." );
  		  		  break;
  		  }
  		  break;

      /* Cluster specific frame command response */
  	  case ZCL_FRAMETYPE_CLUSTER:
  		  if ( pstResponse->status != ZCL_STATUS_SUCCESS )
  		  {
          LOG_ERROR_APP( "Error: cmd not successfully received. 0x%02x.\n", pstResponse->status );
          return;
  		  }

  		  switch( pstResponse->clusterId )
  		  {
  		  	  case ZCL_CLUSTER_DOOR_LOCK:
  		  		  /* DoorLock cluster */
  		  		  APP_ZIGBEE_DoorLockCommandResponseManagement( pstResponse );
  		  		  break;

  		  	  default:
  		  		  LOG_ERROR_APP( "Error: unsupported cluster." );
  		  		  break;
  		  }
  		  break;

      default:
    	  LOG_ERROR_APP( "Error: unsupported frame type." );
    	  break;
  }
}


/**
 * @brief  Management of the SW1 button (Unlock Door)
 * @param  None
 * @retval None
 */
void APP_BSP_Button1Action(void)
{
  /* First, verify if Appli has already Join a Network  */ 
  if ( APP_ZIGBEE_IsAppliJoinNetwork() != false )
  {
    APP_ZIGBEE_DoorLockClientSendUnlock();
  }
}


/**
 * @brief  Management of the SW2 button (Lock Door)
 * @param  None
 * @retval None
 */
void APP_BSP_Button2Action(void)
{
  /* First, verify if Appli has already Join a Network  */ 
  if ( APP_ZIGBEE_IsAppliJoinNetwork() != false )
  {
    APP_ZIGBEE_DoorLockClientSendLock();
  }
}


/**
 * @brief  Management of the SW3 button (Unlock Door with bad code)
 * @param  None
 * @retval None
 */
void APP_BSP_Button3Action(void)
{
  /* First, verify if Appli has already Join a Network  */ 
  if ( APP_ZIGBEE_IsAppliJoinNetwork() != false )
  {
    /* Send Door Unlock Command */
    APP_ZIGBEE_DoorLockClientSendUnlockWithWrongPIN();
  }
}

/* USER CODE END FD_LOCAL_FUNCTIONS */
