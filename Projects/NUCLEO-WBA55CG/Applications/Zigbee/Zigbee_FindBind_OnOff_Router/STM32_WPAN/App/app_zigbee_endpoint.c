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
#include "zcl/general/zcl.scenes.h"
#include "zcl/general/zcl.onoff.h"

/* USER CODE BEGIN PI */
#include "stm32wbaxx_nucleo.h"

/* USER CODE END PI */

/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_CHANNEL                13u
#define APP_ZIGBEE_CHANNEL_MASK           ( 1u << APP_ZIGBEE_CHANNEL )
#define APP_ZIGBEE_TX_POWER               ((int8_t) 10)    /* TX-Power is at +10 dBm. */

#define APP_ZIGBEE_ENDPOINT               14u
#define APP_ZIGBEE_PROFILE_ID             ZCL_PROFILE_HOME_AUTOMATION
#define APP_ZIGBEE_DEVICE_ID              ZCL_DEVICE_ONOFF_LIGHT
#define APP_ZIGBEE_GROUP_ADDRESS          0x0001u

#define APP_ZIGBEE_CLUSTER1_ID            ZCL_CLUSTER_IDENTIFY
#define APP_ZIGBEE_CLUSTER1_NAME          "Identify Client"

#define APP_ZIGBEE_CLUSTER2_ID            ZCL_CLUSTER_SCENES
#define APP_ZIGBEE_CLUSTER2_NAME          "Scenes Client"

#define APP_ZIGBEE_CLUSTER3_ID            ZCL_CLUSTER_ONOFF
#define APP_ZIGBEE_CLUSTER3_NAME          "OnOff Client"

/* USER CODE BEGIN PD */
#define APP_ZIGBEE_SCENE_ID               APP_ZIGBEE_CLUSTER2_ID

#define APP_ZIGBEE_APPLICATION_NAME       "Find-Bind OnOff Router"
#define APP_ZIGBEE_APPLICATION_OS_NAME    "."

/* USER CODE END PD */

// -- Redefine Clusters to better code read --
#define IdentifyClient                    pstZbCluster[0]
#define ScenesClient                      pstZbCluster[1]
#define OnOffClient                       pstZbCluster[2]

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private constants ---------------------------------------------------------*/
/* USER CODE BEGIN PC */

/* USER CODE END PC */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */
static void APP_ZIGBEE_FindBindStart      ( void );

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
  /* Start Find-Bind */
  APP_ZIGBEE_FindBindStart();

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

  /* Add Scenes Client Cluster */
  stZigbeeAppInfo.ScenesClient = ZbZclScenesClientAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT );
  assert( stZigbeeAppInfo.ScenesClient != NULL );
  ZbZclClusterEndpointRegister( stZigbeeAppInfo.ScenesClient );

  /* Add OnOff Client Cluster */
  stZigbeeAppInfo.OnOffClient = ZbZclOnOffClientAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT );
  assert( stZigbeeAppInfo.OnOffClient != NULL );
  ZbZclClusterEndpointRegister( stZigbeeAppInfo.OnOffClient );

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
  LOG_INFO_APP( "%s on Endpoint %d.", APP_ZIGBEE_CLUSTER3_NAME, APP_ZIGBEE_ENDPOINT );

  /* USER CODE BEGIN APP_ZIGBEE_PrintApplicationInfo2 */

  /* USER CODE END APP_ZIGBEE_PrintApplicationInfo2 */

  LOG_INFO_APP( "**********************************************************" );
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

/**
 * @brief  Start the OnOff Client.
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_FindBindStart(void)
{
  /* Search the number of Binding */
  uint16_t              iIndex = 0, iNbEntries = 0;
  struct ZbApsmeBindT   stBindEntry;
  enum ZbStatusCodeT    eStatus;

  do
  {
    eStatus = ZbApsGetIndex( stZigbeeAppInfo.pstZigbee, ZB_APS_IB_ID_BINDING_TABLE, &stBindEntry, sizeof(stBindEntry), iIndex );
    if ( eStatus ==  ZB_APS_STATUS_SUCCESS )
    {
      if ( stBindEntry.srcExtAddr != 0ULL)
      {
          iNbEntries++;
          LOG_INFO_APP( "[FIND-BIND] Entry : Cluster (0x%02X) on Source Endpoint (0x%02X) and Dest EndPoint/Address (0x%02X / 0x%04X)",
                    stBindEntry.clusterId, stBindEntry.srcEndpt, stBindEntry.dst.endpoint, stBindEntry.dst.nwkAddr );
      }
    }
    iIndex++;
  }
  while ( eStatus ==  ZB_APS_STATUS_SUCCESS );

  LOG_INFO_APP( "[FIND-BIND] Binding entries created: %d", iNbEntries );
  if ( iNbEntries != 0u ) 
  {
    APP_LED_ON(LED_GREEN);
  }
}

/**
 * @brief  Management of the SW1 button : Send the OnOff Command
 * @param  None
 * @retval None
 */
void APPE_Button1Action(void)
{
  struct ZbApsAddrT     stDest;
  enum ZclStatusCodeT   eStatus;
  
  /* First, verify if Appli has already Join a Network  */ 
  if ( APP_ZIGBEE_IsAppliJoinNetwork() != false )
  {
    /* Prepare destination */
    memset( &stDest, 0, sizeof( stDest) );
    stDest.endpoint = APP_ZIGBEE_ENDPOINT;
    stDest.mode = ZB_APSDE_ADDRMODE_GROUP;
    stDest.nwkAddr = APP_ZIGBEE_GROUP_ADDRESS;

    LOG_INFO_APP( "[ONOFF] SW1 pushed, sending 'TOGGLE'" );
    eStatus = ZbZclOnOffClientToggleReq( stZigbeeAppInfo.OnOffClient, &stDest, NULL, NULL );
    if ( eStatus != ZCL_STATUS_SUCCESS )
    {
      LOG_ERROR_APP( "[ONOFF] Error, OnOff Client Request failed (0x%02X).", eStatus );
    }
  }
}

/**
 * @brief  Management of the SW2 button : Sends a store scene command via binding if device is on a network.
 * @param  None
 * @retval None
 */
void APPE_Button2Action(void)
{
  struct zcl_scenes_store_request_t stRequest;
  enum ZclStatusCodeT               eStatus;
  
  /* First, verify if Appli has already Join a Network  */ 
  if ( APP_ZIGBEE_IsAppliJoinNetwork() != false )
  {
    memset( &stRequest, 0, sizeof( stRequest ) );

    stRequest.dst = *ZbApsAddrBinding;
    stRequest.groupId = APP_ZIGBEE_GROUP_ADDRESS;
    stRequest.sceneId = APP_ZIGBEE_SCENE_ID;
      
    LOG_INFO_APP( "[SCENES] SW2 PUHSED, send 'Store Scene Command'" );
    eStatus = zcl_scenes_client_store_req( stZigbeeAppInfo.ScenesClient, &stRequest, NULL, NULL );
    if ( eStatus != ZCL_STATUS_SUCCESS )
    {
      LOG_ERROR_APP( "[SCENES] Error, send 'Store Scene Command' Request failed (0x%02X).", eStatus );
    }
  }
}

/**
 * @brief  Management of the SW3 button : Sends a recall scene command via binding if device is on a network.
 * @param  None
 * @retval None
 */
void APPE_Button3Action(void)
{
  struct zcl_scenes_recall_request_t  stRequest;
  enum ZclStatusCodeT                 eStatus;
  
  /* First, verify if Appli has already Join a Network  */ 
  if ( APP_ZIGBEE_IsAppliJoinNetwork() != false )
  {
    memset( &stRequest, 0, sizeof( stRequest ) );

    stRequest.dst = *ZbApsAddrBinding;
    stRequest.groupId = APP_ZIGBEE_GROUP_ADDRESS;
    stRequest.sceneId = APP_ZIGBEE_SCENE_ID;
    stRequest.transition = 0;
      
    LOG_INFO_APP( "[SCENES] SW3 PUHSED, send 'Recall Scene Command'" );
    eStatus = zcl_scenes_client_recall_req( stZigbeeAppInfo.ScenesClient, &stRequest, NULL, NULL );
    if ( eStatus != ZCL_STATUS_SUCCESS )
    {
      LOG_ERROR_APP( "[SCENES] Error, send 'Recall Scene Command' Request failed (0x%02X).", eStatus );
    }
  }
}

/* USER CODE END FD_LOCAL_FUNCTIONS */
