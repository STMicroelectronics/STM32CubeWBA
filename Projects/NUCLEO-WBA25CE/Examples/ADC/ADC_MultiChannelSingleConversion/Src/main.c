/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Examples/ADC/ADC_MultiChannelSingleConversion/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to use a ADC peripheral to convert
  *          several channels, ADC conversions performed successively
  *          in a scan sequence.
  *          This example is based on the STM32WBAxx ADC HAL API.
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
  /* Definition of ADCx conversions data table size */
  /* Size of array set to ADC sequencer number of ranks converted,            */
  /* to have a rank in each array address.                                    */
  #define ADC_CONVERTED_DATA_BUFFER_SIZE   (   3UL)

  /* Init variable out of expected ADC conversion data range */
  #define VAR_CONVERTED_DATA_INIT_VALUE    (__LL_ADC_DIGITAL_SCALE(LL_ADC_RESOLUTION_12B) + 1)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc4;
DMA_NodeTypeDef Node_LPDMA1_Channel1;
DMA_QListTypeDef List_LPDMA1_Channel1;
DMA_HandleTypeDef handle_LPDMA1_Channel1;

/* USER CODE BEGIN PV */

/* Variables for ADC conversion data */
__IO uint16_t uhADCxConvertedData[ADC_CONVERTED_DATA_BUFFER_SIZE]; /* ADC group regular conversion data (array of data) */

/* Variables for ADC conversion data computation to physical values */
uint16_t uhADCxConvertedData_VoltageGPIO_mVolt = 0UL;        /* Value of voltage calculated from ADC conversion data (unit: mV) */
uint16_t uhADCxConvertedData_VrefInt_mVolt = 0UL;            /* Value of internal voltage reference VrefInt calculated from ADC conversion data (unit: mV) */
 int16_t hADCxConvertedData_Temperature_DegreeCelsius = 0UL; /* Value of temperature calculated from ADC conversion data (unit: degree Celsius) */
uint16_t uhADCxConvertedData_VrefAnalog_mVolt = 0UL;         /* Value of analog reference voltage (Vref+), connected to analog voltage supply Vdda, calculated from ADC conversion data (unit: mV) */

/* Variable to report status of DMA transfer of ADC group regular conversions */
/*  0: DMA transfer is not completed                                          */
/*  1: DMA transfer is completed                                              */
/*  2: DMA transfer has not yet been started yet (initial state)              */
__IO uint8_t ubDmaTransferStatus = 2U; /* Variable set into DMA interruption callback */

/* Variable to report number of ADC group regular sequence completed          */
uint32_t ubAdcGrpRegularSequenceConvCount = 0UL; /* Variable set into ADC interruption callback */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_LPDMA1_Init(void);
static void MX_GPIO_Init(void);
static void MX_ADC4_Init(void);
static void MX_ICACHE_Init(void);
/* USER CODE BEGIN PFP */

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
  uint32_t tmp_index;

  /* Initialize ADC group regular data buffer values */
  for (tmp_index = 0; tmp_index < ADC_CONVERTED_DATA_BUFFER_SIZE; tmp_index++)
  {
    uhADCxConvertedData[tmp_index] = VAR_CONVERTED_DATA_INIT_VALUE;
  }

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
  MX_LPDMA1_Init();
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
  /* Note: First start with DMA transfer initialization, following ones
           with basic ADC start. */
  if (HAL_ADC_Start_DMA(&hadc4,
                        (uint32_t *)uhADCxConvertedData,
                        ADC_CONVERTED_DATA_BUFFER_SIZE
                       ) != HAL_OK)
  {
    /* Error: ADC conversion start could not be performed */
    Error_Handler();
  }

  /* Toggle LED at each ADC conversion */
  BSP_LED_On(LD1);
  HAL_Delay(LED_BLINK_SLOW);
  BSP_LED_Off(LD1);
  HAL_Delay(LED_BLINK_SLOW);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* Start ADC group regular conversion */
    if (HAL_ADC_Start(&hadc4) != HAL_OK)
    {
      /* Error: ADC conversion start could not be performed */
      Error_Handler();
    }

    /* Toggle LED at each ADC conversion */
    BSP_LED_On(LD1);
    HAL_Delay(LED_BLINK_SLOW);
    BSP_LED_Off(LD1);
    HAL_Delay(LED_BLINK_SLOW);
    
    /* Note: ADC group regular conversions data are stored into array         */
    /*       "uhADCxConvertedData"                                            */
    /*       (for debug: see variable content into watch window).             */
    /*       - uhADCxConvertedData[0]: ADC channel set on sequence rank 1     */
    /*                                 (ADC1 channel 3)                       */
    /*       - uhADCxConvertedData[1]: ADC channel set on sequence rank 2     */
    /*                                 (ADC1 internal channel VrefInt)        */
    /*       - uhADCxConvertedData[2]: ADC channel set on sequence rank 3     */
    /*                                 (ADC1 internal channel temper. sensor) */

    /* If ADC conversions and DMA transfer are completed, then process data */
    if(ubDmaTransferStatus == 1)
    {
      /* For this example purpose, calculate analog reference voltage (Vref+) */
      /* from ADC conversion of internal voltage reference VrefInt.           */
      /* This voltage should correspond to value of literal "VDDA_APPLI".     */
      /* Note: This calculation can be performed when value of voltage Vref+  */
      /*       is unknown in the application                                  */
      /*       (This is not the case in this example due to target board      */
      /*       supplied by a LDO regulator providing a known constant voltage */
      /*       of value "VDDA_APPLI").                                        */
      /*       In typical case of Vref+ connected to Vdd, it allows to        */
      /*       deduce Vdd value.                                              */
      uhADCxConvertedData_VrefAnalog_mVolt = __LL_ADC_CALC_VREFANALOG_VOLTAGE(uhADCxConvertedData[1], LL_ADC_RESOLUTION_12B);

      /* Computation of ADC conversions raw data to physical values           */
      /* using LL ADC driver helper macro.                                    */
      uhADCxConvertedData_VoltageGPIO_mVolt        = __LL_ADC_CALC_DATA_TO_VOLTAGE(uhADCxConvertedData_VrefAnalog_mVolt, uhADCxConvertedData[0], LL_ADC_RESOLUTION_12B);
      uhADCxConvertedData_VrefInt_mVolt            = __LL_ADC_CALC_DATA_TO_VOLTAGE(uhADCxConvertedData_VrefAnalog_mVolt, uhADCxConvertedData[1], LL_ADC_RESOLUTION_12B);
      hADCxConvertedData_Temperature_DegreeCelsius = __LL_ADC_CALC_TEMPERATURE(uhADCxConvertedData_VrefAnalog_mVolt, uhADCxConvertedData[2], LL_ADC_RESOLUTION_12B);
      /* Update status variable of DMA transfer */
      ubDmaTransferStatus = 0;

      /* Toggle LED 4 times */
      tmp_index = 4*2;
      while(tmp_index != 0)
      {
        BSP_LED_Toggle(LD1);
        HAL_Delay(LED_BLINK_FAST);
        tmp_index--;
      }
      HAL_Delay(500); /* Delay to highlight toggle sequence */
    }

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
  RCC_OscInitStruct.PLL1.PLLN = 8;
  RCC_OscInitStruct.PLL1.PLLP = 2;
  RCC_OscInitStruct.PLL1.PLLQ = 2;
  RCC_OscInitStruct.PLL1.PLLR = 2;
  RCC_OscInitStruct.PLL1.PLLFractional = 0;
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
  RCC_ClkInitStruct.AHB5_PLL1_CLKDivider = RCC_SYSCLK_PLL1_DIV2;
  RCC_ClkInitStruct.AHB5_HSEHSI_CLKDivider = RCC_SYSCLK_HSEHSI_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
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

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC4_Init 1 */

  /* USER CODE END ADC4_Init 1 */

  /** Common config
  */
  hadc4.Instance = ADC4;
  hadc4.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV4;
  hadc4.Init.Resolution = ADC_RESOLUTION_12B;
  hadc4.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc4.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc4.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc4.Init.LowPowerAutoPowerOff = DISABLE;
  hadc4.Init.LowPowerAutonomousDPD = ADC_LP_AUTONOMOUS_DPD_DISABLE;
  hadc4.Init.LowPowerAutoWait = DISABLE;
  hadc4.Init.ContinuousConvMode = DISABLE;
  hadc4.Init.NbrOfConversion = 3;
  hadc4.Init.DiscontinuousConvMode = ENABLE;
  hadc4.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc4.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc4.Init.DMAContinuousRequests = ENABLE;
  hadc4.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_LOW;
  hadc4.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  hadc4.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_79CYCLES_5;
  hadc4.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_79CYCLES_5;
  hadc4.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc4) != HAL_OK)
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

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_VREFINT;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc4, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = ADC_REGULAR_RANK_3;
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
  * @brief LPDMA1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPDMA1_Init(void)
{

  /* USER CODE BEGIN LPDMA1_Init 0 */

  /* USER CODE END LPDMA1_Init 0 */

  /* Peripheral clock enable */
  __HAL_RCC_LPDMA1_CLK_ENABLE();

  /* LPDMA1 interrupt Init */
    HAL_NVIC_SetPriority(LPDMA1_Channel1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(LPDMA1_Channel1_IRQn);

  /* USER CODE BEGIN LPDMA1_Init 1 */

  /* USER CODE END LPDMA1_Init 1 */
  /* USER CODE BEGIN LPDMA1_Init 2 */

  /* USER CODE END LPDMA1_Init 2 */

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

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : LD1_Pin */
  GPIO_InitStruct.Pin = LD1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LD1_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/******************************************************************************/
/*   USER IRQ HANDLER TREATMENT                                               */
/******************************************************************************/

/**
  * @brief  DMA transfer complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
  /* Update status variable of DMA transfer */
  ubDmaTransferStatus = 1;
}

/**
  * @brief  ADC error interruption callback
  * @retval None
  */
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
  /* Note: Disable ADC interruption that caused this error before entering in
           infinite loop below. */

  /* In case of error due to overrun: Disable ADC group regular overrun interruption */
  LL_ADC_DisableIT_OVR(ADC4);

  /* Error reporting */
  Error_Handler();
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
#ifdef USE_FULL_ASSERT
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
