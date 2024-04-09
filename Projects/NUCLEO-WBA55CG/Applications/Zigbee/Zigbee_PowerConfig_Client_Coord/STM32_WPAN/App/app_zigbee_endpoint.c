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
#include "zcl/general/zcl.power.config.h"

/* USER CODE BEGIN PI */
#include "stm32wbaxx_nucleo.h"

/* USER CODE END PI */

/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_CHANNEL                14u
#define APP_ZIGBEE_CHANNEL_MASK           ( 1u << APP_ZIGBEE_CHANNEL )
#define APP_ZIGBEE_TX_POWER               ((int8_t) 10)    /* TX-Power is at +10 dBm. */

#define APP_ZIGBEE_ENDPOINT               17u
#define APP_ZIGBEE_PROFILE_ID             ZCL_PROFILE_HOME_AUTOMATION
#define APP_ZIGBEE_DEVICE_ID              ZCL_DEVICE_HOME_GATEWAY
#define APP_ZIGBEE_GROUP_ADDRESS          0x0001u

#define APP_ZIGBEE_CLUSTER_ID             ZCL_CLUSTER_POWER_CONFIG
#define APP_ZIGBEE_CLUSTER_NAME           "PowerConfig Client"

/* USER CODE BEGIN PD */
#define APP_ZIGBEE_STARTUP_FAIL_DELAY     500u

#define APP_ZIGBEE_APPLICATION_NAME       APP_ZIGBEE_CLUSTER_NAME
#define APP_ZIGBEE_APPLICATION_OS_NAME    "."

/* Report defines */
#define SERVER_REPORT_PERIOD_MIN          60u     /* Request a Report every 60s */
#define SERVER_REPORT_PERIOD_MAX          60u
#define SERVER_REPORT_CHANGE              0u      /* Request a Report if a change occurs */

#define SERVER_REPORT_TIME_LAUNCH         (uint32_t)( 20u * 1000u )  /* 20s */

#define BATTERY_MEASURED_RESOL            (uint16_t)100       /* Voltage unit 100 mV */ 
#define BATTERY_PERCENT_RESOL             (uint8_t)2          /* Percent unit 0.5% */

// -- Redefine task to better code read --
#define CFG_TASK_ZIGBEE_APP_LAUNCH_REPORT_REQ       CFG_TASK_ZIGBEE_APP1
#define TASK_ZIGBEE_APP_LAUNCH_REPORT_REQ_PRIORITY  CFG_SEQ_PRIO_1

/* USER CODE END PD */

// -- Redefine Clusters to better code read --
#define PowerConfigClient                 pstZbCluster[0]

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private constants ---------------------------------------------------------*/
/* USER CODE BEGIN PC */

/* USER CODE END PC */

/* Private variables ---------------------------------------------------------*/
static uint16_t   iDeviceShortAddress;

/* USER CODE BEGIN PV */
static UTIL_TIMER_Object_t      stTimerSendReportRequest;
static uint16_t                 iDeviceShortAddress;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */
static void APP_ZIGBEE_ApplicationTaskInit        ( void );
static void APP_ZIGBEE_LaunchReportTimerCallback  ( void * arg );
static void APP_ZIGBEE_ReportConfig               ( void );

static void APP_ZIGBEE_PowerConfigServerReport    ( struct ZbZclClusterT * pstCluster, struct ZbZclHeaderT * pstZclHeader, struct ZbApsdeDataIndT * pstDataInd,
                                                    uint16_t iAttributeId, enum ZclDataTypeT eDataType, const uint8_t * pDataInputPayload, uint16_t iDataInputLength,
                                                    bool * bDiscard );

static enum ZclStatusCodeT APP_ZIGBEE_PowerConfigServerCommand  ( struct ZbZclClusterT * pstCluster, struct ZbZclHeaderT * pstZclHeader, struct ZbApsdeDataIndT * pstDataInd );

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
  uint16_t  iShortAddress;

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

  /* Add PowerConfig Client Cluster */
  stZigbeeAppInfo.PowerConfigClient = ZbZclPowerConfigClientAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT );
  assert( stZigbeeAppInfo.PowerConfigClient != NULL );
  ZbZclClusterEndpointRegister( stZigbeeAppInfo.PowerConfigClient );

  /* USER CODE BEGIN APP_ZIGBEE_ConfigEndpoints2 */
  /* Server Report callback */
  stZigbeeAppInfo.PowerConfigClient->report = &APP_ZIGBEE_PowerConfigServerReport;
  stZigbeeAppInfo.PowerConfigClient->command = &APP_ZIGBEE_PowerConfigServerCommand;

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
  /* Start the Timer to Launch ConfigRequest */
  UTIL_TIMER_Start( &stTimerSendReportRequest ); 

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

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

/**
 * @brief  Zigbee application Task initialization
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_ApplicationTaskInit( void )
{
  /* Task that Launch Report Request */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_ZIGBEE_APP_LAUNCH_REPORT_REQ, UTIL_SEQ_RFU, APP_ZIGBEE_ReportConfig );
  
  /* Create timer to get the measure of environment data */
  UTIL_TIMER_Create( &stTimerSendReportRequest, SERVER_REPORT_TIME_LAUNCH, UTIL_TIMER_ONESHOT, APP_ZIGBEE_LaunchReportTimerCallback, NULL );
}

/**
 * @brief  Management of the LaunchReportTimer Callback to launch a Pressure Read Task
 * @param  arg    Argument 
 * @retval None
 */
static void APP_ZIGBEE_LaunchReportTimerCallback( void * arg )
{
  UTIL_SEQ_SetTask( 1u << CFG_TASK_ZIGBEE_APP_LAUNCH_REPORT_REQ, TASK_ZIGBEE_APP_LAUNCH_REPORT_REQ_PRIORITY );
}

/**
 * @brief  'Report Config' response callback
 * @param  stCommandRsp
 * @param  arg
 * @retval None
 */
static void APP_ZIGBEE_ReportConfigCallback( struct ZbZclCommandRspT * stCommandRsp, void * arg )
{
  if ( stCommandRsp->status != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP("[POWER CONFIG] Report Config Failed error (0x%02X)", stCommandRsp->status);
  }
  else
  {
    LOG_INFO_APP("[POWER CONFIG] Report Config set with success");
  }
}


/**
 * @brief  Send a Report Config Request to Server.
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_ReportConfig(void)
{
  struct ZbZclAttrReportConfigT   stReportConfig;
  enum ZclStatusCodeT             eStatus;
  
  /* Set Report Configuration  */
  memset( &stReportConfig, 0, sizeof( stReportConfig ) );
  
  stReportConfig.dst.mode = ZB_APSDE_ADDRMODE_SHORT;
  stReportConfig.dst.endpoint = (uint16_t)APP_ZIGBEE_ENDPOINT;
  stReportConfig.dst.nwkAddr =  iDeviceShortAddress;
  
  stReportConfig.num_records = 1;
  stReportConfig.record_list[0].direction = ZCL_REPORT_DIRECTION_NORMAL;
  stReportConfig.record_list[0].min = SERVER_REPORT_PERIOD_MIN;
  stReportConfig.record_list[0].max = SERVER_REPORT_PERIOD_MAX;
  stReportConfig.record_list[0].change = SERVER_REPORT_CHANGE;
  stReportConfig.record_list[0].attr_id = ZCL_POWER_CONFIG_ATTR_BATTERY_PCT;
  stReportConfig.record_list[0].attr_type = ZCL_DATATYPE_UNSIGNED_8BIT;
  
  LOG_INFO_APP( "[POWER CONFIG] Send Report Request every %d seconds to 0x%04X", SERVER_REPORT_PERIOD_MAX, iDeviceShortAddress );
  eStatus = ZbZclAttrReportConfigReq( stZigbeeAppInfo.PowerConfigClient, &stReportConfig, &APP_ZIGBEE_ReportConfigCallback, NULL );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "[POWER CONFIG] Error during Report Config Request (0x%02X)", eStatus );
  }
}


static enum ZclStatusCodeT APP_ZIGBEE_PowerConfigServerCommand( struct ZbZclClusterT * pstCluster, struct ZbZclHeaderT * pstZclHeader, struct ZbApsdeDataIndT * pstDataInd )
{
  LOG_INFO_APP("[POWER CONFIG] Command received");
  return( ZCL_STATUS_SUCCESS );
}


/**
 * @brief  Receive Report from Power Config Server
 * @param  cluster, header, dataind, attribute, datatype, datapayload, datalength, discard
 * @retval None
 */
static void APP_ZIGBEE_PowerConfigServerReport( struct ZbZclClusterT * pstCluster, struct ZbZclHeaderT * pstZclHeader, struct ZbApsdeDataIndT * pstDataInd,
                                                uint16_t iAttributeId, enum ZclDataTypeT eDataType, const uint8_t * pDataInputPayload, uint16_t iDataInputLength,
                                                bool * bDiscard )
{
  int       iAttrLen;
  uint8_t   cBatteryPctInt, cBatteryPctDiv;
  int16_t   iAttrValue;

  iAttrLen = ZbZclAttrParseLength( eDataType, pDataInputPayload, pstDataInd->asduLength, 0 );
  if ( iAttrLen < 0 )
  {
    LOG_ERROR_APP( "[POWER CONFIG] Report error length (%d)", iAttrLen );
    return;
  }

  if ( (uint16_t)iAttrLen > iDataInputLength )
  {
    LOG_ERROR_APP( "[POWER CONFIG] Report error length (%d > %d)", iAttrLen, iDataInputLength );
    return;
  }

  if ( pstDataInd->dst.endpoint != APP_ZIGBEE_ENDPOINT ) 
  {
    LOG_ERROR_APP( "[POWER CONFIG] Report error wrong endpoint (%d)", pstDataInd->dst.endpoint );
    return;
  }

  switch ( iAttributeId )
  {
//    case ZCL_POWER_CONFIG_ATTR_BATTERY_VOLTAGE:
//        iAttrValue= (uint16_t)( (uint8_t)pDataInputPayload[0] )  * BATTERY_MEASURED_RESOL;
//        LOG_INFO_APP( "[POWER CONFIG] From 0x%016" PRIX64 ", Battery Voltage value is %d mV", pstDataInd->src.extAddr, iAttrValue );
//        APP_LED_TOGGLE( LED_BLUE );
//        break;
        
    case ZCL_POWER_CONFIG_ATTR_BATTERY_PCT:
        iAttrValue = (uint16_t)( (uint8_t)pDataInputPayload[0] );
        cBatteryPctInt = (uint8_t)( ( iAttrValue * 10 / BATTERY_PERCENT_RESOL ) / 10u );
        cBatteryPctDiv = (uint8_t)( ( iAttrValue * 10 / BATTERY_PERCENT_RESOL ) % 10u );
        LOG_INFO_APP( "[POWER CONFIG] From 0x%016" PRIX64 ", Battery remain value is %d.%d %%", pstDataInd->src.extAddr, cBatteryPctInt, cBatteryPctDiv );
        APP_LED_TOGGLE( LED_BLUE );
        break;

    default:
        break;
  }
}


/* USER CODE END FD_LOCAL_FUNCTIONS */
