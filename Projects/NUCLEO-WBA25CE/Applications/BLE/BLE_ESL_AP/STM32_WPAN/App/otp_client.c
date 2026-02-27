/**
  ******************************************************************************
  * @file    otp_client.c
  * @author  GPM WBL Application Team
  * @brief   Implementation of Object Client role of Object Transfer Profile.
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

#include "app_common.h"
#include "log_module.h"
#include "ble_core.h"
#include "otp_client.h"
#include "gatt_client_app.h"
#include "stm32_seq.h"
#include "stm32_timer.h"
#include "app_conf.h"
#include "app_ble.h"

#define OTP_DEBUG                                                              1
    
/* SPSM for OTS */
#define SPSM_OTS                                                          0x0025

/* Object Properties Definition  */
#define OBJ_PROP_DELETE                                               0x00000001
#define OBJ_PROP_EXEC                                                 0x00000002
#define OBJ_PROP_READ                                                 0x00000004
#define OBJ_PROP_WRITE                                                0x00000008
#define OBJ_PROP_APPEND                                               0x00000010
#define OBJ_PROP_TRUNC                                                0x00000020
#define OBJ_PROP_PATCH                                                0x00000040
#define OBJ_PROP_MARK                                                 0x00000080

#define IS_EVENT_SET(evt)          (OTPClientContext.event & (1 << (evt)))
#define SET_EVENT(evt)             (OTPClientContext.event |= (1 << (evt)))
#define CLEAR_EVENT(evt)           (OTPClientContext.event &= ~(1 << (evt)))

static void proc_timeout(void *arg);

typedef enum
{
  OTP_CLIENT_STATE_DISCONNECTED = 0,
  OTP_CLIENT_STATE_IDLE,
  OTP_CLIENT_STATE_DISC_FEATURES,
  
}OTPClientState_t;

typedef enum
{
  OTP_EVENT_NONE = 0,
  OTP_EVENT_OLCP_RESPONSE,
  OTP_EVENT_OACP_RESPONSE,
  OTP_EVENT_DISCONNECTION,
  OTP_EVENT_TIMEOUT,
  OTP_EVENT_L2CAP_OPEN,
  OTP_EVENT_L2CAP_TX_CMPLT,
  OTP_EVENT_L2CAP_CREDITS,
  OTP_EVENT_L2CAP_ERROR,
}OTPEvent_t;

typedef struct
{
  OTPClientState_t state;
  uint16_t connection_handle;
  uint8_t l2cap_ch_idx;
  uint16_t peer_mtu;
  uint16_t peer_mps;
  uint16_t l2cap_credits;
  OTSHandleContext_t *OTSHandleContext_p;  
  uint32_t OACPFeautures;
  uint32_t OLCPFeautures;
  uint8_t CP_response_length;
  uint8_t CP_response[7];
  bool features_discovered;  /* TODO: use bitmask to save space. */
  bool truncate;
  uint16_t event;
  UTIL_TIMER_Object_t timer;
  char *search_name;
}OTPClientContext_t;

OTPClientContext_t OTPClientContext;

void OTP_CLIENT_Init(void)
{  
  UTIL_TIMER_Create(&OTPClientContext.timer,
                    OTP_PROCEDURE_TIMEOUT * 1000,
                    UTIL_TIMER_ONESHOT,
                    proc_timeout, 0);
}

void OTP_CLIENT_ConnectionComplete(OTSHandleContext_t *OTSHandleContext_p, uint16_t connection_handle)
{
  if(OTPClientContext.OTSHandleContext_p == NULL)
  {   
    OTPClientContext.OTSHandleContext_p = OTSHandleContext_p;
    
    OTPClientContext.connection_handle = connection_handle;  
    OTPClientContext.state = OTP_CLIENT_STATE_IDLE;
    OTPClientContext.l2cap_ch_idx = 0xFF;
    OTPClientContext.OACPFeautures = 0;
    OTPClientContext.OLCPFeautures = 0;
    OTPClientContext.features_discovered = false;
    UTIL_SEQ_ClrEvt(1 << CFG_EVENT_OTP);
  }
}

void OTP_CLIENT_DisconnectionComplete(void)
{
  UTIL_SEQ_SetEvt(1 << CFG_EVENT_OTP);
  SET_EVENT(OTP_EVENT_DISCONNECTION);
  
  OTPClientContext.state = OTP_CLIENT_STATE_DISCONNECTED;
  OTPClientContext.OTSHandleContext_p = NULL;
  OTPClientContext.l2cap_ch_idx = 0xFF;
  OTPClientContext.OACPFeautures = 0;
  OTPClientContext.OLCPFeautures = 0;
  OTPClientContext.features_discovered = false;
}

void OTP_CLIENT_L2CAPDisconnectionComplete(void)
{
  UTIL_SEQ_SetEvt(1 << CFG_EVENT_OTP);
  SET_EVENT(OTP_EVENT_DISCONNECTION);
  
  OTPClientContext.l2cap_ch_idx = 0xFF;
}

//TODO: do it automatically before first OTP operation? Need to have the handles passed in any case.
//TODO: add return parameter
uint8_t OTP_CLIENT_DiscoverFeatures(void)
{
  uint8_t *data_p;
  uint16_t data_length;
  uint8_t ret;
  
  if(OTPClientContext.state != OTP_CLIENT_STATE_IDLE)
  {
    return 1;
  }
  
  OTPClientContext.state = OTP_CLIENT_STATE_DISC_FEATURES;
  
  ret = GATT_CLIENT_APP_ReadCharacteristic(OTPClientContext.connection_handle, OTPClientContext.OTSHandleContext_p->OTSFeatureValueHdl, &data_length, &data_p, false);
  
  if(ret == 0 && data_length == 8)
  {
    OTPClientContext.OACPFeautures = LE_TO_HOST_32(data_p);
    OTPClientContext.OLCPFeautures = LE_TO_HOST_32(data_p + 4);
    
    LOG_DEBUG_APP("OACP Features 0x%08X\n", OTPClientContext.OACPFeautures);
    LOG_DEBUG_APP("OLCP Features 0x%08X\n", OTPClientContext.OLCPFeautures);
  }
  
  OTPClientContext.state = OTP_CLIENT_STATE_IDLE;
  
  OTPClientContext.features_discovered = true;
  
  return 0;
}

static void proc_timeout(void *arg)
{
  LOG_ERROR_APP("OLCP timeout\n");
  SET_EVENT(OTP_EVENT_TIMEOUT);
  UTIL_SEQ_SetEvt(1 << CFG_EVENT_OTP);
}

static int waitOLCPResponse(void)
{
  int ret = 0;
  
  UTIL_TIMER_Start(&OTPClientContext.timer);
  
  //LOG_DEBUG_APP("Waiting response\n");
  
  while(1)
  {
    UTIL_SEQ_WaitEvt(1 << CFG_EVENT_OTP);
    
    if(IS_EVENT_SET(OTP_EVENT_OLCP_RESPONSE))
    { 
      CLEAR_EVENT(OTP_EVENT_OLCP_RESPONSE);
#if OTP_DEBUG
      LOG_DEBUG_APP("Response: ");
      for(int i = 0; i < OTPClientContext.CP_response_length; i++)
      {
        LOG_DEBUG_APP("%02X ", OTPClientContext.CP_response[i]);
      }
      LOG_DEBUG_APP("\n");
#endif
      
      if(OTPClientContext.CP_response[2] == OLCP_RESULT_SUCCESS)
      {
        //LOG_DEBUG_APP("OLCP Success\n");
        ret = 0;
      }
      else if(OTPClientContext.CP_response[2] == OLCP_RESULT_NO_OBJ ||
              OTPClientContext.CP_response[2] == OLCP_RESULT_OUT_OF_BOUNDS)
      {
        LOG_DEBUG_APP("OLCP List end\n");
        ret = 1;
      }
      else
      {
        LOG_ERROR_APP("OLCP error\n");
        ret = -1;
      }
      break;
    }
    else if(IS_EVENT_SET(OTP_EVENT_DISCONNECTION) || IS_EVENT_SET(OTP_EVENT_TIMEOUT))
    {
      CLEAR_EVENT(OTP_EVENT_DISCONNECTION);
      CLEAR_EVENT(OTP_EVENT_TIMEOUT);
      LOG_ERROR_APP("OLCP procedure interrupted\n");
      ret = -2;
      break;
    } 
  }
  
  UTIL_TIMER_Stop(&OTPClientContext.timer);
  
  return ret;
}

static int waitOACPResponse(void)
{
  int ret = 0;
  
  UTIL_TIMER_Start(&OTPClientContext.timer);
  
  //LOG_DEBUG_APP("Waiting response\n");
  
  while(1)
  {
    UTIL_SEQ_WaitEvt(1 << CFG_EVENT_OTP);
    
    if(IS_EVENT_SET(OTP_EVENT_OACP_RESPONSE))
    {
      CLEAR_EVENT(OTP_EVENT_OACP_RESPONSE);
#if OTP_DEBUG
      LOG_DEBUG_APP("Response: ");
      for(int i = 0; i < OTPClientContext.CP_response_length; i++)
      {
        LOG_DEBUG_APP("%02X ", OTPClientContext.CP_response[i]);
      }
      LOG_DEBUG_APP("\n");
#endif        
      
      if(OTPClientContext.CP_response[2] == OACP_RESULT_SUCCESS)
      {
        //LOG_DEBUG_APP("OACP Success\n");
        ret = 0;
      }
      else
      {
        LOG_ERROR_APP("OACP error\n");
        ret = -1;
      }
      break;
    }
    else if(IS_EVENT_SET(OTP_EVENT_DISCONNECTION) || IS_EVENT_SET(OTP_EVENT_TIMEOUT))
    {
      CLEAR_EVENT(OTP_EVENT_DISCONNECTION);
      CLEAR_EVENT(OTP_EVENT_TIMEOUT);
      LOG_ERROR_APP("OACP procedure interrupted\n");
      ret = -2;
      break;
    }
  }
  
  UTIL_TIMER_Stop(&OTPClientContext.timer);
  
  return ret;
}

//TODO: only one event at a time can be handled. A bitmask would allow more events.
static int waitL2CAPEvent(OTPEvent_t event)
{  
  LOG_DEBUG_APP("Waiting L2CAP event %d\n", event);
  
  while(1)
  {
    UTIL_SEQ_WaitEvt(1 << CFG_EVENT_OTP);
    
    if(IS_EVENT_SET(event))
    {
      LOG_DEBUG_APP("L2CAP Event received\n");
      
      return 0;
    }
    else if(IS_EVENT_SET(OTP_EVENT_DISCONNECTION) ||
            IS_EVENT_SET(OTP_EVENT_L2CAP_ERROR))
    {
      CLEAR_EVENT(OTP_EVENT_DISCONNECTION);
      CLEAR_EVENT(OTP_EVENT_L2CAP_ERROR);
      LOG_ERROR_APP("L2CAP procedure failure\n");      
      return -2;
    }
  }
}

int OTP_CLIENT_DiscoverAllObjects(objectFoundCB_t object_found_cb)
{
  uint8_t data;
  uint8_t *name_p = NULL;
  uint16_t name_length = 0;
  tBleStatus ble_ret;
  int olcp_ret;
  
  if(OTPClientContext.state != OTP_CLIENT_STATE_IDLE)
  {
    return -1;
  }
  
  UTIL_SEQ_ClrEvt(1 << CFG_EVENT_OTP);
  
  if(OTPClientContext.features_discovered == false)
  {
    OTP_CLIENT_DiscoverFeatures();
  }
  
  for(uint8_t i = 0; i < 3; i++)
  {
    if(OTPClientContext.OTSHandleContext_p->ObjListFilterValueHdl[i] != 0)
    {
      uint8_t *data_p;
      uint16_t data_length;
      /* Disable Filter. */
      data = 0;
      ble_ret = GATT_CLIENT_APP_WriteCharacteristic(OTPClientContext.connection_handle, OTPClientContext.OTSHandleContext_p->ObjListFilterValueHdl[i],
                                                    1, &data);
      if(ble_ret != BLE_STATUS_SUCCESS)
      {
        return -1;
      }
      ble_ret = GATT_CLIENT_APP_ReadCharacteristic(OTPClientContext.connection_handle, OTPClientContext.OTSHandleContext_p->ObjListFilterValueHdl[i], &data_length, &data_p, true);
      if(ble_ret != BLE_STATUS_SUCCESS)
      {
        return -1;
      }
#if OTP_DEBUG
      LOG_DEBUG_APP("Object List Filter: ");
      for(int i = 0; i < data_length; i++)
      {
        LOG_DEBUG_APP("%02X ", data_p[i]);
      }
      LOG_DEBUG_APP("\n");
#endif
    }    
  }
      
  data = OLCP_OPCODE_FIRST;
  
  //LOG_DEBUG_APP("OTP first\n");
  
  do {
    
    //LOG_DEBUG_APP("Writing OLCP\n");
    
    ble_ret = GATT_CLIENT_APP_WriteCharacteristic(OTPClientContext.connection_handle, OTPClientContext.OTSHandleContext_p->ObjListCPValueHdl, 1, &data);
    if(ble_ret != BLE_STATUS_SUCCESS)
    {
      return -2;
    }
    
    olcp_ret = waitOLCPResponse();
    if(olcp_ret == 1)
    {
      /* End of list */
      break;      
    }
    else if(olcp_ret < 0) /* Unexpected error */
    {
      return -2;
    }
    
    //LOG_DEBUG_APP("Reading name\n");
    
    ble_ret = GATT_CLIENT_APP_ReadCharacteristic(OTPClientContext.connection_handle, OTPClientContext.OTSHandleContext_p->ObjNameValueHdl, &name_length, &name_p, true);
    if(ble_ret != BLE_STATUS_SUCCESS)
    {
      return -1;
    }
    
    if(object_found_cb((char *)name_p, name_length) == 1)
    {
      return 1;
    }
    
    data = OLCP_OPCODE_NEXT;
    
    //LOG_DEBUG_APP("OTP next\n");
  
  }while(1);
  
  return 0;  
}

static int objectFoundCB(const char * name, uint16_t name_length)
{
  if(strlen(OTPClientContext.search_name) == name_length &&
     memcmp(OTPClientContext.search_name, name, name_length) == 0)
  {
    /* Object found */
    LOG_DEBUG_APP("Object name found!\n");
    return 1;
  }
  
  return 0;
}

int OTP_CLIENT_SearchForObject(char *name)
{
  int ret;
  
  OTPClientContext.search_name = name;
  
  ret = OTP_CLIENT_DiscoverAllObjects(objectFoundCB);
  
  if(ret == 1)
  {
    /* Object found */
    return 0;
  }
  else if(ret == 0)
  {
    /* Object not found */
    return 1;
  }
  else
  {
    /* Error while searching */
    return ret;
  }  
}

int OTP_CLIENT_ReadMetadata(OTPObjectMeatadata_t *metadata)
{
  uint8_t *data_p = NULL;
  uint16_t data_length = 0;
  
  memset(metadata, 0, sizeof(OTPObjectMeatadata_t));
  
  if(GATT_CLIENT_APP_ReadCharacteristic(OTPClientContext.connection_handle, OTPClientContext.OTSHandleContext_p->ObjNameValueHdl, &data_length, &data_p, true) != BLE_STATUS_SUCCESS)
  {
    return -2;
  }  
  
  memcpy(metadata->name, data_p, MIN(data_length, sizeof(metadata->name) - 1));
  
  if(GATT_CLIENT_APP_ReadCharacteristic(OTPClientContext.connection_handle, OTPClientContext.OTSHandleContext_p->ObjTypeValueHdl, &data_length, &data_p, false) != BLE_STATUS_SUCCESS)
  
  if(data_length != 2 && data_length != 16)
  {
    return -1;
  }
  
  metadata->type_length = data_length;
  memcpy(metadata->type, data_p, data_length);
  
  if(GATT_CLIENT_APP_ReadCharacteristic(OTPClientContext.connection_handle, OTPClientContext.OTSHandleContext_p->ObjSizeValueHdl, &data_length, &data_p, false) != BLE_STATUS_SUCCESS)
  {
    return -2;
  }  
  
  if(data_length != 8)
  {
    return -1;
  }
  
  metadata->curr_size = LE_TO_HOST_32(data_p);
  metadata->alloc_size = LE_TO_HOST_32(data_p + 4);
  
  if(OTPClientContext.OTSHandleContext_p->ObjIdValueHdl != 0x0000)
  {
    if(GATT_CLIENT_APP_ReadCharacteristic(OTPClientContext.connection_handle, OTPClientContext.OTSHandleContext_p->ObjIdValueHdl, &data_length, &data_p, false) != BLE_STATUS_SUCCESS)
    {
      return -2;
    }
    
    if(data_length != OBJECT_ID_SIZE)
    {
      return -1;
    }
    
    memcpy(metadata->id, data_p, OBJECT_ID_SIZE);
  }
  
  if(GATT_CLIENT_APP_ReadCharacteristic(OTPClientContext.connection_handle, OTPClientContext.OTSHandleContext_p->ObjPropValueHdl, &data_length, &data_p, false) != BLE_STATUS_SUCCESS)
  {
    return -2;
  }
  
  if(data_length != sizeof(metadata->properties))
  {
    return -1;
  }
  
  metadata->properties = LE_TO_HOST_32(data_p);
  
  return 0;  
}

int OTP_CLIENT_WriteStart(bool truncate)
{
  int ret;
  
  if(OTPClientContext.l2cap_ch_idx == 0xFF)
  {
    ret = aci_l2cap_coc_connect(OTPClientContext.connection_handle, SPSM_OTS, 23, CFG_BLE_COC_MPS_MAX, 1, 1);
    if(ret != BLE_STATUS_SUCCESS)
    {
      return -4;
    }
    ret = waitL2CAPEvent(OTP_EVENT_L2CAP_OPEN);
    if(ret < 0)
    {
      return -4;
    }
  }
  
  OTPClientContext.truncate = truncate;
  
  return 0;
}

static int sendL2CAP_SDU(uint16_t sdu_length, uint8_t *p_sdu_data)
{
  int ret;
  uint16_t data_index = 0;
  uint16_t kframe_length;
  uint8_t kframe[MAX_PEER_MPS];
  uint16_t remaining_data;
    
  while(data_index < sdu_length)
  {
    remaining_data = sdu_length - data_index;
      
    kframe_length = MIN(remaining_data, OTPClientContext.peer_mps);
    
    if(OTPClientContext.l2cap_credits == 0)
    {
      //Wait for credits
      ret = waitL2CAPEvent(OTP_EVENT_L2CAP_CREDITS);
      if(ret < 0)
      {
        return -5;
      }
    }
    
    if(data_index == 0)
    {
      /* First frame needs SDU length */
      HOST_TO_LE_16(kframe, sdu_length);
      memcpy(kframe + 2, p_sdu_data, kframe_length);
      ret = aci_l2cap_coc_tx_data(OTPClientContext.l2cap_ch_idx, kframe_length + 2, kframe);
    }
    else
    {
      ret = aci_l2cap_coc_tx_data(OTPClientContext.l2cap_ch_idx, kframe_length, p_sdu_data + data_index);
    }
    
    LOG_DEBUG_APP("aci_l2cap_coc_tx_data 0x%02X (len %d, offset %d)\n", ret, kframe_length, data_index);
    
    if(ret == BLE_STATUS_SUCCESS)
    {
      data_index += kframe_length;
      OTPClientContext.l2cap_credits--;
    }
    else if(ret == BLE_STATUS_INSUFFICIENT_RESOURCES)
    {
      //Wait for buffers
      ret = waitL2CAPEvent(OTP_EVENT_L2CAP_TX_CMPLT);
      if(ret < 0)
      {
        return -5;
      }
    }
    else
    {
      return -1;
    }
  }
  
  LOG_DEBUG_APP("L2CAP SDU sent\n");
  
  return 0;
}

int OTP_CLIENT_WriteObj(const uint8_t *obj_data, uint16_t obj_data_length)
{
  int ret;
  uint8_t *att_data_p = NULL;
  uint16_t att_data_length = 0;
  uint8_t oacp_packet[10];
  uint16_t obj_data_index = 0;
  uint32_t obj_prop;
  uint32_t curr_size;
  uint32_t alloc_size;
  
  if(OTPClientContext.l2cap_ch_idx == 0xFF)
  {
    return -4;
  }
  
  /* Read and check object properties */  
  if(GATT_CLIENT_APP_ReadCharacteristic(OTPClientContext.connection_handle, OTPClientContext.OTSHandleContext_p->ObjPropValueHdl, &att_data_length, &att_data_p, false) != BLE_STATUS_SUCCESS)
  {
    return -2;
  }  
  if(att_data_length != sizeof(obj_prop))
  {
    return -1;
  }
  
  obj_prop = LE_TO_HOST_32(att_data_p);
  
  if((obj_prop & OBJ_PROP_WRITE) == 0)
  {
    /* Object is not writable. */
    return -3;
  }
  
  if(GATT_CLIENT_APP_ReadCharacteristic(OTPClientContext.connection_handle, OTPClientContext.OTSHandleContext_p->ObjSizeValueHdl, &att_data_length, &att_data_p, false) != BLE_STATUS_SUCCESS)
  {
    return -2;
  }  
  if(att_data_length != 8)
  {
    return -1;
  }
  
  curr_size = LE_TO_HOST_32(att_data_p);
  alloc_size = LE_TO_HOST_32(att_data_p + 4);
  
  LOG_DEBUG_APP("Curr size: %d Alloc size: %d\n", curr_size, alloc_size);
  
  if((obj_data_length > alloc_size) &
     ((obj_prop & OBJ_PROP_APPEND) == 0))
  {
    /* Not possible to append data. TODO: need to check also OTS feature? */
    return -3;
  }
  
  if((obj_data_length < curr_size) & !OTPClientContext.truncate &
     ((obj_prop & OBJ_PROP_PATCH) == 0))
  {
    /* Not possible to patch data. TODO: need to check also OTS feature? */
    return -3;
  }
  
  oacp_packet[0] = OACP_OPCODE_WRITE;
  HOST_TO_LE_32(&oacp_packet[1], 0); /* Offset */
  HOST_TO_LE_32(&oacp_packet[5], obj_data_length);  
  if((obj_data_length < curr_size) & OTPClientContext.truncate)
  {
    oacp_packet[9] = OACP_WRITE_MODE_TRUNCATE;
  }
  else
  {
    oacp_packet[9] = 0; /* Mode (truncate off) */
  }
  
  ret = GATT_CLIENT_APP_WriteCharacteristic(OTPClientContext.connection_handle, OTPClientContext.OTSHandleContext_p->ObjActionCPValueHdl, sizeof(oacp_packet), oacp_packet);
  if(ret != BLE_STATUS_SUCCESS)
  {
    return -2;
  }    
  ret = waitOACPResponse();
  if(ret < 0)
  {
    return -5;
  }
  
  /* Everything is ready to send data */
  while(obj_data_index < obj_data_length)
  {
    uint16_t sdu_length;
    uint16_t remaining_data;
    
    remaining_data = obj_data_length - obj_data_index;
      
    sdu_length = MIN(remaining_data, OTPClientContext.peer_mtu);
    
    LOG_DEBUG_APP("sendL2CAP_SDU ");
    
    ret = sendL2CAP_SDU(sdu_length, (uint8_t *)&obj_data[obj_data_index]);    
    if(ret == 0)
    {
      obj_data_index += sdu_length;
    }    
    else
    {
      return ret;
    }
  }
  
  if(OTPClientContext.OTSHandleContext_p->ObjLastModifiedValueHdl != 0x0000 &&
     OTPClientContext.OTSHandleContext_p->ObjLastModifiedProp & CHAR_PROP_WRITE)
  {
    uint8_t *data_p;
    uint16_t data_length;
    /* If the Object Last-Modified characteristic is exposed and supports the Write property, the Object Client shall update the value
       of the Object Last-Modified characteristic when the object is modified. If the Object Client does not have access to a valid
       date and time to set the object last-modified metadata, the fields of the Object Last-Modified characteristic for which the
       Object Client does not have valid data shall be written with a 0 to show that valid data is not available. */
    
    /* Read characteristic value to know the size. */
    if(GATT_CLIENT_APP_ReadCharacteristic(OTPClientContext.connection_handle, OTPClientContext.OTSHandleContext_p->ObjLastModifiedValueHdl, &data_length, &data_p, false) != BLE_STATUS_SUCCESS)
    {
      return -2;
    }
    
    /* Set value to 0. */    
    memset(data_p, 0, data_length);    
    
    if(GATT_CLIENT_APP_WriteCharacteristic(OTPClientContext.connection_handle, OTPClientContext.OTSHandleContext_p->ObjLastModifiedValueHdl, data_length, data_p) != BLE_STATUS_SUCCESS)
    {
      return -2;
    }
  }
  
  return 0;
}

int OTP_CLIENT_ChannelClose(void)
{
  int ret;
  
  ret = aci_l2cap_coc_disconnect(OTPClientContext.l2cap_ch_idx);
  if(ret != BLE_STATUS_SUCCESS)
  {
    return -2;
  }
  
  ret = waitL2CAPEvent(OTP_EVENT_DISCONNECTION);
  if(ret < 0)
  {
    return -1;
  }
  
  return 0;
}

void OTP_CLIENT_OLCPIndication(uint8_t *data_p, uint16_t data_length)
{
  LOG_DEBUG_APP("OTP_CLIENT_OLCPIndication\n");
  
  if(data_length > sizeof(OTPClientContext.CP_response) ||
     data_p[0] != OLCP_OPCODE_RESPONSE)
  {
    /* Incorrect response */
    return;
  }
  
  memcpy(OTPClientContext.CP_response, data_p, data_length);
  OTPClientContext.CP_response_length = data_length;
  
  SET_EVENT(OTP_EVENT_OLCP_RESPONSE);
  UTIL_SEQ_SetEvt(1 << CFG_EVENT_OTP);
}

void OTP_CLIENT_OACPIndication(uint8_t *data_p, uint16_t data_length)
{
  LOG_DEBUG_APP("OTP_CLIENT_OACPIndication\n");
  
  if(data_length > sizeof(OTPClientContext.CP_response) ||
     data_p[0] != OACP_OPCODE_RESPONSE)
  {
    /* Incorrect response */
    return;
  }
  
  memcpy(OTPClientContext.CP_response, data_p, data_length);
  OTPClientContext.CP_response_length = data_length;
  
  SET_EVENT(OTP_EVENT_OACP_RESPONSE);
  UTIL_SEQ_SetEvt(1 << CFG_EVENT_OTP);
}

//TODO: pass single parameters instead of event for better portability?
void OTP_CLIENT_L2CAPConnectionResp(aci_l2cap_coc_connect_confirm_event_rp0 *event)
{
  if(event->Connection_Handle != OTPClientContext.connection_handle)
    return;
  
  if(event->Result != L2CAP_CONN_SUCCESSFUL)
  {
    SET_EVENT(OTP_EVENT_L2CAP_ERROR);
    UTIL_SEQ_SetEvt(1 << CFG_EVENT_OTP);
    return;
  }
  
  OTPClientContext.l2cap_ch_idx = event->Channel_Index_List[0];
  OTPClientContext.peer_mtu = event->MTU;
  OTPClientContext.peer_mps = event->MPS;
  OTPClientContext.l2cap_credits = event->Initial_Credits;
  
  SET_EVENT(OTP_EVENT_L2CAP_OPEN);
  UTIL_SEQ_SetEvt(1 << CFG_EVENT_OTP);
}

void OTP_CLIENT_L2CAPCredits(uint16_t credits)
{
  OTPClientContext.l2cap_credits++;
  SET_EVENT(OTP_EVENT_L2CAP_CREDITS);
  UTIL_SEQ_SetEvt(1 << CFG_EVENT_OTP);
}

void OTP_CLIENT_L2CAPTxComplete(void)
{
  SET_EVENT(OTP_EVENT_L2CAP_TX_CMPLT);
  UTIL_SEQ_SetEvt(1 << CFG_EVENT_OTP);
}

