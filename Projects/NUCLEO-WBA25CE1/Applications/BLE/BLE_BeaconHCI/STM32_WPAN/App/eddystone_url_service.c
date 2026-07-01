/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    eddystone_url_service.c
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
#include "cmsis_compiler.h"
#include "ble_core.h"
#include "app_conf.h"
#include "app_ble.h"
#include "eddystone_beacon.h"
#include "eddystone_url_service.h"

/* Exported types ------------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADVERTISING_INTERVAL_INCREMENT (16)

/* Private variables ---------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
tBleStatus EddystoneURL_Init(EddystoneURL_InitTypeDef *EddystoneURL_Init)
{
  tBleStatus ret;
  uint16_t AdvertisingInterval = (EddystoneURL_Init->AdvertisingInterval * ADVERTISING_INTERVAL_INCREMENT / 10);
  uint8_t URL_data[] =
  {
    2,                                                                       /*< Length. */
    AD_TYPE_FLAGS,                                                           /*< Flags data type value. */
    (FLAG_BIT_LE_GENERAL_DISCOVERABLE_MODE | FLAG_BIT_BR_EDR_NOT_SUPPORTED), /*< BLE general discoverable, without BR/EDR support. */
    3,                                                                       /*< Length. */
    AD_TYPE_16_BIT_SERV_UUID_CMPLT_LIST,                                     /*< Complete list of 16-bit Service UUIDs data type value. */
    0xAA, 0xFE,                                                              /*< 16-bit Eddystone UUID. */
    6 + EddystoneURL_Init->UrlLength,                                        /*< Length. */
    AD_TYPE_SERVICE_DATA,                                                    /*< Service Data data type value. */
    0xAA, 0xFE,                                                              /*< 16-bit Eddystone UUID. */
    0x10,                                                                    /*< URL frame type. */
    EddystoneURL_Init->CalibratedTxPower,                                    /*< Ranging data. */
    EddystoneURL_Init->UrlScheme,                                            /*< URL Scheme Prefix is http://www. */
    0x00,                                                                    /*< URL */
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
  };

  /* Disable scan response. */
  hci_le_set_scan_response_data(0, NULL);

  ret = hci_le_set_advertising_parameters( AdvertisingInterval, AdvertisingInterval, /*< Set the advertising interval as 700 ms (0.625 us increment). */
                                           ADV_NONCONN_IND,                          /*< Advertise as non-connectable, undirected. */
                                           CFG_BD_ADDRESS_TYPE,                      /*< Use the public address for Own */
                                           CFG_BD_ADDRESS_TYPE,                      /*< Use the public address for Peer */
                                           BleGetBdAddress(),                        /*< Peer Address */
                                           0x07,                                     /* On channels: 37, 38 and 39 */
                                           0x00                                      /* Allow Scan Request and Connect Request from Any */
                                         );
  if (ret != BLE_STATUS_SUCCESS)
  {
    return ret;
  }

  for (uint8_t i = 0; i < EddystoneURL_Init->UrlLength; ++i)
  {
    URL_data[14 + i] = EddystoneURL_Init->Url[i];
  }

  ret = hci_le_set_advertising_data(sizeof(URL_data),
                                    URL_data);
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

tBleStatus EddystoneURL_Process(void)
{
  uint8_t UrlScheme = URL_PREFIX;
  uint8_t Url[]     = PHYSICAL_WEB_URL;
  EddystoneURL_InitTypeDef EddystoneURL_InitStruct =
  {
    .AdvertisingInterval = ADVERTISING_INTERVAL_IN_MS,
    .CalibratedTxPower = CALIBRATED_TX_POWER_AT_0_M,
    .UrlScheme = UrlScheme,
    .Url = Url,
    .UrlLength = sizeof(Url) - 1
  };

  return(EddystoneURL_Init(&EddystoneURL_InitStruct));
}
