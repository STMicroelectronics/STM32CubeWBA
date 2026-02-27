/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gatt_client_app.h
  * @author  MCD Application Team
  * @brief   Header for gatt_client_app.c module
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
#ifndef GATT_CLIENT_APP_H
#define GATT_CLIENT_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  GATT_CLIENT_APP_DISCONNECTED,
  GATT_CLIENT_APP_CONNECTED,
  GATT_CLIENT_APP_DISCOVER_SERVICES,
  /* USER CODE BEGIN GATT_CLIENT_APP_State_t */

  /* USER CODE END GATT_CLIENT_APP_State_t */
}GATT_CLIENT_APP_State_t;

typedef enum
{
  PEER_CONN_HANDLE_EVT,
  PEER_DISCON_HANDLE_EVT,
  /* USER CODE BEGIN GATT_CLIENT_APP_Conn_Opcode_t */

  /* USER CODE END GATT_CLIENT_APP_Conn_Opcode_t */
}GATT_CLIENT_APP_Conn_Opcode_t;

typedef struct
{
  GATT_CLIENT_APP_Conn_Opcode_t          ConnOpcode;
  uint16_t                              ConnHdl;
}GATT_CLIENT_APP_ConnHandle_Notif_evt_t;
/* USER CODE BEGIN ET */
typedef struct
{
  uint8_t *pPayload;
  uint32_t pPayload_n_1;
  uint32_t pPayload_n;
  uint32_t Length;
} DTC_STM_Payload_t;

typedef enum
{
  DTC_APP_FLOW_OFF,
  DTC_APP_FLOW_ON
} DTC_App_Flow_Status_t;

typedef enum
{
  DTC_APP_TRANSFER_REQ_OFF,
  DTC_APP_TRANSFER_REQ_ON
} DTC_App_Transfer_Req_Status_t;

typedef struct
{
  DTC_STM_Payload_t TxData;
  DTC_App_Transfer_Req_Status_t NotificationTransferReq;
  DTC_App_Transfer_Req_Status_t ButtonTransferReq;
  DTC_App_Flow_Status_t DtFlowStatus;
  uint8_t connectionstatus;
} DTC_Context_t;

typedef struct
{
  DTC_STM_Payload_t       DataTransfered;
  uint16_t                ConnectionHandle;
  uint8_t                 ServiceInstance;
} DT_CLI_NotificationEvt_t;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/

/* USER CODE BEGIN EC */
typedef enum
{
  DTC_UNINITIALIZED                                 = 0,
  DTC_INITIALIZED                                   = 1,
  DTC_IDLE                                          = 2,
  DTC_DISC_ALL_PRIMARY_SERVICES                     = 3,
  DTC_DISCOVER_CHARACS                              = 4,
  DTC_DISCOVER_DESC                                 = 5,
  DTC_READ_TX                                       = 6,
  DTC_READ_TX_CCC                                   = 7,
  DTC_READ_RX                                       = 8,
  DTC_WRITE_TX                                      = 9,
  DTC_ENABLE_TX_NOTIFICATION                        = 10,
  DTC_DISABLE_TX_NOTIFICATION                       = 11,
  DTC_ENABLE_THROUGHPUT_NOTIFICATION                = 12,
  DTC_DISABLE_THROUGHPUT_NOTIFICATION               = 13,
  DTC_CONNECTED                                     = 14,
  DTC_CONNECTED_ADVERTISING                         = 15,
}  DTC_SVC_ProfileState;

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void GATT_CLIENT_APP_Init(void);
void GATT_CLIENT_APP_Notification(GATT_CLIENT_APP_ConnHandle_Notif_evt_t *p_Notif);
tBleStatus GATT_CLIENT_APP_Set_Conn_Handle(uint8_t index, uint16_t connHdle);
int GATT_CLIENT_APP_Get_Index(uint16_t connection_handle);
void GATT_CLIENT_APP_DiscoverServices(uint16_t connection_handle, uint8_t start_task);
void GATT_CLIENT_APP_DiscoverServicesWithIndex(uint8_t index);
uint8_t GATT_CLIENT_APP_Get_State(uint8_t index);
tBleStatus GATT_CLIENT_APP_ReadCharacteristic(uint16_t connection_handle, uint16_t ValueHdl, uint16_t *data_length_p, uint8_t **data_p, uint8_t use_read_long);
tBleStatus GATT_CLIENT_APP_WriteCharacteristic(uint16_t connection_handle, uint16_t ValueHdl, uint16_t data_length, uint8_t *data);
/* USER CODE BEGIN EFP */
void DTC_Button1TriggerReceived( void );
void DTC_Button2TriggerReceived( void );
void DTC_Button3TriggerReceived( void );

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*GATT_CLIENT_APP_H */
