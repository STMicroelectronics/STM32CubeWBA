/**
  ******************************************************************************
  * @file    low_level_extmem_device.c
  * @author  MCD Application Team
  * @brief   This file contains device definition for low_level_extmem_device
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#include "loader_flash_layout.h"
#include "low_level_ext_flash.h"

#if defined (OEMIROT_EXTERNAL_FLASH_ENABLE)
static struct ext_flash_range erase_vect[] =
{
 { S_IMAGE_SECONDARY_PARTITION_OFFSET, S_IMAGE_SECONDARY_PARTITION_OFFSET + FLASH_S_PARTITION_SIZE - 1},
 { NS_IMAGE_SECONDARY_PARTITION_OFFSET, NS_IMAGE_SECONDARY_PARTITION_OFFSET + FLASH_NS_PARTITION_SIZE - 1},
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
  { S_DATA_IMAGE_SECONDARY_PARTITION_OFFSET, S_DATA_IMAGE_SECONDARY_PARTITION_OFFSET + FLASH_S_DATA_PARTITION_SIZE - 1},
#endif /* (MCUBOOT_S_DATA_IMAGE_NUMBER == 1) */
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
  { NS_DATA_IMAGE_SECONDARY_PARTITION_OFFSET, NS_DATA_IMAGE_SECONDARY_PARTITION_OFFSET + FLASH_NS_DATA_PARTITION_SIZE - 1},
#endif /* (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1) */
};
static struct ext_flash_range write_vect[] =
{
 { S_IMAGE_SECONDARY_PARTITION_OFFSET, S_IMAGE_SECONDARY_PARTITION_OFFSET + FLASH_S_PARTITION_SIZE - 1},
 { NS_IMAGE_SECONDARY_PARTITION_OFFSET, NS_IMAGE_SECONDARY_PARTITION_OFFSET + FLASH_NS_PARTITION_SIZE - 1},
 { NS_IMAGE_PRIMARY_PARTITION_OFFSET + FLASH_NS_PARTITION_SIZE - 32, NS_IMAGE_PRIMARY_PARTITION_OFFSET + FLASH_NS_PARTITION_SIZE - 16 - 1},
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
  { S_DATA_IMAGE_SECONDARY_PARTITION_OFFSET, S_DATA_IMAGE_SECONDARY_PARTITION_OFFSET + FLASH_S_DATA_PARTITION_SIZE - 1},
#endif /* (MCUBOOT_S_DATA_IMAGE_NUMBER == 1) */
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
  { NS_DATA_IMAGE_SECONDARY_PARTITION_OFFSET, NS_DATA_IMAGE_SECONDARY_PARTITION_OFFSET + FLASH_NS_DATA_PARTITION_SIZE - 1},
  { NS_DATA_IMAGE_PRIMARY_PARTITION_OFFSET, NS_DATA_IMAGE_PRIMARY_PARTITION_OFFSET + FLASH_NS_DATA_PARTITION_SIZE - 1},
#endif /* (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1) */
};

struct low_level_ext_flash_device EXT_FLASH0_DEV =
{
  .erase = { .nb = sizeof(erase_vect) / sizeof(struct ext_flash_range), .range = erase_vect},
  .write = { .nb = sizeof(write_vect) / sizeof(struct ext_flash_range), .range = write_vect},
  .read_error = 1
};
#endif /* OEMIROT_EXTERNAL_FLASH_ENABLE */
