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
#include "ll_sys_if.h"
#include "app_threadx.h"
#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
#include "adc_ctrl.h"
#endif /* (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1) */

/* Private defines -----------------------------------------------------------*/

/* LINK_LAYER_TASK related defines */
#define TASK_LINK_LAYER_STACK_SIZE          (256*7)
#define CFG_TASK_PRIO_LINK_LAYER            (15)
#define CFG_TASK_PREEMP_LINK_LAYER          (0)

/* LINK_LAYER_TEMP_MEAS_TASK related defines */
#define TASK_LINK_LAYER_TEMP_STACK_SIZE     (256*7)
#define CFG_TASK_PRIO_LINK_LAYER_TEMP       (15)
#define CFG_TASK_PREEMP_LINK_LAYER_TEMP     (0)

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private constants ---------------------------------------------------------*/
/* USER CODE BEGIN PC */

/* USER CODE END PC */

/* Private variables ---------------------------------------------------------*/
/* LINK_LAYER_TEMP_MEAS_TASK related resources */
static TX_SEMAPHORE     LinkLayerMeasSemaphore;
static TX_THREAD        LinkLayerMeasThread;

/* Link Layer Task related resources */
static TX_SEMAPHORE     LinkLayerSemaphore;
static TX_THREAD        LinkLayerThread;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/

/* Link Layer Task related resources */
TX_MUTEX                LinkLayerMutex;

/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private functions prototypes-----------------------------------------------*/
#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
static void ll_sys_bg_temperature_measurement_init(void);
static void request_temperature_measurement(void);
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Functions Definition ------------------------------------------------------*/

/**
 * @brief  Link Layer Task for ThreadX
 * @param  None
 * @retval None
 */
static void ll_sys_bg_process_task( ULONG thread_input )
{
  UNUSED( thread_input );

  for(;;)
  {
    tx_semaphore_get(&LinkLayerSemaphore, TX_WAIT_FOREVER);
    tx_mutex_get(&LinkLayerMutex, TX_WAIT_FOREVER);
    ll_sys_bg_process();
    tx_mutex_put(&LinkLayerMutex);
    tx_thread_relinquish();
  }
}

/**
  * @brief  Link Layer background process initialization
  * @param  None
  * @retval None
  */
void ll_sys_bg_process_init(void)
{
  UINT  ThreadXStatus;
  CHAR  * pStack = TX_NULL;

  /* Register LinkLayer Semaphore */
  ThreadXStatus = tx_semaphore_create( &LinkLayerSemaphore, "LinkLayerSem", 0 );
  if ( ThreadXStatus != TX_SUCCESS )
  {
    LOG_ERROR_APP( "ERROR THREADX : LINK LAYER SEMAPHORE CREATION FAILED (%d)", ThreadXStatus );
    Error_Handler();
  }

  /* Register LinkLayer Mutex */
  ThreadXStatus = tx_mutex_create( &LinkLayerMutex, "LinkLayerMutex", 0 );
  if ( ThreadXStatus != TX_SUCCESS )
  {
    LOG_ERROR_APP( "ERROR FREERTOS : LINK LAYER MUTEX CREATION FAILED." );
    Error_Handler();
  }

  /* Thread associated with LinkLayer Task */
  ThreadXStatus = tx_byte_allocate( pBytePool, (VOID**) &pStack, TASK_LINK_LAYER_STACK_SIZE, TX_NO_WAIT );
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus = tx_thread_create( &LinkLayerThread, "LinkLayerThread", ll_sys_bg_process_task, 0, pStack,
                                      TASK_LINK_LAYER_STACK_SIZE, CFG_TASK_PRIO_LINK_LAYER, CFG_TASK_PREEMP_LINK_LAYER,
                                      TX_NO_TIME_SLICE, TX_AUTO_START );
  }
  if ( ThreadXStatus != TX_SUCCESS )
  {
    LOG_ERROR_APP( "ERROR THREADX : LINK LAYER THREAD CREATION FAILED (%d)", ThreadXStatus );
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
  tx_semaphore_put( &LinkLayerSemaphore );
}

/**
  * @brief  Link Layer background process next iteration scheduling from ISR
  * @param  None
  * @retval None
  */
void ll_sys_schedule_bg_process_isr(void)
{
  tx_semaphore_put( &LinkLayerSemaphore );
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

  /* Link Layer power table */
  ll_intf_select_tx_power_table(CFG_RF_TX_POWER_TABLE_ID);
}

#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
/**
 * @brief  Link Layer Temperature Measurement  Task for FreeRTOS
 * @param  thread_input   Argument passed the first time.
 * @retval None
 */
static void request_temperature_measurement_task( ULONG thread_input )
{
  UNUSED( thread_input );

  for(;;)
  {
    tx_semaphore_get(&LinkLayerMeasSemaphore, TX_WAIT_FOREVER);
    tx_mutex_get(&LinkLayerMutex, TX_WAIT_FOREVER);
    request_temperature_measurement();
    tx_mutex_put(&LinkLayerMutex);
    tx_thread_relinquish();
  }
}

/**
  * @brief  Link Layer temperature request background process initialization
  * @param  None
  * @retval None
  */
void ll_sys_bg_temperature_measurement_init(void)
{
  UINT  ThreadXStatus;
  CHAR  * pStack = TX_NULL;

  /* Register Temp Semaphore */
  ThreadXStatus = tx_semaphore_create( &LinkLayerMeasSemaphore, "LinkLayerTempSem", 0 );
  if ( ThreadXStatus != TX_SUCCESS )
  {
    LOG_ERROR_APP( "ERROR THREADX : LINK LAYER MEAS SEMAPHORE CREATION FAILED (%d)", ThreadXStatus );
    Error_Handler();
  }

  /* Thread associated with LinkLayer Temp Task */
  ThreadXStatus = tx_byte_allocate( pBytePool, (VOID**) &pStack, TASK_LINK_LAYER_TEMP_STACK_SIZE, TX_NO_WAIT );
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus = tx_thread_create( &LinkLayerMeasThread, "LinkLayerMeasThread", request_temperature_measurement_task, 0, pStack,
                                      TASK_LINK_LAYER_TEMP_STACK_SIZE, CFG_TASK_PRIO_LINK_LAYER_TEMP, CFG_TASK_PREEMP_LINK_LAYER_TEMP,
                                      TX_NO_TIME_SLICE, TX_AUTO_START );
  }
  if ( ThreadXStatus != TX_SUCCESS )
  {
    LOG_ERROR_APP( "ERROR THREADX : LINK LAYER MEAS THREAD CREATION FAILED (%d)", ThreadXStatus );
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
  tx_semaphore_put(&LinkLayerMeasSemaphore);
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

#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */
