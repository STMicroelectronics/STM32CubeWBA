/**
 ******************************************************************************
 * @file    usecase_dev_mgmt.h
 * @author  MCD Application Team
 * @brief   Device Management Interface for Use Case Profiles
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
#ifndef USECASE_DEV_MGMT_H
#define USECASE_DEV_MGMT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "cmsis_compiler.h"
#include "ble_types.h"
#include "svc_ctl.h"
#include "app_conf.h"
#include "ble_core.h"

/* Defines -----------------------------------------------------------*/
#define USECASE_DEV_MGMT_MAX_CONNECTION        (CFG_BLE_NUM_LINK)

/* ATT Procedure Identifier types*/
#define TMAP_ATT_PROCEDURE_ID                   (0x01)
#define GMAP_ATT_PROCEDURE_ID                   (0x02)
#define HAP_HARC_ATT_PROCEDURE_ID               (0x03)
#define HAP_IAC_ATT_PROCEDURE_ID                (0x04)


/* Types -----------------------------------------------------------*/

/*Structure used to store EATT Bearer information*/
typedef struct
{
  uint8_t       ChannelIdx;             /*Channel Index*/
  uint8_t       State;                  /*0x00: EATT bearer created, 0x01: EATT bearer terminated*/
} BleEATTBearer_t;

/*Structure used to store Use Case Connection information*/
typedef struct
{
  uint16_t      Connection_Handle;      /*ACL Connection handle*/
  uint8_t       Role;                   /*local device role (0x00 : Master ; 0x01 : Slave)*/
  uint8_t       Peer_Address_Type;      /*Peer address type*/
  uint8_t       Peer_Address[6];        /*Peer address*/
  uint16_t      MTU;                    /* MTU Exchanged */
  uint8_t       LinkEncrypted;          /* Link Encrypted */

  /* CSIP Information */
  uint8_t       CSIPDiscovered;         /* Set to 1 if Set Member has been discovered */
  uint8_t       SIRK_type;
  uint8_t       aSIRK[16];
  uint8_t       Rank;
  uint8_t       Size;
#if (CFG_BLE_EATT_BEARER_PER_LINK > 0)
  BleEATTBearer_t aEATTBearer[CFG_BLE_EATT_BEARER_PER_LINK]; /*Table of EATT Bearer associated to the ACL Link */
#endif /* (CFG_BLE_EATT_BEARER_PER_LINK > 0) */
} UseCaseConnInfo_t;

/* Functions -----------------------------------------------------------------*/
/**
  * @brief Use Case Manager initialization.
  * @note  This function shall be called before any Use Case Profile function
  */
tBleStatus USECASE_DEV_MGMT_Init(void);

/**
  * @brief  Get the number of connected devices
  * @retval number of connected devices
  */
uint8_t USECASE_DEV_MGMT_GetNumConnectedDevices(void);

/**
  * @brief  Get ACL Connection Information and potential EATT Bearer Information corresponding to a
  *         specified connection handle
  * @param  ConnHandle: connection handle
  * @param  pConnInfo : pointer on ACL connection information
  * @param  pEATTBearer : pointer on EATT Bearer information
  * @retval Status of the operation
  */
tBleStatus USECASE_DEV_MGMT_GetConnInfo(uint16_t ConnHandle,
                                        UseCaseConnInfo_t **pConnInfo,
                                        BleEATTBearer_t **pEATTBearer);

/**
  * @brief Set CSIP information for ConnHandle
  * @param ConnHandle: Connection Handle of the set member
  * @param pSIRK: Pointer to the SIRC of the set
  * @param SIRKType: Type of SIRC of the set
  * @param Rank: Rank of the set member
  * @param SetSize: Size of the set member
  */
void USECASE_DEV_MGMT_SetCSIPInfo(uint16_t ConnHandle, uint8_t *pSIRK, uint8_t SIRKType, uint8_t Rank, uint8_t SetSize);

/**
 * @brief Update characteristic and indicate selectively the generation of Indication/Notification over ATT Bearer
 *        or EATT Bearer to all subscribed clients
 * @param Service_Handle Handle of service to which the characteristic belongs
 * @param Char_Handle Handle of the characteristic declaration
 * @param Update_Type Allow Notification or Indication generation, if enabled
 *        in the client characteristic configuration descriptor
 *        Flags:
 *        - 0x00: Do not notify
 *        - 0x01: Notification
 *        - 0x02: Indication
 * @param Char_Length Total length of the characteristic value.
 *        In case of a variable size characteristic, this field specifies the
 *        new length of the characteristic value after the update; in case of
 *        fixed length characteristic this field is ignored.
 * @param Value_Offset The offset from which the attribute value has to be
 *        updated.
 * @param Value_Length Length of the Value parameter in octets.
 * @param Value Updated characteristic value
 * @return Value indicating success or error code.
 */
uint8_t USECASE_DEV_MGMT_UpdateCharValue(uint16_t Service_Handle,
                                         uint16_t Char_Handle,
                                         uint8_t Update_Type,
                                         uint16_t Char_Length,
                                         uint16_t Value_Offset,
                                         uint8_t Value_Length,
                                         const uint8_t* Value );

/**
  * @brief  Get Connection Handle corresponding to a specified connection index
  * @param  ConnIndex: connection index
  * @retval Connection Handle (0xFFFF if connection index doesn't correspond to a connected device)
  */
extern uint16_t USECASE_DEV_MGMT_GetConnHandle(uint8_t ConnIndex);

/** @brief This function is used by the Device in the TMAP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */
SVCCTL_EvtAckStatus_t TMAP_GATT_Event_Handler(void *pEvent);

/** @brief This function is used by the Device in the HAP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */
SVCCTL_EvtAckStatus_t HAP_GATT_Event_Handler(void *pEvent);

/** @brief This function is used by the Device in the GMAP Profile to handle GATT Events received
  *        from the BLE core device.
  * @param  Event: Address of the buffer holding the Event
  * @retval Ack: Return whether the Event has been managed or not
  */
SVCCTL_EvtAckStatus_t GMAP_GATT_Event_Handler(void *pEvent);

/**
  * @brief Indicate that connection with specified device is down
  * @param  ConnHandle: connection handle
  */
void TMAP_AclDisconnection(uint16_t ConnHandle);

/**
  * @brief Indicate that connection with specified device is down
  * @param  ConnHandle: connection handle
  */
void HAP_AclDisconnection(uint16_t ConnHandle);

/**
  * @brief Indicate that connection with specified device is down
  * @param  ConnHandle: connection handle
  */
void GMAP_AclDisconnection(uint16_t ConnHandle);

/**
  * @brief Indicate that HCI_ENCRYPTION_CHANGE_EVT_CODE event for a specified device is received
  * @param  ConnHandle: connection handle
  */
void TMAP_LinkEncrypted(uint16_t ConnHandle);

/**
  * @brief Indicate that HCI_ENCRYPTION_CHANGE_EVT_CODE event for a specified device is received
  * @param  ConnHandle: connection handle
  */
void HAP_LinkEncrypted(uint16_t ConnHandle);

/**
  * @brief Indicate that HCI_ENCRYPTION_CHANGE_EVT_CODE event for a specified device is received
  * @param  ConnHandle: connection handle
  */
void GMAP_LinkEncrypted(uint16_t ConnHandle);

#ifdef __cplusplus
}
#endif

#endif /*USECASE_DEV_MGMT_H*/
