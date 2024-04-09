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
        CAP_MEM_TOTAL_BUFFER_SIZE(APP_CAP_ROLE,\
                                  CFG_BLE_NUM_LINK, \
                                  0u,0u, \
                                  0u, \
                                  0u, \
                                  APP_CCP_CLT_FEATURE_SUPPORT,APP_CCP_NUM_REMOTE_BEARER_INSTANCES,\
                                  APP_MCP_CLT_FEATURE_SUPPORT,APP_MCP_NUM_REMOTE_MEDIA_PLAYER_INSTANCES, \
                                  MAX_NUM_USR_SNK_ASE,MAX_NUM_USR_SRC_ASE, \
                                  0u,0u,0u, \
                                  0u,0u)

/*Memory size required to allocate resource for Published Audio Capabilities Server Context*/
#define BAP_PACS_SRV_DYN_ALLOC_SIZE \
        BAP_PACS_SRV_TOTAL_BUFFER_SIZE(APP_NUM_SNK_PAC_RECORDS,APP_NUM_SRC_PAC_RECORDS)

/*Memory size required to allocate resource for Audio Stream Endpoint in Unicats Server Context*/
#define BAP_USR_ASE_BLOCKS_SIZE \
        BAP_MEM_BLOCKS_ASE_PER_CONN_SIZE_BYTES(MAX_NUM_USR_SNK_ASE, \
                                               MAX_NUM_USR_SRC_ASE, \
                                               MAX_USR_CODEC_CONFIG_SIZE, \
                                               MAX_USR_METADATA_SIZE)

/*Memory size required for Audio Stream management in Unicast Server Context*/
#define BAP_ASCS_SRV_DYN_ALLOC_SIZE \
        BAP_ASCS_SRV_TOTAL_BUFFER_SIZE(MAX_NUM_USR_LINK, \
                                       MAX_NUM_USR_SNK_ASE, \
                                       MAX_NUM_USR_SRC_ASE, \
                                       BAP_USR_ASE_BLOCKS_SIZE)


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


#if (APP_CCP_ROLE_CLIENT_SUPPORT == 1u)
#define BLE_CCP_CLT_DYN_ALLOC_SIZE \
        (BLE_CCP_CLT_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK,APP_CCP_NUM_REMOTE_BEARER_INSTANCES,APP_CCP_CALL_URI_LENGTH))
#endif /*(APP_CCP_ROLE_CLIENT_SUPPORT == 1u)*/

#if (APP_MCP_ROLE_CLIENT_SUPPORT == 1u)
#define BLE_MCP_CLT_DYN_ALLOC_SIZE \
        BLE_MCP_CLT_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK,APP_MCP_NUM_REMOTE_MEDIA_PLAYER_INSTANCES)
#endif /*(APP_CCP_ROLE_CLIENT_SUPPORT == 1u)*/

#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)
#define BLE_VCP_RDR_DYN_ALLOC_SIZE\
        BLE_VCP_RENDERER_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK, \
                                           APP_VCP_RDR_NUM_AIC_INSTANCES, \
                                           APP_VCP_RDR_NUM_VOC_INSTANCES)
#endif /*(APP_VCP_ROLE_RENDERER_SUPPORT == 1u)*/

#define TMAP_DYN_ALLOC_SIZE      TMAP_MEM_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK)

#define VOLUME_STEP 10

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
static uint32_t aPACSSrvMemBuffer[DIVC(BAP_PACS_SRV_DYN_ALLOC_SIZE,4)];
static uint32_t aASCSSrvMemBuffer[DIVC(BAP_ASCS_SRV_DYN_ALLOC_SIZE,4)];
static uint32_t aISOChnlMemBuffer[DIVC(BAP_ISO_CHNL_DYN_ALLOC_SIZE,4)];
static uint32_t aNvmMgmtMemBuffer[DIVC(BAP_NVM_MGMT_DYN_ALLOC_SIZE,4)];
static uint32_t audio_init_buffer[BLE_AUDIO_DYN_ALLOC_SIZE];
static BleAudioInit_t pBleAudioInit;
#if (APP_CCP_ROLE_CLIENT_SUPPORT == 1u)
static uint32_t aCCPCltMemBuffer[DIVC(BLE_CCP_CLT_DYN_ALLOC_SIZE,4)];
#endif /*APP_CCP_ROLE_CLIENT_SUPPORT == 1u)*/
#if (APP_MCP_ROLE_CLIENT_SUPPORT == 1u)
static uint32_t aMCPCltMemBuffer[DIVC(BLE_MCP_CLT_DYN_ALLOC_SIZE,4)];
#endif /*(APP_MCP_ROLE_CLIENT_SUPPORT == 1u)*/
#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)
static uint32_t aRenderMemBuffer[DIVC(BLE_VCP_RDR_DYN_ALLOC_SIZE,4)];
#endif /*(APP_VCP_ROLE_RENDERER_SUPPORT == 1u)*/

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

#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)
#if (APP_VCP_RDR_NUM_AIC_INSTANCES > 0)
static VCP_AIC_InitInst_t aAICInst[APP_VCP_RDR_NUM_AIC_INSTANCES];
#endif /*(APP_VCP_RDR_NUM_AIC_INSTANCES > 0)*/
#if (APP_VCP_RDR_NUM_VOC_INSTANCES > 0)
static VCP_VOC_InitInst_t aVOCInst[APP_VCP_RDR_NUM_VOC_INSTANCES];
#endif /*(APP_VCP_RDR_NUM_VOC_INSTANCES > 0)*/
uint8_t Volume = 0x7F;
uint8_t Mute = 0x00;
#endif /*(APP_VCP_ROLE_RENDERER_SUPPORT == 1u)*/

/*Buffer allocation used in TMAP for internal use*/
static uint32_t aTMAPMemBuffer[DIVC(TMAP_DYN_ALLOC_SIZE,4)];

/* Private functions prototypes-----------------------------------------------*/
static tBleStatus CAPAPP_Init(Audio_Role_t AudioRole,uint8_t AudioConfId);
static tBleStatus TMAPAPP_TMAPInit(uint16_t Role);
static void TMAPAPP_CAPNotification(CAP_Notification_Evt_t *pNotification);
static APP_ACL_Conn_t *APP_GetACLConn(uint16_t ConnHandle);
static APP_ACL_Conn_t *APP_AllocateConn(uint16_t ConnHandle);
static APP_ASE_Info_t *APP_GetASE(uint8_t ASE_ID,uint16_t ACL_ConnHandle);
static uint8_t APP_SetupAudioDataPath(uint16_t ACL_ConnHandle,
                                         uint16_t CIS_ConnHandle,
                                         uint8_t ASE_ID,
                                         uint32_t ControllerDelay);
static void start_audio_source(void);
static void start_audio_sink(void);
static uint8_t APP_GetBitsAudioChnlAllocations(Audio_Chnl_Allocation_t ChnlLocations);
static void APP_ParseMetadataParams(APP_ASE_Info_t *pASE,
                                    uint16_t ConnHandle,
                                    uint8_t const *pMetadata,
                                    uint8_t MetadataLength);
static uint8_t TMAPAPP_BuildAdvDataPacket(uint8_t *pAdvData,
                                         uint8_t *AdvPacketLen,
                                         CAP_Announcement_t AnnouncementType,
                                         uint8_t EnableSolicitationRequest);

#if (APP_CCP_ROLE_CLIENT_SUPPORT == 1u)
static void CCP_MetaEvt_Notification(CCP_Notification_Evt_t *pNotification);
#endif /*(APP_CCP_ROLE_CLIENT_SUPPORT == 1u)*/

#if (APP_MCP_ROLE_CLIENT_SUPPORT == 1u)
static void MCP_MetaEvt_Notification(MCP_Notification_Evt_t *pNotification);
#endif /*(APP_MCP_ROLE_CLIENT_SUPPORT == 1u)*/

#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)
static void VCP_MetaEvt_Notification(VCP_Notification_Evt_t *pNotification);
#endif /*(APP_VCP_ROLE_RENDERER_SUPPORT == 1u)*/

static char Hex_To_Char(uint8_t Hex);
static void Print_String(uint8_t *pString, uint8_t StringLen);

/* Exported functions --------------------------------------------------------*/

extern void APP_NotifyToRun(void);

/* Functions Definition ------------------------------------------------------*/

void TMAPAPP_Init()
{
  tBleStatus status;
  /* Init CAP Acceptor with Unicast Server */
  status = CAPAPP_Init((AUDIO_ROLE_SOURCE | AUDIO_ROLE_SINK),
                       USR_AUDIO_CONF_5);
  LOG_INFO_APP("CAPAPP_Init() returns status 0x%02X\n",status);
  if (status == BLE_STATUS_SUCCESS)
  {
    /* Initialize the TMAP layer*/
    status = TMAPAPP_TMAPInit(APP_TMAP_ROLE);
    LOG_INFO_APP("TMAPAPP_TMAPInit() with role 0x%02X returns status 0x%02X\n",APP_TMAP_ROLE,status);
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
      tBleStatus ret;
      /* Restore the GAF Profiles saved in NVM*/
      ret = CAP_Linkup(ConnHandle,NVMLink,0x00);
      LOG_INFO_APP("CAP_Linkup() for GAF restoration for ConnHandle 0x%04X returns status 0x%02X\n",
                  ConnHandle,
                  ret);
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
      APP_ACL_Conn_t *p_conn;
      LOG_INFO_APP("TMAP Linkup Complete Event with ConnHandle 0x%04X is received with status 0x%02X\n",
                  pNotification->ConnHandle,
                  pNotification->Status);
      p_conn = APP_GetACLConn(pNotification->ConnHandle);

      if (p_conn != 0)
      {
        if (p_conn->ConfirmIndicationRequired == 1u)
        {
          /* Confirm indication now that the GATT is available */
          ret = aci_gatt_confirm_indication(pNotification->ConnHandle);
          LOG_INFO_APP("aci_gatt_confirm_indication() returns status 0x%02X\n", ret);
          p_conn->ConfirmIndicationRequired = 0u;
        }
      }

      if (pNotification->Status == BLE_STATUS_SUCCESS)
      {
        APP_ACL_Conn_t *p_conn = APP_GetACLConn(pNotification->ConnHandle);
        if (p_conn != 0)
        {
#if (APP_MCP_ROLE_CLIENT_SUPPORT == 1)
          GAF_Profiles_Link_t current_link = CAP_GetCurrentLinkedProfiles(pNotification->ConnHandle);
          if ((current_link & MCP_LINK) == MCP_LINK)
          {
            ret = MCP_CLIENT_ConfigureTrackTitleNotification(pNotification->ConnHandle,
                                                             p_conn->GenericMediaPlayerCCID,
                                                             0x01u);
            LOG_INFO_APP("MCP_CLIENT_ConfigureTrackTitleNotification() returns status 0x%02X\n", ret);
          }
#endif /* (APP_MCP_ROLE_CLIENT_SUPPORT == 1) */
          p_conn->AudioProfile |= AUDIO_PROFILE_TMAP;
        }
      }


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

uint8_t TMAPAPP_StartAdvertising(CAP_Announcement_t AnnouncementType,uint8_t EnableSolicitationRequest)
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
  uint8_t Adv_Data[ADV_LEN] = {(ADV_AD_FLAGS_LEN-1), AD_TYPE_FLAGS, (FLAG_BIT_LE_GENERAL_DISCOVERABLE_MODE|FLAG_BIT_BR_EDR_NOT_SUPPORTED),
                               (ADV_LOCAL_NAME_LEN-1), AD_TYPE_COMPLETE_LOCAL_NAME,
                                 'S','T','M', '3', '2', 'W','B','A', '_', '0', '0', '0', '0'};

  aci_hal_read_config_data(CONFIG_DATA_PUBADDR_OFFSET, &Pb_Addr_Len, &Pb_Addr[0]);
  Adv_Data[ADV_AD_FLAGS_LEN+ADV_LOCAL_NAME_LEN-4] = Hex_To_Char((Pb_Addr[1] & 0xF0) >> 4);
  Adv_Data[ADV_AD_FLAGS_LEN+ADV_LOCAL_NAME_LEN-3] = Hex_To_Char(Pb_Addr[1] & 0x0F);
  Adv_Data[ADV_AD_FLAGS_LEN+ADV_LOCAL_NAME_LEN-2] = Hex_To_Char((Pb_Addr[0] & 0xF0) >> 4);
  Adv_Data[ADV_AD_FLAGS_LEN+ADV_LOCAL_NAME_LEN-1] = Hex_To_Char(Pb_Addr[0] & 0x0F);

  Menu_SetIdentifier((char *)&Adv_Data[ADV_AD_FLAGS_LEN+ADV_LOCAL_NAME_LEN-4]);

  // Start Fast or Low Power Advertising
  status = aci_gap_adv_set_configuration(ADV_IND,
                                         0,
                                         HCI_ADV_EVENT_PROP_CONNECTABLE,
                                         0x0080,
                                         0x00A0,
                                         Primary_Adv_Channel_Map,
                                         GAP_PUBLIC_ADDR,
                                         GAP_PUBLIC_ADDR,
                                         Peer_Address,
                                         NO_WHITE_LIST_USE,
                                         Adv_TX_Power,
                                         Secondary_Adv_Max_Skip,
                                         0x01,
                                         0,
                                         Scan_Req_Notification_Enable);
  LOG_INFO_APP("hci_le_set_extended_advertising_data() returns status 0x%02X\n",status);
  if (status != BLE_STATUS_SUCCESS)
  {
    return status;
  }

  // Set Extended Advertising data
  TMAPAPP_BuildAdvDataPacket(&Adv_Data[(ADV_AD_FLAGS_LEN+ADV_LOCAL_NAME_LEN)],
                            &advPacketLen,
                            AnnouncementType,
                            EnableSolicitationRequest);
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

uint8_t TMAPAPP_StopAdvertising(void)
{
  Adv_Set_t adv_set;
  uint8_t status;
  adv_set.Advertising_Handle = 0;
  adv_set.Duration = 0;
  adv_set.Max_Extended_Advertising_Events = 0;

  status = hci_le_set_extended_advertising_enable(0, 1, &adv_set);

  return status;
}

uint8_t TMAPAPP_Disconnect(void)
{
  uint8_t status;

  status = hci_disconnect(0x01, HCI_REMOTE_USER_TERMINATED_CONNECTION_ERR_CODE);
  LOG_INFO_APP("hci_disconnect() returns status 0x%02X\n",status);

  return status;
}

uint8_t TMAPAPP_VolumeUp(void)
{
#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)
  uint8_t status;
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

  status = VCP_RENDER_SetAbsVolume(volume);
  LOG_INFO_APP("VCP_RENDER_SetAbsVolume() with volume %d returns status 0x%02X\n", volume, status);

  return status;
#else /*#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)*/
  return HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif
}

uint8_t TMAPAPP_VolumeDown(void)
{
#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)
  uint8_t status;
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

  status = VCP_RENDER_SetAbsVolume(volume);
  LOG_INFO_APP("VCP_RENDER_SetAbsVolume() with volume %d returns status 0x%02X\n", volume, status);

  return status;
#else /*#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)*/
  return HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif
}

uint8_t TMAPAPP_ToggleMute(void)
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

tBleStatus TMAPAPP_NextTrack(void)
{
#if (APP_MCP_ROLE_CLIENT_SUPPORT == 1u)
  tBleStatus status;

  status = MCP_CLIENT_MoveNextTrack(TMAPAPP_Context.ACL_Conn[0].Acl_Conn_Handle,
                                    TMAPAPP_Context.ACL_Conn[0].GenericMediaPlayerCCID);
  LOG_INFO_APP("MCP_CLIENT_MoveNextTrack() returns status 0x%02X\n", status);

  return status;
#else /*#if (APP_MCP_ROLE_CLIENT_SUPPORT == 1u)*/
  return HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif
}

tBleStatus TMAPAPP_PreviousTrack(void)
{
#if (APP_MCP_ROLE_CLIENT_SUPPORT == 1u)
  tBleStatus status;

  status = MCP_CLIENT_MovePreviousTrack(TMAPAPP_Context.ACL_Conn[0].Acl_Conn_Handle,
                                    TMAPAPP_Context.ACL_Conn[0].GenericMediaPlayerCCID);
  LOG_INFO_APP("MCP_CLIENT_MovePreviousTrack() returns status 0x%02X\n", status);

  return status;
#else /*#if (APP_MCP_ROLE_CLIENT_SUPPORT == 1u)*/
  return HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif
}

tBleStatus TMAPAPP_PlayPause(void)
{
#if (APP_MCP_ROLE_CLIENT_SUPPORT == 1u)
  tBleStatus status;

  TMAPAPP_Context.ACL_Conn[0].PlayPauseOperation = 1u;
  status = MCP_CLIENT_ReadMediaState(TMAPAPP_Context.ACL_Conn[0].Acl_Conn_Handle,
                                     TMAPAPP_Context.ACL_Conn[0].GenericMediaPlayerCCID);
  LOG_INFO_APP("MCP_CLIENT_ReadMediaState() returns status 0x%02X\n", status);

  return status;
#else /*#if (APP_MCP_ROLE_CLIENT_SUPPORT == 1u)*/
  return HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif
}

tBleStatus TMAPAPP_AnswerCall(void)
{
#if (APP_CCP_ROLE_CLIENT_SUPPORT == 1u)
  tBleStatus status;

  status = CCP_CLIENT_AnswerIncomingCall(TMAPAPP_Context.ACL_Conn[0].Acl_Conn_Handle,
                                         TMAPAPP_Context.ACL_Conn[0].GenericTelephoneBearerCCID,
                                         TMAPAPP_Context.ACL_Conn[0].CurrentCallID);
  LOG_INFO_APP("CCP_CLIENT_AnswerIncomingCall() returns status 0x%02X\n", status);

  return status;
#else /*#if (APP_CCP_ROLE_CLIENT_SUPPORT == 1u)*/
  return HCI_COMMAND_DISALLOWED_ERR_CODE;
#endif
}

tBleStatus TMAPAPP_TerminateCall(void)
{
#if (APP_CCP_ROLE_CLIENT_SUPPORT == 1u)
  tBleStatus status;

  status = CCP_CLIENT_TerminateCall(TMAPAPP_Context.ACL_Conn[0].Acl_Conn_Handle,
                                    TMAPAPP_Context.ACL_Conn[0].GenericTelephoneBearerCCID,
                                    TMAPAPP_Context.ACL_Conn[0].CurrentCallID);
  LOG_INFO_APP("CCP_CLIENT_TerminateCall() returns status 0x%02X\n", status);

  return status;
#else /*#if (APP_CCP_ROLE_CLIENT_SUPPORT == 1u)*/
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
  APP_ACL_Conn_t *p_conn = APP_GetACLConn(ConnHandle);
  if (p_conn == 0)
  {
    p_conn = APP_AllocateConn(ConnHandle);
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
    /*Free CAP Connection Slot*/
    p_conn->Acl_Conn_Handle = 0xFFFFu;
    p_conn->AudioProfile = AUDIO_PROFILE_NONE;
    p_conn->ConfirmIndicationRequired = 0u;
    p_conn->ForceCompleteLinkup = 0u;

    /* Start Advertising */
    uint8_t status = TMAPAPP_StartAdvertising(CAP_GENERAL_ANNOUNCEMENT,0);
    LOG_INFO_APP("TMAPAPP_StartAdvertising() returns status 0x%02X\n",status);

    Menu_SetWaitConnPage();

#if (MAX_NUM_USR_SNK_ASE > 0u)
    for ( i = 0; i < MAX_NUM_USR_SNK_ASE;i++)
    {
      if (p_conn->pASEs->aSnkASE[i].ID != 0x00)
      {
        LOG_INFO_APP("Free ASE ID %d on ACL connection handle 0x%04X\n",p_conn->pASEs->aSnkASE[i].ID,Conn_Handle);
        p_conn->pASEs->aSnkASE[i].ID = 0x00;
        p_conn->pASEs->aSnkASE[i].state  = ASE_STATE_IDLE;
        p_conn->pASEs->aSnkASE[i].num_channels  = 0x00;
        p_conn->pASEs->aSnkASE[i].streaming_audio_context = 0x00u;
        p_conn->pASEs->aSnkASE[i].allocated = 0x00u;
      }
    }
#endif /*(MAX_NUM_USR_SNK_ASE > 0u)*/
#if (MAX_NUM_USR_SRC_ASE > 0u)
    for ( i = 0; i < MAX_NUM_USR_SRC_ASE;i++)
    {
      if (p_conn->pASEs->aSrcASE[i].ID != 0x00)
      {
        LOG_INFO_APP("Free ASE ID %d on ACL connection handle 0x%04X\n",p_conn->pASEs->aSrcASE[i].ID,Conn_Handle);
        p_conn->pASEs->aSrcASE[i].ID = 0x00;
        p_conn->pASEs->aSrcASE[i].state  = ASE_STATE_IDLE;
        p_conn->pASEs->aSrcASE[i].num_channels  = 0x00;
        p_conn->pASEs->aSrcASE[i].streaming_audio_context = 0x00u;
        p_conn->pASEs->aSrcASE[i].allocated = 0x00u;
      }
    }
#endif /*(MAX_NUM_USR_SRC_ASE > 0u)*/
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

static tBleStatus CAPAPP_Init(Audio_Role_t AudioRole,uint8_t AudioConfId)
{
  tBleStatus status;
  uint8_t i;
  Audio_Role_t audio_role = 0;
  Audio_Context_t available_snk_context = 0;
  Audio_Context_t available_src_context = 0;
  Audio_Context_t supported_snk_context = 0;
  Audio_Context_t supported_src_context = 0;
  uint8_t a_snk_cap_id[3] = {LC3_16_2,LC3_24_2};
  uint8_t a_src_cap_id[2] = {LC3_16_2};
  uint8_t num_snk_pac_records = 0;
  uint8_t num_src_pac_records = 0;
  Audio_Location_t audio_locations = MONO_AUDIO_LOCATIONS;

  LOG_INFO_APP("CAPAPP_Init() with conf_audio %d\n",AudioConfId);

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

  APP_BAP_Config.Role = BAP_ROLE_UNICAST_SERVER;
  APP_BAP_Config.MaxNumBleLinks = CFG_BLE_NUM_LINK;
  APP_BAP_Config.MaxNumUSRLinks = MAX_NUM_USR_LINK;

  /*Published Audio Capabilites of Unicast Server and Broadcast Sink Configuration*/
  APP_BAP_Config.PACSSrvConfig.MaxNumSnkPACRecords = APP_NUM_SNK_PAC_RECORDS;
  APP_BAP_Config.PACSSrvConfig.MaxNumSrcPACRecords = APP_NUM_SRC_PAC_RECORDS;
  APP_BAP_Config.PACSSrvConfig.pStartRamAddr = (uint8_t *)&aPACSSrvMemBuffer;
  APP_BAP_Config.PACSSrvConfig.RamSize = BAP_PACS_SRV_DYN_ALLOC_SIZE;

  /*Audio Stream Endpoint of Unicast Server Configuration*/
  APP_BAP_Config.ASCSSrvConfig.AudioRole = AudioRole;
  APP_BAP_Config.ASCSSrvConfig.MaxNumSnkASEs = MAX_NUM_USR_SNK_ASE;
  APP_BAP_Config.ASCSSrvConfig.MaxNumSrcASEs = MAX_NUM_USR_SRC_ASE;
  APP_BAP_Config.ASCSSrvConfig.MaxCodecConfSize = MAX_USR_CODEC_CONFIG_SIZE;
  APP_BAP_Config.ASCSSrvConfig.MaxMetadataLength = MAX_USR_METADATA_SIZE;
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

#if (APP_CCP_ROLE_CLIENT_SUPPORT == 1u)
  APP_CCP_Config.Role = CCP_ROLE_CLIENT;
  APP_CCP_Config.MaxNumBleLinks = CFG_BLE_NUM_LINK;
  APP_CCP_Config.Clt.ProceduresMask = CCP_CLT_SUPPORT_PROC_ALL;
  APP_CCP_Config.Clt.MaxNumTBInstPerConn = APP_CCP_NUM_REMOTE_BEARER_INSTANCES;
  APP_CCP_Config.Clt.MaxCallURILength = APP_CCP_CALL_URI_LENGTH;
  APP_CCP_Config.Clt.pStartRamAddr = (uint8_t*)aCCPCltMemBuffer;
  APP_CCP_Config.Clt.RamSize = BLE_CCP_CLT_DYN_ALLOC_SIZE;
#endif /*(APP_CCP_ROLE_CLIENT_SUPPORT == 1u)*/

#if (APP_MCP_ROLE_CLIENT_SUPPORT == 1u)
  APP_MCP_Config.Role = MCP_ROLE_CLIENT;
  APP_MCP_Config.MaxNumBleLinks = CFG_BLE_NUM_LINK;
  APP_MCP_Config.Clt.MaxNumMPInstPerConn = APP_MCP_NUM_REMOTE_MEDIA_PLAYER_INSTANCES;
  APP_MCP_Config.Clt.pStartRamAddr = (uint8_t*)aMCPCltMemBuffer;
  APP_MCP_Config.Clt.RamSize = BLE_MCP_CLT_DYN_ALLOC_SIZE;
#endif /*(APP_MCP_ROLE_CLIENT_SUPPORT == 1u)*/

#if (APP_VCP_ROLE_RENDERER_SUPPORT == 1u)
  APP_VCP_Config.Role = VCP_ROLE_RENDERER;
  APP_VCP_Config.MaxNumBleLinks = CFG_BLE_NUM_LINK;
  APP_VCP_Config.Renderer.InitialMuteState = 0u;
  APP_VCP_Config.Renderer.InitialVolumeSetting = 2u;
  APP_VCP_Config.Renderer.VolumeStepSize = 1u;
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
  TMAPAPP_Context.NumSnkASEs = 0u;
  TMAPAPP_Context.NumSrcASEs = 0u;

  TMAPAPP_Context.bap_role = APP_BAP_Config.Role;

  for (uint8_t conn = 0; conn< CFG_BLE_NUM_LINK; conn++)
  {
    TMAPAPP_Context.aASEs[conn].acl_conn_handle = 0xFFFFu;
#if (MAX_NUM_USR_SNK_ASE > 0u)
    for (uint8_t i = 0; i< MAX_NUM_USR_SNK_ASE; i++)
    {
      TMAPAPP_Context.aASEs[conn].aSnkASE[i].ID = 0x00;
      TMAPAPP_Context.aASEs[conn].aSnkASE[i].type = ASE_SINK;
      TMAPAPP_Context.aASEs[conn].aSnkASE[i].state  = ASE_STATE_IDLE;
      TMAPAPP_Context.aASEs[conn].aSnkASE[i].num_channels  = 0x00;
      TMAPAPP_Context.aASEs[conn].aSnkASE[i].streaming_audio_context = 0x00u;
      TMAPAPP_Context.aASEs[conn].aSnkASE[i].allocated = 0x00u;
    }
#endif /*(MAX_NUM_USR_SNK_ASE > 0u)*/

#if (MAX_NUM_USR_SRC_ASE > 0u)
    for (uint8_t i = 0; i< MAX_NUM_USR_SRC_ASE; i++)
    {
      TMAPAPP_Context.aASEs[conn].aSrcASE[i].ID = 0x00;
      TMAPAPP_Context.aASEs[conn].aSrcASE[i].state  = ASE_STATE_IDLE;
      TMAPAPP_Context.aASEs[conn].aSrcASE[i].type = ASE_SOURCE;
      TMAPAPP_Context.aASEs[conn].aSrcASE[i].num_channels  = 0x00;
      TMAPAPP_Context.aASEs[conn].aSrcASE[i].streaming_audio_context = 0x00u;
      TMAPAPP_Context.aASEs[conn].aSrcASE[i].allocated = 0x00u;
    }
#endif /*(MAX_NUM_USR_SRC_ASE > 0u)*/
  }

  for (uint8_t i = 0; i< CFG_BLE_NUM_LINK; i++)
  {
    TMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle = 0xFFFFu;
    TMAPAPP_Context.ACL_Conn[i].AudioProfile = AUDIO_PROFILE_NONE;
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

  /*Initialize Flags of the APP Context*/
  TMAPAPP_Context.audio_role_setup = 0x00;
  TMAPAPP_Context.num_cis_src = 0;
  TMAPAPP_Context.num_cis_snk = 0;
  TMAPAPP_Context.audio_role_setup = 0x00;
  TMAPAPP_Context.num_cis_src = 0;
  TMAPAPP_Context.num_cis_snk = 0;
  TMAPAPP_Context.AudioConfId = AudioConfId;

  if (APP_UnicastAudioConf[AudioConfId].NumSnkASEs >= 1)
  {
    audio_role |= AUDIO_ROLE_SINK;
    supported_snk_context = SUPPORTED_SNK_CONTEXTS;
    available_snk_context = SUPPORTED_SNK_CONTEXTS;
#if (APP_NUM_SNK_PAC_RECORDS > 0u)
    /* 2 Audio Sink Mandatory codec specific capabilities */
    num_snk_pac_records = 2;
#endif /* (APP_NUM_SNK_PAC_RECORDS > 0u) */
  }
  else
  {
   supported_snk_context = AUDIO_CONTEXT_UNSPECIFIED;
   available_snk_context = AUDIO_CONTEXT_UNSPECIFIED;
  }

  if(APP_UnicastAudioConf[AudioConfId].NumSrcASEs >= 1){
    audio_role |= AUDIO_ROLE_SOURCE;
    supported_src_context = SUPPORTED_SRC_CONTEXTS;
    available_src_context = SUPPORTED_SRC_CONTEXTS;
#if (APP_NUM_SRC_PAC_RECORDS > 0u)
    num_src_pac_records = 1; /* 1 Audio Source Mandatory codec specific capabilities */
#endif /* (APP_NUM_SRC_PAC_RECORDS > 0u) */
  }
  else
  {
    supported_src_context = AUDIO_CONTEXT_UNSPECIFIED;
    available_src_context = AUDIO_CONTEXT_UNSPECIFIED;
  }

  /*Set Codec ID configuration*/
  if (audio_role & AUDIO_ROLE_SINK)
  {
#if (APP_NUM_SNK_PAC_RECORDS > 0u)
    /* Register the Sink Codec Capabilities */
    for (i = 0; i < num_snk_pac_records ;i++)
    {
      /*Set Codec ID configuration*/
      TMAPAPP_Context.aSnkPACRecord[i].Cap.CodecID.CodingFormat = AUDIO_CODING_FORMAT_LC3;
      TMAPAPP_Context.aSnkPACRecord[i].Cap.CodecID.CompanyID = COMPANY_ID;
      TMAPAPP_Context.aSnkPACRecord[i].Cap.CodecID.VsCodecID = 0x0000;

      /* Set supported codec capabilities configuration */
      TMAPAPP_Context.aSnkCap[i][0] = 0x03;
      TMAPAPP_Context.aSnkCap[i][1] = CODEC_CAP_SAMPLING_FREQ;
      TMAPAPP_Context.aSnkCap[i][2] = (uint8_t) (APP_CodecSpecificCap[a_snk_cap_id[i]].Freq);
      TMAPAPP_Context.aSnkCap[i][3] = (uint8_t) ((APP_CodecSpecificCap[a_snk_cap_id[i]].Freq >> 8 ));

      TMAPAPP_Context.aSnkCap[i][4] = 0x02;
      TMAPAPP_Context.aSnkCap[i][5] = CODEC_CAP_FRAME_DURATION;
      TMAPAPP_Context.aSnkCap[i][6] = APP_CodecSpecificCap[a_snk_cap_id[i]].FrameDuration;

      TMAPAPP_Context.aSnkCap[i][7] = 0x05;
      TMAPAPP_Context.aSnkCap[i][8] = CODEC_CAP_OCTETS_PER_CODEC_FRAME;
      TMAPAPP_Context.aSnkCap[i][9] = (uint8_t) (APP_CodecSpecificCap[a_snk_cap_id[i]].MinOctetsPerCodecFrame);
      TMAPAPP_Context.aSnkCap[i][10] = (uint8_t) ((APP_CodecSpecificCap[a_snk_cap_id[i]].MinOctetsPerCodecFrame >> 8 ));
      TMAPAPP_Context.aSnkCap[i][11] = (uint8_t) (APP_CodecSpecificCap[a_snk_cap_id[i]].MaxOctetsPerCodecFrame);
      TMAPAPP_Context.aSnkCap[i][12] = (uint8_t) ((APP_CodecSpecificCap[a_snk_cap_id[i]].MaxOctetsPerCodecFrame >> 8 ));

      TMAPAPP_Context.aSnkCap[i][13] = 0x02;
      TMAPAPP_Context.aSnkCap[i][14] = CODEC_CAP_AUDIO_CHNL_COUNTS;
      TMAPAPP_Context.aSnkCap[i][15] = APP_UnicastAudioConf[AudioConfId].NumAudioChnlsPerSnkASE;

      TMAPAPP_Context.aSnkPACRecord[i].Cap.pSpecificCap = &TMAPAPP_Context.aSnkCap[i][0];
      TMAPAPP_Context.aSnkPACRecord[i].Cap.SpecificCapLength = 16u;

      /* metadata in Audio Codec Configuration*/
      TMAPAPP_Context.aSnkPACRecord[i].Cap.MetadataLength = 0x04;
      TMAPAPP_Context.aSnkMetadata[i][0] = 0x03;
      TMAPAPP_Context.aSnkMetadata[i][1] = 0x01;
      if (APP_CodecSpecificCap[a_snk_cap_id[i]].Freq == SUPPORTED_SAMPLE_FREQ_16000_HZ)
      {
        TMAPAPP_Context.aSnkMetadata[i][2] = 0x02;
        TMAPAPP_Context.aSnkMetadata[i][3] = 0x00;
      }
      else
      {
        TMAPAPP_Context.aSnkMetadata[i][2] = 0x04;
        TMAPAPP_Context.aSnkMetadata[i][3] = 0x00;
      }
      TMAPAPP_Context.aSnkPACRecord[i].Cap.pMetadata = &TMAPAPP_Context.aSnkMetadata[i][0];

      status = CAP_RegisterSnkPACRecord(&TMAPAPP_Context.aSnkPACRecord[i],&TMAPAPP_Context.aSnkPACRecordHandle[i]);
      if (status == BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("Successfully registered Sink Audio Codec record id %d\n",i);
      }
      else
      {
        LOG_INFO_APP("Failed to register Sink Audio Codec record id %d\n",i);
        return status;
      }
    }
#endif /* (APP_NUM_SNK_PAC_RECORDS > 0u) */

    if (APP_UnicastAudioConf[AudioConfId].MinSnkAudioLocations >= 1)
    {

      if (APP_UnicastAudioConf[AudioConfId].MinSnkAudioLocations == 1)
      {
        audio_locations = MONO_AUDIO_LOCATIONS;
      }
      if (APP_UnicastAudioConf[AudioConfId].MinSnkAudioLocations == 2)
      {
        audio_locations = STEREO_AUDIO_LOCATIONS;
      }
      /*
       * Set the supported Audio Locations, which will be exposed
       * to remote device for reception of audio data
       */
      status = CAP_SetSnkAudioLocations(audio_locations);
      if (status == BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("Successfully set the Supported Sink Audio Locations 0x%08X\n",audio_locations);
      }
      else {
        LOG_INFO_APP("Failed to set the Supported Sink Audio Locations\n");
        return status;
      }
    }
    else
    {
      if(APP_UnicastAudioConf[AudioConfId].NumSnkASEs >= 1){
        status = CAP_SetSnkAudioLocations(MONO_AUDIO_LOCATIONS);
        if (status == BLE_STATUS_SUCCESS)
        {
          LOG_INFO_APP("Successfully set the Supported Sink Audio Locations 0x%08X\n",MONO_AUDIO_LOCATIONS);
        }
        else {
          LOG_INFO_APP("Failed to set the Supported Sink Audio Locations\n");
          return status;
        }
      }
    }
  }

  if (audio_role & AUDIO_ROLE_SOURCE)
  {
#if (APP_NUM_SRC_PAC_RECORDS > 0u)
    /* Register the Source Codec Capabilities */
    for (i = 0; i < num_src_pac_records ;i++)
    {
      /*Set Codec ID configuration*/
      TMAPAPP_Context.aSrcPACRecord[i].Cap.CodecID.CodingFormat = AUDIO_CODING_FORMAT_LC3;
      TMAPAPP_Context.aSrcPACRecord[i].Cap.CodecID.CompanyID = COMPANY_ID;
      TMAPAPP_Context.aSrcPACRecord[i].Cap.CodecID.VsCodecID = 0x0000;

      /* Set supported codec capabilities configuration */
      TMAPAPP_Context.aSrcCap[i][0] = 0x03;
      TMAPAPP_Context.aSrcCap[i][1] = CODEC_CAP_SAMPLING_FREQ;
      TMAPAPP_Context.aSrcCap[i][2] = (uint8_t) (APP_CodecSpecificCap[a_src_cap_id[i]].Freq);
      TMAPAPP_Context.aSrcCap[i][3] = (uint8_t) ((APP_CodecSpecificCap[a_src_cap_id[i]].Freq >> 8 ));

      TMAPAPP_Context.aSrcCap[i][4] = 0x02;
      TMAPAPP_Context.aSrcCap[i][5] = CODEC_CAP_FRAME_DURATION;
      TMAPAPP_Context.aSrcCap[i][6] = APP_CodecSpecificCap[a_src_cap_id[i]].FrameDuration;

      TMAPAPP_Context.aSrcCap[i][7] = 0x05;
      TMAPAPP_Context.aSrcCap[i][8] = CODEC_CAP_OCTETS_PER_CODEC_FRAME;
      TMAPAPP_Context.aSrcCap[i][9] = (uint8_t) (APP_CodecSpecificCap[a_src_cap_id[i]].MinOctetsPerCodecFrame);
      TMAPAPP_Context.aSrcCap[i][10] = (uint8_t) ((APP_CodecSpecificCap[a_src_cap_id[i]].MinOctetsPerCodecFrame >> 8 ));
      TMAPAPP_Context.aSrcCap[i][11] = (uint8_t) (APP_CodecSpecificCap[a_src_cap_id[i]].MaxOctetsPerCodecFrame);
      TMAPAPP_Context.aSrcCap[i][12] = (uint8_t) ((APP_CodecSpecificCap[a_src_cap_id[i]].MaxOctetsPerCodecFrame >> 8 ));

      TMAPAPP_Context.aSrcCap[i][13] = 0x02;
      TMAPAPP_Context.aSrcCap[i][14] = CODEC_CAP_AUDIO_CHNL_COUNTS;
      TMAPAPP_Context.aSrcCap[i][15] = APP_UnicastAudioConf[AudioConfId].NumAudioChnlsPerSrcASE;

      TMAPAPP_Context.aSrcPACRecord[i].Cap.pSpecificCap = &TMAPAPP_Context.aSrcCap[i][0];
      TMAPAPP_Context.aSrcPACRecord[i].Cap.SpecificCapLength = 16u;

      /* metadata in Audio Codec Configuration*/
      TMAPAPP_Context.aSrcPACRecord[i].Cap.MetadataLength = 0x04;
      TMAPAPP_Context.aSrcMetadata[i][0] = 0x03;
      TMAPAPP_Context.aSrcMetadata[i][1] = 0x01;
      TMAPAPP_Context.aSrcMetadata[i][2] = 0x02;
      TMAPAPP_Context.aSrcMetadata[i][3] = 0x00;
      TMAPAPP_Context.aSrcPACRecord[i].Cap.pMetadata = &TMAPAPP_Context.aSrcMetadata[i][0];

      status = CAP_RegisterSrcPACRecord(&TMAPAPP_Context.aSrcPACRecord[i],&TMAPAPP_Context.aSrcPACRecordHandle[i]);
      if (status == BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("Successfully registered Source Audio Codec record id %d\n",i);
      }
      else
      {
        LOG_INFO_APP("Failed to register Source Audio Codec record id %d\n",i);
        return status;
      }
    }
#endif /*(APP_NUM_SRC_PAC_RECORDS > 0u)*/
    if (APP_UnicastAudioConf[AudioConfId].MinSrcAudioLocations >= 1)
    {

      if (APP_UnicastAudioConf[AudioConfId].MinSrcAudioLocations == 1)
      {
        audio_locations = MONO_AUDIO_LOCATIONS;
      }
      if (APP_UnicastAudioConf[AudioConfId].MinSrcAudioLocations == 2)
      {
        audio_locations = STEREO_AUDIO_LOCATIONS;
      }
      /*
       * Set the supported Audio Locations, which will be exposed
       * to remote device for reception of audio data
       */
      status = CAP_SetSrcAudioLocations(audio_locations);
      if (status == BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("Successfully set the Supported Source Audio Locations 0x%08X\n",audio_locations);
      }
      else {
        LOG_INFO_APP("Failed to set the Supported Source Audio Locations\n");
        return status;
      }
    }
    else
    {
      if(APP_UnicastAudioConf[AudioConfId].NumSrcASEs >= 1){
        status = CAP_SetSrcAudioLocations(MONO_AUDIO_LOCATIONS);
        if (status == BLE_STATUS_SUCCESS)
        {
          LOG_INFO_APP("Successfully set the Supported Source Audio Locations 0x%08X\n",MONO_AUDIO_LOCATIONS);
        }
        else
        {
          LOG_INFO_APP("Failed to set the Supported Source Audio Locations\n");
          return status;
        }
      }
    }
  }

  /* Set supported audio context for reception and transmission */
  status = CAP_SetSupportedAudioContexts(supported_snk_context, supported_src_context);
  if (status == BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("Successfully set the Supported Audio Contexts:\n");
    LOG_INFO_APP("Snk Audio Contexts : 0x%04X\n",supported_snk_context);
    LOG_INFO_APP("Src Audio Contexts : 0x%04X\n",supported_src_context);
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
  }
  else
  {
    LOG_INFO_APP("Failed to set the Available Audio Contexts\n");
    return status;
  }
  /* Register the Audio Stream Endpoints*/
  status = CAP_RegisterAudioChannels(APP_UnicastAudioConf[AudioConfId].NumSnkASEs,
                                     APP_UnicastAudioConf[AudioConfId].NumSrcASEs);
  if (status == BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("Successfully register %ld Sink Audio Stream Endpoints\n",
                APP_UnicastAudioConf[AudioConfId].NumSnkASEs);
    LOG_INFO_APP("Successfully register %ld Source Audio Stream Endpoints\n",
                APP_UnicastAudioConf[AudioConfId].NumSrcASEs);
    TMAPAPP_Context.NumSnkASEs = APP_UnicastAudioConf[AudioConfId].NumSnkASEs;
    TMAPAPP_Context.NumSrcASEs = APP_UnicastAudioConf[AudioConfId].NumSrcASEs;
  }
  else
  {
    LOG_INFO_APP("Failed to register Audio Channels\n");
    return status;
  }
  return status;
}

static tBleStatus TMAPAPP_TMAPInit(uint16_t Role)
{
  uint8_t status;
  TMAP_Config_t tmap_config;
  uint8_t i;
#if (APP_NUM_SNK_PAC_RECORDS > 0)
  uint8_t snk_record_i = MIN_NUM_BAP_SNK_PAC_RECORDS;
#endif /*(APP_NUM_SNK_PAC_RECORDS > 0)*/
#if (APP_NUM_SRC_PAC_RECORDS > 0)
  uint8_t src_record_i = MIN_NUM_BAP_SRC_PAC_RECORDS;
#endif /*(APP_NUM_SRC_PAC_RECORDS > 0)*/
  uint8_t ret = BLE_STATUS_SUCCESS;
  uint8_t num_ct_pac_records = 3;
  uint8_t a_ct_pac_id[3] = {2, 6, 7};
#if (APP_NUM_SNK_PAC_RECORDS > 0u)
  uint8_t num_umr_pac_records = 6;
  uint8_t a_umr_pac_id[6] = {10, 11, 12, 13, 14, 15};
#endif /*(APP_NUM_SNK_PAC_RECORDS > 0)*/

  tmap_config.Role = Role;
  tmap_config.MaxNumBleLinks = CFG_BLE_NUM_LINK;
  tmap_config.pStartRamAddr = (uint8_t *)&aTMAPMemBuffer;
  tmap_config.RamSize = TMAP_DYN_ALLOC_SIZE;

  status = USECASE_DEV_MGMT_Init();

  if (status == BLE_STATUS_SUCCESS)
  {
    if (tmap_config.Role & TMAP_ROLE_CALL_TERMINAL)
    {
#if (APP_NUM_SNK_PAC_RECORDS > 0u)
      /* Register the mandatory CT Role Sink Codec Capabilities */
      for (i = 0; i < num_ct_pac_records ;i++)
      {
        /*Set Codec ID configuration*/
        TMAPAPP_Context.aSnkPACRecord[snk_record_i].Cap.CodecID.CodingFormat = AUDIO_CODING_FORMAT_LC3;
        TMAPAPP_Context.aSnkPACRecord[snk_record_i].Cap.CodecID.CompanyID = COMPANY_ID;
        TMAPAPP_Context.aSnkPACRecord[snk_record_i].Cap.CodecID.VsCodecID = 0x0000;

        /* Set supported codec capabilities configuration */
        TMAPAPP_Context.aSnkCap[snk_record_i][0] = 0x03;
        TMAPAPP_Context.aSnkCap[snk_record_i][1] = CODEC_CAP_SAMPLING_FREQ;
        TMAPAPP_Context.aSnkCap[snk_record_i][2] = (uint8_t) (APP_CodecSpecificCap[a_ct_pac_id[i]].Freq);
        TMAPAPP_Context.aSnkCap[snk_record_i][3] = (uint8_t) ((APP_CodecSpecificCap[a_ct_pac_id[i]].Freq >> 8 ));

        TMAPAPP_Context.aSnkCap[snk_record_i][4] = 0x02;
        TMAPAPP_Context.aSnkCap[snk_record_i][5] = CODEC_CAP_FRAME_DURATION;
        TMAPAPP_Context.aSnkCap[snk_record_i][6] = APP_CodecSpecificCap[a_ct_pac_id[i]].FrameDuration;

        TMAPAPP_Context.aSnkCap[snk_record_i][7] = 0x05;
        TMAPAPP_Context.aSnkCap[snk_record_i][8] = CODEC_CAP_OCTETS_PER_CODEC_FRAME;
        TMAPAPP_Context.aSnkCap[snk_record_i][9] = (uint8_t) (APP_CodecSpecificCap[a_ct_pac_id[i]].MinOctetsPerCodecFrame);
        TMAPAPP_Context.aSnkCap[snk_record_i][10] = (uint8_t) ((APP_CodecSpecificCap[a_ct_pac_id[i]].MinOctetsPerCodecFrame >> 8 ));
        TMAPAPP_Context.aSnkCap[snk_record_i][11] = (uint8_t) (APP_CodecSpecificCap[a_ct_pac_id[i]].MaxOctetsPerCodecFrame);
        TMAPAPP_Context.aSnkCap[snk_record_i][12] = (uint8_t) ((APP_CodecSpecificCap[a_ct_pac_id[i]].MaxOctetsPerCodecFrame >> 8 ));

        TMAPAPP_Context.aSnkCap[snk_record_i][13] = 0x02;
        TMAPAPP_Context.aSnkCap[snk_record_i][14] = CODEC_CAP_AUDIO_CHNL_COUNTS;
        TMAPAPP_Context.aSnkCap[snk_record_i][15] = 0x03;

        TMAPAPP_Context.aSnkPACRecord[snk_record_i].Cap.pSpecificCap = &TMAPAPP_Context.aSnkCap[snk_record_i][0];
        TMAPAPP_Context.aSnkPACRecord[snk_record_i].Cap.SpecificCapLength = 16u;

        /* metadata in Audio Codec Configuration*/
        TMAPAPP_Context.aSnkPACRecord[snk_record_i].Cap.MetadataLength = 0x00;

        TMAPAPP_Context.aSnkPACRecord[snk_record_i].Cap.pMetadata = &TMAPAPP_Context.aSnkMetadata[snk_record_i][0];
        ret = CAP_RegisterSnkPACRecord(&TMAPAPP_Context.aSnkPACRecord[snk_record_i],&TMAPAPP_Context.aSnkPACRecordHandle[snk_record_i]);
        LOG_INFO_APP("Registered TMAP Snk PAC Record of id %d with status 0x%04X\n", a_ct_pac_id[i], ret);
        if (ret == BLE_STATUS_SUCCESS)
        {
          snk_record_i++;
        }
        else
        {
          return ret;
        }
      }
#endif /* (APP_NUM_SNK_PAC_RECORDS > 0u)*/

#if (APP_NUM_SRC_PAC_RECORDS > 0u)
      /* Register the mandatory CT Role Source Codec Capabilities */
      for (i = 0; i < num_ct_pac_records ;i++)
      {
        /*Set Codec ID configuration*/
        TMAPAPP_Context.aSrcPACRecord[src_record_i].Cap.CodecID.CodingFormat = AUDIO_CODING_FORMAT_LC3;
        TMAPAPP_Context.aSrcPACRecord[src_record_i].Cap.CodecID.CompanyID = COMPANY_ID;
        TMAPAPP_Context.aSrcPACRecord[src_record_i].Cap.CodecID.VsCodecID = 0x0000;

        /* Set supported codec capabilities configuration */
        TMAPAPP_Context.aSrcCap[src_record_i][0] = 0x03;
        TMAPAPP_Context.aSrcCap[src_record_i][1] = CODEC_CAP_SAMPLING_FREQ;
        TMAPAPP_Context.aSrcCap[src_record_i][2] = (uint8_t) (APP_CodecSpecificCap[a_ct_pac_id[i]].Freq);
        TMAPAPP_Context.aSrcCap[src_record_i][3] = (uint8_t) ((APP_CodecSpecificCap[a_ct_pac_id[i]].Freq >> 8 ));

        TMAPAPP_Context.aSrcCap[src_record_i][4] = 0x02;
        TMAPAPP_Context.aSrcCap[src_record_i][5] = CODEC_CAP_FRAME_DURATION;
        TMAPAPP_Context.aSrcCap[src_record_i][6] = APP_CodecSpecificCap[a_ct_pac_id[i]].FrameDuration;

        TMAPAPP_Context.aSrcCap[src_record_i][7] = 0x05;
        TMAPAPP_Context.aSrcCap[src_record_i][8] = CODEC_CAP_OCTETS_PER_CODEC_FRAME;
        TMAPAPP_Context.aSrcCap[src_record_i][9] = (uint8_t) (APP_CodecSpecificCap[a_ct_pac_id[i]].MinOctetsPerCodecFrame);
        TMAPAPP_Context.aSrcCap[src_record_i][10] = (uint8_t) ((APP_CodecSpecificCap[a_ct_pac_id[i]].MinOctetsPerCodecFrame >> 8 ));
        TMAPAPP_Context.aSrcCap[src_record_i][11] = (uint8_t) (APP_CodecSpecificCap[a_ct_pac_id[i]].MaxOctetsPerCodecFrame);
        TMAPAPP_Context.aSrcCap[src_record_i][12] = (uint8_t) ((APP_CodecSpecificCap[a_ct_pac_id[i]].MaxOctetsPerCodecFrame >> 8 ));

        TMAPAPP_Context.aSrcCap[src_record_i][13] = 0x02;
        TMAPAPP_Context.aSrcCap[src_record_i][14] = CODEC_CAP_AUDIO_CHNL_COUNTS;
        TMAPAPP_Context.aSrcCap[src_record_i][15] = 0x03;

        TMAPAPP_Context.aSrcPACRecord[src_record_i].Cap.pSpecificCap = &TMAPAPP_Context.aSrcCap[src_record_i][0];
        TMAPAPP_Context.aSrcPACRecord[src_record_i].Cap.SpecificCapLength = 16u;

        /* metadata in Audio Codec Configuration*/
        TMAPAPP_Context.aSrcPACRecord[src_record_i].Cap.MetadataLength = 0x00;

        TMAPAPP_Context.aSrcPACRecord[src_record_i].Cap.pMetadata = &TMAPAPP_Context.aSrcMetadata[src_record_i][0];
        ret = CAP_RegisterSrcPACRecord(&TMAPAPP_Context.aSrcPACRecord[src_record_i],&TMAPAPP_Context.aSrcPACRecordHandle[src_record_i]);
        LOG_INFO_APP("Registered TMAP Src PAC Record of id %d with status 0x%04X\n", a_ct_pac_id[i], ret);
        if (ret == BLE_STATUS_SUCCESS)
        {
          src_record_i++;
        }
        else
        {
          return ret;
        }
      }
#endif /* (APP_NUM_SRC_PAC_RECORDS > 0u)*/
    }
    if (tmap_config.Role & TMAP_ROLE_UNICAST_MEDIA_RECEIVER)
    {
#if (APP_NUM_SNK_PAC_RECORDS > 0u)
      /* Register the mandatory CT Role Codec Capabilities */
      for (i = 0; i < num_umr_pac_records; i++)
      {
        /*Set Codec ID configuration*/
        TMAPAPP_Context.aSnkPACRecord[snk_record_i].Cap.CodecID.CodingFormat = AUDIO_CODING_FORMAT_LC3;
        TMAPAPP_Context.aSnkPACRecord[snk_record_i].Cap.CodecID.CompanyID = COMPANY_ID;
        TMAPAPP_Context.aSnkPACRecord[snk_record_i].Cap.CodecID.VsCodecID = 0x0000;

        /* Set supported codec capabilities configuration */
        TMAPAPP_Context.aSnkCap[snk_record_i][0] = 0x03;
        TMAPAPP_Context.aSnkCap[snk_record_i][1] = CODEC_CAP_SAMPLING_FREQ;
        TMAPAPP_Context.aSnkCap[snk_record_i][2] = (uint8_t) (APP_CodecSpecificCap[a_umr_pac_id[i]].Freq);
        TMAPAPP_Context.aSnkCap[snk_record_i][3] = (uint8_t) ((APP_CodecSpecificCap[a_umr_pac_id[i]].Freq >> 8 ));

        TMAPAPP_Context.aSnkCap[snk_record_i][4] = 0x02;
        TMAPAPP_Context.aSnkCap[snk_record_i][5] = CODEC_CAP_FRAME_DURATION;
        TMAPAPP_Context.aSnkCap[snk_record_i][6] = APP_CodecSpecificCap[a_umr_pac_id[i]].FrameDuration;

        TMAPAPP_Context.aSnkCap[snk_record_i][7] = 0x05;
        TMAPAPP_Context.aSnkCap[snk_record_i][8] = CODEC_CAP_OCTETS_PER_CODEC_FRAME;
        TMAPAPP_Context.aSnkCap[snk_record_i][9] = (uint8_t) (APP_CodecSpecificCap[a_umr_pac_id[i]].MinOctetsPerCodecFrame);
        TMAPAPP_Context.aSnkCap[snk_record_i][10] = (uint8_t) ((APP_CodecSpecificCap[a_umr_pac_id[i]].MinOctetsPerCodecFrame >> 8 ));
        TMAPAPP_Context.aSnkCap[snk_record_i][11] = (uint8_t) (APP_CodecSpecificCap[a_umr_pac_id[i]].MaxOctetsPerCodecFrame);
        TMAPAPP_Context.aSnkCap[snk_record_i][12] = (uint8_t) ((APP_CodecSpecificCap[a_umr_pac_id[i]].MaxOctetsPerCodecFrame >> 8 ));

        TMAPAPP_Context.aSnkCap[snk_record_i][13] = 0x02;
        TMAPAPP_Context.aSnkCap[snk_record_i][14] = CODEC_CAP_AUDIO_CHNL_COUNTS;
        TMAPAPP_Context.aSnkCap[snk_record_i][15] = 0x03;

        TMAPAPP_Context.aSnkPACRecord[snk_record_i].Cap.pSpecificCap = &TMAPAPP_Context.aSnkCap[snk_record_i][0];
        TMAPAPP_Context.aSnkPACRecord[snk_record_i].Cap.SpecificCapLength = 16u;

        /* metadata in Audio Codec Configuration*/
        TMAPAPP_Context.aSnkPACRecord[snk_record_i].Cap.MetadataLength = 0x00;

        TMAPAPP_Context.aSnkPACRecord[snk_record_i].Cap.pMetadata = &TMAPAPP_Context.aSnkMetadata[snk_record_i][0];

        ret = CAP_RegisterSnkPACRecord(&TMAPAPP_Context.aSnkPACRecord[snk_record_i],&TMAPAPP_Context.aSnkPACRecordHandle[snk_record_i]);
        LOG_INFO_APP("Registered TMAP Snk PAC Record of id %d with status 0x%04X\n", a_umr_pac_id[i], ret);
        if (ret == BLE_STATUS_SUCCESS)
        {
          snk_record_i++;
        }
        else
        {
          return ret;
        }
      }
#endif /* (APP_NUM_SNK_PAC_RECORDS > 0u)*/
    }
  }

  return TMAP_Init(&tmap_config);
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
    case CAP_LINKUP_COMPLETE_EVT:
    {
      LOG_INFO_APP("CAP Linkup process on ConnHandle 0x%04X is complete with status 0x%02X\n",
                        pNotification->ConnHandle,
                        pNotification->Status);
      APP_ACL_Conn_t *p_conn = APP_GetACLConn(pNotification->ConnHandle);
      if (p_conn != 0)
      {
        /* Check if TMAP is already linked or not*/
        if ((p_conn->AudioProfile & AUDIO_PROFILE_TMAP ) == 0)
        {
          tBleStatus ret;
          /* Check if TMAP link is present in NVM from a previous connection*/
          if ((TMAP_DB_IsPresent(p_conn->Peer_Address_Type,p_conn->Peer_Address) == 0) \
              || (p_conn->ForceCompleteLinkup == 1u))
          {
            /*Start Complete TMAP link*/
            ret = TMAP_Linkup(pNotification->ConnHandle,TMAP_LINKUP_MODE_COMPLETE);
            LOG_INFO_APP("Complete TMAP Linkup on ConnHandle 0x%04X is started with status 0x%02X\n",
                        pNotification->ConnHandle,
                        ret);
          }
          else
          {
            /* Restore TMAP link*/
            ret = TMAP_Linkup(pNotification->ConnHandle,TMAP_LINKUP_MODE_RESTORE);
            LOG_INFO_APP("Restore TMAP Linkup on ConnHandle 0x%04X returns status 0x%02X\n",
                        pNotification->ConnHandle,
                        ret);
          }
        }
        else if (p_conn->ConfirmIndicationRequired == 1u)
        {
          /* Confirm indication now that the GATT is available */
          tBleStatus ret;
          ret = aci_gatt_confirm_indication(pNotification->ConnHandle);
          LOG_INFO_APP("aci_gatt_confirm_indication() returns status 0x%02X\n", ret);
          p_conn->ConfirmIndicationRequired = 0u;
#if (APP_MCP_ROLE_CLIENT_SUPPORT == 1)
          GAF_Profiles_Link_t current_link = CAP_GetCurrentLinkedProfiles(pNotification->ConnHandle);
          if ((current_link & MCP_LINK) == MCP_LINK)
          {
            ret = MCP_CLIENT_ConfigureTrackTitleNotification(pNotification->ConnHandle,
                                                             p_conn->GenericMediaPlayerCCID,
                                                             0x01u);
            LOG_INFO_APP("MCP_CLIENT_ConfigureTrackTitleNotification() returns status 0x%02X\n", ret);
          }
#endif /* (APP_MCP_ROLE_CLIENT_SUPPORT == 1) */
        }
        p_conn->ForceCompleteLinkup = 0;
      }
      break;
    }
    case CAP_UNICAST_AUDIOSTARTED_EVT:
    {
      LOG_INFO_APP("CAP Unicast Start Procedure is Complete with status 0x%02X\n",pNotification->Status);
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
      APP_ASE_Info_t *p_ase = 0;
      LOG_INFO_APP("ASE ID %d [Type 0x%02X ]  with remote CAP Initiator on ConnHandle 0x%04X is in State 0x%02X\n",
                  p_info->ASE_ID,
                  p_info->Type,
                  pNotification->ConnHandle,
                  p_info->State);

      APP_ACL_Conn_t *p_conn = APP_GetACLConn(pNotification->ConnHandle);
      if (p_conn != 0)
      {
        p_ase = APP_GetASE(p_info->ASE_ID,pNotification->ConnHandle);
        if (p_ase == 0)
        {
          if (p_info->Type == ASE_SINK)
          {
#if (MAX_NUM_USR_SNK_ASE > 0u)
            for ( uint8_t i = 0; i < TMAPAPP_Context.NumSnkASEs;i++)
            {
              if (p_conn->pASEs->aSnkASE[i].allocated == 0)
              {
                p_ase = &p_conn->pASEs->aSnkASE[i];
                p_ase->allocated = 1u;
              }
            }
#endif /* (MAX_NUM_USR_SNK_ASE > 0u) */
          }
          if (p_info->Type == ASE_SOURCE)
          {
#if (MAX_NUM_USR_SRC_ASE > 0u)
            for (uint8_t i = 0; i < TMAPAPP_Context.NumSrcASEs;i++)
            {
              if (p_conn->pASEs->aSrcASE[i].allocated == 0)
              {
                p_ase = &p_conn->pASEs->aSrcASE[i];
                p_ase->allocated = 1u;
              }
            }
#endif /* (MAX_NUM_USR_SRC_ASE > 0u) */
          }
        }
        if (p_ase != 0)
        {
          p_ase->state = p_info->State;
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
        uint8_t num_snk_ases = TMAPAPP_Context.NumSnkASEs;
        uint8_t num_src_ases = TMAPAPP_Context.NumSrcASEs;

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
        /*maxmimum simulateous Sink ASEs in streaming is 2 sink ASEs*/
        if (num_snk_ases > 2u)
        {
          num_snk_ases = 2u;
        }
        /*maxmimum simulateous Source ASEs in streaming is 2 source ASEs*/
        if (num_src_ases > 2u)
        {
          num_src_ases = 2u;
        }

        /* Adjust the minimum controller delay according to the fact that other ASEs could be in streaming in the
         * same time that the specified ASE
         */
        controller_delay_min = (num_snk_ases * info->SnkControllerDelayMin) + (num_src_ases * info->SrcControllerDelayMin);
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
                          controller_delay_min,
                          (APP_DELAY_SNK_MAX+info->SnkControllerDelayMax));
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
        else /* (info->ControllerDelayMax < PREFFERED_CONTROLLER_DELAY)*/
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
      APP_SetupAudioDataPath(pNotification->ConnHandle,info->CIS_ConnHandle,info->ASE_ID,controller_delay);
      break;
    }
    case CAP_CODEC_CONFIGURED_EVT:
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
        LOG_INFO_APP("Controller Delay : %d us\n",pNotification->Status,info->ControllerDelay);
        LOG_INFO_APP("Transport Latency : %d ms\n",pNotification->Status,info->TransportLatency);
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
    case CAP_CCP_META_EVT:
    {
      CCP_Notification_Evt_t *p_ccp_evt = (CCP_Notification_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Call Control Meta Event 0x%02X is received on ConnHandle 0x%04X:\n",
                  p_ccp_evt->EvtOpcode,
                  p_ccp_evt->ConnHandle);
#if (APP_CCP_ROLE_CLIENT_SUPPORT == 1u)
      CCP_MetaEvt_Notification(p_ccp_evt);
#endif /*(APP_CCP_ROLE_CLIENT_SUPPORT == 1u)*/
      break;
    }
    case CAP_MCP_META_EVT:
    {
      MCP_Notification_Evt_t *p_mcp_evt = (MCP_Notification_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Media Control Meta Event 0x%02X is received on ConnHandle 0x%04X:\n",
                  p_mcp_evt->EvtOpcode,
                  p_mcp_evt->ConnHandle);
#if (APP_MCP_ROLE_CLIENT_SUPPORT == 1u)
      MCP_MetaEvt_Notification(p_mcp_evt);
#endif /*(APP_MCP_ROLE_CLIENT_SUPPORT == 1u)*/
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
    case CAP_MCP_LINKUP_EVT:
        LOG_INFO_APP("Media Control Profile is linked on ConnHandle 0x%04X\n",pNotification->ConnHandle);
      break;

    case CAP_MCP_INST_ACTIVATED_EVT:
      {
        CAP_MCP_InstInfo_Evt_t *mp_info = (CAP_MCP_InstInfo_Evt_t *)pNotification->pInfo;
        LOG_INFO_APP("Media Player Instance with CCID %d is active to control Audio Stream with remote TMAP Device (ConnHandle 0x%04X)\n",
                    mp_info->ContentControlID,
                    pNotification->ConnHandle);
        if (mp_info->OptionFeaturesMask & MCP_FEATURE_PLAYBACK_SPEED)
        {
          LOG_INFO_APP("Playback Speed feature is supported on remote TMAP Device\n");
        }
        if (mp_info->OptionFeaturesMask & MCP_FEATURE_SEEKING_SPEED)
        {
          LOG_INFO_APP("Seeking Speed feature is supported on remote TMAP Device\n");
        }
        if (mp_info->OptionFeaturesMask & MCP_FEATURE_PLAYING_ORDER)
        {
          LOG_INFO_APP("Playing Order feature is supported on remote TMAP Device\n");
        }
        if (mp_info->OptionFeaturesMask & MCP_FEATURE_PLAYING_ORDERS_SUPPORTED)
        {
          LOG_INFO_APP("Supported Playing Orders feature is supported on remote TMAP Device\n");
        }
        if (mp_info->OptionFeaturesMask & MCP_FEATURE_MEDIA_CTRL_OP)
        {
          LOG_INFO_APP("Media Control Operation feature is supported on remote TMAP Device\n");
        }
        if (mp_info->NotifUpdateMask & MCP_NOTIFICATION_MEDIA_PLAYER_NAME)
        {
          LOG_INFO_APP("Media Player Name update Notification feature is supported on remote TMAP Device\n");
        }
        if (mp_info->NotifUpdateMask & MCP_NOTIFICATION_TRACK_TITLE)
        {
          LOG_INFO_APP("Track Title update Notification feature is supported on remote TMAP Device\n");
        }
        if (mp_info->NotifUpdateMask & MCP_NOTIFICATION_TRACK_DURATION)
        {
          LOG_INFO_APP("Track Duration update Notification feature is supported on remote TMAP Device\n");
        }
        if (mp_info->NotifUpdateMask & MCP_NOTIFICATION_TRACK_POSITION)
        {
          LOG_INFO_APP("Track Position update Notification feature is supported on remote TMAP Device\n");
        }
        if (mp_info->NotifUpdateMask & MCP_NOTIFICATION_PLAYBACK_SPEED)
        {
          LOG_INFO_APP("Playback Speed update Notification feature is supported on remote TMAP Device\n");
        }
        if (mp_info->NotifUpdateMask & MCP_NOTIFICATION_SEEKING_SPEED)
        {
          LOG_INFO_APP("Seeking Speed update Notification feature is supported on remote TMAP Device\n");
        }
        if (mp_info->NotifUpdateMask & MCP_NOTIFICATION_PLAYING_ORDER)
        {
          LOG_INFO_APP("Playing Order update Notification feature is supported on remote TMAP Device\n");
        }
        if (mp_info->NotifUpdateMask & MCP_NOTIFICATION_MEDIA_CTRL_OP_SUPPORTED)
        {
          LOG_INFO_APP("Supported Media Control Operation update Notification feature is supported on remote TMAP Device\n");
        }
        MCP_CLIENT_ConfigureTrackTitleNotification(pNotification->ConnHandle,(mp_info->ContentControlID), 0x01u);
      }
      break;

      case CAP_MCP_INST_DEACTIVATED_EVT:
      {
        uint8_t *ccid = (uint8_t *)pNotification->pInfo;
        LOG_INFO_APP("Media Player Instance with CCID %d is no more associated to Audio Stream with remote TMAP Device (connection handle 0x%04X)\n",
                    *ccid,
                    pNotification->ConnHandle);
      }
      break;

    case CAP_CCP_LINKUP_EVT:
        LOG_INFO_APP("Call Control Profile is linked on ConnHandle 0x%04X\n",pNotification->ConnHandle);
      break;

    case CAP_CCP_INST_ACTIVATED_EVT:
      {
        CAP_CCP_InstInfo_Evt_t *info = (CAP_CCP_InstInfo_Evt_t *)pNotification->pInfo;
        LOG_INFO_APP("Telephone Bearer Instance with CCID %d is active to control Audio Stream with remote TMAP Device (connHandle 0x%04X)\n",
                    info->ContentControlID,
                    pNotification->ConnHandle);
        if (info->FeaturesMask & CCP_FEATURE_BEARER_SIGNAL_STRENGTH)
        {
          LOG_INFO_APP("Bearer Signal Strength ( and associated Signal Strength Reporting Interval) is supported on remote TMAP Device\n");
        }
        if (info->FeaturesMask & CCP_FEATURE_INC_CALL_TARGET_BEARER_URI)
        {
          LOG_INFO_APP("Incoming Call Target Bearer URI is supported on remote TMAP Device\n");
        }
        if (info->FeaturesMask & CCP_FEATURE_CALL_FRIENDLY_NAME)
        {
          LOG_INFO_APP("Call friendly Name is supported on remote TMAP Device\n");
        }
      }
      break;

    case CAP_CCP_INST_DEACTIVATED_EVT:
      {
        uint8_t *ccid = (uint8_t *)pNotification->pInfo;
        LOG_INFO_APP("Telephone Bearer Instance with CCID %d is no more associated to Audio Stream with remote TMAP Device (connection handle 0x%04X)\n",
                    *ccid,
                    pNotification->ConnHandle);
      }
      break;
    default:
      break;
  }
}

static uint8_t TMAPAPP_BuildAdvDataPacket(uint8_t *pAdvData,
                                         uint8_t *AdvPacketLen,
                                         CAP_Announcement_t AnnouncementType,
                                         uint8_t EnableSolicitationRequest)
{
  *AdvPacketLen = TMAP_BuildAdvPacket(AnnouncementType,
                                      (void*)0u,
                                      0u,
                                      GAP_APPEARANCE_EARBUD,
                                      &pAdvData[0],
                                      (CAP_ADV_DATA_LEN+TMAP_ADV_DATA_LEN));

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
  APP_ACL_Conn_t *p_conn = APP_GetACLConn(ConnHandle);
  if (p_conn != 0)
  {
  status = LTV_GetStreamingAudioContexts((uint8_t *)pMetadata,MetadataLength,&audio_context);
  if(status == 1u)
    {
      LOG_INFO_APP("ASE ID 0x%02X on ConnHandle 0x%04X is associated to a Streaming Audio Context 0x%04X\n",
                   pASE->ID,
                   ConnHandle,
                   audio_context);
      pASE->streaming_audio_context = audio_context;
    }

    status = LTV_GetPreferredAudioContexts((uint8_t *)pMetadata,MetadataLength,&audio_context);
    if(status == 1u)
    {
      LOG_INFO_APP("ASE ID 0x%02X on ConnHandle 0x%04X is associated to a Preferred Audio Context 0x%04X\n",
                   pASE->ID,
                   ConnHandle,
                   audio_context);
    }
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

static APP_ACL_Conn_t *APP_AllocateConn(uint16_t ConnHandle)
{
  /* Allocate new slot */
  for (uint8_t i = 0; i < CFG_BLE_NUM_LINK; i++)
  {
    if ( TMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle == 0xFFFFu)
    {
      TMAPAPP_Context.ACL_Conn[i].Acl_Conn_Handle = ConnHandle;
      TMAPAPP_Context.ACL_Conn[i].PlayPauseOperation = 0;
      return &TMAPAPP_Context.ACL_Conn[i];
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
#if (MAX_NUM_USR_SNK_ASE > 0)
      if (TMAPAPP_Context.NumSnkASEs > 0u)
      {
        for ( i = 0; i < TMAPAPP_Context.NumSnkASEs;i++)
        {
          if ((p_conn->pASEs->aSnkASE[i].ID == ASE_ID) && (p_conn->pASEs->aSnkASE[i].allocated == 1u))
          {
            return &p_conn->pASEs->aSnkASE[i];
          }
        }
      }
#endif /*(MAX_NUM_USR_SNK_ASE > 0)*/
#if (MAX_NUM_USR_SRC_ASE > 0)
      if (TMAPAPP_Context.NumSrcASEs > 0u)
      {
        for ( i = 0; i < TMAPAPP_Context.NumSrcASEs;i++)
        {
          if ((p_conn->pASEs->aSrcASE[i].ID == ASE_ID) && (p_conn->pASEs->aSrcASE[i].allocated == 1u))
          {
            return &p_conn->pASEs->aSrcASE[i];
          }
        }
      }
#endif /*(MAX_NUM_USR_SRC_ASE > 0)*/
    }
  }
  return 0;
}

static uint8_t APP_SetupAudioDataPath(uint16_t ACL_ConnHandle,
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
                   (uint8_t *)aSrcBuff);
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

    if (role == AUDIO_ROLE_SOURCE)
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

#if (APP_CCP_ROLE_CLIENT_SUPPORT == 1u)
/**
  * @brief  Notify CCP Meta Events
  * @param  pNotification: pointer on notification information
 */
static void CCP_MetaEvt_Notification(CCP_Notification_Evt_t *pNotification)
{

  switch(pNotification->EvtOpcode)
  {
    case CCP_CLT_CALL_STATE_EVT:
    {
      CCP_CLT_Call_State_Params_Evt_t *cs_params = (CCP_CLT_Call_State_Params_Evt_t *)pNotification->pInfo;
      switch(cs_params->State)
      {
        case CCP_CS_IDLE:
          LOG_INFO_APP("Call %d is terminated on Telephone Bearer ID %d\n",
                      cs_params->CallIdx,
                      pNotification->ContentControlID);
          Menu_SetCallState("IDLE");
        break;

        case CCP_CS_INCOMING:
          LOG_INFO_APP("Call %d is Incoming : A remote party is calling on Telephone Bearer ID %d\n",
                      cs_params->CallIdx,
                      pNotification->ContentControlID);
          Menu_SetCallState("INCOMING");
        break;

        case CCP_CS_DIALING:
          LOG_INFO_APP("Call %d is dialing on Telephone Bearer ID %d (remote party not already alerted)\n",
                      cs_params->CallIdx,
                      pNotification->ContentControlID);
          Menu_SetCallState("DIALING");
        break;

        case CCP_CS_ALERTING:
          LOG_INFO_APP("remote party is being alerted on connectio handle 0x%04X on Telephone Bearer ID %d\n",
                      pNotification->ConnHandle,
                      pNotification->ContentControlID);
          Menu_SetCallState("ALERTING");
        break;

        case CCP_CS_ACTIVE:
          LOG_INFO_APP("Call %d is active on Telephone Bearer ID %d\n",
                      cs_params->CallIdx,
                      pNotification->ContentControlID);
          Menu_SetCallState("ACTIVE");
        break;

        case CCP_CS_LOCALLY_HELD:
          LOG_INFO_APP("Call %d is connected but held locally on Telephone Bearer ID %d\n",
                      cs_params->CallIdx,
                      pNotification->ContentControlID);
          Menu_SetCallState("HELD");
        break;

        case CCP_CS_REMOTELY_HELD:
          LOG_INFO_APP("Call %d is connected but held remotely on Telephone Bearer ID %d\n",
                      cs_params->CallIdx,
                      pNotification->ContentControlID);
          Menu_SetCallState("HELD");
        break;

        case CCP_CS_LOCALLY_REMOTELY_HELD:
          LOG_INFO_APP("Call %d is connected but held locally and remotely on Telephone Bearer ID %d\n",
                      cs_params->CallIdx,
                      pNotification->ContentControlID);
          Menu_SetCallState("HELD");
        break;
      }
      LOG_INFO_APP("     Call Flags : 0x%02X\n",cs_params->CallFlags);
      break;
    }

    case CCP_CLT_CURRENT_CALL_EVT:
    {
      CCP_CLT_Call_Item_Evt_t *bearer_info = (CCP_CLT_Call_Item_Evt_t *)pNotification->pInfo;
      APP_ACL_Conn_t *p_conn = APP_GetACLConn(pNotification->ConnHandle);
      LOG_INFO_APP("Current Call of Telephone Bearer Instance ID %d on remote CCP Server : \n",
                  pNotification->ContentControlID);
      LOG_INFO_APP("     Call Index : %d\n",bearer_info->CallIndex);
      if (p_conn != 0)
      {
        p_conn->CurrentCallID = bearer_info->CallIndex;
      }
      switch(bearer_info->CallState)
      {
        case CCP_CS_IDLE:
          LOG_INFO_APP("     Call State : IDLE\n");
        break;

        case CCP_CS_INCOMING:
          LOG_INFO_APP("     Call State : INCOMING\n");
        break;

        case CCP_CS_DIALING:
          LOG_INFO_APP("     Call State : DIALING\n");
        break;

        case CCP_CS_ALERTING:
          LOG_INFO_APP("     Call State : ALERTING\n");
        break;

        case CCP_CS_ACTIVE:
          LOG_INFO_APP("     Call State : ACTIVE\n");
        break;

        case CCP_CS_LOCALLY_HELD:
          LOG_INFO_APP("     Call State : LOCALLY HELD\n");
        break;

        case CCP_CS_REMOTELY_HELD:
          LOG_INFO_APP("     Call State : REMOTELY HELD\n");
        break;

        case CCP_CS_LOCALLY_REMOTELY_HELD:
          LOG_INFO_APP("     Call State : LOCALLY AND REMOTELY HELD\n");
        break;
      }
      LOG_INFO_APP("     Call Flags : 0x%02X\n",bearer_info->CallFlags);
      if (bearer_info->CallURILength > 0u)
      {
        LOG_INFO_APP("     Call URI : ");
        Print_String(bearer_info->pCallURI, bearer_info->CallURILength);
        LOG_INFO_APP("\n");
      }
      break;
    }

    case CCP_CLT_GTBS_INFO_EVT:
    {
      CCP_CLT_BearerInfo_Evt_t *bearer_info = (CCP_CLT_BearerInfo_Evt_t *)pNotification->pInfo;
      APP_ACL_Conn_t *p_conn = APP_GetACLConn(pNotification->ConnHandle);
      LOG_INFO_APP("Generic Telephone Bearer with CCID %d on remote CCP Server (connection handle 0x%04X)\n",
                  pNotification->ContentControlID,
                  pNotification->ConnHandle);
      if (p_conn != 0)
      {
        p_conn->GenericTelephoneBearerCCID = pNotification->ContentControlID;
      }
      LOG_INFO_APP("Start ATT Handle : 0x%04X - End ATT Handle : 0x%04X\n",
                      bearer_info->StartAttHandle,
                      bearer_info->EndAttHandle);
      if (bearer_info->FeaturesMask & CCP_FEATURE_BEARER_SIGNAL_STRENGTH)
      {
        LOG_INFO_APP("Bearer Signal Strength ( and associated Signal Strength Reporting Interval) is supported on remote CCP Server\n");
      }
      if (bearer_info->FeaturesMask & CCP_FEATURE_INC_CALL_TARGET_BEARER_URI)
      {
        LOG_INFO_APP("Incoming Call Target Bearer URI is supported on remote CCP Server\n");
      }
      if (bearer_info->FeaturesMask & CCP_FEATURE_CALL_FRIENDLY_NAME)
      {
        LOG_INFO_APP("Call friendly Name is supported on remote CCP Server\n");
      }
      break;
    }

    case CCP_CLT_OPERATION_COMPLETE_EVT:
    {
      LOG_INFO_APP("CCP Client Operation with remote CCP Server on connection handle 0x%04X is complete with status 0x%02X\n",
                  pNotification->ConnHandle,
                  pNotification->Status);
      break;
    }

    case CCP_CLT_OPERATION_TRANSMITTED_EVT:
    {
      CCP_CLT_CallCtrlOpTransmitted_Evt_t *p_event_params = (CCP_CLT_CallCtrlOpTransmitted_Evt_t *) pNotification->pInfo;
      LOG_INFO_APP("CCP Client Call Control Operation 0x02X with remote CCP Server on connection handle 0x%04X is transmitted with status 0x%02X\n",
                  pNotification->ConnHandle,
                  p_event_params->CallCtrlOp,
                  pNotification->Status);
      break;
    }

    case CCP_CLT_PROVIDER_NAME_EVT:
    {
      CCP_CLT_Bearer_Provider_Name_Evt_t *info = (CCP_CLT_Bearer_Provider_Name_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Provider Name of Telephone Bearer Instance ID %d on remote CCP Server (len %d) : ",
                  pNotification->ContentControlID,
                  info->NameLength);
      Print_String(info->pName, info->NameLength);
      LOG_INFO_APP("\n");
      break;
    }

    case CCP_CLT_UCI_EVT:
    {
      CCP_CLT_Bearer_UCI_Evt_t *info = (CCP_CLT_Bearer_UCI_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("UCI of Telephone Bearer Instance ID %d on remote CCP Server (len %d) : ",
                  pNotification->ContentControlID,
                  info->Length);
      Print_String(info->pUCI, info->Length);
      LOG_INFO_APP("\n");
      break;
    }

    case CCP_CLT_TECHNOLOGY_EVT:
    {
      CCP_CLT_Bearer_Technology_Evt_t *info = (CCP_CLT_Bearer_Technology_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Techoplogy of Telephone Bearer Instance ID %d on remote CCP Server : %d\n",
                  pNotification->ContentControlID,
                  info->Techno);
      break;
    }

    case CCP_CLT_URI_SCHEMES_LIST_EVT:
    {
      CCP_CLT_Bearer_URI_Schemes_List_Evt_t *info = (CCP_CLT_Bearer_URI_Schemes_List_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Supported URI Schemes of Telephone Bearer Instance ID %d on remote CCP Server (len %d) : ",
                  pNotification->ContentControlID,
                  info->Length);
      Print_String(info->pURISchemes, info->Length);
      LOG_INFO_APP("\n");
      break;
    }

    case CCP_CLT_SIGNAL_STRENGTH_EVT:
    {
      CCP_CLT_Signal_Strength_Evt_t *info = (CCP_CLT_Signal_Strength_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Signal Strength of Telephone Bearer Instance ID %d on remote CCP Server : %d\n",
                  pNotification->ContentControlID,
                  info->SignalStrength);
      break;
    }

    case CCP_CLT_SSR_INTERVAL_EVT:
    {
      CCP_CLT_SSR_Interval_Evt_t *info = (CCP_CLT_SSR_Interval_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("SSR Interval of Telephone Bearer Instance ID %d on remote CCP Server : %d seconds\n",
                  pNotification->ContentControlID,
                  info->SSR_Interval);
      break;
    }

    case CCP_CLT_NO_CURRENT_CALL_EVT:
    {
      LOG_INFO_APP("No Call in Progress\n");
      break;
    }

    case CCP_CLT_INC_CALL_TARGET_URI_EVT:
    {
      CCP_CLT_Call_URI_Evt_t *info = (CCP_CLT_Call_URI_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Incoming Call Target URI for Call ID %d of Telephone Bearer Instance ID %d on remote CCP Server (len %d) : ",
                  info->CallIndex,
                  pNotification->ContentControlID,
                  info->URILength);
      Print_String(info->pURI, info->URILength);
      LOG_INFO_APP("\n");
      break;
    }

    case CCP_CLT_FEATURES_STATUS_EVT:
    {
      CCP_CLT_Features_Status_Evt_t *info = (CCP_CLT_Features_Status_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Features Status of Telephone Bearer Instance ID %d on remote CCP Server :\n",
                  pNotification->ContentControlID);
      if (info->InbandRingtone == CCP_INBAND_RINGTONE_ENABLED)
      {
        LOG_INFO_APP("   InBand RingTone : Enabled\n");
      }
      else
      {
        LOG_INFO_APP("   InBand RingTone : Disabled\n");
      }
      if (info->SilentMode == CCP_SILENT_MODE_ON)
      {
        LOG_INFO_APP("   Silent Mode : ON\n");
      }
      else
      {
        LOG_INFO_APP("   Silent Mode : OFF\n");
      }
      break;
    }

    case CCP_CLT_CALL_CTRL_OPTIONS_EVT:
    {
      CCP_CLT_Call_Ctrl_Options_Evt_t *info = (CCP_CLT_Call_Ctrl_Options_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Call Control Options of Telephone Bearer Instance ID %d on remote CCP Server :\n",
                  pNotification->ContentControlID);
      if (info->LocalHoldOption == CCP_LOCAL_HOLD_FEATURE_SUPPORTED)
      {
        LOG_INFO_APP("   Local Hold and Local Retrieve Call Control supported\n");
      }
      else
      {
        LOG_INFO_APP("   Local Hold and Local Retrieve Call Control not supported\n");
      }
      if (info->JoinOption == CCP_JOIN_FEATURE_SUPPORTED)
      {
        LOG_INFO_APP("   Join Call Control Point supported\n");
      }
      else
      {
        LOG_INFO_APP("   Join Call Control Point not supported\n");
      }
      break;
    }

    case CCP_CLT_INCOMING_CALL_EVT:
    {
      CCP_CLT_Call_URI_Evt_t *info = (CCP_CLT_Call_URI_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Incoming Caller URI for Call ID %d of Telephone Bearer Instance ID %d on remote CCP Server (len %d) : ",
                  info->CallIndex,
                  pNotification->ContentControlID,
                  info->URILength);
      Print_String(info->pURI, info->URILength);
      LOG_INFO_APP("\n");
      break;
    }

    case CCP_CLT_CALL_FRIENDLY_NAME_EVT:
    {
      CCP_CLT_Call_Name_Evt_t *info = (CCP_CLT_Call_Name_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Friendly Name for Call ID %d of Telephone Bearer Instance ID %d on remote CCP Server (len %d) : ",
                  info->CallIndex,
                  pNotification->ContentControlID,
                  info->NameLength);
      Print_String(info->pName, info->NameLength);
      LOG_INFO_APP("\n");
      break;
    }

    case CCP_CLT_CALL_TERMINATION_EVT:
    {
      CCP_CLT_Call_Termination_Evt_t *info = (CCP_CLT_Call_Termination_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Termination of Call ID %d of Telephone Bearer Instance ID %d on remote CCP Server for Reason: %d\n",
                  info->CallIndex,
                  pNotification->ContentControlID,
                  info->Reason);
      break;
    }

    case CCP_CLT_RESPONSE_EVT:
    {
      CCP_CLT_Call_Ctrl_Rsp_Evt_t *info = (CCP_CLT_Call_Ctrl_Rsp_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("Response for Call Control Operation 0x%02X on Call ID %d of Telephone Bearer Instance ID %d : 0x%02X\n",
                  info->CallCtrlOp,
                  info->CallIndex,
                  pNotification->ContentControlID,
                  info->CallCtrlResp);
      break;
    }

    default:
      break;
  }
}
#endif /*(APP_CCP_ROLE_CLIENT_SUPPORT == 1u)*/

#if (APP_MCP_ROLE_CLIENT_SUPPORT == 1u)
/**
  * @brief  Notify MCP Meta Events
  * @param  pNotification: pointer on notification information
 */
static void MCP_MetaEvt_Notification(MCP_Notification_Evt_t *pNotification)
{
  switch(pNotification->EvtOpcode)
  {
    case MCP_CLT_GMP_INFO_EVT:
    case MCP_CLT_MPI_INFO_EVT:
    {
      MCP_CLT_MediaPlayerInfo_Evt_t *mp_info = (MCP_CLT_MediaPlayerInfo_Evt_t *)pNotification->pInfo;
      if (pNotification->EvtOpcode == MCP_CLT_GMP_INFO_EVT)
      {
        APP_ACL_Conn_t *p_conn = APP_GetACLConn(pNotification->ConnHandle);
        LOG_INFO_APP("Generic Media Player with CCID %d on remote MCP Server (connection handle 0x%04X)\n",
                    pNotification->ContentControlID,
                    pNotification->ConnHandle);
        if (p_conn != 0)
        {
          p_conn->GenericMediaPlayerCCID = pNotification->ContentControlID;
        }
      }
      else
      {
        LOG_INFO_APP("Media Player Instance with CCID %d on remote MCP Server (connection handle 0x%04X)\n",
                    pNotification->ContentControlID,
                    pNotification->ConnHandle);
      }
      if (mp_info->OptionFeaturesMask & MCP_FEATURE_PLAYBACK_SPEED)
      {
        LOG_INFO_APP("Playback Speed feature is supported on remote MCP Server\n");
      }
      if (mp_info->OptionFeaturesMask & MCP_FEATURE_SEEKING_SPEED)
      {
        LOG_INFO_APP("Seeking Speed feature is supported on remote MCP Server\n");
      }
      if (mp_info->OptionFeaturesMask & MCP_FEATURE_PLAYING_ORDER)
      {
        LOG_INFO_APP("Playing Order feature is supported on remote MCP Server\n");
      }
      if (mp_info->OptionFeaturesMask & MCP_FEATURE_PLAYING_ORDERS_SUPPORTED)
      {
        LOG_INFO_APP("Supported Playing Orders feature is supported on remote MCP Server\n");
      }
      if (mp_info->OptionFeaturesMask & MCP_FEATURE_MEDIA_CTRL_OP)
      {
        LOG_INFO_APP("Media Control Operation feature is supported on remote MCP Server\n");
      }
      if (mp_info->NotifUpdateMask & MCP_NOTIFICATION_MEDIA_PLAYER_NAME)
      {
        LOG_INFO_APP("Media Player Name update Notification feature is supported on remote MCP Server\n");
      }
      if (mp_info->NotifUpdateMask & MCP_NOTIFICATION_TRACK_TITLE)
      {
        LOG_INFO_APP("Track Title update Notification feature is supported on remote MCP Server\n");
      }
      if (mp_info->NotifUpdateMask & MCP_NOTIFICATION_TRACK_DURATION)
      {
        LOG_INFO_APP("Track Duration update Notification feature is supported on remote MCP Server\n");
      }
      if (mp_info->NotifUpdateMask & MCP_NOTIFICATION_TRACK_POSITION)
      {
        LOG_INFO_APP("Track Position update Notification feature is supported on remote MCP Server\n");
      }
      if (mp_info->NotifUpdateMask & MCP_NOTIFICATION_PLAYBACK_SPEED)
      {
        LOG_INFO_APP("Playback Speed update Notification feature is supported on remote MCP Server\n");
      }
      if (mp_info->NotifUpdateMask & MCP_NOTIFICATION_SEEKING_SPEED)
      {
        LOG_INFO_APP("Seeking Speed update Notification feature is supported on remote MCP Server\n");
      }
      if (mp_info->NotifUpdateMask & MCP_NOTIFICATION_PLAYING_ORDER)
      {
        LOG_INFO_APP("Playing Order update Notification feature is supported on remote MCP Server\n");
      }
      if (mp_info->NotifUpdateMask & MCP_NOTIFICATION_MEDIA_CTRL_OP_SUPPORTED)
      {
        LOG_INFO_APP("Supported Media Control Operation update Notification feature is supported on remote MCP Server\n");
      }
      LOG_INFO_APP("Start ATT Handle : 0x%04X - End ATT Handle : 0x%04X\n",
                      mp_info->StartAttHandle,
                      mp_info->EndAttHandle);
      break;
    }

    case MCP_CLT_OPERATION_COMPLETE_EVT:
    {
      tBleStatus ret;
      LOG_INFO_APP("[MP %d] MCP Client operation with connection handle 0x%04X is complete with status 0x%02X\n",
                  pNotification->ContentControlID,
                  pNotification->ConnHandle,
                  pNotification->Status);

      APP_ACL_Conn_t *p_conn = APP_GetACLConn(pNotification->ConnHandle);
      if (p_conn != 0)
      {
        if (p_conn->PlayPauseOperation == 1u)
        {
          p_conn->PlayPauseOperation = 0;

          if (p_conn->MediaState == MCP_MEDIA_STATE_INACTIVE || p_conn->MediaState == MCP_MEDIA_STATE_PAUSED)
          {
            ret = MCP_CLIENT_PlayTrack(pNotification->ConnHandle, p_conn->GenericMediaPlayerCCID);
            LOG_INFO_APP("MCP_CLIENT_PlayTrack with status 0x%02X\n", ret);
          }
          else if (p_conn->MediaState == MCP_MEDIA_STATE_PLAYING)
          {
            ret = MCP_CLIENT_PauseTrack(pNotification->ConnHandle, p_conn->GenericMediaPlayerCCID);
            LOG_INFO_APP("MCP_CLIENT_PauseTrack with status 0x%02X\n", ret);
          }
        }
      }
      break;
    }

    case MCP_CLT_OPERATION_TRANSMITTED_EVT:
    {
      MCP_CLT_MediaCtrlOpTransmitted_Evt_t *p_event_params = (MCP_CLT_MediaCtrlOpTransmitted_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("[MP %d] MCP Client operation 0x%02X with connection handle 0x%04X is transmitted with status 0x%02X\n",
                  pNotification->ContentControlID,
                  p_event_params->CtrlOp,
                  pNotification->ConnHandle,
                  pNotification->Status);
      break;
    }

    case MCP_CLT_RESPONSE_EVT:
    {
      MCP_CLT_MediaCtrlRsp_Evt_t *p_event_params = (MCP_CLT_MediaCtrlRsp_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("[MP %d] MCP Client receives response 0x%02X for MCP Ctrl operation 0x%02X (ConnHandle 0x%04X)\n",
                  pNotification->ContentControlID,
                  p_event_params->ResultCode,
                  p_event_params->CtrlOp,
                  pNotification->ConnHandle);
      break;
    }

    case MCP_CLT_MEDIA_PLAYER_NAME_EVT:
    {
      MCP_CLT_MediaPlayerName_Evt_t *p_event_params = (MCP_CLT_MediaPlayerName_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("[MP %d] MCP Client receives Media Player Name ",
                  pNotification->ContentControlID);
      Print_String(p_event_params->pName, p_event_params->NameLength);
      LOG_INFO_APP(" (offset %d, length %d)\n",
                  p_event_params->Offset,
                  p_event_params->NameLength);
      break;
    }

    case MCP_CLT_TRACK_CHANGED_EVT:
    {
      LOG_INFO_APP("[MP %d] MCP Client is notified that Track has changed in MCP Server (ConnHandle 0x%04X)\n",
                  pNotification->ContentControlID,
                  pNotification->ConnHandle);
      break;
    }

    case MCP_CLT_TRACK_TITLE_EVT:
    {
      MCP_CLT_TrackTitle_Evt_t *p_event_params = (MCP_CLT_TrackTitle_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("[MP %d] MCP Client receives Track Title ",
                  pNotification->ContentControlID);
      Print_String(p_event_params->pTitle, p_event_params->TitleLength);
      LOG_INFO_APP(" (offset %d, length %d)\n",
                  p_event_params->Offset,
                  p_event_params->TitleLength);
      Menu_SetTrackTitle(p_event_params->pTitle, p_event_params->TitleLength);
      break;
    }

    case MCP_CLT_TRACK_DURATION_EVT:
    {
      MCP_CLT_TrackDuration_Evt_t *p_event_params = (MCP_CLT_TrackDuration_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("[MP %d] MCP Client receives Track Duration %d\n",
                  pNotification->ContentControlID,
                  p_event_params->Duration);
      break;
    }

    case MCP_CLT_TRACK_POSITION_EVT:
    {
      MCP_CLT_TrackPosition_Evt_t *p_event_params = (MCP_CLT_TrackPosition_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("[MP %d] MCP Client receives Track Position %d\n",
                  pNotification->ContentControlID,
                  p_event_params->Position);
      break;
    }

    case MCP_CLT_PLAYBACK_SPEED_EVT:
    {
        MCP_CLT_PlaybackSpeed_Evt_t *p_event_params = (MCP_CLT_PlaybackSpeed_Evt_t *)pNotification->pInfo;
        uint16_t speed_rate = ((uint32_t)(p_event_params->Speed) * 100);
        LOG_INFO_APP("[MP %d] MCP Client receives Playback Speed : %f (%d %) of normal playback speed\n",
                    pNotification->ContentControlID,
                    p_event_params->Speed,
                    speed_rate);
      break;
    }

    case MCP_CLT_SEEKING_SPEED_EVT:
    {
      MCP_CLT_SeekingSpeed_Evt_t *p_event_params = (MCP_CLT_SeekingSpeed_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("[MP %d] MCP Client receives Seeking Speed : %d of real-time playback\n",
                  pNotification->ContentControlID,
                  p_event_params->Speed);
      break;
    }

    case MCP_CLT_PLAYING_ORDER_EVT:
    {
      MCP_CLT_PlayingOrder_Evt_t *p_event_params = (MCP_CLT_PlayingOrder_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("[MP %d] MCP Client receives Playing Order type 0x%02X from remote MCP Server\n",
                  pNotification->ContentControlID,
                  p_event_params->PlayingOrder);
      break;
    }

    case MCP_CLT_PLAYING_ORDERS_SUPPORTED_EVT:
    {
      MCP_CLT_SupportedPlayingOrders_Evt_t *p_event_params = (MCP_CLT_SupportedPlayingOrders_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("[MP %d] MCP Client receives Playing Orders mask 0x%04X from remote MCP Server\n",
                  pNotification->ContentControlID,
                  p_event_params->PlayingOrdersMask);
      break;
    }

    case MCP_MEDIA_STATE_EVT:
    {
      MCP_MediaState_Evt_t *p_event_params = (MCP_MediaState_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("[MP %d] MCP Client receives Media State 0x%02X from remote MCP Server\n",
                  pNotification->ContentControlID,
                  p_event_params->MediaState);

      APP_ACL_Conn_t *p_conn = APP_GetACLConn(pNotification->ConnHandle);
      if (p_conn != 0)
      {
        p_conn->MediaState = p_event_params->MediaState;
      }

      switch (p_event_params->MediaState)
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

      break;
    }

    case MCP_CLT_MEDIA_CTRL_OP_FEATURES_EVT:
    {
      MCP_CLT_MediaCtrlOpFeatures_Evt_t *p_event_params = (MCP_CLT_MediaCtrlOpFeatures_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("[MP %d] MCP Client receives Supported Media Ctrl Operation mask 0x%08X from remote MCP Server\n",
                  pNotification->ContentControlID,
                  p_event_params->CtrlOpFeatureMask);
      break;
    }

    default:
      break;
  }
}
#endif /*(APP_MCP_ROLE_CLIENT_SUPPORT == 1u)*/

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
        Menu_SetVolume(p_info->VolSetting);
        Set_Volume(p_info->VolSetting);
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
      break;
    }

    case VCP_RENDERER_UPDATED_AUDIO_OUTPUT_LOCATION_EVT:
    {
      VCP_AudioLocation_Evt_t *p_info = (VCP_AudioLocation_Evt_t *)pNotification->pInfo;
      LOG_INFO_APP("[VOC %d] Updated Audio Location : 0x%08X\n",p_info->VOCInst,p_info->AudioLocation);
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

/* USER CODE BEGIN FD */
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
/* USER CODE END FD */

