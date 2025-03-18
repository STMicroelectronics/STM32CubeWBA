/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : zdd_stack_wrapper.h
 * Description        : Header for ZDD wrapper.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ZDD_STACK_WRAPPER_H
#define ZDD_STACK_WRAPPER_H

#include "ble_types.h"


// Handler to manage ACI attributes
SVCCTL_EvtAckStatus_t handle_zdd_aci_gatt_atribute_modif(aci_gatt_attribute_modified_event_rp0 *attribute_modified,SVCCTL_EvtAckStatus_t ret);
void zdd_port_disconnect_zvd(struct ZigBeeT *zb);
void zdd_port_update_status(struct ZigBeeT *zb, uint8_t *bindata, uint8_t binlen);
bool zdd_port_send_tun_data(struct ZigBeeT *zb, uint8_t *bindata, uint8_t binlen);
void initialize_zdd_ble_asynch_event_queue(void);
tBleStatus write_zdd_bd_address(void);
void zdd_generate_bd_addr(void);
bool initialize_gap_gatt_interface(void);
void initialize_zdd_port_info(void);
bool zdd_port_open_process(struct ZigBeeT *zb, const char *adapter_name, const char *virtual_path,
    const char *virtual_sniffer);
void zdd_port_disconnect_zvd(struct ZigBeeT *zb);
void zdd_gatt_services_init(void);
void zdd_port_disable_advert(struct ZigBeeT *zb);
void zdd_port_disconnect_zvd(struct ZigBeeT *zb);
void zdd_port_update_advert(struct ZigBeeT *zb, uint16_t pan_id, uint16_t nwk_addr, bool pjoin);
void disconnect_zdd_port_zvd(void);
// Zdd task
void zdd_port_task(void);
// Wrapper function to set the active thread name
void set_active_thread_name(const char *name);
// Function to retrieve the BleAsynchEventQueue
tListNode* get_ble_asynch_event_queue(void);
// Wrapper function to clear the active thread name
void clear_active_thread_name(void);
// Wrapper function to update the handler
void  update_zdd_port_handle(uint16_t conn_handle);
// Wrapper function to check connection handle
bool check_zdd_connection_handle(uint16_t conn_handle);
// Wrapper function to handle read request
bool handle_zdd_read_request(uint16_t attribute_handle);
// Wrapper function to handle unhandled events
void handle_unhandled_event(uint16_t line_number);
// Wrapper function to handle new connection
bool handle_new_zdd_connection(void);
// Wrapper function to write ZDD
bool processWriteForZdd(aci_gatt_write_permit_req_event_rp0 *write_perm_req);
// Wrapper function to check active write parameter
bool check_zdd_write_active(void); 
// Wrapper function to update the active write
void update_zdd_write_active(void);

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
