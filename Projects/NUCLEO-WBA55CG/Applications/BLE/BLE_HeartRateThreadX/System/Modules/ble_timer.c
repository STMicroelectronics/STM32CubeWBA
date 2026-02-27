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
static tListNode BLE_TIMER_RunningList = {0};
static tListNode BLE_TIMER_ExpiredList = {0};

/* ThreadX objects declaration */

static TX_THREAD        BleTimerTaskHandle = {0};
static TX_SEMAPHORE     BleTimerSemaphore  = {0};
static CHAR            *BleTimerStack_p;

/* Private functions prototype------------------------------------------------*/
static void BLE_TIMER_Callback(void* arg);
static BLE_TIMER_t* BLE_TIMER_GetFromList(tListNode * listHead, uint16_t id);
static void BLE_TIMER_Task_Entry(ULONG lArgument);

void BLE_TIMER_Init(void)
{
  /* Initializes timers Queue */
  LST_init_head(&BLE_TIMER_RunningList);
  LST_init_head(&BLE_TIMER_ExpiredList);

  UINT TXstatus;
  /* Create BLE Timer ThreadX objects */

  TXstatus = tx_byte_allocate(pBytePool, (void **)&BleTimerStack_p, TASK_STACK_SIZE_BLE_TIMER, TX_NO_WAIT);

  if( TXstatus == TX_SUCCESS )
  {
    TXstatus = tx_thread_create(&BleTimerTaskHandle, "BLE Timer Task", BLE_TIMER_Task_Entry, 0,
                                 BleTimerStack_p, TASK_STACK_SIZE_BLE_TIMER,
                                 TASK_PRIO_BLE_TIMER, TASK_PREEMP_BLE_TIMER,
                                 TX_NO_TIME_SLICE, TX_AUTO_START);

    TXstatus |= tx_semaphore_create(&BleTimerSemaphore, "BLE Timer Semaphore", 0);
  }

  if( TXstatus != TX_SUCCESS )
  {
    LOG_ERROR_APP( "BLE Timer ThreadX objects creation FAILED, status: %d", TXstatus);
    Error_Handler();
  }
}

void BLE_TIMER_Deinit(void)
{
  tListNode *listNodeRemoved;

  /* Delete BLE Timer ThreadX objects */
  /* Only delete semaphore if it was successfully created: check its internal ID */
  if (BleTimerSemaphore.tx_semaphore_id == TX_SEMAPHORE_ID)
  {
    LOG_INFO_APP("Deleting semaphore %s, status : ",BleTimerSemaphore.tx_semaphore_name);
    if (tx_semaphore_delete(&BleTimerSemaphore) != TX_SUCCESS)
    {
      LOG_INFO_APP("FAILED\n");
      Error_Handler();
    }
    else
    {
      LOG_INFO_APP("SUCCESS\n");
    }
  }

  /* Terminate then delete the thread only if created */
  if (BleTimerTaskHandle.tx_thread_id == TX_THREAD_ID)
  {
    LOG_INFO_APP("Terminating and Deleting thread %s, status : ",BleTimerTaskHandle.tx_thread_name);
    if (tx_thread_terminate(&BleTimerTaskHandle) != TX_SUCCESS)
    {
      LOG_INFO_APP("FAILED\n");
      Error_Handler();
    }
    else
    {
      if (tx_thread_delete(&BleTimerTaskHandle) != TX_SUCCESS)
      {
        LOG_INFO_APP("FAILED\n");
        Error_Handler();
      }
      else
      {
        LOG_INFO_APP("SUCCESS\n");
      }
    }
  }

  /* Release the allocated memory if present */
  if (BleTimerStack_p != NULL)
  {
    if (tx_byte_release(BleTimerStack_p) != TX_SUCCESS)
    {
      LOG_INFO_APP("BLE Timer memory release FAILED\n");
      Error_Handler();
    }
    else
    {
      LOG_INFO_APP("BLE Timer memory release SUCCESS\n");
      BleTimerStack_p = NULL;
    }
  }

  while(LST_is_empty(&BLE_TIMER_RunningList) != TRUE)
  {
    LST_remove_tail(&BLE_TIMER_RunningList, &listNodeRemoved);
    (void)AMM_Free((uint32_t *)listNodeRemoved);
  }
  while(LST_is_empty(&BLE_TIMER_ExpiredList) != TRUE)
  {
    LST_remove_tail(&BLE_TIMER_ExpiredList, &listNodeRemoved);
    (void)AMM_Free((uint32_t *)listNodeRemoved);
  }

  /* Reset timers Queues */
  LST_init_head(&BLE_TIMER_RunningList);
  LST_init_head(&BLE_TIMER_ExpiredList);

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
      tx_semaphore_ceiling_put(&BleTimerSemaphore, 1);
    }
  }
}

static void BLE_TIMER_Task_Entry(ULONG lArgument)
{
  UNUSED(lArgument);
  /* USER CODE BEGIN BLE_TIMER_Task_Entry_0 */

  /* USER CODE END BLE_TIMER_Task_Entry_0 */

  for(;;)
  {
    tx_semaphore_get(&BleTimerSemaphore, TX_WAIT_FOREVER);
    BLE_TIMER_Background();
    tx_thread_relinquish();
  }
}

static void BLE_TIMER_Callback(void* arg)
{
  /* Remove timer from running list */
  LST_remove_node((tListNode *)arg);
  /* Add it to expired list */
  LST_insert_tail(&BLE_TIMER_ExpiredList, (tListNode *)arg);

  tx_semaphore_ceiling_put(&BleTimerSemaphore, 1);
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
