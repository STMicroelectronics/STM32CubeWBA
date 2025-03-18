/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Templates_LL/TrustZoneDisabled/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wbaxx_ll_icache.h"
#include "stm32wbaxx_ll_pwr.h"
#include "stm32wbaxx_ll_rcc.h"
#include "stm32wbaxx_ll_bus.h"
#include "stm32wbaxx_ll_system.h"
#include "stm32wbaxx_ll_exti.h"
#include "stm32wbaxx_ll_cortex.h"
#include "stm32wbaxx_ll_utils.h"
#include "stm32wbaxx_ll_dma.h"
#include "stm32wbaxx_ll_gpio.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* ==============   BOARD SPECIFIC CONFIGURATION CODE BEGIN    ============== */
/**
  * @brief LED
  */
#define LED1_PIN                           LL_GPIO_PIN_8
#define LED1_GPIO_PORT                     GPIOD
#define LED1_GPIO_CLK_ENABLE()             LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD)

#define LED2_PIN                           LL_GPIO_PIN_4
#define LED2_GPIO_PORT                     GPIOC
#define LED2_GPIO_CLK_ENABLE()             LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC)

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

#define BUTTON2_PIN                         LL_GPIO_PIN_5
#define BUTTON2_GPIO_PORT                   GPIOC
#define BUTTON2_GPIO_CLK_ENABLE()           LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC)
#define BUTTON2_EXTI_LINE                   LL_EXTI_LINE_5
#define BUTTON2_EXTI_IRQn                   EXTI5_IRQn
#define BUTTON2_EXTI_LINE_ENABLE()          LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_5)
#define BUTTON2_EXTI_FALLING_TRIG_ENABLE()  LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_5)
#define BUTTON2_IRQHANDLER                  EXTI5_IRQHandler

#define BUTTON3_PIN                         LL_GPIO_PIN_4
#define BUTTON3_GPIO_PORT                   GPIOB
#define BUTTON3_GPIO_CLK_ENABLE()           LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB)
#define BUTTON3_EXTI_LINE                   LL_EXTI_LINE_4
#define BUTTON3_EXTI_IRQn                   EXTI4_IRQn
#define BUTTON3_EXTI_LINE_ENABLE()          LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_4)
#define BUTTON3_EXTI_FALLING_TRIG_ENABLE()  LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_4)
#define BUTTON3_IRQHANDLER                  EXTI4_IRQHandler

/* ==============   BOARD SPECIFIC CONFIGURATION CODE END      ============== */

/**
  * @brief Toggle periods for various blinking modes
  */
#define LED_BLINK_FAST  200
#define LED_BLINK_SLOW  500
#define LED_BLINK_ERROR 1000

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
