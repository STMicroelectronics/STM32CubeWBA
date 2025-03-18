/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service5_app.c
  * @author  MCD Application Team
  * @brief   service5_app application definition.
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
#include "ble.h"
#include "ets_app.h"
#include "ets.h"
#include "stm32_rtos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32_timer.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
  UNKNOWN = 0,
  NETWORK_PROTOCOL,
  GPS,
  RADIO_TIME_SIGNAL,
  MANUAL,
  ATOMIC_CLOCK,
  CELLULAR_NETWORK,
  NOT_SYNCHRONIZED
} ETS_APP_TimeSourceType_t;

typedef enum
{
  NO_FLAG = 0,
  TIME_IS_TICK_COUNTER               = (1<<0),
  TIME_IS_UTC                        = (1<<1),
  RESOLUTION_1_S                     = (0<<3),
  RESOLUTION_100_MS                  = (1<<3),
  RESOLUTION_1_MS                    = (2<<3),
  RESOLUTION_100_US                  = (3<<3),
  TZ_DST_OFFSET_IS_USED              = (1<<4),
  TIMESTAMP_IS_FROM_CURRENT_TIMELINE = (1<<5)
} ETS_Flags_t;

typedef struct
{
  uint8_t Flags;
  uint64_t Timevalue;
  uint8_t TimeSyncSourceType;
  int8_t TZDSTOffset;
  uint8_t ClockStatus;
  uint8_t ClockCapabilities;
} ETS_APP_ElapsedTime_t;
/* USER CODE END PTD */

typedef enum
{
  Cet_INDICATION_OFF,
  Cet_INDICATION_ON,
  /* USER CODE BEGIN Service3_APP_SendInformation_t */

  /* USER CODE END Service3_APP_SendInformation_t */
  ETS_APP_SENDINFORMATION_LAST
} ETS_APP_SendInformation_t;

typedef struct
{
  ETS_APP_SendInformation_t     Cet_Indication_Status;
  /* USER CODE BEGIN Service3_APP_Context_t */
  ETS_APP_ElapsedTime_t Ets_ElapsedTimeChar;
  uint32_t StartTick;
  UTIL_TIMER_Object_t ElapsedTime_Id;
  uint16_t time_factor;
  /* USER CODE END Service3_APP_Context_t */
  uint16_t              ConnectionHandle;
} ETS_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ETS_APP_INTERVAL                                                     (1)
#define ETS_APP_CLOCK_NEEDS_TO_BE_SET                                        (1)
/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static ETS_APP_Context_t ETS_APP_Context;

uint8_t a_ETS_UpdateCharData[247];

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void ETS_Cet_SendIndication(void);

/* USER CODE BEGIN PFP */
static void etsapp_timer_handler_alapsed_time_process(void *arg);
static void etsapp_task_elapsed_time(void);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void ETS_Notification(ETS_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service3_Notification_1 */

  /* USER CODE END Service3_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service3_Notification_Service3_EvtOpcode */

    /* USER CODE END Service3_Notification_Service3_EvtOpcode */

    case ETS_CET_READ_EVT:
      /* USER CODE BEGIN Service3Char1_READ_EVT */

      /* USER CODE END Service3Char1_READ_EVT */
      break;

    case ETS_CET_INDICATE_ENABLED_EVT:
      /* USER CODE BEGIN Service3Char1_INDICATE_ENABLED_EVT */

      /* USER CODE END Service3Char1_INDICATE_ENABLED_EVT */
      break;

    case ETS_CET_INDICATE_DISABLED_EVT:
      /* USER CODE BEGIN Service3Char1_INDICATE_DISABLED_EVT */

      /* USER CODE END Service3Char1_INDICATE_DISABLED_EVT */
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

void ETS_APP_EvtRx(ETS_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service3_APP_EvtRx_1 */

  /* USER CODE END Service3_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service3_APP_EvtRx_Service3_EvtOpcode */

    /* USER CODE END Service3_APP_EvtRx_Service3_EvtOpcode */
    case ETS_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service3_APP_CONN_HANDLE_EVT */
      /* Start Current Time Update */
      UTIL_TIMER_StartWithPeriod(&(ETS_APP_Context.ElapsedTime_Id), 
                                 ETS_APP_INTERVAL * ETS_APP_Context.time_factor);
      /* USER CODE END Service3_APP_CONN_HANDLE_EVT */
      break;

    case ETS_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service3_APP_DISCON_HANDLE_EVT */
      UTIL_TIMER_Stop(&(ETS_APP_Context.ElapsedTime_Id));
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

void ETS_APP_Init(void)
{
  UNUSED(ETS_APP_Context);
  ETS_Init();

  /* USER CODE BEGIN Service3_APP_Init */
  ETS_Data_t msg_conf;
  uint8_t length = 0;
  
  ETS_APP_Context.Ets_ElapsedTimeChar.Flags = TIMESTAMP_IS_FROM_CURRENT_TIMELINE |
                                              TZ_DST_OFFSET_IS_USED;
  ETS_APP_Context.Ets_ElapsedTimeChar.TimeSyncSourceType = CELLULAR_NETWORK;
  ETS_APP_Context.Ets_ElapsedTimeChar.Timevalue = 0x2DF18600; /* 2024-06-06 8:00:00 */
  ETS_APP_Context.Ets_ElapsedTimeChar.TZDSTOffset = 0;
  ETS_APP_Context.Ets_ElapsedTimeChar.ClockStatus = ETS_APP_CLOCK_NEEDS_TO_BE_SET;
  ETS_APP_Context.Ets_ElapsedTimeChar.ClockCapabilities = 0;

  ETS_APP_Context.StartTick = HAL_GetTick();
  
  ETS_APP_Context.Ets_ElapsedTimeChar.Timevalue += (HAL_GetTick() - ETS_APP_Context.StartTick) / 1000;
  
  a_ETS_UpdateCharData[length++] =   ETS_APP_Context.Ets_ElapsedTimeChar.Flags;
  a_ETS_UpdateCharData[length++] =  (ETS_APP_Context.Ets_ElapsedTimeChar.Timevalue) & 0xFF;
  a_ETS_UpdateCharData[length++] = ((ETS_APP_Context.Ets_ElapsedTimeChar.Timevalue) >> 8) & 0xFF;
  a_ETS_UpdateCharData[length++] = ((ETS_APP_Context.Ets_ElapsedTimeChar.Timevalue) >> 16) & 0xFF;
  a_ETS_UpdateCharData[length++] = ((ETS_APP_Context.Ets_ElapsedTimeChar.Timevalue) >> 24) & 0xFF;
  a_ETS_UpdateCharData[length++] = ((ETS_APP_Context.Ets_ElapsedTimeChar.Timevalue) >> 32) & 0xFF;
  a_ETS_UpdateCharData[length++] = ((ETS_APP_Context.Ets_ElapsedTimeChar.Timevalue) >> 40) & 0xFF;
  a_ETS_UpdateCharData[length++] =   ETS_APP_Context.Ets_ElapsedTimeChar.TimeSyncSourceType;
  a_ETS_UpdateCharData[length++] =   ETS_APP_Context.Ets_ElapsedTimeChar.TZDSTOffset;
  a_ETS_UpdateCharData[length++] =   ETS_APP_Context.Ets_ElapsedTimeChar.ClockStatus;
  a_ETS_UpdateCharData[length++] =   ETS_APP_Context.Ets_ElapsedTimeChar.ClockCapabilities;
  
  msg_conf.Length = length;
  msg_conf.p_Payload = a_ETS_UpdateCharData;
  ETS_UpdateValue(ETS_CET, &msg_conf);
  
  /* Create timer for Elapsed Time */
  if(((ETS_APP_Context.Ets_ElapsedTimeChar.Flags) & RESOLUTION_1_S) == RESOLUTION_1_S)
  {
    ETS_APP_Context.time_factor = 1000;
  }
  else if(((ETS_APP_Context.Ets_ElapsedTimeChar.Flags) & RESOLUTION_100_MS) == RESOLUTION_100_MS)
  {
    ETS_APP_Context.time_factor = 100;
  }
  UTIL_TIMER_Create(&(ETS_APP_Context.ElapsedTime_Id),
                    ETS_APP_INTERVAL * ETS_APP_Context.time_factor,
                    UTIL_TIMER_PERIODIC,
                    &etsapp_timer_handler_alapsed_time_process, 
                    0);
  /* Register task for ETS Elapsed Time */
  UTIL_SEQ_RegTask( 1<<CFG_TASK_ETS_ID, UTIL_SEQ_RFU, etsapp_task_elapsed_time );
  
  /* Start Current Time Update */
  UTIL_TIMER_StartWithPeriod(&(ETS_APP_Context.ElapsedTime_Id), 
                             ETS_APP_INTERVAL * ETS_APP_Context.time_factor);

  /* USER CODE END Service3_APP_Init */
  return;
}

/* USER CODE BEGIN FD */
void ETS_APP_GetElapsedTime(uint8_t * p_elapsedTime)
{
  uint8_t length = 0;
  
  p_elapsedTime[length++] =   ETS_APP_Context.Ets_ElapsedTimeChar.Flags;
  p_elapsedTime[length++] =  (ETS_APP_Context.Ets_ElapsedTimeChar.Timevalue) & 0xFF;
  p_elapsedTime[length++] = ((ETS_APP_Context.Ets_ElapsedTimeChar.Timevalue) >> 8) & 0xFF;
  p_elapsedTime[length++] = ((ETS_APP_Context.Ets_ElapsedTimeChar.Timevalue) >> 16) & 0xFF;
  p_elapsedTime[length++] = ((ETS_APP_Context.Ets_ElapsedTimeChar.Timevalue) >> 24) & 0xFF;
  p_elapsedTime[length++] = ((ETS_APP_Context.Ets_ElapsedTimeChar.Timevalue) >> 32) & 0xFF;
  p_elapsedTime[length++] = ((ETS_APP_Context.Ets_ElapsedTimeChar.Timevalue) >> 40) & 0xFF;
  p_elapsedTime[length++] =   ETS_APP_Context.Ets_ElapsedTimeChar.TimeSyncSourceType;
  p_elapsedTime[length++] =   ETS_APP_Context.Ets_ElapsedTimeChar.TZDSTOffset;
}

void ETS_APP_SetElapsedTime(uint8_t * p_elapsedTime)
{
  /* Update Flags and/or TimeSyncSourceType and/or TZDSTOffset */
  ETS_APP_Context.Ets_ElapsedTimeChar.Flags = p_elapsedTime[0];
  ETS_APP_Context.Ets_ElapsedTimeChar.TimeSyncSourceType = p_elapsedTime[7];
  ETS_APP_Context.Ets_ElapsedTimeChar.TZDSTOffset = p_elapsedTime[8];
}
/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void ETS_Cet_SendIndication(void) /* Property Indication */
{
  ETS_APP_SendInformation_t indication_on_off = Cet_INDICATION_OFF;
  ETS_Data_t ets_indication_data;

  ets_indication_data.p_Payload = (uint8_t*)a_ETS_UpdateCharData;
  ets_indication_data.Length = 0;

  /* USER CODE BEGIN Service3Char1_IS_1 */

  /* USER CODE END Service3Char1_IS_1 */

  if (indication_on_off != Cet_INDICATION_OFF)
  {
    ETS_UpdateValue(ETS_CET, &ets_indication_data);
  }

  /* USER CODE BEGIN Service3Char1_IS_Last */

  /* USER CODE END Service3Char1_IS_Last */

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */
static void etsapp_timer_handler_alapsed_time_process(void *arg)
{
  /**
  * The code shall be executed in the background as aci command may be sent
  * The background is the only place where the application can make sure a new aci command
  * is not sent if there is a pending one
  */
  UTIL_SEQ_SetTask( 1<<CFG_TASK_ETS_ID, CFG_SEQ_PRIO_0);
  
  return;
}

static void etsapp_task_elapsed_time(void)
{
  ETS_Data_t msg_conf;
  uint8_t length = 0;

  ETS_APP_Context.Ets_ElapsedTimeChar.Timevalue += (HAL_GetTick() - ETS_APP_Context.StartTick) / 1000;
  
  a_ETS_UpdateCharData[length++] =   ETS_APP_Context.Ets_ElapsedTimeChar.Flags;
  a_ETS_UpdateCharData[length++] =  (ETS_APP_Context.Ets_ElapsedTimeChar.Timevalue) & 0xFF;
  a_ETS_UpdateCharData[length++] = ((ETS_APP_Context.Ets_ElapsedTimeChar.Timevalue) >> 8) & 0xFF;
  a_ETS_UpdateCharData[length++] = ((ETS_APP_Context.Ets_ElapsedTimeChar.Timevalue) >> 16) & 0xFF;
  a_ETS_UpdateCharData[length++] = ((ETS_APP_Context.Ets_ElapsedTimeChar.Timevalue) >> 24) & 0xFF;
  a_ETS_UpdateCharData[length++] = ((ETS_APP_Context.Ets_ElapsedTimeChar.Timevalue) >> 32) & 0xFF;
  a_ETS_UpdateCharData[length++] = ((ETS_APP_Context.Ets_ElapsedTimeChar.Timevalue) >> 40) & 0xFF;
  a_ETS_UpdateCharData[length++] =   ETS_APP_Context.Ets_ElapsedTimeChar.TimeSyncSourceType;
  a_ETS_UpdateCharData[length++] =   ETS_APP_Context.Ets_ElapsedTimeChar.TZDSTOffset;
  a_ETS_UpdateCharData[length++] =   ETS_APP_Context.Ets_ElapsedTimeChar.ClockStatus;
  a_ETS_UpdateCharData[length++] =   ETS_APP_Context.Ets_ElapsedTimeChar.ClockCapabilities;
  
  msg_conf.Length = length;
  msg_conf.p_Payload = a_ETS_UpdateCharData;
  ETS_UpdateValue(ETS_CET, &msg_conf);
  
  return;
}

/* USER CODE END FD_LOCAL_FUNCTIONS */
