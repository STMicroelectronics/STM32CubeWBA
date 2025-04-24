/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Tunneling_app.c
  * @author  MCD Application Team
  * @brief   Tunneling_app application definition.
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
#include "tunneling_app.h"
#include "tunneling.h"
#include "stm32_rtos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef enum
{
  Tunnzdtsnpdu_INDICATION_OFF,
  Tunnzdtsnpdu_INDICATION_ON,
  /* USER CODE BEGIN Service4_APP_SendInformation_t */

  /* USER CODE END Service4_APP_SendInformation_t */
  TUNNELING_APP_SENDINFORMATION_LAST
} TUNNELING_APP_SendInformation_t;

typedef struct
{
  TUNNELING_APP_SendInformation_t     Tunnzdtsnpdu_Indication_Status;
  /* USER CODE BEGIN Service4_APP_Context_t */

  /* USER CODE END Service4_APP_Context_t */
  uint16_t              ConnectionHandle;
} TUNNELING_APP_Context_t;

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
static TUNNELING_APP_Context_t TUNNELING_APP_Context;

uint8_t a_TUNNELING_UpdateCharData[247];

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void TUNNELING_Tunnzdtsnpdu_SendIndication(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void TUNNELING_Notification(TUNNELING_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service4_Notification_1 */

  /* USER CODE END Service4_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service4_Notification_Service4_EvtOpcode */

    /* USER CODE END Service4_Notification_Service4_EvtOpcode */

    case TUNNELING_TUNNZDTSNPDU_WRITE_EVT:
      /* USER CODE BEGIN Service4Char1_WRITE_EVT */

      /* USER CODE END Service4Char1_WRITE_EVT */
      break;

    case TUNNELING_TUNNZDTSNPDU_INDICATE_ENABLED_EVT:
      /* USER CODE BEGIN Service4Char1_INDICATE_ENABLED_EVT */

      /* USER CODE END Service4Char1_INDICATE_ENABLED_EVT */
      break;

    case TUNNELING_TUNNZDTSNPDU_INDICATE_DISABLED_EVT:
      /* USER CODE BEGIN Service4Char1_INDICATE_DISABLED_EVT */

      /* USER CODE END Service4Char1_INDICATE_DISABLED_EVT */
      break;

    default:
      /* USER CODE BEGIN Service4_Notification_default */

      /* USER CODE END Service4_Notification_default */
      break;
  }
  /* USER CODE BEGIN Service4_Notification_2 */

  /* USER CODE END Service4_Notification_2 */
  return;
}

void TUNNELING_APP_EvtRx(TUNNELING_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service4_APP_EvtRx_1 */

  /* USER CODE END Service4_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service4_APP_EvtRx_Service4_EvtOpcode */

    /* USER CODE END Service4_APP_EvtRx_Service4_EvtOpcode */
    case TUNNELING_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service4_APP_CONN_HANDLE_EVT */

      /* USER CODE END Service4_APP_CONN_HANDLE_EVT */
      break;

    case TUNNELING_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service4_APP_DISCON_HANDLE_EVT */

      /* USER CODE END Service4_APP_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN Service4_APP_EvtRx_default */

      /* USER CODE END Service4_APP_EvtRx_default */
      break;
  }

  /* USER CODE BEGIN Service4_APP_EvtRx_2 */

  /* USER CODE END Service4_APP_EvtRx_2 */

  return;
}

void TUNNELING_APP_Init(void)
{
  UNUSED(TUNNELING_APP_Context);
  TUNNELING_Init();

  /* USER CODE BEGIN Service4_APP_Init */

  /* USER CODE END Service4_APP_Init */
  return;
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void TUNNELING_Tunnzdtsnpdu_SendIndication(void) /* Property Indication */
{
  TUNNELING_APP_SendInformation_t indication_on_off = Tunnzdtsnpdu_INDICATION_OFF;
  TUNNELING_Data_t tunneling_indication_data;

  tunneling_indication_data.p_Payload = (uint8_t*)a_TUNNELING_UpdateCharData;
  tunneling_indication_data.Length = 0;

  /* USER CODE BEGIN Service4Char1_IS_1 */

  /* USER CODE END Service4Char1_IS_1 */

  if (indication_on_off != Tunnzdtsnpdu_INDICATION_OFF)
  {
    TUNNELING_UpdateValue(TUNNELING_TUNNZDTSNPDU, &tunneling_indication_data);
  }

  /* USER CODE BEGIN Service4Char1_IS_Last */

  /* USER CODE END Service4Char1_IS_Last */

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

/* USER CODE END FD_LOCAL_FUNCTIONS */
