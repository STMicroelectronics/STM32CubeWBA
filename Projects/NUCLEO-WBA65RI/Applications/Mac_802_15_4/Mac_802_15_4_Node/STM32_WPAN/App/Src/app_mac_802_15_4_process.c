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
#include "app_mac.h"
#include "app_conf.h"
#include "app_mac_802_15_4_process.h"
#include "stm32_seq.h"
#include "log_module.h"

/* Global define -------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/
extern ST_MAC_associateCnf_t g_MAC_associateCnf;
extern uint8_t g_channel;
extern ST_MAC_beaconNotifyInd_t  g_BeaconNotifyInd;
extern uint8_t deviceAssociated;
extern uint8_t beacon_coord_received;

/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static uint8_t checkMsgXorSignature(const char * pMessage, uint8_t message_len, uint8_t sign, uint8_t expectedRes);

/* Public variables ----------------------------------------------------------*/

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
  if (pAssociateCnf->status == 0x00) { // If success
    APP_DBG("Node MAC APP - Association CNF Received, Short address is 0x%02x%02x on channel %d\n\r",
            pAssociateCnf->a_assoc_short_address[1],pAssociateCnf->a_assoc_short_address[0],g_channel);
    
    deviceAssociated = 1; // This device is associated now
    
    BSP_LED_On(LD2);
  } else {
    APP_DBG("Node MAC APP - Association CNF Received with bad status: 0x%02x\n\r", pAssociateCnf->status);
  }

  memcpy(&g_MAC_associateCnf, pAssociateCnf, sizeof(ST_MAC_associateCnf_t)); // Copy table

  /* return */
  return MAC_SUCCESS;
}


MAC_Status_t APP_MAC_mlmeAssociateIndCb( const  ST_MAC_associateInd_t * pAssociateInd )
{
  /* return */
  return MAC_SUCCESS;

}


MAC_Status_t APP_MAC_mlmeBeaconNotifyIndCb( const  ST_MAC_beaconNotifyInd_t * pBeaconNotifyInd )
{
  uint16_t BeaconPanId = 0x0000; 
  uint8_t Beacon_payload[BEACON_PAYLOAD_SIZE];

  BeaconPanId = ((pBeaconNotifyInd->PAN_descriptor.a_coord_PAN_id[1])<<8) + pBeaconNotifyInd->PAN_descriptor.a_coord_PAN_id[0];
  if (pBeaconNotifyInd->sdu_length == BEACON_PAYLOAD_SIZE) { // First filter with the size
    memcpy(&Beacon_payload, pBeaconNotifyInd->sduPtr, pBeaconNotifyInd->sdu_length); // Getting the beacon payload
    if (strncmp(BEACON_PAYLOAD, (char const *) &Beacon_payload, BEACON_PAYLOAD_SIZE) == 0) { // Second filter with the payload
      APP_DBG("Node MAC - Receive BeaconNotify.Ind with the correct payload, PANID: 0x%04x, channel: %d\r\n", BeaconPanId, pBeaconNotifyInd->PAN_descriptor.logical_channel);
      memcpy(&g_BeaconNotifyInd, pBeaconNotifyInd, sizeof(ST_MAC_beaconNotifyInd_t)); // Copy table
      beacon_coord_received  = 0x01;
      return MAC_SUCCESS;
    }
  }
  APP_DBG("Node MAC - Receive BeaconNotify.Ind with the incorrect payload, PANID: 0x%04x, channel: %d\r\n", BeaconPanId, pBeaconNotifyInd->PAN_descriptor.logical_channel);
  
  /* return */
  return MAC_SUCCESS;

}


MAC_Status_t APP_MAC_mlmeCommStatusIndCb( const  ST_MAC_commStatusInd_t * pCommStatusInd )
{
  /* return */
  return MAC_SUCCESS;

}


MAC_Status_t APP_MAC_mlmeDisassociateCnfCb( const  ST_MAC_disassociateCnf_t * pDisassociateCnf )
{
  UTIL_SEQ_SetEvt(EVENT_DISASSOCIATE_CNF);
  APP_DBG("Node MAC APP - Disassociate CNF Received\r\n");
  deviceAssociated = 0; // This device is disassociated now
  /* return */
  return MAC_SUCCESS;

}


MAC_Status_t APP_MAC_mlmeDisassociateIndCb( const  ST_MAC_disassociateInd_t * pDisassociateInd )
{
  APP_DBG("Node MAC APP - Disassociate.Ind RECEIVED by 0x%02x%02x\r\n", pDisassociateInd->a_device_address[1],pDisassociateInd->a_device_address[0]);
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
  APP_DBG("Node MAC APP - Scan CNF Received\r\n");
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
    APP_DBG("Node MAC APP - ERROR : CORRUPTED RECEIVED DATA\r\n");
  }
  else{
    // Copy payload message
    memcpy(&data_payload, pDataInd->msdu, (pDataInd->msdu_length)-1);
    // Add \0 at the end of the message
    data_payload[(pDataInd->msdu_length)-1] ='\0';
    APP_DBG("Node MAC APP - DATA RECEIVED: %s \r\n", data_payload);
  }
  /* return */
  return MAC_SUCCESS;
}


MAC_Status_t APP_MAC_mcpsDataCnfCb( const  ST_MAC_dataCnf_t * pDataCnf )
{
  UTIL_SEQ_SetEvt(EVENT_DATA_CNF);
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
