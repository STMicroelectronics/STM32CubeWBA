/**
  ******************************************************************************
  * @file    coc_peripheral_app.c
  * @author  MCD Application Team
  * @brief   BLE peripheral COC Application
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_common.h"
#include "dbg_trace.h"
#include "ble.h"
#include "app_ble.h"
#include "coc_peripheral_app.h"
#include "stm32_seq.h"
#include "usart_if.h"
#include "host_stack_if.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private defines ------------------------------------------------------------*/
#define BUFFUARTRX_SIZE   248
#define BUFFCOCTX_SIZE    BUFFUARTRX_SIZE
/* Private macros -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint8_t a_buffUartRx[BUFFUARTRX_SIZE];
static uint8_t buffUartRxIndex;

static uint8_t a_buffCocTx[BUFFCOCTX_SIZE];
static uint8_t buffCocTxLen;

/* Global variables ----------------------------------------------------------*/
extern uint8_t charRx;
extern uint8_t real_time;

/* Private function prototypes -----------------------------------------------*/
static void RxUART_Init(void);
static void COC_Periph_APP_Terminal_UART_RxCpltCallback(uint8_t *pdata, uint16_t size, uint8_t error);
static void COC_PERIPH_APP_Terminal_Init(void);

/* Functions Definition ------------------------------------------------------*/

void COC_PERIPH_APP_Notification(COC_APP_ConnHandle_Not_evt_t *pNotification)
{
  uint8_t i;
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  
  switch(pNotification->CoC_Evt_Opcode)
  {
    case EXCHANGE_ATT_MTU:
      ret = aci_gatt_exchange_config(BleCoCContextPeriph.Conn_Handle);
      if (ret == BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Success: change MTU\n");
      }
      break;
    case BLE_CONN_HANDLE_EVT :
      UTIL_SEQ_SetTask(1U << CFG_TASK_CONN_UPDATE_REG_ID, CFG_SEQ_PRIO_0);
      break;
    case BLE_DISCON_HANDLE_EVT :
      break;    
    case L2CAP_DATA_RECEIVED:
    {
      /* display received data */
      for (i=2;i<pNotification->DataLength;i++)
      {
        LOG_INFO_APP("%c", pNotification->DataTable[i]);
      }
    }
    break;
    default:
      break;
  }

  return;
}

void COC_PERIPH_APP_Init(void)
{
  UTIL_SEQ_RegTask(1U << CFG_TASK_COC_PERIPH_TX_REQ_ID, UTIL_SEQ_RFU, PeriphSendData );
  
  COC_PERIPH_APP_Terminal_Init();

  return;
}

/**
  * @brief  This function initialize terminal
  * @param  None
  * @retval None
  */
static void COC_PERIPH_APP_Terminal_Init(void)
{ 
  RxUART_Init();
  real_time = 1;
  LOG_INFO_APP("Real time mode\n");

  return;
}

static void RxUART_Init(void)
{
  UART_StartRx(COC_Periph_APP_Terminal_UART_RxCpltCallback);
}

static void COC_Periph_APP_Terminal_UART_RxCpltCallback(uint8_t *pdata, uint16_t size, uint8_t error)
{
 
  uint8_t byte_received;
    
  if(size == 1)
  {
    byte_received = pdata[0];
    
    if(buffUartRxIndex < sizeof(a_buffUartRx))
    {
      a_buffUartRx[buffUartRxIndex++] = byte_received;
    }
    else
    {
      buffUartRxIndex = 0;
    }
    
    if( (real_time ==1) || (real_time == 0 && (byte_received == '\n')))
    {
      memcpy(&a_buffCocTx[2], &a_buffUartRx[0], buffUartRxIndex);
      buffCocTxLen = buffUartRxIndex;

      buffUartRxIndex = 0;
      a_buffCocTx[0] = buffCocTxLen;
      a_buffCocTx[1] = 0;

      UTIL_SEQ_SetTask(1 << CFG_TASK_COC_PERIPH_TX_REQ_ID, CFG_SEQ_PRIO_0);      
    }    
  }
  else
  {
    
  }
  UART_StartRx(COC_Periph_APP_Terminal_UART_RxCpltCallback);
  return; 
}


/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/


void PeriphSendData( void )
{
  tBleStatus status = BLE_STATUS_INVALID_PARAMS;

  /*Data Packet to send to remote*/  
  status = aci_l2cap_coc_tx_data(BleCoCContextPeriph.Channel_Index_List, 
                                 2 + buffCocTxLen,/* 2 bytes of header */ 
                                 (uint8_t *) &a_buffCocTx[0]);
  
  if (status != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("==>> aci_l2cap_coc_tx_data : Fail, reason: 0x%02X\n", status);
  }
  BleStackCB_Process();
  return;
}