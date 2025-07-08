/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service3_app.c
  * @author  MCD Application Team
  * @brief   service3_app application definition.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "bas_app.h"
#include "bas.h"
#include "stm32_rtos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32_timer.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef enum
{
  Bal_NOTIFICATION_OFF,
  Bal_NOTIFICATION_ON,
  /* USER CODE BEGIN Service3_APP_SendInformation_t */

  /* USER CODE END Service3_APP_SendInformation_t */
  BAS_APP_SENDINFORMATION_LAST
} BAS_APP_SendInformation_t;

typedef struct
{
  BAS_APP_SendInformation_t     Bal_Notification_Status;
  /* USER CODE BEGIN Service3_APP_Context_t */
  uint16_t  Level;
  UTIL_TIMER_Object_t   TimerLevel_Id;
  /* USER CODE END Service3_APP_Context_t */
  uint16_t              ConnectionHandle;
} BAS_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BAS_APP_DEFAULT_BAT_LEVEL       100  /**100% */
#define BAS_APP_DEFAULT_BAT_LEVEL_CHG   (5000)  /**< 5s */
/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static BAS_APP_Context_t BAS_APP_Context;

uint8_t a_BAS_UpdateCharData[247];

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void BAS_Bal_SendNotification(void);

/* USER CODE BEGIN PFP */
static void BAS_APP_Level(void);
static void BAS_APP_UpdateLevel_timCB(void *arg);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void BAS_Notification(BAS_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service3_Notification_1 */

  /* USER CODE END Service3_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service3_Notification_Service3_EvtOpcode */

    /* USER CODE END Service3_Notification_Service3_EvtOpcode */

    case BAS_BAL_READ_EVT:
      /* USER CODE BEGIN Service3Char1_READ_EVT */

      /* USER CODE END Service3Char1_READ_EVT */
      break;

    case BAS_BAL_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service3Char1_NOTIFY_ENABLED_EVT */
      LOG_INFO_APP("BAS_BAL_NOTIFY_ENABLED_EVT\n");
      UTIL_TIMER_Stop(&(BAS_APP_Context.TimerLevel_Id));
      UTIL_TIMER_StartWithPeriod(&(BAS_APP_Context.TimerLevel_Id), BAS_APP_DEFAULT_BAT_LEVEL_CHG);
      /* USER CODE END Service3Char1_NOTIFY_ENABLED_EVT */
      break;

    case BAS_BAL_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service3Char1_NOTIFY_DISABLED_EVT */
      LOG_INFO_APP("BAS_BAL_NOTIFY_DISABLED_EVT\n");
      UTIL_TIMER_Stop(&(BAS_APP_Context.TimerLevel_Id));
      /* USER CODE END Service3Char1_NOTIFY_DISABLED_EVT */
      break;

    default:
      /* USER CODE BEGIN Service3_Notification_default */

      /* USER CODE END Service3_Notification_default */
      break;
  }
  /* USER CODE BEGIN Service3_Notification_2 */

  /* USER CODE END Service3_Notification_2 */
  return;
}

void BAS_APP_EvtRx(BAS_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service3_APP_EvtRx_1 */

  /* USER CODE END Service3_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service3_APP_EvtRx_Service3_EvtOpcode */

    /* USER CODE END Service3_APP_EvtRx_Service3_EvtOpcode */
    case BAS_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service3_APP_CONN_HANDLE_EVT */

      /* USER CODE END Service3_APP_CONN_HANDLE_EVT */
      break;

    case BAS_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service3_APP_DISCON_HANDLE_EVT */
      UTIL_TIMER_Stop(&(BAS_APP_Context.TimerLevel_Id));
      /* USER CODE END Service3_APP_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN Service3_APP_EvtRx_default */

      /* USER CODE END Service3_APP_EvtRx_default */
      break;
  }

  /* USER CODE BEGIN Service3_APP_EvtRx_2 */

  /* USER CODE END Service3_APP_EvtRx_2 */

  return;
}

void BAS_APP_Init(void)
{
  UNUSED(BAS_APP_Context);
  BAS_Init();

  /* USER CODE BEGIN Service3_APP_Init */
  BAS_Data_t msg_conf;
  tBleStatus result = BLE_STATUS_INVALID_PARAMS;

  UTIL_SEQ_RegTask( 1<< CFG_TASK_BAS_LEVEL_REQ_ID, UTIL_SEQ_RFU, BAS_APP_Level );

  
  /**
   * Initialize Level
   */
  BAS_APP_Context.Level = BAS_APP_DEFAULT_BAT_LEVEL;
  memset((void*)a_BAS_UpdateCharData, 0, sizeof(a_BAS_UpdateCharData));
  memcpy((void*)a_BAS_UpdateCharData, (void *)&(BAS_APP_Context.Level), sizeof(BAS_APP_Context.Level));
 
  msg_conf.p_Payload = a_BAS_UpdateCharData;
  msg_conf.Length = 1;
  result = BAS_UpdateValue(BAS_BAL, &msg_conf);
  if(result != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("BAS_UpdateValue fails\n");
  }

  /**
   * Create timer for Battery Level
   */
  UTIL_TIMER_Create(&(BAS_APP_Context.TimerLevel_Id), 
                    BAS_APP_DEFAULT_BAT_LEVEL_CHG, 
                    UTIL_TIMER_PERIODIC,
                    &BAS_APP_UpdateLevel_timCB, 0);
  /* USER CODE END Service3_APP_Init */
  return;
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void BAS_Bal_SendNotification(void) /* Property Notification */
{
  BAS_APP_SendInformation_t notification_on_off = Bal_NOTIFICATION_OFF;
  BAS_Data_t bas_notification_data;

  bas_notification_data.p_Payload = (uint8_t*)a_BAS_UpdateCharData;
  bas_notification_data.Length = 0;

  /* USER CODE BEGIN Service3Char1_NS_1 */

  /* USER CODE END Service3Char1_NS_1 */

  if (notification_on_off != Bal_NOTIFICATION_OFF)
  {
    BAS_UpdateValue(BAS_BAL, &bas_notification_data);
  }

  /* USER CODE BEGIN Service3Char1_NS_Last */

  /* USER CODE END Service3Char1_NS_Last */

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */
static void BAS_APP_Level( void )
{
  BAS_Data_t msg_conf;
  tBleStatus result = BLE_STATUS_INVALID_PARAMS;

  if(BAS_APP_Context.Level > 0)
    BAS_APP_Context.Level -= 1;
  else
    BAS_APP_Context.Level = BAS_APP_DEFAULT_BAT_LEVEL;

  memset((void*)a_BAS_UpdateCharData, 0, sizeof(a_BAS_UpdateCharData));
  memcpy((void*)a_BAS_UpdateCharData, (void *)&(BAS_APP_Context.Level), sizeof(BAS_APP_Context.Level));
 
  msg_conf.p_Payload = a_BAS_UpdateCharData;
  msg_conf.Length = 1;
  result = BAS_UpdateValue(BAS_BAL, &msg_conf);
  if(result != BLE_STATUS_SUCCESS)
  {
    LOG_ERROR_APP("BAS_UpdateValue fails\n");
  }

  return;
}

static void BAS_APP_UpdateLevel_timCB(void *arg)
{
  /**
   * The code shall be executed in the background as aci command may be sent
   * The background is the only place where the application can make sure a new aci command
   * is not sent if there is a pending one
   */
  UTIL_SEQ_SetTask( 1<<CFG_TASK_BAS_LEVEL_REQ_ID, CFG_SEQ_PRIO_0);

  return;
}
/* USER CODE END FD_LOCAL_FUNCTIONS */
