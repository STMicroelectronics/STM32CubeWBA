
/**
  ******************************************************************************
  * @file    ghs_cp.h
  * @author  MCD Application Team
  * @brief   Header for ghs_cp.c module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GHS_CP_H
#define __GHS_CP_H

#ifdef __cplusplus
extern "C" 
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "ghs.h"
#include "ghs_app.h" 

/* Exported defines-----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
  
typedef __PACKED_STRUCT
{
  uint8_t RequestOpCode;
  uint8_t ResponseCode;
} GHS_CP_GeneralResponse_t;

typedef __PACKED_STRUCT
{
  uint16_t OpCode;
} GHS_CP_Procedure_t;

typedef __PACKED_STRUCT
{
  uint8_t OpCode;
} GHS_CP_Response_t;

typedef enum
{
  GHS_CP_START_SENDING_LIVE_OBSERVATIONS_EVENT,
  GHS_CP_STOP_SENDING_LIVE_OBSERVATIONS_EVENT
} GHS_CP_App_EventCode_t;

typedef struct
{
  GHS_CP_App_EventCode_t      EventCode;
  uint16_t                    ConnectionHandle;
  uint8_t                     ServiceInstance;
} GHS_CP_App_Event_t;
/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void GHS_CP_Init(void);
void GHS_CP_RequestHandler(uint8_t * pRequestData, uint8_t requestDataLength);
void GHS_CP_AcknowledgeHandler(void);
uint8_t GHS_CP_CheckRequestValid(uint8_t * pRequestData, uint8_t requestDataLength);
void GHS_CP_InitSingleActiveBolus(void);
void GHS_CP_SetNoActiveBasalRateDelivery(void);
uint8_t GHS_CP_APP_EventHandler(GHS_CP_App_Event_t * pNotification);

#endif /* __GHS_CP_H */
