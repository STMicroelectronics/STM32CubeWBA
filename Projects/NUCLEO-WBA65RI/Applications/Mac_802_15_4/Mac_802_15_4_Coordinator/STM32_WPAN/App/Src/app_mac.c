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

#include <stdio.h>
#include "stm32wbaxx_hal.h"
#include "common_types.h"
#include "app_mac.h"
#include "stm32_seq.h"
#include "stm32_lpm.h"
#include "app_conf.h"
#include "scm.h"
#include "app_mac_802_15_4_process.h"
#include "st_mac_802_15_4_types.h"
#include "st_mac_802_15_4_sap.h"
#include "radio.h"
#include "stm32_timer.h"
#include "log_module.h"

/* Global define -------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/
extern RNG_HandleTypeDef hrng;
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define SEND_TIMEOUT  (500) /**< 500ms */

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint8_t g_dataHandle = 0x02;
static uint8_t rfBuffer[256];
static uint8_t g_channel_page = 0x00;

/* Private functions ---------------------------------------------------------*/
static uint8_t xorSign( const char * pmessage, uint8_t message_len);
static void Prepare_MLME_Set_Req(uint8_t PIB_attribute_indice, uint8_t * PIB_attribute_value);
static void Prepare_MLME_Scan_Req(uint8_t scan_type, uint8_t scan_duration, uint8_t * select_channel);

void app_mac_regMacCallback( ST_MAC_callbacks_t * macCallback);
/* Public variables ---------------------------------------------------------*/
MAC_handle mac_hndl; // Instance MAC 
ST_MAC_callbacks_t macCallback; // Structure with all callbacks
UTIL_TIMER_Object_t LEDTimer; // LED timer

uint16_t shortAssociationAddrList[MAX_DEVICES_ASSO]; // Tab with all address associated devices
uint8_t idAssociationList = 0; // Number of elements in the tab
uint8_t indDeviceSendData = 0; // Indice to check if the device still connected
uint8_t status_check_device = 0; // Return device status when BT2 is pushed
uint16_t tab_panId_already_use[MAX_TAB_PAN_ID]; //tab with all PAN ID already use in the local environment

/* All variables for configuration and set data, define in app_conf.h */
uint8_t extAddr[8]                           = {0xAC,0xDE,0x48,00,0x00,0x00,0x00,0x01};
uint16_t shortAddr                           = COORD_ADDRESS; // Can be modifiate in app_conf.h
uint16_t panId                               = 0x0000;
uint8_t g_channel                            = 11;
uint8_t Beacon_payload[BEACON_PAYLOAD_SIZE]  = {BEACON_PAYLOAD}; // Can be modifiate in app_conf.h

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

/**
  * @brief  This function allows you to off LED 2 and LED 3 with a timer
  *
  * @param  arg  
  * @retval nothing
  */
static void LEDTimerCallback (void * arg)
{
  BSP_LED_Off(LD2);
  BSP_LED_Off(LD3);
}

/**
  * @brief  This function allows you to set a PIB attribute to put in parameters 
  * only the PIB attribute and this value.
  *
  * @param  PIB_attribute_indice   : PIB attribute you want to change
  * @param  PIB_attribute_value: PIB attribute value you want to change
  * @retval nothing
  */
static void Prepare_MLME_Set_Req(uint8_t PIB_attribute_indice, uint8_t * PIB_attribute_value)
{
  MAC_Status_t MacStatus;
  ST_MAC_setReq_t   SetReq;
  
  memset(&SetReq, 0x00, sizeof(ST_MAC_setReq_t));
  SetReq.PIB_attribute = PIB_attribute_indice;
  SetReq.PIB_attribute_valuePtr = PIB_attribute_value;
  MacStatus = ST_MAC_MLMESetReq(mac_hndl, &SetReq);
  if (MAC_SUCCESS != MacStatus) {
    APP_DBG("COORD MAC APP - Set PIB attribute Fails\r\n");
    return;
  }
  
  /* Wait SET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SET_CNF); 
}

/**
  * @brief  This function allows you to use mlme-scan.request primitive.
  *
  * @param  PIB_attribute_indice   : PIB attribute you want to change
  * @param  PIB_attribute_value: PIB attribute value you want to change
  * @retval nothing
  */
static void Prepare_MLME_Scan_Req(uint8_t scan_type, uint8_t scan_duration, uint8_t * select_channel)
{
  MAC_Status_t MacStatus;
  ST_MAC_scanReq_t  ScanReq;
  
  memset(&ScanReq, 0x00, sizeof(ST_MAC_scanReq_t));
  ScanReq.scan_type = scan_type;
  ScanReq.scan_duration = scan_duration; // Max frame duration in us (16 * 960 * (2^n + 1)), with n = BASE_SCAN_DURATION
  ScanReq.channel_page = g_channel_page; // Always this value in 802.15.4 non-beacon enabled
  ScanReq.security_level = g_MAC_UNSECURED_MODE_c; // Unsecure frame
  memcpy(&ScanReq.a_scan_channels, select_channel, 0x04);
   
  MacStatus = ST_MAC_MLMEScanReq(mac_hndl, &ScanReq);
  if (MAC_SUCCESS != MacStatus) {
    APP_DBG("COORD MAC APP - mlme-Scan.req Fails\r\n");
    return;
  }
  
  /* Wait SCAN CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SCAN_CNF);  
}

/* USER CODE BEGIN FD*/
void APPE_Button1Action(void)
{
  /* Display all associated devices */
  if (idAssociationList > 0) {
    for (uint8_t i=0;i<idAssociationList;i++) {
      APP_DBG("COORD MAC APP - Short address device %d: 0x%02x\r\n",i,shortAssociationAddrList[i]);
    }
  }else{
     APP_DBG("COORD MAC APP - No device connected\r\n");
  }
}

void APPE_Button2Action(void)
{
  /* Node still associated */
  indDeviceSendData = 0; // Reset variable if we put several time on BT2
  APP_FFD_MAC_802_15_4_SendData();
}

void APPE_Button3Action(void)
{
}

void app_mac_regMacCallback( ST_MAC_callbacks_t * macCallback)
{
  
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

void APP_MAC_Init(void)
{
  /* Disable Low power */
  UTIL_LPM_SetStopMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);
  UTIL_LPM_SetOffMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);

  /* Register tasks */
  UTIL_SEQ_RegTask(TASK_FFD, UTIL_SEQ_RFU, APP_FFD_MAC_802_15_4_SetupTask); // Setup task

  UTIL_SEQ_RegTask( 1<<CFG_TASK_SEND_DATA, UTIL_SEQ_RFU, APP_FFD_MAC_802_15_4_SendData); // Send data
  
  /* Timer LED */
  UTIL_TIMER_Status_t tmr_status = UTIL_TIMER_OK;
  tmr_status = UTIL_TIMER_Create(&LEDTimer, SEND_TIMEOUT, UTIL_TIMER_ONESHOT,
                                 &LEDTimerCallback, NULL);
  if (tmr_status != UTIL_TIMER_OK){
    APP_DBG("COORD MAC APP - Create Timer Fails\r\n");
  }
  
  /* Configuration MAC 802_15_4 */
  app_mac_regMacCallback(&macCallback);
  ST_MAC_init(&macCallback);
  
  /* Execute once at startup */
  UTIL_SEQ_SetTask(TASK_FFD, TASK_MAC_APP_PRIO);
}

void APP_FFD_MAC_802_15_4_SetupTask(void)
{

  MAC_Status_t MacStatus;
  ST_MAC_resetReq_t ResetReq;
  ST_MAC_startReq_t StartReq;
  
  uint8_t PIB_Value = 0x00;
  uint8_t start_time[4] = {0x00, 0x00, 0x00, 0x00};
  /** Select all channels 
    * The channel numbers to be scanned. The 27 bits (b0, b1, …, b26) indicate 
    * which channels are to be scanned (1 = scan, 0 = do not scan) for each 
    * of the 27 channels.
   **/
  uint8_t scan_channel[4] = {0x00, 0xF8, 0xFF, 0x07};

  APP_DBG("COORD MAC APP - Coordinator Startup\r\n");
  
  /* Reset MAC */
  memset(&ResetReq, 0x00, sizeof(ST_MAC_resetReq_t));
  ResetReq.set_default_PIB = TRUE;
    
  MacStatus = ST_MAC_MLMEResetReq(mac_hndl, &ResetReq );
  if (MAC_SUCCESS != MacStatus) {
    APP_DBG("COORD MAC APP - Reset Fails\r\n");
    return;
  }
  /* Wait RESET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_RESET_CNF);    

  /* Get automatically the extended address, but already set during MAC init */
  otPlatRadioGetIeeeEui64(NULL,extAddr);
  APP_DBG("COORD MAC APP - Extended address: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\r\n",extAddr[7],
          extAddr[6],extAddr[5],extAddr[4],extAddr[3],extAddr[2],extAddr[1],extAddr[0]);

  /* Set MAC_AUTO_REQUEST to 0 to able to receive beaconNotify.ind */
  PIB_Value = FALSE; 
  Prepare_MLME_Set_Req(g_MAC_AUTO_REQUEST_c, (uint8_t *) &PIB_Value);

  /* Set Beacon payload for node identify our coordinator */
  PIB_Value = BEACON_PAYLOAD_SIZE; 
  Prepare_MLME_Set_Req(g_MAC_BEACON_PAYLOAD_LENGTH_c, (uint8_t *) &PIB_Value);

  /* Set Beacon payload for node identify our coordinator */
  Prepare_MLME_Set_Req(g_MAC_BEACON_PAYLOAD_c, (uint8_t *) &Beacon_payload[0]);
  
  /* Set Device Short Address */
  Prepare_MLME_Set_Req(g_MAC_SHORT_ADDRESS_c, (uint8_t *) &shortAddr);
   
  /* Set Association Permit to 1, allows device to associate with this device */
  PIB_Value = g_TRUE; 
  Prepare_MLME_Set_Req(g_MAC_ASSOCIATION_PERMIT_c, (uint8_t *) &PIB_Value);

  /* Set TX Power to 10 dBm, range between [-20;10] */
  PIB_Value = 10; 
  Prepare_MLME_Set_Req(g_PHY_TRANSMIT_POWER_c, (uint8_t *) &PIB_Value);

#ifndef DEMO_CHANNEL
  /* ED Scan and select channel */
  APP_DBG("COORD MAC APP - ED Scan on all channels\r\n");
  Prepare_MLME_Scan_Req(g_MAC_ED_SCAN_TYPE_c, BASE_SCAN_DURATION, &scan_channel[0]);
#else
  g_channel = DEMO_CHANNEL;
  APP_DBG("COORD MAC APP - Select the channel manually: %d \r\n", g_channel);
#endif
  /* Active Scan and getting all PANIDs */
  memset(&tab_panId_already_use, 0x00, sizeof(tab_panId_already_use)); // Empty the table
  
  APP_DBG("COORD MAC APP - Active Scan on all channels\r\n");
  Prepare_MLME_Scan_Req(g_MAC_ACTIVE_SCAN_TYPE_c, BASE_SCAN_DURATION, &scan_channel[0]);
  
  /* We perform a second active scan to increase the chances of getting all PANIDs. */
  APP_DBG("COORD MAC APP - Second active Scan on all channels\r\n");
  Prepare_MLME_Scan_Req(g_MAC_ACTIVE_SCAN_TYPE_c, BASE_SCAN_DURATION, &scan_channel[0]);

  /** Select PANID not used and check if this PANID in tab_panId_already_use 
    * if DEMO_PANID is already use, we take a random other.
  **/
#ifdef MAC_802_15_4_RANDOM_PAN_ID
  uint32_t tmp_pan_id = 0x00000000; // Take random PANID
  uint8_t panid_index = 0;
  while(panid_index < MAX_TAB_PAN_ID) {
    if (tab_panId_already_use[panid_index] == panId || panId == 0x0000 || panId == 0xFFFF) {
      HW_RNG_Get(2, &tmp_pan_id);
      panId = tmp_pan_id & 0xFFFF; // Convert to 16 bits
      panid_index = 0;
    }else {
      panid_index++;
    }
  }
#else
  panId = DEMO_PANID;
#endif
  
  APP_DBG("COORD MAC APP - Select PAN ID 0x%04x\r\n", panId);
  /* Start COORD Device */  
  memset(&StartReq, 0x00, sizeof(ST_MAC_startReq_t));
  memcpy(StartReq.a_PAN_id, (uint8_t *) &panId, 0x02); // PANID selected to start the network
  StartReq.channel_number   = g_channel; // Channel selected to start the network
  StartReq.beacon_order     = 0x0F; // Always this value in 802.15.4 non-beacon enabled
  StartReq.superframe_order = 0x0F; // Always this value in 802.15.4 non-beacon enabled
  StartReq.PAN_coordinator  = g_TRUE; // Become PAN COORDINATOR
  StartReq.battery_life_extension = g_FALSE; // Always this value in 802.15.4 non-beacon enabled
  memcpy(StartReq.a_start_time, (uint8_t *) &start_time, 0x04);
    
  MacStatus = ST_MAC_MLMEStartReq(mac_hndl, &StartReq);
  if (MAC_SUCCESS != MacStatus) {
    APP_DBG("COORD MAC APP - Set Association Permit Fails\r\n");
    return;
  }
  /* Wait START CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_START_CNF);
    
  /* Set RxOnWhenIdle to 1 to able to receive frames */
  PIB_Value = g_TRUE; 
  Prepare_MLME_Set_Req(g_MAC_RX_ON_WHEN_IDLE_c, (uint8_t *) &PIB_Value);    

  APP_DBG("COORD MAC APP - Ready to handle association and receive data\r\n");
  BSP_LED_On(LD1);

}

void APP_FFD_MAC_802_15_4_SendData(void)
{
  MAC_Status_t MacStatus;
  ST_MAC_dataReq_t DataReq;
  static uint16_t tmp_dst_a_short_addr = 0x0000;
  char Data[18] = "DEVICE STILL HERE";
  
  /* structure initialization */
  memset(&DataReq, 0x00, sizeof(ST_MAC_dataReq_t));
  DataReq.src_addr_mode = g_SHORT_ADDR_MODE_c; // Address source mode
  DataReq.dst_addr_mode = g_SHORT_ADDR_MODE_c; // Address destination mode
  if (indDeviceSendData < idAssociationList) {
    memcpy(DataReq.a_dst_PAN_id, &panId, 0x02); // PANID destination
    tmp_dst_a_short_addr = shortAssociationAddrList[indDeviceSendData];
    memcpy(DataReq.dst_address.a_short_addr, &tmp_dst_a_short_addr, 0x02); // Address destination 

    DataReq.msdu_handle = g_dataHandle++; // The handle associated with the MSDU
    DataReq.ack_Tx = TRUE; // Request ACK = TRUE, otherwise FALSE
    DataReq.GTS_Tx = FALSE; // Always this value in 802.15.4 non-beacon enabled
    memcpy(&rfBuffer, Data, strlen(Data));
    
    /* Xor Data to potentially verified data paylaod on receiver side (optional) */
    rfBuffer[strlen(Data)] = xorSign(Data, strlen(Data));
    DataReq.msduPtr = (uint8_t *) rfBuffer; // Data payload
    DataReq.msdu_length = strlen(Data) + 1; // Data length
    DataReq.security_level = g_MAC_UNSECURED_MODE_c; // Unsecure frame
    MacStatus = ST_MAC_MCPSDataReq(mac_hndl, &DataReq);
    if (MAC_SUCCESS != MacStatus) {
      APP_DBG("COORD MAC APP - Data Req Fails\r\n");
      return;
    }
    
    /* Wait DATA CONFIRMATION */
    UTIL_SEQ_WaitEvt(EVENT_DATA_CNF);
    if (status_check_device == MAC_SUCCESS) {
      APP_DBG("COORD MAC APP - Check if device 0x%04x still connected -> OK\r\n", tmp_dst_a_short_addr);
      indDeviceSendData++; // Pass to the next list element
    } else {
        APP_DBG("COORD MAC APP - Check if device 0x%04x still connected -> KO because 0x%02x\r\n", tmp_dst_a_short_addr, status_check_device);
    }
  }
}