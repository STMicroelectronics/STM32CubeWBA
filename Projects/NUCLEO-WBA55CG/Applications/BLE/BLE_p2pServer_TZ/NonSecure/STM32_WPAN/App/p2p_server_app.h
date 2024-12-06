/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    p2p_server_app.h
  * @author  MCD Application Team
  * @brief   Header for p2p_server_app.c
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
#ifndef P2P_SERVER_APP_H
#define P2P_SERVER_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  P2P_SERVER_CONN_HANDLE_EVT,
  P2P_SERVER_DISCON_HANDLE_EVT,

  /* USER CODE BEGIN Service1_OpcodeNotificationEvt_t */

  /* USER CODE END Service1_OpcodeNotificationEvt_t */

  P2P_SERVER_LAST_EVT,
} P2P_SERVER_APP_OpcodeNotificationEvt_t;

typedef struct
{
  P2P_SERVER_APP_OpcodeNotificationEvt_t          EvtOpcode;
  uint16_t                                 ConnectionHandle;

  /* USER CODE BEGIN P2P_SERVER_APP_ConnHandleNotEvt_t */

  /* USER CODE END P2P_SERVER_APP_ConnHandleNotEvt_t */
} P2P_SERVER_APP_ConnHandleNotEvt_t;
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
void P2P_SERVER_APP_Init(void);
void P2P_SERVER_APP_EvtRx(P2P_SERVER_APP_ConnHandleNotEvt_t *p_Notification);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*P2P_SERVER_APP_H */
