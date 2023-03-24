/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    general_config.h
  * @author  MCD Application Team
  * @brief   This file contains definitions that can be changed to configure
  *          some modules of the STM32 firmware application.
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

#ifndef GENERAL_CONFIG_H__
#define GENERAL_CONFIG_H__

/* ------------------------------------------------------------------------- *
 *  WIRELESS FIRMWARE INFORMATION                                            *
 * ------------------------------------------------------------------------- */

/* Version
 * *******
 */
#define CFG_FW_MAJOR_VERSION      (1)
#define CFG_FW_MINOR_VERSION      (11)
#define CFG_FW_SUBVERSION         (0)

/* Version Type
 * *******
 * This parameter indicates whether the bin is a test version or a TAG version
 * It holds the TAG number
 * 0 means Test Version - Cannot be rebuilt
 */
#define CFG_FW_VERSION_TYPE       (0)

/* Branch
 * *******
 * This parameter is directly managed in the IDE
 * CFG_FW_BRANCH = 0 -> Cut2.1
 * CFG_FW_BRANCH = 1 -> Cut2.0
 */

/**
 * FLASH size
 * The size reported is the real size of the .bin
 */
#if   (BEACON_ONLY != 0)
#define CFG_FLASH_SIZE            (10)
#elif (LL_ONLY != 0)
#define CFG_FLASH_SIZE            (20)
#elif (SLAVE_ONLY != 0)
#define CFG_FLASH_SIZE            (30)
#else
#define CFG_FLASH_SIZE            (41)
#endif

/**
 * SRAM2 size
 * The memories reported are the secured one and not the memories required by the firmware
 */
#if(STM32WB15xx != 0)
#if   (BEACON_ONLY != 0)
#define CFG_MEMORY_RESERVED       (0)
#define CFG_SRAM2A_SIZE           (12)
#define CFG_SRAM2B_SIZE           (4)
#elif (LL_ONLY != 0)
#define CFG_MEMORY_RESERVED       (0)
#define CFG_SRAM2A_SIZE           (15)
#define CFG_SRAM2B_SIZE           (4)
#elif (SLAVE_ONLY != 0)
#define CFG_MEMORY_RESERVED       (0)
#define CFG_SRAM2A_SIZE           (20)
#define CFG_SRAM2B_SIZE           (4)
#else
#define CFG_MEMORY_RESERVED       (0)
#define CFG_SRAM2A_SIZE           (22)
#define CFG_SRAM2B_SIZE           (4)
#endif
#else
#if   (BEACON_ONLY != 0)
#define CFG_MEMORY_RESERVED       (0)
#define CFG_SRAM2A_SIZE           (9)
#define CFG_SRAM2B_SIZE           (7)
#elif (LL_ONLY != 0)
#define CFG_MEMORY_RESERVED       (0)
#define CFG_SRAM2A_SIZE           (13)
#define CFG_SRAM2B_SIZE           (7)
#elif (SLAVE_ONLY != 0)
#define CFG_MEMORY_RESERVED       (0)
#define CFG_SRAM2A_SIZE           (14)
#define CFG_SRAM2B_SIZE           (19)
#else
#define CFG_MEMORY_RESERVED       (0)
#define CFG_SRAM2A_SIZE           (22)
#define CFG_SRAM2B_SIZE           (17)
#endif
#endif

/* Type
 * ******
 * This parameter is directly managed in the IDE
 * CFG_FW_STACK_TYPE = INFO_STACK_TYPE_BLE_FULL -> full stack
 * CFG_FW_STACK_TYPE = INFO_STACK_TYPE_BLE_LIGHT -> light stack - slave only
 * CFG_FW_STACK_TYPE = INFO_STACK_TYPE_BLE_HCI -> llo stack
 * CFG_FW_STACK_TYPE = INFO_STACK_TYPE_BLE_BEACON -> beacon only stack
 */
/* ------------------------------------------------------------------------- *
 * LOW POWER configuration                                                     *
 * ------------------------------------------------------------------------- */

/**
 * This shall be set to 1 when standby is supported dynamically.
 * Otherwise, it should be set to 0 for marginal code and test execution saving
 * Note that keeping that setting to 1 when standby is not supported does not hurt
 */
#if (STM32WB15xx != 0)
#define CFG_LOW_POWER_STANDBY_SUPPORT                       1
#else
#define CFG_LOW_POWER_STANDBY_SUPPORT                       0
#endif

/* ------------------------------------------------------------------------- *
 * HW configuration                                                          *
 * ------------------------------------------------------------------------- */

/* Offset of the error numbers reported by HW */
#define CFG_HW_ERROR_OFFSET                          0x1000

/* Low speed oscillator choice: 0=LSE 1=LSI2 */
/*
 * Updated by Carnal
 * When this is enable, the M4 has the opportunity to select either LSE or LSI2
 */
#define CFG_HW_INIT_USE_LSI2                              1

/* ------------------------------------------------------------------------- *
 * HW_AES configuration                                                      *
 * ------------------------------------------------------------------------- */

/* Index of the AES H/W instance (1 or 2) used by M0 firmware */
#define CFG_HW_AES_INSTANCE                               2

/* ------------------------------------------------------------------------- *
 * HW_RNG configuration                                                      *
 * ------------------------------------------------------------------------- */

/* Index of the semaphore used to protect access to RNG */
#define CFG_HW_RNG_SEMID                                  0

/* Number of 32-bit random values stored in internal pool */
#define CFG_HW_RNG_POOL_SIZE                             32

/* ------------------------------------------------------------------------- *
 * HW_PKA configuration                                                      *
 * ------------------------------------------------------------------------- */

/* Index of the semaphore used to protect access to PKA */
#define CFG_HW_PKA_SEMID                                  1

/* ------------------------------------------------------------------------- *
 * HW_IPCC configuration                                                     *
 * ------------------------------------------------------------------------- */

/* Mode BLE only (0 or 1): if 1, use only channels 1 & 3 */
#if defined BLE_WB || defined THREAD_WB
#define CFG_HW_IPCC_BLE_ONLY                              0
#else
#define CFG_HW_IPCC_BLE_ONLY                              1
#endif

/* ------------------------------------------------------------------------- *
 * HW_UART configuration                                                     *
 * ------------------------------------------------------------------------- */

/* Enables the UART IOs and clock configuration when set to 1
   (otherwise, this configuration should be done on M4 side) */
#define CFG_HW_UART_IO_CONFIG                             1

/* Selects the UART: 0 = USART1, 1 = LPUART1 */
#define CFG_HW_UART_USE_LPUART                            0

/* Baudrate */
#define CFG_HW_UART_BAUDRATE                         921600

/* DMAMUX channel index used for UART (0..13) */
#define CFG_HW_UART_DMA_CHANNEL_IDX                       0

/* ------------------------------------------------------------------------- *
 * HW_GPIO configuration                                                     *
 * ------------------------------------------------------------------------- */

#define CFG_HW_GPIO_LPO_OUT                               1
#define CFG_HW_GPIO_MCO_OUT                               1

/* ------------------------------------------------------------------------- *
 * BAES configuration                                                        *
 * ------------------------------------------------------------------------- */

/* For cut 2, use of AES S/W implementation instead of H/W when set to 1 */
#define CFG_BAES_SW                                       0

/* ------------------------------------------------------------------------- *
 * TRACE configuration                                                       *
 * ------------------------------------------------------------------------- */

/* Total trace buffer size in bytes */
#define CFG_TRACE_BUF_SIZE                             2048

/* ------------------------------------------------------------------------- *
 * NVM configuration                                                         *
 * ------------------------------------------------------------------------- */

/**
 * Do not enable ALIGN mode
 * Used only by BLE Test Fw
 */
#define CFG_NVM_ALIGN                             (0)

/* Enables the RAM emulation when set to 1 */
#define CFG_NVM_EMUL                                      1

  /**
   * This is the max size of data the BLE Stack needs to write in NVM
   * This is different to the size allocated in the EEPROM emulator
   * The BLE Stack shall write all data at an address in the range of [0 : (x-1)]
   * The size is a number of 32bits values
   * NOTE:
   * THIS VALUE SHALL BE IN LINE WITH THE BLOCK DEFINE IN THE SCATTER FILE BLOCK_STACKLIB_FLASH_DATA
   * There are 8x32bits = 32 Bytes header in the EEPROM for each sector.
   * a page is a collection of 1 or more sectors
   */
#if(STM32WB15xx != 0)
#define CFG_NVM_BLE_MAX_SIZE                           ((2*(2048 - 32) - 4)/2/4)
#else
#define CFG_NVM_BLE_MAX_SIZE                           ((4096 - 32 - 4)/2/4)
#endif

  /**
   * This is the max size of data the THREAD Stack needs to write in NVM
   * This is different to the size allocated in the EEPROM emulator
   * The THREAD Stack shall write all data at an address in the range of [0 : (y-1)]
   * The size is a number of 32bits values
   */
#define CFG_NVMA_THREAD_NVM_SIZE               ( 0 )

/* ------------------------------------------------------------------------- *
 * EE configuration                                                         *
 * ------------------------------------------------------------------------- */

/* Size of the first bank in bytes (must be greater than 0).
   It must be a multiple of twice the page size. */
#if(STM32WB15xx != 0)
#define CFG_EE_BANK0_SIZE          (8 * HW_FLASH_PAGE_SIZE)
#else
#define CFG_EE_BANK0_SIZE          (4 * HW_FLASH_PAGE_SIZE)
#endif

/* Maximum number of 32-bit data that can be stored in the first bank. */
#define CFG_EE_BANK0_MAX_NB        (CFG_NVM_BLE_MAX_SIZE + CFG_NVMA_THREAD_NVM_SIZE)

/* Size of the second bank in bytes (can be 0 if the bank is not used).
   It must be a multiple of twice the page size. */
#define CFG_EE_BANK1_SIZE          ( 0 )

/* Maximum number of 32-bit data that can be stored in the second bank. */
#define CFG_EE_BANK1_MAX_NB                             224

#endif /* ! GENERAL_CONFIG_H__ */

