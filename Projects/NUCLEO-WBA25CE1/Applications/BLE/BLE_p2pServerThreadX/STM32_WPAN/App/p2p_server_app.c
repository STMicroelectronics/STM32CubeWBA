/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    p2p_server_app.c
  * @author  MCD Application Team
  * @brief   p2p_server_app application definition.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "p2p_server_app.h"
#include "p2p_server.h"
#include "stm32_rtos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_bsp.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
 typedef struct{
    uint8_t             Device_Led_Selection;
    uint8_t             Led1;
 }P2P_LedCharValue_t;

 typedef struct{
    uint8_t             Device_Button_Selection;
    uint8_t             ButtonStatus;
 }P2P_ButtonCharValue_t;

/* USER CODE END PTD */

typedef enum
{
  Switch_c_NOTIFICATION_OFF,
  Switch_c_NOTIFICATION_ON,
  /* USER CODE BEGIN Service1_APP_SendInformation_t */

  /* USER CODE END Service1_APP_SendInformation_t */
  P2P_SERVER_APP_SENDINFORMATION_LAST
} P2P_SERVER_APP_SendInformation_t;

typedef struct
{
  P2P_SERVER_APP_SendInformation_t     Switch_c_Notification_Status;
  /* USER CODE BEGIN Service1_APP_Context_t */
  P2P_LedCharValue_t              LedControl;
  P2P_ButtonCharValue_t           ButtonControl;

  /* USER CODE END Service1_APP_Context_t */
  uint16_t              ConnectionHandle;
} P2P_SERVER_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define SEND_NOTIF_TASK_STACK_SIZE    (1024)
#define SEND_NOTIF_TASK_PRIO          (15)
#define SEND_NOTIF_TASK_PREEM_TRES    (0)

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static P2P_SERVER_APP_Context_t P2P_SERVER_APP_Context;

uint8_t a_P2P_SERVER_UpdateCharData[247];

/* USER CODE BEGIN PV */

/* SEND_NOTIF_TASK related resources */
TX_THREAD SEND_NOTIF_Thread;
TX_SEMAPHORE SEND_NOTIF_Thread_Sem;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void P2P_SERVER_Switch_c_SendNotification(void);

/* USER CODE BEGIN PFP */
static void P2P_SERVER_APP_LED_BUTTON_context_Init(void);
static void P2P_SERVER_Switch_c_SendNotification_Entry(unsigned long thread_input);

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void P2P_SERVER_Notification(P2P_SERVER_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_Notification_1 */

  /* USER CODE END Service1_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_Notification_Service1_EvtOpcode */

    /* USER CODE END Service1_Notification_Service1_EvtOpcode */

    case P2P_SERVER_LED_C_READ_EVT:
      /* USER CODE BEGIN Service1Char1_READ_EVT */

      /* USER CODE END Service1Char1_READ_EVT */
      break;

    case P2P_SERVER_LED_C_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN Service1Char1_WRITE_NO_RESP_EVT */
      if(p_Notification->DataTransfered.p_Payload[1] == 0x01)
      {
        APP_BSP_LED_On(LED_BLUE);
        LOG_INFO_APP("-- P2P APPLICATION SERVER : LED1 ON\n");
        P2P_SERVER_APP_Context.LedControl.Led1 = 0x01; /* LED1 ON */
      }
      if(p_Notification->DataTransfered.p_Payload[1] == 0x00)
      {
        APP_BSP_LED_Off(LED_BLUE);
        LOG_INFO_APP("-- P2P APPLICATION SERVER : LED1 OFF\n");
        P2P_SERVER_APP_Context.LedControl.Led1 = 0x00; /* LED1 OFF */
      }
      /* USER CODE END Service1Char1_WRITE_NO_RESP_EVT */
      break;

    case P2P_SERVER_SWITCH_C_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char2_NOTIFY_ENABLED_EVT */
      P2P_SERVER_APP_Context.Switch_c_Notification_Status = Switch_c_NOTIFICATION_ON;
      LOG_INFO_APP("-- P2P APPLICATION SERVER : NOTIFICATION ENABLED\n");
      LOG_INFO_APP(" \n\r");
      /* USER CODE END Service1Char2_NOTIFY_ENABLED_EVT */
      break;

    case P2P_SERVER_SWITCH_C_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char2_NOTIFY_DISABLED_EVT */
      P2P_SERVER_APP_Context.Switch_c_Notification_Status = Switch_c_NOTIFICATION_OFF;
      LOG_INFO_APP("-- P2P APPLICATION SERVER : NOTIFICATION DISABLED\n");
      LOG_INFO_APP(" \n\r");
      /* USER CODE END Service1Char2_NOTIFY_DISABLED_EVT */
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

void P2P_SERVER_APP_EvtRx(P2P_SERVER_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_APP_EvtRx_1 */

  /* USER CODE END Service1_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_APP_EvtRx_Service1_EvtOpcode */

    /* USER CODE END Service1_APP_EvtRx_Service1_EvtOpcode */
    case P2P_SERVER_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_CONN_HANDLE_EVT */

      /* USER CODE END Service1_APP_CONN_HANDLE_EVT */
      break;

    case P2P_SERVER_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_DISCON_HANDLE_EVT */
      P2P_SERVER_APP_LED_BUTTON_context_Init();
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

void P2P_SERVER_APP_Init(void)
{
  UNUSED(P2P_SERVER_APP_Context);
  P2P_SERVER_Init();

  /* USER CODE BEGIN Service1_APP_Init */
  CHAR * pStack;

  if (tx_byte_allocate(pBytePool, (void **) &pStack, SEND_NOTIF_TASK_STACK_SIZE,TX_NO_WAIT) != TX_SUCCESS)
  {
    Error_Handler();
  }
  if (tx_semaphore_create(&SEND_NOTIF_Thread_Sem, "SEND_NOTIF_Thread_Sem", 0)!= TX_SUCCESS )
  {
    Error_Handler();
  }
  if (tx_thread_create(&SEND_NOTIF_Thread, "SEND_NOTIF_Thread", P2P_SERVER_Switch_c_SendNotification_Entry, 0,
                         pStack, SEND_NOTIF_TASK_STACK_SIZE,
                         SEND_NOTIF_TASK_PRIO, SEND_NOTIF_TASK_PREEM_TRES,
                         TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
  {
    Error_Handler();
  }

  /**
   * Initialize LedButton Service
   */
  P2P_SERVER_APP_Context.Switch_c_Notification_Status= Switch_c_NOTIFICATION_OFF;
  P2P_SERVER_APP_LED_BUTTON_context_Init();
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
__USED void P2P_SERVER_Switch_c_SendNotification(void) /* Property Notification */
{
  P2P_SERVER_APP_SendInformation_t notification_on_off = Switch_c_NOTIFICATION_OFF;
  P2P_SERVER_Data_t p2p_server_notification_data;

  p2p_server_notification_data.p_Payload = (uint8_t*)a_P2P_SERVER_UpdateCharData;
  p2p_server_notification_data.Length = 0;

  /* USER CODE BEGIN Service1Char2_NS_1 */

  if(P2P_SERVER_APP_Context.ButtonControl.ButtonStatus == 0x00)
  {
    P2P_SERVER_APP_Context.ButtonControl.ButtonStatus = 0x01;
  }
  else
  {
    P2P_SERVER_APP_Context.ButtonControl.ButtonStatus = 0x00;
  }
  a_P2P_SERVER_UpdateCharData[0] = 0x01; /* Device Led selection */
  a_P2P_SERVER_UpdateCharData[1] = P2P_SERVER_APP_Context.ButtonControl.ButtonStatus;
  /* Update notification data length */
  p2p_server_notification_data.Length = (p2p_server_notification_data.Length) + 2;

  if(P2P_SERVER_APP_Context.Switch_c_Notification_Status == Switch_c_NOTIFICATION_ON)
  {
    LOG_INFO_APP("-- P2P APPLICATION SERVER : INFORM CLIENT BUTTON 1 PUSHED\n");
    notification_on_off = Switch_c_NOTIFICATION_ON;
  }
  else
  {
    LOG_INFO_APP("-- P2P APPLICATION SERVER : CAN'T INFORM CLIENT - NOTIFICATION DISABLED\n");
  }
  /* USER CODE END Service1Char2_NS_1 */

  if (notification_on_off != Switch_c_NOTIFICATION_OFF)
  {
    P2P_SERVER_UpdateValue(P2P_SERVER_SWITCH_C, &p2p_server_notification_data);
  }

  /* USER CODE BEGIN Service1Char2_NS_Last */

  /* USER CODE END Service1Char2_NS_Last */

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */
static void P2P_SERVER_APP_LED_BUTTON_context_Init(void)
{
  APP_BSP_LED_Off(LED_BLUE);
  P2P_SERVER_APP_Context.LedControl.Device_Led_Selection=0x01;        /* select device 01 */
  P2P_SERVER_APP_Context.LedControl.Led1=0x00;                        /* led OFF */
  P2P_SERVER_APP_Context.ButtonControl.Device_Button_Selection=0x01;  /* select device 01 */
  P2P_SERVER_APP_Context.ButtonControl.ButtonStatus=0x00;

  return;
}

static void P2P_SERVER_Switch_c_SendNotification_Entry(unsigned long thread_input)
{
  (void)(thread_input);

  while(1)
  {
    tx_semaphore_get(&SEND_NOTIF_Thread_Sem, TX_WAIT_FOREVER);
    P2P_SERVER_Switch_c_SendNotification();
    tx_thread_relinquish();
  }
}

/* USER CODE END FD_LOCAL_FUNCTIONS */
