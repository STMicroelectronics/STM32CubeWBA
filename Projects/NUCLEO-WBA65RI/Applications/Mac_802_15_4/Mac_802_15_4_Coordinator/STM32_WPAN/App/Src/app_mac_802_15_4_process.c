/**
  ******************************************************************************
  * @file 	 app_mac_802-15-4_process.c
  * @author  MDG-RF Team
  * @brief	 ST MAC Wrapper Process / Handle incoming MAC request 
  *          from upper layer / client MAC service.
  *
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
#include "stm32_lpm.h"
#include "stm32wbaxx_hal.h"
#include "st_mac_802_15_4_sap.h"
#include "app_conf.h"
#include "app_mac_802_15_4_process.h"
#include "stm32_seq.h"
#include "stm32_timer.h"
#include "mac_host_intf.h"
#include "log_module.h"

/* Global define -------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/
extern MAC_handle mac_hndl;
extern uint8_t g_channel; // Channel select with ED scan
extern uint16_t shortAssociationAddrList[MAX_DEVICES_ASSO]; // Table with all devices short address
extern uint8_t idAssociationList; // Number of devices associated
extern uint8_t indDeviceSendData; // Which device to send data
extern uint8_t status_check_device; // Return device status when BT2 is pushed
extern UTIL_TIMER_Object_t LEDTimer; // LED timer
extern uint16_t tab_panId_already_use[MAX_TAB_PAN_ID]; // Table with all PAN ID already use in the local environment
/* Private defines -----------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static uint8_t checkMsgXorSignature(const char * pMessage, uint8_t message_len, uint8_t sign, uint8_t expectedRes);

/* Public variables ----------------------------------------------------------*/
ST_MAC_associateInd_t g_MAC_associateInd; // Use to association device

/* Private function Definition -----------------------------------------------*/
static uint8_t checkMsgXorSignature(const char * pMessage, uint8_t message_len, uint8_t sign, uint8_t expectedRes)
{
  uint8_t seed = sign;
  for (uint8_t i=0x00;i<message_len;i++)
    seed = (uint8_t)pMessage[i]^seed;
  return (seed==expectedRes);
}

/*! callbacks to be called by the MAC layer */
MAC_Status_t APP_MAC_mlmeAssociateCnfCb( const  ST_MAC_associateCnf_t * pAssociateCnf )
{
  UTIL_SEQ_SetEvt(EVENT_ASSOCIATE_CNF);
  /* return */
  return MAC_SUCCESS;
}


MAC_Status_t APP_MAC_mlmeAssociateIndCb( const  ST_MAC_associateInd_t * pAssociateInd )
{
  /* Store ASSOCIATION IND */
  MAC_Status_t MacStatus = MAC_ERROR;
  ST_MAC_associateRes_t AssociateRes;
  uint8_t address_selected = 0;
  uint16_t shortAssociationAddr = COORD_ADDRESS; // Reset this variable each time we have association
  
  /* Selected association address in a range of FIRST_ASSOCIATED_SHORT_ADDRESS to FIRST_ASSOCIATED_SHORT_ADDRESS + MAX_DEVICES_ASSO*/
  for (uint8_t indiceAsso=0;indiceAsso<MAX_DEVICES_ASSO && shortAssociationAddr==COORD_ADDRESS ;indiceAsso++) {
      for (uint8_t i=0;i<idAssociationList;i++) {
        if (shortAssociationAddrList[i] == (FIRST_ASSOCIATED_SHORT_ADDRESS + indiceAsso)) {
         address_selected = 1;
        } 
    }
    if (!address_selected) {
      shortAssociationAddr = FIRST_ASSOCIATED_SHORT_ADDRESS + indiceAsso;//this value is selected
    }
    address_selected = 0;
  }

  /* If any address is free */
  if (shortAssociationAddr == 0x2000) {
    APP_DBG("COORD MAC - Association Response Fails max device associated\r\n");
    UTIL_SEQ_SetTask(1U<<CFG_TASK_SEND_DATA, TASK_MAC_APP_PRIO);
    return MacStatus;
  }
  
  shortAssociationAddrList[idAssociationList] = shortAssociationAddr; // Add in tab the address of the new device
  idAssociationList++; // Increase number of device

  memcpy(&g_MAC_associateInd, pAssociateInd, sizeof(ST_MAC_associateInd_t));

  /* structure initialization */
  memset(&AssociateRes, 0x00, sizeof(ST_MAC_associateRes_t));
  
  memcpy(AssociateRes.a_device_address, g_MAC_associateInd.a_device_address, 0x08);
  memcpy(AssociateRes.a_assoc_short_address, &shortAssociationAddr, 0x02);
  AssociateRes.security_level = g_MAC_UNSECURED_MODE_c; // Unsecure frame
  AssociateRes.status = MAC_SUCCESS;

  MacStatus = ST_MAC_MLMEAssociateRes(mac_hndl, &AssociateRes);
  if (MAC_SUCCESS != MacStatus) {
   APP_DBG("COORD MAC - Association Response Fails\r\n");
  }
  BSP_LED_On(LD2);
  UTIL_TIMER_Start(&LEDTimer);
  APP_DBG("COORD MAC APP - Association done and the short address given 0x%04x\r\n", shortAssociationAddr);
  /* return */
  return MAC_SUCCESS;

}

MAC_Status_t APP_MAC_mlmeBeaconNotifyIndCb( const  ST_MAC_beaconNotifyInd_t * pBeaconNotifyInd )
{
  uint16_t BeaconPanId = 0x0000; 
  static uint8_t indice_tab = 0;
  uint8_t tmp_pan_id_use = 0;
    
  BeaconPanId = ((pBeaconNotifyInd->PAN_descriptor.a_coord_PAN_id[1])<<8) + pBeaconNotifyInd->PAN_descriptor.a_coord_PAN_id[0];
  APP_DBG("COORD MAC - Receive BeaconNotify.Indication with PANID: 0x%04x\r\n", BeaconPanId);
  
  /* Check if the pan id is not in the tab */
  for (uint8_t i=0;i<MAX_TAB_PAN_ID;i++) {
    if (tab_panId_already_use[i] == BeaconPanId) {
      tmp_pan_id_use = 1;
    }
  }
  if (!tmp_pan_id_use && indice_tab <= MAX_TAB_PAN_ID) {
    tab_panId_already_use[indice_tab++] = BeaconPanId;
  }
  /* return */
  return MAC_SUCCESS;

}

MAC_Status_t APP_MAC_mlmeCommStatusIndCb( const  ST_MAC_commStatusInd_t * pCommStatusInd )
{
  if (pCommStatusInd->status == MAC_SUCCESS) {
    if (pCommStatusInd->dst_addr_mode == g_SHORT_ADDR_MODE_c) {
      APP_DBG("COORD MAC - Association succeed for 0x%02x%02x\r\n", pCommStatusInd->dst_address.a_short_addr[0], pCommStatusInd->dst_address.a_short_addr[1]);
    }else {
      if (pCommStatusInd->dst_addr_mode == g_EXTENDED_ADDR_MODE_c) {
        APP_DBG("COORD MAC - Association succeed for 0x%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\r\n", pCommStatusInd->dst_address.a_extend_addr[7], 
                pCommStatusInd->dst_address.a_extend_addr[6],pCommStatusInd->dst_address.a_extend_addr[5],pCommStatusInd->dst_address.a_extend_addr[4],
                pCommStatusInd->dst_address.a_extend_addr[3],pCommStatusInd->dst_address.a_extend_addr[2],pCommStatusInd->dst_address.a_extend_addr[1],
                pCommStatusInd->dst_address.a_extend_addr[0]);
      }
    }
  }
  /* return */
  return MAC_SUCCESS;

}


MAC_Status_t APP_MAC_mlmeDisassociateCnfCb( const  ST_MAC_disassociateCnf_t * pDisassociateCnf )
{
  UTIL_SEQ_SetEvt(EVENT_DISASSOCIATE_CNF);
  APP_DBG("COORD MAC APP - Disassociate CNF RECEIVED\r\n");
  /* return */
  return MAC_SUCCESS;

}


MAC_Status_t APP_MAC_mlmeDisassociateIndCb( const  ST_MAC_disassociateInd_t * pDisassociateInd )
{
  APP_DBG("COORD MAC APP - Disassociate IND RECEIVED by 0x%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\r\n", pDisassociateInd->a_device_address[7],
            pDisassociateInd->a_device_address[6],pDisassociateInd->a_device_address[5],pDisassociateInd->a_device_address[4],pDisassociateInd->a_device_address[3],
            pDisassociateInd->a_device_address[2],pDisassociateInd->a_device_address[1],pDisassociateInd->a_device_address[0]);
  /* return */
  return MAC_SUCCESS;

}


MAC_Status_t APP_MAC_mlmeGetCnfCb( const  ST_MAC_getCnf_t * pGetCnf )
{
  UTIL_SEQ_SetEvt(EVENT_GET_CNF);
  /* return */
  return MAC_SUCCESS;

}


MAC_Status_t APP_MAC_mlmeOrphanIndCb( const  ST_MAC_orphanInd_t * pOrphanInd )
{
  APP_DBG("COORD MAC APP - Orphan IND RECEIVED\r\n");
  /* return */
  return MAC_SUCCESS;

}


MAC_Status_t APP_MAC_mlmePollCnfCb( const  ST_MAC_pollCnf_t * pPollCnf )
{
  UTIL_SEQ_SetEvt(EVENT_POLL_CNF);
  /* return */
  return MAC_SUCCESS;

}


MAC_Status_t APP_MAC_mlmeResetCnfCb( const  ST_MAC_resetCnf_t * pResetCnf )
{
  UTIL_SEQ_SetEvt(EVENT_RESET_CNF);
  /* return */
  return MAC_SUCCESS;

}


MAC_Status_t APP_MAC_mlmeRxEnableCnfCb( const  ST_MAC_rxEnableCnf_t * pRxEnableCnf )
{
  UTIL_SEQ_SetEvt(EVENT_RX_ON_WHEN_IDLE_CNF);
  /* return */
  return MAC_SUCCESS;

}

MAC_Status_t APP_MAC_mlmeScanCnfCb( const  ST_MAC_scanCnf_t * pScanCnf )
{
  uint8_t tmp_energy = 0xFF;
  uint8_t i = 0;
  /* Select the best channel */
  if(pScanCnf->status == MAC_SUCCESS) {
    if (pScanCnf->scan_type == 0x00) {// ED scan 
      while(i < g_MAX_ED_SCAN_RESULTS_SUPPORTED_c) { 
        /* Select the least noisy channel */
        if (pScanCnf->a_energy_detect_list[i] < tmp_energy) {
          //APP_DBG("FFD MAC APP - channel %d : %x\r\n", i+11, pScanCnf->a_energy_detect_list[i]);
          g_channel = 11 + i;
          tmp_energy = pScanCnf->a_energy_detect_list[i];
        }
        i++;
      }
    APP_DBG("COORD MAC APP - Channel selected %d\r\n", g_channel);
    }
  }
  UTIL_SEQ_SetEvt(EVENT_SCAN_CNF);
  /* return */
  return MAC_SUCCESS;
}


MAC_Status_t APP_MAC_mlmeSetCnfCb( const  ST_MAC_setCnf_t * pSetCnf )
{
  UTIL_SEQ_SetEvt(EVENT_SET_CNF);
  /* return */
  return MAC_SUCCESS;
}


MAC_Status_t APP_MAC_mlmeStartCnfCb( const  ST_MAC_startCnf_t * pStartCnf )
{
  UTIL_SEQ_SetEvt(EVENT_START_CNF);
  /* return */
  return MAC_SUCCESS;

}


MAC_Status_t APP_MAC_mcpsDataIndCb( const  ST_MAC_dataInd_t * pDataInd )
{
  uint8_t data_payload[127];
  // Check validity of the received Message extracting associated 
  // simple xor signature
  if (!checkMsgXorSignature((char const *)(pDataInd->msdu),
                           pDataInd->msdu_length-1,
                           pDataInd->msdu[pDataInd->msdu_length-1],
                           0x00))
  {
    APP_DBG("COORD MAC APP - ERROR : CORRUPTED RECEIVED DATA\r\n");
  }
  else{
    // Copy payload message
    memcpy(&data_payload,pDataInd->msdu,(pDataInd->msdu_length)-1);
    // Add \0 at the end of the message
    data_payload[(pDataInd->msdu_length)-1] ='\0';
    APP_DBG("COORD MAC APP - DATA RECEIVED %s \r\n", data_payload);
    BSP_LED_On(LD3);
    UTIL_TIMER_Start(&LEDTimer);
  }
  /* return */
  return MAC_SUCCESS;
}


MAC_Status_t APP_MAC_mcpsDataCnfCb( const  ST_MAC_dataCnf_t * pDataCnf )
{
  UTIL_SEQ_SetEvt(EVENT_DATA_CNF);
  UTIL_SEQ_SetTask(1U << CFG_TASK_SEND_DATA, TASK_MAC_APP_PRIO);
  
  status_check_device = pDataCnf->status;
  if (pDataCnf->status != MAC_STATUS_SUCCESS) {
    /* The device is disconnected */
    uint16_t tmp_shortAssociationAddrList[MAX_DEVICES_ASSO]; // Create temporary tab
    memset(&tmp_shortAssociationAddrList, 0x00, idAssociationList*2);// Add 0 in the tab
    memcpy(&tmp_shortAssociationAddrList, &shortAssociationAddrList, idAssociationList*2); // Copy table
    uint8_t indiceAsso = 0;
    for (uint8_t i=0;i<idAssociationList;i++) {
      if (indDeviceSendData != i) {
        shortAssociationAddrList[indiceAsso] = tmp_shortAssociationAddrList[i];
        indiceAsso++;
      }
    }
    /* Element to remove -> shortAssociationAddrList[indDeviceSendData] */
    idAssociationList--;
  }

  /* return */
  return MAC_SUCCESS;

}


MAC_Status_t APP_MAC_mcpsPurgeCnfCb( const  ST_MAC_purgeCnf_t * pPurgeCnf )
{
  UTIL_SEQ_SetEvt(EVENT_PURGE_CNF);
  /* return */
  return MAC_SUCCESS;

}


MAC_Status_t  APP_MAC_mlmePollIndCb ( const  ST_MAC_pollInd_t * pPollInd )
{
  if (pPollInd->addr_mode == g_SHORT_ADDR_MODE_c) {// Display only short address
    APP_DBG("COORD MAC APP - POLL IND RECEIVED by 0x%02x%02x\r\n", pPollInd->request_address.a_short_addr[1], pPollInd->request_address.a_short_addr[0]);
  }
  /* return */
  return MAC_SUCCESS;

}


MAC_Status_t APP_MAC_mlmeSyncLossIndCb( const ST_MAC_syncLoss_t * syncLossPtr )
{
  /* return */
  return MAC_SUCCESS;
}


MAC_Status_t APP_MAC_mlmeBeaconReqIndCb( const  ST_MAC_beaconReqInd_t * pBeaconReqInd )
{
  /* return */
  return MAC_SUCCESS;
}


MAC_Status_t APP_MAC_mlmeBeaconCnfCb( const ST_MAC_beaconCnf_t * pBeaconCnf )
{
  UTIL_SEQ_SetEvt(EVENT_BEACON_CNF);
  /* return */
  return MAC_SUCCESS;
}


MAC_Status_t APP_MAC_mlmeGetPwrInfoTableCnfCb( const ST_MAC_getPwrInfoTableCnf_t * pGetPwrInfoTableCnf )
{
  UTIL_SEQ_SetEvt(EVENT_GET_PWR_INFO_TABLE_CNF);
  /* return */
  return MAC_SUCCESS;
}


MAC_Status_t APP_MAC_mlmeSetPwrInfoTableCnfCb( const ST_MAC_setPwrInfoTableCnf_t * pSetPwrInfoTableCnf )
{
  UTIL_SEQ_SetEvt(EVENT_SET_PWR_INFO_TABLE_CNF);
  /* return */
  return MAC_SUCCESS;
}

/*! NOT YET IMPLEMENTED */
MAC_Status_t APP_MAC_mlmeCalibrateCnfCb( const ST_MAC_calibrateCnf_t * pCallibrateCnf)
{
  return MAC_NOT_IMPLEMENTED_STATUS;
}


MAC_Status_t APP_MAC_mlmeDpsCnfCb( const ST_MAC_dpsCnf_t * pDpsCnf  )
{
  return MAC_NOT_IMPLEMENTED_STATUS;
}


MAC_Status_t APP_MAC_mlmeDpsIndCb( const ST_MAC_dpsInd_t * pDpsInd )
{
  return MAC_NOT_IMPLEMENTED_STATUS;
}


MAC_Status_t APP_MAC_mlmeSoundingCnfCb( const ST_MAC_soundingCnf_t * pSoudingCnf)
{
  return MAC_NOT_IMPLEMENTED_STATUS;
}


MAC_Status_t APP_MAC_mlmeGtsCnfCb( const ST_MAC_gtsCnf_t * pGtsCnf)
{
  return MAC_NOT_IMPLEMENTED_STATUS;
}


MAC_Status_t APP_MAC_mlmeGtsIndCb( const ST_MAC_GtsInd_t * pGtsInd )
{
  return MAC_NOT_IMPLEMENTED_STATUS;
}
