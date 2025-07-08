/**
  ******************************************************************************
  * @file    tmap_db.c
  * @author  MCD Application Team
  * @brief   This file contains interfaces of TMAP Database
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
#include "tmap.h"
#include "tmap_log.h"
#include "tmap_alloc.h"
#include "ble_gap_aci.h"
#include "ble_audio_stack.h"
/* Private typedef -----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions prototype------------------------------------------------*/
static tBleStatus TMAP_CLT_GetTMASDatabase(TMAP_CltInst_t const *pTMAP_CltInst,
                                           uint16_t UUID,
                                           uint8_t *pData,
                                           uint16_t MaxDataLen,
                                           uint16_t *len);
static tBleStatus TMAP_CLT_SetDatabase(TMAP_CltInst_t *pTMAP_CltInst,
                                       uint8_t *pData,
                                       uint16_t Len);
/* External functions prototype------------------------------------------------*/
extern tBleStatus TMAP_CLT_Check_TMAS_Service(TMAP_CltInst_t *pTMAP_Ctlr);
/* Functions Definition ------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/
void TMAP_CLT_StoreDatabase(TMAP_CltInst_t const *pTMAP_CltInst)
{
  uint16_t                len = 0u;
  uint16_t                remain_len = TMAP_GATT_DATABASE_SIZE;
  uint8_t                 temp_database[TMAP_GATT_DATABASE_SIZE]= {0};
  tBleStatus              status;
  uint16_t                total_len = 0u;
  UseCaseConnInfo_t *p_conn_info;
  BleEATTBearer_t   *p_eatt_bearer = 0;

  BLE_DBG_TMAP_MSG("Store TMAP GATT Database associated to remote device on Connection Handle 0x%04X\n",
                      pTMAP_CltInst->pConnInfo->Connection_Handle);

  if (USECASE_DEV_MGMT_GetConnInfo(pTMAP_CltInst->pConnInfo->Connection_Handle,&p_conn_info,&p_eatt_bearer) == BLE_STATUS_SUCCESS)
  {
    if (aci_gap_check_bonded_device(p_conn_info->Peer_Address_Type,
                                      &p_conn_info->Peer_Address[0],
                                     (uint8_t *)&p_conn_info->Peer_Address_Type,
                                     (uint8_t *)p_conn_info->Peer_Address) == BLE_STATUS_SUCCESS)
    {
      /*Get ATT Database of the TMAS*/
      status = TMAP_CLT_GetTMASDatabase(pTMAP_CltInst,
                                      TELEPHONY_AND_MEDIA_AUDIO_SERVICE_UUID,
                                      &temp_database[(TMAP_GATT_DATABASE_SIZE-remain_len)],
                                      remain_len,&len);
      if (status == BLE_STATUS_SUCCESS)
      {
        BLE_DBG_TMAP_MSG("%d bytes of TMAS Information database to store in NVM\n",len);
        total_len += len;
      }
      else
      {
        BLE_DBG_TMAP_MSG("Get ATT Database information of TMAS has failed (status 0x%02X)\n",status);
      }
      if (status == BLE_STATUS_SUCCESS)
      {
        status = BLE_AUDIO_STACK_DB_AddRecord(BLE_AUDIO_DB_TYPE_TMAP,
                                             p_conn_info->Peer_Address_Type,
                                             p_conn_info->Peer_Address,
                                             temp_database,
                                             total_len);
        BLE_DBG_TMAP_MSG("BLE_AUDIO_STACK_DB_AddRecord() returns status 0x%02X\n",status);
      }
    }
  }
}


tBleStatus TMAP_CLT_RestoreDatabase(TMAP_CltInst_t *pTMAP_CltInst)
{
  tBleStatus              status = BLE_STATUS_SUCCESS;
  uint8_t                 temp_database[TMAP_GATT_DATABASE_SIZE];
  uint16_t                dataLen = 0u;

  BLE_DBG_TMAP_MSG("Start Restoration of TMAP Profile\n",dataLen);

  if (aci_gap_check_bonded_device(pTMAP_CltInst->pConnInfo->Peer_Address_Type,
                                  &pTMAP_CltInst->pConnInfo->Peer_Address[0],
                                  (uint8_t *)&pTMAP_CltInst->pConnInfo->Peer_Address_Type,
                                  (uint8_t *)pTMAP_CltInst->pConnInfo->Peer_Address) == BLE_STATUS_SUCCESS)
  {
    if (BLE_AUDIO_STACK_DB_GetRecord(BLE_AUDIO_DB_TYPE_TMAP,
                                     pTMAP_CltInst->pConnInfo->Peer_Address_Type,
                                     pTMAP_CltInst->pConnInfo->Peer_Address,
                                     temp_database,
                                     TMAP_GATT_DATABASE_SIZE,
                                     &dataLen) == BLE_STATUS_SUCCESS)
    {
      /*Restore the TMAP */
      status = TMAP_CLT_SetDatabase((TMAP_CltInst_t *)pTMAP_CltInst,&temp_database[0],dataLen);
      BLE_DBG_TMAP_MSG("Restore TMAP Database returns status 0x%02X\n",status);

      if (status == BLE_STATUS_SUCCESS)
      {
        TMAP_Notification_Evt_t evt;
        evt.Status = BLE_STATUS_SUCCESS;
        pTMAP_CltInst->LinkupState = TMAP_LINKUP_COMPLETE;
        evt.EvtOpcode = TMAP_LINKUP_COMPLETE_EVT;
        evt.ConnHandle = pTMAP_CltInst->pConnInfo->Connection_Handle;
        TMAP_Notification(&evt);
      }
      else
      {
        status = BLE_STATUS_FAILED;
      }
    }
    else
    {
      BLE_DBG_TMAP_MSG("Peer Device has no TMAP information in NVM\n");
      status = BLE_STATUS_FAILED;
    }
  }
  else
  {
    status = BLE_STATUS_FAILED;
  }
  return status;
}

/**
  * @brief  Remove the record of the services in TMAP stored in the Non Volatile memory.
  * @param  PeerIdentityAddressType: Identity address type.
  *                                  Values:
  *                                     - 0x00: Public Identity Address
  *                                     - 0x01: Random (static) Identity Address
  * @param  PeerIdentityAddress : Public or Random (static) Identity address of the peer device
  * @retval status of the operation
  */
tBleStatus TMAP_DB_RemoveServicesRecord(uint8_t PeerIdentityAddressType,const uint8_t PeerIdentityAddress[6])
{
  uint8_t cl_addr[6u] = {0};
  uint8_t type;
  tBleStatus status = BLE_STATUS_SUCCESS;

  status = aci_gap_check_bonded_device(PeerIdentityAddressType,
                                         &PeerIdentityAddress[0],
                                         &type,
                                         &cl_addr[0]);
  if (status == BLE_STATUS_SUCCESS)
  {
    status = BLE_AUDIO_STACK_DB_RemoveRecord(BLE_AUDIO_DB_TYPE_TMAP,type,&cl_addr[0]);
    BLE_DBG_TMAP_MSG("Remove TMAP information in NVM returns status 0x%02X\n",status);
  }
  return status;
}

/**
  * @brief Indicate if TMAP Database is saved in NVM
  * @param Peer_Address_Type: Peer Address type
  * @param Peer_Address: Peer Address
  * @retval 0 if not present in the database, else 1
  */
uint8_t TMAP_DB_IsPresent(uint8_t Peer_Address_Type,const uint8_t Peer_Address[6])
{
  uint8_t       cl_addr[6u] = {0u};
  uint8_t       type;
  tBleStatus    status = BLE_STATUS_SUCCESS;

  status = aci_gap_check_bonded_device(Peer_Address_Type,
                                       &Peer_Address[0],
                                       &type,
                                       &cl_addr[0]);
  if (status != BLE_STATUS_SUCCESS)
  {
    BLE_DBG_TMAP_MSG("Check TMAP Services Record fails because BLE Address has not been successfully resolved\n");
    return status;
  }
  /*Find in database the record associated to the remote device*/
  if (BLE_AUDIO_STACK_DB_FindRecord(BLE_AUDIO_DB_TYPE_TMAP,type,cl_addr) == BLE_STATUS_SUCCESS)
  {
    BLE_DBG_TMAP_MSG("Peer Device has TMAP information in NVM\n");
    return 1u;
  }
  else
  {
    BLE_DBG_TMAP_MSG("Peer Device has no TMAP information in NVM\n");
  }
  return 0u;
}

/* Private functions ----------------------------------------------------------*/
static tBleStatus TMAP_CLT_GetCharacteristicInfo(TMAP_GATT_CharacteristicInfo_t *pCharInfo,
                                                 uint16_t uuid,
                                                 uint8_t *pData,
                                                 uint16_t MaxDataLen,
                                                 uint16_t *len)
{
  tBleStatus hciCmdResult = BLE_STATUS_SUCCESS;
  uint8_t info_len = 9u;

  *len = 0u;

  if (pCharInfo->ValueHandle != 0x0000u)
  {
    if (MaxDataLen >= info_len)
    {
      pData[0] = (uint8_t) (pCharInfo->ValueHandle);
      pData[1] = (uint8_t) ((pCharInfo->ValueHandle >> 8 ));
      /* Characteristic Type*/
      pData[2] = UUID_TYPE_16;
      /* Characteristic UUID*/
      pData[3] = (uint8_t) uuid;
      pData[4] = (uint8_t) ((uuid >> 8 ));
      /*value length*/
      pData[5] = 3u;
      /*Properties*/
      pData[6] = pCharInfo->Properties;
      /*Descriptor handle*/
      pData[7] = (uint8_t) (pCharInfo->DescHandle);
      pData[8] = (uint8_t) ((pCharInfo->DescHandle >> 8 ));
      *len = info_len;
    }
    else
    {
      BLE_DBG_TMAP_MSG("No sufficient resource to store data of characteristic UUID 0x%04X\n",uuid);
      hciCmdResult = BLE_STATUS_INSUFFICIENT_RESOURCES;
    }
  }
  return hciCmdResult;
}

static tBleStatus TMAP_CLT_GetTMASDatabase(TMAP_CltInst_t const *pTMAP_CltInst,
                                          uint16_t UUID,
                                          uint8_t *pData,
                                          uint16_t MaxDataLen,
                                          uint16_t *len)
{
  tBleStatus                     hciCmdResult = BLE_STATUS_SUCCESS;
  uint16_t                       value_length;
  uint16_t                       remain = MaxDataLen;
  TMAP_GATT_CharacteristicInfo_t *p_char_info;
  uint16_t                       uuid;
  *len = 0u;

  if (pTMAP_CltInst->LinkupState == TMAP_LINKUP_COMPLETE)
  {
    /* Store Service information*/
    value_length = 2u;
    if (remain >= (value_length + 6u))
    {
      /* Store :
       * - 2 bytes for Service Start Handle Attribute handle
       * - 1 byte for Characteristic type
       * - 2 bytes for Service UUID
       * - 1 byte to indicate data length
       */
      /* Service Characteristic Attribute handle */
      pData[(*len)] = (uint8_t) (pTMAP_CltInst->ServiceStartHandle);
      pData[(*len)+1] = (uint8_t) ((pTMAP_CltInst->ServiceStartHandle >> 8 ));
      /* Characteristic Type*/
      pData[(*len)+2] = UUID_TYPE_16;
      /* UUID*/
      pData[(*len)+3] = (uint8_t) UUID;
      pData[(*len)+4] = (uint8_t) ((UUID >> 8 ));
      /*value length*/
      pData[(*len)+5] = value_length;
      /*save Service End Handle*/
      pData[(*len)+6] = (uint8_t) (pTMAP_CltInst->ServiceEndHandle);
      pData[(*len)+7] = (uint8_t) ((pTMAP_CltInst->ServiceEndHandle >> 8 ));
      remain = (remain - (value_length + 6u));
      *len += (value_length + 6u);
    }
    else
    {
      BLE_DBG_TMAP_MSG("No sufficient resource to store data of Service UUID 0x%04X\n",UUID);
      return BLE_STATUS_INSUFFICIENT_RESOURCES;
    }


    p_char_info = (TMAP_GATT_CharacteristicInfo_t *)&pTMAP_CltInst->TMAPRoleChar;
    uuid = TMAP_ROLE_UUID;
    hciCmdResult = TMAP_CLT_GetCharacteristicInfo(p_char_info,
                                                  uuid,
                                                  &pData[(*len)],
                                                  remain,
                                                  &value_length);
    if ( hciCmdResult == BLE_STATUS_SUCCESS)
    {
      *len += value_length;
    }
  }
  return hciCmdResult;
}

static tBleStatus TMAP_CLT_SetTMAPDatabase(TMAP_CltInst_t *pTMAP_CltInst,
                                           uint8_t *pData,
                                           uint16_t Len,
                                           uint16_t *read_len)
{
  tBleStatus    status = BLE_STATUS_SUCCESS;
  uint16_t      data_index = 0u;
  uint16_t      att_handle;
  uint8_t       uuid_type;
  uint16_t      uuid_value;
  uint8_t       data_len;

  *read_len = 0u;

  /*Get Service information*/
  att_handle = pData[data_index] | (pData[data_index+1u] << 8);
  uuid_type = pData[data_index+2u];
  uuid_value = pData[data_index+3u] | (pData[data_index+4u] << 8);
  data_len = pData[data_index+5u];
  pTMAP_CltInst->ServiceStartHandle = att_handle;
  pTMAP_CltInst->ServiceEndHandle = pData[data_index+6u] | (pData[data_index+7u] << 8);
  BLE_DBG_TMAP_MSG("Restore TMAS Service UUID 0x%04X information : Start Handle 0x%04X, End Handle 0x%04X\n",
                      uuid_value,
                      pTMAP_CltInst->ServiceStartHandle,
                      pTMAP_CltInst->ServiceEndHandle);
  data_index += (data_len + 6u);
  if (data_index < Len)
  {
    status = BLE_STATUS_PENDING;
    /*Restore the characteristic associated to the Service*/
    while ((data_index < Len) && (status == BLE_STATUS_PENDING))
    {
      att_handle = pData[data_index] | (pData[data_index+1u] << 8);
      uuid_type = pData[data_index+2u];
      uuid_value = pData[data_index+3u] | (pData[data_index+4u] << 8);
      data_len = pData[data_index+5u];

      if (uuid_type == UUID_TYPE_16)
      {
        switch (uuid_value)
        {
          case TMAP_ROLE_UUID:
            pTMAP_CltInst->TMAPRoleChar.ValueHandle = att_handle;
            pTMAP_CltInst->TMAPRoleChar.Properties = pData[data_index+6u];
            pTMAP_CltInst->TMAPRoleChar.DescHandle = pData[data_index+7u] | (pData[data_index+8u] << 8);
            BLE_DBG_TMAP_MSG("TMAP Role Characteristic is restored:\n");
            BLE_DBG_TMAP_MSG("Characteristic Properties = 0x%02X\n",pTMAP_CltInst->TMAPRoleChar.Properties);
            BLE_DBG_TMAP_MSG("Value Handle = 0x%04X\n",att_handle);
            BLE_DBG_TMAP_MSG("Descriptor Handle = 0x%04X\n",pTMAP_CltInst->TMAPRoleChar.DescHandle);
          break;
          default:
            BLE_DBG_TMAP_MSG("UUID 0x%04X doesn't correspond to a TMAP Characteristic\n",uuid_value);
            status = BLE_STATUS_SUCCESS;
            break;
        }
        if (status == BLE_STATUS_PENDING)
        {
          data_index += (data_len + 6u);
        }
      }
      else
      {
        /* uuid type is not valid*/
        status = BLE_STATUS_FAILED;
      }
    }
  }
  else
  {
    /* no characteristic assocaed to the service*/
    status = BLE_STATUS_FAILED;
  }
  /*Check if TMAS Service is valid*/
  if ((status == BLE_STATUS_SUCCESS) || (status == BLE_STATUS_PENDING))
  {
    status = TMAP_CLT_Check_TMAS_Service(pTMAP_CltInst);
  }
  *read_len = data_index;
  return status;
}

static tBleStatus TMAP_CLT_SetDatabase(TMAP_CltInst_t *pTMAP_CltInst, uint8_t *pData,uint16_t Len)
{

  tBleStatus    status = BLE_STATUS_SUCCESS;
  uint16_t      data_index = 0u;
  uint8_t       uuid_type;
  uint16_t      uuid_value;
  uint8_t       data_len;
  uint16_t      read_len;

  /* Restore the service in the TMAP Client Instance*/
  while ((data_index < Len) && (status == BLE_STATUS_SUCCESS))
  {
    uuid_type = pData[data_index+2u];
    uuid_value = pData[data_index+3u] | (pData[data_index+4u] << 8);
    data_len = pData[data_index+5u];
    if ((uuid_type == UUID_TYPE_16) && (uuid_value == TELEPHONY_AND_MEDIA_AUDIO_SERVICE_UUID))
    {
      status = TMAP_CLT_SetTMAPDatabase(pTMAP_CltInst,
                                        &pData[data_index],
                                        (Len - data_index),
                                        &read_len);
      data_index += read_len;
    }
    else
    {
      data_index += (data_len + 6u);
    }
  }
  return status;
}