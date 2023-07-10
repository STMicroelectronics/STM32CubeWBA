/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Examples_LL/PKA/PKA_ModularExponentiation/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to use PKA peripheral to generate an
  *          ECDSA signature using the STM32WBAxx PKA LL API.
  *          Peripheral initialization done using LL unitary services functions.
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

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

__IO uint32_t endOfProcess = 0;
uint8_t buffer[256] = {0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RNG_Init(void);
static void MX_ICACHE_Init(void);
static void MX_PKA_Init(void);
/* USER CODE BEGIN PFP */

void     LED_Init(void);
void     LED_On(void);
void     LED_Blinking(uint32_t Period);
static uint32_t Buffercmp(const uint8_t* pBuffer1,const uint8_t* pBuffer2, uint32_t BufferLength);
__IO uint32_t *PKA_Memcpy_u8_to_u32(__IO uint32_t dst[], const uint8_t src[], uint32_t n);

void PKA_Memcpy_u32_to_u8(uint8_t dst[], __IO const uint32_t src[], uint32_t n);
void PKA_load_ciphering_parameter(void);
void PKA_load_unciphering_parameter(void);

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

  uint32_t result = 0;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  /* System interrupt init*/
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

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
  MX_RNG_Init();
  MX_ICACHE_Init();
  MX_PKA_Init();
  /* USER CODE BEGIN 2 */
  
  /* Set mode to ECDSA signature generation in interrupt mode */
  
   LL_PKA_SetMode(PKA, LL_PKA_MODE_MODULAR_EXP);
  LL_PKA_EnableIT_ADDRERR(PKA);
  LL_PKA_EnableIT_RAMERR(PKA);
  LL_PKA_EnableIT_PROCEND(PKA);
  
  LL_mDelay(1);
  
  /*   FROM PLAINTEXT TO CIPHERTEXT   */
  
  /* Loads the input buffers to PKA RAM */
  PKA_load_ciphering_parameter();
  
  /* Launch the computation in interrupt mode */
  LL_PKA_Start(PKA);
  
  /* Wait for the interrupt callback */
  while(endOfProcess != 1);
  endOfProcess = 0;
  
  /* Retrieve the result and output buffer */
   PKA_Memcpy_u32_to_u8(buffer, &PKA->RAM[PKA_MODULAR_EXP_OUT_RESULT],rsa_pub_2048_modulus_len ) ;

  
  /* Compare to expected results */
  result = Buffercmp(buffer, ciphertext_bin, ciphertext_bin_len);
  if (result != 0)
  {
    LED_Blinking(LED_BLINK_ERROR);
  } 
  
  /*   FROM CIPHERTEXT TO PLAINTEXT   */
  
  /* Loads the input buffers to PKA RAM */  
  PKA_load_unciphering_parameter();
  
  /* Launch the computation in interrupt mode */
  LL_PKA_Start(PKA);
  
  /* Wait for the interrupt callback */
  while(endOfProcess != 1);
  endOfProcess = 0;
  
  /* Retrieve the result and output buffer */
   PKA_Memcpy_u32_to_u8(buffer, &PKA->RAM[PKA_MODULAR_EXP_OUT_RESULT], rsa_pub_2048_modulus_len);
  
  /* Compare to expected results */
  result = Buffercmp(buffer, plaintext_bin, plaintext_bin_len);
  if (result != 0)
  {
    LED_Blinking(LED_BLINK_ERROR);
  }  
  
  LED_On();
 
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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

  LL_RCC_PLL1_ConfigDomain_SYS(LL_RCC_PLL1SOURCE_HSE, 2, 25, 4);
  LL_RCC_PLL1_ConfigDomain_PLL1Q(LL_RCC_PLL1SOURCE_HSE, 2, 25, 8);
  LL_RCC_PLL1_EnableDomain_PLL1Q();
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
  * @brief PKA Initialization Function
  * @param None
  * @retval None
  */
static void MX_PKA_Init(void)
{

  /* USER CODE BEGIN PKA_Init 0 */

  /* USER CODE END PKA_Init 0 */

  /* Peripheral clock enable */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_PKA);

  /* USER CODE BEGIN PKA_Init 1 */

  /* USER CODE END PKA_Init 1 */
  LL_PKA_Enable(PKA);
  /* USER CODE BEGIN PKA_Init 2 */

    /* Configure NVIC for PKA interrupts */
    /*   Set priority for PKA_IRQn */
    /*   Enable PKA_IRQn */
    NVIC_SetPriority(PKA_IRQn, 0);  
    NVIC_EnableIRQ(PKA_IRQn);
  
  /* USER CODE END PKA_Init 2 */

}

/**
  * @brief RNG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RNG_Init(void)
{

  /* USER CODE BEGIN RNG_Init 0 */

  /* USER CODE END RNG_Init 0 */

  LL_RCC_SetRNGClockSource(LL_RCC_RNG_CLKSOURCE_PLL1Q);

  /* Peripheral clock enable */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_RNG);

  /* USER CODE BEGIN RNG_Init 1 */

  /* USER CODE END RNG_Init 1 */
  LL_RNG_Enable(RNG);
  LL_RNG_EnableClkErrorDetect(RNG);
  /* USER CODE BEGIN RNG_Init 2 */

  /* USER CODE END RNG_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

  /**/
  LL_GPIO_SetOutputPin(LD1_GPIO_Port, LD1_Pin);

  /**/
  GPIO_InitStruct.Pin = LD1_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LD1_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief  Load into PKA RAM the ciphering parameters.
  * @param  None
  * @retval None
  */
void PKA_load_ciphering_parameter(void)
{
  /* Get the number of bit per operand */
  PKA->RAM[PKA_MODULAR_EXP_IN_OP_NB_BITS] = rsa_pub_2048_modulus_len*8;

  /* Get the number of bit of the exponent */
  PKA->RAM[PKA_MODULAR_EXP_IN_EXP_NB_BITS] = rsa_pub_2048_publicExponent_len*8;

  /* Move the input parameters pOp1 to PKA RAM */

  PKA_Memcpy_u8_to_u32(&PKA->RAM[PKA_MODULAR_EXP_IN_EXPONENT_BASE], plaintext_bin, rsa_pub_2048_modulus_len);

  PKA->RAM[PKA_MODULAR_EXP_IN_EXPONENT_BASE + ((rsa_pub_2048_modulus_len  +3UL)  / 4UL)] = 0;
  /* Move the exponent to PKA RAM */
  PKA_Memcpy_u8_to_u32(&PKA->RAM[PKA_MODULAR_EXP_IN_EXPONENT], rsa_pub_2048_publicExponent, rsa_pub_2048_publicExponent_len);
  PKA->RAM[PKA_MODULAR_EXP_IN_EXPONENT + ((rsa_pub_2048_publicExponent_len  +3UL)  / 4UL)] = 0;

  /* Move the modulus to PKA RAM */
  PKA_Memcpy_u8_to_u32(&PKA->RAM[PKA_MODULAR_EXP_IN_MODULUS], rsa_pub_2048_modulus, rsa_pub_2048_modulus_len);
   PKA->RAM[PKA_MODULAR_EXP_IN_MODULUS + ((rsa_pub_2048_modulus_len +3UL)  / 4UL)] = 0;
}

/**
  * @brief  Load into PKA RAM the unciphering parameters.
  * @param  None
  * @retval None
  */
void PKA_load_unciphering_parameter(void)
{
  /* Get the number of bit per operand */
  PKA->RAM[PKA_MODULAR_EXP_IN_OP_NB_BITS] = rsa_pub_2048_modulus_len*8;

  /* Get the number of bit of the exponent */
  PKA->RAM[PKA_MODULAR_EXP_IN_EXP_NB_BITS] = rsa_priv_2048_privateExponent_len*8;

  /* Move the input parameters pOp1 to PKA RAM */
  PKA_Memcpy_u8_to_u32(&PKA->RAM[PKA_MODULAR_EXP_IN_EXPONENT_BASE], ciphertext_bin, rsa_pub_2048_modulus_len);
  PKA->RAM[PKA_MODULAR_EXP_IN_EXPONENT_BASE + rsa_pub_2048_modulus_len / 4] = 0;

  /* Move the exponent to PKA RAM */
  PKA_Memcpy_u8_to_u32(&PKA->RAM[PKA_MODULAR_EXP_IN_EXPONENT], rsa_priv_2048_privateExponent, rsa_priv_2048_privateExponent_len);
  PKA->RAM[PKA_MODULAR_EXP_IN_EXPONENT + rsa_priv_2048_privateExponent_len / 4] = 0;

  /* Move the modulus to PKA RAM */
  PKA_Memcpy_u8_to_u32(&PKA->RAM[PKA_MODULAR_EXP_IN_MODULUS], rsa_priv_2048_modulus, rsa_pub_2048_modulus_len);
  PKA->RAM[PKA_MODULAR_EXP_IN_MODULUS + rsa_pub_2048_modulus_len / 4] = 0;
  
}


void PKA_ERROR_callback(void)
{
  LED_Blinking(LED_BLINK_ERROR);
}

void PKA_PROCEND_callback(void)
{
  endOfProcess = 1;
}
/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 0  : pBuffer1 identical to pBuffer2
  *         >0 : pBuffer1 differs from pBuffer2
  */
static uint32_t Buffercmp(const uint8_t* pBuffer1,const uint8_t* pBuffer2, uint32_t BufferLength)
{
  while (BufferLength--)
  {
    if ((*pBuffer1) != *pBuffer2)
    {
      return BufferLength;
    }
    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}

/**
  * @brief  Copy uint8_t array to uint32_t array to fit PKA number representation.
  * @param  dst Pointer to destination
  * @param  src Pointer to source
  * @param  n Number of u32 to be handled
  * @retval dst
  */
__IO uint32_t *PKA_Memcpy_u8_to_u32(__IO uint32_t dst[], const uint8_t src[], uint32_t n)
{
  const uint32_t *ptrSrc = (const uint32_t *) src;

  if (dst != 0)
  {
    for (uint32_t index = 0; index < n / 4; index++)
    {
      dst[index] = __REV(ptrSrc[n / 4 - index - 1]);
    }
  }
  return dst;
}

/**
  * @brief  Copy uint32_t array to uint8_t array to fit PKA number representation.
  * @param  dst Pointer to destination
  * @param  src Pointer to source
  * @param  n Number of u8 to be handled (must be multiple of 4)
  * @retval dst
  */
  
  void PKA_Memcpy_u32_to_u8(uint8_t dst[], __IO const uint32_t src[], uint32_t n)
{
  if (dst != 0)
  {
    if (src != 0)
    {
      uint32_t index_uint32_t = 0UL; /* This index is used outside of the loop */

      for (; index_uint32_t < (n / 4UL); index_uint32_t++)
      {
        /* Avoid casting from uint8_t* to uint32_t* by copying 4 uint8_t in a row */
        /* Apply __REV equivalent */
        uint32_t index_uint8_t = n - 4UL - (index_uint32_t * 4UL);
        dst[index_uint8_t + 3UL] = (uint8_t)((src[index_uint32_t] & 0x000000FFU));
        dst[index_uint8_t + 2UL] = (uint8_t)((src[index_uint32_t] & 0x0000FF00U) >> 8UL);
        dst[index_uint8_t + 1UL] = (uint8_t)((src[index_uint32_t] & 0x00FF0000U) >> 16UL);
        dst[index_uint8_t + 0UL] = (uint8_t)((src[index_uint32_t] & 0xFF000000U) >> 24UL);
      }

      /* Manage the buffers not aligned on uint32_t */
      if ((n % 4UL) == 1UL)
      {
        dst[0UL] = (uint8_t)((src[index_uint32_t] & 0x000000FFU));
      }
      else if ((n % 4UL) == 2UL)
      {
        dst[1UL] = (uint8_t)((src[index_uint32_t] & 0x000000FFU));
        dst[0UL] = (uint8_t)((src[index_uint32_t] & 0x0000FF00U) >> 8UL);
      }
      else if ((n % 4UL) == 3UL)
      {
        dst[2UL] = (uint8_t)((src[index_uint32_t] & 0x000000FFU));
        dst[1UL] = (uint8_t)((src[index_uint32_t] & 0x0000FF00U) >> 8UL);
        dst[0UL] = (uint8_t)((src[index_uint32_t] & 0x00FF0000U) >> 16UL);
      }
      else
      {
        /* The last element is already handle in the loop */
      }
    }
  }
}

/**
  * @brief  Turn-on LED2.
  * @param  None
  * @retval None
  */
void LED_On(void)
{
  /* Turn LED2 on */
  LL_GPIO_ResetOutputPin(LD1_GPIO_Port, LD1_Pin); 
}

/**
  * @brief  Set LED2 to Blinking mode for an infinite loop (toggle period based on value provided as input parameter).
  * @param  Period : Period of time (in ms) between each toggling of LED
  *   This parameter can be user defined values. Pre-defined values used in that example are :
  *     @arg LED_BLINK_FAST : Fast Blinking
  *     @arg LED_BLINK_SLOW : Slow Blinking
  *     @arg LED_BLINK_ERROR : Error specific Blinking
  * @retval None
  */
void LED_Blinking(uint32_t Period)
{
  /* Toggle LED2 in an infinite loop */
  while (1)
  {
    LL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin); 
    LL_mDelay(Period);
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
