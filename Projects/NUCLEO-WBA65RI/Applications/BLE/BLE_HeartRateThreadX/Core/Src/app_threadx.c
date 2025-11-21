/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.c
  * @author  MCD Application Team
  * @brief   ThreadX applicative file
  ******************************************************************************
    * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "app_entry.h"
#include "app_ble.h"
#include "stm32_rtos.h"
#include "hrs_app.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* ADV_TASK related resources */
TX_THREAD                 ADV_Thread;
TX_QUEUE                  ADV_Queue;
TX_TIMER                  ADV_LP_Timer;

/* MEAS_REQ_TASK related resources */
TX_THREAD                 MEAS_REQ_Thread;
TX_SEMAPHORE              MEAS_REQ_Thread_Sem;
TX_TIMER                  MEAS_REQ_Timer;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static void ADV_LP_Timer_cb(unsigned long thread_input);
static void MEAS_REQ_Timer_cb(unsigned long thread_input);
/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;
  /* USER CODE BEGIN App_ThreadX_MEM_POOL */

  /* USER CODE END App_ThreadX_MEM_POOL */
  /* USER CODE BEGIN App_ThreadX_Init */
  CHAR *pStack;

  if (tx_byte_allocate(memory_ptr, (void **) &pStack, ADV_QUEUE_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    Error_Handler();
  }
  if (tx_queue_create(&ADV_Queue, "ADV_Queue", TX_1_ULONG, pStack, ADV_QUEUE_SIZE)!= TX_SUCCESS )
  {
    Error_Handler();
  }

  if (tx_byte_allocate(memory_ptr, (void **) &pStack, ADV_TASK_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    Error_Handler();
  }
  if (tx_thread_create(&ADV_Thread, "ADV_Thread", APP_BLE_Adv_Entry, 0,
                         pStack, ADV_TASK_STACK_SIZE,
                         ADV_TASK_PRIO, ADV_TASK_PREEM_TRES,
                         TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
  {
    Error_Handler();
  }
  
  /* Create timer for Advertising period update */
  if (tx_timer_create(&ADV_LP_Timer, "ADV_LP_Timer", ADV_LP_Timer_cb, 0, 
                      HRS_APP_ADV_LP_UPDATE_TIME, 0, TX_NO_ACTIVATE ) != TX_SUCCESS)
  {
    Error_Handler();
  }
    
  if (tx_byte_allocate(memory_ptr, (void **) &pStack, MEAS_REQ_TASK_STACK_SIZE,TX_NO_WAIT) != TX_SUCCESS)
  {
    Error_Handler();
  }
  if (tx_semaphore_create(&MEAS_REQ_Thread_Sem, "MEAS_REQ_Thread_Sem", 0)!= TX_SUCCESS )
  {
    Error_Handler();
  }
  if (tx_thread_create(&MEAS_REQ_Thread, "MEAS_REQ_Thread", HRS_APP_Measurements_Entry, 0,
                         pStack, MEAS_REQ_TASK_STACK_SIZE,
                         MEAS_REQ_TASK_PRIO, MEAS_REQ_TASK_PREEM_TRES,
                         TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
  {
    Error_Handler();
  }
  
  /* Create timer for Heart Rate Measurement request */
  if (tx_timer_create(&MEAS_REQ_Timer, "MEAS_REQ_Timer", MEAS_REQ_Timer_cb, 0, 
                      HRS_APP_MEASUREMENT_INTERVAL, HRS_APP_MEASUREMENT_INTERVAL, TX_NO_ACTIVATE ) != TX_SUCCESS)
  {
    Error_Handler();
  }
  /* USER CODE END App_ThreadX_Init */

  return ret;
}

  /**
  * @brief  Function that implements the kernel's initialization.
  * @param  None
  * @retval None
  */
void MX_ThreadX_Init(void)
{
  /* USER CODE BEGIN Before_Kernel_Start */

  /* USER CODE END Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN Kernel_Start_Error */

  /* USER CODE END Kernel_Start_Error */
}

/**
  * @brief  ThreadXLowPowerUserEnter
  * @param  None
  * @retval None
  */
__weak void ThreadXLowPowerUserEnter(void)
{
  /* USER CODE BEGIN  ThreadXLowPowerUserEnter */

  /* USER CODE END  ThreadXLowPowerUserEnter */
}

/**
  * @brief  ThreadXLowPowerUserExit
  * @param  None
  * @retval None
  */
__weak void ThreadXLowPowerUserExit(void)
{
  /* USER CODE BEGIN  ThreadXLowPowerUserExit */

  /* USER CODE END  ThreadXLowPowerUserExit */
}

/* USER CODE BEGIN 1 */
static void ADV_LP_Timer_cb(unsigned long thread_input)
{
  UNUSED(thread_input);

  ULONG adv_cmd = 1;
  tx_queue_send(&ADV_Queue, &adv_cmd, TX_NO_WAIT);
}

static void MEAS_REQ_Timer_cb(unsigned long thread_input)
{
  UNUSED(thread_input);

  tx_semaphore_put(&MEAS_REQ_Thread_Sem);
}
/* USER CODE END 1 */
