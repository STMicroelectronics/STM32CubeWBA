/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_zigbee.c
  * Description        : Zigbee Application common code.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "app_zigbee_endpoint.h"
#include "dbg_trace.h"
#include "ieee802154_enums.h"
#include "mcp_enums.h"

#include "stm32_rtos.h"
#include "stm32_timer.h"

#include "zigbee.h"
#include "zigbee.nwk.h"
#include "zigbee.security.h"

#include "zcl/zcl.h"

/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN PI */
#include "app_bsp.h"
#include "app_zigbee_debug_zd.h"
#include "stm32_adv_trace.h"
/* USER CODE END PI */

/* Public variables -----------------------------------------------------------*/
ZigbeeAppInfo_t                                     stZigbeeAppInfo;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_STARTUP_FAIL_DELAY               500u        // Time (in ms) between two tentative to Join a Coord/Router.
#define APP_ZIGBEE_STARTUP_WAIT_JOINT_DELAY         1000u       // Time (in ms) between two Timer callback during the time after the Join (17 s).

/* Defines for Basic Cluster Server */
#define APP_ZIGBEE_MFR_NAME                         "STMicroelectronics"
#define APP_ZIGBEE_CHIP_NAME                        "STM32WBA"
#define APP_ZIGBEE_CHIP_VERSION                     0x20        // Cut 2.0
#define APP_ZIGBEE_BOARD_POWER                      0x00        // No Power

#define APP_ZIGBEE_APP_DATE_CODE                    "20240915"
#define APP_ZIGBEE_APP_BUILD_ID                     "V1.6"
#define APP_ZIGBEE_APP_VERSION                      0x16        // Application Version v1.5
#define APP_ZIGBEE_STACK_VERSION                    0x10        // Stack Version v1.0

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private constants ---------------------------------------------------------*/
/* USER CODE BEGIN PC */

/* USER CODE END PC */

/* Private function prototypes -----------------------------------------------*/
static enum ZbStatusCodeT ZbStartupWait       ( struct ZigBeeT * zb, struct ZbStartupT * pstConfig );

static void APP_ZIGBEE_ConfigBasicServer      ( void );
static void APP_ZIGBEE_TraceError             ( const char * pMess, uint32_t lErrCode );
static void APP_ZIGBEE_ConfigMeshNetwork      ( void );
static void APP_ZIGBEE_NwkFormWaitElapsed     ( void * arg );
static void APP_ZIGBEE_NwkFormWaitJoinElapsed ( void * arg );
static void APP_ZIGBEE_Printf                 ( struct ZigBeeT * zb, uint32_t lMask, const char * pHeader, const char * pFrame, va_list argptr );

static enum zb_msg_filter_rc APP_ZIGBEE_DeviceJointCallback   ( struct ZigBeeT * zb, uint32_t lId, void * pMessage, void * arg );

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private variabless -----------------------------------------------*/
static enum ZbStatusCodeT       eZbStartupWaitStatus;
static UTIL_TIMER_Object_t      stNwkFormWaitTimer, stNwkFormWaitJoinTimer;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Functions Definition ------------------------------------------------------*/

/**
 * @brief  Initialize 'Network Form or Join' Task
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_NwkFormOrJoinTaskInit( void )
{
  /* First, create the Timer service to relaunch the Network Form when not requested */
  UTIL_TIMER_Create( &stNwkFormWaitTimer, APP_ZIGBEE_STARTUP_FAIL_DELAY, UTIL_TIMER_ONESHOT, &APP_ZIGBEE_NwkFormWaitElapsed, NULL );

  if ( stZigbeeAppInfo.eStartupControl == ZbStartTypeJoin )
  {
    /* Create the Timer service to can advertise user during the time after the Join (by default 17 seconds) */
    UTIL_TIMER_Create( &stNwkFormWaitJoinTimer, APP_ZIGBEE_STARTUP_WAIT_JOINT_DELAY, UTIL_TIMER_PERIODIC, &APP_ZIGBEE_NwkFormWaitJoinElapsed, NULL );
  }

  /* Create the Task associated with network creation process */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_ZIGBEE_NETWORK_FORM, UTIL_SEQ_RFU, APP_ZIGBEE_NwkFormOrJoin );

  /* launch the startup of the mesh network setup */
  UTIL_SEQ_SetTask( 1U << CFG_TASK_ZIGBEE_NETWORK_FORM, TASK_PRIO_ZIGBEE_NETWORK_FORM );
}

/**
 * @brief  Configure Zigbee Basic Server Cluster
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_ConfigBasicServer( void )
{
  static struct ZbZclBasicServerDefaults   stBasicServerDefaults;

  /* Initialize Basic Server Cluster 'defaults' information */
  memset( &stBasicServerDefaults, 0x00, sizeof(stBasicServerDefaults) );

  stBasicServerDefaults.mfr_name[0] = sizeof( APP_ZIGBEE_MFR_NAME );
  memcpy( &stBasicServerDefaults.mfr_name[1], APP_ZIGBEE_MFR_NAME, sizeof( APP_ZIGBEE_MFR_NAME ) );

  stBasicServerDefaults.model_name[0] = sizeof( APP_ZIGBEE_CHIP_NAME );
  memcpy( &stBasicServerDefaults.model_name[1], APP_ZIGBEE_CHIP_NAME, sizeof( APP_ZIGBEE_CHIP_NAME ) );

  stBasicServerDefaults.date_code[0] = sizeof( APP_ZIGBEE_APP_DATE_CODE );
  memcpy( &stBasicServerDefaults.date_code[1], APP_ZIGBEE_APP_DATE_CODE, sizeof( APP_ZIGBEE_APP_DATE_CODE ) );

  stBasicServerDefaults.sw_build_id[0] = sizeof( APP_ZIGBEE_APP_BUILD_ID );
  memcpy( &stBasicServerDefaults.sw_build_id[1], APP_ZIGBEE_APP_BUILD_ID, sizeof( APP_ZIGBEE_APP_BUILD_ID ) );

  /* Version are on 8 bits : 3 bits for Major version and 5 bits for Minor version */
  stBasicServerDefaults.app_version = (uint8_t)( ( APP_ZIGBEE_APP_VERSION & 0x70u ) << 1u ) | ( APP_ZIGBEE_APP_VERSION & 0x0Fu );
  stBasicServerDefaults.stack_version = (uint8_t)( ( APP_ZIGBEE_STACK_VERSION & 0x70u ) << 1u ) | ( APP_ZIGBEE_STACK_VERSION & 0x0Fu );
  stBasicServerDefaults.hw_version = (uint8_t)( ( APP_ZIGBEE_CHIP_VERSION & 0x70u ) << 1u ) | ( APP_ZIGBEE_CHIP_VERSION & 0x0Fu );

  stBasicServerDefaults.power_source = APP_ZIGBEE_BOARD_POWER;

  /* Configure default Basic Server */
  ZbZclBasicServerConfigDefaults( stZigbeeAppInfo.pstZigbee , &stBasicServerDefaults );
}

/**
 * @brief  Initialize Zigbee stack layers
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_StackLayersInit( void )
{
  struct ZbInitTblSizesT stTableSizes;

  /* Initialise internal table sizes */
  memset( &stTableSizes, 0, sizeof( stTableSizes ) );
  stTableSizes.heapPtr = NULL;
  stTableSizes.heapSz = 0U;

  /* Initialise Zigbee */
  stZigbeeAppInfo.pstZigbee = ZbInit( 0U, &stTableSizes, NULL );
  assert(stZigbeeAppInfo.pstZigbee != NULL);

  /* Configure Zigbee Logging with log Error/Warning/Info/Debug */
  ZbSetLogging( stZigbeeAppInfo.pstZigbee, ZIGBEE_CONFIG_LOG_LEVEL, APP_ZIGBEE_Printf );

  /* Configure Application Basic Server */
  APP_ZIGBEE_ConfigBasicServer();

  /* Create the endpoint and cluster(s) */
  APP_ZIGBEE_ConfigEndpoints();

  /* USER CODE BEGIN APP_ZIGBEE_StackLayersInit1 */
  /* All Led Off at startup */
  APP_LED_OFF(LED_RED);
  APP_LED_OFF(LED_GREEN);
  APP_LED_OFF(LED_BLUE);
  /* -- Initialize UART -- */
  UTIL_ADV_TRACE_StartRxProcess(APP_ZIGBEE_App_UartRxCallback);

  /* USER CODE END APP_ZIGBEE_StackLayersInit1 */

  /* Configure the joining parameters */
  stZigbeeAppInfo.eJoinStatus = (enum ZbStatusCodeT) 0x01;  /* init to error status */
  stZigbeeAppInfo.lJoinDelay = HAL_GetTick();               /* now */

  /* Initialization Complete */
  stZigbeeAppInfo.bHasInit = true;

  /* Print Application information */
  APP_ZIGBEE_PrintApplicationInfo();

  if ( stZigbeeAppInfo.bPersistNotification != false )
    { APP_ZIGBEE_PersistenceStartup(); }

  if ( stZigbeeAppInfo.bNwkStartup != false )
  {
    /* Create the NwkFormOrJoin Task */
    APP_ZIGBEE_NwkFormOrJoinTaskInit();
  }
  else
  {
    /* Start directly Zigbee Application */
    /* USER CODE BEGIN APP_ZIGBEE_StackLayersInit2 */
    APP_LED_OFF(LED_BLUE);

    /* USER CODE END APP_ZIGBEE_StackLayersInit2 */
    APP_ZIGBEE_ConfigMeshNetwork();
    APP_ZIGBEE_ApplicationStart();
  }
}

/**
  * @brief  Callback triggered when the Timer between two Join tentative expire
  * @param  arg : Not used
  * @retval None
  */
static void APP_ZIGBEE_NwkFormWaitElapsed( void * arg )
{
  UNUSED( arg );

  LOG_INFO_APP( "Waiting time between two 'Join' tentatives Elapsed." );

  /* Stop Timer that can advertise user during 'Join' waiting time */
  if ( stZigbeeAppInfo.eStartupControl == ZbStartTypeJoin )
  {
    UTIL_TIMER_Stop( &stNwkFormWaitJoinTimer );
  }

  UTIL_SEQ_SetTask( 1U << CFG_TASK_ZIGBEE_NETWORK_FORM, TASK_PRIO_ZIGBEE_NETWORK_FORM );
}

/**
  * @brief  Callback triggered when the 'Join Wait' Timer expire
  * @param  arg : Not used
  * @retval None
  */
static void APP_ZIGBEE_NwkFormWaitJoinElapsed( void  * arg )
{
  UNUSED( arg );

  if ( stZigbeeAppInfo.eJoinStatus != ZB_STATUS_SUCCESS )
  {
    /* USER CODE BEGIN APP_ZIGBEE_NwkFormWaitJoinElapsed */
    /* Blinck Led to indicate to user that a Join is 'On Going' */
    APP_LED_TOGGLE( LED_BLUE );

    /* USER CODE END APP_ZIGBEE_NwkFormWaitJoinElapsed */
  }
  else
  {
    UTIL_TIMER_Stop( &stNwkFormWaitJoinTimer );
    /* USER CODE BEGIN APP_ZIGBEE_NwkFormWaitJoinElapsed_2 */
    APP_LED_OFF( LED_BLUE );

    /* USER CODE END APP_ZIGBEE_NwkFormWaitJoinElapsed_2 */
  }
}

/**
 * @brief  Handle Zigbee network forming and joining
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_NwkFormOrJoin(void)
{
  struct ZbStartupT   stConfig;

  if ( stZigbeeAppInfo.eJoinStatus != ZB_STATUS_SUCCESS )
  {
    /* Application configure Startup */
    APP_ZIGBEE_GetStartupConfig( &stConfig );

    /* Using ZbStartupWait (blocking) */
    stZigbeeAppInfo.eJoinStatus = ZbStartupWait( stZigbeeAppInfo.pstZigbee, &stConfig );

    if ( stZigbeeAppInfo.eJoinStatus == ZB_STATUS_SUCCESS )
    {
      stZigbeeAppInfo.lJoinDelay = 0u;
      stZigbeeAppInfo.bInitAfterJoin = true;

      /* USER CODE BEGIN APP_ZIGBEE_NwkFormOrJoin */
      APP_LED_ON( LED_BLUE );

      /* USER CODE END APP_ZIGBEE_NwkFormOrJoin */
      if ( stZigbeeAppInfo.eStartupControl == ZbStartTypeForm )
        { LOG_INFO_APP( "Mesh network created." ); }
      else
        { LOG_INFO_APP( "Association accepted." ); }
    }
    else
    {
      LOG_INFO_APP( "Startup Wait Callback Status : 0x%02X", stZigbeeAppInfo.eJoinStatus );
      LOG_INFO_APP( "Startup failed, attempting again after a short delay (%d ms)", APP_ZIGBEE_STARTUP_FAIL_DELAY );

      if ( stZigbeeAppInfo.eStartupControl == ZbStartTypeJoin )
      {
        /* Reset ZigBee data to be sure that start with good data */
        ZbReset( stZigbeeAppInfo.pstZigbee );
      }

      stZigbeeAppInfo.lJoinDelay = HAL_GetTick() + APP_ZIGBEE_STARTUP_FAIL_DELAY;
    }
  }

  /* If Network forming/joining was not successful reschedule the current task to retry the process */
  if ( stZigbeeAppInfo.eJoinStatus != ZB_STATUS_SUCCESS )
  {
    UTIL_TIMER_Start( &stNwkFormWaitTimer );
  }
  else
  {
    APP_ZIGBEE_ConfigMeshNetwork();
    APP_ZIGBEE_ApplicationStart();
  }
}

/**
 * @brief  Handle Zigbee network forming and joining
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_ConfigMeshNetwork(void)
{
  bool      bReturn;
  uint32_t  lBroadcastTimeOut = 3;

  stZigbeeAppInfo.bInitAfterJoin = false;

  /* Assign ourselves to the group addresses */
  bReturn = APP_ZIGBEE_ConfigGroupAddr();

  /* If we're using group addressing (broadcast), shorten the broadcast timeout */
  if ( bReturn != false )
  {
    ZbNwkSet( stZigbeeAppInfo.pstZigbee, ZB_NWK_NIB_ID_NetworkBroadcastDeliveryTime, &lBroadcastTimeOut, sizeof( lBroadcastTimeOut ) );
  }

  /* If Coord or Router, start the possibility to know where a 'Device' (End Device ou Router) Join the Network */
  if ( stZigbeeAppInfo.eStartupControl == ZbStartTypeForm )
  {
    ZbMsgFilterRegister( stZigbeeAppInfo.pstZigbee, ZB_MSG_FILTER_JOIN_IND, ZB_MSG_DEFAULT_PRIO, APP_ZIGBEE_DeviceJointCallback, NULL );
  }
}

/**
 * @brief  Handle Zigbee network forming and joining
 * @param  eZbStatus :
 * @param  pCallBackArg :
 * @retval None
 */
static void ZbStartupWaitCallback( enum ZbStatusCodeT eZbStatus, void * pCallBackArg )
{
  eZbStartupWaitStatus = eZbStatus;

  UTIL_SEQ_SetEvt( EVENT_ZIGBEE_STARTUP_ENDED );
}

/**
 * @brief ZbStartupWait
 * @param  zb : Zigbee stack handler
 * @param  config : Configuration parameter used to form or join the network
 * @retval None
 */
static enum ZbStatusCodeT ZbStartupWait( struct ZigBeeT * pstZigbee, struct ZbStartupT * pstConfig )
{
  enum ZbStatusCodeT  eZbStatus;

  /* Variable also used by ZbStartupWaitCallback */
  eZbStartupWaitStatus = ZB_STATUS_SUCCESS;

  /* Start & Timer to can advertise user during 'Join' waiting time */
  if ( stZigbeeAppInfo.eStartupControl == ZbStartTypeJoin )
  {
    UTIL_TIMER_Start( &stNwkFormWaitJoinTimer );
  }

  eZbStatus = ZbStartup( pstZigbee, pstConfig, ZbStartupWaitCallback, NULL );
  if ( eZbStatus != ZB_STATUS_SUCCESS )
  {
    return eZbStatus;
  }

  /* Wait ZB Join finished */
  UTIL_SEQ_WaitEvt( EVENT_ZIGBEE_STARTUP_ENDED );

  /* Stop Timer that can advertise user during 'Join' waiting time */
  if ( stZigbeeAppInfo.eStartupControl == ZbStartTypeJoin )
  {
    UTIL_TIMER_Stop( &stNwkFormWaitJoinTimer );
    /* USER CODE BEGIN ZbStartupWait */
    APP_LED_OFF( LED_BLUE );

    /* USER CODE END ZbStartupWait */
  }

  return eZbStartupWaitStatus;
}

/**
 * @brief Callback called every time a new Device (Router ou EndDevice) join the Network.
 *        Information around the Device (Address & Capability) are sent on 'APP_ZIGBEE_NewDevice()' function.
 * @param zb        Zigbee stack handler
 * @param lId       Type of filter Message
 * @param pMessage  Pointer on Message information
 * @param arg       Pointer on User Argument.
 */
static enum zb_msg_filter_rc APP_ZIGBEE_DeviceJointCallback( struct ZigBeeT * zb, uint32_t lId, void * pMessage, void * arg )
{
  struct ZbNlmeJoinIndT * pstJoinMessage;

  if ( lId == ZB_MSG_FILTER_JOIN_IND )
  {
    pstJoinMessage = ( struct ZbNlmeJoinIndT * )pMessage;

    switch ( pstJoinMessage->rejoinNetwork )
    {
      case ZB_NWK_REJOIN_TYPE_ASSOC :
      case ZB_NWK_REJOIN_TYPE_ORPHAN :
      case ZB_NWK_REJOIN_TYPE_NWKREJOIN :
      case ZB_NWK_REJOIN_TYPE_NWKCOMMISS_JOIN :
      case ZB_NWK_REJOIN_TYPE_NWKCOMMISS_REJOIN :
          /* A new Device has Join Network at MAC/NWK level */
          APP_ZIGBEE_SetNewDevice( pstJoinMessage->nwkAddr, pstJoinMessage->extAddr, pstJoinMessage->capabilityInfo );
          break;

      default :
          break;
    }
  }

  return( ZB_MSG_CONTINUE );
}

/*************************************************************
 *
 * GENERIC FUNCTIONS
 *
 *************************************************************/

/**
 * @brief  Callback that indicate if a 'Permit to Join' the network can be done
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_ZbZdoPermitJoinReqCallback( struct ZbZdoPermitJoinRspT * pstJoinResponse, void * arg )
{
  UNUSED( arg );

  if ( pstJoinResponse->status != ZB_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "Error, cannot set Permit Join duration (%d)", pstJoinResponse->status );
  }
  else
  {
    LOG_INFO_APP( "Permit Join duration successfully changed." );
  }
}

/**
 * @brief  Send the request to NWK layer a Permit to Join the network
 * @param  cPermitJoinDelay   Delay in second during the new Permit to Join is available
 * @retval None
 */
void APP_ZIGBEE_PermitJoin( uint8_t cPermitJoinDelay )
{
  struct ZbZdoPermitJoinReqT  stJoinRequest;
  enum ZbStatusCodeT          eStatus;

  memset( &stJoinRequest, 0, sizeof( stJoinRequest ) );
  stJoinRequest.destAddr = ZB_NWK_ADDR_UNDEFINED;
  stJoinRequest.tcSignificance = true;
  stJoinRequest.duration = cPermitJoinDelay;

  LOG_INFO_APP( "Send command Permit Join during %ds", cPermitJoinDelay );
  eStatus = ZbZdoPermitJoinReq( stZigbeeAppInfo.pstZigbee, &stJoinRequest, APP_ZIGBEE_ZbZdoPermitJoinReqCallback, NULL);
  if ( eStatus != ZB_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "Error during command Permit Join (%d)", eStatus );
  }
}

/**
 * @brief  Indicate if Application as already join the network
 * @param  None
 * @retval True if Join OK, else False
 */
bool APP_ZIGBEE_IsAppliJoinNetwork( void )
{
  bool                bJoinOk = false;
  uint64_t            dlEpid = 0U;
  enum ZbStatusCodeT  eZbStatus;

  if ( stZigbeeAppInfo.pstZigbee != NULL )
  {
    /* Check if the router joined the network, and if the waiting time after the Join is finished */
    eZbStatus = ZbNwkGet( stZigbeeAppInfo.pstZigbee, ZB_NWK_NIB_ID_ExtendedPanId, &dlEpid, sizeof( dlEpid ) );
    if ( ( eZbStatus == ZB_STATUS_SUCCESS) && ( dlEpid != 0u ) && ( stZigbeeAppInfo.eJoinStatus == ZB_STATUS_SUCCESS ) )
      { bJoinOk = true; }
  }

  return( bJoinOk );
}

/**
 * @brief   Indicate a Device with Install Code request a Join.
 *          Add the LinkKey (from Install Code) on List and start a Join during 30s.
 *
 * @param   dlExtendedAddress   Device Extended Address
 * @param   szInstallCode       Device Install Code
 * @param   cPermitJoinDelay    Time to Device to Join network. If 0, PermitJoin is not called.
 */
void APP_ZIGBEE_AddDeviceWithInstallCode( uint64_t dlExtendedAddress, uint8_t * szInstallCode, uint8_t cPermitJoinDelay )
{
  uint32_t                  lTcPolicy = 0;
  struct ZbApsmeAddKeyReqT  stAddKeyReq;
  struct ZbApsmeAddKeyConfT stAddKeyConf;
  static  bool              bTrustCenterDone = false;

  if ( bTrustCenterDone == false )
  {
    ZbApsGet( stZigbeeAppInfo.pstZigbee, ZB_APS_IB_ID_TRUST_CENTER_POLICY, &lTcPolicy, sizeof(lTcPolicy));
    lTcPolicy |= (ZB_APSME_POLICY_IC_SUPPORTED | ZB_APSME_POLICY_TCLK_UPDATE_REQUIRED | ZB_APSME_POLICY_TC_POLICY_CHANGE);

    ZbApsSet( stZigbeeAppInfo.pstZigbee, ZB_APS_IB_ID_TRUST_CENTER_POLICY, &lTcPolicy, sizeof(lTcPolicy) );
    bTrustCenterDone = true;
  }

  /* Register 'Application Link Key' for the Device */
  memset( &stAddKeyConf, 0, sizeof( stAddKeyConf ) );
  memset( &stAddKeyReq, 0, sizeof( stAddKeyReq ) );

  stAddKeyReq.keyType = ZB_SEC_KEYTYPE_TC_LINK;
  stAddKeyReq.keySeqNumber = 0;
  stAddKeyReq.partnerAddr = dlExtendedAddress;

  /*Extract Link Key from the Install Code*/
  ZbAesMmoHash( szInstallCode, ( ZB_SEC_KEYSIZE + 2u ), stAddKeyReq.key );

  /* Add the new Link Key */
  ZbApsmeAddKeyReq( stZigbeeAppInfo.pstZigbee, &stAddKeyReq, &stAddKeyConf );
  if ( stAddKeyConf.status != ZB_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "Error Add Link Key (0x%02X)", stAddKeyConf.status );
  }
  else
  {
    LOG_INFO_APP( "Add of Device Link Key OK." );
    if ( cPermitJoinDelay != 0 )
    {
      APP_ZIGBEE_PermitJoin( cPermitJoinDelay );
      LOG_INFO_APP( "Device can now Join Network during %d seconds.", cPermitJoinDelay );
    }
  }
}

/**
 * @brief  Get the current RF channel
 * @param  cCurrentChannel    Current Channel
 * @retval True if Ok, else false.
 */
bool APP_ZIGBEE_GetCurrentChannel( uint8_t * cCurrentChannel )
{
  bool      bResult = false;
  uint8_t   cChannel = 0 ;
  uint32_t  lChannelmask;
  struct ZbNlmeGetInterfaceReqT   stZbNlmeRequest;
  struct ZbNlmeGetInterfaceConfT  stZbNlmeConfig;

  /* Verify if Appli has already Join a Network  */
  if ( APP_ZIGBEE_IsAppliJoinNetwork() != false )
  {
    /* NLME-GET-INTERFACE.request on Interface 0 */
    memset( &stZbNlmeRequest, 0, sizeof( stZbNlmeRequest ) );
    memset( &stZbNlmeConfig, 0, sizeof( stZbNlmeConfig ) );

    stZbNlmeRequest.ifcIndex = 0;

    ZbNlmeGetInterface( stZigbeeAppInfo.pstZigbee, &stZbNlmeRequest, &stZbNlmeConfig);
    if ( stZbNlmeConfig.status == ZB_STATUS_SUCCESS )
    {
      /* Current channel */
      lChannelmask = stZbNlmeConfig.channelInUse;
      while ( ( lChannelmask & 1u ) == 0u )
      {
        /* Iterate through bits until we find the only set bit */
        lChannelmask = lChannelmask >> 1u;
        cChannel++;
      }

      *cCurrentChannel = cChannel;
      bResult = true;
    }
  }

  return( bResult );
}

/**
 * @brief  Set the current RF TX Power
 * @param  cTxPower    Transmit Power to set
 * @retval True if Ok, else false.
 */
bool APP_ZIGBEE_SetTxPower( uint8_t cTxPower )
{
  return ZbNwkIfSetTxPower( stZigbeeAppInfo.pstZigbee, "wpan0", cTxPower );
}

/**
 * @brief Display a Security Key or Install Code
 *
 * @param szCode  Code to display
 * @return        String of the Code
 */
char * APP_ZIGBEE_GetDisplaySecKey( const uint8_t * szCode, uint16_t iLength, bool bSpace )
{
  uint16_t      iIndex;
  static char   szCodeValue[( ( ZB_SEC_KEYSIZE + 2u ) * 3u ) + 1u];

  /* Initialize & verify parameters */
  memset( szCodeValue, 0, sizeof(szCodeValue) );
  if ( iLength > ( ZB_SEC_KEYSIZE + 2u ) )
  {
    iLength = ( ZB_SEC_KEYSIZE + 2u );
  }

  for ( iIndex= 0; iIndex < iLength; iIndex++ )
  {
    if ( bSpace != false )
    {
      snprintf( &szCodeValue[iIndex * 3u], ( sizeof(szCodeValue) - ( iIndex * 3u ) ), "%02X ", szCode[iIndex] );
    }
    else
    {
      snprintf( &szCodeValue[iIndex * 2u], ( sizeof(szCodeValue) - ( iIndex * 2u ) ), "%02X", szCode[iIndex] );
    }
  }

  return (char *)&szCodeValue;
}

/**
 * @brief  Print standard application information (channel in use, etc..) common to all Zigbee Application
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_PrintGenericInfo( void )
{
  /* Display Application Version. */
  LOG_INFO_APP( "Application Version : %d.%d", ( APP_ZIGBEE_APP_VERSION >> 4u ), ( APP_ZIGBEE_APP_VERSION & 0x0F ) );

  /* Display Zigbee Stack Info */
#if ( CONFIG_ZB_ZCL_SE == 1 )
  LOG_INFO_APP( "Zigbee Stack version : R%02d 'Smart Energy'", CONFIG_ZB_REV );
#else // ( CONFIG_ZB_ZCL_SE == 1 )
  LOG_INFO_APP( "Zigbee Stack version : R%02d", CONFIG_ZB_REV );
#endif // ( CONFIG_ZB_ZCL_SE == 1 )

  LOG_INFO_APP( "Zigbee Extended Address : " LOG_DISPLAY64(), LOG_NUMBER64( ZbExtendedAddress( stZigbeeAppInfo.pstZigbee ) ) );

  /* Display Link Key */
  LOG_INFO_APP( "Link Key : %s", APP_ZIGBEE_GetDisplaySecKey( sec_key_ha, ZB_SEC_KEYSIZE, true ) );
}

/**
 * @brief  Send the Trace from Low-Level ZigBee stack
 * @param  pstZigbee      Pointer on ZigBee Structure.
 * @param  lMask          Trace Level Mask.
 * @param  pFunctionName  Function name.
 * @param  pData          Trace message.
 * @param  pArgList       Pointer on Trace arguments list.
 * @retval None
 */
static void APP_ZIGBEE_Printf( struct ZigBeeT * pstZigbee, uint32_t lMask, const char * pFunctionName, const char * pData, va_list pArgList )
{
#if (CFG_LOG_SUPPORTED != 0)
  if ( ( lMask & ZIGBEE_CONFIG_LOG_LEVEL ) != 0u )
  {
    Log_Module_PrintWithArg( LOG_VERBOSE_INFO, LOG_REGION_ZIGBEE, pData, pArgList );
  }
#endif /* (CFG_LOG_SUPPORTED != 0) */
}

/**
 * @brief  Trace the error or the warning reported.
 * @param  ErrId :
 * @param  ErrCode
 * @retval None
 */
void APP_ZIGBEE_Error( uint32_t ErrId, uint32_t ErrCode )
{
  switch ( ErrId )
  {
    default:
        APP_ZIGBEE_TraceError( "ERROR Unknown", 0 );
        break;
  }
}

/**
 * @brief  Warn the user that an error has occurred.
 *
 * @param  pMess  : Message associated to the error.
 * @param  ErrCode: Error code associated to the module (Zigbee or other module if any)
 * @retval None
 */
static void APP_ZIGBEE_TraceError( const char * pMess, uint32_t ErrCode )
{
  LOG_ERROR_APP( "**** Fatal error = %s (Err = 0x%02X) ****", pMess, ErrCode );

  // Intentional INFINITE_LOOP
  // coverity[no_escape]
  while (1U == 1U)
  {
    /* USER CODE BEGIN APP_ZIGBEE_TraceError */
    APP_LED_TOGGLE( LED_BLUE );
    HAL_Delay( 500U );
    APP_LED_TOGGLE( LED_GREEN );
    HAL_Delay( 500U );
    APP_LED_TOGGLE( LED_RED );
    HAL_Delay( 500U );

    /* USER CODE END APP_ZIGBEE_TraceError */
  }
}

