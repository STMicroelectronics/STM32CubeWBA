/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    zigbee_sys_if.c
  * @author  MCD Application Team
  * @brief   Source file for using Zigbee Layer with a RTOS
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
#include "app_zigbee.h"
#include "stm32_rtos.h"

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static TX_SEMAPHORE     stZigbeeLayerSemaphore;
static TX_THREAD        stZigbeeLayerThread;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/
/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Functions Definition ------------------------------------------------------*/

/**
 * @brief  Zigbee Layer Task for ThreadX
 * @param  lArgument  Argument passed the first time.
 * @retval None
 */
static void ZigbeeSys_Process( ULONG lArgument )
{
  UNUSED( lArgument );

  for (;;)
  {
    tx_semaphore_get( &stZigbeeLayerSemaphore, TX_WAIT_FOREVER );
    APP_ZIGBEE_Task();
  }
}

/**
  * @brief  Zigbee Layer Initialisation
  * @param  None
  * @retval None
  */
void ZigbeeSys_Init(void)
{
  UINT    lThreadXStatus;
  CHAR  * pStack = TX_NULL;

  /* Create the ZigBee Layer Task Semaphore */
  lThreadXStatus = tx_semaphore_create( &stZigbeeLayerSemaphore, "ZigbeeLayerTaskSemaphore", 0 );
  if ( lThreadXStatus != TX_SUCCESS )
  {
    LOG_ERROR_APP( "ERROR THREADX : ZIGBEE LAYER TASK SEMAPHORE CREATION FAILED." );
    Error_Handler();
  }

  /* Create the ZigBee Layer Thread and this Stack */
  lThreadXStatus = tx_byte_allocate( pBytePool, (VOID**) &pStack, TASK_ZIGBEE_LAYER_STACK_SIZE, TX_NO_WAIT);
  if ( lThreadXStatus == TX_SUCCESS )
  {
    lThreadXStatus = tx_thread_create( &stZigbeeLayerThread, "ZigbeeLayerTaskId", ZigbeeSys_Process, 0, pStack,
                                        TASK_ZIGBEE_LAYER_STACK_SIZE, CFG_TASK_PRIO_ZIGBEE_LAYER, CFG_TASK_PREEMP_ZIGBEE_LAYER,
                                        TX_NO_TIME_SLICE, TX_AUTO_START);
  }
  if ( lThreadXStatus != TX_SUCCESS )
  {
    LOG_ERROR_APP( "ERROR THREADX : ZIGBEE LAYER TASK CREATION FAILED." );
    Error_Handler();
  }

  /* Register Event is not needed */
}

/**
  * @brief  Zigbee Layer Task Resume
  * @param  None
  * @retval None
  */
void ZigbeeSys_Resume(void)
{
  tx_thread_resume( &stZigbeeLayerThread );
}

/**
  * @brief  Zigbee Layer set Task.
  * @param  None
  * @retval None
  */
void ZigbeeSys_SemaphoreSet(void)
{
  if ( stZigbeeLayerSemaphore.tx_semaphore_count == 0 )
  {
    tx_semaphore_put( &stZigbeeLayerSemaphore );
  }
}

/**
  * @brief  Zigbee Layer Task wait. Not used with Sequencer.
  * @param  None
  * @retval None
  */
void ZigbeeSys_SemaphoreWait( void )
{
  tx_semaphore_get( &stZigbeeLayerSemaphore, TX_WAIT_FOREVER );
}

/**
  * @brief  Zigbee Layer set Event.
  * @param  None
  * @retval None
  */
void ZigbeeSys_EventSet( void )
{
  /* Not used */
}

/**
  * @brief  Zigbee Layer wait Event.
  * @param  None
  * @retval None
  */
void ZigbeeSys_EventWait( void )
{
  /* Not used */
}

