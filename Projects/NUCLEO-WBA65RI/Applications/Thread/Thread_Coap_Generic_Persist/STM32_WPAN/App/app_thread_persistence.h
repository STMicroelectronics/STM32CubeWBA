/*****************************************************************************
 * @file    app_thread_persistence.h
 * @author  MCD Application Team
 * @brief   This file contains the interface of the EEPROM emulator
 *          for the STM32WBA platform.
 *****************************************************************************
 * @attention
 *
 * Copyright (c) 2018-2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 *****************************************************************************
 */

#ifndef APP_THREAD_PERSISTENCE_H__
#define APP_THREAD_PERSISTENCE_H__

#include <stdbool.h>

/*
 * Description
 * -----------
 * Public API for persistence module
 * 
 */

/**
 * @brief  Initialise the Persistence 
 * @param  None
 * @retval true if success , false if fail
 */
void APP_THREAD_Persistence_Init(void);   


/**
 * @brief  Delete part of the persistent data
 * @param  None
 * @retval 'true' if successful, else 'false'
 */
bool APP_THREAD_Persistence_Delete(uint32_t pOtBufferOffset, uint32_t pOtBufferLen);

/**
 * @brief  Earse all the persistent data
 * @param  None
 * @retval None
 */
void APP_THREAD_Persistence_Earse(void);


/**
 * @brief  Thread persistence restore
 * @param  pointer to otOperationalDataset structure
 * @retval 'true' if startup from persistence is successful, else 'false'
 */
 void APP_THREAD_Persistence_Restore(void);
 
/**
 * @brief  Save 'Persistent' data
 * @param  pointer to otOperationalDataset structure
 * @retval true if success , false if fail
 */
void APP_THREAD_Persistence_Save(void);

/**
 * @brief  Load persistent data
 * @param  None
 * @retval true if success, false if fail
 */
bool APP_THREAD_Persistence_Load(void);

#endif /* NVM_PERSISTENCE_H__ */
