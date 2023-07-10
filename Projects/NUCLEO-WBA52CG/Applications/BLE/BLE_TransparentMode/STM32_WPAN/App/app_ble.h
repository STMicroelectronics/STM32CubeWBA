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

typedef enum
{
  APP_BLE_IDLE,
  APP_BLE_LP_CONNECTING,
  APP_BLE_CONNECTED_SERVER,
  APP_BLE_CONNECTED_CLIENT,

} APP_BLE_ConnStatus_t;

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

/**
  * Event type
  */

/**
  * This the payload of TL_Evt_t for a command status event
  */
typedef __PACKED_STRUCT
{
  uint8_t   status;
  uint8_t   numcmd;
  uint16_t  cmdcode;
} TL_CsEvt_t;

/**
  * This the payload of TL_Evt_t for a command complete event
  */
typedef __PACKED_STRUCT
{
  uint8_t   numcmd;
  uint16_t  cmdcode;
  uint8_t   payload[1];
} TL_CcEvt_t;

/**
  * This the payload of TL_Evt_t for an asynchronous event
  */
typedef __PACKED_STRUCT
{
  uint16_t  subevtcode;
  uint8_t   payload[1];
} TL_AsynchEvt_t;

/**
  * LHCI Command Types
  */
typedef __PACKED_STRUCT
{
  uint32_t *next;
  uint32_t *prev;
} BleCmdPacketHeader_t;

typedef __PACKED_STRUCT
{
  uint16_t   cmdcode;
  uint8_t   plen;
  uint8_t   payload[255];
} BleCmd_t;

typedef __PACKED_STRUCT
{
  uint8_t   type;
  BleCmd_t  cmd;
} BleCmdSerial_t;

typedef __PACKED_STRUCT
{
  BleCmdPacketHeader_t  header;
  BleCmdSerial_t     cmdserial;
} BleCmdPacket_t;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
#define TL_LOCCMD_PKT_TYPE             ( 0x20 )
#define TL_LOCRSP_PKT_TYPE             ( 0x21 )
#define TL_EVT_CS_PAYLOAD_SIZE         ( 4 )
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ---------------------------------------------*/
void APP_BLE_Init(void);
/* USER CODE BEGIN EF */

/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*APP_BLE_H */
