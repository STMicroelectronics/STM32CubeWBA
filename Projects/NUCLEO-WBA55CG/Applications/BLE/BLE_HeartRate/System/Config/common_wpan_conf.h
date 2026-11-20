/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    common_wpan_conf.h
  * @author  MCD Application Team
  * @brief   Configuration file for STM32 WPAN Common Utilities.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef COMMON_WPAN_CONF_H
#define COMMON_WPAN_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32_adv_trace.h"

/******************************************************************************
 * Operating System configuration
 ******************************************************************************/
 #define CFG_USE_SEQUENCER 1

/******************************************************************************
 * Timer Interface configuration
 ******************************************************************************/
#define TIMER_IF_RTCCLK_FREQ_HZ      (LSE_VALUE)

/* USER CODE BEGIN TIF_RTC */

/* USER CODE END TIF_RTC */

/* USER CODE BEGIN TIF_DBG1 */

/* USER CODE END TIF_DBG1 */

#ifdef TIF_DEBUG
#define TIMER_IF_DBG_PRINTF(...) do{ {UTIL_ADV_TRACE_COND_FSend(VLEVEL_ALWAYS, T_REG_OFF, TS_OFF, __VA_ARGS__);} }while(0);

/* USER CODE BEGIN TIF_DBG2 */

/* USER CODE END TIF_DBG2 */
#else
#define TIMER_IF_DBG_PRINTF(...)
#endif /* TIF_DEBUG */

/******************************************************************************
 * Application System configuration (app_sys)
 ******************************************************************************/
/**
 * Defines time to wake up from standby before radio event to meet timings
 * This value will be dynamically updated when using CFG_LPM_WAKEUP_TIME_PROFILING
 */
#define CFG_LPM_STDBY_WAKEUP_TIME (1500U)

/* USER CODE BEGIN AS_C */

/* USER CODE END AS_C */

#ifdef __cplusplus
}
#endif

#endif /* COMMON_WPAN_CONF_H */
