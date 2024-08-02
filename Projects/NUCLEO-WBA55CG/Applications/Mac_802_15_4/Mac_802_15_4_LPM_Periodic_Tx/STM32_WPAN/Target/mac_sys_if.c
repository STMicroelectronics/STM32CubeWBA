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
#include "stm32_seq.h"
#include "st_mac_802_15_4_core.h"

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
  * @brief  Mac Layer Sequencer Initialisation
  * @param  None
  * @retval None
  */
void MacSys_Init(void)
{
  /* Register tasks */
  UTIL_SEQ_RegTask( TASK_MAC_LAYER, UTIL_SEQ_RFU, mac_baremetal_run);
}


/**
  * @brief  Mac Layer Sequencer Resume
  * @param  None
  * @retval None
  */
void MacSys_Resume(void)
{
  UTIL_SEQ_ResumeTask( TASK_MAC_LAYER );
}


/**
  * @brief  MAC Layer set Task. 
  * @param  None
  * @retval None
  */
void MacSys_SemaphoreSet(void)
{
  UTIL_SEQ_SetTask( TASK_MAC_LAYER, TASK_MAC_LAYER_PRIO );
}


/**
  * @brief  MAC Layer Task wait. Not used with Sequencer.
  * @param  None
  * @retval None
  */
void MacSys_SemaphoreWait( void )
{
}

/* Not used in this application, Zigbee layer */
/**
  * @brief  MAC Layer set Event. 
  * @param  None
  * @retval None
  */
void MacSys_EventSet( void )
{
}


/**
  * @brief  MAC Layer wait Event. 
  * @param  None
  * @retval None
  */
void MacSys_EventWait( void )
{
}