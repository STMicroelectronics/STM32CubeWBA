/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Tunneling_app.h
  * @author  MCD Application Team
  * @brief   Header for Tunneling_app.c
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
#ifndef TUNNELING_APP_H
#define TUNNELING_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  TUNNELING_CONN_HANDLE_EVT,
  TUNNELING_DISCON_HANDLE_EVT,

  /* USER CODE BEGIN Service4_OpcodeNotificationEvt_t */

  /* USER CODE END Service4_OpcodeNotificationEvt_t */

  TUNNELING_LAST_EVT,
} TUNNELING_APP_OpcodeNotificationEvt_t;

typedef struct
{
  TUNNELING_APP_OpcodeNotificationEvt_t          EvtOpcode;
  uint16_t                                 ConnectionHandle;

  /* USER CODE BEGIN TUNNELING_APP_ConnHandleNotEvt_t */

  /* USER CODE END TUNNELING_APP_ConnHandleNotEvt_t */
} TUNNELING_APP_ConnHandleNotEvt_t;
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
void TUNNELING_APP_Init(void);
void TUNNELING_APP_EvtRx(TUNNELING_APP_ConnHandleNotEvt_t *p_Notification);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*TUNNELING_APP_H */
