/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    uart_rcp.c
  * @author  MCD Application Team
  * @brief   This file implements the OpenThread platform abstraction for RCP
  *          UART communication.
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

#ifdef OPENTHREAD_RCP

#include "main.h"
#include "platform_wba.h"
#include "uart.h"
#include "log_module.h"

/* Extern variables  ---------------------------------------------------------*/
#ifndef OT_RCP_UART_HANDLER
extern UART_HandleTypeDef huart1;
#define OT_RCP_UART_HANDLER huart1
#endif


/* Private function prototypes -----------------------------------------------*/
static void otUart_TxCpltCallback(UART_HandleTypeDef *huart);
static void otUart_RxCpltCallback(UART_HandleTypeDef *huart);



/* Private define ------------------------------------------------------------*/
#define BUFFER_SIZE   1300  /* RX RCP buffer size = SPINEL_FRAME_MAX_SIZE */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t Rx_buffer[BUFFER_SIZE] = {0};
static uint16_t Rx_Buff_idx;
UART_HandleTypeDef *OtRcpHuart = &OT_RCP_UART_HANDLER;

uint8_t otUartRX = 0;

bool hdlc_frame_ongoing = FALSE;

char spineldbgout[32];
char spineldbgin[32];

/*
 * UART used for commands input/output
 * The selection among those available is done in this order:
 * USART1 --> Error (no UART found)
 *
 */


otError otPlatUartEnable(void)
{
  otError error = OT_ERROR_NONE;

  Rx_Buff_idx = 0U;

  /*
   * It is assumed that UART1 MSP Init is done by
   * CubeMX layer. As such, no need to call
   * MX_USART1_UART_Init & HAL_UART_MspInit.
   */

  /* register callbacks */
  OtRcpHuart->TxCpltCallback = otUart_TxCpltCallback;
  OtRcpHuart->RxCpltCallback = otUart_RxCpltCallback;
  HAL_UART_Receive_IT(OtRcpHuart, &otUartRX, 1);

  return error;
}

otError otPlatUartDisable(void)
{
  return OT_ERROR_NOT_IMPLEMENTED;
}


otError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
  if (aBufLength > 0)
  {
    /* Wait for uart to be ready */
    while(OtRcpHuart->gState != HAL_UART_STATE_READY);
    /* aBuf will not be written by OT stack while otPlatUartSendDone() is not called */
    HAL_UART_Transmit_DMA(OtRcpHuart, aBuf, aBufLength);
    APP_DBG("[RCP] Tx tid %d cmd %d prop 0x%x size %d\r\n", aBuf[1]&(0x0f), aBuf[2], aBuf[3], aBufLength);
    if (aBuf[3] == 0x71) //Raw frame
    {
      sprintf(spineldbgout, "%02x, %02x, %02x, %02x, %02x",aBuf[4], aBuf[5], aBuf[6], aBuf[7], aBuf[8]);
      APP_DBG(spineldbgout);
    }
  }

  return OT_ERROR_NONE;
}

static void otUart_TxCpltCallback(UART_HandleTypeDef *huart)
{
  UNUSED(huart);
  //Prevent OT stack : Uart Tx RCP done
  otPlatUartSendDone();
}

static void otUart_RxCpltCallback(UART_HandleTypeDef *huart)
{
  UNUSED(huart);
  bool hdlc_fanion;
  HAL_UART_Receive_IT(OtRcpHuart, &otUartRX, 1);

  hdlc_fanion = (otUartRX == 0x7E) ? TRUE : FALSE;

  if (hdlc_frame_ongoing == TRUE)
  {
    Rx_buffer[Rx_Buff_idx] = otUartRX;
    Rx_Buff_idx++;
    /* End of trame, forward */
    if (hdlc_fanion)
    {
      hdlc_frame_ongoing = FALSE;
      otPlatUartReceived(Rx_buffer, Rx_Buff_idx);
      APP_DBG("[RCP] Rx tid %d cmd %d prop 0x%x size %d\r\n", Rx_buffer[1]&(0x0f),Rx_buffer[2], Rx_buffer[3], Rx_Buff_idx);
      if (Rx_buffer[3] == 0x71)
      {
        sprintf(spineldbgout, "%02x, %02x, %02x, %02x, %02x",Rx_buffer[4], Rx_buffer[5], Rx_buffer[6], Rx_buffer[7], Rx_buffer[8]);
        APP_DBG(spineldbgout);
      }
      memset(Rx_buffer, 0x0, Rx_Buff_idx);
      Rx_Buff_idx = 0;
    }
  }
  else
  {
    if (hdlc_fanion)
    {
      hdlc_frame_ongoing = TRUE;
      Rx_buffer[0] = otUartRX;
      Rx_Buff_idx = 1;
    }
  }
}

#endif /* OPENTHREAD_RCP */