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
#define APP_HAP_ROLE                                    (HAP_ROLE_HEARING_AID)
#define APP_CAP_ROLE                                    (CAP_ROLE_ACCEPTOR)
#define APP_AUDIO_ROLE                                  (AUDIO_ROLE_SOURCE | AUDIO_ROLE_SINK)

#define GAP_APPEARANCE_GENERIC_HEARING_AID              (0x0A40u)

#define APP_BAP_ROLE_UNICAST_SERVER_SUPPORT             (1u)
#define APP_BAP_ROLE_UNICAST_CLIENT_SUPPORT             (0u)
#define APP_BAP_ROLE_BROADCAST_SOURCE_SUPPORT           (0u)
#define APP_BAP_ROLE_BROADCAST_SINK_SUPPORT             (0u)
#define APP_BAP_ROLE_SCAN_DELEGATOR_SUPPORT             (0u)
#define APP_BAP_ROLE_BROADCAST_ASSISTANT_SUPPORT        (0u)

#define APP_CCP_ROLE_SERVER_SUPPORT                     (0u)
#define APP_CCP_ROLE_CLIENT_SUPPORT                     (1u)

#define APP_MCP_ROLE_SERVER_SUPPORT                     (0u)
#define APP_MCP_ROLE_CLIENT_SUPPORT                     (0u)

#define APP_VCP_ROLE_CONTROLLER_SUPPORT                 (0u)
#if ((APP_AUDIO_ROLE & AUDIO_ROLE_SINK) == AUDIO_ROLE_SINK)
#define APP_VCP_ROLE_RENDERER_SUPPORT                   (1u)
#else
#define APP_VCP_ROLE_RENDERER_SUPPORT                   (0u)
#endif /*((APP_AUDIO_ROLE & AUDIO_ROLE_SINK) == AUDIO_ROLE_SINK)*/

#define APP_MICP_ROLE_CONTROLLER_SUPPORT                (0u)
#define APP_MICP_ROLE_DEVICE_SUPPORT                    (1u)

#define APP_CSIP_ROLE_SET_COORDINATOR_SUPPORT           (0u)
#define APP_CSIP_ROLE_SET_MEMBER_SUPPORT                (1u)

/*Server Preferred QoS Settings */
#define APP_DEFAULT_BAP_FRAMING                 BAP_FRAMING_UNFRAMED
#define APP_DEFAULT_BAP_PREF_PHY                LE_2M_PHY_PREFERRED
#define APP_DEFAULT_BAP_PREF_RETRANSMISSIONS    (2u)
#define APP_DEFAULT_BAP_MAX_TRANSPORT_LATENCY   (0x0005)

/* Allow the Unicast Server to accept Audio High Reliability during Audio setup process
 * Note High Reliability increases the audio latency
 */
#define ENABLE_AUDIO_HIGH_RELIABILITY           (0u)

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

#define APP_DELAY_SNK_MIN                       (100)                           /* DMA delay for transmitting to
                                                                                 * SAI peripheral (for more precision,
                                                                                 * should add DAC delay)
                                                                                 */
#define APP_DELAY_SNK_MAX                       (APP_DELAY_SNK_MIN + 0u)        /* No extra buffering of audio data*/

/*BAP mandatory supported delay definition*/
#define BAP_MANDATORY_PRESENTATION_DELAY        (40000u)

/*Preferred Controller Delay in us*/
#define PREFFERED_CONTROLLER_DELAY              (22000u)

/*HAP and CAP Configuration Settings*/
#define MAX_NUM_USR_LINK                        CFG_BLE_NUM_LINK        /* Maximum number of Link supported by Unicast Server*/

#define MAX_NUM_CIG                             (1u)                    /* Maximum number of CIGes */
#define MAX_NUM_CIS_PER_CIG                     (2u)                    /* Maximum number of CISes per CIG */
#define MAX_NUM_BIG                             (0u)                    /* Maximum number of BIGes */
#define MAX_NUM_BIS_PER_BIG                     (0u)                    /* Maximum number of BISes per BIG */

#define MAX_USR_CODEC_CONFIG_SIZE               (20u)                   /* Maximum size of the Codec Specific Configuration for each Audio Stream Endpoint supported by Unicast Server */
#define MAX_USR_METADATA_SIZE                   (10u)                   /* Maximum size of the Metadata for each Audio Stream Endpoint supported by Unicast Server */
#define PACS_CODEC_SPECIFIC_CAPABILITIES_LENGTH (16u)                   /* Size of the Codec Specific Capabilities Length of the registered PACS records */
#define PACS_SRV_METADATA_SIZE                  (4u)                    /* Size of the metadata associated to the registered PACS records */


#define MIN_NUM_BAP_SNK_PAC_RECORDS             (2u)                    /* 2 Audio Sink Mandatory codec specific capabilities (cf BAP Specification)*/
#define MIN_NUM_BAP_SRC_PAC_RECORDS             (1u)                    /* 1 Audio Source Mandatory codec specific capabilities (cf BAP Specification)*/


#if ((APP_AUDIO_ROLE & AUDIO_ROLE_SINK) == AUDIO_ROLE_SINK)
#define APP_NUM_SNK_PAC_RECORDS                 (MIN_NUM_BAP_SNK_PAC_RECORDS)
#else
#define APP_NUM_SNK_PAC_RECORDS                 (0u)
#endif /* ((APP_AUDIO_ROLE & AUDIO_ROLE_SINK) == AUDIO_ROLE_SINK) */

#if ((APP_AUDIO_ROLE & AUDIO_ROLE_SOURCE) == AUDIO_ROLE_SOURCE)
#define APP_NUM_SRC_PAC_RECORDS                 (MIN_NUM_BAP_SRC_PAC_RECORDS)
#else
#define APP_NUM_SRC_PAC_RECORDS                 (0u)
#endif /* ((APP_AUDIO_ROLE & AUDIO_ROLE_SOURCE) == AUDIO_ROLE_SOURCE) */

#define COMPANY_ID                              (0x0000u)

#define STEREO_AUDIO_LOCATIONS                  (FRONT_LEFT|FRONT_RIGHT)
#define MONO_AUDIO_LOCATIONS                    FRONT_LEFT
#define APP_MAX_NUM_CIS                         MAX_NUM_CIS_PER_CIG

#if ((APP_AUDIO_ROLE & AUDIO_ROLE_SINK) == AUDIO_ROLE_SINK)
#define MAX_NUM_USR_SNK_ASE                     (2u)
#else
#define MAX_NUM_USR_SNK_ASE                     (0u)
#endif /* ((APP_AUDIO_ROLE & AUDIO_ROLE_SINK) == AUDIO_ROLE_SINK) */
#if ((APP_AUDIO_ROLE & AUDIO_ROLE_SOURCE) == AUDIO_ROLE_SOURCE)
#define MAX_NUM_USR_SRC_ASE                     (2u)
#else
#define MAX_NUM_USR_SRC_ASE                     (0u)
#endif /* ((APP_AUDIO_ROLE & AUDIO_ROLE_SOURCE) == AUDIO_ROLE_SOURCE) */

#define SUPPORTED_SNK_CONTEXTS                  (AUDIO_CONTEXT_UNSPECIFIED | AUDIO_CONTEXT_CONVERSATIONAL | AUDIO_CONTEXT_MEDIA | \
                                                  AUDIO_CONTEXT_ALERTS | AUDIO_CONTEXT_RINGTONE)
#define SUPPORTED_SRC_CONTEXTS                  (AUDIO_CONTEXT_UNSPECIFIED | AUDIO_CONTEXT_CONVERSATIONAL)

/*GAP Configuration Settings*/
#define APPBLE_GAP_DEVICE_NAME_LENGTH           (14u)
#define BD_ADDR_SIZE_LOCAL                      (6u)
#define BAP_ADV_DATA_LEN                        (18u)
#define ADV_LOCAL_NAME_LEN                      (APPBLE_GAP_DEVICE_NAME_LENGTH+1)
#define ADV_EXT_LEN                             (10u)
#define ADV_AD_FLAGS_LEN                        (3u)
#define CAP_ADV_DATA_LEN                        (BAP_ADV_DATA_LEN+13u)
#define HAP_ADV_DATA_LEN                        (10u)
#define ADV_LEN                                 (ADV_AD_FLAGS_LEN+ADV_LOCAL_NAME_LEN+CAP_ADV_DATA_LEN+ADV_EXT_LEN+HAP_ADV_DATA_LEN+10)



/* Call Control Profile Settings*/
#if (APP_CCP_ROLE_CLIENT_SUPPORT == 1u)
#define APP_CCP_CLT_FEATURE_SUPPORT                     (1u)
#define APP_CCP_NUM_REMOTE_BEARER_INSTANCES             (2u)
#define APP_CCP_CALL_URI_LENGTH                         (30)
#else /*(APP_CCP_ROLE_CLIENT_SUPPORT == 0u)*/
#define APP_CCP_CLT_FEATURE_SUPPORT                     (0u)
#define APP_CCP_NUM_REMOTE_BEARER_INSTANCES             (0u)
#endif /* (APP_CCP_ROLE_CLIENT_SUPPORT == 1u) */

/*Volume Renderer Profile Settings*/
#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)
#define APP_VCP_RDR_NUM_AIC_INSTANCES                   (0u)
#define APP_VCP_RDR_NUM_VOC_INSTANCES                   (0u)
#define APP_VCP_RDR_AIC_DESCRIPTION_LENGTH              (0u)
#define APP_VCP_RDR_VOC_DESCRIPTION_LENGTH              (0u)
#else /*(APP_VCP_ROLE_RENDERER_SUPPORT == 0u)*/
#define APP_VCP_RDR_NUM_AIC_INSTANCES                   (0u)
#define APP_VCP_RDR_NUM_VOC_INSTANCES                   (0u)
#define APP_VCP_RDR_AIC_DESCRIPTION_LENGTH              (0u)
#define APP_VCP_RDR_VOC_DESCRIPTION_LENGTH              (0u)
#endif /* (APP_VCP_ROLE_RENDERER_SUPPORT == 1u) */

#if (APP_MICP_ROLE_DEVICE_SUPPORT == 1u)
#define APP_MICP_DEV_NUM_AIC_INSTANCES                  (1u)
#define APP_MICP_DEV_AIC_DESCRIPTION_LENGTH             (20u)
#else /*(APP_MICP_ROLE_DEVICE_SUPPORT == 0u)*/
#define APP_MICP_DEV_NUM_AIC_INSTANCES                  (0u)
#define APP_MICP_DEV_AIC_DESCRIPTION_LENGTH             (0u)
#endif /* (APP_MICP_ROLE_DEVICE_SUPPORT == 1u) */

/*Coordinated Set Profile Settings*/
#if (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1u)
#define APP_CSIP_SET_MEMBER_SIZE                        (2u)
#define APP_CSIP_SET_MEMBER_NUM_INSTANCES               (1u)
#define APP_SIRK                                        "STM32WBA_APPSIRK"
#else /*(APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 0u)*/
#define APP_CSIP_SET_MEMBER_NUM_INSTANCES               (0u)
#endif /* (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1u) */

#define APP_HAP_NUM_PRESETS                             (4u)


/**
 * Maximum number of Services that can be stored in the GATT database.
 * Note that the GAP and GATT services are automatically added so this parameter should be 2 plus the number of user services
 */
#define BLE_APP_PACS_SRV_NUM_GATT_SERVICES              BAP_PACS_SRV_NUM_GATT_SERVICES
#define BLE_APP_ASCS_SRV_NUM_GATT_SERVICES              BAP_ASCS_SRV_NUM_GATT_SERVICES
#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)
#define BLE_APP_VCP_RENDERER_NUM_GATT_SERVICES          VCP_RENDERER_NUM_GATT_SERVICES(APP_VCP_RDR_NUM_AIC_INSTANCES,APP_VCP_RDR_NUM_VOC_INSTANCES)
#else
#define BLE_APP_VCP_RENDERER_NUM_GATT_SERVICES          0u
#endif /*(APP_VCP_ROLE_RENDERER_SUPPORT == 1)*/

#if (APP_MICP_ROLE_DEVICE_SUPPORT == 1u)
#define BLE_APP_MICP_DEVICE_NUM_GATT_SERVICES           MICP_DEVICE_NUM_GATT_SERVICES(APP_MICP_DEV_NUM_AIC_INSTANCES)
#else
#define BLE_APP_MICP_DEVICE_NUM_GATT_SERVICES   0u
#endif /*(APP_MICP_ROLE_DEVICE_SUPPORT == 1)*/

#if (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1u)
#define BLE_APP_CSIP_SET_MEMBER_NUM_GATT_SERVICES      CSIP_SET_MEMBER_NUM_GATT_SERVICES(APP_CSIP_SET_MEMBER_NUM_INSTANCES)
#else
#define BLE_APP_CSIP_SET_MEMBER_NUM_GATT_SERVICES      0u
#endif /*(APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1)*/

#define BLE_APP_CAP_ACC_NUM_GATT_SERVICES               CAP_ACC_COM_NUM_GATT_SERVICES
#define BLE_APP_HAP_NUM_GATT_SERVICES                   HAP_HA_NUM_GATT_SERVICES


#define BLE_APP_NUM_GATT_SERVICES                       (2U + BLE_APP_PACS_SRV_NUM_GATT_SERVICES \
                                                        + BLE_APP_ASCS_SRV_NUM_GATT_SERVICES \
                                                        + BLE_APP_VCP_RENDERER_NUM_GATT_SERVICES \
                                                        + BLE_APP_MICP_DEVICE_NUM_GATT_SERVICES \
                                                        + BLE_APP_CAP_ACC_NUM_GATT_SERVICES \
                                                        + BLE_APP_CSIP_SET_MEMBER_NUM_GATT_SERVICES \
                                                        + BLE_APP_HAP_NUM_GATT_SERVICES)
/**
 * Maximum number of Attributes
 */
#define BLE_APP_PACS_SRV_NUM_GATT_ATTRIBUTES            BAP_PACS_SRV_GATT_ATTRIBUTES(APP_NUM_SRC_PAC_RECORDS,APP_NUM_SNK_PAC_RECORDS)
#define BLE_APP_ASCS_SRV_NUM_GATT_ATTRIBUTES            BAP_ASCS_SRV_GATT_ATTRIBUTES(MAX_NUM_USR_SNK_ASE,MAX_NUM_USR_SRC_ASE)
#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)
#define BLE_APP_VCP_RENDERER_NUM_GATT_ATTRIBUTES        VCP_RENDERER_NUM_GATT_ATTRIBUTES(APP_VCP_RDR_NUM_AIC_INSTANCES,APP_VCP_RDR_NUM_VOC_INSTANCES)
#else
#define BLE_APP_VCP_RENDERER_NUM_GATT_ATTRIBUTES        0u
#endif /*(APP_VCP_ROLE_RENDERER_SUPPORT == 1)*/

#if (APP_MICP_ROLE_DEVICE_SUPPORT == 1u)
#define BLE_APP_MICP_DEVICE_NUM_GATT_ATTRIBUTES         MICP_DEVICE_NUM_GATT_ATTRIBUTES(APP_MICP_DEV_NUM_AIC_INSTANCES)
#else
#define BLE_APP_MICP_DEVICE_NUM_GATT_ATTRIBUTES         0u
#endif /*(APP_MICP_ROLE_DEVICE_SUPPORT == 1)*/

#if (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1u)
#define BLE_APP_CSIP_SET_MEMBER_NUM_GATT_ATTRIBUTES     CSIP_SET_MEMBER_NUM_GATT_ATTRIBUTES(APP_CSIP_SET_MEMBER_NUM_INSTANCES)
#else
#define BLE_APP_CSIP_SET_MEMBER_NUM_GATT_ATTRIBUTES     0u
#endif /*(APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1)*/

#define BLE_APP_HAP_NUM_GATT_ATTRIBUTES                 HAP_HA_NUM_GATT_ATTRIBUTES


#define BLE_APP_NUM_GATT_ATTRIBUTES                     (4u + 7u \
                                                         + BLE_APP_PACS_SRV_NUM_GATT_ATTRIBUTES \
                                                         + BLE_APP_ASCS_SRV_NUM_GATT_ATTRIBUTES \
                                                         + BLE_APP_VCP_RENDERER_NUM_GATT_ATTRIBUTES \
                                                         + BLE_APP_MICP_DEVICE_NUM_GATT_ATTRIBUTES \
                                                         + BLE_APP_CSIP_SET_MEMBER_NUM_GATT_ATTRIBUTES \
                                                         + BLE_APP_HAP_NUM_GATT_ATTRIBUTES)

/**
 * Size of the storage area for Attribute values
 */
#define BLE_TOTAL_PAC_RECORDS_ATT_VALUE_ARRAY_SIZE \
                ((APP_NUM_SNK_PAC_RECORDS+ APP_NUM_SRC_PAC_RECORDS) \
                  * BAP_PAC_RECORD_ATT_VALUE_ARRAY_SIZE(CFG_BLE_NUM_LINK, \
                                                        PACS_CODEC_SPECIFIC_CAPABILITIES_LENGTH, \
                                                        PACS_SRV_METADATA_SIZE))

#define BLE_APP_PACS_SRV_ATT_VALUE_ARRAY_SIZE \
                  BAP_PACS_SRV_ATT_VALUE_ARRAY_SIZE(CFG_BLE_NUM_LINK, \
                                                    APP_NUM_SNK_PAC_RECORDS, \
                                                    APP_NUM_SRC_PAC_RECORDS, \
                                                    BLE_TOTAL_PAC_RECORDS_ATT_VALUE_ARRAY_SIZE)

#define BLE_TOTAL_ASES_ATT_VALUE_ARRAY_SIZE \
                    (MAX_NUM_USR_SNK_ASE + MAX_NUM_USR_SRC_ASE) \
                      * BAP_ASE_ATT_VALUE_ARRAY_SIZE(CFG_BLE_NUM_LINK,MAX_USR_CODEC_CONFIG_SIZE,MAX_USR_METADATA_SIZE)

#define BLE_APP_ASCS_SRV_ATT_VALUE_ARRAY_SIZE \
                  BAP_ASCS_SRV_ATT_VALUE_ARRAY_SIZE(CFG_BLE_NUM_LINK,BLE_TOTAL_ASES_ATT_VALUE_ARRAY_SIZE)

#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)
#define BLE_APP_VCP_RENDERER_ATT_VALUE_ARRAY_SIZE       VCP_RENDERER_ATT_VALUE_ARRAY_SIZE(CFG_BLE_NUM_LINK,\
                                                                                    APP_VCP_RDR_NUM_AIC_INSTANCES,\
                                                                                    APP_VCP_RDR_NUM_VOC_INSTANCES,\
                                                                                    APP_VCP_RDR_AIC_DESCRIPTION_LENGTH,\
                                                                                    APP_VCP_RDR_VOC_DESCRIPTION_LENGTH)
#else
#define BLE_APP_VCP_RENDERER_ATT_VALUE_ARRAY_SIZE       0u
#endif /*(APP_VCP_ROLE_RENDERER_SUPPORT == 1)*/

#if (APP_MICP_ROLE_DEVICE_SUPPORT == 1u)
#define BLE_APP_MICP_DEVICE_ATT_VALUE_ARRAY_SIZE        MICP_DEVICE_ATT_VALUE_ARRAY_SIZE(CFG_BLE_NUM_LINK,\
                                                                                    APP_MICP_DEV_NUM_AIC_INSTANCES,\
                                                                                    APP_MICP_DEV_AIC_DESCRIPTION_LENGTH)
#else
#define BLE_APP_MICP_DEVICE_ATT_VALUE_ARRAY_SIZE        0u
#endif /*(APP_MICP_ROLE_DEVICE_SUPPORT == 1)*/

#if (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1u)
#define BLE_APP_CSIP_SET_MEMBER_ATT_VALUE_ARRAY_SIZE    CSIP_SET_MEMBER_ATT_VALUE_ARRAY_SIZE(APP_CSIP_SET_MEMBER_NUM_INSTANCES,CFG_BLE_NUM_LINK)
#else
#define BLE_APP_CSIP_SET_MEMBER_ATT_VALUE_ARRAY_SIZE    0u
#endif /*(APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1)*/

#define BLE_APP_HAP_ATT_VALUE_ARRAY_SIZE                HAP_HA_ATT_VALUE_ARRAY_SIZE(CFG_BLE_NUM_LINK)

#define BLE_APP_ATT_VALUE_ARRAY_SIZE                    (49u + BLE_APP_PACS_SRV_ATT_VALUE_ARRAY_SIZE \
                                                          + BLE_APP_ASCS_SRV_ATT_VALUE_ARRAY_SIZE \
                                                          + BLE_APP_VCP_RENDERER_ATT_VALUE_ARRAY_SIZE \
                                                          + BLE_APP_MICP_DEVICE_ATT_VALUE_ARRAY_SIZE \
                                                          + BLE_APP_CSIP_SET_MEMBER_ATT_VALUE_ARRAY_SIZE \
                                                          + BLE_APP_HAP_ATT_VALUE_ARRAY_SIZE)
/* Exported types ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/

/* Exported macros ------------------------------------------------------------*/

/* Exported functions ---------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /*__HAP_APP_CONF_H */

