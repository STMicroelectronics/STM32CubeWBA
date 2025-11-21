/**
  ******************************************************************************
  * @file    tmap_log.h
  * @author  MCD Application Team
  * @brief   This file contains definitions and configuration used for
  *          Telephony and Media Audio Profile Logging
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
#ifndef __TMAP_LOG_H
#define __TMAP_LOG_H
/* Includes ------------------------------------------------------------------*/
#include "ble_audio_plat.h"
#include "tmap_config.h"
#include "app_conf.h"

/* Defines -------------------------------------------------------------------*/
#ifndef BLE_DBG_TMAP_ENABLE
#define BLE_DBG_TMAP_ENABLE              (0u)
#endif /* BLE_DBG_TMAP_ENABLE */


#if (BLE_DBG_TMAP_ENABLE == 1) && (CFG_LOG_SUPPORTED == 1)
#define BLE_DBG_TMAP_MSG(fmt, ...)               BLE_AUDIO_PLAT_DbgLog("[TMAP] " fmt, ##__VA_ARGS__ )
#define BLE_DBG_TMAS_MSG(fmt, ...)               BLE_AUDIO_PLAT_DbgLog("[TMAS] " fmt, ##__VA_ARGS__ )
#else /* (BLE_DBG_TMAP_ENABLE != 1) || (CFG_LOG_SUPPORTED != 1) */
#define BLE_DBG_TMAP_MSG(fmt, ...)               (void)0
#define BLE_DBG_TMAS_MSG(fmt, ...)               (void)0
#endif /* (BLE_DBG_TMAP_ENABLE == 1) && (CFG_LOG_SUPPORTED == 1) */

#endif /* __TMAP_LOG_H */

