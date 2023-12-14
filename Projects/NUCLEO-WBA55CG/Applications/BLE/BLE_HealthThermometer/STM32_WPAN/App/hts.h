/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service2.h
  * @author  MCD Application Team
  * @brief   Header for service2.c
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
#ifndef HTS_H
#define HTS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported defines ----------------------------------------------------------*/
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
  /* USER CODE BEGIN Service2_CharOpcode_t */

  /* USER CODE END Service2_CharOpcode_t */
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
  /* USER CODE BEGIN Service2_OpcodeEvt_t */

  /* USER CODE END Service2_OpcodeEvt_t */
  HTS_BOOT_REQUEST_EVT
} HTS_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service2_Data_t */

  /* USER CODE END Service2_Data_t */
} HTS_Data_t;

typedef struct
{
  HTS_OpcodeEvt_t       EvtOpcode;
  HTS_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service2_NotificationEvt_t */
  uint16_t  RangeInterval;
  /* USER CODE END Service2_NotificationEvt_t */
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
