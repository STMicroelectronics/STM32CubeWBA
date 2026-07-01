/**
  ******************************************************************************
  * @file    rng_alt.c
  * @author  MCD Application Team
  * @brief   Low Level Interface module to use STM32 RNG Ip as a source of entropy
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
  *  ******************************************************************************
  */

#include "entropy_hardware_alt.h"
#include "stm32wbaxx_ll_bus.h"
#include "utilities_conf.h"

#if defined (MBEDTLS_HAL_ENTROPY_HARDWARE_ALT)
extern void Error_Handler(void);

static RNG_HandleTypeDef handle;
static uint8_t users = 0U;

__weak void RNG_KERNEL_CLK_ON(void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the RNG_KERNEL_CLK_ON could be implemented in the user file
  */   
  LL_RCC_HSI_Enable();
  while(LL_RCC_HSI_IsReady() == 0)
  {
    LL_RCC_HSI_Enable();
  }
}

__weak void RNG_KERNEL_CLK_OFF(void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the RNG_KERNEL_CLK_OFF could be implemented in the user file
  */
}

void HW_RNG_EnableClock(uint8_t user_mask)
{
 
  RNG_KERNEL_CLK_ON();

  UTILS_ENTER_CRITICAL_SECTION();

  if (users == 0U)
  {
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_RNG);
  }

  users |= user_mask;
  
  UTILS_EXIT_CRITICAL_SECTION();
}

void HW_RNG_DisableClock(uint8_t user_mask)
{
  
  UTILS_ENTER_CRITICAL_SECTION( );
  
  users &= ~user_mask;

  if (users == 0U)
  {
    LL_AHB2_GRP1_DisableClock(LL_AHB2_GRP1_PERIPH_RNG);
  }

  UTILS_EXIT_CRITICAL_SECTION( );

  RNG_KERNEL_CLK_OFF();
}

int RNG_Init(void)
{
  uint32_t dummy;
  RNG_ConfigTypeDef rng_conf;

  /* Select RNG clock source */
  __HAL_RCC_RNG_CONFIG(RNGCLKSOURCE_HSI);

  /* RNG Peripheral clock enable */
  HW_RNG_EnableClock(0x1);
  
  /* Initialize RNG instance */
  handle.Instance = RNG;
  handle.State = HAL_RNG_STATE_RESET;
  handle.Lock = HAL_UNLOCKED;

  if (HAL_RNG_Init(&handle) != HAL_OK)
  {
    return -1;
  }

  /* Set NIST configuration for better security */
  rng_conf.Config1 = 0x0FUL;
  rng_conf.Config2 = 0UL;
  rng_conf.Config3 = 0x0DUL;
  rng_conf.ClockDivider = RNG_CLKDIV_BY_1;
  rng_conf.NistCompliance = RNG_NIST_COMPLIANT;
  rng_conf.AutoReset = RNG_ARDIS_ENABLE;
  rng_conf.HealthTest = 0x0000AEC7UL;
  if (HAL_RNGEx_SetConfig(&handle, &rng_conf) != HAL_OK)
  {
    return -1;
  }

  /* first random number generated after setting the RNGEN bit should not be used */
  if (HAL_RNG_GenerateRandomNumber(&handle, &dummy) != HAL_OK)
  {
    return -1;
  }

  return 0;
}

int RNG_GetBytes(uint8_t *output, size_t length, size_t *output_length)
{
  int32_t ret = 0;
  uint8_t try = 0U;
  __IO uint8_t random[4];
  *output_length = 0;

  /* Get Random byte */
  while ((*output_length < length) && (ret == 0))
  {
    if (HAL_RNG_GenerateRandomNumber(&handle, (uint32_t *)random) != HAL_OK)
    {
      /* retry when random number generated are not immediately available */
      if (try < 3U)
      {
        try++;
      }
      else
      {
        ret = -1;
      }
    }
    else
    {
      for (uint8_t i = 0U; (i < 4U) && (*output_length < length) ; i++)
      {
        *output++ = random[i];
        *output_length += 1U;
        random[i] = 0;
      }
    }
  }
  /* Just be extra sure that we didn't do it wrong */
  if ((__HAL_RNG_GET_FLAG(&handle, (RNG_FLAG_CECS | RNG_FLAG_SECS))) != 0)
  {
    *output_length = 0;
  }

  return ret;
}

int RNG_DeInit(void)
{
  /* Disable the RNG peripheral */
  if (HAL_RNG_DeInit(&handle) != HAL_OK)
  {
    return -1;
  }
  /* RNG Peripheral clock disable */
  HW_RNG_DisableClock(0x1);
  
  return 0;
}

/*  interface for mbed-crypto */
int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen)
{
  UNUSED(data);

  if (RNG_Init() != 0)
  {
    return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
  }

  if (RNG_GetBytes(output, len, olen) != 0)
  {
    return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
  }

  if (RNG_DeInit() != 0)
  {
    return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
  }

  if (*olen != len)
  {
    return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
  }

  return 0;
}

#endif /* MBEDTLS_HAL_ENTROPY_HARDWARE_ALT */

