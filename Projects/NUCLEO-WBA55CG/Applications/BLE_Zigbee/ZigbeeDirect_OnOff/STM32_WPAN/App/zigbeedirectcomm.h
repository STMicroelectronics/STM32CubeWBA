/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ZigbeeDirectComm.h
  * @author  MCD Application Team
  * @brief   Header for ZigbeeDirectComm.c
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
#ifndef ZIGBEEDIRECTCOMM_H
#define ZIGBEEDIRECTCOMM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
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
  ZIGBEEDIRECTCOMM_OpcodeEvt_t EvtOpcode;
  ZIGBEEDIRECTCOMM_Data_t      DataTransfered;
  uint16_t                     ConnectionHandle;
  uint16_t                     AttributeHandle;
  uint8_t                      ServiceInstance;
  /* USER CODE BEGIN Service2_NotificationEvt_t */
  uint16_t                     ZigbeedirectcommSvcHdle;
  uint16_t                     CommidentityCharHdle;
  /* USER CODE END Service2_NotificationEvt_t */
} ZIGBEEDIRECTCOMM_NotificationEvt_t;

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
void ZIGBEEDIRECTCOMM_Init(void);
void ZIGBEEDIRECTCOMM_Notification(ZIGBEEDIRECTCOMM_NotificationEvt_t *p_Notification);
tBleStatus ZIGBEEDIRECTCOMM_UpdateValue(ZIGBEEDIRECTCOMM_CharOpcode_t CharOpcode, ZIGBEEDIRECTCOMM_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*ZIGBEEDIRECTCOMM_H */
