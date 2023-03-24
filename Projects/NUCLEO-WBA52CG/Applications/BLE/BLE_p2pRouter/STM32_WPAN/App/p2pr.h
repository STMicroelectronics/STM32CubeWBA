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
#ifndef P2PR_H
#define P2PR_H

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
  P2PR_WRITEFWD,
  P2PR_NOTIFFWD,
  P2PR_DEVINFO,

  /* USER CODE BEGIN Service1_CharOpcode_t */

  /* USER CODE END Service1_CharOpcode_t */

  P2PR_CHAROPCODE_LAST
} P2PR_CharOpcode_t;

typedef enum
{
  P2PR_WRITEFWD_READ_EVT,
  P2PR_WRITEFWD_WRITE_NO_RESP_EVT,
  P2PR_NOTIFFWD_NOTIFY_ENABLED_EVT,
  P2PR_NOTIFFWD_NOTIFY_DISABLED_EVT,
  P2PR_DEVINFO_NOTIFY_ENABLED_EVT,
  P2PR_DEVINFO_NOTIFY_DISABLED_EVT,

  /* USER CODE BEGIN Service1_OpcodeEvt_t */

  /* USER CODE END Service1_OpcodeEvt_t */

  P2PR_BOOT_REQUEST_EVT
} P2PR_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service1_Data_t */

  /* USER CODE END Service1_Data_t */

} P2PR_Data_t;

typedef struct
{
  P2PR_OpcodeEvt_t       EvtOpcode;
  P2PR_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint8_t                 ServiceInstance;

  /* USER CODE BEGIN Service1_NotificationEvt_t */

  /* USER CODE END Service1_NotificationEvt_t */

} P2PR_NotificationEvt_t;

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
void P2PR_Init(void);
void P2PR_Notification(P2PR_NotificationEvt_t *p_Notification);
tBleStatus P2PR_UpdateValue(P2PR_CharOpcode_t CharOpcode, P2PR_Data_t *pData);
/* USER CODE BEGIN EF */

/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*P2PR_H */
