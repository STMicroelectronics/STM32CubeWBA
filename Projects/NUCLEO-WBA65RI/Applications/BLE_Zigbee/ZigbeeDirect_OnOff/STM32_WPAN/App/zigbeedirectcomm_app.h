/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ZigbeeDirectComm_app.h
  * @author  MCD Application Team
  * @brief   Header for ZigbeeDirectComm_app.c
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
#ifndef ZIGBEEDIRECTCOMM_APP_H
#define ZIGBEEDIRECTCOMM_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  ZIGBEEDIRECTCOMM_CONN_HANDLE_EVT,
  ZIGBEEDIRECTCOMM_DISCON_HANDLE_EVT,

  /* USER CODE BEGIN Service2_OpcodeNotificationEvt_t */

  /* USER CODE END Service2_OpcodeNotificationEvt_t */

  ZIGBEEDIRECTCOMM_LAST_EVT,
} ZIGBEEDIRECTCOMM_APP_OpcodeNotificationEvt_t;

typedef struct
{
  ZIGBEEDIRECTCOMM_APP_OpcodeNotificationEvt_t          EvtOpcode;
  uint16_t                                 ConnectionHandle;

  /* USER CODE BEGIN ZIGBEEDIRECTCOMM_APP_ConnHandleNotEvt_t */

  /* USER CODE END ZIGBEEDIRECTCOMM_APP_ConnHandleNotEvt_t */
} ZIGBEEDIRECTCOMM_APP_ConnHandleNotEvt_t;
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
void ZIGBEEDIRECTCOMM_APP_Init(void);
void ZIGBEEDIRECTCOMM_APP_EvtRx(ZIGBEEDIRECTCOMM_APP_ConnHandleNotEvt_t *p_Notification);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*ZIGBEEDIRECTCOMM_APP_H */
