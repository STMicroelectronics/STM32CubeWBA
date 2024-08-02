/**
  ******************************************************************************
  * @file    hap_db.c
  * @author  MCD Application Team
  * @brief   This file contains interfaces of HAP Database
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
#include "hap.h"
#if ((BLE_CFG_HAP_HA_ROLE == 1u) || BLE_CFG_HAP_HARC_ROLE == 1u || BLE_CFG_HAP_IAC_ROLE == 1u)
#include "hap_ha.h"
#include "hap_harc.h"
#include "hap_log.h"
#include "hap_db.h"
#include "has.h"
#include "usecase_dev_mgmt.h"
#include "ble_gap_aci.h"
#include "hap_alloc.h"
#include "hap_iac.h"
/* Private typedef -----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/

#define HAP_HA_GATT_DATABASE_SIZE          (2u + HAP_MAX_PRESET_NUM * (3 + HAP_MAX_PRESET_NAME_LEN))
#define HAP_HARC_GATT_DATABASE_SIZE        (35)
#define HAP_IAC_GATT_DATABASE_SIZE         (14)

/* Database record header length in  bytes :
 * validity (1byte) + 6 bytes for BDAddr + 1 unused + 4 bytes length of data
 * in order to be aligned we define a Header length of 12 bytes and declare it
 * as an array of unsigned U32 of 12/4 size.
 * [0] - Valididy + 3 BD addr
 * [1] - 3 BD addr + 1 unused
 * [2] - 4 Length byte.
 */
#define BLENVM_HAP_HDR_LEN              (12u)

#define BLEAUDIO_PLAT_NVM_TYPE_HAP_HA   (0x0B)
#define BLEAUDIO_PLAT_NVM_TYPE_HAP_HARC (0x0C)
#define BLEAUDIO_PLAT_NVM_TYPE_HAP_IAC  (0x0D)
/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions prototype------------------------------------------------*/
void HAP_DB_StoreDatabase(uint8_t NvmType, uint8_t ClAddress[6], uint8_t *pDatabase, uint16_t DatabaseLen);
static int HAP_FindDatabaseRecord(uint8_t NvmType, const uint8_t* devAddress,uint32_t* dataLen );
tBleStatus HAP_DB_GetRemoteAddress(uint8_t Peer_Address_Type, const uint8_t Peer_Address[6u], uint8_t *Address);
/* External functions prototype------------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/

/**
  * @brief Store every HAP services related to a given connection for GATT servers and clients
  * @param ConnHandle: Handle of the connection
  * @param Peer_Address_Type: Peer Address type
  * @param Peer_Address: Peer Address
  */
void HAP_DB_StoreServices(uint16_t ConnHandle, uint8_t Peer_Address_Type, const uint8_t Peer_Address[6])
{
  tBleStatus    status;
  uint8_t       cl_addr[6u] = {0u};

  status = HAP_DB_GetRemoteAddress(Peer_Address_Type, &Peer_Address[0], &cl_addr[0u]);
  if (status != BLE_STATUS_SUCCESS)
  {
    /* Error during Get Remote Address */
    BLE_DBG_HAP_DB_MSG("Store Services fails because BLE Address has not been successfully resolved\n");
    return;
  }

#if (BLE_CFG_HAP_HA_ROLE == 1u)
  if (HAP_Context.Role & HAP_ROLE_HEARING_AID)
  {
    uint8_t  hap_database[HAP_HA_GATT_DATABASE_SIZE];
    uint16_t len = 0;
    /* Store HAP Server */
    /* Get ATT Database of the HAP */
    status = HAP_HA_StoreDatabase(ConnHandle,hap_database, HAP_HA_GATT_DATABASE_SIZE, &len);
    if ((status == BLE_STATUS_SUCCESS) && (len > 0u))
    {
      BLE_DBG_HAP_DB_MSG("%d bytes of HAP HA Information database to store in NVM\n", len);
      HAP_DB_StoreDatabase(BLEAUDIO_PLAT_NVM_TYPE_HAP_HA, cl_addr, hap_database, len);
    }
    else
    {
      BLE_DBG_HAP_DB_MSG("Get ATT Database information of HAP Service has failed (status 0x%02X)\n", status);
    }
  }
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */

#if (BLE_CFG_HAP_HARC_ROLE == 1u)
  if (HAP_Context.Role & HAP_ROLE_HEARING_AID_REMOTE_CONTROLLER)
  {
    uint8_t  hap_database[HAP_HARC_GATT_DATABASE_SIZE];
    uint16_t len = 0;
    /* Store HAP Server */
    /* Get ATT Database of the HAP */
    status = HAP_HARC_StoreDatabase(ConnHandle,hap_database, HAP_HARC_GATT_DATABASE_SIZE, &len);
    if ((status == BLE_STATUS_SUCCESS) && (len > 0u))
    {
      BLE_DBG_HAP_DB_MSG("%d bytes of HAP HARC Information database to store in NVM\n", len);
      HAP_DB_StoreDatabase(BLEAUDIO_PLAT_NVM_TYPE_HAP_HARC, cl_addr, hap_database, len);
    }
    else
    {
      BLE_DBG_HAP_DB_MSG("Get ATT Database information of HAP HARC has failed (status 0x%02X)\n", status);
    }
  }
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */

#if (BLE_CFG_HAP_IAC_ROLE == 1u)
  if (HAP_Context.Role & HAP_ROLE_IMMEDIATE_ALERT_CLIENT)
  {
    uint8_t  hap_database[HAP_IAC_GATT_DATABASE_SIZE];
    uint16_t len = 0;
    /* Store HAP Server */
    /* Get ATT Database of the HAP */
    status = HAP_IAC_StoreDatabase(ConnHandle, hap_database, HAP_IAC_GATT_DATABASE_SIZE, &len);
    if ((status == BLE_STATUS_SUCCESS) && (len > 0u))
    {
      BLE_DBG_HAP_DB_MSG("%d bytes of HAP IAC Information database to store in NVM\n", len);
      HAP_DB_StoreDatabase(BLEAUDIO_PLAT_NVM_TYPE_HAP_IAC, cl_addr, hap_database, len);
    }
    else
    {
      BLE_DBG_HAP_DB_MSG("Get ATT Database information of HAP IAC has failed (status 0x%02X)\n", status);
    }
  }
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */
}

#if (BLE_CFG_HAP_HA_ROLE == 1u)
/**
  * @brief On a GATT Server, check changes made to Hearing aids presets since the last connection of the given
  *        remote device
  * @param pBleConnInfo: A pointer to the connection info structure
  * @retval status of the operation
  */
tBleStatus HAP_DB_CheckDatabaseChange(const UseCaseConnInfo_t *pBleConnInfo)
{
  uint8_t       cl_addr[6u] = {0u};
  tBleStatus    status = BLE_STATUS_SUCCESS;
  uint32_t dataLen = 0u;
  HAP_HA_Restore_Context_t *p_restore_context;

  status = HAP_DB_GetRemoteAddress(pBleConnInfo->Peer_Address_Type, &pBleConnInfo->Peer_Address[0], &cl_addr[0u]);
  if (status != BLE_STATUS_SUCCESS)
  {
    /* Error during Get Remote Address */
    BLE_DBG_HAP_DB_MSG("Check HAP Services Record fails because BLE Address has not been successfully resolved\n");
    return status;
  }

  status = HAP_HA_GetRestoreContext(pBleConnInfo->Connection_Handle, &p_restore_context);
  if (status != BLE_STATUS_SUCCESS)
  {
    /* Error during Get Restore Context */
    BLE_DBG_HAP_DB_MSG("Could not find related HAP HA Restore Context\n");
    return status;
  }

  /* Find in database the record associated to the remote device */
  if (HAP_FindDatabaseRecord(BLEAUDIO_PLAT_NVM_TYPE_HAP_HA, cl_addr, &dataLen) == BLEAUDIO_PLAT_NVM_OK)
  {
    if (dataLen > 0)
    {
      uint8_t  temp_database[HAP_HA_GATT_DATABASE_SIZE];
      BLE_DBG_HAP_DB_MSG("Peer Device has HAP information (%d bytes) in NVM\n",dataLen);

      /* If len in header is bigger than given length, we will be out of memory */
      if ( dataLen > HAP_HA_GATT_DATABASE_SIZE )
      {
        BLE_DBG_HAP_DB_MSG("Error length in header is bigger than HAP Database length\n");
        p_restore_context->State = HAP_HA_RESTORE_STATE_IDLE;
        return BLE_STATUS_FAILED;
      }

      /*get HAP Database*/
      (void)BLE_AUDIO_PLAT_NvmGet( BLEAUDIO_PLAT_NVM_CURRENT, BLEAUDIO_PLAT_NVM_TYPE_HAP_HA, BLENVM_HAP_HDR_LEN, temp_database, dataLen );
      BLE_DBG_HAP_DB_MSG("BLE_AUDIO_PLAT_NvmGet of HAP Database : len %d \n",dataLen);

      /*Check if the values of HAP have changed when bonded device was not connected*/
      status = HAP_HA_CheckDatabaseChange(pBleConnInfo->Connection_Handle, &temp_database[0], dataLen);
      BLE_DBG_HAP_DB_MSG("Check HAP Database returns status 0x%02X\n",status);
    }
  }
  else
  {
    BLE_DBG_HAP_DB_MSG("No HAP Database found\n");
    /* No more services to restore */
    p_restore_context->State = HAP_HA_RESTORE_STATE_IDLE;
    p_restore_context->ConnHandle = 0xFFFF;
    p_restore_context->ParseIndex = 0;
    p_restore_context->pPreset = 0;
    p_restore_context->PrevIndex = 0;
  }
  return status;
}
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */

/**
  * @brief Indicate if HAP HA Database is saved in NVM
  * @param Peer_Address_Type: Peer Address type
  * @param Peer_Address: Peer Address
  * @retval 0 if not present in the database, else 1
  */
uint8_t HAP_HA_DB_IsPresent(uint8_t Peer_Address_Type,const uint8_t Peer_Address[6])
{
#if (BLE_CFG_HAP_HA_ROLE == 1u)
  tBleStatus    status = BLE_STATUS_SUCCESS;
  uint8_t       cl_addr[6u] = {0u};
  uint32_t      dataLen = 0u;

  status = HAP_DB_GetRemoteAddress(Peer_Address_Type, &Peer_Address[0], &cl_addr[0u]);
  if (status != BLE_STATUS_SUCCESS)
  {
    /* Error during Get Remote Address */
    BLE_DBG_HAP_DB_MSG("Check HAP HA Services Record fails because BLE Address has not been successfully resolved\n");
    return status;
  }

  /*Find in database the record associated to the remote device*/
  if (HAP_FindDatabaseRecord(BLEAUDIO_PLAT_NVM_TYPE_HAP_HA, cl_addr,&dataLen) == BLEAUDIO_PLAT_NVM_OK)
  {
    BLE_DBG_HAP_DB_MSG("Peer Device has HAP HA information (%d bytes) in NVM\n",dataLen);
    return 1u;
  }
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */
  return 0u;
}

/**
  * @brief  Remove the record of the HAP HA stored in the Non Volatile memory.
  * @param  PeerIdentityAddressType: Identity address type.
  *                                  Values:
  *                                     - 0x00: Public Identity Address
  *                                     - 0x01: Random (static) Identity Address
  * @param  PeerIdentityAddress : Public or Random (static) Identity address of the peer device
  * @retval status of the operation
  */
tBleStatus HAP_HA_DB_RemoveServicesRecord(uint8_t PeerIdentityAddressType,const uint8_t PeerIdentityAddress[6])
{
  tBleStatus status = HCI_COMMAND_DISALLOWED_ERR_CODE;
#if (BLE_CFG_HAP_HA_ROLE == 1u)
  uint8_t cl_addr[6u] = {0};
  uint32_t dataLen = 0u;

  status = HAP_DB_GetRemoteAddress(PeerIdentityAddressType, &PeerIdentityAddress[0], &cl_addr[0u]);
  if (status != BLE_STATUS_SUCCESS)
  {
    /* Error during Get Remote Address */
    BLE_DBG_HAP_DB_MSG("Remove HAP HA Services Record fails because BLE Address has not been successfully resolved\n");
    return status;
  }

  /*Find in database the record associated to the remote device*/
  if (HAP_FindDatabaseRecord(BLEAUDIO_PLAT_NVM_TYPE_HAP_HA, cl_addr,&dataLen) == BLEAUDIO_PLAT_NVM_OK)
  {
    BLE_DBG_HAP_DB_MSG("Peer Device has HAP HA information (%d bytes) in NVM, remove it\n",dataLen);
    /* Invalidate current record because data has changed */
    BLE_AUDIO_PLAT_NvmDiscard( BLEAUDIO_PLAT_NVM_CURRENT );
  }
  else
  {
    BLE_DBG_HAP_DB_MSG("Peer Device has not HAP HA information in NVM\n");
    status = BLE_STATUS_DEV_NOT_BONDED;
  }
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */
  return status;
}

#if (BLE_CFG_HAP_HARC_ROLE == 1u)
/**
   * @brief  Restore GATT Database of the HAP in HARC Role
   * @param  pHAP_Client: pointer on HAP Client Instance
   * @retval status of the operation
   */
tBleStatus HAP_HARC_DB_RestoreClientDatabase(HAP_HARC_Inst_t *pHARC_Inst)
{
  uint8_t                 cl_addr[6u] = {0};
  tBleStatus              status = BLE_STATUS_SUCCESS;
  uint8_t                 temp_database[HAP_HARC_GATT_DATABASE_SIZE];
  uint32_t                dataLen = 0u;
  const UseCaseConnInfo_t *p_conn_info;

  BLE_DBG_HAP_DB_MSG("Start Restoration of HAP Profile\n",dataLen);

  if (USECASE_DEV_MGMT_GetConnInfo(pHARC_Inst->pConnInfo->Connection_Handle,&p_conn_info) == BLE_STATUS_SUCCESS)
  {

    status = HAP_DB_GetRemoteAddress(p_conn_info->Peer_Address_Type, &p_conn_info->Peer_Address[0], &cl_addr[0u]);
    if (status != BLE_STATUS_SUCCESS)
    {
      /* Error during Get Remote Address */
      BLE_DBG_HAP_DB_MSG("Restore HAP HARC is aborted because BLE Address has not been successfully resolved\n");
      return status;
    }

    /*Find in database the record associated to the remote device*/
    if (HAP_FindDatabaseRecord(BLEAUDIO_PLAT_NVM_TYPE_HAP_HARC, cl_addr,&dataLen) == BLEAUDIO_PLAT_NVM_OK)
    {
      BLE_DBG_HAP_DB_MSG("Peer Device has HAP HARC information (%d bytes) in NVM\n",dataLen);

      /* If len in header is bigger than given length, we will be out of memory */
      if ( dataLen > HAP_HARC_GATT_DATABASE_SIZE )
      {
        BLE_DBG_HAP_DB_MSG("Error length in header is bigger than HAP Controller Database length\n");
        return BLE_STATUS_FAILED;
      }

      /*get HAP Database*/
      (void)BLE_AUDIO_PLAT_NvmGet( BLEAUDIO_PLAT_NVM_CURRENT, BLEAUDIO_PLAT_NVM_TYPE_HAP_HARC,BLENVM_HAP_HDR_LEN, temp_database, dataLen );
      BLE_DBG_HAP_DB_MSG("BLE_AUDIO_PLAT_NvmGet of HAP Controller Database : len %d \n",dataLen);

      /*Restore the HAP */
      status = HAP_HARC_RestoreDatabase(pHARC_Inst, &temp_database[0], dataLen);
      BLE_DBG_HAP_DB_MSG("Restore HAP Database returns status 0x%02X\n",status);

      if (status == BLE_STATUS_SUCCESS)
      {
        HAP_HARC_NotificationEvt_t evt;
        evt.Status = BLE_STATUS_SUCCESS;
        pHARC_Inst->LinkupState = HAP_HARC_LINKUP_COMPLETE;
        evt.EvtOpcode = HARC_LINKUP_COMPLETE_EVT;
        evt.ConnHandle = pHARC_Inst->pConnInfo->Connection_Handle;
        HAP_HARC_Notification(&evt);
      }
      else
      {
        status = BLE_STATUS_FAILED;
      }
    }
    else
    {
      BLE_DBG_HAP_DB_MSG("Peer Device has no HAP HARC information in NVM\n");
      status = BLE_STATUS_FAILED;
    }
  }
  else
  {
    status = BLE_STATUS_FAILED;
  }
  return status;
}
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */

/**
  * @brief Indicate if HAP HARC Database is saved in NVM
  * @param Peer_Address_Type: Peer Address type
  * @param Peer_Address: Peer Address
  * @retval 0 if not present in the database, else 1
  */
uint8_t HAP_HARC_DB_IsPresent(uint8_t Peer_Address_Type, const uint8_t Peer_Address[6])
{
#if (BLE_CFG_HAP_HARC_ROLE == 1u)
  uint8_t       cl_addr[6u] = {0u};
  tBleStatus    status = BLE_STATUS_SUCCESS;
  uint32_t      dataLen = 0u;

  status = HAP_DB_GetRemoteAddress(Peer_Address_Type, &Peer_Address[0], &cl_addr[0u]);
  if (status != BLE_STATUS_SUCCESS)
  {
    /* Error during Get Remote Address */
    BLE_DBG_HAP_DB_MSG("Check HAP HARC Services Record fails because BLE Address has not been successfully resolved\n");
    return status;
  }

  /*Find in database the record associated to the remote device*/
  if (HAP_FindDatabaseRecord(BLEAUDIO_PLAT_NVM_TYPE_HAP_HARC, cl_addr,&dataLen) == BLEAUDIO_PLAT_NVM_OK)
  {
    BLE_DBG_HAP_DB_MSG("Peer Device has HAP HARC information (%d bytes) in NVM\n",dataLen);
    return 1u;
  }
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */
  return 0u;
}

/**
  * @brief  Remove the record of the HAP HARC stored in the Non Volatile memory.
  * @param  PeerIdentityAddressType: Identity address type.
  *                                  Values:
  *                                     - 0x00: Public Identity Address
  *                                     - 0x01: Random (static) Identity Address
  * @param  PeerIdentityAddress : Public or Random (static) Identity address of the peer device
  * @retval status of the operation
  */
tBleStatus HAP_HARC_DB_RemoveServicesRecord(uint8_t PeerIdentityAddressType,const uint8_t PeerIdentityAddress[6])
{
  tBleStatus status = HCI_COMMAND_DISALLOWED_ERR_CODE;
#if (BLE_CFG_HAP_HARC_ROLE == 1u)
  uint8_t cl_addr[6u] = {0};
  uint32_t dataLen = 0u;

  status = HAP_DB_GetRemoteAddress(PeerIdentityAddressType, &PeerIdentityAddress[0], &cl_addr[0u]);
  if (status != BLE_STATUS_SUCCESS)
  {
    /* Error during Get Remote Address */
    BLE_DBG_HAP_DB_MSG("Remove HAP HARC Services Record fails because BLE Address has not been successfully resolved\n");
    return status;
  }

  /*Find in database the record associated to the remote device*/
  if (HAP_FindDatabaseRecord(BLEAUDIO_PLAT_NVM_TYPE_HAP_HARC, cl_addr,&dataLen) == BLEAUDIO_PLAT_NVM_OK)
  {
    BLE_DBG_HAP_DB_MSG("Peer Device has HAP HARC information (%d bytes) in NVM, remove it\n",dataLen);
    /* Invalidate current record because data has changed */
    BLE_AUDIO_PLAT_NvmDiscard( BLEAUDIO_PLAT_NVM_CURRENT );
  }
  else
  {
    BLE_DBG_HAP_DB_MSG("Peer Device has not HAP HARC information in NVM\n");
    status = BLE_STATUS_DEV_NOT_BONDED;
  }
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */
  return status;
}

#if (BLE_CFG_HAP_IAC_ROLE == 1u)
/**
   * @brief  Restore GATT Database of the HAP in IAC Role
   * @param  pHAP_Client: pointer on HAP Client Instance
   * @retval status of the operation
   */
tBleStatus HAP_IAC_DB_RestoreClientDatabase(HAP_IAC_Inst_t *pIAC_Inst)
{
  uint8_t                 cl_addr[6u] = {0};
  tBleStatus              status = BLE_STATUS_SUCCESS;
  uint8_t                 temp_database[HAP_IAC_GATT_DATABASE_SIZE];
  uint32_t                dataLen = 0u;
  const UseCaseConnInfo_t *p_conn_info;

  BLE_DBG_HAP_DB_MSG("Start Restoration of HAP IAC\n",dataLen);

  if (USECASE_DEV_MGMT_GetConnInfo(pIAC_Inst->ConnHandle,&p_conn_info) == BLE_STATUS_SUCCESS)
  {

    status = HAP_DB_GetRemoteAddress(p_conn_info->Peer_Address_Type, &p_conn_info->Peer_Address[0], &cl_addr[0u]);
    if (status != BLE_STATUS_SUCCESS)
    {
      /* Error during Get Remote Address */
      BLE_DBG_HAP_DB_MSG("Restore HAP IAC is aborted because BLE Address has not been successfully resolved\n");
      return status;
    }

    /*Find in database the record associated to the remote device*/
    if (HAP_FindDatabaseRecord(BLEAUDIO_PLAT_NVM_TYPE_HAP_IAC, cl_addr,&dataLen) == BLEAUDIO_PLAT_NVM_OK)
    {
      BLE_DBG_HAP_DB_MSG("Peer Device has HAP IAC information (%d bytes) in NVM\n",dataLen);

      /* If len in header is bigger than given length, we will be out of memory */
      if ( dataLen > HAP_IAC_GATT_DATABASE_SIZE )
      {
        BLE_DBG_HAP_DB_MSG("Error length in header is bigger than HAP Controller Database length\n");
        return BLE_STATUS_FAILED;
      }

      /*get HAP Database*/
      (void)BLE_AUDIO_PLAT_NvmGet( BLEAUDIO_PLAT_NVM_CURRENT, BLEAUDIO_PLAT_NVM_TYPE_HAP_IAC, BLENVM_HAP_HDR_LEN,
                                  temp_database, dataLen );
      BLE_DBG_HAP_DB_MSG("BLE_AUDIO_PLAT_NvmGet of HAP Controller Database : len %d \n",dataLen);

      /*Restore the HAP */
      status = HAP_IAC_RestoreDatabase(pIAC_Inst, &temp_database[0], dataLen);
      BLE_DBG_HAP_DB_MSG("Restore HAP IAC Database returns status 0x%02X\n",status);

      if (status == BLE_STATUS_SUCCESS)
      {
        HAP_IAC_NotificationEvt_t evt;
        evt.Status = BLE_STATUS_SUCCESS;
        pIAC_Inst->LinkupState = HAP_IAC_LINKUP_COMPLETE;
        evt.EvtOpcode = IAC_LINKUP_COMPLETE_EVT;
        evt.ConnHandle = pIAC_Inst->ConnHandle;
        HAP_IAC_Notification(&evt);
      }
      else
      {
        status = BLE_STATUS_FAILED;
      }
    }
    else
    {
      BLE_DBG_HAP_DB_MSG("Peer Device has no HAP IAC information in NVM\n");
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
  * @brief Indicate if HAP IAC Database is saved in NVM
  * @param Peer_Address_Type: Peer Address type
  * @param Peer_Address: Peer Address
  * @retval 0 if not present in the database, else 1
  */
uint8_t HAP_IAC_DB_IsPresent(uint8_t Peer_Address_Type, const uint8_t Peer_Address[6])
{
  uint8_t       cl_addr[6u] = {0u};
  tBleStatus    status = BLE_STATUS_SUCCESS;
  uint32_t      dataLen = 0u;

  status = HAP_DB_GetRemoteAddress(Peer_Address_Type, &Peer_Address[0], &cl_addr[0u]);
  if (status != BLE_STATUS_SUCCESS)
  {
    /* Error during Get Remote Address */
    BLE_DBG_HAP_DB_MSG("Check HAP IAC Services Record fails because BLE Address has not been successfully resolved\n");
    return status;
  }

  /*Find in database the record associated to the remote device*/
  if (HAP_FindDatabaseRecord(BLEAUDIO_PLAT_NVM_TYPE_HAP_IAC, cl_addr,&dataLen) == BLEAUDIO_PLAT_NVM_OK)
  {
    BLE_DBG_HAP_DB_MSG("Peer Device has HAP IAC information (%d bytes) in NVM\n",dataLen);
    return 1u;
  }
  return 0u;
}

/**
  * @brief  Remove the record of the HAP IAC stored in the Non Volatile memory.
  * @param  PeerIdentityAddressType: Identity address type.
  *                                  Values:
  *                                     - 0x00: Public Identity Address
  *                                     - 0x01: Random (static) Identity Address
  * @param  PeerIdentityAddress : Public or Random (static) Identity address of the peer device
  * @retval status of the operation
  */
tBleStatus HAP_IAC_DB_RemoveServicesRecord(uint8_t PeerIdentityAddressType, const uint8_t PeerIdentityAddress[6])
{
  uint8_t cl_addr[6u] = {0};
  tBleStatus status = BLE_STATUS_SUCCESS;
  uint32_t dataLen = 0u;

  status = HAP_DB_GetRemoteAddress(PeerIdentityAddressType, &PeerIdentityAddress[0], &cl_addr[0u]);
  if (status != BLE_STATUS_SUCCESS)
  {
    /* Error during Get Remote Address */
    BLE_DBG_HAP_DB_MSG("Remove HAP IAC Services Record fails because BLE Address has not been successfully resolved\n");
    return status;
  }

  /*Find in database the record associated to the remote device*/
  if (HAP_FindDatabaseRecord(BLEAUDIO_PLAT_NVM_TYPE_HAP_IAC, cl_addr,&dataLen) == BLEAUDIO_PLAT_NVM_OK)
  {
    BLE_DBG_HAP_DB_MSG("Peer Device has HAP IAC information (%d bytes) in NVM, remove it\n",dataLen);
    /* Invalidate current record because data has changed */
    BLE_AUDIO_PLAT_NvmDiscard( BLEAUDIO_PLAT_NVM_CURRENT );
  }
  else
  {
    BLE_DBG_HAP_DB_MSG("Peer Device has not HAP IAC information in NVM\n");
    status = BLE_STATUS_DEV_NOT_BONDED;
  }

  return status;
}
#endif /* (BLE_CFG_HAP_IAC_ROLE == 1u) */

/* Private functions ----------------------------------------------------------*/

/**
  * @brief Store a database related to a service in NVM
  * @param ClAddress: Address of the peer device
  * @param pDatabase: A pointer to the database to store
  * @param DatabaseLen: Length of the database to store
  */
void HAP_DB_StoreDatabase(uint8_t NvmType, uint8_t ClAddress[6], uint8_t *pDatabase, uint16_t DatabaseLen)
{
  uint8_t res;
  uint32_t data_len = 0u;
  uint32_t hdr[BLENVM_HAP_HDR_LEN / 4];

  /*Find in database the record associated to the remote device*/
  res = HAP_FindDatabaseRecord(NvmType, ClAddress, &data_len);
  if (res == BLEAUDIO_PLAT_NVM_OK)
  {

    BLE_DBG_HAP_DB_MSG("Compare already stored HAP database of length %d bytes vs current HAP Database of length %d\n", data_len, DatabaseLen);
    /* Compare data from byte 12 */
    if ( (DatabaseLen == data_len) &&
         (BLE_AUDIO_PLAT_NvmCompare( BLENVM_HAP_HDR_LEN, pDatabase, data_len)
          == BLEAUDIO_PLAT_NVM_OK) )
    {

      BLE_DBG_HAP_DB_MSG("HAP database hasn't changed, no need to perform NVM Save operation\n");
      /* Return directly as we have found the same data already in NVM */
      return ;
    }

    BLE_DBG_HAP_DB_MSG("HAP database has changed\n");
    /* Invalidate current record because data has changed */
    BLE_AUDIO_PLAT_NvmDiscard( BLEAUDIO_PLAT_NVM_CURRENT );
  }


  ((uint8_t*)(hdr + 0))[0] = 0xFDU;
  ((uint8_t*)(hdr + 0))[1] = ClAddress[0];
  ((uint8_t*)(hdr + 0))[2] = ClAddress[1];
  ((uint8_t*)(hdr + 0))[3] = ClAddress[2];
  ((uint8_t*)(hdr + 1))[0] = ClAddress[3];
  ((uint8_t*)(hdr + 1))[1] = ClAddress[4];
  ((uint8_t*)(hdr + 1))[2] = ClAddress[5];
  ((uint8_t*)(hdr + 1))[3] = 0xFFU;

  /* the last byte of the flash word containing the address will have 0xFF.
   * The length of the database will be at the next word offset in flash.
   */
  hdr[2] = DatabaseLen;
  BLE_DBG_HAP_DB_MSG("Store %d bytes of HAP Information database in NVM for peer device 0x%02x%02x%02x%02x%02x%02x\n",
                     DatabaseLen,
                     ClAddress[5],
                     ClAddress[4],
                     ClAddress[3],
                     ClAddress[2],
                     ClAddress[1],
                     ClAddress[0]);
  res = BLE_AUDIO_PLAT_NvmAdd(NvmType,(uint8_t*)hdr, BLENVM_HAP_HDR_LEN, pDatabase, DatabaseLen );
  BLE_DBG_HAP_DB_MSG("BLE_AUDIO_PLAT_NvmAdd() returns status 0x%02X\n",res);
}

static int HAP_FindDatabaseRecord(uint8_t NvmType, const uint8_t* devAddress,uint32_t* dataLen )
{
  /* Search a GATT record in the SDB
   */
  uint32_t hdr[BLENVM_HAP_HDR_LEN / 4];
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
    res = BLE_AUDIO_PLAT_NvmGet( mode, NvmType,0, (uint8_t*)hdr, BLENVM_HAP_HDR_LEN );

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

/**
  * @brief If an address is a resolvable private address, resolve it. Otherwise just copy the value of the address
  * @param Peer_Address_Type: Type of the address
  * @param Peer_Address: Peer Address
  * @param Address: Output address
  * @retval status of the operation
  */
tBleStatus HAP_DB_GetRemoteAddress(uint8_t Peer_Address_Type, const uint8_t Peer_Address[6u], uint8_t *Address)
{
  uint8_t status = BLE_STATUS_SUCCESS;

  /* Check if the address is a resolvable private address */
  if (((Peer_Address_Type) == 1u) && ((Peer_Address[5] & 0xC0U) == 0x40U))
  {
    status = aci_gap_resolve_private_addr( &Peer_Address[0], Address);
  }
  else
  {
    memcpy(Address, &Peer_Address[0], 6u);
  }
  return status;
}
#endif /* ((BLE_CFG_HAP_HA_ROLE == 1u) || BLE_CFG_HAP_HARC_ROLE == 1u || BLE_CFG_HAP_IAC_ROLE == 1u) */