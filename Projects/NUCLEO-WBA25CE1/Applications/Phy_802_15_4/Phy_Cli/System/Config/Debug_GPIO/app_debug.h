/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_debug.h
  * @author  MCD Application Team
  * @brief   Real Time Debug module application APIs and signal table
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
/* USER CODE END Header */
#ifndef APP_DEBUG_H
#define APP_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "RTDebug.h"

/***************************************************/
/** Specific application debug signals definition **/
/***************************************************/
typedef enum {
  APP_APPE_INIT,
  /* Ready-to-use user debug signals.
     Enable one in debug_config.h (USE_RT_DEBUG_APP_USERx) and assign its GPIO.
     A friendly alias can be defined in the application code, e.g.
       #define My_Debug_GPIO  APP_USER1
     then used as APP_DEBUG_SIGNAL_SET(My_Debug_GPIO). */
  APP_USER1,
  APP_USER2,
  APP_USER3,
  APP_USER4,
  APP_USER5,
  APP_USER6,
  APP_USER7,
  APP_USER8,
  APP_USER9,
  APP_USER10,
} app_debug_signal_t;

#if(CFG_RT_DEBUG_GPIO_MODULE == 1)

/************************************/
/** Application local signal table **/
/************************************/
static const rt_debug_signal_t app_debug_table[] = {
#if (USE_RT_DEBUG_APP_APPE_INIT == 1)
  [APP_APPE_INIT] = RT_DEBUG_APP_APPE_INIT,
#else
  [APP_APPE_INIT] = RT_DEBUG_SIGNAL_UNUSED,
#endif /* USE_RT_DEBUG_APP_APPE_INIT */
#if (USE_RT_DEBUG_APP_USER1 == 1)
  [APP_USER1] = RT_DEBUG_APP_USER1,
#else
  [APP_USER1] = RT_DEBUG_SIGNAL_UNUSED,
#endif /* USE_RT_DEBUG_APP_USER1 */
#if (USE_RT_DEBUG_APP_USER2 == 1)
  [APP_USER2] = RT_DEBUG_APP_USER2,
#else
  [APP_USER2] = RT_DEBUG_SIGNAL_UNUSED,
#endif /* USE_RT_DEBUG_APP_USER2 */
#if (USE_RT_DEBUG_APP_USER3 == 1)
  [APP_USER3] = RT_DEBUG_APP_USER3,
#else
  [APP_USER3] = RT_DEBUG_SIGNAL_UNUSED,
#endif /* USE_RT_DEBUG_APP_USER3 */
#if (USE_RT_DEBUG_APP_USER4 == 1)
  [APP_USER4] = RT_DEBUG_APP_USER4,
#else
  [APP_USER4] = RT_DEBUG_SIGNAL_UNUSED,
#endif /* USE_RT_DEBUG_APP_USER4 */
#if (USE_RT_DEBUG_APP_USER5 == 1)
  [APP_USER5] = RT_DEBUG_APP_USER5,
#else
  [APP_USER5] = RT_DEBUG_SIGNAL_UNUSED,
#endif /* USE_RT_DEBUG_APP_USER5 */
#if (USE_RT_DEBUG_APP_USER6 == 1)
  [APP_USER6] = RT_DEBUG_APP_USER6,
#else
  [APP_USER6] = RT_DEBUG_SIGNAL_UNUSED,
#endif /* USE_RT_DEBUG_APP_USER6 */
#if (USE_RT_DEBUG_APP_USER7 == 1)
  [APP_USER7] = RT_DEBUG_APP_USER7,
#else
  [APP_USER7] = RT_DEBUG_SIGNAL_UNUSED,
#endif /* USE_RT_DEBUG_APP_USER7 */
#if (USE_RT_DEBUG_APP_USER8 == 1)
  [APP_USER8] = RT_DEBUG_APP_USER8,
#else
  [APP_USER8] = RT_DEBUG_SIGNAL_UNUSED,
#endif /* USE_RT_DEBUG_APP_USER8 */
#if (USE_RT_DEBUG_APP_USER9 == 1)
  [APP_USER9] = RT_DEBUG_APP_USER9,
#else
  [APP_USER9] = RT_DEBUG_SIGNAL_UNUSED,
#endif /* USE_RT_DEBUG_APP_USER9 */
#if (USE_RT_DEBUG_APP_USER10 == 1)
  [APP_USER10] = RT_DEBUG_APP_USER10,
#else
  [APP_USER10] = RT_DEBUG_SIGNAL_UNUSED,
#endif /* USE_RT_DEBUG_APP_USER10 */
};

#endif /* CFG_RT_DEBUG_GPIO_MODULE */

/**************************************/
/** Application debug API definition **/
/**************************************/
void APP_DEBUG_SIGNAL_SET(app_debug_signal_t signal);
void APP_DEBUG_SIGNAL_RESET(app_debug_signal_t signal);
void APP_DEBUG_SIGNAL_TOGGLE(app_debug_signal_t signal);

/*******************************/
/** Debug GPIO Initialization **/
/*******************************/
void RT_DEBUG_GPIO_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_DEBUG_H */
