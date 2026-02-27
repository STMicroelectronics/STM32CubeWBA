/**
  ******************************************************************************
  * @file    esl_profile_ap.c
  * @author  GPM WBL Application Team
  * @brief   Implementation of ESL commands management.
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
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "main.h"
#include "app_common.h"
#include "ble_core.h"
#include "esl_profile_ap.h"
#include "stm_list.h"
#include "gatt_client_app.h"    
#include "stm32_seq.h"
#include "app_ble.h"
#include "log_module.h"
#include "esl_profile_ap.h"
#include "otp_client.h"

#define MAX_COMMAND_SIZE                        (17U)
#define MAX_ESL_PAYLOAD_SIZE                    (48U)
#define EAD_MIC_SIZE                            (4U)
#define EAD_RANDOMZER_SIZE                      (5U)
#define MAX_UNENCRYPTED_ADV_PAYLOAD             (MAX_ESL_PAYLOAD_SIZE + 2)
#define MAX_ADV_PAYLOAD                         (MAX_UNENCRYPTED_ADV_PAYLOAD + EAD_RANDOMZER_SIZE + EAD_MIC_SIZE + 2)
    
#define ESL_AP_ESL_INFO_RECORD_TYPE                                            0
#define ESL_AP_SYNC_KEY_RECORD_TYPE                                            1

typedef __PACKED_STRUCT Subevent_Data_Parameters_t_s {
  /** The number of octets in the Subevent_Data parameter.
   *  Values:
   *  - 0x00 ... 0xFB
   */
  uint8_t Subevent;
  /** The number of response slots to be used.
   */
  uint8_t Response_Slot_Start;
  /** The number of response slots to be used.
   */
  uint8_t Response_Slot_Count;
  /** The number of octets in buffer pointed by Subevent_Data parameter.
   *  Values:
   *  - 0x00 ... 0xFB
   */
  uint8_t Subevent_Data_Length;
  /** Advertising data formatted as defined in [Vol 3] Part C, Section 11.
   */
  uint8_t Subevent_Data[0];
} Subevent_Data_Parameters_t;

typedef struct {
  uint8_t group_id;
  uint8_t esl_id;
  resp_cb_t resp_cb;
  uint8_t cmd[MAX_COMMAND_SIZE];  
} cmd_ECP_buff_t;
    
typedef struct {
  tListNode node;
  uint8_t tx_count;
  resp_cb_t resp_cb;
  uint8_t cmd[0];  
} cmd_buff_t;

typedef enum {
  PACKET_STATE_EMPTY,
  PACKET_STATE_WAITING_RESP,
}packet_state_t;

/* List of ESL bonded to AP */
typedef struct {
  tListNode cmd_queue;
  uint8_t adv_packet_len;
  uint8_t unencrypted_payload[MAX_UNENCRYPTED_ADV_PAYLOAD];
  uint8_t adv_packet[MAX_ADV_PAYLOAD];
  packet_state_t packet_state;
} esl_group_info_t;

esl_group_info_t esl_group_info[MAX_GROUPS];

ESL_AP_context_t ESL_AP_Context;

cmd_ECP_buff_t cmd_ECP_buff;

static uint8_t GetEslIdFromResponse(uint8_t subevent, uint8_t response_slot, uint8_t *esl_id_p);

static void ESL_AP_SaveKey_Task(void);

void ESL_AP_Init(void)
{  
  UTIL_SEQ_RegTask(1U << CFG_TASK_ESL_AP_SAVE_KEY, UTIL_SEQ_RFU, ESL_AP_SaveKey_Task);
  UTIL_SEQ_RegTask(1U << CFG_TASK_NVMDB_PROCESS, UTIL_SEQ_RFU, NVMDB_Tick);
  
  for(int i = 0; i < MAX_GROUPS; i++)
  {
    LST_init_head(&esl_group_info[i].cmd_queue);
  }
  
  NVMDB_Init();
  
  UTIL_SEQ_SetTask(1u << CFG_TASK_ESL_AP_SAVE_KEY, CFG_SEQ_PRIO_0);
  
  ESL_AP_Context.conn_handle = 0xFFFF;
}

void NVMDB_ProcessRequest(void)
{
  UTIL_SEQ_SetTask(1u << CFG_TASK_NVMDB_PROCESS, CFG_SEQ_PRIO_0);
}

static void ESL_AP_SaveKey_Task(void)
{
  NVMDB_status_t status;
  NVMDB_HandleType esl_db_h;
  NVMDB_RecordSizeType record_size;
  
  /* Check if there is a stored AP sync key material */
  NVMDB_HandleInit(AP_NVM_ID, &esl_db_h);
  
  status = NVMDB_ReadNextRecord(&esl_db_h, ESL_AP_SYNC_KEY_RECORD_TYPE, 0,
                                (uint8_t *)&ESL_AP_Context.ap_sync_key_material, sizeof(ESL_AP_Context.ap_sync_key_material),
                                &record_size);
  if(status == NVMDB_STATUS_OK)
  {
    LOG_INFO_APP("Retrieved saved AP sync key\n");
    
    LOG_DEBUG_APP("Session key: ");
    for(int i = sizeof(ESL_AP_Context.ap_sync_key_material.session_key); i >= 0; i--)
    {
      LOG_DEBUG_APP("%02X ", ESL_AP_Context.ap_sync_key_material.session_key[i]);
    }
    LOG_DEBUG_APP("\nIV: ");
    for(int i = sizeof(ESL_AP_Context.ap_sync_key_material.iv); i >= 0; i--)
    {
      LOG_DEBUG_APP("%02X ", ESL_AP_Context.ap_sync_key_material.iv[i]);
    }
    LOG_DEBUG_APP("\n");
  }
  else
  {
    if(status == NVMDB_STATUS_END_OF_DB)
    {
      LOG_INFO_APP("No saved AP Sync key\n");      
    }
    else
    {
      LOG_ERROR_APP("Error retrieving AP Sync key\n");
      NVMDB_Erase(AP_NVM_ID);    
      LOG_WARNING_APP("NVM erased\n");
    }
      
    ESL_AP_GenerateKeyMaterial(&ESL_AP_Context.ap_sync_key_material);
    
    LOG_INFO_APP("Generated new AP Sync key\n");
    
    LOG_DEBUG_APP("Session key: ");
    for(int i = sizeof(ESL_AP_Context.ap_sync_key_material.session_key); i >= 0; i--)
    {
      LOG_DEBUG_APP("%02X ", ESL_AP_Context.ap_sync_key_material.session_key[i]);
    }
    LOG_DEBUG_APP("\nIV: ");
    for(int i = sizeof(ESL_AP_Context.ap_sync_key_material.iv); i >= 0; i--)
    {
      LOG_DEBUG_APP("%02X ", ESL_AP_Context.ap_sync_key_material.iv[i]);
    }
    LOG_DEBUG_APP("\n");
    
    status = NVMDB_AppendRecord(&esl_db_h, ESL_AP_SYNC_KEY_RECORD_TYPE, 0, NULL, sizeof(ESL_AP_Context.ap_sync_key_material), &ESL_AP_Context.ap_sync_key_material);
    
    if(status != NVMDB_STATUS_OK)
    {
      LOG_ERROR_APP("Error while saving key\n");
      
      return;
    }
    
    LOG_INFO_APP("Key saved\n");
  }
}

/* insert the ESL on list if it is not already present, else update Conn_Handle */
uint8_t ESL_AP_StoreESLInfo(const esl_info_t *p_esl_info)
{
  esl_info_t esl_info_tmp;
  NVMDB_HandleType esl_db_h;
  NVMDB_status_t status;
  
  if(ESL_AP_GetESLInfoByBDAddress(p_esl_info->bd_address_type, p_esl_info->bd_address, &esl_info_tmp, &esl_db_h) == 0)
  {
    /* Device already present */
    LOG_INFO_APP("ESL already configured\n");
    if(memcmp(&esl_info_tmp, p_esl_info, sizeof(esl_info_t)) == 0)
    {
      LOG_DEBUG_APP("No need to update info\n");
      return 0;
    }
    else
    {
      LOG_DEBUG_APP("Record deleted\n");
      NVMDB_DeleteRecord(&esl_db_h);
    }
  }
  
  status = NVMDB_AppendRecord(&esl_db_h, ESL_AP_ESL_INFO_RECORD_TYPE, 0, NULL, sizeof(esl_info_t), p_esl_info);
  if(status != NVMDB_STATUS_OK)
  {
    LOG_ERROR_APP("Error while adding record in NVM\n");
    return 2;
  }
  
  LOG_INFO_APP("New record added\n");
  
  return 0;
}

uint8_t ESL_AP_DeleteESLInfo(uint16_t esl_address)
{
  NVMDB_HandleType esl_db_h;
  NVMDB_RecordSizeType record_size;
  NVMDB_status_t status;
  esl_info_t esl_info;
  
  LOG_DEBUG_APP("ESL_AP_DeleteESLInfo\n");
  
  NVMDB_HandleInit(AP_NVM_ID, &esl_db_h);
  
  do {
    
    status = NVMDB_ReadNextRecord(&esl_db_h, ESL_AP_ESL_INFO_RECORD_TYPE, 0, (uint8_t *)&esl_info, sizeof(esl_info_t), &record_size);
    
    if (esl_info.esl_address == esl_address)
    {
      NVMDB_DeleteRecord(&esl_db_h);
      aci_gap_remove_bonded_device(esl_info.bd_address_type, esl_info.bd_address);
      LOG_DEBUG_APP("Record removed\n");
      return 0;
    }    
  
  }while(status == NVMDB_STATUS_OK);
  
  if(status == NVMDB_STATUS_END_OF_DB)
  {
    /* No record found */
    return 1;
  }
  else
  {
    /* Error */
    return 2;
  }
}

/* Search by Peer address and return an ESL node on the list */
uint8_t ESL_AP_GetESLInfoByBDAddress(uint8_t bd_address_type, const uint8_t bd_address[6], esl_info_t *p_esl_info, NVMDB_HandleType *p_db_h)
{  
  NVMDB_RecordSizeType record_size;
  NVMDB_status_t status;
  
  NVMDB_HandleInit(AP_NVM_ID, p_db_h);
  
  do {
    
    status = NVMDB_ReadNextRecord(p_db_h, ESL_AP_ESL_INFO_RECORD_TYPE, 0, (uint8_t *)p_esl_info, sizeof(esl_info_t), &record_size);
    
    if (p_esl_info->bd_address_type == bd_address_type
        && (memcmp(p_esl_info->bd_address, bd_address, sizeof(p_esl_info->bd_address)) == 0))
    {
      return 0;
    }    
  
  }while(status == NVMDB_STATUS_OK);
  
  if(status == NVMDB_STATUS_END_OF_DB)
  {
    /* No record found */
    return 1;
  }
  else
  {
    /* Error */
    return 2;
  }
}

uint8_t ESL_AP_GetESLInfoByESLAddress(uint16_t esl_address, esl_info_t *p_esl_info, NVMDB_HandleType *p_db_h)
{  
  NVMDB_RecordSizeType record_size;
  NVMDB_status_t status;
  
  NVMDB_HandleInit(AP_NVM_ID, p_db_h);
  
  do {
    
    status = NVMDB_ReadNextRecord(p_db_h, ESL_AP_ESL_INFO_RECORD_TYPE, 0, (uint8_t *)p_esl_info, sizeof(esl_info_t), &record_size);
    
    if (p_esl_info->esl_address == esl_address)
    {
      return 0;
    }    
  
  }while(status == NVMDB_STATUS_OK);
  
  return 1; 
}

uint8_t ESL_AP_ClearNVMDB(void)
{
  uint8_t ret;
  
  LOG_DEBUG_APP("Clear Security DB\n");
  
  ret = aci_gap_clear_security_db();
  if(ret != BLE_STATUS_SUCCESS)
    return 1;
  
  LOG_DEBUG_APP("Clear ESL info DB\n");
  
  ret = NVMDB_Erase(AP_NVM_ID);
  
  if(ret != NVMDB_STATUS_OK)
    return 2;
  
  return 0;
}

void ESL_AP_GenerateKeyMaterial(ESL_AP_key_material_t *p_key_material)
{
  hci_le_rand(&p_key_material->session_key[0]);
  hci_le_rand(&p_key_material->session_key[8]);
  
  hci_le_rand(p_key_material->iv);
}

static void * PrepareCmdBuff(uint8_t cmd_opcode, uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb)
{
  uint8_t cmd_length;
  cmd_buff_t * cmd_buff;
  
  if(group_id >= MAX_GROUPS)
    return NULL;
  
  cmd_length = GET_LENGTH_FROM_OPCODE(cmd_opcode);
  
  cmd_buff = malloc(sizeof(cmd_buff_t)+cmd_length);
  
  if(cmd_buff == NULL)
    return NULL;
  
  LST_insert_tail(&esl_group_info[group_id].cmd_queue, &cmd_buff->node);
  
  cmd_buff->resp_cb = resp_cb;
  
  if(esl_id == BRC_ESL_ID)
  {
    cmd_buff->tx_count = BRC_RETRANSMISSIONS + 1;
  }
  else
  {
    cmd_buff->tx_count = UNC_RETRANSMISSIONS + 1;
  }
  
  cmd_buff->cmd[0] = cmd_opcode;
  cmd_buff->cmd[1] = esl_id;
  
  return cmd_buff;
}

// to prepare cmd packet on Updating state and send command by ECP char
static uint8_t PrepareCmdECPBuff(uint8_t cmd_opcode, uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb)
{
  uint8_t cmd_length;
  
  if(group_id >= MAX_GROUPS)
    return 0;
  
  cmd_length = GET_LENGTH_FROM_OPCODE(cmd_opcode);
  
  cmd_ECP_buff.group_id = group_id;
  cmd_ECP_buff.esl_id = esl_id;
  
  cmd_ECP_buff.resp_cb = resp_cb;
  
  cmd_ECP_buff.cmd[0] = cmd_opcode;
  cmd_ECP_buff.cmd[1] = esl_id;
  
  return cmd_length;
}

/* If bResponse is true the command wait for an ESL response
   else the command has no response */
static uint8_t ESL_AP_SendCmdByECP(uint8_t cmd_length, bool bResponse)
{
  tBleStatus ble_status;
  
  ble_status = GATT_CLIENT_APP_WriteECP(ESL_AP_Context.conn_handle, cmd_ECP_buff.cmd, cmd_length, bResponse);
  if (ble_status != BLE_STATUS_SUCCESS)
  {
    LOG_ERROR_APP("==>> GATT_CLIENT_APP_WriteECP - fail, result: 0x%02X\n", ble_status);
  }
  else
  {
    LOG_DEBUG_APP("==>> Success: GATT_CLIENT_APP_WriteECP\n");
  }
  
  return ble_status;
}

void ESL_AP_ECPNotificationReceived(uint8_t *current_data_resp_p)
{
  cmd_ECP_buff.resp_cb(cmd_ECP_buff.group_id, cmd_ECP_buff.esl_id, current_data_resp_p);
}

uint8_t ESL_AP_Command(uint8_t cmd_opcode, uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb)
{
  uint8_t ret;
  cmd_buff_t * cmd_buff;
  uint8_t cmd_length;
  esl_info_t esl_info;
  NVMDB_HandleType db_h;

  if(esl_id != BRC_ESL_ID)
  {
    /* Return an ESL bonded to AP given the Group_ID and ESL_ID*/
    //TODO: instead of searching for ESL address, keep ESL address of connected ESL
    // and check if the provided ESL address matches the address of the connected one.
    ret = ESL_AP_GetESLInfoByESLAddress(GET_ESL_ADDRESS(group_id,esl_id), &esl_info, &db_h); 
    if (ret != 0)  
      return 2;
  }
  
  if (esl_id == BRC_ESL_ID || memcmp(get_bleAppContext_address(), esl_info.bd_address, BD_ADDR_SIZE) != 0) // TODO: check addr type
  { 
    /* ESL is not connected. */
    cmd_buff = PrepareCmdBuff(cmd_opcode, group_id, esl_id, resp_cb);
    if(cmd_buff == NULL)
      return 2;
  } 
  else
  {   
    /* ESL is connected. */
    cmd_length = PrepareCmdECPBuff(cmd_opcode, group_id, esl_id, resp_cb);
       
    if(cmd_length == 0)
      return 1;
    
    if (esl_id == BRC_ESL_ID)
      return 1;
    else
      ESL_AP_SendCmdByECP(cmd_length, true);
  }  
  return 0;
}

static void LedCmdBuff(uint8_t *cmd, uint8_t led_index, uint8_t led_component, uint64_t flash_pattern, uint8_t off_period, uint8_t on_period, uint16_t repeat) 
{
  cmd[2] = led_index;
  /* if the LED is a monochrome LED, the value of the Color fields shall be ignored.*/
  cmd[3] = led_component; /* RGB and Brightness component. Not used.  */                          
  
  //Flashing pattern
  memcpy(&cmd[4], &flash_pattern, 5);
  //Flashing off period
  cmd[9] = off_period;
  //Flashing on period
  cmd[10] = on_period;
  
  //Repeat type and duration
  memcpy(&cmd[11], &repeat, 2);
}

uint8_t ESL_AP_CmdLedControl(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint8_t led_index, uint8_t led_component, uint64_t flash_pattern, uint8_t off_period, uint8_t on_period, uint16_t repeat)
{
  uint8_t ret;
  cmd_buff_t * cmd_buff;
  uint8_t cmd_opcode = ESL_CMD_LED_CONTROL;
  esl_info_t esl_info;
  NVMDB_HandleType db_h;
  
  if(esl_id != BRC_ESL_ID)
  {
    /* Return an ESL bonded to AP given the Group_ID and ESL_ID*/
    ret = ESL_AP_GetESLInfoByESLAddress(GET_ESL_ADDRESS(group_id,esl_id), &esl_info, &db_h); 
    if (ret != 0)  
      return 2;
  }
  
  if (esl_id == BRC_ESL_ID || memcmp(get_bleAppContext_address(), esl_info.bd_address, BD_ADDR_SIZE) != 0)    
  {   
    // to prepare cmd packet on Synchronized state
    cmd_buff = PrepareCmdBuff(cmd_opcode, group_id, esl_id, resp_cb);
    
    if(cmd_buff == NULL)
      return 1;
    
    LedCmdBuff(cmd_buff->cmd, led_index, led_component, flash_pattern, off_period, on_period, repeat);
  }
  else
  {  
    // to prepare cmd packet on Updating state
    uint8_t cmd_length;
    
    cmd_length = PrepareCmdECPBuff(cmd_opcode, group_id, esl_id, resp_cb);
    
    if(cmd_length == 0)
      return 1;
    
    LedCmdBuff(cmd_ECP_buff.cmd, led_index, led_component, flash_pattern, off_period, on_period, repeat);
    // to send command by ECP char
    ESL_AP_SendCmdByECP(cmd_length, true);
  }  
  
  return 0;
}

/* Can send a maximum of 11 characters. */
uint8_t ESL_AP_CmdTxt(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, const char *txt)
{
  uint8_t ret;
  cmd_buff_t * cmd_buff;
  uint8_t cmd_opcode = ESL_CMD_VS_TXT;
  uint8_t cmd_length;
  esl_info_t esl_info;
  NVMDB_HandleType db_h;
  
  if(esl_id != BRC_ESL_ID)
  {  
    /* Return an ESL bonded to AP given the Group_ID and ESL_ID*/
    ret = ESL_AP_GetESLInfoByESLAddress(GET_ESL_ADDRESS(group_id,esl_id), &esl_info, &db_h); 
    if (ret != 0)  
      return 2;
  }
  
  if (esl_id == BRC_ESL_ID || memcmp(get_bleAppContext_address(), esl_info.bd_address, BD_ADDR_SIZE) != 0)
  { 
    // to prepare cmd packet on Synchronized state  
    cmd_buff = PrepareCmdBuff(cmd_opcode, group_id, esl_id, resp_cb);
    
    if(cmd_buff == NULL)
      return 1;
    
    cmd_length = GET_LENGTH_FROM_OPCODE(cmd_opcode);
    
    memset(&cmd_buff->cmd[2], 0, cmd_length-2);
    memcpy(&cmd_buff->cmd[2], txt, MIN(strlen(txt), cmd_length-2));
  }
  else
  {
    // to prepare cmd packet on Updating state
    cmd_length = PrepareCmdECPBuff(cmd_opcode, group_id, esl_id, resp_cb);
     
    if(cmd_length == 0)
      return 1;  
    
    memset(&cmd_ECP_buff.cmd[2], 0, cmd_length-2);
    memcpy(&cmd_ECP_buff.cmd[2], txt, MIN(strlen(txt), cmd_length-2));
    // to send command by ECP char
    ESL_AP_SendCmdByECP(cmd_length, true);
  }  
  
  return 0;  
}

uint8_t ESL_AP_CmdPrice(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint16_t val_int, uint8_t val_fract)
{
  uint8_t ret;
  cmd_buff_t * cmd_buff;
  uint8_t cmd_opcode = ESL_CMD_VS_PRICE;
  esl_info_t esl_info;
  NVMDB_HandleType db_h;
  
  if(esl_id != BRC_ESL_ID)
  {
    /* Return an ESL bonded to AP given the Group_ID and ESL_ID*/
    ret = ESL_AP_GetESLInfoByESLAddress(GET_ESL_ADDRESS(group_id,esl_id), &esl_info, &db_h); 
    if (ret != 0)  
      return 2;
  }
  
  if (esl_id == BRC_ESL_ID || memcmp(get_bleAppContext_address(), esl_info.bd_address, BD_ADDR_SIZE) != 0)
  { 
    // to prepare cmd packet on Synchronized state  
    cmd_buff = PrepareCmdBuff(cmd_opcode, group_id, esl_id, resp_cb);
    
    if(cmd_buff == NULL)
      return 1;
    
    HOST_TO_LE_16(&cmd_buff->cmd[2],val_int);
    cmd_buff->cmd[4] = val_fract;
  }
  else
  { 
    // to prepare cmd packet on Updating state  
    uint8_t cmd_length;
    
    cmd_length = PrepareCmdECPBuff(cmd_opcode, group_id, esl_id, resp_cb);
     
    if(cmd_length == 0)
      return 1;
    
    HOST_TO_LE_16(&cmd_ECP_buff.cmd[2],val_int);
    cmd_ECP_buff.cmd[4] = val_fract;
    // to send command by ECP char
    ESL_AP_SendCmdByECP(cmd_length, true);
  }
  
  return 0;  
}

uint8_t ESL_AP_CmdReadSensorData(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint8_t sensor_index)
{
  uint8_t ret;
  cmd_buff_t * cmd_buff;
  uint8_t cmd_opcode = ESL_CMD_READ_SENSOR_DATA;
  esl_info_t esl_info;
  NVMDB_HandleType db_h;
  
  if(esl_id != BRC_ESL_ID)
  {  
    /* Return an ESL bonded to AP given the Group_ID and ESL_ID*/
    ret = ESL_AP_GetESLInfoByESLAddress(GET_ESL_ADDRESS(group_id,esl_id), &esl_info, &db_h); 
    if (ret != 0)  
      return 2;
  }
  
  if (esl_id == BRC_ESL_ID || memcmp(get_bleAppContext_address(), esl_info.bd_address, BD_ADDR_SIZE) != 0)  
  {   
    // to prepare cmd packet on Synchronized state  
    cmd_buff = PrepareCmdBuff(cmd_opcode, group_id, esl_id, resp_cb);
    
    if(cmd_buff == NULL)
      return 1;
    
    cmd_buff->cmd[2] = sensor_index;
  }
  else
  {
    // to prepare cmd packet on Updating state  
    uint8_t cmd_length;
    
    cmd_length = PrepareCmdECPBuff(cmd_opcode, group_id, esl_id, resp_cb);
      
    if(cmd_length == 0)
      return 1;
    
    cmd_ECP_buff.cmd[2] = sensor_index;
    // to send command by ECP char
    ESL_AP_SendCmdByECP(cmd_length, true);
  }  
  
  return 0;  
}

uint8_t ESL_AP_CmdDisplayImage(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint8_t display_index, uint8_t image_index)
{
  uint8_t ret;
  cmd_buff_t * cmd_buff;
  uint8_t cmd_opcode = ESL_CMD_DISPLAY_IMG;
  esl_info_t esl_info;
  NVMDB_HandleType db_h;
  
  if(esl_id != BRC_ESL_ID)
  {
    /* Return an ESL bonded to AP given the Group_ID and ESL_ID*/
    ret = ESL_AP_GetESLInfoByESLAddress(GET_ESL_ADDRESS(group_id,esl_id), &esl_info, &db_h); 
    if (ret != 0)  
      return 2;
  }
  
  if (esl_id == BRC_ESL_ID || memcmp(get_bleAppContext_address(), esl_info.bd_address, BD_ADDR_SIZE) != 0)
  { 
    // to prepare cmd packet on Synchronized state  
    cmd_buff = PrepareCmdBuff(cmd_opcode, group_id, esl_id, resp_cb);
    
    if(cmd_buff == NULL)
      return 1;
    
    cmd_buff->cmd[2] = display_index;
    cmd_buff->cmd[3] = image_index;
  }
  else
  {
    // to prepare cmd packet on Updating state  
    uint8_t cmd_length;
    
    cmd_length = PrepareCmdECPBuff(cmd_opcode, group_id, esl_id, resp_cb);
  
    if(cmd_length == 0)
      return 1;
    
    cmd_ECP_buff.cmd[2] = display_index;
    cmd_ECP_buff.cmd[3] = image_index;  
    // to send command by ECP char
    ESL_AP_SendCmdByECP(cmd_length, true);
  }  

  return 0;  
}

uint8_t ESL_AP_CmdLedTimedControl(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, 
                                  uint8_t led_index, uint8_t led_component, 
                                  uint64_t flash_pattern, uint8_t off_period, 
                                  uint8_t on_period, uint16_t repeat, 
                                  uint32_t absolute_time)
{
  uint8_t ret;
  cmd_buff_t * cmd_buff;
  uint8_t cmd_opcode = ESL_CMD_LED_TIMED_CONTROL;
  esl_info_t esl_info;
  NVMDB_HandleType db_h;
  
  if(esl_id != BRC_ESL_ID)
  {
    /* Return an ESL bonded to AP given the Group_ID and ESL_ID*/
    ret = ESL_AP_GetESLInfoByESLAddress(GET_ESL_ADDRESS(group_id,esl_id), &esl_info, &db_h); 
    if (ret != 0)  
      return 2;
  }
  
  if (esl_id == BRC_ESL_ID || memcmp(get_bleAppContext_address(), esl_info.bd_address, BD_ADDR_SIZE) != 0)    
  {   
    // to prepare cmd packet on Synchronized state
    cmd_buff = PrepareCmdBuff(cmd_opcode, group_id, esl_id, resp_cb);
    
    if(cmd_buff == NULL)
      return 1;
    
    LedCmdBuff(cmd_buff->cmd, led_index, led_component, flash_pattern, off_period, on_period, repeat);
    HOST_TO_LE_32(&cmd_buff->cmd[13], absolute_time);
  }
  else
  {  
    // to prepare cmd packet on Updating state
    uint8_t cmd_length;
    
    cmd_length = PrepareCmdECPBuff(cmd_opcode, group_id, esl_id, resp_cb);
   
    if(cmd_length == 0)
      return 1;
    
    LedCmdBuff(cmd_ECP_buff.cmd, led_index, led_component, flash_pattern, off_period, on_period, repeat);
    HOST_TO_LE_32(&cmd_ECP_buff.cmd[13], absolute_time);
    // to send command by ECP char
    ESL_AP_SendCmdByECP(cmd_length, true);
  }  

  return 0;  
}

uint8_t ESL_AP_CmdDisplayTimedImage(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint8_t display_index, uint8_t image_index, uint32_t absolute_time)
{
  uint8_t ret;
  cmd_buff_t * cmd_buff;
  uint8_t cmd_opcode = ESL_CMD_DISPLAY_TIMED_IMG;
  esl_info_t esl_info;
  NVMDB_HandleType db_h;
  
  if(esl_id != BRC_ESL_ID)
  {
    /* Return an ESL bonded to AP given the Group_ID and ESL_ID*/
    ret = ESL_AP_GetESLInfoByESLAddress(GET_ESL_ADDRESS(group_id,esl_id), &esl_info, &db_h); 
    if (ret != 0)  
      return 2;
  }
  
  if (esl_id == BRC_ESL_ID || memcmp(get_bleAppContext_address(), esl_info.bd_address, BD_ADDR_SIZE) != 0)
  { 
    // to prepare cmd packet on Synchronized state  
    cmd_buff = PrepareCmdBuff(cmd_opcode, group_id, esl_id, resp_cb);
    
    if(cmd_buff == NULL)
      return 1;
    
    cmd_buff->cmd[2] = display_index;
    cmd_buff->cmd[3] = image_index;
    HOST_TO_LE_32(&cmd_buff->cmd[4], absolute_time);
  }
  else
  {
    // to prepare cmd packet on Updating state  
    uint8_t cmd_length;
    
    cmd_length = PrepareCmdECPBuff(cmd_opcode, group_id, esl_id, resp_cb);
  
    if(cmd_length == 0)
      return 1;
    
    cmd_ECP_buff.cmd[2] = display_index;
    cmd_ECP_buff.cmd[3] = image_index;
    HOST_TO_LE_32(&cmd_ECP_buff.cmd[4], absolute_time);
    // to send command by ECP char
    ESL_AP_SendCmdByECP(cmd_length, true);  
  }
 
  return 0;  
}

uint8_t ESL_AP_CmdRefreshDisplay(uint8_t group_id, uint8_t esl_id, uint8_t display_index, resp_cb_t resp_cb)
{
  uint8_t ret;
  cmd_buff_t * cmd_buff;
  uint8_t cmd_opcode = ESL_CMD_REFRESH_IMG;
  esl_info_t esl_info;
  NVMDB_HandleType db_h;
  
  if(esl_id != BRC_ESL_ID)
  {
    /* Return an ESL bonded to AP given the Group_ID and ESL_ID*/
    ret = ESL_AP_GetESLInfoByESLAddress(GET_ESL_ADDRESS(group_id,esl_id), &esl_info, &db_h); 
    if (ret != 0)  
      return 2;
  }
  
  if (esl_id == BRC_ESL_ID || memcmp(get_bleAppContext_address(), esl_info.bd_address, BD_ADDR_SIZE) != 0)  
  {   
    // to prepare cmd packet on Synchronized state  
    cmd_buff = PrepareCmdBuff(cmd_opcode, group_id, esl_id, resp_cb);
    
    if(cmd_buff == NULL)
      return 1;
    
    cmd_buff->cmd[2] = display_index;
  }
  else
  {
    // to prepare cmd packet on Updating state  
    uint8_t cmd_length;
    
    cmd_length = PrepareCmdECPBuff(cmd_opcode, group_id, esl_id, resp_cb);
    
    if(cmd_length == 0)
      return 1;
    
    cmd_ECP_buff.cmd[2] = display_index;
    // to send command by ECP char
    ESL_AP_SendCmdByECP(cmd_length, true);  
  }
  return 0;  
}

uint8_t ESL_AP_CmdFactoryReset(void)
{
  uint8_t cmd_length;

  /* The Factory Reset command is reserved for use in the Configuring state and  
     the Updating state, the AP shall not send the Factory Reset command to an 
     ESL that is in the Synchronized state. */ 
  
  cmd_length = PrepareCmdECPBuff(ESL_CMD_FACTORY_RESET,
                                    GET_GROUP_ID(ESL_AP_Context.conn_esl_info.esl_address),
                                    GET_ESL_ID(ESL_AP_Context.conn_esl_info.esl_address),
                                    NULL);
  
  if(cmd_length == 0)
    return 1;
  
  if(ESL_AP_SendCmdByECP(cmd_length, false) != BLE_STATUS_SUCCESS)
  {    
    return 1;      
  }
  
  ESL_AP_DeleteESLInfo(ESL_AP_Context.conn_esl_info.esl_address);
  
  return 0;
}

uint8_t ESL_AP_CmdUpdateComplete(void)
{
  uint8_t cmd_length;
  
  cmd_length = PrepareCmdECPBuff(ESL_CMD_UPDATE_COMPLETE,
                                    GET_GROUP_ID(ESL_AP_Context.conn_esl_info.esl_address),
                                    GET_ESL_ID(ESL_AP_Context.conn_esl_info.esl_address),
                                    NULL);
  
  if(cmd_length == 0)
    return 1;
  
  // to send command by ECP char
  if (ESL_AP_SendCmdByECP(cmd_length, false) == BLE_STATUS_SUCCESS)   //No response 
  {    
    /* After the Update Complete command sending, the AP shall start the PAST procedure */
    periodic_sync_info_transfer();
    LOG_DEBUG_APP("Synchronized State transition\n");
    return 0;
  }
  
  return 1;  
}

void ESL_AP_SubeventDataRequest(uint8_t subevent)
{
  tBleStatus ble_status;
  tListNode *list_head_p;
  cmd_buff_t* current_node_p;
  cmd_buff_t* node_to_remove_p = NULL;
  esl_group_info_t *esl_group_info_p;
  uint8_t *unencrypted_payload_p;
  uint8_t curr_payload_len;
  uint8_t subevent_param[sizeof(Subevent_Data_Parameters_t) + MAX_ADV_PAYLOAD];
  Subevent_Data_Parameters_t *subevent_data_parameters_p = (Subevent_Data_Parameters_t *)subevent_param;
  uint8_t cmd_length;
  uint8_t num_cmd = 0;
  uint16_t encrypted_data_len;
  
  /* subevent is the Group ID */  
  if(subevent >= MAX_GROUPS)
    return;
  
  esl_group_info_p = &esl_group_info[subevent];  
  
  if(esl_group_info_p->packet_state == PACKET_STATE_WAITING_RESP)
    return;
 
  unencrypted_payload_p = esl_group_info_p->unencrypted_payload;
  
  unencrypted_payload_p[1] = AD_TYPE_ELECTRONIC_SHELF_LABEL;
  
  /* First byte of synchronization packet for commands is the Group ID. */
  /* Group_ID with value N shall be trasmitted in a PAwR subevent with number N */
  unencrypted_payload_p[2] = subevent; 
  curr_payload_len = 3; 
  
  list_head_p = &esl_group_info_p->cmd_queue;
    
  current_node_p = (cmd_buff_t*)list_head_p->next;
  
  while(&current_node_p->node != list_head_p)
  {
    num_cmd++;
    
    if(num_cmd > PAWR_NUM_RESPONSE_SLOTS && current_node_p->cmd[ESL_ID_CMD_OFFSET] != BRC_ESL_ID)
    {
      /* We do not send a unicast command if we cannot receive a response for it.
         However we can send a broadcast command regardless of the number of
         response slots, because they have no response. */
      break;
    }
        
    cmd_length = GET_LENGTH_FROM_OPCODE(current_node_p->cmd[0]);
    
    if(curr_payload_len + cmd_length > MAX_ESL_PAYLOAD_SIZE)
      break;
    
    /*TODO: workaround may not be needed on WBA2. But need first to check
     if the response report is received only after the subevent data is sent. */
    /* NOTE: this is a workaround: command should be removed from the queue in 
       ESL_AP_ResponseReport(). However, no response report is received if the
       response packet is not received. So the command is removed at next data
       request event. This will be fixed in next version of BLE stack. 
       This workaround does not work if a subevent data request is received
       before next response report. */
    /* Remove command from list if transmission count has reached 0. */
    if(current_node_p->tx_count == 0)
    {
      LST_remove_node(&current_node_p->node);
      node_to_remove_p = current_node_p;
    }
    else 
    {
      current_node_p->tx_count--;      
      memcpy(&unencrypted_payload_p[curr_payload_len], current_node_p->cmd, cmd_length);
      curr_payload_len += cmd_length;
    }
    
    LST_get_next_node(&current_node_p->node, (tListNode **)&current_node_p);
    
    if(node_to_remove_p != NULL)
    {
      free(node_to_remove_p);
      node_to_remove_p = NULL;
    } 
  }
  
  if(curr_payload_len == 3)
  {
    /* No commands to be sent. */
    esl_group_info_p->adv_packet_len = 0;
    
    return;
  }
  
  unencrypted_payload_p[0] = curr_payload_len - 1;    /* Length of following data. */
  
  /*
  LOG_DEBUG_APP("Packet:\n"); 
  int i;
  for(i=0; i<curr_payload_len-1; i++)
  {
    LOG_DEBUG_APP("0x%02X - ", esl_payload_tag[i]);
  }  
  LOG_DEBUG_APP("0x%02X\n", esl_payload_tag[i]); */
  
  /* The "ap_sync_key_material" info is the same for all ESLs */
  aci_hal_ead_encrypt_decrypt(0, /* Encryption */
                              ESL_AP_Context.ap_sync_key_material.session_key,
                              ESL_AP_Context.ap_sync_key_material.iv,
                              curr_payload_len,
                              unencrypted_payload_p,
                              &encrypted_data_len,
                              &esl_group_info_p->adv_packet[2]);

  esl_group_info_p->adv_packet[0] = curr_payload_len + EAD_MIC_SIZE + EAD_RANDOMZER_SIZE + 1;
  esl_group_info_p->adv_packet[1] = AD_TYPE_ENCRYPTED_ADVERTISING_DATA;
  esl_group_info_p->adv_packet_len = esl_group_info_p->adv_packet[0] + 1;
  
    
  subevent_data_parameters_p->Subevent = subevent;
  subevent_data_parameters_p->Response_Slot_Start = 0;
  // TODO: calculate the used response slots (Response_Slot_Count) depending on the current commands.
  subevent_data_parameters_p->Response_Slot_Count = PAWR_NUM_RESPONSE_SLOTS;
  subevent_data_parameters_p->Subevent_Data_Length = esl_group_info_p->adv_packet_len;
  memcpy(subevent_data_parameters_p->Subevent_Data, esl_group_info_p->adv_packet, esl_group_info_p->adv_packet_len);
  
  ble_status = hci_le_set_periodic_advertising_subevent_data(0, 1, subevent_param);
  if (ble_status != BLE_STATUS_SUCCESS)
  {
    LOG_ERROR_APP("==>> hci_le_set_periodic_advertising_subevent_data - fail, result: 0x%02X\n", ble_status);
  }
  else
  {
    esl_group_info_p->packet_state = PACKET_STATE_WAITING_RESP;
    LOG_DEBUG_APP("==>> Success: hci_le_set_periodic_advertising_subevent_data\n");
  }  
}

void ESL_AP_ResponseReport(uint8_t subevent, uint8_t response_slot, uint8_t data_length, uint8_t *data)
{
  esl_group_info_t *esl_group_info_p;
  tListNode *list_head_p;
  cmd_buff_t* current_node_p;
  cmd_buff_t* node_to_remove_p = NULL;
  uint8_t esl_id;
  uint8_t *current_data_resp_p;
  uint8_t resp_length;
  uint8_t decrypted_data[MAX_ESL_PAYLOAD_SIZE];
  uint16_t decrypted_data_length;
  uint8_t encrypted_data_length;
  tBleStatus ret;
  esl_info_t esl_info;
  NVMDB_HandleType db_h;
  
  /* subevent correspond to group_id */
   
  if(subevent >= MAX_GROUPS)
    return;
  
  /* We assume only one AD type is present */
  if(data[0] != data_length - 1 || data[1] != AD_TYPE_ENCRYPTED_ADVERTISING_DATA)
    return;
  
  if(GetEslIdFromResponse(subevent, response_slot, &esl_id) != 0)
  {
    /* No corresponding ESL ID found for that response. */
    return;
  } 
  
  /* ADV data decryption */
  /* ADV packet length - 2 for Len and ED Tag (see Fig 5.1 ESL profile spec) */
  encrypted_data_length = data_length - 2;
  
  /* Subevent is equal to the group id */
  ret = ESL_AP_GetESLInfoByESLAddress(GET_ESL_ADDRESS(subevent,esl_id), &esl_info, &db_h); 
  if (ret != 0)  
    return;
  
  /* take the "esl_resp_key_material" info by esl_node */
  ret = aci_hal_ead_encrypt_decrypt(1, /* Decryption */
                                    esl_info.esl_resp_key_material.session_key,
                                    esl_info.esl_resp_key_material.iv,
                                    encrypted_data_length,
                                    &data[2],
                                    &decrypted_data_length,
                                    decrypted_data);  
  if(ret != BLE_STATUS_SUCCESS)
  {
    LOG_ERROR_APP("Decryption fail.\n");
    return;
  }
  
  decrypted_data_length = decrypted_data[0] + 1;

  if((decrypted_data_length != encrypted_data_length - EAD_RANDOMZER_SIZE - EAD_MIC_SIZE) ||
     (decrypted_data[1] != AD_TYPE_ELECTRONIC_SHELF_LABEL))
  {
    return;
  }  
 
  /* Inspect sent commands and link them to the responses. */
  esl_group_info_p = &esl_group_info[subevent];
  
  /* A response for a packet has been received. Mark buffer as empty. */
  esl_group_info_p->packet_state = PACKET_STATE_EMPTY;
  
  list_head_p = &esl_group_info_p->cmd_queue;
    
  current_node_p = (cmd_buff_t*)list_head_p->next;
  
  current_data_resp_p = decrypted_data + 2; /* Skip first 2 bytes for AD length and type. */
  
  while(&current_node_p->node != list_head_p && current_data_resp_p < decrypted_data + decrypted_data_length)
  {
    if(current_node_p->cmd[ESL_ID_CMD_OFFSET] == esl_id)
    {
      if(current_node_p->resp_cb != NULL)
      {
        current_node_p->resp_cb(subevent, esl_id, current_data_resp_p);
      }
      
      /* Command has received a response. Remove from queue. */      
      LST_remove_node(&current_node_p->node);      
      node_to_remove_p = current_node_p;
      
      /* Pass to following response. */
      resp_length = GET_LENGTH_FROM_OPCODE(current_data_resp_p[0]);
      current_data_resp_p += resp_length;
    }
    
    LST_get_next_node(&current_node_p->node, (tListNode **)&current_node_p);
    
    if(node_to_remove_p != NULL)
    {
      free(node_to_remove_p);
      node_to_remove_p = NULL;
    }    
  }  
}

static uint8_t GetEslIdFromResponse(uint8_t subevent, uint8_t response_slot, uint8_t *esl_id_p)
{
  esl_group_info_t *esl_group_info_p;
  uint8_t *esl_payload_p;
  uint8_t esl_payload_len;
  uint8_t *cmd_p;
  int8_t cmd_idx = -1;
  uint8_t cmd_length;
  
  if(subevent >= MAX_GROUPS)
    return 1;
  
  /* Inspect last sent ESL payload. */
  esl_group_info_p = &esl_group_info[subevent];
  esl_payload_p = &esl_group_info_p->unencrypted_payload[2]; /* Two bytes for AD length and type */
  esl_payload_len = esl_group_info_p->unencrypted_payload[0] - 1;
  
  cmd_p = &esl_payload_p[1]; /* First byte contains group id */
  
  while(cmd_p < esl_payload_p + esl_payload_len)
  {
    cmd_idx++;
    
    if(cmd_idx == response_slot)
    {
      if(cmd_p[ESL_ID_CMD_OFFSET] == BRC_ESL_ID)
      {
        return 1;
      }
      
      *esl_id_p = cmd_p[ESL_ID_CMD_OFFSET];
      
      return 0;
    }    
    
    cmd_length = GET_LENGTH_FROM_OPCODE(cmd_p[0]);    
    cmd_p += cmd_length;
  }
  
  return 1;
}

uint8_t ESL_AP_SetNewEslAddress(uint8_t group_id, uint8_t esl_id)
{
  if(group_id >= MAX_GROUPS)
    return 1;
  
  ESL_AP_Context.conn_esl_info.esl_address = GET_ESL_ADDRESS(group_id, esl_id);
  
  return GATT_CLIENT_APP_ConfigureESL(ESL_AP_Context.conn_handle);
}  

uint8_t ESL_AP_StartUpdate(uint8_t group_id, uint8_t esl_id)
{
  NVMDB_HandleType db_h;
  
  if(group_id >= MAX_GROUPS)
    return 1;
  
  if(ESL_AP_GetESLInfoByESLAddress(GET_ESL_ADDRESS(group_id,esl_id), &ESL_AP_Context.conn_esl_info, &db_h) != 0)
  {
    LOG_ERROR_APP("ESL_AP_StartUpdate: info not found\n");
    return 2;
  }
   
  ESL_AP_Context.configuring = false;
  
  UTIL_SEQ_SetTask( 1u << CFG_TASK_UPDATING_STATE_TRANSITION, CFG_SEQ_PRIO_0);
  
  return 0;
} 

//Task for CFG_TASK_UPDATING_STATE_TRANSITION
void ESL_AP_UpdatingStateTransition(void)
{  
  /* To transition an ESL from the Synchronized state to the Updating state, 
  the AP shall use the Periodic Advertising Connection procedure.
  When the AP connects with the ESL, the ESL transitions to the Updating state. */
  create_periodic_advertising_connection(GET_GROUP_ID(ESL_AP_Context.conn_esl_info.esl_address),
                                         ESL_AP_Context.conn_esl_info.bd_address_type,
                                         ESL_AP_Context.conn_esl_info.bd_address);
}

void ESL_AP_ESLConnected(uint16_t conn_handle, uint8_t bd_address_type, const uint8_t bd_address[6])
{
  ESL_AP_Context.conn_handle = conn_handle;
#ifndef PTS_OTP
  NVMDB_HandleType db_h;
    
  if(!ESL_AP_Context.provisioning)
  {
    /* We are connected to a configured ESL. */
    if(ESL_AP_GetESLInfoByBDAddress(bd_address_type, bd_address, &ESL_AP_Context.conn_esl_info, &db_h) != 0)
    {
      /* This should not happen since we should have already checked that there is a record before connecting. */
      aci_gap_terminate(conn_handle, HCI_REMOTE_USER_TERMINATED_CONNECTION_ERR_CODE);
      return;
    }
  }
#endif  
  GATT_CLIENT_APP_DiscoverServices(conn_handle, TRUE);
}

void ESL_AP_DisconnectionComplete(uint16_t conn_handle)
{  
  ESL_AP_Context.conn_handle = 0xFFFF;
  OTP_CLIENT_DisconnectionComplete();
}

uint8_t ESL_AP_CmdProvisioning(uint8_t addr_type, uint8_t address[6], uint8_t group_id, uint8_t esl_id)
{
  esl_info_t esl_info;
  NVMDB_HandleType db_h;
  uint16_t esl_address = GET_ESL_ADDRESS(group_id, esl_id);
  
  if(group_id >= MAX_GROUPS)
    return 1;
  
  /* check if group_id and esl_id is already assigned to another ESL */
  if(ESL_AP_GetESLInfoByESLAddress(esl_address, &esl_info, &db_h) == 0)
  {
    LOG_ERROR_APP("ESL Address (0x%04X) already assigned!\n", esl_address);
    return 1;
  }
  
  ESL_AP_Context.provisioning = true;
  ESL_AP_Context.configuring = true;
  ESL_AP_Context.conn_esl_info.bd_address_type = addr_type;
  memcpy(ESL_AP_Context.conn_esl_info.bd_address, address, sizeof(ESL_AP_Context.conn_esl_info.bd_address));
  ESL_AP_Context.conn_esl_info.esl_address = esl_address;
  
  set_bleAppContext_address(addr_type, address);
  
  /* Connect to ESL */
  UTIL_SEQ_SetTask(1u << CFG_TASK_CONN_DEV_ID, CFG_SEQ_PRIO_0);

  return 0;
}

uint8_t ESL_AP_Reconnect(uint8_t address_type, const uint8_t address[6])
{
  set_bleAppContext_address(address_type, address);
  ESL_AP_Context.provisioning = false;
  ESL_AP_Context.configuring = true;  
  
  UTIL_SEQ_SetTask(1u << CFG_TASK_CONN_DEV_ID, CFG_SEQ_PRIO_0);

  return 0;
}
