/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm_logging.h
  * @author  MCD Application Team
  * @brief   Application header file for logging
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

#ifndef STM_LOGGING_H_
#define STM_LOGGING_H_

#include "app_conf.h"

#define LOG_LEVEL_NONE  0  /* None     */
#define LOG_LEVEL_CRIT  1U  /* Critical */
#define LOG_LEVEL_WARN  2U  /* Warning  */
#define LOG_LEVEL_INFO  3U  /* Info     */
#define LOG_LEVEL_DEBG  4U  /* Debug    */

#define APP_DBG_FULL(level, region, ...)                                                    \
  {                                                                                         \
    if (APPLI_PRINT_FILE_FUNC_LINE == 1U)                                                   \
    {                                                                                       \
        printf("\r\n[%s][%s][%d] ", DbgTraceGetFileName(__FILE__),__FUNCTION__,__LINE__);   \
    }                                                                                       \
    logApplication(level, region, __VA_ARGS__);                                             \
  }
  
/*
#define APP_DBG(...)                                                                        \
  {                                                                                         \
    if (APPLI_PRINT_FILE_FUNC_LINE == 1U)                                                   \
    {                                                                                       \
        printf("\r\n[%s][%s][%d] ", DbgTraceGetFileName(__FILE__),__FUNCTION__,__LINE__);   \
    }                                                                                       \
    logApplication(LOG_LEVEL_NONE, APPLI_LOG_REGION_GENERAL, __VA_ARGS__);                  \
  }
*/  

#define ADV_TRACE_TIMESTAMP_ENABLE          0U

/* New implementation using stm32_adv_trace */
#ifndef APP_DBG
#define APP_DBG(...)                                                                            \
{                                                                                               \
  UTIL_ADV_TRACE_COND_FSend(VLEVEL_L, LOG_REGION_APP, ADV_TRACE_TIMESTAMP_ENABLE, __VA_ARGS__); \
}
#endif /* APP_DBG */

/**
 * This enumeration represents log regions.
 *
 */
typedef enum
{
  APPLI_LOG_REGION_GENERAL                    = 1U,  /* General                 */
  APPLI_LOG_REGION_OPENTHREAD_API             = 2U,  /* OpenThread API          */
  APPLI_LOG_REGION_OT_API_LINK                = 3U,  /* OpenThread Link API     */
  APPLI_LOG_REGION_OT_API_INSTANCE            = 4U,  /* OpenThread Instance API */
  APPLI_LOG_REGION_OT_API_MESSAGE             = 5U   /* OpenThread Message API  */
} appliLogRegion_t;

typedef uint8_t appliLogLevel_t;

void logApplication(appliLogLevel_t aLogLevel, appliLogRegion_t aLogRegion, const char *aFormat, ...);
const char *DbgTraceGetFileName( const char *fullpath );

#endif  /* STM_LOGGING_H_ */
