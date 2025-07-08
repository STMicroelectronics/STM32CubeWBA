/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service1.h
  * @author  MCD Application Team
  * @brief   Header for service1.c
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef GHS_H
#define GHS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ble_types.h"
#include "ble_core.h"
#include "svc_ctl.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported defines ----------------------------------------------------------*/
/* USER CODE BEGIN ED */

/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  GHS_HSF,
  GHS_LHO,
  GHS_GHSCP,
  GHS_OSC,
  /* USER CODE BEGIN Service1_CharOpcode_t */

  /* USER CODE END Service1_CharOpcode_t */
  GHS_CHAROPCODE_LAST
} GHS_CharOpcode_t;

typedef enum
{
  GHS_HSF_READ_EVT,
  GHS_HSF_INDICATE_ENABLED_EVT,
  GHS_HSF_INDICATE_DISABLED_EVT,
  GHS_LHO_NOTIFY_ENABLED_EVT,
  GHS_LHO_NOTIFY_DISABLED_EVT,
  GHS_LHO_INDICATE_ENABLED_EVT,
  GHS_LHO_INDICATE_DISABLED_EVT,
  GHS_GHSCP_WRITE_EVT,
  GHS_GHSCP_INDICATE_ENABLED_EVT,
  GHS_GHSCP_INDICATE_DISABLED_EVT,
  GHS_OSC_INDICATE_ENABLED_EVT,
  GHS_OSC_INDICATE_DISABLED_EVT,
  /* USER CODE BEGIN Service1_OpcodeEvt_t */
  GHS_HSFOSCDESC_WRITE_EVT,
  /* USER CODE END Service1_OpcodeEvt_t */
  GHS_BOOT_REQUEST_EVT
} GHS_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service1_Data_t */

  /* USER CODE END Service1_Data_t */
} GHS_Data_t;

typedef struct
{
  GHS_OpcodeEvt_t       EvtOpcode;
  GHS_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service1_NotificationEvt_t */

  /* USER CODE END Service1_NotificationEvt_t */
} GHS_NotificationEvt_t;

/* USER CODE BEGIN ET */
typedef enum
{
  NO_FEATURE_FLAG = 0,
  SUPPORTED_DEVICE_SPECIALIZATIONS_FIELD_PRESENT = (1<<0),
} GHS_HSF_Flags_t;

typedef enum
{
  OBSERVATION_TYPE_PRESENT                 = 0,
  TIME_STAMP_PRESENT                       = 1,
  MEASUREMENT_DURATION_PRESENT             = 2,
  MEASUREMENT_STATUS_PRESENT               = 3,
  OBSERVATION_ID_PRESENT                   = 4,
  PATIENT_PRESENT                          = 5,
  SUPPLEMENT_INFORMATION_PRESENT           = 6,
  DERIVED_FROM_PRESENT                     = 7,
  IS_MEMBER_OF_PRESENT                     = 8,
  TLVS_PRESENT                             = 9
} GHS_HOB_Flags_t;

typedef enum
{
  NUMERIC                 = 1,
  SIMPLE_DISCRETE         = 2,
  STRING                  = 3,
  SIMPLE_ARRAY            = 4,
  COMPOUND_DISCRETE_EVENT = 5,
  COMPOUND_STATE_EVENT    = 6,
  COMPOUND                = 7,
  TLV_ENCODED             = 8,
  OBSERVATION             = 255
} GHS_Observation_Class_Type_t;

typedef enum
{
  INVALID                 = 0,
  QUESTIONABLE            = 1,
  NOT_AVAILABLE           = 2,
  CALIBRATING             = 3,
  TEST_DATA               = 4,
  EALY_ESTIMATE           = 5,
  MANUALLY_ENTERED        = 6,
  SETTING                 = 7,
  THRESHOLD_ERROR         = 14,
  THRESHOLD_DISABLED      = 15
} GHS_Measurement_Status_t;

typedef struct
{
  uint8_t ObservationClassType;
  uint16_t Length;
  uint8_t * p_Data;
} GHS_HealthObservationBodies_t;

typedef struct
{
  uint8_t Count;
  uint32_t * p_Data;
} GHS_DataTypes_t;

typedef struct
{
  uint8_t Flags;
  GHS_DataTypes_t SupportedObservationTypes;
  GHS_DataTypes_t SupportedDeviceSpecializations;
}GHS_HSF_t;

typedef struct
{
  uint8_t FirstSegment;
  uint8_t LastSegment;
  uint8_t SegmentCounter;
}GHS_SegmentationHeader_t;

typedef struct
{
  GHS_SegmentationHeader_t SegmentationHeader;
  GHS_HealthObservationBodies_t HealthObservationBody;
}GHS_LHO_t;

typedef struct
{
  uint32_t ObservationType;
  uint32_t MeasurementPeriod;
  uint32_t UpdateInterval;
}GHS_OSC_t;

typedef struct
{
  uint32_t ObservationType;
  uint16_t UnitCode;
  uint32_t LowerLimit;
  uint32_t UpperLimit;
  uint32_t Accuracy;
}GHS_ValidRangeAccuracy_t;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
/* New values defined on Core spec supplement (CSS v4), common profile and service error code
   It is used on PTS 5.3.0 */
#define GHS_ATT_ERROR_CODE_PROCEDURE_ALREADY_IN_PROGRESS                  (0xFE)
/* New values defined on Core spec supplement (CSS v4), common profile and service error code
   It is used on PTS 5.3.0 */
#define GHS_ATT_ERROR_CODE_PROCEDURE_ALREADY_IN_PROGRESS                  (0xFE)
  
/* New values defined on Core spec supplement (CSS v4), common profile and service error code
   It is used on PTS 5.3.0 */
#define GHS_ATT_ERROR_CODE_CLIENT_CHAR_CONF_DESC_IMPROPERLY_CONFIGURED    (0xFD)
#define GHS_ATT_ERROR_CODE_OPCODE_NOT_SUPPORTED                           (0x81)                  
#define GHS_ATT_ERROR_CODE_INVALIDE_OPERAND                               (0x71)                  
#define GHS_ATT_ERROR_CODE_PROCEDURE_NOT_COMPLETED                        (0x72)                  
#define GHS_ATT_ERROR_CODE_PARAMETER_OUT_OF_RANGE                         (0x73)                  
#define GHS_ATT_ERROR_CODE_PROCEDURE_NOT_APPLICABLE                       (0x74)                  
#define GHS_ATT_ERROR_CODE_OUT_OF_RANGE                                   (0xFF)
  
#define NB_SUPPORTED_OBSERVATION_TYPES                                       (2)
/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void GHS_Init(void);
void GHS_Notification(GHS_NotificationEvt_t *p_Notification);
tBleStatus GHS_UpdateValue(GHS_CharOpcode_t CharOpcode, GHS_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*GHS_H */
