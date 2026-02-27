/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_zigbee_ota_client.c
  * Description        : Application to manage OTA Client Cluster.
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
#include "app_zigbee.h"
#include "app_zigbee_ota_defines.h"

#include "stm32_rtos.h"
#include "stm32_timer.h"
#include "log_module.h"

#include "zigbee.h"
#include "zigbee.nwk.h"
#include "zigbee.security.h"

/* Private includes -----------------------------------------------------------*/
#include "zcl/zcl.h"
#include "zcl/general/zcl.ota.h"

/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_LED_TOGGLE_MS            1000u

#define HW_FLASH_WIDTH                      16u   // Flash Write by 4 words / 16 bytes.

#define CFG_TASK_PRIO_OTA_REQUEST_UPGRADE   CFG_SEQ_PRIO_1
#define CFG_TASK_PRIO_OTA_START_DOWNLOAD    CFG_SEQ_PRIO_1

/* Private typedef -----------------------------------------------------------*/
/* OTA client block transfer cached information */
typedef struct
{
  /* Transmission related fields */
  struct ZbZclOtaHeader stCurrentHeader;      /* The ZB image header of the current ongoing transmission */
  uint32_t              lCurrentOffset;       /* The current offset in the ongoing transmission image data */
  uint32_t              lDownloadStartTime;   /* The download start time for the ongoing transmission */
  uint32_t              lDownloadEndTime;     /* The download end time for the ongoing transmission */

  /* RAM buffer related fields */
  uint8_t 	            szFirmwareBuffer[FUOTA_RAM_BUFFER_SIZE];  /* The RAM buffer for temporary image data storage before transferring to flash */
  uint32_t 	            lFirmwareBufferCurrentOffset;             /* The current offset in the RAM buffer (should wrap when reaching the size) */
  bool                  bFirmwareBufferFull;  /* True if the RAM buffer is full and needs to be transferred to flash */

  /* Flash related fields */
  uint32_t 	            lFlashCurrentAddr;    /* The current address in flash */
} AppZbOta_ClientBlockTransfer_st;

/* Reboot Function definition */
typedef void ( * ReBootFunction_t ) ( void );

/* ImageValidate Function definition */
typedef enum ZclStatusCodeT (* ImageValidate_t )(struct ZbZclClusterT *cluster, struct ZbZclOtaHeader *header, void *arg);
typedef enum ZclStatusCodeT (* UpgradeEnd_t)(struct ZbZclClusterT *cluster, struct ZbZclOtaHeader *header, uint32_t current_time, uint32_t upgrade_time, void *arg);

/* Private constants ---------------------------------------------------------*/

/* Public variables ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static bool                             bOtaServerAvailable;
static struct ZbZclClusterT             * pOtaClient;
static AppZbOta_ClientInfo_st           stCurrentOtaClientInfo;
static AppZbOta_ClientBlockTransfer_st  stOtaClientBlockTransfer;
static UTIL_TIMER_Object_t              stTimerRequestUpgradeWaitId, stTimerStartDownloadWaitId, stTimerUpgradeEndRequestRetryId;
static struct ZbHash                    stOtaImageHash;

//static ImageValidate_t                  fZbStackImageValidateCallback = NULL;
static UpgradeEnd_t                     fZbStackUpgradeEndCallback = NULL;

/* Private function prototypes -----------------------------------------------*/

/* ZCL OTA cluster related functions */
static void                     AppZbOtaClient_DiscoverCompleteCallback   ( struct ZbZclClusterT * clusterPtr, enum ZclStatusCodeT status,void * arg );
static enum ZclStatusCodeT      AppZbOtaClient_ImageNotifyCallback        ( struct ZbZclClusterT * pstCluster, uint8_t cPayloadType, uint8_t cJitter, struct ZbZclOtaImageDefinition * pstImageDefinition, struct ZbApsdeDataIndT * pstDataInd, struct ZbZclHeaderT * pstZclHeader );
static void                     AppZbOtaClient_QueryNextImageCallback     ( struct ZbZclClusterT * pstCluster, enum ZclStatusCodeT eStatus, struct ZbZclOtaImageDefinition * pstImageDefinition, uint32_t lImageSize, void * arg );
static enum ZclStatusCodeT      AppZbOtaClient_WriteImageCallback         ( struct ZbZclClusterT * pstCluster, struct ZbZclOtaHeader * pstHeader, uint8_t cLength, uint8_t * pData, void * arg );
static enum ZclStatusCodeT      AppZbOtaClient_UpgradeEndCallback         ( struct ZbZclClusterT * pstCluster, struct ZbZclOtaHeader * pstHeader, uint32_t lCurrentTime, uint32_t lUpgradeTime, void * arg );
static enum ZclStatusCodeT      AppZbOtaClient_RawDataCallback            ( struct ZbZclClusterT * pstCluster, uint8_t length,uint8_t *data, void *arg );
static enum ZclStatusCodeT      AppZbOtaClient_ImageValidateCallback      ( struct ZbZclClusterT * pstCluster, struct ZbZclOtaHeader * pstHeader, void * arg );
static void                     AppZbOtaClient_RebootCallback             ( struct ZbZclClusterT * pstCluster, void * arg );
static enum ZclStatusCodeT      AppZbOtaClient_AbortDownloadCallback      ( struct ZbZclClusterT * pstCluster, enum ZbZclOtaCommandId eCommandId, void * arg );

static void AppZbOtaClient_StartDownload                 ( void );
static void AppZbOtaClient_RequestUpgrade                ( void );
static void AppZbOtaClient_TimeoutForRequestUpgrade      ( void * arg );
static void AppZbOtaClient_TimeoutForStartDownload       ( void * arg );
static void AppZbOtaClient_TimeoutForUpgradeEndResponse  ( void * arg );

/* Functions Definition ------------------------------------------------------*/

/**
 * @brief  OTA Endpoint Initialisation
 * @return
 */
static bool AppZbOtaClient_AddEndpoint( struct ZigBeeT * pstZigbee )
{
  struct ZbApsmeAddEndpointReqT   stRequest;
  struct ZbApsmeAddEndpointConfT  stConfig;

  /* Verify if Endpoint already exist */
  if ( ZbApsEndpointExists( pstZigbee, stCurrentOtaClientInfo.cEndpoint ) != false )
  {
     LOG_INFO_APP( "OTA Endpoint %d already exist.", stCurrentOtaClientInfo.cEndpoint );
  }
  else
  {
    /* Add EndPoint */
    LOG_INFO_APP( "Creating OTA Endpoint %d ...", stCurrentOtaClientInfo.cEndpoint);

    (void)memset( &stRequest, 0, sizeof( stRequest ) );
    (void)memset( &stConfig, 0, sizeof( stConfig ) );

    stRequest.endpoint = stCurrentOtaClientInfo.cEndpoint;
    stRequest.profileId = stCurrentOtaClientInfo.iProfile;
    stRequest.deviceId = stCurrentOtaClientInfo.iDeviceId;

    ZbZclAddEndpoint( pstZigbee, &stRequest, &stConfig );
    if ( stConfig.status != ZB_STATUS_SUCCESS)
    {
      LOG_ERROR_APP( "Error, ZbZclAddEndpoint failed." );
      return false;
    }
  }

  return true;
}

/**
 * @brief  Add OTA Client Cluster
 * @param  None
 * @retval None
 */
static bool AppZbOtaClient_AddCluster( struct ZigBeeT * pstZigbee )
{
  struct ZbZclOtaClientConfig stOtaClientConfig ;

  /* Add OTA Client Cluster */
  LOG_INFO_APP( "Creating OTA Client Cluster ..." );
  (void)memset( &stOtaClientConfig, 0, sizeof( stOtaClientConfig ) );

  stOtaClientConfig.endpoint = stCurrentOtaClientInfo.cEndpoint;
  stOtaClientConfig.profile_id = stCurrentOtaClientInfo.iProfile;
  stOtaClientConfig.activation_policy = stCurrentOtaClientInfo.eActivationPolicy;
  stOtaClientConfig.timeout_policy = stCurrentOtaClientInfo.eTimeOutPolicy;

  /* Set Default Callback */
  ZbZclOtaClientGetDefaultCallbacks( &stOtaClientConfig.callbacks );

  /* Set Specifics Callback */
  stOtaClientConfig.callbacks.discover_complete = AppZbOtaClient_DiscoverCompleteCallback;
  stOtaClientConfig.callbacks.image_notify = AppZbOtaClient_ImageNotifyCallback;
  stOtaClientConfig.callbacks.query_next = AppZbOtaClient_QueryNextImageCallback;
  stOtaClientConfig.callbacks.write_image = AppZbOtaClient_WriteImageCallback;
  stOtaClientConfig.callbacks.reboot = AppZbOtaClient_RebootCallback;
  stOtaClientConfig.callbacks.abort_download = AppZbOtaClient_AbortDownloadCallback;

  fZbStackUpgradeEndCallback = stOtaClientConfig.callbacks.upgrade_end;
  stOtaClientConfig.callbacks.upgrade_end = AppZbOtaClient_UpgradeEndCallback;

  if ( stCurrentOtaClientInfo.pCertPublicKey != NULL )
  {
    /* Setup CA pub key + len to have an ECDSA verification */
    stOtaClientConfig.ca_pub_key_array = stCurrentOtaClientInfo.pCertPublicKey;
    stOtaClientConfig.ca_pub_key_len = stCurrentOtaClientInfo.iCertPublicSize;
  }
  else
  {
    stOtaClientConfig.callbacks.image_validate = AppZbOtaClient_ImageValidateCallback;
    stOtaClientConfig.callbacks.update_raw = AppZbOtaClient_RawDataCallback;
  }

  /* OTA Client */
  pOtaClient = ZbZclOtaClientAlloc( pstZigbee, &stOtaClientConfig, NULL );
  if ( pOtaClient == NULL )
  {
    LOG_ERROR_APP( "  Error, ZbZclOtaClientAlloc failed." );
    return false;
  }

  if ( stCurrentOtaClientInfo.iProfile == (uint16_t)ZCL_PROFILE_SMART_ENERGY )
  {
    /* Set the Smart Energy Profile Id (should be redundant for this SE cluster) */
    ZbZclClusterSetProfileId( pOtaClient, stCurrentOtaClientInfo.iProfile );

    /* Set minimum security level */
    (void)ZbZclClusterSetMinSecurity( pOtaClient, ZB_APS_STATUS_SECURED_LINK_KEY );
  }

  if ( ZbZclClusterEndpointRegister( pOtaClient ) == false )
  {
    LOG_ERROR_APP( "  Error, ZbZclClusterEndpointRegister failed." );
    return false;
  }
  else
  {
    if ( stCurrentOtaClientInfo.pCertPublicKey != NULL )
    {
      /* Init Variables for Validation */
      ZbHashInit( &stOtaImageHash );
      (void)memset( &stOtaImageHash, 0, AES_BLOCK_SIZE );
    }

    /* Init also needed Attributes */
    (void)ZbZclAttrIntegerWrite( pOtaClient, ZCL_OTA_ATTR_MANUFACTURER_ID, stCurrentOtaClientInfo.iManufacturerCode );
    (void)ZbZclAttrIntegerWrite( pOtaClient, ZCL_OTA_ATTR_IMAGE_TYPE_ID, stCurrentOtaClientInfo.iImageType );
  }

  return true;
}

/**
 * @brief  OTA Cluster Initialisation
 * @param  None
 * @retval None
 */
bool AppZbOtaClient_ClusterInit( struct ZigBeeT * pstZigbee, const AppZbOta_ClientInfo_st * pstClusterUserInfo )
{
  bool  bReturn;

  /* Add FW Information. Need to have a specific functions ? */
  memcpy( &stCurrentOtaClientInfo, pstClusterUserInfo, sizeof(stCurrentOtaClientInfo) );

  /* Configure OTA Client Endpoint */
  bReturn = AppZbOtaClient_AddEndpoint( pstZigbee );

  /* Configure OTA Client Clusters */
  if ( bReturn != false )
  {
    bReturn = AppZbOtaClient_AddCluster( pstZigbee );
  }

  if ( bReturn != false )
  {
    /* Tasks associated to OTA upgrade process */
    UTIL_SEQ_RegTask( TASK_ZIGBEE_OTA_REQUEST_UPGRADE, UTIL_SEQ_RFU, AppZbOtaClient_RequestUpgrade );
    UTIL_SEQ_RegTask( TASK_ZIGBEE_OTA_START_DOWNLOAD, UTIL_SEQ_RFU, AppZbOtaClient_StartDownload );

    /* Timer associated to OTA upgrade process */
    UTIL_TIMER_Create( &stTimerRequestUpgradeWaitId, 0, UTIL_TIMER_ONESHOT, AppZbOtaClient_TimeoutForRequestUpgrade, NULL );
    UTIL_TIMER_Create( &stTimerStartDownloadWaitId, 0, UTIL_TIMER_ONESHOT, AppZbOtaClient_TimeoutForStartDownload, NULL );

    UTIL_TIMER_Create( &stTimerUpgradeEndRequestRetryId, 0, UTIL_TIMER_ONESHOT, AppZbOtaClient_TimeoutForUpgradeEndResponse, NULL );
  }

  return bReturn;
}

/**
 * @brief  OTA Cluster Free
 */
void AppZbOtaClient_ClusterRemove( void )
{
    LOG_INFO_APP( "Remove OTA Client\n");
    ZbZclClusterFree( pOtaClient );
}

/**
 *
 * @param pTimer
 * @param arg
 */
static void AppZbOtaClient_TimeoutForRequestUpgrade( void * arg )
{
  LOG_INFO_APP( "TimeOut of pstTimerRequestUpgradeWaitId.\n" );
  UTIL_TIMER_Stop( &stTimerRequestUpgradeWaitId );

  UTIL_SEQ_SetTask( TASK_ZIGBEE_OTA_REQUEST_UPGRADE, CFG_TASK_PRIO_OTA_REQUEST_UPGRADE );
}

/**
 *
 * @param pTimer
 * @param arg
 */
static void AppZbOtaClient_TimeoutForStartDownload( void * arg )
{
  LOG_INFO_APP( "TimeOut of pstTimerStartDownloadWaitId." );
  UTIL_TIMER_Stop( &stTimerStartDownloadWaitId );

  UTIL_SEQ_SetTask( TASK_ZIGBEE_OTA_START_DOWNLOAD, CFG_TASK_PRIO_OTA_START_DOWNLOAD );
}

/**
 *
 * @param pTimer
 * @param arg
 */
static void AppZbOtaClient_TimeoutForUpgradeEndResponse( void * arg )
{
  LOG_INFO_APP( "Retrying upgrade end request ....." );

  ZbZclOtaClientImageTransferResume(pOtaClient);
}

/**
 * @brief  OTA client Image Notify callback
 * @param  pstCluster: ZCL Cluster pointer
 * @param  cPayloadType: Payload type
 * @param  cJitter: Upgrade jitter given to the client
 * @param  pstImageDefinition: Image query definition pointer
 * @param  pstDataInd: APS layer packet info
 * @param  pstZclHeader: ZCL header
 * @retval ZCL status code
 */
static enum ZclStatusCodeT AppZbOtaClient_ImageNotifyCallback( struct ZbZclClusterT * pstCluster, uint8_t cPayloadType, uint8_t cJitter,
                                                               struct ZbZclOtaImageDefinition * pstImageDefinition, struct ZbApsdeDataIndT * pstDataInd,
                                                               struct ZbZclHeaderT * pstZclHeader )
{
  LOG_INFO_APP( "[OTA] Received a Image Notify Request..." );

  /* Request an upgrade if the manufacturer code, image type, and file version are okay */
  switch( (enum ZbZclOtaImageNotifyCmd)cPayloadType )
  {
    case ZCL_OTA_NOTIFY_TYPE_FILE_VERSION:
      LOG_INFO_APP( "[OTA] Contents of the Image Notify Request:" );
      LOG_INFO_APP( "  Jitter            : %d", cJitter );
      LOG_INFO_APP( "  Manufacturer Code : 0x%04X", pstImageDefinition->manufacturer_code );
      LOG_INFO_APP( "  Image type        : 0x%04X", pstImageDefinition->image_type );
      LOG_INFO_APP( "  File version      : 0x%08X", pstImageDefinition->file_version);

      if ( ( pstImageDefinition->manufacturer_code == stCurrentOtaClientInfo.iManufacturerCode )
        && ( pstImageDefinition->image_type == stCurrentOtaClientInfo.iImageType )
        && ( pstImageDefinition->file_version >= stCurrentOtaClientInfo.lCurrentFileVersion ) )
      {
        LOG_INFO_APP( "[OTA] Requesting an upgrade ..." );

        /* Not Launch immediately the Task, wait the end of 'Notify Callback' answer send */
        UTIL_TIMER_StartWithPeriod( &stTimerRequestUpgradeWaitId, 50 );
      }
      else
      {
        LOG_INFO_APP( "[OTA] Error, notified Image is not for this chip." );
        return ZCL_STATUS_FAILURE;
      }
      break;

    default:
      LOG_INFO_APP( "[OTA] Error, unsupported payload type (0x%02X).", cPayloadType );
      return ZCL_STATUS_FAILURE;
  }

  return ZCL_STATUS_SUCCESS;
}

/**
 * @brief  OTA client start download
 * @param  None
 * @retval None
 */
static void AppZbOtaClient_StartDownload( void )
{
  /* Record the start time */
  stOtaClientBlockTransfer.lDownloadStartTime = HAL_GetTick();

  /* Start an OTA download */
  (void)ZbZclOtaClientImageTransferStart( pOtaClient );
}

/**
 * @brief  OTA client Query Next Image callback
 * @param  pstCluster: ZCL Cluster pointer
 * @param  eStatus: ZCL status code returned by the server for the requested image
 * @param  pstImageDefinition: Image query definition pointer
 * @param  lImageSize: OTA file format image total size
 * @param  arg: Passed arg
 * @retval ZCL status code
 */
static void AppZbOtaClient_QueryNextImageCallback( struct ZbZclClusterT * pstCluster, enum ZclStatusCodeT eStatus,
                                                         struct ZbZclOtaImageDefinition * pstImageDefinition, uint32_t lImageSize, void * arg )
{
  uint32_t    lMaximumImageSize;

  if ( eStatus != ZCL_STATUS_SUCCESS )
  {
    LOG_INFO_APP( "[OTA] Error, Query Next Image Response failed (0x%02).", eStatus );
    return;
  }

  /* Verify if Flash size to save Image is OK */
  lMaximumImageSize = ( FLASH_BASE + FLASH_SIZE ) - stCurrentOtaClientInfo.lBaseAddress;
  if ( lImageSize > lMaximumImageSize )
  {
    LOG_INFO_APP( "[OTA] Error, image will not fit in flash (%u/%u bytes).", lImageSize, lMaximumImageSize );
    return;
  }

  LOG_INFO_APP( "[OTA] Starting an OTA download (%d bytes)...", lImageSize );

  /* Not Launch immediately the Task, wait the end of 'NextImage Callback' answer send */
  UTIL_TIMER_StartWithPeriod( &stTimerStartDownloadWaitId, 50 );
}

/**
 * @brief  OTA client writing firmware data from internal RAM cache to flash
 * @param
 * @retval Application status code
 */
static inline bool AppZbOtaClient_WriteFirmwareData( uint32_t lFlashAddress, uint8_t * pcFirmwareBuffer, uint32_t lFirmwareBufferSize )
{
  uint32_t              lIndex;
  HAL_StatusTypeDef     eHalStatus;
  bool                  bStatus = true;

  for( lIndex = 0; lIndex < lFirmwareBufferSize; lIndex += HW_FLASH_WIDTH )
  {

    (void)HAL_FLASH_Unlock();

    eHalStatus = HAL_FLASH_Program( FLASH_TYPEPROGRAM_QUADWORD, lFlashAddress + lIndex, ( uint32_t )&pcFirmwareBuffer[lIndex] );
    if ( eHalStatus != HAL_OK )
    {
      LOG_INFO_APP( "[OTA] Error, HAL_FLASH_Program failed (0x%02X).", eHalStatus );
      bStatus = false;
    }

    (void)HAL_FLASH_Lock();

    /* Read back the value for verification */
    if ( memcmp( ( void * )( lFlashAddress + lIndex ), &pcFirmwareBuffer[lIndex], HW_FLASH_WIDTH ) != 0 )
    {
      LOG_INFO_APP( "[OTA] Error, flash verification failed." );
      bStatus = false;
    }

    if ( bStatus != true)
    {
      return bStatus;
    }
  }

  return bStatus;
}

/**
 * @brief  OTA client Write Image callback
 * @param  pstCluster: ZCL Cluster pointer
 * @param  pstHeader: ZCL OTA file format image header
 * @param  cLength: received chunk length
 * @param  data: received chunk payload
 * @param  arg: Passed arg
 * @retval ZCL status code
 */
static enum ZclStatusCodeT AppZbOtaClient_WriteImageCallback( struct ZbZclClusterT * pstCluster, struct ZbZclOtaHeader * pstHeader,
                                                               	    uint8_t cLength, uint8_t * pData, void * arg )
{
  uint8_t       cLengthToWrite = cLength;
  uint8_t       cLengthResidual = 0;

  /* Display Transfer Progress */
//  LOG_INFO_APP( "\x1b[A[OTA] Transfer current offset 0x%08l\n", stOtaClientBlockTransfer.lCurrentOffset );

  if ( stOtaClientBlockTransfer.lCurrentOffset == 0u )
  {
    /* A new transfer is starting */
    if ( stOtaClientBlockTransfer.stCurrentHeader.header_length == 0u )
    {
      /* Make a copy of the current header */
      LOG_INFO_APP( "[OTA] Making a copy of the current header\n\n" );
      (void)memcpy( &stOtaClientBlockTransfer.stCurrentHeader, pstHeader, sizeof( stOtaClientBlockTransfer.stCurrentHeader ) );
    }
  }
  else
  {
    /* A transfer is resuming */
    if ( memcmp( &stOtaClientBlockTransfer.stCurrentHeader, pstHeader, sizeof( struct ZbZclOtaHeader ) ) != 0 )
    {
      /* The header is different from the current header */
      LOG_INFO_APP( "[OTA] Error, the header is different from the current header." );
      return ZCL_STATUS_FAILURE;
    }
  }

  /* Copy the image data into the RAM buffer */
  if ( ( stOtaClientBlockTransfer.lFirmwareBufferCurrentOffset + cLength ) > FUOTA_RAM_BUFFER_SIZE )
  {
    /* The image data will not fit in the remaining RAM buffer */
    cLengthToWrite = ( uint8_t ) ( FUOTA_RAM_BUFFER_SIZE - stOtaClientBlockTransfer.lFirmwareBufferCurrentOffset );
    cLengthResidual = cLength - cLengthToWrite;
    stOtaClientBlockTransfer.bFirmwareBufferFull = true;
  }
  else
  {
    if ( ( stOtaClientBlockTransfer.lFirmwareBufferCurrentOffset + cLength ) == FUOTA_RAM_BUFFER_SIZE )
    {
      /* The image data fits perfectly in the remaining RAM buffer */
      stOtaClientBlockTransfer.bFirmwareBufferFull = true;
    }
  }
  (void)memcpy( ( void * )( stOtaClientBlockTransfer.szFirmwareBuffer + stOtaClientBlockTransfer.lFirmwareBufferCurrentOffset ), pData, cLengthToWrite );
  stOtaClientBlockTransfer.lFirmwareBufferCurrentOffset += cLengthToWrite;

  if ( stOtaClientBlockTransfer.bFirmwareBufferFull == true )
  {
    /* Copy the RAM buffer into flash. (First chars is for return to previous log line) */
    LOG_INFO_APP( "\x1b[A[OTA] Copying %u bytes to flash at 0x%08X\n", FUOTA_RAM_BUFFER_SIZE, stOtaClientBlockTransfer.lFlashCurrentAddr );
    if ( AppZbOtaClient_WriteFirmwareData( stOtaClientBlockTransfer.lFlashCurrentAddr, stOtaClientBlockTransfer.szFirmwareBuffer, FUOTA_RAM_BUFFER_SIZE ) == false )
    {
      LOG_INFO_APP( "[OTA] Error, failed to write to flash during 'WriteImage' Callback." );
      return ZCL_STATUS_FAILURE;
    }

    /* Configure the global write info */
    /* Note: The RAM buffer length should be > 127 to prevent the residual from overflowing the buffer a second time here */
    (void)memset( stOtaClientBlockTransfer.szFirmwareBuffer, 0xFF, sizeof(stOtaClientBlockTransfer.szFirmwareBuffer) );
    if ( ( cLengthResidual != 0u ) && ( ( cLengthToWrite + cLengthResidual ) <= sizeof(stOtaClientBlockTransfer.szFirmwareBuffer) ) )
    {
      (void)memcpy( stOtaClientBlockTransfer.szFirmwareBuffer, &pData[cLengthToWrite], cLengthResidual );
    }
    stOtaClientBlockTransfer.lFirmwareBufferCurrentOffset = cLengthResidual;
    stOtaClientBlockTransfer.bFirmwareBufferFull = false;
    stOtaClientBlockTransfer.lFlashCurrentAddr += FUOTA_RAM_BUFFER_SIZE;
  }

  stOtaClientBlockTransfer.lCurrentOffset += cLength;

  return ZCL_STATUS_SUCCESS;
}


/**
 *
 */
static enum ZclStatusCodeT AppZbOtaClient_UpgradeEndCallback( struct ZbZclClusterT * pstCluster, struct ZbZclOtaHeader * pstHeader, uint32_t lCurrentTime, uint32_t lUpgradeTime, void * arg )
{
  uint32_t      lFirmwareBufferRemainingSize;
  double        lDownloadElaspedTime;
  double        lDownloadThroughput;
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;

  lFirmwareBufferRemainingSize = stOtaClientBlockTransfer.lFirmwareBufferCurrentOffset;
  if ( lFirmwareBufferRemainingSize != 0u )
  {
    /* Copy the remaining RAM buffer into flash */
    LOG_INFO_APP( "\x1b[A[OTA] Copying %u bytes to flash at 0x%08X\n", lFirmwareBufferRemainingSize, stOtaClientBlockTransfer.lFlashCurrentAddr );
    if ( AppZbOtaClient_WriteFirmwareData( stOtaClientBlockTransfer.lFlashCurrentAddr, stOtaClientBlockTransfer.szFirmwareBuffer, lFirmwareBufferRemainingSize ) == false )
    {
      LOG_INFO_APP( "[OTA] Error, failed to write to flash during 'UpgradeEnd' callback.");
      eStatus = ZCL_STATUS_FAILURE;
    }
    else
    {
      /* Indicate End of download is done */
      stOtaClientBlockTransfer.lFirmwareBufferCurrentOffset = 0;

      /* Set the new file version */
      /* Note: It doesn't matter if this is line is called because the device will undergo a soft reset */
      stCurrentOtaClientInfo.lCurrentFileVersion = pstHeader->file_version;

      /* Record the end time */
      stOtaClientBlockTransfer.lDownloadEndTime = HAL_GetTick();

      /* Calculate the performance */
      lDownloadElaspedTime = ( double )( stOtaClientBlockTransfer.lDownloadEndTime - stOtaClientBlockTransfer.lDownloadStartTime ) / 1000.f;
      lDownloadThroughput = ( double )( stOtaClientBlockTransfer.lFlashCurrentAddr - stCurrentOtaClientInfo.lBaseAddress ) / lDownloadElaspedTime * 8.f / 1000.f;
      LOG_INFO_APP( "  Image download timing data:" );
      LOG_INFO_APP( "    Download start time (ms):      %u", stOtaClientBlockTransfer.lDownloadStartTime );
      LOG_INFO_APP( "    Download end time (ms):        %u", stOtaClientBlockTransfer.lDownloadEndTime );
      LOG_INFO_APP( "    Download elapsed time (s):     %.2f", lDownloadElaspedTime );
      LOG_INFO_APP( "    Download throughput (kbit/s):  %.2f", lDownloadThroughput );
      LOG_INFO_APP( "    Current time (s):              %u", lCurrentTime );
      LOG_INFO_APP( "    Upgrade time (s):              %u", lUpgradeTime );

      /* Use ZB Stack to launch Reset Callback */
      eStatus = fZbStackUpgradeEndCallback( pstCluster, pstHeader, lCurrentTime, lUpgradeTime, arg );
    }
  }

  return eStatus;
}

/**
 * @brief  OTA client Image Validate callback
 * @param  clusterPtr: ZCL Cluster pointer
 * @param  pstHeader: ZCL OTA file format image header
 * @param  arg: Passed arg
 * @retval ZCL status code
 */
static enum ZclStatusCodeT AppZbOtaClient_ImageValidateCallback( struct ZbZclClusterT * pstCluster, struct ZbZclOtaHeader * pstHeader, void * arg )
{
//  uint32_t      lBinarySize;
//  uint32_t      lBinaryCrc;
  enum ZclStatusCodeT     eStatus = ZCL_STATUS_FAILURE;

//    /* Configure the global write info */
//    (void)memset( stOtaClientBlockTransfer.szFirmwareBuffer, 0xFF, sizeof(stOtaClientBlockTransfer.szFirmwareBuffer) );
//    stOtaClientBlockTransfer.lFirmwareBufferCurrentOffset = 0u;
//    stOtaClientBlockTransfer.lFlashCurrentAddr += lFirmwareBufferRemainingSize;

//    /* Get the binary info*/
//    LOG_INFO_APP( "[OTA] Getting the binary info..." );
//    if ( AppZbOtaClient_GetBinInfo( stCurrentOtaClientInfo, true, &lBinarySize, &lBinaryCrc ) == false )
//    {
//      LOG_INFO_APP( "[OTA] Error, binary info invalid." );
//      eStatus = ZCL_STATUS_INVALID_IMAGE;
//    }
//    else
//    {
//      /* Verify the binary length is correct */
//      /* Note: This is an additional check */
//      LOG_INFO_APP( "[OTA] Verifying the binary length is correct..." );
//      if ( stOtaClientBlockTransfer.lFlashCurrentAddr - stCurrentOtaClientInfo.lBaseAddress != lBinarySize )
//      {
//        LOG_INFO_APP( "[OTA] Error, binary length is incorrect (0x%08X / 0x%08X).", (stOtaClientBlockTransfer.lFlashCurrentAddr - stCurrentOtaClientInfo.lBaseAddress), lBinarySize );
//        eStatus = ZCL_STATUS_INVALID_IMAGE;
//      }
//      else
//      {
        LOG_INFO_APP( "[OTA] The downloaded firmware is valid" );
        eStatus = ZCL_STATUS_SUCCESS;
//      }
//    }

  return eStatus;
}

/**
 * @brief  Reset block transfer cached information
 * @param  None
 * @retval None
 */
static void AppZbOtaClient_ResetBlockTransfer( void )
{
  (void)memset( &stOtaClientBlockTransfer.stCurrentHeader, 0, sizeof( stOtaClientBlockTransfer.stCurrentHeader ) );
  stOtaClientBlockTransfer.lCurrentOffset = 0;
  stOtaClientBlockTransfer.lDownloadStartTime = 0;
  stOtaClientBlockTransfer.lDownloadEndTime = 0;

  (void)memset( &stOtaClientBlockTransfer.szFirmwareBuffer, 0xFF, sizeof( stOtaClientBlockTransfer.szFirmwareBuffer ) );
  stOtaClientBlockTransfer.lFirmwareBufferCurrentOffset = 0;
  stOtaClientBlockTransfer.bFirmwareBufferFull = false;
  stOtaClientBlockTransfer.lFlashCurrentAddr = stCurrentOtaClientInfo.lBaseAddress;
}

/**
 * @brief  Deleting sectors helper
 * @param  None
 * @retval True if Erase OK else False.
 */
static bool AppZbOtaClient_DelectSectors( uint32_t lBinaryAddress, uint32_t lBinarySize )
{
  bool bReturn = true;
  FLASH_EraseInitTypeDef  stEraseInit;
  uint32_t                lPageError = 0;
  HAL_StatusTypeDef       eStatus;

  /* Determine the page, the number of pages to delete, etc. */
  (void)memset( &stEraseInit, 0, sizeof( stEraseInit ) );

  stEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
#ifdef FLASH_DBANK_SUPPORT
  stEraseInit.Banks = FUOTA_APP_DOWNLOAD_BINARY_BANK;
#endif // FLASH_DBANK_SUPPORT
  stEraseInit.Page = ( lBinaryAddress - FLASH_BASE ) / FLASH_PAGE_SIZE;
  stEraseInit.NbPages = lBinarySize / FLASH_PAGE_SIZE;
  if ( lBinarySize % FLASH_PAGE_SIZE != 0u )
  {
    stEraseInit.NbPages++;
  }

  /* Delete the flash sectors */
  (void)HAL_FLASH_Unlock();
  eStatus = HAL_FLASHEx_Erase( &stEraseInit, &lPageError );
  (void)HAL_FLASH_Lock();

  if (eStatus != HAL_OK )
  {
    bReturn = false;
  }

  return bReturn;
}

/**
 * @brief  OTA client Reboot callback
 * @param  pstCluster: ZCL Cluster pointer
 * @param  arg: Passed arg
 * @retval None
 */
static void AppZbOtaClient_RebootCallback( struct ZbZclClusterT * pstCluster, void * arg )
{
  LOG_INFO_APP( "**********************************************************" );
  LOG_INFO_APP( "[OTA] Rebooting." );
  LOG_INFO_APP( "**********************************************************" );

  /* Start a reboot. Wait for the log to flush */
  HAL_Delay( 1000u );

  /* Reboot on Update FW */
  * ( uint32_t * ) SRAM1_BASE = (uint32_t)FUOTA_REBOOT_ON_UPDATE_FW;
  NVIC_SystemReset();
}

/**
 * @brief  OTA client Abort callback
 * @param  pstCluster: ZCL Cluster pointer
 * @param  eCommandId: ZCL OTA command ID associated to the block transfer abortion
 * @param  arg: Passed arg
 * @retval ZCL status code
 */
static enum ZclStatusCodeT AppZbOtaClient_AbortDownloadCallback( struct ZbZclClusterT * pstCluster, enum ZbZclOtaCommandId eCommandId, void * arg )
{
  LOG_INFO_APP( "[OTA] Aborting download with associated OTA command ID 0x%02X.", eCommandId);

  if ( eCommandId == ZCL_OTA_COMMAND_UPGRADE_END_REQUEST)
  {
    // Ignore it and retry later using transfer resume
    //normally done by a timer to allow context switch ....

    UTIL_TIMER_StartWithPeriod( &stTimerUpgradeEndRequestRetryId, 5000 );

    return ZCL_STATUS_FAILURE;
  }

  /* Reset block transfer cached information */
  AppZbOtaClient_ResetBlockTransfer();

  return ZCL_STATUS_SUCCESS;
}

/**
 *
 */
static enum ZclStatusCodeT AppZbOtaClient_RawDataCallback (struct ZbZclClusterT *cluster, uint8_t length, uint8_t *data, void *arg)
{
//  LOG_INFO_APP( "[OTA] Adding partial data length %d bytes to Hash.", length);
  (void)ZbHashAdd( &stOtaImageHash, data, length );

  return ZCL_STATUS_SUCCESS;
}

/**
 * @brief  OTA Client Discover callback
 *
 * @param  pstClusterPtr  Pointer on Cluster
 * @param  eStatus        Callback status
 * @param  arg            Passed argument
 *
 * @retval None
 */
static void AppZbOtaClient_DiscoverCompleteCallback( struct ZbZclClusterT * pstCluster, enum ZclStatusCodeT eStatus, void * arg )
{
  enum ZclStatusCodeT   eInternalStatus = ZCL_STATUS_SUCCESS;
  uint64_t              lRequestedServerExt = 0;

  if ( eStatus == ZCL_STATUS_SUCCESS )
  {
    /* The OTA server extended address in stored in ZCL_OTA_ATTR_UPGRADE_SERVER_ID attribute */
    lRequestedServerExt = (uint64_t)ZbZclAttrIntegerRead( pOtaClient, (uint16_t)ZCL_OTA_ATTR_UPGRADE_SERVER_ID, NULL, &eInternalStatus );
    if ( eInternalStatus != ZCL_STATUS_SUCCESS )
    {
      LOG_INFO_APP( "[OTA] Error, ZbZclAttrIntegerRead failed (0x%02X).", eInternalStatus );
    }
    else
    {
      LOG_INFO_APP( "[OTA] Server located with extended address " LOG_DISPLAY64() ".", LOG_NUMBER64( lRequestedServerExt ) );
      bOtaServerAvailable = true;
    }
  }
  else
  {
    LOG_INFO_APP( "[OTA] Server not found." );
  }

  UTIL_SEQ_SetEvt( EVENT_ZIGBEE_OTA_SERVER_FOUND );
}

/**
 * @brief  Search the OTA Server. Blocking function.
 * @param  None
 * @retval None
 */
bool AppZbOtaClient_SearchOtaServer( void )
{
  enum ZclStatusCodeT   eStatus;
  struct ZbApsAddrT     stDestination;

  LOG_INFO_APP( "[OTA] Searching for Server ..." );

  /* Setting up the addressing mode */
  (void)memset( &stDestination, 0, sizeof(stDestination) );

  stDestination.mode = ZB_APSDE_ADDRMODE_SHORT;
  stDestination.endpoint = stCurrentOtaClientInfo.cEndpoint;
  stDestination.nwkAddr = 0x0;
  bOtaServerAvailable = false;

  /* Sending Discovery request to server */
  UTIL_SEQ_ClrEvt( EVENT_ZIGBEE_OTA_SERVER_FOUND );
  eStatus = ZbZclOtaClientDiscover( pOtaClient, &stDestination );
  if ( eStatus == ZCL_STATUS_SUCCESS )
  {
    UTIL_SEQ_WaitEvt( EVENT_ZIGBEE_OTA_SERVER_FOUND );
  }

  return bOtaServerAvailable;
}

/**
 * @brief  OTA client request upgrade
 * @param  None
 * @retval None
 */
static void AppZbOtaClient_RequestUpgrade( void )
{
  uint32_t                        lMaximumImageSize, lBinarySize;
  struct ZbZclOtaImageDefinition  stImageDefinition;
  enum ZclStatusCodeT             eStatus;

  LOG_INFO_APP( "[OTA] Server Request Upgrade ..." );
  if ( stCurrentOtaClientInfo.lOtaTagMagicKeyword != 0u )
  {
    /* Get the binary info*/
    lMaximumImageSize = ( FLASH_BASE + FLASH_SIZE ) - stCurrentOtaClientInfo.lBaseAddress;

    LOG_INFO_APP( "[OTA] Getting the binary info..." );

    /* Set the binary size to the maximum size */
    lBinarySize = lMaximumImageSize;

    /* Erase the sectors used to download the image */
    LOG_INFO_APP( "[OTA] Deleting flash sectors from 0x%08X to 0x%08X...", stCurrentOtaClientInfo.lBaseAddress, ( stCurrentOtaClientInfo.lBaseAddress + lBinarySize ) );
    if ( AppZbOtaClient_DelectSectors(stCurrentOtaClientInfo.lBaseAddress, lBinarySize ) == false )
    {
      LOG_INFO_APP( "[OTA] Error during Page Erase. Download cannot be done." );
      return;
    }

    /* Reset block transfer cached information */
    AppZbOtaClient_ResetBlockTransfer();

    /* Image definition configuration */
    (void)memset( &stImageDefinition, 0, sizeof( stImageDefinition ) );
    stImageDefinition.manufacturer_code = stCurrentOtaClientInfo.iManufacturerCode;
    stImageDefinition.image_type = stCurrentOtaClientInfo.iImageType;
    stImageDefinition.file_version = stCurrentOtaClientInfo.lCurrentFileVersion;

    /* Sending Query Next Image Request to server */
    /* Note: HW version is provided as additional info in Query Next Image Request */
    LOG_INFO_APP( "[OTA] Contents of the Query Next Image Request:" );
    LOG_INFO_APP( "  Manufacturer Code    : 0x%04X", stImageDefinition.manufacturer_code );
    LOG_INFO_APP( "  Image type           : 0x%04X", stImageDefinition.image_type );
    LOG_INFO_APP( "  Current file version : 0x%08X", stImageDefinition.file_version );
    LOG_INFO_APP( "  Sending Query Next Image Request ..." );

    eStatus = ZbZclOtaClientQueryNextImageReq( pOtaClient, &stImageDefinition, (uint8_t)ZCL_OTA_QUERY_FIELD_CONTROL_HW_VERSION, FUOTA_FW_APP_HARDWARE_VERSION );
    if ( eStatus != ZCL_STATUS_SUCCESS )
    {
      LOG_INFO_APP( "[OTA] Error, Query Next Image Request failed (0x%02X).", eStatus );
    }
  }
  else
  {
    LOG_INFO_APP( "[OTA] Error, pointer on Client Info is NULL." );
  }
}

