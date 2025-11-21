/**
  ******************************************************************************
  * @file    ukapp_config_sections.h
  * @author  MCD Application Team
  * @brief   Sample code
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* //////////////////////////////////////////////////////////////////////// */
/*                                  HEADER                                  */
/* //////////////////////////////////////////////////////////////////////// */
/*__________________________________________________________________________*/
#include <stdint.h>
/* Verify that this include file has not been already processed.              */
#if !defined(UKAPP_CONFIG_SECTIONS_H)

#define UKAPP_CONFIG_SECTIONS_H_INSIDE

/* This file is only for IAR use */

#ifdef __IARCC__

/* We define some barrier magic values */
#define LDukUKAPP_BLOCK_CODE_BARRIER_MAGIC32 0xA55A5AA5
#define LDukUKAPP_BLOCK_DATA_BARRIER_MAGIC32 0x96696996
#define LDukADDRESS_IGNORE 0xFFFFFFFE

/* We define some sections and set some of them with readonly values (magic values)*/
#pragma section=".ukapp_descriptor"
#pragma section=".ukapp_code_exec_head"
const volatile __ro_placement uint32_t magic_head[2]  __attribute__((used, section(".ukapp_code_exec_head")))= {0xEEEEEEEE, 0xEEEEEEEE};

#pragma section=".ukapp_code_exec_uklib"

#pragma section=".ukapp_code_exec_lib"
#pragma section=".ukapp_code_exec_arm_specific"
#pragma section=".ukapp_code_exec_tail"
const volatile __ro_placement uint32_t magic_tail[2]  __attribute__((used, section(".ukapp_code_exec_tail")))= {LDukUKAPP_BLOCK_CODE_BARRIER_MAGIC32, LDukUKAPP_BLOCK_CODE_BARRIER_MAGIC32};

#pragma section=".ukapp_code_const_head"
const volatile __ro_placement uint32_t const_head[2]  __attribute__((used, section(".ukapp_code_const_head")))= {LDukUKAPP_BLOCK_CODE_BARRIER_MAGIC32, LDukUKAPP_BLOCK_CODE_BARRIER_MAGIC32};
#pragma section=".ukapp_code_const"
const volatile __ro_placement uint32_t code_const[2]  __attribute__((used, section(".ukapp_code_const")))= {LDukUKAPP_BLOCK_CODE_BARRIER_MAGIC32, LDukUKAPP_BLOCK_CODE_BARRIER_MAGIC32};
#pragma section=".ukapp_code_const_tail"
const volatile __ro_placement uint32_t const_tail[2]  __attribute__((used, section(".ukapp_code_const_tail")))= {LDukUKAPP_BLOCK_CODE_BARRIER_MAGIC32, LDukUKAPP_BLOCK_CODE_BARRIER_MAGIC32};

#pragma section=".ukapp_data_volatile_var_init_head"
const volatile __ro_placement uint32_t var_head[2]  __attribute__((used, section(".ukapp_data_volatile_var_init_head")))= {LDukUKAPP_BLOCK_DATA_BARRIER_MAGIC32, LDukUKAPP_BLOCK_DATA_BARRIER_MAGIC32};
#pragma section=".ukapp_data_volatile_var_init_uklib"
#pragma section=".ukapp_data_volatile_var_init_lib"
#pragma section=".ukapp_data_volatile_var_init_tail"
const volatile __ro_placement uint32_t var_tail[2]  __attribute__((used, section(".ukapp_data_volatile_var_init_tail")))= {LDukUKAPP_BLOCK_DATA_BARRIER_MAGIC32, LDukUKAPP_BLOCK_DATA_BARRIER_MAGIC32};

#pragma section=".ukapp_data_volatile_var_null_head"
__root uint32_t null_head[2] __attribute__((used, section(".ukapp_data_volatile_var_null_head")));
#pragma section=".ukapp_data_volatile_var_null"
#pragma section=".ukapp_data_volatile_var_null_tail"
__root uint32_t null_tail[2] __attribute__((used, section(".ukapp_data_volatile_var_null_tail")));

/* These values are updated automatically by SMAK provisioning script */
/* If you want change these values, update the configuration ../../../../ROT_Provisioning/SMAK/Config/SMAK_Config_General.xml with TrustedPackageCreator (tab H5-OBkey) */
/* and run ../../../../ROT_Provisioning/SMAK/update_appli_setup.bat for automatically update the necessary files */
#define UKAPP_MEMORY_REGION_CODE_ORIGIN                 0xC06E020
#define UKAPP_MEMORY_REGION_CODE_ORIGIN_END             0xC01F820

/* If you want to change these values, please read the ukapp.icf file first */
#define UKAPP_MEMORY_REGION_DATA_VOLATILE_ORIGIN        0x30063000
#define UKAPP_MEMORY_REGION_DATA_VOLATILE_ORIGIN_END    0x30069000

/* If you want to change these values, please read the ukapp.icf file first */
#define LDukUKAPP_MEMORY_MAPPING_HW_MCU_DEVICE_FILE_VERSION  0x02010000
#define LDukUKAPP_BLOCK_DATA_VOLATILE_HEAP_SIZE_MIN          1024 
#define LDukUKAPP_BLOCK_DATA_VOLATILE_STACK_SIZE_RESERVED    8192

#endif



/* ######################################################################## */
/*                                  FOOTER                                  */
/* ######################################################################## */
#undef UKAPP_CONFIG_SECTIONS_H_INSIDE
#endif /* !defined(UKAPP_CONFIG_SECTIONS_H_) */
/** @} */
/* ------------------------------End-Of-File------------------------------- */

