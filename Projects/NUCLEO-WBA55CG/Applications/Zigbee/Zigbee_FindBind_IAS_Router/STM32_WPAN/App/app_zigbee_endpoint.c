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
#include "zcl/general/zcl.identify.h"
#include "zcl/se/zcl.message.h"

/* USER CODE BEGIN PI */
#include "zcl/security/zcl.ias_wd.h"
#include "stm32wbaxx_nucleo.h"

/* USER CODE END PI */

/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_CHANNEL                13u
#define APP_ZIGBEE_CHANNEL_MASK           ( 1u << APP_ZIGBEE_CHANNEL )
#define APP_ZIGBEE_TX_POWER               ((int8_t) 10)    /* TX-Power is at +10 dBm. */

#define APP_ZIGBEE_ENDPOINT               12u
#define APP_ZIGBEE_PROFILE_ID             ZCL_PROFILE_HOME_AUTOMATION
#define APP_ZIGBEE_DEVICE_ID              ZCL_DEVICE_ONOFF_LIGHT
#define APP_ZIGBEE_GROUP_ADDRESS          0x0001u

#define APP_ZIGBEE_CLUSTER1_ID            ZCL_CLUSTER_IDENTIFY
#define APP_ZIGBEE_CLUSTER1_NAME          "Identify Client"

#define APP_ZIGBEE_CLUSTER2_ID            ZCL_CLUSTER_MESSAGING
#define APP_ZIGBEE_CLUSTER2_NAME          "Messaging Client"

/* USER CODE BEGIN PD */
#define APP_ZIGBEE_FIND_BIND_CHECK_DELAY  5000u  // Check Find & Bind after 5s.

/* Redefine tasks & priorities to better code read */
#define CFG_TASK_ZIGBEE_APP_CHECK_BINDINGS        CFG_TASK_ZIGBEE_APP1
#define TASK_ZIGBEE_APP_CHECK_BINDINGS_PRIORITY   CFG_SEQ_PRIO_0

/* Define IAS Wd Client that be initialised after SW1 active and not during initialisation */
#define APP_ZIGBEE_CLUSTER3_ID            ZCL_CLUSTER_IAS_WD
#define APP_ZIGBEE_CLUSTER3_NAME          "IasWd Client"

#define IasWdClient                       pstZbCluster[2]

#define APP_ZIGBEE_APPLICATION_NAME       "Find-Bind IAS Router"
#define APP_ZIGBEE_APPLICATION_OS_NAME    "."

/* USER CODE END PD */

// -- Redefine Clusters to better code read --
#define IdentifyClient                    pstZbCluster[0]
#define MessagingClient                   pstZbCluster[1]

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private constants ---------------------------------------------------------*/
/* USER CODE BEGIN PC */

/* USER CODE END PC */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static UTIL_TIMER_Object_t      stTimerCheckFindBind;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* Messaging Client Callbacks */
static enum ZclStatusCodeT APP_ZIGBEE_MessagingClientDisplayMessageCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclMsgMessageT * pstMessage, struct ZbZclAddrInfoT * pstSrcInfo );

static struct ZbZclMsgClientCallbacksT stMessagingClientCallbacks =
{
  .display_message = APP_ZIGBEE_MessagingClientDisplayMessageCallback,
};

/* USER CODE BEGIN PFP */
static void APP_ZIGBEE_ApplicationTaskInit                          ( void );
static void APP_ZIGBEE_FindBindClientStart                          ( void );
static void APP_ZIGBEE_FindBindCheckTimerCallback                   ( void * arg );
static void APP_ZIGBEE_FindBindCheckBindings                        ( void );

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
  /* Start OnOff Client */
  APP_ZIGBEE_FindBindClientStart();

  /* Display Short Address */
  LOG_INFO_APP( "Use Short Address : 0x%04X", ZbShortAddress( stZigbeeAppInfo.pstZigbee ) );
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

  /* Add Identify Client Cluster */
  stZigbeeAppInfo.IdentifyClient = ZbZclIdentifyClientAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT );
  assert( stZigbeeAppInfo.IdentifyClient != NULL );
  ZbZclClusterEndpointRegister( stZigbeeAppInfo.IdentifyClient );

  /* Add Messaging Client Cluster */
  stZigbeeAppInfo.MessagingClient = ZbZclMsgClientAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT, &stMessagingClientCallbacks, NULL );
  assert( stZigbeeAppInfo.MessagingClient != NULL );
  ZbZclClusterEndpointRegister( stZigbeeAppInfo.MessagingClient );

  /* USER CODE BEGIN APP_ZIGBEE_ConfigEndpoints2 */
  ZbZclClusterSetProfileId( stZigbeeAppInfo.MessagingClient, APP_ZIGBEE_PROFILE_ID );

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
 * @brief  Messaging Client 'DisplayMessage' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_MessagingClientDisplayMessageCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclMsgMessageT * pstMessage, struct ZbZclAddrInfoT * pstSrcInfo )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_MessagingClientDisplayMessageCallback */
  LOG_INFO_APP( "[MESSAGE CLIENT] Display Message (%d)", ZCL_MESSAGE_SVR_CMD_DISPLAY_MESSAGE );
  LOG_INFO_APP( "   Message ID = 0x%08x", pstMessage->message_id );
  LOG_INFO_APP( "   Start time = 0x%08x", pstMessage->start_time );
  LOG_INFO_APP( "   Duration = %d", pstMessage->duration );
  LOG_INFO_APP( "   Message Control = 0x%02x", pstMessage->message_control );
  LOG_INFO_APP( "   Message String = \"%s\"", pstMessage->message_str );
  LOG_INFO_APP( "   Extended_control = 0x%02x", pstMessage->extended_control );
  LOG_INFO_APP( "" );

  /* USER CODE END APP_ZIGBEE_MessagingClientDisplayMessageCallback */
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
  /* Task that check previous Find & Bind */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_ZIGBEE_APP_CHECK_BINDINGS, UTIL_SEQ_RFU, APP_ZIGBEE_FindBindCheckBindings );
  
  /* Create timer to launch check of Find & Bind */
  UTIL_TIMER_Create( &stTimerCheckFindBind, APP_ZIGBEE_FIND_BIND_CHECK_DELAY, UTIL_TIMER_ONESHOT, APP_ZIGBEE_FindBindCheckTimerCallback, NULL );
}

/**
 * @brief  Start the FindBind Client.
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_FindBindClientStart(void)
{
  /* Not used */
}

/**
 * @brief  Checks the number of valid entries in the binding table.
 * @param  None
 * @retval Number of bindings
 */
static uint16_t APP_ZIGBEE_GetNumberOfBinding( void )
{
  uint16_t              iIndex = 0, iNumEntries = 0;
  struct ZbApsmeBindT   stBindEntry;
  enum ZbStatusCodeT    eStatus;

  do
  {
    eStatus = ZbApsGetIndex( stZigbeeAppInfo.pstZigbee, ZB_APS_IB_ID_BINDING_TABLE, &stBindEntry, sizeof( stBindEntry ), iIndex );
    if ( eStatus == ZB_APS_STATUS_SUCCESS )
    {
      if ( stBindEntry.srcExtAddr != 0u ) 
      {
        iNumEntries++;
        LOG_INFO_APP( "[FIND-BIND] Entry : Cluster (0x%02X) on Source Endpoint (0x%02X) and Dest EndPoint/Address (0x%02X / 0x%04X)",
                    stBindEntry.clusterId, stBindEntry.srcEndpt, stBindEntry.dst.endpoint, stBindEntry.dst.nwkAddr );
      }
    }
    iIndex++;
  }
  while ( eStatus == ZB_APS_STATUS_SUCCESS );
         
  return iNumEntries;
}

/**
 * @brief  Allocates the IAS Warn Device Client if it doesn't exists. 
 *         Otherwise, it just returns a status of ZCL_STATUS_SUCCESS if IAS WD client has been previously allocated.
 * @param  None
 * @retval ZCL status code
 */
static void APP_ZIGBEE_FindBindAllocateIasClient( void )
{
  if ( stZigbeeAppInfo.IasWdClient == NULL) 
  {
    /* Add Idenfity Server Cluster */
    stZigbeeAppInfo.IasWdClient = ZbZclIasWdClientAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT, NULL );
    assert( stZigbeeAppInfo.IasWdClient != NULL );
    ZbZclClusterEndpointRegister( stZigbeeAppInfo.IasWdClient );
  }
}

/**
 * @brief  Task called after F&B process to check the number of bindings. Will turn on Green LED
 * if bindings are successfully created, and attempt to allocate the IAS WD client cluster.
 *
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_FindBindCheckBindings( void )
{
  uint16_t  iNbBinding;
  
  iNbBinding = APP_ZIGBEE_GetNumberOfBinding();
  if ( iNbBinding != 0u ) 
  {
    APP_LED_ON(LED_GREEN);
    
    /* First F&B is successful allocate IAS cluster */
    APP_ZIGBEE_FindBindAllocateIasClient();
  }
  
  LOG_INFO_APP( "[FIND-BIND] Binding entries created: %d", iNbBinding);
}

/**
 * @brief  Management of the Timer Callback to the Check the Find & Bind.
 * @param  arg    Argument 
 * @retval None
 */
static void APP_ZIGBEE_FindBindCheckTimerCallback( void * arg )
{
  UTIL_SEQ_SetTask( 1U << CFG_TASK_ZIGBEE_APP_CHECK_BINDINGS, TASK_ZIGBEE_APP_CHECK_BINDINGS_PRIORITY );
}

/**
 * @brief  Management of the SW1 button : Start Finding and Binding process as an initiator.
 * @param  None
 * @retval None
 */
void APPE_Button1Action( void )
{
  enum ZbStatusCodeT    eStatus;
  
  /* First, verify if Appli has already Join a Network  */ 
  if ( APP_ZIGBEE_IsAppliJoinNetwork() != false )
  {
    LOG_INFO_APP( "[FIND-BIND] SW1 PUSHED : Initiate Find & Bind");
    
    eStatus = ZbStartupFindBindStart( stZigbeeAppInfo.pstZigbee, NULL, NULL );
    if ( eStatus != ZB_STATUS_SUCCESS ) 
    {
      LOG_ERROR_APP("[FIND-BIND] Error, cannot start Finding & Binding (0x%02X)", eStatus);
    }
    else 
    {
      /* Start wait to Check Find & Bind results */
      UTIL_TIMER_Start( &stTimerCheckFindBind ); 
    }
  }
}

/**
 * @brief  Management of the SW2 button : Sends a IAS start warning command via binding if device is on a network.
 * @param  None
 * @retval None
 */
void APPE_Button2Action( void )
{
  struct ZbZclIasWdClientStartWarningReqT   stRequest;
  enum ZclStatusCodeT                       eStatus;

  /* First, verify if Appli has already Join a Network  */ 
  if ( APP_ZIGBEE_IsAppliJoinNetwork() != false )
  {
    memset( &stRequest, 0, sizeof( stRequest ) );
    
    stRequest.warning_mode = ZCL_IAS_WD_WARNING_MODE_STOP;
    stRequest.strobe = ZCL_IAS_WD_STROBE_ON;
    stRequest.siren_level = ZCL_IAS_WD_LEVEL_LOW;
    stRequest.warning_duration = 3u;
    stRequest.strobe_dutycycle = 100u;
    stRequest.strobe_level = ZCL_IAS_WD_LEVEL_LOW;
    
    LOG_INFO_APP( "[FIND-BIND] SW2 PUSHED : Send 'Start Warning Command'" );
    eStatus = (enum ZclStatusCodeT)ZbZclIasWdClientStartWarningReq( stZigbeeAppInfo.IasWdClient, ZbApsAddrBinding, &stRequest, NULL, NULL );
    if ( eStatus != ZCL_STATUS_SUCCESS ) 
    {
      LOG_ERROR_APP( "[FIND-BIND] Error, cannot send 'Start Warning Command' (0x%02X)", eStatus );
    }
  }
}


/**
 * @brief  Management of the SW3 button : Sends a get last message command via binding if device is on a network.
 * @param  None
 * @retval None
 */
void APPE_Button3Action( void )
{
  enum ZclStatusCodeT  eStatus;
  
  /* First, verify if Appli has already Join a Network  */ 
  if ( APP_ZIGBEE_IsAppliJoinNetwork() != false )
  {
    LOG_INFO_APP( "[FIND-BIND] SW3 PUSHED : Send 'Get Last Message Command'" );
    
    eStatus = ZbZclMsgClientGetLastReq( stZigbeeAppInfo.MessagingClient, ZbApsAddrBinding, NULL, NULL );
    if ( eStatus != ZCL_STATUS_SUCCESS ) 
    {
      LOG_ERROR_APP( "[FIND-BIND] Error, failed to send 'Get Last Message' (0x%02X)", eStatus );
    }
  }
}

/* USER CODE END FD_LOCAL_FUNCTIONS */
