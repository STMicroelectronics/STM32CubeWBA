/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service1.c
  * @author  MCD Application Team
  * @brief   service1 definition.
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
#include "ghs.h"

/* USER CODE BEGIN Includes */
#include "stm32_seq.h"
#include "app_conf.h"
#include "ghs_app.h"
#include "ghs_cp.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef struct{
  uint16_t  GhsSvcHdle;                  /**< Ghs Service Handle */
  uint16_t  HsfCharHdle;                  /**< HSF Characteristic Handle */
  uint16_t  LhoCharHdle;                  /**< LHO Characteristic Handle */
  uint16_t  GhscpCharHdle;                  /**< GHSCP Characteristic Handle */
  uint16_t  OscCharHdle;                  /**< OSC Characteristic Handle */
/* USER CODE BEGIN Context */
  /* Place holder for Characteristic Descriptors Handle*/
  uint16_t OscDescHdle[NB_SUPPORTED_OBSERVATION_TYPES];              /**< OSC Descriptor Handle */
  uint16_t VraDescHdle[NB_SUPPORTED_OBSERVATION_TYPES];              /**< VRA Descriptor Handle */
/* USER CODE END Context */
}GHS_Context_t;

/* Private defines -----------------------------------------------------------*/
#define UUID_128_SUPPORTED  1

#if (UUID_128_SUPPORTED == 1)
#define BM_UUID_LENGTH  UUID_TYPE_128
#else
#define BM_UUID_LENGTH  UUID_TYPE_16
#endif

#define BM_REQ_CHAR_SIZE    (3)

/* USER CODE BEGIN PD */
#define UNSUPPORTED_MEASUREMENT_PERIOD                              (0x0FFFFFFF)
#define UNSUPPORTED_UPDATE_INTERVAL                                 (0x0FFFFFFF)
/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */
extern GHS_OSC_t ObservationSchedule[2];
extern GHS_ValidRangeAccuracy_t ValidRangeAccuracy[2];
/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
#define CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET         2
#define CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET              1
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static const uint16_t SizeHsf = 20;
static const uint16_t SizeLho = 80;
static const uint16_t SizeGhscp = 1;
static const uint16_t SizeOsc = 12;

static GHS_Context_t GHS_Context;

/* USER CODE BEGIN PV */
static const uint8_t SizeOscDesc = 12;
static const uint8_t SizeVraDesc = 18;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static SVCCTL_EvtAckStatus_t GHS_EventHandler(void *p_pckt);

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
static SVCCTL_EvtAckStatus_t GHS_EventHandler(void *p_Event)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *p_event_pckt;
  evt_blecore_aci *p_blecore_evt;
  aci_gatt_attribute_modified_event_rp0 *p_attribute_modified;
  aci_gatt_write_permit_req_event_rp0   *p_write_perm_req;
  GHS_NotificationEvt_t                 notification;
  /* USER CODE BEGIN Service1_EventHandler_1 */
  uint8_t error_code = 0;
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
          if(p_attribute_modified->Attr_Handle == (GHS_Context.HsfCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service1_Char_1 */

            /* USER CODE END Service1_Char_1 */

            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service1_Char_1_attribute_modified */

              /* USER CODE END Service1_Char_1_attribute_modified */

              /* Disabled Indication management */
              case (0x00):
                /* USER CODE BEGIN Service1_Char_1_Disabled_BEGIN */

                /* USER CODE END Service1_Char_1_Disabled_BEGIN */
                notification.EvtOpcode = GHS_HSF_INDICATE_DISABLED_EVT;
                GHS_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_1_Disabled_END */

                /* USER CODE END Service1_Char_1_Disabled_END */
                break;

              /* Enabled Indication management */
              case GATT_CHAR_UPDATE_SEND_INDICATION:
                /* USER CODE BEGIN Service1_Char_1_COMSVC_Indication_BEGIN */

                /* USER CODE END Service1_Char_1_COMSVC_Indication_BEGIN */
                notification.EvtOpcode = GHS_HSF_INDICATE_ENABLED_EVT;
                GHS_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_1_COMSVC_Indication_END */

                /* USER CODE END Service1_Char_1_COMSVC_Indication_END */
                break;

              default:
                /* USER CODE BEGIN Service1_Char_1_default */

                /* USER CODE END Service1_Char_1_default */
                break;
            }
          }

          else if(p_attribute_modified->Attr_Handle == (GHS_Context.LhoCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service1_Char_2 */

            /* USER CODE END Service1_Char_2 */

            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service1_Char_2_attribute_modified  */

              /* USER CODE END Service1_Char_2_attribute_modified  */

              /* Disabled Notification and Indication management */
              case (0x00):
                /* USER CODE BEGIN Service1_Char_2_Disabled_BEGIN  */

                /* USER CODE END Service1_Char_2_Disabled_BEGIN  */
                notification.EvtOpcode = GHS_LHO_NOTIFY_DISABLED_EVT;
                GHS_Notification(&notification);
                notification.EvtOpcode = GHS_LHO_INDICATE_DISABLED_EVT;
                GHS_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_2_Disabled_END */

                /* USER CODE END Service1_Char_2_Disabled_END */
                break;

              /* Enabled Notification management */
              case GATT_CHAR_UPDATE_SEND_NOTIFICATION:
                /* USER CODE BEGIN Service1_Char_2_COMSVC_Notification_BEGIN */

                /* USER CODE END Service1_Char_2_COMSVC_Notification_BEGIN */
                notification.EvtOpcode = GHS_LHO_NOTIFY_ENABLED_EVT;
                GHS_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_2_COMSVC_Notification_END */

                /* USER CODE END Service1_Char_2_COMSVC_Notification_END */
                break;

              /* Enabled Indication management */
              case GATT_CHAR_UPDATE_SEND_INDICATION:
                /* USER CODE BEGIN Service1_Char_2_COMSVC_Indication_BEGIN */

                /* USER CODE END Service1_Char_2_COMSVC_Indication_BEGIN */
                notification.EvtOpcode = GHS_LHO_INDICATE_ENABLED_EVT;
                GHS_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_2_COMSVC_Indication_END */

                /* USER CODE END Service1_Char_2_COMSVC_Indication_END */
                break;

              default:
                /* USER CODE BEGIN Service1_Char_2_default */

                /* USER CODE END Service1_Char_2_default */
                break;
            }
          }

          else if(p_attribute_modified->Attr_Handle == (GHS_Context.GhscpCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service1_Char_3 */
            LOG_INFO_APP("p_attribute_modified->Attr_Data[0] (0x%02X)\r\n", p_attribute_modified->Attr_Data[0]);
            /* USER CODE END Service1_Char_3 */

            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service1_Char_3_attribute_modified */

              /* USER CODE END Service1_Char_3_attribute_modified */

              /* Disabled Indication management */
              case (0x00):
                /* USER CODE BEGIN Service1_Char_3_Disabled_BEGIN */

                /* USER CODE END Service1_Char_3_Disabled_BEGIN */
                notification.EvtOpcode = GHS_GHSCP_INDICATE_DISABLED_EVT;
                GHS_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_3_Disabled_END */

                /* USER CODE END Service1_Char_3_Disabled_END */
                break;

              /* Enabled Indication management */
              case GATT_CHAR_UPDATE_SEND_INDICATION:
                /* USER CODE BEGIN Service1_Char_3_COMSVC_Indication_BEGIN */

                /* USER CODE END Service1_Char_3_COMSVC_Indication_BEGIN */
                notification.EvtOpcode = GHS_GHSCP_INDICATE_ENABLED_EVT;
                GHS_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_3_COMSVC_Indication_END */

                /* USER CODE END Service1_Char_3_COMSVC_Indication_END */
                break;

              default:
                /* USER CODE BEGIN Service1_Char_3_default */

                /* USER CODE END Service1_Char_3_default */
                break;
            }
          }

          else if(p_attribute_modified->Attr_Handle == (GHS_Context.OscCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service1_Char_4 */

            /* USER CODE END Service1_Char_4 */

            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service1_Char_4_attribute_modified */

              /* USER CODE END Service1_Char_4_attribute_modified */

              /* Disabled Indication management */
              case (0x00):
                /* USER CODE BEGIN Service1_Char_4_Disabled_BEGIN */

                /* USER CODE END Service1_Char_4_Disabled_BEGIN */
                notification.EvtOpcode = GHS_OSC_INDICATE_DISABLED_EVT;
                GHS_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_4_Disabled_END */

                /* USER CODE END Service1_Char_4_Disabled_END */
                break;

              /* Enabled Indication management */
              case GATT_CHAR_UPDATE_SEND_INDICATION:
                /* USER CODE BEGIN Service1_Char_4_COMSVC_Indication_BEGIN */

                /* USER CODE END Service1_Char_4_COMSVC_Indication_BEGIN */
                notification.EvtOpcode = GHS_OSC_INDICATE_ENABLED_EVT;
                GHS_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_4_COMSVC_Indication_END */

                /* USER CODE END Service1_Char_4_COMSVC_Indication_END */
                break;

              default:
                /* USER CODE BEGIN Service1_Char_4_default */

                /* USER CODE END Service1_Char_4_default */
                break;
            }
          }

          else if(p_attribute_modified->Attr_Handle == (GHS_Context.GhscpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;

            notification.EvtOpcode = GHS_GHSCP_WRITE_EVT;
            /* USER CODE BEGIN Service1_Char_3_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */

            /* USER CODE END Service1_Char_3_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
            GHS_Notification(&notification);
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
          if(p_write_perm_req->Attribute_Handle == (GHS_Context.GhscpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service1_Char_3_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
            LOG_INFO_APP("ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE GHS_CP, data length: %d\r\n", p_write_perm_req->Data_Length);
        
            error_code = GHS_CP_CheckRequestValid(p_write_perm_req->Data, p_write_perm_req->Data_Length);
            if (error_code == 0x00)
            {
              aci_gatt_permit_write(p_write_perm_req->Connection_Handle,
                                    p_write_perm_req->Attribute_Handle,
                                    0x00,
                                    0x00,
                                    p_write_perm_req->Data_Length,
                                    p_write_perm_req->Data);
          
              LOG_INFO_APP("ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE GHS_CP >>> PERMITTED\r\n");
              GHS_CP_RequestHandler(p_write_perm_req->Data, p_write_perm_req->Data_Length);
            }
            else 
            {
              aci_gatt_permit_write(p_write_perm_req->Connection_Handle,
                                    p_write_perm_req->Attribute_Handle,
                                    0x01,
                                    error_code,
                                    p_write_perm_req->Data_Length,
                                    p_write_perm_req->Data);
              LOG_INFO_APP("ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE GHS_CP >>> NOT PERMITTED\r\n");
            }

            /*USER CODE END Service1_Char_3_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE*/
          }

          /* USER CODE BEGIN EVT_BLUE_GATT_WRITE_PERMIT_REQ_END */
          else if((p_write_perm_req->Attribute_Handle == (GHS_Context.OscDescHdle[0])) ||
                  (p_write_perm_req->Attribute_Handle == (GHS_Context.OscDescHdle[1])))
          {
            return_value = SVCCTL_EvtAckFlowEnable;

            uint32_t observation_type =  p_write_perm_req->Data[0] |
                                        ((p_write_perm_req->Data[1]) << 8) | 
                                        ((p_write_perm_req->Data[2]) << 16) | 
                                        ((p_write_perm_req->Data[3]) << 24);
            uint32_t measurement_period =   p_write_perm_req->Data[4] |
                                          ((p_write_perm_req->Data[5]) << 8) | 
                                          ((p_write_perm_req->Data[6]) << 16) |
                                          ((p_write_perm_req->Data[7]) << 24);
            uint32_t update_interval =   p_write_perm_req->Data[8] |
                                       ((p_write_perm_req->Data[9]) << 8) | 
                                       ((p_write_perm_req->Data[10]) << 16) |
                                       ((p_write_perm_req->Data[11]) << 24);
            if(((observation_type != (ObservationSchedule[0].ObservationType)) && 
                (observation_type != (ObservationSchedule[1].ObservationType))) ||
               (update_interval > measurement_period) ||
               (measurement_period == UNSUPPORTED_MEASUREMENT_PERIOD) ||
               (update_interval == UNSUPPORTED_UPDATE_INTERVAL))
            {
              aci_gatt_permit_write(p_write_perm_req->Connection_Handle,
                                    p_write_perm_req->Attribute_Handle,
                                    0x01,
                                    GHS_ATT_ERROR_CODE_OUT_OF_RANGE,
                                    p_write_perm_req->Data_Length,
                                    p_write_perm_req->Data);
            }
            else
            {
              aci_gatt_permit_write(p_write_perm_req->Connection_Handle,
                                    p_write_perm_req->Attribute_Handle,
                                    0x00,
                                    0x00,
                                    p_write_perm_req->Data_Length,
                                    p_write_perm_req->Data);
            }
          }
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
          LOG_INFO_APP(">>== ACI_GATT_SERVER_CONFIRMATION_VSEVT_CODE\n");
          UTIL_SEQ_SetEvt(1 << CFG_EVENT_GATT_INDICATION_COMPLETE);

          /* USER CODE END ACI_GATT_SERVER_CONFIRMATION_VSEVT_CODE */
          break;/* ACI_GATT_SERVER_CONFIRMATION_VSEVT_CODE */
        }
        /* USER CODE BEGIN BLECORE_EVT */

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
}/* end GHS_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void GHS_Init(void)
{
  Char_UUID_t  uuid;
  tBleStatus ret;
  uint8_t max_attr_record;

  /* USER CODE BEGIN SVCCTL_InitService1Svc_1 */

  /* USER CODE END SVCCTL_InitService1Svc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  SVCCTL_RegisterSvcHandler(GHS_EventHandler);

  /**
   * GHS
   *
   * Max_Attribute_Records = 1 + 2*4 + 1*no_of_char_with_notify_or_indicate_property + 1*no_of_char_with_broadcast_property
   * service_max_attribute_record = 1 for GHS +
   *                                2 for HSF +
   *                                2 for LHO +
   *                                2 for GHSCP +
   *                                2 for OSC +
   *                                1 for HSF configuration descriptor +
   *                                1 for LHO configuration descriptor +
   *                                1 for GHSCP configuration descriptor +
   *                                1 for OSC configuration descriptor +
   *                              = 13
   * This value doesn't take into account number of descriptors manually added
   * In case of descriptors added, please update the max_attr_record value accordingly in the next SVCCTL_InitService User Section
   */
  max_attr_record = 13;

  /* USER CODE BEGIN SVCCTL_InitService */
  /* max_attr_record to be updated if descriptors have been added */
  /* Added 1 Observation Schedule descriptor and 1 Valid Range and Accuracy descriptor per Supported Observation Type */
  max_attr_record += 2*NB_SUPPORTED_OBSERVATION_TYPES;
  /* USER CODE END SVCCTL_InitService */

  uuid.Char_UUID_16 = 0x1840;
  ret = aci_gatt_add_service(UUID_TYPE_16,
                             (Service_UUID_t *) &uuid,
                             PRIMARY_SERVICE,
                             max_attr_record,
                             &(GHS_Context.GhsSvcHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_BLE("  Fail   : aci_gatt_add_service command: GHS, error code: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_BLE("  Success: aci_gatt_add_service command: GhsSvcHdle = 0x%04X\n",GHS_Context.GhsSvcHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService_2 */

  /* USER CODE END SVCCTL_InitService_2 */

  /**
   * HSF
   */
  uuid.Char_UUID_16 = 0x2bf3;
  ret = aci_gatt_add_char(GHS_Context.GhsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeHsf,
                          CHAR_PROP_READ | CHAR_PROP_INDICATE,
                          ATTR_PERMISSION_AUTHOR_WRITE,
                          GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(GHS_Context.HsfCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_BLE("  Fail   : aci_gatt_add_char command   : HSF, error code: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_BLE("  Success: aci_gatt_add_char command   : HsfCharHdle = 0x%04X\n",GHS_Context.HsfCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char1 */
  /* Place holder for Characteristic Descriptors */
  for(uint8_t i = 0; i < NB_SUPPORTED_OBSERVATION_TYPES; i++)
  {
    /**
     * Observation Schedule descriptor
     */
    uuid.Char_UUID_16 = 0x2910;

    /* add the Observation Schedule descriptor */
    ret = aci_gatt_add_char_desc(GHS_Context.GhsSvcHdle, 
                                 GHS_Context.HsfCharHdle, 
                                 UUID_TYPE_16,
                                 (Char_Desc_Uuid_t *)&uuid, 
                                 SizeOscDesc,
                                 SizeOscDesc,
                                 (void *)&(ObservationSchedule[i]),
                                 ATTR_PERMISSION_NONE,
                                 ATTR_ACCESS_READ_WRITE,
                                 GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                                 0x10, 
                                 CHAR_VALUE_LEN_CONSTANT,
                                 &(GHS_Context.OscDescHdle[i]));
    if (ret != BLE_STATUS_SUCCESS)
    {
      LOG_ERROR_APP("  Fail   : aci_gatt_add_char_desc command   : OSC DESC%d, error code: 0x%2X\n", i, ret);
    }
    else
    {
      LOG_INFO_APP("  Success: aci_gatt_add_char_desc command   : OSC DESC%d on handle 0x%x\n",
                  i, GHS_Context.OscDescHdle);
    }

    /**
     * Valid Range and Accuracy descriptor
     */
    uuid.Char_UUID_16 = 0x2911;
    /* add the Valid Range and Accuracy descriptor */
    ret = aci_gatt_add_char_desc(GHS_Context.GhsSvcHdle, 
                                 GHS_Context.HsfCharHdle, 
                                 UUID_TYPE_16,
                                 (Char_Desc_Uuid_t *)&uuid, 
                                 SizeVraDesc,
                                 SizeVraDesc,
                                 (void *)&(ValidRangeAccuracy[i]),
                                 ATTR_PERMISSION_NONE,
                                 ATTR_ACCESS_READ_ONLY,
                                 GATT_DONT_NOTIFY_EVENTS,
                                 0x10, 
                                 CHAR_VALUE_LEN_CONSTANT,
                                 &(GHS_Context.VraDescHdle[i]));
    if (ret != BLE_STATUS_SUCCESS)
    {
      LOG_ERROR_APP("  Fail   : aci_gatt_add_char_desc command   : VRA DESC%d, error code: 0x%2X\n", i, ret);
    }
    else
    {
      LOG_ERROR_APP("  Success: aci_gatt_add_char_desc command   : VRA DESC%d on handle 0x%x\n",
                  i, GHS_Context.VraDescHdle);
    }
  } /* End for(uint8_t i = 0; i < NB_SUPPORTED_OBSERVATION_TYPES; i++) */
  /* USER CODE END SVCCTL_InitService1Char1 */

  /**
   * LHO
   */
  uuid.Char_UUID_16 = 0x2b8b;
  ret = aci_gatt_add_char(GHS_Context.GhsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeLho,
                          CHAR_PROP_NOTIFY | CHAR_PROP_INDICATE,
                          ATTR_PERMISSION_NONE,
                          GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(GHS_Context.LhoCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_BLE("  Fail   : aci_gatt_add_char command   : LHO, error code: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_BLE("  Success: aci_gatt_add_char command   : LhoCharHdle = 0x%04X\n",GHS_Context.LhoCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char2 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char2 */

  /**
   * GHSCP
   */
  uuid.Char_UUID_16 = 0x2bf4;
  ret = aci_gatt_add_char(GHS_Context.GhsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeGhscp,
                          CHAR_PROP_WRITE | CHAR_PROP_INDICATE,
                          ATTR_PERMISSION_NONE,
                          GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(GHS_Context.GhscpCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_BLE("  Fail   : aci_gatt_add_char command   : GHSCP, error code: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_BLE("  Success: aci_gatt_add_char command   : GhscpCharHdle = 0x%04X\n",GHS_Context.GhscpCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char3 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char3 */

  /**
   * OSC
   */
  uuid.Char_UUID_16 = 0x2bf1;
  ret = aci_gatt_add_char(GHS_Context.GhsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeOsc,
                          CHAR_PROP_INDICATE,
                          ATTR_PERMISSION_NONE,
                          GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(GHS_Context.OscCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_BLE("  Fail   : aci_gatt_add_char command   : OSC, error code: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_BLE("  Success: aci_gatt_add_char command   : OscCharHdle = 0x%04X\n",GHS_Context.OscCharHdle);
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
tBleStatus GHS_UpdateValue(GHS_CharOpcode_t CharOpcode, GHS_Data_t *pData)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Service1_App_Update_Char_1 */

  /* USER CODE END Service1_App_Update_Char_1 */

  switch(CharOpcode)
  {
    case GHS_HSF:
      ret = aci_gatt_update_char_value(GHS_Context.GhsSvcHdle,
                                       GHS_Context.HsfCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_BLE("  Fail   : aci_gatt_update_char_value HSF command, error code: 0x%02X\n", ret);
      }
      else
      {
        LOG_INFO_BLE("  Success: aci_gatt_update_char_value HSF command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_1 */

      /* USER CODE END Service1_Char_Value_1 */
      break;

    case GHS_LHO:
      ret = aci_gatt_update_char_value(GHS_Context.GhsSvcHdle,
                                       GHS_Context.LhoCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_BLE("  Fail   : aci_gatt_update_char_value LHO command, error code: 0x%02X\n", ret);
      }
      else
      {
        LOG_INFO_BLE("  Success: aci_gatt_update_char_value LHO command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_2 */

      /* USER CODE END Service1_Char_Value_2 */
      break;

    case GHS_GHSCP:
      ret = aci_gatt_update_char_value(GHS_Context.GhsSvcHdle,
                                       GHS_Context.GhscpCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_BLE("  Fail   : aci_gatt_update_char_value GHSCP command, error code: 0x%02X\n", ret);
      }
      else
      {
        LOG_INFO_BLE("  Success: aci_gatt_update_char_value GHSCP command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_3 */

      /* USER CODE END Service1_Char_Value_3 */
      break;

    case GHS_OSC:
      ret = aci_gatt_update_char_value(GHS_Context.GhsSvcHdle,
                                       GHS_Context.OscCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_BLE("  Fail   : aci_gatt_update_char_value OSC command, error code: 0x%02X\n", ret);
      }
      else
      {
        LOG_INFO_BLE("  Success: aci_gatt_update_char_value OSC command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_4 */

      /* USER CODE END Service1_Char_Value_4 */
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service1_App_Update_Char_2 */

  /* USER CODE END Service1_App_Update_Char_2 */

  return ret;
}
