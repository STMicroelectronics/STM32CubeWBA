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
#include "zcl/general/zcl.power.config.h"

/* USER CODE BEGIN PI */
#include "stm32wbaxx_nucleo.h"
#include "stm32wbaxx_ll_adc.h"

/* Used to simulate a Power */
#include "zigbee_plat.h"

/* USER CODE END PI */

/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_CHANNEL                14u
#define APP_ZIGBEE_CHANNEL_MASK           ( 1u << APP_ZIGBEE_CHANNEL )
#define APP_ZIGBEE_TX_POWER               ((int8_t) 10)    /* TX-Power is at +10 dBm. */

#define APP_ZIGBEE_ENDPOINT               17u
#define APP_ZIGBEE_PROFILE_ID             ZCL_PROFILE_HOME_AUTOMATION
#define APP_ZIGBEE_DEVICE_ID              ZCL_DEVICE_ENVIRONMENTAL_SENSOR

#define APP_ZIGBEE_CLUSTER_ID             ZCL_CLUSTER_POWER_CONFIG
#define APP_ZIGBEE_CLUSTER_NAME           "PowerConfig Server"

#define APP_ZIGBEE_ZED_SLEEP_TIME         1u            /* For a ZED, Sleep Time Unit is 30 seconds. */

/* USER CODE BEGIN PD */
#define APP_ZIGBEE_STARTUP_FAIL_DELAY     500u

#define APP_ZIGBEE_APPLICATION_NAME       APP_ZIGBEE_CLUSTER_NAME
#define APP_ZIGBEE_APPLICATION_OS_NAME    "."

/* Report defines */
#define BATTERY_VOLTAGE_RESOL             100u        /* Attribute Voltage unit 100 mV */ 
#define BATTERY_PERCENT_RESOL             2u          /* Attribute Percent unit 0.5% */
#define BATTERY_MEASURED_UPDATE_PERIOD    (uint32_t)( 30u * 1000u ) /* 30s */

#define BATTERY_VOLTAGE_MAX               3600u       /* Voltage in mV */
#define BATTERY_PERCENT_MAX               100u        /* Percent in % */

// -- Redefine task to better code read --
#define CFG_TASK_ZIGBEE_APP_BATTERY_READ        CFG_TASK_ZIGBEE_APP1
#define TASK_ZIGBEE_APP_BATTERY_READ_PRIORITY   CFG_SEQ_PRIO_1

/* USER CODE END PD */

// -- Redefine Clusters to better code read --
#define PowerConfigServer                 pstZbCluster[0]

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private constants ---------------------------------------------------------*/
/* USER CODE BEGIN PC */

/* USER CODE END PC */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static uint16_t             iBatteryVoltage;
static UTIL_TIMER_Object_t  stTimerUpdateMeasure;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */
static void APP_ZIGBEE_ApplicationTaskInit        ( void );
static void APP_ZIGBEE_AttributeBatteryUpdate     ( void );
static void APP_ZIGBEE_TimerUpdateCallback        ( void * arg );

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
  /* Display Short Address */
  LOG_INFO_APP( "Use Short Address : 0x%04X", ZbShortAddress( stZigbeeAppInfo.pstZigbee ) );
  LOG_INFO_APP( "%s ready to work !", APP_ZIGBEE_APPLICATION_NAME );

  /* Update default Battery Voltage */
  iBatteryVoltage = BATTERY_VOLTAGE_MAX - (2u * BATTERY_VOLTAGE_RESOL );

  /* Update immediately Battery Measure */
  APP_ZIGBEE_AttributeBatteryUpdate();

  /* Start periodic Battery Measure */
  UTIL_TIMER_Start( &stTimerUpdateMeasure );

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

  /* Add PowerConfig Server Cluster */
  stZigbeeAppInfo.PowerConfigServer = ZbZclPowerConfigServerAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT );
  assert( stZigbeeAppInfo.PowerConfigServer != NULL );
  ZbZclClusterEndpointRegister( stZigbeeAppInfo.PowerConfigServer );

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
  /* Attempt to join a zigbee network */
  ZbStartupConfigGetProDefaults( pstConfig );

  /* Using the default HA preconfigured Link Key */
  memcpy( pstConfig->security.preconfiguredLinkKey, sec_key_ha, ZB_SEC_KEYSIZE );

  /* Setting up additional startup configuration parameters */
  pstConfig->startupControl = stZigbeeAppInfo.eStartupControl;
  pstConfig->channelList.count = 1;
  pstConfig->channelList.list[0].page = 0;
  pstConfig->channelList.list[0].channelMask = APP_ZIGBEE_CHANNEL_MASK;

  /* Add End Device configuration */
  pstConfig->capability &= ~( MCP_ASSOC_CAP_RXONIDLE | MCP_ASSOC_CAP_DEV_TYPE | MCP_ASSOC_CAP_ALT_COORD );
  pstConfig->endDeviceTimeout = APP_ZIGBEE_ZED_SLEEP_TIME;

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
  LOG_INFO_APP( "Network config : CENTRALIZED END DEVICE" );

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
  /* Task that Update Pressure AttributeValue */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_ZIGBEE_APP_BATTERY_READ, UTIL_SEQ_RFU, APP_ZIGBEE_AttributeBatteryUpdate );
  
  /* Create timer to get the measure of pressure sensor */
  UTIL_TIMER_Create( &stTimerUpdateMeasure, BATTERY_MEASURED_UPDATE_PERIOD, UTIL_TIMER_PERIODIC, APP_ZIGBEE_TimerUpdateCallback, NULL );
}

/**
 * @brief  Management of the SW1 button. Start periodic measure of Battery.
 * @param  None
 * @retval None
 */
void APPE_Button1Action(void)
{
  UTIL_TIMER_Start( &stTimerUpdateMeasure ); 
}

/**
 * @brief  Management of the SW2 button. Stop periodic measure of Battery.
 * @param  None
 * @retval None
 */
void APPE_Button2Action(void)
{
  UTIL_TIMER_Stop( &stTimerUpdateMeasure );
}


/**
 * @brief  Management of the UpdateTimer Callback to launch a Pressure Read Task
 * @param  arg    Argument 
 * @retval None
 */
static void APP_ZIGBEE_TimerUpdateCallback( void * arg )
{
  UTIL_SEQ_SetTask( 1u << CFG_TASK_ZIGBEE_APP_BATTERY_READ, TASK_ZIGBEE_APP_BATTERY_READ_PRIORITY );
}

/**
 * @brief  Write locallyBattery Voltage & Percent attributes 
 * @param  none
 * @retval None
 */
static void APP_ZIGBEE_AttributeBatteryUpdate( void )
{
  uint8_t             cBatteryVariation;
#if (CFG_LOG_SUPPORTED != 0)
  uint8_t             cBatteryPctInt, cBatteryPctDiv;
#endif /* (CFG_LOG_SUPPORTED != 0) */
  uint16_t            iBatteryPercent; 
  enum ZclStatusCodeT eStatus;
  
  /* Indicate Update start */
  APP_LED_ON(LED_GREEN);
  
#ifdef BOARD_IS_WITH_PRESS_SENSOR
  /* Read Voltage from Battery */
  
#else // BOARD_IS_WITH_PRESS_SENSOR
  /* Simulation of a Battery  */
  ZIGBEE_PLAT_RngGet( 1, &cBatteryVariation );
  if ( cBatteryVariation > 128u )
  { 
    iBatteryVoltage += BATTERY_VOLTAGE_RESOL; 
  }
  else
  { 
    iBatteryVoltage -= BATTERY_VOLTAGE_RESOL; 
  }
#endif // BOARD_IS_WITH_PRESS_SENSOR
  
  /* Verify if Pressure on limits */
  if ( iBatteryVoltage > BATTERY_VOLTAGE_MAX )
  { 
    iBatteryVoltage = BATTERY_VOLTAGE_MAX; 
  }
    
  /* Calc also Battery percent */
  iBatteryPercent = ( iBatteryVoltage * 100u * BATTERY_PERCENT_RESOL ) / BATTERY_VOLTAGE_MAX;
#if (CFG_LOG_SUPPORTED != 0)  
  cBatteryPctInt = (uint8_t)( ( iBatteryPercent * 10u / BATTERY_PERCENT_RESOL ) / 10u );
  cBatteryPctDiv = (uint8_t)( ( iBatteryPercent * 10u / BATTERY_PERCENT_RESOL ) % 10u );
  
  LOG_INFO_APP( "[POWER CONFIG] Update Battery Voltage ( %d mV ) and Percent ( %d.%d %% )", iBatteryVoltage, cBatteryPctInt, cBatteryPctDiv );
#endif /* (CFG_LOG_SUPPORTED != 0) */
  
  /*  Update Voltage Attribute */
  eStatus = ZbZclAttrIntegerWrite( stZigbeeAppInfo.PowerConfigServer, ZCL_POWER_CONFIG_ATTR_BATTERY_VOLTAGE, (uint8_t)( iBatteryVoltage / BATTERY_VOLTAGE_RESOL ) );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "[POWER CONFIG] Voltage Attribute Write error (0x%02X)", eStatus );
  }
  
  /*  Upodate Percent Attribute */
  eStatus = ZbZclAttrIntegerWrite( stZigbeeAppInfo.PowerConfigServer, ZCL_POWER_CONFIG_ATTR_BATTERY_PCT, (uint8_t)iBatteryPercent );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "[POWER CONFIG] Percent Attribute Write error (0x%02X)", eStatus );
  }
  
  /* Indicate Update is finished */
  APP_LED_OFF(LED_GREEN);
}


/* USER CODE END FD_LOCAL_FUNCTIONS */
