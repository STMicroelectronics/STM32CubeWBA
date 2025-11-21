/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dis_app.c
  * @author  MCD Application Team
  * @brief   DIS application definition.
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
#include "dis_app.h"
#include "dis.h"
#include "stm32_rtos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef enum
{
  /* USER CODE BEGIN Service2_APP_SendInformation_t */

  /* USER CODE END Service2_APP_SendInformation_t */
  DIS_APP_SENDINFORMATION_LAST
} DIS_APP_SendInformation_t;

typedef struct
{
  /* USER CODE BEGIN Service2_APP_Context_t */

  /* USER CODE END Service2_APP_Context_t */
  uint16_t              ConnectionHandle;
} DIS_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DISAPP_MANUFACTURER_NAME              "STM"

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static DIS_APP_Context_t DIS_APP_Context;

uint8_t a_DIS_UpdateCharData[247];

/* USER CODE BEGIN PV */
static const uint8_t system_id[BLE_CFG_DIS_SYSTEM_ID_LEN_MAX] =
{
  (uint8_t)((DISAPP_MANUFACTURER_ID & 0xFF0000) >> 16),
  (uint8_t)((DISAPP_MANUFACTURER_ID & 0x00FF00) >> 8),
  (uint8_t)(DISAPP_MANUFACTURER_ID & 0x0000FF),
  0xFE,
  0xFF,
  (uint8_t)((DISAPP_OUI & 0xFF0000) >> 16),
  (uint8_t)((DISAPP_OUI & 0x00FF00) >> 8),
  (uint8_t)(DISAPP_OUI & 0x0000FF)
};
static uint8_t regulatory_certification_data_list[] =
{
  0x00, 0x01,  /* Regulatory Certification Data list Count: 1 */
  0x00, 0x0C,  /* Regulatory Certification Data list Length: 12 bytes */
  0x02, 0x01,  /* Regulatory Certification Data list: auth-body-continua 2, auth-body-struc-type = continua-version-struct = 1 */
  0x00, 0x08,  /* auth-body-data.length = 8 */
  0x01, 0x05,  /* auth-body-data:  ContinuaBodyStruct.major-IG-version = 5, ContinuaBodyStruct.minor-IG-version = 1 */
  0x00, 0x01,  /* CertifiedDeviceClassList.count = 1 */
  0x00, 0x02,  /* CertifiedDeviceClassList.length = 2 */
  0x80, 0x08,  /* CertifiedDeviceClassList = 0x8008 */
};
static uint8_t udi_for_medical_devices[] =
{
  0x01, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,  /* 01 Device Identifier */
  0x11, 0x01, 0x02, 0x03,                          /* 17 Expiration Date */
  0x0A, 0x01, 0x02, 0x03,                          /* 10 Lot Number */
  0x15, 0x01, 0x02, 0x03, 0x04, 0x05               /* 21 Serial Number */
};
static uint8_t PnP_ID[] =
{
  0x01,                   /* Vendor ID Source */
  0x01, 0x02,             /* Vendor ID */
  0x01, 0x02,             /* Product ID */
  0x01, 0x02,             /* Product Version */
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void DIS_Notification(DIS_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service2_Notification_1 */

  /* USER CODE END Service2_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service2_Notification_Service2_EvtOpcode */

    /* USER CODE END Service2_Notification_Service2_EvtOpcode */

    case DIS_SRS_READ_EVT:
      /* USER CODE BEGIN Service2Char1_READ_EVT */

      /* USER CODE END Service2Char1_READ_EVT */
      break;

    case DIS_MANS_READ_EVT:
      /* USER CODE BEGIN Service2Char2_READ_EVT */

      /* USER CODE END Service2Char2_READ_EVT */
      break;

    case DIS_MONS_READ_EVT:
      /* USER CODE BEGIN Service2Char3_READ_EVT */

      /* USER CODE END Service2Char3_READ_EVT */
      break;

    case DIS_SNS_READ_EVT:
      /* USER CODE BEGIN Service2Char4_READ_EVT */

      /* USER CODE END Service2Char4_READ_EVT */
      break;

    case DIS_SYID_READ_EVT:
      /* USER CODE BEGIN Service2Char5_READ_EVT */

      /* USER CODE END Service2Char5_READ_EVT */
      break;

    case DIS_UDIFMD_READ_EVT:
      /* USER CODE BEGIN Service2Char6_READ_EVT */

      /* USER CODE END Service2Char6_READ_EVT */
      break;

    case DIS_FRS_READ_EVT:
      /* USER CODE BEGIN Service2Char7_READ_EVT */

      /* USER CODE END Service2Char7_READ_EVT */
      break;

    case DIS_HRS_READ_EVT:
      /* USER CODE BEGIN Service2Char8_READ_EVT */

      /* USER CODE END Service2Char8_READ_EVT */
      break;

    case DIS_IRCDL_READ_EVT:
      /* USER CODE BEGIN Service2Char9_READ_EVT */

      /* USER CODE END Service2Char9_READ_EVT */
      break;

    case DIS_PNI_READ_EVT:
      /* USER CODE BEGIN Service2Char10_READ_EVT */

      /* USER CODE END Service2Char10_READ_EVT */
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

void DIS_APP_EvtRx(DIS_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service2_APP_EvtRx_1 */

  /* USER CODE END Service2_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service2_APP_EvtRx_Service2_EvtOpcode */

    /* USER CODE END Service2_APP_EvtRx_Service2_EvtOpcode */
    case DIS_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service2_APP_CONN_HANDLE_EVT */

      /* USER CODE END Service2_APP_CONN_HANDLE_EVT */
      break;

    case DIS_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service2_APP_DISCON_HANDLE_EVT */

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

void DIS_APP_Init(void)
{
  UNUSED(DIS_APP_Context);
  DIS_Init();

  /* USER CODE BEGIN Service2_APP_Init */
  DIS_Data_t dis_information_data;

  LOG_INFO_APP("DIS_APP_Init\n\r");
  /**
   * Update SOFTWARE REVISION Information
   *
   * @param UUID
   * @param pPData
   * @return
   */
  dis_information_data.p_Payload = (uint8_t*)DISAPP_SOFTWARE_REVISION_NUMBER;
  dis_information_data.Length = sizeof(DISAPP_SOFTWARE_REVISION_NUMBER);
  DIS_UpdateValue(DIS_SRS, &dis_information_data);

  /**
   * Update MANUFACTURER NAME Information
   *
   * @param UUID
   * @param pPData
   * @return
   */
  dis_information_data.p_Payload = (uint8_t*)DISAPP_MANUFACTURER_NAME;
  dis_information_data.Length = sizeof(DISAPP_MANUFACTURER_NAME);
  DIS_UpdateValue(DIS_MANS, &dis_information_data);

  /**
   * Update MODEL NUMBER Information
   *
   * @param UUID
   * @param pPData
   * @return
   */
  dis_information_data.p_Payload = (uint8_t*)DISAPP_MODEL_NUMBER;
  dis_information_data.Length = sizeof(DISAPP_MODEL_NUMBER);
  DIS_UpdateValue(DIS_MONS, &dis_information_data);

  /**
   * Update SERIAL NUMBER Information
   *
   * @param UUID
   * @param pPData
   * @return
   */
  dis_information_data.p_Payload = (uint8_t*)DISAPP_SERIAL_NUMBER;
  dis_information_data.Length = sizeof(DISAPP_SERIAL_NUMBER);
  DIS_UpdateValue(DIS_SNS, &dis_information_data);

  /**
   * Update System ID
   *
   * @param UUID
   * @param pPData
   * @return
   */
  dis_information_data.p_Payload = (uint8_t *)system_id;
  dis_information_data.Length = sizeof(system_id);
  DIS_UpdateValue(DIS_SYID, &dis_information_data);

  /**
   * Update UDI For Medical Device
   *
   * @param UUID
   * @param pPData
   * @return
   */
  dis_information_data.p_Payload = (uint8_t *)udi_for_medical_devices;
  dis_information_data.Length = sizeof(udi_for_medical_devices);
  DIS_UpdateValue(DIS_UDIFMD, &dis_information_data);

  /**
   * Update FIRMWARE REVISION Information
   *
   * @param UUID
   * @param pPData
   * @return
   */
  dis_information_data.p_Payload = (uint8_t*)DISAPP_FIRMWARE_REVISION_NUMBER;
  dis_information_data.Length = sizeof(DISAPP_FIRMWARE_REVISION_NUMBER);
  DIS_UpdateValue(DIS_FRS, &dis_information_data);

  /**
   * Update HARDWARE REVISION Information
   *
   * @param UUID
   * @param pPData
   * @return
   */
  dis_information_data.p_Payload = (uint8_t*)DISAPP_HARDWARE_REVISION_NUMBER;
  dis_information_data.Length = sizeof(DISAPP_HARDWARE_REVISION_NUMBER);
  DIS_UpdateValue(DIS_HRS, &dis_information_data);

  /**
   * Update Regulatory Certification Data List
   *
   * @param UUID
   * @param pPData
   * @return
   */
  dis_information_data.p_Payload = (uint8_t *)regulatory_certification_data_list;
  dis_information_data.Length = sizeof(regulatory_certification_data_list);
  DIS_UpdateValue(DIS_IRCDL, &dis_information_data);

  /**
   * Update PnP ID
   *
   * @param UUID
   * @param pPData
   * @return
   */
  dis_information_data.p_Payload = (uint8_t *)PnP_ID;
  dis_information_data.Length = sizeof(PnP_ID);
  DIS_UpdateValue(DIS_PNI, &dis_information_data);
  /* USER CODE END Service2_APP_Init */
  return;
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

/* USER CODE END FD_LOCAL_FUNCTIONS */
