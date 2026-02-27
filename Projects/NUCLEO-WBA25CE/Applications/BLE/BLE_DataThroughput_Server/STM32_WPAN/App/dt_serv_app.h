/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service1_app.h
  * @author  MCD Application Team
  * @brief   Header for service1_app.c
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
#ifndef DT_SERV_APP_H
#define DT_SERV_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ble_types.h"
#include "ble_core.h"
/* USER CODE BEGIN Includes */
#define BUTTON_PRESSED                                            GPIO_PIN_RESET
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  DT_SERV_CONN_HANDLE_EVT,
  DT_SERV_DISCON_HANDLE_EVT,

  /* USER CODE BEGIN Service1_OpcodeNotificationEvt_t */

  /* USER CODE END Service1_OpcodeNotificationEvt_t */

  DT_SERV_LAST_EVT,
} DT_SERV_APP_OpcodeNotificationEvt_t;

typedef struct
{
  DT_SERV_APP_OpcodeNotificationEvt_t          EvtOpcode;
  uint16_t                                 ConnectionHandle;

  /* USER CODE BEGIN DT_SERV_APP_ConnHandleNotEvt_t */

  /* USER CODE END DT_SERV_APP_ConnHandleNotEvt_t */
} DT_SERV_APP_ConnHandleNotEvt_t;
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
void DT_SERV_APP_Init(void);
void DT_SERV_APP_EvtRx(DT_SERV_APP_ConnHandleNotEvt_t *p_Notification);
/* USER CODE BEGIN EFP */
void DTS_Button1TriggerReceived(void);
void DTS_Button2TriggerReceived(void);
void DTS_Button3TriggerReceived(void);
void Resume_Notification(void);
/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*DT_SERV_APP_H */
