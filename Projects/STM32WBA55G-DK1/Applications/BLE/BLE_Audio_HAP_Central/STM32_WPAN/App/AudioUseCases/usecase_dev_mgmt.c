/**
  ******************************************************************************
  * @file    usecase_dev_mgmt.c
  * @author  MCD Application Team
  * @brief   This file contains Device Management for Use Case Profiles
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
#include "usecase_dev_mgmt.h"
#include "ble_vs_codes.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  uint8_t               NumConnectedLinks;
  UseCaseConnInfo_t     pConnInfo[USECASE_DEV_MGMT_MAX_CONNECTION];
}USECASE_DEV_MGMT_Context_t;

/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static USECASE_DEV_MGMT_Context_t  USECASE_DEV_MGMT_Context = {0};

/* Private functions prototype------------------------------------------------*/
static SVCCTL_EvtAckStatus_t UDM_HCI_Event_Handler(void *pEvent);
static SVCCTL_EvtAckStatus_t UDM_GATT_Event_Handler(void *pEvent);
/* External functions prototype-----------------------------------------------*/

/* Functions Definition ------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/
/**
  * @brief Use Case Manager initialization.
  * @note  This function shall be called before any Use Case Profile function
  */
tBleStatus USECASE_DEV_MGMT_Init(void)
{
  uint8_t i;
  /* Clear audio stack variables */
  memset( &USECASE_DEV_MGMT_Context, 0, sizeof(USECASE_DEV_MGMT_Context_t) );

  USECASE_DEV_MGMT_Context.NumConnectedLinks = 0u;
  for (i = 0u; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
  {
    USECASE_DEV_MGMT_Context.pConnInfo[i].Connection_Handle = 0xFFFFu;
    USECASE_DEV_MGMT_Context.pConnInfo[i].CSIPDiscovered = 0;
  }
#if (USECASE_DEV_MGMT_MAX_CONNECTION > 0u)
  /**
   *  Register the hci event handler to the BLE controller
   */
  SVCCTL_RegisterHandler(UDM_HCI_Event_Handler);

  /**
  *	Register the event handler to the BLE controller
  */
  SVCCTL_RegisterSvcHandler(UDM_GATT_Event_Handler);
#endif /* (USECASE_DEV_MGMT_MAX_CONNECTION > 0u) */

  return BLE_STATUS_SUCCESS;
}

/**
  * @brief  Get the number of connected devices
  * @retval number of connected devices
  */
uint8_t USECASE_DEV_MGMT_GetNumConnectedDevices(void)
{
  return USECASE_DEV_MGMT_Context.NumConnectedLinks;
}

/**
  * @brief  Get Connection Information corresponding to a specified connection handle
  * @param  ConnHandle: connection handle
  * @param  pConnInfo : pointer on connection information
  * @retval Status of the operation
  */
tBleStatus USECASE_DEV_MGMT_GetConnInfo(uint16_t ConnHandle,const UseCaseConnInfo_t **pConnInfo)
{
  uint8_t i;
  for (i = 0u; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
  {
    if (USECASE_DEV_MGMT_Context.pConnInfo[i].Connection_Handle == ConnHandle)
    {
      *pConnInfo = &USECASE_DEV_MGMT_Context.pConnInfo[i];
      return BLE_STATUS_SUCCESS;
    }
  }
  return BLE_STATUS_FAILED;
}

/**
  * @brief  Get Connection Handle corresponding to a specified connection index
  * @param  ConnIndex: connection index
  * @retval Connection Handle (0xFFFF if connection index doesn't correspond to a connected device)
  */
uint16_t USECASE_DEV_MGMT_GetConnHandle(uint8_t ConnIndex)
{
  uint8_t i;
  uint8_t index = 0;
  for (i = 0u; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
  {
    if (USECASE_DEV_MGMT_Context.pConnInfo[i].Connection_Handle != 0xFFFFu)
    {
      if (index == ConnIndex)
      {
        return USECASE_DEV_MGMT_Context.pConnInfo[i].Connection_Handle;
      }
      index++;
    }
  }
  return 0xFFFFu;
}

/**
  * @brief Set CSIP information for ConnHandle
  * @param ConnHandle: Connection Handle of the set member
  * @param pSIRK: Pointer to the SIRC of the set
  * @param SIRKType: Type of SIRC of the set
  * @param Rank: Rank of the set member
  * @param SetSize: Size of the set member
  */
void USECASE_DEV_MGMT_SetCSIPInfo(uint16_t ConnHandle, uint8_t *pSIRK, uint8_t SIRKType, uint8_t Rank, uint8_t SetSize)
{
  uint8_t i;
  for (i = 0u; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
  {
    if (USECASE_DEV_MGMT_Context.pConnInfo[i].Connection_Handle == ConnHandle)
    {
      USECASE_DEV_MGMT_Context.pConnInfo[i].CSIPDiscovered = 1u;
      memcpy(&USECASE_DEV_MGMT_Context.pConnInfo[i].aSIRK[0], pSIRK, 16);
      USECASE_DEV_MGMT_Context.pConnInfo[i].SIRK_type = SIRKType;
      USECASE_DEV_MGMT_Context.pConnInfo[i].Rank = Rank;
      USECASE_DEV_MGMT_Context.pConnInfo[i].Size = SetSize;
      break;
    }
  }
}

__WEAK void TMAP_AclDisconnection(uint16_t ConnHandle)
{
}

__WEAK void HAP_AclDisconnection(uint16_t ConnHandle)
{
}

__WEAK void GMAP_AclDisconnection(uint16_t ConnHandle)
{
}

__WEAK void TMAP_LinkEncrypted(uint16_t ConnHandle)
{
}

__WEAK void HAP_LinkEncrypted(uint16_t ConnHandle)
{
}

__WEAK void GMAP_LinkEncrypted(uint16_t ConnHandle)
{
}

__WEAK SVCCTL_EvtAckStatus_t TMAP_GATT_Event_Handler(void *pEvent)
{
  return SVCCTL_EvtNotAck;
}

__WEAK SVCCTL_EvtAckStatus_t HAP_GATT_Event_Handler(void *pEvent)
{
  return SVCCTL_EvtNotAck;
}

__WEAK SVCCTL_EvtAckStatus_t GMAP_GATT_Event_Handler(void *pEvent)
{
  return SVCCTL_EvtNotAck;
}

/* Private functions ----------------------------------------------------------*/
static SVCCTL_EvtAckStatus_t UDM_HCI_Event_Handler(void *pEvent)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *p_event_pckt;
  evt_le_meta_event *p_meta_evt;
  uint8_t i;

  return_value = SVCCTL_EvtNotAck;
  p_event_pckt = (hci_event_pckt *)(((hci_uart_pckt*)pEvent)->data);

  switch (p_event_pckt->evt)
  {
    case HCI_DISCONNECTION_COMPLETE_EVT_CODE:
    {
      hci_disconnection_complete_event_rp0 *p_disconnection_complete_event = (hci_disconnection_complete_event_rp0 *) p_event_pckt->data;
      /* Clear the Connection Information corresponding to the disconnected remote device*/
      for (i = 0u; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
      {
        if (USECASE_DEV_MGMT_Context.pConnInfo[i].Connection_Handle == p_disconnection_complete_event->Connection_Handle)
        {
          TMAP_AclDisconnection(p_disconnection_complete_event->Connection_Handle);
          HAP_AclDisconnection(p_disconnection_complete_event->Connection_Handle);
          GMAP_AclDisconnection(p_disconnection_complete_event->Connection_Handle);

          USECASE_DEV_MGMT_Context.NumConnectedLinks--;
          USECASE_DEV_MGMT_Context.pConnInfo[i].Connection_Handle = 0xFFFFu;

          return return_value;
        }
      }
    }
    break; /* HCI_DISCONNECTION_COMPLETE_EVT_CODE */

    case HCI_ENCRYPTION_CHANGE_EVT_CODE:
    {
      hci_encryption_change_event_rp0 *enc_change_event = (hci_encryption_change_event_rp0 *) p_event_pckt->data;
      if ((enc_change_event->Status == BLE_STATUS_SUCCESS) && (enc_change_event->Encryption_Enabled == 0x01))
      {
        uint8_t i;
        for (i = 0u; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
        {
          if (USECASE_DEV_MGMT_Context.pConnInfo[i].Connection_Handle == enc_change_event->Connection_Handle)
          {
              USECASE_DEV_MGMT_Context.pConnInfo[i].LinkEncrypted = 1u;
              break;
          }
        }

        TMAP_LinkEncrypted(enc_change_event->Connection_Handle);
        HAP_LinkEncrypted(enc_change_event->Connection_Handle);
        GMAP_LinkEncrypted(enc_change_event->Connection_Handle);
      }
    }
    break;

    case HCI_LE_META_EVT_CODE:
    {
      p_meta_evt = (evt_le_meta_event*) p_event_pckt->data;
      /* USER CODE BEGIN EVT_LE_META_EVENT */

      /* USER CODE END EVT_LE_META_EVENT */
      switch (p_meta_evt->subevent)
      {
        case HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE:
        {
          hci_le_connection_complete_event_rp0 *p_connection_complete_event = (hci_le_connection_complete_event_rp0 *) p_meta_evt->data;
          if (p_connection_complete_event->Status == 0)
          {
            /*Allocate ConnInfo slot for this ACL Connection*/
            for (i = 0u; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
            {
              if (USECASE_DEV_MGMT_Context.pConnInfo[i].Connection_Handle == 0xFFFFu)
              {
                USECASE_DEV_MGMT_Context.NumConnectedLinks++;
                USECASE_DEV_MGMT_Context.pConnInfo[i].Connection_Handle = p_connection_complete_event->Connection_Handle;
                USECASE_DEV_MGMT_Context.pConnInfo[i].Role = p_connection_complete_event->Role;
                USECASE_DEV_MGMT_Context.pConnInfo[i].Peer_Address_Type = p_connection_complete_event->Peer_Address_Type;
                memcpy(&USECASE_DEV_MGMT_Context.pConnInfo[i].Peer_Address[0],&p_connection_complete_event->Peer_Address[0],6u);
                USECASE_DEV_MGMT_Context.pConnInfo[i].MTU = 0;
                USECASE_DEV_MGMT_Context.pConnInfo[i].CSIPDiscovered = 0u;
                break;
              }
            }
          }
        }
        break;

        case HCI_LE_ENHANCED_CONNECTION_COMPLETE_SUBEVT_CODE:
        {
          hci_le_enhanced_connection_complete_event_rp0 *p_connection_complete_event;
          p_connection_complete_event = (hci_le_enhanced_connection_complete_event_rp0 *) p_meta_evt->data;
          if (p_connection_complete_event->Status == 0)
          {
            /*Allocate ConnInfo slot for this ACL Connection*/
            for (i = 0u; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
            {
              if (USECASE_DEV_MGMT_Context.pConnInfo[i].Connection_Handle == 0xFFFFu)
              {
                USECASE_DEV_MGMT_Context.NumConnectedLinks++;
                USECASE_DEV_MGMT_Context.pConnInfo[i].Connection_Handle = p_connection_complete_event->Connection_Handle;
                USECASE_DEV_MGMT_Context.pConnInfo[i].Role = p_connection_complete_event->Role;
                USECASE_DEV_MGMT_Context.pConnInfo[i].Peer_Address_Type = p_connection_complete_event->Peer_Address_Type;
                memcpy(&USECASE_DEV_MGMT_Context.pConnInfo[i].Peer_Address[0],&p_connection_complete_event->Peer_Address[0],6u);
                USECASE_DEV_MGMT_Context.pConnInfo[i].MTU = 0;
                USECASE_DEV_MGMT_Context.pConnInfo[i].LinkEncrypted = 0;
                USECASE_DEV_MGMT_Context.pConnInfo[i].CSIPDiscovered = 0u;
                break;
              }
            }
          }
        }
        break;

      default:
        break;
      }
    }
    break;

    default:
      break;
  }
  return return_value;
}

static SVCCTL_EvtAckStatus_t UDM_GATT_Event_Handler(void *pEvent)
{
  SVCCTL_EvtAckStatus_t return_value = SVCCTL_EvtNotAck;

  hci_event_pckt *p_event_pckt;
  evt_blecore_aci *p_blecore_evt;

  p_event_pckt = (hci_event_pckt *)(((hci_uart_pckt*)pEvent)->data);

  switch (p_event_pckt->evt)
  {
    case HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE:
    {
      p_blecore_evt = (evt_blecore_aci*)p_event_pckt->data;
      switch (p_blecore_evt->ecode)
      {
        case ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE:
        {
          aci_att_exchange_mtu_resp_event_rp0 *mtu_resp_event = (void*)p_blecore_evt->data;
          uint8_t i;
          for (i = 0u; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
          {
            if (USECASE_DEV_MGMT_Context.pConnInfo[i].Connection_Handle == mtu_resp_event->Connection_Handle)
            {
                USECASE_DEV_MGMT_Context.pConnInfo[i].MTU = mtu_resp_event->Server_RX_MTU;
                break;
            }
          }
          break;
        }
      }
      break;
    }
  }

  return_value = TMAP_GATT_Event_Handler(pEvent);

  if (return_value == SVCCTL_EvtNotAck)
  {
    return_value = HAP_GATT_Event_Handler(pEvent);
  }
  if (return_value == SVCCTL_EvtNotAck)
  {
    return_value = GMAP_GATT_Event_Handler(pEvent);
  }

  return return_value;
}