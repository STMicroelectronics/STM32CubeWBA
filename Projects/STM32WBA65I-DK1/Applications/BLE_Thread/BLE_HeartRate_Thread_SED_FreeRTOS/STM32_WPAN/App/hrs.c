/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    hrs.c
  * @author  MCD Application Team
  * @brief   hrs definition.
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
#include "log_module.h"
#include "common_blesvc.h"
#include "hrs.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef struct{
  uint16_t  HrsSvcHdle;                  /**< Hrs Service Handle */
  uint16_t  HrmeCharHdle;                  /**< HRME Characteristic Handle */
  uint16_t  BslCharHdle;                  /**< BSL Characteristic Handle */
  uint16_t  HrcpCharHdle;                  /**< HRCP Characteristic Handle */
/* USER CODE BEGIN Context */
  /* Place holder for Characteristic Descriptors Handle*/

/* USER CODE END Context */
}HRS_Context_t;

/* Private defines -----------------------------------------------------------*/
#define UUID_128_SUPPORTED  1

#if (UUID_128_SUPPORTED == 1)
#define BM_UUID_LENGTH  UUID_TYPE_128
#else
#define BM_UUID_LENGTH  UUID_TYPE_16
#endif

#define BM_REQ_CHAR_SIZE    (3)

/* USER CODE BEGIN PD */
#define HRS_CNTL_POINT_RESET_ENERGY_EXPENDED      (0x01)
#define HRS_CNTL_POINT_VALUE_IS_SUPPORTED         (0x00)
#define HRS_CNTL_POINT_VALUE_NOT_SUPPORTED        (0x80)

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
static const uint16_t SizeHrme = 23;
static const uint16_t SizeBsl = 1;
static const uint16_t SizeHrcp = 1;

static HRS_Context_t HRS_Context;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static SVCCTL_EvtAckStatus_t HRS_EventHandler(void *p_pckt);

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

/* USER CODE BEGIN PF */

/* USER CODE END PF */

/**
 * @brief  Event handler
 * @param  p_Event: Address of the buffer holding the p_Event
 * @retval Ack: Return whether the p_Event has been managed or not
 */
static SVCCTL_EvtAckStatus_t HRS_EventHandler(void *p_Event)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *p_event_pckt;
  evt_blecore_aci *p_blecore_evt;
  aci_gatt_attribute_modified_event_rp0 *p_attribute_modified;
  aci_gatt_write_permit_req_event_rp0   *p_write_perm_req;
  HRS_NotificationEvt_t                 notification;
  /* USER CODE BEGIN Service1_EventHandler_1 */

  /* USER CODE END Service1_EventHandler_1 */

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
          if(p_attribute_modified->Attr_Handle == (HRS_Context.HrmeCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service1_Char_1 */
                LOG_INFO_APP("ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE HRS_NOTIFICATION_DISABLED\n");

            /* USER CODE END Service1_Char_1 */
            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service1_Char_1_attribute_modified */

              /* USER CODE END Service1_Char_1_attribute_modified */

              /* Disabled Notification management */
              case (!(COMSVC_Notification)):
                /* USER CODE BEGIN Service1_Char_1_Disabled_BEGIN */
                LOG_INFO_APP("ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE HRS_NOTIFICATION_DISABLED\n");
                /* USER CODE END Service1_Char_1_Disabled_BEGIN */
                notification.EvtOpcode = HRS_HRME_NOTIFY_DISABLED_EVT;
                HRS_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_1_Disabled_END */

                /* USER CODE END Service1_Char_1_Disabled_END */
                break;

              /* Enabled Notification management */
              case COMSVC_Notification:
                /* USER CODE BEGIN Service1_Char_1_COMSVC_Notification_BEGIN */
                LOG_INFO_APP("ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE HRS_NOTIFICATION_ENABLED\n");
                /* USER CODE END Service1_Char_1_COMSVC_Notification_BEGIN */
                notification.EvtOpcode = HRS_HRME_NOTIFY_ENABLED_EVT;
                HRS_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_1_COMSVC_Notification_END */

                /* USER CODE END Service1_Char_1_COMSVC_Notification_END */
                break;

              default:
                /* USER CODE BEGIN Service1_Char_1_default */

                /* USER CODE END Service1_Char_1_default */
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
          if(p_write_perm_req->Attribute_Handle == (HRS_Context.HrcpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service1_Char_3_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */

            if (p_write_perm_req->Data[0] == HRS_CNTL_POINT_RESET_ENERGY_EXPENDED)
            {
              /* received a correct value for HRM control point char */
              aci_gatt_write_resp(p_write_perm_req->Connection_Handle,
                                  p_write_perm_req->Attribute_Handle,
                                  0x00, /* write_status = 0 (no error))*/
                                  (uint8_t)HRS_CNTL_POINT_VALUE_IS_SUPPORTED, /* err_code */
                                  p_write_perm_req->Data_Length,
                                  (uint8_t *)&p_write_perm_req->Data[0]);

              /**
               * Notify the application to Reset The Energy Expended Value
               */
              notification.EvtOpcode = HRS_RESET_ENERGY_EXPENDED_EVT;
              HRS_Notification(&notification);
            }
            else
            {
              /* received value of HRM control point char is incorrect */
              aci_gatt_write_resp(p_write_perm_req->Connection_Handle,
                                  p_write_perm_req->Attribute_Handle,
                                  0x1, /* write_status = 1 (error))*/
                                  (uint8_t)HRS_CNTL_POINT_VALUE_NOT_SUPPORTED, /* err_code */
                                  p_write_perm_req->Data_Length,
                                  (uint8_t *)&p_write_perm_req->Data[0]);
            }

            /*USER CODE END Service1_Char_3_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE*/
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
          break; /* end ACI_GATT_INDICATION_VSEVT_CODE */
        }
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

  /* USER CODE BEGIN Service1_EventHandler_2 */

  /* USER CODE END Service1_EventHandler_2 */

  return(return_value);
}/* end HRS_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void HRS_Init(void)
{
  Char_UUID_t  uuid;
  tBleStatus ret;
  uint8_t max_attr_record;

  /* USER CODE BEGIN SVCCTL_InitService1Svc_1 */

  /* USER CODE END SVCCTL_InitService1Svc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  SVCCTL_RegisterSvcHandler(HRS_EventHandler);

  /**
   * HRS
   *
   * Max_Attribute_Records = 1 + 2*3 + 1*no_of_char_with_notify_or_indicate_property + 1*no_of_char_with_broadcast_property
   * service_max_attribute_record = 1 for HRS +
   *                                2 for HRME +
   *                                2 for BSL +
   *                                2 for HRCP +
   *                                1 for HRME configuration descriptor +
   *                              = 8
   * This value doesn't take into account number of descriptors manually added
   * In case of descriptors added, please update the max_attr_record value accordingly in the next SVCCTL_InitService User Section
   */
  max_attr_record = 8;

  /* USER CODE BEGIN SVCCTL_InitService */
  /* max_attr_record to be updated if descriptors have been added */

  /* USER CODE END SVCCTL_InitService */

  uuid.Char_UUID_16 = 0x180d;
  ret = aci_gatt_add_service(UUID_TYPE_16,
                             (Service_UUID_t *) &uuid,
                             PRIMARY_SERVICE,
                             max_attr_record,
                             &(HRS_Context.HrsSvcHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_service command: HRS, error code: 0x%x \n\r", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_service command: HRS \n\r");
  }

  /**
   * HRME
   */
  uuid.Char_UUID_16 = 0x2a37;
  ret = aci_gatt_add_char(HRS_Context.HrsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeHrme,
                          CHAR_PROP_NOTIFY,
                          ATTR_PERMISSION_NONE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(HRS_Context.HrmeCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : HRME, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : HRME\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char1 */

  /* USER CODE END SVCCTL_InitService1Char1 */

  /**
   * BSL
   */
  uuid.Char_UUID_16 = 0x2a38;
  ret = aci_gatt_add_char(HRS_Context.HrsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeBsl,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_NONE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(HRS_Context.BslCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : BSL, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : BSL\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char2 */

  /* USER CODE END SVCCTL_InitService1Char2 */

  /**
   * HRCP
   */
  uuid.Char_UUID_16 = 0x2a39;
  ret = aci_gatt_add_char(HRS_Context.HrsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeHrcp,
                          CHAR_PROP_WRITE,
                          ATTR_PERMISSION_NONE,
                          GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(HRS_Context.HrcpCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : HRCP, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : HRCP\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char3 */

  /* USER CODE END SVCCTL_InitService1Char3 */

  /* USER CODE BEGIN SVCCTL_InitService1Svc_2 */

  /* USER CODE END SVCCTL_InitService1Svc_2 */

  return;
}

/**
 * @brief  Characteristic update
 * @param  CharOpcode: Characteristic identifier
 * @param  pData: Structure holding data to update
 *
 */
tBleStatus HRS_UpdateValue(HRS_CharOpcode_t CharOpcode, HRS_Data_t *pData)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Service1_App_Update_Char_1 */

  /* USER CODE END Service1_App_Update_Char_1 */

  switch(CharOpcode)
  {
    case HRS_HRME:
      ret = aci_gatt_update_char_value(HRS_Context.HrsSvcHdle,
                                       HRS_Context.HrmeCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value HRME command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value HRME command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_1 */

      /* USER CODE END Service1_Char_Value_1 */
      break;

    case HRS_BSL:
      ret = aci_gatt_update_char_value(HRS_Context.HrsSvcHdle,
                                       HRS_Context.BslCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value BSL command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value BSL command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_2 */

      /* USER CODE END Service1_Char_Value_2 */
      break;

    case HRS_HRCP:
      ret = aci_gatt_update_char_value(HRS_Context.HrsSvcHdle,
                                       HRS_Context.HrcpCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value HRCP command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value HRCP command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_3 */

      /* USER CODE END Service1_Char_Value_3 */
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service1_App_Update_Char_2 */

  /* USER CODE END Service1_App_Update_Char_2 */

  return ret;
}
