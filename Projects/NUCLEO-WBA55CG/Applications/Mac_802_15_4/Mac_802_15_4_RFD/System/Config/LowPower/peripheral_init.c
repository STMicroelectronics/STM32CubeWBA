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

/* External variables --------------------------------------------------------*/
#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
extern ADC_HandleTypeDef hadc4;
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */
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

#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
  memset(&hadc4, 0, sizeof(hadc4));
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */
  memset(&hramcfg_SRAM1, 0, sizeof(hramcfg_SRAM1));
  memset(&hrng, 0, sizeof(hrng));

  MX_GPIO_Init();
  MX_RAMCFG_Init();
#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
  MX_ADC4_Init();
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */
  MX_RNG_Init();
  MX_ICACHE_Init();

  /* USER CODE BEGIN MX_STANDBY_EXIT_PERIPHERAL_INIT_2 */

  /* USER CODE END MX_STANDBY_EXIT_PERIPHERAL_INIT_2 */
}
