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
#include "ble_audio_stack.h"
#include "cap.h"
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
