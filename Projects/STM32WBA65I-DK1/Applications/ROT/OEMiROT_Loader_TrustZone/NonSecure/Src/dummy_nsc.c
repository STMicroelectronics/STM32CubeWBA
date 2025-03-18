/**
  ******************************************************************************
  * @file    dummy_nsc.c
  * @author  MCD Application Team
  * @brief   This file contains the non-secure dummy callback
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
  ******************************************************************************
  */

#include "loader_region_defs.h"
void dummy(void);
void dummy(void)
{
#ifdef MCUBOOT_PRIMARY_ONLY
#error
#endif
}
