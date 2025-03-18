/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ibeacon_service.c
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
#include "ibeacon.h"
#include "ibeacon_service.h"
#include "app_ble.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef ------------------------------------------------------------*/

typedef struct
{
    uint16_t AdvertisingInterval;/*!< Specifies the desired advertising interval. */
    uint8_t * UuID;              /*!< Specifies the 16-byte UUID to which the beacon belongs. */
    uint8_t * MajorID;           /*!< Specifies the 2-byte beacon identifying information. */
    uint8_t * MinorID;           /*!< Specifies the 2-byte beacon identifying information. */
    uint8_t CalibratedTxPower;   /*!< Specifies the power at 1m. */
} IBeacon_InitTypeDef;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
#define ADVERTISING_INTERVAL_INCREMENT (16)
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/

static tBleStatus IBeacon_Init(IBeacon_InitTypeDef *IBeacon_Init)
{
/* USER CODE BEGIN IBeacon_Init_1 */

/* USER CODE END IBeacon_Init_1 */
  tBleStatus ret = BLE_STATUS_SUCCESS;
  uint16_t AdvertisingInterval = (IBeacon_Init->AdvertisingInterval * ADVERTISING_INTERVAL_INCREMENT / 10);
  uint8_t service_data[] =
  {
    2,                                                                       /*< Length. */
    AD_TYPE_FLAGS,                                                           /*< Flags data type value. */
    (FLAG_BIT_LE_GENERAL_DISCOVERABLE_MODE | FLAG_BIT_BR_EDR_NOT_SUPPORTED), /*< BLE general discoverable, without BR/EDR support. */
    26,                                                                      /*< Length. */
    AD_TYPE_MANUFACTURER_SPECIFIC_DATA,                                      /*< Manufacturer Specific Data data type value. */
    0x4C, 0x00, 0x02, 0x15,                                                  /*< 32-bit Manufacturer Data. */
    IBeacon_Init->UuID[0],                                                   /*< 16-byte Proximity UUID. */
    IBeacon_Init->UuID[1],
    IBeacon_Init->UuID[2],
    IBeacon_Init->UuID[3],
    IBeacon_Init->UuID[4],
    IBeacon_Init->UuID[5],
    IBeacon_Init->UuID[6],
    IBeacon_Init->UuID[7],
    IBeacon_Init->UuID[8],
    IBeacon_Init->UuID[9],
    IBeacon_Init->UuID[10],
    IBeacon_Init->UuID[11],
    IBeacon_Init->UuID[12],
    IBeacon_Init->UuID[13],
    IBeacon_Init->UuID[14],
    IBeacon_Init->UuID[15],
    IBeacon_Init->MajorID[0],                                                /*< 2-byte Major. */
    IBeacon_Init->MajorID[1],
    IBeacon_Init->MinorID[0],                                                /*< 2-byte Minor. */
    IBeacon_Init->MinorID[1],
    IBeacon_Init->CalibratedTxPower,                                         /*< Ranging data. */
  };

  /* Disable scan response. */
  hci_le_set_scan_response_data(0, NULL);

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
/* USER CODE BEGIN IBeacon_Init_2 */

/* USER CODE END IBeacon_Init_2 */
  return ret;
}

void IBeacon_Process(void)
{
/* USER CODE BEGIN IBeacon_Process_1 */

/* USER CODE END IBeacon_Process_1 */
  /* Default ibeacon */
  uint8_t UuID[]    = { UUID };
  uint8_t MajorID[] = { MAJOR_ID };
  uint8_t MinorID[] = { MINOR_ID };

  IBeacon_InitTypeDef IBeacon_InitStruct =
  {
    .AdvertisingInterval = ADVERTISING_INTERVAL_IN_MS,
    .UuID                = UuID,
    .MajorID             = MajorID,
    .MinorID             = MinorID,
    .CalibratedTxPower   = CALIBRATED_TX_POWER_AT_1_M
  };

  IBeacon_Init(&IBeacon_InitStruct);
/* USER CODE BEGIN IBeacon_Process_2 */

/* USER CODE END IBeacon_Process_2 */
}
/* USER CODE BEGIN FD */

/* USER CODE END FD */
