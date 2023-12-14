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
#include "zcl/general/zcl.therm.h"

/* USER CODE BEGIN PI */
#include "stm32wbaxx_nucleo.h"
#include "app_menu.h"

/* Used to simulate a Temperature Sensor */
#include "zigbee_plat.h"

/* USER CODE END PI */

/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_CHANNEL                13u
#define APP_ZIGBEE_CHANNEL_MASK           ( 1u << APP_ZIGBEE_CHANNEL )
#define APP_ZIGBEE_TX_POWER               ((int8_t) 10)    /* TX-Power is at +10 dBm. */

#define APP_ZIGBEE_ENDPOINT               17u
#define APP_ZIGBEE_PROFILE_ID             ZCL_PROFILE_HOME_AUTOMATION
#define APP_ZIGBEE_DEVICE_ID              ZCL_DEVICE_THERMOSTAT
#define APP_ZIGBEE_GROUP_ADDRESS          0x0001u

#define APP_ZIGBEE_CLUSTER_ID             ZCL_CLUSTER_HVAC_THERMOSTAT
#define APP_ZIGBEE_CLUSTER_NAME           "Thermostat Server"

/* USER CODE BEGIN PD */
#define APP_ZIGBEE_STARTUP_FAIL_DELAY     500u

/* Temperature Measures */
#define TEMP_MEASURED_MIN                 (int16_t)0xF060   /* Sensor limit : -40°C   */
#define TEMP_MEASURED_DEFAULT             (int16_t)0x09C4   /* Default Temp : +25C   */
#define TEMP_MEASURED_MAX                 (int16_t)0x30d4   /* Sensor limit : +125°C  */
#define TEMP_TOLERANCE                    (int16_t)0x0032   /* Temp tolerance : +/-0.5C */
#define TEMP_DISPLAY_SCALE                (int16_t)100u     /* Temperature Display divider */
#define TEMP_INERTIA                      10u               /* Temperature echange every 10 secs */

/* Temperature Simulation */
#define TEMP_SIMU_START_DEFAULT           TEMP_MEASURED_DEFAULT
#define TEMP_SIMU_INCREMENT               (int16_t)0x000A   /* Temp Simutation change : +/-0.1C */

/* Relay State (On/Off) & Play (Cooling/Heating) */
#define RELAY_STATE_OPEN                  0x00u
#define RELAY_STATE_CLOSE                 0x01u

#define RELAY_NO_PLAY                     0x00u
#define RELAY_PLAY_HEATING                0x01u
#define RELAY_PLAY_COOLING                0x02u

#define TEMP_SENSOR_UPDATE_PERIOD         500u              /* 500 ms */

/* Tmperature for menu */
#define MENU_TEMP_MIN                     (int16_t)( TEMP_MEASURED_MIN / 100 ) 
#define MENU_TEMP_MAX                     (int16_t)( TEMP_MEASURED_MAX / 100 )
#define MENU_TIME_TO_CHOICE_TIMEOUT       (uint32_t)( 10u * 1000u )    /* 10 seconds */

#define APP_ZIGBEE_APPLICATION_NAME       APP_ZIGBEE_CLUSTER_NAME
#define APP_ZIGBEE_APPLICATION_OS_NAME    "."

// -- Redefine tasks & priorities to better code read --
#define CFG_TASK_ZIGBEE_APP_SENSOR_READ   CFG_TASK_ZIGBEE_APP1
#define CFG_TASK_ZIGBEE_APP_MENU_UPDATE   CFG_TASK_ZIGBEE_APP2

#define TASK_ZIGBEE_APP_SENSOR_READ_PRIORITY  CFG_SEQ_PRIO_1
#define TASK_ZIGBEE_APP_MENU_UPDATE_PRIORITY  CFG_SEQ_PRIO_1

/* USER CODE END PD */

// -- Redefine Clusters to better code read --
#define ThermostatServer                  pstZbCluster[0]

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private constants ---------------------------------------------------------*/
/* USER CODE BEGIN PC */
/* Menu Lists */
static const AppMenuSentenceT szControlList[] = 
{
  "Cooling only",
  "Cooling + Reheat",
  "Heating only",
  "Heating + Reheat",
  "Cooling and Heating",
  "Cooling and Heating + Reheat",
};

static const AppMenuSentenceT szSysModeList[] = 
{
  "OFF",
  "Auto",
  "Not Used",
  "Cooling",
  "Heating",
  "Ext Heating",
  "Pre-Cooling",
  "Fan",
  "Dry",
  "Sleep",
};

/* Description of Relay State/Play */
static const char szRelayState[2][8] = { "Open", "Close" };
static const char szRelayPlay[3][15] = { "", "for heating", "for cooling" };

/* Link Menu/Attributes */
static const enum ZbZclThermAttrT eMenuAttributesList[] = { ZCL_THERM_SVR_ATTR_OCCUP_COOL_SETPOINT, ZCL_THERM_SVR_ATTR_OCCUP_HEAT_SETPOINT,
                                                            ZCL_THERM_SVR_ATTR_CONTROL_SEQ_OPER, ZCL_THERM_SVR_ATTR_SYSTEM_MODE };

/* USER CODE END PC */

/* Private variables ---------------------------------------------------------*/
static uint16_t   iDeviceShortAddress;

/* USER CODE BEGIN PV */
static UTIL_TIMER_Object_t      stTimerSensorUpdate, stTimerMenuExit;

static uint8_t                  cRelayState, cRelayPlay;
static int16_t                  iTemperatureCurrent;
static int16_t                  iTemperatureCooling, iTemperatureHeating, iControlOper, iSysMode;
static AppMenuButtonsT          eUsedButton;

/* Description of Menu : MenuList, MenuName, List do display, linked variables, values min & max */
static const struct MenuButtonsT stMenuButtonList[] = 
{
  { "Menu Cooling", "Cooling Temp " , NULL,           &iTemperatureCooling, TEMP_MEASURED_MIN,                TEMP_MEASURED_MAX,  TEMP_TOLERANCE, TEMP_DISPLAY_SCALE },
  { "Menu Heating", "Heating Temp " , NULL,           &iTemperatureHeating, TEMP_MEASURED_MIN,                TEMP_MEASURED_MAX,  TEMP_TOLERANCE, TEMP_DISPLAY_SCALE },
  { "Menu Control", "Control: "     , szControlList,  &iControlOper,        ZCL_THERM_CONTROL_OPERCOOL_ONLY,  ZCL_THERM_CONTROL_OPERCOOL_HEAT_REHEAT, 1, 1 },
  { "Menu SysMode", "SysMode: "     , szSysModeList,  &iSysMode,            ZCL_THERM_SYSMODE_OFF,            ZCL_THERM_SYSMODE_SLEEP,                1, 1 },
  { APP_MENU_EXIT,  NULL            , NULL,           NULL,                 0,                                0          },
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* Thermostat Server Callbacks */
static enum ZclStatusCodeT APP_ZIGBEE_ThermostatServerGetRelayStatusLogCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclAddrInfoT * pstSrcInfo );

static struct ZbZclThermServerCallbacksT stThermostatServerCallbacks =
{
  .get_relay_status_log = APP_ZIGBEE_ThermostatServerGetRelayStatusLogCallback,
};

/* USER CODE BEGIN PFP */
static void APP_ZIGBEE_ApplicationTaskInit        ( void );
static void APP_ZIGBEE_ThermostatServerStart      ( void );
static void APP_ZIGBEE_TempSensorRead             ( void );
static void APP_ZIGBEE_MenuUpdate                 ( void );

static void APP_ZIGBEE_TimerSensorUpdateCallback  ( void * arg );
static void APP_ZIGBEE_TimerMenuExitCallback      ( void * arg );


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
  uint16_t  iShortAddress;

  /* Start Thermostat Server */
  APP_ZIGBEE_ThermostatServerStart();

  /* Display Short Address */
  iShortAddress = ZbShortAddress( stZigbeeAppInfo.pstZigbee );
  LOG_INFO_APP( "Use Short Address : 0x%04X", iShortAddress );
  
  LOG_INFO_APP( "%s ready to work !", APP_ZIGBEE_APPLICATION_NAME );
  
  /* Start periodic Temperaure Measure */
  UTIL_TIMER_Start( &stTimerSensorUpdate );

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
  stRequest.profileId = APP_ZIGBEE_PROFILE_ID;
  stRequest.deviceId = APP_ZIGBEE_DEVICE_ID;
  stRequest.endpoint = APP_ZIGBEE_ENDPOINT;
  ZbZclAddEndpoint( stZigbeeAppInfo.pstZigbee, &stRequest, &stConfig );
  assert( stConfig.status == ZB_STATUS_SUCCESS );

  /* Add Thermostat Server Cluster */
  stZigbeeAppInfo.ThermostatServer = ZbZclThermServerAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT, &stThermostatServerCallbacks, NULL );
  assert( stZigbeeAppInfo.ThermostatServer != NULL );
  ZbZclClusterEndpointRegister( stZigbeeAppInfo.ThermostatServer );

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
 * @brief  Thermostat Server 'GetRelayStatusLog' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_ThermostatServerGetRelayStatusLogCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclAddrInfoT * pstSrcInfo )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_ThermostatServerGetRelayStatusLogCallback */

  /* USER CODE END APP_ZIGBEE_ThermostatServerGetRelayStatusLogCallback */
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
  /* Task that Update Temperature AttributeValue */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_ZIGBEE_APP_SENSOR_READ, UTIL_SEQ_RFU, APP_ZIGBEE_TempSensorRead );
  
  /* Task that Manage Menu */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_ZIGBEE_APP_MENU_UPDATE, UTIL_SEQ_RFU, APP_ZIGBEE_MenuUpdate );
  

  /* Create timer to exit of Menu after a TimeOut */
  UTIL_TIMER_Create( &stTimerSensorUpdate, TEMP_SENSOR_UPDATE_PERIOD, UTIL_TIMER_PERIODIC, APP_ZIGBEE_TimerSensorUpdateCallback, NULL );
  
  /* Create timer to get the measure of temperature sensor */
  UTIL_TIMER_Create( &stTimerMenuExit, MENU_TIME_TO_CHOICE_TIMEOUT, UTIL_TIMER_ONESHOT, APP_ZIGBEE_TimerMenuExitCallback, NULL );
}


/**
 * @brief  Start of Thermostat Server
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_ThermostatServerStart( void )
{
  enum ZclStatusCodeT eStatus;
  int8_t   cTemp;
  int16_t  iTemperature;
  
  /* Normally, Read Temperature from sensor */
  
  /* Simulation of a Sensor. Start Temp in default Temp +/- 13°C */
  ZIGBEE_PLAT_RngGet( 1, (uint8_t *)&cTemp );
  iTemperature = TEMP_SIMU_START_DEFAULT + ( cTemp * 10 );
  
  /* Update default values for Thermostats. Diff between heating/cooling setpoint is 5°C and default mode is 'Auto'. */
  iTemperatureCurrent = iTemperature;
  iTemperatureCooling = TEMP_MEASURED_DEFAULT + 100;
  iTemperatureHeating = TEMP_MEASURED_DEFAULT - 400;
  iControlOper = ZCL_THERM_CONTROL_OPERCOOL_HEAT;
  iSysMode = ZCL_THERM_SYSMODE_AUTO;
  cRelayState = RELAY_STATE_OPEN;
  cRelayPlay = RELAY_NO_PLAY;
  
  /* Update Attributes */
  eStatus = ZbZclAttrIntegerWrite( stZigbeeAppInfo.ThermostatServer, ZCL_THERM_SVR_ATTR_LOCAL_TEMP, iTemperatureCurrent );
  eStatus |= ZbZclAttrIntegerWrite( stZigbeeAppInfo.ThermostatServer, ZCL_THERM_SVR_ATTR_OCCUP_COOL_SETPOINT, iTemperatureCooling );
  eStatus |= ZbZclAttrIntegerWrite( stZigbeeAppInfo.ThermostatServer, ZCL_THERM_SVR_ATTR_OCCUP_HEAT_SETPOINT, iTemperatureHeating );
  eStatus |= ZbZclAttrIntegerWrite( stZigbeeAppInfo.ThermostatServer, ZCL_THERM_SVR_ATTR_CONTROL_SEQ_OPER, iControlOper );
  eStatus |= ZbZclAttrIntegerWrite( stZigbeeAppInfo.ThermostatServer, ZCL_THERM_SVR_ATTR_SYSTEM_MODE, iSysMode );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "[THERMOSTAT] Error during an attribute write (0x%02X)", eStatus );
  }  
  
  /* Initialize Menu for Parameters */
  eUsedButton = MENU_BUTTON_NONE;
  APP_MENU_Init( (struct MenuButtonsT *) &stMenuButtonList );
}


/**
 * @brief  Management of the UpdateTimer Callback to launch a Temperature Read Task
 * @param  arg    Argument 
 * @retval None
 */
static void APP_ZIGBEE_TimerSensorUpdateCallback( void * arg )
{
  UTIL_SEQ_SetTask( 1u << CFG_TASK_ZIGBEE_APP_SENSOR_READ, TASK_ZIGBEE_APP_SENSOR_READ_PRIORITY );
}


/**
 * @brief  Exit of the Menu after a TimeOut
 * @param  arg    Argument 
 * @retval None
 */
static void APP_ZIGBEE_TimerMenuExitCallback( void * arg )
{
  eUsedButton = MENU_BUTTON_STOP;
  UTIL_SEQ_SetTask( 1u << CFG_TASK_ZIGBEE_APP_MENU_UPDATE, TASK_ZIGBEE_APP_MENU_UPDATE_PRIORITY );
}


/**
 * @brief  Management of the SW1 button. Play 'Enter' button of Menu.
 * @param  None
 * @retval None
 */
void APPE_Button1Action(void)
{
  eUsedButton = MENU_BUTTON_ENTER;
  UTIL_SEQ_SetTask( 1u << CFG_TASK_ZIGBEE_APP_MENU_UPDATE, TASK_ZIGBEE_APP_MENU_UPDATE_PRIORITY );
}


/**
 * @brief  Management of the SW2 button. Play 'Low' button of Menu
 * @param  None
 * @retval None
 */
void APPE_Button2Action(void)
{
  eUsedButton = MENU_BUTTON_ARROW_LOW;
  UTIL_SEQ_SetTask( 1u << CFG_TASK_ZIGBEE_APP_MENU_UPDATE, TASK_ZIGBEE_APP_MENU_UPDATE_PRIORITY );
}


/**
 * @brief  Management of the SW3 button. Play 'High' button of Menu
 * @param  None
 * @retval None
 */
void APPE_Button3Action(void)
{
  eUsedButton = MENU_BUTTON_ARROW_HIGH;
  UTIL_SEQ_SetTask( 1u << CFG_TASK_ZIGBEE_APP_MENU_UPDATE, TASK_ZIGBEE_APP_MENU_UPDATE_PRIORITY );
}


/**
 * @brief   Management of all buttons for Menu.
 *          If a setting change, update related attribute.
 * @param   None
 * @retval  None
 */
static void APP_ZIGBEE_MenuUpdate( void )
{
  uint8_t             bChange;
  uint16_t            iIndexChange;
  int16_t             iValue;
  char                szText[10];
  enum ZclDataTypeT   eDataType;
  enum ZclStatusCodeT eStatus;
  
  // -- First Stop Timer  --
  UTIL_TIMER_Stop( &stTimerMenuExit );
  
  switch( eUsedButton )
  {
    case MENU_BUTTON_ENTER :  
        bChange = APP_MENU_ButtonEnter( &iIndexChange );
        if ( bChange != FALSE )
        { 
          // -- Found the modified value -- */
          iValue = *(stMenuButtonList[iIndexChange].piOptionValue);
          
          // -- In all case, verify if TempCooling & TempHeating are good */
          if ( ( eMenuAttributesList[iIndexChange] == ZCL_THERM_SVR_ATTR_OCCUP_COOL_SETPOINT ) && ( iValue <= iTemperatureHeating ) )
          {
            sprintf(szText, "%d.%02d", ( iTemperatureHeating / TEMP_DISPLAY_SCALE ),  (uint8_t)( iTemperatureHeating % TEMP_DISPLAY_SCALE ) );
            LOG_ERROR_APP( "ERROR : 'Cooling Temp' setpoint need to be more than 'Heating Temp' setpoint (%s C)", szText );
            iTemperatureCooling = (int16_t) ZbZclAttrIntegerRead(stZigbeeAppInfo.ThermostatServer, ZCL_THERM_SVR_ATTR_OCCUP_COOL_SETPOINT, &eDataType , &eStatus );
          }
          else
          {
            if ( ( eMenuAttributesList[iIndexChange] == ZCL_THERM_SVR_ATTR_OCCUP_HEAT_SETPOINT ) && ( iValue >= iTemperatureCooling ) )
            {
              sprintf(szText, "%d.%02d", ( iTemperatureCooling / TEMP_DISPLAY_SCALE ),  (uint8_t)( iTemperatureCooling % TEMP_DISPLAY_SCALE ) );
              LOG_ERROR_APP( "ERROR : 'Heating Temp' setpoint need to be less than 'Cooling Temp' setpoint (%s C)", szText );
              iTemperatureHeating = (int16_t) ZbZclAttrIntegerRead(stZigbeeAppInfo.ThermostatServer, ZCL_THERM_SVR_ATTR_OCCUP_HEAT_SETPOINT, &eDataType , &eStatus );
            }
            else
            {
              // -- Save the modified value -- */
              eStatus = ZbZclAttrIntegerWrite( stZigbeeAppInfo.ThermostatServer, eMenuAttributesList[iIndexChange], iValue );
              if ( eStatus != ZCL_STATUS_SUCCESS )
              {
                LOG_ERROR_APP( "[THERMOSTAT] Error during an attribute write (0x%02X)", eStatus );
              }
            }
          }
        }
        else
        {
          UTIL_TIMER_Start( &stTimerMenuExit );
        }
        break;
                              
    case MENU_BUTTON_ARROW_LOW: 
        APP_MENU_ButtonArrowLow();
        UTIL_TIMER_Start( &stTimerMenuExit );
        break;
        
    case MENU_BUTTON_ARROW_HIGH: 
        APP_MENU_ButtonArrowHigh();
        UTIL_TIMER_Start( &stTimerMenuExit );
        break;
        
    case MENU_BUTTON_STOP:
        APP_MENU_ButtonStop();
        break;
        
    default :
        break;
  }
        
  eUsedButton = MENU_BUTTON_NONE;
}


/**
 * @brief  Generate a new temperature based on random variation
 * @param  iTempIncrement   Current Temperaturure increment
 * @retval None
 */
static void APP_ZIGBEE_TempSensorGenerateRandom( int16_t iTempIncrement )
{
  uint8_t             cTempVariation; 
  
  ZIGBEE_PLAT_RngGet( 1, &cTempVariation );
  if ( cTempVariation > 128 )
  { 
    iTemperatureCurrent += iTempIncrement; 
    if ( iTemperatureCurrent > TEMP_MEASURED_MAX )
    { 
      iTemperatureCurrent = TEMP_MEASURED_MAX; 
    }
  }
  else
  { 
    iTemperatureCurrent -= iTempIncrement; 
    if ( iTemperatureCurrent < TEMP_MEASURED_MIN )
    { 
      iTemperatureCurrent = TEMP_MEASURED_MIN; 
    }
  }
}

/**
 * @brief  Write locally temp meas attribute 
 * @param  sensor read temp value
 * @retval None
 */
static void APP_ZIGBEE_TempSensorRead( void )
{
  int16_t             iTempIncrement = 0;
  enum ZclStatusCodeT eStatus;
  char                szText[10];
  static uint16_t     iTempInertia = TEMP_INERTIA;
  
  /* Normally, Read Temperature from sensor */
  
  /* Simulation of a Sensor */
  
  /* Simulate Temperature Inertia */
  if ( --iTempInertia == 0x00 )
  { 
    iTempInertia = TEMP_INERTIA;
    iTempIncrement = TEMP_SIMU_INCREMENT;
  }
    
  /* Update Temperature in function of Thermostat state */
  switch ( iSysMode )
  {
    case ZCL_THERM_SYSMODE_COOL :
    case ZCL_THERM_SYSMODE_PRECOOL :
    case ZCL_THERM_SYSMODE_FAN :
        if ( iTemperatureCurrent > iTemperatureCooling )
        {
          iTemperatureCurrent -= iTempIncrement;
          cRelayState = RELAY_STATE_CLOSE;
          cRelayPlay = RELAY_PLAY_COOLING;
        }
        else
        { 
          cRelayState = RELAY_STATE_OPEN;
          cRelayPlay = RELAY_NO_PLAY;
          iTemperatureCurrent += iTempIncrement;
        }
        break;
      
    case ZCL_THERM_SYSMODE_HEAT :
    case ZCL_THERM_SYSMODE_EHEAT :
    case ZCL_THERM_SYSMODE_DRY :
        if ( iTemperatureCurrent < iTemperatureHeating )
        {
          iTemperatureCurrent += iTempIncrement;
          cRelayState = RELAY_STATE_CLOSE;
          cRelayPlay = RELAY_PLAY_HEATING;
        }
        else
        {
          cRelayState = RELAY_STATE_OPEN;
          cRelayPlay = RELAY_NO_PLAY;
          iTemperatureCurrent -= iTempIncrement;
        }
        break;
          
    case ZCL_THERM_SYSMODE_AUTO :
        if ( iTemperatureCurrent < iTemperatureHeating )
        {
          iTemperatureCurrent += iTempIncrement;
          cRelayState = RELAY_STATE_CLOSE;
          cRelayPlay = RELAY_PLAY_HEATING;
        }
        else
        {
          if ( iTemperatureCurrent > iTemperatureCooling )
          {
            iTemperatureCurrent -= iTempIncrement;
            cRelayState = RELAY_STATE_CLOSE;
            cRelayPlay = RELAY_PLAY_COOLING;
          }
          else
          {
            APP_ZIGBEE_TempSensorGenerateRandom( iTempIncrement );
            cRelayState = RELAY_STATE_OPEN;
            cRelayPlay = RELAY_NO_PLAY;
          }
        }
        break;
        
    default : 
        APP_ZIGBEE_TempSensorGenerateRandom( iTempIncrement );
        break;
  }  
  
  /* Display (if not Menu On) & Save Updated Temperature */
  if ( APP_MENU_IsMenuDisplayed() == FALSE )
  {
    sprintf(szText, "%d.%02d", ( iTemperatureCurrent / TEMP_DISPLAY_SCALE ),  (uint8_t)( iTemperatureCurrent % TEMP_DISPLAY_SCALE ) );
    LOG_INFO_APP( "[THERMOSTAT] Update TempMeasure : %s C and Relay is : %s %s", szText, szRelayState[cRelayState], szRelayPlay[cRelayPlay] );
  }
  APP_LED_TOGGLE(LED_GREEN);
  
  eStatus = ZbZclAttrIntegerWrite( stZigbeeAppInfo.ThermostatServer, ZCL_THERM_SVR_ATTR_LOCAL_TEMP, iTemperatureCurrent);
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "[THERMOSTAT] Error during 'Local Temp' attribute write (0x%02X)", eStatus );
  }
}


/* USER CODE END FD_LOCAL_FUNCTIONS */
