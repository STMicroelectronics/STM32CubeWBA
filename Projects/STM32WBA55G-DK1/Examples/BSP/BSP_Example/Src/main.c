/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the STM32WBAxx Nucleo BSP Driver
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

__ALIGN_END const uint8_t st_logo_small[] =
{
  /* bmp picture Header */
  0x42, 0x4d, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00,
  0x00, 0x28, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0xc4,
  0x0e, 0x00, 0x00, 0xc4, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
  /* Raw Data generate in vertical mode */
  0xff, 0xff, 0xff, 0xff, 0x7f, 0x1f, 0x07, 0x01, 0x01, 0xe1, 0xb1, 0x31, 0x31, 0x31, 0x31, 0x31,
  0x31, 0x31, 0x31, 0x31, 0x31, 0xb1, 0xf1, 0x31, 0x31, 0x31, 0x31, 0x31, 0xf1, 0xf1, 0xff, 0xff,
  0xff, 0xff, 0xef, 0xe3, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe1, 0xe3, 0xe6, 0x7c, 0x18, 0x00,
  0x00, 0x00, 0xc0, 0xf8, 0x1e, 0x07, 0x00, 0x00, 0x80, 0xe0, 0xf8, 0xfe, 0xff, 0xff, 0xff, 0xff
};

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO JOYPin_TypeDef JoyPinPressed = JOY_NONE;
uint8_t DemoIndex = 0;

BSP_DemoTypeDef  BSP_examples[] =
{
  {Led_demo,         "LED Demo"},
  {Joystick_demo,    "Joystick Demo"},
  {Lcd_demo,         "LCD Demo"},
  {AudioPlay_demo,   "AUDIO Play"},
  {AudioRecord_demo, "AUDIO Record"},
};

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Display_DemoDescription(uint8_t DemoIndex);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{

  /* STM32WBAxx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the System clock to have a frequency of 100 MHz */
  SystemClock_Config();

  /* For better performances, enable the instruction cache in 1-way direct mapped mode */
  HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY);
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Add your application code here
     */
  /* Initialize joystick */
  if (BSP_JOY_Init(JOY1, JOY_MODE_EXTI, JOY_ALL) != BSP_ERROR_NONE)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Init LCD */
  BSP_LCD_Init(0, LCD_ORIENTATION_LANDSCAPE);
  UTIL_LCD_SetFuncDriver(&LCD_Driver);
  UTIL_LCD_SetDevice(0);

  /* Set the LCD Text Color */
  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_SetTextColor(LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(LCD_COLOR_BLACK);

  Display_DemoDescription(DemoIndex);

  /* Infinite loop */
  while (1)
  {
    if (JoyPinPressed == JOY_SEL)
    {
      /* Reset Joystick state */
      JoyPinPressed = JOY_NONE;
      /* Execute BSP Example */
      BSP_examples[DemoIndex++].DemoFunc();

      if (DemoIndex >= COUNT_OF_EXAMPLE(BSP_examples))
      {
        DemoIndex = 0;
      }
      Display_DemoDescription(DemoIndex);
    }
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 100000000
  *            HCLK(Hz)                       = 100000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            APB7 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 32000000
  *            PLL_M                          = 4
  *            PLL_N                          = 25
  *            PLL_P                          = 2
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            Flash Latency(WS)              = 3
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  /* At reset, the regulator is the LDO, in range 2 */
  /* Need to move to range 1 to reach 100 MHz */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEDiv = RCC_HSE_DIV1;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL1.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL1.PLLFractional = 0;
  RCC_OscInitStruct.PLL1.PLLM = 4;
  RCC_OscInitStruct.PLL1.PLLN = 25;   /* VCO = HSE/M * N = 32 / 4 * 25 = 200 MHz */
  RCC_OscInitStruct.PLL1.PLLR = 2;    /* PLLSYS = 200 MHz / 2 = 100 MHz */
  RCC_OscInitStruct.PLL1.PLLP = 2;
  RCC_OscInitStruct.PLL1.PLLQ = 2;
  RCC_OscInitStruct.PLL1.PLLFractional = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1, PCLK2 and PCLK7
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 |
                                 RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK7 | RCC_CLOCKTYPE_HCLK5);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB7CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHB5_PLL1_CLKDivider = RCC_SYSCLK_PLL1_DIV4;
  RCC_ClkInitStruct.AHB5_HSEHSI_CLKDivider = RCC_SYSCLK_HSEHSI_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
}

/**
  * @brief  System Clock Configuration for Audio
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 982850000
  *            HCLK(Hz)                       = 982850000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            APB7 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 32000000
  *            PLL_M                          = 8
  *            PLL_N                          = 172
  *            PLL_P                          = 14
  *            PLL_Q                          = 7
  *            PLL_R                          = 7
  *            Flash Latency(WS)              = 3
  * @param  hsai: Pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @param  SampleRate: SAI SampleRate
  * @retval HAL status
  */

HAL_StatusTypeDef MX_SAI1_ClockConfig(SAI_HandleTypeDef *hsai, uint32_t SampleRate)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsai);
  UNUSED(SampleRate);

  HAL_StatusTypeDef ret = HAL_OK;
  RCC_PeriphCLKInitTypeDef rcc_ex_clk_init_struct;
  RCC_OscInitTypeDef       rcc_osc_init_struct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /* Select HSE as system clock source and configure the HCLK, PCLK1, PCLK2 and PCLK7
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 |
                                 RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK7 | RCC_CLOCKTYPE_HCLK5);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB7CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHB5_PLL1_CLKDivider = RCC_SYSCLK_PLL1_DIV4;
  RCC_ClkInitStruct.AHB5_HSEHSI_CLKDivider = RCC_SYSCLK_HSEHSI_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  rcc_osc_init_struct.OscillatorType      = RCC_OSCILLATORTYPE_HSE;
  rcc_osc_init_struct.HSEState            = RCC_HSE_ON;
  rcc_osc_init_struct.HSEDiv              = RCC_HSE_DIV1;
  rcc_osc_init_struct.PLL1.PLLState       = RCC_PLL_ON;
  rcc_osc_init_struct.PLL1.PLLSource      = RCC_PLLSOURCE_HSE;
  rcc_osc_init_struct.PLL1.PLLM           = 8;
  /* SAI clock config:
  PLL1_VCO Input = HSE/PLLM = 4 Mhz
  PLL1_VCO Output = PLL1_VCO Input * PLLN = 688 Mhz
  SAI_CLK_x = PLL1_VCO Output/PLLP = 688/14 = 49.142 Mhz */
  rcc_osc_init_struct.PLL1.PLLN           = 172;
  rcc_osc_init_struct.PLL1.PLLP           = 14;
  rcc_osc_init_struct.PLL1.PLLQ           = 7;
  rcc_osc_init_struct.PLL1.PLLR           = 7; /* PLLSYS = 344 MHz / 4 = 98.285 MHz */
  rcc_osc_init_struct.PLL1.PLLFractional  = 0;
  if (HAL_RCC_OscConfig(&rcc_osc_init_struct) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1, PCLK2 and PCLK7
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 |
                                 RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK7 | RCC_CLOCKTYPE_HCLK5);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB7CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHB5_PLL1_CLKDivider = RCC_SYSCLK_PLL1_DIV4;
  RCC_ClkInitStruct.AHB5_HSEHSI_CLKDivider = RCC_SYSCLK_HSEHSI_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  rcc_ex_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
  rcc_ex_clk_init_struct.Sai1ClockSelection   = RCC_SAI1CLKSOURCE_PLL1P;

  if (HAL_RCCEx_PeriphCLKConfig(&rcc_ex_clk_init_struct) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  __HAL_RCC_SAI1_CLK_ENABLE();

  return ret;
}

/**
  * @brief  Display main demo messages
  * @param  DemoIndex: Index of Demo to play
  * @retval None
  */
static void Display_DemoDescription(uint8_t DemoIndex)
{
  uint32_t x_size;
  uint32_t y_size;

  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);

  BSP_LCD_DisplayOn(0);
  BSP_LCD_Clear(0, LCD_COLOR_BLACK);

  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_SetTextColor(LCD_COLOR_BLACK);
  UTIL_LCD_SetBackColor(LCD_COLOR_WHITE);
  UTIL_LCD_FillRect(32, 0, 96, 16, LCD_COLOR_WHITE);
  UTIL_LCD_DisplayStringAt(40, 3, (uint8_t *)"BSP Example", LEFT_MODE);
  UTIL_LCD_SetTextColor(LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(LCD_COLOR_BLACK);
  BSP_LCD_DrawBitmap(0,  0,  0, (uint8_t *)st_logo_small);
  UTIL_LCD_DrawRect(0, 16, 128, 48, SSD1315_COLOR_WHITE);
  UTIL_LCD_DrawHLine(1, 16, 126, SSD1315_COLOR_BLACK);

  UTIL_LCD_DisplayStringAt(0, 21, (uint8_t *)"STM32WBA55G-DK1", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 34, (uint8_t *)"Press Joy SEL for", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 48, (uint8_t *)BSP_examples[DemoIndex].DemoName, CENTER_MODE);

  /* Refresh LCD to Display FrameBuffer */
  BSP_LCD_Refresh(0);
}

/**
  * @brief  Display string messages
  * @param  String: pointer on String to display
  * @retval None
  */
void Display_string(char *String)
{
  /* Clear Demo LCD area */
  UTIL_LCD_FillRect(1, 16, 126, 47, SSD1315_COLOR_BLACK);
  UTIL_LCD_DisplayStringAt(0, 34, (uint8_t *)String, CENTER_MODE);
  /* Refresh LCD to Display FrameBuffer */
  BSP_LCD_Refresh(0);
}

/**
  * @brief  BSP Joystick Callback.
  * @param  JOY Joystick to be de-init.
  *   This parameter can be JOY1
  * @param  JoyPins Specifies joystick pin.
  *   This parameter can be one of following parameters:
  *     @arg JOY_SEL
  *     @arg JOY_DOWN
  *     @arg JOY_LEFT
  *     @arg JOY_RIGHT
  *     @arg JOY_UP
  * @retval None.
  */
void BSP_JOY_Callback(JOY_TypeDef JOY, JOYPin_TypeDef JoyPin)
{
  JoyPinPressed = JoyPin;
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */
