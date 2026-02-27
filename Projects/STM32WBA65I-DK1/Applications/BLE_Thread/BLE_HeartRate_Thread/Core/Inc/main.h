/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wbaxx_hal.h"
#include "app_conf.h"
#include "app_entry.h"
#include "app_common.h"
#include "app_debug.h"

#include "stm32wbaxx_ll_icache.h"
#include "stm32wbaxx_ll_tim.h"
#include "stm32wbaxx_ll_bus.h"
#include "stm32wbaxx_ll_cortex.h"
#include "stm32wbaxx_ll_rcc.h"
#include "stm32wbaxx_ll_system.h"
#include "stm32wbaxx_ll_utils.h"
#include "stm32wbaxx_ll_pwr.h"
#include "stm32wbaxx_ll_gpio.h"
#include "stm32wbaxx_ll_dma.h"

#include "stm32wbaxx_ll_exti.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_bsp.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void MX_ICACHE_Init(void);
void MX_RAMCFG_Init(void);
void MX_RNG_Init(void);
void MX_RTC_Init(void);
void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void MX_CRC_Init(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SCK_A_Pin GPIO_PIN_7
#define SCK_A_GPIO_Port GPIOA
#define USER_Buttons_Pin GPIO_PIN_3
#define USER_Buttons_GPIO_Port GPIOA
#define SPI3_MISO_Pin GPIO_PIN_1
#define SPI3_MISO_GPIO_Port GPIOA
#define SPI3_SCK_Pin GPIO_PIN_0
#define SPI3_SCK_GPIO_Port GPIOA
#define LD5_Pin GPIO_PIN_9
#define LD5_GPIO_Port GPIOD
#define LD6_Pin GPIO_PIN_8
#define LD6_GPIO_Port GPIOD
#define USB_HS_N_Pin GPIO_PIN_7
#define USB_HS_N_GPIO_Port GPIOD
#define USB_HS_P_Pin GPIO_PIN_6
#define USB_HS_P_GPIO_Port GPIOD
#define SD_A_Pin GPIO_PIN_5
#define SD_A_GPIO_Port GPIOD
#define SPI3_MOSI_Pin GPIO_PIN_8
#define SPI3_MOSI_GPIO_Port GPIOB
#define OSC32_OUT_Pin GPIO_PIN_15
#define OSC32_OUT_GPIO_Port GPIOC
#define OSC32_IN_Pin GPIO_PIN_14
#define OSC32_IN_GPIO_Port GPIOC
#define RST_DISP_Pin GPIO_PIN_3
#define RST_DISP_GPIO_Port GPIOE
#define MCLK_A_Pin GPIO_PIN_2
#define MCLK_A_GPIO_Port GPIOE
#define CS_DISP_Pin GPIO_PIN_1
#define CS_DISP_GPIO_Port GPIOE
#define D_C_DISP_Pin GPIO_PIN_0
#define D_C_DISP_GPIO_Port GPIOE
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define JTDI_Pin GPIO_PIN_15
#define JTDI_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define I2C1_SCL_Pin GPIO_PIN_2
#define I2C1_SCL_GPIO_Port GPIOB
#define I2C1_SDA_Pin GPIO_PIN_1
#define I2C1_SDA_GPIO_Port GPIOB
#define SD_B_Pin GPIO_PIN_5
#define SD_B_GPIO_Port GPIOC
#define FS_A_Pin GPIO_PIN_4
#define FS_A_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
