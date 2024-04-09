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
#include "zcl/general/zcl.meter.id.h"

/* USER CODE BEGIN PI */
#include "stm32wbaxx_nucleo.h"

/* USER CODE END PI */

/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_CHANNEL                13u
#define APP_ZIGBEE_CHANNEL_MASK           ( 1u << APP_ZIGBEE_CHANNEL )
#define APP_ZIGBEE_TX_POWER               ((int8_t) 10)    /* TX-Power is at +10 dBm. */

#define APP_ZIGBEE_ENDPOINT               17u
#define APP_ZIGBEE_PROFILE_ID             ZCL_PROFILE_HOME_AUTOMATION
#define APP_ZIGBEE_DEVICE_ID              ZCL_DEVICE_METER_INTERFACE
#define APP_ZIGBEE_GROUP_ADDRESS          0x0001u

#define APP_ZIGBEE_CLUSTER_ID             ZCL_CLUSTER_METER_ID
#define APP_ZIGBEE_CLUSTER_NAME           "MeterId Server"

/* USER CODE BEGIN PD */
#define APP_ZIGBEE_STARTUP_FAIL_DELAY     500u

/* Needed Meter-ID Information defines */
#define METER_ID_COMPANY_NAME             "ST"                // Company Name
#define METER_ID_METER_TYPE_ID            0x0110u             // Generic Meter
#define METER_ID_DATA_QUALITY_ID          0x0003u             // No Certified Data
#define METER_ID_POD                      "T124U56825697SOP"  // Point Of Delivery attribute 
#define METER_ID_AVAILABLE_POWER          (int32_t)9000       // Available Power
#define METER_ID_POWER_THRESHOLD          (int32_t)10000      // Power Threahold before overload.
#define METER_ID_STRING_MAX_SIZE          16                  // Meter-ID Field String can be have up to 16 bytes.

#define APP_ZIGBEE_APPLICATION_NAME       APP_ZIGBEE_CLUSTER_NAME
#define APP_ZIGBEE_APPLICATION_OS_NAME    "."

/* USER CODE END PD */

// -- Redefine Clusters to better code read --
#define MeterIdServer                     pstZbCluster[0]

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private constants ---------------------------------------------------------*/
/* USER CODE BEGIN PC */

/* USER CODE END PC */

/* Private variables ---------------------------------------------------------*/
static uint16_t   iDeviceShortAddress;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */
static void APP_ZIGBEE_MeterIdServerStart         ( void );

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

  /* Start MeterId Server */
  APP_ZIGBEE_MeterIdServerStart();

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

  /* Add MeterId Server Cluster */
  stZigbeeAppInfo.MeterIdServer = ZbZclMeterIdServerAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT );
  assert( stZigbeeAppInfo.MeterIdServer != NULL );
  ZbZclClusterEndpointRegister( stZigbeeAppInfo.MeterIdServer );

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

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

/**
 * @brief  MeterId Server Start
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_MeterIdServerStart(void)
{
  uint8_t   cBufSize;
  uint8_t   szBuffer[METER_ID_STRING_MAX_SIZE + 2u ];
  enum ZclStatusCodeT eStatus;
  
  /* Compute company name string in ZCL string format */
  cBufSize = strlen( METER_ID_COMPANY_NAME );
  memset( szBuffer, 0, ( METER_ID_STRING_MAX_SIZE + 2u ) );
  memcpy( &szBuffer[1], METER_ID_COMPANY_NAME, cBufSize );
  szBuffer[0] = cBufSize;
  
  /* ZCL_METER_ID_ATTR_COMPANY_NAME attribute init */
  LOG_INFO_APP( "[METER ID] Writing 'Company Name' attribute." );
  eStatus = ZbZclAttrStringWriteShort( stZigbeeAppInfo.MeterIdServer, ZCL_METER_ID_ATTR_COMPANY_NAME, (const uint8_t *)&szBuffer );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "[METER ID] Error during writing 'Company Name' (%d).", eStatus );
    while(1) {}
  }
  
  /* ZCL_METER_ID_ATTR_METER_TYPE_ID attribute init */
  LOG_INFO_APP("[METER ID] Writing 'Meter Type ID' attribute.");
  eStatus = ZbZclAttrIntegerWrite( stZigbeeAppInfo.MeterIdServer, ZCL_METER_ID_ATTR_METER_TYPE_ID, METER_ID_METER_TYPE_ID);
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP("[METER ID] Error during writing 'Meter Type ID' (%d).", eStatus);
    while(1) {}
  }

  /* ZCL_METER_ID_ATTR_DATA_QUAL_ID attribute init */
  LOG_INFO_APP("[METER ID] Writing 'Data Quality' attribute.");
  eStatus = ZbZclAttrIntegerWrite( stZigbeeAppInfo.MeterIdServer, ZCL_METER_ID_ATTR_DATA_QUAL_ID, METER_ID_DATA_QUALITY_ID);
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP("[METER ID] Error during writing 'Data Quality' (%d).", eStatus);
    while(1) {}
  }

  /* Compute POD string in ZCL string format */
  cBufSize = strlen( METER_ID_POD );
  memset( szBuffer, 0, ( METER_ID_STRING_MAX_SIZE + 2u ) );
  memcpy( &szBuffer[1], METER_ID_POD, cBufSize );
  szBuffer[0] = cBufSize;
  
  /* ZCL_METER_ID_ATTR_METER_TYPE_ID attribute init */
  LOG_INFO_APP("[METER ID] Writing 'Point Of Delivery' attribute.");
  eStatus = ZbZclAttrStringWriteShort( stZigbeeAppInfo.MeterIdServer, ZCL_METER_ID_ATTR_POD, (const uint8_t *)&szBuffer );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP("[METER ID] Error during writing 'Point Of Delivery' (%d).", eStatus);
    while(1) {}
  }
}

/* USER CODE END FD_LOCAL_FUNCTIONS */
