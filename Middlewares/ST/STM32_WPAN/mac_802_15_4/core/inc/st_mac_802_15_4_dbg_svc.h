/**
  ******************************************************************************
  * @file    st_mac_802_15_4_dbg_svc.h
  * @author  MCD Application Team
  * @brief   Contains STM32WB specificities requested to debug the 802.15.4
  *          MAC interface.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */


#ifndef _ST_MAC_802_15_4_DBG_H_
#define _ST_MAC_802_15_4_DBG_H_


#include <stdint.h>
#include <stdbool.h>

typedef struct {
  /** @brief   A counter that is incremented each time the MAC layer use the primitive mcps-data.Request.*/
  uint32_t macDataReqCounter;
  /** @brief   A counter that is incremented each time the MAC layer use the primitive mcps-data.Confirm.*/
  uint32_t macDataCnfCounter;
  /** @brief   A counter that is incremented each time the MAC layer use the primitive mcps-data.Indication.*/
  uint32_t macDataIndCounter;
  /** @brief   A counter that is incremented each time the MAC layer use the primitive mcps-purge.Request.*/
  uint32_t macPurgeReqCounter;
  /** @brief   A counter that is incremented each time the MAC layer use the primitive mcps-purge.Confirm.*/
  uint32_t macPurgeCnfCounter;
  /** @brief   A counter that is incremented each time the MAC layer use the primitive mlme-scan.Request.*/
  uint32_t macScanReqCounter;
  /** @brief   A counter that is incremented each time the MAC layer use the primitive mlme-scan.Confirm.*/
  uint32_t macScanCnfCounter;
  /** @brief   A counter that is incremented each time the MAC layer use the primitive mlme-associate.Request.*/
  uint32_t macAssociationReqCounter;
  /** @brief   A counter that is incremented each time the MAC layer use the primitive mlme-association.Confirm.*/
  uint32_t macAssociationCnfCounter;
  /** @brief    A counter that is incremented each time the MAC layer fails an association.*/
  uint32_t macAssociationCnfFail;
  /** @brief A counter that is incremented each time the MAC layer fails an association with the status=0xE9.*/
  uint32_t macAssociationCnfNoACK;
  /** @brief    A counter that is incremented each time the MAC layer fails an association with the status=0xE1.*/
  uint32_t macAssociationCnfCCAFailure;
  /** @brief    A counter that is incremented each time the MAC layer fails an association with the status=0xEB.*/
  uint32_t macAssociationCnfNoData;
  /** @brief   A counter that is incremented each time the MAC layer use the primitive mlme-association.Indication.*/
  uint32_t macAssociationIndCounter;
  /** @brief   A counter that is incremented each time the MAC layer use the primitive mlme-association.Response.*/
  uint32_t macAssociationResCounter;
  /** @brief   A counter that is incremented each time the MAC layer use the primitive mlme-poll.Request.*/
  uint32_t macPollReqCounter;
  /** @brief   A counter that is incremented each time the MAC layer use the primitive mlme-poll.Confirm.*/
  uint32_t macPollCnfCounter;
  /** @brief   A counter that is incremented each time the MAC layer use the primitive mlme-poll.Indication.*/
  uint32_t macPollIndCounter;
  /** @brief   A counter that is incremented each time the MAC layer use the primitive mlme-beaconNotify.Indication.*/
  uint32_t macBeaconNotifyIndCounter;
  /** @brief   A counter that is incremented each time the MAC layer use the primitive mlme-Disassociate.Request.*/
  uint32_t macDisassociationReqCounter;
  /** @brief   A counter that is incremented each time the MAC layer use the primitive mlme-Disassociation.Confirm.*/
  uint32_t macDisassociationCnfCounter;
  /** @brief   A counter that is incremented each time the MAC layer use the primitive mlme-Disassociation.Indication.*/
  uint32_t macDisassociationIndCounter;
  /** @brief A counter that is incremented each time the MAC layer fails to send a unicast with the status=0xE9.*/
  uint32_t macDataCnfNoACK;
  /** @brief    A counter that is incremented each time the MAC layer fails to send a unicast with the status=0xE1.*/
  uint32_t macDataCnfCCAFailure;
  /** @brief    A counter that is incremented each time the MAC layer fails to send a unicast with the status=0xF1.*/
  uint32_t macDataCnfTransactionOverFlow;
  /** @brief    A counter that is incremented each time the MAC layer fails to send a unicast.*/
  uint32_t macDataCnfFail;
  /** @brief    This variable allows you to save the last fail status for Data.cnf.*/
  uint8_t macDataCnfLastFail;
  /** @brief    This variable allows you to save the last status for Data.cnf.*/
  uint8_t macDataCnfLastStatus;
  /** @brief    A counter that is incremented each time the Queue fails*/
  uint32_t macQueueFail;
  /** @brief    A counter that is incremented each time the Enqueue fails*/
  uint32_t macEnqueueFail;
  /** @brief    A counter that is incremented each time no buffer is allocated*/
  uint32_t macQueueNoBuffer;
  /** @brief    A counter that is incremented each time the buffer allocate fail*/
  uint32_t macQueueBufferAllocateFailure;
} mac_ext_diagnostics_t;


/** diagnostics function */
void ST_init_mac_diagnostics(void);
void ST_get_mac_diagnostics(mac_ext_diagnostics_t * mac_ext_diagnostics);

/** MCPS function */
void set_mac_ext_macDataReqCounterHandler(void);
uint32_t get_mac_ext_macDataReqCounterHandler(void);
void set_mac_ext_macDataCnfCounterHandler(void);
uint32_t get_mac_ext_macDataCnfCounterHandler(void);
void set_mac_ext_macDataIndCounterHandler(void);
uint32_t get_mac_ext_macDataIndCounterHandler(void);

void set_mac_ext_macPurgeReqCounterHandler(void);
uint32_t get_mac_ext_macPurgeReqCounterHandler(void);
void set_mac_ext_macPurgeCnfCounterHandler(void);
uint32_t get_mac_ext_macPurgeCnfCounterHandler(void);

/** MLME function */
void set_mac_ext_macScanReqCounterHandler(void);
uint32_t get_mac_ext_macScanReqCounterHandler(void);
void set_mac_ext_macScanCnfCounterHandler(void);
uint32_t get_mac_ext_macScanCnfCounterHandler(void);

void set_mac_ext_macAssociationReqCounterHandler(void);
uint32_t get_mac_ext_macAssociationReqCounterHandler(void);
void set_mac_ext_macAssociationCnfCounterHandler(void);
uint32_t get_mac_ext_macAssociationCnfCounterHandler(void);

void set_mac_ext_macAssociationIndCounterHandler(void);
uint32_t get_mac_ext_macAssociationIndCounterHandler(void);
void set_mac_ext_macAssociationResCounterHandler(void);
uint32_t get_mac_ext_macAssociationResCounterHandler(void);

void set_mac_ext_macPollReqCounterHandler(void);
uint32_t get_mac_ext_macPollReqCounterHandler(void);
void set_mac_ext_macPollCnfCounterHandler(void);
uint32_t get_mac_ext_macPollCnfCounterHandler(void);
void set_mac_ext_macPollIndCounterHandler(void);
uint32_t get_mac_ext_macPollIndCounterHandler(void);

void set_mac_ext_macBeaconNotifyIndCounterHandler(void);
uint32_t get_mac_ext_macBeaconNotifyIndCounterHandler(void);

void set_mac_ext_macDisassociationReqCounterHandler(void);
uint32_t get_mac_ext_macDisassociationReqCounterHandler(void);
void set_mac_ext_macDisassociationCnfCounterHandler(void);
uint32_t get_mac_ext_macDisassociationCnfCounterHandler(void);
void set_mac_ext_macDisassociationIndCounterHandler(void);
uint32_t get_mac_ext_macDisassociationIndCounterHandler(void);

/** Associate.Confirm Fails*/
void set_mac_ext_macAssociationCnfFailCounterHandler(void);
uint32_t get_mac_ext_macAssociationCnfFailCounterHandler(void);
void set_mac_ext_macAssociationCnfNoACKCounterHandler(void);
uint32_t get_mac_ext_macAssociationCnfNoACKCounterHandler(void);
void set_mac_ext_macAssociationCnfCCAFailureCounterHandler(void);
uint32_t get_mac_ext_macAssociationCnfCCAFailureCounterHandler(void);
void set_mac_ext_macAssociationCnfNoDataCounterHandler(void);
uint32_t get_mac_ext_macAssociationCnfNoDataCounterHandler(void);

/** Data.Confirm Fails*/
void set_mac_ext_macDataCnfNoACKCounterHandler(void);
uint32_t get_mac_ext_macDataCnfNoACKCounterHandler(void);
void set_mac_ext_macDataCnfCCAFailureCounterHandler(void);
uint32_t get_mac_ext_macDataCnfCCAFailureCounterHandler(void);
void set_mac_ext_macDataCnfTransactionOverFlowCounterHandler(void);
uint32_t get_mac_ext_macDataCnfTransactionOverFlowCounterHandler(void);
void set_mac_ext_macDataCnfFailCounterHandler(void);
uint32_t get_mac_ext_macDataCnfFailCounterHandler(void);
void set_mac_ext_macDataCnfLastFailHandler(uint8_t status);
uint8_t get_mac_ext_macDataCnfLastFailHandler(void);
void set_mac_ext_macDataCnfLastStatusHandler(uint8_t status);
uint8_t get_mac_ext_macDataCnfLastStatusHandler(void);

/** Queue function */
void set_mac_ext_macQueueFailHandler(void);
uint8_t get_mac_ext_macQueueFailHandler(void);
void set_mac_ext_macEnqueueFailHandler(void);
uint8_t get_mac_ext_macEnqueueFailHandler(void);
void set_mac_ext_macQueueNoBufferHandler(void);
uint8_t get_mac_ext_macQueueNoBufferHandler(void);
void set_mac_ext_macQueueBufferAllocateFailureHandler(void);
uint8_t get_mac_ext_macQueueBufferAllocateFailureHandler(void);

#endif /* _ST_MAC_802_15_4_DBG_H_ */
