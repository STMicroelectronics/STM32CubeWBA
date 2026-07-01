/**
  ******************************************************************************
  * @file    esl_profile_ap.h
  * @author  GPM WBL Application Team
  * @brief   Header file for ESL profile.
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
#ifndef ESL_PROFILE_AP_H
#define ESL_PROFILE_AP_H

#include <stdbool.h>
#include <stdint.h>
#include "nvm_db.h"

/* Exported macro ------------------------------------------------------------*/

#define MAX_GROUPS                              PAWR_NUM_SUBEVENTS
#define MAX_ESL_PAYLOAD_SIZE                    (48U)

#define AP_NVM_ID                               (0U)

/* AD Type */
#define AD_TYPE_ELECTRONIC_SHELF_LABEL          (0x34)
#define AD_TYPE_ENCRYPTED_ADVERTISING_DATA      (0x31)

/* Codes for Commands */
#define ESL_CMD_PING                            (0x00)
#define ESL_CMD_READ_SENSOR_DATA                (0x10)
#define ESL_CMD_DISPLAY_IMG                     (0x20)
#define ESL_CMD_LED_CONTROL                     (0xB0)

#define ESL_CMD_UNASSOCIATE_FROM_AP             (0x01)
#define ESL_CMD_SERVICE_RESET                   (0x02)
#define ESL_CMD_FACTORY_RESET                   (0x03)
#define ESL_CMD_REFRESH_IMG                     (0x11)
#define ESL_CMD_DISPLAY_TIMED_IMG               (0x60)
#define ESL_CMD_LED_TIMED_CONTROL               (0xF0)

/* Requests that the ESL return to the Synchronized state once synchronized */
#define ESL_CMD_UPDATE_COMPLETE                 (0x04)
    
/* Code for vendor-specific commands */
#define ESL_CMD_VS_PRICE                        (0x3F)
#define ESL_CMD_VS_TXT                          (0xBF)

/* Codes for Responses */
#define ESL_RESP_ERROR                          (0x00)
#define ESL_RESP_LED_STATE                      (0x01)
#define ESL_RESP_BASIC_STATE                    (0x10)
#define ESL_RESP_DISPLAY_STATE                  (0x11)
#define ESL_RESP_SENSOR_VALUE_TAG_NIBBLE        (0x0E) /* This is only the value of the Tag nibble. The Length is variable */
#define ESL_RESP_VS_OK                          (0x0F)

/* Basic state response flags */
#define BASIC_STATE_SERVICE_NEEDED_BIT          (0x01)
#define BASIC_STATE_SYNCHRONIZED_BIT            (0x02)
#define BASIC_STATE_ACTIVE_LED_BIT              (0x04)
#define BASIC_STATE_PENDING_LED_UPDATE_BIT      (0x08)
#define BASIC_STATE_PENDING_DISPLAY_UPDATE_BIT  (0x10)

/* Error codes for Error Response */
#define ERROR_INVALID_OPCODE                    (0x02)
#define ERROR_INVALID_PARAMETERS                (0x06)

/* Offset in the command where to find ESL ID */
#define ESL_ID_CMD_OFFSET                       (1)

#define BRC_ESL_ID                              (0xFF)

#define BRC_RETRANSMISSIONS                     (0)
#define UNC_RETRANSMISSIONS                     (0)


#define GET_PARAM_LENGTH_FROM_OPCODE(opcode)          ((((opcode) & 0xF0) >> 4) + 1)

#define GET_LENGTH_FROM_OPCODE(opcode)                (GET_PARAM_LENGTH_FROM_OPCODE(opcode) + 1)

#define GET_ESL_ADDRESS(group_id, esl_id)             ((uint16_t)(((group_id) << 8) | ((esl_id) & 0x00FF)))
#define GET_ESL_ID(esl_address)                       ((uint8_t)((esl_address) & 0xFF))
#define GET_GROUP_ID(esl_address)                     ((uint8_t)(((esl_address) >> 8 ) & 0xFF))

/* Exported types ------------------------------------------------------------*/

typedef struct
{
  uint8_t session_key[16];
  uint8_t iv[8];
} ESL_AP_key_material_t;

typedef __PACKED_STRUCT {
  uint8_t bd_address_type;
  uint8_t bd_address[6];  
  uint16_t esl_address;   
  ESL_AP_key_material_t esl_resp_key_material;
} esl_info_t;

typedef struct {
  uint16_t conn_handle;
  bool provisioning;
  bool configuring; /* true if AP will automatically configure ESL */
  esl_info_t conn_esl_info;
  ESL_AP_key_material_t ap_sync_key_material;
} ESL_AP_context_t;

typedef void (*resp_cb_t)(uint8_t group_id, uint8_t esl_id, uint8_t *resp);

/* External variables --------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

void ESL_AP_Init(void);

void ESL_AP_GenerateKeyMaterial(ESL_AP_key_material_t *p_key_material);

uint8_t ESL_AP_StoreESLInfo(const esl_info_t *esl_info);

uint8_t ESL_AP_GetESLInfoByBDAddress(uint8_t bd_address_type, const uint8_t bd_address[6], esl_info_t *p_esl_info, NVMDB_HandleType *p_db_h);

uint8_t ESL_AP_GetESLInfoByESLAddress(uint16_t esl_address, esl_info_t *p_esl_info, NVMDB_HandleType *p_db_h);

uint8_t ESL_AP_DeleteESLInfo(uint16_t esl_address);

uint8_t ESL_AP_ClearNVMDB(void);

uint8_t ESL_AP_UpdateESLInfo(uint8_t bd_address_type, const uint8_t bd_address[6], esl_info_t new_esl_info);

uint8_t ESL_AP_Command(uint8_t cmd_opcode, uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb);

uint8_t ESL_AP_CmdLedControl(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint8_t led_index, uint8_t led_component, uint64_t flash_pattern, uint8_t off_period, uint8_t on_period, uint16_t repeat);  

uint8_t ESL_AP_CmdTxt(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, const char *txt);

uint8_t ESL_AP_CmdPrice(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint16_t val_int, uint8_t val_fract);

uint8_t ESL_AP_CmdReadSensorData(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint8_t sensor_index);

uint8_t ESL_AP_CmdDisplayImage(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint8_t display_index, uint8_t image_index);

uint8_t ESL_AP_CmdLedTimedControl(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint8_t led_index, uint8_t led_component, uint64_t flash_pattern, uint8_t off_period, uint8_t on_period, uint16_t repeat, uint32_t absolute_time);

uint8_t ESL_AP_CmdDisplayTimedImage(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint8_t display_index, uint8_t image_index, uint32_t absolute_time);

uint8_t ESL_AP_CmdFactoryReset(void);

uint8_t ESL_AP_CmdUpdateComplete(void);

uint8_t ESL_AP_CmdRefreshDisplay(uint8_t group_id, uint8_t esl_id, uint8_t display_index, resp_cb_t resp_cb);

void ESL_AP_SubeventDataRequest(uint8_t subevent);

void ESL_AP_ResponseReport(uint8_t subevent, uint8_t response_slot, uint8_t data_length, uint8_t *data);

uint8_t ESL_AP_SetNewEslAddress(uint8_t group_id, uint8_t esl_id);

uint8_t ESL_AP_StartUpdate(uint8_t group_id, uint8_t esl_id);

void ESL_AP_UpdatingStateTransition(void);

uint8_t ESL_AP_CmdProvisioning(uint8_t addr_type, uint8_t address[6], uint8_t group_id, uint8_t esl_id);

void ESL_AP_ECPNotificationReceived(uint8_t *current_data_resp_p);

void ESL_AP_ESLConnected(uint16_t conn_handle, uint8_t bd_address_type, const uint8_t bd_address[6]);

uint8_t ESL_AP_Reconnect(uint8_t address_type, const uint8_t address[6]);

void ESL_AP_DisconnectionComplete(uint16_t conn_handle);

#endif /* ESL_PROFILE_AP_H */
