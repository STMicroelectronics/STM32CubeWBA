/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_zigbee.h
  * Description        : Header for Zigbee Application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_ZIGBEE_H
#define APP_ZIGBEE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "app_common.h"
#include "app_zigbee_endpoint.h"
#include "zigbee.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported defines ----------------------------------------------------------*/
/* USER CODE BEGIN ED */

/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
/* --- Zigbee Application status --- */
typedef enum
{
  APP_ZIGBEE_OK       = 0x00u,
  APP_ZIGBEE_ERROR    = 0x01u,
} APP_ZIGBEE_StatusTypeDef;

/* --- Zigbee Application Information --- */
typedef struct ZigbeeAppInfoT
{
  bool                  bHasInit;
  bool                  bInitAfterJoin;
  bool                  bPersistNotification;
  bool                  bNwkStartup;
  struct ZigBeeT        * pstZigbee;
  enum ZbStartType      eStartupControl;
  enum ZbStatusCodeT    eJoinStatus;
  uint32_t              lPersistNumWrites;
  uint32_t              lJoinDelay;
  uint64_t              dlExtendedAddress;
  /* USER CODE BEGIN ZigbeeAppInfo_t */

  /* USER CODE END ZigbeeAppInfo_t */

  struct ZbZclClusterT  * pstZbCluster[CLUSTER_NB_MAX];
} ZigbeeAppInfo_t;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants ------------------------------------------------------- */
extern const uint8_t                      sec_key_distrib_uncert[ZB_SEC_KEYSIZE];
extern const uint8_t                      sec_key_ha[ZB_SEC_KEYSIZE];

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables ------------------------------------------------------- */
extern ZigbeeAppInfo_t                    stZigbeeAppInfo;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
extern void       APP_ZIGBEE_Init                         ( void );
extern void       APP_ZIGBEE_Task                         ( void );
extern void       APP_ZIGBEE_StackLayersInit              ( void );
extern void       APP_ZIGBEE_NwkFormOrJoinTaskInit        ( void );
extern void       APP_ZIGBEE_NwkFormOrJoin                ( void );

extern void       APP_ZIGBEE_PermitJoin                   ( uint8_t cPermitJoinDelay );
extern bool       APP_ZIGBEE_IsAppliJoinNetwork           ( void );
extern void       APP_ZIGBEE_AddDeviceWithInstallCode     ( uint64_t dlExtendedAddress, uint8_t * szInstallCode, uint8_t cPermitJoinDelay );
extern uint16_t   APP_ZIGBEE_GetDisplayBindTable          ( bool bDisplay );
extern bool       APP_ZIGBEE_GetCurrentChannel            ( uint8_t * cCurrentChannel );
extern bool       APP_ZIGBEE_SetTxPower                   ( uint8_t cTxPower );
extern char *     APP_ZIGBEE_GetDisplaySecKey             ( const uint8_t * szCode, uint16_t iLength, bool bSpace );
extern void       APP_ZIGBEE_PrintGenericInfo             ( void );
extern void       APP_ZIGBEE_PrintApplicationInfo         ( void );
extern void       APP_ZIGBEE_Error                        ( uint32_t ErrId, uint32_t ErrCode );

extern void       APP_ZIGBEE_SerialCommandInstallCode     ( uint8_t * pRxBuffer, uint16_t iRxBufferSize );

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* APP_ZIGBEE_H */

