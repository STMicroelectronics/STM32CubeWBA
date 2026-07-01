/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ots.h
  * @author  GPAM Application Team
  * @brief   Header file for ots.c
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#ifndef OTS_H
#define OTS_H

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
#define OBJECT_TRANSFER_SERVICE_UUID                                    (0x1825)
  
#define OBJECT_ID_SIZE                                                6

/* Result codes for OACP responses */
#define OACP_RESULT_SUCCESS                                         0x01
#define OACP_RESULT_NOT_SUPPORTED                                   0x02
#define OACP_RESULT_INVALID_PARAM                                   0x03
#define OACP_RESULT_INSUFF_RESOURCES                                0x04
#define OACP_RESULT_INVALID_OBJ                                     0x05
#define OACP_RESULT_CHANNEL_UNAVAILABLE                             0x06
#define OACP_RESULT_UNSUPPORTED_TYPE                                0x07
#define OACP_RESULT_PROC_NOT_PERMITTED                              0x08
#define OACP_RESULT_OBJ_LOCKED                                      0x09
#define OACP_RESULT_OPERATION_FAILED                                0x0A
  
  
/* Op Codes for OLCP procedures */
#define OLCP_OPCODE_FIRST                                           0x01
#define OLCP_OPCODE_LAST                                            0x02
#define OLCP_OPCODE_PREVIOUS                                        0x03
#define OLCP_OPCODE_NEXT                                            0x04
#define OLCP_OPCODE_GOTO                                            0x05
#define OLCP_OPCODE_ORDER                                           0x06
#define OLCP_OPCODE_REQUEST_NUM_OBJ                                 0x07
#define OLCP_OPCODE_CLEAR_MARKING                                   0x08
#define OLCP_OPCODE_RESPONSE                                        0x70
  
/* Result codes for OLCP responses */
#define OLCP_RESULT_SUCCESS                                         0x01
#define OLCP_RESULT_NOT_SUPPORTED                                   0x02
#define OLCP_RESULT_INVALID_PARAM                                   0x03
#define OLCP_RESULT_OPERATION_FAILED                                0x04
#define OLCP_RESULT_OUT_OF_BOUNDS                                   0x05
#define OLCP_RESULT_TOO_MANY_OBJ                                    0x06
#define OLCP_RESULT_NO_OBJ                                          0x07
#define OLCP_RESULT_OBJ_ID_NOT_FOUND                                0x08
  
/* Write mode bits */
#define WRITE_MODE_TRUNCATE_MASK                                    0x02  
/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  OTS_OTF,
  OTS_OBN,
  OTS_OBT,
  OTS_OBS,
  OTS_OBI,
  OTS_OBP,
  OTS_OACP,
  OTS_OLCP,
  /* USER CODE BEGIN Service1_CharOpcode_t */

  /* USER CODE END Service1_CharOpcode_t */
  OTS_CHAROPCODE_LAST
} OTS_CharOpcode_t;

typedef enum
{
  OTS_OTF_READ_EVT,
  OTS_OBN_READ_EVT,
  OTS_OBT_READ_EVT,
  OTS_OBS_READ_EVT,
  OTS_OBI_READ_EVT,
  OTS_OBP_READ_EVT,
  OTS_OACP_WRITE_EVT,
  OTS_OACP_INDICATE_ENABLED_EVT,
  OTS_OACP_INDICATE_DISABLED_EVT,
  OTS_OLCP_WRITE_EVT,
  OTS_OLCP_INDICATE_ENABLED_EVT,
  OTS_OLCP_INDICATE_DISABLED_EVT,
  /* USER CODE BEGIN Service1_OpcodeEvt_t */

  /* USER CODE END Service1_OpcodeEvt_t */
  OTS_BOOT_REQUEST_EVT
} OTS_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service1_Data_t */

  /* USER CODE END Service1_Data_t */
} OTS_Data_t;

typedef struct
{
  OTS_OpcodeEvt_t       EvtOpcode;
  OTS_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service1_NotificationEvt_t */

  /* USER CODE END Service1_NotificationEvt_t */
} OTS_NotificationEvt_t;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */
extern uint16_t OTS_ServiceStartHandle;
extern uint16_t OTS_ServiceEndHandle;
/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void OTS_Init(void);
void OTS_Notification(OTS_NotificationEvt_t *p_Notification);
tBleStatus OTS_UpdateValue(OTS_CharOpcode_t CharOpcode, OTS_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*OTS_H */
