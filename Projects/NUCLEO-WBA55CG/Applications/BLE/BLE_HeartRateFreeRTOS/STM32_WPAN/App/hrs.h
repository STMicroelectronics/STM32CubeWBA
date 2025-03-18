/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    hrs.h
  * @author  MCD Application Team
  * @brief   Header for hrs.c
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
#ifndef HRS_H
#define HRS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported defines ----------------------------------------------------------*/
/* USER CODE BEGIN ED */
#define HRS_MAX_NBR_RR_INTERVAL_VALUES                     (9)
/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  HRS_HRME,
  HRS_BSL,
  HRS_HRCP,
  /* USER CODE BEGIN Service1_CharOpcode_t */

  /* USER CODE END Service1_CharOpcode_t */
  HRS_CHAROPCODE_LAST
} HRS_CharOpcode_t;

typedef enum
{
  HRS_HRME_NOTIFY_ENABLED_EVT,
  HRS_HRME_NOTIFY_DISABLED_EVT,
  HRS_BSL_READ_EVT,
  HRS_HRCP_WRITE_EVT,
  /* USER CODE BEGIN Service1_OpcodeEvt_t */
  HRS_RESET_ENERGY_EXPENDED_EVT,

  /* USER CODE END Service1_OpcodeEvt_t */
  HRS_BOOT_REQUEST_EVT
} HRS_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service1_Data_t */

  /* USER CODE END Service1_Data_t */
} HRS_Data_t;

typedef struct
{
  HRS_OpcodeEvt_t       EvtOpcode;
  HRS_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service1_NotificationEvt_t */

  /* USER CODE END Service1_NotificationEvt_t */
} HRS_NotificationEvt_t;

/* USER CODE BEGIN ET */
typedef struct{
    uint16_t    MeasurementValue;
    uint16_t    EnergyExpended;
    uint16_t    aRRIntervalValues[HRS_MAX_NBR_RR_INTERVAL_VALUES];
    uint8_t     NbreOfValidRRIntervalValues;
    uint8_t     Flags;
}HRS_MeasVal_t;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
typedef enum
{
  HRS_HRM_VALUE_FORMAT_UINT16 = 0x01,
  HRS_HRM_SENSOR_CONTACTS_PRESENT = 0x02,
  HRS_HRM_SENSOR_CONTACTS_SUPPORTED = 0x04,
  HRS_HRM_ENERGY_EXPENDED_PRESENT = 0x08,
  HRS_HRM_RR_INTERVAL_PRESENT = 0x10
}HRS_HrmFlags_t;

typedef enum
{
  HRS_BODY_SENSOR_LOCATION_OTHER = 0,
  HRS_BODY_SENSOR_LOCATION_CHEST = 1,
  HRS_BODY_SENSOR_LOCATION_WRIST = 2,
  HRS_BODY_SENSOR_LOCATION_FINGER = 3,
  HRS_BODY_SENSOR_LOCATION_HAND = 4,
  HRS_BODY_SENSOR_LOCATION_EAR_LOBE = 5,
  HRS_BODY_SENSOR_LOCATION_FOOT = 6
}HRS_BodySensorLocation_t;

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void HRS_Init(void);
void HRS_Notification(HRS_NotificationEvt_t *p_Notification);
tBleStatus HRS_UpdateValue(HRS_CharOpcode_t CharOpcode, HRS_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*HRS_H */
