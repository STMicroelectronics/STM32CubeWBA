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

/* Public variables ---------------------------------------------------------*/
MAC_handle mac_hndl; // Instance MAC 
ST_MAC_callbacks_t macCallback; // Structure with all callbacks
UTIL_TIMER_Object_t LEDTimer; // LED timer

uint16_t shortAssociationAddrList[MAX_DEVICES_ASSO]; // Tab with all address associated devices
uint8_t idAssociationList = 0; //number of elements in the tab
uint8_t indDeviceSendData = 0; //indice to check if the device still connected
uint8_t retry_check_device = 0; //allows to retry when data req is sending

/* All variables for configuration and set data, define in app_conf.h */
uint8_t extAddr[8]          = {0xAC,0xDE,0x48,00,0x00,0x00,0x00,0x01};
uint16_t shortAddr          = COORD_ADDRESS;
uint16_t panId              = DEMO_PANID;
uint8_t g_channel           = 11; // General channel


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

static void LEDTimerCallback (void * arg){
  BSP_LED_Off(LD2);
  BSP_LED_Off(LD3);
}

/* USER CODE BEGIN FD*/
void APPE_Button1Action(void)
{
  /* Display all associated devices */
  if (idAssociationList > 0){
    for (uint8_t i=0;i<idAssociationList;i++){
      APP_DBG("COORD MAC - Short address device %d: 0x%02x\r\n",i,shortAssociationAddrList[i]);
    }
  }else{
     APP_DBG("COORD MAC - No device connected\r\n");
  }
}

void APPE_Button2Action(void)
{
  /* Node still associated */
  indDeviceSendData = 0;//reset variable if we put several time on BT2
  APP_FFD_MAC_802_15_4_SendData();
}

void APPE_Button3Action(void)
{
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
  UTIL_SEQ_RegTask(TASK_FFD, UTIL_SEQ_RFU, APP_FFD_MAC_802_15_4_SetupTask); // Setup task

  UTIL_SEQ_RegTask( 1<<TASK_BUTTON_1, UTIL_SEQ_RFU, APPE_Button1Action); // Button 1
  UTIL_SEQ_RegTask( 1<<TASK_BUTTON_2, UTIL_SEQ_RFU, APPE_Button2Action); // Button 2
  UTIL_SEQ_RegTask( 1<<TASK_BUTTON_3, UTIL_SEQ_RFU, APPE_Button3Action); // Button 3
  UTIL_SEQ_RegTask( 1<<CFG_TASK_SEND_DATA, UTIL_SEQ_RFU, APP_FFD_MAC_802_15_4_SendData); // Send data
  
  /* Timer LED */
  UTIL_TIMER_Status_t tmr_status = UTIL_TIMER_OK;
  tmr_status = UTIL_TIMER_Create(&LEDTimer, SEND_TIMEOUT, UTIL_TIMER_ONESHOT,
                                 &LEDTimerCallback, NULL);
  if (tmr_status != UTIL_TIMER_OK){
    APP_DBG("Node MAC APP - Create Timer Fails\r\n");
  }
  
  /* Configuration MAC 802_15_4 */
  app_mac_regMacCallback(&macCallback);
  ST_MAC_init(&macCallback);
  
  /* Execute once at startup */
  UTIL_SEQ_SetTask(TASK_FFD, TASK_MAC_APP_PRIO);
}

void APP_FFD_MAC_802_15_4_SetupTask(void)
{

  MAC_Status_t MacStatus = MAC_ERROR;

  ST_MAC_resetReq_t ResetReq;
  ST_MAC_setReq_t   SetReq;
  ST_MAC_startReq_t StartReq;
  ST_MAC_scanReq_t  ScanReq;

  uint8_t PIB_Value = 0x00;
  int8_t tx_power_pib_value = 0;
  uint8_t start_time[4] = {0x00, 0x00, 0x00, 0x00};
  uint8_t scan_channel[4];

  APP_DBG("Run Coordinator MAC 802.15.4 - Coordinator Startup\r\n");
  /* Reset MAC */
  memset(&ResetReq, 0x00, sizeof(ST_MAC_resetReq_t));
  ResetReq.set_default_PIB = TRUE;
    
  MacStatus = ST_MAC_MLMEResetReq(mac_hndl, &ResetReq );
  if (MAC_SUCCESS != MacStatus) {
    APP_DBG("COORD MAC - Reset Fails\r\n");
    return;
  }
  /* Wait RESET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_RESET_CNF);    

  /* Get automatically the extended address*/
  otPlatRadioGetIeeeEui64(NULL,extAddr);
  APP_DBG("COORD MAC APP - Extended address: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\r\n",extAddr[0],
          extAddr[1],extAddr[2],extAddr[3],extAddr[4],extAddr[5],extAddr[6],extAddr[7]);
  /* Set Device Extended Address */
  memset(&SetReq, 0x00, sizeof(ST_MAC_setReq_t));
  SetReq.PIB_attribute = g_MAC_EXTENDED_ADDRESS_c;
  SetReq.PIB_attribute_valuePtr = (uint8_t *) &extAddr[0];
    
  MacStatus = ST_MAC_MLMESetReq(mac_hndl, &SetReq);
  if (MAC_SUCCESS != MacStatus) {
    APP_DBG("COORD MAC APP - Set Extended Addr Fails\r\n");
    return;
  }
  /* Wait SET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SET_CNF);    

  /* Set Device Short Address */
  memset(&SetReq,0x00,sizeof(ST_MAC_setReq_t));
  SetReq.PIB_attribute = g_MAC_SHORT_ADDRESS_c;
  SetReq.PIB_attribute_valuePtr =(uint8_t *) &shortAddr;
   
  MacStatus = ST_MAC_MLMESetReq(mac_hndl, &SetReq);
  if (MAC_SUCCESS != MacStatus) {
    APP_DBG("COORD MAC APP - Set Short Addr Fails");
    return;
  }
  /* Wait SET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SET_CNF);
    
  /* Set Association Permit */
  memset(&SetReq, 0x00, sizeof(ST_MAC_setReq_t));
  SetReq.PIB_attribute = g_MAC_ASSOCIATION_PERMIT_c;
  PIB_Value = g_TRUE;
  SetReq.PIB_attribute_valuePtr = &PIB_Value;

  MacStatus = ST_MAC_MLMESetReq(mac_hndl, &SetReq);
  if ( MAC_SUCCESS != MacStatus ) {
    APP_DBG("COORD MAC APP - Set Association Permit Fails\r\n");
    return;
  }
  /* Wait SET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SET_CNF);    

  /* Set TX Power */
  memset(&SetReq,0x00,sizeof(ST_MAC_setReq_t));
  SetReq.PIB_attribute = g_PHY_TRANSMIT_POWER_c;
  tx_power_pib_value = 10;  /* int8 dBm value in the range [-20;10] */
  SetReq.PIB_attribute_valuePtr = (uint8_t *) &tx_power_pib_value;

  MacStatus = ST_MAC_MLMESetReq(mac_hndl, &SetReq);
  if (MAC_SUCCESS != MacStatus) {
    APP_DBG("COORD MAC APP - Set Tx Power Fails\r\n");
    return;
  }
  /* Wait SET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SET_CNF);    

  /* ED Scan and select channel */
  APP_DBG("COORD MAC APP - ED Scan on all channels\r\n");
  /** Select all channels 
   * The channel numbers to be scanned. The 27 bits (b0, b1, …, b26) indicate 
   * which channels are to be scanned (1 = scan, 0 = do not scan) for each 
   * of the 27 channels.
   **/
  scan_channel[0] = 0x00;
  scan_channel[1] = 0xF8;
  scan_channel[2] = 0xFF;
  scan_channel[3] = 0x07;
  memset(&ScanReq, 0x00, sizeof(ST_MAC_scanReq_t));
  ScanReq.scan_type = g_MAC_ED_SCAN_TYPE_c; //ED scan
  ScanReq.scan_duration = g_PHY_MAX_FRAME_DURATION_c;//Max frame duration
  ScanReq.channel_page = g_channel_page;
  ScanReq.security_level = g_MAC_UNSECURED_MODE_c; //unsecure frame
  memcpy(&ScanReq.a_scan_channels, (uint8_t *) &scan_channel, 0x04);
   
  MacStatus = ST_MAC_MLMEScanReq(mac_hndl, &ScanReq);
  if (MAC_SUCCESS != MacStatus) {
    APP_DBG("COORD MAC APP - ED Scan on all channels Fails\r\n");
    return;
  }
  /* Wait SCAN CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SCAN_CNF);  

  /* Start COORD Device */  
  memset(&StartReq, 0x00, sizeof(ST_MAC_startReq_t));
  memcpy(StartReq.a_PAN_id, (uint8_t *) &panId, 0x02);
  StartReq.channel_number   = g_channel;
  StartReq.beacon_order     = 0x0F;
  StartReq.superframe_order = 0x0F;
  StartReq.PAN_coordinator  = g_TRUE;
  StartReq.battery_life_extension = g_FALSE;
  memcpy(StartReq.a_start_time, (uint8_t *) &start_time, 0x04);
    
  MacStatus = ST_MAC_MLMEStartReq(mac_hndl, &StartReq);
  if (MAC_SUCCESS != MacStatus) {
    APP_DBG("COORD MAC APP - Set Association Permit Fails\r\n");
    return;
  }
  /* Wait START CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_START_CNF);
    
  /* Set RxOnWhenIdle */
  memset(&SetReq, 0x00, sizeof(ST_MAC_setReq_t));
  SetReq.PIB_attribute = g_MAC_RX_ON_WHEN_IDLE_c;
  PIB_Value = g_TRUE;
  SetReq.PIB_attribute_valuePtr = &PIB_Value;
    
  MacStatus = ST_MAC_MLMESetReq(mac_hndl, &SetReq);
  if (MAC_SUCCESS != MacStatus) {
    APP_DBG("COORD MAC APP - Set Rx On When Idle Fails\r\n");
    return;
  }
  /* Wait SET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SET_CNF);     

  APP_DBG("COORD MAC APP - Ready to handle association and receive data with PANID 0x%04x\r\n", panId);
  BSP_LED_On(LD1);

}

void APP_FFD_MAC_802_15_4_SendData(void)
{
  MAC_Status_t MacStatus = MAC_ERROR;
  ST_MAC_dataReq_t DataReq;
  char Data[127] = "DEVICE STILL HERE";
  
  DataReq.src_addr_mode = g_SHORT_ADDR_MODE_c;
  DataReq.dst_addr_mode = g_SHORT_ADDR_MODE_c;
  if (indDeviceSendData<idAssociationList){
    memcpy(DataReq.a_dst_PAN_id, &panId, 0x02);
    memcpy(DataReq.dst_address.a_short_addr, &shortAssociationAddrList[indDeviceSendData], 0x02);
    if (!retry_check_device) {
      APP_DBG("COORD MAC APP - Check if device 0x%02x still connected -> ", shortAssociationAddrList[indDeviceSendData]);
    }
    DataReq.msdu_handle = g_dataHandle++;
    DataReq.ack_Tx = TRUE;
    DataReq.GTS_Tx = FALSE;
    memcpy(&rfBuffer, Data, strlen(Data));
    rfBuffer[strlen(Data)] = xorSign(Data, strlen(Data));
    DataReq.msduPtr = (uint8_t *) rfBuffer;
    DataReq.msdu_length = strlen(Data) + 1;
    DataReq.security_level = g_MAC_UNSECURED_MODE_c;
    MacStatus = ST_MAC_MCPSDataReq(mac_hndl, &DataReq);
    if (MAC_SUCCESS != MacStatus) {
      APP_DBG("COORD MAC APP - Data Req Fails\r\n");
      return;
    }
    
    UTIL_SEQ_WaitEvt(EVENT_DATA_CNF);
  }
}