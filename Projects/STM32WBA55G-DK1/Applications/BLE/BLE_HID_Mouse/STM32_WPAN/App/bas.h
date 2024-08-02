/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service3.h
  * @author  MCD Application Team
  * @brief   Header for service3.c
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
#ifndef BAS_H
#define BAS_H

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
typedef enum
{
  BAS_BAL,
  /* USER CODE BEGIN Service3_CharOpcode_t */

  /* USER CODE END Service3_CharOpcode_t */
  BAS_CHAROPCODE_LAST
} BAS_CharOpcode_t;

typedef enum
{
  BAS_BAL_READ_EVT,
  BAS_BAL_NOTIFY_ENABLED_EVT,
  BAS_BAL_NOTIFY_DISABLED_EVT,
  /* USER CODE BEGIN Service3_OpcodeEvt_t */

  /* USER CODE END Service3_OpcodeEvt_t */
  BAS_BOOT_REQUEST_EVT
} BAS_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service3_Data_t */

  /* USER CODE END Service3_Data_t */
} BAS_Data_t;

typedef struct
{
  BAS_OpcodeEvt_t       EvtOpcode;
  BAS_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service3_NotificationEvt_t */

  /* USER CODE END Service3_NotificationEvt_t */
} BAS_NotificationEvt_t;

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
void BAS_Init(void);
void BAS_Notification(BAS_NotificationEvt_t *p_Notification);
tBleStatus BAS_UpdateValue(BAS_CharOpcode_t CharOpcode, BAS_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*BAS_H */
