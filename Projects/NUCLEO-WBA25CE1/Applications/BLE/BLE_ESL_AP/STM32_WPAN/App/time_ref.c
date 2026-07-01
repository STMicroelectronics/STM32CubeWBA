/**
  ******************************************************************************
  * @file         time_ref.c
  * @brief        This library implements a 32 bit virtual clock with tick
  *               duration of 1 millisecond, without the need to have a real
  *               tick (it relies on system time).
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "stm32_timer.h"    
#include "time_ref.h"

typedef struct
{
  uint32_t time_diff;  
}TIMEREF_Context_t;

static TIMEREF_Context_t context;

void TIMEREF_SetAbsoluteTime(uint32_t absolute_time)
{
  context.time_diff = UTIL_TIMER_GetCurrentTime() - absolute_time;
}

uint32_t TIMEREF_GetCurrentAbsTime(void)
{  
  return UTIL_TIMER_GetCurrentTime() - context.time_diff;
}
