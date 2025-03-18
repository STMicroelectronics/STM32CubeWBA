/**
  ******************************************************************************
  * @file    st_mac_802_15_4_core.h
  * @author  MCD Application Team
  * @brief   This file contains all the defines and structures used for the
  *          communication between the linklayer and the wrapper.
  *
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



#ifndef _ST_MAC_802_15_4_CORE_H_
#define _ST_MAC_802_15_4_CORE_H_

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>

#include "st_mac_802_15_4_sys.h"
#include "st_mac_802_15_4_types.h"

/* SNPS */
#include "mac_host_intf.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Exported defines ----------------------------------------------------------*/
#define MAC_CMD_MSG_ID_OFFSET         0x00
#define MAC_CMD_MSG_HANDLE_ID_OFFSET  0x01
#define MAC_CMD_MSG_STRUCT_LEN_OFFSET 0x02
#define MAC_CMD_MSG_STUFF_OFFSET      0x03
#define MAC_CMD_MSG_HEADER_OFFSET     0x04


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* External functions --------------------------------------------------------*/
/** @brief  Enqueue function used in the MAC wrapper */
MAC_Status_t ST_MAC_enqueue_radio_Incoming(uint8_t command_id, MAC_handle st_mac_hndl, uint8_t req_len, void * reqPtr);
MAC_Status_t ST_MAC_enqueue_radio_Incoming_with_payload(uint8_t command_id, MAC_handle st_mac_hndl, uint8_t req_len, void * reqPtr, uint8_t * payload, uint8_t payload_len);
uint8_t ST_MAC_enqueue_req(uint8_t command_id, MAC_handle st_mac_hndl, uint8_t req_len, void * reqPtr);

/** @brief  Function allows to serialize pan description for MLME-BeaconNotify.indication */
void mac_serialize_pan_desc_to_ST(  ST_MAC_PAN_Desc_t * pan_desc,  pan_descr_st * pan_desc_snps);

/** @brief  Function allows to identify if a beacon is already received with theses information like channel, PANID, addr mode, short/ext address */
uint8_t mac_unique_list_PAN_beacon(ST_MAC_PAN_Desc_t MAC_PAN_Desc);

/** @brief  Interface Functions */
uint8_t is_MAC_ready(uint8_t mac_command_id);
uint8_t mac_command_validate(uint8_t command_id, uint8_t command_len);
uint8_t mac_get_pib_attribute_len(void* mac_cntx_ptr,uint8_t pib_attr_id);
MAC_handle ST_MAC_get_mac_interface(void * mac_cntx_ptr);
MAC_Status_t MAC_config(void);
MAC_Status_t MAC_reset(void);

/** @brief  Main function */
void mac_baremetal_run(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _ST_MAC_802_15_4_CORE_H_ */
