/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    zigbee_sys_if.c
  * @author  MCD Application Team
  * @brief   Source file for using Zigbee Layer with a RTOS
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
#include "app_zigbee.h"
#include "stm32_rtos.h"
#include "zigbee.stm32wba.sys.h"

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* FreeRTOS objects declaration */

static osThreadId_t     stZigbeeLayerTaskHandle;
static osSemaphoreId_t  stZigbeeLayerSemaphore;

/* FreeRtos stacks attributes */
const osThreadAttr_t stZigbeeLayerTask_attributes =
{
  .name         = "ZigbeeLayer Task",
  .priority     = TASK_PRIO_ZIGBEE_LAYER,
  .stack_size   = TASK_STACK_SIZE_ZIGBEE_LAYER,
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE,
  .stack_mem    = TASK_DEFAULT_STACK_MEM
};

const osSemaphoreAttr_t  stZigbeeLayerSemaphore_attributes =
{
  .name         = "ZigbeeLayer Semaphore",
  .attr_bits    = SEMAPHORE_DEFAULT_ATTR_BITS,
  .cb_mem       = SEMAPHORE_DEFAULT_CB_MEM,
  .cb_size      = SEMAPHORE_DEFAULT_CB_SIZE,
};

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/
/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Functions Definition ------------------------------------------------------*/

/**
 * @brief  Zigbee Layer Task for FreeRTOS
 * @param  void *argument
 * @retval None
 */
static void ZigbeeSys_Task_Entry( void * argument )
{
  UNUSED( argument );

  for (;;)
  {
    osSemaphoreAcquire( stZigbeeLayerSemaphore, osWaitForever );
    APP_ZIGBEE_Task();
    osThreadYield();
  }
}

/**
  * @brief  Zigbee Layer Initialisation
  * @param  None
  * @retval None
  */
void ZigbeeSys_Init(void)
{
  /* Create Zigbee Layer FreeRTOS objects */
  stZigbeeLayerSemaphore = osSemaphoreNew( 1U, 0U, &stZigbeeLayerSemaphore_attributes );

  stZigbeeLayerTaskHandle = osThreadNew( ZigbeeSys_Task_Entry, NULL, &stZigbeeLayerTask_attributes );

  if ( ( stZigbeeLayerTaskHandle == NULL ) || ( stZigbeeLayerSemaphore == NULL ) )
  {
    LOG_ERROR_APP( "Zigbee Layer FreeRTOS objects creation FAILED" );
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
  osThreadResume( stZigbeeLayerTaskHandle );
}

/**
  * @brief  Zigbee Layer set Task.
  * @param  None
  * @retval None
  */
void ZigbeeSys_SemaphoreSet(void)
{
  if ( osSemaphoreGetCount ( stZigbeeLayerSemaphore ) == 0 )
  {
    osSemaphoreRelease( stZigbeeLayerSemaphore );
  }
}

/**
  * @brief  Zigbee Layer Task wait. Not used with Sequencer.
  * @param  None
  * @retval None
  */
void ZigbeeSys_SemaphoreWait( void )
{
  osSemaphoreAcquire( stZigbeeLayerSemaphore, osWaitForever );
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

