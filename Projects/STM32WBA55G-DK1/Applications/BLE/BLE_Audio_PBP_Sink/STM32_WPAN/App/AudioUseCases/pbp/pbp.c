/**
  ******************************************************************************
  * @file    pbp.c
  * @author  MCD Application Team
  * @brief   This file contains Public Broadcast Profile feature
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
#include "cmsis_compiler.h"
#include "pbp_log.h"
#include "pbp_alloc.h"
#include "pbp.h"
#include "pbp_config.h"
#include "cap.h"
#include "stm32_mem.h"
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions prototype------------------------------------------------*/

/* External functions prototype------------------------------------------------*/

/* Functions Definition ------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/
/**
  * @brief  Initialize the Public Broadcast Profile
  * @param  Role: PBP Role to initialize
  * @retval status of the initialization
  */
tBleStatus PBP_Init(PBP_Role_t Role)
{
  tBleStatus status = BLE_STATUS_SUCCESS;

  /*Check Role parameter*/
  if( (Role == 0) || (Role > (PBP_ROLE_PUBLIC_BROADCAST_SOURCE
                                                |PBP_ROLE_PUBLIC_BROADCAST_SINK
                                                  |PBP_ROLE_PUBLIC_BROADCAST_ASSISTANT)))
  {
    return BLE_STATUS_INVALID_PARAMS;
  }
#if (BLE_CFG_PBP_PUBLIC_BROADCAST_SOURCE_ROLE == 0u)
  if( (Role & PBP_ROLE_PUBLIC_BROADCAST_SOURCE) == PBP_ROLE_PUBLIC_BROADCAST_SOURCE)
  {
    return BLE_STATUS_INVALID_PARAMS;
  }
#endif /*(BLE_CFG_PBP_PUBLIC_BROADCAST_SOURCE_ROLE == 0u)*/

#if (BLE_CFG_PBP_PUBLIC_BROADCAST_SINK_ROLE == 0u)
  if( (Role & PBP_ROLE_PUBLIC_BROADCAST_SINK) == PBP_ROLE_PUBLIC_BROADCAST_SINK)
  {
    return BLE_STATUS_INVALID_PARAMS;
  }
#endif /*(BLE_CFG_PBP_PUBLIC_BROADCAST_SINK_ROLE == 0u)*/

#if (BLE_CFG_PBP_PUBLIC_BROADCAST_ASSISTANT_ROLE == 0u)
  if( (Role & PBP_ROLE_PUBLIC_BROADCAST_ASSISTANT) == PBP_ROLE_PUBLIC_BROADCAST_ASSISTANT)
  {
    return BLE_STATUS_INVALID_PARAMS;
  }
#endif /*(BLE_CFG_PBP_PUBLIC_BROADCAST_ASSISTANT_ROLE == 0u)*/

  PBP_Context.Role = Role;
  PBP_Context.PBKScanState = PBP_PBK_SCAN_STATE_IDLE;
  PBP_Context.PBKPASyncState = PBP_PBK_PA_SYNC_STATE_IDLE;
  PBP_Context.PBKBIGSyncState = PBP_PBK_BIG_SYNC_STATE_IDLE;
  PBP_Context.PBAScanState = PBP_PBA_SCAN_STATE_IDLE;
  PBP_Context.PBAPASyncState = PBP_PBA_PA_SYNC_STATE_IDLE;
  PBP_Context.PBSState = PBP_PBS_STATE_IDLE;
  return status;
}

/**
  * @brief Perform a Broadcast Audio Start procedure using PBP requirements
  * @param pPBSBroadcastAudioStartParams: A pointer to the PBS Broadcast Audio Start Parameters Structure
  * @retval status of the operation
  */
tBleStatus PBP_PBS_BroadcastAudioStart(PBP_PBS_BroadcastAudioStart_Params_t *pPBSBroadcastAudioStartParams)
{
  tBleStatus ret = HCI_COMMAND_DISALLOWED_ERR_CODE;

#if (BLE_CFG_PBP_PUBLIC_BROADCAST_SOURCE_ROLE == 1u)
  if (PBP_Context.Role & PBP_ROLE_PUBLIC_BROADCAST_SOURCE)
  {
    uint8_t public_broadcast_announcement[12 + BLE_PBP_MAX_ADV_METADATA_SIZE + PBP_MAX_BROADCAST_NAME_SIZE
    + BLE_PBP_MAX_ADDITIONAL_ADV_DATA];
    uint8_t data_len = 0;

    if (pPBSBroadcastAudioStartParams->AdvMetadataLen > BLE_PBP_MAX_ADV_METADATA_SIZE
        || pPBSBroadcastAudioStartParams->BroadcastNameLen < 4 || pPBSBroadcastAudioStartParams->BroadcastNameLen > 32
          || pPBSBroadcastAudioStartParams->pBroadcastAudioStartParams->AdditionalAdvDataLen
            > BLE_PBP_MAX_ADDITIONAL_ADV_DATA)
    {
      return BLE_STATUS_INVALID_PARAMS;
    }

    /* Add Public Broadcast Announcement */
    public_broadcast_announcement[data_len++] = 5 + pPBSBroadcastAudioStartParams->AdvMetadataLen;
    public_broadcast_announcement[data_len++] = AD_TYPE_SERVICE_DATA;
    public_broadcast_announcement[data_len++] = PUBLIC_BROADCAST_ANNOUNCEMENT_SERVICE_UUID & 0xFF;
    public_broadcast_announcement[data_len++] = PUBLIC_BROADCAST_ANNOUNCEMENT_SERVICE_UUID >> 8;
    public_broadcast_announcement[data_len++] = pPBSBroadcastAudioStartParams->pBroadcastAudioStartParams->Encryption
      * PBP_FEATURES_ENCRYPTED| pPBSBroadcastAudioStartParams->StandardQuality * PBP_FEATURES_STANDARD_QUALITY
        | pPBSBroadcastAudioStartParams->HighQuality * PBP_FEATURES_HIGH_QUALITY;
    public_broadcast_announcement[data_len++] = pPBSBroadcastAudioStartParams->AdvMetadataLen;

    UTIL_MEM_cpy_8(&public_broadcast_announcement[data_len], pPBSBroadcastAudioStartParams->pAdvMetadata,
                   pPBSBroadcastAudioStartParams->AdvMetadataLen);
    data_len += pPBSBroadcastAudioStartParams->AdvMetadataLen;

    /* Add Appearance */
    public_broadcast_announcement[data_len++] = 3;
    public_broadcast_announcement[data_len++] = AD_TYPE_APPEARANCE;
    public_broadcast_announcement[data_len++] = pPBSBroadcastAudioStartParams->Appearance & 0xFF;
    public_broadcast_announcement[data_len++] = pPBSBroadcastAudioStartParams->Appearance >> 8;

    /* Add Broadcast Name */
    if (pPBSBroadcastAudioStartParams->BroadcastNameLen > 0)
    {
      public_broadcast_announcement[data_len++] = pPBSBroadcastAudioStartParams->BroadcastNameLen + 1;
      public_broadcast_announcement[data_len++] = AD_TYPE_BROADCAST_NAME;
      UTIL_MEM_cpy_8(&public_broadcast_announcement[data_len], pPBSBroadcastAudioStartParams->pBroadcastName,
                     pPBSBroadcastAudioStartParams->BroadcastNameLen);
      data_len += pPBSBroadcastAudioStartParams->BroadcastNameLen;
    }

    /* Copy Additional Data */
    UTIL_MEM_cpy_8(&public_broadcast_announcement[data_len],
                   pPBSBroadcastAudioStartParams->pBroadcastAudioStartParams->pAdditionalAdvData,
                   pPBSBroadcastAudioStartParams->pBroadcastAudioStartParams->AdditionalAdvDataLen);
    data_len += pPBSBroadcastAudioStartParams->pBroadcastAudioStartParams->AdditionalAdvDataLen;

    pPBSBroadcastAudioStartParams->pBroadcastAudioStartParams->pAdditionalAdvData = &public_broadcast_announcement[0];
    pPBSBroadcastAudioStartParams->pBroadcastAudioStartParams->AdditionalAdvDataLen = data_len;

    ret = CAP_Broadcast_AudioStart(pPBSBroadcastAudioStartParams->pBroadcastAudioStartParams);
    if (ret == BLE_STATUS_SUCCESS)
    {
      PBP_Context.PBSState = PBP_PBS_STATE_STARTING;
    }
  }
#endif /* (BLE_CFG_PBP_PUBLIC_BROADCAST_SOURCE_ROLE == 1u) */
  return ret;

}

/**
  * @brief Perform the Broadcast Audio Stop procedure
  * @param BigHandle: Handle of the BIG of the Broadcast Source to stop
  * @param Release: 0x00 to perform Disable operation, 0x01 to perform Release operation
  * @retval status of the operation
  */
tBleStatus PBP_PBS_BroadcastAudioStop(uint8_t BigHandle, uint8_t Release)
{
  tBleStatus ret = HCI_COMMAND_DISALLOWED_ERR_CODE;

#if (BLE_CFG_PBP_PUBLIC_BROADCAST_SOURCE_ROLE == 1u)
  if (PBP_Context.Role & PBP_ROLE_PUBLIC_BROADCAST_SOURCE)
  {
    ret = CAP_Broadcast_AudioStop(BigHandle, Release);
    PBP_Context.WaitForAudioDown = 1u;
    if (ret == BLE_STATUS_SUCCESS)
    {
      PBP_Context.PBSState = PBP_PBS_STATE_STOPPING;
    }
  }
#endif /* (BLE_CFG_PBP_PUBLIC_BROADCAST_SOURCE_ROLE == 1u) */
  return ret;
}

/**
  * @brief Perform the Broadcast Audio Update procedure
  * @param AdvHandle: Handle of the periodic advertising of the broadcast source
  * @param pBASEGroup: Pointer to the BASE Group fo the broadcast source
  * @retval status of the operation
  */
tBleStatus PBP_PBS_BroadcastAudioUpdate(uint8_t AdvHandle, BAP_BASE_Group_t *pBASEGroup)
{
  tBleStatus ret = HCI_COMMAND_DISALLOWED_ERR_CODE;

#if (BLE_CFG_PBP_PUBLIC_BROADCAST_SOURCE_ROLE == 1u)
  if (PBP_Context.Role & PBP_ROLE_PUBLIC_BROADCAST_SOURCE)
  {
    ret = CAP_Broadcast_AudioUpdate(AdvHandle, pBASEGroup);
  }
#endif /* (BLE_CFG_PBP_PUBLIC_BROADCAST_SOURCE_ROLE == 1u) */
  return ret;
}

/**
 * @brief Start the parsing of Extended Advertising Reports to look for PBP UUIDs and generate
 *        PBP_PBK_BROADCAST_SOURCE_ADV_REPORT_EVT events
 * @retval status of the operation
 */
tBleStatus PBP_PBK_StartAdvReportParsing(void)
{
  tBleStatus ret = HCI_COMMAND_DISALLOWED_ERR_CODE;

#if (BLE_CFG_PBP_PUBLIC_BROADCAST_SINK_ROLE == 1u)
  if (PBP_Context.Role & PBP_ROLE_PUBLIC_BROADCAST_SINK
      && PBP_Context.PBKScanState == PBP_PBK_SCAN_STATE_IDLE)
  {
    ret = CAP_Broadcast_StartAdvReportParsing();
    if (ret == BLE_STATUS_SUCCESS)
    {
      PBP_Context.PBKScanState = PBP_PBK_SCAN_STATE_SCANNING;
    }
  }
#endif /* (BLE_CFG_PBP_PUBLIC_BROADCAST_SINK_ROLE == 1u) */
  return ret;
}

/**
 * @brief Stops the parsing of Extended Advertising Reports to look for PBP UUIDs
 * @retval status of the operation
 */
tBleStatus PBP_PBK_StopAdvReportParsing(void)
{
  tBleStatus ret = HCI_COMMAND_DISALLOWED_ERR_CODE;

#if (BLE_CFG_PBP_PUBLIC_BROADCAST_SINK_ROLE == 1u)
  if (PBP_Context.Role & PBP_ROLE_PUBLIC_BROADCAST_SINK
      && PBP_Context.PBKScanState == PBP_PBK_SCAN_STATE_SCANNING)
  {
    ret = CAP_Broadcast_StopAdvReportParsing();
    if (ret == BLE_STATUS_SUCCESS)
    {
      PBP_Context.PBKScanState = PBP_PBK_SCAN_STATE_IDLE;
    }
  }
#endif /* (BLE_CFG_PBP_PUBLIC_BROADCAST_SINK_ROLE == 1u) */
  return ret;
}

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
                               uint16_t SyncTimeout)
{
  tBleStatus ret = HCI_COMMAND_DISALLOWED_ERR_CODE;

#if (BLE_CFG_PBP_PUBLIC_BROADCAST_SINK_ROLE == 1u)
  if (PBP_Context.Role & PBP_ROLE_PUBLIC_BROADCAST_SINK)
  {
    ret = CAP_Broadcast_StartPASync(AdvSid, pAdvAddress, pAddressType, Skip, SyncTimeout);
    if (ret == BLE_STATUS_SUCCESS)
    {
      PBP_Context.PBKPASyncState = PBP_PBK_PA_SYNC_STATE_SYNCHRONIZING;
    }
  }
#endif /* (BLE_CFG_PBP_PUBLIC_BROADCAST_SINK_ROLE == 1u) */
  return ret;
}

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
                                uint16_t Timeout)
{
  tBleStatus ret = HCI_COMMAND_DISALLOWED_ERR_CODE;

#if (BLE_CFG_PBP_PUBLIC_BROADCAST_SINK_ROLE == 1u)
  if (PBP_Context.Role & PBP_ROLE_PUBLIC_BROADCAST_SINK)
  {
    ret = CAP_Broadcast_StartBIGSync(BigHandle, SyncHandle, pBisIndex,NumBis, pGroup,
                                     SubgroupIndex, Encryption, BroadcastCode, MSE, Timeout);
    if (ret == BLE_STATUS_SUCCESS)
    {
      PBP_Context.PBKBIGSyncState = PBP_PBK_BIG_SYNC_STATE_SYNCHRONIZING;
    }
  }
#endif /* (BLE_CFG_PBP_PUBLIC_BROADCAST_SINK_ROLE == 1u) */
  return ret;
}

/**
 * @brief Stop the synchronization to a periodic advertising train
 * @param SyncHandle: The handle of the Periodic Advertising train
 * @retval status of the operation
 */
tBleStatus PBP_PBK_StopPASync(uint16_t SyncHandle)
{
  tBleStatus ret = HCI_COMMAND_DISALLOWED_ERR_CODE;

#if (BLE_CFG_PBP_PUBLIC_BROADCAST_SINK_ROLE == 1u)
  if (PBP_Context.Role & PBP_ROLE_PUBLIC_BROADCAST_SINK)
  {
    ret = CAP_Broadcast_StopPASync(SyncHandle);
    if (ret == BLE_STATUS_SUCCESS)
    {
      PBP_Context.PBKPASyncState = PBP_PBK_PA_SYNC_STATE_IDLE;
    }
  }
#endif /* (BLE_CFG_PBP_PUBLIC_BROADCAST_SINK_ROLE == 1u) */
  return ret;
}

/**
 * @brief Terminate the synchronization to a BIG
 * @param BigHandle: the handle of the BIG to terminate
 * @retval status of the operation
 */
tBleStatus PBP_PBK_StopBIGSync(uint8_t BigHandle)
{
  tBleStatus ret = HCI_COMMAND_DISALLOWED_ERR_CODE;

#if (BLE_CFG_PBP_PUBLIC_BROADCAST_SINK_ROLE == 1u)
  if (PBP_Context.Role & PBP_ROLE_PUBLIC_BROADCAST_SINK)
  {
    PBP_Context.WaitForAudioDown = 1u;
    ret = CAP_Broadcast_StopBIGSync(BigHandle);
    if (ret == BLE_STATUS_SUCCESS)
    {
      PBP_Context.PBKBIGSyncState = PBP_PBK_BIG_SYNC_STATE_IDLE;
    }
  }
#endif /* (BLE_CFG_PBP_PUBLIC_BROADCAST_SINK_ROLE == 1u) */
  return ret;
}

/**
 * @brief Start parsing Extended Advertising report to search for Scan Delagors solliciting assistance and Broadcast
 *        Sources on behalf a connected Scan Delegator
 * @param ConnHandle: The handle of the connection to the remote Scan Delegator. If no Scan Delegator is currently
 *                    connected, ignore this parameter
 * @retval status of the operation
*/
tBleStatus PBP_PBA_StartAdvReportParsing(uint16_t ConnHandle)
{
  tBleStatus ret = HCI_COMMAND_DISALLOWED_ERR_CODE;

#if (BLE_CFG_PBP_PUBLIC_BROADCAST_ASSISTANT_ROLE == 1u)
  if (PBP_Context.Role & PBP_ROLE_PUBLIC_BROADCAST_ASSISTANT
      && PBP_Context.PBAScanState == PBP_PBA_SCAN_STATE_IDLE)
  {
    ret = CAP_BroadcastAssistant_StartAdvReportParsing(ConnHandle);
  }
#endif /* (BLE_CFG_PBP_PUBLIC_BROADCAST_ASSISTANT_ROLE == 1u) */
  return ret;
}

/**
 * @brief Stop parsing Extended Advertising report to search for Scan Delagors solliciting assistance and Broadcast
 *        Sources on behalf a connected Scan Delegator
 * @param ConnHandle: The handle of the connection to the remote Scan Delegator. If no Scan Delegator is currently
 *                    connected, ignore this parameter
 * @retval status of the operation
*/
tBleStatus PBP_PBA_StopAdvReportParsing(uint16_t ConnHandle)
{
  tBleStatus ret = HCI_COMMAND_DISALLOWED_ERR_CODE;

#if (BLE_CFG_PBP_PUBLIC_BROADCAST_ASSISTANT_ROLE == 1u)
  if (PBP_Context.Role & PBP_ROLE_PUBLIC_BROADCAST_ASSISTANT
      && PBP_Context.PBAScanState == PBP_PBA_SCAN_STATE_SCANNING)
  {
    ret = CAP_BroadcastAssistant_StopAdvReportParsing(ConnHandle);
  }
#endif /* (BLE_CFG_PBP_PUBLIC_BROADCAST_ASSISTANT_ROLE == 1u) */
  return ret;
}

/**
  * @brief Perform the Audio Reception Start Procedure to request remote Public Broadcast Sinks to synchronize to a
  *        Broadcast Source
  * @param SetType: Set Type (Ad-Hoc or Coordinated Set)
  * @param NumAcceptors: Number of CAP Acceptors
  * @param pAudioReceptionStart: Table of CAP_Broadcast_AudioReceptionStart_Params_t structures containing
                                 details about the Broadcast Source for each CAP Acceptors
  * @retval status of the operation
  */
tBleStatus PBP_PBA_AudioReceptionStart(CAP_Set_Acceptors_t SetType,
                                       uint8_t NumAcceptors,
                                       CAP_Broadcast_AudioReceptionStart_Params_t *pAudioReceptionStart)
{
  tBleStatus ret = HCI_COMMAND_DISALLOWED_ERR_CODE;

#if (BLE_CFG_PBP_PUBLIC_BROADCAST_ASSISTANT_ROLE == 1u)
  if (PBP_Context.Role & PBP_ROLE_PUBLIC_BROADCAST_ASSISTANT)
  {
    ret = CAP_Broadcast_AudioReceptionStart(SetType, NumAcceptors, pAudioReceptionStart);
  }
#endif /* (BLE_CFG_PBP_PUBLIC_BROADCAST_ASSISTANT_ROLE == 1u) */
  return ret;
}

/**
  * @brief Perform the Audio Reception Stop Procedure to request remote Public Broadcast Sink(s) to desynchronize from
           a Broadcast Source
  * @param SetType: Set Type (Ad-Hoc or Coordinated Set)
  * @param NumAcceptors: Number of CAP Acceptors
  * @param pAudioReceptionStop: Table of CAP_Broadcast_AudioReceptionStop_Params_t structures containing
                                 details about the Broadcast Source for each CAP Acceptors
  * @retval status of the operation
  */
tBleStatus PBP_PBA_AudioReceptionStop(CAP_Set_Acceptors_t SetType,
                                      uint8_t NumAcceptors,
                                      CAP_Broadcast_AudioReceptionStop_Params_t *pAudioReceptionStop)
{
  tBleStatus ret = HCI_COMMAND_DISALLOWED_ERR_CODE;

#if (BLE_CFG_PBP_PUBLIC_BROADCAST_ASSISTANT_ROLE == 1u)
  if (PBP_Context.Role & PBP_ROLE_PUBLIC_BROADCAST_ASSISTANT)
  {
    ret = CAP_Broadcast_AudioReceptionStop(SetType, NumAcceptors, pAudioReceptionStop);
  }
#endif /* (BLE_CFG_PBP_PUBLIC_BROADCAST_ASSISTANT_ROLE == 1u) */
  return ret;
}

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
                               uint16_t SyncTimeout)
{
  tBleStatus ret = HCI_COMMAND_DISALLOWED_ERR_CODE;

#if (BLE_CFG_PBP_PUBLIC_BROADCAST_ASSISTANT_ROLE == 1u)
  if (PBP_Context.Role & PBP_ROLE_PUBLIC_BROADCAST_ASSISTANT)
  {
    ret = CAP_BroadcastAssistant_StartPASync(AdvSid, pAdvAddress, pAddressType, Skip, SyncTimeout);

    if (ret == BLE_STATUS_SUCCESS)
    {
      PBP_Context.PBAPASyncState = PBP_PBA_PA_SYNC_STATE_SYNCHRONIZING;
    }
  }
#endif /* (BLE_CFG_PBP_PUBLIC_BROADCAST_ASSISTANT_ROLE == 1u) */
  return ret;
}

/**
 * @brief Stop the synchronization to a periodic advertising train
 * @param SyncHandle: The handle of the Periodic Advertising train
 * @retval status of the operation
 */
tBleStatus PBP_PBA_StopPASync(uint16_t SyncHandle)
{
  tBleStatus ret = HCI_COMMAND_DISALLOWED_ERR_CODE;

#if (BLE_CFG_PBP_PUBLIC_BROADCAST_ASSISTANT_ROLE == 1u)
  if (PBP_Context.Role & PBP_ROLE_PUBLIC_BROADCAST_ASSISTANT)
  {
    ret = CAP_BroadcastAssistant_StopPASync(SyncHandle);
  }
#endif /* (BLE_CFG_PBP_PUBLIC_BROADCAST_ASSISTANT_ROLE == 1u) */
  return ret;
}

/**
  * @brief  Notify CAP Events
  * @param  pNotification: pointer on notification information
 */
void CAP_PBP_Notification(CAP_Notification_Evt_t *pNotification)
{
  switch(pNotification->EvtOpcode)
  {
    case CAP_BROADCAST_AUDIOSTARTED_EVT:
    {
      if (PBP_Context.PBSState == PBP_PBS_STATE_STARTING)
      {
        if (pNotification->Status == BLE_STATUS_SUCCESS)
        {
          PBP_Context.PBSState = PBP_PBS_STATE_STARTED;
        }
        else
        {
          PBP_Context.PBSState = PBP_PBS_STATE_IDLE;
        }
        PBP_Notification_Evt_t notif = {
            PBP_PBS_BROADCAST_AUDIO_STARTED_EVT,
            pNotification->Status,
            0,
            pNotification->pInfo
        };
        PBP_Notification(&notif);
      }
      break;
    }
    case CAP_BROADCAST_AUDIOSTOPPED_EVT:
    {
      if (PBP_Context.PBSState == PBP_PBS_STATE_STOPPING)
      {
        PBP_Notification_Evt_t notif = {
            PBP_PBS_BROADCAST_AUDIO_STOPPED_EVT,
            pNotification->Status,
            0,
            pNotification->pInfo
        };
        PBP_Notification(&notif);
      }
      break;
    }
    case CAP_BROADCAST_SOURCE_ADV_REPORT_EVT:
    case CAP_BA_BROADCAST_SOURCE_ADV_REPORT_EVT:
    {
      if ((PBP_Context.Role == PBP_ROLE_PUBLIC_BROADCAST_SINK
               && pNotification->EvtOpcode == CAP_BROADCAST_SOURCE_ADV_REPORT_EVT
                 && PBP_Context.PBKScanState == PBP_PBK_SCAN_STATE_SCANNING)
          || (PBP_Context.Role == PBP_ROLE_PUBLIC_BROADCAST_ASSISTANT
               && pNotification->EvtOpcode == CAP_BA_BROADCAST_SOURCE_ADV_REPORT_EVT
                 && PBP_Context.PBAScanState == PBP_PBA_SCAN_STATE_SCANNING))
      {
        uint8_t i = 0;
        uint8_t is_pbs = 0;
        BAP_Broadcast_Source_Adv_Report_Data_t *data = (BAP_Broadcast_Source_Adv_Report_Data_t *) pNotification->pInfo;
        PBP_Broadcast_Source_Adv_Report_Data_t info;

        /* Check if Broadcast Source contains the Public Broadcast Announcement Service */
        while (i + 1 < data->AdvertisingDataLength)
        {
          uint8_t length = data->pAdvertisingData[i];
          uint8_t type = data->pAdvertisingData[i+1];
          if (type == AD_TYPE_SERVICE_DATA
              && i+5 < data->AdvertisingDataLength
              && data->pAdvertisingData[i+2] == (PUBLIC_BROADCAST_ANNOUNCEMENT_SERVICE_UUID & 0xFF)
              && data->pAdvertisingData[i+3] == (PUBLIC_BROADCAST_ANNOUNCEMENT_SERVICE_UUID >> 8))
          {
            /* Found Public Broadcast Announcement Service */
            is_pbs = 1u;
            info.pBAPReport = data;
            info.Features = data->pAdvertisingData[i+4]
              & (PBP_FEATURES_ENCRYPTED | PBP_FEATURES_STANDARD_QUALITY | PBP_FEATURES_HIGH_QUALITY);
            info.MetadataLength = data->pAdvertisingData[i+5];
            if (i + 5 + info.MetadataLength >= data->AdvertisingDataLength)
            {
              /* Error in Advertising Data */
              return;
            }
            info.pMetadata = &(data->pAdvertisingData[i+6]);
          }
          else if (type == AD_TYPE_APPEARANCE
                   && i+3 < data->AdvertisingDataLength)
          {
            info.Appearance = data->pAdvertisingData[i+2] + (data->pAdvertisingData[i+3] << 8);
          }
          else if (type == AD_TYPE_BROADCAST_NAME
                   && i+2 < data->AdvertisingDataLength)
          {
            info.BroadcastNameLength = length - 1;
            if (i + 1 + info.BroadcastNameLength >= data->AdvertisingDataLength)
            {
              /* Error in Advertising Data */
              return;
            }
            info.pBroadcastName = &(data->pAdvertisingData[i+2]);
          }

          i+= length + 1;
        }

        if (is_pbs == 1u)
        {
          PBP_Notification_Evt_t notif = {
              PBP_PBK_BROADCAST_SOURCE_ADV_REPORT_EVT,
              pNotification->Status,
              0,
              (uint8_t *) &(info)
          };
          if (pNotification->EvtOpcode == CAP_BA_BROADCAST_SOURCE_ADV_REPORT_EVT)
          {
            notif.EvtOpcode = PBP_PBA_BROADCAST_SOURCE_ADV_REPORT_EVT;
          }
          PBP_Notification(&notif);
        }
      }
      break;
    }
    case CAP_BROADCAST_PA_SYNC_ESTABLISHED_EVT:
    {
      if (PBP_Context.PBKPASyncState == PBP_PBK_PA_SYNC_STATE_SYNCHRONIZING)
      {
        /* Event concerns a public broadcast source */
        PBP_Context.PBKPASyncState = PBP_PBK_PA_SYNC_STATE_SYNCHRONIZED;
        PBP_Notification_Evt_t notif = {
            PBP_PBK_PA_SYNC_ESTABLISHED_EVT,
            pNotification->Status,
            0,
            pNotification->pInfo
        };
        PBP_Notification(&notif);
      }
      break;
    }
    case CAP_BROADCAST_BASE_REPORT_EVT:
    {
      if (PBP_Context.PBKPASyncState == PBP_PBK_PA_SYNC_STATE_SYNCHRONIZED)
      {
        /* Event concerns a public broadcast source */
        PBP_Notification_Evt_t notif = {
            PBP_PBK_BASE_REPORT_EVT,
            pNotification->Status,
            0,
            pNotification->pInfo
        };
        PBP_Notification(&notif);
      }
      break;
    }
    case CAP_BROADCAST_BIGINFO_REPORT_EVT:
    {
      if (PBP_Context.PBKPASyncState == PBP_PBK_PA_SYNC_STATE_SYNCHRONIZED)
      {
        /* Event concerns a public broadcast source */
        PBP_Notification_Evt_t notif = {
            PBP_PBK_BIGINFO_REPORT_EVT,
            pNotification->Status,
            0,
            pNotification->pInfo
        };
        PBP_Notification(&notif);
      }
      break;
    }
    case CAP_BROADCAST_BIG_SYNC_ESTABLISHED_EVT:
    {
      if (PBP_Context.PBKBIGSyncState == PBP_PBK_BIG_SYNC_STATE_SYNCHRONIZING)
      {
        /* Event concerns a public broadcast source */
        PBP_Context.PBKBIGSyncState = PBP_PBK_BIG_SYNC_STATE_SYNCHRONIZED;
        PBP_Notification_Evt_t notif = {
            PBP_PBK_BIG_SYNC_ESTABLISHED_EVT,
            pNotification->Status,
            0,
            pNotification->pInfo
        };
        PBP_Notification(&notif);
      }
      break;
    }
    case CAP_BROADCAST_PA_SYNC_LOST_EVT:
    {
      if (PBP_Context.PBKPASyncState > PBP_PBK_PA_SYNC_STATE_IDLE)
      {
        /* Event concerns a public broadcast source */
        PBP_Context.PBKPASyncState = PBP_PBK_PA_SYNC_STATE_IDLE;
        PBP_Notification_Evt_t notif = {
            PBP_PBK_PA_SYNC_LOST_EVT,
            pNotification->Status,
            0,
            pNotification->pInfo
        };
        PBP_Notification(&notif);
      }
      break;
    }
    case CAP_BROADCAST_BIG_SYNC_LOST_EVT:
    {
      if (PBP_Context.PBKBIGSyncState > PBP_PBK_BIG_SYNC_STATE_IDLE)
      {
        /* Event concerns a public broadcast source */
        PBP_Context.PBKBIGSyncState = PBP_PBK_BIG_SYNC_STATE_IDLE;
        PBP_Context.WaitForAudioDown = 1u;
        PBP_Notification_Evt_t notif = {
            PBP_PBK_BIG_SYNC_LOST_EVT,
            pNotification->Status,
            0,
            pNotification->pInfo
        };
        PBP_Notification(&notif);
      }
      break;
    }
    case CAP_BROADCAST_AUDIO_UP_EVT:
    {
      if (PBP_Context.PBKBIGSyncState > PBP_PBK_BIG_SYNC_STATE_IDLE
          || PBP_Context.PBSState > PBP_PBS_STATE_IDLE)
      {
        /* Event concerns a public broadcast source */
        PBP_Notification_Evt_t notif = {
            PBP_BROADCAST_AUDIO_UP_EVT,
            pNotification->Status,
            0,
            pNotification->pInfo
        };
        PBP_Notification(&notif);
      }
      break;
    }
    case CAP_BROADCAST_AUDIO_DOWN_EVT:
    {
      if (PBP_Context.WaitForAudioDown == 1u)
      {
        /* Event concerns a public broadcast source */
        PBP_Context.WaitForAudioDown = 0u;
        PBP_Notification_Evt_t notif = {
            PBP_BROADCAST_AUDIO_DOWN_EVT,
            pNotification->Status,
            0,
            0
        };
        PBP_Notification(&notif);

        PBP_Context.PBSState = PBP_PBS_STATE_IDLE;
      }
      break;
    }
    case CAP_BA_REM_BROADCAST_RECEIVE_STATE_INFO_EVT:
    {
      if (PBP_Context.Role & PBP_ROLE_PUBLIC_BROADCAST_ASSISTANT)
      {
        PBP_Notification_Evt_t notif = {
            PBP_PBA_REM_BROADCAST_RECEIVE_STATE_INFO_EVT,
            pNotification->Status,
            pNotification->ConnHandle,
            pNotification->pInfo
        };
        PBP_Notification(&notif);
      }
      break;
    }
    case CAP_BA_AUDIO_RECEPTION_STARTED_EVT:
    {
      if (PBP_Context.Role & PBP_ROLE_PUBLIC_BROADCAST_ASSISTANT)
      {
        PBP_Notification_Evt_t notif = {
            PBP_PBA_AUDIO_RECEPTION_STARTED_EVT,
            pNotification->Status,
            pNotification->ConnHandle,
            0
        };
        PBP_Notification(&notif);
      }
      break;
    }
    case CAP_BA_AUDIO_RECEPTION_STOPPED_EVT:
    {
      if (PBP_Context.Role & PBP_ROLE_PUBLIC_BROADCAST_ASSISTANT)
      {
        PBP_Notification_Evt_t notif = {
            PBP_PBA_AUDIO_RECEPTION_STOPPED_EVT,
            pNotification->Status,
            pNotification->ConnHandle,
            0
        };
        PBP_Notification(&notif);
      }
      break;
    }
    case CAP_BA_REM_REMOVED_SOURCE_EVT:
    {
      if (PBP_Context.Role & PBP_ROLE_PUBLIC_BROADCAST_ASSISTANT)
      {
        PBP_Notification_Evt_t notif = {
            PBP_PBA_REM_REMOVED_SOURCE_EVT,
            pNotification->Status,
            pNotification->ConnHandle,
            pNotification->pInfo
        };
        PBP_Notification(&notif);
      }
      break;
    }
    case CAP_BA_SOLICITING_SCAN_DELEGATOR_REPORT_EVT:
    {
      if (PBP_Context.Role & PBP_ROLE_PUBLIC_BROADCAST_ASSISTANT)
      {
        PBP_Notification_Evt_t notif = {
            PBP_PBA_SOLICITING_SCAN_DELEGATOR_REPORT_EVT,
            pNotification->Status,
            pNotification->ConnHandle,
            pNotification->pInfo
        };
        PBP_Notification(&notif);
      }
      break;
    }
    case CAP_BA_PA_SYNC_ESTABLISHED_EVT:
    {
      if (PBP_Context.Role & PBP_ROLE_PUBLIC_BROADCAST_ASSISTANT
          && PBP_Context.PBAPASyncState == PBP_PBA_PA_SYNC_STATE_SYNCHRONIZING)
      {
        PBP_Context.PBAPASyncState = PBP_PBA_PA_SYNC_STATE_SYNCHRONIZED;
        PBP_Notification_Evt_t notif = {
            PBP_PBA_PA_SYNC_ESTABLISHED_EVT,
            pNotification->Status,
            0,
            pNotification->pInfo
        };
        PBP_Notification(&notif);
      }
    break;
    }
    case CAP_BA_PA_SYNC_LOST_EVT:
    {
      if (PBP_Context.Role & PBP_ROLE_PUBLIC_BROADCAST_ASSISTANT
          && PBP_Context.PBAPASyncState > PBP_PBA_PA_SYNC_STATE_IDLE)
      {
        PBP_Context.PBAPASyncState = PBP_PBA_PA_SYNC_STATE_IDLE;
        PBP_Notification_Evt_t notif = {
            PBP_PBA_PA_SYNC_LOST_EVT,
            pNotification->Status,
            0,
            pNotification->pInfo
        };
        PBP_Notification(&notif);
      }
      break;
    }
    case CAP_BA_BASE_REPORT_EVT:
    {
      if (PBP_Context.Role & PBP_ROLE_PUBLIC_BROADCAST_ASSISTANT
          && PBP_Context.PBAPASyncState == PBP_PBA_PA_SYNC_STATE_SYNCHRONIZING)
      {
        PBP_Notification_Evt_t notif = {
            PBP_PBA_BASE_REPORT_EVT,
            pNotification->Status,
            0,
            pNotification->pInfo
        };
        PBP_Notification(&notif);
      }
      break;
    }
    case CAP_BA_BIGINFO_REPORT_EVT:
    {
      if (PBP_Context.Role & PBP_ROLE_PUBLIC_BROADCAST_ASSISTANT
          && PBP_Context.PBAPASyncState == PBP_PBA_PA_SYNC_STATE_SYNCHRONIZING)
      {
        PBP_Notification_Evt_t notif = {
            PBP_PBA_BIGINFO_REPORT_EVT,
            pNotification->Status,
            0,
            pNotification->pInfo
        };
        PBP_Notification(&notif);
      }
      break;
    }
    default:
    {
      /* Ignore event */
      break;
    }
  }
}


/* Private functions ----------------------------------------------------------*/
