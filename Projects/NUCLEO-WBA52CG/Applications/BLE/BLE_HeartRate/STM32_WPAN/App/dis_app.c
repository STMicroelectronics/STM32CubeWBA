/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service2_app.c
  * @author  MCD Application Team
  * @brief   service2_app application definition.
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
#include "dis_app.h"
#include "dis.h"
#include "stm32_seq.h"

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
#define DISAPP_MODEL_NUMBER                   "4502-1.0"
#define DISAPP_SERIAL_NUMBER                  "1.0"
#define DISAPP_HARDWARE_REVISION_NUMBER       "1.0"
#define DISAPP_FIRMWARE_REVISION_NUMBER       "1.0"
/*#define DISAPP_SOFTWARE_REVISION_NUMBER       "1.0"
#define DISAPP_OUI                            0x123456
#define DISAPP_MANUFACTURER_ID                0x9ABCDE*/
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

    case DIS_MANS_READ_EVT:
      /* USER CODE BEGIN Service2Char1_READ_EVT */

      /* USER CODE END Service2Char1_READ_EVT */
      break;

    case DIS_MNBS_READ_EVT:
      /* USER CODE BEGIN Service2Char2_READ_EVT */

      /* USER CODE END Service2Char2_READ_EVT */
      break;

    case DIS_SNS_READ_EVT:
      /* USER CODE BEGIN Service2Char3_READ_EVT */

      /* USER CODE END Service2Char3_READ_EVT */
      break;

    case DIS_HRS_READ_EVT:
      /* USER CODE BEGIN Service2Char4_READ_EVT */

      /* USER CODE END Service2Char4_READ_EVT */
      break;

    case DIS_FRS_READ_EVT:
      /* USER CODE BEGIN Service2Char5_READ_EVT */

      /* USER CODE END Service2Char5_READ_EVT */
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

    /* USER CODE END Service2_Notification_Service2_EvtOpcode */
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
  
  dis_information_data.p_Payload = (uint8_t*)DISAPP_MANUFACTURER_NAME;
  dis_information_data.Length = sizeof(DISAPP_MANUFACTURER_NAME);
  DIS_UpdateValue(DIS_MANS, &dis_information_data);
  
  dis_information_data.p_Payload = (uint8_t*)DISAPP_MODEL_NUMBER;
  dis_information_data.Length = sizeof(DISAPP_MODEL_NUMBER);
  DIS_UpdateValue(DIS_MNBS, &dis_information_data);
  
  dis_information_data.p_Payload = (uint8_t*)DISAPP_SERIAL_NUMBER;
  dis_information_data.Length = sizeof(DISAPP_SERIAL_NUMBER);
  DIS_UpdateValue(DIS_SNS, &dis_information_data);
  
  dis_information_data.p_Payload = (uint8_t*)DISAPP_HARDWARE_REVISION_NUMBER;
  dis_information_data.Length = sizeof(DISAPP_HARDWARE_REVISION_NUMBER);
  DIS_UpdateValue(DIS_HRS, &dis_information_data);
  
  dis_information_data.p_Payload = (uint8_t*)DISAPP_FIRMWARE_REVISION_NUMBER;
  dis_information_data.Length = sizeof(DISAPP_FIRMWARE_REVISION_NUMBER);
  DIS_UpdateValue(DIS_FRS, &dis_information_data);
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

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/

/* USER CODE END FD_LOCAL_FUNCTIONS*/
