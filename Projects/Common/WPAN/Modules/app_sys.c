/**
  ******************************************************************************
  * @file    app_sys.c
  * @author  MCD Application Team
  * @brief   Application system for STM32WPAN Middleware.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024-2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "app_sys.h"
#include "app_conf.h"
#include "timer_if.h"
#include "stm32_lpm.h"
#include "ll_intf.h"
#include "ll_sys.h"
#if (UTIL_LPM_LEGACY_ENABLED == 0)  
#include "stm32_lpm_if.h"
#endif
#include "utilities_common.h"

/* External functions --------------------------------------------------------*/

/* External variables --------------------------------------------------------*/

/* Private variables  --------------------------------------------------------*/
#if (CFG_LPM_LEVEL != 0)  
static uint32_t wakeup_offset = RADIO_DEEPSLEEP_WAKEUP_TIME_US;

/* Functions Definition ------------------------------------------------------*/

void APP_SYS_EnterDeepSleep(void)
{
  uint32_t next_radio_evt;
 
  /* Check if radio is not active nor already in deep sleep */
  if (LL_PWR_GetRadioMode() == LL_PWR_RADIO_SLEEP_MODE) 
  {
    next_radio_evt = (uint32_t)os_timer_get_earliest_time();
 
    if (next_radio_evt == LL_DP_SLP_NO_WAKEUP)
    {
      /* No radio event scheduled */
      (void)ll_sys_dp_slp_enter(LL_DP_SLP_NO_WAKEUP);
    }
    else if (next_radio_evt > wakeup_offset)
    {
      /* No event in a "near" futur */
      (void)ll_sys_dp_slp_enter(next_radio_evt - wakeup_offset);
    }
    else
    {
      /* Not worth setting radio in deep sleep, allow stop1 at most */    
#if (UTIL_LPM_LEGACY_ENABLED == 1)      
      UTIL_LPM_SetOffMode(1U << CFG_LPM_LL_DEEPSLEEP, UTIL_LPM_DISABLE);
#else /*  (UTIL_LPM_LEGACY_ENABLED == 1) */
#if (CFG_LPM_STOP1_SUPPORTED == 1)
      UTIL_LPM_SetMaxMode(1U << CFG_LPM_LL_DEEPSLEEP, UTIL_LPM_STOP1_MODE);
#else /* (CFG_LPM_STOP1_SUPPORTED == 1) */
      UTIL_LPM_SetMaxMode(1U << CFG_LPM_LL_DEEPSLEEP, UTIL_LPM_SLEEP_MODE);
#endif /* (CFG_LPM_STOP1_SUPPORTED == 1) */
#endif /*  (UTIL_LPM_LEGACY_ENABLED == 1) */      
    }
  }
}

void APP_SYS_SetWakeupOffset(uint32_t wakeup_offset_us)
{
  wakeup_offset = wakeup_offset_us;
}

#endif /* (CFG_LPM_LEVEL != 0) */