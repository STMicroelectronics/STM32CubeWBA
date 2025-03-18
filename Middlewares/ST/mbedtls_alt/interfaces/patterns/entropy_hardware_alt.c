/**
  ******************************************************************************
  * @file    entropy_hardware_alt.c
  * @author  MCD Application Team
  * @brief   Entropy Hardware module to use STM32 RNG Ip as a source of entropy
  *
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
  *  ******************************************************************************
  */

#include "entropy_hardware_alt.h"
#if defined (MBEDTLS_HAL_ENTROPY_HARDWARE_ALT)
extern void Error_Handler(void);

static RNG_HandleTypeDef handle;
static uint8_t users = 0U;
uint8_t init_ctx = 0U;

#define COMPILER_BARRIER() __ASM __IO("" : : : "memory")

static uint8_t atomic_incr_u8(__IO uint8_t *valuePtr, uint8_t delta)
{
  COMPILER_BARRIER();
  uint8_t newValue;
  do
  {
    newValue = __LDREXB(valuePtr) + delta;
  } while (__STREXB(newValue, valuePtr));
  COMPILER_BARRIER();
  return newValue;
}

void RNG_Init(void)
{
  uint32_t dummy;
  RNG_ConfigTypeDef rng_conf;

  /*  We're only supporting a single user of RNG */
  if (atomic_incr_u8(&users, 1U) > 1U)
  {
    Error_Handler();
  }

  /* Select RNG clock source */
  __HAL_RCC_RNG_CONFIG(RNGCLKSOURCE_HSI);

  /* RNG Peripheral clock enable */
  __HAL_RCC_RNG_CLK_ENABLE();

  /* Initialize RNG instance */
  handle.Instance = RNG;
  handle.State = HAL_RNG_STATE_RESET;
  handle.Lock = HAL_UNLOCKED;

  if (HAL_RNG_Init(&handle) != HAL_OK)
  {
    Error_Handler();
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
    Error_Handler();
  }

  /* first random number generated after setting the RNGEN bit should not be used */
  HAL_RNG_GenerateRandomNumber(&handle, &dummy);
}

void RNG_GetBytes(uint8_t *output, size_t length, size_t *output_length)
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
}

void RNG_DeInit(void)
{
  /*Disable the RNG peripheral */
  HAL_RNG_DeInit(&handle);
  /* RNG Peripheral clock disable - assume we're the only users of RNG  */
  __HAL_RCC_RNG_CLK_DISABLE();

  users = 0;
}

/*  interface for mbed-crypto */
int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen)
{
  UNUSED(data);

  if (init_ctx == 0U)
  {
    RNG_Init();
    init_ctx = 1U;
  }

  RNG_GetBytes(output, len, olen);

  if (*olen != len)
  {
    return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
  }

  return 0;
}
#endif /* MBEDTLS_HAL_ENTROPY_HARDWARE_ALT */

