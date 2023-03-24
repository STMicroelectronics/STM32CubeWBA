/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service2.c
  * @author  MCD Application Team
  * @brief   service2 definition.
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
#include "common_blesvc.h"
#include "hts.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef struct{
  uint16_t  HtsSvcHdle;                  /**< Hts Service Handle */
  uint16_t  TemmCharHdle;                  /**< TEMM Characteristic Handle */
  uint16_t  MnbsCharHdle;                  /**< MNBS Characteristic Handle */
  uint16_t  IntCharHdle;                  /**< INT Characteristic Handle */
  uint16_t  MeiCharHdle;                  /**< MEI Characteristic Handle */
}HTS_Context_t;

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
#define INTERVAL_OUT_OF_RANGE       (0x80)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static const uint8_t SizeTemm = 13;
static const uint8_t SizeMnbs = 1;
static const uint8_t SizeInt = 13;
static const uint8_t SizeMei = 2;

static HTS_Context_t HTS_Context;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static SVCCTL_EvtAckStatus_t HTS_EventHandler(void *p_pckt);

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

/* Hardware Characteristics Service */
/*
 The following 128bits UUIDs have been generated from the random UUID
 generator:
 D973F2E0-B19E-11E2-9E96-0800200C9A66: Service 128bits UUID
 D973F2E1-B19E-11E2-9E96-0800200C9A66: Characteristic_1 128bits UUID
 D973F2E2-B19E-11E2-9E96-0800200C9A66: Characteristic_2 128bits UUID
 */

/* USER CODE BEGIN PF */

/* USER CODE END PF */

/**
 * @brief  Event handler
 * @param  p_Event: Address of the buffer holding the p_Event
 * @retval Ack: Return whether the p_Event has been managed or not
 */
static SVCCTL_EvtAckStatus_t HTS_EventHandler(void *p_Event)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *p_event_pckt;
  evt_blecore_aci *p_blecore_evt;
  aci_gatt_attribute_modified_event_rp0 *p_attribute_modified;
  aci_gatt_write_permit_req_event_rp0   *p_write_perm_req;
  HTS_NotificationEvt_t                 notification;
  /* USER CODE BEGIN Service2_EventHandler_1 */

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
          /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_BEGIN */

          /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_BEGIN */
          p_attribute_modified = (aci_gatt_attribute_modified_event_rp0*)p_blecore_evt->data;
          if(p_attribute_modified->Attr_Handle == (HTS_Context.TemmCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service2_Char_1 */

            /* USER CODE END Service2_Char_1 */

            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service2_Char_1_attribute_modified */

              /* USER CODE END Service2_Char_1_attribute_modified */

              /* Disabled Indication management */
              case (!(COMSVC_Indication)):
                /* USER CODE BEGIN Service2_Char_1_Disabled_BEGIN */
                APP_DBG_MSG("ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE HTS_TEMM_INDICATE_DISABLED_EVT\n");
                /* USER CODE END Service2_Char_1_Disabled_BEGIN */
                notification.EvtOpcode = HTS_TEMM_INDICATE_DISABLED_EVT;
                HTS_Notification(&notification);
                /* USER CODE BEGIN Service2_Char_1_Disabled_END */

                /* USER CODE END Service2_Char_1_Disabled_END */
                break;

              /* Enabled Indication management */
              case COMSVC_Indication:
                /* USER CODE BEGIN Service2_Char_1_COMSVC_Indication_BEGIN */
                APP_DBG_MSG("ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE HTS_TEMM_INDICATE_ENABLED_EVT\n");
                /* USER CODE END Service2_Char_1_COMSVC_Indication_BEGIN */
                notification.EvtOpcode = HTS_TEMM_INDICATE_ENABLED_EVT;
                HTS_Notification(&notification);
                /* USER CODE BEGIN Service2_Char_1_COMSVC_Indication_END */

                /* USER CODE END Service2_Char_1_COMSVC_Indication_END */
                break;

              default:
                /* USER CODE BEGIN Service2_Char_1_default */

                /* USER CODE END Service2_Char_1_default */
                break;
            }
          }  /* if(p_attribute_modified->Attr_Handle == (HTS_Context.TEMMHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

          else if(p_attribute_modified->Attr_Handle == (HTS_Context.IntCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service2_Char_3 */

            /* USER CODE END Service2_Char_3 */
            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service2_Char_3_attribute_modified */

              /* USER CODE END Service2_Char_3_attribute_modified */

              /* Disabled Notification management */
              case (!(COMSVC_Notification)):
                /* USER CODE BEGIN Service2_Char_3_Disabled_BEGIN */
                APP_DBG_MSG("ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE HTS_INT_NOTIFY_DISABLED_EVT\n");
                /* USER CODE END Service2_Char_3_Disabled_BEGIN */
                notification.EvtOpcode = HTS_INT_NOTIFY_DISABLED_EVT;
                HTS_Notification(&notification);
                /* USER CODE BEGIN Service2_Char_3_Disabled_END */

                /* USER CODE END Service2_Char_3_Disabled_END */
                break;

              /* Enabled Notification management */
              case COMSVC_Notification:
                /* USER CODE BEGIN Service2_Char_3_COMSVC_Notification_BEGIN */
                APP_DBG_MSG("ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE HTS_INT_NOTIFY_ENABLED_EVT\n");                
                /* USER CODE END Service2_Char_3_COMSVC_Notification_BEGIN */
                notification.EvtOpcode = HTS_INT_NOTIFY_ENABLED_EVT;
                HTS_Notification(&notification);
                /* USER CODE BEGIN Service2_Char_3_COMSVC_Notification_END */

                /* USER CODE END Service2_Char_3_COMSVC_Notification_END */
                break;

              default:
                /* USER CODE BEGIN Service2_Char_3_default */

                /* USER CODE END Service2_Char_3_default */
                break;
            }
          }  /* if(p_attribute_modified->Attr_Handle == (HTS_Context.IntCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

          else if(p_attribute_modified->Attr_Handle == (HTS_Context.MeiCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service2_Char_4 */

            /* USER CODE END Service2_Char_4 */

            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service2_Char_4_attribute_modified */

              /* USER CODE END Service2_Char_4_attribute_modified */

              /* Disabled Indication management */
              case (!(COMSVC_Indication)):
                /* USER CODE BEGIN Service2_Char_4_Disabled_BEGIN */
                APP_DBG_MSG("ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE HTS_MEI_INDICATE_DISABLED_EVT\n");
                /* USER CODE END Service2_Char_4_Disabled_BEGIN */
                notification.EvtOpcode = HTS_MEI_INDICATE_DISABLED_EVT;
                HTS_Notification(&notification);
                /* USER CODE BEGIN Service2_Char_4_Disabled_END */

                /* USER CODE END Service2_Char_4_Disabled_END */
                break;

              /* Enabled Indication management */
              case COMSVC_Indication:
                /* USER CODE BEGIN Service2_Char_4_COMSVC_Indication_BEGIN */
                APP_DBG_MSG("ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE HTS_MEI_INDICATE_ENABLED_EVT\n");
                /* USER CODE END Service2_Char_4_COMSVC_Indication_BEGIN */
                notification.EvtOpcode = HTS_MEI_INDICATE_ENABLED_EVT;
                HTS_Notification(&notification);
                /* USER CODE BEGIN Service2_Char_4_COMSVC_Indication_END */

                /* USER CODE END Service2_Char_4_COMSVC_Indication_END */
                break;

              default:
                /* USER CODE BEGIN Service2_Char_4_default */

                /* USER CODE END Service2_Char_4_default */
                break;
            }
          }  /* if(p_attribute_modified->Attr_Handle == (HTS_Context.MEIHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

          else if(p_attribute_modified->Attr_Handle == (HTS_Context.MeiCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service2_Char_4_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */

            /* USER CODE END Service2_Char_4_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
            notification.EvtOpcode = HTS_MEI_WRITE_EVT;
            HTS_Notification(&notification);
          } /* if(p_attribute_modified->Attr_Handle == (HTS_Context.MeiCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

          /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */

          /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */
          break;

        case ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE :
          /* USER CODE BEGIN EVT_BLUE_GATT_READ_PERMIT_REQ_BEGIN */

          /* USER CODE END EVT_BLUE_GATT_READ_PERMIT_REQ_BEGIN */

          /* USER CODE BEGIN EVT_BLUE_GATT_READ_PERMIT_REQ_END */

          /* USER CODE END EVT_BLUE_GATT_READ_PERMIT_REQ_END */
          break;

        case ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE:
          /* USER CODE BEGIN EVT_BLUE_GATT_WRITE_PERMIT_REQ_BEGIN */

          /* USER CODE END EVT_BLUE_GATT_WRITE_PERMIT_REQ_BEGIN */
          p_write_perm_req = (aci_gatt_write_permit_req_event_rp0*)p_blecore_evt->data;
          if(p_write_perm_req->Attribute_Handle == (HTS_Context.MeiCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service2_Char_4_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
            uint32_t validrangevalue = p_write_perm_req->Data[0] + (p_write_perm_req->Data[1] << 8);
            
            if ((validrangevalue == 0) ||
                ((validrangevalue >= BLE_CFG_HTS_TEMPERATURE_INTERVAL_MIN_VALUE) &&
                 (validrangevalue <= BLE_CFG_HTS_TEMPERATURE_INTERVAL_MAX_VALUE)))
            {
              /**
              * received a correct value
              */
              
              aci_gatt_write_resp(p_write_perm_req->Connection_Handle,
                                  p_write_perm_req->Attribute_Handle,
                                  0x00, /* write_status = 0 (no error))*/
                                  0x00, /* err_code */
                                  p_write_perm_req->Data_Length,
                                  (uint8_t *)&p_write_perm_req->Data[0]);
              
              /**
              * Notify to the application a new interval has been received
              */
              notification.RangeInterval = (uint16_t)validrangevalue;
              notification.EvtOpcode = HTS_MEI_READ_EVT;
              HTS_Notification(&notification);
            }
            else
            {
              /**
              * received a value out of range
              */
              aci_gatt_write_resp(p_write_perm_req->Connection_Handle,
                                  p_write_perm_req->Attribute_Handle,
                                  0x1, /* write_status = 1 (error))*/
                                  INTERVAL_OUT_OF_RANGE, /* err_code */
                                  p_write_perm_req->Data_Length,
                                  (uint8_t *)&p_write_perm_req->Data[0]);
            }
            /*USER CODE END Service2_Char_4_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE*/
          } /*if(p_write_perm_req->Attribute_Handle == (HTS_Context.MeiCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

          /* USER CODE BEGIN EVT_BLUE_GATT_WRITE_PERMIT_REQ_END */

          /* USER CODE END EVT_BLUE_GATT_WRITE_PERMIT_REQ_END */
          break;
        case ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE:
        {
          aci_gatt_tx_pool_available_event_rp0 *p_tx_pool_available_event;
          p_tx_pool_available_event = (aci_gatt_tx_pool_available_event_rp0 *) p_blecore_evt->data;
          UNUSED(p_tx_pool_available_event);

          /* USER CODE BEGIN ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */

          /* USER CODE END ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */
        }
          break;/* ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE*/
        case ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE:
        {
          aci_att_exchange_mtu_resp_event_rp0 *p_exchange_mtu;
          p_exchange_mtu = (aci_att_exchange_mtu_resp_event_rp0 *)  p_blecore_evt->data;
          UNUSED(p_exchange_mtu);

          /* USER CODE BEGIN ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */

          /* USER CODE END ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */

        }
          break;
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

  /* USER CODE BEGIN Service2_EventHandler_2 */

  /* USER CODE END Service2_EventHandler_2 */

  return(return_value);
}/* end HTS_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void HTS_Init(void)
{
  Char_UUID_t  uuid;
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN SVCCTL_InitService2Svc_1 */

  /* USER CODE END SVCCTL_InitService2Svc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  SVCCTL_RegisterSvcHandler(HTS_EventHandler);

  /**
   * HTS
   *
   * Max_Attribute_Records = 1 + 2*4 + 1*no_of_char_with_notify_or_indicate_property + 1*no_of_char_with_broadcast_property
   * service_max_attribute_record = 1 for HTS +
   *                                2 for TEMM +
   *                                2 for MNBS +
   *                                2 for INT +
   *                                2 for MEI +
   *                                1 for TEMM configuration descriptor +
   *                                1 for INT configuration descriptor +
   *                                1 for MEI configuration descriptor +
   *                              = 12
   */
  uuid.Char_UUID_16 = 0x1809;
  ret = aci_gatt_add_service(UUID_TYPE_16,
                             (Service_UUID_t *) &uuid,
                             PRIMARY_SERVICE,
                             12,
                             &(HTS_Context.HtsSvcHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_gatt_add_service command: HTS, error code: 0x%x \n\r", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: aci_gatt_add_service command: HTS \n\r");
  }

  /**
   * TEMM
   */
  uuid.Char_UUID_16 = 0x2a1c;
  ret = aci_gatt_add_char(HTS_Context.HtsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeTemm,
                          CHAR_PROP_INDICATE,
                          ATTR_PERMISSION_NONE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(HTS_Context.TemmCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_gatt_add_char command   : TEMM, error code: 0x%2X\n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: aci_gatt_add_char command   : TEMM\n");
  }

  /**
   * MNBS
   */
  uuid.Char_UUID_16 = 0x2a1d;
  ret = aci_gatt_add_char(HTS_Context.HtsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeMnbs,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_NONE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(HTS_Context.MnbsCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_gatt_add_char command   : MNBS, error code: 0x%2X\n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: aci_gatt_add_char command   : MNBS\n");
  }

  /**
   * INT
   */
  uuid.Char_UUID_16 = 0x2a1e;
  ret = aci_gatt_add_char(HTS_Context.HtsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeInt,
                          CHAR_PROP_NOTIFY,
                          ATTR_PERMISSION_NONE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(HTS_Context.IntCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_gatt_add_char command   : INT, error code: 0x%2X\n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: aci_gatt_add_char command   : INT\n");
  }

  /**
   * MEI
   */
  uuid.Char_UUID_16 = 0x2a21;
  ret = aci_gatt_add_char(HTS_Context.HtsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeMei,
                          CHAR_PROP_READ | CHAR_PROP_WRITE | CHAR_PROP_INDICATE,
                          ATTR_PERMISSION_NONE,
                          GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(HTS_Context.MeiCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_gatt_add_char command   : MEI, error code: 0x%2X\n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: aci_gatt_add_char command   : MEI\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService2Svc_2 */

  /* USER CODE END SVCCTL_InitService2Svc_2 */

  return;
}

/**
 * @brief  Characteristic update
 * @param  CharOpcode: Characteristic identifier
 * @param  Service_Instance: Instance of the service to which the characteristic belongs
 *
 */
tBleStatus HTS_UpdateValue(HTS_CharOpcode_t CharOpcode, HTS_Data_t *pData)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Service2_App_Update_Char_1 */

  /* USER CODE END Service2_App_Update_Char_1 */

  switch(CharOpcode)
  {
    case HTS_TEMM:
      ret = aci_gatt_update_char_value(HTS_Context.HtsSvcHdle,
                                       HTS_Context.TemmCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("  Fail   : aci_gatt_update_char_value TEMM command, error code: 0x%2X\n", ret);
      }
      else
      {
        APP_DBG_MSG("  Success: aci_gatt_update_char_value TEMM command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_1*/

      /* USER CODE END Service2_Char_Value_1*/
      break;

    case HTS_MNBS:
      ret = aci_gatt_update_char_value(HTS_Context.HtsSvcHdle,
                                       HTS_Context.MnbsCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("  Fail   : aci_gatt_update_char_value MNBS command, error code: 0x%2X\n", ret);
      }
      else
      {
        APP_DBG_MSG("  Success: aci_gatt_update_char_value MNBS command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_2*/

      /* USER CODE END Service2_Char_Value_2*/
      break;

    case HTS_INT:
      ret = aci_gatt_update_char_value(HTS_Context.HtsSvcHdle,
                                       HTS_Context.IntCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("  Fail   : aci_gatt_update_char_value INT command, error code: 0x%2X\n", ret);
      }
      else
      {
        APP_DBG_MSG("  Success: aci_gatt_update_char_value INT command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_3*/

      /* USER CODE END Service2_Char_Value_3*/
      break;

    case HTS_MEI:
      ret = aci_gatt_update_char_value(HTS_Context.HtsSvcHdle,
                                       HTS_Context.MeiCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("  Fail   : aci_gatt_update_char_value MEI command, error code: 0x%2X\n", ret);
      }
      else
      {
        APP_DBG_MSG("  Success: aci_gatt_update_char_value MEI command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_4*/

      /* USER CODE END Service2_Char_Value_4*/
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service2_App_Update_Char_2 */

  /* USER CODE END Service2_App_Update_Char_2 */

  return ret;
}
