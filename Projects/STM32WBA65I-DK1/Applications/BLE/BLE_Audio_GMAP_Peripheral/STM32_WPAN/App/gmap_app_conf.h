/**
  ******************************************************************************
  * @file    gmap_app_conf.h
  * @author  MCD Application Team
  * @brief   Application configuration file for gmap_app.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GMAP_APP_CONF_H
#define __GMAP_APP_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "cap.h"
#include "bap_bufsize.h"
#include "ccp.h"
#include "mcp.h"
#include "csip.h"
#include "vcp.h"
#include "app_conf.h"
#include "gmap.h"
/* Private includes ----------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/*Audio Profiles Roles configuration*/
#define APP_GMAP_ROLE                                   (GMAP_ROLE_UNICAST_GAME_TERMINAL | GMAP_ROLE_BROADCAST_GAME_RECEIVER)
#define APP_CAP_ROLE                                    (CAP_ROLE_ACCEPTOR)
#define APP_AUDIO_ROLE                                  (AUDIO_ROLE_SOURCE | AUDIO_ROLE_SINK)

#define APP_GMAP_UGT_FEATURE                            (UGT_FEATURES_SOURCE_SUPPORT \
                                                         | UGT_FEATURES_80_KBPS_SOURCE_SUPPORT \
                                                         | UGT_FEATURES_SINK_SUPPORT \
                                                         | UGT_FEATURES_64_KBPSSINK_SUPPORT \
                                                         | UGT_FEATURES_MULTIPLEX_SUPPORT \
                                                         | UGT_FEATURES_MULTISINK_SUPPORT \
                                                         | UGT_FEATURES_MULTISOURCE_SUPPORT)

#define APP_GMAP_BGR_FEATURE                            (BGR_FEATURES_MULTISINK_SUPPORT \
                                                         | BGR_FEATURES_MULTIPLEX_SUPPORT)

#define GAP_APPEARANCE_EARBUD                           (0x0941u)
#define GAP_APPEARANCE_HEADPHONES                       (0x0943u)

#if ((APP_GMAP_ROLE & GMAP_ROLE_UNICAST_GAME_TERMINAL) != 0)
#define APP_BAP_ROLE_UNICAST_SERVER_SUPPORT             (1u)
#else /*((APP_GMAP_ROLE & GMAP_ROLE_UNICAST_GAME_TERMINAL) != 0)*/
#define APP_BAP_ROLE_UNICAST_SERVER_SUPPORT             (0u)
#endif /*((APP_GMAP_ROLE & GMAP_ROLE_UNICAST_GAME_TERMINAL) != 0)*/
#define APP_BAP_ROLE_UNICAST_CLIENT_SUPPORT             (0u)
#define APP_BAP_ROLE_BROADCAST_SOURCE_SUPPORT           (0u)
#if ((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)
#define APP_BAP_ROLE_BROADCAST_SINK_SUPPORT             (1u)
#define APP_BAP_ROLE_SCAN_DELEGATOR_SUPPORT             (1u)
#else /*(APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)*/
#define APP_BAP_ROLE_BROADCAST_SINK_SUPPORT             (0u)
#define APP_BAP_ROLE_SCAN_DELEGATOR_SUPPORT             (0u)
#endif /*(APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)*/
#define APP_BAP_ROLE_BROADCAST_ASSISTANT_SUPPORT        (0u)

#define APP_VCP_ROLE_CONTROLLER_SUPPORT                 (0u)
#if ((APP_GMAP_ROLE & GMAP_ROLE_UNICAST_GAME_TERMINAL) == GMAP_ROLE_UNICAST_GAME_TERMINAL)
#if ((APP_AUDIO_ROLE & AUDIO_ROLE_SINK) == AUDIO_ROLE_SINK)
#define APP_VCP_ROLE_RENDERER_SUPPORT                   (1u)
#else
#define APP_VCP_ROLE_RENDERER_SUPPORT                   (0u)
#endif /*((APP_AUDIO_ROLE & AUDIO_ROLE_SINK) == AUDIO_ROLE_SINK)*/
#endif /*((APP_GMAP_ROLE & GMAP_ROLE_UNICAST_GAME_TERMINAL) == GMAP_ROLE_UNICAST_GAME_TERMINAL)*/

#define APP_MICP_ROLE_CONTROLLER_SUPPORT                (0u)
#define APP_MICP_ROLE_DEVICE_SUPPORT                    (1u)

#define APP_CSIP_ROLE_SET_COORDINATOR_SUPPORT           (0u)
#define APP_CSIP_ROLE_SET_MEMBER_SUPPORT                (1u)

/**
 * Maximum Number of Bonded Devices to store in Non-Volatile Memory
 */
#define APP_MAX_NUM_BONDED_DEVICES                      (5u)

/**
 * Memory size used to store GATT Service information related to a device to store in Non Volatile Memory
 */
#define GATTSERVICE_GATT_DATABASE_SIZE                  (8u)

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

/**
 * Server Preferred QoS Settings used in Unicast mode
 */
#define APP_DEFAULT_BAP_FRAMING                 BAP_FRAMING_UNFRAMED
#define APP_DEFAULT_BAP_PREF_PHY                LE_2M_PHY_PREFERRED
#define APP_DEFAULT_BAP_PREF_RETRANSMISSIONS    (2u)
#define APP_DEFAULT_BAP_MAX_TRANSPORT_LATENCY   (0x0005)

/**
 * Allow the Unicast Server to accept Audio High Reliability during Audio setup process
 * Note High Reliability increases the audio latency
 */
#define ENABLE_AUDIO_HIGH_RELIABILITY           (1u)

/**
 * Indicate if ASCS Server caches the ASE codec configuration during autonomous Released operation when ASE
 * is in Releasing state. Transition an ASE from Releasing state to the Idle state (caching = 0) or the
 *  Codec Configured state (caching = 1)
 */
#define APP_ASCS_CACHING                        (0x01u)

/**
 * Maximum number of Links supported in Unicast Server
 */
#define MAX_NUM_USR_LINK                        CFG_BLE_NUM_LINK

/**
 * Broadcast and Connects Isochronous Stream
 */
#define MAX_NUM_CIG                             (1u)    /* Maximum number of CIGes */
#define MAX_NUM_CIS_PER_CIG                     (2u)    /* Maximum number of CISes per CIG */
#define MAX_NUM_BIG                             (1u)    /* Maximum number of BIGes */
#define MAX_NUM_BIS_PER_BIG                     (2u)    /* Maximum number of BISes per BIG */
#define APP_MAX_NUM_CIS                         (MAX_NUM_CIS_PER_CIG * MAX_NUM_CIG)
#define APP_MAX_NUM_BIS                         (MAX_NUM_BIS_PER_BIG * MAX_NUM_BIG)

/**
 * Default Audio Locations Settings
 */
#define STEREO_AUDIO_LOCATIONS                  (FRONT_LEFT|FRONT_RIGHT)
#define MONO_AUDIO_LOCATIONS                    (FRONT_LEFT)

/**
 * Scan Delegator Settings
 */
#define MAX_NUM_SDE_BSRC_INFO                   (1u)    /* Maximum Number of Broadcast Source Information supported by Scan Delegator */
#define MAX_BASS_CODEC_CONFIG_SIZE              (20u)   /* Maximum size of the Codec Specific Configuration for each Broadcast Source Information supported by Scan Delegator */
#define MAX_BASS_METADATA_SIZE                  (50u)   /* Maximum size of the metadata for each Broadcast Source Information supported by Scan Delegator */
#define MAX_NUM_BASS_BASE_SUBGROUPS             (2u)    /* Maximum number of number of subgroups present in the BASE structure used to describe a BIG */

/**
 * Broadcast Sink Settings
 */
#define MAX_BSNK_SUBGROUP_CODEC_CONFIG_SIZE     (19u)   /* Maximum size of the Codec Specific Configuration for each subgroup used by GMAP Broadcast Media Receiver role */
#define MAX_BSNK_BIS_CODEC_CONFIG_SIZE          (6u)    /* Maximum size of the Codec Specific Configuration for each BIS in each subgroup used by GMAP Broadcast Media Receiver role */
#define MAX_BSNK_METADATA_LEN                   (50u)   /* Size of the metadata associated to the subgroup used by GMAP Broadcast Media Receiver role */

/**
 * Codec Specific Capabilities Settings
 */
#define MAX_USR_CODEC_CONFIG_SIZE               (20u)   /* Maximum size of the Codec Specific Configuration for each Audio Stream Endpoint supported by Unicast Server */
#define MIN_NUM_BAP_SNK_CODEC_CAP               (2u)    /* 2 Audio Sink Mandatory codec specific capabilities (cf BAP Specification)*/
#define MIN_NUM_BAP_SRC_CODEC_CAP               (1u)    /* 1 Audio Source Mandatory codec specific capabilities (cf BAP Specification)*/
#define MIN_NUM_GMAP_SRC_CODEC_CAP              (6u)    /* 6 Audio Source Mandatory codec specific capabilities for GMAP (cf GMAP Specification)*/
#define MIN_NUM_GMAP_SNK_CODEC_CAP              (6u)    /* 6 Audio Sink Mandatory codec specific capabilities for GMAP (cf GMAP Specification)*/

#define BAP_MANDATORY_SNK_CODEC_CAP_LIST        {LC3_16_2,LC3_24_2}
#define BAP_MANDATORY_SRC_CODEC_CAP_LIST        {LC3_16_2}

/**
 * PAC Record Settings
 */
#define COMPANY_ID                              (0x0000u)
#define PACS_CODEC_SPECIFIC_CAPABILITIES_LENGTH (16u)   /* Size of the Codec Specific Capabilities buffer of the registered PACS records */
#define PACS_SRV_METADATA_SIZE                  (4u)    /* Size of the metadata associated to the registered PACS records */

#define MIN_NUM_BAP_SNK_PAC_RECORDS             (1u)    /* 2 Audio Sink Mandatory codec specific capabilities (cf BAP Specification) -> Into 1 PAC Record*/
#define MIN_NUM_BAP_SRC_PAC_RECORDS             (1u)    /* 1 Audio Source Mandatory codec specific capabilities (cf BAP Specification) -> Into 1 PAC Record*/
#define MIN_NUM_GMAP_UGT_SNK_PAC_RECORDS        (1u)    /* 6 Audio Sink Mandatory codec specific capabilities for GMAP Unicast Game Terminal role (cf GMAP Specification) -> Into 1 PAC Record*/
#define MIN_NUM_GMAP_UGT_SRC_PAC_RECORDS        (1u)    /* 6 Audio Source Mandatory codec specific capabilities for GMAP Unicast Game Terminal role (cf GMAP Specification) -> Into 1 PAC Record*/

#if ((APP_AUDIO_ROLE & AUDIO_ROLE_SINK) == AUDIO_ROLE_SINK)
#define APP_NUM_SNK_PAC_RECORDS                 (MIN_NUM_BAP_SNK_PAC_RECORDS \
                                                + (((APP_GMAP_ROLE & GMAP_ROLE_UNICAST_GAME_TERMINAL) == (GMAP_ROLE_UNICAST_GAME_TERMINAL)) ? (MIN_NUM_GMAP_UGT_SNK_PAC_RECORDS) : (0)))
#else
#define APP_NUM_SNK_PAC_RECORDS                 (0u)
#endif /* ((APP_AUDIO_ROLE & AUDIO_ROLE_SINK) == AUDIO_ROLE_SINK) */

#if ((APP_AUDIO_ROLE & AUDIO_ROLE_SOURCE) == AUDIO_ROLE_SOURCE)
#define APP_NUM_SRC_PAC_RECORDS                 (MIN_NUM_BAP_SRC_PAC_RECORDS \
                                                + (((APP_GMAP_ROLE & GMAP_ROLE_UNICAST_GAME_TERMINAL) == (GMAP_ROLE_UNICAST_GAME_TERMINAL)) ? (MIN_NUM_GMAP_UGT_SRC_PAC_RECORDS) : (0)))
#else
#define APP_NUM_SRC_PAC_RECORDS                 (0u)
#endif /* ((APP_AUDIO_ROLE & AUDIO_ROLE_SOURCE) == AUDIO_ROLE_SOURCE) */

/**
 * Audio Stream Endpoint Settings used in Unicast mode
 */
#if (((APP_AUDIO_ROLE & AUDIO_ROLE_SINK) == AUDIO_ROLE_SINK) \
    && ((APP_GMAP_ROLE & GMAP_ROLE_UNICAST_GAME_TERMINAL) != 0))
#define APP_NUM_SNK_ASE                         (2u)    /* Number of Sink ASEs to register */
#define APP_MAX_CHANNEL_PER_SNK_ASE             (2u)    /* Maximum number of channels per Sink ASE */
#else
#define APP_NUM_SNK_ASE                         (0u)    /* Number of Sink ASEs to register */
#define APP_MAX_CHANNEL_PER_SNK_ASE             (0u)    /* Maximum number of channels per Sink ASE */
#endif /* (((APP_AUDIO_ROLE & AUDIO_ROLE_SINK) == AUDIO_ROLE_SINK) && ((APP_GMAP_ROLE & GMAP_ROLE_UNICAST_GAME_TERMINAL) != 0)) */
#if ((APP_AUDIO_ROLE & AUDIO_ROLE_SOURCE) == AUDIO_ROLE_SOURCE)
#define APP_NUM_SRC_ASE                         (1u)    /* Number of Source ASEs to register */
#define APP_MAX_CHANNEL_PER_SRC_ASE             (1u)    /* Maximum number of channels per Source ASE */
#else
#define APP_NUM_SRC_ASE                         (0u)    /* Number of Source ASEs to register */
#define APP_MAX_CHANNEL_PER_SRC_ASE             (0u)    /* Maximum number of channels per Source ASE */
#endif /* ((APP_AUDIO_ROLE & AUDIO_ROLE_SOURCE) == AUDIO_ROLE_SOURCE) */

#if ((APP_GMAP_ROLE & GMAP_ROLE_UNICAST_GAME_TERMINAL) != 0)
#define MAX_USR_METADATA_SIZE                   (10u)   /* Maximum size of the Metadata for each Audio Stream Endpoint supported by Unicast Server */
#endif /* ((APP_GMAP_ROLE & GMAP_ROLE_UNICAST_GAME_TERMINAL) != 0)*/

/**
 * Audio Contexts Settings
 */
#define SUPPORTED_SNK_MEDIA_RECEIVER_CONTEXTS   (AUDIO_CONTEXT_GAME | AUDIO_CONTEXT_MEDIA | AUDIO_CONTEXT_ALERTS | AUDIO_CONTEXT_RINGTONE | AUDIO_CONTEXT_SOUND_EFFECTS)
#define SUPPORTED_SNK_CALL_TERMINAL_CONTEXTS    (AUDIO_CONTEXT_CONVERSATIONAL | AUDIO_CONTEXT_MEDIA | AUDIO_CONTEXT_ALERTS | AUDIO_CONTEXT_RINGTONE | AUDIO_CONTEXT_SOUND_EFFECTS)
#define SUPPORTED_SNK_CONTEXTS                  (AUDIO_CONTEXT_UNSPECIFIED | SUPPORTED_SNK_CALL_TERMINAL_CONTEXTS| SUPPORTED_SNK_MEDIA_RECEIVER_CONTEXTS)

#define SUPPORTED_SRC_CALL_TERMINAL_CONTEXTS    (AUDIO_CONTEXT_CONVERSATIONAL)
#define SUPPORTED_SRC_CONTEXTS                  (AUDIO_CONTEXT_UNSPECIFIED | SUPPORTED_SRC_CALL_TERMINAL_CONTEXTS)

/**
 * GAP Configuration Settings
 */
#define APPBLE_GAP_DEVICE_NAME_LENGTH           (14u)
#define BD_ADDR_SIZE_LOCAL                      (6u)
#define BAP_ADV_DATA_LEN                        (18u)
#define ADV_LOCAL_NAME_LEN                      (APPBLE_GAP_DEVICE_NAME_LENGTH+1)
#define ADV_EXT_LEN                             (10u)
#define ADV_AD_FLAGS_LEN                        (3u)
#define CAP_ADV_DATA_LEN                        (BAP_ADV_DATA_LEN+13u)
#define GMAP_USR_ADV_DATA_LEN                   (10u)
#define ADV_LEN                                 (ADV_AD_FLAGS_LEN+ADV_LOCAL_NAME_LEN+CAP_ADV_DATA_LEN+ADV_EXT_LEN+GMAP_USR_ADV_DATA_LEN+10)

/**
 * Volume Control Profile Settings
 */
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

/**
 * Microphone Control Profile Settings
 */
#if (APP_MICP_ROLE_DEVICE_SUPPORT == 1u)
#define APP_MICP_DEV_NUM_AIC_INSTANCES                  (1u)
#define APP_MICP_DEV_AIC_DESCRIPTION_LENGTH             (20u)
#else /*(APP_MICP_ROLE_DEVICE_SUPPORT == 0u)*/
#define APP_MICP_DEV_NUM_AIC_INSTANCES                  (0u)
#define APP_MICP_DEV_AIC_DESCRIPTION_LENGTH             (0u)
#endif /* (APP_MICP_ROLE_DEVICE_SUPPORT == 1u) */

/**
 * Coordinated Set Profile Settings
 */
#if (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1u)
#define APP_CSIP_SET_MEMBER_SIZE                        (2u)
#define APP_CSIP_SET_MEMBER_NUM_INSTANCES               (1u)
#define APP_SIRK                                        "STM32WBA_GMASIRK"
#else /*(APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 0u)*/
#define APP_CSIP_SET_MEMBER_NUM_INSTANCES               (0u)
#endif /* (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1u) */

/**
 * Maximum number of Services that can be stored in the GATT database.
 * Note that the GAP and GATT services are automatically added so this parameter should be 2 plus the number of user services
 */
#define BLE_HOST_NUM_GATT_SERVICES                      (2u)
#define BLE_APP_PACS_SRV_NUM_GATT_SERVICES              BAP_PACS_SRV_NUM_GATT_SERVICES
#define BLE_APP_ASCS_SRV_NUM_GATT_SERVICES              BAP_ASCS_SRV_NUM_GATT_SERVICES
#if (APP_BAP_ROLE_SCAN_DELEGATOR_SUPPORT == 1u)
#define BLE_APP_BASS_SRV_NUM_GATT_SERVICES              BAP_BASS_SRV_NUM_GATT_SERVICES
#else /*(APP_BAP_ROLE_SCAN_DELEGATOR_SUPPORT == 0u)*/
#define BLE_APP_BASS_SRV_NUM_GATT_SERVICES              0u
#endif/*(APP_BAP_ROLE_SCAN_DELEGATOR_SUPPORT == 1u)*/
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
#define BLE_APP_GMAP_NUM_GATT_SERVICES                  GMAP_NUM_GATT_SERVICES

#define BLE_APP_NUM_GATT_SERVICES                       (BLE_HOST_NUM_GATT_SERVICES \
                                                        + BLE_APP_PACS_SRV_NUM_GATT_SERVICES \
                                                        + BLE_APP_ASCS_SRV_NUM_GATT_SERVICES \
                                                        + BLE_APP_BASS_SRV_NUM_GATT_SERVICES \
                                                        + BLE_APP_VCP_RENDERER_NUM_GATT_SERVICES \
                                                        + BLE_APP_MICP_DEVICE_NUM_GATT_SERVICES \
                                                        + BLE_APP_CAP_ACC_NUM_GATT_SERVICES \
                                                        + BLE_APP_CSIP_SET_MEMBER_NUM_GATT_SERVICES \
                                                        + BLE_APP_GMAP_NUM_GATT_SERVICES)

/**
 * Maximum number of Attributes
 */
#if ((CFG_BLE_OPTIONS & BLE_OPTIONS_ENHANCED_ATT) == BLE_OPTIONS_ENHANCED_ATT)
#define BLE_HOST_NUM_GATT_ATTRIBUTES                    (15u)
#else
#define BLE_HOST_NUM_GATT_ATTRIBUTES                    (11u)
#endif /*((CFG_BLE_OPTIONS & BLE_OPTIONS_ENHANCED_ATT) == BLE_OPTIONS_ENHANCED_ATT)*/

#define BLE_APP_PACS_SRV_NUM_GATT_ATTRIBUTES            BAP_PACS_SRV_GATT_ATTRIBUTES(APP_NUM_SRC_PAC_RECORDS,APP_NUM_SNK_PAC_RECORDS)
#define BLE_APP_ASCS_SRV_NUM_GATT_ATTRIBUTES            BAP_ASCS_SRV_GATT_ATTRIBUTES(APP_NUM_SNK_ASE,APP_NUM_SRC_ASE)
#if (APP_BAP_ROLE_SCAN_DELEGATOR_SUPPORT == 1u)
#define BLE_APP_BASS_SRV_NUM_GATT_ATTRIBUTES            BAP_BASS_SRV_GATT_ATTRIBUTES(MAX_NUM_SDE_BSRC_INFO)
#else /*(APP_BAP_ROLE_SCAN_DELEGATOR_SUPPORT == 0u)*/
#define BLE_APP_BASS_SRV_NUM_GATT_ATTRIBUTES            0u
#endif/*(APP_BAP_ROLE_SCAN_DELEGATOR_SUPPORT == 1u)*/
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

#define BLE_APP_GMAP_NUM_GATT_ATTRIBUTES                GMAP_NUM_GATT_ATTRIBUTES

#define BLE_APP_NUM_GATT_ATTRIBUTES                     (BLE_HOST_NUM_GATT_ATTRIBUTES \
                                                         + BLE_APP_PACS_SRV_NUM_GATT_ATTRIBUTES \
                                                         + BLE_APP_ASCS_SRV_NUM_GATT_ATTRIBUTES \
                                                         + BLE_APP_BASS_SRV_NUM_GATT_ATTRIBUTES \
                                                         + BLE_APP_VCP_RENDERER_NUM_GATT_ATTRIBUTES \
                                                         + BLE_APP_MICP_DEVICE_NUM_GATT_ATTRIBUTES \
                                                         + BLE_APP_CSIP_SET_MEMBER_NUM_GATT_ATTRIBUTES \
                                                         + BLE_APP_GMAP_NUM_GATT_ATTRIBUTES)

/**
 * Size of the storage area for Attribute values
 */
#if ((CFG_BLE_OPTIONS & BLE_OPTIONS_ENHANCED_ATT) == BLE_OPTIONS_ENHANCED_ATT)
#define BLE_HOST_ATT_VALUE_ARRAY_SIZE                   (59u)
#else
#define BLE_HOST_ATT_VALUE_ARRAY_SIZE                   (49u)
#endif /*((CFG_BLE_OPTIONS & BLE_OPTIONS_ENHANCED_ATT) == BLE_OPTIONS_ENHANCED_ATT)*/
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
                    (APP_NUM_SNK_ASE + APP_NUM_SRC_ASE) \
                      * BAP_ASE_ATT_VALUE_ARRAY_SIZE(CFG_BLE_NUM_LINK,MAX_USR_CODEC_CONFIG_SIZE,MAX_USR_METADATA_SIZE)

#define BLE_APP_ASCS_SRV_ATT_VALUE_ARRAY_SIZE \
                  BAP_ASCS_SRV_ATT_VALUE_ARRAY_SIZE(CFG_BLE_NUM_LINK,BLE_TOTAL_ASES_ATT_VALUE_ARRAY_SIZE)

#if (APP_BAP_ROLE_SCAN_DELEGATOR_SUPPORT == 1u)
#define BLE_BASS_SRV_ATT_VALUE_ARRAY_SIZE \
                      BAP_BASS_SRV_ATT_VALUE_ARRAY_SIZE(MAX_NUM_SDE_BSRC_INFO,CFG_BLE_NUM_LINK,MAX_BASS_METADATA_SIZE, \
                                                        MAX_NUM_BASS_BASE_SUBGROUPS)
#else /*(APP_BAP_ROLE_SCAN_DELEGATOR_SUPPORT == 0u)*/
#define BLE_BASS_SRV_ATT_VALUE_ARRAY_SIZE               0u
#endif/*(APP_BAP_ROLE_SCAN_DELEGATOR_SUPPORT == 1u)*/
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
#define BLE_APP_CAP_ATT_VALUE_ARRAY_SIZE                CAP_ACC_ATT_VALUE_ARRAY_SIZE(APP_CSIP_SET_MEMBER_NUM_INSTANCES)
#else
#define BLE_APP_CSIP_SET_MEMBER_ATT_VALUE_ARRAY_SIZE    0u
#define BLE_APP_CAP_ATT_VALUE_ARRAY_SIZE                0u
#endif /*(APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1)*/

#define BLE_APP_GMAP_ATT_VALUE_ARRAY_SIZE               GMAP_ATT_VALUE_ARRAY_SIZE

#define BLE_APP_ATT_VALUE_ARRAY_SIZE                    (BLE_HOST_ATT_VALUE_ARRAY_SIZE \
                                                        + BLE_APP_PACS_SRV_ATT_VALUE_ARRAY_SIZE \
                                                        + BLE_APP_ASCS_SRV_ATT_VALUE_ARRAY_SIZE \
                                                        + BLE_BASS_SRV_ATT_VALUE_ARRAY_SIZE \
                                                        + BLE_APP_VCP_RENDERER_ATT_VALUE_ARRAY_SIZE \
                                                        + BLE_APP_MICP_DEVICE_ATT_VALUE_ARRAY_SIZE \
                                                        + BLE_APP_CSIP_SET_MEMBER_ATT_VALUE_ARRAY_SIZE \
														+ BLE_APP_CAP_ATT_VALUE_ARRAY_SIZE \
                                                        + BLE_APP_GMAP_ATT_VALUE_ARRAY_SIZE)

/**
 * Size of the storage area for Database of Audio Services/Profile to save in Non Volatile Memory
 */
/*Memory size required to store all devices in NVM for ASCS Server role*/
#define BAP_ASCS_SRV_NVM_ALLOC_SIZE             BAP_ASCS_SRV_DB_BUFFER_SIZE(APP_MAX_NUM_BONDED_DEVICES,\
                                                                            APP_ASCS_CACHING,\
                                                                            APP_NUM_SNK_ASE, \
                                                                            APP_NUM_SRC_ASE, \
                                                                            MAX_USR_CODEC_CONFIG_SIZE)

/*Memory size required to store all devices in NVM for PACS Server role*/
#define BAP_PACS_SRV_NVM_ALLOC_SIZE             BAP_PACS_SRV_DB_BUFFER_SIZE(APP_MAX_NUM_BONDED_DEVICES,\
                                                                            APP_NUM_SNK_PAC_RECORDS,\
                                                                            APP_NUM_SRC_PAC_RECORDS)

/*Memory size required to store all devices in NVM for CCP Client role*/
#if (APP_CCP_ROLE_CLIENT_SUPPORT == 1u)
#define BLE_CCP_CLT_NVM_ALLOC_SIZE              BLE_CCP_CLT_DB_BUFFER_SIZE(APP_MAX_NUM_BONDED_DEVICES,\
                                                                           APP_CCP_NUM_REMOTE_BEARER_INSTANCES)
#else
#define BLE_CCP_CLT_NVM_ALLOC_SIZE              (0u)
#endif /*(APP_CCP_ROLE_CLIENT_SUPPORT == 1u)*/

#if (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1)
/*Memory size required to store all devices in NVM for CSIP in Set Member role*/
#define BLE_CSIP_SET_MEMBER_NVM_ALLOC_SIZE      BLE_CSIP_SET_MEMBER_DB_BUFFER_SIZE(APP_MAX_NUM_BONDED_DEVICES,\
                                                                                    APP_CSIP_SET_MEMBER_NUM_INSTANCES)
#else
#define BLE_CSIP_SET_MEMBER_NVM_ALLOC_SIZE      (0u)
#endif /* (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1) */

#define BLE_APP_AUDIO_NVM_ALLOC_SIZE            (BAP_ASCS_SRV_NVM_ALLOC_SIZE + BAP_PACS_SRV_NVM_ALLOC_SIZE \
                                                + BLE_CCP_CLT_NVM_ALLOC_SIZE + BLE_CSIP_SET_MEMBER_NVM_ALLOC_SIZE \
                                                + BLE_AUDIO_DB_BUFFER_SIZE(APP_MAX_NUM_BONDED_DEVICES,GATTSERVICE_GATT_DATABASE_SIZE))

/* Exported types ------------------------------------------------------------*/

/* External variables --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /*__GMAP_APP_CONF_H */

