/**
  ******************************************************************************
  * @file    gmap_log.h
  * @author  MCD Application Team
  * @brief   This file contains definitions and configuration used for
  *          Gaming Audio Profile Logging
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
#ifndef __GMAP_LOG_H
#define __GMAP_LOG_H
/* Includes ------------------------------------------------------------------*/
#include "ble_audio_plat.h"
#include "gmap_config.h"
#include "app_conf.h"

/* Defines -------------------------------------------------------------------*/
#ifndef BLE_DBG_GMAP_ENABLE
#define BLE_DBG_GMAP_ENABLE              (0u)
#endif /* BLE_DBG_GMAP_ENABLE */


#if (BLE_DBG_GMAP_ENABLE == 1) && (CFG_LOG_SUPPORTED == 1)
#define BLE_DBG_GMAP_MSG(fmt, ...)               BLE_AUDIO_PLAT_DbgLog("[GMAP] " fmt, ##__VA_ARGS__ )
#define BLE_DBG_GMAS_MSG(fmt, ...)               BLE_AUDIO_PLAT_DbgLog("[GMAS] " fmt, ##__VA_ARGS__ )
#else /* (BLE_DBG_GMAP_ENABLE != 1) || (CFG_LOG_SUPPORTED != 1) */
#define BLE_DBG_GMAP_MSG(fmt, ...)               (void)0
#define BLE_DBG_GMAS_MSG(fmt, ...)               (void)0
#endif /* (BLE_DBG_GMAP_ENABLE == 1) && (CFG_LOG_SUPPORTED == 1) */

#endif /* __GMAP_LOG_H */

