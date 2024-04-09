/*****************************************************************************
 * @file    ble_codec.c
 * @author  MDG
 * @brief   This file implements the Codec functions for BLE stack library.
 *****************************************************************************
 * @attention
 *
 * Copyright (c) 2018-2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 *****************************************************************************
 */

#include "app_common.h"
#include "ble_codec.h"
#include "ble.h"

#include "codec_mngr.h"

#define BLE_PLAT_NUM_CIS             (2u)
#define BLE_PLAT_NUM_BIS             (2u)

typedef struct _CIS_Conf_t
{
  uint8_t CIG_ID;
  uint16_t CIS_Conn_Handle;
  uint8_t is_peripheral;
}CIS_Conf_t;

typedef struct _BIS_Conf_t
{
  uint8_t BIG_Handle;
  uint16_t BIS_Conn_Handle;
}BIS_Conf_t;

CIS_Conf_t CIS_Conf[BLE_PLAT_NUM_CIS] = {0};         /*One CIG with 2 CIS*/
BIS_Conf_t BIS_Conf[BLE_PLAT_NUM_BIS] = {0};         /*One BIG with 2 BIS*/

static uint8_t BLE_GetFreeCISConfSlot(CIS_Conf_t **pCIS_Conf);
static uint8_t BLE_GetExistingCISConfSlot(uint16_t CIS_Conn_Handle,CIS_Conf_t **pCIS_Conf);
static uint8_t BLE_SetFreeCISConfSlot(uint16_t CIS_Conn_Handle, uint8_t *CIG_ID);




uint8_t BLE_CodecInit( void )
{
  for (int32_t i = 0 ; i < BLE_PLAT_NUM_CIS ; i++)
  {
    CIS_Conf[i].CIG_ID = 0xFFU;
    CIS_Conf[i].CIS_Conn_Handle = 0xFFFFU;
    CIS_Conf[i].is_peripheral = 0;
  }

  for (int32_t i = 0 ; i < BLE_PLAT_NUM_BIS ; i++)
  {
    BIS_Conf[i].BIG_Handle = 0xFFU;
    BIS_Conf[i].BIS_Conn_Handle = 0xFFFFU;
  }
  /* reset codec manager */
  CODEC_ManagerReset();
  return 0;
}


/*****************************************************************************/

uint8_t BLE_ConfigureDataPath( uint8_t data_path_direction,
                                   uint8_t data_pathID,
                                   uint8_t vendor_specific_config_length,
                                   const uint8_t* vendor_specific_config )
{
  tBleStatus ret;
  CODEC_ConfigureDataPathCmd_t param;
  param.direction = data_path_direction;
  param.path_ID = data_pathID;
  param.config_len = vendor_specific_config_length;
  param.sample_depth = vendor_specific_config[0];
  param.decimation = vendor_specific_config[1];
  LOG_INFO_APP("==>> CODEC Configure Data Path with following parameters:\n");
  LOG_INFO_APP("  Direction : %d\n",data_path_direction);
  LOG_INFO_APP("  Data Path ID : 0x%02X\n",data_pathID);
  LOG_INFO_APP("  Sample Depth : %d\n",param.sample_depth);
  LOG_INFO_APP("  Decimation : %d\n",param.decimation);
  ret = CODEC_ConfigureDataPath((uint8_t*)&param);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("==>> CODEC_ConfigureDataPath() : Fail, reason: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_APP("==>> CODEC_ConfigureDataPath() : Success\n");
  }
  return ret;
}

/*****************************************************************************/

uint8_t BLE_ReadLocalSupportedCodecs(
                                uint8_t* Num_supported_standard_codecs,
                                uint8_t* Std_Codec,
                                uint8_t* Num_Supported_Vendor_Specific_Codecs,
                                uint8_t* VS_Codec )
{
  uint8_t* p_std_codec;
  uint8_t* p_vs_codec;
  uint8_t status;
  status = CODEC_ReadLocalSupportedCodecsV2(Num_supported_standard_codecs,
                                          &p_std_codec,
                                          Num_Supported_Vendor_Specific_Codecs,
                                          &p_vs_codec);
  if (status == 0x00)
  {
    if (*Num_supported_standard_codecs > 0u)
    {
      memcpy(&Std_Codec[0],p_std_codec,((*Num_supported_standard_codecs)*2));
    }
    if (*Num_Supported_Vendor_Specific_Codecs > 0u)
    {
      memcpy(&VS_Codec[0],p_vs_codec,((*Num_Supported_Vendor_Specific_Codecs)*2));
    }
  }
  return status;
}

/*****************************************************************************/

uint8_t BLE_ReadLocalSupportedCodecCapabilities(
                                        const uint8_t* codecID,
                                        uint8_t logical_transport_type,
                                        uint8_t direction,
                                        uint8_t* num_codec_capabilities,
                                        uint8_t* codec_capability )
{
  uint8_t* p_codec_capability;
  uint8_t status;
  uint8_t codec_capabilities_len;
  uint8_t len = 0u;
  uint8_t i;
  status = CODEC_ReadSupportedCodecCapabilies((uint8_t*)codecID,
                                              logical_transport_type,
                                              direction,
                                              num_codec_capabilities,
                                              &p_codec_capability);
  if (status == 0x00)
  {
    for (i = 0; i < (*num_codec_capabilities) ; i++)
    {
      codec_capabilities_len = p_codec_capability[len];
      memcpy(&codec_capability[len],&p_codec_capability[len],(codec_capabilities_len+1));
      len = (len+codec_capabilities_len+1);
    }
  }
  return status;
}

/*****************************************************************************/

uint8_t BLE_ReadLocalSupportedControllerDelay(
                                        const uint8_t* codec_ID,
                                        uint8_t logical_transport_type,
                                        uint8_t direction,
                                        uint8_t codec_configuration_length,
                                        const uint8_t* codec_configuration,
                                        uint8_t* min_controller_delay,
                                        uint8_t* max_controller_delay )
{
  int32_t status;
  uint32_t min_delay = 0;
  uint32_t max_delay = 0;

  status = CODEC_ReadLocalSupportedControllerDelay((uint8_t*)codec_ID,
                                                   logical_transport_type,
                                                   direction,
                                                   codec_configuration_length,
                                                   (uint8_t*)codec_configuration,
                                                   &min_delay,
                                                   &max_delay);
  if (status == 0x00)
  {
    min_controller_delay[2] = (uint8_t) ((min_delay >> 16 ));
    min_controller_delay[1] = (uint8_t) ((min_delay >> 8 ));
    min_controller_delay[0] = (uint8_t) (min_delay );

    max_controller_delay[2] = (uint8_t) ((max_delay >> 16 ));
    max_controller_delay[1] = (uint8_t) ((max_delay >> 8 ));
    max_controller_delay[0] = (uint8_t) (max_delay );
  }
  return status;
}

/*****************************************************************************/

uint8_t BLE_SetupIsoDataPath(uint16_t connection_handle,hci_le_setup_iso_data_path_params* iso_command_params)
{
  int32_t ret;
  CODEC_SetupIsoDataPathCmd_t param;

  param.con_hdle = connection_handle;
  param.codec_ID[0] = iso_command_params->Codec_ID[0];
  param.direction = iso_command_params->Data_Path_Direction;
  param.path_ID = iso_command_params->Data_Path_ID;
  memcpy(&param.controller_delay[0],&iso_command_params->Controller_Delay,3);
  param.codec_conf_len = iso_command_params->Codec_Configuration_Length;
  memcpy(&param.codec_conf,iso_command_params->pCodec_Configuration,iso_command_params->Codec_Configuration_Length);

  LOG_INFO_APP("==>> CODEC Setup ISO Data Path for CIS Connection Handle 0x%04X\n", connection_handle);
  ret = CODEC_SetupIsoDataPath((uint8_t*)&param);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("==>> CODEC_SetupIsoDataPath() : Fail, reason: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_APP("==>> CODEC_SetupIsoDataPath() : Success\n");
  }
  return ret;
}

/*****************************************************************************/

uint8_t BLE_RemoveIsoDataPath( uint16_t connection_handle,
                               uint8_t data_path_direction )
{
  int32_t ret;
  uint8_t a_param[3];

  a_param[0] = (uint8_t) (connection_handle);
  a_param[1] = (uint8_t) ((connection_handle >> 8 ));
  a_param[2] = data_path_direction;

  LOG_INFO_APP("==>> CODEC Remove ISO Data Path for CIS Connection Handle 0x%04X and path direction mask 0x%02X\n",
              connection_handle,
              data_path_direction);
  ret = CODEC_RemoveIsoDataPath((uint8_t*)&a_param);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("==>> CODEC_RemoveIsoDataPath() : Fail, reason: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_APP("==>> CODEC_RemoveIsoDataPath() : Success\n");
  }
  return ret;
}

/*****************************************************************************/
uint8_t BLE_SendIsoDataOutToCodec(uint16_t iso_connection_handle,
                                  uint8_t pb_flag,
                                  uint8_t ts_flag,
                                  uint32_t timestamp,
                                  uint16_t PSN,
                                  uint8_t  packet_status_flag,
                                  uint16_t iso_data_load_length,
                                  uint32_t* iso_data)
{
  return CODEC_ReceiveMediaPacket(iso_connection_handle,
                                  pb_flag,
                                  ts_flag,
                                  timestamp,
                                  PSN,
                                  packet_status_flag,
                                  iso_data_load_length,
                                  (uint8_t*)iso_data);
}

/*****************************************************************************/

uint8_t BLE_SendIsoDataInToCodec(uint16_t iso_connection_handle,
                                 uint8_t pb_flag,
                                 uint8_t ts_flag,
                                 uint32_t timestamp,
                                 uint16_t PSN,
                                 uint16_t iso_data_load_length,
                                 uint8_t* iso_data)
{
  return 0;
}

/*****************************************************************************/

void BLE_LeSyncEvent(uint8_t group_id,
                     uint32_t next_anchor_point,
                     uint32_t time_stamp,
                     uint32_t next_sdu_delivery_timeout)
{
  AUDIO_SyncEventClbk(group_id,next_anchor_point,time_stamp,next_sdu_delivery_timeout);
}

/*****************************************************************************/

void BLE_IsochronousGroupEvent(uint16_t opcode,
                              uint8_t status,
                              uint8_t big_handle,
                              uint8_t cig_id,
                              uint16_t iso_interval,
                              uint8_t num_connection_handles,
                              uint16_t* iso_con_handle,
                              uint8_t* transport_latency_C_to_P,
                              uint8_t* transport_latency_P_to_C)
{
  uint8_t type;
  uint8_t ID;
  CIS_Conf_t *p_cis_conf;
  LOG_INFO_APP(">>== ISOCHRONOUS_GROUP_EVENT\n");
  LOG_INFO_APP("     - Opcode:   0x%04X\n", opcode);
  if ((opcode == 0x001DU || opcode == 0x001BU) && (status == 0))
  {
    /* HCI_LE_BIG_SYNC_ESTABLISHED_EVENT || HCI_LE_CREATE_BIG_COMPLETE_EVENT */
    type = 1;
    ID = big_handle;
    for (int32_t i = 0 ; i < num_connection_handles ; i++)
    {
      BIS_Conf[i].BIG_Handle = big_handle;
      BIS_Conf[i].BIS_Conn_Handle = iso_con_handle[i];
    }
    LOG_INFO_APP("     - big_handle:   0x%02X\n", big_handle);
    LOG_INFO_APP("     - iso_interval:   0x%04X\n", iso_interval);
    LOG_INFO_APP("==>> AUDIO_RegisterGroup()\n");
    uint32_t transport_latency_c2p = transport_latency_C_to_P[0] + (transport_latency_C_to_P[1]<<8) + (transport_latency_C_to_P[2]<<16);
    AUDIO_RegisterGroup(type,
                        ID,
                        num_connection_handles,
                        iso_con_handle,
                        iso_interval,
                        0,
                        transport_latency_c2p,
                        0);
  }
  else if (opcode == 0x001CU )
  {
    /* HCI_LE_TERMINATE_BIG_COMPLETE_EVENT */
    type = 1;
    for ( int32_t i = 0 ; i < BLE_PLAT_NUM_BIS ; i++)
    {
      if ( big_handle == BIS_Conf[i].BIG_Handle )
      {
        BLE_RemoveIsoDataPath(BIS_Conf[i].BIS_Conn_Handle,0x01);

        BIS_Conf[i].BIG_Handle = 0xFFu;
        BIS_Conf[i].BIS_Conn_Handle = 0xFFFFu;
        LOG_INFO_APP("     - big_handle:   0x%02X\n", big_handle);
        LOG_INFO_APP("==>> AUDIO_UnregisterGroup()\n");
        AUDIO_UnregisterGroup(type, big_handle);
      }
    }
  }
  else if (opcode == 0x001EU || opcode == 0x206CU)
  {
     /*HCI_LE_BIG_SYNC_LOST_EVENT || HCI_LE_BIG_Terminate_Sync*/
    type = 1;
    for ( int32_t i = 0 ; i < BLE_PLAT_NUM_BIS ; i++)
    {
      if ( big_handle == BIS_Conf[i].BIG_Handle )
      {
        BLE_RemoveIsoDataPath(BIS_Conf[i].BIS_Conn_Handle,0x02);
        BIS_Conf[i].BIG_Handle = 0xFFu;
        BIS_Conf[i].BIS_Conn_Handle = 0xFFFFu;
        LOG_INFO_APP("     - big_handle:   0x%02X\n", big_handle);
        LOG_INFO_APP("==>> AUDIO_UnregisterGroup()\n");
        AUDIO_UnregisterGroup(type, big_handle);
      }
    }
  }
  else if (opcode == 0x001AU) /*HCI_LE_CIS_REQUEST_EVENT*/
  {
    /* HCI_LE_CIS_REQUEST_EVENT (cig_id and ConnectionHandle)*/
    if ((BLE_GetExistingCISConfSlot(iso_con_handle[0], &p_cis_conf) == 0u) || (BLE_GetFreeCISConfSlot(&p_cis_conf) == 0u))
    {
      p_cis_conf->CIS_Conn_Handle = iso_con_handle[0];
      p_cis_conf->CIG_ID = cig_id;
      p_cis_conf->is_peripheral = 1;
    }
    else
    {
      /* should not be reached */
    }
    LOG_INFO_APP("     - CIG ID:   %d     - cis_conn_handle:   0x%04X\n", cig_id, iso_con_handle[0]);
  }
  else if (opcode == 0x2062U) /*HCI_LE_Set_CIG_Parameters*/
  {
    /* HCI_LE_Set_CIG_Parameters (cig_id and ConnectionHandle)*/
    for (int32_t i = 0 ; i<num_connection_handles ; i++)
    {
      APP_DBG_MSG("     - CIG ID:   %d     - cis_conn_handle:   0x%04X\n", cig_id, iso_con_handle[i]);
      if ((BLE_GetExistingCISConfSlot(iso_con_handle[i], &p_cis_conf) == 0u) || (BLE_GetFreeCISConfSlot(&p_cis_conf) == 0u))
      {
        p_cis_conf->CIS_Conn_Handle = iso_con_handle[i];
        p_cis_conf->CIG_ID = cig_id;
        p_cis_conf->is_peripheral = 0;
      }
      else
      {
        /* should not be reached */
      }
    }
  }
  else if (opcode == 0x0019U)
  {
    /*HCI_LE_CIS_ESTABLISHED_EVENT  (iso_interval and ConnectionHandle)*/
    type = 0;
    LOG_INFO_APP("     - cis_conn_handle:   0x%04X    - iso_interval:   %d\n", iso_con_handle[0], iso_interval);
	if ((status == 0) &&    
        ((BLE_GetExistingCISConfSlot(iso_con_handle[0], &p_cis_conf) == 0u) || (BLE_GetFreeCISConfSlot(&p_cis_conf) == 0u)))
    {
      /* the CIS can be re-established without a new call to Set_CIG_Parameters */
      p_cis_conf->CIS_Conn_Handle = iso_con_handle[0];
        
      LOG_INFO_APP("==>> AUDIO_RegisterGroup()\n");
      uint32_t transport_latency_c2p = transport_latency_C_to_P[0] + (transport_latency_C_to_P[1]<<8) + (transport_latency_C_to_P[2]<<16);
      uint32_t transport_latency_p2c = transport_latency_P_to_C[0] + (transport_latency_P_to_C[1]<<8) + (transport_latency_P_to_C[2]<<16);

      AUDIO_RegisterGroup(type,
                          p_cis_conf->CIG_ID,
                          1,
                          &iso_con_handle[0],
                          iso_interval,
                          p_cis_conf->is_peripheral,
                          transport_latency_c2p,
                          transport_latency_p2c);

    }
  }
  else if (opcode == 0x0005U)
  {
    /*HCI_DISCONNECTION_COMPLETE_EVENT*/
    uint8_t l_cig_id;
    LOG_INFO_APP("     - cis_conn_handle:   0x%04X\n", iso_con_handle[0]);
    if (BLE_SetFreeCISConfSlot(iso_con_handle[0], &l_cig_id) == 0)
    {
      LOG_INFO_APP("==>> AUDIO_UnregisterGroup()\n");
      AUDIO_UnregisterGroup(0, l_cig_id);
    }
  }
}

/*****************************************************************************/

void BLE_CalibrationCallback(uint32_t timeStamp)
{
  AUDIO_CalibrationClbk(timeStamp);
}

/*****************************************************************************/
static uint8_t BLE_GetExistingCISConfSlot(uint16_t CIS_Conn_Handle,CIS_Conf_t **pCIS_Conf)
{
  uint8_t i;
  for (i = 0; i < BLE_PLAT_NUM_CIS ; i++)
  {
    if ((CIS_Conf[i].CIS_Conn_Handle == CIS_Conn_Handle) && (CIS_Conf[i].CIG_ID != 0xFFu))
    {
      *pCIS_Conf = &CIS_Conf[i];
      return 0u;
    }
  }
  return 1u;
}

static uint8_t BLE_GetFreeCISConfSlot(CIS_Conf_t **pCIS_Conf)
{
  uint8_t i;
  for (i = 0; i < BLE_PLAT_NUM_CIS ; i++)
  {
    if (CIS_Conf[i].CIS_Conn_Handle == 0xFFFF)
    {
      *pCIS_Conf = &CIS_Conf[i];
      return 0u;
    }
  }
  return 1u;
}

static uint8_t BLE_SetFreeCISConfSlot(uint16_t CIS_Conn_Handle, uint8_t *CIG_ID)
{
  uint8_t i;
  uint8_t cig_id = 0xff;

  for (i = 0; i < BLE_PLAT_NUM_CIS ; i++)
  {
    if (CIS_Conf[i].CIS_Conn_Handle == CIS_Conn_Handle)
    {
      CIS_Conf[i].CIS_Conn_Handle = 0xFFFF;
      cig_id = CIS_Conf[i].CIG_ID;
      CIS_Conf[i].is_peripheral = 0;
      /* don't erase CIG ID because it may be reused */

      int8_t cnt = 0;
      for (int8_t k = 0; k < BLE_PLAT_NUM_CIS ; k++)
      {
        if ((CIS_Conf[k].CIG_ID == cig_id) && (CIS_Conf[k].CIS_Conn_Handle != 0xFFFF))
        {
          cnt++;
        }
      }

      if (cnt > 0)
      {
        return 1u; /* CIG still in use by another CIS */
      }
    }
  }

  if (cig_id == 0xff)
  {
    /* The connection handle didn't match any registered CIS con handle */
    return 1u;
  }

  *CIG_ID = cig_id;
  return 0u; /* the CIG is now inactive */
}
