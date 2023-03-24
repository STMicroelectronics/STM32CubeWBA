/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    STM32_WPAN
  * @author  MCD Application Team
  * @brief   Header for STM32_WPAN
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
#ifndef OTA_APP_H
#define OTA_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ota.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  OTA_CONN_HANDLE_EVT,
  OTA_DISCON_HANDLE_EVT,

  /* USER CODE BEGIN Service3_OpcodeNotificationEvt_t */

  /* USER CODE END Service3_OpcodeNotificationEvt_t */

  OTA_LAST_EVT,
} OTA_APP_OpcodeNotificationEvt_t;

typedef struct
{
  OTA_APP_OpcodeNotificationEvt_t          EvtOpcode;
  uint16_t                                 ConnectionHandle;

  /* USER CODE BEGIN OTA_APP_ConnHandleNotEvt_t */

  /* USER CODE END OTA_APP_ConnHandleNotEvt_t */
} OTA_APP_ConnHandleNotEvt_t;
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

/* Exported functions ------------------------------------------------------- */
void OTA_APP_Init(void);
void OTA_APP_EvtRx(OTA_APP_ConnHandleNotEvt_t *p_Notification);
/* USER CODE BEGIN EF */
uint8_t OTA_APP_GetConfStatus(void);
/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*OTA_APP_H */
