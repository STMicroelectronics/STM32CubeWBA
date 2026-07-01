/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_zigbee_utility.h
  * @author  MCD Application Team
  * @brief   API to manage Zigbee Stack.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_ZIGBEE_UTILITY_H
#define APP_ZIGBEE_UTILITY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

#include "zigbee.h"

/* Global Defines -------------------------------------------------------------*/
/* Allows any provider id as valid/authorized. */
#define ZCL_METER_PROVIDER_ID_ANY                   0xffffffffU

/* Global Structures -----------------------------------------------------------*/

/* Global Variables -----------------------------------------------------------*/

/* Global Functions -----------------------------------------------------------*/

bool      AppZbUtil_RequestNwkAddress           ( struct ZigBeeT * pstZigbee, uint64_t dlExtendedAddress, uint16_t * piNwkAddressOut );
bool      AppZbUtil_RequestExtendedAddress      ( struct ZigBeeT * pstZigbee, uint16_t iNwkAddress, uint64_t * pdlExtendedAddressOut );

uint16_t  AppZbUtil_DisplayBindingTable         ( struct ZigBeeT * pstZigbee, bool bDisplay );

bool      AppZbUtil_RequestMatchDescription     ( struct ZbZclClusterT * pstCluster, struct ZbApsAddrT * pstMatchedList, uint16_t * piListSize );
bool      AppZbUtil_RequestBindWithDevice       ( struct ZbZclClusterT * pstCluster, uint64_t dlMyExtendedAddress, uint64_t dlDeviceExtendedAddress, uint16_t iDeviceAddress, uint8_t cDeviceEnpoint );
bool      AppZbUtil_RequestMyBind               ( struct ZbZclClusterT * pstCluster, uint64_t dlMyExtendedAddress, uint64_t dlDeviceExtendedAddress, uint8_t cDeviceEnpoint );
uint16_t  AppZbUtil_SearchServersOnBindingTable ( struct ZbZclClusterT * pstCluster, struct ZbApsAddrT * pstServerList, uint16_t iNbServerMax );

bool      AppZbUtil_AddDeviceWithInstallCode    ( struct ZigBeeT * pstZigbee, uint64_t dlExtendedAddress, uint8_t * szInstallCode );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // APP_ZIGBEE_UTILITY_H
