/**
  ******************************************************************************
  * @file    pbp_app.c
  * @author  MCD Application Team
  * @brief   Public Broadcast Profile Application
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "pbp_app.h"
#include "main.h"
#include "ble.h"
#include "ble_audio_stack.h"
#include "stm32_seq.h"
#include "codec_mngr.h"
#include "ltv_utils.h"
#include "pbp.h"


/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  Sampling_Freq_t       Freq;
  Frame_Duration_t      FrameDuration;
  uint16_t              OctetsPerCodecFrame;
} PBPAPP_CodecConf_t;

typedef struct
{
  Sampling_Freq_t       Freq;
  uint32_t              SduInterval;
  BAP_Framing_t         Framing;
  uint16_t              MaxSDU;
  uint8_t               RTN;
  uint16_t              MaxTpLatency;
  uint32_t              PresentationDelay;
} PBPAPP_QoSConf_t;
/* Private defines -----------------------------------------------------------*/
/* Allows the codec manager to support latencies up to 8 LC3 frames at 48KHz (120 bytes) for each audio channel */
#define CODEC_POOL_SUB_SIZE                     (960u)

/* double buffer, stereo :
 * 480 (max LC3 frame len (48KHz, 10ms)) x 2 (Max Channels Number (stereo)) x 2 (double buffer configuration)
 */
#define SAI_MAX_BUFF_SIZE                       (480*2*2)

#define SOURCE_ID_LIST_SIZE                     (3u)

/* Broadcast Source Config
 *  16_2_1 is 3
 *  32_2_1 is 7
 *  48_4_1 is 13
 */
#define BROADCAST_SOURCE_BAP_CONFIG             (13u)
#define BROADCAST_SOURCE_FRAME_BLOCK_PER_SDU    (1u)
#define BROADCAST_SOURCE_NUM_BIS                (1u)
#define BROADCAST_SOURCE_CHANNEL_ALLOC_1        (FRONT_LEFT|FRONT_RIGHT)
#define BROADCAST_SOURCE_CHANNEL_ALLOC_2        (FRONT_RIGHT)
#define BROADCAST_CONTROLLER_DELAY              (22000u)
#define BAP_BROADCAST_MAX_TRANSPORT_LATENCY     (20u)
#define BAP_BROADCAST_ENCRYPTION                (0u)
#define BIG_HANDLE                              (0u)

/* 0x0002 is conversational
 * 0x0004 is Media
 */
#define STREAMING_AUDIO_CONTEXT                 (AUDIO_CONTEXT_MEDIA)

/* 0x0880 is Generic Audio Source
 * See Assigned Numbers section 2.6
 */
#define APPEARANCE                              (APPEARANCE_GENERIC_AUDIO_SOURCE)

/* Length of the aPBPAPP_BroadcastName String */
#define BROADCAST_NAME_LENGTH                   10

#define BLE_AUDIO_DYN_ALLOC_SIZE                (BLE_AUDIO_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK))

/*Memory required for CAP*/
#define CAP_DYN_ALLOC_SIZE      CAP_MEM_TOTAL_BUFFER_SIZE(CAP_ROLE_INITIATOR,CFG_BLE_NUM_LINK, \
                                    MAX_NUM_CIG,MAX_NUM_CIS_PER_CIG, \
                                    MAX_NUM_UCL_SNK_ASE, \
                                    MAX_NUM_UCL_SRC_ASE, \
                                    0u,0u,\
                                    0u,0u, \
                                    0u,0u, \
                                    0u,0u,0u, \
                                    0u,0u)

#define CODEC_LC3_SESSION_DYN_ALLOC_SIZE CODEC_GET_TOTAL_SESSION_BUFFER_SIZE(CODEC_LC3_NUM_SESSION)

#define CODEC_LC3_ENCODER_DYN_ALLOC_SIZE CODEC_GET_TOTAL_ENCODER_CH_BUFFER_SIZE(CODEC_LC3_NUM_ENCODER_CHANNEL)

#define CODEC_LC3_DECODER_DYN_ALLOC_SIZE CODEC_GET_TOTAL_DECODER_CH_BUFFER_SIZE(CODEC_LC3_NUM_DECODER_CHANNEL)
/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
PBPAPP_Context_t PBPAPP_Context = {0};
CAP_Config_t PBPAPP_CAP_Config = {0};
BAP_Config_t PBPAPP_BAP_Config = {0};
CCP_Config_t PBPAPP_CCP_Config = {0};
MCP_Config_t PBPAPP_MCP_Config = {0};
VCP_Config_t PBPAPP_VCP_Config = {0};
MICP_Config_t PBPAPP_MICP_Config = {0};
CSIP_Config_t PBPAPP_CSIP_Config = {0};

static uint32_t aCAPMemBuffer[DIVC(CAP_DYN_ALLOC_SIZE,4)];
static uint32_t aAudioInitBuffer[BLE_AUDIO_DYN_ALLOC_SIZE];
static BleAudioInit_t BleAudioInit;

#if (CODEC_LC3_NUM_SESSION > 0u)
static uint32_t aLC3SessionMemBuffer[DIVC(CODEC_LC3_SESSION_DYN_ALLOC_SIZE,4)];
#endif /* CODEC_LC3_NUM_SESSION */
#if (CODEC_LC3_NUM_ENCODER_CHANNEL > 0u)
static uint32_t aLC3EncoderMemBuffer[DIVC(CODEC_LC3_ENCODER_DYN_ALLOC_SIZE,4)];
#endif /* CODEC_LC3_NUM_ENCODER_CHANNEL */
#if (CODEC_LC3_NUM_DECODER_CHANNEL > 0u)
static uint32_t aLC3DecoderMemBuffer[DIVC(CODEC_LC3_DECODER_DYN_ALLOC_SIZE,4)];
#endif /* CODEC_LC3_NUM_DECODER_CHANNEL */

const PBPAPP_QoSConf_t aPBPAPP_BroadcastQoSConf[NUM_LC3_QOS_CONFIG] =    \
                  {{SAMPLE_FREQ_8000_HZ,7500,BAP_FRAMING_UNFRAMED,26,2,8,40000}, \
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
                  {SAMPLE_FREQ_48000_HZ,10000,BAP_FRAMING_UNFRAMED,155,4,65,40000}};

const PBPAPP_CodecConf_t aPBPAPP_CodecConf[NUM_LC3_CODEC_CONFIG] =    \
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


uint8_t aCodecPacketsMemory[MAX_PATH_NB][CODEC_POOL_SUB_SIZE];


/*Double buffer for an LC3 frame at source*/
uint16_t aSrcBuff[SAI_MAX_BUFF_SIZE] __attribute__((aligned));

#if (BAP_BROADCAST_ENCRYPTION == 1)
uint32_t aPBPAPP_BroadcastCode[4u] = {0x00000001, 0x00000002, 0x00000003, 0x00000004};
#else /*(BAP_BROADCAST_ENCRYPTION == 1)*/
uint32_t aPBPAPP_BroadcastCode[4u] = {0x00000000, 0x00000000, 0x00000000, 0x00000000};
#endif /*(BAP_BROADCAST_ENCRYPTION == 1)*/

uint8_t aPBPAPP_BroadcastName[10] = "PBP SOURCE";
uint32_t PreferredControllerDelay = 12000u;

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

/* Private functions prototypes-----------------------------------------------*/
static tBleStatus PBPAPP_Init(CAP_Role_t CAP_Role, BAP_Role_t BAP_Role);
static void CAP_App_Notification(CAP_Notification_Evt_t *pNotification);
static void PBPAPP_SetupBASE(uint8_t Encryption,
                             uint32_t *pBroadcastCode,
                             uint8_t MetadataLen,
                             uint8_t *pMetadata,
                             uint8_t NumBIS,
                             uint8_t BAPConfID,
                             uint32_t *pChannelAllocation,
                             uint8_t *pFrameBlockPerSDU);
static tBleStatus PBPAPP_BroadcastSetupAudio(Audio_Role_t role);
static void PBPAPP_StartBroadcastAudio(Audio_Role_t role);
static uint8_t PBPAPP_BuildManufacturerAvertisingData(uint8_t *pAdvData, uint8_t AdvDataLen);
static void start_audio_source(void);
static void start_audio_sink(void);
/* Exported functions --------------------------------------------------------*/
extern void APP_NotifyToRun(void);

/* Functions Definition ------------------------------------------------------*/
/**
  * @brief  Notify CAP Events
  * @param  pNotification: pointer on notification information
 */
void CAP_Notification(CAP_Notification_Evt_t *pNotification)
{
  /* Notify PBP */
  CAP_PBP_Notification(pNotification);

  /* Notify App */
  CAP_App_Notification(pNotification);
}


/**
  * @brief  Notify PBP Events
  * @param  pNotification: pointer on notification information
 */
void PBP_Notification(PBP_Notification_Evt_t *pNotification)
{
  switch (pNotification->EvtOpcode)
  {

    case PBP_PBS_BROADCAST_AUDIO_STARTED_EVT:
      {
        CAP_Broadcast_AudioStarted_Info_t *data = (CAP_Broadcast_AudioStarted_Info_t*) pNotification->pInfo;
        LOG_INFO_APP(">>== PBP_PBS_BROADCAST_AUDIO_STARTED_EVT\n");
        LOG_INFO_APP("     - Status = 0x%02X\n",pNotification->Status);
        LOG_INFO_APP("     - BIG_Handle : 0x%02x\n",BIG_HANDLE);
        if (pNotification->Status == BLE_STATUS_SUCCESS)
        {
          LOG_INFO_APP("     - Num Created BISes = %d\n",data->NumBISes);
        }
        PBPAPP_Context.current_num_bis = data->NumBISes;

        UTIL_MEM_cpy_8(&(PBPAPP_Context.current_BIS_conn_handles[0]),
                       data->pConnHandle,
                       (data->NumBISes * sizeof(uint16_t)));

        (void)PBPAPP_BroadcastSetupAudio(AUDIO_ROLE_SOURCE);
      }
      break;
    case PBP_BROADCAST_AUDIO_UP_EVT:
      {
      }
      break;
    case PBP_BROADCAST_AUDIO_DOWN_EVT:
    {
      MX_AudioDeInit();
      LOG_INFO_APP(">>== PBP_BROADCAST_AUDIO_DOWN_EVT\n");
    }
    break;


   default:
      break;
  }
  return;
}

/**
  * @brief The BLE Audio Stack requests to be executed. BLE_AUDIO_STACK_Task() shall be called.
  * @note  If the Host task is executed in a separate thread, the thread shall be unlocked here.
  */
void BLE_AUDIO_STACK_NotifyToRun(void)
{
  APP_NotifyToRun();
}

/**
  * @brief Initialize PBP Source and Start Broadcast Audio Procedure.
  */
uint8_t PBPAPP_InitSource(void)
{
  uint8_t ret = 0;
  uint32_t a_channel_allocation[2] = {BROADCAST_SOURCE_CHANNEL_ALLOC_1, BROADCAST_SOURCE_CHANNEL_ALLOC_2};
  uint8_t a_frame_block_per_sdu[2] = {BROADCAST_SOURCE_FRAME_BLOCK_PER_SDU, BROADCAST_SOURCE_FRAME_BLOCK_PER_SDU};
  uint8_t metadata_len = 4;
  uint8_t a_metadata[4] = {0x03, METADATA_STREAMING_AUDIO_CONTEXTS,
                           STREAMING_AUDIO_CONTEXT & 0xFF,
                           (STREAMING_AUDIO_CONTEXT >> 8) & 0xFF};

  PreferredControllerDelay = BROADCAST_CONTROLLER_DELAY;

  LOG_INFO_APP(">>==  Start CAP Initiator initialization\n");
  ret = PBPAPP_Init(CAP_ROLE_INITIATOR, BAP_ROLE_BROADCAST_SOURCE);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : PBPAPP_Init() function, result: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: PBPAPP_Init() function\n");
  }

  /*Enable Audio Codec in LE Controller */
  PBPAPP_Context.aStandardCodec[0].CodingFormat = AUDIO_CODING_FORMAT_LC3;
  PBPAPP_Context.aStandardCodec[0].TransportMask = 0x01; /*CIS Transport Type only*/
  PBPAPP_Context.AudioCodecInController.NumStandardCodecs = 0x01;
  PBPAPP_Context.AudioCodecInController.pStandardCodec = &PBPAPP_Context.aStandardCodec[0];
  PBPAPP_Context.AudioCodecInController.NumVendorSpecificCodecs = 0x00;
  PBPAPP_Context.AudioCodecInController.pVendorSpecificCodec = (void *)0;

  ret = CAP_EnableAudioCodecController(&PBPAPP_Context.AudioCodecInController);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : CAP_EnableAudioCodecController() function, result: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: CAP_EnableAudioCodecController() function\n");
  }

  PBPAPP_Context.audio_driver_config = AUDIO_DRIVER_CONFIG_HEADSET;

  PBPAPP_SetupBASE(BAP_BROADCAST_ENCRYPTION,
                   aPBPAPP_BroadcastCode,
                   metadata_len,
                   a_metadata,
                   BROADCAST_SOURCE_NUM_BIS,
                   BROADCAST_SOURCE_BAP_CONFIG,
                   a_channel_allocation,
                   a_frame_block_per_sdu);

  if (aPBPAPP_CodecConf[BROADCAST_SOURCE_BAP_CONFIG%16].Freq == SAMPLE_FREQ_48000_HZ
     && (BROADCAST_SOURCE_NUM_BIS == 2
       || LTV_GetNumberOfChannelsSet(BROADCAST_SOURCE_CHANNEL_ALLOC_1) == 2))
  {
    /* Reduce RTN */
    PBPAPP_Context.RTN = 1;
  }

  ret = PBP_Init(PBP_ROLE_PUBLIC_BROADCAST_SOURCE);

  LOG_INFO_APP(">>==  End CAP Initiator initialization\n");
  return ret;
}

uint8_t PBPAPP_StartSource(void)
{
  uint8_t ret;
  uint8_t a_additional_adv_data[6+16] = {5u,
                                                   AD_TYPE_COMPLETE_LOCAL_NAME,
                                                   (uint8_t) 'P',
                                                   (uint8_t) 'B',
                                                   (uint8_t) 'P',
                                                   (uint8_t) '1'};
  PBP_PBS_BroadcastAudioStart_Params_t pbp_audio_start_params;
  CAP_Broadcast_AudioStart_Params_t cap_audio_start_params;

  pbp_audio_start_params.pBroadcastAudioStartParams = &cap_audio_start_params;
  pbp_audio_start_params.StandardQuality = 0;
  pbp_audio_start_params.HighQuality = 0;
  pbp_audio_start_params.pAdvMetadata = 0;
  pbp_audio_start_params.AdvMetadataLen = 0;
  pbp_audio_start_params.pBroadcastName = &aPBPAPP_BroadcastName[0];
  pbp_audio_start_params.BroadcastNameLen = BROADCAST_NAME_LENGTH;
  pbp_audio_start_params.Appearance = APPEARANCE;

  cap_audio_start_params.pBaseGroup = &PBPAPP_Context.base_group;
  cap_audio_start_params.BigHandle = 0;

  /* Generate Random Broadcast ID */
  HW_RNG_Get(1, &cap_audio_start_params.BroadcastId);
  /* Reduce broadcast id on 3 bytes */
  cap_audio_start_params.BroadcastId &= 0x00FFFFFF;

  cap_audio_start_params.Rtn = PBPAPP_Context.RTN;
  cap_audio_start_params.Phy = 0x02;
  cap_audio_start_params.Packing = BAP_PACKING_SEQUENTIAL;
  cap_audio_start_params.MaxTransportLatency = PBPAPP_Context.max_transport_latency;
  cap_audio_start_params.Encryption = PBPAPP_Context.encryption;
  cap_audio_start_params.pBroadcastCode = &PBPAPP_Context.broadcast_code[0];
  cap_audio_start_params.AdvHandle = 0;
  cap_audio_start_params.pExtendedAdvParams = &extended_adv_params;
  cap_audio_start_params.pPeriodicAdvParams = &periodic_adv_params;
  cap_audio_start_params.pAdditionalAdvData = &a_additional_adv_data[0];
  cap_audio_start_params.AdditionalAdvDataLen = 6;


  if (BROADCAST_SOURCE_BAP_CONFIG%16 == 3 || BROADCAST_SOURCE_BAP_CONFIG%16 == 5)
  {
    pbp_audio_start_params.StandardQuality = 1u;
  }
  else if (BROADCAST_SOURCE_BAP_CONFIG%16 >= 10)
  {
    pbp_audio_start_params.HighQuality = 1u;
  }

  cap_audio_start_params.AdditionalAdvDataLen += PBPAPP_BuildManufacturerAvertisingData(
                                                 &(a_additional_adv_data[cap_audio_start_params.AdditionalAdvDataLen]),
                                                 16);

  LOG_INFO_APP("  Start Broadcast Audio\n");
  ret = PBP_PBS_BroadcastAudioStart(&pbp_audio_start_params);

  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : PBP_PBS_BroadcastAudioStart() function, result: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: PBP_PBS_BroadcastAudioStart() function\n");
  }

  return ret;
}

uint8_t PBPAPP_StopSource(void)
{
  uint8_t ret;

  LOG_INFO_APP("  Stop Broadcast Audio\n");
  ret = PBP_PBS_BroadcastAudioStop(BIG_HANDLE, 1);

  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : PBP_PBS_BroadcastAudioStop() function, result: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: PBP_PBS_BroadcastAudioStop() function\n");
  }

  return ret;
}

void APP_NotifyRxAudioCplt(uint16_t AudioFrameSize)
{
  uint8_t i;

  if (PBPAPP_Context.bap_role & BAP_ROLE_BROADCAST_SOURCE)
  {
    for (i = 0; i< PBPAPP_Context.current_num_bis; i++)
    {
      CODEC_SendData(PBPAPP_Context.current_BIS_conn_handles[i], 1, &aSrcBuff[0] + AudioFrameSize/2 + i);
    }
  }
}

void APP_NotifyRxAudioHalfCplt(void)
{
  uint8_t i;

  if (PBPAPP_Context.bap_role & BAP_ROLE_BROADCAST_SOURCE)
  {
    for (i = 0; i< PBPAPP_Context.current_num_bis; i++)
    {
      CODEC_SendData(PBPAPP_Context.current_BIS_conn_handles[i], 1, &aSrcBuff[0] + i);
    }
  }
}

void APP_NotifyTxAudioCplt(uint16_t AudioFrameSize)
{
  UNUSED(AudioFrameSize);
}

void APP_NotifyTxAudioHalfCplt(void)
{

}

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
/**
  * @brief  Notify CAP Events for App
  * @param  pNotification: pointer on notification information
 */
static void CAP_App_Notification(CAP_Notification_Evt_t *pNotification)
{

  switch (pNotification->EvtOpcode)
  {
    case CAP_CODEC_CONFIGURED_EVT:
      {
        LOG_INFO_APP(">>== CAP_CODEC_CONFIGURED_EVT\n");
        Sampling_Freq_t *freq = (Sampling_Freq_t *)pNotification->pInfo;
        LOG_INFO_APP("     - Sample Frequency Type:   0x%02X\n",*freq);
        LOG_INFO_APP("==>> Audio Clock with Sample Frequency Type 0x%02X Initialization\n",*freq);
        AudioClock_Init(*freq);
      }
      break;

    default:
      break;
  }
}

static uint8_t PBPAPP_Init(CAP_Role_t CAP_Role, BAP_Role_t BAP_Role)
{
  tBleStatus ret = 0;
  /* Initialize the Audio IP*/
  BleAudioInit.NumOfLinks = CFG_BLE_NUM_LINK;
  BleAudioInit.bleStartRamAddress = (uint8_t*)aAudioInitBuffer;
  BleAudioInit.total_buffer_size = BLE_AUDIO_DYN_ALLOC_SIZE;
  ret = BLE_AUDIO_STACK_Init(&BleAudioInit);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : BLE_AUDIO_STACK_Init() function, result: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: BLE_AUDIO_STACK_Init() function\n");
    LOG_INFO_APP("BLE Audio Stack Lib version: %s\n", BLE_AUDIO_STACK_GetFwVersion());
  }
  if (ret == BLE_STATUS_SUCCESS)
  {
    /*Clear the CAP Configuration*/
    memset(&PBPAPP_CAP_Config,0,sizeof(PBPAPP_CAP_Config));

    /*Clear the BAP Configuration*/
    memset(&PBPAPP_BAP_Config,0,sizeof(PBPAPP_BAP_Config));

    /*Clear the CCP Configuration*/
    memset(&PBPAPP_CCP_Config,0,sizeof(CCP_Config_t));

    /*Clear the MCP Configuration*/
    memset(&PBPAPP_MCP_Config,0,sizeof(MCP_Config_t));

    /*Clear the VCP Configuration*/
    memset(&PBPAPP_VCP_Config,0,sizeof(VCP_Config_t));

    /*Clear the VCP Configuration*/
    memset(&PBPAPP_MICP_Config,0,sizeof(MICP_Config_t));

    /*Clear the CSIP Configuration*/
    memset(&PBPAPP_CSIP_Config,0,sizeof(CSIP_Config_t));

    PBPAPP_CAP_Config.Role = CAP_Role;
    PBPAPP_CAP_Config.MaxNumLinks = CFG_BLE_NUM_LINK;

    PBPAPP_CAP_Config.pStartRamAddr = (uint8_t *)&aCAPMemBuffer;
    PBPAPP_CAP_Config.RamSize = CAP_DYN_ALLOC_SIZE;

    PBPAPP_BAP_Config.Role = BAP_Role;

    PBPAPP_BAP_Config.MaxNumBleLinks = CFG_BLE_NUM_LINK;

    /*Isochronous Channels Configuration*/
    PBPAPP_BAP_Config.ISOChnlConfig.MaxNumCIG = 0;
    PBPAPP_BAP_Config.ISOChnlConfig.MaxNumCISPerCIG = 0;
    PBPAPP_BAP_Config.ISOChnlConfig.MaxNumBIG = MAX_NUM_BIG;
    PBPAPP_BAP_Config.ISOChnlConfig.MaxNumBISPerBIG = MAX_NUM_BIS_PER_BIG;
    PBPAPP_BAP_Config.ISOChnlConfig.pStartRamAddr = 0;
    PBPAPP_BAP_Config.ISOChnlConfig.RamSize = 0u;

    /* Initialize the Basic Audio Profile*/
    ret = CAP_Init(&PBPAPP_CAP_Config,
                   &PBPAPP_BAP_Config,
                   &PBPAPP_VCP_Config,
                   &PBPAPP_MICP_Config,
                   &PBPAPP_CCP_Config,
                   &PBPAPP_MCP_Config,
                   &PBPAPP_CSIP_Config);
    if (ret != BLE_STATUS_SUCCESS)
    {
      LOG_INFO_APP("  Fail   : CAP_Init() function, result: 0x%02X\n", ret);
    }
    else
    {
      LOG_INFO_APP("  Success: CAP_Init() function\n");
    }
    if (ret == BLE_STATUS_SUCCESS)
    {

      /*Initialize the Audio Codec (LC3)*/
      CODEC_LC3Config_t lc3_config = {0};
      PBPAPP_Context.bap_role = PBPAPP_BAP_Config.Role;

      /*Register the Audio Task */
      UTIL_SEQ_RegTask( 1<<CFG_TASK_AUDIO_ID, UTIL_SEQ_RFU, BLE_AUDIO_STACK_Task);

      lc3_config.NumDecChannel = CODEC_LC3_NUM_DECODER_CHANNEL;
      lc3_config.NumEncChannel = CODEC_LC3_NUM_ENCODER_CHANNEL;
      lc3_config.NumSession = CODEC_LC3_NUM_SESSION;

#if (CODEC_LC3_NUM_SESSION > 0u)
      lc3_config.pSessionStart = aLC3SessionMemBuffer;
#endif /* CODEC_LC3_NUM_SESSION */
#if (CODEC_LC3_NUM_ENCODER_CHANNEL > 0u)
      lc3_config.pEncChannelStart = aLC3EncoderMemBuffer;
#endif /* CODEC_LC3_NUM_ENCODER_CHANNEL */
#if (CODEC_LC3_NUM_DECODER_CHANNEL > 0u)
      lc3_config.pDecChannelStart = aLC3DecoderMemBuffer;
#endif /* CODEC_LC3_NUM_DECODER_CHANNEL */

      CODEC_ManagerInit(MAX_PATH_NB*CODEC_POOL_SUB_SIZE,
                        (uint8_t*)aCodecPacketsMemory,
                        &lc3_config,
                        100u,
                        1700u,
                        CODEC_MODE_DEFAULT);
    }
  }
  return ret;
}

static void PBPAPP_SetupBASE(uint8_t Encryption,
                             uint32_t *pBroadcastCode,
                             uint8_t MetadataLen,
                             uint8_t *pMetadata,
                             uint8_t NumBIS,
                             uint8_t BAPConfID,
                             uint32_t *pChannelAllocation,
                             uint8_t *pFrameBlockPerSDU)
{
  uint8_t i;

  /* Init Broadcast Source Configuration */
  PBPAPP_Context.audio_driver_config = AUDIO_DRIVER_CONFIG_LINEIN;
  PBPAPP_Context.phy = TARGET_LE_2M_PHY;

  PBPAPP_Context.base_bis[0].BIS_Index = 0x01;
  PBPAPP_Context.base_bis[0].CodecSpecificConfLength = 0x06;
  PBPAPP_Context.base_bis[0].pCodecSpecificConf = &(PBPAPP_Context.codec_specific_config_bis[0][0]);

  PBPAPP_Context.base_bis[1].BIS_Index = 0x02;
  PBPAPP_Context.base_bis[1].CodecSpecificConfLength = 0x06;
  PBPAPP_Context.base_bis[1].pCodecSpecificConf = &(PBPAPP_Context.codec_specific_config_bis[1][0]);

  PBPAPP_Context.base_subgroups[0].NumBISes = NumBIS;
  PBPAPP_Context.base_subgroups[0].pBIS = &(PBPAPP_Context.base_bis[0]);
  PBPAPP_Context.base_subgroups[0].CodecID = 0x0000000006; /* LC3 */
  PBPAPP_Context.base_subgroups[0].CodecSpecificConfLength = 0x0D;
  PBPAPP_Context.base_subgroups[0].pCodecSpecificConf = &(PBPAPP_Context.codec_specific_config_subgroup[0][0]);
  UTIL_MEM_cpy_8(PBPAPP_Context.subgroup_metadata[0], &(pMetadata[0]), MetadataLen);
  PBPAPP_Context.base_subgroups[0].pMetadata = &PBPAPP_Context.subgroup_metadata[0][0];
  PBPAPP_Context.base_subgroups[0].MetadataLength = MetadataLen;

  PBPAPP_Context.base_group.NumSubgroups = 1;
  PBPAPP_Context.base_group.pSubgroups = &(PBPAPP_Context.base_subgroups[0]);

  PBPAPP_Context.encryption = Encryption;
  PBPAPP_Context.broadcast_code[0] = pBroadcastCode[0];
  PBPAPP_Context.broadcast_code[1] = pBroadcastCode[1];
  PBPAPP_Context.broadcast_code[2] = pBroadcastCode[2];
  PBPAPP_Context.broadcast_code[3] = pBroadcastCode[3];

  PBPAPP_Context.RTN = 1u;
  uint8_t subgroup_i = 0;
  uint8_t same_fbpsdu_conf = 1u;

  for (i = 1; i < NumBIS; i++)
  {
    if (pFrameBlockPerSDU[i-1] != pFrameBlockPerSDU[i])
    {
      same_fbpsdu_conf = 0;
    }
  }

  PBPAPP_Context.base_group.pSubgroups[0].CodecSpecificConfLength = 0;
  PBPAPP_Context.codec_specific_config_subgroup[0][subgroup_i++] = 0x02;
  PBPAPP_Context.codec_specific_config_subgroup[0][subgroup_i++] = CODEC_SAMPLING_FREQ;
  PBPAPP_Context.codec_specific_config_subgroup[0][subgroup_i++] = aPBPAPP_CodecConf[BAPConfID%16].Freq;
  PBPAPP_Context.codec_specific_config_subgroup[0][subgroup_i++] = 0x02;
  PBPAPP_Context.codec_specific_config_subgroup[0][subgroup_i++] = CODEC_FRAME_DURATION;
  PBPAPP_Context.codec_specific_config_subgroup[0][subgroup_i++] = aPBPAPP_CodecConf[BAPConfID%16].FrameDuration;
  PBPAPP_Context.codec_specific_config_subgroup[0][subgroup_i++] = 0x03;
  PBPAPP_Context.codec_specific_config_subgroup[0][subgroup_i++] = CODEC_OCTETS_PER_CODEC_FRAME;
  PBPAPP_Context.codec_specific_config_subgroup[0][subgroup_i++] = (uint8_t) aPBPAPP_CodecConf[BAPConfID%16].OctetsPerCodecFrame;
  PBPAPP_Context.codec_specific_config_subgroup[0][subgroup_i++] = (uint8_t) (aPBPAPP_CodecConf[BAPConfID%16].OctetsPerCodecFrame >> 8);

  PBPAPP_Context.base_group.pSubgroups[0].CodecSpecificConfLength += 10;

  if (same_fbpsdu_conf == 1u)
  {
    PBPAPP_Context.codec_specific_config_subgroup[0][subgroup_i++] = 0x02;
    PBPAPP_Context.codec_specific_config_subgroup[0][subgroup_i++] = CODEC_FRAMES_BLOCKS_PER_SDU;
    PBPAPP_Context.codec_specific_config_subgroup[0][subgroup_i++] = pFrameBlockPerSDU[0];

    PBPAPP_Context.base_group.pSubgroups[0].CodecSpecificConfLength += 3;
  }


  for (i = 0; i < NumBIS; i++)
  {
    uint8_t bis_i = 0;
    /* Take max RTN and transport latency */
    PBPAPP_Context.RTN = MAX(PBPAPP_Context.RTN, aPBPAPP_BroadcastQoSConf[BAPConfID].RTN);
    PBPAPP_Context.max_transport_latency = MAX( PBPAPP_Context.max_transport_latency,
                                               aPBPAPP_BroadcastQoSConf[BAPConfID].MaxTpLatency);
    PBPAPP_Context.base_group.PresentationDelay = MAX(PBPAPP_Context.base_group.PresentationDelay,
                                                      aPBPAPP_BroadcastQoSConf[BAPConfID].PresentationDelay);

    PBPAPP_Context.base_group.pSubgroups[0].pBIS[i].CodecSpecificConfLength = 0;

    if (same_fbpsdu_conf == 0u)
    {
      PBPAPP_Context.codec_specific_config_bis[i][bis_i++] = 0x02;
      PBPAPP_Context.codec_specific_config_bis[i][bis_i++] = CODEC_FRAMES_BLOCKS_PER_SDU;
      PBPAPP_Context.codec_specific_config_bis[i][bis_i++] = pFrameBlockPerSDU[i];

      PBPAPP_Context.base_group.pSubgroups[0].pBIS[i].CodecSpecificConfLength += 3;
    }

    PBPAPP_Context.codec_specific_config_bis[i][bis_i++] = 0x05;
    PBPAPP_Context.codec_specific_config_bis[i][bis_i++] = CODEC_AUDIO_CHNL_ALLOCATION;
    PBPAPP_Context.codec_specific_config_bis[i][bis_i++] = pChannelAllocation[i] & 0xFF;
    PBPAPP_Context.codec_specific_config_bis[i][bis_i++] = (pChannelAllocation[i] >> 8) & 0xFF;
    PBPAPP_Context.codec_specific_config_bis[i][bis_i++] = (pChannelAllocation[i] >> 16) & 0xFF;
    PBPAPP_Context.codec_specific_config_bis[i][bis_i++] = (pChannelAllocation[i] >> 24) & 0xFF;

    PBPAPP_Context.base_group.pSubgroups[0].pBIS[i].CodecSpecificConfLength += 6;
  }
}

static void PBPAPP_StartBroadcastAudio(Audio_Role_t role)
{
  if (role == AUDIO_ROLE_SOURCE)
  {
    CODEC_RegisterTriggerClbk(1,0,&start_audio_source);
  }
  else if (role == AUDIO_ROLE_SINK)
  {
    CODEC_RegisterTriggerClbk(1,1,&start_audio_sink);
  }
}

static tBleStatus PBPAPP_BroadcastSetupAudio(Audio_Role_t role)
{
  Sampling_Freq_t sampling_freq;
  Frame_Duration_t frame_duration;
  uint8_t direction = DATA_PATH_INPUT;
  uint32_t controller_delay;
  uint32_t controller_delay_min = 0;
  uint32_t controller_delay_max = 0;
  uint8_t a_codec_id[5] = {0x00,0x00,0x00,0x00,0x00};
  tBleStatus ret;

  LOG_INFO_APP("==>> Start PBPAPP_BroadcastSetupAudio function\n");

  a_codec_id[0] = AUDIO_CODING_FORMAT_LC3;

  sampling_freq = LTV_GetConfiguredSamplingFrequency(
      &PBPAPP_Context.base_group.pSubgroups[0].pCodecSpecificConf[0],
      PBPAPP_Context.base_group.pSubgroups[0].CodecSpecificConfLength);

  frame_duration = LTV_GetConfiguredFrameDuration(
      &PBPAPP_Context.base_group.pSubgroups[0].pCodecSpecificConf[0],
      PBPAPP_Context.base_group.pSubgroups[0].CodecSpecificConfLength);

  if ((sampling_freq != 0) && (frame_duration != 0xFF))
  {
    PBPAPP_StartBroadcastAudio(role);
    MX_AudioInit(role,
                 sampling_freq,
                 frame_duration,
                 NULL,
                 (uint8_t *)aSrcBuff,
                 (AudioDriverConfig)PBPAPP_Context.audio_driver_config);

    if (role == AUDIO_ROLE_SINK)
    {
      direction = DATA_PATH_OUTPUT;
      CAP_Broadcast_ReadSupportedControllerDelay(PBPAPP_Context.current_num_bis,
                                                 DATA_PATH_OUTPUT,
                                                 a_codec_id,
                                                 &controller_delay_min,
                                                 &controller_delay_max);

      /* choice of implementation : we try to use as much as possible the controller RAM for delaying before the application RAM*/
      controller_delay = PBPAPP_Context.base_group.PresentationDelay - APP_DELAY_SNK_MIN;

      /* check that we don't exceed the maximum value */
      if ( controller_delay > controller_delay_max)
      {
        controller_delay = controller_delay_max;
        LOG_INFO_APP("Controller delay chosen to maximum value %d us\n",controller_delay);
      }
      else
      {
        LOG_INFO_APP("Controller delay chosen to %d us\n",controller_delay);
      }

      /* compute the application delay */
      LOG_INFO_APP("Expecting application to respect the delay of %d us\n",
                  (PBPAPP_Context.base_group.PresentationDelay - controller_delay));
      if ((PBPAPP_Context.base_group.PresentationDelay - controller_delay) > APP_DELAY_SNK_MAX)
      {
        LOG_INFO_APP("Warning, could not respect the presentation delay value");
      }
    }
    else
    {
      /* AUDIO_ROLE_SOURCE */
      direction = DATA_PATH_INPUT;
      CAP_Broadcast_ReadSupportedControllerDelay(PBPAPP_Context.current_num_bis,
                                                 DATA_PATH_INPUT,
                                                 a_codec_id,
                                                 &controller_delay_min,
                                                 &controller_delay_max);

      /* at source we don't have to respect a presentation delay */
      controller_delay = PreferredControllerDelay;
      if ( controller_delay > controller_delay_max)
      {
        controller_delay = controller_delay_max;
        LOG_INFO_APP("Controller delay chosen to value %d us\n", controller_delay);
      }
    }

    CODEC_DataPathParam_t param;
    /* sample coded on 16bits */
    param.SampleDepth = 16;

    /* SAI/I2C peripheral driver requests to set decimation to 2 (stereo buffer)*/
    param.Decimation = 2;

    ret = CAP_Broadcast_SetupAudioDataPath(PBPAPP_Context.current_num_bis,
                                            &PBPAPP_Context.current_BIS_conn_handles[0],
                                            direction,
                                            a_codec_id,
                                            controller_delay,
                                            DATA_PATH_CIRCULAR_BUF,
                                            CONFIGURE_DATA_PATH_CONFIG_LEN,
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
  LOG_INFO_APP("==>> End PBPAPP_BroadcastSetupAudio function\n");
  return ret;
}


/*Audio Source */
static void start_audio_source(void)
{
  Start_RxAudio();
}

/*Audio Sink */
static void start_audio_sink(void)
{
  Start_TxAudio();
}

static uint8_t PBPAPP_BuildManufacturerAvertisingData(uint8_t *pAdvData, uint8_t AdvDataLen)
{
  if (AdvDataLen >= 16)
  {
    uint8_t bd_addr[6];
    uint8_t length;

    /* Length */
    pAdvData[0] = 15;

    /* Type: Manufacturer Data */
    pAdvData[1] = 0xFF;

    /* ST Company ID */
    pAdvData[2] = 0x30;
    pAdvData[3] = 0x00;

    /* BlueSTSDK protocol version */
    pAdvData[4] = 0x02;

    /* Board ID: Discovery Kit WBA */
    pAdvData[5] = 0x8C;

    /* Firmware ID: Public Broadcast Source */
    pAdvData[6] = 0x90;

    /* Source ID*/
    pAdvData[7] = BAP_BROADCAST_SOURCE_ID & 0xFF;
    pAdvData[8] = (BAP_BROADCAST_SOURCE_ID >> 8) & 0xFF;
    pAdvData[9] = (BAP_BROADCAST_SOURCE_ID >> 16) & 0xFF;

    /* BD Address */
    aci_hal_read_config_data(CONFIG_DATA_PUBADDR_OFFSET, &length, &(bd_addr[0]));
    pAdvData[10] = bd_addr[0];
    pAdvData[11] = bd_addr[1];
    pAdvData[12] = bd_addr[2];
    pAdvData[13] = bd_addr[3];
    pAdvData[14] = bd_addr[4];
    pAdvData[15] = bd_addr[5];

    return 16;
  }
  else
  {
    /* Advertising Data size insufficient */
    return 0;
  }
}