/**
  ******************************************************************************
  * @file    stm32wbaxx_hal_msp.c
  * @author  MCD Application Team
  * @brief   HAL MSP module.
  *
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
  @verbatim
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
    [..]

  @endverbatim
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32wbaxx_hal.h"

/** @addtogroup STM32WBAxx_HAL_Driver
  * @{
  */

/** @defgroup HAL_MSP HAL MSP module driver
  * @brief HAL MSP module.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions HAL MSP Private Functions
  * @{
  */

/**
  * @brief  Initializes the Global MSP.
  * @retval None
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_PWR_CLK_ENABLE();

  /* System interrupt init*/

  /* Peripheral interrupt init */
  /* GTZC_IRQn interrupt configuration */

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/**
  * @brief  DeInitializes the Global MSP.
  * @retval None
  */
void HAL_MspDeInit(void)
{
  __HAL_RCC_PWR_CLK_DISABLE();
}

/**
  * @brief  Initialize the PPP MSP.
  * @retval None
  */
/*
void HAL_PPP_MspInit(void)
{
}
*/

/**
  * @brief  DeInitialize the PPP MSP.
  * @retval None
  */
/*
void HAL_PPP_MspDeInit(void)
{
}
*/

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

