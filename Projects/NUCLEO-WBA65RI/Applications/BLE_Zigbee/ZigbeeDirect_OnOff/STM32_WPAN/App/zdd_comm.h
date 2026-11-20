/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    zdd_comm.h
  * @author  MCD Application Team
  * @brief   Header for zdd_comm.c
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
#ifndef ZDD_COMM_H
#define ZDD_COMM_H

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

/** 1 Service for zdd_comm */
#define ZDD_COMM_NUM_GATT_SERVICES (1)

/**
  * Num_Attributes = 0 for zdd_comm (service declaration not counted in attributes)
  *                + 2 for FORMNETWORK
  *                + 2 for JOINNETWORK
  *                + 2 for PERMITJOIN
  *                + 2 for LEAVENETWORK
  *                + 2 for COMMSTATUS
  *                + 2 for MANAGEJOINER
  *                + 2 for COMMIDENTITY
  *                + 2 for FINDBIND
  *                + 1 for COMMSTATUS CCCD
  *                = 17
**/
#define ZDD_COMM_NUM_GATT_ATTRIBUTES (17)

/**
  * Size_Attributes = 0 for zdd_comm (service declaration not counted in attributes)
  *                 + 19 for FORMNETWORK declaration + 140 for FORMNETWORK size
  *                 + 19 for JOINNETWORK declaration + 140 for JOINNETWORK size
  *                 + 19 for PERMITJOIN declaration + 12 for PERMITJOIN size
  *                 + 19 for LEAVENETWORK declaration + 12 for LEAVENETWORK size
  *                 + 19 for COMMSTATUS declaration + 80 for COMMSTATUS size
  *                 + 19 for MANAGEJOINER declaration + 60 for MANAGEJOINER size
  *                 + 19 for COMMIDENTITY declaration + 12 for COMMIDENTITY size
  *                 + 19 for FINDBIND declaration + 12 for FINDBIND size
  *                 + 2 for COMMSTATUS CCCD * CFG_BLE_NUM_LINK
  *                 = 620 + 2 * CFG_BLE_NUM_LINK
**/
#define ZDD_COMM_ATT_VALUE_ARRAY_SIZE  (620 + 2 * CFG_BLE_NUM_LINK)
/* USER CODE BEGIN ED */

/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  ZDD_COMM_FORMNETWORK,
  ZDD_COMM_JOINNETWORK,
  ZDD_COMM_PERMITJOIN,
  ZDD_COMM_LEAVENETWORK,
  ZDD_COMM_COMMSTATUS,
  ZDD_COMM_MANAGEJOINER,
  ZDD_COMM_COMMIDENTITY,
  ZDD_COMM_FINDBIND,
  /* USER CODE BEGIN Service2_CharOpcode_t */

  /* USER CODE END Service2_CharOpcode_t */
  ZDD_COMM_CHAROPCODE_LAST
} ZDD_COMM_CharOpcode_t;

typedef enum
{
  ZDD_COMM_FORMNETWORK_WRITE_EVT,
  ZDD_COMM_JOINNETWORK_WRITE_EVT,
  ZDD_COMM_PERMITJOIN_WRITE_EVT,
  ZDD_COMM_LEAVENETWORK_WRITE_EVT,
  ZDD_COMM_COMMSTATUS_READ_EVT,
  ZDD_COMM_COMMSTATUS_NOTIFY_ENABLED_EVT,
  ZDD_COMM_COMMSTATUS_NOTIFY_DISABLED_EVT,
  ZDD_COMM_MANAGEJOINER_WRITE_EVT,
  ZDD_COMM_COMMIDENTITY_READ_EVT,
  ZDD_COMM_COMMIDENTITY_WRITE_EVT,
  ZDD_COMM_FINDBIND_WRITE_EVT,
  /* USER CODE BEGIN Service2_OpcodeEvt_t */

  /* USER CODE END Service2_OpcodeEvt_t */
  ZDD_COMM_BOOT_REQUEST_EVT
} ZDD_COMM_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service2_Data_t */

  /* USER CODE END Service2_Data_t */
} ZDD_COMM_Data_t;

typedef struct
{
  ZDD_COMM_OpcodeEvt_t       EvtOpcode;
  ZDD_COMM_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service2_NotificationEvt_t */

  /* USER CODE END Service2_NotificationEvt_t */
} ZDD_COMM_NotificationEvt_t;

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
void ZDD_COMM_Init(void);
void ZDD_COMM_Notification(ZDD_COMM_NotificationEvt_t *p_Notification);
tBleStatus ZDD_COMM_UpdateValue(ZDD_COMM_CharOpcode_t CharOpcode, ZDD_COMM_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*ZDD_COMM_H */
