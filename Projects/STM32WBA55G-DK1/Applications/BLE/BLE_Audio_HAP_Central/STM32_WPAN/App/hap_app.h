/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    hap_app.h
  * @author  MCD Application Team
  * @brief   Header for hap_app.c module
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
#ifndef __HAP_APP_H
#define __HAP_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wbaxx_hal.h"
#include "hap.h"
#include "hap_app_conf.h"
#include "app_conf.h"
#include "stm32_timer.h"
/* Private includes ----------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

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
#define AUDIO_PROFILE_HAP                       0x80u

/* Init state */
typedef uint8_t HAPAPP_InitState_t;
#define HAP_APP_INIT_STATE_IDLE                 0x00
#define HAP_APP_INIT_STATE_CAP_LINKUP           0x01
#define HAP_APP_INIT_STATE_HAP_LINKUP           0x02
#define HAP_APP_INIT_STATE_READ_ACTIVE_PRESET   0x03
#define HAP_APP_INIT_STATE_READ_PRESETS         0x04
#define HAP_APP_INIT_STATE_SET_MEMBER_DISC      0x05
#define HAP_APP_INIT_STATE_SET_VOLUME           0x06
#define HAP_APP_INIT_STATE_SET_MIC_MUTE         0x07

/*Structure used for ASE information storage*/
typedef struct
{
  uint8_t               ID;
  ASE_State_t           state;
  ASE_Type_t            type;           /*Source or Sink*/
  uint8_t               allocated;
  uint8_t               num_channels;
  uint32_t              presentation_delay;
  uint32_t              controller_delay;
}APP_ASE_Info_t;

typedef struct
{
  uint16_t              acl_conn_handle;
#if (MAX_NUM_UCL_SNK_ASE_PER_LINK > 0)
  APP_ASE_Info_t        aSnkASE[MAX_NUM_UCL_SNK_ASE_PER_LINK];
#endif /* (MAX_NUM_UCL_SNK_ASE_PER_LINK > 0) */
#if (MAX_NUM_UCL_SRC_ASE_PER_LINK > 0)
  APP_ASE_Info_t        aSrcASE[MAX_NUM_UCL_SRC_ASE_PER_LINK];
#endif /* (MAX_NUM_UCL_SRC_ASE_PER_LINK > 0) */
}APP_ASEs_t;

#if (APP_CCP_ROLE_SERVER_SUPPORT == 1u)
typedef struct
{
  uint8_t               CCID;
}HAPAPP_Bearer_t;
#endif /* (APP_CCP_ROLE_SERVER_SUPPORT == 1u) */

/*Structure used for connection information strorage*/
typedef struct
{
  uint16_t              Acl_Conn_Handle;
  uint8_t               Peer_Address_Type;
  uint8_t               Peer_Address[6];
  uint8_t               Role;
  audio_profile_t       AudioProfile;
  uint8_t               CSIPDiscovered; /*Set to 1 if Set Member has been discovered (but not yet CAP linked)*/
  uint8_t               SIRK_type;
  uint8_t               SIRK[16];
  uint8_t               Rank;
  uint8_t               Size;
  uint8_t               ForceCompleteLinkup;
  uint8_t               ConfirmIndicationRequired;

  BAP_Unicast_Server_Info_t UnicastServerInfo;
  APP_ASEs_t            *pASEs;

}APP_ACL_Conn_t;

typedef struct
{
  BAP_Role_t                    bap_role;

  APP_ACL_Conn_t                ACL_Conn[CFG_BLE_NUM_LINK];

  uint8_t                       audio_driver_config;

  Audio_Role_t                  audio_role_setup;

  uint16_t                      cis_src_handle[APP_MAX_NUM_CIS];

  uint8_t                       num_cis_src;

  uint16_t                      cis_snk_handle[APP_MAX_NUM_CIS];

  uint8_t                       num_cis_snk;

  uint16_t                      cis_handle[APP_MAX_NUM_CIS];

  uint8_t                       num_cis_established;

  BAP_AudioCodecController_t    AudioCodecInController;

  BAP_SupportedStandardCodec_t  aStandardCodec[1];

  APP_ASEs_t                    aASEs[CFG_BLE_NUM_LINK];

#if (APP_CCP_ROLE_SERVER_SUPPORT == 1u)
  HAPAPP_Bearer_t              GenericBearer;
#if (APP_CCP_NUM_LOCAL_BEARER_INSTANCES > 0u)
  HAPAPP_Bearer_t              BearerInstance[APP_CCP_NUM_LOCAL_BEARER_INSTANCES];
#endif /*(APP_CCP_NUM_LOCAL_BEARER_INSTANCES > 0u)*/

  uint8_t                       CurrentCallID;
#endif /* (APP_CCP_ROLE_SERVER_SUPPORT == 1u) */

  uint8_t                       NumOutputChannels;

  uint8_t                       SetMemberDiscoveryProcActive;

  HAPAPP_InitState_t            InitState;

  uint16_t                      OperationConnHandle;
} HAPAPP_Context_t;

typedef struct
{
  Sampling_Freq_t       freq;
  Frame_Duration_t      frame_duration;
  uint16_t              octets_per_codec_frame;
} APP_CodecConf_t;

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

typedef struct
{
  uint8_t Index;
  uint8_t Properties;
  char Name[HAP_MAX_PRESET_NAME_LEN+1];
} HAPAPP_Preset_t;

/* External variables --------------------------------------------------------*/

/* Exported macros ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ---------------------------------------------*/
void HAPAPP_Init();
tBleStatus HAPAPP_Linkup(uint16_t ConnHandle);
uint8_t HAPAPP_StartScanning(void);
uint8_t HAPAPP_StopScanning(void);
uint8_t HAPAPP_CreateConnection(uint8_t *pAddress, uint8_t AddressType);
uint8_t HAPAPP_Disconnect(void);
uint8_t HAPAPP_RemoteVolumeUp(void);
uint8_t HAPAPP_RemoteVolumeDown(void);
uint8_t HAPAPP_RemoteToggleMute(void);
void HAPAPP_LocalVolumeUp(void);
void HAPAPP_LocalVolumeDown(void);
void HAPAPP_LocalToggleMute(void);
uint8_t HAPAPP_RemoteToggleMicMute(void);
tBleStatus HAPAPP_NextPreset(void);
tBleStatus HAPAPP_PreviousPreset(void);
tBleStatus HAPAPP_AnswerCall(void);
tBleStatus HAPAPP_TerminateCall(void);
tBleStatus HAPAPP_IncomingCall(void);
uint8_t HAPAPP_StartMediaStream(void);
uint8_t HAPAPP_StartTelephonyStream(void);
uint8_t HAPAPP_StopStream(void);
void HAPAPP_AclConnected(uint16_t Conn_Handle,uint8_t Peer_Address_Type,uint8_t Peer_Address[6],uint8_t role);
void HAPAPP_CISConnected(uint16_t Conn_Handle);
void HAPAPP_LinkDisconnected(uint16_t Conn_Handle);
void HAPAPP_ConfirmIndicationRequired(uint16_t Conn_Handle);
void HAPAPP_BondLost(uint16_t Conn_Handle);
#ifdef __cplusplus
}
#endif
#endif /* __HAP_APP_H */

