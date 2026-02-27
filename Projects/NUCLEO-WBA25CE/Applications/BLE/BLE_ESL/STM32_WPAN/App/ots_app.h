/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    OTS_app.h
  * @author  MCD Application Team
  * @brief   Header for OTS_app.c
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#ifndef OTS_APP_H
#define OTS_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ble_types.h"
#include "ble_core.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  OTS_CONN_HANDLE_EVT,
  OTS_DISCON_HANDLE_EVT,

  /* USER CODE BEGIN Service1_OpcodeNotificationEvt_t */

  /* USER CODE END Service1_OpcodeNotificationEvt_t */

  OTS_LAST_EVT,
} OTS_APP_OpcodeNotificationEvt_t;

typedef struct
{
  OTS_APP_OpcodeNotificationEvt_t          EvtOpcode;
  uint16_t                                 ConnectionHandle;

  /* USER CODE BEGIN OTS_APP_ConnHandleNotEvt_t */

  /* USER CODE END OTS_APP_ConnHandleNotEvt_t */
} OTS_APP_ConnHandleNotEvt_t;
/* USER CODE BEGIN ET */

typedef struct
{
  uint8_t *obj_p;
  uint32_t size;
  uint32_t alloc_size;
}OTS_ObjInfo_t;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* Flash area for files starts 16 KB before area used by SNVMA (SNVMA uses 2 sectors: 8KB) */
#define OTS_FLASH_STORAGE_START_ADDRESS   (FLASH_BASE + FLASH_SIZE - (2 + 4) * FLASH_PAGE_SIZE)
#define OTS_FLASH_STORAGE_END_ADDRESS     (FLASH_BASE + FLASH_SIZE - 2 * FLASH_PAGE_SIZE)
  
/* Object size: 200 x 200 pixels black and white image */
#define OBJ_ALLOC_SIZE                                                      5000
#define OBJ_HEADER_SIZE                                                        4
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void OTS_APP_Init(void);
void OTS_APP_EvtRx(OTS_APP_ConnHandleNotEvt_t *p_Notification);
/* USER CODE BEGIN EFP */

void OTS_APP_DeleteImages(void);

void OTS_APP_GetObjInfo(uint8_t obj_index, OTS_ObjInfo_t *info);

void OTS_APP_L2CAPChannelOpened(uint16_t conn_handle, uint8_t cid);

void OTS_APP_L2CAPChannelClosed(void);

void OTS_APP_L2CAPDataReceived(uint16_t sdu_length, uint8_t *sdu_data);

uint8_t OTS_APP_OLCPExec(uint8_t op_code);

uint8_t OTS_APP_OACPWrite(uint32_t offset, uint32_t length, uint8_t mode);

void OTS_APP_GetCurrentObjName(char **name_p);

/*uuid_type: 0 for 16-bit UUIDs, 1 for 128-bit UUIDS */
void OTS_APP_GetCurrentObjType(uint8_t *uuid_type_p, uint8_t **uuid_p);

void OTS_APP_GetCurrentObjSize(uint32_t *current_size_p, uint32_t *allocated_size_p);

void OTS_APP_GetCurrentObjID(uint8_t **id_p);

void OTS_APP_GetCurrentObjProp(uint32_t *prop_p);

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*OTS_APP_H */
