/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_zigbee_utility.c
  * @author  MCD Application Team
  * @brief   API to manage Zigbee Stack.
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
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include "app_common.h"
#include "app_conf.h"
#include "app_zigbee_utility.h"
#include "log_module.h"

#include "stm32_rtos.h"
#include "stm32_timer.h"

#include "zcl/zcl.h"

/* Private Constants ------------------------------------------------------------------*/
#define   APP_ZB_UTIL_WAIT_CALLBACK_TIMEOUT_DEFAULT_MS    30000u
#define   EVENT_APP_ZB_UTIL_WAIT_CALLBACK_DONE            EVENT_ZIGBEE_APP4

/* Private structures -----------------------------------------------------------------*/
typedef struct
{
  struct ZbApsAddrT   * pstMatchedList;
  uint16_t            iListSize;
  uint16_t            iListIndex;
} MatchDescription_t;

/* Private variables ------------------------------------------------------------------*/
static UTIL_TIMER_Object_t      stWaitCallbackTimeOutTimer;
static enum ZbStatusCodeT       eCallbackStatus;

/* Private Functions ------------------------------------------------------------------*/
static void AppZbUtil_WaitReceiveCallbackSemaphoreSet( void )
{
  UTIL_SEQ_SetEvt( EVENT_APP_ZB_UTIL_WAIT_CALLBACK_DONE );
}

static void AppZbUtil_WaitReceiveCallbackTimeOutCallback( void * arg )
{
  AppZbUtil_WaitReceiveCallbackSemaphoreSet();
}

static void AppZbUtil_WaitReceiveCallbackInit( void)
{
  static bool bFirst = false;

  if ( bFirst == false )
  {
    UTIL_TIMER_Create( &stWaitCallbackTimeOutTimer, 0, UTIL_TIMER_ONESHOT, &AppZbUtil_WaitReceiveCallbackTimeOutCallback, NULL );
    bFirst = true;
  }

  UTIL_SEQ_ClrEvt( EVENT_APP_ZB_UTIL_WAIT_CALLBACK_DONE );
  eCallbackStatus = ZB_ZDP_STATUS_TIMEOUT;
}

static void AppZbUtil_WaitReceiveCallbackStart( uint32_t lTimeMs )
{
  UTIL_TIMER_StartWithPeriod( &stWaitCallbackTimeOutTimer, lTimeMs );
  UTIL_SEQ_WaitEvt( EVENT_APP_ZB_UTIL_WAIT_CALLBACK_DONE );
  UTIL_TIMER_Stop( &stWaitCallbackTimeOutTimer );
}

/* Functions --------------------------------------------------------------------------*/
/**
 * @brief ZDO Network Address Callback
 *
 * @param pstResponse
 * @param arg
 */
static void AppZbUtil_RequestNwkAddressCallback(struct ZbZdoNwkAddrRspT * pstResponse, void * arg)
{
  uint16_t  iIndex;
  uint16_t  * pNwkAddress = arg;

  eCallbackStatus = pstResponse->status;
  if ( pstResponse->status != ZB_STATUS_SUCCESS )
  {
    LOG_INFO_APP( "Error during Network Address Callback. Status : 0x%02X\n\n", pstResponse->status );
    AppZbUtil_WaitReceiveCallbackSemaphoreSet();
    return;
  }

  if ( pNwkAddress != NULL )
  {
    *pNwkAddress = pstResponse->nwkAddr;
  }

  LOG_INFO_APP( "Network Address Response\n");
  LOG_INFO_APP( "   ExtAddr: " LOG_DISPLAY64() "\n", LOG_NUMBER64( pstResponse->extAddr ) );
  LOG_INFO_APP( "   NwkAddr: 0x%04x\n", pstResponse->nwkAddr );
  for ( iIndex = 0; iIndex < pstResponse->deviceCount; iIndex++ )
  {
    LOG_INFO_APP( "   Device List[%d]: 0x%04x\n", iIndex, pstResponse->deviceList[iIndex] );
  }
  LOG_INFO_APP( "\n" );

  AppZbUtil_WaitReceiveCallbackSemaphoreSet();
}

/**
 * @brief ZDO Network Address Request
 *
 * @param   pstZigbee         Pointer to Zigbee Stack.
 * @param   dlExtendedAddress Device Extended Address.
 * @param   piNwkAddressOut   Pointer to Network Address to set.
 * @return  True if OK else false.
 */
bool AppZbUtil_RequestNwkAddress( struct ZigBeeT * pstZigbee, uint64_t dlExtendedAddress, uint16_t * piNwkAddressOut )
{
  struct ZbZdoNwkAddrReqT   stRequest;
  enum ZbStatusCodeT        eStatus;

  LOG_INFO_APP( "\nPerforming ZDO Network Address Request...\n" );

  memset( &stRequest, 0, sizeof( stRequest ) );

  stRequest.dstNwkAddr = ZB_NWK_ADDR_BCAST_ROUTERS;
  stRequest.extAddr = dlExtendedAddress;
  stRequest.reqType = ZB_ZDO_ADDR_REQ_TYPE_SINGLE;

  AppZbUtil_WaitReceiveCallbackInit();
  eStatus = ZbZdoNwkAddrReq( pstZigbee, &stRequest, AppZbUtil_RequestNwkAddressCallback, piNwkAddressOut );
  if ( eStatus != ZB_STATUS_SUCCESS )
  {
    LOG_INFO_APP( "Error : Nwk Address Request Fail (0x%02x)\n", eStatus);
    return false;
  }

  AppZbUtil_WaitReceiveCallbackStart( APP_ZB_UTIL_WAIT_CALLBACK_TIMEOUT_DEFAULT_MS );
  if ( eCallbackStatus != ZB_STATUS_SUCCESS )
  {
    return false;
  }

  return true;
}

/**
 * @brief ZDO IEEE/Extended Address Callback
 *
 * @param pstResponse
 * @param arg
 */
static void AppZbUtil_RequestExtendedAddressCallback( struct ZbZdoIeeeAddrRspT * pstResponse, void * arg )
{
  uint16_t  iIndex;
  uint64_t  * pIeeeAddress = arg;

  eCallbackStatus = pstResponse->status;
  if ( pstResponse->status != ZB_STATUS_SUCCESS )
  {
    LOG_INFO_APP( "Error during IEEE/Extended Address Callback. Status : 0x%02X\n\n", pstResponse->status );
    AppZbUtil_WaitReceiveCallbackSemaphoreSet();
  }
  else
  {
    if ( pIeeeAddress != NULL )
    {
      *pIeeeAddress = pstResponse->extAddr;
    }

    LOG_INFO_APP( "IEEE/Extended Address Response :\n" );
    LOG_INFO_APP( "   ExtAddr: " LOG_DISPLAY64() "\n", LOG_NUMBER64( pstResponse->extAddr ) );
    LOG_INFO_APP( "   NwkAddr: 0x%04x\n", pstResponse->nwkAddr );
    for ( iIndex = 0; iIndex < pstResponse->deviceCount; iIndex++ )
    {
      LOG_INFO_APP( "   Device List[%d]: 0x%04x\n", iIndex, pstResponse->deviceList[iIndex] );
    }
    LOG_INFO_APP( "\n" );

    AppZbUtil_WaitReceiveCallbackSemaphoreSet();
  }
}

/**
 * @brief  ZDO IEEE Extended Address Request
 *
 * @param   pstZigbee         Pointer to Zigbee Stack.
 * @param   pIeeeAddressOut   Pointer to Extended Address to set.
 * @return  true if OK else false.
 */
bool AppZbUtil_RequestExtendedAddress( struct ZigBeeT * pstZigbee, uint16_t iNwkAddress, uint64_t * pdlExtendedAddressOut )
{
  struct ZbZdoIeeeAddrReqT  stRequest;
  enum ZbStatusCodeT        eStatus;
  bool                      bReturn = true;

  LOG_INFO_APP( "\nPerforming ZDO IEEE_Addr_Request...\n" );

  memset( &stRequest, 0, sizeof( stRequest ) );

  stRequest.dstNwkAddr = ZB_NWK_ADDR_BCAST_ROUTERS;
  stRequest.nwkAddrOfInterest = iNwkAddress;
  stRequest.reqType = ZB_ZDO_ADDR_REQ_TYPE_SINGLE;

  AppZbUtil_WaitReceiveCallbackInit();
  eStatus = ZbZdoIeeeAddrReq( pstZigbee, &stRequest, AppZbUtil_RequestExtendedAddressCallback, pdlExtendedAddressOut );
  if ( eStatus != ZB_STATUS_SUCCESS )
  {
    LOG_INFO_APP( "Error : Ieee Address Request Fail (0x%02x)\n", eStatus);
    bReturn = false;
  }
  else
  {
    AppZbUtil_WaitReceiveCallbackStart( APP_ZB_UTIL_WAIT_CALLBACK_TIMEOUT_DEFAULT_MS );
    if ( eCallbackStatus != ZB_STATUS_SUCCESS )
    {
      bReturn = false;
    }
  }

  return bReturn;
}

/**
 *
 * @param pstResponse
 * @param arg
 */
static void AppZbUtil_RequestMatchDescriptionCallback( struct ZbZdoMatchDescRspT * pstResponse, void * arg )
{
  MatchDescription_t  *pstMatchedEndpoint = arg;
  uint16_t  iIndex;

  eCallbackStatus = pstResponse->status;
  if ( pstResponse->status != ZB_STATUS_SUCCESS )
  {
    if ( pstResponse->status != ZB_ZDP_STATUS_TIMEOUT )
    {
      LOG_ERROR_APP( "  Error during Match-Multi Description Callback. Status : 0x%02X\n\n", pstResponse->status );
    }
    else
    {
      LOG_INFO_APP( "End of Match-Multi Description Callbacks." );
      pstMatchedEndpoint->iListSize = pstMatchedEndpoint->iListIndex;
      eCallbackStatus = ZB_STATUS_SUCCESS;
    }

    AppZbUtil_WaitReceiveCallbackSemaphoreSet();
  }
  else
  {
    LOG_INFO_APP( "  Nwk Address  : 0x%04X\n", pstResponse->nwkAddr );
    LOG_INFO_APP( "    Num Entries : %d\n", pstResponse->matchLength );
    if ( pstResponse->matchLength != 0u )
    {
      if ( pstMatchedEndpoint != NULL )
      {
        /* For the moment only the first */
        if ( ( pstMatchedEndpoint->iListIndex < pstMatchedEndpoint->iListSize ) )
        {
          pstMatchedEndpoint->pstMatchedList[pstMatchedEndpoint->iListIndex].mode = ZB_APSDE_ADDRMODE_SHORT;
          pstMatchedEndpoint->pstMatchedList[pstMatchedEndpoint->iListIndex].nwkAddr = pstResponse->nwkAddr;
          pstMatchedEndpoint->pstMatchedList[pstMatchedEndpoint->iListIndex].endpoint = pstResponse->matchList[0];
          pstMatchedEndpoint->iListIndex++;
        }
      }

      /* Just for Display */
      for ( iIndex = 0; iIndex < pstResponse->matchLength; iIndex++ )
      {
        LOG_INFO_APP( "    Endpoint List[%d] : 0x%02x\n", iIndex, pstResponse->matchList[iIndex] );
      }
    }
    LOG_INFO_APP( "\n" );
  }
}

/**
 * @brief Request the Match List with a Cluster on another Devices. Blocking until Callbacks occurs.
 *
 * @param pstCluster            Selected Cluster
 * @param pstMatchedList[Out]   Pointer on Matched List
 * @param piListSize            List Size : In input to indicate MatchedList size, in output to indicate number of found match.
 *
 * @return True if request done else false
 */
bool AppZbUtil_RequestMatchDescription( struct ZbZclClusterT * pstCluster, struct ZbApsAddrT * pstMatchedList, uint16_t * piListSize )
{
  struct ZbZdoMatchDescReqT stRequest;
  MatchDescription_t        stMatchDescription;
  enum ZbStatusCodeT        eStatus;
  enum ZbZclClusterIdT      eClusterId;
  bool                      bReturn = true;

  /* Retrieve Cluster ID */
  eClusterId = ZbZclClusterGetClusterId( pstCluster );

  /* Match Descriptor for the Cluster on the Device */
  LOG_INFO_APP( "Send a Match Request for Cluster 0x%03X on others Devices ...\n", eClusterId );

  memset( &stRequest, 0, sizeof( stRequest ) );
  stRequest.nwkAddrOfInterest = 0xFFFF;
  stRequest.dstNwkAddr = 0xFFFF;
  stRequest.profileId = ZbZclClusterGetProfileId( pstCluster );
  if ( ZbZclClusterGetDirection( pstCluster ) == ZCL_DIRECTION_TO_SERVER )
  {
    /* This Cluster is a Server so we search Clients */
    stRequest.numOutClusters = 1;
    stRequest.outClusterList[0] = eClusterId;
  }
  else
  {
    /* This Cluster is a Client so we search Server */
    stRequest.numInClusters = 1;
    stRequest.inClusterList[0] = eClusterId;
  }

  /* Preset Match List */
  stMatchDescription.pstMatchedList = pstMatchedList;
  stMatchDescription.iListSize = *piListSize;
  stMatchDescription.iListIndex = 0;

  AppZbUtil_WaitReceiveCallbackInit();
  eStatus = ZbZdoMatchDescMulti( pstCluster->zb, &stRequest, AppZbUtil_RequestMatchDescriptionCallback, &stMatchDescription );
  if ( eStatus != ZB_STATUS_SUCCESS )
  {
    LOG_INFO_APP( "ZbZdoMatchDescReq Return (status = 0x%02x)\n", eStatus );
    bReturn = false;
  }
  else
  {
    AppZbUtil_WaitReceiveCallbackStart( APP_ZB_UTIL_WAIT_CALLBACK_TIMEOUT_DEFAULT_MS );
    if ( eCallbackStatus != ZB_STATUS_SUCCESS )
    {
      bReturn  = false;
    }
  }

  /* Update List Size */
  *piListSize = stMatchDescription.iListSize;

  return bReturn;
}

/**
 *
 * @param pstResponse
 * @param arg
 */
static void AppZbUtil_RequestBindWithDeviceCallback( struct ZbZdoBindRspT * pstResponse, void * arg )
{
  LOG_INFO_APP( "Bind Response (status = 0x%02x)\n\n", pstResponse->status );

  eCallbackStatus = pstResponse->status;
  AppZbUtil_WaitReceiveCallbackSemaphoreSet();
}

/**
 * @brief Send a Bind Request on another Device. Blocking until Callback occurs.
 *
 * @param pstCluster                Selected Cluster
 * @param dlMyExtendedAddress       This Device Extended Address
 * @param dlSourceExtendedAddress   Selected Device Extended Address
 * @param cSourceEnpoint            Selected Device Endpoint
 *
 * @return True of request done else false.
 */
bool AppZbUtil_RequestBindWithDevice( struct ZbZclClusterT * pstCluster, uint64_t dlMyExtendedAddress, uint64_t dlDeviceExtendedAddress, uint16_t iDeviceAddress, uint8_t cDeviceEnpoint )
{
  struct ZbZdoBindReqT    stRequest;
  enum ZbStatusCodeT      eStatus;
  enum ZbZclClusterIdT    eClusterId;
  bool                    bReturn = true;

  /* Retrieve Cluster ID */
  eClusterId = ZbZclClusterGetClusterId( pstCluster );

  LOG_INFO_APP( "Send a Bind Request for Cluster 0x%03X on EndPoint %d ...\n", eClusterId, cDeviceEnpoint );

  /* Request Bind with our Cluster on Device */
  memset( &stRequest, 0, sizeof( stRequest ) );
  stRequest.target = iDeviceAddress;
  stRequest.clusterId = eClusterId;
  stRequest.srcEndpt = cDeviceEnpoint;
  stRequest.srcExtAddr = dlDeviceExtendedAddress;

  stRequest.dst.mode = ZB_APSDE_ADDRMODE_EXT;
  stRequest.dst.endpoint = ZbZclClusterGetEndpoint( pstCluster );
  stRequest.dst.extAddr = dlMyExtendedAddress;

  AppZbUtil_WaitReceiveCallbackInit();
  eStatus = ZbZdoBindReq( pstCluster->zb, &stRequest, AppZbUtil_RequestBindWithDeviceCallback, NULL );
  if ( eStatus != ZB_STATUS_SUCCESS )
  {
    LOG_INFO_APP( "ZbZdoBindReq Return (status = 0x%02x)\n", eStatus );
    bReturn = false;
  }
  else
  {
    AppZbUtil_WaitReceiveCallbackStart( APP_ZB_UTIL_WAIT_CALLBACK_TIMEOUT_DEFAULT_MS );
    if ( eCallbackStatus != ZB_STATUS_SUCCESS )
    {
      bReturn = false;
    }
  }

  return bReturn;
}

/**
 * @brief Request Bind with our Cluster on this Device
 *
 * @param pstCluster                Selected Cluster
 * @param dlMyExtendedAddress       This Device Extended Address
 * @param dlDeviceExtendedAddress   Selected Device Extended Address
 * @param cDeviceEnpoint            Selected Device Endpoint

 * @return True if request done else false.
 */
bool AppZbUtil_RequestMyBind( struct ZbZclClusterT * pstCluster, uint64_t dlMyExtendedAddress, uint64_t dlDeviceExtendedAddress, uint8_t cDeviceEnpoint )
{
  struct ZbApsmeBindReqT  stRequest;
  struct ZbApsmeBindConfT stConfig;
  enum ZbZclClusterIdT    eClusterId;
  bool                    bReturn = true;

  /* Retrieve Cluster ID */
  eClusterId = ZbZclClusterGetClusterId( pstCluster );

  LOG_INFO_APP( "Call a Bind Request on this Device for Cluster 0x%03X on EndPoint %d\n", eClusterId, cDeviceEnpoint );

  /* Request Bind with our Cluster on this Device */
  memset( &stConfig, 0, sizeof( stConfig ) );
  memset( &stRequest, 0, sizeof( stRequest ) );
  stRequest.clusterId = eClusterId;
  stRequest.srcEndpt = ZbZclClusterGetEndpoint( pstCluster );
  stRequest.srcExtAddr = dlMyExtendedAddress;

  stRequest.dst.mode = ZB_APSDE_ADDRMODE_EXT;
  stRequest.dst.endpoint = cDeviceEnpoint;
  stRequest.dst.extAddr = dlDeviceExtendedAddress;

  AppZbUtil_WaitReceiveCallbackInit();
  ZbApsmeBindReq( pstCluster->zb, &stRequest, &stConfig );
  if ( stConfig.status != ZB_STATUS_SUCCESS )
  {
    LOG_INFO_APP( "ZbApsmeBindReq Return (status = 0x%02x)\n", stConfig.status );
    bReturn = false;
  }
  else
  {
    LOG_INFO_APP( "Done\n\n" );
  }

  return bReturn;
}

/**
 * @brief Check and Display Binding Table
 *
 * @param pstZigbee     Pointer to Zigbee Stack.
 * @param bDisplay      Indicate if Display Table on only count.
 *
 * @return Number of Bind Found in table.
 */
uint16_t AppZbUtil_DisplayBindingTable( struct ZigBeeT * pstZigbee, bool bDisplay )
{
  struct ZbApsmeBindT   stEntry;
  enum ZbStatusCodeT    eStatus;
  bool        bEndDone = false;
  uint16_t    iIndex = 0, iCount = 0;

  if ( bDisplay != false )
  {
    LOG_INFO_APP( "\nDisplay Binding table below:\n" );
    LOG_INFO_APP( "  Item |  ClusterId | Dest. Ext. Address | Dst EP | Src EP |\n" );
    LOG_INFO_APP( "  -----|------------|--------------------|--------|--------|\n" );
  }

  /* Go through each elements */
  do
  {
    /* Check the end of the table */
    eStatus = ZbApsGetIndex( pstZigbee, ZB_APS_IB_ID_BINDING_TABLE, &stEntry, sizeof(stEntry), iIndex );
    if ( eStatus != ZB_APS_STATUS_SUCCESS)
    {
      if ( eStatus != ZB_APS_STATUS_INVALID_INDEX )
      {
        LOG_INFO_APP( "ERROR ! ZbApsGetIndex failed (0x%02X)\n", eStatus );
      }
      bEndDone = true;
    }
    else
    {
      /* If empty, ignore */
      if ( stEntry.srcExtAddr != 0u )
      {
        if ( bDisplay != false )
        {
          /* Display element */
          LOG_INFO_APP( "   %2d  |    0x%03X   | " LOG_DISPLAY64() " |   %03d  |   %03d  |\n", iIndex, stEntry.clusterId, LOG_NUMBER64(stEntry.dst.extAddr),
                      stEntry.dst.endpoint, stEntry.srcEndpt );
        }
        iCount++;
      }
    }
    iIndex++;
  }
  while ( bEndDone == false );

  if ( bDisplay != false )
  {
    LOG_INFO_APP( "  Found %d items\n", iCount );
  }

  return iCount;
}

/**
 * @brief Search on Binding Table all Server in relation with the selected Cluster
 *
 * @param pstCluster      Selected Cluster
 * @param pstServerList   List of Server for this Cluster
 * @param iNbServerMax    Maximum number of Server to list (to prevent overflow)
 *
 * @return Number of Server Found.
 */
uint16_t AppZbUtil_SearchServersOnBindingTable( struct ZbZclClusterT * pstCluster, struct ZbApsAddrT * pstServerList, uint16_t iNbServerMax )
{
  bool      bEndDone = false;
  uint16_t  iNbServer = 0, iIndex = 0;
  uint16_t  iClusterId;
  enum ZbStatusCodeT    eStatus;
  struct ZbApsmeBindT   stEntry;

  iClusterId = ZbZclClusterGetClusterId( pstCluster );

  /* Go through each elements */
  do
  {
    eStatus = ZbApsGetIndex( pstCluster->zb, ZB_APS_IB_ID_BINDING_TABLE, &stEntry, sizeof(stEntry), iIndex );
    if ( eStatus != ZB_APS_STATUS_SUCCESS )
    {
      bEndDone = true;
    }
    else
    {
      /* If empty, ignore */
      if ( stEntry.srcExtAddr != 0u )
      {
        if ( stEntry.clusterId == iClusterId )
        {
          memcpy( &pstServerList[iNbServer], &stEntry.dst, sizeof(struct ZbApsAddrT) );
          iNbServer++;

          /* Verify if not Overflow output List */
          if ( iNbServer >= iNbServerMax )
          {
            bEndDone = true;
          }
        }
      }
    }
    iIndex++;
  }
  while ( bEndDone == false );

  return iNbServer;
}

/**
 * @brief   Add the LinkKey of a Device with Install Code on LinkKey List.
 *
 * @param   pstZigbee           Pointer to Zigbee Stack.
 * @param   dlExtendedAddress   Device Extended Address
 * @param   szInstallCode       Device Install Code
  */
bool AppZbUtil_AddDeviceWithInstallCode( struct ZigBeeT * pstZigbee, uint64_t dlExtendedAddress, uint8_t * szInstallCode )
{
  bool                      bReturn = false;
  uint32_t                  lTcPolicy = 0;
  struct ZbApsmeAddKeyReqT  stAddKeyReq;
  struct ZbApsmeAddKeyConfT stAddKeyConf;
  static  bool              bTrustCenterDone = false;

  if ( bTrustCenterDone == false )
  {
    ZbApsGet( pstZigbee, ZB_APS_IB_ID_TRUST_CENTER_POLICY, &lTcPolicy, sizeof(lTcPolicy));
    lTcPolicy |= (ZB_APSME_POLICY_IC_SUPPORTED | ZB_APSME_POLICY_TCLK_UPDATE_REQUIRED | ZB_APSME_POLICY_TC_POLICY_CHANGE);

    ZbApsSet( pstZigbee, ZB_APS_IB_ID_TRUST_CENTER_POLICY, &lTcPolicy, sizeof(lTcPolicy) );
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
  ZbApsmeAddKeyReq( pstZigbee, &stAddKeyReq, &stAddKeyConf );
  if ( stAddKeyConf.status != ZB_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "Error Add Link Key (0x%02X)", stAddKeyConf.status );
  }
  else
  {
    LOG_INFO_APP( "Add of Device Link Key OK." );
    bReturn = true;
  }

  return bReturn;
}
