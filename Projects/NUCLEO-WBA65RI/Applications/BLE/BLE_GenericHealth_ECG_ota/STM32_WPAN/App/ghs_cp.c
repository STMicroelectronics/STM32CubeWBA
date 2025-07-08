/**
  ******************************************************************************
  * @file    ghs_cp.c
  * @author  MCD Application Team
  * @brief   GHS CP
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

/* Includes ------------------------------------------------------------------*/
#include "log_module.h"
#include "stm32_wpan_common.h"
#include "stm32_seq.h"
#include "stm32_timer.h"
#include "ghs.h"
#include "ghs_cp.h"
#include "ghs_app.h"

/******************************************************************************
* Macro Declarations
******************************************************************************/
/* Private typedef -----------------------------------------------------------*/  
/* GHS Control Point: Op Code Values */
#define GHS_CP_START_SENDING_LIVE_OBSERVATIONS                            0x01
#define GHS_CP_STOP_SENDING_LIVE_OBSERVATIONS                             0x02
#define GHS_CP_SUCCESS                                                    0x80
 
/* GHS Specific Ops Control Point: fields position */
#define GHS_CP_OP_CODE_LENGTH                                              (1) 
#define GHS_CP_OP_CODE_POSITION                                            (0)

/* GHS Specific Ops Control Point: responses values */
#define GHS_CP_ERROR_CODE_SUCCESS                                       (0x0F)
#define GHS_CP_ERROR_CODE_OPCODE_NOT_SUPPORTED                          (0x70)
#define GHS_CP_ERROR_CODE_PROCEDURE_NOT_COMPLETED                       (0x72)
#define GHS_CP_ERROR_CODE_PROCEDURE_NOT_APPLICABLE                      (0x74)

typedef struct
{
  GHS_CP_Procedure_t Procedure;
  GHS_CP_Response_t Response;
  uint16_t Flag;
} GHS_CP_Context_t;

/* Private variables ---------------------------------------------------------*/
static GHS_CP_Context_t GHS_CP_Context;

/* Global variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static tBleStatus GHS_CP_ResponseCode(uint8_t responseCode);

/* Private functions ---------------------------------------------------------*/
extern uint8_t a_GHS_UpdateCharData[247];

/**
  * @brief SRCP response code to the previous request
  * @param [in] responseCode : response code
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus GHS_CP_ResponseCode(uint8_t responseCode)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  GHS_Data_t msg_conf;
  uint8_t length = 0;

  LOG_INFO_APP("GHS_CP_ResponseCode >>>\r\n");

  a_GHS_UpdateCharData[length++] = responseCode;

  msg_conf.Length = length;
  msg_conf.p_Payload = a_GHS_UpdateCharData;
  retval = GHS_UpdateValue(GHS_GHSCP, &msg_conf);

  LOG_INFO_APP("GHS_CP_ResponseCode <<<\r\n");

  return retval;
} /* end of GHS_CP_ResponseCode() */

/* Public functions ----------------------------------------------------------*/

/**
  * @brief CP context initialization
  * @param None
  * @retval None
  */
void GHS_CP_Init(void)
{

} /* end GHS_CP_Init() */

/**
  * @brief SRCP request handler 
  * @param [in] pRequestData : pointer to received SRCP request data
  * @param [in] requestDataLength : received SRCP request data length
  * @retval None
  */
void GHS_CP_RequestHandler(uint8_t * pRequestData, uint8_t requestDataLength)
{
  LOG_INFO_APP("GHS_CP_RequestHandler >>>\r\n");
  LOG_INFO_APP("SRCP Request, request data length: %d\r\n", requestDataLength);
  
  /* Get the requested procedure OpCode */
  GHS_CP_Context.Procedure.OpCode = pRequestData[GHS_CP_OP_CODE_POSITION];
  
  /* Check and Process the OpCode */
  switch(GHS_CP_Context.Procedure.OpCode)
  {
    case GHS_CP_START_SENDING_LIVE_OBSERVATIONS:
    {
      GHS_CP_App_Event_t AppEvent;
      
      LOG_INFO_APP("GHS Control Point Start Sending Live Observations\r\n"); 
                
      AppEvent.EventCode = GHS_CP_START_SENDING_LIVE_OBSERVATIONS_EVENT;
      if (GHS_CP_APP_EventHandler(&AppEvent) == TRUE)
      {
        LOG_INFO_APP("GHS_CP_RESPONSE_CODE_SUCCESS\r\n"); 
        GHS_CP_ResponseCode(GHS_CP_SUCCESS);
      }
      else 
      {
        LOG_INFO_APP("GHS_CP_RESPONSE_CODE_PROCEDURE_NOT_COMPLETED\r\n"); 
        GHS_CP_ResponseCode(GHS_CP_SUCCESS);
      }
    }
    break;

    case GHS_CP_STOP_SENDING_LIVE_OBSERVATIONS:
    {
      GHS_CP_App_Event_t AppEvent;
      
      LOG_INFO_APP("GHS Control Point Stop Sending Live Observations\r\n"); 
                
      AppEvent.EventCode = GHS_CP_STOP_SENDING_LIVE_OBSERVATIONS_EVENT;
      if (GHS_CP_APP_EventHandler(&AppEvent) == TRUE)
      {
        LOG_INFO_APP("GHS_CP_RESPONSE_CODE_SUCCESS\r\n"); 
        GHS_CP_ResponseCode(GHS_CP_SUCCESS);
      }
      else 
      {
        LOG_INFO_APP("GHS_CP_RESPONSE_CODE_PROCEDURE_NOT_COMPLETED\r\n"); 
        GHS_CP_ResponseCode(GHS_CP_SUCCESS);
      }
    }
    break;

    default:
    {
      GHS_CP_ResponseCode(GHS_CP_ERROR_CODE_OPCODE_NOT_SUPPORTED);
    }
    break;
  }
  LOG_INFO_APP("GHS_CP_RequestHandler <<<\r\n");
} /* end GHS_CP_RequestHandler() */


/**
* @brief SRCP new write request permit check
* @param [in] pRequestData: Pointer to the request data byte array
* @param [in] pRequestDataLength: Length of the request data byte array
* @retval 0x00 when no error, error code otherwise
*/
uint8_t GHS_CP_CheckRequestValid(uint8_t * pRequestData, uint8_t requestDataLength)
{
  uint8_t retval = 0x00;
  
  LOG_INFO_APP("GHS_CP_CheckRequestValid >>>\r\n");
  GHS_CP_Context.Procedure.OpCode = pRequestData[GHS_CP_OP_CODE_POSITION];
  LOG_INFO_APP("GH CP Procedure OpCode: 0x%02X\r\n", GHS_CP_Context.Procedure.OpCode);
  
  if (GHS_APP_GetGHSCPCharacteristicIndicationEnabled() == FALSE)
  {
    return GHS_ATT_ERROR_CODE_CLIENT_CHAR_CONF_DESC_IMPROPERLY_CONFIGURED;
  }
  
  switch(GHS_CP_Context.Procedure.OpCode)
  {
    case GHS_CP_START_SENDING_LIVE_OBSERVATIONS:
    {
      LOG_INFO_APP("GHS Control Point START_SENDING_LIVE_OBSERVATIONS\r\n"); 
    }
    break;
     
    case GHS_CP_STOP_SENDING_LIVE_OBSERVATIONS:
    {
      LOG_INFO_APP("GHS Control Point STOP_SENDING_LIVE_OBSERVATIONS\r\n"); 
    }
    break;
     
    default:
    {
      return GHS_ATT_ERROR_CODE_OPCODE_NOT_SUPPORTED;
    }
    break;
  }
  
  LOG_INFO_APP("GHS_CP_CheckRequestValid <<<\r\n");
  return retval;
} /* end of GHS_CP_CheckRequestValid() */

