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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_ZIGBEE_OTA_DEFINES_H
#define APP_ZIGBEE_OTA_DEFINES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

/* Exported defines ------------------------------------------------------------*/

/* FUOTA defines for Endpoint/Cluster */
#define FUOTA_ENDPOINT                          1u
#define FUOTA_PROFILE                           ZCL_PROFILE_HOME_AUTOMATION
#define FUOTA_CLUSTER_ID                        ZCL_CLUSTER_OTA_UPGRADE
#define FUOTA_CLUSTER_CLIENT_NAME               "OTA Client"
#define FUOTA_CLUSTER_SERVER_NAME               "OTA Server"
  
#define FUOTA_STACK_VALIDATION                  1                 /* Indicate that OTA use ZB Stack Validation (ECDSA). */
  
/* FUOTA specific defines */
#define FUOTA_MAGIC_KEYWORD_TAG_APPLI           0x0BEEF11Eu       /* Keyword found on the OTA TAG definition, at the begin of Binary to download */
#define FUOTA_MAGIC_KEYWORD_END_APPLI           0xF11E0BEEu       /* Keyword found at the end of Binary Application */

#define FUOTA_FW_APP_MANUFACTURER_CODE          0x1041u           /* ST Tests */
#define FUOTA_FW_APP_IMAGE_TYPE                 (uint16_t)0x01u   /* FW Application Image Type. */ 
#define FUOTA_FW_APP_FILE_VERSION               (uint16_t)0x1800u /* Current FW Application Version : v1.8.0.0 */
//#define FUOTA_FW_APP_HARDWARE_VERSION           (uint16_t)0x01u   /* STM3WBA5x Application Processor. */
#define FUOTA_FW_APP_HARDWARE_VERSION           (uint16_t)0x02u   /* STM3WBA5x Application Processor. */
  
/* Flash Memory defines */
#define FUOTA_APP_DOWNLOAD_BINARY_ADDRESS       0x08080000        /* Application Download Binary Address. For STM32WBA5x. */
#define FUOTA_APP_DOWNLOAD_BINARY_BANK          FLASH_BANK_1      /* Application Download Binary in the First Bank for STM32WBA5x */

#define FUOTA_APP_FW_BINARY_ADDRESS             0x08006000        /* Application FW Binary Address */
#define FUOTA_APP_FW_TAG_OFFSET                 0x400u            /* Offset of the OTA TAFG on the FW Application */
#define FUOTA_APP_FW_TAG_ADDRESS                ( FUOTA_APP_FW_BINARY_ADDRESS + FUOTA_APP_OTA_TAG_OFFSET )         /* Address of the OTA tag in flash for Application binary */

#define FUOTA_CURRENT_FLASH_SIZE                FLASH_SIZE        /* By default, Flash Size of 1 MB. */
#define FUOTA_CURRENT_FLASH_BANK_SIZE           ( FUOTA_CURRENT_FLASH_SIZE >> 1u )    /* Flash Bank is Half of Flash Size */

#define FUOTA_RAM_BUFFER_SIZE                   (1024u)           /* The RAM buffer size for temporary image data storage before transferring to flash */
  
#define FUOTA_OTA_REBOOT_MSG                    ( *(uint32_t *)( SRAM1_BASE ) )
  
/* Others defines */
#define FUOTA_ST_MANUFACTURER_NAME              "STMicroelectronics"
#define FUOTA_ST_HARDWARE_NAME                  "STM32WBAxx"

  
/* Define list of reboot reason (ZCL OTA) -------------------------------------------*/
#define FUOTA_REBOOT_ON_APP_FW                  0x00u             /* Reboot on the downloaded Firmware */
#define FUOTA_REBOOT_ON_UPDATE_FW               0xCAFEDECAu       /* Reboot on the Update FW */


/* Exported types ------------------------------------------------------------*/

/* Exported constants ------------------------------------------------------- */

/* External variables ------------------------------------------------------- */

/* Exported macros -----------------------------------------------------------*/

/* Exported functions prototypes -------------------------------- */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* APP_ZIGBEE_OTA_DEFINES_H */
