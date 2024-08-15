/**
  ******************************************************************************
  * @file    hap.c
  * @author  MCD Application Team
  * @brief   This file contains Hearing Access Profile feature
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
#include "hap_alloc.h"
#include "hap_ha.h"
#include "hap_harc.h"
#include "hap_iac.h"
#include "svc_ctl.h"
#include "usecase_dev_mgmt.h"
#include "ble_gap_aci.h"
#include "hap_log.h"
#include "hap_db.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions prototype------------------------------------------------*/

/* External functions prototype------------------------------------------------*/

/* Functions Definition ------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/
/**
  * @brief  Initialize the Hearing Access Profile
  * @param  pConfig: Pointer to the HAP Config structure
  * @retval status of the initialization
  */
tBleStatus HAP_Init(HAP_Role_t Role)
{
  tBleStatus status = BLE_STATUS_SUCCESS;

  HAP_Context.Role = Role;

  if (HAP_Context.Role & HAP_ROLE_HEARING_AID)
  {
#if (BLE_CFG_HAP_HA_ROLE == 1u)
    status = HAP_HA_Init();
#else /* (BLE_CFG_HAP_HA_ROLE == 1u) */
    status = HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */
  }

  if ((status == BLE_STATUS_SUCCESS) && (HAP_Context.Role & HAP_ROLE_HEARING_AID_REMOTE_CONTROLLER))
  {
#if (BLE_CFG_HAP_HARC_ROLE == 1u)
    status = HAP_HARC_Init();
#else /* (BLE_CFG_HAP_HARC_ROLE == 1u) */
    status = HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */
  }

  if ((status == BLE_STATUS_SUCCESS) && (HAP_Context.Role & HAP_ROLE_IMMEDIATE_ALERT_CLIENT))
  {
#if (BLE_CFG_HAP_IAC_ROLE == 1u)
    status = HAP_IAC_Init();
#else /* (BLE_CFG_HAP_IAC_ROLE == 1u) */
    status = HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif /* (BLE_CFG_HAP_IAC_ROLE == 1u) */
  }

  return status;
}

/**
  * @brief Link Up the HAP Client roles with remote HAP HA
  * @param  ConnHandle: Connection handle
  * @param  LinkupMode: LinkUp Mode
  * @note HAP_LINKUP_COMPLETE_EVT event will be generated once process is complete
  * @retval status of the operation
  */
tBleStatus HAP_Linkup(uint16_t ConnHandle, HAP_LinkupMode_t LinkupMode)
{
  tBleStatus    hciCmdResult = HCI_COMMAND_DISALLOWED_ERR_CODE;

#if ((BLE_CFG_HAP_IAC_ROLE == 1u) && (BLE_CFG_HAP_HARC_ROLE == 1u))
  HAP_Context.LinkupMode = LinkupMode;
#endif /* ((BLE_CFG_HAP_IAC_ROLE == 1u) && (BLE_CFG_HAP_HARC_ROLE == 1u)) */

#if (BLE_CFG_HAP_HARC_ROLE == 1u)
  if (HAP_Context.Role & HAP_ROLE_HEARING_AID_REMOTE_CONTROLLER)
  {
    hciCmdResult = HAP_HARC_Linkup(ConnHandle, LinkupMode);
  }
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */

#if (BLE_CFG_HAP_IAC_ROLE == 1u)
  if ((hciCmdResult == HCI_COMMAND_DISALLOWED_ERR_CODE) && (HAP_Context.Role & HAP_ROLE_IMMEDIATE_ALERT_CLIENT))
  {
    hciCmdResult = HAP_IAC_Linkup(ConnHandle, LinkupMode);
  }
#endif /* (BLE_CFG_HAP_IAC_ROLE == 1u) */

  return hciCmdResult;
}

/** @brief This function is used by the Device in the HAP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */
SVCCTL_EvtAckStatus_t HAP_GATT_Event_Handler(void *pEvent)
{
  SVCCTL_EvtAckStatus_t return_value = SVCCTL_EvtNotAck;

#if (BLE_CFG_HAP_HA_ROLE == 1u)
  if ((HAP_Context.Role & HAP_ROLE_HEARING_AID) == HAP_ROLE_HEARING_AID)
  {
    return_value = HAP_HA_GATT_Event_Handler(pEvent);
  }
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */

#if (BLE_CFG_HAP_HARC_ROLE == 1u)
  if (return_value == SVCCTL_EvtNotAck)
  {
    if ((HAP_Context.Role  & HAP_ROLE_HEARING_AID_REMOTE_CONTROLLER) == HAP_ROLE_HEARING_AID_REMOTE_CONTROLLER)
    {
      return_value = HAP_HARC_GATT_Event_Handler(pEvent);
    }
  }
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */

#if (BLE_CFG_HAP_IAC_ROLE == 1u)
  if (return_value == SVCCTL_EvtNotAck)
  {
    if ((HAP_Context.Role  & HAP_ROLE_IMMEDIATE_ALERT_CLIENT) == HAP_ROLE_IMMEDIATE_ALERT_CLIENT)
    {
      return_value = HAP_IAC_GATT_Event_Handler(pEvent);
    }
  }
#endif /* (BLE_CFG_HAP_IAC_ROLE == 1u) */
  return return_value;
}

/**
  * @brief Indicate that connection with specified device is down
  * @param  ConnHandle: connection handle
  */
void HAP_AclDisconnection(uint16_t ConnHandle)
{
#if ((BLE_CFG_HAP_HA_ROLE == 1u) || (BLE_CFG_HAP_HARC_ROLE == 1u) || (BLE_CFG_HAP_IAC_ROLE == 1u))
  if (HAP_Context.Role & HAP_ROLE_HEARING_AID
      || HAP_Context.Role & HAP_ROLE_HEARING_AID_REMOTE_CONTROLLER
        || HAP_Context.Role & HAP_ROLE_IMMEDIATE_ALERT_CLIENT)
  {
    const UseCaseConnInfo_t *p_conn_info;

    if (USECASE_DEV_MGMT_GetConnInfo(ConnHandle,&p_conn_info) == BLE_STATUS_SUCCESS)
    {
      /*Check if the remote device is bonded in order to save PACS and ASCS characteristics values*/
      if (aci_gap_is_device_bonded( p_conn_info->Peer_Address_Type,
                                    &p_conn_info->Peer_Address[0]) == BLE_STATUS_SUCCESS)
      {
        BLE_DBG_HAP_MSG("Peer Device 0x%02x%02x%02x%02x%02x%02x is bonded, store HAP information values in NVM\n",
                        p_conn_info->Peer_Address[5],
                        p_conn_info->Peer_Address[4],
                        p_conn_info->Peer_Address[3],
                        p_conn_info->Peer_Address[2],
                        p_conn_info->Peer_Address[1],
                        p_conn_info->Peer_Address[0]);
        /*Store HAP information values in NVM*/
        HAP_DB_StoreServices(ConnHandle,p_conn_info->Peer_Address_Type,&p_conn_info->Peer_Address[0]);
      }
      else
      {
        BLE_DBG_HAP_MSG("Peer Device 0x%02x%02x%02x%02x%02x%02x is not bonded\n",p_conn_info->Peer_Address[5],
                                                                                  p_conn_info->Peer_Address[4],
                                                                                  p_conn_info->Peer_Address[3],
                                                                                  p_conn_info->Peer_Address[2],
                                                                                  p_conn_info->Peer_Address[1],
                                                                                  p_conn_info->Peer_Address[0]);
      }
    }
  }
#endif /* ((BLE_CFG_HAP_HA_ROLE == 1u) || (BLE_CFG_HAP_HARC_ROLE == 1u) || (BLE_CFG_HAP_IAC_ROLE == 1u)) */

#if (BLE_CFG_HAP_HARC_ROLE == 1u)
  if (HAP_Context.Role & HAP_ROLE_HEARING_AID_REMOTE_CONTROLLER)
  {
    HAP_HARC_AclDisconnection(ConnHandle);
  }
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */

#if (BLE_CFG_HAP_IAC_ROLE == 1u)
  if (HAP_Context.Role & HAP_ROLE_IMMEDIATE_ALERT_CLIENT)
  {
    HAP_IAC_AclDisconnection(ConnHandle);
  }
#endif /* (BLE_CFG_HAP_IAC_ROLE == 1u) */
}

void HAP_LinkEncrypted(uint16_t ConnHandle)
{
#if (BLE_CFG_HAP_HARC_ROLE == 1u)
  if (HAP_Context.Role & HAP_ROLE_HEARING_AID_REMOTE_CONTROLLER)
  {
    HAP_HARC_LinkEncrypted(ConnHandle);
  }
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */

#if (BLE_CFG_HAP_HA_ROLE == 1u)
  if (HAP_Context.Role & HAP_ROLE_HEARING_AID)
  {
    HAP_HA_LinkEncrypted(ConnHandle);
  }
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */
}

/**
  * @brief  Notify CAP Events
  * @param  pNotification: pointer on notification information
 */
void CAP_HAP_Notification(CAP_Notification_Evt_t *pNotification)
{
#if (BLE_CFG_HAP_HARC_ROLE == 1u)
  if (pNotification->EvtOpcode == CAP_CSI_LINKUP_EVT)
  {
    CSIP_SetMember_Info_t *p_info = (CSIP_SetMember_Info_t *) pNotification->pInfo;

    USECASE_DEV_MGMT_SetCSIPInfo(pNotification->ConnHandle, p_info->pSetIdentityResolvingKey,
                                 p_info->SetIdentityResolvingKeyType, p_info->SetMemberRank,
                                 p_info->CoordinatedSetSize);
  }
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */
}

#if (BLE_CFG_HAP_IAC_ROLE == 1u)
/**
 * @brief  Notify HAP IAC Event
 * @param  pNotification: pointer on notification information
 */
void HAP_IAC_Notification(HAP_IAC_NotificationEvt_t const *pNotification)
{
  switch (pNotification->EvtOpcode)
  {
    case IAC_LINKUP_COMPLETE_EVT:
    {
      HAP_Notification_Evt_t evt;
      evt.ConnHandle = pNotification->ConnHandle;

      /* Notify HAP IAC Linkup Complete */
      evt.EvtOpcode = HAP_IAC_LINKUP_COMPLETE_EVT;
      evt.Status = pNotification->Status;
      evt.pInfo = pNotification->pInfo;
      HAP_Notification(&evt);

      /* Notify HAP Linkup Complete */
      evt.EvtOpcode = HAP_LINKUP_COMPLETE_EVT;
      evt.Status = BLE_STATUS_SUCCESS;
      evt.pInfo = 0;
      HAP_Notification(&evt);
      break;
    }

    default:
      break;
  }
}
#endif /* (BLE_CFG_HAP_IAC_ROLE == 1u) */

#if (BLE_CFG_HAP_HARC_ROLE == 1u)
/**
 * @brief  Notify HAP HARC Event
 * @param  pNotification: pointer on notification information
 */
void HAP_HARC_Notification(HAP_HARC_NotificationEvt_t const *pNotification)
{
  switch (pNotification->EvtOpcode)
  {
    case HARC_LINKUP_COMPLETE_EVT:
    {
      HAP_Notification_Evt_t evt;
      evt.ConnHandle = pNotification->ConnHandle;

      /* Notify HAP IAC Linkup Complete */
      evt.EvtOpcode = HAP_IAC_LINKUP_COMPLETE_EVT;
      evt.Status = pNotification->Status;
      evt.pInfo = pNotification->pInfo;
      HAP_Notification(&evt);

#if (BLE_CFG_HAP_IAC_ROLE == 1u)
      if (HAP_Context.Role & HAP_ROLE_IMMEDIATE_ALERT_CLIENT)
      {
        tBleStatus status;
        status = HAP_IAC_Linkup(pNotification->ConnHandle, HAP_Context.LinkupMode);

        if (status != BLE_STATUS_SUCCESS)
        {
          /* Notify HAP IAC Linkup Complete */
          evt.EvtOpcode = HAP_IAC_LINKUP_COMPLETE_EVT;
          evt.Status = status;
          evt.pInfo = pNotification->pInfo;
          HAP_Notification(&evt);
        }
      }
      else
      {
        /* Notify HAP Linkup Complete */
        evt.EvtOpcode = HAP_LINKUP_COMPLETE_EVT;
        evt.Status = BLE_STATUS_SUCCESS;
        evt.pInfo = 0;
        HAP_Notification(&evt);
      }
#else /* (BLE_CFG_HAP_IAC_ROLE == 1u) */
      /* Notify HAP Linkup Complete */
      evt.EvtOpcode = HAP_LINKUP_COMPLETE_EVT;
      evt.Status = BLE_STATUS_SUCCESS;
      evt.pInfo = 0;
      HAP_Notification(&evt);
#endif /* (BLE_CFG_HAP_IAC_ROLE == 1u) */
      break;
    }

    default:
    {
      HAP_Notification_Evt_t evt;
      evt.EvtOpcode = (HAP_NotCode_t) (((uint8_t) pNotification->EvtOpcode) + 1);
      evt.Status = pNotification->Status;
      evt.pInfo = pNotification->pInfo;
      evt.ConnHandle = pNotification->ConnHandle;
      HAP_Notification(&evt);
      break;
    }
  }
}
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */
/* Private functions ----------------------------------------------------------*/