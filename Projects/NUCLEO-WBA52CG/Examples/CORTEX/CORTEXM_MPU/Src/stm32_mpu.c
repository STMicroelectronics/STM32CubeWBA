/**
  ******************************************************************************
  * @file    Cortex/CORTEXM_MPU/Src/stm32_mpu.c
  * @author  MCD Application Team
  * @brief   Access rights configuration using Cortex-M3 MPU regions.
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
#include "stm32_mpu.h"

/** @addtogroup STM32WBAxx_HAL_Examples
  * @{
  */

/** @addtogroup CORTEXM_MPU
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ARRAY_ADDRESS_START    (0x20002000UL)
#define ARRAY_SIZE             SRAM2_SIZE
#define ARRAY_REGION_NUMBER    MPU_REGION_NUMBER3
#define ARRAY_ATTRIBUTE_NUMBER MPU_ATTRIBUTES_NUMBER3


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if defined ( __ICCARM__ )
#pragma location=0x20002000
__no_init uint8_t PrivilegedReadOnlyArray[32];
#elif defined   (  __GNUC__  ) || defined(__ARMCC_VERSION)
uint8_t PrivilegedReadOnlyArray[32] __attribute__((section(".ROarraySection")));
#endif


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Configures the main MPU regions.
  * @param  None
  * @retval None
  */
void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;
  MPU_Attributes_InitTypeDef   MPU_AttributesInit;

  /* Disable MPU */
  HAL_MPU_Disable();

  /* Define cacheable memory via MPU */
  MPU_AttributesInit.Number             = EXAMPLE_RAM_ATTRIBUTE_NUMBER;
  MPU_AttributesInit.Attributes         = MPU_NOT_CACHEABLE;
  HAL_MPU_ConfigMemoryAttributes(&MPU_AttributesInit);

  /* Configure RAM region as Region Number 0, 64KB of size and R/W region */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = EXAMPLE_RAM_ADDRESS_START;
  MPU_InitStruct.LimitAddress = EXAMPLE_RAM_ADDRESS_START + EXAMPLE_RAM_SIZE - 1;
  MPU_InitStruct.AccessPermission = portMPU_REGION_READ_WRITE;
  MPU_InitStruct.AttributesIndex = EXAMPLE_RAM_ATTRIBUTE_NUMBER;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = EXAMPLE_RAM_REGION_NUMBER;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Configure FLASH region as REGION Number 1, 1M of size and R/W region */
  /* Define cacheable memory via MPU */
  MPU_AttributesInit.Number             = EXAMPLE_FLASH_ATTRIBUTE_NUMBER;
  MPU_AttributesInit.Attributes         = MPU_NOT_CACHEABLE;
  HAL_MPU_ConfigMemoryAttributes(&MPU_AttributesInit);

  MPU_InitStruct.BaseAddress = EXAMPLE_FLASH_ADDRESS_START;
  MPU_InitStruct.LimitAddress = EXAMPLE_FLASH_ADDRESS_START + EXAMPLE_FLASH_SIZE - 1;
  MPU_InitStruct.AttributesIndex = EXAMPLE_FLASH_ATTRIBUTE_NUMBER;
  MPU_InitStruct.Number = EXAMPLE_FLASH_REGION_NUMBER;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Configure Peripheral region as REGION Number 2, 512MB of size, R/W and Execute
  Never region */
  /* Define cacheable memory via MPU */
  MPU_AttributesInit.Number             = EXAMPLE_PERIPH_ATTRIBUTE_NUMBER;
  MPU_AttributesInit.Attributes         = MPU_NOT_CACHEABLE;
  HAL_MPU_ConfigMemoryAttributes(&MPU_AttributesInit);

  MPU_InitStruct.BaseAddress = EXAMPLE_PERIPH_ADDRESS_START;
  MPU_InitStruct.LimitAddress = EXAMPLE_PERIPH_ADDRESS_START + EXAMPLE_PERIPH_SIZE - 1;
  MPU_InitStruct.AttributesIndex = EXAMPLE_PERIPH_ATTRIBUTE_NUMBER;
  MPU_InitStruct.IsShareable = MPU_ACCESS_OUTER_SHAREABLE;
  MPU_InitStruct.Number = EXAMPLE_PERIPH_REGION_NUMBER;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable MPU (any access not covered by any enabled region will cause a fault) */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
  * @brief  This function configures the access right using Cortex-M33 MPU regions.
  * @param  None
  * @retval None
  */
void MPU_AccessPermConfig(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;
  MPU_Attributes_InitTypeDef   MPU_AttributesInit;

  /* Configure region for PrivilegedReadOnlyArray as REGION Number 3, 32byte and R
     only in privileged mode */
  /* Disable MPU */
  HAL_MPU_Disable();

  /* Define cacheable memory via MPU */
  MPU_AttributesInit.Number             = ARRAY_ATTRIBUTE_NUMBER;
  MPU_AttributesInit.Attributes         = MPU_NOT_CACHEABLE;
  HAL_MPU_ConfigMemoryAttributes(&MPU_AttributesInit);

  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = ARRAY_ADDRESS_START;
  MPU_InitStruct.LimitAddress = ARRAY_ADDRESS_START + ARRAY_SIZE - 1;
  MPU_InitStruct.AccessPermission = portMPU_REGION_PRIVILEGED_READ_ONLY;
  MPU_InitStruct.AttributesIndex = ARRAY_ATTRIBUTE_NUMBER;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = ARRAY_REGION_NUMBER;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable MPU (any access not covered by any enabled region will cause a fault) */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

  /* Read from PrivilegedReadOnlyArray. This will not generate error */
  (void)PrivilegedReadOnlyArray[0];

  /* Uncomment the following line to write to PrivilegedReadOnlyArray. This will
     generate error */
  /* PrivilegedReadOnlyArray[0] = 'e'; */
}

/**
  * @}
  */

/**
  * @}
  */
