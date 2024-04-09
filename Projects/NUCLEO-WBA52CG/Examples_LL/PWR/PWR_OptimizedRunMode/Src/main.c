/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Examples_LL/PWR/PWR_OptimizedRunMode/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to enter or exit Low Power Run mode and update
  *          the core frequency on the fly through the STM32WBAxx PWR LL API.
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

#define USE_LED

typedef enum {
  RUN_MODE_DOWN_TO_32MHZ = 0,
  RUN_MODE_DOWN_TO_1000KHZ  = 1,
  RUN_MODE_UP_TO_32MHZ   = 2,
  RUN_MODE_UP_TO_100MHZ   = 3
}RunMode_Typedef;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

RunMode_Typedef RunMode_Next = RUN_MODE_DOWN_TO_32MHZ;
__IO uint8_t ubSequenceRunModes_OnGoing = 1;
#ifdef USE_LED
__IO uint16_t uhLedBlinkSpeed = LED_BLINK_FAST;
#endif

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ICACHE_Init(void);
/* USER CODE BEGIN PFP */

#ifdef USE_LED
void     LED_Blinking(void);
#endif
void     EnterRunMode_DownTo32MHz(void);
void     EnterRunMode_LowPower_DownTo1000KHz(void);
void     EnterRunMode_UpTo32MHz(void);
void     EnterRunMode_UpTo100MHz(void);

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

  /* System interrupt init*/
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),15, 0));

  /* Enable PWR clock interface */

  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_PWR);

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ICACHE_Init();
  /* USER CODE BEGIN 2 */

  /* Remain in this loop until end of sequences of different run modes */
  while(ubSequenceRunModes_OnGoing != 0)
  {
#ifdef USE_LED
    /* Led blinking until User push-button action */
    LED_Blinking();
#endif
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

#ifdef USE_LED
    /* Led blinking in infinite loop */
    LED_Blinking();
#endif
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_3);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_3)
  {
  }

  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {
  }

  LL_RCC_PLL1_ConfigDomain_SYS(LL_RCC_PLL1SOURCE_HSE, 4, 25, 2);
  LL_RCC_PLL1_EnableDomain_PLL1R();
  LL_RCC_PLL1_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_8_16);
  LL_RCC_PLL1_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL1_IsReady() != 1)
  {
  }

  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1R);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1R)
  {
  }

  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAHB5Prescaler(LL_RCC_AHB5_DIV_4);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetAPB7Prescaler(LL_RCC_APB7_DIV_1);

  LL_Init1msTick(100000000);

  LL_SetSystemCoreClock(100000000);
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
  LL_ICACHE_SetMode(LL_ICACHE_1WAY);
  LL_ICACHE_Enable();
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
  LL_EXTI_InitTypeDef EXTI_InitStruct = {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

  /**/
  LL_GPIO_SetOutputPin(LD1_GPIO_Port, LD1_Pin);

  /**/
  LL_EXTI_SetEXTISource(LL_EXTI_CONFIG_PORTC, LL_EXTI_CONFIG_LINE13);

  /**/
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_13;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
  LL_EXTI_Init(&EXTI_InitStruct);

  /**/
  LL_GPIO_SetPinPull(B1_GPIO_Port, B1_Pin, LL_GPIO_PULL_UP);

  /**/
  LL_GPIO_SetPinMode(B1_GPIO_Port, B1_Pin, LL_GPIO_MODE_INPUT);

  /**/
  GPIO_InitStruct.Pin = LD1_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LD1_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  NVIC_SetPriority(EXTI13_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(EXTI13_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

#ifdef USE_LED
/**
  * @brief  Set LD1 to Blinking mode (Shall be call in a Loop).
  * @param  None
  * @retval None
  */
void LED_Blinking(void)
{
  /* Toggle IO. This function shall be called in a loop to toggle */
  LL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
  LL_mDelay(uhLedBlinkSpeed);
}
#endif

/**
  * @brief  Function to decrease Frequency at 32MHz in Run Mode.
  * @param  None
  * @retval None
  */
void EnterRunMode_DownTo32MHz(void)
{
  /* 1 - Switch clock source on HSE */
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSE);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSE)
  {
  };

  /* Set HSE to 32MHz */
  ;

  /* Disable PLL to decrease power consumption */
  LL_RCC_PLL1_Disable();
  while(LL_RCC_PLL1_IsReady() != 0)
  {
  };

  /* Set systick to 1ms in using frequency set to 32MHz */
  LL_Init1msTick(32 * 1000000);
  /* Update CMSIS variable */
  LL_SetSystemCoreClock(32 * 1000000);

  /* 2 - Adjust Flash Wait state after decrease Clock Frequency */
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);

  /* 3 - Set Voltage scaling (decrease Vcore) */
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
}

/**
  * @brief  Function to decrease Frequency at 1000KHZ in Low Power Run Mode.
  * @param  None
  * @retval None
  */
void EnterRunMode_LowPower_DownTo1000KHz(void)
{
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE2);
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {
  }

  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {
  }

  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_16);

  /* 1 - Set Frequency to 1000KHz to activate Low Power Run Mode: 1000KHz */
  ;
  /* Set systick to 1ms in using frequency set to 1000KHz */
  LL_Init1msTick(1000 * 1000);
  /* Update CMSIS variable */
  LL_SetSystemCoreClock(1000 * 1000);

  /* 2 - Adjust Flash Wait state after decrease Clock Frequency */
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);

  /* 3 - Activate Low Power Run Mode */
  LL_PWR_EnableUltraLowPowerMode();
}

/**
  * @brief  Function to increase Frequency at 32MHz in Run Mode.
  * @param  None
  * @retval None
  */
void EnterRunMode_UpTo32MHz(void)
{
  /* 1 - Deactivate Low Power Run Mode to increase Frequency up to 32MHz */
  LL_PWR_DisableUltraLowPowerMode();

  /* 2 - Adjust Flash Wait state before increase Clock Frequency */
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);

  /* Wait for flash latency setting effective before increase clock frequency */
  while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0)
  {
  };

  /* 3 - Set Frequency to 32MHz (HSE) */
  ;
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {
  }

  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSE);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSE)
  {
  }

  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  /* Set systick to 1ms in using frequency set to 32MHz */
  LL_Init1msTick(32 * 1000000);
  /* Update CMSIS variable */
  LL_SetSystemCoreClock(32 * 1000000);
}

/**
  * @brief  Function to increase Frequency at 100MHz in Run Mode.
  * @param  None
  * @retval None
  */
void EnterRunMode_UpTo100MHz(void)
{
  /* Set back same system configuration as before sequences of different run modes */
  SystemClock_Config();
}


/******************************************************************************/
/*   USER IRQ HANDLER TREATMENT                                               */
/******************************************************************************/
/**
  * @brief  Function to manage BUTTON IRQ Handler
  * @param  None
  * @retval None
  */
void UserButton_Callback(void)
{
  if(ubSequenceRunModes_OnGoing != 0)
  {
    switch(RunMode_Next)
    {
    case RUN_MODE_DOWN_TO_32MHZ:
      {
        /* Decrease core frequency and voltage
         * Frequency: 100MHz -> 32MHz
         * Voltage Scaling Range 2
         */
        EnterRunMode_DownTo32MHz();
  #ifdef USE_LED
        uhLedBlinkSpeed = LED_BLINK_MEDIUM;
  #endif
        /* Set Next RunMode to execute */
        RunMode_Next = RUN_MODE_DOWN_TO_1000KHZ;
        break;
      }
    case RUN_MODE_DOWN_TO_1000KHZ:
      {
        /* Decrease core frequency and enter Low Power Run mode
         * Frequency: 1000KHz
         * Voltage Scaling Range 2
         * LowPowerRunMode activated
         */
        EnterRunMode_LowPower_DownTo1000KHz();
  #ifdef USE_LED
        uhLedBlinkSpeed = LED_BLINK_SLOW;
  #endif
        /* Set Next RunMode to execute */
        RunMode_Next = RUN_MODE_UP_TO_32MHZ;
        break;
      }
    case RUN_MODE_UP_TO_32MHZ:
      {
        /* Increase core frequency and exit Low Power Run mode
         * Frequency: 1000KHz -> 32MHz
         * Voltage Scaling Range 2
         * LowPowerRunMode deactivated
         */
        EnterRunMode_UpTo32MHz();
  #ifdef USE_LED
        uhLedBlinkSpeed = LED_BLINK_MEDIUM;
  #endif
        /* Set Next RunMode to execute */
        RunMode_Next = RUN_MODE_UP_TO_100MHZ;
        break;
      }
    case RUN_MODE_UP_TO_100MHZ:
      {
        /* Increase core frequency and voltage:
         * Frequency: 32MHz -> 100MHz
         * Voltage Scaling Range 1
         */
        EnterRunMode_UpTo100MHz();
  #ifdef USE_LED
        uhLedBlinkSpeed = LED_BLINK_FAST;
  #endif

        /* Exit Test */
        ubSequenceRunModes_OnGoing = 0;
        break;
      }
    }
  }
  else
  {
    /* End of sequences of different run modes, no action */
  }
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
