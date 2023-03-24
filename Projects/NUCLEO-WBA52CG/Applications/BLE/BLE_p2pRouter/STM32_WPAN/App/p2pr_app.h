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
#ifndef P2PR_APP_H
#define P2PR_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  P2PR_CONN_HANDLE_EVT,
  P2PR_DISCON_HANDLE_EVT,

  /* USER CODE BEGIN Service1_OpcodeNotificationEvt_t */

  /* USER CODE END Service1_OpcodeNotificationEvt_t */

  P2PR_LAST_EVT,
} P2PR_APP_OpcodeNotificationEvt_t;

typedef struct
{
  P2PR_APP_OpcodeNotificationEvt_t          EvtOpcode;
  uint16_t                                 ConnectionHandle;

  /* USER CODE BEGIN P2PR_APP_ConnHandleNotEvt_t */

  /* USER CODE END P2PR_APP_ConnHandleNotEvt_t */
} P2PR_APP_ConnHandleNotEvt_t;
/* USER CODE BEGIN ET */
typedef enum
{
  P2PR_DEV_NONE,
  P2PR_DEV_FOUND,
  P2PR_DEV_CONNECTING,
  P2PR_DEV_DISCOVERING,
  P2PR_DEV_CONNECTED,
  P2PR_DEV_LOST,
  P2PR_DEV_LAST
}P2PR_deviceStatus_t;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* remove 1 to leave a link for a smartphone */
#define P2P_DEVICE_COUNT_MAX (CFG_BLE_NUM_LINK - 1) 
/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ------------------------------------------------------- */
void P2PR_APP_Init(void);
void P2PR_APP_EvtRx(P2PR_APP_ConnHandleNotEvt_t *p_Notification);
/* USER CODE BEGIN EF */
uint8_t P2PR_setDeviceInfo(uint8_t id, uint8_t *p_bdAddr, uint8_t dev_status);
uint8_t P2PR_getDeviceInfo(uint8_t id, uint8_t *p_bdAddr, uint8_t *p_dev_status);
uint8_t P2PR_getIndexFromConnHdl(uint16_t connHdl);
uint16_t P2PR_getConnHdlFromIndex(uint8_t index);
uint8_t P2PR_getWriteData(uint32_t *p_P2PR_writeValAddr, uint8_t *p_P2PR_writeValLen);
uint8_t P2PR_setNotifLevel(uint8_t dev_idx, uint8_t level);
uint8_t P2PR_analyseAdvReport(hci_le_advertising_report_event_rp0 *p_adv_report);
void P2PR_ForwardNotification(void);
/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*P2PR_APP_H */
