/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    BLE_Sensor.h
  * @author  MCD Application Team
  * @brief   Header for BLE_Sensor.c
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
#ifndef BLE_SENSOR_H
#define BLE_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported defines ----------------------------------------------------------*/
/* USER CODE BEGIN ED */

/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  BLE_SENSOR_MOTION_C,
  BLE_SENSOR_ENV_C,
  /* USER CODE BEGIN Service1_CharOpcode_t */

  /* USER CODE END Service1_CharOpcode_t */
  BLE_SENSOR_CHAROPCODE_LAST
} BLE_SENSOR_CharOpcode_t;

typedef enum
{
  BLE_SENSOR_MOTION_C_NOTIFY_ENABLED_EVT,
  BLE_SENSOR_MOTION_C_NOTIFY_DISABLED_EVT,
  BLE_SENSOR_ENV_C_READ_EVT,
  BLE_SENSOR_ENV_C_NOTIFY_ENABLED_EVT,
  BLE_SENSOR_ENV_C_NOTIFY_DISABLED_EVT,
  /* USER CODE BEGIN Service1_OpcodeEvt_t */

  /* USER CODE END Service1_OpcodeEvt_t */
  BLE_SENSOR_BOOT_REQUEST_EVT
} BLE_SENSOR_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service1_Data_t */

  /* USER CODE END Service1_Data_t */
} BLE_SENSOR_Data_t;

typedef struct
{
  BLE_SENSOR_OpcodeEvt_t       EvtOpcode;
  BLE_SENSOR_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service1_NotificationEvt_t */

  /* USER CODE END Service1_NotificationEvt_t */
} BLE_SENSOR_NotificationEvt_t;

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
void BLE_SENSOR_Init(void);
void BLE_SENSOR_Notification(BLE_SENSOR_NotificationEvt_t *p_Notification);
tBleStatus BLE_SENSOR_UpdateValue(BLE_SENSOR_CharOpcode_t CharOpcode, BLE_SENSOR_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*BLE_SENSOR_H */
