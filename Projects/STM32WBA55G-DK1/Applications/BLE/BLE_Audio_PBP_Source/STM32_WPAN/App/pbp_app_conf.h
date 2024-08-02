/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    pbp_app_conf.h
  * @author  MCD Application Team
  * @brief   Application configuration file for pbp_app.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PBP_APP_CONF_H
#define __PBP_APP_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Private includes ----------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
#define BAP_BROADCAST_SOURCE_ID                         (0x000001) /* 3 Bytes Source ID */

/*Audio Profiles Roles configuration*/
#define APP_BAP_ROLE_UNICAST_SERVER_SUPPORT             (0u)
#define APP_BAP_ROLE_UNICAST_CLIENT_SUPPORT             (0u)
#define APP_BAP_ROLE_BROADCAST_SOURCE_SUPPORT           (1u)
#define APP_BAP_ROLE_BROADCAST_SINK_SUPPORT             (0u)
#define APP_BAP_ROLE_SCAN_DELEGATOR_SUPPORT             (0u)
#define APP_BAP_ROLE_BROADCAST_ASSISTANT_SUPPORT        (0u)


#define APP_CCP_ROLE_SERVER_SUPPORT                     (0u)
#define APP_CCP_ROLE_CLIENT_SUPPORT                     (0u)

#define APP_MCP_ROLE_SERVER_SUPPORT                     (0u)
#define APP_MCP_ROLE_CLIENT_SUPPORT                     (0u)

#define APP_VCP_ROLE_CONTROLLER_SUPPORT                 (0u)
#define APP_VCP_ROLE_RENDERER_SUPPORT                   (0u)

#define APP_MICP_ROLE_CONTROLLER_SUPPORT                (0u)
#define APP_MICP_ROLE_DEVICE_SUPPORT                    (0u)

#define APP_CSIP_ROLE_SET_COORDINATOR_SUPPORT           (0u)
#define APP_CSIP_ROLE_SET_MEMBER_SUPPORT                (0u)

/*BAP Configuration Settings*/
#define MAX_NUM_UCL_SNK_ASE                             (0u)
#define MAX_NUM_UCL_SRC_ASE                             (0u)
#define MAX_NUM_USR_SNK_ASE                             (0u)
#define MAX_NUM_USR_SRC_ASE                             (0u)
#define MAX_NUM_CIG                                     (0u)    /* Maximum number of CIGes */
#define MAX_NUM_CIS_PER_CIG                             (0u)    /* Maximum number of CISes per CIG */
#define MAX_NUM_BIG                                     (1u)    /* Maximum number of BIGes */
#define MAX_NUM_BIS_PER_BIG                             (2u)    /* Maximum number of BISes per BIG */
#define PBP_MAX_BIS                                     (2u)
#define MAX_METADATA_LEN                                (50u)


/* These delays refers to the time at which the audio signal passes through an
 * audio interface (such an electroacoustic transducer ) to or from
 * the Codec interface.
 * These delays are a subpart of the Presentation Delay as described in chapter 7
 * of the Basic Audio Profile specification
 * The delay unit is us
 */
#define APP_DELAY_SRC_MIN                       (100)                           /* DMA delay for transmitting to
                                                                                 * SAI peripheral (for more precision,
                                                                                 * should add ADC delay)
                                                                                 */
#define APP_DELAY_SRC_MAX                       (APP_DELAY_SRC_MIN + 0u)        /* No extra buffering of audio data*/



/* Exported types ------------------------------------------------------------*/


/* External variables --------------------------------------------------------*/

/* Exported macros ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ---------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /*__PBP_APP_CONF_H */

