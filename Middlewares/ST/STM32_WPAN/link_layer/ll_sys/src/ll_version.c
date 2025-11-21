/**
  ******************************************************************************
  * @file    ll_version.c
  * @author  MCD Application Team
  * @brief   Link Layer version interface
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

/* Includes ------------------------------------------------------------------*/
/* Integer types */
#include <stdint.h>

/* Own header file  */
#include "ll_version.h"

/* Temporary header file for version tracking */
#include "ll_tmp_version.h"

/* Private defines -----------------------------------------------------------*/
/**
 * @brief Magic keyword to identify the system version when debugging
 */
 #define LL_SYS_MAGIC_KEYWORD  0xDEADBEEF
 
/* Private macros ------------------------------------------------------------*/
/* Macro to set a specific field value */  
#define LL_SYS_SET_FIELD_VALUE(value, mask, pos) \
  (((value) << (pos)) & (mask))

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief Link Layer system version structure definition
  */
typedef struct ll_sys_version 
{
  uint32_t magicKeyWord; /* Magic key word to identify the system version */
  uint32_t version; /* System version - i.e.: short hash of latest commit */
}ll_sys_version_t;

/* Private variables ---------------------------------------------------------*/
/**
 * @brief Link Layer brief version definition
 */
const uint8_t ll_sys_brief_version = LL_SYS_SET_FIELD_VALUE(LL_SYS_BRIEF_VERSION_MAJOR,
                                                            LL_SYS_BRIEF_VERSION_MAJOR_MASK,
                                                            LL_SYS_BRIEF_VERSION_MAJOR_POS) |
                                     LL_SYS_SET_FIELD_VALUE(LL_SYS_BRIEF_VERSION_MINOR,
                                                            LL_SYS_BRIEF_VERSION_MINOR_MASK,
                                                            LL_SYS_BRIEF_VERSION_MINOR_POS) |
                                     LL_SYS_SET_FIELD_VALUE(LL_SYS_BRIEF_VERSION_PATCH,
                                                            LL_SYS_BRIEF_VERSION_PATCH_MASK,
                                                            LL_SYS_BRIEF_VERSION_PATCH_POS);

/**
 * @brief Link Layer system version structure definition
 */
const ll_sys_version_t ll_sys_system_version =
{
  .magicKeyWord = LL_SYS_MAGIC_KEYWORD,
  .version = LL_SYS_SYSTEM_VERSION
};

/**
 * @brief Link Layer source version structure definition
 */
const ll_sys_version_t ll_sys_source_version =
{
  .magicKeyWord = LL_SYS_MAGIC_KEYWORD,
  .version = LL_SYS_SOURCE_VERSION
};

/* Functions Definition ------------------------------------------------------*/
uint8_t ll_sys_get_brief_fw_version(void)
{
  return ll_sys_brief_version;
}

uint32_t ll_sys_get_system_fw_version(void)
{
 return ll_sys_system_version.version;
}

uint32_t ll_sys_get_source_fw_version(void)
{
  return ll_sys_source_version.version;
}