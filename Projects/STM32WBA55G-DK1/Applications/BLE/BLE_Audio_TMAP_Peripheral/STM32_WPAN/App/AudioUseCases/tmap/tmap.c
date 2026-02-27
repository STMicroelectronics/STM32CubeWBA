/**
  ******************************************************************************
  * @file    tmap.c
  * @author  MCD Application Team
  * @brief   This file contains Telephony and Media Audio Profile feature
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
#include "cmsis_compiler.h"
#include "tmap_log.h"
#include "tmap_alloc.h"
#include "tmap.h"
#include "tmas.h"
#include "tmap_db.h"
#include "tmap_config.h"
#include "cap.h"
#include "ble_gatt_aci.h"
#include "ble_vs_codes.h"
#include "usecase_dev_mgmt.h"
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
#define DIVC( x, y )         (((x)+(y)-1)/(y))

#define UNPACK_2_BYTE_PARAMETER(ptr)  \
        ((uint16_t)((uint16_t)(*((uint8_t *)ptr))) |   \
        (uint16_t)((((uint16_t)(*((uint8_t *)ptr + 1))) << 8)))
/* Private variables ---------------------------------------------------------*/
/* Private functions prototype------------------------------------------------*/
static tBleStatus TMAP_CLT_Linkup_Process(TMAP_CltInst_t *pTMAP_Clt,BleEATTBearer_t *pEATTBearer,uint8_t ErrorCode);
static void TMAP_CLT_Post_Linkup_Event(TMAP_CltInst_t *pTMAP_Clt, tBleStatus const Status);
tBleStatus TMAP_CLT_Check_TMAS_Service(TMAP_CltInst_t *pTMAP_Clt);
static tBleStatus TMAP_CLT_MemAlloc(const TMAP_Config_t *pConfig);
void* TMAP_MemAssign(uint32_t** base, uint16_t n, uint16_t size);
static TMAP_CltInst_t *TMAP_CLT_GetAvailableInstance(void);
static TMAP_CltInst_t *TMAP_CLT_GetInstance(uint16_t ConnHandle,BleEATTBearer_t **pEATTBearer);
static tBleStatus TMAP_IsATTProcedureInProgress(uint16_t ConnHandle,BleEATTBearer_t *pEATTBearer);
static void TMAP_CLT_InitInstance(TMAP_CltInst_t *pTMAP_Clt);
static SVCCTL_EvtAckStatus_t TMAP_ReadAttRespHandle(uint16_t ConnHandle,
                                                    uint8_t *pAttribute_Value,
                                                    uint16_t Offset);
/* External functions prototype------------------------------------------------*/

/* Functions Definition ------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/
/**
  * @brief  Initialize the Telephony and Media Audio Profile
  * @param  pConfig: Pointer to the TMAP Config structure
  * @retval status of the initialization
  */
tBleStatus TMAP_Init(TMAP_Config_t *pConfig)
{
  tBleStatus status = BLE_STATUS_SUCCESS;
  uint8_t i;

  TMAP_Context.Role = pConfig->Role;
  TMAP_Context.MaxNumBleLinks = pConfig->MaxNumBleLinks;

  /* Init TMAP memory */
  status = TMAP_CLT_MemAlloc(pConfig);

  if (status == BLE_STATUS_SUCCESS)
  {
    for (i = 0; i < pConfig->MaxNumBleLinks; i++)
    {
      TMAP_CLT_InitInstance(&TMAP_Context.pInst[i]);
    }

    status = TMAS_InitService(&TMAP_Context.TMASSvc);
    BLE_DBG_TMAP_MSG("TMAS Service Init returns status 0x%04X\n", status);

    if (status == BLE_STATUS_SUCCESS)
    {
      status = TMAS_InitCharacteristics(&TMAP_Context.TMASSvc);
      BLE_DBG_TMAP_MSG("TMAS Characteristics Init returns status 0x%04X\n", status);

      if (status == BLE_STATUS_SUCCESS)
      {
        status = TMAS_SetTMAPRole(&TMAP_Context.TMASSvc, pConfig->Role);
        BLE_DBG_TMAP_MSG("Role %04X has been set in TMAS with status 0x%04X\n", pConfig->Role, status);
      }
    }
  }

  return status;
}

/**
  * @brief Link Up the TMAP Client with remote TMAP Server
  * @param  ConnHandle: ACL Connection handle
  * @param  LinkupMode: LinkUp Mode
  * @note TMAP_LINKUP_COMPLETE_EVT event will be generated once process is complete
  * @retval status of the operation
  */
tBleStatus TMAP_Linkup(uint16_t ConnHandle, TMAP_LinkupMode_t LinkupMode)
{
  tBleStatus            hciCmdResult = HCI_COMMAND_DISALLOWED_ERR_CODE;
  TMAP_CltInst_t        *p_tmap_clt;
  UseCaseConnInfo_t     *p_conn_info;
  BleEATTBearer_t       *p_eatt_bearer = 0;
  uint16_t              conn_handle;
  uint8_t               channel_index;
  BLE_DBG_TMAP_MSG("Start TMAP Link Up procedure on ACL Connection Handle 0x%04X\n", ConnHandle);

  if (USECASE_DEV_MGMT_GetConnInfo(ConnHandle,&p_conn_info,&p_eatt_bearer) == BLE_STATUS_SUCCESS)
  {
    conn_handle = p_conn_info->Connection_Handle;
    if (LinkupMode == TMAP_LINKUP_MODE_COMPLETE)
    {
      if (BLE_AUDIO_STACK_EATT_GetNumSubscribedBearers(conn_handle) > 0)
      {
        if (BLE_AUDIO_STACK_EATT_GetAvailableBearer(conn_handle,&channel_index) == BLE_STATUS_SUCCESS)
        {
          conn_handle = (0xEA << 8) | (channel_index);
        }
        else
        {
          BLE_DBG_TMAP_MSG("TMAP Link Up procedure is aborted because no available EATT Bearer\n");
          return BLE_STATUS_BUSY;
        }
      }
      else if (BLE_AUDIO_STACK_ATT_IsRegisteredATTProcedure(p_conn_info->Connection_Handle,0) == BLE_STATUS_SUCCESS)
      {
        BLE_DBG_TMAP_MSG("TMAP Link Up procedure is aborted because ATT Bearer already used\n");
        return BLE_STATUS_BUSY;
      }
    }
    /* Check if a TMAP Client Instance with specified Connection Handle is already allocated*/
    p_tmap_clt = TMAP_CLT_GetInstance(p_conn_info->Connection_Handle,&p_eatt_bearer);
    if (p_tmap_clt == 0)
    {
      /*Get an available TMAP Client Instance*/
      p_tmap_clt = TMAP_CLT_GetAvailableInstance();
      if (p_tmap_clt != 0)
      {
        p_tmap_clt->pConnInfo = p_conn_info;
        /*Check that TMAP LinkUp process is not already started*/
        if (p_tmap_clt->LinkupState == TMAP_LINKUP_IDLE)
        {
          p_tmap_clt->LinkupMode = LinkupMode;

          if (p_tmap_clt->LinkupMode == TMAP_LINKUP_MODE_COMPLETE)
          {
            /* First step of Link Up process : find the TMAS in the remote GATT Database*/
            hciCmdResult = aci_gatt_disc_all_primary_services(conn_handle);
            BLE_DBG_TMAP_MSG("aci_gatt_disc_all_primary_services() on ConnHandle 0x%04X returns status 0x%x\n",
                             conn_handle,
                             hciCmdResult);
            if (hciCmdResult == BLE_STATUS_SUCCESS)
            {
              if (BLE_AUDIO_STACK_EATT_GetNumSubscribedBearers(p_tmap_clt->pConnInfo->Connection_Handle) > 0)
              {
                BLE_AUDIO_STACK_EATT_RegisterATTProcedure(channel_index,TMAP_ATT_PROCEDURE_ID);
              }
              else
              {
                BLE_AUDIO_STACK_ATT_RegisterATTProcedure(p_tmap_clt->pConnInfo->Connection_Handle,
                                                         TMAP_ATT_PROCEDURE_ID);
              }
              /*Start Microphone Control Service Linkup*/
              p_tmap_clt->LinkupState = TMAP_LINKUP_DISC_SERVICE;
            }
            else
            {
              TMAP_CLT_InitInstance(p_tmap_clt);
            }
          }
          else if (p_tmap_clt->LinkupMode == TMAP_LINKUP_MODE_RESTORE)
          {
            /*TMAS shall be restored from Service Database*/
            hciCmdResult = TMAP_CLT_RestoreDatabase(p_tmap_clt);
            if (hciCmdResult != BLE_STATUS_SUCCESS)
            {
              BLE_DBG_TMAP_MSG("TMAP Link restoration has failed\n");
              TMAP_CLT_InitInstance(p_tmap_clt);
            }
          }
        }
        else if (p_tmap_clt->LinkupState == TMAP_LINKUP_COMPLETE)
        {
          /*TMAP Link Up is already performed*/
          BLE_DBG_TMAP_MSG("TMAP Link Up is already performed\n");
          hciCmdResult = HCI_COMMAND_DISALLOWED_ERR_CODE;
        }
        else
        {
          BLE_DBG_TMAP_MSG("TMAP Link Up process is already in progress\n");
          /*Microphone Control Link Up process is already in progress*/
          hciCmdResult = BLE_STATUS_BUSY;
        }
      }
      else
      {
        BLE_DBG_TMAP_MSG("No resource to use a TMAP Client Instance\n");
        hciCmdResult = BLE_STATUS_FAILED;
      }
    }
    else
    {
      BLE_DBG_TMAP_MSG("TMAP Client Instance is already associated to the connection handle 0x%04X\n",ConnHandle);
      if (p_tmap_clt->LinkupState == TMAP_LINKUP_COMPLETE)
      {
        /*TMAP Link Up is already performed*/
        hciCmdResult = HCI_COMMAND_DISALLOWED_ERR_CODE;
      }
      else
      {
        /* TMAP Link Up process is already in progress*/
        hciCmdResult = BLE_STATUS_BUSY;
      }
    }
  }
  else
  {
    hciCmdResult = BLE_STATUS_INVALID_PARAMS;
  }
  return hciCmdResult;
}

/**
  * @brief Read the remote TMAP Role value
  * @param ConnHandle: The connection of the remote TMAP Server
  * @note TMAP_REM_ROLE_VALUE_EVT will be generated upon characteristic read
  * @return Status of the operation
  */
tBleStatus TMAP_ReadRemoteTMAPRole(uint16_t ConnHandle)
{
  tBleStatus            hciCmdResult = BLE_STATUS_INVALID_PARAMS;
  TMAP_CltInst_t        *p_tmap_clt;
  BleEATTBearer_t       *p_eatt_bearer = 0;
  uint16_t              conn_handle;
  uint8_t               channel_index;
  BLE_DBG_TMAP_MSG("Start Read Remote TMAP Role on connection handle 0x%04X\n",ConnHandle);

  p_tmap_clt = TMAP_CLT_GetInstance(ConnHandle,&p_eatt_bearer);
  if (p_tmap_clt != 0)
  {
    /* Check if ATT or EATT Bearer is available to perform the ATT procedure */
    if (BLE_AUDIO_STACK_EATT_GetNumSubscribedBearers(p_tmap_clt->pConnInfo->Connection_Handle) > 0)
    {
      if (BLE_AUDIO_STACK_EATT_GetAvailableBearer(p_tmap_clt->pConnInfo->Connection_Handle,
                                                  &channel_index) == BLE_STATUS_SUCCESS)
      {
        conn_handle = (0xEA << 8) | (channel_index);
      }
      else
      {
        BLE_DBG_TMAP_MSG("TMAP Link Up procedure is aborted because no available EATT Bearer\n");
        hciCmdResult = BLE_STATUS_BUSY;
      }
    }
    else
    {
      if (BLE_AUDIO_STACK_ATT_IsRegisteredATTProcedure(p_tmap_clt->pConnInfo->Connection_Handle,0) == BLE_STATUS_FAILED)
      {
        conn_handle = p_tmap_clt->pConnInfo->Connection_Handle;
      }
      else
      {
        hciCmdResult = BLE_STATUS_BUSY;
      }
    }

    if (hciCmdResult != BLE_STATUS_BUSY)
    {
      if (p_tmap_clt->LinkupState == TMAP_LINKUP_COMPLETE)
      {
        if (p_tmap_clt->TMAPRoleChar.ValueHandle != 0)
        {
          hciCmdResult = aci_gatt_read_char_value(conn_handle, p_tmap_clt->TMAPRoleChar.ValueHandle);
          BLE_DBG_TMAP_MSG("aci_gatt_read_char_value() (connHandle: %04X, val_handle: %04X) returns status 0x%x\n",
                              conn_handle,
                              p_tmap_clt->TMAPRoleChar.ValueHandle,
                              hciCmdResult);
          if (hciCmdResult == BLE_STATUS_SUCCESS)
          {
            if (BLE_AUDIO_STACK_EATT_GetNumSubscribedBearers(p_tmap_clt->pConnInfo->Connection_Handle) > 0)
            {
              BLE_AUDIO_STACK_EATT_RegisterATTProcedure(channel_index,TMAP_ATT_PROCEDURE_ID);
            }
            else
            {
              BLE_AUDIO_STACK_ATT_RegisterATTProcedure(p_tmap_clt->pConnInfo->Connection_Handle,
                                                       TMAP_ATT_PROCEDURE_ID);
            }
            p_tmap_clt->Op = TMAP_OP_READ;
            BLE_DBG_TMAP_MSG("Start TMAP READ Operation(ConnHandle 0x%04X)\n", ConnHandle);
          }
        }
      }
      else
      {
        BLE_DBG_TMAP_MSG("TMAP Linkup is not complete on Connection Handle 0x%04X\n", ConnHandle);
        return HCI_COMMAND_DISALLOWED_ERR_CODE;
      }
    }
    else
    {
      BLE_DBG_TMAP_MSG("An ATT Procedure is ongoing on Connection Handle 0x%04X\n", ConnHandle);
      return BLE_STATUS_BUSY;
    }
  }
  else
  {
    BLE_DBG_TMAP_MSG("Connection Handle 0x%04X doesn't correspond to an allocated TMAP Client Instance\n", ConnHandle);
    hciCmdResult = BLE_STATUS_INVALID_PARAMS;
  }

  return hciCmdResult;
}

/**
  * @brief  Build ADV Packet for TMAP peripheral
  * @param Announcement: Targeted or General Announcement
  * @param pMetadata: pointer on the LTV-formatted Metadata
  * @param MetadataLength: size of the LTV-formatted Metadata
  * @param Appearance: Appearance Value of the device
  * @param pAdvPacket: buffer provided by upper layer to store ADV information
  * @param AdvPacketLength: size of the buffer pointed by pAdvPacket.
  * @retval length of the built ADV Packet
  */
uint8_t TMAP_BuildAdvPacket(CAP_Announcement_t Announcement,
                               uint8_t const *pMetadata,
                               uint8_t MetadataLength,
                               uint16_t Appearance,
                               uint8_t *pAdvPacket,
                               uint8_t AdvPacketLength)
{
  uint8_t built_packet_length = 0;

  built_packet_length = CAP_BuildAdvPacket(Announcement, pMetadata, MetadataLength, pAdvPacket, AdvPacketLength);

  if (AdvPacketLength - built_packet_length >= 6)
  {
    pAdvPacket[built_packet_length++] = 5;
    pAdvPacket[built_packet_length++] = AD_TYPE_SERVICE_DATA;
    pAdvPacket[built_packet_length++] = TELEPHONY_AND_MEDIA_AUDIO_SERVICE_UUID & 0xFF;
    pAdvPacket[built_packet_length++] = (TELEPHONY_AND_MEDIA_AUDIO_SERVICE_UUID >> 8) & 0xFF;
    pAdvPacket[built_packet_length++] = TMAP_Context.Role & 0xFF;
    pAdvPacket[built_packet_length++] = (TMAP_Context.Role >> 8) & 0xFF;
  }

  if (AdvPacketLength - built_packet_length >= 4)
  {
    pAdvPacket[built_packet_length++] = 3;
    pAdvPacket[built_packet_length++] = AD_TYPE_APPEARANCE;
    pAdvPacket[built_packet_length++] = Appearance & 0xFF;
    pAdvPacket[built_packet_length++] = (Appearance >> 8) & 0xFF;

  }

  return built_packet_length;
}

/**
  * @brief  Notify ACL Disconnection to TMAP
  * @param  ConnHandle: ACL Connection Handle
  */
void TMAP_AclDisconnection(uint16_t ConnHandle)
{
  TMAP_CltInst_t *p_tmap_clt;
  BleEATTBearer_t *p_eatt_bearer = 0;
  /* Check if a TMAP Client Instance with specified Connection Handle is already allocated*/
  p_tmap_clt = TMAP_CLT_GetInstance(ConnHandle,&p_eatt_bearer);
  if (p_tmap_clt != 0)
  {
    BLE_DBG_TMAP_MSG("ACL Disconnection on Connection Handle 0x%04X : Reset TMAP Client Instance\n",ConnHandle);
    /*Check if a TMAP Linkup procedure is in progress*/
    if ((p_tmap_clt->LinkupState != TMAP_LINKUP_IDLE) && (p_tmap_clt->LinkupState != TMAP_LINKUP_COMPLETE))
    {
      /*Notify that TMAP Link Up is complete*/
      TMAP_Notification_Evt_t evt;
      evt.ConnHandle = p_tmap_clt->pConnInfo->Connection_Handle;
      evt.Status = BLE_STATUS_FAILED;
      evt.EvtOpcode = (TMAP_NotCode_t) TMAP_LINKUP_COMPLETE_EVT;
      evt.pInfo = 0;
      TMAP_Notification(&evt);
      if ((BLE_AUDIO_STACK_ATT_IsRegisteredATTProcedure(p_tmap_clt->pConnInfo->Connection_Handle,
                                                        TMAP_ATT_PROCEDURE_ID) == BLE_STATUS_FAILED) \
          && (BLE_AUDIO_STACK_EATT_IsRegisteredATTProcedure(p_tmap_clt->pConnInfo->Connection_Handle,
                                                            TMAP_ATT_PROCEDURE_ID) == BLE_STATUS_FAILED))
      {
        TMAP_CLT_InitInstance(p_tmap_clt);
      }
      else
      {
        /* Wait ACI_GATT_PROC_COMPLETE_VSEVT_CODE event to deallocate the TMAP Client Instance*/
        p_tmap_clt->DelayDeallocation = 1u;
      }
    }
    else
    {
      /* Store the GATT database of the TMAP */
      TMAP_CLT_StoreDatabase(p_tmap_clt);
      if ((BLE_AUDIO_STACK_ATT_IsRegisteredATTProcedure(p_tmap_clt->pConnInfo->Connection_Handle,
                                                        TMAP_ATT_PROCEDURE_ID) == BLE_STATUS_FAILED) \
          && (BLE_AUDIO_STACK_EATT_IsRegisteredATTProcedure(p_tmap_clt->pConnInfo->Connection_Handle,
                                                            TMAP_ATT_PROCEDURE_ID) == BLE_STATUS_FAILED))
      {
        TMAP_CLT_InitInstance(p_tmap_clt);
      }
      else
      {
        /* Wait ACI_GATT_PROC_COMPLETE_VSEVT_CODE event to deallocate the TMAP Client Instance*/
        p_tmap_clt->DelayDeallocation = 1u;
      }
    }
  }
}

/** @brief This function is used by the Device in the TMAP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */
SVCCTL_EvtAckStatus_t TMAP_GATT_Event_Handler(void *pEvent)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *p_event_pckt;
  BleEATTBearer_t *p_eatt_bearer = 0;
  return_value = SVCCTL_EvtNotAck;
  p_event_pckt = (hci_event_pckt *)(((hci_uart_pckt*)pEvent)->data);

  switch (p_event_pckt->evt)
  {
    case HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE:
    {
      evt_blecore_aci *p_blecore_evt = (evt_blecore_aci*)p_event_pckt->data;
      TMAP_CltInst_t *p_tmap_clt;
      switch (p_blecore_evt->ecode)
      {
        case ACI_ATT_READ_BY_GROUP_TYPE_RESP_VSEVT_CODE:
        {
          aci_att_read_by_group_type_resp_event_rp0 *pr = (void*)p_blecore_evt->data;
          uint8_t numServ, i, idx;
          uint16_t uuid;

          /* Check if a TMAP Instance with specified Connection Handle exists*/
          p_tmap_clt = TMAP_CLT_GetInstance(pr->Connection_Handle,&p_eatt_bearer);
          if (p_tmap_clt != 0)
          {
            if (TMAP_IsATTProcedureInProgress(p_tmap_clt->pConnInfo->Connection_Handle,p_eatt_bearer) == BLE_STATUS_SUCCESS)
            {
              BLE_DBG_TMAP_MSG("ACI_ATT_READ_BY_GROUP_TYPE_RESP_EVENT is received on conn handle %04X\n",
                               pr->Connection_Handle);

              return_value = SVCCTL_EvtAckFlowEnable;

              /*Check that TMAP Link Up Process State is in Service Discovery state*/
              if ((p_tmap_clt->LinkupState & TMAP_LINKUP_DISC_SERVICE) == TMAP_LINKUP_DISC_SERVICE)
              {
                numServ = (pr->Data_Length) / pr->Attribute_Data_Length;
                BLE_DBG_TMAP_MSG("Number of services in the GATT response : %d\n",numServ);
                if (pr->Attribute_Data_Length == 6)
                {
                  idx = 4;
                  for (i=0; i<numServ; i++)
                  {
                    uuid = UNPACK_2_BYTE_PARAMETER(&pr->Attribute_Data_List[idx]);
                    /*Check that UUID in the GATT response corresponds to the MICS, a VOCS or a AICS */
                    if (uuid == TELEPHONY_AND_MEDIA_AUDIO_SERVICE_UUID)
                    {
                      /* Save start handle and the end handle of the TMAS
                       * for the next step of the TMAP Link Up process
                       */
                      p_tmap_clt->ServiceStartHandle = UNPACK_2_BYTE_PARAMETER(&pr->Attribute_Data_List[idx-4]);
                      p_tmap_clt->ServiceEndHandle = UNPACK_2_BYTE_PARAMETER (&pr->Attribute_Data_List[idx-2]);
                      BLE_DBG_TMAP_MSG("Telephony and Media Audio Service has been found (start: %04X, end: %04X)\n",
                                          p_tmap_clt->ServiceStartHandle,
                                          p_tmap_clt->ServiceEndHandle);
                    }
                    idx += 6;
                  }
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

          /* Check if a TMAP Client Instance with specified Connection Handle exists*/
          p_tmap_clt = TMAP_CLT_GetInstance(pr->Connection_Handle,&p_eatt_bearer);
          if (p_tmap_clt != 0)
          {
            if (TMAP_IsATTProcedureInProgress(p_tmap_clt->pConnInfo->Connection_Handle,p_eatt_bearer) == BLE_STATUS_SUCCESS)
            {
            /* the event data will be
             * 2 bytes start handle
             * 1 byte char properties
             * 2 bytes handle
             * 2 or 16 bytes data
             */
              BLE_DBG_TMAP_MSG("ACI_ATT_READ_BY_TYPE_RESP_EVENT is received on connHandle %04X\n",pr->Connection_Handle);
              return_value = SVCCTL_EvtAckFlowEnable;

              /*Check that TMAP Link Up Process State is in Characteristics Discovery state*/
              if ((p_tmap_clt->LinkupState & TMAP_LINKUP_DISC_CHAR) == TMAP_LINKUP_DISC_CHAR)
              {
                idx = 5;
                data_length = pr->Data_Length;
                if (pr->Handle_Value_Pair_Length == 7)
                {
                  data_length -= 1;
                  /*Check if characteristic handle corresponds to the Microphone Control Service range*/
                  if ((UNPACK_2_BYTE_PARAMETER(&pr->Handle_Value_Pair_Data[0u])>= p_tmap_clt->ServiceStartHandle)  \
                     && (UNPACK_2_BYTE_PARAMETER(&pr->Handle_Value_Pair_Data[0u]) <= p_tmap_clt->ServiceEndHandle))
                  {
                    while (idx < data_length)
                    {
                      /* extract the characteristic UUID */
                      uuid = UNPACK_2_BYTE_PARAMETER(&pr->Handle_Value_Pair_Data[idx]);
                      /*  extract the characteristic handle */
                      handle = UNPACK_2_BYTE_PARAMETER(&pr->Handle_Value_Pair_Data[idx-2]);
                      /*  extract the start handle of the characteristic */
                      start_handle = UNPACK_2_BYTE_PARAMETER(&pr->Handle_Value_Pair_Data[idx-5]);

                      /* Check that gatt characteristic is valid or not : this pointer corresponds
                       * to the previous found characteristic :
                       * the end handle of the previously found characteristic of each Service
                       * is calculated thanks start handle of the next characteristic in the GATT Database
                      */
                      if (p_tmap_clt->pGattChar != 0x00000000)
                      {
                        p_tmap_clt->pGattChar->EndHandle = (start_handle -1u);
                      }

                      switch (uuid)
                      {
                        case TMAP_ROLE_UUID:
                          BLE_DBG_TMAP_MSG("TMAP Role Characteristic has been found:\n");
                          BLE_DBG_TMAP_MSG("Attribute Handle = %04X\n",start_handle);
                          BLE_DBG_TMAP_MSG("Characteristic Properties = 0x%02X\n",pr->Handle_Value_Pair_Data[idx-3]);
                          BLE_DBG_TMAP_MSG("Handle = 0x%04X\n",handle);
                          p_tmap_clt->TMAPRoleChar.ValueHandle = handle;
                          p_tmap_clt->TMAPRoleChar.EndHandle = p_tmap_clt->ServiceEndHandle;
                          p_tmap_clt->TMAPRoleChar.Properties = pr->Handle_Value_Pair_Data[idx-3];
                          p_tmap_clt->pGattChar = &p_tmap_clt->TMAPRoleChar;
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
          }
          else
          {
            BLE_DBG_TMAP_MSG("No TMAP Client allocated for this connection handle\n");
          }
        }
        break;

        case ACI_ATT_READ_RESP_VSEVT_CODE:
        {
          aci_att_read_resp_event_rp0 *pr = (void*)p_blecore_evt->data;
          
          return_value = TMAP_ReadAttRespHandle(pr->Connection_Handle,
                                                pr->Attribute_Value,
                                                0u);
          if (return_value == SVCCTL_EvtAckFlowEnable)
          {
            BLE_DBG_TMAP_MSG("ACI_ATT_READ_RESP_EVENT is received on conn handle %04X\n",
                             pr->Connection_Handle);
          }
        }
        break;

        case ACI_GATT_READ_EXT_VSEVT_CODE:
        {
          aci_gatt_read_ext_event_rp0 *pr = (void*)p_blecore_evt->data;
          
          return_value = TMAP_ReadAttRespHandle(pr->Connection_Handle,
                                                pr->Attribute_Value,
                                                pr->Offset);
          if (return_value == SVCCTL_EvtAckFlowEnable)
          {
            BLE_DBG_TMAP_MSG("ACI_GATT_READ_EXT_EVENT is received on conn handle %04X\n",
                             pr->Connection_Handle);
          }
        }
        break;

        case ACI_GATT_PROC_COMPLETE_VSEVT_CODE:
        {
          aci_gatt_proc_complete_event_rp0 *pr = (void*)p_blecore_evt->data;
          /* Check if a TMAP Client Instance with specified Connection Handle exists*/
          p_tmap_clt = TMAP_CLT_GetInstance(pr->Connection_Handle,&p_eatt_bearer);
          if (p_tmap_clt != 0)
          {
            uint8_t tmap_proc = 0u;
            BLE_DBG_TMAP_MSG("ACI_GATT_PROC_COMPLETE_EVENT is received on conn handle %04X (ErrorCode %04X)\n",
                                pr->Connection_Handle,
                                pr->Error_Code);
            if (p_eatt_bearer != 0)
            {
              if (BLE_AUDIO_STACK_EATT_IsBearerRegisteredATTProcedure(p_eatt_bearer->ChannelIdx,
                                                                      TMAP_ATT_PROCEDURE_ID) == BLE_STATUS_SUCCESS)
              {
                BLE_AUDIO_STACK_EATT_UnregisterATTProcedure(p_eatt_bearer->ChannelIdx,TMAP_ATT_PROCEDURE_ID);
                tmap_proc = 1u;
              }
            }
            else
            {
              /* Check if an ATT Procedure was started*/
              if (BLE_AUDIO_STACK_ATT_IsRegisteredATTProcedure(p_tmap_clt->pConnInfo->Connection_Handle,
                                                               TMAP_ATT_PROCEDURE_ID) == BLE_STATUS_SUCCESS)
              {
                BLE_AUDIO_STACK_ATT_UnregisterATTProcedure(p_tmap_clt->pConnInfo->Connection_Handle,
                                                           TMAP_ATT_PROCEDURE_ID);
                tmap_proc = 1u;
              }
            }
            if (tmap_proc == 1u)
            {
              return_value = SVCCTL_EvtAckFlowEnable;
              /*Check if a TMAP Linkup procudre is in progress*/
              if ((p_tmap_clt->LinkupState != TMAP_LINKUP_IDLE) && (p_tmap_clt->LinkupState != TMAP_LINKUP_COMPLETE))
              {
                if (p_tmap_clt->DelayDeallocation == 0u)
                {
                  /* GATT Process is complete, continue, if needed, the TMAP Link Up Process */
                  TMAP_CLT_Linkup_Process(p_tmap_clt,p_eatt_bearer,pr->Error_Code);
                }
              }
              if ((p_tmap_clt->DelayDeallocation == 1u) \
                  && (BLE_AUDIO_STACK_ATT_IsRegisteredATTProcedure(p_tmap_clt->pConnInfo->Connection_Handle,
                                                                   TMAP_ATT_PROCEDURE_ID) == BLE_STATUS_FAILED) \
                  && (BLE_AUDIO_STACK_EATT_IsRegisteredATTProcedure(p_tmap_clt->pConnInfo->Connection_Handle,
                                                                    TMAP_ATT_PROCEDURE_ID) == BLE_STATUS_FAILED))
              {
                BLE_DBG_TMAP_MSG("Free Completely the TMAP Client on conn handle %04X\n",pr->Connection_Handle);
                p_tmap_clt->DelayDeallocation = 0u;
                TMAP_CLT_InitInstance(p_tmap_clt);
              }
            }
          }
        }
        break; /*ACI_GATT_PROC_COMPLETE_VSEVT_CODE*/

        case ACI_GATT_ERROR_RESP_VSEVT_CODE:
        {
          aci_gatt_error_resp_event_rp0 *pr = (void*)p_blecore_evt->data;
          /* Check if a TMAP Client Instance with specified Connection Handle exists*/
          p_tmap_clt = TMAP_CLT_GetInstance(pr->Connection_Handle,&p_eatt_bearer);
          if (p_tmap_clt != 0)
          {
            if (TMAP_IsATTProcedureInProgress(p_tmap_clt->pConnInfo->Connection_Handle,p_eatt_bearer) == BLE_STATUS_SUCCESS)
            {
              BLE_DBG_TMAP_MSG("ACI_GATT_ERROR_RESP_EVENT is received on conn handle %04X\n",pr->Connection_Handle);
              return_value = SVCCTL_EvtAckFlowEnable;
              if ((p_tmap_clt->LinkupState == TMAP_LINKUP_COMPLETE) && (p_tmap_clt->Op != TMAP_OP_NONE))
              {
                p_tmap_clt->ErrorCode = pr->Error_Code;
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
 * @brief TMAP Link Up process
 * @param  pTMAP_Clt: pointer on TMAP Client Instance
 * @param  pEATTBearer: pointer on EATT Bearer
 * @param  ErrorCode: Error Code from Host Stack when a GATT procedure is complete
 * @retval status of the operation
 */
static tBleStatus TMAP_CLT_Linkup_Process(TMAP_CltInst_t *pTMAP_Clt,BleEATTBearer_t *pEATTBearer,uint8_t ErrorCode)
{
  tBleStatus hciCmdResult = BLE_STATUS_FAILED;
  uint16_t conn_handle = pTMAP_Clt->pConnInfo->Connection_Handle;
  if (pEATTBearer != 0)
  {
    conn_handle = (0xEA << 8) | (pEATTBearer->ChannelIdx);
  }

  BLE_DBG_TMAP_MSG("TMAP Link Up Process, state 0x%x\n",pTMAP_Clt->LinkupState);

  /*check that error indicate that linkup process succeeds*/
  if (ErrorCode == 0x00)
  {
    if (pTMAP_Clt->LinkupMode == TMAP_LINKUP_MODE_COMPLETE)
    {
      if ((pTMAP_Clt->LinkupState & TMAP_LINKUP_DISC_SERVICE) == TMAP_LINKUP_DISC_SERVICE)
      {
        pTMAP_Clt->LinkupState &= ~TMAP_LINKUP_DISC_SERVICE;
        /* TMAP Link Up process is in first Step :
         * TMAS discovery in the remote GATT Database
         */
        if (pTMAP_Clt->ServiceEndHandle != 0x0000)
        {
          /* Telephony and Media Audio Service has been found */
          BLE_DBG_TMAP_MSG("GATT : Discover TMAS Characteristics\n");
          pTMAP_Clt->pGattChar = 0u;
          /* Discover all the characteristics of the TMAS in the remote GATT Database */
          hciCmdResult = aci_gatt_disc_all_char_of_service(conn_handle,
                                                          pTMAP_Clt->ServiceStartHandle,
                                                          pTMAP_Clt->ServiceEndHandle);
          BLE_DBG_TMAP_MSG("aci_gatt_disc_all_char_of_service() (start: %04X, end: %04X) returns status 0x%x\n",
                              pTMAP_Clt->ServiceStartHandle,
                              pTMAP_Clt->ServiceEndHandle,
                              hciCmdResult);

          if (hciCmdResult == BLE_STATUS_SUCCESS)
          {
            if (pEATTBearer != 0)
            {
              BLE_AUDIO_STACK_EATT_RegisterATTProcedure(pEATTBearer->ChannelIdx,TMAP_ATT_PROCEDURE_ID);
            }
            else
            {
              BLE_AUDIO_STACK_ATT_RegisterATTProcedure(pTMAP_Clt->pConnInfo->Connection_Handle,
                                                       TMAP_ATT_PROCEDURE_ID);
            }
            pTMAP_Clt->LinkupState |= TMAP_LINKUP_DISC_CHAR;
          }
          else
          {
            /*Notify that TMAP Link Up is complete*/
            TMAP_CLT_Post_Linkup_Event(pTMAP_Clt,hciCmdResult);
          }
        }
        else
        {
          BLE_DBG_TMAP_MSG("No TMAS is present in the remote GATT Database\n");
          /*Notify that TMAP Link Up is complete*/
          TMAP_CLT_Post_Linkup_Event(pTMAP_Clt,BLE_STATUS_FAILED);
        }
      }
      else if ((pTMAP_Clt->LinkupState & TMAP_LINKUP_DISC_CHAR) == TMAP_LINKUP_DISC_CHAR)
      {
        /* Discovery of the characteristics in the Service is complete */
        pTMAP_Clt->LinkupState &= ~TMAP_LINKUP_DISC_CHAR;
        /*Check if remote TMAS is valid*/
        if (TMAP_CLT_Check_TMAS_Service(pTMAP_Clt) == BLE_STATUS_SUCCESS)
        {
          pTMAP_Clt->LinkupState |= TMAP_LINKUP_READ_CHAR;
          /* read the characteristic value */
          hciCmdResult = aci_gatt_read_char_value(conn_handle,pTMAP_Clt->TMAPRoleChar.ValueHandle);
          BLE_DBG_TMAP_MSG("aci_gatt_read_char_value() (connHandle: %04X, val_handle: %04X) returns status 0x%x\n",
                              conn_handle,
                              pTMAP_Clt->TMAPRoleChar.ValueHandle,
                              hciCmdResult);
          if (hciCmdResult != BLE_STATUS_SUCCESS)
          {
            pTMAP_Clt->LinkupState &= ~TMAP_LINKUP_READ_CHAR;
            TMAP_CLT_Post_Linkup_Event(pTMAP_Clt,hciCmdResult);
          }
          else
          {
            if (pEATTBearer != 0)
            {
              BLE_AUDIO_STACK_EATT_RegisterATTProcedure(pEATTBearer->ChannelIdx,TMAP_ATT_PROCEDURE_ID);
            }
            else
            {
              BLE_AUDIO_STACK_ATT_RegisterATTProcedure(pTMAP_Clt->pConnInfo->Connection_Handle,
                                                       TMAP_ATT_PROCEDURE_ID);
            }
          }
        }
        else
        {
          /*Notify that TMAP Link Up is complete*/
          TMAP_CLT_Post_Linkup_Event(pTMAP_Clt, hciCmdResult);
        }
      }
      else if ((pTMAP_Clt->LinkupState & TMAP_LINKUP_READ_CHAR) == TMAP_LINKUP_READ_CHAR)
      {
          pTMAP_Clt->LinkupState &= ~TMAP_LINKUP_READ_CHAR;
          TMAP_CLT_Post_Linkup_Event(pTMAP_Clt,BLE_STATUS_SUCCESS);
      }
    }
  }
  else
  {
    if (pTMAP_Clt->LinkupMode == TMAP_LINKUP_MODE_RESTORE)
    {
      /*Notify that TMAP Link Up is complete*/
      TMAP_Notification_Evt_t evt;
      evt.Status = BLE_STATUS_FAILED;
      evt.ConnHandle = pTMAP_Clt->pConnInfo->Connection_Handle;
      evt.pInfo = 0;
      evt.EvtOpcode = (TMAP_NotCode_t) TMAP_LINKUP_COMPLETE_EVT;
      TMAP_Notification(&evt);
      /*reset the TMAP Client Instance*/
      TMAP_CLT_InitInstance(pTMAP_Clt);
    }
    else
    {
      /*Notify that TMAP Link Up is complete*/
      TMAP_CLT_Post_Linkup_Event(pTMAP_Clt,BLE_STATUS_FAILED);
    }
  }
  return hciCmdResult;
}

/**
 * @brief Notify to upper layer that TMAP Link Up Process is complete
 * @param  pTMAP_Clt: pointer on TMAP Client Instance
 * @param  Status : status of the operation
 */
static void TMAP_CLT_Post_Linkup_Event(TMAP_CltInst_t *pTMAP_Clt, tBleStatus const Status)
{
  TMAP_Notification_Evt_t evt;
  BLE_DBG_TMAP_MSG("TMAP Client notifies TMAP Linkup is complete with status 0x%x\n",Status);
  evt.ConnHandle = pTMAP_Clt->pConnInfo->Connection_Handle;
  evt.Status = Status;

  pTMAP_Clt->LinkupState = TMAP_LINKUP_COMPLETE;
  evt.EvtOpcode = (TMAP_NotCode_t) TMAP_LINKUP_COMPLETE_EVT;
  if (evt.Status == BLE_STATUS_SUCCESS)
  {
    TMAP_AttServiceInfo_Evt_t params;
    params.StartAttHandle = pTMAP_Clt->ServiceStartHandle;
    params.EndAttHandle = pTMAP_Clt->ServiceEndHandle;
    evt.pInfo = (void *) &params;
  }
  else
  {
    evt.pInfo = 0;
  }
  TMAP_Notification(&evt);
  if (evt.Status != BLE_STATUS_SUCCESS)
  {
    BLE_DBG_TMAP_MSG("Linkup has failed, reset the TMAP Client Instance\n");
    TMAP_CLT_InitInstance(pTMAP_Clt);
  }
}

/**
 * @brief Check TMAS Validity
 * @param  pTMAP_Clt: pointer on TMAP Client Instance
 * @retval status of the operation
 */
tBleStatus TMAP_CLT_Check_TMAS_Service(TMAP_CltInst_t *pTMAP_Clt)
{
  tBleStatus status = BLE_STATUS_SUCCESS;

  if (pTMAP_Clt->TMAPRoleChar.ValueHandle == 0x0000u)
  {
    BLE_DBG_TMAP_MSG("Error : TMAP Role Chararacteristic is not found in remote GATT Database\n");
    status = BLE_STATUS_FAILED;
  }
  else
  {
    /*Check if the characteristic properties are valid*/
    if (pTMAP_Clt->TMAPRoleChar.Properties != CHAR_PROP_READ)
    {
      BLE_DBG_TMAP_MSG("Error : Properties 0x%02X of the Microphone State Characteristic is not valid\n",
                  pTMAP_Clt->TMAPRoleChar.Properties);
      status = BLE_STATUS_FAILED;
    }
  }
  if (status == BLE_STATUS_SUCCESS)
  {
    BLE_DBG_TMAP_MSG("All TMAS Characteristics are valids\n");
  }
  return status;
}


static tBleStatus TMAP_CLT_MemAlloc(const TMAP_Config_t *pConfig)
{
  uint32_t *base;
  uint32_t **ptr;

  if (sizeof(TMAP_CltInst_t) != TMAP_CLT_MEM_PER_CONN_SIZE_BYTES)
  {
    BLE_DBG_TMAP_MSG("Sizeof TMAP_CtlrInst_t %d is not compliant with TMAP_CLT_MEM_PER_CONN_SIZE_BYTES %d\n",
                        sizeof(TMAP_CltInst_t),
                        TMAP_CLT_MEM_PER_CONN_SIZE_BYTES);
    return HCI_UNSPECIFIED_ERROR_ERR_CODE;
  }

  /* Allocate the TMAP Client Connection contexts*/
  base = (uint32_t*)pConfig->pStartRamAddr;
  ptr = &base;
  memset(&pConfig->pStartRamAddr[0],0,pConfig->RamSize);
  TMAP_Context.pInst = TMAP_MemAssign(ptr,pConfig->MaxNumBleLinks,sizeof(TMAP_CltInst_t));
  if ( (((uint8_t*)base) - pConfig->pStartRamAddr) > (int32_t)pConfig->RamSize )
  {
    BLE_DBG_TMAP_MSG("Memory used for Context %d bytes exceeds total RAM size %d bytes\n",
                        (((uint8_t*)base) - pConfig->pStartRamAddr),
                        pConfig->RamSize);
    return HCI_MEMORY_CAPACITY_EXCEEDED_ERR_CODE ;
  }

  BLE_DBG_TMAP_MSG("Memory used for TMAP Client Context %d bytes on total RAM size %d bytes\n",
                      (((uint8_t*)base) - pConfig->pStartRamAddr),
                      pConfig->RamSize);
  return BLE_STATUS_SUCCESS;
}

void* TMAP_MemAssign( uint32_t** base, uint16_t n, uint16_t size )
{
  uint32_t *ptr = *base;

  *base = ptr + DIVC(n * (uint32_t)size, sizeof(uint32_t));

  return (void*)ptr;
}

/**
  * @brief Initialize a TMAP Client Instance
  * @param pTMAP_Clt: pointer on TMAP Client Instance
  */
static void TMAP_CLT_InitInstance(TMAP_CltInst_t *pTMAP_Clt)
{
  BLE_DBG_TMAP_MSG("Initialize TMAP Client Instance\n");
  /*Initialize the TMAP_CltInst_t structure*/
  pTMAP_Clt->LinkupState = TMAP_LINKUP_IDLE;
  pTMAP_Clt->ReqHandle = 0x0000;
  pTMAP_Clt->pConnInfo = 0;
  pTMAP_Clt->Op = TMAP_OP_NONE;
  pTMAP_Clt->TMAPRoleChar.ValueHandle = 0x0000u;
  pTMAP_Clt->TMAPRoleChar.DescHandle = 0x0000u;
  pTMAP_Clt->TMAPRoleChar.Properties = 0x00u;
  pTMAP_Clt->TMAPRoleChar.EndHandle = 0x0000u;
  pTMAP_Clt->ServiceStartHandle = 0x0000;
  pTMAP_Clt->ServiceEndHandle = 0x0000;
  pTMAP_Clt->pGattChar = 0;
  pTMAP_Clt->ErrorCode = 0x00u;
}


static TMAP_CltInst_t *TMAP_CLT_GetAvailableInstance(void)
{
  uint8_t i;

  /*Get a free TMAP Client Instance*/
  for (i = 0 ; i < TMAP_Context.MaxNumBleLinks ; i++)
  {
    if (TMAP_Context.pInst[i].pConnInfo == 0u)
    {
      return &TMAP_Context.pInst[i];
    }
  }
  return 0u;
}

static TMAP_CltInst_t *TMAP_CLT_GetInstance(uint16_t ConnHandle,BleEATTBearer_t **pEATTBearer)
{
  uint8_t i;
#if (CFG_BLE_EATT_BEARER_PER_LINK > 0u)
  /*Check if the connection handle corresponds to a Connection Oriented Channel*/
  if ((uint8_t) (ConnHandle >> 8) == 0xEA)
  {
    for (i = 0 ; i < TMAP_Context.MaxNumBleLinks ; i++)
    {
      if (TMAP_Context.pInst[i].pConnInfo != 0)
      {
        if (TMAP_Context.pInst[i].pConnInfo->Connection_Handle != 0xFFFFu)
        {
          for (uint8_t j = 0u; j < CFG_BLE_EATT_BEARER_PER_LINK; j++)
          {
            if ( (TMAP_Context.pInst[i].pConnInfo->aEATTBearer[j].State == 0u) \
                && (TMAP_Context.pInst[i].pConnInfo->aEATTBearer[j].ChannelIdx == (uint8_t)(ConnHandle)))
            {
              *pEATTBearer = &TMAP_Context.pInst[i].pConnInfo->aEATTBearer[j];
              return &TMAP_Context.pInst[i];
            }
          }
        }
      }
    }
  }
  else
#endif /*(CFG_BLE_EATT_BEARER_PER_LINK > 0u)*/
  {
    for (i = 0 ; i < TMAP_Context.MaxNumBleLinks ; i++)
    {
      if (TMAP_Context.pInst[i].pConnInfo != 0)
      {
        if (TMAP_Context.pInst[i].pConnInfo->Connection_Handle == ConnHandle)
        {
          return &TMAP_Context.pInst[i];
        }
      }
    }

  }
  return 0u;
}


static tBleStatus TMAP_IsATTProcedureInProgress(uint16_t ConnHandle,BleEATTBearer_t *pEATTBearer)
{

  tBleStatus status = BLE_STATUS_FAILED;

  if (pEATTBearer != 0)
  {
    if (BLE_AUDIO_STACK_EATT_IsBearerRegisteredATTProcedure(pEATTBearer->ChannelIdx,
                                                            TMAP_ATT_PROCEDURE_ID) == BLE_STATUS_SUCCESS)
    {
      status = BLE_STATUS_SUCCESS;
    }
  }
  else
  {
    /* Check if an ATT Procedure was started*/
    if (BLE_AUDIO_STACK_ATT_IsRegisteredATTProcedure(ConnHandle,TMAP_ATT_PROCEDURE_ID) == BLE_STATUS_SUCCESS)
    {
      status = BLE_STATUS_SUCCESS;
    }
  }
  return status;
}

static SVCCTL_EvtAckStatus_t TMAP_ReadAttRespHandle(uint16_t ConnHandle,
                                                    uint8_t *pAttribute_Value,
                                                    uint16_t Offset)
{
  SVCCTL_EvtAckStatus_t return_value = SVCCTL_EvtNotAck;
  BleEATTBearer_t       *p_eatt_bearer = 0;
  TMAP_CltInst_t        *p_tmap_clt;

  /* Check if a TMAP Client Instance with specified Connection Handle exists*/
  p_tmap_clt = TMAP_CLT_GetInstance(ConnHandle,&p_eatt_bearer);
  if (p_tmap_clt != 0)
  {
    if (TMAP_IsATTProcedureInProgress(p_tmap_clt->pConnInfo->Connection_Handle,p_eatt_bearer) == BLE_STATUS_SUCCESS)
    {
      return_value = SVCCTL_EvtAckFlowEnable;
      
      if ( Offset == 0u)
      {
        /* Handle the ATT read response */
        if ((p_tmap_clt->Op == TMAP_OP_READ) \
            || ((p_tmap_clt->LinkupState & TMAP_LINKUP_READ_CHAR) == TMAP_LINKUP_READ_CHAR))
        {
          TMAP_Notification_Evt_t evt;
          uint16_t tmap_role;

          return_value = SVCCTL_EvtAckFlowEnable;

          tmap_role = pAttribute_Value[0] + (pAttribute_Value[1] << 8);

          evt.ConnHandle = p_tmap_clt->pConnInfo->Connection_Handle;
          evt.EvtOpcode = TMAP_REM_ROLE_VALUE_EVT;
          evt.Status = BLE_STATUS_SUCCESS;
          evt.pInfo = (uint8_t *) &tmap_role;

          TMAP_Notification(&evt);
        }
      }
    }
  }

  return return_value;
}