/*******************************************************************************
 * @file    stm32_lpm_if.c
 * @author  MCD Application Team
 * @brief   Low layer function to enter/exit low power modes (stop, sleep)
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32_lpm.h"
#include "stm32_lpm_if.h"

/** @addtogroup TINY_LPM_IF
  * @{
  */

/* USER CODE BEGIN include */
#include "stm32wbaxx_hal.h"
/* USER CODE END include */

/* Exported variables --------------------------------------------------------*/
/** @defgroup TINY_LPM_IF_Exported_varaibles TINY LPM IF exported variables
  * @{
  */

/**
 * @brief variable to provide all the functions corresponding to the different low power modes.
 */
const struct UTIL_LPM_Driver_s UTIL_PowerDriver =
{
  PWR_EnterSleepMode,
  PWR_ExitSleepMode,

  PWR_EnterStopMode,
  PWR_ExitStopMode,

  PWR_EnterOffMode,
  PWR_ExitOffMode,
};

/**
 * @}
 */
/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN Private_Function_Prototypes */

/* USER CODE END Private_Function_Prototypes */
/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN Private_Typedef */

/* USER CODE END Private_Typedef */
/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Private_Define */

/* USER CODE END Private_Define */
/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Private_Macro */

/* USER CODE END Private_Macro */
/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Private_Variables */
extern void SystemClock_Config(void);
/* USER CODE END Private_Variables */

/** @addtogroup TINY_LPM_IF_Exported_functions
 * @{
 */

void PWR_EnterOffMode( void )
{
/* USER CODE BEGIN PWR_EnterOffMode */
  HAL_PWREx_EnableUltraLowPowerMode();
  HAL_PWR_EnterSTANDBYMode();
/* USER CODE END PWR_EnterOffMode */
}

void PWR_ExitOffMode( void )
{
/* USER CODE BEGIN PWR_ExitOffMode */

/* USER CODE END PWR_ExitOffMode */
}

void PWR_EnterStopMode( void )
{
/* USER CODE BEGIN PWR_EnterStopMode */
  HAL_PWREx_EnableUltraLowPowerMode();
  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
/* USER CODE END PWR_EnterStopMode */
}

void PWR_ExitStopMode( void )
{
/* USER CODE BEGIN PWR_ExitStopMode */
  __HAL_PWR_CLEAR_FLAG(PWR_WAKEUP_FLAG2);
  SystemClock_Config();
/* USER CODE END PWR_ExitStopMode */
}

void PWR_EnterSleepMode( void )
{
/* USER CODE BEGIN PWR_EnterSleepMode */
  HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
/* USER CODE END PWR_EnterSleepMode */
}

void PWR_ExitSleepMode( void )
{
/* USER CODE BEGIN PWR_ExitSleepMode */
#if 0
  /* temporary patch to restore the PLL */
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_OFF;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

   SystemClock_Config();
#endif
  /* USER CODE END PWR_ExitSleepMode */
}

/* USER CODE BEGIN Private_Functions */

/* USER CODE END Private_Functions */

/**
 * @}
 */

/**
 * @}
 */

