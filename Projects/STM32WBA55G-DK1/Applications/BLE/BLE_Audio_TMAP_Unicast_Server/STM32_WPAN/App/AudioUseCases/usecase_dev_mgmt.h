/**
 ******************************************************************************
 * @file    usecase_dev_mgmt.h
 * @author  MCD Application Team
 * @brief   BDevice Management Interface for Use Case Profiles
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

/* Defines -----------------------------------------------------------*/
#define USECASE_DEV_MGMT_MAX_CONNECTION        (2u)

/*Structure used to store Use Case Connection information*/
typedef struct
{
  uint16_t      Connection_Handle;      /*ACL Connection handle*/
  uint8_t       Role;                   /*local device role (0x00 : Master ; 0x01 : Slave)*/
  uint8_t       Peer_Address_Type;      /*Peer address type*/
  uint8_t       Peer_Address[6];        /*Peer address*/
} UseCaseConnInfo_t;

/* Functions -----------------------------------------------------------------*/
/**
  * @brief Use Case Manager initialization.
  * @note  This function shall be called before any Use Case Profile function
  */
tBleStatus USECASE_DEV_MGMT_Init();

/**
  * @brief  Get the number of connected devices
  * @retval number of connected devices
  */
uint8_t USECASE_DEV_MGMT_GetNumConnectedDevices(void);

/**
  * @brief  Get Connection Information corresponding to a specified connection handle
  * @param  ConnHandle: connection handle
  * @param  pConnInfo : pointer on connection information
  * @retval Status of the operation
  */
tBleStatus USECASE_DEV_MGMT_GetConnInfo(uint16_t ConnHandle,const UseCaseConnInfo_t **pConnInfo);

/**
  * @brief  Get Connection Handle corresponding to a specified connection index
  * @param  ConnIndex: connection index
  * @retval Connection Handle (0xFFFF if connection index doesn't correspond to a connected device)
  */
extern uint16_t USECASE_DEV_MGMT_GetConnHandle(uint8_t ConnIndex);

#ifdef __cplusplus
}
#endif

#endif /*USECASE_DEV_MGMT_H*/
