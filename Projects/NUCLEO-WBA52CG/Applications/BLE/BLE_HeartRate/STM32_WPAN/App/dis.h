/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service2.h
  * @author  MCD Application Team
  * @brief   Header for service2.c
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
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported defines ----------------------------------------------------------*/
/* USER CODE BEGIN ED */

/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  DIS_MANS,
  DIS_MNBS,
  DIS_SNS,
  DIS_HRS,
  DIS_FRS,

  /* USER CODE BEGIN Service2_CharOpcode_t */

  /* USER CODE END Service2_CharOpcode_t */

  DIS_CHAROPCODE_LAST
} DIS_CharOpcode_t;

typedef enum
{
  DIS_MANS_READ_EVT,
  DIS_MNBS_READ_EVT,
  DIS_SNS_READ_EVT,
  DIS_HRS_READ_EVT,
  DIS_FRS_READ_EVT,

  /* USER CODE BEGIN Service2_OpcodeEvt_t */

  /* USER CODE END Service2_OpcodeEvt_t */

  DIS_BOOT_REQUEST_EVT
} DIS_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service2_Data_t */

  /* USER CODE END Service2_Data_t */

} DIS_Data_t;

typedef struct
{
  DIS_OpcodeEvt_t       EvtOpcode;
  DIS_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint8_t                 ServiceInstance;

  /* USER CODE BEGIN Service2_NotificationEvt_t */

  /* USER CODE END Service2_NotificationEvt_t */

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

/* Exported functions ------------------------------------------------------- */
void DIS_Init(void);
void DIS_Notification(DIS_NotificationEvt_t *p_Notification);
tBleStatus DIS_UpdateValue(DIS_CharOpcode_t CharOpcode, DIS_Data_t *pData);
/* USER CODE BEGIN EF */

/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*DIS_H */
