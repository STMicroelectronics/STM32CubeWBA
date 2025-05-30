/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_entry.h
  * @author  MCD Application Team
  * @brief   Interface to the application
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#ifndef APP_ENTRY_H
#define APP_ENTRY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Private includes ----------------------------------------------------------*/
#include "app_common.h"
#include "tx_api.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
#define WPAN_SUCCESS 0u

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported variables --------------------------------------------------------*/
extern TX_BYTE_POOL *pBytePool; /* ThreadX byte pool pointer for whole WPAN middleware */
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void MX_APPE_Config(void);
uint32_t MX_APPE_Init(void *p_param);
void MX_APPE_LinkLayerInit(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*APP_ENTRY_H */
