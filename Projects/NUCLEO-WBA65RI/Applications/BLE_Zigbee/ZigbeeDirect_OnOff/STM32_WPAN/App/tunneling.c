/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Tunneling.c
  * @author  MCD Application Team
  * @brief   Tunneling definition.
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
#include "tunneling.h"

/* USER CODE BEGIN Includes */
#include "zigbee.zd.h"
#include "zdd_stack_wrapper.h"
#include "stm32_rtos.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef struct{
  uint16_t  TunnelingSvcHdle;                  /**< Tunneling Service Handle */
  uint16_t  TunnzdtsnpduCharHdle;                  /**< TUNNZDTSNPDU Characteristic Handle */
/* USER CODE BEGIN Context */
  /* Place holder for Characteristic Descriptors Handle*/

/* USER CODE END Context */
}TUNNELING_Context_t;

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
static const uint16_t SizeTunnzdtsnpdu = 160;

static TUNNELING_Context_t TUNNELING_Context;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static SVCCTL_EvtAckStatus_t TUNNELING_EventHandler(void *p_pckt);

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
 8BD178FD000045F48120B2378BD5313F: Service 128bits UUID
 8BD178FD000145F48120B2378BD5313F: Characteristic 128bits UUID
 */
#define COPY_TUNNELING_UUID(uuid_struct)       COPY_UUID_128(uuid_struct,0x8b,0xd1,0x78,0xfd,0x00,0x00,0x45,0xf4,0x81,0x20,0xb2,0x37,0x8b,0xd5,0x31,0x3f)
#define COPY_TUNNZDTSNPDU_UUID(uuid_struct)       COPY_UUID_128(uuid_struct,0x8b,0xd1,0x78,0xfd,0x00,0x01,0x45,0xf4,0x81,0x20,0xb2,0x37,0x8b,0xd5,0x31,0x3f)

/* USER CODE BEGIN PF */

/* USER CODE END PF */

/**
 * @brief  Event handler
 * @param  p_Event: Address of the buffer holding the p_Event
 * @retval Ack: Return whether the p_Event has been managed or not
 */
static SVCCTL_EvtAckStatus_t TUNNELING_EventHandler(void *p_Event)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *p_event_pckt;
  evt_blecore_aci *p_blecore_evt;
  aci_gatt_attribute_modified_event_rp0 *p_attribute_modified;
  aci_gatt_write_permit_req_event_rp0   *p_write_perm_req;
  TUNNELING_NotificationEvt_t                 notification;
  /* USER CODE BEGIN Service4_EventHandler_1 */
  ZDD_Stack_Notification_evt_t            notification_wrapper;
  /* USER CODE END Service4_EventHandler_1 */

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
          if(p_attribute_modified->Attr_Handle == (TUNNELING_Context.TunnzdtsnpduCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service4_Char_1 */
            LOG_INFO_APP("Processing Indicate for Tunn Npdu (handle = 0x%04x, length = %d)",
                         p_attribute_modified->Attr_Handle, p_attribute_modified->Attr_Data_Length);
            /* USER CODE END Service4_Char_1 */

            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service4_Char_1_attribute_modified */

              /* USER CODE END Service4_Char_1_attribute_modified */

              /* Disabled Indication management */
              case (!(COMSVC_Indication)):
                /* USER CODE BEGIN Service4_Char_1_Disabled_BEGIN */

                /* USER CODE END Service4_Char_1_Disabled_BEGIN */
                notification.EvtOpcode = TUNNELING_TUNNZDTSNPDU_INDICATE_DISABLED_EVT;
                TUNNELING_Notification(&notification);
                /* USER CODE BEGIN Service4_Char_1_Disabled_END */

                /* USER CODE END Service4_Char_1_Disabled_END */
                break;

              /* Enabled Indication management */
              case COMSVC_Indication:
                /* USER CODE BEGIN Service4_Char_1_COMSVC_Indication_BEGIN */

                /* USER CODE END Service4_Char_1_COMSVC_Indication_BEGIN */
                notification.EvtOpcode = TUNNELING_TUNNZDTSNPDU_INDICATE_ENABLED_EVT;
                TUNNELING_Notification(&notification);
                /* USER CODE BEGIN Service4_Char_1_COMSVC_Indication_END */

                /* USER CODE END Service4_Char_1_COMSVC_Indication_END */
                break;

              default:
                /* USER CODE BEGIN Service4_Char_1_default */

                /* USER CODE END Service4_Char_1_default */
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

          /* USER CODE BEGIN EVT_BLUE_GATT_READ_PERMIT_REQ_END */

          /* USER CODE END EVT_BLUE_GATT_READ_PERMIT_REQ_END */
          break;/* ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE */
        }
        case ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE:
        {
          /* USER CODE BEGIN EVT_BLUE_GATT_WRITE_PERMIT_REQ_BEGIN */

          /* USER CODE END EVT_BLUE_GATT_WRITE_PERMIT_REQ_BEGIN */
          p_write_perm_req = (aci_gatt_write_permit_req_event_rp0*)p_blecore_evt->data;
          if(p_write_perm_req->Attribute_Handle == (TUNNELING_Context.TunnzdtsnpduCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service4_Char_1_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
            notification_wrapper.Opcode = BLE_ZDD_WRAP_TUNNELING_TUNNZDTSNPDU_WRITE_EVT;
            notification_wrapper.DataTransfered.pPayload = p_write_perm_req->Data;
            notification_wrapper.DataTransfered.Length = p_write_perm_req->Data_Length;
            notification_wrapper.ConnectionHandle = p_write_perm_req->Connection_Handle;
            notification_wrapper.AttributeHandle = p_write_perm_req->Attribute_Handle;

            ZDD_Stack_Notification(&notification_wrapper);
            /*USER CODE END Service4_Char_1_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE*/
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
        case ACI_GATT_SERVER_CONFIRMATION_VSEVT_CODE:
        {
          aci_gatt_server_confirmation_event_rp0 *p_server_confirmation;
          p_server_confirmation = (aci_gatt_server_confirmation_event_rp0 *)  p_blecore_evt->data;
          UNUSED(p_server_confirmation);

          /* USER CODE BEGIN ACI_GATT_SERVER_CONFIRMATION_VSEVT_CODE */
          return_value = SVCCTL_EvtAckFlowEnable;

          notification_wrapper.Opcode = BLE_ZDD_WRAP_SERVER_CONFIRMATION_EVT;
          notification_wrapper.ConnectionHandle = p_server_confirmation->Connection_Handle;
          ZDD_Stack_Notification(&notification_wrapper);
          /* USER CODE END ACI_GATT_SERVER_CONFIRMATION_VSEVT_CODE */
          break;/* ACI_GATT_SERVER_CONFIRMATION_VSEVT_CODE */
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

      /* USER CODE END EVENT_PCKT */
      break;
  }

  /* USER CODE BEGIN Service4_EventHandler_2 */

  /* USER CODE END Service4_EventHandler_2 */

  return(return_value);
}/* end TUNNELING_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void TUNNELING_Init(void)
{
  Char_UUID_t  uuid;
  tBleStatus ret;
  uint8_t max_attr_record;

  /* USER CODE BEGIN SVCCTL_InitService4Svc_1 */

  /* USER CODE END SVCCTL_InitService4Svc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  SVCCTL_RegisterSvcHandler(TUNNELING_EventHandler);

  /**
   * Tunneling
   *
   * Max_Attribute_Records = 1 + 2*1 + 1*no_of_char_with_notify_or_indicate_property + 1*no_of_char_with_broadcast_property
   * service_max_attribute_record = 1 for Tunneling +
   *                                2 for TUNNZDTSNPDU +
   *                                1 for TUNNZDTSNPDU configuration descriptor +
   *                              = 4
   * This value doesn't take into account number of descriptors manually added
   * In case of descriptors added, please update the max_attr_record value accordingly in the next SVCCTL_InitService User Section
   */
  max_attr_record = 4;

  /* USER CODE BEGIN SVCCTL_InitService */
  /* max_attr_record to be updated if descriptors have been added */

  /* USER CODE END SVCCTL_InitService */

  COPY_TUNNELING_UUID(uuid.Char_UUID_128);
  ret = aci_gatt_add_service(UUID_TYPE_128,
                             (Service_UUID_t *) &uuid,
                             PRIMARY_SERVICE,
                             max_attr_record,
                             &(TUNNELING_Context.TunnelingSvcHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_service command: Tunneling, error code: 0x%x \n\r", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_service command: Tunneling \n\r");
  }

  /**
   * TUNNZDTSNPDU
   */
  COPY_TUNNZDTSNPDU_UUID(uuid.Char_UUID_128);
  ret = aci_gatt_add_char(TUNNELING_Context.TunnelingSvcHdle,
                          UUID_TYPE_128,
                          (Char_UUID_t *) &uuid,
                          SizeTunnzdtsnpdu,
                          CHAR_PROP_WRITE | CHAR_PROP_INDICATE,
                          ATTR_PERMISSION_NONE,
                          GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x0A,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(TUNNELING_Context.TunnzdtsnpduCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : TUNNZDTSNPDU, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : TUNNZDTSNPDU\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService4Char1 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService4Char1 */

  /* USER CODE BEGIN SVCCTL_InitService4Svc_2 */

  /* USER CODE END SVCCTL_InitService4Svc_2 */

  return;
}

/**
 * @brief  Characteristic update
 * @param  CharOpcode: Characteristic identifier
 * @param  pData: Structure holding data to update
 *
 */
tBleStatus TUNNELING_UpdateValue(TUNNELING_CharOpcode_t CharOpcode, TUNNELING_Data_t *pData)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Service4_App_Update_Char_1 */

  /* USER CODE END Service4_App_Update_Char_1 */

  switch(CharOpcode)
  {
    case TUNNELING_TUNNZDTSNPDU:
      ret = aci_gatt_update_char_value(TUNNELING_Context.TunnelingSvcHdle,
                                       TUNNELING_Context.TunnzdtsnpduCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value TUNNZDTSNPDU command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value TUNNZDTSNPDU command\n");
      }
      /* USER CODE BEGIN Service4_Char_Value_1 */

      /* USER CODE END Service4_Char_Value_1 */
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service4_App_Update_Char_2 */

  /* USER CODE END Service4_App_Update_Char_2 */

  return ret;
}
