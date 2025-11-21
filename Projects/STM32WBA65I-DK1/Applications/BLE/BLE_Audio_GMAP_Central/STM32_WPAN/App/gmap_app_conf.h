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
#define APP_GMAP_ROLE                                   (GMAP_ROLE_UNICAST_GAME_GATEWAY \
                                                        | GMAP_ROLE_BROADCAST_GAME_SENDER)
#define APP_CAP_ROLE                                    (CAP_ROLE_INITIATOR | CAP_ROLE_COMMANDER)

#define APP_GMAP_UGG_FEATURE                            (UGG_FEATURES_MULTIPLEX_SUPPORT \
                                                         | UGG_FEATURES_96_KBPS_SOURCE_SUPPORT \
                                                         | UGG_FEATURES_MULTISINK_SUPPORT)

#define APP_GMAP_BGS_FEATURE                            (BGS_FEATURES_96_KBPS_SOURCE_SUPPORT)

#define APP_BAP_ROLE_UNICAST_SERVER_SUPPORT             (0u)
#define APP_BAP_ROLE_UNICAST_CLIENT_SUPPORT             (1u)
#define APP_BAP_ROLE_BROADCAST_SOURCE_SUPPORT           (1u)
#define APP_BAP_ROLE_BROADCAST_SINK_SUPPORT             (0u)
#define APP_BAP_ROLE_SCAN_DELEGATOR_SUPPORT             (0u)
#define APP_BAP_ROLE_BROADCAST_ASSISTANT_SUPPORT        (0u)

#define APP_CCP_ROLE_SERVER_SUPPORT                     (0u)
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

/*Preferred Controller Delay in us*/
#define PREFFERED_CONTROLLER_DELAY              (20000u)
#define BROADCAST_PREFFERED_CONTROLLER_DELAY    (20000u)

/**
 * Maximum number of Links supported in Unicast Server
 */
#define MAX_NUM_UCL_LINK                        CFG_BLE_NUM_LINK        /* Maximum number of Link supported by Unicast Client*/

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
 * Broadcast Source Settings
 */
#define APP_MAX_SUBGROUPS                       (1u)
#define APP_MAX_SUBGROUP_CODEC_CONFIG_LEN       (19u)
#define MAX_BIS_CODEC_CONFIG_LEN                (6u)
#define MAX_BSRC_METADATA_LEN                   (20u)

/**
 * GAP Configuration Settings
 */
#define APPBLE_GAP_DEVICE_NAME_LENGTH           (14u)
#define BD_ADDR_SIZE_LOCAL                      (6u)

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
 * Microphone Control Profile Settings
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

#if ((APP_CAP_ROLE & CAP_ROLE_COMMANDER) == CAP_ROLE_COMMANDER)
#define BLE_APP_CAP_COM_NUM_GATT_SERVICES      CAP_ACC_COM_NUM_GATT_SERVICES
#else
#define BLE_APP_CAP_COM_NUM_GATT_SERVICES      0u
#endif /*((APP_CAP_ROLE & CAP_ROLE_ACCEPTOR) == CAP_ROLE_ACCEPTOR)*/

#define BLE_APP_GMAP_NUM_GATT_SERVICES          GMAP_NUM_GATT_SERVICES

#define BLE_APP_NUM_GATT_SERVICES              (BLE_HOST_NUM_GATT_SERVICES \
                                               + BLE_APP_CAP_COM_NUM_GATT_SERVICES \
                                               + BLE_APP_GMAP_NUM_GATT_SERVICES)

/**
 * Maximum number of Attributes
 */
#if ((CFG_BLE_OPTIONS & BLE_OPTIONS_ENHANCED_ATT) == BLE_OPTIONS_ENHANCED_ATT)
#define BLE_HOST_NUM_GATT_ATTRIBUTES                    (15u)
#else
#define BLE_HOST_NUM_GATT_ATTRIBUTES                    (11u)
#endif /*((CFG_BLE_OPTIONS & BLE_OPTIONS_ENHANCED_ATT) == BLE_OPTIONS_ENHANCED_ATT)*/

#define BLE_APP_GMAP_NUM_GATT_ATTRIBUTES       GMAP_NUM_GATT_ATTRIBUTES

#define BLE_APP_NUM_GATT_ATTRIBUTES            (BLE_HOST_NUM_GATT_ATTRIBUTES \
                                               + BLE_APP_GMAP_NUM_GATT_ATTRIBUTES)

/**
 * Size of the storage area for Attribute values
 */
#if ((CFG_BLE_OPTIONS & BLE_OPTIONS_ENHANCED_ATT) == BLE_OPTIONS_ENHANCED_ATT)
#define BLE_HOST_ATT_VALUE_ARRAY_SIZE                   (64u)
#else
#define BLE_HOST_ATT_VALUE_ARRAY_SIZE                   (54u)
#endif /*((CFG_BLE_OPTIONS & BLE_OPTIONS_ENHANCED_ATT) == BLE_OPTIONS_ENHANCED_ATT)*/

#define BLE_APP_GMAP_ATT_VALUE_ARRAY_SIZE       GMAP_ATT_VALUE_ARRAY_SIZE

#define BLE_APP_ATT_VALUE_ARRAY_SIZE            (BLE_HOST_ATT_VALUE_ARRAY_SIZE + BLE_APP_GMAP_ATT_VALUE_ARRAY_SIZE)

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
                                                + BLE_MICP_CTLR_NVM_ALLOC_SIZE)

/* Exported types ------------------------------------------------------------*/

/* External variables --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /*__GMAP_APP_CONF_H */

