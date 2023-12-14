/**
  ******************************************************************************
  * @file    stm32wba55g_discovery.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for STM32WBA55G_DK1:
  *          LEDs
  *          Joystick
  *          COM port
  *          hardware resources.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32WBA55G_DK1_H
#define STM32WBA55G_DK1_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wba55g_discovery_conf.h"
#include "stm32wba55g_discovery_errno.h"

#if (USE_BSP_COM_FEATURE > 0)
#if (USE_COM_LOG > 0)
#include <stdio.h>
#endif /* USE_COM_LOG */
#endif /* USE_BSP_COM_FEATURE */

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32WBA55G_DK1
  * @{
  */

/** @addtogroup STM32WBA55G_DK1_LOW_LEVEL
  * @{
  */

/** @defgroup STM32WBA55G_DK1_LOW_LEVEL_Exported_Types LOW LEVEL Exported Types
  * @{
  */
typedef enum
{
  LD3 = 0U,
  LED_BLUE = LD3
} Led_TypeDef;

#if (USE_BSP_COM_FEATURE == 1)
typedef enum
{
  COM1 = 0
} COM_TypeDef;

typedef enum
{
  COM_WORDLENGTH_7B = UART_WORDLENGTH_7B,
  COM_WORDLENGTH_8B = UART_WORDLENGTH_8B,
  COM_WORDLENGTH_9B = UART_WORDLENGTH_9B
} COM_WordLengthTypeDef;

typedef enum
{
  COM_STOPBITS_1 = UART_STOPBITS_1,
  COM_STOPBITS_2 = UART_STOPBITS_2
} COM_StopBitsTypeDef;

typedef enum
{
  COM_PARITY_NONE = UART_PARITY_NONE,
  COM_PARITY_EVEN = UART_PARITY_EVEN,
  COM_PARITY_ODD  = UART_PARITY_ODD
} COM_ParityTypeDef;

typedef enum
{
  COM_HWCONTROL_NONE    = UART_HWCONTROL_NONE,
  COM_HWCONTROL_RTS     = UART_HWCONTROL_RTS,
  COM_HWCONTROL_CTS     = UART_HWCONTROL_CTS,
  COM_HWCONTROL_RTS_CTS = UART_HWCONTROL_RTS_CTS
} COM_HwFlowCtlTypeDef;

typedef struct
{
  uint32_t              BaudRate;
  COM_WordLengthTypeDef WordLength;
  COM_StopBitsTypeDef   StopBits;
  COM_ParityTypeDef     Parity;
  COM_HwFlowCtlTypeDef  HwFlowCtl;
} COM_InitTypeDef;

#define MX_UART_InitTypeDef COM_InitTypeDef

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
typedef struct
{
  pUART_CallbackTypeDef  pMspInitCb;
  pUART_CallbackTypeDef  pMspDeInitCb;
} BSP_COM_Cb_t;
#endif
#endif /* (USE_BSP_COM_FEATURE == 1) */

typedef enum
{
  JOY_NONE  = 0x00,
  JOY_SEL   = 0x01,
  JOY_DOWN  = 0x02,
  JOY_LEFT  = 0x04,
  JOY_RIGHT = 0x08,
  JOY_UP    = 0x10,
  JOY_ALL   = 0x1F
} JOYPin_TypeDef;

typedef enum
{
  JOY_MODE_GPIO = 0,
  JOY_MODE_POLLING = JOY_MODE_GPIO,
  JOY_MODE_EXTI = 1,
  JOY_MODE_IT = JOY_MODE_EXTI
} JOYMode_TypeDef;

typedef enum
{
  JOY1 = 0
} JOY_TypeDef;

typedef struct
{
  FunctionalState ContinuousConvMode;
} MX_ADC_Config_t;

#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
typedef struct
{
  pADC_CallbackTypeDef  pMspInitCb;
  pADC_CallbackTypeDef  pMspDeInitCb;
} BSP_JOY_Cb_t;
#endif /* (USE_HAL_ADC_REGISTER_CALLBACKS == 1) */
/**
  * @}
  */

/** @defgroup STM32WBA55G_DK1_LOW_LEVEL_Exported_Constants LOW LEVEL Exported Constants
  * @{
  */
/**
  * @brief  Define for STM32WBA55G_DK1 board
  */
#if !defined (USE_STM32WBA55G_DK1)
#define USE_STM32WBA55G_DK1
#endif /* USE_STM32WBA55G_DK1 */

/**
  * @brief STM32WBA55G_DK1 BSP Driver version number
  */
#define STM32WBA55G_DK1_BSP_VERSION_MAIN   (0x01U) /*!< [31:24] main version */
#define STM32WBA55G_DK1_BSP_VERSION_SUB1   (0x00U) /*!< [23:16] sub1 version */
#define STM32WBA55G_DK1_BSP_VERSION_SUB2   (0x00U) /*!< [15:8]  sub2 version */
#define STM32WBA55G_DK1_BSP_VERSION_RC     (0x00U) /*!< [7:0]  release candidate */
#define STM32WBA55G_DK1_BSP_VERSION        ((STM32WBA55G_DK1_BSP_VERSION_MAIN << 24)\
                                            |(STM32WBA55G_DK1_BSP_VERSION_SUB1 << 16)\
                                            |(STM32WBA55G_DK1_BSP_VERSION_SUB2 << 8 )\
                                            |(STM32WBA55G_DK1_BSP_VERSION_RC))

#define STM32WBA55G_DK1_BSP_BOARD_NAME  "STM32WBA55G-DK1";
#define STM32WBA55G_DK1_BSP_BOARD_ID    "MB1802 and MB1803";

/** @defgroup STM32WBA55G_DK1_LOW_LEVEL_LED LOW LEVEL LED
  * @{
  */
#define LEDn                                   1U

#if defined (STM32WBA55G_DK1_LD3_ON_PB15)
#define LD3_GPIO_PORT                          GPIOB
#define LD3_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOB_CLK_ENABLE()
#define LD3_PIN                                GPIO_PIN_15
#else
#define LD3_GPIO_PORT                          GPIOA
#define LD3_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOA_CLK_ENABLE()
#define LD3_PIN                                GPIO_PIN_0
#endif
/**
  * @}
  */

#if (USE_BSP_COM_FEATURE == 1)
/** @defgroup STM32WBA55G_DK1_LOW_LEVEL_COM LOW LEVEL COM
  * @{
  */
#define COMn                                    1U

#define COM1_UART                               USART1
#define COM1_CLK_ENABLE()                       __HAL_RCC_USART1_CLK_ENABLE()
#define COM1_CLK_DISABLE()                      __HAL_RCC_USART1_CLK_DISABLE()
#define COM1_TX_GPIO_PORT                       GPIOB
#define COM1_TX_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOB_CLK_ENABLE()
#define COM1_TX_PIN                             GPIO_PIN_12
#define COM1_TX_AF                              GPIO_AF7_USART1
#define COM1_RX_GPIO_PORT                       GPIOA
#define COM1_RX_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOA_CLK_ENABLE()
#define COM1_RX_PIN                             GPIO_PIN_8
#define COM1_RX_AF                              GPIO_AF7_USART1
/**
  * @}
  */
#endif /* (USE_BSP_COM_FEATURE == 1) */


/** @defgroup STM32WBA55G_DK1_LOW_LEVEL_JOY LOW LEVEL JOYSTICK
  * @{
  */
#define JOYn                                    1U

#define JOY1_ADC                                ADC4
#define JOY1_ADC_CHANNEL                        ADC_CHANNEL_8
#define JOY1_CLK_ENABLE()                       __HAL_RCC_ADC4_CLK_ENABLE()
#define JOY1_CLK_DISABLE()                      __HAL_RCC_ADC4_CLK_DISABLE()
#define JOY1_CHANNEL_GPIO_PORT                  GPIOA
#define JOY1_CHANNEL_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOA_CLK_ENABLE()
#define JOY1_CHANNEL_GPIO_PIN                   GPIO_PIN_1

#ifndef BSP_JOY_DEBOUNCE_DELAY
/* Joystick Debounce Delay in ms */
#define BSP_JOY_DEBOUNCE_DELAY                  200
#endif
/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup STM32WBA55G_DK1_LOW_LEVEL_Exported_Variables
  * @{
  */
#if (USE_BSP_COM_FEATURE == 1)
extern UART_HandleTypeDef hcom_uart[COMn];
#endif

extern ADC_HandleTypeDef hjoy_adc[JOYn];
/**
  * @}
  */

/** @addtogroup STM32WBA55G_DK1_LOW_LEVEL_Exported_Functions
  * @{
  */
int32_t  BSP_GetVersion(void);
const uint8_t *BSP_GetBoardName(void);
const uint8_t *BSP_GetBoardID(void);

int32_t  BSP_LED_Init(Led_TypeDef Led);
int32_t  BSP_LED_DeInit(Led_TypeDef Led);
int32_t  BSP_LED_On(Led_TypeDef Led);
int32_t  BSP_LED_Off(Led_TypeDef Led);
int32_t  BSP_LED_Toggle(Led_TypeDef Led);
int32_t  BSP_LED_GetState(Led_TypeDef Led);

#if (USE_BSP_COM_FEATURE == 1)
int32_t BSP_COM_Init(COM_TypeDef COM, COM_InitTypeDef *COM_Init);
int32_t BSP_COM_DeInit(COM_TypeDef COM);
#if (USE_COM_LOG == 1)
int32_t BSP_COM_SelectLogPort(COM_TypeDef COM);
#endif
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
int32_t BSP_COM_RegisterDefaultMspCallbacks(COM_TypeDef COM);
int32_t BSP_COM_RegisterMspCallbacks(COM_TypeDef COM, BSP_COM_Cb_t *CallBacks);
#endif
HAL_StatusTypeDef MX_USART1_Init(UART_HandleTypeDef* huart, MX_UART_InitTypeDef *MXInit);
#endif /* (USE_BSP_COM_FEATURE == 1) */

int32_t BSP_JOY_Init(JOY_TypeDef JOY, JOYMode_TypeDef JoyMode, JOYPin_TypeDef JoyPins);
int32_t BSP_JOY_DeInit(JOY_TypeDef JOY, JOYPin_TypeDef JoyPins);
int32_t BSP_JOY_GetState(JOY_TypeDef JOY);
void    BSP_JOY_Callback(JOY_TypeDef JOY, JOYPin_TypeDef JoyPin);
void    BSP_JOY_IRQHandler(JOY_TypeDef JOY, JOYPin_TypeDef JoyPin);
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
int32_t BSP_JOY_RegisterDefaultMspCallbacks(JOY_TypeDef JOY);
int32_t BSP_JOY_RegisterMspCallbacks(JOY_TypeDef JOY, BSP_JOY_Cb_t *CallBacks);
#endif /* (USE_HAL_ADC_REGISTER_CALLBACKS == 1) */
HAL_StatusTypeDef MX_ADC4_Init(ADC_HandleTypeDef* hadc, MX_ADC_Config_t *MXInit);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif
#endif /* STM32WBA55G_DK1_H */
