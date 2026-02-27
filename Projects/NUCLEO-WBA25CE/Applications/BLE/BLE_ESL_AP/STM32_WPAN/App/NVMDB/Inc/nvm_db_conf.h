/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    nvm_db_conf.h
  * @author  GPM WBL Application Team
  * @brief   Header file for NVM manager, that can be customized.
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
#ifndef NVM_DB_CONF_H
#define NVM_DB_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32wbaxx_hal.h"

#ifndef AUTO_CLEAN
#define AUTO_CLEAN 1
#endif

/** @addtogroup NVM_Manager_Peripheral  NVM Manager
 * @{
 */

/** @defgroup NVM_Manager_Exported_Types Exported Types
 * @{
 */

typedef struct
{
  uint8_t id;           // Database id.
  uint32_t offset;      // Offset of the database in the page.
#if AUTO_CLEAN
  uint16_t clean_threshold;      // Threshold of free space under which a clean operation is triggered. Set to 0 to disable.
#endif
} NVMDB_SmallDBType;

typedef struct
{
  uint32_t page_address;                    // Start address of the page where the databases are located.
  uint8_t num_db;                           // Number of databases inside the page.
  const NVMDB_SmallDBType *dbs;             // Pointer to an array, where each element is the offset of each database in the page.
} NVMDB_SmallDBContainerType;

typedef struct
{
  uint32_t address;
  uint16_t size;
  uint8_t id;
#if AUTO_CLEAN
  uint16_t clean_threshold;      // Threshold of free space under which a clean operation is triggered. Set to 0 to disable.
#endif
} NVMDB_StaticInfoType;

/**
 * @}
 */

/** @defgroup NVM_Manager_Exported_Constants  Exported Constants
 * @{
 */

#define PAGE_SIZE FLASH_PAGE_SIZE // It specifies the minimum size that can be erased. It must be multiple of 2.

/* Change the following macros in order to match the desired database set. */

#define NUM_SMALL_DB_PAGES      0
#define NUM_SMALL_DBS           0
#define NUM_LARGE_DBS           1

#define NUM_DB (NUM_SMALL_DBS + NUM_LARGE_DBS)

/**
 * @}
 */

/** @defgroup NVM_Manager_Exported_Macros           Exported Macros
 * @{
 */

/**
 * @}
 */

/** @defgroup NVM_Manager_Exported_Functions        Exported Functions
 * @{
 */
void NVMDB_InitConf(void);
/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* NVM_DB_CONF_H */
