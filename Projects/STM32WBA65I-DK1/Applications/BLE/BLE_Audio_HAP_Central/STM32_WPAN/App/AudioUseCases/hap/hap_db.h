/**
  ******************************************************************************
  * @file    tmap_db.h
  * @author  MCD Application Team
  * @brief   This file contains definitions used for HAP Profile
  *          Database.
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
#ifndef __HAP_DB_H
#define __HAP_DB_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "hap_iac.h"
#include "usecase_dev_mgmt.h"

/* Functions ---------------------------------------------------------------------*/
/**
  * @brief Store every HAP services related to a given connection for GATT servers and clients
  * @param ConnHandle: Handle of the connection
  * @param Peer_Address_Type: Peer Address type
  * @param Peer_Address: Peer Address
  */
void HAP_DB_StoreServices(uint16_t ConnHandle,uint8_t Peer_Address_Type,const uint8_t Peer_Address[6]);

/**
  * @brief On a GATT Server, check changes made to Hearing aids presets since the last connection of the given
  *        remote device
  * @param pBleConnInfo: A pointer to the connection info structure
  * @retval status of the operation
  */
tBleStatus HAP_DB_CheckDatabaseChange(const UseCaseConnInfo_t *pBleConnInfo);

/**
   * @brief  Restore GATT Database of the HAP in HARC Role
   * @param  pHAP_Client: pointer on HAP Client Instance
   * @retval status of the operation
   */
tBleStatus HAP_HARC_DB_RestoreClientDatabase(HAP_HARC_Inst_t *pHAP_CltInst);

/**
   * @brief  Restore GATT Database of the HAP in IAC Role
   * @param  pHAP_Client: pointer on HAP Client Instance
   * @retval status of the operation
   */
tBleStatus HAP_IAC_DB_RestoreClientDatabase(HAP_IAC_Inst_t *pIAC_Inst);

#ifdef __cplusplus
}
#endif

#endif /* __HAP_DB_H */

