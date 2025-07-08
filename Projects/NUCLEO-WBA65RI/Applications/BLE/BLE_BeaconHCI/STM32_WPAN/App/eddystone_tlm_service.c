/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    eddystone_tlm_service.c
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
#include "app_common.h"
#include "app_ble.h"
#include "cmsis_compiler.h"
#include "ble_core.h"
#include "eddystone_beacon.h"
#include "eddystone_url_service.h"
#include "eddystone_uid_service.h"
#include "system_stm32wbaxx.h"
#include "eddystone_tlm_service.h"
#include "stm32_timer.h"

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  uint16_t AdvertisingInterval;/*!< Specifies the desired advertising interval. */
  uint8_t  TLM_Version;        /*!< Specifies the version of the TLM frame. */
  uint16_t BatteryVoltage;     /*!< Specifies the battery voltage, in 1 mV/bit. */
  uint16_t BeaconTemperature;  /*!< Specifies the beacon temperature, in Signed 8.8 Fixed Point notation. */
  uint32_t AdvertisingCount;   /*!< Specifies the running count of all advertisement frames. */
  uint32_t Uptime;             /*!< Specifies the time sinbe the beacon was powered-up or rebooted. */
} EddystoneTLM_InitTypeDef;

/* Private types -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADVERTISING_INTERVAL_INCREMENT                                      (16)
#define DEFAULT_BEACON_SEC                                                (1000)  /**< 1s */

/* Private variables ---------------------------------------------------------*/
uint8_t tlm_adv;
UTIL_TIMER_Object_t TimerTLM_Id;
#if (CFG_BEACON_TYPE & CFG_EDDYSTONE_URL_BEACON_TYPE)
EddystoneURL_InitTypeDef EddystoneURL_InitStruct;
#elif  (CFG_BEACON_TYPE & CFG_EDDYSTONE_UID_BEACON_TYPE)
EddystoneUID_InitTypeDef EddystoneUID_InitStruct;
#endif
EddystoneTLM_InitTypeDef EddystoneTLM_InitStruct;

/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static tBleStatus EddystoneTLM_Init(EddystoneTLM_InitTypeDef *EddystoneTLM_Init)
{
  tBleStatus ret = BLE_STATUS_SUCCESS;
  uint16_t AdvertisingInterval = (EddystoneTLM_Init->AdvertisingInterval * ADVERTISING_INTERVAL_INCREMENT / 10);
  uint8_t service_data[] =
  {
    2,                                                                       /*< Length. */
    AD_TYPE_FLAGS,                                                           /*< Flags data type value. */
    (FLAG_BIT_LE_GENERAL_DISCOVERABLE_MODE | FLAG_BIT_BR_EDR_NOT_SUPPORTED), /*< BLE general discoverable, without BR/EDR support. */
    3,                                                                      /*< Length. */
    AD_TYPE_16_BIT_SERV_UUID_CMPLT_LIST,                                    /*< Complete list of 16-bit Service UUIDs data type value. */
    0xAA, 0xFE,                                                              /*< 16-bit Eddystone UUID. */
    17,                                                                      /*< Length. */
    AD_TYPE_SERVICE_DATA,                                                    /*< Service Data data type value. */
    0xAA, 0xFE,                                                              /*< 16-bit Eddystone UUID. */
    0x20,                                                                    /*< TLM frame type. */
    (EddystoneTLM_Init->TLM_Version),                                        /*< TLM version. */
    (EddystoneTLM_Init->BatteryVoltage & 0xFF00) >> 8,                       /*< Battery voltage. */
    (EddystoneTLM_Init->BatteryVoltage & 0x00FF),
    (EddystoneTLM_Init->BeaconTemperature & 0xFF00) >> 8,                    /*< Beacon temperature. */
    (EddystoneTLM_Init->BeaconTemperature & 0x00FF),
    (EddystoneTLM_Init->AdvertisingCount & 0xFF000000) >> 24,                /*< Advertising PDU count. */
    (EddystoneTLM_Init->AdvertisingCount & 0x00FF0000) >> 16,
    (EddystoneTLM_Init->AdvertisingCount & 0x0000FF00) >> 8,
    (EddystoneTLM_Init->AdvertisingCount & 0x000000FF),
    (EddystoneTLM_Init->Uptime & 0xFF000000) >> 24,                          /*< Time since power-on or reboot. */
    (EddystoneTLM_Init->Uptime & 0x00FF0000) >> 16,
    (EddystoneTLM_Init->Uptime & 0x0000FF00) >> 8,
    (EddystoneTLM_Init->Uptime & 0x000000FF)
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
  
  return ret;
}

static void EddystoneTLM(void *arg)
{
  tBleStatus ret;
#if (CFG_BEACON_TYPE & CFG_EDDYSTONE_URL_BEACON_TYPE)
  uint8_t UrlScheme     = URL_PREFIX;
  uint8_t Url[]         = PHYSICAL_WEB_URL;

  EddystoneURL_InitStruct.UrlScheme = UrlScheme;
  EddystoneURL_InitStruct.Url = Url;
  EddystoneURL_InitStruct.UrlLength = sizeof(Url) - 1;
#elif (CFG_BEACON_TYPE & CFG_EDDYSTONE_UID_BEACON_TYPE)
  uint8_t NamespaceID[] = { NAMESPACE_ID };
  uint8_t BeaconID[]    = { BEACON_ID };
  
  EddystoneUID_InitStruct.NamespaceID = NamespaceID;
  EddystoneUID_InitStruct.BeaconID = BeaconID;
#endif
  
  if(tlm_adv == TRUE)
  { /* Advertising of TLM */
    ret = hci_le_set_advertising_enable(0x00);

    if (ret != BLE_STATUS_SUCCESS)
    {
      while(1);
    }

    EddystoneTLM_Init(&EddystoneTLM_InitStruct);

    tlm_adv = FALSE;

    /* Wait 1s */
    UTIL_TIMER_StartWithPeriod(&TimerTLM_Id, DEFAULT_BEACON_SEC * 5);
  }
  else
  { /* Advertising of UUID or URL */
    ret = hci_le_set_advertising_enable(0x00);

    if (ret != BLE_STATUS_SUCCESS)
    {
      while(1);
    }

#if (CFG_BEACON_TYPE & CFG_EDDYSTONE_UID_BEACON_TYPE)
    EddystoneUID_Init(&EddystoneUID_InitStruct);
#elif (CFG_BEACON_TYPE & CFG_EDDYSTONE_URL_BEACON_TYPE)
    EddystoneURL_Init(&EddystoneURL_InitStruct);
#endif
    
    tlm_adv = TRUE;
    /* 10s of URL advertise */
    UTIL_TIMER_StartWithPeriod(&TimerTLM_Id, DEFAULT_BEACON_SEC * 5);
  }
}

/* Exported functions --------------------------------------------------------*/
void EddystoneTLM_Process(void)
{
#if(CFG_BEACON_TYPE & CFG_EDDYSTONE_URL_BEACON_TYPE)
  uint8_t UrlScheme     = URL_PREFIX;
  uint8_t Url[]         = PHYSICAL_WEB_URL;

  EddystoneURL_InitStruct.AdvertisingInterval = ADVERTISING_INTERVAL_IN_MS;
  EddystoneURL_InitStruct.CalibratedTxPower = CALIBRATED_TX_POWER_AT_0_M;
  EddystoneURL_InitStruct.UrlScheme = UrlScheme;
  EddystoneURL_InitStruct.Url = Url;
  EddystoneURL_InitStruct.UrlLength = sizeof(Url) - 1;

  EddystoneURL_Init(&EddystoneURL_InitStruct);
#elif (CFG_BEACON_TYPE & CFG_EDDYSTONE_UID_BEACON_TYPE) 
  uint8_t NamespaceID[] = { NAMESPACE_ID };
  uint8_t BeaconID[]    = { BEACON_ID };

  EddystoneUID_InitStruct.AdvertisingInterval = ADVERTISING_INTERVAL_IN_MS;
  EddystoneUID_InitStruct.CalibratedTxPower = CALIBRATED_TX_POWER_AT_0_M;
  EddystoneUID_InitStruct.NamespaceID = NamespaceID;
  EddystoneUID_InitStruct.BeaconID = BeaconID;
  
  EddystoneUID_Init(&EddystoneUID_InitStruct);
#endif
  
  EddystoneTLM_InitStruct.AdvertisingInterval = ADVERTISING_INTERVAL_IN_MS;
  EddystoneTLM_InitStruct.TLM_Version       = 0;
  EddystoneTLM_InitStruct.BatteryVoltage = 3000;
  EddystoneTLM_InitStruct.BeaconTemperature = 10000;
  EddystoneTLM_InitStruct.Uptime = 2000000;
  EddystoneTLM_InitStruct.AdvertisingCount = 3000000;


  UTIL_TIMER_Create(&(TimerTLM_Id), DEFAULT_BEACON_SEC * 5, UTIL_TIMER_ONESHOT, EddystoneTLM, 0);
  
  tlm_adv = TRUE;
  
  /* 10s of URL advertise */
  UTIL_TIMER_StartWithPeriod(&TimerTLM_Id, DEFAULT_BEACON_SEC * 5);
}
