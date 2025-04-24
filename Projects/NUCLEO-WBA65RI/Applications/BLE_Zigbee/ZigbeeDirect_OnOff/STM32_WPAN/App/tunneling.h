/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Tunneling.h
  * @author  MCD Application Team
  * @brief   Header for Tunneling.c
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
#ifndef TUNNELING_H
#define TUNNELING_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported defines ----------------------------------------------------------*/
/* USER CODE BEGIN ED */

/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
#ifdef BLE_MULTIPLE_DEFINITION
typedef enum
{
  TUNNELING_TUNNZDTSNPDU,
  /* USER CODE BEGIN Service4_CharOpcode_t */

  /* USER CODE END Service4_CharOpcode_t */
  TUNNELING_CHAROPCODE_LAST
} TUNNELING_CharOpcode_t;

typedef enum
{
  TUNNELING_TUNNZDTSNPDU_WRITE_EVT,
  TUNNELING_TUNNZDTSNPDU_INDICATE_ENABLED_EVT,
  TUNNELING_TUNNZDTSNPDU_INDICATE_DISABLED_EVT,
  /* USER CODE BEGIN Service4_OpcodeEvt_t */

  /* USER CODE END Service4_OpcodeEvt_t */
  TUNNELING_BOOT_REQUEST_EVT
} TUNNELING_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service4_Data_t */

  /* USER CODE END Service4_Data_t */
} TUNNELING_Data_t;

#else
#include "zdd_ble_interface.h"
#endif /* NO_SINGLE_INTERFACE */

typedef struct
{
  TUNNELING_OpcodeEvt_t       EvtOpcode;
  TUNNELING_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service4_NotificationEvt_t */

  /* USER CODE END Service4_NotificationEvt_t */
} TUNNELING_NotificationEvt_t;

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
void TUNNELING_Init(void);
void TUNNELING_Notification(TUNNELING_NotificationEvt_t *p_Notification);
tBleStatus TUNNELING_UpdateValue(TUNNELING_CharOpcode_t CharOpcode, TUNNELING_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*TUNNELING_H */
