/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_freertos.h
  * @author  MCD Application Team
  * @brief   FreeRTOS applicative header file
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
#ifndef __APP_FREERTOS_H
#define __APP_FREERTOS_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os2.h"
//#include "cmsis_os.h"   

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern osTimerId_t advLowPowerTimerHandle;
extern osTimerId_t HRSAPPMeasurementsTimerHandle;

extern osSemaphoreId_t advertisingSemaphoreHandle;

extern osMessageQueueId_t advertisingCmdQueueHandle;

extern osMutexId_t             LinkLayerMutex;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
extern void* p_param;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Main thread defines -------------------------------------------------------*/
/* USER CODE BEGIN MTD */

/* USER CODE END MTD */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
//void MX_FreeRTOS_Start(void);
extern void MX_FreeRtos_Init        ( void );

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, char *pcTaskName);
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);
/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

#ifdef __cplusplus
}
#endif
#endif /* __APP_FREERTOS_H__ */
