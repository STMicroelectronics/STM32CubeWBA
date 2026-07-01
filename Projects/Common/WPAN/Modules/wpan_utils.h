/**
  ******************************************************************************
  * @file    wpan_utils.h
  * @author  MCD Application Team
  * @brief   Header for Utilities for Common/WPAN
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef WPAN_UTILS_H
#define WPAN_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* External variables --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief   Get the Device ID.
 *
 * @return  Device ID.
 */
uint16_t WpanUtils_GetDeviceId( void );

/**
 * @brief   Get the Device Name.
 *
 * @return  Pointer on Device Name.
 */
char * WpanUtils_GetDeviceName( void );

/**
 * @brief   Get the Device Revision ID.
 *
 * @return  Device Revision ID.
 */
uint16_t WpanUtils_GetDeviceRevId( void );

/**
 * @brief   Get the Device Revision Name.
 *
 * @return  Pointer on Device Revision Name.
 */
char * WpanUtils_GetDeviceRevName( void );

/**
 * @brief   Get the Device Package ID.
 *
 * @return  Device Package ID.
 */
uint16_t WpanUtils_GetDevicePackageId( void );

/**
 * @brief   Get the Device Package Name.
 *
 * @return  Pointer on Device Package Name.
 */
char * WpanUtils_GetDevicePackageName( void );

#ifdef __cplusplus
}
#endif

#endif /* WPAN_UTILS_H */
