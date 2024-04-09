/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ll_sys_if.c
  * @author  MCD Application Team
  * @brief   Source file for initiating system
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

#include "app_common.h"
#include "main.h"
#include "ll_intf.h"
#include "ll_sys.h"
#include "ll_sys_if.h"
#include "stm32_rtos.h"

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/

/* Redefine access to Low Level API to maintain compatibility */
extern uint32_t llhwc_cmn_sys_configure_ll_ctx          (uint8_t param1, uint8_t param2);
extern uint8_t  ll_tx_pwr_if_select_tx_power_mode       (uint8_t param1);

#define ll_intf_config_ll_ctx_params(A, B)              llhwc_cmn_sys_configure_ll_ctx(A, B)
#define ll_intf_select_tx_power_table(A)                ll_tx_pwr_if_select_tx_power_mode(A)

/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private constants ---------------------------------------------------------*/
/* USER CODE BEGIN PC */

/* USER CODE END PC */

/* Private variables ---------------------------------------------------------*/
/* Link Layer Task related resources */
static TX_SEMAPHORE     LinkLayerSemaphore;
static TX_THREAD        LinkLayerThread;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/
/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private functions prototypes-----------------------------------------------*/

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Functions Definition ------------------------------------------------------*/

/**
 * @brief  Link Layer Task for ThreadX
 * @param  None
 * @retval None
 */
static void ll_sys_bg_process_task( ULONG thread_input )
{
  UNUSED( thread_input );

  for(;;)
  {
    tx_semaphore_get( &LinkLayerSemaphore, TX_WAIT_FOREVER );
    ll_sys_bg_process();
    tx_thread_relinquish();
  }
}

/**
  * @brief  Link Layer background process initialization
  * @param  None
  * @retval None
  */
void ll_sys_bg_process_init(void)
{
  UINT  ThreadXStatus;
  CHAR  * pStack = TX_NULL;

  /* Register LinkLayer Semaphore */
  ThreadXStatus = tx_semaphore_create( &LinkLayerSemaphore, "LinkLayerSem", 0 );
  if ( ThreadXStatus != TX_SUCCESS )
  {
    LOG_ERROR_APP( "ERROR THREADX : LINK LAYER SEMAPHORE CREATION FAILED (%d)", ThreadXStatus );
    Error_Handler();
  }
  
  
  /* Thread associated with LinkLayer Task */
  ThreadXStatus = tx_byte_allocate( pBytePool, (VOID**) &pStack, TASK_LINK_LAYER_STACK_SIZE, TX_NO_WAIT );
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus = tx_thread_create( &LinkLayerThread, "LinkLayerThread", ll_sys_bg_process_task, 0, pStack,
                                      TASK_LINK_LAYER_STACK_SIZE, CFG_TASK_PRIO_LINK_LAYER, CFG_TASK_PREEMP_LINK_LAYER,
                                      TX_NO_TIME_SLICE, TX_AUTO_START );
  }
  if ( ThreadXStatus != TX_SUCCESS )
  {
    LOG_ERROR_APP( "ERROR THREADX : LINK LAYER THREAD CREATION FAILED (%d)", ThreadXStatus );
    Error_Handler();
  }
}

/**
  * @brief  Link Layer background process next iteration scheduling
  * @param  None
  * @retval None
  */
void ll_sys_schedule_bg_process(void)
{
  if ( LinkLayerSemaphore.tx_semaphore_count == 0 )
  {
    tx_semaphore_put( &LinkLayerSemaphore );
  }
}

/**
  * @brief  Link Layer background process next iteration scheduling from ISR
  * @param  None
  * @retval None
  */
void ll_sys_schedule_bg_process_isr(void)
{
  if ( LinkLayerSemaphore.tx_semaphore_count == 0 )
  {
    tx_semaphore_put( &LinkLayerSemaphore );
  }
}

/**
  * @brief  Link Layer configuration phase before application startup.
  * @param  None
  * @retval None
  */
void ll_sys_config_params(void)
{
  /* Configure link layer behavior for low ISR use and next event scheduling method:
   * - SW low ISR is used.
   * - Next event is scheduled from ISR.
   */
  ll_intf_config_ll_ctx_params(USE_RADIO_LOW_ISR, NEXT_EVENT_SCHEDULING_FROM_ISR);

  /* Link Layer power table */
  ll_intf_select_tx_power_table(CFG_RF_TX_POWER_TABLE_ID);
}

