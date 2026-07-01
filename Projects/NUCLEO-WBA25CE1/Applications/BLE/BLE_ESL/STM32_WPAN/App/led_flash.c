/**
 ******************************************************************************
 * @file    led_flash.c
 * @author  GPM WBL Application Team
 * @brief   Implementation of LED flashing for ESL profile.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
#include <string.h>
#include "stm32_timer.h"
#include "esl_device.h"     /* for NUM_LEDS */
#include "led_flash.h"

#define MAX_NUM_LEDS                    NUM_LEDS

#define REPEAT_TYPE_PATTERN_COUNTS      0
#define REPEAT_TYPE_SECONDS             1

typedef struct 
{
  uint8_t flashing_pattern[5];
  uint8_t start_bit_index;
  int8_t curr_bit_index;
  uint8_t off_period;
  uint8_t on_period;
  uint8_t repeat_type;
  uint16_t repeat_duration; 
  uint16_t repeat_count;
  UTIL_TIMER_Object_t led_change_timer;
  UTIL_TIMER_Object_t duration_timer;
} LEDFlashState_t;

void LEDChangeTimerCB(void *arg);
void durationTimerCB(void *arg);
static void programNextLEDChange(LEDFlashState_t *led_state_p);

static LEDFlashState_t led_flash_states[MAX_NUM_LEDS];

static uint8_t get_pattern_bit(uint8_t pattern[5], uint8_t index)
{
  uint8_t byte_idx; /* Index of the byte inside the array */
  uint8_t bit_offset; /* Index of the bit inside the byte */
  
  byte_idx = index / 8;  
  bit_offset = index % 8;
  
  if((pattern[byte_idx] & (1 << bit_offset)) != 0)
  {
    return 1;
  }
  else
  {
    return 0;
  }  
}

static int8_t get_next_bit(uint8_t pattern[5], uint8_t start_index, uint8_t bit_value)
{
  int8_t i;
  
  for(i = start_index; i >= 0; i--)
  {
    if(get_pattern_bit(pattern, i) == bit_value)
    {
      return i;
    }
  }
  
  /* No bit set to bit_value. */
  return -1;
}

/* flashing_pattern is little endian: flashing_pattern[4] is the most significant
   byte and flashing pattern starts from most significant bit of flashing_pattern[4]. */
int LED_FLASH_Start(uint8_t led_index, uint8_t flashing_pattern[5], uint8_t off_period, uint8_t on_period, uint8_t repeat_type, uint16_t repeat_duration)
{
  int8_t curr_bit_index;
  
  if(led_index >= MAX_NUM_LEDS)
    return -1;
  
  if(repeat_duration == 0 || repeat_duration > 0x7FFF)
    return -1;
  
  memcpy(led_flash_states[led_index].flashing_pattern, flashing_pattern, sizeof(led_flash_states[led_index].flashing_pattern));
  led_flash_states[led_index].off_period = off_period;
  led_flash_states[led_index].on_period = on_period;
  led_flash_states[led_index].repeat_type = repeat_type;
  led_flash_states[led_index].repeat_duration = repeat_duration;
  led_flash_states[led_index].repeat_count = 0;
  
  /* Just in case timers are already started. We assume area is initialized to 0. */
  UTIL_TIMER_Stop(&led_flash_states[led_index].led_change_timer);  
  UTIL_TIMER_Create(&led_flash_states[led_index].led_change_timer,
                    0,
                    UTIL_TIMER_ONESHOT,
                    LEDChangeTimerCB, &led_flash_states[led_index]);
  
  UTIL_TIMER_Stop(&led_flash_states[led_index].duration_timer);
  UTIL_TIMER_Create(&led_flash_states[led_index].duration_timer,
                    0,
                    UTIL_TIMER_ONESHOT,
                    durationTimerCB, &led_flash_states[led_index]);
  
  /* Look for first bit set to 1. */
  curr_bit_index = get_next_bit(flashing_pattern, 39, 1);
  if(curr_bit_index < 0)
  {
    /* No bit set to 1. */
    return -1;
  }
  led_flash_states[led_index].start_bit_index = curr_bit_index;
  led_flash_states[led_index].curr_bit_index = curr_bit_index;
  
  /* Look for next bit set to 0 and program timer. */
  programNextLEDChange(&led_flash_states[led_index]);
  
  if(repeat_type == REPEAT_TYPE_SECONDS)
  {
    UTIL_TIMER_StartWithPeriod(&led_flash_states[led_index].duration_timer, repeat_duration*1000);
  }
  
  LED_FLASH_LEDChangeCB(led_index, 1, false);
  
  return 0;
}

int LED_FLASH_Stop(uint8_t led_index)
{
  if(led_index >= MAX_NUM_LEDS)
    return -1;
  
  UTIL_TIMER_Stop(&led_flash_states[led_index].led_change_timer);
  UTIL_TIMER_Stop(&led_flash_states[led_index].duration_timer);
  
  return 0;
}

static void programNextLEDChange(LEDFlashState_t *led_state_p)
{
  int8_t next_bit_index;
  uint8_t curr_bit_value;
  uint8_t curr_period;
  uint16_t time_interval_ms;
  
  curr_bit_value = get_pattern_bit(led_state_p->flashing_pattern, led_state_p->curr_bit_index);
  
  if(curr_bit_value == 1)
  {
    curr_period = led_state_p->on_period;
  }
  else
  {
    curr_period = led_state_p->off_period;
  }
  
  /* Calculate time for next led state change */
  next_bit_index = get_next_bit(led_state_p->flashing_pattern, led_state_p->curr_bit_index, !curr_bit_value);
  
  time_interval_ms = (led_state_p->curr_bit_index - next_bit_index) * curr_period * 2;
  
  led_state_p->curr_bit_index = next_bit_index;
  
  UTIL_TIMER_StartWithPeriod(&led_state_p->led_change_timer, time_interval_ms);
}

__weak void LED_FLASH_LEDChangeCB(uint8_t led_index, uint8_t led_state, bool duration_end)
{
}

void LEDChangeTimerCB(void *arg)
{
  uint8_t curr_bit_value;
  LEDFlashState_t *led_state_p = arg;
  uint8_t led_index = led_state_p - led_flash_states;
  
  if(led_index >= MAX_NUM_LEDS)
    return;
  
  if(led_state_p->curr_bit_index == -1)
  {
    /* End of pattern is reached */
    led_state_p->curr_bit_index = led_state_p->start_bit_index;
    
    if(led_state_p->repeat_type == REPEAT_TYPE_PATTERN_COUNTS)
    {
      led_state_p->repeat_count++;
      if(led_state_p->repeat_count == led_state_p->repeat_duration)
      {
        /* End of duration is reached. */
        LED_FLASH_LEDChangeCB(led_index, 0, true);
        
        return;
      }
    }
  }
  
  curr_bit_value = get_pattern_bit(led_state_p->flashing_pattern, led_state_p->curr_bit_index);
  
  if(led_state_p->curr_bit_index == led_state_p->start_bit_index)
  {
    /* Check if there is really a led state change when we restart the pattern. */
    uint8_t prev_bit_value = get_pattern_bit(led_state_p->flashing_pattern, 0);
    
    if(curr_bit_value != prev_bit_value)
    {
      LED_FLASH_LEDChangeCB(led_index, curr_bit_value, false);
    }
  }
  else
  {
    LED_FLASH_LEDChangeCB(led_index, curr_bit_value, false);
  }
  
  programNextLEDChange(led_state_p);
}

void durationTimerCB(void *arg)
{
  LEDFlashState_t *led_state_p = arg;
  uint8_t led_index = led_state_p - led_flash_states;
  
  LED_FLASH_LEDChangeCB(led_index, 0, true);
  
  UTIL_TIMER_Stop(&led_state_p->led_change_timer);
}
