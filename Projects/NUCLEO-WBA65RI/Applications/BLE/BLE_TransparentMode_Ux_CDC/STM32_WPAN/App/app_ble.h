/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_ble.h
  * @author  MCD Application Team
  * @brief   Header for ble application
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm_list.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/

/**
  * HCI Event Packet Types
  */

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

/**
  * Event type
  */

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
  * LHCI Command Types
  */

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

#define HCI_DATA_MAX_SIZE         315

/**
  * USB packet node
  */
typedef struct
{
  tListNode                 node;  /* Actual node in the list */
  uint8_t buf[HCI_DATA_MAX_SIZE];  /* Memory buffer */
  uint16_t len;                    /* Data Length */
} USBNode_t;

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

/* Exported functions prototypes ---------------------------------------------*/
void APP_BLE_Init(void);
void BleStack_Process_BG(void);
/**
  * @brief Retrieve a USB packet from the USB RX pool
  * @retval a USB packet from the USB RX pool
  */
USBNode_t* TM_GetUSBRXPacket(void);

/**
  * @brief Add a USB packet to USB RX list and run the TX to Host task to process it
  * @param pNode: a pointer to the USB packet to add to the USB RX list
  * @retval status of the operation
  */
tBleStatus TM_AddUSBRXPacket(USBNode_t *pNode);

/**
  * @brief Retrieve a USB packet from the USB TX list
  * @retval a USB packet from the USB TX List
  */
USBNode_t* TM_GetUSBTXPacket(void);

/**
  * @brief Returns USB packet to the USB TX Pool
  * @param pNode: pointer to the packet to return to the TX Pool
  * @retval None
  */
void TM_FreeUSBTXPacket(USBNode_t *pNode);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*APP_BLE_H */
