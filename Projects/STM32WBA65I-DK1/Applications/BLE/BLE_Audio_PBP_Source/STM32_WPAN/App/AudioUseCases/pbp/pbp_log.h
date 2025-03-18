/**
  ******************************************************************************
  * @file    pbp_log.h
  * @author  MCD Application Team
  * @brief   This file contains definitions and configuration used for
  *          Public Broadcast Profile Logging
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
#ifndef __PBP_LOG_H
#define __PBP_LOG_H
/* Includes ------------------------------------------------------------------*/
#include "ble_audio_plat.h"
#include "pbp_config.h"

/* Defines -------------------------------------------------------------------*/
#ifndef BLE_DBG_PBP_ENABLE
#define BLE_DBG_PBP_ENABLE              (0u)
#endif /* BLE_DBG_PBP_ENABLE */


#if BLE_DBG_PBP_ENABLE == 1
#define BLE_DBG_PBP_MSG(fmt, ...)               BLE_AUDIO_PLAT_DbgLog("PBP: " fmt, ##__VA_ARGS__ )
#else /*BLE_DBG_PBP_ENABLE != 1*/
#define BLE_DBG_PBP_MSG(fmt, ...)               (void)0
#endif /* BLE_DBG_PBP_ENABLE == 1*/

#endif /* __PBP_LOG_H */

