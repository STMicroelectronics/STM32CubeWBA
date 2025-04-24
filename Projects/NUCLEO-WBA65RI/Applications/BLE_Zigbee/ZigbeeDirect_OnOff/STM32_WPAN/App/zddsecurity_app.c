/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ZDDSecurity_app.c
  * @author  MCD Application Team
  * @brief   ZDDSecurity_app application definition.
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
#include "zddsecurity_app.h"
#include "zddsecurity.h"
#include "stm32_rtos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef enum
{
  Security25519aes_INDICATION_OFF,
  Security25519aes_INDICATION_ON,
  Security25519sha_INDICATION_OFF,
  Security25519sha_INDICATION_ON,
  P256sha_INDICATION_OFF,
  P256sha_INDICATION_ON,
  /* USER CODE BEGIN Service3_APP_SendInformation_t */

  /* USER CODE END Service3_APP_SendInformation_t */
  ZDDSECURITY_APP_SENDINFORMATION_LAST
} ZDDSECURITY_APP_SendInformation_t;

typedef struct
{
  ZDDSECURITY_APP_SendInformation_t     Security25519aes_Indication_Status;
  ZDDSECURITY_APP_SendInformation_t     Security25519sha_Indication_Status;
  ZDDSECURITY_APP_SendInformation_t     P256sha_Indication_Status;
  /* USER CODE BEGIN Service3_APP_Context_t */

  /* USER CODE END Service3_APP_Context_t */
  uint16_t              ConnectionHandle;
} ZDDSECURITY_APP_Context_t;

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
static ZDDSECURITY_APP_Context_t ZDDSECURITY_APP_Context;

uint8_t a_ZDDSECURITY_UpdateCharData[247];

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void ZDDSECURITY_Security25519aes_SendIndication(void);
static void ZDDSECURITY_Security25519sha_SendIndication(void);
static void ZDDSECURITY_P256sha_SendIndication(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void ZDDSECURITY_Notification(ZDDSECURITY_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service3_Notification_1 */

  /* USER CODE END Service3_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service3_Notification_Service3_EvtOpcode */

    /* USER CODE END Service3_Notification_Service3_EvtOpcode */

    case ZDDSECURITY_SECURITY25519AES_WRITE_EVT:
      /* USER CODE BEGIN Service3Char1_WRITE_EVT */

      /* USER CODE END Service3Char1_WRITE_EVT */
      break;

    case ZDDSECURITY_SECURITY25519AES_INDICATE_ENABLED_EVT:
      /* USER CODE BEGIN Service3Char1_INDICATE_ENABLED_EVT */

      /* USER CODE END Service3Char1_INDICATE_ENABLED_EVT */
      break;

    case ZDDSECURITY_SECURITY25519AES_INDICATE_DISABLED_EVT:
      /* USER CODE BEGIN Service3Char1_INDICATE_DISABLED_EVT */

      /* USER CODE END Service3Char1_INDICATE_DISABLED_EVT */
      break;

    case ZDDSECURITY_SECURITY25519SHA_WRITE_EVT:
      /* USER CODE BEGIN Service3Char2_WRITE_EVT */

      /* USER CODE END Service3Char2_WRITE_EVT */
      break;

    case ZDDSECURITY_SECURITY25519SHA_INDICATE_ENABLED_EVT:
      /* USER CODE BEGIN Service3Char2_INDICATE_ENABLED_EVT */

      /* USER CODE END Service3Char2_INDICATE_ENABLED_EVT */
      break;

    case ZDDSECURITY_SECURITY25519SHA_INDICATE_DISABLED_EVT:
      /* USER CODE BEGIN Service3Char2_INDICATE_DISABLED_EVT */

      /* USER CODE END Service3Char2_INDICATE_DISABLED_EVT */
      break;

    case ZDDSECURITY_P256SHA_WRITE_EVT:
      /* USER CODE BEGIN Service3Char3_WRITE_EVT */

      /* USER CODE END Service3Char3_WRITE_EVT */
      break;

    case ZDDSECURITY_P256SHA_INDICATE_ENABLED_EVT:
      /* USER CODE BEGIN Service3Char3_INDICATE_ENABLED_EVT */

      /* USER CODE END Service3Char3_INDICATE_ENABLED_EVT */
      break;

    case ZDDSECURITY_P256SHA_INDICATE_DISABLED_EVT:
      /* USER CODE BEGIN Service3Char3_INDICATE_DISABLED_EVT */

      /* USER CODE END Service3Char3_INDICATE_DISABLED_EVT */
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

void ZDDSECURITY_APP_EvtRx(ZDDSECURITY_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service3_APP_EvtRx_1 */

  /* USER CODE END Service3_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service3_APP_EvtRx_Service3_EvtOpcode */

    /* USER CODE END Service3_APP_EvtRx_Service3_EvtOpcode */
    case ZDDSECURITY_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service3_APP_CONN_HANDLE_EVT */

      /* USER CODE END Service3_APP_CONN_HANDLE_EVT */
      break;

    case ZDDSECURITY_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service3_APP_DISCON_HANDLE_EVT */

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

void ZDDSECURITY_APP_Init(void)
{
  UNUSED(ZDDSECURITY_APP_Context);
  ZDDSECURITY_Init();

  /* USER CODE BEGIN Service3_APP_Init */

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
__USED void ZDDSECURITY_Security25519aes_SendIndication(void) /* Property Indication */
{
  ZDDSECURITY_APP_SendInformation_t indication_on_off = Security25519aes_INDICATION_OFF;
  ZDDSECURITY_Data_t zddsecurity_indication_data;

  zddsecurity_indication_data.p_Payload = (uint8_t*)a_ZDDSECURITY_UpdateCharData;
  zddsecurity_indication_data.Length = 0;

  /* USER CODE BEGIN Service3Char1_IS_1 */

  /* USER CODE END Service3Char1_IS_1 */

  if (indication_on_off != Security25519aes_INDICATION_OFF)
  {
    ZDDSECURITY_UpdateValue(ZDDSECURITY_SECURITY25519AES, &zddsecurity_indication_data);
  }

  /* USER CODE BEGIN Service3Char1_IS_Last */

  /* USER CODE END Service3Char1_IS_Last */

  return;
}

__USED void ZDDSECURITY_Security25519sha_SendIndication(void) /* Property Indication */
{
  ZDDSECURITY_APP_SendInformation_t indication_on_off = Security25519sha_INDICATION_OFF;
  ZDDSECURITY_Data_t zddsecurity_indication_data;

  zddsecurity_indication_data.p_Payload = (uint8_t*)a_ZDDSECURITY_UpdateCharData;
  zddsecurity_indication_data.Length = 0;

  /* USER CODE BEGIN Service3Char2_IS_1 */

  /* USER CODE END Service3Char2_IS_1 */

  if (indication_on_off != Security25519sha_INDICATION_OFF)
  {
    ZDDSECURITY_UpdateValue(ZDDSECURITY_SECURITY25519SHA, &zddsecurity_indication_data);
  }

  /* USER CODE BEGIN Service3Char2_IS_Last */

  /* USER CODE END Service3Char2_IS_Last */

  return;
}

__USED void ZDDSECURITY_P256sha_SendIndication(void) /* Property Indication */
{
  ZDDSECURITY_APP_SendInformation_t indication_on_off = P256sha_INDICATION_OFF;
  ZDDSECURITY_Data_t zddsecurity_indication_data;

  zddsecurity_indication_data.p_Payload = (uint8_t*)a_ZDDSECURITY_UpdateCharData;
  zddsecurity_indication_data.Length = 0;

  /* USER CODE BEGIN Service3Char3_IS_1 */

  /* USER CODE END Service3Char3_IS_1 */

  if (indication_on_off != P256sha_INDICATION_OFF)
  {
    ZDDSECURITY_UpdateValue(ZDDSECURITY_P256SHA, &zddsecurity_indication_data);
  }

  /* USER CODE BEGIN Service3Char3_IS_Last */

  /* USER CODE END Service3Char3_IS_Last */

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

/* USER CODE END FD_LOCAL_FUNCTIONS */
