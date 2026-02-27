/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    SerialPort_server_app.c
  * @author  MCD Application Team
  * @brief   SerialPort_server_app application definition.
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
#include "serialport_server_app.h"
#include "serialport_server.h"
#include "stm32_rtos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "serial_cmd_interpreter.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef enum
{
  Tx_NOTIFICATION_OFF,
  Tx_NOTIFICATION_ON,
  /* USER CODE BEGIN Service1_APP_SendInformation_t */

  /* USER CODE END Service1_APP_SendInformation_t */
  SERIALPORT_SERVER_APP_SENDINFORMATION_LAST
} SERIALPORT_SERVER_APP_SendInformation_t;

typedef struct
{
  SERIALPORT_SERVER_APP_SendInformation_t     Tx_Notification_Status;
  /* USER CODE BEGIN Service1_APP_Context_t */

  /* USER CODE END Service1_APP_Context_t */
  uint16_t              ConnectionHandle;
} SERIALPORT_SERVER_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define BUFFUARTRX_SIZE         20
#define BUFFSPTX_SIZE           BUFFUARTRX_SIZE
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static SERIALPORT_SERVER_APP_Context_t SERIALPORT_SERVER_APP_Context;

uint8_t a_SERIALPORT_SERVER_UpdateCharData[247];

/* USER CODE BEGIN PV */
static uint8_t a_buffUartRx[BUFFUARTRX_SIZE];
static volatile uint8_t buffUartRxIndex;
static uint8_t a_buffUartRxValid[BUFFUARTRX_SIZE];
static volatile uint8_t buffUartRxValidIndex;
static uint8_t a_buffSpTx[BUFFSPTX_SIZE];
static volatile uint8_t buffSpTxLen;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void SERIALPORT_SERVER_Tx_SendNotification(void);

/* USER CODE BEGIN PFP */
void Send_Data_Over_BLE(void);
static void RxUART_ValidProcess(uint8_t data);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void SERIALPORT_SERVER_Notification(SERIALPORT_SERVER_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_Notification_1 */

  /* USER CODE END Service1_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_Notification_Service1_EvtOpcode */

    /* USER CODE END Service1_Notification_Service1_EvtOpcode */

    case SERIALPORT_SERVER_TX_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char1_NOTIFY_ENABLED_EVT */
      SERIALPORT_SERVER_APP_Context.Tx_Notification_Status = Tx_NOTIFICATION_ON;
      LOG_INFO_APP("-- SERIAL PORT APPLICATION SERVER : NOTIFICATION ENABLED\n"); 
      LOG_INFO_APP(" \n");
      /* USER CODE END Service1Char1_NOTIFY_ENABLED_EVT */
      break;

    case SERIALPORT_SERVER_TX_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char1_NOTIFY_DISABLED_EVT */
      SERIALPORT_SERVER_APP_Context.Tx_Notification_Status = Tx_NOTIFICATION_OFF;
      LOG_INFO_APP("-- SERIAL PORT APPLICATION SERVER : NOTIFICATION DISABLED\n"); 
      LOG_INFO_APP(" \n");
      /* USER CODE END Service1Char1_NOTIFY_DISABLED_EVT */
      break;

    case SERIALPORT_SERVER_RX_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN Service1Char2_WRITE_NO_RESP_EVT */

      /* USER CODE END Service1Char2_WRITE_NO_RESP_EVT */
      break;

    case SERIALPORT_SERVER_RX_WRITE_EVT:
      /* USER CODE BEGIN Service1Char2_WRITE_EVT */

      /* USER CODE END Service1Char2_WRITE_EVT */
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

void SERIALPORT_SERVER_APP_EvtRx(SERIALPORT_SERVER_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_APP_EvtRx_1 */

  /* USER CODE END Service1_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_APP_EvtRx_Service1_EvtOpcode */

    /* USER CODE END Service1_APP_EvtRx_Service1_EvtOpcode */
    case SERIALPORT_SERVER_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_CONN_HANDLE_EVT */

      /* USER CODE END Service1_APP_CONN_HANDLE_EVT */
      break;

    case SERIALPORT_SERVER_DISCON_HANDLE_EVT :
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

void SERIALPORT_SERVER_APP_Init(void)
{
  UNUSED(SERIALPORT_SERVER_APP_Context);
  SERIALPORT_SERVER_Init();

  /* USER CODE BEGIN Service1_APP_Init */
  SERIALPORT_SERVER_APP_Context.Tx_Notification_Status= Tx_NOTIFICATION_OFF;
  
  UTIL_SEQ_RegTask( 1<< CFG_TASK_SERIALPORT_TX_REQ_ID, UTIL_SEQ_RFU, Send_Data_Over_BLE);
  /* USER CODE END Service1_APP_Init */
  return;
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void SERIALPORT_SERVER_Tx_SendNotification(void) /* Property Notification */
{
  SERIALPORT_SERVER_APP_SendInformation_t notification_on_off = Tx_NOTIFICATION_OFF;
  SERIALPORT_SERVER_Data_t serialport_server_notification_data;

  serialport_server_notification_data.p_Payload = (uint8_t*)a_SERIALPORT_SERVER_UpdateCharData;
  serialport_server_notification_data.Length = 0;

  /* USER CODE BEGIN Service1Char1_NS_1 */

  /* USER CODE END Service1Char1_NS_1 */

  if (notification_on_off != Tx_NOTIFICATION_OFF)
  {
    SERIALPORT_SERVER_UpdateValue(SERIALPORT_SERVER_TX, &serialport_server_notification_data);
  }

  /* USER CODE BEGIN Service1Char1_NS_Last */

  /* USER CODE END Service1Char1_NS_Last */

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

static void RxUART_ValidProcess(uint8_t data)
{
  if(buffUartRxValidIndex < sizeof(a_buffUartRxValid))
  {
    a_buffUartRxValid[buffUartRxValidIndex++] = data;

    if(data == '\r')
    {
      a_buffUartRxValid[buffUartRxValidIndex-1] = '\0';
      Serial_CMD_Interpreter_CmdExecute( a_buffUartRxValid, buffUartRxValidIndex );
      buffUartRxValidIndex = 0;
    }
  }
  else
  {
    buffUartRxValidIndex = 0;
  }

  return;
}

void UartRxCpltCallback(uint8_t * pdata, uint16_t size)
{
 
  uint8_t byte_received;
    
  if(size == 1)
  {
    byte_received = pdata[0];

    RxUART_ValidProcess(byte_received);

    if(buffUartRxIndex < sizeof(a_buffUartRx))
    {
      a_buffUartRx[buffUartRxIndex++] = byte_received;
    }
    else
    {
      buffUartRxIndex = 0;
    }
    
    if( (byte_received == '\n') || (buffUartRxIndex >= sizeof(a_buffUartRx)))
    {
      memcpy(&a_buffSpTx[0], &a_buffUartRx[0], buffUartRxIndex);
      buffSpTxLen = buffUartRxIndex;
      
      buffUartRxIndex = 0;
      UTIL_SEQ_SetTask(1 << CFG_TASK_SERIALPORT_TX_REQ_ID, CFG_SEQ_PRIO_0);
    }    
  }
  else
  {

  }
  return; 
}

void My_Log_Module_Print(Log_Verbose_Level_t VerboseLevel, Log_Region_t Region, const char * Text, ...)
{
  va_list variadic_args;

  va_start(variadic_args, Text);
  Log_Module_PrintWithArg(VerboseLevel, Region, Text, variadic_args);
  va_end(variadic_args);
}

void Send_Data_Over_BLE(void)
{
  SERIALPORT_SERVER_Data_t TxBuffer;
  
  TxBuffer.p_Payload = a_buffSpTx;
  TxBuffer.Length = buffSpTxLen;
  SERIALPORT_SERVER_UpdateValue(SERIALPORT_SERVER_TX, &TxBuffer);

  return;
}
/* USER CODE END FD_LOCAL_FUNCTIONS */
