/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_zigbee_ota_defines.h
  * Description        : Header for defines all OTA information
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

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef APP_ZIGBEE_OTA_DEFINES_H
#define APP_ZIGBEE_OTA_DEFINES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */
#include "zcl/general/zcl.ota.h"

/* Exported defines --------------------------------------------------------- */

/* FUOTA defines for Endpoint/Cluster */
#define FUOTA_STACK_VALIDATION                  1                 /* Indicate that OTA use ZB Stack Validation (ECDSA). */

/* FUOTA specific defines */
#define FUOTA_MAGIC_KEYWORD_TAG_APPLI           0x0BEEF11Eu       /* Keyword found on the OTA TAG definition, at the begin of Binary to download */
#define FUOTA_MAGIC_KEYWORD_END_APPLI           0xF11E0BEEu       /* Keyword found at the end of Binary Application */

#define FUOTA_FW_APP_MANUFACTURER_CODE          0x1041u           /* ST Tests */
#define FUOTA_FW_APP_IMAGE_TYPE                 (uint16_t)0x01u   /* FW Application Image Type. */
#define FUOTA_FW_APP_FILE_VERSION               (uint16_t)0x1800u /* Current FW Application Version : v1.8.0.0 */
#define FUOTA_FW_APP_HARDWARE_VERSION           (uint16_t)0x02u   /* STM3WBA5x Application Processor. */

/* Flash Memory defines */
#define FUOTA_APP_DOWNLOAD_BINARY_BANK          FLASH_BANK_1      /* Application Download Binary in the First Back */
#define FUOTA_APP_DOWNLOAD_BINARY_ADDRESS       0x08080000        /* Application Download Binary Address. For STM32WBA5x. */

#define FUOTA_APP_FW_BINARY_ADDRESS             0x08006000        /* Application FW Binary Address */
#define FUOTA_APP_FW_TAG_OFFSET                 0x400u            /* Offset of the OTA TAG on the FW Application */
#define FUOTA_APP_FW_TAG_ADDRESS                ( FUOTA_APP_FW_BINARY_ADDRESS + FUOTA_APP_OTA_TAG_OFFSET )         /* Address of the OTA tag in flash for Application binary */

#define FUOTA_CURRENT_FLASH_SIZE                FLASH_SIZE        /* By default, Flash Size of 1 MB. */
#define FUOTA_CURRENT_FLASH_BANK_SIZE           ( FUOTA_CURRENT_FLASH_SIZE >> 1u )    /* Flash Bank is Half of Flash Size */

#define FUOTA_RAM_BUFFER_SIZE                   (1024u)           /* The RAM buffer size for temporary image data storage before transferring to flash */

#define FUOTA_OTA_REBOOT_MSG                    ( *(uint32_t *)( SRAM1_BASE ) )

/* Others defines */
#define FUOTA_ST_MANUFACTURER_NAME              "STMicroelectronics"
#define FUOTA_ST_HARDWARE_NAME                  "STM32WBAxx"

/* Define list of reboot reason (ZCL OTA) ----------------------------------- */
#define FUOTA_REBOOT_ON_APP_FW                  0x00u             /* Reboot on the downloaded Firmware */
#define FUOTA_REBOOT_ON_UPDATE_FW               0xCAFEDECAu       /* Reboot on the Update FW */

/* Exported types ----------------------------------------------------------- */
/* OTA Client information */
typedef struct
{
  uint32_t    lFwEndMagicKeyAddress;                /* The FW Binary End Magic keyword Address */
  uint32_t    lBaseAddress;                         /* The image Base Address in flash */
  uint32_t    lOtaTagOffset;                        /* The OTA TAG Offset in binary file */
  uint32_t    lOtaTagMagicKeyword;                  /* The OTA TAG Magic keyword */

  uint16_t    iManufacturerCode;                    /* The manufacturer code for this device */
  uint16_t    iImageType;                           /* The image type for this device */
  uint32_t    lCurrentFileVersion;                  /* The current file version for this device */

  enum ZbZclOtaActivationPolicy eActivationPolicy;  /* Indicate if 'Server' or 'Out-of-band' activation after Download */
  enum ZbZclOtaTimeoutPolicy    eTimeOutPolicy;     /* Indicate if Upgrade after TimeOut or not. */

  uint8_t     *pCertPublicKey;                      /* Pointer on the Certificate Public Key. */
  uint16_t    iCertPublicSize;                      /* Size of the Certificate Public. */

  uint8_t     cEndpoint;                            /* Endpoint used for OTA Client. Created if not exist */
  uint16_t    iProfile;                             /* Profile used for this Endpoint */
  uint16_t    iDeviceId;                            /* DeviceID used for this Endpoint */
} AppZbOta_ClientInfo_st;

/* Exported constants ------------------------------------------------------- */

/* External variables ------------------------------------------------------- */

/* Exported macros ---------------------------------------------------------- */

/* Exported functions prototypes -------------------------------------------- */

/* For OTA Client Cluster */
bool  AppZbOtaClient_ClusterInit            ( struct ZigBeeT * pstZigbee, const AppZbOta_ClientInfo_st * pstClusterUserInfo );
void  AppZbOtaClient_ClusterRemove          ( void );
bool  AppZbOtaClient_SearchOtaServer        ( void );

/* For OTA Server Cluster */
bool  AppZbOtaServer_ClusterInit            ( struct ZigBeeT * pstZigbee, uint8_t cEndpoint, uint16_t iProfile, uint16_t iDeviceId );
bool  AppZbOtaServer_ClusterRemove          ( void );

bool  AppZbOtaServer_GetBinInfo             ( void );
void  AppZbOtaServer_PrintServerInformation ( void );
void  AppZbOtaServer_ImageNotify            ( void );
void  AppZbOtaServer_StartClientUpdate      ( void );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* APP_ZIGBEE_OTA_DEFINES_H */
