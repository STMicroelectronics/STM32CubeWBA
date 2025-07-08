/**
  ******************************************************************************
  * @file    has_iac.h
  * @author  MCD Application Team
  * @brief   This file contains definitions used for Hearing Access Service
             as Immediate Alert Client Role
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
#ifndef __HAS_IAC_H
#define __HAS_IAC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "cmsis_compiler.h"
#include <stdint.h>
#include "ble_types.h"
#include "svc_ctl.h"
#include "hap.h"
#include "usecase_dev_mgmt.h"

/* Defines -----------------------------------------------------------*/

typedef uint8_t HAP_LinkupProcState_t;
#define HAP_IAC_LINKUP_IDLE               (0x00u)
#define HAP_IAC_LINKUP_DISC_SERVICE       (0x01u) /* Discover HAS in remote GATT database */
#define HAP_IAC_LINKUP_DISC_CHAR          (0x02u) /* Discover all Characteristics of the
                                               * HAS in remote GATT database
                                               */
#define HAP_IAC_LINKUP_RESTORE            (0x20u) /* HAP Linkup restore*/
#define HAP_IAC_LINKUP_COMPLETE           (0x40u) /* HAP Link Up is complete with success */

/* Types -----------------------------------------------------------*/

/* Structure used to store GATT characteristic information */
typedef struct
{
  uint16_t      ValueHandle;            /* Handle of the characteristic value */
  uint16_t      DescHandle;             /* handle of the characteristic descriptor */
  uint8_t       Properties;             /* Properties of the characteristic */
  uint16_t      EndHandle;              /* Last handle of the characteristic */
}HAP_GATT_CharacteristicInfo_t;

/* HAP Controller Instance Structure */
typedef struct
{
  UseCaseConnInfo_t              *pConnInfo;                     /* ACL Connection Information*/
  HAP_LinkupMode_t               LinkupMode;                     /* Linkup Mode */
  uint16_t                       LinkupState;                    /* Bitmask of HAP_LinkupProcState_t and
                                                                  * HAS_Characteristic_t
                                                                  */
  uint16_t                       ReqHandle;                      /* Att Handle under process during ATT operation */
  uint8_t                        DelayDeallocation;              /* Indicate if the HAP Client Instance deallocation
                                                                  * should be delayed when ACI_GATT_PROC_COMPLETE_VSEVT_CODE
                                                                  * event is received */

  uint16_t                       IASServiceStartHandle;          /* start handle of the Service in the
                                                                  * GATT Database of the IAS Server
                                                                  */
  uint16_t                       IASServiceEndHandle;            /* end handle of the Service in the
                                                                  * GATT Database of the HAS Server
                                                                  */
  HAP_GATT_CharacteristicInfo_t  AlertLevelChar;                 /* IAS Alert Level characteristic */

  uint8_t                        ErrorCode;                      /* GATT Error Code */
} HAP_IAC_Inst_t;

typedef enum
{
  IAC_LINKUP_COMPLETE_EVT,                       /* HAP IAC Linkup is complete */
} HAP_IAC_NotCode_t;

typedef struct
{
  tBleStatus        Status;
  HAP_IAC_NotCode_t EvtOpcode;
  uint16_t          ConnHandle;
  uint8_t            *pInfo;
  uint16_t          DataLen;
} HAP_IAC_NotificationEvt_t;

void HAP_IAC_Notification(HAP_IAC_NotificationEvt_t const *pNotification);

/**
  * @brief  Initialize the Hearing Access Profile as Immmediate Alert Client Role
  * @retval status of the initialization
  */
tBleStatus HAP_IAC_Init(void);

/** @brief This function is used by the Device in the HAP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */
SVCCTL_EvtAckStatus_t HAP_IAC_GATT_Event_Handler(void *pEvent);

/**
  * @brief Link Up the HAP IAC with remote HAP HA
  * @param  ConnHandle: Connection handle
  * @param  LinkupMode: LinkUp Mode
  * @note HAP_IAC_LINKUP_COMPLETE_EVT event will be generated once process is complete
  * @retval status of the operation
  */
tBleStatus HAP_IAC_Linkup(uint16_t ConnHandle, HAP_LinkupMode_t LinkupMode);

/**
   * @brief  Store GATT Database of the HAP IAS Characteristics and Services
   * @param  ConnHandle: Connetion Handle of the remote connection
   * @param  [in] pData: pointer on buffer in which GATT Database of HAP characteristics in which information is stored
   * @param  [in] MaxDataLen: maximum data length to store
   * @param  [out] len : length in bytes of stored data
   * @retval status of the operation
   */
tBleStatus HAP_IAC_StoreDatabase(uint16_t ConnHandle, uint8_t *pData, uint16_t MaxDataLen, uint16_t *len);

/**
   * @brief  Restore GATT Database of the IAS characteristics
   * @param  pIAC_Inst: pointer on IAS Client Instance
   * @param  pData: pointer on buffer including GATT Database of IAS characteristics
   * @param  len : length in bytes of stored data
   * @retval status of the operation
   */
tBleStatus HAP_IAC_RestoreDatabase(HAP_IAC_Inst_t *pIAC_Inst, uint8_t *pData,uint16_t Len);

/**
  * @brief  Notify ACL Disconnection to HAP IAC
  * @param  ConnHandle: ACL Connection Handle
  */
void HAP_IAC_AclDisconnection(uint16_t ConnHandle);

#ifdef __cplusplus
}
#endif

#endif /* __HAS_IAC_H */