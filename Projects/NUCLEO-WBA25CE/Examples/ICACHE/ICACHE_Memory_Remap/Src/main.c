/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : ICACHE\ICACHE_Memory_Remap\Src\main.c
  * @brief          : Main program body
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* Function pointer declaration used to call remapped function */
typedef void (*funcptr)(void);

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define EXT_MEM_ADDRESS       0x90000000
#define XSPI_START_OFFSET_ADD 0

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static uint8_t *ext_flash_addr;
static uint32_t ext_flash_size;
static ICACHE_RegionConfigTypeDef region_config;
static funcptr IO_Blink_extFLASH_Remapped;
BSP_XSPI_Init_t XSPI_init;

#if defined(__ARMCC_VERSION)
extern uint32_t Load$$EXTFLASH_REGION$$Base;
extern uint32_t Load$$EXTFLASH_REGION$$Length;
#elif defined(__ICCARM__)
#pragma section =".ext_flash_region"
#pragma section =".ext_flash_region_init"
#elif defined(__GNUC__)
extern uint32_t _extflash_region_init_base;
extern uint32_t _extflash_region_init_length;
#endif

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ICACHE_Init(void);
/* USER CODE BEGIN PFP */
static void IO_Blink_extFLASH(void);

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

  /* Init XSPI */
  XSPI_init.InterfaceMode = MX25R3235F_QUAD_IO_MODE;
  BSP_XSPI_Init(0, &XSPI_init);

  /* Erase external memory Block (4KByte)*/
  BSP_XSPI_Erase_Block(0, XSPI_START_OFFSET_ADD, MX25R3235F_ERASE_4K);
  while(BSP_XSPI_GetStatus(0) == BSP_ERROR_BUSY);

  BSP_LED_Init(LED_RED);
  BSP_LED_Init(LED_GREEN);

  /* Memory copy from internal Flash to external Flash */
#if defined(__ARMCC_VERSION)
  ext_flash_size = (uint32_t)(&Load$$EXTFLASH_REGION$$Length);
  ext_flash_addr = (uint8_t *)(&Load$$EXTFLASH_REGION$$Base);
#elif defined(__ICCARM__)
  ext_flash_size = __section_size(".ext_flash_region");
  ext_flash_addr = (uint8_t *)(__section_begin(".ext_flash_region_init"));
#elif defined(__GNUC__)
  ext_flash_size = (uint32_t)((uint8_t *)(&_extflash_region_init_length));
  ext_flash_addr = (uint8_t *)(&_extflash_region_init_base);
#endif

  /* Preload external ospi memory with ciphred code into dedicated address*/
  BSP_XSPI_Write(0, (uint8_t *)ext_flash_addr, XSPI_START_OFFSET_ADD, ext_flash_size);

  /* Activate memory mapping */
  BSP_XSPI_EnableMemoryMappedMode(0);

  /* Execute function from external flash before remap */
  IO_Blink_extFLASH(); /* Blink the Green Led once, before remapping */

  /* Disable Instruction cache prior to enable the remapped region */
  HAL_ICACHE_Disable();

  /* Remap external flash memory to C-Bus */
  region_config.BaseAddress     = 0x10000000;
  region_config.RemapAddress    = EXT_MEM_ADDRESS;
  region_config.TrafficRoute    = ICACHE_MASTER2_PORT;
  region_config.OutputBurstType = ICACHE_OUTPUT_BURST_INCR;
  region_config.Size            = ICACHE_REGIONSIZE_2MB;

  /* Enable the remapped region */
  if(HAL_ICACHE_EnableRemapRegion(ICACHE_REGION_0, &region_config) != HAL_OK)
  {
    Error_Handler();
  }

  /* Enable the Instruction and Data Cache */
  HAL_ICACHE_Enable();

   /* Set remap Blink function pointer */
  IO_Blink_extFLASH_Remapped = (funcptr)(0x10000001UL);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    IO_Blink_extFLASH_Remapped();
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
  RCC_ClkInitStruct.AHB5_PLL1_CLKDivider = RCC_SYSCLK_PLL1_DIV2;
  RCC_ClkInitStruct.AHB5_HSEHSI_CLKDivider = RCC_SYSCLK_HSEHSI_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
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
  * @brief  Blinking the Green led function located in external Flash
  * @retval None
  */
#if defined(__ICCARM__)
static void IO_Blink_extFLASH(void) @ ".ext_flash_region"
#elif defined(__ARMCC_VERSION)|| defined(__GNUC__)
static void __attribute__((section(".ext_flash_region"), noinline)) IO_Blink_extFLASH(void)
#endif
{
  /* Turn on the Green Led */
  CLEAR_BIT(LD2_GPIO_PORT->ODR, LD2_PIN);

  /* Delay */
  for(uint32_t i=0;i< 0xFFFFFU;i++ )
  {
    __asm("nop");
  }

  /* Turn off the Green Led */
  SET_BIT(LD2_GPIO_PORT->ODR, LD2_PIN);

  /* Delay */
  for(uint32_t i=0;i< 0xFFFFFU;i++ )
  {
    __asm("nop");
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
  BSP_LED_On(LED_RED);
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

  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
