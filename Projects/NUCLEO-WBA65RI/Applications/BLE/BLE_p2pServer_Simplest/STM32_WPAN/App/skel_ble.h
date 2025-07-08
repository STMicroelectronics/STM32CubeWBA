/**
  ******************************************************************************
  * @file    ble_if.h
  * @author  MCD Application Team
  * @brief   Header for ble application
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BLE_H
#define BLE_H

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
/**
*   Define service and characteristic parameters
*/
#define MY_BLE_SERVICE_MAX_ATT_RECORDS                  8
#define CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET           1
#define MY_BLE_NOTIFY_CHARACTERISTIC_VALUE_LENGTH       2
#define MY_BLE_WRITE_CHARACTERISTIC_VALUE_LENGTH        2


#define CHARACTERISTIC_CONFIGURATION_DESCRIPTOR_OFFSET  2

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
#define VariableBit_Reset_BB(VariableAddress, BitNumber) \
(*(volatile uint32_t *) (VariableAddress) &= ~(1 << BitNumber))
#define VariableBit_Set_BB(VariableAddress, BitNumber) \
(*(volatile uint32_t *) (VariableAddress) |= (1 << BitNumber))
#define VariableBit_Get_BB(VariableAddress, BitNumber) \
((*(volatile uint32_t *) (VariableAddress) & (1 << BitNumber)) >> BitNumber)

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/

/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void BLE_Init(void);
void Ble_HostStack_Process(void);
void BLE_Process(void);
void ll_process(void);

/* USER CODE BEGIN EFP */
void BLE_Service_Init(void);
void APP_BLE_Key_Button1_Action(void);
/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*BLE_H */
