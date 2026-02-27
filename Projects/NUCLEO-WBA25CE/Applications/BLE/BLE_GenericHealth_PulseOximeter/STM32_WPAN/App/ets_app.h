/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service5_app.h
  * @author  MCD Application Team
  * @brief   Header for service5_app.c
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
#ifndef ETS_APP_H
#define ETS_APP_H

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
  ETS_CONN_HANDLE_EVT,
  ETS_DISCON_HANDLE_EVT,

  /* USER CODE BEGIN Service3_OpcodeNotificationEvt_t */

  /* USER CODE END Service3_OpcodeNotificationEvt_t */

  ETS_LAST_EVT,
} ETS_APP_OpcodeNotificationEvt_t;

typedef struct
{
  ETS_APP_OpcodeNotificationEvt_t          EvtOpcode;
  uint16_t                                 ConnectionHandle;

  /* USER CODE BEGIN ETS_APP_ConnHandleNotEvt_t */

  /* USER CODE END ETS_APP_ConnHandleNotEvt_t */
} ETS_APP_ConnHandleNotEvt_t;
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
void ETS_APP_Init(void);
void ETS_APP_EvtRx(ETS_APP_ConnHandleNotEvt_t *p_Notification);
/* USER CODE BEGIN EFP */
uint8_t ETS_CheckRequestValid(uint8_t * pRequestData, 
                              uint8_t requestDataLength);
void ETS_APP_GetElapsedTime(uint8_t * p_elapsedTime);
void ETS_APP_SetElapsedTime(uint8_t * p_elapsedTime);
/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*ETS_APP_H */
