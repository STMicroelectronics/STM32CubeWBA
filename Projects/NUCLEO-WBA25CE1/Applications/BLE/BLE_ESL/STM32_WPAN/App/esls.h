/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ESLS.h
  * @author  MCD Application Team
  * @brief   Header for ESLS.c
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
#ifndef ESLS_H
#define ESLS_H

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
/* USER CODE BEGIN ED */

/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  ESLS_ESL_ADDR,
  ESLS_AP_SYNC_KEY_MATERIAL,
  ESLS_ESL_RESP_KEY_MATERIAL,
  ESLS_ESL_CURR_ABS_TIME,
  ESLS_ESL_DISPLAY_INFO,
  ESLS_ESL_IMAGE_INFO,
  ESLS_ESL_SENSOR_INFO,
  ESLS_ESL_LED_INFO,
  ESLS_ESL_CONTROL_POINT,
  /* USER CODE BEGIN Service2_CharOpcode_t */

  /* USER CODE END Service2_CharOpcode_t */
  ESLS_CHAROPCODE_LAST
} ESLS_CharOpcode_t;

typedef enum
{
  ESLS_ESL_ADDR_WRITE_EVT,
  ESLS_AP_SYNC_KEY_MATERIAL_WRITE_EVT,
  ESLS_ESL_RESP_KEY_MATERIAL_WRITE_EVT,
  ESLS_ESL_CURR_ABS_TIME_WRITE_EVT,
  ESLS_ESL_DISPLAY_INFO_READ_EVT,
  ESLS_ESL_IMAGE_INFO_READ_EVT,
  ESLS_ESL_SENSOR_INFO_READ_EVT,
  ESLS_ESL_LED_INFO_READ_EVT,
  ESLS_ESL_CONTROL_POINT_WRITE_NO_RESP_EVT,
  ESLS_ESL_CONTROL_POINT_WRITE_EVT,
  ESLS_ESL_CONTROL_POINT_NOTIFY_ENABLED_EVT,
  ESLS_ESL_CONTROL_POINT_NOTIFY_DISABLED_EVT,
  /* USER CODE BEGIN Service2_OpcodeEvt_t */

  /* USER CODE END Service2_OpcodeEvt_t */
  ESLS_BOOT_REQUEST_EVT
} ESLS_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service2_Data_t */

  /* USER CODE END Service2_Data_t */
} ESLS_Data_t;

typedef struct
{
  ESLS_OpcodeEvt_t       EvtOpcode;
  ESLS_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service2_NotificationEvt_t */

  /* USER CODE END Service2_NotificationEvt_t */
} ESLS_NotificationEvt_t;

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
void ESLS_Init(void);
void ESLS_Notification(ESLS_NotificationEvt_t *p_Notification);
tBleStatus ESLS_UpdateValue(ESLS_CharOpcode_t CharOpcode, ESLS_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*ESLS_H */
