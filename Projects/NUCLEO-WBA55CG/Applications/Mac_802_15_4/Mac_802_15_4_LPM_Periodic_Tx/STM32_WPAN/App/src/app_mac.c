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
#include "scm.h"
#include "app_mac_802_15_4_process.h"
#include "stm32_timer.h"
#include "st_mac_802_15_4_types.h"
#include "st_mac_802_15_4_sap.h"
#include "log_module.h"

/* Private typedef -----------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
extern LPTIM_HandleTypeDef hlptim1;

/* Private define ------------------------------------------------------------*/
#define DEMO_CHANNEL             11
#define DATA "HELLO COORDINATOR\0"
#define FRAME_MAX_SIZE 127

/* Set the Maximum value of the counter (Auto-Reload) that defines the Period */
#define PERIOD               (uint32_t) (32768 - 1) // 1s
#define SEND_TIMEOUT  (1000) /**< 1s */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t rfBuffer[256];
static uint16_t     g_panId             = 0x1AAA;
static uint8_t      g_dataHandle        = 0x02;
static uint16_t     g_shortAddr         = 0xABCD;
static uint16_t     g_broadcastAddr     = 0xFFFF; /* Broadcast Addr */
static uint8_t      g_channel           = DEMO_CHANNEL;

#if CFG_LPM_STDBY_SUPPORTED
static UTIL_TIMER_Object_t SendTimer;
#endif

/* Private function prototypes -----------------------------------------------*/
static uint8_t xorSign( const char * pmessage, uint8_t message_len);

#if CFG_LPM_STDBY_SUPPORTED
static void SendTimerCallback(void *arg);
#else
static void HAL_LPTIM_CompareMatchCallback(LPTIM_HandleTypeDef *hlptim);
static void Start_Timer_MicroSec_AddFifo(uint32_t TimeUs);
#endif

void app_mac_regMacCallback( ST_MAC_callbacks_t * macCallback);

/* Public variables ---------------------------------------------------------*/
MAC_handle mac_hndl;
ST_MAC_callbacks_t macCallback;

/* Private function -----------------------------------------------*/
#if CFG_LPM_STDBY_SUPPORTED
static void SendTimerCallback(void *arg)
{
  // Request Data Transmit on Compare Match callback
  UTIL_SEQ_SetTask(1 << CFG_TASK_DATA_NODE, TASK_MAC_APP_PRIO );
}
#else
static void HAL_LPTIM_UpdateEventCallback(LPTIM_HandleTypeDef *hlptim)
{
  // Request Data Transmit on Compare Match callback
  UTIL_SEQ_SetTask(1 << CFG_TASK_DATA_NODE, TASK_MAC_APP_PRIO );

}

static void Start_Timer_MicroSec_AddFifo(uint32_t TimeUs)
{

  // uint32_t Ticks = (TimeUs-8)*50; //LPTIM = 50MHz; -8 because of IRQ and LPMTIM Init Processing time
  hlptim1.Init.Period = TimeUs;
  /* Set LPTIM Period */
  HAL_LPTIM_Init(&hlptim1);
  
  __HAL_LPTIM_ENABLE(&hlptim1);
  
  __HAL_LPTIM_ENABLE_IT(&hlptim1, LPTIM_IT_UPDATE);
  
  /* Start timer in continuous mode */
  __HAL_LPTIM_START_CONTINUOUS(&hlptim1);

}
#endif
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

void APP_MAC_Init(void) {
  /* Disable Low power */
  UTIL_LPM_SetStopMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);
  UTIL_LPM_SetOffMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);

  /* Register tasks */
  UTIL_SEQ_RegTask(TASK_RFD, UTIL_SEQ_RFU, APP_LPM_TRANSMIT_MAC_802_15_4_SetupTask);
  /* Register Data emiter node task */
  UTIL_SEQ_RegTask( 1<<CFG_TASK_DATA_NODE, UTIL_SEQ_RFU, APP_LPM_TRANSMIT_MAC_802_15_4_NodeDataTask);
  
  /* Configuration MAC 802_15_4 */
  app_mac_regMacCallback(&macCallback);
  ST_MAC_init(&macCallback);
  #if CFG_LPM_STDBY_SUPPORTED
  /* Create Timer for standby mode*/
    UTIL_TIMER_Status_t tmr_status = UTIL_TIMER_OK;
  /**
   * Create timer to handle COAP request sending
   */
  tmr_status = UTIL_TIMER_Create(&SendTimer, SEND_TIMEOUT, UTIL_TIMER_PERIODIC,
                                 &SendTimerCallback, NULL);
  if (tmr_status != UTIL_TIMER_OK){
    while(1);
  }
#endif
  
  /* Execute once at startup */
  UTIL_SEQ_SetTask(TASK_RFD, TASK_MAC_APP_PRIO);
}

void APP_LPM_TRANSMIT_MAC_802_15_4_NodeDataTask(void)
{
  APP_LPM_TRANSMIT_MAC_802_15_4_SendData(DATA);
}

void APP_LPM_TRANSMIT_MAC_802_15_4_SetupTask(void)
{
  MAC_Status_t MacStatus;
  ST_MAC_resetReq_t    ResetReq;
  ST_MAC_setReq_t      SetReq;
  int8_t tx_power_pib_value;

  APP_DBG("Run LPM MAC 802.15.4 - 2 - LPM Startup\r\n");
  /* Reset MAC */
  memset(&ResetReq,0x00,sizeof(ST_MAC_resetReq_t));
  ResetReq.set_default_PIB = TRUE;
  APP_DBG("LPM MAC - Reset Req\r\n");
  MacStatus = ST_MAC_MLMEResetReq(mac_hndl,&ResetReq );
  if ( MAC_SUCCESS != MacStatus ) {
    APP_DBG("LPM MAC - Reset Fails\r\n");
    return;
  }
  /* Wait RESET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_RESET_CNF);

  /* Setup device for Silent Start */
  /* Set Device Short Address */
  memset(&SetReq,0x00,sizeof(ST_MAC_setReq_t));
  SetReq.PIB_attribute = g_MAC_SHORT_ADDRESS_c;
  SetReq.PIB_attribute_valuePtr = (uint8_t*)&g_shortAddr;
  MacStatus = ST_MAC_MLMESetReq(mac_hndl, &SetReq );
  if ( MAC_SUCCESS != MacStatus ) {
    APP_DBG("LPM MAC - Set Short Addr Fails\r\n");
    return;
  }
  /* Wait SET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SET_CNF);

  APP_DBG("LPM MAC APP - Set PAN ID REQ\n\r");
  /* Set PanID Address */
  memset(&SetReq,0x00,sizeof(ST_MAC_setReq_t));
  SetReq.PIB_attribute = g_MAC_PAN_ID_c;
  SetReq.PIB_attribute_valuePtr = (uint8_t*)&g_panId;
  MacStatus = ST_MAC_MLMESetReq(mac_hndl, &SetReq );
  if ( MAC_SUCCESS != MacStatus ) {
    APP_DBG("LPM MAC - Set PAN ID Req Fails\n\r");
    return;
  }
  /* Wait SET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SET_CNF);

  /* Setup TX Channel */
  memset(&SetReq,0x00,sizeof(ST_MAC_setReq_t));
  SetReq.PIB_attribute = g_PHY_CURRENT_CHANNEL_c;
  SetReq.PIB_attribute_valuePtr = (uint8_t*)&g_channel;
  MacStatus = ST_MAC_MLMESetReq(mac_hndl,&SetReq);
  if ( MAC_SUCCESS != MacStatus ) {
    APP_DBG("LPM MAC - Set channel Fails\n\r");
    return;
  }
  /* Wait SET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SET_CNF);

  /* Set Tx Power */
  APP_DBG("LPM MAC APP - Set TX Power\r\n");
  memset(&SetReq,0x00,sizeof(ST_MAC_setReq_t));
  SetReq.PIB_attribute = g_PHY_TRANSMIT_POWER_c;
  tx_power_pib_value = 10; // in dBm value in the range [-20;10]
  SetReq.PIB_attribute_valuePtr = (uint8_t *)&tx_power_pib_value;
  MacStatus = ST_MAC_MLMESetReq(mac_hndl, &SetReq );
  if ( MAC_SUCCESS != MacStatus ) {
    APP_DBG("LPM MAC - Set Tx Power Fails\r\n");
    return;
  }
  /* Wait SET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SET_CNF);
    
  APP_DBG("LPM MAC APP - Ready \r\n"); // To Handle Association Req and Receive Data
#if (CFG_LPM_STDBY_SUPPORTED == 1)
  UTIL_SEQ_SetTask(1 << CFG_TASK_DATA_NODE, TASK_MAC_APP_PRIO );
  UTIL_TIMER_Start(&SendTimer);
#else 
  /* Start LPTimer to trigg the data emiter node task */
  Start_Timer_MicroSec_AddFifo(PERIOD);
#endif

}

void APP_LPM_TRANSMIT_MAC_802_15_4_SendData(const char * data)
{
  MAC_Status_t MacStatus;
  ST_MAC_dataReq_t DataReq;
  
  /* If the payload frame is greater than 127 bytes, we don't send the frame */
  if (strlen(data) > FRAME_MAX_SIZE) {
      return;
  }
  
  /* structure initialization */
  memset(&DataReq, 0x00, sizeof(ST_MAC_dataReq_t));
  
  DataReq.src_addr_mode = g_SHORT_ADDR_MODE_c; // Address source mode
  DataReq.dst_addr_mode = g_SHORT_ADDR_MODE_c; // Address destination mode
  memcpy(DataReq.a_dst_PAN_id,&g_panId,0x02); // PANID destination
  memcpy(DataReq.dst_address.a_short_addr,&g_broadcastAddr,0x02); // Address destination
  DataReq.msdu_handle = g_dataHandle++; // The handle associated with the MSDU
  DataReq.ack_Tx = 0x00;  // No ACK is required
  memcpy(&rfBuffer,data,strlen(data));

  /* Xor Data to potentially verified data paylaod on receiver side (optional) */
  rfBuffer[strlen(data)] = xorSign(data,strlen(data));
  DataReq.msduPtr = (uint8_t*) rfBuffer; // Data payload
  DataReq.msdu_length = strlen(data)+1; // Data length

  /* Request Data transmission */
  MacStatus = ST_MAC_MCPSDataReq(mac_hndl, &DataReq );
  if ( MAC_SUCCESS != MacStatus ) {
    APP_DBG("LPM MAC APP - Data Req Fails\r\n");
    return;
  }
  
  /* Wait DATA CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_DATA_CNF);
}
