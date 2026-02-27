/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    nvm_db_conf.c
  * @author  GPM WBL Application Team
  * @brief   This file provides the structure of the database.
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

/* Includes ------------------------------------------------------------------*/
#include "nvm_db_conf.h"
#include <stdlib.h>

/** @defgroup NVM_Manager  NVM Manager
 * @{
 */

/** @defgroup NVM_Manager_TypesDefinitions Private Type Definitions
 * @{
 */

/**
 * @}
 */

/** @defgroup NVM_Manager_Private_Defines Private Defines
 * @{
 */

/**
 * @}
 */

/** @defgroup NVM_Manager_Conf_Private_Macros NVM Manager Configuration Private Macros
 * @{
 */

#define FLASH_NVM_DATASIZE  (0x6000)  // Make sure this space is reserved in the linker script.
#define NVM_START_ADDRESS   (FLASH_BASE + FLASH_SIZE - FLASH_NVM_DATASIZE)

#define DB0_SIZE  (2*FLASH_PAGE_SIZE)

/**
 * @}
 */

/** @defgroup NVM_Manager_Conf_Constants NVM Manager Configuration Constants
 * @{
 */

const NVMDB_SmallDBContainerType *NVM_SMALL_DB_STATIC_INFO = NULL;
NVMDB_StaticInfoType NVM_LARGE_DB_STATIC_INFO[NUM_LARGE_DBS];

void NVMDB_InitConf(void)
{
  NVM_LARGE_DB_STATIC_INFO[0].address = NVM_START_ADDRESS;
  NVM_LARGE_DB_STATIC_INFO[0].size = DB0_SIZE;
  NVM_LARGE_DB_STATIC_INFO[0].id = 0;
#if AUTO_CLEAN
  NVM_LARGE_DB_STATIC_INFO[0].clean_threshold = DB0_SIZE / 3;
#endif
}

/**
 * @}
 */

/**
 * @}
 */
