/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    pbp_app.h
  * @author  MCD Application Team
  * @brief   Header for pbp_app.c module
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
#ifndef __PBP_APP_H
#define __PBP_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wbaxx_hal.h"
#include "bap_bufsize.h"
#include "cap.h"
/* Private includes ----------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/*Audio Profiles Roles configuration*/
#define APP_BAP_ROLE_UNICAST_SERVER_SUPPORT                     (0u)
#define APP_BAP_ROLE_UNICAST_CLIENT_SUPPORT                     (0u)
#define APP_BAP_ROLE_BROADCAST_SOURCE_SUPPORT                   (0u)
#define APP_BAP_ROLE_BROADCAST_SINK_SUPPORT                     (1u)
#define APP_BAP_ROLE_SCAN_DELEGATOR_SUPPORT                     (1u)
#define APP_BAP_ROLE_BROADCAST_ASSISTANT_SUPPORT                (0u)


#define APP_CCP_ROLE_SERVER_SUPPORT                             (0u)
#define APP_CCP_ROLE_CLIENT_SUPPORT                             (0u)

#define APP_MCP_ROLE_SERVER_SUPPORT                             (0u)
#define APP_MCP_ROLE_CLIENT_SUPPORT                             (0u)

#define APP_VCP_ROLE_CONTROLLER_SUPPORT                         (0u)
#define APP_VCP_ROLE_RENDERER_SUPPORT                           (0u)

#define APP_MICP_ROLE_CONTROLLER_SUPPORT                        (0u)
#define APP_MICP_ROLE_DEVICE_SUPPORT                            (0u)

#define APP_CSIP_ROLE_SET_COORDINATOR_SUPPORT                   (0u)
#define APP_CSIP_ROLE_SET_MEMBER_SUPPORT                        (0u)


#define SETUP_SPEAKER                           0


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

#define NUM_LC3_QOS_CONFIG      32
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
                      BAP_BASS_SRV_ATT_VALUE_ARRAY_SIZE(MAX_NUM_SDE_BSRC_INFO,CFG_BLE_NUM_LINK,MAX_BASS_METADATA_SIZE)


#define BLE_APP_ATT_VALUE_ARRAY_SIZE    (47u + BLE_PACS_SRV_ATT_VALUE_ARRAY_SIZE \
                                        + BLE_BASS_SRV_ATT_VALUE_ARRAY_SIZE)

/* Exported types ------------------------------------------------------------*/
typedef uint8_t PBPAPP_PASyncState_t;
#define PBPAPP_PA_SYNC_STATE_IDLE           (0x00)
#define PBPAPP_PA_SYNC_STATE_SYNCHRONIZING  (0x01)
#define PBPAPP_PA_SYNC_STATE_SYNCHRONIZED   (0x02)

typedef uint8_t PBPAPP_BIGSyncState_t;
#define PBPAPP_BIG_SYNC_STATE_IDLE          (0x00)
#define PBPAPP_BIG_SYNC_STATE_SYNCHRONIZING (0x01)
#define PBPAPP_BIG_SYNC_STATE_SYNCHRONIZED  (0x02)

typedef uint8_t PBPAPP_ScanState_t;
#define PBPAPP_SCAN_STATE_IDLE              (0x00)
#define PBPAPP_SCAN_STATE_SCANNING          (0x01)

typedef struct
{
  BAP_Role_t                    bap_role;
  BAP_BASE_Group_t              base_group;
  BAP_BASE_Subgroup_t           base_subgroups[2];
  BAP_BASE_BIS_t                base_bis[2];
  uint8_t                       codec_specific_config_bis[2][0x06];
  uint8_t                       codec_specific_config_subgroup[2][0x13];
  uint8_t                       RTN;
  uint16_t                      max_transport_latency;
  uint8_t                       subgroup_metadata[2][MAX_METADATA_LEN];
  Target_Phy_t                  phy;
  uint8_t                       encryption;
  uint32_t                      broadcast_code[4u];
  uint8_t                       audio_driver_config;
  uint16_t                      current_BIS_conn_handles[MAX_NUM_BIS_PER_BIG];
  uint8_t                       current_num_bis;
  BAP_AudioCodecController_t    AudioCodecInController;
  BAP_SupportedStandardCodec_t  aStandardCodec[1];
  PBPAPP_PASyncState_t          PASyncState;
  PBPAPP_BIGSyncState_t         BIGSyncState;
  PBPAPP_ScanState_t            ScanState;
  uint8_t                       PASyncHandle;
} PBPAPP_Context_t;

/* External variables --------------------------------------------------------*/

/* Exported macros ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ---------------------------------------------*/
uint8_t PBPAPP_InitSink(void);
uint8_t PBPAPP_StartSink(void);
uint8_t PBPAPP_StopSink(void);
void PBPAPP_SwitchBrdSource(uint8_t next, uint32_t *pSourceID);
uint32_t PBPAPP_GetBrdSource(void);

#ifdef __cplusplus
}
#endif

#endif /*__PBP_APP_H */

