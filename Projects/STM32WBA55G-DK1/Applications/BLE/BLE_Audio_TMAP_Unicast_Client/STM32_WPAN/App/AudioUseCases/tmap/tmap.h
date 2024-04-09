/**
  ******************************************************************************
  * @file    tmap.h
  * @author  MCD Application Team
  * @brief   This file contains definitions used for Telephony and Media Audio
  *          Profile
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
#ifndef __TMAP_H
#define __TMAP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "cmsis_compiler.h"
#include "bap_types.h"
#include "cap.h"

/* Defines -------------------------------------------------------------------*/

/*
 * TMAP_NUM_GATT_SERVICES: number of GATT services required for Telephony and Media Audio Profile.
 */
#define TMAP_NUM_GATT_SERVICES             (1u)

/*
 * TMAP_NUM_GATT_ATTRIBUTES: number of GATT attributes required for Telephony and Media Audio Profile
 */
#define TMAP_NUM_GATT_ATTRIBUTES           (4u)

/*
 * TMAP_CLT_MEM_PER_CONN_SIZE_BYTES: memory size used per link in TMAP Client
 */
#define TMAP_CLT_MEM_PER_CONN_SIZE_BYTES   (32u)

/*
 * TMAP_MEM_TOTAL_BUFFER_SIZE: total memory used by the TMAP, function of the number of links possible
 */
#define TMAP_MEM_TOTAL_BUFFER_SIZE(num_ble_links) (num_ble_links * TMAP_CLT_MEM_PER_CONN_SIZE_BYTES)


/* Types ---------------------------------------------------------------------*/
/* Types of mask for roles of the Media Control Profile */
typedef uint16_t TMAP_Role_t;
#define TMAP_ROLE_CALL_GATEWAY                  (0x01)
#define TMAP_ROLE_CALL_TERMINAL                 (0x02)
#define TMAP_ROLE_UNICAST_MEDIA_SENDER          (0x04)
#define TMAP_ROLE_UNICAST_MEDIA_RECEIVER        (0x08)
#define TMAP_ROLE_BROADCAST_MEDIA_SENDER        (0x10)
#define TMAP_ROLE_BROADCAST_MEDIA_RECEIVER      (0x20)

typedef uint8_t TMAP_Operation_t;
#define TMAP_OP_NONE                            (0x00u)         /* No operation */
#define TMAP_OP_READ                            (0x01u)         /* Read Operation */

/* Types of Microphone Control Profile Linkup Mode */
typedef uint8_t TMAP_LinkupMode_t;
#define TMAP_LINKUP_MODE_COMPLETE          (0x00u) /* Link Up procedure shall be a complete one
                                                    * with remote service and characteristic discovery
                                                                 */
#define TMAP_LINKUP_MODE_RESTORE           (0x01u) /* Link Up information is restored from previous
                                                    * complete Link Up from persistent memory.
                                                    */

typedef uint8_t TMAP_LinkupProcState_t;
#define TMAP_LINKUP_IDLE                   (0x00u)
#define TMAP_LINKUP_DISC_SERVICE           (0x01u) /* Discover TMAS in remote GATT database */
#define TMAP_LINKUP_DISC_CHAR              (0x02u) /* Discover all Characteristics of the
                                                    * TMAS in remote GATT database
                                                    */
#define TMAP_LINKUP_DISC_CHAR_DESC         (0x04u) /* Discover all Characteristics descriptor
                                                    * of the TMAS in remote GATT database
                                                    */
#define TMAP_LINKUP_READ_CHAR              (0x08u) /* Read Characteristic in the remote GATT
                                                    * database.
                                                    */
#define TMAP_LINKUP_RESTORE                (0x10u) /* TMAP Linkup restore*/
#define TMAP_LINKUP_COMPLETE               (0x20u) /* TMAP Link Up is complete with success */

/* Types for Telephony and Media Audio Profile Events */
typedef enum
{
  TMAP_LINKUP_COMPLETE_EVT,             /* This event is notified by TMAP when the Linkup to a remote
                                         * TMAP Server is complete
                                         */
  TMAP_REM_ROLE_VALUE_EVT               /* This event is notified by TMAP when the remote TMAP Role value
                                         * has been read
                                         */
} TMAP_NotCode_t;

/* Structure used in parameter when TMAP_LINKUP_COMPLETE_EVT event is notified*/
typedef struct
{
  uint16_t              StartAttHandle; /* ATT Start Handle of TMAS in the remote TMAP Device */
  uint16_t              EndAttHandle;   /* ATT End Handle of the TMAS in the remote TMAP Device */
} TMAP_AttServiceInfo_Evt_t;

typedef struct
{
  TMAP_NotCode_t        EvtOpcode;      /* Opcode of the notified event */
  tBleStatus            Status;         /* Status associated to the event */
  uint16_t              ConnHandle;     /* ACL Connection Handle associated to the event */
  uint8_t               *pInfo;         /* Pointer on information associated to the event */
}TMAP_Notification_Evt_t;

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
  uint16_t                       ConnHandle;                     /* Connection Handle associated to the link
                                                                  * between the TMAP Client with the remote
                                                                  * TMAP Server
                                                                  */
  TMAP_LinkupMode_t              LinkupMode;                     /* Linkup Mode */
  uint16_t                       LinkupState;                    /* Bitmask of TMAP_LinkupProcState_t and
                                                                  * TMAS_Characteristic_t
                                                                  */
  TMAP_GATT_CharacteristicInfo_t *pGattChar;                     /* Pointer on GATT Characteristic */
  TMAP_Operation_t               Op;                             /* Current TMAP_Operation_t */
  uint16_t                       ReqHandle;                      /* Att Handle under process during ATT operation */
  uint8_t                        AttProcStarted;                 /* Flag to 1 if an ATT procedure is started */
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

/* TMAP Configuration */
typedef struct
{
  uint16_t              Role;                   /* Bitfield of TMAP Roles */
  uint8_t               MaxNumBleLinks;         /* Maximum Number of BLE Links */
  uint8_t               *pStartRamAddr;         /* Start address of the RAM buffer allocated for TMAP Controller
                                                 * Context memory resource.
                                                 * It must be a 32bit aligned RAM area.
                                                 */
  uint16_t              RamSize;                /* Size of the RAM allocated at pStartRamAddr
                                                 * pointer.
                                                 */
} TMAP_Config_t;

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Initialize the Telephony and Media Audio Profile
  * @param  pConfig: Pointer to the TMAP Config structure
  * @retval status of the initialization
  */
tBleStatus TMAP_Init(TMAP_Config_t *pConfig);

/**
  * @brief Link Up the TMAP Client with remote TMAP Server
  * @param  ConnHandle: Connection handle
  * @param  LinkupMode: LinkUp Mode
  * @note TMAP_LINKUP_COMPLETE_EVT event will be generated once process is complete
  * @retval status of the operation
  */
tBleStatus TMAP_Linkup(uint16_t ConnHandle, TMAP_LinkupMode_t LinkupMode);

/**
  * @brief Indicate if TMAP Database is saved in NVM
  * @param Peer_Address_Type: Peer Address type
  * @param Peer_Address: Peer Address
  * @retval 0 if not present in the database, else 1
  */
uint8_t TMAP_DB_IsPresent(uint8_t Peer_Address_Type,const uint8_t Peer_Address[6]);

/**
  * @brief  Build ADV Packet for TMAP peripheral
  * @param Announcement: Targeted or General Announcement
  * @param pMetadata: pointer on the LTV-formatted Metadata
  * @param MetadataLength: size of the LTV-formatted Metadata
  * @param Appearance: Appearance Value of the device
  * @param pAdvPacket: buffer provided by upper layer to store ADV information
  * @param AdvPacketLength: size of the buffer pointed by pAdvPacket.
  * @retval length of the built ADV Packet
  */
uint8_t TMAP_BuildAdvPacket(CAP_Announcement_t Announcement,
                               uint8_t const *pMetadata,
                               uint8_t MetadataLength,
                               uint16_t Appearance,
                               uint8_t *pAdvPacket,
                               uint8_t AdvPacketLength);

/**
  * @brief Read the remote TMAP Role value
  * @param ConnHandle: The connection of the remote TMAP Server
  * @note TMAP_REM_ROLE_VALUE_EVT will be generated upon characteristic read
  * @return Status of the operation
  */
tBleStatus TMAP_ReadRemoteTMAPRole(uint16_t ConnHandle);

/**
  * @brief  Notify TMAP Events
  * @param  pNotification: pointer on notification information
 */
void TMAP_Notification(TMAP_Notification_Evt_t *pNotification);


#ifdef __cplusplus
}
#endif

#endif /* __TMAP_H */
