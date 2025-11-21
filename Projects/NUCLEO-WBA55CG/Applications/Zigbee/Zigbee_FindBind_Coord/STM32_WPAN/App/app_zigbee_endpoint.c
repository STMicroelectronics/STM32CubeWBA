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
#include "stm32_lpm_if.h"

#include "zigbee.h"
#include "zigbee.nwk.h"
#include "zigbee.security.h"

/* Private includes -----------------------------------------------------------*/
#include "zcl/zcl.h"
#include "zcl/general/zcl.identify.h"
#include "zcl/general/zcl.scenes.h"
#include "zcl/general/zcl.onoff.h"
#include "zcl/security/zcl.ias_wd.h"
#include "zcl/se/zcl.message.h"

/* USER CODE BEGIN PI */
#include "app_bsp.h"

/* USER CODE END PI */

/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_CHANNEL                13u
#define APP_ZIGBEE_CHANNEL_MASK           ( 1u << APP_ZIGBEE_CHANNEL )
#define APP_ZIGBEE_TX_POWER               ((int8_t) 10)    /* TX-Power is at +10 dBm. */

#define APP_ZIGBEE_ENDPOINT               10u
#define APP_ZIGBEE_PROFILE_ID             ZCL_PROFILE_HOME_AUTOMATION
#define APP_ZIGBEE_DEVICE_ID              ZCL_DEVICE_ONOFF_LIGHT
#define APP_ZIGBEE_GROUP_ADDRESS          0x0001u

#define APP_ZIGBEE_CLUSTER1_ID            ZCL_CLUSTER_IDENTIFY
#define APP_ZIGBEE_CLUSTER1_NAME          "Identify Server"

#define APP_ZIGBEE_CLUSTER2_ID            ZCL_CLUSTER_SCENES
#define APP_ZIGBEE_CLUSTER2_NAME          "Scenes Server"

#define APP_ZIGBEE_CLUSTER3_ID            ZCL_CLUSTER_ONOFF
#define APP_ZIGBEE_CLUSTER3_NAME          "OnOff Server"

#define APP_ZIGBEE_CLUSTER4_ID            ZCL_CLUSTER_IAS_WD
#define APP_ZIGBEE_CLUSTER4_NAME          "IasWd Server"

#define APP_ZIGBEE_CLUSTER5_ID            ZCL_CLUSTER_MESSAGING
#define APP_ZIGBEE_CLUSTER5_NAME          "Messaging Server"

/* Scenes specific defines ----------------------------------------------------*/
#define ZCL_SCENES_MAX_SCENES             10
/* USER CODE BEGIN Scenes defines */
/* USER CODE END Scenes defines */

/* USER CODE BEGIN PD */
#define APP_ZIGBEE_IDENTIFY_MODE_DELAY    30u     /* 30s  */

#define APP_ZIGBEE_APPLICATION_NAME       "Find-Bind Coordinator"
#define APP_ZIGBEE_APPLICATION_OS_NAME    "."

// -- Redefine tasks & priorities to better code read --
#define CFG_TASK_ZIGBEE_APP_IAS_WD_START_WARNING        CFG_TASK_ZIGBEE_APP1
#define TASK_ZIGBEE_APP_IAS_WD_START_WARNING_PRIORITY   CFG_SEQ_PRIO_0

#define CFG_TASK_ZIGBEE_APP_IAS_WD_SQUAWK               CFG_TASK_ZIGBEE_APP2
#define TASK_ZIGBEE_APP_IAS_WD_SQUAWK_PRIORITY          CFG_SEQ_PRIO_0

/* USER CODE END PD */

// -- Redefine Clusters to better code read --
#define IdentifyServer                    pstZbCluster[0]
#define ScenesServer                      pstZbCluster[1]
#define OnOffServer                       pstZbCluster[2]
#define IasWdServer                       pstZbCluster[3]
#define MessagingServer                   pstZbCluster[4]

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private constants ---------------------------------------------------------*/
/* USER CODE BEGIN PC */
const uint8_t     pszMessageBuffer[] = "ZCL Test Message";

/* USER CODE END PC */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
static bool       bIsSquawkStrobe = false;
static struct ZbZclIasWdClientStartWarningReqT  stWarningRequest;
static uint16_t   iDeviceIndex = 0;
static uint64_t   dlDeviceExtendedAddress[10];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* OnOff Server Callbacks */
static enum ZclStatusCodeT APP_ZIGBEE_OnOffServerOffCallback( struct ZbZclClusterT * pstCluster, struct ZbZclAddrInfoT * pstSrcInfo, void * arg );
static enum ZclStatusCodeT APP_ZIGBEE_OnOffServerOnCallback( struct ZbZclClusterT * pstCluster, struct ZbZclAddrInfoT * pstSrcInfo, void * arg );
static enum ZclStatusCodeT APP_ZIGBEE_OnOffServerToggleCallback( struct ZbZclClusterT * pstCluster, struct ZbZclAddrInfoT * pstSrcInfo, void * arg );

static struct ZbZclOnOffServerCallbacksT stOnOffServerCallbacks =
{
  .off = APP_ZIGBEE_OnOffServerOffCallback,
  .on = APP_ZIGBEE_OnOffServerOnCallback,
  .toggle = APP_ZIGBEE_OnOffServerToggleCallback,
};

/* IasWd Server Callbacks */
static enum ZclStatusCodeT APP_ZIGBEE_IasWdServerStartWarningCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclIasWdClientStartWarningReqT * pstRequest );
static enum ZclStatusCodeT APP_ZIGBEE_IasWdServerSquawkCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclIasWdClientSquawkReqT * pstRequest );

static struct ZbZclIasWdServerCallbacksT stIasWdServerCallbacks =
{
  .start_warning = APP_ZIGBEE_IasWdServerStartWarningCallback,
  .squawk = APP_ZIGBEE_IasWdServerSquawkCallback,
};

/* Messaging Server Callbacks */
static enum ZclStatusCodeT APP_ZIGBEE_MessagingServerGetLastMessageCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclAddrInfoT * pstSrcInfo );
static enum ZclStatusCodeT APP_ZIGBEE_MessagingServerMessageConfirmationCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclMsgMessageConfT * pstMessageConfirm, struct ZbZclAddrInfoT * pstSrcInfo );

static struct ZbZclMsgServerCallbacksT stMessagingServerCallbacks =
{
  .get_last_message = APP_ZIGBEE_MessagingServerGetLastMessageCallback,
  .message_confirmation = APP_ZIGBEE_MessagingServerMessageConfirmationCallback,
};

/* USER CODE BEGIN PFP */
static void APP_ZIGBEE_ApplicationTaskInit                            ( void );

static enum ZclStatusCodeT APP_ZIGBEE_ZclAttributeCallback            ( struct ZbZclClusterT * pstCluster, struct ZbZclAttrCbInfoT * pstCallbackInfo );

static void APP_ZIGBEE_IasWdServerAlarmProcessing                     ( void );
static void APP_ZIGBEE_IasWdServerSquawkProcessing                    ( void );

/* Overwrite attribute from cluster with application specific callbacks */
static const struct ZbZclAttrT  stOnOffAttributes = 
{
  ZCL_ONOFF_ATTR_ONOFF, ZCL_DATATYPE_BOOLEAN,
  ZCL_ATTR_FLAG_REPORTABLE | ZCL_ATTR_FLAG_CB_WRITE, 0, APP_ZIGBEE_ZclAttributeCallback, {0, 0}, {0, 0}
};

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
  enum ZclStatusCodeT             eStatus;

  /* USER CODE END APP_ZIGBEE_ConfigEndpoints1 */

  /* Add EndPoint */
  memset( &stRequest, 0, sizeof( stRequest ) );
  memset( &stConfig, 0, sizeof( stConfig ) );

  stRequest.profileId = APP_ZIGBEE_PROFILE_ID;
  stRequest.deviceId = APP_ZIGBEE_DEVICE_ID;
  stRequest.endpoint = APP_ZIGBEE_ENDPOINT;
  ZbZclAddEndpoint( stZigbeeAppInfo.pstZigbee, &stRequest, &stConfig );
  assert( stConfig.status == ZB_STATUS_SUCCESS );

  /* Add Identify Server Cluster */
  stZigbeeAppInfo.IdentifyServer = ZbZclIdentifyServerAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT, NULL, NULL );
  assert( stZigbeeAppInfo.IdentifyServer != NULL );
  if ( ZbZclClusterEndpointRegister( stZigbeeAppInfo.IdentifyServer ) == false )
  {
    LOG_ERROR_APP( "Error during Identify Server Endpoint Register." );
  }

  /* Add Scenes Server Cluster */
  stZigbeeAppInfo.ScenesServer = ZbZclScenesServerAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT, ZCL_SCENES_MAX_SCENES );
  assert( stZigbeeAppInfo.ScenesServer != NULL );
  if ( ZbZclClusterEndpointRegister( stZigbeeAppInfo.ScenesServer ) == false )
  {
    LOG_ERROR_APP( "Error during Scenes Server Endpoint Register." );
  }

  /* Add OnOff Server Cluster */
  stZigbeeAppInfo.OnOffServer = ZbZclOnOffServerAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT, &stOnOffServerCallbacks, NULL );
  assert( stZigbeeAppInfo.OnOffServer != NULL );
  if ( ZbZclClusterEndpointRegister( stZigbeeAppInfo.OnOffServer ) == false )
  {
    LOG_ERROR_APP( "Error during OnOff Server Endpoint Register." );
  }

  /* Add IasWd Server Cluster */
  stZigbeeAppInfo.IasWdServer = ZbZclIasWdServerAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT, &stIasWdServerCallbacks, NULL );
  assert( stZigbeeAppInfo.IasWdServer != NULL );
  if ( ZbZclClusterEndpointRegister( stZigbeeAppInfo.IasWdServer ) == false )
  {
    LOG_ERROR_APP( "Error during IasWd Server Endpoint Register." );
  }

  /* Add Messaging Server Cluster */
  stZigbeeAppInfo.MessagingServer = ZbZclMsgServerAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT, &stMessagingServerCallbacks, NULL );
  assert( stZigbeeAppInfo.MessagingServer != NULL );
  if ( ZbZclClusterEndpointRegister( stZigbeeAppInfo.MessagingServer ) == false )
  {
    LOG_ERROR_APP( "Error during Messaging Server Endpoint Register." );
  }

  /* USER CODE BEGIN APP_ZIGBEE_ConfigEndpoints2 */
  ZbZclClusterSetProfileId( stZigbeeAppInfo.MessagingServer, APP_ZIGBEE_PROFILE_ID );
  
  eStatus = ZbZclAttrAppendList( stZigbeeAppInfo.OnOffServer, &stOnOffAttributes, 1 );
  if ( eStatus != ZCL_STATUS_SUCCESS ) 
  {
    LOG_ERROR_APP( "Config Clusters: Failed to append OnOff attribute list (0x%02X)",eStatus );
    while(1) {}
  }

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
  pstConfig->bdbCommissioningMode |= BDB_COMMISSION_MODE_FIND_BIND;

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
  dlDeviceExtendedAddress[iDeviceIndex++] = dlExtendedAddress;

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
  LOG_INFO_APP( "%s on Endpoint %d.", APP_ZIGBEE_CLUSTER1_NAME, APP_ZIGBEE_ENDPOINT );
  LOG_INFO_APP( "%s on Endpoint %d.", APP_ZIGBEE_CLUSTER2_NAME, APP_ZIGBEE_ENDPOINT );
  LOG_INFO_APP( "%s on Endpoint %d.", APP_ZIGBEE_CLUSTER3_NAME, APP_ZIGBEE_ENDPOINT );
  LOG_INFO_APP( "%s on Endpoint %d.", APP_ZIGBEE_CLUSTER4_NAME, APP_ZIGBEE_ENDPOINT );
  LOG_INFO_APP( "%s on Endpoint %d.", APP_ZIGBEE_CLUSTER5_NAME, APP_ZIGBEE_ENDPOINT );

  /* USER CODE BEGIN APP_ZIGBEE_PrintApplicationInfo2 */

  /* USER CODE END APP_ZIGBEE_PrintApplicationInfo2 */

  LOG_INFO_APP( "**********************************************************" );
}

/**
 * @brief  OnOff Server 'Off' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_OnOffServerOffCallback( struct ZbZclClusterT * pstCluster, struct ZbZclAddrInfoT * pstSrcInfo, void * arg )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_OnOffServerOffCallback */
  uint8_t cEndpoint;

  cEndpoint = ZbZclClusterGetEndpoint( pstCluster );
  if ( cEndpoint == APP_ZIGBEE_ENDPOINT)
  {
    LOG_INFO_APP( "[ONOFF] Led 'OFF'" );
    APP_LED_OFF( LED_WORK );
    (void)ZbZclAttrIntegerWrite( pstCluster, ZCL_ONOFF_ATTR_ONOFF, 0 );
  }
  else
  {
    /* Unknown endpoint */
    eStatus = ZCL_STATUS_FAILURE;
  }

  /* USER CODE END APP_ZIGBEE_OnOffServerOffCallback */
  return eStatus;
}

/**
 * @brief  OnOff Server 'On' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_OnOffServerOnCallback( struct ZbZclClusterT * pstCluster, struct ZbZclAddrInfoT * pstSrcInfo, void * arg )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_OnOffServerOnCallback */
  uint8_t   cEndpoint;

  cEndpoint = ZbZclClusterGetEndpoint( pstCluster );
  if ( cEndpoint == APP_ZIGBEE_ENDPOINT )
  {
    LOG_INFO_APP( "[ONOFF] Led 'ON'" );
    APP_LED_ON( LED_WORK );
    (void)ZbZclAttrIntegerWrite( pstCluster, ZCL_ONOFF_ATTR_ONOFF, 1 );
  }
  else
  {
    /* Unknown endpoint */
    eStatus = ZCL_STATUS_FAILURE;
  }

  /* USER CODE END APP_ZIGBEE_OnOffServerOnCallback */
  return eStatus;
}

/**
 * @brief  OnOff Server 'Toggle' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_OnOffServerToggleCallback( struct ZbZclClusterT * pstCluster, struct ZbZclAddrInfoT * pstSrcInfo, void * arg )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_OnOffServerToggleCallback */
  uint8_t   cAttrVal;

  if ( ZbZclAttrRead( pstCluster, ZCL_ONOFF_ATTR_ONOFF, NULL, &cAttrVal, sizeof(cAttrVal), false) != ZCL_STATUS_SUCCESS )
  {
      eStatus = ZCL_STATUS_FAILURE;
  }
  else
  {
    if ( cAttrVal != 0u )
    {
        eStatus = APP_ZIGBEE_OnOffServerOffCallback( pstCluster, pstSrcInfo, arg );
    }
    else
    {
        eStatus = APP_ZIGBEE_OnOffServerOnCallback( pstCluster, pstSrcInfo, arg );
    }
  }

  /* USER CODE END APP_ZIGBEE_OnOffServerToggleCallback */
  return eStatus;
}

/**
 * @brief  IasWd Server 'StartWarning' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_IasWdServerStartWarningCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclIasWdClientStartWarningReqT * pstRequest )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_IasWdServerStartWarningCallback */
  LOG_INFO_APP( "[IAS WD] IAS WD StartWarning command received." );

  /* Dumping the request parameters */
  LOG_INFO_APP("[IAS WD] Warning mode: 0x%02x.",  pstRequest->warning_mode);
  LOG_INFO_APP("[IAS WD] Warning duration: 0x%04x.",  pstRequest->warning_duration);
  LOG_INFO_APP("[IAS WD] Siren level: 0x%02x.",  pstRequest->siren_level);
  LOG_INFO_APP("[IAS WD] Strobe enabled?  %s.",  (pstRequest->strobe) ? "Yes" : "No");
  LOG_INFO_APP("[IAS WD] Strobe duty cycle: 0x%02x.\n",  pstRequest->strobe_dutycycle);

  /* Activate the warning state */
  memcpy( &stWarningRequest, pstRequest, sizeof( stWarningRequest ) );
  UTIL_SEQ_SetTask(1U << CFG_TASK_ZIGBEE_APP_IAS_WD_START_WARNING, TASK_ZIGBEE_APP_IAS_WD_START_WARNING_PRIORITY );

  /* USER CODE END APP_ZIGBEE_IasWdServerStartWarningCallback */
  return eStatus;
}

/**
 * @brief  IasWd Server 'Squawk' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_IasWdServerSquawkCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclIasWdClientSquawkReqT * pstRequest )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_IasWdServerSquawkCallback */
  LOG_INFO_APP( "[IAS WD] IAS WD Squawk command received.");

  /* Dumping the request parameters */
  LOG_INFO_APP( "[IAS WD] Squawk mode: 0x%02x.",  pstRequest->squawk_mode );
  LOG_INFO_APP( "[IAS WD] Squawk level: 0x%02x.",  pstRequest->squawk_level );
  LOG_INFO_APP( "[IAS WD] Strobe enabled?  %s.",  (pstRequest->strobe) ? "Yes" : "No" );

  LOG_INFO_APP( "[IAS WD] Activating Squawk signal." );
  bIsSquawkStrobe = (bool)pstRequest->strobe;
  UTIL_SEQ_SetTask( 1U << CFG_TASK_ZIGBEE_APP_IAS_WD_SQUAWK, TASK_ZIGBEE_APP_IAS_WD_SQUAWK_PRIORITY );

  /* USER CODE END APP_ZIGBEE_IasWdServerSquawkCallback */
  return eStatus;
}

/**
 * @brief  Messaging Server 'GetLastMessage' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_MessagingServerGetLastMessageCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclAddrInfoT * pstSrcInfo )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_MessagingServerGetLastMessageCallback */
  LOG_INFO_APP( "[MESSAGE] Get Last Message Received" );
  
  /* USER CODE END APP_ZIGBEE_MessagingServerGetLastMessageCallback */
  return eStatus;
}

/**
 * @brief  Messaging Server 'MessageConfirmation' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_MessagingServerMessageConfirmationCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclMsgMessageConfT * pstMessageConfirm, struct ZbZclAddrInfoT * pstSrcInfo )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_MessagingServerMessageConfirmationCallback */
  LOG_INFO_APP( "[MESSAGE] Message Confirm Received" );

  /* USER CODE END APP_ZIGBEE_MessagingServerMessageConfirmationCallback */
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
  /* Register tasks associated with IAS WD */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_ZIGBEE_APP_IAS_WD_START_WARNING, UTIL_SEQ_RFU, APP_ZIGBEE_IasWdServerAlarmProcessing );
  UTIL_SEQ_RegTask( 1U << CFG_TASK_ZIGBEE_APP_IAS_WD_SQUAWK, UTIL_SEQ_RFU, APP_ZIGBEE_IasWdServerSquawkProcessing );
}

/**
 * @brief  ZCL Attribute write callback handler
 * @param  pstCluster     ZCL Cluster pointer
 * @param  pstSource      Sender info
 * @param  iAttributeId   Attribute ID
 * @param  pDataInput     Data to be written to attribute
 * @param  iInputMaxLen   Max Length of input data
 * @param  pAttrData      Attribute data information
 * @param  stMode         ZCL Write mode
 * @param  arg            Passed argument
 * @retval ZCL Status codexxxxx
 */
static enum ZclStatusCodeT APP_ZIGBEE_ZclAttributesWriteCallback( struct ZbZclClusterT * pstCluster, const struct ZbApsAddrT * pstSource, uint16_t iAttributeId,
                                                                  const uint8_t * pDataInput, unsigned int iInputMaxLen, void * pAttrData, ZclWriteModeT stMode, void * arg )
{
  uint16_t  iLen = 0;

  switch ( iAttributeId ) 
  {
    case ZCL_ONOFF_ATTR_ONOFF:
        iLen = 1;
        (void)memcpy( pAttrData, pDataInput, iLen );
        if ( pDataInput[0] == 1 ) 
        {
          LOG_INFO_APP( "Light 'ON'" );
          APP_LED_ON( LED_WORK );
        }
        else 
        {
          LOG_INFO_APP(  "Light 'OFF'" );
          APP_LED_OFF( LED_WORK );
        }
        return ZCL_STATUS_SUCCESS;

    default:
        return ZCL_STATUS_UNSUPP_ATTRIBUTE;
  }
}

/**
 * @brief  ZCL Attribute read/write callback handler
 * @param  pstCluster       ZCL Cluster pointer
 * @param  pstCallbackInfo  Attribute callback info
 * @retval ZCL Status code
 */
static enum ZclStatusCodeT APP_ZIGBEE_ZclAttributeCallback( struct ZbZclClusterT * pstCluster, struct ZbZclAttrCbInfoT * pstCallbackInfo )
{
  if ( pstCallbackInfo->type == ZCL_ATTR_CB_TYPE_WRITE) 
  {
    return APP_ZIGBEE_ZclAttributesWriteCallback( pstCluster, pstCallbackInfo->src, pstCallbackInfo->info->attributeId, pstCallbackInfo->zcl_data, pstCallbackInfo->zcl_len,
            pstCallbackInfo->attr_data, pstCallbackInfo->write_mode, pstCallbackInfo->app_cb_arg );
  }
  else 
  {
    return ZCL_STATUS_FAILURE;
  }
}

/**
 * @brief  Zigbee application IAS WD StartWarning command processing
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_IasWdServerAlarmProcessing( void )
{
  enum ZclStatusCodeT   eStatus;
  enum ZclDataTypeT     eType;
  uint16_t  iWarningDuration;
  uint16_t  iMaxDuration;
  uint16_t  iIndex;

  /* reading local MaxDuration attribute */
  eType = ZCL_DATATYPE_UNSIGNED_16BIT;
  iWarningDuration = stWarningRequest.warning_duration;
  
  iMaxDuration = (uint16_t)ZbZclAttrIntegerRead( stZigbeeAppInfo.IasWdServer, ZCL_IAS_WD_SVR_ATTR_MAX_DURATION, &eType, &eStatus );
  if ( eStatus != ZCL_STATUS_SUCCESS ) 
  {
    LOG_ERROR_APP( "[IAS WD] Error during 'Read Integer Attribute' (0x%02X)", eStatus );
    assert(0);
  }

  /* The alarm cannot last ;ore that MaxDuration attribute value */
  if ( iMaxDuration < iWarningDuration) 
  {
    iWarningDuration = iMaxDuration;
  }

  for ( iIndex = 0; iIndex < iWarningDuration ; iIndex++ ) 
  {
    LOG_INFO_APP( "[IAS WD] Alarm!" );
    if ( stWarningRequest.strobe ) 
    {
      /* strobe enabled */
      APP_LED_ON(LED_WORK);
      HAL_Delay( stWarningRequest.strobe_dutycycle*10); /* time in ms corresponding to strobe duty cycle */
      APP_LED_OFF(LED_WORK);
      HAL_Delay(1000 - stWarningRequest.strobe_dutycycle*10); /* remaining time in 1s */
    } 
    else 
    {
      HAL_Delay(1000);
    }
  }

  LOG_INFO_APP( "[IAS WD] Alarm ended." );
}


/**
 * @brief  Zigbee application IAS WD Squawk command processing
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_IasWdServerSquawkProcessing( void )
{
  uint16_t  iIndex;
  
  for ( iIndex = 0 ; iIndex < 3u ; iIndex++ ) 
  { 
    /* 3 sec loop */
    LOG_INFO_APP( "[IAS WD] Squawk!" );
    if ( bIsSquawkStrobe != false ) 
    {
      APP_LED_ON(LED_WORK);
    }
    
    HAL_Delay(100);

    if ( bIsSquawkStrobe != false ) 
    {
      APP_LED_OFF(LED_WORK);
    }
    
    HAL_Delay(100);
  }
  
  LOG_INFO_APP( "[IAS WD] Squawk signal ended." );
}

/**
 * @brief  Management of the SW1 button : Put Identify cluster into identify mode.
 * @param  None
 * @retval None
 */
void APP_BSP_Button1Action( void )
{
  /* First, verify if Appli has already Join a Network  */ 
  if ( APP_ZIGBEE_IsAppliJoinNetwork() != false )
  {
    if ( APP_BSP_ButtonIsLongPressed( B1 ) == false )
    {
      LOG_INFO_APP( "SW1 PUSHED : Turn on 'Identify Mode' during %ds", APP_ZIGBEE_IDENTIFY_MODE_DELAY );
      ZbZclIdentifyServerSetTime( stZigbeeAppInfo.IdentifyServer, APP_ZIGBEE_IDENTIFY_MODE_DELAY );
    }
    else
    {
      (void)APP_ZIGBEE_GetDisplayBindTable( true );
    }
  }
}

///**
// * @brief   Callback after a Find&Bind Request
// *
// */
//static void APP_ZIGBEE_ZbStartupFindBindStartCallback( enum ZbStatusCodeT eStatus, void * arg )
//{
//  enum ZbStatusCodeT  * pZbStatus = arg;
//  
//  LOG_INFO_APP( "FindBindStartCallback" );
//  *pZbStatus = eStatus;
//  UTIL_SEQ_SetEvt( EVENT_ZIGBEE_APP1 );
//}


/**
 * @brief  Management of the SW2 button : Send ZCL Display Message request via bindings.
 * @param  None
 * @retval None
 */
void APP_BSP_Button2Action( void )
{
  struct ZbZclMsgMessageT   stMessage;
  struct ZbApsAddrT         stDestAddress;
  enum ZclStatusCodeT       eZclStatus;
  
  /* First, verify if Appli has already Join a Network  */ 
  if ( APP_ZIGBEE_IsAppliJoinNetwork() != false )
  {
    LOG_INFO_APP( "SW2 PUSHED : Send New Message" );
    
    /* Set Address */
    memset( &stDestAddress, 0, sizeof( stDestAddress ) );
    
    stDestAddress.mode = ZB_APSDE_ADDRMODE_EXT;
    stDestAddress.extAddr = dlDeviceExtendedAddress[0];
    stDestAddress.endpoint = 12;
          
    /* Set Message */
    memset( &stMessage, 0, sizeof( stMessage ) );

    stMessage.message_id = 1;
    stMessage.duration = 1;
    stMessage.message_str = (uint8_t *)pszMessageBuffer;
    stMessage.message_len = sizeof( pszMessageBuffer );
    
    //eZclStatus = ZbZclMsgServerDisplayMessageUnsolic( stZigbeeAppInfo.MessagingServer, ZbApsAddrBinding, &stMessage, NULL, NULL );
    eZclStatus = ZbZclMsgServerDisplayMessageUnsolic( stZigbeeAppInfo.MessagingServer, &stDestAddress, &stMessage, NULL, NULL );
    if ( eZclStatus != ZCL_STATUS_SUCCESS) 
    {
      LOG_ERROR_APP( "Failed to display New message (0x%02X)", eZclStatus );
    }
  }
}


/* USER CODE END FD_LOCAL_FUNCTIONS */
