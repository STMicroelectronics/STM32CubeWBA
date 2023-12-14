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
#include "mac_stack_configuration.h"
#include "st_mac_802_15_4_sap.h"
#include "app_conf.h"
#include "app_mac_802_15_4_process.h"
#include "stm32_seq.h"


/* Global define -------------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
extern ST_MAC_associateInd_t g_MAC_associateInd;
extern MAC_handle mac_hndl;
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static uint8_t checkMsgXorSignature(const char * pMessage, uint8_t message_len, uint8_t sign, uint8_t expectedRes);

/* Public variables ----------------------------------------------------------*/
/* Private function  ---------------------------------------------------------*/

static uint8_t checkMsgXorSignature(const char * pMessage, uint8_t message_len, uint8_t sign, uint8_t expectedRes)
{
  uint8_t seed = sign;
  for (uint8_t i=0x00;i<message_len;i++)
    seed = (uint8_t)pMessage[i]^seed;
  return (seed==expectedRes);
}

/* callbacks to be called by the MAC layer */
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

  APP_DBG("FFD MAC APP - Association Requested\r\n");

  uint16_t shortAssociationAddr = 0x3344;

  memcpy(&g_MAC_associateInd,pAssociateInd,sizeof(ST_MAC_associateInd_t));

  memcpy(AssociateRes.a_device_address,g_MAC_associateInd.a_device_address,0x08);
  memcpy(AssociateRes.a_assoc_short_address,&shortAssociationAddr,0x02);
  AssociateRes.security_level = 0x00;
  AssociateRes.status = MAC_SUCCESS;

  MacStatus = ST_MAC_MLMEAssociateRes(mac_hndl, &AssociateRes);
  if ( MAC_SUCCESS != MacStatus ) {
   APP_DBG("FFD MAC - Association Response Fails\r\n");
  }
  HAL_Delay(200);
  BSP_LED_Off(LD2);
  /* return */
  return MAC_SUCCESS;

}


MAC_Status_t APP_MAC_mlmeBeaconNotifyIndCb( const  ST_MAC_beaconNotifyInd_t * pBeaconNotifyInd )
{
  /* return */
  return MAC_SUCCESS;

}


MAC_Status_t APP_MAC_mlmeCommStatusIndCb( const  ST_MAC_commStatusInd_t * pCommStatusInd )
{
  if (pCommStatusInd->status == MAC_SUCCESS){
    APP_DBG("FFD MAC - Association succeed\r\n");
  }
  /* return */
  return MAC_SUCCESS;

}


MAC_Status_t APP_MAC_mlmeDisassociateCnfCb( const  ST_MAC_disassociateCnf_t * pDisassociateCnf )
{
  UTIL_SEQ_SetEvt(EVENT_DISASSOCIATE_CNF);
  /* return */
  return MAC_SUCCESS;

}


MAC_Status_t APP_MAC_mlmeDisassociateIndCb( const  ST_MAC_disassociateInd_t * pDisassociateInd )
{
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
    APP_DBG("FFD MAC APP - ERROR : CORRUPTED RECEIVED DATA ");
  }
  else{
    // Copy payload message
    memcpy(&data_payload,pDataInd->msdu,(pDataInd->msdu_length)-1);
    // Add \0 at the end of the message
    data_payload[(pDataInd->msdu_length)-1] ='\0';
    APP_DBG("FFD MAC APP - DATA RECEIVED: %s \r\n", data_payload);
    BSP_LED_On(LD3);
    HAL_Delay(300);
    BSP_LED_Off(LD3); 
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

