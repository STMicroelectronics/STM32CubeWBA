/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : app_zigbee_zbcli.h
 * Description        : Header for Zigbee CLI Application.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_ZIGBEE_H
#define APP_ZIGBEE_H

#include "zigbee.h"
#include "zigbee.zd.h"

#define CLI_MAX_LINE_LEN                    384U /* was C_SIZE_CMD_STRING (256) */

#define CLI_CMD_QUEUE_SZ                    4U

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

struct app_info_t {
    struct ZigBeeT *zb;
    uint64_t eui64;
    bool savePersistence;

    struct ZbZclClusterT *onoff_server;
    bool onoff_state;

    /* UART */
    struct zbcli_buf_t buf_ring[CLI_CMD_QUEUE_SZ];
    unsigned int buf_wr_idx;
    unsigned int buf_rd_idx;
};

extern struct app_info_t app_info;

extern void     APP_ZIGBEE_StackLayersInit(void);
extern void     APP_ZIGBEE_Init(void);
extern  void    APP_ZIGBEE_Task(void);

#define APP_LOG_PRINTF(...) \
    do { \
        cli_port_print_fmt(__func__, __VA_ARGS__); \
    } while (false)

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
