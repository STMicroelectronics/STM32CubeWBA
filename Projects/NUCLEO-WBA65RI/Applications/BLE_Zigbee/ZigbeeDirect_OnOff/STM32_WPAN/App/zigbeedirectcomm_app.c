/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ZigbeeDirectComm_app.c
  * @author  MCD Application Team
  * @brief   ZigbeeDirectComm_app application definition.
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
#include "zigbeedirectcomm_app.h"
#include "zigbeedirectcomm.h"
#include "stm32_rtos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef enum
{
  Commstatus_NOTIFICATION_OFF,
  Commstatus_NOTIFICATION_ON,
  /* USER CODE BEGIN Service2_APP_SendInformation_t */

  /* USER CODE END Service2_APP_SendInformation_t */
  ZIGBEEDIRECTCOMM_APP_SENDINFORMATION_LAST
} ZIGBEEDIRECTCOMM_APP_SendInformation_t;

typedef struct
{
  ZIGBEEDIRECTCOMM_APP_SendInformation_t     Commstatus_Notification_Status;
  /* USER CODE BEGIN Service2_APP_Context_t */

  /* USER CODE END Service2_APP_Context_t */
  uint16_t              ConnectionHandle;
} ZIGBEEDIRECTCOMM_APP_Context_t;

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
static ZIGBEEDIRECTCOMM_APP_Context_t ZIGBEEDIRECTCOMM_APP_Context;

uint8_t a_ZIGBEEDIRECTCOMM_UpdateCharData[247];

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void ZIGBEEDIRECTCOMM_Commstatus_SendNotification(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void ZIGBEEDIRECTCOMM_Notification(ZIGBEEDIRECTCOMM_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service2_Notification_1 */

  /* USER CODE END Service2_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service2_Notification_Service2_EvtOpcode */

    /* USER CODE END Service2_Notification_Service2_EvtOpcode */

    case ZIGBEEDIRECTCOMM_FORMNETWORK_WRITE_EVT:
      /* USER CODE BEGIN Service2Char1_WRITE_EVT */

      /* USER CODE END Service2Char1_WRITE_EVT */
      break;

    case ZIGBEEDIRECTCOMM_JOINNETWORK_WRITE_EVT:
      /* USER CODE BEGIN Service2Char2_WRITE_EVT */

      /* USER CODE END Service2Char2_WRITE_EVT */
      break;

    case ZIGBEEDIRECTCOMM_PERMITJOIN_WRITE_EVT:
      /* USER CODE BEGIN Service2Char3_WRITE_EVT */

      /* USER CODE END Service2Char3_WRITE_EVT */
      break;

    case ZIGBEEDIRECTCOMM_LEAVENETWORK_WRITE_EVT:
      /* USER CODE BEGIN Service2Char4_WRITE_EVT */

      /* USER CODE END Service2Char4_WRITE_EVT */
      break;

    case ZIGBEEDIRECTCOMM_COMMSTATUS_READ_EVT:
      /* USER CODE BEGIN Service2Char5_READ_EVT */

      /* USER CODE END Service2Char5_READ_EVT */
      break;

    case ZIGBEEDIRECTCOMM_COMMSTATUS_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service2Char5_NOTIFY_ENABLED_EVT */

      /* USER CODE END Service2Char5_NOTIFY_ENABLED_EVT */
      break;

    case ZIGBEEDIRECTCOMM_COMMSTATUS_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service2Char5_NOTIFY_DISABLED_EVT */

      /* USER CODE END Service2Char5_NOTIFY_DISABLED_EVT */
      break;

    case ZIGBEEDIRECTCOMM_MANAGEJOINER_WRITE_EVT:
      /* USER CODE BEGIN Service2Char6_WRITE_EVT */

      /* USER CODE END Service2Char6_WRITE_EVT */
      break;

    case ZIGBEEDIRECTCOMM_COMMIDENTITY_READ_EVT:
      /* USER CODE BEGIN Service2Char7_READ_EVT */

      /* USER CODE END Service2Char7_READ_EVT */
      break;

    case ZIGBEEDIRECTCOMM_COMMIDENTITY_WRITE_EVT:
      /* USER CODE BEGIN Service2Char7_WRITE_EVT */

      /* USER CODE END Service2Char7_WRITE_EVT */
      break;

    case ZIGBEEDIRECTCOMM_FINDBIND_WRITE_EVT:
      /* USER CODE BEGIN Service2Char8_WRITE_EVT */

      /* USER CODE END Service2Char8_WRITE_EVT */
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

void ZIGBEEDIRECTCOMM_APP_EvtRx(ZIGBEEDIRECTCOMM_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service2_APP_EvtRx_1 */

  /* USER CODE END Service2_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service2_APP_EvtRx_Service2_EvtOpcode */

    /* USER CODE END Service2_APP_EvtRx_Service2_EvtOpcode */
    case ZIGBEEDIRECTCOMM_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service2_APP_CONN_HANDLE_EVT */

      /* USER CODE END Service2_APP_CONN_HANDLE_EVT */
      break;

    case ZIGBEEDIRECTCOMM_DISCON_HANDLE_EVT :
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

void ZIGBEEDIRECTCOMM_APP_Init(void)
{
  UNUSED(ZIGBEEDIRECTCOMM_APP_Context);
  ZIGBEEDIRECTCOMM_Init();

  /* USER CODE BEGIN Service2_APP_Init */

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
__USED void ZIGBEEDIRECTCOMM_Commstatus_SendNotification(void) /* Property Notification */
{
  ZIGBEEDIRECTCOMM_APP_SendInformation_t notification_on_off = Commstatus_NOTIFICATION_OFF;
  ZIGBEEDIRECTCOMM_Data_t zigbeedirectcomm_notification_data;

  zigbeedirectcomm_notification_data.p_Payload = (uint8_t*)a_ZIGBEEDIRECTCOMM_UpdateCharData;
  zigbeedirectcomm_notification_data.Length = 0;

  /* USER CODE BEGIN Service2Char5_NS_1 */

  /* USER CODE END Service2Char5_NS_1 */

  if (notification_on_off != Commstatus_NOTIFICATION_OFF)
  {
    ZIGBEEDIRECTCOMM_UpdateValue(ZIGBEEDIRECTCOMM_COMMSTATUS, &zigbeedirectcomm_notification_data);
  }

  /* USER CODE BEGIN Service2Char5_NS_Last */

  /* USER CODE END Service2Char5_NS_Last */

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

/* USER CODE END FD_LOCAL_FUNCTIONS */
