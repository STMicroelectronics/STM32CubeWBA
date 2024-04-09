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
#include "st_mac_802_15_4_types.h"
#include "st_mac_802_15_4_sap.h"


/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define DEMO_CHANNEL             11
#define DATA "HELLO COORDINATOR\0"
#define FRAME_MAX_SIZE           127

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t rfBuffer[128];
static uint16_t     g_panId             = 0x1AAA;
static uint16_t     g_coordShortAddr    = 0x1122;
static uint8_t      g_dataHandle        = 0x02;
static long long    g_extAddr           = 0xACDE480000000002;
static uint8_t      g_channel           = DEMO_CHANNEL;
static uint8_t      g_channel_page      = 0x00;


/* Private function prototypes -----------------------------------------------*/
static uint8_t xorSign( const char * pmessage, uint8_t message_len);

/* Public variables ---------------------------------------------------------*/
MAC_handle mac_hndl;
ST_MAC_callbacks_t macCallback;
ST_MAC_associateCnf_t g_MAC_associateCnf;

/* Private function  ---------------------------------------------------------*/
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


/* USER CODE BEGIN FD*/
void APPE_Button1Action(void)
{
  APP_RFD_MAC_802_15_4_SendData("Data From Node\0");
}

void APPE_Button2Action(void)
{
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

void APP_MAC_Init(void)
{
  /* Disable Low power */
  UTIL_LPM_SetStopMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);
  UTIL_LPM_SetOffMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);

  /* Register tasks */
  UTIL_SEQ_RegTask(TASK_RFD, UTIL_SEQ_RFU, APP_RFD_MAC_802_15_4_SetupTask); // Setup task

  
  /* Configuration MAC 802_15_4 */
  app_mac_regMacCallback(&macCallback);
  ST_MAC_init(&macCallback);
  
  /* Execute once at startup */
  UTIL_SEQ_SetTask(TASK_RFD, TASK_MAC_APP_PRIO);
}

void APP_RFD_MAC_802_15_4_SetupTask(void)
{

  MAC_Status_t MacStatus;
  ST_MAC_resetReq_t    ResetReq;
  ST_MAC_setReq_t      SetReq;
  ST_MAC_associateReq_t AssociateReq;
  int8_t tx_power_pib_value = 0;

  APP_DBG("Run RFD MAC 802.15.4 - 2 - RFD Startup\r\n");
  /* Reset MAC */
  memset(&ResetReq,0x00,sizeof(ST_MAC_resetReq_t));
  ResetReq.set_default_PIB = TRUE;
  APP_DBG("RFD MAC - Reset Req\r\n");
  MacStatus = ST_MAC_MLMEResetReq(mac_hndl,&ResetReq );
  if ( MAC_SUCCESS != MacStatus ) {
    APP_DBG("RFD MAC - Reset Fails\r\n");
    return;
  }
  /* Wait RESET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_RESET_CNF);

  /* Set Device Extended Address */
  APP_DBG("RFD MAC APP - Set Extended Address\r\n");
  memset(&SetReq,0x00,sizeof(ST_MAC_setReq_t));
  SetReq.PIB_attribute = g_MAC_EXTENDED_ADDRESS_c;
  SetReq.PIB_attribute_valuePtr = (uint8_t*) &g_extAddr;
  MacStatus = ST_MAC_MLMESetReq(mac_hndl, &SetReq );
  if ( MAC_SUCCESS != MacStatus ) {
    APP_DBG("RFD MAC - Set Extended Addr Fails\r\n");
    return;
  }
  /* Wait SET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SET_CNF);
    
  /* Set Tx Power */
  APP_DBG("RFD MAC APP - Set TX Power\r\n");
  memset(&SetReq,0x00,sizeof(ST_MAC_setReq_t));
  SetReq.PIB_attribute = g_PHY_TRANSMIT_POWER_c;
  tx_power_pib_value = 10;                       /* int8 dBm value in the range [-20;10] */
  SetReq.PIB_attribute_valuePtr = (uint8_t *)&tx_power_pib_value;

  MacStatus = ST_MAC_MLMESetReq(mac_hndl, &SetReq );
  if ( MAC_SUCCESS != MacStatus ) {
    APP_DBG("RFFD MAC - Set Tx Power Fails\r\n");
    return;
  }
  /* Wait SET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SET_CNF);
    
  /* Association request */
  APP_DBG("RFD MAC APP - Association REQ\n\r");
  AssociateReq.channel_number   = g_channel;
  AssociateReq.channel_page     = g_channel_page;
  AssociateReq.coord_addr_mode  = g_SHORT_ADDR_MODE_c;
  memcpy(AssociateReq.coord_address.a_short_addr,&g_coordShortAddr,0x02);
  AssociateReq.capability_information = 0x80;
  memcpy(AssociateReq.a_coord_PAN_id,&g_panId,0x02);
  AssociateReq.security_level = 0x00;
  MacStatus = ST_MAC_MLMEAssociateReq(mac_hndl,&AssociateReq);
  if ( MAC_SUCCESS != MacStatus ) {
    APP_DBG("RFD MAC - Association Req Fails\n\r");
    return;
  }
  /* Wait SET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_ASSOCIATE_CNF);

  /* Set Device Short Address */
  APP_DBG("RFD MAC APP - Set Short Address\n\r");
  memset(&SetReq,0x00,sizeof(ST_MAC_setReq_t));
  SetReq.PIB_attribute = g_MAC_SHORT_ADDRESS_c;
  SetReq.PIB_attribute_valuePtr = (uint8_t*)&g_MAC_associateCnf.a_assoc_short_address;
  MacStatus = ST_MAC_MLMESetReq(mac_hndl,&SetReq);
  if ( MAC_SUCCESS != MacStatus ) {
    APP_DBG("RFD MAC - Set Short Addr Fails\n\r");
    return;
  }
 
  /* Wait SET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SET_CNF);
 
  APP_DBG("RFD MAC APP - Ready \r\n");//to Handle Association Req and Receive Data
  BSP_LED_On(LD1);
  APP_RFD_MAC_802_15_4_SendData(DATA);
}

void APP_RFD_MAC_802_15_4_SendData(const char * data)
{
  MAC_Status_t MacStatus;
  ST_MAC_dataReq_t DataReq;
  
  /* If the payload frame is greater than 127 bytes, we don't send the frame */
  if (strlen(data) > FRAME_MAX_SIZE) {
      return;
  }
  
  BSP_LED_On(LD3);
  APP_DBG("RFD MAC APP - Send Data to Coordinator\r\n");
  DataReq.src_addr_mode = g_SHORT_ADDR_MODE_c;
  DataReq.dst_addr_mode = g_SHORT_ADDR_MODE_c;

  memcpy(DataReq.a_dst_PAN_id,&g_panId,0x02);
  memcpy(DataReq.dst_address.a_short_addr,&g_coordShortAddr,0x02);

  DataReq.msdu_handle = g_dataHandle++;
  DataReq.ack_Tx =0x00;
  DataReq.GTS_Tx = FALSE;
  memcpy(&rfBuffer,data,strlen(data));
  rfBuffer[strlen(data)] = xorSign(data,strlen(data));
  DataReq.msduPtr = (uint8_t*) rfBuffer;
  DataReq.msdu_length = strlen(data)+1;
  DataReq.security_level = 0x00;
  MacStatus = ST_MAC_MCPSDataReq(mac_hndl,&DataReq);
  if ( MAC_SUCCESS != MacStatus ) {
    APP_DBG("RFD MAC - Data Req Fails\r\n");
    return;
  }
  
  UTIL_SEQ_WaitEvt(EVENT_DATA_CNF);
  BSP_LED_Off(LD3);
  APP_DBG("RFD MAC APP - DATA CNF Received\r\n");
}