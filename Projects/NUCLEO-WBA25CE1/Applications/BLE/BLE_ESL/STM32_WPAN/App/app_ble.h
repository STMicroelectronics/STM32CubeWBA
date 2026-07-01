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
#include <stdbool.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/

typedef enum
{
  APP_BLE_IDLE,
  APP_BLE_LP_CONNECTING,
  APP_BLE_CONNECTED_SERVER,
  APP_BLE_CONNECTED_CLIENT,
  APP_BLE_ADV_FAST,
  APP_BLE_ADV_LP,
  APP_BLE_ADV_NON_CONN_FAST,
  APP_BLE_ADV_NON_CONN_LP,
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
  PROC_GAP_PERIPH_ADVERTISE_START_LP,
  PROC_GAP_PERIPH_ADVERTISE_START_FAST,
  PROC_GAP_PERIPH_ADVERTISE_NON_CONN_START_LP,
  PROC_GAP_PERIPH_ADVERTISE_NON_CONN_START_FAST,
  PROC_GAP_PERIPH_ADVERTISE_STOP,
  PROC_GAP_PERIPH_ADVERTISE_DATA_UPDATE,
  PROC_GAP_PERIPH_CONN_PARAM_UPDATE,
  /* USER CODE BEGIN ProcGapPeripheralId_t */

  /* USER CODE END ProcGapPeripheralId_t */
}ProcGapPeripheralId_t;

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
  BOARD_ID_B_WBA5M_WPAN = 0x91,
  BOARD_ID_NUCLEO_WBA6X = 0x8E,
  BOARD_ID_DK_WBA6X     = 0x92,
  BOARD_ID_B_WBA6M_WPAN = 0x93,
  BOARD_ID_NUCLEO_WBA2X = 0x90
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

/* Maximum size of L2CAP SDUs that can be received. It must be at least 64 for
  enhanced credit based mode. For OTS implementation, it cannot be greater than
  a Flash page. */
#define MAX_SDU_LENGTH                                                       244

/* SPSM for OTS */
#define SPSM_OTS                                                          0x0025
          
/* ATT Error codes */          
#define BLE_ATT_ERR_NONE                                (0x00U) /** Not an error! */
#define BLE_ATT_ERR_INVALID_HANDLE                      (0x01U) /** The attribute handle given was not valid on this server. */
#define BLE_ATT_ERR_READ_NOT_PERM                       (0x02U) /** The attribute cannot be read. */
#define BLE_ATT_ERR_WRITE_NOT_PERM                      (0x03U) /** The attribute cannot be written. */
#define BLE_ATT_ERR_INVALID_PDU                         (0x04U) /** The attribute PDU was invalid. */
#define BLE_ATT_ERR_INSUFF_AUTHENTICATION               (0x05U) /** The attribute requires authentication before it can be read or written. */
#define BLE_ATT_ERR_REQ_NOT_SUPP                        (0x06U) /** Attribute server does not support the request received from the client. */
#define BLE_ATT_ERR_INVALID_OFFSET                      (0x07U) /** Offset specified was past the end of the attribute. */
#define BLE_ATT_ERR_INSUFF_AUTHORIZATION                (0x08U) /** The attribute requires authorization before it can be read or written. */
#define BLE_ATT_ERR_PREP_QUEUE_FULL                     (0x09U) /** Too many prepare writes have been queued. */
#define BLE_ATT_ERR_ATTR_NOT_FOUND                      (0x0AU) /** No attribute found within the given attribute handle range. */
#define BLE_ATT_ERR_ATTR_NOT_LONG                       (0x0BU) /** The attribute cannot be read using the Read Blob Request. */
#define BLE_ATT_ERR_INSUFF_ENCR_KEY_SIZE                (0x0CU) /** The Encryption Key Size used for encrypting this link is insufficient. */
#define BLE_ATT_ERR_INVAL_ATTR_VALUE_LEN                (0x0DU) /** The attribute value length is invalid for the operation. */
#define BLE_ATT_ERR_UNLIKELY                            (0x0EU) /** The attribute request that was requested has encountered an error
                                                                 *  that was unlikely, and therefore could not be completed as requested. */
#define BLE_ATT_ERR_INSUFF_ENCRYPT                      (0x0FU) /** The attribute requires encryption before it can be read or written. */
#define BLE_ATT_ERR_UNSUPP_GRP_TYPE                     (0x10U) /** The attribute type is not a supported grouping attribute as defined by
                                                                 *   a higher layer specification. */
#define BLE_ATT_ERR_INSUFF_RESOURCES                    (0x11U) /** Insufficient Resources to complete the request. */
#define BLE_ATT_ERR_DB_OUT_OF_SYNC                      (0x12U) /** The server requests the client to rediscover the database. */
#define BLE_ATT_ERR_VALUE_NOT_ALLOWED                   (0x13U) /** The attribute parameter value was not allowed. */
#define BLE_ATT_ERR_APPL_MIN                            (0x80U) /** Application error code defined by a higher layer specification.
                                                                 *   Lower error value.
                                                                 */
#define BLE_ATT_ERR_APPL_MAX                            (0x9FU) /** Application error code defined by a higher layer specification.
                                                                 *   Higher error value.
                                                                 */
#define BLE_ATT_ERR_CMN_PROF_SRVC_MIN                   (0xE0U) /** Common profile and service error codes defined in [Core Specification
                                                                 *   Supplement], Part B.
                                                                 *   Lower error value.
                                                                 */
#define BLE_ATT_ERR_CCCD_IMPROPERLY_CONFIGURED          (0xFDU) /** The Client Characteristic Configuration Descriptor Improperly Configured error
                                                                 *   code is used when a Client Characteristic Configuration descriptor is not
                                                                 *   configured according to the requirements of the profile or service.
                                                                 *   2.3 CLIENT CHARACTERISTIC CONFIGURATION DESCRIPTOR IMPROPERLY CONFIGURED (0xFD)
                                                                 *   Supplement to the Bluetooth Core Specification | CSS v10, Part B page 34
                                                                 */
#define BLE_ATT_ERR_CMN_PROF_SRVC_MAX                   (0xFFU) /** Common profile and service error codes defined in [Core Specification
                                                                 *   Supplement], Part B.
                                                                 *   Higher error value. */
          
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

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void APP_BLE_Init(void);
void APP_BLE_Deinit(void);
void BleStack_Process_BG(void);
APP_BLE_ConnStatus_t APP_BLE_Get_Server_Connection_Status(void);
void APP_BLE_Procedure_Gap_General(ProcGapGeneralId_t ProcGapGeneralId);
void APP_BLE_Procedure_Gap_Peripheral(ProcGapPeripheralId_t ProcGapPeripheralId);
const uint8_t* BleGetBdAddress(void);
tBleStatus SetGapAppearance(uint16_t appearance);
tBleStatus SetGapDeviceName(uint8_t *devicename, uint8_t devicename_len);
void Ble_UserEvtRx(void);
void APP_BLE_HostNvmStore(void);
/* USER CODE BEGIN EFP */
void APP_BLE_StartAdvertising(bool fast_adv);
void APP_BLE_StopAdvertising(void);
/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*APP_BLE_H */
