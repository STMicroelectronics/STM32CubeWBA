/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    STM32_WPAN
  * @author  MCD Application Team
  * @brief   Header for STM32_WPAN
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#ifndef OTA_H
#define OTA_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ble_types.h"
#include "ble_core.h"
#include "svc_ctl.h"
/* USER CODE BEGIN Includes */
#define OTA_RAW_DATA_SIZE    (248)
/* USER CODE END Includes */

/* Exported defines ----------------------------------------------------------*/
/* USER CODE BEGIN ED */

/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  OTA_BASE_ADR,
  OTA_CONF,
  OTA_RAW_DATA,
  /* USER CODE BEGIN Service2_CharOpcode_t */

  /* USER CODE END Service2_CharOpcode_t */
  OTA_CHAROPCODE_LAST
} OTA_CharOpcode_t;

typedef enum
{
  OTA_BASE_ADR_WRITE_NO_RESP_EVT,
  OTA_CONF_INDICATE_ENABLED_EVT,
  OTA_CONF_INDICATE_DISABLED_EVT,
  OTA_RAW_DATA_WRITE_NO_RESP_EVT,
  /* USER CODE BEGIN Service2_OpcodeEvt_t */
  OTA_CONF_EVT,
  OTA_READY_EVT,
  /* USER CODE END Service2_OpcodeEvt_t */
  OTA_BOOT_REQUEST_EVT
} OTA_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service2_Data_t */

  /* USER CODE END Service2_Data_t */
} OTA_Data_t;

typedef struct
{
  OTA_OpcodeEvt_t       EvtOpcode;
  OTA_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service2_NotificationEvt_t */

  /* USER CODE END Service2_NotificationEvt_t */
} OTA_NotificationEvt_t;

/* USER CODE BEGIN ET */

typedef enum
{
  OTA_BASE_ADDR_ID,
  OTA_CONF_ID,
  OTA_RAW_DATA_ID,
  OTA_CONF_EVENT_ID,
  OTA_READY_ID,
 } OTA_ChardId_t;

typedef enum
{
  OTA_STOP_ALL_UPLOAD          = 0x00,
  OTA_USER_CONF_UPLOAD         = 0x01,
  OTA_APPLICATION_UPLOAD       = 0x02,
  OTA_END_FILE_TRANSFER        = 0x06,
  OTA_UPLOAD_FINISHED          = 0x07,
  OTA_CANCEL_UPLOAD            = 0x08,
} OTA_Command_t;

typedef enum
{
  OTA_REBOOT_CONFIRMED          = 0x01,
  OTA_READY_TO_RECEIVE_FILE     = 0x02,
  OTA_NOT_READY_TO_RECEIVE_FILE = 0x03,
  OTA_FW_NOT_VALID              = 0x04,
} OTA_Indication_Msg_t;

typedef struct
{
  uint8_t             *pPayload;
  OTA_ChardId_t        ChardId;
  uint16_t             ValueLength;
} OTA_Notification_t;

typedef __PACKED_STRUCT
{
  OTA_Command_t   Command;      /* [0:7] command to perform */
  uint8_t         Base_Addr[3]; /* [8:31] base address*/
  uint8_t         Sectors;      /* [32:39] number of sector to erase*/
} OTA_Base_Addr_Event_Format_t;

typedef __PACKED_STRUCT
{
  uint8_t Raw_Data[OTA_RAW_DATA_SIZE];
} OTA_Raw_Data_Event_Format_t;

typedef __PACKED_STRUCT
{
  aci_gatt_server_confirmation_event_rp0    Conf_Event;
} OTA_Conf_Event_Format_t;

typedef __PACKED_STRUCT
{
  OTA_Indication_Msg_t Conf_Msg;
} OTA_Conf_Char_Format_t;
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
void OTA_Init(void);
void OTA_Notification(OTA_NotificationEvt_t *p_Notification);
tBleStatus OTA_UpdateValue(OTA_CharOpcode_t CharOpcode, OTA_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*OTA_H */
