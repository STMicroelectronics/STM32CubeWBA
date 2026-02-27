/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_zigbee_ota_server.c
  * Description        : Zigbee Application to OTA Server Cluster
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
#include "zcl/zcl.payload.h"
#include "zcl/general/zcl.ota.h"

/* Private defines -----------------------------------------------------------*/
/* Define OTA Tags properties */
#define OTA_TAGS_NB_MAX                     20u
#define OTA_TAG_HEADER_SIZE                 6u
#define OTA_HEADER_STRING_SIZE              32u

#define CFG_TASK_PRIO_OTA_REQUEST_UPDATE    CFG_SEQ_PRIO_1

/* Private Structures -----------------------------------------------------------*/

/** OTA Upgrade Header Fields structure */
typedef struct __attribute__((packed))
{
    uint32_t file_identifier;
    uint16_t header_version;
    uint16_t header_length;
    uint16_t header_field_control;
    uint16_t manufacturer_code;
    uint16_t image_type;
    uint32_t file_version;
    uint16_t stack_version;
    uint8_t header_string[32];
    uint32_t total_image_size;
    uint8_t sec_credential_version;
    uint64_t file_destination;
    uint16_t min_hardware_version;
    uint16_t max_hardware_version;
} ZbZclOtaHeaderPacked_st;

/* OTA Image Structures */
typedef struct
{
    uint16_t  iTagId;
    uint32_t  lTagLength;
    uint32_t  lTagOffset;
} OtaServerTagInfo_st;

typedef struct
{
    struct ZbZclOtaHeader   stHeader;
    OtaServerTagInfo_st     stTagInfo[OTA_TAGS_NB_MAX];
    uint8_t     szHeaderBuffer[ZCL_OTA_HEADER_LENGTH_MAX];
    uint32_t    lHeaderBufferSize;
    uint32_t    lBaseAddress;
    uint32_t    lBinarySize;
    uint32_t    lBinaryCrc;
    uint32_t    lUpdateTimeStart;
    uint32_t    lUpdateTimeEnd;

    uint16_t    iTagCount;
    bool        bTagsDiscovered;
} OtaServerImage_st;

typedef struct
{
    struct ZbApsAddrT               stDest;
    struct ZbZclOtaImageDefinition  stImageDefinition;
    struct ZbZclOtaEndResponseTimes stResponseTimes;
} OtaServerReset_st;

/* Private Function -----------------------------------------------------------*/
static void       AppZbOtaServer_TimeoutForRequestUpdate  ( void * arg );
static bool       AppZbOtaServer_DiscoverTags             ( OtaServerImage_st * pstOtaImage );
static uint16_t   AppZbOtaServer_HeaderMake               ( const struct ZbZclOtaHeader * pstOtaHeader, uint8_t  *pcPayload );

static bool       AppZbOtaServer_ImageEvalCallback        ( struct ZbZclOtaImageDefinition *pstQueryImage, uint8_t cFieldControl,
                                                            uint16_t iHardwareVersion, uint32_t *plImageSize, void *arg);

static enum ZclStatusCodeT AppZbOtaServer_ImageReadCallback ( struct ZbZclOtaImageDefinition *pstImage, struct ZbZclOtaImageData *pstImageData,
                                                              uint8_t cFieldControl, uint64_t dlRequestNodeAddress,
                                                              struct ZbZclOtaImageWaitForData *pstImageWait, void *arg);

static enum ZclStatusCodeT AppZbOtaServer_ImageUpgradeEndReqCallback( struct ZbZclOtaImageDefinition *pstImageDefinition,
                                                                      enum ZclStatusCodeT eUpdateStatus, struct ZbZclOtaEndResponseTimes *pstEndResponseTimes,
                                                                      struct ZbZclAddrInfoT *pstSourceInfo, void *arg);

/* Private Variables -----------------------------------------------------------*/
static struct ZbZclClusterT       * pOtaServer;
static OtaServerImage_st          stOtaBinaryImage;
static OtaServerReset_st          stOtaServerReset;
static UTIL_TIMER_Object_t        stTimerRequestUpdateWaitId;

/**
 * @brief  OTA Endpoint Initialisation
 * @return
 */
static bool AppZbOtaServer_AddEnpoint( struct ZigBeeT * pstZigbee, uint8_t cEndpoint, uint16_t iProfile, uint16_t iDeviceId )
{
  struct ZbApsmeAddEndpointReqT   stRequest;
  struct ZbApsmeAddEndpointConfT  stConfig;

  /* Verify if Endpoint already exist */
  if ( ZbApsEndpointExists( pstZigbee, cEndpoint ) != false )
  {
     LOG_INFO_APP( "OTA Endpoint %d already exist.", cEndpoint );
  }
  else
  {
    /* Add EndPoint */
    LOG_INFO_APP( "Creating OTA Endpoint ..." );

    (void)memset( &stRequest, 0, sizeof( stRequest ) );
    (void)memset( &stConfig, 0, sizeof( stConfig ) );

    stRequest.endpoint = cEndpoint;
    stRequest.profileId = iProfile;
    stRequest.deviceId = iDeviceId;

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
 * @brief  Add OTA Server Cluster
 */
static bool AppZbOtaServer_AddCluster( struct ZigBeeT * pstZigbee, uint8_t cEndpoint, uint16_t iProfile )
{
  struct ZbZclOtaServerConfig   stOtaServerConfig ;

  /* Add OTA Client Cluster */
  LOG_INFO_APP( "Creating OTA Client Cluster ..." );
  (void)memset( &stOtaServerConfig, 0, sizeof( stOtaServerConfig ) );

  stOtaServerConfig.endpoint = cEndpoint;
  stOtaServerConfig.profile_id = iProfile;
  stOtaServerConfig.image_eval = AppZbOtaServer_ImageEvalCallback;
  stOtaServerConfig.image_read = AppZbOtaServer_ImageReadCallback;
  stOtaServerConfig.image_upgrade_end_req = AppZbOtaServer_ImageUpgradeEndReqCallback;

  /* OTA Server */
  pOtaServer = ZbZclOtaServerAlloc( pstZigbee, &stOtaServerConfig, (void*)&stOtaBinaryImage );
  if ( pOtaServer == NULL )
  {
    LOG_ERROR_APP( "  Error, ZbZclOtaClientAlloc failed." );
    return false;
  }

  if ( iProfile == (uint16_t)ZCL_PROFILE_SMART_ENERGY )
  {
    /* Set the Smart Energy Profile Id (should be redundant for this SE cluster) */
    ZbZclClusterSetProfileId( pOtaServer, iProfile );

    /* Set minimum security level */
    (void)ZbZclClusterSetMinSecurity( pOtaServer, ZB_APS_STATUS_SECURED_LINK_KEY );
  }

  if ( ZbZclClusterEndpointRegister( pOtaServer ) == false )
  {
    LOG_ERROR_APP( "  Error, ZbZclClusterEndpointRegister failed." );
    return false;
  }

  return true;
}

/**
 * @brief  Init OTA Server Cluster/Endpoint
 */
bool AppZbOtaServer_ClusterInit( struct ZigBeeT * pstZigbee, uint8_t cEndpoint, uint16_t iProfile, uint16_t iDeviceId )
{
  bool    bReturn;

  /* Configure OTA Server Endpoint */
  bReturn = AppZbOtaServer_AddEnpoint( pstZigbee, cEndpoint, iProfile, iDeviceId );

  /* Configure OTA Server Cluster */
  if ( bReturn != false )
  {
    bReturn = AppZbOtaServer_AddCluster( pstZigbee, cEndpoint, iProfile );
  }

  if ( bReturn != false )
  {
    /* Tasks associated to OTA update process */
    UTIL_SEQ_RegTask( TASK_ZIGBEE_OTA_REQUEST_UPGRADE, UTIL_SEQ_RFU, AppZbOtaServer_StartClientUpdate );

    /* Timer associated to OTA update process */
    UTIL_TIMER_Create( &stTimerRequestUpdateWaitId, 0, UTIL_TIMER_ONESHOT, AppZbOtaServer_TimeoutForRequestUpdate, NULL );
  }

  return bReturn;
}

/**
 * @brief Maker the TAG Header for OTA CLient.
 */
static uint16_t AppZbOtaServer_HeaderMake( const struct ZbZclOtaHeader * pstOtaHeader, uint8_t * pcPayload )
{
    unsigned int  iIndex = 0;
    const uint16_t iCapacity = ZCL_OTA_HEADER_LENGTH_MAX;

    if ( zb_zcl_append_uint32( pcPayload, iCapacity, &iIndex, pstOtaHeader->file_identifier ) < 0 )
    {
        return 0;
    }

    if ( zb_zcl_append_uint16( pcPayload, iCapacity, &iIndex, pstOtaHeader->header_version ) < 0 )
    {
        return 0;
    }

    if ( zb_zcl_append_uint16( pcPayload, iCapacity, &iIndex, pstOtaHeader->header_length ) < 0 )
    {
        return 0;
    }

    if ( zb_zcl_append_uint16( pcPayload, iCapacity, &iIndex, pstOtaHeader->header_field_control)  < 0 )
    {
        return 0;
    }

    if ( zb_zcl_append_uint16( pcPayload, iCapacity, &iIndex, pstOtaHeader->manufacturer_code ) < 0 )
    {
        return 0;
    }

    if ( zb_zcl_append_uint16( pcPayload, iCapacity, &iIndex, pstOtaHeader->image_type ) < 0 )
    {
        return 0;
    }

    if ( zb_zcl_append_uint32( pcPayload, iCapacity, &iIndex, pstOtaHeader->file_version ) < 0 )
    {
        return 0;
    }

    if ( zb_zcl_append_uint16( pcPayload, iCapacity, &iIndex, pstOtaHeader->stack_version ) < 0 )
    {
        return 0;
    }

    if ( ( iIndex + OTA_HEADER_STRING_SIZE ) > iCapacity )
    {
        return 0;
    }

    memcpy( pcPayload + iIndex, pstOtaHeader->header_string, OTA_HEADER_STRING_SIZE );
    iIndex += OTA_HEADER_STRING_SIZE;

    if ( zb_zcl_append_uint32(pcPayload, iCapacity, &iIndex, pstOtaHeader->total_image_size ) < 0 )
    {
        return 0;
    }

    if ( ( pstOtaHeader->header_field_control & ZCL_OTA_HEADER_FIELD_CONTROL_SECURITY_VERSION ) != 0u )
    {
        if ( zb_zcl_append_uint8( pcPayload, iCapacity, &iIndex, pstOtaHeader->sec_credential_version ) < 0 )
        {
            return 0;
        }
    }

    if ( ( pstOtaHeader->header_field_control & ZCL_OTA_HEADER_FIELD_CONTROL_DEVICE_SPECIFIC ) != 0u )
    {
        if ( zb_zcl_append_uint64( pcPayload, iCapacity, &iIndex, pstOtaHeader->file_destination ) < 0)
            return 0;
    }

    if ( ( pstOtaHeader->header_field_control & ZCL_OTA_HEADER_FIELD_CONTROL_HARDWARE_VERSIONS ) != 0u )
    {
        if ( zb_zcl_append_uint16( pcPayload, iCapacity, &iIndex, pstOtaHeader->min_hardware_version ) < 0 )
        {
            return 0;
        }

        if ( zb_zcl_append_uint16( pcPayload, iCapacity, &iIndex, pstOtaHeader->max_hardware_version ) < 0 )
        {
            return 0;
        }
    }

    return (uint16_t)iIndex;
}

/**
 * @brief   Parse Header from Enpackted Binary File
 *
 * @param   pstOtaImage
 * @return  True if Ok else false
 */
bool AppZbOtaServer_GetBinInfo( void )
{
    const uint8_t     * pFlashAddress;
    OtaServerImage_st * pstOtaImage;
    ZbZclOtaHeaderPacked_st   stPackedHeader;

    /* Init parameters */
    pstOtaImage = &stOtaBinaryImage;
    pFlashAddress = (const uint8_t *)FUOTA_APP_DOWNLOAD_BINARY_ADDRESS;

    LOG_INFO_APP("[OTA] Reading OTA header from 0x%08X ...", FUOTA_APP_DOWNLOAD_BINARY_ADDRESS);

    /* Copy OTA Header in a 'PACKED structure */
    memcpy( &stPackedHeader, pFlashAddress, sizeof(stPackedHeader) );

    if ( stPackedHeader.file_identifier != FUOTA_MAGIC_KEYWORD_TAG_APPLI )
    {
        LOG_INFO_APP( "  Invalid file identifier: 0x%08X", stPackedHeader.file_identifier );
        return false;
    }

    if  ( ( stPackedHeader.header_length < 56u ) || ( stPackedHeader.header_length > 64u ) )
    {
        LOG_INFO_APP("  Invalid header_length: %u", stPackedHeader.header_length);
        return false;
    }

    if ( stPackedHeader.total_image_size < stPackedHeader.header_length )
    {
        LOG_INFO_APP("  Total Image Size (%u) < Header Length (%u)", pstOtaImage->stHeader.total_image_size, pstOtaImage->stHeader.header_length );
        return false;
    }

    /* Copy from Packed to UnPacked structure */
    pstOtaImage->stHeader.file_identifier = stPackedHeader.file_identifier;
    pstOtaImage->stHeader.header_version = stPackedHeader.header_version;
    pstOtaImage->stHeader.header_length = stPackedHeader.header_length;
    pstOtaImage->stHeader.header_field_control = stPackedHeader.header_field_control;
    pstOtaImage->stHeader.manufacturer_code = stPackedHeader.manufacturer_code;
    pstOtaImage->stHeader.image_type = stPackedHeader.image_type;
    pstOtaImage->stHeader.file_version = stPackedHeader.file_version;
    pstOtaImage->stHeader.stack_version = stPackedHeader.stack_version;
    memcpy( pstOtaImage->stHeader.header_string, stPackedHeader.header_string, sizeof(stPackedHeader.header_string) );
    pstOtaImage->stHeader.total_image_size = stPackedHeader.total_image_size;

    /* Add 'possible Unused' fields */
    if ( ( stPackedHeader.header_field_control & ZCL_OTA_HEADER_FIELD_CONTROL_SECURITY_VERSION ) != 0u )
    {
        pstOtaImage->stHeader.sec_credential_version = stPackedHeader.sec_credential_version;
    }

    if ( ( stPackedHeader.header_field_control & ZCL_OTA_HEADER_FIELD_CONTROL_DEVICE_SPECIFIC ) != 0u )
    {
        pstOtaImage->stHeader.file_destination = stPackedHeader.file_destination;
    }

    if ( (  stPackedHeader.header_field_control & ZCL_OTA_HEADER_FIELD_CONTROL_HARDWARE_VERSIONS) != 0u )
    {
        pstOtaImage->stHeader.min_hardware_version = stPackedHeader.min_hardware_version;
        pstOtaImage->stHeader.max_hardware_version = stPackedHeader.max_hardware_version;
    }

    /* Update others fields */
    pstOtaImage->lBaseAddress = FUOTA_APP_DOWNLOAD_BINARY_ADDRESS;
    pstOtaImage->lBinarySize = pstOtaImage->stHeader.total_image_size;
    pstOtaImage->lHeaderBufferSize = 0;
    pstOtaImage->bTagsDiscovered = false;
    pstOtaImage->iTagCount = 0;

    /* Display Info */
    LOG_INFO_APP( "  Binary Image Found:" );
    LOG_INFO_APP( "    Manufacturer Code  : 0x%04X", pstOtaImage->stHeader.manufacturer_code );
    LOG_INFO_APP( "    Image type         : 0x%04X", pstOtaImage->stHeader.image_type );
    LOG_INFO_APP( "    File version       : 0x%08X", pstOtaImage->stHeader.file_version );
    LOG_INFO_APP( "    Size (bytes)       : 0x%08X", pstOtaImage->lBinarySize );
    if ( pstOtaImage->stHeader.header_string[0] != 0u )
    {
      LOG_INFO_APP( "    Image Name         : %.32s", pstOtaImage->stHeader.header_string );
    }

    return true;
}

/*
 * @brief   Search and Parse TAGs from Enpackted Binary File (for info only).
 *
 * @param   pstOtaImage
 * @return  True if Ok else false
 */
static bool AppZbOtaServer_DiscoverTags( OtaServerImage_st * pstOtaImage )
{
    uint8_t * pFlashAddress;
    char    * szTagName;
    bool      bReturn = false;
    uint16_t  iTagId;
    uint32_t  lTagLength, lTagStartOffset, lTagEndOffset, lCurrentOffset;

    /* Init Parameters */
    lTagStartOffset = pstOtaImage->stHeader.header_length;
    lTagEndOffset = pstOtaImage->stHeader.total_image_size;
    lCurrentOffset = lTagStartOffset;

    pFlashAddress = (uint8_t *)FUOTA_APP_DOWNLOAD_BINARY_ADDRESS;
    LOG_INFO_APP("[OTA] Reading TAG from 0x%08X ...", FUOTA_APP_DOWNLOAD_BINARY_ADDRESS);

    pstOtaImage->iTagCount = 0;
    while ( ( ( lCurrentOffset + OTA_TAG_HEADER_SIZE ) <= lTagEndOffset ) && ( pstOtaImage->iTagCount < OTA_TAGS_NB_MAX ) )
    {
        memcpy( &iTagId, &pFlashAddress[lCurrentOffset], sizeof(iTagId) );
        lCurrentOffset += sizeof(iTagId);

        memcpy( &lTagLength, &pFlashAddress[lCurrentOffset], sizeof(lTagLength) );
        lCurrentOffset += sizeof(lTagLength);

        if ( (lCurrentOffset + OTA_TAG_HEADER_SIZE + lTagLength) > lTagEndOffset )
        {
            LOG_INFO_APP("  Sub-element overrun: offset=0x%X, length=%u, end=0x%X", lCurrentOffset, lTagLength, lTagEndOffset);
            return false;
        }

        switch ( iTagId )
        {
            case ZCL_OTA_SUB_TAG_UPGRADE_IMAGE:
                szTagName = "Upgrade Image";
                break;

            case ZCL_OTA_SUB_TAG_ECDSA_SIG1:
                szTagName = "ECDSA P256 Signature";
                break;

            case ZCL_OTA_SUB_TAG_ECDSA_CERT_1:
                szTagName = "ECDSA P256 Certificate";
                break;

            case ZCL_OTA_SUB_TAG_IMAGE_INTEGRITY_CODE:
                szTagName = "Image Integrity Code";
                break;

            case ZCL_OTA_SUB_TAG_ECDSA_SIG2:
                szTagName = "ECDSA P384 Signature";
                break;

            case ZCL_OTA_SUB_TAG_ECDSA_CERT_2:
                szTagName = "ECDSA P384 Certificate";
                break;

            default:
                if (iTagId >= 0xF000)
                {
                  szTagName = "Manufacturer Specific";
                }
                else
                {
                  szTagName = "Unknown";
                }
                break;
        }

        LOG_INFO_APP("  Sub-element[%u]: ID=0x%04X (%s), Length=%u bytes, Offset=0x%X", pstOtaImage->iTagCount, iTagId, szTagName, lTagLength, lCurrentOffset);

        pstOtaImage->stTagInfo[pstOtaImage->iTagCount].iTagId = iTagId;
        pstOtaImage->stTagInfo[pstOtaImage->iTagCount].lTagLength = lTagLength;
        pstOtaImage->stTagInfo[pstOtaImage->iTagCount].lTagOffset = lCurrentOffset;
        pstOtaImage->iTagCount++;

        lCurrentOffset += OTA_TAG_HEADER_SIZE + lTagLength;
        bReturn = true;
    }

    /* Final test */
    if ( lCurrentOffset != lTagEndOffset )
    {
        LOG_INFO_APP("  Sub-element parsing ended at 0x%X, expected 0x%X", lCurrentOffset, lTagEndOffset );
        bReturn = false;
    }

    return bReturn;
}

/**
 *
 */
static bool AppZbOtaServer_ImageEvalCallback( struct ZbZclOtaImageDefinition *pstQueryImage, uint8_t cFieldControl, uint16_t iHardwareVersion, uint32_t *plImageSize, void *arg)
{
    OtaServerImage_st * pstOtaImage = (OtaServerImage_st *)arg;

    LOG_INFO_APP("[OTA] Query Next Image: mfg=0x%04X, type=0x%04X, ver=0x%08X",
                 pstQueryImage->manufacturer_code, pstQueryImage->image_type, pstQueryImage->file_version);

    if ( pstOtaImage == NULL )
    {
      LOG_INFO_APP("[OTA] OTA Image is NULL");
      return false;
    }

    if ( ( pstQueryImage->manufacturer_code != pstOtaImage->stHeader.manufacturer_code ) || ( pstQueryImage->image_type != pstOtaImage->stHeader.image_type ) )
    {
        LOG_INFO_APP("[OTA] Image mismatch");
        return false;
    }

    if ( pstOtaImage->stHeader.file_version <= pstQueryImage->file_version )
    {
        LOG_INFO_APP("[OTA] No newer version");
        return false;
    }

    *plImageSize = pstOtaImage->stHeader.total_image_size;
    pstQueryImage->file_version = pstOtaImage->stHeader.file_version;

    LOG_INFO_APP("[OTA] Found suitable image: size=%u bytes", *plImageSize);

    pstOtaImage->lUpdateTimeStart = HAL_GetTick();

    return true;
}

/**
 *
 */
static enum ZclStatusCodeT AppZbOtaServer_ImageReadCallback( struct ZbZclOtaImageDefinition *pstImage, struct ZbZclOtaImageData *pstImageData,
                                                             uint8_t cFieldControl, uint64_t dlRequestNodeAddress, struct ZbZclOtaImageWaitForData *pstImageWait,
                                                             void *arg)
{
    OtaServerImage_st * pstOtaImage = (OtaServerImage_st *)arg;

    /* Discover sub-elements once on first request */
    if ( ( pstOtaImage->bTagsDiscovered != false ) && ( pstImageData->file_offset == 0u ) )
    {
        LOG_INFO_APP("[OTA] *** STAGE 1: DISCOVERING SUB-ELEMENTS ***");

        if ( AppZbOtaServer_DiscoverTags(pstOtaImage) == false )
        {
            LOG_INFO_APP("  Sub-element discovery failed");
            return ZCL_STATUS_INVALID_IMAGE;
        }

        pstOtaImage->bTagsDiscovered = true;
    }

    /* Build OTA header buffer once (send only header structure, NOT sub-elements) */
    if ( pstOtaImage->lHeaderBufferSize == 0u )
    {
        LOG_INFO_APP("[OTA] *** STAGE 2: TRANSFER HEADER & TAGS ***\n\n");

        pstOtaImage->lHeaderBufferSize = AppZbOtaServer_HeaderMake( &pstOtaImage->stHeader, pstOtaImage->szHeaderBuffer );

        if ( pstOtaImage->lHeaderBufferSize == 0u )
        {
            LOG_INFO_APP("[OTA] Failed to build header buffer");
            return ZCL_STATUS_FAILURE;
        }
    }

    /*
     * CRITICAL: Check if this request would exceed total_image_size
     * Clip data_size if necessary to not overshoot
     */
    uint32_t lMaxAllowedOffset = pstOtaImage->stHeader.total_image_size;
    uint32_t lEndOffset = pstImageData->file_offset + pstImageData->data_size;

    if (lEndOffset > lMaxAllowedOffset)
    {
        /* Clip to exact boundary */
        pstImageData->data_size = lMaxAllowedOffset - pstImageData->file_offset;

        if (pstImageData->data_size == 0)
        {
            LOG_INFO_APP("[OTA] End of file reached (no more data to serve)");
            return ZCL_STATUS_SUCCESS;
        }
    }

    if (pstImageData->file_offset < pstOtaImage->lHeaderBufferSize)
    {
        /* First: Serve OTA header from RAM buffer */
        uint32_t lRemainingInHeader = pstOtaImage->lHeaderBufferSize - pstImageData->file_offset;
        uint32_t lToCopyFromHeader = (pstImageData->data_size < lRemainingInHeader) ?
                                      pstImageData->data_size : lRemainingInHeader;

        memcpy(pstImageData->data, &pstOtaImage->szHeaderBuffer[pstImageData->file_offset], lToCopyFromHeader);

        /* If request spans header and sub-elements, fill rest from flash */
        if (lToCopyFromHeader < pstImageData->data_size)
        {
            uint32_t lRemainingToServe = pstImageData->data_size - lToCopyFromHeader;
            uint32_t lFlashSubElementBase = pstOtaImage->lBaseAddress + pstOtaImage->stHeader.header_length;

            memcpy( pstImageData->data + lToCopyFromHeader, (const void *)lFlashSubElementBase, lRemainingToServe );
        }

    }
    else
    {
        /* Then: Serve sub-elements directly from flash (with their tag headers intact) */
        uint32_t lSubElementOffset = pstImageData->file_offset - pstOtaImage->lHeaderBufferSize;
        uint32_t lFlashBase = pstOtaImage->lBaseAddress + pstOtaImage->stHeader.header_length;

        memcpy(pstImageData->data, (const void *)(lFlashBase + lSubElementOffset), pstImageData->data_size);

        LOG_INFO_APP( "\x1b[A[OTA] Transfer from memory address 0x%08X", ( pstOtaImage->lBaseAddress + pstImageData->file_offset ) );
    }

    return ZCL_STATUS_SUCCESS;
}

/**
 *
 */
static enum ZclStatusCodeT AppZbOtaServer_ImageUpgradeEndReqCallback( struct ZbZclOtaImageDefinition *pstImageDefinition, enum ZclStatusCodeT eUpdateStatus,
                                                                      struct ZbZclOtaEndResponseTimes *pstEndResponseTimes, struct ZbZclAddrInfoT *pstSourceInfo,
                                                                      void *arg )
{
    OtaServerImage_st  * pstOtaImage = (OtaServerImage_st *)arg;
    double                      dlUpgrateTime, dlUpgrateTroughput = 0;

    /* Save Update End Time */
    pstOtaImage->lUpdateTimeEnd = HAL_GetTick();

    LOG_INFO_APP("[OTA] *** STAGE 3: UPGRADE END REQUEST ***");

    /* Show upgrade end status */
    switch ( eUpdateStatus )
    {
        case ZCL_STATUS_SUCCESS:
          dlUpgrateTime = ( pstOtaImage->lUpdateTimeEnd - pstOtaImage->lUpdateTimeStart ) / 1000u;
          if ( dlUpgrateTime != 0u )
          {
            dlUpgrateTroughput = (((double)pstOtaImage->stHeader.total_image_size / dlUpgrateTime ) / 1000u) * 8u;
          }

          LOG_INFO_APP( "[OTA] Upgrade End status : SUCCESS");
          LOG_INFO_APP( "  Image download timing data:" );
          LOG_INFO_APP( "  Download start time (ms)    : %u", pstOtaImage->lUpdateTimeStart );
          LOG_INFO_APP( "  Download end time (ms)      : %u", pstOtaImage->lUpdateTimeEnd );
          LOG_INFO_APP( "  Download elapsed time (s)   : %.2f", dlUpgrateTime );
          LOG_INFO_APP( "  Download throughput (kbit/s): %.2f", dlUpgrateTroughput );

          // for OTA test case OTA-TC-08C, we set upgrade time to 0xFFFFFFFF which means "wait for upgrade command".
          pstEndResponseTimes->upgrade_time  = 10;

          if ( ( pstEndResponseTimes->current_time != 0u ) && ( pstEndResponseTimes->upgrade_time != 0xFFFFFFFFu ) )
          {
            LOG_INFO_APP( "  Client Current Time         : %u", pstEndResponseTimes->current_time );
            LOG_INFO_APP( "  Client Upgrade Time         : %u", pstEndResponseTimes->upgrade_time );
          }
          break;

        case ZCL_STATUS_INVALID_IMAGE:
          LOG_INFO_APP( "[OTA] Upgrade End status : INVALID_IMAGE" );
          break;

        case ZCL_STATUS_REQUIRE_MORE_IMAGE:
          LOG_INFO_APP( "[OTA] Upgrade End status : REQUIRE_MORE_IMAGE" );
          break;

        case ZCL_STATUS_ABORT:
          LOG_INFO_APP( "[OTA] Upgrade End status : UpgradeEnd status ABORT" );
          break;

        default:
          LOG_ERROR_APP( "[OTA] Upgrade End status : UNKNOWN 0x%02x", eUpdateStatus );
          eUpdateStatus = ZCL_STATUS_FAILURE;
          break;
    }

    return eUpdateStatus;
}

/**
 *
 */
void AppZbOtaServer_PrintServerInformation( void )
{
    struct ZbZclOtaHeader        *pstOtaHeader;

    LOG_INFO_APP("\n[OTA] Contents of the Image:");

    pstOtaHeader = &stOtaBinaryImage.stHeader;

    LOG_INFO_APP("  Header string         : %.32s",  pstOtaHeader->header_string);
    LOG_INFO_APP("  File identifier       : 0x%08X", pstOtaHeader->file_identifier);
    LOG_INFO_APP("  Header version        : 0x%04X", pstOtaHeader->header_version);
    LOG_INFO_APP("  Header length         : %u",     pstOtaHeader->header_length);
    LOG_INFO_APP("  Header field control  : 0x%04X", pstOtaHeader->header_field_control);
    LOG_INFO_APP("  Manufacturer code     : 0x%04X", pstOtaHeader->manufacturer_code);
    LOG_INFO_APP("  Image type            : 0x%04X", pstOtaHeader->image_type);
    LOG_INFO_APP("  File version          : 0x%08X", pstOtaHeader->file_version);
    LOG_INFO_APP("  Stack version         : 0x%04X", pstOtaHeader->stack_version);
    LOG_INFO_APP("  Image size            : %u",     pstOtaHeader->total_image_size);
    LOG_INFO_APP("  Base address          : 0x%08X", stOtaBinaryImage.lBaseAddress);
    LOG_INFO_APP("  Binary size           : %u",     stOtaBinaryImage.lBinarySize);
    LOG_INFO_APP("  Binary CRC            : 0x%08X", stOtaBinaryImage.lBinaryCrc);

    if ( ( pstOtaHeader->header_field_control & ZCL_OTA_HEADER_FIELD_CONTROL_DEVICE_SPECIFIC ) != 0u )
    {
      LOG_INFO_APP("  File destination      : " LOG_DISPLAY64(), LOG_NUMBER64( pstOtaHeader->file_destination ) );
    }

    if ( ( pstOtaHeader->header_field_control & ZCL_OTA_HEADER_FIELD_CONTROL_HARDWARE_VERSIONS ) != 0u )
    {
      LOG_INFO_APP("  Minimum HW version    : %u", pstOtaHeader->min_hardware_version);
      LOG_INFO_APP("  Maximum HW version    : %u", pstOtaHeader->max_hardware_version);
    }

    if ( ( pstOtaHeader->header_field_control & ZCL_OTA_HEADER_FIELD_CONTROL_SECURITY_VERSION ) != 0u )
    {
      LOG_INFO_APP("  Security Credential version : %u", pstOtaHeader->sec_credential_version);
    }

    LOG_INFO_APP( "\n" );
}

/**
 *
 */
void AppZbOtaServer_ImageNotify( void )
{
    struct ZbApsAddrT     stDestination;
    struct ZbZclOtaHeader *pstOtaHeader;
    struct ZbZclOtaImageDefinition stImageDefinition;

    pstOtaHeader = &stOtaBinaryImage.stHeader;

    memset(&stDestination, 0, sizeof(stDestination));
    stDestination.mode = ZB_APSDE_ADDRMODE_SHORT;
    stDestination.endpoint = 255;
    stDestination.nwkAddr = 0xFFFF;

    LOG_INFO_APP("[OTA] Broadcasting Image Notify ...");

    memset(&stImageDefinition, 0, sizeof(stImageDefinition));
    stImageDefinition.manufacturer_code = pstOtaHeader->manufacturer_code;
    stImageDefinition.image_type = pstOtaHeader->image_type;
    stImageDefinition.file_version = pstOtaHeader->file_version;

    enum ZclStatusCodeT eStatus = ZbZclOtaServerImageNotifyReq( pOtaServer, &stDestination, ZCL_OTA_NOTIFY_TYPE_FILE_VERSION, 0, &stImageDefinition);
    if (eStatus != ZCL_STATUS_SUCCESS)
    {
        LOG_INFO_APP("  Image notify failed: 0x%02X", eStatus);
    }
}

/**
 *
 */
void AppZbOtaServer_TimeoutForRequestUpdate( void * arg )
{
  UTIL_TIMER_Stop( &stTimerRequestUpdateWaitId );

  /* Launch Start Client Update */
  UTIL_SEQ_SetTask( TASK_ZIGBEE_OTA_REQUEST_UPGRADE, CFG_TASK_PRIO_OTA_REQUEST_UPDATE );
}

/**
 *
 */
void AppZbOtaServer_StartClientUpdate( void )
{
  enum ZclStatusCodeT   eStatus;

  stOtaServerReset.stResponseTimes.upgrade_time = 10;

  LOG_INFO_APP("[OTA] Send ServerUpgradeEndRespUnsolic ...");

  eStatus = ZbZclOtaServerUpgradeEndRespUnsolic( pOtaServer, &stOtaServerReset.stDest, &stOtaServerReset.stImageDefinition, &stOtaServerReset.stResponseTimes );
  if (eStatus != ZCL_STATUS_SUCCESS)
  {
      LOG_INFO_APP("  StartClientUpdate failed: 0x%02X", eStatus);
  }
}
