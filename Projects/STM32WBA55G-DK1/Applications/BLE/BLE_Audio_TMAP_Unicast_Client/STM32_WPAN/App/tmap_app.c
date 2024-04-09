/**
  ******************************************************************************
  * @file    tmap_app.c
  * @author  MCD Application Team
  * @brief   TMAP Application
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

#include "tmap_app.h"
#include "main.h"
#include "ble.h"
#include "ble_audio_stack.h"
#include "stm32_seq.h"
#include "codec_mngr.h"
#include "ltv_utils.h"
#include "cap.h"
#include "tmap.h"
#include "usecase_dev_mgmt.h"
#include "app_menu_cfg.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/

/* Allows the codec manager to support latencies up to 8 LC3 frames at 48KHz (120 bytes) for each audio channel */
#define CODEC_POOL_SUB_SIZE             (960u)

/* double buffer, stereo :
 * 480 (max LC3 frame len (48KHz, 10ms)) x 2 (Max Channels Number (stereo)) x 2 (double buffer configuration)
 */
#define SAI_MAX_BUFF_SIZE               (480*2*2)

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
                                    0,0)

/*Memory size required to allocate resource for Audio Stream Endpoint in Unicats Server Context*/
#define BAP_UCL_ASE_BLOCKS_SIZE \
          BAP_MEM_BLOCKS_ASE_PER_CONN_SIZE_BYTES(MAX_NUM_UCL_SNK_ASE_PER_LINK,MAX_NUM_UCL_SRC_ASE_PER_LINK,MAX_UCL_CODEC_CONFIG_SIZE,MAX_UCL_METADATA_SIZE)

/* Memory size required to allocate resource for Published Audio Capabilites for Unicast Client and/or Broadcast Assistant*/
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

#define CODEC_LC3_SESSION_DYN_ALLOC_SIZE \
        CODEC_GET_TOTAL_SESSION_BUFFER_SIZE(CODEC_LC3_NUM_SESSION)

#define CODEC_LC3_ENCODER_DYN_ALLOC_SIZE \
        CODEC_GET_TOTAL_ENCODER_CH_BUFFER_SIZE(CODEC_LC3_NUM_ENCODER_CHANNEL)

#define CODEC_LC3_DECODER_DYN_ALLOC_SIZE \
        CODEC_GET_TOTAL_DECODER_CH_BUFFER_SIZE(CODEC_LC3_NUM_DECODER_CHANNEL)


#if (APP_CCP_ROLE_SERVER_SUPPORT == 1u)
#define BLE_MEM_BLOCKS_URI_SCHEMES_LIST_DYN_ALLOC_SIZE \
        BLE_CCP_SRV_MEM_BLOCKS_URI_SCHEMES_LIST_SIZE_BYTES(APP_CCP_NUM_LOCAL_BEARER_INSTANCES, \
                                                           APP_CCP_BEARER_URI_SCHEMES_SUPPORTED_LIST_LENGTH)

#define BLE_MEM_BLOCKS_CALL_FRIENDLY_NAME_DYN_ALLOC_SIZE \
        BLE_CCP_SRV_MEM_BLOCKS_CALL_FRIENDLY_NAME_SIZE_BYTES(APP_CCP_NUM_LOCAL_BEARER_INSTANCES,APP_CCP_NUM_CALLS, \
                                                            APP_CCP_CALL_FRIENDLY_NAME_LENGTH)

#define BLE_MEM_BLOCKS_URI_DYN_ALLOC_SIZE \
        BLE_CCP_SRV_MEM_BLOCKS_URI_SIZE_BYTES(APP_CCP_NUM_LOCAL_BEARER_INSTANCES,APP_CCP_NUM_CALLS, \
                                              APP_CCP_FEATURES,APP_CCP_CALL_URI_LENGTH)
#define BLE_CCP_SRV_DYN_ALLOC_SIZE \
        BLE_CCP_SRV_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK,APP_CCP_NUM_LOCAL_BEARER_INSTANCES,APP_CCP_NUM_CALLS,APP_CCP_READLONG_CFG,\
                                      BLE_MEM_BLOCKS_URI_DYN_ALLOC_SIZE,BLE_MEM_BLOCKS_URI_SCHEMES_LIST_DYN_ALLOC_SIZE, \
                                      BLE_MEM_BLOCKS_CALL_FRIENDLY_NAME_DYN_ALLOC_SIZE)
#endif /* (APP_CCP_ROLE_SERVER_SUPPORT == 1u) */

#if (APP_MCP_ROLE_SERVER_SUPPORT == 1u)
#define BLE_MCP_SRV_DYN_ALLOC_SIZE \
        BLE_MCP_SRV_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK,APP_MCP_NUM_LOCAL_MEDIA_PLAYER_INSTANCES,APP_MCP_READLONG_CFG,\
                                      APP_MCP_MEDIA_PLAYER_NAME_LENGTH,APP_MCP_TRACK_TITLE_LENGTH)
#endif /* (APP_MCP_ROLE_SERVER_SUPPORT == 1u) */

#if (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u)
#define BLE_VCP_CTLR_DYN_ALLOC_SIZE     BLE_VCP_CTLR_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK, \
                                                                        APP_VCP_CTLR_NUM_AIC_INSTANCES, \
                                                                        APP_VCP_CTLR_NUM_VOC_INSTANCES)
#endif /* (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u) */

#define BLE_CSIP_SET_COORDINATOR_DYN_ALLOC_SIZE         BLE_CSIP_SET_COORDINATOR_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK)

#define TMAP_DYN_ALLOC_SIZE      TMAP_MEM_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK)

#define VOLUME_STEP 10
#define BASE_VOLUME 128

#define MEDIA_STREAM_QOS_CONFIG 13
#define MEDIA_STREAM_AUDIO_CHANNEL_ALLOC 0x03
#define MEDIA_STREAM_CHANNEL_PER_CIS 2

#define TELEPHONY_STREAM_QOS_CONFIG 7
#define TELEPHONY_STREAM_AUDIO_CHANNEL_ALLOC 0x01
#define TELEPHONY_STREAM_CHANNEL_PER_CIS 2

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
const APP_QoSConf_t APP_QoSConf[NUM_LC3_QoSConf] =    \
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
                  {SAMPLE_FREQ_48000_HZ,10000,BAP_FRAMING_UNFRAMED,155,13,100,40000}};

/* Application context*/
TMAPAPP_Context_t TMAPAPP_Context = {0};

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
#if (APP_CCP_ROLE_SERVER_SUPPORT == 1u)
static uint32_t aCCPSrvMemBuffer[DIVC(BLE_CCP_SRV_DYN_ALLOC_SIZE,4)];
#endif /*(APP_CCP_ROLE_SERVER_SUPPORT == 1u)*/
#if (APP_MCP_ROLE_SERVER_SUPPORT == 1u)
static uint32_t aMCPSrvMemBuffer[DIVC(BLE_MCP_SRV_DYN_ALLOC_SIZE,4)];
#endif /*(APP_MCP_ROLE_SERVER_SUPPORT == 1u)*/
#if (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u)
static uint32_t aCltrMemBuffer[DIVC(BLE_VCP_CTLR_DYN_ALLOC_SIZE,4)];
#endif /*(APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u)*/
static uint32_t aSetCoordinatorMemBuffer[DIVC(BLE_CSIP_SET_COORDINATOR_DYN_ALLOC_SIZE,4)];

/* Buffers allocation for LC3*/
static uint32_t aLC3SessionMemBuffer[DIVC(CODEC_LC3_SESSION_DYN_ALLOC_SIZE,4)];
static uint32_t aLC3EncoderMemBuffer[DIVC(CODEC_LC3_ENCODER_DYN_ALLOC_SIZE,4)];
static uint32_t aLC3DecoderMemBuffer[DIVC(CODEC_LC3_DECODER_DYN_ALLOC_SIZE,4)];

static uint8_t aCodecPacketsMemory[MAX_PATH_NB][CODEC_POOL_SUB_SIZE];

/* In and Out audio buffers sized for an LC3 frame (double buffer, 16 bits per sample) */
static uint16_t aSrcBuff[SAI_MAX_BUFF_SIZE] __attribute__((aligned));
static uint16_t aSnkBuff[SAI_MAX_BUFF_SIZE] __attribute__((aligned));

static uint16_t* pPrevData = NULL;
static uint8_t Nb_Active_Ch = 0;

/*Buffer allocation used in TMAP for internal use*/
static uint32_t aTMAPMemBuffer[DIVC(TMAP_DYN_ALLOC_SIZE,4)];

#if (APP_MCP_ROLE_SERVER_SUPPORT == 1u)
static TMAPAPP_Track_t MCPAPP_Track[APP_MCP_NUM_GROUPS][APP_MCP_NUM_TRACKS] = {{{20000,(uint8_t *)"Track1",6u},
                                                                                {16000,(uint8_t *)"Track2",6u},
                                                                                {10000,(uint8_t *)"Track3",6u},
                                                                                {28000,(uint8_t *)"Track4",6u},
                                                                                {8000,(uint8_t *)"Track5",6u},
                                                                                {12000,(uint8_t *)"Track6",6u},
                                                                                {21000,(uint8_t *)"Track7",6u},
                                                                                {16000,(uint8_t *)"Track8",6u}}};
#endif /*(APP_MCP_ROLE_SERVER_SUPPORT == 1u)*/

/* Structures used as Unicast Audio Start parameters */
CAP_Unicast_AudioStart_Stream_Params_t StartStreamParams;
CAP_Unicast_AudioStart_Codec_Conf_t CodecConfSnk;
CAP_Unicast_AudioStart_Codec_Conf_t CodecConfSrc;
CAP_Unicast_AudioStart_QoS_Conf_t QosConfSnk;
CAP_Unicast_AudioStart_QoS_Conf_t QosConfSrc;
uint8_t aMetadataSnk[20];
uint8_t aMetadataSrc[20];

/* Private functions prototypes-----------------------------------------------*/
static tBleStatus CAPAPP_Init(Audio_Role_t AudioRole);
static tBleStatus TMAPAPP_TMAPInit(uint16_t Role);
static void TMAPAPP_CAPNotification(CAP_Notification_Evt_t *pNotification);
static APP_ACL_Conn_t *APP_GetACLConn(uint16_t ConnHandle);
static APP_ACL_Conn_t *APP_AllocateACLConn(uint16_t ConnHandle);
static APP_ASE_Info_t * TMAPAPP_GetASE(uint8_t ASE_ID,uint16_t ACL_ConnHandle);
static uint8_t TMAPAPP_SetupAudioDataPath(uint16_t ACL_ConnHandle,
                                         uint16_t CIS_ConnHandle,
                                         uint8_t ASE_ID,
                                         uint32_t ControllerDelay);
static void start_audio_source(void);
static void start_audio_sink(void);
static uint8_t APP_GetBitsAudioChnlAllocations(Audio_Chnl_Allocation_t ChnlLocations);

#if (APP_CCP_ROLE_SERVER_SUPPORT == 1u)
static tBleStatus TMAPAPP_RegisterGenericTelephonyBearer(uint8_t *pCCID);
#if (APP_CCP_NUM_LOCAL_BEARER_INSTANCES > 0u)
static tBleStatus TMAPAPP_RegisterTelephonyBearerInstance(uint8_t *pCCID);
#endif /*(APP_CCP_NUM_LOCAL_BEARER_INSTANCES > 0u)*/
static void CCP_MetaEvt_Notification(CCP_Notification_Evt_t *pNotification);
#endif /*(APP_CCP_ROLE_SERVER_SUPPORT == 1u)*/

#if (APP_MCP_ROLE_SERVER_SUPPORT == 1u)
static tBleStatus TMAPAPP_RegisterGenericMediaPlayer(uint8_t *pCCID);
#if (APP_MCP_NUM_LOCAL_MEDIA_PLAYER_INSTANCES > 0u)
static tBleStatus TMAPAPP_RegisterMediaPlayerInstance(uint8_t *pCCID);
#endif /*APP_MCP_NUM_LOCAL_MEDIA_PLAYER_INSTANCES > 0u) */
static tBleStatus TMAPAPP_SetMediaState(uint8_t MediaPlayerCCID,MCP_MediaState_t MediaState);
static TMAPAPP_MediaPlayer_t *TMAPAPP_GetMediaPlayer(uint8_t CCID);
static void TMAPAPP_TrackPositionCB(void *arg);
static void MCP_MetaEvt_Notification(MCP_Notification_Evt_t *pNotification);
#endif /*(APP_CCP_ROLE_SERVER_SUPPORT == 1u)*/

#if (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u)
uint8_t RemoteVolume = BASE_VOLUME;
uint8_t RemoteMute = 0x00;
static void VCP_MetaEvt_Notification(VCP_Notification_Evt_t *pNotification);
#endif /*(APP_CCP_ROLE_SERVER_SUPPORT == 1u)*/
uint8_t LocalVolume = BASE_VOLUME;
uint8_t LocalMute = 0x00;

/* Exported functions --------------------------------------------------------*/

extern void APP_NotifyToRun(void);

/* Functions Definition ------------------------------------------------------*/

void TMAPAPP_Init()
{
  tBleStatus status;
  /* Init Initiator/Commander with Unicast Client */
  status = CAPAPP_Init(AUDIO_ROLE_SOURCE | AUDIO_ROLE_SINK);
  LOG_INFO_APP("CAPAPP_Init() returns status 0x%02X\n",status);
  Set_Volume(BASE_VOLUME);
  Menu_SetLocalVolume(BASE_VOLUME);
  if (status == BLE_STATUS_SUCCESS)
  {
    /* Initialize the TMAP layer*/
    status = TMAPAPP_TMAPInit(APP_TMAP_ROLE);
    LOG_INFO_APP("TMAPAPP_TMAPInit() with role 0x%02X returns status 0x%02X\n", APP_TMAP_ROLE, status);
  }
}

tBleStatus TMAPAPP_Linkup(uint16_t ConnHandle)
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
  TMAPAPP_CAPNotification(pNotification);
}

/**
  * @brief  Notify TMAP Events
  * @param  pNotification: pointer on notification information
 */
void TMAP_Notification(TMAP_Notification_Evt_t *pNotification)
{
  switch(pNotification->EvtOpcode)
  {
    case TMAP_LINKUP_COMPLETE_EVT:
    {
      tBleStatus ret;
      APP_ACL_Conn_t *p_conn = 0u;
      LOG_INFO_APP("TMAP Linkup Complete Event with ConnHandle 0x%04X is received with status 0x%02X\n",
                  pNotification->ConnHandle,
                  pNotification->Status);
      p_conn = APP_GetACLConn(pNotification->ConnHandle);
      if (pNotification->Status == BLE_STATUS_SUCCESS)
      {
        if (p_conn != 0)
        {
          p_conn->AudioProfile |= AUDIO_PROFILE_TMAP;
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

#if (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u)
      ret = CAP_VolumeController_StartSetVolumeProcedure(pNotification->ConnHandle, BASE_VOLUME);
      LOG_INFO_APP("CAP_VolumeController_StartSetVolumeProcedure() returns status 0x%02X\n", ret);
      Menu_SetRemoteVolume(BASE_VOLUME);
#endif /* (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u) */
      break;
    }
    case TMAP_REM_ROLE_VALUE_EVT:
    {
      uint16_t *tmap_role = (uint16_t *) pNotification->pInfo;
      LOG_INFO_APP("TMAP Remote Role Event with ConnHandle 0x%04X is received, TMAP role : %X\n",
                  pNotification->ConnHandle,
                  *tmap_role);
      break;
    }
  }
}

uint8_t TMAPAPP_StartScanning(void)
{
  uint8_t status;

  status = aci_gap_start_general_connection_establish_proc(0x00, 0x50, 0x50, 0x00, 0x00, 0x01);
  LOG_INFO_APP("aci_gap_start_general_connection_establish_proc() returns status 0x%02X\n",status);

  return status;
}

uint8_t TMAPAPP_StopScanning(void)
{
  uint8_t status;

  status = aci_gap_terminate_gap_proc(GAP_GENERAL_CONNECTION_ESTABLISHMENT_PROC);
  LOG_INFO_APP("aci_gap_terminate_gap_proc() returns status 0x%02X\n",status);

  return status;
}

uint8_t TMAPAPP_CreateConnection(uint8_t *pAddress, uint8_t AddressType)
{
  uint8_t status;

  status = aci_gap_create_connection(0x0400,
                                     0x0400,
                                     AddressType,
                                     pAddress,
                                     0x00,
                                     0x20,
                                     0x20,
                                     0x0000,
                                     0x03E8,
                                     0x0000,
                                     0x03E8);
  LOG_INFO_APP("aci_gap_create_connection() returns status 0x%02X\n",status);
  return status;
}

uint8_t TMAPAPP_Disconnect(void)
{
  uint8_t status;

  status = hci_disconnect(0x01, HCI_REMOTE_USER_TERMINATED_CONNECTION_ERR_CODE);
  LOG_INFO_APP("hci_disconnect() returns status 0x%02X\n",status);

  return status;
}

uint8_t TMAPAPP_StartMediaStream(void)
{
  uint8_t status;
  uint8_t metadata_snk_len = 0;

  TMAPAPP_Context.audio_driver_config = AUDIO_DRIVER_CONFIG_LINEIN;

  StartStreamParams.ConnHandle = TMAPAPP_Context.ACL_Conn[0].Acl_Conn_Handle;
  StartStreamParams.StreamRole = AUDIO_ROLE_SINK;
  CodecConfSnk.TargetLatency = TARGET_LATENCY_LOW;
  CodecConfSnk.TargetPhy = TARGET_LE_2M_PHY;
  CodecConfSnk.CodecID.CodingFormat = 0x06;
  CodecConfSnk.CodecID.CompanyID = 0x0000;
  CodecConfSnk.CodecID.VsCodecID = 0x0000;
  CodecConfSnk.CodecSpecificConfParams.SamplingFrequency = APP_CodecConf[MEDIA_STREAM_QOS_CONFIG%16].freq;
  CodecConfSnk.CodecSpecificConfParams.FrameDuration = APP_CodecConf[MEDIA_STREAM_QOS_CONFIG%16].frame_duration;
  CodecConfSnk.CodecSpecificConfParams.FrameBlockPerSdu = 1;
  CodecConfSnk.CodecSpecificConfParams.OctetPerCodecFrame = APP_CodecConf[MEDIA_STREAM_QOS_CONFIG%16].octets_per_codec_frame;
  CodecConfSnk.AudioChannelAllocation = MEDIA_STREAM_AUDIO_CHANNEL_ALLOC;
  CodecConfSnk.ChannelPerCIS = MEDIA_STREAM_CHANNEL_PER_CIS;
  QosConfSnk.SDUInterval = APP_QoSConf[MEDIA_STREAM_QOS_CONFIG].sdu_interval;
  QosConfSnk.Framing = APP_QoSConf[MEDIA_STREAM_QOS_CONFIG].framing;
  QosConfSnk.MaxSDU = APP_QoSConf[MEDIA_STREAM_QOS_CONFIG].max_sdu \
    * MIN(APP_GetBitsAudioChnlAllocations(CodecConfSnk.AudioChannelAllocation), CodecConfSnk.ChannelPerCIS);
  QosConfSnk.RetransmissionNumber = APP_QoSConf[MEDIA_STREAM_QOS_CONFIG].rtx_num;
  QosConfSnk.MaxTransportLatency = APP_QoSConf[MEDIA_STREAM_QOS_CONFIG].max_tp_latency;
  QosConfSnk.PresentationDelay = APP_QoSConf[MEDIA_STREAM_QOS_CONFIG].presentation_delay;

  aMetadataSnk[metadata_snk_len++] = 0x03;
  aMetadataSnk[metadata_snk_len++] = METADATA_STREAMING_AUDIO_CONTEXTS;
  aMetadataSnk[metadata_snk_len++] = (uint8_t) AUDIO_CONTEXT_MEDIA;
  aMetadataSnk[metadata_snk_len++] = (uint8_t) (AUDIO_CONTEXT_MEDIA >> 8);
  aMetadataSnk[metadata_snk_len++] = 0x02;
  aMetadataSnk[metadata_snk_len++] = METADATA_CCID_LIST;
  aMetadataSnk[metadata_snk_len++] = 0x00;

  StartStreamParams.StreamSnk.pCodecConf = &CodecConfSnk;
  StartStreamParams.StreamSnk.pQoSConf = &QosConfSnk;
  StartStreamParams.StreamSnk.pMetadata = &aMetadataSnk[0];
  StartStreamParams.StreamSnk.MetadataLen = metadata_snk_len;

  StartStreamParams.StreamSrc.pCodecConf = &CodecConfSrc;
  StartStreamParams.StreamSrc.pQoSConf = &QosConfSrc;
  StartStreamParams.StreamSrc.pMetadata = &aMetadataSrc[0];
  StartStreamParams.StreamSrc.MetadataLen = 0;

  status = CAP_Unicast_AudioStart(CAP_SET_TYPE_AD_HOC, 1, &StartStreamParams);
  LOG_INFO_APP("CAP_Unicast_AudioStart with status %02X\n", status);

  return status;
}

uint8_t TMAPAPP_StartTelephonyStream(void)
{
  uint8_t status;
  uint8_t metadata_snk_len = 0;
  uint8_t metadata_src_len = 0;

  TMAPAPP_Context.audio_driver_config = AUDIO_DRIVER_CONFIG_HEADSET;

  StartStreamParams.ConnHandle = TMAPAPP_Context.ACL_Conn[0].Acl_Conn_Handle;
  StartStreamParams.StreamRole = AUDIO_ROLE_SINK | AUDIO_ROLE_SOURCE;

  CodecConfSnk.TargetLatency = TARGET_LATENCY_LOW;
  CodecConfSnk.TargetPhy = TARGET_LE_2M_PHY;
  CodecConfSnk.CodecID.CodingFormat = 0x06;
  CodecConfSnk.CodecID.CompanyID = 0x0000;
  CodecConfSnk.CodecID.VsCodecID = 0x0000;
  CodecConfSnk.CodecSpecificConfParams.SamplingFrequency = APP_CodecConf[TELEPHONY_STREAM_QOS_CONFIG%16].freq;
  CodecConfSnk.CodecSpecificConfParams.FrameDuration = APP_CodecConf[TELEPHONY_STREAM_QOS_CONFIG%16].frame_duration;
  CodecConfSnk.CodecSpecificConfParams.FrameBlockPerSdu = 1;
  CodecConfSnk.CodecSpecificConfParams.OctetPerCodecFrame = APP_CodecConf[TELEPHONY_STREAM_QOS_CONFIG%16].octets_per_codec_frame;
  CodecConfSnk.AudioChannelAllocation = TELEPHONY_STREAM_AUDIO_CHANNEL_ALLOC;
  CodecConfSnk.ChannelPerCIS = TELEPHONY_STREAM_CHANNEL_PER_CIS;
  QosConfSnk.SDUInterval = APP_QoSConf[TELEPHONY_STREAM_QOS_CONFIG].sdu_interval;
  QosConfSnk.Framing = APP_QoSConf[TELEPHONY_STREAM_QOS_CONFIG].framing;
  QosConfSnk.MaxSDU = APP_QoSConf[TELEPHONY_STREAM_QOS_CONFIG].max_sdu \
    * MIN(APP_GetBitsAudioChnlAllocations(CodecConfSnk.AudioChannelAllocation), CodecConfSnk.ChannelPerCIS);
  QosConfSnk.RetransmissionNumber = APP_QoSConf[TELEPHONY_STREAM_QOS_CONFIG].rtx_num;
  QosConfSnk.MaxTransportLatency = APP_QoSConf[TELEPHONY_STREAM_QOS_CONFIG].max_tp_latency;
  QosConfSnk.PresentationDelay = APP_QoSConf[TELEPHONY_STREAM_QOS_CONFIG].presentation_delay;
  aMetadataSnk[metadata_snk_len++] = 0x03;
  aMetadataSnk[metadata_snk_len++] = METADATA_STREAMING_AUDIO_CONTEXTS;
  aMetadataSnk[metadata_snk_len++] = (uint8_t) AUDIO_CONTEXT_CONVERSATIONAL;
  aMetadataSnk[metadata_snk_len++] = (uint8_t) (AUDIO_CONTEXT_CONVERSATIONAL >> 8);
  aMetadataSnk[metadata_snk_len++] = 0x02;
  aMetadataSnk[metadata_snk_len++] = METADATA_CCID_LIST;
  aMetadataSnk[metadata_snk_len++] = 0x00;

  CodecConfSrc.TargetLatency = TARGET_LATENCY_LOW;
  CodecConfSrc.TargetPhy = TARGET_LE_2M_PHY;
  CodecConfSrc.CodecID.CodingFormat = 0x06;
  CodecConfSrc.CodecID.CompanyID = 0x0000;
  CodecConfSrc.CodecID.VsCodecID = 0x0000;
  CodecConfSrc.CodecSpecificConfParams.SamplingFrequency = APP_CodecConf[TELEPHONY_STREAM_QOS_CONFIG%16].freq;
  CodecConfSrc.CodecSpecificConfParams.FrameDuration = APP_CodecConf[TELEPHONY_STREAM_QOS_CONFIG%16].frame_duration;
  CodecConfSrc.CodecSpecificConfParams.FrameBlockPerSdu = 1;
  CodecConfSrc.CodecSpecificConfParams.OctetPerCodecFrame = APP_CodecConf[TELEPHONY_STREAM_QOS_CONFIG%16].octets_per_codec_frame;
  CodecConfSrc.AudioChannelAllocation = TELEPHONY_STREAM_AUDIO_CHANNEL_ALLOC;
  CodecConfSrc.ChannelPerCIS = TELEPHONY_STREAM_CHANNEL_PER_CIS;
  QosConfSrc.SDUInterval = APP_QoSConf[TELEPHONY_STREAM_QOS_CONFIG].sdu_interval;
  QosConfSrc.Framing = APP_QoSConf[TELEPHONY_STREAM_QOS_CONFIG].framing;
  QosConfSrc.MaxSDU = APP_QoSConf[TELEPHONY_STREAM_QOS_CONFIG].max_sdu \
    * MIN(APP_GetBitsAudioChnlAllocations(CodecConfSrc.AudioChannelAllocation), CodecConfSrc.ChannelPerCIS);
  QosConfSrc.RetransmissionNumber = APP_QoSConf[TELEPHONY_STREAM_QOS_CONFIG].rtx_num;
  QosConfSrc.MaxTransportLatency = APP_QoSConf[TELEPHONY_STREAM_QOS_CONFIG].max_tp_latency;
  QosConfSrc.PresentationDelay = APP_QoSConf[TELEPHONY_STREAM_QOS_CONFIG].presentation_delay;
  aMetadataSrc[metadata_src_len++] = 0x03;
  aMetadataSrc[metadata_src_len++] = METADATA_STREAMING_AUDIO_CONTEXTS;
  aMetadataSrc[metadata_src_len++] = (uint8_t) AUDIO_CONTEXT_CONVERSATIONAL;
  aMetadataSrc[metadata_src_len++] = (uint8_t) (AUDIO_CONTEXT_CONVERSATIONAL >> 8);
  aMetadataSrc[metadata_src_len++] = 0x02;
  aMetadataSrc[metadata_src_len++] = METADATA_CCID_LIST;
  aMetadataSrc[metadata_src_len++] = 0x00;

  StartStreamParams.StreamSnk.pCodecConf = &CodecConfSnk;
  StartStreamParams.StreamSnk.pQoSConf = &QosConfSnk;
  StartStreamParams.StreamSnk.pMetadata = &aMetadataSnk[0];
  StartStreamParams.StreamSnk.MetadataLen = metadata_snk_len;

  StartStreamParams.StreamSrc.pCodecConf = &CodecConfSrc;
  StartStreamParams.StreamSrc.pQoSConf = &QosConfSrc;
  StartStreamParams.StreamSrc.pMetadata = &aMetadataSrc[0];
  StartStreamParams.StreamSrc.MetadataLen = metadata_src_len;

  status = CAP_Unicast_AudioStart(CAP_SET_TYPE_AD_HOC, 1, &StartStreamParams);
  LOG_INFO_APP("CAP_Unicast_AudioStart with status %02X\n", status);

  return status;
}

uint8_t TMAPAPP_StopStream(void)
{
  uint8_t status;

  status = CAP_Unicast_AudioStop(1, &TMAPAPP_Context.ACL_Conn[0].Acl_Conn_Handle, 1);
  LOG_INFO_APP("CAP_Unicast_AudioStop with status %02X\n", status);

  return status;
}

uint8_t TMAPAPP_RemoteVolumeUp(void)
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

    status = CAP_VolumeController_StartSetVolumeProcedure(TMAPAPP_Context.ACL_Conn[0].Acl_Conn_Handle, volume);
    LOG_INFO_APP("CAP_VolumeController_StartSetVolumeProcedure() with Volume %d returns status 0x%02X\n",
                 volume,
                 status);
  }
  else
  {
    status = TMAPAPP_RemoteToggleMute();
  }

  return status;
#else /* (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u) */
  return HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif /* (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u) */
}

uint8_t TMAPAPP_RemoteVolumeDown(void)
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

    status = CAP_VolumeController_StartSetVolumeProcedure(TMAPAPP_Context.ACL_Conn[0].Acl_Conn_Handle, volume);
    LOG_INFO_APP("CAP_VolumeController_StartSetVolumeProcedure() with Volume %d returns status 0x%02X\n",
                 volume,
                 status);
  }
  else
  {
    status = TMAPAPP_RemoteToggleMute();
  }

  return status;
#else /* (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u) */
  return HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif /* (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u) */
}

uint8_t TMAPAPP_RemoteToggleMute(void)
{
#if (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u)
  uint8_t status = BLE_STATUS_SUCCESS;
  uint8_t mute = (RemoteMute + 1) % 2;

  status = CAP_VolumeController_StartSetVolumeMuteStateProcedure(TMAPAPP_Context.ACL_Conn[0].Acl_Conn_Handle, mute);
  LOG_INFO_APP("CAP_VolumeController_StartSetVolumeMuteStateProcedure() with Mute %d returns status 0x%02X\n",
               mute, status);

  return status;
#else /* (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u) */
  return HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif /* (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u) */
}

void TMAPAPP_LocalVolumeUp(void)
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

void TMAPAPP_LocalVolumeDown(void)
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

void TMAPAPP_LocalToggleMute(void)
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

tBleStatus TMAPAPP_NextTrack(void)
{
#if (APP_MCP_ROLE_SERVER_SUPPORT == 1u)
  tBleStatus status;
  TMAPAPP_MediaPlayer_t *p_mediaplayer = TMAPAPP_GetMediaPlayer(TMAPAPP_Context.GenericMediaPlayer.CCID);

  if( p_mediaplayer->TrackID == APP_MCP_NUM_TRACKS)
  {
   p_mediaplayer->TrackID = 1u;
  }
  else
  {
    p_mediaplayer->TrackID += 1u;
  }

  LOG_INFO_APP("[MP %d] Change to Track %d\n",
                TMAPAPP_Context.GenericMediaPlayer.CCID,
                p_mediaplayer->TrackID);
  status = MCP_SERVER_SetTrackTitle(TMAPAPP_Context.GenericMediaPlayer.CCID,
                                    MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].pTitle,
                                    6u);
  LOG_INFO_APP("[MP %d] Set Track Title of Track %d in Group %d : status 0x%02X\n",
                TMAPAPP_Context.GenericMediaPlayer.CCID,
                p_mediaplayer->TrackID,
                p_mediaplayer->GroupID,
                status);
  if (status == BLE_STATUS_SUCCESS)
  {
    Menu_SetTrackTitle(MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].pTitle,
                     MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].TitleLen);
  }
  status = MCP_SERVER_SetTrackDuration(TMAPAPP_Context.GenericMediaPlayer.CCID,
                                       MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].Duration);
  LOG_INFO_APP("[MP %d] Set Track Duration (%d) of Track %d in Group %d : status 0x%02X\n",
                TMAPAPP_Context.GenericMediaPlayer.CCID,
                MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].Duration,
                p_mediaplayer->TrackID,
                p_mediaplayer->GroupID,
                status);
  p_mediaplayer->TrackPosition = 0;
  status =  MCP_SERVER_SetTrackPosition(TMAPAPP_Context.GenericMediaPlayer.CCID,p_mediaplayer->TrackPosition);
  LOG_INFO_APP("[MP %d] Set Track Position (%d) of Track %d in Group %d : status 0x%02X\n",
                TMAPAPP_Context.GenericMediaPlayer.CCID,
                (int32_t)p_mediaplayer->TrackPosition,
                p_mediaplayer->TrackID,
                p_mediaplayer->GroupID,
                status);
  status = MCP_SERVER_NotifyTrackChanged(TMAPAPP_Context.GenericMediaPlayer.CCID);
  LOG_INFO_APP("[MP %d] Track Change Notification status 0x%02X\n",TMAPAPP_Context.GenericMediaPlayer.CCID);

  return status;
#else /* (APP_MCP_ROLE_SERVER_SUPPORT == 1u) */
  return HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif /* (APP_MCP_ROLE_SERVER_SUPPORT == 1u) */
}

tBleStatus TMAPAPP_PreviousTrack(void)
{
#if (APP_MCP_ROLE_SERVER_SUPPORT == 1u)
  tBleStatus status;
  TMAPAPP_MediaPlayer_t *p_mediaplayer = TMAPAPP_GetMediaPlayer(TMAPAPP_Context.GenericMediaPlayer.CCID);

  if( p_mediaplayer->TrackID > 1u)
  {
   p_mediaplayer->TrackID -= 1u;
  }
  else
  {
    p_mediaplayer->TrackID = 1u;
  }

  LOG_INFO_APP("[MP %d] Change to Track %d\n",
                TMAPAPP_Context.GenericMediaPlayer.CCID,
                p_mediaplayer->TrackID);
  status = MCP_SERVER_SetTrackTitle(TMAPAPP_Context.GenericMediaPlayer.CCID,
                                    MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].pTitle,
                                    6u);
  LOG_INFO_APP("[MP %d] Set Track Title of Track %d in Group %d : status 0x%02X\n",
                TMAPAPP_Context.GenericMediaPlayer.CCID,
                p_mediaplayer->TrackID,
                p_mediaplayer->GroupID,
                status);
  if (status == BLE_STATUS_SUCCESS)
  {
    Menu_SetTrackTitle(MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].pTitle,
                       MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].TitleLen);
  }
  status = MCP_SERVER_SetTrackDuration(TMAPAPP_Context.GenericMediaPlayer.CCID,
                                       MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].Duration);
  LOG_INFO_APP("[MP %d] Set Track Duration (%d) of Track %d in Group %d : status 0x%02X\n",
                TMAPAPP_Context.GenericMediaPlayer.CCID,
                MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].Duration,
                p_mediaplayer->TrackID,
                p_mediaplayer->GroupID,
                status);
  p_mediaplayer->TrackPosition = 0;
  status =  MCP_SERVER_SetTrackPosition(TMAPAPP_Context.GenericMediaPlayer.CCID,p_mediaplayer->TrackPosition);
  LOG_INFO_APP("[MP %d] Set Track Position (%d) of Track %d in Group %d : status 0x%02X\n",
                TMAPAPP_Context.GenericMediaPlayer.CCID,
                (int32_t)p_mediaplayer->TrackPosition,
                p_mediaplayer->TrackID,
                p_mediaplayer->GroupID,
                status);
  status = MCP_SERVER_NotifyTrackChanged(TMAPAPP_Context.GenericMediaPlayer.CCID);
  LOG_INFO_APP("[MP %d] Track Change Notification status 0x%02X\n", TMAPAPP_Context.GenericMediaPlayer.CCID);

  return status;
#else /* (APP_MCP_ROLE_SERVER_SUPPORT == 1u) */
  return HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif /* (APP_MCP_ROLE_SERVER_SUPPORT == 1u) */
}

tBleStatus TMAPAPP_PlayPause(void)
{
#if (APP_MCP_ROLE_SERVER_SUPPORT == 1u)
  tBleStatus status = BLE_STATUS_SUCCESS;
  TMAPAPP_MediaPlayer_t *p_mediaplayer = TMAPAPP_GetMediaPlayer(TMAPAPP_Context.GenericMediaPlayer.CCID);

  switch (p_mediaplayer->MediaState)
  {
    case MCP_MEDIA_STATE_INACTIVE:
    case MCP_MEDIA_STATE_PAUSED:
    {
      status = TMAPAPP_SetMediaState(TMAPAPP_Context.GenericMediaPlayer.CCID, MCP_MEDIA_STATE_PLAYING);
    }
    break;
    case MCP_MEDIA_STATE_PLAYING:
    case MCP_MEDIA_STATE_SEEKING:
    {
      status = TMAPAPP_SetMediaState(TMAPAPP_Context.GenericMediaPlayer.CCID, MCP_MEDIA_STATE_PAUSED);
    }
    break;
  }

  return status;
#else /* (APP_MCP_ROLE_SERVER_SUPPORT == 1u) */
  return HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif /* (APP_MCP_ROLE_SERVER_SUPPORT == 1u) */
}

tBleStatus TMAPAPP_AnswerCall(void)
{
#if (APP_CCP_ROLE_SERVER_SUPPORT == 1u)
  tBleStatus status;

  status = CCP_SERVER_SetActiveCall(TMAPAPP_Context.GenericBearer.CCID,
                                    TMAPAPP_Context.ACL_Conn[0].CurrentCallID);
  LOG_INFO_APP("CCP_SERVER_SetActiveCall() returns status 0x%02X\n", status);

  return status;
#else /* (APP_CCP_ROLE_SERVER_SUPPORT == 1u) */
  return HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif /* (APP_CCP_ROLE_SERVER_SUPPORT == 1u) */
}

tBleStatus TMAPAPP_TerminateCall(void)
{
#if (APP_CCP_ROLE_SERVER_SUPPORT == 1u)
  tBleStatus status;

  status = CCP_SERVER_TerminateCall(TMAPAPP_Context.GenericBearer.CCID,
                                    TMAPAPP_Context.ACL_Conn[0].CurrentCallID,
                                    CCP_CALL_TERMINATION_SERVER_END);
  LOG_INFO_APP("CCP_SERVER_TerminateCall() returns status 0x%02X\n", status);

  return status;
#else /* (APP_CCP_ROLE_SERVER_SUPPORT == 1u) */
  return HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif /* (APP_CCP_ROLE_SERVER_SUPPORT == 1u) */
}

tBleStatus TMAPAPP_IncomingCall(void)
{
#if (APP_CCP_ROLE_SERVER_SUPPORT == 1u)
  tBleStatus status;
  uint8_t incoming_call_uri[14] = {'t','e','l',':','0','1','0','2','0','3','0','4','0','5'};

  status = CCP_SERVER_SetupIncomingCall(TMAPAPP_Context.GenericBearer.CCID, &incoming_call_uri[0], 14, 0, 0,
                                        &TMAPAPP_Context.ACL_Conn[0].CurrentCallID);
  LOG_INFO_APP("CCP_SERVER_SetupIncomingCall() returns status 0x%02X\n", status);

  return status;
#else /* (APP_CCP_ROLE_SERVER_SUPPORT == 1u) */
  return HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif /* (APP_CCP_ROLE_SERVER_SUPPORT == 1u) */
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
    if (TMAPAPP_Context.cis_snk_handle[i] != 0xFFFFu)
    {
      CODEC_ReceiveData(TMAPAPP_Context.cis_snk_handle[i], 1, &aSnkBuff[0] + AudioFrameSize/2 + i);
    }
  }
}

void APP_NotifyTxAudioHalfCplt(void)
{
  uint8_t i;
  for (i = 0; i< APP_MAX_NUM_CIS; i++)
  {
    if (TMAPAPP_Context.cis_snk_handle[i] != 0xFFFFu)
    {
      CODEC_ReceiveData(TMAPAPP_Context.cis_snk_handle[i], 1, &aSnkBuff[0] + i);
    }
  }
}

extern uint32_t Sink_frame_size;

void CODEC_NotifyDataReady(uint16_t conn_handle, void* decoded_data)
{
  /* When only one channel is active, duplicate it for fake stereo on SAI */
  uint32_t i;
  uint16_t *pData = (uint16_t *)(decoded_data);   /* 16 bits samples */

  if(pData == (pPrevData + 1)){
    /* we start receiving two channels */
    Nb_Active_Ch = 2;
  }
  pPrevData = pData;

  if (Nb_Active_Ch < 2)
  {
    for (i = 0; i < Sink_frame_size/2; i+=2)
    {
      /* decoded_data is organized with a decimation equal to 2, so we duplicate each sample */
      pData[i+1] = pData[i];
    }
  }
}

void APP_NotifyRxAudioCplt(uint16_t AudioFrameSize)
{
  uint8_t i;

  for (i = 0; i< APP_MAX_NUM_CIS; i++)
  {
    if (TMAPAPP_Context.cis_src_handle[i] != 0xFFFFu)
    {
      CODEC_SendData(TMAPAPP_Context.cis_src_handle[i], 1, &aSrcBuff[0] + AudioFrameSize/2 + i);
    }
  }
}

void APP_NotifyRxAudioHalfCplt(void)
{
  uint8_t i;

  for (i = 0; i< APP_MAX_NUM_CIS; i++)
  {
    if (TMAPAPP_Context.cis_src_handle[i] != 0xFFFFu)
    {
      CODEC_SendData(TMAPAPP_Context.cis_src_handle[i], 1, &aSrcBuff[0]+i);
    }
  }
}

void TMAPAPP_AclConnected(uint16_t ConnHandle, uint8_t Peer_Address_Type, uint8_t Peer_Address[6], uint8_t role)
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
    p_conn->Peer_Address_Type = Peer_Address_Type;
    for (uint8_t i = 0 ; i < 6u; i++)
    {
      p_conn->Peer_Address[i] = Peer_Address[i];
    }
    p_conn->Role = role;
    for (uint8_t i = 0; i < CFG_BLE_NUM_LINK;i++)
    {
      if (TMAPAPP_Context.aASEs[i].acl_conn_handle == 0xFFFFu)
      {
        p_conn->pASEs = &TMAPAPP_Context.aASEs[i];
        p_conn->pASEs->acl_conn_handle = ConnHandle;
        break;
      }
    }
  }
  status = aci_gap_send_pairing_req(ConnHandle, 0x00);
  LOG_INFO_APP("aci_gap_send_pairing_req returns %d\n",status);
}

void TMAPAPP_ConfirmIndicationRequired(uint16_t Conn_Handle)
{
  APP_ACL_Conn_t *p_conn = APP_GetACLConn(Conn_Handle);
  if (p_conn != 0)
  {
    p_conn->ConfirmIndicationRequired = 1u;
  }
}

void TMAPAPP_BondLost(uint16_t Conn_Handle)
{
  /* Force COMPLETE_LINKUP */
  APP_ACL_Conn_t *p_conn = APP_GetACLConn(Conn_Handle);
  if (p_conn != 0)
  {
    p_conn->ForceCompleteLinkup = 1u;
  }
}

void TMAPAPP_CISConnected(uint16_t Conn_Handle)
{
  for (uint8_t i = 0; i< APP_MAX_NUM_CIS; i++)
  {
    if (TMAPAPP_Context.cis_handle[i] == 0xFFFFu)
    {
      TMAPAPP_Context.cis_handle[i] = Conn_Handle;
      TMAPAPP_Context.num_cis_established++;
      break;
    }
  }
}

void TMAPAPP_LinkDisconnected(uint16_t Conn_Handle)
{
  uint8_t i;
  APP_ACL_Conn_t *p_conn = APP_GetACLConn(Conn_Handle);
  if (p_conn != 0)
  {
    uint8_t status;
    /*Free CAP Connection Slot*/
    p_conn->Acl_Conn_Handle = 0xFFFFu;
    p_conn->ForceCompleteLinkup = 0x00u;
    p_conn->ConfirmIndicationRequired = 0x00u;
    p_conn->AudioProfile = AUDIO_PROFILE_NONE;

    status = TMAPAPP_StartScanning();
    LOG_INFO_APP("TMAPAPP_StartScanning() returns status 0x%02X\n",status);

    Menu_SetScanningPage();
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
  }
  else
  {
    /* Check if the Connection Handle corresponds to a CIS link */
    for (uint8_t i = 0; i< APP_MAX_NUM_CIS; i++)
    {
      if (TMAPAPP_Context.cis_handle[i] == Conn_Handle)
      {
        TMAPAPP_Context.cis_handle[i] = 0xFFFFu;
        TMAPAPP_Context.num_cis_established--;
        if (TMAPAPP_Context.num_cis_established == 0u)
        {
          LOG_INFO_APP("Deinitialized Audio Peripherals\n");
          MX_AudioDeInit();
        }
        break;
      }
    }
  }
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

  APP_BAP_Config.Role = BAP_ROLE_UNICAST_CLIENT;
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

#if (APP_CCP_ROLE_SERVER_SUPPORT == 1)
  APP_CCP_Config.Role |= CCP_ROLE_SERVER;
  APP_CCP_Config.MaxNumBleLinks = CFG_BLE_NUM_LINK;
  APP_CCP_Config.Srv.ExtFeatures = APP_CCP_FEATURES;
  APP_CCP_Config.Srv.MaxNumTBInstances = APP_CCP_NUM_LOCAL_BEARER_INSTANCES;
  APP_CCP_Config.Srv.MaxNumCalls = APP_CCP_NUM_CALLS;
  APP_CCP_Config.Srv.MaxCallURILength = APP_CCP_CALL_URI_LENGTH;
  APP_CCP_Config.Srv.MaxBearerProviderNameLength = APP_CCP_BEARER_PROVIDER_NAME_LENGTH;
  APP_CCP_Config.Srv.MaxBearerUCILength = APP_CCP_BEARER_UCI_LENGTH;
  APP_CCP_Config.Srv.MaxBearerURISchemesListLength = APP_CCP_BEARER_URI_SCHEMES_SUPPORTED_LIST_LENGTH;
  APP_CCP_Config.Srv.MaxCallFriendlyNameLength = APP_CCP_CALL_FRIENDLY_NAME_LENGTH;
  APP_CCP_Config.Srv.ReadLongEn = APP_CCP_READLONG_CFG;
  APP_CCP_Config.Srv.pStartRamAddr = (uint8_t*)aCCPSrvMemBuffer;
  APP_CCP_Config.Srv.RamSize = BLE_CCP_SRV_DYN_ALLOC_SIZE;
#endif /* (APP_CCP_ROLE_SERVER_SUPPORT == 1) */

#if (APP_MCP_ROLE_SERVER_SUPPORT == 1)
  APP_MCP_Config.Role |= MCP_ROLE_SERVER;
  APP_MCP_Config.MaxNumBleLinks = CFG_BLE_NUM_LINK;
  APP_MCP_Config.Srv.MaxNumMPInstances = APP_MCP_NUM_LOCAL_MEDIA_PLAYER_INSTANCES;
  APP_MCP_Config.Srv.MaxMediaPlayerNameLength = APP_MCP_MEDIA_PLAYER_NAME_LENGTH;
  APP_MCP_Config.Srv.MaxTrackTitleLength = APP_MCP_TRACK_TITLE_LENGTH;
  APP_MCP_Config.Srv.ReadLongEn = APP_MCP_READLONG_CFG;
  APP_MCP_Config.Srv.OptionFeatures = APP_MCP_SRV_FEATURE_OPTIONS;
  APP_MCP_Config.Srv.pStartRamAddr = (uint8_t*)aMCPSrvMemBuffer;
  APP_MCP_Config.Srv.RamSize = BLE_MCP_SRV_DYN_ALLOC_SIZE;
#endif /* (APP_MCP_ROLE_SERVER_SUPPORT == 1) */

#if (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1)
  APP_VCP_Config.Role = VCP_ROLE_CONTROLLER;
  APP_VCP_Config.MaxNumBleLinks = CFG_BLE_NUM_LINK;
  APP_VCP_Config.Controller.MaxNumAICInstPerConn = APP_VCP_CTLR_NUM_AIC_INSTANCES;
  APP_VCP_Config.Controller.MaxNumVOCInstPerConn = APP_VCP_CTLR_NUM_VOC_INSTANCES;
  APP_VCP_Config.Controller.pStartRamAddr = (uint8_t*)aCltrMemBuffer;
  APP_VCP_Config.Controller.RamSize = BLE_VCP_CTLR_DYN_ALLOC_SIZE;
#endif /* (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1) */

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

  for (uint8_t i = 0; i< APP_MAX_NUM_CIS; i++)
  {
    TMAPAPP_Context.cis_src_handle[i] = 0xFFFFu;
    TMAPAPP_Context.cis_snk_handle[i] = 0xFFFFu;
    TMAPAPP_Context.cis_handle[i] = 0xFFFFu;
  }

  TMAPAPP_Context.bap_role = APP_BAP_Config.Role;
  for (uint8_t conn = 0; conn< CFG_BLE_NUM_LINK; conn++)
  {
    TMAPAPP_Context.aASEs[conn].acl_conn_handle = 0xFFFFu;
#if (MAX_NUM_UCL_SNK_ASE_PER_LINK > 0u)
    for (uint8_t i = 0; i< MAX_NUM_UCL_SNK_ASE_PER_LINK; i++)
    {
        TMAPAPP_Context.aASEs[conn].aSnkASE[i].ID = 0x00;
        TMAPAPP_Context.aASEs[conn].aSnkASE[i].type = ASE_SINK;
        TMAPAPP_Context.aASEs[conn].aSnkASE[i].state  = ASE_STATE_IDLE;
        TMAPAPP_Context.aASEs[conn].aSnkASE[i].num_channels  = 0x00;
        TMAPAPP_Context.aASEs[conn].aSnkASE[i].allocated = 0x00u;
    }
#endif /*(MAX_NUM_UCL_SNK_ASE_PER_LINK > 0u)*/

#if (MAX_NUM_UCL_SRC_ASE_PER_LINK > 0u)
    for (uint8_t i = 0; i< MAX_NUM_UCL_SRC_ASE_PER_LINK; i++)
    {
        TMAPAPP_Context.aASEs[conn].aSrcASE[i].ID = 0x00;
        TMAPAPP_Context.aASEs[conn].aSrcASE[i].state  = ASE_STATE_IDLE;
        TMAPAPP_Context.aASEs[conn].aSrcASE[i].type = ASE_SOURCE;
        TMAPAPP_Context.aASEs[conn].aSrcASE[i].num_channels  = 0x00;
        TMAPAPP_Context.aASEs[conn].aSrcASE[i].allocated = 0x00u;
    }
#endif /*(MAX_NUM_UCL_SRC_ASE_PER_LINK > 0u)*/
  }

  for (uint8_t i = 0; i< CFG_BLE_NUM_LINK; i++)
  {
    TMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle = 0xFFFFu;
    TMAPAPP_Context.ACL_Conn[i].AudioProfile = AUDIO_PROFILE_NONE;
    TMAPAPP_Context.ACL_Conn[i].ForceCompleteLinkup = 0x00u;
    TMAPAPP_Context.ACL_Conn[i].ConfirmIndicationRequired = 0x00u;
  }

  /*Register the Audio Task */
  UTIL_SEQ_RegTask( 1<<CFG_TASK_AUDIO_ID, UTIL_SEQ_RFU, BLE_AUDIO_STACK_Task);
  /*Initialize the Audio Codec (LC3)*/
  CODEC_LC3Config_t lc3_config;

  lc3_config.NumDecChannel = CODEC_LC3_NUM_DECODER_CHANNEL;
  lc3_config.NumEncChannel = CODEC_LC3_NUM_ENCODER_CHANNEL;
  lc3_config.NumSession = CODEC_LC3_NUM_SESSION;

  lc3_config.pDecChannelStart = aLC3DecoderMemBuffer;
  lc3_config.pEncChannelStart = aLC3EncoderMemBuffer;
  lc3_config.pSessionStart = aLC3SessionMemBuffer;

  CODEC_ManagerInit(MAX_PATH_NB*CODEC_POOL_SUB_SIZE,
                        (uint8_t*)aCodecPacketsMemory,
                        &lc3_config,
                        100,
                        1650,
                        CODEC_MODE_DEFAULT);

#if (APP_CCP_ROLE_SERVER_SUPPORT == 1u)
#if (APP_CCP_NUM_LOCAL_BEARER_INSTANCES > 0u)
    for(uint8_t i = 0u ; i < APP_CCP_NUM_LOCAL_BEARER_INSTANCES ; i++)
    {
      TMAPAPP_Context.BearerInstance[i].CCID = 0u;
    }
#endif /*(APP_CCP_NUM_LOCAL_BEARER_INSTANCES > 0u)*/

      if ((APP_CCP_Config.Role & CCP_ROLE_SERVER) == CCP_ROLE_SERVER)
      {
        uint8_t ccid;
        /*register the Generic Telephony Bearer and the potential Telephony Bearer Instances*/
        status = TMAPAPP_RegisterGenericTelephonyBearer(&ccid);
#if (APP_CCP_NUM_LOCAL_BEARER_INSTANCES > 0u)
        for(uint8_t i = 0u ; ((i < APP_CCP_NUM_LOCAL_BEARER_INSTANCES) && (status == BLE_STATUS_SUCCESS)) ; i++)
        {
          status = TMAPAPP_RegisterTelephonyBearerInstance(&ccid);
        }
#endif /*(APP_CCP_NUM_LOCAL_BEARER_INSTANCES > 0u)*/
      }
#endif /*(APP_CCP_ROLE_SERVER_SUPPORT == 1u)*/

#if (APP_MCP_ROLE_SERVER_SUPPORT == 1u)
      TMAPAPP_Context.GenericMediaPlayer.TrackID = 1u;
      TMAPAPP_Context.GenericMediaPlayer.GroupID = 1u;
      TMAPAPP_Context.GenericMediaPlayer.MediaState = MCP_MEDIA_STATE_PAUSED;
#if (APP_MCP_NUM_LOCAL_MEDIA_PLAYER_INSTANCES > 0u)
      for(uint8_t i = 0u ; i < APP_MCP_NUM_LOCAL_MEDIA_PLAYER_INSTANCES ; i++)
      {
        TMAPAPP_Context.MediaPlayerInstance[i].CCID = 0x00;
        TMAPAPP_Context.MediaPlayerInstance[i].TrackID = 1u;
        TMAPAPP_Context.MediaPlayerInstance[i].GroupID = 1u;
        TMAPAPP_Context.MediaPlayerInstance[i].MediaState = MCP_MEDIA_STATE_PAUSED;
      }
#endif /*(APP_MCP_NUM_LOCAL_MEDIA_PLAYER_INSTANCES > 0u)*/
      if ((APP_MCP_Config.Role & MCP_ROLE_SERVER) == MCP_ROLE_SERVER)
      {
        uint8_t ccid;
        /*register the Generic Media Player and the potential Media Player Instances*/
        status = TMAPAPP_RegisterGenericMediaPlayer(&ccid);
#if (APP_MCP_NUM_LOCAL_MEDIA_PLAYER_INSTANCES > 0u)
        for(uint8_t i = 0u ; ((i < APP_MCP_NUM_LOCAL_MEDIA_PLAYER_INSTANCES) && (status == BLE_STATUS_SUCCESS)) ; i++)
        {
          status = TMAPAPP_RegisterMediaPlayerInstance(&ccid);
        }
#endif /*(APP_MCP_NUM_LOCAL_MEDIA_PLAYER_INSTANCES > 0u)*/
      }
#endif /* (APP_MCP_ROLE_SERVER_SUPPORT == 1u)*/

  /*Enable Audio Codec in LE Controller */
  TMAPAPP_Context.aStandardCodec[0].CodingFormat = AUDIO_CODING_FORMAT_LC3;
  TMAPAPP_Context.aStandardCodec[0].TransportMask = 0x01; /*CIS Transport Type only*/
  TMAPAPP_Context.AudioCodecInController.NumStandardCodecs = 0x01;
  TMAPAPP_Context.AudioCodecInController.pStandardCodec = &TMAPAPP_Context.aStandardCodec[0];
  TMAPAPP_Context.AudioCodecInController.NumVendorSpecificCodecs = 0x00;
  TMAPAPP_Context.AudioCodecInController.pVendorSpecificCodec = (void *)0;

  status = CAP_EnableAudioCodecController(&TMAPAPP_Context.AudioCodecInController);
  LOG_INFO_APP("Enable Audio Codec in LE Controller status 0x%02X\n",status);
  if (status != BLE_STATUS_SUCCESS)
  {
    return status;
  }

  TMAPAPP_Context.audio_driver_config = AUDIO_DRIVER_CONFIG_HEADSET;

  /*Clear Flags of the CAPAPP Context*/
  TMAPAPP_Context.audio_role_setup = 0x00;
  TMAPAPP_Context.num_cis_src = 0;
  TMAPAPP_Context.num_cis_snk = 0;

  return status;
}

static tBleStatus TMAPAPP_TMAPInit(uint16_t Role)
{
  TMAP_Config_t tmap_config;

  tmap_config.Role = Role;
  tmap_config.MaxNumBleLinks = CFG_BLE_NUM_LINK;
  tmap_config.pStartRamAddr = (uint8_t *)&aTMAPMemBuffer;
  tmap_config.RamSize = TMAP_DYN_ALLOC_SIZE;

  USECASE_DEV_MGMT_Init();
  return TMAP_Init(&tmap_config);
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
static void TMAPAPP_CAPNotification(CAP_Notification_Evt_t *pNotification)
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
        /* Check if TMAP is already linked or not*/
        if ((p_conn->AudioProfile & AUDIO_PROFILE_TMAP ) == 0)
        {
          tBleStatus ret;
          /* Check if TMAP link is present in NVM from a previous connection*/
          if ((TMAP_DB_IsPresent(p_conn->Peer_Address_Type,p_conn->Peer_Address) == 0) \
              || (p_conn->ForceCompleteLinkup == 1))
          {
            /*Start Complete TMAP link*/
            ret = TMAP_Linkup(pNotification->ConnHandle,TMAP_LINKUP_MODE_COMPLETE);
            LOG_INFO_APP("Complete TMAP Linkup on ConnHandle 0x%04X is started with status 0x%02X\n",
                        pNotification->ConnHandle,
                        ret);
          }
          else
          {
            p_conn->ForceCompleteLinkup = 0;
            /* Restore TMAP link*/
            ret = TMAP_Linkup(pNotification->ConnHandle,TMAP_LINKUP_MODE_RESTORE);
            LOG_INFO_APP("Restore TMAP Linkup on ConnHandle 0x%04X returns status 0x%02X\n",
                        pNotification->ConnHandle,
                        ret);
          }
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
          }
          if (pNotification->Status == BLE_STATUS_SUCCESS)
          {
#if (APP_VCP_ROLE_CONTROLLER_SUPPORT == 1u)
            tBleStatus ret;
            ret = CAP_VolumeController_StartSetVolumeProcedure(pNotification->ConnHandle, BASE_VOLUME);
            LOG_INFO_APP("CAP_VolumeController_StartSetVolumeProcedure() returns status 0x%02X\n", ret);
            Menu_SetRemoteVolume(BASE_VOLUME);
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
      if (p_conn != 0)
      {
        p_conn->AudioProfile |= AUDIO_PROFILE_UNICAST;
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
      break;
    }
    case CAP_UNICAST_AUDIO_DATA_PATH_SETUP_REQ_EVT:
    {
      BAP_SetupAudioDataPathReq_t *info = (BAP_SetupAudioDataPathReq_t *)pNotification->pInfo;
      uint32_t controller_delay;
      APP_ASE_Info_t *p_ase;
      LOG_INFO_APP("Setup Audio Data Path is requested for ASE ID %d (CIS Conn Handle 0x%04X)\n",
                  info->ASE_ID,
                  info->CIS_ConnHandle);
      if (info->PathDirection == BAP_AUDIO_PATH_INPUT){
        LOG_INFO_APP("Input Audio Data Path Configuration is requested\n");
      }
      else
      {
        LOG_INFO_APP("Output Audio Data Path Configuration is requested\n");
      }
      LOG_INFO_APP("  Codec ID\n");
      LOG_INFO_APP("    Coding format : 0x%02X\n",info->CodecConf.CodecID.CodingFormat);
      LOG_INFO_APP("    Company id : 0x%04X\n",info->CodecConf.CodecID.CompanyID);
      LOG_INFO_APP("    VS codec id : 0x%04X\n",info->CodecConf.CodecID.VsCodecID);
      LOG_INFO_APP("  Presentation Delay %d us\n",info->PresentationDelay);
      LOG_INFO_APP("  Min Controller Delay %d us\n",info->ControllerDelayMin);
      LOG_INFO_APP("  Max Controller Delay %d us\n",info->ControllerDelayMax);

      if (TMAPAPP_Context.bap_role & BAP_ROLE_UNICAST_SERVER)
      {
        /*Unicast Server role*/
        if (info->PathDirection == BAP_AUDIO_PATH_INPUT){
          /* Source for Unicast Server*/
          controller_delay = info->PresentationDelay - APP_DELAY_SRC_MIN;
          if ( controller_delay > info->ControllerDelayMax)
          {
            controller_delay = info->ControllerDelayMax;
          }
        }
        else /* (info->PathDirection == BAP_AUDIO_PATH_OUTPUT) */
        {
          /* Sink for Unicast Server*/
          controller_delay = info->PresentationDelay - APP_DELAY_SNK_MIN;
          if ( controller_delay > info->ControllerDelayMax)
          {
            controller_delay = info->ControllerDelayMax;
          }
        }
      }
      else
      {
        /*Unicast Client role*/
        if((info->ControllerDelayMin <= PREFFERED_CONTROLLER_DELAY) \
          && (info->ControllerDelayMax >= PREFFERED_CONTROLLER_DELAY))
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
      }
      p_ase = TMAPAPP_GetASE(info->ASE_ID,pNotification->ConnHandle);
      if (p_ase != 0)
      {
        p_ase->controller_delay = controller_delay;
        p_ase->presentation_delay = info->PresentationDelay;
      }
      TMAPAPP_SetupAudioDataPath(pNotification->ConnHandle,info->CIS_ConnHandle,info->ASE_ID,controller_delay);
      break;
    }
    case CAP_CODEC_CONFIGURED_EVT:
    {
      Sampling_Freq_t *freq = (Sampling_Freq_t *)pNotification->pInfo;
      LOG_INFO_APP("Init Audio Clock with freq %d\n",*freq);
      AudioClock_Init(*freq);
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
        LOG_INFO_APP("Transport Latency : %d ms\n", info->TransportLatency);
        if (pNotification->Status == 0x00)
        {
          TMAPAPP_Context.audio_role_setup |= AUDIO_ROLE_SOURCE;
        }
      }
      else
      {
        Audio_Chnl_Allocation_t channel_alloc = LTV_GetConfiguredAudioChannelAllocation(info->CodecConf.pSpecificConf,
                                                                                        info->CodecConf.SpecificConfLength);
        TMAPAPP_Context.NumOutputChannels += APP_GetBitsAudioChnlAllocations(channel_alloc);
        LOG_INFO_APP("Number of Output Channels Up:  %d\n", TMAPAPP_Context.NumOutputChannels);
        LOG_INFO_APP("Output Audio Data Path is up with status 0x%02X for CIS Conn handle 0x%04X\n",
                    pNotification->Status,
                    info->CIS_ConnHandle);
        LOG_INFO_APP("Controller Delay : %d us\n", info->ControllerDelay);
        LOG_INFO_APP("Transport Latency : %d ms\n", info->TransportLatency);
        if (pNotification->Status == 0x00)
        {
          TMAPAPP_Context.audio_role_setup |= AUDIO_ROLE_SINK;
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
            if (TMAPAPP_Context.cis_src_handle[i] == info->CIS_ConnHandle)
            {
              TMAPAPP_Context.cis_src_handle[i] = 0xFFFFu;
              TMAPAPP_Context.num_cis_src--;
              break;
            }
          }
        }
        if (TMAPAPP_Context.num_cis_src == 0u)
        {
          LOG_INFO_APP("No more Input Audio Data Path is up\n");
          TMAPAPP_Context.audio_role_setup &= ~AUDIO_ROLE_SOURCE;
          LOG_INFO_APP("Stop Rx Audio Peripheral Driver\n");
          Stop_RxAudio();
        }
      }
      else
      {
        Audio_Chnl_Allocation_t channel_alloc = LTV_GetConfiguredAudioChannelAllocation(info->CodecConf.pSpecificConf,
                                                                                        info->CodecConf.SpecificConfLength);
        TMAPAPP_Context.NumOutputChannels -= APP_GetBitsAudioChnlAllocations(channel_alloc);
        LOG_INFO_APP("Number of Output Channels Up:  %d\n", TMAPAPP_Context.NumOutputChannels);
        LOG_INFO_APP("Output Audio Data Path is down with status 0x%02X for CIS Conn handle 0x%04X\n",
                    pNotification->Status,
                    info->CIS_ConnHandle);

        if (info->CIS_ConnHandle != 0xFFFF)
        {
          for (i = 0; i< APP_MAX_NUM_CIS; i++)
          {
            if (TMAPAPP_Context.cis_snk_handle[i] == info->CIS_ConnHandle)
            {
              TMAPAPP_Context.cis_snk_handle[i] = 0xFFFFu;
              TMAPAPP_Context.num_cis_snk--;
              break;
            }
          }
        }
        if (TMAPAPP_Context.num_cis_snk == 0u)
        {
          LOG_INFO_APP("No more Output Audio Data Path is up\n");
          TMAPAPP_Context.audio_role_setup &= ~AUDIO_ROLE_SINK;
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
        p_ase = TMAPAPP_GetASE(p_info->ASE_ID,pNotification->ConnHandle);
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

    case CAP_CCP_META_EVT:
    {
      CCP_Notification_Evt_t *p_ccp_evt = (CCP_Notification_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Call Control Meta Event 0x%02X is received on ConnHandle 0x%04X:\n",
                  p_ccp_evt->EvtOpcode,
                  p_ccp_evt->ConnHandle);
#if (APP_CCP_ROLE_SERVER_SUPPORT == 1u)
      CCP_MetaEvt_Notification(p_ccp_evt);
#endif /* (APP_CCP_ROLE_SERVER_SUPPORT == 1u) */
      break;
    }
    case CAP_MCP_META_EVT:
    {
      MCP_Notification_Evt_t *p_mcp_evt = (MCP_Notification_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Media Control Meta Event 0x%02X is received on ConnHandle 0x%04X:\n",
                  p_mcp_evt->EvtOpcode,
                  p_mcp_evt->ConnHandle);
#if (APP_MCP_ROLE_SERVER_SUPPORT == 1u)
      MCP_MetaEvt_Notification(p_mcp_evt);
#endif /* (APP_MCP_ROLE_SERVER_SUPPORT == 1u) */
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
    case CAP_VOLUME_CTRL_LINKUP_EVT:
    {
      APP_ACL_Conn_t *p_conn;
      LOG_INFO_APP("Volume Control Profile is linked on ConnHandle 0x%04X\n",pNotification->ConnHandle);
      p_conn = APP_GetACLConn(pNotification->ConnHandle);
      if (p_conn != 0)
      {
        p_conn->AudioProfile |= AUDIO_PROFILE_VCP;
      }
      break;
    }
    case CAP_SET_VOLUME_PROCEDURE_COMPLETE_EVT:
    {
      APP_ACL_Conn_t *p_conn = APP_GetACLConn(pNotification->ConnHandle);
      if (p_conn != 0)
      {
        if (p_conn->ConfirmIndicationRequired == 1u)
        {
          tBleStatus ret;
          /* Confirm indication now that the GATT is available */
          ret = aci_gatt_confirm_indication(pNotification->ConnHandle);
          LOG_INFO_APP("aci_gatt_confirm_indication() on ConnHandle 0x%04X returns status 0x%02X\n",
                       pNotification->ConnHandle,
                       ret);
          p_conn->ConfirmIndicationRequired = 0u;
        }
      }
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
    if ( TMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle == ConnHandle)
    {
      return &TMAPAPP_Context.ACL_Conn[i];
    }
  }
  return 0;
}

static APP_ACL_Conn_t *APP_AllocateACLConn(uint16_t ConnHandle)
{
  /* Allocate new slot */
  for (uint8_t i = 0; i < CFG_BLE_NUM_LINK; i++)
  {
    if ( TMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle == 0xFFFFu)
    {
      TMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle = ConnHandle;
      return &TMAPAPP_Context.ACL_Conn[i];
    }
  }
  return 0;
}

static APP_ASE_Info_t * TMAPAPP_GetASE(uint8_t ASE_ID,uint16_t ACL_ConnHandle)
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

static uint8_t TMAPAPP_SetupAudioDataPath(uint16_t ACL_ConnHandle,
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

  BAP_ASE_Info_t * p_ase;

  p_ase = CAP_Unicast_GetASEInfo(ACL_ConnHandle,ASE_ID);
  if(p_ase != 0)
  {
    frequency = LTV_GetConfiguredSamplingFrequency(p_ase->params.Codec.CodecConf.pSpecificConf,
                                                   p_ase->params.Codec.CodecConf.SpecificConfLength);
    frame_duration = LTV_GetConfiguredFrameDuration(p_ase->params.Codec.CodecConf.pSpecificConf,
                                                    p_ase->params.Codec.CodecConf.SpecificConfLength);

   if (((p_ase->Type == ASE_SOURCE) && (TMAPAPP_Context.bap_role & BAP_ROLE_UNICAST_SERVER)) || \
      ((p_ase->Type == ASE_SINK) && (TMAPAPP_Context.bap_role & BAP_ROLE_UNICAST_CLIENT)))
    {
      /*Audio Data packets are sent to the remote device*/
      role = AUDIO_ROLE_SOURCE;
      LOG_INFO_APP("Setup Audio Peripheral for Source Role at Sampling frequency %d\n",frequency);
    }
    else if(((p_ase->Type == ASE_SINK) && (TMAPAPP_Context.bap_role & BAP_ROLE_UNICAST_SERVER)) || \
          ((p_ase->Type == ASE_SOURCE) && (TMAPAPP_Context.bap_role & BAP_ROLE_UNICAST_CLIENT)))
    {
      /* Audio Data packets are received from the remote device.*/
      role = AUDIO_ROLE_SINK;
      LOG_INFO_APP("Setup Audio Peripheral for Sink Role at Sampling frequency %d\n",frequency);
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
    if (TMAPAPP_Context.audio_role_setup == 0x00)
    {
      LOG_INFO_APP("Configure Audio Periphal drivers at Sampling frequency %d\n",frequency);
      MX_AudioInit(role,frequency,
                   frame_duration,
                   (uint8_t *)aSnkBuff,
                   (uint8_t *)aSrcBuff,
                   (AudioDriverConfig) TMAPAPP_Context.audio_driver_config);
    }
    if (role == AUDIO_ROLE_SOURCE)
    {
      for (i = 0; i< APP_MAX_NUM_CIS; i++)
      {
        if (TMAPAPP_Context.cis_src_handle[i] == 0xFFFFu)
        {
          TMAPAPP_Context.cis_src_handle[i] = CIS_ConnHandle;
          TMAPAPP_Context.num_cis_src++;
          break;
        }
      }
      if ((TMAPAPP_Context.audio_role_setup & role) == 0x00)
      {
        LOG_INFO_APP("Register callback to Start Audio Peripheral Rx\n");
        CODEC_RegisterTriggerClbk(1,0,&start_audio_source);
      }
    }
    else if (role == AUDIO_ROLE_SINK)
    {

      for (i = 0; i< APP_MAX_NUM_CIS; i++)
      {
        if (TMAPAPP_Context.cis_snk_handle[i] == 0xFFFFu)
        {
          TMAPAPP_Context.cis_snk_handle[i] = CIS_ConnHandle;
          TMAPAPP_Context.num_cis_snk++;
          break;
        }
      }
      if ((TMAPAPP_Context.audio_role_setup & role) == 0x00)
      {
        LOG_INFO_APP("Register callback to Start Audio Peripheral Tx\n");
        CODEC_RegisterTriggerClbk(1,1,&start_audio_sink);
      }
    }
    CODEC_DataPathParam_t param;
    /* input data path */
    param.SampleDepth = 16;

    /* SAI/I2C peripheral driver requests to set decimation to 2*/

    if ((role == AUDIO_ROLE_SOURCE) && (TMAPAPP_Context.audio_driver_config == AUDIO_DRIVER_CONFIG_HEADSET))
    {
      param.Decimation = 1;
    }
    else
    {
      param.Decimation = 2;
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
          if (TMAPAPP_Context.cis_src_handle[i] == CIS_ConnHandle)
          {
            TMAPAPP_Context.cis_src_handle[i] = 0xFFFFu;
            TMAPAPP_Context.num_cis_src--;
            break;
          }
        }
      }
      else if (role == AUDIO_ROLE_SINK)
      {
        for (i = 0; i< APP_MAX_NUM_CIS; i++)
        {
          if (TMAPAPP_Context.cis_snk_handle[i] == CIS_ConnHandle)
          {
            TMAPAPP_Context.cis_snk_handle[i] = 0xFFFFu;
            TMAPAPP_Context.num_cis_snk--;
            break;
          }
        }
      }
    }
    else
    {
      APP_ASE_Info_t *p_app_ase;
      p_app_ase = TMAPAPP_GetASE(ASE_ID,ACL_ConnHandle);
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
static void start_audio_source(void)
{
  LOG_INFO_APP("START AUDIO SOURCE (input)\n");
  Start_RxAudio();
}


/*Audio Sink */
static void start_audio_sink(void)
{
  /* reset numbers of active channels */
  Nb_Active_Ch = 0;

  LOG_INFO_APP("START AUDIO SINK (output)\n");
  Start_TxAudio();
}

#if (APP_CCP_ROLE_SERVER_SUPPORT == 1u)
static tBleStatus TMAPAPP_RegisterGenericTelephonyBearer(uint8_t *pCCID)
{
  tBleStatus ret = HCI_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE_ERR_CODE;
  CCP_BearerInit_t BearerInit;
  BearerInit.pProviderName = (uint8_t *)"Orange";
  BearerInit.pUCI = (uint8_t *)"skype";
  BearerInit.Technology = CCP_BEARER_TECHNOLOGY_4G;
  BearerInit.pURISchemesList = (uint8_t *)"skype,tel";
  BearerInit.EnSignalStrength = 1u;
  BearerInit.SignalStrength = 50;
  BearerInit.InbandRingtone = CCP_INBAND_RINGTONE_ENABLED;
  BearerInit.SilentMode = CCP_SILENT_MODE_ON;
  BearerInit.LocalHoldOption = CCP_LOCAL_HOLD_FEATURE_SUPPORTED;
  BearerInit.JoinOption = CCP_JOIN_FEATURE_SUPPORTED;

  ret = CAP_RegisterGenericTelephonyBearer(TMAPAPP_Context.GenericBearer.CCID,&BearerInit);
  if (ret == BLE_STATUS_SUCCESS)
  {
    TMAPAPP_Context.GenericBearer.CCID = TMAPAPP_Context.GenericBearer.CCID;
    LOG_INFO_APP("Generic Telephony Bearer CCID %d successfully registered\n",TMAPAPP_Context.GenericBearer.CCID);
    *pCCID = TMAPAPP_Context.GenericBearer.CCID;
  }
  else {
    LOG_INFO_APP("Failed to register Generic Telephony Bearer\n");
  }
  return ret;
}
#if (APP_CCP_NUM_LOCAL_BEARER_INSTANCES > 0u)
tBleStatus TMAPAPP_RegisterTelephonyBearerInstance(uint8_t *pCCID)
{
  tBleStatus ret = HCI_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE_ERR_CODE;
  uint8_t i = 0u;
  uint8_t index = 0u;
  CCP_BearerInit_t BearerInit;


  for(i = 0u ; i < APP_CCP_NUM_LOCAL_BEARER_INSTANCES ; i++)
  {
    if(TMAPAPP_Context.BearerInstance[i].CCID == 0u)
    {
      index = i;
      break;
    }
  }

  if (i == APP_CCP_NUM_LOCAL_BEARER_INSTANCES)
  {
    LOG_INFO_APP("No Available Telephony Bearer Instance resource\n");
    return BLE_STATUS_INSUFFICIENT_RESOURCES;
  }

  if (index == 0u)
  {
    BearerInit.pProviderName = (uint8_t *)"SFR";
    BearerInit.pUCI = (uint8_t *)"lync";
    BearerInit.Technology = CCP_BEARER_TECHNOLOGY_4G;
    BearerInit.pURISchemesList = (uint8_t *)"tel,skype";
    BearerInit.EnSignalStrength = 1u;
    BearerInit.SignalStrength = 50;
    BearerInit.InbandRingtone = CCP_INBAND_RINGTONE_ENABLED;
    BearerInit.SilentMode = CCP_SILENT_MODE_ON;
    BearerInit.LocalHoldOption = CCP_LOCAL_HOLD_FEATURE_SUPPORTED;
    BearerInit.JoinOption = CCP_JOIN_FEATURE_SUPPORTED;
  }
  else if (index == 1u)
  {
    BearerInit.pProviderName = (uint8_t *)"Free";
    BearerInit.pUCI = (uint8_t *)"ftime";
    BearerInit.Technology = CCP_BEARER_TECHNOLOGY_4G;
    BearerInit.pURISchemesList = (uint8_t *)"tel,sip,skype";
    BearerInit.EnSignalStrength = 1u;
    BearerInit.SignalStrength = 50;
    BearerInit.InbandRingtone = CCP_INBAND_RINGTONE_ENABLED;
    BearerInit.SilentMode = CCP_SILENT_MODE_ON;
    BearerInit.LocalHoldOption = CCP_LOCAL_HOLD_FEATURE_SUPPORTED;
    BearerInit.JoinOption = CCP_JOIN_FEATURE_SUPPORTED;
  }
  else
  {
    LOG_INFO_APP("No Sufficient allocated Telephony Bearer Instance resources\n");
    return BLE_STATUS_INSUFFICIENT_RESOURCES;
  }
  ret = CAP_RegisterTelephonyBearerInstance((TMAPAPP_Context.GenericBearer.CCID+index+1u),&BearerInit);
  if (ret == BLE_STATUS_SUCCESS)
  {
    TMAPAPP_Context.BearerInstance[index].CCID = (TMAPAPP_Context.GenericBearer.CCID+index+1u);
    *pCCID = TMAPAPP_Context.BearerInstance[index].CCID;
    LOG_INFO_APP("Telephony Bearer Instance CCID %d successfully registered\n",
                 TMAPAPP_Context.BearerInstance[index].CCID);
  }
  else {
    LOG_INFO_APP("Failed to register Telephony Bearer Instance\n");
  }
  return ret;
}
#endif /*(APP_CCP_NUM_LOCAL_BEARER_INSTANCES > 0u)*/

/**
  * @brief  Notify CCP Meta Events
  * @param  pNotification: pointer on notification information
 */
static void CCP_MetaEvt_Notification(CCP_Notification_Evt_t *pNotification)
{
  tBleStatus status;
  switch(pNotification->EvtOpcode)
  {
    case CCP_SRV_ORIGINATE_CALL_REQ_EVT:
    {
      CCP_SRV_Originate_Call_Req_Params_Evt_t *info = (CCP_SRV_Originate_Call_Req_Params_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Originate Call Request of Telephone Bearer Instance ID %d\n",pNotification->ContentControlID);
      *info->WithHeldByServer = 0u;
      *info->WithHeldByNetwork = 0u;
      break;
    }
    case CCP_SRV_CALL_STATE_EVT:
    {
      CCP_SRV_Call_State_Params_Evt_t *bearer_info = (CCP_SRV_Call_State_Params_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Current Call of Telephone Bearer Instance ID %d : \n",pNotification->ContentControlID);
      LOG_INFO_APP("     Call Index : %d\n",bearer_info->CallIndex);
      switch(bearer_info->CallState)
      {
        case CCP_CS_IDLE:
          LOG_INFO_APP("     Call State : IDLE\n");
          Menu_SetCallState("IDLE");
        break;

        case CCP_CS_INCOMING:
          LOG_INFO_APP("     Call State : INCOMING\n");
          Menu_SetCallState("INCOMING");
          status = CCP_SERVER_SetCallFriendlyName(pNotification->ContentControlID,
                                                  bearer_info->CallIndex,
                                                  (uint8_t *)"Long Call Friendly Name",
                                                  23);
          LOG_INFO_APP("Set Friendly Name of Call %d returns status 0x%02X\n",bearer_info->CallIndex,status);
        break;

        case CCP_CS_DIALING:
          LOG_INFO_APP("     Call State : DIALING\n");
          Menu_SetCallState("DIALING");
        break;

        case CCP_CS_ALERTING:
          LOG_INFO_APP("     Call State : ALERTING\n");
          Menu_SetCallState("ALERTING");
        break;

        case CCP_CS_ACTIVE:
          LOG_INFO_APP("     Call State : ACTIVE\n");
          Menu_SetCallState("ACTIVE");
        break;

        case CCP_CS_LOCALLY_HELD:
          LOG_INFO_APP("     Call State : LOCALLY HELD\n");
          Menu_SetCallState("HELD");
        break;

        case CCP_CS_REMOTELY_HELD:
          LOG_INFO_APP("     Call State : REMOTELY HELD\n");
          Menu_SetCallState("HELD");
        break;

        case CCP_CS_LOCALLY_REMOTELY_HELD:
          LOG_INFO_APP("     Call State : LOCALLY AND REMOTELY HELD\n");
          Menu_SetCallState("HELD");
        break;
      }
      LOG_INFO_APP("     Call Flags : 0x%02X\n",bearer_info->CallFlags);
      if (bearer_info->CallURILength > 0u)
      {
        uint8_t i;
        LOG_INFO_APP("     Call URI : ");
        for (i = 0; i < bearer_info->CallURILength; i++)
        {
          LOG_INFO_APP("%c",bearer_info->pCallURI[i]);
        }
        LOG_INFO_APP("\n");
      }
      break;
    }

    default:
      break;
  }
}
#endif /* (APP_CCP_ROLE_SERVER_SUPPORT == 1u) */

#if (APP_MCP_ROLE_SERVER_SUPPORT == 1u)
static tBleStatus TMAPAPP_RegisterGenericMediaPlayer(uint8_t *pCCID)
{
  tBleStatus ret = HCI_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE_ERR_CODE;

  ret = CAP_RegisterGenericMediaPlayer(TMAPAPP_Context.GenericMediaPlayer.CCID);
  if (ret == BLE_STATUS_SUCCESS)
  {
    TMAPAPP_Context.GenericMediaPlayer.CCID = TMAPAPP_Context.GenericMediaPlayer.CCID;
    LOG_INFO_APP("Generic Media Player CCID %d successfully registered\n",TMAPAPP_Context.GenericMediaPlayer.CCID);
    ret = MCP_SERVER_SetMediaPlayerName(TMAPAPP_Context.GenericMediaPlayer.CCID,(uint8_t *)"Radio App",9u);
    LOG_INFO_APP("[MP %d] Set Media Player Name status 0x%02X\n",TMAPAPP_Context.GenericMediaPlayer.CCID,ret);

    ret =  MCP_SERVER_SetSupportedPlayingOrders(TMAPAPP_Context.GenericMediaPlayer.CCID, 0x3FFu);
    LOG_INFO_APP("[MP %d] Set Supported Playing Orders status 0x%02X\n",TMAPAPP_Context.GenericMediaPlayer.CCID,ret);

    ret =  MCP_SERVER_SetPlayingOrder(TMAPAPP_Context.GenericMediaPlayer.CCID,MCP_PLAYING_ORDER_IN_ORDER_ONCE);
    LOG_INFO_APP("[MP %d] Set Playing Order status 0x%02X\n",TMAPAPP_Context.GenericMediaPlayer.CCID,ret);

    ret =  MCP_SERVER_SetSupportedMediaCtrlFeatures(TMAPAPP_Context.GenericMediaPlayer.CCID,0x001FFFFFu);
    LOG_INFO_APP("[MP %d] Set Supported Media Ctrl features status 0x%02X\n",TMAPAPP_Context.GenericMediaPlayer.CCID,ret);

    /* Create timer for Track Position */
    TMAPAPP_Context.GenericMediaPlayer.TrackPositionInterval = 10;
    ret = UTIL_TIMER_Create(&(TMAPAPP_Context.GenericMediaPlayer.TimerTrackPosition_Id),
                    TMAPAPP_Context.GenericMediaPlayer.TrackPositionInterval,
                    UTIL_TIMER_PERIODIC,
                    &TMAPAPP_TrackPositionCB, &TMAPAPP_Context.GenericMediaPlayer);
    LOG_INFO_APP("[MP %d] Timer for Track Position initialization status 0x%02X\n",
                TMAPAPP_Context.GenericMediaPlayer.CCID,
                ret);

    TMAPAPP_SetMediaState(TMAPAPP_Context.GenericMediaPlayer.CCID,TMAPAPP_Context.GenericMediaPlayer.MediaState);
    ret = MCP_SERVER_SetTrackTitle(TMAPAPP_Context.GenericMediaPlayer.CCID,
                                    MCPAPP_Track[TMAPAPP_Context.GenericMediaPlayer.GroupID-1u][TMAPAPP_Context.GenericMediaPlayer.TrackID-1u].pTitle,
                                    MCPAPP_Track[TMAPAPP_Context.GenericMediaPlayer.GroupID-1u][TMAPAPP_Context.GenericMediaPlayer.TrackID-1u].TitleLen);
    LOG_INFO_APP("[MP %d] Set Track Title of Track %d in Group %d : status 0x%02X\n",
                        TMAPAPP_Context.GenericMediaPlayer.CCID,
                        TMAPAPP_Context.GenericMediaPlayer.TrackID,
                        TMAPAPP_Context.GenericMediaPlayer.GroupID,
                        ret);
    if (ret == BLE_STATUS_SUCCESS)
    {
      Menu_SetTrackTitle(MCPAPP_Track[TMAPAPP_Context.GenericMediaPlayer.GroupID-1u][TMAPAPP_Context.GenericMediaPlayer.TrackID-1u].pTitle,
                         MCPAPP_Track[TMAPAPP_Context.GenericMediaPlayer.GroupID-1u][TMAPAPP_Context.GenericMediaPlayer.TrackID-1u].TitleLen);
    }
    TMAPAPP_Context.GenericMediaPlayer.TrackPosition = 0xFFFFFFFF;
    ret =  MCP_SERVER_SetTrackPosition(TMAPAPP_Context.GenericMediaPlayer.CCID,TMAPAPP_Context.GenericMediaPlayer.TrackPosition);
    LOG_INFO_APP("[MP %d] Set Track Position (%d) status 0x%02X\n",
                TMAPAPP_Context.GenericMediaPlayer.CCID,
                (int32_t)TMAPAPP_Context.GenericMediaPlayer.TrackPosition,
                ret);
    *pCCID = TMAPAPP_Context.GenericMediaPlayer.CCID;
  }
  else {
    LOG_INFO_APP("Failed to register Generic Media Player\n");
  }

  return ret;
}

#if (APP_MCP_NUM_LOCAL_MEDIA_PLAYER_INSTANCES > 0u)
static tBleStatus TMAPAPP_RegisterMediaPlayerInstance(uint8_t *pCCID)
{

  tBleStatus ret = HCI_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE_ERR_CODE;

  uint8_t i = 0u;
  uint8_t index = 0u;

  for(i = 0u ; i < APP_MCP_NUM_LOCAL_MEDIA_PLAYER_INSTANCES ; i++)
  {
    if(TMAPAPP_Context.MediaPlayerInstance[i].CCID == 0x00u)
    {
      index = i;
      break;
    }
  }

  if (i == APP_MCP_NUM_LOCAL_MEDIA_PLAYER_INSTANCES)
  {
    LOG_INFO_APP("No Available Media Player Instance resource\n");
    return BLE_STATUS_INSUFFICIENT_RESOURCES;
  }

  ret = CAP_RegisterMediaPlayerInstance((TMAPAPP_Context.GenericMediaPlayer.CCID+index+1));
  if (ret == BLE_STATUS_SUCCESS)
  {
    TMAPAPP_Context.MediaPlayerInstance[index].CCID = (TMAPAPP_Context.GenericMediaPlayer.CCID+index+1);
    LOG_INFO_APP("[MP %d] Media Player Instance CCID %d successfully registered\n",
                TMAPAPP_Context.MediaPlayerInstance[index].CCID,
                TMAPAPP_Context.MediaPlayerInstance[index].CCID);
    if (index == 0u)
    {
      ret = MCP_SERVER_SetMediaPlayerName(TMAPAPP_Context.MediaPlayerInstance[index].CCID,(uint8_t *)"Podcast App",11u);
    }
    else if (index == 1u)
    {
      ret = MCP_SERVER_SetMediaPlayerName(TMAPAPP_Context.MediaPlayerInstance[index].CCID,(uint8_t *)"Television",10u);
    }
    LOG_INFO_APP("[MP %d] Set Media Player Name status 0x%02X\n",TMAPAPP_Context.MediaPlayerInstance[index].CCID,ret);

    ret =  MCP_SERVER_SetSupportedPlayingOrders(TMAPAPP_Context.MediaPlayerInstance[index].CCID, 0x3FFu);
    LOG_INFO_APP("[MP %d] Set Supported Playing Orders status 0x%02X\n",
                TMAPAPP_Context.MediaPlayerInstance[index].CCID,
                ret);

    ret =  MCP_SERVER_SetPlayingOrder(TMAPAPP_Context.MediaPlayerInstance[index].CCID,MCP_PLAYING_ORDER_IN_ORDER_ONCE);
    LOG_INFO_APP("[MP %d] Set Playing Order status 0x%02X\n",TMAPAPP_Context.MediaPlayerInstance[index].CCID,ret);

    ret =  MCP_SERVER_SetSupportedMediaCtrlFeatures(TMAPAPP_Context.MediaPlayerInstance[index].CCID,0x001FFFFFu);
    LOG_INFO_APP("[MP %d] Set Supported Media Ctrl features status 0x%02X\n",
                TMAPAPP_Context.MediaPlayerInstance[index].CCID,
                ret);

    /* Create timer for Track Position */
    TMAPAPP_Context.MediaPlayerInstance[index].TrackPositionInterval = 10;
    ret = UTIL_TIMER_Create(&(TMAPAPP_Context.MediaPlayerInstance[index].TimerTrackPosition_Id),
                    TMAPAPP_Context.MediaPlayerInstance[index].TrackPositionInterval,
                    UTIL_TIMER_PERIODIC,
                    &TMAPAPP_TrackPositionCB, &TMAPAPP_Context.MediaPlayerInstance[index]);

    TMAPAPP_SetMediaState(TMAPAPP_Context.MediaPlayerInstance[index].CCID,TMAPAPP_Context.MediaPlayerInstance[index].MediaState);
    ret = MCP_SERVER_SetTrackTitle(TMAPAPP_Context.MediaPlayerInstance[index].CCID,
                                    MCPAPP_Track[TMAPAPP_Context.MediaPlayerInstance[index].GroupID-1u][TMAPAPP_Context.MediaPlayerInstance[index].TrackID-1u].pTitle,
                                    MCPAPP_Track[TMAPAPP_Context.MediaPlayerInstance[index].GroupID-1u][TMAPAPP_Context.MediaPlayerInstance[index].TrackID-1u].TitleLen);
    LOG_INFO_APP("[MP %d] Set Track Title of Track %d in Group %d : status 0x%02X\n",
                        TMAPAPP_Context.MediaPlayerInstance[index].CCID,
                        TMAPAPP_Context.MediaPlayerInstance[index].TrackID,
                        TMAPAPP_Context.MediaPlayerInstance[index].GroupID,
                        ret);
    if (status == BLE_STATUS_SUCCESS)
    {
      Menu_SetTrackTitle(MCPAPP_Track[TMAPAPP_Context.MediaPlayerInstance[index].GroupID-1u][TMAPAPP_Context.MediaPlayerInstance[index].TrackID-1u].pTitle,
                         MCPAPP_Track[TMAPAPP_Context.MediaPlayerInstance[index].GroupID-1u][TMAPAPP_Context.MediaPlayerInstance[index].TrackID-1u].TitleLen);
    }
    TMAPAPP_Context.MediaPlayerInstance[index].TrackPosition = 0xFFFFFFFF;
    ret =  MCP_SERVER_SetTrackPosition(TMAPAPP_Context.MediaPlayerInstance[index].CCID,TMAPAPP_Context.MediaPlayerInstance[index].TrackPosition);
    LOG_INFO_APP("[MP %d] Set Track Position (%d) status 0x%02X\n",
                TMAPAPP_Context.MediaPlayerInstance[index].CCID,
                (int32_t)TMAPAPP_Context.MediaPlayerInstance[index].TrackPosition,
                ret);
    *pCCID = TMAPAPP_Context.MediaPlayerInstance[index].CCID;
  }
  else {
    LOG_INFO_APP("Failed to register Media Player Instance\n");
  }
  return ret;
}
#endif /*(APP_MCP_NUM_LOCAL_MEDIA_PLAYER_INSTANCES > 0u)*/

static tBleStatus TMAPAPP_SetMediaState(uint8_t MediaPlayerCCID,MCP_MediaState_t MediaState)
{
  tBleStatus status = HCI_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE_ERR_CODE;

  UTIL_TIMER_Status_t timer_status;
  TMAPAPP_MediaPlayer_t *p_mediaplayer = TMAPAPP_GetMediaPlayer(MediaPlayerCCID);
  if (p_mediaplayer != 0)
  {
    if ((MediaState == MCP_MEDIA_STATE_INACTIVE) || (MediaState == MCP_MEDIA_STATE_PAUSED))
    {
      UTIL_TIMER_Stop(&(p_mediaplayer->TimerTrackPosition_Id));
    }
    else if (MediaState == MCP_MEDIA_STATE_PLAYING)
    {
      if ((p_mediaplayer->MediaState == MCP_MEDIA_STATE_INACTIVE) \
          || (p_mediaplayer->MediaState == MCP_MEDIA_STATE_PAUSED) \
          || (p_mediaplayer->MediaState == MCP_MEDIA_STATE_SEEKING))
      {
        UTIL_TIMER_Stop(&(p_mediaplayer->TimerTrackPosition_Id));
        p_mediaplayer->TrackPositionInterval = 10;
        UTIL_TIMER_StartWithPeriod( &(p_mediaplayer->TimerTrackPosition_Id), p_mediaplayer->TrackPositionInterval);
      }
    }
    else if (MediaState == MCP_MEDIA_STATE_SEEKING)
    {
      timer_status =UTIL_TIMER_Stop(&(p_mediaplayer->TimerTrackPosition_Id));
      LOG_INFO_APP("[MP %d] Stop Timer returns status 0x%02X\n",MediaPlayerCCID,p_mediaplayer->MediaState,timer_status);
      /*Start Timer with a Timer frequency equal to a multiple of the of the real-time playback speed*/
      timer_status = UTIL_TIMER_StartWithPeriod( &(p_mediaplayer->TimerTrackPosition_Id), (p_mediaplayer->TrackPositionInterval/2u));
      LOG_INFO_APP("[MP %d] Start Timer with int %dms returns status 0x%02X\n",
                  MediaPlayerCCID,
                  (p_mediaplayer->TrackPositionInterval/2u),
                  timer_status);
    }
    p_mediaplayer->MediaState = MediaState;
    status = MCP_SERVER_SetMediaState(MediaPlayerCCID,p_mediaplayer->MediaState);
    LOG_INFO_APP("[MP %d] Set Media State 0x%02X : status 0x%02X\n",MediaPlayerCCID,p_mediaplayer->MediaState,status);

    if (status == BLE_STATUS_SUCCESS)
    {
      switch(MediaState)
      {
        case MCP_MEDIA_STATE_INACTIVE:
        {
          Menu_SetMediaState("INACTIVE");
        }
        break;
        case MCP_MEDIA_STATE_PAUSED:
        {
          Menu_SetMediaState("PAUSED");
        }
        break;
        case MCP_MEDIA_STATE_PLAYING:
        {
          Menu_SetMediaState("PLAYING");
        }
        break;
        case MCP_MEDIA_STATE_SEEKING:
        {
          Menu_SetMediaState("SEEKING");
        }
        break;
      }
    }
  }
  return status;
}

static TMAPAPP_MediaPlayer_t *TMAPAPP_GetMediaPlayer(uint8_t CCID)
{
  TMAPAPP_MediaPlayer_t *p_mp = 0;
  if (TMAPAPP_Context.GenericMediaPlayer.CCID == CCID)
  {
    p_mp = &TMAPAPP_Context.GenericMediaPlayer;
  }
#if (APP_MCP_NUM_LOCAL_MEDIA_PLAYER_INSTANCES > 0u)
  else
  {
    uint8_t i;
    for(i = 0u ; i < APP_MCP_NUM_LOCAL_MEDIA_PLAYER_INSTANCES ; i++)
    {
      if(TMAPAPP_Context.MediaPlayerInstance[i].CCID == CCID)
      {
        p_mp = &TMAPAPP_Context.MediaPlayerInstance[i];
        break;
      }
    }
  }
#endif /*(APP_MCP_NUM_LOCAL_MEDIA_PLAYER_INSTANCES > 0u)*/
  return p_mp;
}

static void TMAPAPP_TrackPositionCB(void *arg)
{
  tBleStatus status;
  TMAPAPP_MediaPlayer_t *p_mediaplayer = (TMAPAPP_MediaPlayer_t *)arg;

  if (p_mediaplayer->MediaState == MCP_MEDIA_STATE_SEEKING)
  {
    if ( p_mediaplayer->SeekingDir == SEEKING_DIR_FAST_REWIND)
    {
      if ( p_mediaplayer->TrackPosition > (p_mediaplayer->TrackPositionInterval/10u))
      {
        p_mediaplayer->TrackPosition -= (p_mediaplayer->TrackPositionInterval/10u);
        if (p_mediaplayer->TrackPosition == 0u)
        {
          UTIL_TIMER_Stop(&(p_mediaplayer->TimerTrackPosition_Id));
          p_mediaplayer->TrackPosition = 0;
          TMAPAPP_SetMediaState(p_mediaplayer->CCID,MCP_MEDIA_STATE_PAUSED);
        }
      }
      else
      {
        UTIL_TIMER_Stop(&(p_mediaplayer->TimerTrackPosition_Id));
        p_mediaplayer->TrackPosition = 0;
        TMAPAPP_SetMediaState(p_mediaplayer->CCID,MCP_MEDIA_STATE_PAUSED);
      }
    }
    else if ( p_mediaplayer->SeekingDir == SEEKING_DIR_FAST_FORWARD)
    {
      p_mediaplayer->TrackPosition += (p_mediaplayer->TrackPositionInterval/10u);
      if (p_mediaplayer->TrackPosition >= MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].Duration)
      {
        if( p_mediaplayer->TrackID == APP_MCP_NUM_TRACKS)
        {
          p_mediaplayer->TrackID = 1u;
        }
        else
        {
          p_mediaplayer->TrackID += 1u;
        }
        UTIL_TIMER_Stop(&(p_mediaplayer->TimerTrackPosition_Id));
        LOG_INFO_APP("[MP %d] Change to Track %d\n",p_mediaplayer->CCID,p_mediaplayer->TrackID);
        status = MCP_SERVER_SetTrackTitle(p_mediaplayer->CCID,
                                          MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].pTitle,
                                          6u);
        LOG_INFO_APP("[MP %d] Set Track Title (%d) of Track %d in Group %d : status 0x%02X\n",
                        p_mediaplayer->CCID,
                        p_mediaplayer->TrackID,
                        p_mediaplayer->GroupID,
                        status);
        if (status == BLE_STATUS_SUCCESS)
        {
          Menu_SetTrackTitle(MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].pTitle,
                             MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].TitleLen);
        }
        status = MCP_SERVER_SetTrackDuration(p_mediaplayer->CCID,MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].Duration);
        LOG_INFO_APP("[MP %d] Set Track Duration (%d) of Track %d in Group %d : status 0x%02X\n",
                    p_mediaplayer->CCID,
                    MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].Duration,
                    p_mediaplayer->TrackID,
                    p_mediaplayer->GroupID,
                    status);
        p_mediaplayer->TrackPosition = 0;
        TMAPAPP_SetMediaState(p_mediaplayer->CCID,MCP_MEDIA_STATE_PAUSED);
        status = MCP_SERVER_NotifyTrackChanged(p_mediaplayer->CCID);
        LOG_INFO_APP("[MP %d] Track Change Notification status 0x%02X\n",p_mediaplayer->CCID);
      }
    }
  }
  else if (p_mediaplayer->MediaState == MCP_MEDIA_STATE_PLAYING)
  {
    p_mediaplayer->TrackPosition += (p_mediaplayer->TrackPositionInterval/10u);
    if (p_mediaplayer->TrackPosition >= MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].Duration)
    {
      if( p_mediaplayer->TrackID == APP_MCP_NUM_TRACKS)
      {
        p_mediaplayer->TrackID = 1u;
      }
      else
      {
        p_mediaplayer->TrackID += 1u;
      }
      LOG_INFO_APP("[MP %d] Change to Track %d\n",p_mediaplayer->CCID,p_mediaplayer->TrackID);
      status = MCP_SERVER_SetTrackTitle(p_mediaplayer->CCID,
                                          MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].pTitle,
                                          6u);
      LOG_INFO_APP("[MP %d] Set Track Title (%d) of Track %d in Group %d : status 0x%02X\n",
                        p_mediaplayer->CCID,
                        p_mediaplayer->TrackID,
                        p_mediaplayer->GroupID,
                        status);
      if (status == BLE_STATUS_SUCCESS)
      {
        Menu_SetTrackTitle(MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].pTitle,
                           MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].TitleLen);
      }
      status = MCP_SERVER_SetTrackDuration(p_mediaplayer->CCID,MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].Duration);
      LOG_INFO_APP("[MP %d] Set Track Duration (%d) of Track %d in Group %d : status 0x%02X\n",
                    p_mediaplayer->CCID,
                    MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].Duration,
                    p_mediaplayer->TrackID,
                    p_mediaplayer->GroupID,
                    status);
      p_mediaplayer->TrackPosition = 0;
      status = MCP_SERVER_NotifyTrackChanged(p_mediaplayer->CCID);
      LOG_INFO_APP("[MP %d] Track Change Notification status 0x%02X\n",p_mediaplayer->CCID);
    }
  }
}

/**
  * @brief  Notify MCP Meta Events
  * @param  pNotification: pointer on notification information
 */
static void MCP_MetaEvt_Notification(MCP_Notification_Evt_t *pNotification)
{
  tBleStatus status;
  TMAPAPP_MediaPlayer_t *p_mediaplayer = TMAPAPP_GetMediaPlayer(pNotification->ContentControlID);
  switch(pNotification->EvtOpcode)
  {
    case MCP_SRV_TRACK_POSITION_REQ_EVT:
    {
      MCP_SRV_TrackPositionReq_Evt_t *p_info = (MCP_SRV_TrackPositionReq_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("[MP %d] Track Position has been requested by remote MCP Client\n",
                  pNotification->ContentControlID);
      LOG_INFO_APP("     Position : %d\n",p_mediaplayer->TrackPosition);
      *(p_info->TrackPosition) = p_mediaplayer->TrackPosition;
    }
    break;

  case MCP_SRV_CTRL_REQ_EVT:
    {
      MCP_SRV_MediaCtrlReq_Evt_t *p_info = (MCP_SRV_MediaCtrlReq_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("[MP %d] Media Control Request has been received from remote MCP Client : \n",
                  pNotification->ContentControlID);
      LOG_INFO_APP("     Ctrl Type : 0x%02x\n",p_info->CtrlOp);
      LOG_INFO_APP("     Value : %d\n",p_info->Value);
      switch (p_info->CtrlOp)
      {
        case MCP_MEDIA_CTRL_PLAY:
          /* The media player shall start playing the current track and call MCP_SERVER_SetMediaState()
           * with State MCP_MEDIA_STATE_PLAYING
           */
          LOG_INFO_APP("[MP %d] Media State 0x%02X transits to PLAYING State (0x%02x)\n",
                      pNotification->ContentControlID,
                      p_mediaplayer->MediaState,
                      MCP_MEDIA_STATE_PLAYING);
          TMAPAPP_SetMediaState(pNotification->ContentControlID,MCP_MEDIA_STATE_PLAYING);
          status = MCP_SERVER_CtrlOpRsp(pNotification->ConnHandle,
                                       pNotification->ContentControlID,
                                       p_info->CtrlOp,
                                       BLE_STATUS_SUCCESS);
          LOG_INFO_APP("[MP %d] Response to Ctrl Operation Request status 0x%02X\n",
                      pNotification->ContentControlID,
                      status);
          break;

        case MCP_MEDIA_CTRL_PAUSE:
          if (p_mediaplayer->MediaState == MCP_MEDIA_STATE_PLAYING)
          {
            /* If the media state is in PLAYING state, the media player shall pause playing the current
            * track and call MCP_SERVER_SetMediaState() with State MCP_MEDIA_STATE_PAUSED.
            */
            LOG_INFO_APP("[MP %d] Media State Playing transits to PAUSED State\n",pNotification->ContentControlID);
            TMAPAPP_SetMediaState(pNotification->ContentControlID,MCP_MEDIA_STATE_PAUSED);
            UTIL_TIMER_Stop(&(p_mediaplayer->TimerTrackPosition_Id));
          }
          else if (p_mediaplayer->MediaState == MCP_MEDIA_STATE_SEEKING)
          {
            /* If the media state is in SEEKING state, the media player shall stop seeking, set the current
            * track and track position, thanks to the MCP_SERVER_SetTrackPosition(), as a result of seeking,
            * and call MCP_SERVER_SetMediaState() with State MCP_MEDIA_STATE_PAUSED.
            */
            LOG_INFO_APP("[MP %d] Media State Seeking transits to PAUSED State\n",pNotification->ContentControlID);
            UTIL_TIMER_Stop(&(p_mediaplayer->TimerTrackPosition_Id));
            TMAPAPP_SetMediaState(pNotification->ContentControlID,MCP_MEDIA_STATE_PAUSED);
            status =  MCP_SERVER_SetTrackPosition(pNotification->ContentControlID,p_mediaplayer->TrackPosition);
            LOG_INFO_APP("[MP %d] Set Track Position (%d) of Track %d in Group %d :status 0x%02X\n",
                        pNotification->ContentControlID,
                        (int32_t)p_mediaplayer->TrackPosition,
                        p_mediaplayer->TrackID,
                        p_mediaplayer->GroupID,
                        status);
          }
          status = MCP_SERVER_CtrlOpRsp(pNotification->ConnHandle,
                                       pNotification->ContentControlID,
                                       p_info->CtrlOp,
                                       BLE_STATUS_SUCCESS);
          LOG_INFO_APP("[MP %d] Response to Ctrl Operation Request status 0x%02X\n",
                      pNotification->ContentControlID,
                      status);
          break;

        case MCP_MEDIA_CTRL_STOP:
          /* The media player shall stop any activity  and call MCP_SERVER_SetMediaState() with State
           * MCP_MEDIA_STATE_PAUSED. The track position shall be set to the beginning of the current track
           * thanks to MCP_SERVER_SetTrackPosition().
           */
          UTIL_TIMER_Stop(&(p_mediaplayer->TimerTrackPosition_Id));
          LOG_INFO_APP("[MP %d] Media State 0x%02X transits to PAUSED State\n",
                      pNotification->ContentControlID,
                      p_mediaplayer->MediaState);
          TMAPAPP_SetMediaState(pNotification->ContentControlID,MCP_MEDIA_STATE_PAUSED);
          p_mediaplayer->TrackPosition = 0;
          status =  MCP_SERVER_SetTrackPosition(pNotification->ContentControlID,p_mediaplayer->TrackPosition);
          LOG_INFO_APP("[MP %d] Set Track Position (%d) of Track %d in Group %d :status 0x%02X\n",
                      pNotification->ContentControlID,
                      (int32_t)p_mediaplayer->TrackPosition,
                      p_mediaplayer->TrackID,
                      p_mediaplayer->GroupID,
                      status);
          status = MCP_SERVER_CtrlOpRsp(pNotification->ConnHandle,
                                       pNotification->ContentControlID,
                                       p_info->CtrlOp,
                                       BLE_STATUS_SUCCESS);
          LOG_INFO_APP("[MP %d] Response to Ctrl Operation Request status 0x%02X\n",
                      pNotification->ContentControlID,
                      status);
          break;

        case MCP_MEDIA_CTRL_PREVIOUS_TRACK:
        case MCP_MEDIA_CTRL_NEXT_TRACK:
        case MCP_MEDIA_CTRL_FIRST_TRACK:
        case MCP_MEDIA_CTRL_LAST_TRACK:
        case MCP_MEDIA_CTRL_GOTO_TRACK:
          if (p_mediaplayer->MediaState == MCP_MEDIA_STATE_INACTIVE)
          {
            LOG_INFO_APP("[MP %d] Media State Inactive transits to PAUSED State\n",
                        pNotification->ContentControlID,
                        p_mediaplayer->MediaState);
            TMAPAPP_SetMediaState(pNotification->ContentControlID,MCP_MEDIA_STATE_PAUSED);
          }
          else if (p_mediaplayer->MediaState == MCP_MEDIA_STATE_SEEKING)
          {
            p_mediaplayer->TrackPosition = 0;
            UTIL_TIMER_Stop(&(p_mediaplayer->TimerTrackPosition_Id));
            LOG_INFO_APP("[MP %d] Media State Seeking transits to PAUSED State\n",
                        pNotification->ContentControlID,
                        p_mediaplayer->MediaState);
            TMAPAPP_SetMediaState(p_mediaplayer->CCID,MCP_MEDIA_STATE_PAUSED);
          }
          else if (p_mediaplayer->MediaState == MCP_MEDIA_STATE_PLAYING)
          {
            p_mediaplayer->TrackPosition = 0;
            /*Restart the timer*/
            UTIL_TIMER_Stop(&(p_mediaplayer->TimerTrackPosition_Id));
            UTIL_TIMER_StartWithPeriod( &(p_mediaplayer->TimerTrackPosition_Id), p_mediaplayer->TrackPositionInterval);
          }
          if (p_info->CtrlOp == MCP_MEDIA_CTRL_FIRST_TRACK)
          {
            p_mediaplayer->TrackID = 1u;
          }
          else if (p_info->CtrlOp == MCP_MEDIA_CTRL_PREVIOUS_TRACK)
          {
            if( p_mediaplayer->TrackID > 1u)
            {
             p_mediaplayer->TrackID -= 1u;
            }
            else
            {
              p_mediaplayer->TrackID = 1u;
            }
          }
          else if (p_info->CtrlOp == MCP_MEDIA_CTRL_NEXT_TRACK)
          {
            if( p_mediaplayer->TrackID == APP_MCP_NUM_TRACKS)
            {
             p_mediaplayer->TrackID = 1u;
            }
            else
            {
              p_mediaplayer->TrackID += 1u;
            }
          }
          else if (p_info->CtrlOp == MCP_MEDIA_CTRL_LAST_TRACK)
          {
            p_mediaplayer->TrackID = APP_MCP_NUM_TRACKS;
          }
          else if (p_info->CtrlOp == MCP_MEDIA_CTRL_GOTO_TRACK)
          {
            if ( p_info->Value > 0)
            {
              p_mediaplayer->TrackID = p_info->Value;
            }
            else if ( p_info->Value < 0)
            {
              p_mediaplayer->TrackID = APP_MCP_NUM_TRACKS - (abs(p_info->Value) -1u);
            }
          }
          LOG_INFO_APP("[MP %d] Change to Track %d\n",
                        pNotification->ContentControlID,
                        p_mediaplayer->TrackID);
          status = MCP_SERVER_SetTrackTitle(pNotification->ContentControlID,
                                            MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].pTitle,
                                            6u);
          LOG_INFO_APP("[MP %d] Set Track Title of Track %d in Group %d : status 0x%02X\n",
                        pNotification->ContentControlID,
                        p_mediaplayer->TrackID,
                        p_mediaplayer->GroupID,
                        status);
          if (status == BLE_STATUS_SUCCESS)
          {
            Menu_SetTrackTitle(MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].pTitle,
                             MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].TitleLen);
          }
          status = MCP_SERVER_SetTrackDuration(pNotification->ContentControlID,
                                               MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].Duration);
          LOG_INFO_APP("[MP %d] Set Track Duration (%d) of Track %d in Group %d : status 0x%02X\n",
                        pNotification->ContentControlID,
                        MCPAPP_Track[p_mediaplayer->GroupID-1u][p_mediaplayer->TrackID-1u].Duration,
                        p_mediaplayer->TrackID,
                        p_mediaplayer->GroupID,
                        status);
          p_mediaplayer->TrackPosition = 0;
          status =  MCP_SERVER_SetTrackPosition(pNotification->ContentControlID,p_mediaplayer->TrackPosition);
          LOG_INFO_APP("[MP %d] Set Track Position (%d) of Track %d in Group %d : status 0x%02X\n",
                        pNotification->ContentControlID,
                        (int32_t)p_mediaplayer->TrackPosition,
                        p_mediaplayer->TrackID,
                        p_mediaplayer->GroupID,
                        status);
          status = MCP_SERVER_NotifyTrackChanged(pNotification->ContentControlID);
          LOG_INFO_APP("[MP %d] Track Change Notification status 0x%02X\n",pNotification->ContentControlID);
          status = MCP_SERVER_CtrlOpRsp(pNotification->ConnHandle,
                                       pNotification->ContentControlID,
                                       p_info->CtrlOp,
                                       BLE_STATUS_SUCCESS);
          LOG_INFO_APP("[MP %d] Response to Ctrl Operation Request status 0x%02X\n",
                      pNotification->ContentControlID,
                      status);
        break;

        default:
         LOG_INFO_APP("Ctrl Operation not handled in Application Layer\n");
        break;
      }
    }
    break;
    default:
      break;
  }
}
#endif /* (APP_MCP_ROLE_SERVER_SUPPORT == 1u) */

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

/* USER CODE BEGIN FD */

/* USER CODE END FD */

