/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service1_app.h
  * @author  MCD Application Team
  * @brief   Header for service1_app.c
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
#ifndef GHS_APP_H
#define GHS_APP_H

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
  GHS_CONN_HANDLE_EVT,
  GHS_DISCON_HANDLE_EVT,

  /* USER CODE BEGIN Service1_OpcodeNotificationEvt_t */

  /* USER CODE END Service1_OpcodeNotificationEvt_t */

  GHS_LAST_EVT,
} GHS_APP_OpcodeNotificationEvt_t;

typedef struct
{
  GHS_APP_OpcodeNotificationEvt_t          EvtOpcode;
  uint16_t                                 ConnectionHandle;

  /* USER CODE BEGIN GHS_APP_ConnHandleNotEvt_t */

  /* USER CODE END GHS_APP_ConnHandleNotEvt_t */
} GHS_APP_ConnHandleNotEvt_t;
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
void GHS_APP_Init(void);
void GHS_APP_EvtRx(GHS_APP_ConnHandleNotEvt_t *p_Notification);
/* USER CODE BEGIN EFP */
uint8_t GHS_APP_GetGHSCPCharacteristicIndicationEnabled(void);
uint32_t GHS_APP_GetLHOTimerStarted(void);
void GHS_APP_UpdateFeature(void);
void GHS_APP_UpdateLiveHealthObservation(void);
void GHS_APP_UpdateObservationScheduleChanged(void);
/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*GHS_APP_H */
