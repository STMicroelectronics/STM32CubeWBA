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

/* Definitions specific to the NUCLEO-WBA25CE board */
#define LED1_Pin            GPIO_PIN_7
#define LED1_GPIO_Port      GPIOA
#define LED1_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()

#define LED2_Pin            GPIO_PIN_12
#define LED2_GPIO_Port      GPIOB
#define LED2_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()

#define LED3_Pin            GPIO_PIN_15
#define LED3_GPIO_Port      GPIOB
#define LED3_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()

#endif /* __STM32_BOARD_H__ */
