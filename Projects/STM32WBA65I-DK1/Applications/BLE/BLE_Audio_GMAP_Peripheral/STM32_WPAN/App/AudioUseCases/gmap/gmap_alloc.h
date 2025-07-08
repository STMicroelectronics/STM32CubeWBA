/**
  ******************************************************************************
  * @file    gmap_alloc.h
  * @author  MCD Application Team
  * @brief   This file contains definitions used for Gaming Audio Profile
  *          Allocation context
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
#ifndef __GMAP_ALLOC_H
#define __GMAP_ALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "cmsis_compiler.h"
#include "gmap.h"
#include "gmap_config.h"
#include "gmas.h"
#include "usecase_dev_mgmt.h"

/* Types ---------------------------------------------------------------------*/

typedef uint8_t GMAP_LinkupProcState_t;
#define GMAP_LINKUP_IDLE                   (0x00u)
#define GMAP_LINKUP_DISC_SERVICE           (0x01u) /* Discover GMAS in remote GATT database */
#define GMAP_LINKUP_DISC_CHAR              (0x02u) /* Discover all Characteristics of the
                                                    * GMAS in remote GATT database
                                                    */
#define GMAP_LINKUP_DISC_CHAR_DESC         (0x04u) /* Discover all Characteristics descriptor
                                                    * of the GMAS in remote GATT database
                                                    */
#define GMAP_LINKUP_READ_CHAR              (0x08u) /* Read Characteristic in the remote GATT
                                                    * database.
                                                    */
#define GMAP_LINKUP_RESTORE                (0x10u) /* GMAP Linkup restore*/
#define GMAP_LINKUP_COMPLETE               (0x20u) /* GMAP Link Up is complete with success */


typedef uint8_t GMAS_Characteristic_t;
#define GMAS_CHAR_GMAP_ROLE                (0x01)
#define GMAS_CHAR_UGG_FEATURES             (0x02)
#define GMAS_CHAR_UGT_FEATURES             (0x03)
#define GMAS_CHAR_BGS_FEATURES             (0x04)
#define GMAS_CHAR_BGR_FEATURES             (0x05)

/* Structure used to store GATT characteristic information */
typedef struct
{
  uint16_t      ValueHandle;            /* Handle of the characteristic value */
  uint16_t      DescHandle;             /* handle of the characteristic descriptor */
  uint8_t       Properties;             /* Properties of the characteristic */
  uint16_t      EndHandle;              /* Last handle of the characteristic */
}GMAP_GATT_CharacteristicInfo_t;

/* GMAP Controller Instance Structure */
typedef struct
{
  UseCaseConnInfo_t              *pConnInfo;                     /* Pointer to Connection info structure */
  GMAP_LinkupProcState_t         LinkupState;                    /* State of the ongoing Linkup */
  GMAS_Characteristic_t          CurrentLinkupChar;              /* HAP Characteristic being read/discovered */
  GMAP_GATT_CharacteristicInfo_t *pGattChar;                     /* Pointer on GATT Characteristic */
  uint16_t                       ReqHandle;                      /* Att Handle under process during ATT operation */
  uint8_t                        DelayDeallocation;              /* Indicate if the GMAP Client Instance deallocation
                                                                  * should be delayed when ACI_GATT_PROC_COMPLETE_VSEVT_CODE
                                                                  * event is received */
  uint16_t                       ServiceStartHandle;             /* start handle of the Service in the
                                                                  * GATT Database of the MICS Server
                                                                  */
  uint16_t                       ServiceEndHandle;               /* end handle of the Service in the
                                                                  * GATT Database of the MICS Server
                                                                  */
  GMAP_GATT_CharacteristicInfo_t GMAPRoleChar;                   /* GMAP Role characteristic */
  GMAP_Role_t                    GMAPRole;                       /* GMAP Role value */
  GMAP_GATT_CharacteristicInfo_t UGGFeaturesChar;                /* UGG Features characteristic */
  UGGFeatures_t                  UGGFeatures;                    /* UGG Features value */
  GMAP_GATT_CharacteristicInfo_t UGTFeaturesChar;                /* UGT Features characteristic */
  UGTFeatures_t                  UGTFeatures;                    /* UGT Features value */
  GMAP_GATT_CharacteristicInfo_t BGSFeaturesChar;                /* BGS Features characteristic */
  BGSFeatures_t                  BGSFeatures;                    /* BGS Features value */
  GMAP_GATT_CharacteristicInfo_t BGRFeaturesChar;                /* BGR Features characteristic */
  BGRFeatures_t                  BGRFeatures;                    /* BGR Features value */

  uint8_t                        ErrorCode;                      /* GATT Error Code */
}GMAP_CltInst_t;

typedef struct
{
  GMAP_Role_t             Role;
  UGGFeatures_t           UGGFeatures;
  UGTFeatures_t           UGTFeatures;
  BGSFeatures_t           BGSFeatures;
  BGRFeatures_t           BGRFeatures;
  GMAS_ServiceContext_t   GMASSvc;
  GMAP_CltInst_t          CltInst[USECASE_DEV_MGMT_MAX_CONNECTION]; /* Pointer on Connection Instances */
}GMAP_Context_t;

extern GMAP_Context_t GMAP_Context;

#ifdef __cplusplus
}
#endif

#endif /* __GMAP_ALLOC_H */

