/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    DF_Tag.h
  * @author  MCD Application Team
  * @brief   Header for DF_Tag.c
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
#ifndef DF_TAG_H
#define DF_TAG_H

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
  DF_TAG_LED_C,
  DF_TAG_SWITCH_C,
  /* USER CODE BEGIN Service1_CharOpcode_t */

  /* USER CODE END Service1_CharOpcode_t */
  DF_TAG_CHAROPCODE_LAST
} DF_TAG_CharOpcode_t;

typedef enum
{
  DF_TAG_LED_C_READ_EVT,
  DF_TAG_LED_C_WRITE_NO_RESP_EVT,
  DF_TAG_SWITCH_C_NOTIFY_ENABLED_EVT,
  DF_TAG_SWITCH_C_NOTIFY_DISABLED_EVT,
  /* USER CODE BEGIN Service1_OpcodeEvt_t */

  /* USER CODE END Service1_OpcodeEvt_t */
  DF_TAG_BOOT_REQUEST_EVT
} DF_TAG_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service1_Data_t */

  /* USER CODE END Service1_Data_t */
} DF_TAG_Data_t;

typedef struct
{
  DF_TAG_OpcodeEvt_t       EvtOpcode;
  DF_TAG_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service1_NotificationEvt_t */

  /* USER CODE END Service1_NotificationEvt_t */
} DF_TAG_NotificationEvt_t;

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
void DF_TAG_Init(void);
void DF_TAG_Notification(DF_TAG_NotificationEvt_t *p_Notification);
tBleStatus DF_TAG_UpdateValue(DF_TAG_CharOpcode_t CharOpcode, DF_TAG_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*DF_TAG_H */
