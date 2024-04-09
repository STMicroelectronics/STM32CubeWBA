
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
#include "stm32wbaxx_nucleo.h"


/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_CHANNEL                13u
#define APP_ZIGBEE_CHANNEL_MASK           ( 1u << APP_ZIGBEE_CHANNEL )
#define APP_ZIGBEE_TX_POWER               ((int8_t) 10)    /* TX-Power is at +10 dBm. */

#define APP_ZIGBEE_ENDPOINT               1u
#define APP_ZIGBEE_PROFILE_ID             0xF000u       // Manufacturer Specific Profiles range [0xc000, 0xffff]
#define APP_ZIGBEE_CLUSTER_REQ_ID         0x0000u
#define APP_ZIGBEE_CLUSTER_RSP_ID         0x0001u

#define APP_ZIGBEE_APPLICATION_NAME       "APS Coordinator"

#define APP_ZIGBEE_APPLICATION_OS_NAME    "."

/* Private typedef -----------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint16_t   iDeviceShortAddress;

/* Private function prototypes -----------------------------------------------*/
static enum zb_msg_filter_rc APP_ZIGBEE_ApsIndicationCallback( struct ZbApsdeDataIndT * pstDataInd, void * arg );

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
  uint16_t  iShortAddress;
  
  /* Display Short Address */
  iShortAddress = ZbShortAddress( stZigbeeAppInfo.pstZigbee );
  LOG_INFO_APP( "Use Short Address : 0x%04X", iShortAddress );
  
  LOG_INFO_APP( "%s ready to work !", APP_ZIGBEE_APPLICATION_NAME );
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

  /* Add EndPoint */
  memset( &stRequest, 0, sizeof( stRequest ) );
  memset( &stConfig, 0, sizeof( stConfig ) );

  stRequest.profileId = APP_ZIGBEE_PROFILE_ID;
  stRequest.endpoint = APP_ZIGBEE_ENDPOINT;
  ZbZclAddEndpoint( stZigbeeAppInfo.pstZigbee, &stRequest, &stConfig );
  assert( stConfig.status == ZB_STATUS_SUCCESS );

  /* Create filter to receive APS message */
  stZigbeeAppInfo.stApsInfo.pstApsFilter = ZbApsFilterEndpointAdd( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT, APP_ZIGBEE_PROFILE_ID,
                                                                   APP_ZIGBEE_ApsIndicationCallback, NULL );
  assert( stZigbeeAppInfo.stApsInfo.pstApsFilter != NULL );
}


/**
 * @brief  Set Group Addressing mode (if used)
 * @param  None
 * @retval 'true' if Group Address used else 'false'.
 */
bool APP_ZIGBEE_ConfigGroupAddr( void )
{
  /* Not used */

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
}


/**
 * @brief  Print application information to the console
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_PrintApplicationInfo(void)
{
  LOG_INFO_APP( "**********************************************************" );
  LOG_INFO_APP( "Network config : COORDINATOR" );
  
  LOG_INFO_APP( "Application Flashed : Zigbee %s %s", APP_ZIGBEE_APPLICATION_NAME, APP_ZIGBEE_APPLICATION_OS_NAME );
  LOG_INFO_APP( "Channel used: %d.", APP_ZIGBEE_CHANNEL );
   
  APP_ZIGBEE_PrintGenericInfo();

  LOG_INFO_APP( "Allocations are: ");
  LOG_INFO_APP( "APS on Endpoint %d", APP_ZIGBEE_ENDPOINT );
  LOG_INFO_APP("**********************************************************");
}


/**
 * @brief Parse & display APSDE Data Frame, with a payload.
   @note  If payload is more than 25 bytes, display only the first 25 bytes.
 * @param  pstDataInd   APSDE Data Indication frame
 * @retval None
 */
static void APP_ZIGBEE_ApsParseFrame( struct ZbApsdeDataIndT * pstDataInd )
{
  uint16_t  iIndex, iMaxIndex;
  uint8_t   szPayload[(25u * 4u ) + 1u] = { 0 };
  
  LOG_INFO_APP( "  Source Network Address: 0x%04X", pstDataInd->src.nwkAddr );
  LOG_INFO_APP( "  Profile ID: 0x%02X", pstDataInd->profileId );
  LOG_INFO_APP( "  Cluster ID: 0x%02X", pstDataInd->clusterId );
  
  /* Payload */
  iMaxIndex = pstDataInd->asduLength;
  if ( iMaxIndex > 25u )
  {
    iMaxIndex = 25 ;
  }
  
  for ( iIndex = 0; iIndex < iMaxIndex; iIndex++ )
  {
    snprintf( (char *)&szPayload[iIndex * 4u], sizeof( szPayload ), "0x%02X", pstDataInd->asdu[iIndex] );
  }
  LOG_INFO_APP( "  Payload: %s", szPayload );
  
  LOG_INFO_APP( "  Link quality: %d", pstDataInd->linkQuality );
  LOG_INFO_APP( "  RSSI: %d", pstDataInd->rssi );
}


/**
 * @brief  ZigBee application APSDE Data Confirmation callback
 * @param  pstDataConf  APSDE Data Confirmation
 * @param  arg          Passed argument
 * @retval None
 */
static void APP_ZIGBEE_ApsConfirmationCallback( struct ZbApsdeDataConfT * pstDataConf, void * arg)
{
  if ( pstDataConf->status != ZB_STATUS_SUCCESS ) 
  {
    LOG_ERROR_APP("[APS] Error during APS confirm status -(0x%02X)", pstDataConf->status );
  }
}

                                                                   
/**
 * @brief  Send APS response
 * @param  pstDestAddr  Destination address info
 * @retval None
 */
static void APP_ZIGBEE_ApsSendResponse(struct ZbApsAddrT * pstDestAddr)
{
  struct ZbApsdeDataReqT  stApsRequest;
  const uint8_t           szPayload[] = { 0xab, 0xcd, 0xef }; /* ! This demo app sends a canned payload */
  enum ZbStatusCodeT      eStatus;

  memset( &stApsRequest, 0, sizeof(stApsRequest) );
    
  stApsRequest.dst = *pstDestAddr;
  stApsRequest.profileId = APP_ZIGBEE_PROFILE_ID;
  stApsRequest.clusterId = APP_ZIGBEE_CLUSTER_RSP_ID;
  stApsRequest.srcEndpt = APP_ZIGBEE_ENDPOINT;
  stApsRequest.asdu = szPayload;
  stApsRequest.asduLength = sizeof( szPayload );
  stApsRequest.asduHandle = stZigbeeAppInfo.stApsInfo.lAsduHandle++;
  /* Enable packet security */
  stApsRequest.txOptions |= ZB_APSDE_DATAREQ_TXOPTIONS_SECURITY;
  /* Enable APS ACKing */
  stApsRequest.txOptions |= ZB_APSDE_DATAREQ_TXOPTIONS_ACK;
  /* Enable ZB_APSDE_DATAREQ_TXOPTIONS_NWKKEY if you just want Network security.
   * Otherwise APS and NWK security are used. */
  /* aps_req.txOptions |= ZB_APSDE_DATAREQ_TXOPTIONS_NWKKEY; */

  LOG_INFO_APP( "[APS] Sending Response" );
  eStatus = ZbApsdeDataReqCallback( stZigbeeAppInfo.pstZigbee, &stApsRequest, APP_ZIGBEE_ApsConfirmationCallback, NULL) ;
  if (eStatus != ZB_STATUS_SUCCESS) 
  {
    LOG_ERROR_APP("[APS] Error during APS send for response (0x%02X)", eStatus );
  }
}
                                                                   
                                                                   
/**
 * @brief  ZigBee application APSDE Data Indication callback
 * @param  pstDataInd   APSDE Data Indication frame
 * @param  arg          Passed argument
 * @retval enum zb_msg_filter_rc
 */
static enum zb_msg_filter_rc APP_ZIGBEE_ApsIndicationCallback( struct ZbApsdeDataIndT * pstDataInd, void * arg )
{
  switch ( pstDataInd->clusterId ) 
  {
    case APP_ZIGBEE_CLUSTER_REQ_ID:
        LOG_INFO_APP( "[APS] Received request:" );
        APP_ZIGBEE_ApsParseFrame( pstDataInd );
        APP_ZIGBEE_ApsSendResponse( &pstDataInd->src );
        break;
        
    case APP_ZIGBEE_CLUSTER_RSP_ID:
        LOG_INFO_APP( "[APS] Received response:" );
        APP_ZIGBEE_ApsParseFrame( pstDataInd );
        break;

     default:
        break;
  }
  
  return ZB_MSG_CONTINUE;
}                                                         

                                                                   