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
/* ThreadX priorities by default  */
#define CFG_TASK_PRIO_MAC_LAYER                 5u
#define CFG_TASK_PREEMP_MAC_LAYER               0u

#define CFG_TASK_PRIO_LINK_LAYER                7u
#define CFG_TASK_PREEMP_LINK_LAYER              0u

#define CFG_TASK_PRIO_LINK_LAYER_TEMP           7u
#define CFG_TASK_PREEMP_LINK_LAYER_TEMP         0u

#define CFG_TASK_PRIO_ZIGBEE_LAYER              9u
#define CFG_TASK_PREEMP_ZIGBEE_LAYER            0u

#define CFG_TASK_PRIO_HW_RNG                    11u
#define CFG_TASK_PREEMP_HW_RNG                  0u

#define CFG_TASK_PRIO_ZIGBEE_NETWORK_FORM       16u
#define CFG_TASK_PREEMP_ZIGBEE_NETWORK_FORM     0u

#define CFG_TASK_PRIO_ZIGBEE_APP_START          17u
#define CFG_TASK_PREEMP_ZIGBEE_APP_START        0u

/* USER CODE BEGIN TASK_Priority_Define */
#define CFG_TASK_PRIO_BUTTON_SWx                13u
#define CFG_TASK_PREEMP_BUTTON_SWx              0u

/* USER CODE END TASK_Priority_Define */

#define RTOS_MAX_THREAD                         20u

#define RTOS_STACK_SIZE_LARGE                   ( 1024u * 3u )
#define RTOS_STACK_SIZE_ENHANCED                ( 1024u * 2u )
#define RTOS_STACK_SIZE_NORMAL                  ( 1024u )
#define RTOS_STACK_SIZE_REDUCED                 ( 512u )
#define RTOS_STACK_SIZE_SMALL                   ( 384u )

/* Tasks stack sizes by default  */
#define TASK_LINK_LAYER_STACK_SIZE              RTOS_STACK_SIZE_LARGE
#define TASK_LINK_LAYER_TEMP_STACK_SIZE         RTOS_STACK_SIZE_REDUCED
#define TASK_MAC_LAYER_STACK_SIZE               RTOS_STACK_SIZE_ENHANCED
#define TASK_ZIGBEE_LAYER_STACK_SIZE            RTOS_STACK_SIZE_LARGE
#define TASK_HW_RNG_STACK_SIZE                  RTOS_STACK_SIZE_REDUCED
#define TASK_ZIGBEE_NETWORK_FORM_STACK_SIZE     RTOS_STACK_SIZE_LARGE
#define TASK_ZIGBEE_APP_START_STACK_SIZE        RTOS_STACK_SIZE_NORMAL
/* USER CODE BEGIN TASK_Size_Define */
#define TASK_BUTTON_SWx_STACK_SIZE              RTOS_STACK_SIZE_SMALL

/* USER CODE END TASK_Size_Define */

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported variables --------------------------------------------------------*/
extern TX_MUTEX           LinkLayerMutex;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported functions prototypes ---------------------------------------------*/
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // STM32_RTOS_H
