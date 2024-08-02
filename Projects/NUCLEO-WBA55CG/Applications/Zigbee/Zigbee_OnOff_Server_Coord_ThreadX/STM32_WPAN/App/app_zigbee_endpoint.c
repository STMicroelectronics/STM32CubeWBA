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
#include "zcl/general/zcl.onoff.h"

/* USER CODE BEGIN PI */
#include "stm32wbaxx_nucleo.h"

/* USER CODE END PI */

/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_CHANNEL                14u
#define APP_ZIGBEE_CHANNEL_MASK           ( 1u << APP_ZIGBEE_CHANNEL )
#define APP_ZIGBEE_TX_POWER               ((int8_t) 10)    /* TX-Power is at +10 dBm. */

#define APP_ZIGBEE_ENDPOINT               17u
#define APP_ZIGBEE_PROFILE_ID             ZCL_PROFILE_HOME_AUTOMATION
#define APP_ZIGBEE_DEVICE_ID              ZCL_DEVICE_ONOFF_LIGHT
#define APP_ZIGBEE_GROUP_ADDRESS          0x0001u

#define APP_ZIGBEE_CLUSTER_ID             ZCL_CLUSTER_ONOFF
#define APP_ZIGBEE_CLUSTER_NAME           "OnOff Server"

/* USER CODE BEGIN PD */
#define APP_ZIGBEE_APPLICATION_NAME       APP_ZIGBEE_CLUSTER_NAME
#define APP_ZIGBEE_APPLICATION_OS_NAME    "on ThreadX"

/* USER CODE END PD */

// -- Redefine Clusters to better code read --
#define OnOffServer                       pstZbCluster[0]

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

/* OnOff Server Callbacks */
static enum ZclStatusCodeT  APP_ZIGBEE_OnOffServerOffCallback               ( struct ZbZclClusterT * pstCluster, struct ZbZclAddrInfoT * pstSrcInfo, void * arg );
static enum ZclStatusCodeT  APP_ZIGBEE_OnOffServerOnCallback                ( struct ZbZclClusterT * pstCluster, struct ZbZclAddrInfoT * pstSrcInfo, void * arg );
static enum ZclStatusCodeT  APP_ZIGBEE_OnOffServerToggleCallback            ( struct ZbZclClusterT * pstCluster, struct ZbZclAddrInfoT * pstSrcInfo, void * arg );

static struct ZbZclOnOffServerCallbacksT stOnOffServerCallbacks =
{
  .off = APP_ZIGBEE_OnOffServerOffCallback,
  .on = APP_ZIGBEE_OnOffServerOnCallback,
  .toggle = APP_ZIGBEE_OnOffServerToggleCallback,
};

/* USER CODE BEGIN PFP */
static void APP_ZIGBEE_OnOffClientStart       ( void );

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
  APP_ZIGBEE_OnOffClientStart();

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

  /* Add OnOff Server Cluster */
  stZigbeeAppInfo.OnOffServer = ZbZclOnOffServerAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT, &stOnOffServerCallbacks, NULL );
  assert( stZigbeeAppInfo.OnOffServer != NULL );
  ZbZclClusterEndpointRegister( stZigbeeAppInfo.OnOffServer );

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
    LOG_INFO_APP( "[ONOFF] Red Led 'OFF'" );
    APP_LED_OFF( LED_RED );
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
    LOG_INFO_APP( "[ONOFF] Red Led 'ON'" );
    APP_LED_ON( LED_RED );
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
    eStatus =  ZCL_STATUS_FAILURE;
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

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

/**
 * @brief  Start the OnOff Client.
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_OnOffClientStart(void)
{
}

/* USER CODE END FD_LOCAL_FUNCTIONS */
