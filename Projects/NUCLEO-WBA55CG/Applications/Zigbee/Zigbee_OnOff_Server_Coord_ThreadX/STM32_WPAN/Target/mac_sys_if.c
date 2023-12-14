/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mac_sys_if.c
  * @author  MCD Application Team
  * @brief   Source file for using MAC Layer with a RTOS
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

#include "app_conf.h"
#include "main.h"
#include "stm32_rtos.h"

extern void mac_baremetal_run(void);

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static TX_SEMAPHORE     stMacLayerTaskSemaphore, stMacLayerEventSemaphore;
static TX_THREAD        stMacLayerThread;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/
/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Functions Definition ------------------------------------------------------*/

/**
 * @brief  Mac Layer Task for ThreadX
 * @param  lArgument  Argument passed the first time.
 * @retval None
 */
static void MacSys_Process( ULONG lArgument )
{
  UNUSED( lArgument );

  for (;;)
  {
    tx_semaphore_get( &stMacLayerTaskSemaphore, TX_WAIT_FOREVER );
    mac_baremetal_run();
  }
}

/**
  * @brief  Mac Layer Initialisation
  * @param  None
  * @retval None
  */
void MacSys_Init(void)
{
  UINT    lThreadXStatus;
  CHAR    * pStack = TX_NULL;

  /* Create the Mac Layer Task Semaphore.*/
  lThreadXStatus = tx_semaphore_create( &stMacLayerTaskSemaphore, "MacLayerTaskSemaphore", 0 );
  if ( lThreadXStatus != TX_SUCCESS )
  {
    LOG_ERROR_APP( "ERROR THREADX : MAC LAYER TASK SEMAPHORE CREATION FAILED (%d)", lThreadXStatus );
    Error_Handler();
  }

  /* Create the Mac Layer Thread and this Stack */
  lThreadXStatus = tx_byte_allocate( pBytePool, (VOID**) &pStack, TASK_MAC_LAYER_STACK_SIZE, TX_NO_WAIT);
  if ( lThreadXStatus == TX_SUCCESS )
  {
    lThreadXStatus = tx_thread_create( &stMacLayerThread, "MacLayerTaskId", MacSys_Process, 0, pStack,
                                        TASK_MAC_LAYER_STACK_SIZE, CFG_TASK_PRIO_MAC_LAYER, CFG_TASK_PREEMP_MAC_LAYER,
                                        TX_NO_TIME_SLICE, TX_AUTO_START);
  }
  if ( lThreadXStatus != TX_SUCCESS )
  {
    LOG_ERROR_APP( "ERROR THREADX : MAC LAYER TASK CREATION FAILED (%d)", lThreadXStatus );
    Error_Handler();
  }

  /* Create the Mac Layer Event Semaphore. */
  lThreadXStatus = tx_semaphore_create( &stMacLayerEventSemaphore, "MacLayerEventSemaphore", 0 );
  if ( lThreadXStatus != TX_SUCCESS )
  {
    LOG_ERROR_APP( "ERROR THREADX : EVENT MAC LAYER EVENT SEMAPHORE CREATION FAILED (%d)", lThreadXStatus );
    Error_Handler();
  }
}

/**
  * @brief  Mac Layer Resume
  * @param  None
  * @retval None
  */
void MacSys_Resume(void)
{
  tx_thread_resume( &stMacLayerThread );
}

/**
  * @brief  MAC Layer set Task.
  * @param  None
  * @retval None
  */
void MacSys_SemaphoreSet(void)
{
  if ( stMacLayerTaskSemaphore.tx_semaphore_count == 0 )
  {
    tx_semaphore_put( &stMacLayerTaskSemaphore );
  }
}

/**
  * @brief  MAC Layer Task wait.
  * @param  None
  * @retval None
  */
void MacSys_SemaphoreWait( void )
{
  tx_semaphore_get( &stMacLayerTaskSemaphore, TX_WAIT_FOREVER );
}

/**
  * @brief  MAC Layer set Event.
  * @param  None
  * @retval None
  */
void MacSys_EventSet( void )
{
  if ( stMacLayerEventSemaphore.tx_semaphore_count == 0 )
  {
    tx_semaphore_put( &stMacLayerEventSemaphore );
  }
}

/**
  * @brief  MAC Layer wait Event.
  * @param  None
  * @retval None
  */
void MacSys_EventWait( void )
{
  tx_semaphore_get( &stMacLayerEventSemaphore, TX_WAIT_FOREVER );
}

