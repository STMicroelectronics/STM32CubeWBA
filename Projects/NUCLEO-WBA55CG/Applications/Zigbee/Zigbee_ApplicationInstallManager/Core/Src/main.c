/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_zigbee_ota_defines.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef void (* ResetHandler_t)(void);

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config               (void);

/* USER CODE BEGIN PFP */
void     BootModeCheck                ( void );
uint32_t CheckFwAppValidity           ( void );
uint32_t CheckFwDownloadedValidity    ( void );

static void     MoveToActiveSlot      ( uint32_t lEndAppFwAddress );

static void     JumpSelectionOnPowerUp( void );
void            JumpFwApp             ( void );

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
void BootModeCheck( void )
{
  uint32_t  lEndAppFwAddress = 0;
  
  if ( LL_RCC_IsActiveFlag_SFTRST( ) || LL_RCC_IsActiveFlag_OBLRST() )
  {
    /* The SRAM1 content is kept on Software Reset. First address of the SRAM1 indicates which kind of action has been requested */

    /* First, verify if FW App is valid */
    lEndAppFwAddress = CheckFwAppValidity();
  
    /* Check Boot Mode from SRAM1 */
    if ( FUOTA_OTA_REBOOT_MSG == FUOTA_REBOOT_ON_APP_FW )
    {
      /* The user has requested to start on the firmware application */
      if ( lEndAppFwAddress != 0u )
      {
        /* The FW application is a valid application. Verify if a valid application is also ready in the download slot and copy it */
        MoveToActiveSlot( lEndAppFwAddress );
    
        /* Jump now on the application */
        JumpFwApp();
      }
      else
      {
        /* Firmware application is no valid application. Request to reboot on Application Install Manager */
        FUOTA_OTA_REBOOT_MSG = FUOTA_REBOOT_ON_UPDATE_FW;
        
        /* Reboot on Update FW */
        NVIC_SystemReset();
      }
    }
    else 
    {
      if ( FUOTA_OTA_REBOOT_MSG == FUOTA_REBOOT_ON_UPDATE_FW )
      {
        /* Update Required. Verify if a valid application is also ready in the download slot and copy it */
        MoveToActiveSlot( lEndAppFwAddress );
        
        /* Jump to Fw application */
        if ( CheckFwAppValidity() != 0u )
        {
            JumpFwApp();
        }
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
  }
  else
  {
    /* By default, Jump after Power On */
    JumpSelectionOnPowerUp();
  }
}


/**
 * @brief   Verify if FW application is valid.
 * 
 * @return  0 if FW App not valid, else end of FW Application address.
 */
uint32_t CheckFwAppValidity( void )
{
  uint32_t  lMagicKeywordAddress, lEndFwAddress = 0;
  uint32_t  lEndFlashAddress;
  
  lMagicKeywordAddress = *(uint32_t *)( FUOTA_APP_FW_BINARY_ADDRESS + FUOTA_APP_FW_TAG_OFFSET );
  lEndFlashAddress = ( FLASH_BASE + FLASH_SIZE ) - 4u;
  
  if ( ( lMagicKeywordAddress >= FLASH_BASE) && ( lMagicKeywordAddress <= lEndFlashAddress ) )
  {
    if( ( *(uint32_t*)lMagicKeywordAddress) == FUOTA_MAGIC_KEYWORD_END_APPLI  )
    {
      /* The firmware application is available */
      lEndFwAddress = lMagicKeywordAddress + 4u;
    }
  }

  return lEndFwAddress;
}


/**
 * @brief   Verify if FW Downloaded is valid.
 * 
 * @return  0 if FW App not valid, else end of FW Download address.
 */
uint32_t CheckFwDownloadedValidity( void )
{
  uint32_t  lMagicKeywordAddress, lEndFwAddress = 0;
  uint32_t  lEndFlashAddress;
  
  lMagicKeywordAddress = *(uint32_t *)( FUOTA_APP_DOWNLOAD_BINARY_ADDRESS + FUOTA_APP_FW_TAG_OFFSET );
  lMagicKeywordAddress = lMagicKeywordAddress + ( FUOTA_APP_DOWNLOAD_BINARY_ADDRESS - FUOTA_APP_FW_BINARY_ADDRESS );
  lEndFlashAddress = ( FLASH_BASE + FLASH_SIZE ) - 4u;
  
  if ( ( lMagicKeywordAddress >= FLASH_BASE) && ( lMagicKeywordAddress <= lEndFlashAddress ) )
  {
    if( ( *(uint32_t*)lMagicKeywordAddress) == FUOTA_MAGIC_KEYWORD_END_APPLI  )
    {
      /* The firmware application is available */
      lEndFwAddress = lMagicKeywordAddress + 4u;
    }
  }

  return lEndFwAddress;
}


/**
 * Jump to existing FW App in flash
 * It never returns
 */
void JumpFwApp( void )
{
  ResetHandler_t   fAppResetHandler;

  SCB->VTOR = FUOTA_APP_FW_BINARY_ADDRESS;
  __set_MSP( *(uint32_t*)( FUOTA_APP_FW_BINARY_ADDRESS ) );
  fAppResetHandler = (ResetHandler_t)(*(uint32_t*)( FUOTA_APP_FW_BINARY_ADDRESS + 4u) );
  fAppResetHandler();

  /**
   * fAppResetHandler() never returns.
   * However, if for any reason a PUSH instruction is added at the entry of  JumpFwApp(),
   * we need to make sure the POP instruction is not there before fAppResetHandler() is called
   * The way to ensure this is to add a dummy code after fAppResetHandler() is called
   * This prevents fAppResetHandler() to be the last code in the function.
   */
  __WFI();
}


/**
 * Erase active or download slot
 */
static void DeleteSlot( uint32_t lStartAddress, uint32_t lEndAddress )
{
  /* It shall be checked whether the number of sectors to erase does not overlap on the secured Flash
   * The limit can be read from the SFSA option byte which provides the first secured sector address. */
  FLASH_EraseInitTypeDef stEraseInit;
  uint32_t lPageError;
  
  if ( lStartAddress < FUOTA_APP_FW_BINARY_ADDRESS )
  {
      /* Something has been wrong as there is no case we should delete the Zigbee_BootMngr application. Reboot on the active firmware application */
      FUOTA_OTA_REBOOT_MSG = FUOTA_REBOOT_ON_APP_FW;
      NVIC_SystemReset();
  }

  stEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
  stEraseInit.Page = (uint32_t)( ( lStartAddress - FLASH_BASE ) / FLASH_PAGE_SIZE );
  stEraseInit.NbPages = (uint32_t)( ( lEndAddress - lStartAddress ) / FLASH_PAGE_SIZE );
  if ( ( ( lEndAddress - lStartAddress ) % FLASH_PAGE_SIZE ) != 0u )
  {
      stEraseInit.NbPages++;
  }
    
#if defined(FLASH_DBANK_SUPPORT)
  stEraseInit.Banks = FUOTA_APP_DOWNLOAD_BINARY_BANK;
#endif

  /* Clear all Flash flags before write operation*/
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
  
  HAL_FLASH_Unlock();
  HAL_FLASHEx_Erase( &stEraseInit, &lPageError );
  HAL_FLASH_Lock();
}


/**
 * Move download slot to active slot
 */
static void CopyToActiveSlot( uint32_t lAppliStartAddress, uint32_t lDownloadedStartAddress, uint32_t lDownloadedEndAddress )
{
  HAL_StatusTypeDef   eStatus;
  uint32_t            lAppliAddress, lDownloadedAddress;
      
  /* The flash is written by bunch of 16 bytes. Data are written in flash as long as there are at least 16 bytes */
  lAppliAddress = lAppliStartAddress;
  lDownloadedAddress = lDownloadedStartAddress;
  while ( lDownloadedAddress <= lDownloadedEndAddress )
  {
    /* Clear all Flash flags before write operation*/
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS); 

    HAL_FLASH_Unlock();
    do
    {
      eStatus = HAL_FLASH_Program( FLASH_TYPEPROGRAM_QUADWORD, lAppliAddress, lDownloadedAddress );
    }
    while ( eStatus != HAL_OK);
    HAL_FLASH_Lock();

    if ( ( *(uint64_t *)(lAppliAddress) == *(uint64_t *)(lDownloadedAddress) ) && ( *(uint64_t *)( lAppliAddress + 8u ) == *(uint64_t *)( lDownloadedAddress + 8u ) ) )
    {
      lAppliAddress += 16;
      lDownloadedAddress += 16;
    }
  }
}


static void MoveToActiveSlot( uint32_t lEndAppFwAddress )
{
  uint32_t  lEndDownloadFwAddress, lDownloadFwSize;
  
  /* It has been requested to reboot on App Install Manager application to transfer data. */
  lEndDownloadFwAddress = CheckFwDownloadedValidity();
  if ( lEndDownloadFwAddress != 0u )
  {
    /* If End of Application FW address not found, erase until Download Zone */
    if ( lEndAppFwAddress == 0u )
    {
      lEndAppFwAddress = FUOTA_APP_DOWNLOAD_BINARY_ADDRESS - 1u;
    }
    
    /* Erase active slot */
    lDownloadFwSize = lEndDownloadFwAddress - FUOTA_APP_DOWNLOAD_BINARY_ADDRESS;
    DeleteSlot( FUOTA_APP_FW_BINARY_ADDRESS, ( FUOTA_APP_FW_BINARY_ADDRESS + lDownloadFwSize ) );
    
    /* Move download slot to active slot */
    CopyToActiveSlot( FUOTA_APP_FW_BINARY_ADDRESS, FUOTA_APP_DOWNLOAD_BINARY_ADDRESS, lEndDownloadFwAddress ); 
    
    if ( CheckFwAppValidity() != 0u )
    {
      /* Erase download slot */
      DeleteSlot( FUOTA_APP_DOWNLOAD_BINARY_ADDRESS, lEndDownloadFwAddress ); 
    }
  }
}



static void JumpSelectionOnPowerUp( void )
{
  /* Check if there is a FW App */
  if ( CheckFwAppValidity() != 0u )
  {
      /* The SRAM1 is random. Initialize SRAM1 to indicate we requested to reboot of firmware application */
      FUOTA_OTA_REBOOT_MSG = FUOTA_REBOOT_ON_APP_FW;

      /* A valid application is available. Jump now on the application */
      JumpFwApp();
  }
  else
  {
      /* The SRAM1 is random. Initialize SRAM1 to indicate we requested to reboot of Install Manager application */
      FUOTA_OTA_REBOOT_MSG = FUOTA_REBOOT_ON_UPDATE_FW;
  }
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
