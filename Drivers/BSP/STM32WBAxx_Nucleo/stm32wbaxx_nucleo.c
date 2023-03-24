/**
  ******************************************************************************
  * @file    stm32wbaxx_nucleo.c
  * @author  MCD Application Team
  * @brief   This file provides set of firmware functions to manage:
  *          - LEDs and push-button available on STM32WBAXX-Nucleo Kit
  *            from STMicroelectronics
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

/* Includes ------------------------------------------------------------------*/
#include "stm32wbaxx_nucleo.h"

/** @addtogroup BSP
  * @{
  */

/** @defgroup STM32WBAXX_NUCLEO STM32WBAXX_NUCLEO
  * @{
  */

/** @defgroup STM32WBAXX_NUCLEO_COMMON STM32WBAXX_NUCLEO COMMON
  * @brief This file provides set of firmware functions to manage Leds and push-button
  *        available on STM32WBAxx-Nucleo Kit from STMicroelectronics.
  * @{
  */

/** @defgroup STM32WBAXX_NUCLEO_COMMON_Private_Defines STM32WBAXX_NUCLEO COMMON Private Defines
  * @{
  */
#if (USE_BSP_COM_FEATURE == 1)
#if (USE_COM_LOG == 1)
#define COM_POLL_TIMEOUT 1000
#endif
#endif
/**
  * @}
  */

/** @defgroup STM32WBAXX_NUCLEO_COMMON_Private_TypesDefinitions STM32WBAXX_NUCLEO COMMON Private Types Definitions
  * @{
  */
typedef void (* BSP_EXTI_LineCallback)(void);
/**
  * @}
  */

/** @defgroup STM32WBAXX_NUCLEO_COMMON_Private_FunctionPrototypes STM32WBAXX_NUCLEO COMMON Private Function Prototypes
  * @{
  */
static void B1_EXTI_Callback(void);
static void B2_EXTI_Callback(void);
static void B3_EXTI_Callback(void);

#if (USE_BSP_COM_FEATURE == 1)
static void UART_MspInit(UART_HandleTypeDef *huart);
static void UART_MspDeInit(UART_HandleTypeDef *huart);
#endif
/**
  * @}
  */

/** @defgroup STM32WBAXX_NUCLEO_COMMON_Exported_Variables STM32WBAXX_NUCLEO COMMON Exported Variables
  * @{
  */
#if (USE_BSP_COM_FEATURE == 1)
UART_HandleTypeDef hcom_uart[COMn];
#endif
/**
  * @}
  */

/** @defgroup STM32WBAXX_NUCLEO_COMMON_Private_Variables STM32WBAXX_NUCLEO COMMON Private Variables
  * @{
  */
static uint16_t      LED_PIN[LEDn]  = {LD1_PIN, LD2_PIN, LD3_PIN};
static GPIO_TypeDef *LED_PORT[LEDn] = {LD1_GPIO_PORT, LD2_GPIO_PORT, LD3_GPIO_PORT};

static uint16_t           BUTTON_PIN[BUTTONn]  = {B1_PIN, B2_PIN, B3_PIN};
static GPIO_TypeDef      *BUTTON_PORT[BUTTONn] = {B1_GPIO_PORT, B2_GPIO_PORT, B3_GPIO_PORT};
static IRQn_Type          BUTTON_IRQn[BUTTONn] = {B1_EXTI_IRQn, B2_EXTI_IRQn, B3_EXTI_IRQn};
static EXTI_HandleTypeDef hpb_exti[BUTTONn];

#if (USE_BSP_COM_FEATURE == 1)
static USART_TypeDef     *COM_UART[COMn] = {COM1_UART};
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
static uint32_t           IsComMspCbValid[COMn] = {0U};
#endif
#if (USE_COM_LOG == 1)
static COM_TypeDef        COM_ActiveLogPort = COM1;
#endif
#endif /* (USE_BSP_COM_FEATURE == 1) */
/**
  * @}
  */

/** @defgroup STM32WBAXX_NUCLEO_COMMON_Exported_Functions STM32WBAXX_NUCLEO COMMON Exported Functions
  * @{
  */

/**
  * @brief  This method returns the STM32WBAxx NUCLEO BSP Driver revision.
  * @retval version : 0xXYZR (8bits for each decimal, R for RC).
  */
int32_t BSP_GetVersion(void)
{
  return ((int32_t)STM32WBAxx_NUCLEO_BSP_VERSION);
}

/**
  * @brief  This method returns the board name.
  * @retval pointer to the board name string.
  */
const uint8_t* BSP_GetBoardName(void)
{
  return (const uint8_t*)STM32WBAXX_NUCLEO_BSP_BOARD_NAME;
}

/**
  * @brief  This method returns the board ID.
  * @retval pointer to the board ID string.
  */
const uint8_t* BSP_GetBoardID(void)
{
  return (const uint8_t*)STM32WBAXX_NUCLEO_BSP_BOARD_ID;
}

/** @defgroup STM32WBAXX_NUCLEO_COMMON_LED_Functions STM32WBAXX_NUCLEO COMMON LED Functions
  * @{
  */

/**
  * @brief  Configure LED GPIO.
  * @param  Led Specifies the Led to be configured.
  *   This parameter can be one of following parameters:
  *     @arg  LD1
  *     @arg  LD2
  *     @arg  LD3
  * @retval BSP error code.
  */
int32_t BSP_LED_Init(Led_TypeDef Led)
{
  GPIO_InitTypeDef GPIO_Init;

  /* Enable the GPIO_LED Clock */
  if (Led == LD1)
  {
    LD1_GPIO_CLK_ENABLE();
  }
  else if (Led == LD2)
  {
    LD2_GPIO_CLK_ENABLE();
  }
  else /* Led = LD3 */
  {
    LD3_GPIO_CLK_ENABLE();
  }

  /* configure the GPIO_LED pin */
  GPIO_Init.Pin   = LED_PIN[Led];
  GPIO_Init.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_Init.Pull  = GPIO_PULLUP;
  GPIO_Init.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED_PORT[Led], &GPIO_Init);

  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);

  return BSP_ERROR_NONE;
}

/**
  * @brief  DeInitialize LEDs.
  * @param  Led LED to be de-init.
  *   This parameter can be one of the following values:
  *     @arg  LD1
  *     @arg  LD2
  *     @arg  LD3
  * @note BSP_LED_DeInit() does not disable the GPIO clock.
  * @retval BSP error code.
  */
int32_t BSP_LED_DeInit(Led_TypeDef Led)
{
  GPIO_InitTypeDef GPIO_Init;

  /* DeInit the GPIO_LED pin */
  GPIO_Init.Pin = LED_PIN[Led];

  /* Turn off LED */
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);
  HAL_GPIO_DeInit(LED_PORT[Led], GPIO_Init.Pin);
  return BSP_ERROR_NONE;
}

/**
  * @brief  Turn selected LED On.
  * @param  Led Specifies the Led to be set on.
  *   This parameter can be one of following parameters:
  *     @arg  LD1
  *     @arg  LD2
  *     @arg  LD3
  * @retval BSP error code.
  */
int32_t BSP_LED_On(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET);

  return BSP_ERROR_NONE;
}

/**
  * @brief  Turn selected LED Off.
  * @param  Led Specifies the Led to be set off.
  *   This parameter can be one of following parameters:
  *     @arg  LD1
  *     @arg  LD2
  *     @arg  LD3
  * @retval BSP error code.
  */
int32_t BSP_LED_Off(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);

  return BSP_ERROR_NONE;
}

/**
  * @brief  Toggle the selected LED.
  * @param  Led Specifies the Led to be toggled.
  *   This parameter can be one of following parameters:
  *     @arg  LD1
  *     @arg  LD2
  *     @arg  LD3
  * @retval BSP error code.
  */
int32_t BSP_LED_Toggle(Led_TypeDef Led)
{
  HAL_GPIO_TogglePin(LED_PORT[Led], LED_PIN[Led]);
  return BSP_ERROR_NONE;
}

/**
  * @brief  Get the state of selected LED.
  * @param  Led Specifies the Led.
  *   This parameter can be one of following parameters:
  *     @arg  LD1
  *     @arg  LD2
  *     @arg  LD3
  * @retval 0 means off, 1 means on.
  */
int32_t BSP_LED_GetState(Led_TypeDef Led)
{
  int32_t status;

  status = (HAL_GPIO_ReadPin(LED_PORT[Led], LED_PIN[Led]) == GPIO_PIN_RESET) ? 1 : 0;

  return status;
}

/**
  * @}
  */

/** @defgroup STM32WBAXX_NUCLEO_COMMON_BUTTON_Functions STM32WBAXX_NUCLEO_COMMON BUTTON Functions
  * @{
  */

/**
  * @brief  Configure Button GPIO and EXTI Line.
  * @param  Button Specifies the Button to be configured.
  *   This parameter should be:
  *     @arg B1
  *     @arg B2
  *     @arg B3
  * @param  ButtonMode Specifies Button mode.
  *   This parameter can be one of following parameters:
  *     @arg BUTTON_MODE_GPIO: Button will be used as simple IO
  *     @arg BUTTON_MODE_EXTI: Button will be connected to EXTI line with interrupt
  *                            generation capability
  * @retval BSP error code.
  */
int32_t BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode)
{
  int32_t               status = BSP_ERROR_NONE;
  GPIO_InitTypeDef      GPIO_Init;
  uint32_t              BSP_BUTTON_IT_PRIO[BUTTONn] = {BSP_B1_IT_PRIORITY,
                                                       BSP_B2_IT_PRIORITY,
                                                       BSP_B3_IT_PRIORITY};
  uint32_t              BUTTON_EXTI_LINE[BUTTONn]   = {B1_EXTI_LINE, B2_EXTI_LINE, B3_EXTI_LINE};
  BSP_EXTI_LineCallback ButtonCallback[BUTTONn]     = {B1_EXTI_Callback,
                                                       B2_EXTI_Callback,
                                                       B3_EXTI_Callback};

  /* Enable the BUTTON clock */
  if (Button == B1)
  {
    B1_GPIO_CLK_ENABLE();
  }
  else if (Button == B2)
  {
    B2_GPIO_CLK_ENABLE();
  }
  else /* B3 */
  {
    B3_GPIO_CLK_ENABLE();
  }

  GPIO_Init.Pin   = BUTTON_PIN[Button];
  GPIO_Init.Pull  = GPIO_NOPULL;
  GPIO_Init.Speed = GPIO_SPEED_FREQ_HIGH;

  if (ButtonMode == BUTTON_MODE_GPIO)
  {
    /* Configure Button pin as input */
    GPIO_Init.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(BUTTON_PORT[Button], &GPIO_Init);
  }

  if (ButtonMode == BUTTON_MODE_EXTI)
  {
    /* Configure Button pin as input with External interrupt */
    GPIO_Init.Mode = GPIO_MODE_IT_FALLING;
    HAL_GPIO_Init(BUTTON_PORT[Button], &GPIO_Init);

    if (HAL_EXTI_GetHandle(&hpb_exti[Button], BUTTON_EXTI_LINE[Button]) == HAL_OK)
    {
      if (HAL_EXTI_RegisterCallback(&hpb_exti[Button], HAL_EXTI_FALLING_CB_ID, ButtonCallback[Button]) == HAL_OK)
      {
        /* Enable and set Button EXTI Interrupt to the lowest priority */
        HAL_NVIC_SetPriority(BUTTON_IRQn[Button], BSP_BUTTON_IT_PRIO[Button], 0x00);
        HAL_NVIC_EnableIRQ(BUTTON_IRQn[Button]);
      }
      else
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
    }
    else
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
  }

  return status;
}

/**
  * @brief  DeInitialize Push Button.
  * @param  Button Button to be configured.
  *   This parameter should be:
  *     @arg B1
  *     @arg B2
  *     @arg B3
  * @note BSP_PB_DeInit() does not disable the GPIO clock.
  * @retval BSP error code.
  */
int32_t BSP_PB_DeInit(Button_TypeDef Button)
{
  GPIO_InitTypeDef GPIO_Init;

  GPIO_Init.Pin = BUTTON_PIN[Button];
  HAL_NVIC_DisableIRQ(BUTTON_IRQn[Button]);
  HAL_GPIO_DeInit(BUTTON_PORT[Button], GPIO_Init.Pin);

  return BSP_ERROR_NONE;
}

/**
  * @brief  Return the selected Button state.
  * @param  Button Specifies the Button to be checked.
  *   This parameter should be:
  *     @arg B1
  *     @arg B2
  *     @arg B3
  * @retval 0 means released, 1 means pressed.
  */
int32_t BSP_PB_GetState(Button_TypeDef Button)
{
  int32_t status;

  status = (HAL_GPIO_ReadPin(BUTTON_PORT[Button], BUTTON_PIN[Button]) == GPIO_PIN_RESET) ? 1 : 0;

  return status;
}

/**
  * @brief  BSP Push Button callback.
  * @param  Button Specifies the pin connected EXTI line.
  *   This parameter should be:
  *     @arg B1
  *     @arg B2
  *     @arg B3
  * @retval None.
  */
__weak void BSP_PB_Callback(Button_TypeDef Button)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Button);

  /* This function should be implemented by the user application.
     It is called into this driver when an event on Button is triggered. */
}

/**
  * @brief  BSP PB interrupt handler.
  * @param  Button Button where interrupt occurs.
  *   This parameter should be:
  *     @arg B1
  *     @arg B2
  *     @arg B3
  * @retval None.
  */
void BSP_PB_IRQHandler(Button_TypeDef Button)
{
  HAL_EXTI_IRQHandler(&hpb_exti[Button]);
}

/**
  * @}
  */

#if (USE_BSP_COM_FEATURE == 1)
/** @defgroup STM32WBAXX_NUCLEO_COMMON_COM_Functions STM32WBAXX_NUCLEO COMMON COM Functions
  * @{
  */

/**
  * @brief  Configure COM port.
  * @param  COM COM port to be configured.
  *          This parameter can be COM1.
  * @param  COM_Init Pointer to a COM_InitTypeDef structure that contains the
  *                  configuration information for the specified COM peripheral.
  * @retval BSP error code.
  */
int32_t BSP_COM_Init(COM_TypeDef COM, COM_InitTypeDef *COM_Init)
{
  int32_t status = BSP_ERROR_NONE;

  if (COM_Init == NULL)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Initialize COM instance */
    hcom_uart[COM].Instance = COM_UART[COM];

#if (USE_HAL_UART_REGISTER_CALLBACKS == 0)
    UART_MspInit(&hcom_uart[COM]);
#else
    if (IsComMspCbValid[COM] == 0U)
    {
      /* Register the MSP callbacks */
      if (BSP_COM_RegisterDefaultMspCallbacks(COM) != BSP_ERROR_NONE)
      {
        status = BSP_ERROR_MSP_FAILURE;
      }
    }
#endif

    if (status == BSP_ERROR_NONE)
    {
      if (MX_USART1_Init(&hcom_uart[COM], COM_Init) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
    }
  }

  return status;
}

/**
  * @brief  DeInitialize COM port.
  * @param  COM COM port to be de_init.
  *          This parameter can be COM1.
  * @retval BSP error code.
  */
int32_t BSP_COM_DeInit(COM_TypeDef COM)
{
  int32_t status = BSP_ERROR_NONE;

  /* COM de-init */
  hcom_uart[COM].Instance = COM_UART[COM];
  if (HAL_UART_DeInit(&hcom_uart[COM]) != HAL_OK)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  }
#if (USE_HAL_UART_REGISTER_CALLBACKS == 0)
  else
  {
    UART_MspDeInit(&hcom_uart[COM]);
  }
#endif

  return status;
}

#if (USE_COM_LOG == 1)
/**
  * @brief  Select the active COM port.
  * @param  COM COM port to be activated.
  *          This parameter can be COM1.
  * @retval BSP error code.
  */
int32_t BSP_COM_SelectLogPort(COM_TypeDef COM)
{
  if (COM_ActiveLogPort != COM)
  {
    COM_ActiveLogPort = COM;
  }
  return BSP_ERROR_NONE;
}

#if defined(__ARMCC_VERSION) || defined(__ICCARM__)
size_t __write(int Handle, const unsigned char *Buf, size_t Bufsize)
{
  int i;

  for(i=0; i<Bufsize; i++)
  {
    (void)HAL_UART_Transmit(&hcom_uart[COM_ActiveLogPort], (uint8_t *)&Buf[i], 1, COM_POLL_TIMEOUT);
  }

  return Bufsize;
}
#elif __GNUC__
int __io_putchar(int ch)
{
  (void) HAL_UART_Transmit(&hcom_uart[COM_ActiveLogPort], (uint8_t *) &ch, 1, COM_POLL_TIMEOUT);
  return ch;
}
#endif
#endif /* (USE_COM_LOG == 1) */

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
/**
  * @brief  Register default COM msp callbacks.
  * @param  COM COM port.
  *          This parameter can be COM1.
  * @retval BSP status.
  */
int32_t BSP_COM_RegisterDefaultMspCallbacks(COM_TypeDef COM)
{
  int32_t status = BSP_ERROR_NONE;

  /* Register MspInit/MspDeInit callbacks */
  if (HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPINIT_CB_ID, UART_MspInit) != HAL_OK)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  }
  else if (HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPDEINIT_CB_ID, UART_MspDeInit) != HAL_OK)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    IsComMspCbValid[COM] = 1U;
  }
  /* Return BSP status */
  return status;
}

/**
  * @brief  Register BSP COM msp callbacks.
  * @param  COM COM port.
  *          This parameter can be COM1.
  * @param  CallBacks Pointer to MspInit/MspDeInit callback functions.
  * @retval BSP status.
  */
int32_t BSP_COM_RegisterMspCallbacks(COM_TypeDef COM, BSP_COM_Cb_t *CallBacks)
{
  int32_t status = BSP_ERROR_NONE;

  if (CallBacks == NULL)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Register MspInit/MspDeInit callbacks */
    if (HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPINIT_CB_ID, CallBacks->pMspInitCb) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    else if (HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPDEINIT_CB_ID, CallBacks->pMspDeInitCb) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      IsComMspCbValid[COM] = 1U;
    }
  }
  /* Return BSP status */
  return status;
}
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS == 1) */

/**
  * @brief  Initialize USART1.
  * @param  huart UART handle.
  * @param  MXInit UART initialization structure.
  * @retval HAL status.
  */
__weak HAL_StatusTypeDef MX_USART1_Init(UART_HandleTypeDef* huart, MX_UART_InitTypeDef *MXInit)
{
  /* UART configuration */
  huart->Init.BaudRate       = MXInit->BaudRate;
  huart->Init.WordLength     = (uint32_t) MXInit->WordLength;
  huart->Init.StopBits       = (uint32_t) MXInit->StopBits;
  huart->Init.Parity         = (uint32_t) MXInit->Parity;
  huart->Init.Mode           = UART_MODE_TX_RX;
  huart->Init.HwFlowCtl      = (uint32_t) MXInit->HwFlowCtl;
  huart->Init.OverSampling   = UART_OVERSAMPLING_8;
  huart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart->Init.ClockPrescaler = UART_PRESCALER_DIV1;

  return HAL_UART_Init(huart);
}

/**
  * @}
  */
#endif /* (USE_BSP_COM_FEATURE == 1) */

/**
  * @}
  */

/** @defgroup STM32WBAXX_NUCLEO_COMMON_Private_Functions STM32WBAXX_NUCLEO COMMON Private Functions
  * @{
  */

/**
  * @brief  B1 EXTI line detection callbacks.
  * @retval None.
  */
static void B1_EXTI_Callback(void)
{
  BSP_PB_Callback(B1);
}

/**
  * @brief  B2 EXTI line detection callbacks.
  * @retval None.
  */
static void B2_EXTI_Callback(void)
{
  BSP_PB_Callback(B2);
}

/**
  * @brief  B3 EXTI line detection callbacks.
  * @retval None.
  */
static void B3_EXTI_Callback(void)
{
  BSP_PB_Callback(B3);
}

#if (USE_BSP_COM_FEATURE == 1)
/**
  * @brief  Initialize UART MSP.
  * @param  huart UART handle.
  * @retval None.
  */
static void UART_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef GPIO_Init;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* Enable COM and GPIO clocks */
  COM1_TX_GPIO_CLK_ENABLE();
  COM1_RX_GPIO_CLK_ENABLE();
  COM1_CLK_ENABLE();

  /* Configure COM Tx as alternate function */
  GPIO_Init.Pin       = COM1_TX_PIN;
  GPIO_Init.Mode      = GPIO_MODE_AF_PP;
  GPIO_Init.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_Init.Pull      = GPIO_PULLUP;
  GPIO_Init.Alternate = COM1_TX_AF;
  HAL_GPIO_Init(COM1_TX_GPIO_PORT, &GPIO_Init);

  /* Configure COM Rx as alternate function */
  GPIO_Init.Pin       = COM1_RX_PIN;
  GPIO_Init.Alternate = COM1_RX_AF;
  HAL_GPIO_Init(COM1_RX_GPIO_PORT, &GPIO_Init);
}

/**
  * @brief  Deinitialize UART MSP.
  * @param  huart UART handle.
  * @retval None.
  */
static void UART_MspDeInit(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* Disable GPIOs and COM clock */
  HAL_GPIO_DeInit(COM1_TX_GPIO_PORT, COM1_TX_PIN);
  HAL_GPIO_DeInit(COM1_RX_GPIO_PORT, COM1_RX_PIN);
  COM1_CLK_DISABLE();
}
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
