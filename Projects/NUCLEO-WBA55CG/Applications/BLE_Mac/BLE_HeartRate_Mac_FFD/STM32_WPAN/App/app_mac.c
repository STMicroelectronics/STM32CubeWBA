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
#include "app_conf.h"
#include "stm32wbaxx_hal.h"
#include "stm32wbaxx_nucleo.h"
#include "common_types.h"
#include "app_mac.h"
#include "stm32_seq.h"
#include "stm32_lpm.h"
#include "app_mac_802_15_4_process.h"
#include "st_mac_802_15_4_types.h"
#include "st_mac_802_15_4_sap.h"
#include "log_module.h"



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DEMO_CHANNEL             11
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Public variables ---------------------------------------------------------*/
MAC_handle mac_hndl;
ST_MAC_callbacks_t macCallback;
ST_MAC_associateInd_t g_MAC_associateInd;

/* USER CODE BEGIN FD*/

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
  UTIL_SEQ_RegTask(TASK_FFD, UTIL_SEQ_RFU, APP_FFD_MAC_802_15_4_SetupTask); // Setup task

  
  /* Configuration MAC 802_15_4 */
  app_mac_regMacCallback(&macCallback);
  ST_MAC_init(&macCallback);
  
  /* Execute once at startup */
  UTIL_SEQ_SetTask(TASK_FFD, TASK_MAC_APP_PRIO);
}

void APP_FFD_MAC_802_15_4_SetupTask(void)
{

  MAC_Status_t MacStatus;

  ST_MAC_resetReq_t    ResetReq;
  ST_MAC_setReq_t      SetReq;
  ST_MAC_startReq_t    StartReq;

  static long long extAddr = 0xACDE480000000001;
  uint16_t shortAddr   = 0x1122;
  uint16_t panId       = 0x1AAA;
  uint8_t channel      = DEMO_CHANNEL;
  uint8_t PIB_Value = 0x00;
  
  int8_t tx_power_pib_value = 0;

  APP_DBG("Run FFD MAC 802.15.4 - 2 - FFD Startup\r\n");
  /* Reset MAC */
  memset(&ResetReq,0x00,sizeof(ST_MAC_resetReq_t));
  ResetReq.set_default_PIB = TRUE;
  APP_DBG("FFD MAC - Reset Req\r\n");
    
  MacStatus = ST_MAC_MLMEResetReq(mac_hndl,&ResetReq );
  if ( MAC_SUCCESS != MacStatus ) {
    APP_DBG("FFD MAC - Reset Fails\r\n");
    return;
  }
  /* Wait RESET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_RESET_CNF);

  /* Set Device Extended Address */
  APP_DBG("FFD MAC APP - Set Extended Address\r\n");
  memset(&SetReq,0x00,sizeof(ST_MAC_setReq_t));
  SetReq.PIB_attribute = g_MAC_EXTENDED_ADDRESS_c;
  SetReq.PIB_attribute_valuePtr = (uint8_t*) &extAddr;
    
  MacStatus = ST_MAC_MLMESetReq(mac_hndl, &SetReq );
  if ( MAC_SUCCESS != MacStatus ) {
    APP_DBG("FFD MAC - Set Extended Addr Fails\r\n");
    return;
  }
  /* Wait SET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SET_CNF);
    
  /* Set Device Short Address */
  APP_DBG("FFD MAC APP - Set Short Address\r\n");
  memset(&SetReq,0x00,sizeof(ST_MAC_setReq_t));
  SetReq.PIB_attribute = g_MAC_SHORT_ADDRESS_c;
  SetReq.PIB_attribute_valuePtr =(uint8_t*) &shortAddr;
    
  MacStatus = ST_MAC_MLMESetReq(mac_hndl, &SetReq );
  if ( MAC_SUCCESS != MacStatus ) {
    APP_DBG("FFD MAC - Set Short Addr Fails");
    return;
  }
  /* Wait SET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SET_CNF);
    
  APP_DBG("FFD MAC APP - Set Association Permit\r\n");
  memset(&SetReq,0x00,sizeof(ST_MAC_setReq_t));
  SetReq.PIB_attribute = g_MAC_ASSOCIATION_PERMIT_c;
  PIB_Value = g_TRUE;
  SetReq.PIB_attribute_valuePtr = &PIB_Value;

  MacStatus = ST_MAC_MLMESetReq(mac_hndl, &SetReq );
  if ( MAC_SUCCESS != MacStatus ) {
    APP_DBG("FFD MAC - Set Association Permit Fails\r\n");
    return;
  }
  /* Wait SET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SET_CNF);

  /* Set Tx Power */
  APP_DBG("FFD MAC APP - Set TX Power\r\n");
  memset(&SetReq,0x00,sizeof(ST_MAC_setReq_t));
  SetReq.PIB_attribute = g_PHY_TRANSMIT_POWER_c;
  tx_power_pib_value = 10;                       /* int8 dBm value in the range [-20;10] */
  SetReq.PIB_attribute_valuePtr = (uint8_t *)&tx_power_pib_value;

  MacStatus = ST_MAC_MLMESetReq(mac_hndl, &SetReq );
  if ( MAC_SUCCESS != MacStatus ) {
    APP_DBG("FFD MAC - Set Tx Power Fails\r\n");
    return;
  }
  /* Wait SET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SET_CNF);

  APP_DBG("FFD MAC APP - Start FFD Device\r\n");
  memset(&StartReq,0x00,sizeof(ST_MAC_startReq_t));
  memcpy(StartReq.a_PAN_id,(uint8_t*)&panId,0x02);
  StartReq.channel_number   = channel;
  StartReq.beacon_order     = 0x0F;
  StartReq.superframe_order = 0x0F;
  StartReq.PAN_coordinator  = g_TRUE;
  StartReq.battery_life_extension = g_FALSE;
    
  MacStatus = ST_MAC_MLMEStartReq(mac_hndl, &StartReq);
  if ( MAC_SUCCESS != MacStatus ) {
    APP_DBG("FFD MAC - Set Association Permit Fails\r\n");
    return;
  }
  /* Wait START CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_START_CNF);

  /* Set RxOnWhenIdle */
  APP_DBG("FFD MAC APP - Set RX On When Idle\r\n");
  memset(&SetReq,0x00,sizeof(ST_MAC_setReq_t));
  SetReq.PIB_attribute = g_MAC_RX_ON_WHEN_IDLE_c;
  PIB_Value = g_TRUE;
  SetReq.PIB_attribute_valuePtr = &PIB_Value;
    
  MacStatus = ST_MAC_MLMESetReq(mac_hndl, &SetReq );
  if ( MAC_SUCCESS != MacStatus ) {
    APP_DBG("FFD MAC - Set Rx On When Idle Fails\r\n");
    return;
  }
  /* Wait SET CONFIRMATION */
  UTIL_SEQ_WaitEvt(EVENT_SET_CNF);

  APP_DBG("FFD MAC APP - Ready to Handle Association Req and Receive Data\r\n");
  BSP_LED_On(LD1);

}