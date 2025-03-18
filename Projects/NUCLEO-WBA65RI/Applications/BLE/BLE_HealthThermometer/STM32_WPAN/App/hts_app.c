/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    HTS_app.c
  * @author  MCD Application Team
  * @brief   HTS_app application definition.
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
#include "hts_app.h"
#include "hts.h"
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
  Temm_INDICATION_OFF,
  Temm_INDICATION_ON,
  Int_NOTIFICATION_OFF,
  Int_NOTIFICATION_ON,
  Mei_INDICATION_OFF,
  Mei_INDICATION_ON,
  /* USER CODE BEGIN Service1_APP_SendInformation_t */

  /* USER CODE END Service1_APP_SendInformation_t */
  HTS_APP_SENDINFORMATION_LAST
} HTS_APP_SendInformation_t;

typedef struct
{
  HTS_APP_SendInformation_t     Temm_Indication_Status;
  HTS_APP_SendInformation_t     Int_Notification_Status;
  HTS_APP_SendInformation_t     Mei_Indication_Status;
  /* USER CODE BEGIN Service1_APP_Context_t */
  HTS_TemperatureValue_t        IntermediateTemperatureChar;
  HTS_TemperatureValue_t        TemperatureMeasurementChar;
  uint16_t                      MeasurementIntervalChar;
  UTIL_TIMER_Object_t           TimerIntTemp_Id;
  UTIL_TIMER_Object_t           TimerMeasurement_Id;
  uint8_t                       TimerMeasurementStarted;
  UTIL_TIMER_Object_t           TimerMeasInt_Id;
  uint32_t                      StartTick;
  /* USER CODE END Service1_APP_Context_t */
  uint16_t              ConnectionHandle;
} HTS_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEFAULT_HTS_MEASUREMENT_INTERVAL       (10000)  /**< 10s */
#define DEFAULT_HTS_INT_MEASUREMENT_INTERVAL   (1000)  /**< 1s */
#define DEFAULT_TEMPERATURE_TYPE           TT_Armpit
#define NB_SAVED_MEASURES                  10
/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static HTS_APP_Context_t HTS_APP_Context;

uint8_t a_HTS_UpdateCharData[247];

/* USER CODE BEGIN PV */
static HTS_TemperatureValue_t HTS_Measurement[NB_SAVED_MEASURES];
static int8_t HTS_CurrentIndex, HTS_OldIndex;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void HTS_Temm_SendIndication(void);
static void HTS_Int_SendNotification(void);
static void HTS_Mei_SendIndication(void);

/* USER CODE BEGIN PFP */
static void HTS_APP_UpdateMeasurementInterval_timCb(void *arg);
static void HTS_APP_UpdateIntermediateTemperature_timCb(void *arg);
static void HTS_APP_Measurements_timCb(void *arg);

static void HTS_APP_IntermediateTemperature(void);
static void HTS_APP_MeasurementInterval(void);
static void HTS_APP_Measurement(void);
static void HTS_APP_Update_TimeStamp(HTS_TimeStamp_t *p_time_stamp, uint8_t flag_measurement);
static uint32_t HTS_APP_Read_RTC_SSR_SS ( void );
static void HTS_APP_Suppress(void);
static void HTS_APP_Store(void);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void HTS_Notification(HTS_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_Notification_1 */

  /* USER CODE END Service1_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_Notification_Service1_EvtOpcode */

    /* USER CODE END Service1_Notification_Service1_EvtOpcode */

    case HTS_TEMM_INDICATE_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char1_INDICATE_ENABLED_EVT */
      HTS_APP_Context.Temm_Indication_Status = Temm_INDICATION_ON;
      if(HTS_APP_Context.TimerMeasurementStarted == 0)
      {
        HTS_APP_Measurement();
        UTIL_TIMER_Stop(&HTS_APP_Context.TimerMeasurement_Id);
        UTIL_TIMER_StartWithPeriod(&HTS_APP_Context.TimerMeasurement_Id, DEFAULT_HTS_MEASUREMENT_INTERVAL);
        HTS_APP_Context.TimerMeasurementStarted = 1;
      }
      /* USER CODE END Service1Char1_INDICATE_ENABLED_EVT */
      break;

    case HTS_TEMM_INDICATE_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char1_INDICATE_DISABLED_EVT */
      HTS_APP_Context.Temm_Indication_Status = Temm_INDICATION_OFF;
      UTIL_TIMER_Stop(&HTS_APP_Context.TimerMeasurement_Id);
      HTS_APP_Context.TimerMeasurementStarted = 0;
      /* USER CODE END Service1Char1_INDICATE_DISABLED_EVT */
      break;

    case HTS_MNBS_READ_EVT:
      /* USER CODE BEGIN Service1Char2_READ_EVT */

      /* USER CODE END Service1Char2_READ_EVT */
      break;

    case HTS_INT_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char3_NOTIFY_ENABLED_EVT */
      LOG_INFO_APP("HTS_INT_NOTIFY_ENABLED_EVT\n");
      HTS_APP_Context.Int_Notification_Status = Int_NOTIFICATION_ON;
      UTIL_TIMER_Stop(&HTS_APP_Context.TimerIntTemp_Id);
      UTIL_TIMER_Start(&HTS_APP_Context.TimerIntTemp_Id);
      /* USER CODE END Service1Char3_NOTIFY_ENABLED_EVT */
      break;

    case HTS_INT_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char3_NOTIFY_DISABLED_EVT */
      LOG_INFO_APP("HTS_INT_NOTIFY_DISABLED_EVT\n");
      HTS_APP_Context.Int_Notification_Status = Int_NOTIFICATION_OFF;
      UTIL_TIMER_Stop(&HTS_APP_Context.TimerIntTemp_Id);
      /* USER CODE END Service1Char3_NOTIFY_DISABLED_EVT */
      break;

    case HTS_MEI_READ_EVT:
      /* USER CODE BEGIN Service1Char4_READ_EVT */
      {
        LOG_INFO_APP("HTS_MEI_READ_EVT\n");
        if(p_Notification->RangeInterval != 0)
        {
          HTS_APP_Context.MeasurementIntervalChar = p_Notification->RangeInterval;
          LOG_INFO_APP("HTS_MEI_READ_EVT: %d \n", 
                       p_Notification->RangeInterval);
          if(HTS_APP_Context.Temm_Indication_Status != Temm_INDICATION_OFF)
          {
            if(HTS_APP_Context.TimerMeasurementStarted == 0)
            {
              UTIL_TIMER_StartWithPeriod(&HTS_APP_Context.TimerMeasurement_Id, 
                                         (HTS_APP_Context.MeasurementIntervalChar)*1000);
              HTS_APP_Context.TimerMeasurementStarted = 1;
            }
            else
            {
              UTIL_TIMER_Stop(&HTS_APP_Context.TimerMeasurement_Id);
              UTIL_TIMER_StartWithPeriod(&HTS_APP_Context.TimerMeasurement_Id, 
                                         (HTS_APP_Context.MeasurementIntervalChar)*1000);
            }
          }
        }
        else
        {
          UTIL_TIMER_Stop(&HTS_APP_Context.TimerMeasurement_Id);
          HTS_APP_Context.TimerMeasurementStarted = 0;
        }
      }
      /* USER CODE END Service1Char4_READ_EVT */
      break;

    case HTS_MEI_WRITE_EVT:
      /* USER CODE BEGIN Service1Char4_WRITE_EVT */

      /* USER CODE END Service1Char4_WRITE_EVT */
      break;

    case HTS_MEI_INDICATE_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char4_INDICATE_ENABLED_EVT */
      LOG_INFO_APP("HTS_MEI_INDICATE_ENABLED_EVT\n");
      HTS_APP_Context.Mei_Indication_Status = Mei_INDICATION_ON;
      UTIL_TIMER_Stop(&HTS_APP_Context.TimerMeasInt_Id);
      UTIL_TIMER_StartWithPeriod(&HTS_APP_Context.TimerMeasInt_Id, DEFAULT_HTS_MEASUREMENT_INTERVAL*3);
      /* USER CODE END Service1Char4_INDICATE_ENABLED_EVT */
      break;

    case HTS_MEI_INDICATE_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char4_INDICATE_DISABLED_EVT */
      LOG_INFO_APP("HTS_MEI_INDICATE_DISABLED_EVT\n");
      HTS_APP_Context.Mei_Indication_Status = Mei_INDICATION_OFF;
      UTIL_TIMER_Stop(&HTS_APP_Context.TimerMeasInt_Id);
      /* USER CODE END Service1Char4_INDICATE_DISABLED_EVT */
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

void HTS_APP_EvtRx(HTS_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_APP_EvtRx_1 */

  /* USER CODE END Service1_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_APP_EvtRx_Service1_EvtOpcode */

    /* USER CODE END Service1_APP_EvtRx_Service1_EvtOpcode */
    case HTS_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_CONN_HANDLE_EVT */

      /* USER CODE END Service1_APP_CONN_HANDLE_EVT */
      break;

    case HTS_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_DISCON_HANDLE_EVT */
      UTIL_TIMER_Stop(&(HTS_APP_Context.TimerMeasurement_Id));
      UTIL_TIMER_StartWithPeriod(&HTS_APP_Context.TimerMeasurement_Id, DEFAULT_HTS_MEASUREMENT_INTERVAL);
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

void HTS_APP_Init(void)
{
  UNUSED(HTS_APP_Context);
  HTS_Init();

  /* USER CODE BEGIN Service1_APP_Init */
  HTS_Data_t msg_conf;

  HTS_APP_Context.StartTick = HAL_GetTick();
  
  UTIL_SEQ_RegTask( 1<< CFG_TASK_HTS_MEAS_REQ_ID, UTIL_SEQ_RFU, HTS_APP_Measurement);
  UTIL_SEQ_RegTask( 1<< CFG_TASK_HTS_INTERMEDIATE_TEMPERATURE_REQ_ID, UTIL_SEQ_RFU, HTS_APP_IntermediateTemperature);
  UTIL_SEQ_RegTask( 1<< CFG_TASK_HTS_MEAS_INTERVAL_REQ_ID, UTIL_SEQ_RFU, HTS_APP_MeasurementInterval);

  /**
   * Initialize Flags
   */
  HTS_APP_Context.TemperatureMeasurementChar.Flags = (uint8_t)NO_FLAGS;
  HTS_APP_Context.IntermediateTemperatureChar.Flags = (uint8_t)NO_FLAGS;

  HTS_APP_Context.TemperatureMeasurementChar.Flags |= (uint8_t)SENSOR_TIME_STAMP_PRESENT;
  HTS_APP_Context.IntermediateTemperatureChar.Flags |= (uint8_t)SENSOR_TIME_STAMP_PRESENT;
  HTS_APP_Context.TemperatureMeasurementChar.TimeStamp.Year = 2017;
  HTS_APP_Context.TemperatureMeasurementChar.TimeStamp.Month = 4;
  HTS_APP_Context.TemperatureMeasurementChar.TimeStamp.Day = 1;
  HTS_APP_Context.TemperatureMeasurementChar.TimeStamp.Hours = 0;
  HTS_APP_Context.TemperatureMeasurementChar.TimeStamp.Minutes = 0;
  HTS_APP_Context.TemperatureMeasurementChar.TimeStamp.Seconds  = 0;
  HTS_APP_Context.IntermediateTemperatureChar.TimeStamp.Year = 2017;
  HTS_APP_Context.IntermediateTemperatureChar.TimeStamp.Month = 4;
  HTS_APP_Context.IntermediateTemperatureChar.TimeStamp.Day = 1;
  HTS_APP_Context.IntermediateTemperatureChar.TimeStamp.Hours = 0;
  HTS_APP_Context.IntermediateTemperatureChar.TimeStamp.Minutes = 0;
  HTS_APP_Context.IntermediateTemperatureChar.TimeStamp.Seconds  = 0;
  /**
   * Initialize Measurement Interval
   */
  msg_conf.Length = 1;
  if(BLE_CFG_HTS_TEMPERATURE_INTERVAL_MIN_VALUE == 0)
    a_HTS_UpdateCharData[0] = BLE_CFG_HTS_TEMPERATURE_INTERVAL_MIN_VALUE + 10;
  else
    a_HTS_UpdateCharData[0] = BLE_CFG_HTS_TEMPERATURE_INTERVAL_MIN_VALUE + 9;

  HTS_APP_Context.Temm_Indication_Status = Temm_INDICATION_OFF;
  msg_conf.p_Payload = a_HTS_UpdateCharData;
  HTS_UpdateValue(HTS_MEI, &msg_conf);
  /**
   * Create timer for Health Temperature Measurement Interval
   */
  UTIL_TIMER_Create(&(HTS_APP_Context.TimerMeasInt_Id), DEFAULT_HTS_INT_MEASUREMENT_INTERVAL, UTIL_TIMER_PERIODIC, &HTS_APP_UpdateMeasurementInterval_timCb, 0);
  /**
   * Set Temperature Measurement Type
   */
  HTS_APP_Context.TemperatureMeasurementChar.TemperatureType = DEFAULT_TEMPERATURE_TYPE;
  HTS_APP_Context.IntermediateTemperatureChar.TemperatureType = DEFAULT_TEMPERATURE_TYPE;
  a_HTS_UpdateCharData[0] = HTS_APP_Context.TemperatureMeasurementChar.TemperatureType;
  msg_conf.p_Payload = a_HTS_UpdateCharData;
  HTS_UpdateValue(HTS_MNBS, &msg_conf);
  
  HTS_APP_Context.TemperatureMeasurementChar.Flags |= (uint8_t)SENSOR_TEMPERATURE_TYPE_PRESENT;
  HTS_APP_Context.IntermediateTemperatureChar.Flags |= (uint8_t)SENSOR_TEMPERATURE_TYPE_PRESENT;
  HTS_APP_Context.Int_Notification_Status = Int_NOTIFICATION_OFF;

  /**
   * Create timer for Health Temperature Measurement
   */
  UTIL_TIMER_Create(&(HTS_APP_Context.TimerMeasurement_Id), DEFAULT_HTS_MEASUREMENT_INTERVAL, UTIL_TIMER_PERIODIC, &HTS_APP_Measurements_timCb, 0);
  HTS_APP_Context.Temm_Indication_Status = Temm_INDICATION_OFF;

  /**
   * Create timer for Health Temperature Intermediate Temperature
   */
  UTIL_TIMER_Create(&(HTS_APP_Context.TimerIntTemp_Id), DEFAULT_HTS_INT_MEASUREMENT_INTERVAL, UTIL_TIMER_PERIODIC, HTS_APP_UpdateIntermediateTemperature_timCb, 0);

  HTS_CurrentIndex = -1;
  HTS_OldIndex = 0;

  UTIL_TIMER_StartWithPeriod(&HTS_APP_Context.TimerMeasurement_Id, DEFAULT_HTS_MEASUREMENT_INTERVAL);
  /* USER CODE END Service1_APP_Init */
  return;
}

/* USER CODE BEGIN FD */
static void HTS_APP_UpdateMeasurementInterval_timCb(void *arg)
{
  /**
   * The code shall be executed in the background as aci command may be sent
   * The background is the only place where the application can make sure a new aci command
   * is not sent if there is a pending one
   */
  UTIL_SEQ_SetTask( 1<<CFG_TASK_HTS_MEAS_INTERVAL_REQ_ID,CFG_SEQ_PRIO_0);
  return;
}

static void HTS_APP_Measurements_timCb(void *arg)
{
  /**
   * The code shall be executed in the background as aci command may be sent
   * The background is the only place where the application can make sure a new aci command
   * is not sent if there is a pending one
   */
  UTIL_SEQ_SetTask(1<<CFG_TASK_HTS_MEAS_REQ_ID, CFG_SEQ_PRIO_0);

  return;
}

static void HTS_APP_UpdateIntermediateTemperature_timCb(void *arg)
{
  /**
   * The code shall be executed in the background as aci command may be sent
   * The background is the only place where the application can make sure a new aci command
   * is not sent if there is a pending one
   */
  UTIL_SEQ_SetTask( 1<<CFG_TASK_HTS_INTERMEDIATE_TEMPERATURE_REQ_ID, CFG_SEQ_PRIO_0);
  
  return;
}
/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void HTS_Temm_SendIndication(void) /* Property Indication */
{
  HTS_APP_SendInformation_t indication_on_off = Temm_INDICATION_OFF;
  HTS_Data_t hts_indication_data;

  hts_indication_data.p_Payload = (uint8_t*)a_HTS_UpdateCharData;
  hts_indication_data.Length = 0;

  /* USER CODE BEGIN Service1Char1_IS_1 */

  /* USER CODE END Service1Char1_IS_1 */

  if (indication_on_off != Temm_INDICATION_OFF)
  {
    HTS_UpdateValue(HTS_TEMM, &hts_indication_data);
  }

  /* USER CODE BEGIN Service1Char1_IS_Last */

  /* USER CODE END Service1Char1_IS_Last */

  return;
}

__USED void HTS_Int_SendNotification(void) /* Property Notification */
{
  HTS_APP_SendInformation_t notification_on_off = Int_NOTIFICATION_OFF;
  HTS_Data_t hts_notification_data;

  hts_notification_data.p_Payload = (uint8_t*)a_HTS_UpdateCharData;
  hts_notification_data.Length = 0;

  /* USER CODE BEGIN Service1Char3_NS_1 */

  /* USER CODE END Service1Char3_NS_1 */

  if (notification_on_off != Int_NOTIFICATION_OFF)
  {
    HTS_UpdateValue(HTS_INT, &hts_notification_data);
  }

  /* USER CODE BEGIN Service1Char3_NS_Last */

  /* USER CODE END Service1Char3_NS_Last */

  return;
}

__USED void HTS_Mei_SendIndication(void) /* Property Indication */
{
  HTS_APP_SendInformation_t indication_on_off = Mei_INDICATION_OFF;
  HTS_Data_t hts_indication_data;

  hts_indication_data.p_Payload = (uint8_t*)a_HTS_UpdateCharData;
  hts_indication_data.Length = 0;

  /* USER CODE BEGIN Service1Char4_IS_1 */

  /* USER CODE END Service1Char4_IS_1 */

  if (indication_on_off != Mei_INDICATION_OFF)
  {
    HTS_UpdateValue(HTS_MEI, &hts_indication_data);
  }

  /* USER CODE BEGIN Service1Char4_IS_Last */

  /* USER CODE END Service1Char4_IS_Last */

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */
void HTS_APP_Measurement(void)
{
  uint32_t measurement;
  tBleStatus ret;
  HTS_Data_t msg_conf;

  uint8_t atm_value[
                    1 +         /* Flags */
                    4 +         /* MeasurementValue */
                    7 +         /* TimeStamp */
                    1           /* TemperatureType */
                    ];
  uint8_t tm_char_length;

  if(HTS_APP_Context.Int_Notification_Status == Int_NOTIFICATION_ON)
  {
    measurement = HTS_APP_Context.IntermediateTemperatureChar.MeasurementValue;
  }
  else
  {
    measurement = ((HTS_APP_Read_RTC_SSR_SS()) + 37);
  }
  
  HTS_APP_Context.TemperatureMeasurementChar.MeasurementValue = measurement;
  LOG_INFO_APP("HTS_APP_Measurement: %d \n", (int)measurement);

  HTS_APP_Update_TimeStamp(&(HTS_APP_Context.TemperatureMeasurementChar.TimeStamp), 1);

  if((APP_BLE_Get_Server_Connection_Status() == APP_BLE_CONNECTED_SERVER) &&
     (HTS_APP_Context.TimerMeasurementStarted == 1))
  {
    uint32_t interval;
    
    UTIL_TIMER_Stop(&HTS_APP_Context.TimerIntTemp_Id);
    
    if(HTS_Measurement[HTS_OldIndex].MeasurementValue > 0)
    {
      LOG_INFO_APP ("Send stored measurement %d\n", HTS_OldIndex);  
      HTS_APP_Suppress();
      UTIL_TIMER_Stop(&HTS_APP_Context.TimerMeasurement_Id);
      
      if(HTS_APP_Context.MeasurementIntervalChar > 0)
        interval = (HTS_APP_Context.MeasurementIntervalChar)*1000;
      else
        interval = 1000;
      
      UTIL_TIMER_StartWithPeriod(&HTS_APP_Context.TimerMeasurement_Id, 
                                 interval);
    }
    else
    {
      UTIL_TIMER_Stop(&HTS_APP_Context.TimerMeasurement_Id);
      if(HTS_APP_Context.MeasurementIntervalChar > 0)
        interval = (HTS_APP_Context.MeasurementIntervalChar)*1000;
      else
        interval = DEFAULT_HTS_MEASUREMENT_INTERVAL;
      
      UTIL_TIMER_StartWithPeriod(&HTS_APP_Context.TimerMeasurement_Id, 
                                 interval);

      LOG_INFO_APP("HTS_APP_Measurement: %d \n", (int)measurement);
      
      /**
       * Flags update
       */
      atm_value[0] = (uint8_t)HTS_APP_Context.TemperatureMeasurementChar.Flags;

      /**
       *  Temperature Measurement Value
       */
      atm_value[1] = (uint8_t)(HTS_APP_Context.TemperatureMeasurementChar.MeasurementValue);
      atm_value[2] = (uint8_t)(HTS_APP_Context.TemperatureMeasurementChar.MeasurementValue >> 8);
      atm_value[3] = (uint8_t)(HTS_APP_Context.TemperatureMeasurementChar.MeasurementValue >> 16);
      atm_value[4] = (uint8_t)(HTS_APP_Context.TemperatureMeasurementChar.MeasurementValue >> 24);

      tm_char_length = 5;

      if (HTS_APP_Context.TemperatureMeasurementChar.Flags &  SENSOR_TIME_STAMP_PRESENT)
      {
        atm_value[tm_char_length] = (uint8_t)(HTS_APP_Context.TemperatureMeasurementChar.TimeStamp.Year);
        tm_char_length++;
        atm_value[tm_char_length] = (uint8_t)(HTS_APP_Context.TemperatureMeasurementChar.TimeStamp.Year >> 8);
        tm_char_length++;
        atm_value[tm_char_length] = (uint8_t)(HTS_APP_Context.TemperatureMeasurementChar.TimeStamp.Month);
        tm_char_length++;
        atm_value[tm_char_length] = (uint8_t)(HTS_APP_Context.TemperatureMeasurementChar.TimeStamp.Day);
        tm_char_length++;
        atm_value[tm_char_length] = (uint8_t)(HTS_APP_Context.TemperatureMeasurementChar.TimeStamp.Hours);
        tm_char_length++;
        atm_value[tm_char_length] = (uint8_t)(HTS_APP_Context.TemperatureMeasurementChar.TimeStamp.Minutes);
        tm_char_length++;
        atm_value[tm_char_length] = (uint8_t)(HTS_APP_Context.TemperatureMeasurementChar.TimeStamp.Seconds);
        tm_char_length++;
      }

      atm_value[tm_char_length] = (uint8_t)(HTS_APP_Context.TemperatureMeasurementChar.TemperatureType);
      tm_char_length++;

      /**
       * Update Temperature Measurement Value
       */
      msg_conf.p_Payload = atm_value;
      msg_conf.Length = tm_char_length;
      ret = HTS_UpdateValue(HTS_TEMM, &msg_conf);
      if(ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("HTS_UpdateValue fails\n");
      }
    }
  }
  else
  {
    UTIL_TIMER_StartWithPeriod(&HTS_APP_Context.TimerIntTemp_Id, DEFAULT_HTS_INT_MEASUREMENT_INTERVAL);
    HTS_APP_Store();
  }

  return;
}

static void HTS_APP_IntermediateTemperature(void)
{
  tBleStatus ret;
  HTS_Data_t msg_conf;
  uint8_t atm_value[
                    1 +         /* Flags */
                    4 +         /* MeasurementValue */
                    7 +         /* TimeStamp */
                    1           /* TemperatureType */
                    ];
  uint8_t tm_char_length;  

  HTS_APP_Context.IntermediateTemperatureChar.MeasurementValue = ((HTS_APP_Read_RTC_SSR_SS()) + 37);

  HTS_APP_Update_TimeStamp(&(HTS_APP_Context.IntermediateTemperatureChar.TimeStamp), 0);

  if(HTS_APP_Context.Int_Notification_Status == Int_NOTIFICATION_ON)
  {
    LOG_INFO_APP("HTS_APP_IntermediateTemperature: %d \n", HTS_APP_Context.IntermediateTemperatureChar.MeasurementValue);
    /**
     * Flags update
     */
    atm_value[0] = (uint8_t)HTS_APP_Context.IntermediateTemperatureChar.Flags;

    /**
     *  Intermediate Temperature Measurement Value
     */
    atm_value[1] = (uint8_t)(HTS_APP_Context.IntermediateTemperatureChar.MeasurementValue);
    atm_value[2] = (uint8_t)(HTS_APP_Context.IntermediateTemperatureChar.MeasurementValue >> 8);
    atm_value[3] = (uint8_t)(HTS_APP_Context.IntermediateTemperatureChar.MeasurementValue >> 16);
    atm_value[4] = (uint8_t)(HTS_APP_Context.IntermediateTemperatureChar.MeasurementValue >> 24);

    tm_char_length = 5;

    if (HTS_APP_Context.IntermediateTemperatureChar.Flags &  SENSOR_TIME_STAMP_PRESENT)
    {
      atm_value[tm_char_length] = (uint8_t)(HTS_APP_Context.IntermediateTemperatureChar.TimeStamp.Year);
      tm_char_length++;
      atm_value[tm_char_length] = (uint8_t)(HTS_APP_Context.IntermediateTemperatureChar.TimeStamp.Year >> 8);
      tm_char_length++;
      atm_value[tm_char_length] = (uint8_t)(HTS_APP_Context.IntermediateTemperatureChar.TimeStamp.Month);
      tm_char_length++;
      atm_value[tm_char_length] = (uint8_t)(HTS_APP_Context.IntermediateTemperatureChar.TimeStamp.Day);
      tm_char_length++;
      atm_value[tm_char_length] = (uint8_t)(HTS_APP_Context.IntermediateTemperatureChar.TimeStamp.Hours);
      tm_char_length++;
      atm_value[tm_char_length] = (uint8_t)(HTS_APP_Context.IntermediateTemperatureChar.TimeStamp.Minutes);
      tm_char_length++;
      atm_value[tm_char_length] = (uint8_t)(HTS_APP_Context.IntermediateTemperatureChar.TimeStamp.Seconds);
      tm_char_length++;
    }

    atm_value[tm_char_length] = (uint8_t)(HTS_APP_Context.IntermediateTemperatureChar.TemperatureType);
    tm_char_length++;

    /**
     * Update Intermediate Temperature Measurement Value
     */
    msg_conf.p_Payload = atm_value;
    msg_conf.Length = tm_char_length;

    ret = HTS_UpdateValue(HTS_INT, &msg_conf);
    if(ret != BLE_STATUS_SUCCESS)
    {
      LOG_INFO_APP("HTS_UpdateValue fails\n");
    }
  }
  return;
}

static void HTS_APP_MeasurementInterval(void)
{
  tBleStatus ret;
  HTS_Data_t msg_conf;
  uint8_t htm_char_length = 0;

  if(HTS_APP_Context.MeasurementIntervalChar < BLE_CFG_HTS_TEMPERATURE_INTERVAL_MAX_VALUE)
  {
    HTS_APP_Context.MeasurementIntervalChar += 1;
  }  
  else
  {
    HTS_APP_Context.MeasurementIntervalChar = BLE_CFG_HTS_TEMPERATURE_INTERVAL_MIN_VALUE;
  }
  LOG_INFO_APP("Measurement Interval: %d \n", HTS_APP_Context.MeasurementIntervalChar);

  a_HTS_UpdateCharData[htm_char_length] = (uint8_t)(HTS_APP_Context.MeasurementIntervalChar & 0xFF);
  htm_char_length++;
  a_HTS_UpdateCharData[htm_char_length] = (uint8_t)(HTS_APP_Context.MeasurementIntervalChar >> 8);
  htm_char_length++;

  /**
   * Update Measurement Interval Value
   */
  msg_conf.p_Payload = a_HTS_UpdateCharData;
  msg_conf.Length = htm_char_length;
  ret = HTS_UpdateValue(HTS_MEI, &msg_conf);
  if(ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("HTS_UpdateValue fails\n");
  }

  if(HTS_APP_Context.TimerMeasurementStarted == 0)
  {
    UTIL_TIMER_StartWithPeriod(&HTS_APP_Context.TimerMeasurement_Id, 
                               (HTS_APP_Context.MeasurementIntervalChar)*1000);
    HTS_APP_Context.TimerMeasurementStarted = 1;
  }
  else
  {
    UTIL_TIMER_Stop(&HTS_APP_Context.TimerMeasurement_Id);
    UTIL_TIMER_StartWithPeriod(&HTS_APP_Context.TimerMeasurement_Id, 
                               (HTS_APP_Context.MeasurementIntervalChar)*1000);
  }
  
  return;
}

static uint32_t HTS_APP_Read_RTC_SSR_SS ( void )
{
  return ((uint32_t) ((READ_REG(RTC->SSR) & 0x0C00) >> 10));
}

static void HTS_APP_Update_TimeStamp(HTS_TimeStamp_t *p_time_stamp, uint8_t flag_measurement)
{
  if(flag_measurement)
  {
    if(HTS_APP_Context.MeasurementIntervalChar > 0)
      p_time_stamp->Seconds += 
        HTS_APP_Context.MeasurementIntervalChar;
    else
      p_time_stamp->Seconds += 
        (DEFAULT_HTS_MEASUREMENT_INTERVAL / 1000);
  }
  else
  {
    p_time_stamp->Seconds += 
      /*10 * */(DEFAULT_HTS_INT_MEASUREMENT_INTERVAL / 1000);
  }    
  
  while(p_time_stamp->Seconds >= 60)
  {
    p_time_stamp->Seconds -= 60;
    p_time_stamp->Minutes += 1;
  }
  while(p_time_stamp->Minutes >= 60)
  {
    p_time_stamp->Minutes -= 60;
    p_time_stamp->Hours += 1;
  }
  while(p_time_stamp->Hours >= 24)
  {
    p_time_stamp->Hours -= 24;
    p_time_stamp->Day += 1;
  }
  while(p_time_stamp->Day >= 31)
  {
    p_time_stamp->Day -= 31;
    p_time_stamp->Month += 1;
  }
  while(p_time_stamp->Month >= 12)
  {
    p_time_stamp->Month -= 11;
    p_time_stamp->Year += 1;
  }
}

static void HTS_APP_Store(void)
{
  HTS_CurrentIndex++;
  if(HTS_CurrentIndex == NB_SAVED_MEASURES)
  {
    HTS_CurrentIndex = 0;
  }
  if((HTS_CurrentIndex == HTS_OldIndex) && (HTS_Measurement[HTS_CurrentIndex].MeasurementValue > 0))
  {
    HTS_OldIndex = HTS_CurrentIndex + 1;
    if(HTS_OldIndex == NB_SAVED_MEASURES)
    {
      HTS_OldIndex = 0;
    }
  }
  LOG_INFO_APP ("Stored measurement %d, index of first measure saved: %d\n", 
                HTS_CurrentIndex, HTS_OldIndex);  
  memcpy(&HTS_Measurement[HTS_CurrentIndex], 
         &(HTS_APP_Context.TemperatureMeasurementChar), 
         sizeof(HTS_TemperatureValue_t)); 
}

static void HTS_APP_Suppress(void)
{
  tBleStatus ret;
  HTS_Data_t msg_conf;
  uint8_t length = 0;

  a_HTS_UpdateCharData[length++] = HTS_Measurement[HTS_CurrentIndex].Flags;
  a_HTS_UpdateCharData[length++] = (HTS_Measurement[HTS_CurrentIndex].MeasurementValue) & 0xFF;
  a_HTS_UpdateCharData[length++] = ((HTS_Measurement[HTS_CurrentIndex].MeasurementValue) >> 8) & 0xFF;
  a_HTS_UpdateCharData[length++] = ((HTS_Measurement[HTS_CurrentIndex].MeasurementValue) >> 16) & 0xFF;
  a_HTS_UpdateCharData[length++] = ((HTS_Measurement[HTS_CurrentIndex].MeasurementValue) >> 24) & 0xFF;
  a_HTS_UpdateCharData[length++] = (HTS_Measurement[HTS_CurrentIndex].TimeStamp.Year) & 0xFF;
  a_HTS_UpdateCharData[length++] = ((HTS_Measurement[HTS_CurrentIndex].TimeStamp.Year) >> 8) & 0xFF;
  a_HTS_UpdateCharData[length++] = HTS_Measurement[HTS_CurrentIndex].TimeStamp.Month;
  a_HTS_UpdateCharData[length++] = HTS_Measurement[HTS_CurrentIndex].TimeStamp.Day;
  a_HTS_UpdateCharData[length++] = HTS_Measurement[HTS_CurrentIndex].TimeStamp.Hours;
  a_HTS_UpdateCharData[length++] = HTS_Measurement[HTS_CurrentIndex].TimeStamp.Minutes;
  a_HTS_UpdateCharData[length++] = HTS_Measurement[HTS_CurrentIndex].TimeStamp.Seconds;
  a_HTS_UpdateCharData[length++] = (uint8_t)(HTS_Measurement[HTS_CurrentIndex].TemperatureType);
  msg_conf.Length = length;
  msg_conf.p_Payload = a_HTS_UpdateCharData;

  ret = HTS_UpdateValue(HTS_TEMM, &msg_conf);
  if(ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("HTS_UpdateValue fails\n");
  }

  HTS_Measurement[HTS_OldIndex].MeasurementValue = 0;
  HTS_OldIndex++;
  if(HTS_OldIndex == NB_SAVED_MEASURES)
  {
    HTS_OldIndex = 0;
  }
}
/* USER CODE END FD_LOCAL_FUNCTIONS */
