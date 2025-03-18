/**
  ******************************************************************************
  * @file    has_harc.h
  * @author  MCD Application Team
  * @brief   This file contains definitions used for Hearing Access Service
             as Hearing Aid Remote Controller Role
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
#ifndef __HAS_HARC_H
#define __HAS_HARC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "cmsis_compiler.h"
#include <stdint.h>
#include "ble_types.h"
#include "svc_ctl.h"
#include "hap.h"
#include "usecase_dev_mgmt.h"

/* Defines -----------------------------------------------------------*/

/* Types -----------------------------------------------------------*/

typedef uint8_t HAP_HARC_Operation_t;
#define HAP_HARC_OP_NONE                     (0x00u)         /* No operation */
#define HAP_HARC_OP_READ_HA_FEATURES         (0x01u)         /* Read HA Features Operation */
#define HAP_HARC_OP_READ_ACTIVE_PRESET_INDEX (0x02u)         /* Read Active Prest Index Operation */
#define HAP_HARC_OP_READ_PRESETS_REQUEST     (0x03u)         /* Read Presets Request Operation */
#define HAP_HARC_OP_WRITE_PRESET_NAME        (0x04u)         /* Write Preset Name Operation */
#define HAP_HARC_OP_SET_ACTIVE_PRESET        (0x05u)         /* Set Active Preset Operation */
#define HAP_HARC_OP_SET_NEXT_PRESET          (0x06u)         /* Set Next Preset Operation */
#define HAP_HARC_OP_SET_PREVIOUS_PRESET      (0x07u)         /* Set Previous Preset Operation */


typedef uint8_t HAP_HARC_LinkupProcState_t;
#define HAP_HARC_LINKUP_IDLE                 (0x00u)
#define HAP_HARC_LINKUP_DISC_SERVICE         (0x01u) /* Discover HAS in remote GATT database */
#define HAP_HARC_LINKUP_DISC_CHAR            (0x02u) /* Discover all Characteristics of the
                                                     * HAS in remote GATT database
                                                     */
#define HAP_HARC_LINKUP_DISC_CHAR_DESC       (0x04u) /* Discover all Characteristics descriptor
                                                     * of the HAS in remote GATT database
                                                     */
#define HAP_HARC_LINKUP_READ_CHAR            (0x08u) /* Read Characteristic in the remote GATT
                                                     * database.
                                                     */
#define HAP_HARC_LINKUP_CLIENT_CONFIG        (0x10u) /* Read Characteristic in the remote GATT
                                                     * database.
                                                     */
#define HAP_HARC_LINKUP_RESTORE              (0x20u) /* HAP Linkup restore*/
#define HAP_HARC_LINKUP_COMPLETE             (0x40u) /* HAP Link Up is complete with success */

typedef uint8_t HAS_Characteristic_t;
#define HAS_CHAR_HA_FEATURES                 (0x01)
#define HAS_CHAR_HA_PRESET_CTRL_POINT        (0x02)
#define HAS_CHAR_ACTIVE_PRESET_INDEX         (0x03)

/* Structure used to store GATT characteristic information */
typedef struct
{
  uint16_t      ValueHandle;            /* Handle of the characteristic value */
  uint16_t      DescHandle;             /* handle of the characteristic descriptor */
  uint8_t       Properties;             /* Properties of the characteristic */
  uint16_t      EndHandle;              /* Last handle of the characteristic */
}HAP_HARC_GATT_CharacteristicInfo_t;

typedef struct
{
  uint16_t ConnHandle;
  uint8_t PresetIndex;
  uint8_t PresetName[HAP_MAX_PRESET_NAME_LEN];
  uint8_t NameLen;
  uint8_t NumPreset;
  uint8_t SyncLocally;
} HAP_Operation_Params_t;

/* HAP Controller Instance Structure */
typedef struct
{
  const UseCaseConnInfo_t             *pConnInfo;                     /* Pointer to Connection info structure */
  HAP_LinkupMode_t                    LinkupMode;                     /* Linkup Mode */
  uint16_t                            LinkupState;                    /* Bitmask of HAP_LinkupProcState_t and
                                                                       * HAS_Characteristic_t
                                                                       */
  HAS_Characteristic_t                CurrentLinkupChar;              /* HAP Characteristic being read/discovered */
  HAP_HARC_GATT_CharacteristicInfo_t  *pGattChar;                     /* Pointer on GATT Characteristic */
  uint16_t                            ReqHandle;                      /* Att Handle under process during ATT operation */
  uint8_t                             AttProcStarted;                 /* Flag to 1 if an ATT procedure is started */
  uint8_t                             DelayDeallocation;              /* Indicate if the HAP Client Instance deallocation
                                                                       * should be delayed when ACI_GATT_PROC_COMPLETE_VSEVT_CODE
                                                                       * event is received */

  uint16_t                            HASServiceStartHandle;          /* start handle of the Service in the
                                                                       * GATT Database of the HAS Server
                                                                       */
  uint16_t                            HASServiceEndHandle;            /* end handle of the Service in the
                                                                       * GATT Database of the HAS Server
                                                                       */
  HAP_HARC_GATT_CharacteristicInfo_t  HAFeaturesChar;                 /* Hearing Aid Feature characteristic */
  HAP_HARC_GATT_CharacteristicInfo_t  HAPresetControlPointChar;       /* Hearing Aid Preset Control Point characteristic */
  HAP_HARC_GATT_CharacteristicInfo_t  ActivePresetIndexChar;          /* Active Preset Index characteristic */

  HAP_HA_Features_t                   HAPFeatures;

  uint8_t                             ErrorCode;                      /* GATT Error Code */
} HAP_HARC_Inst_t;

typedef struct
{
  HAP_HARC_Inst_t        aHARCInst[USECASE_DEV_MGMT_MAX_CONNECTION]; /* Array of HARC Instances */
  HAP_HARC_Operation_t   Op;                                         /* Current HAP_Operation_t */
  HAP_Operation_Params_t OpParams;                                   /* Parameters of the current operation */
} HAP_HARC_Context_t;

typedef enum
{
  HARC_LINKUP_COMPLETE_EVT,          /* This event is notified by HAP when the Linkup to a remote
                                      * HAP Server is complete
                                      */
  HARC_HA_FEATURES_EVT,              /* This event is notified by HAP as Hearing Aid Remote Controller when a Hearing
                                      * Aid Feature Characteristic has been read
                                      */
  HARC_ACTIVE_PRESET_INDEX_EVT,      /* This event is notified by HAP as Hearing Aid Remote Controller when an Active
                                      * Preset Index Characteristic has been read
                                      */
  HARC_READ_PRESET_RESPONSE_EVT,     /* This event is notified by HAP as Hearing Aid Remote Controller when a
                                      * Preset has been notified by remote HAP Server after a Preset read request
                                      */
  HARC_PRESET_RECORD_UPDATE_EVT,      /* This event is notified by HAP as Hearing Aid Remote Controller when a
                                      * Preset record update is reported by the remote HAP Server
                                      */
  HARC_PRESET_RECORD_DELETED_EVT,    /* This event is notified by HAP as Hearing Aid Remote Controller when an Preset
                                      * record has been deleted by the remote HAP Server
                                      */
  HARC_PRESET_RECORD_AVAILABLE_EVT,  /* This event is notified by HAP as Hearing Aid Remote Controller when an Preset
                                      * record is available on the remote HAP Server
                                      */
  HARC_PRESET_RECORD_UNAVAILABLE_EVT, /* This event is notified by HAP as Hearing Aid Remote Controller when an Preset
                                      * record is unavailable on the remote HAP Server
                                      */
  HARC_PROC_COMPLETE_EVT,            /* This event is notified by HAP as Hearing Aid Remote Controller when a
                                          * procedure is complete
                                          */
} HAP_HARC_NotCode_t;

typedef struct
{
  tBleStatus         Status;
  HAP_HARC_NotCode_t EvtOpcode;
  uint16_t           ConnHandle;
  uint8_t            *pInfo;
  uint16_t           DataLen;
} HAP_HARC_NotificationEvt_t;

void HAP_HARC_Notification(HAP_HARC_NotificationEvt_t const *pNotification);

/**
  * @brief  Initialize the Hearing Access Profile as Hearing Aid Remote Controller Role
  * @retval status of the initialization
  */
tBleStatus HAP_HARC_Init(void);


/** @brief This function is used by the Device in the HAP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */
SVCCTL_EvtAckStatus_t HAP_HARC_GATT_Event_Handler(void *pEvent);

/**
  * @brief Link Up the HAP Client with remote HAP Server
  * @param  ConnHandle: Connection handle
  * @param  LinkupMode: LinkUp Mode
  * @note HAP_LINKUP_COMPLETE_EVT event will be generated once process is complete
  * @retval status of the operation
  */
tBleStatus HAP_HARC_Linkup(uint16_t ConnHandle, HAP_LinkupMode_t LinkupMode);

/**
  * @brief  Notify ACL Disconnection to HAP HARC
  * @param  ConnHandle: ACL Connection Handle
  */
void HAP_HARC_AclDisconnection(uint16_t ConnHandle);

/**
  * @brief  Notify Link Encrypted to HAP HARC
  * @param  ConnHandle: ACL Connection Handle
  */
void HAP_HARC_LinkEncrypted(uint16_t ConnHandle);

/**
   * @brief  Store GATT Database of the HAP Characteristics and Services
   * @param  ConnHandle: Connection Handle of the remote connection
   * @param  [in] pData: pointer on buffer in which GATT Database of HAP characteristics in which information is stored
   * @param  [in] MaxDataLen: maximum data length to store
   * @param  [out] len : length in bytes of stored data
   * @retval status of the operation
   */
tBleStatus HAP_HARC_StoreDatabase(uint16_t ConnHandle, uint8_t *pData, uint16_t MaxDataLen, uint16_t *len);

/**
   * @brief  Restore GATT Database of the HAS characteristics
   * @param  pHAP_Client: pointer on HAS Client Instance
   * @param  pData: pointer on buffer including GATT Database of HAS characteristics
   * @param  len : length in bytes of stored data
   * @retval status of the operation
   */
tBleStatus HAP_HARC_RestoreDatabase(HAP_HARC_Inst_t *pHAP_Client, uint8_t *pData,uint16_t Len);

#ifdef __cplusplus
}
#endif

#endif /* __HAS_HARC_H */