/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gatt_client_app.c
  * @author  MCD Application Team
  * @brief   GATT Client Application
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "log_module.h"
#include "app_common.h"
#include "dbg_trace.h"
#include "ble_core.h"
#include "uuid.h"
#include "ble_conf.h"
#include "svc_ctl.h"
#include "gatt_client_app.h"
#include "stm32_seq.h"
#include "app_ble.h"
#include "app_conf.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_bsp.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

typedef struct
{
  uint8_t selection;
  uint8_t level;
}PrivacyButton_t;
/* USER CODE END PTD */

typedef enum
{
  NOTIFICATION_INFO_RECEIVED_EVT,
  /* USER CODE BEGIN GATT_CLIENT_APP_Opcode_t */
  PRIVACY_NOTIFICATION_INFO_RECEIVED_EVT,
  /* USER CODE END GATT_CLIENT_APP_Opcode_t */
}GATT_CLIENT_APP_Opcode_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t length;
}GATT_CLIENT_APP_Data_t;

typedef struct
{
  GATT_CLIENT_APP_Opcode_t Client_Evt_Opcode;
  GATT_CLIENT_APP_Data_t   DataTransfered;
}GATT_CLIENT_APP_Notification_evt_t;

typedef enum
{
  SVC_ID_GAP = 0,
  SVC_ID_GATT,
  /* USER CODE BEGIN svcId_t */
  SVC_ID_PRIVACY,
  /* USER CODE END svcId_t */
  SVC_ID_MAX
}svcId_t;

typedef enum
{
  SVC_INFO_STATUS_EMPTY = 0,
  SVC_INFO_STATUS_FOUND,
  SVC_INFO_STATUS_DISCOVERED,
  SVC_INFO_STATUS_ERROR,
  /* USER CODE BEGIN svcInfoStatus_t */

  /* USER CODE END svcInfoStatus_t */
  SVC_INFO_STATUS_MAX
}svcInfoStatus_t;

typedef struct
{
  uint16_t uuid;

  uint16_t charStartHdl;
  uint16_t charEndHdl;
  uint16_t descStartHdl;
  uint16_t descEndHdl;

  svcInfoStatus_t status;
  uint8_t findIncludedServices; /* Set to TRUE to search for included services */
  /* USER CODE BEGIN SvcInfo_t */

  /* USER CODE END SvcInfo_t */

}SvcInfo_t;

typedef struct
{
  GATT_CLIENT_APP_State_t state;
  uint16_t connHdl;
  uint16_t att_mtu;

  SvcInfo_t svcInfo[SVC_ID_MAX];

  uint16_t ServiceChangedCharValueHdl;
  uint16_t ServiceChangedCharDescHdl;
  uint8_t ServiceChangedCharProperties;
  /* USER CODE BEGIN BleClientAppContext_t */

  /* SVC_ID_P2P characteristics handle */
  /* handles of the Tx characteristic - Write To Server */
  uint16_t PrivacyWriteToServerCharHdl;
  uint16_t PrivacyWriteToServerValueHdl;
  uint16_t PrivacyWriteToServerDescHdl;
  /* handles of the Rx characteristic - Notification From Server */
  uint16_t PrivacyNotificationCharHdl;
  uint16_t PrivacyNotificationValueHdl;
  uint16_t PrivacyNotificationDescHdl;
  /* USER CODE END BleClientAppContext_t */

}BleClientAppContext_t;

typedef enum
{
  PROC_GATT_EXCHANGE_CONFIG,
  PROC_GATT_DISC_ALL_PRIMARY_SERVICES,
  PROC_GATT_FIND_INCLUDED_SERVICES,
  PROC_GATT_DISC_ALL_CHARS,
  PROC_GATT_DISC_ALL_DESCS,
  PROC_GATT_PROPERTIES_ENABLE_ALL,
  /* USER CODE BEGIN ProcGattId_t */

  /* USER CODE END ProcGattId_t */
}ProcGattId_t;

/* Context for the current GATT procedure. One procedure at a time is supported. */
typedef struct
{
  ProcGattId_t current_gatt_proc;
  uint8_t att_error_code;
  uint8_t gatt_error_code;
  uint16_t read_char_len;
  uint8_t read_char[512];
  uint16_t read_char_offset;
}BleClientAppProcContext_t;

/* Private defines ------------------------------------------------------------*/
#define BLE_DEFAULT_ATT_MTU                                                   23
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros -------------------------------------------------------------*/
#define UNPACK_2_BYTE_PARAMETER(ptr)  \
        (uint16_t)((uint16_t)(*((uint8_t *)ptr))) |   \
        (uint16_t)((((uint16_t)(*((uint8_t *)ptr + 1))) << 8))
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

static BleClientAppContext_t a_ClientContext[CFG_BLE_NUM_CLIENT_CONTEXTS];
static BleClientAppProcContext_t procClientContext;

/* USER CODE BEGIN PV */
static uint8_t PrivacyLedSelection;
static uint8_t PrivacyLedLevel;

static PrivacyButton_t PrivacyButtonData;
/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/
/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private function prototypes -----------------------------------------------*/
static SVCCTL_EvtAckStatus_t Event_Handler(void *Event);
static void gatt_parse_services(aci_att_read_by_group_type_resp_event_rp0 *p_evt);
static void gatt_parse_services_by_UUID(aci_att_find_by_type_value_resp_event_rp0 *p_evt);
static void gatt_parse_included_serv(aci_att_read_by_type_resp_event_rp0 *p_evt);
static void gatt_parse_chars(aci_att_read_by_type_resp_event_rp0 *p_evt);
static void gatt_parse_descs(aci_att_find_info_resp_event_rp0 *p_evt);
static int gatt_parse_notification(aci_gatt_notification_event_rp0 *p_evt);
static int gatt_parse_indication(aci_gatt_indication_event_rp0 *p_evt);
static void gatt_Notification(GATT_CLIENT_APP_Notification_evt_t *p_Notif);
static void client_discover_task(void);
static void gatt_cmd_resp_release(void);
static uint8_t gatt_cmd_resp_wait(void);
static uint8_t gatt_procedure(uint8_t index, ProcGattId_t GattProcId);
/* USER CODE BEGIN PFP */
static void Privacy_client_write_char(void);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void GATT_CLIENT_APP_Init(void)
{
  uint8_t index =0, svcIndex=0;
  /* USER CODE BEGIN GATT_CLIENT_APP_Init_1 */
  
  /* USER CODE END GATT_CLIENT_APP_Init_1 */
  for(index = 0; index < CFG_BLE_NUM_CLIENT_CONTEXTS; index++)
  {
    /* USER CODE BEGIN GATT_CLIENT_APP_Init_5 */

    /* USER CODE END GATT_CLIENT_APP_Init_5 */

    a_ClientContext[index].connHdl = 0xFFFF;
    a_ClientContext[index].state = GATT_CLIENT_APP_DISCONNECTED;

    for(svcIndex = 0; svcIndex < SVC_ID_MAX; svcIndex++)
    {
      a_ClientContext[index].svcInfo[svcIndex].uuid = 0;
      a_ClientContext[index].svcInfo[svcIndex].charStartHdl = 0;
      a_ClientContext[index].svcInfo[svcIndex].charEndHdl = 0;
      a_ClientContext[index].svcInfo[svcIndex].descStartHdl = 0;
      a_ClientContext[index].svcInfo[svcIndex].descEndHdl = 0;
      a_ClientContext[index].svcInfo[svcIndex].status = SVC_INFO_STATUS_EMPTY;
      /* USER CODE BEGIN GATT_CLIENT_APP_Init_3 */

      /* USER CODE END GATT_CLIENT_APP_Init_3 */
    }

    a_ClientContext[index].svcInfo[SVC_ID_GAP].uuid  = GAP_SERVICE_UUID;
    a_ClientContext[index].svcInfo[SVC_ID_GATT].uuid = GATT_SERVICE_UUID;
    /* USER CODE BEGIN GATT_CLIENT_APP_Init_4 */
    a_ClientContext[index].svcInfo[SVC_ID_PRIVACY].uuid  = ST_PRIVACY_SERVICE_UUID;
    /* USER CODE END GATT_CLIENT_APP_Init_4 */
  }

  /* Register the event handler to the BLE controller */
  SVCCTL_RegisterCltHandler(Event_Handler);

  /* Register a task allowing to discover all services and characteristics and enable all notifications */
  UTIL_SEQ_RegTask(1U << CFG_TASK_DISCOVER_SERVICES_ID, UTIL_SEQ_RFU, client_discover_task);

  /* USER CODE BEGIN GATT_CLIENT_APP_Init_2 */
  /* USER CODE BEGIN GATT_CLIENT_APP_Init_2 */
  UTIL_SEQ_RegTask(1U << CFG_TASK_WRITE_CHAR_ID, UTIL_SEQ_RFU, Privacy_client_write_char);

  PrivacyLedSelection = 0x00;
  PrivacyLedLevel = 0x00;

  PrivacyButtonData.selection = 0x01;
  PrivacyButtonData.level = 0x00;

  APP_DBG_MSG("-- CENTRAL INITIALIZED \n");
  /* USER CODE END GATT_CLIENT_APP_Init_2 */
  return;
}

void GATT_CLIENT_APP_Notification(GATT_CLIENT_APP_ConnHandle_Notif_evt_t *p_Notif)
{
  /* USER CODE BEGIN GATT_CLIENT_APP_Notification_1 */

  /* USER CODE END GATT_CLIENT_APP_Notification_1 */
  switch(p_Notif->ConnOpcode)
  {
    /* USER CODE BEGIN ConnOpcode */

    /* USER CODE END ConnOpcode */

    case PEER_CONN_HANDLE_EVT :
    {
      uint8_t index;

      for(index = 0; index < CFG_BLE_NUM_CLIENT_CONTEXTS; index++)
      {
        if(a_ClientContext[index].state == GATT_CLIENT_APP_DISCONNECTED)
        {
          /* USER CODE BEGIN PEER_CONN_HANDLE_EVT_1 */

          /* USER CODE END PEER_CONN_HANDLE_EVT_1 */

          a_ClientContext[index].connHdl = p_Notif->ConnHdl;
          a_ClientContext[index].state = GATT_CLIENT_APP_CONNECTED;
          a_ClientContext[index].att_mtu = BLE_DEFAULT_ATT_MTU;

          /* USER CODE BEGIN PEER_CONN_HANDLE_EVT_2 */

          /* USER CODE END PEER_CONN_HANDLE_EVT_2 */

          break;
        }
      }
      if(index == CFG_BLE_NUM_CLIENT_CONTEXTS)
      {
        LOG_ERROR_APP("Error: reached maximum number of connected servers!\n");
        /* USER CODE BEGIN PEER_CONN_HANDLE_EVT_3 */

        /* USER CODE END PEER_CONN_HANDLE_EVT_3 */
      }
    }
    break;

    case PEER_DISCON_HANDLE_EVT :
    {
      for(uint8_t index = 0; index < CFG_BLE_NUM_CLIENT_CONTEXTS; index++)
      {
        if(a_ClientContext[index].connHdl == p_Notif->ConnHdl)
        {
          /* USER CODE BEGIN PEER_DISCON_HANDLE_EVT_1 */

          /* USER CODE END PEER_DISCON_HANDLE_EVT_1 */

          a_ClientContext[index].connHdl = 0xFFFF;
          a_ClientContext[index].state = GATT_CLIENT_APP_DISCONNECTED;

          /* USER CODE BEGIN PEER_DISCON_HANDLE_EVT_2 */

          /* USER CODE END PEER_DISCON_HANDLE_EVT_2 */

          break;
        }
      }
    }
    break;

    default:
    /* USER CODE BEGIN ConnOpcode_Default */

    /* USER CODE END ConnOpcode_Default */
    break;
  }
  /* USER CODE BEGIN GATT_CLIENT_APP_Notification_2 */

  /* USER CODE END GATT_CLIENT_APP_Notification_2 */
  return;
}

/* Deprecated */
uint8_t GATT_CLIENT_APP_Set_Conn_Handle(uint8_t index, uint16_t connHdl)
{
  uint8_t ret;

  if (index < CFG_BLE_NUM_CLIENT_CONTEXTS)
  {
    a_ClientContext[index].connHdl = connHdl;
    a_ClientContext[index].state = GATT_CLIENT_APP_CONNECTED;
    ret = 0;
  }
  else
  {
    ret = 1;
  }

  return ret;
}

int GATT_CLIENT_APP_Get_Index(uint16_t connHdl)
{
  uint8_t index;

  if(connHdl > 0x0EFF)
  {
    return -1;
  }

  for (index = 0 ; index < CFG_BLE_NUM_CLIENT_CONTEXTS ; index++)
  {
    if (a_ClientContext[index].connHdl == connHdl)
    {
      return index;
    }
  }

  return -1;
}

uint8_t GATT_CLIENT_APP_Get_State(uint8_t index)
{
  return a_ClientContext[index].state;
}

void GATT_CLIENT_APP_DiscoverServices(uint16_t connection_handle, uint8_t start_task)
{
  for(uint8_t index = 0; index < CFG_BLE_NUM_CLIENT_CONTEXTS; index++)
  {
    if(a_ClientContext[index].connHdl == connection_handle)
    {
      /* USER CODE BEGIN GATT_CLIENT_APP_Discover_services_1 */

      /* USER CODE END GATT_CLIENT_APP_Discover_services_1 */
      if(start_task)
      {
        a_ClientContext[index].state = GATT_CLIENT_APP_DISCOVER_SERVICES;

        UTIL_SEQ_SetTask( 1U << CFG_TASK_DISCOVER_SERVICES_ID, CFG_SEQ_PRIO_0);

      }
      else
      {
        a_ClientContext[index].state = GATT_CLIENT_APP_CONNECTED;

        GATT_CLIENT_APP_DiscoverServicesWithIndex(index);
      }

      /* USER CODE BEGIN GATT_CLIENT_APP_Discover_services_2 */

      /* USER CODE END GATT_CLIENT_APP_Discover_services_2 */

      break;
    }
  }

  return;
}

void GATT_CLIENT_APP_DiscoverServicesWithIndex(uint8_t index)
{
  gatt_procedure(index, PROC_GATT_EXCHANGE_CONFIG);
  gatt_procedure(index, PROC_GATT_DISC_ALL_PRIMARY_SERVICES);
  gatt_procedure(index, PROC_GATT_FIND_INCLUDED_SERVICES);
  gatt_procedure(index, PROC_GATT_DISC_ALL_CHARS);
  gatt_procedure(index, PROC_GATT_DISC_ALL_DESCS);
  gatt_procedure(index, PROC_GATT_PROPERTIES_ENABLE_ALL);
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

 static uint8_t gatt_procedure(uint8_t index, ProcGattId_t GattProcId)
{
  uint16_t svcIdx;
  tBleStatus bleStatus = BLE_STATUS_SUCCESS;
  uint8_t status;

  if (index >= CFG_BLE_NUM_CLIENT_CONTEXTS)
  {
    status = 1;
  }
  else
  {
    status = 0;

    UTIL_SEQ_ClrEvt(1U << CFG_EVENT_PROC_GATT_COMPLETE);

    procClientContext.current_gatt_proc = GattProcId;

    switch (GattProcId)
    {
      case PROC_GATT_EXCHANGE_CONFIG:
      {
        procClientContext.gatt_error_code = 0;

        LOG_INFO_APP("GATT exchange configuration\n");
        status = aci_gatt_exchange_config(a_ClientContext[index].connHdl);
        if (bleStatus == BLE_STATUS_SUCCESS && gatt_cmd_resp_wait() == BLE_STATUS_SUCCESS)
        {
          LOG_INFO_APP("Completed Successfully\n\n");
        }
        else
        {
          LOG_ERROR_APP("Failed, status=0x%02X, result=0x%02X\n\n", bleStatus, procClientContext.gatt_error_code);
        }
      }
      break; /* PROC_GATT_EXCHANGE_CONFIG */
      case PROC_GATT_DISC_ALL_PRIMARY_SERVICES:
      {
        procClientContext.gatt_error_code = 0;

        LOG_INFO_APP("GATT services discovery\n");
        bleStatus = aci_gatt_disc_all_primary_services(a_ClientContext[index].connHdl);

        if (bleStatus == BLE_STATUS_SUCCESS && gatt_cmd_resp_wait() == BLE_STATUS_SUCCESS)
        {
          LOG_INFO_APP("PROC_GATT_DISC_ALL_PRIMARY_SERVICES services discovered Successfully\n\n");
        }
        else
        {
          LOG_ERROR_APP("PROC_GATT_DISC_ALL_PRIMARY_SERVICES aci_gatt_disc_all_primary_services cmd Failed, status=0x%02X, result=0x%02X\n\n", bleStatus, procClientContext.gatt_error_code);
          status++;
        }
      }
      break; /* PROC_GATT_DISC_ALL_PRIMARY_SERVICES */

      case PROC_GATT_FIND_INCLUDED_SERVICES:
      {
        LOG_INFO_APP("GATT Find Included Services\n");
        for (svcIdx = 0; svcIdx < SVC_ID_MAX; svcIdx++)
        {
          if(a_ClientContext[index].svcInfo[svcIdx].charStartHdl != 0x0000
             && a_ClientContext[index].svcInfo[svcIdx].findIncludedServices)
          {
            procClientContext.gatt_error_code = 0;

            LOG_INFO_APP("  Service UUID 0x%04X, svcHdl[0x%04X - 0x%04X]\n",
                         a_ClientContext[index].svcInfo[svcIdx].uuid,
                         a_ClientContext[index].svcInfo[svcIdx].charStartHdl,
                         a_ClientContext[index].svcInfo[svcIdx].charEndHdl);

            bleStatus = aci_gatt_find_included_services(a_ClientContext[index].connHdl,
                                                        a_ClientContext[index].svcInfo[svcIdx].charStartHdl,
                                                        a_ClientContext[index].svcInfo[svcIdx].charEndHdl);

            if (bleStatus == BLE_STATUS_SUCCESS && gatt_cmd_resp_wait() == BLE_STATUS_SUCCESS)
            {
              LOG_INFO_APP("  Service UUID 0x%04X included service found Successfully\n\n",
                           a_ClientContext[index].svcInfo[svcIdx].uuid);
              a_ClientContext[index].svcInfo[svcIdx].status = SVC_INFO_STATUS_DISCOVERED;
            }
            else
            {
              LOG_ERROR_APP("  Service UUID 0x%04X included service found Failed, status =0x%02X, result=0x%02X\n\n",
                            a_ClientContext[index].svcInfo[svcIdx].uuid, bleStatus, procClientContext.gatt_error_code);
              status++;
            }
            /* USER CODE BEGIN PROC_GATT_FIND_INCLUDED_SERVICES_0 */

            /* USER CODE END PROC_GATT_FIND_INCLUDED_SERVICES_0 */
          }
        }
        /* USER CODE BEGIN PROC_GATT_FIND_INCLUDED_SERVICES */

        /* USER CODE END PROC_GATT_FIND_INCLUDED_SERVICES */
        if (status == 0)
        {
          LOG_INFO_APP("Included service discovery ended successfully\n\n");
        }
        else
        {
          LOG_ERROR_APP("Included service discovery failed\n\n");
        }
      }
      break; /* PROC_GATT_FIND_INCLUDED_SERVICES */

      case PROC_GATT_DISC_ALL_CHARS:
      {
        LOG_INFO_APP("DISCOVER_ALL_CHARS on ConnHdl=0x%04X\n",a_ClientContext[index].connHdl);
        for (svcIdx = 0; svcIdx < SVC_ID_MAX; svcIdx++)
        {
          if(a_ClientContext[index].svcInfo[svcIdx].charStartHdl != 0x0000
             && a_ClientContext[index].svcInfo[svcIdx].uuid != 0)
          {
            procClientContext.gatt_error_code = 0;

            LOG_INFO_APP("  Service UUID 0x%04X, svcHdl[0x%04X - 0x%04X]\n",
                              a_ClientContext[index].svcInfo[svcIdx].uuid,
                              a_ClientContext[index].svcInfo[svcIdx].charStartHdl,
                              a_ClientContext[index].svcInfo[svcIdx].charEndHdl);
            bleStatus = aci_gatt_disc_all_char_of_service(
                              a_ClientContext[index].connHdl,
                              a_ClientContext[index].svcInfo[svcIdx].charStartHdl,
                              a_ClientContext[index].svcInfo[svcIdx].charEndHdl);

            if (bleStatus == BLE_STATUS_SUCCESS && gatt_cmd_resp_wait() == BLE_STATUS_SUCCESS)
            {
              LOG_INFO_APP("  Service UUID 0x%04X characteristics discovered Successfully\n\n",
                           a_ClientContext[index].svcInfo[svcIdx].uuid);
              a_ClientContext[index].svcInfo[svcIdx].status = SVC_INFO_STATUS_DISCOVERED;
            }
            else
            {
              LOG_ERROR_APP("  Service UUID 0x%04X characteristics discovery Failed, status=0x%02X, result=0x%02X\n\n",
                            a_ClientContext[index].svcInfo[svcIdx].uuid, bleStatus, procClientContext.gatt_error_code);
              status++;
            }
            /* USER CODE BEGIN PROC_GATT_DISC_ALL_CHARS_0 */

            /* USER CODE END PROC_GATT_DISC_ALL_CHARS_0 */
          }
        }
        /* USER CODE BEGIN PROC_GATT_DISC_ALL_CHARS */

        /* USER CODE END PROC_GATT_DISC_ALL_CHARS */
        if (status == 0)
        {
          LOG_INFO_APP("All characteristics discovered Successfully \n\n");
        }
        else
        {
          LOG_ERROR_APP("All characteristics discovery Failed \n\n");
        }
      }
      break; /* PROC_GATT_DISC_ALL_CHARS */

      case PROC_GATT_DISC_ALL_DESCS:
      {
        LOG_INFO_APP("DISCOVER_ALL_CHAR_DESCS on ConnHdl=0x%04X\n",
                              a_ClientContext[index].connHdl);
        for (svcIdx = 0; svcIdx < SVC_ID_MAX; svcIdx++)
        {
          if(a_ClientContext[index].svcInfo[svcIdx].charStartHdl != 0x0000
             && a_ClientContext[index].svcInfo[svcIdx].uuid != 0)
          {
            procClientContext.gatt_error_code = 0;

            LOG_INFO_APP("  Service UUID 0x%04X, svcHdl[0x%04X - 0x%04X]\n",
                              a_ClientContext[index].svcInfo[svcIdx].uuid,
                              a_ClientContext[index].svcInfo[svcIdx].charStartHdl,
                              a_ClientContext[index].svcInfo[svcIdx].charEndHdl);
            bleStatus = aci_gatt_disc_all_char_desc(
                              a_ClientContext[index].connHdl,
                              a_ClientContext[index].svcInfo[svcIdx].charStartHdl,
                              a_ClientContext[index].svcInfo[svcIdx].charEndHdl);

            if (bleStatus == BLE_STATUS_SUCCESS && gatt_cmd_resp_wait() == BLE_STATUS_SUCCESS)
            {
              LOG_INFO_APP("  Descriptors discovered Successfully\n\n");
              a_ClientContext[index].svcInfo[svcIdx].status = SVC_INFO_STATUS_DISCOVERED;
            }
            else
            {
              LOG_ERROR_APP("  Descriptors discovery Failed, status=0x%02X, result=0x%02X\n\n", bleStatus, procClientContext.gatt_error_code);
              status++;
            }
            /* USER CODE BEGIN PROC_GATT_DISC_ALL_DESCS_0 */
            if (a_ClientContext[index].svcInfo[svcIdx].status == SVC_INFO_STATUS_DISCOVERED)
            {
              UTIL_SEQ_SetTask(1<<CFG_TASK_CENTRAL_CONN_INTERV_UPDATE_ID, CFG_SEQ_PRIO_0);
            }
            /* USER CODE END PROC_GATT_DISC_ALL_DESCS_0 */
          }
        }
        /* USER CODE BEGIN PROC_GATT_DISC_ALL_DESCS */

        /* USER CODE END PROC_GATT_DISC_ALL_DESCS */
        if (status == 0)
        {
          LOG_INFO_APP("All descriptors discovered Successfully \n\n");
        }
        else
        {
          LOG_ERROR_APP("All descriptors discovery Failed \n\n");
        }
      }
      break; /* PROC_GATT_DISC_ALL_DESCS */
      case PROC_GATT_PROPERTIES_ENABLE_ALL:
      {
        uint16_t charPropVal = 0x0000;

        if (a_ClientContext[index].ServiceChangedCharDescHdl != 0x0000)
        {
          if(((a_ClientContext[index].ServiceChangedCharProperties) & CHAR_PROP_NOTIFY) == CHAR_PROP_NOTIFY)
          {
            charPropVal = 0x0001;
          }
          else if(((a_ClientContext[index].ServiceChangedCharProperties) & CHAR_PROP_INDICATE) == CHAR_PROP_INDICATE)
          {
            charPropVal = 0x0002;
          }

          procClientContext.gatt_error_code = 0;
          bleStatus = aci_gatt_write_char_value(a_ClientContext[index].connHdl,
                                                a_ClientContext[index].ServiceChangedCharDescHdl,
                                                2,
                                                (uint8_t *) &charPropVal);
          if (bleStatus == BLE_STATUS_SUCCESS && gatt_cmd_resp_wait() == BLE_STATUS_SUCCESS)
          {
            LOG_INFO_APP(" ServiceChangedCharDescHdl =0x%04X\n",a_ClientContext[index].ServiceChangedCharDescHdl);
          }
          else
          {
            LOG_ERROR_APP(" ServiceChangedCharDescHdl write Failed, status =0x%02X, result=0x%02X\n", bleStatus, procClientContext.gatt_error_code);
            status++;
          }
        }
        /* USER CODE BEGIN PROC_GATT_PROPERTIES_ENABLE_ALL */
        if(a_ClientContext[index].PrivacyNotificationDescHdl != 0x0000)
        {
          charPropVal = 0x0001;
          bleStatus |= aci_gatt_write_char_value(a_ClientContext[index].connHdl,
                                                 a_ClientContext[index].PrivacyNotificationDescHdl,
                                                 2,
                                                 (uint8_t *) &charPropVal);
          if (bleStatus == BLE_STATUS_SUCCESS)
          {
          gatt_cmd_resp_wait();
          APP_DBG_MSG(" SecurityNotificationDescHdl =0x%04X\n",a_ClientContext[index].PrivacyNotificationDescHdl);
          }
          else
          {
            LOG_ERROR_APP(" SecurityNotificationDescHdl write Failed, status =0x%02X\n", bleStatus);
            status++;
          }
        }
        /* USER CODE END PROC_GATT_PROPERTIES_ENABLE_ALL */

        if (status == 0)
        {
          LOG_INFO_APP("All properties enabled Successfully \n\n");
        }
        else
        {
          LOG_ERROR_APP("All properties enabled Failed \n\n");
        }
      }
      break; /* PROC_GATT_PROPERTIES_ENABLE_ALL */

      /* USER CODE BEGIN GattProcId */

      /* USER CODE END GattProcId */

    default:
      break;
    }
  }

  return status;
}

/**
 * @brief  Event handler
 * @param  Event: Address of the buffer holding the Event
 * @retval Ack: Return whether the Event has been managed or not
 */
static SVCCTL_EvtAckStatus_t Event_Handler(void *Event)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *event_pckt;
  evt_blecore_aci *p_blecore_evt;

  GATT_CLIENT_APP_Notification_evt_t Notification;
  UNUSED(Notification);

  return_value = SVCCTL_EvtNotAck;
  event_pckt = (hci_event_pckt *)(((hci_uart_pckt*)Event)->data);

  switch (event_pckt->evt)
  {
    case HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE:
    {
      p_blecore_evt = (evt_blecore_aci*)event_pckt->data;
      switch (p_blecore_evt->ecode)
      {
        case ACI_ATT_READ_BY_GROUP_TYPE_RESP_VSEVT_CODE:
        {
          aci_att_read_by_group_type_resp_event_rp0 *p_evt_rsp = (void*)p_blecore_evt->data;
          gatt_parse_services(p_evt_rsp);
        }
        break; /* ACI_ATT_READ_BY_GROUP_TYPE_RESP_VSEVT_CODE */
        case ACI_ATT_FIND_BY_TYPE_VALUE_RESP_VSEVT_CODE:
        {
          aci_att_find_by_type_value_resp_event_rp0 *p_evt_rsp = (void*) p_blecore_evt->data;
          gatt_parse_services_by_UUID(p_evt_rsp);
        }
        break; /* ACI_ATT_FIND_BY_TYPE_VALUE_RESP_VSEVT_CODE */
        case ACI_ATT_READ_BY_TYPE_RESP_VSEVT_CODE:
        {
          aci_att_read_by_type_resp_event_rp0 *p_evt_rsp = (void*)p_blecore_evt->data;
          if(procClientContext.current_gatt_proc == PROC_GATT_FIND_INCLUDED_SERVICES)
          {
            gatt_parse_included_serv(p_evt_rsp);
          }
          else if(procClientContext.current_gatt_proc == PROC_GATT_DISC_ALL_CHARS)
          {
            gatt_parse_chars(p_evt_rsp);
          }
        }
        break; /* ACI_ATT_READ_BY_TYPE_RESP_VSEVT_CODE */
        case ACI_ATT_FIND_INFO_RESP_VSEVT_CODE:
        {
          aci_att_find_info_resp_event_rp0 *p_evt_rsp = (void*)p_blecore_evt->data;
          gatt_parse_descs(p_evt_rsp);
        }
        break; /* ACI_ATT_FIND_INFO_RESP_VSEVT_CODE */
        case ACI_GATT_NOTIFICATION_VSEVT_CODE:
        {
          aci_gatt_notification_event_rp0 *p_evt_rsp = (void*)p_blecore_evt->data;
          if(gatt_parse_notification(p_evt_rsp) == 0)
          {
            return_value = SVCCTL_EvtAckFlowEnable;
          }
        }
        break;/* ACI_GATT_NOTIFICATION_VSEVT_CODE */
        case ACI_GATT_INDICATION_VSEVT_CODE:
        {
          aci_gatt_indication_event_rp0 *p_evt_rsp = (void*)p_blecore_evt->data;
          if(gatt_parse_indication(p_evt_rsp) == 0)
          {
            return_value = SVCCTL_EvtAckFlowEnable;
          }
        }
        break;
        case ACI_GATT_PROC_COMPLETE_VSEVT_CODE:
        {
          LOG_DEBUG_APP(">>== ACI_GATT_PROC_COMPLETE_VSEVT_CODE\n");
          aci_gatt_proc_complete_event_rp0 *p_evt_rsp = (void*)p_blecore_evt->data;
          if(p_evt_rsp->Error_Code != BLE_STATUS_SUCCESS)
          {
            LOG_ERROR_APP("GATT procedure ended unsuccessfully, error code: 0x%02X\n",
                          p_evt_rsp->Error_Code);
          }
          else
          {
            LOG_DEBUG_APP("GATT Procedure completed successfully\n");
          }
          procClientContext.gatt_error_code = p_evt_rsp->Error_Code;
          gatt_cmd_resp_release();
        }
        break;/* ACI_GATT_PROC_COMPLETE_VSEVT_CODE */
        case ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE:
        {
          aci_gatt_tx_pool_available_event_rp0 *tx_pool_available;
          tx_pool_available = (aci_gatt_tx_pool_available_event_rp0 *)p_blecore_evt->data;
          UNUSED(tx_pool_available);
          /* USER CODE BEGIN ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */
          APP_DBG_MSG(">>== Connection Handle = %d \n",tx_pool_available->Connection_Handle );
          APP_DBG_MSG(">>== Available buffers = %d \n",tx_pool_available->Available_Buffers );
          APP_DBG_MSG("\n");
          /* USER CODE END ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */
        }
        break;/* ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE*/
        case ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE:
        {
          int index;
          aci_att_exchange_mtu_resp_event_rp0 * exchange_mtu_resp;
          exchange_mtu_resp = (aci_att_exchange_mtu_resp_event_rp0 *)p_blecore_evt->data;
          LOG_INFO_APP("  MTU exchanged size = %d\n",exchange_mtu_resp->Server_RX_MTU );
          index = GATT_CLIENT_APP_Get_Index(exchange_mtu_resp->Connection_Handle);
          if(index >= 0)
          {
            a_ClientContext[index].att_mtu = exchange_mtu_resp->Server_RX_MTU;
          }
          /* USER CODE BEGIN ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
          tBleStatus result;
	  result = hci_le_set_data_length(a_ClientContext[0].connHdl,251,2120);
          if (result != BLE_STATUS_SUCCESS)
          {
            APP_DBG_MSG("  Fail   : set data length command   : error code: 0x%x \n\r", result);
          }
          else
          {
            APP_DBG_MSG("  Success: set data length command  \n\r");
          }

          /* USER CODE END ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
        }
        break;

        /* USER CODE BEGIN VENDOR_SPECIFIC_DEBUG_EVT_CODE_1 */

        /* USER CODE END VENDOR_SPECIFIC_DEBUG_EVT_CODE_1 */
        default:
        /* USER CODE BEGIN VENDOR_SPECIFIC_DEBUG_EVT_CODE_DEFAULT */

        /* USER CODE END VENDOR_SPECIFIC_DEBUG_EVT_CODE_DEFAULT */
        break;
      }/* end switch (p_blecore_evt->ecode) */
    }
    break; /* HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE */
    /* USER CODE BEGIN GATT_CLIENT_EVENT_PACKET_1 */

    /* USER CODE END GATT_CLIENT_EVENT_PACKET_1 */
    default:
    /* USER CODE BEGIN GATT_CLIENT_EVENT_PACKET_DEFAULT */

    /* USER CODE END GATT_CLIENT_EVENT_PACKET_DEFAULT */
    break;
  }/* end switch (event_pckt->evt) */

  return(return_value);
}

__USED static void gatt_Notification(GATT_CLIENT_APP_Notification_evt_t *p_Notif)
{
  /* USER CODE BEGIN gatt_Notification_1 */

  /* USER CODE END gatt_Notification_1 */
  switch (p_Notif->Client_Evt_Opcode)
  {
    /* USER CODE BEGIN Client_Evt_Opcode */
    case PRIVACY_NOTIFICATION_INFO_RECEIVED_EVT:
    {
      PrivacyLedSelection = p_Notif->DataTransfered.p_Payload[0];
      UNUSED(PrivacyLedSelection);
      PrivacyLedLevel = p_Notif->DataTransfered.p_Payload[1];
      
      if (PrivacyLedLevel == 0x00)
      {
        #if (CFG_LED_SUPPORTED == 1)
        APP_BSP_LED_Off(LED_BLUE);
		#endif
        APP_DBG_MSG("  PRIVACY APPLICATION CLIENT : NOTIFICATION RECEIVED - LED OFF \n");
      }
      else
      {
        #if (CFG_LED_SUPPORTED == 1)
        APP_BSP_LED_On(LED_BLUE);
		#endif
        APP_DBG_MSG("  PRIVACY APPLICATION CLIENT : NOTIFICATION RECEIVED - LED ON\n");
      }
    
     
      break;
    }
    /* USER CODE END Client_Evt_Opcode */

    case NOTIFICATION_INFO_RECEIVED_EVT:
      /* USER CODE BEGIN NOTIFICATION_INFO_RECEIVED_EVT */

      /* USER CODE END NOTIFICATION_INFO_RECEIVED_EVT */
      break;

    default:
      /* USER CODE BEGIN Client_Evt_Opcode_Default */

      /* USER CODE END Client_Evt_Opcode_Default */
      break;
  }
  /* USER CODE BEGIN gatt_Notification_2 */

  /* USER CODE END gatt_Notification_2 */
  return;
}

/**
* function of GATT service parse
*/
static void gatt_parse_services(aci_att_read_by_group_type_resp_event_rp0 *p_evt)
{
  uint16_t uuid, ServiceStartHdl, ServiceEndHdl;
  uint8_t uuid_offset, uuid_size = 0U, uuid_short_offset = 0U;
  uint8_t i, j, idx, numServ, index;

  LOG_INFO_APP("ACI_ATT_READ_BY_GROUP_TYPE_RESP_VSEVT_CODE - ConnHdl=0x%04X\n",
                p_evt->Connection_Handle);

  for (index = 0 ; index < CFG_BLE_NUM_CLIENT_CONTEXTS ; index++)
  {
    if (a_ClientContext[index].connHdl == p_evt->Connection_Handle)
    {
      break;
    }
  }

  /* index < CFG_BLE_NUM_CLIENT_CONTEXTS means connection handle identified */
  if (index < CFG_BLE_NUM_CLIENT_CONTEXTS)
  {
    /* Number of attribute value tuples */
    numServ = (p_evt->Data_Length) / p_evt->Attribute_Data_Length;

    /* event data in Attribute_Data_List contains:
    * 2 bytes for start handle
    * 2 bytes for end handle
    * 2 or 16 bytes data for UUID
    */
    uuid_offset = 4;           /*UUID offset in bytes in Attribute_Data_List */
    if (p_evt->Attribute_Data_Length == 20) /* we are interested in the UUID is 128 bit.*/
    {
      idx = 16;                /*UUID index of 2 bytes read part in Attribute_Data_List */
      uuid_size = 16;          /*UUID size in bytes */
      uuid_short_offset = 12;  /*UUID offset of 2 bytes read part in UUID field*/
    }
    if (p_evt->Attribute_Data_Length == 6) /* we are interested in the UUID is 16 bit.*/
    {
      idx = 4;
      uuid_size = 2;
      uuid_short_offset = 0;
    }
    UNUSED(idx);
    UNUSED(uuid_size);

    /* Loop on number of attribute value tuples */
    for (i = 0; i < numServ; i++)
    {
      ServiceStartHdl = UNPACK_2_BYTE_PARAMETER(&p_evt->Attribute_Data_List[uuid_offset - 4]);
      ServiceEndHdl   = UNPACK_2_BYTE_PARAMETER(&p_evt->Attribute_Data_List[uuid_offset - 2]);
      uuid            = UNPACK_2_BYTE_PARAMETER(&p_evt->Attribute_Data_List[uuid_offset + uuid_short_offset]);
      LOG_INFO_APP("  %d/%d short UUID=0x%04X, handle [0x%04X - 0x%04X]",
                   i + 1, numServ, uuid, ServiceStartHdl,ServiceEndHdl);

      for (j = 0; j < SVC_ID_MAX; j++)
      {
        if(uuid == a_ClientContext[index].svcInfo[j].uuid)
        {
          a_ClientContext[index].svcInfo[j].charStartHdl  = ServiceStartHdl;
          a_ClientContext[index].svcInfo[j].charEndHdl    = ServiceEndHdl;
          a_ClientContext[index].svcInfo[j].status        = SVC_INFO_STATUS_FOUND;
          LOG_INFO_APP(", UUID 0x%04X found",a_ClientContext[index].svcInfo[j].uuid);
        }
        /* USER CODE BEGIN gatt_parse_services_2 */

        /* USER CODE END gatt_parse_services_2 */
      }
      LOG_INFO_APP("\n");
      /* USER CODE BEGIN gatt_parse_services_1 */

      /* USER CODE END gatt_parse_services_1 */

      uuid_offset += p_evt->Attribute_Data_Length;
    }
  }
  else
  {
    LOG_INFO_APP("ACI_ATT_READ_BY_GROUP_TYPE_RESP_VSEVT_CODE, failed no free index in connection table !\n");
  }

  return;
}

/**
* function of GATT service parse by UUID
*/
static void gatt_parse_services_by_UUID(aci_att_find_by_type_value_resp_event_rp0 *p_evt)
{
  uint8_t i;

  LOG_INFO_APP("ACI_ATT_FIND_BY_TYPE_VALUE_RESP_VSEVT_CODE - ConnHdl=0x%04X, Num_of_Handle_Pair=%d\n",
                p_evt->Connection_Handle,
                p_evt->Num_of_Handle_Pair);

  for(i = 0 ; i < p_evt->Num_of_Handle_Pair ; i++)
  {
    LOG_INFO_APP("ACI_ATT_FIND_BY_TYPE_VALUE_RESP_VSEVT_CODE - PaitId=%d Found_Attribute_Handle=0x%04X, Group_End_Handle=0x%04X\n",
                  i,
                  p_evt->Attribute_Group_Handle_Pair[i].Found_Attribute_Handle,
                  p_evt->Attribute_Group_Handle_Pair[i].Group_End_Handle);
  }

/* USER CODE BEGIN gatt_parse_services_by_UUID_1 */

/* USER CODE END gatt_parse_services_by_UUID_1 */

  return;
}

/**
* function for parsing included services
*/
static void gatt_parse_included_serv(aci_att_read_by_type_resp_event_rp0 *p_evt)
{
  uint16_t uuid = 0, ServiceStartHdl, ServiceEndHdl;
  uint8_t start_handle_offset;
  uint8_t i, numHdlValuePair, index;

  LOG_INFO_APP("  ACI_ATT_READ_BY_TYPE_RESP_VSEVT_CODE - ConnHdl=0x%04X\n",
                p_evt->Connection_Handle);

  for (index = 0 ; index < CFG_BLE_NUM_CLIENT_CONTEXTS ; index++)
  {
    if (a_ClientContext[index].connHdl == p_evt->Connection_Handle)
    {
      break;
    }
  }

  /* index < CFG_BLE_NUM_CLIENT_CONTEXTS means connection handle identified */
  if (index < CFG_BLE_NUM_CLIENT_CONTEXTS)
  {
    /* event data in Attribute_Data_List contains:
    * 2 bytes for start handle
    * 1 byte char properties
    * 2 bytes handle
    * 2 or 16 bytes data for UUID
    */

    /* Number of attribute value tuples */
    numHdlValuePair = p_evt->Data_Length / p_evt->Handle_Value_Pair_Length;

    start_handle_offset = 2;           /* Included Service Attribute Handle offset in bytes in Attribute_Data_List */

    LOG_INFO_APP("  number of value tuples = %d\n", numHdlValuePair);
    /* Loop on number of attribute value tuples */
    for (i = 0; i < numHdlValuePair; i++)
    {
      if (p_evt->Handle_Value_Pair_Length == 8) /* UUID is included */
      {
        uuid = UNPACK_2_BYTE_PARAMETER(&p_evt->Handle_Value_Pair_Data[start_handle_offset + 4]);
      }
      ServiceStartHdl = UNPACK_2_BYTE_PARAMETER(&p_evt->Handle_Value_Pair_Data[start_handle_offset]);
      ServiceEndHdl = UNPACK_2_BYTE_PARAMETER(&p_evt->Handle_Value_Pair_Data[start_handle_offset + 2]);

      LOG_INFO_APP("    %d/%d handles [0x%04X - 0x%04X]",
                     i + 1, numHdlValuePair, ServiceStartHdl, ServiceEndHdl);

      if (uuid != 0x0) /* Supported only 16-bit UUIDs */
      {
        LOG_INFO_APP(", UUID=0x%04X", uuid);
        for (int j = 0; j < SVC_ID_MAX; j++)
        {
          if(uuid == a_ClientContext[index].svcInfo[j].uuid)
          {
            a_ClientContext[index].svcInfo[j].charStartHdl  = ServiceStartHdl;
            a_ClientContext[index].svcInfo[j].charEndHdl    = ServiceEndHdl;
            a_ClientContext[index].svcInfo[j].status        = SVC_INFO_STATUS_FOUND;
            LOG_INFO_APP(", found");
          }
        }
      }

      LOG_INFO_APP("\n");

      start_handle_offset += p_evt->Handle_Value_Pair_Length;
    }
  }
  else
  {
    LOG_INFO_APP("  ACI_ATT_READ_BY_TYPE_RESP_VSEVT_CODE, failed handle not found in connection table !\n");
  }

  return;
}

/**
* function of GATT characteristics parse
*/
static void gatt_parse_chars(aci_att_read_by_type_resp_event_rp0 *p_evt)
{
  uint16_t uuid, CharStartHdl, CharValueHdl;
  uint8_t uuid_offset, uuid_size = 0U, uuid_short_offset = 0U;
  uint8_t i, idx, numHdlValuePair, index;
  uint8_t CharProperties;

  LOG_INFO_APP("  ACI_ATT_READ_BY_TYPE_RESP_VSEVT_CODE - ConnHdl=0x%04X\n",
                p_evt->Connection_Handle);

  for (index = 0 ; index < CFG_BLE_NUM_CLIENT_CONTEXTS ; index++)
  {
    if (a_ClientContext[index].connHdl == p_evt->Connection_Handle)
    {
      break;
    }
  }

  /* index < CFG_BLE_NUM_CLIENT_CONTEXTS means connection handle identified */
  if (index < CFG_BLE_NUM_CLIENT_CONTEXTS)
  {
    /* event data in Attribute_Data_List contains:
    * 2 bytes for start handle
    * 1 byte char properties
    * 2 bytes handle
    * 2 or 16 bytes data for UUID
    */

    /* Number of attribute value tuples */
    numHdlValuePair = p_evt->Data_Length / p_evt->Handle_Value_Pair_Length;

    uuid_offset = 5;           /* UUID offset in bytes in Attribute_Data_List */
    if (p_evt->Handle_Value_Pair_Length == 21) /* we are interested in  128 bit UUIDs */
    {
      idx = 17;                /* UUID index of 2 bytes read part in Attribute_Data_List */
      uuid_size = 16;          /* UUID size in bytes */
      uuid_short_offset = 12;  /* UUID offset of 2 bytes read part in UUID field */
    }
    if (p_evt->Handle_Value_Pair_Length == 7) /* we are interested in  16 bit UUIDs */
    {
      idx = 5;
      uuid_size = 2;
      uuid_short_offset = 0;
    }
    UNUSED(idx);
    UNUSED(uuid_size);

    p_evt->Data_Length -= 1;

    LOG_INFO_APP("  number of value tuples = %d\n", numHdlValuePair);
    /* Loop on number of attribute value tuples */
    for (i = 0; i < numHdlValuePair; i++)
    {
      CharStartHdl = UNPACK_2_BYTE_PARAMETER(&p_evt->Handle_Value_Pair_Data[uuid_offset - 5]);
      CharProperties = p_evt->Handle_Value_Pair_Data[uuid_offset - 3];
      CharValueHdl = UNPACK_2_BYTE_PARAMETER(&p_evt->Handle_Value_Pair_Data[uuid_offset - 2]);
      uuid = UNPACK_2_BYTE_PARAMETER(&p_evt->Handle_Value_Pair_Data[uuid_offset + uuid_short_offset]);

      if ( (uuid != 0x0) && (CharProperties != 0x0) && (CharStartHdl != 0x0) && (CharValueHdl != 0) )
      {
        LOG_INFO_APP("    %d/%d short UUID=0x%04X, Properties=0x%04X, CharHandle [0x%04X - 0x%04X]",
                     i + 1, numHdlValuePair, uuid, CharProperties, CharStartHdl, CharValueHdl);

        if (uuid == DEVICE_NAME_UUID)
        {
          LOG_INFO_APP(", GAP DEVICE_NAME charac found\n");
        }
        else if (uuid == APPEARANCE_UUID)
        {
          LOG_INFO_APP(", GAP APPEARANCE charac found\n");
        }
        else if (uuid == SERVICE_CHANGED_CHARACTERISTIC_UUID)
        {
          a_ClientContext[index].ServiceChangedCharValueHdl = CharValueHdl;
          LOG_INFO_APP(", GATT SERVICE_CHANGED_CHARACTERISTIC_UUID charac found\n");
        }
/* USER CODE BEGIN gatt_parse_chars_1 */
        else if (uuid == ST_PRIVACY_NOTIFY_CHAR_UUID)
        {
          a_ClientContext[index].PrivacyNotificationCharHdl = CharStartHdl;
          a_ClientContext[index].PrivacyNotificationValueHdl = CharValueHdl;
          LOG_INFO_APP(", ST_PRIVACY_NOTIFY_CHAR_UUID charac found\n");
        }
        else if (uuid == ST_PRIVACY_WRITE_CHAR_UUID)
        {
          a_ClientContext[index].PrivacyWriteToServerCharHdl = CharStartHdl;
          a_ClientContext[index].PrivacyWriteToServerValueHdl = CharValueHdl;
          LOG_INFO_APP(", ST_PRIVACY_WRITE_CHAR_UUID charac found\n");
        }
/* USER CODE END gatt_parse_chars_1 */
        else
        {
          LOG_INFO_APP("\n");
        }

      }
      uuid_offset += p_evt->Handle_Value_Pair_Length;
    }
  }
  else
  {
    LOG_INFO_APP("  ACI_ATT_READ_BY_TYPE_RESP_VSEVT_CODE, failed handle not found in connection table !\n");
  }

  return;
}
/**
* function of GATT descriptor parse
*/
static void gatt_parse_descs(aci_att_find_info_resp_event_rp0 *p_evt)
{
  uint16_t uuid, handle;
  uint8_t uuid_offset, uuid_size, uuid_short_offset, handle_uuid_pair_size;
  uint8_t i, numDesc, index;
  static uint16_t gattCharStartHdl = 0;
  static uint16_t gattCharValueHdl = 0;

  LOG_INFO_APP("  ACI_ATT_FIND_INFO_RESP_VSEVT_CODE - ConnHdl=0x%04X\n",
              p_evt->Connection_Handle);

  for (index = 0 ; index < CFG_BLE_NUM_CLIENT_CONTEXTS ; index++)
  {
    if (a_ClientContext[index].connHdl == p_evt->Connection_Handle)
    {
      break;
    }
  }

  /* index < CFG_BLE_NUM_CLIENT_CONTEXTS means connection handle identified */
  if (index < CFG_BLE_NUM_CLIENT_CONTEXTS)
  {
    /* event data in Attribute_Data_List contains:
    * 2 bytes handle
    * 2 or 16 bytes data for UUID
    */
    uuid_offset = 2;
    if (p_evt->Format == UUID_TYPE_16)
    {
      uuid_size = 2;
      uuid_short_offset = 0;
      handle_uuid_pair_size = 4;
    }
    else if (p_evt->Format == UUID_TYPE_128)
    {
      uuid_size = 16;
      uuid_short_offset = 12;
      handle_uuid_pair_size = 18;
    }
    else
    {
      return;
    }
    UNUSED(uuid_size);

    /* Number of handle uuid pairs */
    numDesc = (p_evt->Event_Data_Length) / handle_uuid_pair_size;

    for (i = 0; i < numDesc; i++)
    {
      handle = UNPACK_2_BYTE_PARAMETER(&p_evt->Handle_UUID_Pair[uuid_offset - 2]);
      uuid = UNPACK_2_BYTE_PARAMETER(&p_evt->Handle_UUID_Pair[uuid_offset + uuid_short_offset]);

      if (uuid == PRIMARY_SERVICE_UUID)
      {
        LOG_INFO_APP("    Primary service UUID=0x%04X, handle=0x%04X\n", uuid, handle);
      }
      else if (uuid == CHARACTERISTIC_UUID)
      {
        gattCharStartHdl = 0;
        gattCharValueHdl = 0;

        gattCharStartHdl = handle;
        LOG_INFO_APP("    Charac UUID=0x%04X, handle=0x%04X\n", uuid, handle);
      }
      else if ( (uuid == CHAR_EXTENDED_PROPERTIES_DESCRIPTOR_UUID)
             || (uuid == CLIENT_CHAR_CONFIG_DESCRIPTOR_UUID) )
      {

        LOG_INFO_APP("    Descriptor UUID=0x%04X, handle=0x%04X",
                      uuid, handle);
        if (a_ClientContext[index].ServiceChangedCharValueHdl == gattCharValueHdl)
        {
          a_ClientContext[index].ServiceChangedCharDescHdl = handle;
          LOG_INFO_APP(", Service Changed found\n");
        }
/* USER CODE BEGIN gatt_parse_descs_1 */
        else if (a_ClientContext[index].PrivacyWriteToServerValueHdl == gattCharValueHdl)
        {
          a_ClientContext[index].PrivacyWriteToServerDescHdl = handle;
          APP_DBG_MSG("PrivacyWrite found : Desc UUID=0x%04X handle=0x%04X-0x%04X-0x%04X\n",
                      uuid, gattCharStartHdl, gattCharValueHdl, handle);
        }
        else if (a_ClientContext[index].PrivacyNotificationValueHdl == gattCharValueHdl)
        {
          a_ClientContext[index].PrivacyNotificationDescHdl = handle;
          APP_DBG_MSG("PrivacyNotification found : Desc UUID=0x%04X handle=0x%04X-0x%04X-0x%04X\n",
                      uuid, gattCharStartHdl, gattCharValueHdl, handle);
        }
/* USER CODE END gatt_parse_descs_1 */
        else
        {
          LOG_INFO_APP("\n");
        }
        UNUSED(gattCharStartHdl);
      }
      else
      {
        gattCharValueHdl = handle;

        LOG_INFO_APP("    UUID=0x%04X, handle=0x%04X", uuid, handle);

        if (uuid == DEVICE_NAME_UUID)
        {
          LOG_INFO_APP(", found GAP DEVICE_NAME_UUID\n");
        }
        else if (uuid == APPEARANCE_UUID)
        {
          LOG_INFO_APP(", found GAP APPEARANCE_UUID\n");
        }
        else if (uuid == SERVICE_CHANGED_CHARACTERISTIC_UUID)
        {
          LOG_INFO_APP(", found GATT SERVICE_CHANGED_CHARACTERISTIC_UUID\n");
        }
/* USER CODE BEGIN gatt_parse_descs_2 */
        else if (uuid == ST_PRIVACY_WRITE_CHAR_UUID)
        {
          APP_DBG_MSG(", found ST_PRIVACY_WRITE_CHAR_UUID\n");
        }
        else if (uuid == ST_PRIVACY_NOTIFY_CHAR_UUID)
        {
          APP_DBG_MSG(", found ST_PRIVACY_NOTIFY_CHAR_UUID\n");
        }
/* USER CODE END gatt_parse_descs_2 */
        else
        {
          LOG_INFO_APP("\n");
        }
      }
    uuid_offset += handle_uuid_pair_size;
    }
  }
  else
  {
    LOG_INFO_APP("ACI_ATT_FIND_INFO_RESP_VSEVT_CODE, failed handle not found in connection table !\n");
  }

  return;
}

static int gatt_parse_notification(aci_gatt_notification_event_rp0 *p_evt)
{
  int ret = -1;

  LOG_DEBUG_APP("ACI_GATT_NOTIFICATION_VSEVT_CODE - ConnHdl=0x%04X, Attribute_Handle=0x%04X\n",
                p_evt->Connection_Handle,
                p_evt->Attribute_Handle);
/* USER CODE BEGIN gatt_parse_notification_1 */
  GATT_CLIENT_APP_Notification_evt_t Notification;
  uint8_t index;

  for (index = 0 ; index < CFG_BLE_NUM_CLIENT_CONTEXTS ; index++)
  {
    if (a_ClientContext[index].connHdl == p_evt->Connection_Handle)
    {
      break;
    }
  }

  /* index < CFG_BLE_NUM_CLIENT_CONTEXTS means connection handle identified */
  if (index < CFG_BLE_NUM_CLIENT_CONTEXTS)
  {
    if (p_evt->Attribute_Handle == a_ClientContext[index].PrivacyNotificationValueHdl)
    {
      LOG_INFO_APP("  Incoming Nofification received P2P Server information\n");
      Notification.Client_Evt_Opcode = PRIVACY_NOTIFICATION_INFO_RECEIVED_EVT;
      Notification.DataTransfered.length = p_evt->Attribute_Value_Length;
      Notification.DataTransfered.p_Payload = &p_evt->Attribute_Value[0];

      gatt_Notification(&Notification);
    }
  }
  else
  {
    LOG_INFO_APP("ACI_GATT_NOTIFICATION_VSEVT_CODE, failed handle not found in connection table !\n");
  }
/* USER CODE END gatt_parse_notification_1 */

  return ret;
}

static int gatt_parse_indication(aci_gatt_indication_event_rp0 *p_evt)
{
  int ret = -1;

  LOG_DEBUG_APP("ACI_GATT_CLT_INDICATION_VSEVT_CODE - ConnHdl=0x%04X, Attribute_Handle=0x%04X\n",
                p_evt->Connection_Handle,
                p_evt->Attribute_Handle);

  /* USER CODE BEGIN gatt_parse_indication_1 */

  /* USER CODE END gatt_parse_indication_1 */

  return ret;
}

static void client_discover_task(void)
{
  uint8_t index;

  for(index = 0; index < CFG_BLE_NUM_CLIENT_CONTEXTS; index++)
  {
    if(a_ClientContext[index].state == GATT_CLIENT_APP_DISCOVER_SERVICES)
    {
      a_ClientContext[index].state = GATT_CLIENT_APP_CONNECTED;

      /* USER CODE BEGIN client_discover_1 */

      /* USER CODE END client_discover_1 */

      GATT_CLIENT_APP_DiscoverServicesWithIndex(index);

      /* USER CODE BEGIN client_discover_2 */

      /* USER CODE END client_discover_2 */

      /* Check if in the meantime another server has been connected. */
      UTIL_SEQ_SetTask( 1U << CFG_TASK_DISCOVER_SERVICES_ID, CFG_SEQ_PRIO_0);

      break;
    }
  }

  return;
}

static void gatt_cmd_resp_release(void)
{
  UTIL_SEQ_SetEvt(1U << CFG_EVENT_PROC_GATT_COMPLETE);
  return;
}

static uint8_t gatt_cmd_resp_wait(void)
{
  UTIL_SEQ_WaitEvt(1U << CFG_EVENT_PROC_GATT_COMPLETE);
  return procClientContext.gatt_error_code;
}

static uint8_t ReadLongCharacteristic(uint16_t connection_handle, uint16_t ValueHdl, uint16_t Offset)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;

  LOG_INFO_APP("Read Long\n");

  procClientContext.gatt_error_code = 0;
  procClientContext.att_error_code = 0;

  /* Display Information characteristic */
  ret = aci_gatt_read_long_char_value(connection_handle,
                                      ValueHdl,
                                      Offset);
  if (ret != BLE_STATUS_SUCCESS || (ret = gatt_cmd_resp_wait()) != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("aci_gatt_read_long_char_value failed, connHdl=0x%04X, ValueHdl=0x%04X, ret = 0x%02X\n",
                connection_handle,
                ValueHdl,
                ret);
    return ret;
  }

  LOG_INFO_APP("aci_gatt_read_long_char_value, connHdl=0x%04X, ValueHdl=0x%04X\n",
              connection_handle,
              ValueHdl);

  return procClientContext.gatt_error_code;
}

tBleStatus GATT_CLIENT_APP_ReadCharacteristic(uint16_t connection_handle, uint16_t ValueHdl, uint16_t *data_length_p, uint8_t **data_p, uint8_t use_read_long)
{
  int index;
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;

  index = GATT_CLIENT_APP_Get_Index(connection_handle);
  if(index < 0)
    return ret;

  /* USER CODE BEGIN GATT_CLIENT_APP_ReadCharacteristic_1 */

  /* USER CODE END GATT_CLIENT_APP_ReadCharacteristic_1 */

  procClientContext.read_char_offset = 0;
  procClientContext.gatt_error_code = 0;
  procClientContext.att_error_code = 0;

  UTIL_SEQ_ClrEvt(1U << CFG_EVENT_PROC_GATT_COMPLETE);

  LOG_DEBUG_APP("aci_gatt_read_char_value, connHdl=0x%04X, ValueHdl=0x%04X\n",
                connection_handle,
                ValueHdl);

  ret = aci_gatt_read_char_value(connection_handle,
                                 ValueHdl);

  if (ret != BLE_STATUS_SUCCESS || (ret = gatt_cmd_resp_wait()) != BLE_STATUS_SUCCESS)
  {
    LOG_ERROR_APP("aci_gatt_read_char_value failed, connHdl=0x%04X, ValueHdl=0x%04X\n",
                connection_handle,
                ValueHdl);
    return ret;
  }

  if (use_read_long && procClientContext.read_char_len >= (a_ClientContext[index].att_mtu - 1))
  {
    ret = ReadLongCharacteristic(connection_handle, ValueHdl, procClientContext.read_char_len);
  }

  if(ret != BLE_STATUS_SUCCESS && procClientContext.att_error_code != 0x0B) /* 0x0B: Attribute not long */
  {
    *data_p = NULL;
    *data_length_p = 0;
  }
  else
  {
    *data_p = procClientContext.read_char;
    *data_length_p = procClientContext.read_char_len;
  }

  /* USER CODE BEGIN GATT_CLIENT_APP_ReadCharacteristic_2 */

  /* USER CODE END GATT_CLIENT_APP_ReadCharacteristic_2 */

  return ret;
}

tBleStatus GATT_CLIENT_APP_WriteCharacteristic(uint16_t connection_handle, uint16_t ValueHdl, uint16_t data_length, uint8_t *data)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;

  /* USER CODE BEGIN GATT_CLIENT_APP_WriteCharacteristic_1 */

  /* USER CODE END GATT_CLIENT_APP_WriteCharacteristic_1 */

  procClientContext.gatt_error_code = 0;
  procClientContext.att_error_code = 0;

  UTIL_SEQ_ClrEvt(1U << CFG_EVENT_PROC_GATT_COMPLETE);

  ret = aci_gatt_write_char_value(connection_handle,
                                  ValueHdl,
                                  data_length,
                                  data);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_ERROR_APP("aci_gatt_write_char_value failed, connHdl=0x%04X, ValueHdl=0x%04X\n",
                  connection_handle,
                  ValueHdl);
    return ret;
  }
  else
  {
    LOG_DEBUG_APP("aci_gatt_write_char_value, connHdl=0x%04X, ValueHdl=0x%04X\n",
                  connection_handle,
                  ValueHdl);
  }
  /* wait until a gatt procedure complete is received */
  gatt_cmd_resp_wait();

  /* USER CODE BEGIN GATT_CLIENT_APP_WriteCharacteristic_2 */

  /* USER CODE END GATT_CLIENT_APP_WriteCharacteristic_2 */

  return procClientContext.gatt_error_code;
}

/* USER CODE BEGIN LF */
static void Privacy_client_write_char(void)
{
  /* Specific implementation for CFG_TASK_WRITE_CHAR_ID */
  
  uint8_t index = 0;
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;

  if (PrivacyButtonData.level == 0x00)
  {
    PrivacyButtonData.level = 0x01;
  }
  else
  {
    PrivacyButtonData.level = 0x00;
  }

  for(index = 0; index < CFG_BLE_NUM_CLIENT_CONTEXTS; index++)
  {
    if (a_ClientContext[index].state == GATT_CLIENT_APP_CONNECTED)
    {
      ret = aci_gatt_write_without_resp(a_ClientContext[index].connHdl,
                                            a_ClientContext[index].PrivacyWriteToServerValueHdl,
                                            2,
                                            (uint8_t *)&PrivacyButtonData);

      if (ret != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("aci_gatt_clt_write_without_resp failed, connHdl=0x%04X, ValueHdl=0x%04X\n",
                    a_ClientContext[index].connHdl,
                    a_ClientContext[index].PrivacyWriteToServerValueHdl);
      }
      else
      {
        APP_DBG_MSG("aci_gatt_clt_write_without_resp success, connHdl=0x%04X, ValueHdl=0x%04X\n",
                    a_ClientContext[index].connHdl,
                    a_ClientContext[index].PrivacyWriteToServerValueHdl);
      }
      
    }
  }

  return;
}
/* USER CODE END LF */
