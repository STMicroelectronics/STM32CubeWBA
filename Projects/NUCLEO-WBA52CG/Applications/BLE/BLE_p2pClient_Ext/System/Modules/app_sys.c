/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_sys.c
  * @author  MCD Application Team
  * @brief   Application system for STM32WPAN Middleware.
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

#include "app_sys.h"
#include "app_conf.h"
#include "timer_if.h"
#include "stm32_lpm.h"
#include "stm32_seq.h"
#include "ll_intf.h"
#include "ll_sys.h"

extern uint8_t is_Radio_DeepSleep;

void APP_SYS_BLE_EnterDeepSleep(void)
{
  ble_stat_t cmd_status = GENERAL_FAILURE;
  uint32_t radio_remaining_time = 0;

  if (is_Radio_DeepSleep == 0U)
  {
    /* Enable radio to retrieve next radio activity */
    ll_sys_radio_hclk_ctrl_req(LL_SYS_RADIO_HCLK_PREIDLE, LL_SYS_RADIO_HCLK_ON);

    /* Getting next radio event time if any */
    cmd_status = ll_intf_le_get_remaining_time_for_next_event(&radio_remaining_time);
    assert_param(cmd_status == SUCCESS);

    if (radio_remaining_time == LL_DP_SLP_NO_WAKEUP)
    {
      /* No next radio event scheduled */
      (void)ll_sys_dp_slp_enter(LL_DP_SLP_NO_WAKEUP);
    }
    else if (radio_remaining_time > RADIO_DEEPSLEEP_WAKEUP_TIME_US)
    {
      /* No event in a "near" futur */
      (void)ll_sys_dp_slp_enter(radio_remaining_time - RADIO_DEEPSLEEP_WAKEUP_TIME_US);
    }
    ll_sys_radio_hclk_ctrl_req(LL_SYS_RADIO_HCLK_PREIDLE, LL_SYS_RADIO_HCLK_OFF);
  }
}
