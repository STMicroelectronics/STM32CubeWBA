/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32wbaxx_hal_timebase_TIM.c
  * @brief   HAL time base based on the hardware TIM.
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "stm32wbaxx_hal.h"
#include "stm32wbaxx_hal_tim.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TIM_CNT_FREQ 1000000U /* Timer frequency counter : 1 MHz */
#define TIM_FREQ     1000U    /* Timer frequency : 1 kHz => to have 1 ms interrupt */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef        htim2;
/* Private function prototypes -----------------------------------------------*/
void TIM2_IRQHandler(void);
#if (USE_HAL_TIM_REGISTER_CALLBACKS == 1U)
void TimeBase_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
#endif
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function configures the TIM2 as a time base source.
  *         The time source is configured  to have 1ms time base with a dedicated
  *         Tick interrupt priority.
  * @note   This function is called  automatically at the beginning of program after
  *         reset by HAL_Init() or at any time when clock is configured, by HAL_RCC_ClockConfig().
  * @param  TickPriority: Tick interrupt priority.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  RCC_ClkInitTypeDef    clkconfig;
  uint32_t              uwTimclock, uwAPB1Prescaler;

  uint32_t              uwPrescalerValue;
  uint32_t              pFLatency;
  HAL_StatusTypeDef     Status;

  /* Enable TIM2 clock */
  __HAL_RCC_TIM2_CLK_ENABLE();

  /* Get clock configuration */
  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

  /* Get APB1 prescaler */
  uwAPB1Prescaler = clkconfig.APB1CLKDivider;
  /* Compute TIM2 clock */
  if (uwAPB1Prescaler == RCC_HCLK_DIV1)
  {
    uwTimclock = HAL_RCC_GetPCLK1Freq();
  }
  else
  {
    uwTimclock = 2UL * HAL_RCC_GetPCLK1Freq();
  }

  /* Compute the prescaler value to have TIM2 counter clock equal to TIM_CNT_FREQ */
  uwPrescalerValue = (uint32_t) ((uwTimclock / TIM_CNT_FREQ) - 1U);

  /* Initialize TIM2 */
  htim2.Instance = TIM2;

  /* Initialize TIMx peripheral as follow:

  + Period = [(TIM_CNT_FREQ/TIM_FREQ) - 1]. to have a (1/TIM_FREQ) s time base.
  + Prescaler = (uwTimclock/TIM_CNT_FREQ - 1) to have a TIM_CNT_FREQ counter clock.
  + ClockDivision = 0
  + Counter direction = Up
  */
  htim2.Init.Period = (TIM_CNT_FREQ / TIM_FREQ) - 1U;
  htim2.Init.Prescaler = uwPrescalerValue;
  htim2.Init.ClockDivision = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;

  Status = HAL_TIM_Base_Init(&htim2);
  if (Status == HAL_OK)
  {
    /* Start the TIM time Base generation in interrupt mode */
    Status = HAL_TIM_Base_Start_IT(&htim2);
    if (Status == HAL_OK)
    {
        if (TickPriority < (1UL << __NVIC_PRIO_BITS))
        {
          /* Enable the TIM2 global Interrupt */
          HAL_NVIC_SetPriority(TIM2_IRQn, TickPriority, 0U);
          uwTickPrio = TickPriority;
      }
      else
      {
        Status = HAL_ERROR;
      }
    }
}
#if (USE_HAL_TIM_REGISTER_CALLBACKS == 1U)
  HAL_TIM_RegisterCallback(&htim2, HAL_TIM_PERIOD_ELAPSED_CB_ID, TimeBase_TIM_PeriodElapsedCallback);
#endif
  /* Enable the TIM2 global Interrupt */
  HAL_NVIC_EnableIRQ(TIM2_IRQn);

  /* Return function Status */
  return Status;

}

/**
  * @brief  Suspend Tick increment.
  * @note   Disable the tick increment by disabling TIM2 update interrupt.
  * @param  None
  * @retval None
  */
void HAL_SuspendTick(void)
{
  /* Disable TIM2 update Interrupt */
  __HAL_TIM_DISABLE_IT(&htim2, TIM_IT_UPDATE);
}

/**
  * @brief  Resume Tick increment.
  * @note   Enable the tick increment by Enabling TIM2 update interrupt.
  * @param  None
  * @retval None
  */
void HAL_ResumeTick(void)
{
  /* Enable TIM2 Update interrupt */
  __HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);
}

