/**
 ******************************************************************************
 * @file    led_flash.h
 * @author  GPM WBL Application Team
 * @brief   Header file for led_flash.c.
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

#ifndef LED_FLASH_H
#define LED_FLASH_H

#include <stdbool.h>

int LED_FLASH_Start(uint8_t led_index, uint8_t flashing_pattern[5], uint8_t off_period, uint8_t on_period, uint8_t repeat_type, uint16_t repeat_duration);

int LED_FLASH_Stop(uint8_t led_index);

void LED_FLASH_LEDChangeCB(uint8_t led_index, uint8_t led_state, bool duration_end);

#endif /*LED_FLASH_H */