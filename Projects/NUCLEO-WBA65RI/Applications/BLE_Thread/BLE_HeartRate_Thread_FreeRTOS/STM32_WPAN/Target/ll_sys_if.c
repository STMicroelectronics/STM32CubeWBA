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
#include "stm32_rtos.h"
#include "utilities_common.h"
#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
#include "temp_measurement.h"
#endif /* (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1) */

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
/* FreeRTOS objects declaration */

static osThreadId_t     LinkLayerTaskHandle;
static osSemaphoreId_t  LinkLayerSemaphore;

const osThreadAttr_t LinkLayerTask_attributes = {
  .name         = "Link Layer Task",
  .priority     = CFG_TASK_PRIO_LINK_LAYER,
  .stack_size   = TASK_LINK_LAYER_STACK_SIZE,
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE,
  .stack_mem    = TASK_DEFAULT_STACK_MEM
};

const osSemaphoreAttr_t LinkLayerSemaphore_attributes = {
  .name         = "Link Layer Semaphore",
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE
};

#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
static osThreadId_t     TempMeasLLTaskHandle;
static osSemaphoreId_t  TempMeasLLSemaphore;

const osThreadAttr_t TempMeasLLTask_attributes = {
  .name         = "Temperature Measurement LL Task",
  .priority     = LINK_LAYER_TEMP_MEAS_TASK_PRIO,
  .stack_size   = TASK_LINK_LAYER_TEMP_MEAS_STACK_SIZE,
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE,
  .stack_mem    = TASK_DEFAULT_STACK_MEM
};

const osSemaphoreAttr_t TempMeasLLSemaphore_attributes = {
  .name         = "Temperature Measurement LL Semaphore",
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE
};
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/

osMutexId_t             LinkLayerMutex;

const osMutexAttr_t LinkLayerMutex_attributes = {
  .name         = "Link Layer Mutex",
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE
};

/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private functions prototypes-----------------------------------------------*/
#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
static void ll_sys_bg_temperature_measurement_init(void);
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */
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
 * @brief  Link Layer Task for FreeRTOS
 * @param  void *argument
 * @retval None
 */
static void LinkLayer_Task_Entry(void *argument)
{
  UNUSED(argument);

  for(;;)
  {
    osSemaphoreAcquire(LinkLayerSemaphore, osWaitForever);
    osMutexAcquire(LinkLayerMutex, osWaitForever);
    ll_sys_bg_process();
    osMutexRelease(LinkLayerMutex);
  }
}

/**
  * @brief  Link Layer background process initialization
  * @param  None
  * @retval None
  */
void ll_sys_bg_process_init(void)
{
  /* Create Link Layer FreeRTOS objects */

  LinkLayerSemaphore = osSemaphoreNew(1U, 0U, &LinkLayerSemaphore_attributes);

  LinkLayerTaskHandle = osThreadNew(LinkLayer_Task_Entry, NULL, &LinkLayerTask_attributes);

  LinkLayerMutex = osMutexNew(&LinkLayerMutex_attributes);

  if ((LinkLayerTaskHandle == NULL) || (LinkLayerSemaphore == NULL) || (LinkLayerMutex == NULL))
  {
    LOG_ERROR_APP( "Link Layer FreeRTOS objects creation FAILED");
    Error_Handler();
  }
}

/**
  * @brief  Link Layer background process next iteration scheduling
  * @param  None
  * @retval None
  */
void ll_sys_schedule_bg_process(void)
{
  osSemaphoreRelease(LinkLayerSemaphore);
}

/**
  * @brief  Link Layer background process next iteration scheduling from ISR
  * @param  None
  * @retval None
  */
void ll_sys_schedule_bg_process_isr(void)
{
  osSemaphoreRelease(LinkLayerSemaphore);
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

#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
  /* Initialize link layer temperature measurement background task */
  ll_sys_bg_temperature_measurement_init();

  /* Link layer IP uses temperature based calibration instead of periodic one */
  ll_intf_cmn_set_temperature_sensor_state();
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */

  /* Link Layer power table */
  ll_intf_cmn_select_tx_power_table(CFG_RF_TX_POWER_TABLE_ID);
/* USER CODE BEGIN ll_sys_config_params_2 */

/* USER CODE END ll_sys_config_params_2 */
}

#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
/**
 * @brief  Temperature Measurement Task for FreeRTOS
 * @param
 * @retval None
 */
 static void TempMeasureLL_Task_Entry( void *argument )
{
  UNUSED(argument);

  for(;;)
  {
    osSemaphoreAcquire(TempMeasLLSemaphore, osWaitForever);
    TEMPMEAS_RequestTemperatureMeasurement();
  }
}

/**
  * @brief  Link Layer temperature request background process initialization
  * @param  None
  * @retval None
  */
void ll_sys_bg_temperature_measurement_init(void)
{
  /* Create Temperature Measurement Link Layer FreeRTOS objects */

  TempMeasLLTaskHandle = osThreadNew(TempMeasureLL_Task_Entry, NULL, &TempMeasLLTask_attributes);

  TempMeasLLSemaphore = osSemaphoreNew(1U, 0U, &TempMeasLLSemaphore_attributes);

  if ((TempMeasLLTaskHandle == NULL) || (TempMeasLLSemaphore == NULL))
  {
    LOG_ERROR_APP( "Temperature Measurement Link Layer FreeRTOS objects creation FAILED");
    Error_Handler();
  }

}

/**
  * @brief  Request backroud task processing for temperature measurement
  * @param  None
  * @retval None
  */
void ll_sys_bg_temperature_measurement(void)
{
  static uint8_t initial_temperature_acquisition = 0;

  if(initial_temperature_acquisition == 0)
  {
    TEMPMEAS_RequestTemperatureMeasurement();
    initial_temperature_acquisition = 1;
  }
  else
  {
    osSemaphoreRelease(TempMeasLLSemaphore);
  }
}

#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */

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
