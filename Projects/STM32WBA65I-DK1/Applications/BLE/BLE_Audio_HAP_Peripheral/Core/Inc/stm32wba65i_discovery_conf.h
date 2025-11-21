/**
  ******************************************************************************
  * @file    stm32wba65i_discovery_conf_template.h
  * @author  MCD Application Team
  * @brief   STM32WBA65I_DK1 board configuration file.
  *          This file should be copied to the application folder and renamed
  *          to stm32wba65i_discovery_conf.h .
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32WBA65I_DK1_CONF_H
#define STM32WBA65I_DK1_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wbaxx_hal.h"
#include "wm8904.h"

/* Usage of STM32WBA65I_DK1 board */
#define USE_STM32WBA65I_DK1                  1U

/* COM define */
#define USE_BSP_COM_FEATURE                  1U
#define USE_COM_LOG                          0U

/* IRQ priorities (Default is 15 as lowest priority level) */
#define BSP_AUDIO_OUT_IT_PRIORITY           15U
#define BSP_AUDIO_IN_IT_PRIORITY            15U

/* Joystick Debounce Delay in ms */
#define BSP_JOY_DEBOUNCE_DELAY              200

#define I2C_VALID_TIMING_NBR                16U

/* Headphone output is systematically initialized by WM8904_Init()
   Forcing WM8904_OUT_NONE allows to save time at BSP init by calling WM8904_Init() only one time for BSP_AUDIO_IN_Init()
   WM8904_VoidInit() is used for bypassing WM8904_Init() when called by BSP_AUDIO_OUT_Init() */
#undef WM8904_OUT_NONE
#define WM8904_OUT_NONE                     WM8904_OUT_HEADPHONE

int32_t WM8904_VoidInit(WM8904_Object_t *pObj, WM8904_Init_t *pInit);

#ifdef __cplusplus
}
#endif

#endif /* STM32WBA65I_DK1_CONF_H */

