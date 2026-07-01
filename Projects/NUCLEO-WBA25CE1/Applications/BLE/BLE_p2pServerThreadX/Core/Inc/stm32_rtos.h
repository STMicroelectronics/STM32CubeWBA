/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32_rtos.h
  * @author  MCD Application Team
  * @brief   Include file for all RTOS/Sequencer can be used on WBA
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "tx_initialize.h"
#include "tx_thread.h"
#include "tx_semaphore.h"
#include "app_threadx.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* ThreadX priorities by default  */
#define TASK_PRIO_WPAN                          (11u)
#define TASK_PREEMP_WPAN                        (0u)

#define TASK_PRIO_IDLE                          (31u)
#define TASK_PREEMP_IDLE                        (0u)

/* USER CODE BEGIN TASK_Priority_Define */
#define TASK_PRIO_BUTTON_Bx                     (13u)
#define TASK_PREEMP_BUTTON_Bx                   (13u)

/* USER CODE END TASK_Priority_Define */

#define RTOS_MAX_THREAD                         (20u)

#define RTOS_STACK_SIZE_LARGE                   ( 1024u * 3u )
#define RTOS_STACK_SIZE_MODERATE                ( 2048u )
#define RTOS_STACK_SIZE_NORMAL                  ( 1024u )
#define RTOS_STACK_SIZE_REDUCED                 ( 512u )
#define RTOS_STACK_SIZE_SMALL                   ( 256u )
#define RTOS_STACK_SIZE_TINY                    ( configMINIMAL_STACK_SIZE )

/* Tasks stack sizes by default  */
#define TASK_STACK_SIZE_WPAN                    RTOS_STACK_SIZE_MODERATE
#define TASK_STACK_SIZE_IDLE                    RTOS_STACK_SIZE_REDUCED
/* USER CODE BEGIN TASK_Size_Define */
#define TASK_STACK_SIZE_BUTTON_Bx               RTOS_STACK_SIZE_NORMAL
#if defined(__GNUC__) && defined(DEBUG)
#undef TASK_STACK_SIZE_BUTTON_Bx
#define TASK_STACK_SIZE_BUTTON_Bx               RTOS_STACK_SIZE_MODERATE
#endif
/* USER CODE END TASK_Size_Define */

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported variables --------------------------------------------------------*/
extern TX_BYTE_POOL       *pBytePool;   /* ThreadX byte pool pointer for whole WPAN middleware */

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported functions prototypes ---------------------------------------------*/
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // STM32_RTOS_H
