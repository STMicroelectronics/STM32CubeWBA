/**
  ******************************************************************************
  * @file    hap_harc.c
  * @author  MCD Application Team
  * @brief   This file contains Hearing Access Profile feature for Hearing Aid
             Remote Controller Role
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
#include "hap_harc.h"
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
#if (BLE_CFG_HAP_HARC_ROLE == 1u)
static tBleStatus HAP_HARC_Linkup_Process(HAP_HARC_Inst_t *pHARC_Inst,uint8_t ErrorCode);
static void HAP_HARC_Receive_HAPresetControlPoint(HAP_HARC_Inst_t *pHARC_Inst, uint8_t *pData, uint8_t DataLen);
static void HAP_HARC_Post_Linkup_Event(HAP_HARC_Inst_t *pHARC_Inst, tBleStatus const Status);
tBleStatus HAP_HARC_Check_HAS_Service(HAP_HARC_Inst_t *pHARC_Inst);
static HAP_HARC_Inst_t *HAP_HARC_GetAvailableInstance(void);
static HAP_HARC_Inst_t *HAP_HARC_GetInstance(uint16_t ConnHandle);
static void HAP_HARC_InitInstance(HAP_HARC_Inst_t *pHARC_Inst);
HAP_HARC_Inst_t *HAP_HARC_GetOtherMember(HAP_HARC_Inst_t *pInst);
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */
/* External functions prototype------------------------------------------------*/

/* Functions Definition ------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/

#if (BLE_CFG_HAP_HARC_ROLE == 1u)
/**
  * @brief  Initialize the Hearing Access Profile as Hearing Aid Remote Controller Role
  * @retval status of the initialization
  */
tBleStatus HAP_HARC_Init(void)
{
  uint8_t i;

  for (i = 0; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
  {
    HAP_HARC_InitInstance(&HAP_Context.HARC.aHARCInst[i]);
  }

  return BLE_STATUS_SUCCESS;
}
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */

/**
  * @brief Link Up the HAP Client with remote HAP Server
  * @param  ConnHandle: Connection handle
  * @param  LinkupMode: LinkUp Mode
  * @note HAP_HARC_LINKUP_COMPLETE_EVT event will be generated once process is complete
  * @retval status of the operation
  */
tBleStatus HAP_HARC_Linkup(uint16_t ConnHandle, HAP_LinkupMode_t LinkupMode)
{
  tBleStatus    hciCmdResult = HCI_COMMAND_DISALLOWED_ERR_CODE;
#if (BLE_CFG_HAP_HARC_ROLE == 1u)
  HAP_HARC_Inst_t  *p_hap_inst;

  if (HAP_Context.Role & HAP_ROLE_HEARING_AID_REMOTE_CONTROLLER)
  {
    BLE_DBG_HAP_HARC_MSG("Start HAP Link Up procedure on ACL Connection Handle 0x%04X\n", ConnHandle);
    /* Check if a HAP Client Instance with specified Connection Handle is already allocated*/
    p_hap_inst = HAP_HARC_GetInstance(ConnHandle);
    if (p_hap_inst == 0)
    {
      /*Get an available HAP Client Instance*/
      p_hap_inst = HAP_HARC_GetAvailableInstance();
      if (p_hap_inst != 0)
      {
        const UseCaseConnInfo_t *pConnInfo;
        if (USECASE_DEV_MGMT_GetConnInfo(ConnHandle, &pConnInfo) == BLE_STATUS_SUCCESS)
        {
          p_hap_inst->pConnInfo = pConnInfo;
          /*Check that HAP LinkUp process is not already started*/
          if (p_hap_inst->LinkupState == HAP_HARC_LINKUP_IDLE)
          {
            p_hap_inst->LinkupMode = LinkupMode;

            if (p_hap_inst->LinkupMode == HAP_LINKUP_MODE_COMPLETE)
            {
              /* First step of Link Up process : find the HAS in the remote GATT Database*/
              hciCmdResult = aci_gatt_disc_all_primary_services(p_hap_inst->pConnInfo->Connection_Handle);
              BLE_DBG_HAP_HARC_MSG("aci_gatt_disc_all_primary_services() returns status 0x%x\n", hciCmdResult);
              if (hciCmdResult == BLE_STATUS_SUCCESS)
              {
                p_hap_inst->AttProcStarted = 1u;
                /*Start Hearing Access Service Linkup*/
                p_hap_inst->LinkupState = HAP_HARC_LINKUP_DISC_SERVICE;
              }
              else
              {
                HAP_HARC_InitInstance(p_hap_inst);
              }
            }
            else if (p_hap_inst->LinkupMode == HAP_LINKUP_MODE_RESTORE)
            {
              /*HAS shall be restored from Service Database*/
              hciCmdResult = HAP_HARC_DB_RestoreClientDatabase(p_hap_inst);
              if (hciCmdResult != BLE_STATUS_SUCCESS)
              {
                BLE_DBG_HAP_HARC_MSG("HAP Link restoration has failed\n");
                HAP_HARC_InitInstance(p_hap_inst);
              }
            }
          }
          else if (p_hap_inst->LinkupState == HAP_HARC_LINKUP_COMPLETE)
          {
            /*HAP Link Up is already performed*/
            BLE_DBG_HAP_HARC_MSG("HAP Link Up is already performed\n");
            hciCmdResult = HCI_COMMAND_DISALLOWED_ERR_CODE;
          }
          else
          {
            BLE_DBG_HAP_HARC_MSG("HAP Link Up process is already in progress\n");
            /*Microphone Control Link Up process is already in progress*/
            hciCmdResult = BLE_STATUS_BUSY;
          }
        }
        else
        {
          BLE_DBG_HAP_HARC_MSG("Did not find related Connection Info Structure\n");
          hciCmdResult = BLE_STATUS_FAILED;
        }
      }
      else
      {
        BLE_DBG_HAP_HARC_MSG("No resource to use a HAP Client Instance\n");
        hciCmdResult = BLE_STATUS_FAILED;
      }
    }
    else
    {
      BLE_DBG_HAP_HARC_MSG("HAP Client Instance is already associated to the connection handle 0x%04X\n",ConnHandle);
      if (p_hap_inst->LinkupState == HAP_HARC_LINKUP_COMPLETE)
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
    BLE_DBG_HAP_HARC_MSG("HAP Linkup aborted because Hearing Aid Remote Controller Role is not registered\n");
  }
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */
  return hciCmdResult;
}

/**
  * @brief Read the remote Hearing Aid Features
  * @param ConnHandle: The connection handle of the remote HAP Server
  * @note HAP_HARC_HA_FEATURES_EVT will be generated upon characteristic read
  * @return Status of the operation
  */
tBleStatus HAP_HARC_ReadHAFeatures(uint16_t ConnHandle)
{
  tBleStatus hciCmdResult = BLE_STATUS_INVALID_PARAMS;
#if (BLE_CFG_HAP_HARC_ROLE == 1u)
  if (HAP_Context.Role & HAP_ROLE_HEARING_AID_REMOTE_CONTROLLER)
  {
    HAP_HARC_Inst_t *p_hap_inst;
    BLE_DBG_HAP_HARC_MSG("Start Read Remote Hearing Aid Features on connection handle 0x%04X\n",ConnHandle);

    p_hap_inst = HAP_HARC_GetInstance(ConnHandle);
    if (p_hap_inst != 0)
    {
      if ((p_hap_inst->AttProcStarted) == 0 && (HAP_Context.HARC.Op == HAP_HARC_OP_NONE))
      {
        if (p_hap_inst->LinkupState == HAP_HARC_LINKUP_COMPLETE)
        {
          if (p_hap_inst->HAFeaturesChar.ValueHandle != 0)
          {
            hciCmdResult = aci_gatt_read_char_value(ConnHandle, p_hap_inst->HAFeaturesChar.ValueHandle);
            BLE_DBG_HAP_HARC_MSG("aci_gatt_read_char_value() (connHandle: %04X, val_handle: %04X) returns status 0x%x\n",
                                ConnHandle,
                                p_hap_inst->HAFeaturesChar.ValueHandle,
                                hciCmdResult);
            if (hciCmdResult == BLE_STATUS_SUCCESS)
            {
              p_hap_inst->AttProcStarted = 1u;
              HAP_Context.HARC.Op = HAP_HARC_OP_READ_HA_FEATURES;
              HAP_Context.HARC.OpParams.ConnHandle = ConnHandle;
              BLE_DBG_HAP_HARC_MSG("Start HAP READ Operation(ConnHandle 0x%04X)\n", ConnHandle);
            }
          }
        }
        else
        {
          BLE_DBG_HAP_HARC_MSG("HAP Linkup is not complete on Connection Handle 0x%04X\n", ConnHandle);
          return HCI_COMMAND_DISALLOWED_ERR_CODE;
        }
      }
      else
      {
        BLE_DBG_HAP_HARC_MSG("An ATT Procedure is ongoing on Connection Handle 0x%04X\n", ConnHandle);
        return BLE_STATUS_BUSY;
      }
    }
    else
    {
      BLE_DBG_HAP_HARC_MSG("Connection Handle 0x%04X doesn't correspond to an allocated HAP Client Instance\n", ConnHandle);
      hciCmdResult = BLE_STATUS_INVALID_PARAMS;
    }
  }
  else
  {
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */
    hciCmdResult = HCI_COMMAND_DISALLOWED_ERR_CODE;
#if (BLE_CFG_HAP_HARC_ROLE == 1u)
  }
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */

  return hciCmdResult;
}

/**
  * @brief Read the remote Active Preset Index
  * @param ConnHandle: The connection handle of the remote HAP Server
  * @note HAP_HARC_ACTIVE_PRESET_INDEX_EVT will be generated upon characteristic read
  * @return Status of the operation
  */
tBleStatus HAP_HARC_ReadActivePresetIndex(uint16_t ConnHandle)
{
  tBleStatus hciCmdResult = BLE_STATUS_INVALID_PARAMS;
#if (BLE_CFG_HAP_HARC_ROLE == 1u)
  if (HAP_Context.Role & HAP_ROLE_HEARING_AID_REMOTE_CONTROLLER)
  {
    HAP_HARC_Inst_t *p_hap_inst;
    BLE_DBG_HAP_HARC_MSG("Start Read Remote Active Preset Index on connection handle 0x%04X\n",ConnHandle);

    p_hap_inst = HAP_HARC_GetInstance(ConnHandle);
    if (p_hap_inst != 0)
    {
      if ((p_hap_inst->AttProcStarted) == 0 && (HAP_Context.HARC.Op == HAP_HARC_OP_NONE))
      {
        if (p_hap_inst->LinkupState == HAP_HARC_LINKUP_COMPLETE)
        {
          if (p_hap_inst->HAFeaturesChar.ValueHandle != 0)
          {
            hciCmdResult = aci_gatt_read_char_value(ConnHandle, p_hap_inst->ActivePresetIndexChar.ValueHandle);
            BLE_DBG_HAP_HARC_MSG("aci_gatt_read_char_value() (connHandle: %04X, val_handle: %04X) returns status 0x%x\n",
                                ConnHandle,
                                p_hap_inst->ActivePresetIndexChar.ValueHandle,
                                hciCmdResult);
            if (hciCmdResult == BLE_STATUS_SUCCESS)
            {
              p_hap_inst->AttProcStarted = 1u;
              HAP_Context.HARC.Op = HAP_HARC_OP_READ_ACTIVE_PRESET_INDEX;
              HAP_Context.HARC.OpParams.ConnHandle = ConnHandle;
              BLE_DBG_HAP_HARC_MSG("Start HAP READ Operation(ConnHandle 0x%04X)\n", ConnHandle);
            }
          }
        }
        else
        {
          BLE_DBG_HAP_HARC_MSG("HAP Linkup is not complete on Connection Handle 0x%04X\n", ConnHandle);
          return HCI_COMMAND_DISALLOWED_ERR_CODE;
        }
      }
      else
      {
        BLE_DBG_HAP_HARC_MSG("An ATT Procedure is ongoing on Connection Handle 0x%04X\n", ConnHandle);
        return BLE_STATUS_BUSY;
      }
    }
    else
    {
      BLE_DBG_HAP_HARC_MSG("Connection Handle 0x%04X doesn't correspond to an allocated HAP Client Instance\n", ConnHandle);
      hciCmdResult = BLE_STATUS_INVALID_PARAMS;
    }
  }
  else
  {
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */
    hciCmdResult = HCI_COMMAND_DISALLOWED_ERR_CODE;
#if (BLE_CFG_HAP_HARC_ROLE == 1u)
  }
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */

  return hciCmdResult;
}

/**
  * @brief Send a requests to read remote HAP Server presets
  * @param ConnHandle: The connection handle of the remote HAP Server
  * @param StartIndex: Index from which presets will be read
  * @param NumPresets: Number of presets to read
  * @note HAP_HARC_PRESET_RECORD_EVT will be generated as response from the server
  * @return Status of the operation
  */
tBleStatus HAP_HARC_ReadPresetsRequest(uint16_t ConnHandle, uint8_t StartIndex, uint8_t NumPresets)
{
  tBleStatus hciCmdResult = BLE_STATUS_INVALID_PARAMS;
#if (BLE_CFG_HAP_HARC_ROLE == 1u)
  if (HAP_Context.Role & HAP_ROLE_HEARING_AID_REMOTE_CONTROLLER)
  {
    HAP_HARC_Inst_t *p_hap_inst;
    BLE_DBG_HAP_HARC_MSG("Start Read Remote Presets Request procedure on connection handle 0x%04X\n",ConnHandle);

    p_hap_inst = HAP_HARC_GetInstance(ConnHandle);
    if (p_hap_inst != 0)
    {
      if ((p_hap_inst->AttProcStarted) == 0 && (HAP_Context.HARC.Op == HAP_HARC_OP_NONE))
      {
        if (p_hap_inst->LinkupState == HAP_HARC_LINKUP_COMPLETE)
        {
          if (p_hap_inst->HAPresetControlPointChar.ValueHandle != 0)
          {
            uint8_t a_value[3] = {
              HAP_HA_CONTROL_POINT_OP_READ_PRESETS_REQUEST,
              StartIndex,
              NumPresets
            };
            hciCmdResult = aci_gatt_write_char_value(ConnHandle,
                                                     p_hap_inst->HAPresetControlPointChar.ValueHandle,
                                                     3,
                                                     &a_value[0]);
            BLE_DBG_HAP_HARC_MSG("aci_gatt_write_char_value() (connHandle: %04X, val_handle: %04X) returns status 0x%x\n",
                                ConnHandle,
                                p_hap_inst->HAPresetControlPointChar.ValueHandle,
                                hciCmdResult);
            if (hciCmdResult == BLE_STATUS_SUCCESS)
            {
              p_hap_inst->AttProcStarted = 1u;
              HAP_Context.HARC.Op = HAP_HARC_OP_READ_PRESETS_REQUEST;
              HAP_Context.HARC.OpParams.ConnHandle = ConnHandle;
              HAP_Context.HARC.OpParams.PresetIndex = StartIndex;
              HAP_Context.HARC.OpParams.NumPreset = NumPresets;
              BLE_DBG_HAP_HARC_MSG("Start HAP_HARC_OP_READ_PRESETS_REQUEST Operation(ConnHandle 0x%04X)\n", ConnHandle);
            }
          }
        }
        else
        {
          BLE_DBG_HAP_HARC_MSG("HAP Linkup is not complete on Connection Handle 0x%04X\n", ConnHandle);
          return HCI_COMMAND_DISALLOWED_ERR_CODE;
        }
      }
      else
      {
        BLE_DBG_HAP_HARC_MSG("An ATT Procedure is ongoing on Connection Handle 0x%04X\n", ConnHandle);
        return BLE_STATUS_BUSY;
      }
    }
    else
    {
      BLE_DBG_HAP_HARC_MSG("Connection Handle 0x%04X doesn't correspond to an allocated HAP Client Instance\n", ConnHandle);
      hciCmdResult = BLE_STATUS_INVALID_PARAMS;
    }
  }
  else
  {
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */
    hciCmdResult = HCI_COMMAND_DISALLOWED_ERR_CODE;
#if (BLE_CFG_HAP_HARC_ROLE == 1u)
  }
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */

  return hciCmdResult;
}

/**
  * @brief Send a Write Preset Name command
  * @param ConnHandle: The connection handle of the remote HAP Server
  * @param Index: Index of the preset to write
  * @param pName: A pointer to the new name of the preset
  * @param NameLen: The length of the name
  * @note HAP_HARC_PRESET_RECORD_EVT will be generated upon name update
  * @return Status of the operation
  */
tBleStatus HAP_HARC_WritePresetName(uint16_t ConnHandle, uint8_t Index, uint8_t* pName, uint8_t NameLen)
{
  tBleStatus hciCmdResult = BLE_STATUS_INVALID_PARAMS;
#if (BLE_CFG_HAP_HARC_ROLE == 1u)
  if (HAP_Context.Role & HAP_ROLE_HEARING_AID_REMOTE_CONTROLLER)
  {
    HAP_HARC_Inst_t *p_hap_inst;
    BLE_DBG_HAP_HARC_MSG("Start Write Preset Name on connection handle 0x%04X\n",ConnHandle);

    p_hap_inst = HAP_HARC_GetInstance(ConnHandle);
    if (p_hap_inst != 0)
    {
      if ((p_hap_inst->AttProcStarted) == 0 && (HAP_Context.HARC.Op == HAP_HARC_OP_NONE))
      {
        if (p_hap_inst->LinkupState == HAP_HARC_LINKUP_COMPLETE)
        {
          if (p_hap_inst->HAPresetControlPointChar.ValueHandle != 0)
          {
            uint8_t a_value[2 + HAP_MAX_PRESET_NAME_LEN];
            a_value[0] = HAP_HA_CONTROL_POINT_OP_WRITE_PRESET_NAME,
            a_value[1] = Index;
            memcpy(&a_value[2], pName, NameLen);

            hciCmdResult = aci_gatt_write_char_value(ConnHandle,
                                                     p_hap_inst->HAPresetControlPointChar.ValueHandle,
                                                     2 + NameLen,
                                                     &a_value[0]);
            BLE_DBG_HAP_HARC_MSG("aci_gatt_write_char_value() (connHandle: %04X, val_handle: %04X) returns status 0x%x\n",
                                ConnHandle,
                                p_hap_inst->HAPresetControlPointChar.ValueHandle,
                                hciCmdResult);
            if (hciCmdResult == BLE_STATUS_SUCCESS)
            {
              p_hap_inst->AttProcStarted = 1u;
              HAP_Context.HARC.Op = HAP_HARC_OP_WRITE_PRESET_NAME;
              HAP_Context.HARC.OpParams.ConnHandle = ConnHandle;
              HAP_Context.HARC.OpParams.PresetIndex = Index;
              memcpy(&HAP_Context.HARC.OpParams.PresetName[0], pName, NameLen);
              HAP_Context.HARC.OpParams.NameLen = NameLen;
              BLE_DBG_HAP_HARC_MSG("Start HAP WRITE Operation(ConnHandle 0x%04X)\n", ConnHandle);
            }
          }
        }
        else
        {
          BLE_DBG_HAP_HARC_MSG("HAP Linkup is not complete on Connection Handle 0x%04X\n", ConnHandle);
          return HCI_COMMAND_DISALLOWED_ERR_CODE;
        }
      }
      else
      {
        BLE_DBG_HAP_HARC_MSG("An ATT Procedure is ongoing on Connection Handle 0x%04X\n", ConnHandle);
        return BLE_STATUS_BUSY;
      }
    }
    else
    {
      BLE_DBG_HAP_HARC_MSG("Connection Handle 0x%04X doesn't correspond to an allocated HAP Client Instance\n", ConnHandle);
      hciCmdResult = BLE_STATUS_INVALID_PARAMS;
    }
  }
  else
  {
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */
    hciCmdResult = HCI_COMMAND_DISALLOWED_ERR_CODE;
#if (BLE_CFG_HAP_HARC_ROLE == 1u)
  }
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */

  return hciCmdResult;
}

/**
  * @brief Set preset with specified index on remote HAP Server
  * @param ConnHandle: The connection handle of the remote HAP Server
  * @param Index: The connection handle of the remote HAP Server
  * @param SyncLocally: 1 to enable local synchronization if supported by the remote server
  * @note HAP_HARC_ACTIVE_PRESET_INDEX_EVT will be generated if active preset changes
  * @return Status of the operation
  */
tBleStatus HAP_HARC_SetActivePreset(uint16_t ConnHandle, uint8_t Index, uint8_t SyncLocally)
{
  tBleStatus hciCmdResult = BLE_STATUS_INVALID_PARAMS;
#if (BLE_CFG_HAP_HARC_ROLE == 1u)
  if (HAP_Context.Role & HAP_ROLE_HEARING_AID_REMOTE_CONTROLLER)
  {
    HAP_HARC_Inst_t *p_hap_inst;
    BLE_DBG_HAP_HARC_MSG("Start Active Preset on connection handle 0x%04X\n",ConnHandle);

    p_hap_inst = HAP_HARC_GetInstance(ConnHandle);
    if (p_hap_inst != 0)
    {
      if ((p_hap_inst->AttProcStarted) == 0 && (HAP_Context.HARC.Op == HAP_HARC_OP_NONE))
      {
        if (p_hap_inst->LinkupState == HAP_HARC_LINKUP_COMPLETE)
        {
          if (p_hap_inst->HAPresetControlPointChar.ValueHandle != 0)
          {
            uint8_t a_value[2] = {
              HAP_HA_CONTROL_POINT_OP_SET_ACTIVE_PRESET,
              Index
            };

            if (SyncLocally == 1)
            {
              a_value[0] = HAP_HA_CONTROL_POINT_OP_SET_ACTIVE_PRESET_LOCAL_SYNC;
            }

            hciCmdResult = aci_gatt_write_char_value(ConnHandle,
                                                     p_hap_inst->HAPresetControlPointChar.ValueHandle,
                                                     2,
                                                     &a_value[0]);
            BLE_DBG_HAP_HARC_MSG("aci_gatt_write_char_value() (connHandle: %04X, val_handle: %04X) returns status 0x%x\n",
                                ConnHandle,
                                p_hap_inst->HAPresetControlPointChar.ValueHandle,
                                hciCmdResult);
            if (hciCmdResult == BLE_STATUS_SUCCESS)
            {
              p_hap_inst->AttProcStarted = 1u;
              HAP_Context.HARC.Op = HAP_HARC_OP_SET_ACTIVE_PRESET;
              HAP_Context.HARC.OpParams.PresetIndex = Index;
              HAP_Context.HARC.OpParams.ConnHandle = ConnHandle;
              HAP_Context.HARC.OpParams.SyncLocally = SyncLocally;
              BLE_DBG_HAP_HARC_MSG("Start HAP_HARC_OP_SET_ACTIVE_PRESET Operation(ConnHandle 0x%04X)\n", ConnHandle);
            }
          }
        }
        else
        {
          BLE_DBG_HAP_HARC_MSG("HAP Linkup is not complete on Connection Handle 0x%04X\n", ConnHandle);
          return HCI_COMMAND_DISALLOWED_ERR_CODE;
        }
      }
      else
      {
        BLE_DBG_HAP_HARC_MSG("An ATT Procedure is ongoing on Connection Handle 0x%04X\n", ConnHandle);
        return BLE_STATUS_BUSY;
      }
    }
    else
    {
      BLE_DBG_HAP_HARC_MSG("Connection Handle 0x%04X doesn't correspond to an allocated HAP Client Instance\n", ConnHandle);
      hciCmdResult = BLE_STATUS_INVALID_PARAMS;
    }
  }
  else
  {
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */
    hciCmdResult = HCI_COMMAND_DISALLOWED_ERR_CODE;
#if (BLE_CFG_HAP_HARC_ROLE == 1u)
  }
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */

  return hciCmdResult;
}

/**
  * @brief Set next preset on remote HAP Server
  * @param ConnHandle: The connection handle of the remote HAP Server
  * @param SyncLocally: 1 to enable local synchronization if supported by the remote server
  * @note HAP_HARC_ACTIVE_PRESET_INDEX_EVT will be generated if active preset changes
  * @return Status of the operation
  */
tBleStatus HAP_HARC_SetNextPreset(uint16_t ConnHandle, uint8_t SyncLocally)
{
  tBleStatus hciCmdResult = BLE_STATUS_INVALID_PARAMS;
#if (BLE_CFG_HAP_HARC_ROLE == 1u)
  if (HAP_Context.Role & HAP_ROLE_HEARING_AID_REMOTE_CONTROLLER)
  {
    HAP_HARC_Inst_t *p_hap_inst;
    BLE_DBG_HAP_HARC_MSG("Start Next Preset on connection handle 0x%04X\n",ConnHandle);

    p_hap_inst = HAP_HARC_GetInstance(ConnHandle);
    if (p_hap_inst != 0)
    {
      if ((p_hap_inst->AttProcStarted) == 0 && (HAP_Context.HARC.Op == HAP_HARC_OP_NONE))
      {
        if (p_hap_inst->LinkupState == HAP_HARC_LINKUP_COMPLETE)
        {
          if (p_hap_inst->HAPresetControlPointChar.ValueHandle != 0)
          {
            uint8_t a_value[1] = {
              HAP_HA_CONTROL_POINT_OP_SET_NEXT_PRESET
            };

            if (SyncLocally == 1)
            {
              a_value[0] = HAP_HA_CONTROL_POINT_OP_SET_NEXT_PRESET_LOCAL_SYNC;
            }

            hciCmdResult = aci_gatt_write_char_value(ConnHandle,
                                                     p_hap_inst->HAPresetControlPointChar.ValueHandle,
                                                     1,
                                                     &a_value[0]);
            BLE_DBG_HAP_HARC_MSG("aci_gatt_write_char_value() (connHandle: %04X, val_handle: %04X) returns status 0x%x\n",
                                ConnHandle,
                                p_hap_inst->HAPresetControlPointChar.ValueHandle,
                                hciCmdResult);
            if (hciCmdResult == BLE_STATUS_SUCCESS)
            {
              p_hap_inst->AttProcStarted = 1u;
              HAP_Context.HARC.Op = HAP_HARC_OP_SET_NEXT_PRESET;
              HAP_Context.HARC.OpParams.ConnHandle = ConnHandle;
              HAP_Context.HARC.OpParams.SyncLocally = SyncLocally;
              BLE_DBG_HAP_HARC_MSG("Start HAP_HARC_OP_SET_NEXT_PRESET Operation(ConnHandle 0x%04X)\n", ConnHandle);
            }
          }
        }
        else
        {
          BLE_DBG_HAP_HARC_MSG("HAP Linkup is not complete on Connection Handle 0x%04X\n", ConnHandle);
          return HCI_COMMAND_DISALLOWED_ERR_CODE;
        }
      }
      else
      {
        BLE_DBG_HAP_HARC_MSG("An ATT Procedure is ongoing on Connection Handle 0x%04X\n", ConnHandle);
        return BLE_STATUS_BUSY;
      }
    }
    else
    {
      BLE_DBG_HAP_HARC_MSG("Connection Handle 0x%04X doesn't correspond to an allocated HAP Client Instance\n", ConnHandle);
      hciCmdResult = BLE_STATUS_INVALID_PARAMS;
    }
  }
  else
  {
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */
    hciCmdResult = HCI_COMMAND_DISALLOWED_ERR_CODE;
#if (BLE_CFG_HAP_HARC_ROLE == 1u)
  }
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */

  return hciCmdResult;
}

/**
  * @brief Set previous preset on remote HAP Server
  * @param ConnHandle: The connection handle of the remote HAP Server
  * @param SyncLocally: 1 to enable local synchronization if supported by the remote server
  * @note HAP_HARC_ACTIVE_PRESET_INDEX_EVT will be generated if active preset changes
  * @return Status of the operation
  */
tBleStatus HAP_HARC_SetPreviousPreset(uint16_t ConnHandle, uint8_t SyncLocally)
{
  tBleStatus hciCmdResult = BLE_STATUS_INVALID_PARAMS;
#if (BLE_CFG_HAP_HARC_ROLE == 1u)
  if (HAP_Context.Role & HAP_ROLE_HEARING_AID_REMOTE_CONTROLLER)
  {
    HAP_HARC_Inst_t *p_hap_inst;
    BLE_DBG_HAP_HARC_MSG("Start Previous Preset on connection handle 0x%04X\n",ConnHandle);

    p_hap_inst = HAP_HARC_GetInstance(ConnHandle);
    if (p_hap_inst != 0)
    {
      if ((p_hap_inst->AttProcStarted) == 0 && (HAP_Context.HARC.Op == HAP_HARC_OP_NONE))
      {
        if (p_hap_inst->LinkupState == HAP_HARC_LINKUP_COMPLETE)
        {
          if (p_hap_inst->HAPresetControlPointChar.ValueHandle != 0)
          {
            uint8_t a_value[1] = {
              HAP_HA_CONTROL_POINT_OP_SET_PREVIOUS_PRESET
            };

            if (SyncLocally == 1)
            {
              a_value[0] = HAP_HA_CONTROL_POINT_OP_SET_PREVIOUS_PRESET_LOCAL_SYNC;
            }

            hciCmdResult = aci_gatt_write_char_value(ConnHandle,
                                                     p_hap_inst->HAPresetControlPointChar.ValueHandle,
                                                     1,
                                                     &a_value[0]);
            BLE_DBG_HAP_HARC_MSG("aci_gatt_write_char_value() (connHandle: %04X, val_handle: %04X) returns status 0x%x\n",
                                ConnHandle,
                                p_hap_inst->HAPresetControlPointChar.ValueHandle,
                                hciCmdResult);
            if (hciCmdResult == BLE_STATUS_SUCCESS)
            {
              p_hap_inst->AttProcStarted = 1u;
              HAP_Context.HARC.Op = HAP_HARC_OP_SET_PREVIOUS_PRESET;
              HAP_Context.HARC.OpParams.ConnHandle = ConnHandle;
              HAP_Context.HARC.OpParams.SyncLocally = SyncLocally;
              BLE_DBG_HAP_HARC_MSG("Start HAP_HARC_OP_SET_PREVIOUS_PRESET Operation(ConnHandle 0x%04X)\n", ConnHandle);
            }
          }
        }
        else
        {
          BLE_DBG_HAP_HARC_MSG("HAP Linkup is not complete on Connection Handle 0x%04X\n", ConnHandle);
          return HCI_COMMAND_DISALLOWED_ERR_CODE;
        }
      }
      else
      {
        BLE_DBG_HAP_HARC_MSG("An ATT Procedure is ongoing on Connection Handle 0x%04X\n", ConnHandle);
        return BLE_STATUS_BUSY;
      }
    }
    else
    {
      BLE_DBG_HAP_HARC_MSG("Connection Handle 0x%04X doesn't correspond to an allocated HAP Client Instance\n", ConnHandle);
      hciCmdResult = BLE_STATUS_INVALID_PARAMS;
    }
  }
  else
  {
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */
    hciCmdResult = HCI_COMMAND_DISALLOWED_ERR_CODE;
#if (BLE_CFG_HAP_HARC_ROLE == 1u)
  }
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */

  return hciCmdResult;
}

#if (BLE_CFG_HAP_HARC_ROLE == 1u)
/**
   * @brief  Store GATT Database of the HAP Characteristics and Services
   * @param  ConnHandle: Connetion Handle of the remote connection
   * @param  [in] pData: pointer on buffer in which GATT Database of HAP characteristics in which information is stored
   * @param  [in] MaxDataLen: maximum data length to store
   * @param  [out] len : length in bytes of stored data
   * @retval status of the operation
   */
tBleStatus HAP_HARC_StoreDatabase(uint16_t ConnHandle, uint8_t *pData, uint16_t MaxDataLen, uint16_t *len)
{
  tBleStatus                    status = BLE_STATUS_SUCCESS;
  uint16_t                      remain = MaxDataLen;
  HAP_HARC_Inst_t *p_hap_inst;

  p_hap_inst = HAP_HARC_GetInstance(ConnHandle);
  if (p_hap_inst != 0)
  {
    *len = 0u;

    if (p_hap_inst->LinkupState == HAP_HARC_LINKUP_COMPLETE)
    {
      /* Store HAS Service information*/
      if (remain >= 8u)
      {
        /* Store :
         * - 2 bytes for HAS Service Start Handle Attribute handle
         * - 1 byte for Characteristic type
         * - 2 bytes for HAS Service UUID
         * - 1 byte to indicate data length
         */
        /* HAS Service Start Attribute handle */
        pData[(*len)] = (uint8_t) (p_hap_inst->HASServiceStartHandle);
        pData[(*len)+1] = (uint8_t) ((p_hap_inst->HASServiceStartHandle >> 8 ));
        /* Characteristic Type*/
        pData[(*len)+2] = UUID_TYPE_16;
        /* HAS UUID*/
        pData[(*len)+3] = (uint8_t) HEARING_ACCESS_SERVICE_UUID;
        pData[(*len)+4] = (uint8_t) ((HEARING_ACCESS_SERVICE_UUID >> 8 ));
        /*value length*/
        pData[(*len)+5] = 2;
        /*save Service End Handle*/
        pData[(*len)+6] = (uint8_t) (p_hap_inst->HASServiceEndHandle);
        pData[(*len)+7] = (uint8_t) ((p_hap_inst->HASServiceEndHandle >> 8 ));
        remain -= 8u;
        *len += 8u;
      }
      else
      {
        BLE_DBG_HAP_HARC_MSG("No sufficient resource to store data of HAS Service\n");
        status = BLE_STATUS_INSUFFICIENT_RESOURCES;
      }

      /* Store HA Features information*/
      if (status == BLE_STATUS_SUCCESS)
      {
        if (remain >= 9u)
        {
          /* Store :
           * - 2 bytes for HA Features attribute handle
           * - 1 byte for Characteristic type
           * - 2 bytes for HA Features UUID
           * - 1 byte to indicate data length
           */
          /* HA Features Characteristic Attribute handle */
          BLE_DBG_HAP_HARC_MSG("Store HA Features Characteristic at att handle 0x%04X\n",
                               p_hap_inst->HAFeaturesChar.ValueHandle);
          pData[(*len)] = (uint8_t) (p_hap_inst->HAFeaturesChar.ValueHandle);
          pData[(*len)+1] = (uint8_t) ((p_hap_inst->HAFeaturesChar.ValueHandle>> 8 ));
          /* Characteristic Type*/
          pData[(*len)+2] = UUID_TYPE_16;
          /* HA Features Charactertistic UUID*/
          pData[(*len)+3] = (uint8_t) HEARING_AID_FEATURES_UUID;
          pData[(*len)+4] = (uint8_t) ((HEARING_AID_FEATURES_UUID >> 8 ));
          /*value length*/
          pData[(*len)+5] = 3;
          pData[(*len)+6] = p_hap_inst->HAFeaturesChar.Properties;
          pData[(*len)+7] = (uint8_t) (p_hap_inst->HAFeaturesChar.DescHandle);
          pData[(*len)+8] = (uint8_t) ((p_hap_inst->HAFeaturesChar.DescHandle>> 8 ));

          remain -= 9u;
          *len += 9u;
        }
        else
        {
          BLE_DBG_HAP_HARC_MSG("No sufficient resource to store data of HA Features Characteristic\n");
          status = BLE_STATUS_INSUFFICIENT_RESOURCES;
        }
      }

      /* Store Hearing Aid Preset Control Point Charactertistic information*/
      if (status == BLE_STATUS_SUCCESS)
      {
        if (remain >= 9u)
        {
          /* Store :
           * - 2 bytes for HA Preset Control Point attribute handle
           * - 1 byte for Characteristic type
           * - 2 bytes for HA Preset Control Point UUID
           * - 1 byte to indicate data length
           */
          /* HA Preset Control Point Characteristic Attribute handle */
          BLE_DBG_HAP_HARC_MSG("Store HA Preset Control Point Characteristic at att handle 0x%04X\n",
                               p_hap_inst->HAPresetControlPointChar.ValueHandle);
          pData[(*len)] = (uint8_t) (p_hap_inst->HAPresetControlPointChar.ValueHandle);
          pData[(*len)+1] = (uint8_t) ((p_hap_inst->HAPresetControlPointChar.ValueHandle>> 8 ));
          /* Characteristic Type*/
          pData[(*len)+2] = UUID_TYPE_16;
          /* HA Preset Control Point charactertistic UUID*/
          pData[(*len)+3] = (uint8_t) HEARING_AID_PRESET_CONTROL_POINT_UUID;
          pData[(*len)+4] = (uint8_t) ((HEARING_AID_PRESET_CONTROL_POINT_UUID >> 8 ));
          /*value length*/
          pData[(*len)+5] = 3;
          pData[(*len)+6] = p_hap_inst->ActivePresetIndexChar.Properties;
          pData[(*len)+7] = (uint8_t) (p_hap_inst->ActivePresetIndexChar.DescHandle);
          pData[(*len)+8] = (uint8_t) ((p_hap_inst->ActivePresetIndexChar.DescHandle>> 8 ));

          remain -= 9u;
          *len += 9u;
        }
        else
        {
          BLE_DBG_HAP_HARC_MSG("No sufficient resource to store data of HA Preset Control Point Characteristic\n");
          status = BLE_STATUS_INSUFFICIENT_RESOURCES;
        }
      }

      /* Store Active Preset Index information*/
      if (status == BLE_STATUS_SUCCESS)
      {
        if (remain >= 9u)
        {
          /* Store :
           * - 2 bytes for Active Preset Index attribute handle
           * - 1 byte for Characteristic type
           * - 2 bytes for Active Preset Index UUID
           * - 1 byte to indicate data length
           */
          /* Active Preset Index Characteristic Attribute handle */
          BLE_DBG_HAP_HARC_MSG("Store Active Preset Index Characteristic at att handle 0x%04X\n",
                               p_hap_inst->ActivePresetIndexChar.ValueHandle);
          pData[(*len)] = (uint8_t) (p_hap_inst->ActivePresetIndexChar.ValueHandle);
          pData[(*len)+1] = (uint8_t) ((p_hap_inst->ActivePresetIndexChar.ValueHandle>> 8 ));
          /* Characteristic Type*/
          pData[(*len)+2] = UUID_TYPE_16;
          /* Active Preset Index Charactertistic UUID*/
          pData[(*len)+3] = (uint8_t) ACTIVE_PRESET_INDEX_UUID;
          pData[(*len)+4] = (uint8_t) ((ACTIVE_PRESET_INDEX_UUID >> 8 ));
          /*value length*/
          pData[(*len)+5] = 3;
          pData[(*len)+6] = p_hap_inst->ActivePresetIndexChar.Properties;
          pData[(*len)+7] = (uint8_t) (p_hap_inst->ActivePresetIndexChar.DescHandle);
          pData[(*len)+8] = (uint8_t) ((p_hap_inst->ActivePresetIndexChar.DescHandle>> 8 ));

          remain -= 9u;
          *len += 9u;
        }
        else
        {
          BLE_DBG_HAP_HARC_MSG("No sufficient resource to store data of Active Preset Index Characteristic\n");
          status = BLE_STATUS_INSUFFICIENT_RESOURCES;
        }
      }
    }
  }
  return status;
}

/**
   * @brief  Restore GATT Database of the HAS characteristics
   * @param  pHARC_Inst: pointer on HAS Client Instance
   * @param  pData: pointer on buffer including GATT Database of HAS characteristics
   * @param  len : length in bytes of stored data
   * @retval status of the operation
   */
tBleStatus HAP_HARC_RestoreDatabase(HAP_HARC_Inst_t *pHARC_Inst, uint8_t *pData,uint16_t Len)
{
  tBleStatus            status = BLE_STATUS_SUCCESS;
  uint16_t              data_index = 0u;
  uint16_t              att_handle;
  uint8_t               uuid_type;
  uint16_t              uuid_value;
  uint8_t               data_len;

  BLE_DBG_HAP_HARC_MSG("Restore HAS Service for conn handle 0x%04X\n", pHARC_Inst->pConnInfo->Connection_Handle);

  while ((data_index < Len) && (status == BLE_STATUS_SUCCESS))
  {
    att_handle = pData[data_index] | (pData[data_index+1u] << 8);
    uuid_type = pData[data_index+2u];
    uuid_value = pData[data_index+3u] | (pData[data_index+4u] << 8);
    data_len = pData[data_index+5u];

    if ((uuid_type == UUID_TYPE_16) && (uuid_value == HEARING_ACCESS_SERVICE_UUID))
    {
      pHARC_Inst->HASServiceStartHandle = att_handle;
      pHARC_Inst->HASServiceEndHandle = pData[data_index+6u] | (pData[data_index+7u] << 8);
      BLE_DBG_HAP_HARC_MSG("Restore HAS Service information : Start Handle 0x%04X, Start Handle 0x%04X\n",
                           pHARC_Inst->HASServiceStartHandle,
                           pHARC_Inst->HASServiceEndHandle);
    }
    else if ((uuid_type == UUID_TYPE_16) && (uuid_value == HEARING_AID_FEATURES_UUID) && (status == BLE_STATUS_SUCCESS))
    {
      /* HA Features Characteristic */
      BLE_DBG_HAP_HARC_MSG("Restore HA Features Characteristic:\n");
      BLE_DBG_HAP_HARC_MSG("  Handle = 0x%04X\n", att_handle);

      pHARC_Inst->HAFeaturesChar.ValueHandle = att_handle;
      pHARC_Inst->HAFeaturesChar.Properties = pData[data_index+6u];
      pHARC_Inst->HAFeaturesChar.DescHandle = pData[data_index+7u] | (pData[data_index+8u] << 8);
      pHARC_Inst->HAFeaturesChar.EndHandle = pHARC_Inst->HASServiceEndHandle;
    }
    else if ((uuid_type == UUID_TYPE_16) && (uuid_value == HEARING_AID_PRESET_CONTROL_POINT_UUID) && (status == BLE_STATUS_SUCCESS))
    {
      /* HA Preset Control Point Characteristic */
      BLE_DBG_HAP_HARC_MSG("Restore HA Preset Control Point Characteristic:\n");
      BLE_DBG_HAP_HARC_MSG("  Handle = 0x%04X\n", att_handle);

      pHARC_Inst->HAPresetControlPointChar.ValueHandle = att_handle;
      pHARC_Inst->HAPresetControlPointChar.Properties = pData[data_index+6u];
      pHARC_Inst->HAPresetControlPointChar.DescHandle = pData[data_index+7u] | (pData[data_index+8u] << 8);
      pHARC_Inst->HAPresetControlPointChar.EndHandle = pHARC_Inst->HASServiceEndHandle;
    }
    else if ((uuid_type == UUID_TYPE_16) && (uuid_value == ACTIVE_PRESET_INDEX_UUID) && (status == BLE_STATUS_SUCCESS))
    {
      /* Active Preset Index Characteristic */
      BLE_DBG_HAP_HARC_MSG("Restore Active Preset Index Characteristic:\n");
      BLE_DBG_HAP_HARC_MSG("  Handle = 0x%04X\n", att_handle);

      pHARC_Inst->ActivePresetIndexChar.ValueHandle = att_handle;
      pHARC_Inst->ActivePresetIndexChar.Properties = pData[data_index+6u];
      pHARC_Inst->ActivePresetIndexChar.DescHandle = pData[data_index+7u] | (pData[data_index+8u] << 8);
      pHARC_Inst->ActivePresetIndexChar.EndHandle = pHARC_Inst->HASServiceEndHandle;
    }
    data_index += (data_len + 6u);
  }

  if (status == BLE_STATUS_SUCCESS)
  {
    BLE_DBG_HAP_HARC_MSG("Restore HAS Service is complete with success\n");

    pHARC_Inst->LinkupState = HAP_HARC_LINKUP_COMPLETE;
  }
  else
  {
    BLE_DBG_HAP_HARC_MSG("Restore HAS Service is complete with failure\n");
  }
  return status;
}

/**
  * @brief  Notify ACL Disconnection to HAP HARC
  * @param  ConnHandle: ACL Connection Handle
  */
void HAP_HARC_AclDisconnection(uint16_t ConnHandle)
{
  HAP_HARC_Inst_t *p_hap_inst;
  /* Check if a HAP Client Instance with specified Connection Handle is already allocated*/
  p_hap_inst = HAP_HARC_GetInstance(ConnHandle);
  if (p_hap_inst != 0)
  {
    BLE_DBG_HAP_HARC_MSG("ACL Disconnection on Connection Handle 0x%04X : Reset HAP Client Instance\n",ConnHandle);
    /*Check if a HAP Linkup procedure is in progress*/
    if ((p_hap_inst->LinkupState != HAP_HARC_LINKUP_IDLE) && (p_hap_inst->LinkupState != HAP_HARC_LINKUP_COMPLETE))
    {
      /*Notify that HAP Link Up is complete*/
      HAP_HARC_NotificationEvt_t evt;
      evt.ConnHandle = p_hap_inst->pConnInfo->Connection_Handle;
      evt.Status = BLE_STATUS_FAILED;
      evt.EvtOpcode = (HAP_HARC_NotCode_t) HARC_LINKUP_COMPLETE_EVT;
      evt.pInfo = 0;
      HAP_HARC_Notification(&evt);
      if (p_hap_inst->AttProcStarted == 0u)
      {
        HAP_HARC_InitInstance(p_hap_inst);
      }
      else
      {
        /* Wait ACI_GATT_PROC_COMPLETE_VSEVT_CODE event to deallocate the HAP Client Instance*/
        p_hap_inst->DelayDeallocation = 1u;
      }
    }
    else
    {
      /* Reset HAP Instance */
      if (p_hap_inst->AttProcStarted == 0u)
      {
        HAP_HARC_InitInstance(p_hap_inst);
      }
      else
      {
        /* Wait ACI_GATT_PROC_COMPLETE_VSEVT_CODE event to deallocate the HAP Client Instance*/
        p_hap_inst->DelayDeallocation = 1u;
      }
    }
  }
}

/**
  * @brief  Notify Link Encrypted to HAP HARC
  * @param  ConnHandle: ACL Connection Handle
  */
void HAP_HARC_LinkEncrypted(uint16_t ConnHandle)
{
  const UseCaseConnInfo_t *p_conn_info;
  if ((USECASE_DEV_MGMT_GetConnInfo(ConnHandle,&p_conn_info) == BLE_STATUS_SUCCESS)
      && (HAP_HARC_DB_IsPresent(p_conn_info->Peer_Address_Type,p_conn_info->Peer_Address) == 1u))
  {
    tBleStatus ret;
    /* Restore the HAP Profiles saved in NVM*/
    ret = HAP_Linkup(p_conn_info->Connection_Handle, HAP_LINKUP_MODE_RESTORE);
    BLE_DBG_HAP_HARC_MSG("HAP_Linkup() for Restoration for ConnHandle 0x%04X returns status 0x%02X\n",
                p_conn_info->Connection_Handle,
                ret);
  }
}

/** @brief This function is used by the Device in the HAP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */
SVCCTL_EvtAckStatus_t HAP_HARC_GATT_Event_Handler(void *pEvent)
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
      HAP_HARC_Inst_t *p_hap_inst;
      switch (p_blecore_evt->ecode)
      {
        case ACI_ATT_READ_BY_GROUP_TYPE_RESP_VSEVT_CODE:
        {
          aci_att_read_by_group_type_resp_event_rp0 *pr = (void*)p_blecore_evt->data;
          uint8_t numServ, i, idx;
          uint16_t uuid;

          /* Check if a HAP Instance with specified Connection Handle exists*/
          p_hap_inst = HAP_HARC_GetInstance(pr->Connection_Handle);
          if (p_hap_inst != 0)
          {
            BLE_DBG_HAP_HARC_MSG("ACI_ATT_READ_BY_GROUP_TYPE_RESP_EVENT is received on conn handle %04X\n",
                             pr->Connection_Handle);

            /*Check that HAP Link Up Process State is in Service Discovery state*/
            if ((p_hap_inst->LinkupState & HAP_HARC_LINKUP_DISC_SERVICE) == HAP_HARC_LINKUP_DISC_SERVICE)
            {
              return_value = SVCCTL_EvtAckFlowEnable;
              numServ = (pr->Data_Length) / pr->Attribute_Data_Length;
              BLE_DBG_HAP_HARC_MSG("Number of services in the GATT response : %d\n",numServ);
              if (pr->Attribute_Data_Length == 6)
              {
                idx = 4;
                for (i=0; i<numServ; i++)
                {
                  uuid = UNPACK_2_BYTE_PARAMETER(&pr->Attribute_Data_List[idx]);
                  /*Check that UUID in the GATT response corresponds to the HAS */
                  if (uuid == HEARING_ACCESS_SERVICE_UUID)
                  {
                    /* Save start handle and the end handle of the HAS
                     * for the next step of the HAP Link Up process
                     */
                    p_hap_inst->HASServiceStartHandle = UNPACK_2_BYTE_PARAMETER(&pr->Attribute_Data_List[idx-4]);
                    p_hap_inst->HASServiceEndHandle = UNPACK_2_BYTE_PARAMETER (&pr->Attribute_Data_List[idx-2]);
                    BLE_DBG_HAP_HARC_MSG("Hearing Access Service has been found (start: %04X, end: %04X)\n",
                                        p_hap_inst->HASServiceStartHandle,
                                        p_hap_inst->HASServiceEndHandle);
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
          p_hap_inst = HAP_HARC_GetInstance(pr->Connection_Handle);
          if (p_hap_inst != 0)
          {
            /* the event data will be
             * 2 bytes start handle
             * 1 byte char properties
             * 2 bytes handle
             * 2 or 16 bytes data
             */
            BLE_DBG_HAP_HARC_MSG("ACI_ATT_READ_BY_TYPE_RESP_EVENT is received on connHandle %04X\n",pr->Connection_Handle);

            /*Check that HAP Link Up Process State is in Characteristics Discovery state*/
            if ((p_hap_inst->LinkupState & HAP_HARC_LINKUP_DISC_CHAR) == HAP_HARC_LINKUP_DISC_CHAR)
            {
              idx = 5;
              data_length = pr->Data_Length;
              if (pr->Handle_Value_Pair_Length == 7)
              {
                data_length -= 1;
                /*Check if characteristic handle corresponds to the Hearing Access Service range */
                if (((UNPACK_2_BYTE_PARAMETER(&pr->Handle_Value_Pair_Data[0u])>= p_hap_inst->HASServiceStartHandle)
                     && (UNPACK_2_BYTE_PARAMETER(&pr->Handle_Value_Pair_Data[0u]) <= p_hap_inst->HASServiceEndHandle)))
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

                    /* Check that gatt characteristic is valid or not : this pointer corresponds
                     * to the previous found characteristic :
                     * the end handle of the previously found characteristic of each Service
                     * is calculated thanks start handle of the next characteristic in the GATT Database
                    */
                    if (p_hap_inst->pGattChar != 0x00000000)
                    {
                      p_hap_inst->pGattChar->EndHandle = (start_handle -1u);
                    }

                    switch (uuid)
                    {
                      case HEARING_AID_FEATURES_UUID:
                        BLE_DBG_HAP_HARC_MSG("Hearing Aid Features Characteristic has been found:\n");
                        BLE_DBG_HAP_HARC_MSG("Attribute Handle = %04X\n",start_handle);
                        BLE_DBG_HAP_HARC_MSG("Characteristic Properties = 0x%02X\n",pr->Handle_Value_Pair_Data[idx-3]);
                        BLE_DBG_HAP_HARC_MSG("Handle = 0x%04X\n",handle);
                        p_hap_inst->HAFeaturesChar.ValueHandle = handle;
                        p_hap_inst->HAFeaturesChar.EndHandle = p_hap_inst->HASServiceEndHandle;
                        p_hap_inst->HAFeaturesChar.Properties = pr->Handle_Value_Pair_Data[idx-3];;
                        p_hap_inst->pGattChar = &p_hap_inst->HAFeaturesChar;
                      break;
                      case HEARING_AID_PRESET_CONTROL_POINT_UUID:
                        BLE_DBG_HAP_HARC_MSG("Hearing Aid Preset Control Point Characteristic has been found:\n");
                        BLE_DBG_HAP_HARC_MSG("Attribute Handle = %04X\n",start_handle);
                        BLE_DBG_HAP_HARC_MSG("Characteristic Properties = 0x%02X\n",pr->Handle_Value_Pair_Data[idx-3]);
                        BLE_DBG_HAP_HARC_MSG("Handle = 0x%04X\n",handle);
                        p_hap_inst->HAPresetControlPointChar.ValueHandle = handle;
                        p_hap_inst->HAPresetControlPointChar.EndHandle = p_hap_inst->HASServiceEndHandle;
                        p_hap_inst->HAPresetControlPointChar.Properties = pr->Handle_Value_Pair_Data[idx-3];;
                        p_hap_inst->pGattChar = &p_hap_inst->HAPresetControlPointChar;
                      break;
                      case ACTIVE_PRESET_INDEX_UUID:
                        BLE_DBG_HAP_HARC_MSG("Active Preset Index Characteristic has been found:\n");
                        BLE_DBG_HAP_HARC_MSG("Attribute Handle = %04X\n",start_handle);
                        BLE_DBG_HAP_HARC_MSG("Characteristic Properties = 0x%02X\n",pr->Handle_Value_Pair_Data[idx-3]);
                        BLE_DBG_HAP_HARC_MSG("Handle = 0x%04X\n",handle);
                        p_hap_inst->ActivePresetIndexChar.ValueHandle = handle;
                        p_hap_inst->ActivePresetIndexChar.EndHandle = p_hap_inst->HASServiceEndHandle;
                        p_hap_inst->ActivePresetIndexChar.Properties = pr->Handle_Value_Pair_Data[idx-3];;
                        p_hap_inst->pGattChar = &p_hap_inst->ActivePresetIndexChar;
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
            BLE_DBG_HAP_HARC_MSG("No HAP Client allocated for this connection handle\n");
          }
        }
        break;

        case ACI_ATT_FIND_INFO_RESP_VSEVT_CODE:
        {
          aci_att_read_resp_event_rp0 *pr = (void*)p_blecore_evt->data;
          uint8_t idx;
          uint16_t uuid, handle;
          /* Check if a HAP Client Instance with specified Connection Handle exists*/
          p_hap_inst = HAP_HARC_GetInstance(pr->Connection_Handle);
          if (p_hap_inst != 0)
          {
            BLE_DBG_HAP_HARC_MSG("ACI_ATT_FIND_INFO_RESP_EVENT is received on conn handle %04X\n",pr->Connection_Handle);

            /*Check that HAS Link Up Process State is in Characteristics Descriptor Discovery state*/
            if ((p_hap_inst->LinkupState & HAP_HARC_LINKUP_DISC_CHAR_DESC) == HAP_HARC_LINKUP_DISC_CHAR_DESC)
            {
              idx = 0x03;
              uuid = UNPACK_2_BYTE_PARAMETER(&pr->Attribute_Value[idx]);
              /* store the characteristic handle not the attribute handle */
              handle = UNPACK_2_BYTE_PARAMETER(&pr->Attribute_Value[idx-2]);

              /*Check if characteristic descriptor handle corresponds to the Media Player Service range*/
              if ((handle >= p_hap_inst->HASServiceStartHandle)   \
                 && (handle <= p_hap_inst->HASServiceEndHandle))
              {
                return_value = SVCCTL_EvtAckFlowEnable;
                if (uuid == CLIENT_CHAR_CONFIG_DESCRIPTOR_UUID) /* Client Characteristic Configuration*/
                {
                  /* Store the handle of the Client Characteristic Configuration descriptor */
                  switch (p_hap_inst->CurrentLinkupChar)
                  {
                    case HAS_CHAR_HA_FEATURES:
                    {
                      p_hap_inst->HAFeaturesChar.DescHandle = handle;
                      break;
                    }

                    case HAS_CHAR_HA_PRESET_CTRL_POINT:
                    {
                      p_hap_inst->HAPresetControlPointChar.DescHandle = handle;
                      break;
                    }

                    case HAS_CHAR_ACTIVE_PRESET_INDEX:
                    {
                      p_hap_inst->ActivePresetIndexChar.DescHandle = handle;
                      break;
                    }

                    default:
                      break;
                  }
                }
              }
            }
          }
        }
        break;

        case ACI_ATT_READ_RESP_VSEVT_CODE:
        {
          aci_att_read_resp_event_rp0 *pr = (void*)p_blecore_evt->data;

          /* Check if a HAP Client Instance with specified Connection Handle exists*/
          p_hap_inst = HAP_HARC_GetInstance(pr->Connection_Handle);
          if (p_hap_inst != 0)
          {
            BLE_DBG_HAP_HARC_MSG("ACI_ATT_READ_RESP_EVENT is received on conn handle %04X\n",pr->Connection_Handle);
            /* Handle the ATT read response */
            if ((HAP_Context.HARC.Op == HAP_HARC_OP_READ_HA_FEATURES)
                || (HAP_Context.HARC.Op == HAP_HARC_OP_READ_ACTIVE_PRESET_INDEX)
                || ((p_hap_inst->LinkupState & HAP_HARC_LINKUP_READ_CHAR) == HAP_HARC_LINKUP_READ_CHAR))
            {
              HAP_HARC_NotificationEvt_t evt;
              evt.pInfo = &pr->Attribute_Value[0];
              evt.ConnHandle = p_hap_inst->pConnInfo->Connection_Handle;
              evt.Status = BLE_STATUS_SUCCESS;

              return_value = SVCCTL_EvtAckFlowEnable;

              if ((p_hap_inst->LinkupState & HAP_HARC_LINKUP_READ_CHAR) == HAP_HARC_LINKUP_READ_CHAR)
              {
                switch (p_hap_inst->CurrentLinkupChar)
                {
                  case HAS_CHAR_HA_FEATURES:
                    evt.EvtOpcode = HARC_HA_FEATURES_EVT;
                    p_hap_inst->HAPFeatures = pr->Attribute_Value[0];
                    HAP_HARC_Notification(&evt);
                    break;

                  case HAS_CHAR_ACTIVE_PRESET_INDEX:
                    evt.EvtOpcode = HARC_ACTIVE_PRESET_INDEX_EVT;
                    HAP_HARC_Notification(&evt);
                    break;
                }
              }
              else if (HAP_Context.HARC.Op == HAP_HARC_OP_READ_HA_FEATURES)
              {
                evt.EvtOpcode = HARC_HA_FEATURES_EVT;
                p_hap_inst->HAPFeatures = pr->Attribute_Value[0];
                HAP_HARC_Notification(&evt);
              }
              else if  (HAP_Context.HARC.Op == HAP_HARC_OP_READ_ACTIVE_PRESET_INDEX)
              {
                evt.EvtOpcode = HARC_ACTIVE_PRESET_INDEX_EVT;
                HAP_HARC_Notification(&evt);
              }
            }
          }
        }
        break;

        case ACI_GATT_NOTIFICATION_VSEVT_CODE:
        {
          aci_gatt_notification_event_rp0 *pr = (void*)p_blecore_evt->data;

          BLE_DBG_HAP_HARC_MSG("ACI_GATT_NOTIFICATION_EVENT for attribute handle %04X is received on conn handle %04X\n",
                               pr->Attribute_Handle,
                               pr->Connection_Handle);

          /* Check if a HAP Client Instance with specified Connection Handle exists*/
          p_hap_inst = HAP_HARC_GetInstance(pr->Connection_Handle);
          if (p_hap_inst != 0)
          {
            /* Chec if attribute belongs to HAP Service */
            if (pr->Attribute_Handle >= p_hap_inst->HASServiceStartHandle
                && pr->Attribute_Handle <= p_hap_inst->HASServiceEndHandle)
            {
              return_value = SVCCTL_EvtAckFlowEnable;

              if (pr->Attribute_Handle == p_hap_inst->HAFeaturesChar.ValueHandle
                  && pr->Attribute_Value_Length == 1)
              {
                p_hap_inst->HAPFeatures = pr->Attribute_Value[0];
                HAP_HARC_NotificationEvt_t evt;
                evt.pInfo = &pr->Attribute_Value[0];
                evt.ConnHandle = p_hap_inst->pConnInfo->Connection_Handle;
                evt.Status = BLE_STATUS_SUCCESS;
                evt.EvtOpcode = HARC_HA_FEATURES_EVT;

                HAP_HARC_Notification(&evt);
              }
              else if (pr->Attribute_Handle == p_hap_inst->HAPresetControlPointChar.ValueHandle)
              {
                HAP_HARC_Receive_HAPresetControlPoint(p_hap_inst, &pr->Attribute_Value[0], pr->Attribute_Value_Length);
              }
              else if (pr->Attribute_Handle == p_hap_inst->ActivePresetIndexChar.ValueHandle
                  && pr->Attribute_Value_Length == 1)
              {
                HAP_HARC_NotificationEvt_t evt;
                evt.pInfo = &pr->Attribute_Value[0];
                evt.ConnHandle = p_hap_inst->pConnInfo->Connection_Handle;
                evt.Status = BLE_STATUS_SUCCESS;
                evt.EvtOpcode = HARC_ACTIVE_PRESET_INDEX_EVT;

                HAP_HARC_Notification(&evt);
              }
            }
          }
        }
        break;

        case ACI_GATT_INDICATION_VSEVT_CODE:
        {
          aci_gatt_indication_event_rp0 *pr = (void*)p_blecore_evt->data;

          BLE_DBG_HAP_HARC_MSG("ACI_GATT_INDICATION_VSEVT_CODE for attribute handle %04X is received on conn handle %04X\n",
                               pr->Attribute_Handle,
                               pr->Connection_Handle);

          /* Check if a HAP Client Instance with specified Connection Handle exists*/
          p_hap_inst = HAP_HARC_GetInstance(pr->Connection_Handle);
          if (p_hap_inst != 0)
          {
            /* Chec if attribute belongs to HAP Service */
            if (pr->Attribute_Handle >= p_hap_inst->HASServiceStartHandle
                && pr->Attribute_Handle <= p_hap_inst->HASServiceEndHandle)
            {
              tBleStatus ret;
              return_value = SVCCTL_EvtAckFlowEnable;
              if (pr->Attribute_Handle == p_hap_inst->HAPresetControlPointChar.ValueHandle)
              {
                HAP_HARC_Receive_HAPresetControlPoint(p_hap_inst, &pr->Attribute_Value[0], pr->Attribute_Value_Length);
              }

              /* Confirm Indication */
              ret = aci_gatt_confirm_indication(pr->Connection_Handle);
              if (ret != BLE_STATUS_SUCCESS)
              {
                BLE_DBG_HAP_HARC_MSG("  Fail   : aci_gatt_confirm_indication command, result: 0x%02X\n", ret);
              }
              else
              {
                BLE_DBG_HAP_HARC_MSG("  Success: aci_gatt_confirm_indication command\n");
              }
            }
          }
        }
        break;

        case ACI_GATT_PROC_COMPLETE_VSEVT_CODE:
        {
          aci_gatt_proc_complete_event_rp0 *pr = (void*)p_blecore_evt->data;
          HAP_HARC_Inst_t *p_csipmember_hap_inst = 0;
          /* Check if a HAP Client Instance with specified Connection Handle exists*/
          p_hap_inst = HAP_HARC_GetInstance(pr->Connection_Handle);
          if (p_hap_inst != 0)
          {
            BLE_DBG_HAP_HARC_MSG("ACI_GATT_PROC_COMPLETE_EVENT is received on conn handle %04X (ErrorCode %04X)\n",
                                pr->Connection_Handle,
                                pr->Error_Code);

            /* Check if an ATT Procedure was started*/
            if (p_hap_inst->AttProcStarted != 0u)
            {
              p_hap_inst->AttProcStarted = 0u;
              return_value = SVCCTL_EvtAckFlowEnable;
            }
            /*Check if a HAP Linkup procudre is in progress*/
            if ((p_hap_inst->LinkupState != HAP_HARC_LINKUP_IDLE) && (p_hap_inst->LinkupState != HAP_HARC_LINKUP_COMPLETE))
            {
              if (p_hap_inst->DelayDeallocation == 0u)
              {
                /* GATT Process is complete, continue, if needed, the HAP Link Up Process */
                HAP_HARC_Linkup_Process(p_hap_inst,pr->Error_Code);
              }
            }

            if ((p_hap_inst->DelayDeallocation == 1u) && (p_hap_inst->AttProcStarted == 0u))
            {
              BLE_DBG_HAP_HARC_MSG("Free Completely the HAP Client on conn handle %04X\n",pr->Connection_Handle);
              p_hap_inst->DelayDeallocation = 0u;
              HAP_HARC_InitInstance(p_hap_inst);
            }



            if (HAP_Context.HARC.Op != HAP_HARC_OP_NONE)
            {
              tBleStatus status = BLE_STATUS_SUCCESS;
              uint8_t send_proc_complete = 1u;

              if (p_hap_inst->pConnInfo->CSIPDiscovered == 1u
                  && p_hap_inst->pConnInfo->Size > 1u)
              {
                p_csipmember_hap_inst = HAP_HARC_GetOtherMember(p_hap_inst);
              }

              if ((p_csipmember_hap_inst != 0) \
                  && (p_csipmember_hap_inst->pConnInfo->Connection_Handle != HAP_Context.HARC.OpParams.ConnHandle))
              {
                /* Start Operation on second set member */
                BLE_DBG_HAP_HARC_MSG("Start procedure on Set member with Conn Handle 0x%02X\n",
                                     p_csipmember_hap_inst->pConnInfo->Connection_Handle);
                switch (HAP_Context.HARC.Op)
                {
                  case HAP_HARC_OP_READ_HA_FEATURES:
                  {
                    status = aci_gatt_read_char_value(p_csipmember_hap_inst->pConnInfo->Connection_Handle,
                                                      p_csipmember_hap_inst->HAFeaturesChar.ValueHandle);
                    BLE_DBG_HAP_HARC_MSG("aci_gatt_read_char_value() (connHandle: %04X, val_handle: %04X) returns status 0x%x\n",
                                        p_csipmember_hap_inst->pConnInfo->Connection_Handle,
                                        p_csipmember_hap_inst->HAFeaturesChar.ValueHandle,
                                        status);
                    if (status == BLE_STATUS_SUCCESS)
                    {
                      p_csipmember_hap_inst->AttProcStarted = 1u;
                      send_proc_complete = 0u;
                    }
                    break;
                  }
                  case HAP_HARC_OP_READ_ACTIVE_PRESET_INDEX:
                  {
                    status = aci_gatt_read_char_value(p_csipmember_hap_inst->pConnInfo->Connection_Handle,
                                                      p_csipmember_hap_inst->ActivePresetIndexChar.ValueHandle);
                    BLE_DBG_HAP_HARC_MSG("aci_gatt_read_char_value() (connHandle: %04X, val_handle: %04X) returns status 0x%x\n",
                                        p_csipmember_hap_inst->pConnInfo->Connection_Handle,
                                        p_csipmember_hap_inst->ActivePresetIndexChar.ValueHandle,
                                        status);
                    if (status == BLE_STATUS_SUCCESS)
                    {
                      p_csipmember_hap_inst->AttProcStarted = 1u;
                      send_proc_complete = 0u;
                    }
                    break;
                  }
                  case HAP_HARC_OP_READ_PRESETS_REQUEST:
                  {
                    uint8_t a_value[3] = {
                      HAP_HA_CONTROL_POINT_OP_READ_PRESETS_REQUEST,
                      HAP_Context.HARC.OpParams.PresetIndex,
                      HAP_Context.HARC.OpParams.NumPreset
                    };
                    status = aci_gatt_write_char_value(p_csipmember_hap_inst->pConnInfo->Connection_Handle,
                                                       p_csipmember_hap_inst->HAPresetControlPointChar.ValueHandle,
                                                       3,
                                                       &a_value[0]);
                    BLE_DBG_HAP_HARC_MSG("aci_gatt_write_char_value() (connHandle: %04X, val_handle: %04X) returns status 0x%x\n",
                                        p_csipmember_hap_inst->pConnInfo->Connection_Handle,
                                        p_csipmember_hap_inst->HAPresetControlPointChar.ValueHandle,
                                        status);
                    if (status == BLE_STATUS_SUCCESS)
                    {
                      p_csipmember_hap_inst->AttProcStarted = 1u;
                      send_proc_complete = 0u;
                    }
                    break;
                  }
                  case HAP_HARC_OP_WRITE_PRESET_NAME:
                  {
                    uint8_t a_value[2 + HAP_MAX_PRESET_NAME_LEN];
                    a_value[0] = HAP_HA_CONTROL_POINT_OP_WRITE_PRESET_NAME,
                    a_value[1] = HAP_Context.HARC.OpParams.PresetIndex;
                    memcpy(&a_value[2], &HAP_Context.HARC.OpParams.PresetName[0], HAP_Context.HARC.OpParams.NameLen);

                    status = aci_gatt_write_char_value(p_csipmember_hap_inst->pConnInfo->Connection_Handle,
                                                       p_csipmember_hap_inst->HAPresetControlPointChar.ValueHandle,
                                                       2 + HAP_Context.HARC.OpParams.NameLen,
                                                       &a_value[0]);
                    BLE_DBG_HAP_HARC_MSG("aci_gatt_write_char_value() (connHandle: %04X, val_handle: %04X) returns status 0x%x\n",
                                        p_csipmember_hap_inst->pConnInfo->Connection_Handle,
                                        p_csipmember_hap_inst->HAPresetControlPointChar.ValueHandle,
                                        status);
                    if (status == BLE_STATUS_SUCCESS)
                    {
                      p_csipmember_hap_inst->AttProcStarted = 1u;
                      send_proc_complete = 0u;
                    }
                    break;
                  }
                  case HAP_HARC_OP_SET_ACTIVE_PRESET:
                  {
                    if (HAP_Context.HARC.OpParams.SyncLocally == 0
                        && ((p_csipmember_hap_inst->HAPFeatures & HAP_INDEPENDANT_PRESETS) == 0))
                    {
                      /* Only execute on second device when Local Sync and independant presets are disabled */
                      uint8_t a_value[2] = {
                        HAP_HA_CONTROL_POINT_OP_SET_ACTIVE_PRESET,
                        HAP_Context.HARC.OpParams.PresetIndex
                      };

                      BLE_DBG_HAP_HARC_MSG("Preset Index = %d\n", a_value[1]);


                      status = aci_gatt_write_char_value(p_csipmember_hap_inst->pConnInfo->Connection_Handle,
                                                         p_csipmember_hap_inst->HAPresetControlPointChar.ValueHandle,
                                                         2,
                                                         &a_value[0]);
                      BLE_DBG_HAP_HARC_MSG("aci_gatt_write_char_value() (connHandle: %04X, val_handle: %04X) returns status 0x%x\n",
                                          p_csipmember_hap_inst->pConnInfo->Connection_Handle,
                                          p_csipmember_hap_inst->HAPresetControlPointChar.ValueHandle,
                                          status);
                      if (status == BLE_STATUS_SUCCESS)
                      {
                        p_csipmember_hap_inst->AttProcStarted = 1u;
                        send_proc_complete = 0u;
                      }
                    }
                    break;
                  }
                  case HAP_HARC_OP_SET_NEXT_PRESET:
                  {
                    if (HAP_Context.HARC.OpParams.SyncLocally == 0
                        && ((p_csipmember_hap_inst->HAPFeatures & HAP_INDEPENDANT_PRESETS) == 0))
                    {
                      /* Only execute on second device when Local Sync and independant presets are disabled */
                      uint8_t a_value[1] = {
                        HAP_HA_CONTROL_POINT_OP_SET_NEXT_PRESET
                      };

                      if (HAP_Context.HARC.OpParams.SyncLocally == 1)
                      {
                        a_value[0] = HAP_HA_CONTROL_POINT_OP_SET_NEXT_PRESET_LOCAL_SYNC;
                      }

                      status = aci_gatt_write_char_value(p_csipmember_hap_inst->pConnInfo->Connection_Handle,
                                                         p_csipmember_hap_inst->HAPresetControlPointChar.ValueHandle,
                                                         1,
                                                         &a_value[0]);
                      BLE_DBG_HAP_HARC_MSG("aci_gatt_write_char_value() (connHandle: %04X, val_handle: %04X) returns status 0x%x\n",
                                          p_csipmember_hap_inst->pConnInfo->Connection_Handle,
                                          p_csipmember_hap_inst->HAPresetControlPointChar.ValueHandle,
                                          status);
                      if (status == BLE_STATUS_SUCCESS)
                      {
                        p_csipmember_hap_inst->AttProcStarted = 1u;
                        send_proc_complete = 0u;
                      }
                    }
                    break;
                  }
                  case HAP_HARC_OP_SET_PREVIOUS_PRESET:
                  {
                    if (HAP_Context.HARC.OpParams.SyncLocally == 0
                        && ((p_csipmember_hap_inst->HAPFeatures & HAP_INDEPENDANT_PRESETS) == 0))
                    {
                      /* Only execute on second device when Local Sync and independant presets are disabled */
                      uint8_t a_value[1] = {
                        HAP_HA_CONTROL_POINT_OP_SET_PREVIOUS_PRESET
                      };

                      if (HAP_Context.HARC.OpParams.SyncLocally == 1)
                      {
                        a_value[0] = HAP_HA_CONTROL_POINT_OP_SET_PREVIOUS_PRESET_LOCAL_SYNC;
                      }

                      status = aci_gatt_write_char_value(p_csipmember_hap_inst->pConnInfo->Connection_Handle,
                                                         p_csipmember_hap_inst->HAPresetControlPointChar.ValueHandle,
                                                         1,
                                                         &a_value[0]);
                      BLE_DBG_HAP_HARC_MSG("aci_gatt_write_char_value() (connHandle: %04X, val_handle: %04X) returns status 0x%x\n",
                                          p_csipmember_hap_inst->pConnInfo->Connection_Handle,
                                          p_csipmember_hap_inst->HAPresetControlPointChar.ValueHandle,
                                          status);
                      if (status == BLE_STATUS_SUCCESS)
                      {
                        p_csipmember_hap_inst->AttProcStarted = 1u;
                        send_proc_complete = 0u;
                      }
                    }
                    break;
                  }
                }
              }

              if (send_proc_complete == 1u)
              {
                HAP_HARC_NotificationEvt_t evt;
                HAP_Context.HARC.Op = HAP_HARC_OP_NONE;

                /* Notify Proc complete */
                evt.pInfo = 0;
                evt.ConnHandle = pr->Connection_Handle;
                evt.Status = status;
                evt.EvtOpcode = HARC_PROC_COMPLETE_EVT;

                HAP_HARC_Notification(&evt);
              }
            }
          }
        }
        break; /*ACI_GATT_PROC_COMPLETE_VSEVT_CODE*/

        case ACI_GATT_ERROR_RESP_VSEVT_CODE:
        {
          aci_gatt_error_resp_event_rp0 *pr = (void*)p_blecore_evt->data;
          /* Check if a HAP Client Instance with specified Connection Handle exists*/
          p_hap_inst = HAP_HARC_GetInstance(pr->Connection_Handle);
          if (p_hap_inst != 0)
          {
            BLE_DBG_HAP_HARC_MSG("ACI_GATT_ERROR_RESP_EVENT is received on conn handle %04X\n",pr->Connection_Handle);
            if (pr->Attribute_Handle >= p_hap_inst->HASServiceStartHandle
                && pr->Attribute_Handle <= p_hap_inst->HASServiceEndHandle)
            {
              return_value = SVCCTL_EvtAckFlowEnable;

              if (HAP_Context.HARC.Op != HAP_HARC_OP_NONE)
              {
                HAP_HARC_NotificationEvt_t evt;

                /* Notify Proc complete */
                evt.pInfo = 0;
                evt.ConnHandle = p_hap_inst->pConnInfo->Connection_Handle;
                evt.Status = pr->Error_Code;
                evt.EvtOpcode = HARC_PROC_COMPLETE_EVT;

                HAP_HARC_Notification(&evt);
                HAP_Context.HARC.Op = HAP_HARC_OP_NONE;
              }
            }
            else
            {
              if (((p_hap_inst->LinkupState & HAP_HARC_LINKUP_DISC_SERVICE) == HAP_HARC_LINKUP_DISC_SERVICE) && (pr->Req_Opcode == 0x10u))
              {
                return_value = SVCCTL_EvtAckFlowEnable;
              }
              if ((((p_hap_inst->LinkupState & HAP_HARC_LINKUP_DISC_SERVICE) == HAP_HARC_LINKUP_DISC_SERVICE) && (pr->Req_Opcode == 0x10u)) \
                || (((p_hap_inst->LinkupState & HAP_HARC_LINKUP_DISC_SERVICE) == HAP_HARC_LINKUP_DISC_SERVICE) && (pr->Req_Opcode == 0x08u)) \
                || (((p_hap_inst->LinkupState & HAP_HARC_LINKUP_DISC_CHAR) == HAP_HARC_LINKUP_DISC_CHAR) && (pr->Req_Opcode == 0x08u)))
              {
                /* Error response returned after :
                 * aci_gatt_disc_all_primary_services()
                 * -aci_gatt_find_included_services()
                 * -aci_gatt_disc_all_char_of_service()
                 */
                return_value = SVCCTL_EvtAckFlowEnable;
              }
            }
            if (p_hap_inst->LinkupState == HAP_HARC_LINKUP_COMPLETE
                && HAP_Context.HARC.Op != HAP_HARC_OP_NONE)
            {
              p_hap_inst->ErrorCode = pr->Error_Code;
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
 * @param  pHARC_Inst: pointer on HAP Client Instance
 * @param  ErrorCode: Error Code from Host Stack when a GATT procedure is complete
 * @retval status of the operation
 */
static tBleStatus HAP_HARC_Linkup_Process(HAP_HARC_Inst_t *pHARC_Inst,uint8_t ErrorCode)
{
  tBleStatus hciCmdResult = BLE_STATUS_FAILED;

  BLE_DBG_HAP_HARC_MSG("HAP Link Up Process, state 0x%x\n",pHARC_Inst->LinkupState);

  /*check that error indicate that linkup process succeeds*/
  if (ErrorCode == 0x00)
  {
    if (pHARC_Inst->LinkupMode == HAP_LINKUP_MODE_COMPLETE)
    {
      if ((pHARC_Inst->LinkupState & HAP_HARC_LINKUP_DISC_SERVICE) == HAP_HARC_LINKUP_DISC_SERVICE)
      {
        pHARC_Inst->LinkupState &= ~HAP_HARC_LINKUP_DISC_SERVICE;
        /* HAP Link Up process is in first Step :
         * HAS discovery in the remote GATT Database
         */
        if (pHARC_Inst->HASServiceEndHandle != 0x0000)
        {
          /* Hearing Access Service has been found */
          BLE_DBG_HAP_HARC_MSG("GATT : Discover HAS Characteristics\n");
          pHARC_Inst->pGattChar = 0u;
          /* Discover all the characteristics of the HAS in the remote GATT Database */
          hciCmdResult = aci_gatt_disc_all_char_of_service(pHARC_Inst->pConnInfo->Connection_Handle,
                                                          pHARC_Inst->HASServiceStartHandle,
                                                          pHARC_Inst->HASServiceEndHandle);
          BLE_DBG_HAP_HARC_MSG("aci_gatt_disc_all_char_of_service() (start: %04X, end: %04X) returns status 0x%x\n",
                              pHARC_Inst->HASServiceStartHandle,
                              pHARC_Inst->HASServiceEndHandle,
                              hciCmdResult);

          if (hciCmdResult == BLE_STATUS_SUCCESS)
          {
            pHARC_Inst->AttProcStarted = 1u;
            pHARC_Inst->LinkupState |= HAP_HARC_LINKUP_DISC_CHAR;
          }
          else
          {
            /*Notify that HAP Link Up is complete*/
            HAP_HARC_Post_Linkup_Event(pHARC_Inst,hciCmdResult);
          }
        }
        else
        {
          BLE_DBG_HAP_HARC_MSG("No HAS is present in the remote GATT Database\n");
          /*Notify that HAP Link Up is complete*/
          HAP_HARC_Post_Linkup_Event(pHARC_Inst,BLE_STATUS_FAILED);
        }
      }
      else if ((pHARC_Inst->LinkupState & HAP_HARC_LINKUP_DISC_CHAR) == HAP_HARC_LINKUP_DISC_CHAR)
      {
        /* Discovery of the characteristics in the Service is complete */
        pHARC_Inst->LinkupState &= ~HAP_HARC_LINKUP_DISC_CHAR;
        /*Check if remote HAS is valid*/
        if (HAP_HARC_Check_HAS_Service(pHARC_Inst) == BLE_STATUS_SUCCESS)
        {
          pHARC_Inst->LinkupState |= HAP_HARC_LINKUP_READ_CHAR;
          pHARC_Inst->CurrentLinkupChar = HAS_CHAR_HA_FEATURES;

          /* read the characteristic value */
          hciCmdResult = aci_gatt_read_char_value(pHARC_Inst->pConnInfo->Connection_Handle,
                                                  pHARC_Inst->HAFeaturesChar.ValueHandle);
          BLE_DBG_HAP_HARC_MSG("aci_gatt_read_char_value() (connHandle: %04X, val_handle: %04X) returns status 0x%x\n",
                              pHARC_Inst->pConnInfo->Connection_Handle,
                              pHARC_Inst->HAFeaturesChar.ValueHandle,
                              hciCmdResult);
          if (hciCmdResult != BLE_STATUS_SUCCESS)
          {
            pHARC_Inst->LinkupState &= ~HAP_HARC_LINKUP_READ_CHAR;
            HAP_HARC_Post_Linkup_Event(pHARC_Inst,hciCmdResult);
          }
          else
          {
            pHARC_Inst->AttProcStarted = 1u;
          }
        }
        else
        {
          /*Notify that HAP Link Up is complete*/
          HAP_HARC_Post_Linkup_Event(pHARC_Inst, hciCmdResult);
        }
      }
      else if ((pHARC_Inst->LinkupState & HAP_HARC_LINKUP_READ_CHAR) == HAP_HARC_LINKUP_READ_CHAR)
      {
        if (pHARC_Inst->CurrentLinkupChar == HAS_CHAR_HA_FEATURES)
        {
          pHARC_Inst->CurrentLinkupChar = HAS_CHAR_ACTIVE_PRESET_INDEX;
          /* read the characteristic value */
          hciCmdResult = aci_gatt_read_char_value(pHARC_Inst->pConnInfo->Connection_Handle,
                                                  pHARC_Inst->ActivePresetIndexChar.ValueHandle);
          BLE_DBG_HAP_HARC_MSG("aci_gatt_read_char_value() (connHandle: %04X, val_handle: %04X) returns status 0x%x\n",
                              pHARC_Inst->pConnInfo->Connection_Handle,
                              pHARC_Inst->ActivePresetIndexChar.ValueHandle,
                              hciCmdResult);
          if (hciCmdResult != BLE_STATUS_SUCCESS)
          {
            pHARC_Inst->LinkupState &= ~HAP_HARC_LINKUP_READ_CHAR;
            HAP_HARC_Post_Linkup_Event(pHARC_Inst,hciCmdResult);
          }
          else
          {
            pHARC_Inst->AttProcStarted = 1u;
          }
        }
        else if (pHARC_Inst->CurrentLinkupChar == HAS_CHAR_ACTIVE_PRESET_INDEX)
        {
          pHARC_Inst->LinkupState &= ~HAP_HARC_LINKUP_READ_CHAR;
          pHARC_Inst->LinkupState |= HAP_HARC_LINKUP_DISC_CHAR_DESC;
          pHARC_Inst->CurrentLinkupChar = HAS_CHAR_HA_FEATURES;
          /* read the characteristic value */
          hciCmdResult = aci_gatt_disc_all_char_desc(pHARC_Inst->pConnInfo->Connection_Handle,
                                                     pHARC_Inst->HAFeaturesChar.ValueHandle,
                                                     pHARC_Inst->HASServiceEndHandle);
          BLE_DBG_HAP_HARC_MSG("aci_gatt_disc_all_char_desc() (connHandle: %04X, val_handle: %04X) returns status 0x%x\n",
                              pHARC_Inst->pConnInfo->Connection_Handle,
                              pHARC_Inst->HAFeaturesChar.ValueHandle,
                              hciCmdResult);
          if (hciCmdResult != BLE_STATUS_SUCCESS)
          {
            pHARC_Inst->LinkupState &= ~HAP_HARC_LINKUP_DISC_CHAR_DESC;
            HAP_HARC_Post_Linkup_Event(pHARC_Inst,hciCmdResult);
          }
          else
          {
            pHARC_Inst->AttProcStarted = 1u;
          }
        }
      }
      else if ((pHARC_Inst->LinkupState & HAP_HARC_LINKUP_DISC_CHAR_DESC) == HAP_HARC_LINKUP_DISC_CHAR_DESC)
      {
        if (pHARC_Inst->CurrentLinkupChar == HAS_CHAR_HA_FEATURES)
        {
          pHARC_Inst->CurrentLinkupChar = HAS_CHAR_HA_PRESET_CTRL_POINT;
          /* read the characteristic value */
          hciCmdResult = aci_gatt_disc_all_char_desc(pHARC_Inst->pConnInfo->Connection_Handle,
                                                     pHARC_Inst->HAPresetControlPointChar.ValueHandle,
                                                     pHARC_Inst->HASServiceEndHandle);
          BLE_DBG_HAP_HARC_MSG("aci_gatt_disc_all_char_desc() (connHandle: %04X, val_handle: %04X) returns status 0x%x\n",
                              pHARC_Inst->pConnInfo->Connection_Handle,
                              pHARC_Inst->HAPresetControlPointChar.ValueHandle,
                              hciCmdResult);
          if (hciCmdResult != BLE_STATUS_SUCCESS)
          {
            pHARC_Inst->LinkupState &= ~HAP_HARC_LINKUP_DISC_CHAR_DESC;
            HAP_HARC_Post_Linkup_Event(pHARC_Inst,hciCmdResult);
          }
          else
          {
            pHARC_Inst->AttProcStarted = 1u;
          }
        }
        else if (pHARC_Inst->CurrentLinkupChar == HAS_CHAR_HA_PRESET_CTRL_POINT)
        {
          pHARC_Inst->CurrentLinkupChar = HAS_CHAR_ACTIVE_PRESET_INDEX;
          /* read the characteristic value */
          hciCmdResult = aci_gatt_disc_all_char_desc(pHARC_Inst->pConnInfo->Connection_Handle,
                                                     pHARC_Inst->ActivePresetIndexChar.ValueHandle,
                                                     pHARC_Inst->HASServiceEndHandle);
          BLE_DBG_HAP_HARC_MSG("aci_gatt_disc_all_char_desc() (connHandle: %04X, val_handle: %04X) returns status 0x%x\n",
                              pHARC_Inst->pConnInfo->Connection_Handle,
                              pHARC_Inst->ActivePresetIndexChar.ValueHandle,
                              hciCmdResult);
          if (hciCmdResult != BLE_STATUS_SUCCESS)
          {
            pHARC_Inst->LinkupState &= ~HAP_HARC_LINKUP_DISC_CHAR_DESC;
            HAP_HARC_Post_Linkup_Event(pHARC_Inst,hciCmdResult);
          }
          else
          {
            pHARC_Inst->AttProcStarted = 1u;
          }
        }
        else if (pHARC_Inst->CurrentLinkupChar == HAS_CHAR_ACTIVE_PRESET_INDEX)
        {
          uint8_t a_client_cfg[2];

          pHARC_Inst->LinkupState &= ~HAP_HARC_LINKUP_DISC_CHAR_DESC;
          pHARC_Inst->LinkupState |= HAP_HARC_LINKUP_CLIENT_CONFIG;

          if ((pHARC_Inst->HAFeaturesChar.DescHandle > pHARC_Inst->ReqHandle)     \
            && ((pHARC_Inst->HAFeaturesChar.Properties & CHAR_PROP_NOTIFY) == CHAR_PROP_NOTIFY))
          {
            pHARC_Inst->CurrentLinkupChar = HAS_CHAR_HA_FEATURES;
            /* ATT_CLIENT_CONFIG_NOTIFICATION : 0x0001u*/
            a_client_cfg[0] = 0x01u;
            a_client_cfg[1] = 0x00u;

            /* write the characteristic descriptor value */
            hciCmdResult = aci_gatt_write_char_desc(pHARC_Inst->pConnInfo->Connection_Handle,
                                                    pHARC_Inst->HAPresetControlPointChar.DescHandle,
                                                    2,
                                                    a_client_cfg);
            BLE_DBG_HAP_HARC_MSG("aci_gatt_write_char_desc() (connHandle: %04X, desc_handle: %04X) returns status 0x%x\n",
                                pHARC_Inst->pConnInfo->Connection_Handle,
                                pHARC_Inst->HAPresetControlPointChar.DescHandle,
                                hciCmdResult);
            if (hciCmdResult != BLE_STATUS_SUCCESS)
            {
              pHARC_Inst->LinkupState &= ~HAP_HARC_LINKUP_CLIENT_CONFIG;
              HAP_HARC_Post_Linkup_Event(pHARC_Inst,hciCmdResult);
            }
            else
            {
              pHARC_Inst->AttProcStarted = 1u;
            }
          }
          else
          {
            pHARC_Inst->CurrentLinkupChar = HAS_CHAR_HA_PRESET_CTRL_POINT;
            /* ATT_CLIENT_CONFIG_INDICATION : 0x0002u*/
            a_client_cfg[0] = 0x02u;
            a_client_cfg[1] = 0x00u;

            /* write the characteristic descriptor value */
            hciCmdResult = aci_gatt_write_char_desc(pHARC_Inst->pConnInfo->Connection_Handle,
                                                    pHARC_Inst->HAPresetControlPointChar.DescHandle,
                                                    2,
                                                    a_client_cfg);
            BLE_DBG_HAP_HARC_MSG("aci_gatt_write_char_desc() (connHandle: %04X, desc_handle: %04X) returns status 0x%x\n",
                                pHARC_Inst->pConnInfo->Connection_Handle,
                                pHARC_Inst->HAPresetControlPointChar.DescHandle,
                                hciCmdResult);
            if (hciCmdResult != BLE_STATUS_SUCCESS)
            {
              pHARC_Inst->LinkupState &= ~HAP_HARC_LINKUP_CLIENT_CONFIG;
              HAP_HARC_Post_Linkup_Event(pHARC_Inst,hciCmdResult);
            }
            else
            {
              pHARC_Inst->AttProcStarted = 1u;
            }
          }
        }
      }
      else if ((pHARC_Inst->LinkupState & HAP_HARC_LINKUP_CLIENT_CONFIG) == HAP_HARC_LINKUP_CLIENT_CONFIG)
      {
        if (pHARC_Inst->CurrentLinkupChar == HAS_CHAR_HA_FEATURES)
        {
          uint8_t a_client_cfg[2];
          pHARC_Inst->CurrentLinkupChar = HAS_CHAR_HA_PRESET_CTRL_POINT;

          /* ATT_CLIENT_CONFIG_INDICATION: 0x0002u*/
          a_client_cfg[0] = 0x02u;
          a_client_cfg[1] = 0x00u;

          /* write the characteristic descriptor value */
          hciCmdResult = aci_gatt_write_char_desc(pHARC_Inst->pConnInfo->Connection_Handle,
                                                  pHARC_Inst->HAPresetControlPointChar.DescHandle,
                                                  2,
                                                  a_client_cfg);
          BLE_DBG_HAP_HARC_MSG("aci_gatt_write_char_desc() (connHandle: %04X, desc_handle: %04X) returns status 0x%x\n",
                              pHARC_Inst->pConnInfo->Connection_Handle,
                              pHARC_Inst->HAPresetControlPointChar.DescHandle,
                              hciCmdResult);
          if (hciCmdResult != BLE_STATUS_SUCCESS)
          {
            pHARC_Inst->LinkupState &= ~HAP_HARC_LINKUP_CLIENT_CONFIG;
            HAP_HARC_Post_Linkup_Event(pHARC_Inst,hciCmdResult);
          }
          else
          {
            pHARC_Inst->AttProcStarted = 1u;
          }
        }
        else if (pHARC_Inst->CurrentLinkupChar == HAS_CHAR_HA_PRESET_CTRL_POINT)
        {
          uint8_t a_client_cfg[2];
          pHARC_Inst->CurrentLinkupChar = HAS_CHAR_ACTIVE_PRESET_INDEX;

          /* ATT_CLIENT_CONFIG_NOTIFICATION : 0x0001u*/
          a_client_cfg[0] = 0x01u;
          a_client_cfg[1] = 0x00u;

          /* write the characteristic descriptor value */
          hciCmdResult = aci_gatt_write_char_desc(pHARC_Inst->pConnInfo->Connection_Handle,
                                                  pHARC_Inst->ActivePresetIndexChar.DescHandle,
                                                  2,
                                                  a_client_cfg);
          BLE_DBG_HAP_HARC_MSG("aci_gatt_write_char_desc() (connHandle: %04X, desc_handle: %04X) returns status 0x%x\n",
                              pHARC_Inst->pConnInfo->Connection_Handle,
                              pHARC_Inst->ActivePresetIndexChar.DescHandle,
                              hciCmdResult);
          if (hciCmdResult != BLE_STATUS_SUCCESS)
          {
            pHARC_Inst->LinkupState &= ~HAP_HARC_LINKUP_CLIENT_CONFIG;
            HAP_HARC_Post_Linkup_Event(pHARC_Inst,hciCmdResult);
          }
          else
          {
            pHARC_Inst->AttProcStarted = 1u;
          }
        }
        else if (pHARC_Inst->CurrentLinkupChar == HAS_CHAR_ACTIVE_PRESET_INDEX)
        {
          /* Linkup process end */
          pHARC_Inst->LinkupState &= ~HAP_HARC_LINKUP_CLIENT_CONFIG;
          /*Notify that BASS Link Up is complete*/
          HAP_HARC_Post_Linkup_Event(pHARC_Inst,BLE_STATUS_SUCCESS);
        }
      }
    }
  }
  else
  {
    if (pHARC_Inst->LinkupMode == HAP_LINKUP_MODE_RESTORE)
    {
      /*Notify that HAP Link Up is complete*/
      HAP_HARC_NotificationEvt_t evt;
      evt.Status = BLE_STATUS_FAILED;
      evt.ConnHandle = pHARC_Inst->pConnInfo->Connection_Handle;
      evt.pInfo = 0;
      evt.EvtOpcode = (HAP_HARC_NotCode_t) HARC_LINKUP_COMPLETE_EVT;
      HAP_HARC_Notification(&evt);
      /*reset the HAP Client Instance*/
      HAP_HARC_InitInstance(pHARC_Inst);
    }
    else
    {
      /*Notify that HAP Link Up is complete*/
      HAP_HARC_Post_Linkup_Event(pHARC_Inst,BLE_STATUS_FAILED);
    }
  }
  return hciCmdResult;
}

/**
  * @brief Called when an ATT Notification or indication has been received on the HA Preset Control Point
  * @param pHARC_Inst: Pointer to HAP Client instance
  * @param pData: Pointer to the data received
  * @param DataLen: Length of the data received
  */
static void HAP_HARC_Receive_HAPresetControlPoint(HAP_HARC_Inst_t *pHARC_Inst, uint8_t *pData, uint8_t DataLen)
{
  if (DataLen > 0)
  {
    switch (pData[0])
    {
      case HAP_HA_CONTROL_POINT_OP_READ_PRESETS_RESPONSE:
      {
        HAP_HARC_NotificationEvt_t evt;
        HAP_PresetUpdate_Info_t info;

        BLE_DBG_HAP_HARC_MSG("HAP_HA_CONTROL_POINT_OP_READ_PRESETS_RESPONSE received from remote HAP Hearing Aid with ConnHandle 0x%x\n",
                             pHARC_Inst->pConnInfo->Connection_Handle);

        info.IsLast = pData[1];
        info.Preset.Index = pData[2];
        info.Preset.Properties = pData[3];
        info.Preset.NameLen = DataLen - 4;
        memcpy(&info.Preset.Name[0], &pData[4], DataLen - 4);

        evt.pInfo = (uint8_t*) &info;
        evt.ConnHandle = pHARC_Inst->pConnInfo->Connection_Handle;
        evt.Status = BLE_STATUS_SUCCESS;
        evt.EvtOpcode = HARC_READ_PRESET_RESPONSE_EVT;

        HAP_HARC_Notification(&evt);
        break;
      }

      case HAP_HA_CONTROL_POINT_OP_PRESET_CHANGED:
      {
        if (DataLen > 1)
        {
          switch (pData[1])
          {
            case HAP_PRESET_CHANGE_ID_GENERIC_UPDATE:
            {
              HAP_HARC_NotificationEvt_t evt;
              HAP_PresetUpdate_Info_t info;

              BLE_DBG_HAP_HARC_MSG("HAP_PRESET_CHANGE_ID_GENERIC_UPDATE received from remote HAP Hearing Aid with ConnHandle 0x%x\n",
                                   pHARC_Inst->pConnInfo->Connection_Handle);

              info.IsLast = pData[2];
              info.PrevIndex = pData[3];
              info.Preset.Index = pData[4];
              info.Preset.Properties = pData[5];
              info.Preset.NameLen = DataLen - 6;
              memcpy(&info.Preset.Name[0], &pData[6], DataLen - 6);

              evt.pInfo = (uint8_t*) &info;
              evt.ConnHandle = pHARC_Inst->pConnInfo->Connection_Handle;
              evt.Status = BLE_STATUS_SUCCESS;
              evt.EvtOpcode = HARC_PRESET_RECORD_UPDATE_EVT;

              HAP_HARC_Notification(&evt);
              break;
            }

            case HAP_PRESET_CHANGE_ID_PRESET_RECORD_DELETED:
            case HAP_PRESET_CHANGE_ID_PRESET_RECORD_AVAILABLE:
            case HAP_PRESET_CHANGE_ID_PRESET_RECORD_UNAVAILABLE:
            {
              HAP_HARC_NotificationEvt_t evt;
              HAP_PresetChangedIndex_Info_t info;


              info.IsLast = pData[2];
              info.Index = pData[3];

              evt.pInfo = (uint8_t*) &info;
              evt.ConnHandle = pHARC_Inst->pConnInfo->Connection_Handle;
              evt.Status = BLE_STATUS_SUCCESS;
              if (pData[1] == HAP_PRESET_CHANGE_ID_PRESET_RECORD_DELETED)
              {
                evt.EvtOpcode = HARC_PRESET_RECORD_DELETED_EVT;
                BLE_DBG_HAP_HARC_MSG("HAP_PRESET_CHANGE_ID_PRESET_RECORD_DELETED received from remote HAP Hearing Aid with ConnHandle 0x%x\n",
                                   pHARC_Inst->pConnInfo->Connection_Handle);
              }
              else if (pData[1] == HAP_PRESET_CHANGE_ID_PRESET_RECORD_AVAILABLE)
              {
                evt.EvtOpcode = HARC_PRESET_RECORD_AVAILABLE_EVT;
                BLE_DBG_HAP_HARC_MSG("HAP_HARC_PRESET_RECORD_AVAILABLE_EVT received from remote HAP Hearing Aid with ConnHandle 0x%x\n",
                                   pHARC_Inst->pConnInfo->Connection_Handle);
              }
              else if (pData[1] == HAP_PRESET_CHANGE_ID_PRESET_RECORD_UNAVAILABLE)
              {
                evt.EvtOpcode = HARC_PRESET_RECORD_UNAVAILABLE_EVT;
                BLE_DBG_HAP_HARC_MSG("HAP_HARC_PRESET_RECORD_UNAVAILABLE_EVT received from remote HAP Hearing Aid with ConnHandle 0x%x\n",
                                   pHARC_Inst->pConnInfo->Connection_Handle);
              }
              else
              {
                break;
              }

              HAP_HARC_Notification(&evt);
              break;
            }
          }
        }
        break;
      }
    }
  }
}

/**
 * @brief Notify to upper layer that HAP Link Up Process is complete
 * @param  pHARC_Inst: pointer on HAP Client Instance
 * @param  Status : status of the operation
 */
static void HAP_HARC_Post_Linkup_Event(HAP_HARC_Inst_t *pHARC_Inst, tBleStatus const Status)
{
  HAP_HARC_NotificationEvt_t evt;
  BLE_DBG_HAP_HARC_MSG("HAP Client notifies HAP Linkup is complete with status 0x%x\n",Status);
  evt.ConnHandle = pHARC_Inst->pConnInfo->Connection_Handle;
  evt.Status = Status;

  pHARC_Inst->LinkupState = HAP_HARC_LINKUP_COMPLETE;
  evt.EvtOpcode = (HAP_HARC_NotCode_t) HARC_LINKUP_COMPLETE_EVT;
  if (evt.Status == BLE_STATUS_SUCCESS)
  {
    HAP_AttServiceInfo_Evt_t params;
    params.StartAttHandle = pHARC_Inst->HASServiceStartHandle;
    params.EndAttHandle = pHARC_Inst->HASServiceEndHandle;
    evt.pInfo = (void *) &params;
  }
  else
  {
    evt.pInfo = 0;
  }
  HAP_HARC_Notification(&evt);
  if (evt.Status != BLE_STATUS_SUCCESS)
  {
    BLE_DBG_HAP_HARC_MSG("Linkup has failed, reset the HAP Client Instance\n");
    HAP_HARC_InitInstance(pHARC_Inst);
  }
}

/**
 * @brief Check HAS Validity
 * @param  pHARC_Inst: pointer on HAP Client Instance
 * @retval status of the operation
 */
tBleStatus HAP_HARC_Check_HAS_Service(HAP_HARC_Inst_t *pHARC_Inst)
{
  tBleStatus status = BLE_STATUS_SUCCESS;

  /* Check Hearing Aid Features characteristic existence */
  if (pHARC_Inst->HAFeaturesChar.ValueHandle == 0x0000u)
  {
    BLE_DBG_HAP_HARC_MSG("Error : HA Features Chararacteristic is not found in remote GATT Database\n");
    status = BLE_STATUS_FAILED;
  }
  /* Check Hearing Aid Features characteristic properties */
  else if ((pHARC_Inst->HAFeaturesChar.Properties & CHAR_PROP_READ) == 0)
  {
    BLE_DBG_HAP_HARC_MSG("Error : Properties 0x%02X of the HA Features Characteristic is not valid\n",
                pHARC_Inst->HAFeaturesChar.Properties);
    status = BLE_STATUS_FAILED;
  }
  /* Check Hearing Aid Preset Control Point characteristic properties */
  else if (pHARC_Inst->HAPresetControlPointChar.ValueHandle != 0x0000u
           && (((pHARC_Inst->HAPresetControlPointChar.Properties & CHAR_PROP_WRITE) == 0)
             || ((pHARC_Inst->HAPresetControlPointChar.Properties & CHAR_PROP_INDICATE) == 0)))

  {
    BLE_DBG_HAP_HARC_MSG("Error : Properties 0x%02X of the HA Preset Control Point Characteristic is not valid\n",
                pHARC_Inst->HAPresetControlPointChar.Properties);
    status = BLE_STATUS_FAILED;
  }
  else if (pHARC_Inst->ActivePresetIndexChar.ValueHandle != 0x0000u
           && (((pHARC_Inst->ActivePresetIndexChar.Properties & CHAR_PROP_READ) == 0)
               || ((pHARC_Inst->ActivePresetIndexChar.Properties & CHAR_PROP_NOTIFY) == 0)))
  {
    BLE_DBG_HAP_HARC_MSG("Error : Properties 0x%02X of the Active Preset Index Characteristic is not valid\n",
                pHARC_Inst->ActivePresetIndexChar.Properties);
    status = BLE_STATUS_FAILED;
  }
  if (status == BLE_STATUS_SUCCESS)
  {
    BLE_DBG_HAP_HARC_MSG("All HAS Characteristics are valids\n");
  }
  return status;
}

/**
  * @brief Initialize a HAP Client Instance
  * @param pHARC_Inst: pointer on HAP Client Instance
  */
static void HAP_HARC_InitInstance(HAP_HARC_Inst_t *pHARC_Inst)
{
  BLE_DBG_HAP_HARC_MSG("Initialize HAP Client Instance\n");
  /*Initialize the HAP_HARC_Inst_t structure*/
  pHARC_Inst->LinkupState = HAP_HARC_LINKUP_IDLE;
  pHARC_Inst->CurrentLinkupChar = 0;
  pHARC_Inst->ReqHandle = 0x0000;
  pHARC_Inst->pConnInfo = 0x0;
  pHARC_Inst->AttProcStarted = 0u;
  pHARC_Inst->DelayDeallocation = 0u;
  pHARC_Inst->HAFeaturesChar.ValueHandle = 0x0000u;
  pHARC_Inst->HAFeaturesChar.DescHandle = 0x0000u;
  pHARC_Inst->HAFeaturesChar.Properties = 0x00u;
  pHARC_Inst->HAFeaturesChar.EndHandle = 0x0000u;
  pHARC_Inst->HAPresetControlPointChar.ValueHandle = 0x0000u;
  pHARC_Inst->HAPresetControlPointChar.DescHandle = 0x0000u;
  pHARC_Inst->HAPresetControlPointChar.Properties = 0x00u;
  pHARC_Inst->HAPresetControlPointChar.EndHandle = 0x0000u;
  pHARC_Inst->ActivePresetIndexChar.ValueHandle = 0x0000u;
  pHARC_Inst->ActivePresetIndexChar.DescHandle = 0x0000u;
  pHARC_Inst->ActivePresetIndexChar.Properties = 0x00u;
  pHARC_Inst->ActivePresetIndexChar.EndHandle = 0x0000u;
  pHARC_Inst->HASServiceStartHandle = 0x0000;
  pHARC_Inst->HASServiceEndHandle = 0x0000;
  pHARC_Inst->pGattChar = 0;
  pHARC_Inst->ErrorCode = 0x00u;
  pHARC_Inst->HAPFeatures = 0u;
}


static HAP_HARC_Inst_t *HAP_HARC_GetAvailableInstance(void)
{
  uint8_t i;

  /*Get a free HAP Client Instance*/
  for (i = 0 ; i < USECASE_DEV_MGMT_MAX_CONNECTION ; i++)
  {
    if (HAP_Context.HARC.aHARCInst[i].pConnInfo == 0)
    {
      return &HAP_Context.HARC.aHARCInst[i];
    }
  }
  return 0u;
}

static HAP_HARC_Inst_t *HAP_HARC_GetInstance(uint16_t ConnHandle)
{
  uint8_t i;

  /*Get a free HAP Client Instance*/
  for (i = 0 ; i < USECASE_DEV_MGMT_MAX_CONNECTION ; i++)
  {
    if (HAP_Context.HARC.aHARCInst[i].pConnInfo != 0
        && HAP_Context.HARC.aHARCInst[i].pConnInfo->Connection_Handle == ConnHandle)
    {
      return &HAP_Context.HARC.aHARCInst[i];
    }
  }
  return 0u;
}

HAP_HARC_Inst_t *HAP_HARC_GetOtherMember(HAP_HARC_Inst_t *pInst)
{
  for (uint8_t i = 0 ; i< USECASE_DEV_MGMT_MAX_CONNECTION ; i++)
  {
    if (HAP_Context.HARC.aHARCInst[i].pConnInfo != 0)
    {
      if ((memcmp(HAP_Context.HARC.aHARCInst[i].pConnInfo->aSIRK,pInst->pConnInfo->aSIRK,16) == 0 ) \
        && (HAP_Context.HARC.aHARCInst[i].pConnInfo->SIRK_type == pInst->pConnInfo->SIRK_type) \
        && (HAP_Context.HARC.aHARCInst[i].pConnInfo->Connection_Handle != pInst->pConnInfo->Connection_Handle)  \
        && (HAP_Context.HARC.aHARCInst[i].pConnInfo->CSIPDiscovered == 1u )\
        && (HAP_Context.HARC.aHARCInst[i].DelayDeallocation == 0u))
      {
        return &HAP_Context.HARC.aHARCInst[i];
      }
    }
  }
  return 0;
}
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */