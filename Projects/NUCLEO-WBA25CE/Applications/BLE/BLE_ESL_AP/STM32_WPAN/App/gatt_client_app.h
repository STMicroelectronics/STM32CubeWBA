/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gatt_client_app.h
  * @author  MCD Application Team
  * @brief   Header for gatt_client_app.c module
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
#ifndef GATT_CLIENT_APP_H
#define GATT_CLIENT_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include "stm_list.h"
#include "ble_types.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  GATT_CLIENT_APP_DISCONNECTED,
  GATT_CLIENT_APP_CONNECTED,
  GATT_CLIENT_APP_DISCOVER_SERVICES,
  /* USER CODE BEGIN GATT_CLIENT_APP_State_t */

  /* USER CODE END GATT_CLIENT_APP_State_t */
}GATT_CLIENT_APP_State_t;

typedef enum
{
  PEER_CONN_HANDLE_EVT,
  PEER_DISCON_HANDLE_EVT,
  /* USER CODE BEGIN GATT_CLIENT_APP_Conn_Opcode_t */

  /* USER CODE END GATT_CLIENT_APP_Conn_Opcode_t */
}GATT_CLIENT_APP_Conn_Opcode_t;

typedef struct
{
  GATT_CLIENT_APP_Conn_Opcode_t          ConnOpcode;
  uint16_t                              ConnHdl;
}GATT_CLIENT_APP_ConnHandle_Notif_evt_t;
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/

/* USER CODE BEGIN EC */
#define ESL_SERVICE_UUID                                                (0x1857)
#define ESL_ADDRESS_UUID                                                (0x2BF6)
#define AP_SYNC_KEY_MATERIAL_UUID                                       (0x2BF7)
#define ESL_RESP_KEY_MATERIAL_UUID                                      (0x2BF8)
#define ESL_CURR_ABS_TIME_UUID                                          (0x2BF9)
#define ESL_CONTROL_POINT_UUID                                          (0x2BFE)
#define ESL_DISPLAY_INFO_UUID                                           (0x2BFA)
#define ESL_IMAGE_INFO_UUID                                             (0x2BFB)
#define ESL_SENSOR_INFO_UUID                                            (0x2BFC)
#define ESL_LED_INFO_UUID                                               (0x2BFD)

#define OBJECT_TRANSFER_SERVICE_UUID                                    (0x1825)
#define OTS_FEATURE_UUID                                                (0x2ABD)
#define OBJECT_NAME_UUID                                                (0x2ABE)
#define OBJECT_TYPE_UUID                                                (0x2ABF)
#define OBJECT_SIZE_UUID                                                (0x2AC0)
#define OBJECT_FIRST_CREATED_UUID                                       (0x2AC1)
#define OBJECT_LAST_MODIFIED_UUID                                       (0x2AC2)
#define OBJECT_ID_UUID                                                  (0x2AC3)
#define OBJECT_PROPERTIES_UUID                                          (0x2AC4)
#define OBJECT_ACTION_CONTROL_POINT_UUID                                (0x2AC5)
#define OBJECT_LIST_CONTROL_POINT_UUID                                  (0x2AC6)
#define OBJECT_LIST_FILTER_POINT_UUID                                   (0x2AC7)
#define OBJECT_CHANGED_UUID                                             (0x2AC8)
     
#define DEVICE_INFORMATION_SERVICE_UUID                                 (0x180A)
#define PNPID_UUID                                                      (0x2A50)

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void GATT_CLIENT_APP_Init(void);
void GATT_CLIENT_APP_Notification(GATT_CLIENT_APP_ConnHandle_Notif_evt_t *p_Notif);
tBleStatus GATT_CLIENT_APP_Set_Conn_Handle(uint8_t index, uint16_t connHdle);
int GATT_CLIENT_APP_Get_Index(uint16_t connection_handle);
void GATT_CLIENT_APP_DiscoverServices(uint16_t connection_handle, uint8_t start_task);
void GATT_CLIENT_APP_DiscoverServicesWithIndex(uint8_t index);
uint8_t GATT_CLIENT_APP_Get_State(uint8_t index);
tBleStatus GATT_CLIENT_APP_ReadCharacteristic(uint16_t connection_handle, uint16_t ValueHdl, uint16_t *data_length_p, uint8_t **data_p, uint8_t use_read_long);
tBleStatus GATT_CLIENT_APP_WriteCharacteristic(uint16_t connection_handle, uint16_t ValueHdl, uint16_t data_length, uint8_t *data);
/* USER CODE BEGIN EFP */
uint8_t GATT_CLIENT_APP_ConfigureESL(uint16_t connection_handle);
uint8_t GATT_CLIENT_APP_WriteECP(uint16_t connection_handle, uint8_t* cmd, uint8_t len_cmd, uint8_t bResponse);
uint8_t GATT_CLIENT_APP_ReadAllInfo(uint16_t connection_handle);
uint8_t GATT_CLIENT_APP_ReadDisplayInfo(uint16_t connection_handle);
uint8_t GATT_CLIENT_APP_ReadImageInfo(uint16_t connection_handle);
uint8_t GATT_CLIENT_APP_ReadSensorInfo(uint16_t connection_handle);
uint8_t GATT_CLIENT_APP_ReadLedInfo(uint16_t connection_handle);
uint8_t GATT_CLIENT_APP_ReadPnPID(uint16_t connection_handle);
/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*GATT_CLIENT_APP_H */
