/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service1_app.c
  * @author  MCD Application Team
  * @brief   service1_app application definition.
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_common.h"
#include "app_ble.h"
#include "ll_sys_if.h"
#include "dbg_trace.h"
#include "ble.h"
#include "dt_serv_app.h"
#include "dt_serv.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32_timer.h"
#include "stm32wbaxx_nucleo.h"
#include "host_stack_if.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
  DTS_APP_FLOW_OFF,
  DTS_APP_FLOW_ON
} DTS_App_Flow_Status_t;

typedef enum
{
  DTS_APP_TRANSFER_REQ_OFF,
  DTS_APP_TRANSFER_REQ_ON
} DTS_App_Transfer_Req_Status_t;

typedef struct
{
  DT_SERV_Data_t TxData;
  DTS_App_Transfer_Req_Status_t NotificationTransferReq;
  DTS_App_Transfer_Req_Status_t ButtonTransferReq;
  DTS_App_Flow_Status_t DtFlowStatus;
  uint8_t connectionstatus;
} DTS_App_Context_t;
/* USER CODE END PTD */

typedef enum
{
  Tx_char_NOTIFICATION_OFF,
  Tx_char_NOTIFICATION_ON,
  Through_char_NOTIFICATION_OFF,
  Through_char_NOTIFICATION_ON,
  /* USER CODE BEGIN Service1_APP_SendInformation_t */

  /* USER CODE END Service1_APP_SendInformation_t */
  DT_SERV_APP_SENDINFORMATION_LAST
} DT_SERV_APP_SendInformation_t;

typedef struct
{
  DT_SERV_APP_SendInformation_t     Tx_char_Notification_Status;
  DT_SERV_APP_SendInformation_t     Through_char_Notification_Status;
  /* USER CODE BEGIN Service1_APP_Context_t */
  /**
   * ID of the Write timeout
   */
  UTIL_TIMER_Object_t TimerDataThroughputWrite_Id;
  /* USER CODE END Service1_APP_Context_t */
  uint16_t              ConnectionHandle;
} DT_SERV_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */
extern uint16_t MTUSizeValue;
/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define DELAY_1S (1000)
#define TIMEUNIT  1

#define BOUNCE_THRESHOLD                  (20U)
#define LONG_PRESS_THRESHOLD              (1000U)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static DT_SERV_APP_Context_t DT_SERV_APP_Context;

uint8_t a_DT_SERV_UpdateCharData[247];

/* USER CODE BEGIN PV */
DTS_App_Context_t DTS_Context;
uint32_t DataReceived;
static uint8_t debug = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void DT_SERV_Tx_char_SendNotification(void);
static void DT_SERV_Through_char_SendNotification(void);

/* USER CODE BEGIN PFP */
static void BLE_App_Delay_DataThroughput(void);
static void DataThroughput_proc(void *arg);
static void SendData( void );
static uint8_t Notification_Data_Buffer[DATA_NOTIFICATION_MAX_PACKET_SIZE]; /* DATA_NOTIFICATION_MAX_PACKET_SIZE data + CRC */

extern uint16_t packet_lost;
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void DT_SERV_Notification(DT_SERV_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_Notification_1 */

  /* USER CODE END Service1_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_Notification_Service1_EvtOpcode */

    /* USER CODE END Service1_Notification_Service1_EvtOpcode */

    case DT_SERV_TX_CHAR_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char1_NOTIFY_ENABLED_EVT */
      DTS_Context.NotificationTransferReq = DTS_APP_TRANSFER_REQ_ON;
      UTIL_SEQ_SetTask(1U << CFG_TASK_DATA_TRANSFER_UPDATE_ID, CFG_SEQ_PRIO_0);
      /* USER CODE END Service1Char1_NOTIFY_ENABLED_EVT */
      break;

    case DT_SERV_TX_CHAR_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char1_NOTIFY_DISABLED_EVT */
      DTS_Context.NotificationTransferReq = DTS_APP_TRANSFER_REQ_OFF;
      /* USER CODE END Service1Char1_NOTIFY_DISABLED_EVT */
      break;

    case DT_SERV_RX_CHAR_READ_EVT:
      /* USER CODE BEGIN Service1Char2_READ_EVT */

      /* USER CODE END Service1Char2_READ_EVT */
      break;

    case DT_SERV_RX_CHAR_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN Service1Char2_WRITE_NO_RESP_EVT */
      if (DataReceived == 0)
      {
        /* start timer */
        DataReceived += p_Notification->DataTransfered.Length;
        UTIL_TIMER_Start(&(DT_SERV_APP_Context.TimerDataThroughputWrite_Id));
      }
      else
      {
        DataReceived += p_Notification->DataTransfered.Length;
      }
      /* USER CODE END Service1Char2_WRITE_NO_RESP_EVT */
      break;

    case DT_SERV_THROUGH_CHAR_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char3_NOTIFY_ENABLED_EVT */

      /* USER CODE END Service1Char3_NOTIFY_ENABLED_EVT */
      break;

    case DT_SERV_THROUGH_CHAR_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char3_NOTIFY_DISABLED_EVT */

      /* USER CODE END Service1Char3_NOTIFY_DISABLED_EVT */
      break;

    default:
      /* USER CODE BEGIN Service1_Notification_default */

      /* USER CODE END Service1_Notification_default */
      break;
  }
  /* USER CODE BEGIN Service1_Notification_2 */

  /* USER CODE END Service1_Notification_2 */
  return;
}

void DT_SERV_APP_EvtRx(DT_SERV_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_APP_EvtRx_1 */

  /* USER CODE END Service1_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_APP_EvtRx_Service1_EvtOpcode */

    /* USER CODE END Service1_APP_EvtRx_Service1_EvtOpcode */
    case DT_SERV_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_CONN_HANDLE_EVT */
      DTS_Context.connectionstatus = APP_BLE_CONNECTED_SERVER;

      /* USER CODE END Service1_APP_CONN_HANDLE_EVT */
      break;

    case DT_SERV_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_DISCON_HANDLE_EVT */

      /* USER CODE END Service1_APP_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN Service1_APP_EvtRx_default */

      /* USER CODE END Service1_APP_EvtRx_default */
      break;
  }

  /* USER CODE BEGIN Service1_APP_EvtRx_2 */

  /* USER CODE END Service1_APP_EvtRx_2 */

  return;
}

void DT_SERV_APP_Init(void)
{
  UNUSED(DT_SERV_APP_Context);
  DT_SERV_Init();

  /* USER CODE BEGIN Service1_APP_Init */

  /**
  * Create timer for Data Throughput process (write data)
  */
  UTIL_TIMER_Create(&(DT_SERV_APP_Context.TimerDataThroughputWrite_Id),
                    0,
                    UTIL_TIMER_ONESHOT,
                    &DataThroughput_proc, 
                    0);
  
  UTIL_TIMER_SetPeriod(&(DT_SERV_APP_Context.TimerDataThroughputWrite_Id), DELAY_1S);
  
  UTIL_SEQ_RegTask(1U << CFG_TASK_DATA_PHY_UPDATE_ID, UTIL_SEQ_RFU, BLE_SVC_GAP_Change_PHY);
  UTIL_SEQ_RegTask(1U << CFG_TASK_CONN_INTERV_UPDATE_ID, UTIL_SEQ_RFU, BLE_SVC_L2CAP_Conn_Update);
  
  UTIL_SEQ_RegTask(1U << CFG_TASK_DATA_TRANSFER_UPDATE_ID, UTIL_SEQ_RFU, SendData);
  UTIL_SEQ_RegTask(1U << CFG_TASK_DATA_WRITE_ID, UTIL_SEQ_RFU, BLE_App_Delay_DataThroughput);
  
  /**
   * Initialize data buffer
   */
  uint8_t i;
  for (i = 0 ; i< (DATA_NOTIFICATION_MAX_PACKET_SIZE - 1) ; i++)
  {
    Notification_Data_Buffer[i] = i;
  }
  
  DTS_Context.ButtonTransferReq = DTS_APP_TRANSFER_REQ_OFF;
  DTS_Context.NotificationTransferReq = DTS_APP_TRANSFER_REQ_OFF;
  DTS_Context.DtFlowStatus = DTS_APP_FLOW_ON;
  DTS_Context.connectionstatus = APP_BLE_IDLE;
  
  /* USER CODE END Service1_APP_Init */
  return;
}

/* USER CODE BEGIN FD */
void DTS_Button1TriggerReceived( void )
{
  if  (APPE_ButtonIsLongPressed(B1))
  {
    LOG_INFO_APP("  Long Press B1 \n");
    if (debug == 0)
    {
      debug = 1;
      LOG_INFO_APP("VERBOSE_DEBUG mode\n");
      Log_Module_Set_Verbose_Level(LOG_VERBOSE_DEBUG);
    }
    else
    {
      debug = 0;
      LOG_INFO_APP("VERBOSE_INFO mode \n");
      Log_Module_Set_Verbose_Level(LOG_VERBOSE_INFO);
    } 
  }
  else
  {
  if (DTS_Context.connectionstatus != APP_BLE_CONNECTED_SERVER)
  {

  }
  else
  {
    if(DTS_Context.ButtonTransferReq != DTS_APP_TRANSFER_REQ_OFF)
    {
      BSP_LED_Off(LED_BLUE);
      DTS_Context.ButtonTransferReq = DTS_APP_TRANSFER_REQ_OFF;
    }
    else
    {
      BSP_LED_On(LED_BLUE);
      DTS_Context.ButtonTransferReq = DTS_APP_TRANSFER_REQ_ON;
      UTIL_SEQ_SetTask(1 << CFG_TASK_DATA_TRANSFER_UPDATE_ID, CFG_SEQ_PRIO_0);
    }
  }
  }
  BleStackCB_Process();
  return;
}

void DTS_Button2TriggerReceived( void )
{
  tBleStatus status;
  if (DTS_Context.connectionstatus != APP_BLE_CONNECTED_SERVER)
  {
    status = aci_gap_clear_security_db();
    if (status != BLE_STATUS_SUCCESS)
    {
      LOG_INFO_APP("  Fail   : Clear security DB 0x%x\n", status);
    }
    else
    {
      LOG_INFO_APP("  Success: Clear security DB\n");
    }
  }
  else
  {
    UTIL_SEQ_SetTask(1U << CFG_TASK_CONN_INTERV_UPDATE_ID, CFG_SEQ_PRIO_0);
  }
  BleStackCB_Process();
  return;
}

void DTS_Button3TriggerReceived( void )
{
  if (DTS_Context.connectionstatus != APP_BLE_CONNECTED_SERVER)
  {

  }
  else
  {
    UTIL_SEQ_SetTask(1U << CFG_TASK_DATA_PHY_UPDATE_ID, CFG_SEQ_PRIO_0);
  }
  BleStackCB_Process();
  return;
}

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void DT_SERV_Tx_char_SendNotification(void) /* Property Notification */
{
  DT_SERV_APP_SendInformation_t notification_on_off = Tx_char_NOTIFICATION_OFF;
  DT_SERV_Data_t dt_serv_notification_data;

  dt_serv_notification_data.p_Payload = (uint8_t*)a_DT_SERV_UpdateCharData;
  dt_serv_notification_data.Length = 0;

  /* USER CODE BEGIN Service1Char1_NS_1*/

  /* USER CODE END Service1Char1_NS_1*/

  if (notification_on_off != Tx_char_NOTIFICATION_OFF)
  {
    DT_SERV_UpdateValue(DT_SERV_TX_CHAR, &dt_serv_notification_data);
  }

  /* USER CODE BEGIN Service1Char1_NS_Last*/

  /* USER CODE END Service1Char1_NS_Last*/

  return;
}

__USED void DT_SERV_Through_char_SendNotification(void) /* Property Notification */
{
  DT_SERV_APP_SendInformation_t notification_on_off = Through_char_NOTIFICATION_OFF;
  DT_SERV_Data_t dt_serv_notification_data;

  dt_serv_notification_data.p_Payload = (uint8_t*)a_DT_SERV_UpdateCharData;
  dt_serv_notification_data.Length = 0;

  /* USER CODE BEGIN Service1Char3_NS_1*/

  /* USER CODE END Service1Char3_NS_1*/

  if (notification_on_off != Through_char_NOTIFICATION_OFF)
  {
    DT_SERV_UpdateValue(DT_SERV_THROUGH_CHAR, &dt_serv_notification_data);
  }

  /* USER CODE BEGIN Service1Char3_NS_Last*/

  /* USER CODE END Service1Char3_NS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/
static void BLE_App_Delay_DataThroughput(void)
{
  uint32_t DataThroughput;
  DT_SERV_Data_t ThroughputToSend; 
  
  DataThroughput = (uint32_t)(DataReceived/TIMEUNIT);
  LOG_INFO_APP("  DataThroughput = %ld  bytes/s lost = %ld \n", DataThroughput, packet_lost);
  
  
  ThroughputToSend.Length = 4;
  ThroughputToSend.p_Payload = (uint8_t*)&DataThroughput;
  
  DT_SERV_UpdateValue(DT_SERV_THROUGH_CHAR, (DT_SERV_Data_t*)&ThroughputToSend);
  DataReceived = 0;
  packet_lost = 0;

  BleStackCB_Process();
}

static void DataThroughput_proc(void *arg){
  
  UTIL_SEQ_SetTask(1U << CFG_TASK_DATA_WRITE_ID, CFG_SEQ_PRIO_0);
}

static void SendData( void )
{
  tBleStatus status = BLE_STATUS_INVALID_PARAMS;
  uint8_t crc_result;

  if( (DTS_Context.ButtonTransferReq != DTS_APP_TRANSFER_REQ_OFF)
      && (DTS_Context.NotificationTransferReq != DTS_APP_TRANSFER_REQ_OFF)
      && (DTS_Context.DtFlowStatus != DTS_APP_FLOW_OFF) )
  {   
    /*Data Packet to send to remote*/
    Notification_Data_Buffer[0] += 1;
    /* compute CRC */
    crc_result = APP_BLE_ComputeCRC8((uint8_t*) Notification_Data_Buffer, (MTUSizeValue - 1));
    Notification_Data_Buffer[MTUSizeValue - 1] = crc_result;

    DTS_Context.TxData.p_Payload = Notification_Data_Buffer;
    DTS_Context.TxData.Length =  MTUSizeValue;

    status = DT_SERV_UpdateValue(DT_SERV_TX_CHAR, (DT_SERV_Data_t *) &DTS_Context.TxData);
    
    if (status == BLE_STATUS_INSUFFICIENT_RESOURCES)
    {
      DTS_Context.DtFlowStatus = DTS_APP_FLOW_OFF;
      (Notification_Data_Buffer[0])-=1;
    }
    else
    {
      UTIL_SEQ_SetTask(1 << CFG_TASK_DATA_TRANSFER_UPDATE_ID, CFG_SEQ_PRIO_0);
    }
  }

  BleStackCB_Process();
  return;
}

void Resume_Notification(void)
{
  DTS_Context.DtFlowStatus = DTS_APP_FLOW_ON;
  UTIL_SEQ_SetTask(1 << CFG_TASK_DATA_TRANSFER_UPDATE_ID, CFG_SEQ_PRIO_0);
}
/* USER CODE END FD_LOCAL_FUNCTIONS*/
