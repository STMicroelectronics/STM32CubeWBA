/**
  ******************************************************************************
  * @file    b_wba5m_wpan_env_sensors_ex.h
  * @author  MCD Software Solutions Team
  * @brief   This file provides a set of extended functions needed to manage the environmental sensors
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
#ifndef B_WBA5M_WPAN_ENV_SENSOR_EX_H
#define B_WBA5M_WPAN_ENV_SENSOR_EX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "b_wba5m_wpan_env_sensors.h"

/** @addtogroup BSP BSP
  * @{
  */

/** @addtogroup B_WBA5M_WPAN B_WBA5M_WPAN
  * @{
  */

/** @addtogroup B_WBA5M_WPAN_ENV_SENSOR_EX B_WBA5M_WPAN_ENV_SENSOR_EX
  * @{
  */

/** @addtogroup B_WBA5M_WPAN_ENV_SENSOR_EX_Exported_Functions B_WBA5M_WPAN_ENV_SENSOR_EX Exported Functions
  * @{
  */

int32_t BSP_ENV_SENSOR_Read_Register(uint32_t Instance, uint8_t Reg, uint8_t *Data);
int32_t BSP_ENV_SENSOR_Write_Register(uint32_t Instance, uint8_t Reg, uint8_t Data);
int32_t BSP_ENV_SENSOR_Get_DRDY_Status(uint32_t Instance, uint32_t Function, uint8_t *Status);
int32_t BSP_ENV_SENSOR_FIFO_Get_Data(uint32_t Instance, float *Press, float *Temp);
int32_t BSP_ENV_SENSOR_FIFO_Get_Fth_Status(uint32_t Instance, uint8_t *Status);
int32_t BSP_ENV_SENSOR_FIFO_Get_Full_Status(uint32_t Instance, uint8_t *Status);
int32_t BSP_ENV_SENSOR_FIFO_Get_Num_Samples(uint32_t Instance, uint8_t *NumSamples);
int32_t BSP_ENV_SENSOR_FIFO_Get_Ovr_Status(uint32_t Instance, uint8_t *Status);
int32_t BSP_ENV_SENSOR_FIFO_Reset_Interrupt(uint32_t Instance, uint8_t Interrupt);
int32_t BSP_ENV_SENSOR_FIFO_Set_Interrupt(uint32_t Instance, uint8_t Interrupt);
int32_t BSP_ENV_SENSOR_FIFO_Set_Mode(uint32_t Instance, uint8_t Mode);
int32_t BSP_ENV_SENSOR_FIFO_Set_Watermark_Level(uint32_t Instance, uint8_t Watermark);
int32_t BSP_ENV_SENSOR_FIFO_Stop_On_Watermark(uint32_t Instance, uint8_t Stop);
int32_t BSP_ENV_SENSOR_Set_High_Temperature_Threshold(uint32_t Instance, float Value);
int32_t BSP_ENV_SENSOR_Set_Low_Temperature_Threshold(uint32_t Instance, float Value);
int32_t BSP_ENV_SENSOR_Get_Temperature_Limit_Status(uint32_t Instance, uint8_t *HighLimit, uint8_t *LowLimit,
                                                       uint8_t *ThermLimit);
int32_t BSP_ENV_SENSOR_Set_Event_Pin(uint32_t Instance, uint8_t Enable);
int32_t BSP_ENV_SENSOR_Set_One_Shot(uint32_t Instance);
int32_t BSP_ENV_SENSOR_Get_One_Shot_Status(uint32_t Instance, uint8_t *Status);

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
#endif

#endif /* B_WBA5M_WPAN_ENV_SENSOR_EX_H */
