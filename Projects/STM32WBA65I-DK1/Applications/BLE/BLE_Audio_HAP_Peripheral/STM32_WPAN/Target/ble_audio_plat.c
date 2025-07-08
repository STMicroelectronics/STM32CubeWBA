/**
  ******************************************************************************
  * @file    ble_audio_plat.c
  * @author  MCD Application Team
  * @brief   This file implements the platform functions for BLE Audio Profiles
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
#include "stm32wbaxx_hal.h"
#include "stdio.h"
#include "ble_audio_plat.h"
#include "simple_nvm_arbiter.h"
#include "baes.h"
#include "hw.h"
#include "stm32_timer.h"
#include "log_module.h"
#include "ble_dbg_conf.h"

/* Private typedef -----------------------------------------------------------*/
/*---------------------------------------------------------------------------
 * TimerFunc type
 *     Type for a timer notification callback. The notification function
 *     is called to indicate that the timer has fired.
 */
typedef void (*TimerFunc)(void *params);
/* Private defines -----------------------------------------------------------*/
/**
 * Enable or Disable traces from BLE Audio Stack
 */
#define BLE_DBG_AUDIO_EN		(0u)

#define SYS_MAX_MSG     		(255u)

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Function callback to call when timer has fired */
TimerFunc BLE_AUDIO_TimerFunction;

/* Parameter of the function callback */
void *AUDIO_TimerParam;

/*Timer used by the Basic Audio Profile Host*/
static UTIL_TIMER_Object_t AUDIO_TimerObj;

/* Private functions prototypes-----------------------------------------------*/
static void BLE_AUDIO_TimerCallback(void *arg);
static void BLE_AUDIO_NvmCallback (SNVMA_Callback_Status_t CbkStatus);
/* Functions Definition ------------------------------------------------------*/

/*****************************************************************************/


void BLE_AUDIO_PLAT_NvmStore( const uint32_t* ptr, uint32_t size )
{
  UNUSED(ptr);
  UNUSED(size);
#if (CFG_LOG_SUPPORTED != 0)
  SNVMA_Cmd_Status_t status;
#endif
  /* Call SNVMA for storing - Without callback */
#if (CFG_LOG_SUPPORTED != 0)
  status =
#endif
  SNVMA_Write (APP_AUDIO_NvmBuffer,BLE_AUDIO_NvmCallback);
  LOG_INFO_APP("SNVMA_Write() for AUDIO NVM Buffer returns status 0x%02X\n",status);
}

/*****************************************************************************/

void BLE_AUDIO_PLAT_AesEcbEncrypt( const uint8_t* key,
                            const uint8_t* input,
                            uint8_t* output )
{
  BAES_EcbCrypt( key, input, output, 1 );
}

/*****************************************************************************/

void BLE_AUDIO_PLAT_AesCmacSetKey( const uint8_t* key )
{
  BAES_CmacSetKey( key );
}

/*****************************************************************************/

void BLE_AUDIO_PLAT_AesCmacCompute( const uint8_t* input,
                                   uint32_t input_length,
                                   uint8_t* output_tag )
{
  BAES_CmacCompute( input, input_length, output_tag );
}

/*****************************************************************************/

void BLE_AUDIO_PLAT_RngGet( uint8_t n, uint32_t* val )
{
  /* Read 32-bit random values from HW driver */
  HW_RNG_Get( n, val );
}

/*****************************************************************************/

void BLE_AUDIO_PLAT_TimerStart(void *pCallbackFunc, void *pParam, uint32_t Timeout)
{

  /* Save the parameters */
  BLE_AUDIO_TimerFunction = (TimerFunc )pCallbackFunc;
  AUDIO_TimerParam = pParam;

  UTIL_TIMER_Create(&AUDIO_TimerObj,Timeout,UTIL_TIMER_ONESHOT,&BLE_AUDIO_TimerCallback, 0);

  UTIL_TIMER_Start(&AUDIO_TimerObj);

}

/*****************************************************************************/

void BLE_AUDIO_PLAT_TimerStop(void)
{
   UTIL_TIMER_Stop(&AUDIO_TimerObj);
}

/*****************************************************************************/

uint32_t BLE_AUDIO_PLAT_TimerGetRemainMs(void)
{

  uint32_t elapsedTime = 0;

  UTIL_TIMER_GetRemainingTime(&AUDIO_TimerObj, &elapsedTime);

  return elapsedTime;
}

/*****************************************************************************/

void BLE_AUDIO_PLAT_DbgLog(char *format,...)
{
#if (BLE_DBG_AUDIO_EN != 0)
  char msg[SYS_MAX_MSG];

  va_list args;

  va_start(args, format );
  vsnprintf(msg, SYS_MAX_MSG-1, format, args);
  va_end(args);

  Log_Module_Print( LOG_VERBOSE_INFO, LOG_REGION_APP, msg);
#endif /* (BLE_DBG_AUDIO_EN != 0) */
}

/* Private functions ----------------------------------------------------------*/

static void BLE_AUDIO_TimerCallback(void *arg)
{
  //Call timer Callback of the Audio Stack
  BLE_AUDIO_TimerFunction(AUDIO_TimerParam);
}


static void BLE_AUDIO_NvmCallback (SNVMA_Callback_Status_t CbkStatus)
{
  if (CbkStatus != SNVMA_OPERATION_COMPLETE)
  {
#if (CFG_LOG_SUPPORTED != 0)
    SNVMA_Cmd_Status_t status;
#endif
  /* Call SNVMA for storing - Without callback */
#if (CFG_LOG_SUPPORTED != 0)
    status =
#endif
    /* Retry the write operation */
    SNVMA_Write (APP_AUDIO_NvmBuffer,BLE_AUDIO_NvmCallback);
    LOG_INFO_APP("SNVMA_Write() for AUDIO NVM Buffer returns status 0x%02X\n",status);
  }
}