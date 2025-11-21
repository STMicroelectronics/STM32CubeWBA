/**
  ******************************************************************************
  * @file    stm32_board.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for TrustZone application.
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
#ifndef __STM32_BOARD_H__
#define __STM32_BOARD_H__
#include "appli_region_defs.h"

#if defined(USE_STM32WBA65I_DK1)
/* Definitions specific to the STM32WBA65I-DK1 board */
#define LED3_Pin            GPIO_PIN_8
#define LED3_GPIO_Port      GPIOD
#define LED3_CLK_ENABLE()   __HAL_RCC_GPIOD_CLK_ENABLE()
/* Alias génériques */
#define LED_Pin             LED3_Pin
#define LED_GPIO_Port       LED3_GPIO_Port
#define LED_CLK_ENABLE()    LED3_CLK_ENABLE()
#elif defined(USE_STM32WBA55G_DK1)
/* Definitions specific to the STM32WBA55G-DK1 board */
#define LED3_Pin            GPIO_PIN_15
#define LED3_GPIO_Port      GPIOB
#define LED3_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()
/* Alias génériques */
#define LED_Pin             LED3_Pin
#define LED_GPIO_Port       LED3_GPIO_Port
#define LED_CLK_ENABLE()    LED3_CLK_ENABLE()
#elif defined(USE_NUCLEO_64)
#if defined(STM32WBA65xx)
/* Definitions specific to the NUCLEO-WBA65RI board */
#define LED1_Pin            GPIO_PIN_8
#define LED1_GPIO_Port      GPIOD
#define LED1_CLK_ENABLE()   __HAL_RCC_GPIOD_CLK_ENABLE()
/* Alias génériques */
#define LED_Pin             LED1_Pin
#define LED_GPIO_Port       LED1_GPIO_Port
#define LED_CLK_ENABLE()    LED1_CLK_ENABLE()
#elif defined(STM32WBA55xx)
/* Definitions specific to the NUCLEO-WBA55CG board */
#define LED1_Pin            GPIO_PIN_4
#define LED1_GPIO_Port      GPIOB
#define LED1_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()
/* Alias génériques */
#define LED_Pin             LED1_Pin
#define LED_GPIO_Port       LED1_GPIO_Port
#define LED_CLK_ENABLE()    LED1_CLK_ENABLE()
#else
#error "Unknown target."
#endif
#else
#error "LED's pin not configured yet also needed."
#endif


#endif /* __STM32_BOARD_H__ */
