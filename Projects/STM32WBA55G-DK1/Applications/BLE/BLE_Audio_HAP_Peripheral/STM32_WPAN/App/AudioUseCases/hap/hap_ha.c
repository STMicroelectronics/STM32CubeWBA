/**
  ******************************************************************************
  * @file    hap_ha.c
  * @author  MCD Application Team
  * @brief   This file contains Hearing Access Profile feature for Hearing Aid
             Role
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
#include "hap_ha.h"
#include "hap.h"
#include "has.h"
#include "hap_alloc.h"
#include "hap_log.h"
#include "cap.h"
#include "ias.h"
#include "ble_vs_codes.h"
#include "hap_db.h"
#include "stm_list.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define ATT_ERR_INVALID_OPCODE                              (0x80u)
#define ATT_ERR_WRITE_NAME_NOT_ALLOWED                      (0x81u)
#define ATT_ERR_PRESET_SYNC_NOT_SUPPORTED                   (0x82u)
#define ATT_ERR_PRESET_OP_NOT_POSSIBLE                      (0x83u)
#define ATT_ERR_INVALID_PARAMETERS_LENGTH                   (0x84u)
#define ATT_ERR_CLIENT_CHAR_CONF_DESC_IMPROPERLY_CONFIGURED (0xFDu)
#define ATT_ERR_VALUE_OUT_OF_RANGE                          (0xFFu)
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions prototype------------------------------------------------*/
tBleStatus HAP_HA_FindPresetWithIndex(uint8_t Index, HAP_Preset_t** pPreset, uint8_t* pPrevIndex);
tBleStatus HAP_HA_FindNextAvailablePreset(HAP_Preset_t* pStartPreset, HAP_Preset_t** pNextPreset, uint8_t decreasing);
/* External functions prototype------------------------------------------------*/

/* Functions Definition ------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/

#if (BLE_CFG_HAP_HA_ROLE == 1u)
/**
  * @brief  Initialize the Hearing Access Profile as Hearing Aid Role
  * @retval status of the initialization
  */
tBleStatus HAP_HA_Init(void)
{
  uint8_t i;
  tBleStatus status;

  HAP_Context.HA.ActivePreset = 0x00;
  HAP_Context.HA.Features = 0x00;

  for (i = 0; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
  {
    HAP_Context.HA.RestoreContext[i].ConnHandle = 0xFFFF;
    HAP_Context.HA.RestoreContext[i].State = HAP_HA_RESTORE_STATE_IDLE;
    HAP_Context.HA.RestoreContext[i].ParseIndex = 0;
    HAP_Context.HA.RestoreContext[i].pPreset = 0;
    HAP_Context.HA.RestoreContext[i].PrevIndex = 0;
  }

  for (i = 0; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
  {
    HAP_Context.HA.CtrlPointDescContext[i].ConnHandle = 0xFFFF;
    HAP_Context.HA.CtrlPointDescContext[i].CtrlPointDescValue = 0;
  }

  status = HAS_InitService(&HAP_Context.HA.HASSvc);
  BLE_DBG_HAP_HA_MSG("HAS Service Init returns status 0x%04X\n", status);

  if (status == BLE_STATUS_SUCCESS)
  {
    status = HAS_InitCharacteristics(&HAP_Context.HA.HASSvc);
    BLE_DBG_HAP_HA_MSG("HAS Characteristics Init returns status 0x%04X\n", status);

    if (status == BLE_STATUS_SUCCESS)
    {
#if (HAP_HA_IAS_SUPPORT == 1u)
      status = IAS_Init();
#endif /* (HAP_HA_IAS_SUPPORT == 1u) */

      if (status == BLE_STATUS_SUCCESS)
      {
        LST_init_head(&HAP_Context.HA.PresetPoolList);
        LST_init_head(&HAP_Context.HA.PresetList);

        for (i = 0; i < HAP_MAX_PRESET_NUM; i++)
        {
          HAP_Context.HA.PresetPool[i].Index = i+1;
          LST_insert_tail(&HAP_Context.HA.PresetPoolList, (tListNode *)(&HAP_Context.HA.PresetPool[i]));
        }
      }
    }
  }

  return status;
}
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */

/**
  * @brief  Build ADV Packet for HAP Hearing Aid Role
  * @param Announcement: Targeted or General Announcement
  * @param pMetadata: pointer on the LTV-formatted Metadata
  * @param MetadataLength: size of the LTV-formatted Metadata
  * @param Appearance: Appearance Value of the device, 0x00 to disable appearance in adv data
  * @param pAdvPacket: buffer provided by upper layer to store ADV information
  * @param AdvPacketLength: size of the buffer pointed by pAdvPacket.
  * @retval length of the built ADV Packet
  */
uint8_t HAP_HA_BuildAdvPacket(CAP_Announcement_t Announcement,
                           uint8_t const *pMetadata,
                           uint8_t MetadataLength,
                           uint16_t Appearance,
                           uint8_t *pAdvPacket,
                           uint8_t AdvPacketLength)
{
  uint8_t built_packet_length = 0;

  built_packet_length = CAP_BuildAdvPacket(Announcement, pMetadata, MetadataLength, pAdvPacket, AdvPacketLength);

#if (BLE_CFG_HAP_HA_ROLE == 1u)
  if ((HAP_Context.Role & HAP_ROLE_HEARING_AID) && AdvPacketLength - built_packet_length >= 4)
  {
    pAdvPacket[built_packet_length++] = 3;
    pAdvPacket[built_packet_length++] = AD_TYPE_SERVICE_DATA;
    pAdvPacket[built_packet_length++] = HEARING_ACCESS_SERVICE_UUID & 0xFF;
    pAdvPacket[built_packet_length++] = (HEARING_ACCESS_SERVICE_UUID >> 8) & 0xFF;
  }
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */

  if (Appearance != 0 && AdvPacketLength - built_packet_length >= 4)
  {
    pAdvPacket[built_packet_length++] = 3;
    pAdvPacket[built_packet_length++] = AD_TYPE_APPEARANCE;
    pAdvPacket[built_packet_length++] = Appearance & 0xFF;
    pAdvPacket[built_packet_length++] = (Appearance >> 8) & 0xFF;

  }

  return built_packet_length;
}

/**
  * @brief Set the Hearing Aids Features value
  * @param Features: The bitfield of Hearing Aids Features supported
  * @return Status of the operation
  */
tBleStatus HAP_HA_SetHearingAidFeatures(HAP_HA_Features_t Features)
{
#if (BLE_CFG_HAP_HA_ROLE == 1u)
  if (HAP_Context.Role & HAP_ROLE_HEARING_AID)
  {
    uint8_t ret;
    ret = HAS_SetHearingAidFeatures(&HAP_Context.HA.HASSvc, 0x0000, Features);
    BLE_DBG_HAP_HA_MSG("Set Hearing Aid Features executed with status 0x%02X\n", ret);

    if (ret == BLE_STATUS_SUCCESS)
    {
      HAP_Context.HA.Features = Features;
    }

    return ret;
  }
  else
  {
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */
    BLE_DBG_HAP_HA_MSG("Set Hearing Aid Features aborted because Hearing Aid Role is not registered\n");
    return HCI_COMMAND_DISALLOWED_ERR_CODE;
#if (BLE_CFG_HAP_HA_ROLE == 1u)
  }
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */
}

/**
  * @brief Add a new preset
  * @param Properties: The properties of the preset
  * @param pName: Pointer to the name of the preset
  * @param NameLen: Length of the name of the preset
  * @param pPresetIndex: Returns the index of the preset added
  * @return Status of the operation
  */
tBleStatus HAP_HA_AddPreset(HAP_Preset_Properties_t Properties, uint8_t* pName, uint8_t NameLen, uint8_t* pPresetIndex)
{
#if (BLE_CFG_HAP_HA_ROLE == 1u)
  if (HAP_Context.Role & HAP_ROLE_HEARING_AID)
  {
    if (NameLen > HAP_MAX_PRESET_NAME_LEN)
    {
      return BLE_STATUS_INVALID_PARAMS;
    }

    if (!LST_is_empty(&HAP_Context.HA.PresetPoolList))
    {
      HAP_Preset_t* p_preset;
      HAP_Preset_t* p_parse_preset;
      tBleStatus ret;
      uint8_t prev_index = 0;

      LST_remove_head(&HAP_Context.HA.PresetPoolList, (tListNode **) &p_preset);

      p_preset->Properties = Properties;
      memcpy(&p_preset->Name[0], pName, NameLen);
      p_preset->NameLen = NameLen;
      *pPresetIndex = p_preset->Index;

      if (LST_is_empty(&HAP_Context.HA.PresetList))
      {
        /* List empty, insert tail */
        LST_insert_tail(&HAP_Context.HA.PresetList, (tListNode *)p_preset);
      }
      else
      {
        p_parse_preset = (HAP_Preset_t *)&HAP_Context.HA.PresetList;
        while(1)
        {
          /* Find insert index */
          LST_get_next_node((tListNode *)p_parse_preset, (tListNode **)&p_parse_preset) ;

          if ((tListNode *)p_parse_preset == &HAP_Context.HA.PresetList
              || p_parse_preset->Index > p_preset->Index)
          {
            LST_insert_node_before((tListNode *)p_preset, (tListNode *)p_parse_preset);
            break;
          }
          prev_index = p_parse_preset->Index;
        }
      }

      BLE_DBG_HAP_HA_MSG("New preset successfully added with index %d\n", p_preset->Index);

      /* Notify clients */
      ret = HAS_SetPresetGenericUpdate(&HAP_Context.HA.HASSvc, 0x0000, 1, prev_index, p_preset);
      BLE_DBG_HAP_HA_MSG("HAS_SetPresetGenericUpdate with status 0x%02X\n", ret);

      return ret;
    }
    else
    {
      BLE_DBG_HAP_HA_MSG("Unable to add new preset: pool is empty\n");
      return BLE_STATUS_INSUFFICIENT_RESOURCES;
    }
  }
  else
  {
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */
    BLE_DBG_HAP_HA_MSG("HAP Add Preset aborted because Hearing Aid Role is not registered\n");
    return HCI_COMMAND_DISALLOWED_ERR_CODE;
#if (BLE_CFG_HAP_HA_ROLE == 1u)
  }
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */
}

/**
  * @brief Remove a preset
  * @param Index: The index of the preset to remove
  * @return Status of the operation
  */
tBleStatus HAP_HA_RemovePreset(uint8_t Index)
{
#if (BLE_CFG_HAP_HA_ROLE == 1u)
  if (HAP_Context.Role & HAP_ROLE_HEARING_AID)
  {
    tBleStatus ret;
    HAP_Preset_t* p_preset;

    ret = HAP_HA_FindPresetWithIndex(Index, &p_preset, 0);

    if (ret == BLE_STATUS_SUCCESS)
    {
      /* Found Preset to remove */
      LST_remove_node((tListNode *)p_preset);
      LST_insert_tail(&HAP_Context.HA.PresetPoolList, (tListNode *)p_preset);

      ret = HAS_SetPresetRecordDeleted(&HAP_Context.HA.HASSvc, 0x0000, 1, Index);
      BLE_DBG_HAP_HA_MSG("HAS_SetPresetRecordDeleted with status 0x%02X\n", ret);
    }

    return ret;
  }
  else
  {
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */
    BLE_DBG_HAP_HA_MSG("HAP Remove Preset aborted because Hearing Aid Role is not registered\n");
    return HCI_COMMAND_DISALLOWED_ERR_CODE;
#if (BLE_CFG_HAP_HA_ROLE == 1u)
  }
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */
}

/**
  * @brief Set the availability value of a preset
  * @param Index: The index of the preset to update
  * @param Available: 1 if preset should be set to available, 0 for unavailable
  * @return Status of the operation
  */
tBleStatus HAP_HA_SetPresetAvailability(uint8_t Index, uint8_t Available)
{
#if (BLE_CFG_HAP_HA_ROLE == 1u)
  if (HAP_Context.Role & HAP_ROLE_HEARING_AID)
  {
    tBleStatus ret;
    HAP_Preset_t* p_preset;
    ret = HAP_HA_FindPresetWithIndex(Index, &p_preset, 0);

    if (ret == BLE_STATUS_SUCCESS)
    {
      /* Found Preset to edit */
      if (Available == 0x01)
      {
        p_preset->Properties |= HAP_PRESET_PROPERTIES_IS_AVAILABLE;
        ret = HAS_SetPresetRecordAvailable(&HAP_Context.HA.HASSvc, 0x0000, 1, p_preset->Index);
        BLE_DBG_HAP_HA_MSG("HAS_SetPresetRecordAvailable with status 0x%02X\n", ret);
      }
      else
      {
        p_preset->Properties &= ~HAP_PRESET_PROPERTIES_IS_AVAILABLE;
        ret = HAS_SetPresetRecordUnavailable(&HAP_Context.HA.HASSvc, 0x0000, 1, p_preset->Index);
        BLE_DBG_HAP_HA_MSG("HAS_SetPresetRecordAvailable with status 0x%02X\n", ret);
      }
    }
    return ret;
  }
  else
  {
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */
    BLE_DBG_HAP_HA_MSG("HAP Set Preset Availability aborted because Hearing Aid Role is not registered\n");
    return HCI_COMMAND_DISALLOWED_ERR_CODE;
#if (BLE_CFG_HAP_HA_ROLE == 1u)
  }
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */
}

/**
  * @brief Edit a preset
  * @param Index: The index of the preset to update
  * @param Properties: The properties to set
  * @param pName: The name to set
  * @param NameLen: The length of the name to set
  * @return Status of the operation
  */
tBleStatus HAP_HA_EditPreset(uint8_t Index, HAP_Preset_Properties_t Properties, uint8_t* pName, uint8_t NameLen)
{
#if (BLE_CFG_HAP_HA_ROLE == 1u)
  if (HAP_Context.Role & HAP_ROLE_HEARING_AID)
  {
    tBleStatus ret;
    uint8_t prev_index;
    HAP_Preset_t* p_preset;

    if (NameLen > HAP_MAX_PRESET_NAME_LEN)
    {
      return BLE_STATUS_INVALID_PARAMS;
    }

    ret = HAP_HA_FindPresetWithIndex(Index, &p_preset, &prev_index);

    if (ret == BLE_STATUS_SUCCESS)
    {
      /* Found Preset to edit */
      p_preset->Properties = Properties;
      memcpy(&p_preset->Name[0], pName, NameLen);
      p_preset->NameLen = NameLen;

      ret = HAS_SetPresetGenericUpdate(&HAP_Context.HA.HASSvc, 0x0000, 1, prev_index, p_preset);
      BLE_DBG_HAP_HA_MSG("HAS_SetPresetGenericUpdate with status 0x%02X\n", ret);
    }
    return ret;
  }
  else
  {
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */
    BLE_DBG_HAP_HA_MSG("HAP Edit Preset aborted because Hearing Aid Role is not registered\n");
    return HCI_COMMAND_DISALLOWED_ERR_CODE;
#if (BLE_CFG_HAP_HA_ROLE == 1u)
  }
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */
}

/**
  * @brief Set the Active Preset Index value
  * @param ActivePresetIndex: The index of the preset to set as active
  * @return Status of the operation
  */
tBleStatus HAP_HA_SetActivePresetIndex(uint8_t ActivePresetIndex)
{
#if (BLE_CFG_HAP_HA_ROLE == 1u)
  if (HAP_Context.Role & HAP_ROLE_HEARING_AID)
  {
    uint8_t ret;
    ret = HAS_SetActivePresetIndex(&HAP_Context.HA.HASSvc, 0x0000, ActivePresetIndex);
    BLE_DBG_HAP_HA_MSG("Set Active Preset Index executed with status 0x%02X\n", ret);

    if (ret == BLE_STATUS_SUCCESS)
    {
      HAP_Context.HA.ActivePreset = ActivePresetIndex;
    }

    return ret;
  }
  else
  {
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */
    BLE_DBG_HAP_HA_MSG("Set Active Preset Index aborted because Hearing Aid Role is not registered\n");
    return HCI_COMMAND_DISALLOWED_ERR_CODE;
#if (BLE_CFG_HAP_HA_ROLE == 1u)
  }
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */
}

#if (BLE_CFG_HAP_HA_ROLE == 1u)
/**
  * @brief  Store GATT Database of the HAP presets
  * @param  ConnHandle: connection handle specifying the remote device
  * @param  pData: pointer on buffer in which GATT Database of presets in which information is stored
  * @param  MaxDataLen: maximum data length to store
  * @param  len : length in bytes of stored data
  * @retval status of the operation
  */
tBleStatus HAP_HA_StoreDatabase(uint16_t ConnHandle, uint8_t *pData,uint16_t MaxDataLen, uint16_t *len)
{
  tBleStatus status = BLE_STATUS_SUCCESS;
  uint16_t data_len = 0u;
  HAP_Preset_t* p_preset;

  if (MaxDataLen >= 2)
  {
    uint8_t i;
    BLE_DBG_HAP_HA_MSG("Store server generic info (conn handle 0x%04X)\n", ConnHandle);

    /* Store server info:
     * - 1 byte for Control Point Descriptor Value
     * - 1 byte for HA Features
     * - 1 byte for Active Preset
     */

    for (i = 0; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
    {
      if (HAP_Context.HA.CtrlPointDescContext[i].ConnHandle == ConnHandle)
      {
        pData[data_len++] = HAP_Context.HA.CtrlPointDescContext[i].CtrlPointDescValue;

        /* Reset Context */
        HAP_Context.HA.CtrlPointDescContext[i].ConnHandle = 0xFFFF;
        break;
      }
    }

    pData[data_len++] = HAP_Context.HA.Features;
    pData[data_len++] = HAP_Context.HA.ActivePreset;


    BLE_DBG_HAP_HA_MSG("Store Presets List in database (conn handle 0x%04X)\n", ConnHandle);

    LST_get_next_node((tListNode *)&HAP_Context.HA.PresetList, (tListNode **)&p_preset) ;

    /* Parse preset list */
    while ((tListNode *)p_preset != &HAP_Context.HA.PresetList)
    {
      if ((data_len + 2 < MaxDataLen)
          && (data_len + 2 + pData[data_len+2] < MaxDataLen))
      {
        /* Store presets:
         * - 1 byte for Preset Index
         * - 1 byte for Preset Properties
         * - 1 byte for Name Length
         * - 'Name Length' bytes for Name
         */
        pData[data_len++] = p_preset->Index;
        pData[data_len++] = p_preset->Properties;
        pData[data_len++] = p_preset->NameLen;
        memcpy(&pData[data_len], p_preset->Name, p_preset->NameLen);
        data_len += p_preset->NameLen;

        LST_get_next_node((tListNode *)p_preset, (tListNode **)&p_preset) ;
      }
      else
      {
        BLE_DBG_HAP_HA_MSG("Out of NVM memory to store HAP Database (conn handle 0x%04X)\n", ConnHandle);
        status = BLE_STATUS_OUT_OF_MEMORY;
        break;
      }
    }
  }
  else
  {
    BLE_DBG_HAP_HA_MSG("Out of NVM memory to store HAP Database (conn handle 0x%04X)\n", ConnHandle);
    status = BLE_STATUS_OUT_OF_MEMORY;
  }

  if (data_len == 0u)
  {
    status = BLE_STATUS_FAILED;
    BLE_DBG_HAP_HA_MSG("No Preset information to store in database\n");
  }
  *len = data_len;
  return status;
}

/**
  * @brief  Check if database has changed since last connection of remote device
  * @param  ConnHandle: connection handle specifying the remote device
  * @param  pData: pointer on buffer of GATT Database
  * @param  Len: length of the GATT database
  * @retval status of the operation
  */
tBleStatus HAP_HA_CheckDatabaseChange(uint16_t ConnHandle, uint8_t *pData, uint16_t Len)
{
  tBleStatus                    status = BLE_STATUS_SUCCESS;
  HAP_HA_Restore_Context_t      *p_restore_context;
  uint16_t                      conn_handle = ConnHandle;
  uint8_t                       channel_index;
  status = HAP_HA_GetRestoreContext(ConnHandle, &p_restore_context);
  if (status != BLE_STATUS_SUCCESS)
  {
    /* Error during Get Restore Context */
    BLE_DBG_HAP_HA_MSG("Could not find related HAP HA Restore Context\n");
    return status;
  }

  if (p_restore_context->State == HAP_HA_RESTORE_STATE_CTRL_POINT_DESC_VALUE)
  {
    uint8_t i;

    for (i = 0; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
    {
      if (HAP_Context.HA.CtrlPointDescContext[i].ConnHandle == ConnHandle)
      {
        HAP_Context.HA.CtrlPointDescContext[i].CtrlPointDescValue = pData[0];
        break;
      }
    }
    p_restore_context->State = HAP_HA_RESTORE_STATE_HA_FEATURES;
  }

  if (p_restore_context->State == HAP_HA_RESTORE_STATE_HA_FEATURES)
  {
    /* Check HA Features */
    if (pData[1] != HAP_Context.HA.Features)
    {
      /* Notify new HA Features */
      BLE_DBG_HAP_HA_MSG("Hearing Aid Features value has changed\n");
      if (BLE_AUDIO_STACK_EATT_GetNumSubscribedBearers(ConnHandle) > 0)
      {
        if (BLE_AUDIO_STACK_EATT_GetAvailableBearer(ConnHandle,&channel_index) == BLE_STATUS_SUCCESS)
        {
          conn_handle = (0xEA << 8) | (channel_index);
        }
        else
        {
          return BLE_STATUS_INSUFFICIENT_RESOURCES;
        }
      }
      status = HAS_SetHearingAidFeatures(&HAP_Context.HA.HASSvc, conn_handle, HAP_Context.HA.Features);

      if ((status == BLE_STATUS_INSUFFICIENT_RESOURCES) \
                || (status == BLE_STATUS_BUSY) \
                || (status == BLE_STATUS_SEC_PERMISSION_ERROR) \
                || (status == HCI_COMMAND_DISALLOWED_ERR_CODE))
      {
        /* send ATT notification to the remote device has failed because no ATT packet resource is available
         * or link not already encrypted
         */
        return status;
      }
    }
    p_restore_context->State = HAP_HA_RESTORE_STATE_ACTIVE_PRESET_INDEX;
  }


  if (p_restore_context->State == HAP_HA_RESTORE_STATE_ACTIVE_PRESET_INDEX)
  {
    /* Check Active Preset */
    if (pData[2] != HAP_Context.HA.ActivePreset)
    {
      /* Notify new Active Preset */
      BLE_DBG_HAP_HA_MSG("Active Preset Index value has changed since last connection with ConnHandle 0x%04X\n",
                         ConnHandle);

      if (BLE_AUDIO_STACK_EATT_GetNumSubscribedBearers(ConnHandle) > 0)
      {
        if (BLE_AUDIO_STACK_EATT_GetAvailableBearer(ConnHandle,&channel_index) == BLE_STATUS_SUCCESS)
        {
          conn_handle = (0xEA << 8) | (channel_index);
        }
        else
        {
          return BLE_STATUS_INSUFFICIENT_RESOURCES;
        }
      }
      status = HAS_SetActivePresetIndex(&HAP_Context.HA.HASSvc, conn_handle, HAP_Context.HA.ActivePreset);

      if ((status == BLE_STATUS_INSUFFICIENT_RESOURCES) \
                || (status == BLE_STATUS_BUSY) \
                || (status == BLE_STATUS_SEC_PERMISSION_ERROR) \
                || (status == HCI_COMMAND_DISALLOWED_ERR_CODE))
      {
        /* send ATT notification to the remote device has failed because no ATT packet resource is available
         * or link not already encrypted
         */
        return status;
      }
    }
    p_restore_context->State = HAP_HA_RESTORE_STATE_PRESET_LIST;
    LST_get_next_node((tListNode *)&HAP_Context.HA.PresetList, (tListNode **)&p_restore_context->pPreset) ;
    p_restore_context->ParseIndex = 3;
    p_restore_context->PrevIndex = 0;
  }

  if (p_restore_context->State == HAP_HA_RESTORE_STATE_PRESET_LIST)
  {
    uint8_t preset_to_notify = 0u;
    uint8_t isLast = 1u;
    uint8_t preset_deleted = 0u;
    HAP_Preset_t *p_preset = 0;
    uint8_t index;
    uint8_t prev_index = 0;
    while((tListNode *) p_restore_context->pPreset != &HAP_Context.HA.PresetList
          || p_restore_context->ParseIndex < Len)
    {
      if (p_restore_context->ParseIndex >= Len
          || pData[p_restore_context->ParseIndex] > p_restore_context->pPreset->Index)
      {
        if (preset_to_notify == 0u)
        {
          /* Notify new preset */
          BLE_DBG_HAP_HA_MSG("Preset index %d has been added since last connection with ConnHandle 0x%04X\n",
                             p_restore_context->pPreset->Index, ConnHandle);
          preset_to_notify = 1u;
          p_preset = p_restore_context->pPreset;
          prev_index = p_restore_context->PrevIndex;

          p_restore_context->PrevIndex = p_restore_context->pPreset->Index;
          LST_get_next_node((tListNode *)p_restore_context->pPreset, (tListNode **)&p_restore_context->pPreset) ;
        }
        else
        {
          /* Found next preset to notify */
          isLast = 0u;
          break;
        }
      }

      else if ((tListNode *) p_restore_context->pPreset == &HAP_Context.HA.PresetList
               || pData[p_restore_context->ParseIndex] < p_restore_context->pPreset->Index)
      {
        if (preset_to_notify == 0u)
        {
          /* Notify preset deleted */
          BLE_DBG_HAP_HA_MSG("Preset index %d has been deleted since last connection with ConnHandle 0x%04X\n",
                             pData[p_restore_context->ParseIndex], ConnHandle);
          preset_to_notify = 1u;
          preset_deleted = 1u;
          index = pData[p_restore_context->ParseIndex];

          p_restore_context->ParseIndex += pData[p_restore_context->ParseIndex+2] + 3;
        }
        else
        {
          /* Found next preset to notify */
          isLast = 0u;
          break;
        }
      }

      else if (pData[p_restore_context->ParseIndex] == p_restore_context->pPreset->Index)
      {
        /* Compare properties and Name */
        if ((pData[p_restore_context->ParseIndex+1] != p_restore_context->pPreset->Properties)
            || (pData[p_restore_context->ParseIndex+2] != p_restore_context->pPreset->NameLen)
              || ((memcmp(&pData[p_restore_context->ParseIndex+3],
                          &p_restore_context->pPreset->Name[0],
                          p_restore_context->pPreset->NameLen)) != 0))
        {
          if (preset_to_notify == 0u)
          {
            /* Notify preset update */
            BLE_DBG_HAP_HA_MSG("Preset index %d has been updated since last connection with ConnHandle 0x%04X\n",
                               p_restore_context->pPreset->Index, ConnHandle);
            preset_to_notify = 1u;
            p_preset = p_restore_context->pPreset;
            prev_index = p_restore_context->PrevIndex;
          }
          else
          {
            /* Found next preset to notify */
            isLast = 0u;
            break;
          }
        }

        p_restore_context->PrevIndex = p_restore_context->pPreset->Index;
        LST_get_next_node((tListNode *)p_restore_context->pPreset, (tListNode **)&p_restore_context->pPreset) ;
        p_restore_context->ParseIndex += pData[p_restore_context->ParseIndex+2] + 3;
      }
    }

    if (preset_to_notify == 1u)
    {
      if (BLE_AUDIO_STACK_EATT_GetNumSubscribedBearers(ConnHandle) > 0)
      {
        if (BLE_AUDIO_STACK_EATT_GetAvailableBearer(ConnHandle,&channel_index) == BLE_STATUS_SUCCESS)
        {
          conn_handle = (0xEA << 8) | (channel_index);
        }
        else
        {
          return BLE_STATUS_INSUFFICIENT_RESOURCES;
        }
      }
      if (preset_deleted == 0u)
      {
        status = HAS_SetPresetGenericUpdate(&HAP_Context.HA.HASSvc,
                                            conn_handle,
                                            isLast,
                                            prev_index,
                                            p_preset);
        BLE_DBG_HAP_HA_MSG("HAS_SetPresetGenericUpdate on preset index %d with status 0x%02X\n",
                           p_preset->Index,
                           status);
      }
      else
      {
        status = HAS_SetPresetRecordDeleted(&HAP_Context.HA.HASSvc, conn_handle, isLast, index);
        BLE_DBG_HAP_HA_MSG("HAS_SetPresetRecordDeleted on present index %d with status 0x%02X\n",
                           index,
                           status);
      }
    }

    if (isLast == 1u)
    {
      p_restore_context->State = HAP_HA_RESTORE_STATE_IDLE;
      p_restore_context->ConnHandle = 0xFFFF;
      p_restore_context->ParseIndex = 0;
      p_restore_context->pPreset = 0;
      p_restore_context->PrevIndex = 0;
    }

  }

  return status;
}

/**
  * @brief Returns pointer to restore context structure related to ConnHandle
  * @param ConnHandle: ACL Connection Handle
  * @param pRestoreContext: Output pointer to a Restore Structure
  * @retval Status of the operation
  */
tBleStatus HAP_HA_GetRestoreContext(uint16_t ConnHandle, HAP_HA_Restore_Context_t **pRestoreContext)
{
  uint8_t i;

  for (i = 0; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
  {
    if (HAP_Context.HA.RestoreContext[i].ConnHandle == ConnHandle)
    {
      *pRestoreContext = &HAP_Context.HA.RestoreContext[i];
      return BLE_STATUS_SUCCESS;
    }
  }
  return BLE_STATUS_FAILED;
}

/**
 * @brief  Notify Hearing Access Service Event
 * @param  pNotification: pointer on notification information
 */
void HAS_Notification(HAS_NotificationEvt_t const *pNotification)
{
  UseCaseConnInfo_t     *p_conn_info;
  BleEATTBearer_t       *p_eatt_bearer;

  if (USECASE_DEV_MGMT_GetConnInfo(pNotification->ConnHandle, &p_conn_info,&p_eatt_bearer) == BLE_STATUS_SUCCESS)
  {
    uint16_t    conn_handle = p_conn_info->Connection_Handle;
    uint8_t     channel_index;
    switch (pNotification->EvtOpcode)
    {
      case HAS_CONTROL_POINT_EVT:
      {
        HAP_HA_Control_Point_Op_t CtrlOp = pNotification->pData[0];

        switch (CtrlOp)
        {
          case HAP_HA_CONTROL_POINT_OP_READ_PRESETS_REQUEST:
          {
            tBleStatus ret;

            if (!LST_is_empty(&HAP_Context.HA.PresetList))
            {
              HAP_Preset_t* p_preset;
              LST_get_next_node((tListNode *)&HAP_Context.HA.PresetList, (tListNode **)&p_preset) ;

              /* Parse preset list */
              while ((tListNode *)p_preset != &HAP_Context.HA.PresetList)
              {
                if (p_preset->Index >= pNotification->pData[1])
                {
                  break;
                }
                LST_get_next_node((tListNode *)p_preset, (tListNode **)&p_preset) ;
              }

              if ((tListNode *)p_preset != &HAP_Context.HA.PresetList)
              {
                uint8_t is_last = 0;

                if ((((tListNode *)p_preset)->next == &HAP_Context.HA.PresetList)
                       || (pNotification->pData[2] == 1))
                {
                  is_last = 1;
                }
                if (BLE_AUDIO_STACK_EATT_GetNumSubscribedBearers(p_conn_info->Connection_Handle) > 0)
                {
                  if (BLE_AUDIO_STACK_EATT_GetAvailableBearer(p_conn_info->Connection_Handle,
                                                              &channel_index) == BLE_STATUS_SUCCESS)
                  {
                    conn_handle = (0xEA << 8) | (channel_index);
                  }
                  else
                  {
                    return;
                  }
                }
                ret = HAS_SetReadPresetsResponse(&HAP_Context.HA.HASSvc, conn_handle, p_preset, is_last);
                BLE_DBG_HAP_HA_MSG("Set Read Presets Response executed with status 0x%02X\n", ret);

                if (ret == BLE_STATUS_SUCCESS)
                {
                  HAP_Context.HA.CtrlPointOp.CurrentOp = HAP_HA_CONTROL_POINT_OP_READ_PRESETS_RESPONSE;
                  HAP_Context.HA.CtrlPointOp.pCurrentNode = (tListNode*) p_preset;
                  HAP_Context.HA.CtrlPointOp.RemainingNumRecord = pNotification->pData[2] - 1;
                }
              }
            }
            break;
          }

          case HAP_HA_CONTROL_POINT_OP_WRITE_PRESET_NAME:
          {
            tBleStatus ret;
            HAP_Preset_t* p_preset;
            uint8_t prev_index;

            ret = HAP_HA_FindPresetWithIndex(pNotification->pData[1], &p_preset, &prev_index);

            if (ret == BLE_STATUS_SUCCESS)
            {
              p_preset->NameLen = pNotification->DataLen - 2;
              memcpy(&p_preset->Name[0], &pNotification->pData[2], pNotification->DataLen - 2);
              ret = HAS_SetPresetGenericUpdate(&HAP_Context.HA.HASSvc, 0x0000, 1, prev_index, p_preset);
              BLE_DBG_HAP_HA_MSG("Set Preset Generic Update executed with status 0x%02X\n", ret);

              if (ret == BLE_STATUS_SUCCESS)
              {
                HAP_Notification_Evt_t notif;

                notif.ConnHandle = p_conn_info->Connection_Handle;
                notif.EvtOpcode = HAP_HA_PRESET_NAME_UPDATE_EVT;
                notif.pInfo = (uint8_t*) p_preset;
                notif.Status = BLE_STATUS_SUCCESS;
                HAP_Notification(&notif);
              }
            }

            break;
          }

          case HAP_HA_CONTROL_POINT_OP_SET_ACTIVE_PRESET:
          case HAP_HA_CONTROL_POINT_OP_SET_ACTIVE_PRESET_LOCAL_SYNC:
          {
            tBleStatus ret;
            HAP_Preset_t* p_preset;
            ret = HAP_HA_FindPresetWithIndex(pNotification->pData[1], &p_preset, 0);

            if (ret == BLE_STATUS_SUCCESS)
            {
              ret = HAS_SetActivePresetIndex(&HAP_Context.HA.HASSvc, 0x0000, pNotification->pData[1]);
              BLE_DBG_HAP_HA_MSG("Set Active Preset index %d executed with status 0x%02X\n",
                                 pNotification->pData[1],
                                 ret);
              if (ret == BLE_STATUS_SUCCESS)
              {
                HAP_Notification_Evt_t notif;
                HAP_ActivePreset_Info_t info;
                info.pPreset = p_preset;
                if (pNotification->EvtOpcode == HAP_HA_CONTROL_POINT_OP_SET_PREVIOUS_PRESET_LOCAL_SYNC)
                {
                  info.SyncLocally = 1;
                }
                else
                {
                  info.SyncLocally = 0;
                }

                notif.ConnHandle = p_conn_info->Connection_Handle;
                notif.EvtOpcode = HAP_HA_ACTIVE_PRESET_EVT;
                notif.pInfo = (uint8_t*) &info;
                notif.Status = BLE_STATUS_SUCCESS;
                HAP_Notification(&notif);

                HAP_Context.HA.ActivePreset = p_preset->Index;
              }
            }

            break;
          }

          case HAP_HA_CONTROL_POINT_OP_SET_NEXT_PRESET:
          case HAP_HA_CONTROL_POINT_OP_SET_NEXT_PRESET_LOCAL_SYNC:
          {
            tBleStatus ret;
            HAP_Preset_t* p_preset;
            ret = HAP_HA_FindPresetWithIndex(HAP_Context.HA.ActivePreset, &p_preset, 0);

            if (ret == BLE_STATUS_SUCCESS)
            {
              ret = HAP_HA_FindNextAvailablePreset(p_preset, &p_preset, 0);

              if (ret == BLE_STATUS_SUCCESS)
              {
                ret = HAS_SetActivePresetIndex(&HAP_Context.HA.HASSvc, 0x0000, p_preset->Index);
                BLE_DBG_HAP_HA_MSG("Set Active Preset index %d executed with status 0x%02X\n", p_preset->Index, ret);

                if (ret == BLE_STATUS_SUCCESS)
                {
                  HAP_Notification_Evt_t notif;
                  HAP_ActivePreset_Info_t info;
                  info.pPreset = p_preset;
                  if (pNotification->EvtOpcode == HAP_HA_CONTROL_POINT_OP_SET_NEXT_PRESET_LOCAL_SYNC)
                  {
                    info.SyncLocally = 1;
                  }
                  else
                  {
                    info.SyncLocally = 0;
                  }

                  notif.ConnHandle = p_conn_info->Connection_Handle;
                  notif.EvtOpcode = HAP_HA_ACTIVE_PRESET_EVT;
                  notif.pInfo = (uint8_t*) &info;
                  notif.Status = BLE_STATUS_SUCCESS;
                  HAP_Notification(&notif);

                  HAP_Context.HA.ActivePreset = p_preset->Index;
                }
              }
            }
            break;
          }

          case HAP_HA_CONTROL_POINT_OP_SET_PREVIOUS_PRESET:
          case HAP_HA_CONTROL_POINT_OP_SET_PREVIOUS_PRESET_LOCAL_SYNC:
          {
            tBleStatus ret;
            HAP_Preset_t* p_preset;
            ret = HAP_HA_FindPresetWithIndex(HAP_Context.HA.ActivePreset, &p_preset, 0);

            if (ret == BLE_STATUS_SUCCESS)
            {
              ret = HAP_HA_FindNextAvailablePreset(p_preset, &p_preset, 1);

              if (ret == BLE_STATUS_SUCCESS)
              {
                ret = HAS_SetActivePresetIndex(&HAP_Context.HA.HASSvc, 0x0000, p_preset->Index);
                BLE_DBG_HAP_HA_MSG("Set Active Preset index %d executed with status 0x%02X\n", p_preset->Index, ret);

                if (ret == BLE_STATUS_SUCCESS)
                {
                  HAP_Notification_Evt_t notif;
                  HAP_ActivePreset_Info_t info;
                  info.pPreset = p_preset;
                  if (pNotification->EvtOpcode == HAP_HA_CONTROL_POINT_OP_SET_PREVIOUS_PRESET_LOCAL_SYNC)
                  {
                    info.SyncLocally = 1;
                  }
                  else
                  {
                    info.SyncLocally = 0;
                  }

                  notif.ConnHandle = p_conn_info->Connection_Handle;
                  notif.EvtOpcode = HAP_HA_ACTIVE_PRESET_EVT;
                  notif.pInfo = (uint8_t*) &info;
                  notif.Status = BLE_STATUS_SUCCESS;
                  HAP_Notification(&notif);

                  HAP_Context.HA.ActivePreset = p_preset->Index;
                }
              }
            }
            break;
          }

          default:
          {
            /* Invalid Opcode, Should not happen */
            break;
          }
        }

        break;
      }

      case HAS_CONTROL_POINT_DESC_EVT:
      {
        uint8_t i;

        for (i = 0; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
        {
          if (HAP_Context.HA.CtrlPointDescContext[i].ConnHandle == p_conn_info->Connection_Handle)
          {
            HAP_Context.HA.CtrlPointDescContext[i].CtrlPointDescValue = pNotification->pData[0];
          }
        }
        break;
      }

      default:
        break;
    }
  }
}

uint8_t HAS_CheckCtrlOpParams(uint16_t ConnHandle, uint8_t *pData,uint8_t DataLen)
{
  uint8_t               err_code = 0x00;
  UseCaseConnInfo_t     *p_conn_info;
  BleEATTBearer_t       *p_eatt_bearer;

  if (USECASE_DEV_MGMT_GetConnInfo(ConnHandle, &p_conn_info,&p_eatt_bearer) == BLE_STATUS_SUCCESS)
  {
    if (DataLen > 0u)
    {
      switch (pData[0])
      {
        case HAP_HA_CONTROL_POINT_OP_READ_PRESETS_REQUEST:
        {
          uint8_t i;

          /* Check if descriptor is configured for indication */
          for (i = 0; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
          {
            if (HAP_Context.HA.CtrlPointDescContext[i].ConnHandle == p_conn_info->Connection_Handle)
            {
              if (HAP_Context.HA.CtrlPointDescContext[i].CtrlPointDescValue == 0)
              {
                BLE_DBG_HAP_HA_MSG("Descriptor isn't configured for indication\n");
                err_code = ATT_ERR_CLIENT_CHAR_CONF_DESC_IMPROPERLY_CONFIGURED;
              }
              break;
            }
          }

          if (err_code == 0x00)
          {
            /* Check if DataLen is valid */
            if (DataLen != 3)
            {
              BLE_DBG_HAP_HA_MSG("Read Presets Request with invalid parameters length\n");
              err_code = ATT_ERR_INVALID_PARAMETERS_LENGTH;
            }
            else if ((pData[1] == 0)
                  || (pData[2] == 0)
                    || (LST_is_empty(&HAP_Context.HA.PresetList))
                      || (((HAP_Preset_t*)HAP_Context.HA.PresetList.prev)->Index < pData[1]))
            {
              BLE_DBG_HAP_HA_MSG("Read Presets Request parameters out of range\n");
              err_code = ATT_ERR_VALUE_OUT_OF_RANGE;
            }
          }
          break;
        }

        case HAP_HA_CONTROL_POINT_OP_WRITE_PRESET_NAME:
        {
          uint8_t i;

          /* Check if descriptor is configured for indication */
          for (i = 0; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
          {
            if (HAP_Context.HA.CtrlPointDescContext[i].ConnHandle == p_conn_info->Connection_Handle)
            {
              if ((HAP_Context.HA.CtrlPointDescContext[i].CtrlPointDescValue & GATT_CHAR_UPDATE_SEND_INDICATION) == 0)
              {
                BLE_DBG_HAP_HA_MSG("Descriptor isn't configured for indication\n");
                err_code = ATT_ERR_CLIENT_CHAR_CONF_DESC_IMPROPERLY_CONFIGURED;
              }
              break;
            }
          }

          if (err_code == 0x00)
          {
            if (DataLen < 3 || DataLen > 42)
            {
              BLE_DBG_HAP_HA_MSG("Write Preset Name with invalid parameters length\n");
              err_code = ATT_ERR_INVALID_PARAMETERS_LENGTH;
            }
            else if ((HAP_Context.HA.Features & HAP_WRITABLE_PRESETS_SUPPORTED) == 0)
            {
              BLE_DBG_HAP_HA_MSG("Writable Presets records are not supported\n");
              err_code = ATT_ERR_WRITE_NAME_NOT_ALLOWED;
            }
            else
            {
              tBleStatus ret;
              HAP_Preset_t* p_preset;
              ret = HAP_HA_FindPresetWithIndex(pData[1], &p_preset, 0);

              if (ret == BLE_STATUS_SUCCESS)
              {
                if ((p_preset->Properties & HAP_PRESET_PROPERTIES_WRITABLE) == 0)
                {
                  BLE_DBG_HAP_HA_MSG("Write Preset Name on preset index %d not allowed\n", pData[1]);
                  err_code = ATT_ERR_WRITE_NAME_NOT_ALLOWED;
                }
              }
              else
              {
                BLE_DBG_HAP_HA_MSG("Write Preset Name index %d out of range\n", pData[1]);
                err_code = ATT_ERR_VALUE_OUT_OF_RANGE;
              }
            }
          }

          break;
        }

        case HAP_HA_CONTROL_POINT_OP_SET_ACTIVE_PRESET:
        case HAP_HA_CONTROL_POINT_OP_SET_ACTIVE_PRESET_LOCAL_SYNC:
        {
          if (err_code == 0x00)
          {
            if (DataLen != 2)
            {
              BLE_DBG_HAP_HA_MSG("Set Active Preset with invalid parameters length\n");
              err_code = ATT_ERR_INVALID_PARAMETERS_LENGTH;
            }
            else if ((pData[0] == HAP_HA_CONTROL_POINT_OP_SET_ACTIVE_PRESET_LOCAL_SYNC)
                     && ((HAP_Context.HA.Features & HAP_PRESET_SYNC_SUPPORTED) == 0))
            {
              BLE_DBG_HAP_HA_MSG("Preset sync is not supported\n");
              err_code = ATT_ERR_PRESET_SYNC_NOT_SUPPORTED;
            }
            else
            {
              tBleStatus ret;
              HAP_Preset_t* p_preset;
              ret = HAP_HA_FindPresetWithIndex(pData[1], &p_preset, 0);

              if (ret == BLE_STATUS_SUCCESS)
              {
                if (p_preset->Properties & HAP_PRESET_PROPERTIES_IS_AVAILABLE)
                {
                  HAP_Notification_Evt_t notif;
                  HAP_ActivePresetChangeReq_Info_t info;
                  HAP_SetActivePreset_Resp_t resp = 0;

                  info.pPreset = p_preset;
                  info.pResp = &resp;

                  notif.ConnHandle = p_conn_info->Connection_Handle;
                  notif.EvtOpcode = HAP_HA_SET_PRESET_REQ_EVT;
                  notif.pInfo = (uint8_t*) &info;
                  notif.Status = BLE_STATUS_SUCCESS;
                  HAP_Notification(&notif);

                  if(resp != 0)
                  {
                    BLE_DBG_HAP_HA_MSG("Set Active Preset with index %d denied\n", pData[1]);
                    err_code = ATT_ERR_PRESET_OP_NOT_POSSIBLE;
                  }
                }
                else
                {
                  BLE_DBG_HAP_HA_MSG("Preset with index %d is not available\n", pData[1]);
                  err_code = ATT_ERR_PRESET_OP_NOT_POSSIBLE;
                }
              }
              else
              {
                BLE_DBG_HAP_HA_MSG("Set Active Preset with index %d out of range\n", pData[1]);
                err_code = ATT_ERR_VALUE_OUT_OF_RANGE;
              }
            }
          }
          break;
        }

        case HAP_HA_CONTROL_POINT_OP_SET_NEXT_PRESET:
        case HAP_HA_CONTROL_POINT_OP_SET_NEXT_PRESET_LOCAL_SYNC:
        {
          if (DataLen != 1)
          {
            BLE_DBG_HAP_HA_MSG("Set Next Preset with invalid parameters length\n");
            err_code = ATT_ERR_INVALID_PARAMETERS_LENGTH;
          }
          else if ((pData[0] == HAP_HA_CONTROL_POINT_OP_SET_NEXT_PRESET_LOCAL_SYNC)
                   && ((HAP_Context.HA.Features & HAP_PRESET_SYNC_SUPPORTED) == 0))
          {
            BLE_DBG_HAP_HA_MSG("Preset sync is not supported\n");
            err_code = ATT_ERR_PRESET_SYNC_NOT_SUPPORTED;
          }
          else
          {
            tBleStatus ret;
            HAP_Preset_t* p_preset;
            ret = HAP_HA_FindPresetWithIndex(HAP_Context.HA.ActivePreset, &p_preset, 0);

            if (ret == BLE_STATUS_SUCCESS)
            {
              ret = HAP_HA_FindNextAvailablePreset(p_preset, &p_preset, 0);

              if (ret == BLE_STATUS_SUCCESS)
              {
                HAP_Notification_Evt_t notif;
                HAP_ActivePresetChangeReq_Info_t info;
                HAP_SetActivePreset_Resp_t resp = 0;

                info.pPreset = p_preset;
                info.pResp = &resp;

                notif.ConnHandle = p_conn_info->Connection_Handle;
                notif.EvtOpcode = HAP_HA_SET_PRESET_REQ_EVT;
                notif.pInfo = (uint8_t*) &info;
                notif.Status = BLE_STATUS_SUCCESS;
                HAP_Notification(&notif);

                if(resp != 0)
                {
                  BLE_DBG_HAP_HA_MSG("Set Next Preset with index %d denied\n", p_preset->Index);
                  err_code = ATT_ERR_PRESET_OP_NOT_POSSIBLE;
                }
              }
              else
              {
                BLE_DBG_HAP_HA_MSG("No other available preset\n", pData[1]);
                err_code = ATT_ERR_PRESET_OP_NOT_POSSIBLE;
              }
            }
          }

          break;
        }

        case HAP_HA_CONTROL_POINT_OP_SET_PREVIOUS_PRESET:
        case HAP_HA_CONTROL_POINT_OP_SET_PREVIOUS_PRESET_LOCAL_SYNC:
        {
          if (DataLen != 1)
          {
            BLE_DBG_HAP_HA_MSG("Set Previous Preset with invalid parameters length\n");
            err_code = ATT_ERR_INVALID_PARAMETERS_LENGTH;
          }
          else if ((pData[0] == HAP_HA_CONTROL_POINT_OP_SET_PREVIOUS_PRESET_LOCAL_SYNC)
                   && ((HAP_Context.HA.Features & HAP_PRESET_SYNC_SUPPORTED) == 0))
          {
            BLE_DBG_HAP_HA_MSG("Preset sync is not supported\n");
            err_code = ATT_ERR_PRESET_SYNC_NOT_SUPPORTED;
          }
          else
          {
            tBleStatus ret;
            HAP_Preset_t* p_preset;
            ret = HAP_HA_FindPresetWithIndex(HAP_Context.HA.ActivePreset, &p_preset, 0);

            if (ret == BLE_STATUS_SUCCESS)
            {
              ret = HAP_HA_FindNextAvailablePreset(p_preset, &p_preset, 1);

              if (ret == BLE_STATUS_SUCCESS)
              {
                HAP_Notification_Evt_t notif;
                HAP_ActivePresetChangeReq_Info_t info;
                HAP_SetActivePreset_Resp_t resp = 0;

                info.pPreset = p_preset;
                info.pResp = &resp;

                notif.ConnHandle = p_conn_info->Connection_Handle;
                notif.EvtOpcode = HAP_HA_SET_PRESET_REQ_EVT;
                notif.pInfo = (uint8_t*) &info;
                notif.Status = BLE_STATUS_SUCCESS;
                HAP_Notification(&notif);

                if(resp != 0)
                {
                  BLE_DBG_HAP_HA_MSG("Set Next Preset with index %d denied\n", p_preset->Index);
                  err_code = ATT_ERR_PRESET_OP_NOT_POSSIBLE;
                }
              }
              else
              {
                BLE_DBG_HAP_HA_MSG("No other available preset\n", pData[1]);
                err_code = ATT_ERR_PRESET_OP_NOT_POSSIBLE;
              }
            }
          }
          break;
        }

        default:
        {
          BLE_DBG_HAP_HA_MSG("Hearing Aid Preset Ctrl Operation 0x%02X is invalid\n", pData[0]);
          err_code = ATT_ERR_INVALID_OPCODE;
          break;
        }
      }
    }
  }
  else
  {
    err_code = ATT_ERR_INVALID_OPCODE;
  }

  return err_code;
}

SVCCTL_EvtAckStatus_t HAS_HandleCtrlPointIndicateComplete(uint16_t ConnHandle)
{

  UseCaseConnInfo_t     *p_conn_info;
  BleEATTBearer_t       *p_eatt_bearer;

  if (USECASE_DEV_MGMT_GetConnInfo(ConnHandle, &p_conn_info,&p_eatt_bearer) == BLE_STATUS_SUCCESS)
  {
    uint16_t    conn_handle = p_conn_info->Connection_Handle;
    uint8_t     channel_index;
    if (HAP_Context.HA.CtrlPointOp.CurrentOp == HAP_HA_CONTROL_POINT_OP_READ_PRESETS_RESPONSE)
    {
      HAP_Preset_t *p_preset = (HAP_Preset_t *) HAP_Context.HA.CtrlPointOp.pCurrentNode;
      LST_get_next_node((tListNode *)p_preset, (tListNode **)&p_preset) ;

      if ((HAP_Context.HA.CtrlPointOp.RemainingNumRecord == 0) \
          || ((tListNode *) p_preset == &HAP_Context.HA.PresetList))
      {
        /* End of procedure */
        HAP_Context.HA.CtrlPointOp.CurrentOp = 0;
      }
      else
      {
        uint8_t is_last = 0;
        tBleStatus ret;

        HAP_Context.HA.CtrlPointOp.RemainingNumRecord --;

        if ((HAP_Context.HA.CtrlPointOp.RemainingNumRecord == 0) \
            || (((tListNode *) p_preset)->next == &HAP_Context.HA.PresetList))
        {
          is_last = 1;
        }
        if (BLE_AUDIO_STACK_EATT_GetNumSubscribedBearers(p_conn_info->Connection_Handle) > 0)
        {
          if (BLE_AUDIO_STACK_EATT_GetAvailableBearer(p_conn_info->Connection_Handle,
                                                      &channel_index) == BLE_STATUS_SUCCESS)
          {
            conn_handle = (0xEA << 8) | (channel_index);
          }
          else
          {
            BLE_DBG_HAP_HA_MSG("Set Read Presets Response executed aborted because no EATT Bearer is available\n");
            HAP_Context.HA.CtrlPointOp.CurrentOp = 0;
            return SVCCTL_EvtAckFlowEnable;
          }
        }
        ret = HAS_SetReadPresetsResponse(&HAP_Context.HA.HASSvc, conn_handle, p_preset, is_last);
        BLE_DBG_HAP_HA_MSG("Set Read Presets Response executed with status 0x%02X\n", ret);

        if (ret == BLE_STATUS_SUCCESS)
        {
          HAP_Context.HA.CtrlPointOp.pCurrentNode = (tListNode*) p_preset;
        }
        else
        {
          HAP_Context.HA.CtrlPointOp.CurrentOp = 0;
        }
      }
      return SVCCTL_EvtAckFlowEnable;
    }
  }
  return SVCCTL_EvtNotAck;
}

/**
  * @brief  Notify Link Encrypted to HAP HA
  * @param  ConnHandle: ACL Connection Handle
  */
void HAP_HA_LinkEncrypted(uint16_t ConnHandle)
{
  uint8_t               i;
  UseCaseConnInfo_t     *p_conn_info;
  BleEATTBearer_t       *p_eatt_bearer;

  /* Allocate Ctrl Point Descriptor Context */
  for (i = 0; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
  {
    if (HAP_Context.HA.CtrlPointDescContext[i].ConnHandle == 0xFFFF)
    {
      HAP_Context.HA.CtrlPointDescContext[i].ConnHandle = ConnHandle;
      break;
    }
  }

  if ((USECASE_DEV_MGMT_GetConnInfo(ConnHandle, &p_conn_info,&p_eatt_bearer) == BLE_STATUS_SUCCESS) \
      && (p_conn_info->MTU >= 49u) \
      && (p_conn_info->LinkEncrypted == 1u))
  {
    for (i = 0; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
    {
      if (HAP_Context.HA.RestoreContext[i].ConnHandle == 0xFFFF)
      {
        HAP_Context.HA.RestoreContext[i].ConnHandle = p_conn_info->Connection_Handle;
        HAP_Context.HA.RestoreContext[i].State = HAP_HA_RESTORE_STATE_CTRL_POINT_DESC_VALUE;
        HAP_DB_CheckDatabaseChange(p_conn_info);
        break;
      }
    }
  }
}

/** @brief This function is used by the Device in the HAP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */
SVCCTL_EvtAckStatus_t HAP_HA_GATT_Event_Handler(void *pEvent)
{
  SVCCTL_EvtAckStatus_t return_value;

  hci_event_pckt        *p_event_pckt;
  evt_blecore_aci       *p_blecore_evt;
  BleEATTBearer_t       *p_eatt_bearer;

  p_event_pckt = (hci_event_pckt *)(((hci_uart_pckt*)pEvent)->data);

  switch (p_event_pckt->evt)
  {
    case HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE:
    {
      p_blecore_evt = (evt_blecore_aci*)p_event_pckt->data;
      BLE_DBG_HAP_HA_MSG("GATT Event in HAP HA Layer : %04X with code %04X!!\n",p_event_pckt->evt,p_blecore_evt->ecode);
      switch (p_blecore_evt->ecode)
      {
        case ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE:
        {
          aci_att_exchange_mtu_resp_event_rp0 *mtu_resp_event = (void*)p_blecore_evt->data;
          UseCaseConnInfo_t *p_conn_info;
          if ((USECASE_DEV_MGMT_GetConnInfo(mtu_resp_event->Connection_Handle, &p_conn_info,&p_eatt_bearer) == BLE_STATUS_SUCCESS) \
              && (p_conn_info->MTU >= 49u) \
              && (p_conn_info->LinkEncrypted == 1u))
          {
            uint8_t i;
            for (i = 0; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
            {
              if (HAP_Context.HA.RestoreContext[i].ConnHandle == 0xFFFF)
              {
                HAP_Context.HA.RestoreContext[i].ConnHandle = p_conn_info->Connection_Handle;
                HAP_Context.HA.RestoreContext[i].State = HAP_HA_RESTORE_STATE_CTRL_POINT_DESC_VALUE;
                HAP_DB_CheckDatabaseChange(p_conn_info);
                break;
              }
            }
          }
          break;
        }

        case ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE:
        case ACI_GATT_SERVER_CONFIRMATION_VSEVT_CODE:
        {
          UseCaseConnInfo_t     *p_conn_info;
          uint8_t               num_links;
          uint16_t              conn_handle;
          /*Get the number of connected devices*/
          num_links = USECASE_DEV_MGMT_GetNumConnectedDevices();
          if (num_links > 0u)
          {
            for (uint8_t i = 0; i < num_links;i++)
            {
              conn_handle = USECASE_DEV_MGMT_GetConnHandle(i);
              if (conn_handle != 0xFFFFu)
              {
                if (USECASE_DEV_MGMT_GetConnInfo(conn_handle, &p_conn_info,&p_eatt_bearer) == BLE_STATUS_SUCCESS)
                {
                  /* Check if connection role is Slave (0x01u)*/
                  if ((p_conn_info->Connection_Handle != 0xFFFFu) && (p_conn_info->Role == 0x01u))
                  {
                    HAP_HA_Restore_Context_t *p_restore_context;

                    if ((HAP_HA_GetRestoreContext(p_conn_info->Connection_Handle, &p_restore_context) == BLE_STATUS_SUCCESS)
                        && p_restore_context->State > 0)
                    {
                      HAP_DB_CheckDatabaseChange(p_conn_info);
                    }
                  }
                }
              }
            }
          }
          break;
        }
      }
    }
  }



  return_value = HAS_ATT_Event_Handler(pEvent);

#if (HAP_HA_IAS_SUPPORT == 1u)
  if (return_value == SVCCTL_EvtNotAck)
  {
    return_value = IAS_Event_Handler(pEvent);
  }
#endif /* (HAP_HA_IAS_SUPPORT == 1u) */

  return return_value;
}

void IAS_Notification(IAS_Notification_Evt_t *pNotification)
{
  HAP_Notification_Evt_t evt;
  uint8_t               alert_value;
  UseCaseConnInfo_t     *p_conn_info;
  BleEATTBearer_t       *p_eatt_bearer = 0;

  if (USECASE_DEV_MGMT_GetConnInfo(pNotification->ConnHandle,&p_conn_info,&p_eatt_bearer) == BLE_STATUS_SUCCESS)
  {
    evt.ConnHandle = p_conn_info->Connection_Handle;
    evt.EvtOpcode = HAP_HA_IAS_ALERT_EVT;
    evt.pInfo = &alert_value;

    switch(pNotification->EvtOpcode)
    {
      case IAS_NO_ALERT_EVT:
      {
        alert_value = 0x00;
        break;
      }

      case IAS_MID_ALERT_EVT:
      {
        alert_value = 0x01;
        break;
      }

      case IAS_HIGH_ALERT_EVT:
      {
        alert_value = 0x02;
        break;
      }

      default:
        break;
    }

    HAP_Notification(&evt);
  }
}

/* Private functions ----------------------------------------------------------*/

tBleStatus HAP_HA_FindPresetWithIndex(uint8_t Index, HAP_Preset_t** pPreset, uint8_t* pPrevIndex)
{
  if (!LST_is_empty(&HAP_Context.HA.PresetList))
  {
    HAP_Preset_t* p_preset;
    uint8_t prev_index = 0;
    LST_get_next_node((tListNode *)&HAP_Context.HA.PresetList, (tListNode **)&p_preset) ;

    /* Parse preset list */
    while ((tListNode *)p_preset != &HAP_Context.HA.PresetList)
    {
      if (p_preset->Index == Index)
      {
        /* Found preset */
        if (pPrevIndex != 0)
        {
          *pPrevIndex = prev_index;
        }
        *pPreset = p_preset;
        return BLE_STATUS_SUCCESS;
      }
      prev_index = p_preset->Index;
      LST_get_next_node((tListNode *)p_preset, (tListNode **)&p_preset) ;
    }
    /* No preset has been found */
    return BLE_STATUS_INVALID_PARAMS;
  }
  else
  {
    return HCI_COMMAND_DISALLOWED_ERR_CODE;
  }
}

tBleStatus HAP_HA_FindNextAvailablePreset(HAP_Preset_t* pStartPreset, HAP_Preset_t** pNextPreset, uint8_t decreasing)
{
  HAP_Preset_t *p_preset = pStartPreset;
  while(1)
  {
    if (decreasing == 0)
    {
      /* Get next preset */
      LST_get_next_node((tListNode *)p_preset, (tListNode **)&p_preset) ;

      if ((tListNode *) p_preset == &HAP_Context.HA.PresetList)
      {
        /* End of list, take first preset */
        LST_get_next_node((tListNode *)p_preset, (tListNode **)&p_preset);
      }
    }
    else
    {
      /* Get prev preset */
      LST_get_prev_node((tListNode *)p_preset, (tListNode **)&p_preset) ;

      if ((tListNode *) p_preset == &HAP_Context.HA.PresetList)
      {
        /* Start of list, take last preset */
        LST_get_prev_node((tListNode *)p_preset, (tListNode **)&p_preset);
      }
    }

    if ((p_preset->Properties & HAP_PRESET_PROPERTIES_IS_AVAILABLE) != 0)
    {
      *pNextPreset = p_preset;
      return BLE_STATUS_SUCCESS;
    }
    else if (p_preset == pStartPreset)
    {
      /* No available preset was found */
      return BLE_STATUS_FAILED;
    }
  }
}
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */
