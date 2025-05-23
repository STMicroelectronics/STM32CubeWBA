/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_zigbee_endpoint.h
  * Description        : Header for Zigbee Application and it endpoint.
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
#define CLUSTER_NB_MAX                          6u          /* Maximum number of Clusters in this application */

/* USER CODE BEGIN ED */

#define FUOTA_MAGIC_KEYWORD_RESET_OTA           0xCAFEDECAu       /* Keyword to reset in OTA Mode */
  
#define FUOTA_MAGIC_KEYWORD_M4_APP              0x94448A29u       /* Keyword found at the end of Zigbee Ota file for M4 Application Processor binary */
#define FUOTA_MAGIC_KEYWORD_M33_APP             0xBF806133u       /* Keyword found at the end of Zigbee Ota file for M33 Application Processor binary */
#define FUOTA_MAGIC_KEYWORD_COPRO_WIRELESS      0xD3A12C5Eu       /* Keyword found at the end of encrypted file for Wireless Coprocessor binary. For WB Compatibility */
/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

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
extern void       APP_ZIGBEE_PersistenceStartup           ( void );
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
