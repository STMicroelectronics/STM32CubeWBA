/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_zigbee_debug_zd.h
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
#ifndef APP_ZIGBEE_DEBUG_ZD_H
#define APP_ZIGBEE_DEBUG_ZD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "app_common.h"
#include "zigbee.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported defines ------------------------------------------------------------*/
#define CLI_MAX_LINE_LEN                    384U 

enum zbcli_buf_status_t {
    BUF_STATUS_EMPTY,
    BUF_STATUS_READY,
    BUF_STATUS_OVERFLOW
};

struct zbcli_buf_t {
    uint8_t buf[CLI_MAX_LINE_LEN];
    uint16_t len;
    enum zbcli_buf_status_t status;
};


/* Exported functions prototypes -------------------------------- */
void cli_status_zdo(void);
void cli_print_aps_channel_mask(void);
void cli_status_nwk(void);
void cli_status_nnt(void);
void cli_security_dump(void);
void APP_ZIGBEE_InitStatusDebug(void);
void APP_ZIGBEE_App_UartRxCallback(uint8_t *pData, uint16_t iSize, uint8_t cError);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* APP_ZIGBEE_DEBUG_ZD_H */
