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
#include "bap_bufsize.h"
/* Private includes ----------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/*Audio Profiles Roles configuration*/
#define APP_BAP_ROLE_UNICAST_SERVER_SUPPORT             (0u)
#define APP_BAP_ROLE_UNICAST_CLIENT_SUPPORT             (0u)
#define APP_BAP_ROLE_BROADCAST_SOURCE_SUPPORT           (0u)
#define APP_BAP_ROLE_BROADCAST_SINK_SUPPORT             (1u)
#define APP_BAP_ROLE_SCAN_DELEGATOR_SUPPORT             (1u)
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


#define SETUP_SPEAKER                                   (0u)


/*BAP Configuration Settings*/
#define MAX_NUM_UCL_SNK_ASE                     (0u)
#define MAX_NUM_UCL_SRC_ASE                     (0u)
#define MAX_NUM_USR_SNK_ASE                     (0u)
#define MAX_NUM_USR_SRC_ASE                     (0u)
#define MAX_NUM_PAC_SNK_RECORDS                 (3u)    /* Maximum number of Sink PAC records supported by Broadcast Sink*/
#define MAX_NUM_PAC_SRC_RECORDS                 (3u)    /* Maximum number of Source PAC records supported by Broadcast Sink*/
#define PACS_CODEC_SPECIFIC_CAPABILITIES_LENGTH (16u)   /* Size of the Codec Specific Capabilities Length of the registered PACS records */
#define PACS_SRV_METADATA_SIZE                  (4u)    /* Size of the metadata associated to the registered PACS records */
#define MAX_NUM_CIG                             (0u)    /* Maximum number of CIGes */
#define MAX_NUM_CIS_PER_CIG                     (0u)    /* Maximum number of CISes per CIG */
#define MAX_NUM_BIG                             (1u)    /* Maximum number of BIGes */
#define MAX_NUM_BIS_PER_BIG                     (2u)    /* Maximum number of BISes per BIG */
#define MAX_NUM_SDE_BSRC_INFO                   (1u)    /* Maximum Number of Broadcast Source Information supported by Scan Delegator */
#define MAX_BASS_CODEC_CONFIG_SIZE              (20u)   /* Maximum size of the Codec Specific Configuration for each Broadcast Source Information supported by Scan Delegator */
#define MAX_BASS_METADATA_SIZE                  (20u)   /* Maximum size of the etadata for each Broadcast Source Information supported by Scan Delegator */
#define MAX_NUM_BASS_BASE_SUBGROUPS             (2u)    /* Maximum number of number of subgroups present in the BASE structure used to describe a BIG */
#define PBP_MAX_BIS                             (2u)
#define MAX_METADATA_LEN                        (50u)


/* Maximum number of Ble links is supported in CAP Acceptor role configuration*/
#define MAX_CAP_ACCEPTOR_INSTANCES              (1u)

/* These delays refers to the time at which the audio signal passes through an
 * audio interface (such an electroacoustic transducer ) to or from
 * the Codec interface.
 * These delays are a subpart of the Presentation Delay as described in chapter 7
 * of the Basic Audio Profile specification
 * The delay unit is us
 */

#define APP_DELAY_SNK_MIN                       (10)                            /* DMA delay for transmitting to
                                                                                 * SAI peripheral (for more precision,
                                                                                 * should add DAC delay)
                                                                                 */
#define APP_DELAY_SNK_MAX                       (APP_DELAY_SNK_MIN + 0u)        /* No extra buffering of audio data*/


/**
 * GATT Services Configuration
 */
#define BLE_APP_NUM_GATT_SERVICES       (2u + BAP_PACS_SRV_NUM_GATT_SERVICES \
                                        + BAP_BASS_SRV_NUM_GATT_SERVICES \
                                        + CAP_ACC_COM_NUM_GATT_SERVICES)

/**
 * GATT Attributes Configuration
 */
#define BLE_APP_NUM_GATT_ATTRIBUTES     (4u + 7u \
                                        + BAP_PACS_SRV_GATT_ATTRIBUTES(MAX_NUM_PAC_SNK_RECORDS,MAX_NUM_PAC_SRC_RECORDS) \
                                        + BAP_BASS_SRV_GATT_ATTRIBUTES(MAX_NUM_SDE_BSRC_INFO))

/**
 * Size of the storage area for Attribute values
 */
#define BLE_TOTAL_PAC_RECORDS_ATT_VALUE_ARRAY_SIZE \
                ((MAX_NUM_PAC_SNK_RECORDS+ MAX_NUM_PAC_SRC_RECORDS) \
                  * BAP_PAC_RECORD_ATT_VALUE_ARRAY_SIZE(CFG_BLE_NUM_LINK, \
                                                        PACS_CODEC_SPECIFIC_CAPABILITIES_LENGTH, \
                                                        PACS_SRV_METADATA_SIZE))
#define BLE_PACS_SRV_ATT_VALUE_ARRAY_SIZE \
                  BAP_PACS_SRV_ATT_VALUE_ARRAY_SIZE(CFG_BLE_NUM_LINK, \
                                                    MAX_NUM_PAC_SNK_RECORDS, \
                                                    MAX_NUM_PAC_SRC_RECORDS, \
                                                    BLE_TOTAL_PAC_RECORDS_ATT_VALUE_ARRAY_SIZE)

#define BLE_BASS_SRV_ATT_VALUE_ARRAY_SIZE \
                      BAP_BASS_SRV_ATT_VALUE_ARRAY_SIZE(MAX_NUM_SDE_BSRC_INFO,CFG_BLE_NUM_LINK,MAX_BASS_METADATA_SIZE, \
                                                        MAX_NUM_BASS_BASE_SUBGROUPS)


#define BLE_APP_ATT_VALUE_ARRAY_SIZE    (52u + BLE_PACS_SRV_ATT_VALUE_ARRAY_SIZE \
                                        + BLE_BASS_SRV_ATT_VALUE_ARRAY_SIZE)

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

