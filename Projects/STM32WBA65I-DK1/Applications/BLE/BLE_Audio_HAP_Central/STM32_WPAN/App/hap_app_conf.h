/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    hap_app_conf.h
  * @author  MCD Application Team
  * @brief   Application configuration file for hap_app.c module
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
#ifndef __HAP_APP_CONF_H
#define __HAP_APP_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "cap.h"
#include "bap_bufsize.h"
#include "ccp.h"
#include "csip.h"
#include "vcp.h"
#include "hap.h"
#include "app_conf.h"
/* Private includes ----------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/*Audio Profiles Roles configuration*/
#define APP_HAP_ROLE                                    (HAP_ROLE_HEARING_AID_REMOTE_CONTROLLER \
                                                         | HAP_ROLE_HEARING_AID_UNICAST_CLIENT \
                                                         | HAP_ROLE_IMMEDIATE_ALERT_CLIENT)
#define APP_CAP_ROLE                                    (CAP_ROLE_INITIATOR | CAP_ROLE_COMMANDER)

#define APP_BAP_ROLE_UNICAST_SERVER_SUPPORT             (0u)
#define APP_BAP_ROLE_UNICAST_CLIENT_SUPPORT             (1u)
#define APP_BAP_ROLE_BROADCAST_SOURCE_SUPPORT           (0u)
#define APP_BAP_ROLE_BROADCAST_SINK_SUPPORT             (0u)
#define APP_BAP_ROLE_SCAN_DELEGATOR_SUPPORT             (0u)
#define APP_BAP_ROLE_BROADCAST_ASSISTANT_SUPPORT        (0u)

#define APP_CCP_ROLE_SERVER_SUPPORT                     (1u)
#define APP_CCP_ROLE_CLIENT_SUPPORT                     (0u)

#define APP_MCP_ROLE_SERVER_SUPPORT                     (0u)
#define APP_MCP_ROLE_CLIENT_SUPPORT                     (0u)

#define APP_VCP_ROLE_CONTROLLER_SUPPORT                 (1u)
#define APP_VCP_ROLE_RENDERER_SUPPORT                   (0u)

#define APP_MICP_ROLE_CONTROLLER_SUPPORT                (1u)
#define APP_MICP_ROLE_DEVICE_SUPPORT                    (0u)

#define APP_CSIP_ROLE_SET_COORDINATOR_SUPPORT           (1u)
#define APP_CSIP_ROLE_SET_MEMBER_SUPPORT                (0u)
/**
 * Maximum Number of Bonded Devices to store in Non-Volatile Memory
 */
#define APP_MAX_NUM_BONDED_DEVICES                      (5u)

/**
 * Audio Processing Delay Settings
 */
/* These delays refers to the time at which the audio signal passes through an
 * audio interface (such an electroacoustic transducer ) to or from
 * the Codec interface.
 * These delays are a subpart of the Presentation Delay as described in chapter 7
 * of the Basic Audio Profile specification
 * The delay unit is us
 */
#define APP_DELAY_SRC_MIN                       (10)                            /* DMA delay for transmitting to
                                                                                 * SAI peripheral (for more precision,
                                                                                 * should add ADC delay)
                                                                                 */
#define APP_DELAY_SRC_MAX                       (APP_DELAY_SRC_MIN + 0u)        /* No extra buffering of audio data*/

#define APP_DELAY_SNK_MIN                       (10)                            /* DMA delay for transmitting to
                                                                                 * SAI peripheral (for more precision,
                                                                                 * should add DAC delay)
                                                                                 */
#define APP_DELAY_SNK_MAX                       (APP_DELAY_SNK_MIN + 0u)        /* No extra buffering of audio data*/

/*Preferred Controller Delay in us*/
#define PREFFERED_CONTROLLER_DELAY              (22000u)


/**
 * Maximum number of Links supported in Unicast Server
 */
#define MAX_NUM_UCL_LINK                        CFG_BLE_NUM_LINK        /* Maximum number of Link supported by Unicast Client*/

/**
 * Broadcast and Connects Isochronous Stream
 */
#define MAX_NUM_CIG                             (1u)                    /* Maximum number of CIGes */
#define MAX_NUM_CIS_PER_CIG                     (2u)                    /* Maximum number of CISes per CIG */
#define MAX_NUM_BIG                             (0u)                    /* Maximum number of BIGes */
#define MAX_NUM_BIS_PER_BIG                     (0u)                    /* Maximum number of BISes per BIG */
#define APP_MAX_NUM_CIS                         (MAX_NUM_CIS_PER_CIG * MAX_NUM_CIG)

/**
 * Basic Audio Profile Settings
 */
#define MAX_NUM_UCL_SNK_ASE_PER_LINK            (2u)                    /* Maximum number of Sink Audio Stream Endpoints per Link supported by Unicast Client*/
#define MAX_NUM_UCL_SRC_ASE_PER_LINK            (2u)                    /* Maximum number of Source Audio Stream Endpoints per Link supported by Unicast Client*/
#define MAX_UCL_CODEC_CONFIG_SIZE               (30u)                   /* Maximum size of the Codec Specific Configuration for each Audio Stream Endpoint supported by Unicast Client */
#define MAX_UCL_METADATA_SIZE                   (30u)                   /* Maximum size of the Metadata for each Audio Stream Endpoint supported by Unicast Client */

#define MAX_NUM_CLT_SNK_PAC_RECORDS_PER_LINK    (12u)                   /* Maximum number of Sink PAC records per link supported by Unicast Client and Broadcast Assistant */
#define MAX_NUM_CLT_SRC_PAC_RECORDS_PER_LINK    (12u)                   /* Maximum number of Source PAC records per link supported by Unicast Client and Broadcast Assistant */
#define MAX_NUM_UCL_SNK_ASE                     (MAX_NUM_UCL_LINK * MAX_NUM_UCL_SNK_ASE_PER_LINK)
#define MAX_NUM_UCL_SRC_ASE                     (MAX_NUM_UCL_LINK * MAX_NUM_UCL_SRC_ASE_PER_LINK)


/**
 * GAP Configuration Settings
 */
#define APPBLE_GAP_DEVICE_NAME_LENGTH           (14u)
#define BD_ADDR_SIZE_LOCAL                      (6u)

/**
 * Hearing Aids Profile Settings
 */
#define APP_HAP_NUM_PRESETS                     (4u)

/**
 * Call Control Profile Settings
 */
#if (APP_CCP_ROLE_SERVER_SUPPORT == 1u)
#define APP_CCP_NUM_LOCAL_BEARER_INSTANCES                      (0u)
#define APP_CCP_START_CCID                                      (0u)
#define APP_CCP_NUM_CALLS                                       (1u)
#define APP_CCP_CALL_URI_LENGTH                                 (30)
#define APP_CCP_BEARER_PROVIDER_NAME_LENGTH                     (30u)
#define APP_CCP_BEARER_UCI_LENGTH                               (30u)
#define APP_CCP_BEARER_URI_SCHEMES_SUPPORTED_LIST_LENGTH        (30u)
#define APP_CCP_CALL_FRIENDLY_NAME_LENGTH                       (50u)
#define APP_CCP_FEATURES                                        (CCP_FEATURE_BEARER_SIGNAL_STRENGTH \
                                                                | CCP_FEATURE_INC_CALL_TARGET_BEARER_URI \
                                                                | CCP_FEATURE_CALL_FRIENDLY_NAME)
#define APP_CCP_READLONG_CFG                                    (1u)
#else
#define APP_CCP_NUM_LOCAL_BEARER_INSTANCES                      (0u)
#define APP_CCP_START_CCID                                      (0u)
#define APP_CCP_NUM_CALLS                                       (0u)
#define APP_CCP_CALL_URI_LENGTH                                 (0u)
#define APP_CCP_BEARER_PROVIDER_NAME_LENGTH                     (0u)
#define APP_CCP_BEARER_UCI_LENGTH                               (0u)
#define APP_CCP_BEARER_URI_SCHEMES_SUPPORTED_LIST_LENGTH        (0u)
#define APP_CCP_CALL_FRIENDLY_NAME_LENGTH                       (0u)
#define APP_CCP_FEATURES                                        (0u)
#define APP_CCP_READLONG_CFG                                    (0u)
#endif /* (APP_CCP_ROLE_SERVER_SUPPORT == 1u) */

/**
 * Volume Control Profile Settings
 */
#if (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u)
#define APP_VCP_CTLR_NUM_AIC_INSTANCES                          (2u)
#define APP_VCP_CTLR_NUM_VOC_INSTANCES                          (2u)
#else
#define APP_VCP_CTLR_NUM_AIC_INSTANCES                          (0u)
#define APP_VCP_CTLR_NUM_VOC_INSTANCES                          (0u)
#endif /* (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u) */

/**
 * Microphone Input Profile Settings
 */
#if (APP_MICP_ROLE_CONTROLLER_SUPPORT == 1u)
#define APP_MICP_CTLR_NUM_AIC_INSTANCES                         (2u)
#else /* (APP_MICP_ROLE_CONTROLLER_SUPPORT == 1u) */
#define APP_MICP_CTLR_NUM_AIC_INSTANCES                         (0u)
#endif /* (APP_MICP_ROLE_CONTROLLER_SUPPORT == 1u) */

/**
 * Coordinated Set Profile Settings
 */
#if (APP_CSIP_ROLE_SET_COORDINATOR_SUPPORT == 1)
#define APP_CSIP_AUTOMATIC_SET_MEMBERS_DISCOVERY                (1u)
#endif /* (APP_CSIP_ROLE_SET_COORDINATOR_SUPPORT == 1u) */

/**
 * Maximum number of Services that can be stored in the GATT database.
 * Note that the GAP and GATT services are automatically added so this parameter should be 2 plus the number of user services
 */
#define BLE_HOST_NUM_GATT_SERVICES             (2u)
#if (APP_CCP_ROLE_SERVER_SUPPORT == 1u)
#define BLE_APP_CCP_SRV_NUM_GATT_SERVICES      CCP_SRV_NUM_GATT_SERVICES(APP_CCP_NUM_LOCAL_BEARER_INSTANCES+1u)
#else
#define BLE_APP_CCP_SRV_NUM_GATT_SERVICES      0u
#endif /*(APP_CCP_ROLE_SERVER_SUPPORT == 1)*/

#if ((APP_CAP_ROLE & CAP_ROLE_COMMANDER) == CAP_ROLE_COMMANDER)
#define BLE_APP_CAP_COM_NUM_GATT_SERVICES      CAP_ACC_COM_NUM_GATT_SERVICES
#else
#define BLE_APP_CAP_COM_NUM_GATT_SERVICES      0u
#endif /*((APP_CAP_ROLE & CAP_ROLE_ACCEPTOR) == CAP_ROLE_ACCEPTOR)*/


#define BLE_APP_NUM_GATT_SERVICES              (BLE_HOST_NUM_GATT_SERVICES \
                                               + BLE_APP_CCP_SRV_NUM_GATT_SERVICES \
                                               + BLE_APP_CAP_COM_NUM_GATT_SERVICES)
/**
 * Maximum number of Attributes
 */
#if ((CFG_BLE_OPTIONS & BLE_OPTIONS_ENHANCED_ATT) == BLE_OPTIONS_ENHANCED_ATT)
#define BLE_HOST_NUM_GATT_ATTRIBUTES                    (15u)
#else
#define BLE_HOST_NUM_GATT_ATTRIBUTES                    (11u)
#endif /*((CFG_BLE_OPTIONS & BLE_OPTIONS_ENHANCED_ATT) == BLE_OPTIONS_ENHANCED_ATT)*/
#if (APP_CCP_ROLE_SERVER_SUPPORT == 1u)
#define BLE_APP_CCP_SRV_NUM_GATT_ATTRIBUTES    CCP_SRV_NUM_GATT_ATTRIBUTES((APP_CCP_NUM_LOCAL_BEARER_INSTANCES+1u),\
                                                                            APP_CCP_FEATURES)
#else
#define BLE_APP_CCP_SRV_NUM_GATT_ATTRIBUTES    0u
#endif /*(APP_CCP_ROLE_SERVER_SUPPORT == 1)*/

#define BLE_APP_NUM_GATT_ATTRIBUTES            (BLE_HOST_NUM_GATT_ATTRIBUTES + BLE_APP_CCP_SRV_NUM_GATT_ATTRIBUTES)

/**
 * Size of the storage area for Attribute values
 */
#if ((CFG_BLE_OPTIONS & BLE_OPTIONS_ENHANCED_ATT) == BLE_OPTIONS_ENHANCED_ATT)
#define BLE_HOST_ATT_VALUE_ARRAY_SIZE                   (64u)
#else
#define BLE_HOST_ATT_VALUE_ARRAY_SIZE                   (54u)
#endif /*((CFG_BLE_OPTIONS & BLE_OPTIONS_ENHANCED_ATT) == BLE_OPTIONS_ENHANCED_ATT)*/
#if (APP_CCP_ROLE_SERVER_SUPPORT == 1u)
#define BLE_APP_CCP_SRV_ATT_VALUE_ARRAY_SIZE   CCP_SRV_ATT_VALUE_ARRAY_SIZE((APP_CCP_NUM_LOCAL_BEARER_INSTANCES +1u), \
                                                                             CFG_BLE_NUM_LINK, \
                                                                             APP_CCP_FEATURES, \
                                                                             APP_CCP_BEARER_PROVIDER_NAME_LENGTH, \
                                                                             APP_CCP_BEARER_UCI_LENGTH, \
                                                                             APP_CCP_BEARER_URI_SCHEMES_SUPPORTED_LIST_LENGTH, \
                                                                             APP_CCP_NUM_CALLS,\
                                                                             APP_CCP_CALL_URI_LENGTH, \
                                                                             APP_CCP_CALL_FRIENDLY_NAME_LENGTH)
#else
#define BLE_APP_CCP_SRV_ATT_VALUE_ARRAY_SIZE    0u
#endif /*(APP_CCP_ROLE_SERVER_SUPPORT == 1)*/

#define BLE_APP_ATT_VALUE_ARRAY_SIZE            (BLE_HOST_ATT_VALUE_ARRAY_SIZE + BLE_APP_CCP_SRV_ATT_VALUE_ARRAY_SIZE)

/**
 * Size of the storage area for Database of Audio Services/Profile to save in Non Volatile Memory
 */

/*Memory size required to store all devices in NVM for ASCS Client role*/
#define BAP_ASCS_CLT_NVM_ALLOC_SIZE             BAP_ASCS_CLT_DB_BUFFER_SIZE(APP_MAX_NUM_BONDED_DEVICES,\
                                                                            MAX_NUM_UCL_SNK_ASE_PER_LINK,\
                                                                            MAX_NUM_UCL_SRC_ASE_PER_LINK,\
                                                                            MAX_UCL_CODEC_CONFIG_SIZE)

/*Memory size required to store all devices in NVM for PACS Client role*/
#define BAP_PACS_CLT_NVM_ALLOC_SIZE             BAP_PACS_CLT_DB_BUFFER_SIZE(APP_MAX_NUM_BONDED_DEVICES,\
                                                                            (MAX_NUM_CLT_SNK_PAC_RECORDS_PER_LINK + MAX_NUM_CLT_SRC_PAC_RECORDS_PER_LINK))

/*Memory size required to store all devices in NVM for VCP Controller role*/
#if (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u)
#define BLE_VCP_CTLR_NVM_ALLOC_SIZE             BLE_VCP_CTLR_DB_BUFFER_SIZE(APP_MAX_NUM_BONDED_DEVICES,\
                                                                            APP_VCP_CTLR_NUM_AIC_INSTANCES,\
                                                                            APP_VCP_CTLR_NUM_VOC_INSTANCES)
#else
#define BLE_VCP_CTLR_NVM_ALLOC_SIZE             0u
#endif /* (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u) */

/*Memory size required to store all devices in NVM for MICP Controller role*/
#if (APP_MICP_ROLE_CONTROLLER_SUPPORT == 1u)
#define BLE_MICP_CTLR_NVM_ALLOC_SIZE             BLE_MICP_CTLR_DB_BUFFER_SIZE(APP_MAX_NUM_BONDED_DEVICES,\
                                                                              APP_MICP_CTLR_NUM_AIC_INSTANCES)
#else
#define BLE_MICP_CTLR_NVM_ALLOC_SIZE             0u
#endif /* (APP_MICP_ROLE_CONTROLLER_SUPPORT == 1u) */

/*Memory size required to store all devices in NVM for CSIP in Set Coordinator role*/
#if (APP_CSIP_ROLE_SET_COORDINATOR_SUPPORT == 1)
#define BLE_CSIP_SET_COORDINATOR_NVM_ALLOC_SIZE BLE_CSIP_SET_COORDINATOR_DB_BUFFER_SIZE(APP_MAX_NUM_BONDED_DEVICES)
#else
#define BLE_CSIP_SET_COORDINATOR_NVM_ALLOC_SIZE 0u
#endif /* (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u) */



#define BLE_APP_AUDIO_NVM_ALLOC_SIZE            (BAP_ASCS_CLT_NVM_ALLOC_SIZE + BAP_PACS_CLT_NVM_ALLOC_SIZE \
                                                + BLE_VCP_CTLR_NVM_ALLOC_SIZE + BLE_CSIP_SET_COORDINATOR_NVM_ALLOC_SIZE \
                                                + BLE_MICP_CTLR_NVM_ALLOC_SIZE \
                                                + BLE_AUDIO_DB_BUFFER_SIZE(APP_MAX_NUM_BONDED_DEVICES,HAP_HARC_GATT_DATABASE_SIZE) \
                                                + BLE_AUDIO_DB_BUFFER_SIZE(APP_MAX_NUM_BONDED_DEVICES,HAP_IAC_GATT_DATABASE_SIZE))
/* Exported types ------------------------------------------------------------*/

/* External variables --------------------------------------------------------*/

/* Exported macros ------------------------------------------------------------*/

/* Exported functions ---------------------------------------------*/
#ifdef __cplusplus
}
#endif
#endif /* __HAP_APP_CONF_H */

