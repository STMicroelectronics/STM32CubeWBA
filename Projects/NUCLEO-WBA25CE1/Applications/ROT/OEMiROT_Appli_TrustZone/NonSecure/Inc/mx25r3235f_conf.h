/**
  ******************************************************************************
  * @file    mx25r3235f_conf.h
  * @author  MCD Application Team
  * @brief   This file contains all the description of the MX25R3235F XSPI memory.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MX25R3235F_CONF_H
#define MX25R3235F_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wbaxx_hal.h" /* User application to set STM32 series HAL header */

#define DUMMY_CYCLES_READ            8U
#define DUMMY_CYCLES_READ_DUAL       4U
#define DUMMY_CYCLES_READ_QUAD       6U

#ifdef __cplusplus
}
#endif

#endif /* MX25R3235F_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
