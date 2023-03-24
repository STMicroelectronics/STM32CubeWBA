/**
  ******************************************************************************
  * @file    optionbytes_interface.c
  * @author  MCD Application Team
  * @brief   Contains Option Bytes access functions
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

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "common_interface.h"

#include "openbl_mem.h"

#include "app_openbootloader.h"
#include "optionbytes_interface.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
OPENBL_MemoryTypeDef OB1_Descriptor =
{
  OB1_START_ADDRESS,
  OB1_END_ADDRESS,
  OB1_SIZE,
  OB_AREA,
  OPENBL_OB_Read,
  OPENBL_OB_Write,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Launch the option byte loading.
  * @retval None.
  */
void OPENBL_OB_Launch(void)
{
  /* Set the option start bit */
  HAL_FLASH_OB_Launch();
}

/**
  * @brief  This function is used to read data from a given address.
  * @param  Address The address to be read.
  * @retval Returns the read value.
  */
uint8_t OPENBL_OB_Read(uint32_t Address)
{
  return (*(uint8_t *)(Address));
}

/**
  * @brief  This function is used to write data in Option bytes.
  * @param  Address The address where that data will be written.
  * @param  pData The data to be written.
  * @param  DataLength The length of the data to be written.
  * @retval None.
  */
void OPENBL_OB_Write(uint32_t Address, uint8_t *pData, uint32_t DataLength)
{
  /* Unlock the FLASH & Option Bytes Registers access */
  HAL_FLASH_Unlock();
  HAL_FLASH_OB_Unlock();

  /* Clear error programming flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  if (Address == OB1_START_ADDRESS)
  {
    /* Write OPTR */
    if (DataLength >= 4U)
    {
      WRITE_REG(FLASH->OPTR, (pData[0]
                              | (pData[1] << 8U)
                              | (pData[2] << 16U)
                              | (pData[3] << 24U)));
    }

    /* Write NSBOOTADD0R */
    if (DataLength >= 8U)
    {
      WRITE_REG(FLASH->NSBOOTADD0R, (pData[4]
                                     | (pData[5] << 8U)
                                     | (pData[6] << 16U)
                                     | (pData[7] << 24U)));
    }

    /* Write NSBOOTADD1R */
    if (DataLength >= 12U)
    {
      WRITE_REG(FLASH->NSBOOTADD1R, (pData[8]
                                     | (pData[9] << 8U)
                                     | (pData[10] << 16U)
                                     | (pData[11] << 24U)));
    }

    /* Write SECBOOTADD0R */
    if (DataLength >= 16U)
    {
      WRITE_REG(FLASH->SECBOOTADD0R, (pData[12]
                                      | (pData[13] << 8U)
                                      | (pData[14] << 16U)
                                      | (pData[15] << 24U)));
    }

    /* Write SECWMR1 */
    if (DataLength >= 20U)
    {
      WRITE_REG(FLASH->SECWMR1, (pData[16]
                                 | (pData[17] << 8U)
                                 | (pData[18] << 16U)
                                 | (pData[19] << 24U)));
    }

    /* Write SECWMR2 */
    if (DataLength >= 24U)
    {
      WRITE_REG(FLASH->SECWMR2, (pData[20]
                                 | (pData[21] << 8U)
                                 | (pData[22] << 16U)
                                 | (pData[23] << 24U)));
    }

    /* Write WRPAR */
    if (DataLength >= 28U)
    {
      WRITE_REG(FLASH->WRPAR, (pData[24]
                               | (pData[25] << 8U)
                               | (pData[26] << 16U)
                               | (pData[27] << 24U)));
    }

    /* Write WRPBR */
    if (DataLength >= 32U)
    {
      WRITE_REG(FLASH->WRPBR, (pData[28]
                               | (pData[29] << 8U)
                               | (pData[30] << 16U)
                               | (pData[31] << 24U)));
    }

    /* Write OEM1KEYR1 */
    if (DataLength >= 52U)
    {
      WRITE_REG(FLASH->OEM1KEYR1, (pData[32]
                                   | (pData[33] << 8U)
                                   | (pData[34] << 16U)
                                   | (pData[35] << 24U)));
    }

    /* Write OEM1KEYR2 */
    if (DataLength >= 56U)
    {
      WRITE_REG(FLASH->OEM1KEYR2, (pData[36]
                                   | (pData[37] << 8U)
                                   | (pData[38] << 16U)
                                   | (pData[39] << 24U)));
    }

    /* Write OEM2KEYR1 */
    if (DataLength >= 60U)
    {
      WRITE_REG(FLASH->OEM2KEYR1, (pData[40]
                                   | (pData[41] << 8U)
                                   | (pData[42] << 16U)
                                   | (pData[43] << 24U)));
    }

    /* Write OEM2KEYR2 */
    if (DataLength >= 64U)
    {
      WRITE_REG(FLASH->OEM2KEYR2, (pData[44]
                                   | (pData[45] << 8U)
                                   | (pData[46] << 16U)
                                   | (pData[47] << 24U)));
    }
  }

  SET_BIT(FLASH->NSCR1, FLASH_NSCR1_OPTSTRT);

  /* Wait for last operation to be completed */
  FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);

  /* Register system reset callback */
  Common_SetPostProcessingCallback(OPENBL_OB_Launch);
}
