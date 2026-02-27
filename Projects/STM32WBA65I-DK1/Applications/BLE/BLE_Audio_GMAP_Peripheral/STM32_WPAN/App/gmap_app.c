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
#include "ble_core.h"
#include "ble_audio_stack.h"
#include "stm32_seq.h"
#include "stm32_timer.h"
#include "codec_mngr.h"
#include "ltv_utils.h"
#include "cap.h"
#include "gmap.h"
#include "usecase_dev_mgmt.h"
#include "app_menu_cfg.h"
#include "log_module.h"
#include "app_ble.h"
#include "micp.h"
#include "simple_nvm_arbiter.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  Sampling_Freq_t Frequency;
  uint8_t         Multiplier1;
  uint8_t         Multiplier2;
} CombinedFreqParameters;

/* Private defines -----------------------------------------------------------*/
/*BAP mandatory supported delay definition*/
#define BAP_MANDATORY_PRESENTATION_DELAY        (40000u)

/*Advertising Settings*/
#define FAST_ADV_INTERVAL_MIN                   (0x0080)
#define FAST_ADV_INTERVAL_MAX                   (0x00A0)
#define SLOW_ADV_INTERVAL_MIN                   (0x0640)
#define SLOW_ADV_INTERVAL_MAX                   (0x0FA0)
#define ADV_TYPE                                ADV_IND
#define ADV_FILTER                              NO_WHITE_LIST_USE

/* Audio chain memory sizing: must be aligned with PAC (frame len) and ASEs (channels nb)
 * These macro are generic and could be overwritten by the user for a fine tuning
 */

/* Memory pool used by the codec manager to manage audio latencies
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

#if ((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)
#define SCAN_INTERVAL                   (0x40) /* Scan Interval (*0.625ms): 40ms */
#define SCAN_WINDOW                     (0x20) /* Scan Window (*0.625ms): 20ms */
#define PA_EVENT_SKIP                   (0u)
#define PA_SYNC_TIMEOUT                 (0x03E8)
#define BAP_BROADCAST_ENCRYPTION        (0u)
#define BIG_HANDLE                      (0u)
#define BIG_MSE                         (0u)
#define BIG_SYNC_TIMEOUT                (0x0190)
#endif /*(APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)*/

#define BLE_AUDIO_DYN_ALLOC_SIZE        (BLE_AUDIO_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK, CFG_BLE_EATT_BEARER_PER_LINK))

/*Memory size required for CAP*/
#define CAP_DYN_ALLOC_SIZE \
        CAP_MEM_TOTAL_BUFFER_SIZE(APP_CAP_ROLE, CFG_BLE_NUM_LINK, \
                                  0u, 0u, \
                                  0u, 0u, \
                                  0u, 0u, \
                                  0u, 0u, \
                                  APP_NUM_SNK_ASE, APP_NUM_SRC_ASE, \
                                  0u, 0u, 0u, \
                                  0u, 0u)

/*Memory size required to allocate resource for Published Audio Capabilities Server Context*/
#define BAP_PACS_SRV_DYN_ALLOC_SIZE \
        BAP_PACS_SRV_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK,APP_NUM_SNK_PAC_RECORDS,APP_NUM_SRC_PAC_RECORDS)

/*Memory size required to allocate resource for Audio Stream Endpoint in Unicats Server Context*/
#define BAP_USR_ASE_BLOCKS_SIZE \
        BAP_MEM_BLOCKS_ASE_PER_CONN_SIZE_BYTES(APP_NUM_SNK_ASE, \
                                               APP_NUM_SRC_ASE, \
                                               MAX_USR_CODEC_CONFIG_SIZE, \
                                               MAX_USR_METADATA_SIZE)
#if ((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)
/* Memory required to allocate resource for Broadcast Audio Scan for Scan Delegator*/
#define BAP_BASS_SRV_DYN_ALLOC_SIZE  \
                BAP_BASS_SRV_TOTAL_BUFFER_SIZE(MAX_NUM_SDE_BSRC_INFO,\
                                              MAX_NUM_BIS_PER_BIG,\
                                              MAX_BASS_CODEC_CONFIG_SIZE,\
                                              MAX_BASS_METADATA_SIZE,\
                                              MAX_NUM_BASS_BASE_SUBGROUPS)
#endif /*(APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)*/

/*Memory size required for Audio Stream management in Unicast Server Context*/
#define BAP_ASCS_SRV_DYN_ALLOC_SIZE \
        BAP_ASCS_SRV_TOTAL_BUFFER_SIZE(MAX_NUM_USR_LINK, \
                                       APP_NUM_SNK_ASE, \
                                       APP_NUM_SRC_ASE, \
                                       BAP_USR_ASE_BLOCKS_SIZE)

/* Memory size required to allocate resource for CIG*/
#define BAP_ISO_CHNL_DYN_ALLOC_SIZE  \
        BAP_MEM_BLOCKS_CIG_SIZE_BYTES(MAX_NUM_CIG, MAX_NUM_CIS_PER_CIG)

/* Memory size required to allocate resource for Non-Volatile Memory Management for BAP Services restoration*/
#define BAP_NVM_MGMT_DYN_ALLOC_SIZE  \
        BAP_NVM_MGMT_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK)

#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)
#define BLE_VCP_RDR_DYN_ALLOC_SIZE\
        BLE_VCP_RENDERER_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK, \
                                           APP_VCP_RDR_NUM_AIC_INSTANCES, \
                                           APP_VCP_RDR_NUM_VOC_INSTANCES)
#endif /*(APP_VCP_ROLE_RENDERER_SUPPORT == 1u)*/

#if (APP_MICP_ROLE_DEVICE_SUPPORT == 1)
#define BLE_MICP_DEV_DYN_ALLOC_SIZE      BLE_MICP_DEVICE_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK,\
                                                                          APP_MICP_DEV_NUM_AIC_INSTANCES)
#endif /* (APP_MICP_ROLE_DEVICE_SUPPORT == 1) */

#if (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1)
#define BLE_CSIP_SET_MEMBER_DYN_ALLOC_SIZE      BLE_CSIP_SET_MEMBER_TOTAL_BUFFER_SIZE(APP_CSIP_SET_MEMBER_NUM_INSTANCES, \
                                                                                      CFG_BLE_NUM_LINK)
#endif /* (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1) */

/* Number of 64-bit words in NVM flash area */
#define CFG_BLE_AUDIO_PLAT_NVM_MAX_SIZE         ((BLE_APP_AUDIO_NVM_ALLOC_SIZE/8) + 4u)

#define VOLUME_STEP                             10
#define BASE_VOLUME                             128

/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* Codec Specific Capabilities defined in Basic Audio Profile Specification*/
const APP_CodecCap_t APP_CodecSpecificCap[NUM_LC3_CODEC_CAP] =    \
                  {{SUPPORTED_SAMPLE_FREQ_8000_HZ,SUPPORTED_FRAME_DURATION_7_5_MS,26,26}, \
                  {SUPPORTED_SAMPLE_FREQ_8000_HZ,SUPPORTED_FRAME_DURATION_10_MS,30,30}, \
                  {SUPPORTED_SAMPLE_FREQ_16000_HZ,SUPPORTED_FRAME_DURATION_7_5_MS,30,30}, \
                  {SUPPORTED_SAMPLE_FREQ_16000_HZ,SUPPORTED_FRAME_DURATION_10_MS,40,40}, \
                  {SUPPORTED_SAMPLE_FREQ_24000_HZ,SUPPORTED_FRAME_DURATION_7_5_MS,45,45}, \
                  {SUPPORTED_SAMPLE_FREQ_24000_HZ,SUPPORTED_FRAME_DURATION_10_MS,60,60}, \
                  {SUPPORTED_SAMPLE_FREQ_32000_HZ,SUPPORTED_FRAME_DURATION_7_5_MS,60,60}, \
                  {SUPPORTED_SAMPLE_FREQ_32000_HZ,SUPPORTED_FRAME_DURATION_10_MS,80,80}, \
                  {SUPPORTED_SAMPLE_FREQ_44100_HZ,SUPPORTED_FRAME_DURATION_7_5_MS,97,97}, \
                  {SUPPORTED_SAMPLE_FREQ_44100_HZ,SUPPORTED_FRAME_DURATION_10_MS,130,130}, \
                  {SUPPORTED_SAMPLE_FREQ_48000_HZ,SUPPORTED_FRAME_DURATION_7_5_MS,75,75}, \
                  {SUPPORTED_SAMPLE_FREQ_48000_HZ,SUPPORTED_FRAME_DURATION_10_MS,100,100}, \
                  {SUPPORTED_SAMPLE_FREQ_48000_HZ,SUPPORTED_FRAME_DURATION_7_5_MS,90,90}, \
                  {SUPPORTED_SAMPLE_FREQ_48000_HZ,SUPPORTED_FRAME_DURATION_10_MS,120,120}, \
                  {SUPPORTED_SAMPLE_FREQ_48000_HZ,SUPPORTED_FRAME_DURATION_7_5_MS,117,117}, \
                  {SUPPORTED_SAMPLE_FREQ_48000_HZ,SUPPORTED_FRAME_DURATION_10_MS,155,155}};

/* QoS Configuration settings defined in Basic Audio Profile Specification for low latency and for high-reliability audio data*/
const APP_QoSConf_t APP_QoSConf[NUM_LC3_QoSConf + NUM_GMAP_LC3_QoSConf] =    \
                  {{SAMPLE_FREQ_8000_HZ,7500,BAP_FRAMING_UNFRAMED,26,2,8,40000}, \
                  {SAMPLE_FREQ_8000_HZ,10000,BAP_FRAMING_UNFRAMED,30,2,10,40000}, \
                  {SAMPLE_FREQ_16000_HZ,7500,BAP_FRAMING_UNFRAMED,30,2,8,40000}, \
                  {SAMPLE_FREQ_16000_HZ,10000,BAP_FRAMING_UNFRAMED,40,2,10,40000}, \
                  {SAMPLE_FREQ_24000_HZ,7500,BAP_FRAMING_UNFRAMED,45,2,8,40000}, \
                  {SAMPLE_FREQ_24000_HZ,10000,BAP_FRAMING_UNFRAMED,60,2,10,40000}, \
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

/* Unicast Audio Configuration defined in Basic Audio Profile Specification */
const APP_UnicastAudioConf_t APP_UnicastAudioConf[NUM_USR_AUDIO_CONF] =    \
                  {{1,0,SUPPORTED_AUDIO_CHNL_COUNT_1,0,0,0,1,1}, \
                  {0,1,0,0,SUPPORTED_AUDIO_CHNL_COUNT_1,0,1,1}, \
                  {1,1,SUPPORTED_AUDIO_CHNL_COUNT_1,0,SUPPORTED_AUDIO_CHNL_COUNT_1,0,1,2}, \
                  {1,0,SUPPORTED_AUDIO_CHNL_COUNT_2,2,0,0,1,1}, \
                  {1,1,SUPPORTED_AUDIO_CHNL_COUNT_2,2,SUPPORTED_AUDIO_CHNL_COUNT_1,0,1,2}, \
                  {2,0,SUPPORTED_AUDIO_CHNL_COUNT_1,2,0,0,2,2}, \
                  {1,0,SUPPORTED_AUDIO_CHNL_COUNT_1,1,0,0,1,1}, \
                  {1,1,SUPPORTED_AUDIO_CHNL_COUNT_1,0,SUPPORTED_AUDIO_CHNL_COUNT_1,0,2,2}, \
                  {1,0,SUPPORTED_AUDIO_CHNL_COUNT_1,0,0,0,1,1}, \
                  {2,1,SUPPORTED_AUDIO_CHNL_COUNT_1,2,SUPPORTED_AUDIO_CHNL_COUNT_1,0,2,3}, \
                  {1,1,SUPPORTED_AUDIO_CHNL_COUNT_1,1,SUPPORTED_AUDIO_CHNL_COUNT_1,1,1,2}, \
                  {0,2,0,0,SUPPORTED_AUDIO_CHNL_COUNT_1,2,2,2}, \
                  {0,1,0,0,SUPPORTED_AUDIO_CHNL_COUNT_1,1,1,1}, \
                  {0,1,0,0,SUPPORTED_AUDIO_CHNL_COUNT_2,2,1,1}, \
                  {2,2,SUPPORTED_AUDIO_CHNL_COUNT_1,2,SUPPORTED_AUDIO_CHNL_COUNT_1,2,2,4}, \
                  {1,1,SUPPORTED_AUDIO_CHNL_COUNT_1,1,SUPPORTED_AUDIO_CHNL_COUNT_1,1,1,2}};

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
static uint32_t aPACSSrvMemBuffer[DIVC(BAP_PACS_SRV_DYN_ALLOC_SIZE,4)];
#if ((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)
static uint32_t aBASSSrvMemBuffer[DIVC(BAP_BASS_SRV_DYN_ALLOC_SIZE,4)];
static uint32_t aBASEMemBuffer[DIVC(BAP_BASE_TOTAL_BUFFER_SIZE,4)];
#endif /*((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)*/
static uint32_t aASCSSrvMemBuffer[DIVC(BAP_ASCS_SRV_DYN_ALLOC_SIZE,4)];
static uint32_t aISOChnlMemBuffer[DIVC(BAP_ISO_CHNL_DYN_ALLOC_SIZE,4)];
static uint32_t aNvmMgmtMemBuffer[DIVC(BAP_NVM_MGMT_DYN_ALLOC_SIZE,4)];
static uint32_t audio_init_buffer[BLE_AUDIO_DYN_ALLOC_SIZE];
static uint64_t audio_buffer_nvm[CFG_BLE_AUDIO_PLAT_NVM_MAX_SIZE] = {0};
static BleAudioInit_t pBleAudioInit;
#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)
static uint32_t aRenderMemBuffer[DIVC(BLE_VCP_RDR_DYN_ALLOC_SIZE,4)];
#endif /*(APP_VCP_ROLE_RENDERER_SUPPORT == 1u)*/
#if (APP_MICP_ROLE_DEVICE_SUPPORT == 1)
static uint32_t aMicDevMemBuffer[DIVC(BLE_MICP_DEV_DYN_ALLOC_SIZE,4)];
#endif /*(APP_MICP_ROLE_DEVICE_SUPPORT == 1u)*/
#if (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1u)
static uint32_t aSetMemberMemBuffer[DIVC(BLE_CSIP_SET_MEMBER_DYN_ALLOC_SIZE,4)];
#endif /*(APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1u)*/

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

static uint16_t* pPrevData = NULL;
static uint8_t Nb_Active_Ch = 0;

#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)
#if (APP_VCP_RDR_NUM_AIC_INSTANCES > 0)
static VCP_AIC_InitInst_t aAICInst[APP_VCP_RDR_NUM_AIC_INSTANCES];
#endif /*(APP_VCP_RDR_NUM_AIC_INSTANCES > 0)*/
#if (APP_VCP_RDR_NUM_VOC_INSTANCES > 0)
static VCP_VOC_InitInst_t aVOCInst[APP_VCP_RDR_NUM_VOC_INSTANCES];
#endif /*(APP_VCP_RDR_NUM_VOC_INSTANCES > 0)*/
uint8_t Volume = BASE_VOLUME;
uint8_t Mute = 0x00;
#endif /*(APP_VCP_ROLE_RENDERER_SUPPORT == 1u)*/

#if (APP_MICP_ROLE_DEVICE_SUPPORT == 1)
#if (APP_MICP_DEV_NUM_AIC_INSTANCES > 0u)
static MICP_AIC_InitInst_t aMicAICInst[APP_MICP_DEV_NUM_AIC_INSTANCES];
#endif /* (APP_MICP_DEV_NUM_AIC_INSTANCES > 0u) */
uint8_t MicMute = 0x00;
#endif /*(APP_MICP_ROLE_DEVICE_SUPPORT == 1u)*/

#if ((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)
#if (BAP_BROADCAST_ENCRYPTION == 1)
uint32_t aAPP_BroadcastCode[4u] = {0x00000001, 0x00000002, 0x00000003, 0x00000004};
#else /*(BAP_BROADCAST_ENCRYPTION == 1)*/
uint32_t aAPP_BroadcastCode[4u] = {0x00000000, 0x00000000, 0x00000000, 0x00000000};
#endif /*(BAP_BROADCAST_ENCRYPTION == 1)*/
#endif /*((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)*/

extern uint32_t Source_frame_size;
extern uint32_t Sink_frame_size;

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
static tBleStatus CAPAPP_Init(Audio_Role_t AudioRole, uint8_t csip_config_id);
static tBleStatus GMAPAPP_GMAPInit(GMAP_Role_t Role);
static void GMAPAPP_CAPNotification(CAP_Notification_Evt_t *pNotification);
static APP_ACL_Conn_t *APP_GetACLConn(uint16_t ConnHandle);
static APP_ACL_Conn_t *APP_AllocateACLConn(uint16_t ConnHandle);
static APP_ASE_Info_t *APP_GetASE(uint8_t ASE_ID,uint16_t ACL_ConnHandle);
static uint8_t APP_UnicastSetupAudioDataPath(uint16_t ACL_ConnHandle,
                                             uint16_t CIS_ConnHandle,
                                             uint8_t ASE_ID,
                                             uint32_t ControllerDelay);
#if ((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)
static uint8_t APP_BroadcastSetupAudio(Audio_Role_t role);
static uint8_t APP_StartBroadcastAudio(Audio_Role_t role);
#endif /*((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)*/
static int32_t start_audio_source(void);
static int32_t start_audio_sink(void);
static uint8_t APP_GetBitsAudioChnlAllocations(Audio_Chnl_Allocation_t ChnlLocations);
static void APP_ParseMetadataParams(APP_ASE_Info_t *pASE,
                                    uint16_t ConnHandle,
                                    uint8_t const *pMetadata,
                                    uint8_t MetadataLength);
static uint8_t GMAPAPP_BuildAdvDataPacket(uint8_t *pAdvData,
                                         uint8_t *AdvPacketLen,
                                         CAP_Announcement_t AnnouncementType,
                                         uint8_t EnableSolicitationRequest,
                                         uint16_t Appearance);
static uint16_t APP_UnavailableAudioContexts(ASE_Type_t Type,
                                             Audio_Context_t AvailableAudioContext,
                                             Audio_Context_t AudioContexts);
static uint16_t APP_ReleasedASEStreamingAudioContexts(APP_ACL_Conn_t *pConn,APP_ASE_Info_t *pASE, uint16_t AudioContexts);

#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)
static void VCP_MetaEvt_Notification(VCP_Notification_Evt_t *pNotification);
#endif /*(APP_VCP_ROLE_RENDERER_SUPPORT == 1u)*/

#if (APP_MICP_ROLE_DEVICE_SUPPORT == 1)
static void MICP_MetaEvt_Notification(MICP_Notification_Evt_t *pNotification);
#endif /*(APP_MICP_ROLE_DEVICE_SUPPORT == 1u)*/

static void Print_String(uint8_t *pString, uint8_t StringLen);
static char Hex_To_Char(uint8_t Hex);

/* Exported functions --------------------------------------------------------*/
extern void APP_NotifyToRun(void);

/* Functions Definition ------------------------------------------------------*/
tBleStatus APP_AUDIO_STACK_Init(void)
{
  tBleStatus status;

  /* First register the APP BLE Audio buffer */
  SNVMA_Register(APP_AUDIO_NvmBuffer,
                 (uint32_t *)audio_buffer_nvm,
                 (CFG_BLE_AUDIO_PLAT_NVM_MAX_SIZE * 2u));

  /* Realize a restore */
  SNVMA_Restore (APP_AUDIO_NvmBuffer);

  /* Initialize the Audio IP*/
  pBleAudioInit.NumOfLinks = CFG_BLE_NUM_LINK;
  pBleAudioInit.NumOfEATTBearersPerLink = CFG_BLE_EATT_BEARER_PER_LINK;
  pBleAudioInit.bleAudioStartRamAddress = (uint8_t*)audio_init_buffer;
  pBleAudioInit.total_buffer_size = BLE_AUDIO_DYN_ALLOC_SIZE;
  pBleAudioInit.MaxNumOfBondedDevices = APP_MAX_NUM_BONDED_DEVICES;
  pBleAudioInit.bleAudioStartRamAddress_NVM = audio_buffer_nvm;
  pBleAudioInit.total_buffer_size_NVM = CFG_BLE_AUDIO_PLAT_NVM_MAX_SIZE;
  status = BLE_AUDIO_STACK_Init(&pBleAudioInit);
  LOG_INFO_APP("BLE_AUDIO_STACK_Init() returns status 0x%02X\n",status);
  LOG_INFO_APP("BLE Audio Stack Lib version: %s\n",BLE_AUDIO_STACK_GetFwVersion());

  return status;
}

void GMAPAPP_Init(uint8_t csip_config_id)
{
  tBleStatus status;

#if (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 0u)
  if ( csip_config_id > 0u)
  {
    return HCI_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE_ERR_CODE;
  }
#endif /* (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 0u) */

  /* Set correct appearance in GAP Database */
  if (csip_config_id == 0)
  {
    status = SetGapAppearance(GAP_APPEARANCE_HEADPHONES);
  }
  else
  {
    status = SetGapAppearance(GAP_APPEARANCE_EARBUD);
  }

  /* Init CAP Acceptor with Unicast Server */
  status = CAPAPP_Init(APP_AUDIO_ROLE,csip_config_id);
  LOG_INFO_APP("CAPAPP_Init() returns status 0x%02X\n", status);
  if (status == BLE_STATUS_SUCCESS)
  {
    /* Initialize the GMAP layer*/
    status = GMAPAPP_GMAPInit(APP_GMAP_ROLE);
    LOG_INFO_APP("GMAPAPP_GMAPInit() with role 0x%02X returns status 0x%02X\n", APP_GMAP_ROLE, status);
  }
  UNUSED(status);

  Set_Volume(Volume);
  Menu_SetVolume(Volume);
}

tBleStatus GMAPAPP_Linkup(uint16_t ConnHandle)
{
  tBleStatus status = HCI_COMMAND_DISALLOWED_ERR_CODE;
  APP_ACL_Conn_t *p_conn = APP_GetACLConn(ConnHandle);
  if (p_conn != 0)
  {
    /* GMAP linkup*/
    status = GMAP_Linkup(ConnHandle);
    LOG_INFO_APP("GMAP Linkup on ConnHandle 0x%04X returns status 0x%02X\n",
                ConnHandle,
                status);
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
      if (pNotification->Status == BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("GMAP Role = 0x%02X\n", p_gmap_info->GMAPRole);
        LOG_INFO_APP("UGG Features = 0x%02X\n", p_gmap_info->UGGFeatures);
        LOG_INFO_APP("UGT Features = 0x%02X\n", p_gmap_info->UGTFeatures);
        LOG_INFO_APP("BGS Features = 0x%02X\n", p_gmap_info->BGSFeatures);
        LOG_INFO_APP("BGR Features = 0x%02X\n", p_gmap_info->BGRFeatures);
      }

      p_conn = APP_GetACLConn(pNotification->ConnHandle);
      if (p_conn != 0)
      {
        if (p_conn->ConfirmIndicationRequired == 1u)
        {
          /* Confirm indication now that the GATT is available */
          ret = aci_gatt_confirm_indication(pNotification->ConnHandle);
          if (ret != BLE_STATUS_SUCCESS)
          {
            LOG_INFO_APP("  Fail   : aci_gatt_confirm_indication command, result: 0x%02X\n", ret);
          }
          else
          {
            LOG_INFO_APP("  Success: aci_gatt_confirm_indication command\n");
          }
          p_conn->ConfirmIndicationRequired = 0u;
        }

        if (pNotification->Status == BLE_STATUS_SUCCESS)
        {
          p_conn->AudioProfile |= AUDIO_PROFILE_GMAP;
        }
      }

      UNUSED(ret);
      UNUSED(p_gmap_info);
      break;
    }
  }
}

uint8_t GMAPAPP_StartAdvertising(CAP_Announcement_t AnnouncementType
                                 ,uint8_t EnableSolicitationRequest,
                                 uint16_t Appearance)
{
  tBleStatus status;
  uint8_t advPacketLen;
  uint8_t Primary_Adv_Channel_Map = ADV_CH_37 | ADV_CH_38 | ADV_CH_39;
  uint8_t Adv_TX_Power = 0x00;
  uint8_t Scan_Req_Notification_Enable = 0x00;
  uint8_t Secondary_Adv_Max_Skip = 0x00;
  const uint8_t Peer_Address[BD_ADDR_SIZE_LOCAL] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint8_t Pb_Addr_Len;
  uint8_t Pb_Addr[6] = {0x00};
  uint32_t Primary_Adv_Interval_Min;
  uint32_t Primary_Adv_Interval_Max;

  uint8_t Adv_Data[ADV_LEN] = {(ADV_AD_FLAGS_LEN-1), AD_TYPE_FLAGS, (FLAG_BIT_LE_GENERAL_DISCOVERABLE_MODE|FLAG_BIT_BR_EDR_NOT_SUPPORTED),
                               (ADV_LOCAL_NAME_LEN-1), AD_TYPE_COMPLETE_LOCAL_NAME,
                                 'S','T','M', '3', '2', 'W','B','A', '_', '0', '0', '0', '0'};
#if (CFG_TEST_VALIDATION == 1u)
  Adv_Data[5] = 'G';
  Adv_Data[6] = 'M';
  Adv_Data[7] = 'A';
  Adv_Data[8] = 'P';
  Adv_Data[9] = '_';
#endif /*(CFG_TEST_VALIDATION == 1u)*/
  aci_hal_read_config_data(CONFIG_DATA_PUBADDR_OFFSET, &Pb_Addr_Len, &Pb_Addr[0]);
  Adv_Data[ADV_AD_FLAGS_LEN+ADV_LOCAL_NAME_LEN-4] = Hex_To_Char((Pb_Addr[1] & 0xF0) >> 4);
  Adv_Data[ADV_AD_FLAGS_LEN+ADV_LOCAL_NAME_LEN-3] = Hex_To_Char(Pb_Addr[1] & 0x0F);
  Adv_Data[ADV_AD_FLAGS_LEN+ADV_LOCAL_NAME_LEN-2] = Hex_To_Char((Pb_Addr[0] & 0xF0) >> 4);
  Adv_Data[ADV_AD_FLAGS_LEN+ADV_LOCAL_NAME_LEN-1] = Hex_To_Char(Pb_Addr[0] & 0x0F);

  Menu_SetIdentifier((char *)&Adv_Data[ADV_AD_FLAGS_LEN+2], 14);

  /* Start Fast or Low Power Advertising.*/
  if (GMAPAPP_Context.NumConn == 0u)
  {
    Primary_Adv_Interval_Min = FAST_ADV_INTERVAL_MIN;
    Primary_Adv_Interval_Max = FAST_ADV_INTERVAL_MAX;
    LOG_INFO_APP("Start Fast Advertising\n");
  }
  else
  {
    Primary_Adv_Interval_Min = SLOW_ADV_INTERVAL_MIN;
    Primary_Adv_Interval_Max = SLOW_ADV_INTERVAL_MAX;
    LOG_INFO_APP("Start Slow Advertising\n");
  }
  status = aci_gap_adv_set_configuration(ADV_TYPE,
                                         0,
                                         HCI_ADV_EVENT_PROP_CONNECTABLE,
                                         Primary_Adv_Interval_Min,
                                         Primary_Adv_Interval_Max,
                                         Primary_Adv_Channel_Map,
                                         GAP_PUBLIC_ADDR,
                                         GAP_PUBLIC_ADDR,
                                         Peer_Address,
                                         ADV_FILTER,
                                         Adv_TX_Power,
                                         Secondary_Adv_Max_Skip,
                                         0x01,
                                         0,
                                         Scan_Req_Notification_Enable);
  LOG_INFO_APP("aci_gap_adv_set_configuration() returns status 0x%02X\n",status);
  if (status != BLE_STATUS_SUCCESS)
  {
    return status;
  }

  /* Set Extended Advertising data */
  GMAPAPP_BuildAdvDataPacket(&Adv_Data[(ADV_AD_FLAGS_LEN+ADV_LOCAL_NAME_LEN)],
                            &advPacketLen,
                            AnnouncementType,
                            EnableSolicitationRequest,
                            Appearance);
  LOG_INFO_APP("CAP Advertising Packet (Length : %d) :\n", advPacketLen);

  status = hci_le_set_extended_advertising_data(0,
                                                HCI_SET_ADV_DATA_OPERATION_COMPLETE,
                                                0,
                                                (ADV_AD_FLAGS_LEN+ADV_LOCAL_NAME_LEN+advPacketLen),
                                                &Adv_Data[0]);
  LOG_INFO_APP("hci_le_set_extended_advertising_data() returns status 0x%02X\n",status);
  if (status != BLE_STATUS_SUCCESS)
  {
    return status;
  }

  /* Enable Extended Advertising */
  Adv_Set_t adv_set;
  adv_set.Advertising_Handle = 0;
  adv_set.Duration = 0;
  adv_set.Max_Extended_Advertising_Events = 0;
  status = hci_le_set_extended_advertising_enable(0x01, 1, &adv_set);
  LOG_INFO_APP("hci_le_set_extended_advertising_enable() returns status 0x%02X\n",status);
  return status;
}

uint8_t GMAPAPP_StopAdvertising(void)
{
  Adv_Set_t adv_set;
  uint8_t status;
  adv_set.Advertising_Handle = 0;
  adv_set.Duration = 0;
  adv_set.Max_Extended_Advertising_Events = 0;

  status = hci_le_set_extended_advertising_enable(0, 1, &adv_set);

  return status;
}

uint8_t GMAPAPP_Disconnect(void)
{
  uint8_t status = HCI_COMMAND_DISALLOWED_ERR_CODE;
  for (uint8_t i = 0; i < CFG_BLE_NUM_LINK; i++)
  {
    if (GMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle != 0xFFFFu)
    {
      status = hci_disconnect(GMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle,
                              HCI_REMOTE_USER_TERMINATED_CONNECTION_ERR_CODE);
      LOG_INFO_APP("hci_disconnect() with ConnHandle 0x%04X return status: 0x%02X\n",
                   GMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle,
                   status);
    }
  }
  return status;
}

uint8_t GMAPAPP_VolumeUp(void)
{
#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)
  uint8_t status = BLE_STATUS_SUCCESS;
  uint8_t volume;
  if (Mute == 1)
  {
    status = VCP_RENDER_SetMuteState(0);
    LOG_INFO_APP("VCP_RENDER_SetMuteState() with Mute %d returns status 0x%02X\n", Mute, status);
  }

  if (0xFF - Volume > VOLUME_STEP)
  {
    volume = Volume + VOLUME_STEP;
  }
  else
  {
    volume = 0xFF;
  }

  if (status == BLE_STATUS_SUCCESS)
  {
    status = VCP_RENDER_SetAbsVolume(volume);
    LOG_INFO_APP("VCP_RENDER_SetAbsVolume() with volume %d returns status 0x%02X\n", volume, status);
  }
  return status;
#else /*#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)*/
  return HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif
}

uint8_t GMAPAPP_VolumeDown(void)
{
#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)
  uint8_t status = BLE_STATUS_SUCCESS;
  uint8_t volume;

  if (Volume > VOLUME_STEP)
  {
    volume = Volume - VOLUME_STEP;
  }
  else
  {
    volume = 0;
    status = VCP_RENDER_SetMuteState(1);
    LOG_INFO_APP("VCP_RENDER_SetMuteState() with Mute %d returns status 0x%02X\n", Mute, status);
  }

  if (status == BLE_STATUS_SUCCESS)
  {
    status = VCP_RENDER_SetAbsVolume(volume);
    LOG_INFO_APP("VCP_RENDER_SetAbsVolume() with volume %d returns status 0x%02X\n", volume, status);
  }
  return status;
#else /*#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)*/
  return HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif
}

uint8_t GMAPAPP_ToggleMute(void)
{
#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)
  uint8_t status;

  Mute = (Mute + 1) % 2;

  status = VCP_RENDER_SetMuteState(Mute);
  LOG_INFO_APP("VCP_RENDER_SetMuteState() returns status 0x%02X\n",status);

  return status;
#else /*#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)*/
  return HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif
}

uint8_t GMAPAPP_ToggleMicrophoneMute(void)
{
#if (APP_MICP_ROLE_DEVICE_SUPPORT == 1u)
  uint8_t status;

  MicMute = (MicMute + 1) % 2;

  status = MICP_DEVICE_SetMute(MicMute);
  LOG_INFO_APP("MICP_DEVICE_SetMute() with Mute State %d returns status 0x%02X\n",MicMute,status);

  if (status == BLE_STATUS_SUCCESS)
  {
    Menu_SetMicMute(MicMute);
  }

  return status;
#else /*#if (APP_MICP_ROLE_DEVICE_SUPPORT == 1u)*/
  return HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif
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

#if ((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)
  if (GMAPAPP_Context.bap_role & BAP_ROLE_BROADCAST_SINK)
  {
    if (GMAPAPP_Context.BSNK.BIGSyncState == APP_BIG_SYNC_STATE_SYNCHRONIZED)
    {
      for (i = 0; i< GMAPAPP_Context.BSNK.current_num_bis; i++)
      {
        CODEC_ReceiveData(GMAPAPP_Context.BSNK.current_BIS_conn_handles[i], 1, &aSnkBuff[0]  + AudioFrameSize/2 + i);
      }
    }
  }
#endif /*((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)*/
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

#if ((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)
  if (GMAPAPP_Context.bap_role & BAP_ROLE_BROADCAST_SINK)
  {
    if (GMAPAPP_Context.BSNK.BIGSyncState == APP_BIG_SYNC_STATE_SYNCHRONIZED)
    {
      for (i = 0; i< GMAPAPP_Context.BSNK.current_num_bis; i++)
      {
        CODEC_ReceiveData(GMAPAPP_Context.BSNK.current_BIS_conn_handles[i], 1, &aSnkBuff[0] + i);
      }
    }
  }
#endif /*((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)*/
}

void CODEC_NotifyDataReady(uint16_t conn_handle, void* decoded_data, uint8_t channel_idx, uint8_t channel_nb)
{
  /* When only one channel is active, duplicate it for fake stereo on SAI */
  uint32_t i;
  uint8_t j;
  uint16_t *pData = (uint16_t *)(decoded_data);   /* 16 bits samples */

  if(pData == (pPrevData + 1)){
    /* we start receiving two channels */
    Nb_Active_Ch = 2;
  }
  pPrevData = pData;

  if (Nb_Active_Ch < 2)
  {
    for (i = 0; i < Sink_frame_size/2; i+=2*GMAPAPP_Context.SinkDecimation)
    {
      /* decoded_data is organized with a decimation equal to 2, so we duplicate each sample */
      pData[i+1] = pData[i];

      /* When Decimation multiplier is more than one, copy sample to adjacent empty samples */
      for (j = 1; j < GMAPAPP_Context.SinkDecimation; j++)
      {
        pData[i+2*j] = pData[i];
        pData[i+2*j+1] = pData[i];
      }
    }
  }
}

void APP_NotifyRxAudioCplt(uint16_t AudioFrameSize)
{
  uint8_t i;

  if (MicMute == 1)
  {
    /* Invalidate Rx data if microphone is mute */
    MEMSET(&aSrcBuff[0] + Source_frame_size/2, 0, Source_frame_size);
  }

  for (i = 0; i< APP_MAX_NUM_CIS; i++)
  {
    if (GMAPAPP_Context.cis_src_handle[i] != 0xFFFFu)
    {
      CODEC_SendData(GMAPAPP_Context.cis_src_handle[i], 1, &aSrcBuff[0] + AudioFrameSize/2 + i);
    }
  }
}

void APP_NotifyRxAudioHalfCplt(void)
{
  uint8_t i;

  if (MicMute == 1)
  {
    /* Invalidate Rx data if microphone is mute */
    MEMSET(&aSrcBuff[0], 0, Source_frame_size);
  }

  for (i = 0; i< APP_MAX_NUM_CIS; i++)
  {
    if (GMAPAPP_Context.cis_src_handle[i] != 0xFFFFu)
    {
      CODEC_SendData(GMAPAPP_Context.cis_src_handle[i], 1, &aSrcBuff[0]+i);
    }
  }
}

void GMAPAPP_AclConnected(uint16_t ConnHandle, uint8_t Peer_Address_Type, uint8_t Peer_Address[6], uint8_t role)
{
  APP_ACL_Conn_t *p_conn = APP_GetACLConn(ConnHandle);
  if (p_conn == 0)
  {
    p_conn = APP_AllocateACLConn(ConnHandle);
  }
  if (p_conn != 0)
  {
    p_conn->AudioProfile = AUDIO_PROFILE_NONE;
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

    /* Set Available Audio Contexts */
    p_conn->AvailableSnkAudioContext = GMAPAPP_Context.AvailableSnkAudioContext;
    p_conn->AvailableSrcAudioContext = GMAPAPP_Context.AvailableSrcAudioContext;
  }

  GMAPAPP_Context.NumConn++;
  if (GMAPAPP_Context.NumConn < CFG_BLE_NUM_LINK )
  {
    tBleStatus status;
    GMAPAPP_StopAdvertising();
    /* Start Advertising */
    if (GMAPAPP_Context.CSIPRank > 0)
    {
      status = GMAPAPP_StartAdvertising(CAP_GENERAL_ANNOUNCEMENT, 0, GAP_APPEARANCE_EARBUD);
    }
    else
    {
      status = GMAPAPP_StartAdvertising(CAP_GENERAL_ANNOUNCEMENT, 0, GAP_APPEARANCE_HEADPHONES);
    }
    LOG_INFO_APP("GMAPAPP_StartAdvertising() returns status 0x%02X\n",status);
    UNUSED(status);
  }
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
  /* Nothing to do */
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
  uint8_t status;
  Audio_Context_t snk_context = 0x0000;
  Audio_Context_t src_context = 0x0000;

  APP_ACL_Conn_t *p_conn = APP_GetACLConn(Conn_Handle);
  if (p_conn != 0)
  {
    /*Free CAP Connection Slot*/
    p_conn->Acl_Conn_Handle = 0xFFFFu;
    p_conn->AudioProfile = AUDIO_PROFILE_NONE;
    p_conn->ConfirmIndicationRequired = 0u;
    GMAPAPP_Context.NumConn--;

    /* Stop Sink if device was connected to Broadcast Assistant */
    GMAPAPP_StopSink();

#if (APP_NUM_SNK_ASE > 0u)
    for ( i = 0; i < APP_NUM_SNK_ASE;i++)
    {
      if (p_conn->pASEs->aSnkASE[i].allocated != 0x00)
      {
        if ( p_conn->pASEs->aSnkASE[i].streaming_audio_context != 0x0000u)
        {
          /* Calculate the available Audio Contexts now ASEs have 'released' streaming audio contexts */
          snk_context |= APP_ReleasedASEStreamingAudioContexts(p_conn,
                                                               &p_conn->pASEs->aSnkASE[i],
                                                               p_conn->pASEs->aSnkASE[i].streaming_audio_context);
        }
        LOG_INFO_APP("Free ASE ID %d on ACL connection handle 0x%04X\n",p_conn->pASEs->aSnkASE[i].ID,Conn_Handle);
        p_conn->pASEs->aSnkASE[i].ID = 0x00;
        p_conn->pASEs->aSnkASE[i].state  = ASE_STATE_IDLE;
        p_conn->pASEs->aSnkASE[i].num_channels  = 0x00;
        p_conn->pASEs->aSnkASE[i].streaming_audio_context = 0x00u;
        p_conn->pASEs->aSnkASE[i].allocated = 0x00u;
      }
    }
#endif /*(APP_NUM_SNK_ASE > 0u)*/
#if (APP_NUM_SRC_ASE > 0u)
    for ( i = 0; i < APP_NUM_SRC_ASE;i++)
    {
      if (p_conn->pASEs->aSrcASE[i].allocated != 0x00)
      {
        if ( p_conn->pASEs->aSrcASE[i].streaming_audio_context != 0x0000u)
        {
          /* Calculate the available Audio Contexts now ASEs have 'released' streaming audio contexts */
          src_context |= APP_ReleasedASEStreamingAudioContexts(p_conn,
                                                               &p_conn->pASEs->aSrcASE[i],
                                                               p_conn->pASEs->aSrcASE[i].streaming_audio_context);
        }
        LOG_INFO_APP("Free ASE ID %d on ACL connection handle 0x%04X\n",p_conn->pASEs->aSrcASE[i].ID,Conn_Handle);
        p_conn->pASEs->aSrcASE[i].ID = 0x00;
        p_conn->pASEs->aSrcASE[i].state  = ASE_STATE_IDLE;
        p_conn->pASEs->aSrcASE[i].num_channels  = 0x00;
        p_conn->pASEs->aSrcASE[i].streaming_audio_context = 0x00u;
        p_conn->pASEs->aSrcASE[i].allocated = 0x00u;
      }
    }
#endif /*(APP_NUM_SRC_ASE > 0u)*/

    p_conn->pASEs->acl_conn_handle = 0xFFFFu;
    p_conn->pASEs = 0;
    p_conn->AvailableSnkAudioContext = 0x0000u;
    p_conn->AvailableSrcAudioContext = 0x0000u;

    /* if context are now available, update them */
    if ((snk_context != 0x0000) || (src_context != 0x0000) )
    {
      uint8_t enable_ongoing = 0u;
      APP_ACL_Conn_t *p_other_conn;

      GMAPAPP_Context.AvailableSnkAudioContext |= snk_context;
      GMAPAPP_Context.AvailableSrcAudioContext |= src_context;

      /* Check if all supported audio contexts are now available and so AUDIO_CONTEXT_UNSPECIFIED should be also
       * set as available.
       */
      if (snk_context != 0x0000)
      {
        /*Check if "Unspecified Audio Context" should be add in available Audio Contexts now*/
        if ( (GMAPAPP_Context.AvailableSnkAudioContext | AUDIO_CONTEXT_UNSPECIFIED) == SUPPORTED_SNK_CONTEXTS)
        {
          GMAPAPP_Context.AvailableSnkAudioContext |= AUDIO_CONTEXT_UNSPECIFIED;
        }
      }
      if (src_context != 0x0000)
      {
        /*Check if "Unspecified Audio Context" should be add in available Audio Contexts now*/
        if ( (GMAPAPP_Context.AvailableSrcAudioContext | AUDIO_CONTEXT_UNSPECIFIED) == SUPPORTED_SRC_CONTEXTS)
        {
          GMAPAPP_Context.AvailableSrcAudioContext |= AUDIO_CONTEXT_UNSPECIFIED;
        }
      }

      status = CAP_SetAvailableAudioContexts(GMAPAPP_Context.AvailableSnkAudioContext,
                                             GMAPAPP_Context.AvailableSrcAudioContext);
      if (status == BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("Set Available Snk Audio Contexts 0x%04X and Available Src Audio Contexts 0x%04X\n",
                     GMAPAPP_Context.AvailableSnkAudioContext,
                     GMAPAPP_Context.AvailableSrcAudioContext);
      }
      else
      {
        LOG_INFO_APP("FAILURE to Set Available Snk Audio Contexts 0x%04X and Available Src Audio Contexts 0x%04X\n",
                     GMAPAPP_Context.AvailableSnkAudioContext,
                     GMAPAPP_Context.AvailableSrcAudioContext);
      }
      /*check if a response to an Enable operation Request is suspended with another CAP Initiator*/
      for (uint8_t conn = 0u; conn < CFG_BLE_NUM_LINK ; conn++)
      {
        if ((GMAPAPP_Context.ACL_Conn[conn].Acl_Conn_Handle != 0xFFFFu) && (&GMAPAPP_Context.ACL_Conn[conn] != p_conn))
        {
          p_other_conn = &GMAPAPP_Context.ACL_Conn[conn];
#if (APP_NUM_SNK_ASE > 0u)
          for ( uint8_t i = 0; i < GMAPAPP_Context.NumSnkASEs;i++)
          {
            if (p_other_conn->pASEs->aSnkASE[i].allocated == 1)
            {
              if (p_other_conn->pASEs->aSnkASE[i].enable_req == 1)
              {
                enable_ongoing = 1u;
                /* Send an response to the Enable Operation request if no CIS is still up*/
                if (GMAPAPP_Context.num_cis_established == 0u)
                {
                  status = CAP_Unicast_EnableOpRsp(p_other_conn->Acl_Conn_Handle,
                                                   p_other_conn->pASEs->aSnkASE[i].ID,
                                                   ASE_OP_RESP_SUCCESS,
                                                   ASE_OP_RESP_NO_REASON);
                  if (status != BLE_STATUS_SUCCESS)
                  {
                    LOG_INFO_APP("  Fail   : Enable Operation Response returns status 0x%02X for ASE ID %d on ConnHandle 0x%04X\n",
                                 status,
                                 p_other_conn->pASEs->aSnkASE[i].ID,
                                 p_other_conn->Acl_Conn_Handle);
                  }
                  else
                  {
                    LOG_INFO_APP("  Success: Enable Operation Response for ASE ID %d on ConnHandle 0x%04X\n",
                                 p_other_conn->pASEs->aSnkASE[i].ID,
                                 p_other_conn->Acl_Conn_Handle);
                  }
                }
              }
            }
          }
#endif /* (APP_NUM_SNK_ASE > 0u) */
#if (APP_NUM_SRC_ASE > 0u)
          for ( uint8_t i = 0; i < GMAPAPP_Context.NumSrcASEs;i++)
          {
            if (p_other_conn->pASEs->aSrcASE[i].allocated == 1)
            {
              if (p_other_conn->pASEs->aSrcASE[i].enable_req == 1)
              {
                enable_ongoing = 1u;
                /* Send an response to the Enable Operation request if no CIS is still up*/
                if (GMAPAPP_Context.num_cis_established == 0u)
                {
                  status = CAP_Unicast_EnableOpRsp(p_other_conn->Acl_Conn_Handle,
                                p_other_conn->pASEs->aSrcASE[i].ID,
                                ASE_OP_RESP_SUCCESS,
                                ASE_OP_RESP_NO_REASON);
                  if (status != BLE_STATUS_SUCCESS)
                  {
                    LOG_INFO_APP("  Fail   : Enable Operation Response returns status 0x%02X for ASE ID %d on ConnHandle 0x%04X\n",
                                 status,
                                 p_other_conn->pASEs->aSrcASE[i].ID,
                                 p_other_conn->Acl_Conn_Handle);
                  }
                  else
                  {
                    LOG_INFO_APP("  Success: Enable Operation Response for ASE ID %d on ConnHandle 0x%04X\n",
                                 p_other_conn->pASEs->aSrcASE[i].ID,
                                 p_other_conn->Acl_Conn_Handle);
                  }
                }
              }
            }
          }
#endif /* (APP_NUM_SRC_ASE > 0u) */
        }
      }
      /* Check if no response to an Enable Request is in progress to update Audio Contexts availability to
      * the connected CAP Initiators.
      */
      if (enable_ongoing == 0u)
      {
        for (uint8_t conn = 0u; conn < CFG_BLE_NUM_LINK ; conn++)
        {
          if (GMAPAPP_Context.ACL_Conn[conn].Acl_Conn_Handle != 0xFFFFu)
          {
            p_other_conn = &GMAPAPP_Context.ACL_Conn[conn];
            if ((p_other_conn->AvailableSnkAudioContext != GMAPAPP_Context.AvailableSnkAudioContext) \
                || (p_other_conn->AvailableSrcAudioContext != GMAPAPP_Context.AvailableSrcAudioContext))
            {
              /*update the available Audio Contexts of the all the connected CAP Initiators*/
              status = CAP_UpdateAvailableAudioContexts(p_other_conn->Acl_Conn_Handle,
                                                        GMAPAPP_Context.AvailableSnkAudioContext,
                                                        GMAPAPP_Context.AvailableSrcAudioContext);
              LOG_INFO_APP("Send Update Available Snk Audio Contexts 0x%04X and Available Src Audio Contexts 0x%04X with CAP Initiator on ConnHandle 0x%04X returns status 0x%02X\n",
                           GMAPAPP_Context.AvailableSnkAudioContext,
                           GMAPAPP_Context.AvailableSrcAudioContext,
                           p_other_conn->Acl_Conn_Handle,
                           status);
              p_other_conn->AvailableSnkAudioContext = GMAPAPP_Context.AvailableSnkAudioContext;
              p_other_conn->AvailableSrcAudioContext = GMAPAPP_Context.AvailableSrcAudioContext;
              UNUSED(status);
            }
            else
            {
              LOG_INFO_APP("Available Snk Audio Contexts 0x%04X and Available Src Audio Contexts 0x%04X with CAP Initiator on ConnHandle 0x%04X haven't changed\n",
                           GMAPAPP_Context.AvailableSnkAudioContext,
                           GMAPAPP_Context.AvailableSrcAudioContext,
                           p_other_conn->Acl_Conn_Handle);
            }
          }
        }
      }
    }
    /* Check if the disconnection is issued to a local User action*/
    if (Reason != HCI_CONNECTION_TERMINATED_BY_LOCAL_HOST_ERR_CODE)
    {
      GMAPAPP_StopAdvertising();
      /* Start Advertising */
      if (GMAPAPP_Context.CSIPRank > 0)
      {
        status = GMAPAPP_StartAdvertising(CAP_GENERAL_ANNOUNCEMENT, 0, GAP_APPEARANCE_EARBUD);
      }
      else
      {
        status = GMAPAPP_StartAdvertising(CAP_GENERAL_ANNOUNCEMENT, 0, GAP_APPEARANCE_HEADPHONES);
      }
      LOG_INFO_APP("GMAPAPP_StartAdvertising() returns status 0x%02X\n",status);
      Menu_SetWaitConnPage();
    }
    else
    {
      GMAPAPP_StopAdvertising();
      Menu_SetStartupPage();
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
#if (CFG_LCD_SUPPORTED == 1)
          Menu_SetNoStreamPage();
#endif /* (CFG_LCD_SUPPORTED == 1) */
          /*check if a response to an Enable operation Request is suspended with another CAP Initiator*/
          for (uint8_t conn = 0u; conn < CFG_BLE_NUM_LINK ; conn++)
          {
            if (GMAPAPP_Context.ACL_Conn[conn].Acl_Conn_Handle != 0xFFFFu)
            {
              p_conn = &GMAPAPP_Context.ACL_Conn[conn];
#if (APP_NUM_SNK_ASE > 0u)
              for ( uint8_t i = 0; i < GMAPAPP_Context.NumSnkASEs;i++)
              {
                if (p_conn->pASEs->aSnkASE[i].allocated == 1)
                {
                  if (p_conn->pASEs->aSnkASE[i].enable_req == 1)
                  {
                    status = CAP_Unicast_EnableOpRsp(p_conn->Acl_Conn_Handle,
                                  p_conn->pASEs->aSnkASE[i].ID,
                                  ASE_OP_RESP_SUCCESS,
                                  ASE_OP_RESP_NO_REASON);
                    LOG_INFO_APP("Enable Operation Response returns status 0x%02X for ASE ID %d on ConnHandle 0x%04X\n",
                                status,
                                p_conn->pASEs->aSnkASE[i].ID,
                                p_conn->Acl_Conn_Handle);
                  }
                }
              }
#endif /* (APP_NUM_SNK_ASE > 0u) */
#if (APP_NUM_SRC_ASE > 0u)
              for ( uint8_t i = 0; i < GMAPAPP_Context.NumSrcASEs;i++)
              {
                if (p_conn->pASEs->aSrcASE[i].allocated == 1)
                {
                  if (p_conn->pASEs->aSrcASE[i].enable_req == 1)
                  {
                    status = CAP_Unicast_EnableOpRsp(p_conn->Acl_Conn_Handle,
                                  p_conn->pASEs->aSrcASE[i].ID,
                                  ASE_OP_RESP_SUCCESS,
                                  ASE_OP_RESP_NO_REASON);
                    LOG_INFO_APP("Enable Operation Response returns status 0x%02X for ASE ID %d on ConnHandle 0x%04X\n",
                                status,
                                p_conn->pASEs->aSrcASE[i].ID,
                                p_conn->Acl_Conn_Handle);
                  }
                }
              }
#endif /* (APP_NUM_SRC_ASE > 0u) */
            }
          }
        }
        break;
      }
    }
  }
}

uint8_t GMAPAPP_SyncToPA(uint8_t AdvSID, uint8_t *pAdvAddress, uint8_t AdvAddressType)
{
#if ((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)
  uint8_t status;
  GMAPAPP_Context.BSNK.PASyncState = APP_PA_SYNC_STATE_SYNCHRONIZING;
  status = CAP_Broadcast_StartPASync(AdvSID,
                                     pAdvAddress,
                                     AdvAddressType,
                                     PA_EVENT_SKIP,
                                     PA_SYNC_TIMEOUT);
  LOG_INFO_APP("==>> Starting Broadcasting DiscoveryDevice with Source %02X:%02X:%02X:%02X:%02X:%02X\n",
                    pAdvAddress[5],
                    pAdvAddress[4],
                    pAdvAddress[3],
                    pAdvAddress[2],
                    pAdvAddress[1],
                    pAdvAddress[0]);
  if (status != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : CAP_Broadcast_StartPASync() function, result: 0x%02X\n", status);
  }
  else
  {
    LOG_INFO_APP("  Success: CAP_Broadcast_StartPASync() function\n");
  }

  return status;
#else /*((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)*/
  return HCI_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE_ERR_CODE;
#endif /*((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)*/
}

uint8_t GMAPAPP_StartSink(void)
{
#if ((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)
  uint8_t ret;

  LOG_INFO_APP(">>==  Start Sink\n");

  ret = CAP_Broadcast_StartAdvReportParsing();
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : CAP_Broadcast_StartAdvReportParsing() function, result: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: CAP_Broadcast_StartAdvReportParsing() function\n");
  }

  if (ret == BLE_STATUS_SUCCESS)
  {
    Scan_Param_Phy_t scan_param_phy;
    scan_param_phy.Scan_Type     = 0x00; /*Passive scanning*/
    scan_param_phy.Scan_Interval = SCAN_INTERVAL;
    scan_param_phy.Scan_Window   = SCAN_WINDOW;
    /* Starts an Observation procedure */
    ret = aci_gap_ext_start_scan( 0x00,
                                  GAP_OBSERVATION_PROC,
                                  0x00,                         /* Address type: Public */
                                  0x00,                         /* Filter duplicates: No */
                                  0x00,                         /* Scan continuously until explicitly disable */
                                  0x00,                         /* Scan continuously */
                                  0x00,                         /* Filter policy: Accept all */
                                  HCI_SCANNING_PHYS_LE_1M,
                                  &scan_param_phy);
    if (ret != BLE_STATUS_SUCCESS)
    {
      LOG_INFO_APP("  Fail   : aci_gap_ext_start_scan() function with Scan procedure 0x%02X, result: 0x%02X\n",
                   GAP_OBSERVATION_PROC,
                   ret);
    }
    else
    {
      LOG_INFO_APP("  Success: aci_gap_ext_start_scan() function with Scan procedure 0x%02X\n",
                   GAP_OBSERVATION_PROC);
      GMAPAPP_Context.BSNK.ScanState = APP_SCAN_STATE_SCANNING;
    }
  }

  return ret;
#else /*((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)*/
  return HCI_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE_ERR_CODE;
#endif /*((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)*/
}

uint8_t GMAPAPP_StopSink(void)
{
#if ((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)
  uint8_t ret = BLE_STATUS_SUCCESS;

  LOG_INFO_APP(">>==  Start Stop Broadcast Sink\n");

  if (GMAPAPP_Context.BSNK.ScanState == APP_SCAN_STATE_SCANNING)
  {
    ret = CAP_Broadcast_StopAdvReportParsing();
    if (ret != BLE_STATUS_SUCCESS)
    {
      LOG_INFO_APP("  Fail   : CAP_Broadcast_StopAdvReportParsing() function, result: 0x%02X\n", ret);
    }
    else
    {
      LOG_INFO_APP("  Success: CAP_Broadcast_StopAdvReportParsing() function\n");
    }

    ret = aci_gap_terminate_gap_proc(GAP_OBSERVATION_PROC);
    if (ret != BLE_STATUS_SUCCESS)
    {
      LOG_INFO_APP("  Fail   : aci_gap_terminate_gap_proc() function, result: 0x%02X\n", ret);
    }
    else
    {
      GMAPAPP_Context.BSNK.ScanState = APP_SCAN_STATE_IDLE;
      LOG_INFO_APP("  Success: aci_gap_terminate_gap_proc() function\n");
    }
  }

  if (GMAPAPP_Context.BSNK.PASyncState != APP_PA_SYNC_STATE_IDLE)
  {
    ret = CAP_Broadcast_StopPASync(GMAPAPP_Context.BSNK.PASyncHandle);
    if (ret != BLE_STATUS_SUCCESS)
    {
      LOG_INFO_APP("  Fail   : CAP_Broadcast_StopPASync() function, result: 0x%02X\n", ret);
    }
    else
    {
      LOG_INFO_APP("  Success: CAP_Broadcast_StopPASync() function\n");
      GMAPAPP_Context.BSNK.PASyncState = APP_PA_SYNC_STATE_IDLE;
    }
  }

  if (GMAPAPP_Context.BSNK.BIGSyncState != APP_BIG_SYNC_STATE_IDLE)
  {
    ret = CAP_Broadcast_StopBIGSync(BIG_HANDLE);
    if (ret != BLE_STATUS_SUCCESS)
    {
      LOG_INFO_APP("  Fail   : CAP_Broadcast_StopBIGSync() function, result: 0x%02X\n", ret);
    }
    else
    {
      LOG_INFO_APP("  Success: CAP_Broadcast_StopBIGSync() function\n");
      GMAPAPP_Context.BSNK.BIGSyncState = APP_BIG_SYNC_STATE_IDLE;
    }
  }

  LOG_INFO_APP(">>==  End Stop Broadcast Sink\n");
  return ret;

#else /*((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)*/
  return HCI_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE_ERR_CODE;
#endif /*((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)*/
}

void GMAPAPP_SetBroadcastMode(APP_BroadcastMode_t mode)
{
  GMAPAPP_Context.BroadcastMode = mode;
}

void GMAPAPP_ClearDatabase(void)
{
  BLE_AUDIO_STACK_DB_ClearAllRecords();
}

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
static tBleStatus CAPAPP_Init(Audio_Role_t AudioRole, uint8_t csip_config_id)
{
  tBleStatus status;
  Audio_Role_t audio_role = 0;
  Audio_Context_t available_snk_context = 0;
  Audio_Context_t available_src_context = 0;
  Audio_Context_t supported_snk_context = 0;
  Audio_Context_t supported_src_context = 0;
#if (APP_NUM_SNK_PAC_RECORDS > 0u)
  uint8_t a_snk_cap_id[MIN_NUM_BAP_SNK_CODEC_CAP] = BAP_MANDATORY_SNK_CODEC_CAP_LIST;
  uint8_t num_snk_capabilities = 0;
#endif /*(APP_NUM_SNK_PAC_RECORDS > 0u)*/
#if (APP_NUM_SRC_PAC_RECORDS > 0u)
  uint8_t a_src_cap_id[MIN_NUM_BAP_SRC_CODEC_CAP] = BAP_MANDATORY_SRC_CODEC_CAP_LIST;
  uint8_t num_src_capabilities = 0;
#endif /*(APP_NUM_SRC_PAC_RECORDS > 0u)*/
  Audio_Location_t audio_locations = FRONT_LEFT;

  LOG_INFO_APP("CAPAPP_Init() with audio role 0x%02X\n", AudioRole);

  /*Clear the CAP Configuration*/
  memset(&APP_CAP_Config, 0, sizeof(CAP_Config_t));

  /*Clear the BAP Configuration*/
  memset(&APP_BAP_Config, 0, sizeof(BAP_Config_t));

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

#if (APP_BAP_ROLE_UNICAST_SERVER_SUPPORT == 1u)
  APP_BAP_Config.Role |= BAP_ROLE_UNICAST_SERVER;
#endif /*(APP_BAP_ROLE_UNICAST_SERVER_SUPPORT == 1u)*/
#if (APP_BAP_ROLE_UNICAST_CLIENT_SUPPORT == 1u)
  APP_BAP_Config.Role |= BAP_ROLE_UNICAST_CLIENT;
#endif /*(APP_BAP_ROLE_UNICAST_CLIENT_SUPPORT == 1u)*/
#if (APP_BAP_ROLE_BROADCAST_SOURCE_SUPPORT == 1u)
  APP_BAP_Config.Role |= BAP_ROLE_BROADCAST_SOURCE;
#endif /*(APP_BAP_ROLE_BROADCAST_SOURCE_SUPPORT == 1u)*/
#if (APP_BAP_ROLE_BROADCAST_SINK_SUPPORT == 1u)
  APP_BAP_Config.Role |= BAP_ROLE_BROADCAST_SINK;
#endif /*(APP_BAP_ROLE_BROADCAST_SINK_SUPPORT == 1u)*/
#if (APP_BAP_ROLE_SCAN_DELEGATOR_SUPPORT == 1u)
  APP_BAP_Config.Role |= BAP_ROLE_SCAN_DELEGATOR;
#endif /*(APP_BAP_ROLE_SCAN_DELEGATOR_SUPPORT == 1u)*/
#if (APP_BAP_ROLE_BROADCAST_ASSISTANT_SUPPORT == 1u)
  APP_BAP_Config.Role |= BAP_ROLE_BROADCAST_ASSISTANT;
#endif /*(APP_BAP_ROLE_BROADCAST_ASSISTANT_SUPPORT == 1u)*/

  APP_BAP_Config.MaxNumBleLinks = CFG_BLE_NUM_LINK;
  APP_BAP_Config.MaxNumUSRLinks = MAX_NUM_USR_LINK;

  /*Published Audio Capabilities of Unicast Server and Broadcast Sink Configuration*/
  APP_BAP_Config.PACSSrvConfig.MaxNumSnkPACRecords = APP_NUM_SNK_PAC_RECORDS;
  APP_BAP_Config.PACSSrvConfig.MaxNumSrcPACRecords = APP_NUM_SRC_PAC_RECORDS;
  APP_BAP_Config.PACSSrvConfig.pStartRamAddr = (uint8_t *)&aPACSSrvMemBuffer;
  APP_BAP_Config.PACSSrvConfig.RamSize = BAP_PACS_SRV_DYN_ALLOC_SIZE;

#if ((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)
  if ((APP_BAP_Config.Role & BAP_ROLE_SCAN_DELEGATOR) == BAP_ROLE_SCAN_DELEGATOR)
  {
    /*Broadcast Audio Scan for Scan Delegator Configuration*/
    APP_BAP_Config.BASSSrvConfig.MaxNumBSRC = MAX_NUM_SDE_BSRC_INFO;
    APP_BAP_Config.BASSSrvConfig.MaxCodecConfSize = MAX_BASS_CODEC_CONFIG_SIZE;
    APP_BAP_Config.BASSSrvConfig.MaxMetadataLength = MAX_BASS_METADATA_SIZE;
    APP_BAP_Config.BASSSrvConfig.MaxNumBaseSubgroups = MAX_NUM_BASS_BASE_SUBGROUPS;
    APP_BAP_Config.BASSSrvConfig.pStartRamAddr = (uint8_t *)&aBASSSrvMemBuffer;
    APP_BAP_Config.BASSSrvConfig.RamSize = BAP_BASS_SRV_DYN_ALLOC_SIZE;
  }

  if (((APP_BAP_Config.Role & BAP_ROLE_SCAN_DELEGATOR) == BAP_ROLE_SCAN_DELEGATOR)
   || ((APP_BAP_Config.Role & BAP_ROLE_BROADCAST_SINK) == BAP_ROLE_BROADCAST_SINK))
  {
    /* BASE structure for fragmented PA reports */
    APP_BAP_Config.BASEStrConfig.pStartRamAddr = (uint8_t *)&aBASEMemBuffer;
    APP_BAP_Config.BASEStrConfig.RamSize = BAP_BASE_TOTAL_BUFFER_SIZE;
  }
#endif /*((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)*/

  /*Audio Stream Endpoint of Unicast Server Configuration*/
  APP_BAP_Config.ASCSSrvConfig.AudioRole = AudioRole;
  APP_BAP_Config.ASCSSrvConfig.MaxNumSnkASEs = APP_NUM_SNK_ASE;
  APP_BAP_Config.ASCSSrvConfig.MaxNumSrcASEs = APP_NUM_SRC_ASE;
  APP_BAP_Config.ASCSSrvConfig.MaxCodecConfSize = MAX_USR_CODEC_CONFIG_SIZE;
  APP_BAP_Config.ASCSSrvConfig.MaxMetadataLength = MAX_USR_METADATA_SIZE;
  APP_BAP_Config.ASCSSrvConfig.CachingEn = APP_ASCS_CACHING;
  APP_BAP_Config.ASCSSrvConfig.pStartRamAddr = (uint8_t *)&aASCSSrvMemBuffer;
  APP_BAP_Config.ASCSSrvConfig.RamSize = BAP_ASCS_SRV_DYN_ALLOC_SIZE;

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

#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)
  APP_VCP_Config.Role = VCP_ROLE_RENDERER;
  APP_VCP_Config.MaxNumBleLinks = CFG_BLE_NUM_LINK;
  APP_VCP_Config.Renderer.InitialMuteState = 0u;
  APP_VCP_Config.Renderer.InitialVolumeSetting = Volume;
  APP_VCP_Config.Renderer.VolumeStepSize = VOLUME_STEP;
  APP_VCP_Config.Renderer.NumAICInst = APP_VCP_RDR_NUM_AIC_INSTANCES;
  APP_VCP_Config.Renderer.NumVOCInst = APP_VCP_RDR_NUM_VOC_INSTANCES;
  APP_VCP_Config.Renderer.pStartRamAddr = (uint8_t*)aRenderMemBuffer;
  APP_VCP_Config.Renderer.RamSize = BLE_VCP_RDR_DYN_ALLOC_SIZE;
#if (APP_VCP_RDR_NUM_AIC_INSTANCES > 0)
  for (uint8_t inst = 0u ; inst < APP_VCP_RDR_NUM_AIC_INSTANCES ; inst++)
  {
    aAICInst[inst].InstID = inst;
    aAICInst[inst].Status = 0x00u;
    aAICInst[inst].State.GainSetting = 0;
    aAICInst[inst].State.Mute = VCP_AIC_MUTE_DISABLED;
    aAICInst[inst].State.GainMode = VCP_AIC_GAIN_MODE_MANUAL_ONLY;
    aAICInst[inst].Prop.GainSettingUnits = 10u;
    aAICInst[inst].Prop.GainSettingMin = -20;
    aAICInst[inst].Prop.GainSettingMax = 10;
    aAICInst[inst].AudioInputType = AUDIO_INPUT_BLUETOOTH;
    aAICInst[inst].MaxDescriptionLength = APP_VCP_RDR_AIC_DESCRIPTION_LENGTH;
    if (inst == 0u)
    {
      aAICInst[inst].pDescription = (uint8_t *)"Bluetooth";
      aAICInst[inst].DescriptionLength = 9u;
    }
    else
    {
      aAICInst[inst].pDescription = (uint8_t *)"Line In";
      aAICInst[inst].DescriptionLength = 7u;
    }
  }
  APP_VCP_Config.Renderer.pAICInst = &aAICInst[0u];
#endif /*(APP_VCP_RDR_NUM_AIC_INSTANCES > 0)*/
#if (APP_VCP_RDR_NUM_VOC_INSTANCES > 0)
  for (uint8_t inst = 0u ; inst < APP_VCP_RDR_NUM_VOC_INSTANCES ; inst++)
  {
    aVOCInst[inst].InstID = inst;
    if (inst == 0u)
    {
      aVOCInst[inst].VolumeOffset = -23;
      aVOCInst[inst].AudioLocation = FRONT_LEFT;
      aVOCInst[inst].MaxDescriptionLength = APP_VCP_RDR_VOC_DESCRIPTION_LENGTH;
      aVOCInst[inst].pDescription = (uint8_t *)"Left Speaker";
      aVOCInst[inst].DescriptionLength = 12u;
    }
    else
    {
      aVOCInst[inst].VolumeOffset = -23;
      aVOCInst[inst].AudioLocation = FRONT_RIGHT;
      aVOCInst[inst].MaxDescriptionLength = APP_VCP_RDR_VOC_DESCRIPTION_LENGTH;
      aVOCInst[inst].pDescription = (uint8_t *)"Right Speaker";
      aVOCInst[inst].DescriptionLength = 13u;
    }
  }
  APP_VCP_Config.Renderer.pVOCInst = &aVOCInst[0u];
#endif /*(APP_VCP_RDR_NUM_VOC_INSTANCES > 0)*/
#endif /*(APP_VCP_ROLE_RENDERER_SUPPORT == 1u)*/

#if (APP_MICP_ROLE_DEVICE_SUPPORT == 1)
    APP_MICP_Config.Role = MICP_ROLE_DEVICE;
    APP_MICP_Config.MaxNumBleLinks = CFG_BLE_NUM_LINK;
    APP_MICP_Config.Device.InitialMuteState = 0u;
    APP_MICP_Config.Device.NumAICInst = APP_MICP_DEV_NUM_AIC_INSTANCES;
#if (APP_MICP_DEV_NUM_AIC_INSTANCES > 0u)
    for (uint8_t inst = 0u ; inst < APP_MICP_DEV_NUM_AIC_INSTANCES ; inst++)
    {
      aMicAICInst[inst].InstID = inst;
      aMicAICInst[inst].Status = 0x00u;
      aMicAICInst[inst].State.GainSetting = 0;
      aMicAICInst[inst].State.Mute = MICP_AIC_MUTE_DISABLED;
      aMicAICInst[inst].State.GainMode = MICP_AIC_GAIN_MODE_MANUAL_ONLY;
      aMicAICInst[inst].Prop.GainSettingUnits = 10u;
      aMicAICInst[inst].Prop.GainSettingMin = -20;
      aMicAICInst[inst].Prop.GainSettingMax = 10;
      aMicAICInst[inst].AudioInputType = AUDIO_INPUT_BLUETOOTH;
      aMicAICInst[inst].MaxDescriptionLength = APP_MICP_DEV_AIC_DESCRIPTION_LENGTH;
      if (inst == 0u)
      {
        aMicAICInst[inst].pDescription = (uint8_t *)"Bluetooth";
        aMicAICInst[inst].DescriptionLength = 9u;
      }
#if (APP_MICP_DEV_NUM_AIC_INSTANCES > 1u)
      else
      {
        aMicAICInst[inst].pDescription = (uint8_t *)"Line In";
        aMicAICInst[inst].DescriptionLength = 7u;
      }
#endif /* (APP_MICP_DEV_NUM_AIC_INSTANCES > 1u) */
    }
    APP_MICP_Config.Device.pAICInst = &aMicAICInst[0u];
#else
    APP_MICP_Config.Device.pAICInst = 0;
#endif /* (APP_MICP_DEV_NUM_AIC_INSTANCES > 0u) */
    APP_MICP_Config.Device.pStartRamAddr = (uint8_t*)aMicDevMemBuffer;
    APP_MICP_Config.Device.RamSize = BLE_MICP_DEV_DYN_ALLOC_SIZE;
#endif /* (APP_MICP_ROLE_DEVICE_SUPPORT == 1) */

#if (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1)
    APP_CSIP_Config.Role = CSIP_ROLE_SET_MEMBER;
    APP_CSIP_Config.MaxNumBleLinks = CFG_BLE_NUM_LINK;
    APP_CSIP_Config.Set_Member_Config.MaxNumCSISInstances = APP_CSIP_SET_MEMBER_NUM_INSTANCES;
    APP_CSIP_Config.Set_Member_Config.pStartRamAddr = (uint8_t*)aSetMemberMemBuffer;
    APP_CSIP_Config.Set_Member_Config.RamSize = BLE_CSIP_SET_MEMBER_DYN_ALLOC_SIZE;
#endif /* (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1) */

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

  for (uint8_t i = 0; i< APP_MAX_NUM_CIS; i++)
  {
    GMAPAPP_Context.cis_src_handle[i] = 0xFFFFu;
    GMAPAPP_Context.cis_snk_handle[i] = 0xFFFFu;
    GMAPAPP_Context.cis_handle[i] = 0xFFFFu;
  }

  GMAPAPP_Context.NumSnkASEs = 0u;
  GMAPAPP_Context.NumSrcASEs = 0u;
  GMAPAPP_Context.NumConn = 0;
  GMAPAPP_Context.bap_role = APP_BAP_Config.Role;

  for (uint8_t conn = 0; conn< CFG_BLE_NUM_LINK; conn++)
  {
    GMAPAPP_Context.aASEs[conn].acl_conn_handle = 0xFFFFu;
#if (APP_NUM_SNK_ASE > 0u)
    for (uint8_t i = 0; i< APP_NUM_SNK_ASE; i++)
    {
      GMAPAPP_Context.aASEs[conn].aSnkASE[i].ID = 0x00;
      GMAPAPP_Context.aASEs[conn].aSnkASE[i].type = ASE_SINK;
      GMAPAPP_Context.aASEs[conn].aSnkASE[i].state  = ASE_STATE_IDLE;
      GMAPAPP_Context.aASEs[conn].aSnkASE[i].num_channels  = 0x00;
      GMAPAPP_Context.aASEs[conn].aSnkASE[i].streaming_audio_context = 0x00u;
      GMAPAPP_Context.aASEs[conn].aSnkASE[i].allocated = 0x00u;
    }
#endif /*(APP_NUM_SNK_ASE > 0u)*/

#if (APP_NUM_SRC_ASE > 0u)
    for (uint8_t i = 0; i< APP_NUM_SRC_ASE; i++)
    {
      GMAPAPP_Context.aASEs[conn].aSrcASE[i].ID = 0x00;
      GMAPAPP_Context.aASEs[conn].aSrcASE[i].state  = ASE_STATE_IDLE;
      GMAPAPP_Context.aASEs[conn].aSrcASE[i].type = ASE_SOURCE;
      GMAPAPP_Context.aASEs[conn].aSrcASE[i].num_channels  = 0x00;
      GMAPAPP_Context.aASEs[conn].aSrcASE[i].streaming_audio_context = 0x00u;
      GMAPAPP_Context.aASEs[conn].aSrcASE[i].allocated = 0x00u;
    }
#endif /*(APP_NUM_SRC_ASE > 0u)*/
    GMAPAPP_Context.ACL_Conn[conn].Acl_Conn_Handle = 0xFFFFu;
    GMAPAPP_Context.ACL_Conn[conn].AudioProfile = AUDIO_PROFILE_NONE;
    GMAPAPP_Context.ACL_Conn[conn].AvailableSnkAudioContext = 0x0000u;
    GMAPAPP_Context.ACL_Conn[conn].AvailableSrcAudioContext = 0x0000u;
  }

  /* Initialise BASE report variables */
  GMAPAPP_Context.BSNK.base_group.pSubgroups = &(GMAPAPP_Context.BSNK.base_subgroups[0]);
  GMAPAPP_Context.BSNK.base_subgroups[0].pCodecSpecificConf = &(GMAPAPP_Context.BSNK.codec_specific_config_subgroup[0][0]);
  GMAPAPP_Context.BSNK.base_subgroups[0].pMetadata = &(GMAPAPP_Context.BSNK.subgroup_metadata[0][0]);
  GMAPAPP_Context.BSNK.base_subgroups[0].pBIS = &(GMAPAPP_Context.BSNK.base_bis[0]);
  GMAPAPP_Context.BSNK.base_subgroups[1].pCodecSpecificConf = &(GMAPAPP_Context.BSNK.codec_specific_config_subgroup[1][0]);
  GMAPAPP_Context.BSNK.base_subgroups[1].pMetadata = &(GMAPAPP_Context.BSNK.subgroup_metadata[1][0]);
  GMAPAPP_Context.BSNK.base_subgroups[1].pBIS = &(GMAPAPP_Context.BSNK.base_bis[1]);
  GMAPAPP_Context.BSNK.base_bis[0].pCodecSpecificConf = &(GMAPAPP_Context.BSNK.codec_specific_config_bis[0][0]);
  GMAPAPP_Context.BSNK.base_bis[1].pCodecSpecificConf = &(GMAPAPP_Context.BSNK.codec_specific_config_bis[1][0]);


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

  /*Initialize Flags of the APP Context*/
  GMAPAPP_Context.audio_role_setup = 0x00;
  GMAPAPP_Context.num_cis_src = 0;
  GMAPAPP_Context.num_cis_snk = 0;
  GMAPAPP_Context.audio_role_setup = 0x00;
  GMAPAPP_Context.num_cis_src = 0;
  GMAPAPP_Context.num_cis_snk = 0;

#if (APP_NUM_SNK_ASE >= 1)
  audio_role |= AUDIO_ROLE_SINK;
  supported_snk_context = SUPPORTED_SNK_CONTEXTS;
  available_snk_context = SUPPORTED_SNK_CONTEXTS;
#if (APP_NUM_SNK_PAC_RECORDS > 0u)
  /* 2 Audio Sink Mandatory codec specific capabilities */
  num_snk_capabilities = MIN_NUM_BAP_SNK_CODEC_CAP;
#endif /* (APP_NUM_SNK_PAC_RECORDS > 0u) */
#else /* (APP_NUM_SNK_ASE >= 1) */
   supported_snk_context = AUDIO_CONTEXT_UNSPECIFIED;
   available_snk_context = AUDIO_CONTEXT_UNSPECIFIED;
#endif /* (APP_NUM_SNK_ASE >= 1) */

#if (APP_NUM_SRC_ASE >= 1)
  audio_role |= AUDIO_ROLE_SOURCE;
  supported_src_context = SUPPORTED_SRC_CONTEXTS;
  available_src_context = SUPPORTED_SRC_CONTEXTS;
#if (APP_NUM_SRC_PAC_RECORDS > 0u)
  num_src_capabilities = MIN_NUM_BAP_SRC_CODEC_CAP; /* 1 Audio Source Mandatory codec specific capabilities */
#endif /* (APP_NUM_SRC_PAC_RECORDS > 0u) */
#else /* (APP_NUM_SRC_ASE >= 1) */
  supported_src_context = AUDIO_CONTEXT_UNSPECIFIED;
  available_src_context = AUDIO_CONTEXT_UNSPECIFIED;
#endif /* (APP_NUM_SRC_ASE >= 1) */

  /*Set Codec ID configuration*/
  if (audio_role & AUDIO_ROLE_SINK)
  {
#if (APP_NUM_SNK_PAC_RECORDS > 0u)
    Audio_Context_t preferred_audio_context = AUDIO_CONTEXT_MEDIA;
    /* Register the Sink Codec Capabilities */
    /*Set Codec ID configuration*/
    GMAPAPP_Context.aSnkPACRecord[0].Cap.CodecID.CodingFormat = AUDIO_CODING_FORMAT_LC3;
    GMAPAPP_Context.aSnkPACRecord[0].Cap.CodecID.CompanyID = COMPANY_ID;
    GMAPAPP_Context.aSnkPACRecord[0].Cap.CodecID.VsCodecID = 0x0000;

    /* Set supported codec capabilities configuration */
    GMAPAPP_Context.aSnkCap[0][0] = 0x03;
    GMAPAPP_Context.aSnkCap[0][1] = CODEC_CAP_SAMPLING_FREQ;
    for (uint8_t i = 0; i < num_snk_capabilities ;i++)
    {
      GMAPAPP_Context.aSnkCap[0][2] |= (uint8_t) (APP_CodecSpecificCap[a_snk_cap_id[i]].Freq);
      GMAPAPP_Context.aSnkCap[0][3] |= (uint8_t) ((APP_CodecSpecificCap[a_snk_cap_id[i]].Freq >> 8 ));
      if (APP_CodecSpecificCap[a_snk_cap_id[i]].Freq == SUPPORTED_SAMPLE_FREQ_16000_HZ)
      {
        preferred_audio_context = AUDIO_CONTEXT_CONVERSATIONAL;
      }
    }

    GMAPAPP_Context.aSnkCap[0][4] = 0x02;
    GMAPAPP_Context.aSnkCap[0][5] = CODEC_CAP_FRAME_DURATION;
    for (uint8_t i = 0; i < num_snk_capabilities ;i++)
    {
      GMAPAPP_Context.aSnkCap[0][6] |= APP_CodecSpecificCap[a_snk_cap_id[i]].FrameDuration;
    }

    GMAPAPP_Context.aSnkCap[0][7] = 0x05;
    GMAPAPP_Context.aSnkCap[0][8] = CODEC_CAP_OCTETS_PER_CODEC_FRAME;
    for (uint8_t i = 0; i < num_snk_capabilities ;i++)
    {
      uint32_t min_octet_per_codec_frame = APP_CodecSpecificCap[a_snk_cap_id[i]].MinOctetsPerCodecFrame;
      uint32_t max_octet_per_codec_frame = APP_CodecSpecificCap[a_snk_cap_id[i]].MaxOctetsPerCodecFrame;

      if (i > 0)
      {
        min_octet_per_codec_frame = MIN(min_octet_per_codec_frame,
                                        GMAPAPP_Context.aSnkCap[0][9] + (GMAPAPP_Context.aSnkCap[0][10] << 8));
        max_octet_per_codec_frame = MAX(max_octet_per_codec_frame,
                                        GMAPAPP_Context.aSnkCap[0][11] + (GMAPAPP_Context.aSnkCap[0][12] << 8));
      }

      GMAPAPP_Context.aSnkCap[0][9] = (uint8_t) (min_octet_per_codec_frame);
      GMAPAPP_Context.aSnkCap[0][10] = (uint8_t) (min_octet_per_codec_frame >> 8 );
      GMAPAPP_Context.aSnkCap[0][11] = (uint8_t) (max_octet_per_codec_frame);
      GMAPAPP_Context.aSnkCap[0][12] = (uint8_t) (max_octet_per_codec_frame >> 8);
    }

    GMAPAPP_Context.aSnkCap[0][13] = 0x02;
    GMAPAPP_Context.aSnkCap[0][14] = CODEC_CAP_AUDIO_CHNL_COUNTS;
    GMAPAPP_Context.aSnkCap[0][15] = (APP_MAX_CHANNEL_PER_SNK_ASE | SUPPORTED_AUDIO_CHNL_COUNT_1);

    GMAPAPP_Context.aSnkPACRecord[0].Cap.pSpecificCap = &GMAPAPP_Context.aSnkCap[0][0];
    GMAPAPP_Context.aSnkPACRecord[0].Cap.SpecificCapLength = 16u;

    /* metadata in Audio Codec Configuration*/
    GMAPAPP_Context.aSnkPACRecord[0].Cap.MetadataLength = 0x04;
    GMAPAPP_Context.aSnkMetadata[0][0] = METADATA_PREFERRED_AUDIO_CONTEXTS_LENGTH;
    GMAPAPP_Context.aSnkMetadata[0][1] = METADATA_PREFERRED_AUDIO_CONTEXTS;
    GMAPAPP_Context.aSnkMetadata[0][2] = (uint8_t) (preferred_audio_context);
    GMAPAPP_Context.aSnkMetadata[0][3] = (uint8_t) (preferred_audio_context >> 8 );
    GMAPAPP_Context.aSnkPACRecord[0].Cap.pMetadata = &GMAPAPP_Context.aSnkMetadata[0][0];
    status = CAP_RegisterSnkPACRecord(&GMAPAPP_Context.aSnkPACRecord[0],&GMAPAPP_Context.aSnkPACRecordHandle[0]);
    if (status == BLE_STATUS_SUCCESS)
    {
      LOG_INFO_APP("Successfully registered Sink PAC record id %d\n",0);
    }
    else
    {
      LOG_INFO_APP("Failed to register Sink PAC record id %d\n",0);
      return status;
    }
#endif /* (APP_NUM_SNK_PAC_RECORDS > 0u) */

#if (APP_NUM_SNK_ASE >= 1)
#if (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 0u)
#if (APP_NUM_SNK_ASE == 1 && APP_MAX_CHANNEL_PER_SNK_ASE == 1)
    audio_locations = MONO_AUDIO_LOCATIONS;
#else /* (APP_NUM_SNK_ASE == 1 && APP_MAX_CHANNEL_PER_SNK_ASE == 1) */
    audio_locations = STEREO_AUDIO_LOCATIONS;
#endif /* (APP_NUM_SNK_ASE == 1 && APP_MAX_CHANNEL_PER_SNK_ASE == 1) */
#else /*(APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1u) */
    if (csip_config_id == 0)
    {
#if (APP_NUM_SNK_ASE == 1 && APP_MAX_CHANNEL_PER_SNK_ASE == 1)
      audio_locations = MONO_AUDIO_LOCATIONS;
#else /* (APP_NUM_SNK_ASE == 1 && APP_MAX_CHANNEL_PER_SNK_ASE == 1) */
      audio_locations = STEREO_AUDIO_LOCATIONS;
#endif /* (APP_NUM_SNK_ASE == 1 && APP_MAX_CHANNEL_PER_SNK_ASE == 1) */
    }
    else
    {
      audio_locations = (1u << (csip_config_id - 1u));
    }
#endif /*(APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 0u) */
    /*
     * Set the supported Audio Locations, which will be exposed
     * to remote device for reception of audio data
     */
    status = CAP_SetSnkAudioLocations(audio_locations);
    if (status == BLE_STATUS_SUCCESS)
    {
      LOG_INFO_APP("Successfully set the Supported Sink Audio Locations 0x%08X\n",audio_locations);
      GMAPAPP_Context.SnkAudioLocations = audio_locations;
    }
    else
    {
      LOG_INFO_APP("Failed to set the Supported Sink Audio Locations\n");
      return status;
    }
#endif /*(APP_NUM_SNK_ASE >= 1)*/
  }

  if (audio_role & AUDIO_ROLE_SOURCE)
  {
#if (APP_NUM_SRC_PAC_RECORDS > 0u)
    /* Register the Source Codec Capabilities */
    /*Set Codec ID configuration*/
    GMAPAPP_Context.aSrcPACRecord[0].Cap.CodecID.CodingFormat = AUDIO_CODING_FORMAT_LC3;
    GMAPAPP_Context.aSrcPACRecord[0].Cap.CodecID.CompanyID = COMPANY_ID;
    GMAPAPP_Context.aSrcPACRecord[0].Cap.CodecID.VsCodecID = 0x0000;

    /* Set supported codec capabilities configuration */
    GMAPAPP_Context.aSrcCap[0][0] = 0x03;
    GMAPAPP_Context.aSrcCap[0][1] = CODEC_CAP_SAMPLING_FREQ;
    for (uint8_t i = 0; i < num_src_capabilities ;i++)
    {
      GMAPAPP_Context.aSrcCap[0][2] |= (uint8_t) (APP_CodecSpecificCap[a_src_cap_id[i]].Freq);
      GMAPAPP_Context.aSrcCap[0][3] |= (uint8_t) ((APP_CodecSpecificCap[a_src_cap_id[i]].Freq >> 8 ));
    }

    GMAPAPP_Context.aSrcCap[0][4] = 0x02;
    GMAPAPP_Context.aSrcCap[0][5] = CODEC_CAP_FRAME_DURATION;
    for (uint8_t i = 0; i < num_src_capabilities ;i++)
    {
      GMAPAPP_Context.aSrcCap[0][6] |= APP_CodecSpecificCap[a_src_cap_id[i]].FrameDuration;
    }

    GMAPAPP_Context.aSrcCap[0][7] = 0x05;
    GMAPAPP_Context.aSrcCap[0][8] = CODEC_CAP_OCTETS_PER_CODEC_FRAME;
    for (uint8_t i = 0; i < num_src_capabilities ;i++)
    {
      uint32_t min_octet_per_codec_frame = APP_CodecSpecificCap[a_src_cap_id[i]].MinOctetsPerCodecFrame;
      uint32_t max_octet_per_codec_frame = APP_CodecSpecificCap[a_src_cap_id[i]].MaxOctetsPerCodecFrame;

      if (i > 0)
      {
        min_octet_per_codec_frame = MIN(min_octet_per_codec_frame,
                                        GMAPAPP_Context.aSrcCap[0][9] + (GMAPAPP_Context.aSrcCap[0][10] << 8));
        max_octet_per_codec_frame = MAX(max_octet_per_codec_frame,
                                        GMAPAPP_Context.aSrcCap[0][11] + (GMAPAPP_Context.aSrcCap[0][12] << 8));
      }

      GMAPAPP_Context.aSrcCap[0][9] = (uint8_t) (min_octet_per_codec_frame);
      GMAPAPP_Context.aSrcCap[0][10] = (uint8_t) (min_octet_per_codec_frame >> 8 );
      GMAPAPP_Context.aSrcCap[0][11] = (uint8_t) (max_octet_per_codec_frame);
      GMAPAPP_Context.aSrcCap[0][12] = (uint8_t) (max_octet_per_codec_frame >> 8);
    }

    GMAPAPP_Context.aSrcCap[0][13] = 0x02;
    GMAPAPP_Context.aSrcCap[0][14] = CODEC_CAP_AUDIO_CHNL_COUNTS;
    GMAPAPP_Context.aSrcCap[0][15] = (APP_MAX_CHANNEL_PER_SRC_ASE | SUPPORTED_AUDIO_CHNL_COUNT_1);

    GMAPAPP_Context.aSrcPACRecord[0].Cap.pSpecificCap = &GMAPAPP_Context.aSrcCap[0][0];
    GMAPAPP_Context.aSrcPACRecord[0].Cap.SpecificCapLength = 16u;

    /* metadata in Audio Codec Configuration*/
    GMAPAPP_Context.aSrcPACRecord[0].Cap.MetadataLength = 0x04;
    GMAPAPP_Context.aSrcMetadata[0][0] = METADATA_PREFERRED_AUDIO_CONTEXTS_LENGTH;
    GMAPAPP_Context.aSrcMetadata[0][1] = METADATA_PREFERRED_AUDIO_CONTEXTS;
    GMAPAPP_Context.aSrcMetadata[0][2] = (uint8_t) (AUDIO_CONTEXT_CONVERSATIONAL);
    GMAPAPP_Context.aSrcMetadata[0][3] = (uint8_t) (AUDIO_CONTEXT_CONVERSATIONAL >> 8 );
    GMAPAPP_Context.aSrcPACRecord[0].Cap.pMetadata = &GMAPAPP_Context.aSrcMetadata[0][0];

    status = CAP_RegisterSrcPACRecord(&GMAPAPP_Context.aSrcPACRecord[0],&GMAPAPP_Context.aSrcPACRecordHandle[0]);
    if (status == BLE_STATUS_SUCCESS)
    {
      LOG_INFO_APP("Successfully registered Source PAC record id %d\n",0);
    }
    else
    {
      LOG_INFO_APP("Failed to register Source PAC record id %d\n",0);
      return status;
    }
#endif /*(APP_NUM_SRC_PAC_RECORDS > 0u)*/
#if (APP_NUM_SRC_ASE >= 1)
#if (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 0u)
#if (APP_NUM_SRC_ASE == 1 && APP_MAX_CHANNEL_PER_SRC_ASE == 1)
    audio_locations = MONO_AUDIO_LOCATIONS;
#else /* (APP_NUM_SRC_ASE == 1 && APP_MAX_CHANNEL_PER_SRC_ASE == 1) */
    audio_locations = STEREO_AUDIO_LOCATIONS;
#endif /* (APP_NUM_SRC_ASE == 1 && APP_MAX_CHANNEL_PER_SRC_ASE == 1) */
#else /*(APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1u) */
    if (csip_config_id == 0)
    {
#if (APP_NUM_SRC_ASE == 1 && APP_MAX_CHANNEL_PER_SRC_ASE == 1)
        audio_locations = MONO_AUDIO_LOCATIONS;
#else /* (APP_NUM_SRC_ASE == 1 && APP_MAX_CHANNEL_PER_SRC_ASE == 1) */
        audio_locations = STEREO_AUDIO_LOCATIONS;
#endif /* (APP_NUM_SRC_ASE == 1 && APP_MAX_CHANNEL_PER_SRC_ASE == 1) */
    }
    else
    {
      audio_locations =  (1u << (csip_config_id - 1u));
    }
#endif /*(APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 0u) */
    /*
     * Set the supported Audio Locations, which will be exposed
     * to remote device for reception of audio data
     */
    status = CAP_SetSrcAudioLocations(audio_locations);
    if (status == BLE_STATUS_SUCCESS)
    {
      LOG_INFO_APP("Successfully set the Supported Source Audio Locations 0x%08X\n",audio_locations);
      GMAPAPP_Context.SrcAudioLocations = audio_locations;
    }
    else
    {
      LOG_INFO_APP("Failed to set the Supported Source Audio Locations\n");
      return status;
    }
  }
#endif  /* (APP_NUM_SRC_ASE >= 1) */

  /* Set supported audio context for reception and transmission */
  status = CAP_SetSupportedAudioContexts(supported_snk_context, supported_src_context);
  if (status == BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("Successfully set the Supported Audio Contexts:\n");
    LOG_INFO_APP("Snk Audio Contexts : 0x%04X\n",supported_snk_context);
    LOG_INFO_APP("Src Audio Contexts : 0x%04X\n",supported_src_context);
    GMAPAPP_Context.SupportedSnkAudioContext = supported_snk_context;
    GMAPAPP_Context.SupportedSrcAudioContext = supported_src_context;
  }
  else
  {
    LOG_INFO_APP("Failed to set the Supported Audio Contexts\n");
    return status;
  }

  /* Set Available audio context for reception and transmission */
  status = CAP_SetAvailableAudioContexts(available_snk_context, available_src_context);
  if (status == BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("Successfully set the Available Audio Contexts\n");
    LOG_INFO_APP("Snk Audio Contexts : 0x%04X\n",available_snk_context);
    LOG_INFO_APP("Src Audio Contexts : 0x%04X\n",available_src_context);
    GMAPAPP_Context.AvailableSnkAudioContext = available_snk_context;
    GMAPAPP_Context.AvailableSrcAudioContext = available_src_context;
  }
  else
  {
    LOG_INFO_APP("Failed to set the Available Audio Contexts\n");
    return status;
  }
  /* Register the Audio Stream Endpoints*/
  status = CAP_RegisterAudioChannels(APP_NUM_SNK_ASE,
                                     APP_NUM_SRC_ASE);
  if (status == BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("Successfully register %ld Sink Audio Stream Endpoints\n", APP_NUM_SNK_ASE);
    LOG_INFO_APP("Successfully register %ld Source Audio Stream Endpoints\n", APP_NUM_SRC_ASE);
    GMAPAPP_Context.NumSnkASEs = APP_NUM_SNK_ASE;
    GMAPAPP_Context.NumSrcASEs = APP_NUM_SRC_ASE;
  }
  else
  {
    LOG_INFO_APP("Failed to register Audio Channels\n");
    return status;
  }

#if ((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)
  if (GMAPAPP_Context.SnkAudioLocations == 0)
  {
  /*Configure Audio Location for Broadcast Sink */
#if (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 0u)
    audio_locations = STEREO_AUDIO_LOCATIONS;
#else /*(APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1u) */
    if (csip_config_id == 0)
    {
      audio_locations = STEREO_AUDIO_LOCATIONS;
    }
    else
    {
      audio_locations = (1u << (csip_config_id - 1u));
    }
#endif /*(APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 0u) */
    status = CAP_SetSnkAudioLocations(audio_locations);
    if (status == BLE_STATUS_SUCCESS)
    {
      LOG_INFO_APP("Successfully set the Supported Sink Audio Locations 0x%08X\n",audio_locations);
      GMAPAPP_Context.BSNK.Audio_Location = audio_locations;
    }
    else
    {
      LOG_INFO_APP("Failed to set the Supported Sink Audio Locations\n");
      return status;
    }
  }
  else
  {
    GMAPAPP_Context.BSNK.Audio_Location = GMAPAPP_Context.SnkAudioLocations;
  }
#endif /*((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER) */

  return status;
}

static tBleStatus GMAPAPP_GMAPInit(GMAP_Role_t Role)
{
  uint8_t status;
  uint8_t i;
#if (APP_NUM_SNK_PAC_RECORDS > 0)
  uint8_t snk_record_i = MIN_NUM_BAP_SNK_PAC_RECORDS;
#endif /*(APP_NUM_SNK_PAC_RECORDS > 0)*/
#if (APP_NUM_SRC_PAC_RECORDS > 0)
  uint8_t src_record_i = MIN_NUM_BAP_SRC_PAC_RECORDS;
#endif /*(APP_NUM_SRC_PAC_RECORDS > 0)*/
  uint8_t ret = BLE_STATUS_SUCCESS;
  uint8_t num_gmap_src_pac = 0;
  uint8_t num_gmap_snk_pac = 0;
  uint8_t a_ugt_src_pac_id[MIN_NUM_GMAP_SRC_CODEC_CAP];
  uint8_t a_ugt_snk_pac_id[MIN_NUM_GMAP_SNK_CODEC_CAP];
  status = USECASE_DEV_MGMT_Init();

  if (status == BLE_STATUS_SUCCESS)
  {
    if ((APP_GMAP_ROLE & GMAP_ROLE_UNICAST_GAME_TERMINAL) != 0)
    {
      if ((APP_GMAP_UGT_FEATURE & UGT_FEATURES_SOURCE_SUPPORT) != 0)
      {
        a_ugt_src_pac_id[num_gmap_src_pac++] = LC3_16_1;
        a_ugt_src_pac_id[num_gmap_src_pac++] = LC3_16_2;
        a_ugt_src_pac_id[num_gmap_src_pac++] = LC3_32_1;
        a_ugt_src_pac_id[num_gmap_src_pac++] = LC3_32_2;
      }

      if ((APP_GMAP_UGT_FEATURE & UGT_FEATURES_80_KBPS_SOURCE_SUPPORT) != 0)
      {
        a_ugt_src_pac_id[num_gmap_src_pac++] = LC3_48_1;
        a_ugt_src_pac_id[num_gmap_src_pac++] = LC3_48_2;
      }

      if ((APP_GMAP_UGT_FEATURE & UGT_FEATURES_64_KBPSSINK_SUPPORT) != 0)
      {
        a_ugt_snk_pac_id[num_gmap_snk_pac++] = LC3_32_1;
        a_ugt_snk_pac_id[num_gmap_snk_pac++] = LC3_32_2;
      }
    }

    if (((APP_GMAP_ROLE & GMAP_ROLE_UNICAST_GAME_TERMINAL) != 0
        && (APP_GMAP_UGT_FEATURE & UGT_FEATURES_SINK_SUPPORT) != 0)
        || (APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) != 0)
    {
        a_ugt_snk_pac_id[num_gmap_snk_pac++] = LC3_48_1;
        a_ugt_snk_pac_id[num_gmap_snk_pac++] = LC3_48_2;
        a_ugt_snk_pac_id[num_gmap_snk_pac++] = LC3_48_3;
        a_ugt_snk_pac_id[num_gmap_snk_pac++] = LC3_48_4;
    }
#if (APP_NUM_SNK_PAC_RECORDS > 0u)
    if (num_gmap_snk_pac > 0)
    {
      /* Register the Sink Codec Capabilities */

      /*Set Codec ID configuration*/
      GMAPAPP_Context.aSnkPACRecord[snk_record_i].Cap.CodecID.CodingFormat = AUDIO_CODING_FORMAT_LC3;
      GMAPAPP_Context.aSnkPACRecord[snk_record_i].Cap.CodecID.CompanyID = COMPANY_ID;
      GMAPAPP_Context.aSnkPACRecord[snk_record_i].Cap.CodecID.VsCodecID = 0x0000;

      /* Set supported codec capabilities configuration */
      GMAPAPP_Context.aSnkCap[snk_record_i][0] = 0x03;
      GMAPAPP_Context.aSnkCap[snk_record_i][1] = CODEC_CAP_SAMPLING_FREQ;

      for (i = 0; i < num_gmap_snk_pac; i++)
      {
        GMAPAPP_Context.aSnkCap[snk_record_i][2] |= (uint8_t) (APP_CodecSpecificCap[a_ugt_snk_pac_id[i]].Freq);
        GMAPAPP_Context.aSnkCap[snk_record_i][3] |= (uint8_t) ((APP_CodecSpecificCap[a_ugt_snk_pac_id[i]].Freq >> 8 ));
      }

      GMAPAPP_Context.aSnkCap[snk_record_i][4] = 0x02;
      GMAPAPP_Context.aSnkCap[snk_record_i][5] = CODEC_CAP_FRAME_DURATION;

      for (i = 0; i < num_gmap_snk_pac; i++)
      {
        GMAPAPP_Context.aSnkCap[snk_record_i][6] |= APP_CodecSpecificCap[a_ugt_snk_pac_id[i]].FrameDuration;
      }

      GMAPAPP_Context.aSnkCap[snk_record_i][7] = 0x05;
      GMAPAPP_Context.aSnkCap[snk_record_i][8] = CODEC_CAP_OCTETS_PER_CODEC_FRAME;

      for (i = 0; i < num_gmap_snk_pac; i++)
      {
        uint32_t min_octet_per_codec_frame = APP_CodecSpecificCap[a_ugt_snk_pac_id[i]].MinOctetsPerCodecFrame;
        uint32_t max_octet_per_codec_frame = APP_CodecSpecificCap[a_ugt_snk_pac_id[i]].MaxOctetsPerCodecFrame;

        if (i > 0)
        {
          min_octet_per_codec_frame = MIN(min_octet_per_codec_frame,
                                          GMAPAPP_Context.aSnkCap[snk_record_i][9] + (GMAPAPP_Context.aSnkCap[snk_record_i][10] << 8));
          max_octet_per_codec_frame = MAX(max_octet_per_codec_frame,
                                          GMAPAPP_Context.aSnkCap[snk_record_i][11] + (GMAPAPP_Context.aSnkCap[snk_record_i][12] << 8));
        }

        GMAPAPP_Context.aSnkCap[snk_record_i][9] = (uint8_t) (min_octet_per_codec_frame);
        GMAPAPP_Context.aSnkCap[snk_record_i][10] = (uint8_t) (min_octet_per_codec_frame >> 8 );
        GMAPAPP_Context.aSnkCap[snk_record_i][11] = (uint8_t) (max_octet_per_codec_frame);
        GMAPAPP_Context.aSnkCap[snk_record_i][12] = (uint8_t) (max_octet_per_codec_frame >> 8);
      }

      GMAPAPP_Context.aSnkCap[snk_record_i][13] = 0x02;
      GMAPAPP_Context.aSnkCap[snk_record_i][14] = CODEC_CAP_AUDIO_CHNL_COUNTS;
      GMAPAPP_Context.aSnkCap[snk_record_i][15] = (APP_MAX_CHANNEL_PER_SRC_ASE | SUPPORTED_AUDIO_CHNL_COUNT_1);

      GMAPAPP_Context.aSnkPACRecord[snk_record_i].Cap.pSpecificCap = &GMAPAPP_Context.aSnkCap[snk_record_i][0];
      GMAPAPP_Context.aSnkPACRecord[snk_record_i].Cap.SpecificCapLength = 16u;

      /* metadata in Audio Codec Configuration*/
      GMAPAPP_Context.aSnkPACRecord[snk_record_i].Cap.MetadataLength = 0x00;

      GMAPAPP_Context.aSnkPACRecord[snk_record_i].Cap.pMetadata = &GMAPAPP_Context.aSnkMetadata[snk_record_i][0];

      ret = CAP_RegisterSnkPACRecord(&GMAPAPP_Context.aSnkPACRecord[snk_record_i],
                                     &GMAPAPP_Context.aSnkPACRecordHandle[snk_record_i]);
      if (ret == BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("Successfully registered Sink PAC Record id %d\n",snk_record_i);
        snk_record_i++;
      }
      else
      {
        LOG_INFO_APP("Failed to register Sink PAC Record id %d\n",snk_record_i);
        return ret;
      }
    }
#endif /* (APP_NUM_SNK_PAC_RECORDS > 0u)*/

#if (APP_NUM_SRC_PAC_RECORDS > 0u)
    if (num_gmap_src_pac > 0)
    {
      /* Register the Source Codec Capabilities */
      /*Set Codec ID configuration*/
      GMAPAPP_Context.aSrcPACRecord[src_record_i].Cap.CodecID.CodingFormat = AUDIO_CODING_FORMAT_LC3;
      GMAPAPP_Context.aSrcPACRecord[src_record_i].Cap.CodecID.CompanyID = COMPANY_ID;
      GMAPAPP_Context.aSrcPACRecord[src_record_i].Cap.CodecID.VsCodecID = 0x0000;

      /* Set supported codec capabilities configuration */
      GMAPAPP_Context.aSrcCap[src_record_i][0] = 0x03;
      GMAPAPP_Context.aSrcCap[src_record_i][1] = CODEC_CAP_SAMPLING_FREQ;

      for (i = 0; i < num_gmap_src_pac ;i++)
      {
        GMAPAPP_Context.aSrcCap[src_record_i][2] |= (uint8_t) (APP_CodecSpecificCap[a_ugt_src_pac_id[i]].Freq);
        GMAPAPP_Context.aSrcCap[src_record_i][3] |= (uint8_t) ((APP_CodecSpecificCap[a_ugt_src_pac_id[i]].Freq >> 8 ));
      }

      GMAPAPP_Context.aSrcCap[src_record_i][4] = 0x02;
      GMAPAPP_Context.aSrcCap[src_record_i][5] = CODEC_CAP_FRAME_DURATION;

      for (i = 0; i < num_gmap_src_pac ;i++)
      {
        GMAPAPP_Context.aSrcCap[src_record_i][6] |= APP_CodecSpecificCap[a_ugt_src_pac_id[i]].FrameDuration;
      }

      GMAPAPP_Context.aSrcCap[src_record_i][7] = 0x05;
      GMAPAPP_Context.aSrcCap[src_record_i][8] = CODEC_CAP_OCTETS_PER_CODEC_FRAME;

      for (i = 0; i < num_gmap_src_pac ;i++)
      {
        uint32_t min_octet_per_codec_frame = APP_CodecSpecificCap[a_ugt_snk_pac_id[i]].MinOctetsPerCodecFrame;
        uint32_t max_octet_per_codec_frame = APP_CodecSpecificCap[a_ugt_snk_pac_id[i]].MaxOctetsPerCodecFrame;

        if (i > 0)
        {
          min_octet_per_codec_frame = MIN(min_octet_per_codec_frame,
                                          GMAPAPP_Context.aSrcCap[src_record_i][9] + (GMAPAPP_Context.aSrcCap[src_record_i][10] << 8));
          max_octet_per_codec_frame = MAX(max_octet_per_codec_frame,
                                          GMAPAPP_Context.aSrcCap[src_record_i][11] + (GMAPAPP_Context.aSrcCap[src_record_i][12] << 8));
        }

        GMAPAPP_Context.aSrcCap[src_record_i][9] = (uint8_t) (min_octet_per_codec_frame);
        GMAPAPP_Context.aSrcCap[src_record_i][10] = (uint8_t) (min_octet_per_codec_frame >> 8 );
        GMAPAPP_Context.aSrcCap[src_record_i][11] = (uint8_t) (max_octet_per_codec_frame);
        GMAPAPP_Context.aSrcCap[src_record_i][12] = (uint8_t) (max_octet_per_codec_frame >> 8);
      }

      GMAPAPP_Context.aSrcCap[src_record_i][13] = 0x02;
      GMAPAPP_Context.aSrcCap[src_record_i][14] = CODEC_CAP_AUDIO_CHNL_COUNTS;
      GMAPAPP_Context.aSrcCap[src_record_i][15] = (APP_MAX_CHANNEL_PER_SNK_ASE | SUPPORTED_AUDIO_CHNL_COUNT_1);

      GMAPAPP_Context.aSrcPACRecord[src_record_i].Cap.pSpecificCap = &GMAPAPP_Context.aSrcCap[src_record_i][0];
      GMAPAPP_Context.aSrcPACRecord[src_record_i].Cap.SpecificCapLength = 16u;

      /* metadata in Audio Codec Configuration*/
      GMAPAPP_Context.aSrcPACRecord[src_record_i].Cap.MetadataLength = 0x00;

      GMAPAPP_Context.aSrcPACRecord[src_record_i].Cap.pMetadata = &GMAPAPP_Context.aSrcMetadata[src_record_i][0];

      ret = CAP_RegisterSrcPACRecord(&GMAPAPP_Context.aSrcPACRecord[src_record_i],
                                     &GMAPAPP_Context.aSrcPACRecordHandle[src_record_i]);
      if (ret == BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("Successfully registered Source PAC Record id %d\n",src_record_i);
        src_record_i++;
      }
      else
      {
        LOG_INFO_APP("Failed to register Source PAC Record id %d\n",src_record_i);
        return ret;
      }
    }
#endif /* (APP_NUM_SRC_PAC_RECORDS > 0u)*/
  }
  return GMAP_Init(Role, 0, APP_GMAP_UGT_FEATURE, 0x00, APP_GMAP_BGR_FEATURE);
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

    case CAP_UNICAST_SERVER_ASE_STATE_EVT:
    {
      BAP_ASE_State_Params_t *p_info = (BAP_ASE_State_Params_t *)pNotification->pInfo;
      tBleStatus status;
      APP_ASE_Info_t *p_ase = 0;
      uint8_t streaming_ongoing = 0u;
      APP_ACL_Conn_t *p_conn;
      APP_ACL_Conn_t *p_other_conn;
      LOG_INFO_APP("ASE ID %d [Type 0x%02X ] with remote CAP Initiator on ConnHandle 0x%04X is in State 0x%02X\n",
                  p_info->ASE_ID,
                  p_info->Type,
                  pNotification->ConnHandle,
                  p_info->State);
      p_conn = APP_GetACLConn(pNotification->ConnHandle);
      if (p_conn != 0)
      {
        p_ase = APP_GetASE(p_info->ASE_ID,pNotification->ConnHandle);
        if (p_ase == 0)
        {
          if (p_info->Type == ASE_SINK)
          {
#if (APP_NUM_SNK_ASE > 0u)
            for ( uint8_t i = 0; i < GMAPAPP_Context.NumSnkASEs;i++)
            {
              if (p_conn->pASEs->aSnkASE[i].allocated == 0)
              {
                p_ase = &p_conn->pASEs->aSnkASE[i];
                p_ase->allocated = 1u;
                p_ase->ID = p_info->ASE_ID;
                p_ase->type = p_info->Type;
                p_ase->enable_req = 0u;
                break;
              }
            }
#endif /* (APP_NUM_SNK_ASE > 0u) */
          }
          if (p_info->Type == ASE_SOURCE)
          {
#if (APP_NUM_SRC_ASE > 0u)
            for (uint8_t i = 0; i < GMAPAPP_Context.NumSrcASEs;i++)
            {
              if (p_conn->pASEs->aSrcASE[i].allocated == 0)
              {
                p_ase = &p_conn->pASEs->aSrcASE[i];
                p_ase->allocated = 1u;
                p_ase->ID = p_info->ASE_ID;
                p_ase->type = p_info->Type;
                p_ase->enable_req = 0u;
                break;
              }
            }
#endif /* (APP_NUM_SRC_ASE > 0u) */
          }
        }
        if (p_ase != 0)
        {
          Audio_Context_t snk_context = 0;
          Audio_Context_t src_context = 0;

          /* If ASE State Notification is received, we can consider that potential response to an Enable Operation Request
           * is no more expected by remote CAP Initiator
           */
          p_ase->enable_req = 0u;

          /* ASE was in Enabling/Streaming state but state is no more one of these : streaming is ending*/
          if (((p_ase->state == ASE_STATE_ENABLING) || (p_ase->state == ASE_STATE_STREAMING)) \
            && ((p_info->State != ASE_STATE_ENABLING) && (p_info->State != ASE_STATE_STREAMING)))
          {
            LOG_INFO_APP("ASE ID %d associated to Streaming Audio Contexts 0x%04X is released/Disabled, check if Audio Contexts should be Available now\n",
                         p_ase->ID,
                         p_ase->streaming_audio_context);
            /* Release the Streaming Audio Contexts associated to the ASE to calculate resulting
             * new available audio contexts in compliance with other ASEs in ENABLING or STREAMING state
             */
            if (p_ase->type == ASE_SINK)
            {
              if ( p_ase->streaming_audio_context != 0x0000u)
              {
                snk_context = APP_ReleasedASEStreamingAudioContexts(p_conn,
                                                                    p_ase,
                                                                    p_ase->streaming_audio_context);
              }
            }
            else
            {
              if ( p_ase->streaming_audio_context != 0x0000u)
              {
                src_context = APP_ReleasedASEStreamingAudioContexts(p_conn,
                                                                    p_ase,
                                                                    p_ase->streaming_audio_context);
              }
            }
            /* reset the Audio Contexts which was associated to the ASE*/
            p_ase->streaming_audio_context = 0x0000u;

            /*update the ASE state*/
            p_ase->state = p_info->State;

            /* Check if potential removed Audio Contexts is also associated to an Enabling/streaming ASE in order to
             * check if it would be available or not*/
#if (APP_NUM_SNK_ASE > 0u)
            for ( uint8_t i = 0; i < GMAPAPP_Context.NumSnkASEs;i++)
            {
              if (p_conn->pASEs->aSnkASE[i].allocated == 1)
              {
                if ((p_conn->pASEs->aSnkASE[i].state == ASE_STATE_ENABLING) \
                  || (p_conn->pASEs->aSnkASE[i].state == ASE_STATE_STREAMING))
                {
                  streaming_ongoing = 1u;
                }
              }
            }
#endif /* (APP_NUM_SNK_ASE > 0u) */
#if (APP_NUM_SRC_ASE > 0u)
            for ( uint8_t i = 0; i < GMAPAPP_Context.NumSrcASEs;i++)
            {
              if (p_conn->pASEs->aSrcASE[i].allocated == 1)
              {
                if ((p_conn->pASEs->aSrcASE[i].state == ASE_STATE_ENABLING) \
                  || (p_conn->pASEs->aSrcASE[i].state == ASE_STATE_STREAMING))
                {
                  streaming_ongoing = 1u;
                }
              }
            }
#endif /* (APP_NUM_SRC_ASE > 0u) */
            /* if context are now available, update them */
            if ((snk_context != 0x0000) || (src_context != 0x0000) )
            {
              GMAPAPP_Context.AvailableSnkAudioContext |= snk_context;
              GMAPAPP_Context.AvailableSrcAudioContext |= src_context;
              /* Check if all supported audio contexts are now available and so AUDIO_CONTEXT_UNSPECIFIED should be also
               * set as available.
               */
              if (snk_context != 0x0000)
              {
                /*Check if "Unspecified Audio Context" should be add in available Audio Contexts now*/
                if ( (GMAPAPP_Context.AvailableSnkAudioContext | AUDIO_CONTEXT_UNSPECIFIED) == SUPPORTED_SNK_CONTEXTS)
                {
                  GMAPAPP_Context.AvailableSnkAudioContext |= AUDIO_CONTEXT_UNSPECIFIED;
                }
              }
              if (src_context != 0x0000)
              {
                /*Check if "Unspecified Audio Context" should be add in available Audio Contexts now*/
                if ( (GMAPAPP_Context.AvailableSrcAudioContext | AUDIO_CONTEXT_UNSPECIFIED) == SUPPORTED_SRC_CONTEXTS)
                {
                  GMAPAPP_Context.AvailableSrcAudioContext |= AUDIO_CONTEXT_UNSPECIFIED;
                }
              }
              /*Set new available audio contexts for BAP Announcement*/
              status = CAP_SetAvailableAudioContexts(GMAPAPP_Context.AvailableSnkAudioContext,
                                                     GMAPAPP_Context.AvailableSrcAudioContext);
              if (status == BLE_STATUS_SUCCESS)
              {
                LOG_INFO_APP("Set Available Snk Audio Contexts 0x%04X and Available Src Audio Contexts 0x%04X\n",
                             GMAPAPP_Context.AvailableSnkAudioContext,
                             GMAPAPP_Context.AvailableSrcAudioContext);
              }
              else
              {
                LOG_INFO_APP("FAILURE to Set Available Snk Audio Contexts 0x%04X and Available Src Audio Contexts 0x%04X\n",
                             GMAPAPP_Context.AvailableSnkAudioContext,
                             GMAPAPP_Context.AvailableSrcAudioContext);
              }
              if (GMAPAPP_Context.NumConn < CFG_BLE_NUM_LINK )
              {
                GMAPAPP_StopAdvertising();
                /* Start Advertising */
                if (GMAPAPP_Context.CSIPRank > 0)
                {
                  status = GMAPAPP_StartAdvertising(CAP_GENERAL_ANNOUNCEMENT, 0, GAP_APPEARANCE_EARBUD);
                }
                else
                {
                  status = GMAPAPP_StartAdvertising(CAP_GENERAL_ANNOUNCEMENT, 0, GAP_APPEARANCE_HEADPHONES);
                }
                LOG_INFO_APP("GMAPAPP_StartAdvertising() returns status 0x%02X\n",status);
              }
            }
            /* If no more ASEs are in Enabling/Streaming state, we could check if ASE associated to other CAP Initiator
             * are waiting for a response to an Enable Operation Request.
             */
            if (streaming_ongoing == 0u)
            {
              uint8_t enable_ongoing = 0u;
              /*check if a response to an Enable operation Request is suspended with another CAP Initiator*/
              for (uint8_t conn = 0u; conn < CFG_BLE_NUM_LINK ; conn++)
              {
                if ((GMAPAPP_Context.ACL_Conn[conn].Acl_Conn_Handle != 0xFFFFu) && (&GMAPAPP_Context.ACL_Conn[conn] != p_conn))
                {
                  p_other_conn = &GMAPAPP_Context.ACL_Conn[conn];
#if (APP_NUM_SNK_ASE > 0u)
                  for ( uint8_t i = 0; i < GMAPAPP_Context.NumSnkASEs;i++)
                  {
                    if (p_other_conn->pASEs->aSnkASE[i].allocated == 1)
                    {
                      if (p_other_conn->pASEs->aSnkASE[i].enable_req == 1)
                      {
                        enable_ongoing = 1u;
                        /* Send an response to the Enable Operation request if no CIS is still up*/
                        if (GMAPAPP_Context.num_cis_established == 0u)
                        {
                          status = CAP_Unicast_EnableOpRsp(p_other_conn->Acl_Conn_Handle,
                                        p_other_conn->pASEs->aSnkASE[i].ID,
                                        ASE_OP_RESP_SUCCESS,
                                        ASE_OP_RESP_NO_REASON);
                          if (status != BLE_STATUS_SUCCESS)
                          {
                            LOG_INFO_APP("  Fail   : Enable Operation Response returns status 0x%02X for ASE ID %d on ConnHandle 0x%04X\n",
                                         status,
                                         p_other_conn->pASEs->aSnkASE[i].ID,
                                         p_other_conn->Acl_Conn_Handle);
                          }
                          else
                          {
                            LOG_INFO_APP("  Success: Enable Operation Response for ASE ID %d on ConnHandle 0x%04X\n",
                                         p_other_conn->pASEs->aSnkASE[i].ID,
                                         p_other_conn->Acl_Conn_Handle);
                          }
                        }
                      }
                    }
                  }
#endif /* (APP_NUM_SNK_ASE > 0u) */
#if (APP_NUM_SRC_ASE > 0u)
                  for ( uint8_t i = 0; i < GMAPAPP_Context.NumSrcASEs;i++)
                  {
                    if (p_other_conn->pASEs->aSrcASE[i].allocated == 1)
                    {
                      if (p_other_conn->pASEs->aSrcASE[i].enable_req == 1)
                      {
                        enable_ongoing = 1u;
                        /* Send an response to the Enable Operation request if no CIS is still up*/
                        if (GMAPAPP_Context.num_cis_established == 0u)
                        {
                          status = CAP_Unicast_EnableOpRsp(p_other_conn->Acl_Conn_Handle,
                                        p_other_conn->pASEs->aSrcASE[i].ID,
                                        ASE_OP_RESP_SUCCESS,
                                        ASE_OP_RESP_NO_REASON);
                          if (status != BLE_STATUS_SUCCESS)
                          {
                            LOG_INFO_APP("  Fail   : Enable Operation Response returns status 0x%02X for ASE ID %d on ConnHandle 0x%04X\n",
                                         status,
                                         p_other_conn->pASEs->aSrcASE[i].ID,
                                         p_other_conn->Acl_Conn_Handle);
                          }
                          else
                          {
                            LOG_INFO_APP("  Success: Enable Operation Response for ASE ID %d on ConnHandle 0x%04X\n",
                                         p_other_conn->pASEs->aSrcASE[i].ID,
                                         p_other_conn->Acl_Conn_Handle);
                          }
                        }
                      }
                    }
                  }
#endif /* (APP_NUM_SRC_ASE > 0u) */
                }
              }
              /* Check if no response to an Enable Request is in progress to update Audio Contexts availability to
              * the connected CAP Initiators.
              */
              if (enable_ongoing == 0u)
              {
                for (uint8_t conn = 0u; conn < CFG_BLE_NUM_LINK ; conn++)
                {
                  if (GMAPAPP_Context.ACL_Conn[conn].Acl_Conn_Handle != 0xFFFFu)
                  {
                    p_other_conn = &GMAPAPP_Context.ACL_Conn[conn];
                    if ((p_other_conn->AvailableSnkAudioContext != GMAPAPP_Context.AvailableSnkAudioContext) \
                        || (p_other_conn->AvailableSrcAudioContext != GMAPAPP_Context.AvailableSrcAudioContext))
                    {
                      /*update the available Audio Contexts of the all the connected CAP Initiators*/
                      status = CAP_UpdateAvailableAudioContexts(p_other_conn->Acl_Conn_Handle,
                                                                GMAPAPP_Context.AvailableSnkAudioContext,
                                                                GMAPAPP_Context.AvailableSrcAudioContext);
                      LOG_INFO_APP("Send Update Available Snk Audio Contexts 0x%04X and Available Src Audio Contexts 0x%04X with CAP Initiator on ConnHandle 0x%04X returns status 0x%02X\n",
                                   GMAPAPP_Context.AvailableSnkAudioContext,
                                   GMAPAPP_Context.AvailableSrcAudioContext,
                                   p_other_conn->Acl_Conn_Handle,
                                   status);
                      p_other_conn->AvailableSnkAudioContext = GMAPAPP_Context.AvailableSnkAudioContext;
                      p_other_conn->AvailableSrcAudioContext = GMAPAPP_Context.AvailableSrcAudioContext;
                      UNUSED(status);
                    }
                    else
                    {
                      LOG_INFO_APP("Available Snk Audio Contexts 0x%04X and Available Src Audio Contexts 0x%04X with CAP Initiator on ConnHandle 0x%04X haven't changed\n",
                                   GMAPAPP_Context.AvailableSnkAudioContext,
                                   GMAPAPP_Context.AvailableSrcAudioContext,
                                   p_other_conn->Acl_Conn_Handle);
                    }
                  }
                }
              }
            }
          }
          else
          {
            /*update the ASE state*/
            p_ase->state = p_info->State;
          }

          if ((p_info->State == ASE_STATE_ENABLING) || (p_info->State == ASE_STATE_STREAMING))
          {
            /*Parse the Metadata*/
            APP_ParseMetadataParams(p_ase,
                                    pNotification->ConnHandle,
                                    p_info->p.pEnStrDisConf->pMetadata,
                                    p_info->p.pEnStrDisConf->MetadataLength);
          }
        }
      }
      break;
    }

    case CAP_UNICAST_ASE_METADATA_UPDATED_EVT:
    {
      BAP_ASE_State_Params_t *p_info = (BAP_ASE_State_Params_t *)pNotification->pInfo;
      APP_ASE_Info_t *p_ase = 0;
      APP_ACL_Conn_t *p_conn = 0;
      LOG_INFO_APP("Metadata of ASE ID %d with remote CAP device on ConnHandle 0x%04X is updated\n",
                  p_info->ASE_ID,
                  pNotification->ConnHandle);
      p_conn = APP_GetACLConn(pNotification->ConnHandle);
      if (p_conn != 0)
      {
        p_ase = APP_GetASE(p_info->ASE_ID,pNotification->ConnHandle);
        if (p_ase != 0)
        {
          if ((p_info->State == ASE_STATE_ENABLING) || (p_info->State == ASE_STATE_STREAMING))
          {
            /*Parse the Metadata*/
            APP_ParseMetadataParams(p_ase,
                                    pNotification->ConnHandle,
                                    p_info->p.pEnStrDisConf->pMetadata,
                                    p_info->p.pEnStrDisConf->MetadataLength);
          }
        }
      }
      break;
    }

    case CAP_UNICAST_SERVER_ENABLE_REQ_EVT:
    {
      BAP_ASE_Enable_Req_Params_t *p_info = (BAP_ASE_Enable_Req_Params_t *)pNotification->pInfo;
      tBleStatus ret;
      APP_ASE_Info_t *p_ase = 0;
      APP_ACL_Conn_t *p_conn = 0;
      APP_ACL_Conn_t *p_other_conn = 0;
      Audio_Context_t audio_context = 0x0000;
      Audio_Context_t snk_context = GMAPAPP_Context.AvailableSnkAudioContext;
      Audio_Context_t src_context = GMAPAPP_Context.AvailableSrcAudioContext;

      LOG_INFO_APP("Enable Operation Request for ASE ID %d in state %d is received from remote CAP Initiator on ConnHandle 0x%04X\n",
                  p_info->ASE_ID,
                  p_info->State,
                  pNotification->ConnHandle);
      p_conn = APP_GetACLConn(pNotification->ConnHandle);
      if (p_conn != 0)
      {
        uint8_t status = LTV_GetStreamingAudioContexts((uint8_t *)p_info->pMetadata,
                                                       p_info->MetadataLength,
                                                       &audio_context);
        if(status == 1u)
        {
          LOG_INFO_APP("ASE ID 0x%02X on ConnHandle 0x%04X would be associated to a Streaming Audio Contexts 0x%04X\n",
                       p_info->ASE_ID,
                       pNotification->ConnHandle,
                       audio_context);
          /* Set audio contexts to removed from availability according to the streaming audio contexts associated to the
           * ASE.
           */
          if (p_info->Type == ASE_SINK)
          {
            /* Remove the contexts which are not available according to Streaming Audio contexts of the ASE */
            snk_context = APP_UnavailableAudioContexts(ASE_SINK,snk_context,audio_context);
          }
          else /*(p_info->Type & ASE_SOURCE)*/
          {
            /* Remove the contexts which are not available according to Streaming Audio contexts of the ASE */
            src_context = APP_UnavailableAudioContexts(ASE_SOURCE,src_context,audio_context);
          }

          LOG_INFO_APP("Previous Available Contexts are : Snk 0x%04X  - Src  0x%04X\n",
                       GMAPAPP_Context.AvailableSnkAudioContext,
                       GMAPAPP_Context.AvailableSrcAudioContext);
          LOG_INFO_APP("Calculated New Available Contexts are : Snk 0x%04X  - Src  0x%04X\n",
                       snk_context,
                       src_context);

          /*Set new available audio contexts for BAP Announcement*/
          status = CAP_SetAvailableAudioContexts(snk_context,src_context);
          if (status == BLE_STATUS_SUCCESS)
          {
            if (p_info->Type == ASE_SINK)
            {
              GMAPAPP_Context.AvailableSnkAudioContext = snk_context;
            }
            else
            {
              GMAPAPP_Context.AvailableSrcAudioContext = src_context;
            }
            LOG_INFO_APP("Set Available Snk Audio Contexts 0x%04X and Available Src Audio Contexts 0x%04X\n",
                         GMAPAPP_Context.AvailableSnkAudioContext,
                         GMAPAPP_Context.AvailableSrcAudioContext);
          }
          else
          {
            LOG_INFO_APP("FAILURE to Set Available Snk Audio Contexts 0x%04X and Available Src Audio Contexts 0x%04X\n",
                         snk_context,
                         src_context);
          }
          if (GMAPAPP_Context.NumConn < CFG_BLE_NUM_LINK )
          {
            GMAPAPP_StopAdvertising();
            /* Start Advertising */
            if (GMAPAPP_Context.CSIPRank > 0)
            {
              status = GMAPAPP_StartAdvertising(CAP_GENERAL_ANNOUNCEMENT, 0, GAP_APPEARANCE_EARBUD);
            }
            else
            {
              status = GMAPAPP_StartAdvertising(CAP_GENERAL_ANNOUNCEMENT, 0, GAP_APPEARANCE_HEADPHONES);
            }
            LOG_INFO_APP("GMAPAPP_StartAdvertising() returns status 0x%02X\n",status);
          }

        }
        else
        {
          /*No Streaming Audio Context*/
        }

        if (GMAPAPP_Context.NumConn == 1u )
        {
          /* If only one CAP Initiator is connected, we can accept directly the Enable Operation Request*/
          p_ase = APP_GetASE(p_info->ASE_ID,pNotification->ConnHandle);
          if (p_ase != 0)
          {
            ret = CAP_Unicast_EnableOpRsp(pNotification->ConnHandle,
                                      p_info->ASE_ID,
                                      ASE_OP_RESP_SUCCESS,
                                      ASE_OP_RESP_NO_REASON);
            LOG_INFO_APP("Enable Operation Response returns status 0x%02X for ASE ID %d on ConnHandle 0x%04X\n",
                        ret,
                        p_info->ASE_ID,
                        pNotification->ConnHandle);
          }
        }
        else
        {
          uint8_t delay_resp = 0u;

          if ((p_conn->AvailableSnkAudioContext != SUPPORTED_SNK_CONTEXTS) \
              || (p_conn->AvailableSrcAudioContext != SUPPORTED_SRC_CONTEXTS))
          {
            /*update the available Audio Contexts of the CAP Initiator requesting the enable operation*/
            ret = CAP_UpdateAvailableAudioContexts(pNotification->ConnHandle,
                                                   SUPPORTED_SNK_CONTEXTS,
                                                   SUPPORTED_SRC_CONTEXTS);
            LOG_INFO_APP("Send Update Available Snk Audio Contexts 0x%04X and Available Src Audio Contexts 0x%04X with CAP Initiator on ConnHandle 0x%04X returns status 0x%02X\n",
                         SUPPORTED_SNK_CONTEXTS,
                         SUPPORTED_SRC_CONTEXTS,
                         pNotification->ConnHandle,
                         ret);
            p_conn->AvailableSnkAudioContext = SUPPORTED_SNK_CONTEXTS;
            p_conn->AvailableSrcAudioContext = SUPPORTED_SRC_CONTEXTS;
            UNUSED(status);
          }
          else
          {
            LOG_INFO_APP("Available Snk Audio Contexts 0x%04X and Available Src Audio Contexts 0x%04X with CAP Initiator on ConnHandle 0x%04X haven't changed\n",
                         SUPPORTED_SNK_CONTEXTS,
                         SUPPORTED_SRC_CONTEXTS,
                         p_conn->Acl_Conn_Handle);
          }

          /*check if streaming is ongoing on another connection. If yes, the CAP Acceptor terminates the Audio Stream*/
          for (uint8_t conn = 0u; conn < CFG_BLE_NUM_LINK ; conn++)
          {
            uint8_t streaming_ongoing = 0u;
            if ((GMAPAPP_Context.ACL_Conn[conn].Acl_Conn_Handle != 0xFFFFu) && (&GMAPAPP_Context.ACL_Conn[conn] != p_conn))
            {
              p_other_conn = &GMAPAPP_Context.ACL_Conn[conn];
#if (APP_NUM_SNK_ASE > 0u)
              for ( uint8_t i = 0; i < GMAPAPP_Context.NumSnkASEs;i++)
              {
                if (p_other_conn->pASEs->aSnkASE[i].allocated == 1)
                {
                  if ((p_other_conn->pASEs->aSnkASE[i].state == ASE_STATE_ENABLING) \
                    || (p_other_conn->pASEs->aSnkASE[i].state == ASE_STATE_STREAMING))
                  {
                    streaming_ongoing = 1u;
                  }
                }
              }
#endif /* (APP_NUM_SNK_ASE > 0u) */
#if (APP_NUM_SRC_ASE > 0u)
              for ( uint8_t i = 0; i < GMAPAPP_Context.NumSrcASEs;i++)
              {
                if (p_other_conn->pASEs->aSrcASE[i].allocated == 1)
                {
                  if ((p_other_conn->pASEs->aSrcASE[i].state == ASE_STATE_ENABLING) \
                    || (p_other_conn->pASEs->aSrcASE[i].state == ASE_STATE_STREAMING))
                  {
                    streaming_ongoing = 1u;
                  }
                }
              }
#endif /* (APP_NUM_SRC_ASE > 0u) */

              if ((p_other_conn->AvailableSnkAudioContext != GMAPAPP_Context.AvailableSnkAudioContext) \
                  || (p_other_conn->AvailableSrcAudioContext != GMAPAPP_Context.AvailableSrcAudioContext))
              {
                /*we update the Audio Contexts availability to other connected CAP Initiator*/
                ret = CAP_UpdateAvailableAudioContexts(p_other_conn->Acl_Conn_Handle,
                                                       GMAPAPP_Context.AvailableSnkAudioContext,
                                                       GMAPAPP_Context.AvailableSrcAudioContext);
                LOG_INFO_APP("Send Update Available Snk Audio Contexts 0x%04X and Available Src Audio Contexts 0x%04X with CAP Initiator on ConnHandle 0x%04X returns status 0x%02X\n",
                             GMAPAPP_Context.AvailableSnkAudioContext,
                             GMAPAPP_Context.AvailableSrcAudioContext,
                             p_other_conn->Acl_Conn_Handle,
                             ret);
                p_other_conn->AvailableSnkAudioContext = GMAPAPP_Context.AvailableSnkAudioContext;
                p_other_conn->AvailableSrcAudioContext = GMAPAPP_Context.AvailableSrcAudioContext;
                UNUSED(status);
              }
              else
              {
                LOG_INFO_APP("Available Snk Audio Contexts 0x%04X and Available Src Audio Contexts 0x%04X with CAP Initiator on ConnHandle 0x%04X haven't changed\n",
                             GMAPAPP_Context.AvailableSnkAudioContext,
                             GMAPAPP_Context.AvailableSrcAudioContext,
                             p_other_conn->Acl_Conn_Handle);
              }
              if ( streaming_ongoing == 1u)
              {
                delay_resp = 1u;
                p_ase = APP_GetASE(p_info->ASE_ID,pNotification->ConnHandle);
                if (p_ase != 0)
                {
                  p_ase->enable_req = 1u;
                  LOG_INFO_APP("Delay Enable Operation Response for ASE ID %d on ConnHandle 0x%04X\n",
                              p_info->ASE_ID,
                              pNotification->ConnHandle);
                }
                LOG_INFO_APP("Terminate Audio Stream with CAP Initiator on ConnHandle 0x%04X shall be terminated before send a response to the Enable Request\n");
                /*Terminate the streaming with this device*/
                ret = CAP_Unicast_TerminateAudioStream(p_other_conn->Acl_Conn_Handle);
                LOG_INFO_APP("Terminate Audio Stream with CAP Initiator on ConnHandle 0x%04X returns status 0x%02X\n",
                             p_other_conn->Acl_Conn_Handle,
                             ret);

              }
              UNUSED(ret);
            }
          }
          /* Check if Audio Stream termination is in progress or not to send a response to an Enable Operation */
          if (delay_resp == 0u)
          {
            p_ase = APP_GetASE(p_info->ASE_ID,pNotification->ConnHandle);
            if (p_ase != 0)
            {
              ret = CAP_Unicast_EnableOpRsp(pNotification->ConnHandle,
                                            p_info->ASE_ID,
                                            ASE_OP_RESP_SUCCESS,
                                            ASE_OP_RESP_NO_REASON);
              LOG_INFO_APP("Enable Operation Response returns status 0x%02X for ASE ID %d on ConnHandle 0x%04X\n",
                          ret,
                          p_info->ASE_ID,
                          pNotification->ConnHandle);
            }
          }
        }
      }
      UNUSED(ret);
      break;
    }

    case CAP_UNICAST_PREFERRED_QOS_REQ_EVT:
    {
      BAP_PrefQoSConfReq_t *info = (BAP_PrefQoSConfReq_t *)pNotification->pInfo;
      Sampling_Freq_t frequency;
      Frame_Duration_t frame_duration;
      uint8_t blocksPerSDU;
      uint16_t sduInterval;
      uint16_t octetsPerFrame;
      uint8_t i;
      uint8_t offset = 0;
      uint32_t controller_delay_min;
      uint8_t num_snk_ases = GMAPAPP_Context.NumSnkASEs;
      uint8_t num_src_ases = GMAPAPP_Context.NumSrcASEs;

      LOG_INFO_APP("Preferred Server QoS Settings is requested for ASE ID %d (Type 0x%02X)\n",info->ASE_ID,info->Type);
      LOG_INFO_APP("  Target Latency : %d\n", info->TargetLatency);
      LOG_INFO_APP("  Target Phy %d to achieve the Target Latency\n",info->TargetPhy);
      LOG_INFO_APP("  Codec ID\n");
      LOG_INFO_APP("    Coding format : 0x%02X\n",info->CodecConf.CodecID.CodingFormat);
      LOG_INFO_APP("    Company id : 0x%04X\n",info->CodecConf.CodecID.CompanyID);
      LOG_INFO_APP("    VS codec id : 0x%04X\n",info->CodecConf.CodecID.VsCodecID);
      frequency = LTV_GetConfiguredSamplingFrequency(info->CodecConf.pSpecificConf,info->CodecConf.SpecificConfLength);
      LOG_INFO_APP("    Frequency : %d\n", frequency);
      frame_duration = LTV_GetConfiguredFrameDuration(info->CodecConf.pSpecificConf,info->CodecConf.SpecificConfLength);
      LOG_INFO_APP("    Frame Duration : %d\n", frame_duration);
      octetsPerFrame = LTV_GetConfiguredOctetsPerCodecFrame(info->CodecConf.pSpecificConf,info->CodecConf.SpecificConfLength);
      LOG_INFO_APP("    Octets Per Codec Frame : %d\n", octetsPerFrame);
      blocksPerSDU = LTV_GetConfiguredFramesBlockPerSdu(info->CodecConf.pSpecificConf,info->CodecConf.SpecificConfLength);
      if (blocksPerSDU == 0)
      {
        blocksPerSDU = 1; /* default value */
      }
      LOG_INFO_APP("    Codec Frame Blocks Per SDU : %d\n",blocksPerSDU);
      if (frame_duration == FRAME_DURATION_7_5_MS)
      {
        if (frequency == SAMPLE_FREQ_44100_HZ)
        {
          sduInterval = 8163*blocksPerSDU;
        }
        else
        {
          sduInterval = 7500*blocksPerSDU;
        }
      }
      else if (frame_duration == FRAME_DURATION_10_MS)
      {
        if (frequency == SAMPLE_FREQ_44100_HZ)
        {
          sduInterval = 10884*blocksPerSDU;
        }
        else
        {
          sduInterval = 10000*blocksPerSDU;
        }
      }
      else
      {
        LOG_INFO_APP("Unsupported frame duration\n",frame_duration);
        *(info->pResp) = ASE_OP_RESP_UNSUPPORTED_CONF_PARAM;
        *(info->pReason) = ASE_OP_RESP_REASON_CODEC_SPEC_CONF;
        return;
      }
      LOG_INFO_APP("  Min Controller Delay in Source role %d us\n",info->SrcControllerDelayMin);
      LOG_INFO_APP("  Max Controller Delay in Source role %d us\n",info->SrcControllerDelayMax);
      LOG_INFO_APP("  Min Controller Delay in Sink role %d us\n",info->SnkControllerDelayMin);
      LOG_INFO_APP("  Max Controller Delay in Sink role %d us\n",info->SnkControllerDelayMax);

      LOG_INFO_APP("Calculated SDU Interval : %d us\n",sduInterval);

      *(info->pResp) = ASE_OP_RESP_SUCCESS;
      *(info->pReason) = ASE_OP_RESP_NO_REASON;
      info->pPrefQoSConfRsp->Framing = APP_DEFAULT_BAP_FRAMING;
      info->pPrefQoSConfRsp->PrefPHY = APP_DEFAULT_BAP_PREF_PHY;
      info->pPrefQoSConfRsp->PrefRetransmissionNumber = APP_DEFAULT_BAP_PREF_RETRANSMISSIONS;
      info->pPrefQoSConfRsp->MaxTransportLatency = APP_DEFAULT_BAP_MAX_TRANSPORT_LATENCY;
#if (ENABLE_AUDIO_HIGH_RELIABILITY == 1u)
      if(info->TargetLatency > TARGET_LATENCY_BALANCED_RELIABILITY)
      {
        offset = 16;
      }
#endif /* (ENABLE_AUDIO_HIGH_RELIABILITY == 1u) */
      for(i = 0;i<15;i++)
      {
        if((frequency == APP_QoSConf[i+offset].freq) \
            && (sduInterval == APP_QoSConf[i+offset].sdu_interval) \
            && (octetsPerFrame == APP_QoSConf[i+offset].max_sdu))
        {
          info->pPrefQoSConfRsp->PrefRetransmissionNumber = APP_QoSConf[i+offset].rtx_num;
          info->pPrefQoSConfRsp->MaxTransportLatency = APP_QoSConf[i+offset].max_tp_latency;
        }
      }
      /*maximum simulateous Sink ASEs in streaming is 2 sink ASEs*/
      if (num_snk_ases > 2u)
      {
        num_snk_ases = 2u;
      }
      /*maximum simulateous Source ASEs in streaming is 2 source ASEs*/
      if (num_src_ases > 2u)
      {
        num_src_ases = 2u;
      }

      /* Adjust the minimum controller delay according to the fact that other ASEs could be in streaming in the
       * same time that the specified ASE
       */
      controller_delay_min = (num_snk_ases * info->SnkControllerDelayMin) + (num_src_ases * info->SrcControllerDelayMin);

      /* Remove extra margins added by the codec manager since they could to be considered only one time but not for each ASE */
      if ((num_snk_ases + num_src_ases) > 1)
      {
        uint32_t controller_delay_margin_src = 0;
        uint32_t controller_delay_margin_snk = 0;
        uint32_t max_controller_delay;
        uint8_t a_codec_id[5] = {0x00,0x00,0x00,0x00,0x00};
        a_codec_id[0] = AUDIO_CODING_FORMAT_LC3;

        hci_read_local_supported_controller_delay((const uint8_t*)&a_codec_id,
                                                   0x02,
                                                   DATA_PATH_OUTPUT,
                                                   0,
                                                   NULL,
                                                   (uint8_t*)&controller_delay_margin_snk,
                                                   (uint8_t*)&max_controller_delay);

         hci_read_local_supported_controller_delay((const uint8_t*)&a_codec_id,
                                                    0x02,
                                                    DATA_PATH_INPUT,
                                                    0,
                                                    NULL,
                                                    (uint8_t*)&controller_delay_margin_src,
                                                    (uint8_t*)&max_controller_delay);
        if (num_src_ases > 0)
        {
          if (num_src_ases > 1)
          {
            controller_delay_min = controller_delay_min - (controller_delay_margin_src * (num_src_ases - 1));
          }
          /* if a source is present, processing margin are already included for that frame duration */
          controller_delay_min = controller_delay_min - (controller_delay_margin_snk * num_snk_ases);
        }
        else
        {
          if( num_snk_ases > 0)
          {
            controller_delay_min = controller_delay_min - (controller_delay_margin_snk * (num_snk_ases - 1));
          }
        }
      }

      LOG_INFO_APP("Computed Min Controller Delay with %d Snk ASEs and %d Src ASEs : %d us\n",
                   num_snk_ases,
                   num_src_ases,
                   controller_delay_min);

      /*check if computed minimum controller delay is compliant with the frame */
      if (frame_duration == FRAME_DURATION_7_5_MS)
      {
        if(controller_delay_min > sduInterval)
        {
          LOG_INFO_APP("WARNING : Computed Controller Delay Min ( %d us) could be higher than the frame duration (7.5 ms)\n",
                      controller_delay_min);
          controller_delay_min = sduInterval;
        }
      }
      else if (frame_duration == FRAME_DURATION_10_MS)
      {
        if(controller_delay_min > sduInterval)
        {
          LOG_INFO_APP("WARNING : Controller Delay Min ( %d us) could be higher than the frame duration (10 ms)\n",
                      controller_delay_min);
          controller_delay_min = sduInterval;
        }
      }

      /*Calculate presentation delay to submitt to remote unicat client*/
      if (*(info->pResp) == ASE_OP_RESP_SUCCESS)
      {
        if (info->Type ==  ASE_SINK)
        {
          /* For the Unicast Server, the supported Presentation_Delay range in the Codec Configured state shall
           * include BAP_MANDATORY_PRESENTATION_DELAY when the ASE is a Sink ASE
           */
          if (((APP_DELAY_SNK_MIN+controller_delay_min) > BAP_MANDATORY_PRESENTATION_DELAY) \
              || ((APP_DELAY_SNK_MAX+info->SnkControllerDelayMax) < BAP_MANDATORY_PRESENTATION_DELAY))
          {
            LOG_INFO_APP("ERROR : Mandatory presentation %d us is not included in supported Presentation Delay range [%d - %d]\n",
                        BAP_MANDATORY_PRESENTATION_DELAY,
                          APP_DELAY_SNK_MIN+controller_delay_min,
                          APP_DELAY_SNK_MAX+info->SnkControllerDelayMax);
            *(info->pResp) = ASE_OP_RESP_UNSUPPORTED_CONF_PARAM;
            *(info->pReason) = ASE_OP_RESP_REASON_PRES_DELAY;
          }
          info->pPrefQoSConfRsp->PresentationDelayMin = (APP_DELAY_SNK_MIN+controller_delay_min);
          info->pPrefQoSConfRsp->PresentationDelayMax = (APP_DELAY_SNK_MAX+info->SnkControllerDelayMax);
          info->pPrefQoSConfRsp->PrefPresentationDelayMin = (APP_DELAY_SNK_MIN+controller_delay_min);
          info->pPrefQoSConfRsp->PrefPresentationDelayMax = (APP_DELAY_SNK_MAX+info->SnkControllerDelayMax);
        }
        else if (info->Type ==  ASE_SOURCE)
        {
          info->pPrefQoSConfRsp->PresentationDelayMin = (APP_DELAY_SRC_MIN+controller_delay_min);
          info->pPrefQoSConfRsp->PresentationDelayMax = (APP_DELAY_SRC_MAX+info->SrcControllerDelayMax);
          info->pPrefQoSConfRsp->PrefPresentationDelayMin = (APP_DELAY_SRC_MIN+controller_delay_min);
          info->pPrefQoSConfRsp->PrefPresentationDelayMax = (APP_DELAY_SRC_MAX+info->SrcControllerDelayMax);
        }
        LOG_INFO_APP("Submitted Min Presentation Delay %d us\n",info->pPrefQoSConfRsp->PresentationDelayMin);
        LOG_INFO_APP("Submitted Max Presentation Delay %d us\n",info->pPrefQoSConfRsp->PresentationDelayMax);
      }
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

      /* BAP_ROLE_UNICAST_SERVER */
      if (info->PathDirection == BAP_AUDIO_PATH_INPUT)
      {
        /* Source for Unicast Server*/
        controller_delay = info->PresentationDelay - APP_DELAY_SRC_MIN;
        if ( controller_delay > info->ControllerDelayMax)
        {
          controller_delay = info->ControllerDelayMax;
        }
      }
      else /* (info->PathDirection == BAP_AUDIO_PATH_OUTPUT)*/
      {
        /* Sink for Unicast Server*/
        controller_delay = info->PresentationDelay - APP_DELAY_SNK_MIN;
        if ( controller_delay > info->ControllerDelayMax)
        {
          controller_delay = info->ControllerDelayMax;
        }
      }

      p_ase = APP_GetASE(info->ASE_ID,pNotification->ConnHandle);
      if (p_ase != 0)
      {
        p_ase->controller_delay = controller_delay;
        p_ase->presentation_delay = info->PresentationDelay;
      }
      APP_UnicastSetupAudioDataPath(pNotification->ConnHandle,info->CIS_ConnHandle,info->ASE_ID,controller_delay);
      break;
    }

    case CAP_AUDIO_CLOCK_REQ_EVT:
    {
      Sampling_Freq_t *freq = (Sampling_Freq_t *)pNotification->pInfo;
      LOG_INFO_APP("Init Audio Clock with freq %d\n",*freq);
      AudioClock_Init(*freq);
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
        GMAPAPP_Context.NumOutputChannels += APP_GetBitsAudioChnlAllocations(channel_alloc);
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
        GMAPAPP_Context.NumOutputChannels -= APP_GetBitsAudioChnlAllocations(channel_alloc);
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

    case CAP_VCP_META_EVT:
    {
      VCP_Notification_Evt_t *p_vcp_evt = (VCP_Notification_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Volume Control Meta Event 0x%02X is received on ConnHandle 0x%04X:\n",
                  p_vcp_evt->EvtOpcode,
                  p_vcp_evt->ConnHandle);
#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)
      VCP_MetaEvt_Notification(p_vcp_evt);
#endif /*(APP_VCP_ROLE_RENDERER_SUPPORT == 1u)*/
      break;
    }

    case CAP_MICP_META_EVT:
    {
      MICP_Notification_Evt_t *p_micp_evt = (MICP_Notification_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Microphone Control Meta Event 0x%02X is received on ConnHandle 0x%04X:\n",
                  p_micp_evt->EvtOpcode,
                  p_micp_evt->ConnHandle);
#if (APP_MICP_ROLE_DEVICE_SUPPORT == 1u)
      MICP_MetaEvt_Notification(p_micp_evt);
#endif /*(APP_MICP_ROLE_DEVICE_SUPPORT == 1u)*/
      break;
    }

#if ((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)
    case CAP_BROADCAST_AUDIO_UP_EVT:
    {
      Sampling_Freq_t sampling_freq;
      char freq_text[8] = {0};
      LOG_INFO_APP(">>== CAP_BROADCAST_AUDIO_UP_EVT\n");

      sampling_freq = LTV_GetConfiguredSamplingFrequency(
          &GMAPAPP_Context.BSNK.base_group.pSubgroups[0].pCodecSpecificConf[0],
          GMAPAPP_Context.BSNK.base_group.pSubgroups[0].CodecSpecificConfLength);

      switch (sampling_freq)
      {
        case SAMPLE_FREQ_8000_HZ:
        {
          strncpy(&freq_text[0], "8KHz",5u);
          break;
        }
        case SAMPLE_FREQ_16000_HZ:
        {
          strncpy(&freq_text[0], "16KHz",6u);
          break;
        }
        case SAMPLE_FREQ_24000_HZ:
        {
          strncpy(&freq_text[0], "24KHz",6u);
          break;
        }
        case SAMPLE_FREQ_32000_HZ:
        {
          strncpy(&freq_text[0], "32KHz",6u);
          break;
        }
        case SAMPLE_FREQ_44100_HZ:
        {
          strncpy(&freq_text[0], "44.1KHz",8u);
          break;
        }
        case SAMPLE_FREQ_48000_HZ:
        {
          strncpy(&freq_text[0], "48KHz",6u);
          break;
        }
        default:
        {
          strncpy(&freq_text[0], "Unknown",8u);
          break;
        }
      }
      Menu_SetBroadcastSyncedPage(freq_text);
      break;
    }

    case CAP_BROADCAST_AUDIO_DOWN_EVT:
    {
      LOG_INFO_APP(">>== CAP_BROADCAST_AUDIO_DOWN_EVT\n");
      Stop_TxAudio();
      MX_AudioDeInit();

      if (GMAPAPP_Context.BroadcastMode == APP_BROADCAST_MODE_SINK_ONLY)
      {
        Menu_SetBroadcastScanPage();
        GMAPAPP_StopSink();
        GMAPAPP_StartSink();
      }
      else
      {
        Menu_SetNoStreamPage();
      }
      break;
    }

    case CAP_BROADCAST_SOURCE_ADV_REPORT_EVT:
    {
      BAP_Broadcast_Source_Adv_Report_Data_t *data = (BAP_Broadcast_Source_Adv_Report_Data_t*) pNotification->pInfo;
      LOG_INFO_APP(">>== CAP_BROADCAST_SOURCE_ADV_REPORT_EVT\n");
      LOG_INFO_APP("     - Broadcasting Device with address %02X:%02X:%02X:%02X:%02X:%02X\n",
                  data->pAdvAddress[5],
                  data->pAdvAddress[4],
                  data->pAdvAddress[3],
                  data->pAdvAddress[2],
                  data->pAdvAddress[1],
                  data->pAdvAddress[0]);
      if(GMAPAPP_Context.BSNK.PASyncState == APP_PA_SYNC_STATE_IDLE
         && GMAPAPP_Context.BSNK.BIGSyncState == APP_BIG_SYNC_STATE_IDLE)
      {
        uint8_t parse_index = 0;
        char name[30] = "Unknown";
        char bid[12] = "ID:0x\0";
        const uint8_t* name_ptr = 0;
        uint8_t name_len = 0;

        while (parse_index + 3 < data->AdvertisingDataLength)
        {
          /* First priority to the Broadcast Name */
          if (data->pAdvertisingData[parse_index + 1] == 0x30)
          {
            name_ptr = &data->pAdvertisingData[parse_index + 2];
            name_len = MIN(data->pAdvertisingData[parse_index] - 1, 29);
            break;
          }
          /* Second priority to the Complete Local Name */
          else if (data->pAdvertisingData[parse_index + 1] == AD_TYPE_COMPLETE_LOCAL_NAME)
          {
            name_ptr = &data->pAdvertisingData[parse_index + 2];
            name_len = MIN(data->pAdvertisingData[parse_index] - 1, 29);
          }
          /* Third priority to the Broadcast ID */
          else if (data->pAdvertisingData[parse_index + 1] == AD_TYPE_SERVICE_DATA &&
              data->pAdvertisingData[parse_index + 2] == 0x52 &&
              data->pAdvertisingData[parse_index + 3] == 0x18)
          {
            sprintf(&bid[5],"%X", data->pAdvertisingData[parse_index + 6]);
            sprintf(&bid[7],"%X", data->pAdvertisingData[parse_index + 5]);
            sprintf(&bid[9],"%X",data->pAdvertisingData[parse_index + 4]);
          }
          parse_index += data->pAdvertisingData[parse_index] + 1;
        }
        if (name_ptr != 0)
        {
          UTIL_MEM_cpy_8(&name[0], name_ptr, name_len);
          name[name_len] = '\0';
        }
        else if (bid[5] != '\0')
        {
          UTIL_MEM_cpy_8(&name[0], bid, 11);
          name[11] = '\0';
        }

#if (CFG_TEST_VALIDATION == 1u)
        if (memcmp(&name[0], "GMAP_WBA", 8u) != 0)
        {
          return;
        }
#endif /*(CFG_TEST_VALIDATION == 1u)*/

        Menu_AddBroadcastSource(data->AdvSID, (uint8_t *) &data->pAdvAddress[0],
                                data->AdvAddressType, &name[0]);
      }
      break;
    }

    case CAP_BROADCAST_PA_SYNC_ESTABLISHED_EVT:
    {
      tBleStatus ret;
      LOG_INFO_APP(">>== CAP_BROADCAST_PA_SYNC_ESTABLISHED_EVT\n");
      BAP_PA_Sync_Established_Data_t *data = (BAP_PA_Sync_Established_Data_t*) pNotification->pInfo;
      LOG_INFO_APP("     - Status : 0x%02x\n",pNotification->Status);
      LOG_INFO_APP("     - SyncHandle : 0x%02x\n",data->SyncHandle);
      GMAPAPP_Context.BSNK.PASyncHandle = data->SyncHandle;
      GMAPAPP_Context.BSNK.PASyncState = APP_PA_SYNC_STATE_SYNCHRONIZED;

      ret = CAP_Broadcast_StopAdvReportParsing();
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : CAP_Broadcast_StopAdvReportParsing() function, result: 0x%02X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: CAP_Broadcast_StopAdvReportParsing() function\n");
      }
      ret = aci_gap_terminate_gap_proc(GAP_OBSERVATION_PROC);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gap_terminate_gap_proc() function, result: 0x%02X\n", ret);
      }
      else
      {
        GMAPAPP_Context.BSNK.ScanState = APP_SCAN_STATE_IDLE;
        LOG_INFO_APP("  Success: aci_gap_terminate_gap_proc() function\n");
      }
      break;
    }

    case CAP_BROADCAST_PA_SYNC_LOST_EVT:
      LOG_INFO_APP(">>== CAP_BROADCAST_PA_SYNC_LOST_EVT\n");
      GMAPAPP_Context.BSNK.PASyncState = APP_PA_SYNC_STATE_IDLE;
    break;

    case CAP_BROADCAST_BASE_REPORT_EVT:
    {
      BAP_BASE_Report_Data_t *base_data;
      uint8_t status;
      uint8_t index = 0;
      uint8_t i;
      uint8_t j;
      uint8_t k;
      uint8_t l;
      uint8_t is_different;
      uint8_t payload_len = 0;
      LOG_INFO_APP(">>== CAP_BROADCAST_BASE_REPORT_EVT\n");

      base_data = (BAP_BASE_Report_Data_t*) pNotification->pInfo;
      if (GMAPAPP_Context.BSNK.PASyncState == APP_PA_SYNC_STATE_SYNCHRONIZED)
      {
        GMAPAPP_Context.BSNK.PASyncState = APP_PA_SYNC_STATE_BASE_RECEIVED;
        /* First BASE report after PA sync, force the parsing */
        is_different = 0x01u;
      }
      else
      {
        is_different = CAP_Broadcast_IsBASEGroupDifferent(base_data->pBasePayload,
                                              base_data->BasePayloadLength,
                                              &(GMAPAPP_Context.BSNK.base_group),
                                              &(index));
      }
      if (is_different == 0x00u)
      {
        payload_len += index;
        for (i = 0; i < GMAPAPP_Context.BSNK.base_group.NumSubgroups && is_different == 0x00u; i++)
        {
          is_different = CAP_Broadcast_IsBASESubgroupDifferent(base_data->pBasePayload + payload_len,
                                                    base_data->BasePayloadLength - payload_len,
                                                    &(GMAPAPP_Context.BSNK.base_subgroups[i]),
                                                    &(index));
          payload_len += index;
          for (j = 0; (j < GMAPAPP_Context.BSNK.base_subgroups[i].NumBISes && is_different == 0x00u); j++)
          {
            is_different = CAP_Broadcast_IsBASEBISDifferent(base_data->pBasePayload + payload_len,
                                                          base_data->BasePayloadLength - payload_len,
                                                          &(GMAPAPP_Context.BSNK.base_bis[j]),
                                                          &(index));
            payload_len += index;
          }
        }
      }
      if (is_different == 0x00u)
      {
        /* BASE report is not different from the one in GMAPAPP_Context.BSNK */
        /* No need to parse, copy, or log it again*/
        break;
      }
      status = CAP_Broadcast_ParseBASEGroup(base_data->pBasePayload,
                                            base_data->BasePayloadLength,
                                            &(GMAPAPP_Context.BSNK.base_group),
                                            &(index));

      if (status == BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("==>> Start BAP BSNK Parse BASE Group INFO\n");
        LOG_INFO_APP("   Payload Len role : 0x%02x\n",base_data->BasePayloadLength);
        LOG_INFO_APP("   Presentation_delay: 0x%08x\n",GMAPAPP_Context.BSNK.base_group.PresentationDelay);
        LOG_INFO_APP("   Num_subgroups : 0x%02x\n",GMAPAPP_Context.BSNK.base_group.NumSubgroups);
        base_data->pBasePayload += index;
        base_data->BasePayloadLength -= index;

        if(GMAPAPP_Context.BSNK.BIGSyncState == APP_BIG_SYNC_STATE_IDLE)
        {
          GMAPAPP_Context.BSNK.num_sync_bis = 0;
        }

        /* Parse Subgroups */
        for (i = 0; i < GMAPAPP_Context.BSNK.base_group.NumSubgroups && status == BLE_STATUS_SUCCESS; i++)
        {
          status = CAP_Broadcast_ParseBASESubgroup(base_data->pBasePayload,
                                                    base_data->BasePayloadLength,
                                                    &(GMAPAPP_Context.BSNK.base_subgroups[i]),
                                                    &(index));
          base_data->pBasePayload += index;
          base_data->BasePayloadLength -= index;

          LOG_INFO_APP("    BAP_BSNK_ParseBASESubgroup INFO Number :%d\n", i);
          LOG_INFO_APP("    Codec ID : 0x%08x\n",GMAPAPP_Context.BSNK.base_subgroups[i].CodecID);
          LOG_INFO_APP("    Codec specific config length : %d bytes\n",
                      GMAPAPP_Context.BSNK.base_subgroups[i].CodecSpecificConfLength);
          if (GMAPAPP_Context.BSNK.base_subgroups[i].CodecSpecificConfLength > 0u)
          {
            for (k = 0;k<GMAPAPP_Context.BSNK.base_subgroups[i].CodecSpecificConfLength;k++)
            {
              if (GMAPAPP_Context.BSNK.base_subgroups[i].pCodecSpecificConf[k] > 0u)
              {
                LOG_INFO_APP("      Length: 0x%02x\n",GMAPAPP_Context.BSNK.base_subgroups[i].pCodecSpecificConf[k]);
                LOG_INFO_APP("        Type: 0x%02x\n",GMAPAPP_Context.BSNK.base_subgroups[i].pCodecSpecificConf[k+1u]);
                LOG_INFO_APP("        Value: 0x");
                for (l = 0 ;l<(GMAPAPP_Context.BSNK.base_subgroups[i].pCodecSpecificConf[k]-1);l++)
                {
                  LOG_INFO_APP("%02x",GMAPAPP_Context.BSNK.base_subgroups[i].pCodecSpecificConf[k+2u+l]);
                }
                LOG_INFO_APP("\n");
              }
              k+=GMAPAPP_Context.BSNK.base_subgroups[i].pCodecSpecificConf[k];
            }
          }
          LOG_INFO_APP("    Metadata length : %d bytes\n",GMAPAPP_Context.BSNK.base_subgroups[i].MetadataLength);
          if (GMAPAPP_Context.BSNK.base_subgroups[i].MetadataLength > 0)
          {
            for (k = 0;k<GMAPAPP_Context.BSNK.base_subgroups[i].MetadataLength;k++)
            {
              if (GMAPAPP_Context.BSNK.base_subgroups[i].pMetadata[k] > 0u)
              {
                LOG_INFO_APP("      Length: 0x%02x\n",GMAPAPP_Context.BSNK.base_subgroups[i].pMetadata[k]);
                LOG_INFO_APP("        Type: 0x%02x\n",GMAPAPP_Context.BSNK.base_subgroups[i].pMetadata[k+1u]);
                LOG_INFO_APP("        Value: 0x");
                for (l = 0 ;l<(GMAPAPP_Context.BSNK.base_subgroups[i].pMetadata[k]-1);l++)
                {
                  LOG_INFO_APP("%02x",GMAPAPP_Context.BSNK.base_subgroups[i].pMetadata[k+2u+l]);
                }
                LOG_INFO_APP("\n");
              }
              k+=GMAPAPP_Context.BSNK.base_subgroups[i].pMetadata[k];
            }
          }
          LOG_INFO_APP("    Num_BIS : %d\n",GMAPAPP_Context.BSNK.base_subgroups[i].NumBISes);

          /* Parse BIS */
          for (j = 0; (j < GMAPAPP_Context.BSNK.base_subgroups[i].NumBISes) && (status == BLE_STATUS_SUCCESS); j++)
          {
            status = CAP_Broadcast_ParseBASEBIS(base_data->pBasePayload,
                                                base_data->BasePayloadLength,
                                                &(GMAPAPP_Context.BSNK.base_bis[j]),
                                                &(index));
            base_data->pBasePayload += index;
            base_data->BasePayloadLength -= index;

            LOG_INFO_APP("      BIS INDEX : 0x%02x\n",GMAPAPP_Context.BSNK.base_bis[j].BIS_Index);
            LOG_INFO_APP("      Codec specific config length : %d bytes\n",GMAPAPP_Context.BSNK.base_bis[j].CodecSpecificConfLength);
            Audio_Chnl_Allocation_t channel_alloc = 0x00000000;
            if (GMAPAPP_Context.BSNK.base_bis[j].CodecSpecificConfLength > 0u)
            {
              for (int k = 0;k<GMAPAPP_Context.BSNK.base_bis[j].CodecSpecificConfLength;k++)
              {
                if (GMAPAPP_Context.BSNK.base_bis[j].pCodecSpecificConf[k] > 0u)
                {
                  LOG_INFO_APP("        Length: 0x%02x\n",GMAPAPP_Context.BSNK.base_bis[j].pCodecSpecificConf[k]);
                  LOG_INFO_APP("          Type: 0x%02x\n",GMAPAPP_Context.BSNK.base_bis[j].pCodecSpecificConf[k+1u]);
                  LOG_INFO_APP("          Value: 0x");
                  for (int l = 0 ;l<(GMAPAPP_Context.BSNK.base_bis[j].pCodecSpecificConf[k]-1);l++)
                  {
                    LOG_INFO_APP("%02x",GMAPAPP_Context.BSNK.base_bis[j].pCodecSpecificConf[k+2u+l]);
                  }
                  LOG_INFO_APP("\n");
                }
                k+=GMAPAPP_Context.BSNK.base_bis[j].pCodecSpecificConf[k];
              }
              channel_alloc = LTV_GetConfiguredAudioChannelAllocation(GMAPAPP_Context.BSNK.base_bis[j].pCodecSpecificConf,
                                                                      GMAPAPP_Context.BSNK.base_bis[j].CodecSpecificConfLength);
            }
            if (channel_alloc == 0x00000000)
            {
              /* No channel alloc on BIS level, get channel alloc on subgroup level */
              channel_alloc = LTV_GetConfiguredAudioChannelAllocation(GMAPAPP_Context.BSNK.base_subgroups[i].pCodecSpecificConf,
                                                                      GMAPAPP_Context.BSNK.base_subgroups[i].CodecSpecificConfLength);
            }
            if(channel_alloc != 0x00000000)
            {
              LOG_INFO_APP("      Audio Channels Allocation Configuration : 0x%08X\n",channel_alloc);
              LOG_INFO_APP("      Number of Audio Channels %d \n",APP_GetBitsAudioChnlAllocations(channel_alloc));
            }
            if(GMAPAPP_Context.BSNK.BIGSyncState == APP_BIG_SYNC_STATE_IDLE)
            {
              if (GMAPAPP_Context.BSNK.Audio_Location != 0x00000000)
              {
                /* check if the Channel allocation matches with the Sink Audio Location supported by the Broadcast Sink */
                if(channel_alloc != 0x00000000)
                {
                  if ((GMAPAPP_Context.BSNK.Audio_Location & channel_alloc) != 0x00000000)
                  {
                    GMAPAPP_Context.BSNK.sync_bis_index[GMAPAPP_Context.BSNK.num_sync_bis] = (j+1);
                    GMAPAPP_Context.BSNK.num_sync_bis++;
                  }
                }
                else
                {
                  GMAPAPP_Context.BSNK.sync_bis_index[GMAPAPP_Context.BSNK.num_sync_bis] = (j+1);
                  GMAPAPP_Context.BSNK.num_sync_bis++;
                }
              }
              else
              {
                GMAPAPP_Context.BSNK.sync_bis_index[GMAPAPP_Context.BSNK.num_sync_bis] = (j+1);
                GMAPAPP_Context.BSNK.num_sync_bis++;
              }
            }
          }
        }
        LOG_INFO_APP("==>> End Start BAP BSNK Parse BASE Group INFO\n");

        if (GMAPAPP_Context.BSNK.BIGSyncState == APP_BIG_SYNC_STATE_IDLE)
        {
          memcpy(&GMAPAPP_Context.BSNK.codec_specific_config_subgroup[0],
                  GMAPAPP_Context.BSNK.base_subgroups[0].pCodecSpecificConf,
                  GMAPAPP_Context.BSNK.base_subgroups[0].CodecSpecificConfLength);

          uint32_t freq = LTV_GetConfiguredSamplingFrequency(GMAPAPP_Context.BSNK.base_subgroups[0].pCodecSpecificConf,
                                                              GMAPAPP_Context.BSNK.base_subgroups[0].CodecSpecificConfLength);

          LOG_INFO_APP("==>> Audio Clock with Sample Frequency Type 0x%02X Initialization\n",freq);
          AudioClock_Init(freq);
        }
      }
      break;
    }

    case CAP_BROADCAST_BIGINFO_REPORT_EVT:
    {
      if (GMAPAPP_Context.BSNK.BIGSyncState == APP_BIG_SYNC_STATE_IDLE)
      {
        uint8_t status;
        BAP_BIGInfo_Report_Data_t *data = (BAP_BIGInfo_Report_Data_t *) pNotification->pInfo;

        LOG_INFO_APP("  CAP_Broadcast_StartBIGSync() function for %d BIEses:\n", GMAPAPP_Context.BSNK.num_sync_bis);
        for (uint8_t i = 0u; i < GMAPAPP_Context.BSNK.num_sync_bis;i++)
        {
          LOG_INFO_APP("  BIS Index %d\n", GMAPAPP_Context.BSNK.sync_bis_index[i]);
        }
        status = CAP_Broadcast_StartBIGSync(BIG_HANDLE,
                                      data->SyncHandle,
                                      &(GMAPAPP_Context.BSNK.sync_bis_index[0]),
                                      GMAPAPP_Context.BSNK.num_sync_bis,
                                      &(GMAPAPP_Context.BSNK.base_group),
                                      0u,
                                      BAP_BROADCAST_ENCRYPTION,
                                      aAPP_BroadcastCode,
                                      BIG_MSE,
                                      BIG_SYNC_TIMEOUT);
        if (status != BLE_STATUS_SUCCESS)
        {
          LOG_INFO_APP("  Fail   : CAP_Broadcast_StartBIGSync() function, result: 0x%02X\n", status);
        }
        else
        {
          LOG_INFO_APP("  Success: CAP_Broadcast_StartBIGSync() function\n");
          GMAPAPP_Context.BSNK.BIGSyncState = APP_BIG_SYNC_STATE_SYNCHRONIZING;
          Menu_SetBISSyncPage();
        }
      }
      break;
    }

    case CAP_BROADCAST_BIG_SYNC_ESTABLISHED_EVT:
    {
      tBleStatus ret;
      BAP_BIG_Sync_Established_Data_t *data = (BAP_BIG_Sync_Established_Data_t*) pNotification->pInfo;
      LOG_INFO_APP(">>== CAP_BROADCAST_BIG_SYNC_ESTABLISHED_EVT\n");
      LOG_INFO_APP("     - Status = 0x%02x\n",pNotification->Status);

      if (pNotification->Status == BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("     - BIG_Handle = 0x%02x\n",data->BIGHandle);
        LOG_INFO_APP("     - Num BISes = %d\n",data->NumBISes);
        UTIL_MEM_cpy_8(&(GMAPAPP_Context.BSNK.current_BIS_conn_handles[0]),
                       data->pConnHandle,
                       (data->NumBISes * sizeof(uint16_t)));
        GMAPAPP_Context.BSNK.current_num_bis = data->NumBISes;
        GMAPAPP_Context.BSNK.BIGSyncState = APP_BIG_SYNC_STATE_SYNCHRONIZED;

        if (GMAPAPP_Context.BroadcastMode == APP_BROADCAST_MODE_SINK_ONLY)
        {
          ret = CAP_Broadcast_StopPASync(GMAPAPP_Context.BSNK.PASyncHandle);
          if (ret != BLE_STATUS_SUCCESS)
          {
            LOG_INFO_APP("  Fail   : CAP_Broadcast_StopPASync() function, result: 0x%02X\n", ret);
          }
          else
          {
            LOG_INFO_APP("  Success: CAP_Broadcast_StopPASync() function\n");
            GMAPAPP_Context.BSNK.PASyncState = APP_PA_SYNC_STATE_IDLE;
          }
        }
        APP_BroadcastSetupAudio(AUDIO_ROLE_SINK);
      }
      else
      {
        GMAPAPP_Context.BSNK.BIGSyncState = APP_BIG_SYNC_STATE_IDLE;
        if (GMAPAPP_Context.BroadcastMode == APP_BROADCAST_MODE_SINK_ONLY)
        {
          Menu_SetBroadcastScanPage();
          GMAPAPP_StopSink();
          GMAPAPP_StartSink();
        }
        else
        {
          Menu_SetNoStreamPage();
        }
      }
      break;
    }

    case CAP_BROADCAST_BIG_SYNC_LOST_EVT:
    {
      LOG_INFO_APP(">>== CAP_BROADCAST_BIG_SYNC_LOST_EVT\n");
      GMAPAPP_Context.BSNK.BIGSyncState = APP_BIG_SYNC_STATE_IDLE;
      break;
    }

    case CAP_BROADCAST_SDE_ADD_SOURCE_REQ_EVT:
    {
      LOG_INFO_APP(">>== CAP_BROADCAST_SDE_ADD_SOURCE_REQ_EVT\n");

      CAP_Broadcast_AcceptAddBroadcastSource();
      break;
    }

    case CAP_BROADCAST_SDE_MODIFY_SOURCE_REQ_EVT:
    {
      LOG_INFO_APP(">>== CAP_BROADCAST_SDE_MODIFY_SOURCE_REQ_EVT\n");

      CAP_Broadcast_AcceptModifyBroadcastSource();
      break;
    }

    case CAP_BROADCAST_SDE_PA_SYNC_TERMINATED_EVT:
    {
      LOG_INFO_APP(">>== CAP_BROADCAST_SDE_PA_SYNC_TERMINATED_EVT\n");
      GMAPAPP_Context.BSNK.PASyncState = APP_PA_SYNC_STATE_IDLE;
      break;
    }

    case CAP_BROADCAST_SDE_BIG_SYNC_TERMINATED_EVT:
    {
      LOG_INFO_APP(">>== CAP_BROADCAST_SDE_BIG_SYNC_TERMINATED_EVT\n");
      GMAPAPP_Context.BSNK.BIGSyncState = APP_BIG_SYNC_STATE_IDLE;
      break;
    }

    case CAP_BROADCAST_SDE_START_SCAN_REQ_EVT:
    {
      uint8_t status;

      LOG_INFO_APP(">>== CAP_BROADCAST_SDE_START_SCAN_REQ_EVT\n");

      status = CAP_Broadcast_StartAdvReportParsing();

      if (status == BLE_STATUS_SUCCESS)
      {
        Scan_Param_Phy_t scan_param_phy;
        scan_param_phy.Scan_Type     = 0x00; /*Passive scanning*/
        scan_param_phy.Scan_Interval = SCAN_INTERVAL;
        scan_param_phy.Scan_Window   = SCAN_WINDOW;
        /* Starts an Observation procedure */
        status = aci_gap_ext_start_scan( 0x00,
                                        GAP_OBSERVATION_PROC,
                                        0x00,                         /* Address type: Public */
                                        0x00,                         /* Filter duplicates: No */
                                        0x00,                         /* Scan continuously until explicitly disable */
                                        0x00,                         /* Scan continuously */
                                        0x00,                         /* Filter policy: Accept all */
                                        HCI_SCANNING_PHYS_LE_1M,
                                        &scan_param_phy);
        if (status != BLE_STATUS_SUCCESS)
        {
          LOG_INFO_APP("  Fail   : aci_gap_ext_start_scan() function with Scan procedure 0x%02X, result: 0x%02X\n",
                       GAP_OBSERVATION_PROC,
                       status);
        }
        else
        {
          LOG_INFO_APP("  Success: aci_gap_ext_start_scan() function with Scan procedure 0x%02X\n",
                       GAP_OBSERVATION_PROC);
        }
      }

      *(pNotification->pInfo) = status;

      GMAPAPP_Context.BSNK.BIGSyncState = APP_BIG_SYNC_STATE_IDLE;
      break;
    }
#endif /*((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)*/

    default:
      break;
  }
}

#if ((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)
static uint8_t APP_BroadcastSetupAudio(Audio_Role_t role)
{
  Sampling_Freq_t sampling_freq;
  Frame_Duration_t frame_duration;
  uint8_t direction = DATA_PATH_INPUT;
  uint32_t controller_delay;
  uint32_t controller_delay_min = 0;
  uint32_t controller_delay_max = 0;
  uint8_t a_codec_id[5] = {0x00,0x00,0x00,0x00,0x00};
  tBleStatus ret;

  LOG_INFO_APP("==>> Start APP_BroadcastSetupAudio function\n");

  a_codec_id[0] = AUDIO_CODING_FORMAT_LC3;

  sampling_freq = LTV_GetConfiguredSamplingFrequency(
      &GMAPAPP_Context.BSNK.base_group.pSubgroups[0].pCodecSpecificConf[0],
      GMAPAPP_Context.BSNK.base_group.pSubgroups[0].CodecSpecificConfLength);

  frame_duration = LTV_GetConfiguredFrameDuration(
      &GMAPAPP_Context.BSNK.base_group.pSubgroups[0].pCodecSpecificConf[0],
      GMAPAPP_Context.BSNK.base_group.pSubgroups[0].CodecSpecificConfLength);

  if ((sampling_freq != 0) && (frame_duration != 0xFF))
  {
    GMAPAPP_Context.SinkDecimation = 1;
    APP_StartBroadcastAudio(role);
    MX_AudioInit(role,
                 sampling_freq,
                 frame_duration,
                 (uint8_t *)aSnkBuff,
                 NULL);

    /* AUDIO_ROLE_SINK */
    direction = DATA_PATH_OUTPUT;
    CAP_Broadcast_ReadSupportedControllerDelay(GMAPAPP_Context.BSNK.current_num_bis,
                                               DATA_PATH_OUTPUT,
                                               a_codec_id,
                                               &controller_delay_min,
                                               &controller_delay_max);

    /* choice of implementation : we try to use as much as possible the controller RAM for delaying before the application RAM*/
    controller_delay = GMAPAPP_Context.BSNK.base_group.PresentationDelay - APP_DELAY_SNK_MIN;

    /* check that we don't exceed the maximum value */
    if (controller_delay > controller_delay_max)
    {
      controller_delay = controller_delay_max;
      LOG_INFO_APP("Controller delay clipped to maximum value %d us\n", controller_delay);
    }
    else if (controller_delay < controller_delay_min)
    {
      controller_delay = controller_delay_min;
      LOG_INFO_APP("Controller delay clipped to minimum value %d us\n", controller_delay);
      LOG_INFO_APP("Warning, could not respect the presentation delay (too low)\n");
    }
    else
    {
      LOG_INFO_APP("Controller delay chosen to %d us\n", controller_delay);
    }

    /* compute the application delay */
    LOG_INFO_APP("Expecting application to respect the delay of %d us\n",
                (GMAPAPP_Context.BSNK.base_group.PresentationDelay - controller_delay));
    if ((GMAPAPP_Context.BSNK.base_group.PresentationDelay - controller_delay) > APP_DELAY_SNK_MAX)
    {
      LOG_INFO_APP("Warning, could not respect the presentation delay (too high)\n");
    }

    CODEC_DataPathSampleParam_t param;
    /* sample coded on 16bits */
    param.SampleDepth = 16;

    /* SAI/I2C peripheral driver requests to set decimation to 2 (stereo buffer)*/
    param.Decimation = 2;

    ret = CAP_Broadcast_SetupAudioDataPath(GMAPAPP_Context.BSNK.current_num_bis,
                                            &GMAPAPP_Context.BSNK.current_BIS_conn_handles[0],
                                            direction,
                                            a_codec_id,
                                            controller_delay,
                                            DATA_PATH_SAMPLE_CIRC_BUF,
                                            CONFIGURE_DATA_PATH_SAMPLE_LEN,
                                            (const uint8_t*) &param);
    if (ret != BLE_STATUS_SUCCESS)
    {
      LOG_INFO_APP("  Fail   : CAP_Broadcast_SetupAudioDataPath() function, result: 0x%02X\n", ret);
    }
    else
    {
      LOG_INFO_APP("  Success: CAP_Broadcast_SetupAudioDataPath() function\n");
    }
  }
  else
  {
    LOG_INFO_APP("Sampling Frequency in LTV is invalid\n");
    ret = BLE_STATUS_FAILED;
  }
  LOG_INFO_APP("==>> End APP_BroadcastSetupAudio function\n");
  return ret;
}

static uint8_t APP_StartBroadcastAudio(Audio_Role_t role)
{
  if(role == AUDIO_ROLE_SOURCE)
  {
    CODEC_RegisterTriggerClbk(DATA_PATH_SAMPLE_CIRC_BUF, DATA_PATH_INPUT, &start_audio_source);
  }
  else if (role == AUDIO_ROLE_SINK)
  {
    CODEC_RegisterTriggerClbk(DATA_PATH_SAMPLE_CIRC_BUF, DATA_PATH_OUTPUT, &start_audio_sink);
  }
  return 0;
}
#endif /*((APP_GMAP_ROLE & GMAP_ROLE_BROADCAST_GAME_RECEIVER) == GMAP_ROLE_BROADCAST_GAME_RECEIVER)*/

static uint8_t GMAPAPP_BuildAdvDataPacket(uint8_t *pAdvData,
                                         uint8_t *AdvPacketLen,
                                         CAP_Announcement_t AnnouncementType,
                                         uint8_t EnableSolicitationRequest,
                                         uint16_t Appearance)
{
  *AdvPacketLen = GMAP_BuildAdvPacket(AnnouncementType,
                                      (void*)0u,
                                      0u,
                                      Appearance,
                                      &pAdvData[0],
                                      (CAP_ADV_DATA_LEN+GMAP_USR_ADV_DATA_LEN));

  if (*AdvPacketLen > 0u)
  {
    uint8_t i;
    LOG_INFO_APP("CAP Advertising Packet (Length : %d) :\n",(*AdvPacketLen));
    for (i = 0u; i < (*AdvPacketLen) ; i++)
    {
      LOG_INFO_APP("0x%02X ",pAdvData[i]);
    }
    LOG_INFO_APP("\n");
    return BLE_STATUS_SUCCESS;
  }
  else
  {
    LOG_INFO_APP("Error to build Advertising Packet by CAP\n");
    return BLE_STATUS_ERROR;
  }
}

static void APP_ParseMetadataParams(APP_ASE_Info_t *pASE,
                                    uint16_t ConnHandle,
                                    uint8_t const *pMetadata,
                                    uint8_t MetadataLength)
{
  uint8_t status;
  Audio_Context_t audio_context = 0x0000;
  Audio_Context_t snk_context = 0x0000;
  Audio_Context_t src_context = 0x0000;
  Audio_Context_t available_snk_context = 0x0000;
  Audio_Context_t available_src_context = 0x0000;
  APP_ACL_Conn_t *p_conn = APP_GetACLConn(ConnHandle);
  LOG_INFO_APP("Parse Metadata of ASE ID 0x%02X (Type 0x%02X) in state 0x%02X on ConnHandle 0x%04X\n",
               pASE->ID,
               pASE->type,
               pASE->state,
               ConnHandle);
  if (p_conn != 0)
  {
    status = LTV_GetStreamingAudioContexts((uint8_t *)pMetadata,MetadataLength,&audio_context);
    if(status == 1u)
    {
      LOG_INFO_APP("ASE ID 0x%02X on ConnHandle 0x%04X is associated to a Streaming Audio Contexts 0x%04X\n",
                   pASE->ID,
                   ConnHandle,
                   audio_context);
      /*update available audio contexts in case of streaming audio contexts associated to the ASE has changed*/
      if (pASE->streaming_audio_context != audio_context)
      {
        LOG_INFO_APP("ASE ID 0x%02X on ConnHandle 0x%04X was previously associated to a Streaming Audio Contexts 0x%04X\n",
                   pASE->ID,
                   ConnHandle,
                   pASE->streaming_audio_context);
        /* Release the Streaming Audio Contexts previously associated to the ASE to calculate resulting
         * new available audio contexts in compliance with other ASEs in ENABLING or STREAMING state
         */
        if (pASE->type == ASE_SINK)
        {
          if ( pASE->streaming_audio_context != 0x0000u)
          {
            snk_context = APP_ReleasedASEStreamingAudioContexts(p_conn,
                                                                pASE,
                                                                pASE->streaming_audio_context);
          }
        }
        else
        {
          if ( pASE->streaming_audio_context != 0x0000u)
          {
            src_context = APP_ReleasedASEStreamingAudioContexts(p_conn,
                                                                pASE,
                                                                pASE->streaming_audio_context);
          }
        }
        /* if context are now available, update them */
        available_snk_context = GMAPAPP_Context.AvailableSnkAudioContext | snk_context;
        available_src_context = GMAPAPP_Context.AvailableSrcAudioContext | src_context;

        /* now it's time to update the available audio contexts according to the new streaming audio context
         * of the specified ASE
         */
        pASE->streaming_audio_context = audio_context;

        /* Set audio contexts to removed from availability according to the streaming audio contexts associated to the
         * ASE.
         */
        if (pASE->type == ASE_SINK)
        {
          /* Remove the contexts which are not available according to Streaming Audio contexts of the ASE */
          available_snk_context = APP_UnavailableAudioContexts(pASE->type,available_snk_context,audio_context);
        }
        else /*(p_info->type & ASE_SOURCE)*/
        {
          /* Remove the contexts which are not available according to Streaming Audio contexts of the ASE */
          available_src_context = APP_UnavailableAudioContexts(ASE_SOURCE,available_src_context,audio_context);
        }
        LOG_INFO_APP("Previous Available Contexts are : Snk 0x%04X  - Src  0x%04X\n",
                     GMAPAPP_Context.AvailableSnkAudioContext,
                     GMAPAPP_Context.AvailableSrcAudioContext);
        LOG_INFO_APP("Calculated New Available Contexts are : Snk 0x%04X  - Src  0x%04X\n",
                     available_snk_context,
                     available_src_context);

        /*Set new available audio contexts for BAP Announcement*/
        status = CAP_SetAvailableAudioContexts(available_snk_context,available_src_context);
        if (status == BLE_STATUS_SUCCESS)
        {
          if (pASE->type == ASE_SINK)
          {
            GMAPAPP_Context.AvailableSnkAudioContext = available_snk_context;
          }
          else /*(p_info->type & ASE_SOURCE)*/
          {
            GMAPAPP_Context.AvailableSrcAudioContext = available_src_context;
          }
          LOG_INFO_APP("Set Available Snk Audio Contexts 0x%04X and Available Src Audio Contexts 0x%04X\n",
                       GMAPAPP_Context.AvailableSnkAudioContext,
                       GMAPAPP_Context.AvailableSrcAudioContext);
        }
        else
        {
          LOG_INFO_APP("FAILURE to Set Available Snk Audio Contexts 0x%04X and Available Src Audio Contexts 0x%04X\n",
                       available_snk_context,
                       available_src_context);
        }

        if (GMAPAPP_Context.NumConn > 1u )
        {
          if ((p_conn->AvailableSnkAudioContext != SUPPORTED_SNK_CONTEXTS) \
              || (p_conn->AvailableSrcAudioContext != SUPPORTED_SRC_CONTEXTS))
          {
            /*update the available Audio Contexts of the CAP Initiator requesting the enable operation*/
            status = CAP_UpdateAvailableAudioContexts(ConnHandle,
                                                      SUPPORTED_SNK_CONTEXTS,
                                                      SUPPORTED_SRC_CONTEXTS);
            LOG_INFO_APP("Send Update Available Snk Audio Contexts 0x%04X and Available Src Audio Contexts 0x%04X with CAP Initiator on ConnHandle 0x%04X returns status 0x%02X\n",
                         SUPPORTED_SNK_CONTEXTS,
                         SUPPORTED_SRC_CONTEXTS,
                         ConnHandle,
                         status);
            p_conn->AvailableSnkAudioContext = SUPPORTED_SNK_CONTEXTS;
            p_conn->AvailableSrcAudioContext = SUPPORTED_SRC_CONTEXTS;
          }
          else
          {
            LOG_INFO_APP("Available Snk Audio Contexts 0x%04X and Available Src Audio Contexts 0x%04X with CAP Initiator on ConnHandle 0x%04X haven't changed\n",
                         SUPPORTED_SNK_CONTEXTS,
                         SUPPORTED_SRC_CONTEXTS,
                         ConnHandle);
          }

          for (uint8_t conn = 0u; conn < CFG_BLE_NUM_LINK ; conn++)
          {
            if ((GMAPAPP_Context.ACL_Conn[conn].Acl_Conn_Handle != 0xFFFFu) && (&GMAPAPP_Context.ACL_Conn[conn] != p_conn))
            {
              if ((GMAPAPP_Context.ACL_Conn[conn].AvailableSnkAudioContext != GMAPAPP_Context.AvailableSnkAudioContext) \
                  || (GMAPAPP_Context.ACL_Conn[conn].AvailableSrcAudioContext != GMAPAPP_Context.AvailableSrcAudioContext))
              {
                /* we update the Audio Contexts availability to other connected CAP Initiators */
                status = CAP_UpdateAvailableAudioContexts(GMAPAPP_Context.ACL_Conn[conn].Acl_Conn_Handle,
                                                          GMAPAPP_Context.AvailableSnkAudioContext,
                                                          GMAPAPP_Context.AvailableSrcAudioContext);
                LOG_INFO_APP("Send Update Available Snk Audio Contexts 0x%04X and Available Src Audio Contexts 0x%04X with CAP Initiator on ConnHandle 0x%04X returns status 0x%02X\n",
                             GMAPAPP_Context.AvailableSnkAudioContext,
                             GMAPAPP_Context.AvailableSrcAudioContext,
                             GMAPAPP_Context.ACL_Conn[conn].Acl_Conn_Handle,
                             status);
                GMAPAPP_Context.ACL_Conn[conn].AvailableSnkAudioContext = GMAPAPP_Context.AvailableSnkAudioContext;
                GMAPAPP_Context.ACL_Conn[conn].AvailableSrcAudioContext = GMAPAPP_Context.AvailableSrcAudioContext;
              }
              else
              {
                LOG_INFO_APP("Available Snk Audio Contexts 0x%04X and Available Src Audio Contexts 0x%04X with CAP Initiator on ConnHandle 0x%04X haven't changed\n",
                             GMAPAPP_Context.AvailableSnkAudioContext,
                             GMAPAPP_Context.AvailableSrcAudioContext,
                             GMAPAPP_Context.ACL_Conn[conn].Acl_Conn_Handle);
              }
            }
          }
        }
        if (GMAPAPP_Context.NumConn < CFG_BLE_NUM_LINK )
        {
          GMAPAPP_StopAdvertising();
          /* Start Advertising */
          if (GMAPAPP_Context.CSIPRank > 0)
          {
            status = GMAPAPP_StartAdvertising(CAP_GENERAL_ANNOUNCEMENT, 0, GAP_APPEARANCE_EARBUD);
          }
          else
          {
            status = GMAPAPP_StartAdvertising(CAP_GENERAL_ANNOUNCEMENT, 0, GAP_APPEARANCE_HEADPHONES);
          }
          LOG_INFO_APP("GMAPAPP_StartAdvertising() returns status 0x%02X\n",status);
        }
      }

    }

    status = LTV_GetPreferredAudioContexts((uint8_t *)pMetadata,MetadataLength,&audio_context);
    if(status == 1u)
    {
      LOG_INFO_APP("ASE ID 0x%02X on ConnHandle 0x%04X is associated to a Preferred Audio Contexts 0x%04X\n",
                   pASE->ID,
                   ConnHandle,
                   audio_context);
    }
  }
}

/* Update the available audio contextsaccording to a Audio Contexts in use
 * Return new available audio contexts
*/
static uint16_t APP_UnavailableAudioContexts(ASE_Type_t Type,
                                             Audio_Context_t AvailableAudioContext,
                                             Audio_Context_t AudioContexts)
{
  Audio_Context_t res_audio_contexts = AvailableAudioContext;
  if (Type == ASE_SINK)
  {
    /* Remove the contexts which are not available according to Streaming Audio contexts of the other ASEs */
    if (AudioContexts & AUDIO_CONTEXT_GAME)
    {
      /*If Audio Contexts includes Media type :  Media, Game, Alerts, Sound Effects and Unspecified Contexts would be no more available*/
      res_audio_contexts &= ( ~(AUDIO_CONTEXT_MEDIA | AUDIO_CONTEXT_GAME | AUDIO_CONTEXT_UNSPECIFIED | AUDIO_CONTEXT_ALERTS | AUDIO_CONTEXT_SOUND_EFFECTS));
    }
    if (AudioContexts & AUDIO_CONTEXT_MEDIA)
    {
      /*If Audio Contexts includes Media type :  Media, Game, Alerts, Sound Effects and Unspecified Contexts would be no more available*/
      res_audio_contexts &= ( ~(AUDIO_CONTEXT_MEDIA | AUDIO_CONTEXT_GAME | AUDIO_CONTEXT_UNSPECIFIED | AUDIO_CONTEXT_ALERTS | AUDIO_CONTEXT_SOUND_EFFECTS));
    }
    if (AudioContexts & AUDIO_CONTEXT_ALERTS)
    {
      /*If Audio Contexts includes Alerts type :  Media, Game, Alerts, Sound Effects and Unspecified Contexts would be no more available*/
      res_audio_contexts &= ( ~(AUDIO_CONTEXT_ALERTS | AUDIO_CONTEXT_UNSPECIFIED | AUDIO_CONTEXT_MEDIA | AUDIO_CONTEXT_GAME |  AUDIO_CONTEXT_SOUND_EFFECTS));
    }
    if (AudioContexts & AUDIO_CONTEXT_SOUND_EFFECTS)
    {
      /*If Audio Contexts includes Sound Effect type :  Sound Effect and Unspecified Contexts be would no more available*/
      res_audio_contexts &= ( ~(AUDIO_CONTEXT_SOUND_EFFECTS | AUDIO_CONTEXT_UNSPECIFIED));
    }
    if (AudioContexts & AUDIO_CONTEXT_RINGTONE)
    {
      /*If Audio Contexts includes RingTone type :  Ringtone, Media, Game, Alerts, Sound Effects and Unspecified Contexts would be no more available*/
      res_audio_contexts &= ( ~(AUDIO_CONTEXT_RINGTONE | AUDIO_CONTEXT_UNSPECIFIED | AUDIO_CONTEXT_GAME | AUDIO_CONTEXT_MEDIA | AUDIO_CONTEXT_ALERTS | AUDIO_CONTEXT_SOUND_EFFECTS));
    }
    if (AudioContexts & AUDIO_CONTEXT_CONVERSATIONAL)
    {
      /*If Audio Contexts includes Conversational type :  Conversational still available but Ringtone, Media, Game, Alerts, Sound Effects and Unspecified Contexts would no more available*/
      res_audio_contexts &= ( ~(AUDIO_CONTEXT_UNSPECIFIED | AUDIO_CONTEXT_RINGTONE | AUDIO_CONTEXT_MEDIA | AUDIO_CONTEXT_GAME | AUDIO_CONTEXT_ALERTS | AUDIO_CONTEXT_SOUND_EFFECTS));
    }
  }
  else
  {
    if (AudioContexts & AUDIO_CONTEXT_CONVERSATIONAL)
    {
      /*If Audio Contexts includes Conversational type :  Conversational still available but Unspecified Context would no more available*/
      res_audio_contexts &= ( ~(AUDIO_CONTEXT_UNSPECIFIED));
    }
  }
  return res_audio_contexts;
}
/* Release the Streaming Audio Contexts associated to an ASE to update the available audio contexts
 * and return new available audio contexts
*/
static uint16_t APP_ReleasedASEStreamingAudioContexts(APP_ACL_Conn_t *pConn,APP_ASE_Info_t *pASE, uint16_t AudioContexts)
{
  Audio_Context_t available_context = 0;
  Audio_Context_t use_context = 0;

  /* Check if potential removed Audio Contexts is also associated to an Enabling/streaming ASE in order to
   * check if it would be available or not
   */
#if (APP_NUM_SNK_ASE > 0u)
  if (pASE->type == ASE_SINK)
  {
    available_context = AudioContexts;
    for (uint8_t conn = 0u; conn < CFG_BLE_NUM_LINK ; conn++)
    {
      if (GMAPAPP_Context.ACL_Conn[conn].Acl_Conn_Handle != 0xFFFFu)
      {
        for ( uint8_t i = 0; i < GMAPAPP_Context.NumSnkASEs;i++)
        {
          if ((GMAPAPP_Context.ACL_Conn[conn].pASEs->aSnkASE[i].allocated == 1) \
              && (&GMAPAPP_Context.ACL_Conn[conn].pASEs->aSnkASE[i] != pASE))
          {
            if ((GMAPAPP_Context.ACL_Conn[conn].pASEs->aSnkASE[i].state == ASE_STATE_ENABLING) \
              || (GMAPAPP_Context.ACL_Conn[conn].pASEs->aSnkASE[i].state == ASE_STATE_STREAMING))
            {
              use_context |= GMAPAPP_Context.ACL_Conn[conn].pASEs->aSnkASE[i].streaming_audio_context;
              available_context &= ~GMAPAPP_Context.ACL_Conn[conn].pASEs->aSnkASE[i].streaming_audio_context;
              LOG_INFO_APP("Streaming Audio Contexts 0x%04X associated to ASE ID %d on ConnHandle 0x%04X still in use\n",
                           GMAPAPP_Context.ACL_Conn[conn].pASEs->aSnkASE[i].streaming_audio_context,
                           GMAPAPP_Context.ACL_Conn[conn].pASEs->aSnkASE[i].ID,
                           GMAPAPP_Context.ACL_Conn[conn].Acl_Conn_Handle);
            }
          }
        }
      }
    }
    /* if audio contexts are now available, update them because if audio contexts are released, other audio contexts
     * become also available
     */
    if (available_context != 0x0000)
    {
      if (available_context & AUDIO_CONTEXT_GAME)
      {
        available_context |= SUPPORTED_SNK_MEDIA_RECEIVER_CONTEXTS;
      }
      if (available_context & AUDIO_CONTEXT_MEDIA)
      {
        available_context |= SUPPORTED_SNK_MEDIA_RECEIVER_CONTEXTS;
      }
      if (available_context & AUDIO_CONTEXT_ALERTS)
      {
        available_context |= SUPPORTED_SNK_MEDIA_RECEIVER_CONTEXTS;
      }
      if (available_context & AUDIO_CONTEXT_RINGTONE)
      {
        available_context |= SUPPORTED_SNK_MEDIA_RECEIVER_CONTEXTS;
      }
      if (available_context & AUDIO_CONTEXT_CONVERSATIONAL)
      {
        available_context |= SUPPORTED_SNK_MEDIA_RECEIVER_CONTEXTS;
      }
      /* Remove the contexts which are not available according to Streaming Audio contexts of the other ASEs */
      available_context = APP_UnavailableAudioContexts(ASE_SINK,available_context,use_context);
      if (available_context != 0x0000)
      {
        LOG_INFO_APP("Add Audio Contexts 0x%04X to Available Snk Audio Contexts 0x%04X\n",
                     available_context,
                     GMAPAPP_Context.AvailableSnkAudioContext);
      }
    }
  }
#endif /* (APP_NUM_SNK_ASE > 0u) */
#if (APP_NUM_SRC_ASE > 0u)
  if (pASE->type == ASE_SOURCE)
  {
    available_context = AudioContexts;
    for (uint8_t conn = 0u; conn < CFG_BLE_NUM_LINK ; conn++)
    {
      if (GMAPAPP_Context.ACL_Conn[conn].Acl_Conn_Handle != 0xFFFFu)
      {
        for ( uint8_t i = 0; i < GMAPAPP_Context.NumSrcASEs;i++)
        {
          if ((GMAPAPP_Context.ACL_Conn[conn].pASEs->aSrcASE[i].allocated == 1) \
              && (&GMAPAPP_Context.ACL_Conn[conn].pASEs->aSrcASE[i] != pASE))
          {
            if ((GMAPAPP_Context.ACL_Conn[conn].pASEs->aSrcASE[i].state == ASE_STATE_ENABLING) \
              || (GMAPAPP_Context.ACL_Conn[conn].pASEs->aSrcASE[i].state == ASE_STATE_STREAMING))
            {
              use_context |= GMAPAPP_Context.ACL_Conn[conn].pASEs->aSrcASE[i].streaming_audio_context;
              available_context &= ~GMAPAPP_Context.ACL_Conn[conn].pASEs->aSrcASE[i].streaming_audio_context;
              LOG_INFO_APP("Streaming Audio Contexts 0x%04X associated to ASE ID %d on ConnHandle 0x%04X still in use\n",
                           GMAPAPP_Context.ACL_Conn[conn].pASEs->aSrcASE[i].streaming_audio_context,
                           GMAPAPP_Context.ACL_Conn[conn].pASEs->aSrcASE[i].ID,
                           GMAPAPP_Context.ACL_Conn[conn].Acl_Conn_Handle);
            }
          }
        }
      }
    }
    /* Remove the contexts which are not available according to Streaming Audio contexts of the other ASEs */
    available_context = APP_UnavailableAudioContexts(ASE_SOURCE,available_context,use_context);
    if (available_context != 0x0000)
    {
      LOG_INFO_APP("Add Audio Contexts 0x%04X to Available Src Audio Contexts 0x%04X\n",
                   available_context,
                   GMAPAPP_Context.AvailableSrcAudioContext);
    }
  }
#endif /* (APP_NUM_SRC_ASE > 0u) */

  return available_context;
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
      GMAPAPP_Context.ACL_Conn[i].AudioProfile = AUDIO_PROFILE_NONE;
      GMAPAPP_Context.ACL_Conn[i].ConfirmIndicationRequired = 0u;

      return &GMAPAPP_Context.ACL_Conn[i];
    }
  }
  return 0;
}

static APP_ASE_Info_t *APP_GetASE(uint8_t ASE_ID,uint16_t ACL_ConnHandle)
{
  uint8_t i;
  APP_ACL_Conn_t *p_conn = APP_GetACLConn(ACL_ConnHandle);
  if (p_conn != 0)
  {
    if (p_conn->pASEs != 0)
    {
#if (APP_NUM_SNK_ASE > 0)
      if (GMAPAPP_Context.NumSnkASEs > 0u)
      {
        for ( i = 0; i < GMAPAPP_Context.NumSnkASEs;i++)
        {
          if ((p_conn->pASEs->aSnkASE[i].ID == ASE_ID) && (p_conn->pASEs->aSnkASE[i].allocated == 1u))
          {
            return &p_conn->pASEs->aSnkASE[i];
          }
        }
      }
#endif /*(APP_NUM_SNK_ASE > 0)*/
#if (APP_NUM_SRC_ASE > 0)
      if (GMAPAPP_Context.NumSrcASEs > 0u)
      {
        for ( i = 0; i < GMAPAPP_Context.NumSrcASEs;i++)
        {
          if ((p_conn->pASEs->aSrcASE[i].ID == ASE_ID) && (p_conn->pASEs->aSrcASE[i].allocated == 1u))
          {
            return &p_conn->pASEs->aSrcASE[i];
          }
        }
      }
#endif /*(APP_NUM_SRC_ASE > 0)*/
    }
  }
  return 0;
}

static uint8_t APP_UnicastSetupAudioDataPath(uint16_t ACL_ConnHandle,
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
    if ((frame_duration != FRAME_DURATION_7_5_MS) && (frame_duration != FRAME_DURATION_10_MS))
    {
      return BLE_STATUS_INVALID_PARAMS;
    }

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
      Sampling_Freq_t target_frequency = frequency;
      APP_ASE_Info_t *p_app_ase2 = 0;
      BAP_ASE_Info_t *p_ase2 = 0;
      GMAPAPP_Context.SinkDecimation = 1;
      GMAPAPP_Context.SourceDecimation = 1;
      /* Check if other ASEs are configured with different sample rates */
      for (i = 0; i < APP_NUM_SNK_ASE + APP_NUM_SRC_ASE; i++)
      {
        if (i < APP_NUM_SNK_ASE)
        {
          p_app_ase2 = &p_conn->pASEs->aSnkASE[i];
        }
        else
        {
          p_app_ase2 = &p_conn->pASEs->aSrcASE[i-APP_NUM_SNK_ASE];
        }
        if (p_app_ase2 != 0)
        {
          if (p_app_ase2->ID != p_ase->ASE_ID
            && p_app_ase2->type != p_ase->Type
            && p_app_ase2->allocated == 1u)
          {
            /* Found another ASE allocated */
            p_ase2 = CAP_Unicast_GetASEInfo(ACL_ConnHandle, p_app_ase2->ID);
            if (p_ase2 != 0)
            {
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
                  LOG_INFO_APP("ASE id %d type %d and ASE id %d type %d are starting, using combined frequency %d\n",
                               p_ase->ASE_ID,
                               p_ase->Type,
                               p_app_ase2->ID,
                               p_app_ase2->type,
                               target_frequency);
                  LOG_INFO_APP("Source Decimation Multiplier = %d, Sink Decimation Multiplier = %d\n",
                               GMAPAPP_Context.SourceDecimation,
                               GMAPAPP_Context.SinkDecimation);
                  break;
                }
              }
            }
          }
        }
      }

      LOG_INFO_APP("Configure Audio Periphal drivers at Sampling frequency %d\n", target_frequency);
      MX_AudioInit(role,target_frequency,
                   frame_duration,
                   (uint8_t *)aSnkBuff,
                   (uint8_t *)aSrcBuff);
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
        CODEC_RegisterTriggerClbk(DATA_PATH_SAMPLE_CIRC_BUF, DATA_PATH_INPUT, &start_audio_source);
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
        CODEC_RegisterTriggerClbk(DATA_PATH_SAMPLE_CIRC_BUF, DATA_PATH_OUTPUT, &start_audio_sink);
      }
    }
    CODEC_DataPathSampleParam_t param;
    /* input data path */
    param.SampleDepth = 16;

    /* SAI/I2C peripheral driver requests to set decimation to 2*/

    if (role == AUDIO_ROLE_SOURCE)
    {
      param.Decimation = GMAPAPP_Context.SourceDecimation; /* 1 * sample rate multiplier */
    }
    else
    {
      param.Decimation = GMAPAPP_Context.SinkDecimation * 2; /* 2 * sample rate multiplier */
    }

    /*Data Path ID is vendor-specific transport interface : 0x01 for "Shared memory of SAI"*/
    status = CAP_Unicast_SetupAudioDataPath(CIS_ConnHandle,
                                            ASE_ID,
                                            DATA_PATH_SAMPLE_CIRC_BUF,
                                            ControllerDelay,
                                            CONFIGURE_DATA_PATH_SAMPLE_LEN,
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
      p_app_ase = APP_GetASE(ASE_ID,ACL_ConnHandle);
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

/*Audio Source */
static int32_t start_audio_source(void)
{
  int32_t status;
  LOG_INFO_APP("START AUDIO SOURCE (input)\n");
  status = Start_RxAudio();

#if (CFG_LCD_SUPPORTED == 1)

  if ((GMAPAPP_Context.num_cis_established > 0u) && (GMAPAPP_Context.audio_role_setup != 0x00))
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
#endif /* (CFG_LCD_SUPPORTED == 1) */
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

#if (CFG_LCD_SUPPORTED == 1)

  if ((GMAPAPP_Context.num_cis_established > 0u) && (GMAPAPP_Context.audio_role_setup != 0x00))
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
#endif /* (CFG_LCD_SUPPORTED == 1) */
  return status;
}

#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)
static void VCP_MetaEvt_Notification(VCP_Notification_Evt_t *pNotification)
{
  switch(pNotification->EvtOpcode)
  {
    case VCP_RENDERER_UPDATED_VOLUME_STATE_EVT:
    {
      VCP_VolumeState_Evt_t *p_info = (VCP_VolumeState_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Updated Volume State :\n");
      LOG_INFO_APP("     Volume Setting : %d\n",p_info->VolSetting);
      LOG_INFO_APP("     Mute : %d\n",p_info->Mute);
      LOG_INFO_APP("     Change Counter : %d\n",p_info->ChangeCounter);
      Volume = p_info->VolSetting;
      Mute = p_info->Mute;
      if (Mute == 0)
      {
        Menu_SetVolume(Volume);
        Set_Volume(Volume);
      }
      else
      {
        Menu_SetVolume(0);
        Set_Volume(0);
      }
      break;
    }

    case VCP_RENDERER_UPDATED_AUDIO_INPUT_STATE_EVT:
    {
      VCP_AudioInputState_Evt_t *p_info = (VCP_AudioInputState_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Updated Audio Input State :\n");
      LOG_INFO_APP("     Instance ID : %d\n",p_info->AICInst);
      LOG_INFO_APP("     Gain Setting : %d\n",p_info->State.GainSetting);
      LOG_INFO_APP("     Mute : %d\n",p_info->State.Mute);
      LOG_INFO_APP("     Gain Mode : %d\n",p_info->State.GainMode);
      LOG_INFO_APP("     Change Counter : %d\n",p_info->ChangeCounter);
      UNUSED(p_info);
      break;
    }

    case VCP_RENDERER_UPDATED_AUDIO_INPUT_DESCRIPTION_EVT:
    {
      VCP_AudioDescription_Evt_t *p_info = (VCP_AudioDescription_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Updated Audio Input Description :\n");
      LOG_INFO_APP("     Instance ID : %d\n",p_info->Inst);
      LOG_INFO_APP("     Description : ");
      Print_String(p_info->pData, p_info->DataLength);
      LOG_INFO_APP("\n");
      break;
    }

    case VCP_RENDERER_UPDATED_VOLUME_OFFSET_STATE_EVT:
    {
      VCP_VolumeOffsetState_Evt_t *p_info = (VCP_VolumeOffsetState_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("[VOC %d] Updated Volume Offset State :\n",p_info->VOCInst);
      LOG_INFO_APP("     Volume Offset: %d \n",p_info->VolumeOffset);
      LOG_INFO_APP("     Change Counter : %d\n",p_info->ChangeCounter);
      UNUSED(p_info);
      break;
    }

    case VCP_RENDERER_UPDATED_AUDIO_OUTPUT_LOCATION_EVT:
    {
      VCP_AudioLocation_Evt_t *p_info = (VCP_AudioLocation_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("[VOC %d] Updated Audio Location : 0x%08X\n",p_info->VOCInst,p_info->AudioLocation);
      UNUSED(p_info);
      break;
    }

    case VCP_RENDERER_UPDATED_AUDIO_OUTPUT_DESCRIPTION_EVT:
    {
      VCP_AudioDescription_Evt_t *p_info = (VCP_AudioDescription_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Updated Audio Output Description :\n");
      LOG_INFO_APP("     Instance ID : %d\n",p_info->Inst);
      LOG_INFO_APP("     Description : ");
      Print_String(p_info->pData, p_info->DataLength);
      LOG_INFO_APP("\n");
      break;
    }
    default:
      break;
  }
}
#endif /*(APP_VCP_ROLE_RENDERER_SUPPORT == 1u)*/

#if (APP_MICP_ROLE_DEVICE_SUPPORT == 1u)
static void MICP_MetaEvt_Notification(MICP_Notification_Evt_t *pNotification)
{
  switch(pNotification->EvtOpcode)
  {
    case MICP_DEVICE_UPDATED_MUTE_EVT:
    {
      LOG_INFO_APP("Received new MICP Mute value: %d\n", pNotification->pInfo[0u]);

      MicMute = pNotification->pInfo[0u];
      Menu_SetMicMute(MicMute);
      break;
    }
    case MICP_DEVICE_UPDATED_AUDIO_INPUT_STATE_EVT:
    {
      MICP_AudioInputState_Evt_t *p_info = (MICP_AudioInputState_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Updated Audio Input State :\n");
      LOG_INFO_APP("     Instance ID : %d\n",p_info->AICInst);
      LOG_INFO_APP("     Gain Setting : %d\n",p_info->State.GainSetting);
      LOG_INFO_APP("     Mute : %d\n",p_info->State.Mute);
      LOG_INFO_APP("     Gain Mode : %d\n",p_info->State.GainMode);
      LOG_INFO_APP("     Change Counter : %d\n",p_info->ChangeCounter);
      UNUSED(p_info);
      break;
    }
    case MICP_DEVICE_UPDATED_AUDIO_INPUT_DESCRIPTION_EVT:
    {
      MICP_AudioDescription_Evt_t *p_info = (MICP_AudioDescription_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Updated Audio Input Description :\n");
      LOG_INFO_APP("     Instance ID : %d\n",p_info->Inst);
      LOG_INFO_APP("     Description : %s\n",p_info->pData);
      UNUSED(p_info);
      break;
    }
    default:
      break;
  }
}
#endif /*(APP_MICP_ROLE_DEVICE_SUPPORT == 1u)*/

#if (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1u)
tBleStatus CSIPAPP_RegisterCSIS(uint8_t instance_id,
                                uint8_t SIRK_OOB,
                                uint8_t SIRK_type,
                                uint8_t* SIRK,
                                uint8_t size,
                                uint8_t rank)
{

  tBleStatus status;
  uint8_t inst_id = instance_id;
  CSIS_ServiceInit_t pInit = {0};
  Audio_Location_t audio_locations;

  pInit.SIRK_type = SIRK_type;
  for (uint8_t i =0 ; i < 16 ; i++)
  {
    pInit.SIRK[i] = SIRK[i];
  }
  pInit.Size = size;
  pInit.Rank = rank;
  pInit.SIRK_OOB = SIRK_OOB;
  status = CSIP_SET_MEMBER_RegisterServiceInstance(inst_id,&pInit);
  LOG_INFO_APP("CSIP_SET_MEMBER_RegisterServiceInstance() of Instance ID %d in rank %d/%d returns status 0x%02X\n",
               inst_id,
               pInit.Rank,
               pInit.Size,
               status);
  if (status == BLE_STATUS_SUCCESS)
  {
    GMAPAPP_Context.CSIPRank = rank;
    status = CAP_RegisterCoordinatedSetMember(inst_id);
    LOG_INFO_APP("CAP_RegisterCoordinatedSetMember() of Instance ID %d returns status 0x%02X\n",
                 inst_id,
                 status);
    audio_locations =  (1u << (GMAPAPP_Context.CSIPRank - 1u));
    if(GMAPAPP_Context.NumSnkASEs >= 1){
      status = CAP_SetSnkAudioLocations(audio_locations);
      if (status == BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("Successfully set the Supported Sink Audio Locations 0x%08X\n",audio_locations);
        GMAPAPP_Context.SnkAudioLocations = audio_locations;
      }
      else
      {
        LOG_INFO_APP("Failed to set the Supported Sink Audio Locations\n");
        return status;
      }
    }
    if(GMAPAPP_Context.NumSrcASEs >= 1){
      status = CAP_SetSrcAudioLocations(audio_locations);
      if (status == BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("Successfully set the Supported Source Audio Locations 0x%08X\n",audio_locations);
        GMAPAPP_Context.SrcAudioLocations = audio_locations;
      }
      else
      {
        LOG_INFO_APP("Failed to set the Supported Source Audio Locations\n");
        return status;
      }
    }

  }
  return status;
}
#endif /*APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1u)*/

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

static void Print_String(uint8_t *pString, uint8_t StringLen)
{
  uint8_t i;
  for (i = 0; i< StringLen; i++)
  {
    LOG_INFO_APP("%c", pString[i]);
  }
}

