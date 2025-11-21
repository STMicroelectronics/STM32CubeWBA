/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gatt_client_app.c
  * @author  MCD Application Team
  * @brief   GATT Client Application
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

/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "log_module.h"
#include "app_common.h"
#include "dbg_trace.h"
#include "ble_core.h"
#include "uuid.h"
#include "ble_conf.h"
#include "svc_ctl.h"
#include "gatt_client_app.h"
#include "stm32_seq.h"
#include "app_ble.h"
#include "app_conf.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_bsp.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */
typedef struct
{
  uint8_t selection;
  uint8_t level;
}P2PButton_t;

/* USER CODE END PTD */

typedef enum
{
  NOTIFICATION_INFO_RECEIVED_EVT,
  /* USER CODE BEGIN GATT_CLIENT_APP_Opcode_t */
  P2P_START_TIMER_EVT,
  P2P_STOP_TIMER_EVT,
  P2P_NOTIFICATION_INFO_RECEIVED_EVT,

  /* USER CODE END GATT_CLIENT_APP_Opcode_t */
}GATT_CLIENT_APP_Opcode_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t length;
}GATT_CLIENT_APP_Data_t;

typedef struct
{
  GATT_CLIENT_APP_Opcode_t Client_Evt_Opcode;
  GATT_CLIENT_APP_Data_t   DataTransfered;
}GATT_CLIENT_APP_Notification_evt_t;

typedef enum
{
  SVC_ID_GAP = 0,
  SVC_ID_GATT,
  /* USER CODE BEGIN svcId_t */
  SVC_ID_P2P,

  /* USER CODE END svcId_t */
  SVC_ID_MAX
}svcId_t;

typedef enum
{
  SVC_INFO_STATUS_EMPTY = 0,
  SVC_INFO_STATUS_FOUND,
  SVC_INFO_STATUS_DISCOVERED,
  SVC_INFO_STATUS_ERROR,
  /* USER CODE BEGIN svcInfoStatus_t */

  /* USER CODE END svcInfoStatus_t */
  SVC_INFO_STATUS_MAX
}svcInfoStatus_t;

typedef struct
{
  uint16_t uuid;

  uint16_t charStartHdl;
  uint16_t charEndHdl;
  uint16_t descStartHdl;
  uint16_t descEndHdl;

  uint16_t status;
  /* USER CODE BEGIN SvcInfo_t */

  /* USER CODE END SvcInfo_t */

}SvcInfo_t;

typedef struct
{
  GATT_CLIENT_APP_State_t state;

  APP_BLE_ConnStatus_t connStatus;
  uint16_t connHdl;

  SvcInfo_t svcInfo[SVC_ID_MAX];

  uint16_t ServiceChangedCharStartHdl;
  uint16_t ServiceChangedCharValueHdl;
  uint16_t ServiceChangedCharDescHdl;
  uint16_t ServiceChangedCharEndHdl;
  uint8_t ServiceChangedCharProperties;
  /* USER CODE BEGIN BleClientAppContext_t */

  /* SVC_ID_P2P characteristics handle */
  /* handles of the Tx characteristic - Write To Server */
  uint16_t P2PWriteToServerCharHdl;
  uint16_t P2PWriteToServerValueHdl;
  uint16_t P2PWriteToServerDescHdl;
  /* handles of the Rx characteristic - Notification From Server */
  uint16_t P2PNotificationCharHdl;
  uint16_t P2PNotificationValueHdl;
  uint16_t P2PNotificationDescHdl;

  /* handles of the Tx characteristic - Write To Server with response */
  uint16_t P2PLongWritewoRespToServerCharHdl;
  uint16_t P2PLongWritewoRespToServerValueHdl;
  uint16_t P2PLongWritewoRespToServerDescHdl;

  /* USER CODE END BleClientAppContext_t */

}BleClientAppContext_t;

/* Private defines ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros -------------------------------------------------------------*/
#define UNPACK_2_BYTE_PARAMETER(ptr)  \
        (uint16_t)((uint16_t)(*((uint8_t *)ptr))) |   \
        (uint16_t)((((uint16_t)(*((uint8_t *)ptr + 1))) << 8))
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

static BleClientAppContext_t a_ClientContext[BLE_CFG_CLT_MAX_NBR_CB];
static uint16_t gattCharStartHdl = 0;
static uint16_t gattCharValueHdl = 0;

/* USER CODE BEGIN PV */

static uint8_t P2PLedSelection;
static uint8_t P2PLedLevel;
static P2PButton_t P2PButtonData;

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/
/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private function prototypes -----------------------------------------------*/
static SVCCTL_EvtAckStatus_t Event_Handler(void *Event);
static void gatt_parse_services(aci_att_read_by_group_type_resp_event_rp0 *p_evt);
static void gatt_parse_services_by_UUID(aci_att_find_by_type_value_resp_event_rp0 *p_evt);
static void gatt_parse_chars(aci_att_read_by_type_resp_event_rp0 *p_evt);
static void gatt_parse_descs(aci_att_find_info_resp_event_rp0 *p_evt);
static void gatt_parse_notification(aci_gatt_notification_event_rp0 *p_evt);
static void gatt_Notification(GATT_CLIENT_APP_Notification_evt_t *p_Notif);
static void client_discover_all(void);
static void gatt_cmd_resp_release(void);
static void gatt_cmd_resp_wait(void);
/* USER CODE BEGIN PFP */

static void P2Pclient_write_char(void);
static void P2Pclient_longwrite_woresp_char(void);
static void Gatt_Config_Exchange(void);

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void GATT_CLIENT_APP_Init(void)
{
  uint8_t index =0, svcIndex=0;
  /* USER CODE BEGIN GATT_CLIENT_APP_Init_1 */

  /* USER CODE END GATT_CLIENT_APP_Init_1 */
  for(index = 0; index < BLE_CFG_CLT_MAX_NBR_CB; index++)
  {
    a_ClientContext[index].connStatus = APP_BLE_IDLE;

    for(svcIndex = 0; svcIndex < SVC_ID_MAX; svcIndex++)
    {
      a_ClientContext[index].svcInfo[svcIndex].uuid = 0;
      a_ClientContext[index].svcInfo[svcIndex].charStartHdl = 0;
      a_ClientContext[index].svcInfo[svcIndex].charEndHdl = 0;
      a_ClientContext[index].svcInfo[svcIndex].descStartHdl = 0;
      a_ClientContext[index].svcInfo[svcIndex].descEndHdl = 0;
      a_ClientContext[index].svcInfo[svcIndex].status = SVC_INFO_STATUS_EMPTY;
      /* USER CODE BEGIN GATT_CLIENT_APP_Init_3 */

      /* USER CODE END GATT_CLIENT_APP_Init_3 */
    }

    a_ClientContext[index].svcInfo[SVC_ID_GAP].uuid  = GAP_SERVICE_UUID;
    a_ClientContext[index].svcInfo[SVC_ID_GATT].uuid = GATT_SERVICE_UUID;
    /* USER CODE BEGIN GATT_CLIENT_APP_Init_4 */
    a_ClientContext[index].svcInfo[SVC_ID_P2P].uuid  = P2P_SERVICE_UUID;

    /* USER CODE END GATT_CLIENT_APP_Init_4 */
  }

  /* Register the event handler to the BLE controller */
  SVCCTL_RegisterCltHandler(Event_Handler);

  /* Register a task allowing to discover all services and characteristics and enable all notifications */
  UTIL_SEQ_RegTask(1U << CFG_TASK_DISCOVER_SERVICES_ID, UTIL_SEQ_RFU, client_discover_all);

  /* USER CODE BEGIN GATT_CLIENT_APP_Init_2 */

  UTIL_SEQ_RegTask(1U << CFG_TASK_P2PC_WRITE_CHAR_ID, UTIL_SEQ_RFU, P2Pclient_write_char);
  UTIL_SEQ_RegTask(1U << CFG_TASK_P2PC_LONG_WRITE_CHAR_ID, UTIL_SEQ_RFU,P2Pclient_longwrite_woresp_char);

  P2PLedSelection = 0x00;
  P2PLedLevel = 0x00;

  P2PButtonData.selection = 0x01;
  P2PButtonData.level = 0x00;

  /* USER CODE END GATT_CLIENT_APP_Init_2 */
  return;
}

void GATT_CLIENT_APP_Notification(GATT_CLIENT_APP_ConnHandle_Notif_evt_t *p_Notif)
{
  /* USER CODE BEGIN GATT_CLIENT_APP_Notification_1 */

  /* USER CODE END GATT_CLIENT_APP_Notification_1 */
  switch(p_Notif->ConnOpcode)
  {
    /* USER CODE BEGIN ConnOpcode */

    /* USER CODE END ConnOpcode */

    case PEER_CONN_HANDLE_EVT :
      /* USER CODE BEGIN PEER_CONN_HANDLE_EVT */

      /* USER CODE END PEER_CONN_HANDLE_EVT */
      break;

    case PEER_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN PEER_DISCON_HANDLE_EVT */
    {
      uint8_t index = 0;
      for(index = 0 ; index < BLE_CFG_CLT_MAX_NBR_CB ; index++)
      {
        if (a_ClientContext[index].connHdl == p_Notif->ConnHdl)
        {
          a_ClientContext[index].state = GATT_CLIENT_APP_IDLE;
        }
      }
    }

      /* USER CODE END PEER_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN ConnOpcode_Default */

      /* USER CODE END ConnOpcode_Default */
      break;
  }
  /* USER CODE BEGIN GATT_CLIENT_APP_Notification_2 */

  /* USER CODE END GATT_CLIENT_APP_Notification_2 */
  return;
}

uint8_t GATT_CLIENT_APP_Set_Conn_Handle(uint8_t index, uint16_t connHdl)
{
  uint8_t ret;

  if (index < BLE_CFG_CLT_MAX_NBR_CB)
  {
    a_ClientContext[index].connHdl = connHdl;
    ret = 0;
  }
  else
  {
    ret = 1;
  }

  return ret;
}

uint8_t GATT_CLIENT_APP_Get_State(uint8_t index)
{
  return a_ClientContext[index].state;
}

void GATT_CLIENT_APP_Discover_services(uint8_t index)
{
  GATT_CLIENT_APP_Procedure_Gatt(index, PROC_GATT_DISC_ALL_PRIMARY_SERVICES);
  GATT_CLIENT_APP_Procedure_Gatt(index, PROC_GATT_DISC_ALL_CHARS);
  GATT_CLIENT_APP_Procedure_Gatt(index, PROC_GATT_DISC_ALL_DESCS);
  GATT_CLIENT_APP_Procedure_Gatt(index, PROC_GATT_PROPERTIES_ENABLE_ALL);

  return;
}

uint8_t GATT_CLIENT_APP_Procedure_Gatt(uint8_t index, ProcGattId_t GattProcId)
{
  uint16_t svcIdx;
  tBleStatus bleStatus = BLE_STATUS_SUCCESS;
  uint8_t status;

  if (index >= BLE_CFG_CLT_MAX_NBR_CB)
  {
    status = 1;
  }
  else
  {
    status = 0;
    switch (GattProcId)
    {
      case PROC_GATT_DISC_ALL_PRIMARY_SERVICES:
      {
        a_ClientContext[index].state = GATT_CLIENT_APP_DISCOVER_SERVICES;

        LOG_INFO_APP("GATT services discovery\n");
        bleStatus = aci_gatt_disc_all_primary_services(a_ClientContext[index].connHdl);

        if (bleStatus == BLE_STATUS_SUCCESS)
        {
          gatt_cmd_resp_wait();
          LOG_INFO_APP("PROC_GATT_DISC_ALL_PRIMARY_SERVICES services discovered Successfully\n\n");
        }
        else
        {
          LOG_ERROR_APP("PROC_GATT_DISC_ALL_PRIMARY_SERVICES aci_gatt_disc_all_primary_services cmd Failed, status =0x%02X\n\n", bleStatus);
          status++;
        }
      }
      break; /* PROC_GATT_DISC_ALL_PRIMARY_SERVICES */

      case PROC_GATT_DISC_ALL_CHARS:
      {
        a_ClientContext[index].state = GATT_CLIENT_APP_DISCOVER_CHARACS;

        LOG_INFO_APP("DISCOVER_ALL_CHARS on ConnHdl=0x%04X\n",a_ClientContext[index].connHdl);
        for (svcIdx = 0; svcIdx < SVC_ID_MAX; svcIdx++)
        {
          if(a_ClientContext[index].svcInfo[svcIdx].uuid != 0)
          {
            LOG_INFO_APP("  Service UUID 0x%04X, svcHdl[0x%04X - 0x%04X]\n",
                              a_ClientContext[index].svcInfo[svcIdx].uuid,
                              a_ClientContext[index].svcInfo[svcIdx].charStartHdl,
                              a_ClientContext[index].svcInfo[svcIdx].charEndHdl);
            bleStatus = aci_gatt_disc_all_char_of_service(
                              a_ClientContext[index].connHdl,
                              a_ClientContext[index].svcInfo[svcIdx].charStartHdl,
                              a_ClientContext[index].svcInfo[svcIdx].charEndHdl);

            if (bleStatus == BLE_STATUS_SUCCESS)
            {
              gatt_cmd_resp_wait();
              LOG_INFO_APP("  Service UUID 0x%04X characteristics discovered Successfully\n\n",
                            a_ClientContext[index].svcInfo[svcIdx].uuid);
              a_ClientContext[index].svcInfo[svcIdx].status = SVC_INFO_STATUS_DISCOVERED;
            }
            else
            {
              LOG_ERROR_APP("  Service UUID 0x%04X characteristics discovery Failed, status =0x%02X\n\n",
                            a_ClientContext[index].svcInfo[svcIdx].uuid, bleStatus);
              status++;
            }
            /* USER CODE BEGIN PROC_GATT_DISC_ALL_CHARS_0 */

            /* USER CODE END PROC_GATT_DISC_ALL_CHARS_0 */
          }
        }
        /* USER CODE BEGIN PROC_GATT_DISC_ALL_CHARS */

        /* USER CODE END PROC_GATT_DISC_ALL_CHARS */
        if (status == 0)
        {
          LOG_INFO_APP("All characteristics discovered Successfully\n\n");
        }
        else
        {
          LOG_ERROR_APP("All characteristics discovery Failed\n\n");
        }
      }
      break; /* PROC_GATT_DISC_ALL_CHARS */

      case PROC_GATT_DISC_ALL_DESCS:
      {
        a_ClientContext[index].state = GATT_CLIENT_APP_DISCOVER_WRITE_DESC;

        LOG_INFO_APP("DISCOVER_ALL_CHAR_DESCS on ConnHdl=0x%04X\n",
                              a_ClientContext[index].connHdl);
        for (svcIdx = 0; svcIdx < SVC_ID_MAX; svcIdx++)
        {
          if(a_ClientContext[index].svcInfo[svcIdx].uuid != 0)
          {
            LOG_INFO_APP("  Service UUID 0x%04X, svcHdl[0x%04X - 0x%04X]\n",
                              a_ClientContext[index].svcInfo[svcIdx].uuid,
                              a_ClientContext[index].svcInfo[svcIdx].charStartHdl,
                              a_ClientContext[index].svcInfo[svcIdx].charEndHdl);
            bleStatus = aci_gatt_disc_all_char_desc(
                              a_ClientContext[index].connHdl,
                              a_ClientContext[index].svcInfo[svcIdx].charStartHdl,
                              a_ClientContext[index].svcInfo[svcIdx].charEndHdl);

            if (bleStatus == BLE_STATUS_SUCCESS)
            {
              gatt_cmd_resp_wait();
              LOG_INFO_APP("  Descriptors discovered Successfully\n\n");
              a_ClientContext[index].svcInfo[svcIdx].status = SVC_INFO_STATUS_DISCOVERED;
            }
            else
            {
              LOG_ERROR_APP("  Descriptors discovery Failed, status =0x%02X\n\n", bleStatus);
              status++;
            }
            /* USER CODE BEGIN PROC_GATT_DISC_ALL_DESCS_0 */

            /* USER CODE END PROC_GATT_DISC_ALL_DESCS_0 */
          }
        }
        /* USER CODE BEGIN PROC_GATT_DISC_ALL_DESCS */

        /* USER CODE END PROC_GATT_DISC_ALL_DESCS */
        if (status == 0)
        {
          LOG_INFO_APP("All descriptors discovered Successfully\n\n");
        }
        else
        {
          LOG_ERROR_APP("All descriptors discovery Failed\n\n");
        }
      }
      break; /* PROC_GATT_DISC_ALL_DESCS */
      case PROC_GATT_PROPERTIES_ENABLE_ALL:
      {
        uint16_t charPropVal = 0x0000;

        if (a_ClientContext[index].ServiceChangedCharDescHdl != 0x0000)
        {
          if(((a_ClientContext[index].ServiceChangedCharProperties) & CHAR_PROP_NOTIFY) == CHAR_PROP_NOTIFY)
          {
            charPropVal = 0x0001;
          }
          else if(((a_ClientContext[index].ServiceChangedCharProperties) & CHAR_PROP_INDICATE) == CHAR_PROP_INDICATE)
          {
            charPropVal = 0x0002;
          }
          bleStatus = aci_gatt_write_char_value(a_ClientContext[index].connHdl,
                                                a_ClientContext[index].ServiceChangedCharDescHdl,
                                                2,
                                                (uint8_t *) &charPropVal);
          if (bleStatus == BLE_STATUS_SUCCESS)
          {
            gatt_cmd_resp_wait();
            LOG_INFO_APP(" ServiceChangedCharDescHdl =0x%04X\n",a_ClientContext[index].ServiceChangedCharDescHdl);
          }
          else
          {
            LOG_ERROR_APP(" ServiceChangedCharDescHdl write Failed, status =0x%02X\n", bleStatus);
            status++;
          }
        }
        /* USER CODE BEGIN PROC_GATT_PROPERTIES_ENABLE_ALL */

        if(a_ClientContext[index].P2PNotificationDescHdl != 0x0000)
        {
          charPropVal = 0x0001;
          bleStatus = aci_gatt_write_char_value(a_ClientContext[index].connHdl,
                                                a_ClientContext[index].P2PNotificationDescHdl,
                                                2,
                                                (uint8_t *) &charPropVal);

          if (bleStatus == BLE_STATUS_SUCCESS)
          {
            gatt_cmd_resp_wait();
            LOG_INFO_APP(" aci_gatt_write_char_value sucess P2PNotificationDescHdl =0x%04X\n",
                          a_ClientContext[index].P2PNotificationDescHdl);
          }
          else
          {
            LOG_ERROR_APP(" P2PNotificationDescHdl write Failed, status =0x%02X\n", bleStatus);
            status++;
          }
        }
        Gatt_Config_Exchange();
        /* USER CODE END PROC_GATT_PROPERTIES_ENABLE_ALL */

        if (status == 0)
        {
          LOG_INFO_APP("All properties enabled Successfully\n\n");
        }
        else
        {
          LOG_ERROR_APP("All properties enabled Failed\n\n");
        }
      }
      break; /* PROC_GATT_PROPERTIES_ENABLE_ALL */

    default:
      break;
    }
  }

  return status;
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

/**
 * @brief  Event handler
 * @param  Event: Address of the buffer holding the Event
 * @retval Ack: Return whether the Event has been managed or not
 */
static SVCCTL_EvtAckStatus_t Event_Handler(void *Event)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *event_pckt;
  evt_blecore_aci *p_blecore_evt;

  GATT_CLIENT_APP_Notification_evt_t Notification;
  UNUSED(Notification);

  return_value = SVCCTL_EvtNotAck;
  event_pckt = (hci_event_pckt *)(((hci_uart_pckt*)Event)->data);

  switch (event_pckt->evt)
  {
    case HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE:
    {
      p_blecore_evt = (evt_blecore_aci*)event_pckt->data;
      switch (p_blecore_evt->ecode)
      {
        case ACI_ATT_READ_BY_GROUP_TYPE_RESP_VSEVT_CODE:
        {
          aci_att_read_by_group_type_resp_event_rp0 *p_evt_rsp = (void*)p_blecore_evt->data;
          gatt_parse_services((aci_att_read_by_group_type_resp_event_rp0 *)p_evt_rsp);
        }
        break; /* ACI_ATT_READ_BY_GROUP_TYPE_RESP_VSEVT_CODE */
        case ACI_ATT_FIND_BY_TYPE_VALUE_RESP_VSEVT_CODE:
        {
          aci_att_find_by_type_value_resp_event_rp0 *p_evt_rsp = (void*) p_blecore_evt->data;
          gatt_parse_services_by_UUID((aci_att_find_by_type_value_resp_event_rp0 *)p_evt_rsp);
        }
        break; /* ACI_ATT_FIND_BY_TYPE_VALUE_RESP_VSEVT_CODE */
        case ACI_ATT_READ_BY_TYPE_RESP_VSEVT_CODE:
        {
          aci_att_read_by_type_resp_event_rp0 *p_evt_rsp = (void*)p_blecore_evt->data;
          gatt_parse_chars((aci_att_read_by_type_resp_event_rp0 *)p_evt_rsp);
        }
        break; /* ACI_ATT_READ_BY_TYPE_RESP_VSEVT_CODE */
        case ACI_ATT_FIND_INFO_RESP_VSEVT_CODE:
        {
          aci_att_find_info_resp_event_rp0 *p_evt_rsp = (void*)p_blecore_evt->data;
          gatt_parse_descs((aci_att_find_info_resp_event_rp0 *)p_evt_rsp);
        }
        break; /* ACI_ATT_FIND_INFO_RESP_VSEVT_CODE */
        case ACI_GATT_NOTIFICATION_VSEVT_CODE:
        {
          aci_gatt_notification_event_rp0 *p_evt_rsp = (void*)p_blecore_evt->data;
          gatt_parse_notification((aci_gatt_notification_event_rp0 *)p_evt_rsp);
        }
        break;/* ACI_GATT_NOTIFICATION_VSEVT_CODE */
        case ACI_GATT_PROC_COMPLETE_VSEVT_CODE:
        {
          aci_gatt_proc_complete_event_rp0 *p_evt_rsp = (void*)p_blecore_evt->data;
          if(p_evt_rsp->Error_Code != BLE_STATUS_SUCCESS)
          {
            LOG_INFO_APP("\n GATT procedure ended unsuccessfully, error code: 0x%02X\n",
                         p_evt_rsp->Error_Code);
          }

          /* Release GATT command response regardless of procedure success or failure */
          gatt_cmd_resp_release();
        }
        break;/* ACI_GATT_PROC_COMPLETE_VSEVT_CODE */
        case ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE:
        {
          aci_att_exchange_mtu_resp_event_rp0 *tx_pool_available;
          tx_pool_available = (aci_att_exchange_mtu_resp_event_rp0 *)p_blecore_evt->data;
          UNUSED(tx_pool_available);
          /* USER CODE BEGIN ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */

          /* USER CODE END ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */
        }
        break;/* ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE*/
        case ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE:
        {
          aci_att_exchange_mtu_resp_event_rp0 * exchange_mtu_resp;
          exchange_mtu_resp = (aci_att_exchange_mtu_resp_event_rp0 *)p_blecore_evt->data;
          LOG_INFO_APP("  MTU exchanged size = %d\n",exchange_mtu_resp->Server_RX_MTU );
          UNUSED(exchange_mtu_resp);
          /* USER CODE BEGIN ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */

          /* USER CODE END ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
        }
        break;

      /* USER CODE BEGIN VENDOR_SPECIFIC_DEBUG_EVT_CODE_1 */

      /* USER CODE END VENDOR_SPECIFIC_DEBUG_EVT_CODE_1 */
      default:
        /* USER CODE BEGIN VENDOR_SPECIFIC_DEBUG_EVT_CODE_DEFAULT */

        /* USER CODE END VENDOR_SPECIFIC_DEBUG_EVT_CODE_DEFAULT */
        break;
      }/* end switch (p_blecore_evt->ecode) */
    }
    break; /* HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE */
    /* USER CODE BEGIN GATT_CLIENT_EVENT_PACKET_1 */

    /* USER CODE END GATT_CLIENT_EVENT_PACKET_1 */
    default:
      /* USER CODE BEGIN GATT_CLIENT_EVENT_PACKET_DEFAULT */

      /* USER CODE END GATT_CLIENT_EVENT_PACKET_DEFAULT */
      break;
  }/* end switch (event_pckt->evt) */

  return(return_value);
}

__USED static void gatt_Notification(GATT_CLIENT_APP_Notification_evt_t *p_Notif)
{
  /* USER CODE BEGIN gatt_Notification_1 */

  /* USER CODE END gatt_Notification_1 */
  switch (p_Notif->Client_Evt_Opcode)
  {
    /* USER CODE BEGIN Client_Evt_Opcode */
    case P2P_NOTIFICATION_INFO_RECEIVED_EVT:
    {
      P2PLedSelection = p_Notif->DataTransfered.p_Payload[0];
      UNUSED(P2PLedSelection);
      P2PLedLevel = p_Notif->DataTransfered.p_Payload[1];

      if (P2PLedLevel == 0x00)
      {
        APP_BSP_LED_Off(LED_BLUE);
        LOG_INFO_APP("  P2P APPLICATION CLIENT : NOTIFICATION RECEIVED - LED OFF \n");
      }
      else
      {
        APP_BSP_LED_On(LED_BLUE);
        LOG_INFO_APP("  P2P APPLICATION CLIENT : NOTIFICATION RECEIVED - LED ON\n");
      }
      break;
    }

    /* USER CODE END Client_Evt_Opcode */

    case NOTIFICATION_INFO_RECEIVED_EVT:
      /* USER CODE BEGIN NOTIFICATION_INFO_RECEIVED_EVT */

      /* USER CODE END NOTIFICATION_INFO_RECEIVED_EVT */
      break;

    default:
      /* USER CODE BEGIN Client_Evt_Opcode_Default */

      /* USER CODE END Client_Evt_Opcode_Default */
      break;
  }
  /* USER CODE BEGIN gatt_Notification_2 */

  /* USER CODE END gatt_Notification_2 */
  return;
}

/**
* function of GATT service parse
*/
static void gatt_parse_services(aci_att_read_by_group_type_resp_event_rp0 *p_evt)
{
  uint16_t uuid, ServiceStartHdl, ServiceEndHdl;
  uint8_t uuid_offset, uuid_size = 0U, uuid_short_offset = 0U;
  uint8_t i, j, idx, numServ, index;

  LOG_INFO_APP("ACI_ATT_READ_BY_GROUP_TYPE_RESP_VSEVT_CODE - ConnHdl=0x%04X\n",
                p_evt->Connection_Handle);

  for (index = 0 ; index < BLE_CFG_CLT_MAX_NBR_CB ; index++)
  {
    if (a_ClientContext[index].connHdl == p_evt->Connection_Handle)
    {
      break;
    }
  }

  /* index < BLE_CFG_CLT_MAX_NBR_CB means connection handle identified */
  if (index < BLE_CFG_CLT_MAX_NBR_CB)
  {
    /* Number of attribute value tuples */
    numServ = (p_evt->Data_Length) / p_evt->Attribute_Data_Length;

    /* event data in Attribute_Data_List contains:
    * 2 bytes for start handle
    * 2 bytes for end handle
    * 2 or 16 bytes data for UUID
    */
    uuid_offset = 4;           /*UUID offset in bytes in Attribute_Data_List */
    if (p_evt->Attribute_Data_Length == 20) /* we are interested in the UUID is 128 bit.*/
    {
      idx = 16;                /*UUID index of 2 bytes read part in Attribute_Data_List */
      uuid_size = 16;          /*UUID size in bytes */
      uuid_short_offset = 12;  /*UUID offset of 2 bytes read part in UUID field*/
    }
    if (p_evt->Attribute_Data_Length == 6) /* we are interested in the UUID is 16 bit.*/
    {
      idx = 4;
      uuid_size = 2;
      uuid_short_offset = 0;
    }
    UNUSED(idx);
    UNUSED(uuid_size);

    /* Loop on number of attribute value tuples */
    for (i = 0; i < numServ; i++)
    {
      ServiceStartHdl = UNPACK_2_BYTE_PARAMETER(&p_evt->Attribute_Data_List[uuid_offset - 4]);
      ServiceEndHdl   = UNPACK_2_BYTE_PARAMETER(&p_evt->Attribute_Data_List[uuid_offset - 2]);
      uuid            = UNPACK_2_BYTE_PARAMETER(&p_evt->Attribute_Data_List[uuid_offset + uuid_short_offset]);
      LOG_INFO_APP("  %d/%d short UUID=0x%04X, handle [0x%04X - 0x%04X]",
                   i + 1, numServ, uuid, ServiceStartHdl,ServiceEndHdl);

      for (j = 0; j < SVC_ID_MAX; j++)
      {
        if(uuid == a_ClientContext[index].svcInfo[j].uuid)
        {
          a_ClientContext[index].svcInfo[j].charStartHdl  = ServiceStartHdl;
          a_ClientContext[index].svcInfo[j].charEndHdl    = ServiceEndHdl;
          a_ClientContext[index].svcInfo[j].status        = SVC_INFO_STATUS_FOUND;
          LOG_INFO_APP(", UUID 0x%04X found",a_ClientContext[index].svcInfo[j].uuid);
        }
        /* USER CODE BEGIN gatt_parse_services_2 */

        /* USER CODE END gatt_parse_services_2 */
      }
      LOG_INFO_APP("\n");
      /* USER CODE BEGIN gatt_parse_services_1 */

      /* USER CODE END gatt_parse_services_1 */

      uuid_offset += p_evt->Attribute_Data_Length;
    }
  }
  else
  {
    LOG_INFO_APP("ACI_ATT_READ_BY_GROUP_TYPE_RESP_VSEVT_CODE, failed no free index in connection table !\n");
  }

  return;
}

/**
* function of GATT service parse by UUID
*/
static void gatt_parse_services_by_UUID(aci_att_find_by_type_value_resp_event_rp0 *p_evt)
{
  uint8_t i;

  LOG_INFO_APP("ACI_ATT_FIND_BY_TYPE_VALUE_RESP_VSEVT_CODE - ConnHdl=0x%04X, Num_of_Handle_Pair=%d\n",
                p_evt->Connection_Handle,
                p_evt->Num_of_Handle_Pair);

  for(i = 0 ; i < p_evt->Num_of_Handle_Pair ; i++)
  {
    LOG_INFO_APP("ACI_ATT_FIND_BY_TYPE_VALUE_RESP_VSEVT_CODE - PaitId=%d Found_Attribute_Handle=0x%04X, Group_End_Handle=0x%04X\n",
                  i,
                  p_evt->Attribute_Group_Handle_Pair[i].Found_Attribute_Handle,
                  p_evt->Attribute_Group_Handle_Pair[i].Group_End_Handle);
  }

/* USER CODE BEGIN gatt_parse_services_by_UUID_1 */

/* USER CODE END gatt_parse_services_by_UUID_1 */

  return;
}

/**
* function of GATT characteristics parse
*/
static void gatt_parse_chars(aci_att_read_by_type_resp_event_rp0 *p_evt)
{
  uint16_t uuid, CharStartHdl, CharValueHdl;
  uint8_t uuid_offset, uuid_size = 0U, uuid_short_offset = 0U;
  uint8_t i, idx, numHdlValuePair, index;
  uint8_t CharProperties;

  LOG_INFO_APP("  ACI_ATT_READ_BY_TYPE_RESP_VSEVT_CODE - ConnHdl=0x%04X\n",
                p_evt->Connection_Handle);

  for (index = 0 ; index < BLE_CFG_CLT_MAX_NBR_CB ; index++)
  {
    if (a_ClientContext[index].connHdl == p_evt->Connection_Handle)
    {
      break;
    }
  }

  /* index < BLE_CFG_CLT_MAX_NBR_CB means connection handle identified */
  if (index < BLE_CFG_CLT_MAX_NBR_CB)
  {
    /* event data in Attribute_Data_List contains:
    * 2 bytes for start handle
    * 1 byte char properties
    * 2 bytes handle
    * 2 or 16 bytes data for UUID
    */

    /* Number of attribute value tuples */
    numHdlValuePair = p_evt->Data_Length / p_evt->Handle_Value_Pair_Length;

    uuid_offset = 5;           /* UUID offset in bytes in Attribute_Data_List */
    if (p_evt->Handle_Value_Pair_Length == 21) /* we are interested in  128 bit UUIDs */
    {
      idx = 17;                /* UUID index of 2 bytes read part in Attribute_Data_List */
      uuid_size = 16;          /* UUID size in bytes */
      uuid_short_offset = 12;  /* UUID offset of 2 bytes read part in UUID field */
    }
    if (p_evt->Handle_Value_Pair_Length == 7) /* we are interested in  16 bit UUIDs */
    {
      idx = 5;
      uuid_size = 2;
      uuid_short_offset = 0;
    }
    UNUSED(idx);
    UNUSED(uuid_size);

    p_evt->Data_Length -= 1;

    LOG_INFO_APP("  number of value tuples = %d\n", numHdlValuePair);
    /* Loop on number of attribute value tuples */
    for (i = 0; i < numHdlValuePair; i++)
    {
      CharStartHdl = UNPACK_2_BYTE_PARAMETER(&p_evt->Handle_Value_Pair_Data[uuid_offset - 5]);
      CharProperties = p_evt->Handle_Value_Pair_Data[uuid_offset - 3];
      CharValueHdl = UNPACK_2_BYTE_PARAMETER(&p_evt->Handle_Value_Pair_Data[uuid_offset - 2]);
      uuid = UNPACK_2_BYTE_PARAMETER(&p_evt->Handle_Value_Pair_Data[uuid_offset + uuid_short_offset]);

      if ( (uuid != 0x0) && (CharProperties != 0x0) && (CharStartHdl != 0x0) && (CharValueHdl != 0) )
      {
        LOG_INFO_APP("    %d/%d short UUID=0x%04X, Properties=0x%04X, CharHandle [0x%04X - 0x%04X]",
                     i + 1, numHdlValuePair, uuid, CharProperties, CharStartHdl, CharValueHdl);

        if (uuid == DEVICE_NAME_UUID)
        {
          LOG_INFO_APP(", GAP DEVICE_NAME charac found\n");
        }
        else if (uuid == APPEARANCE_UUID)
        {
          LOG_INFO_APP(", GAP APPEARANCE charac found\n");
        }
        else if (uuid == SERVICE_CHANGED_CHARACTERISTIC_UUID)
        {
          a_ClientContext[index].ServiceChangedCharStartHdl = CharStartHdl;
          a_ClientContext[index].ServiceChangedCharValueHdl = CharValueHdl;
          LOG_INFO_APP(", GATT SERVICE_CHANGED_CHARACTERISTIC_UUID charac found\n");
        }
/* USER CODE BEGIN gatt_parse_chars_1 */
        else if (uuid == ST_P2P_NOTIFY_CHAR_UUID)
        {
          a_ClientContext[index].P2PNotificationCharHdl = CharStartHdl;
          a_ClientContext[index].P2PNotificationValueHdl = CharValueHdl;
          LOG_INFO_APP(", ST_P2P_NOTIFY_CHAR_UUID charac found\n");
        }
        else if (uuid == ST_P2P_WRITE_CHAR_UUID)
        {
          a_ClientContext[index].P2PWriteToServerCharHdl = CharStartHdl;
          a_ClientContext[index].P2PWriteToServerValueHdl = CharValueHdl;
          LOG_INFO_APP(", ST_P2P_WRITE_CHAR_UUID charac found\n");
        }
        else if (uuid == ST_P2P_LONGWRITE_WO_RES_CHAR_UUID)
        {
          a_ClientContext[0].P2PLongWritewoRespToServerCharHdl = CharStartHdl;
          a_ClientContext[0].P2PLongWritewoRespToServerValueHdl = CharValueHdl;
          LOG_INFO_APP(", ST_P2P_LONGWRITE_WO_RES_CHAR_UUID charac found\n");
        }

/* USER CODE END gatt_parse_chars_1 */
        else
        {
          LOG_INFO_APP("\n");
        }

      }
      uuid_offset += p_evt->Handle_Value_Pair_Length;
    }
  }
  else
  {
    LOG_INFO_APP("  ACI_ATT_READ_BY_TYPE_RESP_VSEVT_CODE, failed handle not found in connection table !\n");
  }

  return;
}
/**
* function of GATT descriptor parse
*/
static void gatt_parse_descs(aci_att_find_info_resp_event_rp0 *p_evt)
{
  uint16_t uuid, handle;
  uint8_t uuid_offset, uuid_size, uuid_short_offset, handle_uuid_pair_size;
  uint8_t i, numDesc, index;

  LOG_INFO_APP("  ACI_ATT_FIND_INFO_RESP_VSEVT_CODE - ConnHdl=0x%04X\n",
              p_evt->Connection_Handle);

  for (index = 0 ; index < BLE_CFG_CLT_MAX_NBR_CB ; index++)
  {
    if (a_ClientContext[index].connHdl == p_evt->Connection_Handle)
    {
      break;
    }
  }

  /* index < BLE_CFG_CLT_MAX_NBR_CB means connection handle identified */
  if (index < BLE_CFG_CLT_MAX_NBR_CB)
  {
    /* event data in Attribute_Data_List contains:
    * 2 bytes handle
    * 2 or 16 bytes data for UUID
    */
    uuid_offset = 2;
    if (p_evt->Format == UUID_TYPE_16)
    {
      uuid_size = 2;
      uuid_short_offset = 0;
      handle_uuid_pair_size = 4;
    }
    else if (p_evt->Format == UUID_TYPE_128)
    {
      uuid_size = 16;
      uuid_short_offset = 12;
      handle_uuid_pair_size = 18;
    }
    else
    {
      return;
    }
    UNUSED(uuid_size);

    /* Number of handle uuid pairs */
    numDesc = (p_evt->Event_Data_Length) / handle_uuid_pair_size;

    for (i = 0; i < numDesc; i++)
    {
      handle = UNPACK_2_BYTE_PARAMETER(&p_evt->Handle_UUID_Pair[uuid_offset - 2]);
      uuid = UNPACK_2_BYTE_PARAMETER(&p_evt->Handle_UUID_Pair[uuid_offset + uuid_short_offset]);

      if (uuid == PRIMARY_SERVICE_UUID)
      {
        LOG_INFO_APP("    Primary service UUID=0x%04X, handle=0x%04X\n", uuid, handle);
      }
      else if (uuid == CHARACTERISTIC_UUID)
      {
        gattCharStartHdl = 0;
        gattCharValueHdl = 0;

        gattCharStartHdl = handle;
        LOG_INFO_APP("    Charac UUID=0x%04X, handle=0x%04X\n", uuid, handle);
      }
      else if ( (uuid == CHAR_EXTENDED_PROPERTIES_DESCRIPTOR_UUID)
             || (uuid == CLIENT_CHAR_CONFIG_DESCRIPTOR_UUID) )
      {

        LOG_INFO_APP("    Descriptor UUID=0x%04X, handle=0x%04X",
                      uuid, handle);
        if (a_ClientContext[index].ServiceChangedCharValueHdl == gattCharValueHdl)
        {
          a_ClientContext[index].ServiceChangedCharDescHdl = handle;
          LOG_INFO_APP(", Service Changed found\n");
        }
/* USER CODE BEGIN gatt_parse_descs_1 */
        else if (a_ClientContext[index].P2PWriteToServerValueHdl == gattCharValueHdl)
        {
          a_ClientContext[index].P2PWriteToServerDescHdl = handle;
          LOG_INFO_APP("P2PWrite found: handle=0x%04X\n",handle);
        }
        else if (a_ClientContext[index].P2PNotificationValueHdl == gattCharValueHdl)
        {
          a_ClientContext[index].P2PNotificationDescHdl = handle;
          LOG_INFO_APP("P2PNotification found: handle=0x%04X\n",handle);
        }
        else if (a_ClientContext[index].P2PLongWritewoRespToServerValueHdl == gattCharValueHdl)
        {
          a_ClientContext[index].P2PLongWritewoRespToServerDescHdl = handle;
          LOG_INFO_APP("P2PLongWrite without response found: handle=0x%04X\n",handle);
        }

/* USER CODE END gatt_parse_descs_1 */
        else
        {
          LOG_INFO_APP("\n");
        }
        UNUSED(gattCharStartHdl);
      }
      else
      {
        gattCharValueHdl = handle;

        LOG_INFO_APP("    UUID=0x%04X, handle=0x%04X", uuid, handle);

        if (uuid == DEVICE_NAME_UUID)
        {
          LOG_INFO_APP(", found GAP DEVICE_NAME_UUID\n");
        }
        else if (uuid == APPEARANCE_UUID)
        {
          LOG_INFO_APP(", found GAP APPEARANCE_UUID\n");
        }
        else if (uuid == SERVICE_CHANGED_CHARACTERISTIC_UUID)
        {
          LOG_INFO_APP(", found GATT SERVICE_CHANGED_CHARACTERISTIC_UUID\n");
        }
/* USER CODE BEGIN gatt_parse_descs_2 */
        else if (uuid == ST_P2P_WRITE_CHAR_UUID)
        {
          LOG_INFO_APP(", found ST_P2P_WRITE_CHAR_UUID\n");
        }
        else if (uuid == ST_P2P_NOTIFY_CHAR_UUID)
        {
          LOG_INFO_APP(", found ST_P2P_NOTIFY_CHAR_UUID\n");
        }
        else if (uuid == ST_P2P_LONGWRITE_WO_RES_CHAR_UUID)
        {
          LOG_INFO_APP(", found ST_P2P_LONGWRITE_WO_RES_CHAR_UUID\n");
        }

/* USER CODE END gatt_parse_descs_2 */
        else
        {
          LOG_INFO_APP("\n");
        }
      }
    uuid_offset += handle_uuid_pair_size;
    }
  }
  else
  {
    LOG_INFO_APP("ACI_ATT_FIND_INFO_RESP_VSEVT_CODE, failed handle not found in connection table !\n");
  }

  return;
}

static void gatt_parse_notification(aci_gatt_notification_event_rp0 *p_evt)
{
  LOG_DEBUG_APP("ACI_GATT_NOTIFICATION_VSEVT_CODE - ConnHdl=0x%04X, Attribute_Handle=0x%04X\n",
                p_evt->Connection_Handle,
                p_evt->Attribute_Handle);
/* USER CODE BEGIN gatt_parse_notification_1 */
  GATT_CLIENT_APP_Notification_evt_t Notification;
  uint8_t index;

  for (index = 0 ; index < BLE_CFG_CLT_MAX_NBR_CB ; index++)
  {
    if (a_ClientContext[index].connHdl == p_evt->Connection_Handle)
    {
      break;
    }
  }

  /* index < BLE_CFG_CLT_MAX_NBR_CB means connection handle identified */
  if (index < BLE_CFG_CLT_MAX_NBR_CB)
  {
    if (p_evt->Attribute_Handle == a_ClientContext[index].P2PNotificationValueHdl)
    {
      LOG_INFO_APP("  Incoming Nofification received P2P Server information\n");
      Notification.Client_Evt_Opcode = P2P_NOTIFICATION_INFO_RECEIVED_EVT;
      Notification.DataTransfered.length = p_evt->Attribute_Value_Length;
      Notification.DataTransfered.p_Payload = &p_evt->Attribute_Value[0];

      gatt_Notification(&Notification);
    }
  }
  else
  {
    LOG_INFO_APP("ACI_GATT_NOTIFICATION_VSEVT_CODE, failed handle not found in connection table !\n");
  }

/* USER CODE END gatt_parse_notification_1 */

  return;
}

static void client_discover_all(void)
{
  uint8_t index = 0;
  /* USER CODE BEGIN client_discover_1 */

  /* USER CODE END client_discover_1 */

  GATT_CLIENT_APP_Discover_services(index);

  /* USER CODE BEGIN client_discover_2 */

  /* USER CODE END client_discover_2 */
  return;
}

static void gatt_cmd_resp_release(void)
{
  UTIL_SEQ_SetEvt(1U << CFG_IDLEEVT_PROC_GATT_COMPLETE);
  return;
}

static void gatt_cmd_resp_wait(void)
{
  UTIL_SEQ_WaitEvt(1U << CFG_IDLEEVT_PROC_GATT_COMPLETE);
  return;
}

/* USER CODE BEGIN LF */

static void P2Pclient_write_char(void)
{
  uint8_t index = 0;
  tBleStatus ret;

  if (P2PButtonData.level == 0x00)
  {
    P2PButtonData.level = 0x01;
  }
  else
  {
    P2PButtonData.level = 0x00;
  }

  for(index = 0; index < BLE_CFG_CLT_MAX_NBR_CB; index++)
  {
    if (a_ClientContext[index].state != GATT_CLIENT_APP_IDLE)
    {
      ret = aci_gatt_write_without_resp(a_ClientContext[index].connHdl,
                                        a_ClientContext[index].P2PWriteToServerValueHdl,
                                        2,
                                        (uint8_t *)&P2PButtonData);

      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("aci_gatt_write_without_resp failed, connHdl=0x%04X, ValueHdl=0x%04X\n",
                a_ClientContext[index].connHdl,
                a_ClientContext[index].P2PWriteToServerValueHdl);
      }
      else
      {
        LOG_INFO_APP("aci_gatt_write_without_resp success, connHdl=0x%04X, ValueHdl=0x%04X\n",
          a_ClientContext[index].connHdl,
          a_ClientContext[index].P2PWriteToServerValueHdl);
      }
    }
  }

  return;
}

static void P2Pclient_longwrite_woresp_char(void)
{
  uint8_t index = 0;
  tBleStatus ret;

  for(index = 0; index < BLE_CFG_CLT_MAX_NBR_CB; index++)
  {
    if (a_ClientContext[index].state != GATT_CLIENT_APP_IDLE)
    {
      ret = aci_gatt_write_without_resp_ext(a_ClientContext[index].connHdl,
                                        a_ClientContext[index].P2PLongWritewoRespToServerValueHdl,
                                        0x00, /* write without response opCode*/
                                        509, /* data length */
                                        0);  /* offset in bytes from the start of extra_data_buffer */

      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("aci_gatt_write_without_resp_ext failed, connHdl=0x%04X, ValueHdl=0x%04X, ret=%x\n",
                a_ClientContext[index].connHdl,
                a_ClientContext[index].P2PLongWritewoRespToServerValueHdl, ret);
      }
      else
      {
        LOG_INFO_APP("aci_gatt_write_without_resp_ext success, connHdl=0x%04X, ValueHdl=0x%04X\n",
          a_ClientContext[index].connHdl,
          a_ClientContext[index].P2PLongWritewoRespToServerValueHdl);
      }
    }
  }

  return;
}

static void Gatt_Config_Exchange(void)
{
  tBleStatus bleStatus = BLE_STATUS_SUCCESS;
  bleStatus = aci_gatt_exchange_config(a_ClientContext[0].connHdl);
  if (bleStatus == BLE_STATUS_SUCCESS)
  {
    gatt_cmd_resp_wait();
    LOG_INFO_APP(" aci_gatt_exchange_config\n");
  }
  else
  {
    LOG_ERROR_APP(" aci_gatt_exchange_config, status =0x%02X\n", bleStatus);
  }
}

/* USER CODE END LF */
