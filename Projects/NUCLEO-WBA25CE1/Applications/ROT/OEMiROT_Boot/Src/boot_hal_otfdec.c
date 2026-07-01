/**
  ******************************************************************************
  * @file    boot_hal_otfdec.c
  * @author  MCD Application Team
  * @brief   security protection implementation for secure boot
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/


#include "flash_layout.h"
#if  defined(OEMIROT_OTFDEC_EXTERNAL_FLASH_ENABLE)
#include "boot_hal_otfdec.h"
#include "Driver_Flash.h"
#include "stm32_board.h"
#if defined(MCUBOOT_USE_HASH_REF)
#include "bootutil_priv.h"
#endif
#ifdef OEMIROT_DEV_MODE
#define BOOT_LOG_LEVEL BOOT_LOG_LEVEL_INFO
#else
#define BOOT_LOG_LEVEL BOOT_LOG_LEVEL_OFF
#endif /* OEMIROT_DEV_MODE  */
#include "bootutil/bootutil_log.h"

/* otfdec magic is flash min write size: doubleword */
const uint32_t otfdec_magic[] =
{
  0x5a8b324f,
  0xc66da104,
};


/* otfdec key and magic flash layout */
#define OTFDEC_KEY_SIZE     BOOT_ENC_KEY_SIZE
#define OTFDEC_MAGIC_ARR_SZ (sizeof otfdec_magic / sizeof otfdec_magic[0])
#define OTFDEC_MAGIC_SZ     (sizeof otfdec_magic)

#define OTFDEC_KEY_ADDR     (FLASH_AREA_OTFDEC_OFFSET)
#define OTFDEC_MAGIC_ADDR   (FLASH_AREA_OTFDEC_OFFSET + OTFDEC_KEY_SIZE)


extern ARM_DRIVER_FLASH EXT_FLASH_DEV_NAME;
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;

/**
  * @brief This function invalidate otfdec symmetric key in internal flash
  *        (if not already invalidated)
  * @param none
  * @return 0 on success; nonzero on failure.
  */
int otfdec_invalidate_key(void)
{
  const uint8_t otfdec_zero[OTFDEC_MAGIC_SZ] = {0};

  /* Check if key is valid */
  if (otfdec_is_key_valid() == 1)
  {
    /* Key can be invalidated */
    if (FLASH_DEV_NAME.ProgramData(OTFDEC_MAGIC_ADDR, otfdec_zero, OTFDEC_MAGIC_SZ)
        != ARM_DRIVER_OK)
    {
      return BOOT_EFLASH;
    }
  }
  return 0;
}

/**
  * @brief This function writes otfdec symmetric key in internal flash
  * @param data source location of otfdec key
  * @return 0 on success; nonzero on failure.
  */
int otfdec_write_key(const void *data)
{
  /* Erase key sector */
  if (FLASH_DEV_NAME.EraseSector(OTFDEC_KEY_ADDR) != ARM_DRIVER_OK)
  {
    return BOOT_EFLASH;
  }

  /* Write key */
  if (FLASH_DEV_NAME.ProgramData(OTFDEC_KEY_ADDR, data, OTFDEC_KEY_SIZE)
      != ARM_DRIVER_OK)
  {
    return BOOT_EFLASH;
  }

  /* Write key valid tag */
  if (FLASH_DEV_NAME.ProgramData(OTFDEC_MAGIC_ADDR, otfdec_magic, OTFDEC_MAGIC_SZ)
      != ARM_DRIVER_OK)
  {
    return BOOT_EFLASH;
  }

  return 0;
}

/**
  * @brief This function check if otfdec symmetric key is valid in
  *        internal flash
  * @param none
  * @return 1 if key is valid, 0 if key is not valid
  */
int otfdec_is_key_valid(void)
{
  uint32_t data[OTFDEC_MAGIC_ARR_SZ];

  /* Read otfdec key magic from internal flash */
  if (FLASH_DEV_NAME.ReadData(OTFDEC_MAGIC_ADDR, data, OTFDEC_MAGIC_SZ) == ARM_DRIVER_OK)
  {
    if (memcmp(data, otfdec_magic, OTFDEC_MAGIC_SZ) == 0)
    {
      return 1;
    }
  }

  return 0;
}

/**
  * @brief This function read otfdec symmetric key from internal flash
  * @param data pointer to key data
  * @return 0 on success; nonzero on failure.
  */
int otfdec_read_key(void *data)
{
  /* Read key valid tag in internal flash */
  if (FLASH_DEV_NAME.ReadData(OTFDEC_KEY_ADDR, data, OTFDEC_KEY_SIZE) != ARM_DRIVER_OK)
  {
    return BOOT_EFLASH;
  }
  return 0;
}

/**
  * @brief This function configures the ospi flash on the fly decryption (OTFDEC).
  * @note
  * @retval execution_status
  */
int32_t otfdec_config(void)
{
  int32_t err = ARM_DRIVER_OK;

  OTFDEC_HandleTypeDef OtfdecHandle;
  OTFDEC_RegionConfigTypeDef OtfdecConfig = {0};
  uint8_t key[OTFDEC_KEY_SIZE];
  uint8_t key_swapped[OTFDEC_KEY_SIZE];
  uint32_t i;

  /* Enable OTFDEC1 clock */
  __HAL_RCC_OTFDEC1_CLK_ENABLE();

  /* Init OTFDEC1 peripheral */
  OtfdecHandle.Instance = OTFDEC1;

  HAL_OTFDEC_DeInit(&OtfdecHandle);

  if (HAL_OTFDEC_Init(&OtfdecHandle) != HAL_OK)
  {
    err = ARM_DRIVER_ERROR;
  }

  /* Set OTFDEC Mode */
  if ((err == ARM_DRIVER_OK)
      && (HAL_OTFDEC_RegionSetMode(&OtfdecHandle, OTFDEC_REGION1,
                                   OTFDEC_REG_MODE_INSTRUCTION_OR_DATA_ACCESSES)
          != HAL_OK))
  {
    err = ARM_DRIVER_ERROR;
  }

  /* Get the decrypted otfdec symmetric Key */
  if ((err == ARM_DRIVER_OK)
      && (otfdec_read_key(key) != 0))
  {
    err = ARM_DRIVER_ERROR;
  }

  BOOT_LOG_INF("otfdec key %x, %x, %x, %x, %x, %x, %x, %x,",
               key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7]);
  BOOT_LOG_INF("otfdec key %x, %x, %x, %x, %x, %x, %x, %x,\r\n",
               key[8], key[9], key[10], key[11], key[12], key[13], key[14], key[15]);

  /* Swap the key :
     - Reakey function : Key[0:127] : byte0[8:0] || byte1[8:0]  ...  || byte15[8:0]
     - OTFDEC : AES_KEY[127:0]= RxKEYR3[31:0] || RxKEYR2[31:0] || RxKEYR1[31:0] || RxKEYR0[31:0]
   */
  if (err == ARM_DRIVER_OK)
  {
    for (i = 0; i < OTFDEC_KEY_SIZE; i++)
    {
      key_swapped[i] = key[OTFDEC_KEY_SIZE - 1 - i];
    }
  }

  /* Set OTFDEC Key */
  if ((err == ARM_DRIVER_OK)
      && (HAL_OTFDEC_RegionSetKey(&OtfdecHandle, OTFDEC_REGION1,
                                  (uint32_t *)key_swapped) != HAL_OK))
  {
    err = ARM_DRIVER_ERROR;
  }

  /* Configure OTFDEC region:
     - Nonce[0] / Nonce[1]  -> most significant 64 bits of IV
     - StartAddress         -> contributes to IV and defines region start
     - EndAddress           -> defines region end
     - Version              -> least significant 16 bits of IV
     - Note: for WBA, 12 LSBs and 4 MSBs of StartAddress/EndAddress are ignored
       when defining the region granularity.
     - With current values, the resulting IV is:
         IV (hex) = 00000000000000000000000009000040
   */
  OtfdecConfig.Nonce[0]     = 0x0;
  OtfdecConfig.Nonce[1]     = 0x0;
  OtfdecConfig.StartAddress = NS_CODE_START;
  OtfdecConfig.EndAddress   = NS_CODE_START + FLASH_NS_PARTITION_SIZE;
  OtfdecConfig.Version      = 0;
  if ((err == ARM_DRIVER_OK)
      && (HAL_OTFDEC_RegionConfig(&OtfdecHandle, OTFDEC_REGION1, &OtfdecConfig,
                                  OTFDEC_REG_CONFIGR_LOCK_DISABLE) != HAL_OK))
  {
    err = ARM_DRIVER_ERROR;
  }

  return err;
}


#endif /* OEMIROT_OTFDEC_EXTERNAL_FLASH_ENABLE */
