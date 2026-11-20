/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dis.h
  * @author  MCD Application Team
  * @brief   Header for dis.c
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
#ifndef DIS_H
#define DIS_H

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

/** 1 Service for dis */
#define DIS_NUM_GATT_SERVICES (1)

/**
  * Num_Attributes = 0 for dis (service declaration not counted in attributes)
  *                + 2 for MANS
  *                + 2 for SYID
  *                + 2 for MONS
  *                + 2 for SNS
  *                + 2 for HRS
  *                + 2 for FRS
  *                = 12
**/
#define DIS_NUM_GATT_ATTRIBUTES (12)

/**
  * Size_Attributes = 0 for dis (service declaration not counted in attributes)
  *                 + 5 for MANS declaration + 32 for MANS size
  *                 + 5 for SYID declaration + 32 for SYID size
  *                 + 5 for MONS declaration + 32 for MONS size
  *                 + 5 for SNS declaration + 32 for SNS size
  *                 + 5 for HRS declaration + 32 for HRS size
  *                 + 5 for FRS declaration + 32 for FRS size
  *                 = 222 + 0 * CFG_BLE_NUM_LINK
**/
#define DIS_ATT_VALUE_ARRAY_SIZE  (222 + 0 * CFG_BLE_NUM_LINK)
/* USER CODE BEGIN ED */

/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  DIS_MANS,
  DIS_SYID,
  DIS_MONS,
  DIS_SNS,
  DIS_HRS,
  DIS_FRS,
  /* USER CODE BEGIN Service1_CharOpcode_t */

  /* USER CODE END Service1_CharOpcode_t */
  DIS_CHAROPCODE_LAST
} DIS_CharOpcode_t;

typedef enum
{
  DIS_MANS_READ_EVT,
  DIS_SYID_READ_EVT,
  DIS_MONS_READ_EVT,
  DIS_SNS_READ_EVT,
  DIS_HRS_READ_EVT,
  DIS_FRS_READ_EVT,
  /* USER CODE BEGIN Service1_OpcodeEvt_t */

  /* USER CODE END Service1_OpcodeEvt_t */
  DIS_BOOT_REQUEST_EVT
} DIS_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service1_Data_t */

  /* USER CODE END Service1_Data_t */
} DIS_Data_t;

typedef struct
{
  DIS_OpcodeEvt_t       EvtOpcode;
  DIS_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service1_NotificationEvt_t */

  /* USER CODE END Service1_NotificationEvt_t */
} DIS_NotificationEvt_t;

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
void DIS_Init(void);
void DIS_Notification(DIS_NotificationEvt_t *p_Notification);
tBleStatus DIS_UpdateValue(DIS_CharOpcode_t CharOpcode, DIS_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*DIS_H */
