/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service1_app.h
  * @author  MCD Application Team
  * @brief   Header for service1_app.c
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
#ifndef PWR_APP_H
#define PWR_APP_H

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
  PWR_CONN_HANDLE_EVT,
  PWR_DISCON_HANDLE_EVT,

  /* USER CODE BEGIN Service1_OpcodeNotificationEvt_t */

  /* USER CODE END Service1_OpcodeNotificationEvt_t */

  PWR_LAST_EVT,
} PWR_APP_OpcodeNotificationEvt_t;

typedef struct
{
  PWR_APP_OpcodeNotificationEvt_t          EvtOpcode;
  uint16_t                                 ConnectionHandle;

  /* USER CODE BEGIN PWR_APP_ConnHandleNotEvt_t */

  /* USER CODE END PWR_APP_ConnHandleNotEvt_t */
} PWR_APP_ConnHandleNotEvt_t;
/* USER CODE BEGIN ET */
typedef enum
{
  PWR_CO_GPIO_OFF,
  PWR_CO_GPIO_ON,
  PWR_CO_GPIO_TIM_RESET_ON,
} PWR_CO_GPIO_Status_t;
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
void PWR_APP_Init(void);
void PWR_APP_EvtRx(PWR_APP_ConnHandleNotEvt_t *p_Notification);
/* USER CODE BEGIN EFP */
void Send_Data(void);
void Disable_GPIOs(void);
/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*PWR_APP_H */
