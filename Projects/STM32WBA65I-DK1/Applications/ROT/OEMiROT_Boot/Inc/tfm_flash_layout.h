/*
 * Copyright (c) 2018 Arm Limited. All rights reserved.
 * Copyright (c) 2024 STMicroelectronics. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __TFM_FLASH_LAYOUT_H__
#define __TFM_FLASH_LAYOUT_H__

/* This header file is included from linker scatter file as well, where only a
 * limited C constructs are allowed. Therefore it is not possible to include
 * here the platform_retarget.h to access flash related defines. To resolve this
 * some of the values are redefined here with different names, these are marked
 * with comment.
 */

#define MCUBOOT_OVERWRITE_ONLY          /* Defined: the FW installation uses overwrite method.
                                           UnDefined: The FW installation uses swap mode. */

#define MCUBOOT_APP_IMAGE_NUMBER 2      /* 2: Two separated images for S and NS application binaries. */
#define MCUBOOT_S_DATA_IMAGE_NUMBER 0   /* 0: No S data image. */
#define MCUBOOT_NS_DATA_IMAGE_NUMBER 0  /* 0: No NS data image. */
/* Total number of images */
#define MCUBOOT_IMAGE_NUMBER (MCUBOOT_APP_IMAGE_NUMBER + MCUBOOT_S_DATA_IMAGE_NUMBER + MCUBOOT_NS_DATA_IMAGE_NUMBER)

/* IMAGE_CODE_SIZE is the space available for the software binary image.
 * It is less than the FLASH_PARTITION_SIZE because we reserve space
 * for the image header and trailer introduced by the bootloader.
 */
#define BL2_HEADER_SIZE                         (0x400) /*!< Appli image header size */
#define BL2_TRAILER_SIZE                        (0x2000)

/* Use image hash reference to reduce boot time (signature check bypass) */
#define MCUBOOT_USE_HASH_REF

/* Sector size of the hardware flash */
#define FLASH_AREA_IMAGE_SECTOR_SIZE    (0x2000)     /* 8 KB */
#define FLASH_B_SIZE                    (0x100000)   /* 1 MBytes*/

#define FLASH_TOTAL_SIZE                (FLASH_B_SIZE + FLASH_B_SIZE) /* 2 MBytes */

/* Offset and size definitions of the flash partitions that are handled by the
 * bootloader. The image swapping is done between IMAGE_0 and IMAGE_1, SCRATCH
 * is used as a temporary storage during image swapping.
 */
/* area for image HASH references */
#define FLASH_HASH_REF_AREA_OFFSET      (0x0000)
/**/
#if defined(MCUBOOT_USE_HASH_REF)
#define FLASH_HASH_REF_AREA_SIZE        (FLASH_AREA_IMAGE_SECTOR_SIZE)
#else
#define FLASH_HASH_REF_AREA_SIZE        (0x0000)
#endif /* MCUBOOT_USE_HASH_REF */
/**/
/* area for HUK and anti roll back counter */
#define FLASH_BL2_NVCNT_AREA_OFFSET     (FLASH_HASH_REF_AREA_OFFSET + FLASH_HASH_REF_AREA_SIZE)
#define FLASH_BL2_NVCNT_AREA_SIZE       (FLASH_AREA_IMAGE_SECTOR_SIZE)
/* scratch area */
#define FLASH_AREA_SCRATCH_OFFSET       (FLASH_BL2_NVCNT_AREA_OFFSET + FLASH_BL2_NVCNT_AREA_SIZE)
#if defined(MCUBOOT_OVERWRITE_ONLY)
#define FLASH_AREA_SCRATCH_SIZE         (0x0000) /* Not used in MCUBOOT_OVERWRITE_ONLY mode */
#else
#define FLASH_AREA_SCRATCH_SIZE         (0x10000) /* 64 KB */
#endif
/* control scratch area */
#if (FLASH_AREA_SCRATCH_OFFSET % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_SCRATCH_OFFSET not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /* (FLASH_AREA_SCRATCH_OFFSET % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0*/

/**/
/* personal area */
#define FLASH_AREA_PERSO_OFFSET         (FLASH_BL2_NVCNT_AREA_OFFSET + FLASH_BL2_NVCNT_AREA_SIZE + \
                                         FLASH_AREA_SCRATCH_SIZE)
#define FLASH_AREA_PERSO_SIZE           (0x2000)
/* control personal area */
#if (FLASH_AREA_PERSO_OFFSET % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_PERSO_OFFSET not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /* FLASH_AREA_PERSO_OFFSET % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */
/**/
/* area for BL2 code protected by hdp */
#define FLASH_AREA_BL2_OFFSET           (FLASH_AREA_PERSO_OFFSET+FLASH_AREA_PERSO_SIZE)
/**/
#if !defined(MCUBOOT_PRIMARY_ONLY) && !defined(MCUBOOT_OVERWRITE_ONLY)
#define FLASH_AREA_BL2_SIZE             (0x12000)
#else
#define FLASH_AREA_BL2_SIZE             (0x10000)
#endif
/* Control area */
#if (FLASH_AREA_BL2_SIZE  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_BL2_SIZE  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /* (FLASH_AREA_BL2_SIZE  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */

/* HDP area end at this address */
#define FLASH_BL2_HDP_END               (FLASH_AREA_BL2_OFFSET + FLASH_AREA_BL2_SIZE -1)
/* area for BL2 code not protected by hdp */
#define FLASH_AREA_BL2_NOHDP_OFFSET     (FLASH_AREA_BL2_OFFSET + FLASH_AREA_BL2_SIZE)
#define FLASH_AREA_BL2_NOHDP_SIZE       (FLASH_AREA_IMAGE_SECTOR_SIZE)
/* control area for BL2 code protected by hdp */
#if (FLASH_AREA_BL2_NOHDP_OFFSET % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "HDP area must be aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /* (FLASH_AREA_BL2_NOHDP_OFFSET % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */

/* HDP area end at this address */
#define FLASH_BL2_HDP_END_OFFSET        (FLASH_AREA_SCRATCH_OFFSET + FLASH_AREA_SCRATCH_SIZE)

/* OTP / Non Volatile Counters definitions */
#define FLASH_OTP_NV_COUNTERS_SECTOR_SIZE   (FLASH_AREA_IMAGE_SECTOR_SIZE)
#define FLASH_OTP_NV_COUNTERS_AREA_OFFSET   (FLASH_AREA_BL2_NOHDP_OFFSET + FLASH_AREA_BL2_NOHDP_SIZE)
#define FLASH_OTP_NV_COUNTERS_AREA_SIZE     (FLASH_OTP_NV_COUNTERS_SECTOR_SIZE + FLASH_OTP_NV_COUNTERS_SECTOR_SIZE)

/* Secure Storage (PS) Service definitions */
#define FLASH_PS_AREA_OFFSET           (FLASH_OTP_NV_COUNTERS_AREA_OFFSET+FLASH_OTP_NV_COUNTERS_AREA_SIZE)
#define FLASH_PS_AREA_SIZE             (FLASH_AREA_IMAGE_SECTOR_SIZE + FLASH_AREA_IMAGE_SECTOR_SIZE)

/* Control Secure Storage (PS) Service definitions*/
#if (FLASH_PS_AREA_OFFSET % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_PS_AREA_OFFSET not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /* (FLASH_PS_AREA_OFFSET % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */

/* Internal Trusted Storage (ITS) Service definitions */
#define FLASH_ITS_AREA_OFFSET           (FLASH_PS_AREA_OFFSET+FLASH_PS_AREA_SIZE)
#define FLASH_ITS_AREA_SIZE             (FLASH_AREA_IMAGE_SECTOR_SIZE + FLASH_AREA_IMAGE_SECTOR_SIZE)

/*Control Internal Trusted Storage (ITS) Service definitions */
#if (FLASH_ITS_AREA_OFFSET % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_ITS_AREA_OFFSET not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /* (FLASH_ITS_AREA_OFFSET % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */

#define FLASH_AREA_BEGIN_OFFSET         (FLASH_ITS_AREA_OFFSET + FLASH_ITS_AREA_SIZE)

/* Flash layout info for BL2 bootloader */
#define FLASH_BASE_ADDRESS              (0x0C000000) /* same as FLASH0_BASE_S */

#define FLASH_AREAS_DEVICE_ID           (FLASH_DEVICE_ID - FLASH_DEVICE_ID)

/* The size of S partition */
#define FLASH_S_PARTITION_SIZE          (0x40000) /* 256 KB for S partition */
#define FLASH_S_DATA_PARTITION_SIZE     (0x0)
/* The size of NS partition */
#define FLASH_NS_PARTITION_SIZE         (0x80000) /* 512 KB for NS partition */

#if (FLASH_S_PARTITION_SIZE > FLASH_NS_PARTITION_SIZE)
#define FLASH_MAX_PARTITION_SIZE FLASH_S_PARTITION_SIZE
#else
#define FLASH_MAX_PARTITION_SIZE FLASH_NS_PARTITION_SIZE
#endif

/* Secure image primary slot */
#define FLASH_AREA_0_ID            (1)
#define FLASH_AREA_0_DEVICE_ID     (FLASH_AREAS_DEVICE_ID)
#define FLASH_AREA_0_OFFSET        (FLASH_AREA_BEGIN_OFFSET)
#define FLASH_AREA_0_SIZE          (FLASH_S_PARTITION_SIZE)
/* Control Secure app image primary slot */
#if (FLASH_AREA_0_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_0_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /*  (FLASH_AREA_0_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */

/* Non-secure image primary slot */
#define FLASH_AREA_1_ID            (2)
#define FLASH_AREA_1_DEVICE_ID     (FLASH_AREAS_DEVICE_ID)
#define FLASH_AREA_1_OFFSET        (FLASH_AREA_0_OFFSET + FLASH_AREA_0_SIZE)
#define FLASH_AREA_1_SIZE          (FLASH_NS_PARTITION_SIZE)
/* Control Non-secure app image primary slot */
#if (FLASH_AREA_1_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_1_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /* (FLASH_AREA_1_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0  */

/* Secure image secondary slot */
#define FLASH_AREA_2_ID            (3)
#define FLASH_AREA_2_DEVICE_ID     (FLASH_AREAS_DEVICE_ID)
#define FLASH_AREA_2_OFFSET        (FLASH_AREA_1_OFFSET + FLASH_AREA_1_SIZE)
#define FLASH_AREA_2_SIZE          (FLASH_S_PARTITION_SIZE)

/* Control Secure image secondary slot */
#if (FLASH_AREA_2_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_2_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /*   (FLASH_AREA_2_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */

/* Non-secure image secondary slot */
#define FLASH_AREA_3_ID            (4)
#define FLASH_AREA_3_DEVICE_ID     (FLASH_AREAS_DEVICE_ID)
#define FLASH_AREA_3_OFFSET        (FLASH_AREA_2_OFFSET + FLASH_AREA_2_SIZE)
#define FLASH_AREA_3_SIZE          (FLASH_NS_PARTITION_SIZE)
/* Control Non-Secure image secondary slot */
#if (FLASH_AREA_3_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_3_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /*  (FLASH_AREA_3_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */

#define FLASH_AREA_4_ID                 (5)
#define FLASH_AREA_4_OFFSET             (0x0)
#define FLASH_AREA_4_SIZE               (0x0)

#define FLASH_AREA_5_ID                 (6)
#define FLASH_AREA_5_OFFSET             (0x0)
#define FLASH_AREA_5_SIZE               (0x0)

#define FLASH_AREA_6_ID                 (7)
#define FLASH_AREA_6_OFFSET             (0x0)
#define FLASH_AREA_6_SIZE               (0x0)

#define FLASH_AREA_7_ID                 (8)
#define FLASH_AREA_7_OFFSET             (0x0)
#define FLASH_AREA_7_SIZE               (0x0)

/* Scratch area */
#define FLASH_AREA_SCRATCH_ID      (9)

/*
 * The maximum number of status entries supported by the bootloader.
 */
#if defined(MCUBOOT_OVERWRITE_ONLY)
#define MCUBOOT_STATUS_MAX_ENTRIES        (0)
#else /* not MCUBOOT_OVERWRITE_ONLY */
#define MCUBOOT_STATUS_MAX_ENTRIES        (((FLASH_MAX_PARTITION_SIZE - 1) / \
                                            FLASH_AREA_SCRATCH_SIZE) + 1)
#endif /* MCUBOOT_OVERWRITE_ONLY */

/* Maximum number of image sectors supported by the bootloader. */
#define MCUBOOT_MAX_IMG_SECTORS    (FLASH_MAX_PARTITION_SIZE / \
                                    FLASH_AREA_IMAGE_SECTOR_SIZE)

/* flash areas end offset */
#define FLASH_AREA_END_OFFSET           (FLASH_AREA_BEGIN_OFFSET + FLASH_AREA_0_SIZE + FLASH_AREA_1_SIZE + \
                                         FLASH_AREA_2_SIZE + FLASH_AREA_3_SIZE)
/* Control flash area end */
#if (FLASH_AREA_END_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_END_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /*  (FLASH_AREA_END_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */

#define FLASH_PARTITION_SIZE            (FLASH_S_PARTITION_SIZE+FLASH_NS_PARTITION_SIZE)

#define SECURE_IMAGE_OFFSET             (0x0)
#define SECURE_IMAGE_MAX_SIZE           FLASH_S_PARTITION_SIZE

#define NON_SECURE_IMAGE_OFFSET         (SECURE_IMAGE_OFFSET + SECURE_IMAGE_MAX_SIZE)
#define NON_SECURE_IMAGE_MAX_SIZE       FLASH_NS_PARTITION_SIZE

/* Flash device name used by BL2 and NV Counter
 * Name is defined in flash driver file: low_level_flash.c
 */
#if !defined(MCUBOOT_OVERWRITE_ONLY)
/* Flash Driver Used to Confirm Secure App Image */
#define  FLASH_PRIMARY_SECURE_DEV_NAME             Driver_FLASH0
#endif /* !defined(MCUBOOT_OVERWRITE_ONLY) */
/* Flash Driver Used to Confirm NonSecure App Image or MCUBOOT_APP_IMAGE_NUMBER = 1 */
#define  FLASH_PRIMARY_NONSECURE_DEV_NAME          Driver_FLASH0
#define FLASH_DEV_NAME                             Driver_FLASH0

/* BL2 NV Counters definitions  */
#define BL2_NV_COUNTERS_AREA_ADDR       FLASH_BL2_NVCNT_AREA_OFFSET
#define BL2_NV_COUNTERS_AREA_SIZE       FLASH_BL2_NVCNT_AREA_SIZE

#endif /* __TFM_FLASH_LAYOUT_H__ */
