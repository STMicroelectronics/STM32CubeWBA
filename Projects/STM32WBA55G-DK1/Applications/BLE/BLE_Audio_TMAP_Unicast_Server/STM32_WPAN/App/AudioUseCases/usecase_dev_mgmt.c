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
#include "svc_ctl.h"

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
/* External functions prototype-----------------------------------------------*/
extern void TMAP_AclDisconnection(uint16_t ConnHandle);

/* Functions Definition ------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/
/**
  * @brief Use Case Manager initialization.
  * @note  This function shall be called before any Use Case Profile function
  */
tBleStatus USECASE_DEV_MGMT_Init()
{
  uint8_t i;
  /* Clear audio stack variables */
  memset( &USECASE_DEV_MGMT_Context, 0, sizeof(USECASE_DEV_MGMT_Context_t) );

  USECASE_DEV_MGMT_Context.NumConnectedLinks = 0u;
  for (i = 0u; i < USECASE_DEV_MGMT_MAX_CONNECTION; i++)
  {
    USECASE_DEV_MGMT_Context.pConnInfo[i].Connection_Handle = 0xFFFFu;
  }
#if (USECASE_DEV_MGMT_MAX_CONNECTION > 0u)
  /**
   *  Register the hci event handler to the BLE controller
   */
  SVCCTL_RegisterHandler(UDM_HCI_Event_Handler);
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
__WEAK void TMAP_AclDisconnection(uint16_t ConnHandle)
{
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

          USECASE_DEV_MGMT_Context.NumConnectedLinks--;
          USECASE_DEV_MGMT_Context.pConnInfo[i].Connection_Handle = 0xFFFFu;

          return return_value;
        }
      }
    }
    break; /* HCI_DISCONNECTION_COMPLETE_EVT_CODE */

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