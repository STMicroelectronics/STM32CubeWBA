/**
  ******************************************************************************
  * @file    ll_sys_hclk_client.c
  * @author  MCD Application Team
  * @brief   Link Layer IP system interface blus clock client
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

#include "linklayer_plat.h"
#include "ll_sys.h"
#include <stdint.h>

/* radio bus clock client list */
static uint32_t ll_sys_radio_hclk_client_list = 0;

/**
  * @brief  Initialize the bus clock client list
  * @param  None
  * @retval None
  */
void ll_sys_radio_hclk_ctrl_init(void)
{
  ll_sys_radio_hclk_client_list = 0;
}

/**
  * @brief  Manage radio bus clock enable/disable requests
  * @param  client     client that requests a bus clock change
  * @param  enable     enable or disable request
  * @retval None
  */
void ll_sys_radio_hclk_ctrl_req(ll_sys_radio_hclk_client_t client, ll_sys_radio_hclk_state_t enable)
{
  
  ll_sys_disable_irq();
  
  if(enable == LL_SYS_RADIO_HCLK_OFF)
  {
  ll_sys_radio_hclk_client_list &= (~(enable << client));
  }
  
  else
  {
    ll_sys_radio_hclk_client_list |= (enable << client);
  }
  
  if(ll_sys_radio_hclk_client_list == 0)
  {
    /* Disable radio clock as there is no request anymore */
    LINKLAYER_PLAT_HclkDisable();
  } 
  
  else{
    /* Enable radio clock as there at least one request */
    LINKLAYER_PLAT_HclkEnable();
  }
  
  ll_sys_enable_irq();
}