/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_zibgee_persistence_nvm.h
  * @author  MCD Application Team
  * @brief   Library to simpliest manage persistence on NVM.
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

#ifndef APP_ZIGBEE_PERSISTENCE_NVM_H
#define APP_ZIGBEE_PERSISTENCE_NVM_H

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported defines ------------------------------------------------------------*/
#define HW_FLASH_WIDTH                                  16u         // Flash Write need 4 Words (16 bytes).

#define APP_ZB_PERSISTENCE_TYPE_DISABLED                0x00u
#define APP_ZB_PERSISTENCE_TYPE_ENABLED                 0x01u
#define APP_ZB_PERSISTENCE_TYPE_ENABLED_ERASED          0x03u

/* USER CODE BEGIN ED */

/* USER CODE END ED */

/* Exported functions prototypes ---------------------------------------------*/
extern HAL_StatusTypeDef    AppZbPersistence_Init   ( struct ZigBeeT * pstZigbee, struct ZbStartupT * pstConfig, uint8_t cPersistenceType );

extern bool    AppZbPersistence_EraseAllData        ( void );
extern bool    AppZbPersistence_SaveData            ( uint8_t * pBuffer, uint16_t iBufferLength );

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#endif /* APP_ZIGBEE_PERSISTENCE_NVM_H */
