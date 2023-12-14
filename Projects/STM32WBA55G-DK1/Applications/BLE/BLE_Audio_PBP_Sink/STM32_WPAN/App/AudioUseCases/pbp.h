/**
  ******************************************************************************
  * @file    pbp.h
  * @author  MCD Application Team
  * @brief   This file contains definitions used for Media Control Profile
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
#ifndef __PBP_H
#define __PBP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ble_types.h"
#include "bap_types.h"
#include "cap.h"
/* Defines -------------------------------------------------------------------*/
#define PUBLIC_BROADCAST_ANNOUNCEMENT_SERVICE_UUID 0x1856
#define AD_TYPE_BROADCAST_NAME                     0x30

#define PBP_MAX_BROADCAST_NAME_SIZE 32

/* Types ---------------------------------------------------------------------*/
/* Types of mask for roles of the Media Control Profile */
typedef uint8_t PBP_Role_t;
#define PBP_ROLE_PUBLIC_BROADCAST_SOURCE			  (0x01)
#define PBP_ROLE_PUBLIC_BROADCAST_SINK			    (0x02)
#define PBP_ROLE_PUBLIC_BROADCAST_ASSISTANT			(0x04)

typedef uint8_t PBP_PBS_State_t;
#define PBP_PBS_STATE_IDLE                      (0x00)
#define PBP_PBS_STATE_STARTING                  (0x01)
#define PBP_PBS_STATE_STARTED                   (0x02)

typedef uint8_t PBP_PBK_Scan_State;
#define PBP_PBK_SCAN_STATE_IDLE     0x00
#define PBP_PBK_SCAN_STATE_SCANNING 0x01

typedef uint8_t PBP_PBK_PA_Sync_State;
#define PBP_PBK_PA_SYNC_STATE_IDLE          0x00
#define PBP_PBK_PA_SYNC_STATE_SYNCHRONIZING 0x01
#define PBP_PBK_PA_SYNC_STATE_SYNCHRONIZED  0x02

typedef uint8_t PBP_PBK_BIG_Sync_State;
#define PBP_PBK_BIG_SYNC_STATE_IDLE          0x00
#define PBP_PBK_BIG_SYNC_STATE_SYNCHRONIZING 0x01
#define PBP_PBK_BIG_SYNC_STATE_SYNCHRONIZED  0x02

typedef uint8_t PBP_PBA_Scan_State;
#define PBP_PBA_SCAN_STATE_IDLE     0x00
#define PBP_PBA_SCAN_STATE_SCANNING 0x01

typedef uint8_t PBP_PBA_PA_Sync_State;
#define PBP_PBA_PA_SYNC_STATE_IDLE          0x00
#define PBP_PBA_PA_SYNC_STATE_SYNCHRONIZING 0x01
#define PBP_PBA_PA_SYNC_STATE_SYNCHRONIZED  0x02

typedef uint8_t PBP_Features_t;
#define PBP_FEATURES_ENCRYPTED                  (0x01)
#define PBP_FEATURES_STANDARD_QUALITY           (0x02)
#define PBP_FEATURES_HIGH_QUALITY               (0x04)

/* Types for Public Broadcast Profile Events */
typedef enum
{
  PBP_PBS_BROADCAST_AUDIO_STARTED_EVT,          /* This event is notified by Public Broadcast Source when a Broadcast
                                                 * Audio Stop procedure is complete
                                                 * The pInfo field indicates details about the Broadcast Source through
                                                 * the CAP_Broadcast_AudioStarted_Info_t type
                                                 */
  PBP_PBS_BROADCAST_AUDIO_STOPPED_EVT,          /* This event is notified by Public Broadcast Sink when a Broadcast
                                                 * Audio Stop procedure is complete
                                                 */
  PBP_PBK_BROADCAST_SOURCE_ADV_REPORT_EVT,      /* This event is notified by Public Broadcast Sink when a Broadcast
                                                 * Source has been scanned
                                                 * The pInfo field indicates details about the Broadcast Source through
                                                 * the PBP_Broadcast_Source_Adv_Report_Data_t type
                                                 */
  PBP_PBK_PA_SYNC_ESTABLISHED_EVT,              /* This event is notified by Acceptor when when synchronization to a
                                                 * periodic advertising train has been established
                                                 * The pInfo field indicates the details about the Periodic Advertising
                                                 * train through the BAP_PA_Sync_Established_Data_t type
                                                 */
  PBP_PBK_BASE_REPORT_EVT,                      /* This event is notified by Acceptor when a BASE report is received
                                                 * through a periodic advertising train
                                                 * The pInfo field indicates the content of the BASE structure through
                                                 * the BAP_BASE_Report_Data_t type
                                                 */
  PBP_PBK_BIGINFO_REPORT_EVT,                   /* This is event is notified by Acceptor when a BIGInfo report is
                                                 * received through a periodic advertising train
                                                 * The pInfo field indicates the content of the BIGInfo through
                                                 * the BAP_BIGInfo_Report_Data_t type
                                                 */
  PBP_PBK_BIG_SYNC_ESTABLISHED_EVT,             /* This event is notified by Acceptor when synchronization to a BIG has
                                                 * been established
                                                 * The pInfo field indicates the details about the BIG through the
                                                 * BAP_BIG_Sync_Established_Data_t type
                                                 */
  PBP_PBK_PA_SYNC_LOST_EVT,                     /* This event is notified by Acceptor when synchronization to a periodic
                                                 * advertising train has been lost
                                                 * The pInfo field indicates the details about the Periodic Advertising
                                                 * train through the BAP_PA_Sync_Lost_Data_t type
                                                 */
  PBP_PBK_BIG_SYNC_LOST_EVT,                    /* This event is notified by Acceptor when synchronization to a BIG has
                                                 * been lost
                                                 * The pInfo field indicates the BIG Handle and the reason of the sync
                                                 * loss through the BAP_BIG_Sync_Lost_Data_t type
                                                 */
  PBP_BROADCAST_AUDIO_UP_EVT,                   /* This event is notified by acceptor and initiator when Audio Path has
                                                 * been setup
                                                 */
  PBP_BROADCAST_AUDIO_DOWN_EVT,                 /* This event is notified by acceptor and initiator when Audio Path has
                                                 * been removed
                                                 */
  PBP_PBA_REM_BROADCAST_RECEIVE_STATE_INFO_EVT, /* This event is notified by Public Broadcast Assistant
                                                 * to report Broadcast Receive State value
                                                 * This event is notified during CAP linkup process or when
                                                 * remote deviced updates it through GATT Notifications
                                                 * The pInfo field indicates the content of the broadcast receive state
                                                 * characteristic through the BAP_BA_Broadcast_Source_State_t type
                                                 */
  PBP_PBA_REM_REMOVED_SOURCE_EVT,               /* This event is notified by Public Broadcast Assistant
                                                 * to report that a remote Scan Delegator has removed a source
                                                 */
  PBP_PBA_SOLICITING_SCAN_DELEGATOR_REPORT_EVT, /* This event is notified by Public Broadcast Assistant
                                                 * when a Scan Delegator soliciting request has been scanned
                                                 * The pInfo field indicates the address and address type of the Scan
                                                 * delagor through the BAP_Soliciting_Scan_Delegator_Data_t type
                                                 */
  PBP_PBA_BROADCAST_SOURCE_ADV_REPORT_EVT,      /* This event is notified by Public Broadcast Assistant
                                                 * when a Broadcast Sourcehas been scanned
                                                 * The pInfo field indicates details about the Broadcast Source through
                                                 * the BAP_Broadcast_Source_Adv_Report_Data_t type
                                                 */
  PBP_PBA_PA_SYNC_ESTABLISHED_EVT,               /* This event is notified by Public Broadcast Assistant
                                                 * when synchronizationto a periodic advertising train has been
                                                 * established
                                                 * The pInfo field indicates the details about the Periodic Advertising
                                                 * train through the BAP_PA_Sync_Established_Data_t type
                                                 */
  PBP_PBA_PA_SYNC_LOST_EVT,                     /* This event is notified by Public Broadcast Assistant
                                                 * when synchronization to a periodic advertising train has been lost
                                                 * The pInfo field indicates the details about the Periodic Advertising
                                                 * train through the BAP_PA_Sync_Lost_Data_t type
                                                 */
  PBP_PBA_BASE_REPORT_EVT,                      /* This is event is notified by Public Broadcast Assistant
                                                 * when a BASE report is received through a periodic advertising train
                                                 * The pInfo field indicates the content of the BASE structure through
                                                 * the BAP_BASE_Report_Data_t type
                                                 */
  PBP_PBA_BIGINFO_REPORT_EVT                    /* This is event is notified by Public Broadcast Assistant
                                                 * when a BIGInfo report is received through a periodic advertising
                                                 * train
                                                 * The pInfo field indicates the content of the BIGInfo through
                                                 * the BAP_BIGInfo_Report_Data_t type
                                                 */

} PBP_NotCode_t;

typedef struct
{
  PBP_NotCode_t EvtOpcode;              /* Opcode of the notified event */
  tBleStatus    Status;                 /* Status associated to the event */
  uint16_t      ConnHandle;             /* ACL Connection Handle associated to the event */
  uint8_t       *pInfo;                 /* Pointer on information associated to the event */
}PBP_Notification_Evt_t;

typedef struct
{
  BAP_Broadcast_Source_Adv_Report_Data_t *pBAPReport;
  PBP_Features_t                         Features;
  const uint8_t                          *pMetadata;
  uint8_t                                MetadataLength;
  uint16_t                               Appearance;
  const uint8_t                          *pBroadcastName;
  uint8_t                                BroadcastNameLength;
}PBP_Broadcast_Source_Adv_Report_Data_t;

/* Structure used in parameter of the PBP_PBS_BroadcastAudioStart() API */
typedef struct
{
  CAP_Broadcast_AudioStart_Params_t *pBroadcastAudioStartParams; /* A pointer to the Brodacast Audio Start Params Struct
                                                                                                                      */
  uint8_t StandardQuality; /* 1 if the broadcast is configured with a mandatory broadcast sink configuration, 0
                                                                                                            otherwise */
  uint8_t HighQuality; /* 1 if the broadcast is configured with a 48KHz BAP configuration, 0 otherwise */
  uint8_t *pAdvMetadata; /* A pointer to a metadata array to include in Public Broadcast Announcement */
  uint8_t AdvMetadataLen; /* The length of the metadata to include in the Public Broadcast Announcement */
  uint8_t *pBroadcastName; /* A human-readable name for the broadcast */
  uint8_t BroadcastNameLen; /* The length of the Broadcast Name (MIN 4, MAX 32) */
  uint16_t Appearance; /* The Appearance of the Broadcast */
} PBP_PBS_BroadcastAudioStart_Params_t;

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Initialize the Public Broadcast Profile
  * @param  Role: PBP Role to initialize
  * @retval status of the initialization
  */
tBleStatus PBP_Init(PBP_Role_t Role);

/**
  * @brief Perform a Broadcast Audio Start procedure using PBP requirements
  * @param pPBSBroadcastAudioStartParams: A pointer to the PBS Broadcast Audio Start Parameters Structure
  * @retval status of the operation
  */
tBleStatus PBP_PBS_BroadcastAudioStart(PBP_PBS_BroadcastAudioStart_Params_t *pPBSBroadcastAudioStartParams);

/**
  * @brief Perform the Broadcast Audio Stop procedure
  * @param BigHandle: Handle of the BIG of the Broadcast Source to stop
  * @param Release: 0x00 to perform Disable operation, 0x01 to perform Release operation
  * @retval status of the operation
  */
tBleStatus PBP_PBS_BroadcastAudioStop(uint8_t BigHandle, uint8_t Release);

/**
  * @brief Perform the Broadcast Audio Update procedure
  * @param AdvHandle: Handle of the periodic advertising of the broadcast source
  * @param pBASEGroup: Pointer to the BASE Group for the broadcast source
  * @retval status of the operation
  */
tBleStatus PBP_PBS_BroadcastAudioUpdate(uint8_t AdvHandle, BAP_BASE_Group_t *pBASEGroup);

/**
 * @brief Start the parsing of Extended Advertising Reports to look for PBP UUIDs and generate
 *        PBP_PBK_BROADCAST_SOURCE_ADV_REPORT_EVT events
 * @retval status of the operation
 */
tBleStatus PBP_PBK_StartAdvReportParsing(void);

/**
 * @brief Stops the parsing of Extended Advertising Reports to look for PBP UUIDs
 * @retval status of the operation
 */
tBleStatus PBP_PBK_StopAdvReportParsing(void);

/**
 * @brief Synchronize to the periodic advertising train of the given device to discover more details about the
 *   related audio stream
 * @param AdvSid: ahe Advertising Set ID of the periodic advertising train to synchronize to
 * @param pAdvAddress: a pointer to the address of the advertising device
 * @param pAddressType: Type of the peer address: 0x00 for Public, 0x01 for Random
 * @param Skip: Number of Periodic Advertising Events skippable
 * @param SyncTimeout: Synchronization Timeout
 * @retval status of the operation
 */
tBleStatus PBP_PBK_StartPASync(uint8_t AdvSid, const uint8_t *pAdvAddress, uint8_t pAddressType, uint16_t Skip,
                               uint16_t SyncTimeout);

/**
 * @brief Synchronize to a BIG and start receiving data from it
 * @param BigHandle: the handle of the BIG
 * @param SyncHandle: The handle of the periodic advertising train related to the BIG
 * @param pBisIndex: an array of the BIS index of the BIG to synchronize to
 * @param NumBis: the number of BIS to synchronize
 * @param pGroup: a pointer to the BASE group related
 * @param SubgroupIndex: The index of the subgroup being synced
 * @param Encryption: 0x00 to disable encryption, 0x01 to enable it
 * @param BroadcastCode: Broadcast code for the encryption
 * @param MSE: Maximum of Sub Events Used
 * @param Timeout: BIG Synchronization Timeout
 * @retval status of the operation
 */
tBleStatus PBP_PBK_StartBIGSync(uint8_t BigHandle,
                                uint16_t SyncHandle,
                                uint8_t *pBisIndex,
                                uint8_t NumBis,
                                BAP_BASE_Group_t *pGroup,
                                uint8_t SubgroupIndex,
                                uint8_t Encryption,
                                uint32_t *BroadcastCode,
                                uint8_t MSE,
                                uint16_t Timeout);

/**
 * @brief Stop the synchronization to a periodic advertising train
 * @param SyncHandle: The handle of the Periodic Advertising train
 * @retval status of the operation
 */
tBleStatus PBP_PBK_StopPASync(uint16_t SyncHandle);

/**
 * @brief Terminate the synchronization to a BIG
 * @param BigHandle: the handle of the BIG to terminate
 * @retval status of the operation
 */
tBleStatus PBP_PBK_StopBIGSync(uint8_t BigHandle);

/**
 * @brief Start parsing Extended Advertising report to search for Scan Delagors solliciting assistance and Broadcast
 *        Sources on behalf a connected Scan Delegator
 * @param ConnHandle: The handle of the connection to the remote Scan Delegator. If no Scan Delegator is currently
 *                    connected, ignore this parameter
 * @retval status of the operation
*/
tBleStatus PBP_PBA_StartAdvReportParsing(uint16_t ConnHandle);

/**
 * @brief Stop parsing Extended Advertising report to search for Scan Delagors solliciting assistance and Broadcast
 *        Sources on behalf a connected Scan Delegator
 * @param ConnHandle: The handle of the connection to the remote Scan Delegator. If no Scan Delegator is currently
 *                    connected, ignore this parameter
 * @retval status of the operation
*/
tBleStatus PBP_PBA_StopAdvReportParsing(uint16_t ConnHandle);

/**
 * @brief Add a Broadcast Source to the remote Scan Delegator
 * @param ConnHandle: The handle of the connection to the remote Scan Delegator
 * @param pBroadcastSource: A pointer to a BAP_BA_Broadcast_Source_Add_t structure containing details about the source to
 *                          add
 * @retval status of the operation
*/
tBleStatus PBP_PBA_AddBroadcastSource(uint16_t ConnHandle, BAP_BA_Broadcast_Source_Add_t *pBroadcastSource);

/**
 * @brief Modify a Broadcast Source on the remote Scan Delegator
 * @param ConnHandle: The handle of the connection to the remote Scan Delegator
 * @param pBroadcastSource: A pointer to a BAP_BA_Broadcast_Source_t structure containing details about the
 *                          source to modify
 * @retval status of the operation
*/
tBleStatus PBP_PBA_ModifyBroadcastSource(uint16_t ConnHandle,
                                                        BAP_BA_Broadcast_Source_Modify_t *pBroadcastSource);

/**
 * @brief Removes a Broadcast Source from a remote Scan Delegator
 * @param ConnHandle: The handle of the connection to the remote Scan Delegator
 * @param SourceId: The ID of the source to remove
 * @retval status of the operation
*/
tBleStatus PBP_PBA_RemoveBroadcastSource(uint16_t ConnHandle, uint8_t SourceId);

/**
 * @brief Send the broadcast code of a broadcast source to a remote Scan Delegator
 * @param ConnHandle: The handle of the connection to the remote Scan Delegator
 * @param SourceId: The ID of the Broadcast Source missing a Broadcast Code
 * @param pBroadcastCode: The 16 bytes broadcast code of the broadcast Source
 * @retval status of the operation
*/
tBleStatus PBP_PBA_SetBroadcastCode(uint16_t ConnHandle, uint8_t SourceId, uint32_t *pBroadcastCode);

/**
 * @brief Perform a Scan Offload operation to transfer a Periodic Advertising Sync to a Scan Delegator
 * @param ConnHandle: The handle of the connection to the remote Scan Delegator
 * @param PAHandle: The handle of the periodic advertising train to transfer (synchandle or advhandle if local)
 * @param SourceId: The ID of the source to remove
 * @retval status of the operation
*/
tBleStatus PBP_PBA_ScanOffload(uint16_t ConnHandle, uint16_t PAHandle, uint8_t SourceId);

/**
 * @brief Synchronize with a Periodic Advertising train to discover BASE info about a broadcast source
 * @param AdvSid: Advertising SID of the periodic advertising train to synchronize
 * @param pAdvAddress: Pointer to the address of the device doing periodic advertising
 * @param pAddressType: Type of the peer address: 0x00 for Public, 0x01 for Random
 * @param Skip: Number of Periodic Advertising Events skippable
 * @param SyncTimeout: Synchronization Timeout
 * @retval status of the operation
 */
tBleStatus PBP_PBA_StartPASync(uint8_t AdvSid, const uint8_t *pAdvAddress, uint8_t pAddressType, uint16_t Skip,
                               uint16_t SyncTimeout);

/**
 * @brief Stop the synchronization to a periodic advertising train
 * @param SyncHandle: The handle of the Periodic Advertising train
 * @retval status of the operation
 */
tBleStatus PBP_PBA_StopPASync(uint16_t SyncHandle);

/**
  * @brief  Notify PBP Events
  * @param  pNotification: pointer on notification information
 */
void PBP_Notification(PBP_Notification_Evt_t *pNotification);

/**
  * @brief  Notify CAP Events for PBP
  * @param  pNotification: pointer on notification information
 */
void CAP_PBP_Notification(CAP_Notification_Evt_t *pNotification);

#ifdef __cplusplus
}
#endif

#endif /* __PBP_H */
