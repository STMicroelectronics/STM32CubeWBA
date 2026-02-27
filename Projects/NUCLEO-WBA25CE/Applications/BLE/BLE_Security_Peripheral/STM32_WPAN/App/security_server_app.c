/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Security_Server_app.c
  * @author  MCD Application Team
  * @brief   Security_Server_app application definition.
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
#include "security_server_app.h"
#include "security_server.h"
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
 }SECURITY_LedCharValue_t;

 typedef struct{
    uint8_t             Device_Button_Selection;
    uint8_t             ButtonStatus;
 }SECURITY_ButtonCharValue_t;
/* USER CODE END PTD */

typedef enum
{
  Switch_c_NOTIFICATION_OFF,
  Switch_c_NOTIFICATION_ON,
  /* USER CODE BEGIN Service1_APP_SendInformation_t */

  /* USER CODE END Service1_APP_SendInformation_t */
  SECURITY_SERVER_APP_SENDINFORMATION_LAST
} SECURITY_SERVER_APP_SendInformation_t;

typedef struct
{
  SECURITY_SERVER_APP_SendInformation_t     Switch_c_Notification_Status;
  /* USER CODE BEGIN Service1_APP_Context_t */
  //VTIMER_HandleType TimerSendNotification_Id;
  uint32_t TimerSendNotification_interval;
  SECURITY_LedCharValue_t              LedControl;
  SECURITY_ButtonCharValue_t           ButtonControl;
  /* USER CODE END Service1_APP_Context_t */
  uint16_t              ConnectionHandle;
} SECURITY_SERVER_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static SECURITY_SERVER_APP_Context_t SECURITY_SERVER_APP_Context;

uint8_t a_SECURITY_SERVER_UpdateCharData[247];

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void SECURITY_SERVER_Switch_c_SendNotification(void);

/* USER CODE BEGIN PFP */
static void SECURITY_SERVER_APP_LED_BUTTON_context_Init(void);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void SECURITY_SERVER_Notification(SECURITY_SERVER_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_Notification_1 */

  /* USER CODE END Service1_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_Notification_Service1_EvtOpcode */

    /* USER CODE END Service1_Notification_Service1_EvtOpcode */

    case SECURITY_SERVER_LED_C_READ_EVT:
      /* USER CODE BEGIN Service1Char1_READ_EVT */

      /* USER CODE END Service1Char1_READ_EVT */
      break;

    case SECURITY_SERVER_LED_C_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN Service1Char1_WRITE_NO_RESP_EVT */
      if(p_Notification->DataTransfered.p_Payload[1] == 0x01)
      {
        APP_BSP_LED_On(LED_BLUE);
        APP_DBG_MSG("-- SECURITY APPLICATION SERVER : LED1 ON\n"); 
        SECURITY_SERVER_APP_Context.LedControl.Led1 = 0x01; /* LED1 ON */
      }
      if(p_Notification->DataTransfered.p_Payload[1] == 0x00)
      {
        APP_BSP_LED_Off(LED_BLUE);
        APP_DBG_MSG("-- SECURITY APPLICATION SERVER : LED1 OFF\n"); 
        SECURITY_SERVER_APP_Context.LedControl.Led1 = 0x00; /* LED1 OFF */
      }
      /* USER CODE END Service1Char1_WRITE_NO_RESP_EVT */
      break;

    case SECURITY_SERVER_SWITCH_C_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char2_NOTIFY_ENABLED_EVT */
      SECURITY_SERVER_APP_Context.Switch_c_Notification_Status = Switch_c_NOTIFICATION_ON;
      APP_DBG_MSG("-- SECURITY APPLICATION SERVER : NOTIFICATION ENABLED\n"); 
      APP_DBG_MSG(" \n");

      /* USER CODE END Service1Char2_NOTIFY_ENABLED_EVT */
      break;

    case SECURITY_SERVER_SWITCH_C_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char2_NOTIFY_DISABLED_EVT */
      SECURITY_SERVER_APP_Context.Switch_c_Notification_Status = Switch_c_NOTIFICATION_OFF;
      APP_DBG_MSG("-- SECURITY APPLICATION SERVER : NOTIFICATION DISABLED\n"); 
      APP_DBG_MSG(" \n");
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

void SECURITY_SERVER_APP_EvtRx(SECURITY_SERVER_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_APP_EvtRx_1 */

  /* USER CODE END Service1_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_APP_EvtRx_Service1_EvtOpcode */

    /* USER CODE END Service1_APP_EvtRx_Service1_EvtOpcode */
    case SECURITY_SERVER_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_CONN_HANDLE_EVT */

      /* USER CODE END Service1_APP_CONN_HANDLE_EVT */
      break;

    case SECURITY_SERVER_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_DISCON_HANDLE_EVT */
      SECURITY_SERVER_APP_LED_BUTTON_context_Init();
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

void SECURITY_SERVER_APP_Init(void)
{
  UNUSED(SECURITY_SERVER_APP_Context);
  SECURITY_SERVER_Init();

  /* USER CODE BEGIN Service1_APP_Init */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_SEND_NOTIF_ID, UTIL_SEQ_RFU, SECURITY_SERVER_Switch_c_SendNotification);

  /**
   * Initialize LedButton Service
   */
  SECURITY_SERVER_APP_Context.Switch_c_Notification_Status= Switch_c_NOTIFICATION_OFF;
  SECURITY_SERVER_APP_LED_BUTTON_context_Init();

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
__USED void SECURITY_SERVER_Switch_c_SendNotification(void) /* Property Notification */
{
  SECURITY_SERVER_APP_SendInformation_t notification_on_off = Switch_c_NOTIFICATION_OFF;
  SECURITY_SERVER_Data_t security_server_notification_data;

  security_server_notification_data.p_Payload = (uint8_t*)a_SECURITY_SERVER_UpdateCharData;
  security_server_notification_data.Length = 0;

  /* USER CODE BEGIN Service1Char2_NS_1 */

  if(SECURITY_SERVER_APP_Context.ButtonControl.ButtonStatus == 0x00)
  {
    SECURITY_SERVER_APP_Context.ButtonControl.ButtonStatus = 0x01;
  }
  else
  {
    SECURITY_SERVER_APP_Context.ButtonControl.ButtonStatus = 0x00;
  }
  a_SECURITY_SERVER_UpdateCharData[0] = 0x01; /* Device Led selection */
  a_SECURITY_SERVER_UpdateCharData[1] = SECURITY_SERVER_APP_Context.ButtonControl.ButtonStatus;
  /* Update notification data length */
  security_server_notification_data.Length = (security_server_notification_data.Length) + 2;

  if(SECURITY_SERVER_APP_Context.Switch_c_Notification_Status == Switch_c_NOTIFICATION_ON)
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
    SECURITY_SERVER_UpdateValue(SECURITY_SERVER_SWITCH_C, &security_server_notification_data);
  }

  /* USER CODE BEGIN Service1Char2_NS_Last */

  /* USER CODE END Service1Char2_NS_Last */

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */
static void SECURITY_SERVER_APP_LED_BUTTON_context_Init(void)
{
  APP_BSP_LED_Off(LED_BLUE);
  SECURITY_SERVER_APP_Context.LedControl.Device_Led_Selection=0x01;        /* select device 01 */
  SECURITY_SERVER_APP_Context.LedControl.Led1=0x00;                        /* led OFF */
  SECURITY_SERVER_APP_Context.ButtonControl.Device_Button_Selection=0x01;  /* select device 01 */
  SECURITY_SERVER_APP_Context.ButtonControl.ButtonStatus=0x00;

  return;
}
/* USER CODE END FD_LOCAL_FUNCTIONS */
