/**
  ******************************************************************************
  * @file    rng_alt.c
  * @author  MCD Application Team
  * @brief   Low Level Interface module to use STM32 RNG for random generation 
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

#include "stm32wbaxx_ll_bus.h"
#include "stm32wbaxx_ll_rng.h"
#include "utilities_conf.h"
#include "psa/crypto.h"
#include "rng_alt.h"
#include "hw.h"


/* Private types definition */
/* RNG pool */
typedef struct
{
  uint32_t pool[CFG_HW_RNG_POOL_SIZE];
  uint16_t rd_idx;                     /* Next readable index */
  uint16_t wr_idx;                     /* Next writable index */
  uint16_t ready_count;                /* Number of words available to read */
  uint8_t  filling_process;            /* Pool being filled */
  uint8_t threshold;                   /* Refill watermark */
} RNGPool_t;

/* Private global variables */
static uint8_t users = 0U;
static RNGPool_t RngPool;

/* Private prototypes */
static int RNG_GetWords(uint32_t *output, size_t length, size_t *output_length);
static int RNG_Init(void);
static int RNG_DeInit(void);
static int _RNG_RecoverSeedError(void);

/* Private defines */
#define RNG_TIMEOUT_VALUE     2U

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

int HW_RNG_Process(void)
{
  RNGPool_t *pool_p = &RngPool;
  uint16_t free_count = 0;
  uint16_t generated_count = 0;
  size_t first_chunk;
  size_t second_chunk;
  size_t output_length;
  int ret = 0;

  UTILS_ENTER_CRITICAL_SECTION();
  if (pool_p->filling_process == 0U)
  {
    /* Single producer scheme */
    free_count = (uint16_t)CFG_HW_RNG_POOL_SIZE - pool_p->ready_count;
    pool_p->filling_process = 1U;
  }
  UTILS_EXIT_CRITICAL_SECTION();
  
  /* Check if worth refilling */
  if (free_count > (uint16_t)(CFG_HW_RNG_POOL_SIZE - pool_p->threshold))
  {
    ret = RNG_Init();
    if (ret == 0)
    {
      /* Split generation in two chunks when generation wraps around ring buffer end */
      first_chunk = CFG_HW_RNG_POOL_SIZE - pool_p->wr_idx;
      if (first_chunk > (size_t)free_count)
      {
        first_chunk = (size_t)free_count;
      }
      second_chunk = (size_t)free_count - first_chunk;
  
      if (first_chunk > 0U)
      {
        ret = RNG_GetWords(&pool_p->pool[pool_p->wr_idx], first_chunk, &output_length);
        if (ret == 0)
        {
          generated_count = (uint16_t)output_length;
        }
      }

      if ((ret == 0) && (second_chunk > 0U))
      {
        ret = RNG_GetWords(&pool_p->pool[0], second_chunk, &output_length);
        if (ret == 0)
        {
          generated_count += (uint16_t)output_length;
        }
      }

      /* Publish generated words */
      if (generated_count != 0U)
      {
        UTILS_ENTER_CRITICAL_SECTION();
        pool_p->ready_count += generated_count;
        pool_p->wr_idx = (pool_p->wr_idx + generated_count) % CFG_HW_RNG_POOL_SIZE;
        UTILS_EXIT_CRITICAL_SECTION();
      }

      if (RNG_DeInit() != 0)
      {
        ret = -1;
      }
    }
  }
  
  {
    UTILS_ENTER_CRITICAL_SECTION();
    pool_p->filling_process = 0U;
    UTILS_EXIT_CRITICAL_SECTION(); 
  }
  
  if (ret != 0)
  {
    assert_param(1);
  }
  
  return ret;
}


void HW_RNG_SetPoolThreshold(uint8_t threshold)
{
  RNGPool_t *pool_p = &RngPool;
  
  if(threshold < HW_RNG_POOL_SIZE)
  {
    pool_p->threshold = threshold;
  }
}

/* PSA API implementation for external RNG */
psa_status_t mbedtls_psa_external_get_random(mbedtls_psa_external_random_context_t *context, uint8_t *output, size_t output_size, size_t *output_length)
{ 
  RNGPool_t *pool_p = &RngPool;
  uint32_t value;
  size_t word_size = output_size >> 2;
  uint32_t i;
  psa_status_t ret = PSA_SUCCESS;
  
  UNUSED(context);
  
  *output_length = 0;
  /* Check 32bits alignment */
  if ((output_size % 4) != 0)
  {
    word_size++;
  }
  while (word_size--)
  {
    UTILS_ENTER_CRITICAL_SECTION();

    if(pool_p->ready_count == 0)
    {
      value = ~pool_p->pool[word_size & (CFG_HW_RNG_POOL_SIZE - 1)];
      ret = PSA_ERROR_HARDWARE_FAILURE;
    }
    else
    {
      value = pool_p->pool[pool_p->rd_idx];
      pool_p->ready_count--;
      pool_p->rd_idx = (pool_p->rd_idx + 1) % CFG_HW_RNG_POOL_SIZE;
    }

    UTILS_EXIT_CRITICAL_SECTION();
    for (i = 0; (i < 4) && (*output_length < output_size); i++)
    {
      *(output++) = ((uint8_t*)&value)[i];
      (*output_length)++;
    }
  }

  /* Trigger refill (background task) */
  HWCB_RNG_Process();
  
  return ret;
}

/* Private functions */
static int RNG_Init(void)
{
  LL_RCC_SetRNGClockSource(LL_RCC_RNG_CLKSOURCE_HSI);
  HW_RNG_EnableClock(1);
  LL_RNG_Disable(RNG);
  while(LL_RNG_IsEnabled(RNG));
 
  /* Recommended value for NIST compliance, refer to application note AN4230 */
  /* Using LL macros to set these values is not convenient as it's split 
     in 3 parts and need some bit polling at each step to check completion.
     So, for efficiency, register direct access */
  WRITE_REG(RNG->CR, RNG_CR_NIST_VALUE | RNG_CR_CONDRST | RNG_CED_DISABLE);
  /* Recommended value for NIST compliance, refer to application note AN4230 */
 
  LL_RNG_DisableClkErrorDetect(RNG);
  LL_RNG_DisableCondReset(RNG);

/* RNG busy flag is not available in STM32WBA5xxx */
#if defined(RNG_SR_BUSY)
  /* wait until RNG_SR_BUSY (bit 4) is cleared */
  while(RNG->SR & (1 << 4));
#endif 
  
  while(LL_RNG_IsEnabledCondReset(RNG));

  LL_RNG_SetHealthConfig(RNG,RNG_HTCR_NIST_VALUE);

  LL_RNG_Enable(RNG);
  /* Wait for data to be ready */
  while(!LL_RNG_IsActiveFlag_DRDY(RNG)); 
  
  return 0;
}

static int RNG_GetWords(uint32_t *output, size_t length, size_t *output_length)
{
  int ret = 0;
  *output_length = 0;

  /* check for RNG clock error */
  if (LL_RNG_IsActiveFlag_CECS(RNG))
  {
    /* Clear RNG clock error interrupt status flags */
    LL_RNG_ClearFlag_CEIS(RNG);
    ret = -1;
  }
  /* check for RNG seed error */
  else if (LL_RNG_IsActiveFlag_SEIS(RNG))
  {
    ret = _RNG_RecoverSeedError();
  }
  
  if (ret == 0)
  {
    /* Get random word */
    while (*output_length < length)
    {
      if (LL_RNG_IsActiveFlag_DRDY(RNG))
      {
        *output++ = LL_RNG_ReadRandData32(RNG);
        *output_length += 1U;
      }
    }
  }

  return ret;
}

static int RNG_DeInit(void)
{  
  /* Disable the RNG peripheral */
  LL_RNG_Disable(RNG);

  /* RNG Peripheral clock disable */
  HW_RNG_DisableClock(0x1);
  
  return 0;
}

/* HAL_RNGEx_RecoverSeedError() adaptation */
#if defined(STM32WBA5xxx)
static int _RNG_RecoverSeedError(void)
{
  volatile uint32_t count = 0U;

  /* Check if seed error current status (SECS)is set */
  if (READ_BIT(RNG->SR, RNG_FLAG_SECS) != (RNG_FLAG_SECS))
  {
    /* RNG performed the reset automatically (auto-reset) */
    /* Clear bit SEIS */
    LL_RNG_ClearFlag_SEIS(RNG);
  }
  else  /* Sequence to fully recover from a seed error */
  {
    /* Writing bit CONDRST=1 */
    LL_RNG_EnableCondReset(RNG);
    /* Writing bit CONDRST=0 */
    LL_RNG_DisableCondReset(RNG);

    /* Wait for conditioning reset process to be completed */
    count = RNG_TIMEOUT_VALUE;
    do
    {
      count-- ;
      if (count == 0U)
      {
        return HW_RNG_NOISE_ERROR;
      }
    } while (READ_BIT(RNG->CR, RNG_CR_CONDRST) == (RNG_CR_CONDRST));

    if (LL_RNG_IsActiveFlag_SEIS(RNG) != 0UL)
    {
      /* Clear bit SEIS */
      LL_RNG_ClearFlag_SEIS(RNG);
    }

    /* Wait for SECS to be cleared */
    count = RNG_TIMEOUT_VALUE;
    do
    {
      count-- ;
      if (count == 0U)
      {
        return HW_RNG_NOISE_ERROR;
      }
    } while (READ_BIT(RNG->SR, RNG_FLAG_SECS) == (RNG_FLAG_SECS));
  }
 
  return 0;
}

#elif defined(STM32WBA2xxx) || defined(STM32WBA6xxx)

static int _RNG_RecoverSeedError(void)
{
  uint32_t timeout;
  uint32_t htsr_temp = 0U;
  uint32_t htsr_previous_temp = 0U;
  uint32_t htsr_count = 0U;
  uint32_t nsmr_temp = 0U;
  uint32_t tickstart1 = 0U;
  uint32_t tickstart2 = 0U;
  uint32_t tickstart3 = 0U;
  uint32_t oscillators_count = 0U;
  uint32_t config_b_fewer_than_6_osc_count = 0U;
  uint8_t count = 0U;

  /* timeout here is an empirical value */
  timeout = (1UL + ((1UL << (READ_BIT(RNG->CR, RNG_CR_CLKDIV) >> 16UL)) * RNG_TIMEOUT_VALUE / 8UL));

  tickstart1 = HAL_GetTick();

  /* Check if seed error current status indicates no error and auto-reset succeeded */
  if (LL_RNG_IsActiveFlag_SECS(RNG) == 0U)
  {
    /* Clear SEIS flag when automatic reset is activated */
    LL_RNG_ClearFlag_SEIS(RNG);
  }
  else  /* Sequence to fully recover from a seed error */
  {
    do
    {
      if (LL_RNG_IsActiveFlag_SECS(RNG) == 0U)
      {
        break;
      }
      /* Read oscillator status registers combined */
      htsr_temp = LL_RNG_GetHealthTestStatus(RNG, 0U);
      htsr_temp |= LL_RNG_GetHealthTestStatus(RNG, 1U);
      if (htsr_temp > 0U)
      {
        /* If any oscillator status bits overlap with previous status, increment counter */
        if ((htsr_temp & htsr_previous_temp) != 0U)
        {
          htsr_count++;
        }

        if (htsr_count > 3U)
        {
          /* if the same repetitive or adaptative error is detected 3 times */
          nsmr_temp = LL_RNG_GetNoiseSourceMask(RNG);

          /* deactivate the same osc in each triple oscillator (Mask oscillators with the seed error by
          clearing bits shifted right by 1) */       
          nsmr_temp = nsmr_temp & ~(htsr_temp >> 1U);

          /* Count the number of active oscillators in nsmr */
          oscillators_count = 0U;
          for (count = 0U; count < 9U; count++)
          {
            if (((nsmr_temp >> count) & 0x1U) != 0U)
            {
              /* increment count1 for each 1 in nsmr */
              oscillators_count++;
            }
          }

          if (oscillators_count < 6U)
          {
            /* If fewer than 6 oscillators remain active, unmask all oscillators --> Reset masking */
            nsmr_temp = LL_RNG_GetOscNoiseSrc(RNG, LL_RNG_NOISE_SRC_1 | LL_RNG_NOISE_SRC_2 | LL_RNG_NOISE_SRC_3);
            htsr_previous_temp = 0;
            htsr_count = 0U;
            if ((RNG->CR  & RNG_CR_CLKDIV_Msk) < ((uint32_t)RNG_CAND_NIST_CR_VALUE & RNG_CR_CLKDIV_Msk))
            {
              config_b_fewer_than_6_osc_count++;
            }
          }

          if (config_b_fewer_than_6_osc_count > 2U)
          {
            /* Reset RNG condition */
            WRITE_REG(RNG->CR, (RNG_CR_CONDRST_Msk | (uint32_t)RNG_CAND_NIST_CR_VALUE));

            /* Update mask register with new oscillator mask */
            LL_RNG_SetNoiseSourceMask(RNG, nsmr_temp);

            /* Clear condition reset bit to resume operation */
            LL_RNG_DisableCondReset(RNG);
          }
          else
          {
            /* Reset RNG condition */
            WRITE_REG(RNG->CR, (RNG->CR & ~RNG_CR_RNGEN_Msk) | RNG_CR_CONDRST_Msk);

            /* Update mask register with new oscillator mask */
            LL_RNG_SetNoiseSourceMask(RNG, nsmr_temp);

            /* Clear condition reset bit to resume operation */
            LL_RNG_DisableCondReset(RNG);
          }
        }
        else
        {
          /* Briefly toggle conditional reset to recover RNG */
          WRITE_REG(RNG->CR, (RNG->CR & ~RNG_CR_RNGEN_Msk) | RNG_CR_CONDRST_Msk);

          /* Unmask all oscillators to find another working condition */
          LL_RNG_SetNoiseSourceMask(RNG, LL_RNG_GetOscNoiseSrc(RNG, LL_RNG_OSC_1 | LL_RNG_OSC_2 | LL_RNG_OSC_3));
          LL_RNG_DisableCondReset(RNG);
        }

        /* Wait until RNG is not busy */
        tickstart2 = HAL_GetTick();
        do
        {
          if ((HAL_GetTick() - tickstart2) > RNG_TIMEOUT_VALUE)
          {
            /* New check to avoid false timeout detection in case of preemption */
            return HW_RNG_NOISE_ERROR;
          }
        } while (READ_BIT(RNG->SR, RNG_SR_BUSY) == (RNG_SR_BUSY));

        /* No timeout */
        tickstart3 = HAL_GetTick();
        do
        {
          if (LL_RNG_IsActiveFlag_DRDY(RNG) != 0UL)
          {
            break;
          }
          if ((HAL_GetTick() - tickstart3) > timeout)
          {
            /* New check to avoid false timeout detection in case of preemption */
            if (LL_RNG_IsActiveFlag_DRDY(RNG) == 0UL)
            {
              if (LL_RNG_IsActiveFlag_SECS(RNG) == 0UL)
              {
                return HW_RNG_NOISE_ERROR;
              }
            }
          }
        } while (LL_RNG_IsActiveFlag_SECS(RNG) == 0UL);

        /* Accumulate seed error status bits */
        htsr_previous_temp = htsr_previous_temp | htsr_temp;
      }
    } while ((HAL_GetTick() - tickstart1) <= timeout);
  }

  /* Check if seed error current status (SECS)is set */
  if (LL_RNG_IsActiveFlag_SECS(RNG) != 0U)
  {
    return HW_RNG_NOISE_ERROR;
  }

  return 0;  
}

#else
#error "Unsupported STM32WBA series for rng_alt.c"
#endif