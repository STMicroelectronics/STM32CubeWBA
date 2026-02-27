/*
 * Copyright (c) 2013-2018 Arm Limited
 * Copyright (c) 2024 STMicroelectronics
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Driver_Flash.h"
#include "low_level_ext_flash.h"
#include <string.h>
#include "stm32_hal.h"
#include "appli_flash_layout.h"
#include <stdio.h>
#include <inttypes.h>

#if defined (OEMIROT_EXTERNAL_FLASH_ENABLE)
#include "stm32wbaxx_nucleo_xspi.h"


#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)  ((void)arg)
#endif /* ARG_UNUSED */

/* config for flash driver */
#define EXT_FLASH0_TOTAL_SIZE   EXT_FLASH_TOTAL_SIZE
#define EXT_FLASH0_SECTOR_SIZE  (uint32_t)(4 * 1024) /* Must be same as internal flash sector size */
#define EXT_FLASH0_BLOCK_SIZE   (uint32_t)(64  * 1024) /* Must be same as internal flash sector size */
#define EXT_FLASH0_PAGE_SIZE    (uint32_t)256
#define EXT_FLASH0_PROG_UNIT    0x02 /* 2 bytes for XSPI DTR mode */
#define EXT_FLASH0_ERASED_VAL   0xff

/*
#define DEBUG_EXT_FLASH_ACCESS
*/
/* Driver version */
#define ARM_EXT_FLASH_DRV_VERSION   ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)

static const ARM_DRIVER_VERSION DriverVersion =
{
  ARM_FLASH_API_VERSION,  /* Defined in the CMSIS Flash Driver header file */
  ARM_EXT_FLASH_DRV_VERSION
};

/**
  * \brief Flash driver capability macro definitions \ref ARM_FLASH_CAPABILITIES
  */
/* Flash Ready event generation capability values */
#define EVENT_READY_NOT_AVAILABLE   (0u)
#define EVENT_READY_AVAILABLE       (1u)
/* Data access size values */
#define DATA_WIDTH_8BIT             (0u)
#define DATA_WIDTH_16BIT            (1u)
#define DATA_WIDTH_32BIT            (2u)
/* Chip erase capability values */
#define CHIP_ERASE_NOT_SUPPORTED    (0u)
#define CHIP_ERASE_SUPPORTED        (1u)


/* Driver Capabilities */
static const ARM_FLASH_CAPABILITIES DriverCapabilities =
{
  EVENT_READY_NOT_AVAILABLE,
  DATA_WIDTH_16BIT, /* 16bits for DTR mode */
  CHIP_ERASE_SUPPORTED
};

/**
  * \brief Flash status macro definitions \ref ARM_FLASH_STATUS
  */
/* Busy status values of the Flash driver  */
#define DRIVER_STATUS_IDLE      (0u)
#define DRIVER_STATUS_BUSY      (1u)
/* Error status values of the Flash driver */
#define DRIVER_STATUS_NO_ERROR  (0u)
#define DRIVER_STATUS_ERROR     (1u)

/**
  * \brief Arm Flash device structure.
  */
struct arm_ext_flash_dev_t
{
  struct low_level_ext_flash_device *dev;
  ARM_FLASH_INFO *data;     /*!< EXT FLASH memory device data */
  BSP_XSPI_Info_t *info;    /* Dynamic information on the external flash */
};

/**
  * @}
  */

/**
  * \brief      Check if the Flash memory boundaries are not violated.
  * \param[in]  flash_dev  Flash device structure \ref arm_ext_flash_dev_t
  * \param[in]  offset     Highest Flash memory address which would be accessed.
  * \return     Returns true if Flash memory boundaries are not violated, false
  *             otherwise.
  */

static bool is_range_valid(struct arm_ext_flash_dev_t *flash_dev,
                           uint32_t offset)
{
  uint32_t flash_limit = 0;

  /* Calculating the highest address of the Flash memory address range */
  flash_limit = EXT_FLASH_TOTAL_SIZE - 1;

  return (offset > flash_limit) ? (false) : (true) ;
}
/**
  * \brief        Check if the parameter is an erasable page.
  * \param[in]    flash_dev  Flash device structure \ref arm_ext_flash_dev_t
  * \param[in]    param      Any number that can be checked against the
  *                          program_unit, e.g. Flash memory address or
  *                          data length in bytes.
  * \return       Returns true if param is a sector eraseable, false
  *               otherwise.
  */
static bool is_erase_allow(struct arm_ext_flash_dev_t *flash_dev,
                           uint32_t param)
{
  /*  allow erase in range provided by device info */
  struct ext_flash_vect *vect = &flash_dev->dev->erase;
  uint32_t nb;
  for (nb = 0; nb < vect->nb; nb++)
    if ((param >= vect->range[nb].base) && (param <= vect->range[nb].limit))
    {
      return true;
    }
  return false;
}
/**
  * \brief        Check if the parameter is writeable area.
  * \param[in]    flash_dev  Flash device structure \ref arm_ext_flash_dev_t
  * \param[in]    param      Any number that can be checked against the
  *                          program_unit, e.g. Flash memory address or
  *                          data length in bytes.
  * \return       Returns true if param is aligned to program_unit, false
  *               otherwise.
  */
static bool is_write_allow(struct arm_ext_flash_dev_t *flash_dev,
                           uint32_t start, uint32_t len)
{
  /*  allow write access in area provided by device info */
  struct ext_flash_vect *vect = &flash_dev->dev->write;
  uint32_t nb;
  for (nb = 0; nb < vect->nb; nb++)
    if ((start >= vect->range[nb].base) && ((start + len - 1) <= vect->range[nb].limit))
    {
      return true;
    }
  return false;
}

/**
  * \brief        Check if the parameter is aligned to program_unit.
  * \param[in]    flash_dev  Flash device structure \ref arm_ext_flash_dev_t
  * \param[in]    param      Any number that can be checked against the
  *                          program_unit, e.g. Flash memory address or
  *                          data length in bytes.
  * \return       Returns true if param is aligned to program_unit, false
  *               otherwise.
  */

static bool is_write_aligned(struct arm_ext_flash_dev_t *flash_dev,
                             uint32_t param)
{
  return ((param % flash_dev->data->program_unit) != 0) ? (false) : (true);
}
/**
  * \brief        Check if the parameter is aligned to page_unit.
  * \param[in]    flash_dev  Flash device structure \ref arm_ext_flash_dev_t
  * \param[in]    param      Any number that can be checked against the
  *                          program_unit, e.g. Flash memory address or
  *                          data length in bytes.
  * \return       Returns true if param is aligned to sector_unit, false
  *               otherwise.
  */
static bool is_erase_aligned(struct arm_ext_flash_dev_t *flash_dev,
                             uint32_t param)
{
  return ((param % (flash_dev->data->sector_size)) != 0) ? (false) : (true);
}


static ARM_FLASH_INFO ARM_EXT_FLASH0_DEV_DATA =
{
  .sector_info    = NULL,     /* Uniform sector layout */
  .sector_count   = EXT_FLASH0_TOTAL_SIZE / EXT_FLASH0_SECTOR_SIZE,
  .sector_size    = EXT_FLASH0_SECTOR_SIZE,
  .page_size      = EXT_FLASH0_PAGE_SIZE,
  .program_unit   = EXT_FLASH0_PROG_UNIT,
  .erased_value   = EXT_FLASH0_ERASED_VAL,
};

static BSP_XSPI_Info_t EXTMEM_FLASH_INFO =
{
  .FlashSize            = 0UL,
  .EraseBlockSize       = 0UL,
  .EraseBlocksNumber    = 0UL,
  .EraseSubBlockSize    = 0UL,
  .EraseSubBlocksNumber = 0UL,
  .EraseSectorSize      = 0UL,
  .EraseSectorsNumber   = 0UL,
  .ProgPageSize         = 0UL,
  .ProgPagesNumber      = 0UL
};

static struct arm_ext_flash_dev_t ARM_EXT_FLASH0_DEV =
{
  .dev  = &(EXT_FLASH0_DEV),
  .data = &(ARM_EXT_FLASH0_DEV_DATA),
  .info = &(EXTMEM_FLASH_INFO)
};

/* Flash Status */
static ARM_FLASH_STATUS ARM_EXT_FLASH0_STATUS = {0, 0, 0};

static ARM_DRIVER_VERSION Ext_Flash_GetVersion(void)
{
  return DriverVersion;
}

static ARM_FLASH_CAPABILITIES Ext_Flash_GetCapabilities(void)
{
  return DriverCapabilities;
}

static int32_t Ext_Flash_Initialize(ARM_Flash_SignalEvent_t cb_event)
{
  ARG_UNUSED(cb_event);
  BSP_XSPI_Init_t bsp_xpsi_init;
  int32_t status;

  bsp_xpsi_init.InterfaceMode = BSP_XSPI_SPI_MODE;

  if (BSP_XSPI_Init(0, &bsp_xpsi_init) != BSP_ERROR_NONE)
  {
    status = ARM_DRIVER_ERROR;
  }
  else
  {
    status = ARM_DRIVER_OK;
  }

  /* Wait for end of initialization */
  while(BSP_XSPI_GetStatus(0) == BSP_ERROR_BUSY);

  return status;
}

static int32_t Ext_Flash_Uninitialize(void)
{
  int32_t status;

  if (BSP_XSPI_DeInit(0) != BSP_ERROR_NONE)
  {
    status = ARM_DRIVER_ERROR;
  }
  else
  {
    status = ARM_DRIVER_OK;
  }

  /* Wait for end of deinitialization */
  while(BSP_XSPI_GetStatus(0) == BSP_ERROR_BUSY);

  return status;
}

static int32_t Ext_Flash_PowerControl(ARM_POWER_STATE state)
{
  switch (state)
  {
    case ARM_POWER_FULL:
      /* Nothing to be done */
      return ARM_DRIVER_OK;
    case ARM_POWER_OFF:
    case ARM_POWER_LOW:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
    default:
      return ARM_DRIVER_ERROR_PARAMETER;
  }
}

static int32_t Ext_Flash_ReadData(uint32_t addr, void *data, uint32_t cnt)
{
  int32_t err = BSP_ERROR_NONE;
  uint8_t data_tmp[2];

  ARM_EXT_FLASH0_STATUS.error = DRIVER_STATUS_NO_ERROR;

#ifdef DEBUG_EXT_FLASH_ACCESS
  printf("read xspi 0x%x n=%x \r\n", (addr + EXT_FLASH_BASE_ADDRESS), cnt);
#endif /*  DEBUG_EXT_FLASH_ACCESS */

  /* Check Flash memory boundaries */
  if (!is_range_valid(&ARM_EXT_FLASH0_DEV, addr + cnt - 1))
  {
    printf("read xspi not allowed 0x%x n=%x \r\n", (addr + EXT_FLASH_BASE_ADDRESS), cnt);
    ARM_EXT_FLASH0_STATUS.error = DRIVER_STATUS_ERROR;
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  ARM_EXT_FLASH0_STATUS.busy = DRIVER_STATUS_BUSY;

  /* XSPI DTR mode constraint: split read request to ensure read at
   * even address with even number of bytes.
   * Flash address to read is the offset from begin of external flash.
   */
  if (addr % 2)
  {
    err = BSP_XSPI_Read(0, (uint8_t *)data_tmp, addr - 1, 2);
    *(uint8_t*)data = data_tmp[1];

    if (cnt > 1)
    {
      if (cnt % 2)
      {
        if (err == BSP_ERROR_NONE)
        {
          err = BSP_XSPI_Read(0, (uint8_t *)data + 1, (addr + 1), cnt - 1);
        }
      }
      else
      {
        if (err == BSP_ERROR_NONE)
        {
          err = BSP_XSPI_Read(0, (uint8_t *)data + 1, (addr + 1), cnt - 2);
        }

        if (err == BSP_ERROR_NONE)
        {
          err = BSP_XSPI_Read(0, (uint8_t *)data_tmp, addr + cnt - 1, 2);
          *((uint8_t*)data + cnt - 1) = data_tmp[0];
        }
      }
    }
  }
  else
  {
    if (cnt % 2)
    {
      if (cnt > 1)
      {
        err = BSP_XSPI_Read(0, (uint8_t *)data, addr, cnt - 1);
      }

      if (err == BSP_ERROR_NONE)
      {
        err = BSP_XSPI_Read(0, (uint8_t *)data_tmp, addr + cnt - 1, 2);
        *((uint8_t*)data + cnt -1) = data_tmp[0];
      }
    }
    else
    {
      err = BSP_XSPI_Read(0, (uint8_t *)data, addr, cnt);
    }
  }

  ARM_EXT_FLASH0_STATUS.busy = DRIVER_STATUS_IDLE;

  if (err != BSP_ERROR_NONE)
  {
    printf("failed read external memory 0x%x n=%x \r\n", (addr + EXT_FLASH_BASE_ADDRESS), cnt);
    return ARM_DRIVER_ERROR;
  }

  return ARM_DRIVER_OK;
}

static int32_t Ext_Flash_ProgramData(uint32_t addr,
                                      const void *data, uint32_t cnt)
{
  int32_t err = BSP_ERROR_NONE;

  ARM_EXT_FLASH0_STATUS.error = DRIVER_STATUS_NO_ERROR;

#ifdef DEBUG_EXT_FLASH_ACCESS
  printf("write xspi 0x%x n=%x \r\n", (addr + EXT_FLASH_BASE_ADDRESS), cnt);
#endif /* DEBUG_EXT_FLASH_ACCESS */
  /* Check Flash memory boundaries and alignment with minimum write size
   * (program_unit), data size also needs to be a multiple of program_unit.
   */
  if ((!is_range_valid(&ARM_EXT_FLASH0_DEV, addr + cnt - 1)) ||
      (!is_write_aligned(&ARM_EXT_FLASH0_DEV, addr))     ||
      (!is_write_aligned(&ARM_EXT_FLASH0_DEV, cnt))      ||
      (!is_write_allow(&ARM_EXT_FLASH0_DEV, addr, cnt))
     )
  {
    printf("write xspi not allowed 0x%x n=%x \r\n", (addr + EXT_FLASH_BASE_ADDRESS), cnt);
    ARM_EXT_FLASH0_STATUS.error = DRIVER_STATUS_ERROR;
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  ARM_EXT_FLASH0_STATUS.busy = DRIVER_STATUS_BUSY;

  /* external flash address to write is the offset from begin of external flash */
  err = BSP_XSPI_Write(0, (uint8_t *) data, addr, cnt);

  /* Wait for end of writing sequence */
  while(BSP_XSPI_GetStatus(0) == BSP_ERROR_BUSY);

  ARM_EXT_FLASH0_STATUS.busy = DRIVER_STATUS_IDLE;

  if (err != BSP_ERROR_NONE)
  {
    printf("->>> Failed to write 0x%" PRIx32 " bytes at 0x%" PRIx32 " (Error 0x%" PRIx32 ")", cnt, (uint32_t)(EXT_FLASH_BASE_ADDRESS + addr), err);
    return ARM_DRIVER_ERROR;
  }
  return ARM_DRIVER_OK;
}

static int32_t Ext_Flash_EraseSector(uint32_t addr)
{
  int32_t err = BSP_ERROR_NONE;

  ARM_EXT_FLASH0_STATUS.error = DRIVER_STATUS_NO_ERROR;

#ifdef DEBUG_EXT_FLASH_ACCESS
  printf("erase xspi 0x%x\r\n", (addr + EXT_FLASH_BASE_ADDRESS));
#endif /* DEBUG_EXT_FLASH_ACCESS */
  if (!(is_range_valid(&ARM_EXT_FLASH0_DEV, addr)) ||
      !(is_erase_aligned(&ARM_EXT_FLASH0_DEV, addr)) ||
      !(is_erase_allow(&ARM_EXT_FLASH0_DEV, addr)))
  {
    printf("erase xspi not allowed 0x%x\r\n", (addr + EXT_FLASH_BASE_ADDRESS));
    ARM_EXT_FLASH0_STATUS.error = DRIVER_STATUS_ERROR;
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  ARM_EXT_FLASH0_STATUS.busy = DRIVER_STATUS_BUSY;

  if (ARM_EXT_FLASH0_DEV.info->EraseSectorSize == BSP_XSPI_ERASE_4K)
  {
    if (BSP_XSPI_Erase_Block(0, addr, BSP_XSPI_ERASE_4K) != BSP_ERROR_NONE)
    {
      err = BSP_ERROR_FEATURE_NOT_SUPPORTED;
    }
  }

  /* Wait for end of erase */
  while(BSP_XSPI_GetStatus(0) == BSP_ERROR_BUSY);

  ARM_EXT_FLASH0_STATUS.busy = DRIVER_STATUS_IDLE;

  if (err != BSP_ERROR_NONE)
  {
    printf("->>> Erase failed at 0x%" PRIx32 " (Error 0x%" PRIx32 ")", (uint32_t)(EXT_FLASH_BASE_ADDRESS + addr), err);
    return ARM_DRIVER_ERROR;
  }
  return ARM_DRIVER_OK;
}

static int32_t Ext_Flash_EraseChip(void)
{
  int32_t status;

  if (BSP_XSPI_Erase_Chip(0) != BSP_ERROR_NONE)
  {
    status = ARM_DRIVER_ERROR;
  }
  else
  {
    status = ARM_DRIVER_OK;
  }

  return status;
}

static ARM_FLASH_STATUS Ext_Flash_GetStatus(void)
{
  return ARM_EXT_FLASH0_STATUS;
}

static ARM_FLASH_INFO *Ext_Flash_GetInfo(void)
{
  return ARM_EXT_FLASH0_DEV.data;
}

ARM_DRIVER_FLASH Driver_EXT_FLASH0 =
{
  Ext_Flash_GetVersion,
  Ext_Flash_GetCapabilities,
  Ext_Flash_Initialize,
  Ext_Flash_Uninitialize,
  Ext_Flash_PowerControl,
  Ext_Flash_ReadData,
  Ext_Flash_ProgramData,
  Ext_Flash_EraseSector,
  Ext_Flash_EraseChip,
  Ext_Flash_GetStatus,
  Ext_Flash_GetInfo
};

#endif /* OEMIROT_EXTERNAL_FLASH_ENABLE */
