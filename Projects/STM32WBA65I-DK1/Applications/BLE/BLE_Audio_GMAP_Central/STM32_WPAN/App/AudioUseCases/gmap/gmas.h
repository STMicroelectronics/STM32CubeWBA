/**
  ******************************************************************************
  * @file    gmas.h
  * @author  MCD Application Team
  * @brief   This file contains definitions used for Gaming Audio Service
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
#ifndef __GMAS_H
#define __GMAS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "gmap_config.h"
#include "gmap.h"

/* Defines -----------------------------------------------------------*/
#define GAMING_AUDIO_SERVICE_UUID  (0x1858)

#define GMAP_ROLE_UUID             (0x2C00)
#define UGG_FEATURES_UUID          (0x2C01)
#define UGT_FEATURES_UUID          (0x2C02)
#define BGS_FEATURES_UUID          (0x2C03)
#define BGR_FEATURES_UUID          (0x2C04)

/* Types -----------------------------------------------------------*/

/* Telephony and Media Audio Service Context Type*/
typedef struct
{
  uint16_t      ServiceHandle;                          /*Service Handle*/
  uint16_t      ServiceEndHandle;                       /*Service End Handle*/
  uint16_t      GMAPRoleHandle;                         /*GMAP Role Characteristic handle*/

#if (BLE_CFG_GMAP_UGG_ROLE == 1)
  uint16_t      UGGFeaturesHandle;                      /*UGG Features Characteristic handle*/
#endif /* (BLE_CFG_GMAP_UGG_ROLE == 1) */

#if (BLE_CFG_GMAP_UGT_ROLE == 1)
  uint16_t      UGTFeaturesHandle;                      /*UGT Features Characteristic handle*/
#endif /* (BLE_CFG_GMAP_UGT_ROLE == 1) */

#if (BLE_CFG_GMAP_BGS_ROLE == 1)
  uint16_t      BGSFeaturesHandle;                      /*BGS Features Characteristic handle*/
#endif /* (BLE_CFG_GMAP_BGS_ROLE == 1) */

#if (BLE_CFG_GMAP_BGR_ROLE == 1)
  uint16_t      BGRFeaturesHandle;                      /*BGR Features Characteristic handle*/
#endif /* (BLE_CFG_GMAP_BGR_ROLE == 1) */
} GMAS_ServiceContext_t;

/**
  * @}
  */
tBleStatus GMAS_InitService(GMAS_ServiceContext_t *pSrvContext);
tBleStatus GMAS_InitCharacteristics(GMAS_ServiceContext_t *pSrvContext,
                                    GMAP_Role_t GMAPRoles,
                                    UGGFeatures_t UGGFeatures,
                                    UGTFeatures_t UGTFeatures,
                                    BGSFeatures_t BGSFeatures,
                                    BGRFeatures_t BGRFeatures);
tBleStatus GMAS_SetGMAPRole(GMAS_ServiceContext_t *pSrvContext, GMAP_Role_t Role);

#ifdef __cplusplus
}
#endif

#endif /* __GMAS_H */