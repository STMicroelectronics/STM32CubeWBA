/**
  ******************************************************************************
  * @file    uart_cmd.c
  * @author  MCD Application Team
  * @brief   Implementation of uart commands for ESL.
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
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "log_module.h"
#include "app_conf.h"
#include "stm32_adv_trace.h"
#include "uart_cmd.h"
#include "esl_profile_ap.h"
#include "app_debug.h"
#include "app_common.h"
#include "gatt_client_app.h"
#include "time_ref.h"
#include "otp_client.h"
#include "stm32_seq.h"
#include "app_ble.h"

#define C_SIZE_CMD_STRING       256U

extern ESL_AP_context_t ESL_AP_Context;

static uint8_t CommandString[C_SIZE_CMD_STRING];
static volatile uint16_t indexReceiveChar = 0;
static uint8_t echo_mode = 0;
static volatile uint8_t buff_lock = 0;

static void UartRxCpltCallback(uint8_t *PData, uint16_t Size, uint8_t Error);
static int objectFoundCB(const char * name, uint16_t name_length);

void UART_CMD_Init(void)
{
  /* Init Communication reception. Need that Log/Traces are activated */
  UTIL_ADV_TRACE_StartRxProcess(UartRxCpltCallback);
}

static void UartRxCpltCallback(uint8_t *PData, uint16_t Size, uint8_t Error)
{
  /* nDataSize always 1 in current implementation. */
  
  if(buff_lock || *PData == '\n')
  {
    /* Previous command still need to be processed. This should not happen.
       Also discard newline character. */
    return;
  }
  
  if(*PData == '\b')
  {
    if(indexReceiveChar > 0)
    {
      indexReceiveChar--;
      if(echo_mode)
      {
        //TODO: replace with putchar
        LOG_INFO_APP("%c", *PData); //putchar(*PData);
      }
    }
    return;
  }
  
  /* Filling buffer and wait for '\r' char */
  if (indexReceiveChar < C_SIZE_CMD_STRING - 1)
  {
    if(echo_mode)
    {
      //TODO: replace with putchar
      LOG_INFO_APP("%c", *PData); //putchar(*PData);
    }
    if (*PData == '\r')
    {
      CommandString[indexReceiveChar] = '\0';
      buff_lock = 1;
        
      //LOG_DEBUG_APP("received %s\n", CommandString);
      
      UART_CMD_ProcessRequestCB();
    }
    else
    {
      CommandString[indexReceiveChar++] = *PData;
    }
  }
}

static void print_esl_resp(uint8_t group_id, uint8_t esl_id, uint8_t *resp)
{
  uint8_t resp_length;
  
  resp_length =  GET_LENGTH_FROM_OPCODE(resp[0]);
  
  LOG_DEBUG_APP("Response from ESL (%d, %d): ", group_id, esl_id);
  
  for(int i = 0; i < resp_length; i++)
  {
    LOG_DEBUG_APP("0x%02X ", resp[i]);
  }
  
  LOG_DEBUG_APP("\n");
}

static void ping_resp_cb(uint8_t group_id, uint8_t esl_id, uint8_t *resp)
{
  uint8_t status = 0;
  uint16_t basic_state = 0;  
  
  print_esl_resp(group_id, esl_id, resp);
  
  if(resp[0] == ESL_RESP_BASIC_STATE)
  {
    basic_state = LE_TO_HOST_16(&resp[1]);    
  }
  else
  {
    status = 1;
  }
  
  LOG_INFO_APP("+STATE:%02X,%02X,%d,%d\r\n", group_id, esl_id, status, basic_state);
}

static void led_resp_cb(uint8_t group_id, uint8_t esl_id, uint8_t *resp)
{
  uint8_t status;
  
  print_esl_resp(group_id, esl_id, resp);
  
  if(resp[0] == ESL_RESP_LED_STATE)
  {
    status = 0;
  }
  else
  {
    status = 1;
  }
  
  LOG_INFO_APP("+LED:%02X,%02X,%d\r\n", group_id, esl_id, status);
}

static void txt_resp_cb(uint8_t group_id, uint8_t esl_id, uint8_t *resp)
{
  uint8_t status;
  
  print_esl_resp(group_id, esl_id, resp);
  
  if(resp[0] == ESL_RESP_VS_OK)
  {
    status = 0;
  }
  else
  {
    status = 1;
  }
  
  LOG_INFO_APP("+TXT:%02X,%02X,%d\r\n", group_id, esl_id, status);
}

static void price_resp_cb(uint8_t group_id, uint8_t esl_id, uint8_t *resp)
{
  uint8_t status;
  
  print_esl_resp(group_id, esl_id, resp);
  
  if(resp[0] == ESL_RESP_VS_OK)
  {
    status = 0;
  }
  else
  {
    status = 1;
  }
  
  LOG_INFO_APP("+PRICE:%02X,%02X,%d\r\n", group_id, esl_id, status);
}

static void batt_resp_cb(uint8_t group_id, uint8_t esl_id, uint8_t *resp)
{
  uint8_t status;  
  uint8_t resp_param_length;
  uint16_t batt_voltage = 0;
  uint8_t sensor_index;
  
  resp_param_length =  GET_PARAM_LENGTH_FROM_OPCODE(resp[0]);
  sensor_index = resp[1];
  
  print_esl_resp(group_id, esl_id, resp);
  
  if(resp_param_length != 3 || sensor_index != 0)
  {
    status = 1;
    goto fail;
  }
  
  if((resp[0] & 0x0F) == ESL_RESP_SENSOR_VALUE_TAG_NIBBLE)
  {
    status = 0;
    batt_voltage = LE_TO_HOST_16(&resp[2]);    
  }
  else
  {
    status = 1;
  }
  
fail:
  LOG_INFO_APP("+SENS:%02X,%02X,%d,%d\r\n", group_id, esl_id, status, batt_voltage);
}

static void img_resp_cb(uint8_t group_id, uint8_t esl_id, uint8_t *resp)
{
  uint8_t status;
  uint8_t display_index = 0;
  uint8_t img_index = 0;
  
  print_esl_resp(group_id, esl_id, resp);
  
  if(resp[0] == ESL_RESP_DISPLAY_STATE)
  {
    status = 0;
    display_index = resp[1];
    img_index = resp[2];
  }
  else
  {
    status = 1;
  }
  
  LOG_INFO_APP("+DISP:%02X,%02X,%d,%d,%d\r\n", group_id, esl_id, status, display_index, img_index);
}

static void unassociated_resp_cb(uint8_t group_id, uint8_t esl_id, uint8_t *resp)
{
  uint8_t status = 0;
  uint16_t basic_state = 0;
  
  print_esl_resp(group_id, esl_id, resp);
  
  if(resp[0] == ESL_RESP_BASIC_STATE)
  {
    basic_state = LE_TO_HOST_16(&resp[1]); 
    /* The ESL removed all bonding information with the AP, so the AP have to 
       remove this ESL from esl_bonded_list*/
    ESL_AP_DeleteESLInfo(GET_ESL_ADDRESS(group_id, esl_id));
  }
  else
  {
    status = 1;
  }
  
  LOG_INFO_APP("+STATE:%02X,%02X,%d,%d\r\n", group_id, esl_id, status, basic_state);
}

static void server_reset_resp_cb(uint8_t group_id, uint8_t esl_id, uint8_t *resp)
{
  uint8_t status = 0;
  uint16_t basic_state = 0;  
  
  print_esl_resp(group_id, esl_id, resp);
  
  if(resp[0] == ESL_RESP_BASIC_STATE)
  {
    basic_state = LE_TO_HOST_16(&resp[1]);    
  }
  else
  {
    status = 1;
  }
  
  LOG_INFO_APP("+STATE:%02X,%02X,%d,%d\r\n", group_id, esl_id, status, basic_state);
}

static int objectFoundCB(const char * name, uint16_t name_length)
{
  LOG_INFO_APP("+OTP: %.*s\n", name_length, name);
  
  return 0;
}

static int parse_cmd(void)
{
  unsigned int group_id, esl_id;
  int ret;
  
  if(strncasecmp((char *)CommandString, "ATE", 3) == 0)
  {
    unsigned int echo = 1;
 
    sscanf((char*)CommandString + 3, "%x", &echo);
    if(echo > 1)
    {
      return 1;
    }
    else
    {
      echo_mode = echo;
      
      return 0;
    }
  }
  else if(strncasecmp((char *)CommandString, "AT+PING=", 8) == 0)
  {    
    ret = sscanf((char*)CommandString + 8, "%x,%x", &group_id, &esl_id);
    if(ret != 2 || group_id >= MAX_GROUPS)
    {
      return 1;
    }
    else
    {  
      if(ESL_AP_Command(ESL_CMD_PING, group_id, esl_id, ping_resp_cb) == 0)
      {
        return 0;
      }
      else
      {
        return 1;
      }
    }
  }
  else if(strncasecmp((char *)CommandString, "AT+UNASSOC=", 11) == 0)
  {    
    ret = sscanf((char*)CommandString + 11, "%x,%x", &group_id, &esl_id);
    if(ret != 2 || group_id >= MAX_GROUPS)
    {
      return 1;
    }
    else
    {
      if(ESL_AP_Command(ESL_CMD_UNASSOCIATE_FROM_AP, group_id, esl_id, unassociated_resp_cb) == 0)
      {
        return 0;
      }
      else
      {
        return 1;
      }
    }
  }  
  else if(strncasecmp((char *)CommandString, "AT+SRVRST=", 10) == 0)
  {
    ret = sscanf((char*)CommandString + 10, "%x,%x", &group_id, &esl_id);
    if(ret != 2 || group_id >= MAX_GROUPS)
    {
      return 1;
    }
    else
    {
      if(ESL_AP_Command(ESL_CMD_SERVICE_RESET, group_id, esl_id, server_reset_resp_cb) == 0)
      {
        return 0;
      }
      else
      {
        return 1;
      }
    }
  }    
  else if(strncasecmp((char *)CommandString, "AT+FRST", 8) == 0)
  {
    if(ESL_AP_CmdFactoryReset() == 0)
    {
      return 0;
    }
    else
    {
      return 1;
    }
  }   
  else if(strncasecmp((char *)CommandString, "AT+UPDCMP", 10) == 0)
  {    
    if(ESL_AP_CmdUpdateComplete() == 0)
    {
      return 0;
    }
    else
    {
      return 1;
    }
  }  
  else if (strncasecmp((char *)CommandString, "AT+SENS=", 8) == 0)
  {
    
    ret = sscanf((char*)CommandString + 8, "%x,%x", &group_id, &esl_id);
    if(ret != 2 || group_id >= MAX_GROUPS || esl_id == 0xFF)
    {
      return 1;
    }
    else
    {
      if(ESL_AP_CmdReadSensorData(group_id, esl_id, batt_resp_cb, 0) == 0)
      {
        return 0;
      }
      else
      {      
        return 1;
      }
    }
  }   
  else if(strncasecmp((char *)CommandString, "AT+REFRESH=", 11) == 0)
  {
    unsigned int display_index;
    
    ret = sscanf((char*)CommandString + 11, "%x,%x,%u",  &group_id, &esl_id, &display_index);
    if(ret != 3 || group_id >= MAX_GROUPS)
    {
      return 1;
    }
    else
    {
      if(ESL_AP_CmdRefreshDisplay(group_id, esl_id, display_index, img_resp_cb) == 0)
      {
        return 0;
      }
      else
      {
        return 1;
      }
    }
  }     
  else if(strncasecmp((char *)CommandString, "AT+IMG=", 7) == 0)
  {
    unsigned int image_index, display_index;
    
    ret = sscanf((char*)CommandString + 7, "%x,%x,%u,%u", &group_id, &esl_id, &display_index, &image_index);
    if(ret != 4 || group_id >= MAX_GROUPS)
    {
      return 1;
    }
    else
    {
      if(ESL_AP_CmdDisplayImage(group_id, esl_id, img_resp_cb, display_index, image_index) == 0)
      {
        return 0;
      }
      else
      {
        return 1;
      }
    }
  }
  else if(strncasecmp((char *)CommandString, "AT+IMGTIM=", 10) == 0)
  {
    unsigned int display_index, image_index;
    unsigned int abs_time;
    
    ret = sscanf((char*)CommandString + 10, "%x,%x,%u,%u,%u", &group_id, &esl_id, &display_index, &image_index, &abs_time);
    if(ret != 5 || group_id >= MAX_GROUPS)
    {
      return 1;
    }
    else
    {
      if(ESL_AP_CmdDisplayTimedImage(group_id, esl_id, img_resp_cb, display_index, image_index, abs_time) == 0)
      {
        return 0;
      }
      else
      {
        return 1;
      }
    }
  }   
  else if (strncasecmp((char *)CommandString, "AT+LED=", 7) == 0)
  {
    unsigned int led_index, led_component, off_period, on_period;
    uint64_t pattern;
    unsigned int repeat_type, repeat_duration;
    
    ret = sscanf((char*)CommandString + 7, "%x,%x,%u,%x,%llx,%u,%u,%u,%u", &group_id, &esl_id, &led_index, &led_component, &pattern, &off_period, &on_period, &repeat_type, &repeat_duration);
    if(ret != 9 || group_id >= MAX_GROUPS || repeat_type > 1 || repeat_duration > 0x7FFF)    
    {
      return 1;
    }
    else
    {
      if(ESL_AP_CmdLedControl(group_id, esl_id, led_resp_cb, led_index, led_component, pattern, off_period, on_period, (repeat_duration<<1)|repeat_type) == 0)
      {
        return 0;
      }
      else
      {
        return 1;
      }
    }
  }  
  else if(strncasecmp((char *)CommandString, "AT+LEDTIM=", 10) == 0)
  {    
    unsigned int led_index, led_component, off_period, on_period;
    uint64_t pattern;
    unsigned int repeat_type, repeat_duration;
    unsigned int abs_time;
    
    ret = sscanf((char*)CommandString + 10, "%x,%x,%u,%x,%llx,%u,%u,%u,%u,%u", &group_id, &esl_id, &led_index, &led_component, &pattern, &off_period, &on_period, &repeat_type, &repeat_duration, &abs_time);
    if(ret != 10 || group_id >= MAX_GROUPS || repeat_type > 1 || repeat_duration > 0x7FFF)    
    {
      return 1;
    }
    else
    {
      if(ESL_AP_CmdLedTimedControl(group_id, esl_id, led_resp_cb, led_index, led_component, pattern, off_period, on_period, (repeat_duration<<1)|repeat_type, abs_time) == 0)
      {
        return 0;
      }
      else
      {
        return 1;
      }
    }    
  }   
  else if (strncasecmp((char *)CommandString, "AT+TXT=", 7) == 0)
  {
    char text[61];
    
    ret = sscanf((char*)CommandString + 7, "%x,%x,%[^\t\r\n]", &group_id, &esl_id, text);
    if(ret != 3 || group_id >= MAX_GROUPS)
    {
      return 1;
    }
    else
    {
      if(ESL_AP_CmdTxt(group_id, esl_id, txt_resp_cb, text) == 0)
      {
        return 0;
      }
      else
      {
        return 1;
      }
    }
  }
  else if (strncasecmp((char *)CommandString, "AT+PRICE=", 9) == 0)
  {
    unsigned int val_int, val_fract;
    
    ret = sscanf((char*)CommandString + 9, "%x,%x,%u.%u", &group_id, &esl_id, &val_int, &val_fract);
    if(ret != 4 || group_id >= MAX_GROUPS || val_int > 999 || val_fract > 99)
    {
      return 1;
    }
    else
    {
      if(ESL_AP_CmdPrice(group_id, esl_id, price_resp_cb, val_int, val_fract) == 0)
      {
        return 0;
      }
      else
      {
        return 1;
      }
    }
  }   
  // Command to reconfigure an ESL with a new address
  else if(strncasecmp((char *)CommandString, "AT+RECONF=", 10) == 0)
  {    
    ret = sscanf((char*)CommandString + 10, "%x,%x", &group_id, &esl_id);
    if(ret != 2 || group_id >= MAX_GROUPS)
    {
      return 1;
    }
    else
    {
    /* To transition an ESL from the Synchronized state to the Updating state, 
       the AP shall use the Periodic Advertising Connection procedure */  
      if(ESL_AP_SetNewEslAddress(group_id, esl_id) == 0)
      {
        return 0;
      }
      else
      {
        return 1;
      }
    }    
  }
  // Command for transition from Synchronized to Updating State for an ESL
  else if(strncasecmp((char *)CommandString, "AT+CONN=", 8) == 0)
  {    
    ret = sscanf((char*)CommandString + 8, "%x,%x", &group_id, &esl_id);
    if(ret != 2 || group_id >= MAX_GROUPS)
    {
      return 1;
    }
    else
    {
    /* To transition an ESL from the Synchronized state to the Updating state, 
       the AP shall use the Periodic Advertising Connection procedure */  
      if(ESL_AP_StartUpdate(group_id, esl_id) == 0)
      {
        return 0;
      }
      else
      {
        return 1;
      }
    }    
  }   
  // Command to read the Display/Image/Sensor/Led Information Characteristics
  else if(strncasecmp((char *)CommandString, "AT+INFO", 7) == 0)
  {
    if(GATT_CLIENT_APP_ReadAllInfo(ESL_AP_Context.conn_handle) == 0)
    { 
      return 0;
    }
    else
    {
      return 1;
    }   
  }    
  // Command to read the Display/Image/Sensor/Led Information Characteristics
  else if(strncasecmp((char *)CommandString, "AT+DISPLAYINFO", 14) == 0)
  {
    if(GATT_CLIENT_APP_ReadDisplayInfo(ESL_AP_Context.conn_handle) == 0)
    { 
      return 0;
    }
    else
    {
      return 1;
    }   
  }
  // Command to read the Display/Image/Sensor/Led Information Characteristics
  else if(strncasecmp((char *)CommandString, "AT+SENSORINFO", 13) == 0)
  {
    if(GATT_CLIENT_APP_ReadSensorInfo(ESL_AP_Context.conn_handle) == 0)
    { 
      return 0;
    }
    else
    {
      return 1;
    }   
  }
  // Command to read the Display/Image/Sensor/Led Information Characteristics
  else if(strncasecmp((char *)CommandString, "AT+LEDINFO", 10) == 0)
  {
    if(GATT_CLIENT_APP_ReadLedInfo(ESL_AP_Context.conn_handle) == 0)
    { 
      return 0;
    }
    else
    {
      return 1;
    }   
  }  
  // Command to send aci_gap_clear_security_db api
  else if(strncasecmp((char *)CommandString, "AT+CLRNVM", 10) == 0)
  {
    if(ESL_AP_ClearNVMDB() == 0)
    { 
      return 0;
    }
    else
    {
      return 1;
    }   
  } 
  // Command to read current absolute time 
  else if(strncasecmp((char *)CommandString, "AT+ABSTIME?", 10) == 0)
  {
    uint32_t abs_time;
    
    abs_time = TIMEREF_GetCurrentAbsTime();
    LOG_INFO_APP("+ABSTIME: %d\n", abs_time);
    
    return 0;
  } 
// Command for connection to ESL with 
  else if(strncasecmp((char *)CommandString, "AT+SCAN", 7) == 0)
  {    
    Scan_proc();
    
    return 0;
  }  
  // Command for connection to ESL with 
  else if(strncasecmp((char *)CommandString, "AT+ADD=", 7) == 0)
  {    
    unsigned int addr_type;
    uint64_t address;
    
    ret = sscanf((char*)CommandString + 7, "%u,%llx,%x,%x",&addr_type, &address, &group_id, &esl_id);
    if(ret != 4 || group_id >= MAX_GROUPS)
    {
      return 1;
    }
    else
    {
      uint8_t peer_address[6];
      // Extract each address byte and store in the peer_address in little-endian order
      peer_address[0] = address & 0xFF;
      peer_address[1] = (address >> 8) & 0xFF;
      peer_address[2] = (address >> 16) & 0xFF;
      peer_address[3] = (address >> 24) & 0xFF;
      peer_address[4] = (address >> 32) & 0xFF;
      peer_address[5] = (address >> 40) & 0xFF;
      
      /* To provision or connect to ESL found with scan procedure*/  
      if(ESL_AP_CmdProvisioning(addr_type, peer_address, group_id, esl_id) == 0)
      {
        return 0;
      }
      else
      {
        return 1;
      }
    }    
  }
#ifdef PTS_OTP
  else if(strncasecmp((char *)CommandString, "AT+OTPCONN=", 11) == 0)
  {
    /* Command only used to test OTP */    
    uint32_t addr_type;
    uint64_t address;
    extern ESL_AP_context_t ESL_AP_Context;
    
    ret = sscanf((char*)CommandString + 11, "%d,%llx",&addr_type, &address);
    if(ret != 2)
    {
      return 1;
    }
    else
    {
      uint8_t peer_address[6];
      // Extract each address byte and store in the peer_address in little-endian order
      peer_address[0] = address & 0xFF;
      peer_address[1] = (address >> 8) & 0xFF;
      peer_address[2] = (address >> 16) & 0xFF;
      peer_address[3] = (address >> 24) & 0xFF;
      peer_address[4] = (address >> 32) & 0xFF;
      peer_address[5] = (address >> 40) & 0xFF;
      
      set_bleAppContext_address(addr_type, peer_address);
      ESL_AP_Context.provisioning = false;
      ESL_AP_Context.configuring = false;
      
      UTIL_SEQ_SetTask(1u << CFG_TASK_CONN_DEV_ID, CFG_SEQ_PRIO_0);
      
      return 0;
    }
  }
  else if(strncasecmp((char *)CommandString, "AT+OTPDISCONN", 13) == 0)
  { 
    APP_BLE_Procedure_Gap_General(PROC_GAP_GEN_CONN_TERMINATE);
    
    return 0;
  }
  else if(strncasecmp((char *)CommandString, "AT+OTPFEAT", 10) == 0)
  { 
    if(OTP_CLIENT_DiscoverFeatures() == 0)
    {
      return 0;
    }
    else
    {
      return 1;
    }
  }
#endif
  else if(strncasecmp((char *)CommandString, "AT+OTPSEARCH=", 13) == 0)
  {
    char text[61];
    
    ret = sscanf((char*)CommandString + 13, "%[^\t\r\n]", text);
    if(ret != 1)
    {
      return 1;
    }
    else
    {
      if(OTP_CLIENT_SearchForObject(text) == 0)
      {
        return 0;
      }
      else
      {
        return 1;
      }      
    }
  }
  else if(strncasecmp((char *)CommandString, "AT+OTPSEARCH", 12) == 0)
  { 
    if(OTP_CLIENT_DiscoverAllObjects(objectFoundCB) == 0)
    {
      return 0;
    }
    else
    {
      return 1;
    }
  }
  else if(strncasecmp((char *)CommandString, "AT+OTPMETA", 10) == 0)
  {
    OTPObjectMeatadata_t metadata;
    
    if(OTP_CLIENT_ReadMetadata(&metadata) == 0)
    {      
      LOG_INFO_APP("Name: %s\n", metadata.name);
      LOG_INFO_APP("Type: 0x ");
      for(int i = 0; i < metadata.type_length; i++)
      {
        LOG_INFO_APP("%02X ", metadata.type[i]);
      }
      LOG_INFO_APP("\n");
      LOG_INFO_APP("Curr Size: %d\n", metadata.curr_size);
      LOG_INFO_APP("Alloc Size: %d\n", metadata.alloc_size);
      LOG_INFO_APP("Prop: 0x%08X\n", metadata.properties);
      LOG_INFO_APP("ID: 0x");
      for(int i = OBJECT_ID_SIZE - 1; i >= 0; i--)
      {
        LOG_INFO_APP("%02X ", metadata.id[i]);
      }
      LOG_INFO_APP("\n");
      
      return 0;
    }
    else
    {
      return 1;
    }
  }
  else if(strncasecmp((char *)CommandString, "AT+OTPSTART=", 12) == 0)
  {
    unsigned int truncate;
    
    ret = sscanf((char*)CommandString + 12, "%u",&truncate);
    if(ret != 1 || truncate > 1)
    {
      return 1;
    }
    else
    {      
      if(OTP_CLIENT_WriteStart(truncate) == 0)
      { 
        return 0;
      }
      else
      {
        return 1;
      }
    }
  }
  else if(strncasecmp((char *)CommandString, "AT+OTPWRITE=", 12) == 0)
  {
    //For debug
    extern const unsigned char gImage[5000];
    int size;    
    
    ret = sscanf((char*)CommandString + 12, "%u",&size);
    
    if(ret != 1 || size > sizeof(gImage))
    {
      return 1;
    }
    
    if(OTP_CLIENT_WriteObj(gImage, size) == 0)
    { 
      return 0;
    }
    else
    {
      return 1;
    }
  }
  else if(strncasecmp((char *)CommandString, "AT+OTPCLOSE", 11) == 0)
  {      
    if(OTP_CLIENT_ChannelClose() == 0)
    { 
      return 0;
    }
    else
    {
      return 1;
    }
  }
  // Help command: list of all ESL AP commands
  else if(strncasecmp((char *)CommandString, "AT+HELP", 7) == 0)
  {
    LOG_INFO_APP("Standard ESL commands (Group and ID to be given as hexadecimal value, other values are decimal unless otherwise specified):\n");
    LOG_INFO_APP("  - AT+PING=<group_id>,<esl_id>: Ping\n");
    LOG_INFO_APP("  - AT+UNASSOC=<group_id>,<esl_id>: Unassociate from AP\n");
    LOG_INFO_APP("  - AT+SRVRST=<group_id>,<esl_id>: Service Reset\n");
    LOG_INFO_APP("  - AT+FRST=<group_id>,<esl_id>: Factory Reset\n");
    LOG_INFO_APP("  - AT+UPDCMP=<group_id>,<esl_id>: Update Complete\n");
    LOG_INFO_APP("  - AT+SENS=<group_id>,<esl_id>,<sensor_index>: Read Sensor Data\n");
    LOG_INFO_APP("  - AT+REFRESH=<group_id>,<esl_id>,<display_index>: Refresh Display\n");
    LOG_INFO_APP("  - AT+IMG=<group_id>,<esl_id>,<display_index>,<image_index>: Display Image\n");
    LOG_INFO_APP("  - AT+IMGTIM=<group_id>,<esl_id>,<display_index>,<image_index>,<absolute_time>: Display Timed Image\n");
    LOG_INFO_APP("  - AT+LED=<group_id>,<esl_id>,<led_index>,<led_component_hex>,<pattern_hex>,<off_period>,<on_period>,<repeat_type>,<repeat_duration>: LED Control\n");
    LOG_INFO_APP("  - AT+LEDTIM=<group_id>,<esl_id>,<led_index>,<led_component>,<pattern>,<off_period>,<on_period>,<repeat_type>,<repeat_duration>,<absolute_time>: LED Timed Control\n");
    
    LOG_INFO_APP("\nProprietary ESL commands: \n");
    LOG_INFO_APP("  - AT+TXT=<group_id>,<esl_id>,<text>: Set text\n  - <text> can be a string with maximum 15 characters\n");
    LOG_INFO_APP("  - AT+PRICE=<group_id>,<esl_id>,<val_int>,<val_fract>: Set price \n");
    
    LOG_INFO_APP("\nCommands for special operations or tests\n");
    LOG_INFO_APP("  - AT+SCAN: Scan for ESLs\n");
    LOG_INFO_APP("  - AT+ADD=<addr_type>,<address>,<group_id>,<esl_id>: Add an ESL to the network\n");  
    LOG_INFO_APP("  - AT+CONN=<group_id>,<esl_id>: Connect to an ESL for update\n");
    LOG_INFO_APP("  - AT+RECONF=<new_group_id>,<new_esl_id>: Reconfigure the connected ESL with a new address\n");
    LOG_INFO_APP("  - AT+INFO: Read all the Information Characteristics from the connected ESL\n");    
    LOG_INFO_APP("  - AT+DISPLAYINFO: Read the Display Information Characteristic from the connected ESL\n");
    LOG_INFO_APP("  - AT+SENSORINFO: Read the Sensor Information Characteristic from the connected ESL\n");
    LOG_INFO_APP("  - AT+LEDINFO: Read the LED Information Characteristic from the connected ESL\n");
    LOG_INFO_APP("  - AT+CLRNVM: Clear the NVM database\n");
    LOG_INFO_APP("  - AT+ABSTIME?: Read current absolute time\n");
    LOG_INFO_APP("  - AT+OTPSEARCH: Discover images on the connected server on the ESL\n");
    LOG_INFO_APP("  - AT+OTPSEARCH=<name>: Search and select the specified image on the connected ESL\n");
    LOG_INFO_APP("  - AT+OTPMETA: Read metadata for current object\n");
    LOG_INFO_APP("  - AT+OTPSTART=<truncate>: Open an L2CAP channel to transfer an image to the connected ESL. Set <truncate> to 1 to truncate image, otherwise set it to 0.\n");
    LOG_INFO_APP("  - AT+OTPWRITE=<size>: Send image data, up to the given size in bytes (maximum is 5000). Data to be sent is stored inside image.c file.\n");
    LOG_INFO_APP("  - AT+OTPCLOSE: Close L2CAP channel to transfer image data. It should be issued when there are not other images to be sent.\n");    
    LOG_INFO_APP("  - AT+HELP: List of AT commands\n");
    return 0;
  }    
  return 1;  
}

void UART_CMD_Process(void)
{
  if(parse_cmd() == 0)
  {
    LOG_INFO_APP("OK\r\n");   
  }
  else
  {
    LOG_INFO_APP("ERROR\r\n");
  }
  
  indexReceiveChar = 0;
  
  buff_lock = 0;
}
