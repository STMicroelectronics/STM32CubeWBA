/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ZigbeeDirectComm.c
  * @author  MCD Application Team
  * @brief   ZigbeeDirectComm definition.
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

/* Includes ------------------------------------------------------------------*/
#include "log_module.h"
#include "common_blesvc.h"
#include "zigbeedirectcomm.h"

/* USER CODE BEGIN Includes */
#include "zigbee.zd.h"
#include "zdd_stack_wrapper.h"
#include "stm32_rtos.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef struct{
  uint16_t  ZigbeedirectcommSvcHdle;                  /**< Zigbeedirectcomm Service Handle */
  uint16_t  FormnetworkCharHdle;                  /**< FORMNETWORK Characteristic Handle */
  uint16_t  JoinnetworkCharHdle;                  /**< JOINNETWORK Characteristic Handle */
  uint16_t  PermitjoinCharHdle;                  /**< PERMITJOIN Characteristic Handle */
  uint16_t  LeavenetworkCharHdle;                  /**< LEAVENETWORK Characteristic Handle */
  uint16_t  CommstatusCharHdle;                  /**< COMMSTATUS Characteristic Handle */
  uint16_t  ManagejoinerCharHdle;                  /**< MANAGEJOINER Characteristic Handle */
  uint16_t  CommidentityCharHdle;                  /**< COMMIDENTITY Characteristic Handle */
  uint16_t  FindbindCharHdle;                  /**< FINDBIND Characteristic Handle */
/* USER CODE BEGIN Context */
  /* Place holder for Characteristic Descriptors Handle*/

/* USER CODE END Context */
}ZIGBEEDIRECTCOMM_Context_t;

/* Private defines -----------------------------------------------------------*/
#define UUID_128_SUPPORTED  1

#if (UUID_128_SUPPORTED == 1)
#define BM_UUID_LENGTH  UUID_TYPE_128
#else
#define BM_UUID_LENGTH  UUID_TYPE_16
#endif

#define BM_REQ_CHAR_SIZE    (3)

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
#define CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET         2
#define CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET              1
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static const uint16_t SizeFormnetwork = 140;
static const uint16_t SizeJoinnetwork = 140;
static const uint16_t SizePermitjoin = 12;
static const uint16_t SizeLeavenetwork = 12;
static const uint16_t SizeCommstatus = 80;
static const uint16_t SizeManagejoiner = 60;
static const uint16_t SizeCommidentity = 12;
static const uint16_t SizeFindbind = 12;

static ZIGBEEDIRECTCOMM_Context_t ZIGBEEDIRECTCOMM_Context;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static SVCCTL_EvtAckStatus_t ZIGBEEDIRECTCOMM_EventHandler(void *p_pckt);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
/* USER CODE BEGIN PFD */

/* USER CODE END PFD */

/* Private functions ----------------------------------------------------------*/

#define COPY_UUID_128(uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7, uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0) \
do {\
    uuid_struct[0] = uuid_0; uuid_struct[1] = uuid_1; uuid_struct[2] = uuid_2; uuid_struct[3] = uuid_3; \
    uuid_struct[4] = uuid_4; uuid_struct[5] = uuid_5; uuid_struct[6] = uuid_6; uuid_struct[7] = uuid_7; \
    uuid_struct[8] = uuid_8; uuid_struct[9] = uuid_9; uuid_struct[10] = uuid_10; uuid_struct[11] = uuid_11; \
    uuid_struct[12] = uuid_12; uuid_struct[13] = uuid_13; uuid_struct[14] = uuid_14; uuid_struct[15] = uuid_15; \
}while(0)

/*
 The following 128bits UUIDs have been generated from the random UUID
 generator:
 7072377D0001421CB163491C27333A61: Characteristic 128bits UUID
 */
/*
 The following 128bits UUIDs have been generated from the random UUID
 generator:
 7072377D0002421CB163491C27333A61: Characteristic 128bits UUID
 */
/*
 The following 128bits UUIDs have been generated from the random UUID
 generator:
 7072377D0003421CB163491C27333A61: Characteristic 128bits UUID
 */
/*
 The following 128bits UUIDs have been generated from the random UUID
 generator:
 7072377D0004421CB163491C27333A61: Characteristic 128bits UUID
 */
/*
 The following 128bits UUIDs have been generated from the random UUID
 generator:
 7072377D0005421CB163491C27333A61: Characteristic 128bits UUID
 */
/*
 The following 128bits UUIDs have been generated from the random UUID
 generator:
 7072377D0006421CB163491C27333A61: Characteristic 128bits UUID
 */
/*
 The following 128bits UUIDs have been generated from the random UUID
 generator:
 7072377D0007421CB163491C27333A61: Characteristic 128bits UUID
 */
/*
 The following 128bits UUIDs have been generated from the random UUID
 generator:
 7072377D0008421CB163491C27333A61: Characteristic 128bits UUID
 */
#define COPY_FORMNETWORK_UUID(uuid_struct)       COPY_UUID_128(uuid_struct,0x70,0x72,0x37,0x7d,0x00,0x01,0x42,0x1c,0xb1,0x63,0x49,0x1c,0x27,0x33,0x3a,0x61)
#define COPY_JOINNETWORK_UUID(uuid_struct)       COPY_UUID_128(uuid_struct,0x70,0x72,0x37,0x7d,0x00,0x02,0x42,0x1c,0xb1,0x63,0x49,0x1c,0x27,0x33,0x3a,0x61)
#define COPY_PERMITJOIN_UUID(uuid_struct)       COPY_UUID_128(uuid_struct,0x70,0x72,0x37,0x7d,0x00,0x03,0x42,0x1c,0xb1,0x63,0x49,0x1c,0x27,0x33,0x3a,0x61)
#define COPY_LEAVENETWORK_UUID(uuid_struct)       COPY_UUID_128(uuid_struct,0x70,0x72,0x37,0x7d,0x00,0x04,0x42,0x1c,0xb1,0x63,0x49,0x1c,0x27,0x33,0x3a,0x61)
#define COPY_COMMSTATUS_UUID(uuid_struct)       COPY_UUID_128(uuid_struct,0x70,0x72,0x37,0x7d,0x00,0x05,0x42,0x1c,0xb1,0x63,0x49,0x1c,0x27,0x33,0x3a,0x61)
#define COPY_MANAGEJOINER_UUID(uuid_struct)       COPY_UUID_128(uuid_struct,0x70,0x72,0x37,0x7d,0x00,0x06,0x42,0x1c,0xb1,0x63,0x49,0x1c,0x27,0x33,0x3a,0x61)
#define COPY_COMMIDENTITY_UUID(uuid_struct)       COPY_UUID_128(uuid_struct,0x70,0x72,0x37,0x7d,0x00,0x07,0x42,0x1c,0xb1,0x63,0x49,0x1c,0x27,0x33,0x3a,0x61)
#define COPY_FINDBIND_UUID(uuid_struct)       COPY_UUID_128(uuid_struct,0x70,0x72,0x37,0x7d,0x00,0x08,0x42,0x1c,0xb1,0x63,0x49,0x1c,0x27,0x33,0x3a,0x61)

/* USER CODE BEGIN PF */

/* USER CODE END PF */

/**
 * @brief  Event handler
 * @param  p_Event: Address of the buffer holding the p_Event
 * @retval Ack: Return whether the p_Event has been managed or not
 */
static SVCCTL_EvtAckStatus_t ZIGBEEDIRECTCOMM_EventHandler(void *p_Event)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *p_event_pckt;
  evt_blecore_aci *p_blecore_evt;
  aci_gatt_attribute_modified_event_rp0 *p_attribute_modified;
  aci_gatt_write_permit_req_event_rp0   *p_write_perm_req;
  aci_gatt_read_permit_req_event_rp0    *p_read_req;
  ZIGBEEDIRECTCOMM_NotificationEvt_t                 notification;
  /* USER CODE BEGIN Service2_EventHandler_1 */
  ZDD_Stack_Notification_evt_t            notification_wrapper;
  /* USER CODE END Service2_EventHandler_1 */

  return_value = SVCCTL_EvtNotAck;
  p_event_pckt = (hci_event_pckt *)(((hci_uart_pckt*)p_Event)->data);

  switch(p_event_pckt->evt)
  {
    case HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE:
      p_blecore_evt = (evt_blecore_aci*)p_event_pckt->data;
      switch(p_blecore_evt->ecode)
      {
        case ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE:
        {
          /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_BEGIN */

          /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_BEGIN */
          p_attribute_modified = (aci_gatt_attribute_modified_event_rp0*)p_blecore_evt->data;
          notification.ConnectionHandle         = p_attribute_modified->Connection_Handle;
          notification.AttributeHandle          = p_attribute_modified->Attr_Handle;
          notification.DataTransfered.Length    = p_attribute_modified->Attr_Data_Length;
          notification.DataTransfered.p_Payload = p_attribute_modified->Attr_Data;
          if(p_attribute_modified->Attr_Handle == (ZIGBEEDIRECTCOMM_Context.CommstatusCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service2_Char_5 */

            /* USER CODE END Service2_Char_5 */
            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service2_Char_5_attribute_modified */

              /* USER CODE END Service2_Char_5_attribute_modified */

              /* Disabled Notification management */
              case (!(COMSVC_Notification)):
                /* USER CODE BEGIN Service2_Char_5_Disabled_BEGIN */

                /* USER CODE END Service2_Char_5_Disabled_BEGIN */
                notification.EvtOpcode = ZIGBEEDIRECTCOMM_COMMSTATUS_NOTIFY_DISABLED_EVT;
                ZIGBEEDIRECTCOMM_Notification(&notification);
                /* USER CODE BEGIN Service2_Char_5_Disabled_END */

                /* USER CODE END Service2_Char_5_Disabled_END */
                break;

              /* Enabled Notification management */
              case COMSVC_Notification:
                /* USER CODE BEGIN Service2_Char_5_COMSVC_Notification_BEGIN */

                /* USER CODE END Service2_Char_5_COMSVC_Notification_BEGIN */
                notification.EvtOpcode = ZIGBEEDIRECTCOMM_COMMSTATUS_NOTIFY_ENABLED_EVT;
                ZIGBEEDIRECTCOMM_Notification(&notification);
                /* USER CODE BEGIN Service2_Char_5_COMSVC_Notification_END */

                /* USER CODE END Service2_Char_5_COMSVC_Notification_END */
                break;

              default:
                /* USER CODE BEGIN Service2_Char_5_default */

                /* USER CODE END Service2_Char_5_default */
                break;
            }
          }

          /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */

          /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */
          break;/* ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
        }
        case ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE :
        {
          /* USER CODE BEGIN EVT_BLUE_GATT_READ_PERMIT_REQ_BEGIN */

          /* USER CODE END EVT_BLUE_GATT_READ_PERMIT_REQ_BEGIN */
          p_read_req = (aci_gatt_read_permit_req_event_rp0*)p_blecore_evt->data;
          if(p_read_req->Attribute_Handle == (ZIGBEEDIRECTCOMM_Context.CommstatusCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service2_Char_5_ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE_1 */
            notification_wrapper.Opcode = BLE_ZDD_WRAP_ZIGBEEDIRECTCOMM_COMMSTATUS_READ_EVT;
            notification_wrapper.ConnectionHandle = p_read_req->Connection_Handle;
            notification_wrapper.AttributeHandle = p_read_req->Attribute_Handle;
            notification_wrapper.ZigbeedirectcommSvcHdle = ZIGBEEDIRECTCOMM_Context.ZigbeedirectcommSvcHdle;
            notification_wrapper.CommidentityCharHdle = ZIGBEEDIRECTCOMM_Context.CommstatusCharHdle;

            ZDD_Stack_Notification(&notification_wrapper);
            /*USER CODE END Service2_Char_5_ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE_1*/

            /*USER CODE BEGIN Service2_Char_5_ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE_2 */
            aci_gatt_allow_read(p_read_req->Connection_Handle);
            LOG_INFO_APP("Allow read (handle = 0x%04x)", p_read_req->Attribute_Handle);
            notification.EvtOpcode = ZIGBEEDIRECTCOMM_COMMSTATUS_READ_EVT;
            ZIGBEEDIRECTCOMM_Notification(&notification);
            /*USER CODE END Service2_Char_5_ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE_2*/
          }

          /* USER CODE BEGIN EVT_BLUE_GATT_READ_PERMIT_REQ_END */

          /* USER CODE END EVT_BLUE_GATT_READ_PERMIT_REQ_END */
          break;/* ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE */
        }
        case ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE:
        {
          /* USER CODE BEGIN EVT_BLUE_GATT_WRITE_PERMIT_REQ_BEGIN */

          /* USER CODE END EVT_BLUE_GATT_WRITE_PERMIT_REQ_BEGIN */
          p_write_perm_req = (aci_gatt_write_permit_req_event_rp0*)p_blecore_evt->data;
          if(p_write_perm_req->Attribute_Handle == (ZIGBEEDIRECTCOMM_Context.FormnetworkCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service2_Char_1_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
            notification_wrapper.Opcode = BLE_ZDD_WRAP_ZIGBEEDIRECTCOMM_FORMNETWORK_WRITE_EVT;
            notification_wrapper.DataTransfered.pPayload = p_write_perm_req->Data;
            notification_wrapper.DataTransfered.Length = p_write_perm_req->Data_Length;
            notification_wrapper.ConnectionHandle = p_write_perm_req->Connection_Handle;
            notification_wrapper.AttributeHandle = p_write_perm_req->Attribute_Handle;

            ZDD_Stack_Notification(&notification_wrapper);
            /*USER CODE END Service2_Char_1_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE*/
          }

          else if(p_write_perm_req->Attribute_Handle == (ZIGBEEDIRECTCOMM_Context.JoinnetworkCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service2_Char_2_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
            notification_wrapper.Opcode = BLE_ZDD_WRAP_ZIGBEEDIRECTCOMM_JOINNETWORK_WRITE_EVT;
            notification_wrapper.DataTransfered.pPayload = p_write_perm_req->Data;
            notification_wrapper.DataTransfered.Length = p_write_perm_req->Data_Length;
            notification_wrapper.ConnectionHandle = p_write_perm_req->Connection_Handle;
            notification_wrapper.AttributeHandle = p_write_perm_req->Attribute_Handle;

            ZDD_Stack_Notification(&notification_wrapper);
            /*USER CODE END Service2_Char_2_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE*/
          }

          else if(p_write_perm_req->Attribute_Handle == (ZIGBEEDIRECTCOMM_Context.PermitjoinCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service2_Char_3_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
            notification_wrapper.Opcode = BLE_ZDD_WRAP_ZIGBEEDIRECTCOMM_PERMITJOIN_WRITE_EVT;
            notification_wrapper.DataTransfered.pPayload = p_write_perm_req->Data;
            notification_wrapper.DataTransfered.Length = p_write_perm_req->Data_Length;
            notification_wrapper.ConnectionHandle = p_write_perm_req->Connection_Handle;
            notification_wrapper.AttributeHandle = p_write_perm_req->Attribute_Handle;

            ZDD_Stack_Notification(&notification_wrapper);
            /*USER CODE END Service2_Char_3_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE*/
          }

          else if(p_write_perm_req->Attribute_Handle == (ZIGBEEDIRECTCOMM_Context.LeavenetworkCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service2_Char_4_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
            notification_wrapper.Opcode = BLE_ZDD_WRAP_ZIGBEEDIRECTCOMM_LEAVENETWORK_WRITE_EVT;
            notification_wrapper.DataTransfered.pPayload = p_write_perm_req->Data;
            notification_wrapper.DataTransfered.Length = p_write_perm_req->Data_Length;
            notification_wrapper.ConnectionHandle = p_write_perm_req->Connection_Handle;
            notification_wrapper.AttributeHandle = p_write_perm_req->Attribute_Handle;

            ZDD_Stack_Notification(&notification_wrapper);
            /*USER CODE END Service2_Char_4_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE*/
          }

          else if(p_write_perm_req->Attribute_Handle == (ZIGBEEDIRECTCOMM_Context.ManagejoinerCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service2_Char_6_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
            notification_wrapper.Opcode = BLE_ZDD_WRAP_ZIGBEEDIRECTCOMM_MANAGEJOINER_WRITE_EVT;
            notification_wrapper.DataTransfered.pPayload = p_write_perm_req->Data;
            notification_wrapper.DataTransfered.Length = p_write_perm_req->Data_Length;
            notification_wrapper.ConnectionHandle = p_write_perm_req->Connection_Handle;
            notification_wrapper.AttributeHandle = p_write_perm_req->Attribute_Handle;

            ZDD_Stack_Notification(&notification_wrapper);
            /*USER CODE END Service2_Char_6_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE*/
          }

          else if(p_write_perm_req->Attribute_Handle == (ZIGBEEDIRECTCOMM_Context.CommidentityCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service2_Char_7_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
            notification_wrapper.Opcode = BLE_ZDD_WRAP_ZIGBEEDIRECTCOMM_COMMIDENTITY_WRITE_EVT;
            notification_wrapper.DataTransfered.pPayload = p_write_perm_req->Data;
            notification_wrapper.DataTransfered.Length = p_write_perm_req->Data_Length;
            notification_wrapper.ConnectionHandle = p_write_perm_req->Connection_Handle;
            notification_wrapper.AttributeHandle = p_write_perm_req->Attribute_Handle;

            ZDD_Stack_Notification(&notification_wrapper);
            /*USER CODE END Service2_Char_7_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE*/
          }

          else if(p_write_perm_req->Attribute_Handle == (ZIGBEEDIRECTCOMM_Context.FindbindCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service2_Char_8_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
            notification_wrapper.Opcode = BLE_ZDD_WRAP_ZIGBEEDIRECTCOMM_FINDBIND_WRITE_EVT;
            notification_wrapper.DataTransfered.pPayload = p_write_perm_req->Data;
            notification_wrapper.DataTransfered.Length = p_write_perm_req->Data_Length;
            notification_wrapper.ConnectionHandle = p_write_perm_req->Connection_Handle;
            notification_wrapper.AttributeHandle = p_write_perm_req->Attribute_Handle;

            ZDD_Stack_Notification(&notification_wrapper);
            /*USER CODE END Service2_Char_8_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE*/
          }

          /* USER CODE BEGIN EVT_BLUE_GATT_WRITE_PERMIT_REQ_END */

          /* USER CODE END EVT_BLUE_GATT_WRITE_PERMIT_REQ_END */
          break;/* ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
        }
        case ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE:
        {
          aci_gatt_tx_pool_available_event_rp0 *p_tx_pool_available_event;
          p_tx_pool_available_event = (aci_gatt_tx_pool_available_event_rp0 *) p_blecore_evt->data;
          UNUSED(p_tx_pool_available_event);

          /* USER CODE BEGIN ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */

          /* USER CODE END ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */
          break;/* ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE*/
        }
        case ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE:
        {
          aci_att_exchange_mtu_resp_event_rp0 *p_exchange_mtu;
          p_exchange_mtu = (aci_att_exchange_mtu_resp_event_rp0 *)  p_blecore_evt->data;
          UNUSED(p_exchange_mtu);

          /* USER CODE BEGIN ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */

          /* USER CODE END ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
          break;/* ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
        }
        /* USER CODE BEGIN BLECORE_EVT */
        /* Manage ACI_GATT_INDICATION_VSEVT_CODE */
        case ACI_GATT_INDICATION_VSEVT_CODE:
          {
            tBleStatus status = BLE_STATUS_FAILED;
            aci_gatt_indication_event_rp0 *pr = (void*)p_blecore_evt->data;
            status = aci_gatt_confirm_indication(pr->Connection_Handle);
            if (status != BLE_STATUS_SUCCESS)
            {
              LOG_INFO_APP("  Fail   : aci_gatt_confirm_indication command, result: 0x%x \n", status);
            }
            else
            {
              LOG_INFO_APP("  Success: aci_gatt_confirm_indication command\n");
            }
          }
          break; /* end ACI_GATT_NOTIFICATION_VSEVT_CODE */
        /* USER CODE END BLECORE_EVT */
        default:
          /* USER CODE BEGIN EVT_DEFAULT */

          /* USER CODE END EVT_DEFAULT */
          break;
      }
      /* USER CODE BEGIN EVT_VENDOR */

      /* USER CODE END EVT_VENDOR */
      break; /* HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE */

      /* USER CODE BEGIN EVENT_PCKT_CASES */

      /* USER CODE END EVENT_PCKT_CASES */

    default:
      /* USER CODE BEGIN EVENT_PCKT */
      LOG_INFO_APP("unhandled evt");
      /* USER CODE END EVENT_PCKT */
      break;
  }

  /* USER CODE BEGIN Service2_EventHandler_2 */

  /* USER CODE END Service2_EventHandler_2 */

  return(return_value);
}/* end ZIGBEEDIRECTCOMM_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void ZIGBEEDIRECTCOMM_Init(void)
{
  Char_UUID_t  uuid;
  tBleStatus ret;
  uint8_t max_attr_record;

  /* USER CODE BEGIN SVCCTL_InitService2Svc_1 */

  /* USER CODE END SVCCTL_InitService2Svc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  SVCCTL_RegisterSvcHandler(ZIGBEEDIRECTCOMM_EventHandler);

  /**
   * ZigbeeDirectComm
   *
   * Max_Attribute_Records = 1 + 2*8 + 1*no_of_char_with_notify_or_indicate_property + 1*no_of_char_with_broadcast_property
   * service_max_attribute_record = 1 for ZigbeeDirectComm +
   *                                2 for FORMNETWORK +
   *                                2 for JOINNETWORK +
   *                                2 for PERMITJOIN +
   *                                2 for LEAVENETWORK +
   *                                2 for COMMSTATUS +
   *                                2 for MANAGEJOINER +
   *                                2 for COMMIDENTITY +
   *                                2 for FINDBIND +
   *                                1 for COMMSTATUS configuration descriptor +
   *                              = 18
   * This value doesn't take into account number of descriptors manually added
   * In case of descriptors added, please update the max_attr_record value accordingly in the next SVCCTL_InitService User Section
   */
  max_attr_record = 18;

  /* USER CODE BEGIN SVCCTL_InitService */
  /* max_attr_record to be updated if descriptors have been added */

  /* USER CODE END SVCCTL_InitService */

  uuid.Char_UUID_16 = 0xfff7;
  ret = aci_gatt_add_service(UUID_TYPE_16,
                             (Service_UUID_t *) &uuid,
                             PRIMARY_SERVICE,
                             max_attr_record,
                             &(ZIGBEEDIRECTCOMM_Context.ZigbeedirectcommSvcHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_service command: ZigbeeDirectComm, error code: 0x%x \n\r", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_service command: ZigbeeDirectComm \n\r");
  }

  /**
   * FORMNETWORK
   */
  COPY_FORMNETWORK_UUID(uuid.Char_UUID_128);
  ret = aci_gatt_add_char(ZIGBEEDIRECTCOMM_Context.ZigbeedirectcommSvcHdle,
                          UUID_TYPE_128,
                          (Char_UUID_t *) &uuid,
                          SizeFormnetwork,
                          CHAR_PROP_WRITE,
                          ATTR_PERMISSION_NONE,
                          GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x0A,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(ZIGBEEDIRECTCOMM_Context.FormnetworkCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : FORMNETWORK, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : FORMNETWORK\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char1 */
  /* Place holder for Characteristic Descriptors */
  LOG_INFO_APP("Created Comm Form Characteristic (handle = 0x%04x)\n",ZIGBEEDIRECTCOMM_Context.FormnetworkCharHdle);
  /* USER CODE END SVCCTL_InitService2Char1 */

  /**
   * JOINNETWORK
   */
  COPY_JOINNETWORK_UUID(uuid.Char_UUID_128);
  ret = aci_gatt_add_char(ZIGBEEDIRECTCOMM_Context.ZigbeedirectcommSvcHdle,
                          UUID_TYPE_128,
                          (Char_UUID_t *) &uuid,
                          SizeJoinnetwork,
                          CHAR_PROP_WRITE,
                          ATTR_PERMISSION_NONE,
                          GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x0A,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(ZIGBEEDIRECTCOMM_Context.JoinnetworkCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : JOINNETWORK, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : JOINNETWORK\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char2 */
  /* Place holder for Characteristic Descriptors */
  LOG_INFO_APP("Created Comm Join Characteristic (handle = 0x%04x)\n",ZIGBEEDIRECTCOMM_Context.JoinnetworkCharHdle);
  /* USER CODE END SVCCTL_InitService2Char2 */

  /**
   * PERMITJOIN
   */
  COPY_PERMITJOIN_UUID(uuid.Char_UUID_128);
  ret = aci_gatt_add_char(ZIGBEEDIRECTCOMM_Context.ZigbeedirectcommSvcHdle,
                          UUID_TYPE_128,
                          (Char_UUID_t *) &uuid,
                          SizePermitjoin,
                          CHAR_PROP_WRITE,
                          ATTR_PERMISSION_NONE,
                          GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x0A,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(ZIGBEEDIRECTCOMM_Context.PermitjoinCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : PERMITJOIN, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : PERMITJOIN\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char3 */
  /* Place holder for Characteristic Descriptors */
  LOG_INFO_APP("Created Comm PJoin Characteristic (handle = 0x%04x)\n",ZIGBEEDIRECTCOMM_Context.PermitjoinCharHdle);
  /* USER CODE END SVCCTL_InitService2Char3 */

  /**
   * LEAVENETWORK
   */
  COPY_LEAVENETWORK_UUID(uuid.Char_UUID_128);
  ret = aci_gatt_add_char(ZIGBEEDIRECTCOMM_Context.ZigbeedirectcommSvcHdle,
                          UUID_TYPE_128,
                          (Char_UUID_t *) &uuid,
                          SizeLeavenetwork,
                          CHAR_PROP_WRITE,
                          ATTR_PERMISSION_NONE,
                          GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x0A,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(ZIGBEEDIRECTCOMM_Context.LeavenetworkCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : LEAVENETWORK, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : LEAVENETWORK\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char4 */
  /* Place holder for Characteristic Descriptors */
  LOG_INFO_APP("Created Comm Leave Characteristic (handle = 0x%04x)\n",ZIGBEEDIRECTCOMM_Context.LeavenetworkCharHdle);
  /* USER CODE END SVCCTL_InitService2Char4 */

  /**
   * COMMSTATUS
   */
  COPY_COMMSTATUS_UUID(uuid.Char_UUID_128);
  ret = aci_gatt_add_char(ZIGBEEDIRECTCOMM_Context.ZigbeedirectcommSvcHdle,
                          UUID_TYPE_128,
                          (Char_UUID_t *) &uuid,
                          SizeCommstatus,
                          CHAR_PROP_READ | CHAR_PROP_NOTIFY,
                          ATTR_PERMISSION_NONE,
                          GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x0A,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(ZIGBEEDIRECTCOMM_Context.CommstatusCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : COMMSTATUS, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : COMMSTATUS\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char5 */
  /* Place holder for Characteristic Descriptors */
  LOG_INFO_APP("Created Comm Status Characteristic (handle = 0x%04x)\n",ZIGBEEDIRECTCOMM_Context.CommstatusCharHdle);
  /* USER CODE END SVCCTL_InitService2Char5 */

  /**
   * MANAGEJOINER
   */
  COPY_MANAGEJOINER_UUID(uuid.Char_UUID_128);
  ret = aci_gatt_add_char(ZIGBEEDIRECTCOMM_Context.ZigbeedirectcommSvcHdle,
                          UUID_TYPE_128,
                          (Char_UUID_t *) &uuid,
                          SizeManagejoiner,
                          CHAR_PROP_WRITE,
                          ATTR_PERMISSION_NONE,
                          GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x0A,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(ZIGBEEDIRECTCOMM_Context.ManagejoinerCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : MANAGEJOINER, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : MANAGEJOINER\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char6 */
  /* Place holder for Characteristic Descriptors */
  LOG_INFO_APP("Created Comm ManageJoiners Characteristic (handle = 0x%04x)\n",ZIGBEEDIRECTCOMM_Context.ManagejoinerCharHdle);
  /* USER CODE END SVCCTL_InitService2Char6 */

  /**
   * COMMIDENTITY
   */
  COPY_COMMIDENTITY_UUID(uuid.Char_UUID_128);
  ret = aci_gatt_add_char(ZIGBEEDIRECTCOMM_Context.ZigbeedirectcommSvcHdle,
                          UUID_TYPE_128,
                          (Char_UUID_t *) &uuid,
                          SizeCommidentity,
                          CHAR_PROP_READ | CHAR_PROP_WRITE,
                          ATTR_PERMISSION_NONE,
                          GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x0A,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(ZIGBEEDIRECTCOMM_Context.CommidentityCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : COMMIDENTITY, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : COMMIDENTITY\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char7 */
  /* Place holder for Characteristic Descriptors */
  LOG_INFO_APP("Created Comm Identify Characteristic (handle = 0x%04x)\n",ZIGBEEDIRECTCOMM_Context.CommidentityCharHdle);
  /* USER CODE END SVCCTL_InitService2Char7 */

  /**
   * FINDBIND
   */
  COPY_FINDBIND_UUID(uuid.Char_UUID_128);
  ret = aci_gatt_add_char(ZIGBEEDIRECTCOMM_Context.ZigbeedirectcommSvcHdle,
                          UUID_TYPE_128,
                          (Char_UUID_t *) &uuid,
                          SizeFindbind,
                          CHAR_PROP_WRITE,
                          ATTR_PERMISSION_NONE,
                          GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x0A,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(ZIGBEEDIRECTCOMM_Context.FindbindCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : FINDBIND, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : FINDBIND\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char8 */
  /* Place holder for Characteristic Descriptors */
  LOG_INFO_APP("Created Comm FindBind Characteristic (handle = 0x%04x)\n",ZIGBEEDIRECTCOMM_Context.FindbindCharHdle);
  /* USER CODE END SVCCTL_InitService2Char8 */

  /* USER CODE BEGIN SVCCTL_InitService2Svc_2 */

  /* USER CODE END SVCCTL_InitService2Svc_2 */

  return;
}

/**
 * @brief  Characteristic update
 * @param  CharOpcode: Characteristic identifier
 * @param  pData: Structure holding data to update
 *
 */
tBleStatus ZIGBEEDIRECTCOMM_UpdateValue(ZIGBEEDIRECTCOMM_CharOpcode_t CharOpcode, ZIGBEEDIRECTCOMM_Data_t *pData)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Service2_App_Update_Char_1 */

  /* USER CODE END Service2_App_Update_Char_1 */

  switch(CharOpcode)
  {
    case ZIGBEEDIRECTCOMM_FORMNETWORK:
      ret = aci_gatt_update_char_value(ZIGBEEDIRECTCOMM_Context.ZigbeedirectcommSvcHdle,
                                       ZIGBEEDIRECTCOMM_Context.FormnetworkCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value FORMNETWORK command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value FORMNETWORK command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_1 */

      /* USER CODE END Service2_Char_Value_1 */
      break;

    case ZIGBEEDIRECTCOMM_JOINNETWORK:
      ret = aci_gatt_update_char_value(ZIGBEEDIRECTCOMM_Context.ZigbeedirectcommSvcHdle,
                                       ZIGBEEDIRECTCOMM_Context.JoinnetworkCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value JOINNETWORK command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value JOINNETWORK command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_2 */

      /* USER CODE END Service2_Char_Value_2 */
      break;

    case ZIGBEEDIRECTCOMM_PERMITJOIN:
      ret = aci_gatt_update_char_value(ZIGBEEDIRECTCOMM_Context.ZigbeedirectcommSvcHdle,
                                       ZIGBEEDIRECTCOMM_Context.PermitjoinCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value PERMITJOIN command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value PERMITJOIN command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_3 */

      /* USER CODE END Service2_Char_Value_3 */
      break;

    case ZIGBEEDIRECTCOMM_LEAVENETWORK:
      ret = aci_gatt_update_char_value(ZIGBEEDIRECTCOMM_Context.ZigbeedirectcommSvcHdle,
                                       ZIGBEEDIRECTCOMM_Context.LeavenetworkCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value LEAVENETWORK command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value LEAVENETWORK command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_4 */

      /* USER CODE END Service2_Char_Value_4 */
      break;

    case ZIGBEEDIRECTCOMM_COMMSTATUS:
      ret = aci_gatt_update_char_value(ZIGBEEDIRECTCOMM_Context.ZigbeedirectcommSvcHdle,
                                       ZIGBEEDIRECTCOMM_Context.CommstatusCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value COMMSTATUS command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value COMMSTATUS command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_5 */

      /* USER CODE END Service2_Char_Value_5 */
      break;

    case ZIGBEEDIRECTCOMM_MANAGEJOINER:
      ret = aci_gatt_update_char_value(ZIGBEEDIRECTCOMM_Context.ZigbeedirectcommSvcHdle,
                                       ZIGBEEDIRECTCOMM_Context.ManagejoinerCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value MANAGEJOINER command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value MANAGEJOINER command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_6 */

      /* USER CODE END Service2_Char_Value_6 */
      break;

    case ZIGBEEDIRECTCOMM_COMMIDENTITY:
      ret = aci_gatt_update_char_value(ZIGBEEDIRECTCOMM_Context.ZigbeedirectcommSvcHdle,
                                       ZIGBEEDIRECTCOMM_Context.CommidentityCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value COMMIDENTITY command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value COMMIDENTITY command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_7 */

      /* USER CODE END Service2_Char_Value_7 */
      break;

    case ZIGBEEDIRECTCOMM_FINDBIND:
      ret = aci_gatt_update_char_value(ZIGBEEDIRECTCOMM_Context.ZigbeedirectcommSvcHdle,
                                       ZIGBEEDIRECTCOMM_Context.FindbindCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value FINDBIND command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value FINDBIND command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_8 */

      /* USER CODE END Service2_Char_Value_8 */
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service2_App_Update_Char_2 */

  /* USER CODE END Service2_App_Update_Char_2 */

  return ret;
}
