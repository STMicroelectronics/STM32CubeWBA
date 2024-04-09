/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_sys.h
  * @author  MCD Application Team
  * @brief   Header for app_sys.c
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
#ifndef APP_SYS_H
#define APP_SYS_H

/* Exported constants --------------------------------------------------------*/
/*
 * high ceil for standby exit -> 500us in theory NOTE: Minimum value for UTIL Timer is 1ms
 * high ceil for radio exit deep sleep -> 300us
 * high ceil for radio entry deep ceil -> 50us
 */
#define RADIO_DEEPSLEEP_WAKEUP_TIME_US (1500)

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported functions prototypes ---------------------------------------------*/

void APP_SYS_BLE_EnterDeepSleep(void);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#endif /* APP_SYS_H */
