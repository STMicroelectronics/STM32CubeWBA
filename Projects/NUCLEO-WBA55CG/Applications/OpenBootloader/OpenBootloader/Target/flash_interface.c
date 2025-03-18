/**
  ******************************************************************************
  * @file    flash_interface.c
  * @author  MCD Application Team
  * @brief   Contains FLASH access functions
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
#include "flash_interface.h"
#include "i2c_interface.h"
#include "optionbytes_interface.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FLASH_PAGE_MAX_NUMBER             ((uint8_t)0x7FU)
#define FLASH_PROG_STEP_SIZE              ((uint8_t)0x10U)
#define FLASH_PAGE_NUMBER                 ((uint16_t)128U)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t Flash_BusyState = FLASH_BUSY_STATE_DISABLED;
static FLASH_ProcessTypeDef FlashProcess = {.Lock = HAL_UNLOCKED, \
                                            .ErrorCode = HAL_FLASH_ERROR_NONE, \
                                            .ProcedureOnGoing = 0U, \
                                            .Address = 0U, \
                                            .Page = 0U, \
                                            .NbPagesToErase = 0U
                                           };

/* Private function prototypes -----------------------------------------------*/
static void OPENBL_FLASH_Program(uint32_t Address, uint32_t DataAddress);
static ErrorStatus OPENBL_FLASH_EnableWriteProtection(uint8_t *pListOfPages, uint32_t Length);
static ErrorStatus OPENBL_FLASH_DisableWriteProtection(void);

/* Exported variables --------------------------------------------------------*/
OPENBL_MemoryTypeDef FLASH_Descriptor =
{
  FLASH_START_ADDRESS,
  FLASH_END_ADDRESS,
  FLASH_MEM_SIZE,
  FLASH_AREA,
  OPENBL_FLASH_Read,
  OPENBL_FLASH_Write,
  OPENBL_FLASH_SetReadOutProtectionLevel,
  OPENBL_FLASH_SetWriteProtection,
  OPENBL_FLASH_JumpToAddress,
  NULL,
  OPENBL_FLASH_Erase
};

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Unlock the FLASH control register access.
  * @retval None.
  */
void OPENBL_FLASH_Unlock(void)
{
  HAL_FLASH_Unlock();
}

/**
  * @brief  Lock the FLASH control register access.
  * @retval None.
  */
void OPENBL_FLASH_Lock(void)
{
  HAL_FLASH_Lock();
}

/**
  * @brief  Unlock the FLASH Option Bytes Registers access.
  * @retval None.
  */
void OPENBL_FLASH_OB_Unlock(void)
{
  HAL_FLASH_Unlock();

  HAL_FLASH_OB_Unlock();
}

/**
  * @brief  This function is used to read data from a given address.
  * @param  Address The address to be read.
  * @retval Returns the read value.
  */
uint8_t OPENBL_FLASH_Read(uint32_t Address)
{
  return (*(uint8_t *)(Address));
}

/**
  * @brief  This function is used to write data in FLASH memory.
  * @param  Address The address where that data will be written.
  * @param  pData The data to be written.
  * @param  DataLength The length of the data to be written.
  * @retval None.
  */
void OPENBL_FLASH_Write(uint32_t Address, uint8_t *pData, uint32_t DataLength)
{
  uint32_t index;
  __ALIGNED(4) uint8_t data[FLASH_PROG_STEP_SIZE] = {0x0U};
  uint8_t remaining;

  if ((pData != NULL) && (DataLength != 0U))
  {
    /* Unlock the flash memory for write operation */
    OPENBL_FLASH_Unlock();

    /* Clear error programming flags */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

    /* Program quad-word by quad-word (16 bytes) */
    while ((DataLength >> 4U) > 0U)
    {
      for (index = 0U; index < FLASH_PROG_STEP_SIZE; index++)
      {
        data[index] = *(pData + index);
      }

      OPENBL_FLASH_Program(Address, (uint32_t)data);

      Address    += FLASH_PROG_STEP_SIZE;
      pData      += FLASH_PROG_STEP_SIZE;
      DataLength -= FLASH_PROG_STEP_SIZE;
    }

    /* If remaining count, go back to fill the rest with 0xFF */
    if (DataLength > 0U)
    {
      remaining = FLASH_PROG_STEP_SIZE - DataLength;

      /* Copy the remaining bytes */
      for (index = 0U; index < DataLength; index++)
      {
        data[index] = *(pData + index);
      }

      /* Fill the upper bytes with 0xFF */
      for (index = 0U; index < remaining; index++)
      {
        data[index + DataLength] = 0xFFU;
      }

      OPENBL_FLASH_Program(Address, (uint32_t)data);
    }

    /* Lock the Flash to disable the flash control register access */
    OPENBL_FLASH_Lock();
  }
}

/**
  * @brief  This function is used to jump to a given address.
  * @param  Address The address where the function will jump.
  * @retval None.
  */
void OPENBL_FLASH_JumpToAddress(uint32_t Address)
{
  Function_Pointer jump_to_address;

  /* De-initialize all HW resources used by the Open Bootloader to their reset values */
  OPENBL_DeInit();

  /* Enable IRQ */
  Common_EnableIrq();

  jump_to_address = (Function_Pointer)(*(__IO uint32_t *)(Address + 4U));

  /* Initialize user application's stack pointer */
  Common_SetMsp(*(__IO uint32_t *) Address);

  jump_to_address();
}

/**
  * @brief  Return the FLASH Read Protection level.
  * @retval The return value can be one of the following values:
  *         @arg OB_RDP_LEVEL_0: No protection
  *         @arg OB_RDP_LEVEL_1: Read protection of the memory
  *         @arg OB_RDP_LEVEL_2: Full chip protection
  */
uint32_t OPENBL_FLASH_GetReadOutProtectionLevel(void)
{
  FLASH_OBProgramInitTypeDef flash_ob;

  /* Get the Option bytes configuration */
  HAL_FLASHEx_OBGetConfig(&flash_ob);

  return flash_ob.RDPLevel;
}

/**
  * @brief  Return the FLASH Read Protection level.
  * @param  Level Can be one of these values:
  *         @arg OB_RDP_LEVEL_0: No protection
  *         @arg OB_RDP_LEVEL_1: Read protection of the memory
  *         @arg OB_RDP_LEVEL_2: Full chip protection
  * @retval None.
  */
void OPENBL_FLASH_SetReadOutProtectionLevel(uint32_t Level)
{
  FLASH_OBProgramInitTypeDef flash_ob;

  if (Level != OB_RDP_LEVEL2)
  {
    flash_ob.OptionType = OPTIONBYTE_RDP;
    flash_ob.RDPLevel   = Level;

    /* Unlock the FLASH registers & Option Bytes registers access */
    OPENBL_FLASH_OB_Unlock();

    /* Clear error programming flags */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

    /* Change the RDP level */
    HAL_FLASHEx_OBProgram(&flash_ob);

    /* Register system reset callback */
    Common_SetPostProcessingCallback(OPENBL_OB_Launch);
  }
}

/**
  * @brief  This function is used to enable or disable write protection of the specified FLASH areas.
  * @param  State Can be one of these values:
  *         @arg DISABLE: Disable FLASH write protection
  *         @arg ENABLE: Enable FLASH write protection
  * @param  pListOfPages Contains the list of pages to be protected.
  * @param  Length The length of the list of pages to be protected.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Enable or disable of the write protection is done
  *          - ERROR:   Enable or disable of the write protection is not done
  */
ErrorStatus OPENBL_FLASH_SetWriteProtection(FunctionalState State, uint8_t *pListOfPages, uint32_t Length)
{
  ErrorStatus status = SUCCESS;

  if (State == ENABLE)
  {
    OPENBL_FLASH_EnableWriteProtection(pListOfPages, Length);

    /* Register system reset callback */
    Common_SetPostProcessingCallback(OPENBL_OB_Launch);
  }
  else if (State == DISABLE)
  {
    OPENBL_FLASH_DisableWriteProtection();

    /* Register system reset callback */
    Common_SetPostProcessingCallback(OPENBL_OB_Launch);
  }
  else
  {
    status = ERROR;
  }

  return status;
}

/**
  * @brief  This function is used to start FLASH mass erase operation.
  * @param  *pData Pointer to the buffer that contains mass erase operation options.
  * @param  DataLength Size of the Data buffer.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Mass erase operation done
  *          - ERROR:   Mass erase operation failed or the value of one parameter is not ok
  */
ErrorStatus OPENBL_FLASH_MassErase(uint8_t *pData, uint32_t DataLength)
{
  uint32_t page_error;
  ErrorStatus status = SUCCESS;
  FLASH_EraseInitTypeDef erase_init_struct;

  /* Unlock the flash memory for erase operation */
  OPENBL_FLASH_Unlock();

  /* Clear error programming flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  erase_init_struct.TypeErase = FLASH_TYPEERASE_MASSERASE;

  if (DataLength >= 2U)
  {
    if (*(uint16_t *)(pData) == FLASH_MASS_ERASE)
    {
      if (HAL_FLASHEx_Erase(&erase_init_struct, &page_error) != HAL_OK)
      {
        status = ERROR;
      }
      else
      {
        status = SUCCESS;
      }
    }
    else
    {
      status = ERROR;
    }
  }
  else
  {
    status = ERROR;
  }

  /* Lock the Flash to disable the flash control register access */
  OPENBL_FLASH_Lock();

  return status;
}

/**
  * @brief  This function is used to erase the specified FLASH pages.
  * @param  *pData Pointer to the buffer that contains erase operation options.
  * @param  DataLength Size of the Data buffer.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Erase operation done
  *          - ERROR:   Erase operation failed or the value of one parameter is not ok
  */
ErrorStatus OPENBL_FLASH_Erase(uint8_t *pData, uint32_t DataLength)
{
  uint32_t counter;
  uint32_t pages_number;
  uint32_t page_error = 0U;
  uint32_t errors     = 0U;
  ErrorStatus status  = SUCCESS;
  FLASH_EraseInitTypeDef erase_init_struct;

  /* Unlock the flash memory for erase operation */
  OPENBL_FLASH_Unlock();

  /* Clear error programming flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  pages_number = (uint32_t)(*(uint16_t *)(pData));

  /* The page number size is 2 bytes */
  pData += 2U;

  erase_init_struct.TypeErase = FLASH_TYPEERASE_PAGES;
  erase_init_struct.NbPages   = 1U;

  for (counter = 0U; ((counter < pages_number) && (counter < (DataLength / 2U))); counter++)
  {
    erase_init_struct.Page = ((uint32_t)(*(uint16_t *)(pData)));

    if (OPENBL_FLASH_ExtendedErase(&erase_init_struct, &page_error) != HAL_OK)
    {
      errors++;
    }

    /* The page number size is 2 bytes */
    pData += 2U;
  }

  /* Lock the Flash to disable the flash control register access */
  OPENBL_FLASH_Lock();

  if (errors > 0U)
  {
    status = ERROR;
  }
  else
  {
    status = SUCCESS;
  }

  return status;
}

/**
  * @brief  This function is used to Set Flash busy state variable to activate busy state sending
  *         during flash operations
  * @retval None.
  */
void OPENBL_Enable_BusyState_Flag(void)
{
  /* Enable Flash busy state sending */
  Flash_BusyState = FLASH_BUSY_STATE_ENABLED;
}

/**
  * @brief  This function is used to disable the send of busy state in I2C non stretch mode.
  * @retval None.
  */
void OPENBL_Disable_BusyState_Flag(void)
{
  /* Disable Flash busy state sending */
  Flash_BusyState = FLASH_BUSY_STATE_DISABLED;
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Program double word at a specified FLASH address.
  * @param  Address specifies the address to be programmed.
  * @param  Data specifies the data to be programmed.
  * @retval None.
  */
static void OPENBL_FLASH_Program(uint32_t Address, uint32_t DataAddress)
{
  /* Clear all FLASH errors flags before starting write operation */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, Address, DataAddress);
}

/**
  * @brief  This function is used to enable write protection of the specified FLASH areas.
  * @param  pListOfPages Contains the list of pages to be protected.
  * @param  Length The length of the list of pages to be protected.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Enable or disable of the write protection is done
  *          - ERROR:   Enable or disable of the write protection is not done
  */
static ErrorStatus OPENBL_FLASH_EnableWriteProtection(uint8_t *pListOfPages, uint32_t Length)
{
  ErrorStatus status = SUCCESS;
  FLASH_OBProgramInitTypeDef flash_ob;

  /* Unlock the FLASH registers & Option Bytes registers access */
  OPENBL_FLASH_OB_Unlock();

  /* Clear error programming flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  flash_ob.OptionType = OPTIONBYTE_WRP;

  /* Write protection of bank 1 area WRPA 1 area */
  if (Length >= 2U)
  {
    flash_ob.WRPArea        = OB_WRPAREA_BANK1_AREAA;
    flash_ob.WRPLock        = DISABLE;
    flash_ob.WRPStartOffset = *(pListOfPages);
    flash_ob.WRPEndOffset   = *(pListOfPages + 1U);

    HAL_FLASHEx_OBProgram(&flash_ob);
  }

  /* Write protection of bank 1 area WRPA 2 area */
  if (Length >= 4U)
  {
    flash_ob.WRPArea        = OB_WRPAREA_BANK1_AREAB;
    flash_ob.WRPLock        = DISABLE;
    flash_ob.WRPStartOffset = *(pListOfPages + 2U);
    flash_ob.WRPEndOffset   = *(pListOfPages + 3U);

    HAL_FLASHEx_OBProgram(&flash_ob);
  }

  return status;
}

/**
  * @brief  This function is used to disable write protection.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Enable or disable of the write protection is done
  *          - ERROR:   Enable or disable of the write protection is not done
  */
static ErrorStatus OPENBL_FLASH_DisableWriteProtection(void)
{
  ErrorStatus status = SUCCESS;
  FLASH_OBProgramInitTypeDef flash_ob;

  /* Unlock the FLASH registers & Option Bytes registers access */
  OPENBL_FLASH_OB_Unlock();

  /* Clear error programming flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  flash_ob.OptionType = OPTIONBYTE_WRP;

  /* Disable write protection of bank 1 area WRPA 1 area */
  flash_ob.WRPArea        = OB_WRPAREA_BANK1_AREAA;
  flash_ob.WRPLock        = DISABLE;
  flash_ob.WRPStartOffset = FLASH_PAGE_MAX_NUMBER;
  flash_ob.WRPEndOffset   = 0x00U;

  HAL_FLASHEx_OBProgram(&flash_ob);

  /* Disable write protection of bank 1 area WRPA 2 area */
  flash_ob.WRPArea        = OB_WRPAREA_BANK1_AREAB;
  flash_ob.WRPLock        = DISABLE;
  flash_ob.WRPStartOffset = FLASH_PAGE_MAX_NUMBER;
  flash_ob.WRPEndOffset   = 0x00U;

  HAL_FLASHEx_OBProgram(&flash_ob);

  return status;
}

/**
  * @brief  Wait for a FLASH operation to complete.
  * @param  Timeout maximum flash operation timeout.
  * @retval HAL_Status
  */
#if defined (__ICCARM__)
__ramfunc HAL_StatusTypeDef OPENBL_FLASH_WaitForLastOperation(uint32_t Timeout)
#else
__attribute__((section(".ramfunc"))) HAL_StatusTypeDef OPENBL_FLASH_WaitForLastOperation(uint32_t Timeout)
#endif /* (__ICCARM__) */
{
  uint32_t counter = 0U;
  uint32_t error;
  __IO uint32_t *reg_sr;
  HAL_StatusTypeDef status = HAL_OK;

  /* While the FLASH is in busy state, send busy byte to the host */
  while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY))
  {
    /* Check if we need to send a busy byte
       NOTE: this can be removed if I2C protocol is not used */
    if (Flash_BusyState == FLASH_BUSY_STATE_ENABLED)
    {
      OPENBL_I2C_SendBusyByte();
    }

    if ((counter++) > Timeout)
    {
      status = HAL_TIMEOUT;
      break;
    }
  }

  /* Access to SECSR or NSSR registers depends on operation type */
  reg_sr = IS_FLASH_SECURE_OPERATION() ? &(FLASH->SECSR) : &(FLASH_NS->NSSR);

  /* Check FLASH operation error flags */
  error = ((*reg_sr) & FLASH_FLAG_SR_ERRORS);

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  error |= (FLASH->NSSR & FLASH_FLAG_OPTWERR);
#endif /* __ARM_FEATURE_CMSE */

  if (error != 0U)
  {
    /* Save the error code */
    FlashProcess.ErrorCode |= error;

    /* Clear error programming flags */
    (*reg_sr) = error;
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    if ((error & FLASH_FLAG_OPTWERR) != 0U)
    {
      FLASH->NSSR = FLASH_FLAG_OPTWERR;
    }
#endif /* __ARM_FEATURE_CMSE */

    status = HAL_ERROR;
  }

  /* Check FLASH End of Operation flag  */
  if (((*reg_sr) & FLASH_FLAG_EOP) != 0U)
  {
    /* Clear FLASH End of Operation pending bit */
    (*reg_sr) = FLASH_FLAG_EOP;
  }

  return status;
}

/**
  * @brief  Perform a mass erase or erase the specified FLASH memory pages.
  * @param[in]  pEraseInit pointer to an FLASH_EraseInitTypeDef structure that
  *         contains the configuration information for the erasing.
  * @param[out]  PageError pointer to variable that contains the configuration
  *         information on faulty page in case of error (0xFFFFFFFF means that all
  *         the pages have been correctly erased).
  * @retval HAL_Status
  */
#if defined (__ICCARM__)
__ramfunc HAL_StatusTypeDef OPENBL_FLASH_ExtendedErase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *PageError)
#else
__attribute__((section(".ramfunc"))) HAL_StatusTypeDef OPENBL_FLASH_ExtendedErase(FLASH_EraseInitTypeDef *pEraseInit,
    uint32_t *PageError)
#endif /* (__ICCARM__) */
{
  HAL_StatusTypeDef status;
  uint32_t errors = 0U;
  __IO uint32_t *reg;
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  uint32_t primask_bit;
#endif /* (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U) */

  /* Clear error programming flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  /* Process Locked */
  __HAL_LOCK(&FlashProcess);

  /* Reset error code */
  FlashProcess.ErrorCode = HAL_FLASH_ERROR_NONE;

  /* Wait for last operation to be completed */
  status = OPENBL_FLASH_WaitForLastOperation(PROGRAM_TIMEOUT);

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  /* Disable interrupts to avoid any interruption */
  primask_bit = __get_PRIMASK();
  __disable_irq();
#endif /* (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U) */

  if (status == HAL_OK)
  {
    FlashProcess.ProcedureOnGoing = pEraseInit->TypeErase;

    /* Access to SECCR or NSCR registers depends on operation type */
    reg = IS_FLASH_SECURE_OPERATION() ? &(FLASH->SECCR1) : &(FLASH_NS->NSCR1);

    /*Initialization of PageError variable*/
    *PageError = 0xFFFFFFFFU;

    /* Proceed to erase the page */
    MODIFY_REG((*reg), (FLASH_NSCR1_PNB | FLASH_NSCR1_PER),
               (((pEraseInit->Page) << FLASH_NSCR1_PNB_Pos) | FLASH_NSCR1_PER));

    /* Set the start bit */
    SET_BIT((*reg), FLASH_NSCR1_STRT);

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    /* Re-enable the interrupts */
    __set_PRIMASK(primask_bit);
#endif /* (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U) */

    /* Wait for last operation to be completed */
    if (OPENBL_FLASH_WaitForLastOperation(PROGRAM_TIMEOUT) != HAL_OK)
    {
      errors++;
    }

    /* If the erase operation is completed, disable the associated bits */
    CLEAR_BIT((*reg), (FlashProcess.ProcedureOnGoing & ~(FLASH_NON_SECURE_MASK)));
  }

  /* Process Unlocked */
  __HAL_UNLOCK(&FlashProcess);

  if (errors > 0U)
  {
    status = HAL_ERROR;
  }
  else
  {
    status = HAL_OK;
  }

  return status;
}
