/**
  ******************************************************************************
  * @file    tmap_db.h
  * @author  MCD Application Team
  * @brief   This file contains definitions used for TMAP Profile
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
#ifndef __TMAP_DB_H
#define __TMAP_DB_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "tmap_alloc.h"

/* Functions ---------------------------------------------------------------------*/
void TMAP_CLT_StoreDatabase(TMAP_CltInst_t const *pTMAP_CltInst);
tBleStatus TMAP_CLT_RestoreDatabase(TMAP_CltInst_t *pTMAP_CltInst);
tBleStatus TMAP_DB_RemoveServicesRecord(uint8_t PeerIdentityAddressType, const uint8_t PeerIdentityAddress[6]);

#ifdef __cplusplus
}
#endif

#endif /* __TMAP_DB_H */

