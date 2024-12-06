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

#include "log_module.h"
/* USER CODE END Header */

#include "main.h"
#include "app_common.h"
#include "app_conf.h"
#include "log_module.h"
#include "ll_intf_cmn.h"
#include "ll_sys.h"
#include "ll_sys_if.h"
#include "stm32_rtos.h"
#include "utilities_common.h"

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private constants ---------------------------------------------------------*/
/* USER CODE BEGIN PC */

/* USER CODE END PC */

/* Private variables ---------------------------------------------------------*/
/* ThreadX objects declaration */

static TX_THREAD        LinkLayerTaskHandle;
static TX_SEMAPHORE     LinkLayerSemaphore;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/

/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private functions prototypes-----------------------------------------------*/
static void ll_sys_sleep_clock_source_selection(void);
void ll_sys_reset(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Functions Definition ------------------------------------------------------*/
/**
 * @brief  Link Layer Task for ThreadX
 * @param  ULONG lArgument
 * @retval None
 */
static void LinkLayer_Task_Entry( ULONG lArgument )
{
  UNUSED(lArgument);

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
  UINT TXstatus;
  CHAR *pStack;

  /* Create Link Layer ThreadX objects */

  TXstatus = tx_byte_allocate(pBytePool, (void **)&pStack, TASK_STACK_SIZE_LINK_LAYER, TX_NO_WAIT);

  if( TXstatus == TX_SUCCESS )
  {
    TXstatus = tx_thread_create(&LinkLayerTaskHandle, "Link Layer  Task", LinkLayer_Task_Entry, 0,
                                 pStack, TASK_STACK_SIZE_LINK_LAYER,
                                 TASK_PRIO_LINK_LAYER, TASK_PREEMP_LINK_LAYER,
                                 TX_NO_TIME_SLICE, TX_AUTO_START);

    TXstatus |= tx_semaphore_create(&LinkLayerSemaphore, "Link Layer Semaphore", 0);
  }

  if( TXstatus != TX_SUCCESS )
  {
    LOG_ERROR_APP( "Link Layer ThreadX objects creation FAILED, status: %d", TXstatus);
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
/* USER CODE BEGIN ll_sys_config_params_0 */

/* USER CODE END ll_sys_config_params_0 */

  /* Configure link layer behavior for low ISR use and next event scheduling method:
   * - SW low ISR is used.
   * - Next event is scheduled from ISR.
   */
  ll_intf_cmn_config_ll_ctx_params(USE_RADIO_LOW_ISR, NEXT_EVENT_SCHEDULING_FROM_ISR);
  /* Apply the selected link layer sleep timer source */
  ll_sys_sleep_clock_source_selection();

/* USER CODE BEGIN ll_sys_config_params_1 */

/* USER CODE END ll_sys_config_params_1 */

  /* Link Layer power table */
  ll_intf_cmn_select_tx_power_table(CFG_RF_TX_POWER_TABLE_ID);
/* USER CODE BEGIN ll_sys_config_params_2 */

/* USER CODE END ll_sys_config_params_2 */
}

void ll_sys_sleep_clock_source_selection(void)
{
  uint16_t freq_value = 0;
  uint32_t linklayer_slp_clk_src = LL_RCC_RADIOSLEEPSOURCE_NONE;

  linklayer_slp_clk_src = LL_RCC_RADIO_GetSleepTimerClockSource();
  switch(linklayer_slp_clk_src)
  {
    case LL_RCC_RADIOSLEEPSOURCE_LSE:
      linklayer_slp_clk_src = RTC_SLPTMR;
      break;

    case LL_RCC_RADIOSLEEPSOURCE_LSI:
      linklayer_slp_clk_src = RCO_SLPTMR;
      break;

    case LL_RCC_RADIOSLEEPSOURCE_HSE_DIV1000:
      linklayer_slp_clk_src = CRYSTAL_OSCILLATOR_SLPTMR;
      break;

    case LL_RCC_RADIOSLEEPSOURCE_NONE:
      /* No Link Layer sleep clock source selected */
      assert_param(0);
      break;
  }
  ll_intf_cmn_le_select_slp_clk_src((uint8_t)linklayer_slp_clk_src, &freq_value);
}

void ll_sys_reset(void)
{
/* USER CODE BEGIN ll_sys_reset_0 */

/* USER CODE END ll_sys_reset_0 */
#if (CFG_RADIO_LSE_SLEEP_TIMER_CUSTOM_SCA_RANGE == 0)
  uint8_t bsca = 0;
#endif /* CFG_RADIO_LSE_SLEEP_TIMER_CUSTOM_SCA_RANGE */

  /* Apply the selected link layer sleep timer source */
  ll_sys_sleep_clock_source_selection();

  /* Configure the link layer sleep clock accuracy if different from the default one */
#if (CFG_RADIO_LSE_SLEEP_TIMER_CUSTOM_SCA_RANGE != 0)
  ll_intf_le_set_sleep_clock_accuracy(CFG_RADIO_LSE_SLEEP_TIMER_CUSTOM_SCA_RANGE);
#else
  if(bsca != STM32WBA5x_DEFAULT_SCA_RANGE)
  {
    ll_intf_le_set_sleep_clock_accuracy(bsca);
  }
#endif /* CFG_RADIO_LSE_SLEEP_TIMER_CUSTOM_SCA_RANGE */

/* USER CODE BEGIN ll_sys_reset_1 */

/* USER CODE END ll_sys_reset_1 */
}
