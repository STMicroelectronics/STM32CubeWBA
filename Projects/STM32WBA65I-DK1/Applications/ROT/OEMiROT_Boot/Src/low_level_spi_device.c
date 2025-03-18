/**
  ******************************************************************************
  * @file    low_level_spi_device.c
  * @author  MCD Application Team
  * @brief   This file contains device definition for low_level_spi_device
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
#include "flash_layout.h"
#include "low_level_spi_flash.h"

#if defined (OEMIROT_EXTERNAL_FLASH_ENABLE)
static struct spi_flash_range erase_vect[] =
{
#if defined(MCUBOOT_USE_HASH_REF)
  { FLASH_HASH_REF_AREA_OFFSET, FLASH_HASH_REF_AREA_OFFSET + FLASH_HASH_REF_AREA_SIZE - 1},
#endif /* MCUBOOT_USE_HASH_REF */
#if !defined(MCUBOOT_PRIMARY_ONLY)
  { FLASH_AREA_SCRATCH_OFFSET, FLASH_AREA_SCRATCH_OFFSET + FLASH_AREA_SCRATCH_SIZE - 1},
  { FLASH_AREA_BEGIN_OFFSET, FLASH_AREA_END_OFFSET - 1}
#else
  { FLASH_AREA_BEGIN_OFFSET, FLASH_AREA_END_OFFSET - 1}
#endif /* MCUBOOT_PRIMARY_ONLY */
};
static struct spi_flash_range write_vect[] =
{
#if defined(MCUBOOT_USE_HASH_REF)
  { FLASH_HASH_REF_AREA_OFFSET, FLASH_HASH_REF_AREA_OFFSET + FLASH_HASH_REF_AREA_SIZE - 1},
#endif /* MCUBOOT_USE_HASH_REF */
  { FLASH_BL2_NVCNT_AREA_OFFSET, FLASH_BL2_NVCNT_AREA_OFFSET  + FLASH_BL2_NVCNT_AREA_SIZE - 1},
#if !defined(MCUBOOT_PRIMARY_ONLY)
  { FLASH_AREA_SCRATCH_OFFSET, FLASH_AREA_SCRATCH_OFFSET + FLASH_AREA_SCRATCH_SIZE - 1},
  { FLASH_AREA_BEGIN_OFFSET, FLASH_AREA_END_OFFSET - 1}
#else
  { FLASH_AREA_BEGIN_OFFSET, FLASH_AREA_END_OFFSET - 1}
#endif /* MCUBOOT_PRIMARY_ONLY */
};

struct low_level_spi_device SPI_FLASH0_DEV =
{
  .erase = { .nb = sizeof(erase_vect) / sizeof(struct spi_flash_range), .range = erase_vect},
  .write = { .nb = sizeof(write_vect) / sizeof(struct spi_flash_range), .range = write_vect},
  .read_error = 1
};
#endif /* OEMIROT_EXTERNAL_FLASH_ENABLE */
