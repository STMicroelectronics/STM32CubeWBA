/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    peripheral_init.c
  * @author  MCD Application Team
  * @brief   peripheral init module
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
#include "app_conf.h"
#include "peripheral_init.h"
#include "main.h"
#include "crc_ctrl.h"
#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
#include "adc_ctrl.h"
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */
#if (CFG_LPM_WAKEUP_TIME_PROFILING == 1)
#include "stm32_lpm_if.h"
#endif /* CFG_LPM_WAKEUP_TIME_PROFILING */
/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_bsp.h"
/* USER CODE END Includes */

/* External variables --------------------------------------------------------*/
extern RAMCFG_HandleTypeDef hramcfg_SRAM1;
extern DMA_HandleTypeDef handle_GPDMA1_Channel1;
extern DMA_HandleTypeDef handle_GPDMA1_Channel0;
extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  Configure the SoC peripherals at Standby mode exit.
  * @param  None
  * @retval None
  */
void MX_StandbyExit_PeripheralInit(void)
{
  /* USER CODE BEGIN MX_STANDBY_EXIT_PERIPHERAL_INIT_1 */

  /* USER CODE END MX_STANDBY_EXIT_PERIPHERAL_INIT_1 */

#if (CFG_LPM_WAKEUP_TIME_PROFILING == 1)
#if (CFG_LPM_STDBY_SUPPORTED == 1)
  /* Do not configure sysTick if currently used by wakeup time profiling mechanism */
  if(LPM_is_wakeup_time_profiling_done() != 0)
  {
    /* Select SysTick source clock */
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_LSE);

    /* Initialize SysTick */
    if (HAL_InitTick(TICK_INT_PRIORITY) != HAL_OK)
    {
      assert_param(0);
    }
  }
#endif /* CFG_LPM_STDBY_SUPPORTED */
#else
  /* Select SysTick source clock */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_LSE);

  /* Initialize SysTick */
  if (HAL_InitTick(TICK_INT_PRIORITY) != HAL_OK)
  {
    assert_param(0);
  }
#endif /* CFG_LPM_WAKEUP_TIME_PROFILING */

#if (CFG_DEBUGGER_LEVEL == 0)
    /* Setup GPIOA 13, 14, 15 in Analog no pull */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIOA->PUPDR &= ~0xFC000000;
    GPIOA->MODER |= 0xFC000000;
    __HAL_RCC_GPIOA_CLK_DISABLE();

    /* Setup GPIOB 3, 4 in Analog no pull */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIOB->PUPDR &= ~0x3C0;
    GPIOB->MODER |= 0x3C0;
    __HAL_RCC_GPIOB_CLK_DISABLE();
#endif /* CFG_DEBUGGER_LEVEL */

  memset(&hramcfg_SRAM1, 0, sizeof(hramcfg_SRAM1));
  memset(&handle_GPDMA1_Channel1, 0, sizeof(handle_GPDMA1_Channel1));
  memset(&handle_GPDMA1_Channel0, 0, sizeof(handle_GPDMA1_Channel0));
#if (CFG_LOG_SUPPORTED == 1)
  memset(&huart1, 0, sizeof(huart1));
#endif

  MX_GPIO_Init();
  MX_GPDMA1_Init();
  MX_RAMCFG_Init();
#if (CFG_LOG_SUPPORTED == 1)
  MX_USART1_UART_Init();
#endif
  MX_ICACHE_Init();
  CRCCTRL_Init();
#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
  ADCCTRL_Init();
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */
  /* USER CODE BEGIN MX_STANDBY_EXIT_PERIPHERAL_INIT_2 */
  APP_BSP_StandbyExit();

  /* USER CODE END MX_STANDBY_EXIT_PERIPHERAL_INIT_2 */
}

