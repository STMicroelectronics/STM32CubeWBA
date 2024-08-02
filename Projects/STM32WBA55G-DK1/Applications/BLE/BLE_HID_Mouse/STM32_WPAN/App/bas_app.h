/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service3_app.h
  * @author  MCD Application Team
  * @brief   Header for service3_app.c
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
#ifndef BAS_APP_H
#define BAS_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  BAS_CONN_HANDLE_EVT,
  BAS_DISCON_HANDLE_EVT,

  /* USER CODE BEGIN Service3_OpcodeNotificationEvt_t */

  /* USER CODE END Service3_OpcodeNotificationEvt_t */

  BAS_LAST_EVT,
} BAS_APP_OpcodeNotificationEvt_t;

typedef struct
{
  BAS_APP_OpcodeNotificationEvt_t          EvtOpcode;
  uint16_t                                 ConnectionHandle;

  /* USER CODE BEGIN BAS_APP_ConnHandleNotEvt_t */

  /* USER CODE END BAS_APP_ConnHandleNotEvt_t */
} BAS_APP_ConnHandleNotEvt_t;
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
void BAS_APP_Init(void);
void BAS_APP_EvtRx(BAS_APP_ConnHandleNotEvt_t *p_Notification);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*BAS_APP_H */
