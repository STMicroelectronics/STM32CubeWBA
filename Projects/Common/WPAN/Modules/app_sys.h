/**
  ******************************************************************************
  * @file    app_sys.h
  * @author  MCD Application Team
  * @brief   Header for app_sys.c
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022-2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef APP_SYS_H
#define APP_SYS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common_wpan_conf.h"

/* Exported constants --------------------------------------------------------*/

/* The RADIO_DEEPSLEEP_WAKEUP_TIME_US macro allows to define when the system
 * needs to wake up before "handle next event".
 * This macro is the sum of the durations of standby exit and Link Layer
 * deep sleep mode exit.
 */
#if !defined(CFG_LPM_STDBY_WAKEUP_TIME)
#define RADIO_DEEPSLEEP_WAKEUP_TIME_US (1500U)
#else
#define RADIO_DEEPSLEEP_WAKEUP_TIME_US (CFG_LPM_STDBY_WAKEUP_TIME)
#endif

/* Legacy APIs backward compatibility */
#define APP_SYS_BLE_EnterDeepSleep      APP_SYS_EnterDeepSleep
#define APP_SYS_LPM_EnterLowPowerMode   APP_SYS_EnterDeepSleep

/* Exported functions prototypes ---------------------------------------------*/
#if (CFG_LPM_LEVEL != 0)  
void APP_SYS_EnterDeepSleep(void);
void APP_SYS_SetWakeupOffset(uint32_t wakeup_offset_us);
#endif /* (CFG_LPM_LEVEL != 0) */

#ifdef __cplusplus
}
#endif

#endif /* APP_SYS_H */
