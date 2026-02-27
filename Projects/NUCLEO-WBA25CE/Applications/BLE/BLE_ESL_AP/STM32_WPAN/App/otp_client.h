/**
  ******************************************************************************
  * @file         otp_client.h
  * @brief        Header file of otp_client.c library.
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

#ifndef OTP_CLIENT_H
#define OTP_CLIENT_H
    
#include <stdbool.h>
#include "ble_events.h"

/* Macros that can be changed by user */
#define OTP_PROCEDURE_TIMEOUT                                         30
#define MAX_NAME_LENGTH                                               60
#define MAX_PEER_MPS                                                  255
/* End of customizable macros */
    
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

/* Op Codes for OACP procedures */
#define OACP_OPCODE_CREATE                                          0x01
#define OACP_OPCODE_DELETE                                          0x02
#define OACP_OPCODE_CALC_CHECKSUM                                   0x03
#define OACP_OPCODE_EXECUTE                                         0x04
#define OACP_OPCODE_READ                                            0x05
#define OACP_OPCODE_WRITE                                           0x06
#define OACP_OPCODE_ABORT                                           0x07
#define OACP_OPCODE_RESPONSE                                        0x60

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

/* Bitmask for Mode Parameter of OACP write */    
#define OACP_WRITE_MODE_TRUNCATE                                    0x02

#define OBJECT_ID_SIZE                                              6

typedef struct
{
  /* Handles for OTS */
  uint16_t OTSFeatureValueHdl;  
  uint16_t ObjNameValueHdl;
  uint16_t ObjTypeValueHdl;
  uint16_t ObjSizeValueHdl;
  uint16_t ObjLastModifiedValueHdl;
  uint16_t ObjIdValueHdl;
  uint16_t ObjPropValueHdl;
  uint16_t ObjActionCPValueHdl;
  uint16_t ObjActionCPCCCDHdl;
  uint16_t ObjListCPValueHdl;
  uint16_t ObjListCPCCCDHdl;
  uint16_t ObjListFilterValueHdl[3];
  uint8_t ObjLastModifiedProp;
}OTSHandleContext_t;

typedef struct
{
  char      name[MAX_NAME_LENGTH + 1];
  uint8_t   type_length;
  uint8_t   type[16];
  uint32_t  curr_size;
  uint32_t  alloc_size;
  uint32_t  properties;
  uint8_t   id[OBJECT_ID_SIZE];
}OTPObjectMeatadata_t;

typedef int (*objectFoundCB_t)(const char * name, uint16_t name_length); 

void OTP_CLIENT_Init(void);
void OTP_CLIENT_ConnectionComplete(OTSHandleContext_t *OTSHandleContext_p, uint16_t connection_handle);
void OTP_CLIENT_DisconnectionComplete(void);
void OTP_CLIENT_OLCPIndication(uint8_t *data_p, uint16_t data_length);
void OTP_CLIENT_OACPIndication(uint8_t *data_p, uint16_t data_length);
void OTP_CLIENT_L2CAPConnectionResp(aci_l2cap_coc_connect_confirm_event_rp0 *event);
int OTP_CLIENT_DiscoverAllObjects(objectFoundCB_t object_found_cb);
uint8_t OTP_CLIENT_DiscoverFeatures(void);
int OTP_CLIENT_SearchForObject(char *name);
int OTP_CLIENT_ReadMetadata(OTPObjectMeatadata_t *metadata);
int OTP_CLIENT_WriteStart(bool truncate);
int OTP_CLIENT_WriteObj(const uint8_t *obj_data, uint16_t obj_data_length);
int OTP_CLIENT_ChannelClose(void);
void OTP_CLIENT_L2CAPCredits(uint16_t credits);
void OTP_CLIENT_L2CAPTxComplete(void);
void OTP_CLIENT_L2CAPDisconnectionComplete(void);

#endif /* OTP_CLIENT_H */
