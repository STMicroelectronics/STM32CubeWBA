/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    peripheral_init.c
  * @author  MCD Application Team
  * @brief   tbd module
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

#include "peripheral_init.h"
#include "main.h"

 /**
  * @brief  Configure the CPU NVIC peripheral at Standby mode exit.
  * @param  None
  * @retval None
  */
void MX_StandbyExit_NVICPeripharalInit(void)
{
    HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(MemoryManagement_IRQn);
    HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(BusFault_IRQn);
    HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(UsageFault_IRQn);
    HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SVCall_IRQn);
    HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DebugMonitor_IRQn);
    HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(PendSV_IRQn);
    HAL_NVIC_SetPriority(SysTick_IRQn, 14, 0);
    HAL_NVIC_EnableIRQ(SysTick_IRQn);
    HAL_NVIC_SetPriority(RCC_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(RCC_IRQn);
    HAL_NVIC_SetPriority(RADIO_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(RADIO_IRQn);
}

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef handle_GPDMA1_Channel3;
extern DMA_HandleTypeDef handle_GPDMA1_Channel2;
extern UART_HandleTypeDef hlpuart1;
extern RAMCFG_HandleTypeDef hramcfg_SRAM1;
extern RNG_HandleTypeDef hrng;
/**
  * @brief  Configure the SoC peripherals at Standby mode exit.
  * @param  None
  * @retval None
  */
void MX_StandbyExit_PeripharalInit(void)
{
  /* USER CODE BEGIN MX_STANDBY_EXIT_PERIPHERAL_INIT_1 */

  /* USER CODE END MX_STANDBY_EXIT_PERIPHERAL_INIT_1 */

  memset(&handle_GPDMA1_Channel3, 0, sizeof(handle_GPDMA1_Channel3));
  memset(&handle_GPDMA1_Channel2, 0, sizeof(handle_GPDMA1_Channel2));
  memset(&hlpuart1, 0, sizeof(hlpuart1));
  memset(&hramcfg_SRAM1, 0, sizeof(hramcfg_SRAM1));
  memset(&hrng, 0, sizeof(hrng));

  MX_GPIO_Init();
  MX_ICACHE_Init();
  MX_RAMCFG_Init();
  MX_LPUART1_UART_Init();
  MX_RNG_Init();

  /* USER CODE BEGIN MX_STANDBY_EXIT_PERIPHERAL_INIT_2 */

  /* USER CODE END MX_STANDBY_EXIT_PERIPHERAL_INIT_2 */
}