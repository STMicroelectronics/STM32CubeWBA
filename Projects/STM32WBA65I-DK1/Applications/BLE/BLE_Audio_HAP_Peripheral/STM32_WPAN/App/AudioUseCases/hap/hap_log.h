/**
  ******************************************************************************
  * @file    hap_log.h
  * @author  MCD Application Team
  * @brief   This file contains definitions and configuration used for
  *          Hearing Access Profile Logging
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HAP_LOG_H
#define __HAP_LOG_H
/* Includes ------------------------------------------------------------------*/
#include "ble_audio_plat.h"
#include "hap_config.h"

/* Defines -------------------------------------------------------------------*/
#ifndef BLE_DBG_HAP_ENABLE
#define BLE_DBG_HAP_ENABLE              (0u)
#endif /* BLE_DBG_HAP_ENABLE */


#if BLE_DBG_HAP_ENABLE == 1
#define BLE_DBG_HAP_MSG(fmt, ...)               BLE_AUDIO_PLAT_DbgLog("[HAP] " fmt, ##__VA_ARGS__ )
#define BLE_DBG_HAP_HA_MSG(fmt, ...)            BLE_AUDIO_PLAT_DbgLog("[HAP HA] " fmt, ##__VA_ARGS__ )
#define BLE_DBG_HAP_HARC_MSG(fmt, ...)          BLE_AUDIO_PLAT_DbgLog("[HAP HARC] " fmt, ##__VA_ARGS__ )
#define BLE_DBG_HAP_IAC_MSG(fmt, ...)           BLE_AUDIO_PLAT_DbgLog("[HAP IAC] " fmt, ##__VA_ARGS__ )
#define BLE_DBG_HAS_MSG(fmt, ...)               BLE_AUDIO_PLAT_DbgLog("[HAS] " fmt, ##__VA_ARGS__ )
#define BLE_DBG_HAP_DB_MSG(fmt, ...)            BLE_AUDIO_PLAT_DbgLog("[HAP DB] " fmt, ##__VA_ARGS__ )
#define BLE_DBG_IAS_MSG(fmt, ...)               BLE_AUDIO_PLAT_DbgLog("[IAS] " fmt, ##__VA_ARGS__ )
#else /*BLE_DBG_HAP_ENABLE != 1*/
#define BLE_DBG_HAP_MSG(fmt, ...)               (void)0
#define BLE_DBG_HAP_HA_MSG(fmt, ...)            (void)0
#define BLE_DBG_HAP_HARC_MSG(fmt, ...)          (void)0
#define BLE_DBG_HAP_IAC_MSG(fmt, ...)           (void)0
#define BLE_DBG_HAS_MSG(fmt, ...)               (void)0
#define BLE_DBG_HAP_DB_MSG(fmt, ...)            (void)0
#define BLE_DBG_IAS_MSG(fmt, ...)               (void)0
#endif /* BLE_DBG_HAP_ENABLE == 1*/

#endif /* __HAP_LOG_H */

