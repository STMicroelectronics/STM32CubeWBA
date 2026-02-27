/**
  ******************************************************************************
  * @file    boot_hal.c
  * @author  MCD Application Team
  * @brief   This file contains  mcuboot hardware specific implementation
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
#include "boot_hal.h"
#include "boot_hal_hash_ref.h"
#include "boot_hal_imagevalid.h"
#include "boot_hal_flowcontrol.h"
#include "mcuboot_config/mcuboot_config.h"
#include "uart_stdout.h"
#include "low_level_security.h"
#include "bootutil/boot_record.h"
#include "target_cfg.h"
#include "cmsis.h"
#include "Driver_Flash.h"
#include "region_defs.h"
#include "low_level_rng.h"
#include "stm32_board.h"
#if defined(OEMIROT_EXTERNAL_FLASH_ENABLE)
#include "low_level_ext_flash.h"
#endif /* OEMIROT_EXTERNAL_FLASH_ENABLE */
#if   defined(MCUBOOT_USE_HASH_REF)
#include "bootutil_priv.h"
#endif


#if defined(OEMIROT_EXTERNAL_FLASH_ENABLE)
extern ARM_DRIVER_FLASH EXT_FLASH_DEV_NAME;
#endif /* OEMIROT_EXTERNAL_FLASH_ENABLE */
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;


#if defined(MCUBOOT_DOUBLE_SIGN_VERIF)
/* Global variables to memorize images validation status */
#if (MCUBOOT_IMAGE_NUMBER == 1)
uint8_t ImageValidStatus[MCUBOOT_IMAGE_NUMBER] = {IMAGE_INVALID};
#elif (MCUBOOT_IMAGE_NUMBER == 2)
uint8_t ImageValidStatus[MCUBOOT_IMAGE_NUMBER] = {IMAGE_INVALID, IMAGE_INVALID};
#elif (MCUBOOT_IMAGE_NUMBER == 3)
uint8_t ImageValidStatus[MCUBOOT_IMAGE_NUMBER] = {IMAGE_INVALID, IMAGE_INVALID, IMAGE_INVALID};
#elif (MCUBOOT_IMAGE_NUMBER == 4)
uint8_t ImageValidStatus[MCUBOOT_IMAGE_NUMBER] = {IMAGE_INVALID, IMAGE_INVALID, IMAGE_INVALID, IMAGE_INVALID};
#else
#error "MCUBOOT_IMAGE_NUMBER not supported"
#endif /* (MCUBOOT_IMAGE_NUMBER == 1)  */
uint8_t ImageValidIndex = 0;
#endif /* MCUBOOT_DOUBLE_SIGN_VERIF */
#if defined(MCUBOOT_DOUBLE_SIGN_VERIF) || defined(MCUBOOT_USE_HASH_REF)
uint8_t ImageValidEnable = 0;
#endif /* MCUBOOT_DOUBLE_SIGN_VERIF || MCUBOOT_USE_HASH_REF */

#if defined(MCUBOOT_USE_HASH_REF)
#define BL2_HASH_REF_ADDR     (FLASH_HASH_REF_AREA_OFFSET)
uint8_t ImageValidHashUpdate = 0;
uint8_t ImageValidHashRef[MCUBOOT_IMAGE_NUMBER * SHA256_LEN] = {0};
#endif /* MCUBOOT_USE_HASH_REF */

#if defined(FLOW_CONTROL)
/* Global variable for Flow Control state */
volatile uint32_t uFlowProtectValue = FLOW_CTRL_INIT_VALUE;
#endif /* FLOW_CONTROL */
volatile uint32_t uFlowStage = FLOW_STAGE_CFG;

/*
#define ICACHE_MONITOR
*/
#if defined(ICACHE_MONITOR)
#define ICACHE_MONITOR_PRINT() printf("icache monitor - Hit: %x, Miss: %x\r\n", \
                                      HAL_ICACHE_Monitor_GetHitValue(), HAL_ICACHE_Monitor_GetMissValue());
#else
#define ICACHE_MONITOR_PRINT()
#endif /* ICACHE_MONITOR */

/* Private function prototypes -----------------------------------------------*/
/** @defgroup BOOT_HAL_Private_Functions  Private Functions
  * @{
  */

#if defined (OEMUROT_ENABLE)
static uint32_t GetPage(uint32_t Addr);

__attribute__((naked)) void boot_jump_to_RSSExt(uint32_t boot_jump_addr1, uint32_t boot_jump_addr2,
                                                uint32_t HdpArea, uint32_t VectorTableAddr);
#else  /* OEMUROT_ENABLE */
__attribute__((naked)) void boot_jump_to_RSS(uint32_t boot_jump_addr1, uint32_t boot_jump_addr2,
                                             uint32_t HdpArea, uint32_t VectorTableAddr);
#endif  /* OEMUROT_ENABLE */

void icache_init(void);
/**
  * @}
  */

#if defined(OEMIROT_FAST_WAKE_UP)
fih_int boot_platform_wakeup(void);
#endif /* OEMIROT_FAST_WAKE_UP */

#if defined (OEMUROT_ENABLE)
/**
  * @brief  Gets the page of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The page of a given address
  */
static uint32_t GetPage(uint32_t Addr)
{
  return ((Addr - FLASH_BASE) / FLASH_PAGE_SIZE);
}

/**
  * @brief  Jumping to next image using RSS service, after erasing SRAM twice
  * @param  boot_jump_addr1 boot_jump_to_RSS address in order to execute twice SRAM erasing
  * @param  boot_jump_addr2 RSS service address (Close and exit HDP Function)
  * @param  HdpArea notifies which hdp area to close, can be a combination of
  *         hdpa area 1 and hdp area 2
  * @param  VectorTableAddr pointer on the vector table containing the reset handler the function
  *         jumps to.
  */
__attribute__((naked)) void boot_jump_to_RSSExt(uint32_t boot_jump_addr1, uint32_t boot_jump_addr2,
                                                uint32_t HdpArea, uint32_t VectorTableAddr)
{
  __ASM volatile(
#if !defined(__ICCARM__)
    ".syntax unified                 \n"
#endif
    "mov     r7, r0                  \n" /* r7 = boot_jump_addr1*/
    "mov     r8, r1                  \n" /* r8 = boot_jump_addr2 (rsslib_sec_Close_Exit_HDP) */
    "mov     r9, r2                  \n" /* r9 = HdpArea (RSSLIB_HDP_AREA1_Msk) */
    "mov     r10, r3                 \n" /* r10 = VectorTableAddr */
    /*"mov     r11, r4                 \n"*/ /* r11 = CloseBound */
    "bl      boot_clear_bl2_ram_area \n" /* Clear RAM before jump */
    "movs    r0, #0                  \n" /* Clear registers: R0-R12, */
    "mov     r1, r0                  \n" /* except R7/R8/R9/R10 */
    "mov     r2, r0                  \n"
    "mov     r3, r0                  \n"
    "mov     r4, r0                  \n"
    "mov     r5, r0                  \n"
    "mov     r6, r0                  \n"
    "mov     r12, r0                 \n"
    "mov     lr,  r0                 \n"
    "mov     r0, r8                  \n" /* r0 = boot_jump_addr2 (rsslib_sec_Close_Exit_HDP) */
    "mov     r1, r9                  \n" /* r1 = HdpArea (RSSLIB_HDP_AREA1_Msk) */
    "mov     r2, r10                 \n" /* r2 = VectorTableAddr */
    "ldr     r3, =0xD6D6D6D6         \n" /* r3 = CloseBound */
    "mov     r8, r4                  \n" /* Clear registers: R8 */
    "mov     r9, r4                  \n" /* Clear registers: R9 */
    "mov     r10, r4                 \n" /* Clear registers: R10 */
    "mov     r11, r4                 \n" /* Clear registers: R11 */
    "bx      r7                      \n" /* Jump to RSS service : typedef uint32_t ( *RSSLIB_S_CloseExitHDPExt_TypeDef)( uint32_t HdpExtArea, uint32_t VectorTableAddr, uint32_t CloseBound ) */
  );
}
#else  /* OEMUROT_ENABLE */
/**
  * @brief  Jumping to next image using RSS service, after erasing SRAM twice
  * @param  boot_jump_addr1 boot_jump_to_RSS address in order to execute twice SRAM erasing
  * @param  boot_jump_addr2 RSS service address (Close and exit HDP Function)
  * @param  HdpArea notifies which hdp area to close, can be a combination of
  *         hdpa area 1 and hdp area 2
  * @param  VectorTableAddr pointer on the vector table containing the reset handler the function
  *         jumps to.
  */
__attribute__((naked)) void boot_jump_to_RSS(uint32_t boot_jump_addr1, uint32_t boot_jump_addr2,
                                             uint32_t HdpArea, uint32_t VectorTableAddr)
{
  __ASM volatile(
#if !defined(__ICCARM__)
    ".syntax unified                 \n"
#endif
    "mov     r7, r0                  \n" /* r7 = boot_jump_addr1*/
    "mov     r8, r1                  \n" /* r8 = boot_jump_addr2 (rsslib_sec_Close_Exit_HDP) */
    "mov     r9, r2                  \n" /* r9 = HdpArea (RSSLIB_HDP_AREA1_Msk) */
    "mov     r10, r3                 \n" /* r10 = VectorTableAddr */
    //"bl      boot_clear_bl2_ram_area \n" /* Clear RAM before jump */
    "movs    r0, #0                  \n" /* Clear registers: R0-R12, */
    "mov     r1, r0                  \n" /* except R7/R8/R9/R10 */
    "mov     r2, r0                  \n"
    "mov     r3, r0                  \n"
    "mov     r4, r0                  \n"
    "mov     r5, r0                  \n"
    "mov     r6, r0                  \n"
    "mov     r11, r0                 \n"
    "mov     r12, r0                 \n"
    "mov     lr,  r0                 \n"
    "mov     r0, r8                  \n" /* r0 = boot_jump_addr2 (rsslib_sec_Close_Exit_HDP) */
    "mov     r1, r9                  \n" /* r1 = HdpArea (RSSLIB_HDP_AREA1_Msk) */
    "mov     r2, r10                 \n" /* r2 = VectorTableAddr */
    "mov     r8, r3                  \n" /* Clear registers: R8 */
    "mov     r9, r3                  \n" /* Clear registers: R9 */
    "mov     r10, r3                 \n" /* Clear registers: R10 */
    "bx      r7                      \n" /* Jump to RSS service : typedef uint32_t ( *RSSLIB_S_CloseExitHDP_TypeDef)( uint32_t HdpArea, uint32_t VectorTableAddr ) */
  );
}
#endif  /* OEMUROT_ENABLE */

#if defined(OEMIROT_FAST_WAKE_UP)
/**
  * @brief This function checks if system wakes-up from low-power mode:
  *        Stand-by mode and shut down mode (depending on HW capability)
  * @note
  * @retval FIH_SUCCESS in case of wake-up
  */
fih_int boot_platform_wakeup(void)
{
  fih_int fih_rc = FIH_FAILURE;

  /* Check Stand-By Flag */
  if (__HAL_PWR_GET_FLAG(PWR_FLAG_SBF))
  {
    fih_rc = FIH_SUCCESS;
  }

  FIH_RET(fih_rc);
}
#endif /* OEMIROT_FAST_WAKE_UP */

/**
  * @brief This function manage the jump to secure application.
  * @note
  * @retval void
  */
void boot_platform_quit(struct boot_arm_vector_table *vector)
{
  static struct boot_arm_vector_table *vt;
#if defined (OEMUROT_ENABLE)
  static FLASH_HDPExtensionTypeDef flash_hdpext;
  uint32_t rsslib_sec_Close_Exit_HDPExt;
#else
  uint32_t rsslib_sec_Close_Exit_HDP;
#endif  /* OEMUROT_ENABLE */
#if defined(MCUBOOT_DOUBLE_SIGN_VERIF)
  uint32_t image_index;

  (void)fih_delay();

#if defined(OEMIROT_FAST_WAKE_UP)
  fih_int fih_rc = FIH_FAILURE;

  /* Check if wake-up from low-power to bypass images revalidation */
  FIH_CALL(boot_platform_wakeup, fih_rc);
  if (fih_not_eq(fih_rc, FIH_SUCCESS))
  {
#else
  {
#endif /* OEMIROT_FAST_WAKE_UP */
    /* Check again if images have been validated, to resist to basic hw attacks */
    for (image_index = 0; image_index < MCUBOOT_IMAGE_NUMBER; image_index++)
    {
      if (ImageValidStatus[image_index] != IMAGE_VALID)
      {
        BOOT_LOG_ERR("Error while double controlling images validation");
        Error_Handler();
      }
    }
  }
#endif /* MCUBOOT_DOUBLE_SIGN_VERIF */

#if defined(MCUBOOT_USE_HASH_REF)
  /* Store new hash references in flash for next boot */
  if (ImageValidHashUpdate)
  {
    if (boot_hash_ref_store())
    {
      BOOT_LOG_ERR("Error while storing hash references");
      Error_Handler();
    }
  }
#endif /* MCUBOOT_USE_HASH_REF */

  /* Check Flow control state */
  FLOW_CONTROL_CHECK(uFlowProtectValue, FLOW_CTRL_STAGE_2);
  uFlowStage = FLOW_STAGE_CFG;

  RNG_DeInit();

  ICACHE_MONITOR_PRINT()

#ifdef OEMIROT_ICACHE_ENABLE
  /* Invalidate ICache before jumping to application */
  if (HAL_ICACHE_Invalidate() != HAL_OK)
  {
      Error_Handler();
  }
#endif /* ICACHE_ENABLED */
  /* Update run time protections for application execution */
  LL_SECU_UpdateRunTimeProtections();

  /* set the secure vector */
  SCB->VTOR = (uint32_t)vector;

  vt = (struct boot_arm_vector_table *)vector;
  /* Check Flow control state */
  FLOW_CONTROL_CHECK(uFlowProtectValue, FLOW_CTRL_STAGE_3_A);
  uFlowStage = FLOW_STAGE_CHK;

  /* Double the update of run time protections, to resist to basic hardware attacks */
  LL_SECU_UpdateRunTimeProtections();

  /* Check Flow control for dynamic protections */
  FLOW_CONTROL_CHECK(uFlowProtectValue, FLOW_CTRL_STAGE_4_A);
  /*  change stack limit  */
  __set_MSPLIM(0);
  /* Restore the Main Stack Pointer Limit register's reset value
   * before passing execution to runtime firmware to make the
   * bootloader transparent to it.
   */
  __set_MSP(vt->msp);
  __DSB();
  __ISB();

#if defined (OEMUROT_ENABLE)

  /* Init RSS jump function descriptor */
  rsslib_sec_Close_Exit_HDPExt = (uint32_t)(RSSLIB_PFUNC->S.CloseExitHDPExt);

  if (rsslib_sec_Close_Exit_HDPExt == 0)
  {
    BOOT_LOG_ERR("Error reading RSS in System Flash!");
    while (1);
  }

  flash_hdpext.Bank    = FLASH_BANK_1;
  flash_hdpext.NbPages = GetPage(FLASH_BASE + FLASH_AREA_BL2_HDP_END_OFFSET) - GetPage(FLASH_BASE + FLASH_HASH_REF_AREA_OFFSET);
  HAL_FLASHEx_ConfigHDPExtension(&flash_hdpext);

  boot_jump_to_RSSExt((uint32_t)&boot_jump_to_RSSExt, rsslib_sec_Close_Exit_HDPExt, RSSLIB_HDP_AREA_Msk, (uint32_t) vt);
#else  /* OEMUROT_ENABLE */

  /* Init RSS jump function descriptor */
  rsslib_sec_Close_Exit_HDP = (uint32_t)(RSSLIB_PFUNC->S.CloseExitHDP);

  if (rsslib_sec_Close_Exit_HDP == 0)
  {
    BOOT_LOG_ERR("Error reading RSS in System Flash!");
    while (1);
  }

  boot_jump_to_RSS((uint32_t)&boot_jump_to_RSS, rsslib_sec_Close_Exit_HDP, RSSLIB_HDP_AREA_Msk, (uint32_t) vt);
#endif  /* OEMUROT_ENABLE */

  /* Avoid compiler to pop registers after having changed MSP */
#if !defined(__ICCARM__)
  __builtin_unreachable();
#else
  while(1);
#endif /* defined(__ICCARM__) */
}

/****************************************************************************************/
/**
  * @brief This function is called to clear all RAM area before jumping in
  * in Secure application .
  * @note
  * @retval void
  */
void boot_clear_bl2_ram_area(void)
{
  __IO uint32_t *pt = (uint32_t *)BL2_DATA_START;
  uint32_t index;

  for (index = 0; index < (BL2_DATA_SIZE / 4); index++)
  {
      pt[index] = 0;
  }
}
/****************************************************************************************/
#if defined(MCUBOOT_USE_HASH_REF)
/**
  * @brief This function store all hash references in flash
  * @return 0 on success; nonzero on failure.
  */
int boot_hash_ref_store(void)
{
  /* Erase hash references flash sector */
  if (FLASH_DEV_NAME.EraseSector(BL2_HASH_REF_ADDR) != ARM_DRIVER_OK)
  {
    return BOOT_EFLASH;
  }

  /* Store new hash references in flash */
  if (FLASH_DEV_NAME.ProgramData(BL2_HASH_REF_ADDR, ImageValidHashRef,
      (SHA256_LEN * MCUBOOT_IMAGE_NUMBER)) != ARM_DRIVER_OK)
  {
    return BOOT_EFLASH;
  }

  return 0;
}

/**
  * @brief This function load all hash references from flash
  * @return 0 on success; nonzero on failure.
  */
int boot_hash_ref_load(void)
{
  /* Read hash references */
  if (FLASH_DEV_NAME.ReadData(BL2_HASH_REF_ADDR, ImageValidHashRef,
      (SHA256_LEN * MCUBOOT_IMAGE_NUMBER)) != ARM_DRIVER_OK)
  {
    return BOOT_EFLASH;
  }

  return 0;
}

/**
  * @brief This function set one hash reference in ram
  * @param hash_ref hash reference to update
  * @param size size of the hash references
  * @param image_index index of image corresponding to hash reference
  * @return 0 on success; nonzero on failure.
  */
int boot_hash_ref_set(uint8_t *hash_ref, uint8_t size, uint8_t image_index)
{
  /* Check size */
  if (size != SHA256_LEN)
  {
    return BOOT_EFLASH;
  }

  /* Check image index */
  if (image_index >= MCUBOOT_IMAGE_NUMBER)
  {
    return BOOT_EFLASH;
  }

  /* Set hash reference */
  memcpy(ImageValidHashRef + (image_index * SHA256_LEN), hash_ref, SHA256_LEN);

  /* Memorize that hash references will have to be updated in flash (later) */
  ImageValidHashUpdate++;

  return 0;
}

/**
  * @brief This function get one hash reference from ram
  * @param hash_ref hash reference to get
  * @param size size of the hash reference
  * @param image_index index of image corresponding to hash reference
  * @return 0 on success; nonzero on failure.
  */
int boot_hash_ref_get(uint8_t *hash_ref, uint8_t size, uint8_t image_index)
{
  /* Check size */
  if (size != SHA256_LEN)
  {
    return BOOT_EFLASH;
  }

  /* Check image index */
  if (image_index >= MCUBOOT_IMAGE_NUMBER)
  {
    return BOOT_EFLASH;
  }

  /* Get hash reference */
  memcpy(hash_ref, ImageValidHashRef + (image_index * SHA256_LEN), SHA256_LEN);

  return 0;
}
#endif /* MCUBOOT_USE_HASH_REF */

/**
  * @brief This function configures and enables the ICache.
  * @note
  * @retval execution_status
  */
void icache_init(void)
{
#ifdef ICACHE_MONITOR
    if (HAL_ICACHE_Monitor_Reset(ICACHE_MONITOR_HIT_MISS) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_ICACHE_Monitor_Start(ICACHE_MONITOR_HIT_MISS) != HAL_OK)
    {
        Error_Handler();
    }
#endif /* ICACHE_MONITOR */
    ICACHE_MONITOR_PRINT()

    /* Enable ICache */
    if (HAL_ICACHE_Enable() != HAL_OK)
    {
        Error_Handler();
    }
}

/* exported variables --------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  Platform init
  * @param  None
  * @retval status
  */
int32_t boot_platform_init(void)
{
    /* STM32xxx HAL library initialization:
         - Systick timer is configured by default as source of time base, but user
               can eventually implement his proper time base source (a general purpose
               timer for example or other time source), keeping in mind that Time base
               duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
               handled in milliseconds basis.
         - Set NVIC Group Priority to 3
         - Low Level Initialization
       */
    HAL_Init();

#ifdef MCUBOOT_HAVE_LOGGING
    /* Init for log */
    stdio_init();
#endif /*  MCUBOOT_HAVE_LOGGING */

#ifdef OEMIROT_ICACHE_ENABLE
    /* Configure and enable ICache */
    icache_init();
#endif /* ICACHE_ENABLED */

    /* Start HW randomization */
    RNG_Init();
   (void)fih_delay_init();
#if defined(OEMUROT_ENABLE)
    /* Disable and clean MPU configuration from previous boot stage for easier MPU reconfiguration */
    LL_SECU_DisableCleanMpu();
#endif /* OEMUROT_ENABLE */

    /* Apply Run time Protection */
    LL_SECU_ApplyRunTimeProtections();
    /* Check static protections */
    LL_SECU_CheckStaticProtections();

    /* Check Flow control state */
    FLOW_CONTROL_CHECK(uFlowProtectValue, FLOW_CTRL_STAGE_1);
    uFlowStage = FLOW_STAGE_CHK;
    /* Double protections apply / check to resist to basic fault injections */
    /* Apply Run time Protection */
   (void)fih_delay();
    LL_SECU_ApplyRunTimeProtections();
    /* Check static protections */
    LL_SECU_CheckStaticProtections();

    if (FLASH_DEV_NAME.Initialize(NULL) != ARM_DRIVER_OK)
    {
        BOOT_LOG_ERR("Error while initializing Flash Interface");
        Error_Handler();
    }

#if defined(OEMIROT_EXTERNAL_FLASH_ENABLE)
    if (EXT_FLASH_DEV_NAME.Initialize(NULL) != ARM_DRIVER_OK)
    {
        BOOT_LOG_ERR("Error while initializing external flash memory");
        Error_Handler();
    }
#endif /* OEMIROT_EXTERNAL_FLASH_ENABLE */

#if defined(MCUBOOT_USE_HASH_REF)
    /* Load all images hash references (for mcuboot) */
    if (boot_hash_ref_load())
    {
        BOOT_LOG_ERR("Error while loading Hash references from FLash");
        Error_Handler();
    }
#endif

    return 0;
}

/**
  * @brief  This function is executed in case of error occurrence.
  *         This function does not return.
  * @param  None
  * @retval None
  */
/* Place code in a specific section */
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".BL2_Error_Code"
__NO_RETURN void Error_Handler_rdp(void)
#else
__attribute__((section(".BL2_Error_Code")))
void Error_Handler_rdp(void)
#endif
{
    /* it is customizeable */
    /* an infinite loop,  and a reset for single fault injection */
    static __IO int twice = 0;
    while (!twice);
    NVIC_SystemReset();
#if !defined(__ICCARM__)
    /* Avoid bx lr instruction (for fault injection) */
    __builtin_unreachable();
#endif /* defined(__ICCARM__) */
}
/* Place code in a specific section */
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".BL2_Error_Code"
__NO_RETURN void Error_Handler(void)
#else
__attribute__((section(".BL2_Error_Code")))
void Error_Handler(void)
#endif
{
  /* it is customizeable */
  /* an infinite loop,  and a reset for single fault injection */
#ifdef OEMIROT_ERROR_HANDLER_STOP_EXEC
    static __IO int twice = 0;
    while (!twice);
#endif /* OEMIROT_ERROR_HANDLER_STOP_EXEC */
    NVIC_SystemReset();
#if !defined(__ICCARM__)
    /* Avoid bx lr instruction (for fault injection) */
    __builtin_unreachable();
#endif /* defined(__ICCARM__) */
}

/* Stop placing data in specified section */
#if defined(__ICCARM__)
#pragma default_function_attributes =
#endif /* __ICCARM__ */

#if defined(__ARMCC_VERSION)
/* reimplement the function to reach Error Handler */
void __aeabi_assert(const char *expr, const char *file, int line)
{
#ifdef OEMIROT_DEV_MODE
    printf("assertion \" %s \" failed: file %s %d\n", expr, file, line);
#endif /*  OEMIROT_DEV_MODE  */
    Error_Handler();

#if !defined(__ICCARM__)
    __builtin_unreachable();
#endif /* defined(__ICCARM__) */
}
#endif  /*  __ARMCC_VERSION */
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    Error_Handler();
}
#endif /* USE_FULL_ASSERT */
