/**
  ******************************************************************************
  * @file    st_mac_802_15_4_sys_svc.c
  * @author  MCD-WBL Application Team
  * @brief   Mac Extended Feature Implementation
  *          This file provides latest MAC Implementation.
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


/* Includes ------------------------------------------------------------------*/
#include "st_mac_802_15_4_concurrent_svc.h"
#include "mac_temporary.h"
#include "llhwc_cmn.h"
#include "platform.h"
#include "coexistence.h"
#include "platform.h"
#include "instance.h"
#include "mac_pkt_mem_if.h"

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#ifndef LL_UNUSED
#define LL_UNUSED(x)(void)(x)
#endif /* LL_UNUSED */

typedef struct _ST_MAC_concurrent_ctx {
  int8_t TxPower;
  uint8_t ChannelNumber;
  uint8_t ACKEnable;
  int8_t  CCAThreshold;
  uint8_t EnableCCA;
  uint8_t EnableCSMA;
  uint8_t FrameRetry;
  otRadioState RadioState;
#if 0
  uint8_t CustomACK;
  uint8_t ConfigCSMA;
  uint8_t Rate;
#endif
  
}ST_MAC_concurrent_ctx;

/* Private variables ---------------------------------------------------------*/
static otRadioFrame *radio_802_15_4_pktPtr = NULL;
static ST_MAC_concurrent_event_callbacks_t st_mac_cbks;
static ST_MAC_concurrent_ctx st_concurrent_ctx;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Radio callbacks */
static void mac_concurrent_svc_tx_done(otInstance *aInstance, otRadioFrame *aFrame, otRadioFrame *aAckFrame, otError aError);
static void mac_concurrent_svc_rx_done(otInstance *aInstance, otRadioFrame *aFrame, otError aError);
static void mac_concurrent_svc_ed_done(otInstance *aInstance, int8_t aEnergyScanMaxRssi);
static void mac_concurrent_svc_tx_started_done(otInstance *aInstance, otRadioFrame *aFrame);

/**
 * @brief Callback dispatch table for MAC operations
 *
 * This static structure contains the callback functions for various MAC operations, including ED scan, TX done, RX done, and TX started, in the context of concurrent operations for 802.15.4 communication.
 */
static struct mac_cbk_dispatch_tbl sy_cbk_dispatch_tbl = {
		.mac_ed_scan_done = mac_concurrent_svc_ed_done,
		.mac_tx_done = mac_concurrent_svc_tx_done,
		.mac_rx_done = mac_concurrent_svc_rx_done,
		.mac_tx_strtd= mac_concurrent_svc_tx_started_done
		};

/**
 * @brief Callback function for concurrent single TX operation completion
 *
 * This static function is a callback for handling the completion of concurrent single TX operation for 802.15.4 communication.
 *
 * @param aInstance : Pointer to the OpenThread instance
 * @param aFrame : Pointer to the transmitted radio frame
 * @param aAckFrame : Pointer to the acknowledgment frame received (if any)
 * @param aError : Error status of the TX operation
 */
static void mac_concurrent_svc_tx_done(otInstance *aInstance, otRadioFrame *aFrame, otRadioFrame *aAckFrame, otError aError)
{
  ST_MAC_concurrent_single_TX_event_t tx_evt;
  
  /* Free the radio transmit frame buffer */
  free_radio_frame(radio_802_15_4_pktPtr);

  /* Select error type */
  switch (aError) {
    case OT_ERROR_NONE:
      tx_evt.tx_status = TX_SUCCESS;
      break;
    case OT_ERROR_CHANNEL_ACCESS_FAILURE:
      tx_evt.tx_status = TX_CHANNEL_ACCESS_FAILURE;
      break;
    case OT_ERROR_NO_ACK:
      tx_evt.tx_status = TX_ACK_TIMEOUT;
      break;
    default:
      tx_evt.tx_status = TX_FAILED;
      break;
  }
  /* Retrun call back to the App */
  st_mac_cbks.p_TX_Done(&tx_evt);
  
  return;
}

/**
 * @brief Callback function for concurrent single RX operation completion
 *
 * This static function is a callback for handling the completion of concurrent single RX operation for 802.15.4 communication.
 *
 * @param aInstance : Pointer to the OpenThread instance
 * @param aFrame : Pointer to the received radio frame
 * @param aError : Error status of the RX operation
 */
static void mac_concurrent_svc_rx_done(otInstance *aInstance, otRadioFrame *aFrame, otError aError)
{
  LL_UNUSED(aInstance);
  ST_MAC_concurrent_single_RX_event_t rx_evt;
  
  // in ral_isr function need to have this patch after case RAL_RX_PKT to have the correct frame length:
  // if(0 == (BF_GET(g_mac_event_info->error_flags,RX_ERR_SHIFT,1)) || (RAL_RX_CRC_ERROR_MASK == (g_mac_event_info->error_flags & RAL_RX_CRC_ERROR_MASK)))
  if (NULL == aFrame) {
   return; 
  }
  
  /* Forward last frame payload & length */
  if (aFrame->mLength > 128) {
    return; 
  }
  switch (aError) {
    case OT_ERROR_NONE:
      rx_evt.payload_len = aFrame->mLength-2; // Remove FCS/CRC
      break;
    case OT_ERROR_FCS:
      rx_evt.payload_len = aFrame->mLength; // No FCS/CRC in this case
      break;
    default:
      return;
      break;
  }
  /* Copy payload */
  rx_evt.payload_ptr = aFrame->mPsdu;
  
  /* Retrun call back to the App */
  st_mac_cbks.p_RX_Done(&rx_evt);
  
  return;
}

/* Not used */
static void mac_concurrent_svc_ed_done(otInstance *aInstance, int8_t aEnergyScanMaxRssi)
{
  (void) (aInstance); 
  (void) (aEnergyScanMaxRssi); 
}

/* Not used */
static void mac_concurrent_svc_tx_started_done(otInstance *aInstance, otRadioFrame *aFrame)
{
  (void) (aInstance); 
  (void) (aFrame); 
}

/* External variables --------------------------------------------------------*/

/* External functions --------------------------------------------------------*/
/**
 * @brief Initialize concurrent service.
 *
 * This function initializes concurrent operation for 802.15.4 communication, allowing the user to set event callbacks for concurrent operation.
 *
 * @param p_callback : Pointer to the structure containing the event callbacks for concurrent operation
 * 
 * @return MAC_Status_t : Status of the MAC operation
 */
MAC_Status_t ST_MAC_concurrent_init(ST_MAC_concurrent_event_callbacks_t *p_callback) 
{
  /* Sanity check */
  if (NULL == p_callback) {
    return MAC_INVALID_PARAMETER;
  }
  MEMCPY(&st_mac_cbks, p_callback, sizeof(ST_MAC_concurrent_event_callbacks_t));

  radio_init();
  radio_call_back_funcs_init(&sy_cbk_dispatch_tbl);

  /* Need to create context and init here */
  memset(&st_concurrent_ctx, 0x00, sizeof(st_concurrent_ctx));
  
  st_concurrent_ctx.CCAThreshold = -75;
  /* Set default CCA threshold */
  ral_set_cca_ed_threshold(st_concurrent_ctx.CCAThreshold);

  /* Disable CCA */
  radio_set_cca_en(st_concurrent_ctx.EnableCCA);
  
  /* Disable CSMA */
  radio_set_csma_en(st_concurrent_ctx.EnableCSMA);
  
  /* Disable frame retry */
  set_max_frm_retries(st_concurrent_ctx.FrameRetry);
  
  /* Setup context */
  if (MEMCMP(&st_mac_cbks, p_callback, sizeof(ST_MAC_concurrent_event_callbacks_t)) != 0) {
    return MAC_ERROR;
  }  

  return MAC_SUCCESS;
}

/**
 * @brief Transmit a frame using concurrent transmission.
 *
 * This function initiates the transmission of a frame using concurrent transmission in 802.15.4.
 *
 * @param pRawTXStartReq : Pointer to the structure containing the concurrent transmission start request
 * 
 * @return MAC_Status_t : Status of the MAC operation
 */
MAC_Status_t ST_MAC_concurrent_TX(ST_MAC_concurrent_TX_start_t * pRawTXStartReq)
{
  MAC_Status_t result = MAC_SUCCESS;
  otRadioState radio_state = OT_RADIO_STATE_INVALID;
  
  /* Sanity check of input parameters */
  if (pRawTXStartReq == NULL) {
    return MAC_INVALID_PARAMETER;
  }

  //LOG_INFO_APP("Concurrent svc - start TX, get state: 0x%02x\r\n", coex_get_curr_proc_evnt());

  /* Check radio state */
  radio_state = otPlatRadioGetState(NULL);
  if (radio_state == OT_RADIO_STATE_RECEIVE || radio_state == OT_RADIO_STATE_SLEEP) {
    switch (pRawTXStartReq->TxType) {
      case ST_MAC_STANDARD_TX:
        
        /* Assign buffer */
        radio_802_15_4_pktPtr = otPlatRadioGetTransmitBuffer(NULL);
        /* Prepare TX packet with input parameters */
        if (radio_802_15_4_pktPtr == NULL) {
          return MAC_HW_BUSY;
        }
        radio_802_15_4_pktPtr->mChannel = st_concurrent_ctx.ChannelNumber;
        radio_802_15_4_pktPtr->mLength = pRawTXStartReq->payloadLen + 2;
        MAC_MEMCPY(&radio_802_15_4_pktPtr->mPsdu[0], &pRawTXStartReq->payload[0], pRawTXStartReq->payloadLen);
        radio_802_15_4_pktPtr->mInfo.mTxInfo.mMaxCsmaBackoffs = 0x00;
        radio_802_15_4_pktPtr->mInfo.mTxInfo.mMaxFrameRetries = st_concurrent_ctx.FrameRetry;
        radio_802_15_4_pktPtr->mInfo.mTxInfo.mCsmaCaEnabled = st_concurrent_ctx.EnableCSMA;
        radio_802_15_4_pktPtr->mInfo.mTxInfo.mIsSecurityProcessed = TRUE;

       /* Start first TX - any following TX will be handled by TX done callback */
        otPlatRadioTransmit(NULL, radio_802_15_4_pktPtr);
        
        break;
#if 0
      case ST_MAC_CONTINUOUS_TX:
        ST_MAC_concurrent_set_State(mac_hdl, ST_MAC_TX);
        cont_modulation_rslt = platform_zigbee_set_phy_cont_modul_mode(0, 1,
                                                                       (pRawTXStartReq->channel_number) - 11,
                                                                        pRawTXStartReq->power);
        result = (cont_modulation_rslt == OT_ERROR_NONE) ? MAC_SUCCESS : MAC_ERROR;
        break;
      case ST_MAC_CONTINUOUS_WAVE:
        ST_MAC_concurrent_set_State(mac_hdl, ST_MAC_TX);
        cont_modulation_rslt = platform_zigbee_set_phy_cont_modul_mode(1, 1,
                                                                       pRawTXStartReq->channel_number - 11,
                                                                       pRawTXStartReq->power);
        result = (cont_modulation_rslt == OT_ERROR_NONE) ? MAC_SUCCESS : MAC_ERROR;
        break;
#endif
      default:
        result = MAC_INVALID_PARAMETER;
        break;
    }
  }
  return result;
}
/**
 * @brief ST_MAC_concurrent_RX manages the frame reception in 802.15.4.
 *
 * @param rx_state : State of concurrent RX (Enable/Disable)
 *
 * @return MAC_Status_t : Status of the operation
 */
MAC_Status_t ST_MAC_concurrent_RX(ST_MAC_CONCURRENT_RX_State_t rx_state) {
  MAC_Status_t error = OT_ERROR_GENERIC;
  otRadioState radio_state = OT_RADIO_STATE_INVALID;
    
  if (rx_state) { // Enable Rx 
    /* Check radio state */
    radio_state = otPlatRadioGetState(NULL);
    if (radio_state == OT_RADIO_STATE_RECEIVE || radio_state == OT_RADIO_STATE_SLEEP) {
      /* Start continuous reception on MAC 802.15.4 */
      error = (MAC_Status_t) otPlatRadioReceive(NULL, st_concurrent_ctx.ChannelNumber);
      if (error != OT_ERROR_NONE) {
        return MAC_ERROR;
      }
    }
  } else { // Disable Rx 
    /* Check radio state */
    radio_state = otPlatRadioGetState(NULL);
    if (radio_state == OT_RADIO_STATE_RECEIVE || radio_state == OT_RADIO_STATE_TRANSMIT) {
      error = (MAC_Status_t) otPlatRadioSleep(NULL);
    }
  }
  return error;
}


/**
 * @brief Set the parameters for concurrent operation.
 * This function allows the user to set the parameters for concurrent operation.
 *
 * @param parameter : Pointer to the structure containing the concurrent operation parameters
 * 
 * @return MAC_Status_t : Status of the operation
 */
MAC_Status_t ST_MAC_concurrent_set_parameter(ST_MAC_concurrent_parameter_t* parameter)
{
  MAC_Status_t mac_status = MAC_SUCCESS;
  
  switch (parameter->attribute) {
    case CONCURRENT_TX_POWER:
      if ((((int8_t) *parameter->value) >= MIN_TX_POWER) && (((int8_t) *parameter->value) <= MAX_TX_POWER)) { // Filter with min and max Tx power
        st_concurrent_ctx.TxPower = (int8_t) *parameter->value; // change the value
        if (OT_ERROR_NONE != otPlatRadioSetTransmitPower(NULL, st_concurrent_ctx.TxPower)) {
          mac_status = MAC_HW_BUSY;
        }
      } else {
        mac_status = MAC_INVALID_PARAMETER;
      }
      break;
    case CONCURRENT_CHANNEL_NUMBER:
      if ((*parameter->value >= 11U) && (*parameter->value <= 26U)) { // Filter with the channel
        st_concurrent_ctx.ChannelNumber = *parameter->value; // change the value
      } else {
        mac_status = MAC_INVALID_PARAMETER;
      }
      break;
    case CONCURRENT_ACK_ENABLE:
        mac_status = MAC_NOT_IMPLEMENTED_STATUS; // NYI
      break;
    case CONCURRENT_CCA_THRESHOLD:
      if ((((int8_t) *parameter->value) <= MIN_CCA_THRESHOLD) && (((int8_t) *parameter->value) >= MAX_CCA_THRESHOLD)) { // Filter with the CCA THRESHOLD
        st_concurrent_ctx.CCAThreshold = *parameter->value; // change the value
        ral_set_cca_ed_threshold(st_concurrent_ctx.CCAThreshold);
      } else {
        mac_status = MAC_INVALID_PARAMETER;
      }
      break;
    case CONCURRENT_ENABLE_CCA:
      if ((0U == *parameter->value) || (1U == *parameter->value)) { // Filter keep only 0 or 1
        st_concurrent_ctx.EnableCCA = *parameter->value; // change the value
        radio_set_cca_en(st_concurrent_ctx.EnableCCA);
      } else {
        mac_status = MAC_INVALID_PARAMETER;
      }
      break;
    case CONCURRENT_ENABLE_CSMA:
      if ((0U == *parameter->value) || (1U == *parameter->value)) { // Filter keep only 0 or 1
        st_concurrent_ctx.EnableCSMA = *parameter->value; // change the value
        radio_set_csma_en(st_concurrent_ctx.EnableCSMA);
      } else {
        mac_status = MAC_INVALID_PARAMETER;
      }
      break;
    case CONCURRENT_FRAME_RETRY:
      mac_status = MAC_NOT_IMPLEMENTED_STATUS; // NYI
      //set_max_frm_retries(st_concurrent_ctx.FrameRetry);
      break;
    case CONCURRENT_RADIO_STATE:
        mac_status = MAC_ERROR; // Read only
      break;
    default:
      mac_status = MAC_NOT_IMPLEMENTED_STATUS;
      break;
    
  }

  return mac_status;
}

/**
 * @brief Get the parameters for concurrent operation.
 *
 * This function allows the user to get the parameters for concurrent operation.
 *
 * @param parameter : Pointer to the structure containing the concurrent operation parameters
 * 
 * @return MAC_Status_t : Status of the operation
 */
MAC_Status_t ST_MAC_concurrent_get_parameter(ST_MAC_concurrent_parameter_t* parameter)
{
    MAC_Status_t mac_status = MAC_SUCCESS;
  
  switch (parameter->attribute) {
    case CONCURRENT_TX_POWER:
      *parameter->value = (int8_t) st_concurrent_ctx.TxPower; // Get the value
      parameter->length = 1U;
      break;
    case CONCURRENT_CHANNEL_NUMBER:
      *parameter->value = st_concurrent_ctx.ChannelNumber; // Get the value
      parameter->length = 1U;
      break;
    case CONCURRENT_ACK_ENABLE:
      mac_status = MAC_NOT_IMPLEMENTED_STATUS; // NYI
      break;
    case CONCURRENT_CCA_THRESHOLD:
      *parameter->value = (int8_t) st_concurrent_ctx.CCAThreshold; // Get the value
      parameter->length = 1U;
      break;
    case CONCURRENT_ENABLE_CCA:
      *parameter->value = st_concurrent_ctx.EnableCCA; // Get the value
      parameter->length = 1U;
      break;
    case CONCURRENT_ENABLE_CSMA:
      *parameter->value = st_concurrent_ctx.EnableCSMA; // Get the value
      parameter->length = 1U;
      break;
    case CONCURRENT_FRAME_RETRY:
      *parameter->value = st_concurrent_ctx.FrameRetry; // Get the value
      break;
    case CONCURRENT_RADIO_STATE:
      st_concurrent_ctx.RadioState = otPlatRadioGetState(NULL); // Get the value
      *parameter->value = st_concurrent_ctx.RadioState;
      parameter->length = 1U;
      break;
    default:
     mac_status = MAC_NOT_IMPLEMENTED_STATUS;
      break;
    
  }

  return mac_status;
}

