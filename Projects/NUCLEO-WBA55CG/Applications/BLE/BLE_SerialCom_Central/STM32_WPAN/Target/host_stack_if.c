/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    host_stack_if.c
  * @author  MCD Application Team
  * @brief : Source file for the stack tasks
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
#include "host_stack_if.h"
#include "app_conf.h"
#include "ll_sys.h"
#include "app_ble.h"
#include "auto/ble_raw_api.h"
#include "stm32_seq.h"
/* External variables --------------------------------------------------------*/
/**
  * @brief  Missed HCI event flag
  */
extern uint8_t missed_hci_event_flag;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* External function prototypes -----------------------------------------------*/
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/**
  * @brief  BLE Host stack processing request.
  * @param  None
  * @retval None
  */
void HostStack_Process(void)
{
  /* USER CODE BEGIN HostStack_Process 0 */

  /* USER CODE END HostStack_Process 0 */

  /* Process BLE Host stack */
  BleStackCB_Process();

  /* USER CODE BEGIN HostStack_Process 1 */

  /* USER CODE END HostStack_Process 1 */
}

/**
  * @brief  BLE Host stack processing callback.
  * @param  None
  * @retval None
  */
void BleStackCB_Process(void)
{
  /* USER CODE BEGIN BleStackCB_Process 0 */

  /* USER CODE END BleStackCB_Process 0 */
  if (missed_hci_event_flag)
  {
    missed_hci_event_flag = 0;
    HCI_HARDWARE_ERROR_EVENT(0x03);
  }
  /* BLE Host stack processing through background task */
  UTIL_SEQ_SetTask( 1U << CFG_TASK_BLE_HOST, CFG_SEQ_PRIO_0);

  /* USER CODE BEGIN BleStackCB_Process 1 */

  /* USER CODE END BleStackCB_Process 1 */
}
