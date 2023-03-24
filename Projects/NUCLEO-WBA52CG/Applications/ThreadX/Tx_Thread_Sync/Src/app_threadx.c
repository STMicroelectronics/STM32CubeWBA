/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.c
  * @author  MCD Application Team
  * @brief   ThreadX applicative file
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

/* Includes ------------------------------------------------------------------*/
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_azure_rtos.h"
#include "main.h"
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
TX_THREAD ThreadOne;
TX_THREAD ThreadTwo;
APP_SYNC_TYPE SyncObject;

extern UART_HandleTypeDef huart1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
VOID ThreadOne_Entry(ULONG thread_input);
VOID ThreadTwo_Entry(ULONG thread_input);
static VOID Led_Toggle(uint16_t led_pin, GPIO_TypeDef *led_port, UINT iter);
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
#if (USE_STATIC_ALLOCATION == 1)
  CHAR *pointer;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

  /* Allocate the stack for ThreadOne.  */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    ret = TX_POOL_ERROR;
  }

  /* Create ThreadOne.  */
  if (tx_thread_create(&ThreadOne, "Thread One", ThreadOne_Entry, 0, pointer, APP_STACK_SIZE, THREAD_ONE_PRIO,
                       THREAD_ONE_PREEMPTION_THRESHOLD, DEFAULT_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
  {
    ret = TX_THREAD_ERROR;
  }

  /* Allocate the stack for ThreadTwo.  */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    ret = TX_POOL_ERROR;
  }

  /* Create ThreadTwo.  */
  if (tx_thread_create(&ThreadTwo, "Thread Two", ThreadTwo_Entry, 0, pointer, APP_STACK_SIZE, THREAD_TWO_PRIO,
                       THREAD_TWO_PREEMPTION_THRESHOLD, DEFAULT_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
  {
    ret = TX_THREAD_ERROR;
  }

  /* Create the Synchronization API used by ThreadOne and ThreadTwo.*/
  if (APP_SYNC_CREATE(&SyncObject) != TX_SUCCESS)
  {
    ret = TX_SYNC_ERROR;
  }
#endif
  /* USER CODE END App_ThreadX_Init */

  return ret;
}

  /**
  * @brief  MX_ThreadX_Init
  * @param  None
  * @retval None
  */
void MX_ThreadX_Init(void)
{
  /* USER CODE BEGIN  Before_Kernel_Start */

  /* USER CODE END  Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN  Kernel_Start_Error */

  /* USER CODE END  Kernel_Start_Error */
}

/* USER CODE BEGIN 1 */
/**
  * @brief  Function implementing the ThreadOne thread.
  * @param  thread_input: Not used
  * @retval None
  */
void ThreadOne_Entry(ULONG thread_input)
{
  UNUSED(thread_input);
  ULONG iteration = 0;

  /* Infinite loop */
  while(1)
  {
    /* try to acquire the sync object without waiting */
    if (APP_SYNC_GET(&SyncObject, TX_NO_WAIT) == TX_SUCCESS)
    {
      printf("** ThreadOne : SyncObject acquired ** \n");

      /*sync object acquired, toggle the LED_GREEN each 500ms for 5s */
      Led_Toggle(LED2_Pin, LED2_GPIO_Port, 10);

      /*release the sync object */
      APP_SYNC_PUT(&SyncObject);

      printf("** ThreadOne : SyncObject released ** \n");

      tx_thread_sleep(1);
    }
    else
    {

      if ((iteration % 2000000) == 0)
      {
        printf("** ThreadOne : waiting for SyncObject !! **\n");
      }
    }
    iteration++;
  }
}

/**
  * @brief  Function implementing the ThreadTwo thread.
  * @param  thread_input: Not used
  * @retval None
  */
void ThreadTwo_Entry(ULONG thread_input)
{
  UNUSED(thread_input);
  ULONG iteration = 0;

  /* Infinite loop */
  while(1)
  {
    /* try to acquire the sync object without waiting */
    if (APP_SYNC_GET(&SyncObject, TX_NO_WAIT) == TX_SUCCESS)
    {
      printf("** ThreadTwo : SyncObject acquired ** \n");

      /*Sync object acquired toggle the LED_RED each 500ms for 5s*/
      Led_Toggle(LED1_Pin, LED1_GPIO_Port, 10);

      /*release the sync object*/
      APP_SYNC_PUT(&SyncObject);

      printf("** ThreadTwo : SyncObject released ** \n");

      tx_thread_sleep(1);

    }
    else
    {
      if ((iteration % 2000000) == 0)
      {
        printf("** ThreadTwo : waiting for SyncObject !! **\n");
      }
    }
    iteration++;
  }
}

/**
  * @brief Critical section function that needs acquiring SyncObject.
  * @param  led: LED to toggle
  * @param  iter: Number of iterations
  * @retval None
  */
static VOID Led_Toggle(uint16_t led_pin, GPIO_TypeDef *led_port, UINT iter)
{
  UINT i;

  HAL_GPIO_WritePin(led_port, led_pin, GPIO_PIN_SET);

  for (i =0; i < iter; i++)
  {
    HAL_GPIO_TogglePin(led_port, led_pin);
    /* Toggle the Led each 500ms */
    tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND/2);
  }

  HAL_GPIO_WritePin(led_port, led_pin, GPIO_PIN_SET);
}

/* USER CODE END 1 */
