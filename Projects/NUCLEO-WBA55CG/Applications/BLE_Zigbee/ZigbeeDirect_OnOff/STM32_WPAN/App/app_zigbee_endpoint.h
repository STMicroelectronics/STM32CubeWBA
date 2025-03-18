/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_zigbee_endpoint.h
  * Description        : Header for Zigbee Application and it endpoint.
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
#ifndef APP_ZIGBEE_ENDPOINT_H
#define APP_ZIGBEE_ENDPOINT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "app_common.h"
#include "zigbee.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported defines ------------------------------------------------------------*/



/* Exported functions prototypes -------------------------------- */
extern void       APP_ZIGBEE_ApplicationInit(void);
extern void       APP_ZIGBEE_App_UartRxCallback(uint8_t *pData, uint16_t iSize, uint8_t cError);
extern void       cli_port_print_fmt(const char *hdr, const char *fmt, ...);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* APP_ZIGBEE_ENDPOINT_H */
