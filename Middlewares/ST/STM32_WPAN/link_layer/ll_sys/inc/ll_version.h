/******************************************************************************
 * @file    ll_version.h
 * @author  MCD Application Team
 * @brief   Header for Link Layer versioning
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#ifndef LL_VERSION_H
#define LL_VERSION_H

/* Exported constants --------------------------------------------------------*/
/**
 * @brief Brief version of the current Link Layer.
 */
#define LL_SYS_BRIEF_VERSION_MAJOR 1
#define LL_SYS_BRIEF_VERSION_MINOR 0
#define LL_SYS_BRIEF_VERSION_PATCH 0

/* LL System version major mask */
#define LL_SYS_BRIEF_VERSION_MAJOR_MASK   (0xC0U)
/* LL System version major pos */
#define LL_SYS_BRIEF_VERSION_MAJOR_POS    (0x06U)
/* LL System version minor mask */
#define LL_SYS_BRIEF_VERSION_MINOR_MASK   (0x3CU)
/* LL System version minor pos */
#define LL_SYS_BRIEF_VERSION_MINOR_POS    (0x02U)
/* LL System version patch mask */
#define LL_SYS_BRIEF_VERSION_PATCH_MASK   (0x03U)
/* LL System version patch pos */
#define LL_SYS_BRIEF_VERSION_PATCH_POS    (0x00U)

/* Exported macros -----------------------------------------------------------*/
/* Macro to get a specific field value */
#define LL_SYS_GET_FIELD_VALUE(value, mask, pos) \
  (((value) & (mask)) >> (pos))

/* Macro to get the system version major value */
#define LL_SYS_GET_SYSTEM_VERSION_MAJOR(value) \
  LL_SYS_GET_FIELD_VALUE((value), LL_SYS_BRIEF_VERSION_MAJOR_MASK, LL_SYS_BRIEF_VERSION_MAJOR_POS)
/* Macro to get the system version minor value */
#define LL_SYS_GET_SYSTEM_VERSION_MINOR(value) \
  LL_SYS_GET_FIELD_VALUE((value), LL_SYS_BRIEF_VERSION_MINOR_MASK, LL_SYS_BRIEF_VERSION_MINOR_POS)
/* Macro to get the system version patch value */
#define LL_SYS_GET_SYSTEM_VERSION_PATCH(value) \
  LL_SYS_GET_FIELD_VALUE((value), LL_SYS_BRIEF_VERSION_PATCH_MASK, LL_SYS_BRIEF_VERSION_PATCH_POS)

/* Exported functions ------------------------------------------------------- */
/**
 * @brief Get the brief Link Layer firmware version
 * 
 * @details This function retunrs the brief firmware version of the Link Layer.
 *          The version is represented as a single byte and follow this organization:
 *          - Bits 7-6: Major version
 *          - Bits 5-2: Minor version
 *          - Bits 1-0: Patch version 
 */
uint8_t ll_sys_get_brief_fw_version(void);

/**
 * @brief Get the system firmware version
 * 
 * @return Short hash of the system firmware commit.
 */
uint32_t ll_sys_get_system_fw_version(void);

/**
 * @brief Get the source firmware version
 * 
 * @return Short hash of the source firmware commit. 
 */
uint32_t ll_sys_get_source_fw_version(void);

#endif /* LL_VERSION_H */