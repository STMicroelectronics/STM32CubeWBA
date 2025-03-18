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

#ifndef __FLASH_LAYOUT_H__
#define __FLASH_LAYOUT_H__

/* This header file is included from linker scatter file as well, where only a
 * limited C constructs are allowed. Therefore it is not possible to include
 * here the platform_retarget.h to access flash related defines. To resolve this
 * some of the values are redefined here with different names, these are marked
 * with comment.
 */

/* OEMiROT/OEMuROT configuration */
/*#define OEMUROT_ENABLE*/                 /* Do not edit. Automatically configured by
                                          ROT provisioning script, depending on bootpath selected.
                                          Defined: the project is used for OEMuRoT boot stage
                                          Undefined: the project is used for OEMuRoT boot stage */

/* Flash layout configuration : begin */
#if !defined (OEMUROT_ENABLE)
/*#define OEMIROT_FIRST_BOOT_STAGE*/ /* Undefined: the project is used to generate OEMiRoT for OEMiRoT boot path
                                        Defined: the project is used to generate OEMiRoT as first boot stage for OEMiRoT_OEMuRoT boot path */

#endif /* OEMUROT_ENABLE */

#define MCUBOOT_OVERWRITE_ONLY          /* Defined: the FW installation uses overwrite method.
                                           UnDefined: The FW installation uses swap mode. */

#define MCUBOOT_EXT_LOADER              /* Defined: Add external local loader application.
                                               To enter it, press user button at reset.
                                           Undefined: No external local loader application. */

#if defined (OEMUROT_ENABLE)
/*#define OEMIROT_EXTERNAL_FLASH_ENABLE*/   /* Defined: External OSPI flash used only for all secondary slots.
                                               Undefined: External OSPI flash not used. */
#endif /* OEMUROT_ENABLE */

#define MCUBOOT_APP_IMAGE_NUMBER 2      /* 1: S application only if FLASH_NS_PARTITION_SIZE = 0 ,
                                              else S and NS application binaries assembled in one single image.
                                           2: Two separated images for S and NS application binaries. */

#define MCUBOOT_S_DATA_IMAGE_NUMBER 0   /* 1: S data image for S application.
                                           0: No S data image. */

#define MCUBOOT_NS_DATA_IMAGE_NUMBER 0  /* 1: NS data image for NS application.
                                           0: No NS data image. */

/* Flash layout configuration : end */

/* Use secure OEMiRoT as first boot stage */
#if defined (OEMIROT_FIRST_BOOT_STAGE)
#undef MCUBOOT_APP_IMAGE_NUMBER
#undef MCUBOOT_S_DATA_IMAGE_NUMBER
#undef MCUBOOT_NS_DATA_IMAGE_NUMBER
#undef OEMIROT_EXTERNAL_FLASH_ENABLE
#define MCUBOOT_APP_IMAGE_NUMBER 1      /* App image contains OEMuRoT code. */
#define MCUBOOT_S_DATA_IMAGE_NUMBER 0   /* No S Data image for OEMuRoT. */
#define MCUBOOT_NS_DATA_IMAGE_NUMBER 0  /* No NS Data image for OEMuRoT. */
#endif /* OEMIROT_FIRST_BOOT_STAGE */

/* Total number of images */
#define MCUBOOT_IMAGE_NUMBER (MCUBOOT_APP_IMAGE_NUMBER + MCUBOOT_S_DATA_IMAGE_NUMBER + MCUBOOT_NS_DATA_IMAGE_NUMBER)

/* Use image hash reference to reduce boot time (signature check bypass) */
#define MCUBOOT_USE_HASH_REF

/* The size of a partition. This should be large enough to contain a S or NS
 * sw binary. Each FLASH_AREA_IMAGE contains two partitions. See Flash layout
 * above.
 */
#if  defined(OEMIROT_EXTERNAL_FLASH_ENABLE)
#define SPI_FLASH_DEV_NAME        Driver_SPI_FLASH0
#endif /* OEMIROT_EXTERNAL_FLASH_ENABLE */
#define FLASH_DEV_NAME             Driver_FLASH0

/* Flash layout info for BL2 bootloader */
#define FLASH_AREA_IMAGE_SECTOR_SIZE    (0x2000)     /* 8 KB */
#define FLASH_B_SIZE                    (0x100000)   /* 1 MBytes*/
#if defined(STM32WBA65xx)
#define FLASH_TOTAL_SIZE                (FLASH_B_SIZE + FLASH_B_SIZE) /* 2 MBytes */
#elif defined(STM32WBA52xx) || defined(STM32WBA55xx)
#define FLASH_TOTAL_SIZE                (FLASH_B_SIZE) /* 1 MBytes */
#else
#error "Unknown target."
#endif
#define FLASH_BASE_ADDRESS              (0x0c000000)

#if  defined(OEMIROT_EXTERNAL_FLASH_ENABLE)
/* External SPI Flash layout info for BL2 bootloader */
#define SPI_FLASH_TOTAL_SIZE           (0x400000)  /* 32 Mbits */
#define SPI_FLASH_BASE_ADDRESS         (0x00000000)

/* Flash device ID */
#define SPI_FLASH_DEVICE_ID            (FLASH_DEVICE_ID + 1) /* SPI Flash */
#endif /* OEMIROT_EXTERNAL_FLASH_ENABLE */

/* Flash area IDs */
#define FLASH_AREA_0_ID                 (1)
#if (MCUBOOT_APP_IMAGE_NUMBER == 2)
#define FLASH_AREA_1_ID                 (2)
#endif /* MCUBOOT_APP_IMAGE_NUMBER == 2 */
#if !defined(MCUBOOT_PRIMARY_ONLY)
#define FLASH_AREA_2_ID                 (3)
#if (MCUBOOT_APP_IMAGE_NUMBER == 2)
#define FLASH_AREA_3_ID                 (4)
#endif /* MCUBOOT_APP_IMAGE_NUMBER == 2 */
#endif /* MCUBOOT_PRIMARY_ONLY */
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
#define FLASH_AREA_4_ID                 (5)
#endif /* MCUBOOT_S_DATA_IMAGE_NUMBER == 1 */
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
#define FLASH_AREA_5_ID                 (6)
#endif /* MCUBOOT_NS_DATA_IMAGE_NUMBER == 1 */
#if !defined(MCUBOOT_PRIMARY_ONLY)
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
#define FLASH_AREA_6_ID                 (7)
#endif /* MCUBOOT_S_DATA_IMAGE_NUMBER == 1 */
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
#define FLASH_AREA_7_ID                 (8)
#endif /* MCUBOOT_NS_DATA_IMAGE_NUMBER == 1 */
#define FLASH_AREA_SCRATCH_ID           (9)
#endif /* MCUBOOT_PRIMARY_ONLY */

/* Hardcoded value necessary for OEMiROT_OEMuROT : begin */
#if defined(OEMIROT_FIRST_BOOT_STAGE) || defined(OEMUROT_ENABLE)

/*
 * Those values are defined for OEMIROT_FIRST_BOOT_STAGE, but 
 * we also need some of them for OEMuROT provisioning.
 */
#if defined(STM32WBA65xx)
#define OEMIROT_AREA_1_SIZE             (0x1B8000)      /* 1760K */
#define OEMIROT_AREA_2_OFFSET           (0x0C1E8000)    /* slot 2 address when flash size: 2 MBytes */
#elif defined(STM32WBA52xx) || defined(STM32WBA55xx)
#define OEMIROT_AREA_1_SIZE             (0xB8000)       /* 736K */
#define OEMIROT_AREA_2_OFFSET           (0x0C0E8000),   /* slot 2 address when flash size: 1 MBytes */
#else
#error "Offset and size not defined for another target"
#endif
#define OEMIROT_AREA_BEGIN_OFFSET       (0x18000)
#define OEMIROT_AREA_BL2_OFFSET         (0x0C006000)
#define OEMIROT_BL2_WRP_START           (0x4000)

#endif  /* OEMIROT_FIRST_BOOT_STAGE || OEMUROT_ENABLE */
/* Hardcoded value necessary for OEMiROT_OEMuROT : end */

/* Offset and size definitions of the flash partitions that are handled by the
 * bootloader. The image swapping is done between IMAGE_0 and IMAGE_1, SCRATCH
 * is used as a temporary storage during image swapping.
 */
/* area for image HASH references */
#if defined (OEMUROT_ENABLE)
#define FLASH_HASH_REF_AREA_OFFSET      OEMIROT_AREA_BEGIN_OFFSET
#else
#define FLASH_HASH_REF_AREA_OFFSET      (0x0000)
#endif /* OEMUROT_ENABLE */
#if defined(MCUBOOT_USE_HASH_REF)
#define FLASH_HASH_REF_AREA_SIZE        (FLASH_AREA_IMAGE_SECTOR_SIZE)
#else
#define FLASH_HASH_REF_AREA_SIZE        (0x0000)
#endif /* MCUBOOT_USE_HASH_REF */

/* area for HUK and anti roll back counter */
#define FLASH_BL2_NVCNT_AREA_OFFSET     (FLASH_HASH_REF_AREA_OFFSET + FLASH_HASH_REF_AREA_SIZE)
#define FLASH_BL2_NVCNT_AREA_SIZE       (FLASH_AREA_IMAGE_SECTOR_SIZE)

/* scratch area */
#if defined(FLASH_AREA_SCRATCH_ID)
#define FLASH_AREA_SCRATCH_DEVICE_ID    (FLASH_DEVICE_ID - FLASH_DEVICE_ID)
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
#else /* FLASH_AREA_SCRATCH_ID */
#define FLASH_AREA_SCRATCH_SIZE         (0x0)
#endif /* FLASH_AREA_SCRATCH_ID */

/* personal area */
#define FLASH_AREA_PERSO_OFFSET         (FLASH_BL2_NVCNT_AREA_OFFSET + FLASH_BL2_NVCNT_AREA_SIZE + \
                                         FLASH_AREA_SCRATCH_SIZE)
#define FLASH_AREA_PERSO_SIZE           (0x2000)
/* control personal area */
#if (FLASH_AREA_PERSO_OFFSET % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_PERSO_OFFSET not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /* FLASH_AREA_PERSO_OFFSET % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */

/* area for BL2 code protected by hdp */
#define FLASH_AREA_BL2_OFFSET           (FLASH_AREA_PERSO_OFFSET+FLASH_AREA_PERSO_SIZE)
#if !defined(MCUBOOT_OVERWRITE_ONLY)
#define FLASH_AREA_BL2_SIZE             (0x12000)
#else
#define FLASH_AREA_BL2_SIZE             (0x10000)
#endif

/* HDP area end at this address */
#define FLASH_BL2_HDP_END               (FLASH_AREA_BL2_OFFSET+FLASH_AREA_BL2_SIZE-1)
/* area for BL2 code not protected by hdp */
#define FLASH_AREA_BL2_NOHDP_OFFSET     (FLASH_AREA_BL2_OFFSET+FLASH_AREA_BL2_SIZE)
#define FLASH_AREA_BL2_NOHDP_SIZE       (FLASH_AREA_IMAGE_SECTOR_SIZE)
/* control area for BL2 code protected by hdp */
#if (FLASH_AREA_BL2_NOHDP_OFFSET % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "HDP area must be aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /* (FLASH_AREA_BL2_NOHDP_OFFSET % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */

/* BL2 partitions size */
#if  defined(OEMIROT_FIRST_BOOT_STAGE)
/* For OEMIROT_FIRST_BOOT_STAGE we set 0 KB for NS partition */
#define FLASH_NS_PARTITION_SIZE         (0x0)

#else /* OEMIROT_FIRST_BOOT_STAGE */
#if defined(MCUBOOT_PRIMARY_ONLY)
/* For PRIMARY_ONLY we need 520 KB for NS partition */
#define FLASH_NS_PARTITION_SIZE         (0x80000)

#else
/* For Others case, we limit the size to 200 KB for NS partition (except for FULL SECURE) */
#define FLASH_NS_PARTITION_SIZE         (0x32000)

#endif /* MCUBOOT_PRIMARY_ONLY */
#endif /* OEMIROT_FIRST_BOOT_STAGE */

#if  defined(OEMIROT_FIRST_BOOT_STAGE)
/* For OEMIROT_FIRST_BOOT_STAGE we need 72K KB for S partition (FLASH_AREA_BL2_SIZE+FLASH_AREA_BL2_NOHDP_SIZE) */
#define FLASH_S_PARTITION_SIZE          (FLASH_AREA_BL2_SIZE + 0x2000)

#elif (MCUBOOT_APP_IMAGE_NUMBER == 1) && (FLASH_NS_PARTITION_SIZE == 0)
/* For FULL SECURE case, we need at least 32 KB for S partition */
#define FLASH_S_PARTITION_SIZE          (0x08000)

#else
/* For Others case, we keep 32 KB for S partition (like FULL SECURE) */
#define FLASH_S_PARTITION_SIZE          (0x08000)

#endif /* OEMIROT_FIRST_BOOT_STAGE */

#define FLASH_PARTITION_SIZE            (FLASH_S_PARTITION_SIZE+FLASH_NS_PARTITION_SIZE)

#if (MCUBOOT_APP_IMAGE_NUMBER == 2)
#define FLASH_MAX_APP_PARTITION_SIZE    ((FLASH_S_PARTITION_SIZE >   \
                                          FLASH_NS_PARTITION_SIZE) ? \
                                         FLASH_S_PARTITION_SIZE : \
                                         FLASH_NS_PARTITION_SIZE)
#else
#define FLASH_MAX_APP_PARTITION_SIZE    FLASH_PARTITION_SIZE
#endif /* (MCUBOOT_APP_IMAGE_NUMBER == 2) */
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
#define FLASH_S_DATA_PARTITION_SIZE     (FLASH_AREA_IMAGE_SECTOR_SIZE)
#else
#define FLASH_S_DATA_PARTITION_SIZE     (0x0)
#endif /* (MCUBOOT_S_DATA_IMAGE_NUMBER == 1) */
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
#define FLASH_NS_DATA_PARTITION_SIZE    (FLASH_AREA_IMAGE_SECTOR_SIZE)
#else
#define FLASH_NS_DATA_PARTITION_SIZE    (0x0)
#endif /* (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1) */

#define FLASH_MAX_DATA_PARTITION_SIZE   ((FLASH_S_DATA_PARTITION_SIZE >   \
                                          FLASH_NS_DATA_PARTITION_SIZE) ? \
                                         FLASH_S_DATA_PARTITION_SIZE : \
                                         FLASH_NS_DATA_PARTITION_SIZE)
#define FLASH_MAX_PARTITION_SIZE        ((FLASH_MAX_APP_PARTITION_SIZE >   \
                                          FLASH_MAX_DATA_PARTITION_SIZE) ? \
                                         FLASH_MAX_APP_PARTITION_SIZE : \
                                         FLASH_MAX_DATA_PARTITION_SIZE)

/* BL2 flash areas */
#if defined (OEMIROT_FIRST_BOOT_STAGE)
#define OEMUROT_HASH_REF_SIZE           (0x2000)
#define OEMUROT_BL2_NVCNT_SIZE          (0x2000)

#if defined(MCUBOOT_OVERWRITE_ONLY)
#define OEMUROT_SCRATCH_SIZE            (0x0) /* Not used in MCUBOOT_OVERWRITE_ONLY mode */
#else
#define OEMUROT_SCRATCH_SIZE            (0x10000) /* 64 KB */
#endif /* MCUBOOT_OVERWRITE_ONLY */

#define OEMUROT_PERSO_SIZE              (0x2000)
#define FLASH_AREA_BEGIN_OFFSET         (FLASH_AREA_BL2_NOHDP_OFFSET + FLASH_AREA_BL2_NOHDP_SIZE + \
                                         OEMUROT_HASH_REF_SIZE + OEMUROT_BL2_NVCNT_SIZE + \
                                         OEMUROT_SCRATCH_SIZE + OEMUROT_PERSO_SIZE)
#else /* OEMIROT_FIRST_BOOT_STAGE */
#define OEMIROT_AREA_0_OFFSET           (OEMIROT_AREA_BEGIN_OFFSET + FLASH_AREA_IMAGE_SECTOR_SIZE + FLASH_AREA_IMAGE_SECTOR_SIZE + FLASH_AREA_IMAGE_SECTOR_SIZE)
#define FLASH_AREA_BEGIN_OFFSET         (FLASH_AREA_BL2_NOHDP_OFFSET + FLASH_AREA_BL2_NOHDP_SIZE)
#endif /* OEMIROT_FIRST_BOOT_STAGE */

#define FLASH_AREAS_DEVICE_ID           (FLASH_DEVICE_ID - FLASH_DEVICE_ID)

/* Secure data image primary slot */
#if defined (FLASH_AREA_4_ID)
#define FLASH_AREA_4_DEVICE_ID          (FLASH_AREAS_DEVICE_ID)
#define FLASH_AREA_4_OFFSET             (FLASH_AREA_BEGIN_OFFSET)
#define FLASH_AREA_4_SIZE               (FLASH_S_DATA_PARTITION_SIZE)


/* Control Secure data image primary slot */
#if (FLASH_AREA_4_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_4_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /* (FLASH_AREA_4_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */
#else /* FLASH_AREA_4_ID */
#define FLASH_AREA_4_OFFSET             (0x0)
#define FLASH_AREA_4_SIZE               (0x0)
#endif /* FLASH_AREA_4_ID */

/* Secure app image primary slot */
#if defined(FLASH_AREA_0_ID)
#define FLASH_AREA_0_DEVICE_ID          (FLASH_AREAS_DEVICE_ID)
#define FLASH_AREA_0_OFFSET             (FLASH_AREA_BEGIN_OFFSET + FLASH_AREA_4_SIZE)
#if (MCUBOOT_APP_IMAGE_NUMBER == 2)
#define FLASH_AREA_0_SIZE               (FLASH_S_PARTITION_SIZE)
#else
#if defined(OEMIROT_FIRST_BOOT_STAGE)
#define FLASH_AREA_0_SIZE               (FLASH_S_PARTITION_SIZE)
#else
#define FLASH_AREA_0_SIZE               (FLASH_PARTITION_SIZE)
#endif /* OEMIROT_FIRST_BOOT_STAGE */
#endif /* (MCUBOOT_APP_IMAGE_NUMBER == 2) */
/* Control Secure app image primary slot */
#if (FLASH_AREA_0_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_0_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /*  (FLASH_AREA_0_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */
#else /* FLASH_AREA_0_ID */
#define FLASH_AREA_0_OFFSET             (0x0)
#define FLASH_AREA_0_SIZE               (0x0)
#endif /* FLASH_AREA_0_ID */

/* Non-secure app image primary slot */
#if defined(FLASH_AREA_1_ID)
#define FLASH_AREA_1_DEVICE_ID          (FLASH_AREAS_DEVICE_ID)
#define FLASH_AREA_1_OFFSET             (FLASH_AREA_BEGIN_OFFSET + FLASH_AREA_4_SIZE + \
                                         FLASH_AREA_0_SIZE)
#define FLASH_AREA_1_SIZE               (FLASH_NS_PARTITION_SIZE)
/* Control Non-secure app image primary slot */
#if (FLASH_AREA_1_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_1_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /* (FLASH_AREA_1_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0  */
#else /* FLASH_AREA_1_ID */
#define FLASH_AREA_1_OFFSET             (0x0)
#define FLASH_AREA_1_SIZE               (0x0)
#endif /* FLASH_AREA_1_ID */

/* Non-secure data image primary slot */
#if defined(FLASH_AREA_5_ID)
#define FLASH_AREA_5_DEVICE_ID          (FLASH_AREAS_DEVICE_ID)
#define FLASH_AREA_5_OFFSET             (FLASH_AREA_BEGIN_OFFSET + FLASH_AREA_4_SIZE + \
                                         FLASH_AREA_0_SIZE + FLASH_AREA_1_SIZE)
#define FLASH_AREA_5_SIZE               (FLASH_NS_DATA_PARTITION_SIZE)
/* Control Non-secure data image primary slot */
#if (FLASH_AREA_5_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_5_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /* (FLASH_AREA_5_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0  */
#else /* FLASH_AREA_5_ID */
#define FLASH_AREA_5_OFFSET             (0x0)
#define FLASH_AREA_5_SIZE               (0x0)
#endif /* FLASH_AREA_5_ID */

/* Secure app image secondary slot */
#if defined(FLASH_AREA_2_ID)
#if defined(OEMIROT_EXTERNAL_FLASH_ENABLE)
#define FLASH_DEV_NAME_2                 SPI_FLASH_DEV_NAME
#define FLASH_DEVICE_ID_2               (SPI_FLASH_DEVICE_ID)
#define FLASH_AREA_2_OFFSET             (SPI_FLASH_BASE_ADDRESS)
#if (MCUBOOT_APP_IMAGE_NUMBER == 2)
#define FLASH_AREA_2_SIZE               (FLASH_S_PARTITION_SIZE)
#else
#define FLASH_AREA_2_SIZE               (FLASH_PARTITION_SIZE)
#endif /* (MCUBOOT_APP_IMAGE_NUMBER == 2) */
#else /* OEMIROT_EXTERNAL_FLASH_ENABLE */
#define FLASH_DEV_NAME_2                 FLASH_DEV_NAME
#define FLASH_DEVICE_ID_2               (FLASH_AREAS_DEVICE_ID)
#if defined(OEMIROT_FIRST_BOOT_STAGE)
#undef FLASH_AREA_1_SIZE
#undef FLASH_AREA_5_SIZE
#define FLASH_AREA_1_SIZE               OEMIROT_AREA_1_SIZE
#define FLASH_AREA_5_SIZE               (0x0)
#endif /* OEMIROT_FIRST_BOOT_STAGE */
#define FLASH_AREA_2_OFFSET             (FLASH_AREA_BEGIN_OFFSET + FLASH_AREA_4_SIZE + \
                                         FLASH_AREA_0_SIZE + FLASH_AREA_1_SIZE + \
                                         FLASH_AREA_5_SIZE)
#if (MCUBOOT_APP_IMAGE_NUMBER == 2)
#define FLASH_AREA_2_SIZE               (FLASH_S_PARTITION_SIZE)
#else
#define FLASH_AREA_2_SIZE               (FLASH_PARTITION_SIZE)
#endif /* (MCUBOOT_APP_IMAGE_NUMBER == 2) */
/* Control Secure app image secondary slot */
#if (FLASH_AREA_2_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_2_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /*   (FLASH_AREA_2_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */
#endif /* OEMIROT_EXTERNAL_FLASH_ENABLE */
#else /* FLASH_AREA_2_ID */
#define FLASH_AREA_2_SIZE               (0x0)
#endif /* FLASH_AREA_2_ID */

/* Non-secure app image secondary slot */
#if defined(FLASH_AREA_3_ID)
#if defined(OEMIROT_EXTERNAL_FLASH_ENABLE)
#define FLASH_DEV_NAME_3                 SPI_FLASH_DEV_NAME
#define FLASH_DEVICE_ID_3               (SPI_FLASH_DEVICE_ID)
#define FLASH_AREA_3_OFFSET             (SPI_FLASH_BASE_ADDRESS + FLASH_AREA_2_SIZE)
#define FLASH_AREA_3_SIZE               (FLASH_NS_PARTITION_SIZE)
#else /* OEMIROT_EXTERNAL_FLASH_ENABLE */
#define FLASH_DEV_NAME_3                 FLASH_DEV_NAME
#define FLASH_DEVICE_ID_3               (FLASH_AREAS_DEVICE_ID)
#define FLASH_AREA_3_OFFSET             (FLASH_AREA_BEGIN_OFFSET + FLASH_AREA_4_SIZE + \
                                         FLASH_AREA_0_SIZE + FLASH_AREA_1_SIZE + \
                                         FLASH_AREA_5_SIZE + FLASH_AREA_2_SIZE)
#define FLASH_AREA_3_SIZE               (FLASH_NS_PARTITION_SIZE)
/* Control Non-Secure app image secondary slot */
#if (FLASH_AREA_3_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_3_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /*  (FLASH_AREA_3_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */
#endif /* OEMIROT_EXTERNAL_FLASH_ENABLE */
#else /* FLASH_AREA_3_ID */
#define FLASH_AREA_3_OFFSET             (0x0)
#define FLASH_AREA_3_SIZE               (0x0)
#endif /* FLASH_AREA_3_ID */

/* Secure data image secondary slot */
#if defined(FLASH_AREA_6_ID)
#if defined(OEMIROT_EXTERNAL_FLASH_ENABLE)
#define FLASH_DEV_NAME_6                 SPI_FLASH_DEV_NAME
#define FLASH_DEVICE_ID_6               (SPI_FLASH_DEVICE_ID)
#define FLASH_AREA_6_OFFSET             (SPI_FLASH_BASE_ADDRESS + FLASH_AREA_2_SIZE + \
                                         FLASH_AREA_3_SIZE)
#define FLASH_AREA_6_SIZE               (FLASH_S_DATA_PARTITION_SIZE)
#else /* OEMIROT_EXTERNAL_FLASH_ENABLE */
#define FLASH_DEV_NAME_6                 FLASH_DEV_NAME
#define FLASH_DEVICE_ID_6               (FLASH_AREAS_DEVICE_ID)
#define FLASH_AREA_6_OFFSET             (FLASH_AREA_BEGIN_OFFSET + FLASH_AREA_4_SIZE + \
                                         FLASH_AREA_0_SIZE + FLASH_AREA_1_SIZE + \
                                         FLASH_AREA_5_SIZE + FLASH_AREA_2_SIZE + \
                                         FLASH_AREA_3_SIZE)
#define FLASH_AREA_6_SIZE               (FLASH_S_DATA_PARTITION_SIZE)
/* Control Secure data image secondary slot */
#if (FLASH_AREA_6_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_6_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /*  (FLASH_AREA_6_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */
#endif /* OEMIROT_EXTERNAL_FLASH_ENABLE */
#else /* FLASH_AREA_6_ID */
#define FLASH_AREA_6_OFFSET             (0x0)
#define FLASH_AREA_6_SIZE               (0x0)
#endif /* FLASH_AREA_6_ID */

/* Non-Secure data image secondary slot */
#if defined(FLASH_AREA_7_ID)
#if defined(OEMIROT_EXTERNAL_FLASH_ENABLE)
#define FLASH_DEV_NAME_7                 SPI_FLASH_DEV_NAME
#define FLASH_DEVICE_ID_7               (SPI_FLASH_DEVICE_ID)
#define FLASH_AREA_7_OFFSET             (SPI_FLASH_BASE_ADDRESS + FLASH_AREA_2_SIZE + \
                                         FLASH_AREA_3_SIZE + FLASH_AREA_6_SIZE)
#define FLASH_AREA_7_SIZE               (FLASH_NS_DATA_PARTITION_SIZE)
#else /* OEMIROT_EXTERNAL_FLASH_ENABLE */
#define FLASH_DEV_NAME_7                 FLASH_DEV_NAME
#define FLASH_DEVICE_ID_7               (FLASH_AREAS_DEVICE_ID)
#define FLASH_AREA_7_OFFSET             (FLASH_AREA_BEGIN_OFFSET + FLASH_AREA_4_SIZE + \
                                         FLASH_AREA_0_SIZE + FLASH_AREA_1_SIZE + \
                                         FLASH_AREA_5_SIZE + FLASH_AREA_2_SIZE + \
                                         FLASH_AREA_3_SIZE + FLASH_AREA_6_SIZE)
#define FLASH_AREA_7_SIZE               (FLASH_NS_DATA_PARTITION_SIZE)
/* Control Non-Secure data image secondary slot */
#if (FLASH_AREA_7_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_7_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /*  (FLASH_AREA_7_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */
#endif /* OEMIROT_EXTERNAL_FLASH_ENABLE */
#else /* FLASH_AREA_7_ID */
#define FLASH_AREA_7_OFFSET             (0x0)
#define FLASH_AREA_7_SIZE               (0x0)
#endif /* FLASH_AREA_7_ID */

/* flash areas end offset */
#define FLASH_AREA_END_OFFSET           (FLASH_AREA_BEGIN_OFFSET + FLASH_AREA_4_SIZE + \
                                         FLASH_AREA_0_SIZE + FLASH_AREA_1_SIZE + \
                                         FLASH_AREA_5_SIZE + FLASH_AREA_2_SIZE + \
                                         FLASH_AREA_3_SIZE + FLASH_AREA_6_SIZE + \
                                         FLASH_AREA_7_SIZE)
/* Control flash area end */
#if (FLASH_AREA_END_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_END_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /*  (FLASH_AREA_END_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */

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
#define MCUBOOT_MAX_IMG_SECTORS           ((FLASH_MAX_PARTITION_SIZE) / \
                                           FLASH_AREA_IMAGE_SECTOR_SIZE)

#define SECURE_IMAGE_OFFSET             (0x0)
#define SECURE_IMAGE_MAX_SIZE           FLASH_S_PARTITION_SIZE

#define NON_SECURE_IMAGE_OFFSET         (SECURE_IMAGE_OFFSET + SECURE_IMAGE_MAX_SIZE)
#define NON_SECURE_IMAGE_MAX_SIZE       FLASH_NS_PARTITION_SIZE

/* BL2 NV Counters definitions  */
#define BL2_NV_COUNTERS_AREA_ADDR        FLASH_BL2_NVCNT_AREA_OFFSET
#define BL2_NV_COUNTERS_AREA_SIZE        FLASH_BL2_NVCNT_AREA_SIZE

/* control configuration */
#if defined(MCUBOOT_PRIMARY_ONLY) && !defined(MCUBOOT_OVERWRITE_ONLY)
#error "Config not supported: When MCUBOOT_PRIMARY_ONLY is enabled, MCUBOOT_OVERWRITE_ONLY is required."
#endif /* defined(MCUBOOT_PRIMARY_ONLY) */

#if defined(MCUBOOT_PRIMARY_ONLY) && defined(OEMIROT_EXTERNAL_FLASH_ENABLE)
#error "External flash not supported: When MCUBOOT_PRIMARY_ONLY is enabled."
#endif /* defined(MCUBOOT_PRIMARY_ONLY) && defined(OEMIROT_EXTERNAL_FLASH_ENABLE) */

#if (FLASH_S_PARTITION_SIZE == 0)
#error "Config not supported: Full NonSecure Appli."
#endif /* FLASH_S_PARTITION_SIZE */

#endif /* __FLASH_LAYOUT_H__ */
