/**
  ******************************************************************************
  * @file    app_cli_runner.c
  * @author  MCD Application Team
  * @brief   Entry point for the CLI Runner
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2023 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "app_conf.h"

#include "st_mac_802_15_4_raw_svc.h"
#include "app_cli_runner.h"

/* Private typedef -----------------------------------------------------------*/
/* For some commands, we have to keep a context. We define here the complete context
 * of the app.
 */ 
typedef struct {
  uint8_t   channel;           /* Default channel to use for Tx/Rx */
  int8_t    tx_power;          /* Default power use for Tx */
  int8_t    cca_threshold;
  int8_t    last_rssi;
  int8_t    lqi_valid;         /* 0: lqi non-valid, valid otherwise */ 
  uint8_t   last_lqi;
  uint8_t   rx_frame_printing; /* If 0: do nothing; Print RX frame upon receiving otherwise */
} cli_fullContext_t;

/* Private define ------------------------------------------------------------*/
cli_fullContext_t cli_full_ctx;

/* Private macro -------------------------------------------------------------*/

/* External variables --------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* ST MAC Radio capabilities */
ST_MAC_raw_caps_t st_mac_caps;
/* Callbacks struct to pass to the MAC layer */
ST_MAC_Raw_event_callbacks_t st_mac_cbks;

/* Holds current status of RX requests */
app_cli_RX_result_t cli_rx_result;
app_cli_TX_result_t cli_tx_result;

/* Dynamic frame printing, holding value */
typedef struct {
  uint8_t payload[ST_MAC_RAW_RX_FRAME_MAX_SIZE];
  uint16_t payload_size;
  uint8_t rssi;
} app_cli_single_rx_t;

/* Maximum number of frame to hold at the same time */
app_cli_single_RX_t frame_to_print_array[RX_FRAME_MAX_NUM];
/* Number of frames waiting to be printed (range from 0..RX_FRAME_MAX_NUM), 0 if none */
uint8_t frame_to_print_nb = 0;
/* Index of most recent frame (range from 0..RX_FRAME_MAX_NUM-1), -1 if none */
int8_t frame_to_print_idx = -1;

/* Private functions ---------------------------------------------------------*/
/* Service end notification callback */
static void app_cli_ex_Notif_callback(MAC_RAW_State_t state);
static void app_cli_ex_single_RX_callback( const ST_MAC_raw_single_RX_event_t * p_RX_evt);
static void app_cli_ex_single_TX_callback( const ST_MAC_raw_single_TX_event_t * p_TX_evt);

static void app_cli_ex_get_tx_results(void);
static void app_cli_ex_get_rx_results(void);
static void app_cli_ex_get_ed_results(void);

static void app_cli_ex_Notif_callback(MAC_RAW_State_t state)
{
  switch (state) {
  case ST_MAC_TX:
    app_cli_ex_get_tx_results();
    break;
  case ST_MAC_RX:
    app_cli_ex_get_rx_results();
    break;
  case ST_MAC_ED:
    app_cli_ex_get_ed_results();
    break;
  case ST_MAC_UNITIALIZED:
    break;
  case ST_MAC_NOTIF:
  case ST_MAC_UNKNOWN:
  case ST_MAC_CCA:
    /* Should never go there */
    while(1);
  case ST_MAC_IDLE:
  default:
      break;
  }
}

static void app_cli_ex_single_RX_callback( const ST_MAC_raw_single_RX_event_t * p_RX_evt)
{
  bool save_payload = FALSE;

  if (p_RX_evt->rx_status == RX_SUCCESS)
  {
    cli_rx_result.packets_received++;
    save_payload = TRUE;
  }
  else if (p_RX_evt->rx_status == RX_REJECTED)
  {
    cli_rx_result.packets_rejected++;
  }
  else if (p_RX_evt->rx_status == RX_FILTERED)
  {
    cli_rx_result.packets_filtered++;
  }

  /* Update recent frame index and number */
  if (save_payload == TRUE)
  {
    frame_to_print_nb++;
    frame_to_print_nb = MIN(frame_to_print_nb, RX_FRAME_MAX_NUM);
    frame_to_print_idx++;
    frame_to_print_idx %= RX_FRAME_MAX_NUM;

    ble_memcpy(&frame_to_print_array[frame_to_print_idx].payload[0],
           p_RX_evt->payload_ptr, p_RX_evt->payload_len);

    frame_to_print_array[frame_to_print_idx].payload_len = p_RX_evt->payload_len;
    frame_to_print_array[frame_to_print_idx].rssi = p_RX_evt->rssi;
    frame_to_print_array[frame_to_print_idx].lqi = p_RX_evt->lqi;
  }

  cli_full_ctx.last_lqi = p_RX_evt->lqi;
  cli_full_ctx.lqi_valid = 1;
  cli_full_ctx.last_rssi = p_RX_evt->rssi;

  //app_cli_print("Received frame length %d, RSSI %d, LQI %d\r\n", p_RX_evt->payload_len, p_RX_evt->rssi, p_RX_evt->lqi);
  
  return;
}

static void app_cli_ex_single_TX_callback( const ST_MAC_raw_single_TX_event_t * p_TX_evt)
{
  if (p_TX_evt->tx_status == TX_SUCCESS)
  {
    cli_tx_result.frames_success++;
  }
  else if (p_TX_evt->tx_status == TX_ACK_TIMEOUT)
  {
    cli_tx_result.frames_noack++;
  }
  else if (p_TX_evt->tx_status == TX_FAILED)
  {
    cli_tx_result.frames_failed++;
  }

  if (p_TX_evt->ack_ptr != NULL)
  {
    cli_tx_result.ack_seen++;
  }
  return;
}

static void app_cli_ex_get_tx_results(void)
{
  /* Not display for continuous modulated/non-modulated */
  if (cli_tx_result.tx_type == ST_MAC_STANDARD_TX)
  {
    app_cli_print_tx_results(cli_tx_result.frames_success, cli_tx_result.frames_failed,
                             cli_tx_result.frames_noack, cli_tx_result.ack_seen);
  }
  memset(&cli_tx_result, 0, sizeof(app_cli_TX_result_t));

  cli_tx_result.tx_type = ST_MAC_NONE_TX;
}

static void app_cli_ex_get_rx_results(void)
{
  app_cli_single_RX_t * lst_frame = NULL;
  if ( (frame_to_print_idx != -1) && (frame_to_print_nb > 0) )
  {
    //lst_frame_length = frame_to_print_array[frame_to_print_idx].payload_size;
    //lst_frame_rssi = frame_to_print_array[frame_to_print_idx].rssi;
    lst_frame = &frame_to_print_array[frame_to_print_idx];
  }

  /* Print last frame received, clear the array */
  app_cli_print_rx_results(cli_rx_result.packets_received, cli_rx_result.packets_rejected,
                           lst_frame);
  frame_to_print_idx = -1;
  frame_to_print_nb = 0;
  
  memset(&cli_rx_result, 0, sizeof(app_cli_RX_result_t));
}

static void app_cli_ex_get_ed_results(void)
{
  ST_MAC_rw_EDscan_result_t RawEDReq = { 0 };
  MAC_Status_t status = ST_MAC_raw_EDscan_get_result(NULL, &RawEDReq);
  LL_UNUSED(status);
  app_cli_print_ed_cb(RawEDReq.ed);
}

void app_cli_ex_init(void)
{
  st_mac_cbks.p_Notif = &app_cli_ex_Notif_callback;
  st_mac_cbks.p_RX_Done = &app_cli_ex_single_RX_callback;
  st_mac_cbks.p_TX_Done = &app_cli_ex_single_TX_callback;
  
  /* Radio & MAC Layer init */
  MAC_Status_t mac_result = ST_MAC_raw_init(&st_mac_cbks);

  if (mac_result != MAC_SUCCESS)
    /* abort init */
    return;

  /* Retrieve radio capabilities */
  mac_result = ST_MAC_raw_get_caps(NULL, &st_mac_caps);
  LL_UNUSED(mac_result);
  
  /* Default config */ 
  memset(&cli_tx_result, 0, sizeof(app_cli_TX_result_t));
  memset(&cli_rx_result, 0, sizeof(app_cli_RX_result_t));

  cli_full_ctx.channel = 22U;
  cli_full_ctx.cca_threshold = st_mac_caps.min_cca_threshold;
  cli_full_ctx.last_lqi = LQI_DEFAULT_VALUE;
  cli_full_ctx.lqi_valid = 0;
  cli_full_ctx.last_rssi = RSSI_DEFAULT_VALUE;
  cli_full_ctx.rx_frame_printing = 0;

  app_cli_ex_set_power(st_mac_caps.max_tx_power);

  for (uint8_t i = 0; i < RX_FRAME_MAX_NUM; i++)
  {
    frame_to_print_array[i].payload_len = 0;
  }
}

uint8_t app_cli_ex_get_version(void)
{
  app_cli_print("PHY valid CLI version : %s\r\n", CLI_APP_VERSION);
  return 0;
}

uint8_t app_cli_ex_get_info(void)
{
  app_cli_print("FW Type : 802.15.4 and radio PHY validation CLI %s for Target %s\r\n", CLI_APP_VERSION, CLI_FW_TARGET);
  return 0;
}

uint8_t app_cli_ex_set_channel(const uint8_t channel)
{
  cli_full_ctx.channel = channel;
  return 0;
}

uint8_t app_cli_ex_set_power(const int8_t power)
{
  /* Sanity check on power */
  if ((power >= st_mac_caps.min_tx_power) &&
      (power <= st_mac_caps.max_tx_power)) {
    cli_full_ctx.tx_power = power;
    return 0;
  } else {
    return 1;
  }
}

uint8_t app_cli_ex_tx_continuous_start(void)
{
  MAC_Status_t status;

  ST_MAC_raw_TX_start_t RawTXReq = { 0 };

  if (ST_MAC_NONE_TX != cli_tx_result.tx_type)
  {
    return (uint8_t) MAC_HW_BUSY;
  }
  cli_tx_result.tx_type = ST_MAC_CONTINUOUS_TX;

  RawTXReq.channel_number = cli_full_ctx.channel;
  RawTXReq.power = cli_full_ctx.tx_power;
  RawTXReq.tx_type = ST_MAC_CONTINUOUS_TX;

  status = ST_MAC_raw_start_TX(NULL, &RawTXReq);

  return (uint8_t) status;
}

uint8_t app_cli_ex_tx_continuous_stop(void)
{
  return app_cli_ex_stop_tx();
}

uint8_t app_cli_ex_cw_start(const uint8_t channel)
{
  MAC_Status_t status;

  ST_MAC_raw_TX_start_t RawTXReq = { 0 };
  if (ST_MAC_NONE_TX != cli_tx_result.tx_type)
  {
    return (uint8_t) MAC_HW_BUSY;
  }

  cli_tx_result.tx_type = ST_MAC_CONTINUOUS_WAVE;
  RawTXReq.channel_number = channel;
  RawTXReq.power = cli_full_ctx.tx_power;
  RawTXReq.tx_type = ST_MAC_CONTINUOUS_WAVE;

  status = ST_MAC_raw_start_TX(NULL, &RawTXReq);

  return (uint8_t) status;
}

uint8_t app_cli_ex_cw_stop(void)
{
  return app_cli_ex_stop_tx();
}

/* packet expected format is '0x01,0x02,etc') */
uint8_t app_cli_ex_start_tx(uint8_t* packet, uint8_t length,uint32_t packetNb,
                            uint16_t delay_ms, uint8_t continue_tx)
{
  MAC_Status_t status;

  ST_MAC_raw_TX_start_t RawTXReq = { 0 };
  uint8_t pkt_to_send;
  char *token;

  cli_tx_result.tx_type = ST_MAC_STANDARD_TX;

  RawTXReq.channel_number = cli_full_ctx.channel;
  RawTXReq.power = cli_full_ctx.tx_power;
  RawTXReq.tx_type = ST_MAC_STANDARD_TX;
  RawTXReq.payload_len = length;
  RawTXReq.frames_number = packetNb;
  RawTXReq.delay_ms = delay_ms;
  RawTXReq.stopTx_if_failure = continue_tx;

  uint8_t i = 0U;
  token = strtok((char*) packet, ",");
  while ( (token != NULL) && (i<(length-2)) ) { 
    pkt_to_send = (uint8_t)strtoul(token+2, NULL, 16);
    RawTXReq.payload[i++] = pkt_to_send;
    token = strtok(NULL, ",");
  }
  
  status = ST_MAC_raw_start_TX(NULL, &RawTXReq);

  return (uint8_t)status;
}

uint8_t app_cli_ex_stop_tx()
{
  MAC_Status_t status = ST_MAC_raw_stop_TX();
  return (uint8_t)status;
}

uint8_t app_cli_ex_start_rx(const uint8_t rx_frame_printing, const uint16_t timeout_ms)
{
  MAC_Status_t status;

  cli_full_ctx.rx_frame_printing = rx_frame_printing;
  
  /* Start RX request */
  ST_MAC_raw_RX_start_t RawRxReq = { 0 };
  RawRxReq.channel_number = cli_full_ctx.channel;
  RawRxReq.period = (uint32_t) timeout_ms;
  RawRxReq.frames_number = 0; //Not supported for the time being

  status = ST_MAC_raw_start_RX(NULL, &RawRxReq);

  return (uint8_t)status;
}

uint8_t app_cli_ex_stop_rx(void)
{
  cli_full_ctx.rx_frame_printing = 0;
  MAC_Status_t status = ST_MAC_raw_stop_RX();
  return (uint8_t)status;
}

uint8_t app_cli_ex_set_cca_threshold(const int8_t cca_threshold)
{
  /* Sanity check on threshold */
  if ((cca_threshold >= st_mac_caps.min_cca_threshold) &&
      (cca_threshold <= st_mac_caps.max_cca_threshold)) {
    cli_full_ctx.cca_threshold = cca_threshold;
    return 0;
  } else {
    return 1;
  }
}

uint8_t app_cli_ex_get_cca_threshold(int8_t * cca_threshold)
{
  if (cca_threshold != NULL)
  {
    *cca_threshold = cli_full_ctx.cca_threshold;
    return 0;
  }
  else
  {
    return 1;
  }
}

uint8_t app_cli_ex_get_cca(uint8_t *channel_is_clear)
{
  ST_MAC_raw_CCA_t CCAReq;
  CCAReq.cca_threshold = cli_full_ctx.cca_threshold;
  CCAReq.channel_number = cli_full_ctx.channel;
  CCAReq.cca_result = 0xFF;
  MAC_Status_t status = ST_MAC_raw_start_CCA(NULL, &CCAReq);
  *channel_is_clear = CCAReq.cca_result;
  return (uint8_t)status;
}

uint8_t app_cli_ex_start_ed_scan(void)
{
  ST_MAC_raw_EDscan_t EdReq;
  EdReq.channel = cli_full_ctx.channel;
  MAC_Status_t status = ST_MAC_raw_EDscan(NULL, &EdReq);
  return (uint8_t)status;
}

uint8_t app_cli_ex_get_lqi(uint8_t* lqi)
{
  uint8_t result = 1;
  if ( (lqi != NULL) && (cli_full_ctx.lqi_valid !=0) )
  {
    *lqi = cli_full_ctx.last_lqi;
    cli_full_ctx.lqi_valid = 0;
    result = 0;
  }
  return result;
}

uint8_t app_cli_ex_get_rssi(int8_t* rssi)
{
  uint8_t result = 1;
  if ( (rssi != NULL) && (cli_full_ctx.last_rssi != RSSI_DEFAULT_VALUE) )
  {
    *rssi = cli_full_ctx.last_rssi;
    cli_full_ctx.last_rssi = RSSI_DEFAULT_VALUE;
    result = 0;
  }
  return result; 
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
