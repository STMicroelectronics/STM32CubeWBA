/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gatt_client_app.h
  * @author  MCD Application Team
  * @brief   Header for gatt_client_app.c module
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
#ifndef GATT_CLIENT_APP_H
#define GATT_CLIENT_APP_H

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
typedef enum
{
  PWR_CO_GPIO_OFF,
  PWR_CO_GPIO_ON,
  PWR_CO_GPIO_TIM_RESET_ON,
} PWR_CO_GPIO_Status_t;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/

/* USER CODE BEGIN EC */

#define ST_LPWR_CO_SERVICE_UUID                                       (0xFF10)
#define ST_LPWR_CO_WRITE_CHAR_UUID                                    (0xFF11)
#define ST_LPWR_CO_NOTIFY_CHAR_UUID                                   (0xFF12)

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

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*GATT_CLIENT_APP_H */
