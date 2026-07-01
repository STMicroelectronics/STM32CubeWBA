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
/* Exported functions --------------------------------------------------------*/
void CAP_COMMANDER_FunctionTable_Init(void);
void CAP_INITIATOR_FunctionTable_Init(void);
void CAP_ACCEPTOR_FunctionTable_Init(void);

void BAP_USR_FunctionTable_Init(void);
void BAP_UCL_FunctionTable_Init(void);
void BAP_BSRC_FunctionTable_Init(void);
void BAP_BSNK_FunctionTable_Init(void);
void BAP_SDE_FunctionTable_Init(void);
void BAP_BA_FunctionTable_Init(void);

void ASCS_SERVER_FunctionTable_Init(void);
void ASCS_CLIENT_FunctionTable_Init(void);
void PACS_SERVER_FunctionTable_Init(void);
void PACS_CLIENT_FunctionTable_Init(void);
void BASS_SERVER_FunctionTable_Init(void);
void BASS_CLIENT_FunctionTable_Init(void);

void CCP_SERVER_FunctionTable_Init(void);
void CCP_CLIENT_FunctionTable_Init(void);
void MCP_SERVER_FunctionTable_Init(void);
void MCP_CLIENT_FunctionTable_Init(void);
void VCP_CONTROLLER_FunctionTable_Init(void);
void VCP_RENDERER_FunctionTable_Init(void);
void CSIP_SET_MEMBER_FunctionTable_Init(void);
void CSIP_SET_COORDINATOR_FunctionTable_Init(void);
void MICP_DEVICE_FunctionTable_Init(void);
void MICP_CONTROLLER_FunctionTable_Init(void);

void BLE_Audio_LinkerError(void);

/* Private user code ---------------------------------------------------------*/
#if (LINK_BAP_UNICAST_SERVER == 0)
void BAP_USR_FunctionTable_Init(void)
{

}

void ASCS_SERVER_FunctionTable_Init(void)
{

}
#endif /* (LINK_BAP_UNICAST_SERVER == 0) */


#if (LINK_BAP_UNICAST_CLIENT == 0)
void BAP_UCL_FunctionTable_Init(void)
{

}

void ASCS_CLIENT_FunctionTable_Init(void)
{

}
#endif /* (LINK_BAP_UNICAST_CLIENT == 0) */


#if (LINK_BAP_BROADCAST_SOURCE == 0)
void BAP_BSRC_FunctionTable_Init(void)
{

}
#endif /* (LINK_BAP_BROADCAST_SOURCE == 0) */


#if (LINK_BAP_BROADCAST_SINK == 0)
void BAP_BSNK_FunctionTable_Init(void)
{

}
#endif /* (LINK_BAP_BROADCAST_SINK == 0) */


#if (LINK_BAP_SCAN_DELEGATOR == 0)
void BAP_SDE_FunctionTable_Init(void)
{

}

void BASS_SERVER_FunctionTable_Init(void)
{

}
#endif /* (LINK_BAP_SCAN_DELEGATOR == 0) */


#if (LINK_BAP_BROADCAST_ASSISTANT == 0)
void BAP_BA_FunctionTable_Init(void)
{
}

void BASS_CLIENT_FunctionTable_Init(void)
{
}
#endif /* (LINK_BAP_BROADCAST_ASSISTANT == 0) */


#if ((LINK_BAP_BROADCAST_ASSISTANT == 0) && (LINK_BAP_UNICAST_CLIENT == 0))
void PACS_CLIENT_FunctionTable_Init(void)
{
}
#endif


#if ((LINK_BAP_SCAN_DELEGATOR == 0) && (LINK_BAP_UNICAST_SERVER == 0))
void PACS_SERVER_FunctionTable_Init(void)
{
}
#endif


#if (LINK_CCP_SERVER == 0u)
void CCP_SERVER_FunctionTable_Init(void)
{
}
#endif /*(LINK_CCP_SERVER == 0u)*/


#if (LINK_CCP_CLIENT == 0u)
void CCP_CLIENT_FunctionTable_Init(void)
{
}
#endif /*(LINK_CCP_CLIENT == 0u)*/


#if (LINK_MCP_SERVER == 0u)
void MCP_SERVER_FunctionTable_Init(void)
{
}
#endif /*(LINK_MCP_SERVER == 0u)*/


#if (LINK_MCP_CLIENT == 0u)
void MCP_CLIENT_FunctionTable_Init(void)
{
}
#endif /*(LINK_MCP_CLIENT == 0u)*/


#if (LINK_CSIP_SET_MEMBER == 0u)
void CSIP_SET_MEMBER_FunctionTable_Init(void)
{
}
#endif /*(LINK_CSIP_SET_MEMBER == 0u)*/


#if (LINK_CSIP_SET_COORDINATOR == 0u)
void CSIP_SET_COORDINATOR_FunctionTable_Init(void)
{
}
#endif /*(LINK_CSIP_SET_COORDINATOR == 0u)*/


#if (LINK_VCP_RENDERER == 0u)
void VCP_RENDERER_FunctionTable_Init(void)
{
}
#endif /*(LINK_VCP_RENDERER == 0u)*/


#if (LINK_VCP_CONTROLLER == 0u)
void VCP_CONTROLLER_FunctionTable_Init(void)
{
}
#endif /*(LINK_VCP_CONTROLLER == 0u)*/


#if (LINK_MICP_DEVICE == 0u)
void MICP_DEVICE_FunctionTable_Init(void)
{
}
#endif /*(LINK_MICP_DEVICE == 0u)*/


#if (LINK_MICP_CONTROLLER == 0u)
void MICP_CONTROLLER_FunctionTable_Init(void)
{
}
#endif /*(LINK_MICP_CONTROLLER == 0u)*/


#if ((APP_CAP_ROLE & CAP_ROLE_COMMANDER) == 0)
void CAP_COMMANDER_FunctionTable_Init(void)
{

}
#endif /* ((APP_CAP_ROLE & CAP_ROLE_COMMANDER) == 0) */


#if ((APP_CAP_ROLE & CAP_ROLE_ACCEPTOR) == 0)
void CAP_ACCEPTOR_FunctionTable_Init(void)
{

}
#endif /*  ((APP_CAP_ROLE & CAP_ROLE_ACCEPTOR) == 0) */


#if ((APP_CAP_ROLE & CAP_ROLE_INITATOR) == 0)
void CAP_INITIATOR_FunctionTable_Init(void)
{

}
#endif /*  ((APP_CAP_ROLE & CAP_ROLE_INITATOR) == 0)*/


void BLE_Audio_LinkerError(void)
{
  Error_Handler();
}