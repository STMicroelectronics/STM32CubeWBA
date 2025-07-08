/**
  ******************************************************************************
  * @file    gmap.c
  * @author  MCD Application Team
  * @brief   This file contains Gaming Audio Profile feature
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
#include "gmap_log.h"
#include "gmap_alloc.h"
#include "gmap.h"
#include "gmas.h"
#include "gmap_config.h"
#include "cap.h"
#include "svc_ctl.h"
#include "ble_gatt_aci.h"
#include "ble_vs_codes.h"
#include "usecase_dev_mgmt.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
#define UNPACK_2_BYTE_PARAMETER(ptr)  \
        ((uint16_t)((uint16_t)(*((uint8_t *)ptr))) |   \
        (uint16_t)((((uint16_t)(*((uint8_t *)ptr + 1))) << 8)))
/* Private variables ---------------------------------------------------------*/
/* Private functions prototype------------------------------------------------*/
static tBleStatus GMAP_CLT_Linkup_Process(GMAP_CltInst_t *pGMAP_Clt,BleEATTBearer_t *pEATTBearer,uint8_t ErrorCode);
static void GMAP_CLT_Post_Linkup_Event(GMAP_CltInst_t *pGMAP_Clt, tBleStatus const Status);
tBleStatus GMAP_CLT_Check_GMAS_GMAPRoleCharacteristic(GMAP_CltInst_t *pGMAP_Clt);
tBleStatus GMAP_CLT_Check_GMAS_FeaturesCharacteristics(GMAP_CltInst_t *pGMAP_Clt);
static GMAP_CltInst_t *GMAP_CLT_GetAvailableInstance(void);
static GMAP_CltInst_t *GMAP_CLT_GetInstance(uint16_t ConnHandle,BleEATTBearer_t **pEATTBearer);
static void GMAP_CLT_InitInstance(GMAP_CltInst_t *pGMAP_Clt);
static tBleStatus GMAP_IsATTProcedureInProgress(uint16_t ConnHandle,BleEATTBearer_t *pEATTBearer);
/* External functions prototype------------------------------------------------*/

/* Functions Definition ------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/
/**
  * @brief  Initialize the Gaming Audio Profile
  * @param  pConfig: Pointer to the GMAP Config structure
  * @retval status of the initialization
  */
tBleStatus GMAP_Init(GMAP_Role_t GMAPRoles,
                     UGGFeatures_t UGGFeatures,
                     UGTFeatures_t UGTFeatures,
                     BGSFeatures_t BGSFeatures,
                     BGRFeatures_t BGRFeatures)
{
  tBleStatus status = BLE_STATUS_SUCCESS;
  uint8_t i;

  if (GMAPRoles == 0)
  {
    BLE_DBG_GMAP_MSG("Cannot init GMAP with GMAPRoles=0\n");
  }

  GMAP_Context.Role = GMAPRoles;
  GMAP_Context.UGGFeatures = UGGFeatures;
  GMAP_Context.UGTFeatures = UGTFeatures;
  GMAP_Context.BGSFeatures = BGSFeatures;
  GMAP_Context.BGRFeatures = BGRFeatures;

  if (status == BLE_STATUS_SUCCESS)
  {
    for (i = 0; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
    {
      GMAP_CLT_InitInstance(&GMAP_Context.CltInst[i]);
    }

    status = GMAS_InitService(&GMAP_Context.GMASSvc);
    BLE_DBG_GMAP_MSG("GMAS Service Init returns status 0x%04X\n", status);

    if (status == BLE_STATUS_SUCCESS)
    {
      status = GMAS_InitCharacteristics(&GMAP_Context.GMASSvc, GMAPRoles, UGGFeatures, UGTFeatures, BGSFeatures,
                                        BGRFeatures);
      BLE_DBG_GMAP_MSG("GMAS Characteristics Init returns status 0x%04X\n", status);
    }
  }

  return status;
}

/**
  * @brief Link Up the GMAP Client with remote GMAP Server
  * @param  ConnHandle: Connection handle
  * @note GMAP_LINKUP_COMPLETE_EVT event will be generated once process is complete
  * @retval status of the operation
  */
tBleStatus GMAP_Linkup(uint16_t ConnHandle)
{
  tBleStatus            hciCmdResult = HCI_COMMAND_DISALLOWED_ERR_CODE;
  GMAP_CltInst_t        *p_gmap_clt;
  BleEATTBearer_t       *p_eatt_bearer = 0;
  UseCaseConnInfo_t     *p_conn_info;
  uint16_t              conn_handle;
  uint8_t               channel_index;
  BLE_DBG_GMAP_MSG("Start GMAP Link Up procedure on ACL Connection Handle 0x%04X\n", ConnHandle);

  if (USECASE_DEV_MGMT_GetConnInfo(ConnHandle,&p_conn_info,&p_eatt_bearer) == BLE_STATUS_SUCCESS)
  {
    conn_handle = p_conn_info->Connection_Handle;
    if (BLE_AUDIO_STACK_EATT_GetNumSubscribedBearers(conn_handle) > 0)
    {
      if (BLE_AUDIO_STACK_EATT_GetAvailableBearer(conn_handle,&channel_index) == BLE_STATUS_SUCCESS)
      {
        conn_handle = (0xEA << 8) | (channel_index);
      }
      else
      {
        BLE_DBG_GMAP_MSG("TMAP Link Up procedure is aborted because no available EATT Bearer\n");
        return BLE_STATUS_BUSY;
      }
    }
    else if (BLE_AUDIO_STACK_ATT_IsRegisteredATTProcedure(p_conn_info->Connection_Handle,0) == BLE_STATUS_SUCCESS)
    {
      BLE_DBG_GMAP_MSG("TMAP Link Up procedure is aborted because ATT Bearer already used\n");
      return BLE_STATUS_BUSY;
    }

    /* Check if a GMAP Client Instance with specified Connection Handle is already allocated*/
    p_gmap_clt = GMAP_CLT_GetInstance(ConnHandle,&p_eatt_bearer);
    if (p_gmap_clt == 0)
    {
      /*Get an available GMAP Client Instance*/
      p_gmap_clt = GMAP_CLT_GetAvailableInstance();
      if (p_gmap_clt != 0)
      {
        p_gmap_clt->pConnInfo = p_conn_info;
        /*Check that GMAP LinkUp process is not already started*/
        if (p_gmap_clt->LinkupState == GMAP_LINKUP_IDLE)
        {
          /* First step of Link Up process : find the GMAS in the remote GATT Database*/
          hciCmdResult = aci_gatt_disc_all_primary_services(conn_handle);
          BLE_DBG_GMAP_MSG("aci_gatt_disc_all_primary_services() returns status 0x%x\n", hciCmdResult);
          if (hciCmdResult == BLE_STATUS_SUCCESS)
          {
            if (BLE_AUDIO_STACK_EATT_GetNumSubscribedBearers(p_gmap_clt->pConnInfo->Connection_Handle) > 0)
            {
              BLE_AUDIO_STACK_EATT_RegisterATTProcedure(channel_index,GMAP_ATT_PROCEDURE_ID);
            }
            else
            {
              BLE_AUDIO_STACK_ATT_RegisterATTProcedure(p_gmap_clt->pConnInfo->Connection_Handle,GMAP_ATT_PROCEDURE_ID);
            }
            /*Start Gaming Audio Service Linkup*/
            p_gmap_clt->LinkupState = GMAP_LINKUP_DISC_SERVICE;
          }
          else
          {
            GMAP_CLT_InitInstance(p_gmap_clt);
          }
        }
        else if (p_gmap_clt->LinkupState == GMAP_LINKUP_COMPLETE)
        {
          /* GMAP Link Up is already performed */
          BLE_DBG_GMAP_MSG("GMAP Link Up is already performed\n");
          hciCmdResult = HCI_COMMAND_DISALLOWED_ERR_CODE;
        }
        else
        {
          BLE_DBG_GMAP_MSG("GMAP Link Up process is already in progress\n");
          /* Gaming Audio profile  Link Up process is already in progress */
          hciCmdResult = BLE_STATUS_BUSY;
        }
      }
      else
      {
        BLE_DBG_GMAP_MSG("No ressource to use a GMAP Client Instance\n");
        hciCmdResult = BLE_STATUS_FAILED;
      }
    }
    else
    {
      BLE_DBG_GMAP_MSG("GMAP Client Instance is already associated to the connection handle 0x%04X\n",ConnHandle);
      if (p_gmap_clt->LinkupState == GMAP_LINKUP_COMPLETE)
      {
        /*GMAP Link Up is already performed*/
        hciCmdResult = HCI_COMMAND_DISALLOWED_ERR_CODE;
      }
      else
      {
        /* GMAP Link Up process is already in progress*/
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
  * @brief  Build ADV Packet for GMAP peripheral
  * @param Announcement: Targeted or General Announcement
  * @param pMetadata: pointer on the LTV-formatted Metadata
  * @param MetadataLength: size of the LTV-formatted Metadata
  * @param Appearance: Appearance Value of the device
  * @param pAdvPacket: buffer provided by upper layer to store ADV information
  * @param AdvPacketLength: size of the buffer pointed by pAdvPacket.
  * @retval length of the built ADV Packet
  */
uint8_t GMAP_BuildAdvPacket(CAP_Announcement_t Announcement,
                               uint8_t const *pMetadata,
                               uint8_t MetadataLength,
                               uint16_t Appearance,
                               uint8_t *pAdvPacket,
                               uint8_t AdvPacketLength)
{
  uint8_t built_packet_length = 0;

  built_packet_length = CAP_BuildAdvPacket(Announcement, pMetadata, MetadataLength, pAdvPacket, AdvPacketLength);

  if (AdvPacketLength - built_packet_length >= 5)
  {
    pAdvPacket[built_packet_length++] = 4;
    pAdvPacket[built_packet_length++] = AD_TYPE_SERVICE_DATA;
    pAdvPacket[built_packet_length++] = GAMING_AUDIO_SERVICE_UUID & 0xFF;
    pAdvPacket[built_packet_length++] = (GAMING_AUDIO_SERVICE_UUID >> 8) & 0xFF;
    pAdvPacket[built_packet_length++] = GMAP_Context.Role;
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
  * @brief  Notify ACL Disconnection to GMAP
  * @param  ConnHandle: ACL Connection Handle
  */
void GMAP_AclDisconnection(uint16_t ConnHandle)
{
  GMAP_CltInst_t *p_gmap_clt;
  BleEATTBearer_t *p_eatt_bearer = 0;
  /* Check if a GMAP Client Instance with specified Connection Handle is already allocated*/
  p_gmap_clt = GMAP_CLT_GetInstance(ConnHandle,&p_eatt_bearer);
  if (p_gmap_clt != 0)
  {
    BLE_DBG_GMAP_MSG("ACL Disconnection on Connection Handle 0x%04X : Reset GMAP Client Instance\n",ConnHandle);
    /*Check if a GMAP Linkup procedure is in progress*/
    if ((p_gmap_clt->LinkupState != GMAP_LINKUP_IDLE) && (p_gmap_clt->LinkupState != GMAP_LINKUP_COMPLETE))
    {
      /*Notify that GMAP Link Up is complete*/
      GMAP_Notification_Evt_t evt;
      evt.ConnHandle = p_gmap_clt->pConnInfo->Connection_Handle;
      evt.Status = BLE_STATUS_FAILED;
      evt.EvtOpcode = (GMAP_NotCode_t) GMAP_LINKUP_COMPLETE_EVT;
      evt.pInfo = 0;
      GMAP_Notification(&evt);
      if ((BLE_AUDIO_STACK_ATT_IsRegisteredATTProcedure(p_gmap_clt->pConnInfo->Connection_Handle,
                                                        GMAP_ATT_PROCEDURE_ID) == BLE_STATUS_FAILED) \
        && (BLE_AUDIO_STACK_EATT_IsRegisteredATTProcedure(p_gmap_clt->pConnInfo->Connection_Handle,
                                                          GMAP_ATT_PROCEDURE_ID) == BLE_STATUS_FAILED))
      {
        GMAP_CLT_InitInstance(p_gmap_clt);
      }
      else
      {
        /* Wait ACI_GATT_PROC_COMPLETE_VSEVT_CODE event to deallocate the GMAP Client Instance*/
        p_gmap_clt->DelayDeallocation = 1u;
      }
    }
    else
    {
      if ((BLE_AUDIO_STACK_ATT_IsRegisteredATTProcedure(p_gmap_clt->pConnInfo->Connection_Handle,
                                                        GMAP_ATT_PROCEDURE_ID) == BLE_STATUS_FAILED) \
        && (BLE_AUDIO_STACK_EATT_IsRegisteredATTProcedure(p_gmap_clt->pConnInfo->Connection_Handle,
                                                          GMAP_ATT_PROCEDURE_ID) == BLE_STATUS_FAILED))
      {
        GMAP_CLT_InitInstance(p_gmap_clt);
      }
      else
      {
        /* Wait ACI_GATT_PROC_COMPLETE_VSEVT_CODE event to deallocate the GMAP Client Instance*/
        p_gmap_clt->DelayDeallocation = 1u;
      }
    }
  }
}

/** @brief This function is used by the Device in the GMAP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */
SVCCTL_EvtAckStatus_t GMAP_GATT_Event_Handler(void *pEvent)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt        *p_event_pckt;
  BleEATTBearer_t       *p_eatt_bearer = 0;

  return_value = SVCCTL_EvtNotAck;
  p_event_pckt = (hci_event_pckt *)(((hci_uart_pckt*)pEvent)->data);

  switch (p_event_pckt->evt)
  {
    case HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE:
    {
      evt_blecore_aci *p_blecore_evt = (evt_blecore_aci*)p_event_pckt->data;
      GMAP_CltInst_t *p_gmap_clt;
      switch (p_blecore_evt->ecode)
      {
        case ACI_ATT_READ_BY_GROUP_TYPE_RESP_VSEVT_CODE:
        {
          aci_att_read_by_group_type_resp_event_rp0 *pr = (void*)p_blecore_evt->data;
          uint8_t numServ, i, idx;
          uint16_t uuid;

          /* Check if a GMAP Instance with specified Connection Handle exists*/
          p_gmap_clt = GMAP_CLT_GetInstance(pr->Connection_Handle,&p_eatt_bearer);
          if (p_gmap_clt != 0)
          {
            /* Check if an ATT Procedure was started*/
            if (GMAP_IsATTProcedureInProgress(p_gmap_clt->pConnInfo->Connection_Handle,p_eatt_bearer) == BLE_STATUS_SUCCESS)
            {
              BLE_DBG_GMAP_MSG("ACI_ATT_READ_BY_GROUP_TYPE_RESP_EVENT is received on conn handle %04X\n",
                               pr->Connection_Handle);

              return_value = SVCCTL_EvtAckFlowEnable;

              /*Check that GMAP Link Up Process State is in Service Discovery state*/
              if (p_gmap_clt->LinkupState == GMAP_LINKUP_DISC_SERVICE)
              {
                numServ = (pr->Data_Length) / pr->Attribute_Data_Length;
                BLE_DBG_GMAP_MSG("Number of services in the GATT response : %d\n",numServ);
                if (pr->Attribute_Data_Length == 6)
                {
                  idx = 4;
                  for (i=0; i<numServ; i++)
                  {
                    uuid = UNPACK_2_BYTE_PARAMETER(&pr->Attribute_Data_List[idx]);
                    /*Check that UUID in the GATT response corresponds to the MICS, a VOCS or a AICS */
                    if (uuid == GAMING_AUDIO_SERVICE_UUID)
                    {
                      /* Save start handle and the end handle of the GMAS
                       * for the next step of the GMAP Link Up process
                       */
                      p_gmap_clt->ServiceStartHandle = UNPACK_2_BYTE_PARAMETER(&pr->Attribute_Data_List[idx-4]);
                      p_gmap_clt->ServiceEndHandle = UNPACK_2_BYTE_PARAMETER (&pr->Attribute_Data_List[idx-2]);
                      BLE_DBG_GMAP_MSG("Gaming Audio Service has been found (start: %04X, end: %04X)\n",
                                          p_gmap_clt->ServiceStartHandle,
                                          p_gmap_clt->ServiceEndHandle);
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

          /* Check if a GMAP Client Instance with specified Connection Handle exists*/
          p_gmap_clt = GMAP_CLT_GetInstance(pr->Connection_Handle,&p_eatt_bearer);
          if (p_gmap_clt != 0)
          {
            /* Check if an ATT Procedure was started*/
            if (GMAP_IsATTProcedureInProgress(p_gmap_clt->pConnInfo->Connection_Handle,p_eatt_bearer) == BLE_STATUS_SUCCESS)
            {
              /* the event data will be
               * 2 bytes start handle
               * 1 byte char properties
               * 2 bytes handle
               * 2 or 16 bytes data
               */
              BLE_DBG_GMAP_MSG("ACI_ATT_READ_BY_TYPE_RESP_EVENT is received on connHandle %04X\n",pr->Connection_Handle);

              return_value = SVCCTL_EvtAckFlowEnable;

              /*Check that GMAP Link Up Process State is in Characteristics Discovery state*/
              if (p_gmap_clt->LinkupState == GMAP_LINKUP_DISC_CHAR)
              {
                idx = 5;
                data_length = pr->Data_Length;
                if (pr->Handle_Value_Pair_Length == 7)
                {
                  data_length -= 1;
                  /*Check if characteristic handle corresponds to the Gaming Audio Service range*/
                  if ((UNPACK_2_BYTE_PARAMETER(&pr->Handle_Value_Pair_Data[0u])>= p_gmap_clt->ServiceStartHandle)  \
                     && (UNPACK_2_BYTE_PARAMETER(&pr->Handle_Value_Pair_Data[0u]) <= p_gmap_clt->ServiceEndHandle))
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
                      if (p_gmap_clt->pGattChar != 0x00000000)
                      {
                        p_gmap_clt->pGattChar->EndHandle = (start_handle -1u);
                      }

                      switch (uuid)
                      {
                        case GMAP_ROLE_UUID:
                          BLE_DBG_GMAP_MSG("GMAP Role Characteristic has been found:\n");
                          BLE_DBG_GMAP_MSG("Attribute Handle = %04X\n",start_handle);
                          BLE_DBG_GMAP_MSG("Characteristic Properties = 0x%02X\n",pr->Handle_Value_Pair_Data[idx-3]);
                          BLE_DBG_GMAP_MSG("Handle = 0x%04X\n",handle);
                          p_gmap_clt->GMAPRoleChar.ValueHandle = handle;
                          p_gmap_clt->GMAPRoleChar.EndHandle = p_gmap_clt->ServiceEndHandle;
                          p_gmap_clt->GMAPRoleChar.Properties = pr->Handle_Value_Pair_Data[idx-3];;
                          p_gmap_clt->pGattChar = &p_gmap_clt->GMAPRoleChar;
                        break;

                        case UGG_FEATURES_UUID:
                          BLE_DBG_GMAP_MSG("UGG Features Characteristic has been found:\n");
                          BLE_DBG_GMAP_MSG("Attribute Handle = %04X\n",start_handle);
                          BLE_DBG_GMAP_MSG("Characteristic Properties = 0x%02X\n",pr->Handle_Value_Pair_Data[idx-3]);
                          BLE_DBG_GMAP_MSG("Handle = 0x%04X\n",handle);
                          p_gmap_clt->UGGFeaturesChar.ValueHandle = handle;
                          p_gmap_clt->UGGFeaturesChar.EndHandle = p_gmap_clt->ServiceEndHandle;
                          p_gmap_clt->UGGFeaturesChar.Properties = pr->Handle_Value_Pair_Data[idx-3];;
                          p_gmap_clt->pGattChar = &p_gmap_clt->UGGFeaturesChar;
                        break;

                        case UGT_FEATURES_UUID:
                          BLE_DBG_GMAP_MSG("UGT Features Characteristic has been found:\n");
                          BLE_DBG_GMAP_MSG("Attribute Handle = %04X\n",start_handle);
                          BLE_DBG_GMAP_MSG("Characteristic Properties = 0x%02X\n",pr->Handle_Value_Pair_Data[idx-3]);
                          BLE_DBG_GMAP_MSG("Handle = 0x%04X\n",handle);
                          p_gmap_clt->UGTFeaturesChar.ValueHandle = handle;
                          p_gmap_clt->UGTFeaturesChar.EndHandle = p_gmap_clt->ServiceEndHandle;
                          p_gmap_clt->UGTFeaturesChar.Properties = pr->Handle_Value_Pair_Data[idx-3];;
                          p_gmap_clt->pGattChar = &p_gmap_clt->UGTFeaturesChar;
                        break;

                        case BGS_FEATURES_UUID:
                          BLE_DBG_GMAP_MSG("BGS Features Characteristic has been found:\n");
                          BLE_DBG_GMAP_MSG("Attribute Handle = %04X\n",start_handle);
                          BLE_DBG_GMAP_MSG("Characteristic Properties = 0x%02X\n",pr->Handle_Value_Pair_Data[idx-3]);
                          BLE_DBG_GMAP_MSG("Handle = 0x%04X\n",handle);
                          p_gmap_clt->BGSFeaturesChar.ValueHandle = handle;
                          p_gmap_clt->BGSFeaturesChar.EndHandle = p_gmap_clt->ServiceEndHandle;
                          p_gmap_clt->BGSFeaturesChar.Properties = pr->Handle_Value_Pair_Data[idx-3];;
                          p_gmap_clt->pGattChar = &p_gmap_clt->BGSFeaturesChar;
                        break;

                        case BGR_FEATURES_UUID:
                          BLE_DBG_GMAP_MSG("BGR Features Characteristic has been found:\n");
                          BLE_DBG_GMAP_MSG("Attribute Handle = %04X\n",start_handle);
                          BLE_DBG_GMAP_MSG("Characteristic Properties = 0x%02X\n",pr->Handle_Value_Pair_Data[idx-3]);
                          BLE_DBG_GMAP_MSG("Handle = 0x%04X\n",handle);
                          p_gmap_clt->BGRFeaturesChar.ValueHandle = handle;
                          p_gmap_clt->BGRFeaturesChar.EndHandle = p_gmap_clt->ServiceEndHandle;
                          p_gmap_clt->BGRFeaturesChar.Properties = pr->Handle_Value_Pair_Data[idx-3];;
                          p_gmap_clt->pGattChar = &p_gmap_clt->BGRFeaturesChar;
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
            BLE_DBG_GMAP_MSG("No GMAP Client allocated for this connection handle\n");
          }
        }
        break;

        case ACI_ATT_READ_RESP_VSEVT_CODE:
        {
          aci_att_read_resp_event_rp0 *pr = (void*)p_blecore_evt->data;

          /* Check if a GMAP Client Instance with specified Connection Handle exists*/
          p_gmap_clt = GMAP_CLT_GetInstance(pr->Connection_Handle,&p_eatt_bearer);
          if (p_gmap_clt != 0)
          {
            /* Check if an ATT Procedure was started*/
            if (GMAP_IsATTProcedureInProgress(p_gmap_clt->pConnInfo->Connection_Handle,p_eatt_bearer) == BLE_STATUS_SUCCESS)
            {
              BLE_DBG_GMAP_MSG("ACI_ATT_READ_RESP_EVENT is received on conn handle %04X\n",pr->Connection_Handle);

              return_value = SVCCTL_EvtAckFlowEnable;

              /* Handle the ATT read response */
              if (p_gmap_clt->LinkupState == GMAP_LINKUP_READ_CHAR)
              {
                switch (GMAP_Context.CltInst->CurrentLinkupChar)
                {
                  case GMAS_CHAR_GMAP_ROLE:
                    GMAP_Context.CltInst->GMAPRole = pr->Attribute_Value[0] & 0x0F;
                    break;

                  case GMAS_CHAR_UGG_FEATURES:
                    GMAP_Context.CltInst->UGGFeatures = pr->Attribute_Value[0] & 0x07;
                    break;

                  case GMAS_CHAR_UGT_FEATURES:
                    GMAP_Context.CltInst->UGTFeatures = pr->Attribute_Value[0] & 0x7F;
                    break;

                  case GMAS_CHAR_BGS_FEATURES:
                    GMAP_Context.CltInst->BGSFeatures = pr->Attribute_Value[0] & 0x01;
                    break;

                  case GMAS_CHAR_BGR_FEATURES:
                    GMAP_Context.CltInst->BGRFeatures = pr->Attribute_Value[0] & 0x03;
                    break;
                }
              }
            }
          }
        }
        break;

        case ACI_GATT_PROC_COMPLETE_VSEVT_CODE:
        {
          aci_gatt_proc_complete_event_rp0 *pr = (void*)p_blecore_evt->data;
          /* Check if a GMAP Client Instance with specified Connection Handle exists*/
          p_gmap_clt = GMAP_CLT_GetInstance(pr->Connection_Handle,&p_eatt_bearer);
          if (p_gmap_clt != 0)
          {
            uint8_t ack = 0u;
            BLE_DBG_GMAP_MSG("ACI_GATT_PROC_COMPLETE_EVENT is received on conn handle %04X (ErrorCode %04X)\n",
                                pr->Connection_Handle,
                                pr->Error_Code);
            if (p_eatt_bearer != 0)
            {
              if (BLE_AUDIO_STACK_EATT_IsBearerRegisteredATTProcedure(p_eatt_bearer->ChannelIdx,
                                                                      GMAP_ATT_PROCEDURE_ID) == BLE_STATUS_SUCCESS)
              {
                BLE_AUDIO_STACK_EATT_UnregisterATTProcedure(p_eatt_bearer->ChannelIdx,GMAP_ATT_PROCEDURE_ID);
                ack = 1u;
              }
            }
            else
            {
              /* Check if an ATT Procedure was started*/
              if (BLE_AUDIO_STACK_ATT_IsRegisteredATTProcedure(p_gmap_clt->pConnInfo->Connection_Handle,
                                                               GMAP_ATT_PROCEDURE_ID) == BLE_STATUS_SUCCESS)
              {
                BLE_AUDIO_STACK_ATT_UnregisterATTProcedure(p_gmap_clt->pConnInfo->Connection_Handle,
                                                           GMAP_ATT_PROCEDURE_ID);
                ack = 1u;
              }
            }
            if (ack == 1u)
            {
              return_value = SVCCTL_EvtAckFlowEnable;
              /*Check if a GMAP Linkup procudre is in progress*/
              if ((p_gmap_clt->LinkupState != GMAP_LINKUP_IDLE) && (p_gmap_clt->LinkupState != GMAP_LINKUP_COMPLETE))
              {
                if (p_gmap_clt->DelayDeallocation == 0u)
                {
                  /* GATT Process is complete, continue, if needed, the GMAP Link Up Process */
                  GMAP_CLT_Linkup_Process(p_gmap_clt,p_eatt_bearer,pr->Error_Code);
                }
              }
              else if (p_gmap_clt->LinkupState == GMAP_LINKUP_COMPLETE)
              {
                /* Should not happen */
              }
              if ((p_gmap_clt->DelayDeallocation == 1u) \
                    && (BLE_AUDIO_STACK_ATT_IsRegisteredATTProcedure(p_gmap_clt->pConnInfo->Connection_Handle,
                                                                     GMAP_ATT_PROCEDURE_ID) == BLE_STATUS_FAILED) \
                    && (BLE_AUDIO_STACK_EATT_IsRegisteredATTProcedure(p_gmap_clt->pConnInfo->Connection_Handle,
                                                                      GMAP_ATT_PROCEDURE_ID) == BLE_STATUS_FAILED))
              {
                BLE_DBG_GMAP_MSG("Free Completely the GMAP Client on conn handle %04X\n",pr->Connection_Handle);
                p_gmap_clt->DelayDeallocation = 0u;
                GMAP_CLT_InitInstance(p_gmap_clt);
              }
            }
          }
        }
        break; /*ACI_GATT_PROC_COMPLETE_VSEVT_CODE*/

        case ACI_GATT_ERROR_RESP_VSEVT_CODE:
        {
          aci_gatt_error_resp_event_rp0 *pr = (void*)p_blecore_evt->data;
          /* Check if a GMAP Client Instance with specified Connection Handle exists*/
          p_gmap_clt = GMAP_CLT_GetInstance(pr->Connection_Handle,&p_eatt_bearer);
          if (p_gmap_clt != 0)
          {
            /* Check if an ATT Procedure was started*/
            if (GMAP_IsATTProcedureInProgress(p_gmap_clt->pConnInfo->Connection_Handle,p_eatt_bearer) == BLE_STATUS_SUCCESS)
            {
              BLE_DBG_GMAP_MSG("ACI_GATT_ERROR_RESP_EVENT is received on conn handle %04X\n",pr->Connection_Handle);

              return_value = SVCCTL_EvtAckFlowEnable;
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
 * @brief GMAP Link Up process
 * @param  pGMAP_Clt: pointer on GMAP Client Instance
 * @param  ErrorCode: Error Code from Host Stack when a GATT procedure is complete
 * @retval status of the operation
 */
static tBleStatus GMAP_CLT_Linkup_Process(GMAP_CltInst_t *pGMAP_Clt,BleEATTBearer_t *pEATTBearer,uint8_t ErrorCode)
{
  tBleStatus hciCmdResult = BLE_STATUS_FAILED;
  uint16_t conn_handle = pGMAP_Clt->pConnInfo->Connection_Handle;
  if (pEATTBearer != 0)
  {
    conn_handle = (0xEA << 8) | (pEATTBearer->ChannelIdx);
  }

  BLE_DBG_GMAP_MSG("GMAP Link Up Process, state 0x%x\n",pGMAP_Clt->LinkupState);

  /*check that error indicate that linkup process succeeds*/
  if (ErrorCode == 0x00)
  {
    if (pGMAP_Clt->LinkupState == GMAP_LINKUP_DISC_SERVICE)
    {
      /* GMAP Link Up process is in first Step :
       * GMAS discovery in the remote GATT Database
       */
      if (pGMAP_Clt->ServiceEndHandle != 0x0000)
      {
        /* Gaming Audio Service has been found */
        BLE_DBG_GMAP_MSG("GATT : Discover GMAS Characteristics\n");
        pGMAP_Clt->pGattChar = 0u;
        /* Discover all the characteristics of the GMAS in the remote GATT Database */
        hciCmdResult = aci_gatt_disc_all_char_of_service(conn_handle,
                                                        pGMAP_Clt->ServiceStartHandle,
                                                        pGMAP_Clt->ServiceEndHandle);
        BLE_DBG_GMAP_MSG("aci_gatt_disc_all_char_of_service() (start: %04X, end: %04X) returns status 0x%x\n",
                            pGMAP_Clt->ServiceStartHandle,
                            pGMAP_Clt->ServiceEndHandle,
                            hciCmdResult);

        if (hciCmdResult == BLE_STATUS_SUCCESS)
        {
          if (pEATTBearer != 0)
          {
            BLE_AUDIO_STACK_EATT_RegisterATTProcedure(pEATTBearer->ChannelIdx,GMAP_ATT_PROCEDURE_ID);
          }
          else
          {
            BLE_AUDIO_STACK_ATT_RegisterATTProcedure(pGMAP_Clt->pConnInfo->Connection_Handle,GMAP_ATT_PROCEDURE_ID);
          }
          pGMAP_Clt->LinkupState = GMAP_LINKUP_DISC_CHAR;
        }
        else
        {
          /*Notify that GMAP Link Up is complete*/
          GMAP_CLT_Post_Linkup_Event(pGMAP_Clt,hciCmdResult);
        }
      }
      else
      {
        BLE_DBG_GMAP_MSG("No GMAS is present in the remote GATT Database\n");
        /*Notify that GMAP Link Up is complete*/
        GMAP_CLT_Post_Linkup_Event(pGMAP_Clt,BLE_STATUS_FAILED);
      }
    }
    else if (pGMAP_Clt->LinkupState == GMAP_LINKUP_DISC_CHAR)
    {
      /* Discovery of the characteristics in the Service is complete */
      /*Check if remote GMAS is valid*/
      if (GMAP_CLT_Check_GMAS_GMAPRoleCharacteristic(pGMAP_Clt) == BLE_STATUS_SUCCESS)
      {
        pGMAP_Clt->LinkupState = GMAP_LINKUP_READ_CHAR;
        pGMAP_Clt->CurrentLinkupChar = GMAS_CHAR_GMAP_ROLE;
        /* read the characteristic value */
        hciCmdResult = aci_gatt_read_char_value(conn_handle,pGMAP_Clt->GMAPRoleChar.ValueHandle);
        BLE_DBG_GMAP_MSG("aci_gatt_read_char_value() (connHandle: %04X, val_handle: %04X) returns status 0x%x\n",
                         conn_handle,
                         pGMAP_Clt->GMAPRoleChar.ValueHandle,
                         hciCmdResult);
        if (hciCmdResult != BLE_STATUS_SUCCESS)
        {
          pGMAP_Clt->LinkupState = GMAP_LINKUP_IDLE;
          GMAP_CLT_Post_Linkup_Event(pGMAP_Clt,hciCmdResult);
        }
        else
        {
          if (pEATTBearer != 0)
          {
            BLE_AUDIO_STACK_EATT_RegisterATTProcedure(pEATTBearer->ChannelIdx,GMAP_ATT_PROCEDURE_ID);
          }
          else
          {
            BLE_AUDIO_STACK_ATT_RegisterATTProcedure(pGMAP_Clt->pConnInfo->Connection_Handle,GMAP_ATT_PROCEDURE_ID);
          }
        }
      }
      else
      {
        /*Notify that GMAP Link Up is complete*/
        GMAP_CLT_Post_Linkup_Event(pGMAP_Clt, hciCmdResult);
      }
    }
    else if (pGMAP_Clt->LinkupState == GMAP_LINKUP_READ_CHAR)
    {
      uint16_t value_handle = 0x0000;

      /* Read the following characteristic, according to GMAP Role read */
      if (pGMAP_Clt->CurrentLinkupChar == GMAS_CHAR_GMAP_ROLE)
      {
        /*Check if remote GMAS is valid*/
        if (GMAP_CLT_Check_GMAS_FeaturesCharacteristics(pGMAP_Clt) == BLE_STATUS_SUCCESS)
        {
          if (pGMAP_Clt->GMAPRole & GMAP_ROLE_UNICAST_GAME_GATEWAY)
          {
            pGMAP_Clt->CurrentLinkupChar = GMAS_CHAR_UGG_FEATURES;
            value_handle = pGMAP_Clt->UGGFeaturesChar.ValueHandle;
          }
          else if (pGMAP_Clt->GMAPRole & GMAP_ROLE_UNICAST_GAME_TERMINAL)
          {
            pGMAP_Clt->CurrentLinkupChar = GMAS_CHAR_UGT_FEATURES;
            value_handle = pGMAP_Clt->UGTFeaturesChar.ValueHandle;
          }
          else if (pGMAP_Clt->GMAPRole & GMAP_ROLE_BROADCAST_GAME_SENDER)
          {
            pGMAP_Clt->CurrentLinkupChar = GMAS_CHAR_BGS_FEATURES;
            value_handle = pGMAP_Clt->BGSFeaturesChar.ValueHandle;
          }
          else if (pGMAP_Clt->GMAPRole & GMAP_ROLE_BROADCAST_GAME_RECEIVER)
          {
            pGMAP_Clt->CurrentLinkupChar = GMAS_CHAR_BGR_FEATURES;
            value_handle = pGMAP_Clt->BGRFeaturesChar.ValueHandle;
          }
        }
        else
        {
          /*Notify that GMAP Link Up is complete*/
          GMAP_CLT_Post_Linkup_Event(pGMAP_Clt, hciCmdResult);
        }
      }
      else if (pGMAP_Clt->CurrentLinkupChar == GMAS_CHAR_UGG_FEATURES)
      {
        if (pGMAP_Clt->GMAPRole & GMAP_ROLE_UNICAST_GAME_TERMINAL)
        {
          pGMAP_Clt->CurrentLinkupChar = GMAS_CHAR_UGT_FEATURES;
          value_handle = pGMAP_Clt->UGTFeaturesChar.ValueHandle;
        }
        else if (pGMAP_Clt->GMAPRole & GMAP_ROLE_BROADCAST_GAME_SENDER)
        {
          pGMAP_Clt->CurrentLinkupChar = GMAS_CHAR_BGS_FEATURES;
          value_handle = pGMAP_Clt->BGSFeaturesChar.ValueHandle;
        }
        else if (pGMAP_Clt->GMAPRole & GMAP_ROLE_BROADCAST_GAME_RECEIVER)
        {
          pGMAP_Clt->CurrentLinkupChar = GMAS_CHAR_BGR_FEATURES;
          value_handle = pGMAP_Clt->BGRFeaturesChar.ValueHandle;
        }
      }
      else if (pGMAP_Clt->CurrentLinkupChar == GMAS_CHAR_UGT_FEATURES)
      {
        if (pGMAP_Clt->GMAPRole & GMAP_ROLE_BROADCAST_GAME_SENDER)
        {
          pGMAP_Clt->CurrentLinkupChar = GMAS_CHAR_BGS_FEATURES;
          value_handle = pGMAP_Clt->BGSFeaturesChar.ValueHandle;
        }
        else if (pGMAP_Clt->GMAPRole & GMAP_ROLE_BROADCAST_GAME_RECEIVER)
        {
          pGMAP_Clt->CurrentLinkupChar = GMAS_CHAR_BGR_FEATURES;
          value_handle = pGMAP_Clt->BGRFeaturesChar.ValueHandle;
        }
      }
      else if (pGMAP_Clt->CurrentLinkupChar == GMAS_CHAR_BGS_FEATURES)
      {
        if (pGMAP_Clt->GMAPRole & GMAP_ROLE_BROADCAST_GAME_RECEIVER)
        {
          pGMAP_Clt->CurrentLinkupChar = GMAS_CHAR_BGR_FEATURES;
          value_handle = pGMAP_Clt->BGRFeaturesChar.ValueHandle;
        }
      }

      if (value_handle != 0x0000)
      {
        /* read the characteristic value */
        hciCmdResult = aci_gatt_read_char_value(conn_handle,value_handle);
        BLE_DBG_GMAP_MSG("aci_gatt_read_char_value() (connHandle: %04X, val_handle: %04X) returns status 0x%x\n",
                         conn_handle,
                         value_handle,
                         hciCmdResult);
        if (hciCmdResult != BLE_STATUS_SUCCESS)
        {
          pGMAP_Clt->LinkupState = GMAP_LINKUP_IDLE;
          GMAP_CLT_Post_Linkup_Event(pGMAP_Clt,hciCmdResult);
        }
        else
        {
          if (pEATTBearer != 0)
          {
            BLE_AUDIO_STACK_EATT_RegisterATTProcedure(pEATTBearer->ChannelIdx,GMAP_ATT_PROCEDURE_ID);
          }
          else
          {
            BLE_AUDIO_STACK_ATT_RegisterATTProcedure(pGMAP_Clt->pConnInfo->Connection_Handle,GMAP_ATT_PROCEDURE_ID);
          }
        }
      }
      else
      {
        /* Every characteristic has been read, linkup is complete */
        pGMAP_Clt->LinkupState = GMAP_LINKUP_IDLE;
        GMAP_CLT_Post_Linkup_Event(pGMAP_Clt,BLE_STATUS_SUCCESS);
      }
    }
  }
  else
  {
    /*Notify that GMAP Link Up is complete*/
    GMAP_CLT_Post_Linkup_Event(pGMAP_Clt,BLE_STATUS_FAILED);
  }
  return hciCmdResult;
}

/**
 * @brief Notify to upper layer that GMAP Link Up Process is complete
 * @param  pGMAP_Clt: pointer on GMAP Client Instance
 * @param  Status : status of the operation
 */
static void GMAP_CLT_Post_Linkup_Event(GMAP_CltInst_t *pGMAP_Clt, tBleStatus const Status)
{
  GMAP_Notification_Evt_t evt;
  GMAP_AttServiceInfo_Evt_t parms;

  BLE_DBG_GMAP_MSG("GMAP Client notifies GMAP Linkup is complete with status 0x%x\n",Status);

  if (pGMAP_Clt->pConnInfo != 0)
  {
    evt.ConnHandle = pGMAP_Clt->pConnInfo->Connection_Handle;

    evt.Status = Status;

    pGMAP_Clt->LinkupState = GMAP_LINKUP_COMPLETE;
    evt.EvtOpcode = (GMAP_NotCode_t) GMAP_LINKUP_COMPLETE_EVT;
    if (evt.Status == BLE_STATUS_SUCCESS)
    {
      parms.GMAPRole = pGMAP_Clt->GMAPRole;
      parms.UGGFeatures = pGMAP_Clt->UGGFeatures;
      parms.UGTFeatures = pGMAP_Clt->UGTFeatures;
      parms.BGSFeatures = pGMAP_Clt->BGSFeatures;
      parms.BGRFeatures = pGMAP_Clt->BGRFeatures;
      parms.StartAttHandle = pGMAP_Clt->ServiceStartHandle;
      parms.EndAttHandle = pGMAP_Clt->ServiceEndHandle;
      evt.pInfo = (void *) &parms;
    }
    else
    {
      evt.pInfo = 0;
    }
    GMAP_Notification(&evt);
    if (Status != BLE_STATUS_SUCCESS)
    {
      BLE_DBG_GMAP_MSG("Linkup has failed, reset the GMAP Client Instance\n");
      GMAP_CLT_InitInstance(pGMAP_Clt);
    }
  }
  else
  {
    BLE_DBG_GMAP_MSG("unknown connection, reset the GMAP Client Instance\n");
    GMAP_CLT_InitInstance(pGMAP_Clt);
  }
}

/**
 * @brief Check GMAS Validity on GMAP Role Characteristic
 * @param  pGMAP_Clt: pointer on GMAP Client Instance
 * @retval status of the operation
 */
tBleStatus GMAP_CLT_Check_GMAS_GMAPRoleCharacteristic(GMAP_CltInst_t *pGMAP_Clt)
{
  tBleStatus status = BLE_STATUS_SUCCESS;

  if (pGMAP_Clt->GMAPRoleChar.ValueHandle == 0x0000u)
  {
    BLE_DBG_GMAP_MSG("Error : GMAP Role Chararacteristic is not found in remote GATT Database\n");
    status = BLE_STATUS_FAILED;
  }
  else
  {
    /*Check if the characteristic properties are valid*/
    if (pGMAP_Clt->GMAPRoleChar.Properties != CHAR_PROP_READ)
    {
      BLE_DBG_GMAP_MSG("Error : Properties 0x%02X of the GMAP Role Characteristic is not valid\n",
                  pGMAP_Clt->GMAPRoleChar.Properties);
      status = BLE_STATUS_FAILED;
    }
  }

  if (status == BLE_STATUS_SUCCESS)
  {
    BLE_DBG_GMAP_MSG("GMAP Role Characteristic is valid\n");
  }
  return status;
}

/**
 * @brief Check GMAS Validity on Features Characteristic
 * @param  pGMAP_Clt: pointer on GMAP Client Instance
 * @retval status of the operation
 */
tBleStatus GMAP_CLT_Check_GMAS_FeaturesCharacteristics(GMAP_CltInst_t *pGMAP_Clt)
{
  if (pGMAP_Clt->GMAPRole & GMAP_ROLE_UNICAST_GAME_GATEWAY)
  {
    /* Check if the characteristic exists */
    if (pGMAP_Clt->UGGFeaturesChar.ValueHandle == 0x0000u)
    {
      BLE_DBG_GMAP_MSG("Error : UGG Features Chararacteristic is not found in remote GATT Database\n");
      return BLE_STATUS_FAILED;
    }
    /*Check if the characteristic properties are valid*/
    else if (pGMAP_Clt->UGGFeaturesChar.Properties != CHAR_PROP_READ)
    {
      BLE_DBG_GMAP_MSG("Error : Properties 0x%02X of the UGG Features Characteristic is not valid\n",
                  pGMAP_Clt->UGGFeaturesChar.Properties);
      return BLE_STATUS_FAILED;
    }
  }

  if (pGMAP_Clt->GMAPRole & GMAP_ROLE_UNICAST_GAME_TERMINAL)
  {
    /* Check if the characteristic exists */
    if (pGMAP_Clt->UGTFeaturesChar.ValueHandle == 0x0000u)
    {
      BLE_DBG_GMAP_MSG("Error : UGT Features Chararacteristic is not found in remote GATT Database\n");
      return BLE_STATUS_FAILED;
    }
    /*Check if the characteristic properties are valid*/
    else if (pGMAP_Clt->UGTFeaturesChar.Properties != CHAR_PROP_READ)
    {
      BLE_DBG_GMAP_MSG("Error : Properties 0x%02X of the UGT Features Characteristic is not valid\n",
                  pGMAP_Clt->UGTFeaturesChar.Properties);
      return BLE_STATUS_FAILED;
    }
  }

  if (pGMAP_Clt->GMAPRole & GMAP_ROLE_BROADCAST_GAME_SENDER)
  {
    /* Check if the characteristic exists */
    if (pGMAP_Clt->BGSFeaturesChar.ValueHandle == 0x0000u)
    {
      BLE_DBG_GMAP_MSG("Error : BGS Feature Chararacteristic is not found in remote GATT Database\n");
      return BLE_STATUS_FAILED;
    }
    /*Check if the characteristic properties are valid*/
    else if (pGMAP_Clt->BGSFeaturesChar.Properties != CHAR_PROP_READ)
    {
      BLE_DBG_GMAP_MSG("Error : Properties 0x%02X of the BGS Features Characteristic is not valid\n",
                  pGMAP_Clt->BGSFeaturesChar.Properties);
      return BLE_STATUS_FAILED;
    }
  }

  if (pGMAP_Clt->GMAPRole & GMAP_ROLE_BROADCAST_GAME_RECEIVER)
  {
    /* Check if the characteristic exists */
    if (pGMAP_Clt->BGRFeaturesChar.ValueHandle == 0x0000u)
    {
      BLE_DBG_GMAP_MSG("Error : BGR Feature Chararacteristic is not found in remote GATT Database\n");
      return BLE_STATUS_FAILED;
    }
    /*Check if the characteristic properties are valid*/
    else if (pGMAP_Clt->BGRFeaturesChar.Properties != CHAR_PROP_READ)
    {
      BLE_DBG_GMAP_MSG("Error : Properties 0x%02X of the BGR Features Characteristic is not valid\n",
                  pGMAP_Clt->BGRFeaturesChar.Properties);
      return BLE_STATUS_FAILED;
    }
  }

  BLE_DBG_GMAP_MSG("All GMAS Feature Characteristics are valids\n");
  return BLE_STATUS_SUCCESS;
}

/**
  * @brief Initialize a GMAP Client Instance
  * @param pGMAP_Clt: pointer on GMAP Client Instance
  */
static void GMAP_CLT_InitInstance(GMAP_CltInst_t *pGMAP_Clt)
{
  BLE_DBG_GMAP_MSG("Initialize GMAP Client Instance\n");
  /*Initialize the GMAP_CltInst_t structure*/
  pGMAP_Clt->LinkupState = GMAP_LINKUP_IDLE;
  pGMAP_Clt->ReqHandle = 0x0000;
  pGMAP_Clt->pConnInfo = 0;
  pGMAP_Clt->DelayDeallocation = 0u;
  pGMAP_Clt->GMAPRoleChar.ValueHandle = 0x0000u;
  pGMAP_Clt->GMAPRoleChar.DescHandle = 0x0000u;
  pGMAP_Clt->GMAPRoleChar.Properties = 0x00u;
  pGMAP_Clt->GMAPRoleChar.EndHandle = 0x0000u;
  pGMAP_Clt->GMAPRole = 0x00u;
  pGMAP_Clt->UGGFeaturesChar.ValueHandle = 0x0000u;
  pGMAP_Clt->UGGFeaturesChar.DescHandle = 0x0000u;
  pGMAP_Clt->UGGFeaturesChar.Properties = 0x00u;
  pGMAP_Clt->UGGFeaturesChar.EndHandle = 0x0000u;
  pGMAP_Clt->UGGFeatures = 0x00u;
  pGMAP_Clt->UGTFeaturesChar.ValueHandle = 0x0000u;
  pGMAP_Clt->UGTFeaturesChar.DescHandle = 0x0000u;
  pGMAP_Clt->UGTFeaturesChar.Properties = 0x00u;
  pGMAP_Clt->UGTFeaturesChar.EndHandle = 0x0000u;
  pGMAP_Clt->UGTFeatures = 0x00u;
  pGMAP_Clt->BGSFeaturesChar.ValueHandle = 0x0000u;
  pGMAP_Clt->BGSFeaturesChar.DescHandle = 0x0000u;
  pGMAP_Clt->BGSFeaturesChar.Properties = 0x00u;
  pGMAP_Clt->BGSFeaturesChar.EndHandle = 0x0000u;
  pGMAP_Clt->BGSFeatures = 0x00u;
  pGMAP_Clt->BGRFeaturesChar.ValueHandle = 0x0000u;
  pGMAP_Clt->BGRFeaturesChar.DescHandle = 0x0000u;
  pGMAP_Clt->BGRFeaturesChar.Properties = 0x00u;
  pGMAP_Clt->BGRFeaturesChar.EndHandle = 0x0000u;
  pGMAP_Clt->BGRFeatures = 0x00u;
  pGMAP_Clt->ServiceStartHandle = 0x0000;
  pGMAP_Clt->ServiceEndHandle = 0x0000;
  pGMAP_Clt->pGattChar = 0;
  pGMAP_Clt->ErrorCode = 0x00u;
}


static GMAP_CltInst_t *GMAP_CLT_GetAvailableInstance(void)
{
  uint8_t i;

  /*Get a free GMAP Client Instance*/
  for (i = 0 ; i < USECASE_DEV_MGMT_MAX_CONNECTION ; i++)
  {
    if (GMAP_Context.CltInst[i].pConnInfo == 0)
    {
      return &GMAP_Context.CltInst[i];
    }
  }
  return 0u;
}

static GMAP_CltInst_t *GMAP_CLT_GetInstance(uint16_t ConnHandle,BleEATTBearer_t **pEATTBearer)
{
  uint8_t i;
#if (CFG_BLE_EATT_BEARER_PER_LINK > 0)
  /*Check if the connection handle corresponds to a Connection Oriented Channel*/
  if ((uint8_t) (ConnHandle >> 8) == 0xEA)
  {
    for (i = 0 ; i < USECASE_DEV_MGMT_MAX_CONNECTION ; i++)
    {
      if (GMAP_Context.CltInst[i].pConnInfo != 0)
      {
        if (GMAP_Context.CltInst[i].pConnInfo->Connection_Handle != 0xFFFFu)
        {
          for (uint8_t j = 0u; j < CFG_BLE_EATT_BEARER_PER_LINK; j++)
          {
            if ( (GMAP_Context.CltInst[i].pConnInfo->aEATTBearer[j].State == 0u) \
                && (GMAP_Context.CltInst[i].pConnInfo->aEATTBearer[j].ChannelIdx == (uint8_t)(ConnHandle)))
            {
              *pEATTBearer = &GMAP_Context.CltInst[i].pConnInfo->aEATTBearer[j];
              return &GMAP_Context.CltInst[i];
            }
          }
        }
      }
    }
  }
  else
#endif /* (CFG_BLE_EATT_BEARER_PER_LINK > 0) */
  {
    for (i = 0 ; i < USECASE_DEV_MGMT_MAX_CONNECTION ; i++)
    {
      if (GMAP_Context.CltInst[i].pConnInfo != 0
          && GMAP_Context.CltInst[i].pConnInfo->Connection_Handle == ConnHandle)
      {
        return &GMAP_Context.CltInst[i];
      }
    }
  }
  return 0u;
}


static tBleStatus GMAP_IsATTProcedureInProgress(uint16_t ConnHandle,BleEATTBearer_t *pEATTBearer)
{

  tBleStatus status = BLE_STATUS_FAILED;

  if (pEATTBearer != 0)
  {
    if (BLE_AUDIO_STACK_EATT_IsBearerRegisteredATTProcedure(pEATTBearer->ChannelIdx,
                                                            GMAP_ATT_PROCEDURE_ID) == BLE_STATUS_SUCCESS)
    {
      status = BLE_STATUS_SUCCESS;
    }
  }
  else
  {
    /* Check if an ATT Procedure was started*/
    if (BLE_AUDIO_STACK_ATT_IsRegisteredATTProcedure(ConnHandle,GMAP_ATT_PROCEDURE_ID) == BLE_STATUS_SUCCESS)
    {
      status = BLE_STATUS_SUCCESS;
    }
  }
  return status;
}
