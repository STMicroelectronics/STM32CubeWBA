/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    simple_nvm_arbiter_conf.h
  * @author  MCD Application Team
  * @brief   Configuration header for simple_nvm_arbiter.c module
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
#ifndef SIMPLE_NVM_ARBITER_CONF_H
#define SIMPLE_NVM_ARBITER_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "utilities_common.h"

#include "simple_nvm_arbiter_common.h"

#include "stm32wbaxx_hal_flash.h"

/* Exported constants --------------------------------------------------------*/

/* ========================================================================== */
/* +                            NVM part - USER DEFINED                     + */
/* ========================================================================== */

/**
 * @brief Number of managed NVMs
 *
 * @details This number must be lower than SNVMA_MAX_NUMBER_NVM
 *
 */
#define SNVMA_NVM_NUMBER                1u

/* Check that NVM number does not exceed limitations */
#if SNVMA_NVM_NUMBER > SNVMA_MAX_NUMBER_NVM
#error Number of NVM to manage is to high
#endif /* SNVMA_NVM_NUMBER > SNVMA_MAX_NUMBER_NVM */

/* ========================================================================== */
/* +                        NVM IDs part - USER DEFINED                     + */
/* ========================================================================== */

/* NVM ID #1 */
#define SNVMA_NVM_ID_1
#define SNVMA_NVM_ID_1_BANK_SIZE        (1u) /* Size of a NVM bank in sector (=FLASH_PAGE_SIZE) unit */
#define SNVMA_NVM_ID_1_BANK_COUNT       (2u) /* Number of Banks for SNVMA erase/write sequence (minimum 2) */

#if (SNVMA_NVM_ID_1_BANK_COUNT == 0u) || (SNVMA_NVM_ID_1_BANK_SIZE == 0u)
#error NVM ID #1 => Bank not initialized
#elif (SNVMA_NVM_ID_1_BANK_COUNT < SNVMA_MIN_NUMBER_BANK)
#error NVM ID #1 => Not enough bank
#endif

/* ========================================================================== */
/* +                       Check part - NOT USER DEFINED                    + */
/* ========================================================================== */

/* Compute the number of sectors required */
#define SNVMA_NUMBER_OF_SECTOR_NEEDED \
		((SNVMA_NVM_ID_1_BANK_COUNT * SNVMA_NVM_ID_1_BANK_SIZE))
/* Equals to 2u */

/* Compute the total number of banks used */
#define SNVMA_NUMBER_OF_BANKS \
		((SNVMA_NVM_ID_1_BANK_COUNT))
/* Equals to 2u */

/* Check that required number of sector does not exceed Flash capacities */
#if SNVMA_NUMBER_OF_SECTOR_NEEDED == 0u
#error SNVMA_NUMBER_OF_SECTOR_NEEDED shall not be zero
#elif SNVMA_NUMBER_OF_BANKS == 0u
#error SNVMA_NUMBER_OF_BANKS shall not be zero
#endif /* SNVMA_NUMBER_OF_SECTOR_NEEDED == 0 */

/* Exported types ------------------------------------------------------------*/

/* ========================================================================== */
/* +                    Buffer ID part - CAN BE USER DEFINED                + */
/* ========================================================================== */

/**
 * @brief Enumeration of the Buffer IDs available
 *
 * @details Each NVM can handle up to 4 user IDs (NVM ID 1 => BufferId 1_1, 1_2, 1_3, 1_4).
 *
 * @details There are dummy buffer IDs for padding to ensure buffers are in the right NVM ID.
 *
 * @details Enumeration member can be renamed to fit user needs - ie: SNVMA_BufferId_1_4 => SNVMA_BleNvmId
 *
 */
typedef enum SNVMA_BufferId
{
  APP_BLE_Host_SNVMA_BufferId_1_1,
  dummy_SNVMA_BufferId_1_2,
  dummy_SNVMA_BufferId_1_3,
  dummy_SNVMA_BufferId_1_4,
  SNVMA_BufferId_Max  /* End of the enumeration */
}SNVMA_BufferId_t;

/* Size of the buffers in NVMs in bytes*/
/* NVM ID #1 */
#define SNVMA_NVM_ID_1_BUFFER_1_SIZE      (2048u)

/* Exported variables --------------------------------------------------------*/
extern SNVMA_NvmElt_t SNVMA_NvmConfiguration [SNVMA_NVM_NUMBER];

/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* SIMPLE_NVM_ARBITER_CONF_H */
