/*
 *  Copyright (c) 2018, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements the OpenThread platform abstraction for the alarm.
 *
 */

#include OPENTHREAD_PROJECT_CORE_CONFIG_FILE
#include "openthread/platform/alarm-milli.h"
#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
#include "openthread/platform/alarm-micro.h"
#endif
#include "platform_wba.h"
#include "bsp.h"
#include "cmsis_compiler.h"

#define OT_INSTANCE_NUMBER                1
#define OT_ALARM_MILLI_MAX_EVENT          1
#define OT_ALARM_MICRO_MAX_EVENT          1
#define NO_STORED_INSTANCE                255

#define FROM_US_TO_MS(ms)   (ms/1000)
#define FROM_MS_TO_US(us)   (us*1000)
#define MIN(A, B) (((A) < (B)) ? (A) : (B))

extern void ral_set_ot_base_slp_time_value(uint32_t time);
extern uint64_t ral_cnvert_slp_tim_to_ot_tim(uint32_t time);
extern uint32_t get_current_time(void);

static void ms_alarm_timer_cbk(void *arg);
static void us_alarm_timer_cbk(void *arg);

uint32_t sCounter_steps = 0;

typedef struct ms_alarm_struct
{
  uint32_t expires;
  uint8_t sIsRunning;
} ms_alarm_struct;

typedef struct csl_alarm_struct
{
  otInstance *aInstance;
  uint32_t expires;
  uint8_t sIsRunning;
}csl_alarm_struct;

ms_alarm_struct  milli_alarm_struct_st[OT_ALARM_MILLI_MAX_EVENT];
csl_alarm_struct micro_alarm_struct_st[OT_INSTANCE_NUMBER];
uint8_t milli_alarm_pending;
uint8_t micro_alarm_pending;

static os_timer_id csl_timer_id;
static os_timer_id milli_timer_id;

/* stubs for external functions 
   prevent linking errors when not defined in application */
__WEAK void APP_THREAD_ScheduleAlarm(void)
{
  while(1);
}

__WEAK void APP_THREAD_ScheduleUsAlarm(void)
{
  while(1);
}

void arcAlarmInit(void)
{
  uint8_t i;
  milli_alarm_pending = 0U;

  milli_timer_id = os_timer_create((void (*)())ms_alarm_timer_cbk, os_timer_once, NULL);
  for (i = 0U; i < OT_INSTANCE_NUMBER ; i++){
    milli_alarm_struct_st[i].sIsRunning = 0;
  }

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
  micro_alarm_pending = 0;
  csl_timer_id = os_timer_create((void (*)())us_alarm_timer_cbk, os_timer_once, NULL);
  for (i = 0U; i < OT_INSTANCE_NUMBER ; i++){
    micro_alarm_struct_st[i].sIsRunning = 0;
  }
#endif /* OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE */

  sCounter_steps = get_current_time();
  ral_set_ot_base_slp_time_value(sCounter_steps);
}

/* Used by RCP for CSL latency transport (UART, SPI etc..) offset calculation */
uint32_t otPlatTimeGet(void)
{
  return otPlatAlarmMicroGetNow();
}

uint32_t otPlatAlarmMilliGetNow(void)
{
  uint32_t steps = get_current_time();
  return  (uint32_t)FROM_US_TO_MS(ral_cnvert_slp_tim_to_ot_tim(steps));
}

void otPlatAlarmMilliStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
  uint64_t alarm_time_us = 0;
  uint64_t ot_base_us = 0;
  uint32_t curr_time= get_current_time();
  uint32_t ret = 0;
    
  uint32_t alarm_expire_ms = (uint32_t)(aT0 + aDt);
  
  os_timer_stop(milli_timer_id); //stop alarm
  
  milli_alarm_struct_st[0].expires = alarm_expire_ms;
  milli_alarm_struct_st[0].sIsRunning = 1;

  /* Schedule Alarm */
  alarm_time_us = FROM_MS_TO_US((uint64_t)alarm_expire_ms);
  ot_base_us    = ral_cnvert_slp_tim_to_ot_tim(curr_time);
  if(alarm_time_us < ot_base_us) //alarm in past
  {
    //schedule alarm right now
    APP_THREAD_ScheduleAlarm();
  }
  else
  {
    alarm_time_us -= ot_base_us;
    if (alarm_time_us> 0xFFFFFFFF) // os_timer only deal with uint32_t
    {
      alarm_time_us = 0x00000000FFFFFFFF;
    }
    
    ret = os_timer_start_in_us(milli_timer_id, (uint32_t)alarm_time_us);
    if (ret != 0)
    {
      bsp_assert(0,1);
    }
  }
}

void otPlatAlarmMilliStop(otInstance *aInstance)
{
  milli_alarm_struct_st[0].sIsRunning = 0;
  os_timer_stop(milli_timer_id);
}

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
void otPlatAlarmMicroStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
  (void)(aInstance);
  uint32_t ret = 0;
  uint32_t curr_time= get_current_time();
  uint32_t alarm_expire_us = (uint32_t)(aT0 + aDt);

  os_timer_stop(csl_timer_id); //stop alarm
  
  micro_alarm_struct_st[0].sIsRunning = 1;
  micro_alarm_struct_st[0].expires = alarm_expire_us;

  /* Schedule Alarm */
  ret = os_timer_start_in_us(csl_timer_id, alarm_expire_us - ral_cnvert_slp_tim_to_ot_tim(curr_time));
   if (ret != 0)
  {
    bsp_assert(0,1);
  }
}

void otPlatAlarmMicroStop(otInstance *aInstance)
{
  micro_alarm_struct_st[0].sIsRunning = 0;
  os_timer_stop(csl_timer_id);
}

uint32_t otPlatAlarmMicroGetNow(void)
{
  uint32_t steps = get_current_time();
  return (uint32_t)ral_cnvert_slp_tim_to_ot_tim(steps);
}

static uint32_t arcMicroAlarmGetRemainingTime(void)
{
  uint32_t rem_time;

  if (micro_alarm_struct_st[0].sIsRunning == 1)
  {
    uint32_t alarm_time = micro_alarm_struct_st[0].expires;
    uint32_t curr_time = otPlatAlarmMicroGetNow();
    rem_time = (alarm_time < curr_time) ? 0 : (alarm_time - curr_time);
  }
  else
  {
    rem_time = NO_EVT_TIME;
  }
  return rem_time;
}

#endif /* OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE */

static uint32_t arcMilliAlarmGetRemainingTime(void)
{
  uint32_t rem_time;

  if (milli_alarm_struct_st[0].sIsRunning == 1)
  {
    uint32_t alarm_time = milli_alarm_struct_st[0].expires;
    uint32_t curr_time = otPlatAlarmMilliGetNow();
    rem_time = (alarm_time < curr_time) ? 0 : (alarm_time - curr_time);
  }
  else
  {
    rem_time = NO_EVT_TIME;
  }
  return rem_time;
}

/**
 * @brief     This function returns the next alarm event in us.
 * @param[in] None
 * @retval    Relative time until next alarm in us. (NO_EVT_TIME if none)
 */
uint64_t arcAlarmGetRemainingTimeNextEvent(void)
{
  uint64_t retval = (uint64_t)arcMilliAlarmGetRemainingTime();
  /* Convert in us */
  retval = FROM_MS_TO_US(retval);
#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
  uint64_t alarm_expires_us = (uint64_t)arcMicroAlarmGetRemainingTime();
  retval = MIN(retval, alarm_expires_us);
#endif /* #if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE */
  return retval;
}

static void ms_alarm_timer_cbk(void *arg)
{
  APP_THREAD_ScheduleAlarm();
}

static void us_alarm_timer_cbk(void *arg)
{
  APP_THREAD_ScheduleUsAlarm();
}

void arcAlarmProcess(otInstance *aInstance)
{
  if (milli_alarm_struct_st[0].sIsRunning == 1)
  {
    milli_alarm_struct_st[0].sIsRunning = 0;
    otPlatAlarmMilliFired(aInstance);
  }
}

void arcUsAlarmProcess(otInstance *aInstance)
{
  if (micro_alarm_struct_st[0].sIsRunning == 1)
  {
    micro_alarm_struct_st[0].sIsRunning = 0;
    otPlatAlarmMicroFired(aInstance);
  }
}