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

/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions prototype------------------------------------------------*/
tBleStatus HAP_DB_GetRemoteAddress(uint8_t Peer_Address_Type,
                                   const uint8_t Peer_Address[6u],
                                   uint8_t *type,
                                   uint8_t *Address);
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
  uint8_t       type;

  status = HAP_DB_GetRemoteAddress(Peer_Address_Type, &Peer_Address[0], &type, &cl_addr[0u]);
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
      status = BLE_AUDIO_STACK_DB_AddRecord(BLEAUDIO_PLAT_NVM_TYPE_HAP_HA,
                                           type,
                                           cl_addr,
                                           hap_database,
                                           len);
      BLE_DBG_HAP_DB_MSG("BLE_AUDIO_STACK_DB_AddRecord() returns status 0x%02X\n",status);
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
      status = BLE_AUDIO_STACK_DB_AddRecord(BLEAUDIO_PLAT_NVM_TYPE_HAP_HARC,
                                           type,
                                           cl_addr,
                                           hap_database,
                                           len);
      BLE_DBG_HAP_DB_MSG("BLE_AUDIO_STACK_DB_AddRecord() returns status 0x%02X\n",status);
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
      status = BLE_AUDIO_STACK_DB_AddRecord(BLEAUDIO_PLAT_NVM_TYPE_HAP_IAC,
                                           type,
                                           cl_addr,
                                           hap_database,
                                           len);
      BLE_DBG_HAP_DB_MSG("BLE_AUDIO_STACK_DB_AddRecord() returns status 0x%02X\n",status);
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
  uint8_t                       cl_addr[6u] = {0u};
  uint8_t                       type;
  tBleStatus                    status = BLE_STATUS_SUCCESS;
  uint16_t                      dataLen = 0u;
  HAP_HA_Restore_Context_t      *p_restore_context;
  uint8_t                       temp_database[HAP_HA_GATT_DATABASE_SIZE];

  status = HAP_DB_GetRemoteAddress(pBleConnInfo->Peer_Address_Type, &pBleConnInfo->Peer_Address[0], &type,&cl_addr[0u]);
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
  if (BLE_AUDIO_STACK_DB_GetRecord(BLEAUDIO_PLAT_NVM_TYPE_HAP_HA,
                                   type,
                                   cl_addr,
                                   temp_database,
                                   HAP_HA_GATT_DATABASE_SIZE,
                                   &dataLen) == BLE_STATUS_SUCCESS)
  {
    if (dataLen > 0)
    {
      BLE_DBG_HAP_DB_MSG("Peer Device has HAP information (%d bytes) in NVM\n",dataLen);

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
  uint8_t       type;

  status = HAP_DB_GetRemoteAddress(Peer_Address_Type, &Peer_Address[0], &type, &cl_addr[0u]);
  if (status != BLE_STATUS_SUCCESS)
  {
    /* Error during Get Remote Address */
    BLE_DBG_HAP_DB_MSG("Check HAP HA Services Record fails because BLE Address has not been successfully resolved\n");
    return status;
  }

  /*Find in database the record associated to the remote device*/
  if (BLE_AUDIO_STACK_DB_FindRecord(BLEAUDIO_PLAT_NVM_TYPE_HAP_HA, type, cl_addr) == BLE_STATUS_SUCCESS)
  {
    BLE_DBG_HAP_DB_MSG("Peer Device has HAP HA information in NVM\n");
    return 1u;
  }
  else
  {
    BLE_DBG_HAP_DB_MSG("Peer Device has no HAP HA information in NVM\n");
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
#if (BLE_CFG_HAP_HA_ROLE == 1u)
  tBleStatus    status;
  uint8_t       type;
  uint8_t       cl_addr[6u] = {0};

  status = HAP_DB_GetRemoteAddress(PeerIdentityAddressType, &PeerIdentityAddress[0], &type, &cl_addr[0u]);
  if (status != BLE_STATUS_SUCCESS)
  {
    /* Error during Get Remote Address */
    BLE_DBG_HAP_DB_MSG("Remove HAP HA Services Record fails because BLE Address has not been successfully resolved\n");
    return status;
  }

  status = BLE_AUDIO_STACK_DB_RemoveRecord(BLEAUDIO_PLAT_NVM_TYPE_HAP_HA,type,&cl_addr[0]);
  BLE_DBG_HAP_DB_MSG("Remove HAP HA information in NVM returns status 0x%02X\n",status);

  return status;
#else /* (BLE_CFG_HAP_HA_ROLE == 0u) */
  return HCI_COMMAND_DISALLOWED_ERR_CODE
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */
}

#if (BLE_CFG_HAP_HARC_ROLE == 1u)
/**
   * @brief  Restore GATT Database of the HAP in HARC Role
   * @param  pHAP_Client: pointer on HAP Client Instance
   * @retval status of the operation
   */
tBleStatus HAP_HARC_DB_RestoreClientDatabase(HAP_HARC_Inst_t *pHARC_Inst)
{
  uint8_t               cl_addr[6u] = {0};
  uint8_t               type;
  tBleStatus            status = BLE_STATUS_SUCCESS;
  uint8_t               temp_database[HAP_HARC_GATT_DATABASE_SIZE];
  uint16_t              dataLen = 0u;

  BLE_DBG_HAP_DB_MSG("Start Restoration of HAP Profile\n",dataLen);

  status = HAP_DB_GetRemoteAddress(pHARC_Inst->pConnInfo->Peer_Address_Type,
                                   &pHARC_Inst->pConnInfo->Peer_Address[0],
                                   &type,
                                   &cl_addr[0u]);
  if (status != BLE_STATUS_SUCCESS)
  {
    /* Error during Get Remote Address */
    BLE_DBG_HAP_DB_MSG("Restore HAP HARC is aborted because BLE Address has not been successfully resolved\n");
    return status;
  }

  if (BLE_AUDIO_STACK_DB_GetRecord(BLEAUDIO_PLAT_NVM_TYPE_HAP_HARC,
                                   type,
                                   cl_addr,
                                   temp_database,
                                   HAP_HARC_GATT_DATABASE_SIZE,
                                   &dataLen) == BLE_STATUS_SUCCESS)
  {
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
  uint8_t       type;
  tBleStatus    status = BLE_STATUS_SUCCESS;

  status = HAP_DB_GetRemoteAddress(Peer_Address_Type, &Peer_Address[0], &type, &cl_addr[0u]);
  if (status != BLE_STATUS_SUCCESS)
  {
    /* Error during Get Remote Address */
    BLE_DBG_HAP_DB_MSG("Check HAP HARC Services Record fails because BLE Address has not been successfully resolved\n");
    return status;
  }

  /*Find in database the record associated to the remote device*/
  if (BLE_AUDIO_STACK_DB_FindRecord(BLEAUDIO_PLAT_NVM_TYPE_HAP_HARC,type,cl_addr) == BLE_STATUS_SUCCESS)
  {
    BLE_DBG_HAP_DB_MSG("Peer Device has HAP HARC information in NVM\n");
    return 1u;
  }
  else
  {
    BLE_DBG_HAP_DB_MSG("Peer Device has no HAP HARC information in NVM\n");
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
#if (BLE_CFG_HAP_HARC_ROLE == 1u)
  tBleStatus    status;
  uint8_t       type;
  uint8_t       cl_addr[6u] = {0};

  status = HAP_DB_GetRemoteAddress(PeerIdentityAddressType, &PeerIdentityAddress[0], &type, &cl_addr[0u]);
  if (status != BLE_STATUS_SUCCESS)
  {
    /* Error during Get Remote Address */
    BLE_DBG_HAP_DB_MSG("Remove HAP HARC Services Record fails because BLE Address has not been successfully resolved\n");
    return status;
  }

  status = BLE_AUDIO_STACK_DB_RemoveRecord(BLEAUDIO_PLAT_NVM_TYPE_HAP_HARC,type,&cl_addr[0]);
  BLE_DBG_HAP_DB_MSG("Remove HAP HARC information in NVM returns status 0x%02X\n",status);

  return status;
#else /*(BLE_CFG_HAP_HARC_ROLE == 0u)*/
  return HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */
}

#if (BLE_CFG_HAP_IAC_ROLE == 1u)
/**
   * @brief  Restore GATT Database of the HAP in IAC Role
   * @param  pHAP_Client: pointer on HAP Client Instance
   * @retval status of the operation
   */
tBleStatus HAP_IAC_DB_RestoreClientDatabase(HAP_IAC_Inst_t *pIAC_Inst)
{
  uint8_t               cl_addr[6u] = {0};
  uint8_t               type;
  tBleStatus            status = BLE_STATUS_SUCCESS;
  uint8_t               temp_database[HAP_IAC_GATT_DATABASE_SIZE];
  uint16_t              dataLen = 0u;

  BLE_DBG_HAP_DB_MSG("Start Restoration of HAP IAC\n",dataLen);

  status = HAP_DB_GetRemoteAddress(pIAC_Inst->pConnInfo->Peer_Address_Type,
                                   &pIAC_Inst->pConnInfo->Peer_Address[0],
                                   &type,
                                   &cl_addr[0u]);
  if (status != BLE_STATUS_SUCCESS)
  {
    /* Error during Get Remote Address */
    BLE_DBG_HAP_DB_MSG("Restore HAP IAC is aborted because BLE Address has not been successfully resolved\n");
    return status;
  }

  if (BLE_AUDIO_STACK_DB_GetRecord(BLEAUDIO_PLAT_NVM_TYPE_HAP_IAC,
                                   type,
                                   cl_addr,
                                   temp_database,
                                   HAP_HARC_GATT_DATABASE_SIZE,
                                   &dataLen) == BLE_STATUS_SUCCESS)
  {
    /*Restore the HAP */
    status = HAP_IAC_RestoreDatabase(pIAC_Inst, &temp_database[0], dataLen);
    BLE_DBG_HAP_DB_MSG("Restore HAP IAC Database returns status 0x%02X\n",status);

    if (status == BLE_STATUS_SUCCESS)
    {
      HAP_IAC_NotificationEvt_t evt;
      evt.Status = BLE_STATUS_SUCCESS;
      pIAC_Inst->LinkupState = HAP_IAC_LINKUP_COMPLETE;
      evt.EvtOpcode = IAC_LINKUP_COMPLETE_EVT;
      evt.ConnHandle = pIAC_Inst->pConnInfo->Connection_Handle;
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
  uint8_t       type;
  tBleStatus    status = BLE_STATUS_SUCCESS;

  status = HAP_DB_GetRemoteAddress(Peer_Address_Type, &Peer_Address[0], &type, &cl_addr[0u]);
  if (status != BLE_STATUS_SUCCESS)
  {
    /* Error during Get Remote Address */
    BLE_DBG_HAP_DB_MSG("Check HAP IAC Services Record fails because BLE Address has not been successfully resolved\n");
    return status;
  }

  /*Find in database the record associated to the remote device*/
  if (BLE_AUDIO_STACK_DB_FindRecord(BLEAUDIO_PLAT_NVM_TYPE_HAP_IAC,type,cl_addr) == BLE_STATUS_SUCCESS)
  {
    BLE_DBG_HAP_DB_MSG("Peer Device has HAP IAC information in NVM\n");
    return 1u;
  }
  else
  {
    BLE_DBG_HAP_DB_MSG("Peer Device has no HAP IAC information in NVM\n");
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
  uint8_t       cl_addr[6u] = {0};
  uint8_t       type;
  tBleStatus    status = BLE_STATUS_SUCCESS;

  status = HAP_DB_GetRemoteAddress(PeerIdentityAddressType, &PeerIdentityAddress[0], &type, &cl_addr[0u]);
  if (status != BLE_STATUS_SUCCESS)
  {
    /* Error during Get Remote Address */
    BLE_DBG_HAP_DB_MSG("Remove HAP IAC Services Record fails because BLE Address has not been successfully resolved\n");
    return status;
  }

  status = BLE_AUDIO_STACK_DB_RemoveRecord(BLEAUDIO_PLAT_NVM_TYPE_HAP_IAC,type,&cl_addr[0]);
  BLE_DBG_HAP_DB_MSG("Remove HAP IAC information in NVM returns status 0x%02X\n",status);

  return status;
}
#endif /* (BLE_CFG_HAP_IAC_ROLE == 1u) */

/* Private functions ----------------------------------------------------------*/

/**
  * @brief If an address is a resolvable private address, resolve it. Otherwise just copy the value of the address
  * @param Peer_Address_Type: Type of the address
  * @param Peer_Address: Peer Address
  * @param type: Output Type of the address
  * @param Address: Output address
  * @retval status of the operation
  */
tBleStatus HAP_DB_GetRemoteAddress(uint8_t Peer_Address_Type,
                                   const uint8_t Peer_Address[6u],
                                   uint8_t *type,
                                   uint8_t *Address)
{
  uint8_t status = BLE_STATUS_SUCCESS;

  /* Check if the address is a resolvable private address */
  if (((Peer_Address_Type) == 1u) && ((Peer_Address[5] & 0xC0U) == 0x40U))
  {
    status = aci_gap_check_bonded_device(1,&Peer_Address[0],type,Address);
  }
  else
  {
    memcpy(Address, &Peer_Address[0], 6u);
    *type = Peer_Address_Type;
  }
  return status;
}
#endif /* ((BLE_CFG_HAP_HA_ROLE == 1u) || BLE_CFG_HAP_HARC_ROLE == 1u || BLE_CFG_HAP_IAC_ROLE == 1u) */