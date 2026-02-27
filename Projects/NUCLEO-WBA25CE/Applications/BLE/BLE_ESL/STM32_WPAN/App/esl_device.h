/**
  ******************************************************************************
  * @file    esl_device.h
  * @author  GPM WBL Application Team
  * @brief   Header file for ESL device.
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
#ifndef ESL_DEVICE_H
#define ESL_DEVICE_H

#include <stdint.h>

#define NUM_LEDS                3
#define NUM_SENSORS             1
#define NUM_DISPLAYS            1
#define NUM_IMAGES              3

#if NUM_DISPLAYS
extern const uint8_t ESL_Display_Info[5*NUM_DISPLAYS];
#endif
#if NUM_SENSORS
/* Size depends on type and number of sensors. To be changed accordingly. */
extern const uint8_t ESL_Sensor_info[3];
#endif
#if NUM_LEDS
extern const uint8_t ESL_LED_info[NUM_LEDS];
#endif

/* Callbacks to be implemented. */
void ESL_DEVICE_LEDControlCmdCB(uint8_t led_index,  uint8_t led_RGB_Brigthness, uint8_t led_flash_pattern[5], uint8_t off_period, uint8_t on_period, uint16_t led_repeat);
uint8_t ESL_DEVICE_SensorDataCmdCB(uint8_t sensor_index, uint8_t *data_p, uint8_t *data_length_p);
uint8_t ESL_DEVICE_TxtVsCmdCB(uint8_t txt_length, char *txt_p);
uint8_t ESL_DEVICE_PriceVsCmdCB(uint16_t int_part, uint8_t fract_part);
uint8_t ESL_DEVICE_DisplayImageCmdCB(uint8_t display_index, uint8_t image_index);
/* ESL_DEVICE_DisplayTimedImageCmdCB() is called when the Timed command is received to verify image validity.
   ESL_DEVICE_DisplayImageCmdCB() will be called when execution time is reached. */
uint8_t ESL_DEVICE_DisplayTimedImageCmdCB(uint8_t image_index);
void ESL_DEVICE_ServiceResetCmdCB(void);
void ESL_DEVICE_FactoryResetCB(void);
uint8_t ESL_DEVICE_RefreshDisplayCmdCB(uint8_t display_index, uint8_t *image_index_p);

#endif /* ESL_DEVICE_H */