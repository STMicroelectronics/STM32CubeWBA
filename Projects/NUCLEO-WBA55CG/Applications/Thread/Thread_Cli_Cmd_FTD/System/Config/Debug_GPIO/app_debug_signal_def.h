/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_debug_signal_def.h
  * @author  MCD Application Team
  * @brief   Real Time Debug module application signal definition
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* This header is part of the RTDebug module mechanisim.
 * In this file, application global debug signals can be
 * define by the user without modyfing files in the module core.
 *
 * There is no need to add this header file as include file
 * in the application files. This is handled by the module itself.
 */

#if (USE_RT_DEBUG_APP_APPE_INIT == 1)
  RT_DEBUG_APP_APPE_INIT,
#endif /* USE_RT_DEBUG_APP_APPE_INIT */

#if (USE_RT_DEBUG_APP_USER1 == 1)
  RT_DEBUG_APP_USER1,
#endif /* USE_RT_DEBUG_APP_USER1 */

#if (USE_RT_DEBUG_APP_USER2 == 1)
  RT_DEBUG_APP_USER2,
#endif /* USE_RT_DEBUG_APP_USER2 */

#if (USE_RT_DEBUG_APP_USER3 == 1)
  RT_DEBUG_APP_USER3,
#endif /* USE_RT_DEBUG_APP_USER3 */

#if (USE_RT_DEBUG_APP_USER4 == 1)
  RT_DEBUG_APP_USER4,
#endif /* USE_RT_DEBUG_APP_USER4 */

#if (USE_RT_DEBUG_APP_USER5 == 1)
  RT_DEBUG_APP_USER5,
#endif /* USE_RT_DEBUG_APP_USER5 */

#if (USE_RT_DEBUG_APP_USER6 == 1)
  RT_DEBUG_APP_USER6,
#endif /* USE_RT_DEBUG_APP_USER6 */

#if (USE_RT_DEBUG_APP_USER7 == 1)
  RT_DEBUG_APP_USER7,
#endif /* USE_RT_DEBUG_APP_USER7 */

#if (USE_RT_DEBUG_APP_USER8 == 1)
  RT_DEBUG_APP_USER8,
#endif /* USE_RT_DEBUG_APP_USER8 */

#if (USE_RT_DEBUG_APP_USER9 == 1)
  RT_DEBUG_APP_USER9,
#endif /* USE_RT_DEBUG_APP_USER9 */

#if (USE_RT_DEBUG_APP_USER10 == 1)
  RT_DEBUG_APP_USER10,
#endif /* USE_RT_DEBUG_APP_USER10 */
