/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "main.h"
#include "app_openbootloader.h"
#include "interfaces_conf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void SysTick_DeInit(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* STM32WBAxx HAL library initialization:
       - Configure the Flash pre-fetch, Flash preread and Buffer caches
       - Systick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the System clock */
  SystemClock_Config();

  OpenBootloader_Init();

  /* Disable SysTick interrupts as not needed */
  SysTick_DeInit();

  /* Infinite loop */
  while (true)
  {
    OpenBootloader_ProtocolDetection();
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 60000000
  *            HCLK(Hz)                       = 60000000
  *            AHB Prescaler                  = 1
  *            AHB5 Prescaler                 = 4
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            APB7 Prescaler                 = 1
  *            HSI Frequency(Hz)              = 16000000
  *            PLL_M                          = 2
  *            PLL_N                          = 25
  *            PLL_P                          = 2
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            Flash Latency(WS)              = 3
  *            Voltage range                  = 1
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0U};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0U};

  __HAL_RCC_PWR_CLK_ENABLE();

  /* At reset, the regulator is the LDO, in range 2 Need to move to range 1 to reach 100 MHz */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    /* Initialization error */
    Error_Handler();
  }

  /* Activate PLL with HSI as source*/
  RCC_OscInitStruct.OscillatorType     = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState           = RCC_HSI_ON;
  RCC_OscInitStruct.PLL1.PLLState      = RCC_PLL_ON;
  RCC_OscInitStruct.PLL1.PLLSource     = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL1.PLLFractional = 0U;
  RCC_OscInitStruct.PLL1.PLLM          = 2U;
  RCC_OscInitStruct.PLL1.PLLN          = 25U;   /* VCO = HSI/M * N = 16 / 2 * 25 = 200 MHz */
  RCC_OscInitStruct.PLL1.PLLR          = 2U;    /* PLLSYS = 200 MHz / 2 = 100 MHz */
  RCC_OscInitStruct.PLL1.PLLP          = 2U;
  RCC_OscInitStruct.PLL1.PLLQ          = 2U;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization error */
    Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1, PCLK2 and PCLK7 clocks dividers */
  RCC_ClkInitStruct.ClockType              = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 \
                                              | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK7 | RCC_CLOCKTYPE_HCLK5);
  RCC_ClkInitStruct.SYSCLKSource           = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider          = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider         = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider         = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB7CLKDivider         = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHB5_PLL1_CLKDivider   = RCC_SYSCLK_PLL1_DIV4;
  RCC_ClkInitStruct.AHB5_HSEHSI_CLKDivider = RCC_SYSCLK_HSEHSI_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    /* Initialization error */
    Error_Handler();
  }
}

/**
  * @brief  This function is used to de-initialized the SysTick.
  * @retval None
  */
static void SysTick_DeInit(void)
{
  /* Disable systick interrupt */
  SysTick->CTRL = 0x00000000U;
  SysTick->LOAD = 0x00000000U;
  SysTick->VAL  = 0x00000000U;
}

/* Public functions ----------------------------------------------------------*/

/**
  * @brief  This function is used to de-initialized the clock.
  * @retval None
  */
void System_DeInit(void)
{
  USARTx_DEINIT();
  I2Cx_DEINIT();
  SPIx_DEINIT();

  HAL_RCC_DeInit();

  HAL_NVIC_DisableIRQ(SPIx_IRQ);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  while (true)
  {
  }
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
  while (true)
  {
  }
}
#endif /* USE_FULL_ASSERT */
