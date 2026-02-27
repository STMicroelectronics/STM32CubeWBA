/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ESLS.c
  * @author  MCD Application Team
  * @brief   ESLS definition.
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "log_module.h"
#include "esls.h"

/* USER CODE BEGIN Includes */
#include "esls_app.h"
#include "esl_device.h"
#include "app_common.h"
#include "ots.h"
#include "app_ble.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef struct{
  uint16_t  EslsSvcHdle;                  /**< Esls Service Handle */
  uint16_t  Esl_AddrCharHdle;                  /**< ESL_ADDR Characteristic Handle */
  uint16_t  Ap_Sync_Key_MaterialCharHdle;                  /**< AP_SYNC_KEY_MATERIAL Characteristic Handle */
  uint16_t  Esl_Resp_Key_MaterialCharHdle;                  /**< ESL_RESP_KEY_MATERIAL Characteristic Handle */
  uint16_t  Esl_Curr_Abs_TimeCharHdle;                  /**< ESL_CURR_ABS_TIME Characteristic Handle */
  uint16_t  Esl_Display_InfoCharHdle;                  /**< ESL_DISPLAY_INFO Characteristic Handle */
  uint16_t  Esl_Image_InfoCharHdle;                  /**< ESL_IMAGE_INFO Characteristic Handle */
  uint16_t  Esl_Sensor_InfoCharHdle;                  /**< ESL_SENSOR_INFO Characteristic Handle */
  uint16_t  Esl_Led_InfoCharHdle;                  /**< ESL_LED_INFO Characteristic Handle */
  uint16_t  Esl_Control_PointCharHdle;                  /**< ESL_CONTROL_POINT Characteristic Handle */
/* USER CODE BEGIN Context */
  /* Place holder for Characteristic Descriptors Handle*/

/* USER CODE END Context */
}ESLS_Context_t;

/* Private defines -----------------------------------------------------------*/
#define UUID_128_SUPPORTED  1

#if (UUID_128_SUPPORTED == 1)
#define BM_UUID_LENGTH  UUID_TYPE_128
#else
#define BM_UUID_LENGTH  UUID_TYPE_16
#endif

#define BM_REQ_CHAR_SIZE    (3)

/* USER CODE BEGIN PD */
/*
 * UUIDs for ESL service
 */
#define ESL_SERVICE_UUID                0x1857
#define ESL_ADDRESS_UUID                0x2BF6
#define AP_SYNC_KEY_MATERIAL_UUID       0x2BF7
#define ESL_RESP_KEY_MATERIAL_UUID      0x2BF8
#define ESL_CURR_ABS_TIME_UUID          0x2BF9
#define ESL_DISPLAY_INFO_UUID           0x2BFA
#define ESL_IMAGE_INFO_UUID             0x2BFB
#define ESL_SENSOR_INFO_UUID            0x2BFC
#define ESL_LED_INFO_UUID               0x2BFD
#define ESL_CONTROL_POINT_UUID          0x2BFE
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
static const uint16_t SizeEsl_Addr = 2;
static const uint16_t SizeAp_Sync_Key_Material = 24;
static const uint16_t SizeEsl_Resp_Key_Material = 24;
static const uint16_t SizeEsl_Curr_Abs_Time = 4;
static const uint16_t SizeEsl_Display_Info = 10;
static const uint16_t SizeEsl_Image_Info = 1;
static const uint16_t SizeEsl_Sensor_Info = 10;
static const uint16_t SizeEsl_Led_Info = 8;
static const uint16_t SizeEsl_Control_Point = 17;

static ESLS_Context_t ESLS_Context;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static SVCCTL_EvtAckStatus_t ESLS_EventHandler(void *p_pckt);

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
static SVCCTL_EvtAckStatus_t ESLS_EventHandler(void *p_Event)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *p_event_pckt;
  evt_blecore_aci *p_blecore_evt;
  aci_gatt_attribute_modified_event_rp0 *p_attribute_modified;
  aci_gatt_write_permit_req_event_rp0   *p_write_perm_req;
  ESLS_NotificationEvt_t                 notification;
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
        {
          /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_BEGIN */

          /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_BEGIN */
          p_attribute_modified = (aci_gatt_attribute_modified_event_rp0*)p_blecore_evt->data;
          notification.ConnectionHandle         = p_attribute_modified->Connection_Handle;
          notification.AttributeHandle          = p_attribute_modified->Attr_Handle;
          notification.DataTransfered.Length    = p_attribute_modified->Attr_Data_Length;
          notification.DataTransfered.p_Payload = p_attribute_modified->Attr_Data;
          if(p_attribute_modified->Attr_Handle == (ESLS_Context.Esl_Control_PointCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service2_Char_9 */

            /* USER CODE END Service2_Char_9 */
            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service2_Char_9_attribute_modified */

              /* USER CODE END Service2_Char_9_attribute_modified */

              /* Disabled Notification management */
              case (0x00):
                /* USER CODE BEGIN Service2_Char_9_Disabled_BEGIN */

                /* USER CODE END Service2_Char_9_Disabled_BEGIN */
                notification.EvtOpcode = ESLS_ESL_CONTROL_POINT_NOTIFY_DISABLED_EVT;
                ESLS_Notification(&notification);
                /* USER CODE BEGIN Service2_Char_9_Disabled_END */

                /* USER CODE END Service2_Char_9_Disabled_END */
                break;

              /* Enabled Notification management */
              case GATT_CHAR_UPDATE_SEND_NOTIFICATION:
                /* USER CODE BEGIN Service2_Char_9_COMSVC_Notification_BEGIN */

                /* USER CODE END Service2_Char_9_COMSVC_Notification_BEGIN */
                notification.EvtOpcode = ESLS_ESL_CONTROL_POINT_NOTIFY_ENABLED_EVT;
                ESLS_Notification(&notification);
                /* USER CODE BEGIN Service2_Char_9_COMSVC_Notification_END */

                /* USER CODE END Service2_Char_9_COMSVC_Notification_END */
                break;

              default:
                /* USER CODE BEGIN Service2_Char_9_default */

                /* USER CODE END Service2_Char_9_default */
                break;
            }
          }

          else if(p_attribute_modified->Attr_Handle == (ESLS_Context.Ap_Sync_Key_MaterialCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;

            notification.EvtOpcode = ESLS_AP_SYNC_KEY_MATERIAL_WRITE_EVT;
            /* USER CODE BEGIN Service2_Char_2_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */

            /* USER CODE END Service2_Char_2_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
            ESLS_Notification(&notification);
          }
          else if(p_attribute_modified->Attr_Handle == (ESLS_Context.Esl_Resp_Key_MaterialCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;

            notification.EvtOpcode = ESLS_ESL_RESP_KEY_MATERIAL_WRITE_EVT;
            /* USER CODE BEGIN Service2_Char_3_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */

            /* USER CODE END Service2_Char_3_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
            ESLS_Notification(&notification);
          }
          else if(p_attribute_modified->Attr_Handle == (ESLS_Context.Esl_Curr_Abs_TimeCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;

            notification.EvtOpcode = ESLS_ESL_CURR_ABS_TIME_WRITE_EVT;
            /* USER CODE BEGIN Service2_Char_4_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */

            /* USER CODE END Service2_Char_4_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
            ESLS_Notification(&notification);
          }
          else if(p_attribute_modified->Attr_Handle == (ESLS_Context.Esl_Control_PointCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;

            notification.EvtOpcode = ESLS_ESL_CONTROL_POINT_WRITE_NO_RESP_EVT;
            /* USER CODE BEGIN Service2_Char_9_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */

            /* USER CODE END Service2_Char_9_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
            ESLS_Notification(&notification);
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
          if(p_write_perm_req->Attribute_Handle == (ESLS_Context.Esl_AddrCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service2_Char_1_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
            uint16_t ESL_address;
            uint8_t att_error = 0;
            
            ESL_address = LE_TO_HOST_16(&p_write_perm_req->Data);
            
            att_error = ESL_APP_SetESLAddress(ESL_address);
            
            aci_gatt_write_resp(p_write_perm_req->Connection_Handle,
                                p_write_perm_req->Attribute_Handle,
                                (att_error == 0)? 0 : 1,
                                att_error, /* err_code */
                                p_write_perm_req->Data_Length,
                                (uint8_t *)&p_write_perm_req->Data[0]);
            /*USER CODE END Service2_Char_1_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE*/
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

  /* USER CODE BEGIN Service2_EventHandler_2 */

  /* USER CODE END Service2_EventHandler_2 */

  return(return_value);
}/* end ESLS_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void ESLS_Init(void)
{
  Char_UUID_t  uuid;
  tBleStatus ret;
  uint8_t max_attr_record;

  /* USER CODE BEGIN SVCCTL_InitService2Svc_1 */
  Include_UUID_t include_UUID;
  uint16_t include_handle;
  /* USER CODE END SVCCTL_InitService2Svc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  SVCCTL_RegisterSvcHandler(ESLS_EventHandler);

  /**
   * ESLS
   *
   * Max_Attribute_Records = 1 + 2*9 + 1*no_of_char_with_notify_or_indicate_property + 1*no_of_char_with_broadcast_property
   * service_max_attribute_record = 1 for ESLS +
   *                                2 for ESL_ADDR +
   *                                2 for AP_SYNC_KEY_MATERIAL +
   *                                2 for ESL_RESP_KEY_MATERIAL +
   *                                2 for ESL_CURR_ABS_TIME +
   *                                2 for ESL_DISPLAY_INFO +
   *                                2 for ESL_IMAGE_INFO +
   *                                2 for ESL_SENSOR_INFO +
   *                                2 for ESL_LED_INFO +
   *                                2 for ESL_CONTROL_POINT +
   *                                1 for ESL_CONTROL_POINT configuration descriptor +
   *                              = 20
   * This value doesn't take into account number of descriptors manually added
   * In case of descriptors added, please update the max_attr_record value accordingly in the next SVCCTL_InitService User Section
   */
  max_attr_record = 20;

  /* USER CODE BEGIN SVCCTL_InitService */
  /* max_attr_record to be updated if descriptors have been added */
  max_attr_record += 1;
  /* USER CODE END SVCCTL_InitService */

  uuid.Char_UUID_16 = 0x1857;
  ret = aci_gatt_add_service(UUID_TYPE_16,
                             (Service_UUID_t *) &uuid,
                             PRIMARY_SERVICE,
                             max_attr_record,
                             &(ESLS_Context.EslsSvcHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_BLE("  Fail   : aci_gatt_add_service command: ESLS, error code: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_BLE("  Success: aci_gatt_add_service command: EslsSvcHdle = 0x%04X\n",ESLS_Context.EslsSvcHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService_2 */
  include_UUID.Include_UUID_16 = OBJECT_TRANSFER_SERVICE_UUID;
  
  ret = aci_gatt_include_service(ESLS_Context.EslsSvcHdle,
                                 OTS_ServiceStartHandle,
                                 OTS_ServiceEndHandle,
                                 UUID_TYPE_16,
                                 &include_UUID,
                                 &include_handle);
  
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_include_service command: OTS, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_include_service command: OTS\n");
  }

  /* USER CODE END SVCCTL_InitService_2 */

  /**
   * ESL_ADDR
   */
  uuid.Char_UUID_16 = 0x2bf6;
  ret = aci_gatt_add_char(ESLS_Context.EslsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeEsl_Addr,
                          CHAR_PROP_WRITE,
                          ATTR_PERMISSION_ENCRY_WRITE,
                          GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(ESLS_Context.Esl_AddrCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_BLE("  Fail   : aci_gatt_add_char command   : ESL_ADDR, error code: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_BLE("  Success: aci_gatt_add_char command   : Esl_AddrCharHdle = 0x%04X\n",ESLS_Context.Esl_AddrCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char1 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService2Char1 */

  /**
   * AP_SYNC_KEY_MATERIAL
   */
  uuid.Char_UUID_16 = 0x2bf7;
  ret = aci_gatt_add_char(ESLS_Context.EslsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeAp_Sync_Key_Material,
                          CHAR_PROP_WRITE,
                          ATTR_PERMISSION_ENCRY_WRITE,
                          GATT_NOTIFY_ATTRIBUTE_WRITE,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(ESLS_Context.Ap_Sync_Key_MaterialCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_BLE("  Fail   : aci_gatt_add_char command   : AP_SYNC_KEY_MATERIAL, error code: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_BLE("  Success: aci_gatt_add_char command   : Ap_Sync_Key_MaterialCharHdle = 0x%04X\n",ESLS_Context.Ap_Sync_Key_MaterialCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char2 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService2Char2 */

  /**
   * ESL_RESP_KEY_MATERIAL
   */
  uuid.Char_UUID_16 = 0x2bf8;
  ret = aci_gatt_add_char(ESLS_Context.EslsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeEsl_Resp_Key_Material,
                          CHAR_PROP_WRITE,
                          ATTR_PERMISSION_ENCRY_WRITE,
                          GATT_NOTIFY_ATTRIBUTE_WRITE,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(ESLS_Context.Esl_Resp_Key_MaterialCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_BLE("  Fail   : aci_gatt_add_char command   : ESL_RESP_KEY_MATERIAL, error code: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_BLE("  Success: aci_gatt_add_char command   : Esl_Resp_Key_MaterialCharHdle = 0x%04X\n",ESLS_Context.Esl_Resp_Key_MaterialCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char3 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService2Char3 */

  /**
   * ESL_CURR_ABS_TIME
   */
  uuid.Char_UUID_16 = 0x2bf9;
  ret = aci_gatt_add_char(ESLS_Context.EslsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeEsl_Curr_Abs_Time,
                          CHAR_PROP_WRITE,
                          ATTR_PERMISSION_ENCRY_WRITE,
                          GATT_NOTIFY_ATTRIBUTE_WRITE,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(ESLS_Context.Esl_Curr_Abs_TimeCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_BLE("  Fail   : aci_gatt_add_char command   : ESL_CURR_ABS_TIME, error code: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_BLE("  Success: aci_gatt_add_char command   : Esl_Curr_Abs_TimeCharHdle = 0x%04X\n",ESLS_Context.Esl_Curr_Abs_TimeCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char4 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService2Char4 */

  /**
   * ESL_DISPLAY_INFO
   */
  uuid.Char_UUID_16 = 0x2bfa;
  ret = aci_gatt_add_char(ESLS_Context.EslsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeEsl_Display_Info,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_ENCRY_READ,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(ESLS_Context.Esl_Display_InfoCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_BLE("  Fail   : aci_gatt_add_char command   : ESL_DISPLAY_INFO, error code: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_BLE("  Success: aci_gatt_add_char command   : Esl_Display_InfoCharHdle = 0x%04X\n",ESLS_Context.Esl_Display_InfoCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char5 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService2Char5 */

  /**
   * ESL_IMAGE_INFO
   */
  uuid.Char_UUID_16 = 0x2bfb;
  ret = aci_gatt_add_char(ESLS_Context.EslsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeEsl_Image_Info,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_ENCRY_READ,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(ESLS_Context.Esl_Image_InfoCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_BLE("  Fail   : aci_gatt_add_char command   : ESL_IMAGE_INFO, error code: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_BLE("  Success: aci_gatt_add_char command   : Esl_Image_InfoCharHdle = 0x%04X\n",ESLS_Context.Esl_Image_InfoCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char6 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService2Char6 */

  /**
   * ESL_SENSOR_INFO
   */
  uuid.Char_UUID_16 = 0x2bfc;
  ret = aci_gatt_add_char(ESLS_Context.EslsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeEsl_Sensor_Info,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_ENCRY_READ,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(ESLS_Context.Esl_Sensor_InfoCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_BLE("  Fail   : aci_gatt_add_char command   : ESL_SENSOR_INFO, error code: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_BLE("  Success: aci_gatt_add_char command   : Esl_Sensor_InfoCharHdle = 0x%04X\n",ESLS_Context.Esl_Sensor_InfoCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char7 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService2Char7 */

  /**
   * ESL_LED_INFO
   */
  uuid.Char_UUID_16 = 0x2bfd;
  ret = aci_gatt_add_char(ESLS_Context.EslsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeEsl_Led_Info,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_ENCRY_READ,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(ESLS_Context.Esl_Led_InfoCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_BLE("  Fail   : aci_gatt_add_char command   : ESL_LED_INFO, error code: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_BLE("  Success: aci_gatt_add_char command   : Esl_Led_InfoCharHdle = 0x%04X\n",ESLS_Context.Esl_Led_InfoCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char8 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService2Char8 */

  /**
   * ESL_CONTROL_POINT
   */
  uuid.Char_UUID_16 = 0x2bfe;
  ret = aci_gatt_add_char(ESLS_Context.EslsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeEsl_Control_Point,
                          CHAR_PROP_WRITE_WITHOUT_RESP | CHAR_PROP_WRITE | CHAR_PROP_NOTIFY,
                          ATTR_PERMISSION_ENCRY_WRITE,
                          GATT_NOTIFY_ATTRIBUTE_WRITE,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(ESLS_Context.Esl_Control_PointCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_BLE("  Fail   : aci_gatt_add_char command   : ESL_CONTROL_POINT, error code: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_BLE("  Success: aci_gatt_add_char command   : Esl_Control_PointCharHdle = 0x%04X\n",ESLS_Context.Esl_Control_PointCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char9 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService2Char9 */

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
tBleStatus ESLS_UpdateValue(ESLS_CharOpcode_t CharOpcode, ESLS_Data_t *pData)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Service2_App_Update_Char_1 */

  /* USER CODE END Service2_App_Update_Char_1 */

  switch(CharOpcode)
  {
    case ESLS_ESL_ADDR:
      ret = aci_gatt_update_char_value(ESLS_Context.EslsSvcHdle,
                                       ESLS_Context.Esl_AddrCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_BLE("  Fail   : aci_gatt_update_char_value ESL_ADDR command, error code: 0x%02X\n", ret);
      }
      else
      {
        LOG_INFO_BLE("  Success: aci_gatt_update_char_value ESL_ADDR command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_1 */

      /* USER CODE END Service2_Char_Value_1 */
      break;

    case ESLS_AP_SYNC_KEY_MATERIAL:
      ret = aci_gatt_update_char_value(ESLS_Context.EslsSvcHdle,
                                       ESLS_Context.Ap_Sync_Key_MaterialCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_BLE("  Fail   : aci_gatt_update_char_value AP_SYNC_KEY_MATERIAL command, error code: 0x%02X\n", ret);
      }
      else
      {
        LOG_INFO_BLE("  Success: aci_gatt_update_char_value AP_SYNC_KEY_MATERIAL command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_2 */

      /* USER CODE END Service2_Char_Value_2 */
      break;

    case ESLS_ESL_RESP_KEY_MATERIAL:
      ret = aci_gatt_update_char_value(ESLS_Context.EslsSvcHdle,
                                       ESLS_Context.Esl_Resp_Key_MaterialCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_BLE("  Fail   : aci_gatt_update_char_value ESL_RESP_KEY_MATERIAL command, error code: 0x%02X\n", ret);
      }
      else
      {
        LOG_INFO_BLE("  Success: aci_gatt_update_char_value ESL_RESP_KEY_MATERIAL command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_3 */

      /* USER CODE END Service2_Char_Value_3 */
      break;

    case ESLS_ESL_CURR_ABS_TIME:
      ret = aci_gatt_update_char_value(ESLS_Context.EslsSvcHdle,
                                       ESLS_Context.Esl_Curr_Abs_TimeCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_BLE("  Fail   : aci_gatt_update_char_value ESL_CURR_ABS_TIME command, error code: 0x%02X\n", ret);
      }
      else
      {
        LOG_INFO_BLE("  Success: aci_gatt_update_char_value ESL_CURR_ABS_TIME command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_4 */

      /* USER CODE END Service2_Char_Value_4 */
      break;

    case ESLS_ESL_DISPLAY_INFO:
      ret = aci_gatt_update_char_value(ESLS_Context.EslsSvcHdle,
                                       ESLS_Context.Esl_Display_InfoCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_BLE("  Fail   : aci_gatt_update_char_value ESL_DISPLAY_INFO command, error code: 0x%02X\n", ret);
      }
      else
      {
        LOG_INFO_BLE("  Success: aci_gatt_update_char_value ESL_DISPLAY_INFO command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_5 */

      /* USER CODE END Service2_Char_Value_5 */
      break;

    case ESLS_ESL_IMAGE_INFO:
      ret = aci_gatt_update_char_value(ESLS_Context.EslsSvcHdle,
                                       ESLS_Context.Esl_Image_InfoCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_BLE("  Fail   : aci_gatt_update_char_value ESL_IMAGE_INFO command, error code: 0x%02X\n", ret);
      }
      else
      {
        LOG_INFO_BLE("  Success: aci_gatt_update_char_value ESL_IMAGE_INFO command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_6 */

      /* USER CODE END Service2_Char_Value_6 */
      break;

    case ESLS_ESL_SENSOR_INFO:
      ret = aci_gatt_update_char_value(ESLS_Context.EslsSvcHdle,
                                       ESLS_Context.Esl_Sensor_InfoCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_BLE("  Fail   : aci_gatt_update_char_value ESL_SENSOR_INFO command, error code: 0x%02X\n", ret);
      }
      else
      {
        LOG_INFO_BLE("  Success: aci_gatt_update_char_value ESL_SENSOR_INFO command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_7 */

      /* USER CODE END Service2_Char_Value_7 */
      break;

    case ESLS_ESL_LED_INFO:
      ret = aci_gatt_update_char_value(ESLS_Context.EslsSvcHdle,
                                       ESLS_Context.Esl_Led_InfoCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_BLE("  Fail   : aci_gatt_update_char_value ESL_LED_INFO command, error code: 0x%02X\n", ret);
      }
      else
      {
        LOG_INFO_BLE("  Success: aci_gatt_update_char_value ESL_LED_INFO command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_8 */

      /* USER CODE END Service2_Char_Value_8 */
      break;

    case ESLS_ESL_CONTROL_POINT:
      ret = aci_gatt_update_char_value(ESLS_Context.EslsSvcHdle,
                                       ESLS_Context.Esl_Control_PointCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_BLE("  Fail   : aci_gatt_update_char_value ESL_CONTROL_POINT command, error code: 0x%02X\n", ret);
      }
      else
      {
        LOG_INFO_BLE("  Success: aci_gatt_update_char_value ESL_CONTROL_POINT command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_9 */

      /* USER CODE END Service2_Char_Value_9 */
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service2_App_Update_Char_2 */

  /* USER CODE END Service2_App_Update_Char_2 */

  return ret;
}
