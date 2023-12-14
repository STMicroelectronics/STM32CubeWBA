/**
  ******************************************************************************
  * @file    Templates_LL/Legacy/Inc/main.h
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
#ifndef MAIN_H
#define MAIN_H

/* Includes ------------------------------------------------------------------*/
/* LL drivers common to all LL examples */
#include "stm32wbaxx_ll_bus.h"
#include "stm32wbaxx_ll_rcc.h"
#include "stm32wbaxx_ll_system.h"
#include "stm32wbaxx_ll_utils.h"
#include "stm32wbaxx_ll_pwr.h"
#include "stm32wbaxx_ll_exti.h"
#include "stm32wbaxx_ll_gpio.h"
#include "stm32wbaxx_ll_icache.h"
/* LL drivers specific to LL examples IPs */
#include "stm32wbaxx_ll_adc.h"
#include "stm32wbaxx_ll_cortex.h"
#include "stm32wbaxx_ll_crc.h"
#include "stm32wbaxx_ll_dma.h"
#include "stm32wbaxx_ll_hsem.h"
#include "stm32wbaxx_ll_i2c.h"
#include "stm32wbaxx_ll_iwdg.h"
#include "stm32wbaxx_ll_lptim.h"
#include "stm32wbaxx_ll_lpuart.h"
#include "stm32wbaxx_ll_pka.h"
#include "stm32wbaxx_ll_rng.h"
#include "stm32wbaxx_ll_rtc.h"
#include "stm32wbaxx_ll_spi.h"
#include "stm32wbaxx_ll_tim.h"
#include "stm32wbaxx_ll_usart.h"
#include "stm32wbaxx_ll_wwdg.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* ==============   BOARD SPECIFIC CONFIGURATION CODE BEGIN    ============== */
/**
  * @brief LED
  */
#define LED1_PIN                           LL_GPIO_PIN_4
#define LED1_GPIO_PORT                     GPIOB
#define LED1_GPIO_CLK_ENABLE()             LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB)

#define LED2_PIN                           LL_GPIO_PIN_11
#define LED2_GPIO_PORT                     GPIOB
#define LED2_GPIO_CLK_ENABLE()             LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB)

#define LED3_PIN                           LL_GPIO_PIN_8
#define LED3_GPIO_PORT                     GPIOB
#define LED3_GPIO_CLK_ENABLE()             LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB)

/**
  * @brief Buttons
  */
#define BUTTON1_PIN                         LL_GPIO_PIN_13
#define BUTTON1_GPIO_PORT                   GPIOC
#define BUTTON1_GPIO_CLK_ENABLE()           LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC)
#define BUTTON1_EXTI_LINE                   LL_EXTI_LINE_13
#define BUTTON1_EXTI_IRQn                   EXTI13_IRQn
#define BUTTON1_EXTI_LINE_ENABLE()          LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_13)
#define BUTTON1_EXTI_FALLING_TRIG_ENABLE()  LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_13)
#define BUTTON1_IRQHANDLER                  EXTI13_IRQHandler

#define BUTTON2_PIN                         LL_GPIO_PIN_6
#define BUTTON2_GPIO_PORT                   GPIOB
#define BUTTON2_GPIO_CLK_ENABLE()           LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB)
#define BUTTON2_EXTI_LINE                   LL_EXTI_LINE_6
#define BUTTON2_EXTI_IRQn                   EXTI6_IRQn
#define BUTTON2_EXTI_LINE_ENABLE()          LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_6)
#define BUTTON2_EXTI_FALLING_TRIG_ENABLE()  LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_6)
#define BUTTON2_IRQHANDLER                  EXTI6_IRQHandler

#define BUTTON3_PIN                         LL_GPIO_PIN_7
#define BUTTON3_GPIO_PORT                   GPIOB
#define BUTTON3_GPIO_CLK_ENABLE()           LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB)
#define BUTTON3_EXTI_LINE                   LL_EXTI_LINE_7
#define BUTTON3_EXTI_IRQn                   EXTI7_IRQn
#define BUTTON3_EXTI_LINE_ENABLE()          LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_7)
#define BUTTON3_EXTI_FALLING_TRIG_ENABLE()  LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_7)
#define BUTTON3_IRQHANDLER                  EXTI7_IRQHandler

/* ==============   BOARD SPECIFIC CONFIGURATION CODE END      ============== */

/**
  * @brief Toggle periods for various blinking modes
  */
#define LED_BLINK_FAST  200
#define LED_BLINK_SLOW  500
#define LED_BLINK_ERROR 1000

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* MAIN_H */
