/**
  ******************************************************************************
  * @file    stm32wbaxx_it.c
  * @author  MCD Application Team
  * @brief   Main Interrupt Service Routines in secure.
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
#include "stm32wbaxx_it.h"

/** @addtogroup STM32WBAxx_HAL_Examples
  * @{
  */

/** @addtogroup Templates_TZ
  * @{
  */

extern funcptr_NS pSecureFaultCallback;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M33 Processor Exceptions Handlers                        */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Secure Fault exception.
  * @param  None
  * @retval None
  */
void SecureFault_Handler(void)
{
  /* USER CODE BEGIN SecureFault_IRQn 0 */
   funcptr_NS callback_NS; /* non-secure callback function pointer */

  if (pSecureFaultCallback != (funcptr_NS)NULL)
  {
    /* return function pointer with cleared LSB */
    callback_NS = (funcptr_NS)cmse_nsfptr_create(pSecureFaultCallback);

    callback_NS();
  }
  else
  {

    while (1)
    {
      /* USER CODE BEGIN W1_SecureFault_IRQn 0 */
      /* USER CODE END W1_SecureFault_IRQn 0 */
    }
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}

/******************************************************************************/
/*                 STM32WBAxx Peripherals Interrupt Handlers                  */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32wbaxx.s).                                              */
/******************************************************************************/

/**
  * @brief  This function handles GTZC interrupt request.
  * @retval None
  */
void GTZC_IRQHandler(void)
{
  HAL_GTZC_IRQHandler();
}

/**
  * @}
  */

/**
  * @}
  */
