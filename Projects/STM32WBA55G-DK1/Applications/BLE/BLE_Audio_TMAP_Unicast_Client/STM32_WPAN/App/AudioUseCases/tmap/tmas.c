/**
  ******************************************************************************
  * @file    tmas.c
  * @author  MCD Application Team
  * @brief   This file contains Telephony and Media Audio Service
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
#include "tmas.h"
#include "tmap_log.h"
#include "ble_gatt_aci.h"
#include "svc_ctl.h"

/* Private defines -----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/
#define NUM_TMAS_CHARACTERISTIC_RECORDS         (4u)
#define ATT_ERR_VALUE_NOT_ALLOWED               (0x13u)
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static TMAS_ServiceContext_t *p_srv_ctxt = 0;
/* Private functions prototype------------------------------------------------*/
static tBleStatus TMAS_SetCharacteristic(uint16_t ServiceHandle, uint16_t CharHandle, uint8_t *pData, uint16_t DataLen);
/* Functions Definition ------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/
/**
  * @brief  Initialize the Telephony and Media Audio Service
  */
tBleStatus TMAS_InitService(TMAS_ServiceContext_t *pSrvContext)
{
  uint16_t uuid;
  uint8_t num_char_records = NUM_TMAS_CHARACTERISTIC_RECORDS;
  tBleStatus hciCmdResult = BLE_STATUS_SUCCESS;

  memset(pSrvContext, 0, sizeof(TMAS_ServiceContext_t) );
  /**
  *  Add Telephony and Media Audio Service
  */
  uuid = TELEPHONY_AND_MEDIA_AUDIO_SERVICE_UUID;
  hciCmdResult = aci_gatt_add_service(UUID_TYPE_16,
                                       (Service_UUID_t *) &uuid,
                                       PRIMARY_SERVICE,
                                       (1+num_char_records),
                                       &(pSrvContext->ServiceHandle));

  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    pSrvContext->ServiceEndHandle = pSrvContext->ServiceHandle+num_char_records;
    BLE_DBG_TMAS_MSG("Telephony and Media Audio Service (TMAS) is added Successfully %04X (Max_Attribute_Records : %d)\n",
                    pSrvContext->ServiceHandle,
                    (1+num_char_records));
    p_srv_ctxt = pSrvContext;
  }
  else
  {
    BLE_DBG_TMAS_MSG("FAILED to add Telephony and Media Audio Service (TMAS), Error: %02X !!\n",hciCmdResult);
  }
  return hciCmdResult;
}

tBleStatus TMAS_InitCharacteristics(TMAS_ServiceContext_t *pSrvContext)
{
  uint16_t uuid;
  tBleStatus hciCmdResult = BLE_STATUS_SUCCESS;
  /*
  *  Add TMAP Role Characteristic
  */
  uuid = TMAP_ROLE_UUID;
  hciCmdResult = aci_gatt_add_char(pSrvContext->ServiceHandle,
                                  UUID_TYPE_16,
                                  (Char_UUID_t *) &uuid ,
                                  2u,
                                  (CHAR_PROP_READ),
                                  ATTR_PERMISSION_ENCRY_READ,
                                  0,
                                  10,
                                  0,
                                  &(pSrvContext->TMAPRoleHandle));

  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    BLE_DBG_TMAS_MSG("TMAP Role Characteristic added Successfully %04X\n", pSrvContext->TMAPRoleHandle);
  }
  else
  {
    BLE_DBG_TMAS_MSG("FAILED to add TMAP Role Characteristic, Error: %02X !!\n", hciCmdResult);
  }

  return hciCmdResult;
}

tBleStatus TMAS_SetTMAPRole(TMAS_ServiceContext_t *pSrvContext, TMAP_Role_t Role)
{
  tBleStatus hciCmdResult = BLE_STATUS_INVALID_PARAMS;

  if (pSrvContext->TMAPRoleHandle != 0x00u)
  {
    hciCmdResult = TMAS_SetCharacteristic(pSrvContext->ServiceHandle,
                                         pSrvContext->TMAPRoleHandle,
                                         (uint8_t*) &Role,
                                         2);
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      BLE_DBG_TMAS_MSG("TMAP Role Characteristic (%04X) Value updated Successfully\n",
                      pSrvContext->TMAPRoleHandle);
    }
    else
    {
      BLE_DBG_TMAS_MSG("FAILED to update TMAP Role Characteristic (%04X) Value, Error: %02X !!\n",
                      pSrvContext->TMAPRoleHandle,
                      hciCmdResult);
    }
  }
  return hciCmdResult;
}

/**
  * @brief  handles ATT Events for TMAS Service .
  * @param  pEvent: pointer on Event
  * @retval indicate if ATT Event has been handled ( returns 1) or not
  *         ( returns 0) by the function.
  */
uint8_t TMAS_ATT_Event_Handler(void *pEvent)
{

  uint8_t return_value = 0u;
  hci_event_pckt *p_event_pckt;
  evt_blecore_aci *p_blecore_evt;
  //TMAS_NotificationEvt_t notification;

  p_event_pckt = (hci_event_pckt *)(((hci_uart_pckt*)pEvent)->data);

  if (p_srv_ctxt != 0x00000000u)
  {
    switch (p_event_pckt->evt)
    {
      case HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE:
      {
        p_blecore_evt = (evt_blecore_aci*)p_event_pckt->data;
        BLE_DBG_TMAS_MSG("GATT Event in TMAS Layer : %04X with code %04X!!\n",p_event_pckt->evt,p_blecore_evt->ecode);
        switch (p_blecore_evt->ecode)
        {
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

static tBleStatus TMAS_SetCharacteristic(uint16_t ServiceHandle, uint16_t CharHandle, uint8_t *pData,uint16_t DataLen)
{
 tBleStatus hciCmdResult;

  hciCmdResult = aci_gatt_update_char_value_ext(0x0000,
                                                ServiceHandle,
                                                CharHandle,
                                                0x01,                           /*Notification*/
                                                DataLen,                        /* Char_Length*/
                                                0,                              /* charValOffset */
                                                DataLen,                        /* charValueLen */
                                                (uint8_t *) &pData[0]);

  return hciCmdResult;
}