/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MAIN_H
#define MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "stm32_lcd.h"
#include "stm32wbaxx_hal.h"
#include "stm32wba55g_discovery.h"
#include "stm32wba55g_discovery_audio.h"
#include "stm32wba55g_discovery_lcd.h"
#include "stm32wba55g_discovery_bus.h"

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  int32_t (*DemoFunc)(void);
  uint8_t  DemoName[20];
} BSP_DemoTypeDef;

/* Exported variables --------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define DISPLAY_TIME           250 /* 250ms display time */

/* Exported macro ------------------------------------------------------------*/
#define COUNT_OF_EXAMPLE(x)    (sizeof(x)/sizeof(BSP_DemoTypeDef))

/* Exported functions ------------------------------------------------------- */
int32_t  Lcd_demo(void);
int32_t  Led_demo(void);
int32_t  Joystick_demo(void);
int32_t  AudioPlay_demo(void);
int32_t  AudioRecord_demo(void);
void     Display_string(char *String);
void     Error_Handler(void);

#endif /* MAIN_H */
