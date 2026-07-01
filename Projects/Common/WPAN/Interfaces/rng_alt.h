/**
  ******************************************************************************
  * @file    rng_alt.h
  * @author  MCD Application Team
  * @brief   Header for rng_alt.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef RNG_ALT_H
#define RNG_ALT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include "mbedtls/entropy.h"

/* Exported functions ------------------------------------------------------- */

void HW_RNG_EnableClock(uint8_t user_mask);
void HW_RNG_DisableClock(uint8_t user_mask);

#ifdef __cplusplus
}
#endif

#endif /* ENTROPY_HARDWARE_ALT_H */
