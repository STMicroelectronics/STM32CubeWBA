/**
  ******************************************************************************
  * @file    ble_audio_if.c
  * @author  MCD Application Team
  * @brief   This file implements the interface functions for BLE Audio Profiles
  *          of the Generic Audio Framework
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#include "app_conf.h"
#include "svc_ctl.h"
#include "main.h"
/* Defines ------------------------------------------------------------------*/
/**
  * @brief  Configuration Defines tell the linker if the subpart of the BLE Generic Audio Framework must be linked,
  *         Values can be set to 0 if feature is not needed, allowing FLASH footprint optimization
  */
#if APP_BAP_ROLE_UNICAST_SERVER_SUPPORT == 1
#define LINK_BAP_UNICAST_SERVER         1
#else
#define LINK_BAP_UNICAST_SERVER         0
#endif /* APP_BAP_ROLE_UNICAST_SERVER_SUPPORT */

#if APP_BAP_ROLE_UNICAST_CLIENT_SUPPORT == 1
#define LINK_BAP_UNICAST_CLIENT         1
#else
#define LINK_BAP_UNICAST_CLIENT         0
#endif /* APP_BAP_ROLE_UNICAST_CLIENT_SUPPORT */

#if APP_BAP_ROLE_BROADCAST_SOURCE_SUPPORT == 1
#define LINK_BAP_BROADCAST_SOURCE       1
#else
#define LINK_BAP_BROADCAST_SOURCE       0
#endif /* APP_BAP_ROLE_BROADCAST_SOURCE_SUPPORT */

#if APP_BAP_ROLE_BROADCAST_SINK_SUPPORT == 1
#define LINK_BAP_BROADCAST_SINK         1
#else
#define LINK_BAP_BROADCAST_SINK         0
#endif /* APP_BAP_ROLE_BROADCAST_SINK_SUPPORT */

#if APP_BAP_ROLE_SCAN_DELEGATOR_SUPPORT == 1
#define LINK_BAP_SCAN_DELEGATOR         1
#else
#define LINK_BAP_SCAN_DELEGATOR         0
#endif /* APP_BAP_ROLE_SCAN_DELEGATOR_SUPPORT */

#if APP_BAP_ROLE_BROADCAST_ASSISTANT_SUPPORT == 1
#define LINK_BAP_BROADCAST_ASSISTANT    1
#else
#define LINK_BAP_BROADCAST_ASSISTANT    0
#endif /* APP_BAP_ROLE_BROADCAST_ASSISTANT_SUPPORT */



#if APP_CCP_ROLE_SERVER_SUPPORT == 1
#define LINK_CCP_SERVER                 1
#else
#define LINK_CCP_SERVER                 0
#endif /* APP_CCP_ROLE_SERVER_SUPPORT */

#if APP_CCP_ROLE_CLIENT_SUPPORT == 1
#define LINK_CCP_CLIENT                 1
#else
#define LINK_CCP_CLIENT                 0
#endif /* APP_CCP_ROLE_CLIENT_SUPPORT */


#if APP_MCP_ROLE_SERVER_SUPPORT == 1
#define LINK_MCP_SERVER                 1
#else
#define LINK_MCP_SERVER                 0
#endif /* APP_MCP_ROLE_SERVER_SUPPORT */

#if APP_MCP_ROLE_CLIENT_SUPPORT == 1
#define LINK_MCP_CLIENT                 1
#else
#define LINK_MCP_CLIENT                 0
#endif /* APP_MCP_ROLE_CLIENT_SUPPORT */

#if APP_CSIP_ROLE_SET_COORDINATOR_SUPPORT == 1
#define LINK_CSIP_SET_COORDINATOR       1
#else
#define LINK_CSIP_SET_COORDINATOR       0
#endif /* APP_CSIP_ROLE_SET_COORDINATOR_SUPPORT */

#if APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1
#define LINK_CSIP_SET_MEMBER            1
#else
#define LINK_CSIP_SET_MEMBER            0
#endif /* APP_CSIP_ROLE_SET_MEMBER_SUPPORT */

#if APP_VCP_ROLE_CONTROLLER_SUPPORT == 1
#define LINK_VCP_CONTROLLER             1
#else
#define LINK_VCP_CONTROLLER             0
#endif /* APP_VCP_ROLE_CONTROLLER_SUPPORT */

#if APP_VCP_ROLE_RENDERER_SUPPORT == 1
#define LINK_VCP_RENDERER               1
#else
#define LINK_VCP_RENDERER               0
#endif /* APP_VCP_ROLE_RENDERER_SUPPORT */

#if APP_MICP_ROLE_CONTROLLER_SUPPORT == 1
#define LINK_MICP_CONTROLLER            1
#else
#define LINK_MICP_CONTROLLER            0
#endif /* APP_MICP_ROLE_CONTROLLER_SUPPORT */

#if APP_MICP_ROLE_DEVICE_SUPPORT == 1
#define LINK_MICP_DEVICE                1
#else
#define LINK_MICP_DEVICE                0
#endif /* APP_MICP_ROLE_DEVICE_SUPPORT */

/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions prototype------------------------------------------------*/

/* Private user code ---------------------------------------------------------*/
#if (LINK_BAP_UNICAST_SERVER == 0)
/**
  * @brief This function is used to execute BAP Unicast Server Task in the BAP Profile.
  */
void BAP_USR_Task(void)
{
  /*should not be reached*/
  Error_Handler();
}

/**
  * @brief This function is used by the Unicast Server in the BAP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */

SVCCTL_EvtAckStatus_t BAP_USR_GattEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
  return SVCCTL_EvtNotAck;
}
#endif /* (LINK_BAP_UNICAST_SERVER == 0) */

#if (LINK_BAP_UNICAST_CLIENT == 0)
/**
  * @brief This function is used by the Unicast Client in the BAP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */
SVCCTL_EvtAckStatus_t BAP_UCL_GattEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
  return SVCCTL_EvtNotAck;
}

/**
  * @brief This function is used by the CAP Initiator in the CAP Profile to handle Unicast Client Events
  *        received from the BAP Profile.
  * @param  Event: Address of the buffer holding the Event
  */
void CAP_INITIATOR_UnicastEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
}
#endif /* (LINK_BAP_UNICAST_CLIENT == 0) */


#if ((LINK_BAP_UNICAST_SERVER == 0) && (LINK_BAP_UNICAST_CLIENT == 0))
/**
  * @brief This function is used by the Unicast Server or the Unicast Client in the BAP Profile to handle HCI Events
  *        received from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  */
void BAP_UNICAST_HciEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
}
#endif /*((LINK_BAP_UNICAST_SERVER == 0) && (LINK_BAP_UNICAST_CLIENT == 0))*/

#if (LINK_BAP_BROADCAST_SOURCE == 0)
/**
  * @brief This function is used by the Broadcast Source in the BAP Profile to handle HCI Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  */
void BAP_BSRC_HciEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
}
#endif /* (LINK_BAP_BROADCAST_SOURCE == 0) */

#if (LINK_BAP_BROADCAST_SINK == 0)
/**
  * @brief This function is used by the Broadcast Sink in the BAP Profile to handle HCI Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  */
void BAP_BSNK_HciEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
}
#endif /* (LINK_BAP_BROADCAST_SINK == 0) */



#if (LINK_BAP_SCAN_DELEGATOR == 0)
/** @brief This function is used by the Scan Delegator in the BAP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */
SVCCTL_EvtAckStatus_t BAP_SDE_GattEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
  return SVCCTL_EvtNotAck;
}

/** @brief This function is used by the Scan Delegator in the BAP Profile to handle HCI Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  */
void BAP_SDE_HciEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
}
#endif /* (LINK_BAP_SCAN_DELEGATOR == 0) */


#if (LINK_BAP_BROADCAST_ASSISTANT == 0)
/** @brief This function is used by the Broadcast Assistant in the BAP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */
SVCCTL_EvtAckStatus_t BAP_BA_GattEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
  return SVCCTL_EvtNotAck;
}

/** @brief This function is used by the Broadcast Assistant in the BAP Profile to handle HCI Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  */
void BAP_BA_HciEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
}
#endif /* (LINK_BAP_BROADCAST_ASSISTANT == 0) */


#if (LINK_CCP_SERVER == 0u)
/**
  * @brief This function is used to execute Call Control Server Task in the CCP Profile.
  */
void CCP_SERVER_Task(void)
{
  /*should not be reached*/
  Error_Handler();
}

/** @brief This function is used by the Call Control Server in the CCP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */
SVCCTL_EvtAckStatus_t CCP_SERVER_GattEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
  return SVCCTL_EvtNotAck;
}

/** @brief This function is used by the Call Control Server in the CCP Profile to handle HCI Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  */
void CCP_SERVER_HciEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
}
#endif /*(LINK_CCP_SERVER == 0u)*/

#if (LINK_CCP_CLIENT == 0u)

/** @brief This function is used by the Call Control Client in the CCP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */
SVCCTL_EvtAckStatus_t CCP_CLIENT_GattEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
  return SVCCTL_EvtNotAck;
}

/** @brief This function is used by the Call Control Client in the CCP Profile to handle HCI Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  */
void CCP_CLIENT_HciEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
}
#endif /*(LINK_CCP_CLIENT == 0u)*/


#if (LINK_MCP_SERVER == 0u)
/**
  * @brief This function is used to execute Media Control Server Task in the MCP Profile.
  */
void MCP_SERVER_Task(void)
{
  /*should not be reached*/
  Error_Handler();
}

/** @brief This function is used by the Media Control Server in the MCP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */
SVCCTL_EvtAckStatus_t MCP_SERVER_GattEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  return SVCCTL_EvtNotAck;
}

/** @brief This function is used by the Media Control Server in the MCP Profile to handle HCI Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  */
void MCP_SERVER_HciEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
}
#endif /*(LINK_MCP_SERVER == 0u)*/

#if (LINK_MCP_CLIENT == 0u)
/** @brief This function is used by the Media Control Client in the MCP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */
SVCCTL_EvtAckStatus_t MCP_CLIENT_GattEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
  return SVCCTL_EvtNotAck;
}

/** @brief This function is used by the Media Control Client in the MCP Profile to handle HCI Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  */
void MCP_CLIENT_HciEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
}
#endif /*(LINK_MCP_CLIENT == 0u)*/

#if (LINK_CSIP_SET_MEMBER == 0u)
/**
  * @brief This function is used to execute Set Member Task in the CSIP Profile.
  */
void CSIP_SET_MEMBER_Task(void)
{
  /*should not be reached*/
  Error_Handler();
}

/** @brief This function is used by the Set Member in the CSIP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */
SVCCTL_EvtAckStatus_t CSIP_SET_MEMBER_GattEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
  return SVCCTL_EvtNotAck;
}

/** @brief This function is used by the Set Member in the CSIP Profile to handle HCI Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  */
void CSIP_SET_MEMBER_HciEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
}
#endif /*(LINK_CSIP_SET_MEMBER == 0u)*/

#if (LINK_CSIP_SET_COORDINATOR == 0u)

/** @brief This function is used by the Set Coordinator in the CSIP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */
SVCCTL_EvtAckStatus_t CSIP_SET_COORDINATOR_GattEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
  return SVCCTL_EvtNotAck;
}

/** @brief This function is used by the Set Coordinator in the CSIP Profile to handle HCI Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  */
void CSIP_SET_COORDINATOR_HciEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
}

/** @brief This function is used by the CAP Initiator or the CAP Commander in the CAP Profile to handle
  *        CSIP Set Coordinator Events receive from the CSIP Profile.
  * @param  Event: Address of the buffer holding the Event
  */
void CAP_SetCoordinatorEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
}
#endif /*(LINK_CSIP_SET_COORDINATOR == 0u)*/


#if (LINK_VCP_RENDERER == 0u)
/**
  * @brief This function is used to execute Renderer Task in the VCP Profile.
  */
void VCP_RENDERER_Task(void)
{
  /*should not be reached*/
  Error_Handler();
}

/** @brief This function is used by the Renderer in the VCP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */
SVCCTL_EvtAckStatus_t VCP_RENDERER_GattEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
  return SVCCTL_EvtNotAck;
}
#endif /*(LINK_VCP_RENDERER == 0u)*/
#if (LINK_VCP_CONTROLLER == 0u)
/**
  * @brief This function is used to execute Controller Task in the VCP Profile.
  */
void VCP_CONTROLLER_Task(void)
{
  /*should not be reached*/
  Error_Handler();
}

/** @brief This function is used by the Controller in the VCP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */
SVCCTL_EvtAckStatus_t VCP_CONTROLLER_GattEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
  return SVCCTL_EvtNotAck;
}

/** @brief This function is used by the Controller in the VCP Profile to handle HCI Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  */
void VCP_CONTROLLER_HciEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
}
#endif /*(LINK_VCP_CONTROLLER == 0u)*/

#if (LINK_MICP_DEVICE == 0u)
/**
  * @brief This function is used to execute Device Task in the MICP Profile.
  */
void MICP_DEVICE_Task(void)
{
  /*should not be reached*/
  Error_Handler();
}

/** @brief This function is used by the Device in the MICP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */
SVCCTL_EvtAckStatus_t MICP_DEVICE_GattEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
  return SVCCTL_EvtNotAck;
}
#endif /*(LINK_MICP_DEVICE == 0u)*/
#if (LINK_MICP_CONTROLLER == 0u)
/**
  * @brief This function is used to execute Controller Task in the MICP Profile.
  */
void MICP_CONTROLLER_Task(void)
{
  /*should not be reached*/
  Error_Handler();
}

/** @brief This function is used by the Controller in the MICP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */
SVCCTL_EvtAckStatus_t MICP_CONTROLLER_GattEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
  return SVCCTL_EvtNotAck;
}

/** @brief This function is used by the Controller in the MICP Profile to handle HCI Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  */
void MICP_CONTROLLER_HciEventHandler(void *pEvent)
{
  UNUSED(pEvent);
  /*should not be reached*/
  Error_Handler();
}
#endif /*(LINK_MICP_CONTROLLER == 0u)*/
