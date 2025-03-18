/**
  ******************************************************************************
  * @file    loader_region_defs.h
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
#include "loader_flash_layout.h"

/* Board selection for each STM32WBA device. By default, discovery board is selected for the device. User can change this configuration. */
#if defined(STM32WBA65xx)
#define USE_STM32WBA65I_DK1
/* #define USE_NUCLEO_64 */
#elif defined(STM32WBA55xx)
#define USE_STM32WBA55G_DK1
/* #define USE_NUCLEO_64 */
#else
#error “unsupported target”
#endif

#if defined(USE_STM32WBA65I_DK) && defined(USE_NUCLEO_64)
#error “only 1 board type allowed”
#endif
#if defined(USE_STM32WBA55I_DK) && defined(USE_NUCLEO_64)
#error “only 1 board type allowed”
#endif

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
/* External SPI Flash base addresses - Non secure and secure aliased */
#define _SPI_FLASH_BASE        (SPI_FLASH_BASE_ADDRESS) /*!< SPI FLASH(up to 64 MB) base address */
#endif /* OEMIROT_EXTERNAL_FLASH_ENABLE */
#elif defined(STM32WBA52xx) || defined(STM32WBA55xx)
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
#define FLASH_AREA_IMAGE_SECTOR_SIZE        (0x2000)     /* 8 KB */

#define S_IMAGE_PRIMARY_PARTITION_OFFSET    (FLASH_AREA_0_OFFSET)
#define S_IMAGE_SECONDARY_PARTITION_OFFSET  (FLASH_AREA_2_OFFSET)
#define NS_IMAGE_PRIMARY_PARTITION_OFFSET   (FLASH_AREA_0_OFFSET + FLASH_S_PARTITION_SIZE)
#define NS_IMAGE_SECONDARY_PARTITION_OFFSET (FLASH_AREA_2_OFFSET + FLASH_S_PARTITION_SIZE)
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
#define S_DATA_IMAGE_PRIMARY_PARTITION_OFFSET    (FLASH_AREA_4_OFFSET)
#define S_DATA_IMAGE_SECONDARY_PARTITION_OFFSET  (FLASH_AREA_6_OFFSET)
#endif /* MCUBOOT_S_DATA_IMAGE_NUMBER == 1 */
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
#define NS_DATA_IMAGE_PRIMARY_PARTITION_OFFSET   (FLASH_AREA_5_OFFSET)
#define NS_DATA_IMAGE_SECONDARY_PARTITION_OFFSET (FLASH_AREA_7_OFFSET)
#endif /* MCUBOOT_NS_DATA_IMAGE_NUMBER == 1 */

#define IMAGE_S_CODE_SIZE \
    (FLASH_S_PARTITION_SIZE - BL2_HEADER_SIZE - BL2_TRAILER_SIZE)
#define IMAGE_NS_CODE_SIZE \
    (FLASH_NS_PARTITION_SIZE - BL2_HEADER_SIZE - BL2_TRAILER_SIZE)

/* Use SRAM1 memory to store Code data */
#define S_ROM_ALIAS_BASE                    (_FLASH_BASE_S)
#define NS_ROM_ALIAS_BASE                   (_FLASH_BASE_NS)

#define LOADER_NS_ROM_ALIAS(x)              (_FLASH_BASE_NS + (x))
#define LOADER_S_ROM_ALIAS(x)               (_FLASH_BASE_S + (x))

#define S_RAM_ALIAS_BASE                    (_SRAM1_BASE_S)
#define NS_RAM_ALIAS_BASE                   (_SRAM1_BASE_NS)

#define S_RAM_ALIAS(x)                      (S_RAM_ALIAS_BASE + (x))
#define NS_RAM_ALIAS(x)                     (NS_RAM_ALIAS_BASE + (x))

/* Alias definitions for secure and non-secure areas*/
#define S_ROM_ALIAS(x)                      (S_ROM_ALIAS_BASE + (x))
#define NS_ROM_ALIAS(x)                     (NS_ROM_ALIAS_BASE + (x))

#define NS_RAM_ALIAS(x)                     (NS_RAM_ALIAS_BASE + (x))

/* Secure regions */
#define S_IMAGE_PRIMARY_AREA_OFFSET         (S_IMAGE_PRIMARY_PARTITION_OFFSET + BL2_HEADER_SIZE)
#define S_CODE_START                        (S_ROM_ALIAS(S_IMAGE_PRIMARY_AREA_OFFSET))

/* Non-secure regions */
#define NS_IMAGE_PRIMARY_AREA_OFFSET        (NS_IMAGE_PRIMARY_PARTITION_OFFSET + BL2_HEADER_SIZE)
#define NS_CODE_START                       (NS_ROM_ALIAS(NS_IMAGE_PRIMARY_AREA_OFFSET))
#define NS_DATA_START                       (_SRAM1_BASE_NS)
#define NS_DATA_SIZE                        (_SRAM1_SIZE_MAX)

#if defined(MCUBOOT_PRIMARY_ONLY)
/*  Secure Loader Image */
#define FLASH_AREA_LOADER_OFFSET            (FLASH_TOTAL_SIZE-LOADER_IMAGE_S_CODE_SIZE-LOADER_NS_CODE_SIZE)
/* Control  Secure Loader Image */
#if (FLASH_AREA_LOADER_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_LOADER_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /* (FLASH_AREA_LOADER_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0  */

/* Non-Secure Loader Image */
#define LOADER_NS_CODE_START                (LOADER_NS_ROM_ALIAS(FLASH_AREA_LOADER_OFFSET + LOADER_IMAGE_S_CODE_SIZE))
/* Control Non-Secure Loader Image */
#if (LOADER_NS_CODE_START  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "LOADER_NS_CODE_START  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /*  (LOADER_NS_CODE_START  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */

/* define used for checking possible overlap */
#define LOADER_CODE_SIZE                    (LOADER_NS_CODE_SIZE+LOADER_IMAGE_S_CODE_SIZE)
#else
/*  Loader Image  */
#define FLASH_AREA_LOADER_OFFSET            (FLASH_TOTAL_SIZE-LOADER_NS_CODE_SIZE)
/* Control  Loader Image   */
#if (FLASH_AREA_LOADER_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_LOADER_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /* (FLASH_AREA_LOADER_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */

#define LOADER_NS_CODE_START                (LOADER_NS_ROM_ALIAS(FLASH_AREA_LOADER_OFFSET))
/* define used for checking possible overlap */
#define LOADER_CODE_SIZE                    (LOADER_NS_CODE_SIZE)
#endif /* MCUBOOT_PRIMARY_ONLY */

/*  Secure and Non-Secure Loader Image */
#define LOADER_IMAGE_S_CODE_SIZE            (0x4000) /* 16 Kbytes */
#define LOADER_IMAGE_S_CODE_SIZE            (0x4000) /* 16 Kbytes */
#define LOADER_S_CODE_START                 (LOADER_S_ROM_ALIAS(FLASH_AREA_LOADER_OFFSET))
#define LOADER_S_CODE_SIZE                  (LOADER_IMAGE_S_CODE_SIZE - NSC_LOADER_SIZE)
#define LOADER_S_DATA_START                 (S_RAM_ALIAS(_SRAM1_SIZE_MAX))
#define LOADER_S_DATA_SIZE                  (_SRAM2_SIZE_MAX)
#define NSC_LOADER_START                    (LOADER_S_CODE_START + LOADER_S_CODE_SIZE)

#define LOADER_NS_CODE_SIZE                 (0x6000) /* 24 Kbytes  */
#define LOADER_NS_DATA_START                (NS_RAM_ALIAS(0x0))
#define LOADER_NS_DATA_SIZE                 (_SRAM1_SIZE_MAX)

#if defined(MCUBOOT_PRIMARY_ONLY)
#define LOADER_MAX_CODE_SIZE                 (FLASH_TOTAL_SIZE - FLASH_AREA_1_OFFSET - FLASH_AREA_1_SIZE)
#else
#define LOADER_MAX_CODE_SIZE                 (FLASH_TOTAL_SIZE - FLASH_AREA_3_OFFSET - FLASH_AREA_3_SIZE)
#endif /*  MCUBOOT_PRIMARY_ONLY */

#if defined(MCUBOOT_EXT_LOADER) && (LOADER_CODE_SIZE > LOADER_MAX_CODE_SIZE)
#error "Loader mapping overlapping slot %LOADER_CODE_SIZE %LOADER_MAX_CODE_SIZE"
#endif /* defined(MCUBOOT_EXT_LOADER) && LOADER_CODE_SIZE > LOADER_MAX_CODE_SIZE */

#endif  /* __APPLI_REGION_DEFS_H__ */