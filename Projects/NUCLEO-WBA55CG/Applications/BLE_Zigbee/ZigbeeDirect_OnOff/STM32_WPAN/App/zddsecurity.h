/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ZDDSecurity.h
  * @author  MCD Application Team
  * @brief   Header for ZDDSecurity.c
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
#ifndef ZDDSECURITY_H
#define ZDDSECURITY_H

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
  ZDDSECURITY_SECURITY25519AES,
  ZDDSECURITY_SECURITY25519SHA,
  ZDDSECURITY_P256SHA,
  /* USER CODE BEGIN Service3_CharOpcode_t */

  /* USER CODE END Service3_CharOpcode_t */
  ZDDSECURITY_CHAROPCODE_LAST
} ZDDSECURITY_CharOpcode_t;

typedef enum
{
  ZDDSECURITY_SECURITY25519AES_WRITE_EVT,
  ZDDSECURITY_SECURITY25519AES_INDICATE_ENABLED_EVT,
  ZDDSECURITY_SECURITY25519AES_INDICATE_DISABLED_EVT,
  ZDDSECURITY_SECURITY25519SHA_WRITE_EVT,
  ZDDSECURITY_SECURITY25519SHA_INDICATE_ENABLED_EVT,
  ZDDSECURITY_SECURITY25519SHA_INDICATE_DISABLED_EVT,
  ZDDSECURITY_P256SHA_WRITE_EVT,
  ZDDSECURITY_P256SHA_INDICATE_ENABLED_EVT,
  ZDDSECURITY_P256SHA_INDICATE_DISABLED_EVT,
  /* USER CODE BEGIN Service3_OpcodeEvt_t */

  /* USER CODE END Service3_OpcodeEvt_t */
  ZDDSECURITY_BOOT_REQUEST_EVT
} ZDDSECURITY_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service3_Data_t */

  /* USER CODE END Service3_Data_t */
} ZDDSECURITY_Data_t;

#else
#include "zdd_ble_interface.h"
#endif /* NO_SINGLE_INTERFACE */

typedef struct
{
  ZDDSECURITY_OpcodeEvt_t       EvtOpcode;
  ZDDSECURITY_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service3_NotificationEvt_t */

  /* USER CODE END Service3_NotificationEvt_t */
} ZDDSECURITY_NotificationEvt_t;

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
void ZDDSECURITY_Init(void);
void ZDDSECURITY_Notification(ZDDSECURITY_NotificationEvt_t *p_Notification);
tBleStatus ZDDSECURITY_UpdateValue(ZDDSECURITY_CharOpcode_t CharOpcode, ZDDSECURITY_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*ZDDSECURITY_H */
