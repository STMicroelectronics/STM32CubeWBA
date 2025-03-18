/**
  ******************************************************************************
  * @file    gmap.h
  * @author  MCD Application Team
  * @brief   This file contains definitions used for Gaming Audio Profile
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
#ifndef __GMAP_H
#define __GMAP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "cmsis_compiler.h"
#include "bap_types.h"
#include "cap.h"
#include "usecase_dev_mgmt.h"
#include "gmap_config.h"

/* Defines -------------------------------------------------------------------*/

/*
 * GMAP_NUM_GATT_SERVICES: number of GATT services required for Gaming Audio Profile.
 */
#define GMAP_NUM_GATT_SERVICES             (1u)

/* Array Size for each role */
#if (BLE_CFG_GMAP_UGG_ROLE == 1)
#define GMAP_UGG_NUM_GATT_ATTRIBUTES       (2u)
#else /* (BLE_CFG_GMAP_UGG_ROLE == 1) */
#define GMAP_UGG_NUM_GATT_ATTRIBUTES       (0u)
#endif /* (BLE_CFG_GMAP_UGG_ROLE == 1) */

#if (BLE_CFG_GMAP_UGT_ROLE == 1)
#define GMAP_UGT_NUM_GATT_ATTRIBUTES       (2u)
#else /* (BLE_CFG_GMAP_UGT_ROLE == 1) */
#define GMAP_UGT_NUM_GATT_ATTRIBUTES       (0u)
#endif /* (BLE_CFG_GMAP_UGT_ROLE == 1) */

#if (BLE_CFG_GMAP_BGS_ROLE == 1)
#define GMAP_BGS_NUM_GATT_ATTRIBUTES       (2u)
#else /* (BLE_CFG_GMAP_BGS_ROLE == 1) */
#define GMAP_BGS_NUM_GATT_ATTRIBUTES       (0u)
#endif /* (BLE_CFG_GMAP_BGS_ROLE == 1) */

#if (BLE_CFG_GMAP_BGR_ROLE == 1)
#define GMAP_BGR_NUM_GATT_ATTRIBUTES       (2u)
#else /* (BLE_CFG_GMAP_BGR_ROLE == 1) */
#define GMAP_BGR_NUM_GATT_ATTRIBUTES       (0u)
#endif /* (BLE_CFG_GMAP_BGR_ROLE == 1) */

/*
 * GMAP_NUM_GATT_ATTRIBUTES: number of GATT attributes required for Gaming Audio Profile
 */
#define GMAP_NUM_GATT_ATTRIBUTES           (2u + GMAP_UGG_NUM_GATT_ATTRIBUTES \
                                            + GMAP_UGT_NUM_GATT_ATTRIBUTES \
                                            + GMAP_BGS_NUM_GATT_ATTRIBUTES \
                                            + GMAP_BGR_NUM_GATT_ATTRIBUTES)

/* Array Size for each role */
#if (BLE_CFG_GMAP_UGG_ROLE == 1)
#define GMAP_UGG_ATT_VALUE_ARRAY_SIZE      (6u)
#else /* (BLE_CFG_GMAP_UGG_ROLE == 1) */
#define GMAP_UGG_ATT_VALUE_ARRAY_SIZE      (0u)
#endif /* (BLE_CFG_GMAP_UGG_ROLE == 1) */

#if (BLE_CFG_GMAP_UGT_ROLE == 1)
#define GMAP_UGT_ATT_VALUE_ARRAY_SIZE      (6u)
#else /* (BLE_CFG_GMAP_UGT_ROLE == 1) */
#define GMAP_UGT_ATT_VALUE_ARRAY_SIZE      (0u)
#endif /* (BLE_CFG_GMAP_UGT_ROLE == 1) */

#if (BLE_CFG_GMAP_BGS_ROLE == 1)
#define GMAP_BGS_ATT_VALUE_ARRAY_SIZE      (6u)
#else /* (BLE_CFG_GMAP_BGS_ROLE == 1) */
#define GMAP_BGS_ATT_VALUE_ARRAY_SIZE      (0u)
#endif /* (BLE_CFG_GMAP_BGS_ROLE == 1) */

#if (BLE_CFG_GMAP_BGR_ROLE == 1)
#define GMAP_BGR_ATT_VALUE_ARRAY_SIZE      (6u)
#else /* (BLE_CFG_GMAP_BGR_ROLE == 1) */
#define GMAP_BGR_ATT_VALUE_ARRAY_SIZE      (0u)
#endif /* (BLE_CFG_GMAP_BGR_ROLE == 1) */

/*
 * GMAP_ATT_VALUE_ARRAY_SIZE: size of the storage area for Attribute values for Gaming Audio Profile
 */
#define GMAP_ATT_VALUE_ARRAY_SIZE          (6u + GMAP_UGG_ATT_VALUE_ARRAY_SIZE \
                                            + GMAP_UGT_ATT_VALUE_ARRAY_SIZE \
                                            + GMAP_BGS_ATT_VALUE_ARRAY_SIZE \
                                            + GMAP_BGR_ATT_VALUE_ARRAY_SIZE)

/* Types ---------------------------------------------------------------------*/
/* Types of mask for roles of the Gaming Audio Profile */
typedef uint8_t GMAP_Role_t;
#define GMAP_ROLE_UNICAST_GAME_GATEWAY           (0x01)
#define GMAP_ROLE_UNICAST_GAME_TERMINAL          (0x02)
#define GMAP_ROLE_BROADCAST_GAME_SENDER          (0x04)
#define GMAP_ROLE_BROADCAST_GAME_RECEIVER        (0x08)

typedef uint8_t UGGFeatures_t;
#define UGG_FEATURES_MULTIPLEX_SUPPORT           (0x01)
#define UGG_FEATURES_96_KBPS_SOURCE_SUPPORT      (0x02)
#define UGG_FEATURES_MULTISINK_SUPPORT           (0x04)

typedef uint8_t UGTFeatures_t;
#define UGT_FEATURES_SOURCE_SUPPORT              (0x01)
#define UGT_FEATURES_80_KBPS_SOURCE_SUPPORT      (0x02)
#define UGT_FEATURES_SINK_SUPPORT                (0x04)
#define UGT_FEATURES_64_KBPSSINK_SUPPORT         (0x08)
#define UGT_FEATURES_MULTIPLEX_SUPPORT           (0x10)
#define UGT_FEATURES_MULTISINK_SUPPORT           (0x20)
#define UGT_FEATURES_MULTISOURCE_SUPPORT         (0x40)

typedef uint8_t BGSFeatures_t;
#define BGS_FEATURES_96_KBPS_SOURCE_SUPPORT      (0x01)

typedef uint8_t BGRFeatures_t;
#define BGR_FEATURES_MULTISINK_SUPPORT           (0x01)
#define BGR_FEATURES_MULTIPLEX_SUPPORT           (0x02)

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

/* Types for Gaming Audio Profile Events */
typedef enum
{
  GMAP_LINKUP_COMPLETE_EVT,             /* This event is notified by GMAP when the Linkup to a remote
                                         * GMAP Server is complete
                                         */
} GMAP_NotCode_t;

/* Structure used in parameter when GMAP_LINKUP_COMPLETE_EVT event is notified*/
typedef struct
{
  GMAP_Role_t           GMAPRole;       /* GMAP Role of remote device */
  UGGFeatures_t         UGGFeatures;    /* UGG Features of the remote device if UGG Role is supported */
  UGTFeatures_t         UGTFeatures;    /* UGT Features of the remote device if UGT Role is supported */
  BGSFeatures_t         BGSFeatures;    /* BGS Features of the remote device if BGS Role is supported */
  BGRFeatures_t         BGRFeatures;    /* BGR Features of the remote device if BGR Role is supported */
  uint16_t              StartAttHandle; /* ATT Start Handle of GMAS in the remote GMAP Device */
  uint16_t              EndAttHandle;   /* ATT End Handle of the GMAS in the remote GMAP Device */
} GMAP_AttServiceInfo_Evt_t;

typedef struct
{
  GMAP_NotCode_t        EvtOpcode;      /* Opcode of the notified event */
  tBleStatus            Status;         /* Status associated to the event */
  uint16_t              ConnHandle;     /* ACL Connection Handle associated to the event */
  uint8_t               *pInfo;         /* Pointer on information associated to the event */
}GMAP_Notification_Evt_t;

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
  const UseCaseConnInfo_t        *pConnInfo;                     /* Pointer to Connection info structure */
  GMAP_LinkupProcState_t         LinkupState;                    /* State of the ongoing Linkup */
  GMAS_Characteristic_t          CurrentLinkupChar;              /* HAP Characteristic being read/discovered */
  GMAP_GATT_CharacteristicInfo_t *pGattChar;                     /* Pointer on GATT Characteristic */
  uint16_t                       ReqHandle;                      /* Att Handle under process during ATT operation */
  uint8_t                        AttProcStarted;                 /* Flag to 1 if an ATT procedure is started */
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

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Initialize the Gaming Audio Profile
  * @param  GMAPRoles: GMAP Roles to initialize
  * @param  UGGFeatures: UGG Features to initialize
  * @param  UGTFeatures: UGT Features to initialize
  * @param  BGSFeatures: BGS Features to initialize
  * @param  BGRFeatures: BGR Features to initialize
  * @retval status of the initialization
  */
tBleStatus GMAP_Init(GMAP_Role_t GMAPRoles,
                     UGGFeatures_t UGGFeatures,
                     UGTFeatures_t UGTFeatures,
                     BGSFeatures_t BGSFeatures,
                     BGRFeatures_t BGRFeatures);

/**
  * @brief Link Up the GMAP Client with remote GMAP Server
  * @param  ConnHandle: Connection handle
  * @note GMAP_LINKUP_COMPLETE_EVT event will be generated once process is complete
  * @retval status of the operation
  */
tBleStatus GMAP_Linkup(uint16_t ConnHandle);

/**
  * @brief  Build ADV Packet for GMAP peripheral
  * @param Announcement: Targeted or General Announcement
  * @param pMetadata: pointer on the LTV-formatted Metadata
  * @param MetadataLength: size of the LTV-formatted Metadata
  * @param Appearance: Appearance Value of the device
  * @param pAdvPacket: buffer provided by upper layer to store ADV information
  * @param AdvPacketLength: size of the buffer pointed by pAdvPacket.
  * @retval length of the built ADV Packet
  */
uint8_t GMAP_BuildAdvPacket(CAP_Announcement_t Announcement,
                               uint8_t const *pMetadata,
                               uint8_t MetadataLength,
                               uint16_t Appearance,
                               uint8_t *pAdvPacket,
                               uint8_t AdvPacketLength);

/**
  * @brief  Notify GMAP Events
  * @param  pNotification: pointer on notification information
 */
void GMAP_Notification(GMAP_Notification_Evt_t *pNotification);


#ifdef __cplusplus
}
#endif

#endif /* __GMAP_H */
