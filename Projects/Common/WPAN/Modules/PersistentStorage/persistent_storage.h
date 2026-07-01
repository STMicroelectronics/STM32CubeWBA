/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    persistent_storage.h
  * @author  MCD Application Team
  * @brief   Header for Persistent Storage module
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

#ifndef PERSISTENT_STORAGE_H
#define PERSISTENT_STORAGE_H

/* Includes ------------------------------------------------------------------*/
/* Standard integer types */
#include <stdint.h>

/* Utilities */
#include "utilities_common.h"

/* Exported defines ----------------------------------------------------------*/
/**
 * @brief Maximum number of banks possible to handle in Persistent Storage
 */
#define PS_BANK_NUMBER_MIN  3u

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Persistent Storage command status codes
 */
typedef enum PS_Cmd_Status
{
  PS_OK,
  PS_NOK,
  PS_OPERATION_PAUSED,
  PS_CLEANUP_REQUIRED,
  PS_DATA_NOT_FOUND,
  PS_CRC_BUSY,
  PS_CRC_ERROR,
  PS_FLASH_BUSY,
  PS_FLASH_ERROR,
  PS_ERROR_ALREADY_INIT,
  PS_ERROR_NOT_INIT,
  PS_ERROR_NULL_POINTER,
  PS_ERROR_CONFIG,
  PS_ERROR_NO_WRITE_TARGET,
  PS_ERROR_DATA_SIZE,
  PS_ERROR_INVALID,
  /* TODO: Fulfill with other error code */
  PS_UNKNOWN,
} PS_Cmd_Status_e;

/**
 * @brief Possible Data IDs
 */
typedef enum PS_Data_ID
{
  PS_DATA_ID_0  = 0u,
  PS_DATA_ID_1  = 1u,
  PS_DATA_ID_2  = 2u,
  PS_DATA_ID_3  = 3u,
  PS_DATA_ID_4  = 4u,
  PS_DATA_ID_5  = 5u,
  PS_DATA_ID_6  = 6u,
  PS_DATA_ID_7  = 7u,
  PS_DATA_ID_8  = 8u,
  PS_DATA_ID_9  = 9u,
  PS_DATA_ID_10 = 10u,
  PS_DATA_ID_11 = 11u,
  PS_DATA_ID_12 = 12u,
  PS_DATA_ID_13 = 13u,
  PS_DATA_ID_14 = 14u,
  PS_DATA_ID_15 = 15u,
  PS_DATA_ID_16 = 16u,
  PS_DATA_ID_17 = 17u,
  PS_DATA_ID_18 = 18u,
  PS_DATA_ID_19 = 19u,
  PS_DATA_ID_20 = 20u,
  PS_DATA_ID_21 = 21u,
  PS_DATA_ID_22 = 22u,
  PS_DATA_ID_23 = 23u,
  PS_DATA_ID_24 = 24u,
  PS_DATA_ID_25 = 25u,
  PS_DATA_ID_26 = 26u,
  PS_DATA_ID_27 = 27u,
  PS_DATA_ID_28 = 28u,
  PS_DATA_ID_29 = 29u,
  PS_DATA_ID_30 = 30u,
  PS_DATA_ID_31 = 31u,
  PS_DATA_ID_MAX        /* Shall not be used. Only present to mark the end of the enumeration */
} PS_Data_ID_e;

/**
 * @brief Persistent Storage Flash driver
 */
typedef struct PS_Flash_Driver
{
  PS_Cmd_Status_e (* Init) (void);                            /* Flash driver initialization */
  PS_Cmd_Status_e (* Write) (uint32_t * const p_Dest,
                                     const uint32_t * const p_Src);   /* Flash line write operation */
  PS_Cmd_Status_e (* Read) (const uint32_t * const p_Src,
                                    uint32_t * const p_Dest,
                                    const uint32_t DataSize);         /* Flash read operation */
  PS_Cmd_Status_e (* Erase) (const uint32_t SectorId);        /* Flash one sector erase */
} PS_Flash_Driver_t;

/**
 * @brief Persistent Storage CRC driver
 */
typedef struct PS_CRC_Driver
{
  PS_Cmd_Status_e (* Init) (void);                              /* CRC driver initialization */
  PS_Cmd_Status_e (* Calculate) (const uint32_t a_Buffer[],
                                         const uint32_t BufferSize,
                                         uint32_t * const p_CrcValue);  /* CRC calculate operation */
  PS_Cmd_Status_e (* Accumulate) (const uint32_t a_Buffer[],
                                          const uint32_t BufferSize,
                                          uint32_t * const p_CrcValue); /* CRC accumulate operation */
} PS_CRC_Driver_t;

/**
 * @brief Persistent Storage configuration structure
 */
typedef struct PS_Configuration
{
  uint32_t BankNumber;            /* Amount of bank inside the Persistent Storage */
  uint32_t BankSize;              /* Size of a bank in sectors */
  uint32_t AgingLimit;            /* Limit of erases difference between the oldest and the newest bank .
                                   * It helps linearize the aging of the flash memory.
                                   */
  uint32_t * p_StartingAddress;   /* First address of the flash memory area where the Persistent Storage shall stand */
  uint32_t * p_SearchLimit;       /* Last address to not overlap during Persistent Storage legacy search */
} PS_Configuration_t;

/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/**
 * @brief This structure is the linked with the IF layer implementation.
 *
 * @details Implemented by user
 */
extern const PS_Flash_Driver_t PS_FlashDriver;

/**
 *  @brief This structure is the linked with the IF layer implementation.
 *
 * @details Implemented by user
 */
extern const PS_CRC_Driver_t PS_CrcDriver;

/* Exported macros -----------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/**
  * @brief  Initialize the Persistent Storage
  *
  * @details During initialization, the module firstly try to recover an
  *          existing configuration located at the given address position.
  *          If an existing configuration is found, the persistent storage
  *          is based on the present configuration.
  *          If no existing Persistent Storage is found, the module formats
  *          the flash memory as specified in the configuration structure.
  *
  * @param p_Config: Configuration to apply for Persistent Storage
  *
  * @return State of the initialization - PS_Cmd_Status_e values
  * @retval PS_Cmd_Status_e::PS_OK
  * @retval PS_Cmd_Status_e::PS_OPERATION_PAUSED
  * @retval PS_Cmd_Status_e::PS_CRC_BUSY
  * @retval PS_Cmd_Status_e::PS_CRC_ERROR
  * @retval PS_Cmd_Status_e::PS_FLASH_BUSY
  * @retval PS_Cmd_Status_e::PS_FLASH_ERROR
  * @retval PS_Cmd_Status_e::PS_ERROR_ALREADY_INIT
  * @retval PS_Cmd_Status_e::PS_ERROR_NULL_POINTER
  * @retval PS_Cmd_Status_e::PS_ERROR_CONFIG
  * @retval PS_Cmd_Status_e::PS_UNKNOWN
  */
PS_Cmd_Status_e PS_Init (const PS_Configuration_t * const p_Config);

/**
 * @brief Write a new version of the data in the Persistent Storage
 *
 * @param DataID: ID of the data to write
 * @param p_DataSrc: Pointer onto the data source buffer
 * @param DataSize: Size of the data to write - in bytes
 *
 * @return State of the operation - PS_Cmd_Status_e values
 * @retval PS_Cmd_Status_e::PS_OK
 * @retval PS_Cmd_Status_e::PS_CLEANUP_REQUIRED
 * @retval PS_Cmd_Status_e::PS_OPERATION_PAUSED
 * @retval PS_Cmd_Status_e::PS_CRC_BUSY
 * @retval PS_Cmd_Status_e::PS_CRC_ERROR
 * @retval PS_Cmd_Status_e::PS_FLASH_BUSY
 * @retval PS_Cmd_Status_e::PS_FLASH_ERROR
 * @retval PS_Cmd_Status_e::PS_ERROR_NOT_INIT
 * @retval PS_Cmd_Status_e::PS_ERROR_NULL_POINTER
 * @retval PS_Cmd_Status_e::PS_ERROR_DATA_SIZE
 * @retval PS_Cmd_Status_e::PS_ERROR_NO_WRITE_TARGET
 * @retval PS_Cmd_Status_e::PS_ERROR_INVALID
 * @retval PS_Cmd_Status_e::PS_UNKNOWN
 */
PS_Cmd_Status_e PS_Write (const PS_Data_ID_e DataID,
                                          const uint8_t * const p_DataSrc,
                                          const uint32_t DataSize);

/**
 * @brief Restore the last valid version of the data
 *
 * @param DataID: ID of the data to restore
 * @param p_DataDest: Pointer onto the data destination buffer
 * @param DataSize: Maximum size of available data
 *
 * @return State of the operation - PS_Cmd_Status_e values
 * @retval PS_Cmd_Status_e::PS_OK
 * @retval PS_Cmd_Status_e::PS_CLEANUP_REQUIRED
 * @retval PS_Cmd_Status_e::PS_OPERATION_PAUSED
 * @retval PS_Cmd_Status_e::PS_CRC_BUSY
 * @retval PS_Cmd_Status_e::PS_CRC_ERROR
 * @retval PS_Cmd_Status_e::PS_FLASH_BUSY
 * @retval PS_Cmd_Status_e::PS_FLASH_ERROR
 * @retval PS_Cmd_Status_e::PS_ERROR_NOT_INIT
 * @retval PS_Cmd_Status_e::PS_ERROR_NULL_POINTER
 * @retval PS_Cmd_Status_e::PS_ERROR_DATA_SIZE
 * @retval PS_Cmd_Status_e::PS_DATA_NOT_FOUND
 * @retval PS_Cmd_Status_e::PS_UNKNOWN
 */
PS_Cmd_Status_e PS_Restore (const PS_Data_ID_e DataID,
                                            uint8_t * const p_DataDest,
                                            const uint32_t DataSize);

/**
 * @brief Execute a clean up operation on the banks
 *
 * @details When this operation ends, it ensure the user that the precedent write operation is a success.
 *          Not before.
 *
 * @return State of the operation - PS_Cmd_Status_e values
 * @retval PS_Cmd_Status_e::PS_OK
 * @retval PS_Cmd_Status_e::PS_CLEANUP_REQUIRED
 * @retval PS_Cmd_Status_e::PS_OPERATION_PAUSED
 * @retval PS_Cmd_Status_e::PS_CRC_BUSY
 * @retval PS_Cmd_Status_e::PS_CRC_ERROR
 * @retval PS_Cmd_Status_e::PS_FLASH_BUSY
 * @retval PS_Cmd_Status_e::PS_FLASH_ERROR
 * @retval PS_Cmd_Status_e::PS_ERROR_NOT_INIT
 * @retval PS_Cmd_Status_e::PS_ERROR_INVALID
 * @retval PS_Cmd_Status_e::PS_UNKNOWN
 */
PS_Cmd_Status_e PS_CleanUp (void);

/**
 * @brief Resume the interrupted operation
 *
 * @return State of the operation - PS_Cmd_Status_e values
 * @retval PS_Cmd_Status_e::PS_OK
 * @retval PS_Cmd_Status_e::PS_CLEANUP_REQUIRED
 * @retval PS_Cmd_Status_e::PS_OPERATION_PAUSED
 * @retval PS_Cmd_Status_e::PS_CRC_BUSY
 * @retval PS_Cmd_Status_e::PS_CRC_ERROR
 * @retval PS_Cmd_Status_e::PS_FLASH_BUSY
 * @retval PS_Cmd_Status_e::PS_FLASH_ERROR
 * @retval PS_Cmd_Status_e::PS_ERROR_NOT_INIT
 * @retval PS_Cmd_Status_e::PS_ERROR_DATA_SIZE
 * @retval PS_Cmd_Status_e::PS_DATA_NOT_FOUND
 * @retval PS_Cmd_Status_e::PS_UNKNOWN
 */
PS_Cmd_Status_e PS_Resume (void);

/**
 * @brief Callback to invoke by user when an ECC correction occurs
 *
 * @param p_ErrorAddr: Address of the flash line error corrected.
 *
 * @return None
 */
void PS_EccCorrectionCallback (uint32_t * const p_ErrorAddr);

/**
 * @brief Callback to invoke by user when an ECC detection occurs
 *
 * @param p_ErrorAddr: Address of the flash line error detected.
 *
 * @return None
 */
void PS_EccDetectionCallback (uint32_t * const p_ErrorAddr);

#endif /* PERSISTENT_STORAGE_H */
