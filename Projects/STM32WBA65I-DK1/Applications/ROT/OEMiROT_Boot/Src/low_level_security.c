/**
  ******************************************************************************
  * @file    low_level_security.c
  * @author  MCD Application Team
  * @brief   security protection implementation for secure boot
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
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "boot_hal_cfg.h"
#include "boot_hal_flowcontrol.h"
#include "mpu_armv8m_drv.h"
#include "region_defs.h"
#include "mcuboot_config/mcuboot_config.h"
#include "low_level_security.h"
#ifdef OEMIROT_DEV_MODE
#define BOOT_LOG_LEVEL BOOT_LOG_LEVEL_INFO
#else
#define BOOT_LOG_LEVEL BOOT_LOG_LEVEL_OFF
#endif /* OEMIROT_DEV_MODE  */
#include "bootutil/bootutil_log.h"
#if (OEMIROT_TAMPER_ENABLE != NO_TAMPER)
#include "low_level_rng.h"
#endif /*  (OEMIROT_TAMPER_ENABLE != NO_TAMPER)*/
#include "target_cfg.h"
#include "bootutil_priv.h"

/** @defgroup OEMIROT_SECURITY_Private_Defines  Private Defines
  * @{
  */
/* DUAL BANK page size */
#define PAGE_SIZE FLASH_AREA_IMAGE_SECTOR_SIZE

#if defined(STM32WBA65xx)
#define BANK_NUMBER 2
#elif defined(STM32WBA52xx) || defined(STM32WBA55xx)
#define BANK_NUMBER 1
#else
#error "Unknown target."
#endif
#define PAGE_MAX_NUMBER_IN_BANK 127


/* OEMiROT_Boot Vector Address  */
#define OEMIROT_BOOT_VTOR_ADDR ((uint32_t)(BL2_CODE_START))


const struct mpu_armv8m_region_cfg_t region_cfg_init_s[] = {
           /* background region is disabled, secure execution on unsecure flash is not possible*/
           /* but non secure execution on unsecure flash is possible , non secure mpu is used to protect execution */
           /* since SAU is enabled later to gain access to non secure flash */
           /* Forbid execution outside of flash write protected area  */
           /* descriptor 0 is set execute readonly before jumping in Secure application */
           {
               0,
               FLASH_BASE_S + S_IMAGE_PRIMARY_PARTITION_OFFSET,
               FLASH_BASE_S + S_IMAGE_PRIMARY_PARTITION_OFFSET + FLASH_S_PARTITION_SIZE - 1 - (~MPU_RLAR_LIMIT_Msk +1),
               MPU_ARMV8M_MAIR_ATTR_DATANOCACHE_IDX,
               MPU_ARMV8M_XN_EXEC_NEVER,
               MPU_ARMV8M_AP_RW_PRIV_ONLY,
               MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
               FLOW_STEP_MPU_S_I_EN_R0,
               FLOW_CTRL_MPU_S_I_EN_R0,
               FLOW_STEP_MPU_S_I_CH_R0,
               FLOW_CTRL_MPU_S_I_CH_R0,
#endif /* FLOW_CONTROL */
           },
           /* region 0: RW/PRIV_ONLY/EXEC_OK for OEMiROT Boot */
           {
               1,
               FLASH_BASE_S + FLASH_AREA_BL2_OFFSET,
               FLASH_BASE_S + FLASH_AREA_BL2_NOHDP_OFFSET + FLASH_AREA_BL2_NOHDP_SIZE - 1,
               MPU_ARMV8M_MAIR_ATTR_CODE_IDX,
               MPU_ARMV8M_XN_EXEC_OK,
               MPU_ARMV8M_AP_RO_PRIV_ONLY,
               MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
               FLOW_STEP_MPU_S_I_EN_R1,
               FLOW_CTRL_MPU_S_I_EN_R1,
               FLOW_STEP_MPU_S_I_CH_R1,
               FLOW_CTRL_MPU_S_I_CH_R1,
#endif /* FLOW_CONTROL */
           },
           /* Region 2: RW/PRIV_ONLY/EXEC_NEVER NS Flash BL2 NVCNT SCRATCH PERSO DATA */
           {
               2,
               FLASH_BASE_NS,
               FLASH_BASE_S + FLASH_AREA_BL2_OFFSET - 1,
               MPU_ARMV8M_MAIR_ATTR_DATANOCACHE_IDX,
               MPU_ARMV8M_XN_EXEC_NEVER,
               MPU_ARMV8M_AP_RW_PRIV_ONLY,
               MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
               FLOW_STEP_MPU_S_I_EN_R2,
               FLOW_CTRL_MPU_S_I_EN_R2,
               FLOW_STEP_MPU_S_I_CH_R2,
               FLOW_CTRL_MPU_S_I_CH_R2,
#endif /* FLOW_CONTROL */
           },
           /* Region 2: RW/PRIV_ONLY/EXEC_NEVER Flash for NV Counter SST ITS */
           {
               3,
               FLASH_BASE_S + FLASH_AREA_BL2_NOHDP_OFFSET + FLASH_AREA_BL2_NOHDP_SIZE,
               FLASH_BASE_S + FLASH_AREAS_TFM_SIZE + FLASH_AREA_0_OFFSET - 1,
               MPU_ARMV8M_MAIR_ATTR_DATANOCACHE_IDX,
               MPU_ARMV8M_XN_EXEC_NEVER,
               MPU_ARMV8M_AP_RW_PRIV_ONLY,
               MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
               FLOW_STEP_MPU_S_I_EN_R3,
               FLOW_CTRL_MPU_S_I_EN_R3,
               FLOW_STEP_MPU_S_I_CH_R3,
               FLOW_CTRL_MPU_S_I_CH_R3,
#endif /* FLOW_CONTROL */
           },
           /* Region 3: RW/PRIV_ONLY/EXEC_NEVER Forbid execution on full SRAM2 area */
           {
               4,
               BL2_SRAM_AREA_BASE,
               BL2_SRAM_AREA_END,
               MPU_ARMV8M_MAIR_ATTR_DATA_IDX,
               MPU_ARMV8M_XN_EXEC_NEVER,
               MPU_ARMV8M_AP_RW_PRIV_ONLY,
               MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
               FLOW_STEP_MPU_S_I_EN_R4,
               FLOW_CTRL_MPU_S_I_EN_R4,
               FLOW_STEP_MPU_S_I_CH_R4,
               FLOW_CTRL_MPU_S_I_CH_R4,
#endif /* FLOW_CONTROL */
           },
           /* Region 4: RW/PRIV_ONLY/EXEC_NEVER forbid secure peripheral execution */
           {
               5,
               PERIPH_BASE_NS,
               PERIPH_BASE_S + 0xFFFFFFF,
               MPU_ARMV8M_MAIR_ATTR_DEVICE_IDX,
               MPU_ARMV8M_XN_EXEC_NEVER,
               MPU_ARMV8M_AP_RW_PRIV_ONLY,
               MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
               FLOW_STEP_MPU_S_I_EN_R5,
               FLOW_CTRL_MPU_S_I_EN_R5,
               FLOW_STEP_MPU_S_I_CH_R5,
               FLOW_CTRL_MPU_S_I_CH_R5,
#endif /* FLOW_CONTROL */
           },
           /* Region 6: RW/PRIV_ONLY/EXEC_OK create a partition to be able to validate s appli */
           {
               6,
               FLASH_BASE_S + S_IMAGE_PRIMARY_PARTITION_OFFSET + FLASH_S_PARTITION_SIZE - (~MPU_RLAR_LIMIT_Msk +1),
               FLASH_BASE_S + S_IMAGE_PRIMARY_PARTITION_OFFSET + FLASH_S_PARTITION_SIZE - 1,
               MPU_ARMV8M_MAIR_ATTR_DATANOCACHE_IDX,
               MPU_ARMV8M_XN_EXEC_NEVER,
               MPU_ARMV8M_AP_RW_PRIV_ONLY,
               MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
               FLOW_STEP_MPU_S_I_EN_R6,
               FLOW_CTRL_MPU_S_I_EN_R6,
               FLOW_STEP_MPU_S_I_CH_R6,
               FLOW_CTRL_MPU_S_I_CH_R6,
#endif /* FLOW_CONTROL */
           }
};

const struct mpu_armv8m_region_cfg_t region_cfg_init_ns[] = {
           /* forbid execution on non secure FLASH /RAM in case of jump in non secure */
           /* Non Secure MPU  background all access in priviligied */
           /* reduced execution to all flash during control */
           {
               0,
               FLASH_BASE_NS + S_IMAGE_PRIMARY_PARTITION_OFFSET,
               FLASH_BASE_NS + FLASH_TOTAL_SIZE - 1,
               MPU_ARMV8M_MAIR_ATTR_DATANOCACHE_IDX,
               MPU_ARMV8M_XN_EXEC_NEVER,
               MPU_ARMV8M_AP_RW_PRIV_ONLY,
               MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
               FLOW_STEP_MPU_NS_I_EN_R0,
               FLOW_CTRL_MPU_NS_I_EN_R0,
               FLOW_STEP_MPU_NS_I_CH_R0,
               FLOW_CTRL_MPU_NS_I_CH_R0,
#endif /* FLOW_CONTROL */
           },
           /* Forbid execution on full SRAM area */
           {
               1,
               SRAM1_BASE_NS,
               SRAM2_BASE_NS + SRAM2_SIZE - 1,
               MPU_ARMV8M_MAIR_ATTR_DATA_IDX,
               MPU_ARMV8M_XN_EXEC_NEVER,
               MPU_ARMV8M_AP_RW_PRIV_ONLY,
               MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
               FLOW_STEP_MPU_NS_I_EN_R1,
               FLOW_CTRL_MPU_NS_I_EN_R1,
               FLOW_STEP_MPU_NS_I_CH_R1,
               FLOW_CTRL_MPU_NS_I_CH_R1,
#endif /* FLOW_CONTROL */
           },
           /* forbid secure peripheral execution */
           {
               2,
               PERIPH_BASE_NS,
               PERIPH_BASE_NS + 0xFFFFFFF,
               MPU_ARMV8M_MAIR_ATTR_DEVICE_IDX,
               MPU_ARMV8M_XN_EXEC_NEVER,
               MPU_ARMV8M_AP_RW_PRIV_ONLY,
               MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
               FLOW_STEP_MPU_NS_I_EN_R2,
               FLOW_CTRL_MPU_NS_I_EN_R2,
               FLOW_STEP_MPU_NS_I_CH_R2,
               FLOW_CTRL_MPU_NS_I_CH_R2,
#endif /* FLOW_CONTROL */
           }
};

#if defined(__ICCARM__)
#pragma location=".BL2_NoHdp_Data"
#endif
const struct mpu_armv8m_region_cfg_t region_cfg_appli_s[] __attribute__((section(".BL2_NoHdp_Data"))) = {
           /* region 0 is now enable for execution */
           {
               0,
               FLASH_BASE_S + S_IMAGE_PRIMARY_PARTITION_OFFSET,
               FLASH_BASE_S + S_IMAGE_PRIMARY_PARTITION_OFFSET + FLASH_S_PARTITION_SIZE - 1 - (~MPU_RLAR_LIMIT_Msk +1),
               MPU_ARMV8M_MAIR_ATTR_DATA_IDX,
               MPU_ARMV8M_XN_EXEC_OK,
               MPU_ARMV8M_AP_RO_PRIV_ONLY,
               MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
               FLOW_STEP_MPU_S_A_EN_R0,
               FLOW_CTRL_MPU_S_A_EN_R0,
               FLOW_STEP_MPU_S_A_CH_R0,
               FLOW_CTRL_MPU_S_A_CH_R0,
#endif /* FLOW_CONTROL */
           }
};

#if defined(__ICCARM__)
#pragma location=".BL2_NoHdp_Data"
#endif
const struct mpu_armv8m_region_cfg_t region_cfg_appli_ns[] __attribute__((section(".BL2_NoHdp_Data"))) = {
           /* region 0 is now enable for execution */
           {
               0,
               FLASH_BASE_NS + S_IMAGE_PRIMARY_PARTITION_OFFSET,
               FLASH_BASE_NS + S_IMAGE_PRIMARY_PARTITION_OFFSET + FLASH_PARTITION_SIZE - (~MPU_RLAR_LIMIT_Msk +1) - 1,
               MPU_ARMV8M_MAIR_ATTR_DATA_IDX,
               MPU_ARMV8M_XN_EXEC_OK,
               MPU_ARMV8M_AP_RO_PRIV_ONLY,
               MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
               FLOW_STEP_MPU_NS_A_EN_R0,
               FLOW_CTRL_MPU_NS_A_EN_R0,
               FLOW_STEP_MPU_NS_A_CH_R0,
               FLOW_CTRL_MPU_NS_A_CH_R0,
#endif /* FLOW_CONTROL */
           },
           {
               5,
               FLASH_BASE_NS + S_IMAGE_PRIMARY_PARTITION_OFFSET + FLASH_PARTITION_SIZE - (~MPU_RLAR_LIMIT_Msk +1),
               FLASH_BASE_NS + FLASH_TOTAL_SIZE - 1,
               MPU_ARMV8M_MAIR_ATTR_DATA_IDX,
               MPU_ARMV8M_XN_EXEC_NEVER,
               MPU_ARMV8M_AP_RW_PRIV_ONLY,
               MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
               FLOW_STEP_MPU_NS_A_EN_R5,
               FLOW_CTRL_MPU_NS_A_EN_R5,
               FLOW_STEP_MPU_NS_A_CH_R5,
               FLOW_CTRL_MPU_NS_A_CH_R5,
#endif  /* FLOW_CONTROL */
           }
};

const struct sau_cfg_t sau_init_cfg[] = {
    /* Allow non secure access to Flash non Secure peripheral for regression */
    /* Secure HAL flash driver uses the non secure flash address to perform access on non secure flash area */
    {
        1,
        (uint32_t)FLASH_NS,
        ((uint32_t)FLASH_NS + 0xffff),
        OEMIROT_FALSE,
#ifdef FLOW_CONTROL
        FLOW_STEP_SAU_I_EN_R1,
        FLOW_CTRL_SAU_I_EN_R1,
        FLOW_STEP_SAU_I_CH_R1,
        FLOW_CTRL_SAU_I_CH_R1,
#endif /* FLOW_CONTROL */
    },
    /* Allow non secure Flash base access for Area 1/2/3-5/6/7 */
    {
        2,
        ((uint32_t)FLASH_BASE_NS + NS_IMAGE_PRIMARY_PARTITION_OFFSET),
        ((uint32_t)FLASH_BASE_NS + FLASH_AREA_END_OFFSET - 1),
        OEMIROT_FALSE,
#ifdef FLOW_CONTROL
        FLOW_STEP_SAU_I_EN_R2,
        FLOW_CTRL_SAU_I_EN_R2,
        FLOW_STEP_SAU_I_CH_R2,
        FLOW_CTRL_SAU_I_CH_R2,
#endif /* FLOW_CONTROL */
    }
};

/**
  * @}
  */
/* Private function prototypes -----------------------------------------------*/
/** @defgroup OEMIROT_SECURITY_Private_Functions  Private Functions
  * @{
  */
#ifdef OEMIROT_OB_RDP_LEVEL_VALUE
static void rdp_level(uint32_t rdplevel, uint32_t current_rdplevel);
#endif /* OEMIROT_OB_RDP_LEVEL_VALUE */
static void gtzc_init_cfg(void);
static void sau_and_idau_cfg(void);
static void mpu_init_cfg(void);
static void mpu_appli_cfg(void);
static void flash_priv_cfg(void);
static void enable_hdp_protection(void);
static void active_tamper(void);
/**
  * @}
  */

/** @defgroup OEMIROT_SECURITY_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief  Apply the runtime security  protections to
  *
  * @param  None
  * @note   By default, the best security protections are applied
  * @retval None
  */
void LL_SECU_ApplyRunTimeProtections(void)
{
  /* Unsecure bottom of SRAM1 for error_handler */
  gtzc_init_cfg();

  /* Set MPU to forbidd execution outside of not muteable code  */
  /* Initialize not secure MPU to forbidd execution on Flash /SRAM */
  mpu_init_cfg();

  /* Enable SAU to gain access to flash area non secure for write/read */
  sau_and_idau_cfg();
  /* With OEMIROT_DEV_MODE , active tamper calls Error_Handler */
  /* Error_Handler requires sau_and_idau_cfg */
  active_tamper();

  /* Configure Flash Privilege access */
  flash_priv_cfg();
}

/* Place code in a specific section */
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".BL2_NoHdp_Code"
#else
__attribute__((section(".BL2_NoHdp_Code")))
#endif /* __ICCARM__ */

/**
  * @brief  Update the runtime security protections for application start
  *
  * @param  None
  * @retval None
  */
void LL_SECU_UpdateRunTimeProtections(void)
{
  /* Enable HDP protection to hide sensible boot material */
  enable_hdp_protection();

  /* Set MPU to enable execution of Secure and Non Secure active slots */
  mpu_appli_cfg();
}

/* Stop placing data in specified section */
#if defined(__ICCARM__)
#pragma default_function_attributes =
#endif /* __ICCARM__ */

/**
  * @brief  Check if the Static security  protections to
  *         all the Sections in Flash:  WRP, SECURE FLASH, SECURE USER FLASH.
  *         those protections not impacted by a Reset. They are set using the Option Bytes
  *         When the device is locked (RDP Level2), these protections cannot be changed anymore
  * @param  None
  * @note   By default, the best security protections are applied to the different
  *         flash sections in order to maximize the security level for the specific MCU.
  * @retval None
  */
void LL_SECU_CheckStaticProtections(void)
{
  static FLASH_OBProgramInitTypeDef flash_option_bytes_bank1 = {0};
#if defined(STM32WBA65xx)
static FLASH_OBProgramInitTypeDef flash_option_bytes_bank2 = {0};
#endif
#ifdef OEMIROT_NSBOOT_CHECK_ENABLE
  static FLASH_OBProgramInitTypeDef flash_option_bytes_nsboot0 = {0};
  static FLASH_OBProgramInitTypeDef flash_option_bytes_nsboot1 = {0};
#endif /* OEMIROT_NSBOOT_CHECK_ENABLE */
#if !defined(OEMIROT_FIRST_BOOT_STAGE)
  uint32_t start;
  uint32_t end;
#endif /* OEMIROT_FIRST_BOOT_STAGE */

#ifdef OEMIROT_NSBOOT_CHECK_ENABLE
  /* Get NSBOOTADD0 and NSBOOTADD1 value */
  flash_option_bytes_nsboot0.BootAddrConfig = OB_BOOTADDR_NS0;
  HAL_FLASHEx_OBGetConfig(&flash_option_bytes_nsboot0);
  flash_option_bytes_nsboot1.BootAddrConfig = OB_BOOTADDR_NS1;
  HAL_FLASHEx_OBGetConfig(&flash_option_bytes_nsboot1);
#endif /* OEMIROT_NSBOOT_CHECK_ENABLE */

  /* Get bank1 areaA OB  */
  flash_option_bytes_bank1.WRPArea = OB_WRPAREA_BANK1_AREAA;
  flash_option_bytes_bank1.WMSecConfig = OB_WMSEC_AREA1;
  flash_option_bytes_bank1.BootAddrConfig = OB_BOOTADDR_SEC0;
  HAL_FLASHEx_OBGetConfig(&flash_option_bytes_bank1);
#if defined(STM32WBA65xx)
  /* Get bank2 areaB OB  */
  flash_option_bytes_bank2.WRPArea = OB_WRPAREA_BANK2_AREAA;
  flash_option_bytes_bank2.WMSecConfig = OB_WMSEC_AREA2;
  HAL_FLASHEx_OBGetConfig(&flash_option_bytes_bank2);
#endif


  /* Check TZEN = 1 , we are in secure */
  if ((flash_option_bytes_bank1.USERConfig & FLASH_OPTR_TZEN) != FLASH_OPTR_TZEN)
  {
    BOOT_LOG_ERR("Unexpected value for TZEN");
    Error_Handler();
  }

#if defined(STM32WBA65xx)
  /* Check if dual bank is set */
  if ((flash_option_bytes_bank1.USERConfig & FLASH_OPTR_DUAL_BANK_Msk) != FLASH_OPTR_DUAL_BANK)
  {
    BOOT_LOG_ERR("Unexpected value for dual bank configuration");
    Error_Handler();
  }

  /* Check if swap bank is reset */
  if ((flash_option_bytes_bank1.USERConfig & FLASH_OPTR_SWAP_BANK) != 0)
  {
    BOOT_LOG_ERR("Unexpected value for swap bank configuration");
    Error_Handler();
  }
#endif

#if !defined (OEMUROT_ENABLE)
  /* Check secure boot address */
  if (flash_option_bytes_bank1.BootAddr != BL2_BOOT_VTOR_ADDR)
  {
    BOOT_LOG_INF("BootAddr 0x%x", flash_option_bytes_bank1.BootAddr);
    BOOT_LOG_ERR("Unexpected value for SEC BOOT Address");
    Error_Handler();
  }
#endif /* OEMUROT_ENABLE */

#ifdef OEMIROT_NSBOOT_CHECK_ENABLE
  /* Check non-secure boot addresses */
  if ((flash_option_bytes_nsboot0.BootAddr != BL2_BOOT_VTOR_ADDR)
      || (flash_option_bytes_nsboot1.BootAddr != BL2_BOOT_VTOR_ADDR))
  {
    BOOT_LOG_ERR("Unexpected value for NS BOOT Address");
    Error_Handler();
  }
#endif /* OEMIROT_NSBOOT_CHECK_ENABLE */

#if !defined(OEMIROT_FIRST_BOOT_STAGE)
  /* Check bank1 secure flash protection */
  start = 0;
  end = (S_IMAGE_PRIMARY_PARTITION_OFFSET  + FLASH_S_PARTITION_SIZE - 1) / PAGE_SIZE;
  if (end > PAGE_MAX_NUMBER_IN_BANK)
  {
    end = PAGE_MAX_NUMBER_IN_BANK;
  }

  if ((start != flash_option_bytes_bank1.WMSecStartPage)
      || (end != flash_option_bytes_bank1.WMSecEndPage))
  {
    BOOT_LOG_INF("BANK 1 secure flash [%d, %d] : OB [%d, %d]",
                 start, end, flash_option_bytes_bank1.WMSecStartPage, flash_option_bytes_bank1.WMSecEndPage);

    BOOT_LOG_ERR("Unexpected value for secure flash protection");
    Error_Handler();
  }

#if defined(STM32WBA65xx)
  /* Check bank2 secure flash protection */
  start = 0;
  end = (S_IMAGE_PRIMARY_PARTITION_OFFSET  + FLASH_S_PARTITION_SIZE - 1) / PAGE_SIZE;
  if (end > PAGE_MAX_NUMBER_IN_BANK)
  {
    end = end - (PAGE_MAX_NUMBER_IN_BANK + 1);
    if ((start != flash_option_bytes_bank2.WMSecStartPage)
        || (end != flash_option_bytes_bank2.WMSecEndPage))
    {
    BOOT_LOG_INF("BANK 2 secure flash [%d, %d] : OB [%d, %d]", start, end, flash_option_bytes_bank2.WMSecStartPage,
                   flash_option_bytes_bank2.WMSecEndPage);
    }
  }
  /* the bank 2 must be fully unsecure */
  else if (flash_option_bytes_bank2.WMSecEndPage >= flash_option_bytes_bank2.WMSecStartPage)
  {
    BOOT_LOG_INF("BANK 2 secure flash [%d, %d] : OB [%d, %d]", PAGE_MAX_NUMBER_IN_BANK, 0, flash_option_bytes_bank2.WMSecStartPage,
                 flash_option_bytes_bank2.WMSecEndPage);
  }
#endif /* STM32WBA65xx */
#endif /* OEMIROT_FIRST_BOOT_STAGE */

#ifdef  OEMIROT_WRP_PROTECT_ENABLE
  /* Check flash write protection */
#if defined (OEMUROT_ENABLE)
  start = (0x4000) / PAGE_SIZE;
  end = (0x17FFF) / PAGE_SIZE;
#else
  start = FLASH_AREA_PERSO_OFFSET / PAGE_SIZE;
  end = (FLASH_AREA_PERSO_OFFSET + FLASH_AREA_PERSO_SIZE + FLASH_AREA_BL2_SIZE +
         FLASH_AREA_BL2_NOHDP_SIZE - 1) / PAGE_SIZE;
#endif /* OEMUROT_ENABLE */

  if ((flash_option_bytes_bank1.WRPStartOffset > flash_option_bytes_bank1.WRPEndOffset)
      || (start != flash_option_bytes_bank1.WRPStartOffset)
      || (end != flash_option_bytes_bank1.WRPEndOffset))
  {
    BOOT_LOG_INF("BANK 1 flash write protection [%d, %d] : OB [%d, %d]", start, end,
                 flash_option_bytes_bank1.WRPStartOffset,
                 flash_option_bytes_bank1.WRPEndOffset);
#ifdef OEMIROT_DEV_MODE
    BOOT_LOG_ERR("Unexpected value for write protection ");
    Error_Handler();
#else
    Error_Handler();
#endif /* OEMIROT_DEV_MODE */
  }
#if  defined(OEMIROT_WRP_LOCK_ENABLE)
  /* Check WRP lock protection */
  if (flash_option_bytes_bank1.WRPLock != ENABLE)
  {
    BOOT_LOG_INF("BANK 1 WRP Lock 0x%x", flash_option_bytes_bank1.WRPLock);
    BOOT_LOG_ERR("Unexpected value for WRP Lock");
    Error_Handler();
  }
#endif /* OEMIROT_WRP_LOCK_ENABLE */
#endif /* OEMIROT_WRP_PROTECT_ENABLE */

#ifdef  OEMIROT_HDP_PROTECT_ENABLE
  /* Check secure user flash protection (HDP) */
  start = 0;
  end = (FLASH_BL2_HDP_END) / PAGE_SIZE;
  if (
    (flash_option_bytes_bank1.WMSecStartPage > flash_option_bytes_bank1.WMHDPEndPage)
    || (start < flash_option_bytes_bank1.WMSecStartPage)
    || (end > flash_option_bytes_bank1.WMHDPEndPage)
    || (flash_option_bytes_bank1.WMSecConfig & OB_WMSEC_HDP_AREA_DISABLE))
  {
    BOOT_LOG_INF("BANK 1 secure user flash [%d, %d] : OB [%d, %d]",
                 start,
                 end,
                 flash_option_bytes_bank1.WMSecStartPage,
                 flash_option_bytes_bank1.WMHDPEndPage);
#ifdef OEMIROT_DEV_MODE
    BOOT_LOG_ERR("Unexpected value for secure user flash protection");
    Error_Handler();
#else
    Error_Handler();
#endif  /*  OEMIROT_DEV_MODE */
  }
#else /* OEMIROT_HDP_PROTECT_ENABLE */
  flash_option_bytes_bank1.WMSecConfig &= ~(OB_WMSEC_HDP_AREA_CONFIG | OB_WMSEC_HDP_AREA_ENABLE);
  flash_option_bytes_bank1.WMSecConfig |= OB_WMSEC_HDP_AREA_DISABLE;
#endif /* OEMIROT_HDP_PROTECT_ENABLE */

#ifdef OEMIROT_SECURE_USER_SRAM2_ERASE_AT_RESET
  /* Check SRAM2 ERASE on reset */
  if ((flash_option_bytes_bank1.USERConfig & FLASH_OPTR_SRAM2_RST) != 0)
  {
    BOOT_LOG_ERR("Unexpected value for SRAM2 ERASE at Reset");
    Error_Handler();
  }
#endif /*OEMIROT_SECURE_USER_SRAM2_ERASE_AT_RESET */

#ifdef OEMIROT_OB_BOOT_LOCK
  /* Check Boot lock protection */
  if (flash_option_bytes_bank1.BootLock != OEMIROT_OB_BOOT_LOCK)
  {
    BOOT_LOG_INF("BootLock 0x%x", flash_option_bytes_bank1.BootLock);
    BOOT_LOG_ERR("Unexpected value for SEC BOOT LOCK");
    Error_Handler();
  }
#endif /* OEMIROT_OB_BOOT_LOCK */

#ifdef OEMIROT_OB_RDP_LEVEL_VALUE
  /* Check RDL level : boot if current RDP level is greater or equal to selected RDP level */
  switch (OEMIROT_OB_RDP_LEVEL_VALUE)
  {
    case OB_RDP_LEVEL_2:
      if (flash_option_bytes_bank1.RDPLevel != OB_RDP_LEVEL_2)
      {
        rdp_level(OEMIROT_OB_RDP_LEVEL_VALUE, flash_option_bytes_bank1.RDPLevel);
        Error_Handler_rdp();
      }
      break;

    case OB_RDP_LEVEL_1:
      if ((flash_option_bytes_bank1.RDPLevel != OB_RDP_LEVEL_2) &&
          (flash_option_bytes_bank1.RDPLevel != OB_RDP_LEVEL_1))
      {
        rdp_level(OEMIROT_OB_RDP_LEVEL_VALUE, flash_option_bytes_bank1.RDPLevel);
        Error_Handler_rdp();
      }
      break;

    case OB_RDP_LEVEL_0_5:
      if ((flash_option_bytes_bank1.RDPLevel != OB_RDP_LEVEL_2) &&
          (flash_option_bytes_bank1.RDPLevel != OB_RDP_LEVEL_1) &&
          (flash_option_bytes_bank1.RDPLevel != OB_RDP_LEVEL_0_5))
      {
        rdp_level(OEMIROT_OB_RDP_LEVEL_VALUE, flash_option_bytes_bank1.RDPLevel);
        Error_Handler_rdp();
      }
      break;

    case OB_RDP_LEVEL_0:
      break;

    default:
      Error_Handler();
      break;
  }
#endif /* OEMIROT_OB_RDP_LEVEL_VALUE */
}

#ifdef OEMIROT_OB_RDP_LEVEL_VALUE
static void rdp_level(uint32_t rdplevel, uint32_t current_rdplevel)
{
  BOOT_LOG_INF("RDPLevel 0x%x (0x%x)", current_rdplevel, rdplevel);
  BOOT_LOG_ERR("Unexpected value for RDP level");
}
#endif /* OEMIROT_OB_RDP_LEVEL_VALUE */
/**
  * @brief  Memory Config Init
  * @param  None
  * @retval None
  */
static void  gtzc_init_cfg(void)
{
   __HAL_RCC_GTZC1_CLK_ENABLE();
}

/**
  * @brief  Sau idau init
  * @param  None
  * @retval None
  */
static void sau_and_idau_cfg(void)
{
  uint32_t i;
  uint32_t rnr;
  uint32_t rbar;
  uint32_t rlar;
  uint32_t rnr_reg;
  uint32_t rbar_reg;
  uint32_t rlar_reg;
  uint32_t ctrl_reg;

  /* configuration stage */
  if (uFlowStage == FLOW_STAGE_CFG)
  {
    /* Disable SAU */
    SAU->CTRL &= ~(SAU_CTRL_ENABLE_Msk);

    for (i = 0; i < ARRAY_SIZE(sau_init_cfg); i++)
    {
      SAU->RNR = sau_init_cfg[i].RNR;
      SAU->RBAR = sau_init_cfg[i].RBAR & SAU_RBAR_BADDR_Msk;
      SAU->RLAR = (sau_init_cfg[i].RLAR & SAU_RLAR_LADDR_Msk) |
                  (sau_init_cfg[i].nsc ? SAU_RLAR_NSC_Msk : 0U) |
                  SAU_RLAR_ENABLE_Msk;

      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, sau_init_cfg[i].flow_step_enable,
                                           sau_init_cfg[i].flow_ctrl_enable);
    }

    /* Force memory writes before continuing */
    __DSB();
    /* Flush and refill pipeline with updated permissions */
    __ISB();
    /* Enable SAU */
    SAU->CTRL |=  (SAU_CTRL_ENABLE_Msk);

    /* Execution stopped if flow control failed */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_SAU_I_EN, FLOW_CTRL_SAU_I_EN);
  }
  /* verification stage */
  else
  {
    for (i = 0; i < ARRAY_SIZE(sau_init_cfg); i++)
    {
      SAU->RNR = sau_init_cfg[i].RNR;

      rnr = sau_init_cfg[i].RNR;
      rbar = sau_init_cfg[i].RBAR & SAU_RBAR_BADDR_Msk;
      rlar = (sau_init_cfg[i].RLAR & SAU_RLAR_LADDR_Msk) |
                  (sau_init_cfg[i].nsc ? SAU_RLAR_NSC_Msk : 0U) |
                  SAU_RLAR_ENABLE_Msk;

      rnr_reg = SAU->RNR;
      rbar_reg = SAU->RBAR;
      rlar_reg = SAU->RLAR;

      if ((rnr_reg != rnr) || (rbar_reg != rbar) || (rlar_reg != rlar))
      {
        Error_Handler();
      }

      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, sau_init_cfg[i].flow_step_check,
                                           sau_init_cfg[i].flow_ctrl_check);
    }

    ctrl_reg = SAU->CTRL;
    if ((ctrl_reg & SAU_CTRL_ENABLE_Msk) != SAU_CTRL_ENABLE_Msk)
    {
      Error_Handler();
    }
    else
    {
      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_SAU_I_CH, FLOW_CTRL_SAU_I_CH);
    }
  }
}

#if  defined(OEMUROT_ENABLE)
void LL_SECU_DisableCleanMpu(void)
{
  struct mpu_armv8m_dev_t dev_mpu_s = { MPU_BASE };
  struct mpu_armv8m_dev_t dev_mpu_ns = { MPU_BASE_NS };
  mpu_armv8m_disable(&dev_mpu_s);
  mpu_armv8m_clean(&dev_mpu_s);
  mpu_armv8m_disable(&dev_mpu_ns);
  mpu_armv8m_clean(&dev_mpu_ns);
}
#endif

/**
  * @brief  mpu init
  * @param  None
  * @retval None
  */
static void mpu_init_cfg(void)
{
#ifdef OEMIROT_BOOT_MPU_PROTECTION
  struct mpu_armv8m_dev_t dev_mpu_s = { MPU_BASE };
  struct mpu_armv8m_dev_t dev_mpu_ns = { MPU_BASE_NS };
  int32_t i;

  /* configuration stage */
  if (uFlowStage == FLOW_STAGE_CFG)
  {
    /* configure secure MPU regions */
    for (i = 0; i < ARRAY_SIZE(region_cfg_init_s); i++)
    {
      if (mpu_armv8m_region_enable(&dev_mpu_s,
        (struct mpu_armv8m_region_cfg_t *)&region_cfg_init_s[i]) != MPU_ARMV8M_OK)
      {
        Error_Handler();
      }
      else
      {
        /* Execution stopped if flow control failed */
        FLOW_CONTROL_STEP(uFlowProtectValue, region_cfg_init_s[i].flow_step_enable,
                                             region_cfg_init_s[i].flow_ctrl_enable);
      }
    }

    /* enable secure MPU */
    mpu_armv8m_enable(&dev_mpu_s, PRIVILEGED_DEFAULT_DISABLE, HARDFAULT_NMI_ENABLE);
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_MPU_S_I_EN, FLOW_CTRL_MPU_S_I_EN);

    /* configure non secure MPU regions */
    for (i = 0; i < ARRAY_SIZE(region_cfg_init_ns); i++)
    {
      if (mpu_armv8m_region_enable(&dev_mpu_ns,
        (struct mpu_armv8m_region_cfg_t *)&region_cfg_init_ns[i]) != MPU_ARMV8M_OK)
      {
        Error_Handler();
      }
      else
      {
        /* Execution stopped if flow control failed */
        FLOW_CONTROL_STEP(uFlowProtectValue, region_cfg_init_ns[i].flow_step_enable,
                                             region_cfg_init_ns[i].flow_ctrl_enable);
      }
    }

    /* enable non secure MPU */
    mpu_armv8m_enable(&dev_mpu_ns, PRIVILEGED_DEFAULT_DISABLE, HARDFAULT_NMI_ENABLE);
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_MPU_NS_I_EN, FLOW_CTRL_MPU_NS_I_EN);
  }
  /* verification stage */
  else
  {
    /* check secure MPU regions */
    for (i = 0; i < ARRAY_SIZE(region_cfg_init_s); i++)
    {
      if (mpu_armv8m_region_enable_check(&dev_mpu_s,
        (struct mpu_armv8m_region_cfg_t *)&region_cfg_init_s[i]) != MPU_ARMV8M_OK)
      {
        Error_Handler();
      }
      else
      {
        /* Execution stopped if flow control failed */
        FLOW_CONTROL_STEP(uFlowProtectValue, region_cfg_init_s[i].flow_step_check,
                                             region_cfg_init_s[i].flow_ctrl_check);
      }
    }

    /* check secure MPU */
    if (mpu_armv8m_check(&dev_mpu_s, PRIVILEGED_DEFAULT_DISABLE,
                      HARDFAULT_NMI_ENABLE) != MPU_ARMV8M_OK)
    {
      Error_Handler();
    }
    else
    {
      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_MPU_S_I_CH, FLOW_CTRL_MPU_S_I_CH);
    }

    /* check non secure MPU regions */
    for (i = 0; i < ARRAY_SIZE(region_cfg_init_ns); i++)
    {
      if (mpu_armv8m_region_enable_check(&dev_mpu_ns,
        (struct mpu_armv8m_region_cfg_t *)&region_cfg_init_ns[i]) != MPU_ARMV8M_OK)
      {
        Error_Handler();
      }
      else
      {
        /* Execution stopped if flow control failed */
        FLOW_CONTROL_STEP(uFlowProtectValue, region_cfg_init_ns[i].flow_step_check,
                                             region_cfg_init_ns[i].flow_ctrl_check);
      }
    }

    /* check non secure MPU */
    if (mpu_armv8m_check(&dev_mpu_ns, PRIVILEGED_DEFAULT_DISABLE,
                      HARDFAULT_NMI_ENABLE) != MPU_ARMV8M_OK)
    {
      Error_Handler();
    }
    else
    {
      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_MPU_NS_I_CH, FLOW_CTRL_MPU_NS_I_CH);
    }
  }
#endif /* OEMIROT_BOOT_MPU_PROTECTION */
}

/* Place code in a specific section */
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".BL2_NoHdp_Code"
#else
__attribute__((section(".BL2_NoHdp_Code")))
#endif /* __ICCARM__ */

static void mpu_appli_cfg(void)
{
#ifdef OEMIROT_BOOT_MPU_PROTECTION
  static struct mpu_armv8m_dev_t dev_mpu_s = { MPU_BASE };
  static struct mpu_armv8m_dev_t dev_mpu_ns = { MPU_BASE_NS };
  int32_t i;

  /* configuration stage */
  if (uFlowStage == FLOW_STAGE_CFG)
  {
    /* configure secure MPU regions */
    for (i = 0; i < ARRAY_SIZE(region_cfg_appli_s); i++)
    {
      if (mpu_armv8m_region_enable(&dev_mpu_s,
        (struct mpu_armv8m_region_cfg_t *)&region_cfg_appli_s[i]) != MPU_ARMV8M_OK)
      {
        Error_Handler();
      }
      else
      {
        /* Execution stopped if flow control failed */
        FLOW_CONTROL_STEP(uFlowProtectValue, region_cfg_appli_s[i].flow_step_enable,
                                             region_cfg_appli_s[i].flow_ctrl_enable);
      }
    }

    /* configure non secure MPU regions */
    for (i = 0; i < ARRAY_SIZE(region_cfg_appli_ns); i++)
    {
      if (mpu_armv8m_region_enable(&dev_mpu_ns,
        (struct mpu_armv8m_region_cfg_t *)&region_cfg_appli_ns[i]) != MPU_ARMV8M_OK)
      {
        Error_Handler();
      }
      else
      {
        /* Execution stopped if flow control failed */
        FLOW_CONTROL_STEP(uFlowProtectValue, region_cfg_appli_ns[i].flow_step_enable,
                                             region_cfg_appli_ns[i].flow_ctrl_enable);
      }
    }
  }
  else
  {
    /* check secure MPU regions */
    for (i = 0; i < ARRAY_SIZE(region_cfg_appli_s); i++)
    {
      if (mpu_armv8m_region_enable_check(&dev_mpu_s,
        (struct mpu_armv8m_region_cfg_t *)&region_cfg_appli_s[i]) != MPU_ARMV8M_OK)
      {
        Error_Handler();
      }
      else
      {
        /* Execution stopped if flow control failed */
        FLOW_CONTROL_STEP(uFlowProtectValue, region_cfg_appli_s[i].flow_step_check,
                                             region_cfg_appli_s[i].flow_ctrl_check);
      }
    }

    /* check non secure MPU regions */
    for (i = 0; i < ARRAY_SIZE(region_cfg_appli_ns); i++)
    {
      if (mpu_armv8m_region_enable_check(&dev_mpu_ns,
        (struct mpu_armv8m_region_cfg_t *)&region_cfg_appli_ns[i]) != MPU_ARMV8M_OK)
      {
        Error_Handler();
      }
      else
      {
        /* Execution stopped if flow control failed */
        FLOW_CONTROL_STEP(uFlowProtectValue, region_cfg_appli_ns[i].flow_step_check,
                                             region_cfg_appli_ns[i].flow_ctrl_check);
      }
    }
  }
#endif /* OEMIROT_BOOT_MPU_PROTECTION */
}
/* Stop placing data in specified section */
#if defined(__ICCARM__)
#pragma default_function_attributes =
#endif /* __ICCARM__ */

/**
  * @brief  configure flash privilege access
  * @param  None
  * @retval None
  */
static void flash_priv_cfg(void)
{
#ifdef OEMIROT_FLASH_PRIVONLY_ENABLE
  /* configuration stage */
  if (uFlowStage == FLOW_STAGE_CFG)
  {
    /* Configure Flash Privilege access */
    HAL_FLASHEx_ConfigPrivMode(FLASH_SPRIV_DENIED);
    /* Execution stopped if flow control failed */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_FLASH_P_EN, FLOW_CTRL_FLASH_P_EN);
  }
  /* verification stage */
  else
  {
    if (HAL_FLASHEx_GetPrivMode() != FLASH_SPRIV_DENIED)
    {
      Error_Handler();
    }
    else
    {
      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_FLASH_P_CH, FLOW_CTRL_FLASH_P_CH);
    }
  }
#endif /*  OEMIROT_FLASH_PRIVONLY_ENABLE */
}

/* Place code in a specific section */
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".BL2_NoHdp_Code"
#else
__attribute__((section(".BL2_NoHdp_Code")))
#endif /* __ICCARM__ */

/**
  * @brief  Enable HDP protection
  * @param  None
  * @retval None
  */
static void enable_hdp_protection(void)
{
#ifdef OEMIROT_HDP_PROTECT_ENABLE
  __IO uint32_t read_reg = (uint32_t) &FLASH->SECHDPCR;
#if defined(STM32WBA65xx)
  do
  {
    /* Activate HDP protection */
    SET_BIT(FLASH->SECHDPCR, FLASH_SECHDPCR_HDP1_ACCDIS);
  } while (((*(uint32_t *)read_reg) & FLASH_SECHDPCR_HDP1_ACCDIS_Msk) != FLASH_SECHDPCR_HDP1_ACCDIS);

  if (((*(uint32_t *)read_reg) & FLASH_SECHDPCR_HDP1_ACCDIS) != FLASH_SECHDPCR_HDP1_ACCDIS)
  {
    /* Security issue : execution stopped ! */
    Error_Handler();
  }

#elif defined(STM32WBA52xx) || defined(STM32WBA55xx)
  do
  {
    /* Activate HDP protection */
    SET_BIT(FLASH->SECHDPCR, FLASH_SECHDPCR_HDP_ACCDIS);
  } while (((*(uint32_t *)read_reg) & FLASH_SECHDPCR_HDP_ACCDIS) != FLASH_SECHDPCR_HDP_ACCDIS);

  if (((*(uint32_t *)read_reg) & FLASH_SECHDPCR_HDP_ACCDIS) != FLASH_SECHDPCR_HDP_ACCDIS)
  {
    /* Security issue : execution stopped ! */
    Error_Handler();
  }
#else
#error "Unknown target."
#endif
#endif /* OEMIROT_HDP_PROTECT_ENABLE */
}

/* Stop placing data in specified section */
#if defined(__ICCARM__)
#pragma default_function_attributes =
#endif /* __ICCARM__ */


#if (OEMIROT_TAMPER_ENABLE != NO_TAMPER)
const RTC_SecureStateTypeDef TamperSecureConf =
{
  .rtcSecureFull = RTC_SECURE_FULL_NO,
  .rtcNonSecureFeatures = RTC_NONSECURE_FEATURE_ALL,
  .tampSecureFull = TAMP_SECURE_FULL_YES,
  .MonotonicCounterSecure = TAMP_MONOTONIC_CNT_SECURE_NO,
  .backupRegisterStartZone2 = 0,
  .backupRegisterStartZone3 = 0
};
const RTC_PrivilegeStateTypeDef TamperPrivConf =
{
  .rtcPrivilegeFull = RTC_PRIVILEGE_FULL_NO,
  .rtcPrivilegeFeatures = RTC_PRIVILEGE_FEATURE_NONE,
  .tampPrivilegeFull = TAMP_PRIVILEGE_FULL_YES,
  .MonotonicCounterPrivilege = TAMP_MONOTONIC_CNT_PRIVILEGE_NO,
  .backupRegisterStartZone2 = 0,
  .backupRegisterStartZone3 = 0
};
const RTC_InternalTamperTypeDef InternalTamperConf =
{
  .IntTamper = RTC_INT_TAMPER_9,   /* Cryptographic peripherals fault */
  .TimeStampOnTamperDetection = RTC_TIMESTAMPONTAMPERDETECTION_DISABLE,
  .NoErase                    = RTC_TAMPER_ERASE_BACKUP_ENABLE
};
/* Place code in a specific section */
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".BL2_NoHdp_Code"
#else
__attribute__((section(".BL2_NoHdp_Code")))
#endif /* __ICCARM__ */
void TAMP_IRQHandler(void)
{
    NVIC_SystemReset();
}
/* Stop placing data in specified section */
#if defined(__ICCARM__)
#pragma default_function_attributes =
#endif /* __ICCARM__ */
#ifdef OEMIROT_DEV_MODE
extern volatile uint32_t TamperEventCleared;
#endif
#endif /* (OEMIROT_TAMPER_ENABLE != NO_TAMPER) */
RTC_HandleTypeDef RTCHandle;

static void active_tamper(void)
{
#if (OEMIROT_TAMPER_ENABLE == ALL_TAMPER)
  RTC_TamperTypeDef sTamper = {0};
#endif /* (OEMIROT_TAMPER_ENABLE == ALL_TAMPER) */
#if (OEMIROT_TAMPER_ENABLE != NO_TAMPER)
  RTC_SecureStateTypeDef TamperSecureConfGet;
  RTC_PrivilegeStateTypeDef TamperPrivConfGet;
  uint32_t DeviceSecretConf;
#endif /* OEMIROT_TAMPER_ENABLE != NO_TAMPER) */
  /* configuration stage */
  if (uFlowStage == FLOW_STAGE_CFG)
  {
#if defined(OEMIROT_DEV_MODE) && (OEMIROT_TAMPER_ENABLE != NO_TAMPER)
    if (TamperEventCleared)
    {
      BOOT_LOG_INF("Boot with TAMPER Event Active");
#if (OEMIROT_TAMPER_ENABLE == ALL_TAMPER)
      /* avoid several re-boot in DEV_MODE with Tamper active */
      BOOT_LOG_INF("Unplug the tamper cable, and reboot");
      BOOT_LOG_INF("Or");
#endif
      BOOT_LOG_INF("Build and Flash with flag #define OEMIROT_TAMPER_ENABLE NO_TAMPER\n");
      Error_Handler();
    }
#endif /*  OEMIROT_DEV_MODE && (OEMIROT_TAMPER_ENABLE != NO_TAMPER) */

    /* RTC Init */
    RTCHandle.Instance = RTC;
    RTCHandle.Init.HourFormat     = RTC_HOURFORMAT_12;
    RTCHandle.Init.AsynchPrediv   = RTC_ASYNCH_PREDIV;
    RTCHandle.Init.SynchPrediv    = RTC_SYNCH_PREDIV;
    RTCHandle.Init.OutPut         = RTC_OUTPUT_DISABLE;
    RTCHandle.Init.OutPutRemap    = RTC_OUTPUT_REMAP_NONE;
    RTCHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RTCHandle.Init.OutPutType     = RTC_OUTPUT_TYPE_PUSHPULL;
    RTCHandle.Init.OutPutPullUp   = RTC_OUTPUT_PULLUP_NONE;

    if (HAL_RTC_Init(&RTCHandle) != HAL_OK)
    {
      Error_Handler();
    }
#if (OEMIROT_TAMPER_ENABLE == ALL_TAMPER)
    /** Enable the RTC Tamper 4 */
    sTamper.Tamper = RTC_TAMPER_4;
    sTamper.Trigger = RTC_TAMPERTRIGGER_LOWLEVEL;
    sTamper.NoErase = RTC_TAMPER_ERASE_BACKUP_ENABLE;
    sTamper.MaskFlag = RTC_TAMPERMASK_FLAG_DISABLE;
    sTamper.Filter = RTC_TAMPERFILTER_DISABLE;
    sTamper.SamplingFrequency = RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV32768;
    sTamper.PrechargeDuration = RTC_TAMPERPRECHARGEDURATION_1RTCCLK;
    sTamper.TamperPullUp = RTC_TAMPER_PULLUP_ENABLE;

    if (HAL_RTCEx_SetTamper(&RTCHandle, &sTamper) != HAL_OK)
    {
      Error_Handler();
    }
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_ACT_EN, FLOW_CTRL_TAMP_ACT_EN);
#else
    HAL_RTCEx_DeactivateTamper(&RTCHandle, RTC_TAMPER_ALL);
#endif  /* (OEMIROT_TAMPER_ENABLE == ALL_TAMPER) */
#if (OEMIROT_TAMPER_ENABLE != NO_TAMPER)
    /*  Internal Tamper activation  */
    /*  Enable Cryptographic IPs fault (tamp_itamp9), Backup domain voltage threshold monitoring (tamp_itamp1)*/
    if (HAL_RTCEx_SetInternalTamper(&RTCHandle, (RTC_InternalTamperTypeDef *)&InternalTamperConf) != HAL_OK)
    {
      Error_Handler();
    }
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_INT_EN, FLOW_CTRL_TAMP_INT_EN);

    /*  Set tamper configuration secure only  */
    if (HAL_RTCEx_SecureModeSet(&RTCHandle, (RTC_SecureStateTypeDef *)&TamperSecureConf) != HAL_OK)
    {
      Error_Handler();
    }
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_SEC_EN, FLOW_CTRL_TAMP_SEC_EN);

    /*  Set tamper configuration privileged only   */
    if (HAL_RTCEx_PrivilegeModeSet(&RTCHandle, (RTC_PrivilegeStateTypeDef *)&TamperPrivConf) != HAL_OK)
    {
      Error_Handler();
    }
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_PRIV_EN, FLOW_CTRL_TAMP_PRIV_EN);

    /*  Activate Secret Erase */
    DeviceSecretConf = (uint32_t)(TAMP_DEVICESECRETS_ERASE_SRAM2 | TAMP_DEVICESECRETS_ERASE_ICACHE | TAMP_DEVICESECRETS_ERASE_PKA_SRAM);
    HAL_RTCEx_ConfigEraseDeviceSecrets(&RTCHandle, DeviceSecretConf);

    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_CFG_EN, FLOW_CTRL_TAMP_CFG_EN);
    BOOT_LOG_INF("TAMPER Activated");
#else
    HAL_RTCEx_DeactivateInternalTamper(&RTCHandle, RTC_INT_TAMPER_ALL);
#endif /* (OEMIROT_TAMPER_ENABLE != NO_TAMPER) */
  }
#if (OEMIROT_TAMPER_ENABLE != NO_TAMPER)
  /* verification stage */
  else
  {
    fih_int fih_rc = FIH_FAILURE;
#if (OEMIROT_TAMPER_ENABLE == ALL_TAMPER)
    /*  Check External Tamper activation */
    if ((READ_REG(TAMP->CR1) & TAMP_CR1_TAMP4E_Msk)   != TAMP_CR1_TAMP4E)
    {
      Error_Handler();
    }
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_ACT_CH, FLOW_CTRL_TAMP_ACT_CH);
#endif  /* (OEMIROT_TAMPER_ENABLE == ALL_TAMPER) */
    /*  Check Internal Tamper activation */
    if ((READ_BIT(RTC->CR, RTC_CR_TAMPTS) != InternalTamperConf.TimeStampOnTamperDetection) ||
#if (OEMIROT_TAMPER_ENABLE == ALL_TAMPER)
        (READ_REG(TAMP->CR1) != (TAMP_CR1_ITAMP9E | TAMP_CR1_TAMP4E)) ||
#else
        (READ_REG(TAMP->CR1) != (TAMP_CR1_ITAMP9E)) ||
#endif /* (OEMIROT_TAMPER_ENABLE == ALL_TAMPER) */
        (READ_REG(TAMP->CR3) != 0x00000000U))
    {
      Error_Handler();
    }
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_INT_CH, FLOW_CTRL_TAMP_INT_CH);

    /*  Check tamper configuration secure only  */
    if (HAL_RTCEx_SecureModeGet(&RTCHandle, (RTC_SecureStateTypeDef *)&TamperSecureConfGet) != HAL_OK)
    {
      Error_Handler();
    }
    FIH_CALL(boot_fih_memequal, fih_rc, (void *)&TamperSecureConf, (void *)&TamperSecureConfGet, sizeof(TamperSecureConf));
    if (fih_not_eq(fih_rc, FIH_SUCCESS))
    {
      Error_Handler();
    }
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_SEC_CH, FLOW_CTRL_TAMP_SEC_CH);

    /*  Check tamper configuration privileged only   */
    if (HAL_RTCEx_PrivilegeModeGet(&RTCHandle, (RTC_PrivilegeStateTypeDef *)&TamperPrivConfGet) != HAL_OK)
    {
      Error_Handler();
    }
    FIH_CALL(boot_fih_memequal, fih_rc, (void *)&TamperPrivConf, (void *)&TamperPrivConfGet, sizeof(TamperPrivConf));
    if (fih_not_eq(fih_rc, FIH_SUCCESS))
    {
      Error_Handler();
    }
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_PRIV_CH, FLOW_CTRL_TAMP_PRIV_CH);

    /*  Check Secret Erase */
    DeviceSecretConf = (uint32_t)TAMP_DEVICESECRETS_ERASE_SRAM2 | TAMP_DEVICESECRETS_ERASE_ICACHE | TAMP_DEVICESECRETS_ERASE_PKA_SRAM;

    if (READ_REG(TAMP->RPCFGR) != DeviceSecretConf)
    {
      Error_Handler();
    }
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_CFG_CH, FLOW_CTRL_TAMP_CFG_CH);
  }
#endif /*  OEMIROT_TAMPER_ENABLE != NO_TAMPER */
}
/**
  * @}
  */
