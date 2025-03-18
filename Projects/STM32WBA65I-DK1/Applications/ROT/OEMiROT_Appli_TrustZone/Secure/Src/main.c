/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   Main application file.
  *          This application demonstrates Secure Services
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
#include "appli_region_defs.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Non-secure Vector table to jump to (internal Flash Bank2 here)             */
/* Caution: address must correspond to non-secure internal Flash where is     */
/*          mapped in the non-secure vector table
                             */
#define VTOR_TABLE_NS_START_ADDR  NS_CODE_START


/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint32_t SecureInitIODone = 0;

/* Private function prototypes -----------------------------------------------*/
static void NonSecure_Init(void);
static void MX_GPIO_Init(void);
static void MX_GTZC_Init(void);
static void  unsecure_sram1(uint32_t start, uint32_t end);


/* Private user code ---------------------------------------------------------*/


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* SAU/IDAU, FPU and interrupts secure/non-secure allocation setup done */
  /* in SystemInit() based on partition_stm32xxx.h file's definitions. */


  /* Enable SecureFault handler (HardFault is default) */
  SCB->SHCSR |= SCB_SHCSR_SECUREFAULTENA_Msk;

  /* STM32xxx **SECURE** HAL library initialization:
       - Secure Systick timer is configured by default as source of time base,
         but user can eventually implement his proper time base source (a general
         purpose timer for example or other time source), keeping in mind that
         Time base duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined
         and handled in milliseconds basis.
       - Low Level Initialization
     */


  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* !!! To boot in a secure way, the RoT has configured and activated the Memory Protection Unit
     In order to keep a secure environment execution, you should reconfigure the
     MPU to make it compatible with your application
     In this example, MPU is disabled */
  HAL_MPU_Disable();

  /* GTZC initialisation */
  MX_GTZC_Init();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();


  SecureInitIODone = 1;

  /* All IOs are by default allocated to secure */
  /* Release them all to non-secure except PC.07 (LED1) kept as secure */
#if defined(GPIOA)
  __HAL_RCC_GPIOA_CLK_ENABLE();
#endif /* GPIOA */
#if defined(GPIOB)
  __HAL_RCC_GPIOB_CLK_ENABLE();
#endif /* GPIOB */
#if defined(GPIOC)
  __HAL_RCC_GPIOC_CLK_ENABLE();
#endif /* GPIOC */
#if defined(GPIOD)
  __HAL_RCC_GPIOD_CLK_ENABLE();
#endif /* GPIOD */
#if defined(GPIOE)
  __HAL_RCC_GPIOE_CLK_ENABLE();
#endif /* GPIOE */
#if defined(GPIOG)
  __HAL_RCC_GPIOG_CLK_ENABLE();
#endif /* GPIOG */
#if defined(GPIOH)
  __HAL_RCC_GPIOH_CLK_ENABLE();
#endif /* GPIOH */

#if defined(GPIOA)
  HAL_GPIO_ConfigPinAttributes(GPIOA, GPIO_PIN_All, GPIO_PIN_NSEC);
#endif /* GPIOA */
#if defined(GPIOB)
  HAL_GPIO_ConfigPinAttributes(GPIOB, GPIO_PIN_All, GPIO_PIN_NSEC);
#endif /* GPIOB */
#if defined(GPIOC)
  HAL_GPIO_ConfigPinAttributes(GPIOC, GPIO_PIN_All, GPIO_PIN_NSEC);
#endif /* GPIOC */
#if defined(GPIOD)
  HAL_GPIO_ConfigPinAttributes(GPIOD, GPIO_PIN_All, GPIO_PIN_NSEC);
#endif /* GPIOD */
#if defined(GPIOE)
  HAL_GPIO_ConfigPinAttributes(GPIOE, GPIO_PIN_All, GPIO_PIN_NSEC);
#endif /* GPIOE */
#if defined(GPIOG)
  HAL_GPIO_ConfigPinAttributes(GPIOG, GPIO_PIN_All, GPIO_PIN_NSEC);
#endif /* GPIOG */
#if defined(GPIOH)
  HAL_GPIO_ConfigPinAttributes(GPIOH, GPIO_PIN_All, GPIO_PIN_NSEC);
#endif /* GPIOH */


  /* remove from NonSecure the PIN reserved for Secure */
  HAL_GPIO_ConfigPinAttributes(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SEC);
#if !defined(USE_STM32WBA55G_DK1)
  HAL_GPIO_ConfigPinAttributes(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SEC);
#endif /* USE_STM32WBA55G_DK1 */
  /* Leave the GPIO clocks enabled to let non-secure having I/Os control */

  /* Secure SysTick should rather be suspended before calling non-secure  */
  /* in order to avoid wake-up from sleep mode entered by non-secure      */
  /* The Secure SysTick shall be resumed on non-secure callable functions */
  /* For the purpose of this example, however the Secure SysTick is kept  */
  /* running to toggle the secure IO and the following is commented:      */
  /* HAL_SuspendTick(); */


  /*************** Setup and jump to non-secure *******************************/

  NonSecure_Init();

  /* Non-secure software does not return, this code is not executed */

  /* Infinite loop */

  while (1)
  {

  }
}

/**
  * @brief  Non-secure call function
  *         This function is responsible for Non-secure initialization and switch
  *         to non-secure state
  * @retval None
  */
static void NonSecure_Init(void)
{
  funcptr_NS NonSecure_ResetHandler;

  SCB_NS->VTOR = VTOR_TABLE_NS_START_ADDR;

  /* Set non-secure main stack (MSP_NS) */
  __TZ_set_MSP_NS((*(uint32_t *)VTOR_TABLE_NS_START_ADDR));

  /* Get non-secure reset handler */
  NonSecure_ResetHandler = (funcptr_NS)(*((uint32_t *)((VTOR_TABLE_NS_START_ADDR) + 4U)));

  /* Start non-secure state software application */
  NonSecure_ResetHandler();
}

/**
  * @brief  Unsecure Area of SRAM1
  * @retval None
  */
static void  unsecure_sram1(uint32_t start, uint32_t end)
{

  MPCBB_ConfigTypeDef MPCBB_desc;
  uint32_t regwrite = 0xffffffff;
  uint32_t index;
  uint32_t block_start = (start - 0x20000000) / (GTZC_MPCBB_BLOCK_SIZE);
  uint32_t block_end = block_start + ((end - start) + 1) / (GTZC_MPCBB_BLOCK_SIZE);

  if (start & 0xff)
    /*  Check alignment to avoid further problem  */
    /*  FIX ME  */
    while (1);

  if (HAL_GTZC_MPCBB_GetConfigMem(SRAM1_BASE, &MPCBB_desc) != HAL_OK)
  {
    /* FIX ME */
    Error_Handler();
  }

  for (index = 0; index < SRAM1_SIZE / GTZC_MPCBB_BLOCK_SIZE; index++)
  {
    /* clean register on index aligned */
    if (!(index & 0x1f))
    {
      regwrite = 0xffffffff;
    }
    if ((index >= block_start) && (index < block_end))
    {
      regwrite = regwrite & ~(1 << (index & 0x1f));
    }
    /* write register when 32 sub block are set  */
    if ((index & 0x1f) == 0x1f)
    {
      MPCBB_desc.AttributeConfig.MPCBB_SecConfig_array[index >> 5] = regwrite;
    }
  }
  if (HAL_GTZC_MPCBB_ConfigMem(SRAM1_BASE, &MPCBB_desc) != HAL_OK)
    /* FIX ME */
  {
    Error_Handler();
  }

}
/**
  * @brief GTZC Initialization Function
  * @param None
  * @retval None
  */
static void MX_GTZC_Init(void)
{

  if (HAL_GTZC_TZSC_ConfigPeriphAttributes(GTZC_PERIPH_ICACHE_REG,
                                           GTZC_TZSC_PERIPH_SEC | GTZC_TZSC_PERIPH_NPRIV) != HAL_OK)
  {
    Error_Handler();
  }

  unsecure_sram1(NS_RAM_START, (NS_RAM_START + NS_RAM_SIZE - 1));
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LED1_CLK_ENABLE();
#if !defined(USE_STM32WBA55G_DK1)
  LED2_CLK_ENABLE();
#endif /* USE_STM32WBA55G_DK1 */

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : LED1_Pin */
  GPIO_InitStruct.Pin = LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);

#if !defined(USE_STM32WBA55G_DK1)
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : LED2_Pin */
  GPIO_InitStruct.Pin = LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED2_GPIO_Port, &GPIO_InitStruct);
#endif /* USE_STM32WBA55G_DK1 */
}

/**
  * @brief  SYSTICK callback.
  * @retval None
  */
void HAL_SYSTICK_Callback(void)
{
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{

  /* Insure LED1 is configured */
  if (SecureInitIODone != 0)
  {
    MX_GPIO_Init();
  }
  /* LED1 on */
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);

  /* Infinite loop */
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
