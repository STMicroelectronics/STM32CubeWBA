/**
  ******************************************************************************
  * @file    st_mac_802_15_4_raw_svc.h
  * @author
  * @brief   Header for mac raw service
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

#ifndef _802_15_4_MAC_RAW_SVC_H_
#define _802_15_4_MAC_RAW_SVC_H_

/* Includes ------------------------------------------------------------------*/
#include "st_mac_802_15_4_types.h"

#include "mac_host_intf.h"

/* Exported defines ----------------------------------------------------------*/

/** @brief Max size of payload reception (CRC excluded) */
#define ST_MAC_RAW_RX_PAYLOAD_MAX_SIZE 125

/** @brief Max size of payload reception (CRC included) */
#define ST_MAC_RAW_RX_FRAME_MAX_SIZE ST_MAC_RAW_RX_PAYLOAD_MAX_SIZE + 2


/******************************************************************************/
/** @brief Holder for events, to be accessed through macros. In general, each events
 should have a set, clear, is_set macros.
 */
extern uint32_t mac_radio_evts;

/* Exported types ------------------------------------------------------------*/
/** @brief Defines the current operation of the radio
 */
typedef enum {
  ST_MAC_UNITIALIZED = 0,
  ST_MAC_IDLE,
  ST_MAC_TX,
  ST_MAC_RX,
  ST_MAC_ED,
  ST_MAC_CCA,
  ST_MAC_NOTIF,
  ST_MAC_UNKNOWN, //Keep last
} MAC_RAW_State_t;

/** @brief Defines the type of transmission
 */
typedef enum {
  ST_MAC_NONE_TX = 0,
  /* Standard TX with paylaod */
  ST_MAC_STANDARD_TX,
  /* Continuous PRBS noise */
  ST_MAC_CONTINUOUS_TX,
  /* Unmodulated continuous TX */
  ST_MAC_CONTINUOUS_WAVE,
  ST_MAC_UNKNOWN_TX, //keep last
} MAC_RAW_TX_Type_t;

/** @brief TX status
 */
typedef enum {
  TX_SUCCESS = 0,
  TX_ACK_TIMEOUT,
  TX_FAILED,
  TX_CHANNEL_ACCESS_FAILURE,
} MAC_RAW_TX_Status_t;

/** @brief TX status
 */
typedef enum {
  RX_SUCCESS = 0,
  RX_NO_PAYLOAD,
  RX_REJECTED,
  RX_FILTERED,
  RX_INVALID_FCS,
} MAC_RAW_RX_Status_t;

/** @brief Initialisation config 
 */
typedef enum {
  /*! 802.15.4 PHY ONLY */
  RAW_CONFIG = 0,
  /*! 802.15.4 custom/proprietary */
  EXT_CONFIG, // NOT YET AVAILABLE
  /*! 802.15.4 PHY + BLE */
  CR_CONFIG
} ST_MAC_Config_Mode;

/* Raw MAC requests */
/** @brief Defines the structure holding Radio capabilities
 */
typedef struct {
  /*! CCA threshold range (dbm) */
  int8_t min_cca_threshold;
  int8_t max_cca_threshold;
  /*! Transmission power range (dbm) */
  int8_t min_tx_power;
  int8_t max_tx_power;
} ST_MAC_raw_caps_t;

/** @brief Defines the structure for sending one or more frames regardless of the
 format. Only the PHY preamble at the start and the FCS field at the end are added.
 */
typedef struct {
  /*! The channel on which to send to the frame */
  uint8_t channel_number;
  /*! Frame power (dbm) */
  int8_t power;
  /*! Type of TX */
  MAC_RAW_TX_Type_t tx_type;
  /*! Payload to send (case ST_MAC_STANDARD_TX only) */
  uint8_t payload[ST_MAC_RAW_RX_PAYLOAD_MAX_SIZE];
  /*! Payload length (CRC included) */
  uint8_t payload_len;
  /*! Number of frames to send */
  uint16_t frames_number;  //0: Infinite NOT AVAILABLE IN CONCURRENT
  /*! Delay between each frames (in ms) */
  uint16_t delay_ms; // NOT AVAILABLE IN CONCURRENT
  /*! Stop next TX if current one failed (0: continue, otherwise stop) */
  uint8_t stopTx_if_failure; // NOT AVAILABLE IN CONCURRENT
} ST_MAC_raw_TX_start_t;

/** @brief Defines the structure for starting the radio in reception.
 */
typedef struct {
  /*! The channel on which to receive frames */
  uint8_t channel_number;
  /*! Reception duration in ms */
  uint32_t period; // 0: infinite NOT AVAILABLE IN CONCURRENT
  /*! Max number of frames to receive; once reached stop the reception */
  uint16_t frames_number; // 0: infinite NOT AVAILABLE IN CONCURRENT
} ST_MAC_raw_RX_start_t;

/** @brief Defines the structure to request a Clear Channel Assessement
 */
typedef struct {
  /*! The channel on which to perform the CCA */
  uint8_t channel_number;
  /*! CCA threshold in dbm */
  int8_t cca_threshold;
  /*! CCA result: 1: clear, 0: busy */
  uint8_t cca_result;
} ST_MAC_raw_CCA_t;

/** @brief Defines the structure to request an ED scan
 */
typedef struct {
  /*! Channel */
  uint8_t channel;
} ST_MAC_raw_EDscan_t;

/** @brief Defines the structure for an ED scan result
 */
typedef struct {
  /*! ED result */
  uint8_t ed;
} ST_MAC_rw_EDscan_result_t;

/** @brief Defines the structure for a single RX event
 */
typedef struct {
  /*! Status of Reception */
  MAC_RAW_RX_Status_t rx_status;
  /*! Pointer to payload of frame received */
  uint8_t *payload_ptr;
  /*! Size of frame received */
  uint8_t payload_len;
  /*! RSSI of frame received */
  int8_t rssi;
   /*! LQI of frame received */
  uint8_t lqi; 
} ST_MAC_raw_single_RX_event_t;

/** @brief Defines the structure for a signel TX event
 */
typedef struct {
  /*! Status of transmission */
  MAC_RAW_TX_Status_t tx_status;
  /*! Pointer to ACK if any (NULL otherwise) */
  uint8_t *ack_ptr;
  /*! Size of ACK (0 if any) */
  uint16_t ack_length;
} ST_MAC_raw_single_TX_event_t;

/** @brief Defines callback init structure for optional events
in MAC Initialisation */
/* Callback when radio enter NOTIF state */
typedef void (*ST_MAC_raw_Notif_callback)(MAC_RAW_State_t state);
typedef void (*ST_MAC_raw_single_RX_cbk) ( const ST_MAC_raw_single_RX_event_t * p_RX_evt);
typedef void (*ST_MAC_raw_single_TX_cbk) ( const ST_MAC_raw_single_TX_event_t * p_TX_evt);

typedef struct {
  ST_MAC_raw_Notif_callback  p_Notif;
  ST_MAC_raw_single_RX_cbk   p_RX_Done;
  ST_MAC_raw_single_TX_cbk   p_TX_Done;
} ST_MAC_Raw_event_callbacks_t;

/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
 * @brief ST_MAC_raw_init allow you to initilize the raw layer. there are 3 modes available. 
 * RAW_CONFIG allows you to send and received frame in 802.15.4 only. 
 * EXT_CONFIG not yet implemented. if used always return MAC_INVALID_PARAMETER.
 * CR_CONFIG allows you to send and received frame in 802.15.4 and have BLE in concurrente.
 *
 * @param *p_callback : [in] callback for TX done, RX received, state notify.
 *        *config : [in] config selected.
 * @retval MAC_Status_t 
 */
MAC_Status_t ST_MAC_raw_init(ST_MAC_Raw_event_callbacks_t *p_callback, ST_MAC_Config_Mode config);

/**
 * @brief ST_MAC_raw_get_caps allows you to get infomation: min_cca_threshold, max_cca_threshold, min_tx_power and max_tx_power.
 *
 * @param *MAC_handle : [in] MAC instance
 *        *ST_MAC_raw_caps_t : [in] raw_caps structure allow you to get CCA and TxPower information.
 * @retval MAC_Status_t 
 */
MAC_Status_t ST_MAC_raw_get_caps(MAC_handle * st_mac_hndl , ST_MAC_raw_caps_t *pRawCaps );

/**
 * @brief ST_MAC_raw_start_TX allows you to send frame in 802.15.4.
 * Depending on initialization mode, can also generate noise on a selected channel.
 *
 * @param *MAC_handle : [in] MAC instance
 *        *pRawTXStartReq : [in] TxFrame to send. Check the structure to have more infomation.
 * @retval MAC_Status_t 
 */
MAC_Status_t ST_MAC_raw_start_TX( MAC_handle * st_mac_hndl,  const ST_MAC_raw_TX_start_t * pRawTXStartReq );

/**
 * @brief ST_MAC_raw_stop_TX allow you to abort frame transmition in 802.15.4.
 *
 * @param None
 * @retval MAC_Status_t 
 */
MAC_Status_t ST_MAC_raw_stop_TX(void);

/**
 * @brief ST_MAC_raw_start_RX allows you to received frame in 802.15.4.
 *
 * @param *MAC_handle : [in] MAC instance
 *        *pRawTXStartReq : [in] Enable the radio to receive frames. Check the structure to have more infomation.
 * @retval MAC_Status_t 
 */
MAC_Status_t ST_MAC_raw_start_RX(MAC_handle * st_mac_hndl, const ST_MAC_raw_RX_start_t * pRawRXStartReq );

/**
 * @brief ST_MAC_raw_stop_RX allow you to stop receiving frame in 802.15.4.
 *
 * @param None
 * @retval MAC_Status_t 
 */
MAC_Status_t ST_MAC_raw_stop_RX(void);

/**
 * @brief ST_MAC_raw_start_CCA allow you to perform a CCA.
 * This feature lets you know whether the channel is noisy or not.
 *
 * @param *MAC_handle : [in] MAC instance
 *        *pRawTXStartReq : [in] Channel and threshold. Check the structure to have more infomation.
 * @retval MAC_Status_t 
 */
MAC_Status_t ST_MAC_raw_start_CCA(MAC_handle * st_mac_hndl, ST_MAC_raw_CCA_t * pRawCCAStartReq );

/* NOT AVAILABLE IN CONCURRENT */
/**
 * @brief ST_MAC_raw_EDscan allow you to perform Energy detector scan on one selected channel.
 *
 * @param *MAC_handle : [in] MAC instance
 *        *pRawEDscanReq : [in] Channel selected.
 * @retval MAC_Status_t 
 */
MAC_Status_t ST_MAC_raw_EDscan(MAC_handle * st_mac_hndl, const ST_MAC_raw_EDscan_t * pRawEDscanReq );

/**
 * @brief ST_MAC_raw_EDscan allow you to get the result for the latest ED scan performed.
 *
 * @param *MAC_handle : [in] MAC instance
 *        *pRawEDscanReq : [in] Energy on the selected channel (busy=255, clear=0).
 * @retval MAC_Status_t 
 */
MAC_Status_t ST_MAC_raw_EDscan_get_result(MAC_handle * st_mac_hndl, const ST_MAC_rw_EDscan_result_t * pRawEDscanReq );

#endif /* _802_15_4_MAC_RAW_SVC_H_ */
