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
#include "app_threadx.h"
#include "adc_ctrl.h"
#include "linklayer_plat.h"
/* Private defines -----------------------------------------------------------*/

/* LINK_LAYER_TASK related defines */
#define LINK_LAYER_TASK_STACK_SIZE    (256*7)
#define LINK_LAYER_TASK_PRIO          (1)
#define LINK_LAYER_TASK_PREEM_TRES    (0)

/* LINK_LAYER_TEMP_MEAS_TASK related defines */
#define LINK_LAYER_TEMP_MEAS_TASK_STACK_SIZE    (256*7)
#define LINK_LAYER_TEMP_MEAS_TASK_PRIO          (5)
#define LINK_LAYER_TEMP_MEAS_TASK_PREEM_TRES    (0)

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* LINK_LAYER_TEMP_MEAS_TASK related resources */
TX_THREAD LINK_LAYER_TEMP_MEAS_Thread;
TX_SEMAPHORE LINK_LAYER_TEMP_MEAS_Thread_Sem;

/* LINK_LAYER_TASK related resources */
TX_THREAD LINK_LAYER_Thread;
TX_SEMAPHORE LINK_LAYER_Thread_Sem;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/

/* LINK_LAYER_TASK related resources */
TX_MUTEX LINK_LAYER_Thread_Mutex;

/* USER CODE BEGIN GV */

/* USER CODE END GV */

#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
void ll_sys_bg_temperature_measurement(void);
static void ll_sys_bg_temperature_measurement_init(void);
static void request_temperature_measurement(void);
static void request_temperature_measurement_Entry(unsigned long thread_input);
static void ll_sys_bg_process_Entry(unsigned long thread_input);
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */

/**
  * @brief  Link Layer background process initialization
  * @param  None
  * @retval None
  */
void ll_sys_bg_process_init(void)
{
  /* Tasks creation */
  CHAR * pStack;

  if (tx_byte_allocate(pBytePool, (void **) &pStack, LINK_LAYER_TASK_STACK_SIZE,TX_NO_WAIT) != TX_SUCCESS)
  {
    Error_Handler();
  }
  if (tx_semaphore_create(&LINK_LAYER_Thread_Sem, "LINK_LAYER_Thread_Sem", 0)!= TX_SUCCESS )
  {
    Error_Handler();
  }
  if (tx_mutex_create(&LINK_LAYER_Thread_Mutex, "LINK_LAYER_Thread_Mutex", TX_NO_INHERIT)!=TX_SUCCESS )
  {
    Error_Handler();
  }
  if (tx_thread_create(&LINK_LAYER_Thread, "LINK_LAYER_Thread", ll_sys_bg_process_Entry, 0,
                         pStack, LINK_LAYER_TASK_STACK_SIZE,
                         LINK_LAYER_TASK_PRIO, LINK_LAYER_TASK_PREEM_TRES,
                         TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
  {
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
  tx_semaphore_put(&LINK_LAYER_Thread_Sem);
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
  CHAR * pStack;

  if (tx_byte_allocate(pBytePool, (void **) &pStack, LINK_LAYER_TEMP_MEAS_TASK_STACK_SIZE,TX_NO_WAIT) != TX_SUCCESS)
  {
    Error_Handler();
  }
  if (tx_semaphore_create(&LINK_LAYER_TEMP_MEAS_Thread_Sem, "LINK_LAYER_TEMP_MEAS_Thread_Sem", 0)!= TX_SUCCESS )
  {
    Error_Handler();
  }
  if (tx_thread_create(&LINK_LAYER_TEMP_MEAS_Thread, "LINK_LAYER_TEMP_MEAS_Thread", request_temperature_measurement_Entry, 0,
                         pStack, LINK_LAYER_TEMP_MEAS_TASK_STACK_SIZE,
                         LINK_LAYER_TEMP_MEAS_TASK_PRIO, LINK_LAYER_TEMP_MEAS_TASK_PREEM_TRES,
                         TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
  {
    Error_Handler();
  }
}

void ll_sys_bg_temperature_measurement(void)
{
  tx_semaphore_put(&LINK_LAYER_TEMP_MEAS_Thread_Sem);
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

static void request_temperature_measurement_Entry(unsigned long thread_input)
{
  (void)(thread_input);

  while(1)
  {
    tx_semaphore_get(&LINK_LAYER_TEMP_MEAS_Thread_Sem, TX_WAIT_FOREVER);
    tx_mutex_get(&LINK_LAYER_Thread_Mutex, TX_WAIT_FOREVER);
    request_temperature_measurement();
    tx_mutex_put(&LINK_LAYER_Thread_Mutex);
  }
}
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */

static void ll_sys_bg_process_Entry(unsigned long thread_input)
{
  (void)(thread_input);

  while(1)
  {
    tx_semaphore_get(&LINK_LAYER_Thread_Sem, TX_WAIT_FOREVER);
    tx_mutex_get(&LINK_LAYER_Thread_Mutex, TX_WAIT_FOREVER);
    ll_sys_bg_process();
    tx_mutex_put(&LINK_LAYER_Thread_Mutex);
  }
}

void LINKLAYER_PLAT_EnableOSContextSwitch(void)
{

}

void LINKLAYER_PLAT_DisableOSContextSwitch(void)
{

}
