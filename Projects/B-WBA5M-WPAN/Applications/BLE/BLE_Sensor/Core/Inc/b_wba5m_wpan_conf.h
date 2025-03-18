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

/* I2C3 Frequency in Hz  */
#define BUS_I2C3_FREQUENCY                   100000UL /* Frequency of I2C3 = 100 KHz */

/* BUS functions */
#define B_WBA5M_WPAN_I2C_Init                BSP_I2C3_Init
#define B_WBA5M_WPAN_I2C_DeInit              BSP_I2C3_DeInit
#define B_WBA5M_WPAN_I2C_ReadReg             BSP_I2C3_ReadReg
#define B_WBA5M_WPAN_I2C_WriteReg            BSP_I2C3_WriteReg
#define B_WBA5M_WPAN_I2C_ReadReg16           BSP_I2C3_ReadReg16
#define B_WBA5M_WPAN_I2C_WriteReg16          BSP_I2C3_WriteReg16
#define B_WBA5M_WPAN_I2C_Read                BSP_I2C3_Recv
#define B_WBA5M_WPAN_I2C_Write               BSP_I2C3_Send
#define B_WBA5M_WPAN_I2C_IsReady             BSP_I2C3_IsReady
#define B_WBA5M_WPAN_GetTick                 BSP_GetTick  
 
/* Environmental Sensors usage */
#define USE_ENV_SENSOR_SHT40AD1B_0           1U

/* Motion Sensors usage */
#define USE_MOTION_SENSOR_ISM330DHCX_0       1U

/* Default EEPROM max trials */
#define EEPROM_MAX_TRIALS                   3000U

  
#ifdef __cplusplus
}
#endif

#endif /* STM32WBAXX_NUCLEO_CONF_H */
