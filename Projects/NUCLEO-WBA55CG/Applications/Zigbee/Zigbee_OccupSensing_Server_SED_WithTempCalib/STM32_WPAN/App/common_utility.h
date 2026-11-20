/**
  ******************************************************************************
  * @file    common_utility.h
  * @author  MCD Application Team
  * @brief   API to manage Common/WPAN utility functions.
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
#ifndef COMMON_UTILITY_H
#define COMMON_UTILITY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdarg.h>
#include <stdint.h>


/* Global Defines -------------------------------------------------------------*/

/* Global Structures -----------------------------------------------------------*/

/* Global Variables -----------------------------------------------------------*/

/* Global Functions -----------------------------------------------------------*/

uint16_t  AppZbUtil_GetDeviceId                 ( void );
uint16_t  AppZbUtil_GetDeviceRevId              ( void );
uint16_t  AppZbUtil_GetDevicePackageId          ( void );

uint8_t * AppZbUtil_GetDeviceName               ( void );
uint8_t * AppZbUtil_GetDeviceRevName            ( void );
uint8_t * AppZbUtil_GetDevicePackageName        ( void );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // COMMON_UTILITY_H
