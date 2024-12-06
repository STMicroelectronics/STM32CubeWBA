/**
  ******************************************************************************
  * @file    openthread_api_config_RCP.h
  * @author  MCD Application Team
  * @brief   This file contains configuration header for configuring OpenThread
  *          stack in RCP mode
  ******************************************************************************
  * @attention
 *
 * Copyright (c) 2018-2021 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef OPENTHREAD_API_CONFIG_RCP_H_
#define OPENTHREAD_API_CONFIG_RCP_H_

#ifndef OPENTHREAD_RCP
#define OPENTHREAD_RCP 1
#endif

/* Define to 1 if you want to enable support for multiple OpenThread
   instances. */
#define OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE 0

/* Define to 1 if you want to enable default log output. */
#define OPENTHREAD_CONFIG_LOG_OUTPUT  OPENTHREAD_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED

/* Define to 1 to enable MAC whitelist/blacklist feature. */
#define OPENTHREAD_CONFIG_MAC_FILTER_ENABLE 1


/* Define to 1 to enable logging on the platform */
#define OPENTHREAD_CONFIG_LOG_PLATFORM 1

/* Define Log Level */
#define OPENTHREAD_CONFIG_LOG_LEVEL OT_LOG_LEVEL_INFO

/* Define to 1 if you want to enable microsecond backoff timer implemented in platform. */
#define OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE 0

/* Define to 1 to enable Mle Link Metrics Initiator */
#define OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE 1

/* Define to 1 to enable Mle Link Metrics Subject */
#define OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE 1

#endif /* OPENTHREAD_API_CONFIG_RCP_H_ */
