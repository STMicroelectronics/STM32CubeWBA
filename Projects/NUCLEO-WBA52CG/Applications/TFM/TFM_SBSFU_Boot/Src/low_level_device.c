/**
  ******************************************************************************
  * @file    low_level_device.c
  * @author  MCD Application Team
  * @brief   This file contains device definition for low_level_device
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
#include "flash_layout.h"
#include "low_level_flash.h"
static struct flash_range erase_vect[] =
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
static struct flash_range write_vect[] =
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
static struct flash_range secure_vect[] =
{
#if defined(MCUBOOT_USE_HASH_REF)
  { FLASH_HASH_REF_AREA_OFFSET, FLASH_HASH_REF_AREA_OFFSET + FLASH_HASH_REF_AREA_SIZE - 1},
#endif /* MCUBOOT_USE_HASH_REF */
  { FLASH_BL2_NVCNT_AREA_OFFSET, FLASH_BL2_NVCNT_AREA_OFFSET  + FLASH_BL2_NVCNT_AREA_SIZE - 1},
#if !defined(MCUBOOT_PRIMARY_ONLY)
  { FLASH_AREA_SCRATCH_OFFSET, FLASH_AREA_SCRATCH_OFFSET + FLASH_AREA_SCRATCH_SIZE - 1},
#endif /* MCUBOOT_PRIMARY_ONLY */
  { FLASH_AREA_BEGIN_OFFSET, FLASH_AREA_0_OFFSET + FLASH_S_PARTITION_SIZE - 1}
};

struct low_level_device FLASH0_DEV =
{
  .erase = { .nb = sizeof(erase_vect) / sizeof(struct flash_range), .range = erase_vect},
  .write = { .nb = sizeof(write_vect) / sizeof(struct flash_range), .range = write_vect},
  .secure = { .nb = sizeof(secure_vect) / sizeof(struct flash_range), .range = secure_vect},
  .read_error = 1
};
