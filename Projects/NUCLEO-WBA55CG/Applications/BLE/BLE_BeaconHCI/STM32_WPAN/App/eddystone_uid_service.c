/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    eddystone_uid_service.c
  * @author  MCD Application Team
  * @brief   
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "ble.h"
#include "eddystone_beacon.h"
#include "eddystone_uid_service.h"
#include "app_ble.h"

/* Exported types ------------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADVERTISING_INTERVAL_INCREMENT (16)

/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
tBleStatus EddystoneUID_Init(EddystoneUID_InitTypeDef *EddystoneUID_Init)
{
  tBleStatus ret = BLE_STATUS_SUCCESS;
  uint8_t dummy_buffer[1] = {0};
  uint16_t AdvertisingInterval = (EddystoneUID_Init->AdvertisingInterval * ADVERTISING_INTERVAL_INCREMENT / 10);
  uint8_t service_data[] =
  {
    2,                                                                       /*< Length. */
    AD_TYPE_FLAGS,                                                           /*< Flags data type value. */
    (FLAG_BIT_LE_GENERAL_DISCOVERABLE_MODE | FLAG_BIT_BR_EDR_NOT_SUPPORTED), /*< BLE general discoverable, without BR/EDR support. */
    3,                                                                      /*< Length. */
    AD_TYPE_16_BIT_SERV_UUID_CMPLT_LIST,                                    /*< Complete list of 16-bit Service UUIDs data type value. */
    0xAA, 0xFE,                                                              /*< 16-bit Eddystone UUID. */
    23,                                                                      /*< Length. */
    AD_TYPE_SERVICE_DATA,                                                    /*< Service Data data type value. */
    0xAA, 0xFE,                                                              /*< 16-bit Eddystone UUID. */
    0x00,                                                                    /*< UID frame type. */
    EddystoneUID_Init->CalibratedTxPower,                                    /*< Ranging data. */
    EddystoneUID_Init->NamespaceID[0],                                       /*< 10-byte ID Namespace. */
    EddystoneUID_Init->NamespaceID[1],
    EddystoneUID_Init->NamespaceID[2],
    EddystoneUID_Init->NamespaceID[3],
    EddystoneUID_Init->NamespaceID[4],
    EddystoneUID_Init->NamespaceID[5],
    EddystoneUID_Init->NamespaceID[6],
    EddystoneUID_Init->NamespaceID[7],
    EddystoneUID_Init->NamespaceID[8],
    EddystoneUID_Init->NamespaceID[9],
    EddystoneUID_Init->BeaconID[0],                                         /*< 6-byte ID Instance. */
    EddystoneUID_Init->BeaconID[1],
    EddystoneUID_Init->BeaconID[2],
    EddystoneUID_Init->BeaconID[3],
    EddystoneUID_Init->BeaconID[4],
    EddystoneUID_Init->BeaconID[5],
    0x00,                                                                   /*< Reserved. */
    0x00                                                                    /*< Reserved. */
  };

  /* Send empty scan response data */
  ret = hci_le_set_scan_response_data(0, &dummy_buffer[0]);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("==>> hci_le_set_scan_response_data - fail, result: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_APP("==>> hci_le_set_scan_response_data - Success\n");
  }

  /* Put the device in a non-connectable mode. */
  ret = hci_le_set_advertising_parameters( AdvertisingInterval,
                                           AdvertisingInterval,
                                           ADV_NONCONN_IND,
                                           CFG_BD_ADDRESS_TYPE,
                                           CFG_BD_ADDRESS_TYPE,
                                           BleGetBdAddress(),
                                           0x07, /* On channels: 37, 38 and 39 */ 
                                           0x00  /* Allow Scan Request and Connect Request from Any */
                                         );
  
  if (ret != BLE_STATUS_SUCCESS)
  {
    return ret;
  }

  ret = hci_le_set_advertising_data(sizeof(service_data),
                                    service_data);
  if (ret != BLE_STATUS_SUCCESS)
  {
    return ret;
  }

  ret = hci_le_set_advertising_enable(0x01);

  if (ret != BLE_STATUS_SUCCESS)
  {
    return ret;
  }

  return ret;
}

void EddystoneUID_Process(void)
{
  uint8_t NamespaceID[] = { NAMESPACE_ID };
  uint8_t BeaconID[] = { BEACON_ID };

  EddystoneUID_InitTypeDef EddystoneUID_InitStruct =
  {
    .AdvertisingInterval = ADVERTISING_INTERVAL_IN_MS,
    .CalibratedTxPower = CALIBRATED_TX_POWER_AT_0_M,
    .NamespaceID = NamespaceID,
    .BeaconID = BeaconID
  };

  EddystoneUID_Init(&EddystoneUID_InitStruct);
}
