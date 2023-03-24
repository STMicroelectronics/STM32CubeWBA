/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_ble.h
  * @author  MCD Application Team
  * @brief   Header for ble application
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
#ifndef APP_BLE_H
#define APP_BLE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ble_const.h"  

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/

typedef enum
{
  APP_BLE_IDLE,
  APP_BLE_LP_CONNECTING,
  APP_BLE_CONNECTED_SERVER,
  APP_BLE_CONNECTED_CLIENT,
  APP_BLE_ADV_FAST,
  APP_BLE_ADV_LP,

} APP_BLE_ConnStatus_t;

/**
  * HCI Event Packet Types
  */
typedef __PACKED_STRUCT
{
  uint32_t *next;
  uint32_t *prev;
} BleEvtPacketHeader_t;

typedef __PACKED_STRUCT
{
  uint8_t   evtcode;
  uint8_t   plen;
  uint8_t   payload[1];
} BleEvt_t;

typedef __PACKED_STRUCT
{
  uint8_t   type;
  BleEvt_t  evt;
} BleEvtSerial_t;

typedef __PACKED_STRUCT __ALIGNED(4)
{
  BleEvtPacketHeader_t  header;
  BleEvtSerial_t        evtserial;
} BleEvtPacket_t;

typedef enum
{
  PROC_GAP_GEN_PHY_TOGGLE,
  PROC_GAP_GEN_CONN_TERMINATE,
  PROC_GATT_EXCHANGE_CONFIG,
  /* USER CODE BEGIN ProcGapGeneralId_t*/

  /* USER CODE END ProcGapGeneralId_t */
}ProcGapGeneralId_t;

typedef enum
{
  PROC_GAP_PERIPH_ADVERTISE_START_LP,
  PROC_GAP_PERIPH_ADVERTISE_START_FAST,
  PROC_GAP_PERIPH_ADVERTISE_STOP,
  PROC_GAP_PERIPH_ADVERTISE_DATA_UPDATE,
  PROC_GAP_PERIPH_CONN_PARAM_UPDATE,

  PROC_GAP_PERIPH_SET_BROADCAST_MODE,
  /* USER CODE BEGIN ProcGapPeripheralId_t */

  /* USER CODE END ProcGapPeripheralId_t */
}ProcGapPeripheralId_t;

typedef enum
{
  PROC_GAP_CENTRAL_SCAN_START,
  PROC_GAP_CENTRAL_SCAN_TERMINATE,
  /* USER CODE BEGIN ProcGapCentralId_t */

  /* USER CODE END ProcGapCentralId_t */
}ProcGapCentralId_t;

/* USER CODE BEGIN ET */
typedef struct
{
  uint16_t Conn_Handle;
  uint16_t Max_Transmission_Unit;
  uint16_t Max_Payload_Size;
  uint16_t Initial_Credits;
  uint16_t SPSM;
  uint8_t Channel_Number;
  uint8_t Channel_Index_List; 
}BleCoCContext_t;

typedef enum
{
  BLE_CONN_HANDLE_EVT,
  BLE_DISCON_HANDLE_EVT,
  BLE_CONN_UPDATE_EVT,
  L2CAP_DATA_RECEIVED,
  EXCHANGE_ATT_MTU,
  BLE_SEND_DATA,
} CoC_APP_Opcode_Notification_evt_t;

typedef struct
{
  uint8_t *pPayload;
  uint32_t pPayload_n_1;
  uint32_t pPayload_n;
} CoC_Payload_t;

typedef struct
{
  CoC_APP_Opcode_Notification_evt_t     CoC_Evt_Opcode;
  CoC_Payload_t                         DataTransfered;
  uint16_t                              ConnectionHandle;
  uint8_t                               DataLength;
  uint8_t                               DataTable[(BLE_EVT_MAX_PARAM_LEN - 2) - 3];
} COC_APP_ConnHandle_Not_evt_t;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */
extern BleCoCContext_t BleCoCContextPeriph;
/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
#define SCAN_WIN_MS(x) ((uint16_t)((x)/0.625f))
#define SCAN_INT_MS(x) ((uint16_t)((x)/0.625f))
#define CONN_INT_MS(x) ((uint16_t)((x)/1.25f))
#define CONN_SUP_TIMEOUT_MS(x) ((uint16_t)((x)/10.0f))
#define CONN_CE_LENGTH_MS(x) ((uint16_t)((x)/0.625f))
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ---------------------------------------------*/
void APP_BLE_Init(void);
APP_BLE_ConnStatus_t APP_BLE_Get_Server_Connection_Status(void);
void APP_BLE_Procedure_Gap_General(ProcGapGeneralId_t ProcGapGeneralId);
void APP_BLE_Procedure_Gap_Peripheral(ProcGapPeripheralId_t ProcGapPeripheralId);
/* USER CODE BEGIN EF */
void APP_Periph_BLE_Key_Button1_Action(void);
void APP_Periph_BLE_Key_Button2_Action(void);
void APP_Periph_BLE_Key_Button3_Action(void);
void COC_PERIPH_APP_Notification( COC_APP_ConnHandle_Not_evt_t *pNotification );
void PeriphSendData( void );
void BLE_SVC_L2CAP_Conn_Update(uint16_t ConnectionHandle, uint16_t conn1, uint16_t conn2);
/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*APP_BLE_H */
