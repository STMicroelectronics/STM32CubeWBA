/**
 ******************************************************************************
 * @file    ble_audio_stack.h
 * @author  MCD Application Team
 * @brief   Bluetooth Low Energy Audio Stack Interface
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
#ifndef BLE_AUDIO_STACK_H
#define BLE_AUDIO_STACK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ble_types.h"


/* Defines ---------------------------------------------------------------------*/

/*
 * BLE_AUDIO_PER_LINK_SIZE_BYTES: additional memory size used per link
 */
#define BLE_AUDIO_PER_LINK_SIZE_BYTES           (24u)

/*
 * BLE_AUDIO_PER_EATT_BEARER_SIZE_BYTES: additional memory size used per EATT Bearer
 */
#define BLE_AUDIO_PER_EATT_BEARER_SIZE_BYTES    (8u)

/*
 * BLE_AUDIO_TOTAL_BUFFER_SIZE: this macro returns the amount of memory, in bytes,
 * needed for the storage of data structures whose size depends on the number of
 * supported connections.
 *
 * @param n_link: Maximum number of simultaneous connections that the device
 * will support.
 */
#define BLE_AUDIO_TOTAL_BUFFER_SIZE(n_link,n_eatt_bearer_per_link) \
                  ((BLE_AUDIO_PER_LINK_SIZE_BYTES * (n_link)) \
                    + (BLE_AUDIO_PER_EATT_BEARER_SIZE_BYTES * n_link * n_eatt_bearer_per_link))

/*
 * BLE_AUDIO_DB_BUFFER_SIZE: this macro returns the amount of memory, in bytes,
 * needed for the storage of information related to an Audio Service in a Non Volatile Memory.
 *
 * @param num_db_devices: Maximum number of device to store in NVM
 *
 * @param data_info_size: Information related to an Audio Service
 */
#define BLE_AUDIO_DB_BUFFER_SIZE(num_db_devices,data_info_size) \
                  (num_db_devices * (16u + (DIVC(data_info_size,4u) * 4u)))

/* Types ---------------------------------------------------------------------*/

/* Enumerated values used for the 'type' of Audio Database functions (BLE_AUDIO_STACK_DB_XXX):
 */
typedef enum
{
  BLE_AUDIO_DB_TYPE_TMAP = 0,
  BLEAUDIO_PLAT_NVM_TYPE_HAP_HA = 1,
  BLEAUDIO_PLAT_NVM_TYPE_HAP_HARC = 2,
  BLEAUDIO_PLAT_NVM_TYPE_HAP_IAC = 3,
  BLE_AUDIO_DB_TYPE_GMAP = 4,
} BLE_Audio_DB_Type;

/* Definition of Bonded_Device_Entry_t */
typedef struct
{
  /* Address type :
   * - 0x00: Public Device Address
   * - 0x01: Random Device Address
   */
  uint8_t AddressType;

  /* Public Device Address or Random Device Address.*/
  uint8_t Address[6u];

} BLE_Audio_Bonded_Device_Entry_t;

/*
 * This structure contains memory and configuration data for the Audio device
*/
typedef struct
{

  /* Start address of the RAM buffer allocated for BLE Audio library.
   * It must be a 32bit aligned RAM area.
   */
  uint8_t*      bleAudioStartRamAddress;

  /* Size of the RAM buffer allocated for BLE Audio library.
   * (could be filled with BLE_AUDIO_TOTAL_BUFFER_SIZE return value)
   */
  uint32_t      total_buffer_size;

  /* Maximum number of simultaneous connections that the device will support.
   * (n_link parameter used in the calculation of BLE_AUDIO_TOTAL_BUFFER_SIZE).
   */
  uint8_t       NumOfLinks;

  /* Maximum number of EATT Bearers supported per ACL Link.
   * (n_eatt_bearer_per_link parameter used in the calculation of BLE_AUDIO_TOTAL_BUFFER_SIZE).
   */
  uint8_t       NumOfEATTBearersPerLink;

  /* Maximum number of Bonded Devices to store in Non-Volatile Memory the BLE Audio Information
   */
  uint8_t       MaxNumOfBondedDevices;

  /* Start address of the RAM buffer allocated for BLE Audio Information Storage process in Non-Volatile Memory.
   */
  uint64_t*     bleAudioStartRamAddress_NVM;

  /* Size of the RAM buffer allocated for BLE Audio Information Storage process in Non-Volatile Memory.
   * (could be filled according to Macro defined for each profiles of the Generic Audio Profile and BLE_AUDIO_DB_BUFFER_SIZE)
   */
  uint32_t      total_buffer_size_NVM;

} BleAudioInit_t;

/**
  * @brief BLE Audio Stack initialization.
  * @note  This function shall be called before any Audio Profile function
  */
extern tBleStatus BLE_AUDIO_STACK_Init(BleAudioInit_t *pBleAudioInit);


/**
  * @brief Return a pointer to the Firmware Version String
  * @retval A pointer to the Firmware Version String
  */
extern uint8_t* BLE_AUDIO_STACK_GetFwVersion(void);

/**
  * @brief The BLE Audio Stack requests to be executed. BLE_AUDIO_STACK_Task() shall be called.
  * @note  If the Host task is executed in a separate thread, the thread shall be unlocked here.
  */
extern void BLE_AUDIO_STACK_NotifyToRun(void);

/**
  * @brief This function is used to run the BLE Audio Stack (may following a call by the stack of BLE_AUDIO_STACK_Task()).
  * @note  It could be called by a thread or by a scheduler. If the BLE Audio stack itself need to run it call the
  *        BLE_AUDIO_STACK_NotifyToRun() interface to inform application that it need to call BLE_AUDIO_STACK_Task().
  * @note  This function shall not be called it the BLE Audio stack context. It could be called by a thread
  *        or by a scheduler.
  */
extern void BLE_AUDIO_STACK_Task(void);

/**
  * @brief This function is used to configure the BLE Audio Stack to subscribe it to EATT Bearers .
  * @note  The subscribed EATT Bearers will be used by the BLE Audio Stack to perform ATT procedure
  * @param pChannel_Index : Table of Channel Index of the EATT Bearer.
  * @param NumEATTBearers : Number of subscribed EATT Bearers
  * @return Status of the operation
  */
extern tBleStatus BLE_AUDIO_STACK_EATT_SubscribeBearers(uint8_t *pChannel_Index,uint8_t NumEATTBearers);

/**
  * @brief This function is used to check if the specified EATT Bearer is subscribed in the the BLE Audio Stack.
  * @param Channel_Index : Channel Index of the EATT Bearer.
  * @return BLE_STATUS_SUCCESS if the EATT Bearer is subscribed in the the BLE Audio Stack, else BLE_STATUS_FAILED
  */
extern tBleStatus BLE_AUDIO_STACK_EATT_IsSubscribedBearer(uint8_t Channel_Index);

/**
  * @brief This function is used to get the number of subscribed EATT Bearers in the the BLE Audio Stack for a
  *        specified ACL Link.
  * @param ACL_ConnHandle : Connection Handle of the ACL Link.
  * @return Number of subscribed EATT Bearers
  */
extern uint8_t BLE_AUDIO_STACK_EATT_GetNumSubscribedBearers(uint16_t ACL_ConnHandle);

/**
  * @brief This function is used to get an available EATT Bearer which was reserved for the BLE Audio Stack
  *        thanks to the BLE_AUDIO_STACK_EATT_SubscribeBearers().
  * @param ACL_ConnHandle : Connection Handle of the ACL Link.
  * @param Channel_Index : returned Channel Index of the available EATT Bearer.
  * @return Value indicating success or error code.
  */
extern tBleStatus BLE_AUDIO_STACK_EATT_GetAvailableBearer(uint16_t ACL_ConnHandle,uint8_t *Channel_Index);

/**
  * @brief This function is used to register an ATT procedure for a specified EATT Bearer which was reserved for the
  *        BLE Audio Stack thanks to the BLE_AUDIO_STACK_EATT_SubscribeBearers().
  * @note The BLE_AUDIO_STACK_EATT_UnregisterATTProcedure() shall be called once ATT procedure is complete in order
  *       to free the EATT Bearer for another ATT procedure.
  * @param Channel_Index : Channel Index of the EATT Bearer.
  * @param ProcedureID : ATT Procedure Identifier (could be specific for each Audio Use Case) depending to the developer
  *                      implementation. Identifier value must be > 0.
  * @return Value indicating success or error code:
  *         BLE_STATUS_SUCCESS : Procedure is registered for the specified EATT Bearer.
  *         BLE_STATUS_BUSY : The specified EATT Bearer is already used for an ATT Procedure
  *         BLE_STATUS_INVALID_PARAMS : The specified EATT Bearer doesn't exist or not subscribed for LE Audio Stack, or
  *                                     ProcedureID is not valid.
  */
extern tBleStatus BLE_AUDIO_STACK_EATT_RegisterATTProcedure(uint8_t Channel_Index,uint8_t ProcedureID);

/**
  * @brief This function is used to unregister an ATT procedure for a specified EATT Bearer which was reserved for the
  *        BLE Audio Stack thanks to the BLE_AUDIO_STACK_EATT_SubscribeBearers().
  * @param Channel_Index : Channel Index of the EATT Bearer.
  * @param ProcedureID : ATT Procedure Identifier (could be specific for each Audio Use Case) depending to the developer
  *                      implementation. Identifier value must be > 0.
  * @return Value indicating success or error code:
  *         BLE_STATUS_SUCCESS : Procedure is unregistered for the specified EATT Bearer.
  *         BLE_STATUS_INVALID_PARAMS : The specified EATT Bearer doesn't exist or not subscribed for LE Audio Stack, or
  *                                     ProcedureID is not registered for the specified EATT Bearer or not valid.
  */
extern tBleStatus BLE_AUDIO_STACK_EATT_UnregisterATTProcedure(uint8_t Channel_Index,uint8_t ProcedureID);

/**
  * @brief This function is used to check if an ATT procedure is registered for a specified EATT Bearer.
  * @note If the 'ProcedureID' is setting to 0 and the function returns BLE_STATUS_SUCCESS, it means that an internal
  *       or external ATT procedure of the Generic Audio Framework is registered for the EATT Bearer.
  * @param Channel_Index : Channel Index of the EATT Bearer.
  * @param ProcedureID : ATT Procedure Identifier (could be specific for each Audio Use Case) depending to the developer
  *                      implementation.
  * @return Value indicating success or error code:
  *         BLE_STATUS_SUCCESS : The specified ATT Procedure is registered for the specified EATT Bearer.
  *         BLE_STATUS_FAILED : The specified ATT Procedure is registered for the specified EATT Bearer.
  *         BLE_STATUS_INVALID_PARAMS : The specified EATT Bearer doesn't exist or not subscribed for LE Audio Stack
  */
extern tBleStatus BLE_AUDIO_STACK_EATT_IsBearerRegisteredATTProcedure(uint8_t Channel_Index,uint8_t ProcedureID);

/**
  * @brief This function is used to check if an ATT procedure is registered for one of the subscribed EATT Bearers.
  * @param ACL_ConnHandle : Connection Handle of the ACL Link.
  * @param ProcedureID : ATT Procedure Identifier (could be specific for each Audio Use Case) depending to the developer
  *                      implementation.Shall be > 0
  * @return Value indicating success or error code:
  *         BLE_STATUS_SUCCESS : The specified ATT Procedure is registered for an EATT Bearer.
  *         BLE_STATUS_FAILED : The specified ATT Procedure is not registered for an EATT Bearer.
  */
extern tBleStatus BLE_AUDIO_STACK_EATT_IsRegisteredATTProcedure(uint16_t ACL_ConnHandle,uint8_t ProcedureID);

/**
  * @brief This function is used to register an ATT procedure for the ATT Bearer associated to the ACL Link
  * @note The BLE_AUDIO_STACK_ATT_UnregisterATTProcedure() shall be called once ATT procedure is complete in order
  *       to free the ATT Bearer for another ATT procedure.
  * @param ACL_ConnHandle : Connection Handle of the ACL Link.
  * @param ProcedureID : ATT Procedure Identifier (could be specific for each Audio Use Case) depending to the developer
  *                      implementation. Identifier value must be > 0.
  * @return Value indicating success or error code:
  *         BLE_STATUS_SUCCESS : Procedure is registered for the specified EATT Bearer.
  *         BLE_STATUS_BUSY : The ATT Bearer is already used for an ATT Procedure
  *         BLE_STATUS_INVALID_PARAMS : The specified EATT Bearer doesn't exist or not subscribed for LE Audio Stack, or
  *                                     ProcedureID is not valid.
  */
extern tBleStatus BLE_AUDIO_STACK_ATT_RegisterATTProcedure(uint16_t ACL_ConnHandle,uint8_t ProcedureID);

/**
  * @brief This function is used to unregister an ATT procedure for the ATT Bearer associated to the ACL Link
  * @param ACL_ConnHandle : Connection Handle of the ACL Link.
  * @param ProcedureID : ATT Procedure Identifier (could be specific for each Audio Use Case) depending to the developer
  *                      implementation. Identifier value must be > 0.
  * @return Value indicating success or error code:
  *         BLE_STATUS_SUCCESS : Procedure is unregistered for the specified EATT Bearer.
  *         BLE_STATUS_INVALID_PARAMS : The ATT Bearer doesn't exist or not subscribed for LE Audio Stack, or
  *                                     ProcedureID or ACL_ConnHandle are not valid.
  */
extern tBleStatus BLE_AUDIO_STACK_ATT_UnregisterATTProcedure(uint16_t ACL_ConnHandle,uint8_t ProcedureID);

/**
  * @brief This function is used to check if an ATT procedure is registered for the ATT Bearer associated to the ACL Link
  * @note If the 'ProcedureID' is setting to 0 and the function returns BLE_STATUS_SUCCESS, it means that an internal
  *       or external ATT procedure of the Generic Audio Framework is registered for the ATT Bearer
  * @param ACL_ConnHandle : Connection Handle of the ACL Link.
  * @param ProcedureID : ATT Procedure Identifier (could be specific for each Audio Use Case) depending to the developer
  *                      implementation.
  * @return Value indicating success or error code:
  *         BLE_STATUS_SUCCESS : The specified ATT Procedure is registered for the ATT Bearer.
  *         BLE_STATUS_FAILED : The specified ATT Procedure is not registered for the ATT Bearer.
  */
extern tBleStatus BLE_AUDIO_STACK_ATT_IsRegisteredATTProcedure(uint16_t ACL_ConnHandle,uint8_t ProcedureID);

/**
  * @brief This function is called by the Audio Stack when an ATT procedure associated to ATT Bearer or EATT Bearer
  *        is registered or unregistered by the Audio Stack for an internal GATT Client operation.
  * @note This function should be implemented in application layer in order to be notified when ATT procedure allocation
  *      state change.
  * @param ConnHandle :  Specifies the ATT bearer for which the ATT procedure applies.
  *        Values:
  *        - 0x0000 ... 0x0EFF: Unenhanced ATT bearer (the parameter is the ACL connection handle)
  *        - 0xEA00 ... 0xEA3F: Enhanced ATT bearer (the LSB-byte of the parameter is the connection-oriented channel index)
  * @param State : ATT Procedure Registration State :
  *        - 0x00: No ATT Procedure is registered for the associated ATT/EATT Bearer
  *        - 0x01: An ATT Procedure is registered for the associated ATT/EATT Bearer
  */
extern void BLE_AUDIO_STACK_ATTProcedureRegistrationUpdated(uint16_t ConnHandle,uint8_t State);

/**
  * @brief This function is used to clear all the records saved in the Database reserved to the Audio Profile information.
  * @note  The BLE_AUDIO_STACK_Init() shall be called previously.
  * @return Value indicating success or error code.
  */
extern tBleStatus BLE_AUDIO_STACK_DB_ClearAllRecords( void );

/**
  * @brief This function is used to add a record related to an Audio Information type and a Bluetooth device in the
  *        Database reserved to the Audio Profile information.
  * @note  The BLE_AUDIO_STACK_Init() shall be called previously.
  * @param Type : Audio Information type (see BLE_Audio_DB_Type).
  * @param PeerAddressType : Type of the Bluetooth Address of the remote device
  * @param pPeerAddress : Bluetooth Address of the remote device
  * @param pData : Pointer on data buffer related to the record
  * @param Size : size in bytes of the specified data buffer
  * @return Value indicating success or error code.
  */
extern tBleStatus BLE_AUDIO_STACK_DB_AddRecord(uint8_t Type,
                                               uint8_t PeerAddressType,
                                               const uint8_t* pPeerAddress,
                                               const uint8_t* pData,
                                               uint16_t Size);

/**
  * @brief This function is used to check if a record related to an Audio Information type and a Bluetooth device is
  *        present in the Database reserved to the Audio Profile information.
  * @note  The BLE_AUDIO_STACK_Init() shall be called previously.
  * @param Type : Audio Information type (see BLE_Audio_DB_Type).
  * @param PeerAddressType : Type of the Bluetooth Address of the remote device
  * @param pPeerAddress : Bluetooth Address of the remote device
  * @return Value indicating success or error code.
  */
extern tBleStatus BLE_AUDIO_STACK_DB_FindRecord(uint8_t Type,
                                                uint8_t PeerAddressType,
                                                const uint8_t* pPeerAddress);

/**
  * @brief This function is used to get a record related to an Audio Information type and a Bluetooth device from the
  *        Database reserved to the Audio Profile information.
  * @note  The BLE_AUDIO_STACK_Init() shall be called previously.
  * @param Type : Audio Information type (see BLE_Audio_DB_Type).
  * @param PeerAddressType : Type of the Bluetooth Address of the remote device
  * @param pPeerAddress : Bluetooth Address of the remote device
  * @param pData : Pointer on data buffer related to the record to fill out
  * @param MaxSize : maximum size, in bytes, in data buffer
  * @param Size : size in bytes of the specified data buffer
  * @return Value indicating success or error code.
  */
extern tBleStatus BLE_AUDIO_STACK_DB_GetRecord(uint8_t Type,
                                               uint8_t PeerAddressType,
                                               const uint8_t* pPeerAddress,
                                               uint8_t* pData,
                                               uint16_t MaxSize,
                                               uint16_t *Size);

/**
  * @brief This function is used to remove a record related to an Audio Information type and a Bluetooth device present
  *        in the Database reserved to the Audio Profile information.
  * @note  The BLE_AUDIO_STACK_Init() shall be called previously.
  * @param Type : Audio Information type (see BLE_Audio_DB_Type).
  * @param PeerAddressType : Type of the Bluetooth Address of the remote device
  * @param pPeerAddress : Bluetooth Address of the remote device
  * @return Value indicating success or error code.
  */
extern tBleStatus BLE_AUDIO_STACK_DB_RemoveRecord(uint8_t Type,
                                                  uint8_t PeerAddressType,
                                                  const uint8_t* pPeerAddress);

/**
  * @brief This function is used to get the number of Devices present in the Database reserved to the Audio Profile
  *         information.
  *        The Audio Profile information are associated to the profiles of the Generic Audio Framework saved by the
  *        Audio IP itself and also to the types saved thanks to the BLE_AUDIO_STACK_DB_AddRecord() function.
  * @note  The BLE_AUDIO_STACK_Init() shall be called previously.
  * @return Number of bonded devices
  */
extern uint8_t BLE_AUDIO_STACK_DB_GetNumBondedDevices(void);

/**
  * @brief This function is used to get the Devices present in the Database reserved to the Audio Profile information.
  *        The Audio Profile information are associated to the profiles of the Generic Audio Framework saved by the
  *        Audio IP itself and also to the types saved thanks to the BLE_AUDIO_STACK_DB_AddRecord() function.
  * @note  The BLE_AUDIO_STACK_Init() shall be called previously.
  * @param NumBondedDevices : number of found bonded devices
  * @param pBondedDevice : pointer on a BLE_Audio_Bonded_Device_Entry_t table filled by the function with each found
  *                        bonded devices.
  * @param MaxNumBondedDevices : Maximum number of devices to set in the  BLE_Audio_Bonded_Device_Entry_t table.
  * @return Value indicating success or error code.
  */
extern tBleStatus BLE_AUDIO_STACK_DB_GetBondedDevices(uint8_t *NumBondedDevices,
                                                      BLE_Audio_Bonded_Device_Entry_t *pBondedDevice,
                                                      uint8_t MaxNumBondedDevices);

/**
  * @brief This function is used to check if the specified Bluetooth Device is present in the Database reserved to the
  *        Audio Profile information.
  * @note  The BLE_AUDIO_STACK_Init() shall be called previously.
  * @param PeerAddressType : Type of the Bluetooth Address of the remote device
  * @param pPeerAddress : Bluetooth Address of the remote device
  * @return Value indicating success or error code.
  */
extern tBleStatus BLE_AUDIO_STACK_DB_CheckBondedDevice(uint8_t PeerAddressType,
                                                       const uint8_t* pPeerAddress);

/**
  * @brief This function is used to remove all Audio Informations associated to the Bonded Bluetooth device present
  *        in the Database reserved to the Audio Profile information.
  *        The Audio Profile information are associated to the profiles of the Generic Audio Framework saved by the
  *        Audio IP itself and also to the types saved thanks to the BLE_AUDIO_STACK_DB_AddRecord() function.
  * @note  The BLE_AUDIO_STACK_Init() shall be called previously.
  * @param PeerAddressType : Type of the Bluetooth Address of the remote device
  * @param pPeerAddress : Bluetooth Address of the remote device
  * @return Value indicating success or error code.
  */
extern tBleStatus BLE_AUDIO_STACK_DB_RemoveBondedDevice(uint8_t PeerAddressType,
                                                        const uint8_t* pPeerAddress);
#ifdef __cplusplus
}
#endif

#endif /*BLE_AUDIO_STACK_H*/
