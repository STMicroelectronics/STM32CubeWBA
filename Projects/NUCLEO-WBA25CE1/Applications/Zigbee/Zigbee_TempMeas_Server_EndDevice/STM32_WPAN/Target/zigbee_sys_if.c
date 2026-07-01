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
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/
/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  Zigbee Layer Process Task
  * @param  None
  * @retval None
  */
void ZigbeeSys_Process(void)
{
  APP_ZIGBEE_Task();
}

/**
  * @brief  Zigbee Layer Initialisation
  * @param  None
  * @retval None
  */
void ZigbeeSys_Init(void)
{
  /* Register tasks */
  UTIL_SEQ_RegTask( TASK_ZIGBEE_LAYER, UTIL_SEQ_RFU, ZigbeeSys_Process);
  UTIL_SEQ_SetTask( TASK_ZIGBEE_LAYER, TASK_PRIO_ZIGBEE_LAYER );

  /* Register Event is not needed */
}

/**
  * @brief  Zigbee Layer Task Resume
  * @param  None
  * @retval None
  */
void ZigbeeSys_Resume(void)
{
  UTIL_SEQ_ResumeTask( TASK_ZIGBEE_LAYER );
}

/**
  * @brief  Zigbee Layer set Task.
  * @param  None
  * @retval None
  */
void ZigbeeSys_SemaphoreSet(void)
{
  UTIL_SEQ_SetTask( TASK_ZIGBEE_LAYER, TASK_PRIO_ZIGBEE_LAYER );
}

/**
  * @brief  Zigbee Layer Task wait. Not used with Sequencer.
  * @param  None
  * @retval None
  */
void ZigbeeSys_SemaphoreWait( void )
{
  /* Not used */
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

