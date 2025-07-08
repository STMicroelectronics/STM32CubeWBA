/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dis.h
  * @author  MCD Application Team
  * @brief   Header for dis.c
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
#ifndef DIS_H
#define DIS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ble_types.h"
#include "ble_core.h"
#include "svc_ctl.h"
#include "zdd_ble_interface.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported defines ----------------------------------------------------------*/
/* USER CODE BEGIN ED */

/* USER CODE END ED */

typedef struct
{
  DIS_OpcodeEvt_t       EvtOpcode;
  DIS_Data_t            DataTransfered;
  uint16_t              ConnectionHandle;
  uint16_t              AttributeHandle;
  uint8_t               ServiceInstance;
  /* USER CODE BEGIN Service1_NotificationEvt_t */

  /* USER CODE END Service1_NotificationEvt_t */
} DIS_NotificationEvt_t;

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
void DIS_Init(void);
void DIS_Notification(DIS_NotificationEvt_t *p_Notification);
tBleStatus DIS_UpdateValue(DIS_CharOpcode_t CharOpcode, DIS_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*DIS_H */
