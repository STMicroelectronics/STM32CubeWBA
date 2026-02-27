/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
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
#include "app_freertos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_ble.h"
#include "p2p_server_app.h"
#include "ll_sys_if.h"
#include "timer_if.h"
#include "log_module.h"
#include "serial_cmd_interpreter.h"
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for advertisingTask */
osThreadId_t advertisingTaskHandle;
const osThreadAttr_t advertisingTask_attributes = {
  .name = "advertisingTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512 * 4
};
/* Definitions for P2PAPPCmdTask */
osThreadId_t P2PAPPCmdTaskHandle;
const osThreadAttr_t P2PAPPCmdTask_attributes = {
  .name = "P2PAPPCmdTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};
/* Definitions for BLEInitTask */
osThreadId_t BLEInitTaskHandle;
const osThreadAttr_t BLEInitTask_attributes = {
  .name = "BLEInitTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 1024 * 4
};
/* Definitions for advLowPowerTimer */
osTimerId_t advLowPowerTimerHandle;
const osTimerAttr_t advLowPowerTimer_attributes = {
  .name = "advLowPowerTimer"
};
/* Definitions for switchOffGPIOTimer */
osTimerId_t switchOffGPIOTimerHandle;
const osTimerAttr_t switchOffGPIOTimer_attributes = {
  .name = "switchOffGPIOTimer"
};
/* Definitions for advertisingCmdQueue */
osMessageQueueId_t advertisingCmdQueueHandle;
const osMessageQueueAttr_t advertisingCmdQueue_attributes = {
  .name = "advertisingCmdQueue"
};
/* Definitions for BLEInitCmdQueue */
osMessageQueueId_t BLEInitCmdQueueHandle;
const osMessageQueueAttr_t BLEInitCmdQueue_attributes = {
  .name = "BLEInitCmdQueue"
};
/* Definitions for P2PAPPSemaphore */
osSemaphoreId_t P2PAPPSemaphoreHandle;
const osSemaphoreAttr_t P2PAPPSemaphore_attributes = {
  .name = "P2PAPPSemaphore"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
/* USER CODE END FunctionPrototypes */

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
  Error_Handler();
}
/* USER CODE END 4 */

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{

}

__weak unsigned long getRunTimeCounterValue(void)
{
  return TIMER_IF_GetTimerValue();
}
/* USER CODE END 1 */

/* USER CODE BEGIN VPORT_SUPPORT_TICKS_AND_SLEEP */
__weak void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
{
  // Generated when configUSE_TICKLESS_IDLE == 2.
  // Function called in tasks.c (in portTASK_FUNCTION).
  // TO BE COMPLETED or TO BE REPLACED by a user one, overriding that weak one.
}
/* USER CODE END VPORT_SUPPORT_TICKS_AND_SLEEP */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */
  /* creation of P2PAPPSemaphore */
  P2PAPPSemaphoreHandle = osSemaphoreNew(1, 1, &P2PAPPSemaphore_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  osSemaphoreAcquire(P2PAPPSemaphoreHandle, osWaitForever);
  /* USER CODE END RTOS_SEMAPHORES */
  /* creation of advLowPowerTimer */
  advLowPowerTimerHandle = osTimerNew(advLowPowerTimer_cb, osTimerOnce, NULL, &advLowPowerTimer_attributes);

  /* creation of switchOffGPIOTimer */
  switchOffGPIOTimerHandle = osTimerNew(switchOffGPIOTimer_cb, osTimerOnce, NULL, &switchOffGPIOTimer_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */
  /* creation of advertisingCmdQueue */
  advertisingCmdQueueHandle = osMessageQueueNew (16, sizeof(uint16_t), &advertisingCmdQueue_attributes);
  /* creation of BLEInitCmdQueue */
  BLEInitCmdQueueHandle = osMessageQueueNew (16, sizeof(uint16_t), &BLEInitCmdQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
  /* creation of advertisingTask */
  advertisingTaskHandle = osThreadNew(advertisingTask_Entry, NULL, &advertisingTask_attributes);

  /* creation of P2PAPPCmdTask */
  P2PAPPCmdTaskHandle = osThreadNew(P2PAPPCmdTask_Entry, NULL, &P2PAPPCmdTask_attributes);

  /* creation of BLEInitTask */
  BLEInitTaskHandle = osThreadNew(BLEInitTask_Entry, NULL, &BLEInitTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_advertisingTask_Entry */
/**
* @brief Function implementing the advertisingTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_advertisingTask_Entry */
void advertisingTask_Entry(void *argument)
{
  /* USER CODE BEGIN advertisingTask */
  uint16_t advCmd;
  uint8_t advCmdPrio;

  for(;;)
  {
    osMessageQueueGet(advertisingCmdQueueHandle, &advCmd, &advCmdPrio, osWaitForever);

    switch (advCmd)
    {
      case 0:
        APP_BLE_AdvStart();
        break;
      case 1:
        APP_BLE_AdvLowPower();
        break;
      case 2:
        APP_BLE_AdvStop();
        break;
      default:
        break;
    }
  }
  /* USER CODE END advertisingTask */
}

/* USER CODE BEGIN Header_P2PAPPCmdTask_Entry */
/**
* @brief Function implementing the P2PAPPCmdTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_P2PAPPCmdTask_Entry */
void P2PAPPCmdTask_Entry(void *argument)
{
  /* USER CODE BEGIN P2PAPPCmdTask */
  /* Infinite loop */
  for(;;)
  {
    osSemaphoreAcquire(P2PAPPSemaphoreHandle, osWaitForever);
    app_P2P_SERVER_Switch_c_SendNotification();
  }
  /* USER CODE END P2PAPPCmdTask */
}

/* USER CODE BEGIN Header_BLEInitTask_Entry */
/**
* @brief Function implementing the BLEInitTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_BLEInitTask_Entry */
void BLEInitTask_Entry(void *argument)
{
  /* USER CODE BEGIN BLEInitTask */
  uint16_t initCmd;
  uint8_t initCmdPrio;

  /* Infinite loop */
  for(;;)
  {
    osMessageQueueGet(BLEInitCmdQueueHandle, &initCmd, &initCmdPrio, osWaitForever);

    switch (initCmd)
    {
      case 0:
        APP_BLE_Init();
        break;
      case 1:
        APP_BLE_Deinit();
        break;
      default:
        break;
    }
  }
  /* USER CODE END BLEInitTask */
}

/* advLowPowerTimer_cb function */
void advLowPowerTimer_cb(void *argument)
{
  /* USER CODE BEGIN advLowPowerTimer_cb */
  uint16_t adv_cmd = 1;
  osMessageQueuePut(advertisingCmdQueueHandle, &adv_cmd, 0U, 0U);
  /* USER CODE END advLowPowerTimer_cb */
}

/* switchOffGPIOTimer_cb function */
void switchOffGPIOTimer_cb(void *argument)
{
  /* USER CODE BEGIN switchOffGPIOTimer_cb */
  Switch_OFF_GPIO();
  /* USER CODE END switchOffGPIOTimer_cb */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* USER CODE END Application */

