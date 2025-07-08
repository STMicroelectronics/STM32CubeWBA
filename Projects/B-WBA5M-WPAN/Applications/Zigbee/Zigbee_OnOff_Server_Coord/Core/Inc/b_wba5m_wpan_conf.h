/**
  ******************************************************************************
  * @file    b_wba5m_wpan_conf.h
  * @author  MCD Application Team
  * @brief   STM32WBAXX B-WBA5M-WPAN board configuration file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef B_WBA5M_WPAN_CONF_H
#define B_WBA5M_WPAN_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wbaxx_hal.h"

/* COM port usage */
#define USE_BSP_COM_FEATURE                  0U
#define USE_COM_LOG                          0U

/* IRQ priorities */
#define BSP_B2_IT_PRIORITY                   0x0FUL

#ifdef __cplusplus
}
#endif

#endif /* STM32WBAXX_NUCLEO_CONF_H */
