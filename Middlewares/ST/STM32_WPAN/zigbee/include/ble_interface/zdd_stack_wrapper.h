/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    zdd_stack_wrapper.h
  * @author  MCD Application Team
  * @brief   Header for ZDD wrapper
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ZDD_STACK_WRAPPER_H
#define ZDD_STACK_WRAPPER_H

/* Exported types ------------------------------------------------------------*/
typedef enum {
  BLE_ZDD_WRAP_HCI_DISCONNECTION_COMPLETE_EVT,
  BLE_ZDD_WRAP_HCI_HARDWARE_ERROR_EVT,
  BLE_ZDD_WRAP_HCI_LE_META_EVT_CONNECTION_UPDATE_COMPLETE_SUBEVT,
  BLE_ZDD_WRAP_HCI_LE_META_EVT_HCI_LE_PHY_UPDATE_COMPLETE_SUBEVT,
  BLE_ZDD_WRAP_HCI_LE_META_EVT_HCI_LE_ENHANCED_CONNECTION_COMPLETE_SUBEVT,
  BLE_ZDD_WRAP_HCI_LE_META_EVT_HCI_LE_CONNECTION_COMPLETE_SUBEVT,
  BLE_ZDD_WRAP_CONN_HANDLE_EVT,
  BLE_ZDD_WRAP_DISCON_HANDLE_EVT,
  BLE_ZDD_WRAP_TUNNELING_TUNNZDTSNPDU_WRITE_EVT,
  BLE_ZDD_WRAP_TUNNELING_TUNNZDTSNPDU_INDICATE_ENABLED_EVT,
  BLE_ZDD_WRAP_TUNNELING_TUNNZDTSNPDU_INDICATE_DISABLED_EVT,
  BLE_ZDD_WRAP_ZDDSECURITY_SECURITY25519AES_WRITE_EVT,
  BLE_ZDD_WRAP_ZDDSECURITY_SECURITY25519AES_INDICATE_ENABLED_EVT,
  BLE_ZDD_WRAP_ZDDSECURITY_SECURITY25519AES_INDICATE_DISABLED_EVT,
  BLE_ZDD_WRAP_ZDDSECURITY_SECURITY25519SHA_WRITE_EVT,
  BLE_ZDD_WRAP_ZDDSECURITY_SECURITY25519SHA_INDICATE_ENABLED_EVT,
  BLE_ZDD_WRAP_ZDDSECURITY_SECURITY25519SHA_INDICATE_DISABLED_EVT,
  BLE_ZDD_WRAP_ZDDSECURITY_P256SHA_WRITE_EVT,
  BLE_ZDD_WRAP_ZDDSECURITY_P256SHA_INDICATE_ENABLED_EVT,
  BLE_ZDD_WRAP_ZDDSECURITY_P256SHA_INDICATE_DISABLED_EVT,
  BLE_ZDD_WRAP_ZIGBEEDIRECTCOMM_FORMNETWORK_WRITE_EVT,
  BLE_ZDD_WRAP_ZIGBEEDIRECTCOMM_JOINNETWORK_WRITE_EVT,
  BLE_ZDD_WRAP_ZIGBEEDIRECTCOMM_PERMITJOIN_WRITE_EVT,
  BLE_ZDD_WRAP_ZIGBEEDIRECTCOMM_LEAVENETWORK_WRITE_EVT,
  BLE_ZDD_WRAP_ZIGBEEDIRECTCOMM_COMMSTATUS_READ_EVT,
  BLE_ZDD_WRAP_ZIGBEEDIRECTCOMM_COMMSTATUS_NOTIFY_ENABLED_EVT,
  BLE_ZDD_WRAP_ZIGBEEDIRECTCOMM_COMMSTATUS_NOTIFY_DISABLED_EVT,
  BLE_ZDD_WRAP_ZIGBEEDIRECTCOMM_MANAGEJOINER_WRITE_EVT,
  BLE_ZDD_WRAP_ZIGBEEDIRECTCOMM_COMMIDENTITY_WRITE_EVT,
  BLE_ZDD_WRAP_ZIGBEEDIRECTCOMM_FINDBIND_WRITE_EVT,
  BLE_ZDD_WRAP_SERVER_CONFIRMATION_EVT,
  
} BLE_ZDD_WRAP_Opcode_evt_t;

typedef struct {
  uint8_t *pPayload;
  uint8_t Length;
} BLE_ZDD_WRAP_Data_t;

typedef struct {
  BLE_ZDD_WRAP_Opcode_evt_t     Opcode;
  BLE_ZDD_WRAP_Data_t           DataTransfered;
  uint16_t                      ConnectionHandle;
  uint16_t                      AttributeHandle;
  uint8_t                       ServiceInstance;
  /* Specific BLE_ZDD_WRAP_HCI_LE_META_EVT_HCI_LE_CONNECTION_COMPLETE_SUBEVT fields */
  uint16_t                      ConnInterval;
  /* Specific BLE_ZDD_WRAP_ZIGBEEDIRECTCOMM_COMMSTATUS_READ_EVT fields */
  uint16_t                      ZigbeedirectcommSvcHdle;
  uint16_t                      CommidentityCharHdle;
} ZDD_Stack_Notification_evt_t;

typedef enum {
  ZDD_BLE_WRAP_TUNNELING_TUNNZDTSNPDU_UPDATEVALUE_EVT,
  ZDD_BLE_WRAP_ZDDSECURITY_SECURITY25519AES_UPDATEVALUE_EVT,
  ZDD_BLE_WRAP_ZDDSECURITY_SECURITY25519SHA_UPDATEVALUE_EVT,
  ZDD_BLE_WRAP_ZDDSECURITY_SECURITY_P256SHA_UPDATEVALUE_EVT,
  ZDD_BLE_WRAP_ZIGBEEDIRECTCOMM_COMMSTATUS_UPDATEVALUE_EVT,
  ZDD_BLE_WRAP_BLE_START_ADVERTISING_EVT,
  ZDD_BLE_WRAP_BLE_STOP_ADVERTISING_EVT,
  ZDD_BLE_WRAP_OS_SHCEDULE_ZDD_TASK_EVT,
  
} ZDD_BLE_WRAP_Opcode_evt_t;

typedef struct {
  uint8_t *pPayload;
  uint8_t Length;
} ZDD_BLE_WRAP_Data_t;

typedef struct {
  uint16_t    PanId;
  uint16_t    NwkAddr;
  bool        PJoin;
} ZDD_BLE_WRAP_AdvData_t;

typedef struct {
  ZDD_BLE_WRAP_Opcode_evt_t     Opcode;
  ZDD_BLE_WRAP_Data_t           Data;
  /* Specific ZDD_BLE_WRAP_BLE_START_ADVERTISING_EVT fields */
  ZDD_BLE_WRAP_AdvData_t       AdvData;
  
} BLE_App_Notification_evt_t;

/* Exported functions prototypes ---------------------------------------------*/

/* Handler used to map the BLE stack to the ZDD stack */
/* Calls from BLE Apps -> ZDD stack (porting)         */
void ZDD_Stack_Notification(ZDD_Stack_Notification_evt_t *pNotification);

/* Handler used to map the ZDD stack to the BLE stack  */
/* Calls from ZDD stack (porting) -> BLE Apps          */
bool BLE_App_Notification(BLE_App_Notification_evt_t *pNotification);

/* Exposed to keep the application OS agnostic */
void Zdd_Task(void);

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
