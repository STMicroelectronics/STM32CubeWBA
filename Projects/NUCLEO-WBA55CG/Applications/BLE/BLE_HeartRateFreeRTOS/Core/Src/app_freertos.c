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
#include "hrs_app.h"
#include "ll_sys_if.h"
#include "timer_if.h"
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
/* Definitions for HRSAPPMeasurementsTask */
osThreadId_t HRSAPPMeasurementsTaskHandle;
const osThreadAttr_t HRSAPPMeasurementsTask_attributes = {
  .name = "HRSAPPMeasurementsTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};
/* Definitions for advLowPowerTimer */
osTimerId_t advLowPowerTimerHandle;
const osTimerAttr_t advLowPowerTimer_attributes = {
  .name = "advLowPowerTimer"
};
/* Definitions for HRSAPPMeasurementsTimer */
osTimerId_t HRSAPPMeasurementsTimerHandle;
const osTimerAttr_t HRSAPPMeasurementsTimer_attributes = {
  .name = "HRSAPPMeasurementsTimer"
};
/* Definitions for advertisingCmdQueue */
osMessageQueueId_t advertisingCmdQueueHandle;
const osMessageQueueAttr_t advertisingCmdQueue_attributes = {
  .name = "advertisingCmdQueue"
};
/* Definitions for HRSAPPMeasurementsSemaphore */
osSemaphoreId_t HRSAPPMeasurementsSemaphoreHandle;
const osSemaphoreAttr_t HRSAPPMeasurementsSemaphore_attributes = {
  .name = "HRSAPPMeasurementsSemaphore"
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
  /* creation of HRSAPPMeasurementsSemaphore */
  HRSAPPMeasurementsSemaphoreHandle = osSemaphoreNew(1, 1, &HRSAPPMeasurementsSemaphore_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  osSemaphoreAcquire(HRSAPPMeasurementsSemaphoreHandle, osWaitForever);
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */
  /* creation of advLowPowerTimer */
  advLowPowerTimerHandle = osTimerNew(advLowPowerTimer_cb, osTimerOnce, NULL, &advLowPowerTimer_attributes);

  /* creation of HRSAPPMeasurementsTimer */
  HRSAPPMeasurementsTimerHandle = osTimerNew(HRSAPPMeasurementsTimer_cb, osTimerPeriodic, NULL, &HRSAPPMeasurementsTimer_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */
  /* creation of advertisingCmdQueue */
  advertisingCmdQueueHandle = osMessageQueueNew (16, sizeof(uint16_t), &advertisingCmdQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
  /* creation of advertisingTask */
  advertisingTaskHandle = osThreadNew(advertisingTask_Entry, NULL, &advertisingTask_attributes);

  /* creation of HRSAPPMeasurementsTask */
  HRSAPPMeasurementsTaskHandle = osThreadNew(HRSAPPMeasurementsTask_Entry, NULL, &HRSAPPMeasurementsTask_attributes);

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

/* USER CODE BEGIN Header_HRSAPPMeasurementsTask_Entry */
/**
* @brief Function implementing the HRSAPPMeasurementsTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_HRSAPPMeasurementsTask_Entry */
void HRSAPPMeasurementsTask_Entry(void *argument)
{
  /* USER CODE BEGIN HRSAPPMeasurementsTask */
  /* Infinite loop */
  for(;;)
  {
    osSemaphoreAcquire(HRSAPPMeasurementsSemaphoreHandle, osWaitForever);

    HRS_APP_Measurements();

    osThreadYield();
  }
  /* USER CODE END HRSAPPMeasurementsTask */
}

/* advLowPowerTimer_cb function */
void advLowPowerTimer_cb(void *argument)
{
  /* USER CODE BEGIN advLowPowerTimer_cb */
  uint16_t adv_cmd = 1;
  osMessageQueuePut(advertisingCmdQueueHandle, &adv_cmd, 0U, 0U);
  /* USER CODE END advLowPowerTimer_cb */
}

/* HRSAPPMeasurementsTimer_cb function */
void HRSAPPMeasurementsTimer_cb(void *argument)
{
  /* USER CODE BEGIN HRSAPPMeasurementsTimer_cb */
  osSemaphoreRelease(HRSAPPMeasurementsSemaphoreHandle);
  /* USER CODE END HRSAPPMeasurementsTimer_cb */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

