/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ots.c
  * @author  MCD Application Team
  * @brief   OTS definition.
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
#include "ots.h"

/* USER CODE BEGIN Includes */
#include "ots_app.h"
#include "app_ble.h"
#include "app_common.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef struct{
  uint16_t  OtsSvcHdle;                  /**< Ots Service Handle */
  uint16_t  OtfCharHdle;                  /**< OTF Characteristic Handle */
  uint16_t  ObnCharHdle;                  /**< OBN Characteristic Handle */
  uint16_t  ObtCharHdle;                  /**< OBT Characteristic Handle */
  uint16_t  ObsCharHdle;                  /**< OBS Characteristic Handle */
  uint16_t  ObiCharHdle;                  /**< OBI Characteristic Handle */
  uint16_t  ObpCharHdle;                  /**< OBP Characteristic Handle */
  uint16_t  OacpCharHdle;                  /**< OACP Characteristic Handle */
  uint16_t  OlcpCharHdle;                  /**< OLCP Characteristic Handle */
/* USER CODE BEGIN Context */
  /* Place holder for Characteristic Descriptors Handle*/

/* USER CODE END Context */
}OTS_Context_t;

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
 * UUIDs for Object Transfer Service
 */
#define OTS_FEATURE_UUID                                                (0x2ABD)
#define OBJECT_NAME_UUID                                                (0x2ABE)
#define OBJECT_TYPE_UUID                                                (0x2ABF)
#define OBJECT_SIZE_UUID                                                (0x2AC0)
#define OBJECT_FIRST_CREATED_UUID                                       (0x2AC1)
#define OBJECT_LAST_MODIFIED_UUID                                       (0x2AC2)
#define OBJECT_ID_UUID                                                  (0x2AC3)
#define OBJECT_PROPERTIES_UUID                                          (0x2AC4)
#define OBJECT_ACTION_CONTROL_POINT_UUID                                (0x2AC5)
#define OBJECT_LIST_CONTROL_POINT_UUID                                  (0x2AC6)
#define OBJECT_LIST_FILTER_POINT_UUID                                   (0x2AC7)
#define OBJECT_CHANGED_UUID                                             (0x2AC8)

/* Op Codes for OACP procedures */
#define OACP_OPCODE_CREATE                                                  0x01
#define OACP_OPCODE_DELETE                                                  0x02
#define OACP_OPCODE_CALC_CHECKSUM                                           0x03
#define OACP_OPCODE_EXECUTE                                                 0x04
#define OACP_OPCODE_READ                                                    0x05
#define OACP_OPCODE_WRITE                                                   0x06
#define OACP_OPCODE_ABORT                                                   0x07
#define OACP_OPCODE_RESPONSE                                                0x60

#define CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET                             2
#define CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET                                  1

#define MAX_OACP_CHARAC_SIZE                                                  21
#define MAX_OLCP_CHARAC_SIZE                                                   7
/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */
uint16_t OTS_ServiceStartHandle;
uint16_t OTS_ServiceEndHandle;
/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
#define CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET         2
#define CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET              1
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static const uint16_t SizeOtf = 8;
static const uint16_t SizeObn = 30;
static const uint16_t SizeObt = 16;
static const uint16_t SizeObs = 8;
static const uint16_t SizeObi = 6;
static const uint16_t SizeObp = 4;
static const uint16_t SizeOacp = 21;
static const uint16_t SizeOlcp = 7;

static OTS_Context_t OTS_Context;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static SVCCTL_EvtAckStatus_t OTS_EventHandler(void *p_pckt);

/* USER CODE BEGIN PFP */

static uint8_t parseOACPFrame(aci_gatt_write_permit_req_event_rp0 *event);
static uint8_t parseOLCPFrame(aci_gatt_write_permit_req_event_rp0 *event);

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
static uint8_t parseOACPFrame(aci_gatt_write_permit_req_event_rp0 *event)
{
  uint8_t op_code;
  uint8_t resp[3];
  uint8_t resp_len;
  uint8_t ret = OACP_RESULT_SUCCESS;
  
  op_code = event->Data[0];
  
  resp[0] = OACP_OPCODE_RESPONSE;         /* Op Code */
  resp[1] = op_code;                      /* Request Op Code */
  resp_len = 2;
  
  /* Only write procedure supported. */
  
  switch(op_code)
  {
  case OACP_OPCODE_WRITE: 
    {
      uint32_t offset, length;
      uint8_t mode;
      
      if(event->Data_Length != 10)
      {
        return BLE_ATT_ERR_INVAL_ATTR_VALUE_LEN;
      }
      
      offset = LE_TO_HOST_32(&event->Data[1]);
      length = LE_TO_HOST_32(&event->Data[5]);
      mode = event->Data[9];
      
      ret = OTS_APP_OACPWrite(offset, length, mode);
      
      resp[resp_len] = ret;
      resp_len += 1;
    }    
    break;
    
  default:
    
    resp[resp_len] = OACP_RESULT_NOT_SUPPORTED;    /* Result code */
    resp_len += 1;
    
    break;
  }
  
  aci_gatt_update_char_value_ext(event->Connection_Handle,
                                 OTS_Context.OtsSvcHdle,
                                 OTS_Context.OacpCharHdle,
                                 0x02, /* Update_Type */
                                 resp_len, /* Char_Length */
                                 0, /* Value_Offset */
                                 resp_len, /* Value_Length */
                                 resp);
  
  return BLE_ATT_ERR_NONE;
}

static uint8_t parseOLCPFrame(aci_gatt_write_permit_req_event_rp0 *event)
{
  uint8_t op_code;
  uint8_t resp[3];
  uint8_t resp_len;
  uint8_t ret = OLCP_RESULT_SUCCESS;
  
  if(event->Data_Length != 1)
  {
    /* Other commands are not supported. */
    return BLE_ATT_ERR_INVAL_ATTR_VALUE_LEN;
  }
  
  op_code = event->Data[0];
  
  resp[0] = OLCP_OPCODE_RESPONSE;         /* Op Code */
  resp[1] = op_code;                      /* Request Op Code */
  resp_len = 2;
  
  ret = OTS_APP_OLCPExec(op_code);
  
  resp[resp_len] = ret;
  resp_len += 1;
  
  aci_gatt_update_char_value_ext(event->Connection_Handle,
                                 OTS_Context.OtsSvcHdle,
                                 OTS_Context.OlcpCharHdle,
                                 0x02, /* Update_Type */
                                 resp_len, /* Char_Length */
                                 0, /* Value_Offset */
                                 resp_len, /* Value_Length */
                                 resp);
  
  return BLE_ATT_ERR_NONE;
}
/* USER CODE END PF */

/**
 * @brief  Event handler
 * @param  p_Event: Address of the buffer holding the p_Event
 * @retval Ack: Return whether the p_Event has been managed or not
 */
static SVCCTL_EvtAckStatus_t OTS_EventHandler(void *p_Event)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *p_event_pckt;
  evt_blecore_aci *p_blecore_evt;
  aci_gatt_attribute_modified_event_rp0 *p_attribute_modified;
  aci_gatt_write_permit_req_event_rp0   *p_write_perm_req;
  aci_gatt_read_permit_req_event_rp0    *p_read_req;
  OTS_NotificationEvt_t                 notification;
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
          if(p_attribute_modified->Attr_Handle == (OTS_Context.OacpCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service1_Char_7 */

            /* USER CODE END Service1_Char_7 */

            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service1_Char_7_attribute_modified */

              /* USER CODE END Service1_Char_7_attribute_modified */

              /* Disabled Indication management */
              case (0x00):
                /* USER CODE BEGIN Service1_Char_7_Disabled_BEGIN */

                /* USER CODE END Service1_Char_7_Disabled_BEGIN */
                notification.EvtOpcode = OTS_OACP_INDICATE_DISABLED_EVT;
                OTS_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_7_Disabled_END */

                /* USER CODE END Service1_Char_7_Disabled_END */
                break;

              /* Enabled Indication management */
              case GATT_CHAR_UPDATE_SEND_INDICATION:
                /* USER CODE BEGIN Service1_Char_7_COMSVC_Indication_BEGIN */

                /* USER CODE END Service1_Char_7_COMSVC_Indication_BEGIN */
                notification.EvtOpcode = OTS_OACP_INDICATE_ENABLED_EVT;
                OTS_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_7_COMSVC_Indication_END */

                /* USER CODE END Service1_Char_7_COMSVC_Indication_END */
                break;

              default:
                /* USER CODE BEGIN Service1_Char_7_default */

                /* USER CODE END Service1_Char_7_default */
                break;
            }
          }

          else if(p_attribute_modified->Attr_Handle == (OTS_Context.OlcpCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service1_Char_8 */

            /* USER CODE END Service1_Char_8 */

            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service1_Char_8_attribute_modified */

              /* USER CODE END Service1_Char_8_attribute_modified */

              /* Disabled Indication management */
              case (0x00):
                /* USER CODE BEGIN Service1_Char_8_Disabled_BEGIN */

                /* USER CODE END Service1_Char_8_Disabled_BEGIN */
                notification.EvtOpcode = OTS_OLCP_INDICATE_DISABLED_EVT;
                OTS_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_8_Disabled_END */

                /* USER CODE END Service1_Char_8_Disabled_END */
                break;

              /* Enabled Indication management */
              case GATT_CHAR_UPDATE_SEND_INDICATION:
                /* USER CODE BEGIN Service1_Char_8_COMSVC_Indication_BEGIN */

                /* USER CODE END Service1_Char_8_COMSVC_Indication_BEGIN */
                notification.EvtOpcode = OTS_OLCP_INDICATE_ENABLED_EVT;
                OTS_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_8_COMSVC_Indication_END */

                /* USER CODE END Service1_Char_8_COMSVC_Indication_END */
                break;

              default:
                /* USER CODE BEGIN Service1_Char_8_default */

                /* USER CODE END Service1_Char_8_default */
                break;
            }
          }

          /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */
          UNUSED(p_attribute_modified);

          /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */
          break;/* ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
        }
        case ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE :
        {
          /* USER CODE BEGIN EVT_BLUE_GATT_READ_PERMIT_REQ_BEGIN */

          /* USER CODE END EVT_BLUE_GATT_READ_PERMIT_REQ_BEGIN */
          p_read_req = (aci_gatt_read_permit_req_event_rp0*)p_blecore_evt->data;
          if(p_read_req->Attribute_Handle == (OTS_Context.ObnCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service1_Char_2_ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE_1 */

            /*USER CODE END Service1_Char_2_ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE_1*/

            /*USER CODE BEGIN Service1_Char_2_ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE_2 */
            char * name_p;
            tBleStatus ret;
            
            OTS_APP_GetCurrentObjName(&name_p);
            
            ret = aci_gatt_update_char_value_ext(0,
                                                 OTS_Context.OtsSvcHdle,
                                                 OTS_Context.ObnCharHdle,
                                                 0, /* Update_Type */
                                                 strlen(name_p), /* Char_Length */
                                                 0, /* Value_Offset */
                                                 strlen(name_p), /* Value_Length */
                                                 (uint8_t *)name_p);
            if(ret != 0)
            {
              LOG_ERROR_APP("aci_gatt_update_char_value_ext fail (0x%02X), handle 0x%04X\n", ret, p_read_req->Attribute_Handle);
            }
            
            ret = aci_gatt_permit_read(p_read_req->Connection_Handle, 0, 0, 0);
            if(ret != 0)
            {
              LOG_ERROR_APP("aci_gatt_permit_read fail (0x%02X)\n", ret, p_read_req->Attribute_Handle);
            }
            /*USER CODE END Service1_Char_2_ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE_2*/
          }
          else if(p_read_req->Attribute_Handle == (OTS_Context.ObtCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service1_Char_3_ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE_1 */

            /*USER CODE END Service1_Char_3_ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE_1*/

            /*USER CODE BEGIN Service1_Char_3_ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE_2 */
            uint8_t uuid_type;
            uint8_t *uuid_p;
            uint16_t char_len;
            tBleStatus ret;
            
            OTS_APP_GetCurrentObjType(&uuid_type, &uuid_p);
            
            if(uuid_type == 0)
            {
              char_len = 2;
            }
            else
            {
              char_len = 16;
            }
            
            ret = aci_gatt_update_char_value_ext(0,
                                                 OTS_Context.OtsSvcHdle,
                                                 OTS_Context.ObtCharHdle,
                                                 0, /* Update_Type */
                                                 char_len, /* Char_Length */
                                                 0, /* Value_Offset */
                                                 char_len, /* Value_Length */
                                                 uuid_p);
            if(ret != 0)
            {
              LOG_ERROR_APP("aci_gatt_update_char_value_ext fail (0x%02X), handle 0x%04X\n", ret, p_read_req->Attribute_Handle);
            }
            
            ret = aci_gatt_permit_read(p_read_req->Connection_Handle, 0, 0, 0);
            if(ret != 0)
            {
              LOG_ERROR_APP("aci_gatt_permit_read fail (0x%02X)\n", ret, p_read_req->Attribute_Handle);
            }
            /*USER CODE END Service1_Char_3_ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE_2*/
          }
          else if(p_read_req->Attribute_Handle == (OTS_Context.ObsCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service1_Char_4_ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE_1 */

            /*USER CODE END Service1_Char_4_ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE_1*/

            /*USER CODE BEGIN Service1_Char_4_ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE_2 */
            uint32_t obj_size[2]; /* current size and allocated size */
            tBleStatus ret;
            
            OTS_APP_GetCurrentObjSize(&obj_size[0], &obj_size[1]);
            
            ret = aci_gatt_update_char_value_ext(0,
                                                 OTS_Context.OtsSvcHdle,
                                                 OTS_Context.ObsCharHdle,
                                                 0, /* Update_Type */
                                                 sizeof(obj_size), /* Char_Length */
                                                 0, /* Value_Offset */
                                                 sizeof(obj_size), /* Value_Length */
                                                 (uint8_t *)obj_size);
            if(ret != 0)
            {
              LOG_ERROR_APP("aci_gatt_update_char_value_ext fail (0x%02X), handle 0x%04X\n", ret, p_read_req->Attribute_Handle);
            }
            
            ret = aci_gatt_permit_read(p_read_req->Connection_Handle, 0, 0, 0);
            if(ret != 0)
            {
              LOG_ERROR_APP("aci_gatt_permit_read fail (0x%02X)\n", ret, p_read_req->Attribute_Handle);
            }
            /*USER CODE END Service1_Char_4_ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE_2*/
          }
          else if(p_read_req->Attribute_Handle == (OTS_Context.ObiCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service1_Char_5_ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE_1 */

            /*USER CODE END Service1_Char_5_ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE_1*/

            /*USER CODE BEGIN Service1_Char_5_ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE_2 */
            uint8_t *id_p;
            tBleStatus ret;
            
            OTS_APP_GetCurrentObjID(&id_p);
            
            ret = aci_gatt_update_char_value_ext(0,
                                                 OTS_Context.OtsSvcHdle,
                                                 OTS_Context.ObiCharHdle,
                                                 0, /* Update_Type */
                                                 OBJECT_ID_SIZE, /* Char_Length */
                                                 0, /* Value_Offset */
                                                 OBJECT_ID_SIZE, /* Value_Length */
                                                 id_p);
            if(ret != 0)
            {
              LOG_ERROR_APP("aci_gatt_update_char_value_ext fail (0x%02X), handle 0x%04X\n", ret, p_read_req->Attribute_Handle);
            }
            
            ret = aci_gatt_permit_read(p_read_req->Connection_Handle, 0, 0, 0);
            if(ret != 0)
            {
              LOG_ERROR_APP("aci_gatt_permit_read fail (0x%02X)\n", ret, p_read_req->Attribute_Handle);
            }
            /*USER CODE END Service1_Char_5_ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE_2*/
          }
          else if(p_read_req->Attribute_Handle == (OTS_Context.ObpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service1_Char_6_ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE_1 */

            /*USER CODE END Service1_Char_6_ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE_1*/

            /*USER CODE BEGIN Service1_Char_6_ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE_2 */
            uint32_t obj_prop;
            tBleStatus ret;
            
            OTS_APP_GetCurrentObjProp(&obj_prop);
            
            ret = aci_gatt_update_char_value_ext(0,
                                                 OTS_Context.OtsSvcHdle,
                                                 OTS_Context.ObpCharHdle,
                                                 0, /* Update_Type */
                                                 sizeof(obj_prop), /* Char_Length */
                                                 0, /* Value_Offset */
                                                 sizeof(obj_prop), /* Value_Length */
                                                 (uint8_t *)&obj_prop);
            if(ret != 0)
            {
              LOG_ERROR_APP("aci_gatt_update_char_value_ext fail (0x%02X), handle 0x%04X\n", ret, p_read_req->Attribute_Handle);
            }
            
            ret = aci_gatt_permit_read(p_read_req->Connection_Handle, 0, 0, 0);
            if(ret != 0)
            {
              LOG_ERROR_APP("aci_gatt_permit_read fail (0x%02X)\n", ret, p_read_req->Attribute_Handle);
            }
            /*USER CODE END Service1_Char_6_ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE_2*/
          }

          /* USER CODE BEGIN EVT_BLUE_GATT_READ_PERMIT_REQ_END */

          /* USER CODE END EVT_BLUE_GATT_READ_PERMIT_REQ_END */
          break;/* ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE */
        }
        case ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE:
        {
          /* USER CODE BEGIN EVT_BLUE_GATT_WRITE_PERMIT_REQ_BEGIN */
          uint8_t att_error = BLE_ATT_ERR_NONE;
          /* USER CODE END EVT_BLUE_GATT_WRITE_PERMIT_REQ_BEGIN */
          p_write_perm_req = (aci_gatt_write_permit_req_event_rp0*)p_blecore_evt->data;
          if(p_write_perm_req->Attribute_Handle == (OTS_Context.OacpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service1_Char_7_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
            if(p_write_perm_req->Data_Length > MAX_OACP_CHARAC_SIZE)
            {
              att_error = BLE_ATT_ERR_INVAL_ATTR_VALUE_LEN;          
            }
            else
            {
              tBleStatus ret;
              uint16_t attr_len;
              uint16_t val_len;
              uint16_t cccd_val;
              
              /* It is possible to use aci_gatt_read_handle_value() on CCCD because we only have one connection. */
              
              ret = aci_gatt_read_handle_value(OTS_Context.OacpCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET,
                                               0, 2, &attr_len, &val_len, (uint8_t *)&cccd_val);
              if(ret != BLE_STATUS_SUCCESS)
              {
                LOG_ERROR_APP("Read CCC error\n");
                
                return return_value;
              }       
              if((cccd_val & 0x02) == 0) /* Check if indications are enabled */
              {
                att_error = BLE_ATT_ERR_CCCD_IMPROPERLY_CONFIGURED;
              }
            }
            
            if(att_error == 0)
            {
              att_error = parseOACPFrame(p_write_perm_req);
            }
            
            aci_gatt_permit_write(p_write_perm_req->Connection_Handle,
                                  p_write_perm_req->Attribute_Handle,
                                  (att_error == 0)? 0 : 1,
                                  att_error,
                                  p_write_perm_req->Data_Length,
                                  p_write_perm_req->Data);
            /*USER CODE END Service1_Char_7_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE*/
          }

          else if(p_write_perm_req->Attribute_Handle == (OTS_Context.OlcpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service1_Char_8_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
            
            if(p_write_perm_req->Data_Length > MAX_OLCP_CHARAC_SIZE)
            {
              att_error = BLE_ATT_ERR_INVAL_ATTR_VALUE_LEN;          
            }
            else
            {
              tBleStatus ret;
              uint16_t attr_len;
              uint16_t val_len;
              uint16_t cccd_val;
              
              /* It is possible to use aci_gatt_read_handle_value() on CCCD because we only have one connection. */
              
              ret = aci_gatt_read_handle_value(OTS_Context.OlcpCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET,
                                               0, 2, &attr_len, &val_len, (uint8_t *)&cccd_val);
              if(ret != BLE_STATUS_SUCCESS)
              {
                LOG_ERROR_APP("Read CCC error\n");
                
                return return_value;
              }        
              if((cccd_val & 0x02) == 0) /* Check if indications are enabled */
              {
                att_error = BLE_ATT_ERR_CCCD_IMPROPERLY_CONFIGURED;
              }
            }
            
            if(att_error == 0)
            {
              att_error = parseOLCPFrame(p_write_perm_req);
            }
            
            aci_gatt_permit_write(p_write_perm_req->Connection_Handle,
                                  p_write_perm_req->Attribute_Handle,
                                  (att_error == 0)? 0 : 1,
                                  att_error,
                                  p_write_perm_req->Data_Length,
                                  p_write_perm_req->Data);
            
            /*USER CODE END Service1_Char_8_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE*/
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

  /* USER CODE BEGIN Service1_EventHandler_2 */

  /* USER CODE END Service1_EventHandler_2 */

  return(return_value);
}/* end OTS_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void OTS_Init(void)
{
  Char_UUID_t  uuid;
  tBleStatus ret;
  uint8_t max_attr_record;

  /* USER CODE BEGIN SVCCTL_InitService1Svc_1 */

  /* USER CODE END SVCCTL_InitService1Svc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  SVCCTL_RegisterSvcHandler(OTS_EventHandler);

  /**
   * OTS
   *
   * Max_Attribute_Records = 1 + 2*8 + 1*no_of_char_with_notify_or_indicate_property + 1*no_of_char_with_broadcast_property
   * service_max_attribute_record = 1 for OTS +
   *                                2 for OTF +
   *                                2 for OBN +
   *                                2 for OBT +
   *                                2 for OBS +
   *                                2 for OBI +
   *                                2 for OBP +
   *                                2 for OACP +
   *                                2 for OLCP +
   *                                1 for OACP configuration descriptor +
   *                                1 for OLCP configuration descriptor +
   *                              = 19
   * This value doesn't take into account number of descriptors manually added
   * In case of descriptors added, please update the max_attr_record value accordingly in the next SVCCTL_InitService User Section
   */
  max_attr_record = 19;

  /* USER CODE BEGIN SVCCTL_InitService */
  /* max_attr_record to be updated if descriptors have been added */

  /* USER CODE END SVCCTL_InitService */

  uuid.Char_UUID_16 = 0x1825;
  ret = aci_gatt_add_service(UUID_TYPE_16,
                             (Service_UUID_t *) &uuid,
                             SECONDARY_SERVICE,
                             max_attr_record,
                             &(OTS_Context.OtsSvcHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_BLE("  Fail   : aci_gatt_add_service command: OTS, error code: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_BLE("  Success: aci_gatt_add_service command: OtsSvcHdle = 0x%04X\n",OTS_Context.OtsSvcHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService_2 */
  if (ret == BLE_STATUS_SUCCESS)
  {
    OTS_ServiceStartHandle = OTS_Context.OtsSvcHdle;
    OTS_ServiceEndHandle = OTS_Context.OtsSvcHdle + max_attr_record - 1;
  }

  /* USER CODE END SVCCTL_InitService_2 */

  /**
   * OTF
   */
  uuid.Char_UUID_16 = 0x2abd;
  ret = aci_gatt_add_char(OTS_Context.OtsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeOtf,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_ENCRY_READ,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(OTS_Context.OtfCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_BLE("  Fail   : aci_gatt_add_char command   : OTF, error code: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_BLE("  Success: aci_gatt_add_char command   : OtfCharHdle = 0x%04X\n",OTS_Context.OtfCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char1 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char1 */

  /**
   * OBN
   */
  uuid.Char_UUID_16 = 0x2abe;
  ret = aci_gatt_add_char(OTS_Context.OtsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeObn,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_ENCRY_READ,
                          GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(OTS_Context.ObnCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_BLE("  Fail   : aci_gatt_add_char command   : OBN, error code: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_BLE("  Success: aci_gatt_add_char command   : ObnCharHdle = 0x%04X\n",OTS_Context.ObnCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char2 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char2 */

  /**
   * OBT
   */
  uuid.Char_UUID_16 = 0x2abf;
  ret = aci_gatt_add_char(OTS_Context.OtsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeObt,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_ENCRY_READ,
                          GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(OTS_Context.ObtCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_BLE("  Fail   : aci_gatt_add_char command   : OBT, error code: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_BLE("  Success: aci_gatt_add_char command   : ObtCharHdle = 0x%04X\n",OTS_Context.ObtCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char3 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char3 */

  /**
   * OBS
   */
  uuid.Char_UUID_16 = 0x2ac0;
  ret = aci_gatt_add_char(OTS_Context.OtsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeObs,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_ENCRY_READ,
                          GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(OTS_Context.ObsCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_BLE("  Fail   : aci_gatt_add_char command   : OBS, error code: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_BLE("  Success: aci_gatt_add_char command   : ObsCharHdle = 0x%04X\n",OTS_Context.ObsCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char4 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char4 */

  /**
   * OBI
   */
  uuid.Char_UUID_16 = 0x2ac3;
  ret = aci_gatt_add_char(OTS_Context.OtsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeObi,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_ENCRY_READ,
                          GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(OTS_Context.ObiCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_BLE("  Fail   : aci_gatt_add_char command   : OBI, error code: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_BLE("  Success: aci_gatt_add_char command   : ObiCharHdle = 0x%04X\n",OTS_Context.ObiCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char5 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char5 */

  /**
   * OBP
   */
  uuid.Char_UUID_16 = 0x2ac4;
  ret = aci_gatt_add_char(OTS_Context.OtsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeObp,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_ENCRY_READ,
                          GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(OTS_Context.ObpCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_BLE("  Fail   : aci_gatt_add_char command   : OBP, error code: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_BLE("  Success: aci_gatt_add_char command   : ObpCharHdle = 0x%04X\n",OTS_Context.ObpCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char6 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char6 */

  /**
   * OACP
   */
  uuid.Char_UUID_16 = 0x2ac5;
  ret = aci_gatt_add_char(OTS_Context.OtsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeOacp,
                          CHAR_PROP_WRITE | CHAR_PROP_INDICATE,
                          ATTR_PERMISSION_ENCRY_READ | ATTR_PERMISSION_ENCRY_WRITE,
                          GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(OTS_Context.OacpCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_BLE("  Fail   : aci_gatt_add_char command   : OACP, error code: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_BLE("  Success: aci_gatt_add_char command   : OacpCharHdle = 0x%04X\n",OTS_Context.OacpCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char7 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char7 */

  /**
   * OLCP
   */
  uuid.Char_UUID_16 = 0x2ac6;
  ret = aci_gatt_add_char(OTS_Context.OtsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeOlcp,
                          CHAR_PROP_WRITE | CHAR_PROP_INDICATE,
                          ATTR_PERMISSION_ENCRY_READ | ATTR_PERMISSION_ENCRY_WRITE,
                          GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(OTS_Context.OlcpCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_BLE("  Fail   : aci_gatt_add_char command   : OLCP, error code: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_BLE("  Success: aci_gatt_add_char command   : OlcpCharHdle = 0x%04X\n",OTS_Context.OlcpCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char8 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char8 */

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
tBleStatus OTS_UpdateValue(OTS_CharOpcode_t CharOpcode, OTS_Data_t *pData)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Service1_App_Update_Char_1 */

  /* USER CODE END Service1_App_Update_Char_1 */

  switch(CharOpcode)
  {
    case OTS_OTF:
      ret = aci_gatt_update_char_value(OTS_Context.OtsSvcHdle,
                                       OTS_Context.OtfCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_BLE("  Fail   : aci_gatt_update_char_value OTF command, error code: 0x%02X\n", ret);
      }
      else
      {
        LOG_INFO_BLE("  Success: aci_gatt_update_char_value OTF command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_1 */

      /* USER CODE END Service1_Char_Value_1 */
      break;

    case OTS_OBN:
      ret = aci_gatt_update_char_value(OTS_Context.OtsSvcHdle,
                                       OTS_Context.ObnCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_BLE("  Fail   : aci_gatt_update_char_value OBN command, error code: 0x%02X\n", ret);
      }
      else
      {
        LOG_INFO_BLE("  Success: aci_gatt_update_char_value OBN command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_2 */

      /* USER CODE END Service1_Char_Value_2 */
      break;

    case OTS_OBT:
      ret = aci_gatt_update_char_value(OTS_Context.OtsSvcHdle,
                                       OTS_Context.ObtCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_BLE("  Fail   : aci_gatt_update_char_value OBT command, error code: 0x%02X\n", ret);
      }
      else
      {
        LOG_INFO_BLE("  Success: aci_gatt_update_char_value OBT command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_3 */

      /* USER CODE END Service1_Char_Value_3 */
      break;

    case OTS_OBS:
      ret = aci_gatt_update_char_value(OTS_Context.OtsSvcHdle,
                                       OTS_Context.ObsCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_BLE("  Fail   : aci_gatt_update_char_value OBS command, error code: 0x%02X\n", ret);
      }
      else
      {
        LOG_INFO_BLE("  Success: aci_gatt_update_char_value OBS command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_4 */

      /* USER CODE END Service1_Char_Value_4 */
      break;

    case OTS_OBI:
      ret = aci_gatt_update_char_value(OTS_Context.OtsSvcHdle,
                                       OTS_Context.ObiCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_BLE("  Fail   : aci_gatt_update_char_value OBI command, error code: 0x%02X\n", ret);
      }
      else
      {
        LOG_INFO_BLE("  Success: aci_gatt_update_char_value OBI command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_5 */

      /* USER CODE END Service1_Char_Value_5 */
      break;

    case OTS_OBP:
      ret = aci_gatt_update_char_value(OTS_Context.OtsSvcHdle,
                                       OTS_Context.ObpCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_BLE("  Fail   : aci_gatt_update_char_value OBP command, error code: 0x%02X\n", ret);
      }
      else
      {
        LOG_INFO_BLE("  Success: aci_gatt_update_char_value OBP command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_6 */

      /* USER CODE END Service1_Char_Value_6 */
      break;

    case OTS_OACP:
      ret = aci_gatt_update_char_value(OTS_Context.OtsSvcHdle,
                                       OTS_Context.OacpCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_BLE("  Fail   : aci_gatt_update_char_value OACP command, error code: 0x%02X\n", ret);
      }
      else
      {
        LOG_INFO_BLE("  Success: aci_gatt_update_char_value OACP command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_7 */

      /* USER CODE END Service1_Char_Value_7 */
      break;

    case OTS_OLCP:
      ret = aci_gatt_update_char_value(OTS_Context.OtsSvcHdle,
                                       OTS_Context.OlcpCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_BLE("  Fail   : aci_gatt_update_char_value OLCP command, error code: 0x%02X\n", ret);
      }
      else
      {
        LOG_INFO_BLE("  Success: aci_gatt_update_char_value OLCP command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_8 */

      /* USER CODE END Service1_Char_Value_8 */
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service1_App_Update_Char_2 */

  /* USER CODE END Service1_App_Update_Char_2 */

  return ret;
}
