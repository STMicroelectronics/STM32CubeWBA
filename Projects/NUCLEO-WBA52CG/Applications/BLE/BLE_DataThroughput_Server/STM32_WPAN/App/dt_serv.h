/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service1.h
  * @author  MCD Application Team
  * @brief   Header for service1.c
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
#ifndef DT_SERV_H
#define DT_SERV_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported defines ----------------------------------------------------------*/
/* USER CODE BEGIN ED */
#define DATA_NOTIFICATION_MAX_PACKET_SIZE (244U)
/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  DT_SERV_TX_CHAR,
  DT_SERV_RX_CHAR,
  DT_SERV_THROUGH_CHAR,
  /* USER CODE BEGIN Service1_CharOpcode_t */

  /* USER CODE END Service1_CharOpcode_t */
  DT_SERV_CHAROPCODE_LAST
} DT_SERV_CharOpcode_t;

typedef enum
{
  DT_SERV_TX_CHAR_NOTIFY_ENABLED_EVT,
  DT_SERV_TX_CHAR_NOTIFY_DISABLED_EVT,
  DT_SERV_RX_CHAR_READ_EVT,
  DT_SERV_RX_CHAR_WRITE_NO_RESP_EVT,
  DT_SERV_THROUGH_CHAR_NOTIFY_ENABLED_EVT,
  DT_SERV_THROUGH_CHAR_NOTIFY_DISABLED_EVT,
  /* USER CODE BEGIN Service1_OpcodeEvt_t */

  /* USER CODE END Service1_OpcodeEvt_t */
  DT_SERV_BOOT_REQUEST_EVT
} DT_SERV_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service1_Data_t */
  uint8_t pPayload_n_1;
  uint8_t pPayload_n;
  /* USER CODE END Service1_Data_t */
} DT_SERV_Data_t;

typedef struct
{
  DT_SERV_OpcodeEvt_t       EvtOpcode;
  DT_SERV_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service1_NotificationEvt_t */

  /* USER CODE END Service1_NotificationEvt_t */
} DT_SERV_NotificationEvt_t;

/* USER CODE BEGIN ET */
typedef struct
{
  uint8_t *pPayload;
  uint32_t pPayload_n_1;
  uint32_t pPayload_n;
  uint32_t Length;
} DTS_STM_Payload_t;
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
void DT_SERV_Init(void);
void DT_SERV_Notification(DT_SERV_NotificationEvt_t *p_Notification);
tBleStatus DT_SERV_UpdateValue(DT_SERV_CharOpcode_t CharOpcode, DT_SERV_Data_t *pData);
/* USER CODE BEGIN EF */

/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*DT_SERV_H */
