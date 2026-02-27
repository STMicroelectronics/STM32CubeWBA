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

/* Includes ------------------------------------------------------------------*/
#include "app_common.h"
#include "codec_mngr.h"
#include "log_module.h"
#include "ble_types.h"

#include "stm_list.h"
#include "ll_sys.h"
#include "ble_codec.h"

/* Defines -------------------------------------------------------------------*/

/* NUM_OF_ISO_LINKS = MAX_NUM_BIG*MAX_NUM_BIS + MAX_NUM_CIG*MAX_NUM_CIS
 *                          = 1*6 + 1*2 = 8
 */
#define NUM_OF_ISO_LINKS                MIN(MAX_ISO_STRM_PER_GRP, 8)

/* NUM_ISO_DATA_PATH = NUM_OF_ISO_LINKS*(1 INPUT + 1 OUTPUT)
 */
#define NUM_ISO_DATA_PATH               MIN(MAX_PATH_NB, NUM_OF_ISO_LINKS*2)

#define LL_MAX_NUMBER_OF_ISO_DATA       13

#define ISO_SDU_STATUS_IDLE             0
#define ISO_SDU_STATUS_BUSY             1


#define GET_U24(b, i)  ((b[(i)+2]<<16) + (b[(i)+1]<<8) + b[(i)])
#define GET_U16(b, i)  ((b[(i)+1]<<8)  +  b[(i)])
#define GET_U8(b, i)    (b[(i)])

/* Typedef -------------------------------------------------------------------*/
typedef struct
{
  tListNode node;
  uint16_t connHandle;
  iso_sdu_buf_hdr_st iso_buff;
} iso_sdu_t;

typedef struct
{
  uint8_t big_handle;
  uint16_t connection_handle;
  uint8_t  direction;
  uint8_t  path_ID;
  uint8_t  coding_format;
} iso_data_path_params_t;

/* Private functions  --------------------------------------------------------*/

static void BLE_CODEC_RxIsoDataVendorClbk( const iso_sdu_buf_hdr_p iso_buff, const uint16_t conn_handle );
static int BLE_GetIsoIndex( uint16_t conn_handle );
static void BLE_ClearIsoSdu( uint16_t conn_handle );
static void BLE_ClearIsoDataPath( uint16_t conn_handle, uint8_t dataPathDirection );
static void BLE_ClearIsoDataPathBIG( uint8_t big_handle, uint8_t clear_iso_sdu_flag );
static void BLE_RegisterIsoPath( uint16_t conn_handle, uint8_t big_handle );

/* Private variables  --------------------------------------------------------*/

static iso_data_path_params_t gIso_data_path[NUM_ISO_DATA_PATH];

static tListNode gIso_sdu_pool_list;
static tListNode gIso_sdu_list[NUM_OF_ISO_LINKS];
static uint8_t gIso_sdu_status[NUM_OF_ISO_LINKS];
static iso_sdu_t gIso_buff_pool[LL_MAX_NUMBER_OF_ISO_DATA];

/*****************************************************************************/
void BLE_CodecReset( void )
{
  /* Initialization of the Setup Iso Data path param memory */
  memset( gIso_data_path, 0xFF, sizeof(gIso_data_path) );

  /* Initialization of the ISO SDU memory */
  for ( uint8_t i = 0; i < NUM_OF_ISO_LINKS; i++ )
  {
    LST_init_head( &gIso_sdu_list[i] );
  }

  LST_init_head( &gIso_sdu_pool_list );

  for ( uint8_t i = 0; i < LL_MAX_NUMBER_OF_ISO_DATA; i++ )
  {
    LST_insert_tail( &gIso_sdu_pool_list, (tListNode*)&gIso_buff_pool[i] );
  }

  CODEC_ManagerReset();
}

/*****************************************************************************/

uint8_t BLECB_ConfigureDataPath( uint8_t data_path_direction,
                                 uint8_t data_pathID,
                                 uint8_t vendor_specific_config_length,
                                 const uint8_t* vendor_specific_config )
{
  uint8_t status;
  LOG_INFO_APP("==>> CODEC Configure Data Path with following parameters:\n   > direction : %d\n   > path ID : 0x%02X\n",
               data_path_direction,
               data_pathID);

  status = CODEC_ConfigureDataPath(data_path_direction,
                                   data_pathID,
                                   vendor_specific_config_length,
                                   (uint8_t*)vendor_specific_config);
  if (status == 0x00)
  {
    LOG_INFO_APP("   > Success\n");
  }
  else
  {
    LOG_INFO_APP("   > Fail, reason: 0x%02X\n", status);
  }
  return status;
}

/*****************************************************************************/

uint8_t BLECB_ReadLocalSupportedCodecs( uint8_t* Num_supported_standard_codecs,
                                        uint8_t* Std_Codec,
                                        uint8_t* Num_Supported_Vendor_Specific_Codecs,
                                        uint8_t* VS_Codec )
{
  uint8_t* p_std_codec;
  uint8_t* p_vs_codec;
  uint8_t status;
  LOG_INFO_APP("==>> CODEC Read Local Supported Codecs\n");

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

uint8_t BLECB_ReadLocalSupportedCodecCapabilities(const uint8_t* codecID,
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
  LOG_INFO_APP("==>> CODEC Read Local Supported Codecs Capabilities\n   > coding format %d\n", codecID[0]);

  status = CODEC_ReadLocalSupportedCodecCapabilies((uint8_t*)codecID,
                                                   logical_transport_type,
                                                   direction,
                                                   num_codec_capabilities,
                                                   &p_codec_capability );
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

uint8_t BLECB_ReadLocalSupportedControllerDelay(
                                        const uint8_t* codec_ID,
                                        uint8_t logical_transport_type,
                                        uint8_t direction,
                                        uint8_t codec_configuration_length,
                                        const uint8_t* codec_configuration,
                                        uint8_t* min_controller_delay,
                                        uint8_t* max_controller_delay )
{
  uint8_t status;
  uint32_t min_delay = 0;
  uint32_t max_delay = 0;
  LOG_INFO_APP("==>> CODEC Read Local Supported Controller Delay\n");

  status = CODEC_ReadLocalSupportedControllerDelay((uint8_t*)codec_ID,
                                                   logical_transport_type,
                                                   direction,
                                                   codec_configuration_length,
                                                   (uint8_t*)codec_configuration,
                                                   &min_delay,
                                                   &max_delay);
  if (status == 0x00)
  {
    LOG_INFO_APP("   > Success\n");

    min_controller_delay[2] = (uint8_t) ((min_delay >> 16 ));
    min_controller_delay[1] = (uint8_t) ((min_delay >> 8 ));
    min_controller_delay[0] = (uint8_t)  (min_delay );

    max_controller_delay[2] = (uint8_t) ((max_delay >> 16 ));
    max_controller_delay[1] = (uint8_t) ((max_delay >> 8 ));
    max_controller_delay[0] = (uint8_t)  (max_delay );
  }
  else
  {
    LOG_INFO_APP("   > Fail, reason: 0x%02X\n", status);
  }
  return status;
}

/*****************************************************************************/

uint8_t BLECB_SetupIsoDataPath( uint16_t conn_handle,
                                uint8_t DataPathDirection,
                                uint8_t DataPathID,
                                const uint8_t CodecID[],
                                const uint8_t ControllerDelay[],
                                uint8_t CodecConfigurationLength,
                                const uint8_t* CodecConfiguration )
{
  ble_intf_setup_iso_data_path ll_param = {0};
  uint8_t i;
  uint8_t status;
  uint8_t lstatus = HCI_UNKNOWN_HCI_COMMAND_ERR_CODE;
  uint8_t new_iso_data_path_allocated = 0;
  uint8_t skip_allocating = 0;
  iso_data_path_params_t *path;

  for ( i = 0; i < NUM_ISO_DATA_PATH; i ++ )
  {
    path = &gIso_data_path[i];

    if ( path->connection_handle == conn_handle )
    {
      if (path->direction == DataPathDirection)
      {
        /* Iso Data Path structure already allocated */
        return HCI_COMMAND_DISALLOWED_ERR_CODE;
      }

      if ( path->direction == 0xFF )
      {
        /* Iso Data Path structure is pre-allocated */
        path->direction = DataPathDirection;
        skip_allocating = 1;
        break;
      }
    }
  }

  if ( !skip_allocating )
  {
      /* Allocate a new Iso Data Path structure for this Data path
       * direction
       */
    for ( i = 0; i < NUM_ISO_DATA_PATH; i ++ )
    {
      path = &gIso_data_path[i];
      if ( (path->connection_handle == 0xFFFF) &&
           (path->direction == 0xFF) )
      {
        /* ISO Data Path structure not yet allocated */
        new_iso_data_path_allocated = 1;
        path->big_handle = 0xFF;
        path->connection_handle = conn_handle;
        path->direction = DataPathDirection;
        break;
      }
    }
  }

  if (path == &gIso_data_path[NUM_ISO_DATA_PATH])
  {
    /* No more space */
    return HCI_MEMORY_CAPACITY_EXCEEDED_ERR_CODE;
  }

  /* Save Data to a struct for further steps */
  path->coding_format = CodecID[0];
  path->path_ID = DataPathID;

  ll_param.data_path_id = DataPathID;
  ll_param.codec_id[0] = AUDIO_CODING_FORMAT_TRANSPARENT;
  ll_param.data_path_dir = path->direction;

  if ((path->path_ID != DATA_PATH_HCI) || (path->coding_format != AUDIO_CODING_FORMAT_TRANSPARENT))
  {
    CODEC_SetupIsoDataPathCmd_t codec_param;

    codec_param.con_hdle = conn_handle;
    memcpy(&codec_param.codec_ID[0], CodecID, 5);
    codec_param.direction = DataPathDirection;
    codec_param.path_ID = DataPathID;
    memcpy(&codec_param.controller_delay[0], ControllerDelay, 3);
    codec_param.codec_conf_len = CodecConfigurationLength;
    memcpy(&codec_param.codec_conf, CodecConfiguration, CodecConfigurationLength);

    LOG_INFO_APP("==>> CODEC Setup ISO Data Path\n   > cis_conn_handle 0x%04X\n   > direction %d\n   > path ID %d\n",
                 conn_handle,
                 DataPathDirection,
                 DataPathID);

    lstatus = CODEC_SetupIsoDataPath((uint8_t*)&codec_param);

    if ( lstatus == 0x00 )
    {
      LOG_INFO_APP("   > Success\n");
      if ((path->path_ID != DATA_PATH_HCI) && (path->direction == DATA_PATH_OUTPUT))
      {
        status = (uint8_t)ll_intf_set_output_data_path(conn_handle, BLE_CODEC_RxIsoDataVendorClbk);
      }
      else
      {
        status = (uint8_t)ll_intf_setup_iso_data_path( conn_handle, &ll_param );
      }
    }
    else
    {
      LOG_INFO_APP("   > Fail, reason: 0x%02X\n", lstatus);
      status = HCI_INVALID_HCI_COMMAND_PARAMETERS_ERR_CODE;
    }
  }
  else
  {
    /* HCI transparent mode */
    /* If the Host issues this command with Codec_Configuration_Length
     * non-zero and Codec_ID set to transparent air mode, the Controller
     * shall return the error code Invalid HCI Command Parameters (0x12).
     */
    if ( CodecConfigurationLength != 0 )
    {
      status = HCI_INVALID_HCI_COMMAND_PARAMETERS_ERR_CODE;
    }
    else
    {
      status = (uint8_t)ll_intf_setup_iso_data_path( conn_handle, &ll_param );
    }
  }

  if ( status != BLE_STATUS_SUCCESS )
  {
    LOG_INFO_APP("Link Layer Setup ISO Data Path failed, reason 0x%02X\n", status);
    if ( new_iso_data_path_allocated == 1 )
    {
      /* Clean the whole allocated iso data path structure */
      memset( path, 0xFFU, sizeof(iso_data_path_params_t) );
    }
    else
    {
      /* Clean only iso data path parameters on the existing structure */
      path->direction = 0xFF;
      path->coding_format = 0xFF;
      path->path_ID = 0xFF;
    }

    /* Clean codec manager if link layer command failed but codec manager status was success*/
    if ( lstatus == 0 )
    {
      CODEC_RemoveIsoDataPath(conn_handle, DataPathDirection);
    }
  }
  return status;
}

/*****************************************************************************/

uint8_t BLECB_RemoveIsoDataPath( uint16_t conn_handle, uint8_t DataPathDirection )
{
  uint8_t lstatus, status;

  LOG_INFO_APP("==>> CODEC Remove ISO Data Path\n   > cis_conn_handle 0x%04X\n   > direction mask 0x%02X\n",
              conn_handle,
              DataPathDirection);

  lstatus = CODEC_RemoveIsoDataPath(conn_handle, DataPathDirection);
  if (lstatus == 0x00)
  {
    LOG_INFO_APP("   > Success\n");
  }
  else
  {
    LOG_INFO_APP("   > Fail, reason: 0x%02X\n", lstatus);
  }

  status = (uint8_t)ll_intf_rmv_iso_data_path( conn_handle, DataPathDirection );
  if ( status == BLE_STATUS_SUCCESS )
  {
    BLE_ClearIsoDataPath( conn_handle, DataPathDirection );
  }
  else
  {
    LOG_INFO_APP("Link Layer Remove ISO Data Path failed, reason 0x%02X\n", status);
  }
  return status;
}

/*****************************************************************************/

void BLECB_SyncEvent(uint8_t group_id,
                     uint32_t next_anchor_point,
                     uint32_t time_stamp,
                     uint32_t next_sdu_delivery_timeout )
{
  AUDIO_SyncEventClbk(group_id,
                      next_anchor_point,
                      time_stamp,
                      next_sdu_delivery_timeout);
}

/*****************************************************************************/

void BLE_CodecEvent( const uint8_t* buffer )
{
  const uint8_t *evt_data = buffer + 1;

  if ( evt_data[0] == HCI_DISCONNECTION_COMPLETE_EVT_CODE )
  {
    uint8_t status = GET_U8(evt_data, 2);
    uint16_t conn_handle = GET_U16(evt_data, 3);

    if ( status == HCI_SUCCESS_ERR_CODE && BLE_GetIsoIndex(conn_handle) >= 0)
    {
      LOG_INFO_APP(">>== BLE CODEC EVT - Disconnection Complete\n");
      LOG_INFO_APP("   - cis_conn_handle:  0x%04X\n", conn_handle);

      if (AUDIO_UnregisterStream(GRP_TYPE_CIG, 0, conn_handle) == 0)
      {
        LOG_INFO_APP("==>> AUDIO_UnregisterStream() unregistered the group\n");
      }

      /* Clear stream local information */
      for ( int i = 0; i < NUM_ISO_DATA_PATH; i ++ )
      {
        iso_data_path_params_t *path = &gIso_data_path[i];

        if ( path->connection_handle == conn_handle )
        {
          memset( path, 0xFFU, sizeof(iso_data_path_params_t) );
        }
      }
    }
  }
  else if ( evt_data[0] == HCI_LE_META_EVT_CODE )
  {
    switch ( evt_data[2] ) /* subevent code */
    {
    case HCI_LE_BIG_SYNC_LOST_SUBEVT_CODE:
      {
        uint8_t big_handle = GET_U8(evt_data, 3);

        LOG_INFO_APP(">>== BLE CODEC EVT - BIG Sync Lost\n");
        LOG_INFO_APP("   - big_handle:  0x%02X\n", big_handle);

        AUDIO_UnregisterStream(GRP_TYPE_BIG, big_handle, 0);

        BLE_ClearIsoDataPathBIG( big_handle, TRUE );
      }
      break;

    case HCI_LE_BIG_SYNC_ESTABLISHED_SUBEVT_CODE:
      {
        uint8_t status = GET_U8(evt_data, 3);
        uint8_t big_handle = GET_U8(evt_data, 4);
        uint32_t transport_latency_big = GET_U24(evt_data, 5);
        uint16_t iso_interval = GET_U16(evt_data, 14);
        uint8_t num_bis = GET_U8(evt_data, 16);
        uint16_t conn_handle[NUM_OF_ISO_LINKS];

        if ( status == BLE_STATUS_SUCCESS )
        {
          for ( int i = 0; i < num_bis; i++ )
          {
            conn_handle[i] = GET_U16(evt_data, 17 + 2*i);

            BLE_RegisterIsoPath( conn_handle[i], big_handle );
          }

          LOG_INFO_APP(">>== BLE CODEC EVT - BIG Sync Established\n");
          LOG_INFO_APP("   - big_handle:  0x%02X\n", big_handle);
          LOG_INFO_APP("   - iso_interval:  0x%04X\n", iso_interval);

          AUDIO_RegisterStream(GRP_TYPE_BIG,
                              big_handle,
                              num_bis,
                              conn_handle,
                              iso_interval,
                              0,
                              transport_latency_big,
                              0);
        }
      }
      break;

    case HCI_LE_CREATE_BIG_COMPLETE_SUBEVT_CODE:
      {
        uint8_t status = GET_U8(evt_data, 3);
        uint8_t big_handle = GET_U8(evt_data, 4);
        uint32_t transport_latency_big = GET_U24(evt_data, 8);
        uint16_t iso_interval = GET_U16(evt_data, 18);
        uint8_t num_bis = GET_U8(evt_data, 20);
        uint16_t conn_handle[NUM_OF_ISO_LINKS];

        if ( status == BLE_STATUS_SUCCESS )
        {
          for ( int i = 0; i < num_bis; i++ )
          {
            conn_handle[i] = GET_U16(evt_data, 21 + 2*i);

            BLE_RegisterIsoPath( conn_handle[i], big_handle );
          }

          LOG_INFO_APP(">>== BLE CODEC EVT - Create BIG Complete\n");
          LOG_INFO_APP("   - big_handle:  0x%02X\n", big_handle);
          LOG_INFO_APP("   - iso_interval:  0x%04X\n", iso_interval);

          AUDIO_RegisterStream(GRP_TYPE_BIG,
                              big_handle,
                              num_bis,
                              conn_handle,
                              iso_interval,
                              0,
                              transport_latency_big,
                              0);
        }
      }
      break;

    case HCI_LE_TERMINATE_BIG_COMPLETE_SUBEVT_CODE:
      {
        uint8_t big_handle = GET_U8(evt_data, 3);

        LOG_INFO_APP(">>== BLE CODEC EVT - Terminate BIG Complete\n");
        LOG_INFO_APP("   - big_handle:   0x%02X\n", big_handle);

        AUDIO_UnregisterStream(GRP_TYPE_BIG, big_handle, 0);

        /* Clear the full Iso data path structure linked to the BIG */
        BLE_ClearIsoDataPathBIG( big_handle, FALSE );
      }
      break;

    case HCI_LE_CIS_ESTABLISHED_SUBEVT_CODE:
      {
        uint8_t status = GET_U8(evt_data, 3);
        uint16_t conn_handle = GET_U16(evt_data, 4);

        if ( status == BLE_STATUS_SUCCESS )
        {
          int i;
          for ( i = 0; i < NUM_ISO_DATA_PATH; i++ )
          {
            /* Check if datapath for conn_handle already exist */
            if ( (gIso_data_path)[i].connection_handle == conn_handle)
            {
              /* Don't save new conn_handle if it already exists */
              break;
            }
          }

          if ( i == NUM_ISO_DATA_PATH )
          {
            BLE_RegisterIsoPath( conn_handle, 0xFFU );
          }

          ble_intf_get_cig_info_st cig_info;
          if (ll_intf_get_cig_info (conn_handle, &cig_info) == 0)
          {
            LOG_INFO_APP(">>== BLE CODEC EVT - CIS Established\n");
            LOG_INFO_APP("   - cis_conn_handle:  0x%04X\n", conn_handle);
            LOG_INFO_APP("   - iso_interval:  0x%04X\n", cig_info.iso_interval);

            if( AUDIO_RegisterStream(GRP_TYPE_CIG,
                                     cig_info.cig_id,
                                     1,
                                     &conn_handle,
                                     cig_info.iso_interval,
                                     cig_info.role,
                                     cig_info.trsnprt_ltncy_m_to_s,
                                     cig_info.trsnprt_ltncy_s_to_m) == 0)
            {
               LOG_INFO_APP("==>> AUDIO_RegisterStream() registered new group\n");
            }
          }
        }
      }
      break;
    }
  }
}

/*****************************************************************************/

void BLECB_BigTerminateSync( uint8_t status, uint8_t big_handle )
{
  if ( status == BLE_STATUS_SUCCESS )
  {
    LOG_INFO_APP(">>== BLE CODEC CB - Terminate BIG Sync\n");
    LOG_INFO_APP("   - big_handle:  0x%02X\n", big_handle);

    AUDIO_UnregisterStream(GRP_TYPE_BIG, big_handle, 0);

    BLE_ClearIsoDataPathBIG( big_handle, TRUE );
  }
}

/*****************************************************************************/

void BLECB_TerminateBig( uint8_t status, uint8_t big_handle )
{
  if ( status == BLE_STATUS_SUCCESS )
  {
    LOG_INFO_APP(">>== BLE CODEC CB - Terminate BIG\n");
    LOG_INFO_APP("   - big_handle:  0x%02X\n", big_handle);

    AUDIO_UnregisterStream(GRP_TYPE_BIG, big_handle, 0);

    BLE_ClearIsoDataPathBIG( big_handle, FALSE );
  }
}

/*****************************************************************************/

void BLECB_IsoCalibration(uint32_t timeStamp)
{
  AUDIO_CalibrationClbk(timeStamp);
}

/*****************************************************************************/

uint8_t CODEC_CB_SendMediaPacket(uint16_t iso_con_hdl, uint8_t pb_flag, uint8_t ts_flag, uint32_t timestamp,
                                 uint16_t PSN, uint16_t iso_data_load_length, uint16_t total_sdu_len, uint8_t* pdata)
{
  return BLE_SendIsoDataToLinkLayer(iso_con_hdl,
                                   pb_flag,
                                   ts_flag,
                                   timestamp,
                                   PSN,
                                   iso_data_load_length,
                                   total_sdu_len,
                                   pdata);
}


/*****************************************************************************/

static int BLE_GetIsoIndex( uint16_t conn_handle )
{
  return ((int)conn_handle) - ll_sys_get_concurrent_state_machines_num( );
}

/*****************************************************************************/

static void BLE_ClearIsoDataPath( uint16_t conn_handle, uint8_t DataPathDirection )
{
  uint8_t mask = 0x01;

  for ( uint8_t dir = 0 ; dir < 2 ; dir++ )
  {
    if ( DataPathDirection & mask )
    {
      /* free the data */
     if (dir == DATA_PATH_OUTPUT)
     {
       BLE_ClearIsoSdu( conn_handle );
     }

      for ( uint8_t i = 0; i < NUM_ISO_DATA_PATH; i ++ )
      {
        iso_data_path_params_t *path = &gIso_data_path[i];

        if ( (path->connection_handle == conn_handle) &&
             (path->direction == dir) )
        {
          path->direction = 0xFF;
          path->coding_format = 0xFF;
          path->path_ID = 0xFF;
          break;
        }
      }
    }
    mask <<= 1;
  }
}

/*****************************************************************************/

static void BLE_ClearIsoDataPathBIG( uint8_t big_handle, uint8_t clear_iso_sdu_flag )
{
  /* Clear the full Iso data path structure linked to the BIG */
  for ( int i = 0; i < NUM_ISO_DATA_PATH; i++ )
  {
    iso_data_path_params_t *path = &gIso_data_path[i];

    if ( path->big_handle == big_handle )
    {
      if ( clear_iso_sdu_flag )
      {
        /* Clear ISO SDU data pending in list */
        BLE_ClearIsoSdu( path->connection_handle );
      }

      memset( path, 0xFFU, sizeof(iso_data_path_params_t) );
    }
  }
}

/*****************************************************************************/

static void BLE_RegisterIsoPath( uint16_t conn_handle, uint8_t big_handle )
{
  /* Save Big Handle to iso_data_path_params_t */
  for ( int i = 0; i < NUM_ISO_DATA_PATH; i++ )
  {
    iso_data_path_params_t *path = &gIso_data_path[i];

    if ( (path->big_handle == 0xFFU) &&
         (path->connection_handle == 0xFFFF) )
    {
      path->big_handle = big_handle;
      path->connection_handle = conn_handle;
      break;
    }
  }
}

/*****************************************************************************
--------------------------ISO RX Flow Control--------------------------------
*****************************************************************************/

static void BLE_ClearIsoSdu( uint16_t conn_handle )
{
  uint8_t index_list = (uint8_t)BLE_GetIsoIndex( conn_handle );
  iso_sdu_t *isoSDU;

  /* Check the index value */
  if ( index_list < NUM_OF_ISO_LINKS )
  {
    ll_sys_disable_irq();

    /* Clear the ISO SDU data stored */
    while ( !LST_is_empty( &gIso_sdu_list[index_list] ) )
    {
      /* Get free node */
      isoSDU = (iso_sdu_t*)gIso_sdu_list[index_list].next;

      ll_intf_free_iso_sdu( &isoSDU->iso_buff );
      /* ISO Sdu sent. The ISO SDU data can be removed from the list
        and insert back in the pool list */
      LST_remove_node( (tListNode*)isoSDU );
      LST_insert_tail( &gIso_sdu_pool_list, (tListNode*)isoSDU );
    }

    ll_sys_enable_irq();
  }
}


/*****************************************************************************/

static void BLE_AddIsoSDU( iso_sdu_buf_hdr_st* iso_buff_received,
                           uint16_t conn_handle )
{
  iso_sdu_t *newIsoSDU;
  uint8_t index_list = (uint8_t)BLE_GetIsoIndex( conn_handle );

  /* Check if there is ISO SDU node available, otherwise discard the ISO SDU
   */
  if ( !LST_is_empty( &gIso_sdu_pool_list ) )
  {
    /* Get free node */
    newIsoSDU = (iso_sdu_t*)gIso_sdu_pool_list.next;

    /* Check the index value */
    if ( index_list >= NUM_OF_ISO_LINKS )
    {
      /* Wrong Conn Handle, then discard the ISO SDU */
      ll_intf_free_iso_sdu( iso_buff_received );
    }
    else
    {
      /* Insert in the transmit list */
      memcpy( &newIsoSDU->iso_buff, iso_buff_received, sizeof(iso_sdu_buf_hdr_st) );
      newIsoSDU->connHandle = conn_handle;
      /* Remove node from pool list */
      LST_remove_node( (tListNode*)newIsoSDU );
      /* Insert node in ISO SDU List */
      LST_insert_tail( &gIso_sdu_list[index_list], (tListNode*)newIsoSDU );
    }
  }
  else
  {
    /* Discard the ISO SDU */
    ll_intf_free_iso_sdu( iso_buff_received );
  }
}

/*****************************************************************************/

static uint8_t BLE_SendIsoSDUtoCodec( const iso_sdu_buf_hdr_p iso_buff, uint16_t conn_handle )
{
  iso_sdu_buf_hdr_st* sdu_ptr = iso_buff;
  uint8_t l_status = CODEC_RCV_STATUS_OK;

  while ( sdu_ptr != NULL )
  {
    uint8_t ts_flag = 0;
    uint32_t *data;

    if ( (sdu_ptr == iso_buff) &&
         ((iso_buff->pb_flag == FIRST_SDU_FRAG) ||
          (iso_buff->pb_flag == FULL_SDU_FRAG)) )
    {
      ts_flag = 1;
      data = sdu_ptr->ptr_sdu_buffer + 3;
    }
    else
    {
      data = sdu_ptr->ptr_sdu_buffer + 2;
    }

    l_status = CODEC_ReceiveMediaPacket( conn_handle,
                                          sdu_ptr->pb_flag,
                                          ts_flag,
                                          sdu_ptr->time_stamp,
                                          sdu_ptr->pkt_sqnc_num,
                                          sdu_ptr->pkt_status_flag,
                                          sdu_ptr->iso_sdu_len,
                                          (uint8_t*)data );

    if ( l_status != CODEC_RCV_STATUS_OK )
    {
      /* Send the ISO SDU data to upper layer. */
      break;
    }

    sdu_ptr = sdu_ptr->ptr_nxt_sdu_buff_hdr;
  }

  return l_status;
}

/*****************************************************************************/

void CODEC_CB_ReceiveMediaPacketReady( uint16_t conn_handle )
{
  uint8_t l_status = CODEC_RCV_STATUS_OK;
  uint8_t index_list = (uint8_t)BLE_GetIsoIndex( conn_handle );
  iso_sdu_t *isoSDU;

  /* Check the index value */
  if ( index_list < NUM_OF_ISO_LINKS )
  {
    while( (l_status == CODEC_RCV_STATUS_OK) &&
           (!LST_is_empty( &gIso_sdu_list[index_list] )) )
    {
      /* Get free node */
      isoSDU = (iso_sdu_t*)gIso_sdu_list[index_list].next;

      l_status = BLE_SendIsoSDUtoCodec( &isoSDU->iso_buff, conn_handle );

      if ( l_status != CODEC_RCV_STATUS_FAIL )
      {
        /* ISO SDU data has been sent to the upper layer,
           the LL memory can be freed */
        ll_intf_free_iso_sdu( &isoSDU->iso_buff );
        /* ISO Sdu sent. The ISO SDU data can be removed from the list
           and insert back in the pool list */
        LST_remove_node( (tListNode*)isoSDU );
        LST_insert_tail( &gIso_sdu_pool_list, (tListNode*)isoSDU );
      }
    }

    if ( LST_is_empty( &gIso_sdu_list[index_list] ) &&
         (l_status == CODEC_RCV_STATUS_OK) )
    {
      /* Change the status of the ISO Data list to idle */
      gIso_sdu_status[index_list] = ISO_SDU_STATUS_IDLE;
    }
  }
}

/*****************************************************************************/

static void BLE_CODEC_RxIsoDataVendorClbk( const iso_sdu_buf_hdr_p iso_buff, uint16_t conn_handle )
{
  uint8_t l_status = CODEC_RCV_STATUS_OK;
  uint8_t index_list = (uint8_t)BLE_GetIsoIndex( conn_handle );

  if ( gIso_sdu_status[index_list] == ISO_SDU_STATUS_IDLE )
  {
    /* Send the ISO SDU data to upper layer. */
    l_status = BLE_SendIsoSDUtoCodec( iso_buff, conn_handle );

    if ( l_status == CODEC_RCV_STATUS_FAIL )
    {
      /* If upper layer didn't handle the ISO SDU,
       * the ISO SDU is saved to be resent later
       */
      BLE_AddIsoSDU( iso_buff, conn_handle );
      gIso_sdu_status[index_list] = ISO_SDU_STATUS_BUSY;
    }
    else if ( l_status == CODEC_RCV_STATUS_BUSY )
    {
      /* If upper layer handled the ISO SDU,
       * but cannot handle anymore ISO SDU
       */
      gIso_sdu_status[index_list] = ISO_SDU_STATUS_BUSY;
      ll_intf_free_iso_sdu( iso_buff );
    }
    else
    {
      /* ISO SDU data has been sent to the upper layer,
       * the LL memory can be freed
       */
      ll_intf_free_iso_sdu( iso_buff );
    }
  }
  else
  {
    /* If upper layer cannot handle the ISO SDU,
     * the ISO SDU is saved to be sent later
     */
    BLE_AddIsoSDU( iso_buff, conn_handle );
  }
}

/*****************************************************************************/
/**
  * @brief Data coming from Host through HCI interface
  */

uint8_t BLECB_SendIsoData( uint16_t conn_handle,
                           uint8_t pb_flag,
                           uint8_t ts_flag,
                           uint32_t timestamp,
                           uint16_t PSN,
                           uint16_t iso_data_load_length,
                           uint16_t total_sdu_length,
                           uint8_t* iso_data )
{
  uint8_t status = BLE_STATUS_FAILED;

  for ( uint8_t i = 0; i < NUM_ISO_DATA_PATH; i ++ )
  {
    iso_data_path_params_t *path = &gIso_data_path[i];

    if ( (path->connection_handle == conn_handle) &&
         (path->direction == DATA_PATH_INPUT) &&
         (path->path_ID == DATA_PATH_HCI))
    {
      if ((path->coding_format != AUDIO_CODING_FORMAT_TRANSPARENT ))
      {
        /* Data need to pass through the codec by calling CODEC_SendData() */
        /* But the vendor specific data path should rather be used for latency management */
        status = BLE_STATUS_PENDING;
      }
      else
      {
        /* Data is directly send to the Link Layer, no need to pass through the codec */
        status = BLE_STATUS_SUCCESS;
      }
      break;
    }
  }
  return status;
}

/*****************************************************************************/

/**
  * @brief Data coming from LL - non vendor specific (HCI)
  */
uint8_t BLECB_IsHciRxIsoDataPathOn( uint16_t conn_handle )
{
  for ( int i = 0; i < NUM_ISO_DATA_PATH; i ++ )
  {
    iso_data_path_params_t *path = &gIso_data_path[i];

    if ( (path->connection_handle == conn_handle) &&
         (path->direction == DATA_PATH_OUTPUT) &&
         (path->path_ID == DATA_PATH_HCI) )
    {
      return TRUE;
    }
  }

  return FALSE;
}

/*****************************************************************************/
