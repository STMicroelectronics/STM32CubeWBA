/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef void (*fct_t)(void);
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* Define the NVM size in sector */
#define CFG_NVM_NB_SECTORS                  (2)
/* Define the User Data size in sector */
#define CFG_USER_DATA_NB_SECTORS            (1)
/* Define the start address where the application shall be located */
#define CFG_ACTIVE_SLOT_START_SECTOR_INDEX  (3)

/* Define mapping of OTA messages in SRAM */
#define CFG_OTA_REBOOT_VAL_MSG              (*(uint8_t*)(SRAM1_BASE+0))
#define CFG_OTA_START_SECTOR_IDX_VAL_MSG    (*(uint8_t*)(SRAM1_BASE+1))

/* Size in Page of Download and Active slots */
#define DOWNLOAD_ACTIVE_NB_SECTORS          ((FLASH_SIZE / FLASH_PAGE_SIZE) - CFG_NVM_NB_SECTORS - CFG_USER_DATA_NB_SECTORS)
/* Start address where the User Data shall be located */
#define USER_CFG_SLOT_START_SECTOR_INDEX    ((FLASH_SIZE / FLASH_PAGE_SIZE) - CFG_NVM_NB_SECTORS - CFG_USER_DATA_NB_SECTORS)
/* Size in Page of Download or active slot */
#define APP_SLOT_PAGE_SIZE                  ((DOWNLOAD_ACTIVE_NB_SECTORS - CFG_ACTIVE_SLOT_START_SECTOR_INDEX) / 2)
/* Define the start address where the NEW application shall be downloaded */
#define DOWNLOAD_SLOT_START_SECTOR_INDEX    (APP_SLOT_PAGE_SIZE + CFG_ACTIVE_SLOT_START_SECTOR_INDEX)

/* Define list of reboot reason */
#define REBOOT_ON_FW_APP                    (0x00)
#define REBOOT_ON_APP_INSTALL_MNGR          (0x01)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void BootModeCheck( void );
static uint8_t CheckFwAppValidity( uint8_t page_idx );
void JumpFwApp( void );
static void DeleteSlot( uint8_t page_idx );
static void MoveToActiveSlot( uint8_t page_idx );
static void JumpSelectionOnPowerUp( void );
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Enable PWR clock interface */

  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_PWR);

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
  BootModeCheck();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_1)
  {
  }

  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE2);
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {
  }

  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {
  }

  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAHB5Divider(LL_RCC_AHB5_DIVIDER_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
  LL_RCC_SetAPB7Prescaler(LL_RCC_APB7_DIV_1);
  LL_SetSystemCoreClock(16000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
     * First address of the SRAM1 indicates which kind of action has been requested
     */

    /**
     * Check Boot Mode from SRAM1
     */
    if((CFG_OTA_REBOOT_VAL_MSG == REBOOT_ON_FW_APP) && (CheckFwAppValidity(CFG_ACTIVE_SLOT_START_SECTOR_INDEX) != 0))
    {
      uint8_t download_slot_start_sector = CFG_OTA_START_SECTOR_IDX_VAL_MSG;
      
      if((download_slot_start_sector >= (FLASH_SIZE / FLASH_PAGE_SIZE)) || 
         ((download_slot_start_sector < CFG_ACTIVE_SLOT_START_SECTOR_INDEX)))
      {
        /* CFG_OTA_START_SECTOR_IDX_VAL_MSG not correctly initialized */
        download_slot_start_sector = DOWNLOAD_SLOT_START_SECTOR_INDEX;
        CFG_OTA_START_SECTOR_IDX_VAL_MSG = DOWNLOAD_SLOT_START_SECTOR_INDEX;
      }
      
      if(CheckFwAppValidity(download_slot_start_sector) != 0)
      {
        /**
         * The user has requested to start on the firmware application and it has been checked
         * a valid application is ready in the download slot
         */
        DeleteSlot( CFG_ACTIVE_SLOT_START_SECTOR_INDEX ); /* Erase active slot */
        MoveToActiveSlot(download_slot_start_sector); /* Move download slot to active slot */
        if(CheckFwAppValidity(CFG_ACTIVE_SLOT_START_SECTOR_INDEX) != 0)
        {
          DeleteSlot( DOWNLOAD_SLOT_START_SECTOR_INDEX ); /* Erase download slot */
        }
      }
      
      /**
       * Jump now on the application
       */
      JumpFwApp();
    }
    else if((CFG_OTA_REBOOT_VAL_MSG == REBOOT_ON_FW_APP) && (CheckFwAppValidity(CFG_ACTIVE_SLOT_START_SECTOR_INDEX) == 0))
    {
      /**
       * The user has requested to start on the firmware application but there is no valid application
       * Erase all sectors specified by byte1 and byte1 in SRAM1 to download a new App.
       */
      CFG_OTA_REBOOT_VAL_MSG = REBOOT_ON_APP_INSTALL_MNGR; /* Request to reboot on Application Install Manager */
      CFG_OTA_START_SECTOR_IDX_VAL_MSG = CFG_ACTIVE_SLOT_START_SECTOR_INDEX;
    }
    else if(CFG_OTA_REBOOT_VAL_MSG == REBOOT_ON_APP_INSTALL_MNGR)
    {
      /**
       * It has been requested to reboot on App Install Manager application to download data
       * Erase download slot to download a new App.
       */
      DeleteSlot( DOWNLOAD_SLOT_START_SECTOR_INDEX ); /* Erase download slot */
      JumpFwApp();                                    /* Jump to OTA application */
    }
    else
    {
      /**
       * There should be no use case to be there because the device already starts from power up
       * and the SRAM1 is then filled with the value define by the user
       * However, it could be that a reset occurs just after a power up and in that case,
       * App Install Manager will be running but the sectors to download a new App may not be erased
       */
      JumpSelectionOnPowerUp( );
    }
  }
  else
  {
    /**
     * On Power up, the content of SRAM1 is random
     * The only thing that could be done is to jump on either the firmware application
     * or the App Install Manager application
     */
    JumpSelectionOnPowerUp( );
  }

  return;
}

/**
 * Return 0 if FW App not valid
 * Return 1 if Fw App valid
 */
static uint8_t CheckFwAppValidity( uint8_t page_idx )
{
  uint8_t status;
  uint32_t magic_keyword_address;
  uint32_t last_user_flash_address;

  magic_keyword_address = *(uint32_t*)(FLASH_BASE + (page_idx * FLASH_PAGE_SIZE + 0x160));
  if(page_idx != CFG_ACTIVE_SLOT_START_SECTOR_INDEX)
  { 
    /* magic_keyword_address is in the download slot = active slot + APP_SLOT_PAGE_SIZE */
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
void JumpFwApp( void )
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

  uint32_t last_page_idx = page_idx + APP_SLOT_PAGE_SIZE - 1;
  FLASH_EraseInitTypeDef p_erase_init;
  uint32_t page_error;
  uint32_t NbrOfPageToBeErased = (uint32_t)APP_SLOT_PAGE_SIZE;

  if(page_idx < CFG_ACTIVE_SLOT_START_SECTOR_INDEX)
  {
    /**
     * Something has been wrong as there is no case we should delete the BLE_BootMngr application
     * Reboot on the active firmware application
     */
    CFG_OTA_REBOOT_VAL_MSG = REBOOT_ON_FW_APP;
    NVIC_SystemReset(); /* it waits until reset */
  }

  if ((page_idx + NbrOfPageToBeErased - 1) > last_page_idx)
  {
    NbrOfPageToBeErased = last_page_idx - page_idx + 1;
  }

  p_erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
  p_erase_init.NbPages = NbrOfPageToBeErased;
  p_erase_init.Page = (uint32_t)(page_idx & (FLASH_PAGE_NB - 1u));

#if defined(FLASH_DBANK_SUPPORT)
  if ((FLASH_PAGE_NB & page_idx) ^ (1u & READ_BIT (FLASH->OPTR, FLASH_OPTR_SWAP_BANK_Msk)))
  {
    p_erase_init.Banks = FLASH_BANK_2;
  }
  else
  {
    p_erase_init.Banks = FLASH_BANK_1;
  } 
#endif

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
  last_addr = *(uint32_t*)(dwn_addr + 0x160);
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
    CFG_OTA_REBOOT_VAL_MSG = REBOOT_ON_FW_APP;

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
    CFG_OTA_REBOOT_VAL_MSG = REBOOT_ON_APP_INSTALL_MNGR;

    /**
     * There is no valid application available
     * Erase all sectors specified by byte1 and byte1 in SRAM1 to download a new App.
     */
    CFG_OTA_START_SECTOR_IDX_VAL_MSG = CFG_ACTIVE_SLOT_START_SECTOR_INDEX;
  }
  return;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
