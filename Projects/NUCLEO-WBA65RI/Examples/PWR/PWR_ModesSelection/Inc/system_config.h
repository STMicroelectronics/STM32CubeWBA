/**
  ******************************************************************************
  * @file    PWR/PWR_ModesSelection/Inc/system_config.h
  * @author  MCD Application Team
  * @brief   Header for system_config.c module
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
/* USER CODE BEGIN 1 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "stm32wbaxx_hal.h"

/* Exported types ------------------------------------------------------------*/

/** @defgroup  System Status Enumeration Definition.
  * @{
  */
typedef enum
{
  SYSTEM_OK    = 0x00, /*!< System status OK             */
  SYSTEM_ERROR = 0x01  /*!< System status ERROR          */

} System_StatusTypeDef;

/** @defgroup  System Low Power Mode Scenario Enumeration Definition.
  * @{
  */
typedef enum
{
  SYSTEM_LOWPOWER_S0,         /*!< RUN - 16 MHz                                                     */
  SYSTEM_LOWPOWER_S1,         /*!< RUN - 100 MHz                                                    */
  SYSTEM_LOWPOWER_S2,         /*!< SLEEP - 16 MHz                                                   */
  SYSTEM_LOWPOWER_S3,         /*!< STOP0 - Full SRAM (SRAM 1 + 2 = 512KB) + PKA + OTG               */
  SYSTEM_LOWPOWER_S4,         /*!< STOP0 - 64KB SRAM2                                               */
  SYSTEM_LOWPOWER_S5,         /*!< STOP1 - Full SRAM (SRAM 1 + 2 = 512KB) + PKA + OTG + ULPMEN=1    */
  SYSTEM_LOWPOWER_S6,         /*!< STOP1 - 64KB SRAM2 + ULPMEN=1                                    */
  SYSTEM_LOWPOWER_S7,         /*!< STOP2 - Full SRAM (SRAM 1 + 2 = 512KB) + PKA + OTG + ULPMEN=1    */
  SYSTEM_LOWPOWER_S8,         /*!< STOP2 - 64KB SRAM2 + ULPMEN=1                                    */
  SYSTEM_LOWPOWER_S9,         /*!< STANDBY(with RTC) - Clocked by LSI1 + ULPMEN=1                   */
  SYSTEM_LOWPOWER_S10,        /*!< STANDBY(with RTC) - Clocked by LSI1 + ULPMEN=0                   */
  SYSTEM_LOWPOWER_S11,        /*!< STANDBY(Retention Mode) - Full SRAM + Radio + ULPMEN=1           */
  SYSTEM_LOWPOWER_S12,        /*!< STANDBY(Retention Mode) - 64KB SRAM2 + ULPMEN=1                  */
  SYSTEM_LOWPOWER_S13,        /*!< STANDBY(Retention Mode) - Radio + ULPMEN=1                       */
  SYSTEM_LOWPOWER_NB
} System_LowPowerModeScenarioTypeDef;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void system_config(System_LowPowerModeScenarioTypeDef scenario);
System_StatusTypeDef system_lowpower_config(System_LowPowerModeScenarioTypeDef scenario);
System_StatusTypeDef system_rtc_config(System_LowPowerModeScenarioTypeDef scenario);
System_StatusTypeDef system_clock_config(System_LowPowerModeScenarioTypeDef scenario);
System_StatusTypeDef system_ram_retention_config(System_LowPowerModeScenarioTypeDef scenario);
System_StatusTypeDef system_regulator_config(System_LowPowerModeScenarioTypeDef scenario);

#endif /* SYSTEM_CONFIG_H */
/* USER CODE END 1 */