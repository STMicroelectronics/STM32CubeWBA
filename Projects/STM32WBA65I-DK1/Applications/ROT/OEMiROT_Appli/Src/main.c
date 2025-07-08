/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   Main application file.
  *          This application demonstrates Secure Services
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

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "com.h"
#include "common.h"
#include "appli_region_defs.h"
#include "main.h"
#include "Driver_Flash.h"
#if defined(USE_STM32WBA65I_DK1)
#include "stm32wba65i_discovery_conf.h"
#elif defined(USE_STM32WBA55G_DK1)
#include "stm32wba55g_discovery_conf.h"
#endif

extern ARM_DRIVER_FLASH FLASH_DEV_NAME;
#if defined(OEMIROT_EXTERNAL_FLASH_ENABLE)
extern ARM_DRIVER_FLASH SPI_FLASH_DEV_NAME;
#endif /* OEMIROT_EXTERNAL_FLASH_ENABLE */

/* Avoids the semihosting issue */
#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
__asm("  .global __ARM_use_no_argv\n");
#endif /* defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) */
#include "fw_update_app.h"
#include "s_data.h"

#if defined(__ICCARM__)
#include <LowLevelIOInterface.h>
#endif /* __ICCARM__ */

/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint32_t SecureInitIODone = 0;
uint8_t *pUserAppId;
const uint8_t UserAppId = 'A';

/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
static void MX_GTZC_Init(void);
#if defined(OEMIROT_EXTERNAL_FLASH_ENABLE)
static void SPI_Init(void);
#endif  /* defined(OEMIROT_EXTERNAL_FLASH_ENABLE) */
void FW_APP_PrintMainMenu(void);
void FW_APP_Run(void);

#if defined(__ICCARM__) || defined ( __CC_ARM ) || defined(__ARMCC_VERSION)
int io_putchar(int ch);
#define PUTCHAR_PROTOTYPE int io_putchar(int ch)
#elif defined(__GNUC__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#endif /* __ICCARM__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  */
PUTCHAR_PROTOTYPE
{
  COM_Transmit((uint8_t*)&ch, 1, TX_TIMEOUT);
  return ch;
}

/* Redirects printf to DRIVER_STDIO in case of ARMCLANG*/
#if defined(__ARMCC_VERSION)
FILE __stdout;

/* __ARMCC_VERSION is only defined starting from Arm compiler version 6 */
int fputc(int ch, FILE *f)
{
  /* Send byte to USART */
  io_putchar(ch);

  /* Return character written */
  return ch;
}
#elif defined(__GNUC__)
/* Redirects printf to DRIVER_STDIO in case of GNUARM */
int _write(int fd, char *str, int len)
{
  int i;

  for (i = 0; i < len; i++)
  {
    /* Send byte to USART */
    __io_putchar(str[i]);
  }

  /* Return the number of characters written */
  return len;
}
#elif defined(__ICCARM__)
size_t __write(int file, unsigned char const *ptr, size_t len)
{
  size_t idx;
  unsigned char const *pdata = ptr;

  for (idx=0; idx < len; idx++)
  {
    io_putchar((int)*pdata);
    pdata++;
  }
  return len;
}
#endif /*  __GNUC__ */

/* Private user code ---------------------------------------------------------*/


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /*  set example to const : this const changes in binary without rebuild */
  pUserAppId = (uint8_t *)&UserAppId;

  /* SAU/IDAU, FPU and interrupts secure/non-secure allocation setup done */
  /* in SystemInit() based on partition_stm32xxx.h file's definitions. */


  /* Enable SecureFault handler (HardFault is default) */
  SCB->SHCSR |= SCB_SHCSR_SECUREFAULTENA_Msk;

  /* STM32xxx HAL library initialization:
  - Systick timer is configured by default as source of time base, but user
  can eventually implement his proper time base source (a general purpose
  timer for example or other time source), keeping in mind that Time base
  duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
  handled in milliseconds basis.
  - Set NVIC Group Priority to 3
  - Low Level Initialization
  */
  HAL_Init();

  /* !!! To boot in a secure way, the RoT has configured and activated the Memory Protection Unit
     In order to keep a secure environment execution, you should reconfigure the
     MPU to make it compatible with your application
     In this example, MPU is disabled */
  HAL_MPU_Disable();

  /* DeInitialize RCC to allow PLL reconfiguration when configuring system clock */
  HAL_RCC_DeInit();

  /* GTZC initialisation */
  MX_GTZC_Init();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  /* Configure Communication module */
  COM_Init();

#if defined(OEMIROT_EXTERNAL_FLASH_ENABLE)
  /* Configure SPI module */
  SPI_Init();
#endif  /* defined(OEMIROT_EXTERNAL_FLASH_ENABLE) */

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

  /* Leave the GPIO clocks enabled to let non-secure having I/Os control */

  /* Secure SysTick should rather be suspended before calling non-secure  */
  /* in order to avoid wake-up from sleep mode entered by non-secure      */
  /* The Secure SysTick shall be resumed on non-secure callable functions */
  /* For the purpose of this example, however the Secure SysTick is kept  */
  /* running to toggle the secure IO and the following is commented:      */
  /* HAL_SuspendTick(); */

  printf("\r\n======================================================================");
  printf("\r\n=              (C) COPYRIGHT 2024 STMicroelectronics                 =");
  printf("\r\n=                                                                    =");
  printf("\r\n=                          User App #%c                               =", *pUserAppId);
  printf("\r\n======================================================================");
  printf("\r\n\r\n");

  if ( FLASH_DEV_NAME.Initialize(NULL) != ARM_DRIVER_OK)
  {
    printf("Driver Flash Init : Failed");
    Error_Handler();
  }
#if defined(OEMIROT_EXTERNAL_FLASH_ENABLE)
  if (SPI_FLASH_DEV_NAME.Initialize(NULL) != ARM_DRIVER_OK)
  {
    printf("Error while initializing EEPROM Interface");
    Error_Handler();
  }
#endif /* OEMIROT_EXTERNAL_FLASH_ENABLE */

  /* User App firmware runs*/
  FW_APP_Run();

  /* Infinite loop */
  while (1U)
  {}

}

#if defined(OEMIROT_EXTERNAL_FLASH_ENABLE)
/**
  * @brief SPI Initialization Function
  * @param None
  * @retval None
  */
static void SPI_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  M95P32_EEPROM_SPI_CS_RCC();

  /*Configure GPIO pin : M95P32_EEPROM_SPI_CS_PIN */
  GPIO_InitStruct.Pin = M95P32_EEPROM_SPI_CS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(M95P32_EEPROM_SPI_CS_PORT, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}
#endif  /* defined(OEMIROT_EXTERNAL_FLASH_ENABLE) */

/**
  * @brief  Display the TEST Main Menu choices on HyperTerminal
  * @param  None.
  * @retval None.
  */
void FW_APP_PrintMainMenu(void)
{
  printf("\r\n===================== Main Menu ==========================\r\n\n");
#if !defined(MCUBOOT_PRIMARY_ONLY)
  printf("  New Firmware Image ------------------------------------ 1\r\n\n");
#endif /* (MCUBOOT_PRIMARY_ONLY) */
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
  printf("  Display Secure Data ----------------------------------- 2\r\n\n");
#endif /* (MCUBOOT_S_DATA_IMAGE_NUMBER == 1) */
  printf("  Selection :\r\n\n");
}

/**
  * @brief  Display the TEST Main Menu choices on HyperTerminal
  * @param  None.
  * @retval None.
  */
void FW_APP_Run(void)
{
  uint8_t key = 0U;

  /*##1- Print Main Menu message*/
  FW_APP_PrintMainMenu();

  while (1U)
  {
    /* Clean the input path */
    COM_Flush();

    /* Receive key */
    if (COM_Receive(&key, 1U, RX_TIMEOUT) == HAL_OK)
    {
      switch (key)
      {
#if !defined(MCUBOOT_PRIMARY_ONLY)
        case '1' :
          FW_UPDATE_Run();
          break;
#endif /* (MCUBOOT_PRIMARY_ONLY) */
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
        case '2' :
          S_DATA_Run();
          break;
#endif /* (MCUBOOT_S_DATA_IMAGE_NUMBER == 1) */
        default:
          printf("Invalid Number !\r");
          break;
      }

      /* Print Main Menu message */
      FW_APP_PrintMainMenu();
    }
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
  while (1U)
  {
  }
}
#endif /* USE_FULL_ASSERT */
