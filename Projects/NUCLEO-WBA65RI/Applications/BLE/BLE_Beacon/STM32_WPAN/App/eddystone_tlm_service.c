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
#include "cmsis_compiler.h"
#include "ble_core.h"
#include "eddystone_beacon.h"
#include "eddystone_url_service.h"
#include "eddystone_uid_service.h"
#include "system_stm32wbaxx.h"
#include "eddystone_tlm_service.h"
#include "stm32_timer.h"
#include "stm32_seq.h"

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
EddystoneTLM_InitTypeDef EddystoneTLM_InitStruct;

/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void EddystoneTLM(void)
{
  tBleStatus ret;
#if (CFG_BEACON_TYPE & CFG_EDDYSTONE_URL_BEACON_TYPE)
  uint8_t UrlScheme     = URL_PREFIX;
  uint8_t Url[]         = PHYSICAL_WEB_URL;

  EddystoneURL_InitTypeDef EddystoneURL_InitStruct =
  {
    .AdvertisingInterval = ADVERTISING_INTERVAL_IN_MS,
    .CalibratedTxPower = CALIBRATED_TX_POWER_AT_0_M,
    .UrlScheme = UrlScheme,
    .Url = Url,
    .UrlLength = sizeof(Url) - 1
  };
#elif (CFG_BEACON_TYPE & CFG_EDDYSTONE_UID_BEACON_TYPE)
  uint8_t NamespaceID[] = { NAMESPACE_ID };
  uint8_t BeaconID[] = { BEACON_ID };

  EddystoneUID_InitTypeDef EddystoneUID_InitStruct =
  {
    .AdvertisingInterval = ADVERTISING_INTERVAL_IN_MS,
    .CalibratedTxPower = CALIBRATED_TX_POWER_AT_0_M,
    .NamespaceID = NamespaceID,
    .BeaconID = BeaconID
  };
#endif

  if(tlm_adv == TRUE)
  { /* Advertising of TLM */
    uint8_t TLM_data[] =
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
      (EddystoneTLM_InitStruct.TLM_Version),                                        /*< TLM version. */
      (EddystoneTLM_InitStruct.BatteryVoltage & 0xFF00) >> 8,                       /*< Battery voltage. */
      (EddystoneTLM_InitStruct.BatteryVoltage & 0x00FF),
      (EddystoneTLM_InitStruct.BeaconTemperature & 0xFF00) >> 8,                    /*< Beacon temperature. */
      (EddystoneTLM_InitStruct.BeaconTemperature & 0x00FF),
      (EddystoneTLM_InitStruct.AdvertisingCount & 0xFF000000) >> 24,                /*< Advertising PDU count. */
      (EddystoneTLM_InitStruct.AdvertisingCount & 0x00FF0000) >> 16,
      (EddystoneTLM_InitStruct.AdvertisingCount & 0x0000FF00) >> 8,
      (EddystoneTLM_InitStruct.AdvertisingCount & 0x000000FF),
      (EddystoneTLM_InitStruct.Uptime & 0xFF000000) >> 24,                          /*< Time since power-on or reboot. */
      (EddystoneTLM_InitStruct.Uptime & 0x00FF0000) >> 16,
      (EddystoneTLM_InitStruct.Uptime & 0x0000FF00) >> 8,
      (EddystoneTLM_InitStruct.Uptime & 0x000000FF)
    };

    /* Update the service data. */
    ret = aci_gap_update_adv_data(sizeof(TLM_data), TLM_data);

    if (ret != BLE_STATUS_SUCCESS)
    {
      while(1);
    }
    tlm_adv = FALSE;

    /* Wait 1s */
    UTIL_TIMER_StartWithPeriod(&TimerTLM_Id, DEFAULT_BEACON_SEC * 10);
  }
  else
  {
#if (CFG_BEACON_TYPE & CFG_EDDYSTONE_UID_BEACON_TYPE)
    uint8_t UID_data[] =
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
      EddystoneUID_InitStruct.CalibratedTxPower,                                    /*< Ranging data. */
      EddystoneUID_InitStruct.NamespaceID[0],                                       /*< 10-byte ID Namespace. */
      EddystoneUID_InitStruct.NamespaceID[1],
      EddystoneUID_InitStruct.NamespaceID[2],
      EddystoneUID_InitStruct.NamespaceID[3],
      EddystoneUID_InitStruct.NamespaceID[4],
      EddystoneUID_InitStruct.NamespaceID[5],
      EddystoneUID_InitStruct.NamespaceID[6],
      EddystoneUID_InitStruct.NamespaceID[7],
      EddystoneUID_InitStruct.NamespaceID[8],
      EddystoneUID_InitStruct.NamespaceID[9],
      EddystoneUID_InitStruct.BeaconID[0],                                         /*< 6-byte ID Instance. */
      EddystoneUID_InitStruct.BeaconID[1],
      EddystoneUID_InitStruct.BeaconID[2],
      EddystoneUID_InitStruct.BeaconID[3],
      EddystoneUID_InitStruct.BeaconID[4],
      EddystoneUID_InitStruct.BeaconID[5],
      0x00,                                                                   /*< Reserved. */
      0x00                                                                    /*< Reserved. */
    };

    /* Update the service data. */
    ret = aci_gap_update_adv_data(sizeof(UID_data), UID_data);

    if (ret != BLE_STATUS_SUCCESS)
    {
      while(1);
    }
#elif (CFG_BEACON_TYPE & CFG_EDDYSTONE_URL_BEACON_TYPE)
    uint8_t URL_data[] =
    {
      2,                                                                       /*< Length. */
      AD_TYPE_FLAGS,                                                           /*< Flags data type value. */
      (FLAG_BIT_LE_GENERAL_DISCOVERABLE_MODE | FLAG_BIT_BR_EDR_NOT_SUPPORTED), /*< BLE general discoverable, without BR/EDR support. */
      3,                                                                       /*< Length. */
      AD_TYPE_16_BIT_SERV_UUID_CMPLT_LIST,                                     /*< Complete list of 16-bit Service UUIDs data type value. */
      0xAA, 0xFE,                                                              /*< 16-bit Eddystone UUID. */
      6 + EddystoneURL_InitStruct.UrlLength,                                   /*< Length. */
      AD_TYPE_SERVICE_DATA,                                                    /*< Service Data data type value. */
      0xAA, 0xFE,                                                              /*< 16-bit Eddystone UUID. */
      0x10,                                                                    /*< URL frame type. */
      EddystoneURL_InitStruct.CalibratedTxPower,                               /*< Ranging data. */
      EddystoneURL_InitStruct.UrlScheme,                                       /*< URL Scheme Prefix is http://www. */
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

    for (uint8_t i = 0; i < EddystoneURL_InitStruct.UrlLength; ++i)
    {
      URL_data[14 + i] = EddystoneURL_InitStruct.Url[i];
    }

    /* Update the service data. */
    ret = aci_gap_update_adv_data(sizeof(URL_data), URL_data);

    if (ret != BLE_STATUS_SUCCESS)
    {
      while(1);
    }
#endif

    tlm_adv = TRUE;
    /* 10s of URL advertise */
    UTIL_TIMER_StartWithPeriod(&TimerTLM_Id, DEFAULT_BEACON_SEC * 10);
  }
}

static void EddystoneTLM_timCb(void *arg)
{
  /**
   * The code shall be executed in the background as aci command may be sent
   * The background is the only place where the application can make sure a new aci command
   * is not sent if there is a pending one
   */
  UTIL_SEQ_SetTask( 1<<CFG_TASK_EDDYSTONE_TLML_REQ_ID,CFG_SEQ_PRIO_0);
  return;
}

/* Exported functions --------------------------------------------------------*/
tBleStatus EddystoneTLM_Process(void)
{
  tBleStatus ret = BLE_STATUS_SUCCESS;
#if(CFG_BEACON_TYPE & CFG_EDDYSTONE_URL_BEACON_TYPE)
  uint8_t UrlScheme     = URL_PREFIX;
  uint8_t Url[]         = PHYSICAL_WEB_URL;

  EddystoneURL_InitTypeDef EddystoneURL_InitStruct =
  {
    .AdvertisingInterval = ADVERTISING_INTERVAL_IN_MS,
    .CalibratedTxPower   = CALIBRATED_TX_POWER_AT_0_M,
    .UrlScheme           = UrlScheme,
    .Url                 = Url,
    .UrlLength           = sizeof(Url) - 1
  };

  ret = EddystoneURL_Init(&EddystoneURL_InitStruct);
  if (ret != BLE_STATUS_SUCCESS)
  {
    return ret;
  }
#elif (CFG_BEACON_TYPE & CFG_EDDYSTONE_UID_BEACON_TYPE)
  uint8_t NamespaceID[] = { NAMESPACE_ID };
  uint8_t BeaconID[]    = { BEACON_ID };

  EddystoneUID_InitTypeDef EddystoneUID_InitStruct =
  {
    .AdvertisingInterval = ADVERTISING_INTERVAL_IN_MS,
    .CalibratedTxPower   = CALIBRATED_TX_POWER_AT_0_M,
    .NamespaceID         = NamespaceID,
    .BeaconID            = BeaconID
  };

  ret = EddystoneUID_Init(&EddystoneUID_InitStruct);
  if (ret != BLE_STATUS_SUCCESS)
  {
    return ret;
  }
#endif

  EddystoneTLM_InitStruct.AdvertisingInterval = ADVERTISING_INTERVAL_IN_MS;
  EddystoneTLM_InitStruct.TLM_Version       = 0;
  EddystoneTLM_InitStruct.BatteryVoltage = 3000;
  EddystoneTLM_InitStruct.BeaconTemperature = 10000;
  EddystoneTLM_InitStruct.Uptime = 2000000;
  EddystoneTLM_InitStruct.AdvertisingCount = 3000000;

  UTIL_SEQ_RegTask( 1<< CFG_TASK_EDDYSTONE_TLML_REQ_ID, UTIL_SEQ_RFU, EddystoneTLM);

  UTIL_TIMER_Create(&(TimerTLM_Id), DEFAULT_BEACON_SEC * 10, UTIL_TIMER_ONESHOT, EddystoneTLM_timCb, 0);

  tlm_adv = TRUE;

  /* 10s of URL or UID advertise */
  UTIL_TIMER_StartWithPeriod(&TimerTLM_Id, DEFAULT_BEACON_SEC * 10);

  return ret;
}