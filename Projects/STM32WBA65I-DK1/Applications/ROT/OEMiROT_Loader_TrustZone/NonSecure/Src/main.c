/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   Main application file.
  *          This application demonstrates Firmware Update, protections
  *          and crypto testing functionalities.
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
#include "loader_flash_layout.h"
#include "Driver_Flash.h"
#if defined(USE_STM32WBA65I_DK1) && defined(OEMIROT_EXTERNAL_FLASH_ENABLE)
#include "stm32wba65i_discovery_conf.h"
#endif  /* defined(USE_STM32WBA65I_DK1) */

extern ARM_DRIVER_FLASH LOADER_FLASH_DEV_NAME;

/* Avoids the semihosting issue */
#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
__asm("  .global __ARM_use_no_argv\n");
#endif /* defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) */

#include "fw_update_app.h"

#if defined(__ICCARM__)
#include <LowLevelIOInterface.h>
#endif /* __ICCARM__ */

#if defined(__ICCARM__) || defined ( __CC_ARM ) || defined(__ARMCC_VERSION)
int io_putchar(int ch);
#define PUTCHAR_PROTOTYPE int io_putchar(int ch)
#elif defined(__GNUC__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#endif /* __ICCARM__ */

/** @defgroup  USER_APP  exported variable
   * @{
  */

/** @addtogroup USER_APP User App Example
  * @{
  */


/** @addtogroup USER_APP_COMMON Common
  * @{
  */
/* Private function prototypes -----------------------------------------------*/
#if defined(OEMIROT_EXTERNAL_FLASH_ENABLE)
static void SPI_Init(void);
#endif  /* defined(OEMIROT_EXTERNAL_FLASH_ENABLE) */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

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

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(int argc, char **argv)
/*int main(void) */
{

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

  /* Configure Communication module */
  COM_Init();

#if defined(OEMIROT_EXTERNAL_FLASH_ENABLE)
  /* Configure SPI module */
  SPI_Init();
#endif  /* defined(OEMIROT_EXTERNAL_FLASH_ENABLE) */

  printf("\r\n======================================================================");
  printf("\r\n=              (C) COPYRIGHT 2024 STMicroelectronics                 =");
  printf("\r\n=                                                                    =");
  printf("\r\n=                          LOCAL LOADER                              =");
  printf("\r\n======================================================================");
  printf("\r\n\r\n");

  if ( LOADER_FLASH_DEV_NAME.Initialize(NULL) != ARM_DRIVER_OK)
  {
    printf("Driver Flash Init : Failed");
  }
  /* User App firmware runs*/
  FW_UPDATE_Run();

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

/**
  * @}
  */

/**
  * @}
  */
