/**
  ******************************************************************************
  * @file    hap_iac.c
  * @author  MCD Application Team
  * @brief   This file contains Hearing Access Profile feature for Immediate
             Alert Client
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
#include "hap_iac.h"
#include "hap.h"
#include "hap_alloc.h"
#include "hap_log.h"
#include "ble_gatt_aci.h"
#include "ble_vs_codes.h"
#include "uuid.h"
#include "hap_db.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
#define UNPACK_2_BYTE_PARAMETER(ptr)  \
        ((uint16_t)((uint16_t)(*((uint8_t *)ptr))) |   \
        (uint16_t)((((uint16_t)(*((uint8_t *)ptr + 1))) << 8)))
/* Private variables ---------------------------------------------------------*/
/* Private functions prototype------------------------------------------------*/
#if (BLE_CFG_HAP_IAC_ROLE == 1u)
static tBleStatus HAP_IAC_Linkup_Process(HAP_IAC_Inst_t *pIAC_Inst,uint8_t ErrorCode);
static void HAP_IAC_Post_Linkup_Event(HAP_IAC_Inst_t *pIAC_Inst, tBleStatus const Status);
tBleStatus HAP_IAC_Check_IAS_Service(HAP_IAC_Inst_t *pIAC_Inst);
static HAP_IAC_Inst_t *HAP_IAC_GetAvailableInstance(void);
static HAP_IAC_Inst_t *HAP_IAC_GetInstance(uint16_t ConnHandle);
static void HAP_IAC_InitInstance(HAP_IAC_Inst_t *pIAC_Inst);
#endif /* (BLE_CFG_HAP_IAC_ROLE == 1u) */
/* External functions prototype------------------------------------------------*/

/* Functions Definition ------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/

#if (BLE_CFG_HAP_IAC_ROLE == 1u)
/**
  * @brief  Initialize the Hearing Access Profile as Immmediate Alert Client Role
  * @retval status of the initialization
  */
tBleStatus HAP_IAC_Init(void)
{
  uint8_t i;

  for (i = 0; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
  {
    HAP_IAC_InitInstance(&HAP_Context.aIACInst[i]);
  }

  return BLE_STATUS_SUCCESS;
}
#endif /* (BLE_CFG_HAP_IAC_ROLE == 1u) */

/**
  * @brief Link Up the HAP IAC with remote HAP HA
  * @param  ConnHandle: Connection handle
  * @param  LinkupMode: LinkUp Mode
  * @note IAC_LINKUP_COMPLETE_EVT event will be generated once process is complete
  * @retval status of the operation
  */
tBleStatus HAP_IAC_Linkup(uint16_t ConnHandle, HAP_LinkupMode_t LinkupMode)
{
  tBleStatus    hciCmdResult = HCI_COMMAND_DISALLOWED_ERR_CODE;
#if (BLE_CFG_HAP_IAC_ROLE == 1u)
  HAP_IAC_Inst_t  *p_iac_inst;

  if (HAP_Context.Role & HAP_ROLE_IMMEDIATE_ALERT_CLIENT)
  {
    BLE_DBG_HAP_IAC_MSG("Start HAP IAC Link Up procedure on ACL Connection Handle 0x%04X\n", ConnHandle);
    /* Check if a HAP Client Instance with specified Connection Handle is already allocated*/
    p_iac_inst = HAP_IAC_GetInstance(ConnHandle);
    if (p_iac_inst == 0)
    {
      /*Get an available HAP Client Instance*/
      p_iac_inst = HAP_IAC_GetAvailableInstance();
      if (p_iac_inst != 0)
      {
        p_iac_inst->ConnHandle = ConnHandle;
        /*Check that HAP LinkUp process is not already started*/
        if (p_iac_inst->LinkupState == HAP_IAC_LINKUP_IDLE)
        {
          p_iac_inst->LinkupMode = LinkupMode;

          if (p_iac_inst->LinkupMode == HAP_LINKUP_MODE_COMPLETE)
          {
            /* First step of Link Up process : find the IAS in the remote GATT Database*/
            hciCmdResult = aci_gatt_disc_all_primary_services(p_iac_inst->ConnHandle);
            BLE_DBG_HAP_IAC_MSG("aci_gatt_disc_all_primary_services() returns status 0x%x\n", hciCmdResult);
            if (hciCmdResult == BLE_STATUS_SUCCESS)
            {
              p_iac_inst->AttProcStarted = 1u;
              /*Start Immediate Alert Service Linkup*/
              p_iac_inst->LinkupState = HAP_IAC_LINKUP_DISC_SERVICE;
            }
            else
            {
              HAP_IAC_InitInstance(p_iac_inst);
            }
          }
          else if (p_iac_inst->LinkupMode == HAP_LINKUP_MODE_RESTORE)
          {
            /*IAS shall be restored from Service Database*/
            hciCmdResult = HAP_IAC_DB_RestoreClientDatabase(p_iac_inst);
            if (hciCmdResult != BLE_STATUS_SUCCESS)
            {
              BLE_DBG_HAP_IAC_MSG("HAP Link restoration has failed\n");
              HAP_IAC_InitInstance(p_iac_inst);
            }
          }
        }
        else if (p_iac_inst->LinkupState == HAP_IAC_LINKUP_COMPLETE)
        {
          /*HAP Link Up is already performed*/
          BLE_DBG_HAP_IAC_MSG("HAP Link Up is already performed\n");
          hciCmdResult = HCI_COMMAND_DISALLOWED_ERR_CODE;
        }
        else
        {
          BLE_DBG_HAP_IAC_MSG("HAP Link Up process is already in progress\n");
          /*Microphone Control Link Up process is already in progress*/
          hciCmdResult = BLE_STATUS_BUSY;
        }
      }
      else
      {
        BLE_DBG_HAP_IAC_MSG("No ressource to use a HAP IAC Instance\n");
        hciCmdResult = BLE_STATUS_FAILED;
      }
    }
    else
    {
      BLE_DBG_HAP_IAC_MSG("HAP IAC Instance is already associated to the connection handle 0x%04X\n",ConnHandle);
      if (p_iac_inst->LinkupState == HAP_IAC_LINKUP_COMPLETE)
      {
        /*HAP Link Up is already performed*/
        hciCmdResult = HCI_COMMAND_DISALLOWED_ERR_CODE;
      }
      else
      {
        /* HAP Link Up process is already in progress*/
        hciCmdResult = BLE_STATUS_BUSY;
      }
    }
  }
  else
  {
    BLE_DBG_HAP_IAC_MSG("HAP Linkup aborted because Immediate Alert Client Role is not registered\n");
  }
#endif /* (BLE_CFG_HAP_IAC_ROLE == 1u) */
  return hciCmdResult;
}

/**
  * @brief Set Alert Level on remote Immediate Alert Server
  * @param ConnHandle: The connection handle of the remote IAS Server
  * @param AlertLevel: The Level of the alert
  * @return Status of the operation
  */
tBleStatus HAP_IAC_SetAlertLevel(uint16_t ConnHandle, HAP_IAC_AlertLevel_t AlertLevel)
{
  tBleStatus hciCmdResult = BLE_STATUS_INVALID_PARAMS;
#if (BLE_CFG_HAP_IAC_ROLE == 1u)
  if (HAP_Context.Role & HAP_ROLE_IMMEDIATE_ALERT_CLIENT)
  {
    HAP_IAC_Inst_t *p_iac_inst;
    BLE_DBG_HAP_IAC_MSG("Start Set Alert Level %d on connection handle 0x%04X\n", AlertLevel, ConnHandle);

    p_iac_inst = HAP_IAC_GetInstance(ConnHandle);
    if (p_iac_inst != 0)
    {
      if (p_iac_inst->AttProcStarted == 0)
      {
        if (p_iac_inst->LinkupState == HAP_IAC_LINKUP_COMPLETE)
        {
          if (p_iac_inst->AlertLevelChar.ValueHandle != 0)
          {
            uint8_t a_value[1] = {
              AlertLevel
            };

            hciCmdResult = aci_gatt_write_without_resp(ConnHandle,
                                                       p_iac_inst->AlertLevelChar.ValueHandle,
                                                       1,
                                                       &a_value[0]);
            BLE_DBG_HAP_IAC_MSG("aci_gatt_write_without_resp() (connHandle: %04X, val_handle: %04X) returns status 0x%x\n",
                                ConnHandle,
                                p_iac_inst->AlertLevelChar.ValueHandle,
                                hciCmdResult);
            if (hciCmdResult == BLE_STATUS_SUCCESS)
            {
              BLE_DBG_HAP_IAC_MSG("Start HAP_IAC_OP_SET_ALERT_LEVEL Operation(ConnHandle 0x%04X)\n", ConnHandle);
            }
          }
        }
        else
        {
          BLE_DBG_HAP_IAC_MSG("HAP Linkup is not complete on Connection Handle 0x%04X\n", ConnHandle);
          return HCI_COMMAND_DISALLOWED_ERR_CODE;
        }
      }
      else
      {
        BLE_DBG_HAP_IAC_MSG("An ATT Procedure is ongoing on Connection Handle 0x%04X\n", ConnHandle);
        return BLE_STATUS_BUSY;
      }
    }
    else
    {
      BLE_DBG_HAP_IAC_MSG("Connection Handle 0x%04X doesn't correspond to an allocated HAP Client Instance\n", ConnHandle);
      hciCmdResult = BLE_STATUS_INVALID_PARAMS;
    }
  }
  else
  {
#endif /* (BLE_CFG_HAP_IAC_ROLE == 1u) */
    hciCmdResult = HCI_COMMAND_DISALLOWED_ERR_CODE;
#if (BLE_CFG_HAP_IAC_ROLE == 1u)
  }
#endif /* (BLE_CFG_HAP_IAC_ROLE == 1u) */

  return hciCmdResult;
}

#if (BLE_CFG_HAP_IAC_ROLE == 1u)
/**
   * @brief  Store GATT Database of the HAP IAS Characteristics and Services
   * @param  ConnHandle: Connetion Handle of the remote connection
   * @param  [in] pData: pointer on buffer in which GATT Database of HAP characteristics in which information is stored
   * @param  [in] MaxDataLen: maximum data length to store
   * @param  [out] len : length in bytes of stored data
   * @retval status of the operation
   */
tBleStatus HAP_IAC_StoreDatabase(uint16_t ConnHandle, uint8_t *pData, uint16_t MaxDataLen, uint16_t *len)
{
  tBleStatus                    status = BLE_STATUS_SUCCESS;
  uint16_t                      remain = MaxDataLen;
  HAP_IAC_Inst_t *p_iac_inst;

  p_iac_inst = HAP_IAC_GetInstance(ConnHandle);
  if (p_iac_inst != 0)
  {
    *len = 0u;

    if (p_iac_inst->LinkupState == HAP_IAC_LINKUP_COMPLETE)
    {
      /* Store IAS Service information*/
      if (remain >= 8u)
      {
        /* Store :
         * - 2 bytes for IAS Service Start Handle Attribute handle
         * - 1 byte for Characteristic type
         * - 2 bytes for IAS Service UUID
         * - 1 byte to indicate data length
         */
        /* IAS Service Start Attribute handle */
        pData[(*len)] = (uint8_t) (p_iac_inst->IASServiceStartHandle);
        pData[(*len)+1] = (uint8_t) ((p_iac_inst->IASServiceStartHandle >> 8 ));
        /* Characteristic Type*/
        pData[(*len)+2] = UUID_TYPE_16;
        /* IAS UUID*/
        pData[(*len)+3] = (uint8_t) IMMEDIATE_ALERT_SERVICE_UUID;
        pData[(*len)+4] = (uint8_t) ((IMMEDIATE_ALERT_SERVICE_UUID >> 8 ));
        /*value length*/
        pData[(*len)+5] = 2;
        /*save Service End Handle*/
        pData[(*len)+6] = (uint8_t) (p_iac_inst->IASServiceEndHandle);
        pData[(*len)+7] = (uint8_t) ((p_iac_inst->IASServiceEndHandle >> 8 ));
        remain -= 8u;
        *len += 8u;
      }
      else
      {
        BLE_DBG_HAP_IAC_MSG("No sufficient resource to store data of IAS Service\n");
        status = BLE_STATUS_INSUFFICIENT_RESOURCES;
      }

      /* Store Alert Level information*/
      if (status == BLE_STATUS_SUCCESS)
      {
        if (remain >= 6u)
        {
          /* Store :
           * - 2 bytes for Alert Level attribute handle
           * - 1 byte for Characteristic type
           * - 2 bytes for Alert Level UUID
           * - 1 byte to indicate data length
           */
          /* Alert Level Characteristic Attribute handle */
          BLE_DBG_HAP_IAC_MSG("Store Alert Level Characteristic at att handle 0x%04X\n",
                               p_iac_inst->AlertLevelChar.ValueHandle);
          pData[(*len)] = (uint8_t) (p_iac_inst->AlertLevelChar.ValueHandle);
          pData[(*len)+1] = (uint8_t) ((p_iac_inst->AlertLevelChar.ValueHandle>> 8 ));
          /* Characteristic Type*/
          pData[(*len)+2] = UUID_TYPE_16;
          /* Alert Level Charactertistic UUID*/
          pData[(*len)+3] = (uint8_t) ALERT_LEVEL_CHARACTERISTIC_UUID;
          pData[(*len)+4] = (uint8_t) ((ALERT_LEVEL_CHARACTERISTIC_UUID >> 8 ));
          /*value length*/
          pData[(*len)+5] = 0;
          *len += 6u;
        }
        else
        {
          BLE_DBG_HAP_IAC_MSG("No sufficient resource to store data of Alert Level Characteristic\n");
          status = BLE_STATUS_INSUFFICIENT_RESOURCES;
        }
      }
    }
  }
  return status;
}

/**
   * @brief  Restore GATT Database of the IAS characteristics
   * @param  pIAC_Inst: pointer on IAS Client Instance
   * @param  pData: pointer on buffer including GATT Database of IAS characteristics
   * @param  len : length in bytes of stored data
   * @retval status of the operation
   */
tBleStatus HAP_IAC_RestoreDatabase(HAP_IAC_Inst_t *pIAC_Inst, uint8_t *pData,uint16_t Len)
{
  tBleStatus            status = BLE_STATUS_SUCCESS;
  uint16_t              data_index = 0u;
  uint16_t              att_handle;
  uint8_t               uuid_type;
  uint16_t              uuid_value;
  uint8_t               data_len;

  BLE_DBG_HAP_IAC_MSG("Restore IAS Service for conn handle 0x%04X\n", pIAC_Inst->ConnHandle);

  while ((data_index < Len) && (status == BLE_STATUS_SUCCESS))
  {
    att_handle = pData[data_index] | (pData[data_index+1u] << 8);
    uuid_type = pData[data_index+2u];
    uuid_value = pData[data_index+3u] | (pData[data_index+4u] << 8);
    data_len = pData[data_index+5u];

    if ((uuid_type == UUID_TYPE_16) && (uuid_value == IMMEDIATE_ALERT_SERVICE_UUID))
    {
      pIAC_Inst->IASServiceStartHandle = att_handle;
      pIAC_Inst->IASServiceEndHandle = pData[data_index+6u] | (pData[data_index+7u] << 8);
      BLE_DBG_HAP_IAC_MSG("Restore IAS Service information : Start Handle 0x%04X, Start Handle 0x%04X\n",
                           pIAC_Inst->IASServiceStartHandle,
                           pIAC_Inst->IASServiceEndHandle);
    }
    else if ((uuid_type == UUID_TYPE_16) && (uuid_value == ALERT_LEVEL_CHARACTERISTIC_UUID) && (status == BLE_STATUS_SUCCESS))
    {
      /* Alert Level Characteristic */
      BLE_DBG_HAP_IAC_MSG("Restore Alert Level Characteristic:\n");
      BLE_DBG_HAP_IAC_MSG("  Handle = 0x%04X\n", att_handle);

      pIAC_Inst->AlertLevelChar.ValueHandle = att_handle;
      pIAC_Inst->AlertLevelChar.Properties = pData[data_index+6u];
      pIAC_Inst->AlertLevelChar.DescHandle = pData[data_index+7u] | (pData[data_index+8u] << 8);
      pIAC_Inst->AlertLevelChar.EndHandle = pIAC_Inst->IASServiceEndHandle;
    }
    data_index += (data_len + 6u);
  }

  if (status == BLE_STATUS_SUCCESS)
  {
    BLE_DBG_HAP_IAC_MSG("Restore IAS Service is complete with success\n");

    pIAC_Inst->LinkupState = HAP_IAC_LINKUP_COMPLETE;
  }
  else
  {
    BLE_DBG_HAP_IAC_MSG("Restore IAS Service is complete with failure\n");
  }
  return status;
}

/**
  * @brief  Notify ACL Disconnection to HAP IAC
  * @param  ConnHandle: ACL Connection Handle
  */
void HAP_IAC_AclDisconnection(uint16_t ConnHandle)
{
  HAP_IAC_Inst_t *p_iac_inst;
  /* Check if a HAP Client Instance with specified Connection Handle is already allocated*/
  p_iac_inst = HAP_IAC_GetInstance(ConnHandle);
  if (p_iac_inst != 0)
  {
    BLE_DBG_HAP_IAC_MSG("ACL Disconnection on Connection Handle 0x%04X : Reset HAP IAC Instance\n",ConnHandle);
    /*Check if a HAP Linkup procedure is in progress*/
    if ((p_iac_inst->LinkupState != HAP_IAC_LINKUP_IDLE) && (p_iac_inst->LinkupState != HAP_IAC_LINKUP_COMPLETE))
    {
      /*Notify that HAP Link Up is complete*/
      HAP_IAC_NotificationEvt_t evt;
      evt.ConnHandle = p_iac_inst->ConnHandle;
      evt.Status = BLE_STATUS_FAILED;
      evt.EvtOpcode = (HAP_IAC_NotCode_t) IAC_LINKUP_COMPLETE_EVT;
      evt.pInfo = 0;
      HAP_IAC_Notification(&evt);
      if (p_iac_inst->AttProcStarted == 0u)
      {
        HAP_IAC_InitInstance(p_iac_inst);
      }
      else
      {
        /* Wait ACI_GATT_PROC_COMPLETE_VSEVT_CODE event to deallocate the HAP Client Instance*/
        p_iac_inst->DelayDeallocation = 1u;
      }
    }
    else
    {
      /* Reset HAP Instance */
      if (p_iac_inst->AttProcStarted == 0u)
      {
        HAP_IAC_InitInstance(p_iac_inst);
      }
      else
      {
        /* Wait ACI_GATT_PROC_COMPLETE_VSEVT_CODE event to deallocate the HAP Client Instance*/
        p_iac_inst->DelayDeallocation = 1u;
      }
    }
  }
}

/** @brief This function is used by the Device in the HAP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */
SVCCTL_EvtAckStatus_t HAP_IAC_GATT_Event_Handler(void *pEvent)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *p_event_pckt;
  return_value = SVCCTL_EvtNotAck;
  p_event_pckt = (hci_event_pckt *)(((hci_uart_pckt*)pEvent)->data);

  switch (p_event_pckt->evt)
  {
    case HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE:
    {
      evt_blecore_aci *p_blecore_evt = (evt_blecore_aci*)p_event_pckt->data;
      HAP_IAC_Inst_t *p_iac_inst;
      switch (p_blecore_evt->ecode)
      {
        case ACI_ATT_READ_BY_GROUP_TYPE_RESP_VSEVT_CODE:
        {
          aci_att_read_by_group_type_resp_event_rp0 *pr = (void*)p_blecore_evt->data;
          uint8_t numServ, i, idx;
          uint16_t uuid;

          /* Check if a HAP Instance with specified Connection Handle exists*/
          p_iac_inst = HAP_IAC_GetInstance(pr->Connection_Handle);
          if (p_iac_inst != 0)
          {
            BLE_DBG_HAP_IAC_MSG("ACI_ATT_READ_BY_GROUP_TYPE_RESP_EVENT is received on conn handle %04X\n",
                             pr->Connection_Handle);

            /*Check that HAP Link Up Process State is in Service Discovery state*/
            if ((p_iac_inst->LinkupState & HAP_IAC_LINKUP_DISC_SERVICE) == HAP_IAC_LINKUP_DISC_SERVICE)
            {
              return_value = SVCCTL_EvtAckFlowEnable;
              numServ = (pr->Data_Length) / pr->Attribute_Data_Length;
              BLE_DBG_HAP_IAC_MSG("Number of services in the GATT response : %d\n",numServ);
              if (pr->Attribute_Data_Length == 6)
              {
                idx = 4;
                for (i=0; i<numServ; i++)
                {
                  uuid = UNPACK_2_BYTE_PARAMETER(&pr->Attribute_Data_List[idx]);
                  /*Check that UUID in the GATT response corresponds to the IAS */
                  if (uuid == IMMEDIATE_ALERT_SERVICE_UUID)
                  {
                    /* Save start handle and the end handle of the IAS
                     * for the next step of the HAP Link Up process
                     */
                    p_iac_inst->IASServiceStartHandle = UNPACK_2_BYTE_PARAMETER(&pr->Attribute_Data_List[idx-4]);
                    p_iac_inst->IASServiceEndHandle = UNPACK_2_BYTE_PARAMETER (&pr->Attribute_Data_List[idx-2]);
                    BLE_DBG_HAP_IAC_MSG("Immediate Alert Service has been found (start: %04X, end: %04X)\n",
                                        p_iac_inst->IASServiceStartHandle,
                                        p_iac_inst->IASServiceEndHandle);
                  }
                  idx += 6;
                }
              }
            }
          }
        }
        break;

        case ACI_ATT_READ_BY_TYPE_RESP_VSEVT_CODE:
        {
          aci_att_read_by_type_resp_event_rp0 *pr = (void*)p_blecore_evt->data;
          uint8_t idx;
          uint16_t uuid, handle, start_handle;
          uint8_t data_length;

          /* Check if a HAP Client Instance with specified Connection Handle exists*/
          p_iac_inst = HAP_IAC_GetInstance(pr->Connection_Handle);
          if (p_iac_inst != 0)
          {
            /* the event data will be
             * 2 bytes start handle
             * 1 byte char properties
             * 2 bytes handle
             * 2 or 16 bytes data
             */
            BLE_DBG_HAP_IAC_MSG("ACI_ATT_READ_BY_TYPE_RESP_EVENT is received on connHandle %04X\n",pr->Connection_Handle);

            /*Check that HAP Link Up Process State is in Characteristics Discovery state*/
            if ((p_iac_inst->LinkupState & HAP_IAC_LINKUP_DISC_CHAR) == HAP_IAC_LINKUP_DISC_CHAR)
            {
              idx = 5;
              data_length = pr->Data_Length;
              if (pr->Handle_Value_Pair_Length == 7)
              {
                data_length -= 1;
                /*Check if characteristic handle corresponds to the Immediate Alert Service range */
                if (((UNPACK_2_BYTE_PARAMETER(&pr->Handle_Value_Pair_Data[0u])>= p_iac_inst->IASServiceStartHandle)
                     && (UNPACK_2_BYTE_PARAMETER(&pr->Handle_Value_Pair_Data[0u]) <= p_iac_inst->IASServiceEndHandle)))
                {
                  return_value = SVCCTL_EvtAckFlowEnable;
                  while (idx < data_length)
                  {
                    /* extract the characteristic UUID */
                    uuid = UNPACK_2_BYTE_PARAMETER(&pr->Handle_Value_Pair_Data[idx]);
                    /*  extract the characteristic handle */
                    handle = UNPACK_2_BYTE_PARAMETER(&pr->Handle_Value_Pair_Data[idx-2]);
                    /*  extract the start handle of the characteristic */
                    start_handle = UNPACK_2_BYTE_PARAMETER(&pr->Handle_Value_Pair_Data[idx-5]);

                    switch (uuid)
                    {
                      case ALERT_LEVEL_CHARACTERISTIC_UUID:
                        BLE_DBG_HAP_IAC_MSG("Alert Level Characteristic has been found:\n");
                        BLE_DBG_HAP_IAC_MSG("Attribute Handle = %04X\n",start_handle);
                        BLE_DBG_HAP_IAC_MSG("Characteristic Properties = 0x%02X\n",pr->Handle_Value_Pair_Data[idx-3]);
                        BLE_DBG_HAP_IAC_MSG("Handle = 0x%04X\n",handle);
                        p_iac_inst->AlertLevelChar.ValueHandle = handle;
                        p_iac_inst->AlertLevelChar.EndHandle = p_iac_inst->IASServiceEndHandle;
                        p_iac_inst->AlertLevelChar.Properties = pr->Handle_Value_Pair_Data[idx-3];;
                      break;

                    default:
                      break;
                    }
                    idx += pr->Handle_Value_Pair_Length;
                  }
                }
              }
            }
          }
          else
          {
            BLE_DBG_HAP_IAC_MSG("No HAP Client allocated for this connection handle\n");
          }
        }
        break;

        case ACI_GATT_PROC_COMPLETE_VSEVT_CODE:
        {
          aci_gatt_proc_complete_event_rp0 *pr = (void*)p_blecore_evt->data;
          /* Check if a HAP Client Instance with specified Connection Handle exists*/
          p_iac_inst = HAP_IAC_GetInstance(pr->Connection_Handle);
          if (p_iac_inst != 0)
          {
            BLE_DBG_HAP_IAC_MSG("ACI_GATT_PROC_COMPLETE_EVENT is received on conn handle %04X (ErrorCode %04X)\n",
                                pr->Connection_Handle,
                                pr->Error_Code);

            /* Check if an ATT Procedure was started*/
            if (p_iac_inst->AttProcStarted != 0u)
            {
              p_iac_inst->AttProcStarted = 0u;
              return_value = SVCCTL_EvtAckFlowEnable;
            }
            /*Check if a HAP Linkup procedure is in progress*/
            if ((p_iac_inst->LinkupState != HAP_IAC_LINKUP_IDLE) && (p_iac_inst->LinkupState != HAP_IAC_LINKUP_COMPLETE))
            {
              if (p_iac_inst->DelayDeallocation == 0u)
              {
                /* GATT Process is complete, continue, if needed, the HAP Link Up Process */
                HAP_IAC_Linkup_Process(p_iac_inst,pr->Error_Code);
              }
            }

            if ((p_iac_inst->DelayDeallocation == 1u) && (p_iac_inst->AttProcStarted == 0u))
            {
              BLE_DBG_HAP_IAC_MSG("Free Completely the HAP Client on conn handle %04X\n",pr->Connection_Handle);
              p_iac_inst->DelayDeallocation = 0u;
              HAP_IAC_InitInstance(p_iac_inst);
            }
          }
        }
        break; /*ACI_GATT_PROC_COMPLETE_VSEVT_CODE*/

        case ACI_GATT_ERROR_RESP_VSEVT_CODE:
        {
          aci_gatt_error_resp_event_rp0 *pr = (void*)p_blecore_evt->data;
          /* Check if a HAP Client Instance with specified Connection Handle exists*/
          p_iac_inst = HAP_IAC_GetInstance(pr->Connection_Handle);
          if (p_iac_inst != 0)
          {
            BLE_DBG_HAP_IAC_MSG("ACI_GATT_ERROR_RESP_EVENT is received on conn handle %04X\n",pr->Connection_Handle);
            if (pr->Attribute_Handle >= p_iac_inst->IASServiceStartHandle
                && pr->Attribute_Handle <= p_iac_inst->IASServiceEndHandle)
            {
              return_value = SVCCTL_EvtAckFlowEnable;
            }
            else
            {
              if (((p_iac_inst->LinkupState & HAP_IAC_LINKUP_DISC_SERVICE) == HAP_IAC_LINKUP_DISC_SERVICE) && (pr->Req_Opcode == 0x10u))
              {
                return_value = SVCCTL_EvtAckFlowEnable;
              }
              if ((((p_iac_inst->LinkupState & HAP_IAC_LINKUP_DISC_SERVICE) == HAP_IAC_LINKUP_DISC_SERVICE) && (pr->Req_Opcode == 0x10u)) \
                || (((p_iac_inst->LinkupState & HAP_IAC_LINKUP_DISC_SERVICE) == HAP_IAC_LINKUP_DISC_SERVICE) && (pr->Req_Opcode == 0x08u)) \
                || (((p_iac_inst->LinkupState & HAP_IAC_LINKUP_DISC_CHAR) == HAP_IAC_LINKUP_DISC_CHAR) && (pr->Req_Opcode == 0x08u)))
              {
                /* Error response returned after :
                 * aci_gatt_disc_all_primary_services()
                 * -aci_gatt_find_included_services()
                 * -aci_gatt_disc_all_char_of_service()
                 */
                return_value = SVCCTL_EvtAckFlowEnable;
              }
            }
          }
        }
        break; /*ACI_GATT_ERROR_RESP_VSEVT_CODE*/

        default:
        break;
      }
    }
  }
  return return_value;
}

/* Private functions ----------------------------------------------------------*/

/**
 * @brief HAP Link Up process
 * @param  pIAC_Inst: pointer on HAP Client Instance
 * @param  ErrorCode: Error Code from Host Stack when a GATT procedure is complete
 * @retval status of the operation
 */
static tBleStatus HAP_IAC_Linkup_Process(HAP_IAC_Inst_t *pIAC_Inst,uint8_t ErrorCode)
{
  tBleStatus hciCmdResult = BLE_STATUS_FAILED;

  BLE_DBG_HAP_IAC_MSG("HAP Link Up Process, state 0x%x\n",pIAC_Inst->LinkupState);

  /*check that error indicate that linkup process succeeds*/
  if (ErrorCode == 0x00)
  {
    if (pIAC_Inst->LinkupMode == HAP_LINKUP_MODE_COMPLETE)
    {
      if ((pIAC_Inst->LinkupState & HAP_IAC_LINKUP_DISC_SERVICE) == HAP_IAC_LINKUP_DISC_SERVICE)
      {
        pIAC_Inst->LinkupState &= ~HAP_IAC_LINKUP_DISC_SERVICE;
        /* HAP Link Up process is in first Step :
         * IAS discovery in the remote GATT Database
         */
        if (pIAC_Inst->IASServiceEndHandle != 0x0000)
        {
          /* Immediate Alert Service has been found */
          BLE_DBG_HAP_IAC_MSG("GATT : Discover IAS Characteristics\n");
          /* Discover all the characteristics of the IAS in the remote GATT Database */
          hciCmdResult = aci_gatt_disc_all_char_of_service(pIAC_Inst->ConnHandle,
                                                          pIAC_Inst->IASServiceStartHandle,
                                                          pIAC_Inst->IASServiceEndHandle);
          BLE_DBG_HAP_IAC_MSG("aci_gatt_disc_all_char_of_service() (start: %04X, end: %04X) returns status 0x%x\n",
                              pIAC_Inst->IASServiceStartHandle,
                              pIAC_Inst->IASServiceEndHandle,
                              hciCmdResult);

          if (hciCmdResult == BLE_STATUS_SUCCESS)
          {
            pIAC_Inst->AttProcStarted = 1u;
            pIAC_Inst->LinkupState |= HAP_IAC_LINKUP_DISC_CHAR;
          }
          else
          {
            /*Notify that HAP Link Up is complete*/
            HAP_IAC_Post_Linkup_Event(pIAC_Inst,hciCmdResult);
          }
        }
        else
        {
          BLE_DBG_HAP_IAC_MSG("No IAS is present in the remote GATT Database\n");
          /*Notify that HAP Link Up is complete*/
          HAP_IAC_Post_Linkup_Event(pIAC_Inst,BLE_STATUS_FAILED);
        }
      }
      else if ((pIAC_Inst->LinkupState & HAP_IAC_LINKUP_DISC_CHAR) == HAP_IAC_LINKUP_DISC_CHAR)
      {
          /* Linkup process end */
          pIAC_Inst->LinkupState &= ~HAP_IAC_LINKUP_DISC_CHAR;
          /*Notify that IAS Link Up is complete*/
          HAP_IAC_Post_Linkup_Event(pIAC_Inst,BLE_STATUS_SUCCESS);
      }
    }
  }
  else
  {
    if (pIAC_Inst->LinkupMode == HAP_LINKUP_MODE_RESTORE)
    {
      /*Notify that HAP Link Up is complete*/
      HAP_IAC_NotificationEvt_t evt;
      evt.Status = BLE_STATUS_FAILED;
      evt.ConnHandle = pIAC_Inst->ConnHandle;
      evt.pInfo = 0;
      evt.EvtOpcode = (HAP_IAC_NotCode_t) IAC_LINKUP_COMPLETE_EVT;
      HAP_IAC_Notification(&evt);
      /*reset the HAP Client Instance*/
      HAP_IAC_InitInstance(pIAC_Inst);
    }
    else
    {
      /*Notify that HAP Link Up is complete*/
      HAP_IAC_Post_Linkup_Event(pIAC_Inst,BLE_STATUS_FAILED);
    }
  }
  return hciCmdResult;
}

/**
 * @brief Notify to upper layer that HAP Link Up Process is complete
 * @param  pIAC_Inst: pointer on HAP Client Instance
 * @param  Status : status of the operation
 */
static void HAP_IAC_Post_Linkup_Event(HAP_IAC_Inst_t *pIAC_Inst, tBleStatus const Status)
{
  HAP_IAC_NotificationEvt_t evt;
  BLE_DBG_HAP_IAC_MSG("HAP Client notifies HAP Linkup is complete with status 0x%x\n",Status);
  evt.ConnHandle = pIAC_Inst->ConnHandle;
  evt.Status = Status;

  pIAC_Inst->LinkupState = HAP_IAC_LINKUP_COMPLETE;
  evt.EvtOpcode = (HAP_IAC_NotCode_t) IAC_LINKUP_COMPLETE_EVT;
  if (evt.Status == BLE_STATUS_SUCCESS)
  {
    HAP_AttServiceInfo_Evt_t parms;
    parms.StartAttHandle = pIAC_Inst->IASServiceStartHandle;
    parms.EndAttHandle = pIAC_Inst->IASServiceEndHandle;
    evt.pInfo = (uint8_t *) &parms;
  }
  else
  {
    evt.pInfo = 0;
  }
  HAP_IAC_Notification(&evt);
  if (evt.Status != BLE_STATUS_SUCCESS)
  {
    BLE_DBG_HAP_IAC_MSG("Linkup has failed, reset the HAP Client Instance\n");
    HAP_IAC_InitInstance(pIAC_Inst);
  }
}

/**
 * @brief Check IAS Validity
 * @param  pIAC_Inst: pointer on HAP Client Instance
 * @retval status of the operation
 */
tBleStatus HAP_IAC_Check_IAS_Service(HAP_IAC_Inst_t *pIAC_Inst)
{
  tBleStatus status = BLE_STATUS_SUCCESS;

  /* Check Hearing Aid Features characteristic existence */
  if (pIAC_Inst->AlertLevelChar.ValueHandle == 0x0000u)
  {
    BLE_DBG_HAP_IAC_MSG("Error : Alert Level Chararacteristic is not found in remote GATT Database\n");
    status = BLE_STATUS_FAILED;
  }
  /* Check Hearing Aid Preset Control Point characteristic properties */
  else if (pIAC_Inst->AlertLevelChar.ValueHandle != 0x0000u
           && ((pIAC_Inst->AlertLevelChar.Properties & CHAR_PROP_WRITE) == 0))
  {
    BLE_DBG_HAP_IAC_MSG("Error : Properties 0x%02X of the HA Preset Control Point Characteristic is not valid\n",
                pIAC_Inst->AlertLevelChar.Properties);
    status = BLE_STATUS_FAILED;
  }
  if (status == BLE_STATUS_SUCCESS)
  {
    BLE_DBG_HAP_IAC_MSG("All IAS Characteristics are valids\n");
  }
  return status;
}

/**
  * @brief Initialize a HAP Client Instance
  * @param pIAC_Inst: pointer on HAP Client Instance
  */
static void HAP_IAC_InitInstance(HAP_IAC_Inst_t *pIAC_Inst)
{
  BLE_DBG_HAP_IAC_MSG("Initialize HAP Client Instance\n");
  /*Initialize the HAP_IAC_Inst_t structure*/
  pIAC_Inst->LinkupState = HAP_IAC_LINKUP_IDLE;
  pIAC_Inst->ReqHandle = 0x0000;
  pIAC_Inst->ConnHandle = 0xFFFF;
  pIAC_Inst->AttProcStarted = 0u;
  pIAC_Inst->DelayDeallocation = 0u;
  pIAC_Inst->AlertLevelChar.ValueHandle = 0x0000u;
  pIAC_Inst->AlertLevelChar.DescHandle = 0x0000u;
  pIAC_Inst->AlertLevelChar.Properties = 0x00u;
  pIAC_Inst->AlertLevelChar.EndHandle = 0x0000u;
  pIAC_Inst->IASServiceStartHandle = 0x0000;
  pIAC_Inst->IASServiceEndHandle = 0x0000;
  pIAC_Inst->AlertLevelChar.ValueHandle = 0x0000u;
  pIAC_Inst->AlertLevelChar.DescHandle = 0x0000u;
  pIAC_Inst->AlertLevelChar.Properties = 0x00u;
  pIAC_Inst->AlertLevelChar.EndHandle = 0x0000u;
  pIAC_Inst->IASServiceStartHandle = 0x0000;
  pIAC_Inst->IASServiceEndHandle = 0x0000;
  pIAC_Inst->ErrorCode = 0x00u;
}


static HAP_IAC_Inst_t *HAP_IAC_GetAvailableInstance(void)
{
  uint8_t i;

  /*Get a free HAP Client Instance*/
  for (i = 0 ; i < USECASE_DEV_MGMT_MAX_CONNECTION ; i++)
  {
    if (HAP_Context.aIACInst[i].ConnHandle == 0xFFFFu)
    {
      return &HAP_Context.aIACInst[i];
    }
  }
  return 0u;
}

static HAP_IAC_Inst_t *HAP_IAC_GetInstance(uint16_t ConnHandle)
{
  uint8_t i;

  /*Get a free HAP Client Instance*/
  for (i = 0 ; i < USECASE_DEV_MGMT_MAX_CONNECTION ; i++)
  {
    if (HAP_Context.aIACInst[i].ConnHandle == ConnHandle)
    {
      return &HAP_Context.aIACInst[i];
    }
  }
  return 0u;
}
#endif /* (BLE_CFG_HAP_IAC_ROLE == 1u) */