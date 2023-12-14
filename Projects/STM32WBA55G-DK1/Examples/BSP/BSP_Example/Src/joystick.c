/**
  ******************************************************************************
  * @file    joystick.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the joystick feature in the
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


/** @addtogroup STM32L5xx_HAL_Examples
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
static void Joystick_SetHint(uint8_t mode);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  JOY demo
  * @param  None
  * @retval 0 if ok, else value < 0.
  */
int32_t Joystick_demo(void)
{
  JOYPin_TypeDef  joystick_state;

  /* Wait Joystick release before DeInitialization */
  while (BSP_JOY_GetState(JOY1) != JOY_NONE);
  BSP_JOY_DeInit(JOY1, JOY_ALL);

  /* 1. GPIO mode ------------------------------------------------------------*/

  /* Display Demo background */
  Joystick_SetHint(0);

  /* Initialize Joystick in GPIO Mode */
  BSP_JOY_Init(JOY1, JOY_MODE_GPIO, JOY_ALL);

  do
  {
    /* Retrieve JOY State in GPIO Mode (Polling) */
    joystick_state = (JOYPin_TypeDef) BSP_JOY_GetState(JOY1);

    if (joystick_state != JoyPinPressed)
    {
      JoyPinPressed = joystick_state;

      switch (JoyPinPressed)
      {
      case JOY_UP:
        Display_string("JOY_UP");
        /* Reset Joystick state */
        JoyPinPressed  = JOY_NONE;
        break;
      case JOY_DOWN:
        Display_string("JOY_DOWN");
        /* Reset Joystick state */
        JoyPinPressed  = JOY_NONE;
        break;
      case JOY_LEFT:
        Display_string("JOY_LEFT");
        /* Reset Joystick state */
        JoyPinPressed  = JOY_NONE;
        break;
      case JOY_RIGHT:
        Display_string("JOY_RIGHT");
        /* Reset Joystick state */
        JoyPinPressed  = JOY_NONE;
        break;
      case JOY_SEL:
        Display_string("Exit");
        HAL_Delay(200);
        break;
      case JOY_NONE:
        Display_string("...");
        break;
      default:
        break;
      }
    }

  } while (JoyPinPressed != JOY_SEL);

  /* Wait Joystick release before DeInitialization */
  while (BSP_JOY_GetState(JOY1) != JOY_NONE);
  BSP_JOY_DeInit(JOY1, JOY_ALL);

  /* 2. EXTI mode ------------------------------------------------------------*/
  Joystick_SetHint(1);

  /* Initialize Joystick in GPIO Mode */
  BSP_JOY_Init(JOY1, JOY_MODE_EXTI, JOY_ALL);

  joystick_state = JOY_NONE;
  JoyPinPressed = JOY_NONE;

  do
  {
    if (joystick_state != JoyPinPressed)
    {
      joystick_state = JoyPinPressed;

      switch (JoyPinPressed)
      {
      case JOY_UP:
        Display_string("JOY_UP");
        /* Reset Joystick state */
        JoyPinPressed  = JOY_NONE;
        break;
      case JOY_DOWN:
        Display_string("JOY_DOWN");
        /* Reset Joystick state */
        JoyPinPressed  = JOY_NONE;
        break;
      case JOY_LEFT:
        Display_string("JOY_LEFT");
        /* Reset Joystick state */
        JoyPinPressed  = JOY_NONE;
        break;
      case JOY_RIGHT:
        Display_string("JOY_RIGHT");
        /* Reset Joystick state */
        JoyPinPressed  = JOY_NONE;
        break;
      case JOY_SEL:
        Display_string("Exit");
        HAL_Delay(200);
        break;
      case JOY_NONE:
      default:
        break;
      }
    }

  } while (joystick_state != JOY_SEL);

  /* Reset Joystick state */
  JoyPinPressed  = JOY_NONE;
  return 0;
}


/**
  * @brief  Display Joystick demo hint
  * @param  None
  * @retval None
  */
static void Joystick_SetHint(uint8_t mode)
{
  /* Clear the LCD */
  BSP_LCD_Clear(0, LCD_COLOR_BLACK);

  UTIL_LCD_DisplayStringAt(40, 0, (uint8_t *)"Demo JOY", LEFT_MODE);
  UTIL_LCD_SetFont(&Font8);
  if (mode)
  {
    UTIL_LCD_DisplayStringAt(100, 0, (uint8_t *)"EXTI", LEFT_MODE);
  }
  else
  {
    UTIL_LCD_DisplayStringAt(100, 0, (uint8_t *)"GPIO", LEFT_MODE);
  }
  UTIL_LCD_SetFont(&Font12);
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


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
