/**
  ******************************************************************************
  * @file    st_mac_802_15_4_raw_svc.h
  * @author
  * @brief   Header for mac raw service
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

/** @brief Max size of payload reception (CRC not included) */
#define ST_MAC_CONCURRENT_RX_PAYLOAD_MAX_SIZE 125

#define MIN_TX_POWER -20
#define MAX_TX_POWER 10
#define MAX_CCA_THRESHOLD -75
#define MIN_CCA_THRESHOLD -35


/******************************************************************************/
/* Exported types ------------------------------------------------------------*/

/** @brief Defines the type of transmission
 */
typedef enum {
  ST_MAC_NONE_TX = 0,
  /* Standard TX with paylaod */
  ST_MAC_STANDARD_TX, 
  /* Continuous PRBS noise */
  ST_MAC_CONTINUOUS_TX, // NYI
  /* Unmodulated continuous TX */
  ST_MAC_CONTINUOUS_WAVE,// NYI
  ST_MAC_UNKNOWN_TX, //keep last
} ST_MAC_CONCURRENT_TX_Type_t;

/** @brief TX status
 */
typedef enum {
  TX_SUCCESS = 0,
  TX_ACK_TIMEOUT,
  TX_CHANNEL_ACCESS_FAILURE,
  TX_FAILED,
} ST_MAC_CONCURRENT_TX_Status_t;

/** @brief RX status
 */
typedef enum {
  RX_SUCCESS = 0,
  RX_NO_PAYLOAD,
  RX_REJECTED,
  RX_FILTERED,
} ST_MAC_CONCURRENT_RX_Status_t;

/** @brief RX state
 */
typedef enum {
  /* Allow you to not received frame in 802.15.4 */
  RX_DISABLE = 0,
  /* Allow you to received frame in 802.15.4 */
  RX_ENABLE,
} ST_MAC_CONCURRENT_RX_State_t;

/** @brief Context attribute parameters
 */
typedef enum {
  CONCURRENT_TX_POWER = 0, // Read and Write
  CONCURRENT_CHANNEL_NUMBER, // Read and Write
  CONCURRENT_ACK_ENABLE, // NYI
  CONCURRENT_CCA_THRESHOLD, // Read and Write
  CONCURRENT_ENABLE_CCA, // Read and Write
  CONCURRENT_ENABLE_CSMA, // Read and Write
  CONCURRENT_FRAME_RETRY, // NYI
  CONCURRENT_RADIO_STATE, // Read only
  CONCURRENT_CUSTOM_ACK, // NYI
  CONCURRENT_CONFIG_CSMA, // NYI
  CONCURRENT_RATE, // NYI
} MAC_CONCURRENT_attribute_t;

/** @brief Defines the structure for sending one or more frames regardless of the
 format. Only the PHY preamble at the start and the FCS field at the end are added.
 */
typedef struct {
  /*! Type of TX */
  ST_MAC_CONCURRENT_TX_Type_t TxType;
  /*! Payload to send (case ST_MAC_STANDARD_TX only) */
  uint8_t payload[ST_MAC_CONCURRENT_RX_PAYLOAD_MAX_SIZE];
  /*! Payload length (CRC excluded) */
  uint8_t payloadLen;
} ST_MAC_concurrent_TX_start_t;


/** @brief Defines the structure for a single RX event
 */
typedef struct {
  /*! Pointer to payload of frame received */
  uint8_t *payload_ptr;
  /*! Size of frame received */
  uint8_t payload_len;
} ST_MAC_concurrent_single_RX_event_t;

/** @brief Defines the structure for a signel TX event
 */
typedef struct {
  /*! Status of transmission */
  ST_MAC_CONCURRENT_TX_Status_t tx_status;
} ST_MAC_concurrent_single_TX_event_t;

typedef struct {
  /*! attribute to set/get parameter*/
  MAC_CONCURRENT_attribute_t attribute;
  /*! value to set/get parameter*/
  uint8_t* value;
  /*! length to set/get parameter*/
  uint8_t length;
} ST_MAC_concurrent_parameter_t;

/** @brief Defines callback init structure for optional events
in MAC Initialisation */
typedef void (*ST_MAC_concurrent_single_RX_cbk) ( const ST_MAC_concurrent_single_RX_event_t * p_RX_evt);
typedef void (*ST_MAC_concurrent_single_TX_cbk) ( const ST_MAC_concurrent_single_TX_event_t * p_TX_evt);

/** @brief Event callbacks for concurrent operation
 *
 * This structure contains the event callbacks for concurrent single RX and TX operations in the MAC layer for 802.15.4 communication.
 */
typedef struct
{
  /* Callback function when frame is received in 802.15.4 */
  ST_MAC_concurrent_single_RX_cbk   p_RX_Done;
  /* Callback function when frame is transmit in 802.15.4 */
  ST_MAC_concurrent_single_TX_cbk   p_TX_Done;
} ST_MAC_concurrent_event_callbacks_t;

/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */



/* Concurrence services */
/**
 * @brief Initialize concurrent service.
 *
 * This function initializes concurrent operation for 802.15.4 communication, allowing the user to set event callbacks for concurrent operation.
 *
 * @param p_callback : Pointer to the structure containing the event callbacks for concurrent operation
 * 
 * @return MAC_Status_t : Status of the MAC operation
 */
MAC_Status_t ST_MAC_concurrent_init(ST_MAC_concurrent_event_callbacks_t *p_callback);

/**
 * @brief Transmit a frame using concurrent transmission.
 *
 * This function initiates the transmission of a frame using concurrent transmission in 802.15.4.
 *
 * @param pRawTXStartReq : Pointer to the structure containing the concurrent transmission start request
 * 
 * @return MAC_Status_t : Status of the MAC operation
 */
MAC_Status_t ST_MAC_concurrent_TX(ST_MAC_concurrent_TX_start_t * pConcurrentTXStartReq );

/**
 * @brief ST_MAC_concurrent_RX manages the frame reception in 802.15.4.
 *
 * @param rx_state : State of concurrent RX (Enable/Disable)
 *
 * @return MAC_Status_t : Status of the operation
 */
MAC_Status_t ST_MAC_concurrent_RX(ST_MAC_CONCURRENT_RX_State_t rx_state);

/**
 * @brief Set the parameters for concurrent operation.
 * This function allows the user to set the parameters for concurrent operation.
 *
 * @param parameter : Pointer to the structure containing the concurrent operation parameters
 * 
 * @return MAC_Status_t : Status of the operation
 */
MAC_Status_t ST_MAC_concurrent_set_parameter(ST_MAC_concurrent_parameter_t* parameter);

/**
 * @brief Get the parameters for concurrent operation.
 *
 * This function allows the user to get the parameters for concurrent operation.
 *
 * @param parameter : Pointer to the structure containing the concurrent operation parameters
 * 
 * @return MAC_Status_t : Status of the operation
 */
MAC_Status_t ST_MAC_concurrent_get_parameter(ST_MAC_concurrent_parameter_t* parameter);

#endif /* _802_15_4_MAC_RAW_SVC_H_ */
