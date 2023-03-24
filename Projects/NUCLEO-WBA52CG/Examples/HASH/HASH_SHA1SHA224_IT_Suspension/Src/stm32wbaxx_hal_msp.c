/**
  ******************************************************************************
  * @file    stm32wbaxx_hal_msp.c
  * @author  MCD Application Team
  * @brief   HAL MSP module.
  *
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

}

/**
  * @brief  DeInitializes the Global MSP.
  * @retval None
  */
void HAL_MspDeInit(void)
{

}

/**
  * @brief  Initializes the HASH MSP.
  *        This function configures the hardware resources used in this example:
  *           - HASH clock enable
  * @param  hhash: HASH handle pointer
  * @retval None
  */
void HAL_HASH_MspInit(HASH_HandleTypeDef *hhash)
{

  /* Enable HASH clock */
  __HAL_RCC_HASH_CLK_ENABLE();
  
  /* Enable and set HASH Interrupt to the highest priority */
  HAL_NVIC_SetPriority(HASH_IRQn, NVIC_PRIORITYGROUP_4, 0);
  HAL_NVIC_EnableIRQ(HASH_IRQn);  
  
}

/**
  * @brief  De-Initializes the HASH MSP.
  * @param  hhash: HASH handle pointer
  * @retval None
  */
void HAL_HASH_MspDeInit(HASH_HandleTypeDef *hhash)
{
  /* Force the HASH Periheral Clock Reset */  
  __HAL_RCC_HASH_FORCE_RESET(); 
  
  /* Release the HASH Periheral Clock Reset */  
  __HAL_RCC_HASH_RELEASE_RESET();
  
  /* Disable HASH clock */
  __HAL_RCC_HASH_CLK_DISABLE();
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

