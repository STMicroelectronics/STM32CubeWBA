/**
  ******************************************************************************
  * @file    has.c
  * @author  MCD Application Team
  * @brief   This file contains Hearing Access Service
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

/* Includes ------------------------------------------------------------------*/
#include "has.h"
#if (BLE_CFG_HAP_HA_ROLE == 1u)
#include "ble_gatt_aci.h"
#include "hap_log.h"
#include "ble_vs_codes.h"

/* Private defines -----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/
#define NUM_HAS_CHARACTERISTIC_RECORDS    (9u)
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static HAS_ServiceContext_t *p_srv_ctxt = 0;
/* Private functions prototype------------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/
/**
  * @brief  Initialize the Hearing Aid Access Service
  */
tBleStatus HAS_InitService(HAS_ServiceContext_t *pSrvContext)
{
  uint16_t uuid;
  uint8_t num_char_records = NUM_HAS_CHARACTERISTIC_RECORDS;
  tBleStatus hciCmdResult = BLE_STATUS_SUCCESS;

  memset(pSrvContext, 0, sizeof(HAS_ServiceContext_t) );
  /**
  *  Add Hearing Aid Access Service
  */
  uuid = HEARING_ACCESS_SERVICE_UUID;
  hciCmdResult = aci_gatt_add_service(UUID_TYPE_16,
                                       (Service_UUID_t *) &uuid,
                                       PRIMARY_SERVICE,
                                       (1+num_char_records),
                                       &(pSrvContext->ServiceHandle));

  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    pSrvContext->ServiceEndHandle = pSrvContext->ServiceHandle+num_char_records;
    BLE_DBG_HAS_MSG("Hearing Access Service (HAS) is added Successfully %04X (Max_Attribute_Records : %d)\n",
                    pSrvContext->ServiceHandle,
                    (1+num_char_records));
    p_srv_ctxt = pSrvContext;
  }
  else
  {
    BLE_DBG_HAS_MSG("FAILED to add Hearing Access Service (HAS), Error: %02X !!\n",hciCmdResult);
  }
  return hciCmdResult;
}

tBleStatus HAS_InitCharacteristics(HAS_ServiceContext_t *pSrvContext)
{
  uint16_t uuid;
  tBleStatus hciCmdResult = BLE_STATUS_SUCCESS;
  /*
  *  Add Hearing Aid Features Characteristic
  */
  uuid = HEARING_AID_FEATURES_UUID;
  hciCmdResult = aci_gatt_add_char(pSrvContext->ServiceHandle,
                                  UUID_TYPE_16,
                                  (Char_UUID_t *) &uuid ,
                                  1u,
                                  (CHAR_PROP_READ | CHAR_PROP_NOTIFY),
                                  ATTR_PERMISSION_ENCRY_READ,
                                  GATT_NOTIFY_NOTIFICATION_COMPLETION,
                                  10,
                                  0,
                                  &(pSrvContext->HearingAidFeaturesHandle));

  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    BLE_DBG_HAS_MSG("Hearing Aid Features Characteristic added Successfully %04X\n", pSrvContext->HearingAidFeaturesHandle);
  }
  else
  {
    BLE_DBG_HAS_MSG("FAILED to add Hearing Aid Features Characteristic, Error: %02X !!\n", hciCmdResult);
  }

  /**
  *  Add Hearing Aid Preset Control Point Characteristic
  */
  uuid = HEARING_AID_PRESET_CONTROL_POINT_UUID;
  hciCmdResult = aci_gatt_add_char(pSrvContext->ServiceHandle,
                                  UUID_TYPE_16,
                                  (Char_UUID_t *) &uuid ,
                                  MAX_HEARING_AID_PRESET_CONTROL_POINT_SIZE,
                                  (CHAR_PROP_WRITE | CHAR_PROP_INDICATE),
                                  ATTR_PERMISSION_ENCRY_WRITE,
                                  (GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP),
                                  10,
                                  1,
                                  &(pSrvContext->HearingAidPresetControlPointHandle));

  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    BLE_DBG_HAS_MSG("Hearing Aid Preset Control Point Characteristic added Successfully %04X\n",pSrvContext->HearingAidPresetControlPointHandle);
  }
  else
  {
    BLE_DBG_HAS_MSG("FAILED to add Hearing Aid Preset Control Point Characteristic, Error: %02X !!\n",hciCmdResult);
  }

  /**
  *  Add Active Preset Index Characteristic
  */
  uuid = ACTIVE_PRESET_INDEX_UUID;
  hciCmdResult = aci_gatt_add_char(pSrvContext->ServiceHandle,
                                  UUID_TYPE_16,
                                  (Char_UUID_t *) &uuid ,
                                  1u,
                                  (CHAR_PROP_READ | CHAR_PROP_NOTIFY),
                                  ATTR_PERMISSION_ENCRY_READ,
                                  GATT_NOTIFY_NOTIFICATION_COMPLETION,
                                  10,
                                  0,
                                  &(pSrvContext->ActivePresetIndexHandle));

  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    BLE_DBG_HAS_MSG("Active Preset Index Characteristic added Successfully %04X\n",pSrvContext->ActivePresetIndexHandle);
  }
  else
  {
    BLE_DBG_HAS_MSG("FAILED to add Active Preset Index Characteristic, Error: %02X !!\n",hciCmdResult);
  }

  return hciCmdResult;
}

tBleStatus HAS_SetHearingAidFeatures(HAS_ServiceContext_t *pSrvContext, uint16_t ConnHandle, HAP_HA_Features_t Features)
{
  tBleStatus hciCmdResult = BLE_STATUS_INVALID_PARAMS;

  if (pSrvContext->HearingAidFeaturesHandle != 0x00u)
  {
    hciCmdResult = aci_gatt_update_char_value_ext(ConnHandle,
                                                  pSrvContext->ServiceHandle,
                                                  pSrvContext->HearingAidFeaturesHandle,
                                                  0x01,                           /*Notification*/
                                                  1,                              /* Char_Length*/
                                                  0,                              /* charValOffset */
                                                  1,                              /* charValueLen */
                                                  (uint8_t*) &Features);
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      BLE_DBG_HAS_MSG("Hearing Aid Features Characteristic (%04X) Value updated Successfully\n",
                      pSrvContext->HearingAidFeaturesHandle);
    }
    else
    {
      BLE_DBG_HAS_MSG("FAILED to update Hearing Aid Features Characteristic (%04X) Value, Error: %02X !!\n",
                      pSrvContext->HearingAidFeaturesHandle,
                      hciCmdResult);
    }
  }
  return hciCmdResult;
}

tBleStatus HAS_SetActivePresetIndex(HAS_ServiceContext_t *pSrvContext, uint16_t ConnHandle, uint8_t ActivePresetIndex)
{
  tBleStatus hciCmdResult = BLE_STATUS_INVALID_PARAMS;

  if (pSrvContext->ActivePresetIndexHandle != 0x00u)
  {
    hciCmdResult = aci_gatt_update_char_value_ext(ConnHandle,
                                                  pSrvContext->ServiceHandle,
                                                  pSrvContext->ActivePresetIndexHandle,
                                                  0x01,                           /*Notification*/
                                                  1,                              /* Char_Length*/
                                                  0,                              /* charValOffset */
                                                  1,                              /* charValueLen */
                                                  &ActivePresetIndex);
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      BLE_DBG_HAS_MSG("Active Preset Index Characteristic (%04X) Value updated Successfully\n",
                      pSrvContext->ActivePresetIndexHandle);
    }
    else
    {
      BLE_DBG_HAS_MSG("FAILED to update Active Preset Index Characteristic (%04X) Value, Error: %02X !!\n",
                      pSrvContext->ActivePresetIndexHandle,
                      hciCmdResult);
    }
  }
  return hciCmdResult;
}

tBleStatus HAS_SetReadPresetsResponse(HAS_ServiceContext_t *pSrvContext, uint16_t ConnHandle,
                                             HAP_Preset_t* PresetRecord, uint8_t IsLast)
{
  tBleStatus hciCmdResult;
  uint8_t a_value[MAX_HEARING_AID_PRESET_CONTROL_POINT_SIZE];

  a_value[0u] = HAP_HA_CONTROL_POINT_OP_READ_PRESETS_RESPONSE;

  a_value[1u] = IsLast;

  a_value[2u] = PresetRecord->Index;
  a_value[3u] = PresetRecord->Properties;
  memcpy(&a_value[4u], &PresetRecord->Name[0], PresetRecord->NameLen);


  hciCmdResult = aci_gatt_update_char_value_ext(ConnHandle,
                                                pSrvContext->ServiceHandle,
                                                pSrvContext->HearingAidPresetControlPointHandle,
                                                0x02,                           /*Notification*/
                                                4 + PresetRecord->NameLen,      /* Char_Length*/
                                                0,                              /* charValOffset */
                                                4 + PresetRecord->NameLen,      /* charValueLen */
                                                &a_value[0]);
  BLE_DBG_HAS_MSG("Read Presets Response set with status 0x%02X\n", hciCmdResult);

  return hciCmdResult;
}

tBleStatus HAS_SetPresetGenericUpdate(HAS_ServiceContext_t *pSrvContext, uint16_t ConnHandle, uint8_t IsLast,
                                      uint8_t PrevIndex,HAP_Preset_t* PresetRecord)
{
  tBleStatus hciCmdResult;
  uint8_t a_value[MAX_HEARING_AID_PRESET_CONTROL_POINT_SIZE];

  a_value[0u] = HAP_HA_CONTROL_POINT_OP_PRESET_CHANGED;
  a_value[1u] = HAP_PRESET_CHANGE_ID_GENERIC_UPDATE;
  a_value[2u] = IsLast;
  a_value[3u] = PrevIndex;
  a_value[4u] = PresetRecord->Index;
  a_value[5u] = PresetRecord->Properties;
  memcpy(&a_value[6u], &PresetRecord->Name[0], PresetRecord->NameLen);

  hciCmdResult = aci_gatt_update_char_value_ext(ConnHandle,
                                                pSrvContext->ServiceHandle,
                                                pSrvContext->HearingAidPresetControlPointHandle,
                                                0x02,                           /*Notification*/
                                                6 + PresetRecord->NameLen,      /* Char_Length*/
                                                0,                              /* charValOffset */
                                                6 + PresetRecord->NameLen,      /* charValueLen */
                                                &a_value[0]);
  BLE_DBG_HAS_MSG("Preset Generic Update set with status 0x%02X\n", hciCmdResult);

  return hciCmdResult;
}

tBleStatus HAS_SetPresetRecordDeleted(HAS_ServiceContext_t *pSrvContext, uint16_t ConnHandle, uint8_t IsLast,
                                      uint8_t Index)
{
  tBleStatus hciCmdResult;
  uint8_t a_value[MAX_HEARING_AID_PRESET_CONTROL_POINT_SIZE];

  a_value[0u] = HAP_HA_CONTROL_POINT_OP_PRESET_CHANGED;
  a_value[1u] = HAP_PRESET_CHANGE_ID_PRESET_RECORD_DELETED;
  a_value[2u] = IsLast;
  a_value[3u] = Index;

  hciCmdResult = aci_gatt_update_char_value_ext(ConnHandle,
                                                pSrvContext->ServiceHandle,
                                                pSrvContext->HearingAidPresetControlPointHandle,
                                                0x02,                           /*Notification*/
                                                4,                              /* Char_Length*/
                                                0,                              /* charValOffset */
                                                4,                              /* charValueLen */
                                                &a_value[0]);
  BLE_DBG_HAS_MSG("Preset Record Deleted set with status 0x%02X\n", hciCmdResult);

  return hciCmdResult;
}

tBleStatus HAS_SetPresetRecordAvailable(HAS_ServiceContext_t *pSrvContext, uint16_t ConnHandle, uint8_t IsLast,
                                        uint8_t Index)
{
  tBleStatus hciCmdResult;
  uint8_t a_value[MAX_HEARING_AID_PRESET_CONTROL_POINT_SIZE];

  a_value[0u] = HAP_HA_CONTROL_POINT_OP_PRESET_CHANGED;
  a_value[1u] = HAP_PRESET_CHANGE_ID_PRESET_RECORD_AVAILABLE;
  a_value[2u] = IsLast;
  a_value[3u] = Index;

  hciCmdResult = aci_gatt_update_char_value_ext(ConnHandle,
                                                pSrvContext->ServiceHandle,
                                                pSrvContext->HearingAidPresetControlPointHandle,
                                                0x02,                           /*Notification*/
                                                4,                              /* Char_Length*/
                                                0,                              /* charValOffset */
                                                4,                              /* charValueLen */
                                                &a_value[0]);
  BLE_DBG_HAS_MSG("Preset Record Available set with status 0x%02X\n", hciCmdResult);

  return hciCmdResult;
}

tBleStatus HAS_SetPresetRecordUnavailable(HAS_ServiceContext_t *pSrvContext, uint16_t ConnHandle, uint8_t IsLast,
                                          uint8_t Index)
{
  tBleStatus hciCmdResult;
  uint8_t a_value[MAX_HEARING_AID_PRESET_CONTROL_POINT_SIZE];

  a_value[0u] = HAP_HA_CONTROL_POINT_OP_PRESET_CHANGED;
  a_value[1u] = HAP_PRESET_CHANGE_ID_PRESET_RECORD_UNAVAILABLE;
  a_value[2u] = IsLast;
  a_value[3u] = Index;

  hciCmdResult = aci_gatt_update_char_value_ext(ConnHandle,
                                                pSrvContext->ServiceHandle,
                                                pSrvContext->HearingAidPresetControlPointHandle,
                                                0x02,                           /*Notification*/
                                                4,                              /* Char_Length*/
                                                0,                              /* charValOffset */
                                                4,                              /* charValueLen */
                                                &a_value[0]);
  BLE_DBG_HAS_MSG("Preset Record Unavailable set with status 0x%02X\n", hciCmdResult);

  return hciCmdResult;
}

/**
  * @brief  handles ATT Events for HAS Service .
  * @param  pEvent: pointer on Event
  * @retval indicate if ATT Event has been handled ( returns 1) or not
  *         ( returns 0) by the function.
  */
SVCCTL_EvtAckStatus_t HAS_ATT_Event_Handler(void *pEvent)
{

  SVCCTL_EvtAckStatus_t return_value = SVCCTL_EvtNotAck;
  hci_event_pckt *p_event_pckt;
  evt_blecore_aci *p_blecore_evt;
  HAS_NotificationEvt_t notification;

  p_event_pckt = (hci_event_pckt *)(((hci_uart_pckt*)pEvent)->data);

  if (p_srv_ctxt != 0x00000000u)
  {
    switch (p_event_pckt->evt)
    {
      case HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE:
      {
        p_blecore_evt = (evt_blecore_aci*)p_event_pckt->data;
        BLE_DBG_HAS_MSG("GATT Event in HAS Layer : %04X with code %04X!!\n",p_event_pckt->evt,p_blecore_evt->ecode);
        switch (p_blecore_evt->ecode)
        {
          case ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE:
          {
            aci_gatt_write_permit_req_event_rp0 *attribute_write_req;
            attribute_write_req = (aci_gatt_write_permit_req_event_rp0*)p_blecore_evt->data;
            /*Check if attribute handle is in one of the Hearing Access Service*/
            if ((attribute_write_req->Attribute_Handle >= p_srv_ctxt->ServiceHandle)  \
             && (attribute_write_req->Attribute_Handle <= p_srv_ctxt->ServiceEndHandle))
            {
              return_value = SVCCTL_EvtAckFlowEnable;
              if (attribute_write_req->Attribute_Handle == (p_srv_ctxt->HearingAidPresetControlPointHandle + 1))
              {
                uint8_t write_status = 0x00;
                uint8_t error_code;
                /* Check opcode validity*/
                error_code = HAS_CheckCtrlOpParams(attribute_write_req->Connection_Handle,
                                                   &attribute_write_req->Data[0],
                                                   attribute_write_req->Data_Length);
                if (error_code != 0x00)
                {
                  BLE_DBG_HAS_MSG("-- GATT : Write Request is rejected for reason 0x%02X\n",error_code);
                  write_status = 0x01;
                }
                /* Send Write response */
                aci_gatt_write_resp( attribute_write_req->Connection_Handle,
                                    attribute_write_req->Attribute_Handle,
                                    write_status,
                                    error_code,
                                    attribute_write_req->Data_Length,
                                    attribute_write_req->Data );
              }
            }
          }
          break;/* ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */

          case ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE:
          {
            aci_gatt_attribute_modified_event_rp0    * attribute_modified;
            attribute_modified = (aci_gatt_attribute_modified_event_rp0*)p_blecore_evt->data;
            /*Check if attribute handle is in one of the Hearing Access Service*/
            if ((attribute_modified->Attr_Handle >= p_srv_ctxt->ServiceHandle)  \
             && (attribute_modified->Attr_Handle <= p_srv_ctxt->ServiceEndHandle))
            {
              return_value = SVCCTL_EvtAckFlowEnable;

              if (attribute_modified->Attr_Handle == (p_srv_ctxt->HearingAidPresetControlPointHandle + 1))
              {
                /* Value modified */
                notification.EvtOpcode = HAS_CONTROL_POINT_EVT;
                notification.ConnHandle = attribute_modified->Connection_Handle;
                notification.pData = attribute_modified->Attr_Data;
                notification.DataLen = attribute_modified->Attr_Data_Length;
                HAS_Notification(&notification);
              }

              if (attribute_modified->Attr_Handle == (p_srv_ctxt->HearingAidPresetControlPointHandle + 2))
              {
                /* Descriptor modified */
                notification.EvtOpcode = HAS_CONTROL_POINT_DESC_EVT;
                notification.ConnHandle = attribute_modified->Connection_Handle;
                notification.pData = attribute_modified->Attr_Data;
                notification.DataLen = attribute_modified->Attr_Data_Length;
                HAS_Notification(&notification);
              }
            }
          }
          break;/* ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */

          case ACI_GATT_SERVER_CONFIRMATION_VSEVT_CODE:
          {
            aci_gatt_server_confirmation_event_rp0 * server_confirmation;
            server_confirmation = (aci_gatt_server_confirmation_event_rp0*)p_blecore_evt->data;

            return_value = HAS_HandleCtrlPointIndicateComplete(server_confirmation->Connection_Handle);
          }

          default:
            break;
        }
      }
      break; /* HCI_HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE_SPECIFIC */

      default:
        break;
    }
  }
  return return_value;
}

/* Private functions ----------------------------------------------------------*/
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */