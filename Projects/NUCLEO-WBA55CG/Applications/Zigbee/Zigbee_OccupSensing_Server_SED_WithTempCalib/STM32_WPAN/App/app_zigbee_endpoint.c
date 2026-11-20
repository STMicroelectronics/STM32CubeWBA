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
#include "zcl/general/zcl.occupancy.h"

/* USER CODE BEGIN PI */
#include "app_bsp.h"
#include "stm32wbaxx_ll_adc.h"

/* USER CODE END PI */

/* Global variables -----------------------------------------------------------*/
/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_CHANNEL_MASK           WPAN_CHANNELMASK_2400MHZ
#define APP_ZIGBEE_TX_POWER               ((int8_t) 0)    /* TX-Power is at 0 dBm by default. */

#define APP_ZIGBEE_ENDPOINT               17u
#define APP_ZIGBEE_PROFILE_ID             ZCL_PROFILE_HOME_AUTOMATION
#define APP_ZIGBEE_DEVICE_ID              ZCL_DEVICE_ONOFF_OCCUPANCY_SENSOR

#define APP_ZIGBEE_CLUSTER_ID             ZCL_CLUSTER_OCCUPANCY
#define APP_ZIGBEE_CLUSTER_NAME           "Occupancy Server"

/* Timeout = (60 * 2^APP_ZIGBEE_SED_SLEEP_TIME) seconds. SED KeepAlive is send every (TimeOut / 4) */
#define APP_ZIGBEE_SED_SLEEP_TIME         1u          /* TimeOut = 2mn. */

/* USER CODE BEGIN PD */
#define APP_ZIGBEE_STARTUP_FAIL_DELAY     500u

#define APP_ZIGBEE_APPLICATION_NAME       APP_ZIGBEE_CLUSTER_NAME
#define APP_ZIGBEE_APPLICATION_OS_NAME    "."

// -- Redefine task to better code read --
#define CFG_TASK_ZIGBEE_APP_OCCUPANCY_UPDATE        CFG_TASK_ZIGBEE_APP1
#define TASK_ZIGBEE_APP_OCCUPANCY_UPDATE_PRIORITY   CFG_SEQ_PRIO_1

#define CFG_TASK_ZIGBEE_APP_LEAVE_NETWORK           CFG_TASK_ZIGBEE_APP2
#define TASK_ZIGBEE_APP_LEAVE_NETWORK_PRIORITY      CFG_SEQ_PRIO_1

/* USER CODE END PD */

// -- Redefine Clusters to better code read --
#define OccupancyServer                   pstZbCluster[0]

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private constants ---------------------------------------------------------*/
/* USER CODE BEGIN PC */

/* USER CODE END PC */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
static uint8_t                  cOccupancyState;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */
static void APP_ZIGBEE_ApplicationTaskInit          ( void );
static void APP_ZIGBEE_LeaveNetwork                 ( void );
static void APP_ZIGBEE_AttributeOccupancyUpdate     ( void );
static void APP_ZIGBEE_LeaveNetwork                 ( void );

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
  stZigbeeAppInfo.bNwkStartupDone = false;

  /* USER CODE BEGIN APP_ZIGBEE_ApplicationInit */
  /* Initialization of used Tasks */
  APP_ZIGBEE_ApplicationTaskInit();  

  /* USER CODE END APP_ZIGBEE_ApplicationInit */

  /* Initialize Zigbee stack layers */
  APP_ZIGBEE_StackLayersInit();

  /* Launch the Startup to Join/Form Network */
  if ( stZigbeeAppInfo.bWaitUserActionToJoin == false )
  {
    UTIL_SEQ_SetTask( 1U << CFG_TASK_ZIGBEE_NETWORK_FORM, TASK_PRIO_ZIGBEE_NETWORK_FORM );
  }
  else
  {
    LOG_INFO_APP( "Wait User action to start a 'Join' ..." );

    /* In this case, the 'User Code' (by example button management) must launch
     * UTIL_SEQ_SetTask( 1U << CFG_TASK_ZIGBEE_NETWORK_FORM, TASK_PRIO_ZIGBEE_NETWORK_FORM );
     * to start the Join process. */
  }
}

/**
 * @brief  Zigbee application start
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_ApplicationStart( void )
{
  /* USER CODE BEGIN APP_ZIGBEE_ApplicationStart */
  /* Update default Occupancy */
  cOccupancyState = 0;

  /* Display Short Address */
  LOG_INFO_APP( "UseShort Address : 0x%04X", ZbShortAddress( stZigbeeAppInfo.pstZigbee ) );
  LOG_INFO_APP( "%s ready to work !", APP_ZIGBEE_APPLICATION_NAME );

  /* Update immediately Occupancy attribute */
  APP_ZIGBEE_AttributeOccupancyUpdate();

  /* USER CODE END APP_ZIGBEE_ApplicationStart */

#if ( CFG_LPM_LEVEL != 0)
  /* Authorize LowPower now */
  UTIL_LPM_SetMaxMode( 1 << CFG_LPM_APP, UTIL_LPM_MAX_MODE );
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

  /* Add Occupancy Server Cluster */
  stZigbeeAppInfo.OccupancyServer = ZbZclOccupancyServerAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT );
  assert( stZigbeeAppInfo.OccupancyServer != NULL );
  if ( ZbZclClusterEndpointRegister( stZigbeeAppInfo.OccupancyServer ) == false )
  {
    LOG_ERROR_APP( "Error during Occupancy Server Endpoint Register." );
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
  /* Not used */

  return false;
}

/**
 * @brief  Return the Startup Configuration
 * @param  pstConfig  Configuration structure to fill
 * @retval None
 */
void APP_ZIGBEE_GetStartupConfig( struct ZbStartupT * pstConfig )
{
  /* Setting to join a zigbee network */
  ZbStartupConfigGetProDefaults( pstConfig );

  /* Using the default HA preconfigured Link Key */
  memcpy( pstConfig->security.preconfiguredLinkKey, sec_key_ha, ZB_SEC_KEYSIZE );

  /* Setting up additional startup configuration parameters */
  pstConfig->startupControl = stZigbeeAppInfo.eStartupControl;
  pstConfig->channelList.count = 1;
  pstConfig->channelList.list[0].page = 0;
  pstConfig->channelList.list[0].channelMask = APP_ZIGBEE_CHANNEL_MASK;

  /* Add 'Sleepy End Device' configuration */
  pstConfig->capability &= ~( MCP_ASSOC_CAP_RXONIDLE | MCP_ASSOC_CAP_DEV_TYPE | MCP_ASSOC_CAP_ALT_COORD );
  pstConfig->endDeviceTimeout = APP_ZIGBEE_SED_SLEEP_TIME;

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
 * @retval None
 */
void APP_ZIGBEE_SetNewDevice( uint16_t iShortAddress, uint64_t dlExtendedAddress )
{
  LOG_INFO_APP( "New Device on Network : with Extended ( " LOG_DISPLAY64() " ) and Short ( 0x%04X ) Address.", LOG_NUMBER64( dlExtendedAddress ), iShortAddress );

  /* USER CODE BEGIN APP_ZIGBEE_SetNewDevice */
  
  /* USER CODE END APP_ZIGBEE_SetNewDevice */
}

/**
 * @brief  Manage a Device on Network that Rejoin Network.
 * @param  iShortAddress      Short Address of Device
 * @param  dlExtendedAddress  Extended Address of Device
 * @retval None
 */
void APP_ZIGBEE_SetRejoinDevice( uint16_t iShortAddress, uint64_t dlExtendedAddress )
{
  LOG_INFO_APP( "Device Rejoin Network : with Extended ( " LOG_DISPLAY64() " ) and Short ( 0x%04X ) Address.", LOG_NUMBER64( dlExtendedAddress ), iShortAddress );

  /* USER CODE BEGIN APP_ZIGBEE_SetRejoinDevice */

  /* USER CODE END APP_ZIGBEE_SetRejoinDevice */
}

/**
 * @brief  Manage a Device on Network that Leave Network.
 * @param  iShortAddress      Short Address of Device
 * @param  dlExtendedAddress  Extended Address of Device
 * @retval None
 */
void APP_ZIGBEE_SetLeaveDevice( uint16_t iShortAddress, uint64_t dlExtendedAddress )
{
  LOG_INFO_APP( "Device Leave Network : with Extended ( " LOG_DISPLAY64() " ) and Short ( 0x%04X ) Address.", LOG_NUMBER64( dlExtendedAddress ), iShortAddress );

  /* USER CODE BEGIN APP_ZIGBEE_SetLeaveDevice */

  /* USER CODE END APP_ZIGBEE_SetLeaveDevice */
}

/**
 * @brief  Print application information to the console
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_PrintApplicationInfo(void)
{
  LOG_INFO_APP( "**********************************************************" );
  LOG_INFO_APP( "Network config : CENTRALIZED END DEVICE" );

  /* USER CODE BEGIN APP_ZIGBEE_PrintApplicationInfo1 */
  LOG_INFO_APP( "Application Flashed : Zigbee %s %s", APP_ZIGBEE_APPLICATION_NAME, APP_ZIGBEE_APPLICATION_OS_NAME );

  /* USER CODE END APP_ZIGBEE_PrintApplicationInfo1 */
  LOG_INFO_APP( "Channel Mask used: 0x%08X.", APP_ZIGBEE_CHANNEL_MASK );

  APP_ZIGBEE_PrintGenericInfo();

  LOG_INFO_APP( "Clusters allocated are:" );
  LOG_INFO_APP( "  %s on Endpoint %d.", APP_ZIGBEE_CLUSTER_NAME, APP_ZIGBEE_ENDPOINT );

  /* USER CODE BEGIN APP_ZIGBEE_PrintApplicationInfo2 */

  /* USER CODE END APP_ZIGBEE_PrintApplicationInfo2 */

  LOG_INFO_APP( "**********************************************************" );
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

/**
 * @brief  Zigbee application Task initialization
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_ApplicationTaskInit( void )
{
  /* Task that Update Pressure AttributeValue */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_ZIGBEE_APP_OCCUPANCY_UPDATE, UTIL_SEQ_RFU, APP_ZIGBEE_AttributeOccupancyUpdate );

  /* Task to Leave Network */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_ZIGBEE_APP_LEAVE_NETWORK, UTIL_SEQ_RFU, APP_ZIGBEE_LeaveNetwork );
}

/**
 * @brief   Finish to Leave Network
 * @param   None
 * @retval  None
 */
static void APP_ZIGBEE_LeaveNetwork( void )
{
  enum ZbStatusCodeT  eStatus;
  uint8_t             cDisableTimers = 1;

  /* First, disable Zigbee Stack periodic timers to avoid any new Zigbee Stack activity */
  eStatus = ZbNwkSet( stZigbeeAppInfo.pstZigbee, ZB_NWK_NIB_ID_DisablePeriodicTimers, &cDisableTimers, sizeof(cDisableTimers) );
  if ( eStatus != ZB_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "Error during command ZbNwkSet (%d)", eStatus );
  }

  /* Indicate 'User' Startup is needed */
  stZigbeeAppInfo.bNwkStartupDone = false;
}

/**
 *
 */
static void APP_BSP_ZbLeaveReqCallback(struct ZbNlmeLeaveConfT * pstConfig, void *arg )
{
  LOG_INFO_APP( "Callback of Leave Request. Status : 0x%02X, Address : " LOG_DISPLAY64() ".", pstConfig->status, LOG_NUMBER64( pstConfig->deviceAddr ) );

  if ( pstConfig->status == ZB_STATUS_SUCCESS )
  {
    LOG_INFO_APP( "Device has left the network." );
    stZigbeeAppInfo.bNwkStartupDone = false;

    UTIL_SEQ_SetTask( 1u << CFG_TASK_ZIGBEE_APP_LEAVE_NETWORK, TASK_ZIGBEE_APP_LEAVE_NETWORK_PRIORITY );
  }
}


/**
 * @brief  Management of the SW1 button. Join or Leave Network depending on the button press duration.
 * @param  None
 * @retval None
 */
void APP_BSP_Button1Action(void)
{
  enum ZbStatusCodeT  eStatus;
  uint8_t             cDisableTimers = 0;

  if ( APP_BSP_ButtonIsLongPressed( B1 ) != false )
  {
    /* Leave Network */
    LOG_INFO_APP( "Send a Leave Request to Zigbee Stack" );

    eStatus = ZbLeaveReq( stZigbeeAppInfo.pstZigbee, APP_BSP_ZbLeaveReqCallback, NULL );
    if ( eStatus != ZB_STATUS_SUCCESS )
    {
      LOG_ERROR_APP("Error during 'ZbLeaveReq'. Status : 0x%02X", eStatus );
    }
  }
  else
  {
    /* Join Network */
    if ( stZigbeeAppInfo.bNwkStartupDone == false )
    {
      LOG_INFO_APP( "Send a Join/Rejoin Request to Zigbee Stack" );

      /* First, re-enable Zigbee Stack periodic timers */
      eStatus = ZbNwkSet( stZigbeeAppInfo.pstZigbee, ZB_NWK_NIB_ID_DisablePeriodicTimers, &cDisableTimers, sizeof(cDisableTimers) );
      if ( eStatus != ZB_STATUS_SUCCESS )
      {
        LOG_ERROR_APP( "Error during command ZbNwkSet (%d)", eStatus );
      }

      /* Launch the startup of the mesh network setup */
      UTIL_SEQ_SetTask( 1U << CFG_TASK_ZIGBEE_NETWORK_FORM, TASK_PRIO_ZIGBEE_NETWORK_FORM );
    }
  }
}


/**
 * @brief  Management of the SW2 button. Indicate room is empty or not.
 * @param  None
 * @retval None
 */
void APP_BSP_Button2Action(void)
{
  /* CHange Occupancy State */
  cOccupancyState = ( cOccupancyState ^ 0x01 );
  UTIL_SEQ_SetTask( 1u << CFG_TASK_ZIGBEE_APP_OCCUPANCY_UPDATE, TASK_ZIGBEE_APP_OCCUPANCY_UPDATE_PRIORITY );
}

/**
 * @brief  Write Occupancy attributes 
 * @param  none
 * @retval None
 */
static void APP_ZIGBEE_AttributeOccupancyUpdate( void )
{
  enum ZclStatusCodeT eStatus;
  
  /*  Update Occupancy State */
  LOG_INFO_APP( "[OCCUPANCY SENSING] Update Occupancy (%d)", cOccupancyState );
  eStatus = ZbZclAttrIntegerWrite( stZigbeeAppInfo.OccupancyServer, ZCL_OCC_ATTR_OCCUPANCY, cOccupancyState );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "[OCCUPANCY SENSING] Occupancy Attribute Write error (0x%02X)", eStatus );
  }
}


/* USER CODE END FD_LOCAL_FUNCTIONS */
