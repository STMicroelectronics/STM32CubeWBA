/**
  ******************************************************************************
  * @file    s_data.c
  * @author  MCD Application Team
  * @brief   S data module.
  *          This file provides examples of S Data access.
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
#include "appli_region_defs.h"
#include "s_data.h"
#include "com.h"


#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)

/** @defgroup  S_Data_Private_Defines Private Defines
  * @{
  */

/**
  * @}
  */

/** @defgroup  S_Data_Private_Functions Private Functions
  * @{
  */
static void S_DATA_Print_menu(void);
static void S_DATA_Display(void);

/**
  * @}
  */

/** @defgroup  S_Data_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief  Display the S Data Main Menu choices on HyperTerminal
  * @param  None.
  * @retval None.
  */
void S_DATA_Run(void)
{
  uint8_t key = 0;
  uint8_t exit = 0;

  /* Print S Data welcome message */
  S_DATA_Print_menu();

  while (exit == 0U)
  {
    key = 0U;

    /* Clean the user input path */
    COM_Flush();

    /* Receive key */
    if (COM_Receive(&key, 1U, COM_UART_TIMEOUT_MAX) == HAL_OK)
    {
      switch (key)
      {
        case '1' :
          S_DATA_Display();
          break;
        case 'x':
          exit = 1;
          break;
        default:
          printf("Invalid Number !\r");
          break;
      }
      S_DATA_Print_menu();
    }
  }
}
/**
  * @}
  */

/** @addtogroup  S_Data_Private_Functions
  * @{
  */

/**
  * @brief  Display the S Data Menu choices on HyperTerminal
  * @param  None.
  * @retval None.
  */
static void S_DATA_Print_menu(void)
{
  printf("\r\n=============== S Data Image Menu ===================\r\n\n");
  printf("  Display Data from S Data Image ------------------------ 1\r\n\n");
  printf("  Previous Menu ----------------------------------------- x\r\n\n");
}

/**
  * @brief  Display the S Data on HyperTerminal
  * @param  None.
  * @retval None.
  */
static void S_DATA_Display(void)
{
  uint8_t *data1;
  data1 = (uint8_t*)(_FLASH_BASE_S + S_DATA_IMAGE_PRIMARY_PARTITION_OFFSET + S_DATA_IMAGE_IAT_PRIV_KEY_OFFSET);

  printf("  -- S Data: %08lx%08lx..%08lx%08lx\r\n\n",
               *((unsigned long *)(&data1[0])),
               *((unsigned long *)(&data1[4])),
               *((unsigned long *)(&data1[S_DATA_IMAGE_IAT_PRIV_KEY_OFFSET - 8])),
               *((unsigned long *)(&data1[S_DATA_IMAGE_IAT_PRIV_KEY_OFFSET - 4]))
              );
}

/**
  * @}
  */

#endif /* (MCUBOOT_S_DATA_IMAGE_NUMBER == 1) */
