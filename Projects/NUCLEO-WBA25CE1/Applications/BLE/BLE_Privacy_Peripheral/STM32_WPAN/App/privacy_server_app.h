/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Privacy_Server_app.h
  * @author  MCD Application Team
  * @brief   Header for Privacy_Server_app.c
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
#ifndef PRIVACY_SERVER_APP_H
#define PRIVACY_SERVER_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ble_types.h"
#include "ble_core.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  PRIVACY_SERVER_CONN_HANDLE_EVT,
  PRIVACY_SERVER_DISCON_HANDLE_EVT,

  /* USER CODE BEGIN Service1_OpcodeNotificationEvt_t */

  /* USER CODE END Service1_OpcodeNotificationEvt_t */

  PRIVACY_SERVER_LAST_EVT,
} PRIVACY_SERVER_APP_OpcodeNotificationEvt_t;

typedef struct
{
  PRIVACY_SERVER_APP_OpcodeNotificationEvt_t          EvtOpcode;
  uint16_t                                 ConnectionHandle;

  /* USER CODE BEGIN PRIVACY_SERVER_APP_ConnHandleNotEvt_t */

  /* USER CODE END PRIVACY_SERVER_APP_ConnHandleNotEvt_t */
} PRIVACY_SERVER_APP_ConnHandleNotEvt_t;
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
void PRIVACY_SERVER_APP_Init(void);
void PRIVACY_SERVER_APP_EvtRx(PRIVACY_SERVER_APP_ConnHandleNotEvt_t *p_Notification);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*PRIVACY_SERVER_APP_H */
