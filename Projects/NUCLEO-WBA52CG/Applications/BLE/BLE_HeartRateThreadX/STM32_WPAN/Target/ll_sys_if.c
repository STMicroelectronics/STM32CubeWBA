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
#define LINK_LAYER_TASK_PRIO          (15)
#define LINK_LAYER_TASK_PREEM_TRES    (0)

/* LINK_LAYER_TEMP_MEAS_TASK related defines */
#define LINK_LAYER_TEMP_MEAS_TASK_STACK_SIZE    (256*7)
#define LINK_LAYER_TEMP_MEAS_TASK_PRIO          (15)
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

/**
  * @brief  Link Layer configuration phase before application startup.
  * @param  None
  * @retval None
  */
void ll_sys_config_params(void)
{
  /* Configure link layer behavior for low ISR use and next event scheduling method:
   * - SW low ISR is used.
   * - Next event is scheduled from ISR.
   */
  ll_intf_config_ll_ctx_params(USE_RADIO_LOW_ISR, NEXT_EVENT_SCHEDULING_FROM_ISR);
#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
  /* Initialize link layer temperature measurement background task */
  ll_sys_bg_temperature_measurement_init();

  /* Link layer IP uses temperature based calibration instead of periodic one */
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

/**
  * @brief  Request backroud task processing for temperature measurement
  * @param  None
  * @retval None
  */
void ll_sys_bg_temperature_measurement(void)
{
  tx_semaphore_put(&LINK_LAYER_TEMP_MEAS_Thread_Sem);
}

/**
  * @brief  Request temperature measurement
  * @param  None
  * @retval None
  */
void request_temperature_measurement(void)
{
  int16_t temperature_value = 0;

  /* Enter limited critical section : disable all the interrupts with priority higher than RCC one
   * Concerns link layer interrupts (high and SW low) or any other high priority user system interrupt
   */
  UTILS_ENTER_LIMITED_CRITICAL_SECTION(RCC_INTR_PRIO<<4);

  /* Request ADC IP activation */
  adc_ctrl_req(SYS_ADC_LL_EVT, ADC_ON);

  /* Get temperature from ADC dedicated channel */
  temperature_value = adc_ctrl_request_temperature();

  /* Request ADC IP deactivation */
  adc_ctrl_req(SYS_ADC_LL_EVT, ADC_OFF);

  /* Give the temperature information to the link layer */
  ll_intf_set_temperature_value(temperature_value);

  /* Exit limited critical section */
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
    tx_thread_relinquish();
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
    tx_thread_relinquish();
  }
}

/**
  * @brief  Enable RTOS context switch.
  * @param  None
  * @retval None
  */
void LINKLAYER_PLAT_EnableOSContextSwitch(void)
{
  tx_interrupt_control(TX_INT_ENABLE);
}

/**
  * @brief  Disable RTOS context switch.
  * @param  None
  * @retval None
  */
void LINKLAYER_PLAT_DisableOSContextSwitch(void)
{
  tx_interrupt_control(TX_INT_DISABLE);
}
