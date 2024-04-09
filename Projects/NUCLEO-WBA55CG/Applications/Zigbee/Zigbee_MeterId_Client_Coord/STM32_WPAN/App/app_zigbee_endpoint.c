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
#define APP_ZIGBEE_CLUSTER_NAME           "MeterId Client"

/* USER CODE BEGIN PD */
#define APP_ZIGBEE_STARTUP_FAIL_DELAY     500u

#define APP_ZIGBEE_APPLICATION_NAME       APP_ZIGBEE_CLUSTER_NAME
#define APP_ZIGBEE_APPLICATION_OS_NAME    "."

/* Needed Meter-ID Information defines */
#define METER_ID_STRING_MAX_SIZE          16                  // Meter-ID Field String can be have up to 16 bytes.

/* USER CODE END PD */

// -- Redefine Clusters to better code read --
#define MeterIdClient                     pstZbCluster[0]

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

  /* Add MeterId Client Cluster */
  stZigbeeAppInfo.MeterIdClient = ZbZclMeterIdClientAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT );
  assert( stZigbeeAppInfo.MeterIdClient != NULL );
  ZbZclClusterEndpointRegister( stZigbeeAppInfo.MeterIdClient );

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
 * @brief  Read Attribute caalback
 * @param  rsp: ZCL response
 * @param  arg: passed argument
 * @retval None
 */
static void APP_ZIGBEE_MeterIdClientReadAttributeCallback( const struct ZbZclReadRspT * pstResponse, void * arg)
{
  uint8_t   cBufferSize;
  uint8_t   szBuffer[METER_ID_STRING_MAX_SIZE * 2u];
  uint16_t  iLength, iIndex;
  
  UNUSED( arg );
  
  if ( pstResponse->status != ZCL_STATUS_SUCCESS)
  {
    LOG_INFO_APP( "[METER ID] The server returned an error after an Attribute Read (0x%02X)", pstResponse->status );
  }
  else
  {
    /* Read Response */
    for ( iIndex = 0; iIndex < pstResponse->count; iIndex++ )
    {
      /* Read each Attributes */
      iLength = pstResponse->attr[iIndex].length;
      if ( iLength <= sizeof( szBuffer ) )
      {
        memcpy( szBuffer, pstResponse->attr[iIndex].value, iLength );
   
        switch( pstResponse->attr[iIndex].attrId )
        {
          case ZCL_METER_ID_ATTR_COMPANY_NAME:
              /* Compute ZCL string in standard string */
              cBufferSize = szBuffer[0];
              memcpy( szBuffer, &szBuffer[1], cBufferSize );
              szBuffer[cBufferSize] = '\0';
              LOG_INFO_APP( "[METER ID] Company Name attribute value: %s", (char*)szBuffer );
              break;
          
          case ZCL_METER_ID_ATTR_METER_TYPE_ID:
              LOG_INFO_APP( "[METER ID] Meter Type ID attribute value: 0x%04x.", *(uint16_t *)szBuffer );
              break;
              
          case ZCL_METER_ID_ATTR_DATA_QUAL_ID:
              LOG_INFO_APP( "[METER ID] Data Quality ID attribute value: 0x%04x.", *(uint16_t *)szBuffer );
              break;

          case ZCL_METER_ID_ATTR_POD:
              /* Compute ZCL string in standard string */
              cBufferSize = szBuffer[0];
              memcpy( szBuffer, &szBuffer[1], cBufferSize );
              szBuffer[cBufferSize] = '\0';
              LOG_INFO_APP( "[METER ID] Point Of Delivery (POD) attribute value: %s", (char*)szBuffer );
              break;

          default:
            LOG_ERROR_APP( "[METER ID] Error on Attributes %d: unsupported attribute (%d).", pstResponse->attr[iIndex].attrId );
            break;
        } 
      }
      else
      {
        LOG_ERROR_APP( "[METER ID] Error on Attributes %d: attribute too long (%d).", iIndex, iLength );
      }
    }
  }
}

/**
 * @brief  Reading Meter-ID Attribute from server
 * @param  eAttribute   Attribute to read.
 * @retval None
 */
static void APP_ZIGBEE_MeterIdClientReadAttribute( enum ZbZclMeterIdSvrAttrT eAttribute )
{
  struct ZbZclReadReqT  stRequest;
  enum ZclStatusCodeT   eStatus;
  
  /* Read request */
  memset( &stRequest, 0, sizeof( stRequest ) );
  stRequest.dst.mode = ZB_APSDE_ADDRMODE_SHORT;
  stRequest.dst.endpoint = APP_ZIGBEE_ENDPOINT;
  stRequest.dst.nwkAddr = iDeviceShortAddress;
  stRequest.count = 1;
  stRequest.attr[0] = eAttribute;
  
  eStatus = ZbZclReadReq( stZigbeeAppInfo.MeterIdClient, &stRequest, APP_ZIGBEE_MeterIdClientReadAttributeCallback, NULL);
  if ( eStatus != ZCL_STATUS_SUCCESS) 
  {
     LOG_ERROR_APP( "[METER ID] Error during Attribute Read (0x%02X).", eStatus );
  }
}

/**
 * @brief  Reading multiples Meter-ID Attribute from server
 * @param  eAttributeList   Attribute to read.
 * @retval None
 */
static void APP_ZIGBEE_MeterIdClientReadMultipleAttribute( enum ZbZclMeterIdSvrAttrT * peAttributeList, uint16_t iNbAtributes )
{
  uint8_t               iIndex;
  struct ZbZclReadReqT  stRequest;
  enum ZclStatusCodeT   eStatus;
  
  /* Read request */
  memset( &stRequest, 0, sizeof( stRequest ) );
  stRequest.dst.mode = ZB_APSDE_ADDRMODE_SHORT;
  stRequest.dst.endpoint = APP_ZIGBEE_ENDPOINT;
  stRequest.dst.nwkAddr = iDeviceShortAddress;
  
  stRequest.count = iNbAtributes;
  for ( iIndex = 0; iIndex < iNbAtributes; iIndex++ )
  {
    stRequest.attr[iIndex] = peAttributeList[iIndex];
  }
  
  eStatus = ZbZclReadReq( stZigbeeAppInfo.MeterIdClient, &stRequest, APP_ZIGBEE_MeterIdClientReadAttributeCallback, NULL);
  if ( eStatus != ZCL_STATUS_SUCCESS) 
  {
     LOG_ERROR_APP( "[METER ID] Error during Attribute Read (0x%02X).", eStatus) ;
  }
}

/**
 * @brief  Management of the SW1 button : Receive Meter-Id Companu Name
 * @param  None
 * @retval None
 */
void APPE_Button1Action(void)
{
  /* First, verify if Appli has already Join a Network  */ 
  if ( APP_ZIGBEE_IsAppliJoinNetwork() != false )
  {
    LOG_INFO_APP( "[METER ID] Read 'Company Name' attribute :" );
    APP_ZIGBEE_MeterIdClientReadAttribute( ZCL_METER_ID_ATTR_COMPANY_NAME );
  }
}

/**
 * @brief  Management of the SW2 button : Receive few Meter-Id Information
 * @param  None
 * @retval None
 */
void APPE_Button2Action(void)
{
  enum ZbZclMeterIdSvrAttrT eAttributeList[3] = { ZCL_METER_ID_ATTR_METER_TYPE_ID, ZCL_METER_ID_ATTR_DATA_QUAL_ID, ZCL_METER_ID_ATTR_POD };
  
  /* First, verify if Appli has already Join a Network  */ 
  if ( APP_ZIGBEE_IsAppliJoinNetwork() != false )
  {
    LOG_INFO_APP( "[METER ID] Read 'Meter Type', 'Data Quality' and 'POD' attributes :" );
    APP_ZIGBEE_MeterIdClientReadMultipleAttribute( eAttributeList, 3 );
  }
}

/* USER CODE END FD_LOCAL_FUNCTIONS */
