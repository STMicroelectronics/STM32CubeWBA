/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    HTS.h
  * @author  MCD Application Team
  * @brief   Header for HTS.c
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
#ifndef HTS_H
#define HTS_H

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

/** 1 Service for HTS */
#define HTS_NUM_GATT_SERVICES (1)

/**
  * Num_Attributes = 0 for HTS (service declaration not counted in attributes)
  *                + 2 for TEMM
  *                + 2 for MNbS
  *                + 2 for INT
  *                + 2 for MEI
  *                + 1 for TEMM CCCD
  *                + 1 for INT CCCD
  *                + 1 for MEI CCCD
  *                = 11
**/
#define HTS_NUM_GATT_ATTRIBUTES (11)

/**
  * Size_Attributes = 0 for HTS (service declaration not counted in attributes)
  *                 + 5 for TEMM declaration + 13 for TEMM size
  *                 + 5 for MNbS declaration + 1 for MNbS size
  *                 + 5 for INT declaration + 13 for INT size
  *                 + 5 for MEI declaration + 2 for MEI size
  *                 + 2 for TEMM CCCD * CFG_BLE_NUM_LINK
  *                 + 2 for INT CCCD * CFG_BLE_NUM_LINK
  *                 + 2 for MEI CCCD * CFG_BLE_NUM_LINK
  *                 = 49 + 6 * CFG_BLE_NUM_LINK
**/
#define HTS_ATT_VALUE_ARRAY_SIZE  (49 + 6 * CFG_BLE_NUM_LINK)
/* USER CODE BEGIN ED */
#define BLE_CFG_HTS_TEMPERATURE_INTERVAL_MIN_VALUE               1  /**< Min Interval Value supported */
#define BLE_CFG_HTS_TEMPERATURE_INTERVAL_MAX_VALUE               UINT16_MAX /**< Max Interval Value supported */
/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  HTS_TEMM,
  HTS_MNBS,
  HTS_INT,
  HTS_MEI,
  /* USER CODE BEGIN Service1_CharOpcode_t */

  /* USER CODE END Service1_CharOpcode_t */
  HTS_CHAROPCODE_LAST
} HTS_CharOpcode_t;

typedef enum
{
  HTS_TEMM_INDICATE_ENABLED_EVT,
  HTS_TEMM_INDICATE_DISABLED_EVT,
  HTS_MNBS_READ_EVT,
  HTS_INT_NOTIFY_ENABLED_EVT,
  HTS_INT_NOTIFY_DISABLED_EVT,
  HTS_MEI_READ_EVT,
  HTS_MEI_WRITE_EVT,
  HTS_MEI_INDICATE_ENABLED_EVT,
  HTS_MEI_INDICATE_DISABLED_EVT,
  /* USER CODE BEGIN Service1_OpcodeEvt_t */

  /* USER CODE END Service1_OpcodeEvt_t */
  HTS_BOOT_REQUEST_EVT
} HTS_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service1_Data_t */

  /* USER CODE END Service1_Data_t */
} HTS_Data_t;

typedef struct
{
  HTS_OpcodeEvt_t       EvtOpcode;
  HTS_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service1_NotificationEvt_t */
  uint16_t  RangeInterval;
  /* USER CODE END Service1_NotificationEvt_t */
} HTS_NotificationEvt_t;

/* USER CODE BEGIN ET */
typedef enum
{
  NO_FLAGS = 0,
  VALUE_UNIT_FAHRENHEIT = (1<<0),
  SENSOR_TIME_STAMP_PRESENT = (1<<1),
  SENSOR_TEMPERATURE_TYPE_PRESENT = (1<<2),
} HTS_TM_Flags_t;

typedef enum
{
  TT_Armpit = 1,
  TT_Body = 2,
  TT_Ear = 3,
  TT_Finger = 4,
  TT_Gastro_intestinal_Tract = 5,
  TT_Mouth = 6,
  TT_Rectum = 7,
  TT_Toe = 8,
  TT_Tympanum = 9
} HTS_Temperature_Type_t;

typedef enum
{
  HTS_MEASUREMENT_INTERVAL_RECEIVED_EVT,
  HTS_MEASUREMENT_IND_ENABLED_EVT,
  HTS_MEASUREMENT_IND_DISABLED_EVT,
  HTS_MEASUREMENT_INTERVAL_IND_ENABLED_EVT,
  HTS_MEASUREMENT_INTERVAL_IND_DISABLED_EVT,
  HTS_INTERMEDIATE_TEMPERATURE_NOT_ENABLED_EVT,
  HTS_INTERMEDIATE_TEMPERATURE_NOT_DISABLED_EVT,
} HTS_App_Opcode_Notification_evt_t;

typedef struct
{
  HTS_App_Opcode_Notification_evt_t  HTS_Evt_Opcode;
}HTS_App_Notification_evt_t;

typedef struct
{
  uint16_t  Year;
  uint8_t   Month;
  uint8_t   Day;
  uint8_t   Hours;
  uint8_t   Minutes;
  uint8_t   Seconds;
}HTS_TimeStamp_t;

typedef struct
{
  uint32_t MeasurementValue;
  HTS_TimeStamp_t TimeStamp;
  HTS_Temperature_Type_t  TemperatureType;
  uint8_t Flags;
}HTS_TemperatureValue_t;
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
void HTS_Init(void);
void HTS_Notification(HTS_NotificationEvt_t *p_Notification);
tBleStatus HTS_UpdateValue(HTS_CharOpcode_t CharOpcode, HTS_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*HTS_H */
