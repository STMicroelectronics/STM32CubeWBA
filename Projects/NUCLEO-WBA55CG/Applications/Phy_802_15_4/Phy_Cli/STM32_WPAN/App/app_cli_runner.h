/**
  ******************************************************************************
  * @file    app_cli_runner.h
  * @author  MCD Application Team
  * @brief   Header for the macros of PHY Runner application
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_CLI_RUNNER_H
#define APP_CLI_RUNNER_H

#include "st_mac_802_15_4_raw_svc.h"

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef struct {
  int    paramType;
  char * paramStr;
} param_t;

typedef struct {
  char *  cmd;         // command name
  int     nArgsMin;    // minimum argument number for this command
  int     nArgsMax;    // maximum argument number for this command
  param_t args[10];    // argument list (max is 10)
} cliCmd_t;


/* Exported defines ----------------------------------------------------------*/
#define LQI_DEFAULT_VALUE       0x00
#define RSSI_DEFAULT_VALUE      -127
#define RX_BUFFER_MAX_SIZE      127
#define RX_FRAME_MAX_NUM        10

/** @brief Defines the structure for returning the status of single RX
 */
typedef struct {
  /*! Payload of last frame received */
  uint8_t payload[RX_BUFFER_MAX_SIZE];
  /*! Size of last frame received */
  uint8_t payload_len;
  /*! RSSI of last frame received */
  int8_t rssi;
   /*! LQI of last frame received */
  uint8_t lqi; 
} app_cli_single_RX_t;

/** @brief Defines the structure for returning the result of a global RX request
 */
typedef struct {
  /*! Frames received */
  uint16_t packets_received;
  /*! Frames rejected */
  uint16_t packets_rejected;
  /*! Frames filtered */
  uint16_t packets_filtered;
} app_cli_RX_result_t;

/** @brief Defines the structure for returning the result of a global TX request
 */
typedef struct {
  /*! TX type */
  MAC_RAW_TX_Type_t tx_type;
  /*! Transmission ended with success (Tx Done with no error) */
  uint16_t frames_success;
  /*! Transmission ended with ack timeout */
  uint16_t frames_noack;
  /*! Transmission ended with error code (other than ack timeout) */
  uint16_t frames_failed;
  /*! Number of ACK received */
  uint16_t ack_seen;
} app_cli_TX_result_t;


/* Exported constants --------------------------------------------------------*/
#define CLI_PROMPT              "802.15.4 valid cli >"
#define CLI_NAME                "(STM32WBA5xx) PHY CLI\r\n"
#define CLI_APP_VERSION         "v1.3.0"

#if defined(STM32WBA52xx)
#define CLI_FW_TARGET           "WBA52"
#elif defined(STM32WBA55xx)
#define CLI_FW_TARGET           "WBA55"
#else
#error "unknown target"
#endif

/* Exported variables --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void APP_PHY_CLI_Init(void);
void app_cli_print(const char *aFormat, ...);
void app_cli_print_tx_results(const uint32_t frm_success, const uint32_t frm_failed,
                              const uint32_t frm_nack, const uint32_t frm_ack);
void app_cli_print_rx_results(const uint16_t packets_received, const uint16_t packets_rejected,
                              const app_cli_single_RX_t *p_last_received_frame);
void app_cli_print_ed_cb(const uint8_t ed);
                              
/* Extended functions */
void app_cli_ex_init(void);
uint8_t app_cli_ex_get_version(void);
uint8_t app_cli_ex_get_info(void);
uint8_t app_cli_ex_set_channel(const uint8_t channel);
uint8_t app_cli_ex_set_power(const int8_t power);
uint8_t app_cli_ex_tx_continuous_start(void);
uint8_t app_cli_ex_tx_continuous_stop(void);
uint8_t app_cli_ex_cw_start(const uint8_t channel);
uint8_t app_cli_ex_cw_stop(void);
uint8_t app_cli_ex_start_tx(uint8_t* packet, uint8_t length,uint32_t packetNb,
                            uint16_t delay_ms, uint8_t continue_tx);
uint8_t app_cli_ex_stop_tx(void);
uint8_t app_cli_ex_start_rx(const uint8_t rx_frame_printing, const uint16_t timeout_ms);
uint8_t app_cli_ex_stop_rx(void);
uint8_t app_cli_ex_set_cca_threshold(const int8_t cca_threshold);
uint8_t app_cli_ex_get_cca_threshold(int8_t * cca_threshold);
uint8_t app_cli_ex_get_cca(uint8_t *channel_is_clear);
uint8_t app_cli_ex_start_ed_scan(void);
uint8_t app_cli_ex_get_lqi(uint8_t *lqi);
uint8_t app_cli_ex_get_rssi(int8_t *rssi);

#endif /* APP_CLI_RUNNER_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
