/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
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


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined ( __ICCARM__ )
#  define CMSE_NS_CALL  __cmse_nonsecure_call
#  define CMSE_NS_ENTRY __cmse_nonsecure_entry
#else
#  define CMSE_NS_CALL  __attribute((cmse_nonsecure_call))
#  define CMSE_NS_ENTRY __attribute((cmse_nonsecure_entry))
#endif /* __ICCARM__  */

/* Includes ------------------------------------------------------------------*/
#include "stm32wbaxx_hal.h"

/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
/* Function pointer declaration in non-secure*/
#if defined ( __ICCARM__ )
typedef void (CMSE_NS_CALL *funcptr)(void);
#else
typedef void CMSE_NS_CALL(*funcptr)(void);
#endif /* __ICCARM__  */

/* typedef for non-secure callback functions */
typedef funcptr funcptr_NS;



/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);


/* Private defines -----------------------------------------------------------*/
#define LED1_Pin GPIO_PIN_4
#define LED1_GPIO_Port GPIOB
#define LED1_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED2_Pin  GPIO_PIN_11
#define LED2_GPIO_Port GPIOB
#define LED2_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H */
