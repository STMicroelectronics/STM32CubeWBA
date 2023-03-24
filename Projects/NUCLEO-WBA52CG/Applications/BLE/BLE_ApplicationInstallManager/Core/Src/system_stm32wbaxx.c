/**
  ******************************************************************************
  * @file    system_stm32wbaxx.c
  * @author  MCD Application Team
  * @brief   CMSIS Cortex-M33 Device Peripheral Access Layer System Source File
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  *   This file provides two functions and one global variable to be called from
  *   user application:
  *      - SystemInit(): This function is called at startup just after reset and
  *                      before branch to main program. This call is made inside
  *                      the "startup_stm32wbaxx.s" file.
  *
  *      - SystemCoreClock variable: Contains the core clock (HCLK), it can be used
  *                                  by the user application to setup the SysTick
  *                                  timer or configure other parameters.
  *
  *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
  *                                 be called whenever the core clock is changed
  *                                 during program execution.
  *
  *   After each device reset the HSI (16 MHz) is used as system clock source.
  *   Then SystemInit() function is called, in "startup_stm32wbaxx.s" file, to
  *   configure the system clock before to branch to main program.
  *
  *   This file configures the system clock as follows:
  *=============================================================================
  *-----------------------------------------------------------------------------
  *        System Clock source                     | HSI
  *-----------------------------------------------------------------------------
  *        SYSCLK(Hz)                              | 16000000
  *-----------------------------------------------------------------------------
  *        HCLK(Hz)                                | 16000000
  *-----------------------------------------------------------------------------
  *        AHB Prescaler                           | 1
  *-----------------------------------------------------------------------------
  *        APB1 Prescaler                          | 1
  *-----------------------------------------------------------------------------
  *        APB2 Prescaler                          | 1
  *-----------------------------------------------------------------------------
  *        PLL1_SRC                                | No clock
  *-----------------------------------------------------------------------------
  *        PLL1_M                                  | 1
  *-----------------------------------------------------------------------------
  *        PLL1_N                                  | 128
  *-----------------------------------------------------------------------------
  *        PLL1_P                                  | 1
  *-----------------------------------------------------------------------------
  *        PLL1_Q                                  | 1
  *-----------------------------------------------------------------------------
  *        PLL1_R                                  | 1
  *-----------------------------------------------------------------------------
  *        Require 48MHz for                       | Disabled
  *        SDIO and RNG clock                      |
  *-----------------------------------------------------------------------------
  *=============================================================================
  */

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup STM32WBAxx_system
  * @{
  */

/** @addtogroup STM32WBAxx_System_Private_Includes
  * @{
  */

#include "stm32wbaxx.h"
#include <math.h>
#include "app_common.h"

/**
  * @}
  */

/** @addtogroup STM32WBAxx_System_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32WBAxx_System_Private_Defines
  * @{
  */
#if !defined  (HSE_VALUE)
  #define HSE_VALUE    (32000000U) /*!< Value of the External oscillator in Hz */
#endif /* HSE_VALUE */

#if !defined  (HSI_VALUE)
  #define HSI_VALUE    (16000000U) /*!< Value of the Internal oscillator in Hz*/
#endif /* HSI_VALUE */

/* Note: Following vector table addresses must be defined in line with linker
         configuration. */
/*!< Uncomment the following line if you need to relocate the vector table
     anywhere in Flash or Sram, else the vector table is kept at the automatic
     remap of boot address selected */
/* #define USER_VECT_TAB_ADDRESS */

#if defined(USER_VECT_TAB_ADDRESS)
/*!< Uncomment the following line if you need to relocate your vector Table
     in Sram else user remap will be done in Flash. */
/* #define VECT_TAB_SRAM */
#if defined(VECT_TAB_SRAM)
#define VECT_TAB_BASE_ADDRESS   SRAM1_BASE      /*!< Vector Table base address field.
                                                     This value must be a multiple of 0x200. */
#define VECT_TAB_OFFSET         0x00000000U     /*!< Vector Table base offset field.
                                                     This value must be a multiple of 0x200. */
#else
#define VECT_TAB_BASE_ADDRESS   FLASH_BASE      /*!< Vector Table base address field.
                                                     This value must be a multiple of 0x200. */
#define VECT_TAB_OFFSET         0x00000000U     /*!< Vector Table base offset field.
                                                     This value must be a multiple of 0x200. */
#endif /* VECT_TAB_SRAM */
#endif /* USER_VECT_TAB_ADDRESS */

/****************************/
static void JumpFwApp( void );
static void BootModeCheck( void );
static void JumpSelectionOnPowerUp( void );
static void DeleteSlot( uint8_t page_idx );
static void MoveToActiveSlot( uint8_t page_idx );

typedef void (*fct_t)(void);

/******************************************************************************/

/**
  * @}
  */

/** @addtogroup STM32WBAxx_System_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32WBAxx_System_Private_Variables
  * @{
  */
  /* The SystemCoreClock variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetHCLKFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
         Note: If you use this function to configure the system clock; then there
               is no need to call the 2 first functions listed above, since SystemCoreClock
               variable is updated automatically.
  */
  uint32_t SystemCoreClock = 16000000U; /* The HSI16 is used as system clock source after startup from reset, configured at 16 MHz. */

  const uint8_t AHBPrescTable[8] = {0U, 0U, 0U, 0U, 1U, 2U, 3U, 4U};
  const uint8_t APBPrescTable[8] = {0U, 0U, 0U, 0U, 1U, 2U, 3U, 4U};
  const uint8_t AHB5PrescTable[8] = {1U, 1U, 1U, 1U, 2U, 3U, 4U, 6U};
/**
  * @}
  */

/** @addtogroup STM32WBAxx_System_Private_FunctionPrototypes
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32WBAxx_System_Private_Functions
  * @{
  */

/**
  * @brief  Setup the microcontroller system.
  * @param  None
  * @retval None
  */

void SystemInit(void)
{
  __IO uint32_t timeout_cpu_cycles;
  __IO uint32_t tmpreg;

  BootModeCheck();

  /* FPU settings ------------------------------------------------------------*/
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
  SCB->CPACR |= ((3UL << 20U)|(3UL << 22U));  /* set CP10 and CP11 Full Access */
#endif

  /* Configure the Vector Table location -------------------------------------*/
#if defined(USER_VECT_TAB_ADDRESS)
  SCB->VTOR = VECT_TAB_BASE_ADDRESS | VECT_TAB_OFFSET; /* Vector Table Relocation */
#endif /* USER_VECT_TAB_ADDRESS */

#if defined(STM32WBAXX_SI_CUT1_0)
  /* Work-around for ADC peripheral issue possibly impacting system
     power consumption.
     Refer to STM32WBA errata sheet item "HSI16 clock cannot be stopped when
     used as kernel clock by ADC".
     Actions: Perform a ADC activation sequence in order to update state
               of internal signals.
  */
  /* Enable ADC kernel clock */
  SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_ADC4EN);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->AHB4ENR, RCC_AHB4ENR_ADC4EN);
  (void)tmpreg;

  /* Enable ADC */
  SET_BIT(ADC4->CR, ADC_CR_ADEN);

  /* Poll for ADC ready */
  /* Set timeout 2 ADC clock cycles */
  /* Note: Approximative computation and timeout execution not taking into
           account processing CPU cycles */
  timeout_cpu_cycles = 2;
  while (READ_BIT(ADC4->ISR, ADC_ISR_ADRDY) == 0)
  {
    timeout_cpu_cycles--;
    if(timeout_cpu_cycles == 0)
    {
      break;
    }
  }

  /* Disable ADC */
  SET_BIT(ADC4->CR, ADC_CR_ADDIS);

  /* Poll for ADC disable is effective */
  /* Set timeout 6 ADC clock cycles */
  /* Note: Approximative computation and timeout execution not taking into
           account processing CPU cycles */
  timeout_cpu_cycles = 6;
  while (READ_BIT(ADC4->CR, ADC_CR_ADEN) != 0)
  {
    timeout_cpu_cycles--;
    if(timeout_cpu_cycles == 0)
    {
      break;
    }
  }

  /* Disable ADC internal voltage regulator */
  CLEAR_BIT(ADC4->CR, ADC_CR_ADVREGEN);

  /* Disable ADC kernel clock */
  CLEAR_BIT(RCC->AHB4ENR, RCC_AHB4ENR_ADC4EN);
#endif
}

/****************************/

/**
 * Return 0 if FW App not valid
 * Return 1 if Fw App valid
 */
static uint8_t  CheckFwAppValidity( uint8_t page_idx );


static uint8_t CheckFwAppValidity( uint8_t page_idx )
{
  uint8_t status;
  uint32_t magic_keyword_address;
  uint32_t last_user_flash_address;

  magic_keyword_address = *(uint32_t*)(FLASH_BASE + (page_idx * FLASH_PAGE_SIZE + 0x158));
  if(page_idx != CFG_ACTIVE_SLOT_START_SECTOR_INDEX)
  { /* magic_keyword_address is in the download slot = active slot + CFG_APP_SLOT_PAGE_SIZE */
    magic_keyword_address += ((page_idx - CFG_ACTIVE_SLOT_START_SECTOR_INDEX) * FLASH_PAGE_SIZE);
  }
  last_user_flash_address = (FLASH_SIZE + FLASH_BASE) - 4;
  if( (magic_keyword_address < FLASH_BASE) || (magic_keyword_address > last_user_flash_address) )
  {
    /**
     * The address is not valid
     */
    status = 0;
  }
  else
  {
    if( (*(uint32_t*)magic_keyword_address) != 0x94448A29  )
    {
      /**
       * A firmware update procedure did not complete
       */
      status = 0;
    }
    else
    {
      /**
       * The firmware application is available
       */
      status = 1;
    }
  }

  return status;
}

/**
 * Jump to existing FW App in flash
 * It never returns
 */
static void JumpFwApp( void )
{
  fct_t app_reset_handler;

  SCB->VTOR = FLASH_BASE + (CFG_ACTIVE_SLOT_START_SECTOR_INDEX * FLASH_PAGE_SIZE);
  __set_MSP(*(uint32_t*)(FLASH_BASE + (CFG_ACTIVE_SLOT_START_SECTOR_INDEX * FLASH_PAGE_SIZE)));
  app_reset_handler = (fct_t)(*(uint32_t*)(FLASH_BASE + (CFG_ACTIVE_SLOT_START_SECTOR_INDEX * FLASH_PAGE_SIZE) + 4));
  app_reset_handler();

  /**
   * app_reset_handler() never returns.
   * However, if for any reason a PUSH instruction is added at the entry of  JumpFwApp(),
   * we need to make sure the POP instruction is not there before app_reset_handler() is called
   * The way to ensure this is to add a dummy code after app_reset_handler() is called
   * This prevents app_reset_handler() to be the last code in the function.
   */
  __WFI();


  return;
}

/**
 * Erase active or download slot
 */
static void DeleteSlot( uint8_t page_idx )
{

  /**
   * The number of sectors to erase is read from SRAM1.
   * It shall be checked whether the number of sectors to erase does not overlap on the secured Flash
   * The limit can be read from the SFSA option byte which provides the first secured sector address.
   */

  uint32_t last_page_idx = page_idx + CFG_APP_SLOT_PAGE_SIZE - 1;
  FLASH_EraseInitTypeDef p_erase_init;
  uint32_t page_error;
  uint32_t NbrOfPageToBeErased = (uint32_t)CFG_APP_SLOT_PAGE_SIZE;

  if(page_idx < CFG_ACTIVE_SLOT_START_SECTOR_INDEX)
  {
    /**
     * Something has been wrong as there is no case we should delete the BLE_BootMngr application
     * Reboot on the active firmware application
     */
    CFG_OTA_REBOOT_VAL_MSG = CFG_REBOOT_ON_FW_APP;
    NVIC_SystemReset(); /* it waits until reset */
  }

  if ((page_idx + NbrOfPageToBeErased - 1) > last_page_idx)
  {
    NbrOfPageToBeErased = last_page_idx - page_idx + 1;
  }

  p_erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
  p_erase_init.NbPages = NbrOfPageToBeErased;
  p_erase_init.Page = (uint32_t)page_idx;

  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS); /* Clear all Flash flags before write operation*/
  
  HAL_FLASH_Unlock();

  HAL_FLASHEx_Erase(&p_erase_init, &page_error);

  HAL_FLASH_Lock();
  
  return;
}

/**
 * Move download slot to active slot
 */
static void MoveToActiveSlot( uint8_t page_idx )
{
  uint32_t dwn_addr, act_addr, last_addr;

  /**
   * Write in Flash the data received in the BLE packet
   */
  dwn_addr = FLASH_BASE + (page_idx * FLASH_PAGE_SIZE);
  act_addr = FLASH_BASE + (CFG_ACTIVE_SLOT_START_SECTOR_INDEX * FLASH_PAGE_SIZE);
  /* Last address of the new binary relative to the start address in the active slot */
  last_addr = *(uint32_t*)(dwn_addr + 0x158);
  last_addr &= 0xFFFFFFF0;
  
  /**
   * The flash is written by bunch of 16 bytes
   * Data are written in flash as long as there are at least 16 bytes
   */
  while( act_addr <= last_addr )
  {
    HAL_StatusTypeDef status = HAL_ERROR;
    
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS); /* Clear all Flash flags before write operation*/

    HAL_FLASH_Unlock();
    
    while(status != HAL_OK)
      status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, 
                                 act_addr, 
                                 dwn_addr);
    
    HAL_FLASH_Lock();
    
    if((*(uint64_t*)(act_addr) == *(uint64_t*)(dwn_addr)) &&
       (*(uint64_t*)((act_addr)+8) == *(uint64_t*)((dwn_addr)+8)))
    {
      act_addr += 16;
      dwn_addr += 16;
    }
  }
}
      
/**
 * Check the Boot mode request
 * Depending on the result, the CPU may either jump to an existing application in the user flash
 * or keep on running the code to start the OTA loader
 */
static void BootModeCheck( void )
{
  if(LL_RCC_IsActiveFlag_SFTRST( ) || LL_RCC_IsActiveFlag_OBLRST( ))
  {
    /**
     * The SRAM1 content is kept on Software Reset.
     * In the Ble_Ota application, the first address of the SRAM1 indicates which kind of action has been requested
     */

    /**
     * Check Boot Mode from SRAM1
     */
    if((CFG_OTA_REBOOT_VAL_MSG == CFG_REBOOT_ON_FW_APP) && (CheckFwAppValidity(CFG_ACTIVE_SLOT_START_SECTOR_INDEX) != 0))
    {
      uint8_t download_slot_start_sector = CFG_OTA_START_SECTOR_IDX_VAL_MSG;
      
      if((download_slot_start_sector >= FLASH_PAGE_NB) || 
         ((download_slot_start_sector < CFG_ACTIVE_SLOT_START_SECTOR_INDEX)))
      {
        /* CFG_OTA_START_SECTOR_IDX_VAL_MSG not correctly initialized */
        download_slot_start_sector = CFG_DOWNLOAD_SLOT_START_SECTOR_INDEX;
        CFG_OTA_START_SECTOR_IDX_VAL_MSG = CFG_DOWNLOAD_SLOT_START_SECTOR_INDEX;
      }
      
      if(CheckFwAppValidity(download_slot_start_sector) != 0)
      {
        /**
         * The user has requested to start on the firmware application and it has been checked
         * a valid application is ready in the download slot
         */
        DeleteSlot( CFG_ACTIVE_SLOT_START_SECTOR_INDEX );     /* Erase active slot */
        MoveToActiveSlot(download_slot_start_sector);                           /* Move download slot to active slot */
        if(CheckFwAppValidity(CFG_ACTIVE_SLOT_START_SECTOR_INDEX) != 0)
        {
          DeleteSlot( CFG_DOWNLOAD_SLOT_START_SECTOR_INDEX ); /* Erase download slot */
        }
      }
      
      /**
       * Jump now on the application
       */
      JumpFwApp();
    }
    else if((CFG_OTA_REBOOT_VAL_MSG == CFG_REBOOT_ON_FW_APP) && (CheckFwAppValidity(CFG_ACTIVE_SLOT_START_SECTOR_INDEX) == 0))
    {
      /**
       * The user has requested to start on the firmware application but there is no valid application
       * Erase all sectors specified by byte1 and byte1 in SRAM1 to download a new App.
       */
      CFG_OTA_REBOOT_VAL_MSG = CFG_REBOOT_ON_BLE_OTA_APP;     /* Request to reboot on BLE_Ota application */
      CFG_OTA_START_SECTOR_IDX_VAL_MSG = CFG_ACTIVE_SLOT_START_SECTOR_INDEX;
      CFG_OTA_NBR_OF_SECTOR_VAL_MSG = 0xFF;
    }
    else if(CFG_OTA_REBOOT_VAL_MSG == CFG_REBOOT_ON_BLE_OTA_APP)
    {
      /**
       * It has been requested to reboot on BLE_Ota application to download data
       * Erase download slot to download a new App.
       */
      DeleteSlot( CFG_DOWNLOAD_SLOT_START_SECTOR_INDEX );     /* Erase download slot */
      JumpFwApp();                                            /* Jump to OTA application */
    }
    else if(CFG_OTA_REBOOT_VAL_MSG == CFG_REBOOT_ON_CPU2_UPGRADE)
    {
      /**
       * It has been requested to reboot on BLE_Ota application to keep running the firmware upgrade process
       *
       */
    }
    else
    {
      /**
       * There should be no use case to be there because the device already starts from power up
       * and the SRAM1 is then filled with the value define by the user
       * However, it could be that a reset occurs just after a power up and in that case, the Ble_Ota
       * will be running but the sectors to download a new App may not be erased
       */
      JumpSelectionOnPowerUp( );
    }
  }
  else
  {
    /**
     * On Power up, the content of SRAM1 is random
     * The only thing that could be done is to jump on either the firmware application
     * or the Ble_Ota application
     */
    JumpSelectionOnPowerUp( );
  }

  /**
   * Return to the startup file and run the Ble_Ota application
   */
  return;
}

static void JumpSelectionOnPowerUp( void )
{
  /**
   * Check if there is a FW App
   */
  if(CheckFwAppValidity(CFG_ACTIVE_SLOT_START_SECTOR_INDEX) != 0)
  {
    /**
     * The SRAM1 is random
     * Initialize SRAM1 to indicate we requested to reboot of firmware application
     */
    CFG_OTA_REBOOT_VAL_MSG = CFG_REBOOT_ON_FW_APP;

    /**
     * A valid application is available
     * Jump now on the application
     */
    JumpFwApp();
  }
  else
  {
    /**
     * The SRAM1 is random
     * Initialize SRAM1 to indicate we requested to reboot of BLE_Ota application
     */
    CFG_OTA_REBOOT_VAL_MSG = CFG_REBOOT_ON_BLE_OTA_APP;

    /**
     * There is no valid application available
     * Erase all sectors specified by byte1 and byte1 in SRAM1 to download a new App.
     */
    CFG_OTA_START_SECTOR_IDX_VAL_MSG = CFG_ACTIVE_SLOT_START_SECTOR_INDEX;
    CFG_OTA_NBR_OF_SECTOR_VAL_MSG = 0xFF;
  }
  return;
}

/**
  * @brief  Update SystemCoreClock variable according to Clock Register Values.
  *         The SystemCoreClock variable contains the core clock (HCLK), it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  *
  * @note   Each time the core clock (HCLK) changes, this function must be called
  *         to update SystemCoreClock variable value. Otherwise, any configuration
  *         based on this variable will be incorrect.
  *
  * @note   - The system frequency computed by this function is not the real
  *           frequency in the chip. It is calculated based on the predefined
  *           constant and the selected clock source:
  *
  *           - If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(**)
  *
  *           - If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(***)
  *
  *           - If SYSCLK source is PLL1, SystemCoreClock will contain the HSE_VALUE(***)
  *             or HSI_VALUE(*)  multiplied/divided by the PLL1 factors.
  *
  *         (**) HSI_VALUE is a constant defined in STM32WBAxx_hal.h file (default value
  *              16 MHz) but the real value may vary depending on the variations
  *              in voltage and temperature.
  *
  *         (***) HSE_VALUE is a constant defined in STM32WBAxx_hal.h file (default value
  *              32 MHz), user has to ensure that HSE_VALUE is same as the real
  *              frequency of the crystal used. Otherwise, this function may
  *              have wrong result.
  *
  *         - The result of this function could be not correct when using fractional
  *           value for HSE crystal.
  *
  * @param  None
  * @retval None
  */
void SystemCoreClockUpdate(void)
{
  uint32_t tmp1;
  uint32_t tmp2;
  uint32_t pllsource;
  uint32_t pllr;
  uint32_t pllm;
  uint32_t plln;
  float_t fracn;
  float_t pllvco;

  /* Get SYSCLK source -------------------------------------------------------*/
  switch (RCC->CFGR1 & RCC_CFGR1_SWS)
  {
    case RCC_CFGR1_SWS_1:  /* HSE used as system clock source */
      SystemCoreClock = (HSE_VALUE >> ((RCC->CR & RCC_CR_HSEPRE) >> RCC_CR_HSEPRE_Pos));
      break;

    case (RCC_CFGR1_SWS_0 | RCC_CFGR1_SWS_1):  /* PLL1 used as system clock source */
      /* PLL_VCO = (PLLsource / PLLM) * PLLN * FractionnalPart
          SYSCLK = PLL_VCO / PLLR */
      /* Get PLL1 CFGR and DIVR register values */
      tmp1 = RCC->PLL1CFGR;
      tmp2 = RCC->PLL1DIVR;

      /* Retrieve PLL1 multiplication factor and divider */
      pllm = ((tmp1 & RCC_PLL1CFGR_PLL1M) >> RCC_PLL1CFGR_PLL1M_Pos) + 1U;
      plln = (tmp2 & RCC_PLL1DIVR_PLL1N) + 1U;
      pllr = ((tmp2 & RCC_PLL1DIVR_PLL1R) >> RCC_PLL1DIVR_PLL1R_Pos) + 1U;

      /* Check if fractional part is enable */
      if ((tmp1 & RCC_PLL1CFGR_PLL1FRACEN) != 0x00u)
      {
        fracn = ((RCC->PLL1FRACR & RCC_PLL1FRACR_PLL1FRACN) >> RCC_PLL1FRACR_PLL1FRACN_Pos);
      }
      else
      {
        fracn = 0;
      }

      /* determine PLL source */
      pllsource = (tmp1 & RCC_PLL1CFGR_PLL1SRC);
      switch (pllsource)
      {
        /* HSI used as PLL1 clock source */
        case RCC_PLL1CFGR_PLL1SRC_1:
          tmp1 = HSI_VALUE;
          break;

        /* HSE used as PLL1 clock source */
        case (RCC_PLL1CFGR_PLL1SRC_0 | RCC_PLL1CFGR_PLL1SRC_1):
          tmp1 = (HSE_VALUE >> ((RCC->CR & RCC_CR_HSEPRE) >> RCC_CR_HSEPRE_Pos));
          break;

        default:
          tmp1 = 0U;
          break;
      }

      /* Compute VCO output frequency */
      pllvco = ((float) tmp1 / (float)pllm) * (((float)plln + (float)(fracn / 0x2000u)));
      SystemCoreClock = (uint32_t)((float_t) pllvco /(float_t) pllr);
      break;

    case 0x00u:  /* HSI used as system clock source */
    default:
      SystemCoreClock = HSI_VALUE;
      break;
  }

  /* Compute HCLK clock frequency --------------------------------------------*/
  /* Get HCLK prescaler */
  tmp1 = AHBPrescTable[(RCC->CFGR2 & RCC_CFGR2_HPRE)];

  /* HCLK clock frequency */
  SystemCoreClock >>= tmp1;
}


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
