/**
  ******************************************************************************
  * @file    b_wba5m_wpan_env_sensors.h
  * @author  MCD Software Solutions Team
  * @brief   This file provides a set of functions needed to manage the environmental sensors
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
#ifndef B_WBA5M_WPAN_ENV_SENSORS_H
#define B_WBA5M_WPAN_ENV_SENSORS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "b_wba5m_wpan_conf.h"
#include "b_wba5m_wpan_errno.h"
#include "env_sensor.h"

#ifndef USE_ENV_SENSOR_SHT40AD1B_0
#define USE_ENV_SENSOR_SHT40AD1B_0                    1U
#endif /* USE_ENV_SENSOR_SHT40AD1B_0 */

#if (USE_ENV_SENSOR_SHT40AD1B_0 == 1U)
#include "sht40ad1b.h"
#endif /* USE_ENV_SENSOR_SHT40AD1B_0 == 1U */

/** @addtogroup BSP BSP
  * @{
  */

/** @addtogroup B_WBA5M_WPAN B_WBA5M_WPAN
  * @{
  */

/** @addtogroup B_WBA5M_WPAN_ENV_SENSORS B_WBA5M_WPAN ENV SENSORS
  * @{
  */

/** @defgroup B_WBA5M_WPAN_ENV_SENSORS_Exported_Types B_WBA5M_WPAN ENV SENSORS Exported Types
  * @{
  */

/* Environmental Sensor instance Info */
typedef struct
{
  uint8_t Temperature;
  uint8_t Pressure;
  uint8_t Humidity;
  uint8_t LowPower;
  float   HumMaxOdr;
  float   TempMaxOdr;
  float   PressMaxOdr;
} ENV_SENSOR_Capabilities_t;

typedef struct
{
  uint32_t Functions;
} ENV_SENSOR_Ctx_t;

/**
  * @}
  */

/** @defgroup B_WBA5M_WPAN_ENV_SENSOR_Exported_Constants B_WBA5M_WPAN ENV SENSOR Exported Constants
  * @{
  */

#if (USE_ENV_SENSOR_SHT40AD1B_0 == 1U)
#define ENV_SENSOR_SHT40AD1B_0 0U
#endif /* USE_ENV_SENSOR_SHT40AD1B_0 == 1U */

#ifndef ENV_TEMPERATURE
#define ENV_TEMPERATURE      1U
#endif /* ENV_TEMPERATURE */

#ifndef ENV_PRESSURE
#define ENV_PRESSURE         2U
#endif /* ENV_PRESSURE */

#ifndef ENV_HUMIDITY
#define ENV_HUMIDITY         4U
#endif /* ENV_HUMIDITY */

#define ENV_SENSOR_FUNCTIONS_NBR    3U
#define ENV_SENSOR_INSTANCES_NBR    (USE_ENV_SENSOR_SHT40AD1B_0)

#if (ENV_SENSOR_INSTANCES_NBR == 0U)
#error "No environmental sensor instance has been selected"
#endif /* (ENV_SENSOR_INSTANCES_NBR == 0U) */

/**
  * @}
  */

/** @addtogroup B_WBA5M_WPAN_ENV_SENSORS_Exported_Functions B_WBA5M_WPAN ENV SENSOR Exported Functions
  * @{
  */

int32_t BSP_ENV_SENSOR_Init(uint32_t Instance, uint32_t Functions);
int32_t BSP_ENV_SENSOR_DeInit(uint32_t Instance);
int32_t BSP_ENV_SENSOR_GetCapabilities(uint32_t Instance, ENV_SENSOR_Capabilities_t *Capabilities);
int32_t BSP_ENV_SENSOR_ReadID(uint32_t Instance, uint8_t *Id);
int32_t BSP_ENV_SENSOR_Enable(uint32_t Instance, uint32_t Function);
int32_t BSP_ENV_SENSOR_Disable(uint32_t Instance, uint32_t Function);
int32_t BSP_ENV_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float *Odr);
int32_t BSP_ENV_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float Odr);
int32_t BSP_ENV_SENSOR_GetValue(uint32_t Instance, uint32_t Function, float *Value);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* B_WBA5M_WPAN_ENV_SENSORS_H */
