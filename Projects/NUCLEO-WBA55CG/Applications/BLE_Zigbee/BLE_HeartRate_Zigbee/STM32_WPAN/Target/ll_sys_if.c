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

#include "main.h"
#include "app_common.h"
#include "app_conf.h"
#include "log_module.h"
#include "ll_intf_cmn.h"
#include "ll_sys.h"
#include "ll_sys_if.h"
#include "platform.h"
#include "stm32_rtos.h"
#include "utilities_common.h"

/* Private defines -----------------------------------------------------------*/
/* Radio event scheduling method - must be set at 1 */
#define USE_RADIO_LOW_ISR                   (1)
#define NEXT_EVENT_SCHEDULING_FROM_ISR      (1)

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
static void ll_sys_sleep_clock_source_selection(void);
static uint8_t ll_sys_BLE_sleep_clock_accuracy_selection(void);
void ll_sys_reset(void);

/* USER CODE BEGIN PFP */

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
void ll_sys_bg_process_init(void)
{
  /* Register Link Layer task */
  UTIL_SEQ_RegTask(1U << CFG_TASK_LINK_LAYER, UTIL_SEQ_RFU, ll_sys_bg_process);
}

/**
  * @brief  Link Layer background process next iteration scheduling
  * @param  None
  * @retval None
  */
void ll_sys_schedule_bg_process(void)
{
  UTIL_SEQ_SetTask(1U << CFG_TASK_LINK_LAYER, TASK_PRIO_LINK_LAYER);
}

/**
  * @brief  Link Layer background process next iteration scheduling from ISR
  * @param  None
  * @retval None
  */
void ll_sys_schedule_bg_process_isr(void)
{
  UTIL_SEQ_SetTask(1U << CFG_TASK_LINK_LAYER, TASK_PRIO_LINK_LAYER);
}

/**
  * @brief  Link Layer configuration phase before application startup.
  * @param  None
  * @retval None
  */
void ll_sys_config_params(void)
{
/* USER CODE BEGIN ll_sys_config_params_0 */

/* USER CODE END ll_sys_config_params_0 */

  /* Configure link layer behavior for low ISR use and next event scheduling method:
   * - SW low ISR is used.
   * - Next event is scheduled from ISR.
   */
  ll_intf_cmn_config_ll_ctx_params(USE_RADIO_LOW_ISR, NEXT_EVENT_SCHEDULING_FROM_ISR);
  /* Apply the selected link layer sleep timer source */
  ll_sys_sleep_clock_source_selection();

/* USER CODE BEGIN ll_sys_config_params_1 */

/* USER CODE END ll_sys_config_params_1 */

  /* Link Layer power table */
  ll_intf_cmn_select_tx_power_table(CFG_RF_TX_POWER_TABLE_ID);
/* USER CODE BEGIN ll_sys_config_params_2 */

/* USER CODE END ll_sys_config_params_2 */
}

uint8_t ll_sys_BLE_sleep_clock_accuracy_selection(void)
{
  uint8_t BLE_sleep_clock_accuracy = 0;
#if (CFG_RADIO_LSE_SLEEP_TIMER_CUSTOM_SCA_RANGE == 0)
  uint32_t RevID = LL_DBGMCU_GetRevisionID();
#endif
  uint32_t linklayer_slp_clk_src = LL_RCC_RADIO_GetSleepTimerClockSource();

  if(linklayer_slp_clk_src == LL_RCC_RADIOSLEEPSOURCE_LSE)
  {
    /* LSE selected as Link Layer sleep clock source.
       Sleep clock accuracy is different regarding the WBA device ID and revision
     */
#if (CFG_RADIO_LSE_SLEEP_TIMER_CUSTOM_SCA_RANGE == 0)
#if defined(STM32WBA52xx) || defined(STM32WBA54xx) || defined(STM32WBA55xx)
    if(RevID == REV_ID_A)
    {
      BLE_sleep_clock_accuracy = STM32WBA5x_REV_ID_A_SCA_RANGE;
    }
    else if(RevID == REV_ID_B)
    {
      BLE_sleep_clock_accuracy = STM32WBA5x_REV_ID_B_SCA_RANGE;
    }
    else
    {
      /* Revision ID not supported, default value of 500ppm applied */
      BLE_sleep_clock_accuracy = STM32WBA5x_DEFAULT_SCA_RANGE;
    }
#elif defined(STM32WBA65xx)
    BLE_sleep_clock_accuracy = STM32WBA6x_SCA_RANGE;
    UNUSED(RevID);
#else
    UNUSED(RevID);
#endif /* defined(STM32WBA52xx) || defined(STM32WBA54xx) || defined(STM32WBA55xx) */
#else /* CFG_RADIO_LSE_SLEEP_TIMER_CUSTOM_SCA_RANGE */
    BLE_sleep_clock_accuracy = CFG_RADIO_LSE_SLEEP_TIMER_CUSTOM_SCA_RANGE;
#endif /* CFG_RADIO_LSE_SLEEP_TIMER_CUSTOM_SCA_RANGE */
  }
  else
  {
    /* LSE is not the Link Layer sleep clock source, sleep clock accurcay default value is 500 ppm */
    BLE_sleep_clock_accuracy = STM32WBA5x_DEFAULT_SCA_RANGE;
  }

  return BLE_sleep_clock_accuracy;
}

void ll_sys_sleep_clock_source_selection(void)
{
  uint16_t freq_value = 0;
  uint32_t linklayer_slp_clk_src = LL_RCC_RADIOSLEEPSOURCE_NONE;

  linklayer_slp_clk_src = LL_RCC_RADIO_GetSleepTimerClockSource();
  switch(linklayer_slp_clk_src)
  {
    case LL_RCC_RADIOSLEEPSOURCE_LSE:
      linklayer_slp_clk_src = RTC_SLPTMR;
      break;

    case LL_RCC_RADIOSLEEPSOURCE_LSI:
      linklayer_slp_clk_src = RCO_SLPTMR;
      break;

    case LL_RCC_RADIOSLEEPSOURCE_HSE_DIV1000:
      linklayer_slp_clk_src = CRYSTAL_OSCILLATOR_SLPTMR;
      break;

    case LL_RCC_RADIOSLEEPSOURCE_NONE:
      /* No Link Layer sleep clock source selected */
      assert_param(0);
      break;
  }
  ll_intf_cmn_le_select_slp_clk_src((uint8_t)linklayer_slp_clk_src, &freq_value);
}

void ll_sys_reset(void)
{
  uint8_t bsca = 0;
  /* Link layer timings */
  uint8_t drift_time = DRIFT_TIME_DEFAULT;
  uint8_t exec_time = EXEC_TIME_DEFAULT;

/* USER CODE BEGIN ll_sys_reset_0 */

/* USER CODE END ll_sys_reset_0 */

  /* Apply the selected link layer sleep timer source */
  ll_sys_sleep_clock_source_selection();

  /* Configure the link layer sleep clock accuracy */
  bsca = ll_sys_BLE_sleep_clock_accuracy_selection();
  ll_intf_le_set_sleep_clock_accuracy(bsca);

  /* Update link layer timings depending on selected configuration */
  if(LL_RCC_RADIO_GetSleepTimerClockSource() == LL_RCC_RADIOSLEEPSOURCE_LSI)
  {
    drift_time += DRIFT_TIME_EXTRA_LSI2;
    exec_time += EXEC_TIME_EXTRA_LSI2;
  }
  else
  {
#if defined(__GNUC__) && defined(DEBUG)
    drift_time += DRIFT_TIME_EXTRA_GCC_DEBUG;
    exec_time += EXEC_TIME_EXTRA_GCC_DEBUG;
#endif
  }

  /* USER CODE BEGIN ll_sys_reset_1 */

  /* USER CODE END ll_sys_reset_1 */

  if((drift_time != DRIFT_TIME_DEFAULT) || (exec_time != EXEC_TIME_DEFAULT))
  {
    ll_sys_config_BLE_schldr_timings(drift_time, exec_time);
  }
  /* USER CODE BEGIN ll_sys_reset_2 */

  /* USER CODE END ll_sys_reset_2 */
}

void ll_sys_set_rtl_polling_time(uint8_t rtl_polling_time)
{
  /* first parameter otInstance *aInstance is unused */
  radio_set_rtl_polling_time(NULL, rtl_polling_time);
}
