/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32_rtos.h
  * @author  MCD Application Team
  * @brief   Include file for all RTOS/Sequencer can be used on WBA
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32_RTOS_H
#define STM32_RTOS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "tx_api.h"
#include "app_threadx.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
#define RTOS_MAX_THREAD			        20u
  
#define RTOS_STACK_SIZE_LARGE                 ( 1024u * 3u )
#define RTOS_STACK_SIZE_ENHANCED 	      ( 1024u * 2u )
#define RTOS_STACK_SIZE_NORMAL		      ( 1024u )
#define RTOS_STACK_SIZE_REDUCED		      ( 512u )
#define RTOS_STACK_SIZE_SMALL	              ( 384u )
#define RTOS_BLOCK_POOL_SIZE        	      ( 100u )
  
/* Tasks stack sizes by default  */
#define TASK_LINK_LAYER_STACK_SIZE            RTOS_STACK_SIZE_LARGE
#define TASK_HW_RNG_STACK_SIZE                RTOS_STACK_SIZE_REDUCED   
  
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported functions prototypes ---------------------------------------------*/
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // STM32_RTOS_H
