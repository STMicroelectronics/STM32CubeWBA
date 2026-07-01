/**
  ******************************************************************************
  * @file    stm32_board.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for stm32wbaxx board.
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

#ifndef __STM32_BOARD_H__
#define __STM32_BOARD_H__

#include "region_defs.h"
#include "string.h"
#include "bootutil/crypto/sha256.h"

#define COM_INSTANCE                           USART1
#define COM_CLK_ENABLE()                       __HAL_RCC_USART1_CLK_ENABLE()
#define COM_CLK_DISABLE()                      __HAL_RCC_USART1_CLK_DISABLE()

#define COM_TX_GPIO_PORT                       GPIOA
#define COM_TX_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOA_CLK_ENABLE()
#define COM_TX_PIN                             GPIO_PIN_6
#define COM_TX_AF                              GPIO_AF10_USART1

#define COM_RX_GPIO_PORT                       GPIOA
#define COM_RX_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOA_CLK_ENABLE()
#define COM_RX_PIN                             GPIO_PIN_12
#define COM_RX_AF                              GPIO_AF7_USART1

#endif /*__STM32_BOARD_H__*/
