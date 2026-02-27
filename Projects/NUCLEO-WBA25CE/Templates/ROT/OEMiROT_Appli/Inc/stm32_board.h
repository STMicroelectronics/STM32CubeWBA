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
/* Alias definitions for LED */
#define LED_Pin             LED1_Pin
#define LED_GPIO_Port       LED1_GPIO_Port
#define LED_CLK_ENABLE()    LED1_CLK_ENABLE()


#endif /* __STM32_BOARD_H__ */
