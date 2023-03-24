/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ll_sys_if.c
  * @author  MCD Application Team
  * @brief   Source file for initiating system
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
#include "main.h"
#include "ll_intf.h"
#include "ll_sys.h"
#include "stm32_seq.h"
#include "adc_ctrl.h"
#include "linklayer_plat.h"
/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/

/* USER CODE BEGIN GV */

/* USER CODE END GV */

#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
void ll_sys_bg_temperature_measurement(void);
static void ll_sys_bg_temperature_measurement_init(void);
static void request_temperature_measurement(void);
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */

/**
  * @brief  Link Layer background process initialization
  * @param  None
  * @retval None
  */
void ll_sys_bg_process_init(void)
{
  /* Tasks creation */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_LINK_LAYER, UTIL_SEQ_RFU, ll_sys_bg_process);
}

/**
  * @brief  Link Layer background process next iteration scheduling
  * @param  None
  * @retval None
  */
void ll_sys_schedule_bg_process(void)
{
  UTIL_SEQ_SetTask(1U << CFG_TASK_LINK_LAYER, CFG_SCH_PRIO_0);
}

void ll_sys_config_params(void)
{
  ll_intf_config_ll_ctx_params(USE_RADIO_LOW_ISR, NEXT_EVENT_SCHEDULING_FROM_ISR);
#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
  ll_sys_bg_temperature_measurement_init();
  ll_intf_set_temperature_sensor_state();
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */
}

#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
/**
  * @brief  Link Layer temperature request background process initialization
  * @param  None
  * @retval None
  */
void ll_sys_bg_temperature_measurement_init(void)
{
  /* Tasks creation */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_LINK_LAYER_TEMP_MEAS, UTIL_SEQ_RFU, request_temperature_measurement);
}

void ll_sys_bg_temperature_measurement(void)
{
  UTIL_SEQ_SetTask(1U << CFG_TASK_LINK_LAYER_TEMP_MEAS, CFG_SCH_PRIO_0);
}

void request_temperature_measurement(void)
{
  int16_t temperature_value = 0;

  UTILS_ENTER_LIMITED_CRITICAL_SECTION(RCC_INTR_PRIO<<4);
  adc_ctrl_req(SYS_ADC_LL_EVT, ADC_ON);
  temperature_value = adc_ctrl_request_temperature();
  adc_ctrl_req(SYS_ADC_LL_EVT, ADC_OFF);
  ll_intf_set_temperature_value(temperature_value);
  UTILS_EXIT_LIMITED_CRITICAL_SECTION();
}

#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */

void LINKLAYER_PLAT_EnableOSContextSwitch(void)
{

}

void LINKLAYER_PLAT_DisableOSContextSwitch(void)
{

}
