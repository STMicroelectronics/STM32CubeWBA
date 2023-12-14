/**
  ******************************************************************************
  * @file    led.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the led feature in the
  *          STM32WBAxx_Nucleo driver
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32WBAxx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

extern __IO JOYPin_TypeDef JoyPinPressed;
extern __ALIGN_END const uint8_t st_logo_small[];
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void Led_SetHint(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Led demo
  * @param  None
  * @retval 0 if ok, else value < 0.
  */
int32_t Led_demo(void)
{
  JOYPin_TypeDef  joystick_state;

  joystick_state = JOY_NONE;

  /* Display Demo background */
  Led_SetHint();

  /* Initialize the LEDs */
  if (BSP_LED_Init(LD3) != BSP_ERROR_NONE)
  {
    /* Initialization Error */
    Error_Handler();
  }

  do
  {
    if (joystick_state != JoyPinPressed)
    {
      joystick_state = JoyPinPressed;
      switch (joystick_state)
      {
      case JOY_UP:
        BSP_LED_On(LD3);
        Display_string("Enable LED");
        HAL_Delay(200);
        /* Reset Joystick state */
        JoyPinPressed  = JOY_NONE;
        break;
      case JOY_DOWN:
        BSP_LED_Off(LD3);
        Display_string("Disable LED");
        HAL_Delay(200);
        /* Reset Joystick state */
        JoyPinPressed  = JOY_NONE;
        break;
      case JOY_LEFT:
      case JOY_RIGHT:
        BSP_LED_Toggle(LD3);
        Display_string("Toggle LED");
        HAL_Delay(200);
        /* Reset Joystick state */
        JoyPinPressed  = JOY_NONE;
        break;
      case JOY_SEL:
        Display_string("Exit");
        HAL_Delay(200);
        break;
      default:
        if (BSP_LED_GetState(LD3))
        {
          Display_string("LED ON");
        }
        else
        {
          Display_string("LED OFF");
        }
        break;
      }
    }
  } while (JoyPinPressed != JOY_SEL);

  /* Reset Joystick state */
  JoyPinPressed  = JOY_NONE;

  /* De-Initialize the LEDs */
  if (BSP_LED_DeInit(LD3) != BSP_ERROR_NONE)
  {
    /* Initialization Error */
    Error_Handler();
  }

  return 0;
}

/**
  * @brief  Display Led demo hint
  * @param  None
  * @retval None
  */
static void Led_SetHint(void)
{
  /* Clear the LCD */
  BSP_LCD_Clear(0, LCD_COLOR_BLACK);

  UTIL_LCD_DisplayStringAt(40, 0, (uint8_t *)"Demo LED", LEFT_MODE);
  BSP_LCD_DrawBitmap(0,  0,  0, (uint8_t *)st_logo_small);
  UTIL_LCD_DrawHLine(32, 13, 96, SSD1315_COLOR_WHITE);
  UTIL_LCD_DrawHLine(32, 14, 96, SSD1315_COLOR_WHITE);
  UTIL_LCD_DrawHLine(32, 15, 96, SSD1315_COLOR_WHITE);
  UTIL_LCD_DrawRect(0, 16, 128, 48, SSD1315_COLOR_WHITE);
  UTIL_LCD_DrawHLine(1, 16, 126, SSD1315_COLOR_BLACK);
  UTIL_LCD_DisplayStringAt(0, 25, (uint8_t *)"JOY Dir to Start", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 42, (uint8_t *)"JOY SEL to Exit", CENTER_MODE);

  /* Refresh LCD to Display FrameBuffer */
  BSP_LCD_Refresh(0);
}


/**
  * @}
  */

/**
  * @}
  */
