/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    CRYP/CRYP_DMA/Src/main.c
  * @author  MCD Application Team
  * @brief   This example provides a short description of how to use the CRYPTO
  *          peripheral to encrypt and decrypt data using CRYP Algorithm with
  *          AES ECB chaining mode.
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

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRYP_HandleTypeDef hcryp;
__ALIGN_BEGIN static const uint32_t pKeyAES[4] __ALIGN_END = {
                            0x2B7E1516,0x28AED2A6,0xABF71588,0x09CF4F3C};
DMA_HandleTypeDef handle_GPDMA1_Channel1;
DMA_HandleTypeDef handle_GPDMA1_Channel0;

/* USER CODE BEGIN PV */
/* Private define ------------------------------------------------------------*/

#define KEY_SIZE          128 /* Key size in bits */

/* The size of the plaintext in Words */
#define PLAINTEXT_SIZE          ((uint32_t)16)
#define AES_TEXT_SIZE           ((uint32_t)16)

#define ECB               1
#define CBC               2
#define CTR               3


/* Plaintext */
uint32_t aPlaintext[AES_TEXT_SIZE] =
                        { 0x6BC1BEE2 ,0x2E409F96 ,0xE93D7E11 ,0x7393172A ,
                          0xAE2D8A57 ,0x1E03AC9C ,0x9EB76FAC ,0x45AF8E51 ,
                          0x30C81C46 ,0xA35CE411 ,0xE5FBC119 ,0x1A0A52EF ,
                          0xF69F2445 ,0xDF4F9B17 ,0xAD2B417B ,0xE66C3710};


/* Ciphertext */
uint32_t aEncryptedtextExpected[AES_TEXT_SIZE] =
                          {0x3AD77BB4 ,0x0D7A3660 ,0xA89ECAF3 ,0x2466EF97 ,
                           0xF5D3D585 ,0x03B9699D ,0xE785895A ,0x96FDBAAF ,
                           0x43B1CD7F ,0x598ECE23 ,0x881B00E3 ,0xED030688 ,
                           0x7B0C785E ,0x27E8AD3F ,0x82232071 ,0x04725DD4};

/* Used for storing the encrypted text */
uint32_t aEncryptedText[AES_TEXT_SIZE] = {0};

/* Used for storing the decrypted text */
uint32_t aDecryptedText[PLAINTEXT_SIZE] = {0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_GPDMA1_Init(void);
static void MX_ICACHE_Init(void);
static void MX_AES_Init(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
static void Display_PlainData(uint32_t datalength);
static void Display_EncryptedData(uint8_t mode, uint16_t keysize, uint32_t datalength);
static void Display_DecryptedData(uint8_t mode, uint16_t keysize, uint32_t datalength);
#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
extern void initialise_monitor_handles(void);
#endif 

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
#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
  initialise_monitor_handles();
  printf("Semihosting Test...\n\r"); 
#endif 

#if (USE_VCP_CONNECTION == 1)
  COM_InitTypeDef COM_Init;
#endif

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
  MX_GPDMA1_Init();
  MX_ICACHE_Init();
  MX_AES_Init();
  /* USER CODE BEGIN 2 */

  /* Configure LEDs */
  BSP_LED_Init(LD1);
  BSP_LED_Init(LD3);

#if (USE_VCP_CONNECTION == 1)
  /* Configure COM port */
  COM_Init.BaudRate   = 115200;
  COM_Init.WordLength = COM_WORDLENGTH_8B;
  COM_Init.StopBits   = COM_STOPBITS_1;
  COM_Init.Parity     = COM_PARITY_NONE;
  COM_Init.HwFlowCtl  = COM_HWCONTROL_NONE;
  if (BSP_COM_Init(COM1, &COM_Init) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  if (BSP_COM_SelectLogPort(COM1) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif

   /*#######################################################################*/
   /*                                                                       */
   /*##-         DMA-based AES 128 ECB encryption              #############*/
   /*                                                                       */
   /*#######################################################################*/
  /*##- Configure the CRYP peripheral ######################################*/
  if (HAL_CRYP_DeInit(&hcryp) != HAL_OK)
  {
    Error_Handler();
  }

  hcryp.Instance = AES;
  hcryp.Init.DataType      = CRYP_NO_SWAP;
  hcryp.Init.KeySize       = CRYP_KEYSIZE_128B;
  hcryp.Init.Algorithm     = CRYP_AES_ECB;
  hcryp.Init.pKey          = (uint32_t *)pKeyAES;
  hcryp.Init.DataWidthUnit = CRYP_DATAWIDTHUNIT_WORD;

  if (HAL_CRYP_Init(&hcryp) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Display Plain Data*/
  Display_PlainData(PLAINTEXT_SIZE);

  HAL_Delay(1);

  if (HAL_CRYP_Encrypt_DMA(&hcryp, aPlaintext, PLAINTEXT_SIZE, aEncryptedText) != HAL_OK)
  {
    /* Processing Error */
    Error_Handler();
  }

  /*  Before starting a new process, the current state of the peripheral is checked;
      as long as the state is not set back to READY, no new ciphering processing
      can be started.
      For simplicity's sake, this example is just waiting till the end of the
      process, but application may perform other tasks while ciphering operation
      is ongoing. */
  while (HAL_CRYP_GetState(&hcryp) != HAL_CRYP_STATE_READY)
  {
  }

  /* Display encrypted Data */
  Display_EncryptedData(ECB, 128, AES_TEXT_SIZE);

  /*##- Compare the encrypted text with the expected one #####################*/
  if(memcmp(aEncryptedText, aEncryptedtextExpected, PLAINTEXT_SIZE*4) != 0)
  {
    Error_Handler();
  }
  else
  {
    /* Correct encryption */
  }

   /*#######################################################################*/
   /*                                                                       */
   /*##-         DMA-based AES 128 ECB decryption              #############*/
   /*                                                                       */
   /*#######################################################################*/
  /* Deinitialize Crypto peripheral */
  HAL_CRYP_DeInit(&hcryp);


  if(HAL_CRYP_Init(&hcryp) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  if(HAL_CRYP_Decrypt_DMA(&hcryp, aEncryptedtextExpected, PLAINTEXT_SIZE, aDecryptedText) != HAL_OK)
  {
    /* Processing Error */
    Error_Handler();
  }

  /*  Before starting a new process, the current state of the peripheral is checked;
      as long as the state is not set back to READY, no new ciphering processing
      can be started.
      For simplicity's sake, this example is just waiting till the end of the
      process, but application may perform other tasks while cihering operation
      is ongoing. */
  while (HAL_CRYP_GetState(&hcryp) != HAL_CRYP_STATE_READY)
  {
  }

  /* Display decrypted Data */
  Display_DecryptedData(ECB, 128, PLAINTEXT_SIZE);

  /*##- Compare the decrypted text with the expected one #####################*/
  if(memcmp(aDecryptedText, aPlaintext, PLAINTEXT_SIZE*4) != 0)
  {
    Error_Handler();
  }
  else
  {
    /* Correct decryption */
  }

  HAL_Delay(100);
  printf("======================================================\n");
  printf("\n\r DMA-based AES 128 ECB encryption/decryption done.\n");
  printf("\n\r No issue detected.\n");
  HAL_Delay(100);

  /* Turn LD1 on */
  BSP_LED_On(LD1);
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
  hcryp.Init.KeyMode = CRYP_KEYMODE_NORMAL;
  if (HAL_CRYP_Init(&hcryp) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN AES_Init 2 */

  /* USER CODE END AES_Init 2 */

}

/**
  * @brief GPDMA1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPDMA1_Init(void)
{

  /* USER CODE BEGIN GPDMA1_Init 0 */

  /* USER CODE END GPDMA1_Init 0 */

  /* Peripheral clock enable */
  __HAL_RCC_GPDMA1_CLK_ENABLE();

  /* GPDMA1 interrupt Init */
    HAL_NVIC_SetPriority(GPDMA1_Channel0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel0_IRQn);
    HAL_NVIC_SetPriority(GPDMA1_Channel1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel1_IRQn);

  /* USER CODE BEGIN GPDMA1_Init 1 */

  /* USER CODE END GPDMA1_Init 1 */
  /* USER CODE BEGIN GPDMA1_Init 2 */

  /* USER CODE END GPDMA1_Init 2 */

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
  * @brief  Display Plain Data
  * @param  datalength: length of the data to display
  * @retval None
  */
static void Display_PlainData(uint32_t datalength)
{
  uint32_t BufferCounter = 0;
  uint32_t count = 0;
  uint8_t * ptr = (uint8_t *)aPlaintext;

  HAL_Delay(100);
  printf("\n\r =============================================================\n\r");
  printf(" ================== Crypt Using HW Cryp  =====================\n\r");
  printf(" =============================================================\n\r");
  printf(" ---------------------------------------\n\r");
  printf(" Plain Data :\n\r");
  printf(" ---------------------------------------\n\r");
  HAL_Delay(100);

  for (BufferCounter = 0; BufferCounter < datalength*4; BufferCounter++)
  {
    printf("[0x%02X]", *ptr++);
    count++;

    if (count == 16)
    {
      count = 0;
      printf(" Block %ld \n\r", BufferCounter / 16);
    }
  }
}

/**
  * @brief  Display Encrypted Data
  * @param  mode: chaining mode
  * @param  keysize: AES key size used
  * @param  datalength: length of the data to display
  * @retval None
  */
static void Display_EncryptedData(uint8_t mode, uint16_t keysize, uint32_t datalength)
{
  uint32_t BufferCounter = 0;
  uint32_t count = 0;
  uint8_t * ptr = (uint8_t *)aEncryptedText;

  printf("\n\r =======================================\n\r");
  printf(" Encrypted Data with AES %d  Mode  ", keysize);

  if (mode == ECB)
  {
    printf("ECB\n\r");
  }
  else if (mode == CBC)
  {
    printf("CBC\n\r");
  }
  else /* if(mode == CTR)*/
  {
    printf("CTR\n\r");
  }

  printf(" ---------------------------------------\n\r");

  for (BufferCounter = 0; BufferCounter < datalength*4; BufferCounter++)
  {
    printf("[0x%02X]", *ptr++);

    count++;
    if (count == 16)
    {
      count = 0;
      printf(" Block %ld \n\r", BufferCounter / 16);
    }
  }
}

/**
  * @brief  Display Decrypted Data
  * @param  mode: chaining mode
  * @param  keysize: AES key size used
  * @param  datalength: length of the data to display
  * @retval None
  */
static void Display_DecryptedData(uint8_t mode, uint16_t keysize, uint32_t datalength)
{
  uint32_t BufferCounter = 0;
  uint32_t count = 0;
  uint8_t * ptr = (uint8_t *)aDecryptedText;

  printf("\n\r =======================================\n\r");
  printf(" Decrypted Data with AES %d  Mode  ", keysize);

  if (mode == ECB)
  {
    printf("ECB\n\r");
  }
  else if (mode == CBC)
  {
    printf("CBC\n\r");
  }
  else /* if(mode == CTR)*/
  {
    printf("CTR\n\r");
  }

  printf(" ---------------------------------------\n\r");

  for (BufferCounter = 0; BufferCounter < datalength*4; BufferCounter++)
  {
    printf("[0x%02X]", *ptr++);
    count++;

    if (count == 16)
    {
      count = 0;
      printf(" Block %ld \n\r", BufferCounter / 16);
    }
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
  printf("\n\r Error Detected...\n ");

  while(1)
  {
    /* Toggle LD3 */
    BSP_LED_Toggle(LD3);
    HAL_Delay(200);
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
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
