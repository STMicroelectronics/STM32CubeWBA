/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32_rtos.h
  * @author  MCD Application Team
  * @brief   Include file for all RTOS/Sequencer can be used on WBA
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32_RTOS_H
#define STM32_RTOS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os2.h"
#include "app_freertos.h"
#include "task.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* FreeRTOS priorities by default  */
#define TASK_PRIO_LINK_LAYER                    osPriorityRealtime2
#define TASK_PRIO_TEMP_MEAS_LL                  osPriorityRealtime2
#define TASK_PRIO_RNG                           osPriorityHigh

#define TASK_PRIO_ALARM                         osPriorityNormal2
#define TASK_PRIO_US_ALARM                      TASK_PRIO_ALARM
#define TASK_PRIO_TASKLETS                      osPriorityRealtime3
#define TASK_PRIO_PKA                           osPriorityNormal1
#define TASK_PRIO_CLI_UART                      osPriorityNormal
#define TASK_PRIO_SEND                          osPriorityNormal
#define TASK_PRIO_RCP_SPINEL_RX                 osPriorityNormal

/* USER CODE BEGIN TASK_Priority_Define */
#define TASK_PRIO_BUTTON_Bx                     osPriorityNormal3

/* USER CODE END TASK_Priority_Define */

#define RTOS_MAX_THREAD                         (20u)

#define RTOS_STACK_SIZE_LARGE                   ( 1024u * 3u )
#define RTOS_STACK_SIZE_MODERATE                ( 2048u )
#define RTOS_STACK_SIZE_NORMAL                  ( 1024u )
#define RTOS_STACK_SIZE_REDUCED                 ( 512u )
#define RTOS_STACK_SIZE_SMALL                   ( 256u )
#define RTOS_STACK_SIZE_TINY                    ( configMINIMAL_STACK_SIZE )

/* Tasks stack sizes by default  */
#define TASK_STACK_SIZE_LINK_LAYER              RTOS_STACK_SIZE_LARGE
#define TASK_STACK_SIZE_TEMP_MEAS_LL            RTOS_STACK_SIZE_REDUCED
#define TASK_STACK_SIZE_RNG                     RTOS_STACK_SIZE_REDUCED
#define TASK_STACK_SIZE_ALARM                   RTOS_STACK_SIZE_MODERATE
#define TASK_STACK_SIZE_ALARM_US                RTOS_STACK_SIZE_NORMAL
#define TASK_STACK_SIZE_TASKLETS                RTOS_STACK_SIZE_LARGE
#define TASK_STACK_SIZE_CLI_UART                RTOS_STACK_SIZE_NORMAL
#define TASK_STACK_SIZE_SEND                    RTOS_STACK_SIZE_NORMAL
#define TASK_STACK_SIZE_PKA                     RTOS_STACK_SIZE_NORMAL
#define TASK_STACK_SIZE_RCP_SPINEL_RX           RTOS_STACK_SIZE_NORMAL
/* USER CODE BEGIN TASK_Size_Define */
#define TASK_STACK_SIZE_BUTTON_Bx               RTOS_STACK_SIZE_NORMAL

/* USER CODE END TASK_Size_Define */

/* Attributes needed by CMSIS */
#define TASK_DEFAULT_ATTR_BITS                  ( 0u )
#define TASK_DEFAULT_CB_MEM                     ( 0u )
#define TASK_DEFAULT_CB_SIZE                    ( 0u )
#define TASK_DEFAULT_STACK_MEM                  ( 0u )

#define SEMAPHORE_DEFAULT_ATTR_BITS             ( 0u )
#define SEMAPHORE_DEFAULT_CB_MEM                ( 0u )
#define SEMAPHORE_DEFAULT_CB_SIZE               ( 0u )

#define MUTEX_DEFAULT_ATTR_BITS                 ( 0u )
#define MUTEX_DEFAULT_CB_MEM                    ( 0u )
#define MUTEX_DEFAULT_CB_SIZE                   ( 0u )

/* USER CODE BEGIN Attributes_Define */

/* USER CODE END Attributes_Define */

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */
#if (CFG_BUTTON_SUPPORTED == 1)
/* Button management */
extern osSemaphoreId_t    ButtonSw1Semaphore, ButtonSw2Semaphore, ButtonSw3Semaphore;
#endif /* (CFG_BUTTON_SUPPORTED == 1) */

/* USER CODE END EV */

/* Exported functions prototypes ---------------------------------------------*/
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // STM32_RTOS_H
