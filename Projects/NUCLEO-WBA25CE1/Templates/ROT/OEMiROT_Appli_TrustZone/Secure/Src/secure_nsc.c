/**
  ******************************************************************************
  * @file    Secure/Src/secure_nsc.c
  * @author  MCD Application Team
  * @brief   This file contains the non-secure callable APIs (secure world)
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "secure_nsc.h"
#include <stdio.h>

/** @addtogroup STM32WBAxx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Global variables ----------------------------------------------------------*/
void *pSecureFaultCallback = NULL;   /* Pointer to secure fault callback in Non-secure */
void *pSecureErrorCallback = NULL;   /* Pointer to secure error callback in Non-secure */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Secure registration of non-secure callback.
  * @param  CallbackId  callback identifier
  * @param  func        pointer to non-secure function
  * @retval None
  */
CMSE_NS_ENTRY void SECURE_RegisterCallback(SECURE_CallbackIDTypeDef CallbackId, void *func)
{
  if(func != NULL)
  {
    switch(CallbackId)
    {
      case SECURE_FAULT_CB_ID:           /* SecureFault Interrupt occurred */
        pSecureFaultCallback = func;
        break;
      case GTZC_ERROR_CB_ID:             /* GTZC Interrupt occurred */
        pSecureErrorCallback = func;
        break;
      default:
        /* unknown */
        break;
    }
  }
}

/**
  * @brief  Secure callable function to resume SysTick.
  * @note   This function resumes the SysTick timer operation in the non-secure world.
  * @retval None
  */
CMSE_NS_ENTRY void SECURE_ResumeTick(void)
{
  HAL_ResumeTick();
}

/**
  * @brief  Secure callable function to create a delay.
  * @param  delay  Duration of the delay in milliseconds.
  * @note   This function uses the SysTick timer to create a delay in the non-secure world.
  * @retval None
  */
CMSE_NS_ENTRY void SECURE_Delay(uint32_t delay)
{
  HAL_Delay(delay);
}

#if defined(OEMIROT_OTFDEC_EXTERNAL_FLASH_ENABLE)
/**
  * @brief  Secure callable function to initialize the GPIO for the LED.
  * @note   This function enables the GPIO clock, configures the LED pin
  *         as a high-speed push-pull output with no pull-up/pull-down,
  *         and sets its initial output level to high.
  * @retval None
  */
CMSE_NS_ENTRY void SECURE_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LED_CLK_ENABLE();

  /*Configure GPIO pin : LED1_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}
#endif /* OEMIROT_OTFDEC_EXTERNAL_FLASH_ENABLE */

/**
  * @}
  */

/**
  * @}
  */
