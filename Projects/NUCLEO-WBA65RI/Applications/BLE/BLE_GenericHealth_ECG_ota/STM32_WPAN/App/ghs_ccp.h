
/**
  ******************************************************************************
  * @file    ghs_ccp.h
  * @author  MCD Application Team
  * @brief   Header for ghs_ccp.c module
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
#ifndef __GHS_CCP_H
#define __GHS_CCP_H

#ifdef __cplusplus
extern "C" 
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "ghs.h"
#include "ghs_app.h" 

/* Exported defines-----------------------------------------------------------*/
#define GHS_CCP_OP_CODE_OPERAND_MAX_LENGTH                                  (15)
/* Exported types ------------------------------------------------------------*/
typedef enum
{
  GHS_CCP_STATE_IDLE,
  GHS_CCP_STATE_PROCEDURE_IN_PROGRESS,
  GHS_CCP_STATE_PROCEDURE_FINISHED,
  GHS_CCP_STATE_PROCEDURE_RESPONSE_PENDING,
  GHS_CCP_STATE_WAITING_FOR_RESPONSE_ACKNOWLEDGE
} GHS_CCP_State_t;

typedef __PACKED_STRUCT
{
  uint16_t OpCode;
  uint8_t Operand[GHS_CCP_OP_CODE_OPERAND_MAX_LENGTH];
} GHS_CCP_Procedure_t;

typedef __PACKED_STRUCT
{
  uint8_t OpCode;
  uint8_t Operand[GHS_CCP_OP_CODE_OPERAND_MAX_LENGTH];
} GHS_CCP_Response_t;

/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void GHS_CCP_Init(void);
void GHS_CCP_RequestHandler(uint8_t * pRequestData, uint8_t requestDataLength);
void GHS_CCP_AcknowledgeHandler(void);
uint8_t GHS_CCP_CheckRequestValid(uint8_t * pRequestData, uint8_t requestDataLength);
//uint8_t GHS_CCP_APP_EventHandler(GHS_CCP_App_Event_t * pNotification);

#endif /* __GHS_CCP_H */
