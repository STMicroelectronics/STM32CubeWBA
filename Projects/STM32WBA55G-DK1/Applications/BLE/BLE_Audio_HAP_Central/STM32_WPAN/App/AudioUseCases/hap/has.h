/**
  ******************************************************************************
  * @file    has.h
  * @author  MCD Application Team
  * @brief   This file contains definitions used for Hearing Access Service
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
#ifndef __HAS_H
#define __HAS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "cmsis_compiler.h"
#include <stdint.h>
#include "ble_types.h"
#include "svc_ctl.h"
#include "hap.h"

/* Defines -----------------------------------------------------------*/
#define HEARING_ACCESS_SERVICE_UUID           (0x1854)

#define HEARING_AID_FEATURES_UUID             (0x2BDA)
#define HEARING_AID_PRESET_CONTROL_POINT_UUID (0x2BDB)
#define ACTIVE_PRESET_INDEX_UUID              (0x2BDC)

/* Types -----------------------------------------------------------*/

/* Hearing Access Service Context Type*/
typedef struct
{
  uint16_t      ServiceHandle;                      /*Service Handle*/
  uint16_t      ServiceEndHandle;                   /*Service End Handle*/
  uint16_t      HearingAidFeaturesHandle;           /*Hearing Aid Features Characteristic handle*/
  uint16_t      HearingAidPresetControlPointHandle; /*Hearing Aid Preset Control Point Characteristic handle*/
  uint16_t      ActivePresetIndexHandle;            /*Active Preset Index Characteristic handle*/
} HAS_ServiceContext_t;

typedef enum
{
  HAS_CONTROL_POINT_EVT,                       /* HAS Control Operation received */
} HAS_NotCode_t;

typedef struct
{
  HAS_NotCode_t EvtOpcode;
  uint16_t      ConnHandle;
  uint8_t       *pData;
  uint16_t      DataLen;
} HAS_NotificationEvt_t;


/**
  * @}
  */
tBleStatus HAS_InitService(HAS_ServiceContext_t *pSrvContext);
tBleStatus HAS_InitCharacteristics(HAS_ServiceContext_t *pSrvContext);
tBleStatus HAS_SetHearingAidFeatures(HAS_ServiceContext_t *pSrvContext, uint16_t ConnHandle, HAP_HA_Features_t Features);
tBleStatus HAS_SetActivePresetIndex(HAS_ServiceContext_t *pSrvContext, uint16_t ConnHandle, uint8_t ActivePresetIndex);
tBleStatus HAS_SetReadPresetsResponse(HAS_ServiceContext_t *pSrvContext, uint16_t ConnHandle,
                                             HAP_Preset_t* PresetRecord, uint8_t IsLast);
tBleStatus HAS_SetPresetGenericUpdate(HAS_ServiceContext_t *pSrvContext, uint16_t ConnHandle, uint8_t IsLast,
                                      uint8_t PrevIndex, HAP_Preset_t* PresetRecord);
tBleStatus HAS_SetPresetRecordDeleted(HAS_ServiceContext_t *pSrvContext, uint16_t ConnHandle, uint8_t IsLast,
                                      uint8_t Index);
tBleStatus HAS_SetPresetRecordAvailable(HAS_ServiceContext_t *pSrvContext, uint16_t ConnHandle, uint8_t IsLast,
                                        uint8_t Index);
tBleStatus HAS_SetPresetRecordUnavailable(HAS_ServiceContext_t *pSrvContext, uint16_t ConnHandle, uint8_t IsLast,
                                          uint8_t Index);
SVCCTL_EvtAckStatus_t HAS_ATT_Event_Handler(void *pEvent);
void HAS_Notification(HAS_NotificationEvt_t const *pNotification);
uint8_t HAS_CheckCtrlOpParams(uint16_t ConnHandle, uint8_t *pData,uint8_t DataLen);
SVCCTL_EvtAckStatus_t HAS_HandleCtrlPointIndicateComplete(uint16_t ConnHandle);

#ifdef __cplusplus
}
#endif

#endif /* __HAS_H */