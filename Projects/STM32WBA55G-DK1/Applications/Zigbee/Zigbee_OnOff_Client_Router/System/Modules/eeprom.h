/*****************************************************************************
 * @file    eeprom.h
 * @author  MCD Application Team
 * @brief   This file contains the interface of the EEPROM emulator
 *          for the STM32WB platform.
 *****************************************************************************
 * @attention
 *
 * Copyright (c) 2018-2021 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 *****************************************************************************
 */

#ifndef EEPROM_H__
#define EEPROM_H__


/*
 * Description
 * -----------
 * the EEPROM module implements an EEPROM emulator in one C file ("eeprom.c").
 * Its interface is defined below in this file ("eeprom.h").
 * A lteast one independent NVM page can be used.
 * Data granularity for store and read is one 8-bit word.
 * 
 */


#include <stdint.h>
#include "hw_flash.h"



/* Definition of the functions return value */
enum EepromReturnT
{
  EEPROM_OK = 0,
  EEPROM_CLEAN_NEEDED,      /* Data is written but an "erase" is needed next time */
  EEPROM_ERROR_NOT_FOUND,   /* Read data is not found in flash */
  EEPROM_ERROR_ERASE,       /* An error occurs during flash erase */
  EEPROM_ERROR_WRITE,       /* An error occurs during flash write */
  EEPROM_ERROR_STATE        /* State of flash is incoherent */
};


/* Eeprom state definition */
enum EepromStateT
{
  EEPROM_STATE_ERASED  = 0,   /* page is erased */
  EEPROM_STATE_ACTIVE  = 1,   /* page contains valid data */
  EEPROM_STATE_ERROR   = 2,   /* Problem during GetState */
};




/**
 * @brief   Initialization of EEPROM emulation module. It must be called once at reset.
 *
 * @param   lBaseAddress    Absolute start address of flash area used for EEPROM emulation. It must be a multiple of flash page size.
 * @param   iSize           Size of the emuled Eeprom. It must be a multiple of flash page size.
 *
 * @return  EepromReturnT 
 */
extern enum EepromReturnT EEPROM_Init( uint32_t lBaseAddress, uint16_t iSize );


/**
 * @brief   Complete erase the EEPROM emulation module.
 *
 * @return  EepromReturnT 
 */
extern enum EepromReturnT EEPROM_Erase( void );


/**
 * @brief   Indicate the State of Eeprom.
 *
 * @return  EepromStateT 
 */
extern enum EepromStateT EEPROM_GetState( void );

/**
 * @brief   Returns the last stored data on Eeprom emulation module.
 *
 * @param   pData     Pointer on buffer to write with stored data
 * @param   piNbData  Maximum number of data to read (to prevent buffer overflow) and data finally read.
 *
 * return: EepromReturnT
 */
extern enum EepromReturnT EEPROM_Read( uint8_t * pData, uint16_t * piNbData );


/**
 * @brief   Writes variable data in Eeprom emulation module.
 *
 * @param   pData     Pointer on buffer data to store.
 * @param   iLength   Number of data to store.
 *
 * return: enum EepromReturnT
 */
extern enum EepromReturnT EEPROM_Write( uint8_t * pData, uint16_t iNbData );


#endif /* EEPROM_H__ */
