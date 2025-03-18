/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Examples/ADC/ADC_AnalogWatchdog/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to use a ADC peripheral
  *          with ADC analog watchdog to monitor a channel and detect
  *          when the corresponding conversion data is out of window thresholds.
  *          This example is based on the STM32WBAxx ADC HAL API.
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* Definitions of environment analog values */
  /* Value of analog reference voltage (Vref+), connected to analog voltage   */
  /* supply Vdda (unit: mV).                                                  */
  #define VDDA_APPLI                       (3300UL)
/* Definitions of data related to this example */
  /* Definition of ADCx analog watchdog window thresholds */
  #define ADC_AWD_THRESHOLD_HIGH           (__LL_ADC_DIGITAL_SCALE(LL_ADC_RESOLUTION_12B) / 2) /* Value of ADC analog watchdog threshold high */
  #define ADC_AWD_THRESHOLD_LOW            (   0UL) /* Value of ADC analog watchdog threshold low */

  /* Init variable out of expected ADC conversion data range */
  #define VAR_CONVERTED_DATA_INIT_VALUE    (__LL_ADC_DIGITAL_SCALE(LL_ADC_RESOLUTION_12B) + 1)
  #define ADC_AWD_REARM_DELAY_MS           (5000UL) /* ADC analog watchdog rearm delay (unit: ms) */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc4;

/* USER CODE BEGIN PV */

/* Variables for ADC conversion data */
__IO uint16_t uhADCxConvertedData = VAR_CONVERTED_DATA_INIT_VALUE; /* ADC group regular conversion data */

/* Variable to report status of ADC analog watchdog 1:                        */
/*  0: ADC conversion data into AWD window                                    */
/*  1: ADC conversion data out of AWD window                                  */
__IO uint8_t ubAnalogWatchdog1Status = 0U; /* Variable set into analog watchdog 1 interruption callback */

uint32_t led_toggle_period_ms = LED_BLINK_SLOW; /* LED toggle period (unit: ms) */
uint32_t adc_awd_rearm_delay_iterations_count; /* ADC analog watchdog rearm delay iterations count */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC4_Init(void);
static void MX_ICACHE_Init(void);
/* USER CODE BEGIN PFP */

void ADC_AnalogWatchdog_Rearm(void);
void LED_On(void);
void LED_Off(void);
void LED_Toggle(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC4_Init();
  MX_ICACHE_Init();
  /* USER CODE BEGIN 2 */

  /* Initialize LED on board */
  BSP_LED_Init(LD1);

  /* Perform ADC calibration */
  if (HAL_ADCEx_Calibration_Start(&hadc4) != HAL_OK)
  {
    /* Calibration Error */
    Error_Handler();
  }

  /* Start ADC group regular conversion */
  if (HAL_ADC_Start(&hadc4) != HAL_OK)
  {
    /* Error: ADC conversion start could not be performed */
    Error_Handler();
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* Process analog watchdog out of window event */
    if(ubAnalogWatchdog1Status == 1)
    {
      /* Wait for ADC analog watchdog rearm */
      adc_awd_rearm_delay_iterations_count--;
      if(adc_awd_rearm_delay_iterations_count == 0)
      {
        led_toggle_period_ms = LED_BLINK_SLOW;

        /* Reset status variable of ADC analog watchdog 1 */
        ubAnalogWatchdog1Status = 0;

        /* Rearm ADC analog watchdog to make it ready for another trig */
        ADC_AnalogWatchdog_Rearm();
      }
    }

    BSP_LED_Toggle(LD1);
    HAL_Delay(led_toggle_period_ms);

    /* Note: LED state depending on ADC analog watchdog 1 status              */
    /*       variable "ubAnalogWatchdog1Status"                               */
    /*       is set into ADC analog watchdog 1 IRQ handler,                   */
    /*       refer to function "HAL_ADC_LevelOutOfWindowCallback()".          */

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEDiv = RCC_HSE_DIV1;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL1.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL1.PLLM = 2;
  RCC_OscInitStruct.PLL1.PLLN = 12;
  RCC_OscInitStruct.PLL1.PLLP = 2;
  RCC_OscInitStruct.PLL1.PLLQ = 2;
  RCC_OscInitStruct.PLL1.PLLR = 2;
  RCC_OscInitStruct.PLL1.PLLFractional = 4096;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK7|RCC_CLOCKTYPE_HCLK5;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB7CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHB5_PLL1_CLKDivider = RCC_SYSCLK_PLL1_DIV4;
  RCC_ClkInitStruct.AHB5_HSEHSI_CLKDivider = RCC_SYSCLK_HSEHSI_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC4_Init(void)
{

  /* USER CODE BEGIN ADC4_Init 0 */

  /* USER CODE END ADC4_Init 0 */

  ADC_AnalogWDGConfTypeDef AnalogWDGConfig = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC4_Init 1 */

  /* USER CODE END ADC4_Init 1 */

  /** Common config
  */
  hadc4.Instance = ADC4;
  hadc4.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV4;
  hadc4.Init.Resolution = ADC_RESOLUTION_12B;
  hadc4.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc4.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc4.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc4.Init.LowPowerAutoPowerOff = DISABLE;
  hadc4.Init.LowPowerAutonomousDPD = ADC_LP_AUTONOMOUS_DPD_DISABLE;
  hadc4.Init.LowPowerAutoWait = DISABLE;
  hadc4.Init.ContinuousConvMode = ENABLE;
  hadc4.Init.NbrOfConversion = 1;
  hadc4.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc4.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc4.Init.DMAContinuousRequests = DISABLE;
  hadc4.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_LOW;
  hadc4.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  hadc4.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_79CYCLES_5;
  hadc4.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_79CYCLES_5;
  hadc4.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc4) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analog WatchDog 1
  */
  AnalogWDGConfig.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
  AnalogWDGConfig.WatchdogMode = ADC_ANALOGWATCHDOG_ALL_REG;
  AnalogWDGConfig.ITMode = ENABLE;
  AnalogWDGConfig.HighThreshold = ADC_AWD_THRESHOLD_HIGH;
  AnalogWDGConfig.LowThreshold = ADC_AWD_THRESHOLD_LOW;
  if (HAL_ADC_AnalogWDGConfig(&hadc4, &AnalogWDGConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
  if (HAL_ADC_ConfigChannel(&hadc4, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC4_Init 2 */

  /* USER CODE END ADC4_Init 2 */

}

/**
  * @brief ICACHE Initialization Function
  * @param None
  * @retval None
  */
static void MX_ICACHE_Init(void)
{

  /* USER CODE BEGIN ICACHE_Init 0 */

  /* USER CODE END ICACHE_Init 0 */

  /* USER CODE BEGIN ICACHE_Init 1 */

  /* USER CODE END ICACHE_Init 1 */

  /** Enable instruction cache in 1-way (direct mapped cache)
  */
  if (HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LD1_Pin */
  GPIO_InitStruct.Pin = LD1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD1_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief  Rearm ADC analog watchdog to make it ready another trig
  *         (ADC handle: hadc4, analog watchdog instance: AWD1).
  * @param  None
  * @retval None
  */
void ADC_AnalogWatchdog_Rearm(void)
{
  /* Clear flag ADC analog watchdog 1 */
  __HAL_ADC_CLEAR_FLAG(&hadc4, ADC_FLAG_AWD1);

  /* Enable ADC analog watchdog 1 interruption */
  __HAL_ADC_ENABLE_IT(&hadc4, ADC_IT_AWD1);
}

/******************************************************************************/
/*   USER IRQ HANDLER TREATMENT                                               */
/******************************************************************************/

/**
  * @brief  Analog watchdog callback in non blocking mode.
  * @param  hadc: ADC handle
  * @retval None
  */
void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc)
{
  /* Disable ADC analog watchdog 1 interruption */
  /* Note: ADC analog watchdog 1 interruption rearmed afterwards after
           event processing completion */
  __HAL_ADC_DISABLE_IT(hadc, ADC_IT_AWD1);

  /* Retrieve ADC conversion data */
  /* Note: data scale corresponds to ADC resolution */
  uhADCxConvertedData = HAL_ADC_GetValue(hadc);

  /* Update status variable of ADC analog watchdog 1 */
  ubAnalogWatchdog1Status = 1;

  /* Change LED toggle frequency depending on voltage below or above
     analog watchdog window */
  if (uhADCxConvertedData > ADC_AWD_THRESHOLD_HIGH)
  {
    led_toggle_period_ms = LED_BLINK_VERY_FAST;
  }
  else /* (uhADCxConvertedData < ADC_AWD_THRESHOLD_LOW) */
  {
    led_toggle_period_ms = LED_BLINK_FAST;
  }

  /* Set ADC analog watchdog rearm delay iterations count */
  adc_awd_rearm_delay_iterations_count = (ADC_AWD_REARM_DELAY_MS / led_toggle_period_ms);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* Turn on LED and remain in infinite loop */
  while (1)
  {
    BSP_LED_On(LD1);
  }
  /* USER CODE END Error_Handler_Debug */
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
  /* USER CODE BEGIN 6 */

  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
