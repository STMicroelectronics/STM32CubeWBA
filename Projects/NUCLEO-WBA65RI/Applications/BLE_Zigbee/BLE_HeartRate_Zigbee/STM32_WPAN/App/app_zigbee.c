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

/* USER CODE END PI */

/* Public variables -----------------------------------------------------------*/
ZigbeeAppInfo_t                                     stZigbeeAppInfo;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_BLINK_LED_DELAY                  1000u             // Time (in ms) between two Led Blinck during the Startup/Join (up to 17 s).
#define APP_ZIGBEE_STARTUP_WAIT_DELAY               ( 60u * 1000u )   // Time (in ms) to wait the finish Join (60 seconds).
#define APP_ZIGBEE_STARTUP_FAIL_DELAY               500u              // Time (in ms) between two tentative to Join a Coord/Router.

/* Defines for Basic Cluster Server */
#define APP_ZIGBEE_MFR_NAME                         "STMicroelectronics"
#define APP_ZIGBEE_CHIP_NAME                        "STM32WBA"
#define APP_ZIGBEE_CHIP_VERSION                     0x20        // Cut 2.0
#define APP_ZIGBEE_BOARD_POWER                      0x00        // No Power

#define APP_ZIGBEE_APP_DATE_CODE                    "20260301"
#define APP_ZIGBEE_APP_BUILD_ID                     "V1.9"
#define APP_ZIGBEE_APP_VERSION                      0x19                    // Application Version v1.9
#define APP_ZIGBEE_STACK_VERSION                    ( ( 26u << 2u ) | 1u )  // Stack Version 2026.1

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private constants ---------------------------------------------------------*/
/* USER CODE BEGIN PC */

/* USER CODE END PC */

/* Private function prototypes -----------------------------------------------*/
static void APP_ZIGBEE_ConfigBasicServer              ( void );
static void APP_ZIGBEE_TraceError                     ( const char * pMess, uint32_t lErrCode );
static void APP_ZIGBEE_ConfigMeshNetwork              ( void );
static void APP_ZIGBEE_ZbCallbackTimeOutTimerCallback ( void * arg );
static void APP_ZIGBEE_ZbRestartJoinTimerCallback     ( void * arg );
static void APP_ZIGBEE_BlinckLedTimerCallback         ( void * arg );
static void APP_ZIGBEE_Printf                         ( struct ZigBeeT * zb, uint32_t lMask, const char * pHeader, const char * pFrame, va_list argptr );

static enum zb_msg_filter_rc APP_ZIGBEE_DeviceJointCallback   ( struct ZigBeeT * zb, uint32_t lId, void * pMessage, void * arg );

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private variabless -----------------------------------------------*/
static UTIL_TIMER_Object_t      stZbCallbackTimeOutTimer, stZbRestartJoinTimer, stBlinckLedTimer;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Functions Definition ------------------------------------------------------*/

/**
 * @brief  Initialize 'Network Form or Join' Task
 *
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_NwkFormOrJoinTaskInit( void )
{
  /* First, create the Timer service to indicate end of a Callback */
  UTIL_TIMER_Create( &stZbCallbackTimeOutTimer, 0, UTIL_TIMER_ONESHOT, &APP_ZIGBEE_ZbCallbackTimeOutTimerCallback, NULL );
  UTIL_TIMER_Create( &stZbRestartJoinTimer, 0, UTIL_TIMER_ONESHOT, &APP_ZIGBEE_ZbRestartJoinTimerCallback, NULL );

  if ( stZigbeeAppInfo.eStartupControl == ZbStartTypeJoin )
  {
    /* Create the Timer service to can advertise user during the time of the Join (by default 17 seconds) */
    UTIL_TIMER_Create( &stBlinckLedTimer, APP_ZIGBEE_BLINK_LED_DELAY, UTIL_TIMER_PERIODIC, &APP_ZIGBEE_BlinckLedTimerCallback, NULL );

    /* Launch Blue Led Blink during Join */
    UTIL_TIMER_Start( &stBlinckLedTimer );
  }

  /* Create the Task associated with network creation process */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_ZIGBEE_NETWORK_FORM, UTIL_SEQ_RFU, APP_ZIGBEE_NwkFormOrJoin );

  /* launch the startup of the mesh network setup */
  UTIL_SEQ_SetTask( 1U << CFG_TASK_ZIGBEE_NETWORK_FORM, TASK_PRIO_ZIGBEE_NETWORK_FORM );
}

/**
 * @brief  Configure Zigbee Basic Server Cluster
 *
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_ConfigBasicServer( void )
{
  static struct ZbZclBasicServerDefaults   stBasicServerDefaults;

  /* Initialize Basic Server Cluster 'defaults' information */
  memset( &stBasicServerDefaults, 0x00, sizeof(stBasicServerDefaults) );

  stBasicServerDefaults.mfr_name[0] = strlen( APP_ZIGBEE_MFR_NAME );
  memcpy( &stBasicServerDefaults.mfr_name[1], APP_ZIGBEE_MFR_NAME, strlen( APP_ZIGBEE_MFR_NAME ) );

  stBasicServerDefaults.model_name[0] = strlen( APP_ZIGBEE_CHIP_NAME );
  memcpy( &stBasicServerDefaults.model_name[1], APP_ZIGBEE_CHIP_NAME, strlen( APP_ZIGBEE_CHIP_NAME ) );

  stBasicServerDefaults.date_code[0] = strlen( APP_ZIGBEE_APP_DATE_CODE );
  memcpy( &stBasicServerDefaults.date_code[1], APP_ZIGBEE_APP_DATE_CODE, strlen( APP_ZIGBEE_APP_DATE_CODE ) );

  stBasicServerDefaults.sw_build_id[0] = strlen( APP_ZIGBEE_APP_BUILD_ID );
  memcpy( &stBasicServerDefaults.sw_build_id[1], APP_ZIGBEE_APP_BUILD_ID, strlen( APP_ZIGBEE_APP_BUILD_ID ) );

  /* Version are on 8 bits : 4 MSB for Major and 4 LSB for Minor, except for stack : 6 MSB for Major and 2 LSB for Minor */
  stBasicServerDefaults.app_version = (uint8_t) APP_ZIGBEE_APP_VERSION;
  stBasicServerDefaults.hw_version = (uint8_t) APP_ZIGBEE_CHIP_VERSION;
  stBasicServerDefaults.stack_version = (uint8_t) APP_ZIGBEE_STACK_VERSION;

  stBasicServerDefaults.power_source = APP_ZIGBEE_BOARD_POWER;

  /* Configure default Basic Server */
  ZbZclBasicServerConfigDefaults( stZigbeeAppInfo.pstZigbee , &stBasicServerDefaults );
}

/**
 * @brief  Initialize Zigbee stack layers
 *
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_StackLayersInit( void )
{
  LOG_INFO_APP( "StackLayers Init (startupMode = %d)", stZigbeeAppInfo.eStartupControl );

  /* Initialise Zigbee */
  stZigbeeAppInfo.pstZigbee = ZbInit( stZigbeeAppInfo.dlExtendedAddress, NULL, NULL );
  assert(stZigbeeAppInfo.pstZigbee != NULL);

  /* Get the Extended Address if based on HW MAC Address */
  if (  stZigbeeAppInfo.dlExtendedAddress == 0u )
  {
    stZigbeeAppInfo.dlExtendedAddress = ZbExtendedAddress( stZigbeeAppInfo.pstZigbee );
  }

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

  /* USER CODE END APP_ZIGBEE_StackLayersInit1 */

  /* Configure the joining parameters */
  stZigbeeAppInfo.eJoinStatus = ZB_ZDP_STATUS_TIMEOUT;  /* Init to error status */
  stZigbeeAppInfo.lJoinDelay = HAL_GetTick();           /* Now */

  /* Initialization Complete */
  stZigbeeAppInfo.bHasInit = true;

  /* Print Application information */
  APP_ZIGBEE_PrintApplicationInfo();

  /* Create the NwkFormOrJoin Task */
  if ( stZigbeeAppInfo.bNwkStartup != false )
  {
    APP_ZIGBEE_NwkFormOrJoinTaskInit();
  }
}

/**
 * @brief  Callback triggered when the Callback Timer expire
 *
 * @param  arg : Not used
 * @retval None
 */
static void APP_ZIGBEE_ZbCallbackTimeOutTimerCallback( void * arg )
{
  UNUSED( arg );

  stZigbeeAppInfo.eJoinStatus = ZB_ZDP_STATUS_TIMEOUT;

  UTIL_SEQ_SetEvt( EVENT_ZIGBEE_CALLBACK_DONE );
}

/**
 * @brief  Callback triggered when the ZbStartup answer
 *
 * @param  eZbStatus :
 * @param  pCallBackArg :
 * @retval None
 */
static void APP_ZIGBEE_ZbStartupCallback( enum ZbStatusCodeT eZbStatus, void * pCallBackArg )
{
  UTIL_TIMER_Stop( &stZbCallbackTimeOutTimer );

  stZigbeeAppInfo.eJoinStatus = eZbStatus;

  UTIL_SEQ_SetEvt( EVENT_ZIGBEE_CALLBACK_DONE );
}

/**
 * @brief  Callback triggered when the Restart Join Timer expire
 *
 * @param  arg : Not used
 * @retval None
 */
void APP_ZIGBEE_ZbRestartJoinTimerCallback( void * arg )
{
  UTIL_SEQ_SetEvt( EVENT_ZIGBEE_RESTART_WAIT );
}

/**
 * @brief  Wait a Callback
 *
 * @param  lTimeOut    Maximum time to wait Callback.
 * @retval None
 */
static void APP_ZIGBEE_WaitForCallback( uint32_t lTimeOut )
{
  UTIL_SEQ_ClrEvt( EVENT_ZIGBEE_CALLBACK_DONE );

  UTIL_TIMER_StartWithPeriod( &stZbCallbackTimeOutTimer, lTimeOut );

  /* Wait Callback finished */
  UTIL_SEQ_WaitEvt( EVENT_ZIGBEE_CALLBACK_DONE );
}

/**
 * @brief  Callback triggered when the Blinck Led Timer expire
 *
 * @param  arg : Not used
 * @retval None
 */
static void APP_ZIGBEE_BlinckLedTimerCallback( void * arg )
{
  /* USER CODE BEGIN APP_ZIGBEE_BlinckLedTimerCallback */

  /* USER CODE END APP_ZIGBEE_BlinckLedTimerCallback */
}

/**
 * @brief  Handle Zigbee network forming and joining
 *
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_NwkFormOrJoin(void)
{
  enum ZbStatusCodeT  eStatus = ZB_ZDP_STATUS_TIMEOUT;
  struct ZbStartupT   stConfig;

  while ( eStatus != ZB_STATUS_SUCCESS )
  {
    /* Application configure Startup */
    APP_ZIGBEE_GetStartupConfig( &stConfig );

    if ( eStatus != ZB_STATUS_SUCCESS )
    {
      /* Start Zigbee Stack & Form/Join network */
      stZigbeeAppInfo.eJoinStatus = ZB_ZDP_STATUS_TIMEOUT;
      eStatus = ZbStartup( stZigbeeAppInfo.pstZigbee, &stConfig, APP_ZIGBEE_ZbStartupCallback, NULL );
      if ( eStatus == ZB_STATUS_SUCCESS )
      {
        APP_ZIGBEE_WaitForCallback( APP_ZIGBEE_STARTUP_WAIT_DELAY );
        eStatus = stZigbeeAppInfo.eJoinStatus;
      }
    }

    if ( eStatus == ZB_STATUS_SUCCESS )
    {
      stZigbeeAppInfo.lJoinDelay = 0u;
      stZigbeeAppInfo.bInitAfterJoin = true;

      if ( stZigbeeAppInfo.eStartupControl == ZbStartTypeForm )
        { LOG_INFO_APP( "Mesh network created." ); }
      else
      {
        LOG_INFO_APP( "Association accepted." );

        /* Stop Blue Led Blink during Join */
        UTIL_TIMER_Stop( &stBlinckLedTimer );
      }

      /* USER CODE BEGIN APP_ZIGBEE_NwkFormOrJoin */
      APP_LED_ON( LED_BLUE );

      /* USER CODE END APP_ZIGBEE_NwkFormOrJoin */

      /* Start Applications */
      APP_ZIGBEE_ConfigMeshNetwork();
      APP_ZIGBEE_ApplicationStart();
    }
    else
    {
      LOG_INFO_APP( "Startup Wait Callback Status : 0x%02X", stZigbeeAppInfo.eJoinStatus );
      LOG_INFO_APP( "Startup failed, attempting again after a short delay (%d ms)", APP_ZIGBEE_STARTUP_FAIL_DELAY );

      /* If Device, reset ZigBee data to be sure that start with good data */
      if ( stZigbeeAppInfo.eStartupControl == ZbStartTypeJoin )
      {
        ZbReset( stZigbeeAppInfo.pstZigbee );
      }

      UTIL_SEQ_ClrEvt( EVENT_ZIGBEE_RESTART_WAIT );
      UTIL_TIMER_StartWithPeriod( &stZbRestartJoinTimer, APP_ZIGBEE_STARTUP_FAIL_DELAY );

      /* Wait Restart Wait finished */
      UTIL_SEQ_WaitEvt( EVENT_ZIGBEE_RESTART_WAIT );
    }
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

/**
 * @brief  Treat Serial commands and test if it's a Request on Install Code.
 *
 * @param  pRxBuffer      Pointer on received data from USART.
 * @param  iRxBufferSize  Number of received data.
 * @retval None
 */
void APP_ZIGBEE_SerialCommandInstallCode( uint8_t * pRxBuffer, uint16_t iRxBufferSize )
{
  uint8_t   szInstallCode[ZB_SEC_KEYSIZE + 2u], szByte[9];
  uint16_t  iBufferIndex, iIndex, iSize, iNormalSize;
  uint64_t  dlExtendedAddress = 0;

  /* Threat USART Command for Install Code */
  if ( strncmp( (char *)pRxBuffer, "IC:", 3 ) == 0 )
  {
    /* Treat Command IC:<ExtendedAddress>:<InstallCode + CRC><CR> */
    iSize = strlen( (char *)pRxBuffer );
    iNormalSize = 3u + ( sizeof(dlExtendedAddress) * 2u ) + 1u + ( sizeof(szInstallCode) * 2u );
    if ( iSize == iNormalSize )
    {
      iBufferIndex = 3u;
      memcpy( szByte, &pRxBuffer[iBufferIndex], 8u );
      szByte[8] = 0;
      dlExtendedAddress = (uint64_t)( strtoul( (char *)szByte, NULL, 16u ) ) << 32u;

      iBufferIndex += 8u;
      memcpy( szByte, &pRxBuffer[iBufferIndex], 8u );
      dlExtendedAddress += (uint64_t)( strtoul( (char *)szByte, NULL, 16u ) );

      iBufferIndex += ( 8u + 1u );
      szByte[2] = 0;
      for ( iIndex = 0; iIndex < sizeof(szInstallCode); iIndex++ )
      {
        szByte[0] = pRxBuffer[iBufferIndex++];
        szByte[1] = pRxBuffer[iBufferIndex++];
        szInstallCode[iIndex] = (uint8_t)strtoul( (char *)szByte, NULL, 16 );
      }

      LOG_INFO_APP( "Command Install Code with ExtAddress " LOG_DISPLAY64() " and InstallCode %s.",
                   LOG_NUMBER64(dlExtendedAddress), &pRxBuffer[3u + ( sizeof(dlExtendedAddress) * 2u ) + 1u] );
      if ( AppZbUtil_AddDeviceWithInstallCode( stZigbeeAppInfo.pstZigbee, dlExtendedAddress, szInstallCode ) != false )
      {
        APP_ZIGBEE_PermitJoin( 30 );
      }
    }
    else
    {
      LOG_ERROR_APP( "Bad Length : %d instead %d.", iSize, iNormalSize );
    }
  }
}
