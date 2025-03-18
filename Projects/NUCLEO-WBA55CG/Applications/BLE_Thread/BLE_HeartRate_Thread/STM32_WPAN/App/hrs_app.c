/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    hrs_app.c
  * @author  MCD Application Team
  * @brief   hrs_app application definition.
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
#include "log_module.h"
#include "app_ble.h"
#include "ll_sys_if.h"
#include "dbg_trace.h"
#include "ble.h"
#include "hrs_app.h"
#include "hrs.h"
#include "stm32_rtos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32_timer.h"
#include "host_stack_if.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef enum
{
  Hrme_NOTIFICATION_OFF,
  Hrme_NOTIFICATION_ON,
  /* USER CODE BEGIN Service1_APP_SendInformation_t */

  /* USER CODE END Service1_APP_SendInformation_t */
  HRS_APP_SENDINFORMATION_LAST
} HRS_APP_SendInformation_t;

typedef struct
{
  HRS_APP_SendInformation_t     Hrme_Notification_Status;
  /* USER CODE BEGIN Service1_APP_Context_t */
  UTIL_TIMER_Object_t           TimerMeasurement_Id;
  HRS_MeasVal_t                 MeasurementVal;
  uint8_t                       ResetEnergyExpended;
  HRS_BodySensorLocation_t      BodySensorLocationVal;

  /* USER CODE END Service1_APP_Context_t */
  uint16_t              ConnectionHandle;
} HRS_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define HRS_APP_MEASUREMENT_INTERVAL (1000)
#define HRS_APP_RR_INTERVAL_NBR      (1)     /* Number of RR interval, shall be lower than HRS_MAX_NBR_RR_INTERVAL_VALUES*/
/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static HRS_APP_Context_t HRS_APP_Context;

uint8_t a_HRS_UpdateCharData[247];

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void HRS_Hrme_SendNotification(void);

/* USER CODE BEGIN PFP */
static void HRS_APP_Measurements_timCB(void *arg);
static void HRS_APP_Measurements(void);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void HRS_Notification(HRS_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_Notification_1 */

  /* USER CODE END Service1_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_Notification_Service1_EvtOpcode */
    case HRS_RESET_ENERGY_EXPENDED_EVT:
      HRS_APP_Context.MeasurementVal.EnergyExpended = 0;
      HRS_APP_Context.ResetEnergyExpended = 1;
      break;
    /* USER CODE END Service1_Notification_Service1_EvtOpcode */

    case HRS_HRME_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char1_NOTIFY_ENABLED_EVT */
      UTIL_TIMER_Stop(&(HRS_APP_Context.TimerMeasurement_Id));
      UTIL_TIMER_StartWithPeriod( &(HRS_APP_Context.TimerMeasurement_Id), HRS_APP_MEASUREMENT_INTERVAL);
      /* USER CODE END Service1Char1_NOTIFY_ENABLED_EVT */
      break;

    case HRS_HRME_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char1_NOTIFY_DISABLED_EVT */
      UTIL_TIMER_Stop(&(HRS_APP_Context.TimerMeasurement_Id));
      /* USER CODE END Service1Char1_NOTIFY_DISABLED_EVT */
      break;

    case HRS_BSL_READ_EVT:
      /* USER CODE BEGIN Service1Char2_READ_EVT */

      /* USER CODE END Service1Char2_READ_EVT */
      break;

    case HRS_HRCP_WRITE_EVT:
      /* USER CODE BEGIN Service1Char3_WRITE_EVT */

      /* USER CODE END Service1Char3_WRITE_EVT */
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

void HRS_APP_EvtRx(HRS_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_APP_EvtRx_1 */

  /* USER CODE END Service1_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_APP_EvtRx_Service1_EvtOpcode */

    /* USER CODE END Service1_APP_EvtRx_Service1_EvtOpcode */
    case HRS_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_CONN_HANDLE_EVT */

      /* USER CODE END Service1_APP_CONN_HANDLE_EVT */
      break;

    case HRS_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_DISCON_HANDLE_EVT */
      UTIL_TIMER_Stop(&(HRS_APP_Context.TimerMeasurement_Id));
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

void HRS_APP_Init(void)
{
  UNUSED(HRS_APP_Context);
  HRS_Init();

  /* USER CODE BEGIN Service1_APP_Init */
  HRS_Data_t msg_conf;

  /* Create timer for Heart Rate Measurement */
  UTIL_TIMER_Create(&(HRS_APP_Context.TimerMeasurement_Id),
                    HRS_APP_MEASUREMENT_INTERVAL,
                    UTIL_TIMER_PERIODIC,
                    &HRS_APP_Measurements_timCB, 0);

  UTIL_SEQ_RegTask(1<<CFG_TASK_MEAS_REQ_ID, UTIL_SEQ_RFU, HRS_APP_Measurements);

  /**
   * Set Flags for measurement value
   */
  HRS_APP_Context.MeasurementVal.Flags = (HRS_HRM_VALUE_FORMAT_UINT16 |
                                          HRS_HRM_SENSOR_CONTACTS_PRESENT |
                                          HRS_HRM_SENSOR_CONTACTS_SUPPORTED |
                                          HRS_HRM_ENERGY_EXPENDED_PRESENT |
                                          HRS_HRM_RR_INTERVAL_PRESENT);

  if(HRS_APP_Context.MeasurementVal.Flags & HRS_HRM_ENERGY_EXPENDED_PRESENT)
  {
    HRS_APP_Context.ResetEnergyExpended = 0;
    HRS_APP_Context.MeasurementVal.EnergyExpended = 10;
  }

  if(HRS_APP_Context.MeasurementVal.Flags & HRS_HRM_RR_INTERVAL_PRESENT)
  {
    uint8_t i;

    HRS_APP_Context.MeasurementVal.NbreOfValidRRIntervalValues = HRS_APP_RR_INTERVAL_NBR;
    for(i = 0; i < HRS_MAX_NBR_RR_INTERVAL_VALUES; i++)
    {
      HRS_APP_Context.MeasurementVal.aRRIntervalValues[i] = 1024;
    }
  }

  msg_conf.Length = 1;
  a_HRS_UpdateCharData[0] = HRS_BODY_SENSOR_LOCATION_HAND;
  msg_conf.p_Payload = a_HRS_UpdateCharData;
  HRS_UpdateValue(HRS_BSL, &msg_conf);

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
__USED void HRS_Hrme_SendNotification(void) /* Property Notification */
{
  HRS_APP_SendInformation_t notification_on_off = Hrme_NOTIFICATION_OFF;
  HRS_Data_t hrs_notification_data;

  hrs_notification_data.p_Payload = (uint8_t*)a_HRS_UpdateCharData;
  hrs_notification_data.Length = 0;

  /* USER CODE BEGIN Service1Char1_NS_1 */

  /* USER CODE END Service1Char1_NS_1 */

  if (notification_on_off != Hrme_NOTIFICATION_OFF)
  {
    HRS_UpdateValue(HRS_HRME, &hrs_notification_data);
  }

  /* USER CODE BEGIN Service1Char1_NS_Last */

  /* USER CODE END Service1Char1_NS_Last */

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */
static void HRS_APP_Measurements_timCB(void *arg)
{
  /**
   * The code shall be executed in the background as aci command may be sent
   * The background is the only place where the application can make sure a new aci command
   * is not sent if there is a pending one
   */
  UTIL_SEQ_SetTask(1<<CFG_TASK_MEAS_REQ_ID, CFG_SEQ_PRIO_0);

  return;
}

static void HRS_APP_Measurements(void)
{
  uint32_t measurement;
  tBleStatus ret;
  HRS_Data_t msg_conf;
  uint8_t hrm_char_length;

  HW_RNG_Get(1, &measurement);
  measurement = (measurement % 15) + 60;

  HRS_APP_Context.MeasurementVal.MeasurementValue = measurement;

  if((HRS_APP_Context.MeasurementVal.Flags & HRS_HRM_ENERGY_EXPENDED_PRESENT) &&
     (HRS_APP_Context.ResetEnergyExpended == 0))
  {
    HRS_APP_Context.MeasurementVal.EnergyExpended += 5;
  }
  else if(HRS_APP_Context.ResetEnergyExpended == 1)
  {
    HRS_APP_Context.ResetEnergyExpended = 0;
  }

  LOG_INFO_APP("Heart Rate value = %d bpm \n", HRS_APP_Context.MeasurementVal.MeasurementValue);
  LOG_INFO_APP("Energy expended = %d kJ \n", HRS_APP_Context.MeasurementVal.EnergyExpended);

  /**
   * Flags update
   */
  a_HRS_UpdateCharData[0] = HRS_APP_Context.MeasurementVal.Flags;
  hrm_char_length = 1;

  /**
   *  Heart Rate Measurement Value
   */
  if ( (HRS_APP_Context.MeasurementVal.Flags) &  HRS_HRM_VALUE_FORMAT_UINT16 )
  {
    a_HRS_UpdateCharData[hrm_char_length] = (uint8_t)(HRS_APP_Context.MeasurementVal.MeasurementValue & 0xFF);
    hrm_char_length++;
    a_HRS_UpdateCharData[hrm_char_length] = (uint8_t)(HRS_APP_Context.MeasurementVal.MeasurementValue >> 8);
    hrm_char_length++;
  }
  else
  {
    a_HRS_UpdateCharData[hrm_char_length] = (uint8_t)HRS_APP_Context.MeasurementVal.MeasurementValue;
    hrm_char_length++;
  }

  /**
   *  Energy Expended
   */
  if ((HRS_APP_Context.MeasurementVal.Flags) &  HRS_HRM_ENERGY_EXPENDED_PRESENT)
  {
    a_HRS_UpdateCharData[hrm_char_length] = (uint8_t)(HRS_APP_Context.MeasurementVal.EnergyExpended & 0xFF);
    hrm_char_length++;
    a_HRS_UpdateCharData[hrm_char_length] = (uint8_t)(HRS_APP_Context.MeasurementVal.EnergyExpended >> 8);
    hrm_char_length++;
  }

  /**
   *  RR Interval Values
   */
  if ((HRS_APP_Context.MeasurementVal.Flags) &  HRS_HRM_RR_INTERVAL_PRESENT)
  {
    uint8_t index;
    uint8_t rr_interval_number;

    if((HRS_APP_Context.MeasurementVal.Flags) &  HRS_HRM_VALUE_FORMAT_UINT16)
    {
      if ((HRS_APP_Context.MeasurementVal.Flags) &  HRS_HRM_ENERGY_EXPENDED_PRESENT)
      {
#if (BLE_CFG_HRS_ENERGY_RR_INTERVAL_FLAG > (HRS_MAX_NBR_RR_INTERVAL_VALUES-2))
        /**
         * When the HRM is on 16bits and the Energy expended info is present
         * The maximum RR values is 7
         */
        if(HRS_APP_Context.MeasurementVal.NbreOfValidRRIntervalValues > (HRS_MAX_NBR_RR_INTERVAL_VALUES-2))
        {
          rr_interval_number = (HRS_MAX_NBR_RR_INTERVAL_VALUES-2);
        }
        else
#endif
        {
          rr_interval_number = HRS_APP_Context.MeasurementVal.NbreOfValidRRIntervalValues;
        }
      }
      else
      {
#if (BLE_CFG_HRS_ENERGY_RR_INTERVAL_FLAG > (HRS_MAX_NBR_RR_INTERVAL_VALUES-1))
        /**
         * When the HRM is on 16bits and the Energy expended info is not present
         * The maximum RR values is 8
         */
        if(HRS_APP_Context.MeasurementVal.NbreOfValidRRIntervalValues > (HRS_MAX_NBR_RR_INTERVAL_VALUES-1))
        {
          rr_interval_number = (HRS_MAX_NBR_RR_INTERVAL_VALUES-1);
        }
        else
#endif
        {
          rr_interval_number = HRS_APP_Context.MeasurementVal.NbreOfValidRRIntervalValues;
        }
      }
    }
    else
    {
      if ((HRS_APP_Context.MeasurementVal.Flags) &  HRS_HRM_ENERGY_EXPENDED_PRESENT)
      {
#if (BLE_CFG_HRS_ENERGY_RR_INTERVAL_FLAG > (HRS_MAX_NBR_RR_INTERVAL_VALUES-1))
        /**
         * When the HRM is on 8bits and the Energy expended info is present
         * The maximum RR values is 8
         */
        if(HRS_APP_Context.MeasurementVal.NbreOfValidRRIntervalValues > (HRS_MAX_NBR_RR_INTERVAL_VALUES-1))
        {
          rr_interval_number = (HRS_MAX_NBR_RR_INTERVAL_VALUES-1);
        }
        else
#endif
        {
          rr_interval_number = HRS_APP_Context.MeasurementVal.NbreOfValidRRIntervalValues;
        }
      }
      else
      {
        rr_interval_number = HRS_APP_Context.MeasurementVal.NbreOfValidRRIntervalValues;
      }
    }

    for ( index = 0 ; index < rr_interval_number ; index++ )
    {
      a_HRS_UpdateCharData[hrm_char_length] = (uint8_t)(HRS_APP_Context.MeasurementVal.aRRIntervalValues[index] & 0xFF);
      hrm_char_length++;
      a_HRS_UpdateCharData[hrm_char_length] = (uint8_t)(HRS_APP_Context.MeasurementVal.aRRIntervalValues[index] >> 8);
      hrm_char_length++;
    }
  }

  msg_conf.p_Payload = a_HRS_UpdateCharData;
  msg_conf.Length = hrm_char_length;
  ret = HRS_UpdateValue(HRS_HRME, &msg_conf);
  if(ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("HRS_UpdateValue fails\n");
  }

  BleStackCB_Process();

  return;
}

/* USER CODE END FD_LOCAL_FUNCTIONS */
