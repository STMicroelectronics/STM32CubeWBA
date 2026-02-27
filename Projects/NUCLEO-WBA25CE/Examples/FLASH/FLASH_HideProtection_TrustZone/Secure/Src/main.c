/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Secure/Src/main.c
  * @author  MCD Application Team
  * @brief   Secure main application
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Flash bank 1 pages configuration :
   - pages 0  to 9   : secure code (located in HDP area)
   - pages 10 to 14  : area used by the example (located in HDP area)
   - page 15         : area used by the example (located in Extended HDP area)
   - page 16         : secure code (located in Extended HDP area)
   - pages 17 to 19  : area used by the example (located in non-HDP area)
   - pages 20 to 127 : secure code (located in non-HDP area) ******************/
#define FLASH_USER_START_HIDE_ADDR     ADDR_FLASH_PAGE_10           /* Start @ of user Flash area (included in secure hide protection) */
#define FLASH_USER_END_HIDE_ADDR       (ADDR_FLASH_PAGE_15 - 1)     /* End @ of user Flash area (included in secure hide protection) */
#define FLASH_USER_START_EXT_HIDE_ADDR ADDR_FLASH_PAGE_15           /* Start @ of user Flash area (excluded from secure hide protection) */
#define FLASH_USER_END_EXT_HIDE_ADDR   (ADDR_FLASH_PAGE_16 - 1)     /* End @ of user Flash area (excluded from secure hide protection) */
#define FLASH_USER_START_NON_HIDE_ADDR ADDR_FLASH_PAGE_17           /* Start @ of user Flash area (excluded from secure hide protection) */
#define FLASH_USER_END_NON_HIDE_ADDR   (ADDR_FLASH_PAGE_20 - 1)     /* End @ of user Flash area (excluded from secure hide protection) */
#define FLASH_USER_START_ADDR          FLASH_USER_START_HIDE_ADDR   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR            FLASH_USER_END_NON_HIDE_ADDR /* End @ of user Flash area */

#define DATA_32                        ((uint32_t)0x12345678)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t FirstPage = 0U, NbOfPages = 0U;
uint32_t Address = 0U, PageError = 0U;
__IO uint32_t MemoryProgramStatus = 0U;
__IO uint32_t data32 = 0;
uint32_t DoubleWord[2] = {0x12345678,
                          0x12345678};

/* Structure used for Erase procedure */
static FLASH_EraseInitTypeDef EraseInitStruct;
/* Structure used for Extended Hide Protection area */
static FLASH_HDPExtensionTypeDef ExtHDPStruct;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void NonSecure_Init(void);
static void MX_GTZC_S_Init(void);
static void MX_SAU_Init(void);
/* USER CODE BEGIN PFP */
static void HDP_Activation(void);
static void HDP_Check(void);
static void HDPExt_Activation(void);
static void HDPExt_Check(void);
static uint32_t GetPage(uint32_t Address);

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

  /* STM32WBAxx **SECURE** HAL library initialization:
       - Secure Systick timer is configured by default as source of time base,
         but user can eventually implement his proper time base source (a general
         purpose timer for example or other time source), keeping in mind that
         Time base duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined
         and handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* Initialize LD2 and LD3 */
  BSP_LED_Init(LD2);
  BSP_LED_Init(LD3);

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GTZC_S_Init();
  MX_SAU_Init();
  /* USER CODE BEGIN 2 */
  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  /* Configure the Extended Hide Protection area ******************************/
  ExtHDPStruct.Bank = FLASH_BANK_1;
  ExtHDPStruct.NbPages = GetPage(FLASH_USER_START_NON_HIDE_ADDR) - GetPage(FLASH_USER_START_EXT_HIDE_ADDR);
  HAL_FLASHEx_ConfigHDPExtension(&ExtHDPStruct);

  /* Erase the user Flash area
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_EXT_HIDE_ADDR) **/

  /* Get the 1st page to erase */
  FirstPage = GetPage(FLASH_USER_START_ADDR);

  /* Get the number of pages to erase from 1st page */
  NbOfPages = GetPage(FLASH_USER_END_EXT_HIDE_ADDR) - FirstPage + 1;

  /* Fill EraseInit structure */
  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.Page        = FirstPage;
  EraseInitStruct.NbPages     = NbOfPages;

  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
  {
    /*
      Error occurred while page erase.
      User can add here some code to deal with this error.
      PageError will contain the faulty page and then to know the code error on this page,
      user can call function 'HAL_FLASH_GetError()'
    */
    Error_Handler();
  }

  /* Erase the user Flash area
    (area defined by FLASH_USER_START_NON_HIDE_ADDR and FLASH_USER_END_ADDR) **/

  /* Get the 1st page to erase */
  FirstPage = GetPage(FLASH_USER_START_NON_HIDE_ADDR);

  /* Get the number of pages to erase from 1st page */
  NbOfPages = GetPage(FLASH_USER_END_ADDR) - FirstPage + 1;

  /* Fill EraseInit structure */
  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.Page        = FirstPage;
  EraseInitStruct.NbPages     = NbOfPages;

  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
  {
    /*
      Error occurred while page erase.
      User can add here some code to deal with this error.
      PageError will contain the faulty page and then to know the code error on this page,
      user can call function 'HAL_FLASH_GetError()'
    */
    Error_Handler();
  }

  /* Program the user Flash area word by word
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_EXT_HIDE_ADDR) **/

  Address = FLASH_USER_START_ADDR;

  while (Address < FLASH_USER_END_EXT_HIDE_ADDR)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, ((uint32_t)DoubleWord)) == HAL_OK)
    {
      Address = Address + 8U;  /* increment to next double word */
    }
    else
    {
      /* Error occurred while writing data in Flash memory.
         User can add here some code to deal with this error */
      Error_Handler();
    }
  }

  /* Program the user Flash area word by word
    (area defined by FLASH_USER_START_NON_HIDE_ADDR and FLASH_USER_END_ADDR) **/

  Address = FLASH_USER_START_NON_HIDE_ADDR;

  while (Address < FLASH_USER_END_ADDR)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, ((uint32_t)DoubleWord)) == HAL_OK)
    {
      Address = Address + 8U;  /* Increment to next double word */
    }
    else
    {
      /* Error occurred while writing data in Flash memory.
         User can add here some code to deal with this error */
      Error_Handler();
    }
  }

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  /* Secure SysTick should rather be suspended before calling non-secure  */
  /* in order to avoid wake-up from sleep mode entered by non-secure      */
  /* The Secure SysTick shall be resumed on non-secure callable functions */
  HAL_SuspendTick();

  /* Configure as non-secure the pin used for non-secure LED */
  HAL_GPIO_ConfigPinAttributes(LD2_GPIO_PORT, LD2_PIN, GPIO_PIN_NSEC);
  HAL_GPIO_ConfigPinAttributes(LD3_GPIO_PORT, LD3_PIN, GPIO_PIN_NSEC);

  /* Activate secure hide protection */
  HDP_Activation();
  /* USER CODE END 2 */

  /* Non-secure software does not return, this code is not executed */
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEDiv = RCC_HSE_DIV1;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL1.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL1.PLLM = 2;
  RCC_OscInitStruct.PLL1.PLLN = 8;
  RCC_OscInitStruct.PLL1.PLLP = 2;
  RCC_OscInitStruct.PLL1.PLLQ = 2;
  RCC_OscInitStruct.PLL1.PLLR = 2;
  RCC_OscInitStruct.PLL1.PLLFractional = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK7|RCC_CLOCKTYPE_HCLK5;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB7CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHB5_PLL1_CLKDivider = RCC_SYSCLK_PLL1_DIV1;
  RCC_ClkInitStruct.AHB5_HSEHSI_CLKDivider = RCC_SYSCLK_HSEHSI_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  Non-secure call function
  *         This function is responsible for Non-secure initialization and switch
  *         to non-secure state
  * @retval None
  */
#if defined(__ICCARM__)
static void NonSecure_Init(void) @ ".non_hdp"
#else
static void __attribute__((section(".non_hdp"), noinline)) NonSecure_Init(void)
#endif
{
  uint32_t vtor_ns;
  funcptr_NS NonSecure_ResetHandler;

  /* There shall be no need to configure VTOR_NS as it's updated from BOOT_NS option bytes */
  vtor_ns = SCB_NS->VTOR;

  /* Set non-secure main stack (MSP_NS) */
  __TZ_set_MSP_NS((*(uint32_t *)vtor_ns));

  /* Get non-secure reset handler */
  NonSecure_ResetHandler = (funcptr_NS)(*((uint32_t *)((vtor_ns) + 4U)));

  /* Start non-secure state software application */
  NonSecure_ResetHandler();
}

/**
  * @brief GTZC_S Initialization Function
  * @param None
  * @retval None
  */
static void MX_GTZC_S_Init(void)
{

  /* USER CODE BEGIN GTZC_S_Init 0 */

  /* USER CODE END GTZC_S_Init 0 */

  MPCBB_ConfigTypeDef MPCBB_Area_Desc = {0};

  /* USER CODE BEGIN GTZC_S_Init 1 */

  /* USER CODE END GTZC_S_Init 1 */
  MPCBB_Area_Desc.SecureRWIllegalMode = GTZC_MPCBB_SRWILADIS_ENABLE;
  MPCBB_Area_Desc.InvertSecureState = GTZC_MPCBB_INVSECSTATE_NOT_INVERTED;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[0] = 0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[1] = 0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[2] = 0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[3] = 0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[0] = 0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[1] = 0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[2] = 0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[3] = 0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_LockConfig_array[0] = 0x00000000;
  if (HAL_GTZC_MPCBB_ConfigMem(SRAM1_BASE, &MPCBB_Area_Desc) != HAL_OK)
  {
    Error_Handler();
  }

  MPCBB_Area_Desc.SecureRWIllegalMode = GTZC_MPCBB_SRWILADIS_ENABLE;
  MPCBB_Area_Desc.InvertSecureState = GTZC_MPCBB_INVSECSTATE_NOT_INVERTED;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[0] = 0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[1] = 0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[0] = 0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[1] = 0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_LockConfig_array[0] = 0x00000000;
  if (HAL_GTZC_MPCBB_ConfigMem(SRAM2_BASE, &MPCBB_Area_Desc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN GTZC_S_Init 2 */

  /* USER CODE END GTZC_S_Init 2 */

}

/**
  * @brief SAU Initialization Function
  * @param None
  * @retval None
  */
static void MX_SAU_Init(void)
{

  /* USER CODE BEGIN SAU_Init 0 */

  /* USER CODE END SAU_Init 0 */

  /* USER CODE BEGIN SAU_Init 1 */

  /* USER CODE END SAU_Init 1 */
  /* USER CODE BEGIN SAU_Init 2 */

  /* USER CODE END SAU_Init 2 */

}

/* USER CODE BEGIN 4 */
/**
  * @brief  Activation of the secure hide protection.
  * @retval None
  */
#if defined(__ICCARM__)
static void HDP_Activation(void) @ ".ram_code"
#else
static void __attribute__((section(".ram_code"), noinline)) HDP_Activation(void)
#endif
{
  /* Enable the Security Hide Protection */
  HAL_FLASHEx_EnableSecHideProtection(FLASH_BANK_1);

  /* Check the data in the HDP area */
  HDP_Check();
}

#if defined(__ICCARM__)
static void HDPExt_Activation(void) @ ".ram_code"
#else
static void __attribute__((section(".ram_code"), noinline)) HDPExt_Activation(void)
#endif
{
  /* Enable the Extended Security Hide Protection */
  HAL_FLASHEx_EnableHDPExtensionProtection(FLASH_BANK_1, FLASH_EXTHDP_ONLY_ACCESS_PROTECTED);

  /* Check the data in the HDP area */
  HDPExt_Check();
}

/**
  * @brief  Data check in the secure hide protection.
  * @retval None
  */
#if defined(__ICCARM__)
static void HDP_Check(void) @ ".hdpext"
#else
static void __attribute__((section(".hdpext"), noinline)) HDP_Check(void)
#endif
{
  MemoryProgramStatus = 0x0U;

  /************* Check data after Security Hide Protection enabled ************/
  /* Check if the programmed data is OK
      MemoryProgramStatus = 0: read as '0' because access to secure hide protection area not allowed
      MemoryProgramStatus != 0: read values different to '0' ******/
  Address = FLASH_USER_START_HIDE_ADDR;
  while (Address < FLASH_USER_END_HIDE_ADDR)
  {
    data32 = *(__IO uint32_t *)Address;

    if (data32 != 0U)
    {
      MemoryProgramStatus++;
    }
    Address = Address + 4U;
  }

  /* Check if the programmed data is OK
      MemoryProgramStatus = 0: data programmed correctly
      MemoryProgramStatus != 0: number of words not programmed correctly ******/
  Address = FLASH_USER_START_EXT_HIDE_ADDR;
  while (Address < FLASH_USER_END_EXT_HIDE_ADDR)
  {
    data32 = *(__IO uint32_t *)Address;

    if (data32 != DATA_32)
    {
      MemoryProgramStatus++;
    }
    Address = Address + 4U;
  }

  Address = FLASH_USER_START_NON_HIDE_ADDR;
  while (Address < FLASH_USER_END_NON_HIDE_ADDR)
  {
    data32 = *(__IO uint32_t *)Address;

    if (data32 != DATA_32)
    {
      MemoryProgramStatus++;
    }
    Address = Address + 4U;
  }

  /* Activate secure hide protection */
  HDPExt_Activation();
}

/**
  * @brief  Data check in the secure hide protection.
  * @retval None
  */
#if defined(__ICCARM__)
static void HDPExt_Check(void) @ ".non_hdp"
#else
static void __attribute__((section(".non_hdp"), noinline)) HDPExt_Check(void)
#endif
{
  /* Check if the programmed data is OK
      MemoryProgramStatus = 0: read as '0' because access to secure hide protection area not allowed
      MemoryProgramStatus != 0: read values different to '0' ******/
  Address = FLASH_USER_START_EXT_HIDE_ADDR;
  while (Address < FLASH_USER_END_EXT_HIDE_ADDR)
  {
    data32 = *(__IO uint32_t *)Address;

    if (data32 != 0U)
    {
      MemoryProgramStatus++;
    }
    Address = Address + 4U;
  }

  /* Check if the programmed data is OK
      MemoryProgramStatus = 0: data programmed correctly
      MemoryProgramStatus != 0: number of words not programmed correctly ******/
  Address = FLASH_USER_START_NON_HIDE_ADDR;
  while (Address < FLASH_USER_END_NON_HIDE_ADDR)
  {
    data32 = *(__IO uint32_t *)Address;

    if (data32 != DATA_32)
    {
      MemoryProgramStatus++;
    }
    Address = Address + 4U;
  }

  /*************** Setup and jump to non-secure *******************************/
  NonSecure_Init();
}

/**
  * @brief  Gets the page of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The page of a given address
  */
static uint32_t GetPage(uint32_t Addr)
{
  uint32_t page = 0;

  if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
  {
    /* Bank 1 */
    page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  }
  else
  {
    /* Bank 2 */
    page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
  }

  return page;
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
  /* Switch on LD3 */
  BSP_LED_On(LD3);

  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

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
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
