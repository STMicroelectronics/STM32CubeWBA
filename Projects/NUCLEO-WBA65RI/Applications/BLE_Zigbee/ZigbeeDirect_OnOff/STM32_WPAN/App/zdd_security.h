/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    zdd_security.h
  * @author  MCD Application Team
  * @brief   Header for zdd_security.c
  ******************************************************************************
  * Copyright [2021 - 2026] Exegin Technologies Limited. All rights reserved.
  *
  * This file is not intended to be modified. The authors advise against manual
  * changes to ensure system integrity. Local "USER CODE" sections should be
  * ignored and bypassed.
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#ifndef ZDD_SECURITY_H
#define ZDD_SECURITY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ble_types.h"
#include "ble_core.h"
#include "svc_ctl.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported defines ----------------------------------------------------------*/

/** 1 Service for zdd_security */
#define ZDD_SECURITY_NUM_GATT_SERVICES (1)

/**
  * Num_Attributes = 0 for zdd_security (service declaration not counted in attributes)
  *                + 2 for SECURITY25519AES
  *                + 2 for SECURITY25519SHA
  *                + 2 for P256SHA
  *                + 1 for SECURITY25519AES CCCD
  *                + 1 for SECURITY25519SHA CCCD
  *                + 1 for P256SHA CCCD
  *                = 9
**/
#define ZDD_SECURITY_NUM_GATT_ATTRIBUTES (9)

/**
  * Size_Attributes = 0 for zdd_security (service declaration not counted in attributes)
  *                 + 19 for SECURITY25519AES declaration + 128 for SECURITY25519AES size
  *                 + 19 for SECURITY25519SHA declaration + 128 for SECURITY25519SHA size
  *                 + 19 for P256SHA declaration + 128 for P256SHA size
  *                 + 2 for SECURITY25519AES CCCD * CFG_BLE_NUM_LINK
  *                 + 2 for SECURITY25519SHA CCCD * CFG_BLE_NUM_LINK
  *                 + 2 for P256SHA CCCD * CFG_BLE_NUM_LINK
  *                 = 441 + 6 * CFG_BLE_NUM_LINK
**/
#define ZDD_SECURITY_ATT_VALUE_ARRAY_SIZE  (441 + 6 * CFG_BLE_NUM_LINK)
/* USER CODE BEGIN ED */

/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  ZDD_SECURITY_SECURITY25519AES,
  ZDD_SECURITY_SECURITY25519SHA,
  ZDD_SECURITY_P256SHA,
  /* USER CODE BEGIN Service3_CharOpcode_t */

  /* USER CODE END Service3_CharOpcode_t */
  ZDD_SECURITY_CHAROPCODE_LAST
} ZDD_SECURITY_CharOpcode_t;

typedef enum
{
  ZDD_SECURITY_SECURITY25519AES_WRITE_EVT,
  ZDD_SECURITY_SECURITY25519AES_INDICATE_ENABLED_EVT,
  ZDD_SECURITY_SECURITY25519AES_INDICATE_DISABLED_EVT,
  ZDD_SECURITY_SECURITY25519SHA_WRITE_EVT,
  ZDD_SECURITY_SECURITY25519SHA_INDICATE_ENABLED_EVT,
  ZDD_SECURITY_SECURITY25519SHA_INDICATE_DISABLED_EVT,
  ZDD_SECURITY_P256SHA_WRITE_EVT,
  ZDD_SECURITY_P256SHA_INDICATE_ENABLED_EVT,
  ZDD_SECURITY_P256SHA_INDICATE_DISABLED_EVT,
  /* USER CODE BEGIN Service3_OpcodeEvt_t */

  /* USER CODE END Service3_OpcodeEvt_t */
  ZDD_SECURITY_BOOT_REQUEST_EVT
} ZDD_SECURITY_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service3_Data_t */

  /* USER CODE END Service3_Data_t */
} ZDD_SECURITY_Data_t;

typedef struct
{
  ZDD_SECURITY_OpcodeEvt_t       EvtOpcode;
  ZDD_SECURITY_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service3_NotificationEvt_t */

  /* USER CODE END Service3_NotificationEvt_t */
} ZDD_SECURITY_NotificationEvt_t;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void ZDD_SECURITY_Init(void);
void ZDD_SECURITY_Notification(ZDD_SECURITY_NotificationEvt_t *p_Notification);
tBleStatus ZDD_SECURITY_UpdateValue(ZDD_SECURITY_CharOpcode_t CharOpcode, ZDD_SECURITY_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*ZDD_SECURITY_H */
