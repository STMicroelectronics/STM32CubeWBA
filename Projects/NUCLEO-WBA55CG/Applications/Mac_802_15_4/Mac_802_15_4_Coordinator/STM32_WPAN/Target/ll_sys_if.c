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
/* For GCC need to optimize all the files*/
#if defined(__GNUC__)
#pragma GCC push_options
#pragma GCC optimize ("Ofast")
#endif

#include "app_common.h"
#include "main.h"
#include "ll_intf.h"
#include "ll_sys.h"
#include "ll_sys_if.h"
#include "stm32_seq.h"
#include "utilities_common.h"
#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
#include "temp_measurement.h"
#endif /* (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1) */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private constants ---------------------------------------------------------*/
/* USER CODE BEGIN PC */

/* USER CODE END PC */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/
/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private functions prototypes-----------------------------------------------*/
extern void llhwc_cmn_sys_configure_ll_ctx(uint8_t b_allow_low_isr, uint8_t b_run_post_evnt_frm_isr);
extern uint8_t  llhwc_cmn_set_temperature_value(uint16_t temperature);
extern void llhwc_cmn_set_temperature_sensor_state(void);
extern uint8_t ll_tx_pwr_if_select_tx_power_mode(uint8_t tx_power_table_id);
#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
static void ll_sys_bg_temperature_measurement_init(void);
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */

/* USER CODE BEGIN PFP */
#if defined(__GNUC__)
#pragma GCC pop_options
#endif
/* USER CODE END PFP */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  Link Layer background process initialization
  * @param  None
  * @retval None
  */
OPTIMIZED void ll_sys_bg_process_init(void)
{
  /* Tasks creation */
  UTIL_SEQ_RegTask(1U << CFG_TASK_LINK_LAYER, UTIL_SEQ_RFU, ll_sys_bg_process);
}

/**
  * @brief  Link Layer background process next iteration scheduling
  * @param  None
  * @retval None
  */
OPTIMIZED void ll_sys_schedule_bg_process(void)
{
  UTIL_SEQ_SetTask(1U << CFG_TASK_LINK_LAYER, CFG_TASK_PRIO_LINK_LAYER);
}

/**
  * @brief  Link Layer background process next iteration scheduling from ISR
  * @param  None
  * @retval None
  */
OPTIMIZED void ll_sys_schedule_bg_process_isr(void)
{
  UTIL_SEQ_SetTask(1U << CFG_TASK_LINK_LAYER, CFG_TASK_PRIO_LINK_LAYER);
}

/**
  * @brief  Link Layer configuration phase before application startup.
  * @param  None
  * @retval None
  */
OPTIMIZED void ll_sys_config_params(void)
{
  /* Configure link layer behavior for low ISR use and next event scheduling method:
   * - SW low ISR is used.
   * - Next event is scheduled from ISR.
   */
  llhwc_cmn_sys_configure_ll_ctx(USE_RADIO_LOW_ISR, NEXT_EVENT_SCHEDULING_FROM_ISR);

#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
  /* Initialize link layer temperature measurement background task */
  ll_sys_bg_temperature_measurement_init();

  /* Link layer IP uses temperature based calibration instead of periodic one */
  llhwc_cmn_set_temperature_sensor_state();
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */

  /* Link Layer power table */
  ll_tx_pwr_if_select_tx_power_mode(CFG_RF_TX_POWER_TABLE_ID);
}

#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
/**
  * @brief  Link Layer temperature request background process initialization
  * @param  None
  * @retval None
  */
OPTIMIZED void ll_sys_bg_temperature_measurement_init(void)
{
  /* Tasks creation */
  UTIL_SEQ_RegTask(1U << CFG_TASK_LINK_LAYER_TEMP_MEAS, UTIL_SEQ_RFU, TEMPMEAS_RequestTemperatureMeasurement);
}

/**
  * @brief  Request backroud task processing for temperature measurement
  * @param  None
  * @retval None
  */
OPTIMIZED void ll_sys_bg_temperature_measurement(void)
{
  UTIL_SEQ_SetTask(1U << CFG_TASK_LINK_LAYER_TEMP_MEAS, CFG_SEQ_PRIO_0);
}

#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */
