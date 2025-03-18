/**
  ******************************************************************************
  * @file    boot_hal_cfg.h
  * @author  MCD Application Team
  * @brief   File fixing configuration flag specific
  *
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


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BOOT_HAL_CFG_H
#define BOOT_HAL_CFG_H

/* Includes ------------------------------------------------------------------*/
#include "stm32_hal.h"
#include "flash_layout.h"

/* RTC clock */
#define  RTC_CLOCK_SOURCE_LSI
#ifdef RTC_CLOCK_SOURCE_LSI
#define RTC_ASYNCH_PREDIV  0x7F
#define RTC_SYNCH_PREDIV   0x00F9
#endif
#ifdef RTC_CLOCK_SOURCE_LSE
#define RTC_ASYNCH_PREDIV  0x7F
#define RTC_SYNCH_PREDIV   0x00FF
#endif

/* ICache */
#define OEMIROT_ICACHE_ENABLE /*!< Instruction cache enable */

/* Static protections */
#if !defined(OEMIROT_FIRST_BOOT_STAGE)
#define OEMIROT_WRP_PROTECT_ENABLE  /*!< Write Protection  */
#define OEMIROT_HDP_PROTECT_ENABLE /*!< HDP protection   */
#endif /* OEMIROT_FIRST_BOOT_STAGE */
#define OEMIROT_SECURE_USER_SRAM2_ERASE_AT_RESET /*!< SRAM2 clear at Reset  */

#ifdef OEMIROT_DEV_MODE
#define OEMIROT_OB_RDP_LEVEL_VALUE OB_RDP_LEVEL_0 /*!< RDP level */
#else
#define OEMIROT_OB_RDP_LEVEL_VALUE OB_RDP_LEVEL_2 /*!< RDP level */
#endif /* OEMIROT_DEV_MODE */

#define NO_TAMPER            (0)                /*!< No tamper activated */
#define INTERNAL_TAMPER_ONLY (1)                /*!< Only Internal tamper activated */
#define ALL_TAMPER           (2)                /*!< Internal and External tamper activated */
#if defined(OEMIROT_FIRST_BOOT_STAGE)
#define OEMIROT_TAMPER_ENABLE NO_TAMPER            /*!< TAMPER configuration flag  */
#else
#define OEMIROT_TAMPER_ENABLE INTERNAL_TAMPER_ONLY            /*!< TAMPER configuration flag  */
#endif /* OEMIROT_FIRST_BOOT_STAGE */

#ifdef OEMIROT_DEV_MODE
#define OEMIROT_OB_BOOT_LOCK 1 /*!< BOOT Lock expected value  */
#else
#define OEMIROT_WRP_LOCK_ENABLE /*!< Write Protection Lock */
#define OEMIROT_OB_BOOT_LOCK 1 /*!< BOOT Lock expected value  */
#define OEMIROT_NSBOOT_CHECK_ENABLE  /*!<  NSBOOTADD0 and NSBOOTADD1 must be set to OEMiROT_Boot Vector  */
#if  (OEMIROT_OB_RDP_LEVEL_VALUE == OB_RDP_LEVEL_0_5)
#error "RDP 0.5 is not allowed in production mode (OEMIROT_DEV_MODE undefined)"
#endif /* (OEMIROT_OB_RDP_LEVEL_VALUE == OB_RDP_LEVEL_0_5) */
#endif /* OEMIROT_DEV_MODE */

/* Run time protections */
#define OEMIROT_FLASH_PRIVONLY_ENABLE  /*!< Flash Command in Privileged only  */
#define OEMIROT_BOOT_MPU_PROTECTION    /*!< OEMiROT_Boot uses MPU to prevent execution outside of OEMiROT_Boot code  */

/* Fast wake-up from low power */
/* #define OEMIROT_FAST_WAKE_UP */          /*!< Enable fast wake-up from low power: bypass images control */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  OEMIROT_SUCCESS = 0U,
  OEMIROT_FAILED
} OEMIROT_ErrorStatus;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Error_Handler(void);
void Error_Handler_rdp(void);
#endif /* GENERATOR_RDP_PASSWORD_AVAILABLE */
