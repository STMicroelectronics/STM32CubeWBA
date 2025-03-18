/**
  ******************************************************************************
  * @file    hap.h
  * @author  MCD Application Team
  * @brief   This file contains definitions used for Hearing Access Profile
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
#ifndef __HAP_H
#define __HAP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "cmsis_compiler.h"
#include <stdint.h>
#include "ble_types.h"
#include "hap_config.h"
#include "cap.h"

/* Defines -------------------------------------------------------------------*/

/*
 * HAP_HA_NUM_GATT_SERVICES: number of GATT services required for Hearing Access Profile.
 */
#if (HAP_HA_IAS_SUPPORT == 1u)
#define HAP_HA_NUM_GATT_SERVICES             (2u)
#else /* (HAP_HA_IAS_SUPPORT == 1u) */
#define HAP_HA_NUM_GATT_SERVICES             (1u)
#endif /* (HAP_HA_IAS_SUPPORT == 1u) */

/*
 * HAP_HA_NUM_GATT_ATTRIBUTES: number of GATT attributes required for Hearing Access Profile as Hearing Aid Role
 */
#if (HAP_HA_IAS_SUPPORT == 1u)
#define HAP_HA_NUM_GATT_ATTRIBUTES           (11u)
#else /* (HAP_HA_IAS_SUPPORT == 1u) */
#define HAP_HA_NUM_GATT_ATTRIBUTES           (9u)
#endif /* (HAP_HA_IAS_SUPPORT == 1u) */

/*
 * HAP_HA_ATT_VALUE_ARRAY_SIZE: this macro returns the size of the storage area for Attribute values
 * in HAP Hearing Aid.
 *
 * @param num_ble_links: Maximum number of simultaneous connections that the device will support.
 */
#if (HAP_HA_IAS_SUPPORT == 1u)
#define HAP_HA_ATT_VALUE_ARRAY_SIZE(num_ble_links) (MAX_HEARING_AID_PRESET_CONTROL_POINT_SIZE + 23 + 6*num_ble_links)
#else /* (HAP_HA_IAS_SUPPORT == 1u) */
#define HAP_HA_ATT_VALUE_ARRAY_SIZE(num_ble_links) (MAX_HEARING_AID_PRESET_CONTROL_POINT_SIZE + 17 + 6*num_ble_links)
#endif /* (HAP_HA_IAS_SUPPORT == 1u) */

/* Types ---------------------------------------------------------------------*/
/* Types of mask for roles of the Media Control Profile */
typedef uint8_t HAP_Role_t;
#define HAP_ROLE_HEARING_AID                   (0x01)
#define HAP_ROLE_HEARING_AID_UNICAST_CLIENT    (0x02)
#define HAP_ROLE_HEARING_AID_REMOTE_CONTROLLER (0x04)
#define HAP_ROLE_IMMEDIATE_ALERT_CLIENT        (0x08)

typedef uint8_t HAP_HA_Features_t;
#define HAP_HA_TYPE_BINAURAL_HEARING_AID       (0x00)
#define HAP_HA_TYPE_MONAURAL_HEARING_AID       (0x01)
#define HAP_HA_TYPE_BANDED_HEARING_AID         (0x02)
#define HAP_PRESET_SYNC_SUPPORTED              (0x04)
#define HAP_INDEPENDANT_PRESETS                (0x08)
#define HAP_DYNAMIC_PRESETS                    (0x10)
#define HAP_WRITABLE_PRESETS_SUPPORTED         (0x20)

typedef uint8_t HAP_Preset_Properties_t;
#define HAP_PRESET_PROPERTIES_WRITABLE         (0x01)
#define HAP_PRESET_PROPERTIES_IS_AVAILABLE     (0x02)

typedef uint8_t HAP_Preset_Change_Id_t;
#define HAP_PRESET_CHANGE_ID_GENERIC_UPDATE            (0x00)
#define HAP_PRESET_CHANGE_ID_PRESET_RECORD_DELETED     (0x01)
#define HAP_PRESET_CHANGE_ID_PRESET_RECORD_AVAILABLE   (0x02)
#define HAP_PRESET_CHANGE_ID_PRESET_RECORD_UNAVAILABLE (0x03)

typedef uint8_t HAP_HA_Control_Point_Op_t;
#define HAP_HA_CONTROL_POINT_OP_READ_PRESETS_REQUEST           (0x01)
#define HAP_HA_CONTROL_POINT_OP_READ_PRESETS_RESPONSE          (0x02)
#define HAP_HA_CONTROL_POINT_OP_PRESET_CHANGED                 (0x03)
#define HAP_HA_CONTROL_POINT_OP_WRITE_PRESET_NAME              (0x04)
#define HAP_HA_CONTROL_POINT_OP_SET_ACTIVE_PRESET              (0x05)
#define HAP_HA_CONTROL_POINT_OP_SET_NEXT_PRESET                (0x06)
#define HAP_HA_CONTROL_POINT_OP_SET_PREVIOUS_PRESET            (0x07)
#define HAP_HA_CONTROL_POINT_OP_SET_ACTIVE_PRESET_LOCAL_SYNC   (0x08)
#define HAP_HA_CONTROL_POINT_OP_SET_NEXT_PRESET_LOCAL_SYNC     (0x09)
#define HAP_HA_CONTROL_POINT_OP_SET_PREVIOUS_PRESET_LOCAL_SYNC (0x0A)

typedef uint8_t HAP_SetActivePreset_Resp_t;
#define HAP_SET_ACTIVE_PRESET_RESP_OK          (0x00)          /* New preset is compatible with current state of HA */
#define HAP_SET_ACTIVE_PRESET_RESP_NOK         (0x01)          /* New preset is incompatible with current state of HA */

/* Types of Microphone Control Profile Linkup Mode */
typedef uint8_t HAP_LinkupMode_t;
#define HAP_LINKUP_MODE_COMPLETE          (0x00u) /* Link Up procedure shall be a complete one
                                                    * with remote service and characteristic discovery
                                                                 */
#define HAP_LINKUP_MODE_RESTORE           (0x01u) /* Link Up information is restored from previous
                                                    * complete Link Up from persistent memory.
                                                    */

typedef uint8_t HAP_IAC_AlertLevel_t;
#define HAP_IAC_ALERT_LEVEL_NO_ALERT      (0x00)
#define HAP_IAC_ALERT_LEVEL_MID_ALERT     (0x01)
#define HAP_IAC_ALERT_LEVEL_HIGH_ALERT    (0x02)

/* Types for Hearing Access Profile Events */
typedef enum
{
  HAP_LINKUP_COMPLETE_EVT,               /* This event is notified by HAP when the Linkup to a remote
                                          * HAP Server is complete
                                          */
  HAP_HARC_LINKUP_COMPLETE_EVT,          /* This event is notified by HAP when the Linkup of the HARC role is complete
                                          */
  HAP_HARC_HA_FEATURES_EVT,              /* This event is notified by HAP as Hearing Aid Remote Controller when a Hearing
                                          * Aid Feature Characteristic has been read
                                          */
  HAP_HARC_ACTIVE_PRESET_INDEX_EVT,      /* This event is notified by HAP as Hearing Aid Remote Controller when an Active
                                          * Preset Index Characteristic has been read
                                          */
  HAP_HARC_READ_PRESET_RESPONSE_EVT,     /* This event is notified by HAP as Hearing Aid Remote Controller when a
                                          * Preset has been notified by remote HAP Server after a Preset read request
                                          */
  HAP_HARC_PRESET_RECORD_UPDATE_EVT,      /* This event is notified by HAP as Hearing Aid Remote Controller when a
                                          * Preset record update is reported by the remote HAP Server
                                          */
  HAP_HARC_PRESET_RECORD_DELETED_EVT,    /* This event is notified by HAP as Hearing Aid Remote Controller when an Preset
                                          * record has been deleted by the remote HAP Server
                                          */
  HAP_HARC_PRESET_RECORD_AVAILABLE_EVT,  /* This event is notified by HAP as Hearing Aid Remote Controller when an Preset
                                          * record is available on the remote HAP Server
                                          */
  HAP_HARC_PRESET_RECORD_UNAVAILABLE_EVT, /* This event is notified by HAP as Hearing Aid Remote Controller when an Preset
                                          * record is unavailable on the remote HAP Server
                                          */
  HAP_HARC_PROC_COMPLETE_EVT,            /* This event is notified by HAP as Hearing Aid Remote Controller when a
                                          * procedure is complete
                                          */
  HAP_IAC_LINKUP_COMPLETE_EVT,           /* This event is notified by HAP when the Linkup of the IAC role is complete
                                          */
  HAP_HA_SET_PRESET_REQ_EVT,             /* This event is notified by HAP when the remote HARC has requested a preset
                                          *  change
                                          */
  HAP_HA_ACTIVE_PRESET_EVT,              /* This event is notified by HAP when a preset has been set as active
                                          */
  HAP_HA_PRESET_NAME_UPDATE_EVT,          /* This event is notified by HAP when a the remote HARC has updated a preset name
                                          */
  HAP_HA_IAS_ALERT_EVT                   /* This event is notified by IAS Server when an alert is received from a HAP
                                          * IAC
                                          */
} HAP_NotCode_t;

typedef struct
{
  uint32_t                *Next;  /* Used internally by the HAP */
  uint32_t                *Prev;  /* Used internally by the HAP */
  uint8_t                 Index;
  HAP_Preset_Properties_t Properties;
  uint8_t                 Name[HAP_MAX_PRESET_NAME_LEN];
  uint8_t                 NameLen;
} HAP_Preset_t;

typedef struct
{
  uint8_t                 Index;
  HAP_Preset_Properties_t Properties;
  uint8_t                 Name[HAP_MAX_PRESET_NAME_LEN];
  uint8_t                 NameLen;
} HAP_PresetRecord_t;

/* Structure used in parameter when HAP_LINKUP_COMPLETE_EVT event is notified*/
typedef struct
{
  uint16_t              StartAttHandle; /* ATT Start Handle of TMAS in the remote HAP Device */
  uint16_t              EndAttHandle;   /* ATT End Handle of the TMAS in the remote HAP Device */
} HAP_AttServiceInfo_Evt_t;

typedef struct
{
  HAP_PresetRecord_t         Preset;
  uint8_t                    IsLast;
  uint8_t                    PrevIndex;
} HAP_PresetUpdate_Info_t;

typedef struct
{
  HAP_PresetRecord_t         Preset;
  uint8_t                    IsLast;
} HAP_ReadPresetResponse_Info_t;

typedef struct
{
  uint8_t                    IsLast;
  uint8_t                    Index;
} HAP_PresetChangedIndex_Info_t;

typedef struct
{
  HAP_Preset_t               *pPreset;
  HAP_SetActivePreset_Resp_t *pResp;
} HAP_ActivePresetChangeReq_Info_t;

typedef struct
{
  HAP_Preset_t               *pPreset;
  uint8_t                    SyncLocally;
} HAP_ActivePreset_Info_t;

typedef struct
{
  HAP_NotCode_t         EvtOpcode;      /* Opcode of the notified event */
  tBleStatus            Status;         /* Status associated to the event */
  uint16_t              ConnHandle;     /* ACL Connection Handle associated to the event */
  uint8_t               *pInfo;         /* Pointer on information associated to the event */
}HAP_Notification_Evt_t;

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Initialize the Hearing Access Profile
  * @param  Role: Bitfield of HAP Roles to enable
  * @retval status of the initialization
  */
tBleStatus HAP_Init(HAP_Role_t Role);

/**
  * @brief Link Up the HAP Client roles with remote HAP HA
  * @param  ConnHandle: Connection handle
  * @param  LinkupMode: LinkUp Mode
  * @note HAP_LINKUP_COMPLETE_EVT event will be generated once process is complete
  * @retval status of the operation
  */
tBleStatus HAP_Linkup(uint16_t ConnHandle, HAP_LinkupMode_t LinkupMode);

/**
  * @brief Read the remote Hearing Aid Features
  * @param ConnHandle: The connection handle of the remote HAP Server
  * @note HAP_HARC_HA_FEATURES_EVT will be generated upon characteristic read
  * @return Status of the operation
  */
tBleStatus HAP_HARC_ReadHAFeatures(uint16_t ConnHandle);

/**
  * @brief Read the remote Active Preset Index
  * @param ConnHandle: The connection handle of the remote HAP Server
  * @note HAP_HARC_ACTIVE_PRESET_INDEX_EVT will be generated upon characteristic read
  * @return Status of the operation
  */
tBleStatus HAP_HARC_ReadActivePresetIndex(uint16_t ConnHandle);

/**
  * @brief Send a requests to read remote HAP Server presets
  * @param ConnHandle: The connection handle of the remote HAP Server
  * @param StartIndex: Index from which presets will be read
  * @param NumPresets: Number of presets to read
  * @note HAP_HARC_PRESET_RECORD_EVT will be generated as response from the server
  * @return Status of the operation
  */
tBleStatus HAP_HARC_ReadPresetsRequest(uint16_t ConnHandle, uint8_t StartIndex, uint8_t NumPresets);

/**
  * @brief Send a Write Preset Name command
  * @param ConnHandle: The connection handle of the remote HAP Server
  * @param Index: Index of the preset to write
  * @param pName: A pointer to the new name of the preset
  * @param NameLen: The length of the name
  * @note HAP_HARC_PRESET_RECORD_EVT will be generated upon name update
  * @return Status of the operation
  */
tBleStatus HAP_HARC_WritePresetName(uint16_t ConnHandle, uint8_t Index, uint8_t* pName, uint8_t NameLen);

/**
  * @brief Set preset with specified index on remote HAP Server
  * @param ConnHandle: The connection handle of the remote HAP Server
  * @param Index: The connection handle of the remote HAP Server
  * @param SyncLocally: 1 to enable local synchronization if supported by the remote server
  * @note HAP_HARC_ACTIVE_PRESET_INDEX_EVT will be generated if active preset changes
  * @return Status of the operation
  */
tBleStatus HAP_HARC_SetActivePreset(uint16_t ConnHandle, uint8_t Index, uint8_t SyncLocally);

/**
  * @brief Set next preset on remote HAP Server
  * @param ConnHandle: The connection handle of the remote HAP Server
  * @param SyncLocally: 1 to enable local synchronization if supported by the remote server
  * @note HAP_HARC_ACTIVE_PRESET_INDEX_EVT will be generated if active preset changes
  * @return Status of the operation
  */
tBleStatus HAP_HARC_SetNextPreset(uint16_t ConnHandle, uint8_t SyncLocally);

/**
  * @brief Set previous preset on remote HAP Server
  * @param ConnHandle: The connection handle of the remote HAP Server
  * @param SyncLocally: 1 to enable local synchronization if supported by the remote server
  * @note HAP_HARC_ACTIVE_PRESET_INDEX_EVT will be generated if active preset changes
  * @return Status of the operation
  */
tBleStatus HAP_HARC_SetPreviousPreset(uint16_t ConnHandle, uint8_t SyncLocally);

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
                           uint8_t AdvPacketLength);

/**
  * @brief Set the Hearing Aids Features value
  * @param Features: The bitfield of Hearing Aids Features supported
  * @return Status of the operation
  */
tBleStatus HAP_HA_SetHearingAidFeatures(HAP_HA_Features_t Features);

/**
  * @brief Add a new preset
  * @param Properties: The properties of the preset
  * @param pName: Pointer to the name of the preset
  * @param NameLen: Length of the name of the preset
  * @param pPresetIndex: Returns the index of the preset added
  * @return Status of the operation
  */
tBleStatus HAP_HA_AddPreset(HAP_Preset_Properties_t Properties, uint8_t* pName, uint8_t NameLen, uint8_t* pPresetIndex);

/**
  * @brief Remove a preset
  * @param Index: The index of the preset to remove
  * @return Status of the operation
  */
tBleStatus HAP_HA_RemovePreset(uint8_t Index);

/**
  * @brief Set the availability value of a preset
  * @param Index: The index of the preset to update
  * @param Available: 1 if preset should be set to available, 0 for unavailable
  * @return Status of the operation
  */
tBleStatus HAP_HA_SetPresetAvailability(uint8_t Index, uint8_t Available);

/**
  * @brief Edit a preset
  * @param Index: The index of the preset to update
  * @param Properties: The properties to set
  * @param pName: The name to set
  * @param NameLen: The length of the name to set
  * @return Status of the operation
  */
tBleStatus HAP_HA_EditPreset(uint8_t Index, HAP_Preset_Properties_t Properties, uint8_t* pName, uint8_t NameLen);

/**
  * @brief Set the Active Preset Index value
  * @param ActivePresetIndex: The index of the preset to set as active
  * @return Status of the operation
  */
tBleStatus HAP_HA_SetActivePresetIndex(uint8_t ActivePresetIndex);

/**
  * @brief Set Alert Level on remote Immediate Alert Server
  * @param ConnHandle: The connection handle of the remote IAS Server
  * @param AlertLevel: The Level of the alert
  * @return Status of the operation
  */
tBleStatus HAP_IAC_SetAlertLevel(uint16_t ConnHandle, HAP_IAC_AlertLevel_t AlertLevel);

/**
  * @brief Indicate if HAP HA Database is saved in NVM
  * @param Peer_Address_Type: Peer Address type
  * @param Peer_Address: Peer Address
  * @retval 0 if not present in the database, else 1
  */
uint8_t HAP_HA_DB_IsPresent(uint8_t Peer_Address_Type,const uint8_t Peer_Address[6]);

/**
  * @brief  Remove the record of the HAP HA stored in the Non Volatile memory.
  * @param  PeerIdentityAddressType: Identity address type.
  *                                  Values:
  *                                     - 0x00: Public Identity Address
  *                                     - 0x01: Random (static) Identity Address
  * @param  PeerIdentityAddress : Public or Random (static) Identity address of the peer device
  * @retval status of the operation
  */
tBleStatus HAP_HA_DB_RemoveServicesRecord(uint8_t PeerIdentityAddressType,const uint8_t PeerIdentityAddress[6]);

/**
  * @brief Indicate if HAP HARC Database is saved in NVM
  * @param Peer_Address_Type: Peer Address type
  * @param Peer_Address: Peer Address
  * @retval 0 if not present in the database, else 1
  */
uint8_t HAP_HARC_DB_IsPresent(uint8_t Peer_Address_Type, const uint8_t Peer_Address[6]);

/**
  * @brief  Remove the record of the HAP HARC stored in the Non Volatile memory.
  * @param  PeerIdentityAddressType: Identity address type.
  *                                  Values:
  *                                     - 0x00: Public Identity Address
  *                                     - 0x01: Random (static) Identity Address
  * @param  PeerIdentityAddress : Public or Random (static) Identity address of the peer device
  * @retval status of the operation
  */
tBleStatus HAP_HARC_DB_RemoveServicesRecord(uint8_t PeerIdentityAddressType,const uint8_t PeerIdentityAddress[6]);

/**
  * @brief Indicate if HAP IAC Database is saved in NVM
  * @param Peer_Address_Type: Peer Address type
  * @param Peer_Address: Peer Address
  * @retval 0 if not present in the database, else 1
  */
uint8_t HAP_IAC_DB_IsPresent(uint8_t Peer_Address_Type, const uint8_t Peer_Address[6]);

/**
  * @brief  Remove the record of the HAP IAC stored in the Non Volatile memory.
  * @param  PeerIdentityAddressType: Identity address type.
  *                                  Values:
  *                                     - 0x00: Public Identity Address
  *                                     - 0x01: Random (static) Identity Address
  * @param  PeerIdentityAddress : Public or Random (static) Identity address of the peer device
  * @retval status of the operation
  */
tBleStatus HAP_IAC_DB_RemoveServicesRecord(uint8_t PeerIdentityAddressType, const uint8_t PeerIdentityAddress[6]);

/**
  * @brief  Notify CAP Events
  * @param  pNotification: pointer on notification information
 */
void CAP_HAP_Notification(CAP_Notification_Evt_t *pNotification);

/**
  * @brief  Notify HAP Events
  * @param  pNotification: pointer on notification information
 */
void HAP_Notification(HAP_Notification_Evt_t *pNotification);


#ifdef __cplusplus
}
#endif

#endif /* __HAP_H */
