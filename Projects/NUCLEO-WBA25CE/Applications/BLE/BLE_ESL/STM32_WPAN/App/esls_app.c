/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ESLS_app.c
  * @author  MCD Application Team
  * @brief   ESLS_app application definition.
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
#include "app_common.h"
#include "log_module.h"
#include "app_ble.h"
#include "ll_sys_if.h"
#include "dbg_trace.h"
#include "esls_app.h"
#include "esls.h"
#include "stm32_rtos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "esl_device.h"
#include "time_ref.h"
#include "stm32_timer.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

#define MAX_ESL_PAYLOAD_SIZE                    (48U)
#define EAD_MIC_SIZE                            (4U)
#define EAD_RANDOMZER_SIZE                      (5U)
#define MAX_ADV_PAYLOAD                         (MAX_ESL_PAYLOAD_SIZE + 2 + EAD_RANDOMZER_SIZE + EAD_MIC_SIZE + 2)

typedef struct
{
  uint8_t Session_Key[16];
  uint8_t IV[8];
}ESL_APP_KeyMaterial_t;

typedef enum 
{
  TIMED_CMD_NOT_PENDING,
  TIMED_CMD_TIMER_RUNNING,
  TIMED_CMD_EXECUTION_PENDING,
}timedCmdState_t;

/* Structures used for LED Timed Control Command */
typedef struct
{
  uint8_t color_brigthness;
  uint8_t pattern[5];
  uint8_t off_period;
  uint8_t on_period;
  uint16_t repeat;
  timedCmdState_t pend_state;
  uint32_t abs_time;
  UTIL_TIMER_Object_t timer;
}LEDTimedInfo_t;

/* Structures used for Display Timed Image Command */
typedef struct
{
  uint8_t image_index;
  timedCmdState_t pend_state;
  uint32_t abs_time;
  UTIL_TIMER_Object_t timer;
}DisplayTimedInfo_t;

/* USER CODE END PTD */

typedef enum
{
  Esl_control_point_NOTIFICATION_OFF,
  Esl_control_point_NOTIFICATION_ON,
  /* USER CODE BEGIN Service2_APP_SendInformation_t */

  /* USER CODE END Service2_APP_SendInformation_t */
  ESLS_APP_SENDINFORMATION_LAST
} ESLS_APP_SendInformation_t;

typedef struct
{
  ESLS_APP_SendInformation_t     Esl_control_point_Notification_Status;
  /* USER CODE BEGIN Service2_APP_Context_t */
  ESL_APP_State_t state;
  uint8_t group_id;
  uint8_t esl_id;
  ESL_APP_KeyMaterial_t ap_sync_key_material;
  ESL_APP_KeyMaterial_t esl_resp_key_material;
  uint16_t sync_handle;
  uint16_t basic_state;
  uint8_t a_resp[MAX_ADV_PAYLOAD];
  uint8_t config_state_flags;   /* Flags for written characteristics during configuration (CONFIG_STATE_FLAG macros) */
  
  /* Synchronized timeout timerID*/
  /* If the ESL has not received a valid ESL message in a synchronization message from
     the AP for 60 minutes then the ESL shall transition to the Unsynchronized state. */
  UTIL_TIMER_Object_t Synchronized_timer_Id;  
  /* If the ESL is not moved to the Updating state for 60 minutes, then the 
     ESL shall transition to the Unassociated state */
  UTIL_TIMER_Object_t Unassociate_timer_Id;
  UTIL_TIMER_Object_t Disconnection_timer_Id;
  uint8_t Peer_Address_Type;
  uint8_t Peer_Address[6];
  bool    connected;
  bool   sync_recvd;
  bool upd_compl_recvd;
  bool bFactoryReset;
  bool disconnect;
  /* USER CODE END Service2_APP_Context_t */
  uint16_t              ConnectionHandle;
} ESLS_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define AD_TYPE_ENCRYPTED_ADVERTISING_DATA      (0x31)
#define AD_TYPE_ELECTRONIC_SHELF_LABEL          (0x34)

/* Codes for Commands */
#define ESL_CMD_PING                            (0x00)
#define ESL_CMD_UNASSOCIATE_FROM_AP             (0x01)
#define ESL_CMD_SERVICE_RESET                   (0x02)
#define ESL_CMD_FACTORY_RESET                   (0x03)
#define ESL_CMD_UPDATE_COMPLETE                 (0x04)
#define ESL_CMD_READ_SENSOR_DATA                (0x10)
#define ESL_CMD_REFRESH_DISPLAY                 (0x11)
#define ESL_CMD_DISPLAY_IMG                     (0x20)
#define ESL_CMD_DISPLAY_TIMED_IMG               (0x60)
#define ESL_CMD_LED_CONTROL                     (0xB0)
#define ESL_CMD_LED_TIMED_CONTROL               (0xF0)

/* Code for vendor-specific commands */
#define ESL_CMD_VS_PRICE                        (0x3F)
#define ESL_CMD_VS_TXT                          (0xBF)

/* Code for response data types */
#define ESL_RESP_ERROR                          (0x00)
#define ESL_RESP_LED_STATE                      (0x01)
#define ESL_RESP_BASIC_STATE                    (0x10)
#define ESL_RESP_DISPLAY_STATE                  (0x11)
#define ESL_RESP_SENSOR_VALUE_TAG_NIBBLE        (0x0E) // This is only the value of the Tag nibble. The Length is variable
#define ESL_RESP_VS_OK                          (0x0F)

#define BRC_ESL_ID                              (0xFF)

#define GET_PARAM_LENGTH_FROM_OPCODE(opcode)          ((((opcode) & 0xF0) >> 4) + 1)

#define GET_LENGTH_FROM_OPCODE(opcode)                (GET_PARAM_LENGTH_FROM_OPCODE(opcode) + 1)

#define SET_LENGTH_TO_OPCODE(tag, param_length)       (((tag) & 0x0F) | ((((param_length) - 1) & 0x0F) << 4))

#define DIV_CEIL(x, y)                  (((x) + (y) - 1U) / (y))


#define CONFIG_STATE_FLAG_ADDRESS           0x01
#define CONFIG_STATE_FLAG_SYNC_KEY          0x02
#define CONFIG_STATE_FLAG_RESPONSE_KEY      0x04
#define CONFIG_STATE_FLAG_ABSOLUTE_TIME     0x08
#define CONFIG_STATE_FLAG_ALL_SET           0x0F

#define SYNC_TIMEOUT_MS                     (60 * 60 * 1000)  // 60 minutes
#define UNSYNC_TIMEOUT_MS                   (60 * 60 * 1000)  // 60 minutes
#define DISCONNECTION_TIMEOUT_MS            200

#define C_SIZE_CMD_STRING       20

#define MAX_TIMED_CMD_DELAY_MS          (4147200000)    // 48 days
/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static ESLS_APP_Context_t ESLS_APP_Context;

uint8_t a_ESLS_UpdateCharData[247];

/* USER CODE BEGIN PV */
static uint8_t CommandString[C_SIZE_CMD_STRING];
static volatile uint16_t indexReceiveChar = 0;

LEDTimedInfo_t led_timed_info[NUM_LEDS];
DisplayTimedInfo_t display_timed_info[NUM_DISPLAYS];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void ESLS_Esl_control_point_SendNotification(void);

/* USER CODE BEGIN PFP */
static uint8_t TLV_OpCode_handling(uint8_t *p_cmd, uint8_t opcode, uint8_t esl_cmd_id, uint8_t * esl_payload_resp, uint8_t param_length, uint8_t resp_idx);
static void synch_packet_received(uint16_t pa_event, uint8_t *p_esl_data, uint8_t size);
static void send_resp(uint16_t pa_event, uint8_t resp_slot, uint8_t *p_esl_resp, uint8_t resp_size);
static void disconnection_delay(void *arg);
static void ESL_APP_UnsynchronizedState(void);
static void ESL_APP_UnsynchronizedStateTimerCB(void *arg);
static void ESL_APP_UnassociatedStateTimerCB(void *arg);
static void ESL_APP_UpdatingStateTransition(uint16_t sync_handle);
static uint8_t factoryResetCmdCB(void);
static void ESL_APP_ProcessRequest(void);
static void ESL_APP_Process(void);

static uint8_t led_states[DIV_CEIL(NUM_LEDS,8)];

static uint8_t LEDControlCmdCB(uint8_t led_index, uint8_t led_RGB_Brigthness, uint8_t led_flash_pattern[5], uint8_t off_period, uint8_t on_period, uint16_t led_repeat);
static uint8_t LEDTimedControlCmdCB(uint8_t led_index,  uint8_t led_RGB_Brigthness, uint8_t led_flash_pattern[5], uint8_t off_period, uint8_t on_period, uint16_t led_repeat, uint32_t abs_time);
static void LED_Timed_Cmd_timeout_cb(void *arg);
static int checkPendingLedUpdate(void);

static uint8_t displayImageCmdCB(uint8_t display_index, uint8_t image_index);
static uint8_t refreshDisplayCmdCB(uint8_t display_index, uint8_t *image_index_p);
static uint8_t displayTimedImageCmdCB(uint8_t display_index, uint8_t image_index, uint32_t abs_time);
static void Img_Timed_Cmd_timeout_cb(void *arg);
static int checkPendingDisplayUpdate(void);

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void ESLS_Notification(ESLS_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service2_Notification_1 */

  /* USER CODE END Service2_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service2_Notification_Service2_EvtOpcode */

    /* USER CODE END Service2_Notification_Service2_EvtOpcode */

    case ESLS_ESL_ADDR_WRITE_EVT:
      /* USER CODE BEGIN Service2Char1_WRITE_EVT */

      /* USER CODE END Service2Char1_WRITE_EVT */
      break;

    case ESLS_AP_SYNC_KEY_MATERIAL_WRITE_EVT:
      /* USER CODE BEGIN Service2Char2_WRITE_EVT */
      LOG_DEBUG_APP("Write on AP Sync Key Material\n");
      ESL_APP_SetAPSyncKeyMaterial(p_Notification->DataTransfered.p_Payload);
      /* USER CODE END Service2Char2_WRITE_EVT */
      break;

    case ESLS_ESL_RESP_KEY_MATERIAL_WRITE_EVT:
      /* USER CODE BEGIN Service2Char3_WRITE_EVT */
      LOG_DEBUG_APP("Write on ESL Resp Key Material\n");
      ESL_APP_SetESLResponseKeyMaterial(p_Notification->DataTransfered.p_Payload);

      /* USER CODE END Service2Char3_WRITE_EVT */
      break;

    case ESLS_ESL_CURR_ABS_TIME_WRITE_EVT:
      /* USER CODE BEGIN Service2Char4_WRITE_EVT */
      LOG_DEBUG_APP("Write on ESL Current Absolute Time\n");
      {
      	uint32_t currAbsTime;
      	currAbsTime = LE_TO_HOST_32(p_Notification->DataTransfered.p_Payload);
            
        ESL_APP_SetCurrentAbsoluteTime(currAbsTime);
      }

      /* USER CODE END Service2Char4_WRITE_EVT */
      break;

    case ESLS_ESL_DISPLAY_INFO_READ_EVT:
      /* USER CODE BEGIN Service2Char5_READ_EVT */

      /* USER CODE END Service2Char5_READ_EVT */
      break;

    case ESLS_ESL_IMAGE_INFO_READ_EVT:
      /* USER CODE BEGIN Service2Char6_READ_EVT */

      /* USER CODE END Service2Char6_READ_EVT */
      break;

    case ESLS_ESL_SENSOR_INFO_READ_EVT:
      /* USER CODE BEGIN Service2Char7_READ_EVT */

      /* USER CODE END Service2Char7_READ_EVT */
      break;

    case ESLS_ESL_LED_INFO_READ_EVT:
      /* USER CODE BEGIN Service2Char8_READ_EVT */

      /* USER CODE END Service2Char8_READ_EVT */
      break;

    case ESLS_ESL_CONTROL_POINT_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN Service2Char9_WRITE_NO_RESP_EVT */
      LOG_DEBUG_APP("Write on ECP\n");
      ESL_APP_ControlPointReceived(p_Notification->DataTransfered.p_Payload, p_Notification->DataTransfered.Length);
      /* USER CODE END Service2Char9_WRITE_NO_RESP_EVT */
      break;

    case ESLS_ESL_CONTROL_POINT_WRITE_EVT:
      /* USER CODE BEGIN Service2Char9_WRITE_EVT */
      LOG_DEBUG_APP("Write on ECP\n");
      ESL_APP_ControlPointReceived(p_Notification->DataTransfered.p_Payload, p_Notification->DataTransfered.Length);
      /* USER CODE END Service2Char9_WRITE_EVT */
      break;

    case ESLS_ESL_CONTROL_POINT_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service2Char9_NOTIFY_ENABLED_EVT */
      LOG_DEBUG_APP("ECP Notification Enabled\n");
      /* USER CODE END Service2Char9_NOTIFY_ENABLED_EVT */
      break;

    case ESLS_ESL_CONTROL_POINT_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service2Char9_NOTIFY_DISABLED_EVT */
      LOG_DEBUG_APP("ECP Notification Disabled\n");
      /* USER CODE END Service2Char9_NOTIFY_DISABLED_EVT */
      break;

    default:
      /* USER CODE BEGIN Service2_Notification_default */

      /* USER CODE END Service2_Notification_default */
      break;
  }
  /* USER CODE BEGIN Service2_Notification_2 */

  /* USER CODE END Service2_Notification_2 */
  return;
}

void ESLS_APP_EvtRx(ESLS_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service2_APP_EvtRx_1 */

  /* USER CODE END Service2_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service2_APP_EvtRx_Service2_EvtOpcode */

    /* USER CODE END Service2_APP_EvtRx_Service2_EvtOpcode */
    case ESLS_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service2_APP_CONN_HANDLE_EVT */

      /* USER CODE END Service2_APP_CONN_HANDLE_EVT */
      break;

    case ESLS_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service2_APP_DISCON_HANDLE_EVT */
      ESL_APP_DisconnectionComplete(p_Notification->ConnectionHandle);
      /* USER CODE END Service2_APP_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN Service2_APP_EvtRx_default */

      /* USER CODE END Service2_APP_EvtRx_default */
      break;
  }

  /* USER CODE BEGIN Service2_APP_EvtRx_2 */

  /* USER CODE END Service2_APP_EvtRx_2 */

  return;
}

void ESLS_APP_Init(void)
{
  UNUSED(ESLS_APP_Context);
  ESLS_Init();

  /* USER CODE BEGIN Service2_APP_Init */
  tBleStatus ret;
  uint8_t image_info = NUM_IMAGES - 1; 
  ESLS_Data_t esl_information_data;
  
  LOG_DEBUG_APP("*** Unassociated State\n");
  ESLS_APP_Context.state = ESL_STATE_UNASSOCIATED;
  
  ESLS_APP_Context.config_state_flags = 0;

  esl_information_data.p_Payload = (uint8_t *)ESL_Display_Info;
  esl_information_data.Length = sizeof(ESL_Display_Info);
  ESLS_UpdateValue(ESLS_ESL_DISPLAY_INFO, &esl_information_data);
  
  esl_information_data.p_Payload = &image_info;
  esl_information_data.Length = sizeof(image_info);
  ESLS_UpdateValue(ESLS_ESL_IMAGE_INFO, &esl_information_data);

  esl_information_data.p_Payload = (uint8_t *)ESL_Sensor_info;
  esl_information_data.Length = sizeof(ESL_Sensor_info);
  ESLS_UpdateValue(ESLS_ESL_SENSOR_INFO, &esl_information_data);
  
  esl_information_data.p_Payload = (uint8_t *)ESL_LED_info;
  esl_information_data.Length = sizeof(ESL_LED_info);
  ESLS_UpdateValue(ESLS_ESL_LED_INFO, &esl_information_data);
 
  aci_gap_clear_security_db();
  
  ret = hci_le_set_default_periodic_advertising_sync_transfer_parameters(0x01, /* Mode: reports disabled */
                                                                         0x0000, /* Skip */
                                                                         1000, /* Sync_Timeout */
                                                                         0); /* CTE_Type*/
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_ERROR_APP("Set PAST parameters fail (0x%02x)\n", ret);
  }
  else
  {
    LOG_DEBUG_APP("Set PAST parameters success\n");
  }  
  
  /* Create timer to manage Synchronized timeout timerID*/
  /* If the ESL has not received a valid ESL message in a synchronization message from
     the AP for 60 minutes then the ESL shall transition to the Unsynchronized state. */  
  UTIL_TIMER_Create(&ESLS_APP_Context.Synchronized_timer_Id,
                    SYNC_TIMEOUT_MS,
                    UTIL_TIMER_ONESHOT,
                    ESL_APP_UnsynchronizedStateTimerCB, 0);
  /* Create timer to manage Unsynchronized state timeout timerID*/
  /* If the ESL is not moved to the Updating state for 60 minutes, then the 
     ESL shall transition to the Unassociated state */
  UTIL_TIMER_Create(&ESLS_APP_Context.Unassociate_timer_Id,
                    UNSYNC_TIMEOUT_MS,
                    UTIL_TIMER_ONESHOT,
                    ESL_APP_UnassociatedStateTimerCB, 0);
  
  UTIL_TIMER_Create(&ESLS_APP_Context.Disconnection_timer_Id,
                    DISCONNECTION_TIMEOUT_MS,
                    UTIL_TIMER_ONESHOT,
                    disconnection_delay, 0);
  
  UTIL_SEQ_RegTask(1<<CFG_TASK_ESL_PROCESS, UTIL_SEQ_RFU, ESL_APP_Process);
  /* USER CODE END Service2_APP_Init */
  return;
}

/* USER CODE BEGIN FD */
void ESL_APP_ConnectionComplete(uint16_t connection_handle, uint16_t sync_handle, uint8_t Peer_Address_Type, uint8_t Peer_Address[6])
{
  ESLS_APP_Context.connected = true;
  ESLS_APP_Context.ConnectionHandle = connection_handle;
  ESLS_APP_Context.Peer_Address_Type = Peer_Address_Type;
  memcpy(ESLS_APP_Context.Peer_Address, Peer_Address, sizeof(ESLS_APP_Context.Peer_Address));
  
  /* When the AP connects with the ESL, using the Periodic Advertising Connection 
   procedure, the ESL transitions to the Updating state. */  
  ESL_APP_UpdatingStateTransition(sync_handle);
  
  ESLS_APP_Context.upd_compl_recvd = false;
}

/* To be called when a bond has been established. */
void ESL_APP_PairingComplete(uint16_t connection_handle)
{
  if(ESLS_APP_Context.state == ESL_STATE_UNASSOCIATED)
  {
    LOG_DEBUG_APP("*** Configuring State\n");
    ESLS_APP_Context.state = ESL_STATE_CONFIGURING;
    ESLS_APP_Context.ConnectionHandle = connection_handle;
  }
}

void ESL_APP_DisconnectionComplete(uint16_t connection_handle)
{
  ESLS_APP_Context.connected = false;
  
  if(ESLS_APP_Context.bFactoryReset)
  {
    /* Request a restart */
    aci_gap_clear_security_db();
    ESL_DEVICE_FactoryResetCB();
    
    return;
  }  
  
  if(ESLS_APP_Context.state == ESL_STATE_CONFIGURING)
  {
     /* If the connection is lost owing to link loss occurring in the Configuring 
        state before the configuration of the ESL has been completed, then the ESL 
        shall transition to the Unassociated state */
     if(ESLS_APP_Context.config_state_flags != CONFIG_STATE_FLAG_ALL_SET)
     {
       ESLS_APP_Context.state = ESL_STATE_UNASSOCIATED;
       LOG_DEBUG_APP("*** Unassociated state from Configuring state for link loss \n");
       // Enter GAP undirected connectable mode.
       APP_BLE_StartAdvertising(true);
     }
     else 
     {
       /* if the connection is lost owing to link loss after the configuration 
          of the ESL has been successfully completed, the ESL shall transition 
          to the Unsynchronized state */
       LOG_DEBUG_APP("*** Unsynchronized state from Configuring state for link loss \n");
       ESL_APP_UnsynchronizedState();
     }
  } 
  /* If the connection is lost owing to link loss occurring in the Updating state, 
     then the ESL shall transition to the Unsynchronized state */
  else if(ESLS_APP_Context.state == ESL_STATE_UPDATING)
  {
    LOG_DEBUG_APP("*** Unsynchronized state from Updating state for link loss \n");
    ESL_APP_UnsynchronizedState();
  }
  else if(ESLS_APP_Context.state == ESL_STATE_UNASSOCIATED)
  {
    /* If a disconnection event is received before moving to configuring state, we go back advertising. */
    APP_BLE_StartAdvertising(true);
  }
  
  memset(ESLS_APP_Context.Peer_Address, 0, 6);
}

void ESL_APP_SyncLost(void)
{
  LOG_DEBUG_APP("*** Unsynchronized state for sync lost\n");
  ESL_APP_UnsynchronizedState();
  ESLS_APP_Context.sync_recvd = false;
}

/* Set the group_id and esl_id by ESL Address and return true if group_id is 
   changed from the previous one */
uint8_t ESL_APP_SetESLAddress(uint16_t address)
{  
  if (ESL_APP_ConfiguringOrUpdatingState())
  {
    uint8_t esl_id = address & 0x00FF;
    if (esl_id == 0xFF)
    {
      return BLE_ATT_ERR_VALUE_NOT_ALLOWED;
    }
    
    ESLS_APP_Context.group_id = (address & 0x7F00) >> 8;
    ESLS_APP_Context.esl_id = esl_id;
    
    ESLS_APP_Context.config_state_flags |= CONFIG_STATE_FLAG_ADDRESS;
    
    LOG_INFO_APP("*** ESL_address: %d,%d\n", ESLS_APP_Context.group_id, ESLS_APP_Context.esl_id);
    
    return BLE_ATT_ERR_NONE;
  }
  return BLE_ATT_ERR_VALUE_NOT_ALLOWED;
}

void ESL_APP_SetAPSyncKeyMaterial(uint8_t key_material[24])
{
  if (ESL_APP_ConfiguringOrUpdatingState())
  {
    memcpy(&ESLS_APP_Context.ap_sync_key_material.Session_Key, key_material, 16);
    memcpy(&ESLS_APP_Context.ap_sync_key_material.IV, &key_material[16], 8);
    
    LOG_INFO_APP("*** Set AP Sync Key Material\n");
    for(uint8_t i=0; i<16; i++)
    {
      LOG_DEBUG_APP("%02X", ESLS_APP_Context.ap_sync_key_material.Session_Key[i]);
          if (i < 15) LOG_DEBUG_APP(":");
    }
    LOG_DEBUG_APP("\n*** IV: \n");
    for(uint8_t i=0; i<8; i++)
    {
      LOG_DEBUG_APP("%02X", ESLS_APP_Context.ap_sync_key_material.IV[i]);
          if (i < 7) LOG_DEBUG_APP(":");
    }
    LOG_DEBUG_APP("\n");
    ESLS_APP_Context.config_state_flags |= CONFIG_STATE_FLAG_SYNC_KEY;
  }
}

void ESL_APP_SetESLResponseKeyMaterial(uint8_t key_material[24])
{
  if (ESL_APP_ConfiguringOrUpdatingState())
  {
    memcpy(&ESLS_APP_Context.esl_resp_key_material.Session_Key, key_material, 16);
    memcpy(&ESLS_APP_Context.esl_resp_key_material.IV, &key_material[16], 8);
    
    LOG_INFO_APP("*** Set ESL Response Key Material\n");
    for(uint8_t i=0; i<16; i++)
    {
      LOG_DEBUG_APP("%02X", ESLS_APP_Context.esl_resp_key_material.Session_Key[i]);
          if (i < 15) LOG_DEBUG_APP(":");
    }
    LOG_DEBUG_APP("\n*** IV: \n");
    for(uint8_t i=0; i<8; i++)
    {
      LOG_DEBUG_APP("%02X", ESLS_APP_Context.esl_resp_key_material.IV[i]);
          if (i < 7) LOG_DEBUG_APP(":");
    }
    LOG_DEBUG_APP("\n");
    ESLS_APP_Context.config_state_flags |= CONFIG_STATE_FLAG_RESPONSE_KEY;
  }
}

void ESL_APP_SetCurrentAbsoluteTime(uint32_t curr_absolute_time)
{
  if (ESL_APP_ConfiguringOrUpdatingState())
  {
    TIMEREF_SetAbsoluteTime(curr_absolute_time);
    ESLS_APP_Context.config_state_flags |= CONFIG_STATE_FLAG_ABSOLUTE_TIME;
    
    LOG_INFO_APP("*** SET CurrentAbsTime: %d\n", TIMEREF_GetCurrentAbsTime());
  }
}

void ESL_APP_SyncInfoReceived(uint16_t sync_handle)
{
  tBleStatus ret;
    
  ESLS_APP_Context.sync_handle = sync_handle;
  ESLS_APP_Context.sync_recvd = true;
  
  ret = hci_le_set_periodic_sync_subevent(sync_handle,
                                          0, /* Periodic_Advertising_Properties */
                                          1, /* Num_Subevents */
                                          &ESLS_APP_Context.group_id);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_ERROR_APP("hci_le_set_periodic_sync_subevent fail (0x%02x)\n", ret);
  }
  
  ret = hci_le_set_periodic_advertising_receive_enable(sync_handle,
                                                       1 /* Enable*/);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_ERROR_APP("hci_le_set_periodic_advertising_receive_enable fail (0x%02x)\n", ret);
  }
  
  /* The Synchronized state may be entered only from the Configuring state or  
     the Updating state. The ESL transitions to the Synchronized state when the  
     ESL completes the Periodic Advertising Bluetooth SIG Sync Transfer procedure 
     and synchronizes to a periodic advertising train transmitted by the AP. */
  if (ESL_APP_ConfiguringOrUpdatingState())
  {  
    LOG_DEBUG_APP("*** Synchronized State\n");
    ESLS_APP_Context.state = ESL_STATE_SYNCHRONIZED;
    /* If the ESL has not received a valid ESL message in a synchronization message from
       the AP for 60 minutes then the ESL shall transition to the Unsynchronized state. */  
    UTIL_TIMER_Start(&ESLS_APP_Context.Synchronized_timer_Id);
  }

  if(ESLS_APP_Context.upd_compl_recvd)  
  {
    ret = aci_gap_terminate(ESLS_APP_Context.ConnectionHandle, HCI_REMOTE_USER_TERMINATED_CONNECTION_ERR_CODE);
    if (ret != BLE_STATUS_SUCCESS)
    {
      LOG_ERROR_APP("aci_gap_terminate failure: reason=0x%02X\n", ret);
    }
    else
    {
      LOG_DEBUG_APP("==>> aci_gap_terminate : Success\n");
    }
  }
    
}

void ESL_APP_AdvPayloadReceived(uint16_t pa_event, uint8_t *p_adv_data, uint8_t size)
{
  tBleStatus ret;
  uint8_t decrypted_data[MAX_ESL_PAYLOAD_SIZE];
  uint8_t encrypted_data_length;
  uint8_t *esl_payload_p;
  uint8_t esl_payload_length;
  uint16_t decrypted_data_length;
   
  /* The ESL must be in Synchronized state */
  if(ESLS_APP_Context.state != ESL_STATE_SYNCHRONIZED)
  {
    /* Ignore data in Updating state. */
    return;
  }
  
  if(size > MAX_ESL_PAYLOAD_SIZE + EAD_MIC_SIZE + EAD_RANDOMZER_SIZE + 4) /* 4 is the overhead for AD types and lengths. */
  {
    return;
  }  
  
  if(p_adv_data[1] != AD_TYPE_ENCRYPTED_ADVERTISING_DATA || p_adv_data[0] + 1 != size)
  {
    /* Accept only (well formatted) encrypted advertising data. */
    return;
  }

  /* If the ESL has received a valid ESL message in a synchronization message from
     the AP then the ESL stop the Synchronized Timer. */  
  UTIL_TIMER_Stop(&ESLS_APP_Context.Synchronized_timer_Id);
  
  /* p_adv_data[0] contains the ADV packet length (see Fig 5.1 ESL profile spec) */
  encrypted_data_length = p_adv_data[0] - 1;
  
  ret = aci_hal_ead_encrypt_decrypt(1, /* Decryption */
                                        ESLS_APP_Context.ap_sync_key_material.Session_Key,
                                        ESLS_APP_Context.ap_sync_key_material.IV,
                                        encrypted_data_length,
                                        &p_adv_data[2],
                                        &decrypted_data_length,
                                        decrypted_data);
  
  if(ret != BLE_STATUS_SUCCESS)
  {
    LOG_ERROR_APP("Decryption fail.\n");
    return;
  }
  
  esl_payload_p = decrypted_data;
  esl_payload_length = esl_payload_p[0];
  
  if(esl_payload_length != encrypted_data_length - EAD_RANDOMZER_SIZE - EAD_MIC_SIZE - 1 ||
     esl_payload_p[1] != AD_TYPE_ELECTRONIC_SHELF_LABEL)
  {
    return;
  }
  
  synch_packet_received(pa_event, &esl_payload_p[2], esl_payload_length); 
  
  LOG_DEBUG_APP("End of ESL_APP_AdvPayloadReceived\n");
}

/* To handle both periodic advertising TLV management and control point characteristic write TLV management */
static uint8_t TLV_OpCode_handling(uint8_t *p_cmd, uint8_t opcode, uint8_t esl_cmd_id, uint8_t * esl_payload_resp, uint8_t param_length, uint8_t resp_idx)
{
    uint8_t ret;
    
    switch(opcode)
    {
    case ESL_CMD_PING:
      {          
        if(esl_cmd_id != BRC_ESL_ID)
        {
          //TBR: to check if response exceeds ESL payload size.
          LOG_DEBUG_APP("PING command [opcode: 0x%02x] \n", opcode);
          esl_payload_resp[resp_idx] = ESL_RESP_BASIC_STATE;
          //Basic State response bitmap parmam (16 bits)
          HOST_TO_LE_16(&esl_payload_resp[resp_idx + 1], ESLS_APP_Context.basic_state);
          resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
        }
      }
      break;
#if NUM_LEDS
    case ESL_CMD_LED_CONTROL:
      {
        uint8_t led_index;
        uint8_t led_RGB_Brigthness;
        uint8_t led_flash_pattern[5];
        uint8_t led_off_period, led_on_period;
        uint16_t led_repeat; /* Repeat type and duration. */
        
        led_index = p_cmd[2];
        led_RGB_Brigthness = p_cmd[3]; 
        memcpy(led_flash_pattern, &p_cmd[4], 5);
        led_off_period = p_cmd[9];
        led_on_period = p_cmd[10];
        led_repeat = LE_TO_HOST_16(&p_cmd[11]);
        
        LOG_DEBUG_APP("LED CONTROL command [opcode: 0x%02x] \n", opcode);
        ret = LEDControlCmdCB(led_index, led_RGB_Brigthness, led_flash_pattern, led_off_period, led_on_period, led_repeat);
        
        if(ret != 0 && esl_cmd_id != BRC_ESL_ID)
        {
          /* Error */
          esl_payload_resp[resp_idx] = ESL_RESP_ERROR;
          esl_payload_resp[resp_idx + 1] = ret; /* ERROR_INVALID_PARAMETERS */
          resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
        }
        
        if(ret == 0 && esl_cmd_id != BRC_ESL_ID)
        {            
          //TBR: to check if response exceeds ESL payload size.
          esl_payload_resp[resp_idx] = ESL_RESP_LED_STATE;
          esl_payload_resp[resp_idx + 1] = led_index;
          resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
        }
      }
      break;
    case ESL_CMD_LED_TIMED_CONTROL:
      {                
        uint8_t led_index;
        uint8_t led_RGB_Brigthness;
        uint16_t led_repeat; /* Repeat type and duration. */
        uint8_t led_flash_pattern[5];
        uint8_t led_off_period, led_on_period;
        uint32_t led_abs_time; 
        
        LOG_DEBUG_APP("LED TIMED CONTROL command [opcode: 0x%02x] \n", opcode);
        
        led_index = p_cmd[2];
        led_RGB_Brigthness = p_cmd[3]; 
        memcpy(led_flash_pattern, &p_cmd[4], 5);
        led_off_period = p_cmd[9];
        led_on_period = p_cmd[10];
        led_repeat = LE_TO_HOST_16(&p_cmd[11]);
        led_abs_time = LE_TO_HOST_32(&p_cmd[13]);
        
        ret = LEDTimedControlCmdCB(led_index,  led_RGB_Brigthness, led_flash_pattern, led_off_period, led_on_period, led_repeat, led_abs_time);
        
        if(ret != 0 && esl_cmd_id != BRC_ESL_ID)
        {
          /* Error */
          esl_payload_resp[resp_idx] = ESL_RESP_ERROR;
          esl_payload_resp[resp_idx + 1] = ret; /* ERROR_INVALID_PARAMETERS or ERROR_IMPLAUSIBLE_ABSOLUTE_TIME or ERROR_QUEUE_FULL */
          resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
        }
        
        if(ret == 0 && esl_cmd_id != BRC_ESL_ID)
        {            
          //TBR: to check if response exceeds ESL payload size.
          esl_payload_resp[resp_idx] = ESL_RESP_LED_STATE;
          esl_payload_resp[resp_idx + 1] = led_index;
          resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
        }
      }
      break;
#endif
#if NUM_SENSORS
    case ESL_CMD_READ_SENSOR_DATA:
      {
        uint8_t sensor_index;
        uint8_t sensor_data_length;
        
        LOG_DEBUG_APP("READ SENSOR DATA command [opcode: 0x%02x] \n", opcode);
        
        if(esl_cmd_id == BRC_ESL_ID)
        {
          /* No response can be sent for broadcast command. Do not invoke callback. */
          break;
        }
                
        sensor_index = p_cmd[2];
        
        if (sensor_index >= NUM_SENSORS)
        {
          ret = ERROR_INVALID_PARAMETERS;
        }
        else
        {
          ret = ESL_DEVICE_SensorDataCmdCB(sensor_index, &esl_payload_resp[resp_idx + 2], &sensor_data_length);
        }
        
        if(ret != 0)
        {
          esl_payload_resp[resp_idx] = ESL_RESP_ERROR;
          esl_payload_resp[resp_idx + 1] = ret;
          resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
        }
        
        if(ret == 0)
        {
          esl_payload_resp[resp_idx] = SET_LENGTH_TO_OPCODE(ESL_RESP_SENSOR_VALUE_TAG_NIBBLE, sensor_data_length + 1);            
          esl_payload_resp[resp_idx + 1] = sensor_index;            
          resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
        }
      }
      break;
#endif
    case ESL_CMD_VS_TXT:
      {
        LOG_DEBUG_APP("TXT VENDOR command [opcode: 0x%02x] \n", opcode);
        
        ret = ESL_DEVICE_TxtVsCmdCB(param_length - 1, (char *)&p_cmd[2]);
        
        if(ret != 0 && esl_cmd_id != BRC_ESL_ID)
        {
          esl_payload_resp[resp_idx] = ESL_RESP_ERROR;
          esl_payload_resp[resp_idx + 1] = ret;
          resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
        }
        
        if(ret == 0 && esl_cmd_id != BRC_ESL_ID)
        {
          esl_payload_resp[resp_idx] = ESL_RESP_VS_OK;
          esl_payload_resp[resp_idx + 1] = 0; /* Not used. */
          resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
        }
        
      }
      break;
    case ESL_CMD_VS_PRICE:
      {        
        LOG_DEBUG_APP("PRICE VENDOR command [opcode: 0x%02x] \n", opcode);
        
        ret = ESL_DEVICE_PriceVsCmdCB(LE_TO_HOST_16(&p_cmd[2]), p_cmd[4]);
        
        if(ret != 0 && esl_cmd_id != BRC_ESL_ID)
        {
          esl_payload_resp[resp_idx] = ESL_RESP_ERROR;
          esl_payload_resp[resp_idx + 1] = ret;
          resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
        }
        
        if(ret == 0 && esl_cmd_id != BRC_ESL_ID)
        {
          esl_payload_resp[resp_idx] = ESL_RESP_VS_OK;
          esl_payload_resp[resp_idx + 1] = 0; /* Not used. */
          resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
        }
        
      }
      break;
    /* When the Update Complete command is received by an ESL, the ESL shall 
       synchronize with the AP and then disconnect the link with the AP. */
    case ESL_CMD_UPDATE_COMPLETE:
      {        
        LOG_DEBUG_APP("UPDATE COMPLETE command [opcode: 0x%02x] \n", opcode);
        
        ESLS_APP_Context.upd_compl_recvd = true;
        
        /* If an ESL receives the Update Complete command and it is synchronized, 
           the ESL shall immediately terminate the ACL connection and transition 
           to the Synchronized state. */
        if (ESLS_APP_Context.sync_recvd)
        {    
          ret = hci_le_set_periodic_sync_subevent(ESLS_APP_Context.sync_handle,
                                                  0, /* Periodic_Advertising_Properties */
                                                  1, /* Num_Subevents */
                                                  &ESLS_APP_Context.group_id);
          if (ret != BLE_STATUS_SUCCESS)
          {
            LOG_ERROR_APP("hci_le_set_periodic_sync_subevent fail (0x%02x)\n", ret);
          }
          
          LOG_DEBUG_APP("*** Synchronized State\n");
          ESLS_APP_Context.state = ESL_STATE_SYNCHRONIZED;
          /* Delay for disconnection */
          UTIL_TIMER_Start(&ESLS_APP_Context.Disconnection_timer_Id);
        }
        /* If an ESL receives the Update Complete command and it is not synchronized, 
           the ESL shall wait for synchronization to be established and then terminate 
           the ACL connection and transition to the Synchronized state. */
        
        /* The Update Complete Command has NO RESPONSE */
      }
      break;
  
    case ESL_CMD_UNASSOCIATE_FROM_AP:
      {          
        LOG_DEBUG_APP("UNASSOCIATE FROM AP command [opcode: 0x%02x] \n", opcode);
        
        if(esl_cmd_id != BRC_ESL_ID)
        {  
          esl_payload_resp[resp_idx] = ESL_RESP_BASIC_STATE;
          //Basic State response bitmap parmam (16 bits)
          HOST_TO_LE_16(&esl_payload_resp[resp_idx + 1], ESLS_APP_Context.basic_state);
          resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
          
          /* Start a timer to unassociate after a while. Do not unassociate immediately,
             otherwise we will notbe able to send back the response. */
          UTIL_TIMER_StartWithPeriod(&ESLS_APP_Context.Unassociate_timer_Id, 1000);
        }
      }
      break; 
    case ESL_CMD_SERVICE_RESET:
      {          
        LOG_DEBUG_APP("SERVICE RESET command [opcode: 0x%02x] \n", opcode);
        
        ESL_DEVICE_ServiceResetCmdCB();
        
        if(esl_cmd_id != BRC_ESL_ID)
        {
          esl_payload_resp[resp_idx] = ESL_RESP_BASIC_STATE;
          //Basic State response bitmap parmam (16 bits)
          HOST_TO_LE_16(&esl_payload_resp[resp_idx + 1], ESLS_APP_Context.basic_state);
          resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
        }      
      }
      break;
    case ESL_CMD_FACTORY_RESET:
      {          
        LOG_DEBUG_APP("FACTORY RESET command [opcode: 0x%02x] \n", opcode);
        
        ret = factoryResetCmdCB();
        
        if(ret != 0 && esl_cmd_id != BRC_ESL_ID)
        {
          esl_payload_resp[resp_idx] = ESL_RESP_ERROR;
          esl_payload_resp[resp_idx + 1] = ret;         //Invalid State
          resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
        }       
      }
      break;
      
#if NUM_DISPLAYS > 0      
    case ESL_CMD_DISPLAY_IMG:
      {        
        LOG_DEBUG_APP("DISPLAY IMAGE command [opcode: 0x%02x] \n", opcode);
        
        ret = displayImageCmdCB(p_cmd[2], p_cmd[3]);
        
        if(ret != 0 && esl_cmd_id != BRC_ESL_ID)
        {
          esl_payload_resp[resp_idx] = ESL_RESP_ERROR;
          esl_payload_resp[resp_idx + 1] = ret;
          resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
        }
        
        if(ret == 0 && esl_cmd_id != BRC_ESL_ID)
        {
          esl_payload_resp[resp_idx] = ESL_RESP_DISPLAY_STATE;
          esl_payload_resp[resp_idx + 1] = p_cmd[2]; /* Display index */
          esl_payload_resp[resp_idx + 2] = p_cmd[3]; /* Image index */            
          resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
        }
      }
      break;      
    case ESL_CMD_REFRESH_DISPLAY:
      {
        uint8_t image_index;
        
        LOG_DEBUG_APP("REFRESH DISPLAY command [opcode: 0x%02x] \n", opcode);
        
        ret = refreshDisplayCmdCB(p_cmd[2], &image_index);
        
        if(ret != 0 && esl_cmd_id != BRC_ESL_ID)
        {
          esl_payload_resp[resp_idx] = ESL_RESP_ERROR;
          esl_payload_resp[resp_idx + 1] = ret;
          resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
        }
        
        if(ret == 0 && esl_cmd_id != BRC_ESL_ID)
        {
          esl_payload_resp[resp_idx] = ESL_RESP_DISPLAY_STATE;
          esl_payload_resp[resp_idx + 1] = p_cmd[2]; /* Display index */
          esl_payload_resp[resp_idx + 2] = image_index;
          resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
        }
      }
      break;
    case ESL_CMD_DISPLAY_TIMED_IMG:
      {    
        uint32_t img_abs_time; 
        
        LOG_DEBUG_APP("DISPLAY TIMED IMAGE command [opcode: 0x%02x] \n", opcode);
        
        img_abs_time = LE_TO_HOST_32(&p_cmd[4]);
        
        ret = displayTimedImageCmdCB(p_cmd[2], p_cmd[3], img_abs_time);
        
        if(ret != 0 && esl_cmd_id != BRC_ESL_ID)
        {
          esl_payload_resp[resp_idx] = ESL_RESP_ERROR;
          esl_payload_resp[resp_idx + 1] = ret;
          resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
        }
        
        if(ret == 0 && esl_cmd_id != BRC_ESL_ID)
        {
          esl_payload_resp[resp_idx] = ESL_RESP_DISPLAY_STATE;
          esl_payload_resp[resp_idx + 1] = p_cmd[2]; /* Display index */
          esl_payload_resp[resp_idx + 2] = p_cmd[3]; /* Image index */            
          resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
        }
      }
      break;          
#endif
    default:
      esl_payload_resp[resp_idx] = ESL_RESP_ERROR;
      esl_payload_resp[resp_idx + 1] = ERROR_INVALID_OPCODE;
      resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
      break;
    }
  return resp_idx;
}

static void disconnection_delay(void *arg)
{
  ESLS_APP_Context.disconnect = true;
  ESL_APP_ProcessRequest();
}

static void synch_packet_received(uint16_t pa_event, uint8_t *p_esl_data, uint8_t size)
{
  uint8_t opcode;
  uint8_t group_id;
  uint8_t param_length;
  uint8_t esl_cmd_id;
  int8_t tlv_num = -1, relevant_cmd_tlv_num = -1;
  uint8_t *p_cmd;
  uint8_t resp_idx = 2; /* Reserve 2 extra bytes for AD header */
  uint8_t esl_payload_resp[MAX_ESL_PAYLOAD_SIZE + 2]; /* 2 extra bytes needed for ESL AD type and length */
  
  group_id = p_esl_data[0] & 0x7F;
  
  if(group_id != ESLS_APP_Context.group_id)
    return;
  
  p_cmd = &p_esl_data[1];
  
  while(p_cmd < p_esl_data + size - 1) /* Shortest command is 2 bytes. */
  {
    param_length = ((p_cmd[0] & 0xF0) >> 4) + 1;
    esl_cmd_id = p_cmd[1]; /* First cmd parameter is always the ESL_ID */
    tlv_num += 1;
    
    if(esl_cmd_id == ESLS_APP_Context.esl_id || esl_cmd_id == BRC_ESL_ID)
    {
      /* Identify the relevant command to choose response slot. */
      /* Broadcast messages shall be disregarded because do not elicit a response. */
      if( esl_cmd_id != BRC_ESL_ID)
      {
        relevant_cmd_tlv_num = tlv_num;
      }
      opcode = p_cmd[0];
      resp_idx = TLV_OpCode_handling(p_cmd, opcode, esl_cmd_id, esl_payload_resp, param_length, resp_idx);
    }
    p_cmd += (param_length + 1); /* increment the iterator to go on next element*/
  }
  
  if(relevant_cmd_tlv_num >= 0)
  {
    /* Send the response */
    esl_payload_resp[0] = resp_idx - 1;
    esl_payload_resp[1] = AD_TYPE_ELECTRONIC_SHELF_LABEL;
    
    send_resp(pa_event, relevant_cmd_tlv_num, esl_payload_resp, resp_idx);
  }
  
}

static void send_resp(uint16_t pa_event, uint8_t resp_slot, uint8_t *p_esl_resp, uint8_t resp_size)
{
  tBleStatus ret;
  uint16_t decrypted_data_length;
  
  ESLS_APP_Context.a_resp[0] = resp_size + EAD_RANDOMZER_SIZE + EAD_MIC_SIZE + 1;
  ESLS_APP_Context.a_resp[1] = AD_TYPE_ENCRYPTED_ADVERTISING_DATA;
  
  ret = aci_hal_ead_encrypt_decrypt(0, /* Encryption */
                                    ESLS_APP_Context.esl_resp_key_material.Session_Key,
                                    ESLS_APP_Context.esl_resp_key_material.IV,
                                    resp_size,
                                    p_esl_resp,
                                    &decrypted_data_length,
                                    &ESLS_APP_Context.a_resp[2]);
  if(ret != BLE_STATUS_SUCCESS)
  {
    LOG_ERROR_APP("Encryption fail.\n");
    return;
  }
  
  ret = hci_le_set_periodic_advertising_response_data(ESLS_APP_Context.sync_handle,
                                                      pa_event,
                                                      ESLS_APP_Context.group_id,
                                                      ESLS_APP_Context.group_id,
                                                      resp_slot,
                                                      ESLS_APP_Context.a_resp[0] + 1,
                                                      ESLS_APP_Context.a_resp);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_ERROR_APP("hci_le_set_periodic_advertising_response_data failure: reason=0x%02X, Sync_Handle 0x%04X, Request_Event %d, Subevent %d, Response_Slot %d\n", ret,
                ESLS_APP_Context.sync_handle,
                pa_event,
                ESLS_APP_Context.group_id,
                resp_slot);
  }
  else
  {
    LOG_DEBUG_APP("==>> hci_le_set_periodic_advertising_response_data: Success\n");
  }
}

void ESL_APP_ControlPointReceived(uint8_t *p_cmd, uint8_t size)
{
  uint8_t opcode = 0;
  uint8_t param_length;
  uint8_t esl_cmd_id;
  
  uint8_t resp_idx = 0;
  uint8_t esl_payload_resp[MAX_ESL_PAYLOAD_SIZE]; 
  
  param_length = ((p_cmd[0] & 0xF0) >> 4) + 1;
  esl_cmd_id = p_cmd[1]; /* First cmd parameter is always the ESL_ID */
  
  /* ESL service specification 3.9.2 Command behavior: If an opcode,  ESL_ID  
     value does not match the ESL_ID of the ESL or matches the Broadcast Address,  
     then the ESL shall reject the command by responding with the Error response: 
     Invalid Parameter(s). */
  if(esl_cmd_id != ESLS_APP_Context.esl_id || esl_cmd_id == BRC_ESL_ID)
  {
    esl_payload_resp[resp_idx] = ESL_RESP_ERROR;
    esl_payload_resp[resp_idx + 1] = ERROR_INVALID_PARAMETERS;
    resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
  }
  else
  {
    /* If, after receipt of the Factory Reset cmd and prior to disconnection from
       the AP, the ESL receives any other cmd from the AP written to the ECP char,
       then the other cmd shall be rejected with the error code: "Unspecified Error" */
    if ((ESLS_APP_Context.bFactoryReset) && (ESLS_APP_Context.connected))
    {
      esl_payload_resp[resp_idx] = ESL_RESP_ERROR;
      esl_payload_resp[resp_idx + 1] = ERROR_UNSPECIFIED;
      resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
    }
    else
    {  
      opcode = p_cmd[0]; /* TLV OpCode */
      /* To handle control point characteristic write TLV management */
      resp_idx = TLV_OpCode_handling(p_cmd, opcode, esl_cmd_id, esl_payload_resp, param_length, resp_idx); 
    }
  } /* end else */
  
  /* Send response through notification */
    
  if (resp_idx > 0)
  {  
    ESLS_Data_t eslResp;
    
    eslResp.p_Payload = esl_payload_resp;
    eslResp.Length = resp_idx;
    
    LOG_DEBUG_APP("RESPONSE opcode: 0x%02x \n", eslResp.p_Payload[0]);
    
    ESLS_UpdateValue(ESLS_ESL_CONTROL_POINT, &eslResp);
  }
}

uint8_t ESL_APP_ConfiguringOrUpdatingState(void)
{
  if((ESLS_APP_Context.state == ESL_STATE_CONFIGURING) || 
     (ESLS_APP_Context.state == ESL_STATE_UPDATING))
    return 1;
  else 
    return 0;
}

static void ESL_APP_UnsynchronizedState(void)
{
  ESLS_APP_Context.state = ESL_STATE_UNSYNCHRONIZED;
  /* In the Unsynchronized state, the ESL shall enter a GAP connectable mode. 
     If a connection is formed with a Client, then the ESL transitions to the 
     Updating */
  APP_BLE_StartAdvertising(true);
  /* If the ESL is not moved to the Updating state for 60 minutes, then the 
     ESL shall transition to the Unassociated state, and shall remove all 
     bonding information with the AP and delete the value of the AP Sync 
     Key Material in internal storage. */
  UTIL_TIMER_Start(&ESLS_APP_Context.Unassociate_timer_Id);
}  

// Synchronized_timer_Id callback
/* If the ESL has not received a valid ESL message in a synchronization message from
   the AP for 60 minutes then the ESL shall transition to the Unsynchronized state. */
static void ESL_APP_UnsynchronizedStateTimerCB(void *arg)
{
  if(ESLS_APP_Context.state == ESL_STATE_SYNCHRONIZED)
  {
    ESLS_APP_Context.state = ESL_STATE_TO_UNSYNCHRONIZED;
    ESL_APP_ProcessRequest();
  }
}

// Unassociate_timer_Id callback
/* If the ESL is not moved to the Updating state for 60 minutes, then the 
   ESL shall transition to the Unassociated state */
static void ESL_APP_UnassociatedStateTimerCB(void *arg)
{
  ESLS_APP_Context.state = ESL_STATE_TO_UNASSOCIATED;
  ESL_APP_ProcessRequest();
}

static void ESL_APP_UpdatingStateTransition(uint16_t sync_handle)
{ 
  if (sync_handle != 0xFFFF)
  {
    ESLS_APP_Context.sync_handle = sync_handle;
    /* To transition an ESL from the Synchronized state to the Updating state, the 
       AP shall use the Periodic Advertising Connection procedure. When the AP 
       connects with the ESL, the ESL transitions to the Updating state. */      
    if(ESLS_APP_Context.state == ESL_STATE_SYNCHRONIZED)
    {  
      /* The ESL shall transition to the Updating state only if the peer device 
         is the Client with which the ESL is already associated. */  
      if(aci_gap_is_device_bonded(ESLS_APP_Context.Peer_Address_Type, ESLS_APP_Context.Peer_Address) == BLE_STATUS_SUCCESS)
      {
        ESLS_APP_Context.state = ESL_STATE_UPDATING;
        LOG_DEBUG_APP("*** Updating state transition from Synchronized state\n"); 
        //Stop the timer
        UTIL_TIMER_Stop(&ESLS_APP_Context.Synchronized_timer_Id);
      }  
    }  
  }  
  else
  {
    /* In the Unsynchronized state, if a connection is formed with a Client, 
       then the ESL transitions to the Updating */
    if (ESLS_APP_Context.state == ESL_STATE_UNSYNCHRONIZED)
    {
      ESLS_APP_Context.state = ESL_STATE_UPDATING;
      LOG_DEBUG_APP("*** Updating state transition from Unsynchronized state\n"); 
      //Stop the timer
      UTIL_TIMER_Stop(&ESLS_APP_Context.Unassociate_timer_Id);
    }  
  }  
}

int ESL_APP_GetAddress(uint8_t *group_id_p, uint8_t *esl_id_p)
{
  if(ESLS_APP_Context.state < ESL_STATE_SYNCHRONIZED)
  {
    return -1;
  }
  
  *group_id_p = ESLS_APP_Context.group_id;
  *esl_id_p = ESLS_APP_Context.esl_id;
  
  return 0;
}

void ESL_APP_PairingRequest(uint16_t connHandle)
{
  Bonded_Device_Entry_t bonded_devices;
  uint8_t num_devices = 0;
  
  /* The Server shall reject any pairing requests that are received 
     while the Server is in the Updating state */
  if (ESLS_APP_Context.state == ESL_STATE_UPDATING)
  {
    //reject any pairing request
    aci_gap_pairing_request_reply(connHandle, 0);
    return;
  }   
  /* An ESL shall be bonded with a maximum of one AP at any one time. */
  aci_gap_get_bonded_devices(&num_devices, &bonded_devices);
  
  if (num_devices == 0)
  {
    /* Accept pairing request */
    LOG_INFO_APP("Pairing accepted\n");
    aci_gap_pairing_request_reply(connHandle, 1);
  }
  else
  {
    /* Reject pairing */
    LOG_INFO_APP("Pairing rejected\n");
    aci_gap_pairing_request_reply(connHandle, 0);    
  }
}    

/* Return 1 if the basic_resp_bit is already set, else set the basic_resp_bit
   on Basic State bitmap and return 0 */
uint8_t ESL_APP_SetBasicStateBitmap(uint8_t basic_resp_bit)
{
  if(ESLS_APP_Context.basic_state & basic_resp_bit) 
  {
    return 1;
  }  
  else
  {  
    ESLS_APP_Context.basic_state |= basic_resp_bit;
    return 0;
  }
}

void ESL_APP_ResetBasicStateBitmap(uint8_t basic_resp_bit)
{
  ESLS_APP_Context.basic_state &= (~basic_resp_bit);
}

void ESL_APP_SerialCmdExecute(uint8_t * pRxBuffer, uint16_t iRxBufferSize)
{
  memcpy(CommandString, pRxBuffer, MIN(iRxBufferSize, C_SIZE_CMD_STRING));
  ESL_APP_CmdProcessRequestCB();
}

static int parse_cmd(void)
{
  if(strncasecmp((char *)CommandString, "HELP", 4) == 0)
  {
    LOG_INFO_APP("List of commands useful for tests: \n");
    LOG_INFO_APP("  - ABSTIME: Get Current Absolute Time\n");
    LOG_INFO_APP("  - SRVNEEDED: Set Service Needed bit to True \n");
    LOG_INFO_APP("  - UNSYNC: Set The ESL state to Unsynchronized \n");
    LOG_INFO_APP("  - NODISPLAY: Each display is not displaying an image \n");
    return 0;
  } 
  else if(strncasecmp((char *)CommandString, "ABSTIME", 7) == 0)
  {
    LOG_INFO_APP("--> Get Current Absolute Time: %d\n", TIMEREF_GetCurrentAbsTime());
    return 0;
  } 
  else if(strncasecmp((char *)CommandString, "SRVNEEDED", 9) == 0)
  {
    LOG_INFO_APP("--> Set Service Needed bit to True\n");
    ESL_APP_SetBasicStateBitmap(BASIC_STATE_SERVICE_NEEDED_BIT);
    return 0;
  } 
  else if(strncasecmp((char *)CommandString, "UNSYNC", 6) == 0)
  {
    LOG_INFO_APP("--> Set The ESL state to Unsynchronized\n");    
    hci_le_periodic_advertising_terminate_sync(ESLS_APP_Context.sync_handle);
    ESLS_APP_Context.sync_recvd = false;
    ESL_APP_UnsynchronizedState();
    return 0;
  }
  return 1;
}

void ESL_APP_CmdProcess(void)
{
  if(parse_cmd() == 0)
  {
    LOG_INFO_APP("OK\r\n");   
  }
  else
  {
    LOG_INFO_APP("ERROR\r\n");
  }
  
  indexReceiveChar = 0; 
}

static uint8_t factoryResetCmdCB(void)
{
  /* If an ESL in the Synchronized state receives a Factory Reset command, 
     then the ESL shall send the Error response: Invalid State.*/
  if (ESLS_APP_Context.state == ESL_STATE_SYNCHRONIZED)
  {
    ESLS_APP_Context.bFactoryReset = false;
    return ERROR_INVALID_STATE;
  }  
  if ((ESLS_APP_Context.state == ESL_STATE_CONFIGURING) ||
      (ESLS_APP_Context.state == ESL_STATE_UPDATING)) 
  {
    ESLS_APP_Context.bFactoryReset = true;
    /* The ESL shall initiate disconnection of the link with the AP */
    UTIL_TIMER_Start(&ESLS_APP_Context.Disconnection_timer_Id);
  }
  
  return 0;
}

#if NUM_LEDS

/* Returns if any LED is set to active */
static bool get_active_led_state(void)
{
  for(uint8_t i = 0; i < sizeof(led_states); i++)
  {
    if(led_states[i] != 0)
      return true;
  }
  
  return false;
}

void ESL_APP_SetLEDState(uint8_t index, ESL_APP_LEDState_t led_state)
{
  uint8_t byte_idx; /* Index of the byte inside the array */
  uint8_t bit_offset; /* Index of the bit inside the byte */
  
  byte_idx = index / 8;  
  bit_offset = index % 8;
  
  if(led_state == ESL_LED_INACTIVE)
  {
    led_states[byte_idx] &= ~(1 << bit_offset);
    
    if(get_active_led_state() == false)
    {
      ESL_APP_ResetBasicStateBitmap(BASIC_STATE_ACTIVE_LED_BIT);
    }
  }
  else
  {
    led_states[byte_idx] |= (1 << bit_offset);
    
    if(get_active_led_state() == true)
    {
      ESL_APP_SetBasicStateBitmap(BASIC_STATE_ACTIVE_LED_BIT);
    }
  }
}

static uint8_t LEDControlCmdCB(uint8_t led_index, uint8_t led_RGB_Brigthness, uint8_t led_flash_pattern[5], uint8_t off_period, uint8_t on_period, uint16_t led_repeat)
{
  uint16_t repeat_duration = led_repeat >> 1;                   //other 15 bits of led_repeat
  
  if (led_index >= NUM_LEDS)
  {
    return ERROR_INVALID_PARAMETERS;
  }
  
  if (repeat_duration != 0)
  {  
    /* A Bit_Off_Period  or Bit_On_Period value of 0 ms is invalid */
    if ((off_period == 0) || (on_period == 0))
    {
      return ERROR_INVALID_PARAMETERS;
    } 
  }
  
  ESL_DEVICE_LEDControlCmdCB(led_index, led_RGB_Brigthness, led_flash_pattern, off_period, on_period, led_repeat);
  
  return 0;
}

static uint8_t LEDTimedControlCmdCB(uint8_t led_index, uint8_t led_RGB_Brigthness, uint8_t led_flash_pattern[5], uint8_t off_period, uint8_t on_period, uint16_t led_repeat, uint32_t abs_time)
{
  uint32_t curr_abs_time;
  uint32_t delay;
  uint16_t repeat_duration = led_repeat >> 1;
  
  if (led_index >= NUM_LEDS)
  {
    return ERROR_INVALID_PARAMETERS;
  }
  
  if (repeat_duration != 0)
  {  
    /* A Bit_Off_Period  or Bit_On_Period value of 0 ms is invalid */
    if ((off_period == 0) || (on_period == 0))
    {
      return ERROR_INVALID_PARAMETERS;
    } 
  }
  
  if(led_timed_info[led_index].pend_state != TIMED_CMD_NOT_PENDING)
  {  
    /* An LED Timed Control command is received while an LED Timed Control command is already pending */
    if (abs_time == 0x00000000) 
    {
      /* If the value of the Absolute Time parameter is zero (0x00000000), 
      then the pending LED Timed Control command shall be deleted.*/
      UTIL_TIMER_Stop(&led_timed_info[led_index].timer);
      led_timed_info[led_index].pend_state = TIMED_CMD_NOT_PENDING;
      LOG_DEBUG_APP("Pending LED command deleted\n");
      checkPendingLedUpdate();
      return 0;
    }
  }
  
  curr_abs_time = TIMEREF_GetCurrentAbsTime();
  delay = abs_time - curr_abs_time;
  
  LOG_DEBUG_APP("Current time: %d\n", curr_abs_time); 
  LOG_DEBUG_APP("Requested time: %d\n", abs_time);
  
  if (delay > MAX_TIMED_CMD_DELAY_MS) 
  {
    //Absolute time is more than 48 days
    return ERROR_IMPLAUSIBLE_ABSOLUTE_TIME;
  }
  
  if(led_timed_info[led_index].pend_state != TIMED_CMD_NOT_PENDING && led_timed_info[led_index].abs_time != abs_time)
  {
    LOG_WARNING_APP("Queue full\n");
    /* The ESL shall send the Error response: Queue Full. The LED Timed 
    Control command that was already pending remains unchanged. */
    return ERROR_QUEUE_FULL;
  }
  
  led_timed_info[led_index].color_brigthness = led_RGB_Brigthness;
  memcpy(led_timed_info[led_index].pattern, led_flash_pattern, sizeof(led_timed_info[led_index].pattern));
  led_timed_info[led_index].off_period = off_period;
  led_timed_info[led_index].on_period = on_period;
  led_timed_info[led_index].repeat = led_repeat;
  led_timed_info[led_index].abs_time = abs_time;
  
  if(led_timed_info[led_index].pend_state != TIMED_CMD_NOT_PENDING)
  {
    LOG_DEBUG_APP("Pending LED command replaced\n");
  }
  else
  {
    if(delay == 0)
    {
      /* Immediately call the callback. */
      ESL_DEVICE_LEDControlCmdCB(led_index, led_RGB_Brigthness, led_flash_pattern, off_period, on_period, led_repeat); 
    }
    else
    {
      UTIL_TIMER_Create(&led_timed_info[led_index].timer,
                        delay,
                        UTIL_TIMER_ONESHOT,
                        LED_Timed_Cmd_timeout_cb, &led_timed_info[led_index]);
      UTIL_TIMER_Start(&led_timed_info[led_index].timer);
      led_timed_info[led_index].pend_state = TIMED_CMD_TIMER_RUNNING;
      ESL_APP_SetBasicStateBitmap(BASIC_STATE_PENDING_LED_UPDATE_BIT);
      LOG_DEBUG_APP("Timer started. Pending LED Update bit set\n");
    }
  }
  
  return 0;
}

/* Reset Pending LED Update bit if no pending commands are present. */
static int checkPendingLedUpdate(void)
{
  uint16_t i;
  
  /* Search for pending LED commands */
  for(i = 0; i < NUM_LEDS; i++)
  {
    if(led_timed_info[i].pend_state == TIMED_CMD_EXECUTION_PENDING)
    {
      return i;
    }
  }
  /* No pending commands */
  ESL_APP_ResetBasicStateBitmap(BASIC_STATE_PENDING_LED_UPDATE_BIT);
  LOG_DEBUG_APP("Pending LED Update bit reset\n");
  return -1;
}

static void LED_Timed_Cmd_timeout_cb(void *arg)
{
  LEDTimedInfo_t *led_timed_info_p = arg;
  
  led_timed_info_p->pend_state =  TIMED_CMD_EXECUTION_PENDING;
  
  ESL_APP_ProcessRequest();
}

#endif

#if NUM_DISPLAYS > 0

static uint8_t displayImageCmdCB(uint8_t display_index, uint8_t image_index)
{
  LOG_DEBUG_APP("Display Index: %d - Image Index: %d\n", display_index, image_index);
  if (display_index >= NUM_DISPLAYS)
  {
    return ERROR_INVALID_PARAMETERS;
  }  
  if (image_index >= NUM_IMAGES)
  {
    return ERROR_INVALID_IMAGE_INDEX;
  }
  
  return ESL_DEVICE_DisplayImageCmdCB(display_index, image_index);
}

static uint8_t displayTimedImageCmdCB(uint8_t display_index, uint8_t image_index, uint32_t abs_time)
{
  uint32_t curr_abs_time;
  uint32_t delay;
  uint8_t ret;
  
  if (display_index >= NUM_DISPLAYS)
    return ERROR_INVALID_PARAMETERS;
  
  if (image_index >= NUM_IMAGES)
    return ERROR_INVALID_IMAGE_INDEX;
  
  if(display_timed_info[display_index].pend_state != TIMED_CMD_NOT_PENDING)
  {  
    /* An Display Timed Image command is received while a Display Timed Image command is already pending */
    if (abs_time == 0x00000000) 
    {
      /* If the value of the Absolute Time parameter is zero (0x00000000), 
      then the pending Display Timed Image command shall be deleted.*/
      UTIL_TIMER_Stop(&display_timed_info[display_index].timer);
      display_timed_info[display_index].pend_state = TIMED_CMD_NOT_PENDING;
      LOG_DEBUG_APP("Pending Display command deleted\n");
      checkPendingDisplayUpdate();
      return 0;
    }
  }
  
  curr_abs_time = TIMEREF_GetCurrentAbsTime();
  delay = abs_time - curr_abs_time;
  
  LOG_DEBUG_APP("Current time: %d\n", curr_abs_time); 
  LOG_DEBUG_APP("Requested time: %d\n", abs_time);

  if (delay > MAX_TIMED_CMD_DELAY_MS) 
  {
    //Absolute time is more than 48 days
    return ERROR_IMPLAUSIBLE_ABSOLUTE_TIME;
  }
  
  if(display_timed_info[display_index].pend_state != TIMED_CMD_NOT_PENDING && display_timed_info[display_index].abs_time != abs_time)
  {
    LOG_WARNING_APP("Queue full\n");
    /* The ESL shall send the Error response: Queue Full. The LED Timed 
    Control command that was already pending remains unchanged. */
    return ERROR_QUEUE_FULL;
  }
  
  /* Call a callback to understand if image is valid */
  ret = ESL_DEVICE_DisplayTimedImageCmdCB(image_index);
  if(ret != 0)
    return ret;
  
  display_timed_info[display_index].image_index = image_index;
  display_timed_info[display_index].abs_time = abs_time;
  
  if(display_timed_info[display_index].pend_state != TIMED_CMD_NOT_PENDING)
  {
    LOG_DEBUG_APP("Pending Display command replaced\n");
  }
  else
  {
    if(delay == 0)
    {
      /* Immediately call the callback. */
      ESL_DEVICE_DisplayImageCmdCB(display_index, image_index);
    }
    else
    {
      UTIL_TIMER_Create(&display_timed_info[display_index].timer,
                        delay,
                        UTIL_TIMER_ONESHOT,
                        Img_Timed_Cmd_timeout_cb, &display_timed_info[display_index]);
      
      UTIL_TIMER_Start(&display_timed_info[display_index].timer);
      display_timed_info[display_index].pend_state = TIMED_CMD_TIMER_RUNNING;
      ESL_APP_SetBasicStateBitmap(BASIC_STATE_PENDING_DISPLAY_UPDATE_BIT);
      LOG_DEBUG_APP("Timer started. Pending Display Update bit set\n");
    }
  }
    
  return 0;
}

/* Reset Pending Display Update bit if no pending commands are present. */
static int checkPendingDisplayUpdate(void)
{
  uint16_t i;
  
  /* Search for pending LED commands */
  for(i = 0; i < NUM_DISPLAYS; i++)
  {
    if(display_timed_info[i].pend_state == TIMED_CMD_EXECUTION_PENDING)
    {
      return i;
    }
  }  
  /* No pending commands */
  ESL_APP_ResetBasicStateBitmap(BASIC_STATE_PENDING_DISPLAY_UPDATE_BIT);
  LOG_DEBUG_APP("Pending Display Update bit reset\n");
  return -1;
}

static void Img_Timed_Cmd_timeout_cb(void *arg)
{
  DisplayTimedInfo_t *display_timed_info_p = arg;
  
  display_timed_info_p->pend_state =  TIMED_CMD_EXECUTION_PENDING;
  
  /* Request process of timed command. */
  ESL_APP_ProcessRequest();
}

static uint8_t refreshDisplayCmdCB(uint8_t display_index, uint8_t *image_index_p)
{ 
  LOG_DEBUG_APP("Refresh Display Command - Index: %d\n", display_index);

  /* If the display identified in the parameter value does not exist, or if there 
     is no image currently being displayed on the specified display, the ESL shall
     send an Error response. */
  
  if (display_index >= NUM_DISPLAYS)
  {
    return ERROR_INVALID_PARAMETERS;
  }

  return ESL_DEVICE_RefreshDisplayCmdCB(display_index, image_index_p);
}

#endif

static void ESL_APP_ProcessRequest(void)
{
  UTIL_SEQ_SetTask(1<<CFG_TASK_ESL_PROCESS, CFG_SEQ_PRIO_0);
}

void ESL_APP_Process(void)
{
  int idx;
  
  LOG_DEBUG_APP("ESL_APP_Process\n");
  
#if NUM_DISPLAYS
  /* Search for pending Display commands */
  do{
    
    idx = checkPendingDisplayUpdate();
    
    if(idx >= 0)
    {
      ESL_DEVICE_DisplayImageCmdCB(idx, display_timed_info[idx].image_index);
      
      display_timed_info[idx].pend_state = TIMED_CMD_NOT_PENDING;
    }
  
  }while(idx >=0);
#endif
  
#if NUM_LEDS
  /* Search for pending LED commands */
  do {
    
    idx = checkPendingLedUpdate();
    
    if(idx >= 0)
    {
      ESL_DEVICE_LEDControlCmdCB(idx,
                                 led_timed_info[idx].color_brigthness,
                                 led_timed_info[idx].pattern,
                                 led_timed_info[idx].off_period,
                                 led_timed_info[idx].on_period,
                                 led_timed_info[idx].repeat);
      
      led_timed_info[idx].pend_state = TIMED_CMD_NOT_PENDING;
    }
    
  }while(idx >=0);
#endif
  
  switch(ESLS_APP_Context.state)
  {
  case ESL_STATE_TO_UNSYNCHRONIZED:
    
    ESLS_APP_Context.state = ESL_STATE_UNSYNCHRONIZED;
    LOG_DEBUG_APP("Unsynchronized state for timeout\n");
    hci_le_periodic_advertising_terminate_sync(ESLS_APP_Context.sync_handle);
    ESLS_APP_Context.sync_recvd = false;
    ESL_APP_UnsynchronizedState();
    
    break;
    
  case ESL_STATE_TO_UNASSOCIATED:
    
    hci_le_periodic_advertising_terminate_sync(ESLS_APP_Context.sync_handle);
    ESLS_APP_Context.sync_recvd = false;
    
    ESLS_APP_Context.state = ESL_STATE_UNASSOCIATED;
    LOG_DEBUG_APP("*** Unassociated state\n");
    /* ESL shall remove all bonding information with the AP and delete the value 
    of the AP Sync Key Material in internal storage. */
    aci_gap_clear_security_db();       
    memset(ESLS_APP_Context.ap_sync_key_material.Session_Key, 0, 16);
    memset(ESLS_APP_Context.ap_sync_key_material.IV, 0, 8);
    ESLS_APP_Context.esl_id = -1;
    
#if NUM_LEDS
    
    for(int led_index = 0; led_index < NUM_LEDS; led_index++)
    {
      if(UTIL_TIMER_IsRunning(&led_timed_info[led_index].timer))
      {
        UTIL_TIMER_Stop(&led_timed_info[led_index].timer);
        LOG_DEBUG_APP("Pending LED command deleted\n");
      }
      led_timed_info[led_index].pend_state = TIMED_CMD_NOT_PENDING;
    }
    ESL_APP_ResetBasicStateBitmap(BASIC_STATE_PENDING_LED_UPDATE_BIT);
    
#endif
    
#if NUM_DISPLAYS
    
    for(int display_index = 0; display_index < NUM_DISPLAYS; display_index++)
    {
      if(UTIL_TIMER_IsRunning(&display_timed_info[display_index].timer))
      {
        UTIL_TIMER_Stop(&display_timed_info[display_index].timer);
        LOG_DEBUG_APP("Pending Display command deleted\n");
      }
      display_timed_info[display_index].pend_state = TIMED_CMD_NOT_PENDING;
    }
    ESL_APP_ResetBasicStateBitmap(BASIC_STATE_PENDING_DISPLAY_UPDATE_BIT);
    
#endif
    
    // Enter GAP undirected connectable mode.
    APP_BLE_StartAdvertising(false);
    
    break;
    
  default:
    break;
  }
  
  if(ESLS_APP_Context.disconnect)
  {
    ESLS_APP_Context.disconnect = false;
    APP_BLE_Procedure_Gap_General(PROC_GAP_GEN_CONN_TERMINATE);
  }
  
}

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void ESLS_Esl_control_point_SendNotification(void) /* Property Notification */
{
  ESLS_APP_SendInformation_t notification_on_off = Esl_control_point_NOTIFICATION_OFF;
  ESLS_Data_t esls_notification_data;

  esls_notification_data.p_Payload = (uint8_t*)a_ESLS_UpdateCharData;
  esls_notification_data.Length = 0;

  /* USER CODE BEGIN Service2Char9_NS_1 */

  /* USER CODE END Service2Char9_NS_1 */

  if (notification_on_off != Esl_control_point_NOTIFICATION_OFF)
  {
    ESLS_UpdateValue(ESLS_ESL_CONTROL_POINT, &esls_notification_data);
  }

  /* USER CODE BEGIN Service2Char9_NS_Last */

  /* USER CODE END Service2Char9_NS_Last */

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

/* USER CODE END FD_LOCAL_FUNCTIONS */
