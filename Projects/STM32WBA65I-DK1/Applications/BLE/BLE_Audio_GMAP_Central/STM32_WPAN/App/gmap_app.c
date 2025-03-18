/**
  ******************************************************************************
  * @file    gmap_app.c
  * @author  MCD Application Team
  * @brief   GMAP Application
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

/* Includes ------------------------------------------------------------------*/

#include "gmap_app.h"
#include "main.h"
#include "ble.h"
#include "ble_audio_stack.h"
#include "stm32_seq.h"
#include "codec_mngr.h"
#include "ltv_utils.h"
#include "cap.h"
#include "gmap.h"
#include "usecase_dev_mgmt.h"
#include "app_menu_cfg.h"
#include "log_module.h"
#include "app_ble.h"
/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/

/* Audio chain memory sizing: must be aligned with PAC (frame len) and ASEs (channels nb)
 * Theses macro are generic and could be overwriten by the user for a fine tuning
 */

/* Memory pool used by the codec manager for managing audio latencies
 * (8 x LC3 encoded frames (Freq, bitrate, 10ms)) per audio channel
 */
#define CODEC_POOL_SUB_SIZE                     CODEC_MAX_BAND <= CODEC_SSWB ? (480u) : (960u)

/* Audio in and out buffers used by the BSP
 * (max LC3 frame len (Freq, 10ms)) x (Max Channels Number (mono vs stereo)) x 2 (double buffer configuration)
 */
#define SAI_SRC_MAX_BUFF_SIZE                   (CODEC_MAX_BAND <= CODEC_SSWB ? 240 : 480)*CODEC_LC3_NUM_ENCODER_CHANNEL*2
#define SAI_SNK_MAX_BUFF_SIZE                   (CODEC_MAX_BAND <= CODEC_SSWB ? 240 : 480)*CODEC_LC3_NUM_DECODER_CHANNEL*2

/* Buffers used by the LC3 codec */
#define CODEC_LC3_SESSION_DYN_ALLOC_SIZE \
        CODEC_GET_TOTAL_SESSION_BUFFER_SIZE(CODEC_LC3_NUM_SESSION, CODEC_MAX_BAND)

#define CODEC_LC3_CHANNEL_DYN_ALLOC_SIZE \
        CODEC_GET_TOTAL_ENCODER_CH_BUFFER_SIZE(CODEC_LC3_NUM_ENCODER_CHANNEL, CODEC_MAX_BAND) + \
        CODEC_GET_TOTAL_DECODER_CH_BUFFER_SIZE(CODEC_LC3_NUM_DECODER_CHANNEL, CODEC_MAX_BAND)

#define CODEC_LC3_STACK_DYN_ALLOC_SIZE \
        MAX(CODEC_LC3_NUM_ENCODER_CHANNEL > 0 ? CODEC_GET_ENCODER_STACK_SIZE(CODEC_MAX_BAND) : 0, \
            CODEC_LC3_NUM_DECODER_CHANNEL > 0 ? CODEC_GET_DECODER_STACK_SIZE(CODEC_MAX_BAND) : 0)


#define BLE_AUDIO_DYN_ALLOC_SIZE        (BLE_AUDIO_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK))

/*Memory size required for CAP*/
#define CAP_DYN_ALLOC_SIZE \
        CAP_MEM_TOTAL_BUFFER_SIZE((CAP_ROLE_INITIATOR|CAP_ROLE_COMMANDER),CFG_BLE_NUM_LINK, \
                                    MAX_NUM_CIG,MAX_NUM_CIS_PER_CIG, \
                                    MAX_NUM_UCL_SNK_ASE_PER_LINK, \
                                    MAX_NUM_UCL_SRC_ASE_PER_LINK, \
                                    0,0,\
                                    0,0, \
                                    0,0, \
                                    APP_VCP_ROLE_CONTROLLER_SUPPORT,APP_VCP_CTLR_NUM_AIC_INSTANCES,APP_VCP_CTLR_NUM_VOC_INSTANCES, \
                                    APP_MICP_ROLE_CONTROLLER_SUPPORT,APP_MICP_CTLR_NUM_AIC_INSTANCES)

/*Memory size required to allocate resource for Audio Stream Endpoint in Unicats Server Context*/
#define BAP_UCL_ASE_BLOCKS_SIZE \
          BAP_MEM_BLOCKS_ASE_PER_CONN_SIZE_BYTES(MAX_NUM_UCL_SNK_ASE_PER_LINK,MAX_NUM_UCL_SRC_ASE_PER_LINK,MAX_UCL_CODEC_CONFIG_SIZE,MAX_UCL_METADATA_SIZE)

/* Memory size required to allocate resource for Published Audio Capabilities for Unicast Client and/or Broadcast Assistant*/
#define BAP_PACS_CLT_DYN_ALLOC_SIZE  \
              BAP_PACS_CLT_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK,MAX_NUM_UCL_LINK,0,(MAX_NUM_CLT_SNK_PAC_RECORDS_PER_LINK + MAX_NUM_CLT_SRC_PAC_RECORDS_PER_LINK))

/*Memory size required for Audio Stream management in Unicast Client Context*/
#define BAP_ASCS_CLT_DYN_ALLOC_SIZE \
            BAP_ASCS_CLT_TOTAL_BUFFER_SIZE(MAX_NUM_UCL_LINK,MAX_NUM_UCL_SNK_ASE_PER_LINK,MAX_NUM_UCL_SRC_ASE_PER_LINK,BAP_UCL_ASE_BLOCKS_SIZE)

/* Memory size required to allocate resource for CIG*/
#define BAP_ISO_CHNL_DYN_ALLOC_SIZE  \
        BAP_MEM_BLOCKS_CIG_SIZE_BYTES(MAX_NUM_CIG, MAX_NUM_CIS_PER_CIG)

/* Memory size required to allocate resource for Non-Volatile Memory Management for BAP Services restoration*/
#define BAP_NVM_MGMT_DYN_ALLOC_SIZE  \
        BAP_NVM_MGMT_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK)

#if (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u)
#define BLE_VCP_CTLR_DYN_ALLOC_SIZE     BLE_VCP_CTLR_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK, \
                                                                        APP_VCP_CTLR_NUM_AIC_INSTANCES, \
                                                                        APP_VCP_CTLR_NUM_VOC_INSTANCES)
#endif /* (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u) */

#if (APP_MICP_ROLE_CONTROLLER_SUPPORT == 1u)
#define BLE_MICP_CTLR_DYN_ALLOC_SIZE     BLE_MICP_CTLR_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK, \
                                                                        APP_MICP_CTLR_NUM_AIC_INSTANCES)
#endif /* (APP_MICP_ROLE_CONTROLLER_SUPPORT == 1u) */

#define BLE_CSIP_SET_COORDINATOR_DYN_ALLOC_SIZE         BLE_CSIP_SET_COORDINATOR_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK)

#define VOLUME_STEP 10
#define BASE_VOLUME 128

#define TARGET_MEDIA_STREAM_QOS_CONFIG LC3_QOS_48_4_1

#define TARGET_TELEPHONY_STREAM_QOS_CONFIG_SINK LC3_QOS_48_2_1
#define TARGET_TELEPHONY_STREAM_QOS_CONFIG_SOURCE LC3_QOS_32_2_1

/* Broadcast Config */
#define BROADCAST_SOURCE_FRAME_BLOCK_PER_SDU    (1u)
#define BROADCAST_SOURCE_NUM_BIS                (2u)
#define BROADCAST_SOURCE_CHANNEL_ALLOC_1        (FRONT_LEFT)
#define BROADCAST_SOURCE_CHANNEL_ALLOC_2        (FRONT_RIGHT)
#define BAP_BROADCAST_MAX_TRANSPORT_LATENCY     (20u)
#define BAP_BROADCAST_ENCRYPTION                (0u)
#define BIG_HANDLE                              (0u)

/* 0x0002 is conversational
 * 0x0004 is Media
 */
#define BROADCAST_STREAMING_AUDIO_CONTEXT                 (AUDIO_CONTEXT_GAME)

/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* Codec Configurations defined in Basic Audio Profile Specification*/
const APP_CodecConf_t APP_CodecConf[NUM_LC3_CODEC_CONFIG] =    \
                  {{SAMPLE_FREQ_8000_HZ,FRAME_DURATION_7_5_MS,26u}, \
                  {SAMPLE_FREQ_8000_HZ,FRAME_DURATION_10_MS,30u}, \
                  {SAMPLE_FREQ_16000_HZ,FRAME_DURATION_7_5_MS,30u}, \
                  {SAMPLE_FREQ_16000_HZ,FRAME_DURATION_10_MS,40u}, \
                  {SAMPLE_FREQ_24000_HZ,FRAME_DURATION_7_5_MS,45u}, \
                  {SAMPLE_FREQ_24000_HZ,FRAME_DURATION_10_MS,60u}, \
                  {SAMPLE_FREQ_32000_HZ,FRAME_DURATION_7_5_MS,60u}, \
                  {SAMPLE_FREQ_32000_HZ,FRAME_DURATION_10_MS,80u}, \
                  {SAMPLE_FREQ_44100_HZ,FRAME_DURATION_7_5_MS,97u}, \
                  {SAMPLE_FREQ_44100_HZ,FRAME_DURATION_10_MS,130u}, \
                  {SAMPLE_FREQ_48000_HZ,FRAME_DURATION_7_5_MS,75u}, \
                  {SAMPLE_FREQ_48000_HZ,FRAME_DURATION_10_MS,100u}, \
                  {SAMPLE_FREQ_48000_HZ,FRAME_DURATION_7_5_MS,90u}, \
                  {SAMPLE_FREQ_48000_HZ,FRAME_DURATION_10_MS,120u}, \
                  {SAMPLE_FREQ_48000_HZ,FRAME_DURATION_7_5_MS,117u}, \
                  {SAMPLE_FREQ_48000_HZ,FRAME_DURATION_10_MS,155u}};

/* QoS Configuration settings defined in Basic Audio Profile Specification for low latency and for high-reliability audio data*/
const APP_QoSConf_t APP_QoSConf[NUM_LC3_QoSConf + NUM_GMAP_LC3_QoSConf] =    \
                  {{SAMPLE_FREQ_8000_HZ,7500,BAP_FRAMING_UNFRAMED,26,2,8,40000}, \
                  {SAMPLE_FREQ_8000_HZ,10000,BAP_FRAMING_UNFRAMED,30,2,10,40000}, \
                  {SAMPLE_FREQ_16000_HZ,7500,BAP_FRAMING_UNFRAMED,30,2,8,40000}, \
                  {SAMPLE_FREQ_16000_HZ,10000,BAP_FRAMING_UNFRAMED,40,2,10,40000}, \
                  {SAMPLE_FREQ_24000_HZ,7500,BAP_FRAMING_UNFRAMED,45,2,8,40000}, \
                  {SAMPLE_FREQ_24000_HZ,10000,BAP_FRAMING_UNFRAMED,60,2,10,10000}, \
                  {SAMPLE_FREQ_32000_HZ,7500,BAP_FRAMING_UNFRAMED,60,2,8,40000}, \
                  {SAMPLE_FREQ_32000_HZ,10000,BAP_FRAMING_UNFRAMED,80,2,10,40000}, \
                  {SAMPLE_FREQ_44100_HZ,8163,BAP_FRAMING_FRAMED,97,5,24,40000}, \
                  {SAMPLE_FREQ_44100_HZ,10884,BAP_FRAMING_FRAMED,130,5,31,40000}, \
                  {SAMPLE_FREQ_48000_HZ,7500,BAP_FRAMING_UNFRAMED,75,5,15,40000}, \
                  {SAMPLE_FREQ_48000_HZ,10000,BAP_FRAMING_UNFRAMED,100,5,20,40000}, \
                  {SAMPLE_FREQ_48000_HZ,7500,BAP_FRAMING_UNFRAMED,90,5,15,40000}, \
                  {SAMPLE_FREQ_48000_HZ,10000,BAP_FRAMING_UNFRAMED,120,5,20,40000}, \
                  {SAMPLE_FREQ_48000_HZ,7500,BAP_FRAMING_UNFRAMED,117,5,15,40000}, \
                  {SAMPLE_FREQ_48000_HZ,10000,BAP_FRAMING_UNFRAMED,155,5,20,40000}, \

                  {SAMPLE_FREQ_8000_HZ,7500,BAP_FRAMING_UNFRAMED,26,13,75,40000}, \
                  {SAMPLE_FREQ_8000_HZ,10000,BAP_FRAMING_UNFRAMED,30,13,95,40000}, \
                  {SAMPLE_FREQ_16000_HZ,7500,BAP_FRAMING_UNFRAMED,30,13,75,40000}, \
                  {SAMPLE_FREQ_16000_HZ,10000,BAP_FRAMING_UNFRAMED,40,13,95,40000}, \
                  {SAMPLE_FREQ_24000_HZ,7500,BAP_FRAMING_UNFRAMED,45,13,75,40000}, \
                  {SAMPLE_FREQ_24000_HZ,10000,BAP_FRAMING_UNFRAMED,60,13,95,40000}, \
                  {SAMPLE_FREQ_32000_HZ,7500,BAP_FRAMING_UNFRAMED,60,13,75,40000}, \
                  {SAMPLE_FREQ_32000_HZ,10000,BAP_FRAMING_UNFRAMED,80,13,95,40000}, \
                  {SAMPLE_FREQ_44100_HZ,8163,BAP_FRAMING_FRAMED,97,13,80,40000}, \
                  {SAMPLE_FREQ_44100_HZ,10884,BAP_FRAMING_FRAMED,130,13,85,40000}, \
                  {SAMPLE_FREQ_48000_HZ,7500,BAP_FRAMING_UNFRAMED,75,13,75,40000}, \
                  {SAMPLE_FREQ_48000_HZ,10000,BAP_FRAMING_UNFRAMED,100,13,95,40000}, \
                  {SAMPLE_FREQ_48000_HZ,7500,BAP_FRAMING_UNFRAMED,90,13,75,40000}, \
                  {SAMPLE_FREQ_48000_HZ,10000,BAP_FRAMING_UNFRAMED,120,13,100,40000}, \
                  {SAMPLE_FREQ_48000_HZ,7500,BAP_FRAMING_UNFRAMED,117,13,75,40000}, \
                  {SAMPLE_FREQ_48000_HZ,10000,BAP_FRAMING_UNFRAMED,155,13,100,40000}, \


                  {SAMPLE_FREQ_16000_HZ,7500,BAP_FRAMING_UNFRAMED,30,1,15,60000}, \
                  {SAMPLE_FREQ_16000_HZ,10000,BAP_FRAMING_UNFRAMED,40,1,20,60000}, \
                  {SAMPLE_FREQ_32000_HZ,7500,BAP_FRAMING_UNFRAMED,60,1,15,60000}, \
                  {SAMPLE_FREQ_32000_HZ,10000,BAP_FRAMING_UNFRAMED,80,1,20,60000}, \
                  {SAMPLE_FREQ_48000_HZ,7500,BAP_FRAMING_UNFRAMED,75,1,15,60000}, \
                  {SAMPLE_FREQ_48000_HZ,10000,BAP_FRAMING_UNFRAMED,100,1,20,60000}, \
                  {SAMPLE_FREQ_32000_HZ,7500,BAP_FRAMING_UNFRAMED,60,1,15,10000}, \
                  {SAMPLE_FREQ_32000_HZ,10000,BAP_FRAMING_UNFRAMED,80,1,20,10000}, \
                  {SAMPLE_FREQ_48000_HZ,7500,BAP_FRAMING_UNFRAMED,75,1,15,10000}, \
                  {SAMPLE_FREQ_48000_HZ,10000,BAP_FRAMING_UNFRAMED,100,1,20,10000}, \
                  {SAMPLE_FREQ_48000_HZ,7500,BAP_FRAMING_UNFRAMED,90,1,15,10000}, \
                  {SAMPLE_FREQ_48000_HZ,10000,BAP_FRAMING_UNFRAMED,120,1,20,10000}};

const APP_QoSConf_t APP_BroadcastQoSConf[NUM_LC3_QoSConf + NUM_GMAP_Brd_LC3_QoSConf] =    \
                  { /* Low Latency BAP Configs */
                  {SAMPLE_FREQ_8000_HZ,7500,BAP_FRAMING_UNFRAMED,26,2,8,40000}, \
                  {SAMPLE_FREQ_8000_HZ,10000,BAP_FRAMING_UNFRAMED,30,2,10,40000}, \
                  {SAMPLE_FREQ_16000_HZ,7500,BAP_FRAMING_UNFRAMED,30,2,8,40000}, \
                  {SAMPLE_FREQ_16000_HZ,10000,BAP_FRAMING_UNFRAMED,40,2,10,40000}, \
                  {SAMPLE_FREQ_24000_HZ,7500,BAP_FRAMING_UNFRAMED,45,2,8,40000}, \
                  {SAMPLE_FREQ_24000_HZ,10000,BAP_FRAMING_UNFRAMED,60,2,10,40000}, \
                  {SAMPLE_FREQ_32000_HZ,7500,BAP_FRAMING_UNFRAMED,60,2,8,40000}, \
                  {SAMPLE_FREQ_32000_HZ,10000,BAP_FRAMING_UNFRAMED,80,2,10,40000}, \
                  {SAMPLE_FREQ_44100_HZ,8163,BAP_FRAMING_FRAMED,97,4,24,40000}, \
                  {SAMPLE_FREQ_44100_HZ,10884,BAP_FRAMING_FRAMED,130,4,31,40000}, \
                  {SAMPLE_FREQ_48000_HZ,7500,BAP_FRAMING_UNFRAMED,75,4,15,40000}, \
                  {SAMPLE_FREQ_48000_HZ,10000,BAP_FRAMING_UNFRAMED,100,4,20,40000}, \
                  {SAMPLE_FREQ_48000_HZ,7500,BAP_FRAMING_UNFRAMED,90,4,15,40000}, \
                  {SAMPLE_FREQ_48000_HZ,10000,BAP_FRAMING_UNFRAMED,120,4,20,40000}, \
                  {SAMPLE_FREQ_48000_HZ,7500,BAP_FRAMING_UNFRAMED,117,4,15,40000}, \
                  {SAMPLE_FREQ_48000_HZ,10000,BAP_FRAMING_UNFRAMED,155,4,20,40000}, \

                    /* High Reliability BAP Configs */
                  {SAMPLE_FREQ_8000_HZ,7500,BAP_FRAMING_UNFRAMED,26,4,45,40000}, \
                  {SAMPLE_FREQ_8000_HZ,10000,BAP_FRAMING_UNFRAMED,30,4,60,40000}, \
                  {SAMPLE_FREQ_16000_HZ,7500,BAP_FRAMING_UNFRAMED,30,4,45,40000}, \
                  {SAMPLE_FREQ_16000_HZ,10000,BAP_FRAMING_UNFRAMED,40,4,60,40000}, \
                  {SAMPLE_FREQ_24000_HZ,7500,BAP_FRAMING_UNFRAMED,45,4,45,40000}, \
                  {SAMPLE_FREQ_24000_HZ,10000,BAP_FRAMING_UNFRAMED,60,4,60,40000}, \
                  {SAMPLE_FREQ_32000_HZ,7500,BAP_FRAMING_UNFRAMED,60,4,45,40000}, \
                  {SAMPLE_FREQ_32000_HZ,10000,BAP_FRAMING_UNFRAMED,80,4,60,40000}, \
                  {SAMPLE_FREQ_44100_HZ,8163,BAP_FRAMING_FRAMED,97,4,54,40000}, \
                  {SAMPLE_FREQ_44100_HZ,10884,BAP_FRAMING_FRAMED,130,4,60,40000}, \
                  {SAMPLE_FREQ_48000_HZ,7500,BAP_FRAMING_UNFRAMED,75,4,50,40000}, \
                  {SAMPLE_FREQ_48000_HZ,10000,BAP_FRAMING_UNFRAMED,100,4,65,40000}, \
                  {SAMPLE_FREQ_48000_HZ,7500,BAP_FRAMING_UNFRAMED,90,4,50,40000}, \
                  {SAMPLE_FREQ_48000_HZ,10000,BAP_FRAMING_UNFRAMED,120,4,65,40000}, \
                  {SAMPLE_FREQ_48000_HZ,7500,BAP_FRAMING_UNFRAMED,117,4,50,40000}, \
                  {SAMPLE_FREQ_48000_HZ,10000,BAP_FRAMING_UNFRAMED,155,4,65,40000}, \

                    /* GMAP Configs */
                  {SAMPLE_FREQ_48000_HZ,7500,BAP_FRAMING_UNFRAMED,75,1,8,10000}, \
                  {SAMPLE_FREQ_48000_HZ,10000,BAP_FRAMING_UNFRAMED,100,1,10,10000}, \
                  {SAMPLE_FREQ_48000_HZ,7500,BAP_FRAMING_UNFRAMED,90,1,8,10000}, \
                  {SAMPLE_FREQ_48000_HZ,10000,BAP_FRAMING_UNFRAMED,120,1,10,10000}};

/* Application context*/
GMAPAPP_Context_t GMAPAPP_Context = {0};

/*Audio profiles configuration*/
CAP_Config_t APP_CAP_Config = {0};
BAP_Config_t APP_BAP_Config = {0};
CCP_Config_t APP_CCP_Config = {0};
MCP_Config_t APP_MCP_Config = {0};
VCP_Config_t APP_VCP_Config = {0};
MICP_Config_t APP_MICP_Config = {0};
CSIP_Config_t APP_CSIP_Config = {0};

/*Buffers allocation used in Generic Audio Framework library for internal use*/
static uint32_t aCAPMemBuffer[DIVC(CAP_DYN_ALLOC_SIZE,4)];
static uint32_t aPACSCltMemBuffer[DIVC(BAP_PACS_CLT_DYN_ALLOC_SIZE,4)];
static uint32_t aASCSCltMemBuffer[DIVC(BAP_ASCS_CLT_DYN_ALLOC_SIZE,4)];
static uint32_t aISOChnlMemBuffer[DIVC(BAP_ISO_CHNL_DYN_ALLOC_SIZE,4)];
static uint32_t aNvmMgmtMemBuffer[DIVC(BAP_NVM_MGMT_DYN_ALLOC_SIZE,4)];
static uint32_t audio_init_buffer[BLE_AUDIO_DYN_ALLOC_SIZE];
static BleAudioInit_t pBleAudioInit;
#if (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u)
static uint32_t aCltrMemBuffer[DIVC(BLE_VCP_CTLR_DYN_ALLOC_SIZE,4)];
#endif /*(APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u)*/
#if (APP_MICP_ROLE_CONTROLLER_SUPPORT == 1)
static uint32_t aMicCltrMemBuffer[DIVC(BLE_MICP_CTLR_DYN_ALLOC_SIZE,4)];
#endif /*(APP_MICP_ROLE_CONTROLLER_SUPPORT == 1u)*/
static uint32_t aSetCoordinatorMemBuffer[DIVC(BLE_CSIP_SET_COORDINATOR_DYN_ALLOC_SIZE,4)];

/* Buffers allocation for LC3*/
static uint32_t aLC3SessionMemBuffer[DIVC(CODEC_LC3_SESSION_DYN_ALLOC_SIZE,4)];
static uint32_t aLC3ChannelMemBuffer[DIVC(CODEC_LC3_CHANNEL_DYN_ALLOC_SIZE,4)];
static uint32_t aLC3StackMemBuffer[DIVC(CODEC_LC3_STACK_DYN_ALLOC_SIZE,4)];

static uint8_t aCodecPacketsMemory[MAX_CHANNEL][CODEC_POOL_SUB_SIZE];

/* In and Out audio buffers sized for an LC3 frame (double buffer, 16 bits per sample) */
#if (SAI_SRC_MAX_BUFF_SIZE != 0)
static uint16_t aSrcBuff[SAI_SRC_MAX_BUFF_SIZE] __attribute__((aligned));
#else
static uint16_t *aSrcBuff = NULL;
#endif /*(SAI_SRC_MAX_BUFF_SIZE != 0)*/
#if (SAI_SNK_MAX_BUFF_SIZE != 0)
static uint16_t aSnkBuff[SAI_SNK_MAX_BUFF_SIZE] __attribute__((aligned));
#else
static uint16_t *aSnkBuff = NULL;
#endif /*(SAI_SNK_MAX_BUFF_SIZE != 0)*/

static int16_t* pPrevData = NULL;
static uint8_t Nb_Active_Ch = 0;

#define MAX_UNICAST_ACCEPTORS      2u
/* Structures used as Unicast Audio Start parameters */
CAP_Unicast_AudioStart_Stream_Params_t StartStreamParams[MAX_UNICAST_ACCEPTORS];
CAP_Unicast_AudioStart_Codec_Conf_t CodecConfSnk[MAX_UNICAST_ACCEPTORS];
CAP_Unicast_AudioStart_Codec_Conf_t CodecConfSrc[MAX_UNICAST_ACCEPTORS];
CAP_Unicast_AudioStart_QoS_Conf_t QosConfSnk[MAX_UNICAST_ACCEPTORS];
CAP_Unicast_AudioStart_QoS_Conf_t QosConfSrc[MAX_UNICAST_ACCEPTORS];
uint8_t aMetadataSnk[MAX_UNICAST_ACCEPTORS][20];
uint8_t aMetadataSrc[MAX_UNICAST_ACCEPTORS][20];

uint32_t aAPP_BroadcastCode[4u] = {0x00000001, 0x00000002, 0x00000003, 0x00000004};

/* Advertising Parameters */
BAP_Extended_Advertising_Params_t extended_adv_params = {
  0x00,          /* Advertising SID */
  100,           /* Advertising Interval min (*0.625) */
  100,           /* Adv Interval max (*0.625) */
  0x07,          /* Channel Map */
  0x00,          /* Own Address type */
  0x00,          /* Peer Address Type */
  {0,0,0,0,0,0}, /* Peer Address */
  0x7F,          /* TX Power */
  0x00,          /* Secondary Advertising Max Skip */
  0x01           /* Secondary Adv PHY */
};
BAP_Periodic_Advertising_Params_t periodic_adv_params = {
  64,  /* Advertising Interval Min */
  64,  /* Advertising Interval Max */
  0x00 /* Advertising Properties */
};

typedef struct
{
  Sampling_Freq_t Frequency;
  uint8_t         Multiplier1;
  uint8_t         Multiplier2;
} CombinedFreqParameters;

const CombinedFreqParameters APP_CombinedFrequencyTable[9][9] =
             /*                         8KHz                         16KHz                                    24 KHz                          32 KHz                                   48KHz       */
             {{{0, 0, 0}, {0, 0, 0},                    {0, 0, 0}, {0, 0, 0},                    {0, 0, 0}, {0, 0, 0},                    {0, 0, 0},                    {0, 0, 0}, {0, 0, 0}}, \
/* 8KHz */    {{0, 0, 0}, {SAMPLE_FREQ_8000_HZ, 1, 1},  {0, 0, 0}, {SAMPLE_FREQ_16000_HZ, 2, 1}, {0, 0, 0}, {SAMPLE_FREQ_24000_HZ, 3, 1}, {SAMPLE_FREQ_32000_HZ, 4, 1}, {0, 0, 0}, {SAMPLE_FREQ_48000_HZ, 6, 1}}, \
              {{0, 0, 0}, {0, 0, 0},                    {0, 0, 0}, {0, 0, 0},                    {0, 0, 0}, {0, 0, 0},                    {0, 0, 0},                    {0, 0, 0}, {0, 0, 0}}, \
/* 16KHz */   {{0, 0, 0}, {SAMPLE_FREQ_16000_HZ, 1, 2}, {0, 0, 0}, {SAMPLE_FREQ_16000_HZ, 1, 1}, {0, 0, 0}, {SAMPLE_FREQ_48000_HZ, 3, 2}, {SAMPLE_FREQ_32000_HZ, 2, 1}, {0, 0, 0}, {SAMPLE_FREQ_48000_HZ, 3, 1}}, \
              {{0, 0, 0}, {0, 0, 0},                    {0, 0, 0}, {0, 0, 0},                    {0, 0, 0}, {0, 0, 0},                    {0, 0, 0},                    {0, 0, 0}, {0, 0, 0}}, \
/* 24KHz */   {{0, 0, 0}, {SAMPLE_FREQ_24000_HZ, 1, 3}, {0, 0, 0}, {SAMPLE_FREQ_48000_HZ, 2, 3}, {0, 0, 0}, {SAMPLE_FREQ_24000_HZ, 1, 1}, {SAMPLE_FREQ_96000_HZ, 4, 3}, {0, 0, 0}, {SAMPLE_FREQ_48000_HZ, 2, 1}}, \
/* 32KHz */   {{0, 0, 0}, {SAMPLE_FREQ_32000_HZ, 1, 4}, {0, 0, 0}, {SAMPLE_FREQ_32000_HZ, 1, 2}, {0, 0, 0}, {SAMPLE_FREQ_96000_HZ, 3, 4}, {SAMPLE_FREQ_32000_HZ, 1, 1}, {0, 0, 0}, {SAMPLE_FREQ_96000_HZ, 3, 2}}, \
/* 44.1KHz */ {{0, 0, 0}, {0, 0, 0},                    {0, 0, 0}, {0, 0, 0},                    {0, 0, 0}, {0, 0, 0},                    {0, 0, 0},                    {0, 0, 0}, {0, 0, 0}}, \
/* 48KHz */   {{0, 0, 0}, {SAMPLE_FREQ_48000_HZ, 1, 6}, {0, 0, 0}, {SAMPLE_FREQ_48000_HZ, 1, 3}, {0, 0, 0}, {SAMPLE_FREQ_48000_HZ, 1, 2}, {SAMPLE_FREQ_96000_HZ, 2, 3}, {0, 0, 0}, {SAMPLE_FREQ_48000_HZ, 1, 1}}, \
              };

/* Private functions prototypes-----------------------------------------------*/
static tBleStatus CAPAPP_Init(Audio_Role_t AudioRole);
static tBleStatus GMAPAPP_GMAPInit(GMAP_Role_t Role);
static void GMAPAPP_CAPNotification(CAP_Notification_Evt_t *pNotification);
static APP_ACL_Conn_t *APP_GetACLConn(uint16_t ConnHandle);
static APP_ACL_Conn_t *APP_AllocateACLConn(uint16_t ConnHandle);
static APP_ASE_Info_t * GMAPAPP_GetASE(uint8_t ASE_ID,uint16_t ACL_ConnHandle);
uint8_t GMAPAPP_SetupAudioDataPath(uint16_t ACL_ConnHandle,
                                         uint16_t CIS_ConnHandle,
                                         uint8_t ASE_ID,
                                         uint32_t ControllerDelay);
static int32_t start_audio_source(void);
static int32_t start_audio_sink(void);
static uint8_t APP_GetBitsAudioChnlAllocations(Audio_Chnl_Allocation_t ChnlLocations);

#if (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u)
uint8_t RemoteVolume = BASE_VOLUME;
uint8_t RemoteMute = 0x00;
static void VCP_MetaEvt_Notification(VCP_Notification_Evt_t *pNotification);
#endif /*(APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u)*/

#if (APP_MICP_ROLE_CONTROLLER_SUPPORT == 1u)
uint8_t RemoteMicMute = 0x00;
static void MICP_MetaEvt_Notification(MICP_Notification_Evt_t *pNotification);
#endif /*(APP_MICP_ROLE_SERVER_SUPPORT == 1u)*/

uint8_t LocalVolume = BASE_VOLUME;
uint8_t LocalMute = 0x00;

static char Hex_To_Char(uint8_t Hex);
static uint8_t Get_LC3_Conf_ID(uint8_t QoSConfId);
static uint8_t Get_LC3_Broadcast_Conf_ID(uint8_t QoSConfId);
static Sampling_Freq_t GMAPAPP_GetTargetFrequency(uint8_t Frequency, Audio_Role_t role, BAP_ASE_Info_t *p_ase);
static void GMAPAPP_SetupBASE(uint8_t QoSConfID);
static tBleStatus GMAPAPP_BroadcastSetupAudio(void);

/* Exported functions --------------------------------------------------------*/

extern void APP_NotifyToRun(void);

/* Functions Definition ------------------------------------------------------*/

void GMAPAPP_Init(void)
{
  tBleStatus status;
  /* Init Initiator/Commander with Unicast Client */
  status = CAPAPP_Init(AUDIO_ROLE_SOURCE | AUDIO_ROLE_SINK);
  LOG_INFO_APP("CAPAPP_Init() returns status 0x%02X\n",status);
  Set_Volume(BASE_VOLUME);
  Menu_SetLocalVolume(BASE_VOLUME);
  if (status == BLE_STATUS_SUCCESS)
  {
    /* Initialize the GMAP layer*/
    status = GMAPAPP_GMAPInit(APP_GMAP_ROLE);
    LOG_INFO_APP("GMAPAPP_GMAPInit() with role 0x%02X returns status 0x%02X\n", APP_GMAP_ROLE, status);
  }
}

tBleStatus GMAPAPP_Linkup(uint16_t ConnHandle)
{
  tBleStatus status = HCI_COMMAND_DISALLOWED_ERR_CODE;
  APP_ACL_Conn_t *p_conn = APP_GetACLConn(ConnHandle);
  if (p_conn != 0)
  {
    /* Perform CAP linkup restoration if profiles are already present in NVM, else start Complete linkup process */
    GAF_Profiles_Link_t NVMLink = 0;
    if (p_conn->ForceCompleteLinkup == 0)
    {
      NVMLink = CAP_DB_GetPresentGAFProfiles(p_conn->Peer_Address_Type,p_conn->Peer_Address);
      LOG_INFO_APP("GAF Profiles Mask 0x%02X present in NVM\n",NVMLink);
    }
    if (NVMLink != 0)
    {
      GAF_Profiles_Link_t link = 0x00u;
      GAF_Profiles_Link_t current_link = CAP_GetCurrentLinkedProfiles(ConnHandle);

      LOG_INFO_APP("profiles 0x%02X of the GAF are already linked on ConnHandle 0x%04x\n",
                  current_link,
                  ConnHandle);
      if (current_link != NVMLink)
      {
        link = NVMLink & (~current_link);
        status = CAP_Linkup(ConnHandle,link,0x00u);
        LOG_INFO_APP("CAP_Linkup() for GAF restoration on ConnHandle 0x%04x for link mask 0x%02X returns status 0x%02X\n",
                    ConnHandle,
                    link,
                    status);
        GMAPAPP_Context.InitState = GMAP_APP_INIT_STATE_CAP_LINKUP;
      }
      else
      {
        LOG_INFO_APP("All profiles (link mask 0x%02X) of the GAF are already linked on ConnHandle 0x%04x\n",
                    link,
                    ConnHandle);
        status = BLE_STATUS_FAILED;
      }
    }
    else
    {
      GAF_Profiles_Link_t link = 0x00u;
      if (p_conn->Role == 0x00u)
      {
        /*Local device is Master*/
        if (APP_CAP_Config.Role  & CAP_ROLE_INITIATOR)
        {
          if ((APP_BAP_Config.Role & BAP_ROLE_UNICAST_CLIENT) == BAP_ROLE_UNICAST_CLIENT)
          {
            link |= BAP_UNICAST_LINK;
          }
        }
        if (APP_CAP_Config.Role  & CAP_ROLE_COMMANDER)
        {
          if ((APP_BAP_Config.Role & BAP_ROLE_BROADCAST_ASSISTANT) == BAP_ROLE_BROADCAST_ASSISTANT)
          {
            link |= BAP_BA_LINK;
          }
          if ((APP_VCP_Config.Role & VCP_ROLE_CONTROLLER) == VCP_ROLE_CONTROLLER)
          {
            link |= VCP_LINK;
          }
          if ((APP_MICP_Config.Role & MICP_ROLE_CONTROLLER) == MICP_ROLE_CONTROLLER)
          {
            link |= MICP_LINK;
          }
        }
        if ((APP_CAP_Config.Role & (CAP_ROLE_INITIATOR | CAP_ROLE_COMMANDER)) != 0)
        {
          if ((APP_CSIP_Config.Role & CSIP_ROLE_SET_COORDINATOR) == CSIP_ROLE_SET_COORDINATOR)
          {
            link |= CSIP_LINK;
          }
        }
      }
      else if (p_conn->Role == 0x01u)
      {
        if ((APP_CAP_Config.Role & (CAP_ROLE_ACCEPTOR | CAP_ROLE_COMMANDER)) != 0)
        {
          /* local device is Slave*/
          if ((((p_conn->AudioProfile & AUDIO_PROFILE_CCP) == 0) && ((APP_CCP_Config.Role & CCP_ROLE_CLIENT) == CCP_ROLE_CLIENT)) \
            || (((p_conn->AudioProfile & AUDIO_PROFILE_MCP) == 0) && ((APP_MCP_Config.Role & MCP_ROLE_CLIENT) == MCP_ROLE_CLIENT)))
          {
            if ((APP_CCP_Config.Role & CCP_ROLE_CLIENT) == CCP_ROLE_CLIENT)
            {
              link |= CCP_LINK;
            }
            if ((APP_MCP_Config.Role & MCP_ROLE_CLIENT) == MCP_ROLE_CLIENT)
            {
              link |= MCP_LINK;
            }
          }
        }
      }
      if (link != 0x00u)
      {
        GAF_Profiles_Link_t current_link = CAP_GetCurrentLinkedProfiles(ConnHandle);

        LOG_INFO_APP("profiles 0x%02X of the GAF are already linked on ConnHandle 0x%04x\n",
                    current_link,
                    ConnHandle);
        if (current_link != link)
        {
          link = link & (~current_link);
          status = CAP_Linkup(ConnHandle,link,0x01u);
          LOG_INFO_APP("CAP_Linkup() on ConnHandle 0x%04x for link mask 0x%02X returns status 0x%02X\n",
                      ConnHandle,
                      link,
                      status);
          GMAPAPP_Context.InitState = GMAP_APP_INIT_STATE_CAP_LINKUP;
        }
        else
        {
          LOG_INFO_APP("All profiles (link mask 0x%02X) of the GAF are already linked on ConnHandle 0x%04x\n",
                      link,
                      ConnHandle);
          status = BLE_STATUS_FAILED;
        }
      }
    }
  }
  return status;
}

/**
  * @brief  Notify CAP Events
  * @param  pNotification: pointer on notification information
 */
void CAP_Notification(CAP_Notification_Evt_t *pNotification)
{
  /* Notify App */
  GMAPAPP_CAPNotification(pNotification);
}

/**
  * @brief  Notify GMAP Events
  * @param  pNotification: pointer on notification information
 */
void GMAP_Notification(GMAP_Notification_Evt_t *pNotification)
{
  switch(pNotification->EvtOpcode)
  {
    case GMAP_LINKUP_COMPLETE_EVT:
    {
      tBleStatus ret;
      APP_ACL_Conn_t *p_conn = 0u;
      GMAP_AttServiceInfo_Evt_t *p_gmap_info = (GMAP_AttServiceInfo_Evt_t *) pNotification->pInfo;
      LOG_INFO_APP("GMAP Linkup Complete Event with ConnHandle 0x%04X is received with status 0x%02X\n",
                  pNotification->ConnHandle,
                  pNotification->Status);
      p_conn = APP_GetACLConn(pNotification->ConnHandle);
      if (pNotification->Status == BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("GMAP Role = 0x%02X\n", p_gmap_info->GMAPRole);
        LOG_INFO_APP("UGG Features = 0x%02X\n", p_gmap_info->UGGFeatures);
        LOG_INFO_APP("UGT Features = 0x%02X\n", p_gmap_info->UGTFeatures);
        LOG_INFO_APP("BGS Features = 0x%02X\n", p_gmap_info->BGSFeatures);
        LOG_INFO_APP("BGR Features = 0x%02X\n", p_gmap_info->BGRFeatures);
        if (p_conn != 0)
        {
          p_conn->AudioProfile |= AUDIO_PROFILE_GMAP;
        }
      }
      Menu_SetNoStreamPage();

      if (p_conn->ConfirmIndicationRequired == 1u)
      {
        /* Confirm indication now that the GATT is available */
        ret = aci_gatt_confirm_indication(pNotification->ConnHandle);
        LOG_INFO_APP("aci_gatt_confirm_indication() returns status 0x%02X\n", ret);
        p_conn->ConfirmIndicationRequired = 0u;
      }

#if (CFG_BLE_NUM_LINK > 0u)
      /*Check if a CAP Linkup with a discovered Set Member should started*/
      for (uint8_t i = 0; i < CFG_BLE_NUM_LINK; i++)
      {
        if (GMAPAPP_Context.ACL_Conn[i].CSIPDiscovered == 1u)
        {
          ret = GMAPAPP_Linkup(GMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle);
          LOG_INFO_APP("GMAPAPP_Linkup() on ConnHandle 0x%04X returns status 0x%02X\n",
                      GMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle,
                      ret);
          if (ret == BLE_STATUS_SUCCESS)
          {
            return;
          }
        }
      }
#endif /*(CFG_BLE_NUM_LINK > 0u)*/
      if( GMAPAPP_Context.SetMemberDiscoveryProcActive == 1u)
      {
        if (p_conn != 0)
        {
          /* Check if a new Set Member discovery should be started */
          if ((p_conn->AudioProfile & AUDIO_PROFILE_CSIP) == AUDIO_PROFILE_CSIP)
          {
            uint8_t num_members = 0u;
#if (CFG_BLE_NUM_LINK > 0u)
            for (uint8_t i = 0; i < CFG_BLE_NUM_LINK; i++)
            {
              if ((GMAPAPP_Context.ACL_Conn[i].AudioProfile & AUDIO_PROFILE_CSIP) == AUDIO_PROFILE_CSIP)
              {
                if ((GMAPAPP_Context.ACL_Conn[i].SIRK_type == p_conn->SIRK_type) \
                   && (memcmp(&GMAPAPP_Context.ACL_Conn[i].SIRK[0], &p_conn->SIRK[0],16u) == 0u))
                {
                  num_members++;
                }
              }
              else if (GMAPAPP_Context.ACL_Conn[i].CSIPDiscovered == 1u)
              {
                  num_members++;
              }
            }
#endif /*(CFG_BLE_NUM_LINK > 0u)*/
            LOG_INFO_APP("Number of Set Members discovered %d on %d\n", num_members,p_conn->Size);
            if (num_members < p_conn->Size)
            {
              /* Start scanning */
              GMAPAPP_StartScanning();
              ret = CAP_StartCoordinatedSetMemberDiscoveryProcedure(pNotification->ConnHandle);
              LOG_INFO_APP("Start Coordinated Set Member Discovery Procedure based on ConnHandle 0x%04X returns status 0x%02X\n",
                           pNotification->ConnHandle,
                           ret);
              /* perform discovery to another potential Coordinated Set Member*/
              if ( ret == BLE_STATUS_SUCCESS)
              {
                return;
              }
              else
              {
                ret = aci_gap_terminate_gap_proc(GAP_GENERAL_CONNECTION_ESTABLISHMENT_PROC);
                LOG_INFO_APP("Terminate GAP Procedure 0x%02x returns status 0x%x\n",GAP_GENERAL_CONNECTION_ESTABLISHMENT_PROC,ret);
              }
            }
          }
        }
      }
#if (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u)
      ret = CAP_VolumeController_StartSetVolumeProcedure(pNotification->ConnHandle, BASE_VOLUME);
      LOG_INFO_APP("CAP_VolumeController_StartSetVolumeProcedure() returns status 0x%02X\n", ret);
      Menu_SetRemoteVolume(BASE_VOLUME);
      if (ret == BLE_STATUS_SUCCESS)
      {
        GMAPAPP_Context.InitState = GMAP_APP_INIT_STATE_SET_VOLUME;
        GMAPAPP_Context.OperationConnHandle = pNotification->ConnHandle;
      }
#endif /* (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u) */
      UNUSED(ret);
      UNUSED(p_gmap_info);
      break;
    }
  }
}

uint8_t GMAPAPP_StartScanning(void)
{
  uint8_t status;
  Scan_Param_Phy_t scan_param_phy;
  scan_param_phy.Scan_Type = 0x00; /*Passive scanning*/
  scan_param_phy.Scan_Interval = 0x50;
  scan_param_phy.Scan_Window = 0x50;

  status = aci_gap_ext_start_scan(0x00,
                                   GAP_GENERAL_CONNECTION_ESTABLISHMENT_PROC,
                                   0x00,
                                   0x01,
                                   0x0000,
                                   0x0000,
                                   0x00,
                                   0x01,
                                   &scan_param_phy);
  LOG_INFO_APP("aci_gap_ext_start_scan() returns status 0x%02X\n",status);

  LOG_INFO_APP("GMAPAPP_StartScanning() returns status 0x%02X\n",status);
  return status;
}

uint8_t GMAPAPP_StopScanning(void)
{
  uint8_t status;

  status = aci_gap_terminate_gap_proc(GAP_GENERAL_CONNECTION_ESTABLISHMENT_PROC);
  LOG_INFO_APP("aci_gap_terminate_gap_proc() returns status 0x%02X\n",status);

  return status;
}

uint8_t GMAPAPP_CreateConnection(uint8_t *pAddress, uint8_t AddressType)
{
  Init_Param_Phy_t init_param_phy;
  tBleStatus status;
  init_param_phy.Scan_Interval       = 0x0400;
  init_param_phy.Scan_Window         = 0x0400;
  init_param_phy.Conn_Interval_Min   = 0x18;
  init_param_phy.Conn_Interval_Max   = 0x18;
  init_param_phy.Conn_Latency        = 0x0000;
  init_param_phy.Supervision_Timeout = 0x03E8;
  init_param_phy.Min_CE_Length       = 0x0000;
  init_param_phy.Max_CE_Length       = 0x03E8;

  status = aci_gap_ext_create_connection( 0x00,
                                          GAP_DIRECT_CONNECTION_ESTABLISHMENT_PROC,
                                          0x00,
                                          AddressType,
                                          pAddress,
                                          0xFFU,
                                          0xFFU,
                                          HCI_INIT_FILTER_NO,
                                          HCI_INIT_PHYS_SCAN_CONN_LE_1M,
                                          &init_param_phy);
  LOG_INFO_APP("aci_gap_ext_create_connection() returns status 0x%02X\n",status);
  return status;
}

uint8_t GMAPAPP_Disconnect(void)
{
  uint8_t status = BLE_STATUS_FAILED;
  for (uint8_t conn = 0u; conn < CFG_BLE_NUM_LINK ; conn++)
  {
    if ( GMAPAPP_Context.ACL_Conn[conn].Acl_Conn_Handle != 0xFFFFu)
    {
      status = hci_disconnect(GMAPAPP_Context.ACL_Conn[conn].Acl_Conn_Handle,
                              HCI_REMOTE_USER_TERMINATED_CONNECTION_ERR_CODE);
      LOG_INFO_APP("hci_disconnect() of ConnHandle 0x%04X returns status 0x%02X\n",
                   GMAPAPP_Context.ACL_Conn[conn].Acl_Conn_Handle,
                   status);
    }
  }

  return status;
}

uint8_t GMAPAPP_StartStream(uint8_t QOSConfIdSink, uint8_t NumChannelSink, Audio_Context_t AudioContextSink,
                            uint8_t QOSConfIdSource, uint8_t NumChannelSource, Audio_Context_t AudioContextSource,
                            uint8_t DriverConfig)
{
  uint8_t status;
  uint8_t metadata_snk_len = 0;
  uint8_t metadata_src_len = 0;
  uint8_t num_acceptors = 0u;
  uint8_t conn_ref = 0u;
  uint8_t codec_conf_id_source;
  uint8_t codec_conf_id_sink;
  CAP_Set_Acceptors_t set_type = CAP_SET_TYPE_AD_HOC;
  GMAPAPP_Context.audio_driver_config = DriverConfig;
  Audio_Role_t req_audio_role = 0;

  codec_conf_id_source = Get_LC3_Conf_ID(QOSConfIdSource);
  codec_conf_id_sink = Get_LC3_Conf_ID(QOSConfIdSink);

  LOG_INFO_APP("Start Stream\n");

  if (NumChannelSink > 0)
  {
    req_audio_role |= AUDIO_ROLE_SINK;
  }

  if (NumChannelSource > 0)
  {
    req_audio_role |= AUDIO_ROLE_SOURCE;
  }

  for (uint8_t conn = 0u; ((conn < CFG_BLE_NUM_LINK) && (num_acceptors < MAX_UNICAST_ACCEPTORS)) ; conn++)
  {
    if (num_acceptors == 0u)
    {
      if ( GMAPAPP_Context.ACL_Conn[conn].Acl_Conn_Handle != 0xFFFFu)
      {
        StartStreamParams[num_acceptors].ConnHandle = GMAPAPP_Context.ACL_Conn[conn].Acl_Conn_Handle;
        StartStreamParams[num_acceptors].StreamRole = (req_audio_role & GMAPAPP_Context.ACL_Conn[conn].UnicastServerInfo.AudioRole);
        if ( (GMAPAPP_Context.ACL_Conn[conn].AudioProfile & AUDIO_PROFILE_CSIP) == AUDIO_PROFILE_CSIP)
        {
          set_type = CAP_SET_TYPE_CSIP;
          conn_ref = conn;
        }
        else
        {
          /* we start stream with only one acceptor*/
          num_acceptors++;
          break;
        }
        num_acceptors++;
      }
    }
    else
    {
      if ( GMAPAPP_Context.ACL_Conn[conn].Acl_Conn_Handle != 0xFFFFu)
      {
        if (set_type == CAP_SET_TYPE_CSIP)
        {
          /* Check that acceptor is in the same Coordinated Set*/
          if(memcmp(&GMAPAPP_Context.ACL_Conn[conn].SIRK[0], &GMAPAPP_Context.ACL_Conn[conn_ref].SIRK[0], 16u) == 0)
          {
            StartStreamParams[num_acceptors].ConnHandle = GMAPAPP_Context.ACL_Conn[conn].Acl_Conn_Handle;
            StartStreamParams[num_acceptors].StreamRole = ((req_audio_role) & GMAPAPP_Context.ACL_Conn[conn].UnicastServerInfo.AudioRole);
            num_acceptors++;
          }
        }
      }
    }
  }

  for( uint8_t i = 0u; i < num_acceptors ; i++)
  {
    metadata_snk_len = 0u;
    metadata_src_len = 0u;
    APP_ACL_Conn_t *p_conn;

    p_conn = APP_GetACLConn(StartStreamParams[i].ConnHandle);

    if (p_conn != 0)
    {
      LOG_INFO_APP("Set Start Stream for CAP Acceptor %d on connHandle 0x%04X :\n",i,StartStreamParams[i].ConnHandle);
      for (Audio_Role_t role = AUDIO_ROLE_SINK; role <= AUDIO_ROLE_SOURCE;role++)
      {
        CAP_Unicast_AudioStart_Codec_Conf_t *pCodecConf;
        CAP_Unicast_AudioStart_QoS_Conf_t *pQoS_Conf;
        uint8_t num_chnls = 0;

        if (num_acceptors == MAX_UNICAST_ACCEPTORS)
        {
          if ((role & AUDIO_ROLE_SINK ) == AUDIO_ROLE_SINK)
          {
            pCodecConf = &CodecConfSnk[i];
            pQoS_Conf = &QosConfSnk[i];
            if ((p_conn->UnicastServerInfo.SnkAudioLocations & FRONT_LEFT) == FRONT_LEFT)
            {
              CodecConfSnk[i].AudioChannelAllocation = FRONT_LEFT;
            }
            else if ((p_conn->UnicastServerInfo.SnkAudioLocations & FRONT_RIGHT) == FRONT_RIGHT)
            {
              CodecConfSnk[i].AudioChannelAllocation = FRONT_RIGHT;
            }
            num_chnls = 1u;
          }
          else
          {
            pCodecConf = &CodecConfSrc[i];
            pQoS_Conf = &QosConfSrc[i];

            /* For conf 8(ii), only source on the first acceptor */
            if (i == 0 || NumChannelSink == 0)
            {
              pCodecConf->AudioChannelAllocation = 0x00;
              num_chnls = 1u;
            }
            else
            {
              StartStreamParams[i].StreamRole &= ~AUDIO_ROLE_SOURCE;
            }
          }
          CodecConfSnk[i].ChannelPerCIS = 1u;
          CodecConfSrc[i].ChannelPerCIS = 1u;
        }
        else
        {
          if ((role & AUDIO_ROLE_SINK ) == AUDIO_ROLE_SINK)
          {
            pCodecConf = &CodecConfSnk[i];
            pQoS_Conf = &QosConfSnk[i];

            if (NumChannelSink > 1)
            {
              pCodecConf->AudioChannelAllocation = 0x03;
              num_chnls = 2u;
            }
            else
            {
              pCodecConf->AudioChannelAllocation = 0x00;
              num_chnls = 1u;
            }

            pCodecConf->ChannelPerCIS = 1u;
          }
          else /* ((role & AUDIO_ROLE_SOURCE ) == AUDIO_ROLE_SOURCE)*/
          {
            pCodecConf = &CodecConfSrc[i];
            pQoS_Conf = &QosConfSrc[i];

            if (NumChannelSource > 1)
            {
              pCodecConf->AudioChannelAllocation = 0x03;
              num_chnls = 2u;
            }
            else
            {
              pCodecConf->AudioChannelAllocation = 0x00;
              num_chnls = 1u;
            }

            pCodecConf->ChannelPerCIS = 1u;
          }
        }

        if ((StartStreamParams[i].StreamRole & role ) == role)
        {
          pCodecConf->TargetLatency = TARGET_LATENCY_LOW;
          pCodecConf->TargetPhy = TARGET_LE_2M_PHY;
          pCodecConf->CodecID.CodingFormat = 0x06;
          pCodecConf->CodecID.CompanyID = 0x0000;
          pCodecConf->CodecID.VsCodecID = 0x0000;
          pCodecConf->CodecSpecificConfParams.FrameBlockPerSdu = 1;

          if ((role & AUDIO_ROLE_SINK ) == AUDIO_ROLE_SINK)
          {
            pCodecConf->CodecSpecificConfParams.SamplingFrequency = APP_CodecConf[codec_conf_id_sink%16].freq;
            pCodecConf->CodecSpecificConfParams.FrameDuration = APP_CodecConf[codec_conf_id_sink%16].frame_duration;
            pCodecConf->CodecSpecificConfParams.OctetPerCodecFrame = APP_CodecConf[codec_conf_id_sink%16].octets_per_codec_frame;

            pQoS_Conf->SDUInterval = APP_QoSConf[QOSConfIdSink].sdu_interval;
            pQoS_Conf->Framing = APP_QoSConf[QOSConfIdSink].framing;
            pQoS_Conf->MaxSDU = APP_QoSConf[QOSConfIdSink].max_sdu \
              * MIN(num_chnls, pCodecConf->ChannelPerCIS);
            pQoS_Conf->RetransmissionNumber = APP_QoSConf[QOSConfIdSink].rtx_num;
            pQoS_Conf->MaxTransportLatency = APP_QoSConf[QOSConfIdSink].max_tp_latency;
            pQoS_Conf->PresentationDelay = APP_QoSConf[QOSConfIdSink].presentation_delay;

            aMetadataSnk[i][metadata_snk_len++] = 0x03;
            aMetadataSnk[i][metadata_snk_len++] = METADATA_STREAMING_AUDIO_CONTEXTS;
            if ( (AudioContextSink & p_conn->UnicastServerInfo.AvailAudioContexts.Snk) != 0)
            {
              aMetadataSnk[i][metadata_snk_len++] = (uint8_t) (AudioContextSink & p_conn->UnicastServerInfo.AvailAudioContexts.Snk);
              aMetadataSnk[i][metadata_snk_len++] = (uint8_t) ((AudioContextSink & p_conn->UnicastServerInfo.AvailAudioContexts.Snk) >> 8);
            }
            else
            {
              aMetadataSnk[i][metadata_snk_len++] = (uint8_t) (AUDIO_CONTEXT_UNSPECIFIED);
              aMetadataSnk[i][metadata_snk_len++] = (uint8_t) (AUDIO_CONTEXT_UNSPECIFIED >> 8);
            }
            StartStreamParams[i].StreamSnk.pCodecConf = pCodecConf;
            StartStreamParams[i].StreamSnk.pQoSConf = pQoS_Conf;
            StartStreamParams[i].StreamSnk.pMetadata = &aMetadataSnk[i][0];
            StartStreamParams[i].StreamSnk.MetadataLen = metadata_snk_len;
            LOG_INFO_APP("Sink Codec Conf :\n");
            LOG_INFO_APP("    Audio Channel Allocation : 0x%08X\n",pCodecConf->AudioChannelAllocation);
            LOG_INFO_APP("    Channel Per CIS : %d\n",pCodecConf->ChannelPerCIS);
            LOG_INFO_APP("    Sampling Freq : 0x%02X\n",pCodecConf->CodecSpecificConfParams.SamplingFrequency);
          }
          else if ((role & AUDIO_ROLE_SOURCE ) == AUDIO_ROLE_SOURCE)
          {
            pCodecConf->CodecSpecificConfParams.SamplingFrequency = APP_CodecConf[codec_conf_id_source%16].freq;
            pCodecConf->CodecSpecificConfParams.FrameDuration = APP_CodecConf[codec_conf_id_source%16].frame_duration;
            pCodecConf->CodecSpecificConfParams.OctetPerCodecFrame = APP_CodecConf[codec_conf_id_source%16].octets_per_codec_frame;

            pQoS_Conf->SDUInterval = APP_QoSConf[QOSConfIdSource].sdu_interval;
            pQoS_Conf->Framing = APP_QoSConf[QOSConfIdSource].framing;
            pQoS_Conf->MaxSDU = APP_QoSConf[QOSConfIdSource].max_sdu \
              * MIN(num_chnls, pCodecConf->ChannelPerCIS);
            pQoS_Conf->RetransmissionNumber = APP_QoSConf[QOSConfIdSource].rtx_num;
            pQoS_Conf->MaxTransportLatency = APP_QoSConf[QOSConfIdSource].max_tp_latency;
            pQoS_Conf->PresentationDelay = APP_QoSConf[QOSConfIdSource].presentation_delay;

            aMetadataSrc[i][metadata_src_len++] = 0x03;
            aMetadataSrc[i][metadata_src_len++] = METADATA_STREAMING_AUDIO_CONTEXTS;
            if ( (AudioContextSource & p_conn->UnicastServerInfo.AvailAudioContexts.Src) != 0)
            {
              aMetadataSrc[i][metadata_src_len++] = (uint8_t) (AudioContextSource & p_conn->UnicastServerInfo.AvailAudioContexts.Src);
              aMetadataSrc[i][metadata_src_len++] = (uint8_t) ((AudioContextSource & p_conn->UnicastServerInfo.AvailAudioContexts.Src) >> 8);
            }
            else
            {
              aMetadataSrc[i][metadata_src_len++] = (uint8_t) (AUDIO_CONTEXT_UNSPECIFIED);
              aMetadataSrc[i][metadata_src_len++] = (uint8_t) (AUDIO_CONTEXT_UNSPECIFIED >> 8);
            }
            StartStreamParams[i].StreamSrc.pCodecConf = pCodecConf;
            StartStreamParams[i].StreamSrc.pQoSConf = pQoS_Conf;
            StartStreamParams[i].StreamSrc.pMetadata = &aMetadataSrc[i][0];
            StartStreamParams[i].StreamSrc.MetadataLen = metadata_src_len;
            LOG_INFO_APP("Source Codec Conf :\n");
            LOG_INFO_APP("    Audio Channel Allocation : 0x%08X\n",pCodecConf->AudioChannelAllocation);
            LOG_INFO_APP("    Channel Per CIS : %d\n",pCodecConf->ChannelPerCIS);
            LOG_INFO_APP("    Sampling Freq : 0x%02X\n",pCodecConf->CodecSpecificConfParams.SamplingFrequency);
          }
        }
      }
    }
  }

  status = CAP_Unicast_AudioStart(set_type, num_acceptors, &StartStreamParams[0]);

  LOG_INFO_APP("CAP_Unicast_AudioStart() of %d CAP Acceptors in Set Type 0x%02X returns status 0x%02X\n",
               num_acceptors,
               set_type,
               status);
  return status;
}

uint8_t GMAPAPP_StopStream(void)
{
  uint8_t status = BLE_STATUS_FAILED;
  uint16_t a_conn_handle[MAX_UNICAST_ACCEPTORS];
  uint8_t num_acceptors = 0u;
  uint8_t acc_find;
  for (uint8_t conn = 0u; conn < CFG_BLE_NUM_LINK ; conn++)
  {
    acc_find = 0u;
    if ( GMAPAPP_Context.ACL_Conn[conn].Acl_Conn_Handle != 0xFFFFu)
    {
      if (GMAPAPP_Context.ACL_Conn[conn].pASEs != 0)
      {
#if (MAX_NUM_UCL_SNK_ASE_PER_LINK > 0)
        for (uint8_t i = 0; i < MAX_NUM_UCL_SNK_ASE_PER_LINK ; i++)
        {
          if (GMAPAPP_Context.ACL_Conn[conn].pASEs->aSnkASE[i].state == ASE_STATE_STREAMING)
          {
            a_conn_handle[num_acceptors] = GMAPAPP_Context.ACL_Conn[conn].Acl_Conn_Handle;
            num_acceptors++;
            acc_find = 1u;
            break;
          }
        }
#endif /* (MAX_NUM_UCL_SNK_ASE_PER_LINK > 0) */
#if (MAX_NUM_UCL_SRC_ASE_PER_LINK > 0)
        if (acc_find == 0u)
        {
          for (uint8_t i = 0; i < MAX_NUM_UCL_SRC_ASE_PER_LINK ; i++)
          {
            if (GMAPAPP_Context.ACL_Conn[conn].pASEs->aSrcASE[i].state == ASE_STATE_STREAMING)
            {
              a_conn_handle[num_acceptors] = GMAPAPP_Context.ACL_Conn[conn].Acl_Conn_Handle;
              num_acceptors++;
              break;
            }
          }
        }
#endif /* (MAX_NUM_UCL_SRC_ASE_PER_LINK > 0) */
      }
    }
  }

  if(num_acceptors > 0u)
  {
    status = CAP_Unicast_AudioStop(num_acceptors, &a_conn_handle[0], 1u);
    LOG_INFO_APP("CAP_Unicast_AudioStop() of %d CAP Acceptors returns status 0x%02X\n",
                 num_acceptors,
                 status);
  }

  return status;
}

uint8_t GMAPAPP_RemoteVolumeUp(void)
{
#if (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u)
  uint8_t status = BLE_STATUS_SUCCESS;

  if (RemoteMute == 0)
  {
    uint8_t volume;

    if (0xFF - RemoteVolume > VOLUME_STEP)
    {
      volume = RemoteVolume + VOLUME_STEP;
    }
    else
    {
      volume = 0xFF;
    }
    for (uint8_t i = 0; i < CFG_BLE_NUM_LINK; i++)
    {
      if ((GMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle != 0xFFFFu) \
          && ((GMAPAPP_Context.ACL_Conn[i].AudioProfile & AUDIO_PROFILE_VCP) == AUDIO_PROFILE_VCP))
      {
        status = CAP_VolumeController_StartSetVolumeProcedure(GMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle, volume);
        LOG_INFO_APP("CAP_VolumeController_StartSetVolumeProcedure() with Volume %d returns status 0x%02X\n",
                     volume,
                     status);
        break;
      }
    }
  }
  else
  {
    status = GMAPAPP_RemoteToggleMute();
  }

  return status;
#else /* (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u) */
  return HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif /* (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u) */
}

uint8_t GMAPAPP_RemoteVolumeDown(void)
{
#if (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u)
  uint8_t status = BLE_STATUS_SUCCESS;

  if (RemoteMute == 0)
  {
    uint8_t volume;
    if (RemoteVolume > VOLUME_STEP)
    {
      volume = RemoteVolume - VOLUME_STEP;
    }
    else
    {
      volume = 0;
    }
    for (uint8_t i = 0; i < CFG_BLE_NUM_LINK; i++)
    {
      if ((GMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle != 0xFFFFu) \
          && ((GMAPAPP_Context.ACL_Conn[i].AudioProfile & AUDIO_PROFILE_VCP) == AUDIO_PROFILE_VCP))
      {
        status = CAP_VolumeController_StartSetVolumeProcedure(GMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle, volume);
        LOG_INFO_APP("CAP_VolumeController_StartSetVolumeProcedure() with Volume %d returns status 0x%02X\n",
                     volume,
                     status);
        break;
      }
    }
  }
  else
  {
    status = GMAPAPP_RemoteToggleMute();
  }

  return status;
#else /* (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u) */
  return HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif /* (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u) */
}

uint8_t GMAPAPP_RemoteToggleMute(void)
{
#if (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u)
  uint8_t status = BLE_STATUS_FAILED;
  uint8_t mute = (RemoteMute + 1) % 2;

  for (uint8_t i = 0; i < CFG_BLE_NUM_LINK; i++)
  {
    if ((GMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle != 0xFFFFu) \
        && ((GMAPAPP_Context.ACL_Conn[i].AudioProfile & AUDIO_PROFILE_VCP) == AUDIO_PROFILE_VCP))
    {
      status = CAP_VolumeController_StartSetVolumeMuteStateProcedure(GMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle, mute);
      LOG_INFO_APP("CAP_VolumeController_StartSetVolumeMuteStateProcedure() with Mute %d returns status 0x%02X\n",
                   mute, status);
      break;
    }
  }
  return status;
#else /* (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u) */
  return HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif /* (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u) */
}

void GMAPAPP_LocalVolumeUp(void)
{
  if (LocalMute == 1)
  {
    LocalMute = 0;
  }

  if (0xFF - LocalVolume > VOLUME_STEP)
  {
    LocalVolume = LocalVolume + VOLUME_STEP;
  }
  else
  {
    LocalVolume = 0xFF;
  }

  Set_Volume(LocalVolume);
  Menu_SetLocalVolume(LocalVolume);
}

void GMAPAPP_LocalVolumeDown(void)
{
  if (LocalVolume > VOLUME_STEP)
  {
    LocalVolume = LocalVolume - VOLUME_STEP;
  }
  else
  {
    LocalVolume = 0;
  }

  Set_Volume(LocalVolume);
  Menu_SetLocalVolume(LocalVolume);
}

void GMAPAPP_LocalToggleMute(void)
{
  if (LocalMute == 0)
  {
    LocalMute = 1;
    Set_Volume(0);
    Menu_SetLocalVolume(0);
  }
  else
  {
    LocalMute = 0;
    Set_Volume(LocalVolume);
    Menu_SetLocalVolume(LocalVolume);
  }
}

uint8_t GMAPAPP_RemoteToggleMicMute(void)
{
#if (APP_MICP_ROLE_CONTROLLER_SUPPORT == 1u)
  uint8_t status = BLE_STATUS_FAILED;
  uint8_t mute = (RemoteMicMute + 1) % 2;

  for (uint8_t i = 0; i < CFG_BLE_NUM_LINK; i++)
  {
    if ((GMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle != 0xFFFFu) \
        && ((GMAPAPP_Context.ACL_Conn[i].AudioProfile & AUDIO_PROFILE_MICP) == AUDIO_PROFILE_MICP))
    {
      status = CAP_MicrophoneController_StartSetMuteProcedure(GMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle, mute);
      LOG_INFO_APP("CAP_MicrophoneController_StartSetMuteProcedure() with Mute State %d returns status 0x%02X\n",
                   mute, status);
      break;
    }
  }
  return status;
#else /* (APP_MICP_ROLE_CONTROLLER_SUPPORT == 1u) */
  return HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif /* (APP_MICP_ROLE_CONTROLLER_SUPPORT == 1u) */
}

tBleStatus GMAPAPP_StartBroadcastSource(uint8_t QOSConfId)
{
  uint8_t ret;
  uint8_t Pb_Addr_Len;
  uint8_t Pb_Addr[6] = {0x00};
  uint8_t a_additional_adv_data[15] = {14u,
                                       AD_TYPE_COMPLETE_LOCAL_NAME,
                                       'S','T','M', '3', '2', 'W','B','A', '_', '0', '0', '0', '0'};
  CAP_Broadcast_AudioStart_Params_t cap_audio_start_params;

#if (CFG_TEST_VALIDATION == 1u)
  a_additional_adv_data[2] = 'G';
  a_additional_adv_data[3] = 'M';
  a_additional_adv_data[4] = 'A';
  a_additional_adv_data[5] = 'P';
  a_additional_adv_data[6] = '_';
#endif /*(CFG_TEST_VALIDATION == 1u)*/

  aci_hal_read_config_data(CONFIG_DATA_PUBADDR_OFFSET, &Pb_Addr_Len, &Pb_Addr[0]);
  a_additional_adv_data[11] = Hex_To_Char((Pb_Addr[1] & 0xF0) >> 4);
  a_additional_adv_data[12] = Hex_To_Char(Pb_Addr[1] & 0x0F);
  a_additional_adv_data[13] = Hex_To_Char((Pb_Addr[0] & 0xF0) >> 4);
  a_additional_adv_data[14] = Hex_To_Char(Pb_Addr[0] & 0x0F);

  GMAPAPP_SetupBASE(QOSConfId);

  if (APP_CodecConf[Get_LC3_Broadcast_Conf_ID(QOSConfId)].freq == SAMPLE_FREQ_48000_HZ
     && (BROADCAST_SOURCE_NUM_BIS == 2
       || LTV_GetNumberOfChannelsSet(BROADCAST_SOURCE_CHANNEL_ALLOC_1) == 2))
  {
    /* Reduce RTN */
    GMAPAPP_Context.RTN = 1;
  }

  cap_audio_start_params.pBaseGroup = &GMAPAPP_Context.BaseGroup;
  cap_audio_start_params.BigHandle = 0;

  /* Generate Random Broadcast ID */
  HW_RNG_Get(1, &cap_audio_start_params.BroadcastId);
  /* Reduce broadcast id on 3 bytes */
  cap_audio_start_params.BroadcastId &= 0x00FFFFFF;

  cap_audio_start_params.Rtn = GMAPAPP_Context.RTN;
  cap_audio_start_params.Phy = 0x02;
  cap_audio_start_params.Packing = BAP_PACKING_SEQUENTIAL;
  cap_audio_start_params.MaxTransportLatency = GMAPAPP_Context.MaxTransportLatency;
  cap_audio_start_params.Encryption = BAP_BROADCAST_ENCRYPTION;
  cap_audio_start_params.pBroadcastCode = &aAPP_BroadcastCode[0];
  cap_audio_start_params.AdvHandle = 0;
  cap_audio_start_params.pExtendedAdvParams = &extended_adv_params;
  cap_audio_start_params.pPeriodicAdvParams = &periodic_adv_params;
  cap_audio_start_params.pAdditionalAdvData = &a_additional_adv_data[0];
  cap_audio_start_params.AdditionalAdvDataLen = 15;

  ret = CAP_Broadcast_AudioStart(&cap_audio_start_params);
  LOG_INFO_APP("CAP_Broadcast_AudioStart() returns status 0x%02X\n", ret);

  return ret;
}

tBleStatus GMAPAPP_StopBroadcastSource(void)
{
  uint8_t ret;

  ret = CAP_Broadcast_AudioStop(BIG_HANDLE, 1);
  LOG_INFO_APP("CAP_Broadcast_AudioStop() returns status 0x%02X\n", ret);

  return ret;
}

/**
  * @brief The BLE Audio Stack requests to be executed. BLE_AUDIO_STACK_Task() shall be called.
  * @note  If the Host task is executed in a separate thread, the thread shall be unlocked here.
  */
void BLE_AUDIO_STACK_NotifyToRun(void)
{
  APP_NotifyToRun();
}

void APP_NotifyTxAudioCplt(uint16_t AudioFrameSize)
{
  uint8_t i;
  for (i = 0; i< APP_MAX_NUM_CIS; i++)
  {
    if (GMAPAPP_Context.cis_snk_handle[i] != 0xFFFFu)
    {
      CODEC_ReceiveData(GMAPAPP_Context.cis_snk_handle[i], 1, &aSnkBuff[0] + AudioFrameSize/2 + i);
    }
  }
}

void APP_NotifyTxAudioHalfCplt(void)
{
  uint8_t i;
  for (i = 0; i< APP_MAX_NUM_CIS; i++)
  {
    if (GMAPAPP_Context.cis_snk_handle[i] != 0xFFFFu)
    {
      CODEC_ReceiveData(GMAPAPP_Context.cis_snk_handle[i], 1, &aSnkBuff[0] + i);
    }
  }
}

extern uint32_t Sink_frame_size;

void CODEC_NotifyDataReady(uint16_t conn_handle, void* decoded_data)
{
  /* When only one channel is active, duplicate it for fake stereo on SAI */
  uint32_t i;
  uint8_t j;
  int16_t *pData = (int16_t *)(decoded_data);   /* 16 bits samples */

  if(pData == (pPrevData + 1)){
    /* we start receiving two channels */
    Nb_Active_Ch = 2;
  }
  pPrevData = pData;

  if (Nb_Active_Ch < 2)
  {
    for (i = 0; i < Sink_frame_size/2 - 2*GMAPAPP_Context.SinkDecimation; i+=2*GMAPAPP_Context.SinkDecimation)
    {
      /* Copy mono data to right channel */
      pData[i+1] = pData[i];

      /* When Decimation multiplier is more than one, perform linear interpolation for missing packets */
      /* Note: for a complete upsampling process, data should also be filtered */
      for (j = 1; j < GMAPAPP_Context.SinkDecimation; j++)
      {
        pData[i+2*j] = (pData[i] + pData[i+2*GMAPAPP_Context.SinkDecimation])/2;
        pData[i+2*j+1] = (pData[i] + pData[i+2*GMAPAPP_Context.SinkDecimation])/2;

      }
    }
    
    /* Process last packets */
    pData[i+1] = pData[i];
    for (j = 1; j < GMAPAPP_Context.SinkDecimation; j++)
    {
      pData[i+2*j] = pData[i];
      pData[i+2*j+1] = pData[i];
    }
  }
}

void APP_NotifyRxAudioCplt(uint16_t AudioFrameSize)
{
  uint8_t i;

  for (i = 0; i< APP_MAX_NUM_CIS; i++)
  {
    if (GMAPAPP_Context.cis_src_handle[i] != 0xFFFFu)
    {
      if (GMAPAPP_Context.audio_driver_config == AUDIO_DRIVER_CONFIG_HEADSET)
      {
        /* BSP provides microphone data in mono, send the same channel to all CIS */
        CODEC_SendData(GMAPAPP_Context.cis_src_handle[i], 1, &aSrcBuff[0] + AudioFrameSize/2);
      }
      else
      {
        /* BSP provides line data in stereo, increment pointer to change channel */
        CODEC_SendData(GMAPAPP_Context.cis_src_handle[i], 1, &aSrcBuff[0] + AudioFrameSize/2 + i);
      }
    }
  }

  for (i = 0; i< GMAPAPP_Context.CurrentNumBIS; i++)
  {
    CODEC_SendData(GMAPAPP_Context.CurrentBISConnHandle[i], 1, &aSrcBuff[0] + AudioFrameSize/2 + i);
  }
}

void APP_NotifyRxAudioHalfCplt(void)
{
  uint8_t i;

  for (i = 0; i< APP_MAX_NUM_CIS; i++)
  {
    if (GMAPAPP_Context.cis_src_handle[i] != 0xFFFFu)
    {
      if (GMAPAPP_Context.audio_driver_config == AUDIO_DRIVER_CONFIG_HEADSET)
      {
        /* BSP provides microphone data in mono, send the same channel to all CIS */
        CODEC_SendData(GMAPAPP_Context.cis_src_handle[i], 1, &aSrcBuff[0]);
      }
      else
      {
        /* BSP provides line data in stereo, increment pointer to change channel */
        CODEC_SendData(GMAPAPP_Context.cis_src_handle[i], 1, &aSrcBuff[0]+i);
      }
    }
  }

  for (i = 0; i< GMAPAPP_Context.CurrentNumBIS; i++)
  {
    CODEC_SendData(GMAPAPP_Context.CurrentBISConnHandle[i], 1, &aSrcBuff[0]+i);
  }
}

void GMAPAPP_AclConnected(uint16_t ConnHandle, uint8_t Peer_Address_Type, uint8_t Peer_Address[6], uint8_t role)
{
  uint8_t status;
  APP_ACL_Conn_t *p_conn = APP_GetACLConn(ConnHandle);
  if (p_conn == 0)
  {
    p_conn = APP_AllocateACLConn(ConnHandle);
  }
  if (p_conn != 0)
  {
    p_conn->AudioProfile = AUDIO_PROFILE_NONE;
    Menu_SetProfilesLinked(p_conn->AudioProfile);
    p_conn->Peer_Address_Type = Peer_Address_Type;
    for (uint8_t i = 0 ; i < 6u; i++)
    {
      p_conn->Peer_Address[i] = Peer_Address[i];
    }
    p_conn->Role = role;
    for (uint8_t i = 0; i < CFG_BLE_NUM_LINK;i++)
    {
      if (GMAPAPP_Context.aASEs[i].acl_conn_handle == 0xFFFFu)
      {
        p_conn->pASEs = &GMAPAPP_Context.aASEs[i];
        p_conn->pASEs->acl_conn_handle = ConnHandle;
        break;
      }
    }
  }
  status = aci_gap_send_pairing_req(ConnHandle, 0x00);
  LOG_INFO_APP("aci_gap_send_pairing_req returns %d\n",status);
  UNUSED(status);
}

void GMAPAPP_ConfirmIndicationRequired(uint16_t Conn_Handle)
{
  APP_ACL_Conn_t *p_conn = APP_GetACLConn(Conn_Handle);
  if (p_conn != 0)
  {
    p_conn->ConfirmIndicationRequired = 1u;
  }
}

void GMAPAPP_BondLost(uint16_t Conn_Handle)
{
  /* Force COMPLETE_LINKUP */
  APP_ACL_Conn_t *p_conn = APP_GetACLConn(Conn_Handle);
  if (p_conn != 0)
  {
    p_conn->ForceCompleteLinkup = 1u;
  }
}

void GMAPAPP_CISConnected(uint16_t Conn_Handle)
{
  for (uint8_t i = 0; i< APP_MAX_NUM_CIS; i++)
  {
    if (GMAPAPP_Context.cis_handle[i] == 0xFFFFu)
    {
      GMAPAPP_Context.cis_handle[i] = Conn_Handle;
      GMAPAPP_Context.num_cis_established++;
      break;
    }
  }
}

void GMAPAPP_LinkDisconnected(uint16_t Conn_Handle,uint8_t Reason)
{
  uint8_t i;
  audio_profile_t profile = 0u;
  APP_ACL_Conn_t *p_conn = APP_GetACLConn(Conn_Handle);
  if (p_conn != 0)
  {
    uint8_t status;
    profile = p_conn->AudioProfile;
    /*Free CAP Connection Slot*/
    p_conn->Acl_Conn_Handle = 0xFFFFu;
    p_conn->ForceCompleteLinkup = 0x00u;
    p_conn->ConfirmIndicationRequired = 0x00u;
    p_conn->AudioProfile = AUDIO_PROFILE_NONE;
    Menu_SetProfilesLinked(p_conn->AudioProfile);

#if (MAX_NUM_UCL_SNK_ASE_PER_LINK > 0u)
    for( i = 0; i < MAX_NUM_UCL_SNK_ASE_PER_LINK;i++)
    {
      if (p_conn->pASEs->aSnkASE[i].ID != 0x00)
      {
        LOG_INFO_APP("Free ASE ID %d on ACL connection handle 0x%04X\n",p_conn->pASEs->aSnkASE[i].ID,Conn_Handle);
        p_conn->pASEs->aSnkASE[i].ID = 0x00;
        p_conn->pASEs->aSnkASE[i].allocated  = 0x00;
        p_conn->pASEs->aSnkASE[i].state  = ASE_STATE_IDLE;
        p_conn->pASEs->aSnkASE[i].num_channels  = 0x00;
      }
    }
#endif /*(MAX_NUM_UCL_SNK_ASE_PER_LINK > 0u)*/
#if (MAX_NUM_UCL_SRC_ASE_PER_LINK > 0u)
    for( i = 0; i < MAX_NUM_UCL_SRC_ASE_PER_LINK;i++)
    {
      if (p_conn->pASEs->aSrcASE[i].ID != 0x00)
      {
        LOG_INFO_APP("Free ASE ID %d on ACL connection handle 0x%04X\n",p_conn->pASEs->aSrcASE[i].ID,Conn_Handle);
        p_conn->pASEs->aSrcASE[i].ID = 0x00;
        p_conn->pASEs->aSrcASE[i].allocated  = 0x00;
        p_conn->pASEs->aSrcASE[i].state  = ASE_STATE_IDLE;
        p_conn->pASEs->aSrcASE[i].num_channels  = 0x00;
      }
    }
#endif /*(MAX_NUM_UCL_SRC_ASE_PER_LINK > 0u)*/
    p_conn->pASEs->acl_conn_handle = 0xFFFFu;
    p_conn->pASEs = 0;
    /* Check if the disconnection is issued to a User action in remote device  or local device */
    if ((Reason == HCI_REMOTE_USER_TERMINATED_CONNECTION_ERR_CODE) \
        || (Reason == HCI_CONNECTION_TERMINATED_BY_LOCAL_HOST_ERR_CODE))
    {
      if (APP_GetNumActiveACLConnections() == 0u)
      {
        Menu_SetConfigPage();
      }
    }
    else
    {
      if (APP_GetNumActiveACLConnections() == 0u)
      {
        Menu_SetScanningPage();
        /* no more active connections, restart scanning */
        status = GMAPAPP_StartScanning();
      }
      else
      {
        if( GMAPAPP_Context.SetMemberDiscoveryProcActive == 1u)
        {
          /* Check if a new Set Member discovery should be started */
          if ((profile & AUDIO_PROFILE_CSIP) == AUDIO_PROFILE_CSIP)
          {
#if (CFG_BLE_NUM_LINK > 0u)
            for (uint8_t i = 0; i < CFG_BLE_NUM_LINK; i++)
            {
              if (GMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle != 0xFFFF)
              {
                if ((GMAPAPP_Context.ACL_Conn[i].AudioProfile & AUDIO_PROFILE_CSIP) == AUDIO_PROFILE_CSIP)
                {
                  if ((GMAPAPP_Context.ACL_Conn[i].SIRK_type == p_conn->SIRK_type) \
                     && (memcmp(&GMAPAPP_Context.ACL_Conn[i].SIRK[0], &p_conn->SIRK[0],16u) == 0u))
                  {
                    tBleStatus ret;
                    /* Start scanning */
                    GMAPAPP_StartScanning();
                    ret = CAP_StartCoordinatedSetMemberDiscoveryProcedure(GMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle) ;
                    LOG_INFO_APP("Start Coordinated Set Member Discovery Procedure based on ConnHandle 0x%04X returns status 0x%02X\n",
                                 GMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle,
                                 ret);
                    /* perform discovery to another potential Coordinated Set Member*/
                    if (ret == BLE_STATUS_SUCCESS)
                    {
                      return;
                    }
                    else
                    {
                      ret = aci_gap_terminate_gap_proc(GAP_GENERAL_CONNECTION_ESTABLISHMENT_PROC);
                      LOG_INFO_APP("Terminate GAP Procedure 0x%02x returns status 0x%x\n",GAP_GENERAL_CONNECTION_ESTABLISHMENT_PROC,ret);
                    }
                  }
                }
              }
            }
#endif /*(CFG_BLE_NUM_LINK > 0u)*/
          }
        }
      }
    }
    UNUSED(status);
  }
  else
  {
    /* Check if the Connection Handle corresponds to a CIS link */
    for (uint8_t i = 0; i< APP_MAX_NUM_CIS; i++)
    {
      if (GMAPAPP_Context.cis_handle[i] == Conn_Handle)
      {
        GMAPAPP_Context.cis_handle[i] = 0xFFFFu;
        GMAPAPP_Context.num_cis_established--;
        if (GMAPAPP_Context.num_cis_established == 0u)
        {
          LOG_INFO_APP("Deinitialized Audio Peripherals\n");
          MX_AudioDeInit();
          Menu_SetNoStreamPage();
        }
        break;
      }
    }
  }
}

uint8_t APP_GetNumActiveACLConnections(void)
{
  uint8_t num = 0u;
  for (uint8_t i = 0; i < CFG_BLE_NUM_LINK; i++)
  {
    if ( GMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle != 0xFFFF)
    {
      num++;
    }
  }
  return num;
}

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

static tBleStatus CAPAPP_Init(Audio_Role_t AudioRole)
{
  tBleStatus status;

  LOG_INFO_APP("CAPAPP_Init()\n");

  /* Initialize the Audio IP*/
  pBleAudioInit.NumOfLinks = CFG_BLE_NUM_LINK;
  pBleAudioInit.bleStartRamAddress = (uint8_t*)audio_init_buffer;
  pBleAudioInit.total_buffer_size = BLE_AUDIO_DYN_ALLOC_SIZE;
  status = BLE_AUDIO_STACK_Init(&pBleAudioInit);
  LOG_INFO_APP("BLE_AUDIO_STACK_Init() returns status 0x%02X\n",status);
  LOG_INFO_APP("BLE Audio Stack Lib version: %s\n",BLE_AUDIO_STACK_GetFwVersion());
  if(status != BLE_STATUS_SUCCESS)
  {
    return status;
  }

  /*Clear the CAP Configuration*/
  memset(&APP_CAP_Config, 0, sizeof(APP_CAP_Config));

  /*Clear the BAP Configuration*/
  memset(&APP_BAP_Config, 0, sizeof(APP_BAP_Config));

  /*Clear the MICP Configuration*/
  memset(&APP_MICP_Config, 0, sizeof(MICP_Config_t));

  /*Clear the VCP Configuration*/
  memset(&APP_VCP_Config, 0, sizeof(VCP_Config_t));

  /*Clear the CCP Configuration*/
  memset(&APP_CCP_Config, 0, sizeof(CCP_Config_t));

  /*Clear the MCP Configuration*/
  memset(&APP_MCP_Config, 0, sizeof(MCP_Config_t));

  /*Clear the CSIP Configuration*/
  memset(&APP_CSIP_Config, 0, sizeof(CSIP_Config_t));

  APP_CAP_Config.Role = APP_CAP_ROLE;
  APP_CAP_Config.MaxNumLinks = CFG_BLE_NUM_LINK;
  APP_CAP_Config.pStartRamAddr = (uint8_t *)&aCAPMemBuffer;
  APP_CAP_Config.RamSize = CAP_DYN_ALLOC_SIZE;

  APP_BAP_Config.Role = BAP_ROLE_UNICAST_CLIENT | BAP_ROLE_BROADCAST_SOURCE;
  APP_BAP_Config.MaxNumBleLinks = CFG_BLE_NUM_LINK;
  APP_BAP_Config.MaxNumUCLLinks = MAX_NUM_UCL_LINK;

  /*Published Audio Capabilites for Unicast Client and/or Broadcast Assistant Configuration*/
  APP_BAP_Config.PACSCltConfig.MaxNumSnkPACRecordsPerLink = MAX_NUM_CLT_SNK_PAC_RECORDS_PER_LINK;
  APP_BAP_Config.PACSCltConfig.MaxNumSrcPACRecordsPerLink = MAX_NUM_CLT_SRC_PAC_RECORDS_PER_LINK;
  APP_BAP_Config.PACSCltConfig.pStartRamAddr = (uint8_t *)&aPACSCltMemBuffer;
  APP_BAP_Config.PACSCltConfig.RamSize = BAP_PACS_CLT_DYN_ALLOC_SIZE;

  /*Audio Stream Endpoint of Unicast Client Configuration*/
  APP_BAP_Config.ASCSCltConfig.AudioRole = AudioRole;
  APP_BAP_Config.ASCSCltConfig.MaxNumSnkASEs = MAX_NUM_UCL_SNK_ASE_PER_LINK;
  APP_BAP_Config.ASCSCltConfig.MaxNumSrcASEs = MAX_NUM_UCL_SRC_ASE_PER_LINK;
  APP_BAP_Config.ASCSCltConfig.MaxCodecConfSize = MAX_UCL_CODEC_CONFIG_SIZE;
  APP_BAP_Config.ASCSCltConfig.MaxMetadataLength = MAX_UCL_METADATA_SIZE;
  APP_BAP_Config.ASCSCltConfig.pStartRamAddr = (uint8_t *)&aASCSCltMemBuffer;
  APP_BAP_Config.ASCSCltConfig.RamSize = BAP_ASCS_CLT_DYN_ALLOC_SIZE;

  /*Isochronous Channels Configuration*/
  APP_BAP_Config.ISOChnlConfig.MaxNumCIG = MAX_NUM_CIG;
  APP_BAP_Config.ISOChnlConfig.MaxNumCISPerCIG = MAX_NUM_CIS_PER_CIG;
  APP_BAP_Config.ISOChnlConfig.MaxNumBIG = MAX_NUM_BIG;
  APP_BAP_Config.ISOChnlConfig.MaxNumBISPerBIG = MAX_NUM_BIS_PER_BIG;
  APP_BAP_Config.ISOChnlConfig.pStartRamAddr = (uint8_t *)&aISOChnlMemBuffer;
  APP_BAP_Config.ISOChnlConfig.RamSize = BAP_ISO_CHNL_DYN_ALLOC_SIZE;

  /* Non-Volatile Memory Management for BAP Services restoration */
  APP_BAP_Config.NvmMgmtConfig.pStartRamAddr = (uint8_t *)&aNvmMgmtMemBuffer;
  APP_BAP_Config.NvmMgmtConfig.RamSize = BAP_NVM_MGMT_DYN_ALLOC_SIZE;

#if (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1)
  APP_VCP_Config.Role = VCP_ROLE_CONTROLLER;
  APP_VCP_Config.MaxNumBleLinks = CFG_BLE_NUM_LINK;
  APP_VCP_Config.Controller.MaxNumAICInstPerConn = APP_VCP_CTLR_NUM_AIC_INSTANCES;
  APP_VCP_Config.Controller.MaxNumVOCInstPerConn = APP_VCP_CTLR_NUM_VOC_INSTANCES;
  APP_VCP_Config.Controller.pStartRamAddr = (uint8_t*)aCltrMemBuffer;
  APP_VCP_Config.Controller.RamSize = BLE_VCP_CTLR_DYN_ALLOC_SIZE;
#endif /* (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1) */

#if (APP_MICP_ROLE_CONTROLLER_SUPPORT == 1)
  APP_MICP_Config.Role = MICP_ROLE_CONTROLLER;
  APP_MICP_Config.MaxNumBleLinks = CFG_BLE_NUM_LINK;
  APP_MICP_Config.Controller.MaxNumAICInstPerConn = APP_MICP_CTLR_NUM_AIC_INSTANCES;
  APP_MICP_Config.Controller.pStartRamAddr = (uint8_t*)aMicCltrMemBuffer;
  APP_MICP_Config.Controller.RamSize = BLE_MICP_CTLR_DYN_ALLOC_SIZE;
#endif /* (APP_MICP_ROLE_CONTROLLER_SUPPORT == 1) */

  APP_CSIP_Config.Role |= CSIP_ROLE_SET_COORDINATOR;
  APP_CSIP_Config.MaxNumBleLinks = CFG_BLE_NUM_LINK;
  APP_CSIP_Config.Set_Coordinator_Config.pStartRamAddr = (uint8_t*)aSetCoordinatorMemBuffer;
  APP_CSIP_Config.Set_Coordinator_Config.RamSize = BLE_CSIP_SET_COORDINATOR_DYN_ALLOC_SIZE;

  status = CAP_Init(&APP_CAP_Config,
                    &APP_BAP_Config,
                    &APP_VCP_Config,
                    &APP_MICP_Config,
                    &APP_CCP_Config,
                    &APP_MCP_Config,
                    &APP_CSIP_Config);

  LOG_INFO_APP("CAP_Init() returns status 0x%02X\n",status);
  if (status != BLE_STATUS_SUCCESS)
  {
    return status;
  }
#if (APP_CSIP_ROLE_SET_COORDINATOR_SUPPORT == 1)
    if (APP_CSIP_Config.Role & CSIP_ROLE_SET_COORDINATOR)
    {
#if (APP_CSIP_AUTOMATIC_SET_MEMBERS_DISCOVERY == 1)
      GMAPAPP_Context.SetMemberDiscoveryProcActive = 1u;
#endif /*(APP_CSIP_AUTOMATIC_SET_MEMBERS_DISCOVERY == 1)*/
    }
#endif /* (APP_CSIP_ROLE_SET_COORDINATOR_SUPPORT == 1) */
  for (uint8_t i = 0; i< APP_MAX_NUM_CIS; i++)
  {
    GMAPAPP_Context.cis_src_handle[i] = 0xFFFFu;
    GMAPAPP_Context.cis_snk_handle[i] = 0xFFFFu;
    GMAPAPP_Context.cis_handle[i] = 0xFFFFu;
  }

  GMAPAPP_Context.bap_role = APP_BAP_Config.Role;
  for (uint8_t conn = 0; conn< CFG_BLE_NUM_LINK; conn++)
  {
    GMAPAPP_Context.aASEs[conn].acl_conn_handle = 0xFFFFu;
#if (MAX_NUM_UCL_SNK_ASE_PER_LINK > 0u)
    for (uint8_t i = 0; i< MAX_NUM_UCL_SNK_ASE_PER_LINK; i++)
    {
        GMAPAPP_Context.aASEs[conn].aSnkASE[i].ID = 0x00;
        GMAPAPP_Context.aASEs[conn].aSnkASE[i].type = ASE_SINK;
        GMAPAPP_Context.aASEs[conn].aSnkASE[i].state  = ASE_STATE_IDLE;
        GMAPAPP_Context.aASEs[conn].aSnkASE[i].num_channels  = 0x00;
        GMAPAPP_Context.aASEs[conn].aSnkASE[i].allocated = 0x00u;
    }
#endif /*(MAX_NUM_UCL_SNK_ASE_PER_LINK > 0u)*/

#if (MAX_NUM_UCL_SRC_ASE_PER_LINK > 0u)
    for (uint8_t i = 0; i< MAX_NUM_UCL_SRC_ASE_PER_LINK; i++)
    {
        GMAPAPP_Context.aASEs[conn].aSrcASE[i].ID = 0x00;
        GMAPAPP_Context.aASEs[conn].aSrcASE[i].state  = ASE_STATE_IDLE;
        GMAPAPP_Context.aASEs[conn].aSrcASE[i].type = ASE_SOURCE;
        GMAPAPP_Context.aASEs[conn].aSrcASE[i].num_channels  = 0x00;
        GMAPAPP_Context.aASEs[conn].aSrcASE[i].allocated = 0x00u;
    }
#endif /*(MAX_NUM_UCL_SRC_ASE_PER_LINK > 0u)*/
  }

  for (uint8_t i = 0; i< CFG_BLE_NUM_LINK; i++)
  {
    GMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle = 0xFFFFu;
    GMAPAPP_Context.ACL_Conn[i].AudioProfile = AUDIO_PROFILE_NONE;
    GMAPAPP_Context.ACL_Conn[i].ForceCompleteLinkup = 0x00u;
    GMAPAPP_Context.ACL_Conn[i].ConfirmIndicationRequired = 0x00u;
  }

  /*Register the Audio Task */
  UTIL_SEQ_RegTask( 1<<CFG_TASK_AUDIO_ID, UTIL_SEQ_RFU, BLE_AUDIO_STACK_Task);
  /*Initialize the Audio Codec (LC3)*/
  CODEC_LC3Config_t lc3_config;

  lc3_config.SessionSize = sizeof(aLC3SessionMemBuffer);
  lc3_config.ChannelSize = sizeof(aLC3ChannelMemBuffer);
  lc3_config.StackSize = sizeof(aLC3StackMemBuffer);

  lc3_config.pSessionStart = aLC3SessionMemBuffer;
  lc3_config.pChannelStart = aLC3ChannelMemBuffer;
  lc3_config.pStackStart = aLC3StackMemBuffer;

  CODEC_ManagerInit(sizeof(aCodecPacketsMemory),
                        (uint8_t*)aCodecPacketsMemory,
                        &lc3_config,
                        CODEC_PROC_MARGIN_US,
                        CODEC_RF_SETUP_US,
                        CODEC_MODE_DEFAULT);

  /*Enable Audio Codec in LE Controller */
  GMAPAPP_Context.aStandardCodec[0].CodingFormat = AUDIO_CODING_FORMAT_LC3;
  GMAPAPP_Context.aStandardCodec[0].TransportMask = 0x03; /* CIS+BIS Transport Type */
  GMAPAPP_Context.AudioCodecInController.NumStandardCodecs = 0x01;
  GMAPAPP_Context.AudioCodecInController.pStandardCodec = &GMAPAPP_Context.aStandardCodec[0];
  GMAPAPP_Context.AudioCodecInController.NumVendorSpecificCodecs = 0x00;
  GMAPAPP_Context.AudioCodecInController.pVendorSpecificCodec = (void *)0;

  status = CAP_EnableAudioCodecController(&GMAPAPP_Context.AudioCodecInController);
  LOG_INFO_APP("Enable Audio Codec in LE Controller status 0x%02X\n",status);
  if (status != BLE_STATUS_SUCCESS)
  {
    return status;
  }

  GMAPAPP_Context.audio_driver_config = AUDIO_DRIVER_CONFIG_HEADSET;

  /*Clear Flags of the CAPAPP Context*/
  GMAPAPP_Context.audio_role_setup = 0x00;
  GMAPAPP_Context.num_cis_src = 0;
  GMAPAPP_Context.num_cis_snk = 0;

  return status;
}

static tBleStatus GMAPAPP_GMAPInit(GMAP_Role_t Role)
{
  USECASE_DEV_MGMT_Init();
  return GMAP_Init(Role, APP_GMAP_UGG_FEATURE, 0x00, APP_GMAP_BGS_FEATURE, 0x00);
}

static void APP_ReportSupportedSampleFreq(Supported_Sampling_Freq_t freq)
{
  if (freq & SUPPORTED_SAMPLE_FREQ_8000_HZ)
  {
    LOG_INFO_APP("              8 kHz\n");
  }
  if (freq & SUPPORTED_SAMPLE_FREQ_11025_HZ)
  {
    LOG_INFO_APP("              11.025 kHz\n");
  }
  if (freq & SUPPORTED_SAMPLE_FREQ_16000_HZ)
  {
    LOG_INFO_APP("              16 kHz\n");
  }
  if (freq & SUPPORTED_SAMPLE_FREQ_22050_HZ)
  {
    LOG_INFO_APP("              22.050 kHz\n");
  }
  if (freq & SUPPORTED_SAMPLE_FREQ_24000_HZ)
  {
    LOG_INFO_APP("              24 kHz\n");
  }
  if (freq & SUPPORTED_SAMPLE_FREQ_32000_HZ)
  {
    LOG_INFO_APP("              32 kHz\n");
  }
  if (freq & SUPPORTED_SAMPLE_FREQ_44100_HZ)
  {
    LOG_INFO_APP("              44.100 kHz\n");
  }
  if (freq & SUPPORTED_SAMPLE_FREQ_48000_HZ)
  {
    LOG_INFO_APP("              48 kHz\n");
  }
  if (freq & SUPPORTED_SAMPLE_FREQ_88200_HZ)
  {
    LOG_INFO_APP("              88.2 kHz\n");
  }
  if (freq & SUPPORTED_SAMPLE_FREQ_96000_HZ)
  {
    LOG_INFO_APP("              96 kHz\n");
  }
  if (freq & SUPPORTED_SAMPLE_FREQ_176400_HZ)
  {
    LOG_INFO_APP("              176.400 kHz\n");
  }
  if (freq & SUPPORTED_SAMPLE_FREQ_192000_HZ)
  {
    LOG_INFO_APP("              192 kHz\n");
  }
  if (freq & SUPPORTED_SAMPLE_FREQ_384000_HZ)
  {
    LOG_INFO_APP("              384 kHz\n");
  }
}

static void APP_ReportSupportedFrameDuration(Supported_Frame_Duration_t duration)
{
  if (duration & SUPPORTED_FRAME_DURATION_7_5_MS)
  {
    LOG_INFO_APP("              7.5 ms\n");
  }
  if (duration & SUPPORTED_FRAME_DURATION_10_MS)
  {
    LOG_INFO_APP("              10 ms\n");
  }
  if (duration & SUPPORTED_FRAME_DURATION_PREFERRED_7_5_MS)
  {
    LOG_INFO_APP("              Preferred 7.5 ms\n");
  }
  if (duration & SUPPORTED_FRAME_DURATION_PREFERRED_10_MS)
  {
    LOG_INFO_APP("              Preferred 10 ms\n");
  }
}

static void APP_ReportSupportedChnlCount(Supported_Channel_Counts_t chnls)
{
  if (chnls & SUPPORTED_AUDIO_CHNL_COUNT_1)
  {
    LOG_INFO_APP("              1 Channel\n");
  }
  if (chnls & SUPPORTED_AUDIO_CHNL_COUNT_2)
  {
    LOG_INFO_APP("              2 Channels\n");
  }
  if (chnls & SUPPORTED_AUDIO_CHNL_COUNT_3)
  {
    LOG_INFO_APP("              3 Channels\n");
  }
  if (chnls & SUPPORTED_AUDIO_CHNL_COUNT_4)
  {
    LOG_INFO_APP("              4 Channels\n");
  }
  if (chnls & SUPPORTED_AUDIO_CHNL_COUNT_5)
  {
    LOG_INFO_APP("              5 Channels\n");
  }
  if (chnls & SUPPORTED_AUDIO_CHNL_COUNT_6)
  {
    LOG_INFO_APP("              6 Channels\n");
  }
  if (chnls & SUPPORTED_AUDIO_CHNL_COUNT_7)
  {
    LOG_INFO_APP("              7 Channels\n");
  }
  if (chnls & SUPPORTED_AUDIO_CHNL_COUNT_8)
  {
    LOG_INFO_APP("              8 Channels\n");
  }
}

/**
  * @brief  Notify CAP Events for App
  * @param  pNotification: pointer on notification information
 */
static void GMAPAPP_CAPNotification(CAP_Notification_Evt_t *pNotification)
{
  LOG_INFO_APP("CAP Event : 0x%02X\n",pNotification->EvtOpcode);

  switch(pNotification->EvtOpcode)
  {
    case CAP_UNICAST_AUDIOSTARTED_EVT:
    {
      LOG_INFO_APP("CAP Unicast Start Procedure is Complete with status 0x%02X\n",
                  pNotification->Status);
      break;
    }
    case CAP_UNICAST_AUDIOSTOPPED_EVT:
    {
      LOG_INFO_APP("CAP Unicast Stop Procedure is Complete with status 0x%02X\n",
                  pNotification->Status);
      break;
    }
    case CAP_UNICAST_AUDIO_UPDATED_EVT:
    {
      LOG_INFO_APP("CAP Unicast Update Procedure is Complete with status 0x%02X\n",
                  pNotification->Status);
      break;
    }
    case CAP_LINKUP_COMPLETE_EVT:
    {
      APP_ACL_Conn_t *p_conn = APP_GetACLConn(pNotification->ConnHandle);
      LOG_INFO_APP("CAP Linkup Complete on Connhandle 0x%04X with status 0x%02X\n",
                   pNotification->ConnHandle,
                   pNotification->Status);
      if (p_conn != 0)
      {
        tBleStatus ret;
        /*Clear the 'CSIPDiscovered' field now CAP Link is up*/
        p_conn->CSIPDiscovered  = 0u;
        /* Check if GMAP is already linked or not*/
        if ((p_conn->AudioProfile & AUDIO_PROFILE_GMAP ) == 0)
        {
          p_conn->ForceCompleteLinkup = 0;
          /* GMAP linkup*/
          ret = GMAP_Linkup(pNotification->ConnHandle);
          LOG_INFO_APP("GMAP Linkup on ConnHandle 0x%04X returns status 0x%02X\n",
                      pNotification->ConnHandle,
                      ret);
          GMAPAPP_Context.InitState = GMAP_APP_INIT_STATE_GMAP_LINKUP;
        }
        else
        {
          p_conn->ForceCompleteLinkup = 0;
          if (p_conn->ConfirmIndicationRequired == 1u)
          {
            /* Confirm indication now that the GATT is available */
            tBleStatus ret;
            ret = aci_gatt_confirm_indication(pNotification->ConnHandle);
            LOG_INFO_APP("aci_gatt_confirm_indication() returns status 0x%02X\n", ret);
            p_conn->ConfirmIndicationRequired = 0u;
            UNUSED(ret);
          }
#if (CFG_BLE_NUM_LINK > 0u)
          /*Check if a CAP Linkup with a discovered Set Member should started*/
          for (uint8_t i = 0; i < CFG_BLE_NUM_LINK; i++)
          {
            if (GMAPAPP_Context.ACL_Conn[i].CSIPDiscovered == 1u)
            {
              ret = GMAPAPP_Linkup(GMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle);
              LOG_INFO_APP("CAP_Linkup() on ConnHandle 0x%04X returns status 0x%02X\n",
                          GMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle,
                          ret);
              if (ret == BLE_STATUS_SUCCESS)
              {
                return;
              }
            }
          }
#endif /*(CFG_BLE_NUM_LINK > 0u)*/
          if (pNotification->Status == BLE_STATUS_SUCCESS)
          {
            if( GMAPAPP_Context.SetMemberDiscoveryProcActive == 1u)
            {
              if (p_conn != 0)
              {
                /* Check if a new Set Member discovery should be started */
                if ((p_conn->AudioProfile & AUDIO_PROFILE_CSIP) == AUDIO_PROFILE_CSIP)
                {
                  uint8_t num_members = 0u;
  #if (CFG_BLE_NUM_LINK > 0u)
                  for (uint8_t i = 0; i < CFG_BLE_NUM_LINK; i++)
                  {
                    if ((GMAPAPP_Context.ACL_Conn[i].AudioProfile & AUDIO_PROFILE_CSIP) == AUDIO_PROFILE_CSIP)
                    {
                      if ((GMAPAPP_Context.ACL_Conn[i].SIRK_type == p_conn->SIRK_type) \
                         && (memcmp(&GMAPAPP_Context.ACL_Conn[i].SIRK[0], &p_conn->SIRK[0],16u) == 0u))
                      {
                        num_members++;
                      }
                    }
                    else if (GMAPAPP_Context.ACL_Conn[i].CSIPDiscovered == 1u)
                    {
                        num_members++;
                    }
                  }
#endif /*(CFG_BLE_NUM_LINK > 0u)*/
                  LOG_INFO_APP("Number of Set Members discovered %d on %d\n", num_members,p_conn->Size);
                  if (num_members < p_conn->Size)
                  {
                    /* Start scanning */
                    GMAPAPP_StartScanning();
                    ret = CAP_StartCoordinatedSetMemberDiscoveryProcedure(pNotification->ConnHandle);
                    LOG_INFO_APP("Start Coordinated Set Member Discovery Procedure based on ConnHandle 0x%04X returns status 0x%02X\n",
                                 pNotification->ConnHandle,
                                 ret);
                    /* perform discovery to another potential Coordinated Set Member*/
                    if (ret == BLE_STATUS_SUCCESS)
                    {
                      GMAPAPP_Context.InitState = GMAP_APP_INIT_STATE_SET_MEMBER_DISC;
                      return;
                    }
                    else
                    {
                      ret = aci_gap_terminate_gap_proc(GAP_GENERAL_CONNECTION_ESTABLISHMENT_PROC);
                      LOG_INFO_APP("Terminate GAP Procedure 0x%02x returns status 0x%02X\n",
                                   GAP_GENERAL_CONNECTION_ESTABLISHMENT_PROC,
                                   ret);
                    }
                  }
                }
              }
            }
#if (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u)
            ret = CAP_VolumeController_StartSetVolumeProcedure(pNotification->ConnHandle, BASE_VOLUME);
            LOG_INFO_APP("CAP_VolumeController_StartSetVolumeProcedure() returns status 0x%02X\n", ret);
            Menu_SetRemoteVolume(BASE_VOLUME);
            if (ret == BLE_STATUS_SUCCESS)
            {
              GMAPAPP_Context.InitState = GMAP_APP_INIT_STATE_SET_VOLUME;
              GMAPAPP_Context.OperationConnHandle = pNotification->ConnHandle;
            }
#endif /* (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u) */
          }
        }
      }
      break;
    }
    case CAP_UNICAST_LINKUP_EVT:
    {
      APP_ACL_Conn_t *p_conn;
      BAP_Unicast_Server_Info_t *p_info = (BAP_Unicast_Server_Info_t *)pNotification->pInfo;
      LOG_INFO_APP("CAP Unicast Linkup Event with status 0x%02X\n",
                  pNotification->Status);
      p_conn = APP_GetACLConn(pNotification->ConnHandle);
      if ((p_conn != 0) && (pNotification->Status == BLE_STATUS_SUCCESS))
      {
        p_conn->AudioProfile |= AUDIO_PROFILE_UNICAST;
        Menu_SetProfilesLinked(p_conn->AudioProfile);
        p_conn->UnicastServerInfo.AudioRole = p_info->AudioRole;
        p_conn->UnicastServerInfo.SnkAudioLocations = p_info->SnkAudioLocations;
        p_conn->UnicastServerInfo.SrcAudioLocations = p_info->SrcAudioLocations;
        p_conn->UnicastServerInfo.AvailAudioContexts = p_info->AvailAudioContexts;
        p_conn->UnicastServerInfo.SuppAudioContexts = p_info->SuppAudioContexts;
        p_conn->UnicastServerInfo.ASCS_StartAttHandle = p_info->ASCS_StartAttHandle;
        p_conn->UnicastServerInfo.ASCS_EndAttHandle = p_info->ASCS_EndAttHandle;
        p_conn->UnicastServerInfo.PACS_StartAttHandle = p_info->PACS_StartAttHandle;
        p_conn->UnicastServerInfo.PACS_EndAttHandle = p_info->PACS_EndAttHandle;
      }
      break;
    }
    case CAP_UNICAST_ASE_METADATA_UPDATED_EVT:
    {
      BAP_ASE_State_Params_t *p_info = (BAP_ASE_State_Params_t *)pNotification->pInfo;
      LOG_INFO_APP("Metadata of ASE ID %d with remote CAP device on ConnHandle 0x%04X is updated\n",
                  p_info->ASE_ID,
                  pNotification->ConnHandle);
      UNUSED(p_info);
      break;
    }
    case CAP_UNICAST_AUDIO_DATA_PATH_SETUP_REQ_EVT:
    {
      BAP_SetupAudioDataPathReq_t *info = (BAP_SetupAudioDataPathReq_t *)pNotification->pInfo;
      uint32_t controller_delay;
      APP_ASE_Info_t *p_ase;
      if (info->PathDirection == BAP_AUDIO_PATH_INPUT){
        LOG_INFO_APP("Setup Input Audio Data Path is requested for ASE ID %d on ACL Conn Handle 0x%04X (CIS Conn Handle 0x%04X)\n",
                     info->ASE_ID,
                     pNotification->ConnHandle,
                     info->CIS_ConnHandle);
      }
      else
      {
        LOG_INFO_APP("Setup Output Audio Data Path is requested for ASE ID %d on ACL Conn Handle 0x%04X (CIS Conn Handle 0x%04X)\n",
                   info->ASE_ID,
                   pNotification->ConnHandle,
                   info->CIS_ConnHandle);
      }
      LOG_INFO_APP("  Codec ID\n");
      LOG_INFO_APP("    Coding format : 0x%02X\n",info->CodecConf.CodecID.CodingFormat);
      LOG_INFO_APP("    Company id : 0x%04X\n",info->CodecConf.CodecID.CompanyID);
      LOG_INFO_APP("    VS codec id : 0x%04X\n",info->CodecConf.CodecID.VsCodecID);
      LOG_INFO_APP("  Presentation Delay %d us\n",info->PresentationDelay);
      LOG_INFO_APP("  Min Controller Delay %d us\n",info->ControllerDelayMin);
      LOG_INFO_APP("  Max Controller Delay %d us\n",info->ControllerDelayMax);

      /* BAP_ROLE_UNICAST_CLIENT */
      /* For client, delays are not depending on remote device, but only on local capabilities */
      if((info->ControllerDelayMin <= PREFFERED_CONTROLLER_DELAY) \
        && (info->ControllerDelayMax > PREFFERED_CONTROLLER_DELAY))
      {
        controller_delay = PREFFERED_CONTROLLER_DELAY;
      }
      else if (info->ControllerDelayMin > PREFFERED_CONTROLLER_DELAY)
      {
        controller_delay = info->ControllerDelayMin;
      }
      else /* (info->ControllerDelayMax < PREFFERED_CONTROLLER_DELAY) */
      {
        controller_delay = info->ControllerDelayMax;
      }

      p_ase = GMAPAPP_GetASE(info->ASE_ID,pNotification->ConnHandle);
      if (p_ase != 0)
      {
        p_ase->controller_delay = controller_delay;
        if (info->PathDirection == BAP_AUDIO_PATH_INPUT){
          p_ase->presentation_delay = controller_delay + APP_DELAY_SRC_MIN;
        }else{
          p_ase->presentation_delay = controller_delay + APP_DELAY_SNK_MIN;
        }
      }

      GMAPAPP_SetupAudioDataPath(pNotification->ConnHandle,info->CIS_ConnHandle,info->ASE_ID,controller_delay);
      break;
    }
    case CAP_AUDIO_CLOCK_REQ_EVT:
    {
      LOG_INFO_APP("Init Audio Clock with freq %d\n", (Sampling_Freq_t) *(pNotification->pInfo));
      AudioClock_Init((Sampling_Freq_t) *(pNotification->pInfo));
      break;
    }
    case CAP_REM_ACC_SNK_PAC_RECORD_INFO_EVT:
    case CAP_REM_ACC_SRC_PAC_RECORD_INFO_EVT:
    {
      BAP_AudioCapabilities_t *p_cap = (BAP_AudioCapabilities_t *)pNotification->pInfo;
      Supported_Sampling_Freq_t  remote_supported_sampling_freq;
      Supported_Frame_Duration_t remote_supported_frame_duration;
      Supported_Channel_Counts_t remote_supported_audio_chan;
      uint32_t                   remote_supported_octet_per_codec_frame;
      uint8_t                    remote_supported_max_codec_frame_per_sdu;
      Audio_Context_t            remote_preferred_audio_contexts = 0x0000u;

      if ( pNotification->EvtOpcode == CAP_REM_ACC_SNK_PAC_RECORD_INFO_EVT)
      {
        LOG_INFO_APP("Sink PAC Record from CAP Acceptor on ConnHandle 0x%04X\n",pNotification->ConnHandle);
      }
      else
      {
        LOG_INFO_APP("Source PAC Record from CAP Acceptor on ConnHandle 0x%04X\n",pNotification->ConnHandle);
      }

      /* Get supported codec config */
      remote_supported_sampling_freq = LTV_GetSupportedSamplingFrequencies(p_cap->pSpecificCap,
                                                                           p_cap->SpecificCapLength);
      remote_supported_frame_duration = LTV_GetSupportedFrameDuration(p_cap->pSpecificCap,
                                                                      p_cap->SpecificCapLength);
      remote_supported_octet_per_codec_frame = LTV_GetSupportedOctetsPerCodecFrame(p_cap->pSpecificCap,
                                                                                   p_cap->SpecificCapLength);
      if (remote_supported_sampling_freq == 0 || remote_supported_frame_duration == 0
          || remote_supported_octet_per_codec_frame == 0)
      {
        /* Error: Supported Sampling Freq and Supported Frame duration should be present in specific capabilities */
        break;
      }

      remote_supported_audio_chan = LTV_GetSupportedAudioChannelCounts(p_cap->pSpecificCap,
                                                                       p_cap->SpecificCapLength);
      if (remote_supported_audio_chan == 0)
      {
        /* Default value */
        remote_supported_audio_chan = 0x01;
      }

      remote_supported_max_codec_frame_per_sdu = LTV_GetSupportedMaxCodecFramesPerSDU(p_cap->pSpecificCap,
                                                                                      p_cap->SpecificCapLength);
      if (remote_supported_max_codec_frame_per_sdu == 0)
      {
        /* Default value */
        remote_supported_max_codec_frame_per_sdu = 0x01;
      }

      LTV_GetPreferredAudioContexts(p_cap->pMetadata,
                                    p_cap->MetadataLength,
                                    &remote_preferred_audio_contexts);

      LOG_INFO_APP("Remote Record: \n");
      LOG_INFO_APP("    Supported Sample Freq: \n");
      APP_ReportSupportedSampleFreq(remote_supported_sampling_freq);
      LOG_INFO_APP("    Supported Frame Duration: \n");
      APP_ReportSupportedFrameDuration(remote_supported_frame_duration);
      LOG_INFO_APP("    Supported Channel Counts: \n");
      APP_ReportSupportedChnlCount(remote_supported_audio_chan);
      LOG_INFO_APP("    Supported Octets Per Codec Frame: 0x%02X\n",remote_supported_octet_per_codec_frame);
      LOG_INFO_APP("    Supported Max Codec Frame Per SDU: %d\n",remote_supported_max_codec_frame_per_sdu);
      LOG_INFO_APP("    Supported Preferred Audio Contexts: 0x%04X\n",remote_preferred_audio_contexts);
      break;
    }
    case CAP_UNICAST_AUDIO_CONNECTION_UP_EVT:
    {
      BAP_Unicast_Audio_Path_t *info = (BAP_Unicast_Audio_Path_t *)pNotification->pInfo;

      if (info->AudioPathDirection == BAP_AUDIO_PATH_INPUT){
        LOG_INFO_APP("Input Audio Data Path is up with status 0x%02X for CIS Conn handle 0x%04X\n",
                    pNotification->Status,info->CIS_ConnHandle);
        LOG_INFO_APP("Controller Delay : %d us\n", info->ControllerDelay);
        LOG_INFO_APP("Transport Latency : %d us\n", info->TransportLatency);
        if (pNotification->Status == 0x00)
        {
          GMAPAPP_Context.audio_role_setup |= AUDIO_ROLE_SOURCE;
        }
      }
      else
      {
        Audio_Chnl_Allocation_t channel_alloc = LTV_GetConfiguredAudioChannelAllocation(info->CodecConf.pSpecificConf,
                                                                                        info->CodecConf.SpecificConfLength);
        uint8_t num_channels = APP_GetBitsAudioChnlAllocations(channel_alloc);
        if (num_channels == 0)
        {
          /* Alloc 0 = Mono */
          num_channels = 1;
        }
        GMAPAPP_Context.NumOutputChannels += num_channels;
        LOG_INFO_APP("Number of Output Channels Up:  %d\n", GMAPAPP_Context.NumOutputChannels);
        LOG_INFO_APP("Output Audio Data Path is up with status 0x%02X for CIS Conn handle 0x%04X\n",
                    pNotification->Status,
                    info->CIS_ConnHandle);
        LOG_INFO_APP("Controller Delay : %d us\n", info->ControllerDelay);
        LOG_INFO_APP("Transport Latency : %d us\n", info->TransportLatency);
        if (pNotification->Status == 0x00)
        {
          GMAPAPP_Context.audio_role_setup |= AUDIO_ROLE_SINK;
        }
      }
      break;
    }
    case CAP_UNICAST_AUDIO_CONNECTION_DOWN_EVT:
    {
      BAP_Unicast_Audio_Path_t *info = (BAP_Unicast_Audio_Path_t *)pNotification->pInfo;
      uint8_t i;

      /*Clear Flags of the CAPAPP Context*/
      GMAPAPP_Context.audio_role_setup = 0x00;

      if (info->AudioPathDirection == BAP_AUDIO_PATH_INPUT){
        LOG_INFO_APP("Input Audio Data Path is down with status 0x%02X for CIS Conn handle 0x%04X\n",
                    pNotification->Status,
                    info->CIS_ConnHandle);
        if (info->CIS_ConnHandle != 0xFFFF)
        {
          for (i = 0; i< APP_MAX_NUM_CIS; i++)
          {
            if (GMAPAPP_Context.cis_src_handle[i] == info->CIS_ConnHandle)
            {
              GMAPAPP_Context.cis_src_handle[i] = 0xFFFFu;
              GMAPAPP_Context.num_cis_src--;
              break;
            }
          }
        }
        if (GMAPAPP_Context.num_cis_src == 0u)
        {
          LOG_INFO_APP("No more Input Audio Data Path is up\n");
          GMAPAPP_Context.audio_role_setup &= ~AUDIO_ROLE_SOURCE;
          LOG_INFO_APP("Stop Rx Audio Peripheral Driver\n");
          Stop_RxAudio();
        }
      }
      else
      {
        Audio_Chnl_Allocation_t channel_alloc = LTV_GetConfiguredAudioChannelAllocation(info->CodecConf.pSpecificConf,
                                                                                        info->CodecConf.SpecificConfLength);
        uint8_t num_channels = APP_GetBitsAudioChnlAllocations(channel_alloc);
        if (num_channels == 0)
        {
          /* Alloc 0 = Mono */
          num_channels = 1;
        }
        GMAPAPP_Context.NumOutputChannels -= num_channels;
        LOG_INFO_APP("Number of Output Channels Up:  %d\n", GMAPAPP_Context.NumOutputChannels);
        LOG_INFO_APP("Output Audio Data Path is down with status 0x%02X for CIS Conn handle 0x%04X\n",
                    pNotification->Status,
                    info->CIS_ConnHandle);

        if (info->CIS_ConnHandle != 0xFFFF)
        {
          for (i = 0; i< APP_MAX_NUM_CIS; i++)
          {
            if (GMAPAPP_Context.cis_snk_handle[i] == info->CIS_ConnHandle)
            {
              GMAPAPP_Context.cis_snk_handle[i] = 0xFFFFu;
              GMAPAPP_Context.num_cis_snk--;
              break;
            }
          }
        }
        if (GMAPAPP_Context.num_cis_snk == 0u)
        {
          LOG_INFO_APP("No more Output Audio Data Path is up\n");
          GMAPAPP_Context.audio_role_setup &= ~AUDIO_ROLE_SINK;
          LOG_INFO_APP("Stop Tx Audio Peripheral Driver\n");
          Stop_TxAudio();
        }
      }
      break;
    }

    case CAP_UNICAST_CLIENT_ASE_STATE_EVT:
    {
      BAP_ASE_State_Params_t *p_info = (BAP_ASE_State_Params_t *)pNotification->pInfo;
      APP_ASE_Info_t *p_ase = 0;
      LOG_INFO_APP("ASE ID %d [Type 0x%02X ] with remote CAP Acceptor on ConnHandle 0x%04X is in State 0x%02X\n",
                  p_info->ASE_ID,
                  p_info->Type,
                  pNotification->ConnHandle,
                  p_info->State);

      APP_ACL_Conn_t *p_conn = APP_GetACLConn(pNotification->ConnHandle);
      if (p_conn != 0)
      {
        p_ase = GMAPAPP_GetASE(p_info->ASE_ID,pNotification->ConnHandle);
        if (p_ase == 0)
        {
          if (p_info->Type == ASE_SINK)
          {
#if (MAX_NUM_UCL_SNK_ASE_PER_LINK > 0u)
            for ( uint8_t i = 0; i < MAX_NUM_UCL_SNK_ASE_PER_LINK;i++)
            {
              if (p_conn->pASEs->aSnkASE[i].allocated == 0)
              {
                p_ase = &p_conn->pASEs->aSnkASE[i];
                p_ase->ID = p_info->ASE_ID;
                p_ase->type = p_info->Type;
                p_ase->allocated = 1u;
              }
            }
#endif /* (MAX_NUM_UCL_SNK_ASE_PER_LINK > 0u) */
          }
          if (p_info->Type == ASE_SOURCE)
          {
#if (MAX_NUM_UCL_SRC_ASE_PER_LINK > 0u)
            for (uint8_t i = 0; i < MAX_NUM_UCL_SRC_ASE_PER_LINK;i++)
            {
              if (p_conn->pASEs->aSrcASE[i].allocated == 0)
              {
                p_ase = &p_conn->pASEs->aSrcASE[i];
                p_ase->ID = p_info->ASE_ID;
                p_ase->type = p_info->Type;
                p_ase->allocated = 1u;
              }
            }
#endif /* (MAX_NUM_UCL_SRC_ASE_PER_LINK > 0u) */
          }
        }
        if (p_ase != 0)
        {
          p_ase->state = p_info->State;
        }
      }
      break;
    }

    case CAP_VCP_META_EVT:
    {
      VCP_Notification_Evt_t *p_vcp_evt = (VCP_Notification_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Volume Control Meta Event 0x%02X is received on ConnHandle 0x%04X:\n",
                  p_vcp_evt->EvtOpcode,
                  p_vcp_evt->ConnHandle);
#if (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u)
      VCP_MetaEvt_Notification(p_vcp_evt);
#endif /* (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u) */
      break;
    }

    case CAP_MICP_META_EVT:
    {
      MICP_Notification_Evt_t *p_micp_evt = (MICP_Notification_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Microphone Control Meta Event 0x%02X is received on ConnHandle 0x%04X:\n",
                  p_micp_evt->EvtOpcode,
                  p_micp_evt->ConnHandle);
#if (APP_MICP_ROLE_CONTROLLER_SUPPORT == 1u)
      MICP_MetaEvt_Notification(p_micp_evt);
#endif /* (APP_MICP_ROLE_CONTROLLER_SUPPORT == 1u) */
      break;
    }

    case CAP_MICROPHONE_CTRL_LINKUP_EVT:
    {
      APP_ACL_Conn_t *p_conn;
      LOG_INFO_APP("Microphone Control Profile is linked on ConnHandle 0x%04X\n",pNotification->ConnHandle);
      p_conn = APP_GetACLConn(pNotification->ConnHandle);
      if ((p_conn != 0) && (pNotification->Status == BLE_STATUS_SUCCESS))
      {
        p_conn->AudioProfile |= AUDIO_PROFILE_MICP;
        Menu_SetProfilesLinked(p_conn->AudioProfile);
      }
      break;
    }

    case CAP_SET_MICROPHONE_MUTE_PROCEDURE_COMPLETE_EVT:
    {
      LOG_INFO_APP("Set Microphone Mute procedure is complete\n");
      if (GMAPAPP_Context.InitState == GMAP_APP_INIT_STATE_SET_MIC_MUTE)
      {
        /* End of Linkup init process */
        GMAPAPP_Context.InitState = GMAP_APP_INIT_STATE_IDLE;
      }
      break;
    }

    case CAP_VOLUME_CTRL_LINKUP_EVT:
    {
      APP_ACL_Conn_t *p_conn;
      LOG_INFO_APP("Volume Control Profile is linked on ConnHandle 0x%04X\n",pNotification->ConnHandle);
      p_conn = APP_GetACLConn(pNotification->ConnHandle);
      if ((p_conn != 0) && (pNotification->Status == BLE_STATUS_SUCCESS))
      {
        p_conn->AudioProfile |= AUDIO_PROFILE_VCP;
        Menu_SetProfilesLinked(p_conn->AudioProfile);
      }
      break;
    }
    case CAP_SET_VOLUME_PROCEDURE_COMPLETE_EVT:
    {
      LOG_INFO_APP("CAP Set Volume Procedure is complete with status 0x%02X\n",
                   pNotification->Status);
      if (GMAPAPP_Context.InitState == GMAP_APP_INIT_STATE_SET_VOLUME)
      {
        tBleStatus status;
        status = CAP_MicrophoneController_StartSetMuteProcedure(GMAPAPP_Context.OperationConnHandle, 0);
        LOG_INFO_APP("VCAP_MicrophoneController_StartSetMuteProcedure with status 0x%02X\n", status);
        UNUSED(status);
        GMAPAPP_Context.InitState = GMAP_APP_INIT_STATE_SET_MIC_MUTE;
      }
      break;
    }

    case CAP_SET_VOLUME_MUTE_STATE_PROCEDURE_COMPLETE_EVT:
    {
      LOG_INFO_APP("CAP Set Volume Mute State Procedure is complete with status 0x%02X\n",
                   pNotification->Status);
      break;
    }

    case CAP_CSI_LINKUP_EVT:
    {
      APP_ACL_Conn_t *p_conn;
      CSIP_SetMember_Info_t *p_info = (CSIP_SetMember_Info_t *) pNotification->pInfo;
      LOG_INFO_APP("Coordinated Set Indentification Profile is linked on ConnHandle 0x%04X\n",
                   pNotification->ConnHandle);
      if (pNotification->Status == BLE_STATUS_SUCCESS)
      {
        p_conn = APP_GetACLConn(pNotification->ConnHandle);
        if (p_conn != 0)
        {
          p_conn->AudioProfile |= AUDIO_PROFILE_CSIP;
          if (p_conn != 0)
          {
            p_conn->AudioProfile |= AUDIO_PROFILE_CSIP;
            p_conn->SIRK_type = p_info->SetIdentityResolvingKeyType;
            p_conn->Rank = p_info->SetMemberRank;
            p_conn->Size = p_info->CoordinatedSetSize;
          }
          for (uint8_t i = 0; i < 16 ; i++)
          {
            if (p_conn != 0)
            {
              p_conn->SIRK[i] =  p_info->pSetIdentityResolvingKey[i];
            }
          }
        }
      }
      break;
    }

    case CAP_CSIP_META_EVT:
      {
        CSIP_Notification_Evt_t *p_csip_evt = (CSIP_Notification_Evt_t *)pNotification->pInfo;
        LOG_INFO_APP("Coordinated Set Identification Meta Event 0x%02X is received on ConnHandle 0x%04X:\n",
                    p_csip_evt->EvtOpcode,
                    p_csip_evt->ConnHandle);
        switch (p_csip_evt->EvtOpcode)
        {
          case CSIP_COO_ADV_REPORT_NEW_SET_MEMBER_DISCOVERED_EVT:
            {
              CSIP_New_Member_Evt_Params_t *p_info = (CSIP_New_Member_Evt_Params_t *) p_csip_evt->pInfo;
              LOG_INFO_APP("New Set Member has been discovered with bdaddr (type : 0x%02X) : %02x:%02x:%02x:%02x:%02x:%02x\n",
                              p_info->AddressType,
                              p_info->Address[5],
                              p_info->Address[4],
                              p_info->Address[3],
                              p_info->Address[2],
                              p_info->Address[1],
                              p_info->Address[0]);

              if( GMAPAPP_Context.SetMemberDiscoveryProcActive == 1u)
              {
                tBleStatus status;
                status = GMAPAPP_CreateConnection(p_info->Address, p_info->AddressType);
                LOG_INFO_APP("GMAPAPP_CreateConnection with status 0x%02X\n", status);
                UNUSED(status);
              }
            }
          break;;

          case CSIP_COO_NEW_SET_MEMBER_DISCOVERED_EVT:
          {
            LOG_INFO_APP("Set Member discovery is Complete with ConnHandle 0x%04X with status 0x%02X\n",
                         p_csip_evt->ConnHandle,
                         p_csip_evt->Status);
            if (p_csip_evt->Status == BLE_STATUS_SUCCESS)
            {
              APP_ACL_Conn_t *p_conn = APP_GetACLConn(p_csip_evt->ConnHandle);
              if (p_conn == 0)
              {
                  APP_ACL_Conn_t *p_conn = APP_AllocateACLConn(p_csip_evt->ConnHandle);
                  if (p_conn != 0)
                  {
                    p_conn->AudioProfile = AUDIO_PROFILE_NONE;
                    Menu_SetProfilesLinked(p_conn->AudioProfile);
                    p_conn->Rank = 0u;
                    p_conn->Size = 0u;
                    p_conn->CSIPDiscovered = 0u;
                  }
              }
              if (p_conn != 0)
              {
                /* Indicate that the Set Member has been discovered */
                p_conn->CSIPDiscovered = 1u;
              }
            }
          break;
          }

          case CSIP_COO_SET_MEMBER_DISCOVERY_PROCEDURE_COMPLETE_EVT:
          {
            tBleStatus hciCmdResult;
            LOG_INFO_APP("CSIP Set Members Discovery Procedure is complete with status 0x%02X\n",
                            pNotification->Status);
            if (pNotification->Status != BLE_STATUS_SUCCESS)
            {
              hciCmdResult = aci_gap_terminate_gap_proc(GAP_GENERAL_CONNECTION_ESTABLISHMENT_PROC);
              LOG_INFO_APP("Terminate GAP Procedure 0x%02x returns status 0x%x\n",GAP_GENERAL_CONNECTION_ESTABLISHMENT_PROC,hciCmdResult);
            }
#if (CFG_BLE_NUM_LINK > 0u)
            /*Check if a CAP Linkup witha discovered Set Member should started*/
            for (uint8_t i = 0; i < CFG_BLE_NUM_LINK; i++)
            {
              if (GMAPAPP_Context.ACL_Conn[i].CSIPDiscovered == 1u)
              {
                hciCmdResult = GMAPAPP_Linkup(GMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle);
                LOG_INFO_APP("GMAPAPP_Linkup() on ConnHandle 0x%04X returns status 0x%02X\n",
                             GMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle,
                             hciCmdResult);
                if (hciCmdResult == BLE_STATUS_SUCCESS)
                {
                  return;
                }
              }
            }
#endif /*(CFG_BLE_NUM_LINK > 0u)*/
#if (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u)
            hciCmdResult = CAP_VolumeController_StartSetVolumeProcedure(pNotification->ConnHandle, BASE_VOLUME);
            LOG_INFO_APP("CAP_VolumeController_StartSetVolumeProcedure() returns status 0x%02X\n", hciCmdResult);
            Menu_SetRemoteVolume(BASE_VOLUME);
            if (hciCmdResult == BLE_STATUS_SUCCESS)
            {
              GMAPAPP_Context.InitState = GMAP_APP_INIT_STATE_SET_VOLUME;
              GMAPAPP_Context.OperationConnHandle = pNotification->ConnHandle;
            }
#endif /* (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u) */
			UNUSED(hciCmdResult);
          break;
          }

          default:
          break;
        }
      }
      break;

    case CAP_BROADCAST_AUDIOSTARTED_EVT:
    {
      tBleStatus ret;
      CAP_Broadcast_AudioStarted_Info_t *data = (CAP_Broadcast_AudioStarted_Info_t*) pNotification->pInfo;
      LOG_INFO_APP("CAP_BROADCAST_AUDIOSTARTED_EVT with status 0x%02X\n", pNotification->Status);

      GMAPAPP_Context.CurrentNumBIS = data->NumBISes;
      UTIL_MEM_cpy_8(&(GMAPAPP_Context.CurrentBISConnHandle[0]),
                     data->pConnHandle,
                     (data->NumBISes * sizeof(uint16_t)));

      ret = GMAPAPP_BroadcastSetupAudio();
      LOG_INFO_APP("GMAPAPP_BroadcastSetupAudio with status 0x%02X\n", pNotification->Status);
      UNUSED(ret);
      break;
    }

    case CAP_BROADCAST_AUDIOSTOPPED_EVT:
    {
      LOG_INFO_APP("CAP_BROADCAST_AUDIOSTOPPED_EVT\n");
      break;
    }

    case CAP_BROADCAST_AUDIO_UP_EVT:
    {
      LOG_INFO_APP("CAP_BROADCAST_AUDIO_UP_EVT\n");
      break;
    }

    case CAP_BROADCAST_AUDIO_DOWN_EVT:
    {
      LOG_INFO_APP("CAP_BROADCAST_AUDIO_DOWN_EVT\n");
      GMAPAPP_Context.CurrentNumBIS = 0;
      MX_AudioDeInit();
      break;
    }

    default:
      break;
  }
}

static APP_ACL_Conn_t *APP_GetACLConn(uint16_t ConnHandle)
{
  for (uint8_t i = 0; i < CFG_BLE_NUM_LINK; i++)
  {
    if ( GMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle == ConnHandle)
    {
      return &GMAPAPP_Context.ACL_Conn[i];
    }
  }
  return 0;
}

static APP_ACL_Conn_t *APP_AllocateACLConn(uint16_t ConnHandle)
{
  /* Allocate new slot */
  for (uint8_t i = 0; i < CFG_BLE_NUM_LINK; i++)
  {
    if ( GMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle == 0xFFFFu)
    {
      GMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle = ConnHandle;

      return &GMAPAPP_Context.ACL_Conn[i];
    }
  }
  return 0;
}

static APP_ASE_Info_t * GMAPAPP_GetASE(uint8_t ASE_ID,uint16_t ACL_ConnHandle)
{
  uint8_t i;
  APP_ACL_Conn_t *p_conn = APP_GetACLConn(ACL_ConnHandle);
  if (p_conn != 0)
  {
    if (p_conn->pASEs != 0)
    {
#if (MAX_NUM_UCL_SNK_ASE_PER_LINK > 0u)
      for( i = 0; i < MAX_NUM_UCL_SNK_ASE_PER_LINK;i++)
      {
        if((p_conn->pASEs->aSnkASE[i].ID == ASE_ID) && (p_conn->pASEs->aSnkASE[i].allocated == 1u))
        {
          return &p_conn->pASEs->aSnkASE[i];
        }
      }
#endif /*(MAX_NUM_UCL_SNK_ASE_PER_LINK > 0u)*/
#if (MAX_NUM_UCL_SRC_ASE_PER_LINK > 0u)
      for( i = 0; i < MAX_NUM_UCL_SRC_ASE_PER_LINK;i++)
      {
        if((p_conn->pASEs->aSrcASE[i].ID == ASE_ID)  && (p_conn->pASEs->aSnkASE[i].allocated == 1u))
        {
          return &p_conn->pASEs->aSrcASE[i];
        }
      }
#endif /*(MAX_NUM_UCL_SRC_ASE_PER_LINK > 0u)*/
    }
  }
  return 0;
}

uint8_t GMAPAPP_SetupAudioDataPath(uint16_t ACL_ConnHandle,
                                  uint16_t CIS_ConnHandle,
                                  uint8_t ASE_ID,
                                  uint32_t ControllerDelay)
{
  tBleStatus status;
  Audio_Role_t role;
  Sampling_Freq_t frequency;
  Frame_Duration_t frame_duration;
  Audio_Chnl_Allocation_t audio_chnls;
  uint8_t num_chnls = 0;
  uint8_t i = 0;

  APP_ACL_Conn_t *p_conn;
  BAP_ASE_Info_t * p_ase;

  p_ase = CAP_Unicast_GetASEInfo(ACL_ConnHandle,ASE_ID);
  p_conn = APP_GetACLConn(ACL_ConnHandle);
  if(p_ase != 0 && p_conn != 0)
  {
    frequency = LTV_GetConfiguredSamplingFrequency(p_ase->params.Codec.CodecConf.pSpecificConf,
                                                   p_ase->params.Codec.CodecConf.SpecificConfLength);
    frame_duration = LTV_GetConfiguredFrameDuration(p_ase->params.Codec.CodecConf.pSpecificConf,
                                                    p_ase->params.Codec.CodecConf.SpecificConfLength);

   if (((p_ase->Type == ASE_SOURCE) && (GMAPAPP_Context.bap_role & BAP_ROLE_UNICAST_SERVER)) || \
      ((p_ase->Type == ASE_SINK) && (GMAPAPP_Context.bap_role & BAP_ROLE_UNICAST_CLIENT)))
    {
      /*Audio Data packets are sent to the remote device*/
      role = AUDIO_ROLE_SOURCE;
      LOG_INFO_APP("Setup Audio Peripheral for Source Role at Sampling frequency %d\n",frequency);
      GMAPAPP_Context.ConfiguredSourceSampleFrequency = frequency;
    }
    else if(((p_ase->Type == ASE_SINK) && (GMAPAPP_Context.bap_role & BAP_ROLE_UNICAST_SERVER)) || \
          ((p_ase->Type == ASE_SOURCE) && (GMAPAPP_Context.bap_role & BAP_ROLE_UNICAST_CLIENT)))
    {
      /* Audio Data packets are received from the remote device.*/
      role = AUDIO_ROLE_SINK;
      LOG_INFO_APP("Setup Audio Peripheral for Sink Role at Sampling frequency %d\n",frequency);
      GMAPAPP_Context.ConfiguredSinkSampleFrequency = frequency;
    }
    else
    {
      return BLE_STATUS_INVALID_PARAMS;
    }
    audio_chnls = LTV_GetConfiguredAudioChannelAllocation(p_ase->params.Codec.CodecConf.pSpecificConf,
                                                          p_ase->params.Codec.CodecConf.SpecificConfLength);

    if(audio_chnls != 0x00000000)
    {
      LOG_INFO_APP("Audio Channels Allocation Configuration : 0x%08X\n",audio_chnls);
      num_chnls = APP_GetBitsAudioChnlAllocations(audio_chnls);
      LOG_INFO_APP("Number of Audio Channels %d \n",num_chnls);
    }
    if (GMAPAPP_Context.audio_role_setup == 0x00)
    {
      Sampling_Freq_t target_frequency = GMAPAPP_GetTargetFrequency(frequency, role, p_ase);

      LOG_INFO_APP("Configure Audio Periphal drivers at Sampling frequency %d\n",target_frequency);
      MX_AudioInit(role,target_frequency,
                   frame_duration,
                   (uint8_t *)aSnkBuff,
                   (uint8_t *)aSrcBuff,
                   (AudioDriverConfig) GMAPAPP_Context.audio_driver_config);
    }
    if (role == AUDIO_ROLE_SOURCE)
    {
      for (i = 0; i< APP_MAX_NUM_CIS; i++)
      {
        if (GMAPAPP_Context.cis_src_handle[i] == 0xFFFFu)
        {
          GMAPAPP_Context.cis_src_handle[i] = CIS_ConnHandle;
          GMAPAPP_Context.num_cis_src++;
          break;
        }
      }
      if ((GMAPAPP_Context.audio_role_setup & role) == 0x00)
      {
        LOG_INFO_APP("Register callback to Start Audio Peripheral Rx\n");
        CODEC_RegisterTriggerClbk(1,0,&start_audio_source);
      }
    }
    else if (role == AUDIO_ROLE_SINK)
    {

      for (i = 0; i< APP_MAX_NUM_CIS; i++)
      {
        if (GMAPAPP_Context.cis_snk_handle[i] == 0xFFFFu)
        {
          GMAPAPP_Context.cis_snk_handle[i] = CIS_ConnHandle;
          GMAPAPP_Context.num_cis_snk++;
          break;
        }
      }
      if ((GMAPAPP_Context.audio_role_setup & role) == 0x00)
      {
        LOG_INFO_APP("Register callback to Start Audio Peripheral Tx\n");
        CODEC_RegisterTriggerClbk(1,1,&start_audio_sink);
      }
    }
    CODEC_DataPathParam_t param;
    /* input data path */
    param.SampleDepth = 16;

    /* SAI/I2C peripheral driver requests to set decimation to 2*/

    if (role == AUDIO_ROLE_SOURCE)
    {
      if (GMAPAPP_Context.audio_driver_config == AUDIO_DRIVER_CONFIG_HEADSET)
      {
        param.Decimation = GMAPAPP_Context.SourceDecimation; /* 1 * sample rate multiplier */
      }
      else
      {
        param.Decimation = GMAPAPP_Context.SourceDecimation * 2; /* 2 * sample rate multiplier */
      }
    }
    else
    {
      param.Decimation = GMAPAPP_Context.SinkDecimation * 2; /* 2 * sample rate multiplier */
    }

    /*Data Path ID is vendor-specific transport interface : 0x01 for "Shared memory of SAI"*/
    status = CAP_Unicast_SetupAudioDataPath(CIS_ConnHandle,
                                            ASE_ID,
                                            DATA_PATH_CIRCULAR_BUF,
                                            ControllerDelay,
                                            CONFIGURE_DATA_PATH_CONFIG_LEN,
                                            (uint8_t *)&param);
    LOG_INFO_APP("Setup Unicast Audio Data Path for ASE ID %d on CIS connection handle 0x%04X with controller delay at %d us returns status 0x%02X\n",
                ASE_ID,
                CIS_ConnHandle,
                ControllerDelay,
                status);
    if(status != BLE_STATUS_SUCCESS)
    {
      /*remove cis_handle*/
      if (role == AUDIO_ROLE_SOURCE)
      {
        for (i = 0; i< APP_MAX_NUM_CIS; i++)
        {
          if (GMAPAPP_Context.cis_src_handle[i] == CIS_ConnHandle)
          {
            GMAPAPP_Context.cis_src_handle[i] = 0xFFFFu;
            GMAPAPP_Context.num_cis_src--;
            break;
          }
        }
      }
      else if (role == AUDIO_ROLE_SINK)
      {
        for (i = 0; i< APP_MAX_NUM_CIS; i++)
        {
          if (GMAPAPP_Context.cis_snk_handle[i] == CIS_ConnHandle)
          {
            GMAPAPP_Context.cis_snk_handle[i] = 0xFFFFu;
            GMAPAPP_Context.num_cis_snk--;
            break;
          }
        }
      }
    }
    else
    {
      APP_ASE_Info_t *p_app_ase;
      p_app_ase = GMAPAPP_GetASE(ASE_ID,ACL_ConnHandle);
      if (p_app_ase != 0)
      {
        LOG_INFO_APP("Audio Data Path is complete with remaining Application delay for audio process to respect : %dus\n",
                     (p_app_ase->presentation_delay - p_app_ase->controller_delay));
      }
    }
  }
  else
  {
    LOG_INFO_APP("No Audio Stream Endpoint with ASE ID %d on ACL connection handle 0x%04X status 0x%02X\n",
                 ASE_ID,
                 ACL_ConnHandle);
    status = BLE_STATUS_INVALID_PARAMS;
  }
  UNUSED(num_chnls);
  return status;
}

static uint8_t APP_GetBitsAudioChnlAllocations(Audio_Chnl_Allocation_t ChnlLocations)
{

  uint8_t bits = 0u;
  uint8_t i;

  for (i = 0; i < (8u*sizeof(Audio_Chnl_Allocation_t)); i++)
  {
    if (ChnlLocations & (1u<<i))
    {
      bits++;
    }
  }
  return bits;
}

static char Hex_To_Char(uint8_t Hex)
{
  if (Hex < 0xA)
  {
    return (char) Hex + 48;
  }
  else
  {
    return (char) Hex + 55;
  }
}

static uint8_t Get_LC3_Conf_ID(uint8_t QoSConfId)
{
  if (QoSConfId < NUM_LC3_QoSConf)
  {
    /* CAP Config */
    return QoSConfId % 16;
  }
  else
  {
    switch (QoSConfId)
    {
      case LC3_QOS_16_1_gs:
        return LC3_16_1;
      case LC3_QOS_16_2_gs:
        return LC3_16_2;
      case LC3_QOS_32_1_gs:
        return LC3_32_1;
      case LC3_QOS_32_2_gs:
        return LC3_32_2;
      case LC3_QOS_48_1_gs:
        return LC3_48_1;
      case LC3_QOS_48_2_gs:
        return LC3_48_2;
      case LC3_QOS_32_1_gr:
        return LC3_32_1;
      case LC3_QOS_32_2_gr:
        return LC3_32_2;
      case LC3_QOS_48_1_gr:
        return LC3_48_1;
      case LC3_QOS_48_2_gr:
        return LC3_48_2;
      case LC3_QOS_48_3_gr:
        return LC3_48_3;
      case LC3_QOS_48_4_gr:
        return LC3_48_4;
      default:
        /* Should not happen */
        return 0;
    }
  }
}

static uint8_t Get_LC3_Broadcast_Conf_ID(uint8_t QoSConfId)
{
  if (QoSConfId < NUM_LC3_QoSConf)
  {
    /* CAP Config */
    return QoSConfId % 16;
  }
  else
  {
    switch (QoSConfId)
    {
      case LC3_QOS_48_1_g:
        return LC3_48_1;
      case LC3_QOS_48_2_g:
        return LC3_48_2;
      case LC3_QOS_48_3_g:
        return LC3_48_3;
      case LC3_QOS_48_4_g:
        return LC3_48_4;
      default:
        /* Should not happen */
        return 0;
    }
  }
}

static Sampling_Freq_t GMAPAPP_GetTargetFrequency(uint8_t Frequency, Audio_Role_t role, BAP_ASE_Info_t *p_ase)
{
  uint8_t i;
  uint8_t j;
  Sampling_Freq_t target_frequency = Frequency;
  APP_ASE_Info_t *p_app_ase2;
  BAP_ASE_Info_t *p_ase2;
  GMAPAPP_Context.SinkDecimation = 1;
  GMAPAPP_Context.SourceDecimation = 1;
  /* Check if other ASEs are configured with different sample rates */
  for (j = 0; j < CFG_BLE_NUM_LINK; j++)
  {
    if (GMAPAPP_Context.ACL_Conn[j].Acl_Conn_Handle != 0xFFFF)
    {
      for (i = 0; i < MAX_NUM_UCL_SNK_ASE_PER_LINK + MAX_NUM_UCL_SRC_ASE_PER_LINK; i++)
      {
        if (i < MAX_NUM_UCL_SNK_ASE_PER_LINK)
        {
          p_app_ase2 = &GMAPAPP_Context.ACL_Conn[j].pASEs->aSnkASE[i];
        }
        else
        {
          p_app_ase2 = &GMAPAPP_Context.ACL_Conn[j].pASEs->aSrcASE[i-MAX_NUM_UCL_SNK_ASE_PER_LINK];
        }

        if (p_app_ase2->ID != p_ase->ASE_ID
            && p_app_ase2->type != p_ase->Type
            && p_app_ase2->allocated == 1u)
        {
          /* Found another ASE allocated */
          p_ase2 = CAP_Unicast_GetASEInfo(GMAPAPP_Context.ACL_Conn[j].Acl_Conn_Handle,
                                          GMAPAPP_Context.ACL_Conn[j].pASEs->aSnkASE[i].ID);
          if (p_ase2->State == ASE_STATE_ENABLING)
          {
            Sampling_Freq_t frequency2 = LTV_GetConfiguredSamplingFrequency(p_ase2->params.Codec.CodecConf.pSpecificConf,
                                                                            p_ase2->params.Codec.CodecConf.SpecificConfLength);
            if (target_frequency <= SAMPLE_FREQ_48000_HZ && frequency2 <= SAMPLE_FREQ_48000_HZ)
            {
              LOG_INFO_APP("Target Frequency = %d, Frequency2 = %d\n", target_frequency, frequency2);
              /* Retrieve frequency and decimation of combined frequencies */
              if (role == AUDIO_ROLE_SINK)
              {
                GMAPAPP_Context.SinkDecimation = APP_CombinedFrequencyTable[target_frequency][frequency2].Multiplier1;
                GMAPAPP_Context.SourceDecimation = APP_CombinedFrequencyTable[target_frequency][frequency2].Multiplier2;
              }
              else
              {
                GMAPAPP_Context.SourceDecimation = APP_CombinedFrequencyTable[target_frequency][frequency2].Multiplier1;
                GMAPAPP_Context.SinkDecimation = APP_CombinedFrequencyTable[target_frequency][frequency2].Multiplier2;
              }
              target_frequency = APP_CombinedFrequencyTable[target_frequency][frequency2].Frequency;
              LOG_INFO_APP("ASE id %d is also starting, using combined frequency %d\n", p_app_ase2->ID, target_frequency);
              LOG_INFO_APP("Source Decimation Multipler = %d, Sink Decimation Multiplier = %d\n",
                           GMAPAPP_Context.SourceDecimation, GMAPAPP_Context.SinkDecimation);
              return target_frequency;
            }
          }
        }
      }
    }
  }
  return target_frequency;
}

static void GMAPAPP_SetupBASE(uint8_t QoSConfID)
{
  uint8_t i;
  uint8_t codec_conf_id = 0;

  codec_conf_id = Get_LC3_Broadcast_Conf_ID(QoSConfID);

  /* Init Broadcast Source Configuration */
  GMAPAPP_Context.audio_driver_config = AUDIO_DRIVER_CONFIG_LINEIN;

  GMAPAPP_Context.BaseBIS[0].BIS_Index = 0x01;
  GMAPAPP_Context.BaseBIS[0].CodecSpecificConfLength = 0x06;
  GMAPAPP_Context.BaseBIS[0].pCodecSpecificConf = &(GMAPAPP_Context.BISCodecSpecificConfig[0][0]);

  GMAPAPP_Context.BaseBIS[1].BIS_Index = 0x02;
  GMAPAPP_Context.BaseBIS[1].CodecSpecificConfLength = 0x06;
  GMAPAPP_Context.BaseBIS[1].pCodecSpecificConf = &(GMAPAPP_Context.BISCodecSpecificConfig[1][0]);

  GMAPAPP_Context.BaseSubgroups[0].NumBISes = BROADCAST_SOURCE_NUM_BIS;
  GMAPAPP_Context.BaseSubgroups[0].pBIS = &(GMAPAPP_Context.BaseBIS[0]);
  GMAPAPP_Context.BaseSubgroups[0].CodecID = 0x0000000006; /* LC3 */
  GMAPAPP_Context.BaseSubgroups[0].CodecSpecificConfLength = 0x0D;
  GMAPAPP_Context.BaseSubgroups[0].pCodecSpecificConf = &(GMAPAPP_Context.SubgroupCodecSpecificConfig[0][0]);
  GMAPAPP_Context.SubgroupMetadata[0][0] = 0x03;
  GMAPAPP_Context.SubgroupMetadata[0][1] = METADATA_STREAMING_AUDIO_CONTEXTS;
  GMAPAPP_Context.SubgroupMetadata[0][2] = BROADCAST_STREAMING_AUDIO_CONTEXT & 0xFF;
  GMAPAPP_Context.SubgroupMetadata[0][3] = (BROADCAST_STREAMING_AUDIO_CONTEXT >> 8) & 0xFF;
  GMAPAPP_Context.BaseSubgroups[0].pMetadata = &GMAPAPP_Context.SubgroupMetadata[0][0];
  GMAPAPP_Context.BaseSubgroups[0].MetadataLength = 4;

  GMAPAPP_Context.BaseGroup.NumSubgroups = 1;
  GMAPAPP_Context.BaseGroup.pSubgroups = &(GMAPAPP_Context.BaseSubgroups[0]);

  uint8_t subgroup_i = 0;

  GMAPAPP_Context.BaseGroup.pSubgroups[0].CodecSpecificConfLength = 0;
  GMAPAPP_Context.SubgroupCodecSpecificConfig[0][subgroup_i++] = 0x02;
  GMAPAPP_Context.SubgroupCodecSpecificConfig[0][subgroup_i++] = CODEC_SAMPLING_FREQ;
  GMAPAPP_Context.SubgroupCodecSpecificConfig[0][subgroup_i++] = APP_CodecConf[codec_conf_id].freq;
  GMAPAPP_Context.SubgroupCodecSpecificConfig[0][subgroup_i++] = 0x02;
  GMAPAPP_Context.SubgroupCodecSpecificConfig[0][subgroup_i++] = CODEC_FRAME_DURATION;
  GMAPAPP_Context.SubgroupCodecSpecificConfig[0][subgroup_i++] = APP_CodecConf[codec_conf_id].frame_duration;
  GMAPAPP_Context.SubgroupCodecSpecificConfig[0][subgroup_i++] = 0x03;
  GMAPAPP_Context.SubgroupCodecSpecificConfig[0][subgroup_i++] = CODEC_OCTETS_PER_CODEC_FRAME;
  GMAPAPP_Context.SubgroupCodecSpecificConfig[0][subgroup_i++] = (uint8_t) APP_CodecConf[codec_conf_id].octets_per_codec_frame;
  GMAPAPP_Context.SubgroupCodecSpecificConfig[0][subgroup_i++] = (uint8_t) (APP_CodecConf[codec_conf_id].octets_per_codec_frame >> 8);

  GMAPAPP_Context.BaseGroup.pSubgroups[0].CodecSpecificConfLength += 10;

  GMAPAPP_Context.SubgroupCodecSpecificConfig[0][subgroup_i++] = 0x02;
  GMAPAPP_Context.SubgroupCodecSpecificConfig[0][subgroup_i++] = CODEC_FRAMES_BLOCKS_PER_SDU;
  GMAPAPP_Context.SubgroupCodecSpecificConfig[0][subgroup_i++] = BROADCAST_SOURCE_FRAME_BLOCK_PER_SDU;

  GMAPAPP_Context.BaseGroup.pSubgroups[0].CodecSpecificConfLength += 3;


  for (i = 0; i < BROADCAST_SOURCE_NUM_BIS; i++)
  {
    uint8_t bis_i = 0;
    /* Take max RTN and transport latency */
    GMAPAPP_Context.RTN = MAX(GMAPAPP_Context.RTN, APP_BroadcastQoSConf[QoSConfID].rtx_num);
    GMAPAPP_Context.MaxTransportLatency = MAX( GMAPAPP_Context.MaxTransportLatency,
                                               APP_BroadcastQoSConf[QoSConfID].max_tp_latency);
    GMAPAPP_Context.BaseGroup.PresentationDelay = MAX(GMAPAPP_Context.BaseGroup.PresentationDelay,
                                                      APP_BroadcastQoSConf[QoSConfID].presentation_delay);

    GMAPAPP_Context.BaseGroup.pSubgroups[0].pBIS[i].CodecSpecificConfLength = 0;

    GMAPAPP_Context.BISCodecSpecificConfig[i][bis_i++] = 0x05;
    GMAPAPP_Context.BISCodecSpecificConfig[i][bis_i++] = CODEC_AUDIO_CHNL_ALLOCATION;
    if (i == 0)
    {
      GMAPAPP_Context.BISCodecSpecificConfig[i][bis_i++] = BROADCAST_SOURCE_CHANNEL_ALLOC_1 & 0xFF;
      GMAPAPP_Context.BISCodecSpecificConfig[i][bis_i++] = (BROADCAST_SOURCE_CHANNEL_ALLOC_1 >> 8) & 0xFF;
      GMAPAPP_Context.BISCodecSpecificConfig[i][bis_i++] = (BROADCAST_SOURCE_CHANNEL_ALLOC_1 >> 16) & 0xFF;
      GMAPAPP_Context.BISCodecSpecificConfig[i][bis_i++] = (BROADCAST_SOURCE_CHANNEL_ALLOC_1 >> 24) & 0xFF;
    }
    else
    {
      GMAPAPP_Context.BISCodecSpecificConfig[i][bis_i++] = BROADCAST_SOURCE_CHANNEL_ALLOC_2 & 0xFF;
      GMAPAPP_Context.BISCodecSpecificConfig[i][bis_i++] = (BROADCAST_SOURCE_CHANNEL_ALLOC_2 >> 8) & 0xFF;
      GMAPAPP_Context.BISCodecSpecificConfig[i][bis_i++] = (BROADCAST_SOURCE_CHANNEL_ALLOC_2 >> 16) & 0xFF;
      GMAPAPP_Context.BISCodecSpecificConfig[i][bis_i++] = (BROADCAST_SOURCE_CHANNEL_ALLOC_2 >> 24) & 0xFF;
    }

    GMAPAPP_Context.BaseGroup.pSubgroups[0].pBIS[i].CodecSpecificConfLength += 6;
  }
}

static tBleStatus GMAPAPP_BroadcastSetupAudio(void)
{
  Sampling_Freq_t sampling_freq;
  Frame_Duration_t frame_duration;
  uint8_t direction = DATA_PATH_INPUT;
  uint32_t controller_delay;
  uint32_t controller_delay_min = 0;
  uint32_t controller_delay_max = 0;
  uint8_t a_codec_id[5] = {0x00,0x00,0x00,0x00,0x00};
  tBleStatus ret;

  a_codec_id[0] = AUDIO_CODING_FORMAT_LC3;

  sampling_freq = LTV_GetConfiguredSamplingFrequency(
      &GMAPAPP_Context.BaseGroup.pSubgroups[0].pCodecSpecificConf[0],
      GMAPAPP_Context.BaseGroup.pSubgroups[0].CodecSpecificConfLength);

  frame_duration = LTV_GetConfiguredFrameDuration(
      &GMAPAPP_Context.BaseGroup.pSubgroups[0].pCodecSpecificConf[0],
      GMAPAPP_Context.BaseGroup.pSubgroups[0].CodecSpecificConfLength);

  if ((sampling_freq != 0) && (frame_duration != 0xFF))
  {
    CODEC_RegisterTriggerClbk(1,0,&start_audio_source);

    MX_AudioInit(AUDIO_ROLE_SOURCE,
                 sampling_freq,
                 frame_duration,
                 NULL,
                 (uint8_t *)aSrcBuff,
                 AUDIO_DRIVER_CONFIG_LINEIN);

    /* AUDIO_ROLE_SOURCE */
    direction = DATA_PATH_INPUT;
    CAP_Broadcast_ReadSupportedControllerDelay(GMAPAPP_Context.CurrentNumBIS,
                                               DATA_PATH_INPUT,
                                               a_codec_id,
                                               &controller_delay_min,
                                               &controller_delay_max);

    /* at source we don't have to respect a presentation delay */
    if((controller_delay_min <= BROADCAST_PREFFERED_CONTROLLER_DELAY) \
      && (controller_delay_max > BROADCAST_PREFFERED_CONTROLLER_DELAY))
    {
      controller_delay = BROADCAST_PREFFERED_CONTROLLER_DELAY;
    }
    else if (controller_delay_min > BROADCAST_PREFFERED_CONTROLLER_DELAY)
    {
      controller_delay = controller_delay_min;
    }
    else /* (controller_delay_max < BROADCAST_PREFFERED_CONTROLLER_DELAY) */
    {
      controller_delay = controller_delay_max;
    }
    LOG_INFO_APP("Controller delay chosen to value %d us\n", controller_delay);

    CODEC_DataPathParam_t param;
    /* sample coded on 16bits */
    param.SampleDepth = 16;

    /* SAI/I2C peripheral driver requests to set decimation to 2 (stereo buffer)*/
    param.Decimation = 2;

    ret = CAP_Broadcast_SetupAudioDataPath(GMAPAPP_Context.CurrentNumBIS,
                                           &GMAPAPP_Context.CurrentBISConnHandle[0],
                                           direction,
                                           a_codec_id,
                                           controller_delay,
                                           DATA_PATH_CIRCULAR_BUF,
                                           CONFIGURE_DATA_PATH_CONFIG_LEN,
                                           (const uint8_t*) &param);

    LOG_INFO_APP("CAP_Broadcast_SetupAudioDataPath() returns status 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_APP("Sampling Frequency in LTV is invalid\n");
    ret = BLE_STATUS_FAILED;
  }

  return ret;
}

/*Audio Source */
static int32_t start_audio_source(void)
{
  int32_t status;
  LOG_INFO_APP("START AUDIO SOURCE (input)\n");
  status = Start_RxAudio();

  if (GMAPAPP_Context.num_cis_established > 0)
  {
    char sink_samplerate_text[8];
    char source_samplerate_text[8];

    switch (GMAPAPP_Context.ConfiguredSinkSampleFrequency)
    {
      case SAMPLE_FREQ_8000_HZ:
        snprintf(&sink_samplerate_text[0], 8, "8KHz");
        break;
      case SAMPLE_FREQ_16000_HZ:
        snprintf(&sink_samplerate_text[0], 8, "16KHz");
        break;
      case SAMPLE_FREQ_24000_HZ:
        snprintf(&sink_samplerate_text[0], 8, "24KHz");
        break;
      case SAMPLE_FREQ_32000_HZ:
        snprintf(&sink_samplerate_text[0], 8, "32KHz");
        break;
      case SAMPLE_FREQ_44100_HZ:
        snprintf(&sink_samplerate_text[0], 8, "44.1KHz");
        break;
      case SAMPLE_FREQ_48000_HZ:
        snprintf(&sink_samplerate_text[0], 8, "48KHz");
        break;
      default:
        snprintf(&sink_samplerate_text[0], 8, "??KHz");
        break;
    }

    switch (GMAPAPP_Context.ConfiguredSourceSampleFrequency)
    {
      case SAMPLE_FREQ_8000_HZ:
        snprintf(&source_samplerate_text[0], 8, "8KHz");
        break;
      case SAMPLE_FREQ_16000_HZ:
        snprintf(&source_samplerate_text[0], 8, "16KHz");
        break;
      case SAMPLE_FREQ_24000_HZ:
        snprintf(&source_samplerate_text[0], 8, "24KHz");
        break;
      case SAMPLE_FREQ_32000_HZ:
        snprintf(&source_samplerate_text[0], 8, "32KHz");
        break;
      case SAMPLE_FREQ_44100_HZ:
        snprintf(&source_samplerate_text[0], 8, "44.1KHz");
        break;
      case SAMPLE_FREQ_48000_HZ:
        snprintf(&source_samplerate_text[0], 8, "48KHz");
        break;
      default:
        snprintf(&source_samplerate_text[0], 8, "??KHz");
        break;
    }

    Menu_SetStreamingPage(sink_samplerate_text, source_samplerate_text, GMAPAPP_Context.audio_role_setup);
  }
  return status;
}


/*Audio Sink */
static int32_t start_audio_sink(void)
{
  int32_t status;
  /* reset numbers of active channels */
  Nb_Active_Ch = 0;

  LOG_INFO_APP("START AUDIO SINK (output)\n");
  status = Start_TxAudio();

  if (GMAPAPP_Context.num_cis_established > 0)
  {
    char sink_samplerate_text[8];
    char source_samplerate_text[8];

    switch (GMAPAPP_Context.ConfiguredSinkSampleFrequency)
    {
      case SAMPLE_FREQ_8000_HZ:
        snprintf(&sink_samplerate_text[0], 8, "8KHz");
        break;
      case SAMPLE_FREQ_16000_HZ:
        snprintf(&sink_samplerate_text[0], 8, "16KHz");
        break;
      case SAMPLE_FREQ_24000_HZ:
        snprintf(&sink_samplerate_text[0], 8, "24KHz");
        break;
      case SAMPLE_FREQ_32000_HZ:
        snprintf(&sink_samplerate_text[0], 8, "32KHz");
        break;
      case SAMPLE_FREQ_44100_HZ:
        snprintf(&sink_samplerate_text[0], 8, "44.1KHz");
        break;
      case SAMPLE_FREQ_48000_HZ:
        snprintf(&sink_samplerate_text[0], 8, "48KHz");
        break;
      default:
        snprintf(&sink_samplerate_text[0], 8, "??KHz");
        break;
    }

    switch (GMAPAPP_Context.ConfiguredSourceSampleFrequency)
    {
      case SAMPLE_FREQ_8000_HZ:
        snprintf(&source_samplerate_text[0], 8, "8KHz");
        break;
      case SAMPLE_FREQ_16000_HZ:
        snprintf(&source_samplerate_text[0], 8, "16KHz");
        break;
      case SAMPLE_FREQ_24000_HZ:
        snprintf(&source_samplerate_text[0], 8, "24KHz");
        break;
      case SAMPLE_FREQ_32000_HZ:
        snprintf(&source_samplerate_text[0], 8, "32KHz");
        break;
      case SAMPLE_FREQ_44100_HZ:
        snprintf(&source_samplerate_text[0], 8, "44.1KHz");
        break;
      case SAMPLE_FREQ_48000_HZ:
        snprintf(&source_samplerate_text[0], 8, "48KHz");
        break;
      default:
        snprintf(&source_samplerate_text[0], 8, "??KHz");
        break;
    }

    Menu_SetStreamingPage(sink_samplerate_text, source_samplerate_text, GMAPAPP_Context.audio_role_setup);
  }
  return status;
}

#if (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u)
static void VCP_MetaEvt_Notification(VCP_Notification_Evt_t *pNotification)
{
  switch(pNotification->EvtOpcode)
  {
    case VCP_CONTROLLER_UPDATED_VOLUME_STATE_EVT:
    {
      VCP_VolumeState_Evt_t *p_info = (VCP_VolumeState_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Updated Volume State :\n");
      LOG_INFO_APP("     Volume Setting : %d\n",p_info->VolSetting);
      LOG_INFO_APP("     Mute : %d\n",p_info->Mute);
      LOG_INFO_APP("     Change Counter : %d\n",p_info->ChangeCounter);

      RemoteVolume = p_info->VolSetting;
      RemoteMute = p_info->Mute;
      if (RemoteMute == 0)
      {
        Menu_SetRemoteVolume(p_info->VolSetting);
      }
      else
      {
        Menu_SetRemoteVolume(0);
      }
    }
    default:
      break;
  }
}
#endif /* (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u) */

#if (APP_MICP_ROLE_CONTROLLER_SUPPORT == 1u)
static void MICP_MetaEvt_Notification(MICP_Notification_Evt_t *pNotification)
{
  switch(pNotification->EvtOpcode)
  {
    case MICP_CONTROLLER_MUTE_EVT:
    {
      LOG_INFO_APP("MICP Mute value %d on ConnHandle 0x%04X\n", pNotification->pInfo[0u], pNotification->ConnHandle);
      RemoteMicMute = pNotification->pInfo[0u];
      Menu_SetRemoteMicMute(RemoteMicMute);
      break;
    }
    case MICP_CONTROLLER_OPERATION_COMPLETE_EVT:
    {
      MICP_CLT_OpComplete_Evt_t *p_info = (MICP_CLT_OpComplete_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("MICP Controller operation 0x%02X with connection handle 0x%04X on Instance %d is complete with status 0x%02X\n",
                  p_info->Op,
                  p_info->Inst,
                  pNotification->ConnHandle,
                  pNotification->Status);
      UNUSED(p_info);
      break;
    }
    case MICP_CONTROLLER_CTRL_OPERATION_TRANSMITTED_EVT:
    {
      MICP_CLT_CtrlOpTransmitted_Evt_t *p_info = (MICP_CLT_CtrlOpTransmitted_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("MICP Controller Ctrl operation 0x%02X  with connection handle 0x%04X on Instance %d is transmitted with status 0x%02X\n",
                  p_info->CtrlOp,
                  p_info->Inst,
                  pNotification->ConnHandle,
                  pNotification->Status);
      UNUSED(p_info);
      break;
    }
    case MICP_CONTROLLER_UPDATED_MUTE_EVT:
    {
      LOG_INFO_APP("New MICP Mute value %d on ConnHandle 0x%04X\n", pNotification->pInfo[0u], pNotification->ConnHandle);
      RemoteMicMute = pNotification->pInfo[0u];
      Menu_SetRemoteMicMute(RemoteMicMute);
      break;
    }
    case MICP_CONTROLLER_AUDIO_INPUT_STATE_EVT:
    {
      MICP_AudioInputState_Evt_t *p_info = (MICP_AudioInputState_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("[AIC %d] Updated Audio Input State :\n",p_info->AICInst);
      LOG_INFO_APP("     Gain Setting : %d\n",p_info->State.GainSetting);
      LOG_INFO_APP("     Mute : %d\n",p_info->State.Mute);
      LOG_INFO_APP("     Gain Mode : %d\n",p_info->State.GainMode);
      LOG_INFO_APP("     Change Counter : %d\n",p_info->ChangeCounter);
      UNUSED(p_info);
      break;
    }
    case MICP_CONTROLLER_GAIN_SETTING_PROP_EVT:
    {
      MICP_GainSettingProp_Evt_t *p_info = (MICP_GainSettingProp_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("[AIC %d] Updated Gain Setting Properties :\n",p_info->AICInst);
      LOG_INFO_APP("     Gain Setting Units: %d dB \n",(p_info->Prop.GainSettingUnits * 0.1));
      LOG_INFO_APP("     Gain Setting Min : %d\n",p_info->Prop.GainSettingMin);
      LOG_INFO_APP("     Gain Setting Max : %d\n",p_info->Prop.GainSettingMax);
      UNUSED(p_info);
      break;
    }
    case MICP_CONTROLLER_AUDIO_INPUT_STATUS_EVT:
    {
      MICP_AudioInputStatus_Evt_t *p_info = (MICP_AudioInputStatus_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("[AIC %d] Updated Audio Input Status :\n",p_info->AICInst);
      LOG_INFO_APP("     Status: %d \n",p_info->Status);
      UNUSED(p_info);
      break;
    }
    case MICP_CONTROLLER_AUDIO_INPUT_DESCRIPTION_EVT:
    {
      MICP_AudioDescription_Evt_t *p_info = (MICP_AudioDescription_Evt_t *)pNotification->pInfo;
      uint8_t i;
      LOG_INFO_APP("[AIC %d] Updated Audio Input Description :\n",p_info->Inst);
      for (i = 0; i < p_info->DataLength;i++)
      {
        LOG_INFO_APP("%c",p_info->pData[i]);
      }
      LOG_INFO_APP("\n");
      UNUSED(p_info);
      UNUSED(i);
      break;
    }
    default:
      break;
  }
}
#endif /* (APP_MICP_ROLE_CONTROLLER_SUPPORT == 1u) */

/* USER CODE BEGIN FD */

/* USER CODE END FD */

