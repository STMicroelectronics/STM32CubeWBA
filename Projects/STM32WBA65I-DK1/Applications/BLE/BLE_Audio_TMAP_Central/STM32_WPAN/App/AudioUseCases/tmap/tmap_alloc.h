/**
  ******************************************************************************
  * @file    tmap_alloc.h
  * @author  MCD Application Team
  * @brief   This file contains definitions used for Telephony and Media Audio
  *          Profile Allocation context
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
#ifndef __TMAP_ALLOC_H
#define __TMAP_ALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "cmsis_compiler.h"
#include "tmap.h"
#include "tmap_config.h"
#include "tmas.h"
#include "usecase_dev_mgmt.h"

/* Types ---------------------------------------------------------------------*/

typedef uint8_t TMAP_Operation_t;
#define TMAP_OP_NONE                            (0x00u) /* No operation */
#define TMAP_OP_READ                            (0x01u) /* Read Operation */

typedef uint8_t TMAP_LinkupProcState_t;
#define TMAP_LINKUP_IDLE                        (0x00u)
#define TMAP_LINKUP_DISC_SERVICE                (0x01u) /* Discover TMAS in remote GATT database */
#define TMAP_LINKUP_DISC_CHAR                   (0x02u) /* Discover all Characteristics of the
                                                         * TMAS in remote GATT database
                                                         */
#define TMAP_LINKUP_DISC_CHAR_DESC              (0x04u) /* Discover all Characteristics descriptor
                                                         * of the TMAS in remote GATT database
                                                         */
#define TMAP_LINKUP_READ_CHAR                   (0x08u) /* Read Characteristic in the remote GATT
                                                         * database.
                                                         */
#define TMAP_LINKUP_RESTORE                     (0x10u) /* TMAP Linkup restore*/
#define TMAP_LINKUP_COMPLETE                    (0x20u) /* TMAP Link Up is complete with success */


/* Structure used to store GATT characteristic information */
typedef struct
{
  uint16_t      ValueHandle;            /* Handle of the characteristic value */
  uint16_t      DescHandle;             /* handle of the characteristic descriptor */
  uint8_t       Properties;             /* Properties of the characteristic */
  uint16_t      EndHandle;              /* Last handle of the characteristic */
}TMAP_GATT_CharacteristicInfo_t;

/* TMAP Controller Instance Structure */
typedef struct
{
  UseCaseConnInfo_t              *pConnInfo;                     /* ACL Connection Information*/
  TMAP_LinkupMode_t              LinkupMode;                     /* Linkup Mode */
  uint16_t                       LinkupState;                    /* Bitmask of TMAP_LinkupProcState_t and
                                                                  * TMAS_Characteristic_t
                                                                  */
  TMAP_GATT_CharacteristicInfo_t *pGattChar;                     /* Pointer on GATT Characteristic */
  TMAP_Operation_t               Op;                             /* Current TMAP_Operation_t */
  uint16_t                       ReqHandle;                      /* Att Handle under process during ATT operation */
  uint8_t                        DelayDeallocation;              /* Indicate if the TMAP Client Instance deallocation
                                                                  * should be delayed when ACI_GATT_PROC_COMPLETE_VSEVT_CODE
                                                                  * event is received */
  uint16_t                       ServiceStartHandle;             /* start handle of the Service in the
                                                                  * GATT Database of the MICS Server
                                                                  */
  uint16_t                       ServiceEndHandle;               /* end handle of the Service in the
                                                                  * GATT Database of the MICS Server
                                                                  */
  TMAP_GATT_CharacteristicInfo_t TMAPRoleChar;                   /* TMAP Role characteristic */

  uint8_t                        ErrorCode;                      /* GATT Error Code */
}TMAP_CltInst_t;

typedef struct
{
  TMAP_Role_t             Role;
  TMAS_ServiceContext_t   TMASSvc;
  uint8_t                 MaxNumBleLinks;       /* Maximum Number of Ble Links */
  TMAP_CltInst_t          *pInst;               /* Pointer on TMAP Instances */
}TMAP_Context_t;

extern TMAP_Context_t TMAP_Context;

#ifdef __cplusplus
}
#endif

#endif /* __TMAP_ALLOC_H */

