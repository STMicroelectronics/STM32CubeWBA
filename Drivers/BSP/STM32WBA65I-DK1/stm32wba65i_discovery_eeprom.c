/**
******************************************************************************
* @file    stm32wba65i_discovery_eeprom.c
* @author  SRA-SAIL
* @brief   This file provides STM32WBA65I-DK1 BSP
*          specific functions
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
/* Includes ------------------------------------------------------------------*/
#include "stm32wba65i_discovery_eeprom.h"

/** @addtogroup BSP
* @{
*/

/* -------- Static functions prototypes --------------------------------------------- */
static M95P32_CommonDrv_t *M95P32Drv[BSP_EEPROM_INSTANCES_NBR];
void *M95P32CompObj[BSP_EEPROM_INSTANCES_NBR];
static int32_t M95P32_Probe(void);

/**
  * @brief  This function gives high on selected control pin
  * @param  None
  * @retval None
  */
void EEPROMEX_CTRL_HIGH(void)
{
  HAL_GPIO_WritePin(M95P32_EEPROM_SPI_CS_PORT,M95P32_EEPROM_SPI_CS_PIN,GPIO_PIN_SET );
}

/**
  * @brief  This function gives low on selected control pin
  * @param  None
  * @retval None
  */
void EEPROMEX_CTRL_LOW(void)
{
  HAL_GPIO_WritePin(M95P32_EEPROM_SPI_CS_PORT,M95P32_EEPROM_SPI_CS_PIN,GPIO_PIN_RESET );
}

/**
  * @brief  Initializes the SPI EEPROMs
  * @param  Instance : SPI EEPROMs instance to be used
  * @retval BSP status
  */
int32_t BSP_EEPROM_Init(uint32_t Instance)
{
  int32_t ret = M95_OK;

  switch (Instance)
  {
  case BSP_EEPROM_0:
    if (M95P32_Probe() != M95_OK)
    {
      return M95_ERROR;
    }
    break;
  default:
    ret = M95_ERROR;
    break;
  }

  return ret;
}

/**
 * @brief  Function pointer to WRITE_ENABLE function in M95P32 driver structure
 * @param  Instance : SPI EEPROMs instance to be used
 * @retval BSP status
 */
int32_t BSP_EEPROM_WriteEnable(uint32_t Instance)
{
  int32_t ret = M95_OK;

  EEPROMEX_CTRL_LOW();

  ret = (M95P32Drv[Instance]->WriteEnable(M95P32CompObj[Instance]));

  EEPROMEX_CTRL_HIGH();

  return ret;
}

/**
 * @brief  Function pointer to WRITE_DISABLE function in M95P32 driver structure
 * @param  Instance : SPI EEPROMs instance to be used
 * @retval BSP status
 */
int32_t BSP_EEPROM_WriteDisable(uint32_t Instance)
{
  int32_t ret = M95_OK;

  EEPROMEX_CTRL_LOW();

  ret = (M95P32Drv[Instance]->WriteDisable(M95P32CompObj[Instance]));

  EEPROMEX_CTRL_HIGH();

  return ret;
}

/**
  * @brief  Function pointer to Read_StatusReg function in M95P32 driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  pData : pointer to store data read
  * @retval BSP status
  */
int32_t BSP_EEPROM_StatusRegRead(uint32_t Instance, uint8_t * pData)
{
  int32_t ret = M95_OK;

  EEPROMEX_CTRL_LOW();

  ret = (M95P32Drv[Instance]->StatusRegRead(M95P32CompObj[Instance], pData));

  EEPROMEX_CTRL_HIGH();

  return ret;

}

/**
  * @brief  Function pointer to Write_StatusConfigReg function in M95P32 driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  pData : pointer to the data to write
  * @param  Size : number of bytes to write
  * @retval BSP status
  */
int32_t BSP_EEPROM_StatusConfigRegWrite(uint32_t Instance, uint8_t * pData, uint16_t Size)
{
  int32_t ret = M95_OK;

  EEPROMEX_CTRL_LOW();

  ret = (M95P32Drv[Instance]->StatusConfigRegWrite(M95P32CompObj[Instance], pData, Size));

  EEPROMEX_CTRL_HIGH();
  HAL_Delay(5);

  EEPROMEX_CTRL_LOW();
  ret = Transmit_Data_polling(M95P32CompObj[Instance]);
  EEPROMEX_CTRL_HIGH();
  return ret;
}

/**
  * @brief  Function pointer to Single_Read function in M95P32 driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  pData : pointer to store data read
  * @param  TarAddr : starting address to read
  * @param  Size : number of bytes to read
  * @retval BSP status
  */
int32_t BSP_EEPROM_ReadBuffer(uint32_t Instance, uint8_t * pData, uint32_t TarAddr, uint16_t Size)
{
  int32_t ret = M95_OK;

  EEPROMEX_CTRL_LOW();

  ret = (M95P32Drv[Instance]->ReadPage(M95P32CompObj[Instance], pData, TarAddr, Size));

  EEPROMEX_CTRL_HIGH();

  return ret;
}

/**
  * @brief  Function pointer to FAST_Read function in M95P32 driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  pData : pointer to store data read
  * @param  TarAddr : starting address to read
  * @param  Size : number of bytes to read
  * @retval BSP status
  */
int32_t BSP_EEPROM_FastRead(uint32_t Instance, uint8_t *pData, uint32_t TarAddr,uint16_t Size)
{
  int32_t ret = M95_OK;

  EEPROMEX_CTRL_LOW();

  ret = (M95P32Drv[Instance]->FastRead(M95P32CompObj[Instance], pData, TarAddr, Size));

  EEPROMEX_CTRL_HIGH();

  return ret;
}


/**
  * @brief  Function pointer to Page_Write function in M95P32 driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  pData : pointer to the data to write
  * @param  TarAddr : starting address to write
  * @param  Size : number of bytes to write
  * @retval BSP status
  */
int32_t BSP_EEPROM_WriteBuffer(uint32_t Instance, uint8_t * pData, uint32_t TarAddr, uint16_t Size)
{
  int32_t ret = M95_OK;
  uint32_t remainingSize = Size;
  uint32_t targetAddress = TarAddr;
  uint8_t statusReg = 0;
  uint32_t bytesToWrite;

  /* Calculate the starting page and offset */
  uint32_t startOffset = TarAddr % M95P32_PAGESIZE;
  uint32_t offset = startOffset;


  /* Check for invalid inputs */
  if((M95P32CompObj[Instance] == NULL) || (pData == NULL) || (M95P32_PAGESIZE == 0U) || (remainingSize == 0U))
  {
    return M95_ERROR;
  }

  /* Check WIP status bit*/
  EEPROMEX_CTRL_LOW();

  do
  {
    ret = (M95P32Drv[Instance]->StatusRegRead(M95P32CompObj[Instance], &statusReg));
  }while((statusReg & 0x01U) != 0U);

  EEPROMEX_CTRL_HIGH();

  /* Iterate over the pages and write the data */
  while(remainingSize > 0U)
  {
    bytesToWrite = (remainingSize < (M95P32_PAGESIZE - offset)) ? remainingSize : (M95P32_PAGESIZE - offset);

    EEPROMEX_CTRL_LOW();

    ret = (M95P32Drv[Instance]->WriteEnable(M95P32CompObj[Instance]));

    EEPROMEX_CTRL_HIGH();
    HAL_Delay(10);
    EEPROMEX_CTRL_LOW();

    ret = (M95P32Drv[Instance]->WritePage(M95P32CompObj[Instance], pData, targetAddress, bytesToWrite));

    EEPROMEX_CTRL_HIGH();
    HAL_Delay(20);

    EEPROMEX_CTRL_LOW();
    ret = Transmit_Data_polling(M95P32CompObj[Instance]);
    EEPROMEX_CTRL_HIGH();

    if(ret == M95_OK)
    {
      /* Update the pointers and sizes for the next page */
      pData += bytesToWrite;
      remainingSize -= bytesToWrite;
      targetAddress += bytesToWrite;
      offset = targetAddress % M95P32_PAGESIZE;

      /* Check WIP status bit*/
      EEPROMEX_CTRL_LOW();

      do
      {
        ret = (M95P32Drv[Instance]->StatusRegRead(M95P32CompObj[Instance], &statusReg));
      }while((statusReg & 0x01U) != 0U);

      EEPROMEX_CTRL_HIGH();
    }
    else
    {
      ret = M95_ERROR;
      break;
    }

  }

  return ret;
}

/**
  * @brief  Function pointer to Page_Prog function in M95P32 driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  pData : pointer to the data to write
  * @param  TarAddr : starting address to write
  * @param  Size : number of bytes to write
  * @retval BSP status
  */
int32_t BSP_EEPROM_ProgramPage(uint32_t Instance, uint8_t * pData, uint32_t TarAddr, uint16_t Size)
{
  int32_t ret = M95_OK;
  uint32_t remainingSize = Size;
  uint32_t targetAddress = TarAddr;
  uint8_t statusReg = 0;
  uint32_t bytesToWrite;

  /* Calculate the starting page and offset */
  uint32_t startOffset = TarAddr % M95P32_PAGESIZE;
  uint32_t offset = startOffset;


  /* Check for invalid inputs */
  if((M95P32CompObj[Instance] == NULL) || (pData == NULL) || (M95P32_PAGESIZE == 0U) || (remainingSize == 0U))
  {
    return M95_ERROR;
  }

  /* Check WIP status bit*/
  EEPROMEX_CTRL_LOW();

  do
  {
    ret = (M95P32Drv[Instance]->StatusRegRead(M95P32CompObj[Instance], &statusReg));
  }while((statusReg & 0x01U) != 0U);

  EEPROMEX_CTRL_HIGH();

  /* Iterate over the pages and write the data */
  while(remainingSize > 0U)
  {
    bytesToWrite = (remainingSize < (M95P32_PAGESIZE - offset)) ? remainingSize : (M95P32_PAGESIZE - offset);

    EEPROMEX_CTRL_LOW();

    ret = (M95P32Drv[Instance]->WriteEnable(M95P32CompObj[Instance]));

    EEPROMEX_CTRL_HIGH();

    EEPROMEX_CTRL_LOW();

    ret = (M95P32Drv[Instance]->ProgramPage(M95P32CompObj[Instance], pData, targetAddress, bytesToWrite));

    EEPROMEX_CTRL_HIGH();
    HAL_Delay(5);

    EEPROMEX_CTRL_LOW();
    ret = Transmit_Data_polling(M95P32CompObj[Instance]);
    EEPROMEX_CTRL_HIGH();

    if(ret == M95_OK)
    {
      /* Update the pointers and sizes for the next page */
      pData += bytesToWrite;
      remainingSize -= bytesToWrite;
      targetAddress += bytesToWrite;
      offset = targetAddress % M95P32_PAGESIZE;

      /* Check WIP status bit*/
      EEPROMEX_CTRL_LOW();

      do
      {
        ret = (M95P32Drv[Instance]->StatusRegRead(M95P32CompObj[Instance], &statusReg));
      }while((statusReg & 0x01U) != 0U);

      EEPROMEX_CTRL_HIGH();
    }
    else
    {
      ret = M95_ERROR;
      break;
    }

  }

  return ret;
}

/**
  * @brief  Function pointer to Page_Erase function in M95P32 driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  TarAddr : starting address of page
  * @retval BSP status
  */
int32_t BSP_EEPROM_ErasePage(uint32_t Instance, uint32_t TarAddr)
{
  int32_t ret = M95_OK;

  EEPROMEX_CTRL_LOW();

  ret = (M95P32Drv[Instance]->ErasePage(M95P32CompObj[Instance], TarAddr));

  EEPROMEX_CTRL_HIGH();
  HAL_Delay(5);

  EEPROMEX_CTRL_LOW();
  ret = Transmit_Data_polling(M95P32CompObj[Instance]);
  EEPROMEX_CTRL_HIGH();

  return ret;
}

/**
  * @brief  Function pointer to Sector_Erase function in M95P32 driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  TarAddr : starting address of sector
  * @retval BSP status
  */
int32_t BSP_EEPROM_EraseSector(uint32_t Instance, uint32_t TarAddr)
{
  int32_t ret = M95_OK;

  EEPROMEX_CTRL_LOW();

  ret = (M95P32Drv[Instance]->EraseSector(M95P32CompObj[Instance], TarAddr));

  EEPROMEX_CTRL_HIGH();
  HAL_Delay(5);

  EEPROMEX_CTRL_LOW();
  ret = Transmit_Data_polling(M95P32CompObj[Instance]);
  EEPROMEX_CTRL_HIGH();

  return ret;
}

/**
  * @brief  Function pointer to Block_Erase function in M95P32 driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  TarAddr : starting address of block
  * @retval BSP status
  */
int32_t BSP_EEPROM_EraseBlock(uint32_t Instance, uint32_t TarAddr)
{
  int32_t ret = M95_OK;
  EEPROMEX_CTRL_LOW();
  ret = (M95P32Drv[Instance]->EraseBlock(M95P32CompObj[Instance], TarAddr));

  EEPROMEX_CTRL_HIGH();
  HAL_Delay(5);

  EEPROMEX_CTRL_LOW();
  ret = Transmit_Data_polling(M95P32CompObj[Instance]);
  EEPROMEX_CTRL_HIGH();

  return ret;
}

/**
  * @brief  Function pointer to Chip_Erase function in M95P32 driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @retval BSP status
  */
int32_t BSP_EEPROM_EraseChip(uint32_t Instance)
{
  int32_t ret = M95_OK;

  EEPROMEX_CTRL_LOW();

  ret = (M95P32Drv[Instance]->EraseChip(M95P32CompObj[Instance]));

  EEPROMEX_CTRL_HIGH();
  HAL_Delay(5);

  EEPROMEX_CTRL_LOW();
  ret = Transmit_Data_polling(M95P32CompObj[Instance]);
  EEPROMEX_CTRL_HIGH();

  return ret;
}

/**
  * @brief  Function pointer to Read_ID function in M95P32 driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  pData : pointer to store data read
  * @param  TarAddr : starting address to read
  * @param  Size : number of bytes to read
  * @retval BSP status
  */
int32_t BSP_EEPROM_ReadID(uint32_t Instance, uint8_t *pData, uint32_t TarAddr, uint16_t Size)
{
  int32_t ret = M95_OK;

  EEPROMEX_CTRL_LOW();

  ret = (M95P32Drv[Instance]->ReadID(M95P32CompObj[Instance], pData, TarAddr, Size));

  EEPROMEX_CTRL_HIGH();

  return ret;
}

/**
  * @brief  Function pointer to FAST_Read_ID function in M95P32 driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  pData : pointer to store data read
  * @param  TarAddr : starting address to read
  * @param  Size : Number of bytes to read
  * @retval BSP status
  */
int32_t BSP_EEPROM_FastReadID(uint32_t Instance, uint8_t *pData, uint32_t TarAddr, uint16_t Size)
{
  int32_t ret = M95_OK;

  EEPROMEX_CTRL_LOW();

  ret = (M95P32Drv[Instance]->FastReadID(M95P32CompObj[Instance], pData, TarAddr, Size));

  EEPROMEX_CTRL_HIGH();

  return ret;

}

/**
  * @brief  Function pointer to Write_ID function in M95P32 driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  pData : pointer to the data to write
  * @param  TarAddr : starting address to write
  * @param  Size : number of bytes to write
  * @retval BSP status
  */
int32_t BSP_EEPROM_WriteID(uint32_t Instance, uint8_t *pData, uint32_t TarAddr, uint16_t Size)
{
  int32_t ret = M95_OK;

  EEPROMEX_CTRL_LOW();

  ret = (M95P32Drv[Instance]->WriteID(M95P32CompObj[Instance], pData, TarAddr, Size));

  EEPROMEX_CTRL_HIGH();
  HAL_Delay(5);

  EEPROMEX_CTRL_LOW();
  ret = Transmit_Data_polling(M95P32CompObj[Instance]);
  EEPROMEX_CTRL_HIGH();

  return ret;

}

/**
 * @brief  Function pointer to Deep_Power_Down function in M95P32 driver structure
 * @param  Instance : SPI EEPROMs instance to be used
 * @retval BSP status
 */
int32_t BSP_EEPROM_DeepPowerDown(uint32_t Instance)
{
  int32_t ret = M95_OK;

  EEPROMEX_CTRL_LOW();

  ret = (M95P32Drv[Instance]->DeepPowerDown(M95P32CompObj[Instance]));

  EEPROMEX_CTRL_HIGH();

  return ret;

}

/**
 * @brief  Function pointer to Deep_Power_Down_Release function in M95P32 driver structure
 * @param  Instance : SPI EEPROMs instance to be used
 * @retval BSP status
 */
int32_t BSP_EEPROM_DeepPowerDownRel(uint32_t Instance)
{
  int32_t ret = M95_OK;

  EEPROMEX_CTRL_LOW();

  ret = (M95P32Drv[Instance]->DeepPowerDownRel(M95P32CompObj[Instance]));

  EEPROMEX_CTRL_HIGH();

  return ret;

}

/**
 * @brief  Function pointer to Read_JEDEC function in M95P32 driver structure
 * @param  Instance : SPI EEPROMs instance to be used
 * @param  pData : pointer to store data read
 * @param  Size : number of bytes to read
 * @retval BSP : status
 */
int32_t BSP_EEPROM_JEDECRead(uint32_t Instance, uint8_t *pData, uint16_t Size)
{
  int32_t ret = M95_OK;

  EEPROMEX_CTRL_LOW();

  ret = (M95P32Drv[Instance]->JEDECRead(M95P32CompObj[Instance], pData, Size));

  EEPROMEX_CTRL_HIGH();

  return ret;

}

/**
  * @brief  Function pointer to ReadConfigReg function in M95P32 driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  pData : pointer to store data read
  * @param  Size : number of bytes to read
  * @retval BSP status
  */
int32_t BSP_EEPROM_ConfSafetyRegRead(uint32_t Instance, uint8_t *pData, uint16_t Size)
{
  int32_t ret = M95_OK;

  EEPROMEX_CTRL_LOW();

  ret = (M95P32Drv[Instance]->ConfSafetyRegRead(M95P32CompObj[Instance], pData, Size));

  EEPROMEX_CTRL_HIGH();

  return ret;

}

/**
 * @brief  Function pointer to ReadVolatileReg function in M95P32 driver structure
 * @param  Instance : SPI EEPROMs instance to be used
 * @param  pData : pointer to store data read
 * @retval BSP status
 */
int32_t BSP_EEPROM_VolRegRead(uint32_t Instance, uint8_t *pData)
{
  int32_t ret = M95_OK;

  EEPROMEX_CTRL_LOW();

  ret = (M95P32Drv[Instance]->VolRegRead(M95P32CompObj[Instance], pData));

  EEPROMEX_CTRL_HIGH();

  return ret;
}

/**
 * @brief  Function pointer to WriteVolatileRegister function in M95P32 driver structure
 * @param  Instance : SPI EEPROMs instance to be used
 * @param  regVal : register value to write
 * @retval BSP status
 */
int32_t BSP_EEPROM_VolRegWrite(uint32_t Instance, uint8_t regVal)
{
  int32_t ret = M95_OK;

  EEPROMEX_CTRL_LOW();

  ret = (M95P32Drv[Instance]->VolRegWrite(M95P32CompObj[Instance], regVal));

  EEPROMEX_CTRL_HIGH();
  HAL_Delay(5);

  EEPROMEX_CTRL_LOW();
  ret = Transmit_Data_polling(M95P32CompObj[Instance]);
  EEPROMEX_CTRL_HIGH();

  return ret;
}

/**
 * @brief  Function pointer to ClearSafetyFlag function in M95P32 driver structure
 * @param  Instance : SPI EEPROMs instance to be used
 * @retval BSP status
 */
int32_t BSP_EEPROM_ClearSafetyFlag(uint32_t Instance)
{

  int32_t ret = M95_OK;

  EEPROMEX_CTRL_LOW();

  ret = (M95P32Drv[Instance]->ClearSafetyFlag(M95P32CompObj[Instance]));

  EEPROMEX_CTRL_HIGH();

  return ret;

}

/**
  * @brief  Function pointer to Read_StatusReg function in M95P32 driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  pData : pointer to store data read
  * @param  TarAddr : starting address to read
  * @param  Size : number of bytes to read
  * @retval BSP status
  */
int32_t BSP_EEPROM_SFDPRead(uint32_t Instance, uint8_t *pData, uint32_t TarAddr, uint16_t Size)
{
  int32_t ret = M95_OK;

  EEPROMEX_CTRL_LOW();

  ret = (M95P32Drv[Instance]->SFDPRegRead(M95P32CompObj[Instance], pData, TarAddr, Size));

  EEPROMEX_CTRL_HIGH();

  return ret;

}

/**
  * @brief  Function pointer to Reset_Enable function in M95P32 driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @retval BSP status
  */
int32_t BSP_EEPROM_EnableReset(uint32_t Instance)
{
  int32_t ret = M95_OK;

  EEPROMEX_CTRL_LOW();

  ret = (M95P32Drv[Instance]->EnableReset(M95P32CompObj[Instance]));

  EEPROMEX_CTRL_HIGH();

  return ret;


}

/**
  * @brief  Function pointer to Soft_Reset function in M95P32 driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @retval BSP status
  */
int32_t BSP_EEPROM_SoftReset(uint32_t Instance)
{
  int32_t ret = M95_OK;

  EEPROMEX_CTRL_LOW();

  ret = (M95P32Drv[Instance]->SoftReset(M95P32CompObj[Instance]));

  EEPROMEX_CTRL_HIGH();

  return ret;
}


/**
  * @brief  Register Bus IOs for instance M95P32 if component ID is OK
  * @param  None
  * @retval BSP status
  */
static int32_t M95P32_Probe(void)
{
  M95_IO_t io_ctxm95p32;
  int32_t ret = M95_OK;
  static M95_Object_t M95P32_obj_0;

  io_ctxm95p32.Init                = SERIAL_COM_INIT;
  io_ctxm95p32.DeInit              = SERIAL_COM_DEINIT;
  io_ctxm95p32.Delay               = EEPROM_DELAY;
  io_ctxm95p32.Read                = SERIAL_COM_RECV;
  io_ctxm95p32.Write               = SERIAL_COM_SEND;
  io_ctxm95p32.SendRecv            = SERIAL_COM_SENDRECV;

  if (M95P32_RegisterBusIO(&M95P32_obj_0,&io_ctxm95p32) != M95_OK)
  {
    ret = M95_ERROR;
  }

  M95P32CompObj[BSP_EEPROM_0] = &M95P32_obj_0;
  M95P32Drv[BSP_EEPROM_0] = (M95P32_CommonDrv_t *)(void *)&M95P32_spi_Drv;

  if (M95P32Drv[BSP_EEPROM_0]->Init(M95P32CompObj[BSP_EEPROM_0]) != M95_OK)
  {
    ret = M95_ERROR;
  }
  else
  {
    ret = M95_OK;
  }
  return ret;
}

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

