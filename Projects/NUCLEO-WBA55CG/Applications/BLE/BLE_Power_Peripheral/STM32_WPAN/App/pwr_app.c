/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service1_app.c
  * @author  MCD Application Team
  * @brief   service1_app application definition.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "pwr_app.h"
#include "pwr.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32_lpm_if.h"
#include "host_stack_if.h"
#include "stm32_timer.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
  PWR_CO_APP_TRANSFER_REQ_OFF,
  PWR_CO_APP_TRANSFER_REQ_ON
} PWR_CO_App_Transfer_Req_Status_t;
/* USER CODE END PTD */

typedef enum
{
  Pwr_rx_NOTIFICATION_OFF,
  Pwr_rx_NOTIFICATION_ON,
  /* USER CODE BEGIN Service1_APP_SendInformation_t */

  /* USER CODE END Service1_APP_SendInformation_t */
  PWR_APP_SENDINFORMATION_LAST
} PWR_APP_SendInformation_t;

typedef struct
{
  PWR_APP_SendInformation_t     Pwr_rx_Notification_Status;
  /* USER CODE BEGIN Service1_APP_Context_t */
  PWR_CO_App_Transfer_Req_Status_t Rx_TransferReq;
  uint8_t   connectionstatus;
  /* USER CODE END Service1_APP_Context_t */
  uint16_t              ConnectionHandle;
} PWR_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
PWR_CO_GPIO_Status_t PWR_CO_GPIO_Status;
uint8_t D_buffer[USER_PAYLOAD_LENGTH] = {0};
/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static PWR_APP_Context_t PWR_APP_Context;

uint8_t a_PWR_UpdateCharData[247];

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void PWR_Pwr_rx_SendNotification(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void PWR_Notification(PWR_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_Notification_1 */

  /* USER CODE END Service1_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_Notification_Service1_EvtOpcode */

    /* USER CODE END Service1_Notification_Service1_EvtOpcode */

    case PWR_PWR_TX_READ_EVT:
      /* USER CODE BEGIN Service1Char1_READ_EVT */

      /* USER CODE END Service1Char1_READ_EVT */
      break;

    case PWR_PWR_TX_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN Service1Char1_WRITE_NO_RESP_EVT */

      /* USER CODE END Service1Char1_WRITE_NO_RESP_EVT */
      break;

    case PWR_PWR_RX_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char2_NOTIFY_ENABLED_EVT */
               PWR_APP_Context.Rx_TransferReq = PWR_CO_APP_TRANSFER_REQ_ON;
               PWR_CO_GPIO_Status = PWR_CO_GPIO_TIM_RESET_ON;
      /* USER CODE END Service1Char2_NOTIFY_ENABLED_EVT */
      break;

    case PWR_PWR_RX_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char2_NOTIFY_DISABLED_EVT */
               PWR_APP_Context.Rx_TransferReq = PWR_CO_APP_TRANSFER_REQ_OFF;
               PWR_CO_GPIO_Status = PWR_CO_GPIO_TIM_RESET_ON;

      /* USER CODE END Service1Char2_NOTIFY_DISABLED_EVT */
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

void PWR_APP_EvtRx(PWR_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_APP_EvtRx_1 */

  /* USER CODE END Service1_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_APP_EvtRx_Service1_EvtOpcode */

    /* USER CODE END Service1_APP_EvtRx_Service1_EvtOpcode */
    case PWR_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_CONN_HANDLE_EVT */
 PWR_APP_Context.connectionstatus = APP_BLE_CONNECTED_SERVER;
      /* USER CODE END Service1_APP_CONN_HANDLE_EVT */
      break;

    case PWR_DISCON_HANDLE_EVT :
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

void PWR_APP_Init(void)
{
  UNUSED(PWR_APP_Context);
  PWR_Init();

  /* USER CODE BEGIN Service1_APP_Init */
  PWR_APP_Context.Rx_TransferReq = PWR_CO_APP_TRANSFER_REQ_OFF;
  PWR_APP_Context.Pwr_rx_Notification_Status = Pwr_rx_NOTIFICATION_ON;
  PWR_CO_GPIO_Status = PWR_CO_GPIO_ON;
  /* USER CODE END Service1_APP_Init */
  return;
}

/* USER CODE BEGIN FD */
 void Send_Data(void)
{

     PWR_Data_t Data_Rx;
     Data_Rx.p_Payload = (uint8_t*)D_buffer;
     Data_Rx.Length = USER_PAYLOAD_LENGTH;

  if((PWR_APP_Context.Rx_TransferReq != PWR_CO_APP_TRANSFER_REQ_OFF)
    && (PWR_APP_Context.Pwr_rx_Notification_Status != Pwr_rx_NOTIFICATION_OFF) )
  {

 PWR_UpdateValue(PWR_PWR_RX,&Data_Rx);

  }
   BleStackCB_Process();

}

void Disable_GPIOs(void)
{
  if(CFG_LPM_LEVEL > 1)
  {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
  
    /*Put all GPIOs on analog*/
    
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin  = GPIO_PIN_All;
  
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
   HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); 
   HAL_GPIO_Init(GPIOH, &GPIO_InitStruct); 
   
__HAL_RCC_GPIOA_CLK_DISABLE();
__HAL_RCC_GPIOB_CLK_DISABLE();
__HAL_RCC_GPIOC_CLK_DISABLE();
__HAL_RCC_GPIOH_CLK_DISABLE();

PWR_CO_GPIO_Status = PWR_CO_GPIO_OFF;
  }

}
/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void PWR_Pwr_rx_SendNotification(void) /* Property Notification */
{
  PWR_APP_SendInformation_t notification_on_off = Pwr_rx_NOTIFICATION_OFF;
  PWR_Data_t pwr_notification_data;

  pwr_notification_data.p_Payload = (uint8_t*)a_PWR_UpdateCharData;
  pwr_notification_data.Length = 0;

  /* USER CODE BEGIN Service1Char2_NS_1*/

  /* USER CODE END Service1Char2_NS_1*/

  if (notification_on_off != Pwr_rx_NOTIFICATION_OFF)
  {
    PWR_UpdateValue(PWR_PWR_RX, &pwr_notification_data);
  }

  /* USER CODE BEGIN Service1Char2_NS_Last*/

  /* USER CODE END Service1Char2_NS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/

/* USER CODE END FD_LOCAL_FUNCTIONS*/
