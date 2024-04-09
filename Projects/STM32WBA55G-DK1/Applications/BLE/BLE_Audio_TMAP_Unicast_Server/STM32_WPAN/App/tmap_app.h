/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    TMAPAPP_.h
  * @author  MCD Application Team
  * @brief   Header for tmap_app_.c module
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
#ifndef __CAP_APP_H
#define __CAP_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wbaxx_hal.h"
#include "app_conf.h"
#include "cap.h"
#include "bap_bufsize.h"
#include "ccp.h"
#include "mcp.h"
#include "csip.h"
#include "vcp.h"
#include "tmap.h"
#include "stm32_timer.h"
/* Private includes ----------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/*Audio Profiles Roles configuration*/
#define APP_TMAP_ROLE                                   (TMAP_ROLE_CALL_TERMINAL | TMAP_ROLE_UNICAST_MEDIA_RECEIVER)
#define APP_CAP_ROLE                                    (CAP_ROLE_ACCEPTOR)

#define GAP_APPEARANCE_EARBUD                           (0x0941u)

#define APP_BAP_ROLE_UNICAST_SERVER_SUPPORT             (1u)
#define APP_BAP_ROLE_UNICAST_CLIENT_SUPPORT             (0u)
#define APP_BAP_ROLE_BROADCAST_SOURCE_SUPPORT           (0u)
#define APP_BAP_ROLE_BROADCAST_SINK_SUPPORT             (0u)
#define APP_BAP_ROLE_SCAN_DELEGATOR_SUPPORT             (0u)
#define APP_BAP_ROLE_BROADCAST_ASSISTANT_SUPPORT        (0u)

#define APP_CCP_ROLE_SERVER_SUPPORT                     (0u)
#define APP_CCP_ROLE_CLIENT_SUPPORT                     (1u)

#define APP_MCP_ROLE_SERVER_SUPPORT                     (0u)
#define APP_MCP_ROLE_CLIENT_SUPPORT                     (1u)

#define APP_VCP_ROLE_CONTROLLER_SUPPORT                 (0u)
#define APP_VCP_ROLE_RENDERER_SUPPORT                   (1u)


#define NUM_LC3_CODEC_CAP       16
#define NUM_LC3_CODEC_CONFIG    16
#define LC3_8_1                 0
#define LC3_8_2                 1
#define LC3_16_1                2
#define LC3_16_2                3
#define LC3_24_1                4
#define LC3_24_2                5
#define LC3_32_1                6
#define LC3_32_2                7
#define LC3_441_1               8
#define LC3_441_2               9
#define LC3_48_1                10
#define LC3_48_2                11
#define LC3_48_3                12
#define LC3_48_4                13
#define LC3_48_5                14
#define LC3_48_6                15

#define NUM_LC3_QoSConf         32
#define LC3_QOS_8_1_1           0
#define LC3_QOS_8_2_1           1
#define LC3_QOS_16_1_1          2
#define LC3_QOS_16_2_1          3
#define LC3_QOS_24_1_1          4
#define LC3_QOS_24_2_1          5
#define LC3_QOS_32_1_1          6
#define LC3_QOS_32_2_1          7
#define LC3_QOS_441_1_1         8
#define LC3_QOS_441_2_1         9
#define LC3_QOS_48_1_1          10
#define LC3_QOS_48_2_1          11
#define LC3_QOS_48_3_1          12
#define LC3_QOS_48_4_1          13
#define LC3_QOS_48_5_1          14
#define LC3_QOS_48_6_1          15
#define LC3_QOS_8_1_2           16
#define LC3_QOS_8_2_2           17
#define LC3_QOS_16_1_2          18
#define LC3_QOS_16_2_2          19
#define LC3_QOS_24_1_2          20
#define LC3_QOS_24_2_2          21
#define LC3_QOS_32_1_2          22
#define LC3_QOS_32_2_2          23
#define LC3_QOS_441_1_2         24
#define LC3_QOS_441_2_2         25
#define LC3_QOS_48_1_2          26
#define LC3_QOS_48_2_2          27
#define LC3_QOS_48_3_2          28
#define LC3_QOS_48_4_2          29
#define LC3_QOS_48_5_2          30
#define LC3_QOS_48_6_2          31

/*Audio Configuration specified in Basic Audio Profile Specification*/
#define NUM_USR_AUDIO_CONF      (16u)
#define USR_AUDIO_CONF_1        0       /* Single Audio Channel. One unidirectional CIS. Unicast Server is Audio Sink
                                         * Mandatory :
                                         *      - 1 Sink PAC
                                         *              Supported_Audio_Channel_Counts : Optional (if present, include at least SUPPORTED_AUDIO_CHNL_COUNT_1)
                                         *      - 1 Sink ASE
                                         *              Audio_Channel_Allocation : Optional (if present, match with Sink Audio Locations)
                                         * Optional :
                                         *      - Sink Audio Locations
                                         */
#define USR_AUDIO_CONF_2        1       /* Single Audio Channel. One unidirectional CIS. Unicast Server is Audio Source.
                                         * Mandatory :
                                         *      - 1 Source PAC
                                         *              Supported_Audio_Channel_Counts : Optional (if present, include at least SUPPORTED_AUDIO_CHNL_COUNT_1)
                                         *      - 1 Source ASE
                                         *              Audio_Channel_Allocation : Optional (if present, match with Source Audio Locations)
                                         * Optional :
                                         *      - Source Audio Locations
                                         */
#define USR_AUDIO_CONF_3        2       /* Multiple Audio Channels. One bidirectional CIS. Unicast Server is Audio Sink and Audio Source.
                                         * Mandatory :
                                         *      - 1 Sink PAC
                                         *              Supported_Audio_Channel_Counts : Optional (if present, include at least SUPPORTED_AUDIO_CHNL_COUNT_1)
                                         *      - 1 Sink ASE
                                         *              Audio_Channel_Allocation : Optional (if present, match with Sink Audio Locations)
                                         *      - 1 Source PAC
                                         *              Supported_Audio_Channel_Counts : Optional (if present, include at least SUPPORTED_AUDIO_CHNL_COUNT_1)
                                         *      - 1 Source ASE
                                         *              Audio_Channel_Allocation : Optional (if present, match with Source Audio Locations)
                                         * Optional :
                                         *      - Sink Audio Locations
                                         *      - Source Audio Locations
                                         */
#define USR_AUDIO_CONF_4        3       /* Multiple Audio Channels. One unidirectional CIS. Unicast Server is Audio Sink.
                                         * Mandatory :
                                         *      - 1 Sink PAC
                                         *              Supported_Audio_Channel_Counts : Mandatory (if present, include at least SUPPORTED_AUDIO_CHNL_COUNT_2)
                                         *      - 1 Sink ASE
                                         *              Audio_Channel_Allocation : Mandatory (if present, match with two bits set to 0b1 in Sink Audio Locations)
                                         *      - Sink Audio Locations (at least two bits set to 0b1)
                                         */
#define USR_AUDIO_CONF_5        4       /* Multiple Audio Channels. One bidirectional CIS. Unicast Server is Audio Sink and Audio Source.* Mandatory :
                                         * Mandatory :
                                         *      - 1 Sink PAC
                                         *              Supported_Audio_Channel_Counts : Mandatory (if present, include at least SUPPORTED_AUDIO_CHNL_COUNT_2)
                                         *      - 1 Sink ASE
                                         *              Audio_Channel_Allocation : Mandatory (if present, match with two bits set to 0b1 in Sink Audio Locations)
                                         *      - 1 Source PAC
                                         *              Supported_Audio_Channel_Counts : Optional (if present, include at least SUPPORTED_AUDIO_CHNL_COUNT_1)
                                         *      - 1 Source ASE
                                         *              Audio_Channel_Allocation : Optional (if present, match with Source Audio Locations)
                                         *      - Sink Audio Locations (at least two bits set to 0b1)
                                         * Optional :
                                         *      - Source Audio Locations
                                         */
#define USR_AUDIO_CONF_6_I      5       /* Multiple Audio Channels. Two unidirectional CISes. Unicast Server is Audio Sink.
                                         * Mandatory :
                                         *      - 1 Sink PAC
                                         *              Supported_Audio_Channel_Counts : Optional (if present, only SUPPORTED_AUDIO_CHNL_COUNT_1)
                                         *      - 2 Sink ASE
                                         *              Audio_Channel_Allocation : Mandatory.
                                         *              Each ASEs has a different Audio_Channel_Allocation value which match with one of the two bits set to 0b1 in Sink Audio Locations
                                         *      - Sink Audio Locations (at least two bits set to 0b1)
                                         */
#define USR_AUDIO_CONF_6_II     6       /* There is 2 USRs with one Sink ASE per USR with its proper Audio Locations
                                         * Multiple Audio Channels. Two unidirectional CISes. Two Unicast Servers. Unicast Server 1 is Audio Sink. Unicast Server 2 is Audio Sink.
                                         * Mandatory :
                                         *      - 1 Sink PAC
                                         *              Supported_Audio_Channel_Counts : Optional (if present, at least SUPPORTED_AUDIO_CHNL_COUNT_1)
                                         *      - 1 Sink ASE
                                         *              Audio_Channel_Allocation : Mandatory (match with Sink Audio Locations)
                                         *      - Sink Audio Locations (at least 1 bit set to 0b1 but different to the other USR)
                                         */
#define USR_AUDIO_CONF_7_I      7       /* Same as config 3 but on 2 CIS unidirectionnal insteaf of 1 CIS bidirectionnal
                                         * Multiple Audio Channels. Two unidirectional CISes. Unicast Server is Audio Sink and Audio Source.
                                         * Mandatory :
                                         *      - 1 Sink PAC
                                         *              Supported_Audio_Channel_Counts : Optional (if present, include at least SUPPORTED_AUDIO_CHNL_COUNT_1)
                                         *      - 1 Sink ASE
                                         *              Audio_Channel_Allocation : Optional (if present, match with Sink Audio Locations)
                                         *      - 1 Source PAC
                                         *              Supported_Audio_Channel_Counts : Optional (if present, include at least SUPPORTED_AUDIO_CHNL_COUNT_1)
                                         *      - 1 Source ASE
                                         *              Audio_Channel_Allocation : Optional (if present, match with Source Audio Locations)
                                         * Optional :
                                         *      - Sink Audio Locations
                                         *      - Source Audio Locations
                                         */
#define USR_AUDIO_CONF_7_II     8       /* One USR has 1 Snk ASE and the other USR has 1 Src ASE
                                         * Multiple Audio Channels. Two Unidirectional CISes. Two Unicast Servers.
                                         * One USR is similar to Audio Configuration (1)
                                         * One USR is similar to Audio Configuration (2)
                                         */
#define USR_AUDIO_CONF_8_I      9       /* Multiple Audio Channels. One bidirectional CIS and one unidirectional CIS. Unicast Server is Audio Sink and Audio Source.
                                         * Mandatory :
                                         *      - 1 Sink PAC
                                         *              Supported_Audio_Channel_Counts : Optional (if present, include at least SUPPORTED_AUDIO_CHNL_COUNT_1)
                                         *      - 1 Source PAC
                                         *              Supported_Audio_Channel_Counts : Optional (if present, include at least SUPPORTED_AUDIO_CHNL_COUNT_1)
                                         *      - 2 Sink ASE
                                         *              Audio_Channel_Allocation : Mandatory.
                                         *              Each ASEs has a different Audio_Channel_Allocation value which match with one of the two bits set to 0b1 in Sink Audio Locations
                                         *      - 1 Source ASE
                                         *              Audio_Channel_Allocation : Optional (if present, match with Source Audio Locations)
                                         *      - Sink Audio Locations (at least two bits set to 0b1)
                                         * Optional :
                                         *      - Source Audio Locations
                                         */
#define USR_AUDIO_CONF_8_II     10      /* 2 USRs, other USR should have one Snk ASE, with 1 Audio Channel and one Audio locations : At UCL point of view, there is 2 Snk ASE and one Src ASE
                                         * Multiple Audio Channels. One bidirectional CIS and one unidirectional CIS. Two Unicast Servers. Unicast
                                         * Mandatory :
                                         *      - 1 Sink PAC
                                         *              Supported_Audio_Channel_Counts : Optional (if present, include at least SUPPORTED_AUDIO_CHNL_COUNT_1)
                                         *      - 1 Source PAC
                                         *              Supported_Audio_Channel_Counts : Optional (if present, include at least SUPPORTED_AUDIO_CHNL_COUNT_1)
                                         *      - 1 Sink ASE
                                         *              Audio_Channel_Allocation : Mandatory.(match with one bits set to 0b1 in Sink Audio Locations)
                                         *      - 1 Source ASE
                                         *              Audio_Channel_Allocation : Optional (if present, match with Source Audio Locations)
                                         *      - Sink Audio Locations (at least 1 bits set to 0b1 and different to other Sink Audio Locations of 2nd USR)
                                         * Optional :
                                         *      - Source Audio Locations
                                         *
                                         * Note The other USR is similar of the USR in Audio Configuration (6ii)
                                         */
#define USR_AUDIO_CONF_9_I      11      /* Multiple Audio Channels. Two unidirectional CISes. Unicast Server is Audio Source.
                                         * Mandatory :
                                         *      - 1 Source PAC
                                         *              Supported_Audio_Channel_Counts : Optional (if present, only SUPPORTED_AUDIO_CHNL_COUNT_1)
                                         *      - 2 Source ASE
                                         *              Audio_Channel_Allocation : Mandatory.
                                         *              Each ASEs has a different Audio_Channel_Allocation value which match with one of the two bits set to 0b1 in Source Audio Locations
                                         *      - Source Audio Locations (at least two bits set to 0b1)
                                         */
#define USR_AUDIO_CONF_9_II     12      /* There is 2 USRs with one Source ASE per USR with its proper Audio Locations
                                         * Multiple Audio Channels. Two unidirectional CISes. Two Unicast Servers. Unicast Server 1 is Audio Sink. Unicast Source 2 is Audio Source.
                                         * Mandatory :
                                         *      - 1 Source PAC
                                         *              Supported_Audio_Channel_Counts : Optional (if present, at least SUPPORTED_AUDIO_CHNL_COUNT_1)
                                         *      - 1 Source ASE
                                         *              Audio_Channel_Allocation : Mandatory (match with Source Audio Locations)
                                         *      - Source Audio Locations (at least 1 bit set to 0b1 but different to the other USR)
                                         */
#define USR_AUDIO_CONF_10       13      /* Multiple Audio Channels. One unidirectional CIS. Unicast Server is Audio Source.
                                         * Equivalent to Audio Configuration (4) in Source mode
                                         * Mandatory :
                                         *      - 1 Source PAC
                                         *              Supported_Audio_Channel_Counts : Mandatory (if present, include at least SUPPORTED_AUDIO_CHNL_COUNT_2)
                                         *      - 1 Source ASE
                                         *              Audio_Channel_Allocation : Mandatory (if present, match with two bits set to 0b1 in Source Audio Locations)
                                         *      - Source Audio Locations (at least two bits set to 0b1)
                                         */
#define USR_AUDIO_CONF_11_I     14      /* Multiple Audio Channels. Two bidirectional CISes. Unicast Server is Audio Sink and Audio Source
                                         * Mandatory :
                                         *      - 1 Sink PAC
                                         *              Supported_Audio_Channel_Counts : Optional (if present, include at least SUPPORTED_AUDIO_CHNL_COUNT_1)
                                         *      - 1 Source PAC
                                         *              Supported_Audio_Channel_Counts : Optional (if present, include at least SUPPORTED_AUDIO_CHNL_COUNT_1)
                                         *      - 2 Sink ASE
                                         *              Audio_Channel_Allocation : Mandatory.
                                         *              Each ASEs has a different Audio_Channel_Allocation value which match with one bits set to 0b1 in Sink Audio Locations
                                         *      - 2 Source ASE
                                         *              Audio_Channel_Allocation : Mandatory.
                                         *              Each ASEs has a different Audio_Channel_Allocation value which match with one bits set to 0b1 in Source Audio Locations
                                         *      - Sink Audio Locations (at least 2 bits set to 0b1 )
                                         *      - Source Audio Locations (at least 2 bits set to 0b1 )
                                         *
                                         */
#define USR_AUDIO_CONF_11_II    15      /* There is 2 USRs with one Sink ASE per USR with its proper Audio Locations and one Source ASE per USR with its proper Audio Locations
                                         * Multiple Audio Channels. Two bidirectional CISes. Two Unicast Servers.
                                         * Mandatory :
                                         *      - 1 Sink PAC
                                         *              Supported_Audio_Channel_Counts : Optional (if present, include at least SUPPORTED_AUDIO_CHNL_COUNT_1)
                                         *      - 1 Source PAC
                                         *              Supported_Audio_Channel_Counts : Optional (if present, include at least SUPPORTED_AUDIO_CHNL_COUNT_1)
                                         *      - 1 Sink ASE
                                         *              Audio_Channel_Allocation : Mandatory ( match with one bits set to 0b1 in Sink Audio Locations)
                                         *                                         Different to the Audio_Channel_Allocation for the other USR
                                         *      - A Source ASE
                                         *              Audio_Channel_Allocation : Mandatory ( match with one bits set to 0b1 in Source Audio Locations)
                                         *                                         Different to the Audio_Channel_Allocation for the other USR
                                         *      - Sink Audio Locations (at least 1 bits set to 0b1 different to the Sink Audio Locations of the 2nd USR )
                                         *      - Source Audio Locations (at least 1 bits set to 0b1 different to the Source Audio Locations of the 2nd USR )
                                         */

/*Server Preferred QoS Settings */
#define APP_DEFAULT_BAP_FRAMING                 BAP_FRAMING_UNFRAMED
#define APP_DEFAULT_BAP_PREF_PHY                NO_PHY_PREFERRED
#define APP_DEFAULT_BAP_PREF_RETRANSMISSIONS    (2u)
#define APP_DEFAULT_BAP_MAX_TRANSPORT_LATENCY   (0x0005)

/* Allow the Unicast Server to accept Audio High Reliability during Audio setup process
 * Note High Reliability increases the audio latency
 */
#define ENABLE_AUDIO_HIGH_RELIABILITY           (0u)

/* These delays refers to the time at which the audio signal passes through an
 * audio interface (such an electroacoustic transducer ) to or from
 * the Codec interface.
 * These delays are a subpart of the Presentation Delay as descibed in chapter 7
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

/*TMAP and CAP Configuration Settings*/
#define MAX_NUM_USR_LINK                        CFG_BLE_NUM_LINK        /* Maximum number of Link supported by Unicast Server*/

#define MAX_NUM_CIG                             (1u)                    /* Maximum number of CIGes */
#define MAX_NUM_CIS_PER_CIG                     (2u)                    /* Maximum number of CISes per CIG */
#define MAX_NUM_BIG                             (0u)                    /* Maximum number of BIGes */
#define MAX_NUM_BIS_PER_BIG                     (0u)                    /* Maximum number of BISes per BIG */

#define MAX_USR_CODEC_CONFIG_SIZE               (20u)                   /* Maximum size of the Codec Specific Configuration for each Audio Stream Endpoint supported by Unicast Server */
#define MAX_USR_METADATA_SIZE                   (10u)                   /* Maximum size of the Metadata for each Audio Stream Endpoint supported by Unicast Server */
#define PACS_CODEC_SPECIFIC_CAPABILITIES_LENGTH (16u)                   /* Size of the Codec Specific Capabilites Length of the registered PACS records */
#define PACS_SRV_METADATA_SIZE                  (4u)                    /* Size of the metadata associated to the registered PACS records */


#define MIN_NUM_BAP_SNK_PAC_RECORDS             (2u)                    /* 2 Audio Sink Mandatory codec specific capabilities (cf BAP Specification)*/
#define MIN_NUM_BAP_SRC_PAC_RECORDS             (1u)                    /* 1 Audio Source Mandatory codec specific capabilities (cf BAP Specification)*/
#define MIN_NUM_TMAP_CT_SNK_PAC_RECORDS         (3u)                    /* 3 Audio Sink Mandatory codec specific capabilities for TMAP Call Terminal role (cf TMAP Specification)*/
#define MIN_NUM_TMAP_CT_SRC_PAC_RECORDS         (6u)                    /* 3 Audio Source Mandatory codec specific capabilities for TMAP Call Terminal role (cf TMAP Specification)*/
#define MIN_NUM_TMAP_UMR_SNK_PAC_RECORDS        (6u)                    /* 6 Audio Sink Mandatory codec specific capabilities for TMAP Unicast Media Receiver role (cf TMAP Specification)*/

#define APP_NUM_SNK_PAC_RECORDS                 (MIN_NUM_BAP_SNK_PAC_RECORDS \
                                                + (((APP_TMAP_ROLE & TMAP_ROLE_CALL_TERMINAL) == (TMAP_ROLE_CALL_TERMINAL)) ? (MIN_NUM_TMAP_CT_SNK_PAC_RECORDS) : (0)) \
                                                + (((APP_TMAP_ROLE & TMAP_ROLE_UNICAST_MEDIA_RECEIVER) == (TMAP_ROLE_UNICAST_MEDIA_RECEIVER)) ? (MIN_NUM_TMAP_UMR_SNK_PAC_RECORDS) : (0)))

#define APP_NUM_SRC_PAC_RECORDS                 (MIN_NUM_BAP_SRC_PAC_RECORDS \
                                                + (((APP_TMAP_ROLE & TMAP_ROLE_CALL_TERMINAL) == (TMAP_ROLE_CALL_TERMINAL)) ? (MIN_NUM_TMAP_CT_SRC_PAC_RECORDS) : (0)))

#define MAX_NUM_USR_SNK_ASE                     (2u)                    /* Maximum number of Sink Audio Stream Endpoints supported by Unicast Server*/
#define MAX_NUM_USR_SRC_ASE                     (2u)                    /* Maximum number of Source Audio Stream Endpoints supported by Unicast Server*/
#define APP_MAX_NUM_CIS                         MAX_NUM_CIS_PER_CIG
#define COMPANY_ID                              0x0000
#define MONO_AUDIO_LOCATIONS                    FRONT_LEFT
#define STEREO_AUDIO_LOCATIONS                  (FRONT_LEFT|FRONT_RIGHT)
#define SUPPORTED_SNK_CONTEXTS                  (AUDIO_CONTEXT_UNSPECIFIED | AUDIO_CONTEXT_CONVERSATIONAL | AUDIO_CONTEXT_MEDIA | \
                                                 AUDIO_CONTEXT_GAME | AUDIO_CONTEXT_NOTIFICATIONS | AUDIO_CONTEXT_RINGTONE | \
                                                 AUDIO_CONTEXT_ALERTS | AUDIO_CONTEXT_EMERGENCY_ALARM |AUDIO_CONTEXT_LIVE)
#define SUPPORTED_SRC_CONTEXTS                  (AUDIO_CONTEXT_UNSPECIFIED | AUDIO_CONTEXT_CONVERSATIONAL | AUDIO_CONTEXT_MEDIA | \
                                                AUDIO_CONTEXT_GAME |AUDIO_CONTEXT_LIVE)

/*GAP Configuration Settings*/
#define APPBLE_GAP_DEVICE_NAME_LENGTH           (14u)
#define BD_ADDR_SIZE_LOCAL                      (6u)
#define BAP_ADV_DATA_LEN                        (18u)
#define ADV_LOCAL_NAME_LEN                      (APPBLE_GAP_DEVICE_NAME_LENGTH+1)
#define ADV_EXT_LEN                             (10u)
#define ADV_AD_FLAGS_LEN                        (3u)
#define CAP_ADV_DATA_LEN                        (BAP_ADV_DATA_LEN+5u)
#define TMAP_ADV_DATA_LEN                       (10u)
#define ADV_LEN                                 (ADV_AD_FLAGS_LEN+ADV_LOCAL_NAME_LEN+CAP_ADV_DATA_LEN+ADV_EXT_LEN+TMAP_ADV_DATA_LEN+10)



/* Call Control Profile Settings*/
#if (APP_CCP_ROLE_CLIENT_SUPPORT == 1u)
#define APP_CCP_CLT_FEATURE_SUPPORT                     (1u)
#define APP_CCP_NUM_REMOTE_BEARER_INSTANCES             (2u)
#define APP_CCP_CALL_URI_LENGTH                         (30)
#else /*(APP_CCP_ROLE_CLIENT_SUPPORT == 0u)*/
#define APP_CCP_CLT_FEATURE_SUPPORT                     (0u)
#define APP_CCP_NUM_REMOTE_BEARER_INSTANCES             (0u)
#endif /* (APP_CCP_ROLE_CLIENT_SUPPORT == 1u) */



/* Media Control Profile Settings*/
#if (APP_MCP_ROLE_CLIENT_SUPPORT == 1u)
#define APP_MCP_CLT_FEATURE_SUPPORT                     (1u)
#define APP_MCP_NUM_REMOTE_MEDIA_PLAYER_INSTANCES       (2u)
#else /*(APP_MCP_ROLE_CLIENT_SUPPORT == 0u)*/
#define APP_MCP_CLT_FEATURE_SUPPORT                     (0u)
#define APP_MCP_NUM_REMOTE_MEDIA_PLAYER_INSTANCES       (0u)
#endif /* (APP_MCP_ROLE_CLIENT_SUPPORT == 1u) */

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
#define BLE_APP_CAP_ACC_NUM_GATT_SERVICES               CAP_ACC_COM_NUM_GATT_SERVICES
#define BLE_APP_TMAP_NUM_GATT_SERVICES                  TMAP_NUM_GATT_SERVICES


#define BLE_APP_NUM_GATT_SERVICES                       (2U + BLE_APP_PACS_SRV_NUM_GATT_SERVICES \
                                                        + BLE_APP_ASCS_SRV_NUM_GATT_SERVICES \
                                                        + BLE_APP_VCP_RENDERER_NUM_GATT_SERVICES \
                                                        + BLE_APP_CAP_ACC_NUM_GATT_SERVICES \
                                                        + BLE_APP_TMAP_NUM_GATT_SERVICES)
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
#define BLE_APP_TMAP_NUM_GATT_ATTRIBUTES                TMAP_NUM_GATT_ATTRIBUTES


#define BLE_APP_NUM_GATT_ATTRIBUTES                     (4u + 7u \
                                                         + BLE_APP_PACS_SRV_NUM_GATT_ATTRIBUTES \
                                                         + BLE_APP_ASCS_SRV_NUM_GATT_ATTRIBUTES \
                                                         + BLE_APP_VCP_RENDERER_NUM_GATT_ATTRIBUTES \
                                                         + BLE_APP_TMAP_NUM_GATT_ATTRIBUTES)

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
#define BLE_APP_TMAP_ATT_VALUE_ARRAY_SIZE               TMAP_MEM_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK)

#define BLE_APP_ATT_VALUE_ARRAY_SIZE                    (49u + BLE_APP_PACS_SRV_ATT_VALUE_ARRAY_SIZE \
                                                          + BLE_APP_ASCS_SRV_ATT_VALUE_ARRAY_SIZE \
                                                          + BLE_APP_VCP_RENDERER_ATT_VALUE_ARRAY_SIZE \
                                                          + BLE_APP_TMAP_ATT_VALUE_ARRAY_SIZE)
/* Exported types ------------------------------------------------------------*/
/* Audio Profile Link State type */
typedef uint8_t audio_profile_t;
#define AUDIO_PROFILE_NONE                      0x00u
#define AUDIO_PROFILE_UNICAST                   0x01u
#define AUDIO_PROFILE_BA                        0x02u
#define AUDIO_PROFILE_CSIP                      0x04u
#define AUDIO_PROFILE_CCP                       0x08u
#define AUDIO_PROFILE_MCP                       0x10u
#define AUDIO_PROFILE_VCP                       0x20u
#define AUDIO_PROFILE_MICP                      0x40u
#define AUDIO_PROFILE_TMAP                      0x80u

/*Structure used for ASE information storage*/
typedef struct
{
  uint8_t               ID;
  ASE_State_t           state;
  ASE_Type_t            type;           /*Source of Sink*/
  uint8_t               allocated;
  uint8_t               num_channels;
  uint32_t              presentation_delay;
  uint32_t              controller_delay;
  Audio_Context_t       streaming_audio_context;
}APP_ASE_Info_t;

typedef struct
{
  uint16_t              acl_conn_handle;
#if (MAX_NUM_USR_SNK_ASE > 0)
  APP_ASE_Info_t        aSnkASE[MAX_NUM_USR_SNK_ASE];
#endif /* (MAX_NUM_USR_SNK_ASE > 0) */
#if (MAX_NUM_USR_SRC_ASE > 0)
  APP_ASE_Info_t        aSrcASE[MAX_NUM_USR_SRC_ASE];
#endif /* (MAX_NUM_USR_SRC_ASE > 0) */
}APP_ASEs_t;


/*Structure used for connection information strorage*/
typedef struct
{
  uint16_t              Acl_Conn_Handle;
  uint8_t               Peer_Address_Type;
  uint8_t               Peer_Address[6];
  uint8_t               Role;
  audio_profile_t       AudioProfile;
  uint8_t               ConfirmIndicationRequired;
  uint8_t               ForceCompleteLinkup;
  uint8_t               GenericMediaPlayerCCID;
  MCP_MediaState_t      MediaState;
  uint8_t               PlayPauseOperation;
  uint8_t               GenericTelephoneBearerCCID;
  uint8_t               CurrentCallID;
  APP_ASEs_t            *pASEs;
}APP_ACL_Conn_t;

typedef struct
{
  BAP_Role_t                    bap_role;

  APP_ACL_Conn_t                ACL_Conn[CFG_BLE_NUM_LINK];

  uint8_t                       audio_driver_config;

  uint8_t                       AudioConfId;

  Audio_Role_t                  audio_role_setup;

  uint16_t                      cis_src_handle[APP_MAX_NUM_CIS];

  uint8_t                       num_cis_src;

  uint16_t                      cis_snk_handle[APP_MAX_NUM_CIS];

  uint8_t                       num_cis_snk;

  uint16_t                      cis_handle[APP_MAX_NUM_CIS];

  uint8_t                       num_cis_established;

  BAP_AudioCodecController_t    AudioCodecInController;

  BAP_SupportedStandardCodec_t  aStandardCodec[1];

  uint8_t                       NumSnkASEs;

  uint8_t                       NumSrcASEs;

  APP_ASEs_t                    aASEs[CFG_BLE_NUM_LINK];

  /* Codec Specific Capabilities for Sink Record */
#if (APP_NUM_SNK_PAC_RECORDS > 0)

  /* Handle of the Sink Audio Codec Record*/
  uint16_t                      aSnkPACRecordHandle[APP_NUM_SNK_PAC_RECORDS];

  /* Sink Published Audio Capabilities Records slots */
  BAP_AudioPACRecord_t          aSnkPACRecord[APP_NUM_SNK_PAC_RECORDS];

  /* Codec Specific Capabilities */
  uint8_t                       aSnkCap[APP_NUM_SNK_PAC_RECORDS][PACS_CODEC_SPECIFIC_CAPABILITIES_LENGTH];

  /* Metadata*/
  uint8_t                       aSnkMetadata[APP_NUM_SNK_PAC_RECORDS][PACS_SRV_METADATA_SIZE];

#endif /*(APP_NUM_SNK_PAC_RECORDS > 0)*/
  /*Codec Specific Capabilities for Source Record*/
#if (APP_NUM_SRC_PAC_RECORDS > 0)

  /*Handle of the Source PAC Record*/
  uint16_t                      aSrcPACRecordHandle[APP_NUM_SRC_PAC_RECORDS];

  /* Source Published Audio Capabilities Records slots*/
  BAP_AudioPACRecord_t          aSrcPACRecord[APP_NUM_SRC_PAC_RECORDS];

  /* Codec Specific Capabilities */
  uint8_t                       aSrcCap[APP_NUM_SRC_PAC_RECORDS][PACS_CODEC_SPECIFIC_CAPABILITIES_LENGTH];

  /* Metadata*/
  uint8_t                       aSrcMetadata[APP_NUM_SRC_PAC_RECORDS][PACS_SRV_METADATA_SIZE];

#endif /*(APP_NUM_SRC_PAC_RECORDS > 0)*/

  uint8_t                       NumOutputChannels;
} TMAPAPP_Context_t;

typedef struct
{
  Supported_Sampling_Freq_t     Freq;
  Supported_Frame_Duration_t    FrameDuration;
  uint16_t                      MinOctetsPerCodecFrame;
  uint16_t                      MaxOctetsPerCodecFrame;

} APP_CodecCap_t;

typedef struct
{
  Sampling_Freq_t       freq;
  uint32_t              sdu_interval;
  BAP_Framing_t         framing;
  uint16_t              max_sdu;
  uint8_t               rtx_num;
  uint16_t              max_tp_latency;
  uint32_t              presentation_delay;

} APP_QoSConf_t;

typedef struct
{
  uint8_t       NumSnkASEs;
  uint8_t       NumSrcASEs;
  uint8_t       NumAudioChnlsPerSnkASE;
  uint8_t       MinSnkAudioLocations;
  uint8_t       NumAudioChnlsPerSrcASE;
  uint8_t       MinSrcAudioLocations;
  uint8_t       NumCISes;
  uint16_t      NumAudioStreams;

} APP_UnicastAudioConf_t;


/* External variables --------------------------------------------------------*/

/* Exported macros ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ---------------------------------------------*/
void TMAPAPP_Init();
tBleStatus TMAPAPP_Linkup(uint16_t ConnHandle);
uint8_t TMAPAPP_StartAdvertising(CAP_Announcement_t AnnouncementType,uint8_t EnableSolicitationRequest);
uint8_t TMAPAPP_StopAdvertising(void);
uint8_t TMAPAPP_VolumeUp(void);
uint8_t TMAPAPP_VolumeDown(void);
uint8_t TMAPAPP_ToggleMute(void);
tBleStatus TMAPAPP_NextTrack(void);
tBleStatus TMAPAPP_PreviousTrack(void);
tBleStatus TMAPAPP_PlayPause(void);
tBleStatus TMAPAPP_AnswerCall(void);
tBleStatus TMAPAPP_TerminateCall(void);
uint8_t TMAPAPP_Disconnect(void);
void TMAPAPP_AclConnected(uint16_t Conn_Handle,uint8_t Peer_Address_Type,uint8_t Peer_Address[6],uint8_t role);
void TMAPAPP_CISConnected(uint16_t Conn_Handle);
void TMAPAPP_LinkDisconnected(uint16_t Conn_Handle);
void TMAPAPP_ConfirmIndicationRequired(uint16_t Conn_Handle);
void TMAPAPP_BondLost(uint16_t Conn_Handle);
#endif /*CAP_SUPPORT*/
#ifdef __cplusplus
}
#endif

