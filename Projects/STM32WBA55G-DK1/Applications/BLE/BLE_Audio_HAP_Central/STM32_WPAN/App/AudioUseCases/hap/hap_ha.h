/**
  ******************************************************************************
  * @file    has_ha.h
  * @author  MCD Application Team
  * @brief   This file contains definitions used for Hearing Access Service
             as Hearing Aid Role
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HAS_HA_H
#define __HAS_HA_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "cmsis_compiler.h"
#include "ble_types.h"
#include "svc_ctl.h"
#include "hap.h"
#include "has.h"
#include "usecase_dev_mgmt.h"
#include "stm_list.h"

/* Defines -----------------------------------------------------------*/

/* Types -----------------------------------------------------------*/

typedef uint8_t HAP_HA_Restore_State_t;
#define HAP_HA_RESTORE_STATE_IDLE (0x00)
#define HAP_HA_RESTORE_STATE_CTRL_POINT_DESC_VALUE (0x01)
#define HAP_HA_RESTORE_STATE_HA_FEATURES (0x02)
#define HAP_HA_RESTORE_STATE_ACTIVE_PRESET_INDEX (0x03)
#define HAP_HA_RESTORE_STATE_PRESET_LIST (0x04)

typedef struct
{
  HAP_HA_Control_Point_Op_t CurrentOp;
  tListNode                 *pCurrentNode;
  uint8_t                   RemainingNumRecord;
} HAP_HA_CtrlPoint_Operation_t;

typedef struct
{
  uint16_t                  ConnHandle;
  HAP_HA_Restore_State_t    State;
  HAP_Preset_t              *pPreset;
  uint16_t                  ParseIndex;
  uint16_t                  PrevIndex;
} HAP_HA_Restore_Context_t;

typedef struct
{
  uint16_t                  ConnHandle;
  uint8_t                   CtrlPointDescValue;
} HAP_HA_CtrlPointDesc_Context_t;

typedef struct
{
  HAS_ServiceContext_t           HASSvc;
  uint8_t                        ActivePreset;
  HAP_Preset_t                   PresetPool[HAP_MAX_PRESET_NUM];
  tListNode                      PresetPoolList;
  tListNode                      PresetList;
  HAP_HA_Features_t              Features;
  HAP_HA_CtrlPoint_Operation_t   CtrlPointOp;

  HAP_HA_Restore_Context_t       RestoreContext[USECASE_DEV_MGMT_MAX_CONNECTION];
  HAP_HA_CtrlPointDesc_Context_t CtrlPointDescContext[USECASE_DEV_MGMT_MAX_CONNECTION];
} HAP_HA_Context_t;

/**
  * @brief  Initialize the Hearing Access Profile as Hearing Aid Role
  * @retval status of the initialization
  */
tBleStatus HAP_HA_Init(void);

/**
  * @brief  Notify Link Encrypted to HAP HA
  * @param  ConnHandle: ACL Connection Handle
  */
void HAP_HA_LinkEncrypted(uint16_t ConnHandle);

/** @brief This function is used by the Device in the HAP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */
SVCCTL_EvtAckStatus_t HAP_HA_GATT_Event_Handler(void *pEvent);

/**
  * @brief  Store GATT Database of the HAP presets
  * @param  ConnHandle: connection handle specifying the remote device
  * @param  pData: pointer on buffer in which GATT Database of presets in which information is stored
  * @param  MaxDataLen: maximum data length to store
  * @param  len : length in bytes of stored data
  * @retval status of the operation
  */
tBleStatus HAP_HA_StoreDatabase(uint16_t ConnHandle, uint8_t *pData,uint16_t MaxDataLen,uint16_t *len);

/**
  * @brief  Check if database has changed since last connection of remote device
  * @param  ConnHandle: connection handle specifying the remote device
  * @param  pData: pointer on buffer of GATT Database
  * @param  Len: length of the GATT database
  * @retval status of the operation
  */
tBleStatus HAP_HA_CheckDatabaseChange(uint16_t ConnHandle, uint8_t *pData, uint16_t Len);

/**
  * @brief Returns pointer to restore context structure related to ConnHandle
  * @param ConnHandle: ACL Connection Handle
  * @param pRestoreContext: Output pointer to a Restore Structure
  * @retval Status of the operation
  */
tBleStatus HAP_HA_GetRestoreContext(uint16_t ConnHandle, HAP_HA_Restore_Context_t **pRestoreContext);

#ifdef __cplusplus
}
#endif

#endif /* __HAS_HA_H */