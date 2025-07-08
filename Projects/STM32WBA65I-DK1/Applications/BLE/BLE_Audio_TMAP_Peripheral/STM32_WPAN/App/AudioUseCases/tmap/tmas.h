/**
  ******************************************************************************
  * @file    tmas.h
  * @author  MCD Application Team
  * @brief   This file contains definitions used for Telephony and Media Audio
  *          Service
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
#ifndef __TMAS_H
#define __TMAS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "tmap_config.h"
#include "tmap.h"

/* Defines -----------------------------------------------------------*/
#define TELEPHONY_AND_MEDIA_AUDIO_SERVICE_UUID  (0x1855)

#define TMAP_ROLE_UUID                          (0x2B51)

/* Types -----------------------------------------------------------*/

/* Telephony and Media Audio Service Context Type*/
typedef struct
{
  uint16_t      ServiceHandle;                          /*Service Handle*/
  uint16_t      ServiceEndHandle;                       /*Service End Handle*/
  uint16_t      TMAPRoleHandle;                         /*TMAP Role Characteristic handle*/
} TMAS_ServiceContext_t;

/**
  * @}
  */
tBleStatus TMAS_InitService(TMAS_ServiceContext_t *pSrvContext);
tBleStatus TMAS_InitCharacteristics(TMAS_ServiceContext_t *pSrvContext);
tBleStatus TMAS_SetTMAPRole(TMAS_ServiceContext_t *pSrvContext, TMAP_Role_t Role);

#ifdef __cplusplus
}
#endif

#endif /* __TMAS_H */