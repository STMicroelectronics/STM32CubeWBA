/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    p2p_server.h
  * @author  MCD Application Team
  * @brief   Header for p2p_server.c
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
#ifndef P2P_SERVER_H
#define P2P_SERVER_H

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
  P2P_SERVER_LED_C,
  P2P_SERVER_SWITCH_C,
  /* USER CODE BEGIN Service1_CharOpcode_t */

  /* USER CODE END Service1_CharOpcode_t */
  P2P_SERVER_CHAROPCODE_LAST
} P2P_SERVER_CharOpcode_t;

typedef enum
{
  P2P_SERVER_LED_C_READ_EVT,
  P2P_SERVER_LED_C_WRITE_NO_RESP_EVT,
  P2P_SERVER_SWITCH_C_NOTIFY_ENABLED_EVT,
  P2P_SERVER_SWITCH_C_NOTIFY_DISABLED_EVT,
  /* USER CODE BEGIN Service1_OpcodeEvt_t */

  /* USER CODE END Service1_OpcodeEvt_t */
  P2P_SERVER_BOOT_REQUEST_EVT
} P2P_SERVER_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service1_Data_t */

  /* USER CODE END Service1_Data_t */
} P2P_SERVER_Data_t;

typedef struct
{
  P2P_SERVER_OpcodeEvt_t       EvtOpcode;
  P2P_SERVER_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service1_NotificationEvt_t */

  /* USER CODE END Service1_NotificationEvt_t */
} P2P_SERVER_NotificationEvt_t;

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
void P2P_SERVER_Init(void);
void P2P_SERVER_Notification(P2P_SERVER_NotificationEvt_t *p_Notification);
tBleStatus P2P_SERVER_UpdateValue(P2P_SERVER_CharOpcode_t CharOpcode, P2P_SERVER_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*P2P_SERVER_H */
