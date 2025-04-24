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
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported defines ----------------------------------------------------------*/
/* USER CODE BEGIN ED */

/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
#ifdef BLE_MULTIPLE_DEFINITION
typedef enum
{
  ZIGBEEDIRECTCOMM_FORMNETWORK,
  ZIGBEEDIRECTCOMM_JOINNETWORK,
  ZIGBEEDIRECTCOMM_PERMITJOIN,
  ZIGBEEDIRECTCOMM_LEAVENETWORK,
  ZIGBEEDIRECTCOMM_COMMSTATUS,
  ZIGBEEDIRECTCOMM_MANAGEJOINER,
  ZIGBEEDIRECTCOMM_COMMIDENTITY,
  ZIGBEEDIRECTCOMM_FINDBIND,
  /* USER CODE BEGIN Service2_CharOpcode_t */

  /* USER CODE END Service2_CharOpcode_t */
  ZIGBEEDIRECTCOMM_CHAROPCODE_LAST
} ZIGBEEDIRECTCOMM_CharOpcode_t;

typedef enum
{
  ZIGBEEDIRECTCOMM_FORMNETWORK_WRITE_EVT,
  ZIGBEEDIRECTCOMM_JOINNETWORK_WRITE_EVT,
  ZIGBEEDIRECTCOMM_PERMITJOIN_WRITE_EVT,
  ZIGBEEDIRECTCOMM_LEAVENETWORK_WRITE_EVT,
  ZIGBEEDIRECTCOMM_COMMSTATUS_READ_EVT,
  ZIGBEEDIRECTCOMM_COMMSTATUS_NOTIFY_ENABLED_EVT,
  ZIGBEEDIRECTCOMM_COMMSTATUS_NOTIFY_DISABLED_EVT,
  ZIGBEEDIRECTCOMM_MANAGEJOINER_WRITE_EVT,
  ZIGBEEDIRECTCOMM_COMMIDENTITY_READ_EVT,
  ZIGBEEDIRECTCOMM_COMMIDENTITY_WRITE_EVT,
  ZIGBEEDIRECTCOMM_FINDBIND_WRITE_EVT,
  /* USER CODE BEGIN Service2_OpcodeEvt_t */

  /* USER CODE END Service2_OpcodeEvt_t */
  ZIGBEEDIRECTCOMM_BOOT_REQUEST_EVT
} ZIGBEEDIRECTCOMM_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service2_Data_t */

  /* USER CODE END Service2_Data_t */
} ZIGBEEDIRECTCOMM_Data_t;

#else
#include "zdd_ble_interface.h"
#endif /* NO_SINGLE_INTERFACE */
typedef struct
{
  ZIGBEEDIRECTCOMM_OpcodeEvt_t       EvtOpcode;
  ZIGBEEDIRECTCOMM_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service2_NotificationEvt_t */
  uint16_t  ZigbeedirectcommSvcHdle;
  uint16_t  CommidentityCharHdle;
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
