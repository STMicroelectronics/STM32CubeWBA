/**
  ******************************************************************************
  * @file    ghs_ccp.c
  * @author  MCD Application Team
  * @brief   IDS CCP
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
/* Includes ------------------------------------------------------------------*/
#include "common_blesvc.h"
#include "stm32_seq.h"
#include "stm32_timer.h"
#include "ghs.h"
#include "ghs_ccp.h"
#include "ghs_app.h"

/******************************************************************************
* Macro Declarations
******************************************************************************/
/* Private typedef -----------------------------------------------------------*/  
/* ID Status Read Control Point: Op Code Values */
/* CCP requests */
#define GHS_CCP_SET_THERAPY_CONTROL_STATE                                 0x0F5A
#define GHS_CCP_SET_FLIGHT_MODE                                           0x0F66
#define GHS_CCP_SNOOZE_ANNUNCIATION                                       0x0F69
#define GHS_CCP_CONFIRM_ANNUNCIATION                                      0x0F99
#define GHS_CCP_READ_BASAL_RATE_PROFILE_TEMPLATE                          0x0FAA
#define GHS_CCP_WRITE_BASAL_RATE_PROFILE_TEMPLATE                         0x0FCC
#define GHS_CCP_SET_TBR_ADJUSTMENT                                        0x0FFF
#define GHS_CCP_CANCEL_TBR_ADJUSTMENT                                     0x1111
#define GHS_CCP_GET_TBR_TEMPLATE                                          0x111E
#define GHS_CCP_SET_TBR_TEMPLATE                                          0x112D
#define GHS_CCP_SET_BOLUS                                                 0x114B
#define GHS_CCP_CANCEL_BOLUS                                              0x1178
#define GHS_CCP_GET_AVAILABLE_BOLUSES                                     0x1188
#define GHS_CCP_GET_BOLUS_TEMPLATE                                        0x11BB
#define GHS_CCP_SET_BOLUS_TEMPLATE                                        0x11DD
#define GHS_CCP_GET_TEMPLATE_STATUS_AND_DETAILS                           0x11EE
#define GHS_CCP_RESET_TEMPLATE_STATUS                                     0x121D 
#define GHS_CCP_ACTIVATE_PROFILE_TEMPLATES                                0x122E 
#define GHS_CCP_GET_ACTIVATED_PROFILE_TEMPLATES                           0x1248 
#define GHS_CCP_START_PRIMING                                             0x127B 
#define GHS_CCP_STOP_PRIMING                                              0x1284 
#define GHS_CCP_SET_INITIAL_RESERVOIR_FILL_LEVEL                          0x128B 
#define GHS_CCP_RESET_RESERVOIR_INSULIN_OPERATION_TIME                    0x12B7 
#define GHS_CCP_READ_ISF_PROFILE_TEMPLATE                                 0x12B8
#define GHS_CCP_WRITE_ISF_PROFILE_TEMPLATE                                0x12DE
#define GHS_CCP_READ_I2CHO_RATIO_PROFILE_TEMPLATE                         0x12ED
#define GHS_CCP_WRITE_I2CHO_RATIO_PROFILE_TEMPLATE                        0x141B
#define GHS_CCP_READ_TARGET_GLUCOSE_RANGE_PROFILE_TEMPLATE                0x1428
#define GHS_CCP_WRITE_TARGET_GLUCOSE_RANGE_PROFILE_TEMPLATE               0x144E
#define GHS_CCP_GET_MAX_BOLUS_AMOUNT                                      0x147D
#define GHS_CCP_SET_MAX_BOLUS_AMOUNT                                      0x148D

/* CCP responses */
/* Mandatory */
#define GHS_CCP_RESPONSE_CODE                                             0x0F55
#define GHS_CCP_SNOOZE_ANNUNCIATION_RESPONSE                              0x0F96
#define GHS_CCP_CONFIRM_ANNUNCIATION_RESPONSE                             0x0FA5
#define GHS_CCP_READ_BASAL_RATE_PROFILE_TEMPLATE_RESPONSE                 0x0FC3
#define GHS_CCP_WRITE_BASAL_RATE_PROFILE_TEMPLATE_RESPONSE                0x0FF0
#define GHS_CCP_GET_TBR_TEMPLATE_RESPONSE                                 0x1122
#define GHS_CCP_SET_TBR_TEMPLATE_RESPONSE                                 0x1144
#define GHS_CCP_SET_BOLUS_RESPONSE                                        0x1177
#define GHS_CCP_CANCEL_BOLUS_RESPONSE                                     0x1187
#define GHS_CCP_GET_AVAILABLE_BOLUSES_RESPONSE                            0x11B4
#define GHS_CCP_GET_BOLUS_TEMPLATE_RESPONSE                               0x11D2
#define GHS_CCP_SET_BOLUS_TEMPLATE_REPONSE                                0x11E1
#define GHS_CCP_GET_TEMPLATE_STATUS_AND_DETAILS_RESPONSE                  0x1212
#define GHS_CCP_RESET_TEMPLATE_STATUS_RESPONSE                            0x1221
#define GHS_CCP_ACTIVATE_PROFILE_TEMPLATES_RESPONSE                       0x1247 
#define GHS_CCP_GET_ACTIVATED_PROFILE_TEMPLATES_RESPONSE                  0x1274 
#define GHS_CCP_READ_ISF_PROFILE_TEMPLATE_RESPONSE                        0x12D1
#define GHS_CCP_WRITE_ISF_PROFILE_TEMPLATE_RESPONSE                       0x12E2
#define GHS_CCP_READ_I2CHO_RATIO_PROFILE_TEMPLATE_RESPONSE                0x1414
#define GHS_CCP_WRITE_I2CHO_RATIO_PROFILE_TEMPLATE_RESPONSE               0x1427
#define GHS_CCP_READ_TARGET_GLUCOSE_RANGE_PROFILE_TEMPLATE_RESPONSE       0x1441
#define GHS_CCP_WRITE_TARGET_GLUCOSE_RANGE_PROFILE_TEMPLATE_RESPONSE      0x1472
#define GHS_CCP_GET_MAX_BOLUS_AMOUNT_RESPONSE                             0x1482
  
/* ID Status Read Control Point: types valid length */
#define GHS_CCP_NO_OPERAND_REQUEST_LENGTH                                    (5)
#define GHS_CCP_SET_THERAPY_CONTROL_STATE_LENGTH                             (6)
#define GHS_CCP_SNOOZE_ANNUNCIATION_LENGTH                                   (7)
#define GHS_CCP_CONFIRM_ANNUNCIATION_LENGTH                                  (7)
#define GHS_CCP_READ_BASAL_RATE_LENGTH                                       (6)
#define GHS_CCP_WRITE_BASAL_RATE_LENGTH                                     (20)
#define GHS_CCP_SET_TBR_ADJUSTMENT_LENGTH                                   (13)
#define GHS_CCP_GET_TBR_TEMPLATE_LENGTH                                      (6)
#define GHS_CCP_SET_TBR_TEMPLATE_LENGTH                                     (11)
#define GHS_CCP_SET_BOLUS_LENGTH                                            (17)
#define GHS_CCP_CANCEL_BOLUS_LENGTH                                          (7)
#define GHS_CCP_GET_BOLUS_TEMPLATE_LENGTH                                    (6)
#define GHS_CCP_SET_BOLUS_TEMPLATE_LENGTH                                   (16)
#define GHS_CCP_RESET_TEMPLATE_STATUS_LENGTH                                (20)
#define GHS_CCP_ACTIVATE_PROFILE_TEMPLATES_LENGTH                           (20)
#define GHS_CCP_GET_ACTIVATED_PROFILE_TEMPLATES_LENGTH                      (20)
#define GHS_CCP_START_PRIMING_LENGTH                                         (7)
#define GHS_CCP_SET_INITIAL_RESERVOIR_FILL_LEVEL_LENGTH                      (7)
#define GHS_CCP_READ_ISF_PROFILE_LENGTH                                      (6)
#define GHS_CCP_WRITE_ISF_PROFILE_LENGTH                                    (20)
#define GHS_CCP_READ_I2CHO_RATIO_PROFILE_TEMPLATE_LENGTH                     (6)
#define GHS_CCP_WRITE_I2CHO_RATIO_PROFILE_TEMPLATE_LENGTH                   (20)
#define GHS_CCP_READ_TARGET_GLUCOSE_RANGE_PROFILE_TEMPLATE_LENGTH            (6)
#define GHS_CCP_WRITE_TARGET_GLUCOSE_RANGE_PROFILE_TEMPLATE_LENGTH          (20)
#define GHS_CCP_SET_MAX_BOLUS_AMOUNT_LENGTH                                  (7)
#define GHS_CCP_FILTER_TYPE_MAX_LENGTH                                      (20)
  
/* CGM Specific Ops Control Point: fields position */
#define GHS_CCP_OP_CODE_LENGTH                                               (2) 
#define GHS_CCP_OP_CODE_POSITION                                             (0)
#define GHS_CCP_OPERAND_POSITION                       (GHS_CCP_OP_CODE_LENGTH)

/* CGM Specific Ops Control Point: responses values */
#define GHS_CCP_RESPONSE_CODE_SUCCESS                                     (0x0F)
#define GHS_CCP_RESPONSE_CODE_OPCODE_NOT_SUPPORTED                        (0x70)
#define GHS_CCP_RESPONSE_CODE_INVALID_OPERAND                             (0x71)
#define GHS_CCP_RESPONSE_CODE_PROCEDURE_NOT_COMPLETED                     (0x72)
#define GHS_CCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE                      (0x73)
#define GHS_CCP_RESPONSE_CODE_PROCEDURE_NOT_APPLICABLE                    (0x74)

#define UNDETERMINED                                                      (0x0F)

#define ANNUNCIATION_SNOOZE_TIMEOUT                                       (5000)

typedef enum
{
  STOP                       = (0x33),
  PAUSE                      = (0x3C),
  RUN                        = (0x55)
} GHS_CCP_Therapy_Control_State_t;

typedef enum
{
  END_TRANSACTION           = (1<<0),
  SECOND_TIME_BLOCK_PRESENT = (1<<1),
  THIRD_TIME_BLOCK_PRESENT  = (1<<2)
} GHS_CCP_Basal_Rate_Profile_Template_Flags_t;

typedef enum
{
  TRANSACTION_COMPLETED = (1<<0)
} GHS_CCP_Basal_Rate_Profile_Template_Response_Flags_t;

typedef enum
{
  TBR_TEMPLATE_NUMBER_PRESENT  = (1<<0),
  TBR_DELIVERY_CONTEXT_PRESENT = (1<<1),
  CHANGE_TBR                   = (1<<2)
} GHS_CCP_TBR_Adjustment_Flags_t;

typedef enum
{
  DEVICE_BASED               = (0x33),
  REMOTE_CONTROL             = (0x3C),
  APP_CONTROLLER             = (0x55)
} GHS_CCP_TBR_Delivery_Context_Value_t;

typedef enum
{
  BOLUS_DELAY_TIME_PRESENT            = (1<<0),
  BOLUS_DELAY_TEMPLATE_NUMBER_PRESENT = (1<<1),
  BOLUS_ACTIVATION_TYPE_PRESENT       = (1<<2),
  BOLUS_DELIVERY_REASON_CORRECTION    = (1<<3),
  BOLUS_DELIVERY_REASON_MEAL          = (1<<4)
} GHS_CCP_Bolus_Flags_t;

typedef enum
{
  MANUAL_BOLUS                     = (0x33),
  RECOMMENDED_BOLUS                = (0x3C),
  MANUAL_CHANGED_RECOMMENDED_BOLUS = (0x55),
  COMMANDED_BOLUS                  = (0x5A)
} GHS_CCP_Activation_Type_t;

typedef enum
{
  FAST_BOLUS_AVAILABLE      = (1<<0),
  EXTENDED_BOLUS_AVAILABLE  = (1<<1),
  MULTIWAVE_BOLUS_AVAILABLE = (1<<2)
} GHS_CCP_Available_Bolus_Flags_t;

typedef enum
{
  BOLUS_TEMPLATE_DELAY_TIME_PRESENT          = (1<<0),
  BOLUS_TEMPLATE_DELIVERY_REASON_CORRECTION  = (1<<1),
  BOLUS_TEMPLATE_DELIVERY_REASON_MEAL        = (1<<2)
} GHS_CCP_Bolus_Template_Flags_t;

typedef struct
{
  GHS_CCP_Procedure_t Procedure;
  GHS_CCP_Response_t Response;
  GHS_CCP_State_t State;
  uint16_t Flag;
  uint8_t E2E_Counter;
  uint16_t E2E_CRC;
  UTIL_TIMER_Object_t TimerCCPResponse_Id;
  UTIL_TIMER_Object_t TimerCCPAnnunciationPending_Id;
  GHS_CCP_Therapy_Control_State_t TherapyControlState;
  uint8_t FlightMode;
} GHS_CCP_Context_t;

#if 0
typedef struct
{
  uint8_t  Flags;
  uint16_t Id;
  uint8_t  Type;
  uint16_t FastAmount;
  uint16_t ExtAmount;
  uint16_t Duration;
  uint16_t DelayTime;
  uint8_t TemplateNumber;
  uint8_t ActivationType;
  uint8_t  State;
} GHS_CCP_Bolus_t;

typedef struct
{
  uint8_t  Flags;
  uint8_t  TemplateNumber;
  uint16_t CurrentConfigValue;
  uint8_t  TBRType;
  uint16_t TBRAdjustmentValue;
  uint16_t TBRDurationProgrammed;
  uint16_t TBRDurationRemaining;
  uint8_t  TBRTemplateNumber;
  uint8_t  BasalDeliveryContext;
} GHS_CCP_Active_Basal_Rate_t;

typedef struct
{
  uint16_t  SumOfBolusDelivered;
  uint16_t  SumOfBasalDelivered;
} GHS_CCP_Total_Daily_Insulin_t;

typedef enum
{
  LIFETIME       = (0x0F),
  WARRANTY_TIME  = (0x33),
  LOANER_TIME    = (0x3C),
  OPERATION_TIME = (0x55)
} GHS_CCP_Counter_Type_t;

typedef enum
{
  CS_REMAINING = (0x0F),
  CS_ELAPSED   = (0x33)
} GHS_CCP_Counter_Selection_t;

typedef struct
{
  int32_t Remaining;
  int32_t Elapsed;
} GHS_CCP_Insulin_Counter_Selection_t;

typedef struct
{
  GHS_CCP_Insulin_Counter_Selection_t  LifeTime;
  GHS_CCP_Insulin_Counter_Selection_t  WarrantyTime;
  GHS_CCP_Insulin_Counter_Selection_t  LoanerTime;
  GHS_CCP_Insulin_Counter_Selection_t  OperationTime;
} GHS_CCP_Insulin_Counter_t;

typedef enum
{
  REMAINING_DURATION_PRESENT = (1<<0)
} GHS_CCP_On_Board_Flags_t;

typedef struct
{
  GHS_CCP_On_Board_Flags_t OnBoardFlags;
  uint16_t InsulinOnBoard;
  uint16_t RemainingDuration;
} GHS_CCP_Insulin_On_Board_t;
#endif

/* Private variables ---------------------------------------------------------*/
/**
 * START of Section BLE_DRIVER_CONTEXT
 */
PLACE_IN_SECTION("BLE_DRIVER_CONTEXT") static GHS_CCP_Context_t GHS_CCP_Context;

#if 0
GHS_CCP_Bolus_t GHS_ActiveBolusesIDs[4] =
{
  {/* Flags */           DELAY_TIME_PRESENT | TEMPLATE_NUMBER_PRESENT | ACTIVATION_TYPE_PRESENT | DELIVERY_REASON_CORRECTION | DELIVERY_REASON_MEAL, 
   /* Id */              0x01,
   /* Type */            FAST,
   /* Fast Amount */     10,
   /* Ext Amount */      0,
   /* Duration (min) */  0,
   /* Delay (min) */     10,
   /* Template Number */  1,
   /* Activation Type */ MANUAL,
   /* State */           1},
  {/* Flags */           DELAY_TIME_PRESENT | TEMPLATE_NUMBER_PRESENT | ACTIVATION_TYPE_PRESENT | DELIVERY_REASON_CORRECTION | DELIVERY_REASON_MEAL, 
   /* Id */              0x02,
   /* Type */            EXTENDED,
   /* Fast Amount */     0,
   /* Ext Amount */      10,
   /* Duration (min) */  10,
   /* Delay (min) */     10,
   /* Template Number */  2,
   /* Activation Type */ RECOMMENDED,
   /* State */           1},
  {/* Flags */           DELAY_TIME_PRESENT | TEMPLATE_NUMBER_PRESENT | ACTIVATION_TYPE_PRESENT | DELIVERY_REASON_CORRECTION | DELIVERY_REASON_MEAL, 
   /* Id */              0x03,
   /* Type */            MULTI_WAVE,
   /* Fast Amount */     10,
   /* Ext Amount */      10,
   /* Duration (min) */  10,
   /* Delay (min) */     10,
   /* Template Number */  3,
   /* Activation Type */ MANUALLY_CHANGED_RECOMMENDED,
   /* State */           1},
  {/* Flags */           DELAY_TIME_PRESENT | TEMPLATE_NUMBER_PRESENT | ACTIVATION_TYPE_PRESENT | DELIVERY_REASON_CORRECTION | DELIVERY_REASON_MEAL, 
   /* Id */              0x04,
   /* Type */            FAST,
   /* Fast Amount */     10,
   /* Ext Amount */      0,
   /* Duration (min) */  0,
   /* Delay (min) */     10,
   /* Template Number */  4,
   /* Activation Type */ COMMANDED,
   /* State */           0}
};

GHS_CCP_Active_Basal_Rate_t GHS_ActiveBasalRate[4] =
{
  {/* Flags */                    TBR_PRESENT | TBR_TEMPLATE_NUMBER_PRESENT | BASAL_DELIVERY_CONTEXT_PRESENT, 
   /* Template Number */          1,
   /* Current Config Value */     10,
   /* TBR Type Value */           ABSOLUTE,
   /* TBR Adjustment Value */     10,
   /* TBR Duration Programmed */  10,
   /* TBR Duration Remaining */   10,
   /* TBR Template Number */      1,
   /* Basal Delivery Context */   10},
  {/* Flags */                    TBR_PRESENT | TBR_TEMPLATE_NUMBER_PRESENT | BASAL_DELIVERY_CONTEXT_PRESENT, 
   /* Template Number */          2,
   /* Current Config Value */     10,
   /* TBR Type Value */           RELATIVE,
   /* TBR Adjustment Value */     10,
   /* TBR Duration Programmed */  10,
   /* TBR Duration Remaining */   10,
   /* TBR Template Number */      2,
   /* Basal Delivery Context */   10},
  {/* Flags */                    TBR_PRESENT | TBR_TEMPLATE_NUMBER_PRESENT | BASAL_DELIVERY_CONTEXT_PRESENT, 
   /* Template Number */          3,
   /* Current Config Value */     10,
   /* TBR Type Value */           ABSOLUTE,
   /* TBR Adjustment Value */     10,
   /* TBR Duration Programmed */  10,
   /* TBR Duration Remaining */   10,
   /* TBR Template Number */      3,
   /* Basal Delivery Context */   10},
  {/* Flags */                    TBR_PRESENT | TBR_TEMPLATE_NUMBER_PRESENT | BASAL_DELIVERY_CONTEXT_PRESENT, 
   /* Template Number */          4,
   /* Current Config Value */     10,
   /* TBR Type Value */           RELATIVE,
   /* TBR Adjustment Value */     10,
   /* TBR Duration Programmed */  10,
   /* TBR Duration Remaining */   10,
   /* TBR Template Number */      4,
   /* Basal Delivery Context */   10}
};

GHS_CCP_Insulin_On_Board_t InsulinOnBoardValue = {REMAINING_DURATION_PRESENT, 100, 50};
#endif

/* Global variables ----------------------------------------------------------*/
#if 0
GHS_CCP_Total_Daily_Insulin_t TotalDailyInsulin;
GHS_CCP_Insulin_Counter_t IDDCounter;
uint32_t BolusAmountDelivered;
uint32_t BasalAmountDelivered;
uint8_t ActiveBolusNumber;
uint8_t ActiveBasalRateDeliveryNumber;
#endif
/* Private function prototypes -----------------------------------------------*/
static tBleStatus GHS_CCP_ResponseCode(uint8_t responseCode);
static tBleStatus GHS_CCP_SnoozeAnnunciationResponse(uint16_t annunciationInstanceId);
static tBleStatus GHS_CCP_ConfirmAnnunciationResponse(uint16_t annunciationInstanceId);
static tBleStatus GHS_CCP_ReadBasalRateProfileTemplateResponse(uint8_t BasalRateProfileTemplateNumber);
static tBleStatus GHS_CCP_WriteBasalRateProfileTemplateResponse(void);
static tBleStatus GHS_CCP_GetTbrTemplateResponse(void);
static tBleStatus GHS_CCP_SetTbrTemplateResponse(void);
static tBleStatus GHS_CCP_SetBolusResponse(void);
static tBleStatus GHS_CCP_CancelBolusResponse(void);
static tBleStatus GHS_CCP_GetAvailableBolusesResponse(void);
static tBleStatus GHS_CCP_GetBolusTemplateResponse(void);
static tBleStatus GHS_CCP_SetBolusTemplateResponse(void);
static tBleStatus GHS_CCP_GetTemplateStatusAndDetailsResponse(void);
static tBleStatus GHS_CCP_ResetTemplateStatusResponse(void);
static tBleStatus GHS_CCP_ActivateProfileTemplatesResponse(void);
static tBleStatus GHS_CCP_GetActivatedProfileTemplatesResponse(void);
static tBleStatus GHS_CCP_ReadIsfProfileTemplateResponse(void);
static tBleStatus GHS_CCP_WriteIsfProfileTemplateResponse(void);
static tBleStatus GHS_CCP_ReadI2choRatioProfileTemplateResponse(void);
static tBleStatus GHS_CCP_WriteI2choRatioProfileTemplateResponse(void);
static tBleStatus GHS_CCP_ReadTargetGlucoseRangeProfileTemplateResponse(void);
static tBleStatus GHS_CCP_WriteTargetGlucoseRangeProfileTemplateResponse(void);
static tBleStatus GHS_CCP_GetMaxBolusAmountResponse(void);
static void GHS_CCP_ResponseProc(void *arg);
static void GHS_CCP_ResponeTask(void);

/* Private functions ---------------------------------------------------------*/
extern uint8_t a_GHS_UpdateCharData[247];

/**
  * @brief CCP response code to the previous request
  * @param [in] responseCode : response code
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus GHS_CCP_ResponseCode(uint8_t responseCode)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  GHS_Data_t msg_conf;
  uint8_t length = 0;

  LOG_INFO_APP("GHS_CCP_ResponseCode >>>\r\n");

  if(GHS_CCP_Context.Procedure.OpCode == GHS_CCP_SNOOZE_ANNUNCIATION)
  {
    a_GHS_UpdateCharData[length++] = ((GHS_CCP_SNOOZE_ANNUNCIATION_RESPONSE) & 0xFF);;
    a_GHS_UpdateCharData[length++] = (((GHS_CCP_SNOOZE_ANNUNCIATION_RESPONSE) >> 8) & 0xFF);
    a_GHS_UpdateCharData[length++] = GHS_CCP_Context.Procedure.Operand[0];
    a_GHS_UpdateCharData[length++] = GHS_CCP_Context.Procedure.Operand[1];
  }
  else if(GHS_CCP_Context.Procedure.OpCode == GHS_CCP_CONFIRM_ANNUNCIATION)
  {
    a_GHS_UpdateCharData[length++] = ((GHS_CCP_CONFIRM_ANNUNCIATION_RESPONSE) & 0xFF);;
    a_GHS_UpdateCharData[length++] = (((GHS_CCP_CONFIRM_ANNUNCIATION_RESPONSE) >> 8) & 0xFF);
    a_GHS_UpdateCharData[length++] = GHS_CCP_Context.Procedure.Operand[0];
    a_GHS_UpdateCharData[length++] = GHS_CCP_Context.Procedure.Operand[1];
  }
  else
  {
    a_GHS_UpdateCharData[length++] = (GHS_CCP_RESPONSE_CODE & 0xFF);
    a_GHS_UpdateCharData[length++] = ((GHS_CCP_RESPONSE_CODE >> 8) & 0xFF);
    a_GHS_UpdateCharData[length++] = ((GHS_CCP_Context.Procedure.OpCode) & 0xFF);;
    a_GHS_UpdateCharData[length++] = (((GHS_CCP_Context.Procedure.OpCode) >> 8) & 0xFF);
    a_GHS_UpdateCharData[length++] = responseCode;
  }
  a_GHS_UpdateCharData[length++] = GHS_CCP_Context.E2E_Counter;
#if 0
  if(((GHS_APP_GetFeature()) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
  {
    GHS_CCP_Context.E2E_CRC = GHS_APP_ComputeCRC(a_GHS_UpdateCharData, length);
    LOG_INFO_APP("E2E_CRC = 0x%x\r\n", GHS_CCP_Context.E2E_CRC);
  }
  else
  {
    GHS_CCP_Context.E2E_CRC = 0xFFFF;
    LOG_INFO_APP("E2E_CRC = 0x%x\r\n", GHS_CCP_Context.E2E_CRC);
  }
#endif
  a_GHS_UpdateCharData[length++] = (uint8_t)((GHS_CCP_Context.E2E_CRC) & 0xFF);
  a_GHS_UpdateCharData[length++] = (uint8_t)(((GHS_CCP_Context.E2E_CRC) >> 8) & 0xFF);

  msg_conf.Length = length;
  msg_conf.p_Payload = a_GHS_UpdateCharData;
//  retval = GHS_UpdateValue(GHS_IDCCP, &msg_conf);


  LOG_INFO_APP("GHS_CCP_ResponseCode <<<\r\n");

  return retval;
} /* end of GHS_CCP_ResponseCode() */

/**
  * @brief CCP snooze annunciation response to the previous request
  * @param [in] annunciation instanve ID
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus GHS_CCP_SnoozeAnnunciationResponse(uint16_t annunciationInstanceId)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  GHS_Data_t msg_conf;
  uint8_t length = 0;

  a_GHS_UpdateCharData[length++] = (GHS_CCP_SNOOZE_ANNUNCIATION_RESPONSE & 0xFF);
  a_GHS_UpdateCharData[length++] = ((GHS_CCP_SNOOZE_ANNUNCIATION_RESPONSE >> 8) & 0xFF);
  a_GHS_UpdateCharData[length++] = (annunciationInstanceId & 0xFF);
  a_GHS_UpdateCharData[length++] = ((annunciationInstanceId >> 8) & 0xFF);
  a_GHS_UpdateCharData[length++] = GHS_CCP_Context.E2E_Counter;
#if 0
  if(((GHS_APP_GetFeature()) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
  {
    GHS_CCP_Context.E2E_CRC = GHS_APP_ComputeCRC(a_GHS_UpdateCharData, length);
  }
  else
  {
    GHS_CCP_Context.E2E_CRC = 0xFFFF;
  }
#endif
  a_GHS_UpdateCharData[length++] = (uint8_t)((GHS_CCP_Context.E2E_CRC) & 0xFF);
  a_GHS_UpdateCharData[length++] = (uint8_t)(((GHS_CCP_Context.E2E_CRC) >> 8) & 0xFF);

  msg_conf.Length = length;
  msg_conf.p_Payload = a_GHS_UpdateCharData;
//  retval = GHS_UpdateValue(GHS_IDCCP, &msg_conf);

  return retval;
} /* end of GHS_CCP_get_active_bolus_ids_response() */

/**
  * @brief CCP confirm annunciation response to the previous request
  * @param [in] Bannunciation instanve ID
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus GHS_CCP_ConfirmAnnunciationResponse(uint16_t annunciationInstanceId)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  GHS_Data_t msg_conf;
  uint8_t length = 0;

  a_GHS_UpdateCharData[length++] = (GHS_CCP_CONFIRM_ANNUNCIATION & 0xFF);
  a_GHS_UpdateCharData[length++] = ((GHS_CCP_CONFIRM_ANNUNCIATION >> 8) & 0xFF);
  a_GHS_UpdateCharData[length++] = (annunciationInstanceId & 0xFF);
  a_GHS_UpdateCharData[length++] = ((annunciationInstanceId >> 8) & 0xFF);
  a_GHS_UpdateCharData[length++] = GHS_CCP_Context.E2E_Counter;
#if 0
  if(((GHS_APP_GetFeature()) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
  {
    GHS_CCP_Context.E2E_CRC = GHS_APP_ComputeCRC(a_GHS_UpdateCharData, length);
  }
  else
  {
    GHS_CCP_Context.E2E_CRC = 0xFFFF;
  }
#endif
  a_GHS_UpdateCharData[length++] = (uint8_t)((GHS_CCP_Context.E2E_CRC) & 0xFF);
  a_GHS_UpdateCharData[length++] = (uint8_t)(((GHS_CCP_Context.E2E_CRC) >> 8) & 0xFF);

  msg_conf.Length = length;
  msg_conf.p_Payload = a_GHS_UpdateCharData;
//  retval = GHS_UpdateValue(GHS_IDCCP, &msg_conf);
  
  return retval;
} /* end of GHS_CCP_get_bolus_delivery_response() */

#if 0
/**
  * @brief CCP get active basal rate delivery response to the previous request
  * @param [in] None
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus GHS_CCP_get_active_basal_rate_delivery_response(void)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  GHS_Data_t msg_conf;
  uint8_t length = 0;

  if(ActiveBasalRateDeliveryNumber > 0)
  {
    a_GHS_UpdateCharData[length++] = (GET_ACTIVE_BASAL_RATE_DELIVERY_RESPONSE & 0xFF);
    a_GHS_UpdateCharData[length++] = ((GET_ACTIVE_BASAL_RATE_DELIVERY_RESPONSE >> 8) & 0xFF);
    a_GHS_UpdateCharData[length++] =   GHS_ActiveBasalRate[0].Flags;
    a_GHS_UpdateCharData[length++] =   GHS_ActiveBasalRate[0].TemplateNumber;
    a_GHS_UpdateCharData[length++] =  (GHS_ActiveBasalRate[0].CurrentConfigValue & 0xFF);
    a_GHS_UpdateCharData[length++] = ((GHS_ActiveBasalRate[0].CurrentConfigValue >> 8) & 0xFF);
    if((GHS_ActiveBasalRate[0].Flags & TBR_PRESENT) == TBR_PRESENT)
    {
      a_GHS_UpdateCharData[length++] =   GHS_ActiveBasalRate[0].TBRType;
      a_GHS_UpdateCharData[length++] = ((GHS_ActiveBasalRate[0].TBRAdjustmentValue) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((GHS_ActiveBasalRate[0].TBRAdjustmentValue) >> 8) & 0xFF);
      a_GHS_UpdateCharData[length++] = ((GHS_ActiveBasalRate[0].TBRDurationProgrammed) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((GHS_ActiveBasalRate[0].TBRDurationProgrammed) >> 8) & 0xFF);
      a_GHS_UpdateCharData[length++] = ((GHS_ActiveBasalRate[0].TBRDurationRemaining) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((GHS_ActiveBasalRate[0].TBRDurationRemaining) >> 8) & 0xFF);
    }
    if((GHS_ActiveBasalRate[0].Flags & TBR_TEMPLATE_NUMBER_PRESENT) == TBR_TEMPLATE_NUMBER_PRESENT)
    {
      a_GHS_UpdateCharData[length++] = GHS_ActiveBasalRate[0].TBRTemplateNumber;
    }
    if((GHS_ActiveBasalRate[0].Flags & BASAL_DELIVERY_CONTEXT_PRESENT) == BASAL_DELIVERY_CONTEXT_PRESENT)
    {
      a_GHS_UpdateCharData[length++] = GHS_ActiveBasalRate[0].BasalDeliveryContext;
    }

    a_GHS_UpdateCharData[length++] = GHS_CCP_Context.E2E_Counter;
    if(((GHS_APP_GetFeature()) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
    {
      GHS_CCP_Context.E2E_CRC = GHS_APP_ComputeCRC(a_GHS_UpdateCharData, length);
    }
    else
    {
      GHS_CCP_Context.E2E_CRC = 0xFFFF;
    }
    a_GHS_UpdateCharData[length++] = (uint8_t)((GHS_CCP_Context.E2E_CRC) & 0xFF);
    a_GHS_UpdateCharData[length++] = (uint8_t)(((GHS_CCP_Context.E2E_CRC) >> 8) & 0xFF);
        
    msg_conf.Length = length;
    msg_conf.p_Payload = a_GHS_UpdateCharData;
    retval = GHS_UpdateValue(GHS_IDCCP, &msg_conf);
  }
  else
  {
    LOG_INFO_APP("PROCEDURE NOT APPLICABLE: no Bolus found\r\n");
    GHS_CCP_ResponseCode(GHS_CCP_RESPONSE_CODE_PROCEDURE_NOT_APPLICABLE);
    return BLE_STATUS_SUCCESS;
  }
    

  return retval;
} /* end of GHS_CCP_get_active_basal_rate_delivery_response() */

/**
  * @brief CCP Get Total Daily Insulin response to the previous request
  * @param [in] None
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus GHS_CCP_get_total_daily_insulin_response(void)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  GHS_Data_t msg_conf;
  uint8_t length = 0;

  a_GHS_UpdateCharData[length++] = (GET_TOTAL_DAILY_INSULIN_STATUS_RESPONSE & 0xFF);
  a_GHS_UpdateCharData[length++] = ((GET_TOTAL_DAILY_INSULIN_STATUS_RESPONSE >> 8) & 0xFF);
  if(((GHS_APP_GetFeature()) & (FAST_BOLUS_SUPPORTED | EXTENDED_BOLUS_SUPPORTED | MULTIWAVE_BOLUS_SUPPORTED)) == 
     (FAST_BOLUS_SUPPORTED | EXTENDED_BOLUS_SUPPORTED | MULTIWAVE_BOLUS_SUPPORTED))
  {
    a_GHS_UpdateCharData[length++] = ((TotalDailyInsulin.SumOfBolusDelivered) & 0xFF);
    a_GHS_UpdateCharData[length++] = (((TotalDailyInsulin.SumOfBolusDelivered) >> 8) & 0xFF);
  }
  if(((GHS_APP_GetFeature()) & BASAL_RATE_SUPPORTED) == BASAL_RATE_SUPPORTED)
  {
    a_GHS_UpdateCharData[length++] = ((TotalDailyInsulin.SumOfBasalDelivered) & 0xFF);
    a_GHS_UpdateCharData[length++] = (((TotalDailyInsulin.SumOfBasalDelivered) >> 8) & 0xFF);
  }
  a_GHS_UpdateCharData[length++] = ((TotalDailyInsulin.SumOfBolusDelivered + TotalDailyInsulin.SumOfBasalDelivered) & 0xFF);
  a_GHS_UpdateCharData[length++] = (((TotalDailyInsulin.SumOfBolusDelivered + TotalDailyInsulin.SumOfBasalDelivered) >> 8) & 0xFF);

  a_GHS_UpdateCharData[length++] = GHS_CCP_Context.E2E_Counter;
  if(((GHS_APP_GetFeature()) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
  {
    GHS_CCP_Context.E2E_CRC = GHS_APP_ComputeCRC(a_GHS_UpdateCharData, length);
  }
  else
  {
    GHS_CCP_Context.E2E_CRC = 0xFFFF;
  }
  a_GHS_UpdateCharData[length++] = (uint8_t)((GHS_CCP_Context.E2E_CRC) & 0xFF);
  a_GHS_UpdateCharData[length++] = (uint8_t)(((GHS_CCP_Context.E2E_CRC) >> 8) & 0xFF);
      
  msg_conf.Length = length;
  msg_conf.p_Payload = a_GHS_UpdateCharData;
  retval = GHS_UpdateValue(GHS_IDCCP, &msg_conf);

  return retval;
} /* end of GHS_CCP_get_total_daily_insulin_response() */

/**
  * @brief CCP get counter response to the previous request
  * @param [in] type, Value Selection
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus GHS_CCP_get_counter_response(uint8_t type, uint8_t valueSelection)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  GHS_Data_t msg_conf;
  uint8_t length = 0;

  if(((GHS_CCP_Counter_Selection_t)valueSelection != CS_REMAINING) ||
     ((GHS_CCP_Counter_Selection_t)valueSelection != CS_ELAPSED))
  {
    GHS_CCP_ResponseCode(GHS_CCP_RESPONSE_CODE_INVALID_OPERAND);
    return BLE_STATUS_SUCCESS;
  }
  
  a_GHS_UpdateCharData[length++] = (GET_COUNTER_RESPONSE & 0xFF);
  a_GHS_UpdateCharData[length++] = ((GET_COUNTER_RESPONSE >> 8) & 0xFF);
  a_GHS_UpdateCharData[length++] = type;
  a_GHS_UpdateCharData[length++] = valueSelection;
  if(type == LIFETIME)
  {
    if(valueSelection == CS_REMAINING)
    {
      a_GHS_UpdateCharData[length++] =  ((IDDCounter.LifeTime.Remaining) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((IDDCounter.LifeTime.Remaining) >> 8) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((IDDCounter.LifeTime.Remaining) >> 16) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((IDDCounter.LifeTime.Remaining) >> 24) & 0xFF);
    }
    else
    {
      a_GHS_UpdateCharData[length++] =  ((IDDCounter.LifeTime.Elapsed) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((IDDCounter.LifeTime.Elapsed) >> 8) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((IDDCounter.LifeTime.Elapsed) >> 16) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((IDDCounter.LifeTime.Elapsed) >> 24) & 0xFF);
    }
  }
  else if(type == WARRANTY_TIME)
  {
    if(valueSelection == CS_REMAINING)
    {
      a_GHS_UpdateCharData[length++] =  ((IDDCounter.WarrantyTime.Remaining) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((IDDCounter.WarrantyTime.Remaining) >> 8) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((IDDCounter.WarrantyTime.Remaining) >> 16) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((IDDCounter.WarrantyTime.Remaining) >> 24) & 0xFF);
    }
    else
    {
      a_GHS_UpdateCharData[length++] =  ((IDDCounter.WarrantyTime.Elapsed) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((IDDCounter.WarrantyTime.Elapsed) >> 8) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((IDDCounter.WarrantyTime.Elapsed) >> 16) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((IDDCounter.WarrantyTime.Elapsed) >> 24) & 0xFF);
    }
  }
  else if(type == LOANER_TIME)
  {
    if(valueSelection == CS_REMAINING)
    {
      a_GHS_UpdateCharData[length++] =  ((IDDCounter.LoanerTime.Remaining) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((IDDCounter.LoanerTime.Remaining) >> 8) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((IDDCounter.LoanerTime.Remaining) >> 16) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((IDDCounter.LoanerTime.Remaining) >> 24) & 0xFF);
    }
    else
    {
      a_GHS_UpdateCharData[length++] =  ((IDDCounter.LoanerTime.Elapsed) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((IDDCounter.LoanerTime.Elapsed) >> 8) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((IDDCounter.LoanerTime.Elapsed) >> 16) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((IDDCounter.LoanerTime.Elapsed) >> 24) & 0xFF);
    }
  }
  else
  {
    if(valueSelection == CS_REMAINING)
    {
      a_GHS_UpdateCharData[length++] =  ((IDDCounter.OperationTime.Remaining) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((IDDCounter.OperationTime.Remaining) >> 8) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((IDDCounter.OperationTime.Remaining) >> 16) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((IDDCounter.OperationTime.Remaining) >> 24) & 0xFF);
    }
    else
    {
      a_GHS_UpdateCharData[length++] =  ((IDDCounter.OperationTime.Elapsed) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((IDDCounter.OperationTime.Elapsed) >> 8) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((IDDCounter.OperationTime.Elapsed) >> 16) & 0xFF);
      a_GHS_UpdateCharData[length++] = (((IDDCounter.OperationTime.Elapsed) >> 24) & 0xFF);
    }
  }

  a_GHS_UpdateCharData[length++] = GHS_CCP_Context.E2E_Counter;
  if(((GHS_APP_GetFeature()) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
  {
    GHS_CCP_Context.E2E_CRC = GHS_APP_ComputeCRC(a_GHS_UpdateCharData, length);
  }
  else
  {
    GHS_CCP_Context.E2E_CRC = 0xFFFF;
  }
  a_GHS_UpdateCharData[length++] = (uint8_t)((GHS_CCP_Context.E2E_CRC) & 0xFF);
  a_GHS_UpdateCharData[length++] = (uint8_t)(((GHS_CCP_Context.E2E_CRC) >> 8) & 0xFF);
      
  msg_conf.Length = length;
  msg_conf.p_Payload = a_GHS_UpdateCharData;
  retval = GHS_UpdateValue(GHS_IDCCP, &msg_conf);
 
  return retval;
} /* end of GHS_CCP_get_counter_response(uint8_t type, uint8_t valueSelection) */

/**
  * @brief CCP Get Delivered Insulin response to the previous request
  * @param [in] None
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus GHS_CCP_get_delivered_insulin_response(void)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  GHS_Data_t msg_conf;
  uint8_t length = 0;

  a_GHS_UpdateCharData[length++] = (GET_DELIVERED_INSULIN_RESPONSE & 0xFF);
  a_GHS_UpdateCharData[length++] = ((GET_DELIVERED_INSULIN_RESPONSE >> 8) & 0xFF);
  a_GHS_UpdateCharData[length++] =  ((BolusAmountDelivered) & 0xFF);
  a_GHS_UpdateCharData[length++] = (((BolusAmountDelivered) >> 8) & 0xFF);
  a_GHS_UpdateCharData[length++] = (((BolusAmountDelivered) >> 16) & 0xFF);
  a_GHS_UpdateCharData[length++] = (((BolusAmountDelivered) >> 24) & 0xFF);
  a_GHS_UpdateCharData[length++] =  ((BasalAmountDelivered) & 0xFF);
  a_GHS_UpdateCharData[length++] = (((BasalAmountDelivered) >> 8) & 0xFF);
  a_GHS_UpdateCharData[length++] = (((BasalAmountDelivered) >> 16) & 0xFF);
  a_GHS_UpdateCharData[length++] = (((BasalAmountDelivered) >> 24) & 0xFF);

  a_GHS_UpdateCharData[length++] = GHS_CCP_Context.E2E_Counter;
  if(((GHS_APP_GetFeature()) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
  {
    GHS_CCP_Context.E2E_CRC = GHS_APP_ComputeCRC(a_GHS_UpdateCharData, length);
  }
  else
  {
    GHS_CCP_Context.E2E_CRC = 0xFFFF;
  }
  a_GHS_UpdateCharData[length++] = (uint8_t)((GHS_CCP_Context.E2E_CRC) & 0xFF);
  a_GHS_UpdateCharData[length++] = (uint8_t)(((GHS_CCP_Context.E2E_CRC) >> 8) & 0xFF);
      
  msg_conf.Length = length;
  msg_conf.p_Payload = a_GHS_UpdateCharData;
  retval = GHS_UpdateValue(GHS_IDCCP, &msg_conf);
  
  return retval;
} /* end of GHS_CCP_get_delivered_insulin_response() */

/**
  * @brief CCP Get Insulin On Board response to the previous request
  * @param [in] None
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus GHS_CCP_get_insulin_on_board_response(void)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  GHS_Data_t msg_conf;
  uint8_t length = 0;

  a_GHS_UpdateCharData[length++] = (GET_INSULIN_ON_BOARD_RESPONSE & 0xFF);
  a_GHS_UpdateCharData[length++] = ((GET_INSULIN_ON_BOARD_RESPONSE >> 8) & 0xFF);
  a_GHS_UpdateCharData[length++] =    InsulinOnBoardValue.OnBoardFlags;
  a_GHS_UpdateCharData[length++] =  ((InsulinOnBoardValue.InsulinOnBoard) & 0xFF);
  a_GHS_UpdateCharData[length++] = (((InsulinOnBoardValue.InsulinOnBoard) >> 8) & 0xFF);
  if((InsulinOnBoardValue.OnBoardFlags & REMAINING_DURATION_PRESENT) == REMAINING_DURATION_PRESENT)
  {
  a_GHS_UpdateCharData[length++] =  ((InsulinOnBoardValue.RemainingDuration) & 0xFF);
  a_GHS_UpdateCharData[length++] = (((InsulinOnBoardValue.RemainingDuration) >> 8) & 0xFF);
  }

  a_GHS_UpdateCharData[length++] = GHS_CCP_Context.E2E_Counter;
  if(((GHS_APP_GetFeature()) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
  {
    GHS_CCP_Context.E2E_CRC = GHS_APP_ComputeCRC(a_GHS_UpdateCharData, length);
  }
  else
  {
    GHS_CCP_Context.E2E_CRC = 0xFFFF;
  }
  a_GHS_UpdateCharData[length++] = (uint8_t) ((GHS_CCP_Context.E2E_CRC) & 0xFF);
  a_GHS_UpdateCharData[length++] = (uint8_t)(((GHS_CCP_Context.E2E_CRC) >> 8) & 0xFF);
      
  msg_conf.Length = length;
  msg_conf.p_Payload = a_GHS_UpdateCharData;
  retval = GHS_UpdateValue(GHS_IDCCP, &msg_conf);

  return retval;
} /* end of GHS_CCP_get_insulin_on_board_response() */
#endif

static void GHS_CCP_ResponseProc(void *arg)
{
  LOG_INFO_APP("Set task for CCP response\r\n");
  UTIL_SEQ_SetTask(1U << CFG_TASK_GHS_CCP_ID, CFG_SEQ_PRIO_0);
}

static void GHS_CCP_ResponeTask(void)
{
  LOG_INFO_APP("Send CCP response\r\n");
  GHS_CCP_ResponseCode(GHS_CCP_RESPONSE_CODE_SUCCESS);
}

static void GHS_CCP_AnnunciationPendingProc(void *arg)
{
  UTIL_SEQ_SetTask(1U << CFG_TASK_GHS_CCP_PENDING_ID, CFG_SEQ_PRIO_0);
}

static void GHS_CCP_AnnunciationPendingTask(void)
{
  GHS_Annunciation_Status_Value_t * pAnnunciationStatus;
  uint16_t StatusFlags;
  
  LOG_INFO_APP("Annunciation SNOOZE timeout, set Annunciation PENDING\r\n");
  
  pAnnunciationStatus = GHS_APP_GetAnnunciationStatus();

  pAnnunciationStatus->Status = PENDING;
  GHS_APP_SetAnnunciationStatus(pAnnunciationStatus);
  StatusFlags = GHS_APP_GetStatusChangedFlag();
  StatusFlags |= ANNUNCIATION_STATUS_CHANGED;
  GHS_APP_SetStatusChangedFlag(StatusFlags);
}

/* Public functions ----------------------------------------------------------*/

/**
  * @brief CCP context initialization
  * @param None
  * @retval None
  */
void GHS_CCP_Init(void)
{
  GHS_CCP_Context.TherapyControlState = STOP;
  GHS_CCP_Context.FlightMode = FALSE;

  UTIL_TIMER_Create(&(GHS_CCP_Context.TimerCCPResponse_Id),
                    0,
                    UTIL_TIMER_ONESHOT,
                    &GHS_CCP_ResponseProc, 
                    0);
  UTIL_TIMER_SetPeriod(&(GHS_CCP_Context.TimerCCPResponse_Id), 1000);
  
  UTIL_SEQ_RegTask( 1<< CFG_TASK_GHS_CCP_ID, UTIL_SEQ_RFU, GHS_CCP_ResponeTask );
  
  UTIL_TIMER_Create(&(GHS_CCP_Context.TimerCCPAnnunciationPending_Id),
                    0,
                    UTIL_TIMER_ONESHOT,
                    &GHS_CCP_AnnunciationPendingProc, 
                    0);
  UTIL_TIMER_SetPeriod(&(GHS_CCP_Context.TimerCCPAnnunciationPending_Id), ANNUNCIATION_SNOOZE_TIMEOUT);
  
  UTIL_SEQ_RegTask( 1<< CFG_TASK_GHS_CCP_PENDING_ID, UTIL_SEQ_RFU, GHS_CCP_AnnunciationPendingTask );
} /* end GHS_CCP_Init() */

/**
  * @brief CCP request handler 
  * @param [in] pRequestData : pointer to received CCP request data
  * @param [in] requestDataLength : received CCP request data length
  * @retval None
  */
void GHS_CCP_RequestHandler(uint8_t * pRequestData, uint8_t requestDataLength)
{
  LOG_INFO_APP("GHS_CCP_RequestHandler >>>\r\n");
  LOG_INFO_APP("CCP Request, request data length: %d\r\n", requestDataLength);
  
  /* Get the requested procedure OpCode */
  GHS_CCP_Context.Procedure.OpCode = ( (pRequestData[GHS_CCP_OP_CODE_POSITION]) |
                                       ((pRequestData[GHS_CCP_OP_CODE_POSITION + 1]) << 8) );
  LOG_INFO_APP("CCP Procedure OpCode: 0x%02X\r\n", GHS_CCP_Context.Procedure.OpCode);
  
  /* Check and Process the OpCode */
  switch(GHS_CCP_Context.Procedure.OpCode)
  {
    case GHS_CCP_SET_THERAPY_CONTROL_STATE:
      {
//        GHS_Status_Value_t * pIdsStatus;
        
//        pIdsStatus = GHS_APP_GetStatus();
        
        LOG_INFO_APP("GHS_CCP_SET_THERAPY_CONTROL_STATE\r\n"); 
                  
        GHS_CCP_Context.Procedure.Operand[0] = pRequestData[GHS_CCP_OPERAND_POSITION];

        if((pRequestData[GHS_CCP_OPERAND_POSITION] != STOP) &&
           (pRequestData[GHS_CCP_OPERAND_POSITION] != PAUSE) && 
           (pRequestData[GHS_CCP_OPERAND_POSITION] != RUN)) 
        {
          GHS_CCP_ResponseCode(GHS_CCP_RESPONSE_CODE_INVALID_OPERAND);
        }
#if 0
        else if((pRequestData[GHS_CCP_OPERAND_POSITION] == RUN) && 
                (((pIdsStatus->Flags ) & RESERVOIR_ATTACHED) != RESERVOIR_ATTACHED))
        {
          GHS_CCP_ResponseCode(GHS_CCP_RESPONSE_CODE_PROCEDURE_NOT_APPLICABLE);
        }
        else
        {
          pIdsStatus->Therapy_Control_State = (GHS_CCP_Therapy_Control_State_t)(pRequestData[GHS_CCP_OPERAND_POSITION]);
          GHS_APP_SetStatus(pIdsStatus);
          LOG_INFO_APP("SUCCESS\r\n");
          LOG_INFO_APP("Set timer for CCP Response task\r\n");
          UTIL_TIMER_Start(&(GHS_CCP_Context.TimerCCPResponse_Id));
        }
#endif
      }
      break;

    case GHS_CCP_SET_FLIGHT_MODE:
      {
        LOG_INFO_APP("GHS_CCP_SET_FLIGHT_MODE\r\n");

        GHS_CCP_Context.FlightMode = TRUE;
        GHS_CCP_ResponseCode(GHS_CCP_RESPONSE_CODE_SUCCESS);
      }
      break;
      
    case GHS_CCP_SNOOZE_ANNUNCIATION:
      {
        uint16_t AnnunciationInstanceId;
        GHS_Annunciation_Status_Value_t * pAnnunciationStatus;
        uint16_t StatusFlags;

        LOG_INFO_APP("GHS_CCP_SNOOZE_ANNUNCIATION\r\n");
        
        pAnnunciationStatus = GHS_APP_GetAnnunciationStatus();

        GHS_CCP_Context.Procedure.Operand[0] = pRequestData[GHS_CCP_OPERAND_POSITION];
        GHS_CCP_Context.Procedure.Operand[1] = pRequestData[GHS_CCP_OPERAND_POSITION + 1];

        AnnunciationInstanceId = (pRequestData[GHS_CCP_OPERAND_POSITION]) |
                                 ((pRequestData[GHS_CCP_OPERAND_POSITION + 1]) << 8);
        
        if((pAnnunciationStatus->Instance_ID != AnnunciationInstanceId) /*||
           (pAnnunciationStatus->Status == CONFIRMED)*/)
        {
          GHS_CCP_ResponseCode(GHS_CCP_RESPONSE_CODE_PROCEDURE_NOT_APPLICABLE);
        }
        else
        {
          pAnnunciationStatus->Status = SNOOZED;
          GHS_APP_SetAnnunciationStatus(pAnnunciationStatus);
          LOG_INFO_APP("SUCCESS\r\n");
          StatusFlags = GHS_APP_GetStatusChangedFlag();
          StatusFlags |= ANNUNCIATION_STATUS_CHANGED;
          GHS_APP_SetStatusChangedFlag(StatusFlags);
          LOG_INFO_APP("Set timer for CCP Response task\r\n");
          UTIL_TIMER_Start(&(GHS_CCP_Context.TimerCCPResponse_Id));
          UTIL_TIMER_Start(&(GHS_CCP_Context.TimerCCPAnnunciationPending_Id));
        }
      }
      break;
      
    case GHS_CCP_CONFIRM_ANNUNCIATION:
      {
        uint16_t AnnunciationInstanceId;
        GHS_Annunciation_Status_Value_t * pAnnunciationStatus;
        uint16_t StatusFlags;

        LOG_INFO_APP("GHS_CCP_CONFIRM_ANNUNCIATION\r\n");

        pAnnunciationStatus = GHS_APP_GetAnnunciationStatus();

        GHS_CCP_Context.Procedure.Operand[0] = pRequestData[GHS_CCP_OPERAND_POSITION];
        GHS_CCP_Context.Procedure.Operand[1] = pRequestData[GHS_CCP_OPERAND_POSITION + 1];

        AnnunciationInstanceId = (pRequestData[GHS_CCP_OPERAND_POSITION]) |
                                 ((pRequestData[GHS_CCP_OPERAND_POSITION + 1]) << 8);
        
        if((pAnnunciationStatus->Instance_ID != AnnunciationInstanceId) /*||
           (pAnnunciationStatus->Status == CONFIRMED)*/)
        {
          GHS_CCP_ResponseCode(GHS_CCP_RESPONSE_CODE_PROCEDURE_NOT_APPLICABLE);
        }
        else
        {
          pAnnunciationStatus->Status = CONFIRMED;
          GHS_APP_SetAnnunciationStatus(pAnnunciationStatus);
          LOG_INFO_APP("SUCCESS\r\n");
          StatusFlags = GHS_APP_GetStatusChangedFlag();
          StatusFlags |= ANNUNCIATION_STATUS_CHANGED;
          GHS_APP_SetStatusChangedFlag(StatusFlags);
          LOG_INFO_APP("Set timer for CCP Response task\r\n");
          UTIL_TIMER_Start(&(GHS_CCP_Context.TimerCCPResponse_Id));
        }
      }
      break;
      
    case GHS_CCP_READ_BASAL_RATE_PROFILE_TEMPLATE:
      {
        LOG_INFO_APP("GHS_CCP_READ_BASAL_RATE_PROFILE_TEMPLATE\r\n");
      }
      break;
      
    case GHS_CCP_WRITE_BASAL_RATE_PROFILE_TEMPLATE:
      {
        LOG_INFO_APP("GHS_CCP_WRITE_BASAL_RATE_PROFILE_TEMPLATE\r\n");
      }
      break;
      
    case GHS_CCP_GET_TBR_TEMPLATE:
      {
        LOG_INFO_APP("GHS_CCP_GET_TBR_TEMPLATE\r\n");
      }
      break;
      
    case GHS_CCP_SET_TBR_TEMPLATE:
      {
        LOG_INFO_APP("GHS_CCP_SET_TBR_TEMPLATE\r\n");
      }
      break;
      
    case GHS_CCP_SET_BOLUS:
      {
        LOG_INFO_APP("GHS_CCP_SET_BOLUS\r\n");
      }
      break;
      
    case GHS_CCP_CANCEL_BOLUS:
      {
        LOG_INFO_APP("GHS_CCP_CANCEL_BOLUS\r\n");
      }
      break;
      
    case GHS_CCP_GET_AVAILABLE_BOLUSES:
      {
        LOG_INFO_APP("GHS_CCP_GET_AVAILABLE_BOLUSES\r\n");
      }
      break;
      
    case GHS_CCP_GET_BOLUS_TEMPLATE:
      {
        LOG_INFO_APP("GHS_CCP_GET_BOLUS_TEMPLATE\r\n");
      }
      break;
      
    case GHS_CCP_SET_BOLUS_TEMPLATE:
      {
        LOG_INFO_APP("GHS_CCP_SET_BOLUS_TEMPLATE\r\n");
     }
      break;
      
    case GHS_CCP_GET_TEMPLATE_STATUS_AND_DETAILS:
      {
        LOG_INFO_APP("GHS_CCP_GET_TEMPLATE_STATUS_AND_DETAILS\r\n");
      }
      break;
      
    case GHS_CCP_RESET_TEMPLATE_STATUS:
      {
        LOG_INFO_APP("GHS_CCP_RESET_TEMPLATE_STATUS\r\n");
      }
      break;
      
    case GHS_CCP_ACTIVATE_PROFILE_TEMPLATES:
      {
        LOG_INFO_APP("GHS_CCP_ACTIVATE_PROFILE_TEMPLATES\r\n");
      }
      break;
      
    case GHS_CCP_START_PRIMING:
      {
        LOG_INFO_APP("GHS_CCP_START_PRIMING\r\n");
      }
      break;
      
    case GHS_CCP_STOP_PRIMING:
      {
        LOG_INFO_APP("GHS_CCP_STOP_PRIMING\r\n");
      }
      break;
      
    case GHS_CCP_SET_INITIAL_RESERVOIR_FILL_LEVEL:
      {
        LOG_INFO_APP("GHS_CCP_SET_INITIAL_RESERVOIR_FILL_LEVEL\r\n");
      }
      break;
      
    case GHS_CCP_RESET_RESERVOIR_INSULIN_OPERATION_TIME:
      {
        LOG_INFO_APP("GHS_CCP_RESET_RESERVOIR_INSULIN_OPERATION_TIME\r\n");
      }
      break;
      
    case GHS_CCP_READ_ISF_PROFILE_TEMPLATE:
      {
        LOG_INFO_APP("GHS_CCP_READ_ISF_PROFILE_TEMPLATE\r\n");
      }
      break;
      
    case GHS_CCP_WRITE_ISF_PROFILE_TEMPLATE:
      {
        LOG_INFO_APP("GHS_CCP_WRITE_ISF_PROFILE_TEMPLATE\r\n");
      }
      break;
      
    case GHS_CCP_READ_I2CHO_RATIO_PROFILE_TEMPLATE:
      {
        LOG_INFO_APP("GHS_CCP_READ_I2CHO_RATIO_PROFILE_TEMPLATE\r\n");
      }
      break;
      
    case GHS_CCP_WRITE_I2CHO_RATIO_PROFILE_TEMPLATE:
      {
        LOG_INFO_APP("GHS_CCP_WRITE_I2CHO_RATIO_PROFILE_TEMPLATE\r\n");
      }
      break;
      
    case GHS_CCP_GET_MAX_BOLUS_AMOUNT:
      {
        LOG_INFO_APP("GHS_CCP_GET_MAX_BOLUS_AMOUNT\r\n");
      }
      break;
      
    case GHS_CCP_SET_MAX_BOLUS_AMOUNT:
      {
        LOG_INFO_APP("GHS_CCP_SET_MAX_BOLUS_AMOUNT\r\n");
      }
      break;
      
    default:
      {
        GHS_CCP_ResponseCode(GHS_CCP_RESPONSE_CODE_OPCODE_NOT_SUPPORTED);
      }
      break;
  }
  LOG_INFO_APP("GHS_CCP_RequestHandler <<<\r\n");
} /* end GHS_CCP_RequestHandler() */


/**
* @brief CCP new write request permit check
* @param [in] pRequestData: Pointer to the request data byte array
* @param [in] pRequestDataLength: Length of the request data byte array
* @retval 0x00 when no error, error code otherwise
*/
uint8_t GHS_CCP_CheckRequestValid(uint8_t * pRequestData, uint8_t requestDataLength)
{
  uint8_t retval = 0x00;
//  uint16_t E2E_CRC;
  
  LOG_INFO_APP("GHS_CCP_CheckRequestValid >>>\r\n");
  GHS_CCP_Context.Procedure.OpCode = (pRequestData[GHS_CCP_OP_CODE_POSITION] |
                                      ((pRequestData[GHS_CCP_OP_CODE_POSITION + 1]) << 8));
  LOG_INFO_APP("CCP Procedure OpCode: 0x%02X\r\n", GHS_CCP_Context.Procedure.OpCode);
  
  if (GHS_APP_GetIDCCPCharacteristicIndicationEnabled() == FALSE)
  {
    return GHS_ATT_ERROR_CODE_CLIENT_CHAR_CONF_DESC_IMPROPERLY_CONFIGURED;
  }
  else if(!((GHS_CCP_Context.Procedure.OpCode >= 0x0F5A) && 
           (GHS_CCP_Context.Procedure.OpCode <= 0x148D)))
  {
    return GHS_ATT_ERROR_CODE_OPCODE_NOT_SUPPORTED;
  }

  switch(GHS_CCP_Context.Procedure.OpCode)
  {
    case GHS_CCP_SET_THERAPY_CONTROL_STATE:
      {
        LOG_INFO_APP("GHS_CCP_SET_THERAPY_CONTROL_STATE \r\n"); 
        if(requestDataLength != GHS_CCP_SET_THERAPY_CONTROL_STATE_LENGTH)
        {
          LOG_INFO_APP("GHS_ATT_ERROR_CODE_INVALIDE_OPERAND\r\n"); 
          retval = GHS_ATT_ERROR_CODE_INVALIDE_OPERAND;
        }
      }
      break;
     
    case GHS_CCP_SET_FLIGHT_MODE:
      {
        LOG_INFO_APP("GHS_CCP_SET_FLIGHT_MODE\r\n"); 
        if(requestDataLength != GHS_CCP_NO_OPERAND_REQUEST_LENGTH)
        {
          LOG_INFO_APP("GHS_ATT_ERROR_CODE_INVALIDE_OPERAND\r\n"); 
          retval = GHS_ATT_ERROR_CODE_INVALIDE_OPERAND;
        }
      }
      break;
     
    case GHS_CCP_SNOOZE_ANNUNCIATION:
      {
        LOG_INFO_APP("GHS_CCP_SNOOZE_ANNUNCIATION\r\n"); 
        if(requestDataLength != GHS_CCP_SNOOZE_ANNUNCIATION_LENGTH)
        {
          LOG_INFO_APP("GHS_ATT_ERROR_CODE_INVALIDE_OPERAND\r\n"); 
          retval = GHS_ATT_ERROR_CODE_INVALIDE_OPERAND;
        }
      }
      break;
     
    case GHS_CCP_CONFIRM_ANNUNCIATION:
      {
        LOG_INFO_APP("GHS_CCP_CONFIRM_ANNUNCIATION\r\n"); 
        if(requestDataLength != GHS_CCP_CONFIRM_ANNUNCIATION_LENGTH)
        {
          LOG_INFO_APP("GHS_ATT_ERROR_CODE_INVALIDE_OPERAND\r\n"); 
          retval = GHS_ATT_ERROR_CODE_INVALIDE_OPERAND;
        }
      }
      break;
     
    case GHS_CCP_READ_BASAL_RATE_PROFILE_TEMPLATE:
      {
        LOG_INFO_APP("GHS_CCP_READ_BASAL_RATE_PROFILE_TEMPLATE\r\n"); 
        if(requestDataLength != GHS_CCP_READ_BASAL_RATE_LENGTH)
        {
          LOG_INFO_APP("GHS_ATT_ERROR_CODE_INVALIDE_OPERAND\r\n"); 
          retval = GHS_ATT_ERROR_CODE_INVALIDE_OPERAND;
        }
      }
      break;
     
    case GHS_CCP_WRITE_BASAL_RATE_PROFILE_TEMPLATE:
      {
        LOG_INFO_APP("GHS_CCP_WRITE_BASAL_RATE_PROFILE_TEMPLATE\r\n"); 
        if(requestDataLength != GHS_CCP_NO_OPERAND_REQUEST_LENGTH)
        {
          LOG_INFO_APP("GHS_ATT_ERROR_CODE_INVALIDE_OPERAND\r\n"); 
          retval = GHS_ATT_ERROR_CODE_INVALIDE_OPERAND;
        }
      }
      break;
     
     case GHS_CCP_GET_TBR_TEMPLATE:
      {
        LOG_INFO_APP("GHS_CCP_GET_TBR_TEMPLATE\r\n"); 
        if(requestDataLength != GHS_CCP_GET_TBR_TEMPLATE_LENGTH)
        {
          LOG_INFO_APP("GHS_ATT_ERROR_CODE_INVALIDE_OPERAND\r\n"); 
          retval = GHS_ATT_ERROR_CODE_INVALIDE_OPERAND;
        }
      }
      break;
     
     case GHS_CCP_SET_TBR_TEMPLATE:
      {
        LOG_INFO_APP("GHS_CCP_SET_TBR_TEMPLATE\r\n"); 
        if(requestDataLength != GHS_CCP_SET_TBR_TEMPLATE_LENGTH)
        {
          LOG_INFO_APP("GHS_ATT_ERROR_CODE_INVALIDE_OPERAND\r\n"); 
          retval = GHS_ATT_ERROR_CODE_INVALIDE_OPERAND;
        }
      }
      break;
     
     case GHS_CCP_SET_BOLUS:
      {
        LOG_INFO_APP("GHS_CCP_SET_BOLUS\r\n"); 
        if(requestDataLength != GHS_CCP_SET_BOLUS_LENGTH)
        {
          LOG_INFO_APP("GHS_ATT_ERROR_CODE_INVALIDE_OPERAND\r\n"); 
          retval = GHS_ATT_ERROR_CODE_INVALIDE_OPERAND;
        }
      }
      break;
     
     case GHS_CCP_CANCEL_BOLUS:
      {
        LOG_INFO_APP("GHS_CCP_CANCEL_BOLUS\r\n"); 
        if(requestDataLength != GHS_CCP_CANCEL_BOLUS_LENGTH)
        {
          LOG_INFO_APP("GHS_ATT_ERROR_CODE_INVALIDE_OPERAND\r\n"); 
          retval = GHS_ATT_ERROR_CODE_INVALIDE_OPERAND;
        }
      }
      break;
     
     case GHS_CCP_GET_AVAILABLE_BOLUSES:
      {
        LOG_INFO_APP("GHS_CCP_GET_AVAILABLE_BOLUSES\r\n"); 
        if(requestDataLength != GHS_CCP_NO_OPERAND_REQUEST_LENGTH)
        {
          LOG_INFO_APP("GHS_ATT_ERROR_CODE_INVALIDE_OPERAND\r\n"); 
          retval = GHS_ATT_ERROR_CODE_INVALIDE_OPERAND;
        }
      }
      break;
     
     case GHS_CCP_GET_BOLUS_TEMPLATE:
      {
        LOG_INFO_APP("GHS_CCP_GET_BOLUS_TEMPLATE\r\n"); 
        if(requestDataLength != GHS_CCP_GET_BOLUS_TEMPLATE_LENGTH)
        {
          LOG_INFO_APP("GHS_ATT_ERROR_CODE_INVALIDE_OPERAND\r\n"); 
          retval = GHS_ATT_ERROR_CODE_INVALIDE_OPERAND;
        }
      }
      break;
     
     case GHS_CCP_SET_BOLUS_TEMPLATE:
      {
        LOG_INFO_APP("GHS_CCP_SET_BOLUS_TEMPLATE\r\n"); 
        if(requestDataLength != GHS_CCP_SET_BOLUS_TEMPLATE_LENGTH)
        {
          LOG_INFO_APP("GHS_ATT_ERROR_CODE_INVALIDE_OPERAND\r\n"); 
          retval = GHS_ATT_ERROR_CODE_INVALIDE_OPERAND;
        }
      }
      break;
     
     case GHS_CCP_GET_TEMPLATE_STATUS_AND_DETAILS:
      {
        LOG_INFO_APP("GHS_CCP_GET_TEMPLATE_STATUS_AND_DETAILS\r\n"); 
        if(requestDataLength != GHS_CCP_NO_OPERAND_REQUEST_LENGTH)
        {
          LOG_INFO_APP("GHS_ATT_ERROR_CODE_INVALIDE_OPERAND\r\n"); 
          retval = GHS_ATT_ERROR_CODE_INVALIDE_OPERAND;
        }
      }
      break;
     
     case GHS_CCP_RESET_TEMPLATE_STATUS:
      {
        LOG_INFO_APP("GHS_CCP_RESET_TEMPLATE_STATUS\r\n"); 
        if(requestDataLength != GHS_CCP_RESET_TEMPLATE_STATUS_LENGTH)
        {
          LOG_INFO_APP("GHS_ATT_ERROR_CODE_INVALIDE_OPERAND\r\n"); 
          retval = GHS_ATT_ERROR_CODE_INVALIDE_OPERAND;
        }
      }
      break;
     
     case GHS_CCP_READ_ISF_PROFILE_TEMPLATE:
      {
        LOG_INFO_APP("GHS_CCP_READ_ISF_PROFILE_TEMPLATE\r\n"); 
        if(requestDataLength != GHS_CCP_NO_OPERAND_REQUEST_LENGTH)
        {
          LOG_INFO_APP("GHS_ATT_ERROR_CODE_INVALIDE_OPERAND\r\n"); 
          retval = GHS_ATT_ERROR_CODE_INVALIDE_OPERAND;
        }
      }
      break;
     
     case GHS_CCP_READ_I2CHO_RATIO_PROFILE_TEMPLATE:
      {
        LOG_INFO_APP("GHS_CCP_READ_I2CHO_RATIO_PROFILE_TEMPLATE\r\n"); 
        if(requestDataLength != GHS_CCP_NO_OPERAND_REQUEST_LENGTH)
        {
          LOG_INFO_APP("GHS_ATT_ERROR_CODE_INVALIDE_OPERAND\r\n"); 
          retval = GHS_ATT_ERROR_CODE_INVALIDE_OPERAND;
        }
      }
      break;
      
      case GHS_CCP_WRITE_I2CHO_RATIO_PROFILE_TEMPLATE:
      {
        LOG_INFO_APP("GHS_CCP_WRITE_I2CHO_RATIO_PROFILE_TEMPLATE\r\n"); 
        if(requestDataLength != GHS_CCP_WRITE_I2CHO_RATIO_PROFILE_TEMPLATE_LENGTH)
        {
          LOG_INFO_APP("GHS_ATT_ERROR_CODE_INVALIDE_OPERAND\r\n"); 
          retval = GHS_ATT_ERROR_CODE_INVALIDE_OPERAND;
        }
      }
      break;
      
      case GHS_CCP_GET_MAX_BOLUS_AMOUNT:
      {
        LOG_INFO_APP("GHS_CCP_GET_MAX_BOLUS_AMOUNT\r\n"); 
        if(requestDataLength != GHS_CCP_NO_OPERAND_REQUEST_LENGTH)
        {
          LOG_INFO_APP("GHS_ATT_ERROR_CODE_INVALIDE_OPERAND\r\n"); 
          retval = GHS_ATT_ERROR_CODE_INVALIDE_OPERAND;
        }
      }
      break;
      
      case GHS_CCP_SET_MAX_BOLUS_AMOUNT:
      {
        LOG_INFO_APP("GHS_CCP_SET_MAX_BOLUS_AMOUNT\r\n"); 
        if(requestDataLength != GHS_CCP_SET_MAX_BOLUS_AMOUNT_LENGTH)
        {
          LOG_INFO_APP("GHS_ATT_ERROR_CODE_INVALIDE_OPERAND\r\n"); 
          retval = GHS_ATT_ERROR_CODE_INVALIDE_OPERAND;
        }
      }
      break;
      
    default:
      {
        return GHS_ATT_ERROR_CODE_OPCODE_NOT_SUPPORTED;
      }
      break;
  }
  
  GHS_CCP_Context.E2E_Counter = pRequestData[requestDataLength - 3];  
  LOG_INFO_APP("E2E_Counter %d\r\n", GHS_CCP_Context.E2E_Counter);
#if 0
  if(((GHS_APP_GetFeature()) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
  {
    E2E_CRC = (pRequestData[requestDataLength - 1] << 8) | pRequestData[requestDataLength - 2];
    LOG_INFO_APP("requestDataLength: 0x%x\r\n", requestDataLength); 
    LOG_INFO_APP("Received E2E_CRC: 0x%x\r\n", E2E_CRC); 
    LOG_INFO_APP("Calculated E2E_CRC: 0x%x\r\n", GHS_APP_ComputeCRC(pRequestData, requestDataLength - 2)); 

    if (GHS_APP_ComputeCRC(pRequestData, requestDataLength - 2) != E2E_CRC)
    {
      LOG_INFO_APP("Invalide E2E_CRC\r\n"); 
      retval = GHS_ATT_ERROR_CODE_INVALID_CRC;
    }
  }
#endif  
  LOG_INFO_APP("GHS_CCP_CheckRequestValid <<<\r\n");
  return retval;
} /* end of GHS_CCP_CheckRequestValid() */

/**
  * @brief Process the acknowledge of sent indication
  * @param None
  * @retval None
  */
void GHS_CCP_AcknowledgeHandler(void)
{
  // STM_TODO : Implement indication acknowledge reception handler, in fact all the operations should be finished after the indication acknowledge is received from GATT client, if not received within 30sec, procedure failed
} /* end GHS_CCP_AcknowledgeHandler() */
