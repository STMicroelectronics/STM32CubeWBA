/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_ble.h
  * @author  MCD Application Team
  * @brief   Header for ble application
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_BLE_H
#define APP_BLE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/

/**
  * HCI Event Packet Types
  */
typedef __PACKED_STRUCT
{
  uint32_t *next;
  uint32_t *prev;
} BleEvtPacketHeader_t;

typedef __PACKED_STRUCT
{
  uint8_t   evtcode;
  uint8_t   plen;
  uint8_t   payload[1];
} BleEvt_t;

typedef __PACKED_STRUCT
{
  uint8_t   type;
  BleEvt_t  evt;
} BleEvtSerial_t;

typedef __PACKED_STRUCT __ALIGNED(4)
{
  BleEvtPacketHeader_t  header;
  BleEvtSerial_t        evtserial;
} BleEvtPacket_t;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
/* ST Manufacturer ID */
#define ST_MANUF_ID  0x30

/* BlueSTSDK Version */
enum
{
  BLUESTSDK_V1 =  0x01,
  BLUESTSDK_V2 =  0x02
};

/* BOARD ID */
enum
{
  BOARD_ID_NUCLEO_WBA5X = 0x8B,
  BOARD_ID_DK_WBA5X     = 0x8C,
  BOARD_ID_B_WBA5M_WPAN = 0x8D,
  BOARD_ID_NUCLEO_WBA6X = 0x8E,
  BOARD_ID_DK_WBA6X     = 0x8F
};

/* FIRMWARE ID */
enum
{
  FW_ID_P2P_SERVER            = 0x83,
  FW_ID_P2P_ROUTER            = 0x85,
  FW_ID_DT_SERVER             = 0x88,
  FW_ID_COC_PERIPH            = 0x87,
  FW_ID_HEART_RATE            = 0x89,
  FW_ID_HEALTH_THERMO         = 0x8A,
  FW_ID_HID                   = 0x8B,
  FW_ID_SENSOR                = 0x9C,
  FW_ID_GENERIC_HEALTH_SENSOR = 0x9D
};

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void APP_BLE_Init(void);
const uint8_t* BleGetBdAddress(void);
/* USER CODE BEGIN EFP */
void APP_NotifyToRun( void );

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*APP_BLE_H */
