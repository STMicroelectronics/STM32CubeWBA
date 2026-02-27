/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ESLS_app.h
  * @author  MCD Application Team
  * @brief   Header for ESLS_app.c
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
#ifndef ESLS_APP_H
#define ESLS_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ble_types.h"
#include "ble_core.h"
/* USER CODE BEGIN Includes */
#include <stdint.h>  
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  ESLS_CONN_HANDLE_EVT,
  ESLS_DISCON_HANDLE_EVT,

  /* USER CODE BEGIN Service2_OpcodeNotificationEvt_t */

  /* USER CODE END Service2_OpcodeNotificationEvt_t */

  ESLS_LAST_EVT,
} ESLS_APP_OpcodeNotificationEvt_t;

typedef struct
{
  ESLS_APP_OpcodeNotificationEvt_t          EvtOpcode;
  uint16_t                                 ConnectionHandle;

  /* USER CODE BEGIN ESLS_APP_ConnHandleNotEvt_t */

  /* USER CODE END ESLS_APP_ConnHandleNotEvt_t */
} ESLS_APP_ConnHandleNotEvt_t;
/* USER CODE BEGIN ET */
typedef enum
{
  ESL_STATE_UNASSOCIATED,
  ESL_STATE_CONFIGURING,
  ESL_STATE_SYNCHRONIZED,
  ESL_STATE_UPDATING,
  ESL_STATE_TO_UNSYNCHRONIZED,
  ESL_STATE_UNSYNCHRONIZED,
  ESL_STATE_TO_UNASSOCIATED,
} ESL_APP_State_t;

typedef enum
{
  ESL_LED_INACTIVE,
  ESL_LED_ACTIVE,
} ESL_APP_LEDState_t;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* Error codes for Error Response (vendor-specific errors from 0xF0 to 0xFF). */
#define ERROR_UNSPECIFIED                       (0x01)
#define ERROR_INVALID_OPCODE                    (0x02)
#define ERROR_INVALID_STATE                     (0x03)
#define ERROR_INVALID_IMAGE_INDEX               (0x04)
#define ERROR_IMAGE_NOT_AVAILABLE               (0x05)
#define ERROR_INVALID_PARAMETERS                (0x06)
#define ERROR_CAPACITY_LIMIT                    (0x07)
#define ERROR_INSUFFICIENT_BATTERY              (0x08)
#define ERROR_INSUFFICIENT_RESOURCES            (0x09)
#define ERROR_RETRY                             (0x0A)
#define ERROR_QUEUE_FULL                        (0x0B)
#define ERROR_IMPLAUSIBLE_ABSOLUTE_TIME         (0x0C)

/* Basic state response flags */
#define BASIC_STATE_SERVICE_NEEDED_BIT          (0x01)
#define BASIC_STATE_SYNCHRONIZED_BIT            (0x02)
#define BASIC_STATE_ACTIVE_LED_BIT              (0x04)
#define BASIC_STATE_PENDING_LED_UPDATE_BIT      (0x08)
#define BASIC_STATE_PENDING_DISPLAY_UPDATE_BIT  (0x10)

/* ATT Error codes */
#define BLE_ATT_ERR_NONE                                (0x00U) /**< Not an error! */
#define BLE_ATT_ERR_INVALID_HANDLE                      (0x01U) /**< The attribute handle given was not valid on this server. */
#define BLE_ATT_ERR_READ_NOT_PERM                       (0x02U) /**< The attribute cannot be read. */
#define BLE_ATT_ERR_WRITE_NOT_PERM                      (0x03U) /**< The attribute cannot be written. */
#define BLE_ATT_ERR_INVALID_PDU                         (0x04U) /**< The attribute PDU was invalid. */
#define BLE_ATT_ERR_INSUFF_AUTHENTICATION               (0x05U) /**< The attribute requires authentication before it can be read or written. */
#define BLE_ATT_ERR_REQ_NOT_SUPP                        (0x06U) /**< Attribute server does not support the request received from the client. */
#define BLE_ATT_ERR_INVALID_OFFSET                      (0x07U) /**< Offset specified was past the end of the attribute. */
#define BLE_ATT_ERR_INSUFF_AUTHORIZATION                (0x08U) /**< The attribute requires authorization before it can be read or written. */
#define BLE_ATT_ERR_PREP_QUEUE_FULL                     (0x09U) /**< Too many prepare writes have been queued. */
#define BLE_ATT_ERR_ATTR_NOT_FOUND                      (0x0AU) /**< No attribute found within the given attribute handle range. */
#define BLE_ATT_ERR_ATTR_NOT_LONG                       (0x0BU) /**< The attribute cannot be read using the Read Blob Request. */
#define BLE_ATT_ERR_INSUFF_ENCR_KEY_SIZE                (0x0CU) /**< The Encryption Key Size used for encrypting this link is insufficient. */
#define BLE_ATT_ERR_INVAL_ATTR_VALUE_LEN                (0x0DU) /**< The attribute value length is invalid for the operation. */
#define BLE_ATT_ERR_UNLIKELY                            (0x0EU) /**< The attribute request that was requested has encountered an error
                                                                 *   that was unlikely, and therefore could not be completed as requested. */
#define BLE_ATT_ERR_INSUFF_ENCRYPT                      (0x0FU) /**< The attribute requires encryption before it can be read or written. */
#define BLE_ATT_ERR_UNSUPP_GRP_TYPE                     (0x10U) /**< The attribute type is not a supported grouping attribute as defined by
                                                                 *   a higher layer specification. */
#define BLE_ATT_ERR_INSUFF_RESOURCES                    (0x11U) /**< Insufficient Resources to complete the request. */
#define BLE_ATT_ERR_DB_OUT_OF_SYNC                      (0x12U) /**< The server requests the client to rediscover the database. */
#define BLE_ATT_ERR_VALUE_NOT_ALLOWED                   (0x13U) /**< The attribute parameter value was not allowed. */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void ESLS_APP_Init(void);
void ESLS_APP_EvtRx(ESLS_APP_ConnHandleNotEvt_t *p_Notification);
/* USER CODE BEGIN EFP */
void ESL_APP_ConnectionComplete(uint16_t connection_handle, uint16_t sync_handle, uint8_t Peer_Address_Type, uint8_t Peer_Address[6]);
void ESL_APP_PairingComplete(uint16_t connection_handle);
void ESL_APP_DisconnectionComplete(uint16_t connection_handle);
void ESL_APP_SyncLost(void);
void ESL_APP_SyncInfoReceived(uint16_t sync_handle);
void ESL_APP_AdvPayloadReceived(uint16_t pa_event, uint8_t *p_adv_data, uint8_t size);
uint8_t ESL_APP_SetESLAddress(uint16_t address);
void ESL_APP_SetAPSyncKeyMaterial(uint8_t key_material[24]);
void ESL_APP_SetESLResponseKeyMaterial(uint8_t key_material[24]);
void ESL_APP_SetCurrentAbsoluteTime(uint32_t curr_absolute_time);
void ESL_APP_ControlPointReceived(uint8_t *p_cmd, uint8_t size);
uint8_t ESL_APP_ConfiguringOrUpdatingState(void);
void ESL_APP_PairingRequest(uint16_t connHandle);
int ESL_APP_GetAddress(uint8_t *group_id_p, uint8_t *esl_id_p);
uint8_t ESL_APP_SetBasicStateBitmap(uint8_t basic_resp_bit);
void ESL_APP_ResetBasicStateBitmap(uint8_t basic_resp_bit);
void ESL_APP_SetLEDState(uint8_t index, ESL_APP_LEDState_t led_state);
void ESL_APP_CmdProcessRequestCB(void);
void ESL_APP_CmdProcess(void);
void ESL_APP_SerialCmdExecute(uint8_t * pRxBuffer, uint16_t iRxBufferSize);

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*ESLS_APP_H */
