/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    FLASH/FLASH_WriteProtection/Src/main.c
  * @author  MCD Application Team
  * @brief   This example provides a description of how to set write protection on
  *          STM32WBAxx FLASH.
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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define FLASH_USER_START_ADDR       ADDR_FLASH_PAGE_16   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR         ADDR_FLASH_PAGE_127 + FLASH_PAGE_SIZE - 1   /* End @ of user Flash area */

uint32_t QuadWord[4] = {0x12345678,
                        0x87654321,
                        0x12344321,
                        0x56788765
                       };

/* Uncomment this line to program the Flash pages */
#define FLASH_PAGE_PROGRAM

/* Uncomment this line to Enable Write Protection */
/* #define WRITE_PROTECTION_ENABLE */

/* Uncomment this line to Disable Write Protection */
/* #define WRITE_PROTECTION_DISABLE */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t StartPage = 0, EndPage = 0;
uint32_t Address = 0;
uint32_t PageError = 0;
__IO uint32_t MemoryProgramStatus = 0;
/*Variable used for Erase procedure*/
#ifdef FLASH_PAGE_PROGRAM
static FLASH_EraseInitTypeDef EraseInitStruct;
#endif
/*Variable used to handle the Options Bytes*/
static FLASH_OBProgramInitTypeDef OptionsBytesStruct, OptionsBytesStruct2;
static uint32_t Check_Program(uint32_t StartAddress, uint32_t EndAddress, uint32_t *Data);

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ICACHE_Init(void);
/* USER CODE BEGIN PFP */
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
  /* STM32WBAxx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ICACHE_Init();
  /* USER CODE BEGIN 2 */
  /* Initialize LD1, LD2 and LD3 */
  BSP_LED_Init(LD1);
  BSP_LED_Init(LD2);
  BSP_LED_Init(LD3);

  /* Disable instruction cache prior to internal cacheable memory update */
  if (HAL_ICACHE_Disable() != HAL_OK)
  {
    Error_Handler();
  }


  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  /* Unlock the Options Bytes *************************************************/
  HAL_FLASH_OB_Unlock();

  /* Get the number of the start and end pages */
  StartPage = GetPage(FLASH_USER_START_ADDR);
  EndPage   = GetPage(FLASH_USER_END_ADDR);

  OptionsBytesStruct.WRPArea  = OB_WRPAREA_BANK1_AREAA;
  OptionsBytesStruct2.WRPArea = OB_WRPAREA_BANK1_AREAB;

  /* Get pages write protection status ****************************************/
  HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct);
  HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct2);

#ifdef WRITE_PROTECTION_DISABLE
  /* Check if desired pages are already write protected ***********************/
  if ((OptionsBytesStruct.WRPStartOffset == StartPage) && (OptionsBytesStruct.WRPEndOffset == EndPage))
  {
    /* Current area correspond to the area to disable */
    OptionsBytesStruct.OptionType     = OPTIONBYTE_WRP;
    OptionsBytesStruct.WRPStartOffset = 0xFF;
    OptionsBytesStruct.WRPEndOffset   = 0;
  }
  else if ((OptionsBytesStruct.WRPStartOffset == StartPage) && (OptionsBytesStruct.WRPEndOffset > EndPage))
  {
    /* Current area is bigger than the area to disable : */
    /* - End of area is bigger than the last page to un-protect */
    OptionsBytesStruct.OptionType     = OPTIONBYTE_WRP;
    OptionsBytesStruct.WRPStartOffset = EndPage + 1;
  }
  else if ((OptionsBytesStruct.WRPStartOffset < StartPage) && (OptionsBytesStruct.WRPEndOffset == EndPage))
  {
    /* Current area is bigger than the area to disable : */
    /* - Start of area is lower than the first page to un-protect */
    OptionsBytesStruct.OptionType     = OPTIONBYTE_WRP;
    OptionsBytesStruct.WRPEndOffset   = StartPage - 1;
  }
  else if ((OptionsBytesStruct.WRPStartOffset < StartPage) && (OptionsBytesStruct.WRPEndOffset > EndPage))
  {
    /* Current area is bigger than the area to disable */
    /* - Start of area is lower than the first page to un-protect */
    /* - End of area is bigger than the last page to un-protect */
    if (OptionsBytesStruct2.WRPStartOffset > OptionsBytesStruct2.WRPEndOffset)
    {
      /* Second area of the bank can be used */
      OptionsBytesStruct2.OptionType     = OPTIONBYTE_WRP;
      OptionsBytesStruct2.WRPStartOffset = EndPage + 1;
      OptionsBytesStruct2.WRPEndOffset   = OptionsBytesStruct.WRPEndOffset;

      OptionsBytesStruct.OptionType     = OPTIONBYTE_WRP;
      OptionsBytesStruct.WRPEndOffset   = StartPage - 1;
    }
    else
    {
      /* Second area of the bank already used for WRP */
      /* => Error : not possible to deactivate only the pages indicated */
      while (1)
      {
        Error_Handler();
      }
    }
  }
  else if ((OptionsBytesStruct2.WRPStartOffset == StartPage) && (OptionsBytesStruct2.WRPEndOffset == EndPage))
  {
    /* Current area correspond to the area to disable */
    OptionsBytesStruct2.OptionType     = OPTIONBYTE_WRP;
    OptionsBytesStruct2.WRPStartOffset = 0xFF;
    OptionsBytesStruct2.WRPEndOffset   = 0;
  }
  else if ((OptionsBytesStruct2.WRPStartOffset == StartPage) && (OptionsBytesStruct2.WRPEndOffset > EndPage))
  {
    /* Current area is bigger than the area to disable : */
    /* - End of area is bigger than the last page to un-protect */
    OptionsBytesStruct2.OptionType     = OPTIONBYTE_WRP;
    OptionsBytesStruct2.WRPStartOffset = EndPage + 1;
  }
  else if ((OptionsBytesStruct2.WRPStartOffset < StartPage) && (OptionsBytesStruct2.WRPEndOffset == EndPage))
  {
    /* Current area is bigger than the area to disable : */
    /* - Start of area is lower than the first page to un-protect */
    OptionsBytesStruct2.OptionType     = OPTIONBYTE_WRP;
    OptionsBytesStruct2.WRPEndOffset   = StartPage - 1;
  }
  else if ((OptionsBytesStruct2.WRPStartOffset < StartPage) && (OptionsBytesStruct2.WRPEndOffset > EndPage))
  {
    /* Current area is bigger than the area to disable */
    /* - Start of area is lower than the first page to un-protect */
    /* - End of area is bigger than the last page to un-protect */
    if (OptionsBytesStruct.WRPStartOffset > OptionsBytesStruct.WRPEndOffset)
    {
      /* Second area of the bank can be used */
      OptionsBytesStruct.OptionType     = OPTIONBYTE_WRP;
      OptionsBytesStruct.WRPStartOffset = EndPage + 1;
      OptionsBytesStruct.WRPEndOffset   = OptionsBytesStruct2.WRPEndOffset;

      OptionsBytesStruct2.OptionType     = OPTIONBYTE_WRP;
      OptionsBytesStruct2.WRPEndOffset   = StartPage - 1;
    }
    else
    {
      /* Second area of the bank already used for WRP */
      /* => Error : not possible to deactivate only the pages indicated */
      while (1)
      {
        Error_Handler();
      }
    }
  }

#elif defined WRITE_PROTECTION_ENABLE
  /* Check if desired pages are not yet write protected ***********************/
  if ((OptionsBytesStruct.WRPStartOffset <= StartPage) && (OptionsBytesStruct.WRPEndOffset >= (StartPage - 1)))
  {
    /* Current area is adjacent to pages to be write protected */
    if (OptionsBytesStruct.WRPEndOffset < EndPage)
    {
      /* Current area will be extended to include the pages to be write protected */
      OptionsBytesStruct.OptionType     = OPTIONBYTE_WRP;
      OptionsBytesStruct.WRPEndOffset   = EndPage;
    }
  }
  else if ((OptionsBytesStruct.WRPStartOffset <= (EndPage + 1)) && (OptionsBytesStruct.WRPEndOffset >= EndPage))
  {
    /* Current area is adjacent to pages to be write protected */
    if (OptionsBytesStruct.WRPStartOffset > StartPage)
    {
      /* Current area will be extended to include the pages to be write protected */
      OptionsBytesStruct.OptionType     = OPTIONBYTE_WRP;
      OptionsBytesStruct.WRPStartOffset = StartPage;
    }
  }
  else if ((OptionsBytesStruct.WRPStartOffset > StartPage) && (OptionsBytesStruct.WRPEndOffset < EndPage))
  {
    /* Current area is included in pages to be write protected */
    OptionsBytesStruct.OptionType     = OPTIONBYTE_WRP;
    OptionsBytesStruct.WRPStartOffset = StartPage;
    OptionsBytesStruct.WRPEndOffset   = EndPage;
  }
  else if ((OptionsBytesStruct2.WRPStartOffset <= StartPage) && (OptionsBytesStruct2.WRPEndOffset >= (StartPage - 1)))
  {
    /* Current area is adjacent to pages to be write protected */
    if (OptionsBytesStruct2.WRPEndOffset < EndPage)
    {
      /* Current area will be extended to include the pages to be write protected */
      OptionsBytesStruct2.OptionType     = OPTIONBYTE_WRP;
      OptionsBytesStruct2.WRPEndOffset   = EndPage;
    }
  }
  else if ((OptionsBytesStruct2.WRPStartOffset <= (EndPage + 1)) && (OptionsBytesStruct2.WRPEndOffset >= EndPage))
  {
    /* Current area is adjacent to pages to be write protected */
    if (OptionsBytesStruct2.WRPStartOffset > StartPage)
    {
      /* Current area will be extended to include the pages to be write protected */
      OptionsBytesStruct2.OptionType     = OPTIONBYTE_WRP;
      OptionsBytesStruct2.WRPStartOffset = StartPage;
    }
  }
  else if ((OptionsBytesStruct2.WRPStartOffset > StartPage) && (OptionsBytesStruct2.WRPEndOffset < EndPage))
  {
    /* Current area is included in pages to be write protected */
    OptionsBytesStruct2.OptionType     = OPTIONBYTE_WRP;
    OptionsBytesStruct2.WRPStartOffset = StartPage;
    OptionsBytesStruct2.WRPEndOffset   = EndPage;
  }
  else if (OptionsBytesStruct.WRPStartOffset > OptionsBytesStruct.WRPEndOffset)
  {
    /* Current area is not used => it will be configured to protect the pages */
    OptionsBytesStruct.OptionType     = OPTIONBYTE_WRP;
    OptionsBytesStruct.WRPStartOffset = StartPage;
    OptionsBytesStruct.WRPEndOffset   = EndPage;
  }
  else if (OptionsBytesStruct2.WRPStartOffset > OptionsBytesStruct2.WRPEndOffset)
  {
    /* Current area is not used => it will be configured to protect the pages */
    OptionsBytesStruct2.OptionType     = OPTIONBYTE_WRP;
    OptionsBytesStruct2.WRPStartOffset = StartPage;
    OptionsBytesStruct2.WRPEndOffset   = EndPage;
  }
  else
  {
    /* No more area available to protect the pages */
    /* => Error : not possible to activate the pages indicated */
    while (1)
    {
      Error_Handler();
    }
  }

#endif /* WRITE_PROTECTION_DISABLE */

  /* Configure write protected pages */
  if (OptionsBytesStruct.OptionType == OPTIONBYTE_WRP)
  {
    if(HAL_FLASHEx_OBProgram(&OptionsBytesStruct) != HAL_OK)
    {
      /* Error occurred while options bytes programming. **********************/
      while (1)
      {
        Error_Handler();
      }
    }
  }

  if (OptionsBytesStruct2.OptionType == OPTIONBYTE_WRP)
  {
    if(HAL_FLASHEx_OBProgram(&OptionsBytesStruct2) != HAL_OK)
    {
      /* Error occurred while options bytes programming. **********************/
      while (1)
      {
        Error_Handler();
      }
    }
  }

  /* Generate System Reset to load the new option byte values ***************/
  if ((OptionsBytesStruct.OptionType == OPTIONBYTE_WRP) || (OptionsBytesStruct2.OptionType == OPTIONBYTE_WRP))
  {
    HAL_FLASH_OB_Launch();
  }

  /* Lock the Options Bytes *************************************************/
  HAL_FLASH_OB_Lock();

#ifdef FLASH_PAGE_PROGRAM
  /* The selected pages are write protected *******************************/
  if (((OptionsBytesStruct.WRPStartOffset  <= StartPage) && (OptionsBytesStruct.WRPEndOffset  >= EndPage)) ||
      ((OptionsBytesStruct2.WRPStartOffset <= StartPage) && (OptionsBytesStruct2.WRPEndOffset >= EndPage)))
  {
    /* The desired pages are write protected */
    /* Check that it is not allowed to write in this page */
    Address = FLASH_USER_START_ADDR;
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, Address, (uint32_t) QuadWord) != HAL_OK)
      {
      /* Error returned during programming. */
      /* Check that WRPERR flag is well set */
      if ((HAL_FLASH_GetError() & HAL_FLASH_ERROR_WRP) != 0)
      {
        MemoryProgramStatus++;
      }
      else
      {
        /* Another error occurred.
           User can add here some code to deal with this error */
        while (1)
        {
          Error_Handler();
        }
      }
    }
    else
    {
      /* Write operation is successful. Should not occur
         User can add here some code to deal with this error */
      while (1)
      {
        Error_Handler();
      }
    }
  }
  else
  {
    /* The desired pages are not write protected */
    /* Fill EraseInit structure************************************************/
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page        = StartPage;
    EraseInitStruct.NbPages     = EndPage - StartPage + 1;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
    {
      /*
        Error occurred while page erase.
        User can add here some code to deal with this error.
        PageError will contain the faulty page and then to know the code error on this page,
        user can call function 'HAL_FLASH_GetError()'
      */
      while (1)
      {
        Error_Handler();
      }
    }

    /* FLASH Word program of DATA at addresses defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR */
    Address = FLASH_USER_START_ADDR;
    while (Address < FLASH_USER_END_ADDR)
    {
      if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, Address, (uint32_t) QuadWord) == HAL_OK)
      {
        Address = Address + 16;
      }
      else
      {
        /* Error occurred while writing data in Flash memory.
           User can add here some code to deal with this error */
        while (1)
        {
          Error_Handler();
        }
      }
    }

    /* Check the correctness of written data
       MemoryProgramStatus = 0: data programmed correctly
       MemoryProgramStatus != 0: number of words not programmed correctly ******/
    MemoryProgramStatus = Check_Program(FLASH_USER_START_ADDR, FLASH_USER_END_ADDR, QuadWord);
  }
#endif /* FLASH_PAGE_PROGRAM */

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  /* Re-enable instruction cache */
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }

  /*Check if there is an issue to program data*/
  if (MemoryProgramStatus != 0)
  {
    /* Error detected. Switch on LD2*/
    BSP_LED_On(LD2);
  }
  else
  {
    /* No error detected. Switch on LD1*/
    BSP_LED_On(LD1);
  }

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
  RCC_OscInitStruct.PLL1.PLLM = 4;
  RCC_OscInitStruct.PLL1.PLLN = 25;
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
  RCC_ClkInitStruct.AHB5_PLL1_CLKDivider = RCC_SYSCLK_PLL1_DIV4;
  RCC_ClkInitStruct.AHB5_HSEHSI_CLKDivider = RCC_SYSCLK_HSEHSI_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ICACHE Initialization Function
  * @param None
  * @retval None
  */
static void MX_ICACHE_Init(void)
{

  /* USER CODE BEGIN ICACHE_Init 0 */

  /* USER CODE END ICACHE_Init 0 */

  /* USER CODE BEGIN ICACHE_Init 1 */

  /* USER CODE END ICACHE_Init 1 */

  /** Enable instruction cache in 1-way (direct mapped cache)
  */
  if (HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
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

/**
  * @brief  Check program operation.
  * param StartAddress Area start address
  * param EndAddress Area end address
  * param Data Expected data
  * @retval FailCounter
  */
static uint32_t Check_Program(uint32_t StartAddress, uint32_t EndAddress, uint32_t *Data)
{
  uint32_t Address;
  uint32_t index, FailCounter = 0;
  uint32_t data32;

  /* check the user Flash area word by word */
  Address = StartAddress;

  while(Address < EndAddress)
  {
    for(index = 0; index<4; index++)
    {
      data32 = *(uint32_t*)Address;
      if(data32 != Data[index])
      {
        FailCounter++;
        return FailCounter;
      }
      Address +=4;
    }
  }
  return FailCounter;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  while(1)
  {
    BSP_LED_On(LD3);
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
