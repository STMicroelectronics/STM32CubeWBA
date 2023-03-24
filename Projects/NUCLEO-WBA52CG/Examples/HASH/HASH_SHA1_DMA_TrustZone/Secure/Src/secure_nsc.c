/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Secure/Src/secure_nsc.c
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
/* USER CODE END Header */

/* USER CODE BEGIN Non_Secure_CallLib */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "secure_nsc.h"
/** @addtogroup STM32WBAxx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Global variables ----------------------------------------------------------*/
void *pSecureFaultCallback = NULL;   /* Pointer to secure fault callback in Non-secure */
void *pSecureErrorCallback = NULL;   /* Pointer to secure error callback in Non-secure */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define HASH_SHA1_DIGEST_SIZE     20U
#define HASH_SHA1_DIGEST_TIMEOUT  255U
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern HASH_HandleTypeDef hhash;
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
  if(func != NULL)
  {
    switch(CallbackId)
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
  * @brief  Secure HASH SHA-1 computation
  * @param  pInputData       Pointer to input data buffer
  * @param  InputDataSize    Size of input data in bytes
  * @param  pOutputSignature Pointer to output signature buffer
  * @retval SUCCESS or ERROR
  */
CMSE_NS_ENTRY ErrorStatus SECURE_HASH_SHA1_8BITS(uint8_t *pInputData, uint32_t InputDataSize, uint8_t *pOutputSignature)
{
  ErrorStatus  ret = ERROR;

  /* Check input and output address range in non-secure) */
  if (cmse_check_address_range(pInputData, InputDataSize * sizeof(uint8_t), CMSE_NONSECURE) &&
      cmse_check_address_range(pOutputSignature, HASH_SHA1_DIGEST_SIZE * sizeof(uint8_t), CMSE_NONSECURE))
  {
    /* Start HASH computation using DMA transfer */
    if (HAL_HASH_Start_DMA(&hhash, pInputData, InputDataSize,pOutputSignature) == HAL_OK)
    {
      /* Wait for DMA transfer to complete */
      while (HAL_HASH_GetState(&hhash) == HAL_HASH_STATE_BUSY);
       ret = SUCCESS;
    }
    else
    {
      /* No output signature in case of error */
    }
  }

  return ret;
}

/**
  * @}
  */

/**
  * @}
  */
/* USER CODE END Non_Secure_CallLib */

