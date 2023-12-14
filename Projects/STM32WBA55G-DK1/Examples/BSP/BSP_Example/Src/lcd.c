/**
  ******************************************************************************
  * @file    lcd.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the lcd feature in the
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
#define LCD_FEATURES_NUM     5
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t Lcd_Feature = 0;

/* Private function prototypes -----------------------------------------------*/
static void Lcd_SetHint(void);
static void Lcd_Show_Feature(uint8_t feature);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Lcd demo
  * @param  None
  * @retval 0 if ok, else value < 0.
  */
int32_t Lcd_demo(void)
{
  Lcd_SetHint();
  Lcd_Feature = 0;

  while (1)
  {
    /* Exit on JOY_SEL detection */
    if (JoyPinPressed == JOY_SEL)
    {
      BSP_LCD_ScrollingStop(0);
      Display_string("Exit");
      /* Reset Joystick state */
      JoyPinPressed = JOY_NONE;
      return 0;
    }
    else if (JoyPinPressed != JOY_NONE)
    {
      if (Lcd_Feature < LCD_FEATURES_NUM)
      {
        Lcd_Show_Feature(Lcd_Feature);
        Lcd_Feature++;
        /* Reset Joystick state */
        JoyPinPressed = JOY_NONE;
      }
    }
    while (JoyPinPressed == JOY_NONE);
  }
}

/**
  * @brief  Show LCD Features
  * @param  feature : feature index
  * @retval None
  */
static void Lcd_Show_Feature(uint8_t feature)
{
  uint32_t x_size;
  uint32_t y_size;

  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);

  switch (feature)
  {
    case 0:
      /* Clear Demo LCD area */
      UTIL_LCD_FillRect(1, 16, 126, 47, SSD1315_COLOR_BLACK);
      /* Display string on Demo area */
      UTIL_LCD_DisplayStringAt(0, 19, (uint8_t *)"Left Mode", LEFT_MODE);
      UTIL_LCD_DisplayStringAt(0, 33, (uint8_t *)"Center Mode", CENTER_MODE);
      UTIL_LCD_DisplayStringAt(0, 47, (uint8_t *)"Right Mode", RIGHT_MODE);
      /* Refresh LCD to Display FrameBuffer */
      BSP_LCD_Refresh(0);
      break;

    case 1:
      /* Clear Demo LCD area */
      UTIL_LCD_FillRect(1, 16, 126, 47, SSD1315_COLOR_BLACK);
      /* Display circles on Demo area */
      UTIL_LCD_FillCircle(32, 40, 20, SSD1315_COLOR_WHITE);
      UTIL_LCD_DrawCircle(96, 40, 20, SSD1315_COLOR_WHITE);
      /* Refresh LCD to Display FrameBuffer */
      BSP_LCD_Refresh(0);
      break;

    case 2:
      /* Clear Demo LCD area */
      UTIL_LCD_FillRect(1, 16, 126, 47, SSD1315_COLOR_BLACK);
      UTIL_LCD_DrawRect(10, 20, 40, 20, SSD1315_COLOR_WHITE);
      UTIL_LCD_FillRect(64, 40, 40, 20, SSD1315_COLOR_WHITE);
      /* Refresh LCD to Display FrameBuffer */
      BSP_LCD_Refresh(0);
      break;

    case 3:
      /* Clear Demo LCD area */
      UTIL_LCD_FillRect(1, 16, 126, 47, SSD1315_COLOR_BLACK);
      /* Fill Dema area with ST logo bitmap */
      for (uint16_t x = 0; x < x_size; x += 32)
      {
        for (uint16_t y = 16; y < y_size; y += 16)
        {
          BSP_LCD_DrawBitmap(0, x, y, (uint8_t *)st_logo_small);
        }
      }
      /* Refresh LCD to Display FrameBuffer */
      BSP_LCD_Refresh(0);
      break;

    case 4:
      BSP_LCD_ScrollingSetup(0, SSD1315_SCROLL_RIGHT, 2, 7, SSD1315_SCROLL_FREQ_2FRAMES);
      BSP_LCD_ScrollingStart(0);
      break;
  }
}


/**
  * @brief  Display Lcd demo hint
  * @param  None
  * @retval None
  */
static void Lcd_SetHint(void)
{
  /* Clear the LCD */
  BSP_LCD_Clear(0, LCD_COLOR_BLACK);

  UTIL_LCD_DisplayStringAt(40, 0, (uint8_t *)"Demo LCD", LEFT_MODE);
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
