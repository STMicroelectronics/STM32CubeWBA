/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    alarm.c
  * @author  MCD Application Team
  * @brief   This file implements the OpenThread platform abstraction for the alarm.
  *
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

#include OPENTHREAD_PROJECT_CORE_CONFIG_FILE
#include "openthread/platform/alarm-milli.h"
#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
#include "openthread/platform/alarm-micro.h"
#endif
#include "platform_wba.h"
#include "alarm.h"
#include "cmsis_compiler.h"
#include "instance.h"
#include "platform/time.h"
#include "platform/radio.h"
#include "linklayer_plat.h"

#define OT_INSTANCE_NUMBER                1
#define OT_ALARM_MILLI_MAX_EVENT          1
#define OT_ALARM_MICRO_MAX_EVENT          1
#define NO_STORED_INSTANCE                255

#define FROM_US_TO_MS(us)        (us/1000)
#define FROM_MS_TO_US(ms)        (ms*1000)
#define FROM_MS_TO_SLP_STEP(ms)  (ms*32) /* Current Sleep timer has 31.25 us step */


#define MAX_UINT32 0xFFFFFFFF
#define MILLI_START_THRESHOLD 0  //ms, do not start milli timer if <= MILLI_START_THRESHOLD
#define MICRO_START_THRESHOLD 100  //us, do not start micro timer if <= MICRO_START_THRESHOLD

extern void ral_set_ot_base_slp_time_value(uint32_t time);
extern uint32_t get_current_time(void);

static void ms_alarm_timer_cbk(void *arg);
#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
static void us_alarm_timer_cbk(void *arg);
#endif

typedef struct ms_alarm_struct
{
  uint32_t expires;
  uint8_t sIsRunning;
} ms_alarm_struct;

typedef struct csl_alarm_struct
{
  uint32_t expires;
  uint8_t sIsRunning;
}csl_alarm_struct;

static ms_alarm_struct  milli_alarm_struct_st[OT_ALARM_MILLI_MAX_EVENT];
static os_timer_id milli_timer_id;

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
static csl_alarm_struct micro_alarm_struct_st[OT_INSTANCE_NUMBER];
static os_timer_id csl_timer_id;
#endif

static uint32_t notnull_intance;
/* stubs for external functions 
   prevent linking errors when not defined in application */
__WEAK void APP_THREAD_ScheduleAlarm(void)
{
  /* Need to be implemented by user (os dependant) */
  while(1);
}

__WEAK void APP_THREAD_ScheduleUsAlarm(void)
{
  /* Need to be implemented by user (os dependant) */
  while(1);
}

void arcAlarmInit(void)
{
  uint8_t i;

  milli_timer_id = os_timer_create((void (*)())ms_alarm_timer_cbk, os_timer_once, NULL);
  for (i = 0U; i < OT_INSTANCE_NUMBER ; i++){
    milli_alarm_struct_st[i].sIsRunning = 0;
  }

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
  csl_timer_id = os_timer_create((void (*)())us_alarm_timer_cbk, os_timer_once, NULL);
  for (i = 0U; i < OT_INSTANCE_NUMBER ; i++){
    micro_alarm_struct_st[i].sIsRunning = 0;
  }
#endif /* OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE */

  ral_set_ot_base_slp_time_value(get_current_time());
}

/* Used by RCP for CSL latency transport (UART, SPI etc..) offset calculation */
uint64_t otPlatTimeGet(void)
{
  return otPlatRadioGetNow((otInstance *)&notnull_intance);
}

uint32_t otPlatAlarmMilliGetNow(void)
{
  uint32_t time_ms = (uint32_t)FROM_US_TO_MS(otPlatRadioGetNow((otInstance *)&notnull_intance));
  return  time_ms;
}

void otPlatAlarmMilliStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
  (void)(aInstance);
  uint64_t alarm_expire_step = 0;
  uint32_t curr_time_ms = 0;
  uint32_t ret = 0;
  uint32_t alarm_expire_ms;

  /* Start critical section */
  LINKLAYER_PLAT_DisableIRQ();
  
  milli_alarm_struct_st[0].expires = aT0 + aDt;
  milli_alarm_struct_st[0].sIsRunning = 1;

  /* Stop Alarm */
  os_timer_stop(milli_timer_id);

  curr_time_ms = otPlatAlarmMilliGetNow();
  
  /* check if Alarm in the Past */
  if (curr_time_ms > (aT0 + aDt))
  {
    /* Set alarm_expire_ms to 0 if alarm in the past */
    alarm_expire_ms = 0;
  }
  else
  {
    alarm_expire_ms = milli_alarm_struct_st[0].expires - curr_time_ms;
  }

  /* Check for overflow */
  if (aT0 > (MAX_UINT32 - aDt))
  {
    if (curr_time_ms >= aT0)
    {
      /* In this case curr_time_ms has not overflowed  */
      alarm_expire_ms = aDt - (curr_time_ms - aT0);
    }
  }

  /* check if alarm need to be scheduled now */
  if(alarm_expire_ms <= MILLI_START_THRESHOLD)
  {
    /* schedule alarm right now */
    APP_THREAD_ScheduleAlarm();
  }
  else
  {
    /* Convert in sleeptimer step */
    alarm_expire_step = FROM_MS_TO_SLP_STEP((uint64_t)alarm_expire_ms);

    /* check if alarm_expire_step overflows */
    if (alarm_expire_step > MAX_UINT32)
    {
      // In this case do nothing
      // It only happens when OT stack asks for alarm in more than 37hours
      // Ot stack will trigger new alarm before 37hours
      milli_alarm_struct_st[0].expires = UINT32_MAX;
      milli_alarm_struct_st[0].sIsRunning = 2;
    }
    else
    {
      /* Start Sleeptimer to schedule alarm */
      ret = os_timer_start(milli_timer_id, (uint32_t)alarm_expire_step);

      if (ret != 0)
      {
        /* os_timer_start shouldn't return error */
        while(1);
      }
    }
  }
  
  /* end critical section */
  LINKLAYER_PLAT_EnableIRQ();
  
}

void otPlatAlarmMilliStop(otInstance *aInstance)
{
  (void)(aInstance);
  milli_alarm_struct_st[0].sIsRunning = 0;
  os_timer_stop(milli_timer_id);
}

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
void otPlatAlarmMicroStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
  (void)(aInstance);
  uint32_t ret = 0;
  uint32_t curr_time_us= 0;
  uint32_t alarm_expire_us;

  micro_alarm_struct_st[0].sIsRunning = 1;
  micro_alarm_struct_st[0].expires = (uint32_t)(aT0 + aDt);

  /* After getting time in us otPlatAlarmMicroStartAt shouldn't be preempted */
  LINKLAYER_PLAT_DisableIRQ();

  /* Get current time in us */
  curr_time_us = otPlatAlarmMicroGetNow();

  /* Stop alarm */
  os_timer_stop(csl_timer_id);

  /* check if Alarm in the Past */
  if (curr_time_us > (aT0 + aDt))
  {
    /* Set alarm_expire_ms to 0 if alarm in the past */
    alarm_expire_us = 0;
  }
  else
  {
    alarm_expire_us = micro_alarm_struct_st[0].expires - curr_time_us;
  }

  /* Check for overflow */
  if (aT0 > MAX_UINT32 - aDt)
  {
    if (curr_time_us >= aT0)
    {
      /* In this case curr_time_ms has not overflowed */
      alarm_expire_us = aDt - (curr_time_us - aT0);
    }
  }

  /* check if alarm need to be scheduled now */
  if(alarm_expire_us <= MICRO_START_THRESHOLD)
  {
    /* schedule alarm right now */
    APP_THREAD_ScheduleUsAlarm();
  }
  else
  {
    /* Start Sleeptimer to schedule alarm */
    ret = os_timer_start_in_us(csl_timer_id, alarm_expire_us);
    if (ret != 0)
    {
      /* os_timer_start shouldn't return error */
      while(1);
    }
  }

  LINKLAYER_PLAT_EnableIRQ();
}

void otPlatAlarmMicroStop(otInstance *aInstance)
{
  (void)(aInstance);
  micro_alarm_struct_st[0].sIsRunning = 0;
  os_timer_stop(csl_timer_id);
}

uint32_t otPlatAlarmMicroGetNow(void)
{
  return (uint32_t)otPlatRadioGetNow((otInstance *)&notnull_intance);
}

#endif /* OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE */

static void ms_alarm_timer_cbk(void *arg)
{
  APP_THREAD_ScheduleAlarm();
}

void arcAlarmProcess(otInstance *aInstance)
{
  (void)(aInstance);
  if (milli_alarm_struct_st[0].sIsRunning == 1)
  {
    milli_alarm_struct_st[0].sIsRunning = 0;
    otPlatAlarmMilliFired(aInstance);
  }
}

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE

static void us_alarm_timer_cbk(void *arg)
{
  APP_THREAD_ScheduleUsAlarm();
}

void arcUsAlarmProcess(otInstance *aInstance)
{
  (void)(aInstance);
  if (micro_alarm_struct_st[0].sIsRunning == 1)
  {
    micro_alarm_struct_st[0].sIsRunning = 0;
    otPlatAlarmMicroFired(aInstance);
  }
}
#endif