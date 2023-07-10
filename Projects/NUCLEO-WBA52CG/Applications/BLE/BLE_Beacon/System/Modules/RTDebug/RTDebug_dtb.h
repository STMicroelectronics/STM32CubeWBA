/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    RTDebug_dtb.h
  * @author  MCD Application Team
  * @brief   Real Time Debug module API declaration for DTB usage
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
#ifndef RT_DEBUG_DTB_H
#define RT_DEBUG_DTB_H

/**
 * The DEBUG DTB interface is INTENDED TO BE USED ONLY ON REQUEST FROM ST SUPPORT.
 * It provides HW signals from RF PHY activity.
 */

#include "app_conf.h"

#if(RT_DEBUG_DTB == 1)
void RT_DEBUG_DTBConfig(void);
void RT_DEBUG_DTBInit(void);
#endif /* RT_DEBUG_DTB */

#endif /* RT_DEBUG_DTB_H */

