/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service5.h
  * @author  MCD Application Team
  * @brief   Header for service5.c
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
#ifndef ETS_H
#define ETS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported defines ----------------------------------------------------------*/
/* USER CODE BEGIN ED */
#define ETS_ATT_ERROR_CODE_TIME_SOURCE_QUALITY_TOO_LOW                    (0x80)
#define ETS_ATT_ERROR_CODE_INCORRECT_TIME_FORMAT                          (0x81)
#define ETS_ATT_ERROR_CODE_OUT_OF_RANGE                                   (0xFF)
/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  ETS_CET,
  /* USER CODE BEGIN Service3_CharOpcode_t */

  /* USER CODE END Service3_CharOpcode_t */
  ETS_CHAROPCODE_LAST
} ETS_CharOpcode_t;

typedef enum
{
  ETS_CET_READ_EVT,
  ETS_CET_INDICATE_ENABLED_EVT,
  ETS_CET_INDICATE_DISABLED_EVT,
  /* USER CODE BEGIN Service3_OpcodeEvt_t */

  /* USER CODE END Service3_OpcodeEvt_t */
  ETS_BOOT_REQUEST_EVT
} ETS_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service3_Data_t */

  /* USER CODE END Service3_Data_t */
} ETS_Data_t;

typedef struct
{
  ETS_OpcodeEvt_t       EvtOpcode;
  ETS_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service3_NotificationEvt_t */

  /* USER CODE END Service3_NotificationEvt_t */
} ETS_NotificationEvt_t;

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
void ETS_Init(void);
void ETS_Notification(ETS_NotificationEvt_t *p_Notification);
tBleStatus ETS_UpdateValue(ETS_CharOpcode_t CharOpcode, ETS_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*ETS_H */
