/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
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

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRYP_HandleTypeDef hcryp;
__ALIGN_BEGIN static const uint32_t pKeyAES[4] __ALIGN_END = {
                            0x00000000,0x00000000,0x00000000,0x00000000};

OTFDEC_HandleTypeDef hotfdec1;

XSPI_HandleTypeDef hxspi1;

/* USER CODE BEGIN PV */
__IO uint8_t CmdCplt;


const uint32_t plain_data[] = { 0x3f25110f, 0x7760604d, 0xaecc9784, 0xf0eeef65, 0x10a1d1f1,
                                0x71515641, 0xa12111e0, 0xf177d156, 0x20a2d2f2, 0x72525642,
                                0xa22212e0, 0xf278d256, 0x30a3d3f3, 0x73535643, 0xa32313e0,
                                0xf37ad356, 0x40a4d4f4, 0x74545644, 0xa42414e0, 0xf47bd456,
                                0x50a5c5f5, 0x75555645, 0xa52515e0, 0xf57cd556, 0x60a6c6f6,
                                0x76565646, 0xa62616e0, 0xf67df6d6, 0x70a7c7e7, 0x77575647,
                                0xa72717e0, 0xf77ed756, 0x8028c8e8, 0x78585648, 0xa82818e0,
                                0xf8e8d8c8, 0xa02acaea, 0x7a5a564a, 0xaa2a1afa, 0xfaeadaca,
                                0xb0ebcbeb, 0x7b5b564b, 0xab2b1bfb, 0x77ebdb56, 0x4cec1cc0,
                                0x7c5c564c, 0xacac1cfc, 0x78ecdc56, 0x3ded9dd0, 0x7d5d564d,
                                0xadad1dfd, 0x7aeddd56, 0x5eee9ee0, 0x7e5e564e, 0xbeee1efe,
                                0x7beede56, 0x1fef9ff0, 0x7f6f564f, 0xbfaf1fff, 0x7cefdf56};

uint32_t Ciphered_data[256];
uint32_t Plain_AES_test[2*PLAIN_SIZE] = {0};
CRYP_HandleTypeDef hcryp;

uint32_t Key[4][4]     = { { 0x71234567, 0x89ABCDEF, 0x71234567, 0x89ABCDEF },
                           { 0xEDCBA987, 0x6543210F, 0xEDCBA987, 0x6543210F },
                           { 0x23456789, 0xABCDEF01, 0x23456789, 0xABCDEF01 },
                           { 0xCBA98765, 0x43210FED, 0xCBA98765, 0x43210FED }
};

uint32_t Nonce[2][2]   =    { { 0xA5A5A5A5, 0xC3C3C3C3 },
                              { 0x11111111, 0x55555555 }
};

OTFDEC_HandleTypeDef hotfdec;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ICACHE_Init(void);
static void MX_XSPI1_Init(void);
static void MX_OTFDEC1_Init(void);
static void MX_AES_Init(void);
/* USER CODE BEGIN PFP */
static void XSPI_WriteEnable(XSPI_HandleTypeDef *hxspi);
static void XSPI_AutoPollingMemReady(XSPI_HandleTypeDef *hxspi);
static void XSPI_AutoPollingMemWriteEnable(XSPI_HandleTypeDef *hxspi);
static void XSPI_QuadModeCfg(XSPI_HandleTypeDef *hxspi);
static void CRYP_Cipher(uint32_t index,uint32_t* pRange, uint32_t start_adress_writing_region);

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
  XSPI_RegularCmdTypeDef sCommand = {0};
  XSPI_MemoryMappedTypeDef sMemMappedCfg = {0};
  uint32_t address = 0;
  uint16_t index;
  __IO uint32_t *mem_addr;
  uint32_t Not_Identic =0 ;
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
  /* Configure LD1, LD2 and LD3 */
  BSP_LED_Init(LD1);
  BSP_LED_Init(LD2);
  BSP_LED_Init(LD3);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ICACHE_Init();
  MX_XSPI1_Init();
  MX_OTFDEC1_Init();
  MX_AES_Init();
  /* USER CODE BEGIN 2 */
  /* Configure the memory in quad mode ------------------------------------- */
  XSPI_QuadModeCfg(&hxspi1);

  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressWidth       = HAL_XSPI_ADDRESS_24_BITS;
  sCommand.AddressDTRMode     = HAL_XSPI_ADDRESS_DTR_DISABLE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;
  sCommand.DQSMode            = HAL_XSPI_DQS_DISABLE;

  /* Enable write operations ------------------------------------------ */
  XSPI_WriteEnable(&hxspi1);

  /* Erasing Sequence ------------------------------------------------- */
  sCommand.OperationType = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand.Instruction   = SECTOR_ERASE_CMD;
  sCommand.AddressMode   = HAL_XSPI_ADDRESS_1_LINE;
  sCommand.Address       = address;
  sCommand.DataMode      = HAL_XSPI_DATA_NONE;
  sCommand.DummyCycles   = 0U;

  if (HAL_XSPI_Command_IT(&hxspi1, &sCommand) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure automatic polling mode to wait for end of erase ------ */
  XSPI_AutoPollingMemReady(&hxspi1);

  /* Enable write operations ---------------------------------------- */
  XSPI_WriteEnable(&hxspi1);

  /* Writing Sequence ----------------------------------------------- */
  sCommand.Instruction        = QUAD_PAGE_PROG_CMD;
  sCommand.Address            = address;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_4_LINES;
  sCommand.DataLength         = BUFFERSIZE;
  sCommand.DummyCycles        = 0U;
  sCommand.DataMode           = HAL_XSPI_DATA_4_LINES;

  if (HAL_XSPI_Command(&hxspi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Encrypt the plain data ------------------------------------- */
  CRYP_Cipher(0,(uint32_t*)0x90000000,0);

  /* Transfer the encrypted data to the external memory ------------------------------------- */
  if(HAL_XSPI_Transmit(&hxspi1, (uint8_t*)Ciphered_data,  BUFFERSIZE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Memory-mapped mode configuration ------------------------------- */
  sCommand.OperationType = HAL_XSPI_OPTYPE_READ_CFG;
  sCommand.Instruction   = FOUR_IO_READ_CMD;
  sCommand.DummyCycles   = DUMMY_CLOCK_CYCLES_4READ;
  sCommand.AddressMode   = HAL_XSPI_ADDRESS_4_LINES;

  if (HAL_XSPI_Command(&hxspi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  sMemMappedCfg.TimeOutActivation  = HAL_XSPI_TIMEOUT_COUNTER_ENABLE;
  sMemMappedCfg.TimeoutPeriodClock = 0x50U;
   if (HAL_XSPI_MemoryMapped(&hxspi1, &sMemMappedCfg) != HAL_OK)
  {
    Error_Handler();
  }

  /* In memory-mapped mode, not possible to check if the memory is ready
  after the programming. So a delay corresponding to max page programming
  time is added */
  HAL_Delay(MEMORY_PAGE_PROG_DELAY);

  OTFDEC_RegionConfigTypeDef Config;

  hotfdec.Instance = OTFDEC1;
  HAL_OTFDEC_DeInit(&hotfdec);
  if (HAL_OTFDEC_Init(&hotfdec) != HAL_OK)
  {
    Error_Handler();
  }

  __HAL_OTFDEC_ENABLE_IT(&hotfdec, OTFDEC_ALL_INT);

  /* Set OTFDEC Mode */
  if (HAL_OTFDEC_RegionSetMode(&hotfdec, 0, OTFDEC_REG_MODE_INSTRUCTION_OR_DATA_ACCESSES) != HAL_OK)
  {
    Error_Handler();
  }
  /* Set OTFDEC Key */
  if (HAL_OTFDEC_RegionSetKey(&hotfdec, 0, Key[0])  != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure and lock region,enable OTFDEC decryption */
  Config.Nonce[0]     = Nonce[0][0];
  Config.Nonce[1]     = Nonce[0][1];
  Config.StartAddress = 0x90000000;
  Config.EndAddress   = 0x90000fff;
  Config.Version      = 0xFEDC;
  if (HAL_OTFDEC_RegionConfig(&hotfdec, 0, &Config, OTFDEC_REG_CONFIGR_LOCK_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Reading Sequence ----------------------------------------------- */
  mem_addr = (uint32_t *)(XSPI1_BASE_MM + address);
  for (index = 0; index < 60; index++)
  {
    if (*mem_addr != plain_data[index])
    {
      Not_Identic++;
    }
    mem_addr++;
  }

  if (Not_Identic !=0)
  {
    Error_Handler();
  }
  else{
    BSP_LED_Toggle(LD1);
    HAL_Delay(50);

    address += XSPI_PAGE_SIZE;
    if(address >= XSPI_END_ADDR)
    {
      address = 0;
    }

    /* Abort XSPI driver to stop the memory-mapped mode ------------ */
    if (HAL_XSPI_Abort(&hxspi1) != HAL_OK)
    {
      Error_Handler();
    }
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
  * @brief AES Initialization Function
  * @param None
  * @retval None
  */
static void MX_AES_Init(void)
{

  /* USER CODE BEGIN AES_Init 0 */

  /* USER CODE END AES_Init 0 */

  /* USER CODE BEGIN AES_Init 1 */

  /* USER CODE END AES_Init 1 */
  hcryp.Instance = AES;
  hcryp.Init.DataType = CRYP_NO_SWAP;
  hcryp.Init.KeySize = CRYP_KEYSIZE_128B;
  hcryp.Init.pKey = (uint32_t *)pKeyAES;
  hcryp.Init.Algorithm = CRYP_AES_ECB;
  hcryp.Init.DataWidthUnit = CRYP_DATAWIDTHUNIT_WORD;
  hcryp.Init.HeaderWidthUnit = CRYP_HEADERWIDTHUNIT_WORD;
  hcryp.Init.KeyIVConfigSkip = CRYP_KEYIVCONFIG_ALWAYS;
  if (HAL_CRYP_Init(&hcryp) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN AES_Init 2 */

  /* USER CODE END AES_Init 2 */

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
  * @brief OTFDEC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_OTFDEC1_Init(void)
{

  /* USER CODE BEGIN OTFDEC1_Init 0 */

  /* USER CODE END OTFDEC1_Init 0 */

  /* USER CODE BEGIN OTFDEC1_Init 1 */

  /* USER CODE END OTFDEC1_Init 1 */
  hotfdec1.Instance = OTFDEC1;
  if (HAL_OTFDEC_Init(&hotfdec1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN OTFDEC1_Init 2 */

  /* USER CODE END OTFDEC1_Init 2 */

}

/**
  * @brief XSPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_XSPI1_Init(void)
{

  /* USER CODE BEGIN XSPI1_Init 0 */

  /* USER CODE END XSPI1_Init 0 */

  /* USER CODE BEGIN XSPI1_Init 1 */

  /* USER CODE END XSPI1_Init 1 */
  /* XSPI1 parameter configuration*/
  hxspi1.Instance = XSPI1;
  hxspi1.Init.FifoThresholdByte = 4;
  hxspi1.Init.MemoryMode = HAL_XSPI_SINGLE_MEM;
  hxspi1.Init.MemoryType = HAL_XSPI_MEMTYPE_MACRONIX;
  hxspi1.Init.MemorySize = HAL_XSPI_SIZE_32MB;
  hxspi1.Init.ChipSelectHighTimeCycle = 1;
  hxspi1.Init.FreeRunningClock = HAL_XSPI_FREERUNCLK_DISABLE;
  hxspi1.Init.ClockMode = HAL_XSPI_CLOCK_MODE_0;
  hxspi1.Init.ClockPrescaler = 1;
  hxspi1.Init.SampleShifting = HAL_XSPI_SAMPLE_SHIFT_NONE;
  hxspi1.Init.DelayHoldQuarterCycle = HAL_XSPI_DHQC_DISABLE;
  hxspi1.Init.DelayBlockBypass = HAL_XSPI_DELAY_BLOCK_BYPASS;
  hxspi1.Init.MemorySelect = HAL_XSPI_CSSEL_NCS1;
  if (HAL_XSPI_Init(&hxspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN XSPI1_Init 2 */

  /* USER CODE END XSPI1_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
  * @brief  Command completed callback.
  * @param  hxspi: XSPI handle
  * @retval None
  */
void HAL_XSPI_CmdCpltCallback(XSPI_HandleTypeDef *hxspi)
{
  CmdCplt++;
}

/**
  * @brief  Transfer Error callback.
  * @param  hxspi: XSPI handle
  * @retval None
  */
void HAL_XSPI_ErrorCallback(XSPI_HandleTypeDef *hxspi)
{
  Error_Handler();
}

/**
  * @brief  This function send a Write Enable and wait it is effective.
  * @param  hxspi: XSPI handle
  * @retval None
  */
static void XSPI_WriteEnable(XSPI_HandleTypeDef *hxspi)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};

  /* Enable write operations ------------------------------------------ */
  sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand.Instruction        = WRITE_ENABLE_CMD;
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_NONE;
  sCommand.DummyCycles        = 0U;
  sCommand.DQSMode            = HAL_XSPI_DQS_DISABLE;

  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure automatic polling mode to wait for write enabling  ------ */
  XSPI_AutoPollingMemWriteEnable(&hxspi1);
}

/**
  * @brief  This function read the SR of the memory and wait the WIP.
  * @param  hxspi: XSPI handle
  * @retval None
  */
static void XSPI_AutoPollingMemReady(XSPI_HandleTypeDef *hxspi)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  uint8_t reg = 0xFFU;

  while((reg & SR_WIP_BIT) == SR_WIP_BIT)
  {
    /* Configure automatic polling mode to wait for write enabling ---- */
    sCommand.Instruction     = READ_STATUS_REG_CMD;
    sCommand.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    sCommand.AddressMode     = HAL_XSPI_ADDRESS_NONE;
    sCommand.AddressDTRMode  = HAL_XSPI_ADDRESS_DTR_DISABLE;
    sCommand.DataMode        = HAL_XSPI_DATA_1_LINE;
    sCommand.DataDTRMode     = HAL_XSPI_DATA_DTR_DISABLE;
    sCommand.DataLength      = 1U;
    sCommand.DummyCycles     = DUMMY_CLOCK_CYCLES_READ_REG;

    if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      Error_Handler();
    }

    if (HAL_XSPI_Receive(hxspi, &reg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      Error_Handler();
    }
  }
}

/**
  * @brief  This function read the SR of the memory and wait the WEL.
  * @param  hxspi: XSPI handle
  * @retval None
  */
static void XSPI_AutoPollingMemWriteEnable(XSPI_HandleTypeDef *hxspi)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  uint8_t reg = 0x0U;

  while((reg & SR_WEL_BIT) != SR_WEL_BIT)
  {
    /* Configure automatic polling mode to wait for write enabling ---- */
    sCommand.Instruction     = READ_STATUS_REG_CMD;
    sCommand.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    sCommand.AddressMode     = HAL_XSPI_ADDRESS_NONE;
    sCommand.AddressDTRMode  = HAL_XSPI_ADDRESS_DTR_DISABLE;
    sCommand.DataMode        = HAL_XSPI_DATA_1_LINE;
    sCommand.DataDTRMode     = HAL_XSPI_DATA_DTR_DISABLE;
    sCommand.DataLength      = 1U;
    sCommand.DummyCycles     = DUMMY_CLOCK_CYCLES_READ_REG;

    if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      Error_Handler();
    }

    if (HAL_XSPI_Receive(hxspi, &reg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      Error_Handler();
    }
  }
}

/**
  * @brief  This function configure the memory in Quad mode.
  * @param  hxspi: XSPI handle
  * @retval None
  */
static void XSPI_QuadModeCfg(XSPI_HandleTypeDef *hxspi)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  uint8_t reg[3];

  /* Read status register operation */
  sCommand.Instruction        = READ_STATUS_REG_CMD;
  sCommand.DataMode           = HAL_XSPI_DATA_1_LINE;
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_1_LINE;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;
  sCommand.DataLength         = 1U;
  sCommand.DummyCycles        = DUMMY_CLOCK_CYCLES_READ_REG;
  sCommand.DQSMode            = HAL_XSPI_DQS_DISABLE;

  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_XSPI_Receive(hxspi, &reg[0], HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Read configuration register operation */
  sCommand.Instruction        = READ_CONF_REG_CMD;
  sCommand.DataLength         = 2U;

  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_XSPI_Receive(hxspi, &reg[1], HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  SET_BIT(reg[0], SR_QE_BIT);

  /* Enable write operations ---------------------------------------- */
  sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand.Instruction        = WRITE_ENABLE_CMD;
  sCommand.DataMode           = HAL_XSPI_DATA_NONE;


  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure automatic polling mode to wait for write enabling ---- */
  XSPI_AutoPollingMemWriteEnable(&hxspi1);


  /* Write Status register (with quad mode enable) --------- */
  sCommand.Instruction    = WRITE_STATUS_REGISTER;
  sCommand.DataMode       = HAL_XSPI_DATA_1_LINE;
  sCommand.DataLength     = 3U;

  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_XSPI_Transmit(hxspi, &reg[0], HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Wait that the memory is ready ---------------------------------- */
  XSPI_AutoPollingMemReady(hxspi);
}


static void CRYP_Cipher(uint32_t index,uint32_t* pRange, uint32_t start_adress_writing_region)
{
  uint32_t tmp;
  uint32_t AESIV[4] = {0};
  uint32_t  j = 0;
  uint32_t Key_tmp[4]={Key[index][0],Key[index][1],Key[index][2],Key[index][3]};

  Key_tmp[0]=Key[index][0];
  Key_tmp[1]=Key[index][1];
  Key_tmp[2]=Key[index][2];
  Key_tmp[3]=Key[index][3];

  tmp       = Key_tmp[0];
  Key_tmp[0] = Key_tmp[3];
  Key_tmp[3] = tmp;
  tmp       = Key_tmp[1];
  Key_tmp[1] = Key_tmp[2];
  Key_tmp[2] = tmp;

  /* Enable CRYP clock */
 SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_AESEN); //  __HAL_RCC_CRYP_CLK_ENABLE();
 SET_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_AESRST); //  __HAL_RCC_CRYP_FORCE_RESET();
 CLEAR_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_AESRST);//   __HAL_RCC_CRYP_RELEASE_RESET();

  AESIV [3] = ((0)<<28) | ((0x90000000+start_adress_writing_region)>>4);
  AESIV [2] = 0xFEDC;
  AESIV [1] = Nonce[0][0];
  AESIV [0] = Nonce[0][1];

  /* Set the CRYP parameters */
  hcryp.Instance = AES;// CRYP;//
  /* Set the CRYP parameters */
  hcryp.Init.DataType  = CRYP_NO_SWAP;
  hcryp.Init.KeySize   = CRYP_KEYSIZE_128B;
  hcryp.Init.pKey      = Key_tmp;
  hcryp.Init.pInitVect = AESIV;
  hcryp.Init.Algorithm = CRYP_AES_CTR;
  HAL_CRYP_Init(&hcryp);
  for (j=0; j<15;j++)
  {
    Plain_AES_test[4*j] = plain_data[4*j+3+(start_adress_writing_region/4)];
    Plain_AES_test[4*j+3] = plain_data[4*j+(start_adress_writing_region/4)];
    Plain_AES_test[4*j+1] = plain_data[4*j+2+(start_adress_writing_region/4)];
    Plain_AES_test[4*j+2] = plain_data[4*j+1+(start_adress_writing_region/4)] ;
  }
  HAL_CRYP_Encrypt(&hcryp,  Plain_AES_test ,PLAIN_SIZE, Ciphered_data, 0x1000);

  for (j=0; j<(PLAIN_SIZE)/4;j++)
  {
    tmp = Ciphered_data[4*j];
    Ciphered_data[4*j] = Ciphered_data[4*j+3];
    Ciphered_data[4*j+3] = tmp;
    tmp = Ciphered_data[4*j+1];
    Ciphered_data[4*j+1] = Ciphered_data[4*j+2];
    Ciphered_data[4*j+2] = tmp;
  }

  HAL_CRYP_DeInit(&hcryp);
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
  /* Turn LD1 off */
  BSP_LED_Off(LD1);
  /* Toggle LD3 for error */
  while(1)
  {
    BSP_LED_Toggle(LD3);
    HAL_Delay(1000);
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
