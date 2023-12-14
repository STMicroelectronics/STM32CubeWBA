/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rf_antenna_switch.c
  * @author  MCD Application Team
  * @brief   RF related module to handle dedictated GPIOs for antenna switch
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
/* USER CODE END Header */

#include "rf_antenna_switch.h"

static void RF_CONTROL_AntennaSwitch_Enable(void);
static void RF_CONTROL_AntennaSwitch_Disable(void);

static void RF_CONTROL_AntennaSwitch_Enable(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  uint32_t table_size = sizeof(rt_antenna_switch_gpio_table)/sizeof(rt_antenna_switch_gpio_table[0]);

  for(unsigned int cpt = 0; cpt<table_size; cpt++)
  {
    GPIO_InitStruct.Pin    = rt_antenna_switch_gpio_table[cpt].GPIO_pin;
    GPIO_InitStruct.Mode   = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull   = GPIO_NOPULL;
    GPIO_InitStruct.Speed  = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = rt_antenna_switch_gpio_table[cpt].GPIO_alternate;
    HAL_GPIO_Init(rt_antenna_switch_gpio_table[cpt].GPIO_port, &GPIO_InitStruct);
  }
}

static void RF_CONTROL_AntennaSwitch_Disable(void)
{
  uint32_t table_size = sizeof(rt_antenna_switch_gpio_table)/sizeof(rt_antenna_switch_gpio_table[0]);

  for(unsigned int cpt = 0; cpt<table_size; cpt++)
  {
    HAL_GPIO_DeInit(rt_antenna_switch_gpio_table[cpt].GPIO_port, rt_antenna_switch_gpio_table[cpt].GPIO_pin);
  }
}

void RF_CONTROL_AntennaSwitch(rf_antenna_switch_state_t state)
{
  if(state == RF_ANTSW_ENABLE)
  {
    RF_CONTROL_AntennaSwitch_Enable();
  }
  else
  {
    RF_CONTROL_AntennaSwitch_Disable();
  }
}
