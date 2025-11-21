/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_zigbee_cluster.h
  * Description        : Header for Zigbee Application and it cluster.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_ZIGBEE_ENDPOINT_H
#define APP_ZIGBEE_ENDPOINT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "app_common.h"
#include "zigbee.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported defines ------------------------------------------------------------*/
#define	CLUSTER_NB_MAX                          6u          /* Maximum number of Clusters in this application */

/* USER CODE BEGIN ED */
/* ZCL OTA FUOTA specific defines ----------------------------------------------------*/
#define FUOTA_MAGIC_KEYWORD_RESET_OTA           0xCAFEDECAu       /* Keyword to reset in OTA Mode */
  
#define FUOTA_MAGIC_KEYWORD_M4_APP              0x94448A29u       /* Keyword found at the end of Zigbee Ota file for M4 Application Processor binary */
#define FUOTA_MAGIC_KEYWORD_M33_APP             0xBF806133u       /* Keyword found at the end of Zigbee Ota file for M33 Application Processor binary */
#define FUOTA_MAGIC_KEYWORD_COPRO_WIRELESS      0xD3A12C5Eu       /* Keyword found at the end of encrypted file for Wireless Coprocessor binary. For WB Compatibility */

#define CFG_APP_START_SECTOR_INDEX              0x40u             /* Define the start address where the application shall be located */
#define FUOTA_APP_FW_BINARY_ADDRESS             ( FLASH_BASE + ( CFG_APP_START_SECTOR_INDEX * FLASH_PAGE_SIZE ) )     /* Address for Application Processor FW Update */
#define FUOTA_COPRO_FW_BINARY_ADDRESS           ( FLASH_BASE + ( CFG_APP_START_SECTOR_INDEX * FLASH_PAGE_SIZE ) )     /* Address for Wireless Coprocessor FW Update. For WB Compatibility */ */

#define FUOTA_M4_APP_OTA_TAG_OFFSET             0x140u            /* Offset of the OTA tag from the base address in flash for M4 Applciation Processor binary */
#define FUOTA_M33_APP_OTA_TAG_OFFSET            0x400u            /* Offset of the OTA tag from the base address in flash for M33 Applciation Processor binary */
#define FUOTA_M4_APP_OTA_TAG_ADDRESS            ( FUOTA_APP_FW_BINARY_ADDRESS + FUOTA_M4_APP_OTA_TAG_OFFSET )        /* Address of the OTA tag in flash for M4 Application Processor binary */
#define FUOTA_M33_APP_OTA_TAG_ADDRESS           ( FUOTA_APP_FW_BINARY_ADDRESS + FUOTA_M33_APP_OTA_TAG_OFFSET )       /* Address of the OTA tag in flash for M33 Application Processor binary */

/* ZCL OTA specific defines ------------------------------------------------------*/
#define ST_ZIGBEE_MANUFACTURER_CODE             0x1041u
#define RAM_FIRMWARE_BUFFER_SIZE                1024u

#define CURRENT_M0_HARDWARE_VERSION             0x01u       /* For WB compatibility */  
#define CURRENT_M4_HARDWARE_VERSION             0x02u       /* For WB compatibility */  
#define CURRENT_M33_HARDWARE_VERSION            0x03u

#define CURRENT_FW_COPRO_WIRELESS_FILE_VERSION  0x01u       /* Current Wireless Coprocessor FW Version. For WB compatibility  */
#define CURRENT_FW_M4_APP_FILE_VERSION          0x01u       /* Current M4 Application Processor FW Version. For WB compatibility  */
#define CURRENT_FW_M33_APP_FILE_VERSION         0x01u       /* Current M33 Application Processor FW Version. */

#define FILE_VERSION_FW_COPRO_WIRELESS          0x02u       /* New Wireless Coprocessor FW version. For WB compatibility  */
#define FILE_VERSION_FW_M4_APP                  0x02u       /* New M4 Application Processor FW version. For WB compatibility */
#define FILE_VERSION_FW_M33_APP                 0x02u       /* New M33 Application Processor FW version. */

#define IMAGE_TYPE_FW_COPRO_WIRELESS            0x01u       /* Wireless Coprocessor binary. For WB compatibility  */
#define IMAGE_TYPE_FW_M4_APP                    0x02u       /* M4 Application Processor binary. For WB compatibility  */
#define IMAGE_TYPE_FW_M33_APP                   0x03u       /* M33 Application Processor binary. */

/* Define list of reboot reason (ZCL OTA) -------------------------------------------*/
#define CFG_REBOOT_ON_DOWNLOADED_FW             0x00u                           /* Reboot on the downloaded Firmware */
#define CFG_REBOOT_ON_OTA_FW                    FUOTA_MAGIC_KEYWORD_RESET_OTA   /* Reboot on OTA FW */
//#define CFG_REBOOT_ON_CPU2_UPGRADE              0x02u                         /* Reboot on OTA FW to download CPU2. For WB Compatibility */


/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
enum APP_ZIGBEE_OtaFileTypeDef_t
{
  fileType_COPRO_WIRELESS   = IMAGE_TYPE_FW_COPRO_WIRELESS,   /* For WB Compatibility */
  fileType_M4_APP           = IMAGE_TYPE_FW_M4_APP,           /* For WB Compatibility */
  fileType_M33_APP          = IMAGE_TYPE_FW_M33_APP,
};

/* USER CODE END ET */

/* Exported constants ------------------------------------------------------- */
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables ------------------------------------------------------- */
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes -------------------------------- */
extern void       APP_ZIGBEE_ApplicationInit              ( void );
extern void       APP_ZIGBEE_ApplicationStart             ( void );
extern void	      APP_ZIGBEE_PersistenceStartup           ( void );
extern void       APP_ZIGBEE_ConfigEndpoints              ( void );
extern bool       APP_ZIGBEE_ConfigGroupAddr              ( void );

extern void       APP_ZIGBEE_GetStartupConfig             ( struct ZbStartupT * pstConfig );
extern void       APP_ZIGBEE_SetNewDevice                 ( uint16_t iShortAddress, uint64_t dlExtendedAddress, uint8_t cCapability );

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* APP_ZIGBEE_ENDPOINT_H */
