/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service1.c
  * @author  MCD Application Team
  * @brief   service1 definition.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "hids.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef struct{
  uint16_t  HidsSvcHdle;                  /**< Hids Service Handle */
  uint16_t  InputrepCharHdle;                  /**< INPUTREP Characteristic Handle */
  uint16_t  RemCharHdle;                  /**< REM Characteristic Handle */
  uint16_t  HiiCharHdle;                  /**< HII Characteristic Handle */
  uint16_t  HcpCharHdle;                  /**< HCP Characteristic Handle */
/* USER CODE BEGIN Context */
  /* Place holder for Characteristic Descriptors Handle*/
  uint16_t  InputRepRefDescHdle;          /**< INPUTREP REF Descriptor Handle */
/* USER CODE END Context */
}HIDS_Context_t;

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
static const uint16_t SizeInputrep = 80;
static const uint16_t SizeRem = 80;
static const uint16_t SizeHii = 4;
static const uint16_t SizeHcp = 1;

static HIDS_Context_t HIDS_Context;

/* USER CODE BEGIN PV */
static const uint8_t SizeRepRefDesc = 2;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static SVCCTL_EvtAckStatus_t HIDS_EventHandler(void *p_pckt);

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
static SVCCTL_EvtAckStatus_t HIDS_EventHandler(void *p_Event)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *p_event_pckt;
  evt_blecore_aci *p_blecore_evt;
  aci_gatt_attribute_modified_event_rp0 *p_attribute_modified;
  aci_gatt_write_permit_req_event_rp0   *p_write_perm_req;
  HIDS_NotificationEvt_t                 notification;
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
          if(p_attribute_modified->Attr_Handle == (HIDS_Context.InputrepCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service1_Char_1 */

            /* USER CODE END Service1_Char_1 */
            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service1_Char_1_attribute_modified */

              /* USER CODE END Service1_Char_1_attribute_modified */

              /* Disabled Notification management */
              case (!(COMSVC_Notification)):
                /* USER CODE BEGIN Service1_Char_1_Disabled_BEGIN */

                /* USER CODE END Service1_Char_1_Disabled_BEGIN */
                notification.EvtOpcode = HIDS_INPUTREP_NOTIFY_DISABLED_EVT;
                HIDS_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_1_Disabled_END */

                /* USER CODE END Service1_Char_1_Disabled_END */
                break;

              /* Enabled Notification management */
              case COMSVC_Notification:
                /* USER CODE BEGIN Service1_Char_1_COMSVC_Notification_BEGIN */

                /* USER CODE END Service1_Char_1_COMSVC_Notification_BEGIN */
                notification.EvtOpcode = HIDS_INPUTREP_NOTIFY_ENABLED_EVT;
                HIDS_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_1_COMSVC_Notification_END */

                /* USER CODE END Service1_Char_1_COMSVC_Notification_END */
                break;

              default:
                /* USER CODE BEGIN Service1_Char_1_default */

                /* USER CODE END Service1_Char_1_default */
                break;
            }
          }  /* if(p_attribute_modified->Attr_Handle == (HIDS_Context.InputrepCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

          else if(p_attribute_modified->Attr_Handle == (HIDS_Context.InputrepCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;

            notification.EvtOpcode = HIDS_INPUTREP_WRITE_EVT;
            /* USER CODE BEGIN Service1_Char_1_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */

            /* USER CODE END Service1_Char_1_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
            HIDS_Notification(&notification);
          } /* if(p_attribute_modified->Attr_Handle == (HIDS_Context.InputrepCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/
          else if(p_attribute_modified->Attr_Handle == (HIDS_Context.HcpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;

            notification.EvtOpcode = HIDS_HCP_WRITE_NO_RESP_EVT;
            /* USER CODE BEGIN Service1_Char_4_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */

            /* USER CODE END Service1_Char_4_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
            HIDS_Notification(&notification);
          } /* if(p_attribute_modified->Attr_Handle == (HIDS_Context.HcpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

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
          if(p_write_perm_req->Attribute_Handle == (HIDS_Context.InputrepCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service1_Char_1_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
            aci_gatt_write_resp(p_write_perm_req->Connection_Handle,
                                p_write_perm_req->Attribute_Handle,
                                0x00, /* write_status = 0 (no error))*/
                                (uint8_t)0, /* err_code */
                                p_write_perm_req->Data_Length,
                                (uint8_t *)&p_write_perm_req->Data[0]);
            /*USER CODE END Service1_Char_1_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE*/
          } /*if(p_write_perm_req->Attribute_Handle == (HIDS_Context.InputrepCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

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

        /* USER CODE END BLECORE_EVT */
        default:
          /* USER CODE BEGIN EVT_DEFAULT */

          /* USER CODE END EVT_DEFAULT */
          break;
      }
      /* USER CODE BEGIN EVT_VENDOR*/

      /* USER CODE END EVT_VENDOR*/
      break; /* HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE */

      /* USER CODE BEGIN EVENT_PCKT_CASES*/

      /* USER CODE END EVENT_PCKT_CASES*/

    default:
      /* USER CODE BEGIN EVENT_PCKT*/

      /* USER CODE END EVENT_PCKT*/
      break;
  }

  /* USER CODE BEGIN Service1_EventHandler_2 */

  /* USER CODE END Service1_EventHandler_2 */

  return(return_value);
}/* end HIDS_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void HIDS_Init(void)
{
  Char_UUID_t  uuid;
  tBleStatus ret;
  uint8_t max_attr_record;

  /* USER CODE BEGIN SVCCTL_InitService1Svc_1 */
  uint8_t buf[2] = {0,0};
  /* USER CODE END SVCCTL_InitService1Svc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  SVCCTL_RegisterSvcHandler(HIDS_EventHandler);

  /**
   * HIDS
   *
   * Max_Attribute_Records = 1 + 2*4 + 1*no_of_char_with_notify_or_indicate_property + 1*no_of_char_with_broadcast_property
   * service_max_attribute_record = 1 for HIDS +
   *                                2 for INPUTREP +
   *                                2 for REM +
   *                                2 for HII +
   *                                2 for HCP +
   *                                1 for INPUTREP configuration descriptor +
   *                              = 10
   * This value doesn't take into account number of descriptors manually added
   * In case of descriptors added, please update the max_attr_record value accordingly in the next SVCCTL_InitService User Section
   */
  max_attr_record = 10;

  /* USER CODE BEGIN SVCCTL_InitService */
  /* max_attr_record to be updated if descriptors have been added */
  /* Added 1 Report Reference characteristic descriptors */
  max_attr_record += 1;
  /* USER CODE END SVCCTL_InitService */

  uuid.Char_UUID_16 = 0x1812;
  ret = aci_gatt_add_service(UUID_TYPE_16,
                             (Service_UUID_t *) &uuid,
                             PRIMARY_SERVICE,
                             max_attr_record,
                             &(HIDS_Context.HidsSvcHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_service command: HIDS, error code: 0x%x \n\r", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_service command: HIDS \n\r");
  }

  /**
   * INPUTREP
   */
  uuid.Char_UUID_16 = 0x2a4d;
  ret = aci_gatt_add_char(HIDS_Context.HidsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeInputrep,
                          CHAR_PROP_READ | CHAR_PROP_WRITE | CHAR_PROP_NOTIFY,
                          ATTR_PERMISSION_NONE,
                          GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(HIDS_Context.InputrepCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : INPUTREP, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : INPUTREP\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char1 */
  /* Place holder for Characteristic Descriptors */
  /**
   * INPUTREP REF descriptor
   */
  uuid.Char_UUID_16 = 0x2908;
  buf[0] = 0;
  buf[1] = 1; /* Input Report */
  /* add the valid descriptor */
  ret = aci_gatt_add_char_desc(HIDS_Context.HidsSvcHdle, 
                               HIDS_Context.InputrepCharHdle, 
                               UUID_TYPE_16,
                               (Char_Desc_Uuid_t *)&uuid, 
                               SizeRepRefDesc,
                               SizeRepRefDesc,
                               (void *)&buf,
                               ATTR_PERMISSION_NONE,
                               ATTR_ACCESS_READ_ONLY,
                               GATT_DONT_NOTIFY_EVENTS,
                               0x10, 
                               CHAR_VALUE_LEN_CONSTANT,
                               &(HIDS_Context.InputRepRefDescHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char_desc command   : INPUT REP REF DESC, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char_desc command   : INPUT REP REF DESC on handle 0x%x\n",
                 HIDS_Context.InputRepRefDescHdle);
  }
  /* USER CODE END SVCCTL_InitService1Char1 */

  /**
   * REM
   */
  uuid.Char_UUID_16 = 0x2a4b;
  ret = aci_gatt_add_char(HIDS_Context.HidsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeRem,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_NONE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(HIDS_Context.RemCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : REM, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : REM\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char2 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char2 */

  /**
   * HII
   */
  uuid.Char_UUID_16 = 0x2a4a;
  ret = aci_gatt_add_char(HIDS_Context.HidsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeHii,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_NONE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(HIDS_Context.HiiCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : HII, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : HII\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char3 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char3 */

  /**
   * HCP
   */
  uuid.Char_UUID_16 = 0x2a4c;
  ret = aci_gatt_add_char(HIDS_Context.HidsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeHcp,
                          CHAR_PROP_WRITE_WITHOUT_RESP,
                          ATTR_PERMISSION_NONE,
                          GATT_NOTIFY_ATTRIBUTE_WRITE,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(HIDS_Context.HcpCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : HCP, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : HCP\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char4 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char4 */

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
tBleStatus HIDS_UpdateValue(HIDS_CharOpcode_t CharOpcode, HIDS_Data_t *pData)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Service1_App_Update_Char_1 */

  /* USER CODE END Service1_App_Update_Char_1 */

  switch(CharOpcode)
  {
    case HIDS_INPUTREP:
      ret = aci_gatt_update_char_value(HIDS_Context.HidsSvcHdle,
                                       HIDS_Context.InputrepCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value INPUTREP command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value INPUTREP command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_1*/

      /* USER CODE END Service1_Char_Value_1*/
      break;

    case HIDS_REM:
      ret = aci_gatt_update_char_value(HIDS_Context.HidsSvcHdle,
                                       HIDS_Context.RemCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value REM command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value REM command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_2*/

      /* USER CODE END Service1_Char_Value_2*/
      break;

    case HIDS_HII:
      ret = aci_gatt_update_char_value(HIDS_Context.HidsSvcHdle,
                                       HIDS_Context.HiiCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value HII command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value HII command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_3*/

      /* USER CODE END Service1_Char_Value_3*/
      break;

    case HIDS_HCP:
      ret = aci_gatt_update_char_value(HIDS_Context.HidsSvcHdle,
                                       HIDS_Context.HcpCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value HCP command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value HCP command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_4*/

      /* USER CODE END Service1_Char_Value_4*/
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service1_App_Update_Char_2 */

  /* USER CODE END Service1_App_Update_Char_2 */

  return ret;
}
