/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_filex.c
  * @author  MCD Application Team
  * @brief   FileX applicative file
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
#include "app_filex.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#if defined(__ICCARM__)
#include <LowLevelIOInterface.h>
#endif /* __ICCARM__ */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEFAULT_SECTOR_SIZE     512
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Define FileX global data structures.  */
/* FileX SRAM disk media instance */
FX_MEDIA        sram_disk;
/* FileX file instance */
FX_FILE         fx_file;

uint32_t media_memory[DEFAULT_SECTOR_SIZE / sizeof(uint32_t)];

extern UART_HandleTypeDef huart1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */
void Error_Handler(void);

#if defined(__ICCARM__)
/* New definition from EWARM V9, compatible with EWARM8 */
int iar_fputc(int ch);
#define PUTCHAR_PROTOTYPE int iar_fputc(int ch)
#elif defined ( __clang__ ) || defined(__ARMCC_VERSION)
/* ARM Compiler 5/6*/
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#elif defined(__GNUC__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#endif /* __ICCARM__ */

/* USER CODE END PFP */
/**
  * @brief  Application FileX Initialization.
  * @param  None
  * @retval int
  */
UINT MX_FileX_Init(void)
{
  UINT ret = FX_SUCCESS;
  /* USER CODE BEGIN MX_FileX_Init */

  /* USER CODE END MX_FileX_Init */

  /* Initialize FileX.  */
  fx_system_initialize();

  /* USER CODE BEGIN MX_FileX_Init 1*/

  /* USER CODE END MX_FileX_Init 1*/

  return ret;
}

/* USER CODE BEGIN 1 */

VOID MX_FileX_Process(VOID)
{
  UINT status;
  ULONG bytes_read;
  CHAR read_buffer[32];
  CHAR data[] = "This is FileX working on STM32";

  /* Start application */
  printf("FileX SRAM Standalone Application Start.\n");

  /* Format the SRAM2_BASE memory as FAT */
  status =  fx_media_format(&sram_disk,                             // RamDisk pointer
                            fx_stm32_sram_driver,                   // Driver entry
                            (VOID *)FX_NULL,                        // Device info pointer
                            (UCHAR *) media_memory,                 // Media buffer pointer
                            512,                                    // Media buffer size
                            "STM32_SRAM_DISK",                      // Volume Name
                            1,                                      // Number of FATs
                            32,                                     // Directory Entries
                            0,                                      // Hidden sectors
                            FX_SRAM_DISK_SIZE / DEFAULT_SECTOR_SIZE,// Total sectors
                            DEFAULT_SECTOR_SIZE,                    // Sector size
                            8,                                      // Sectors per cluster
                            1,                                      // Heads
                            1);                                     // Sectors per track

  /* Check the format status */
  if (status != FX_SUCCESS)
  {
    Error_Handler();
  }

  /* Open the sram_disk driver. */
  status =  fx_media_open(&sram_disk, "STM32_SRAM_DISK", fx_stm32_sram_driver, (VOID *)FX_NULL, (VOID *) media_memory, sizeof(media_memory));

  /* Check the media open status. */
  if (status != FX_SUCCESS)
  {
    Error_Handler();
  }

  printf("SRAM Disk successfully formatted and opened.\n");

  /* Create a file called STM32.TXT in the root directory. */
  status =  fx_file_create(&sram_disk, "STM32.TXT");

  /* Check the create status. */
  if (status != FX_SUCCESS)
  {
    /* Check for an already created status. This is expected on the
    second pass of this loop!  */
    if (status != FX_ALREADY_CREATED)
    {
      /* Create error, call error handler. */
      Error_Handler();
    }
  }

  /* Open the test file.  */
  status =  fx_file_open(&sram_disk, &fx_file, "STM32.TXT", FX_OPEN_FOR_WRITE);

  /* Check the file open status. */
  if (status != FX_SUCCESS)
  {
    /* Error opening file, call error handler. */
    Error_Handler();
  }

  /* Seek to the beginning of the test file. */
  status =  fx_file_seek(&fx_file, 0);

  /* Check the file seek status.  */
  if (status != FX_SUCCESS)
  {
    /* Error performing file seek, call error handler. */
    Error_Handler();
  }

  printf("Writing data into the file. \n");

  /* Write a string to the test file. */
  status =  fx_file_write(&fx_file, data, sizeof(data));

  /* Check the file write status. */
  if (status != FX_SUCCESS)
  {
    /* Error writing to a file, call error handler. */
    Error_Handler();
  }

  /* Close the test file. */
  status =  fx_file_close(&fx_file);

  /* Check the file close status. */
  if (status != FX_SUCCESS)
  {
    /* Error closing the file, call error handler. */
    Error_Handler();
  }

  status = fx_media_flush(&sram_disk);

  /* Check the media flush  status. */
  if (status != FX_SUCCESS)
  {
    /* Error closing the file, call error handler. */
    Error_Handler();
  }

  /* Open the test file. */
  status =  fx_file_open(&sram_disk, &fx_file, "STM32.TXT", FX_OPEN_FOR_READ);

  /* Check the file open status. */
  if (status != FX_SUCCESS)
  {
    /* Error opening file, call error handler. */
    Error_Handler();
  }

  /* Seek to the beginning of the test file. */
  status =  fx_file_seek(&fx_file, 0);

  /* Check the file seek status. */
  if (status != FX_SUCCESS)
  {
    /* Error performing file seek, call error handler. */
    Error_Handler();
  }

  /* Read the first 28 bytes of the test file. */
  status =  fx_file_read(&fx_file, read_buffer, sizeof(data), &bytes_read);

  /* Check the file read status.  */
  if ((status != FX_SUCCESS) || (bytes_read != sizeof(data)))
  {
    /* Error reading file, call error handler. */
    Error_Handler();
  }

  /* Close the test file. */
  status =  fx_file_close(&fx_file);

  /* Check the file close status. */
  if (status != FX_SUCCESS)
  {
    /* Error closing the file, call error handler. */
    Error_Handler();
  }

  /* Close the media. */
  status =  fx_media_close(&sram_disk);

  /* Check the media close status. */
  if (status != FX_SUCCESS)
  {
    /* Error closing the media, call error handler. */
    Error_Handler();
  }

  printf("Data successfully written.\n");

  /* Infinite loop */
  while (1)
  {
    HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    HAL_Delay(500);
  }
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
#if defined(__ICCARM__)
size_t __write(int file, unsigned char const *ptr, size_t len)
{
  size_t idx;
  unsigned char const *pdata = ptr;

  for (idx = 0; idx < len; idx++)
  {
    iar_fputc((int)*pdata);
    pdata++;
  }
  return len;
}
#endif /* __ICCARM__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of putchar here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}


/* USER CODE END 1 */
