/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ble_timer.c
  * @author  MCD Application Team
  * @brief   This module implements the timer core functions
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
#include "main.h"
#include "app_common.h"
#include "log_module.h"
#include "blestack.h"
#include "host_stack_if.h"
#include "stm32_timer.h"
#include "bleplat.h"
#include "stm_list.h"
#include "ble_timer.h"
#include "advanced_memory_manager.h"
#include "app_conf.h"
#include "ll_sys.h"
#include "stm32_rtos.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  tListNode           node;         /* Actual node in the list */
  uint16_t            id;           /* Id of the timer */
  UTIL_TIMER_Object_t timerObject;  /* Timer Server object */
}BLE_TIMER_t;

/* Private defines -----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static tListNode BLE_TIMER_RunningList;
static tListNode BLE_TIMER_ExpiredList;

/* FreeRTOS objects declaration */

static osThreadId_t     BleTimerTaskHandle;
static osSemaphoreId_t  BleTimerSemaphore;

const osThreadAttr_t BleTimerTask_attributes = {
  .name         = "BLE Timer Task",
  .priority     = TASK_PRIO_BLE_TIMER,
  .stack_size   = TASK_STACK_SIZE_BLE_TIMER,
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE,
  .stack_mem    = TASK_DEFAULT_STACK_MEM
};

const osSemaphoreAttr_t BleTimerSemaphore_attributes = {
  .name         = "BLE Timer Semaphore",
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE
};

/* Private functions prototype------------------------------------------------*/
static void BLE_TIMER_Callback(void* arg);
static BLE_TIMER_t* BLE_TIMER_GetFromList(tListNode * listHead, uint16_t id);
static void BLE_TIMER_Task_Entry(void* argument);

void BLE_TIMER_Init(void)
{
  /* This function initializes the timer Queue */
  LST_init_head(&BLE_TIMER_RunningList);
  LST_init_head(&BLE_TIMER_ExpiredList);

  /* Create BLE Timer FreeRTOS objects */

  BleTimerTaskHandle = osThreadNew(BLE_TIMER_Task_Entry, NULL, &BleTimerTask_attributes);

  BleTimerSemaphore = osSemaphoreNew(1U, 0U, &BleTimerSemaphore_attributes);

  if((BleTimerTaskHandle == NULL) || (BleTimerSemaphore == NULL) )
  {
    LOG_ERROR_APP( "BLE Timer FreeRTOS objects creation FAILED");
    Error_Handler();
  }
}

uint8_t BLE_TIMER_Start(uint16_t id, uint32_t timeout)
{
  BLE_TIMER_t *timer = NULL;

  /* If the timer's id already exists, stop it */
  BLE_TIMER_Stop(id);

  /* Create a new timer instance and add it to the list */
  if(AMM_ERROR_OK != AMM_Alloc (CFG_AMM_VIRTUAL_BLE_TIMERS,
                                DIVC(sizeof(BLE_TIMER_t), sizeof(uint32_t)),
                                (uint32_t **)&timer,
                                NULL))
  {
    return BLE_STATUS_INSUFFICIENT_RESOURCES;
  }

  if(UTIL_TIMER_Create(&timer->timerObject, timeout, UTIL_TIMER_ONESHOT, &BLE_TIMER_Callback, timer) != UTIL_TIMER_OK)
  {
    (void)AMM_Free((uint32_t *)timer);
    return BLE_STATUS_FAILED;
  }

  if(UTIL_TIMER_Start(&timer->timerObject) != UTIL_TIMER_OK)
  {
    (void)AMM_Free((uint32_t *)timer);
    return BLE_STATUS_FAILED;
  }

  timer->id = id;
  LST_insert_tail(&BLE_TIMER_RunningList, (tListNode *)timer);

  return BLE_STATUS_SUCCESS;
}

void BLE_TIMER_Stop(uint16_t id)
{
  BLE_TIMER_t* timer;

  /* Search for the id in the running timers list */
  timer = BLE_TIMER_GetFromList(&BLE_TIMER_RunningList, id);
  /* If not found, try elapsed timers list */
  if (NULL == timer)
  {
    timer = BLE_TIMER_GetFromList(&BLE_TIMER_ExpiredList, id);
  }

  /* If the timer's id exists, stop it */
  if(NULL != timer)
  {
    UTIL_TIMER_Stop(&timer->timerObject);
    LST_remove_node((tListNode *)timer);

    (void)AMM_Free((uint32_t *)timer);
  }
}

void BLE_TIMER_Background(void)
{
  BLE_TIMER_t* timer;

  if (TRUE != LST_is_empty(&BLE_TIMER_ExpiredList))
  {
    /* Get first timer from (sorted) expired list and remove it from this list */
    LST_remove_head(&BLE_TIMER_ExpiredList, (tListNode **)&timer);
    BLEPLATCB_TimerExpiry(timer->id);
    BleStackCB_Process();
    (void)AMM_Free((uint32_t *)timer);

    if (TRUE != LST_is_empty(&BLE_TIMER_ExpiredList))
    {
      /* At least one other timer expired and has not been processed,
         so trigger task again */
      osSemaphoreRelease(BleTimerSemaphore);
    }
  }
}

static void BLE_TIMER_Task_Entry(void* argument)
{
  UNUSED(argument);

  for(;;)
  {
    osSemaphoreAcquire(BleTimerSemaphore, osWaitForever);
    BLE_TIMER_Background();
    osThreadYield();
  }
}

static void BLE_TIMER_Callback(void* arg)
{
  /* Remove timer from running list */
  LST_remove_node((tListNode *)arg);
  /* Add it to expired list */
  LST_insert_tail(&BLE_TIMER_ExpiredList, (tListNode *)arg);

  osSemaphoreRelease(BleTimerSemaphore);
}

static BLE_TIMER_t* BLE_TIMER_GetFromList(tListNode * listHead, uint16_t id)
{
  BLE_TIMER_t* currentNode;

  LST_get_next_node(listHead, (tListNode **)&currentNode);
  while((tListNode *)currentNode != listHead)
  {
    if(currentNode->id == id)
    {
      return currentNode;
    }
    LST_get_next_node((tListNode *)currentNode, (tListNode **)&currentNode);
  }
  return NULL;
}
