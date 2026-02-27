/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gatt_client_app.c
  * @author  MCD Application Team
  * @brief   GATT Client Application
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "esl_profile_ap.h"
#include "time_ref.h"
#include "otp_client.h"
#include "stm32_timer.h"
#include "nvm_db.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */
#define ECP_TIMEOUT_MS         (30 * 1000)  // 30 seconds
/* USER CODE END PTD */

typedef enum
{
  NOTIFICATION_INFO_RECEIVED_EVT,
  /* USER CODE BEGIN GATT_CLIENT_APP_Opcode_t */
  ESL_NOTIFICATION_INFO_RECEIVED_EVT
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
  SVC_ID_ESL,
  SVC_ID_OTS,
  SVC_ID_DIS,
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

  /* SVC_ID_ESL characteristics handle */
  /* Handles of ESL Address characteristic */
  uint16_t ESLAddressCharHdl;
  uint16_t ESLAddressValueHdl;
  
  /* Handles of AP Sync Material characteristic */
  uint16_t APSyncKeyMaterialCharHdl;
  uint16_t APSyncKeyMaterialValueHdl;
  
  /* Handles of ESL Resp Key Material characteristic */
  uint16_t ESLRespKeyMaterialCharHdl;
  uint16_t ESLRespKeyMaterialValueHdl;
  
  /* Handles of ESL Current Absolute Time characteristic */
  uint16_t ESLCurrAbsTimeCharHdl;
  uint16_t ESLCurrAbsTimeValueHdl;
  
  /* handles of the Rx characteristic - Notification From Server */
  /* Handles of ESL Control Point characteristic */
  uint16_t ESLControlPointCharHdl;
  uint16_t ESLControlPointValueHdl;
  uint16_t ESLControlPointCCCDHdl;
    
  /* Handles of ESL Display Information characteristic */
  uint16_t ESLDisplayInfoCharHdl;
  uint16_t ESLDisplayInfoValueHdl;
  
  /* Handles of ESL Image Information characteristic */
  uint16_t ESLImageInfoCharHdl;
  uint16_t ESLImageInfoValueHdl;

  /* Handles of ESL LED Information characteristic */
  uint16_t ESLSensorInfoCharHdl;
  uint16_t ESLSensorInfoValueHdl;
  
  /* Handles of ESL LED Information characteristic */
  uint16_t ESLLedInfoCharHdl;
  uint16_t ESLLedInfoValueHdl;
  
  OTSHandleContext_t OTSHandles;
  
  uint16_t DISPNPIdCharHdl;
  uint16_t DISPNPIdValueHdl;
  
  /* ESL Control Point (ECP) timeout timerID*/
  UTIL_TIMER_Object_t ECP_timer_Id;
  
  bool b_ECP_failed; 
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

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/
/* USER CODE BEGIN GV */
extern ESL_AP_context_t ESL_AP_Context;
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
static void context_init(uint8_t index);
static void ECPTimeout(void *arg);
static void print_Info_Char(void);
static uint8_t ReadLongCharacteristic(uint16_t connection_handle, uint16_t ValueHdl, uint16_t Offset);
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
    context_init(index);
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

    /* USER CODE END GATT_CLIENT_APP_Init_4 */
  }

  /* Register the event handler to the BLE controller */
  SVCCTL_RegisterCltHandler(Event_Handler);

  /* Register a task allowing to discover all services and characteristics and enable all notifications */
  UTIL_SEQ_RegTask(1U << CFG_TASK_DISCOVER_SERVICES_ID, UTIL_SEQ_RFU, client_discover_task);

  /* USER CODE BEGIN GATT_CLIENT_APP_Init_2 */
  
  TIMEREF_SetAbsoluteTime(0);
  
  OTP_CLIENT_Init();
  
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
          context_init(index);
          /* USER CODE END PEER_CONN_HANDLE_EVT_1 */

          a_ClientContext[index].connHdl = p_Notif->ConnHdl;
          a_ClientContext[index].state = GATT_CLIENT_APP_CONNECTED;
          a_ClientContext[index].att_mtu = BLE_DEFAULT_ATT_MTU;

          /* USER CODE BEGIN PEER_CONN_HANDLE_EVT_2 */
          OTP_CLIENT_ConnectionComplete(&a_ClientContext[index].OTSHandles, a_ClientContext[index].connHdl);
          /* USER CODE END PEER_CONN_HANDLE_EVT_2 */

          break;
        }
      }
      if(index == CFG_BLE_NUM_CLIENT_CONTEXTS)
      {
        LOG_ERROR_APP("Error: reached maximum number of connected servers!\n");
        /* USER CODE BEGIN PEER_CONN_HANDLE_EVT_3 */
        aci_gap_terminate(p_Notif->ConnHdl, HCI_REMOTE_USER_TERMINATED_CONNECTION_ERR_CODE);
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
          UTIL_TIMER_Stop(&a_ClientContext[index].ECP_timer_Id);
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
        
        LOG_INFO_APP("Enable notifications on ECP (handle 0x%04X)\n", a_ClientContext[index].ESLControlPointCCCDHdl);
        
        if(a_ClientContext[index].ESLControlPointCCCDHdl != 0x0000)
        {
          charPropVal = 0x0001;
          procClientContext.gatt_error_code = 0;          
          bleStatus = aci_gatt_write_char_value(a_ClientContext[index].connHdl,
                                                a_ClientContext[index].ESLControlPointCCCDHdl,
                                                2,
                                                (uint8_t *) &charPropVal);
          if (bleStatus == BLE_STATUS_SUCCESS && gatt_cmd_resp_wait() == BLE_STATUS_SUCCESS)
          {
            LOG_INFO_APP(" aci_gatt_write_char_value success ESLControlPointCCCDHdl =0x%04X\n",a_ClientContext[index].ESLControlPointCCCDHdl);
          }
          else
          {
            LOG_ERROR_APP(" ESLControlPointCCCDHdl write Failed, status =0x%02X, result=0x%02X\n", bleStatus, procClientContext.gatt_error_code);
            status++;
          }
        }
        if(a_ClientContext[index].OTSHandles.ObjActionCPCCCDHdl != 0x0000)
        {
          charPropVal = 0x0002;
          procClientContext.gatt_error_code = 0;
          LOG_INFO_APP("Enable indications on OACP (handle 0x%04X)\n", a_ClientContext[index].OTSHandles.ObjActionCPCCCDHdl);
          bleStatus = aci_gatt_write_char_value(a_ClientContext[index].connHdl,
                                                a_ClientContext[index].OTSHandles.ObjActionCPCCCDHdl,
                                                2,
                                                (uint8_t *) &charPropVal);
          if (bleStatus == BLE_STATUS_SUCCESS && gatt_cmd_resp_wait() == BLE_STATUS_SUCCESS)
          {
            LOG_DEBUG_APP(" aci_gatt_write_char_value success ObjActionCPCCCDHdl =0x%04X\n",a_ClientContext[index].OTSHandles.ObjActionCPCCCDHdl);
          }
          else
          {
            LOG_ERROR_APP(" ObjActionCPCCCDHdl write Failed, status =0x%02X, result=0x%02X\n", bleStatus, procClientContext.gatt_error_code);
            status++;
          }
        }
        if(a_ClientContext[index].OTSHandles.ObjListCPCCCDHdl != 0x0000)
        {
          charPropVal = 0x0002;
          procClientContext.gatt_error_code = 0;
          LOG_INFO_APP("Enable indications on OLCP (handle 0x%04X)\n", a_ClientContext[index].OTSHandles.ObjListCPCCCDHdl);
          bleStatus = aci_gatt_write_char_value(a_ClientContext[index].connHdl,
                                                a_ClientContext[index].OTSHandles.ObjListCPCCCDHdl,
                                                2,
                                                (uint8_t *) &charPropVal);
          if (bleStatus == BLE_STATUS_SUCCESS && gatt_cmd_resp_wait() == BLE_STATUS_SUCCESS)
          {
            LOG_DEBUG_APP(" aci_gatt_write_char_value success ObjActionCPCCCDHdl =0x%04X\n",a_ClientContext[index].OTSHandles.ObjActionCPCCCDHdl);
          }
          else
          {
            LOG_ERROR_APP(" ObjActionCPCCCDHdl write Failed, status =0x%02X, result=0x%02X\n", bleStatus, procClientContext.gatt_error_code);
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
          
          /* USER CODE END ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
        }
        break;

        /* USER CODE BEGIN VENDOR_SPECIFIC_DEBUG_EVT_CODE_1 */
        case ACI_GATT_ERROR_RESP_VSEVT_CODE:
        {
          aci_gatt_error_resp_event_rp0 *aci_gatt_error_resp;
          aci_gatt_error_resp = (aci_gatt_error_resp_event_rp0 *)p_blecore_evt->data;
          UNUSED(aci_gatt_error_resp);
          LOG_INFO_APP("  ACI_GATT_ERROR_RESP_VSEVT_CODE: 0x%x \n", aci_gatt_error_resp->Error_Code);
          procClientContext.att_error_code = aci_gatt_error_resp->Error_Code;
        }
        break;
        case ACI_ATT_READ_RESP_VSEVT_CODE:
        {
          aci_att_read_resp_event_rp0 * read_resp;
          read_resp = (aci_att_read_resp_event_rp0 *)p_blecore_evt->data;          
          procClientContext.read_char_len = read_resp->Event_Data_Length;
          memcpy(procClientContext.read_char, read_resp->Attribute_Value, procClientContext.read_char_len);
          procClientContext.read_char_offset = procClientContext.read_char_len;
        }
        break;
        case ACI_ATT_READ_BLOB_RESP_VSEVT_CODE:
        {
          aci_att_read_blob_resp_event_rp0 * read_resp;
          read_resp = (aci_att_read_blob_resp_event_rp0 *)p_blecore_evt->data;
          LOG_INFO_APP("  GATT Read Blob Response length: 0x%02x \n", read_resp->Event_Data_Length);
          procClientContext.read_char_len += read_resp->Event_Data_Length;
          memcpy(procClientContext.read_char + procClientContext.read_char_offset, read_resp->Attribute_Value, procClientContext.read_char_len);
          procClientContext.read_char_offset = procClientContext.read_char_len;
        }
        break;
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
    case ESL_NOTIFICATION_INFO_RECEIVED_EVT:
      {
        //ECP notify: response of ESL to AT command sent writing the ECP      
        ESL_AP_ECPNotificationReceived(p_Notif->DataTransfered.p_Payload);
      }
      break;
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
#ifndef PTS_OTP
        else if (uuid == ESL_CONTROL_POINT_UUID)
        {
          a_ClientContext[index].ESLControlPointCharHdl = CharStartHdl;
          a_ClientContext[index].ESLControlPointValueHdl = CharValueHdl;
          LOG_INFO_APP(", ESL_CONTROL_POINT_UUID charac found\n");
        }
        else if (uuid == ESL_ADDRESS_UUID)
        {
          a_ClientContext[index].ESLAddressCharHdl = CharStartHdl;
          a_ClientContext[index].ESLAddressValueHdl = CharValueHdl;
          LOG_INFO_APP(", ESL_ADDRESS_UUID charac found\n");
        }
        else if (uuid == AP_SYNC_KEY_MATERIAL_UUID)
        {
          a_ClientContext[index].APSyncKeyMaterialCharHdl = CharStartHdl;
          a_ClientContext[index].APSyncKeyMaterialValueHdl = CharValueHdl;
          LOG_INFO_APP(", AP_SYNC_KEY_MATERIAL_UUID charac found\n");
        }       
        else if (uuid == ESL_RESP_KEY_MATERIAL_UUID)
        {
          a_ClientContext[index].ESLRespKeyMaterialCharHdl = CharStartHdl;
          a_ClientContext[index].ESLRespKeyMaterialValueHdl = CharValueHdl;
          LOG_INFO_APP(", ESL_RESP_KEY_MATERIAL_UUID charac found\n");
        }
        else if (uuid == ESL_CURR_ABS_TIME_UUID)
        {
          a_ClientContext[index].ESLCurrAbsTimeCharHdl = CharStartHdl;
          a_ClientContext[index].ESLCurrAbsTimeValueHdl = CharValueHdl;
          LOG_INFO_APP(", ESL_CURR_ABS_TIME_UUID charac found\n");
        }
        else if (uuid == ESL_DISPLAY_INFO_UUID)
        {
          a_ClientContext[index].ESLDisplayInfoCharHdl = CharStartHdl;
          a_ClientContext[index].ESLDisplayInfoValueHdl = CharValueHdl;
          LOG_INFO_APP(", ESL_DISPLAY_INFO_UUID charac found\n");
        }
        else if (uuid == ESL_IMAGE_INFO_UUID)
        {
          a_ClientContext[index].ESLImageInfoCharHdl = CharStartHdl;
          a_ClientContext[index].ESLImageInfoValueHdl = CharValueHdl;
          LOG_INFO_APP(", ESL_IMAGE_INFO_UUID charac found\n");
        }
        else if (uuid == ESL_SENSOR_INFO_UUID)
        {
          a_ClientContext[index].ESLSensorInfoCharHdl = CharStartHdl;
          a_ClientContext[index].ESLSensorInfoValueHdl = CharValueHdl;
          LOG_INFO_APP(", ESL_SENSOR_INFO_UUID charac found\n");
        }
        else if (uuid == ESL_LED_INFO_UUID)
        {
          a_ClientContext[index].ESLLedInfoCharHdl = CharStartHdl;
          a_ClientContext[index].ESLLedInfoValueHdl = CharValueHdl;
          LOG_INFO_APP(", ESL_LED_INFO_UUID charac found\n");
        }
#endif /* PTS_OTP */
        else if (uuid == OTS_FEATURE_UUID)
        {
          a_ClientContext[index].OTSHandles.OTSFeatureValueHdl = CharValueHdl;
          LOG_INFO_APP(", OTS_FEATURE_UUID charac found\n");
        }
        else if (uuid == OBJECT_NAME_UUID)
        {
          a_ClientContext[index].OTSHandles.ObjNameValueHdl = CharValueHdl;
          LOG_INFO_APP(", OBJECT_NAME_UUID charac found\n");
        }
        else if (uuid == OBJECT_TYPE_UUID)
        {
          a_ClientContext[index].OTSHandles.ObjTypeValueHdl = CharValueHdl;
          LOG_INFO_APP(", OBJECT_TYPE_UUID charac found\n");
        }
        else if (uuid == OBJECT_SIZE_UUID)
        {
          a_ClientContext[index].OTSHandles.ObjSizeValueHdl = CharValueHdl;
          LOG_INFO_APP(", OBJECT_SIZE_UUID charac found\n");
        }
        else if (uuid == OBJECT_LAST_MODIFIED_UUID)
        {
          a_ClientContext[index].OTSHandles.ObjLastModifiedValueHdl = CharValueHdl;
          a_ClientContext[index].OTSHandles.ObjLastModifiedProp = CharProperties;
          LOG_INFO_APP(", OBJECT_LAST_MODIFIED_UUID charac found\n");
        }
        else if (uuid == OBJECT_ID_UUID)
        {
          a_ClientContext[index].OTSHandles.ObjIdValueHdl = CharValueHdl;
          LOG_INFO_APP(", OBJECT_ID_UUID charac found\n");
        }
        else if (uuid == OBJECT_PROPERTIES_UUID)
        {
          a_ClientContext[index].OTSHandles.ObjPropValueHdl = CharValueHdl;
          LOG_INFO_APP(", OBJECT_PROPERTIES_UUID charac found\n");
        }
        else if (uuid == OBJECT_ACTION_CONTROL_POINT_UUID)
        {
          a_ClientContext[index].OTSHandles.ObjActionCPValueHdl = CharValueHdl;
          LOG_INFO_APP(", OBJECT_ACTION_CONTROL_POINT_UUID charac found\n");
        }
        else if (uuid == OBJECT_LIST_CONTROL_POINT_UUID)
        {
          a_ClientContext[index].OTSHandles.ObjListCPValueHdl = CharValueHdl;
          LOG_INFO_APP(", OBJECT_LIST_CONTROL_POINT_UUID charac found\n");
        }
        else if (uuid == OBJECT_LIST_FILTER_POINT_UUID)
        {
          for(uint8_t i = 0; i < 3; i++)
          {
            if(a_ClientContext[index].OTSHandles.ObjListFilterValueHdl[i] == 0)
            {
              a_ClientContext[index].OTSHandles.ObjListFilterValueHdl[i] = CharValueHdl;
              break;
            }
          }
          LOG_INFO_APP(", OBJECT_LIST_FILTER_POINT_UUID charac found\n");
        }
        else if (uuid == PNPID_UUID)
        {
          a_ClientContext[index].DISPNPIdCharHdl = CharStartHdl;
          a_ClientContext[index].DISPNPIdValueHdl = CharValueHdl;
          LOG_INFO_APP(", PNPID_UUID charac found\n");
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
        else if (a_ClientContext[index].ESLControlPointValueHdl == gattCharValueHdl)
        {
          a_ClientContext[index].ESLControlPointCCCDHdl = handle;
          LOG_INFO_APP(", ESL Control Point CCCD found: handle=0x%04X\n", handle);
        }
        else if (a_ClientContext[index].OTSHandles.ObjActionCPValueHdl == gattCharValueHdl)
        {
          a_ClientContext[index].OTSHandles.ObjActionCPCCCDHdl = handle;
          LOG_INFO_APP(", Object Action Control Point CCCD found: handle=0x%04X\n", handle);
        }
        else if (a_ClientContext[index].OTSHandles.ObjListCPValueHdl == gattCharValueHdl)
        {
          a_ClientContext[index].OTSHandles.ObjListCPCCCDHdl = handle;
          LOG_INFO_APP(", Object List Control Point CCCD found: handle=0x%04X\n", handle);
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
    if (p_evt->Attribute_Handle == a_ClientContext[index].ESLControlPointValueHdl)
    {
      /* The AP shall stop the timer when a notification of the ESL Control Point 
         characteristic is received in response to the command.*/
      UTIL_TIMER_Stop(&a_ClientContext[index].ECP_timer_Id);

      LOG_INFO_APP("Incoming Nofification from ECP\n");
      Notification.Client_Evt_Opcode = ESL_NOTIFICATION_INFO_RECEIVED_EVT;
      Notification.DataTransfered.length = p_evt->Attribute_Value_Length;
      Notification.DataTransfered.p_Payload = &p_evt->Attribute_Value[0];

      gatt_Notification(&Notification);
      
      ret = 0;
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
  int index;
  
  index = GATT_CLIENT_APP_Get_Index(p_evt->Connection_Handle);
  if(index < 0)
    return ret;
  
  if(p_evt->Attribute_Handle == a_ClientContext[index].ServiceChangedCharValueHdl)
  {
    LOG_INFO_APP("Service Changed Indication\n");
    
    UTIL_SEQ_SetTask( 1U << CFG_TASK_DISCOVER_SERVICES_ID, CFG_SEQ_PRIO_0);
    ret = 0;
  }      
  else if(p_evt->Attribute_Handle == a_ClientContext[index].OTSHandles.ObjListCPValueHdl)
  {    
    OTP_CLIENT_OLCPIndication(p_evt->Attribute_Value, p_evt->Attribute_Value_Length);
    ret = 0;
  }
  else if(p_evt->Attribute_Handle == a_ClientContext[index].OTSHandles.ObjActionCPValueHdl)
  {    
    OTP_CLIENT_OACPIndication(p_evt->Attribute_Value, p_evt->Attribute_Value_Length);
    ret = 0;
  }
  
  aci_gatt_confirm_indication(p_evt->Connection_Handle);
  
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
      
#ifndef PTS_OTP  
      if (ESL_AP_Context.configuring)
      {
        GATT_CLIENT_APP_ReadAllInfo(a_ClientContext[index].connHdl);
        if(GATT_CLIENT_APP_ConfigureESL(a_ClientContext[index].connHdl) != 0)
        {
          aci_gap_terminate(a_ClientContext[index].connHdl, HCI_REMOTE_USER_TERMINATED_CONNECTION_ERR_CODE);
        }
       }
#endif
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

static void context_init(uint8_t index)
{  
  memset(&a_ClientContext[index], 0, sizeof(BleClientAppContext_t));
  
  a_ClientContext[index].svcInfo[SVC_ID_GAP].uuid  = GAP_SERVICE_UUID;
  a_ClientContext[index].svcInfo[SVC_ID_GATT].uuid = GATT_SERVICE_UUID;
  a_ClientContext[index].svcInfo[SVC_ID_ESL].uuid  = ESL_SERVICE_UUID;
  a_ClientContext[index].svcInfo[SVC_ID_ESL].findIncludedServices = TRUE;
  a_ClientContext[index].svcInfo[SVC_ID_OTS].uuid  = OBJECT_TRANSFER_SERVICE_UUID;
  a_ClientContext[index].svcInfo[SVC_ID_DIS].uuid  = DEVICE_INFORMATION_SERVICE_UUID;
  
    /* When the AP writes to the ECP, the AP shall start a timer with the value 
     set to the ESL Control Point Timeout period (30 seconds). If the timer 
     expires, then the ECP procedure shall be considered to have failed. */
  UTIL_TIMER_Create(&a_ClientContext[index].ECP_timer_Id,
                    ECP_TIMEOUT_MS,
                    UTIL_TIMER_ONESHOT,
                    ECPTimeout,
                    &a_ClientContext[index]);
}

/* If the AP establish a bond with an ESL and ESL is on Configuring state,
   the AP can configure ESL by writing some ESL Service characteristics  
   (called on ACI_GAP_PAIRING_COMPLETE_VSEVT_CODE) */
uint8_t GATT_CLIENT_APP_ConfigureESL(uint16_t connection_handle)
{
  int index;
  uint8_t ret;
  esl_info_t *p_esl_info = &ESL_AP_Context.conn_esl_info;
  uint16_t esl_address = p_esl_info->esl_address;
  
  index = GATT_CLIENT_APP_Get_Index(connection_handle);
  if(index < 0)
    return BLE_STATUS_INVALID_PARAMS;
  
  /* ESL Address characteristic */
  LOG_DEBUG_APP("Writing ESL Address (0x%04X)\n", p_esl_info->esl_address);
  ret = GATT_CLIENT_APP_WriteCharacteristic(a_ClientContext[index].connHdl,
                                            a_ClientContext[index].ESLAddressValueHdl,
                                            2, (uint8_t *)&esl_address);
  if (ret != BLE_STATUS_SUCCESS)
  {
    return 1;
  }
  
  if(ESL_AP_Context.provisioning)
  {
    /* keys are written only during provisioning. We let write other characteristics
       even if we are just updating ESL.  */
  
    //AP Sync Material characteristic
    LOG_DEBUG_APP("Writing AP Sync Material\n");
    GATT_CLIENT_APP_WriteCharacteristic(a_ClientContext[index].connHdl,
                                        a_ClientContext[index].APSyncKeyMaterialValueHdl,
                                        24, (uint8_t *)&ESL_AP_Context.ap_sync_key_material);
    if (ret != BLE_STATUS_SUCCESS)
    {
      return 1;
    }
    
    //ESL Resp Key Material characteristic  
    ESL_AP_GenerateKeyMaterial(&p_esl_info->esl_resp_key_material);
    
    LOG_DEBUG_APP("Writing Response Key Material:\n");
    LOG_DEBUG_APP("Session key: ");
    for(int i = sizeof(p_esl_info->esl_resp_key_material.session_key); i >= 0; i--)
    {
      LOG_DEBUG_APP("%02X ", p_esl_info->esl_resp_key_material.session_key[i]);
    }
    LOG_DEBUG_APP("\nIV: ");
    for(int i = sizeof(p_esl_info->esl_resp_key_material.iv); i >= 0; i--)
    {
      LOG_DEBUG_APP("%02X ", p_esl_info->esl_resp_key_material.iv[i]);
    }
    LOG_DEBUG_APP("\n");
    
    procClientContext.gatt_error_code = 0;
    GATT_CLIENT_APP_WriteCharacteristic(a_ClientContext[index].connHdl,
                                        a_ClientContext[index].ESLRespKeyMaterialValueHdl,
                                        24, (uint8_t *)&p_esl_info->esl_resp_key_material);
    if (ret != BLE_STATUS_SUCCESS)
    {
      return 1;
    }
  }
  
  /* ESL Current Absolute Time characteristic */  
  uint32_t absoluteTime = TIMEREF_GetCurrentAbsTime();
  LOG_DEBUG_APP("Writing Absolute time (%d)\n", absoluteTime);
  ret = GATT_CLIENT_APP_WriteCharacteristic(a_ClientContext[index].connHdl,
                                            a_ClientContext[index].ESLCurrAbsTimeValueHdl,
                                            4,
                                            (uint8_t *)&absoluteTime);
  
  if (ret != BLE_STATUS_SUCCESS)
  {
    return 1;
  }
  
  if(ESL_AP_Context.configuring)
  {
    ret = ESL_AP_CmdUpdateComplete();
    if(ret)
    {
      LOG_ERROR_APP("Fail sending update complete command\n");
      return 2;
    }
  }
  
  ret = ESL_AP_StoreESLInfo(&ESL_AP_Context.conn_esl_info);
  if(ret == 0)
  {
    LOG_DEBUG_APP("ESL configuration saved\n");
  }
  else
  {
    LOG_ERROR_APP("Error while saving configuration\n");
    aci_gap_terminate(a_ClientContext[index].connHdl, HCI_REMOTE_USER_TERMINATED_CONNECTION_ERR_CODE);
    
    return 2;
  }
  
  return 0;
}

/* If bResponse is true the command wait for an ESL response, else the 
   command has no response, so the timer ECP_TIMEOUT_MS must not start */
uint8_t GATT_CLIENT_APP_WriteECP(uint16_t connection_handle, uint8_t* cmd, uint8_t len_cmd, uint8_t bResponse)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  int index;
  
  index = GATT_CLIENT_APP_Get_Index(connection_handle);
  if(index < 0)
    return BLE_STATUS_INVALID_PARAMS;
  
  /* If an ECP procedure times out, then the AP shall not start a new ECP 
   procedure until a new link is established with the ESL.*/
  if(!a_ClientContext[index].b_ECP_failed)
  {
    LOG_INFO_APP("Writing ECP\n");
    ret = GATT_CLIENT_APP_WriteCharacteristic(a_ClientContext[index].connHdl,
                                              a_ClientContext[index].ESLControlPointValueHdl,
                                              len_cmd,
                                              cmd);
    if (ret != BLE_STATUS_SUCCESS)
    {
      return ret;
    }
   
    /* When the AP writes to the ECP, the AP shall start a timer with the value  
       set to the ESL Control Point Timeout period.*/
    if (bResponse)
      UTIL_TIMER_Start(&a_ClientContext[index].ECP_timer_Id);
  }  
  return ret;
}

static void ECPTimeout(void *arg)
{
  BleClientAppContext_t *clt_ctxt = arg;
  
  LOG_ERROR_APP("ECP procedure failed\n");
  /* If an ECP procedure times out, then the AP shall not start a new ECP 
     procedure until a new link is established with the ESL.*/
  clt_ctxt->b_ECP_failed = true;
}

static void print_Info_Char(void)
{  
  for (int i = 0; i < procClientContext.read_char_len; i++) 
  {
    LOG_INFO_APP("%02X", procClientContext.read_char[i]);
    if (i < procClientContext.read_char_len-1) 
      LOG_INFO_APP(":");
  }    
  LOG_INFO_APP("\n");
}

uint8_t GATT_CLIENT_APP_ReadAllInfo(uint16_t connection_handle)
{
  tBleStatus ret = 0;
  
  LOG_INFO_APP("Read Display Information\n");
  GATT_CLIENT_APP_ReadDisplayInfo(connection_handle); 
  
  LOG_INFO_APP("Read Image Information\n");
  GATT_CLIENT_APP_ReadImageInfo(connection_handle);
  
  LOG_INFO_APP("Read Sensor Information\n");
  GATT_CLIENT_APP_ReadSensorInfo(connection_handle);
  
  LOG_INFO_APP("Read LED Information\n");
  GATT_CLIENT_APP_ReadLedInfo(connection_handle);
  
  LOG_INFO_APP("Read PnP ID\n");
  GATT_CLIENT_APP_ReadPnPID(connection_handle);
  
  return ret;
}  

uint8_t GATT_CLIENT_APP_ReadDisplayInfo(uint16_t connection_handle)
{
  tBleStatus ret = BLE_STATUS_SUCCESS;
  int index;
  uint8_t *data_p;
  uint16_t data_length;
  
  index = GATT_CLIENT_APP_Get_Index(connection_handle);
  if(index < 0)
    return BLE_STATUS_INVALID_PARAMS;
  
  if(a_ClientContext[index].ESLDisplayInfoValueHdl == 0)
    return BLE_STATUS_ERROR;
  
  ret = GATT_CLIENT_APP_ReadCharacteristic(connection_handle, a_ClientContext[index].ESLDisplayInfoValueHdl, &data_length, &data_p, true);
  if(ret != BLE_STATUS_SUCCESS)
    return ret;
  
  print_Info_Char();
  
  return ret;
}

uint8_t GATT_CLIENT_APP_ReadImageInfo(uint16_t connection_handle)
{
  tBleStatus ret = BLE_STATUS_SUCCESS;
  int index;
  uint8_t *data_p;
  uint16_t data_length;
  
  index = GATT_CLIENT_APP_Get_Index(connection_handle);
  if(index < 0)
    return BLE_STATUS_INVALID_PARAMS;
  
  if(a_ClientContext[index].ESLImageInfoValueHdl == 0)
    return BLE_STATUS_ERROR;
  
  ret = GATT_CLIENT_APP_ReadCharacteristic(connection_handle, a_ClientContext[index].ESLImageInfoValueHdl, &data_length, &data_p, true);
  if(ret != BLE_STATUS_SUCCESS)
    return ret;
  
  print_Info_Char();
  
  return ret;
}

uint8_t GATT_CLIENT_APP_ReadSensorInfo(uint16_t connection_handle)
{
  tBleStatus ret = BLE_STATUS_SUCCESS;
  int index;
  uint8_t *data_p;
  uint16_t data_length;
  
  index = GATT_CLIENT_APP_Get_Index(connection_handle);
  if(index < 0)
    return BLE_STATUS_INVALID_PARAMS;
  
  if(a_ClientContext[index].ESLSensorInfoValueHdl == 0)
    return BLE_STATUS_ERROR;
  
  ret = GATT_CLIENT_APP_ReadCharacteristic(connection_handle, a_ClientContext[index].ESLSensorInfoValueHdl, &data_length, &data_p, true);
  if(ret != BLE_STATUS_SUCCESS)
    return ret;
  
  print_Info_Char();
  
  return ret;
}

uint8_t GATT_CLIENT_APP_ReadLedInfo(uint16_t connection_handle)
{
  tBleStatus ret = BLE_STATUS_SUCCESS;
  int index;
  uint8_t *data_p;
  uint16_t data_length;
  
  index = GATT_CLIENT_APP_Get_Index(connection_handle);
  if(index < 0)
    return BLE_STATUS_INVALID_PARAMS;
  
  if(a_ClientContext[index].ESLLedInfoValueHdl == 0)
    return BLE_STATUS_ERROR;
  
  ret = GATT_CLIENT_APP_ReadCharacteristic(connection_handle, a_ClientContext[index].ESLLedInfoValueHdl, &data_length, &data_p, true);
  if(ret != BLE_STATUS_SUCCESS)
    return ret;
  
  print_Info_Char();
  
  return ret;
}

uint8_t GATT_CLIENT_APP_ReadPnPID(uint16_t connection_handle)
{
  tBleStatus ret = BLE_STATUS_SUCCESS;
  int index;
  uint8_t *data_p;
  uint16_t data_length;
  
  index = GATT_CLIENT_APP_Get_Index(connection_handle);
  if(index < 0)
    return BLE_STATUS_INVALID_PARAMS;
  
  if(a_ClientContext[index].DISPNPIdValueHdl == 0)
    return BLE_STATUS_ERROR;
  
  ret = GATT_CLIENT_APP_ReadCharacteristic(connection_handle, a_ClientContext[index].DISPNPIdValueHdl, &data_length, &data_p, true);
  if(ret != BLE_STATUS_SUCCESS)
    return ret;
  
  print_Info_Char();
  
  return ret;
}

/* USER CODE END LF */
