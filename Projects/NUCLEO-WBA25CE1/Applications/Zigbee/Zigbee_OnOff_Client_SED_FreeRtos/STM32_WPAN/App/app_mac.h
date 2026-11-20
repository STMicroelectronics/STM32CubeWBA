/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Applications\802.15.4\MAC_802.15.4\Inc
  * @author  MCD Application Team
  * @brief   Header for app_mac_user.h module
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
#ifndef APP_MAC_H
#define APP_MAC_H

#include "stm_queue.h"
#include "app_conf.h"

/* Exported constants --------------------------------------------------------*/
#define EVENT_SET_CNF                       EVENT_ZIGBEE_APP2
#define EVENT_DEVICE_RESET_CNF              EVENT_ZIGBEE_APP3
#define EVENT_DEVICE_STARTED_CNF            EVENT_ZIGBEE_APP4

/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

void APP_MAC_Init(void);

#endif /* APP_MAC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
