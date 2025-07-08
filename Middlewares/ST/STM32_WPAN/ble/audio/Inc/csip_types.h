/**
  ******************************************************************************
  * @file    csip.h
  * @author  MCD Application Team
  * @brief   This file contains types used for Coordinated Set
  *          Identification Profile
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
#ifndef __CSIP_TYPES_H
#define __CSIP_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ble_types.h"
/* Defines -------------------------------------------------------------------*/

/* #############################################################################
   #       Defines and MACRO used to allocate GATT Services, GATT Attributes   #
   #       and storage area for Attribute values for Coordinated Set           #
   #       Identification Profile in Set Member role                           #
   ############################################################################# */

/*
 * CSIP_SET_MEMBER_NUM_GATT_SERVICES: number of GATT services required for Coordinated Set Identification Profile
 * in Set Member role per Coordinated Set Identification Instance registration.
 *
 * @param num_csi_instances: Maximum number of supported Coordinated Set Identification Instances
 */
#define CSIP_SET_MEMBER_NUM_GATT_SERVICES(num_csi_instances)            (num_csi_instances)

/*
 * CSIP_SET_MEMBER_NUM_GATT_ATTRIBUTES: number of GATT attributes required according to the number of
 * Coordinated Set Identification Instance registration.
 *
 * @param num_csi_instances: Maximum number of supported Coordinated Set Identification Instances
 */
#define CSIP_SET_MEMBER_NUM_GATT_ATTRIBUTES(num_csi_instances)          (11u * num_csi_instances)

/*
 * CSIP_SET_MEMBER_ATT_VALUE_ARRAY_SIZE: this macro returns the size of the storage area for Attribute values
 * in CSIP Set Member Server according to the number of Coordinated Set Identification Instance registration.
 *
 * @param num_csi_instances: Maximum number of supported Coordinated Set Identification Instances
 *
 * @param num_ble_links: Maximum number of simultaneous connections that the device will support.
 */
#define CSIP_SET_MEMBER_ATT_VALUE_ARRAY_SIZE(num_csi_instances,num_ble_links) \
                                                                        ((40u + (6u*num_ble_links)) * num_csi_instances)

/* #############################################################################
   #       Defines and MACRO used to allocate memory resource of               #
   #       Coordinated Set Identification Profile in CSIP_Config_t             #
   #       (BLE_CSIP_SET_COORDINATOR_TOTAL_BUFFER_SIZE ,                       #
   #       BLE_CSIP_SET_MEMBER_TOTAL_BUFFER_SIZE)                              #
   ############################################################################# */

/*
 * CSIP_SET_COORDINATOR_MEM_PER_CONN_SIZE_BYTES: memory size used per link in CSIP Set Coordinator role
 */
#define CSIP_SET_COORDINATOR_MEM_PER_CONN_SIZE_BYTES            (68u)

/*
 * CSIP_SET_COORDINATOR_MEM_PER_INSTANCE_SIZE_BYTES: memory size used by CSIP Set Coordinator to allocate Coordinated
 *                                                   Set Identification Instance
 */
#define CSIP_SET_COORDINATOR_MEM_CIS_INSTANCE_SIZE_BYTES        (60u)

/*
 * CSIP_SET_MEMBER_MEM_PER_INSTANCE_SIZE_BYTES: memory size used by CSIP Set Member per Coordinated Set Identification
 *                                              Instances
 */
#define CSIP_SET_MEMBER_MEM_PER_INSTANCE_SIZE_BYTES             (84u)

/*
 * CSIP_SERV_INST_MEM_PER_CONN_SIZE_BYTES: memory size used to allocate Service Context per Connection Instance
 *                                         per Coordinated Set Identification Instances
 */
#define CSIP_SERV_INST_MEM_PER_CONN_SIZE_BYTES                  (16u)

/*
 * CSIP_NVM_MGMT_PER_CONN_SIZE_BYTES: memory size used for Non-Volatile Memory Management and CSIS restoration
 *                                   by CSIP Set Member
 */
#define CSIP_NVM_MGMT_PER_CONN_SIZE_BYTES                       (8u)

/*
 * BLE_CSIP_SET_COORDINATOR_TOTAL_BUFFER_SIZE: this macro returns the amount of memory, in bytes,needed for
 * the storage of data structures in CSIP Set Coordiantor Client whose size depends on the number of supported
 * connections.
 *
 * @param num_ble_links: Maximum number of simultaneous connections that the device
 * will support.
 */
#define BLE_CSIP_SET_COORDINATOR_TOTAL_BUFFER_SIZE(num_ble_links) \
          ((CSIP_SET_COORDINATOR_MEM_PER_CONN_SIZE_BYTES * num_ble_links) + \
           (CSIP_SET_COORDINATOR_MEM_CIS_INSTANCE_SIZE_BYTES * num_ble_links) + \
           (DIVC((2u * num_ble_links),4u) * 4u))

/*
 * BLE_CSIP_SET_MEMBER_TOTAL_BUFFER_SIZE: this macro returns the amount of memory, in bytes,needed for
 * the storage of data structures in CSIP Set Member  whose size depends on the number of supported oordinated Set
 * Identification Instances.
 *
 * @param num_csi_instances: Maximum number of supported Coordinated Set Identification Instances
 *
 * @param num_ble_links: Maximum number of simultaneous connections that the device
 * will support.
 *
 */
#define BLE_CSIP_SET_MEMBER_TOTAL_BUFFER_SIZE(num_csi_instances,num_ble_links) \
          ((CSIP_SET_MEMBER_MEM_PER_INSTANCE_SIZE_BYTES * num_csi_instances) + \
          (CSIP_SERV_INST_MEM_PER_CONN_SIZE_BYTES * (num_csi_instances * num_ble_links)) + \
          (CSIP_NVM_MGMT_PER_CONN_SIZE_BYTES * num_ble_links))

/* #############################################################################
   #       Defines and MACRO used to allocate memory resource required to      #
   #       store information in Non Volatile Memory.                           #
   ############################################################################ */

/*
 * BLE_CSIP_SET_MEMBER_DB_BUFFER_SIZE: this macro returns the maximum amount of memory, in bytes, needed for the storage
 * in Non Volatile Memory of the Coordinated Set Identification Profile in Set Member role.
 *
 * @param num_db_devices: Maximum number of device to store in NVM
 *
 * @param num_csi_instances: Maximum number of supported Coordinated Set Identification Instances
 */
#define BLE_CSIP_SET_MEMBER_DB_BUFFER_SIZE(num_db_devices,num_csi_instances) \
        (num_db_devices \
         * (16u + (DIVC((num_csi_instances * 39u),4u) * 4u)))

/*
 * BLE_CSIP_SET_COORDINATOR_DB_BUFFER_SIZE: this macro returns the maximum amount of memory, in bytes, needed for the
 * storage in Non Volatile Memory of the Coordinated Set Identification Profile in Set Coordinator role.
 *
 * @param num_db_devices: Maximum number of device to store in NVM
 */
#define BLE_CSIP_SET_COORDINATOR_DB_BUFFER_SIZE(num_db_devices) \
        (num_db_devices * 84u)

/* Types ---------------------------------------------------------------------*/
/* Types of mask for roles of the Coordinated Set Identification Profile */
typedef uint8_t CSIP_Role_t;
#define CSIP_ROLE_SET_MEMBER                    (0x01)
#define CSIP_ROLE_SET_COORDINATOR	        (0x02)

/* Types of mask for SIRK over OOB of the Coordinated Set Identification Profile */
typedef uint8_t SIRK_OOB_t;
#define CSIP_SIRK_IS_NOT_OOB                    (0x00)
#define CSIP_SIRK_IS_OOB                        (0x01)

/* Types of mask for type of SIRK of the Coordinated Set Identification Profile */
typedef uint8_t SIRK_Type_t;
#define CSIP_SIRK_ENCRYTED                      (0x00)
#define CSIP_SIRK_PLAINTEXT                     (0x01)

typedef uint8_t CSIP_Error_t;
#define ERROR_CSIP_LOCK_DENIED                  (0x80)
#define ERROR_CSIP_LOCK_RELEASE_NOT_ALLOWED     (0x81)
#define ERROR_CSIP_INVALID_LOCK_VALUE           (0x82)
#define ERROR_CSIP_OOB_SIRK_ONLY                (0x83)
#define ERROR_CSIP_LOCK_ALREADY_GRANTED         (0x84)

typedef uint8_t Lock_Type_t;
#define LOCK_VALUE_UNLOCKED                     (0x01)
#define LOCK_VALUE_LOCKED                       (0x02)

#define TIMEOUT_LOCK                            (30)
#define TIMEOUT_DISCOVER_MEMBERS                (20)

typedef enum
{
  CSIP_COO_ADV_REPORT_NEW_SET_MEMBER_DISCOVERED_EVT,    /* This event is reported by the CSIP Set Coordinator during the
                                                         * Set Members Discovery Procedure when a RSI advertised by a
                                                         * Set Member is successfully resolved and match with the SIRK.
                                                         * The CSIP Set Coordinator shall connect to that Set Member
                                                         * (aci_gap_create_connection) and pair (aci_gap_send_pairing_req).
                                                         * Once Pairing is complete during Set Members Discovery Procedure,
                                                         * the CSIP_SET_COORDINATOR_Linkup() API shall be called to
                                                         * get the value of the SIRK it exposes and if this one
                                                         * is equal to the SIRK of the Coordinated Set of the
                                                         * Discovery Procedure.
                                                         * The CSIP_COO_NEW_SET_MEMBER_DISCOVERED_EVT event will be
                                                         * notified to indicate if the remote device is a Set Member of
                                                         * the Coordinated Set or not.
                                                         */
  CSIP_COO_NEW_SET_MEMBER_DISCOVERED_EVT,               /* This event is reported by the CSIP Set Coordinator during the
                                                         * Set Members Discovery Procedure when a Set Member discovery
                                                         * is complete.
                                                         * If status is BLE_STATUS_SUCCESS, the Set Member is considered
                                                         * as discovered (connected, paired, and SIRK matching).
                                                         * If the  status is BLE_STATUS_TIMEOUT, it means that connection
                                                         * or pairing is not complete during the procedure after
                                                         * CSIP_COO_ADV_REPORT_NEW_SET_MEMBER_DISCOVERED_EVT
                                                         * report.
                                                         * If the status is BLE_STATUS_FAILED, it means that SIRK
                                                         * has not been found or mismatch with the SIRK of the
                                                         * Coordinated Set of the Discovery Procedure.
                                                         */
  CSIP_COO_SET_MEMBER_DISCOVERY_PROCEDURE_COMPLETE_EVT, /* This event is reported by the CSIP Set Coordinator when the
                                                         * Set Members Discovery Procedure is complete ( All Set Members
                                                         * are discovered (BLE_STATUS_SUCCESS) or the discovery timer
                                                         * has expired (BLE_STATUS_TIMEOUT))
                                                         */
  CSIP_COO_LOCKED_EVT,                                  /* This event is reported once the CSIP Set Coordinator has
                                                         * unlocked 1 Set Member
                                                         */
  CSIP_COO_LOCKED_COMPLETE_EVT,                         /* This event is reported once the CSIP Set Coordinator has
                                                         * locked all Set Members
                                                         * with BLE_STATUS_SUCCESS if successful
                                                         * with BLE_STATUS_FAILED if some are already locked and the
                                                         * Unlock will be start automatically.
                                                         */
  CSIP_COO_UNLOCKED_EVT,                                /* This event is reported once the CSIP Set Coordinator has
                                                         * locked 1 Set Member
                                                         */
  CSIP_COO_UNLOCKED_COMPLETE_EVT,                       /* This event is reported once the CSIP Set Coordinator has
                                                         * unlocked all Set Members
                                                         */
  CSIP_COO_ORDERED_ACCESS_COMPLETE_EVT,                 /* This event is reported once the CSIP Set Coordinator has
                                                         * checked that all Set Members are unlocked
                                                         */
  CSIP_COO_LOCK_NOTIFIED_EVT,                           /* This event is reported to the Set Coordinator 
                                                         * if the Set Member's Lock value has changed.
                                                         * the Set Coordinator may register on this event when any 
                                                         * procedure has failed because the Set Member is locked
                                                         */
  CSIP_COO_SIRK_NOTIFIED_EVT,                           /* This event is reported to the Set Coordinator 
                                                         * if the Set Member's SIRK has changed. 
                                                         * the Set Coordinator should execute the Set Member Discovery
                                                         */
  CSIP_COO_SET_SIZE_NOTIFIED_EVT,                       /* This event is reported to the Set Coordinator 
                                                         * if the Set Member's Set Size has changed.
                                                         * the Set Coordinator should execute the Set Member Discovery
                                                         */
  CSIP_MEM_LOCKED_EVT,                                  /* This event is reported once the CSIP Set Member is locked by
                                                         * the set Coordiantor
                                                         */
  CSIP_MEM_UNLOCKED_EVT,                                /* This event is reported once the CSIP Set Member is unlocked by
                                                         * the set Coordiantor
                                                         */

} CSIP_NotCode_t;

/* CSIP Set_Member Configurration  */
typedef struct
{
  uint8_t               *pStartRamAddr;                 /* Start address of the RAM buffer allocated for CCP Client
                                                         * Context memory resource.
                                                         * It must be a 32bit aligned RAM area.
                                                         */
  uint16_t              RamSize;                        /* Size of the RAM allocated at pStartRamAddr
                                                         * pointer.
                                                         */
  uint8_t               MaxNumCSISInstances;
} CSIP_Set_Member_Config_t;


/* CSIP Set_Coordinator Configurration  */
typedef struct
{
  uint8_t               *pStartRamAddr;                 /* Start address of the RAM buffer allocated for CCP Client
                                                         * Context memory resource.
                                                         * It must be a 32bit aligned RAM area.
                                                         */
  uint16_t              RamSize;                        /* Size of the RAM allocated at pStartRamAddr
                                                         * pointer.
                                                         */
} CSIP_Set_Coordinator_Config_t;


typedef struct
{
  CSIP_Role_t                           Role;                           /* Mask of CSIP role */
  uint8_t                               MaxNumBleLinks;                 /* Maximum number of BLE Links */
  CSIP_Set_Member_Config_t              Set_Member_Config;              /* CSIP Set_Member Configurration  */
  CSIP_Set_Coordinator_Config_t         Set_Coordinator_Config;         /* CSIP Set_Coordinator Configurration */
} CSIP_Config_t;


typedef struct
{
  SIRK_OOB_t SIRK_OOB;
  uint8_t SIRK_type;
  uint8_t SIRK[16];
  uint8_t Size;
  uint8_t Rank;
  uint8_t SIRK_notifiable;
  uint8_t size_notifiable;
} CSIS_ServiceInit_t;

typedef struct
{
  CSIP_NotCode_t EvtOpcode;      /* Opcode of the notified event */
  tBleStatus     Status;         /* Status associated to the event */
  uint16_t       ConnHandle;     /* ACL Connection Handle associated to the event */
  uint8_t        *pInfo;         /* Pointer on information associated to the event */

}CSIP_Notification_Evt_t;

typedef struct
{
  uint8_t AddressType;
  uint8_t* Address;
} CSIP_New_Member_Evt_Params_t;

typedef struct
{
  uint8_t Size;
} CSIP_Set_Size_Notif_Evt_Params_t;

typedef struct
{
  uint8_t SIRK_type;
  uint8_t SIRK[16];
} CSIP_SIRK_Notif_Evt_Params_t;

typedef struct
{
  Lock_Type_t Lock_Value;
} CSIP_Lock_Notif_Evt_Params_t;

#ifdef __cplusplus
}
#endif

#endif /* __CSIP_TYPES_H */
