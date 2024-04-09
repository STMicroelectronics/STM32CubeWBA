/**
 ******************************************************************************
 * @file    app_thread.c
 * @author  MCD Application Team
 * @version
 * @date
 * @brief   This file contains OpenThread stack Init functions.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2021 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <assert.h>
#include "stm32wbaxx_hal.h"
#include "app_conf.h"
#include "main.h"
#include "cli.h"
#include "radio.h"
#include "platform.h"
#include "tasklet.h"
#include "stm32_timer.h"
#include "stm32_seq.h"
#include "platform_wba.h"
#include "instance.h"
#include "thread.h"
#include "ll_sys_startup.h"
#include "app_thread.h"
#include OPENTHREAD_CONFIG_FILE

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32wbaxx_nucleo.h"
/* USER CODE END Includes */



/* Private defines -----------------------------------------------------------*/
#define C_CCA_THRESHOLD         -70

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */


/* Private variables ---------------------------------------------------------*/
static otInstance *PtOpenThreadInstance;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void APP_THREAD_DeviceConfig(void);
static void APP_THREAD_TraceError(const char * pMess, uint32_t ErrCode);


/* USER CODE BEGIN PFP */
static void APP_THREAD_RCPInit(otInstance *aInstance);
/* USER CODE END PFP */


/* Functions Definition ------------------------------------------------------*/
void APP_THREAD_ScheduleAlarm(void)
{
  UTIL_SEQ_SetTask( 1U<< CFG_TASK_OT_ALARM, CFG_TASK_PRIO_OT_ALARM);
}

void APP_THREAD_ScheduleUsAlarm(void)
{
  UTIL_SEQ_SetTask( 1U<< CFG_TASK_OT_US_ALARM, CFG_TASK_PRIO_OT_US_ALARM);
}

void Thread_Init(void)
{

  otSysInit(0, NULL);


  PtOpenThreadInstance = otInstanceInitSingle();

  assert(PtOpenThreadInstance);


  APP_THREAD_RCPInit(PtOpenThreadInstance);

  otDispatch_tbl_init(PtOpenThreadInstance);

  /* Register tasks */
  UTIL_SEQ_RegTask(1<<CFG_TASK_OT_ALARM, UTIL_SEQ_RFU, ProcessAlarm);
  
  UTIL_SEQ_RegTask(1<<CFG_TASK_OT_US_ALARM, UTIL_SEQ_RFU, ProcessUsAlarm);
  
  UTIL_SEQ_RegTask(1<<CFG_TASK_OT_TASKLETS, UTIL_SEQ_RFU, ProcessOpenThreadTasklets);

  ll_sys_thread_init();

  /* Run first time */
  UTIL_SEQ_SetTask( 1U<< CFG_TASK_OT_ALARM, CFG_TASK_PRIO_OT_ALARM);
}

void ProcessAlarm(void)
{
  arcAlarmProcess(PtOpenThreadInstance);
}

void ProcessUsAlarm(void)
{
  arcUsAlarmProcess(PtOpenThreadInstance);
}

void ProcessTasklets(void)
{
  if (otTaskletsArePending(PtOpenThreadInstance) == TRUE) {
    UTIL_SEQ_SetTask( 1U<< CFG_TASK_OT_TASKLETS, CFG_SEQ_PRIO_0);
  }
}

/**
 * @brief  ProcessOpenThreadTasklets.
 * @param  None
 * @param  None
 * @retval None
 */
void ProcessOpenThreadTasklets(void)
{
  /* wakeUp the system */
  //ll_sys_radio_hclk_ctrl_req(LL_SYS_RADIO_HCLK_LL_BG, LL_SYS_RADIO_HCLK_ON);
  //ll_sys_dp_slp_exit();
  /* process the tasklet */
  otTaskletsProcess(PtOpenThreadInstance);
  /* Put the IP802_15_4 back to sleep mode */
  //ll_sys_radio_hclk_ctrl_req(LL_SYS_RADIO_HCLK_LL_BG, LL_SYS_RADIO_HCLK_OFF);

  /* Reschedule the tasklets if any */
  ProcessTasklets();
}

/**
 * OpenThread calls this function when the tasklet queue transitions from empty to non-empty.
 *
 * @param[in] aInstance A pointer to an OpenThread instance.
 */
void otTaskletsSignalPending(otInstance *aInstance)
{
  UTIL_SEQ_SetTask( 1U<< CFG_TASK_OT_TASKLETS, CFG_TASK_PRIO_OT_TASKLETS);
}

void APP_THREAD_Init( void )
{
  Thread_Init();

  APP_THREAD_DeviceConfig();
}

/**
 * @brief Thread initialization.
 * @param  None
 * @retval None
 */
static void APP_THREAD_DeviceConfig(void)
{
  otError error = OT_ERROR_NONE;
  otExtAddress ext_addr;

  otPlatRadioGetIeeeEui64(PtOpenThreadInstance, ext_addr.m8);

  otPlatRadioSetExtendedAddress(PtOpenThreadInstance, &ext_addr);
  
  otPlatRadioEnableSrcMatch(PtOpenThreadInstance, true);

  error = otPlatRadioSetCcaEnergyDetectThreshold(PtOpenThreadInstance, C_CCA_THRESHOLD);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_SET_THRESHOLD,error);
  }

  /* USER CODE END DEVICECONFIG */
}


/**
 * @brief  Warn the user that an error has occurred.In this case,
 *         the LEDs on the Board will start blinking.
 *
 * @param  pMess  : Message associated to the error.
 * @param  ErrCode: Error code associated to the module (OpenThread or other module if any)
 * @retval None
 */
static void APP_THREAD_TraceError(const char * pMess, uint32_t ErrCode)
{
  /* USER CODE BEGIN TRACE_ERROR */
  APP_DBG("**** Fatal error = %s (Err = %d)", pMess, ErrCode);
  while(1U == 1U)
  {
#if (CFG_LED_SUPPORTED == 1)
    BSP_LED_Toggle(LD1);
    HAL_Delay(500U);
    BSP_LED_Toggle(LD2);
    HAL_Delay(500U);
    BSP_LED_Toggle(LD3);
    HAL_Delay(500U);
#endif
  }
  /* USER CODE END TRACE_ERROR */
}

/**
 * @brief  Trace the error or the warning reported.
 * @param  ErrId :
 * @param  ErrCode
 * @retval None
 */
void APP_THREAD_Error(uint32_t ErrId, uint32_t ErrCode)
{
  /* USER CODE BEGIN APP_THREAD_Error_1 */

  /* USER CODE END APP_THREAD_Error_1 */
  switch(ErrId)
  {
  case ERR_THREAD_SET_THRESHOLD:
    APP_THREAD_TraceError("SET CCA Theshold", ErrCode);
    break;
  case ERR_THREAD_SET_EXTADDR:
    APP_THREAD_TraceError("SET Extended Address", ErrCode);
    break;    
    /* USER CODE BEGIN APP_THREAD_Error_2 */

    /* USER CODE END APP_THREAD_Error_2 */
  default :
    APP_THREAD_TraceError("ERROR Unknown ", 0);
    break;
  }
}


void app_logger_write(uint8_t *buffer, uint32_t size)
{
  //UTIL_ADV_TRACE_COND_Send(VLEVEL_ALWAYS, ~0x0, 0, buffer, (uint16_t)size);
}

static void APP_THREAD_RCPInit(otInstance *aInstance)
{
  otAppNcpInit(aInstance);
}

/* USER CODE END FD_LOCAL_FUNCTIONS */
