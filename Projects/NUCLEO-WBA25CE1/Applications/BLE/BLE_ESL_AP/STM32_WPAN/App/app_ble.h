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
#include "ble_types.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/

typedef enum
{
  APP_BLE_IDLE,
  APP_BLE_LP_CONNECTING,
  APP_BLE_CONNECTED_SERVER,
  APP_BLE_CONNECTED_CLIENT,
  APP_BLE_SCANNING,
  APP_BLE_CONNECTED,
  APP_BLE_DISCOVERING_SERVICES,
  APP_BLE_DISCOVERING_CHARACS,
  /* USER CODE BEGIN APP_BLE_ConnStatus_t */

  /* USER CODE END APP_BLE_ConnStatus_t */
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

typedef enum
{
  PROC_GAP_GEN_PHY_TOGGLE,
  PROC_GAP_GEN_CONN_TERMINATE,
  /* USER CODE BEGIN ProcGapGeneralId_t */

  /* USER CODE END ProcGapGeneralId_t */
}ProcGapGeneralId_t;

typedef enum
{
  PROC_GAP_CENTRAL_SCAN_START,
  PROC_GAP_CENTRAL_SCAN_TERMINATE,
  /* USER CODE BEGIN ProcGapCentralId_t */

  /* USER CODE END ProcGapCentralId_t */
}ProcGapCentralId_t;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
/*L2CAP Error codes */          
#define L2CAP_CONN_SUCCESSFUL                           0x0000
#define L2CAP_CONN_FAIL_SPSM_NOT_SUPPORTED              0x0002
#define L2CAP_CONN_FAIL_INSUFFICIENT_RESOURCES          0x0004
#define L2CAP_CONN_FAIL_INSUFFICIENT_AUTHENTICATION     0x0005
#define L2CAP_CONN_FAIL_INSUFFICIENT_AUTHORIZATION      0x0006
#define L2CAP_CONN_FAIL_KEY_SIZE_TOO_SHORT              0x0007
#define L2CAP_CONN_FAIL_INSUFFICIENT_ENCRYPTION         0x0008
#define L2CAP_CONN_FAIL_INVALID_SOURCE_CID              0x0009
#define L2CAP_CONN_FAIL_SOURCE_CID_ALREADY_ALLOCATED    0x000A
#define L2CAP_CONN_FAIL_UNACCEPTABLE_PARAMETERS         0x000B
#define L2CAP_CONN_FAIL_INVALID_PARAMETERS              0x000C
#define L2CAP_CONN_FAIL_NO_INFO                         0x000D
#define L2CAP_CONN_FAIL_AUTHENTICATION_PENDING          0x000E
#define L2CAP_CONN_FAIL_AUTHORIZATION_PENDING           0x000F
/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
#define ADV_INT_MS(x) ((uint16_t)((x)/0.625f))
#define SCAN_WIN_MS(x) ((uint16_t)((x)/0.625f))
#define SCAN_INT_MS(x) ((uint16_t)((x)/0.625f))
#define CONN_INT_MS(x) ((uint16_t)((x)/1.25f))
#define CONN_SUP_TIMEOUT_MS(x) ((uint16_t)((x)/10.0f))
#define CONN_CE_LENGTH_MS(x) ((uint16_t)((x)/0.625f))

#define HCI_LE_ADVERTISING_REPORT_RSSI(p) \
        (*(int8_t*)((&((hci_le_advertising_report_event_rp0*)(p))-> \
                      Advertising_Report[0].Length_Data) + 1 + \
                    ((hci_le_advertising_report_event_rp0*)(p))-> \
                    Advertising_Report[0].Length_Data))
/* USER CODE BEGIN EM */
/** @brief Macro that returns a 16-bit value from a buffer where the value is stored in Little Endian Format */
#define LE_TO_HOST_16(ptr)   (uint16_t) ( ((uint16_t)*((uint8_t *)(ptr))) | \
                                          ((uint16_t)*((uint8_t *)(ptr) + 1) << 8) )

/** @brief Macro that returns a 16-bit value from a buffer where the value is stored in Big Endian Format */
#define BE_TO_HOST_16(ptr)  (uint16_t) ( ((uint16_t) *((uint8_t *)ptr)) << 8 | \
                                         ((uint16_t) *((uint8_t *)ptr + 1) ) )

/** @brief Macro that stores a 16-bit value into a buffer in Little Endian Format (2 bytes) */
#define HOST_TO_LE_16(buf, val)    ( ((buf)[0] =  (uint8_t) (val)    ) , \
                                   ((buf)[1] =  (uint8_t) ((val)>>8) ) )

/** @brief Macro that returns a 32-bit value from a buffer where the value is stored in Little Endian Format */
#define LE_TO_HOST_32(ptr)   (uint32_t) ( ((uint32_t)*((uint8_t *)(ptr))) | \
                                           ((uint32_t)*((uint8_t *)(ptr) + 1) << 8)  | \
                                           ((uint32_t)*((uint8_t *)(ptr) + 2) << 16) | \
                                           ((uint32_t)*((uint8_t *)(ptr) + 3) << 24) )

/** @brief Macro that returns a 16-bit value from a buffer where the value is stored in Big Endian Format */
#define BE_TO_HOST_32(ptr)  (uint32_t) ( ((uint32_t) *((uint8_t *)ptr    )) << 24 | \
                                         ((uint32_t) *((uint8_t *)ptr + 1)) << 16 | \
                                         ((uint32_t) *((uint8_t *)ptr + 2)) << 8 | \
                                         ((uint32_t) *((uint8_t *)ptr + 3)) )

/** @brief Macro that stores a 32-bit value into a buffer in Little Endian Format (4 bytes) */
#define HOST_TO_LE_32(buf, val)    ( ((buf)[0] =  (uint8_t) (val)     ) , \
                                   ((buf)[1] =  (uint8_t) ((val)>>8)  ) , \
                                   ((buf)[2] =  (uint8_t) ((val)>>16) ) , \
                                   ((buf)[3] =  (uint8_t) ((val)>>24) ) )

/** @brief Macro that returns a 64-bit value from a buffer where the value is stored in Little Endian Format */
#define LE_TO_HOST_64(ptr)   (uint64_t) ( ((uint64_t)*((uint8_t *)(ptr))) | \
                                          ((uint64_t)*((uint8_t *)(ptr) + 1) << 8)  | \
                                          ((uint64_t)*((uint8_t *)(ptr) + 2) << 16) | \
                                          ((uint64_t)*((uint8_t *)(ptr) + 3) << 24) | \
                                          ((uint64_t)*((uint8_t *)(ptr) + 4) << 32) | \
                                          ((uint64_t)*((uint8_t *)(ptr) + 5) << 40) | \
                                          ((uint64_t)*((uint8_t *)(ptr) + 6) << 48) | \
                                          ((uint64_t)*((uint8_t *)(ptr) + 7) << 56) )

/** @brief Macro that stores a 64-bit value into a buffer in Little Endian Format (8 bytes) */
#define HOST_TO_LE_64(buf, val)    ( ((buf)[0] =  (uint8_t) (val)       ) , \
                                     ((buf)[1] =  (uint8_t) ((val)>>8)  ) , \
                                     ((buf)[2] =  (uint8_t) ((val)>>16) ) , \
                                     ((buf)[3] =  (uint8_t) ((val)>>24) ) , \
                                     ((buf)[4] =  (uint8_t) ((val)>>32) ) , \
                                     ((buf)[5] =  (uint8_t) ((val)>>40) ) , \
                                     ((buf)[6] =  (uint8_t) ((val)>>48) ) , \
                                     ((buf)[7] =  (uint8_t) ((val)>>56) ) )

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void APP_BLE_Init(void);
void APP_BLE_Deinit(void);
void BleStack_Process_BG(void);
APP_BLE_ConnStatus_t APP_BLE_Get_Client_Connection_Status( uint16_t Connection_Handle );
void APP_BLE_Procedure_Gap_General(ProcGapGeneralId_t ProcGapGeneralId);
void APP_BLE_Procedure_Gap_Central(ProcGapCentralId_t ProcGapCentralId);
const uint8_t* BleGetBdAddress(void);
tBleStatus SetGapAppearance(uint16_t appearance);
tBleStatus SetGapDeviceName(uint8_t *devicename, uint8_t devicename_len);
void Ble_UserEvtRx(void);
void APP_BLE_HostNvmStore(void);
/* USER CODE BEGIN EFP */
void create_periodic_advertising_connection(uint8_t subevent, uint8_t Peer_Address_Type, uint8_t Peer_Address[6]);
uint8_t Scan_proc(void);
void set_bleAppContext_address(uint8_t address_type, const uint8_t p_address[6]);
uint8_t* get_bleAppContext_address(void);
void periodic_sync_info_transfer(void);
/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*APP_BLE_H */
