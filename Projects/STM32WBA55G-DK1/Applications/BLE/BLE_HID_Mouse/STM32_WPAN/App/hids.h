/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service1.h
  * @author  MCD Application Team
  * @brief   Header for service1.c
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#ifndef HIDS_H
#define HIDS_H

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
  HIDS_INPUTREP,
  HIDS_REM,
  HIDS_HII,
  HIDS_HCP,
  /* USER CODE BEGIN Service1_CharOpcode_t */

  /* USER CODE END Service1_CharOpcode_t */
  HIDS_CHAROPCODE_LAST
} HIDS_CharOpcode_t;

typedef enum
{
  HIDS_INPUTREP_READ_EVT,
  HIDS_INPUTREP_WRITE_EVT,
  HIDS_INPUTREP_NOTIFY_ENABLED_EVT,
  HIDS_INPUTREP_NOTIFY_DISABLED_EVT,
  HIDS_REM_READ_EVT,
  HIDS_HII_READ_EVT,
  HIDS_HCP_WRITE_NO_RESP_EVT,
  /* USER CODE BEGIN Service1_OpcodeEvt_t */

  /* USER CODE END Service1_OpcodeEvt_t */
  HIDS_BOOT_REQUEST_EVT
} HIDS_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service1_Data_t */

  /* USER CODE END Service1_Data_t */
} HIDS_Data_t;

typedef struct
{
  HIDS_OpcodeEvt_t       EvtOpcode;
  HIDS_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service1_NotificationEvt_t */

  /* USER CODE END Service1_NotificationEvt_t */
} HIDS_NotificationEvt_t;

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
void HIDS_Init(void);
void HIDS_Notification(HIDS_NotificationEvt_t *p_Notification);
tBleStatus HIDS_UpdateValue(HIDS_CharOpcode_t CharOpcode, HIDS_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*HIDS_H */
