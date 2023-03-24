/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    linklayer_plat.c
  * @author  MCD Application Team
  * @brief   Source file for the linklayer plateform adaptation layer
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

#include "app_common.h"
#include "stm32wbaxx_hal.h"
#include "linklayer_plat.h"
#include "stm32wbaxx_hal_conf.h"
#include "stm32wbaxx_ll_rcc.h"
#include "app_conf.h"
#include "scm.h"
#include "adc_ctrl.h"

#define max(a,b) ((a) > (b) ? a : b)

/* 2.4GHz RADIO ISR callbacks */
void (*radio_callback)(void) = NULL;
void (*low_isr_callback)(void) = NULL;

/* RNG handle */
extern RNG_HandleTypeDef hrng;
extern void HAL_Generate_Random_Bytes_To( uint8_t* out, uint8_t n );
extern void ll_sys_bg_temperature_measurement(void);

/* Radio critical sections */
volatile int32_t prio_high_isr_counter = 0;
volatile int32_t prio_low_isr_counter = 0;
volatile int32_t prio_sys_isr_counter = 0;
volatile int32_t irq_counter = 0;
volatile uint32_t local_basepri_value = 0;

/* Radio SW low ISR global variable */
volatile uint8_t radio_sw_low_isr_is_running_high_prio = 0;

/**
  * @brief  Configure the necessary clock sources for the radio :
  *         -  HSE32 (bus clk)
  *         -  LSE  (sleep clk)
  *         Enable radio AHB5ENR peripheral bus clock
  * @param  None
  * @retval None
  */
void LINKLAYER_PLAT_ClockInit()
{
  /* Select LSE as Sleep CLK */
  __HAL_RCC_RADIOSLPTIM_CONFIG(RCC_RADIOSTCLKSOURCE_LSE);

  /* Enable AHB5ENR peripheral clock (bus CLK) */
  __HAL_RCC_RADIO_CLK_ENABLE();
}

void LINKLAYER_PLAT_DelayUs(uint32_t delay)
{
__IO register uint32_t Delay = delay * (SystemCoreClock / 1000000U);
	do
	{
		__NOP();
	}
	while (Delay --);
}

void LINKLAYER_PLAT_Assert(uint8_t condition)
{
  assert_param(condition);
}

void LINKLAYER_PLAT_HclkEnable()
{
  __HAL_RCC_RADIO_CLK_ENABLE();
}

void LINKLAYER_PLAT_HclkDisable()
{

}

void LINKLAYER_PLAT_WaitHclkRdy(void)
{
  while(HAL_RCCEx_GetRadioBusClockReadiness() != RCC_RADIO_BUS_CLOCK_READY);
}

void LINKLAYER_PLAT_AclkCtrl(uint8_t enable)
{
  if(enable){
    /* Enable RADIO baseband clock (active CLK) */
    HAL_RCCEx_EnableRadioBBClock();

    /* Polling on HSE32 activation */
    while ( LL_RCC_HSE_IsReady() == 0);
  }
  else
  {
    /* Disable RADIO baseband clock (active CLK) */
    HAL_RCCEx_DisableRadioBBClock();
  }
}

void LINKLAYER_PLAT_GetRNG(uint8_t *ptr_rnd, uint32_t len)
{
  HAL_Generate_Random_Bytes_To( ptr_rnd, (uint8_t)len );
}

void LINKLAYER_PLAT_SetupRadioIT(void (*intr_cb)())
{
  radio_callback = intr_cb;
  HAL_NVIC_SetPriority((IRQn_Type) RADIO_INTR_NUM, RADIO_INTR_PRIO_HIGH, 0);
  HAL_NVIC_EnableIRQ((IRQn_Type) RADIO_INTR_NUM);
}

void LINKLAYER_PLAT_SetupSwLowIT(void (*intr_cb)())
{
  low_isr_callback = intr_cb;

  HAL_NVIC_SetPriority((IRQn_Type) RADIO_SW_LOW_INTR_NUM, RADIO_SW_LOW_INTR_PRIO, 0);
  HAL_NVIC_EnableIRQ((IRQn_Type) RADIO_SW_LOW_INTR_NUM);
}

void LINKLAYER_PLAT_TriggerSwLowIT(uint8_t priority)
{
  uint8_t low_isr_priority = RADIO_INTR_PRIO_LOW;

  /* Check if a SW low interrupt as already been raised.
   * Nested call far radio low isr are not supported
   **/

  if(NVIC_GetActive(RADIO_SW_LOW_INTR_NUM) == 0)
  {
    /* No nested SW low ISR, default behavior */

    if(priority == 0)
    {
      low_isr_priority = RADIO_SW_LOW_INTR_PRIO;
    }

    HAL_NVIC_SetPriority((IRQn_Type) RADIO_SW_LOW_INTR_NUM, low_isr_priority, 0);
  }
  else
  {
    /* Nested call detected */
    /* No change for SW radio low interrupt priority for the moment */

    if(priority != 0)
    {
      /* At the end of current SW radio low ISR, this pending SW low interrupt
       * will run with RADIO_INTR_PRIO_LOW priority
       **/
      radio_sw_low_isr_is_running_high_prio = 1;
    }
  }

  HAL_NVIC_SetPendingIRQ((IRQn_Type) RADIO_SW_LOW_INTR_NUM);
}

void LINKLAYER_PLAT_EnableIRQ(void)
{
  irq_counter = max(0,irq_counter-1);

  if(irq_counter == 0)
  {
    __enable_irq();
  }
}

void LINKLAYER_PLAT_DisableIRQ(void)
{
  __disable_irq();
  irq_counter ++;
}

void LINKLAYER_PLAT_EnableSpecificIRQ(uint8_t isr_type)
{
  if( (isr_type & LL_HIGH_ISR_ONLY) != 0 )
  {
    prio_high_isr_counter--;
    if(prio_high_isr_counter == 0)
    {
      HAL_NVIC_EnableIRQ(RADIO_INTR_NUM);
    }
  }

  if( (isr_type & LL_LOW_ISR_ONLY) != 0 )
  {
    prio_low_isr_counter--;
    if(prio_low_isr_counter == 0)
    {
      HAL_NVIC_EnableIRQ(RADIO_SW_LOW_INTR_NUM);
    }

  }

  if( (isr_type & SYS_LOW_ISR) != 0 )
  {
    prio_sys_isr_counter--;
    if(prio_sys_isr_counter == 0)
    {
      __set_BASEPRI(local_basepri_value);
    }
  }
}

void LINKLAYER_PLAT_DisableSpecificIRQ(uint8_t isr_type)
{
  if( (isr_type & LL_HIGH_ISR_ONLY) != 0 )
  {
    prio_high_isr_counter++;
    if(prio_high_isr_counter == 1)
    {
      HAL_NVIC_DisableIRQ(RADIO_INTR_NUM);
    }
  }

  if( (isr_type & LL_LOW_ISR_ONLY) != 0 )
  {
    prio_low_isr_counter++;
    if(prio_low_isr_counter == 1)
    {
      HAL_NVIC_DisableIRQ(RADIO_SW_LOW_INTR_NUM);
    }
  }

  if( (isr_type & SYS_LOW_ISR) != 0 )
  {
    prio_sys_isr_counter++;
    if(prio_sys_isr_counter == 1)
    {
      local_basepri_value = __get_BASEPRI();
      __set_BASEPRI_MAX(RADIO_INTR_PRIO_LOW<<4);
    }
  }
}

void LINKLAYER_PLAT_EnableRadioIT(void)
{
  HAL_NVIC_EnableIRQ((IRQn_Type) RADIO_INTR_NUM);
}

void LINKLAYER_PLAT_DisableRadioIT(void)
{
  HAL_NVIC_DisableIRQ((IRQn_Type) RADIO_INTR_NUM);
}

void LINKLAYER_PLAT_StartRadioEvt(void)
{
  __HAL_RCC_RADIO_CLK_SLEEP_ENABLE();
  NVIC_SetPriority(RADIO_INTR_NUM, RADIO_INTR_PRIO_HIGH);
  scm_notifyradiostate(SCM_RADIO_ACTIVE);
}

void LINKLAYER_PLAT_StopRadioEvt(void)
{
  __HAL_RCC_RADIO_CLK_SLEEP_DISABLE();
  NVIC_SetPriority(RADIO_INTR_NUM, RADIO_INTR_PRIO_LOW);
  scm_notifyradiostate(SCM_RADIO_NOT_ACTIVE);
}

void LINKLAYER_PLAT_RequestTemperature(void)
{
#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
  ll_sys_bg_temperature_measurement();
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */
}
