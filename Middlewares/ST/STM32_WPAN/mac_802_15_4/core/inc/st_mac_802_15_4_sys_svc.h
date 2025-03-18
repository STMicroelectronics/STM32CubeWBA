/**
 ******************************************************************************
 * @file    st_mac_802_15_4_sys_svc.h
 * @author  MCD Application Team
 * @brief   Contains STM32WBA specificities requested for antenna diversity.
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */

#ifndef _ST_MAC_802_15_4_SYS_H_
#define _ST_MAC_802_15_4_SYS_H_

#include "st_mac_802_15_4_types.h"


#include "common_types.h"

// SNPS MAC Interface
#include "mac_host_intf.h"

/* Exported functions ------------------------------------------------------- */

/* Antenna diversity */

MAC_Status_t ST_MAC_sys_svc_ant_div_en( MAC_handle * st_mac_hndl);
MAC_Status_t ST_MAC_sys_svc_ant_div_dis( MAC_handle * st_mac_hndl);
MAC_Status_t ST_MAC_sys_svc_ant_div_init( MAC_handle * st_mac_hndl, int8_t rssi_threshold, uint8_t default_ant_id,
                                          uint16_t coord_short_addr, uint8_t * coord_ext_addr);
MAC_Status_t ST_MAC_sys_svc_ant_div_set_intrv_radio_evts( MAC_handle * st_mac_hndl, uint32_t radio_evts);
MAC_Status_t ST_MAC_sys_svc_ant_div_set_intrv_time( MAC_handle * st_mac_hndl, uint32_t time_us);

#endif /* _ST_MAC_802_15_4_SYS_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/