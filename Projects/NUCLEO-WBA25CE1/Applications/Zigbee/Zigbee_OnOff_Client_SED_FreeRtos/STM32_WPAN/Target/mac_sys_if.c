/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mac_sys_if.c
  * @author  MCD Application Team
  * @brief   Source file for using MAC Layer with a RTOS
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include "app_common.h"
#include "app_conf.h"
#include "log_module.h"
#include "stm32_rtos.h"
#include "st_mac_802_15_4_sys.h"

extern void mac_baremetal_run(void);

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* FreeRTOS objects declaration */

static osThreadId_t     stMacLayerTaskHandle;
static osSemaphoreId_t  stMacLayerTaskSemaphore, stMacLayerEventSemaphore;

/* FreeRtos MacLayer stacks attributes */
const osThreadAttr_t stMacLayerTask_attributes =
{
  .name         = "MacLayer Task",
  .priority     = TASK_PRIO_MAC_LAYER,
  .stack_size   = TASK_STACK_SIZE_MAC_LAYER,
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE,
  .stack_mem    = TASK_DEFAULT_STACK_MEM
};

const osSemaphoreAttr_t stMacLayerTaskSemaphore_attributes =
{
  .name         = "MacLayer Task Semaphore",
  .attr_bits    = SEMAPHORE_DEFAULT_ATTR_BITS,
  .cb_mem       =  SEMAPHORE_DEFAULT_CB_MEM,
  .cb_size      =  SEMAPHORE_DEFAULT_CB_SIZE
};

const osSemaphoreAttr_t stMacLayerEventSemaphore_attributes =
{
  .name         = "MacLayer Event Semaphore",
  .attr_bits    = SEMAPHORE_DEFAULT_ATTR_BITS,
  .cb_mem       = SEMAPHORE_DEFAULT_CB_MEM,
  .cb_size      = SEMAPHORE_DEFAULT_CB_SIZE
};

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/
/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Functions Definition ------------------------------------------------------*/

/**
 * @brief  Mac Layer Task for FreeRTOS
 * @param  void *argument
 * @retval None
 */
static void MacSys_Task_Entry( void * argument )
{
  UNUSED( argument );

  for (;;)
  {
    osSemaphoreAcquire( stMacLayerTaskSemaphore, osWaitForever );
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
  /* Create MacLayer FreeRTOS objects */
  stMacLayerTaskSemaphore = osSemaphoreNew( 1, 0, &stMacLayerTaskSemaphore_attributes );
  stMacLayerEventSemaphore = osSemaphoreNew( 1, 0, &stMacLayerEventSemaphore_attributes );

  stMacLayerTaskHandle = osThreadNew( MacSys_Task_Entry, NULL, &stMacLayerTask_attributes );

  if ( ( stMacLayerTaskHandle == NULL ) || ( stMacLayerTaskSemaphore == NULL ) || ( stMacLayerEventSemaphore == NULL ) )
  {
    LOG_ERROR_APP( "MacLayer FreeRTOS objects creation FAILED" );
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
  osThreadResume( stMacLayerTaskHandle );
}

/**
  * @brief  MAC Layer set Task.
  * @param  None
  * @retval None
  */
void MacSys_SemaphoreSet(void)
{
  if ( osSemaphoreGetCount( stMacLayerTaskSemaphore ) == 0 )
  {
    osSemaphoreRelease( stMacLayerTaskSemaphore );
  }
}

/**
  * @brief  MAC Layer Task wait.
  * @param  None
  * @retval None
  */
void MacSys_SemaphoreWait( void )
{
  osSemaphoreAcquire( stMacLayerTaskSemaphore, osWaitForever );
}

/**
  * @brief  MAC Layer set Event.
  * @param  None
  * @retval None
  */
void MacSys_EventSet( void )
{
  if ( osSemaphoreGetCount( stMacLayerEventSemaphore ) == 0 )
  {
    osSemaphoreRelease( stMacLayerEventSemaphore );
  }
}

/**
  * @brief  MAC Layer wait Event.
  * @param  None
  * @retval None
  */
void MacSys_EventWait( void )
{
  osSemaphoreAcquire( stMacLayerEventSemaphore, osWaitForever );
}

