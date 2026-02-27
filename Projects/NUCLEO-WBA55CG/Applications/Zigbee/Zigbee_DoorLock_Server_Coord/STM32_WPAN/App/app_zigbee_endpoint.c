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
#include "zcl/general/zcl.time.h"
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

#define APP_ZIGBEE_CLUSTER1_ID            ZCL_CLUSTER_TIME
#define APP_ZIGBEE_CLUSTER1_NAME          "Time Server"

#define APP_ZIGBEE_CLUSTER2_ID            ZCL_CLUSTER_ALARM
#define APP_ZIGBEE_CLUSTER2_NAME          "Alarm Server"

#define APP_ZIGBEE_CLUSTER3_ID            ZCL_CLUSTER_DOOR_LOCK
#define APP_ZIGBEE_CLUSTER3_NAME          "DoorLock Server"

/* Alarm specific defines ----------------------------------------------------*/
#define ZCL_ALARM_LOG_ENTRY_NB            4
/* USER CODE BEGIN Alarm defines */
/* USER CODE END Alarm defines */

#define MAX_PIN_NB_USERS                  5u

/* USER CODE BEGIN PD */
#define APP_ZIGBEE_APPLICATION_NAME       APP_ZIGBEE_CLUSTER3_NAME
#define TIMER_SERVER_1S_NB_TICKS          ( 1u * 1000u )  /* 1 second */

/* DoorLock specific defines -------------------------------------------------*/
#define DOORLOCK_PASSAGE_MODE_DURATION              ( 10u * TIMER_SERVER_1S_NB_TICKS )
#define DOORLOCK_AUTO_RELOCK_TIME                   5u

#define DOORLOCK_DOORSTATE_OPEN                     ZCL_DRLK_DOORSTATE_OPEN
#define DOORLOCK_DOORSTATE_CLOSE                    ZCL_DRLK_DOORSTATE_CLOSED
#define DOORLOCK_DOORSTATE_ERROR_JAMMED             ZCL_DRLK_DOORSTATE_ERROR_JAMMED
#define DOORLOCK_DOORSTATE_ERROR_FORCED_OPEN        ZCL_DRLK_DOORSTATE_ERROR_FORCED
#define DOORLOCK_DOORSTATE_ERROR_UNSPECIFIED        ZCL_DRLK_DOORSTATE_ERROR_UNKNOWN
#define DOORLOCK_DOORSTATE_ERROR_UNDEFINED          0xFFu

#define DOORLOCK_NORMAL_MODE                        ZCL_DRLK_MODE_NORMAL
#define DOORLOCK_VACATION_MODE                      ZCL_DRLK_MODE_VACATION
#define DOORLOCK_PRIVACY_MODE                       ZCL_DRLK_MODE_PRIVACY
#define DOORLOCK_NO_RF_LOCK_UNLOCK_MODE             ZCL_DRLK_MODE_NO_RF_LOCK
#define DOORLOCK_PASSAGE_MODE                       ZCL_DRLK_MODE_PASSAGE

#define DOORLOCK_NORMAL_MODE_SUPPORTED              ( 1u << DOORLOCK_NORMAL_MODE )
#define DOORLOCK_VACATION_MODE_SUPPORTED            ( 1u << DOORLOCK_VACATION_MODE )
#define DOORLOCK_PRIVACY_MODE_SUPPORTED             ( 1u << DOORLOCK_PRIVACY_MODE )
#define DOORLOCK_NO_RF_LOCK_UNLOCK_MODE_SUPPORTED   ( 1u << DOORLOCK_NO_RF_LOCK_UNLOCK_MODE )
#define DOORLOCK_PASSAGE_MODE_SUPPORTED             ( 1u << DOORLOCK_PASSAGE_MODE )

/* Deadbolt Jammed alarm */
#define DOORLOCK_ALARM_DEADBOLT_JAMMED              0x00u
/* Lock Reset to Factory Defaults alarm */
#define DOORLOCK_ALARM_RESET                        0x01u
/* RF Module Power Cycled alarm */
#define DOORLOCK_ALARM_RF_MODULE                    0x03u
/* Tamper Alarm - wrong code entry limit */
#define DOORLOCK_ALARM_WRONG_CODE_LIMIT             0x04u
/* Tamper Alarm - front escutcheon removed from main */
#define DOORLOCK_ALARM_FRONT                        0x05u
/* Forced Door Open under Door Locked Condition alarm */
#define DOORLOCK_ALARM_FORCED_DOOR                  0x06u

// -- Redefine Task to better code read --
#define APP_ZIGBEE_APPLICATION_OS_NAME                            "."

#define CFG_TASK_ZIGBEE_APP_DOORLOCK_PASSAGE_MODE                 CFG_TASK_ZIGBEE_APP1
#define CFG_TASK_ZIGBEE_APP_DOORLOCK_PASSAGE_MODE_DURATION        CFG_TASK_ZIGBEE_APP2
#define CFG_TASK_ZIGBEE_APP_DOORLOCK_AUTO_RELOCK_TIME			        CFG_TASK_ZIGBEE_APP3

#define TASK_ZIGBEE_APP_DOORLOCK_PASSAGE_MODE_PRIORITY            CFG_SEQ_PRIO_1
#define TASK_ZIGBEE_APP_DOORLOCK_PASSAGE_MODE_DURATION_PRIORITY   CFG_SEQ_PRIO_1
#define TASK_ZIGBEE_APP_DOORLOCK_AUTO_RELOCK_TIME_PRIORITY        CFG_SEQ_PRIO_0
/* USER CODE END PD */

// -- Redefine Clusters to better code read --
#define TimeServer                        pstZbCluster[0]
#define AlarmServer                       pstZbCluster[1]
#define DoorLockServer                    pstZbCluster[2]

/* Private typedef -----------------------------------------------------------*/
struct DoorLockInfoT
{
  uint8_t  cDummy;
  
/* USER CODE BEGIN PTD_1 */
  uint8_t 	                      cCurrentNbUsers;
  struct ZbZclDoorLockSetPinReqT 	stUserTable[MAX_PIN_NB_USERS];
/* USER CODE END PTD_1 */
};

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private constants ---------------------------------------------------------*/
/* USER CODE BEGIN PC */
static enum ZclStatusCodeT APP_ZIGBEE_DoorLockServerAttributeCallback   ( struct ZbZclClusterT * pstCluster, struct ZbZclAttrCbInfoT * pstCallbackInfo );

static const struct ZbZclAttrT optionalAttrList[] = 
{
  {
    ZCL_DRLK_ATTR_DOORSTATE,
    ZCL_DATATYPE_UNSIGNED_8BIT,
    //ZCL_ATTR_FLAG_CB_NOTIFY,
    ZCL_ATTR_FLAG_CB_WRITE,
    0,
    APP_ZIGBEE_DoorLockServerAttributeCallback,
    {0, 0},
    {0, 0}
  },
};

/* USER CODE END PC */

/* Private variables ---------------------------------------------------------*/
static struct DoorLockInfoT     stDoorLockInfo;

/* USER CODE BEGIN PV */
static UTIL_TIMER_Object_t      stTimerPassageDuration, stTimerAutoRelockTime;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* Time Server Callbacks */
static uint32_t APP_ZIGBEE_TimeServerGetTimeCallback( struct ZbZclClusterT * pstCluster, void * arg );
static void APP_ZIGBEE_TimeServerSetTimeCallback( struct ZbZclClusterT * pstCluster, uint32_t lTimeValue, void * arg );

static struct ZbZclTimeServerCallbacks stTimeServerCallbacks =
{
  .get_time = APP_ZIGBEE_TimeServerGetTimeCallback,
  .set_time = APP_ZIGBEE_TimeServerSetTimeCallback,
};

/* DoorLock Server Callbacks */
static enum ZclStatusCodeT APP_ZIGBEE_DoorLockServerLockCallback( struct ZbZclClusterT * pstCluster, struct ZbZclDoorLockLockDoorReqT * pstRequest, struct ZbZclAddrInfoT * pstSrcInfo, void * arg );
static enum ZclStatusCodeT APP_ZIGBEE_DoorLockServerUnlockCallback( struct ZbZclClusterT * pstCluster, struct ZbZclDoorLockUnlockDoorReqT * pstRequest, struct ZbZclAddrInfoT * pstSrcInfo, void * arg );
static enum ZclStatusCodeT APP_ZIGBEE_DoorLockServerSetPinCallback( struct ZbZclClusterT * pstCluster, struct ZbZclDoorLockSetPinReqT * pstRequest, struct ZbZclAddrInfoT * pstSrcInfo, void * arg );

static struct ZbZclDoorLockServerCallbacksT stDoorLockServerCallbacks =
{
  .lock = APP_ZIGBEE_DoorLockServerLockCallback,
  .unlock = APP_ZIGBEE_DoorLockServerUnlockCallback,
  .set_pin = APP_ZIGBEE_DoorLockServerSetPinCallback,
};

/* USER CODE BEGIN PFP */
static void APP_ZIGBEE_DoorLockServerStart			            ( void );
static void APP_ZIGBEE_DoorLockServerPassageMode			      ( void );
static void APP_ZIGBEE_DoorLockServerPassageModeEnded       ( void );
static void APP_ZIGBEE_DoorLockServerPassageModeEndedExec	  ( void * arg );
static void APP_ZIGBEE_DoorLockServerAutoRelockTime		      ( void );
static void APP_ZIGBEE_DoorLockServerAutoRelockTimeExec     ( void * arg );

static void APP_ZIGBEE_ApplicationTaskInit                  ( void );
static void APP_ZIGBEE_BindingTableInsert                   ( uint16_t iNetworkAddress, uint8_t cEndpoint, uint16_t iClusterId );
static int8_t APP_ZIGBEE_DoorLockServerSearchPIN            ( struct DoorLockInfoT * pstDoorLockInfo, uint8_t * pPinCode, uint8_t cPinCodeLength );
static int8_t APP_ZIGBEE_DoorLockServerSearchUserID         ( struct DoorLockInfoT * pstDoorLockInfo, uint16_t iUserId );

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
  /* Start DoorLock Server */
  APP_ZIGBEE_DoorLockServerStart();

  /* Display Short Address */
  LOG_INFO_APP( "Use Short Address : 0x%04X", ZbShortAddress( stZigbeeAppInfo.pstZigbee ) );  
  LOG_INFO_APP( "%s ready to work !", APP_ZIGBEE_APPLICATION_NAME );

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

  /* Add Time Server Cluster */
  stZigbeeAppInfo.TimeServer = ZbZclTimeServerAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT, &stTimeServerCallbacks, NULL );
  assert( stZigbeeAppInfo.TimeServer != NULL );
  if ( ZbZclClusterEndpointRegister( stZigbeeAppInfo.TimeServer ) == false )
  {
    LOG_ERROR_APP( "Error during Time Server Endpoint Register." );
  }

  /* Add Alarm Server Cluster */
  stZigbeeAppInfo.AlarmServer = ZbZclAlarmServerAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT, ZCL_ALARM_LOG_ENTRY_NB, stZigbeeAppInfo.TimeServer );
  assert( stZigbeeAppInfo.AlarmServer != NULL );
  if ( ZbZclClusterEndpointRegister( stZigbeeAppInfo.AlarmServer ) == false )
  {
    LOG_ERROR_APP( "Error during Alarm Server Endpoint Register." );
  }

  /* Add DoorLock Server Cluster */
  stZigbeeAppInfo.DoorLockServer = ZbZclDoorLockServerAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT, &stDoorLockServerCallbacks, &stDoorLockInfo );
  assert( stZigbeeAppInfo.DoorLockServer != NULL );
  if ( ZbZclClusterEndpointRegister( stZigbeeAppInfo.DoorLockServer ) == false )
  {
    LOG_ERROR_APP( "Error during DoorLock Server Endpoint Register." );
  }

  /* USER CODE BEGIN APP_ZIGBEE_ConfigEndpoints2 */
  /* Registering attributes */
  (void)ZbZclAttrAppendList( stZigbeeAppInfo.DoorLockServer, optionalAttrList, ZCL_ATTR_LIST_LEN( optionalAttrList ) );

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
  LOG_INFO_APP( "  %s on Endpoint %d.", APP_ZIGBEE_CLUSTER1_NAME, APP_ZIGBEE_ENDPOINT );
  LOG_INFO_APP( "  %s on Endpoint %d.", APP_ZIGBEE_CLUSTER2_NAME, APP_ZIGBEE_ENDPOINT );
  LOG_INFO_APP( "  %s on Endpoint %d.", APP_ZIGBEE_CLUSTER3_NAME, APP_ZIGBEE_ENDPOINT );

  /* USER CODE BEGIN APP_ZIGBEE_PrintApplicationInfo2 */

  /* USER CODE END APP_ZIGBEE_PrintApplicationInfo2 */

  LOG_INFO_APP( "**********************************************************" );
}

/**
 * @brief  Time Server 'GetTime' command Callback
 */
static uint32_t APP_ZIGBEE_TimeServerGetTimeCallback( struct ZbZclClusterT * pstCluster, void * arg )
{
  static uint32_t   status = 0x01;
  /* USER CODE BEGIN APP_ZIGBEE_TimeServerGetTimeCallback */
  status = HAL_GetTick() * TIMER_SERVER_1S_NB_TICKS;

  /* USER CODE END APP_ZIGBEE_TimeServerGetTimeCallback */
  return status;
}

/**
 * @brief  Time Server 'SetTime' command Callback
 */
static void APP_ZIGBEE_TimeServerSetTimeCallback( struct ZbZclClusterT * pstCluster, uint32_t lTimeValue, void * arg )
{

  /* USER CODE BEGIN APP_ZIGBEE_TimeServerSetTimeCallback */
  /* Unused function */
  UNUSED(pstCluster);
  UNUSED(lTimeValue);
  UNUSED(arg);

  /* USER CODE END APP_ZIGBEE_TimeServerSetTimeCallback */

}

/**
 * @brief  DoorLock Server 'Lock' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_DoorLockServerLockCallback( struct ZbZclClusterT * pstCluster, struct ZbZclDoorLockLockDoorReqT * pstRequest, struct ZbZclAddrInfoT * pstSrcInfo, void * arg )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_DoorLockServerLockCallback */
  struct ZbZclDoorLockLockDoorRspT  stResponse = {.status = ZCL_DRLK_STATUS_FAIL};
  struct DoorLockInfoT  * pstDoorlockInfo = ( struct DoorLockInfoT * ) arg;
  uint16_t              iDoorlLockState;
  uint16_t              iCurrentMode;
  int8_t                cUserPosition = 0;

  /* Get the lock operating mode */
  iCurrentMode = (uint16_t)ZbZclAttrIntegerRead( stZigbeeAppInfo.DoorLockServer, ZCL_DRLK_ATTR_MODE, NULL, &eStatus );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "Error reading local attribute: cannot read 'operating mode' (0x%02X).", eStatus );
    (void) ZbZclDoorLockServerSendLockRsp( pstCluster, pstSrcInfo, &stResponse, NULL, NULL );
    return ZCL_STATUS_SUCCESS_NO_DEFAULT_RESPONSE;
  }

  LOG_INFO_APP( "[DOORLOCK] Door 'Lock' requested ..." );

  /* Read the lock state */
  iDoorlLockState = (uint16_t) ZbZclAttrIntegerRead( stZigbeeAppInfo.DoorLockServer, ZCL_DRLK_ATTR_LOCKSTATE, NULL, &eStatus );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "Error reading local attribute: cannot read 'lock state' (0x%02X).", eStatus );
    (void) ZbZclDoorLockServerSendLockRsp( pstCluster, pstSrcInfo, &stResponse, NULL, NULL );
    return ZCL_STATUS_SUCCESS_NO_DEFAULT_RESPONSE;
  }

  if ( iDoorlLockState == ZCL_DRLK_LOCKSTATE_LOCKED )
  {
    /* Lock already locked */
    LOG_INFO_APP( "[DOORLOCK] Door already 'Locked' !" );
    (void) ZbZclDoorLockServerSendLockRsp( pstCluster, pstSrcInfo, &stResponse, NULL, NULL );
    return ZCL_STATUS_SUCCESS_NO_DEFAULT_RESPONSE;
  }

  /* Passage mode behaviour */
  if ( iCurrentMode == DOORLOCK_PASSAGE_MODE )
  {
    LOG_INFO_APP( "[DOORLOCK] 'Passage Mode' is activated." );

    /* Lock is unlocked -> locked it  */
    eStatus = ZbZclAttrIntegerWrite( stZigbeeAppInfo.DoorLockServer, ZCL_DRLK_ATTR_LOCKSTATE, ZCL_DRLK_LOCKSTATE_LOCKED );
    if ( eStatus != ZCL_STATUS_SUCCESS )
    {
      LOG_ERROR_APP( "Error writing local attribute: cannot set 'lock state' to 'locked' (0x%02X).", eStatus );
      (void) ZbZclDoorLockServerSendLockRsp( pstCluster, pstSrcInfo, &stResponse, NULL, NULL );
      return ZCL_STATUS_SUCCESS_NO_DEFAULT_RESPONSE;
    }

    stResponse.status = ZCL_DRLK_STATUS_SUCCESS;
    APP_LED_ON(LED_GREEN);
    LOG_INFO_APP( "[DOORLOCK] Door is now 'Locked' ! Green Led 'ON'");    

    (void) ZbZclDoorLockServerSendLockRsp( pstCluster, pstSrcInfo, &stResponse, NULL, NULL );
    return ZCL_STATUS_SUCCESS_NO_DEFAULT_RESPONSE;
  }

  /* Check pin length */
  if ( ( pstRequest->pin_len > ZCL_DRLK_MAX_PIN_LEN ) || ( pstRequest->pin_len < ZCL_DRLK_MIN_PIN_LEN ) )
  {
    LOG_ERROR_APP( "Error: Wrong pin format." );
    (void) ZbZclDoorLockServerSendLockRsp( pstCluster, pstSrcInfo, &stResponse, NULL, NULL );
    return ZCL_STATUS_SUCCESS_NO_DEFAULT_RESPONSE;
  }

  /* Check if the PIN is valid */
  cUserPosition = APP_ZIGBEE_DoorLockServerSearchPIN( pstDoorlockInfo, pstRequest->pin, pstRequest->pin_len );
  if ( cUserPosition < 0 )
  {
    LOG_ERROR_APP( "Error: PIN code doesn't match." );
    (void) ZbZclDoorLockServerSendLockRsp( pstCluster, pstSrcInfo, &stResponse, NULL, NULL );
    return ZCL_STATUS_SUCCESS_NO_DEFAULT_RESPONSE;
  }
  LOG_INFO_APP( "[DOORLOCK] Request from User No%d.", pstDoorlockInfo->stUserTable[cUserPosition].user_id );

  /* Lock is unlocked -> locked it  */
  eStatus = ZbZclAttrIntegerWrite(stZigbeeAppInfo.DoorLockServer, ZCL_DRLK_ATTR_LOCKSTATE, ZCL_DRLK_LOCKSTATE_LOCKED );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "Error writing local attribute: cannot set 'lock state' to 'locked' (0x%02X).", eStatus );
    (void) ZbZclDoorLockServerSendLockRsp( pstCluster, pstSrcInfo, &stResponse, NULL, NULL );
    return ZCL_STATUS_SUCCESS_NO_DEFAULT_RESPONSE;
  }

  stResponse.status = ZCL_DRLK_STATUS_SUCCESS;
  APP_LED_ON(LED_GREEN);
  LOG_INFO_APP( "[DOORLOCK] Door is now 'Locked' ! Green Led 'ON'" );

  /* Stop the Auto Relock Time associated timer */
  UTIL_TIMER_Stop( &stTimerAutoRelockTime );

  (void) ZbZclDoorLockServerSendLockRsp( pstCluster, pstSrcInfo, &stResponse, NULL, NULL );
  eStatus = ZCL_STATUS_SUCCESS_NO_DEFAULT_RESPONSE;

  /* USER CODE END APP_ZIGBEE_DoorLockServerLockCallback */
  return eStatus;
}

/**
 * @brief  DoorLock Server 'Unlock' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_DoorLockServerUnlockCallback( struct ZbZclClusterT * pstCluster, struct ZbZclDoorLockUnlockDoorReqT * pstRequest, struct ZbZclAddrInfoT * pstSrcInfo, void * arg )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_DoorLockServerUnlockCallback */
  struct ZbZclDoorLockUnlockDoorRspT stResponse = {.status = ZCL_DRLK_STATUS_FAIL};
  struct DoorLockInfoT  * pstDoorlockInfo = ( struct DoorLockInfoT * ) arg;
  uint32_t              lAutoRelockTime;
  uint16_t              iDoorlLockState;
  uint16_t              iCurrentMode = 0;
  int8_t                cUserPosition = 0;

  /* Get the lock operating mode */
  iCurrentMode = (uint8_t)ZbZclAttrIntegerRead( stZigbeeAppInfo.DoorLockServer, ZCL_DRLK_ATTR_MODE, NULL, &eStatus );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "Error reading local attribute ; cannot read 'operating mode' (0x%02X).", eStatus );
    (void) ZbZclDoorLockServerSendUnlockRsp( pstCluster, pstSrcInfo, &stResponse, NULL, NULL );
    return ZCL_STATUS_SUCCESS_NO_DEFAULT_RESPONSE;
  }

  LOG_INFO_APP( "[DOORLOCK] Door 'Unlock' requested ..." );

  /* Read the lock state */
  iDoorlLockState = ZbZclAttrIntegerRead( stZigbeeAppInfo.DoorLockServer, ZCL_DRLK_ATTR_LOCKSTATE, NULL, &eStatus );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "Error reading local attribute: cannot read 'lock state' (0x%02X).", eStatus );
    (void) ZbZclDoorLockServerSendUnlockRsp( pstCluster, pstSrcInfo, &stResponse, NULL, NULL );
    return ZCL_STATUS_SUCCESS_NO_DEFAULT_RESPONSE;
  }

  if ( iDoorlLockState == ZCL_DRLK_LOCKSTATE_UNLOCKED )
  {
    /* Lock already unlocked */
    LOG_INFO_APP( "[DOORLOCK] Door already 'Unlocked' !" );
    (void) ZbZclDoorLockServerSendUnlockRsp( pstCluster, pstSrcInfo, &stResponse, NULL, NULL );
    return ZCL_STATUS_SUCCESS_NO_DEFAULT_RESPONSE;
  }

  /* Passage mode behaviour */
  if ( iCurrentMode == DOORLOCK_PASSAGE_MODE )
  {
    LOG_INFO_APP( "[DOORLOCK] 'Passage Mode' is activated." );

    /* Lock is locked -> unlocked it  */
    eStatus = ZbZclAttrIntegerWrite(stZigbeeAppInfo.DoorLockServer, ZCL_DRLK_ATTR_LOCKSTATE, ZCL_DRLK_LOCKSTATE_UNLOCKED);
    if ( eStatus != ZCL_STATUS_SUCCESS )
    {
      LOG_ERROR_APP( "Error writing local attribute: cannot set 'lock state' to 'unlocked' (0x%02X).", eStatus );
      (void) ZbZclDoorLockServerSendUnlockRsp( pstCluster, pstSrcInfo, &stResponse, NULL, NULL );
      return ZCL_STATUS_SUCCESS_NO_DEFAULT_RESPONSE;
    }
    
    stResponse.status = ZCL_DRLK_STATUS_SUCCESS;
    APP_LED_OFF(LED_GREEN);
    LOG_INFO_APP( "[DOORLOCK] Door is now 'Unlocked' ! Green Led 'OFF'" );
    

    (void) ZbZclDoorLockServerSendUnlockRsp( pstCluster, pstSrcInfo, &stResponse, NULL, NULL );
    return ZCL_STATUS_SUCCESS_NO_DEFAULT_RESPONSE;
  }

  /* Check pin length */
  if ( ( pstRequest->pin_len > ZCL_DRLK_MAX_PIN_LEN ) || ( pstRequest->pin_len < ZCL_DRLK_MIN_PIN_LEN ) )
  {
    LOG_ERROR_APP( "Error: Wrong pin format." );
    (void) ZbZclDoorLockServerSendUnlockRsp( pstCluster, pstSrcInfo, &stResponse, NULL, NULL );
    return ZCL_STATUS_SUCCESS_NO_DEFAULT_RESPONSE;
  }

  /* Check if the PIN is valid */
  cUserPosition = APP_ZIGBEE_DoorLockServerSearchPIN( pstDoorlockInfo, pstRequest->pin, pstRequest->pin_len );
  if ( cUserPosition < 0 )
  {
    LOG_ERROR_APP( "Error: Pin doesn't match." );
    (void) ZbZclDoorLockServerSendUnlockRsp( pstCluster, pstSrcInfo, &stResponse, NULL, NULL );
    return ZCL_STATUS_SUCCESS_NO_DEFAULT_RESPONSE;
  }
  LOG_INFO_APP( "[DOORLOCK] Request from User No%d.", pstDoorlockInfo->stUserTable[cUserPosition].user_id );

  /* Lock is locked -> unlocked it  */
  eStatus = ZbZclAttrIntegerWrite(stZigbeeAppInfo.DoorLockServer, ZCL_DRLK_ATTR_LOCKSTATE, ZCL_DRLK_LOCKSTATE_UNLOCKED );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP("Error writing local attribute: cannot set lock state to 'unlocked'.\n");
    (void) ZbZclDoorLockServerSendUnlockRsp( pstCluster, pstSrcInfo, &stResponse, NULL, NULL );
    return ZCL_STATUS_SUCCESS_NO_DEFAULT_RESPONSE;
  }

  /* Read the auto relock time value */
  lAutoRelockTime = ZbZclAttrIntegerRead(stZigbeeAppInfo.DoorLockServer, ZCL_DRLK_ATTR_AUTO_RELOCK, NULL, &eStatus );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "Error reading local attribute: cannot read 'lock state' (0x%02X).", eStatus );
    (void) ZbZclDoorLockServerSendUnlockRsp( pstCluster, pstSrcInfo, &stResponse, NULL, NULL );
    return ZCL_STATUS_SUCCESS_NO_DEFAULT_RESPONSE;
  }

  /* Start Auto Relock timer with auto relock time attribute value */
  UTIL_TIMER_StartWithPeriod( &stTimerAutoRelockTime, ( lAutoRelockTime * TIMER_SERVER_1S_NB_TICKS ) );

  stResponse.status = ZCL_DRLK_STATUS_SUCCESS;
  APP_LED_OFF(LED_GREEN);
  LOG_INFO_APP("[DOORLOCK] Door 'Unlocked' for %d seconds ! Green Led 'OFF'", lAutoRelockTime );

  (void) ZbZclDoorLockServerSendUnlockRsp( pstCluster, pstSrcInfo, &stResponse, NULL, NULL );
  eStatus = ZCL_STATUS_SUCCESS_NO_DEFAULT_RESPONSE;

  /* USER CODE END APP_ZIGBEE_DoorLockServerUnlockCallback */
  return eStatus;
}

/**
 * @brief  DoorLock Server 'SetPin' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_DoorLockServerSetPinCallback( struct ZbZclClusterT * pstCluster, struct ZbZclDoorLockSetPinReqT * pstRequest, struct ZbZclAddrInfoT * pstSrcInfo, void * arg )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_DoorLockServerSetPinCallback */
  struct ZbZclDoorLockSetPinRspT  stResponse = {.status = ZCL_DRLK_STATUS_FAIL};
  struct DoorLockInfoT  * pstDoorLockInfo = ( struct DoorLockInfoT * ) arg;
  
  LOG_INFO_APP( "[DOORLOCK] Set PIN requested ..." );

  /* Check if we can add a new PIN user */
  if ( pstDoorLockInfo->cCurrentNbUsers >= MAX_PIN_NB_USERS )
  {
    LOG_ERROR_APP( "Error: PIN users limit reached." );
    stResponse.status = ZCL_DRLK_STATUS_MEM_FULL;
    (void) ZbZclDoorLockServerSendSetPinRsp( pstCluster, pstSrcInfo, &stResponse, NULL, NULL );
    return ZCL_STATUS_SUCCESS_NO_DEFAULT_RESPONSE;
  }

  /* Check if the user id is unique */
  if ( APP_ZIGBEE_DoorLockServerSearchUserID( pstDoorLockInfo, pstRequest->user_id ) >= 0 )
  {
    LOG_ERROR_APP( "Error: User with ID %d already registered.", pstRequest->user_id );
    stResponse.status = ZCL_DRLK_STATUS_DUPLICATE;
    (void) ZbZclDoorLockServerSendSetPinRsp( pstCluster, pstSrcInfo, &stResponse, NULL, NULL );
    return ZCL_STATUS_SUCCESS_NO_DEFAULT_RESPONSE;
  }

  /* PIN length check */
  if ( ( pstRequest->pin_len > ZCL_DRLK_MAX_PIN_LEN ) || ( pstRequest->pin_len < ZCL_DRLK_MIN_PIN_LEN ) )
  {
    LOG_ERROR_APP( "Error: Wrong pin format." );
    (void) ZbZclDoorLockServerSendSetPinRsp( pstCluster, pstSrcInfo, &stResponse, NULL, NULL );
    return ZCL_STATUS_SUCCESS_NO_DEFAULT_RESPONSE;
  }

  /* Check the unicity of the PIN */
  if ( APP_ZIGBEE_DoorLockServerSearchPIN( pstDoorLockInfo, pstRequest->pin, pstRequest->pin_len ) >= 0 )
  {
    LOG_ERROR_APP( "Error: User with this PIN already registered." );
    stResponse.status = ZCL_DRLK_STATUS_DUPLICATE;
    (void) ZbZclDoorLockServerSendSetPinRsp( pstCluster, pstSrcInfo, &stResponse, NULL, NULL );
    return ZCL_STATUS_SUCCESS_NO_DEFAULT_RESPONSE;
  }

  /* create the user */
  pstDoorLockInfo->stUserTable[pstDoorLockInfo->cCurrentNbUsers].user_id = pstRequest->user_id;
  pstDoorLockInfo->stUserTable[pstDoorLockInfo->cCurrentNbUsers].user_status = pstRequest->user_status;
  pstDoorLockInfo->stUserTable[pstDoorLockInfo->cCurrentNbUsers].user_type = pstRequest->user_type;
  memcpy( pstDoorLockInfo->stUserTable[pstDoorLockInfo->cCurrentNbUsers].pin, pstRequest->pin, pstRequest->pin_len );
  pstDoorLockInfo->stUserTable[pstDoorLockInfo->cCurrentNbUsers].pin_len = pstRequest->pin_len;
  pstDoorLockInfo->cCurrentNbUsers++;
  LOG_INFO_APP("[DOORLOCK] PIN set for User No%d !", pstRequest->user_id );

  /* Add Alarm client in the server binding table if not present for receiving Alarm notifications */
  /* DoorLock and Alarm cluster are mapped in the same endpoint */
  APP_ZIGBEE_BindingTableInsert( pstSrcInfo->addr.nwkAddr, pstSrcInfo->addr.endpoint, ZCL_CLUSTER_ALARMS );

  stResponse.status = ZCL_DRLK_STATUS_SUCCESS;
  (void) ZbZclDoorLockServerSendSetPinRsp( pstCluster, pstSrcInfo, &stResponse, NULL, NULL );
  eStatus = ZCL_STATUS_SUCCESS_NO_DEFAULT_RESPONSE;

  /* USER CODE END APP_ZIGBEE_DoorLockServerSetPinCallback */
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
  /* Task associated with Door Lock passage mode */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_ZIGBEE_APP_DOORLOCK_PASSAGE_MODE, UTIL_SEQ_RFU, APP_ZIGBEE_DoorLockServerPassageMode );
  UTIL_SEQ_RegTask( 1U << CFG_TASK_ZIGBEE_APP_DOORLOCK_PASSAGE_MODE_DURATION, UTIL_SEQ_RFU, APP_ZIGBEE_DoorLockServerPassageModeEnded );
  UTIL_SEQ_RegTask( 1U << CFG_TASK_ZIGBEE_APP_DOORLOCK_AUTO_RELOCK_TIME, UTIL_SEQ_RFU, APP_ZIGBEE_DoorLockServerAutoRelockTime );
  
  /* Timers associated with Door Lock passage operating mode */
  UTIL_TIMER_Create( &stTimerPassageDuration, DOORLOCK_PASSAGE_MODE_DURATION, UTIL_TIMER_ONESHOT, APP_ZIGBEE_DoorLockServerPassageModeEndedExec, NULL );
  UTIL_TIMER_Create( &stTimerAutoRelockTime, ( DOORLOCK_AUTO_RELOCK_TIME * TIMER_SERVER_1S_NB_TICKS ), UTIL_TIMER_ONESHOT, APP_ZIGBEE_DoorLockServerAutoRelockTimeExec, NULL );
}


/**
 * @brief  DoorLock Server start
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_DoorLockServerStart(void)
{
  enum ZclStatusCodeT eStatus;

  /* Init the PIN user struct */
  memset( &stDoorLockInfo, 0, sizeof( stDoorLockInfo ) );

  /* At startup, the lock is locked */
  eStatus = ZbZclAttrIntegerWrite( stZigbeeAppInfo.DoorLockServer, ZCL_DRLK_ATTR_LOCKSTATE, ZCL_DRLK_LOCKSTATE_LOCKED );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "Error writing local attribute: cannot set lock state to 'locked' (0x%02X). ", eStatus );
    while(1) {}
  }

  /* Door state is closed */
  eStatus = ZbZclAttrIntegerWrite( stZigbeeAppInfo.DoorLockServer, ZCL_DRLK_ATTR_DOORSTATE, DOORLOCK_DOORSTATE_CLOSE );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP("Error writing local attribute: cannot set Door state is 'closed' (0x%02X).", eStatus );
    while(1) {}
  }

  /* Setting up 'supported mode' */
  eStatus = ZbZclAttrIntegerWrite( stZigbeeAppInfo.DoorLockServer, ZCL_DRLK_ATTR_SUPPORTED_MODES,
                                   (uint16_t)( DOORLOCK_NORMAL_MODE_SUPPORTED | DOORLOCK_PASSAGE_MODE_SUPPORTED ) );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP ("Error writing local attribute: cannot set 'supported mode' (0x%02X).", eStatus );
    while(1) {}
  }

  /* Setting up operating mode attribute */
  eStatus = ZbZclAttrIntegerWrite( stZigbeeAppInfo.DoorLockServer, ZCL_DRLK_ATTR_MODE, DOORLOCK_NORMAL_MODE );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP("Error writing local attribute: cannot set 'operating mode' (0x%02X).", eStatus );
    while(1) {}
  }

  /* Setting up auto relock time attribute */
  eStatus = ZbZclAttrIntegerWrite( stZigbeeAppInfo.DoorLockServer, ZCL_DRLK_ATTR_AUTO_RELOCK, DOORLOCK_AUTO_RELOCK_TIME );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP("Error writing local attribute: cannot set 'relock time' (0x%02X).", eStatus );
    while(1) {}
  }

  /* Setting up alarm bitmap */
  eStatus = ZbZclAttrIntegerWrite( stZigbeeAppInfo.DoorLockServer, ZCL_DRLK_ATTR_ALARM_MASK, ( 1u << DOORLOCK_ALARM_FORCED_DOOR ) );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP("Error writing local attribute: cannot set 'alarm' (0x%02X).", eStatus );
    while(1) {}
  }

  APP_LED_ON(LED_GREEN);
  LOG_INFO_APP( "DoorLock state initialized to 'Locked'. Green Led 'ON'." );
}

/**
 * @brief  Adding entry in binding table
 * @param  iNetworkAddress  Network address
 * @param  cEndpoint        Endpoint
 * @param  iClusterId       ZCL cluster ID
 * @retval None
 */
static void APP_ZIGBEE_BindingTableInsert( uint16_t iNetworkAddress, uint8_t cEndpoint, uint16_t iClusterId )
{
  enum ZbStatusCodeT      eStatus;
  struct ZbApsmeBindT     stCurrentBind;
  struct ZbApsmeBindReqT  stBindRequest;
  struct ZbApsmeBindConfT stBindConfig;
  uint32_t  lBindIndex = 0;
  bool      bFound = false;

  /* Check if this Network address with this particular endpoint is in the Binding Table */
  do
  {
    /* Get the next entry in the binding table. */
    eStatus = ZbApsGetIndex(stZigbeeAppInfo.pstZigbee, ZB_APS_IB_ID_BINDING_TABLE, &stCurrentBind, sizeof(stCurrentBind), lBindIndex++ );
    if ( eStatus == ZB_APS_STATUS_SUCCESS )
    {
      if ( stCurrentBind.srcExtAddr != 0u )
      {
        if ( ( stCurrentBind.clusterId == iClusterId ) && ( stCurrentBind.dst.nwkAddr == iNetworkAddress ) && ( stCurrentBind.dst.endpoint == cEndpoint ) )
        {
          /* matching found, exit */
          bFound = true;
        }
      }
    }
  }
  while ( ( eStatus == ZB_APS_STATUS_SUCCESS ) && ( bFound == false ) );

  /* If not found, it's a new insert in the Binding Table */
  if ( bFound == false )
  {
    /* Binding request */
    memset( &stBindRequest, 0, sizeof( stBindRequest ) );
    memset( &stBindConfig, 0, sizeof(stBindConfig) );
    stBindRequest.srcExtAddr = ZbExtendedAddress(stZigbeeAppInfo.pstZigbee);
    stBindRequest.srcEndpt = APP_ZIGBEE_ENDPOINT;
    stBindRequest.clusterId = iClusterId;
    stBindRequest.dst.mode = ZB_APSDE_ADDRMODE_EXT;
    stBindRequest.dst.endpoint = cEndpoint;
    stBindRequest.dst.extAddr = ZbNwkAddrLookupExt( stZigbeeAppInfo.pstZigbee, iNetworkAddress );

    ZbApsmeBindReq( stZigbeeAppInfo.pstZigbee, &stBindRequest, &stBindConfig );
    if( stBindConfig.status != ZB_WPAN_STATUS_SUCCESS )
    {
      LOG_ERROR_APP("[DOORLOCK] Local Bind failed on DoorLock Cluster (0x%02X).", stBindConfig.status ); 
    }

    LOG_INFO_APP("[DOORLOCK] Binding client with Extended Address: " LOG_DISPLAY64() " and Endpoint: %d.", LOG_NUMBER64( stBindRequest.dst.extAddr ), stBindRequest.dst.endpoint );
  }
}


/**
 * @brief  DoorLock server search user by ID
 * @param  pstDoorLockInfo  Door Lock server info
 * @param  iUserId          ZCL Door Lock user ID
 * @retval User index in the table if found (-1 otherwise)
 */
static int8_t APP_ZIGBEE_DoorLockServerSearchUserID( struct DoorLockInfoT * pstDoorLockInfo, uint16_t iUserId )
{
  int8_t 	  cPosition = -1;
  int8_t 	  cIndex = 0;
  bool      bFound = false;

  while ( ( cIndex < pstDoorLockInfo->cCurrentNbUsers ) && ( bFound == false ) )
  {
    if ( pstDoorLockInfo->stUserTable[cIndex].user_id == iUserId )
    {
      /* User Id found */
      cPosition = cIndex;
      bFound = true;
    }
    
    cIndex++;
  }
  

  return cPosition;
}


/**
 * @brief  DoorLock server search user by PIN
 * @param  pstDoorLockInfo  Door Lock server info
 * @param  pPinCode         PIN code to search
 * @param  cPinCodeLength   PIN code length
 * @retval User index in the table if found (-1 otherwise)
 */
static int8_t APP_ZIGBEE_DoorLockServerSearchPIN(struct DoorLockInfoT * pstDoorLockInfo, uint8_t * pPinCode, uint8_t cPinCodeLength )
{
  int8_t 	  cPosition = -1;
  int8_t	  cIndex = 0;
  bool      bFound = false;
  
  while ( ( cIndex < pstDoorLockInfo->cCurrentNbUsers ) && ( bFound == false ) )
  {
    if ( ( pstDoorLockInfo->stUserTable[cIndex].pin_len == cPinCodeLength ) && ( memcmp( pstDoorLockInfo->stUserTable[cIndex].pin, pPinCode, cPinCodeLength ) == 0u ) )
    {
      /* Pin Code found */
      cPosition = cIndex;
      bFound = true;
    }
    cIndex++;
  }
  

  return cPosition;
}


/**
 * @brief  Door Lock server attribute callback
 * @param  pstCluster       ZCL cluster pointer
 * @param  pstCallbackInfo  Callback info
 * @retval ZCL status code
 */
static enum ZclStatusCodeT APP_ZIGBEE_DoorLockServerAttributeCallback(struct ZbZclClusterT * pstCluster, struct ZbZclAttrCbInfoT * pstCallbackInfo )
{
  uint8_t             cCurrentDoorState;
  uint16_t            iAlarmMask;
  enum ZclStatusCodeT eStatus;

  if ( pstCluster != stZigbeeAppInfo.DoorLockServer )
    { return ZCL_STATUS_FAILURE; }
  
  if ( pstCallbackInfo->type != ZCL_ATTR_CB_TYPE_WRITE )
    { return ZCL_STATUS_FAILURE; }
  
  switch( pstCallbackInfo->info->attributeId )
  {
  	  case ZCL_DRLK_ATTR_DOORSTATE:
  		  /* Getting the door state */
        cCurrentDoorState = *pstCallbackInfo->zcl_data;

  		  if ( cCurrentDoorState == DOORLOCK_DOORSTATE_ERROR_FORCED_OPEN )
        {
          /* Getting the alarm mask */
          iAlarmMask = (uint16_t)ZbZclAttrIntegerRead( stZigbeeAppInfo.DoorLockServer, ZCL_DRLK_ATTR_ALARM_MASK, NULL, &eStatus );
          if ( eStatus != ZCL_STATUS_SUCCESS )
          {
            LOG_ERROR_APP( "Error reading local attribute (0x%02X).", eStatus );
            return ZCL_STATUS_FAILURE;
          }

          /* Check the door state */
          if ( ( ( 1u << DOORLOCK_ALARM_FORCED_DOOR ) & iAlarmMask ) != 0u )
          {
            LOG_ERROR_APP( "[DOORLOCK] Problematic value detected for the door state." );
            ZbZclClusterSendAlarm( stZigbeeAppInfo.DoorLockServer, ZbZclClusterGetEndpoint( stZigbeeAppInfo.DoorLockServer ), cCurrentDoorState );
            LOG_ERROR_APP( "[ALARM] Alarm sent for DoorLock cluster clients that are registered in the Binding Table." );
          }
        }
  		  return ZCL_STATUS_SUCCESS;

  	  default:
  		  return ZCL_STATUS_SUCCESS;
  }
}

/**
 * @brief  DoorLock server Passage mode
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_DoorLockServerPassageMode(void)
{
  enum ZclStatusCodeT eStatus = ZCL_STATUS_FAILURE;
  uint16_t  iSupportedModes = 0;
  uint8_t   cCurrentMode = 0;

  /* Get current operating mode */
  cCurrentMode = (uint8_t)ZbZclAttrIntegerRead(stZigbeeAppInfo.DoorLockServer, ZCL_DRLK_ATTR_MODE, NULL, &eStatus );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "Error reading local attribute." );
    return;
  }

  if ( cCurrentMode == DOORLOCK_PASSAGE_MODE )
  {
    /* Already in passage mode*/
    return;
  }

  /* Get available operating modes */
  iSupportedModes = (uint16_t)ZbZclAttrIntegerRead( stZigbeeAppInfo.DoorLockServer, ZCL_DRLK_ATTR_SUPPORTED_MODES, NULL, &eStatus );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "Error reading local attribute." );
    return;
  }

  /* Check if passage mode is available */
  if ( ( iSupportedModes >> DOORLOCK_PASSAGE_MODE ) == 0u )
  {
    LOG_ERROR_APP( "Error passage mode not available." );
    return;
  }

  /* Activate passage mode */
  eStatus = ZbZclAttrIntegerWrite( stZigbeeAppInfo.DoorLockServer, ZCL_DRLK_ATTR_MODE, DOORLOCK_PASSAGE_MODE );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "Error writing local attribute: cannot set operating mode." );
  }

  /* Start passage mode duration timer */
  UTIL_TIMER_StartWithPeriod( &stTimerPassageDuration, DOORLOCK_PASSAGE_MODE_DURATION );

  BSP_LED_On(LED_WORK);
  LOG_INFO_APP( "[DOORLOCK] 'Passage Mode' activated. Led 'ON'." );
}


/**
 * @brief  DoorLock server stop Passage mode
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_DoorLockServerPassageModeEnded(void)
{
  enum ZclStatusCodeT eStatus;

  /* Operating mode set to normal */
  eStatus = ZbZclAttrIntegerWrite(stZigbeeAppInfo.DoorLockServer, ZCL_DRLK_ATTR_MODE, DOORLOCK_NORMAL_MODE);
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "Error writing local attribute: cannot set 'operating mode' (0x%02X).", eStatus );
  }

  APP_LED_OFF(LED_WORK);
  LOG_INFO_APP( "[DOORLOCK] 'Passage Mode' end. Led 'OFF'" );
}


/**
 * @brief  DoorLock server Passage mode end task launcher wrapper
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_DoorLockServerPassageModeEndedExec( void * arg )
{
  UNUSED(arg);

  /* Start the 'DoorLock Passage Duration' change */
  UTIL_SEQ_SetTask( 1U << CFG_TASK_ZIGBEE_APP_DOORLOCK_PASSAGE_MODE_DURATION, TASK_ZIGBEE_APP_DOORLOCK_PASSAGE_MODE_DURATION_PRIORITY );
}

/**
 * @brief  DoorLock server Auto Relock Time reached
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_DoorLockServerAutoRelockTime(void)
{
  enum ZclStatusCodeT eStatus;

  LOG_INFO_APP("[DOORLOCK] 'Auto Relock' time reached.");

  /* Lock the door */
  eStatus = ZbZclAttrIntegerWrite(stZigbeeAppInfo.DoorLockServer, ZCL_DRLK_ATTR_LOCKSTATE, ZCL_DRLK_LOCKSTATE_LOCKED);
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "Error writing local attribute: cannot set lock state to 'unlocked' (0x%02X).", eStatus);
    return;
  }

  APP_LED_ON(LED_GREEN);
  LOG_INFO_APP("[DOORLOCK] Door is now 'Locked'. Green Led 'ON'");
}


/**
 * @brief  DoorLock server Auto Relock Time reached task launcher wrapper
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_DoorLockServerAutoRelockTimeExec( void * arg )
{
  UNUSED(arg);

  /* Start the Auto-Relock task */
  UTIL_SEQ_SetTask( 1U << CFG_TASK_ZIGBEE_APP_DOORLOCK_AUTO_RELOCK_TIME, TASK_ZIGBEE_APP_DOORLOCK_AUTO_RELOCK_TIME_PRIORITY );
}

/**
 * @brief  Management of the SW1 button (Door Forced Open)
 * @param  None
 * @retval None
 */
void APP_BSP_Button1Action(void)
{
  enum ZclStatusCodeT eStatus;

  /* First, verify if Appli has already Join a Network  */ 
  if ( APP_ZIGBEE_IsAppliJoinNetwork() != false )
  {
    /* Door state is forced opened */
    LOG_INFO_APP( "[DOORLOCK] Try to force to open door." );
    eStatus = ZbZclAttrIntegerWrite( stZigbeeAppInfo.DoorLockServer, ZCL_DRLK_ATTR_DOORSTATE, DOORLOCK_DOORSTATE_ERROR_FORCED_OPEN );
    if ( eStatus != ZCL_STATUS_SUCCESS )
    {
      LOG_ERROR_APP( "Error writing local attribute (0x%02X).", eStatus );
      while(1) {}
    }
  }
}

/**
 * @brief  Management of the SW2 button (Passage Mode)
 * @param  None
 * @retval None
 */
void APP_BSP_Button2Action(void)
{
  /* First, verify if Appli has already Join a Network  */ 
  if ( APP_ZIGBEE_IsAppliJoinNetwork() != false )
  {
    /* Start the 'DoorLock Passage Mode' change */
    UTIL_SEQ_SetTask( 1U << CFG_TASK_ZIGBEE_APP_DOORLOCK_PASSAGE_MODE, TASK_ZIGBEE_APP_DOORLOCK_PASSAGE_MODE_PRIORITY );
  }
}

/* USER CODE END FD_LOCAL_FUNCTIONS */
