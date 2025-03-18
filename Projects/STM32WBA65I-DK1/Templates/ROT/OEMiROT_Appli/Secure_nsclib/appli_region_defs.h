/**
  ******************************************************************************
  * @file    appli_region_defs.h
  * @author  MCD Application Team
  * @brief   This file contains definitions of memory regions for stm32wbaxx.
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

#ifndef __APPLI_REGION_DEFS_H__
#define __APPLI_REGION_DEFS_H__
#include "appli_flash_layout.h"

#if defined(STM32WBA65xx)
#define _SRAM1_SIZE_MAX         (0x70000)  /*!< SRAM1=448k*/
#define _SRAM2_SIZE_MAX         (0x10000)  /*!< SRAM2=64k*/

/* Flash and internal SRAMs base addresses - Non secure aliased */
#define _FLASH_BASE_NS          (0x08000000) /*!< FLASH(1024 KB) base address */
#define _SRAM1_BASE_NS          (0x20000000) /*!< SRAM1(448 KB) base address */
#define _SRAM2_BASE_NS          (0x20070000) /*!< SRAM2(64 KB) base address */
/* Flash and internal SRAMs base addresses - Secure aliased */
#define _FLASH_BASE_S           (0x0C000000) /*!< FLASH(1024 KB) base address */
#define _SRAM1_BASE_S           (0x30000000) /*!< SRAM1(448 KB) base address */
#define _SRAM2_BASE_S           (0x30070000) /*!< SRAM2(64 KB) base address */

#if  defined(OEMIROT_EXTERNAL_FLASH_ENABLE)
/* External OSPI Flash base addresses - Non secure and secure aliased */
#define _OSPI_FLASH_BASE        (OSPI_FLASH_BASE_ADDRESS) /*!< OSPI FLASH(up to 64 MB) base address */
#endif /* OEMIROT_EXTERNAL_FLASH_ENABLE */
#elif defined(STM32WBA52xx)
#define _SRAM1_SIZE_MAX         (0x10000)  /*!< SRAM1=64k*/
#define _SRAM2_SIZE_MAX         (0x10000)  /*!< SRAM2=64k*/

/* Flash and internal SRAMs base addresses - Non secure aliased */
#define _FLASH_BASE_NS          (0x08000000) /*!< FLASH(1024 KB) base address */
#define _SRAM1_BASE_NS          (0x20000000) /*!< SRAM1(64 KB) base address */
#define _SRAM2_BASE_NS          (0x20010000) /*!< SRAM2(64 KB) base address */
/* Flash and internal SRAMs base addresses - Secure aliased */
#define _FLASH_BASE_S           (0x0C000000) /*!< FLASH(1024 KB) base address */
#define _SRAM1_BASE_S           (0x30000000) /*!< SRAM1(64 KB) base address */
#define _SRAM2_BASE_S           (0x30010000) /*!< SRAM2(64 KB) base address */
#else
#error "Unknown target."
#endif

#define TOTAL_ROM_SIZE          FLASH_TOTAL_SIZE
#define S_TOTAL_RAM_SIZE        (_SRAM2_SIZE_MAX) /*! size require for Secure part */

#define BL2_HEADER_SIZE                     (0x400) /*!< Appli image header size */
#define BL2_TRAILER_SIZE                    (0x2000)
#define BL2_DATA_HEADER_SIZE                (0x20)  /*!< Data image header size */
#define S_IMAGE_PRIMARY_PARTITION_OFFSET    (FLASH_AREA_0_OFFSET)
#define S_IMAGE_SECONDARY_PARTITION_OFFSET  (FLASH_AREA_2_OFFSET)
#define NS_IMAGE_PRIMARY_PARTITION_OFFSET   (FLASH_AREA_0_OFFSET + FLASH_S_PARTITION_SIZE)
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
#define S_DATA_IMAGE_PRIMARY_PARTITION_OFFSET    (FLASH_AREA_4_OFFSET)
#define S_DATA_IMAGE_SECONDARY_PARTITION_OFFSET  (FLASH_AREA_6_OFFSET)
#endif /* MCUBOOT_S_DATA_IMAGE_NUMBER == 1 */

#define IMAGE_S_CODE_SIZE \
    (FLASH_S_PARTITION_SIZE - BL2_HEADER_SIZE - BL2_TRAILER_SIZE)
#define IMAGE_NS_CODE_SIZE \
    (FLASH_NS_PARTITION_SIZE - BL2_HEADER_SIZE - BL2_TRAILER_SIZE)

#define S_ROM_ALIAS_BASE                    (_FLASH_BASE_S)
#define NS_ROM_ALIAS_BASE                   (_FLASH_BASE_NS)

/* Alias definitions for secure and non-secure areas*/
#define S_ROM_ALIAS(x)                      (S_ROM_ALIAS_BASE + (x))
#define NS_ROM_ALIAS(x)                     (NS_ROM_ALIAS_BASE + (x))

/* Secure regions */
#define S_IMAGE_PRIMARY_AREA_OFFSET         (S_IMAGE_PRIMARY_PARTITION_OFFSET + BL2_HEADER_SIZE)
#define S_CODE_START                        (S_ROM_ALIAS(S_IMAGE_PRIMARY_AREA_OFFSET))
#define S_CODE_SIZE                         (IMAGE_S_CODE_SIZE - NSC_CODE_SIZE)
#define S_RAM_START                         (_SRAM2_BASE_S)
#define S_RAM_SIZE                          (S_TOTAL_RAM_SIZE)

#define NSC_CODE_START                      (S_ROM_ALIAS(S_IMAGE_PRIMARY_PARTITION_OFFSET) + FLASH_S_PARTITION_SIZE - NSC_CODE_SIZE - PARTITION_RESERVED)

#endif  /* __APPLI_REGION_DEFS_H__ */