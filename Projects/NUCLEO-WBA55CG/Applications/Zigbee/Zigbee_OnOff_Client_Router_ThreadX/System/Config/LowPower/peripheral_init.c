/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    peripheral_init.c
  * @author  MCD Application Team
  * @brief   tbd module
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

/* Includes ------------------------------------------------------------------*/
#include "app_conf.h"
#include "peripheral_init.h"
#include "main.h"

/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_bsp.h"

/* USER CODE END Includes */

/* External variables --------------------------------------------------------*/
extern RAMCFG_HandleTypeDef hramcfg_SRAM1;
extern RNG_HandleTypeDef hrng;

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
  HAL_StatusTypeDef hal_status;
  /* USER CODE BEGIN MX_STANDBY_EXIT_PERIPHERAL_INIT_1 */

  /* USER CODE END MX_STANDBY_EXIT_PERIPHERAL_INIT_1 */

  /* Select SysTick source clock */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_LSE);

  /* Re-Initialize Tick with new clock source */
  hal_status = HAL_InitTick(TICK_INT_PRIORITY);
  if (hal_status != HAL_OK)
  {
    assert_param(0);
  }

  memset(&hramcfg_SRAM1, 0, sizeof(hramcfg_SRAM1));
  memset(&hrng, 0, sizeof(hrng));

  MX_GPIO_Init();
  MX_ICACHE_Init();
  MX_RAMCFG_Init();
  MX_RNG_Init();

#if (CFG_DEBUGGER_LEVEL == 0)
  GPIO_InitTypeDef DbgIOsInit = {0};
  DbgIOsInit.Mode = GPIO_MODE_ANALOG;
  DbgIOsInit.Pull = GPIO_NOPULL;
  DbgIOsInit.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  __HAL_RCC_GPIOA_CLK_ENABLE();
  HAL_GPIO_Init(GPIOA, &DbgIOsInit);

  DbgIOsInit.Mode = GPIO_MODE_ANALOG;
  DbgIOsInit.Pull = GPIO_NOPULL;
  DbgIOsInit.Pin = GPIO_PIN_3|GPIO_PIN_4;
  __HAL_RCC_GPIOB_CLK_ENABLE();
  HAL_GPIO_Init(GPIOB, &DbgIOsInit);
#endif /* CFG_DEBUGGER_LEVEL */
  /* USER CODE BEGIN MX_STANDBY_EXIT_PERIPHERAL_INIT_2 */
  APP_BSP_StandbyExit();

  /* USER CODE END MX_STANDBY_EXIT_PERIPHERAL_INIT_2 */
}
