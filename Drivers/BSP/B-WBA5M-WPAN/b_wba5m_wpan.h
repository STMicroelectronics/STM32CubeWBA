/**
  ******************************************************************************
  * @file    b_wba5m_wpan.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for:
  *          - LEDs and push-button available on B-WBA5M-WPAN Kit
  *            from STMicroelectronics
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef B_WBA5M_WPAN_H
#define B_WBA5M_WPAN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "b_wba5m_wpan_conf.h"
#include "b_wba5m_wpan_errno.h"

#if (USE_BSP_COM_FEATURE > 0)
#if (USE_COM_LOG > 0)
#include <stdio.h>
#endif /* USE_COM_LOG */
#endif /* USE_BSP_COM_FEATURE */

/** @addtogroup BSP
  * @{
  */

/** @addtogroup B_WBA5M_WPAN
  * @{
  */

/** @addtogroup B_WBA5M_WPAN_COMMON
  * @{
  */

/** @defgroup B_WBA5M_WPAN_COMMON_Exported_Types B_WBA5M_WPAN COMMON Exported Types
  * @{
  */
typedef enum
{
  LD1 = 0,
  LED_BLUE = LD1,
} Led_TypeDef;

typedef enum
{
  B2 = 0,
} Button_TypeDef;

typedef enum
{
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
} ButtonMode_TypeDef;

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
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS == 1) */
#endif /* (USE_BSP_COM_FEATURE == 1) */

/**
  * @}
  */

/** @defgroup B_WBA5M_WPAN_COMMON_Exported_Constants B_WBA5M_WPAN COMMON Exported Constants
  * @{
  */

/**
  * @brief B-WBA5M-WPAN BSP Driver version number
  */
#define B_WBA5M_WPAN_BSP_VERSION_MAIN   (0x01U) /*!< [31:24] main version */
#define B_WBA5M_WPAN_BSP_VERSION_SUB1   (0x00U) /*!< [23:16] sub1 version */
#define B_WBA5M_WPAN_BSP_VERSION_SUB2   (0x00U) /*!< [15:8]  sub2 version */
#define B_WBA5M_WPAN_BSP_VERSION_RC     (0x00U) /*!< [7:0]  release candidate */
#define B_WBA5M_WPAN_BSP_VERSION        ((B_WBA5M_WPAN_BSP_VERSION_MAIN << 24U)\
                                        |(B_WBA5M_WPAN_BSP_VERSION_SUB1 << 16U)\
                                        |(B_WBA5M_WPAN_BSP_VERSION_SUB2 << 8U )\
                                        |(B_WBA5M_WPAN_BSP_VERSION_RC))

#define B_WBA5M_WPAN_BSP_BOARD_NAME  "B-WBA5M-WPAN"
#define B_WBA5M_WPAN_BSP_BOARD_ID    "MB2131"

/** @defgroup B_WBA5M_WPAN_COMMON_LED B_WBA5M_WPAN COMMON LED
  * @{
  */
#define LEDn                               1U

#define LD1_GPIO_PORT                      GPIOA
#define LD1_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOA_CLK_ENABLE()
#define LD1_PIN                            GPIO_PIN_1
/**
  * @}
  */

/** @defgroup B_WBA5M_WPAN_COMMON_BUTTON B_WBA5M_WPAN COMMON BUTTON
  * @{
  */
#define BUTTONn                            1U

#define B2_GPIO_PORT                       GPIOC
#define B2_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOC_CLK_ENABLE()
#define B2_PIN                             GPIO_PIN_13
#define B2_EXTI_LINE                       EXTI_LINE_13
#define B2_EXTI_IRQn                       EXTI13_IRQn
/**
  * @}
  */

#if (USE_BSP_COM_FEATURE == 1)
/** @defgroup B_WBA5M_WPAN_COMMON_COM B_WBA5M_WPAN COMMON COM
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

/**
  * @}
  */

/** @addtogroup B_WBA5M_WPAN_COMMON_Exported_Variables
  * @{
  */
#if (USE_BSP_COM_FEATURE == 1)
extern UART_HandleTypeDef hcom_uart[COMn];
#endif
/**
  * @}
  */

/** @addtogroup B_WBA5M_WPAN_COMMON_Exported_Functions
  * @{
  */
int32_t        BSP_GetVersion(void);
const uint8_t* BSP_GetBoardName(void);
const uint8_t* BSP_GetBoardID(void);

/** @addtogroup B_WBA5M_WPAN_COMMON_LED_Functions
  * @{
  */
int32_t BSP_LED_Init(Led_TypeDef Led);
int32_t BSP_LED_DeInit(Led_TypeDef Led);
int32_t BSP_LED_On(Led_TypeDef Led);
int32_t BSP_LED_Off(Led_TypeDef Led);
int32_t BSP_LED_Toggle(Led_TypeDef Led);
int32_t BSP_LED_GetState(Led_TypeDef Led);
/**
  * @}
  */

/** @addtogroup B_WBA5M_WPAN_COMMON_BUTTON_Functions
  * @{
  */
int32_t BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
int32_t BSP_PB_DeInit(Button_TypeDef Button);
int32_t BSP_PB_GetState(Button_TypeDef Button);
void    BSP_PB_Callback(Button_TypeDef Button);
void    BSP_PB_IRQHandler(Button_TypeDef Button);
/**
  * @}
  */

#if (USE_BSP_COM_FEATURE == 1)
/** @addtogroup B_WBA5M_WPAN_COMMON_COM_Functions
  * @{
  */
int32_t BSP_COM_Init(COM_TypeDef COM, COM_InitTypeDef *COM_Init);
int32_t BSP_COM_DeInit(COM_TypeDef COM);
#if (USE_COM_LOG == 1)
int32_t BSP_COM_SelectLogPort(COM_TypeDef COM);
#endif /* (USE_COM_LOG == 1) */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
int32_t BSP_COM_RegisterDefaultMspCallbacks(COM_TypeDef COM);
int32_t BSP_COM_RegisterMspCallbacks(COM_TypeDef COM, BSP_COM_Cb_t *CallBacks);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS == 1) */
HAL_StatusTypeDef MX_USART1_Init(UART_HandleTypeDef* huart, MX_UART_InitTypeDef *MXInit);
/**
  * @}
  */
#endif /* (USE_BSP_COM_FEATURE == 1) */

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

#endif /* B_WBA5M_WPAN_H */
