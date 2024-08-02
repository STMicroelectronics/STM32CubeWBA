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
#include "ll_intf.h"
#include "app_conf.h"

#if (SUPPORT_AOA_AOD == 1)
static const st_gpio_antsw_t rt_antenna_switch_gpio_table[] =
{
  RF_ANTSW0,
  RF_ANTSW1,
  RF_ANTSW2
};

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
  ble_stat_t status;

  if(state == RF_ANTSW_ENABLE)
  {
    status = ll_intf_set_num_of_antennas(RADIO_NUM_OF_ANTENNAS);
    if(status != SUCCESS)
    {
      /* Specified number of antennas is not supported */
      assert_param(0);
      return;
    }
    RF_CONTROL_AntennaSwitch_Enable();
  }
  else
  {
    RF_CONTROL_AntennaSwitch_Disable();
  }
}

#else /* SUPPORT_AOA_AOD */
void RF_CONTROL_AntennaSwitch(rf_antenna_switch_state_t state)
{
  /* AoA-AoD feature is not supported with this Link Layer configuration */
  assert_param(0);
}
#endif /* SUPPORT_AOA_AOD */
