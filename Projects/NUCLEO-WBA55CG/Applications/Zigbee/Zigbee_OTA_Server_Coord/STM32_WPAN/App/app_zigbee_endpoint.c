/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_zigbee_endpoint.c
  * Description        : Zigbee Application to manage endpoints and these clusters.
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
#include <inttypes.h>
  
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
#include "zcl/general/zcl.onoff.h"
#include "zcl/general/zcl.ota.h"

/* USER CODE BEGIN PI */
#include "zcl/zcl.payload.h"
#include "stm32wbaxx_nucleo.h"

/* USER CODE END PI */

/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_CHANNEL                14u
#define APP_ZIGBEE_CHANNEL_MASK           ( 1u << APP_ZIGBEE_CHANNEL )
#define APP_ZIGBEE_TX_POWER               ((int8_t) 10)    /* TX-Power is at +10 dBm. */

#define APP_ZIGBEE_ENDPOINT               17u
#define APP_ZIGBEE_PROFILE_ID             ZCL_PROFILE_HOME_AUTOMATION
#define APP_ZIGBEE_DEVICE_ID              ZCL_DEVICE_ONOFF_LIGHT
#define APP_ZIGBEE_GROUP_ADDRESS          0x0001u

#define APP_ZIGBEE_CLUSTER1_ID            ZCL_CLUSTER_ONOFF
#define APP_ZIGBEE_CLUSTER1_NAME          "OnOff Server"

#define APP_ZIGBEE_CLUSTER2_ID            ZCL_CLUSTER_OTA_UPGRADE
#define APP_ZIGBEE_CLUSTER2_NAME          "OTA Server"

/* USER CODE BEGIN PD */
#define APP_ZIGBEE_APPLICATION_NAME       APP_ZIGBEE_CLUSTER2_NAME
#define APP_ZIGBEE_APPLICATION_OS_NAME    "."

#define APP_ZIGBEE_PERMIT_JOIN_DELAY      60u   /* 60s */

/* USER CODE END PD */

// -- Redefine Clusters to better code read --
#define OnOffServer                       pstZbCluster[0]
#define OTAServer                         pstZbCluster[1]

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* OTA server information */
struct APP_ZIGBEE_OTAServerInfo_t
{
  uint32_t    lOtaTagOffset;      /* The OTA tag offset from the base address in flash (the address of the magic keyword is located here) */
  uint32_t    lMagicKeyword;      /* The image magic keyword (terminating word) */
};

/* OTA header and pointer to the image in flash */
struct APP_ZIGBEE_OTAImage_t
{
  struct ZbZclOtaHeader stHeader; /* The image header struct */
                                  /* Note: The field stHeader cannot be casted to uint8_t * because it is 
                                     not a packed struct instance. Hence, the field szHeaderBuffer is needed. */ 
  uint8_t     szHeaderBuffer[ZCL_OTA_HEADER_LENGTH_MAX];    /* The image header payload */
  uint32_t    lHeaderBufferSize;  /* The image header payload size */
                                  /* Note: Usually the header, tags and data are stored, ready to be transmitted as-is, 
                                     and contiguous in memory. But in our case the header and tags are stored in their struct 
                                     form and the binary is stored in a seperate memory location in flash */
  uint32_t    lBaseAddress;       /* The base address of the image in flash */
  uint32_t    lBinarySize;        /* The binary size */
};

/* USER CODE END PTD */

/* Private constants ---------------------------------------------------------*/
/* USER CODE BEGIN PC */

/* USER CODE END PC */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
static const struct APP_ZIGBEE_OTAServerInfo_t  stOTAServerInfo =
{
  .lOtaTagOffset = FUOTA_M33_APP_OTA_TAG_OFFSET,
  .lMagicKeyword = FUOTA_MAGIC_KEYWORD_M33_APP,
};

static struct APP_ZIGBEE_OTAImage_t             stImageList[] =
{
  {
    .stHeader.file_identifier = ZCL_OTA_HEADER_FILE_IDENTIFIER,
    .stHeader.header_version = ZCL_OTA_HEADER_FILE_VERSION,
    .stHeader.header_length = ZCL_OTA_HEADER_LENGTH_MIN, /* Minimum header size (no optional fields) */
    .stHeader.header_field_control = 0, /* No optional fields in the header */
    .stHeader.manufacturer_code = ST_ZIGBEE_MANUFACTURER_CODE,
    .stHeader.image_type = IMAGE_TYPE_FW_M33_APP,
    .stHeader.file_version = FILE_VERSION_FW_M33_APP,
    .stHeader.stack_version = ZCL_OTA_STACK_VERSION_PRO,
    .stHeader.header_string = "M33 binary",
    .stHeader.total_image_size = 0, /* Temporary (calculated at startup) */
    .szHeaderBuffer = {0}, /* Temporary (calculated on first block transfer) */
    .lHeaderBufferSize = 0, /* Temporary (calculated on first block transfer) */
    .lBaseAddress = FUOTA_APP_FW_BINARY_ADDRESS,
    .lBinarySize = 0, /* Temporary (calculated at startup) */
  },
  /* Note: Additional images can be added here */
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* OnOff Server Callbacks */
static enum ZclStatusCodeT  APP_ZIGBEE_OnOffServerOffCallback               ( struct ZbZclClusterT * pstCluster, struct ZbZclAddrInfoT * pstSrcInfo, void * arg );
static enum ZclStatusCodeT  APP_ZIGBEE_OnOffServerOnCallback                ( struct ZbZclClusterT * pstCluster, struct ZbZclAddrInfoT * pstSrcInfo, void * arg );
static enum ZclStatusCodeT  APP_ZIGBEE_OnOffServerToggleCallback            ( struct ZbZclClusterT * pstCluster, struct ZbZclAddrInfoT * pstSrcInfo, void * arg );

static struct ZbZclOnOffServerCallbacksT stOnOffServerCallbacks =
{
  .off = APP_ZIGBEE_OnOffServerOffCallback,
  .on = APP_ZIGBEE_OnOffServerOnCallback,
  .toggle = APP_ZIGBEE_OnOffServerToggleCallback,
};

/* OTA Server Callbacks */
static bool                 APP_ZIGBEE_OTAServerImageEvalCallback           ( struct ZbZclOtaImageDefinition * pstQueryImage, uint8_t cFieldControl, uint16_t iHardwareVersion, uint32_t * plImageSize, void * arg );
static enum ZclStatusCodeT  APP_ZIGBEE_OTAServerImageReadCallback           ( struct ZbZclOtaImageDefinition * pstImageDefinition, struct ZbZclOtaImageData * pstImageData, uint8_t cFieldControl, uint64_t dlRequestNodeAddress, struct ZbZclOtaImageWaitForData * pstImageWait, void * arg );
static enum ZclStatusCodeT  APP_ZIGBEE_OTAServerImageUpgradeEndReqCallback  ( struct ZbZclOtaImageDefinition * pstImageDefinition, enum ZclStatusCodeT eUpdateStatus, struct ZbZclOtaEndResponseTimes * pstEndResponseTimes, struct ZbZclAddrInfoT * pSourceInfo, void * arg );

/* USER CODE BEGIN PFP */
static void         APP_ZIGBEE_OTAServerStart        ( void );
static uint32_t     APP_ZIGBEE_OTAServerFindImage    ( struct ZbZclOtaImageDefinition * pstQueryImage, bool bIsSuitable);
static unsigned int APP_ZIGBEE_OTAServerHeaderMake   ( const struct ZbZclOtaHeader * pstOtaHeader, uint8_t * pcPayload );
static unsigned int APP_ZIGBEE_OTAServerHeaderAddTag ( uint16_t iTagId, uint32_t lBinarySize, unsigned int iStartIndex, uint8_t * pcPayload );

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
  /* Start OnOff Client */
  APP_ZIGBEE_OTAServerStart();

  /* Display Short Address */
  LOG_INFO_APP( "Use Short Address : 0x%04X", ZbShortAddress( stZigbeeAppInfo.pstZigbee ) );
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

  /* Add OnOff Server Cluster */
  stZigbeeAppInfo.OnOffServer = ZbZclOnOffServerAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT, &stOnOffServerCallbacks, NULL );
  assert( stZigbeeAppInfo.OnOffServer != NULL );
  ZbZclClusterEndpointRegister( stZigbeeAppInfo.OnOffServer );

  /* Add OTA Server Cluster */
  struct ZbZclOtaServerConfig stOTAServerConfig;

  memset( &stOTAServerConfig, 0, sizeof( stOTAServerConfig ) );

  stOTAServerConfig.profile_id = APP_ZIGBEE_PROFILE_ID;
  stOTAServerConfig.endpoint = APP_ZIGBEE_ENDPOINT;

  stOTAServerConfig.image_eval = APP_ZIGBEE_OTAServerImageEvalCallback;
  stOTAServerConfig.image_read = APP_ZIGBEE_OTAServerImageReadCallback;
  stOTAServerConfig.image_upgrade_end_req = APP_ZIGBEE_OTAServerImageUpgradeEndReqCallback;

  /* Please complete the other attributes according to your application */
  /* USER CODE BEGIN OTA Server */
  /* USER CODE END OTA Server */

  stZigbeeAppInfo.OTAServer = ZbZclOtaServerAlloc( stZigbeeAppInfo.pstZigbee, &stOTAServerConfig, NULL );
  assert( stZigbeeAppInfo.OTAServer != NULL );
  ZbZclClusterEndpointRegister( stZigbeeAppInfo.OTAServer );

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
  LOG_INFO_APP( "%s on Endpoint %d.", APP_ZIGBEE_CLUSTER1_NAME, APP_ZIGBEE_ENDPOINT );
  LOG_INFO_APP( "%s on Endpoint %d.", APP_ZIGBEE_CLUSTER2_NAME, APP_ZIGBEE_ENDPOINT );

  /* USER CODE BEGIN APP_ZIGBEE_PrintApplicationInfo2 */

  /* USER CODE END APP_ZIGBEE_PrintApplicationInfo2 */

  LOG_INFO_APP( "**********************************************************" );
}


/**
 * @brief  OnOff Server 'Off' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_OnOffServerOffCallback( struct ZbZclClusterT * pstCluster, struct ZbZclAddrInfoT * pstSrcInfo, void * arg )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_OnOffServerOffCallback */
  uint8_t cEndpoint;

  cEndpoint = ZbZclClusterGetEndpoint( pstCluster );
  if ( cEndpoint == APP_ZIGBEE_ENDPOINT)
  {
    LOG_INFO_APP( "[ONOFF] Red Led 'OFF'" );
    APP_LED_OFF( LED_RED );
    (void)ZbZclAttrIntegerWrite( pstCluster, ZCL_ONOFF_ATTR_ONOFF, 0 );
  }
  else
  {
    /* Unknown endpoint */
    eStatus = ZCL_STATUS_FAILURE;
  }

  /* USER CODE END APP_ZIGBEE_OnOffServerOffCallback */
  return eStatus;
}

/**
 * @brief  OnOff Server 'On' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_OnOffServerOnCallback( struct ZbZclClusterT * pstCluster, struct ZbZclAddrInfoT * pstSrcInfo, void * arg )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_OnOffServerOnCallback */
  uint8_t   cEndpoint;

  cEndpoint = ZbZclClusterGetEndpoint( pstCluster );
  if ( cEndpoint == APP_ZIGBEE_ENDPOINT )
  {
    LOG_INFO_APP( "[ONOFF] Red Led 'ON'" );
    APP_LED_ON( LED_RED );
    (void)ZbZclAttrIntegerWrite( pstCluster, ZCL_ONOFF_ATTR_ONOFF, 1 );
  }
  else
  {
    /* Unknown endpoint */
    eStatus = ZCL_STATUS_FAILURE;
  }

  /* USER CODE END APP_ZIGBEE_OnOffServerOnCallback */
  return eStatus;
}

/**
 * @brief  OnOff Server 'Toggle' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_OnOffServerToggleCallback( struct ZbZclClusterT * pstCluster, struct ZbZclAddrInfoT * pstSrcInfo, void * arg )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_OnOffServerToggleCallback */
  uint8_t   cAttrVal;

  if ( ZbZclAttrRead( pstCluster, ZCL_ONOFF_ATTR_ONOFF, NULL, &cAttrVal, sizeof(cAttrVal), false) != ZCL_STATUS_SUCCESS )
  {
    eStatus =  ZCL_STATUS_FAILURE;
  }
  else
  { 
    if ( cAttrVal != 0u )
    {
      eStatus = APP_ZIGBEE_OnOffServerOffCallback( pstCluster, pstSrcInfo, arg );
    }
    else
    {
      eStatus = APP_ZIGBEE_OnOffServerOnCallback( pstCluster, pstSrcInfo, arg );
    } 
  }

  /* USER CODE END APP_ZIGBEE_OnOffServerToggleCallback */
  return eStatus;
}

/**
 * @brief  OTA Server 'ImageEval' command Callback
 */
static bool APP_ZIGBEE_OTAServerImageEvalCallback( struct ZbZclOtaImageDefinition * pstQueryImage, uint8_t cFieldControl, uint16_t iHardwareVersion, uint32_t * plImageSize, void * arg )
{
  bool   bStatus = true;
  /* USER CODE BEGIN APP_ZIGBEE_OTAServerImageEvalCallback */
  uint32_t lIndex;

  LOG_INFO_APP( "[OTA] Query Next Image Request received" );
  LOG_INFO_APP( "[OTA] Contents of the Query Next Image Request:" );
  LOG_INFO_APP( "[OTA] Sender manufacturer code:         0x%04" PRIX16, pstQueryImage->manufacturer_code ); /* The device's assigned manufacturer code */
  LOG_INFO_APP( "[OTA] Sender current file version:      0x%08" PRIX32, pstQueryImage->file_version ); /* The device's current running file verison */
  LOG_INFO_APP( "[OTA] Sender image type:                0x%04" PRIX16, pstQueryImage->image_type ); /* The device's unique image type to distinguish manufacturer products */

  if ( cFieldControl == ZCL_OTA_QUERY_FIELD_CONTROL_HW_VERSION )
  {
    /* Additional hardware info is provided by the sender */
    LOG_INFO_APP( "[OTA] Sender Hardware version:         0x%04" PRIX16, iHardwareVersion ); /* The device's current running hardware version */
  }

  /* Find a suitable image for the given manufacturer code, image version, and file version (can only be upgraded or downgraded, will return false if the saem version is found) */
  lIndex = APP_ZIGBEE_OTAServerFindImage( pstQueryImage, true );
  if ( lIndex == UINT32_MAX )
  {
    LOG_ERROR_APP("[OTA] Error, no suitable image found.");
    bStatus = false;
  }
  else
  {
    LOG_INFO_APP( "[OTA] Found a suitable image" );
    LOG_INFO_APP( "[OTA] File version:                     0x%08" PRIX32, stImageList[lIndex].stHeader.file_version );
    LOG_INFO_APP( "[OTA] Image size:                       %u", stImageList[lIndex].stHeader.total_image_size );

    /* Send the exact file version, and the total image size in the Query Next Image Response */
    pstQueryImage->file_version = stImageList[lIndex].stHeader.file_version;
    * plImageSize = stImageList[lIndex].stHeader.total_image_size;
  }

  /* USER CODE END APP_ZIGBEE_OTAServerImageEvalCallback */
  return bStatus;
}

/**
 * @brief  OTA Server 'ImageRead' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_OTAServerImageReadCallback( struct ZbZclOtaImageDefinition * pstImageDefinition, struct ZbZclOtaImageData * pstImageData, uint8_t cFieldControl, uint64_t dlRequestNodeAddress, struct ZbZclOtaImageWaitForData * pstImageWait, void * arg )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_OTAServerImageReadCallback */
  uint32_t                              lIndex;
  struct APP_ZIGBEE_OTAImage_t *        pstOtaImage;
  struct ZbZclOtaHeader *               pstOtaHeader;
  unsigned int                          iHeaderBufferIndex;
  uint32_t                              lHeaderSize;

#if 0 /* Debug */
  if (pstImageData->file_offset == 0)
  {
    /* Log the information if it is the first Image Block/Page Request */
    LOG_INFO_APP( "[OTA] Image Block/Page Request received" );
    LOG_INFO_APP( "[OTA] Sender manufacturer code:        0x%04" PRIX16, pstImageDefinition->manufacturer_code ); /* The device's assigned manufacturer code */
    LOG_INFO_APP( "[OTA] Sender requested file version:   0x%08" PRIX32, pstImageDefinition->file_version ); /* The device's requested file verison */
    LOG_INFO_APP( "[OTA] Sender image type:               0x%04" PRIX16, pstImageDefinition->image_type ); /* The device's unique image type to distinguish manufacturer products */
  }
#endif

  /* Find the specified image */
  lIndex = APP_ZIGBEE_OTAServerFindImage( pstImageDefinition, false);
  if (lIndex == UINT32_MAX)
  {
    LOG_ERROR_APP("[OTA] Error, no image found.");
    return ZCL_STATUS_NO_IMAGE_AVAILABLE;
  }
  pstOtaImage = &stImageList[lIndex];
  pstOtaHeader = &pstOtaImage->stHeader;

#if 0 /* Debug */
  if ( pstImageData->file_offset == 0 )
  {
    /* Log the information if it is the first Image Block/Page Request */
    LOG_INFO_APP( "[OTA] Found image" );
    LOG_INFO_APP( "[OTA] File version:                    0x%08" PRIX32, pstOtaHeader->file_version );
    LOG_INFO_APP( "[OTA] Image size:                      %u", pstOtaHeader->total_image_size );
  }
#endif

  /* Display Transfer Progress */
  LOG_INFO_APP( "[OTA] FUOTA Transfer from memory address 0x%08" PRIX32, ( pstOtaImage->lBaseAddress + pstImageData->file_offset ) );

  /* Only construct the image header payload once */
  if ( pstOtaImage->lHeaderBufferSize == 0 )
  {
    /* Construct the ZCL OTA header info */
    LOG_INFO_APP( "[OTA] Constructing the image header" );
    iHeaderBufferIndex = APP_ZIGBEE_OTAServerHeaderMake( pstOtaHeader, pstOtaImage->szHeaderBuffer );
    if ( iHeaderBufferIndex != ( unsigned int )pstOtaHeader->header_length )
    {
      LOG_ERROR_APP("[OTA] Error, constructed OTA header is invalid.");
      return ZCL_STATUS_FAILURE;
    }

    /* Add associated tag to transmit firmware data as OTA sub-element */
    iHeaderBufferIndex = APP_ZIGBEE_OTAServerHeaderAddTag( ZCL_OTA_SUB_TAG_UPGRADE_IMAGE, pstOtaImage->lBinarySize, iHeaderBufferIndex, pstOtaImage->szHeaderBuffer );
    if ( iHeaderBufferIndex != ( unsigned int )pstOtaHeader->header_length + ZCL_OTA_IMAGE_BLOCK_SUB_ELEMENT_HEADER )
    {
      LOG_ERROR_APP("[OTA] Error, constructed OTA header is invalid.");
      return ZCL_STATUS_FAILURE;
    }

    pstOtaImage->lHeaderBufferSize = ( uint32_t )iHeaderBufferIndex;
  }

  /* Copy the data */
  if ( pstImageData->file_offset < pstOtaImage->lHeaderBufferSize )
  {
    /* First, copy the header */
    lHeaderSize = pstOtaImage->lHeaderBufferSize - pstImageData->file_offset;
    if ( lHeaderSize >= pstImageData->data_size )
    {
      /* Copy a portion of the header */
      lHeaderSize = pstImageData->data_size;
      memcpy( pstImageData->data, &pstOtaImage->szHeaderBuffer[pstImageData->file_offset], lHeaderSize );
    }
    else
    {
      /* Copy the remaining portion of the header plus a portion of the image */
      memcpy( pstImageData->data, &pstOtaImage->szHeaderBuffer[pstImageData->file_offset], lHeaderSize );
      memcpy( ( pstImageData->data + lHeaderSize ), ( void * )pstOtaImage->lBaseAddress, ( pstImageData->data_size - lHeaderSize ) );
    }
  }
  else
  {
    /* Second, copy the image */
    if ( pstImageData->file_offset > pstOtaHeader->total_image_size ) 
    {
      LOG_ERROR_APP("[OTA] Error, the offset requested (%u) is larger than the image size (%u).", pstImageData->file_offset, pstOtaHeader->total_image_size );
      return ZCL_STATUS_INVALID_VALUE;
    }

    /* If needed adjust the last block for the end of the image */
    if ( ( pstImageData->file_offset + pstImageData->data_size ) > pstOtaHeader->total_image_size) 
    {
      pstImageData->data_size = pstOtaHeader->total_image_size - pstImageData->file_offset;
    }

    /* Copy the data from the base address */
    memcpy( pstImageData->data, ( void * )( pstOtaImage->lBaseAddress + ( pstImageData->file_offset - pstOtaImage->lHeaderBufferSize ) ), pstImageData->data_size );
  }

  /* USER CODE END APP_ZIGBEE_OTAServerImageReadCallback */
  return eStatus;
}

/**
 * @brief  OTA Server 'ImageUpgradeEndReq' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_OTAServerImageUpgradeEndReqCallback( struct ZbZclOtaImageDefinition * pstImageDefinition, enum ZclStatusCodeT eUpdateStatus,  struct ZbZclOtaEndResponseTimes * pstEndResponseTimes, struct ZbZclAddrInfoT * pSourceInfo, void * arg)
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_OTAServerImageUpgradeEndReqCallback */

  if ( eUpdateStatus == ZCL_STATUS_SUCCESS )
  {
    /* Permit Join during few seconds */
    APP_ZIGBEE_PermitJoin( APP_ZIGBEE_PERMIT_JOIN_DELAY );
  }

  eStatus = eUpdateStatus;
  /* USER CODE END APP_ZIGBEE_OTAServerImageUpgradeEndReqCallback */
  return eStatus;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

/**
 * @brief  OTA Server find image helper
 * @param  pstQuery: Sender image query definition pointer
 * @param  bIsSuitable: True if looking for a suitable image (for upgrade or downgrade)
 * @param  piPosition: Pointer to index value
 * @retval Image index value
 */
static uint32_t APP_ZIGBEE_OTAServerFindImage( struct ZbZclOtaImageDefinition * pstQueryImage, bool bIsSuitable)
{
  uint32_t                      lIndex;
  uint32_t                      iReturnIndex = UINT32_MAX;
  struct ZbZclOtaHeader *       pstOtaHeader;

  for ( lIndex = 0; lIndex < ( sizeof( stImageList ) / sizeof( stImageList[0] ) ); lIndex++ )
  {
    pstOtaHeader = &stImageList[lIndex].stHeader;

    /* Allow either wildcard or exact match for the manufacturer code */
    if ( ( pstQueryImage->manufacturer_code != ZB_MFG_CODE_WILDCARD) && ( pstOtaHeader->manufacturer_code != pstQueryImage->manufacturer_code ) ) 
    {
      continue;
    }

    /* Allow either wildcard or exact match for the image type */
    if ( ( pstQueryImage->image_type != ZCL_OTA_IMAGE_TYPE_WILDCARD ) && ( pstOtaHeader->image_type != pstQueryImage->image_type ) )
    {
      continue;
    }

    if ( bIsSuitable )
    {
      /* Allow upgrades only */
      if ( pstOtaHeader->file_version <= pstQueryImage->file_version )
      {
        continue;
      }

      /* Return the highest file version */
      if ( iReturnIndex == UINT32_MAX || pstOtaHeader->file_version > stImageList[iReturnIndex].stHeader.file_version )
      {
        iReturnIndex = lIndex;
      }
    }
    else
    {
      /* Allow only an exact match for the file version */
      if ( pstOtaHeader->file_version == pstQueryImage->file_version )
      {
        return lIndex;
      }
    }
  }

  return iReturnIndex;
}

/**
 * @brief  OTA Server OTA file format header generation helper
 * @param  pstOtaHeader: OTA header structure
 * @param  pcPayload: Resulting OTA header in binary format
 * @retval Payload index value
 */
static unsigned int APP_ZIGBEE_OTAServerHeaderMake( const struct ZbZclOtaHeader * pstOtaHeader, uint8_t * pcPayload )
{
  unsigned int          iIndex = 0;
  const unsigned int    iCapacity = ZCL_OTA_HEADER_LENGTH_MAX;
  const uint32_t        lHeaderStringSize = sizeof( pstOtaHeader->header_string ) / sizeof( pstOtaHeader->header_string[0] );

  /* Check OTA header file identifier */
  if ( pstOtaHeader->file_identifier != ZCL_OTA_HEADER_FILE_IDENTIFIER ) 
  {
    return 0;
  }

  if ( zb_zcl_append_uint32( pcPayload, iCapacity, &iIndex, pstOtaHeader->file_identifier ) < 0 )
  {
    return iIndex;
  }
  if ( zb_zcl_append_uint16( pcPayload, iCapacity, &iIndex, pstOtaHeader->header_version ) < 0 )
  {
    return iIndex;
  }
  if ( zb_zcl_append_uint16( pcPayload, iCapacity, &iIndex, pstOtaHeader->header_length ) < 0 )
  {
    return iIndex;
  }
  if ( zb_zcl_append_uint16( pcPayload, iCapacity, &iIndex, pstOtaHeader->header_field_control ) < 0 )
  {
    return iIndex;
  }
  if ( zb_zcl_append_uint16( pcPayload, iCapacity, &iIndex, pstOtaHeader->manufacturer_code ) < 0 )
  {
    return iIndex;
  }
  if ( zb_zcl_append_uint16( pcPayload, iCapacity, &iIndex, pstOtaHeader->image_type ) < 0 )
  {
    return iIndex;
  }
  if ( zb_zcl_append_uint32( pcPayload, iCapacity, &iIndex, pstOtaHeader->file_version ) < 0 )
  {
    return iIndex;
  }
  if ( zb_zcl_append_uint16( pcPayload, iCapacity, &iIndex, pstOtaHeader->stack_version ) < 0 )
  {
    return iIndex;
  }
  if ( ( iIndex + 32u ) > iCapacity ) 
  {
    return iIndex;
  }
  memcpy( pcPayload + iIndex, pstOtaHeader->header_string, lHeaderStringSize );
  iIndex += lHeaderStringSize;
  if ( zb_zcl_append_uint32( pcPayload, iCapacity, &iIndex, pstOtaHeader->total_image_size) < 0 )
  {
    return iIndex;
  }

  /* Optional fields are based field control */
  if ( pstOtaHeader->header_field_control & ZCL_OTA_HEADER_FIELD_CONTROL_SECURITY_VERSION )
  {
    if ( zb_zcl_append_uint8( pcPayload, iCapacity, &iIndex, pstOtaHeader->sec_credential_version) < 0 )
    {
      return iIndex;
    }
  }
  if ( pstOtaHeader->header_field_control & ZCL_OTA_HEADER_FIELD_CONTROL_DEVICE_SPECIFIC )
  {
    if ( zb_zcl_append_uint64( pcPayload, iCapacity, &iIndex, pstOtaHeader->file_destination ) < 0 )
    {
      return iIndex;
    }
  }
  if ( pstOtaHeader->header_field_control & ZCL_OTA_HEADER_FIELD_CONTROL_HARDWARE_VERSIONS )
  {
    if ( zb_zcl_append_uint16( pcPayload, iCapacity, &iIndex, pstOtaHeader->min_hardware_version ) < 0 )
    {
      return iIndex;
    }
    if ( zb_zcl_append_uint16( pcPayload, iCapacity, &iIndex, pstOtaHeader->max_hardware_version ) < 0 )
    {
      return iIndex;
    }
  }

  return iIndex;
}

/**
 * @brief  OTA Server OTA file format header generation helper
 * @param  iTagId: OTA tag ID
 * @param  lBinarySize: OTA binary size
 * @retval Payload index value
 */
static unsigned int APP_ZIGBEE_OTAServerHeaderAddTag( uint16_t iTagId, uint32_t lBinarySize, unsigned int iStartIndex, uint8_t * pcPayload )
{
  unsigned int          iIndex = iStartIndex;
  const unsigned int    iCapacity = ZCL_OTA_HEADER_LENGTH_MAX;

  if ( zb_zcl_append_uint16( pcPayload, iCapacity, &iIndex, iTagId ) < 0 )
  {
    return iIndex;
  }
  if ( zb_zcl_append_uint32( pcPayload, iCapacity, &iIndex, lBinarySize ) < 0 )
  {
    return iIndex;
  }

  return iIndex;
}

/**
 *
 */
static void APP_ZIGBEE_PrintServerInformation( void )
{
  uint32_t                              lIndex;
  struct APP_ZIGBEE_OTAImage_t *        pstOtaImage;
  struct ZbZclOtaHeader *               pstOtaHeader;

  LOG_INFO_APP("[OTA] Contents of the image list");
  for ( lIndex = 0; lIndex < sizeof( stImageList ) / sizeof ( stImageList[0] ); lIndex++ )
  {
    pstOtaImage = &stImageList[lIndex];
    pstOtaHeader = &pstOtaImage->stHeader;
    LOG_INFO_APP("[OTA] Image list index:                 %u", lIndex);
    LOG_INFO_APP("[OTA] File identifier:                  0x%08" PRIX32, pstOtaHeader->file_identifier);
    LOG_INFO_APP("[OTA] Header version:                   0x%04" PRIX16, pstOtaHeader->header_version);
    LOG_INFO_APP("[OTA] Header length:                    %u", pstOtaHeader->header_length);
    LOG_INFO_APP("[OTA] Header field control:             0x%04" PRIX16, pstOtaHeader->header_field_control);
    LOG_INFO_APP("[OTA] Manufacturer code:                0x%04" PRIX16, pstOtaHeader->manufacturer_code);
    LOG_INFO_APP("[OTA] Image type:                       0x%04" PRIX16, pstOtaHeader->image_type);
    LOG_INFO_APP("[OTA] File version:                     0x%08" PRIX32, pstOtaHeader->file_version);
    LOG_INFO_APP("[OTA] Stack version:                    0x%04" PRIX16, pstOtaHeader->stack_version);
    LOG_INFO_APP("[OTA] Header string:                    %.32s", pstOtaHeader->header_string);
    LOG_INFO_APP("[OTA] Image size:                       %u", pstOtaHeader->total_image_size);
    LOG_INFO_APP("[OTA] Base address:                     0x%08" PRIX32, pstOtaImage->lBaseAddress);
    LOG_INFO_APP("[OTA] Binary size:                      %u", pstOtaImage->lBinarySize);
    if ( ( pstOtaHeader->header_field_control & ZCL_OTA_HEADER_FIELD_CONTROL_SECURITY_VERSION ) != 0 )
    {
      LOG_INFO_APP("[OTA] Security credential version:      %u", pstOtaHeader->sec_credential_version);
    }
    if ( ( pstOtaHeader->header_field_control & ZCL_OTA_HEADER_FIELD_CONTROL_DEVICE_SPECIFIC ) != 0 )
    {
      LOG_INFO_APP("[OTA] File destination:                 " LOG_DISPLAY64(), LOG_NUMBER64( pstOtaHeader->file_destination ) );
    }
    if ( ( pstOtaHeader->header_field_control & ZCL_OTA_HEADER_FIELD_CONTROL_HARDWARE_VERSIONS ) != 0 )
    {
      LOG_INFO_APP("[OTA] Minimum hardware version:         %u", pstOtaHeader->min_hardware_version);
      LOG_INFO_APP("[OTA] Maximum hardware version:         %u", pstOtaHeader->max_hardware_version);
    }
  }
}

/**
 * @brief  OTA Server Image Notify command
 * @param  lIndex: Index in stImageList
 * @retval None
 */
static void APP_ZIGBEE_OTAServer_ImageNotify( void )
{
  struct ZbApsAddrT                     stDestination;
  struct ZbZclOtaImageDefinition        stImageDefinition;
  uint32_t                              lIndex;
  struct APP_ZIGBEE_OTAImage_t *        pstOtaImage;
  struct ZbZclOtaHeader *               pstOtaHeader;
  enum ZclStatusCodeT                   eStatus;

  /* Destination configuration */
  memset( &stDestination, 0, sizeof(stDestination) );
  stDestination.mode = ZB_APSDE_ADDRMODE_SHORT;
  stDestination.endpoint = 255; /* Broadcast => any endpoint */
  stDestination.nwkAddr = 0xFFFF; /* Broadcast mode */ 

  LOG_INFO_APP( "[OTA] Broadcasting the image definitions from the image list" );
  for ( lIndex = 0; lIndex < sizeof( stImageList ) / sizeof( stImageList[0]); lIndex++ )
  {
    pstOtaImage = &stImageList[lIndex];
    pstOtaHeader = &pstOtaImage->stHeader;

    /* Image definition configuration */
    memset( &stImageDefinition, 0, sizeof(stImageDefinition) );
    stImageDefinition.manufacturer_code = pstOtaHeader->manufacturer_code;
    stImageDefinition.image_type = pstOtaHeader->image_type;
    stImageDefinition.file_version = pstOtaHeader->file_version;

    /* Sending Image Notify Request to client */
    eStatus = ZbZclOtaServerImageNotifyReq( stZigbeeAppInfo.OTAServer, &stDestination, ZCL_OTA_NOTIFY_TYPE_FILE_VERSION, 0, &stImageDefinition );
    if( eStatus != ZCL_STATUS_SUCCESS )
    {
      LOG_ERROR_APP("[OTA] Error, Image Notify Request failed (0x%02" PRIX8 ")", eStatus);
    }
  }
}

/**
 * @brief  OTA Server OTA get binary information helper
 * @param  lBaseAddress: OTA base address in flash
 * @param  lOTATagAddress: OTA tag address in flash (the address of the magic keyword is located here)
 * @param  lMagicKeyword: OTA magic keyword (terminating word of the image)
 * @param  plsize: OTA image size in flash
 * @retval False if the magic keyword address is outside the flash region or the magic keyword is incorrect
 */
static bool APP_ZIGBEE_OTAGetBinInfo( uint32_t lBaseAddress, uint32_t lOTATagAddress, uint32_t lMagicKeyword, uint32_t * plSize)
{
  const uint32_t        lFlashEndAddress = FLASH_BASE + FLASH_SIZE;
  const uint32_t        lMagicKeywordAddress = * ( uint32_t * )( lOTATagAddress );
  uint32_t              lBinaryMagicKeyword;
  uint32_t              lBinaryEndAddress;

  /* Check if the magic keyword address is inside the flash region */
  if ( lMagicKeywordAddress < lBaseAddress || lMagicKeywordAddress > lFlashEndAddress - sizeof( lBinaryMagicKeyword ) )
  {
    LOG_ERROR_APP( "[OTA] Error, magic keyword address is outside the flash region (0x%08" PRIX32 ").", lMagicKeywordAddress );
    return false;
  }

  /* Check if the magic keyword is correct */
  lBinaryMagicKeyword = * ( uint32_t * )( lMagicKeywordAddress );
  if ( lBinaryMagicKeyword != lMagicKeyword )
  {
    LOG_ERROR_APP( "[OTA] Error, magic keyword is incorrect (0x%08" PRIX32 " != 0x%08" PRIX32 ").", lBinaryMagicKeyword, lMagicKeyword );
    return false;
  }

  lBinaryEndAddress = lMagicKeywordAddress + sizeof( lMagicKeyword );
  *plSize = lBinaryEndAddress - lBaseAddress;

  LOG_DEBUG_APP( "*plSize = 0x%08" PRIX32, * plSize );

  return true;
}

/**
 * @brief  Start the OTA Server.
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_OTAServerStart(void)
{
  uint32_t                              lIndex;
  struct APP_ZIGBEE_OTAImage_t *        pstOtaImage;
  struct ZbZclOtaHeader *               pstOtaHeader;
  uint32_t                              lBinarySize;

  /* Get the binary info for each binary in the image list */
  LOG_INFO_APP("[OTA] Getting the binary info for the image list");
  for ( lIndex = 0; lIndex < sizeof( stImageList ) / sizeof (stImageList[0]); lIndex++ )
  {
    pstOtaImage = &stImageList[lIndex];
    pstOtaHeader = &pstOtaImage->stHeader;
    if ( APP_ZIGBEE_OTAGetBinInfo( pstOtaImage->lBaseAddress, pstOtaImage->lBaseAddress + stOTAServerInfo.lOtaTagOffset, stOTAServerInfo.lMagicKeyword, &lBinarySize ) == false )
    {
      LOG_ERROR_APP( "[OTA] Error, could not determine the image size." );
      return;
    }
    /* The total image size is the sum of the OTA header and the actual file data (along with its tag) lengths */
    pstOtaHeader->total_image_size = ( uint32_t ) pstOtaHeader->header_length + ZCL_OTA_IMAGE_BLOCK_SUB_ELEMENT_HEADER + lBinarySize;
    pstOtaImage->lBinarySize = lBinarySize;
  }
}

/**
 * @brief  Management of the SW1 button : Send Image Notify Request
 * @param  None
 * @retval None
 */
void APPE_Button1Action(void)
{
  /* First, verify if Appli has already Join a Network  */ 
  if ( APP_ZIGBEE_IsAppliJoinNetwork() != false )
  {
    LOG_INFO_APP( "[OTA] SW1 Pushed, Sending Image Notify Requests" );
    APP_ZIGBEE_OTAServer_ImageNotify();
  }
}

/**
 * @brief  Management of the SW2 button : Print the server information
 * @param  None
 * @retval None
 */
void APPE_Button2Action(void)
{
  APP_ZIGBEE_PrintServerInformation();
}

/**
 * @brief  Management of the SW3 button : Start a period to authorize a Join.
 * @param  None
 * @retval None
 */
void APPE_Button3Action(void)
{
  /* First, verify if Appli has already Join a Network  */ 
  if ( APP_ZIGBEE_IsAppliJoinNetwork() != false )
  {
    /* Permit Join during few seconds */
    APP_ZIGBEE_PermitJoin( APP_ZIGBEE_PERMIT_JOIN_DELAY );
  }
}

/* USER CODE END FD_LOCAL_FUNCTIONS */
