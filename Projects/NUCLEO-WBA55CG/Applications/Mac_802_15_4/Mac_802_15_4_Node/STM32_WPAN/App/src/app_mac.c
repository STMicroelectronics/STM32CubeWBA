/**
  ******************************************************************************
  * @file    app_mac.c
  * @author  MCD Application Team
  * @brief   Provide MAC Wrapper Solution
  *          Glue between upper stack solution and IP Solution
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

/* Includes ------------------------------------------------------------------*/
#include "stm32wbaxx_hal.h"
#include "common_types.h"
#include "app_mac.h"
#include "stm32_seq.h"
#include "stm32_lpm.h"
#include "app_conf.h"
#include "app_mac_802_15_4_process.h"
#include "stm32_timer.h"
#include "st_mac_802_15_4_types.h"
#include "st_mac_802_15_4_sap.h"
#include "radio.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define DATA "HELLO COORDINATOR\0"

/* Set the Maximum value of the counter (Auto-Reload) that defines the Period */
#define SEND_TIMEOUT  (5000) /**< 5s */

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static UTIL_TIMER_Object_t SendTimer;
static uint8_t rfBuffer[256];

/* Private functions ---------------------------------------------------------*/
static uint8_t xorSign( const char * pmessage, uint8_t message_len);
static void SendTimerCallback(void *arg);

/* Public variables ---------------------------------------------------------*/
MAC_handle mac_hndl; // Instance MAC 
ST_MAC_callbacks_t macCallback; // Structure with all callbacks
ST_MAC_associateCnf_t g_MAC_associateCnf;
ST_MAC_beaconNotifyInd_t  g_BeaconNotifyInd;
uint8_t deviceAssociated = 0; //This variable allows to know if the device is associated or not, associated = 1 otherwise 0
uint8_t enablePollTimer = 0; //This variable allows to enable the poll Timer which send data poll every 5s

/*  All variables for configuration and set data */
uint16_t        g_panId                = 0x0000;
uint16_t        g_coordShortAddr       = 0x0000;;
uint8_t         g_coordExtendedAddr[8] = {0xAC,0xDE,0x48,00,0x00,0x00,0x00,0x01};
uint16_t        g_BroacastShortAddr    = 0xFFFF;
uint8_t         g_dataHandle           = 0x02;
uint8_t         extAddr[8]             = {0xAC,0xDE,0x48,00,0x00,0x00,0x00,0x02};
uint8_t         g_channel              = DEMO_CHANNEL; // General channel
uint8_t         g_channel_page         = 0x00;


/**
  * @brief  compute  simple xor signature of the data to transmit
  *
  * @param  pmessage   : Message to sign.
  * @param  message_len: Message Len
  * @retval Message Signature
  */

static uint8_t xorSign( const char * pmessage, uint8_t message_len)
{
  uint8_t seed = 0x00;
  for (uint8_t i=0x00;i<message_len;i++)
    seed = (uint8_t)pmessage[i]^seed;
  return seed; 
}

static void SendTimerCallback(void *arg)
{
  // Run Data pool task
  UTIL_SEQ_SetTask(1 << CFG_TASK_DATA_POOL, TASK_MAC_APP_PRIO );
}

/* USER CODE BEGIN FD*/
void APPE_Button1Action(void)
{
  /* Program timer which send a data to the coordinator every 5s */
  APP_RFD_MAC_802_15_4_poll();
  if (!enablePollTimer) {
    enablePollTimer = 1;
    UTIL_TIMER_Start(&SendTimer);
  }else{
    enablePollTimer = 0;
    UTIL_TIMER_Stop(&SendTimer);
  }
}

void APPE_Button2Action(void)
{
  /* Send data in broadcast */
  APP_RFD_MAC_802_15_4_SendData_broacast("Data In Broadcast\0");
}

void APPE_Button3Action(void)
{
  /* Disassociation with the coordinator */
  if (deviceAssociated){
    APP_RFD_MAC_802_15_4_Disassociation();
  }else{
   APP_DBG("This device is not associated\r\n");
  }

}

void app_mac_regMacCallback( ST_MAC_callbacks_t * macCallback) {
  
  // ST_MAC_callbacks_t macCallback;
  macCallback->mlmeAssociateCnfCb = APP_MAC_mlmeAssociateCnfCb ;
  macCallback->mlmeAssociateIndCb = APP_MAC_mlmeAssociateIndCb;
  macCallback->mlmeBeaconNotifyIndCb = APP_MAC_mlmeBeaconNotifyIndCb;
  macCallback->mlmeCalibrateCnfCb = APP_MAC_mlmeCalibrateCnfCb;
  macCallback->mlmeCommStatusIndCb = APP_MAC_mlmeCommStatusIndCb;
  macCallback->mlmeDisassociateCnfCb = APP_MAC_mlmeDisassociateCnfCb;
  macCallback->mlmeDisassociateIndCb = APP_MAC_mlmeDisassociateIndCb;
  macCallback->mlmeDpsCnfCb = APP_MAC_mlmeDpsCnfCb;
  macCallback->mlmeDpsIndCb = APP_MAC_mlmeDpsIndCb;
  macCallback->mlmeGetCnfCb = APP_MAC_mlmeGetCnfCb;
  macCallback->mlmeGtsCnfCb = APP_MAC_mlmeGtsCnfCb;
  macCallback->mlmeGtsIndCb = APP_MAC_mlmeGtsIndCb;
  macCallback->mlmeOrphanIndCb = APP_MAC_mlmeOrphanIndCb;
  macCallback->mlmePollCnfCb = APP_MAC_mlmePollCnfCb;
  macCallback->mlmeResetCnfCb = APP_MAC_mlmeResetCnfCb;
  macCallback->mlmeRxEnableCnfCb = APP_MAC_mlmeRxEnableCnfCb;
  macCallback->mlmeScanCnfCb = APP_MAC_mlmeScanCnfCb;
  macCallback->mlmeSetCnfCb = APP_MAC_mlmeSetCnfCb;
  macCallback->mlmeSoundingCnfCb = APP_MAC_mlmeSoundingCnfCb;
  macCallback->mlmeStartCnfCb = APP_MAC_mlmeStartCnfCb;
  macCallback->mlmeSyncLossIndCb = APP_MAC_mlmeSyncLossIndCb;
  macCallback->mcpsDataIndCb = APP_MAC_mcpsDataIndCb;
  macCallback->mcpsDataCnfCb = APP_MAC_mcpsDataCnfCb;
  macCallback->mcpsPurgeCnfCb = APP_MAC_mcpsPurgeCnfCb;
  macCallback->mlmePollIndCb = APP_MAC_mlmePollIndCb;
  macCallback->mlmeBeaconReqIndCb = APP_MAC_mlmeBeaconReqIndCb;
  macCallback->mlmeBeaconCnfCb = APP_MAC_mlmeBeaconCnfCb;
  macCallback->mlmeGetPwrInfoTableCnfCb = APP_MAC_mlmeGetPwrInfoTableCnfCb;
  macCallback->mlmeSetPwrInfoTableCnfCb = APP_MAC_mlmeSetPwrInfoTableCnfCb;
}

void APP_MAC_Init(void) {
  /* Disable Low power */
  UTIL_LPM_SetStopMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);
  UTIL_LPM_SetOffMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);

  /* Register tasks */
  UTIL_SEQ_RegTask(TASK_RFD, UTIL_SEQ_RFU, APP_RFD_MAC_802_15_4_SetupTask); // Setup task

  UTIL_SEQ_RegTask( 1<<TASK_BUTTON_1, UTIL_SEQ_RFU, APPE_Button1Action); // Button 1
  UTIL_SEQ_RegTask( 1<<TASK_BUTTON_2, UTIL_SEQ_RFU, APPE_Button2Action); // Button 2
  UTIL_SEQ_RegTask( 1<<TASK_BUTTON_3, UTIL_SEQ_RFU, APPE_Button3Action); // Button 3
  UTIL_SEQ_RegTask( 1<<CFG_TASK_DATA_POOL, UTIL_SEQ_RFU, APP_RFD_MAC_802_15_4_poll); // Data pool task
  
  /* Configuration MAC 802_15_4 */
  app_mac_regMacCallback(&macCallback);
  ST_MAC_init(&macCallback);
  
  /* Create Timer to send data pool every 5s*/
  UTIL_TIMER_Status_t tmr_status = UTIL_TIMER_OK;
  tmr_status = UTIL_TIMER_Create(&SendTimer, SEND_TIMEOUT, UTIL_TIMER_PERIODIC,
                                 &SendTimerCallback, NULL);
  if (tmr_status != UTIL_TIMER_OK){
    APP_DBG("Node MAC APP - Create Timer Fails\r\n");
  }
  
  /* Execute once at startup */
  UTIL_SEQ_SetTask(TASK_RFD, TASK_MAC_APP_PRIO);
}

void APP_RFD_MAC_802_15_4_SetupTask(void)
{
  MAC_Status_t MacStatus = MAC_ERROR;
  ST_MAC_resetReq_t    ResetReq;
  ST_MAC_setReq_t      SetReq;
  ST_MAC_scanReq_t     ScanReq;
  uint8_t PIB_Value = 0x00;
  uint8_t scan_channel[4];
  int8_t tx_power_pib_value = 0;
  
  APP_DBG("Run Node MAC 802.15.4 - 2 - Node Startup\r\n");
  /* Reset MAC */
  memset(&ResetReq, 0x00, sizeof(ST_MAC_resetReq_t));
  ResetReq.set_default_PIB = TRUE;
    
  MacStatus = ST_MAC_MLMEResetReq(mac_hndl, &ResetReq);
  if (MAC_SUCCESS != MacStatus) {
    APP_DBG("Node MAC APP - Reset Fails\r\n");
    return;
  }
  /* Wait RESET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_RESET_CNF);     
    
  /* Get automatically the extended address */
  otPlatRadioGetIeeeEui64(NULL, extAddr);
  APP_DBG("COORD MAC APP - Extended address: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\r\n",extAddr[0],
            extAddr[1],extAddr[2],extAddr[3],extAddr[4],extAddr[5],extAddr[6],extAddr[7]);
  
  /* Set Device Extended Address */
  memset(&SetReq, 0x00, sizeof(ST_MAC_setReq_t));
  SetReq.PIB_attribute = g_MAC_EXTENDED_ADDRESS_c;
  SetReq.PIB_attribute_valuePtr = (uint8_t *) &extAddr;
  
  MacStatus = ST_MAC_MLMESetReq(mac_hndl, &SetReq);
  if (MAC_SUCCESS != MacStatus) {
    APP_DBG("Node MAC APP - Set Extended Addr Fails\r\n");
    return;
  }
  /* Wait SET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SET_CNF);     

  /* Set MAC_AUTO_REQUEST to 0 to able to receive beaconNotify.ind */
  memset(&SetReq, 0x00, sizeof(ST_MAC_setReq_t));
  SetReq.PIB_attribute = g_MAC_AUTO_REQUEST_c;
  PIB_Value = FALSE; 
  SetReq.PIB_attribute_valuePtr = (uint8_t *) &PIB_Value;
  MacStatus = ST_MAC_MLMESetReq(mac_hndl, &SetReq);
  if (MAC_SUCCESS != MacStatus) {
    APP_DBG("Node MAC - Set MAC_AUTO_REQUEST Fails\r\n");
    return;
  }
  /* Wait SET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SET_CNF);
  
  /* Set Tx Power */
  memset(&SetReq,0x00,sizeof(ST_MAC_setReq_t));
  SetReq.PIB_attribute = g_PHY_TRANSMIT_POWER_c;
  tx_power_pib_value = 10; // int8 dBm value in the range [-20;10]
  SetReq.PIB_attribute_valuePtr = (uint8_t *) &tx_power_pib_value;
  MacStatus = ST_MAC_MLMESetReq(mac_hndl, &SetReq );
  if (MAC_SUCCESS != MacStatus) {
    APP_DBG("Node MAC - Set Tx Power Fails\r\n");
    return;
  }
  /* Wait SET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SET_CNF);
  
  /* Scan Active */
  APP_DBG("Node MAC APP - Active Scan\r\n");
  /** Select all channels 
   * The channel numbers to be scanned. The 27 bits (b0, b1, …, b26) indicate 
   * which channels are to be scanned (1 = scan, 0 = do not scan) for each 
   * of the 27 channels.
  **/
  scan_channel[0] = 0x00;
  scan_channel[1] = 0xF8;
  scan_channel[2] = 0xFF;
  scan_channel[3] = 0x07;//all channels
  memset(&ScanReq, 0x00, sizeof(ST_MAC_scanReq_t));
  ScanReq.scan_type = g_MAC_ACTIVE_SCAN_TYPE_c; //Active scan
  ScanReq.scan_duration = g_PHY_MAX_FRAME_DURATION_c;
  ScanReq.channel_page = g_channel_page;
  ScanReq.security_level = g_channel_page;
  memcpy(&ScanReq.a_scan_channels,(uint8_t *) &scan_channel, 0x04);
    
  MacStatus = ST_MAC_MLMEScanReq(mac_hndl, &ScanReq);
  if (MAC_SUCCESS != MacStatus) {
    APP_DBG("Node MAC APP - Scan active Fails\r\n");
    return;
  }
  /* Wait SCAN CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SCAN_CNF);

  /* Association */
  APP_RFD_MAC_802_15_4_Association();
  /* Wait ASSOCIATION CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_ASSOCIATE_CNF);
  
  /* Set Device Short Address */
  memset(&SetReq,0x00,sizeof(ST_MAC_setReq_t));
  SetReq.PIB_attribute = g_MAC_SHORT_ADDRESS_c;
  SetReq.PIB_attribute_valuePtr = (uint8_t*) &g_MAC_associateCnf.a_assoc_short_address;
  MacStatus = ST_MAC_MLMESetReq(mac_hndl, &SetReq);
  if ( MAC_SUCCESS != MacStatus ) {
    APP_DBG("RFD MAC APP - Set Short Addr Fails\n\r");
    return;
  }
  /* Wait SET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SET_CNF);

  /* Set RxOnWhenIdle */
  memset(&SetReq,0x00,sizeof(ST_MAC_setReq_t));
  SetReq.PIB_attribute = g_MAC_RX_ON_WHEN_IDLE_c;
  PIB_Value = g_TRUE;
  SetReq.PIB_attribute_valuePtr = &PIB_Value;

  MacStatus = ST_MAC_MLMESetReq(mac_hndl, &SetReq);
  if (MAC_SUCCESS != MacStatus) {
    APP_DBG("Node MAC APP - Set Rx On When Idle Fails\r\n");
    return;
  }
  /* Wait SET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SET_CNF);

  APP_DBG("Node MAC APP - Ready \r\n"); //to Handle Association Req and Receive Data
  BSP_LED_On(LD1);
  APP_RFD_MAC_802_15_4_poll(); //send data to the coordinator
}

void APP_RFD_MAC_802_15_4_SendData_broacast(const char * data)
{
  MAC_Status_t MacStatus = MAC_ERROR;
  ST_MAC_dataReq_t DataReq;
  BSP_LED_On(LD3);
  
  APP_DBG("Node MAC APP - Send Data to 0x%04x\r\n",g_BroacastShortAddr);
  DataReq.src_addr_mode = g_SHORT_ADDR_MODE_c; //if the device is associated, we send the broadcast with the short address
  if (!deviceAssociated){
    DataReq.src_addr_mode = g_EXTENDED_ADDR_MODE_c; //if the device is not associated, we send the broadcast with the extended address
  }
  DataReq.dst_addr_mode = g_SHORT_ADDR_MODE_c;

  memcpy(DataReq.a_dst_PAN_id, &g_panId, 0x02);
  memcpy(DataReq.dst_address.a_short_addr, &g_BroacastShortAddr, 0x02);

  DataReq.msdu_handle = g_dataHandle++;
  DataReq.ack_Tx = 0x00; // No ACK request
  DataReq.GTS_Tx = FALSE;
  memcpy(&rfBuffer, data, strlen(data));
  rfBuffer[strlen(data)] = xorSign(data, strlen(data));
  DataReq.msduPtr = (uint8_t *) rfBuffer;
  DataReq.msdu_length = strlen(data) + 1;
  DataReq.security_level = g_MAC_UNSECURED_MODE_c;
  MacStatus = ST_MAC_MCPSDataReq(mac_hndl, &DataReq);
  if (MAC_SUCCESS != MacStatus) {
    APP_DBG("Node MAC APP - Data Req Fails\r\n");
    return;
  }
  BSP_LED_Off(LD3);
}

void APP_RFD_MAC_802_15_4_Disassociation(void)
{
  MAC_Status_t MacStatus = MAC_ERROR;
  ST_MAC_disassociateReq_t DisassiociateReq;
  BSP_LED_On(LD3);
  
  /* If poll timer is enable stop it*/
  if (enablePollTimer) {
    enablePollTimer = 0;
    UTIL_TIMER_Stop(&SendTimer);
  }
  
  DisassiociateReq.device_addr_mode = g_SHORT_ADDR_MODE_c; //short address
  memcpy(DisassiociateReq.a_device_PAN_id, &g_panId, 0x02); //PAN ID 0x1AAA
  DisassiociateReq.disassociate_reason = g_DEVICE_REQUESTED_c; //The device wishes to leave the PAN
  memcpy(DisassiociateReq.device_address.a_short_addr, &g_coordShortAddr,0x02);
  DisassiociateReq.tx_Indirect = FALSE;
  DisassiociateReq.security_level = g_MAC_UNSECURED_MODE_c;
  MacStatus = ST_MAC_MLMEDisassociateReq(mac_hndl, &DisassiociateReq);
  if (MAC_SUCCESS != MacStatus) {
    APP_DBG("Node MAC APP - Disassociate Req Fails\r\n");
    return;
  }

  BSP_LED_Off(LD3);
}

void APP_RFD_MAC_802_15_4_poll(void)
{
  MAC_Status_t MacStatus = MAC_ERROR;
  ST_MAC_pollReq_t PollReq;
  BSP_LED_On(LD3);
  
  PollReq.coord_addr_mode = g_SHORT_ADDR_MODE_c; //short address
  memcpy(PollReq.coord_address.a_short_addr,&g_coordShortAddr,0x02); //coordinator address
  memcpy(PollReq.a_coord_PAN_id, &g_panId, 0x02); //PAN ID
  PollReq.security_level = g_MAC_UNSECURED_MODE_c; //No secured
  
  MacStatus = ST_MAC_MLMEPollReq(mac_hndl, &PollReq);
  if (MAC_SUCCESS != MacStatus) {
    APP_DBG("Node MAC APP - Disassociate Req Fails\r\n");
    return;
  }

  BSP_LED_Off(LD3);
}

void APP_RFD_MAC_802_15_4_Association(void)
{
  MAC_Status_t MacStatus = MAC_ERROR;
  ST_MAC_associateReq_t AssociateReq;

  memset(&AssociateReq,0x00,sizeof(ST_MAC_associateReq_t));
  APP_DBG("Node MAC APP - Association REQ\r\n");
  g_channel = g_BeaconNotifyInd.PAN_descriptor.logical_channel;
  g_panId = ((g_BeaconNotifyInd.PAN_descriptor.a_coord_PAN_id[1]) << 8) + g_BeaconNotifyInd.PAN_descriptor.a_coord_PAN_id[0];
  if (g_BeaconNotifyInd.PAN_descriptor.coord_addr_mode == g_SHORT_ADDR_MODE_c) {
    g_coordShortAddr = ((g_BeaconNotifyInd.PAN_descriptor.coord_addr.a_short_addr[1]) << 8 ) + g_BeaconNotifyInd.PAN_descriptor.coord_addr.a_short_addr[0]; 
    /* prepare coordinator address for association */
    AssociateReq.coord_addr_mode  = g_SHORT_ADDR_MODE_c;
    memcpy(AssociateReq.coord_address.a_short_addr, &g_coordShortAddr, 0x02);
      
  }else{ if (g_BeaconNotifyInd.PAN_descriptor.coord_addr_mode == g_EXTENDED_ADDR_MODE_c) {
    memcpy(g_coordExtendedAddr, &(g_BeaconNotifyInd.PAN_descriptor.coord_addr.a_extend_addr[0]), 0x08);
    /* prepare coordinator address for association */
    AssociateReq.coord_addr_mode  = g_SHORT_ADDR_MODE_c;
    memcpy(AssociateReq.coord_address.a_extend_addr, &g_coordExtendedAddr, 0x08);
  }else {
    APP_DBG("Node MAC APP - Association REQ ERROR No extended or short address\r\n");
    return;
  }
  }
  /* prepare channel for association */
  AssociateReq.channel_number   = g_channel;
  AssociateReq.channel_page     = g_channel_page;//always this value
    
  /* allows you to allocate address and receive ON when IDLE*/
  AssociateReq.capability_information = 0x88;
    
  /* prepare PANID for association */
  memcpy(AssociateReq.a_coord_PAN_id, &g_panId, 0x02);
  AssociateReq.security_level = g_MAC_UNSECURED_MODE_c;
  MacStatus = ST_MAC_MLMEAssociateReq(mac_hndl, &AssociateReq);
  if (MAC_SUCCESS != MacStatus) {
    APP_DBG("Node MAC APP - Association Req Fails\n\r");
    return;
  }

}
