/**
  ******************************************************************************
  * @file    secure_nsc.c
  * @author  MCD Application Team
  * @brief   This file contains the non-secure callable APIs (secure world)
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
#include "main.h"
#include "secure_nsc.h"
#include <stdio.h>
#include "Driver_Flash.h"

extern ARM_DRIVER_FLASH FLASH_PRIMARY_DEV_NAME;
#define TRAILER_MAGIC_SIZE 16

/* Global variables ----------------------------------------------------------*/
void *pSecureFaultCallback = NULL;   /* Pointer to secure fault callback in Non-secure */
void *pSecureErrorCallback = NULL;   /* Pointer to secure error callback in Non-secure */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Secure registration of non-secure callback.
  * @param  CallbackId  callback identifier
  * @param  func        pointer to non-secure function
  * @retval None
  */
CMSE_NS_ENTRY void SECURE_RegisterCallback(SECURE_CallbackIDTypeDef CallbackId, void *func)
{
  if (func != NULL)
  {
    switch (CallbackId)
    {
      case SECURE_FAULT_CB_ID:           /* SecureFault Interrupt occurred */
        pSecureFaultCallback = func;
        break;
      case GTZC_ERROR_CB_ID:             /* GTZC Interrupt occurred */
        pSecureErrorCallback = func;
        break;
      default:
        /* unknown */
        break;
    }
  }
}

/**
  * @brief  Secure Operation to confirm Secure App Image.
  * @param  None
  * @param  None
  * @retval None
  */
CMSE_NS_ENTRY void SECURE_ConfirmSecureAppImage(void)
{
#if defined(MCUBOOT_OVERWRITE_ONLY) || (MCUBOOT_APP_IMAGE_NUMBER == 1)
  return;
#else
  const uint8_t FlagPattern[16]={0x1 ,0xff, 0xff, 0xff, 0xff , 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff , 0xff, 0xff, 0xff };
  const uint32_t ConfirmAddress = FLASH_AREA_0_OFFSET  + FLASH_AREA_0_SIZE - (TRAILER_MAGIC_SIZE + sizeof(FlagPattern));
  if (FLASH_PRIMARY_DEV_NAME.ProgramData(ConfirmAddress, FlagPattern, sizeof(FlagPattern)) == ARM_DRIVER_OK)
  {
        return;
  }
  return;
#endif
}

/**
  * @brief  Secure Operation to confirm Secure Data Image.
  * @param  None
  * @param  None
  * @retval None
  */
CMSE_NS_ENTRY void SECURE_ConfirmSecureDataImage(void)
{
#if defined(MCUBOOT_OVERWRITE_ONLY) || (MCUBOOT_S_DATA_IMAGE_NUMBER == 0)
  return;
#else
  const uint8_t FlagPattern[16]={0x1 ,0xff, 0xff, 0xff, 0xff , 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff , 0xff, 0xff, 0xff };
  const uint32_t ConfirmAddress = FLASH_AREA_4_OFFSET  + FLASH_AREA_4_SIZE - (TRAILER_MAGIC_SIZE + sizeof(FlagPattern));
  if (FLASH_PRIMARY_DEV_NAME.ProgramData(ConfirmAddress, FlagPattern, sizeof(FlagPattern)) == ARM_DRIVER_OK)
  {
        return;
  }
  return;
#endif
}

#if defined(OEMIROT_OTFDEC_EXTERNAL_FLASH_ENABLE)
/**
  * @brief  Secure entry point used by non-secure code to start the secure loader.
  *         This function resides in the secure application and is exposed as a
  *         non-secure callable (NSC) API. When invoked from the non-secure
  *         domain, it transfers execution to the secure loader image.
  * @param  CallbackId  callback identifier
  * @param  func        pointer to non-secure function
  * @retval None
  */
CMSE_NS_ENTRY void SECURE_LoaderRun(void)
{
  /* Interpret the beginning of the secure loader image as its vector table */
  Loader_VectorTable_t *loader_vt = (Loader_VectorTable_t *)LOADER_S_CODE_START;

  __disable_irq();

  __set_MSP(loader_vt->loader_msp);

  /* Relocate the secure vector table to the loader image */
  SCB->VTOR = LOADER_S_CODE_START;

  __DSB();
  __ISB();

  /* Jump to the secure loader Reset_Handler */
  loader_vt->loader_reset();
}
#endif /* OEMIROT_OTFDEC_EXTERNAL_FLASH_ENABLE */