/**
  ******************************************************************************
  * @file    loader_flash_layout.h
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

#ifndef __LOADER_FLASH_LAYOUT_H__
#define __LOADER_FLASH_LAYOUT_H__

/********** Flash layout configuration : begin **********/

#define LOADER_NS_HEAP_SIZE                  0x0000200  /* EWARM / MDK-ARM */
#define LOADER_NS_MSP_STACK_SIZE             0x0000400  /* EWARM / MDK-ARM / STM32CubeIDE*/
#define LOADER_NS_PSP_STACK_SIZE             0x0000400  /* MDK-ARM */

#define LOADER_S_HEAP_SIZE                   0x0000200  /* EWARM / MDK-ARM */
#define LOADER_S_MSP_STACK_SIZE              0x0000400  /* EWARM / MDK-ARM / STM32CubeIDE*/
#define LOADER_S_PSP_STACK_SIZE              0x0000400  /* MDK-ARM */

#define NSC_LOADER_SIZE                      0x100

/********** Flash layout configuration : end **********/

/********** All define are updated automatically from ROT_BOOT project : begin **********/

#define MCUBOOT_OVERWRITE_ONLY                 /* Defined: the FW installation uses overwrite method.
                                                  UnDefined: The FW installation uses swap mode. */

#define MCUBOOT_APP_IMAGE_NUMBER       0x2      /* 1: S and NS application binaries are assembled in one single image.
                                                   2: Two separated images for S and NS application binaries. */

#define MCUBOOT_S_DATA_IMAGE_NUMBER    0x0      /* 1: S data image for S application.
                                                   0: No S data image. */

#define MCUBOOT_NS_DATA_IMAGE_NUMBER   0x0      /* 1: NS data image for NS application.
                                                   0: No NS data image. */

#define FLASH_AREA_0_OFFSET            0x18000 /* Secure app image primary slot offset */

#define FLASH_AREA_0_SIZE              0x8000   /* Secure app image primary slot size */

#define FLASH_AREA_1_OFFSET            0x20000  /* Non-secure app image primary slot offset */

#define FLASH_AREA_1_SIZE              0x32000   /* Non-secure app image primary slot size */

#define FLASH_AREA_2_OFFSET            0x52000  /* Secure app image secondary slot */

#define FLASH_AREA_2_SIZE              0x8000   /* Secure app image secondary slot size */

#define FLASH_AREA_3_OFFSET            0x5A000 /* Non-secure app image secondary slot offset */

#define FLASH_AREA_3_SIZE              0x32000   /* Non-secure app image secondary slot size */

#define FLASH_AREA_4_OFFSET            0x0 /* Secure data image primary slot offset */

#define FLASH_AREA_4_SIZE              0x0   /* Secure data image primary slot size */

#define FLASH_AREA_5_OFFSET            0x0  /* Non-secure data image primary slot offset */

#define FLASH_AREA_5_SIZE              0x0   /* Non-secure data image primary slot size */

#define FLASH_AREA_6_OFFSET            0x0 /* Non-secure data image primary slot offset */

#define FLASH_AREA_6_SIZE              0x0   /* Non-secure data image primary slot size */

#define FLASH_AREA_7_OFFSET            0x0 /* Non-secure data image primary slot offset */

#define FLASH_AREA_7_SIZE              0x0   /* Non-secure data image primary slot size */

#define FLASH_PARTITION_SIZE           0x3A000  /* Secure and Non Secure partition size */

#define FLASH_NS_PARTITION_SIZE        0x32000  /* Non Secure partition size */

#define FLASH_S_PARTITION_SIZE         0x8000   /* secure partition size */

#define FLASH_S_DATA_PARTITION_SIZE    0x0   /* secure data partition size */

#define FLASH_NS_DATA_PARTITION_SIZE   0x0   /* non secure data partition size */

#define FLASH_B_SIZE                   0x100000   /* flash bank size: 1 MBytes*/

/********** All define are updated automatically from ROT_BOOT project : End **********/

#if defined(STM32WBA65xx)
#define FLASH_TOTAL_SIZE               (FLASH_B_SIZE + FLASH_B_SIZE) /* total flash size: 2 MBytes */
#elif defined(STM32WBA52xx) || defined(STM32WBA55xx)
#define FLASH_TOTAL_SIZE               (FLASH_B_SIZE) /* total flash size: 1 MBytes */
#else
#error "Unknown target."
#endif

#if  defined(OEMIROT_EXTERNAL_FLASH_ENABLE)
#define LOADER_FLASH_DEV_NAME             Driver_SPI_FLASH0
#else
#define LOADER_FLASH_DEV_NAME             Driver_FLASH0
#endif  /*  defined(OEMIROT_EXTERNAL_FLASH_ENABLE)  */

#endif  /* __LOADER_FLASH_LAYOUT_H__ */