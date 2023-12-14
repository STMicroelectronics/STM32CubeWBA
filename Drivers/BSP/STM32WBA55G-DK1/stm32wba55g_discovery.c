/**
  ******************************************************************************
  * @file    stm32wba55g_discovery.c
  * @author  MCD Application Team
  * @brief   This file provides a set of firmware functions to manage
  *          - LEDs
  *          - Joystick
  *          - COM port
  *          available on STM32WBA55G_DK1 board (MB1802 and MB1803) from
  *          STMicroelectronics.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32wba55g_discovery.h"

#if defined(__ICCARM__)
#include <LowLevelIOInterface.h>
#endif /* __ICCARM__ */

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32WBA55G_DK1
  * @{
  */

/** @defgroup STM32WBA55G_DK1_LOW_LEVEL LOW LEVEL
  * @{
  */

/** @defgroup STM32WBA55G_DK1_LOW_LEVEL_Private_Defines LOW LEVEL Private Defines
  * @{
  */
#if (USE_BSP_COM_FEATURE == 1)
#if (USE_COM_LOG == 1)
#define COM_POLL_TIMEOUT 1000
#endif
#endif

#define JOY_ADC_POLL_TIMEOUT 1000
/**
  * @}
  */

/** @defgroup STM32WBA55G_DK1_LOW_LEVEL_Private_FunctionPrototypes  LOW LEVEL Private Function Prototypes
  * @{
  */
#if (USE_BSP_COM_FEATURE == 1)
static void UART_MspInit(UART_HandleTypeDef *huart);
static void UART_MspDeInit(UART_HandleTypeDef *huart);
#endif

static void ADC_MspInit(ADC_HandleTypeDef *hadc);
static void ADC_MspDeInit(ADC_HandleTypeDef *hadc);
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
static void ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef *hadc);
#endif /* (USE_ADC_SAI_REGISTER_CALLBACKS == 1) */
/**
  * @}
  */

/** @defgroup STM32WBA55G_DK1_LOW_LEVEL_Exported_Variables LOW LEVEL Exported Variables
  * @{
  */
#if (USE_BSP_COM_FEATURE == 1)
UART_HandleTypeDef hcom_uart[COMn];
#endif

ADC_HandleTypeDef hjoy_adc[JOYn];

__IO uint32_t joy_tick = 0U;
/**
  * @}
  */

/** @defgroup STM32WBA55G_DK1_LOW_LEVEL_Private_Variables LOW LEVEL Private Variables
  * @{
  */
static uint16_t      LED_PIN[LEDn]  = {LD3_PIN};
static GPIO_TypeDef *LED_PORT[LEDn] = {LD3_GPIO_PORT};

#if (USE_BSP_COM_FEATURE == 1)
static USART_TypeDef     *COM_UART[COMn] = {COM1_UART};
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
static uint32_t           IsComMspCbValid[COMn] = {0U};
#endif
#if (USE_COM_LOG == 1)
static COM_TypeDef        COM_ActiveLogPort = COM1;

/**
  * @brief  Redirect console output to COM
  */
#if defined(__ICCARM__)
/* New definition from EWARM V9, compatible with EWARM8 */
int iar_fputc(int ch);
#define PUTCHAR_PROTOTYPE int iar_fputc(int ch)

#elif defined (__CC_ARM) || defined(__ARMCC_VERSION)
/* ARM Compiler 5/6 */
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)

#elif defined(__GNUC__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#endif /* __ICCARM__ */

#endif /* USE_COM_LOG */
#endif /* (USE_BSP_COM_FEATURE == 1) */

static ADC_TypeDef      *JOY_ADC[JOYn] = {JOY1_ADC};
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
static uint32_t          IsJoyMspCbValid[JOYn] = {0U};
#endif /* (USE_HAL_ADC_REGISTER_CALLBACKS == 1) */
static uint32_t          CurrentJoyPins[JOYn]  = {0U};
/**
  * @}
  */

/** @defgroup STM32WBA55G_DK1_LOW_LEVEL_Exported_Functions LOW LEVEL Exported Functions
  * @{
  */

/**
  * @brief  This method returns the STM32WBA55G_DK1 BSP Driver revision
  * @retval version: 0xXYZR (8bits for each decimal, R for RC)
  */
int32_t BSP_GetVersion(void)
{
  return (int32_t)STM32WBA55G_DK1_BSP_VERSION;
}

/**
  * @brief  This method returns the board name
  * @retval pointer to the board name string
  */
const uint8_t *BSP_GetBoardName(void)
{
  return (uint8_t *)STM32WBA55G_DK1_BSP_BOARD_NAME;
}

/**
  * @brief  This method returns the board ID
  * @retval pointer to the board name string
  */
const uint8_t *BSP_GetBoardID(void)
{
  return (uint8_t *)STM32WBA55G_DK1_BSP_BOARD_ID;
}

/**
  * @brief  Configure LED GPIO.
  * @param  Led Specifies the Led to be configured.
  *   This parameter can be one of following parameters:
  *     @arg  LD3
  * @retval BSP error code.
  */
int32_t BSP_LED_Init(Led_TypeDef Led)
{
  GPIO_InitTypeDef GPIO_Init;

  /* Enable the GPIO_LED Clock */
  LD3_GPIO_CLK_ENABLE();

  /* configure the GPIO_LED pin */
  GPIO_Init.Pin   = LED_PIN[Led];
  GPIO_Init.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_Init.Pull  = GPIO_PULLUP;
  GPIO_Init.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED_PORT[Led], &GPIO_Init);

  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET);

  return BSP_ERROR_NONE;
}

/**
  * @brief  DeInitialize LEDs.
  * @param  Led LED to be de-init.
  *   This parameter can be one of the following values:
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
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET);
  HAL_GPIO_DeInit(LED_PORT[Led], GPIO_Init.Pin);
  return BSP_ERROR_NONE;
}

/**
  * @brief  Turn selected LED On.
  * @param  Led Specifies the Led to be set on.
  *   This parameter can be one of following parameters:
  *     @arg  LD3
  * @retval BSP error code.
  */
int32_t BSP_LED_On(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);

  return BSP_ERROR_NONE;
}

/**
  * @brief  Turn selected LED Off.
  * @param  Led Specifies the Led to be set off.
  *   This parameter can be one of following parameters:
  *     @arg  LD3
  * @retval BSP error code.
  */
int32_t BSP_LED_Off(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET);

  return BSP_ERROR_NONE;
}

/**
  * @brief  Toggle the selected LED.
  * @param  Led Specifies the Led to be toggled.
  *   This parameter can be one of following parameters:
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
  *     @arg  LD3
  * @retval 0 means off, 1 means on.
  */
int32_t BSP_LED_GetState(Led_TypeDef Led)
{
  int32_t status;

  status = (HAL_GPIO_ReadPin(LED_PORT[Led], LED_PIN[Led]) == GPIO_PIN_SET) ? 1 : 0;

  return status;
}

#if (USE_BSP_COM_FEATURE == 1)
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

#if defined(__ICCARM__)
size_t __write(int file, unsigned char const *ptr, size_t len)
{
  size_t idx;
  unsigned char const *pdata = ptr;

  for (idx = 0; idx < len; idx++)
  {
    iar_fputc((int)*pdata);
    pdata++;
  }
  return len;
}
#endif /* __ICCARM__ */

/**
  * @brief  Redirect console output to COM
  */
PUTCHAR_PROTOTYPE
{
  (void) HAL_UART_Transmit(&hcom_uart[COM_ActiveLogPort], (uint8_t *) &ch, 1, COM_POLL_TIMEOUT);
  return ch;
}
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
#endif /* (USE_BSP_COM_FEATURE == 1) */

/**
  * @brief  Configure Joystick.
  * @param  JOY Joystick to be configured.
  *   This parameter can be JOY1
  * @param  JoyMode Specifies joystick mode.
  *   This parameter can be one of following parameters:
  *     @arg JOY_MODE_POLLING: Joystick pin will be used in polling.
  *     @arg JOY_MODE_IT: Joystick pin will be used in interrupt.
  * @param  JoyPins Specifies joystick pin.
  *   This parameter can be one of following parameters:
  *     @arg JOY_SEL
  *     @arg JOY_DOWN
  *     @arg JOY_LEFT
  *     @arg JOY_RIGHT
  *     @arg JOY_UP
  *     @arg JOY_ALL: all pins
  * @retval BSP error code
  */
int32_t BSP_JOY_Init(JOY_TypeDef JOY, JOYMode_TypeDef JoyMode, JOYPin_TypeDef JoyPins)
{
  int32_t status = BSP_ERROR_NONE;

  /* Initialize JOY instance */
  hjoy_adc[JOY].Instance = JOY_ADC[JOY];

#if (USE_HAL_ADC_REGISTER_CALLBACKS == 0)
  ADC_MspInit(&hjoy_adc[JOY]);
#else /* (USE_HAL_ADC_REGISTER_CALLBACKS == 0) */
  if (IsJoyMspCbValid[JOY] == 0U)
  {
    /* Register the MSP callbacks */
    if (BSP_JOY_RegisterDefaultMspCallbacks(JOY) != BSP_ERROR_NONE)
    {
      status = BSP_ERROR_MSP_FAILURE;
    }
  }
#endif /* (USE_HAL_ADC_REGISTER_CALLBACKS == 0) */

  if (status == BSP_ERROR_NONE)
  {
    MX_ADC_Config_t mxAdcInit;
    mxAdcInit.ContinuousConvMode = (JoyMode == JOY_MODE_IT) ? ENABLE : DISABLE;
    if (MX_ADC4_Init(&hjoy_adc[JOY], &mxAdcInit) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      /* Store current joystick pins */
      CurrentJoyPins[JOY] = JoyPins;

      if (JoyMode == JOY_MODE_IT)
      {
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
        /* Register ADC Analog watchdog callback */
        if (HAL_ADC_RegisterCallback(&hjoy_adc[JOY], HAL_ADC_LEVEL_OUT_OF_WINDOW_1_CB_ID,
                                     ADC_LevelOutOfWindowCallback) != HAL_OK)
        {
          status = BSP_ERROR_PERIPH_FAILURE;
        }
#endif /* (USE_HAL_ADC_REGISTER_CALLBACKS == 1) */
        if (HAL_ADC_Start(&hjoy_adc[JOY]) != HAL_OK)
        {
          status = BSP_ERROR_PERIPH_FAILURE;
        }
      }
    }
  }

  return status;
}

/**
  * @brief  De-initialize Joystick.
  * @param  JOY Joystick to be de-init.
  *   This parameter can be JOY1
  * @param  JoyPins Specifies joystick pin.
  *   This parameter can be one of following parameters:
  *     @arg JOY_SEL
  *     @arg JOY_DOWN
  *     @arg JOY_LEFT
  *     @arg JOY_RIGHT
  *     @arg JOY_UP
  *     @arg JOY_ALL: all pins
  * @retval BSP error code
  */
int32_t BSP_JOY_DeInit(JOY_TypeDef JOY, JOYPin_TypeDef JoyPins)
{
  int32_t status = BSP_ERROR_NONE;

  /* JOY de-init */
  hjoy_adc[JOY].Instance = JOY_ADC[JOY];
  if (HAL_ADC_DeInit(&hjoy_adc[JOY]) != HAL_OK)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 0)
    ADC_MspDeInit(&hjoy_adc[JOY]);
#endif /* (USE_HAL_ADC_REGISTER_CALLBACKS == 0) */
    /* Update current joystick pins */
    CurrentJoyPins[JOY] = 0U;
  }

  return status;
}

/**
  * @brief  Get Joystick state.
  * @param  JOY Joystick.
  *   This parameter can be JOY1
  * @retval BSP error code if value negative or one of following value:
  *     @arg JOY_NONE
  *     @arg JOY_SEL
  *     @arg JOY_DOWN
  *     @arg JOY_LEFT
  *     @arg JOY_RIGHT
  *     @arg JOY_UP
  */
int32_t BSP_JOY_GetState(JOY_TypeDef JOY)
{
  int32_t   retval;
  int32_t   startadc;
  uint16_t  keyconvertedvalue;

  /* No ADC conversion */
  startadc = 0UL;

  /* Check if ADC conversion is ongoing */
  if(LL_ADC_REG_IsConversionOngoing(hjoy_adc[JOY].Instance) == 1UL)
  {
     /* ADC already started */
     startadc = 1UL;
  }
  else
  {
    /* Start the conversion process */
    if (HAL_ADC_Start(&hjoy_adc[JOY]) != HAL_OK)
    {
      retval = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      /* Wait for the end of conversion */
      if (HAL_ADC_PollForConversion(&hjoy_adc[JOY], JOY_ADC_POLL_TIMEOUT) != HAL_OK)
      {
        retval = BSP_ERROR_PERIPH_FAILURE;
      }
      else
      {
        /* ADC started */
        startadc = 1UL;
      }
    }
  }

  /* If ADC is well started */
  if( startadc == 1UL)
  {
    /* Get the converted value of regular channel */
    keyconvertedvalue = HAL_ADC_GetValue(&hjoy_adc[JOY]);

    /* The ADC value gives the pad pressed.
    JOY_NONE  -> around 3,3V  -> ADC value around 4095.
    JOY_DOWN  -> around 2.6V  -> ADC value around 3288.
    JOY_RIGHT -> around 2.0V  -> ADC value around 2494.
    JOY_LEFT  -> around 1.3V  -> ADC value around 1638.
    JOY_UP    -> around 0.6V  -> ADC value around 831.
    JOY_SEL   -> around 0V    -> ADC value around 0. */

    if ((keyconvertedvalue >= 2800U) && (keyconvertedvalue < 3600U))
    {
      retval = (CurrentJoyPins[JOY] & JOY_DOWN) ? JOY_DOWN : JOY_NONE;
    }
    else if ((keyconvertedvalue >= 2000U) && (keyconvertedvalue < 2800U))
    {
      retval = (CurrentJoyPins[JOY] & JOY_RIGHT) ? JOY_RIGHT : JOY_NONE;
    }
    else if ((keyconvertedvalue >= 1200U) && (keyconvertedvalue < 2000U))
    {
      retval = (CurrentJoyPins[JOY] & JOY_LEFT) ? JOY_LEFT : JOY_NONE;
    }
    else if ((keyconvertedvalue >= 400U) && (keyconvertedvalue < 1200U))
    {
      retval = (CurrentJoyPins[JOY] & JOY_UP) ? JOY_UP : JOY_NONE;
    }
    else if (keyconvertedvalue < 400U)
    {
      retval = (CurrentJoyPins[JOY] & JOY_SEL) ? JOY_SEL : JOY_NONE;
    }
    else
    {
      retval = JOY_NONE;
    }
    if(LL_ADC_REG_IsConversionOngoing(hjoy_adc[JOY].Instance) == 1UL)
    {
      /* Wait converted value return inside ADC Threshold to avoid new interrupt */
      do
      {
        keyconvertedvalue = HAL_ADC_GetValue(&hjoy_adc[JOY]);
      }
      while (keyconvertedvalue < 3600U);
    }
  }

  return retval;
}

/**
  * @brief  Initialize AD4.
  * @param  hadc ADC handle.
  * @param  MXInit ADC initialization structure.
  * @retval HAL status.
  */
__weak HAL_StatusTypeDef MX_ADC4_Init(ADC_HandleTypeDef* hadc, MX_ADC_Config_t *MXInit)
{
  HAL_StatusTypeDef status = HAL_ERROR;

  /* ADC Config */
  hadc->Init.ClockPrescaler        = ADC_CLOCK_ASYNC_DIV256;
  hadc->Init.Resolution            = ADC_RESOLUTION_12B;
  hadc->Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  hadc->Init.ScanConvMode          = ADC_SCAN_DISABLE;
  hadc->Init.EOCSelection          = ADC_EOC_SINGLE_CONV;
  hadc->Init.LowPowerAutoWait      = DISABLE;
  hadc->Init.LowPowerAutoPowerOff  = DISABLE;
  hadc->Init.LowPowerAutonomousDPD = ADC_LP_AUTONOMOUS_DPD_DISABLE;
  hadc->Init.ContinuousConvMode    = MXInit->ContinuousConvMode;
  hadc->Init.NbrOfConversion       = 1;
  hadc->Init.DiscontinuousConvMode = DISABLE;
  hadc->Init.ExternalTrigConv      = ADC_SOFTWARE_START;
  hadc->Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc->Init.DMAContinuousRequests = DISABLE;
  hadc->Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;
  hadc->Init.SamplingTimeCommon1   = ADC_SAMPLETIME_814CYCLES_5;
  hadc->Init.SamplingTimeCommon2   = ADC_SAMPLETIME_814CYCLES_5;
  hadc->Init.OversamplingMode      = DISABLE;
  hadc->Init.TriggerFrequencyMode  = ADC_TRIGGER_FREQ_LOW;

  /* Initialize ADC */
  if (HAL_ADC_Init(hadc) == HAL_OK)
  {
    if (HAL_ADCEx_Calibration_Start(hadc) == HAL_OK)
    {
      /* Select the ADC Channel to be converted */
      ADC_ChannelConfTypeDef sConfig;
      sConfig.Channel      = JOY1_ADC_CHANNEL;
      sConfig.Rank         = ADC_REGULAR_RANK_1;
      sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
      /* Return Joystick initialization status */
      status = HAL_ADC_ConfigChannel(hadc, &sConfig);

      if ((status == HAL_OK) && (MXInit->ContinuousConvMode == ENABLE))
      {
        ADC_AnalogWDGConfTypeDef AnalogWDGConfig;
        AnalogWDGConfig.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
        AnalogWDGConfig.WatchdogMode   = ADC_ANALOGWATCHDOG_SINGLE_REG;
        AnalogWDGConfig.Channel        = ADC_CHANNEL_8;
        AnalogWDGConfig.ITMode         = ENABLE;
        AnalogWDGConfig.HighThreshold  = 0xFFFU;
        AnalogWDGConfig.LowThreshold   = 0xE1FU;
        status = HAL_ADC_AnalogWDGConfig(hadc, &AnalogWDGConfig);
      }
    }
  }

  return status;
}

#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
/**
  * @brief  Register default BSP JOY msp callbacks.
  * @param  JOY_TypeDef Joystick Instance.
  * @retval BSP status.
  */
int32_t BSP_JOY_RegisterDefaultMspCallbacks(JOY_TypeDef JOY)
{
  int32_t status = BSP_ERROR_NONE;

  /* Register MspInit/MspDeInit callbacks */
  if (HAL_ADC_RegisterCallback(&hjoy_adc[JOY], HAL_ADC_MSPINIT_CB_ID, ADC_MspInit) != HAL_OK)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  }
  else if (HAL_ADC_RegisterCallback(&hjoy_adc[JOY], HAL_ADC_MSPDEINIT_CB_ID, ADC_MspDeInit) != HAL_OK)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    IsJoyMspCbValid[JOY] = 1U;
  }

  /* Return BSP status */
  return status;
}

/**
  * @brief  Register BSP JOY msp callbacks.
  * @param  JOY_TypeDef Joystick Instance.
  * @retval BSP status.
  */
int32_t BSP_JOY_RegisterMspCallbacks(JOY_TypeDef JOY, BSP_JOY_Cb_t *CallBacks)
{
  int32_t status = BSP_ERROR_NONE;

  if (CallBacks == NULL)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Register MspInit/MspDeInit callbacks */
    if (HAL_ADC_RegisterCallback(&hjoy_adc[JOY], HAL_ADC_MSPINIT_CB_ID, CallBacks->pMspInitCb) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    else if (HAL_ADC_RegisterCallback(&hjoy_adc[JOY], HAL_ADC_MSPDEINIT_CB_ID, CallBacks->pMspDeInitCb) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      IsJoyMspCbValid[JOY] = 1U;
    }
  }
  /* Return BSP status */
  return status;
}
#endif /* (USE_HAL_ADC_REGISTER_CALLBACKS == 1) */

/**
  * @brief  BSP Joystick Callback.
  * @param  JOY Joystick to be de-init.
  *   This parameter can be JOY1
  * @param  JoyPin Specifies joystick pin.
  *   This parameter can be one of following parameters:
  *     @arg JOY_SEL
  *     @arg JOY_DOWN
  *     @arg JOY_LEFT
  *     @arg JOY_RIGHT
  *     @arg JOY_UP
  * @retval None.
  */
__weak void BSP_JOY_Callback(JOY_TypeDef JOY, JOYPin_TypeDef JoyPin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(JOY);
  UNUSED(JoyPin);

  /* This function should be implemented by the user application.
     It is called into this driver when an event on JoyPin is triggered. */
}

/**
  * @brief  BSP JOY interrupt handler.
  * @param  JOY Joystick.
  *   This parameter can be JOY1
  * @param  JoyPin Specifies joystick pin.
  *   This parameter can be one of following parameters:
  *     @arg JOY_SEL
  *     @arg JOY_DOWN
  *     @arg JOY_LEFT
  *     @arg JOY_RIGHT
  *     @arg JOY_UP
  * @retval None.
  */
void BSP_JOY_IRQHandler(JOY_TypeDef JOY, JOYPin_TypeDef JoyPin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(JoyPin);

  HAL_ADC_IRQHandler(&hjoy_adc[JOY]);
}

#if (USE_HAL_ADC_REGISTER_CALLBACKS == 0)
/**
  * @brief  Analog watchdog callback in non blocking mode.
  * @param  hadc: ADC handle
  * @retval None
  */
void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc)
{
  __IO uint16_t  keyconvertedvalue;
  __IO uint32_t  tickvalue = HAL_GetTick();
  uint32_t       joy_tick_debounce = joy_tick + BSP_JOY_DEBOUNCE_DELAY;

  /* Check if ADC interrupt is called twice inside the debounce delay */
  if (joy_tick_debounce < tickvalue)
  {
    /* Retrieve ADC conversion data */
    /* Note: data scale corresponds to ADC resolution */
    keyconvertedvalue = HAL_ADC_GetValue(hadc);

    if ((keyconvertedvalue >= 2800U) && (keyconvertedvalue < 3600U))
    {
      if (CurrentJoyPins[JOY1] & JOY_DOWN)
      {
        BSP_JOY_Callback(JOY1, JOY_DOWN);
      }
    }
    else if ((keyconvertedvalue >= 2000U) && (keyconvertedvalue < 2800U))
    {
      if (CurrentJoyPins[JOY1] & JOY_RIGHT)
      {
        BSP_JOY_Callback(JOY1, JOY_RIGHT);
      }
    }
    else if ((keyconvertedvalue >= 1200U) && (keyconvertedvalue < 2000U))
    {
      if (CurrentJoyPins[JOY1] & JOY_LEFT)
      {
        BSP_JOY_Callback(JOY1, JOY_LEFT);
      }
    }
    else if ((keyconvertedvalue >= 400U) && (keyconvertedvalue < 1200U))
    {
      if (CurrentJoyPins[JOY1] & JOY_UP)
      {
        BSP_JOY_Callback(JOY1, JOY_UP);
      }
    }
    else if (keyconvertedvalue < 400U)
    {
      if (CurrentJoyPins[JOY1] & JOY_SEL)
      {
        BSP_JOY_Callback(JOY1, JOY_SEL);
      }
    }
    else
    {
      /* Nothing to do */
    }
    /* Save Joystick interrupt tick value */
    joy_tick = HAL_GetTick();
  }
}
#endif /* (USE_HAL_ADC_REGISTER_CALLBACKS == 0) */
/**
  * @}
  */

/** @defgroup STM32WBA55G_DK1_LOW_LEVEL_Private_Functions LOW LEVEL Private Functions
  * @{
  */
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
  * @brief  Initialize ADC MSP.
  * @param  hadc ADC handle.
  * @retval None.
  */
static void ADC_MspInit(ADC_HandleTypeDef *hadc)
{
  GPIO_InitTypeDef         GPIO_Init;
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /* Prevent unused argument(s) compilation warning */
  UNUSED(hadc);

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_HCLK;
  (void) HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

  /* Enable ADC clock */
  JOY1_CLK_ENABLE();

  /* Enable GPIO clock */
  JOY1_CHANNEL_GPIO_CLK_ENABLE();

  /* Configure the selected ADC Channel as analog input */
  GPIO_Init.Pin       = JOY1_CHANNEL_GPIO_PIN;
  GPIO_Init.Mode      = GPIO_MODE_ANALOG;
  GPIO_Init.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_Init.Pull      = GPIO_NOPULL;
  HAL_GPIO_Init(JOY1_CHANNEL_GPIO_PORT, &GPIO_Init);

  /* ADC4 interrupt Init */
  HAL_NVIC_SetPriority(ADC4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(ADC4_IRQn);
}

/**
  * @brief  Deinitialize ADC MSP.
  * @param  hadc ADC handle.
  * @retval None.
  */
static void ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hadc);

  /* Disable GPIOs and ADC clock */
  HAL_GPIO_DeInit(JOY1_CHANNEL_GPIO_PORT, JOY1_CHANNEL_GPIO_PIN);
  JOY1_CLK_DISABLE();
}

#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
/**
  * @brief  Analog watchdog 1 callback in non-blocking mode.
  * @param hadc ADC handle
  * @retval None
  */
static void ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef *hadc)
{
 __IO uint16_t  keyconvertedvalue;
  __IO uint32_t  tickvalue = HAL_GetTick();
  uint32_t       joy_tick_debounce = joy_tick + BSP_JOY_DEBOUNCE_DELAY;

  /* Check if ADC interrupt is called twice inside the debounce delay */
  if (joy_tick_debounce < tickvalue)
  {
    /* Retrieve ADC conversion data */
    /* Note: data scale corresponds to ADC resolution */
    keyconvertedvalue = HAL_ADC_GetValue(hadc);

    if ((keyconvertedvalue >= 2800U) && (keyconvertedvalue < 3600U))
    {
      if (CurrentJoyPins[JOY1] & JOY_DOWN)
      {
        BSP_JOY_Callback(JOY1, JOY_DOWN);
      }
    }
    else if ((keyconvertedvalue >= 2000U) && (keyconvertedvalue < 2800U))
    {
      if (CurrentJoyPins[JOY1] & JOY_RIGHT)
      {
        BSP_JOY_Callback(JOY1, JOY_RIGHT);
      }
    }
    else if ((keyconvertedvalue >= 1200U) && (keyconvertedvalue < 2000U))
    {
      if (CurrentJoyPins[JOY1] & JOY_LEFT)
      {
        BSP_JOY_Callback(JOY1, JOY_LEFT);
      }
    }
    else if ((keyconvertedvalue >= 400U) && (keyconvertedvalue < 1200U))
    {
      if (CurrentJoyPins[JOY1] & JOY_UP)
      {
        BSP_JOY_Callback(JOY1, JOY_UP);
      }
    }
    else if (keyconvertedvalue < 400U)
    {
      if (CurrentJoyPins[JOY1] & JOY_SEL)
      {
        BSP_JOY_Callback(JOY1, JOY_SEL);
      }
    }
    else
    {
      /* Nothing to do */
    }
    /* Save Joystick interrupt tick value */
    joy_tick = HAL_GetTick();
  }
}
#endif /* (USE_ADC_SAI_REGISTER_CALLBACKS == 1) */
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
