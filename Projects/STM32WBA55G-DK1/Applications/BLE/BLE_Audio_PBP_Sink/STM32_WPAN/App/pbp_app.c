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

/* Private defines -----------------------------------------------------------*/
#define CODEC_POOL_SUB_SIZE                     (960u)

/* double buffer, stereo :
 *?480 (max LC3 frame len (48KHz, 10ms)) x 2 (Max Channels Number (stereo)) x 2 (double buffer configuration)
 */
#define PLAY_BUFF_SIZE                          (480*2*2)

#define SOURCE_ID_LIST_SIZE                     (3u)

#define SCAN_INTERVAL                           (0x40) /* Scan Interval (*0.625ms): 40ms */
#define SCAN_WINDOW                             (0x20) /* Scan Window (*0.625ms): 20ms */
#define PA_EVENT_SKIP                           (0u)
#define PA_SYNC_TIMEOUT                         (0x03E8)
#define BROADCAST_CONTROLLER_DELAY              (22000u)
#define BAP_BROADCAST_ENCRYPTION                (0u)
#define BIG_HANDLE                              (0u)
#define BIG_MSE                                 (0u)
#define BIG_SYNC_TIMEOUT                        (0x0190)

#define BLE_AUDIO_DYN_ALLOC_SIZE                (BLE_AUDIO_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK))

/*Memory required for CAP Acceptor Context*/
#define CAP_ACCEPTOR_DYN_ALLOC_SIZE \
        CAP_ACCEPTOR_MEM_TOTAL_BUFFER_SIZE(MAX_CAP_ACCEPTOR_INSTANCES, \
                                           0u, \
                                           0, \
                                           0u, \
                                           0, \
                                           MAX_NUM_USR_SNK_ASE, \
                                           MAX_NUM_USR_SRC_ASE)
/*Memory required for CAP*/
#define CAP_DYN_ALLOC_SIZE      (CAP_ACCEPTOR_DYN_ALLOC_SIZE)

#define BAP_PACS_SRV_DYN_ALLOC_SIZE \
        BAP_PACS_SRV_TOTAL_BUFFER_SIZE(MAX_NUM_PAC_SNK_RECORDS,MAX_NUM_PAC_SRC_RECORDS)

/* Memory required to allocate resource for Broadcast Audio Scan for Scan Delegator*/
#define BAP_BASS_SRV_DYN_ALLOC_SIZE  \
                BAP_BASS_SRV_TOTAL_BUFFER_SIZE(MAX_NUM_SDE_BSRC_INFO,\
                                              MAX_NUM_BIS_PER_BIG,\
                                              MAX_BASS_CODEC_CONFIG_SIZE,\
                                              MAX_BASS_METADATA_SIZE,\
                                              MAX_NUM_BASS_BASE_SUBGROUPS)
/* Memory required to allocate resource for Non-Volatile Memory Management for BAP Services restoration*/
#define BAP_NVM_MGMT_DYN_ALLOC_SIZE  \
              BAP_NVM_MGMT_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK)

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
static uint32_t aPACSSrvMemBuffer[DIVC(BAP_PACS_SRV_DYN_ALLOC_SIZE,4)];
static uint32_t aNvmMgmtMemBuffer[DIVC(BAP_NVM_MGMT_DYN_ALLOC_SIZE,4)];
static uint32_t aBASSSrvMemBuffer[DIVC(BAP_BASS_SRV_DYN_ALLOC_SIZE,4)];
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

uint8_t aCodecPacketsMemory[MAX_PATH_NB][CODEC_POOL_SUB_SIZE];

uint16_t aRecBuff[PLAY_BUFF_SIZE] __attribute__((aligned));


#if (BAP_BROADCAST_ENCRYPTION == 1)
uint32_t aPBPAPP_BroadcastCode[4u] = {0x00000001, 0x00000002, 0x00000003, 0x00000004};
#else /*(BAP_BROADCAST_ENCRYPTION == 1)*/
uint32_t aPBPAPP_BroadcastCode[4u] = {0x00000000, 0x00000000, 0x00000000, 0x00000000};
#endif /*(BAP_BROADCAST_ENCRYPTION == 1)*/

uint32_t aSourceIdList[SOURCE_ID_LIST_SIZE] = {0x000001, 0x000002, 0x000003};
uint8_t CurrentID = 0u;

uint32_t PreferredControllerDelay = 12000u;

/* Private functions prototypes-----------------------------------------------*/
static tBleStatus PBPAPP_Init(CAP_Role_t CAP_Role, BAP_Role_t BAP_Role);
static void CAP_App_Notification(CAP_Notification_Evt_t *pNotification);
static uint8_t PBPAPP_BroadcastSetupAudio(Audio_Role_t role);
static uint8_t PBPAPP_StartBroadcastAudio(Audio_Role_t role);
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
    case PBP_BROADCAST_AUDIO_UP_EVT:
    {
      APP_DBG_MSG(">>== PBP_BROADCAST_AUDIO_UP_EVT\n");
    }
    break;
    case PBP_BROADCAST_AUDIO_DOWN_EVT:
    {
      uint8_t status;
      APP_DBG_MSG(">>== PBP_BROADCAST_AUDIO_DOWN_EVT\n");
      MX_AudioDeInit();

      if (PBPAPP_Context.PASyncState != PBPAPP_PA_SYNC_STATE_IDLE)
      {
        /* Stop Periodic Advertising Sync */
        status = PBP_PBK_StopPASync(PBPAPP_Context.PASyncHandle);
        if (status != BLE_STATUS_SUCCESS)
        {
          APP_DBG_MSG("  Fail   : PBP_PBK_StopPASync() function, result: 0x%02X\n", status);
        }
        else
        {
          APP_DBG_MSG("  Success: PBP_PBK_StopPASync() function\n");
          PBPAPP_Context.PASyncState = PBPAPP_PA_SYNC_STATE_IDLE;
        }
      }

      if (PBPAPP_Context.ScanState == PBPAPP_SCAN_STATE_IDLE)
      {
        /* Start Scan */
        status = CAP_Broadcast_StartAdvReportParsing();
        if (status != BLE_STATUS_SUCCESS)
        {
          APP_DBG_MSG("  Fail   : CAP_Broadcast_StartAdvReportParsing() function, result: 0x%02X\n", status);
        }
        else
        {
          APP_DBG_MSG("  Success: CAP_Broadcast_StartAdvReportParsing() function\n");
        }

        if (status == BLE_STATUS_SUCCESS)
        {
          /* Starts an Observation procedure */
          status = aci_gap_start_observation_proc(SCAN_INTERVAL,
                                                  SCAN_WINDOW,
                                                  0x00,              /* LE_Scan_Type */
                                                  0x00,              /* Address type: Public */
                                                  0x00,              /* Filter duplicates: No */
                                                  0x00 );            /* Filter policy: Accept all */
          if (status != BLE_STATUS_SUCCESS)
          {
            APP_DBG_MSG("  Fail   : aci_gap_start_observation_proc() function, result: 0x%02X\n", status);
          }
          else
          {
            APP_DBG_MSG("  Success: aci_gap_start_observation_proc() function\n");
          PBPAPP_Context.ScanState = PBPAPP_SCAN_STATE_SCANNING;
          }
        }
      }
    }
    break;

    case PBP_PBK_BROADCAST_SOURCE_ADV_REPORT_EVT:
    {
      PBP_Broadcast_Source_Adv_Report_Data_t *data = (PBP_Broadcast_Source_Adv_Report_Data_t*) pNotification->pInfo;
      APP_DBG_MSG(">>== PBP_PBK_BROADCAST_SOURCE_ADV_REPORT_EVT\n");
      APP_DBG_MSG("     - Broadcasting Device with address %02X:%02X:%02X:%02X:%02X:%02X\n",
                  data->pBAPReport->pAdvAddress[5],
                  data->pBAPReport->pAdvAddress[4],
                  data->pBAPReport->pAdvAddress[3],
                  data->pBAPReport->pAdvAddress[2],
                  data->pBAPReport->pAdvAddress[1],
                  data->pBAPReport->pAdvAddress[0]);
      if(PBPAPP_Context.PASyncState == PBPAPP_PA_SYNC_STATE_IDLE
         && PBPAPP_Context.BIGSyncState == PBPAPP_BIG_SYNC_STATE_IDLE)
      {
        uint8_t index = 0;
        while (index + 8 < data->pBAPReport->AdvertisingDataLength)
        {
          if (data->pBAPReport->pAdvertisingData[index+1] == 0xFF)
          {
            /* Found Manufacturer Data */
            if (data->pBAPReport->pAdvertisingData[index+7] == (aSourceIdList[CurrentID] & 0xFF)
                && data->pBAPReport->pAdvertisingData[index+8] == ((aSourceIdList[CurrentID] >> 8) & 0xFF)
                  && data->pBAPReport->pAdvertisingData[index+9] == ((aSourceIdList[CurrentID] >> 16) & 0xFF))
            {
              /* Matching Broadcast Source ID */
              uint8_t status;
              PBPAPP_Context.PASyncState = PBPAPP_PA_SYNC_STATE_SYNCHRONIZING;
              status = PBP_PBK_StartPASync(data->pBAPReport->AdvSID,
                                           data->pBAPReport->pAdvAddress,
                                           data->pBAPReport->AdvAddressType,
                                           PA_EVENT_SKIP,
                                           PA_SYNC_TIMEOUT);
              APP_DBG_MSG("==>> Starting Broadcasting DiscoveryDevice with Source %02X:%02X:%02X:%02X:%02X:%02X\n",
                                data->pBAPReport->pAdvAddress[5],
                                data->pBAPReport->pAdvAddress[4],
                                data->pBAPReport->pAdvAddress[3],
                                data->pBAPReport->pAdvAddress[2],
                                data->pBAPReport->pAdvAddress[1],
                                data->pBAPReport->pAdvAddress[0]);
              if (status != BLE_STATUS_SUCCESS)
              {
                APP_DBG_MSG("  Fail   : PBP_PBK_StartPASync() function, result: 0x%02X\n", status);
              }
              else
              {
                APP_DBG_MSG("  Success: PBP_PBK_StartPASync() function\n");
              }
              break;
            }
            else
            {
              /* Search next */
              index += data->pBAPReport->pAdvertisingData[index] + 1;
            }
          }
          else
          {
            /* Search next */
            index += data->pBAPReport->pAdvertisingData[index] + 1;
          }
        }
      }
    }
    break;

    case PBP_PBK_PA_SYNC_ESTABLISHED_EVT:
    {
      APP_DBG_MSG(">>== PBP_PBK_PA_SYNC_ESTABLISHED_EVT\n");
      BAP_PA_Sync_Established_Data_t *data = (BAP_PA_Sync_Established_Data_t*) pNotification->pInfo;
      APP_DBG_MSG("     - SyncHandle : 0x%02x\n",data->SyncHandle);
      PBPAPP_Context.PASyncHandle = data->SyncHandle;
      PBPAPP_Context.PASyncState = PBPAPP_PA_SYNC_STATE_SYNCHRONIZED;
    }
    break;

    case PBP_PBK_PA_SYNC_LOST_EVT:
      APP_DBG_MSG(">>== PBP_PBK_PA_SYNC_LOST_EVT\n");
      PBPAPP_Context.PASyncState = PBPAPP_PA_SYNC_STATE_IDLE;
    break;

    case PBP_PBK_BASE_REPORT_EVT:
      {
        BAP_BASE_Report_Data_t *base_data;
        uint8_t status;
        uint8_t index;
        uint8_t i;
        uint8_t j;
        uint8_t k;
        uint8_t l;
        APP_DBG_MSG(">>== PBP_PBK_BASE_REPORT_EVT\n");

        base_data = (BAP_BASE_Report_Data_t*) pNotification->pInfo;

        status = CAP_Broadcast_ParseBASEGroup(base_data->pBasePayload,
                                              base_data->BasePayloadLength,
                                              &(PBPAPP_Context.base_group),
                                              &(index));

        PBPAPP_Context.base_group.pSubgroups = &(PBPAPP_Context.base_subgroups[0]);
        PBPAPP_Context.base_subgroups[0].pCodecSpecificConf = &(PBPAPP_Context.codec_specific_config_subgroup[0][0]);
        PBPAPP_Context.base_subgroups[0].pMetadata = &(PBPAPP_Context.subgroup_metadata[0][0]);
        PBPAPP_Context.base_subgroups[1].pCodecSpecificConf = &(PBPAPP_Context.codec_specific_config_subgroup[1][0]);
        PBPAPP_Context.base_subgroups[1].pMetadata = &(PBPAPP_Context.subgroup_metadata[1][0]);


        PBPAPP_Context.base_bis[0].pCodecSpecificConf = &(PBPAPP_Context.codec_specific_config_bis[0][0]);
        PBPAPP_Context.base_bis[1].pCodecSpecificConf = &(PBPAPP_Context.codec_specific_config_bis[1][0]);

        if(status == BLE_STATUS_SUCCESS)
        {
          APP_DBG_MSG("==>> Start BAP BSNK Parse BASE Group INFO\n");
          APP_DBG_MSG("   Payload Len role : 0x%02x\n",base_data->BasePayloadLength);
          APP_DBG_MSG("   Presentation_delay: 0x%08x\n",PBPAPP_Context.base_group.PresentationDelay);
          APP_DBG_MSG("   Num_subgroups : 0x%02x\n",PBPAPP_Context.base_group.NumSubgroups);
          base_data->pBasePayload += index;
          base_data->BasePayloadLength -= index;

          /* Parse Subgroups */
          for (i = 0; i < PBPAPP_Context.base_group.NumSubgroups && status == BLE_STATUS_SUCCESS; i++)
          {
            status = CAP_Broadcast_ParseBASESubgroup(base_data->pBasePayload,
                                                     base_data->BasePayloadLength,
                                                     &(PBPAPP_Context.base_subgroups[i]),
                                                     &(index));
            base_data->pBasePayload += index;
            base_data->BasePayloadLength -= index;

            PBPAPP_Context.base_subgroups[i].pBIS = &(PBPAPP_Context.base_bis[i]);
            APP_DBG_MSG("    BAP_BSNK_ParseBASESubgroup INFO Number :%d\n", i);
            APP_DBG_MSG("    Codec ID : 0x%08x\n",PBPAPP_Context.base_subgroups[i].CodecID);
            APP_DBG_MSG("    Codec specific config length : %d bytes\n",
                        PBPAPP_Context.base_subgroups[i].CodecSpecificConfLength);
            if (PBPAPP_Context.base_subgroups[i].CodecSpecificConfLength > 0u)
            {
              for (k = 0;k<PBPAPP_Context.base_subgroups[i].CodecSpecificConfLength;k++)
              {
                if (PBPAPP_Context.base_subgroups[i].pCodecSpecificConf[k] > 0u)
                {
                  APP_DBG_MSG("      Length: 0x%02x\n",PBPAPP_Context.base_subgroups[i].pCodecSpecificConf[k]);
                  APP_DBG_MSG("        Type: 0x%02x\n",PBPAPP_Context.base_subgroups[i].pCodecSpecificConf[k+1u]);
                  APP_DBG_MSG("        Value: 0x");
                  for (l = 0 ;l<(PBPAPP_Context.base_subgroups[i].pCodecSpecificConf[k]-1);l++)
                  {
                    APP_DBG_MSG("%02x",PBPAPP_Context.base_subgroups[i].pCodecSpecificConf[k+2u+l]);
                  }
                  APP_DBG_MSG("\n");
                }
                k+=PBPAPP_Context.base_subgroups[i].pCodecSpecificConf[k];
              }
            }
            APP_DBG_MSG("    Metadata length : %d bytes\n",PBPAPP_Context.base_subgroups[i].MetadataLength);
            if (PBPAPP_Context.base_subgroups[i].MetadataLength > 0)
            {
              for (k = 0;k<PBPAPP_Context.base_subgroups[i].MetadataLength;k++)
              {
                if (PBPAPP_Context.base_subgroups[i].pMetadata[k] > 0u)
                {
                  APP_DBG_MSG("      Length: 0x%02x\n",PBPAPP_Context.base_subgroups[i].pMetadata[k]);
                  APP_DBG_MSG("        Type: 0x%02x\n",PBPAPP_Context.base_subgroups[i].pMetadata[k+1u]);
                  APP_DBG_MSG("        Value: 0x");
                  for (l = 0 ;l<(PBPAPP_Context.base_subgroups[i].pMetadata[k]-1);l++)
                  {
                    APP_DBG_MSG("%02x",PBPAPP_Context.base_subgroups[i].pMetadata[k+2u+l]);
                  }
                  APP_DBG_MSG("\n");
                }
                k+=PBPAPP_Context.base_subgroups[i].pMetadata[k];
              }
            }
            APP_DBG_MSG("    Num_BIS : %d\n",PBPAPP_Context.base_subgroups[i].NumBISes);

            /* Parse BIS */
            for (j = 0; (j < PBPAPP_Context.base_subgroups[i].NumBISes) && (status == BLE_STATUS_SUCCESS); j++)
            {
              status = CAP_Broadcast_ParseBASEBIS(base_data->pBasePayload, base_data->BasePayloadLength, &(PBPAPP_Context.base_bis[j]),
                                                  &(index));
              base_data->pBasePayload += index;
              base_data->BasePayloadLength -= index;
              APP_DBG_MSG("      BIS INDEX : 0x%02x\n",PBPAPP_Context.base_bis[j].BIS_Index);
              APP_DBG_MSG("      Codec specific config length : %d bytes\n",PBPAPP_Context.base_bis[j].CodecSpecificConfLength);
              if (PBPAPP_Context.base_bis[j].CodecSpecificConfLength > 0u)
              {
                for (int k = 0;k<PBPAPP_Context.base_bis[j].CodecSpecificConfLength;k++)
                {
                  if (PBPAPP_Context.base_bis[j].pCodecSpecificConf[k] > 0u)
                  {
                    APP_DBG_MSG("        Length: 0x%02x\n",PBPAPP_Context.base_bis[j].pCodecSpecificConf[k]);
                    APP_DBG_MSG("          Type: 0x%02x\n",PBPAPP_Context.base_bis[j].pCodecSpecificConf[k+1u]);
                    APP_DBG_MSG("          Value: 0x");
                    for (int l = 0 ;l<(PBPAPP_Context.base_bis[j].pCodecSpecificConf[k]-1);l++)
                    {
                      APP_DBG_MSG("%02x",PBPAPP_Context.base_bis[j].pCodecSpecificConf[k+2u+l]);
                    }
                    APP_DBG_MSG("\n");
                  }
                  k+=PBPAPP_Context.base_bis[j].pCodecSpecificConf[k];
                }
              }
            }
          }
          APP_DBG_MSG("==>> End Start BAP BSNK Parse BASE Group INFO\n");

          if(PBPAPP_Context.BIGSyncState == PBPAPP_BIG_SYNC_STATE_IDLE)
          {
            memcpy(&PBPAPP_Context.codec_specific_config_subgroup[0],
                   PBPAPP_Context.base_subgroups[0].pCodecSpecificConf,
                   PBPAPP_Context.base_subgroups[0].CodecSpecificConfLength);
            PBPAPP_Context.base_group.PresentationDelay = PBPAPP_Context.base_group.PresentationDelay;

            uint32_t freq = LTV_GetConfiguredSamplingFrequency(PBPAPP_Context.base_subgroups[0].pCodecSpecificConf,
                                                               PBPAPP_Context.base_subgroups[0].CodecSpecificConfLength);

            APP_DBG_MSG("==>> Audio Clock with Sample Frequency Type 0x%02X Initialization\n",freq);
            AudioClock_Init(freq);
          }
        }
      break;
      }

    case PBP_PBK_BIGINFO_REPORT_EVT:
    {
      if (PBPAPP_Context.BIGSyncState == PBPAPP_BIG_SYNC_STATE_IDLE)
      {
        uint8_t status;
        BAP_BIGInfo_Report_Data_t *data = (BAP_BIGInfo_Report_Data_t *) pNotification->pInfo;
        uint8_t bis_index[2u] = {0x01, 0x02};

        status = PBP_PBK_StartBIGSync(BIG_HANDLE,
                                      data->SyncHandle,
                                      &(bis_index[0]),
                                      data->NumBIS,
                                      &(PBPAPP_Context.base_group),
                                      0u,
                                      BAP_BROADCAST_ENCRYPTION,
                                      aPBPAPP_BroadcastCode,
                                      BIG_MSE,
                                      BIG_SYNC_TIMEOUT);
        if (status != BLE_STATUS_SUCCESS)
        {
          APP_DBG_MSG("  Fail   : PBP_PBK_StartBIGSync() function, result: 0x%02X\n", status);
        }
        else
        {
          APP_DBG_MSG("  Success: PBP_PBK_StartBIGSync() function\n");
          PBPAPP_Context.BIGSyncState = PBPAPP_BIG_SYNC_STATE_SYNCHRONIZING;
        }
      }
      break;
    }

    case PBP_PBK_BIG_SYNC_ESTABLISHED_EVT:
      {
        tBleStatus ret;
        BAP_BIG_Sync_Established_Data_t *data = (BAP_BIG_Sync_Established_Data_t*) pNotification->pInfo;
        APP_DBG_MSG(">>== PBP_PBK_BIG_SYNC_ESTABLISHED_EVT\n");
        APP_DBG_MSG("     - BIG_Handle = 0x%02x\n",data->BIGHandle);
        APP_DBG_MSG("     - Num BISes = %d\n",data->NumBISes);

        UTIL_MEM_cpy_8(&(PBPAPP_Context.current_BIS_conn_handles[0]),
                       data->pConnHandle,
                       (data->NumBISes * sizeof(uint16_t)));
        PBPAPP_Context.current_num_bis = data->NumBISes;
        PBPAPP_Context.BIGSyncState = PBPAPP_BIG_SYNC_STATE_SYNCHRONIZED;

        ret = CAP_Broadcast_StopAdvReportParsing();
        if (ret != BLE_STATUS_SUCCESS)
        {
          APP_DBG_MSG("  Fail   : CAP_Broadcast_StopAdvReportParsing() function, result: 0x%02X\n", ret);
        }
        else
        {
          APP_DBG_MSG("  Success: CAP_Broadcast_StopAdvReportParsing() function\n");
        }
        ret = aci_gap_terminate_gap_proc(GAP_OBSERVATION_PROC);
        if (ret != BLE_STATUS_SUCCESS)
        {
          APP_DBG_MSG("  Fail   : aci_gap_terminate_gap_proc() function, result: 0x%02X\n", ret);
        }
        else
        {
          PBPAPP_Context.ScanState = PBPAPP_SCAN_STATE_IDLE;
          APP_DBG_MSG("  Success: aci_gap_terminate_gap_proc() function\n");
        }
        ret = PBP_PBK_StopPASync(PBPAPP_Context.PASyncHandle);
        if (ret != BLE_STATUS_SUCCESS)
        {
          APP_DBG_MSG("  Fail   : PBP_PBK_StopPASync() function, result: 0x%02X\n", ret);
        }
        else
        {
          APP_DBG_MSG("  Success: PBP_PBK_StopPASync() function\n");
          PBPAPP_Context.PASyncState = PBPAPP_PA_SYNC_STATE_IDLE;
        }

        PBPAPP_BroadcastSetupAudio(AUDIO_ROLE_SINK);
      }
      break;

    case PBP_PBK_BIG_SYNC_LOST_EVT:
      {
        APP_DBG_MSG(">>== PBP_PBK_BIG_SYNC_LOST_EVT\n");
        PBPAPP_Context.BIGSyncState = PBPAPP_BIG_SYNC_STATE_IDLE;
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

uint8_t PBPAPP_InitSink(void)
{
  uint8_t ret = 0;

  PBPAPP_Context.phy = 0x02;
#if (SETUP_SPEAKER == 1u)
  PBPAPP_Context.audio_driver_config = AUDIO_DRIVER_CONFIG_SPEAKER;
#else
  PBPAPP_Context.audio_driver_config = AUDIO_DRIVER_CONFIG_LINEIN;
#endif /*SETUP_SPEAKER == 1u*/
  PreferredControllerDelay = BROADCAST_CONTROLLER_DELAY;

  APP_DBG_MSG(">>==  Start CAP Acceptor initialization\n");
  ret = PBPAPP_Init(CAP_ROLE_ACCEPTOR, (BAP_ROLE_BROADCAST_SINK | BAP_ROLE_SCAN_DELEGATOR));
    if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : PBPAPP_Init() function, result: 0x%02X\n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: PBPAPP_Init() function\n");
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
    APP_DBG_MSG("  Fail   : CAP_EnableAudioCodecController() function, result: 0x%02X\n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: CAP_EnableAudioCodecController() function\n");
  }

  PBPAPP_Context.audio_driver_config = AUDIO_DRIVER_CONFIG_HEADSET;

  ret = PBP_Init(PBP_ROLE_PUBLIC_BROADCAST_SINK);
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : PBP_Init() function, result: 0x%02X\n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: PBP_Init() function\n");
  }
  APP_DBG_MSG(">>==  End CAP Acceptor initialization\n");
  return ret;
}

uint8_t PBPAPP_StartSink(void)
{
  uint8_t ret;

  APP_DBG_MSG(">>==  Start Sink\n");

  ret = CAP_Broadcast_StartAdvReportParsing();
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : CAP_Broadcast_StartAdvReportParsing() function, result: 0x%02X\n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: CAP_Broadcast_StartAdvReportParsing() function\n");
  }

  if (ret == BLE_STATUS_SUCCESS)
  {
    /* Starts an Observation procedure */
    ret = aci_gap_start_observation_proc(SCAN_INTERVAL,
                                         SCAN_WINDOW,
                                         0x00,                        /* LE_Scan_Type */
                                         0x00,                        /* Address type: Public */
                                         0x00,                        /* Filter duplicates: No */
                                         0x00 );                      /* Filter policy: Accept all */
    if (ret != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG("  Fail   : aci_gap_start_observation_proc() function, result: 0x%02X\n", ret);
    }
    else
    {
      APP_DBG_MSG("  Success: aci_gap_start_observation_proc() function\n");
      PBPAPP_Context.ScanState = PBPAPP_SCAN_STATE_SCANNING;
    }
  }

  return ret;
}

uint8_t PBPAPP_StopSink(void)
{
  uint8_t ret;

  APP_DBG_MSG(">>==  Start Stop Broadcast Sink\n");

  if (PBPAPP_Context.ScanState == PBPAPP_SCAN_STATE_SCANNING)
  {
    ret = aci_gap_terminate_gap_proc(GAP_OBSERVATION_PROC);
    if (ret != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG("  Fail   : aci_gap_terminate_gap_proc() function, result: 0x%02X\n", ret);
    }
    else
    {
      PBPAPP_Context.ScanState = PBPAPP_SCAN_STATE_IDLE;
      APP_DBG_MSG("  Success: aci_gap_terminate_gap_proc() function\n");
    }
  }

  if (PBPAPP_Context.PASyncState != PBPAPP_PA_SYNC_STATE_IDLE)
  {
    ret = PBP_PBK_StopPASync(PBPAPP_Context.PASyncHandle);
    if (ret != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG("  Fail   : PBP_PBK_StopPASync() function, result: 0x%02X\n", ret);
    }
    else
    {
      APP_DBG_MSG("  Success: PBP_PBK_StopPASync() function\n");
      PBPAPP_Context.PASyncState = PBPAPP_PA_SYNC_STATE_IDLE;
    }
  }

  if (PBPAPP_Context.PASyncState != PBPAPP_PA_SYNC_STATE_IDLE)
  {
    ret = PBP_PBK_StopPASync(PBPAPP_Context.PASyncHandle);
    if (ret != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG("  Fail   : PBP_PBK_StopPASync() function, result: 0x%02X\n", ret);
    }
    else
    {
      APP_DBG_MSG("  Success: PBP_PBK_StopPASync() function\n");
      PBPAPP_Context.PASyncState = PBPAPP_PA_SYNC_STATE_IDLE;
    }
  }

  if (PBPAPP_Context.BIGSyncState != PBPAPP_BIG_SYNC_STATE_IDLE)
  {
    ret = PBP_PBK_StopBIGSync(BIG_HANDLE);
    if (ret != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG("  Fail   : BP_PBK_StopBIGSync() function, result: 0x%02X\n", ret);
    }
    else
    {
      APP_DBG_MSG("  Success: BP_PBK_StopBIGSync() function\n");
      PBPAPP_Context.BIGSyncState = PBPAPP_BIG_SYNC_STATE_IDLE;
    }
  }

  APP_DBG_MSG("   >>==  MX_AudioDeInit()\n");
  MX_AudioDeInit();

  APP_DBG_MSG(">>==  End Stop Broadcast Sink\n");

  return ret;
}

void PBPAPP_SwitchBrdSource(uint8_t next, uint32_t *pSourceID)
{
  uint8_t ret;
  APP_DBG_MSG(">>==  Start Switch Broadcast Source\n");

  if (next == 1)
  {
    /* Get next ID */
    CurrentID = (CurrentID + 1) % SOURCE_ID_LIST_SIZE;
  }
  else
  {
    /* Get previous ID */
    CurrentID = (CurrentID + SOURCE_ID_LIST_SIZE - 1) % SOURCE_ID_LIST_SIZE;
  }

  if (PBPAPP_Context.PASyncState != PBPAPP_PA_SYNC_STATE_IDLE)
  {
    ret = PBP_PBK_StopPASync(PBPAPP_Context.PASyncHandle);
    if (ret != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG("  Fail   : PBP_PBK_StopPASync() function, result: 0x%02X\n", ret);
    }
    else
    {
      APP_DBG_MSG("  Success: PBP_PBK_StopPASync() function\n");
      PBPAPP_Context.PASyncState = PBPAPP_PA_SYNC_STATE_IDLE;
    }
  }

  if (PBPAPP_Context.BIGSyncState != PBPAPP_BIG_SYNC_STATE_IDLE)
  {
    ret = PBP_PBK_StopBIGSync(BIG_HANDLE);
    if (ret != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG("  Fail   : BP_PBK_StopBIGSync() function, result: 0x%02X\n", ret);
    }
    else
    {
      APP_DBG_MSG("  Success: BP_PBK_StopBIGSync() function\n");
      PBPAPP_Context.BIGSyncState = PBPAPP_BIG_SYNC_STATE_IDLE;
    }
  }

  APP_DBG_MSG("   >>==  MX_AudioDeInit()\n");
  MX_AudioDeInit();

  if (PBPAPP_Context.ScanState == PBPAPP_SCAN_STATE_IDLE)
  {
    ret = CAP_Broadcast_StartAdvReportParsing();
    if (ret != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG("  Fail   : CAP_Broadcast_StartAdvReportParsing() function, result: 0x%02X\n", ret);
    }
    else
    {
      APP_DBG_MSG("  Success: CAP_Broadcast_StartAdvReportParsing() function\n");
    }

    if (ret == BLE_STATUS_SUCCESS)
    {
      /* Starts an Observation procedure */
      ret = aci_gap_start_observation_proc(SCAN_INTERVAL,
                                           SCAN_WINDOW,
                                           0x00,              /* LE_Scan_Type */
                                           0x00,              /* Address type: Public */
                                           0x00,              /* Filter duplicates: No */
                                           0x00 );            /* Filter policy: Accept all */
      if (ret != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("  Fail   : aci_gap_start_observation_proc() function, result: 0x%02X\n", ret);
      }
      else
      {
        APP_DBG_MSG("  Success: aci_gap_start_observation_proc() function\n");
        PBPAPP_Context.ScanState = PBPAPP_SCAN_STATE_SCANNING;
      }
    }
  }

  *pSourceID = aSourceIdList[CurrentID];
  APP_DBG_MSG(">>==  End Switch Broadcast Source\n");
}

uint32_t PBPAPP_GetBrdSource(void)
{
  return aSourceIdList[CurrentID];
}

void APP_NotifyRxAudioCplt(uint16_t AudioFrameSize)
{
  UNUSED(AudioFrameSize);
}

void APP_NotifyRxAudioHalfCplt(void)
{

}

void APP_NotifyTxAudioCplt(uint16_t AudioFrameSize)
{
  uint8_t i;

  if (PBPAPP_Context.bap_role & BAP_ROLE_BROADCAST_SINK)
  {
    for (i = 0; i< PBPAPP_Context.current_num_bis; i++)
    {
      CODEC_ReceiveData(PBPAPP_Context.current_BIS_conn_handles[i], 1, &aRecBuff[0]  + AudioFrameSize/2 + i);
    }
  }
}

void APP_NotifyTxAudioHalfCplt(void)
{
  uint8_t i;

  if (PBPAPP_Context.bap_role & BAP_ROLE_BROADCAST_SINK)
  {
    for (i = 0; i< PBPAPP_Context.current_num_bis; i++)
    {
      CODEC_ReceiveData(PBPAPP_Context.current_BIS_conn_handles[i], 1, &aRecBuff[0] + i);
    }
  }
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
        APP_DBG_MSG(">>== CAP_CODEC_CONFIGURED_EVT\n");
        Sampling_Freq_t *freq = (Sampling_Freq_t *)pNotification->pInfo;
        APP_DBG_MSG("     - Sample Frequency Type:   0x%02X\n",*freq);
        APP_DBG_MSG("==>> Audio Clock with Sample Frequency Type 0x%02X Initialization\n",*freq);
        AudioClock_Init(*freq);
      }
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
    APP_DBG_MSG("  Fail   : BLE_AUDIO_STACK_Init() function, result: 0x%02X\n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: BLE_AUDIO_STACK_Init() function\n");
  }
  if(ret == BLE_STATUS_SUCCESS)
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

    /*Clear the MICP Configuration*/
    memset(&PBPAPP_MICP_Config,0,sizeof(MICP_Config_t));

    /*Clear the CSIP Configuration*/
    memset(&PBPAPP_CSIP_Config,0,sizeof(CSIP_Config_t));

    PBPAPP_CAP_Config.Role = CAP_Role;
    PBPAPP_CAP_Config.MaxNumLinks = CFG_BLE_NUM_LINK;

    if ((CAP_Role & CAP_ROLE_ACCEPTOR) == CAP_ROLE_ACCEPTOR)
    {
      PBPAPP_CAP_Config.MaxNumAcceptorInstances = MAX_CAP_ACCEPTOR_INSTANCES;
    }
    PBPAPP_CAP_Config.pStartRamAddr = (uint8_t *)&aCAPMemBuffer;
    PBPAPP_CAP_Config.RamSize = CAP_DYN_ALLOC_SIZE;

    PBPAPP_BAP_Config.Role = BAP_Role;

    PBPAPP_BAP_Config.MaxNumBleLinks = CFG_BLE_NUM_LINK;

    if ((PBPAPP_BAP_Config.Role & BAP_ROLE_BROADCAST_SINK) == BAP_ROLE_BROADCAST_SINK)
    {
      /*Published Audio Capabilities of Unicast Server and Broadcast Sink Configuration*/
      PBPAPP_BAP_Config.PACSSrvConfig.MaxNumSnkPACRecords = MAX_NUM_PAC_SNK_RECORDS;
      PBPAPP_BAP_Config.PACSSrvConfig.MaxNumSrcPACRecords = MAX_NUM_PAC_SRC_RECORDS;
      PBPAPP_BAP_Config.PACSSrvConfig.pStartRamAddr = (uint8_t *)&aPACSSrvMemBuffer;
      PBPAPP_BAP_Config.PACSSrvConfig.RamSize = BAP_PACS_SRV_DYN_ALLOC_SIZE;
    }

    if ((PBPAPP_BAP_Config.Role & BAP_ROLE_SCAN_DELEGATOR) == BAP_ROLE_SCAN_DELEGATOR)
    {
      /*Broadcast Audio Scan for Scan Delegator Configuration*/
      PBPAPP_BAP_Config.BASSSrvConfig.MaxNumBSRC = MAX_NUM_SDE_BSRC_INFO;
      PBPAPP_BAP_Config.BASSSrvConfig.MaxCodecConfSize = MAX_BASS_CODEC_CONFIG_SIZE;
      PBPAPP_BAP_Config.BASSSrvConfig.MaxMetadataLength = MAX_BASS_METADATA_SIZE;
      PBPAPP_BAP_Config.BASSSrvConfig.MaxNumBaseSubgroups = MAX_NUM_BASS_BASE_SUBGROUPS;
      PBPAPP_BAP_Config.BASSSrvConfig.pStartRamAddr = (uint8_t *)&aBASSSrvMemBuffer;
      PBPAPP_BAP_Config.BASSSrvConfig.RamSize = BAP_BASS_SRV_DYN_ALLOC_SIZE;
    }

    /*Isochronous Channels Configuration*/
    PBPAPP_BAP_Config.ISOChnlConfig.MaxNumCIG = 0;
    PBPAPP_BAP_Config.ISOChnlConfig.MaxNumCISPerCIG = 0;
    PBPAPP_BAP_Config.ISOChnlConfig.MaxNumBIG = MAX_NUM_BIG;
    PBPAPP_BAP_Config.ISOChnlConfig.MaxNumBISPerBIG = MAX_NUM_BIS_PER_BIG;
    PBPAPP_BAP_Config.ISOChnlConfig.pStartRamAddr = 0;
    PBPAPP_BAP_Config.ISOChnlConfig.RamSize = 0u;

    if (((PBPAPP_BAP_Config.Role & BAP_ROLE_BROADCAST_ASSISTANT) == BAP_ROLE_BROADCAST_ASSISTANT)\
      || ((PBPAPP_BAP_Config.Role & BAP_ROLE_SCAN_DELEGATOR) == BAP_ROLE_SCAN_DELEGATOR))
    {
      /* Non-Volatile Memory Management for BAP Services restoration */
      PBPAPP_BAP_Config.NvmMgmtConfig.pStartRamAddr = (uint8_t *)&aNvmMgmtMemBuffer;
      PBPAPP_BAP_Config.NvmMgmtConfig.RamSize = BAP_NVM_MGMT_DYN_ALLOC_SIZE;
    }
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
      APP_DBG_MSG("  Fail   : CAP_Init() function, result: 0x%02X\n", ret);
    }
    else
    {
      APP_DBG_MSG("  Success: CAP_Init() function\n");
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

      CODEC_ManagerInit(MAX_PATH_NB*CODEC_POOL_SUB_SIZE, (uint8_t*)aCodecPacketsMemory, &lc3_config, 100u, 1700u);
    }
  }
  return ret;
}

static uint8_t PBPAPP_StartBroadcastAudio(Audio_Role_t role)
{
  if(role == AUDIO_ROLE_SOURCE)
  {
    CODEC_RegisterTriggerClbk(1,0,&start_audio_source);
  }
  else if (role == AUDIO_ROLE_SINK)
  {
    CODEC_RegisterTriggerClbk(1,1,&start_audio_sink);
  }
  return 0;
}

static uint8_t PBPAPP_BroadcastSetupAudio(Audio_Role_t role)
{
  Sampling_Freq_t sampling_freq;
  Frame_Duration_t frame_duration;
  uint8_t direction = DATA_PATH_INPUT;
  uint32_t controller_delay, controller_delay_min, controller_delay_max;
  uint8_t a_codec_id[5] = {0x00,0x00,0x00,0x00,0x00};
  tBleStatus ret;

  APP_DBG_MSG("==>> Start PBPAPP_BroadcastSetupAudio function\n");

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
                 (uint8_t *)aRecBuff,
                 0,
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
        APP_DBG_MSG("Controller delay chosen to maximum value %d us\n",controller_delay);
      }
      else
      {
        APP_DBG_MSG("Controller delay chosen to %d us\n",controller_delay);
      }

      /* compute the application delay */
      APP_DBG_MSG("Expecting application to respect the delay of %d us\n",
                  (PBPAPP_Context.base_group.PresentationDelay - controller_delay));
      if ((PBPAPP_Context.base_group.PresentationDelay - controller_delay) > APP_DELAY_SNK_MAX)
      {
        APP_DBG_MSG("Warning, could not respect the presentation delay value");
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
        APP_DBG_MSG("Controller delay chosen to value %d us\n", controller_delay);
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
      APP_DBG_MSG("  Fail   : CAP_Broadcast_SetupAudioDataPath() function, result: 0x%02X\n", ret);
    }
    else
    {
      APP_DBG_MSG("  Success: CAP_Broadcast_SetupAudioDataPath() function\n");
    }
  }
  else
  {
    APP_DBG_MSG("Sampling Frequency in LTV is invalid\n");
    ret = BLE_STATUS_FAILED;
  }
  APP_DBG_MSG("==>> End PBPAPP_BroadcastSetupAudio function\n");
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