/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    zdd_tunneling.h
  * @author  MCD Application Team
  * @brief   Header for zdd_tunneling.c
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
#ifndef ZDD_TUNNELING_H
#define ZDD_TUNNELING_H

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

/** 1 Service for zdd_tunneling */
#define ZDD_TUNNELING_NUM_GATT_SERVICES (1)

/**
  * Num_Attributes = 0 for zdd_tunneling (service declaration not counted in attributes)
  *                + 2 for TUNNZDTSNPDU
  *                + 1 for TUNNZDTSNPDU CCCD
  *                = 3
**/
#define ZDD_TUNNELING_NUM_GATT_ATTRIBUTES (3)

/**
  * Size_Attributes = 0 for zdd_tunneling (service declaration not counted in attributes)
  *                 + 19 for TUNNZDTSNPDU declaration + 160 for TUNNZDTSNPDU size
  *                 + 2 for TUNNZDTSNPDU CCCD * CFG_BLE_NUM_LINK
  *                 = 179 + 2 * CFG_BLE_NUM_LINK
**/
#define ZDD_TUNNELING_ATT_VALUE_ARRAY_SIZE  (179 + 2 * CFG_BLE_NUM_LINK)
/* USER CODE BEGIN ED */

/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  ZDD_TUNNELING_TUNNZDTSNPDU,
  /* USER CODE BEGIN Service4_CharOpcode_t */

  /* USER CODE END Service4_CharOpcode_t */
  ZDD_TUNNELING_CHAROPCODE_LAST
} ZDD_TUNNELING_CharOpcode_t;

typedef enum
{
  ZDD_TUNNELING_TUNNZDTSNPDU_WRITE_EVT,
  ZDD_TUNNELING_TUNNZDTSNPDU_INDICATE_ENABLED_EVT,
  ZDD_TUNNELING_TUNNZDTSNPDU_INDICATE_DISABLED_EVT,
  /* USER CODE BEGIN Service4_OpcodeEvt_t */

  /* USER CODE END Service4_OpcodeEvt_t */
  ZDD_TUNNELING_BOOT_REQUEST_EVT
} ZDD_TUNNELING_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service4_Data_t */

  /* USER CODE END Service4_Data_t */
} ZDD_TUNNELING_Data_t;

typedef struct
{
  ZDD_TUNNELING_OpcodeEvt_t       EvtOpcode;
  ZDD_TUNNELING_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service4_NotificationEvt_t */

  /* USER CODE END Service4_NotificationEvt_t */
} ZDD_TUNNELING_NotificationEvt_t;

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
void ZDD_TUNNELING_Init(void);
void ZDD_TUNNELING_Notification(ZDD_TUNNELING_NotificationEvt_t *p_Notification);
tBleStatus ZDD_TUNNELING_UpdateValue(ZDD_TUNNELING_CharOpcode_t CharOpcode, ZDD_TUNNELING_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*ZDD_TUNNELING_H */
