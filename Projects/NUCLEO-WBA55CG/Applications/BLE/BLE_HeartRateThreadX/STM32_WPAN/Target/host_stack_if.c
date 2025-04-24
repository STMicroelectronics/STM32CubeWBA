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
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables --------------------------------------------------------*/
/**
  * @brief  Missed HCI event flag
  */
extern uint8_t missed_hci_event_flag;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* External function prototypes -----------------------------------------------*/
/* USER CODE BEGIN EFP */
#undef BLE_WRAP_PREPROC
#undef BLE_WRAP_POSTPROC
/* USER CODE END EFP */

/* Release Link Layer Mutex before calling any aci/hci functions */
#define BLE_WRAP_PREPROC tx_mutex_get(&LinkLayerMutex, TX_WAIT_FOREVER)

/* Release Link Layer Mutex and Trigger BLE Host stack process after calling any aci/hci functions */
#define BLE_WRAP_POSTPROC do{ \
                              tx_mutex_put(&LinkLayerMutex);  \
                              BleStackCB_Process(); \
                            }while(0)

/**
  * @brief  Host stack processing request from Link Layer.
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
  tx_semaphore_put(&BleHostSemaphore);

  /* USER CODE BEGIN BleStackCB_Process 1 */

  /* USER CODE END BleStackCB_Process 1 */
}
