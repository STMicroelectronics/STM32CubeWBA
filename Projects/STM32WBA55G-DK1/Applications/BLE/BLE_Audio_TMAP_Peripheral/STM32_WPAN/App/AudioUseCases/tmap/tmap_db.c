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
#include "tmap_db.h"
#include "tmas.h"
#include "usecase_dev_mgmt.h"
#include "ble_gap_aci.h"
/* Private typedef -----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/

#define TMAP_GATT_DATABASE_SIZE            (17u)

/* Database record header length in  bytes :
 * validity (1byte) + 6 bytes for BDAddr + 1 unused + 4 bytes length of data
 * in order to be aligned we define a Header length of 12 bytes and declare it
 * as an array of unsigned U32 of 12/4 size.
 * [0] - Valididy + 3 BD addr
 * [1] - 3 BD addr + 1 unused
 * [2] - 4 Length byte.
 */
#define BLENVM_TMAP_HDR_LEN              (12u)

#define BLEAUDIO_PLAT_NVM_TYPE_TMAP      (0x0A)
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
static int TMAP_FindDatabaseRecord(const uint8_t* devAddress,uint32_t* dataLen );
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
  uint8_t                 cl_addr[6u] = {0};
  uint8_t                 i;
  uint32_t                hdr[BLENVM_TMAP_HDR_LEN / 4];
  uint16_t                total_len = 0u;
  uint32_t                data_len = 0u;
  const UseCaseConnInfo_t *p_conn_info;

  BLE_DBG_TMAP_MSG("Store TMAP GATT Database associated to remote device on Connection Handle 0x%04X\n",
                      pTMAP_CltInst->ConnHandle);

  if (USECASE_DEV_MGMT_GetConnInfo(pTMAP_CltInst->ConnHandle,&p_conn_info) == BLE_STATUS_SUCCESS)
  {
    /* Check if the address is a resolvable private address */
    if (((p_conn_info->Peer_Address_Type) == 1u) && ((p_conn_info->Peer_Address[5] & 0xC0U) == 0x40U))
    {
      status = aci_gap_resolve_private_addr( &p_conn_info->Peer_Address[0], &cl_addr[0]);
      if (status != BLE_STATUS_SUCCESS)
      {
        BLE_DBG_TMAP_MSG("Store TMAP GATT Database is aborted because BLE Address has not been successfully resolved\n");
        return;
      }
    }
    else
    {
      for (i = 0; i < 6u ; i++)
      {
        cl_addr[i] = p_conn_info->Peer_Address[i];
      }
    }

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
    /*Find in database the record associated to the remote device*/
    if (TMAP_FindDatabaseRecord(cl_addr,&data_len) == BLEAUDIO_PLAT_NVM_OK)
    {

      BLE_DBG_TMAP_MSG("Compare already stored TMAP database of length %d bytes vs current MCP Database of length %d\n",
                          data_len,
                          total_len);
      /* Compare data from byte 12 */
      if ( (total_len == data_len) &&
           (BLE_AUDIO_PLAT_NvmCompare( BLENVM_TMAP_HDR_LEN, temp_database, data_len )
            == BLEAUDIO_PLAT_NVM_OK) )
      {

        BLE_DBG_TMAP_MSG("TMAP database hasn't changed, no need to perform NVM Save operation\n");
        /* Return directly as we have found the same data already in NVM */
        return ;
      }

      BLE_DBG_TMAP_MSG("TMAP database has changed\n");
      /* Invalidate current record because data has changed */
      BLE_AUDIO_PLAT_NvmDiscard( BLEAUDIO_PLAT_NVM_CURRENT );
    }
    else
    {
      BLE_DBG_TMAP_MSG("Peer Device has no TMAP information in NVM\n");
    }


    ((uint8_t*)(hdr + 0))[0] = 0xFDU;
    ((uint8_t*)(hdr + 0))[1] = cl_addr[0];
    ((uint8_t*)(hdr + 0))[2] = cl_addr[1];
    ((uint8_t*)(hdr + 0))[3] = cl_addr[2];
    ((uint8_t*)(hdr + 1))[0] = cl_addr[3];
    ((uint8_t*)(hdr + 1))[1] = cl_addr[4];
    ((uint8_t*)(hdr + 1))[2] = cl_addr[5];
    ((uint8_t*)(hdr + 1))[3] = 0xFFU;

    /* the last byte of the flash word containing the address will have 0xFF.
     * The length of the database will be at the next word offset in flash.
     */
    hdr[2] = total_len;
    BLE_DBG_TMAP_MSG("Store %d bytes of TMAP Information database in NVM for peer device 0x%02x%02x%02x%02x%02x%02x\n",
                        total_len,
                        cl_addr[5],
                        cl_addr[4],
                        cl_addr[3],
                        cl_addr[2],
                        cl_addr[1],
                        cl_addr[0]);
    status = BLE_AUDIO_PLAT_NvmAdd( BLEAUDIO_PLAT_NVM_TYPE_TMAP,(uint8_t*)hdr, BLENVM_TMAP_HDR_LEN, temp_database, total_len );
    BLE_DBG_TMAP_MSG("BLE_AUDIO_PLAT_NvmAdd() returns status 0x%02X\n",status);
  }
}


tBleStatus TMAP_CLT_RestoreDatabase(TMAP_CltInst_t *pTMAP_CltInst)
{
  uint8_t                 cl_addr[6u] = {0};
  tBleStatus              status = BLE_STATUS_SUCCESS;
  uint8_t                 temp_database[TMAP_GATT_DATABASE_SIZE];
  uint8_t                 i;
  uint32_t                dataLen = 0u;
  const UseCaseConnInfo_t *p_conn_info;

  BLE_DBG_TMAP_MSG("Start Restoration of TMAP Profile\n",dataLen);

  if (USECASE_DEV_MGMT_GetConnInfo(pTMAP_CltInst->ConnHandle,&p_conn_info) == BLE_STATUS_SUCCESS)
  {
    /* Check if the address is a resolvable private address */
    if (((p_conn_info->Peer_Address_Type) == 1u) && ((p_conn_info->Peer_Address[5] & 0xC0U) == 0x40U))
    {
      status = aci_gap_resolve_private_addr( &p_conn_info->Peer_Address[0], &cl_addr[0]);
      if (status != BLE_STATUS_SUCCESS)
      {
        BLE_DBG_TMAP_MSG("Restore TMAP Profile is aborted because BLE Address has not been successfully resolved\n");
        return status;
      }
    }
    else
    {
      for (i = 0; i < 6u ; i++)
      {
        cl_addr[i] = p_conn_info->Peer_Address[i];
      }
    }

    /*Find in database the record associated to the remote device*/
    if (TMAP_FindDatabaseRecord(cl_addr,&dataLen) == BLEAUDIO_PLAT_NVM_OK)
    {
      BLE_DBG_TMAP_MSG("Peer Device has TMAP information (%d bytes) in NVM\n",dataLen);

      /* If len in header is bigger than given length, we will be out of memory */
      if ( dataLen > TMAP_GATT_DATABASE_SIZE )
      {
        BLE_DBG_TMAP_MSG("Error length in header is bigger than TMAP Controller Database length\n");
        return BLE_STATUS_FAILED;
      }

      /*get TMAP Database*/
      (void)BLE_AUDIO_PLAT_NvmGet( BLEAUDIO_PLAT_NVM_CURRENT, BLEAUDIO_PLAT_NVM_TYPE_TMAP,BLENVM_TMAP_HDR_LEN, temp_database, dataLen );
      BLE_DBG_TMAP_MSG("BLE_AUDIO_PLAT_NvmGet of TMAP Controller Database : len %d \n",dataLen);

      /*Restore the TMAP */
      status = TMAP_CLT_SetDatabase((TMAP_CltInst_t *)pTMAP_CltInst,&temp_database[0],dataLen);
      BLE_DBG_TMAP_MSG("Restore TMAP Database returns status 0x%02X\n",status);

      if (status == BLE_STATUS_SUCCESS)
      {
        TMAP_Notification_Evt_t evt;
        evt.Status = BLE_STATUS_SUCCESS;
        pTMAP_CltInst->LinkupState = TMAP_LINKUP_COMPLETE;
        evt.EvtOpcode = TMAP_LINKUP_COMPLETE_EVT;
        evt.ConnHandle = pTMAP_CltInst->ConnHandle;
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
  tBleStatus status = BLE_STATUS_SUCCESS;
  uint8_t i;
  uint32_t dataLen = 0u;

  /* Check if the address is a resolvable private address */
  if (((PeerIdentityAddressType) == 1u) && ((PeerIdentityAddress[5] & 0xC0U) == 0x40U))
  {
    status = aci_gap_resolve_private_addr( &PeerIdentityAddress[0], &cl_addr[0]);
    if (status != BLE_STATUS_SUCCESS)
    {
      BLE_DBG_TMAP_MSG("Remove TMAP Services Record fails because BLE Address has not been successfully resolved\n");
      return status;
    }
  }
  else
  {
    for (i = 0; i < 6u ; i++)
    {
      cl_addr[i] = PeerIdentityAddress[i];
    }
  }

  /*Find in database the record associated to the remote device*/
  if (TMAP_FindDatabaseRecord(cl_addr,&dataLen) == BLEAUDIO_PLAT_NVM_OK)
  {
    BLE_DBG_TMAP_MSG("Peer Device has TMAP information (%d bytes) in NVM, remove it\n",dataLen);
    /* Invalidate current record because data has changed */
    BLE_AUDIO_PLAT_NvmDiscard( BLEAUDIO_PLAT_NVM_CURRENT );
  }
  else
  {
    BLE_DBG_TMAP_MSG("Peer Device has not TMAP information in NVM\n");
    status = BLE_STATUS_DEV_NOT_BONDED;
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
  tBleStatus    status = BLE_STATUS_SUCCESS;
  uint8_t       i;
  uint32_t      dataLen = 0u;

  /* Check if the address is a resolvable private address */
  if (((Peer_Address_Type) == 1u) && ((Peer_Address[5] & 0xC0U) == 0x40U))
  {
    status = aci_gap_resolve_private_addr( &Peer_Address[0], &cl_addr[0]);
  if (status != BLE_STATUS_SUCCESS)
  {
    BLE_DBG_TMAP_MSG("Check TMAP Services Record fails because BLE Address has not been successfully resolved\n");
    return status;
  }
  }
  else
  {
    for (i = 0; i < 6u ; i++)
    {
      cl_addr[i] = Peer_Address[i];
    }
  }
  /*Find in database the record associated to the remote device*/
  if (TMAP_FindDatabaseRecord(cl_addr,&dataLen) == BLEAUDIO_PLAT_NVM_OK)
  {
    BLE_DBG_TMAP_MSG("Peer Device has TMAP information (%d bytes) in NVM\n",dataLen);
    return 1u;
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

static int TMAP_FindDatabaseRecord(const uint8_t* devAddress,uint32_t* dataLen )
{
  /* Search a GATT record in the SDB
   */
  uint32_t hdr[BLENVM_TMAP_HDR_LEN / 4];
  int res;
  uint8_t mode;
  uint8_t i;
  uint8_t is_diff;

  /* Set NVM get mode as "first" */
  mode = BLEAUDIO_PLAT_NVM_FIRST;

  while ( 1 )
  {
    /* Get the header part of each record in order to find if there is already in NVM some data corresponding
     * to the same remote device address
     */
    res = BLE_AUDIO_PLAT_NvmGet( mode, BLEAUDIO_PLAT_NVM_TYPE_TMAP,0, (uint8_t*)hdr, BLENVM_TMAP_HDR_LEN );

    /* Set NVM get mode as "next" */
    mode = BLEAUDIO_PLAT_NVM_NEXT;

    if ( res == BLEAUDIO_PLAT_NVM_EOF )
    {
      /* No more record found */
      return res;
    }

    /* We found a valid record */

    /* Compare addresses */
    is_diff = 0u;
    for (i = 0u ; i < 6u ; i++)
    {
      if (((uint8_t*)hdr)[1+i] != devAddress[i])
      {
        is_diff = 1u;
      }
    }
    if ( is_diff == 0u)
    {
      /* Extract data length of the record */
      *dataLen = hdr[2];

      return BLEAUDIO_PLAT_NVM_OK;
    }
  }
}