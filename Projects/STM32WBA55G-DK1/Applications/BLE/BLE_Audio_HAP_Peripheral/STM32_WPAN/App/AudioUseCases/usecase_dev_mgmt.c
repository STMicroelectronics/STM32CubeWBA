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
#include "ble_audio_stack.h"
#include "ble_vs_codes.h"
#include "ble_gatt_aci.h"
/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  uint8_t               NumConnectedLinks;
#if (USECASE_DEV_MGMT_MAX_CONNECTION > 0u)
  UseCaseConnInfo_t     pConnInfo[USECASE_DEV_MGMT_MAX_CONNECTION];
#endif /* (USECASE_DEV_MGMT_MAX_CONNECTION > 0u) */
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
#if (USECASE_DEV_MGMT_MAX_CONNECTION > 0u)
  for (i = 0u; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
  {
    USECASE_DEV_MGMT_Context.pConnInfo[i].Connection_Handle = 0xFFFFu;
    USECASE_DEV_MGMT_Context.pConnInfo[i].CSIPDiscovered = 0;
#if (CFG_BLE_EATT_BEARER_PER_LINK > 0)
    /* Initialize the EATT Bearer Instance associated to each ACL Link */
    for (uint8_t j = 0u; j < CFG_BLE_EATT_BEARER_PER_LINK; j++)
    {
      USECASE_DEV_MGMT_Context.pConnInfo[i].aEATTBearer[j].State = 1u;
    }
#endif /* (CFG_BLE_EATT_BEARER_PER_LINK > 0) */
  }
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
  * @brief  Get ACL Connection Information and potential EATT Bearer Information corresponding to a
  *         specified connection handle
  * @param  ConnHandle: connection handle
  * @param  pConnInfo : pointer on ACL connection information
  * @param  pEATTBearer : pointer on EATT Bearer information
  * @retval Status of the operation
  */
tBleStatus USECASE_DEV_MGMT_GetConnInfo(uint16_t ConnHandle,
                                        UseCaseConnInfo_t **pConnInfo,
                                        BleEATTBearer_t **pEATTBearer)
{
#if (USECASE_DEV_MGMT_MAX_CONNECTION > 0u)
  uint8_t i;
  for (i = 0u; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
  {
    if (USECASE_DEV_MGMT_Context.pConnInfo[i].Connection_Handle == ConnHandle)
    {
      *pConnInfo = &USECASE_DEV_MGMT_Context.pConnInfo[i];
      *pEATTBearer = 0;
      return BLE_STATUS_SUCCESS;
    }
  }
#if (CFG_BLE_EATT_BEARER_PER_LINK > 0)
  /*Check if the connection handle corresponds to a Connection Oriented Channel*/
  if ((uint8_t) (ConnHandle >> 8) == 0xEA)
  {
    for (i = 0u; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
    {
      if (USECASE_DEV_MGMT_Context.pConnInfo[i].Connection_Handle != 0xFFFF)
      {
        for (uint8_t j = 0u; j < CFG_BLE_EATT_BEARER_PER_LINK; j++)
        {
          if ((USECASE_DEV_MGMT_Context.pConnInfo[i].aEATTBearer[j].State == 0u)  \
              && (USECASE_DEV_MGMT_Context.pConnInfo[i].aEATTBearer[j].ChannelIdx == (uint8_t)(ConnHandle)))
          {
            *pConnInfo = &USECASE_DEV_MGMT_Context.pConnInfo[i];
            *pEATTBearer = &USECASE_DEV_MGMT_Context.pConnInfo[i].aEATTBearer[j];
            return BLE_STATUS_SUCCESS;
          }
        }
      }
    }
  }
#endif /* (CFG_BLE_EATT_BEARER_PER_LINK > 0u) */
#endif /* (USECASE_DEV_MGMT_MAX_CONNECTION > 0u) */
  return BLE_STATUS_FAILED;
}

/**
  * @brief  Get Connection Handle corresponding to a specified connection index
  * @param  ConnIndex: connection index
  * @retval Connection Handle (0xFFFF if connection index doesn't correspond to a connected device)
  */
uint16_t USECASE_DEV_MGMT_GetConnHandle(uint8_t ConnIndex)
{
#if (USECASE_DEV_MGMT_MAX_CONNECTION > 0u)
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
#endif /* (USECASE_DEV_MGMT_MAX_CONNECTION > 0u) */
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
#if (USECASE_DEV_MGMT_MAX_CONNECTION > 0u)
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
#endif /* (USECASE_DEV_MGMT_MAX_CONNECTION > 0u) */
}

/**
 * @brief Update characteristic and indicate selectively the generation of Indication/Notification over ATT Bearer
 *        or EATT Bearer to all subscribed clients
 * @param Service_Handle Handle of service to which the characteristic belongs
 * @param Char_Handle Handle of the characteristic declaration
 * @param Update_Type Allow Notification or Indication generation, if enabled
 *        in the client characteristic configuration descriptor
 *        Flags:
 *        - 0x00: Do not notify
 *        - 0x01: Notification
 *        - 0x02: Indication
 * @param Char_Length Total length of the characteristic value.
 *        In case of a variable size characteristic, this field specifies the
 *        new length of the characteristic value after the update; in case of
 *        fixed length characteristic this field is ignored.
 * @param Value_Offset The offset from which the attribute value has to be
 *        updated.
 * @param Value_Length Length of the Value parameter in octets.
 * @param Value Updated characteristic value
 * @return Value indicating success or error code.
 */
uint8_t USECASE_DEV_MGMT_UpdateCharValue(uint16_t Service_Handle,
                                         uint16_t Char_Handle,
                                         uint8_t Update_Type,
                                         uint16_t Char_Length,
                                         uint16_t Value_Offset,
                                         uint8_t Value_Length,
                                         const uint8_t* Value )
{
#if (USECASE_DEV_MGMT_MAX_CONNECTION > 0u)
  tBleStatus            hciCmdResult = BLE_STATUS_SUCCESS;

#if (CFG_BLE_EATT_BEARER_PER_LINK > 0)
  tBleStatus            ret;
  uint16_t              conn_handle;

  if ((USECASE_DEV_MGMT_Context.NumConnectedLinks > 0) && (Update_Type != 0))
  {
    for (uint8_t i = 0u; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
    {
      if (USECASE_DEV_MGMT_Context.pConnInfo[i].Connection_Handle != 0xFFFFu)
      {
        conn_handle = USECASE_DEV_MGMT_Context.pConnInfo[i].Connection_Handle;
        if ((BLE_AUDIO_STACK_EATT_GetNumSubscribedBearers(USECASE_DEV_MGMT_Context.pConnInfo[i].Connection_Handle) > 0))
        {
          for (uint8_t j = 0u; j < CFG_BLE_EATT_BEARER_PER_LINK; j++)
          {
            if ((USECASE_DEV_MGMT_Context.pConnInfo[i].aEATTBearer[j].State == 0u) \
                && (BLE_AUDIO_STACK_EATT_IsSubscribedBearer(USECASE_DEV_MGMT_Context.pConnInfo[i].aEATTBearer[j].ChannelIdx) == BLE_STATUS_SUCCESS))
            {
              conn_handle = (0xEA << 8) | (USECASE_DEV_MGMT_Context.pConnInfo[i].aEATTBearer[j].ChannelIdx);
              break;
            }
          }
        }
        ret = aci_gatt_update_char_value_ext(conn_handle,
                                              Service_Handle,
                                              Char_Handle,
                                              Update_Type,
                                              Char_Length,
                                              Value_Offset,
                                              Value_Length,
                                              Value);
        hciCmdResult |= ret;
      }
    }
  }
  else
#endif /* (CFG_BLE_EATT_BEARER_PER_LINK > 0u) */
  {
    hciCmdResult = aci_gatt_update_char_value_ext(0x0000,
                                                  Service_Handle,
                                                  Char_Handle,
                                                  Update_Type,
                                                  Char_Length,
                                                  Value_Offset,
                                                  Value_Length,
                                                  Value);
  }
#else /* (USECASE_DEV_MGMT_MAX_CONNECTION > 0u) */
  hciCmdResult = aci_gatt_update_char_value_ext(0x0000,
                                              Service_Handle,
                                              Char_Handle,
                                              0x0000u,
                                              Char_Length,
                                              Value_Offset,
                                              Value_Length,
                                              Value);
#endif /* (USECASE_DEV_MGMT_MAX_CONNECTION > 0u) */
  return hciCmdResult;
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

#if (USECASE_DEV_MGMT_MAX_CONNECTION > 0u)

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
#endif /* (USECASE_DEV_MGMT_MAX_CONNECTION > 0u) */
  return return_value;
}

static SVCCTL_EvtAckStatus_t UDM_GATT_Event_Handler(void *pEvent)
{
  SVCCTL_EvtAckStatus_t return_value = SVCCTL_EvtNotAck;

#if (USECASE_DEV_MGMT_MAX_CONNECTION > 0u)
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
#if (CFG_BLE_EATT_BEARER_PER_LINK > 0)
        case ACI_GATT_EATT_BEARER_VSEVT_CODE:
        {
          aci_gatt_eatt_bearer_event_rp0 *p_eatt_bearer_event;
          p_eatt_bearer_event = (aci_gatt_eatt_bearer_event_rp0*)p_blecore_evt->data;
          for (uint8_t i = 0u; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
          {
            if (USECASE_DEV_MGMT_Context.pConnInfo[i].Connection_Handle == p_eatt_bearer_event->Connection_Handle)
            {
              for (uint8_t j = 0u; j < CFG_BLE_EATT_BEARER_PER_LINK; j++)
              {
                if (p_eatt_bearer_event->EAB_State == 0u)
                {
                  /* EATT is connected*/
                  if (USECASE_DEV_MGMT_Context.pConnInfo[i].aEATTBearer[j].State == 1u)
                  {
                    USECASE_DEV_MGMT_Context.pConnInfo[i].aEATTBearer[j].State = p_eatt_bearer_event->EAB_State;
                    USECASE_DEV_MGMT_Context.pConnInfo[i].aEATTBearer[j].ChannelIdx = p_eatt_bearer_event->Channel_Index;
                    break;
                  }
                }
                else
                {
                  /* EATT is discconnected*/
                  if ((USECASE_DEV_MGMT_Context.pConnInfo[i].aEATTBearer[j].State == 0u) \
                      && (USECASE_DEV_MGMT_Context.pConnInfo[i].aEATTBearer[j].ChannelIdx == p_eatt_bearer_event->Channel_Index))
                  {
                    USECASE_DEV_MGMT_Context.pConnInfo[i].aEATTBearer[j].State = p_eatt_bearer_event->EAB_State;
                    USECASE_DEV_MGMT_Context.pConnInfo[i].aEATTBearer[j].ChannelIdx = 0x00u;
                    break;
                  }
                }
              }
              break;
            }
          }
          break;
        }
#endif /* (CFG_BLE_EATT_BEARER_PER_LINK > 0) */
      }
      break;
    }
  }
#endif /* (USECASE_DEV_MGMT_MAX_CONNECTION > 0u) */

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