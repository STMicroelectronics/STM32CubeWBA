/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    STM32_WPAN.c
  * @author  MCD Application Team
  * @brief   STM32_WPAN definition.
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
#include "ota.h"

/* USER CODE BEGIN Includes */
#include "ota_app.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */
typedef enum
{
  OTA_No_Pending    = 2,
  OTA_Pending       = 3,
  OTA_Ready_Pending = 4
} OTA_Confirmation_Status_t;
/* USER CODE END PTD */

typedef struct{
  uint16_t  OtaSvcHdle;                  /**< Ota Service Handle */
  uint16_t  Base_AdrCharHdle;                  /**< BASE_ADR Characteristic Handle */
  uint16_t  ConfCharHdle;                  /**< CONF Characteristic Handle */
  uint16_t  Raw_DataCharHdle;                  /**< RAW_DATA Characteristic Handle */
/* USER CODE BEGIN Context */
  /* Place holder for Characteristic Descriptors Handle*/

/* USER CODE END Context */
}OTA_Context_t;

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
static const uint16_t SizeBase_Adr = 6;
static const uint16_t SizeConf = 1;
static const uint16_t SizeRaw_Data = 248;

static OTA_Context_t OTA_Context;

/* USER CODE BEGIN PV */
static OTA_Confirmation_Status_t OTA_Conf_Status; 
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static SVCCTL_EvtAckStatus_t OTA_EventHandler(void *p_pckt);

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
 0000FE20CC7A482A984A7F2ED5B3E58F: Service 128bits UUID
 0000FE228E2245419D4C21EDAE82ED19: Characteristic 128bits UUID
 0000FE238E2245419D4C21EDAE82ED19: Characteristic 128bits UUID
 0000FE248E2245419D4C21EDAE82ED19: Characteristic 128bits UUID
 */
#define COPY_OTA_UUID(uuid_struct)       COPY_UUID_128(uuid_struct,0x00,0x00,0xfe,0x20,0xcc,0x7a,0x48,0x2a,0x98,0x4a,0x7f,0x2e,0xd5,0xb3,0xe5,0x8f)
#define COPY_BASE_ADR_UUID(uuid_struct)       COPY_UUID_128(uuid_struct,0x00,0x00,0xfe,0x22,0x8e,0x22,0x45,0x41,0x9d,0x4c,0x21,0xed,0xae,0x82,0xed,0x19)
#define COPY_CONF_UUID(uuid_struct)       COPY_UUID_128(uuid_struct,0x00,0x00,0xfe,0x23,0x8e,0x22,0x45,0x41,0x9d,0x4c,0x21,0xed,0xae,0x82,0xed,0x19)
#define COPY_RAW_DATA_UUID(uuid_struct)       COPY_UUID_128(uuid_struct,0x00,0x00,0xfe,0x24,0x8e,0x22,0x45,0x41,0x9d,0x4c,0x21,0xed,0xae,0x82,0xed,0x19)

/* USER CODE BEGIN PF */

/* USER CODE END PF */

/**
 * @brief  Event handler
 * @param  p_Event: Address of the buffer holding the p_Event
 * @retval Ack: Return whether the p_Event has been managed or not
 */
static SVCCTL_EvtAckStatus_t OTA_EventHandler(void *p_Event)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *p_event_pckt;
  evt_blecore_aci *p_blecore_evt;
  aci_gatt_attribute_modified_event_rp0 *p_attribute_modified;
  aci_gatt_write_permit_req_event_rp0   *p_write_perm_req;
  OTA_NotificationEvt_t                 notification;
  /* USER CODE BEGIN Service2_EventHandler_1 */
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  aci_gatt_indication_event_rp0 *pr;
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
          if(p_attribute_modified->Attr_Handle == (OTA_Context.ConfCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service2_Char_2 */

            /* USER CODE END Service2_Char_2 */

            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service2_Char_2_attribute_modified */

              /* USER CODE END Service2_Char_2_attribute_modified */

              /* Disabled Indication management */
              case (!(COMSVC_Indication)):
                /* USER CODE BEGIN Service2_Char_2_Disabled_BEGIN */

                /* USER CODE END Service2_Char_2_Disabled_BEGIN */
                notification.EvtOpcode = OTA_CONF_INDICATE_DISABLED_EVT;
                OTA_Notification(&notification);
                /* USER CODE BEGIN Service2_Char_2_Disabled_END */

                /* USER CODE END Service2_Char_2_Disabled_END */
                break;

              /* Enabled Indication management */
              case COMSVC_Indication:
                /* USER CODE BEGIN Service2_Char_2_COMSVC_Indication_BEGIN */

                /* USER CODE END Service2_Char_2_COMSVC_Indication_BEGIN */
                notification.EvtOpcode = OTA_CONF_INDICATE_ENABLED_EVT;
                OTA_Notification(&notification);
                /* USER CODE BEGIN Service2_Char_2_COMSVC_Indication_END */

                /* USER CODE END Service2_Char_2_COMSVC_Indication_END */
                break;

              default:
                /* USER CODE BEGIN Service2_Char_2_default */

                /* USER CODE END Service2_Char_2_default */
                break;
            }
          }  /* if(p_attribute_modified->Attr_Handle == (OTA_Context.CONFHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

          else if(p_attribute_modified->Attr_Handle == (OTA_Context.Base_AdrCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;

            notification.EvtOpcode = OTA_BASE_ADR_WRITE_NO_RESP_EVT;
            /* USER CODE BEGIN Service2_Char_1_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
            /**
             * Base Address
             */
            notification.DataTransfered.p_Payload = (uint8_t*)&p_attribute_modified->Attr_Data[0];
            notification.DataTransfered.Length = p_attribute_modified->Attr_Data_Length;
            /* USER CODE END Service2_Char_1_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
            OTA_Notification(&notification);
          } /* if(p_attribute_modified->Attr_Handle == (OTA_Context.Base_AdrCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/
          else if(p_attribute_modified->Attr_Handle == (OTA_Context.Raw_DataCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;

            notification.EvtOpcode = OTA_RAW_DATA_WRITE_NO_RESP_EVT;
            /* USER CODE BEGIN Service2_Char_3_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
            /**
             * Raw Data
             */
            notification.DataTransfered.p_Payload = (uint8_t*)&p_attribute_modified->Attr_Data[0];
            notification.DataTransfered.Length = p_attribute_modified->Attr_Data_Length;
            /* USER CODE END Service2_Char_3_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
            OTA_Notification(&notification);
          } /* if(p_attribute_modified->Attr_Handle == (OTA_Context.Raw_DataCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

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
          if(p_write_perm_req->Attribute_Handle == (OTA_Context.Raw_DataCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service2_Char_3_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
            ret = aci_gatt_write_resp(p_write_perm_req->Connection_Handle,
                                      p_write_perm_req->Attribute_Handle,
                                      0x00, /* write_status = 0 (no error))*/
                                      0x00, /* err_code */
                                      p_write_perm_req->Data_Length,
                                     (uint8_t *)&(p_write_perm_req->Data[0]));
            if(ret != BLE_STATUS_SUCCESS)
            {
              LOG_INFO_APP("==>> ACI GATT Write response Failed , result: %d \n\r", ret);
            }
            /*USER CODE END Service2_Char_3_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE*/
          } /*if(p_write_perm_req->Attribute_Handle == (OTA_Context.Raw_DataCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

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
        case ACI_GATT_SERVER_CONFIRMATION_VSEVT_CODE:
          p_attribute_modified = (aci_gatt_attribute_modified_event_rp0*)p_blecore_evt->data;
          OTA_Conf_Status = (OTA_Confirmation_Status_t)OTA_APP_GetConfStatus();
          if( OTA_Conf_Status == OTA_Pending)
          {
            /**
             * Confirmation Event
             */
            OTA_Conf_Status = OTA_No_Pending;

            return_value = SVCCTL_EvtAckFlowEnable;

            notification.EvtOpcode = OTA_CONF_EVT;
            notification.DataTransfered.p_Payload = (uint8_t*)&p_attribute_modified->Attr_Data[0];
            notification.DataTransfered.Length = p_attribute_modified->Attr_Data_Length;
            OTA_Notification( &notification );
          }
          else if( OTA_Conf_Status == OTA_Ready_Pending)
          {
            /**
             * Confirmation Event
             */
            OTA_Conf_Status = OTA_No_Pending;

            return_value = SVCCTL_EvtAckFlowEnable;

            notification.EvtOpcode = OTA_READY_EVT;
            notification.DataTransfered.p_Payload = (uint8_t*)&p_attribute_modified->Attr_Data[0];
            notification.DataTransfered.Length = p_attribute_modified->Attr_Data_Length;
            OTA_Notification( &notification );
          }
          break;
      
        /* Manage ACI_GATT_INDICATION_VSEVT_CODE occurring on Android 12 */   
        case ACI_GATT_INDICATION_VSEVT_CODE:
          pr = (void*)p_blecore_evt->data;
          ret = aci_gatt_confirm_indication(pr->Connection_Handle);
          if (ret != BLE_STATUS_SUCCESS)
          {
            LOG_INFO_APP("==>> ACI GATT Confirmation indication Failed , result: %d \n\r", ret);
          }
          break; /* end ACI_GATT_NOTIFICATION_VSEVT_CODE */
          
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
}/* end OTA_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void OTA_Init(void)
{
  Char_UUID_t  uuid;
  tBleStatus ret;
  uint8_t max_attr_record;

  /* USER CODE BEGIN SVCCTL_InitService2Svc_1 */

  /* USER CODE END SVCCTL_InitService2Svc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  SVCCTL_RegisterSvcHandler(OTA_EventHandler);

  /**
   * OTA
   *
   * Max_Attribute_Records = 1 + 2*3 + 1*no_of_char_with_notify_or_indicate_property + 1*no_of_char_with_broadcast_property
   * service_max_attribute_record = 1 for OTA +
   *                                2 for BASE_ADR +
   *                                2 for CONF +
   *                                2 for RAW_DATA +
   *                                1 for CONF configuration descriptor +
   *                              = 8
   * This value doesn't take into account number of descriptors manually added
   * In case of descriptors added, please update the max_attr_record value accordingly in the next SVCCTL_InitService User Section
   */
  max_attr_record = 8;

  /* USER CODE BEGIN SVCCTL_InitService */
  /* max_attr_record to be updated if descriptors have been added */

  /* USER CODE END SVCCTL_InitService */

  COPY_OTA_UUID(uuid.Char_UUID_128);
  ret = aci_gatt_add_service(UUID_TYPE_128,
                             (Service_UUID_t *) &uuid,
                             PRIMARY_SERVICE,
                             max_attr_record,
                             &(OTA_Context.OtaSvcHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_service command: OTA, error code: 0x%x \n\r", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_service command: OTA \n\r");
  }

  /**
   * BASE_ADR
   */
  COPY_BASE_ADR_UUID(uuid.Char_UUID_128);
  ret = aci_gatt_add_char(OTA_Context.OtaSvcHdle,
                          UUID_TYPE_128,
                          (Char_UUID_t *) &uuid,
                          SizeBase_Adr,
                          CHAR_PROP_WRITE_WITHOUT_RESP,
                          ATTR_PERMISSION_NONE,
                          GATT_NOTIFY_ATTRIBUTE_WRITE,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(OTA_Context.Base_AdrCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : BASE_ADR, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : BASE_ADR\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char1 */

  /* USER CODE END SVCCTL_InitService2Char1 */

  /**
   * CONF
   */
  COPY_CONF_UUID(uuid.Char_UUID_128);
  ret = aci_gatt_add_char(OTA_Context.OtaSvcHdle,
                          UUID_TYPE_128,
                          (Char_UUID_t *) &uuid,
                          SizeConf,
                          CHAR_PROP_INDICATE,
                          ATTR_PERMISSION_NONE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(OTA_Context.ConfCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : CONF, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : CONF\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char2 */

  /* USER CODE END SVCCTL_InitService2Char2 */

  /**
   * RAW_DATA
   */
  COPY_RAW_DATA_UUID(uuid.Char_UUID_128);
  ret = aci_gatt_add_char(OTA_Context.OtaSvcHdle,
                          UUID_TYPE_128,
                          (Char_UUID_t *) &uuid,
                          SizeRaw_Data,
                          CHAR_PROP_WRITE_WITHOUT_RESP,
                          ATTR_PERMISSION_NONE,
                          GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(OTA_Context.Raw_DataCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : RAW_DATA, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : RAW_DATA\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char3 */

  /* USER CODE END SVCCTL_InitService2Char3 */

  /* USER CODE BEGIN SVCCTL_InitService2Svc_2 */
  OTA_Conf_Status = OTA_No_Pending;
  /* USER CODE END SVCCTL_InitService2Svc_2 */

  return;
}

/**
 * @brief  Characteristic update
 * @param  CharOpcode: Characteristic identifier
 * @param  pData: Structure holding data to update
 *
 */
tBleStatus OTA_UpdateValue(OTA_CharOpcode_t CharOpcode, OTA_Data_t *pData)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Service2_App_Update_Char_1 */

  /* USER CODE END Service2_App_Update_Char_1 */

  switch(CharOpcode)
  {
    case OTA_BASE_ADR:
      ret = aci_gatt_update_char_value(OTA_Context.OtaSvcHdle,
                                       OTA_Context.Base_AdrCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value BASE_ADR command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value BASE_ADR command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_1*/

      /* USER CODE END Service2_Char_Value_1*/
      break;

    case OTA_CONF:
      ret = aci_gatt_update_char_value(OTA_Context.OtaSvcHdle,
                                       OTA_Context.ConfCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value CONF command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value CONF command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_2*/

      /* USER CODE END Service2_Char_Value_2*/
      break;

    case OTA_RAW_DATA:
      ret = aci_gatt_update_char_value(OTA_Context.OtaSvcHdle,
                                       OTA_Context.Raw_DataCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value RAW_DATA command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value RAW_DATA command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_3*/

      /* USER CODE END Service2_Char_Value_3*/
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service2_App_Update_Char_2 */

  /* USER CODE END Service2_App_Update_Char_2 */

  return ret;
}
