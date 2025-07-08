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
#include "zcl/general/zcl.ota.h"

/* USER CODE BEGIN PI */
#include "stm32wbaxx_nucleo.h"
#include "hw_flash.h"

/* USER CODE END PI */

/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_CHANNEL                14u
#define APP_ZIGBEE_CHANNEL_MASK           ( 1u << APP_ZIGBEE_CHANNEL )
#define APP_ZIGBEE_TX_POWER               ((int8_t) 10)    /* TX-Power is at +10 dBm. */

#define APP_ZIGBEE_ENDPOINT               17u
#define APP_ZIGBEE_PROFILE_ID             ZCL_PROFILE_HOME_AUTOMATION
#define APP_ZIGBEE_DEVICE_ID              ZCL_DEVICE_ONOFF_SWITCH
#define APP_ZIGBEE_GROUP_ADDRESS          0x0001u

#define APP_ZIGBEE_CLUSTER_ID             ZCL_CLUSTER_OTA_UPGRADE
#define APP_ZIGBEE_CLUSTER_NAME           "OTA Client"

/* USER CODE BEGIN PD */
#define APP_ZIGBEE_APPLICATION_NAME       APP_ZIGBEE_CLUSTER_NAME
#define APP_ZIGBEE_APPLICATION_OS_NAME    "."

#define APP_ZIGBEE_LED_TOGGLE_MS          1000u

#define APP_ZIGBEE_OTAClient_WRITE_TO_FLASH
#define APP_ZIGBEE_OTAClient_DELETE_SECTORS_IF_INVALID

// -- Redefine Tasks to better code read --
#define TASK_ZIGBEE_OTA_REQUEST_UPGRADE           TASK_ZIGBEE_APP1
#define TASK_ZIGBEE_OTA_START_DOWNLOAD            TASK_ZIGBEE_APP2

#define TASK_ZIGBEE_OTA_REQUEST_UPGRADE_PRIORITY  CFG_SEQ_PRIO_0
#define TASK_ZIGBEE_OTA_START_DOWNLOAD_PRIORITY   CFG_SEQ_PRIO_0

#define EVENT_ZIGBEE_OTA_SERVER_FOUND             EVENT_ZIGBEE_APP1

/* USER CODE END PD */

// -- Redefine Clusters to better code read --
#define OTAClient                         pstZbCluster[0]

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* OTA client information */
struct APP_ZIGBEE_OTAClientInfo_t
{
  uint32_t              lBaseAddress; /* The image base address in flash */
  uint32_t              lOTATagAddress; /* The OTA tag address in flash (the address of the magic keyword is located here) */
  uint32_t              lMagicKeyword; /* The image magic keyword (terminating word of the image) */
  uint16_t              iManufacturerCode; /* The manufacturer code for this device */
  uint16_t              iImageType; /* The image type for this device */
  uint32_t              lCurrentFileVersion; /* The current file version for this device */
};

/* OTA client block transfer cached information */
struct APP_ZIGBEE_OTAClientBlockTransfer_t
{
  /* Transmission related fields */
  struct ZbZclOtaHeader stCurrentHeader; /* The image header of the current ongoing transmission */
  uint32_t              lCurrentOffset; /* The current offset in the ongoing transmission image data */
  /* RAM buffer related fields */
  uint8_t 	        szFirmwareBuffer[RAM_FIRMWARE_BUFFER_SIZE]; /* The RAM buffer for temporary image data storage before transferring to flash */
  uint32_t 	        lFirmwareBufferCurrentOffset; /* The current offset in the RAM buffer (should wrap when reaching the size) */
  bool                  bFirmwareBufferFull; /* True if the RAM buffer is full and needs to be transferred to flash */
  /* Flash related fields */
  uint32_t 	        lFlashCurrentAddr; /* The current address in flash */
};

/* OTA client global cached information */
struct APP_ZIGBEE_OTAClientGlobals_t
{
  bool                                          bDownloadStarted; /* Is the download started? Y/n */
  uint32_t                                      lDownloadStartTime; /* The download start time for the ongoing transmission */
  uint32_t                                      lDownloadEndTime; /* The download end time for the ongoing transmission */
  struct APP_ZIGBEE_OTAClientBlockTransfer_t    stBlockTransfer; /* The block transfer related information */
};

/* Reboot Function definition */
typedef void ( * ReBootFunction_t ) ( void );

/* USER CODE END PTD */

/* Private constants ---------------------------------------------------------*/
/* USER CODE BEGIN PC */

/* USER CODE END PC */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
/* OTA application variables */
static struct APP_ZIGBEE_OTAClientInfo_t                stOTAClientInfo =
{
  .lBaseAddress = FUOTA_APP_FW_BINARY_ADDRESS,
  .lOTATagAddress = FUOTA_M33_APP_OTA_TAG_ADDRESS,
  .lMagicKeyword = FUOTA_MAGIC_KEYWORD_M33_APP,
  .iManufacturerCode = ST_ZIGBEE_MANUFACTURER_CODE,
  .iImageType = IMAGE_TYPE_FW_M33_APP,
  .lCurrentFileVersion = CURRENT_FW_M33_APP_FILE_VERSION,
};

static struct APP_ZIGBEE_OTAClientGlobals_t             stOTAClientGlobals;

/* Timer for LED toggling */
static UTIL_TIMER_Object_t                              stTimerLedToggle;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* OTA Client Callbacks */
static void                 APP_ZIGBEE_OTAClientDiscoverCompleteCallback    ( struct ZbZclClusterT * pstCluster, enum ZclStatusCodeT eStatus, void * arg );
static enum ZclStatusCodeT  APP_ZIGBEE_OTAClientImageNotifyCallback         ( struct ZbZclClusterT * pstCluster, uint8_t cPayloadType, uint8_t cJitter, struct ZbZclOtaImageDefinition * pstImageDefinition, struct ZbApsdeDataIndT * pstDataInd, struct ZbZclHeaderT * pstZclHeader );
static void                 APP_ZIGBEE_OTAClientQueryNextCallback           ( struct ZbZclClusterT * pstCluster, enum ZclStatusCodeT eStatus, struct ZbZclOtaImageDefinition * pstImageDefinition, uint32_t lImageSize, void * arg );
static enum ZclStatusCodeT  APP_ZIGBEE_OTAClientWriteImageCallback          ( struct ZbZclClusterT * pstCluster, struct ZbZclOtaHeader * pstHeader, uint8_t cLength, uint8_t * pData, void * arg );
static enum ZclStatusCodeT  APP_ZIGBEE_OTAClientImageValidateCallback       ( struct ZbZclClusterT * pstCluster, struct ZbZclOtaHeader * pstHeader, void * arg );
static void                 APP_ZIGBEE_OTAClientRebootCallback              ( struct ZbZclClusterT * pstCluster, void * arg );
static enum ZclStatusCodeT  APP_ZIGBEE_OTAClientAbortDownloadCallback       ( struct ZbZclClusterT * pstCluster, enum ZbZclOtaCommandId eCommandId, void * arg );

/* USER CODE BEGIN PFP */
static void                 APP_ZIGBEE_LEDToggle                    ( void * arg );
static void                 APP_ZIGBEE_ApplicationTaskInit          ( void );
static void                 APP_ZIGBEE_OTAClientStart               ( void );
static void                 APP_ZIGBEE_OTAClientStartDownload       ( void );
static void                 APP_ZIGBEE_OTAClientResetGlobals        ( void );
static void                 APP_ZIGBEE_OTAClientRequestUpgrade      ( void );
static bool                 APP_ZIGBEE_OTAGetBinInfo                ( uint32_t lBaseAddress, uint32_t lOTATagAddress, uint32_t lMagicKeyword, uint32_t * plSize );
#ifdef APP_ZIGBEE_OTAClient_WRITE_TO_FLASH
static inline void                      APP_ZIGBEE_DeleteSectors                ( uint32_t lBaseAddress, uint32_t lBinarySize );
static inline APP_ZIGBEE_StatusTypeDef  APP_ZIGBEE_OTAClientWriteFirmwareData   ( uint32_t lFlashAddress, uint8_t * pcFirmwareBuffer, uint32_t lFirmwareBufferSize );
#endif // APP_ZIGBEE_OTAClient_WRITE_TO_FLASH

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
  /* Start OTA Client */
  APP_ZIGBEE_OTAClientStart();

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

  /* Add OTA Client Cluster */
  struct ZbZclOtaClientConfig stOTAClientConfig;

  memset( &stOTAClientConfig, 0, sizeof( stOTAClientConfig ) );

  stOTAClientConfig.profile_id = APP_ZIGBEE_PROFILE_ID;
  stOTAClientConfig.endpoint = APP_ZIGBEE_ENDPOINT;
  stOTAClientConfig.activation_policy = ZCL_OTA_ACTIVATION_POLICY_SERVER;
  stOTAClientConfig.timeout_policy = ZCL_OTA_TIMEOUT_POLICY_APPLY_UPGRADE;

  ZbZclOtaClientGetDefaultCallbacks( &stOTAClientConfig.callbacks );

  stOTAClientConfig.callbacks.discover_complete = APP_ZIGBEE_OTAClientDiscoverCompleteCallback;
  stOTAClientConfig.callbacks.image_notify = APP_ZIGBEE_OTAClientImageNotifyCallback;
  stOTAClientConfig.callbacks.query_next = APP_ZIGBEE_OTAClientQueryNextCallback;
  stOTAClientConfig.callbacks.write_image = APP_ZIGBEE_OTAClientWriteImageCallback;
  stOTAClientConfig.callbacks.image_validate = APP_ZIGBEE_OTAClientImageValidateCallback;
  stOTAClientConfig.callbacks.reboot = APP_ZIGBEE_OTAClientRebootCallback;
  stOTAClientConfig.callbacks.abort_download = APP_ZIGBEE_OTAClientAbortDownloadCallback;

  /* Please complete the other attributes according to your application */
  /* USER CODE BEGIN OTA Client */
  /* USER CODE END OTA Client */

  stZigbeeAppInfo.OTAClient = ZbZclOtaClientAlloc( stZigbeeAppInfo.pstZigbee, &stOTAClientConfig, NULL );
  assert( stZigbeeAppInfo.OTAClient != NULL );
  ZbZclClusterEndpointRegister( stZigbeeAppInfo.OTAClient );

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
 * @brief  OTA Client 'DiscoverComplete' command Callback
 */
static void APP_ZIGBEE_OTAClientDiscoverCompleteCallback( struct ZbZclClusterT * pstCluster, enum ZclStatusCodeT eStatus, void * arg )
{
  /* USER CODE BEGIN APP_ZIGBEE_OTAClientDiscoverCompleteCallback */
  enum ZclStatusCodeT   eInternalStatus = ZCL_STATUS_SUCCESS;
  uint64_t              lRequestedServerExt = 0;

  if ( eStatus == ZCL_STATUS_SUCCESS )
  {
    /* The OTA server extended address in stored in ZCL_OTA_ATTR_UPGRADE_SERVER_ID attribute */
    lRequestedServerExt = ZbZclAttrIntegerRead( stZigbeeAppInfo.OTAClient, ZCL_OTA_ATTR_UPGRADE_SERVER_ID, NULL, &eInternalStatus );
    if ( eInternalStatus != ZCL_STATUS_SUCCESS )
    {
      LOG_ERROR_APP( "[OTA] Error, ZbZclAttrIntegerRead failed (0x%02" PRIX8 ").", eInternalStatus );
      return;
    }

    LOG_INFO_APP( "[OTA] Server located with extended address " LOG_DISPLAY64() ".", LOG_NUMBER64( lRequestedServerExt ) );
  }
  else
  {
    LOG_INFO_APP( "[OTA] No OTA Server located on Network ..." );
  }
  UTIL_SEQ_SetEvt( EVENT_ZIGBEE_OTA_SERVER_FOUND );

  /* USER CODE END APP_ZIGBEE_OTAClientDiscoverCompleteCallback */

}

/**
 * @brief  OTA Client 'ImageNotify' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_OTAClientImageNotifyCallback( struct ZbZclClusterT * pstCluster, uint8_t cPayloadType, uint8_t cJitter, struct ZbZclOtaImageDefinition * pstImageDefinition, struct ZbApsdeDataIndT * pstDataInd, struct ZbZclHeaderT * pstZclHeader )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_OTAClientImageNotifyCallback */
  
  LOG_INFO_APP( "[OTA] Received a Image Notify Request" );

  /* Request an upgrade if the manufacturer code, image type, and file version are okay */
  switch( cPayloadType )
  {
    case ZCL_OTA_NOTIFY_TYPE_FILE_VERSION:
      LOG_INFO_APP( "[OTA] Contents of the Image Notify Request:" );
      LOG_INFO_APP( "[OTA] Jitter: %d", cJitter );
      LOG_INFO_APP( "[OTA] Manufacturer code:              %u", pstImageDefinition->manufacturer_code );
      LOG_INFO_APP( "[OTA] Image type:                     0x%04" PRIX16, pstImageDefinition->image_type );
      LOG_INFO_APP( "[OTA] File version:                   0x%08" PRIX32,  pstImageDefinition->file_version);

      if ( pstImageDefinition->manufacturer_code != stOTAClientInfo.iManufacturerCode )
      {
        LOG_ERROR_APP( "[OTA] Error, unauthorized manufacturer code (0x%04" PRIX16 ")", pstImageDefinition->manufacturer_code );
        return ZCL_STATUS_FAILURE;
      }

      if ( pstImageDefinition->image_type != stOTAClientInfo.iImageType )
      {
        LOG_ERROR_APP( "[OTA] Error, unauthorized image type (0x%04" PRIX16 ")", pstImageDefinition->image_type );
        return ZCL_STATUS_FAILURE;
      }

      if ( pstImageDefinition->file_version <= stOTAClientInfo.lCurrentFileVersion )
      {
        LOG_ERROR_APP( "[OTA] Error, cannot upgrade to the file version (0x%04" PRIX16 ")", pstImageDefinition->file_version );
        return ZCL_STATUS_FAILURE;
      }

      LOG_INFO_APP( "[OTA] Requesting an upgrade" );
      UTIL_SEQ_SetTask( TASK_ZIGBEE_OTA_REQUEST_UPGRADE, TASK_ZIGBEE_OTA_REQUEST_UPGRADE_PRIORITY );
      break;

    default:
      LOG_ERROR_APP( "[OTA] Error, unsupported payload type (0x%02" PRIX8 ")", cPayloadType );
      return ZCL_STATUS_FAILURE;
  }

  /* USER CODE END APP_ZIGBEE_OTAClientImageNotifyCallback */
  return eStatus;
}

/**
 * @brief  OTA Client 'QueryNext' command Callback
 */
static void APP_ZIGBEE_OTAClientQueryNextCallback( struct ZbZclClusterT * pstCluster, enum ZclStatusCodeT eStatus, struct ZbZclOtaImageDefinition * pstImageDefinition, uint32_t lImageSize, void * arg )
{
  /* USER CODE BEGIN APP_ZIGBEE_OTAClientQueryNextCallback */
  const uint32_t        lFlashEndAddress = FLASH_BASE + FLASH_SIZE;
  const uint32_t        lMaximumImageSize = lFlashEndAddress - stOTAClientInfo.lBaseAddress;

  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "[OTA] Error, Query Next Image Response failed (0x%02" PRIX8 ").", eStatus );
    return;
  }

  if ( lImageSize > lMaximumImageSize )
  {
    LOG_ERROR_APP( "[OTA] Error, image will not fit in flash (%u).", lImageSize );
  }

  LOG_INFO_APP( "[OTA] Starting an OTA download" );
  UTIL_SEQ_SetTask( TASK_ZIGBEE_OTA_START_DOWNLOAD, TASK_ZIGBEE_OTA_START_DOWNLOAD_PRIORITY );

  /* USER CODE END APP_ZIGBEE_OTAClientQueryNextCallback */

}

/**
 * @brief  OTA Client 'WriteImage' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_OTAClientWriteImageCallback( struct ZbZclClusterT * pstCluster, struct ZbZclOtaHeader * pstHeader, uint8_t cLength, uint8_t * pData, void * arg )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_OTAClientWriteImageCallback */
  uint8_t       cLengthToWrite = cLength;
  uint8_t       cLengthResidual = 0;

  /* Display Transfer Progress */
  LOG_INFO_APP("[OTA] FUOTA Transfer current offset 0x%08" PRIX32, stOTAClientGlobals.stBlockTransfer.lCurrentOffset);

  if ( stOTAClientGlobals.stBlockTransfer.lCurrentOffset == 0 )
  {
    /* A new transfer is starting */
    if ( stOTAClientGlobals.stBlockTransfer.stCurrentHeader.header_length == 0 )
    {
      /* Make a copy of the current header */
      LOG_INFO_APP( "[OTA] Making a copy of the current header" );
      memcpy( &stOTAClientGlobals.stBlockTransfer.stCurrentHeader, pstHeader, sizeof( stOTAClientGlobals.stBlockTransfer.stCurrentHeader ) );
    }
  }
  else
  {
    /* A transfer is resuming */
    if ( memcmp( &stOTAClientGlobals.stBlockTransfer.stCurrentHeader, pstHeader, sizeof( struct ZbZclOtaHeader ) ) != 0 )
    {
      /* The header is different from the current header */
      LOG_ERROR_APP( "[OTA] Error, the header is different from the current header." );
      return ZCL_STATUS_FAILURE;
    }
  }

  uint32_t lTime = HAL_GetTick();
  LOG_DEBUG_APP( "lTime = %u, cLength = %u", lTime, cLength );

  /* Copy the image data into the RAM buffer */
  if ( ( stOTAClientGlobals.stBlockTransfer.lFirmwareBufferCurrentOffset + cLength ) > RAM_FIRMWARE_BUFFER_SIZE )
  {
    /* The image data will not fit in the remaining RAM buffer */
    cLengthToWrite = ( uint8_t ) ( RAM_FIRMWARE_BUFFER_SIZE - stOTAClientGlobals.stBlockTransfer.lFirmwareBufferCurrentOffset );
    cLengthResidual = cLength - cLengthToWrite;
    stOTAClientGlobals.stBlockTransfer.bFirmwareBufferFull = true;
  }
  else if ( ( stOTAClientGlobals.stBlockTransfer.lFirmwareBufferCurrentOffset + cLength ) == RAM_FIRMWARE_BUFFER_SIZE )
  {
    /* The image data fits perfectly in the remaining RAM buffer */
    stOTAClientGlobals.stBlockTransfer.bFirmwareBufferFull = true;
  }
  memcpy( ( void * )( stOTAClientGlobals.stBlockTransfer.szFirmwareBuffer + stOTAClientGlobals.stBlockTransfer.lFirmwareBufferCurrentOffset ), pData, cLengthToWrite );
  stOTAClientGlobals.stBlockTransfer.lFirmwareBufferCurrentOffset += cLengthToWrite;

  if ( stOTAClientGlobals.stBlockTransfer.bFirmwareBufferFull == true )
  {
#ifdef APP_ZIGBEE_OTAClient_WRITE_TO_FLASH
    /* Copy the RAM buffer into flash */
    LOG_INFO_APP( "[OTA] Copying %u bytes to flash", stOTAClientGlobals.stBlockTransfer.lFirmwareBufferCurrentOffset );
    if ( APP_ZIGBEE_OTAClientWriteFirmwareData( stOTAClientGlobals.stBlockTransfer.lFlashCurrentAddr, stOTAClientGlobals.stBlockTransfer.szFirmwareBuffer, RAM_FIRMWARE_BUFFER_SIZE ) != APP_ZIGBEE_OK )
    {
      LOG_ERROR_APP("[OTA] Error, failed to write to flash.");
      return ZCL_STATUS_FAILURE;
    }
#else
    /* Clear the RAM buffer (fallthrough) */
    LOG_INFO_APP( "[OTA] Clearing %u bytes from the buffer", stOTAClientGlobals.stBlockTransfer.lFirmwareBufferCurrentOffset );
#endif /* APP_ZIGBEE_OTAClient_WRITE_TO_FLASH */

    /* Configure the global write info */
    /* Note: The RAM buffer length should be > 127 to prevent the residual from overflowing the buffer a second time here */
    memset( stOTAClientGlobals.stBlockTransfer.szFirmwareBuffer, 0xFF, sizeof(stOTAClientGlobals.stBlockTransfer.szFirmwareBuffer) );
    memcpy( stOTAClientGlobals.stBlockTransfer.szFirmwareBuffer, pData + cLengthToWrite, cLengthResidual );
    stOTAClientGlobals.stBlockTransfer.lFirmwareBufferCurrentOffset = cLengthResidual;
    stOTAClientGlobals.stBlockTransfer.bFirmwareBufferFull = false;
    stOTAClientGlobals.stBlockTransfer.lFlashCurrentAddr += RAM_FIRMWARE_BUFFER_SIZE;
  }

  stOTAClientGlobals.stBlockTransfer.lCurrentOffset += cLength;

  /* USER CODE END APP_ZIGBEE_OTAClientWriteImageCallback */
  return eStatus;
}

/**
 * @brief  OTA Client 'ImageValidate' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_OTAClientImageValidateCallback( struct ZbZclClusterT * pstCluster, struct ZbZclOtaHeader * pstHeader, void * arg )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_OTAClientImageValidateCallback */
  uint32_t              lFirmwareBufferRemainingSize;
#ifdef APP_ZIGBEE_OTAClient_WRITE_TO_FLASH
#ifdef APP_ZIGBEE_OTAClient_DELETE_SECTORS_IF_INVALID
  const uint32_t        lFlashEndAddress = FLASH_BASE + FLASH_SIZE;
  const uint32_t        lMaximumImageSize = lFlashEndAddress - stOTAClientInfo.lBaseAddress;
#endif /* APP_ZIGBEE_OTAClient_DELETE_SECTORS_IF_INVALID */
  uint32_t              lBinarySize = 0u;
#endif /* APP_ZIGBEE_OTAClient_WRITE_TO_FLASH */
  double                lDownloadElaspedTime;
  double                lDownloadThroughput;

  lFirmwareBufferRemainingSize = stOTAClientGlobals.stBlockTransfer.lFirmwareBufferCurrentOffset;
  if ( lFirmwareBufferRemainingSize != 0u )
  {
#ifdef APP_ZIGBEE_OTAClient_WRITE_TO_FLASH
    /* Copy the remaining RAM buffer into flash */
    LOG_INFO_APP( "[OTA] Copying %u bytes to flash", lFirmwareBufferRemainingSize );
    if ( APP_ZIGBEE_OTAClientWriteFirmwareData( stOTAClientGlobals.stBlockTransfer.lFlashCurrentAddr, stOTAClientGlobals.stBlockTransfer.szFirmwareBuffer, lFirmwareBufferRemainingSize ) != APP_ZIGBEE_OK )
    {
      LOG_INFO_APP("[OTA] Error, failed to write to flash.");
      return ZCL_STATUS_FAILURE;
    }
#else
    /* Clear the RAM buffer (fallthrough) */
    LOG_INFO_APP( "[OTA] Clearing %u bytes from the buffer", stOTAClientGlobals.stBlockTransfer.lFirmwareBufferCurrentOffset );
#endif /* APP_ZIGBEE_OTAClient_WRITE_TO_FLASH */
  }

  /* Configure the global write info */
  memset( stOTAClientGlobals.stBlockTransfer.szFirmwareBuffer, 0xFF, sizeof(stOTAClientGlobals.stBlockTransfer.szFirmwareBuffer) );
  stOTAClientGlobals.stBlockTransfer.lFirmwareBufferCurrentOffset = 0u;
  stOTAClientGlobals.stBlockTransfer.lFlashCurrentAddr += lFirmwareBufferRemainingSize;

#ifdef APP_ZIGBEE_OTAClient_WRITE_TO_FLASH
  /* Get the binary info*/
  LOG_INFO_APP( "[OTA] Getting the binary info" );
  if ( APP_ZIGBEE_OTAGetBinInfo( stOTAClientInfo.lBaseAddress, stOTAClientInfo.lOTATagAddress, stOTAClientInfo.lMagicKeyword, &lBinarySize ) == false )
  {
    LOG_ERROR_APP( "[OTA] Error, binary info invalid." );
    eStatus = ZCL_STATUS_INVALID_IMAGE;
  }

  /* Verify the binary length is correct */
  /* Note: This is an additional check */
  LOG_INFO_APP( "[OTA] Verifying the binary length is correct" );
  if ( stOTAClientGlobals.stBlockTransfer.lFlashCurrentAddr - stOTAClientInfo.lBaseAddress != lBinarySize )
  {
    LOG_ERROR_APP( "[OTA] Error, binary length is incorrect." );
    eStatus = ZCL_STATUS_INVALID_IMAGE;
  }

  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
#ifdef APP_ZIGBEE_OTAClient_DELETE_SECTORS_IF_INVALID
    /* Set the binary size to the maximum size */
    lBinarySize = lMaximumImageSize;

    /* Erase the sectors used to download the image */
    LOG_INFO_APP( "[OTA] Deleting flash sectors from 0x%08" PRIX32 " to 0x%08" PRIX32, stOTAClientInfo.lBaseAddress, lBinarySize );
    APP_ZIGBEE_DeleteSectors( stOTAClientInfo.lBaseAddress, lBinarySize );
#endif /* APP_ZIGBEE_OTAClient_DELETE_SECTORS_IF_INVALID */

    return eStatus;
  }

  LOG_INFO_APP( "[OTA] The downloaded firmware is valid" );

  /* Set the new file version */
  /* Note: It doesn't matter if this is line is called because the device will undergo a soft reset */
  stOTAClientInfo.lCurrentFileVersion = pstHeader->file_version;
#endif /* APP_ZIGBEE_OTAClient_WRITE_TO_FLASH */

  /* Record the end time */
  stOTAClientGlobals.lDownloadEndTime = HAL_GetTick();
  LOG_DEBUG_APP( "lDownloadEndTime = %u", stOTAClientGlobals.lDownloadEndTime );

  /* Calculate the performance */
  lDownloadElaspedTime = ( double )( stOTAClientGlobals.lDownloadEndTime - stOTAClientGlobals.lDownloadStartTime ) / 1000.f;
  lDownloadThroughput = ( double )( lBinarySize ) / ( double )( lDownloadElaspedTime ) * 8.f / 1000.f;
  LOG_INFO_APP( "[OTA] Image download timing data:" );
  LOG_INFO_APP( "[OTA] Download start time (ms):         %u", stOTAClientGlobals.lDownloadStartTime );
  LOG_INFO_APP( "[OTA] Download end time (ms):           %u", stOTAClientGlobals.lDownloadEndTime );
  LOG_INFO_APP( "[OTA] Download elapsed time (s):        %.2f", lDownloadElaspedTime );
  LOG_INFO_APP( "[OTA] Download throughput (kbit/s):     %.2f", lDownloadThroughput );

  /* Reset global cached information */
  APP_ZIGBEE_OTAClientResetGlobals();

  /* Stop the LED toggling */
  UTIL_TIMER_Stop( &stTimerLedToggle );
  APP_LED_OFF( LED_GREEN );

  /* USER CODE END APP_ZIGBEE_OTAClientImageValidateCallback */
  return eStatus;
}

/**
 * @brief  OTA Client 'Reboot' command Callback
 */
static void APP_ZIGBEE_OTAClientRebootCallback( struct ZbZclClusterT * pstCluster, void * arg )
{

  /* USER CODE BEGIN APP_ZIGBEE_OTAClientRebootCallback */
  LOG_INFO_APP( "**********************************************************" );
  LOG_INFO_APP( "[OTA] Rebooting." );
  LOG_INFO_APP( "**********************************************************" );

  /* Start a reboot */
  HAL_Delay( 1000u ); /* Wait for the log to flush */
  * ( uint32_t * ) SRAM1_BASE = CFG_REBOOT_ON_OTA_FW;
  HAL_Delay( 1000u ); /* Wait for the write to complete */
  HAL_Delay( 1000u ); /* Wait for the write to complete */
  NVIC_SystemReset();

  /* USER CODE END APP_ZIGBEE_OTAClientRebootCallback */

}

/**
 * @brief  OTA Client 'AbortDownload' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_OTAClientAbortDownloadCallback( struct ZbZclClusterT * pstCluster, enum ZbZclOtaCommandId eCommandId, void * arg )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_OTAClientAbortDownloadCallback */
  LOG_INFO_APP("[OTA] Aborting download with associated OTA command ID 0x%02" PRIX8, eCommandId);

  /* Reset global cached information */
  APP_ZIGBEE_OTAClientResetGlobals();

  /* Stop the LED toggling */
  UTIL_TIMER_Stop( &stTimerLedToggle );
  APP_LED_OFF( LED_GREEN );

  /* USER CODE END APP_ZIGBEE_OTAClientAbortDownloadCallback */
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
  /* Timer associated to GREEN LED toggling */
  UTIL_TIMER_Create( &stTimerLedToggle, APP_ZIGBEE_LED_TOGGLE_MS, UTIL_TIMER_PERIODIC, APP_ZIGBEE_LEDToggle, NULL);

  /* Tasks associated to OTA upgrade process */
  UTIL_SEQ_RegTask( TASK_ZIGBEE_OTA_REQUEST_UPGRADE, UTIL_SEQ_RFU, APP_ZIGBEE_OTAClientRequestUpgrade );
  UTIL_SEQ_RegTask( TASK_ZIGBEE_OTA_START_DOWNLOAD, UTIL_SEQ_RFU, APP_ZIGBEE_OTAClientStartDownload );
}

/**
 * @brief  Application timer server callback (led toggling)
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_LEDToggle( void * arg )
{
  APP_LED_TOGGLE( LED_GREEN );
}

/**
 * @brief  OTA client start download
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_OTAClientStartDownload( void )
{
#ifdef APP_ZIGBEE_OTAClient_WRITE_TO_FLASH
  const uint32_t                        lFlashEndAddress = FLASH_BASE + FLASH_SIZE;
  const uint32_t                        lMaximumImageSize = lFlashEndAddress - stOTAClientInfo.lBaseAddress;
  uint32_t                              lBinarySize;
#endif /* APP_ZIGBEE_OTAClient_WRITE_TO_FLASH */

  if( stOTAClientGlobals.bDownloadStarted == true )
  {
    LOG_ERROR_APP( "[OTA] Error, download already ongoing." );
    return;
  }

#ifdef APP_ZIGBEE_OTAClient_WRITE_TO_FLASH
  /* Get the binary info*/
  LOG_INFO_APP( "[OTA] Getting the binary info" );
  if ( APP_ZIGBEE_OTAGetBinInfo( stOTAClientInfo.lBaseAddress, stOTAClientInfo.lOTATagAddress, stOTAClientInfo.lMagicKeyword, &lBinarySize ) == false )
  {
    /* Set the binary size to the maximum size */
    lBinarySize = lMaximumImageSize;
  }

  /* Erase the sectors used to download the image */
  LOG_INFO_APP( "[OTA] Deleting flash sectors from 0x%08" PRIX32 " to 0x%08" PRIX32, stOTAClientInfo.lBaseAddress, lBinarySize );
  APP_ZIGBEE_DeleteSectors( stOTAClientInfo.lBaseAddress, lBinarySize );
#endif /* APP_ZIGBEE_OTAClient_WRITE_TO_FLASH */

  /* Reset global cached information */
  APP_ZIGBEE_OTAClientResetGlobals();

  /* Start the LED toggling */
  UTIL_TIMER_StartWithPeriod( &stTimerLedToggle, APP_ZIGBEE_LED_TOGGLE_MS );

  /* Record the start time */
  stOTAClientGlobals.bDownloadStarted = true;
  stOTAClientGlobals.lDownloadStartTime = HAL_GetTick();
  LOG_DEBUG_APP( "lDownloadStartTime = %u", stOTAClientGlobals.lDownloadStartTime );

  /* Start an OTA download */
  ZbZclOtaClientImageTransferStart( stZigbeeAppInfo.OTAClient );
}

#ifdef APP_ZIGBEE_OTAClient_WRITE_TO_FLASH

/**
 * @brief  OTA client writing firmware data from internal RAM cache to flash
 * @param  
 * @retval Application status code
 */
static inline APP_ZIGBEE_StatusTypeDef APP_ZIGBEE_OTAClientWriteFirmwareData( uint32_t lFlashAddress, uint8_t * pcFirmwareBuffer, uint32_t lFirmwareBufferSize )
{
  uint32_t                      lIndex;
  HAL_StatusTypeDef             eHalStatus;
  APP_ZIGBEE_StatusTypeDef      eStatus = APP_ZIGBEE_OK;

  for( lIndex = 0; lIndex < lFirmwareBufferSize; lIndex += HW_FLASH_WIDTH )
  {
    /* Write to Flash Memory */
    uint32_t    lWord1 = * ( ( uint32_t *)( &pcFirmwareBuffer[lIndex + 0 * sizeof( uint32_t )] ) );
    uint32_t    lWord2 = * ( ( uint32_t *)( &pcFirmwareBuffer[lIndex + 1 * sizeof( uint32_t )] ) );
    uint32_t    lWord3 = * ( ( uint32_t *)( &pcFirmwareBuffer[lIndex + 2 * sizeof( uint32_t )] ) );
    uint32_t    lWord4 = * ( ( uint32_t *)( &pcFirmwareBuffer[lIndex + 3 * sizeof( uint32_t )] ) );
    LOG_DEBUG_APP( "0x%08" PRIX32 " | 0x%08" PRIX32 " | 0x%08" PRIX32 " | 0x%08" PRIX32, lWord1, lWord2, lWord3, lWord4 );

    HAL_FLASH_Unlock();

    eHalStatus = HAL_FLASH_Program( FLASH_TYPEPROGRAM_QUADWORD, lFlashAddress + lIndex, ( uint32_t )&pcFirmwareBuffer[lIndex] );
    if ( eHalStatus != HAL_OK )
    {
      LOG_ERROR_APP( "[OTA] Error, HAL_FLASH_Program failed (0x%02" PRIX8 ").", eHalStatus );
      eStatus = APP_ZIGBEE_ERROR;
    }
    
    HAL_FLASH_Lock();

    /* Read back the value for verification */
    if ( memcmp( ( void * )( lFlashAddress + lIndex ), &pcFirmwareBuffer[lIndex], HW_FLASH_WIDTH ) != 0x00u )
    {
      LOG_ERROR_APP( "[OTA] Error, flash verifaction failed." );
      eStatus = APP_ZIGBEE_ERROR;
    }

    if (eStatus != APP_ZIGBEE_OK)
    {
      return eStatus;
    }
  }

  return eStatus;
}

#endif /* APP_ZIGBEE_OTAClient_WRITE_TO_FLASH */


/**
 * @brief  Reset global cached information
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_OTAClientResetGlobals( void )
{
  stOTAClientGlobals.bDownloadStarted = false;
  stOTAClientGlobals.lDownloadStartTime = 0u;
  stOTAClientGlobals.lDownloadEndTime = 0u;
  memset( &stOTAClientGlobals.stBlockTransfer.stCurrentHeader, 0, sizeof( stOTAClientGlobals.stBlockTransfer.stCurrentHeader ) );
  stOTAClientGlobals.stBlockTransfer.lCurrentOffset = 0u;
  memset( &stOTAClientGlobals.stBlockTransfer.szFirmwareBuffer, 0xFF, sizeof( stOTAClientGlobals.stBlockTransfer.szFirmwareBuffer ) );
  stOTAClientGlobals.stBlockTransfer.lFirmwareBufferCurrentOffset = 0u;
  stOTAClientGlobals.stBlockTransfer.bFirmwareBufferFull = false;
  stOTAClientGlobals.stBlockTransfer.lFlashCurrentAddr = stOTAClientInfo.lBaseAddress;
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

  LOG_ERROR_APP( "[OTA] Sanity check before starting the OTA in case a previous OTA took place");
  LOG_ERROR_APP( "[OTA] Whatever the value of the MagicKeywordAddress another OTA can be started. Don't take into account next Warning line, if any");

  /* Check if the magic keyword address is inside the flash region */
  if ( lMagicKeywordAddress < lBaseAddress || lMagicKeywordAddress > lFlashEndAddress - sizeof( lBinaryMagicKeyword ) )
  {
    LOG_ERROR_APP( "[OTA] Warning, keyword address is outside the flash region (0x%08" PRIX32 ").", lMagicKeywordAddress );
    return false;
  }

  /* Check if the magic keyword is correct */
  lBinaryMagicKeyword = * ( uint32_t * )( lMagicKeywordAddress );
  if ( lBinaryMagicKeyword != lMagicKeyword )
  {
    LOG_ERROR_APP( "[OTA] Warning, keyword is incorrect (0x%08" PRIX32 " != 0x%08" PRIX32 ").", lBinaryMagicKeyword, lMagicKeyword );
    return false;
  }

  lBinaryEndAddress = lMagicKeywordAddress + sizeof( lMagicKeyword );
  *plSize = lBinaryEndAddress - lBaseAddress;

  LOG_DEBUG_APP( "*plSize = 0x%08" PRIX32, * plSize );

  return true;
}

/**
 * @brief  Deleting sectors helper
 * @param  None
 * @retval None
 */
static inline void APP_ZIGBEE_DeleteSectors( uint32_t lBaseAddress, uint32_t lBinarySize )
{
  FLASH_EraseInitTypeDef        stEraseInit;
  uint32_t                      lPageError;

  /* Determine the page, the number of pages to delete, etc. */
  stEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
  stEraseInit.Page = ( lBaseAddress - FLASH_BASE ) / FLASH_PAGE_SIZE;
  stEraseInit.NbPages = lBinarySize / FLASH_PAGE_SIZE;
  if ( lBinarySize % FLASH_PAGE_SIZE != 0 )
  {
    stEraseInit.NbPages++;
  }

  LOG_DEBUG_APP( "stEraseInit" );
  LOG_DEBUG_APP( ".TypeErase     = 0x%08" PRIX32, stEraseInit.TypeErase );
  LOG_DEBUG_APP( ".Page          = 0x%08" PRIX32, stEraseInit.Page );
  LOG_DEBUG_APP( ".NbPages       = %u", stEraseInit.NbPages );

  /* Delete the flash sectors */
  HAL_FLASH_Unlock();
  HAL_FLASHEx_Erase(&stEraseInit, &lPageError);
  HAL_FLASH_Lock();
}

/**
 * @brief  Start the OTA Client.
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_OTAClientStart(void)
{
  enum ZclStatusCodeT   eStatus;
  struct ZbApsAddrT     stDestination;

  LOG_INFO_APP( "[OTA] Searching for server" );

  /* Setting up the addressing mode */
  memset( &stDestination, 0, sizeof(stDestination) );
  stDestination.mode = ZB_APSDE_ADDRMODE_SHORT;
  stDestination.endpoint = APP_ZIGBEE_ENDPOINT;
  stDestination.nwkAddr = 0x0;

  /* Sending Discovery request to server */
  eStatus = ZbZclOtaClientDiscover( stZigbeeAppInfo.OTAClient, &stDestination );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "[OTA] Error, ZbZclOTAClientDiscover failed (0x%02" PRIX8 ").", eStatus );
  }

  UTIL_SEQ_WaitEvt( EVENT_ZIGBEE_OTA_SERVER_FOUND );
}

/**
 * @brief  OTA client request upgrade
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_OTAClientRequestUpgrade( void )
{
  struct ZbZclOtaImageDefinition        stImageDefinition;
  enum ZclStatusCodeT                   eStatus;

  /* Image definition configuration */
  stImageDefinition.manufacturer_code = stOTAClientInfo.iManufacturerCode;
  stImageDefinition.image_type = stOTAClientInfo.iImageType;
  stImageDefinition.file_version = stOTAClientInfo.lCurrentFileVersion;

  /* Sending Query Next Image Request to server */
  /* Note: HW version is provided as additional info in Query Next Image Request */
  LOG_INFO_APP( "[OTA] Contents of the Query Next Image Request:" );
  LOG_INFO_APP( "[OTA] Manufacturer code:                0x%08" PRIX32, stImageDefinition.manufacturer_code );
  LOG_INFO_APP( "[OTA] Image type:                       0x%04" PRIX16, stImageDefinition.image_type );
  LOG_INFO_APP( "[OTA] Current file version:             0x%08" PRIX32, stImageDefinition.file_version );
  LOG_INFO_APP( "[OTA] Sending Query Next Image Request" );
  eStatus = ZbZclOtaClientQueryNextImageReq( stZigbeeAppInfo.OTAClient, &stImageDefinition, ZCL_OTA_QUERY_FIELD_CONTROL_HW_VERSION, CURRENT_M33_HARDWARE_VERSION );
  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_ERROR_APP( "[OTA] Error, Query Next Image Request failed (0x%02" PRIX8 ").", eStatus );
  }
}

/**
 * @brief  Management of the SW1 button : Send Query Next Image Request
 * @param  None
 * @retval None
 */
void APPE_Button1Action(void)
{
  /* First, verify if Appli has already Join a Network  */ 
  if ( APP_ZIGBEE_IsAppliJoinNetwork() != false )
  {
    LOG_INFO_APP( "[OTA] SW1 pushed, Requesting an upgrade" );
    UTIL_SEQ_SetTask( TASK_ZIGBEE_OTA_REQUEST_UPGRADE, TASK_ZIGBEE_OTA_REQUEST_UPGRADE_PRIORITY );
  }
}

/**
 * @brief  Management of the SW2 button : Reboot and run OTA Application
 * @param  None
 * @retval None
 */
void APPE_Button2Action(void)
{
  uint32_t      lBinarySize;

  /* Get the binary info*/
  LOG_INFO_APP( "[OTA] Getting the binary info" );
  if ( APP_ZIGBEE_OTAGetBinInfo( stOTAClientInfo.lBaseAddress, stOTAClientInfo.lOTATagAddress, stOTAClientInfo.lMagicKeyword, &lBinarySize ) == false )
  {
    LOG_ERROR_APP( "[OTA] Error, binary info invalid." );
    return;
  }

  LOG_INFO_APP( "[OTA] The binary is valid" );

  APP_ZIGBEE_OTAClientRebootCallback(NULL, NULL);
}

/*************************************************************
 *
 * REBOOT MANAGEMENT FUNCTIONS
 *
 *************************************************************/

static void JumpFwApp( void )
{
  ReBootFunction_t      fApplicationResetHandler;

  SCB->VTOR = FUOTA_APP_FW_BINARY_ADDRESS;
  __set_MSP( * ( uint32_t * )FUOTA_APP_FW_BINARY_ADDRESS );
  fApplicationResetHandler = ( ReBootFunction_t )( * (uint32_t *)( FUOTA_APP_FW_BINARY_ADDRESS + 4u ) );
  fApplicationResetHandler();

  /**
   * fApplicationResetHandler() never returns.
   * However, if for any reason a PUSH instruction is added at the entry of JumpFwApp(),
   * we need to make sure the POP instruction is not there before fApplicationResetHandler() is called
   * The way to ensure this is to add a dummy code after fApplicationResetHandler() is called
   * This prevents fApplicationResetHandler() to be the last code in the function.
   */
  __WFI();

  return;
}

void BootModeCheck( void )
{
  if ( LL_RCC_IsActiveFlag_SFTRST( ) || LL_RCC_IsActiveFlag_OBLRST( ) )
  {
    /* Check SRAM1 for the boot mode */
    if ( ( * ( uint32_t * )SRAM1_BASE ) == CFG_REBOOT_ON_OTA_FW )
    {
      JumpFwApp();
    }
  }

  return;
}

/* USER CODE END FD_LOCAL_FUNCTIONS */


