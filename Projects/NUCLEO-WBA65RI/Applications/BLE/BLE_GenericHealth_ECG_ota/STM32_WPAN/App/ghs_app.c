/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service1_app.c
  * @author  MCD Application Team
  * @brief   service1_app application definition.
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
#include "ghs_app.h"
#include "ghs.h"
#include "stm32_rtos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ghs_cp.h"
#include "stm32_timer.h"
#include "ets_app.h"
#include "app_conf.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef enum
{
  Hsf_INDICATION_OFF,
  Hsf_INDICATION_ON,
  Lho_NOTIFICATION_OFF,
  Lho_NOTIFICATION_ON,
  Lho_INDICATION_OFF,
  Lho_INDICATION_ON,
  Ghscp_INDICATION_OFF,
  Ghscp_INDICATION_ON,
  Osc_INDICATION_OFF,
  Osc_INDICATION_ON,
  /* USER CODE BEGIN Service1_APP_SendInformation_t */

  /* USER CODE END Service1_APP_SendInformation_t */
  GHS_APP_SENDINFORMATION_LAST
} GHS_APP_SendInformation_t;

typedef struct
{
  GHS_APP_SendInformation_t     Hsf_Indication_Status;
  GHS_APP_SendInformation_t     Lho_Notification_Status;
  GHS_APP_SendInformation_t     Lho_Indication_Status;
  GHS_APP_SendInformation_t     Ghscp_Indication_Status;
  GHS_APP_SendInformation_t     Osc_Indication_Status;
  /* USER CODE BEGIN Service1_APP_Context_t */
  GHS_HSF_t HSFChar;
  GHS_LHO_t LHOChar;
  GHS_OSC_t OSCChar;
  UTIL_TIMER_Object_t TimerLHO_Id;
  /* USER CODE END Service1_APP_Context_t */
  uint16_t              ConnectionHandle;
} GHS_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define GHS_APP_LHO_INTERVAL                                         (1.5*1000)

#define GHS_APP_LHO_CHAR_SIZE                                        (80)

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static GHS_APP_Context_t GHS_APP_Context;

uint8_t a_GHS_UpdateCharData[247];

/* USER CODE BEGIN PV */

static uint32_t MeasurementPeriod[2] =
{
  0xFF00000A,
  0xFF00000A
};

static uint32_t UpdateInterval[2] =
{
  0xFF00000A,
  0xFF00000A
};

const GHS_OSC_t ObservationSchedule[2] = 
{
  {0x00024BB8, GHS_APP_LHO_INTERVAL, GHS_APP_LHO_INTERVAL},
  {0x0002481A, GHS_APP_LHO_INTERVAL, GHS_APP_LHO_INTERVAL}
};

GHS_ValidRangeAccuracy_t ValidRangeAccuracy[2] =
{
  { 0x01, 0x0220, 0, 100, 10 },
  { 0x01, 0x0AA0, 0, 100, 1 }
};

static uint32_t observationType_ECG[1] =
{
  0x00020101
};

static uint8_t SimpleArrayECGWaveform1[] =
{
  0x04,                                 /* Observation Class Type: Sample array observation */
  0x38, 0x01,                           /* Length: 312 bytes */
  0x03, 0x00,                           /* Flags:  OBSERVATION_TYPE_PRESENT | TIME_STAMP_PRESENT */
  0x01, 0x01, 0x02, 0x00,               /* Observation Type: MDC_ECG_ELEC_POTL_I */               
  0x22,                                 /* Time Stamp */
  0x72, 0x9D, 0x2B, 0x29, 0x00, 0x00,   
  0x06, 0x00,                                                  
  0xB2, 0x10,                           /* Observation Value: Unit Code MDC_DIM_MILLI_VOLT */
  0x4C, 0x06, 0x00, 0x00,               /*                    Scale Factor 1.612 */
  0x00, 0x08, 0x00, 0x00,               /*                    Offset 2048 */
  0x0A, 0x00, 0x00, 0xFD,               /*                    Sample Period 0.010 (10 msec) */
  0x01,                                 /*                    Number of Samples Per Period 1 */
  0x02,                                 /*                    Bytes Per Sample 2 */
  0x8C,                                 /*                    Number Of Samples 140 */
                                        /*                    Scaled Samples: */
                                        
  0xD7, 0x03, 0xD1, 0x03, 0xB8, 0x03, 0xB2, 0x03, 0xE2, 0x03, 0xF7, 0x03, 0x01, 0x04, 0xEB, 0x03, 
  0xED, 0x03, 0xCB, 0x03, 0xB6, 0x03, 0xB2, 0x03, 0x9F, 0x03, 0xA3, 0x03, 0xB5, 0x03, 0x95, 0x03, 
  0xBD, 0x03, 0xF9, 0x04, 0xA2, 0x06, 0x81, 0x05, 0xA8, 0x02, 0x39, 0x02, 0x00, 0x02, 0xE6, 0x01, 
  0xFF, 0x01, 0x63, 0x02, 0x08, 0x03, 0x95, 0x03, 0xAC, 0x03, 0x8C, 0x03, 0xB6, 0x03, 0xE3, 0x03, 
  0xDC, 0x03, 0xD4, 0x03, 0x0C, 0x04, 0x0A, 0x04, 0x01, 0x04, 0x2D, 0x04, 0x46, 0x04, 0x5B, 0x04, 
  0x8F, 0x04, 0xD5, 0x04, 0x00, 0x05, 0x2A, 0x05, 0x34, 0x05, 0x30, 0x05, 0x1B, 0x05, 0xCF, 0x04, 
  0x93, 0x04, 0x6B, 0x04, 0x34, 0x04, 0x09, 0x04, 0xFE, 0x03, 0xE4, 0x03, 0xC7, 0x03, 0xC0, 0x03, 
  0xC6, 0x03, 0xAD, 0x03, 0xA6, 0x03, 0xA9, 0x03, 0xBB, 0x03, 0xC0, 0x03, 0xC2, 0x03, 0xB1, 0x03, 
  0xC3, 0x03, 0xC3, 0x03, 0xB4, 0x03, 0xB4, 0x03, 0xC8, 0x03, 0xAC, 0x03, 0xB4, 0x03, 0xCB, 0x03, 
  0xC1, 0x03, 0xCF, 0x03, 0xC7, 0x03, 0xC4, 0x03, 0xCE, 0x03, 0xE3, 0x03, 0x0C, 0x04, 0x18, 0x04, 
  0xFC, 0x03, 0xED, 0x03, 0xE1, 0x03, 0xD8, 0x03, 0xB4, 0x03, 0xA6, 0x03, 0xAA, 0x03, 0xA5, 0x03, 
  0xB9, 0x03, 0x48, 0x04, 0xBF, 0x05, 0xBD, 0x06, 0xC6, 0x03, 0x86, 0x02, 0x19, 0x02, 0x01, 0x02, 
  0x0A, 0x02, 0x43, 0x02, 0xC4, 0x02, 0x4E, 0x03, 0x8B, 0x03, 0xA7, 0x03, 0xD0, 0x03, 0xC8, 0x03, 
  0xCA, 0x03, 0xDE, 0x03, 0xFA, 0x03, 0xF0, 0x03, 0xF2, 0x03, 0x10, 0x04, 0x36, 0x04, 0x53, 0x04, 
  0x78, 0x04, 0xA7, 0x04, 0xD0, 0x04, 0xEB, 0x04, 0x26, 0x05, 0x21, 0x05, 0xF6, 0x04, 0xD5, 0x04, 
  0xA1, 0x04, 0x5B, 0x04, 0x14, 0x04, 0xF5, 0x03, 0xE0, 0x03, 0xCF, 0x03, 0xAC, 0x03, 0xAD, 0x03, 
  0x93, 0x03, 0x8B, 0x03, 0x8E, 0x03, 0x78, 0x03, 0x97, 0x03, 0xA5, 0x03, 0xA4, 0x03, 0x90, 0x03, 
  0x89, 0x03, 0xA0, 0x03, 0x95, 0x03, 0x94, 0x03
};

static uint8_t SimpleArrayECGAnomalyWaveform[] =
{
  0x04,                                 /* Observation Class Type: Sample array observation */
  0x08, 0x01,                           /* Length: 264 bytes */
  0x03, 0x00,                           /* Flags:  OBSERVATION_TYPE_PRESENT | TIME_STAMP_PRESENT */
  0x01, 0x01, 0x02, 0x00,               /* Observation Type: MDC_ECG_ELEC_POTL_I */               
  0x22,                                 /* Time Stamp */
  0x72, 0x9D, 0x2B, 0x29, 0x00, 0x00,   
  0x06, 0x00,                                                  
  0xB2, 0x10,                           /* Observation Value: Unit Code MDC_DIM_MILLI_VOLT */
  0x4C, 0x06, 0x00, 0x00,               /*                    Scale Factor 1.612 */
  0x00, 0x08, 0x00, 0x00,               /*                    Offset 2048 */
  0x0A, 0x00, 0x00, 0xFD,               /*                    Sample Period 0.010 (10 msec) */
  0x01,                                 /*                    Number of Samples Per Period 1 */
  0x02,                                 /*                    Bytes Per Sample 2 */
  0x74,                                 /*                    Number Of Samples 116 */
                                        /*                    Scaled Samples: */                        
  0x17, 0x04, 0xE5, 0x03, 0xA3, 0x03, 0x82, 0x03, 0x64, 0x03, 0x5C, 0x03, 0x52, 
  0x03, 0x51, 0x03, 0x48, 0x03, 0x40, 0x03, 0x18, 0x03, 0x1E, 0x03, 0x49, 0x03, 
  0x73, 0x03, 0x62, 0x03, 0x52, 0x03, 0x8B, 0x03, 0x4F, 0x03, 0x4C, 0x03, 0x5A, 
  0x03, 0x5C, 0x03, 0x71, 0x03, 0x8C, 0x03, 0xAC, 0x03, 0xD7, 0x03, 0xC6, 0x03, 
  0xB0, 0x03, 0xCD, 0x03, 0xCF, 0x03, 0x8B, 0x03, 0x4F, 0x03, 0x4C, 0x03, 0x5A, 
  0x03, 0x5C, 0x03, 0x71, 0x03, 0x8C, 0x03, 0xAC, 0x03, 0xD7, 0x03, 0xC6, 0x03, 
  0xB0, 0x03, 0xCD, 0x03, 0xCF, 0x03, 0x49, 0x04, 0x16, 0x06, 0xC8, 0x04, 0xFD, 
  0x02, 0xBC, 0x02, 0xD0, 0x02, 0x17, 0x03, 0xBC, 0x03, 0x6A, 0x04, 0x8E, 0x04, 
  0xB9, 0x04, 0xC5, 0x04, 0xF8, 0x04, 0xEF, 0x04, 0xFD, 0x04, 0x4E, 0x05, 0x8F, 
  0x05, 0xAA, 0x05, 0xE1, 0x05, 0xEC, 0x05, 0x00, 0x06, 0xD9, 0x05, 0x9D, 0x05, 
  0x4A, 0x05, 0x0F, 0x05, 0xE7, 0x04, 0x93, 0x04, 0x9E, 0x04, 0x95, 0x04, 0x53, 
  0x04, 0x4A, 0x04, 0x4D, 0x04, 0x38, 0x04, 0x11, 0x04, 0x1E, 0x04, 0x10, 0x04, 
  0xFB, 0x03, 0xE9, 0x03, 0xC0, 0x03, 0xBB, 0x03, 0xB5, 0x03, 0xBC, 0x03, 0xD5, 
  0x03, 0xDB, 0x03, 0xBC, 0x03, 0x09, 0x04, 0xD7, 0x03, 0xB2, 0x03, 0x8D, 0x03, 
  0x62, 0x03, 0x54, 0x03, 0x50, 0x03, 0x78, 0x03, 0xAB, 0x04, 0x55, 0x06, 0x34, 
  0x02, 0xB4, 0x01, 0xA9, 0x01, 0xC2, 0x01, 0x69, 0x02, 0x0A, 0x03, 0x2E, 0x03, 
  0x53, 0x03, 0x5D, 0x03, 0x54, 0x03, 0x90, 0x03, 0x7D, 0x03, 0xB5, 0x03, 0xDB, 
  0x03, 0x0C, 0x04, 0x40, 0x04, 0x8F, 0x04, 0xB5, 0x04, 0xAC, 0x04
};

uint8_t a_GHS_data[1+sizeof(SimpleArrayECGWaveform1)];
uint8_t ArrayECGWaveform[sizeof(SimpleArrayECGWaveform1)];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void GHS_Hsf_SendIndication(void);
static void GHS_Lho_SendNotification(void);
static void GHS_Lho_SendIndication(void);
static void GHS_Ghscp_SendIndication(void);
static void GHS_Osc_SendIndication(void);

/* USER CODE BEGIN PFP */
static void ghsapp_timer_handler_lho_process(void *arg);
static void ghsapp_task_lho(void);
static void ghsapp_start_session(void);
static void ghsapp_stop_session(void);
static tBleStatus ghsapp_send_segment(uint8_t segment_size,
                                      uint16_t total_size,
                                      uint8_t * p_Data,
                                      GHS_CharOpcode_t CharOpcode);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void GHS_Notification(GHS_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_Notification_1 */

  /* USER CODE END Service1_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_Notification_Service1_EvtOpcode */

    /* USER CODE END Service1_Notification_Service1_EvtOpcode */

    case GHS_HSF_READ_EVT:
      /* USER CODE BEGIN Service1Char1_READ_EVT */
      LOG_INFO_APP("HSF READ\r\n");
      /* USER CODE END Service1Char1_READ_EVT */
      break;

    case GHS_HSF_INDICATE_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char1_INDICATE_ENABLED_EVT */
      LOG_INFO_APP("HSF Indication Enabled\r\n");
      GHS_APP_Context.Hsf_Indication_Status = Hsf_INDICATION_ON;
      if(aci_gatt_store_db() != BLE_STATUS_SUCCESS)
      {
        /* Save the descriptor value in GATT database */
        LOG_INFO_APP("aci_gatt_store_db failed\r\n");
      }
      /* USER CODE END Service1Char1_INDICATE_ENABLED_EVT */
      break;

    case GHS_HSF_INDICATE_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char1_INDICATE_DISABLED_EVT */
      LOG_INFO_APP("HSF Indication Disabled\r\n");
      GHS_APP_Context.Hsf_Indication_Status = Hsf_INDICATION_OFF;
      if(aci_gatt_store_db() != BLE_STATUS_SUCCESS)
      {
        /* Save the descriptor value in GATT database */
        LOG_INFO_APP("aci_gatt_store_db failed\r\n");
      }
      /* USER CODE END Service1Char1_INDICATE_DISABLED_EVT */
      break;

    case GHS_LHO_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char2_NOTIFY_ENABLED_EVT */

      /* USER CODE END Service1Char2_NOTIFY_ENABLED_EVT */
      break;

    case GHS_LHO_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char2_NOTIFY_DISABLED_EVT */

      /* USER CODE END Service1Char2_NOTIFY_DISABLED_EVT */
      break;

    case GHS_LHO_INDICATE_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char2_INDICATE_ENABLED_EVT */
      LOG_INFO_APP("LHO Indication Enabled\r\n");
      GHS_APP_Context.Lho_Indication_Status = Lho_INDICATION_ON;
      if(aci_gatt_store_db() != BLE_STATUS_SUCCESS)
      {
        /* Save the descriptor value in GATT database */
        LOG_INFO_APP("aci_gatt_store_db failed\r\n");
      }
      /* USER CODE END Service1Char2_INDICATE_ENABLED_EVT */
      break;

    case GHS_LHO_INDICATE_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char2_INDICATE_DISABLED_EVT */
      LOG_INFO_APP("LHO Indication Disabled\r\n");
      GHS_APP_Context.Lho_Indication_Status = Lho_INDICATION_OFF;
      if(aci_gatt_store_db() != BLE_STATUS_SUCCESS)
      {
        /* Save the descriptor value in GATT database */
        LOG_INFO_APP("aci_gatt_store_db failed\r\n");
      }
      /* USER CODE END Service1Char2_INDICATE_DISABLED_EVT */
      break;

    case GHS_GHSCP_WRITE_EVT:
      /* USER CODE BEGIN Service1Char3_WRITE_EVT */

      /* USER CODE END Service1Char3_WRITE_EVT */
      break;

    case GHS_GHSCP_INDICATE_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char3_INDICATE_ENABLED_EVT */
      LOG_INFO_APP("GHSCP Indication Enabled\r\n");
      GHS_APP_Context.Ghscp_Indication_Status = Ghscp_INDICATION_ON;
      if(aci_gatt_store_db() != BLE_STATUS_SUCCESS)
      {
        /* Save the descriptor value in GATT database */
        LOG_INFO_APP("aci_gatt_store_db failed\r\n");
      }

      /* USER CODE END Service1Char3_INDICATE_ENABLED_EVT */
      break;

    case GHS_GHSCP_INDICATE_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char3_INDICATE_DISABLED_EVT */
      LOG_INFO_APP("GHSCP Indication Disabled\r\n");
      GHS_APP_Context.Ghscp_Indication_Status = Ghscp_INDICATION_OFF;
      if(aci_gatt_store_db() != BLE_STATUS_SUCCESS)
      {
        /* Save the descriptor value in GATT database */
        LOG_INFO_APP("aci_gatt_store_db failed\r\n");
      }

      /* USER CODE END Service1Char3_INDICATE_DISABLED_EVT */
      break;

    case GHS_OSC_INDICATE_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char4_INDICATE_ENABLED_EVT */
      LOG_INFO_APP("OSC Indication Enabled\r\n");
      GHS_APP_Context.Osc_Indication_Status = Osc_INDICATION_ON;
      if(aci_gatt_store_db() != BLE_STATUS_SUCCESS)
      {
        /* Save the descriptor value in GATT database */
        LOG_INFO_APP("aci_gatt_store_db failed\r\n");
      }

      /* USER CODE END Service1Char4_INDICATE_ENABLED_EVT */
      break;

    case GHS_OSC_INDICATE_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char4_INDICATE_DISABLED_EVT */
      LOG_INFO_APP("OSC Indication Disabled\r\n");
      GHS_APP_Context.Osc_Indication_Status = Osc_INDICATION_OFF;
      if(aci_gatt_store_db() != BLE_STATUS_SUCCESS)
      {
        /* Save the descriptor value in GATT database */
        LOG_INFO_APP("aci_gatt_store_db failed\r\n");
      }

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

void GHS_APP_EvtRx(GHS_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_APP_EvtRx_1 */

  /* USER CODE END Service1_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_APP_EvtRx_Service1_EvtOpcode */

    /* USER CODE END Service1_APP_EvtRx_Service1_EvtOpcode */
    case GHS_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_CONN_HANDLE_EVT */

      /* USER CODE END Service1_APP_CONN_HANDLE_EVT */
      break;

    case GHS_DISCON_HANDLE_EVT :
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

void GHS_APP_Init(void)
{
  UNUSED(GHS_APP_Context);
  GHS_Init();

  /* USER CODE BEGIN Service1_APP_Init */
  
  /* GHS Feature */
  GHS_APP_UpdateFeature();
  
  GHS_APP_Context.LHOChar.SegmentationHeader.FirstSegment = 1;
  GHS_APP_Context.LHOChar.SegmentationHeader.LastSegment = 1;
  GHS_APP_Context.LHOChar.SegmentationHeader.SegmentCounter = 0;

  /* Register tasks for Live Health Observations */
  UTIL_SEQ_RegTask( 1<<CFG_TASK_LHO_ID, UTIL_SEQ_RFU, ghsapp_task_lho );

  /* Create timer for Live Health Observations */
  UTIL_TIMER_Create(&(GHS_APP_Context.TimerLHO_Id),
                    GHS_APP_LHO_INTERVAL,
                    UTIL_TIMER_PERIODIC,
                    &ghsapp_timer_handler_lho_process, 
                    0);
  
  /* USER CODE END Service1_APP_Init */
  return;
}

/* USER CODE BEGIN FD */

/**
  * @brief Get the flag holding whether GHS CP characteristic indication is enabled or not
  * @param None
  * @retval None
  */
uint8_t GHS_APP_GetGHSCPCharacteristicIndicationEnabled(void)
{
  return ((GHS_APP_Context.Ghscp_Indication_Status == Ghscp_INDICATION_ON) ? TRUE: FALSE);
} /* end of GHS_APP_GetGHSCPCharacteristicIndicationEnabled() */

/**
  * @brief Get the status of LHO timer is started or not
  * @param None
  * @retval None
  */
uint32_t GHS_APP_GetLHOTimerStarted(void)
{
  return (UTIL_TIMER_IsRunning(&(GHS_APP_Context.TimerLHO_Id)));
}

/**
  * @brief Update GHS Feature characteristic
  * @param None
  * @retval None
  */
void GHS_APP_UpdateFeature(void)
{
  GHS_Data_t msg_conf;
  uint8_t length = 0;
  tBleStatus ret;

  /* GHS Feature */
  GHS_APP_Context.HSFChar.Flags = NO_FEATURE_FLAG;

  GHS_APP_Context.HSFChar.SupportedObservationTypes.Count = 1;
  GHS_APP_Context.HSFChar.SupportedObservationTypes.p_Data = &observationType_ECG[0];
  
  a_GHS_UpdateCharData[length] = GHS_APP_Context.HSFChar.Flags;
  length++;
  a_GHS_UpdateCharData[length] = GHS_APP_Context.HSFChar.SupportedObservationTypes.Count;
  length++;
  for(uint8_t i = 0; i < GHS_APP_Context.HSFChar.SupportedObservationTypes.Count; i++)
  {
    a_GHS_UpdateCharData[length] = (GHS_APP_Context.HSFChar.SupportedObservationTypes.p_Data[i]) & 0xFF ;
    length++;
    a_GHS_UpdateCharData[length] = ((GHS_APP_Context.HSFChar.SupportedObservationTypes.p_Data[i]) >> 8) & 0xFF;
    length++;
    a_GHS_UpdateCharData[length] = ((GHS_APP_Context.HSFChar.SupportedObservationTypes.p_Data[i]) >> 16) & 0xFF;
    length++;
    a_GHS_UpdateCharData[length] = ((GHS_APP_Context.HSFChar.SupportedObservationTypes.p_Data[i]) >> 24) & 0xFF;
    length++;
  }
  
  msg_conf.Length = length;
  msg_conf.p_Payload = a_GHS_UpdateCharData;
  ret= GHS_UpdateValue(GHS_HSF, &msg_conf);
  if(ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("Feature update fails\n");
  }

} /* end of GHS_APP_UpdateFeature() */

/**
  * @brief Update GHS Live Health Observations characteristic
  * @param None
  * @retval None
  */
void GHS_APP_UpdateLiveHealthObservation(void)
{
  uint16_t hob_length;
  uint8_t transmit_size;
  uint8_t length = 0;

  /* Max Number of bytes in transmission */ 
  if(GHS_APP_LHO_CHAR_SIZE <= (CFG_BLE_ATT_MTU_MAX - 3))
  { /* LHO char. size is less than MAX ATT MTU size */ 
    transmit_size = GHS_APP_LHO_CHAR_SIZE;
  }
  else
  { /* MAX ATT MTU size is less than LHO char. size */ 
    transmit_size = CFG_BLE_ATT_MTU_MAX - 3;
  }

  uint8_t a_elapsedTime[9];
  
  ETS_APP_GetElapsedTime(a_elapsedTime);  

  

  if (AnomalyState == 1) {
    memcpy(ArrayECGWaveform, SimpleArrayECGAnomalyWaveform, sizeof(SimpleArrayECGAnomalyWaveform)); 
    AnomalyState = 0;
  }
  else 
  {
    memcpy(ArrayECGWaveform, SimpleArrayECGWaveform1, sizeof(SimpleArrayECGWaveform1));

    /* Get Timestamp from ETS Elapsed Time */
    memcpy(&(ArrayECGWaveform[10]), &(a_elapsedTime[1]), sizeof(a_elapsedTime) - 1);
    
    hob_length = (ArrayECGWaveform[1]) |
                 ((ArrayECGWaveform[2]) << 8);
    
    /* By default only one segment */
    GHS_APP_Context.LHOChar.SegmentationHeader.FirstSegment = 1;
    GHS_APP_Context.LHOChar.SegmentationHeader.LastSegment = 1;
    GHS_APP_Context.LHOChar.SegmentationHeader.SegmentCounter += 1;
    if(GHS_APP_Context.LHOChar.SegmentationHeader.SegmentCounter > 63)
    {  
      GHS_APP_Context.LHOChar.SegmentationHeader.SegmentCounter = 0;
    }
    GHS_APP_Context.LHOChar.HealthObservationBody.ObservationClassType = ArrayECGWaveform[0];
    GHS_APP_Context.LHOChar.HealthObservationBody.Length = hob_length;
    GHS_APP_Context.LHOChar.HealthObservationBody.p_Data = &(ArrayECGWaveform[3]);

    length = 0;
    a_GHS_data[length] = (GHS_APP_Context.LHOChar.SegmentationHeader.FirstSegment) |
                                   ((GHS_APP_Context.LHOChar.SegmentationHeader.LastSegment) << 1) |
                                   ((GHS_APP_Context.LHOChar.SegmentationHeader.SegmentCounter)) << 2;
    length++;
    a_GHS_data[length] = ArrayECGWaveform[0]; /* Class Type */
    length++;
    a_GHS_data[length] = ArrayECGWaveform[1]; /* Length */
    length++;
    a_GHS_data[length] = ArrayECGWaveform[2];
    length++;
    memcpy(&(a_GHS_data[length]), &(ArrayECGWaveform[3]), hob_length);
    
    ghsapp_send_segment(transmit_size,
                        hob_length + 4,
                        &a_GHS_data[0],
                        GHS_LHO);
  }
  
  return;
} /* end of GHS_APP_UpdateLiveHealthObservation() */

/**
  * @brief Update GHS Observation Schedule Changed characteristic
  * @param None
  * @retval None
  */
void GHS_APP_UpdateObservationScheduleChanged(void)
{
  GHS_Data_t msg_conf;
  uint8_t length;
  tBleStatus ret;

  for(uint8_t i = 0; i < GHS_APP_Context.HSFChar.SupportedObservationTypes.Count; i++)
  {
    /* GHS Observation Schedule Changed */
    GHS_APP_Context.OSCChar.ObservationType = GHS_APP_Context.HSFChar.SupportedObservationTypes.p_Data[i];
    GHS_APP_Context.OSCChar.MeasurementPeriod = MeasurementPeriod[i];
    GHS_APP_Context.OSCChar.UpdateInterval = UpdateInterval[i];
    
    length = 0;
    a_GHS_UpdateCharData[length] = (GHS_APP_Context.OSCChar.ObservationType) & 0xFF ;
    length++;
    a_GHS_UpdateCharData[length] = ((GHS_APP_Context.OSCChar.ObservationType) >> 8) & 0xFF;
    length++;
    a_GHS_UpdateCharData[length] = ((GHS_APP_Context.OSCChar.ObservationType) >> 16) & 0xFF;
    length++;
    a_GHS_UpdateCharData[length] = ((GHS_APP_Context.OSCChar.ObservationType) >> 24) & 0xFF;
    length++;
    a_GHS_UpdateCharData[length] = (GHS_APP_Context.OSCChar.MeasurementPeriod) & 0xFF ;
    length++;
    a_GHS_UpdateCharData[length] = ((GHS_APP_Context.OSCChar.MeasurementPeriod) >> 8) & 0xFF;
    length++;
    a_GHS_UpdateCharData[length] = ((GHS_APP_Context.OSCChar.MeasurementPeriod) >> 16) & 0xFF;
    length++;
    a_GHS_UpdateCharData[length] = ((GHS_APP_Context.OSCChar.MeasurementPeriod) >> 24) & 0xFF;
    length++;
    a_GHS_UpdateCharData[length] = (GHS_APP_Context.OSCChar.UpdateInterval) & 0xFF ;
    length++;
    a_GHS_UpdateCharData[length] = ((GHS_APP_Context.OSCChar.UpdateInterval) >> 8) & 0xFF;
    length++;
    a_GHS_UpdateCharData[length] = ((GHS_APP_Context.OSCChar.UpdateInterval) >> 16) & 0xFF;
    length++;
    a_GHS_UpdateCharData[length] = ((GHS_APP_Context.OSCChar.UpdateInterval) >> 24) & 0xFF;
    length++;
    
    msg_conf.Length = length;
    msg_conf.p_Payload = a_GHS_UpdateCharData;
    ret= GHS_UpdateValue(GHS_OSC, &msg_conf);
    if(ret != BLE_STATUS_SUCCESS)
    {
      LOG_INFO_APP("Observation Schedule Changed update fails\n");
    }
  }

} /* end of GHS_APP_UpdateObservationScheduleChanged() */

/**
  * @brief Control Point event handler
  * @param pointer on CP event
  * @retval None
  */
uint8_t GHS_CP_APP_EventHandler(GHS_CP_App_Event_t *pCPAppEvent)
{
  switch(pCPAppEvent->EventCode)
  {      
    case GHS_CP_START_SENDING_LIVE_OBSERVATIONS_EVENT:
      {
        ghsapp_start_session();
      }
      break;
      
    case GHS_CP_STOP_SENDING_LIVE_OBSERVATIONS_EVENT:
      {
        ghsapp_stop_session();
      }
      break;
      
    default:
      break;
  }
  
  return TRUE;
} /* end of GHS_CP_APP_EventHandler() */
/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void GHS_Hsf_SendIndication(void) /* Property Indication */
{
  GHS_APP_SendInformation_t indication_on_off = Hsf_INDICATION_OFF;
  GHS_Data_t ghs_indication_data;

  ghs_indication_data.p_Payload = (uint8_t*)a_GHS_UpdateCharData;
  ghs_indication_data.Length = 0;

  /* USER CODE BEGIN Service1Char1_IS_1 */

  /* USER CODE END Service1Char1_IS_1 */

  if (indication_on_off != Hsf_INDICATION_OFF)
  {
    GHS_UpdateValue(GHS_HSF, &ghs_indication_data);
  }

  /* USER CODE BEGIN Service1Char1_IS_Last */

  /* USER CODE END Service1Char1_IS_Last */

  return;
}

__USED void GHS_Lho_SendNotification(void) /* Property Notification */
{
  GHS_APP_SendInformation_t notification_on_off = Lho_NOTIFICATION_OFF;
  GHS_Data_t ghs_notification_data;

  ghs_notification_data.p_Payload = (uint8_t*)a_GHS_UpdateCharData;
  ghs_notification_data.Length = 0;

  /* USER CODE BEGIN Service1Char2_NS_1 */

  /* USER CODE END Service1Char2_NS_1 */

  if (notification_on_off != Lho_NOTIFICATION_OFF)
  {
    GHS_UpdateValue(GHS_LHO, &ghs_notification_data);
  }

  /* USER CODE BEGIN Service1Char2_NS_Last */

  /* USER CODE END Service1Char2_NS_Last */

  return;
}

__USED void GHS_Lho_SendIndication(void) /* Property Indication */
{
  GHS_APP_SendInformation_t indication_on_off = Lho_INDICATION_OFF;
  GHS_Data_t ghs_indication_data;

  ghs_indication_data.p_Payload = (uint8_t*)a_GHS_UpdateCharData;
  ghs_indication_data.Length = 0;

  /* USER CODE BEGIN Service1Char2_IS_1 */

  /* USER CODE END Service1Char2_IS_1 */

  if (indication_on_off != Lho_INDICATION_OFF)
  {
    GHS_UpdateValue(GHS_LHO, &ghs_indication_data);
  }

  /* USER CODE BEGIN Service1Char2_IS_Last */

  /* USER CODE END Service1Char2_IS_Last */

  return;
}

__USED void GHS_Ghscp_SendIndication(void) /* Property Indication */
{
  GHS_APP_SendInformation_t indication_on_off = Ghscp_INDICATION_OFF;
  GHS_Data_t ghs_indication_data;

  ghs_indication_data.p_Payload = (uint8_t*)a_GHS_UpdateCharData;
  ghs_indication_data.Length = 0;

  /* USER CODE BEGIN Service1Char3_IS_1 */

  /* USER CODE END Service1Char3_IS_1 */

  if (indication_on_off != Ghscp_INDICATION_OFF)
  {
    GHS_UpdateValue(GHS_GHSCP, &ghs_indication_data);
  }

  /* USER CODE BEGIN Service1Char3_IS_Last */

  /* USER CODE END Service1Char3_IS_Last */

  return;
}

__USED void GHS_Osc_SendIndication(void) /* Property Indication */
{
  GHS_APP_SendInformation_t indication_on_off = Osc_INDICATION_OFF;
  GHS_Data_t ghs_indication_data;

  ghs_indication_data.p_Payload = (uint8_t*)a_GHS_UpdateCharData;
  ghs_indication_data.Length = 0;

  /* USER CODE BEGIN Service1Char4_IS_1 */

  /* USER CODE END Service1Char4_IS_1 */

  if (indication_on_off != Osc_INDICATION_OFF)
  {
    GHS_UpdateValue(GHS_OSC, &ghs_indication_data);
  }

  /* USER CODE BEGIN Service1Char4_IS_Last */

  /* USER CODE END Service1Char4_IS_Last */

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */
static void ghsapp_timer_handler_lho_process(void *arg)
{
  /**
  * The code shall be executed in the background as aci command may be sent
  * The background is the only place where the application can make sure a new aci command
  * is not sent if there is a pending one
  */
  UTIL_SEQ_SetTask( 1<<CFG_TASK_LHO_ID, CFG_SEQ_PRIO_0);
  
  return;
}

static void ghsapp_task_lho(void)
{
  /* New Live Health Observations */
  GHS_APP_UpdateLiveHealthObservation();

  return;
}

static void ghsapp_start_session( void )
{    
  LOG_INFO_APP("ghsapp_start_session\r\n");
  
  UTIL_TIMER_Stop(&(GHS_APP_Context.TimerLHO_Id));
    
  UTIL_TIMER_StartWithPeriod(&(GHS_APP_Context.TimerLHO_Id), 
                             GHS_APP_LHO_INTERVAL);
}

static void ghsapp_stop_session( void )
{
  LOG_INFO_APP("ghsapp_stop_session\r\n");

  UTIL_TIMER_Stop(&(GHS_APP_Context.TimerLHO_Id));
}

static tBleStatus ghsapp_send_segment(uint8_t segment_size,
                                      uint16_t total_size,
                                      uint8_t * p_Data,
                                      GHS_CharOpcode_t CharOpcode)
{
  GHS_Data_t msg_conf;
  tBleStatus ret = BLE_STATUS_SUCCESS;
  uint8_t segment[247];

  if(total_size <= segment_size)
  {
    /* Only one segment */
    msg_conf.Length = total_size;
    msg_conf.p_Payload = p_Data;

    ret = GHS_UpdateValue(CharOpcode, &msg_conf);
    if(ret != BLE_STATUS_SUCCESS)
    {
      if(CharOpcode == GHS_LHO)
      {
        LOG_INFO_APP("Live Health Observations update fails\n");
      }
    }
  }
  else
  {
    /* Multiple segments */
    uint32_t current_index;
    
    /* Update with first segment */
    GHS_APP_Context.LHOChar.SegmentationHeader.FirstSegment = 1;
    GHS_APP_Context.LHOChar.SegmentationHeader.LastSegment = 0;
    GHS_APP_Context.LHOChar.SegmentationHeader.SegmentCounter += 1;
    if(GHS_APP_Context.LHOChar.SegmentationHeader.SegmentCounter > 63)
    {  
      GHS_APP_Context.LHOChar.SegmentationHeader.SegmentCounter = 0;
    }

    p_Data[0] =  (GHS_APP_Context.LHOChar.SegmentationHeader.FirstSegment) |
                ((GHS_APP_Context.LHOChar.SegmentationHeader.LastSegment) << 1) |
                ((GHS_APP_Context.LHOChar.SegmentationHeader.SegmentCounter) << 2);
    current_index = 0;
    memcpy(segment, &(p_Data[current_index]), segment_size);
    current_index += segment_size;
    
    msg_conf.Length = segment_size;
    msg_conf.p_Payload = segment;

    ret = GHS_UpdateValue(CharOpcode, &msg_conf);
    if(ret != BLE_STATUS_SUCCESS)
    {
      if(CharOpcode == GHS_LHO)
      {
        LOG_INFO_APP("Live Health Observations first segment update fails\n");
      }
      else
      {
        LOG_INFO_APP("Stored Health Observations first segment update fails\n");
      }
      return ret;
    }
    else
    {
      if(GHS_APP_Context.Lho_Indication_Status == Lho_INDICATION_ON)
      {  
        UTIL_SEQ_ClrEvt(1U << CFG_EVENT_GATT_INDICATION_COMPLETE);
        UTIL_SEQ_WaitEvt(1U << CFG_EVENT_GATT_INDICATION_COMPLETE);
      }
      if(CharOpcode == GHS_LHO)
      {
        LOG_INFO_APP("Live Health Observations first segment generated\n");
      }
    }
    
    total_size -= segment_size - 1;
    while(total_size > (segment_size - 1))
    {
      /* Update with intermediate segments */
      GHS_APP_Context.LHOChar.SegmentationHeader.FirstSegment = 0;
      GHS_APP_Context.LHOChar.SegmentationHeader.LastSegment = 0;
      GHS_APP_Context.LHOChar.SegmentationHeader.SegmentCounter += 1;
      if(GHS_APP_Context.LHOChar.SegmentationHeader.SegmentCounter > 63)
      {  
        GHS_APP_Context.LHOChar.SegmentationHeader.SegmentCounter = 0;
      }
      
      segment[0] =  (GHS_APP_Context.LHOChar.SegmentationHeader.FirstSegment) |
                   ((GHS_APP_Context.LHOChar.SegmentationHeader.LastSegment) << 1) |
                   ((GHS_APP_Context.LHOChar.SegmentationHeader.SegmentCounter) << 2);

      memcpy(&(segment[1]), &(p_Data[current_index]), (segment_size - 1));
      current_index += (segment_size - 1);
      
      msg_conf.Length = segment_size;
      msg_conf.p_Payload = segment;

      ret = GHS_UpdateValue(CharOpcode, &msg_conf);
      if(ret != BLE_STATUS_SUCCESS)
      {
        if(CharOpcode == GHS_LHO)
        {
          LOG_INFO_APP("Live Health Observations intermediate segment update fails\n");
        }
        return ret;
      }
      else
      {
        if(GHS_APP_Context.Lho_Indication_Status == Lho_INDICATION_ON)
        {  
          UTIL_SEQ_ClrEvt(1U << CFG_EVENT_GATT_INDICATION_COMPLETE);
          UTIL_SEQ_WaitEvt(1U << CFG_EVENT_GATT_INDICATION_COMPLETE);
        }
        if(CharOpcode == GHS_LHO)
        {
          LOG_INFO_APP("Live Health Observations intermediate segment generated\n");
        }
        else
        {
          LOG_INFO_APP("Stored Health Observations intermediate segment generated\n");
        }
        total_size -= (segment_size - 1);
      }
    } /* End while(total_size > (segment_size - 1)) */
    
    /* Update with last segment */
    GHS_APP_Context.LHOChar.SegmentationHeader.FirstSegment = 0;
    GHS_APP_Context.LHOChar.SegmentationHeader.LastSegment = 1;
    GHS_APP_Context.LHOChar.SegmentationHeader.SegmentCounter += 1;
    if(GHS_APP_Context.LHOChar.SegmentationHeader.SegmentCounter > 63)
    {  
      GHS_APP_Context.LHOChar.SegmentationHeader.SegmentCounter = 0;
    }
    segment[0] =  (GHS_APP_Context.LHOChar.SegmentationHeader.FirstSegment) |
                 ((GHS_APP_Context.LHOChar.SegmentationHeader.LastSegment) << 1) |
                 ((GHS_APP_Context.LHOChar.SegmentationHeader.SegmentCounter) << 2);
    memcpy(&(segment[1]), &(p_Data[current_index]), total_size);
    current_index += total_size;

    msg_conf.Length = total_size;
    msg_conf.p_Payload = segment;

    ret = GHS_UpdateValue(CharOpcode, &msg_conf);
    if(ret != BLE_STATUS_SUCCESS)
    {
      if(CharOpcode == GHS_LHO)
      {
        LOG_INFO_APP("Live Health Observations last segment update fails\n");
      }
      return ret;
    }
    else
    {
      if(GHS_APP_Context.Lho_Indication_Status == Lho_INDICATION_ON)
      {  
        UTIL_SEQ_ClrEvt(1U << CFG_EVENT_GATT_INDICATION_COMPLETE);
        UTIL_SEQ_WaitEvt(1U << CFG_EVENT_GATT_INDICATION_COMPLETE);
      }
      if(CharOpcode == GHS_LHO)
      {
        LOG_INFO_APP("Live Health Observations last segment generated\n");
      }
    }
  } /* End else of if(total_size <= segment_size) */
  
  return ret;
}  

/* USER CODE END FD_LOCAL_FUNCTIONS */
