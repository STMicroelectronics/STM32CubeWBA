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
#include "ble.h"
#include "gatt_client_app.h"
#include "stm32_seq.h"
#include "app_ble.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32_timer.h"
#include "stm32wbaxx_nucleo.h"
#include "host_stack_if.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */
typedef struct
{
  UTIL_TIMER_Object_t TimerDataThroughput_Id;
  uint16_t            ConnectionHandle;
} DT_CLI_APP_Context_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;
  uint8_t pPayload_n_1;
  uint8_t pPayload_n;
  uint8_t Length32;
} DT_Data_FromServer_t;

/* USER CODE END PTD */

typedef enum
{
  NOTIFICATION_INFO_RECEIVED_EVT,
  /* USER CODE BEGIN GATT_CLIENT_APP_Opcode_t */

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

typedef struct
{
  GATT_CLIENT_APP_State_t state;

  APP_BLE_ConnStatus_t connStatus;
  uint16_t connHdl;

  uint16_t ALLServiceStartHdl;
  uint16_t ALLServiceEndHdl;

  uint16_t GAPServiceStartHdl;
  uint16_t GAPServiceEndHdl;

  uint16_t GATTServiceStartHdl;
  uint16_t GATTServiceEndHdl;

  uint16_t ServiceChangedCharStartHdl;
  uint16_t ServiceChangedCharValueHdl;
  uint16_t ServiceChangedCharDescHdl;
  uint16_t ServiceChangedCharEndHdl;
  uint8_t ServiceChangedCharProperties;
  /* USER CODE BEGIN BleClientAppContext_t */
  /* handles of DT service */
  uint16_t DTServiceHdl;
  uint16_t DTServiceEndHdl;

  /* handles of Tx characteristic - Notification from server */
  uint16_t DTTXCharHdle;
  uint16_t DTTXValueHdle;
  uint16_t DTTXDescHdl;

  /* handles of Rx characteristic - write to server */
  uint16_t DTRXCharHdle;
  uint16_t DTRXValueHdle;
  uint16_t DTRXDescHdl;

  /* handles of DT DataThroughput characteristic - Notification from server, used to send result of write data received by server */
  uint16_t DTThroughputCharHdle;
  uint16_t DTThroughputValueHdle;
  uint16_t DTThroughputDescHdl;

  DTC_STM_Payload_t TxData;
  DTC_App_Transfer_Req_Status_t NotificationTransferReq;
  DTC_App_Transfer_Req_Status_t ButtonTransferReq;
  DTC_App_Flow_Status_t DtFlowStatus;
  uint8_t connectionstatus;
  uint16_t MTUSizeValue;

/* USER CODE END BleClientAppContext_t */

}BleClientAppContext_t;

/* Private defines ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DT_TX_SERV_UUID                                               (0xFE80)
#define DT_TX_CHAR_UUID                                               (0xFE81)
#define DT_RX_CHAR_UUID                                               (0xFE82)
#define DT_THROUGHPUT_CHAR_UUID                                       (0xFE83)

#define TIMEUNIT  1

#define DATA_NOTIFICATION_MAX_PACKET_SIZE     244

/* USER CODE END PD */

/* Private macros -------------------------------------------------------------*/
#define UNPACK_2_BYTE_PARAMETER(ptr)  \
        (uint16_t)((uint16_t)(*((uint8_t *)ptr))) |   \
        (uint16_t)((((uint16_t)(*((uint8_t *)ptr + 1))) << 8))
/* USER CODE BEGIN PM */
#define COPY_UUID_16(uuid_struct, uuid_1, uuid_0)\
do {\
    uuid_struct[0] = uuid_0; uuid_struct[1] = uuid_1;\
}while(0)

#define COPY_DT_SERV_UUID(uuid_struct)       COPY_UUID_16(uuid_struct,0xfe,0x80)
#define COPY_TX_CHAR_UUID(uuid_struct)       COPY_UUID_16(uuid_struct,0xfe,0x81)
#define COPY_RX_CHAR_UUID(uuid_struct)       COPY_UUID_16(uuid_struct,0xfe,0x82)
#define COPY_THROUGH_CHAR_UUID(uuid_struct)  COPY_UUID_16(uuid_struct,0xfe,0x83)

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

static BleClientAppContext_t a_ClientContext[BLE_CFG_CLT_MAX_NBR_CB];
static uint16_t gattCharStartHdl = 0;
static uint16_t gattCharValueHdl = 0;

/* USER CODE BEGIN PV */
static uint8_t Notification_Data_Buffer[DATA_NOTIFICATION_MAX_PACKET_SIZE]; /* DATA_NOTIFICATION_MAX_PACKET_SIZE data + CRC */
static DT_CLI_APP_Context_t DT_CLI_APP_Context;
static uint16_t packet_lost;
static uint32_t DataThroughputValue;
static uint32_t packet_lost_local;
static DT_CLI_NotificationEvt_t NotificationData;
DTC_Context_t DTC_Context;
uint32_t dtc_n=0;
uint32_t DataTransfered = 0;

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
static void SendDataWrite(void);
static void DataThroughputNotif_proc(void *arg);
static void DataT_Notification_Data( void );
static void BLE_SVC_GAP_Security_Req(void);
static void Resume_Write(void);

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void GATT_CLIENT_APP_Init(void)
{
  uint8_t index =0;
  /* USER CODE BEGIN GATT_CLIENT_APP_Init_1 */
  uint8_t i;

  /* USER CODE END GATT_CLIENT_APP_Init_1 */
  for(index = 0; index < BLE_CFG_CLT_MAX_NBR_CB; index++)
  {
    a_ClientContext[index].connStatus = APP_BLE_IDLE;
  }

  /* Register the event handler to the BLE controller */
  SVCCTL_RegisterCltHandler(Event_Handler);

  /* Register a task allowing to discover all services and characteristics and enable all notifications */
  UTIL_SEQ_RegTask(1U << CFG_TASK_DISCOVER_SERVICES_ID, UTIL_SEQ_RFU, client_discover_all);

  /* USER CODE BEGIN GATT_CLIENT_APP_Init_2 */
  UTIL_SEQ_RegTask(1U << CFG_TASK_WRITE_DATA_WO_RESP_ID, UTIL_SEQ_RFU, SendDataWrite);
  UTIL_SEQ_RegTask(1U << CFG_TASK_SECURITY_REQ_ID, UTIL_SEQ_RFU, BLE_SVC_GAP_Security_Req);

  UTIL_TIMER_Create(&(DT_CLI_APP_Context.TimerDataThroughput_Id),
        0,
        UTIL_TIMER_ONESHOT,
        &DataThroughputNotif_proc,
        0);

  UTIL_TIMER_SetPeriod(&(DT_CLI_APP_Context.TimerDataThroughput_Id), 1000);

  UTIL_SEQ_RegTask(1U << CFG_TASK_DATA_FROM_SERVER_ID, UTIL_SEQ_RFU, DataT_Notification_Data);

  /**
   * Initialize data buffer
   */
  for (i = 0 ; i < (DATA_NOTIFICATION_MAX_PACKET_SIZE - 1) ; i++)
  {
    Notification_Data_Buffer[i] = i;
  }

  a_ClientContext[0].state = GATT_CLIENT_APP_IDLE;
  a_ClientContext[0].connHdl = 0xFFFF;
  DTC_Context.connectionstatus = APP_BLE_IDLE;
  DTC_Context.NotificationTransferReq = DTC_APP_TRANSFER_REQ_OFF;
  DTC_Context.ButtonTransferReq = DTC_APP_TRANSFER_REQ_OFF;
  DTC_Context.DtFlowStatus = DTC_APP_FLOW_ON;

  LOG_INFO_APP("-- DT CLIENT INITIALIZED \n");

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
      DTC_Context.connectionstatus = APP_BLE_CONNECTED_CLIENT;

      /* USER CODE END PEER_CONN_HANDLE_EVT */
      break;

    case PEER_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN PEER_DISCON_HANDLE_EVT */
  DTC_Context.connectionstatus = APP_BLE_IDLE;

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
  tBleStatus result = BLE_STATUS_SUCCESS;
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
        result = aci_gatt_disc_all_primary_services(a_ClientContext[index].connHdl);

        if (result == BLE_STATUS_SUCCESS)
        {
          gatt_cmd_resp_wait();
          LOG_INFO_APP("PROC_GATT_DISC_ALL_PRIMARY_SERVICES services discovered Successfully\n\n");
        }
        else
        {
          LOG_INFO_APP("PROC_GATT_DISC_ALL_PRIMARY_SERVICES aci_gatt_disc_all_primary_services cmd NOK status =0x%02X\n\n", result);
        }
      }
      break; /* PROC_GATT_DISC_ALL_PRIMARY_SERVICES */

      case PROC_GATT_DISC_ALL_CHARS:
      {
        a_ClientContext[index].state = GATT_CLIENT_APP_DISCOVER_CHARACS;

        LOG_INFO_APP("DISCOVER_ALL_CHARS ConnHdl=0x%04X ALLServiceHandle[0x%04X - 0x%04X]\n",
                          a_ClientContext[index].connHdl,
                          a_ClientContext[index].ALLServiceStartHdl,
                          a_ClientContext[index].ALLServiceEndHdl);

        result = aci_gatt_disc_all_char_of_service(
                           a_ClientContext[index].connHdl,
                           a_ClientContext[index].ALLServiceStartHdl,
                           a_ClientContext[index].ALLServiceEndHdl);

        if (result == BLE_STATUS_SUCCESS)
        {
          gatt_cmd_resp_wait();
          LOG_INFO_APP("All characteristics discovered Successfully\n\n");
        }
        else
        {
          LOG_INFO_APP("All characteristics discovery Failed, status =0x%02X\n\n", result);
        }
      }
      break; /* PROC_GATT_DISC_ALL_CHARS */

      case PROC_GATT_DISC_ALL_DESCS:
      {
        a_ClientContext[index].state = GATT_CLIENT_APP_DISCOVER_WRITE_DESC;

        LOG_INFO_APP("DISCOVER_ALL_CHAR_DESCS [0x%04X - 0x%04X]\n",
                         a_ClientContext[index].ALLServiceStartHdl,
                         a_ClientContext[index].ALLServiceEndHdl);
        result = aci_gatt_disc_all_char_desc(
                         a_ClientContext[index].connHdl,
                         a_ClientContext[index].ALLServiceStartHdl,
                         a_ClientContext[index].ALLServiceEndHdl);

        if (result == BLE_STATUS_SUCCESS)
        {
          gatt_cmd_resp_wait();
          LOG_INFO_APP("All characteristic descriptors discovered Successfully\n\n");
        }
        else
        {
          LOG_INFO_APP("All characteristic descriptors discovery Failed, status =0x%02X\n\n", result);
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
          if(((a_ClientContext[index].ServiceChangedCharProperties) & CHAR_PROP_INDICATE) == CHAR_PROP_INDICATE)
          {
            charPropVal = 0x0002;
          }
          result = aci_gatt_write_char_desc(a_ClientContext[index].connHdl,
                                            a_ClientContext[index].ServiceChangedCharDescHdl,
                                            2,
                                            (uint8_t *) &charPropVal);
          if (result == BLE_STATUS_SUCCESS)
          {
            gatt_cmd_resp_wait();
            LOG_INFO_APP(" ServiceChangedCharDescHdl =0x%04X\n",a_ClientContext[index].ServiceChangedCharDescHdl);
          }
          else
          {
            LOG_INFO_APP(" ServiceChangedCharDescHdl write Failed, status =0x%02X\n\n", result);
          }
        }
        /* USER CODE BEGIN PROC_GATT_PROPERTIES_ENABLE_ALL */
        /* Enable TX notification */
        uint16_t enable = 0x0001;
        tBleStatus aci_status = BLE_STATUS_ERROR;

        if (a_ClientContext[index].DTTXDescHdl != 0x0000)
        {
          aci_status = aci_gatt_write_char_desc(a_ClientContext[index].connHdl,
                                                a_ClientContext[index].DTTXDescHdl,
                                                2,
                                                (uint8_t *) &enable);
          if(aci_status == BLE_STATUS_SUCCESS)
          {
            gatt_cmd_resp_wait();
          }
          LOG_INFO_APP(" DTTXDescHdl =0x%04X, status =0x%02X\n",a_ClientContext[index].DTTXDescHdl, aci_status);
        }
        /* Enable RX notification */
        if (a_ClientContext[index].DTRXDescHdl != 0x0000)
        {
          aci_status = aci_gatt_write_char_desc(a_ClientContext[index].connHdl,
                                                a_ClientContext[index].DTRXDescHdl,
                                                2,
                                                (uint8_t *) &enable);
          if(aci_status == BLE_STATUS_SUCCESS)
          {
            gatt_cmd_resp_wait();
          }
          LOG_INFO_APP(" DTRXDescHdl =0x%04X, status =0x%02X\n",a_ClientContext[index].DTRXDescHdl, aci_status);
        }
        /* Enable Throughput notification */
        if (a_ClientContext[index].DTThroughputDescHdl != 0x0000)
        {
          aci_status = aci_gatt_write_char_desc(a_ClientContext[index].connHdl,
                                                a_ClientContext[index].DTThroughputDescHdl,
                                                2,
                                                (uint8_t *) &enable);
          if(aci_status == BLE_STATUS_SUCCESS)
          {
            gatt_cmd_resp_wait();
          }
          LOG_INFO_APP(" DTThroughputDescHdl =0x%04X, status =0x%02X\n",a_ClientContext[index].DTThroughputDescHdl, aci_status);
        }
        UTIL_SEQ_SetTask(1U << CFG_TASK_CONN_UPDATE_ID, CFG_SEQ_PRIO_0);

        result |= aci_status;
        /* USER CODE END PROC_GATT_PROPERTIES_ENABLE_ALL */

        if (result == BLE_STATUS_SUCCESS)
        {
          LOG_INFO_APP("All notifications enabled Successfully\n\n");
        }
        else
        {
          LOG_INFO_APP("All notifications enabled Failed, status =0x%02X\n\n", result);
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

          uint8_t index;
          for (index = 0 ; index < BLE_CFG_CLT_MAX_NBR_CB ; index++)
          {
            if (a_ClientContext[index].connHdl == p_evt_rsp->Connection_Handle)
            {
              gatt_cmd_resp_release();
              break;
            }
          }
        }
        break;/* ACI_GATT_PROC_COMPLETE_VSEVT_CODE */
        case ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE:
        {
          aci_att_exchange_mtu_resp_event_rp0 *tx_pool_available;
          tx_pool_available = (aci_att_exchange_mtu_resp_event_rp0 *)p_blecore_evt->data;
          UNUSED(tx_pool_available);
          /* USER CODE BEGIN ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */
          Resume_Write();

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
          if (exchange_mtu_resp->Server_RX_MTU < DATA_NOTIFICATION_MAX_PACKET_SIZE)
          {
            a_ClientContext[0].MTUSizeValue = exchange_mtu_resp->Server_RX_MTU - 3;
          }
          else
          {
            a_ClientContext[0].MTUSizeValue = DATA_NOTIFICATION_MAX_PACKET_SIZE;
          }
          LOG_INFO_APP("  MTU_size = %d\n", a_ClientContext[0].MTUSizeValue);
          tBleStatus status;
          status = hci_le_set_data_length(a_ClientContext[0].connHdl,251,2120);
          if (status != BLE_STATUS_SUCCESS)
          {
            LOG_INFO_APP("  Fail   : set data length command   : error code: 0x%x \n\r", status);
          }
          else
          {
            LOG_INFO_APP("  Success: set data length command  \n\r");
          }
		
          /* USER CODE END ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
        }
        break;

        default:
          break;
      }/* end switch (p_blecore_evt->ecode) */
    }
    break; /* HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE */
    default:
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
  uint8_t i, idx, numServ, index;

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
      ServiceStartHdl =  UNPACK_2_BYTE_PARAMETER(&p_evt->Attribute_Data_List[uuid_offset - 4]);
      ServiceEndHdl = UNPACK_2_BYTE_PARAMETER(&p_evt->Attribute_Data_List[uuid_offset - 2]);
      uuid = UNPACK_2_BYTE_PARAMETER(&p_evt->Attribute_Data_List[uuid_offset + uuid_short_offset]);
      LOG_INFO_APP("  %d/%d short UUID=0x%04X, handle [0x%04X - 0x%04X]",
                   i + 1, numServ, uuid, ServiceStartHdl,ServiceEndHdl);

      /* complete context fields */
      if ( (a_ClientContext[index].ALLServiceStartHdl == 0x0000) || (ServiceStartHdl < a_ClientContext[index].ALLServiceStartHdl) )
      {
        a_ClientContext[index].ALLServiceStartHdl = ServiceStartHdl;
      }
      if ( (a_ClientContext[index].ALLServiceEndHdl == 0x0000) || (ServiceEndHdl > a_ClientContext[index].ALLServiceEndHdl) )
      {
        a_ClientContext[index].ALLServiceEndHdl = ServiceEndHdl;
      }

      if (uuid == GAP_SERVICE_UUID)
      {
        a_ClientContext[index].GAPServiceStartHdl = ServiceStartHdl;
        a_ClientContext[index].GAPServiceEndHdl = ServiceEndHdl;

        LOG_INFO_APP(", GAP_SERVICE_UUID found\n");
      }
      else if (uuid == GENERIC_ATTRIBUTE_SERVICE_UUID)
      {
        a_ClientContext[index].GATTServiceStartHdl = ServiceStartHdl;
        a_ClientContext[index].GATTServiceEndHdl = ServiceEndHdl;

        LOG_INFO_APP(", GENERIC_ATTRIBUTE_SERVICE_UUID found\n");
      }
/* USER CODE BEGIN gatt_parse_services_1 */
      else if (uuid == DT_TX_SERV_UUID)
      {
        a_ClientContext[index].DTServiceHdl = ServiceStartHdl;

        LOG_INFO_APP(", GENERIC_ATTRIBUTE_SERVICE_UUID found\n");
      }
/* USER CODE END gatt_parse_services_1 */
      else
      {
        LOG_INFO_APP("\n");
      }

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

  LOG_INFO_APP("ACI_ATT_READ_BY_TYPE_RESP_VSEVT_CODE - ConnHdl=0x%04X\n",
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

    LOG_INFO_APP("  ConnHdl=0x%04X, number of value pair = %d\n", a_ClientContext[index].connHdl, numHdlValuePair);
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
        else if (uuid == DT_TX_CHAR_UUID)
        {
          a_ClientContext[index].DTTXCharHdle = CharStartHdl;
          a_ClientContext[index].DTTXValueHdle = CharValueHdl;
          LOG_INFO_APP(", GATT TX_CHARACTERISTIC_UUID charac found\n");
        }
        else if (uuid == DT_RX_CHAR_UUID)
        {
          a_ClientContext[index].DTRXCharHdle = CharStartHdl;
          a_ClientContext[index].DTRXValueHdle = CharValueHdl;
          LOG_INFO_APP(", GATT RX_CHARACTERISTIC_UUID charac found\n");
        }
        else if (uuid == DT_THROUGHPUT_CHAR_UUID)
        {
          a_ClientContext[index].DTThroughputCharHdle = CharStartHdl;
          a_ClientContext[index].DTThroughputValueHdle = CharValueHdl;
          LOG_INFO_APP(", GATT THROUGHPUT_CHARACTERISTIC_UUID charac found\n");
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
    LOG_INFO_APP("ACI_ATT_READ_BY_TYPE_RESP_VSEVT_CODE, failed handle not found in connection table !\n");
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

  LOG_INFO_APP("ACI_ATT_FIND_INFO_RESP_VSEVT_CODE - ConnHdl=0x%04X\n",
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
        LOG_INFO_APP("PRIMARY_SERVICE_UUID=0x%04X handle=0x%04X\n", uuid, handle);
      }
      else if (uuid == CHARACTERISTIC_UUID)
      {
        /* reset UUID & handle */
        gattCharStartHdl = 0;
        gattCharValueHdl = 0;

        gattCharStartHdl = handle;
        LOG_INFO_APP("reset - UUID & handle - CHARACTERISTIC_UUID=0x%04X CharStartHandle=0x%04X\n", uuid, handle);
      }
      else if ( (uuid == CHAR_EXTENDED_PROPERTIES_DESCRIPTOR_UUID)
             || (uuid == CLIENT_CHAR_CONFIG_DESCRIPTOR_UUID) )
      {

        LOG_INFO_APP("Descriptor UUID=0x%04X, handle=0x%04X-0x%04X-0x%04X",
                      uuid,
                      gattCharStartHdl,
                      gattCharValueHdl,
                      handle);
        if (a_ClientContext[index].ServiceChangedCharValueHdl == gattCharValueHdl)
        {
          a_ClientContext[index].ServiceChangedCharDescHdl = handle;
          LOG_INFO_APP(", Service Changed found\n");
        }
/* USER CODE BEGIN gatt_parse_descs_1 */
        if (a_ClientContext[index].DTTXValueHdle == gattCharValueHdl)
        {
          a_ClientContext[index].DTTXDescHdl = handle;
          LOG_INFO_APP(", DTTX Client descriptor found\n");
        }
        if (a_ClientContext[index].DTRXValueHdle == gattCharValueHdl)
        {
          a_ClientContext[index].DTRXDescHdl = handle;
          LOG_INFO_APP(", DTRX Client descriptor found\n");
        }
        if (a_ClientContext[index].DTThroughputValueHdle == gattCharValueHdl)
        {
          a_ClientContext[index].DTThroughputDescHdl = handle;
          LOG_INFO_APP(", DTThroughput Client descriptor found\n");
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

        LOG_INFO_APP("  UUID=0x%04X, handle=0x%04X", uuid, handle);

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
  uint8_t CRC_Result;
  uint8_t CRC_Received;

  if ((p_evt->Attribute_Handle == a_ClientContext[0].DTTXValueHdle) &&
      (p_evt->Attribute_Value_Length > (2)))
  {
    NotificationData.DataTransfered.Length = p_evt->Attribute_Value_Length;
    NotificationData.DataTransfered.pPayload = (p_evt->Attribute_Value);
    NotificationData.DataTransfered.pPayload_n = *((uint32_t*) &(p_evt->Attribute_Value[0]));

    __disable_irq();
    if (NotificationData.DataTransfered.pPayload_n >= (NotificationData.DataTransfered.pPayload_n_1 + 2))
    {
      packet_lost +=
        ((NotificationData.DataTransfered.pPayload_n - NotificationData.DataTransfered.pPayload_n_1) - 1);
    }
    __enable_irq();
    /* CRC computation */
    CRC_Result = APP_BLE_ComputeCRC8((uint8_t*) (p_evt->Attribute_Value), (p_evt->Attribute_Value_Length) - 1);
    /* get low weight byte */
    CRC_Received = (uint8_t) (p_evt->Attribute_Value[a_ClientContext[0].MTUSizeValue - 1]);

    if (CRC_Received != CRC_Result)
    {
      dtc_n += 1;
      LOG_INFO_APP("** data error **  N= %d \r\n",(int32_t)dtc_n);
    }
    NotificationData.DataTransfered.pPayload_n_1 = NotificationData.DataTransfered.pPayload_n;
    if (DataTransfered == 0)
    {
      UTIL_TIMER_Start(&(DT_CLI_APP_Context.TimerDataThroughput_Id));
    }
    DataTransfered += NotificationData.DataTransfered.Length;
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
void DTC_Button1TriggerReceived( void )
{
  if (DTC_Context.connectionstatus != APP_BLE_CONNECTED_CLIENT)
  {
    UTIL_SEQ_SetTask(1U << CFG_TASK_START_SCAN_ID, CFG_SEQ_PRIO_0);
  }
  else
  {
    if(DTC_Context.ButtonTransferReq != DTC_APP_TRANSFER_REQ_OFF)
    {
      #if (CFG_LED_SUPPORTED == 1)
      BSP_LED_Off(LED_BLUE);
      #endif
      DTC_Context.ButtonTransferReq = DTC_APP_TRANSFER_REQ_OFF;
    }
    else
    {
      #if (CFG_LED_SUPPORTED == 1)
      BSP_LED_On(LED_BLUE);
      #endif
      DTC_Context.ButtonTransferReq = DTC_APP_TRANSFER_REQ_ON;
      UTIL_SEQ_SetTask(1U << CFG_TASK_WRITE_DATA_WO_RESP_ID, CFG_SEQ_PRIO_0);
    }
  }
  BleStackCB_Process();
  return;
}

void DTC_Button2TriggerReceived( void )
{
  tBleStatus status;
  if (DTC_Context.connectionstatus != APP_BLE_CONNECTED_CLIENT)
  {
    status = aci_gap_clear_security_db();
    if (status != BLE_STATUS_SUCCESS)
    {
      LOG_INFO_APP("  Fail   : Clear security DB 0x%x\n", status);
    }
    else
    {
      LOG_INFO_APP("  Success: Clear security DB \n");
    }
  }
  else
  {
    UTIL_SEQ_SetTask(1 << CFG_TASK_SECURITY_REQ_ID, CFG_SEQ_PRIO_0);
  }
  BleStackCB_Process();
  return;
}

void DTC_Button3TriggerReceived( void )
{

  return;
}

static void SendDataWrite( void )
{
  tBleStatus status = BLE_STATUS_INVALID_PARAMS;
  uint8_t crc_result;

  if( (DTC_Context.ButtonTransferReq != DTC_APP_TRANSFER_REQ_OFF) &&
      (DTC_Context.DtFlowStatus != DTC_APP_FLOW_OFF) )
  {
    /*Data Packet to send to remote*/
    Notification_Data_Buffer[0] += 1;
    /* compute CRC */
    crc_result = APP_BLE_ComputeCRC8((uint8_t*) Notification_Data_Buffer, (a_ClientContext[0].MTUSizeValue - 1));
    Notification_Data_Buffer[a_ClientContext[0].MTUSizeValue - 1] = crc_result;

    DTC_Context.TxData.pPayload = Notification_Data_Buffer;
    DTC_Context.TxData.Length =  a_ClientContext[0].MTUSizeValue;

    status = aci_gatt_write_without_resp(a_ClientContext[0].connHdl,
                                         a_ClientContext[0].DTRXValueHdle,
                                         a_ClientContext[0].MTUSizeValue,
                                         (const uint8_t*)(DTC_Context.TxData.pPayload));

    if (status == BLE_STATUS_INSUFFICIENT_RESOURCES)
    {
      DTC_Context.DtFlowStatus = DTC_APP_FLOW_OFF;
      (Notification_Data_Buffer[0])-=1;
    }
    else
    {
      UTIL_SEQ_SetTask(1U << CFG_TASK_BLE_HOST, CFG_SEQ_PRIO_0);
      UTIL_SEQ_SetTask(1U << CFG_TASK_WRITE_DATA_WO_RESP_ID, CFG_SEQ_PRIO_0);
    }
  }
  BleStackCB_Process();

  return;
}

static void Resume_Write(void)
{
  DTC_Context.DtFlowStatus = DTC_APP_FLOW_ON;
  UTIL_SEQ_SetTask(1U << CFG_TASK_WRITE_DATA_WO_RESP_ID, CFG_SEQ_PRIO_0);
}

static void DataThroughputNotif_proc(void *arg)
{
  UTIL_SEQ_SetTask(1U << CFG_TASK_DATA_FROM_SERVER_ID, CFG_SEQ_PRIO_0);
}

static void DataT_Notification_Data( void )
{
  DataThroughputValue = (uint32_t)(DataTransfered / TIMEUNIT);
  packet_lost_local = (uint32_t)(packet_lost / TIMEUNIT);
  DataTransfered = 0;
  packet_lost = 0;

  LOG_INFO_APP("  DataThroughput = %ld bytes/s lost = %ld \n",DataThroughputValue, packet_lost_local);
  return;
}

static void BLE_SVC_GAP_Security_Req( void )
{
  tBleStatus status;

  status = aci_gap_send_pairing_req(a_ClientContext[0].connHdl, 0x01);
  if (status != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : Pairing request: 0x%x\n", status);
  }
  else
  {
    LOG_INFO_APP("  Success: Pairing request\n");
  }

  BleStackCB_Process();
  return;
}

/* USER CODE END LF */
