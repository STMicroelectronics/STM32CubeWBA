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
  TX_THREAD tx_app_thread;
  TX_SEMAPHORE tx_app_semaphore;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
  extern void SystemClock_Config(void);
  static VOID App_Delay(ULONG Delay);
/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

  /* USER CODE BEGIN App_ThreadX_MEM_POOL */

  /* USER CODE END App_ThreadX_MEM_POOL */
CHAR *pointer;

  /* Allocate the stack for Main Thread  */
  if (tx_byte_allocate(byte_pool, (VOID**) &pointer,
                       TX_APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
   /* Create Main Thread.  */
  if (tx_thread_create(&tx_app_thread, "Main Thread", MainThread_Entry, 0, pointer,
                       TX_APP_STACK_SIZE, TX_APP_THREAD_PRIO, TX_APP_THREAD_PREEMPTION_THRESHOLD,
                       TX_APP_THREAD_TIME_SLICE, TX_APP_THREAD_AUTO_START) != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  /* Create Semaphore.  */
  if (tx_semaphore_create(&tx_app_semaphore, "Semaphore", 0) != TX_SUCCESS)
  {
    return TX_SEMAPHORE_ERROR;
  }

  /* USER CODE BEGIN App_ThreadX_Init */
  /* USER CODE END App_ThreadX_Init */

  return ret;
}
/**
  * @brief  Function implementing the MainThread_Entry thread.
  * @param  thread_input: Not used.
  * @retval None
  */
void MainThread_Entry(ULONG thread_input)
{
  /* USER CODE BEGIN MainThread_Entry */
(void) thread_input;
  UINT i = 0;
  /* Infinite loop */
  while (1)
  {
    if (tx_semaphore_get(&tx_app_semaphore, TX_WAIT_FOREVER) == TX_SUCCESS)
    {
      for (i=0; i<10; i++)
      {
      /* Toggle LED to indicate status*/
      HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
      App_Delay(50);
      }
    }
  }
  /* USER CODE END MainThread_Entry */
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

/**
  * @brief  App_ThreadX_LowPower_Enter
  * @param  None
  * @retval None
  */
void App_ThreadX_LowPower_Enter(void)
{
  /* USER CODE BEGIN  App_ThreadX_LowPower_Enter */
  GPIO_InitTypeDef   GPIO_InitStruct;

  HAL_PWREx_DisableRAMsContentStopRetention(PWR_SRAM1_FULL_STOP_RETENTION);
  HAL_PWREx_DisableRAMsContentStopRetention(PWR_SRAM2_FULL_STOP_RETENTION);
  HAL_PWREx_DisableRAMsContentStopRetention(PWR_ICACHE_FULL_STOP_RETENTION);

  /* Configuration of the LPM read mode */
  if (HAL_FLASHEx_ConfigLowPowerRead(FLASH_LPM_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Disable Flash memory fast wakeup */
  HAL_PWREx_DisableFlashFastWakeUp();

  /* Enable ultra low power mode mode in
  Stop 1 and Standby modes */
  HAL_PWREx_EnableUltraLowPowerMode();

  /* Enable LEDs GPIO port clock */
  __HAL_RCC_GPIOB_CLK_ENABLE();


  /* Configure LED port pins as analogue */
  GPIO_InitStruct.Mode  = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Pin   = GPIO_PIN_All ;

  /* Initialize GPIO pins */
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Redisable LEDs GPIO port clock */
  __HAL_RCC_GPIOB_CLK_DISABLE();

  /* Enter Stop1 mode, wake up on interrupt */
  /* Enter to the stop mode */
  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
  /* USER CODE END  App_ThreadX_LowPower_Enter */
}

/**
  * @brief  App_ThreadX_LowPower_Exit
  * @param  None
  * @retval None
  */
void App_ThreadX_LowPower_Exit(void)
{
  /* USER CODE BEGIN  App_ThreadX_LowPower_Exit */
  GPIO_InitTypeDef   GPIO_InitStruct;

  /* Restore system clock */
  SystemClock_Config();

  /* Initialize the LED pin which will be blinked */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStruct.Pin   = LED2_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE END  App_ThreadX_LowPower_Exit */
}

/* USER CODE BEGIN 1 */

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
  ULONG currentValue = 0;
  if (GPIO_Pin == BUTTON_USER_Pin)
  {
    /* Add additional checks to avoid multiple semaphore puts by successively
    clicking on the user button */
    tx_semaphore_info_get(&tx_app_semaphore, NULL, &currentValue, NULL, NULL, NULL);
    if (currentValue == 0)
    {
      /* Put the semaphore to release the MainThread */
      tx_semaphore_put(&tx_app_semaphore);
    }
  }
}

/**
  * @brief  Application Delay function.
  * @param  Delay : number of ticks to wait
  * @retval None
  */
void App_Delay(ULONG Delay)
{
  ULONG initial_time = tx_time_get();
  while ((tx_time_get() - initial_time) < Delay);
}

/* USER CODE END 1 */
