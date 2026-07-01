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
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADV_TASK_STACK_SIZE           (RTOS_STACK_SIZE_NORMAL+RTOS_STACK_SIZE_REDUCED)
#define ADV_TASK_PRIO                 (15u)
#define ADV_TASK_PREEM_TRES           (0u)
#define ADV_QUEUE_SIZE                (8u * sizeof(ULONG))
#define ADV_TIMEOUT_MS                (60 * 1000)
#define APP_BLE_MS_TO_TICKS(ms)       ((ULONG)((((uint64_t)(ms)) * TX_TIMER_TICKS_PER_SECOND + 999u) / 1000u))

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
TX_THREAD                 ADV_Thread;
TX_QUEUE                  ADV_Queue;
TX_TIMER                  Advertising_mgr_timer;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static void Advertising_mgr_timer_cb(unsigned long thread_input);

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
  if (tx_queue_create(&ADV_Queue, "ADV_Queue", TX_1_ULONG, pStack, ADV_QUEUE_SIZE) != TX_SUCCESS)
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

  if (tx_timer_create(&Advertising_mgr_timer,
                      "Advertising_mgr_timer",
                      Advertising_mgr_timer_cb,
                      0,
                      APP_BLE_MS_TO_TICKS(ADV_TIMEOUT_MS),
                      0,
                      TX_NO_ACTIVATE) != TX_SUCCESS)
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

/* USER CODE BEGIN 1 */
static void Advertising_mgr_timer_cb(unsigned long thread_input)
{
  ULONG adv_cmd = 1;

  UNUSED(thread_input);

  tx_queue_send(&ADV_Queue, &adv_cmd, TX_NO_WAIT);
}

/* USER CODE END 1 */
