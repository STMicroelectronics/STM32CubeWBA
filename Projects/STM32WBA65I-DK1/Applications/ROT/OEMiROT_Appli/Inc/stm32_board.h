/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#define LED1_Pin GPIO_PIN_8
#define LED1_GPIO_Port GPIOD
#define LED1_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define LED2_Pin  GPIO_PIN_9
#define LED2_GPIO_Port GPIOD
#define LED2_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#elif defined(USE_STM32WBA55G_DK1)
#define LED1_Pin GPIO_PIN_0
#define LED1_GPIO_Port GPIOA
#define LED1_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#elif defined(USE_NUCLEO_64)
#if defined(STM32WBA65xx)
#define LED1_Pin GPIO_PIN_8
#define LED1_GPIO_Port GPIOD
#define LED1_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define LED2_Pin  GPIO_PIN_4
#define LED2_GPIO_Port GPIOC
#define LED2_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#elif defined(STM32WBA52xx) || defined(STM32WBA55xx)
#define LED1_Pin GPIO_PIN_4
#define LED1_GPIO_Port GPIOB
#define LED1_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED2_Pin  GPIO_PIN_9
#define LED2_GPIO_Port GPIOA
#define LED2_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#else
#error "Unknown target."
#endif
#else
#error "LED's pin not configured yet also needed."
#endif


#endif /* __STM32_BOARD_H__ */
