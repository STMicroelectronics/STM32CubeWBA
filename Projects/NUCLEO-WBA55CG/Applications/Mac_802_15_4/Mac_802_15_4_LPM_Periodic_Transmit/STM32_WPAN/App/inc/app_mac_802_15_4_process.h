/**
  ******************************************************************************
  * @file    app_mac_802_15_4_process.h
  * @author  MCD Application Team
  * @brief   Header for INTERNAL Certification MAC 802.15.4 processing activtities
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_CERTIFICATION_MAC_802_15_4_PROCESS_H
#define APP_CERTIFICATION_MAC_802_15_4_PROCESS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "st_mac_802_15_4_types.h"


MAC_Status_t APP_MAC_mlmeAssociateCnfCb( const  ST_MAC_associateCnf_t * pAssociateCnf );
MAC_Status_t APP_MAC_mlmeAssociateIndCb( const  ST_MAC_associateInd_t * pAssociateInd );
MAC_Status_t APP_MAC_mlmeBeaconNotifyIndCb( const  ST_MAC_beaconNotifyInd_t * pBeaconNotifyInd );
MAC_Status_t APP_MAC_mlmeCommStatusIndCb( const  ST_MAC_commStatusInd_t * pCommStatusInd );
MAC_Status_t APP_MAC_mlmeDisassociateCnfCb( const  ST_MAC_disassociateCnf_t * pDisassociateCnf );
MAC_Status_t APP_MAC_mlmeDisassociateIndCb( const  ST_MAC_disassociateInd_t * pDisassociateInd );
MAC_Status_t APP_MAC_mlmeGetCnfCb( const  ST_MAC_getCnf_t * pGetCnf );
MAC_Status_t APP_MAC_mlmeOrphanIndCb( const  ST_MAC_orphanInd_t * pOrphanInd );
MAC_Status_t APP_MAC_mlmePollCnfCb( const  ST_MAC_pollCnf_t * pPollCnf );
MAC_Status_t APP_MAC_mlmeResetCnfCb( const  ST_MAC_resetCnf_t * pResetCnf );
MAC_Status_t APP_MAC_mlmeRxEnableCnfCb( const  ST_MAC_rxEnableCnf_t * pRxEnableCnf );
MAC_Status_t APP_MAC_mlmeScanCnfCb( const  ST_MAC_scanCnf_t * pScanCnf );
MAC_Status_t APP_MAC_mlmeSetCnfCb( const  ST_MAC_setCnf_t * pSetCnf );
MAC_Status_t APP_MAC_mlmeStartCnfCb( const  ST_MAC_startCnf_t * pStartCnf );
MAC_Status_t APP_MAC_mcpsDataIndCb( const  ST_MAC_dataInd_t * pDataInd );
MAC_Status_t APP_MAC_mcpsDataCnfCb( const  ST_MAC_dataCnf_t * pDataCnf );
MAC_Status_t APP_MAC_mcpsPurgeCnfCb( const  ST_MAC_purgeCnf_t * pPurgeCnf );
MAC_Status_t APP_MAC_mlmePollIndCb( const  ST_MAC_pollInd_t * pPollInd );
MAC_Status_t APP_MAC_mlmeBeaconReqIndCb( const  ST_MAC_beaconReqInd_t * pBeaconReqInd );
MAC_Status_t APP_MAC_mlmeBeaconCnfCb( const  ST_MAC_beaconCnf_t * pBeaconCnf );
MAC_Status_t APP_MAC_mlmeGetPwrInfoTableCnfCb( const ST_MAC_getPwrInfoTableCnf_t * pGetPwrInfoTableCnf );
MAC_Status_t APP_MAC_mlmeSetPwrInfoTableCnfCb( const ST_MAC_setPwrInfoTableCnf_t * pSetPwrInfoTableCnf );

/* Not yet implemented */
MAC_Status_t APP_MAC_mlmeSyncLossIndCb( const ST_MAC_syncLoss_t * syncLossPtr );
MAC_Status_t APP_MAC_mlmeCalibrateCnfCb( const ST_MAC_calibrateCnf_t * pCallibrateCnf);
MAC_Status_t APP_MAC_mlmeDpsCnfCb( const ST_MAC_dpsCnf_t * pDpsCnf  );
MAC_Status_t APP_MAC_mlmeDpsIndCb( const ST_MAC_dpsInd_t * pDpsInd );
MAC_Status_t APP_MAC_mlmeSoundingCnfCb( const ST_MAC_soundingCnf_t * pSoudingCnf);
MAC_Status_t APP_MAC_mlmeGtsCnfCb( const ST_MAC_gtsCnf_t * pGtsCnf);
MAC_Status_t APP_MAC_mlmeGtsIndCb( const ST_MAC_GtsInd_t * pGtsInd );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* APP_CERTIFICATION_MAC_802_15_4_PROCESS_H */
