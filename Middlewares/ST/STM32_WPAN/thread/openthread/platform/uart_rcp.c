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
static void otUart_ErrorCallback(UART_HandleTypeDef *huart);
static void otUart_RxEvntCpltCallback(UART_HandleTypeDef *huart, uint16_t size);


/* Private define ------------------------------------------------------------*/
#define BUFFER_SIZE    255  /* RX RCP spinel uart buffer size */

/** RX RCP Spinel Uart buffer number, min 2 (one buffer used for reception while 
 *  the second one is used for processing 
 *   Can be increased in case of high trafic on spinel 
**/
#define SPIN_BUFFER_NB 2

#define HDLC_FANION  0x7E /* Do not modify, standart HDLC fanion (HDLC lite is used to encapsulate spinel) */
/* Private macro -------------------------------------------------------------*/

/** Spinel Rx buffer with size 
 *   
**/
typedef struct {
  uint8_t SpinelRxBuffer[BUFFER_SIZE];
  uint16_t size;
} SpinelRx_Frame_t;

/** Spinel Rx Queue containing SPIN_BUFFER_NB of SpinelRx_Frame_t
 *  InIdx  : index by UART DMA
 *  OutIdx : current frame index processed by OT otPlatUartReceived
**/
typedef struct {
  SpinelRx_Frame_t Frame[SPIN_BUFFER_NB];
  uint8_t InIdx;
  uint8_t OutIdx;
}SpinelRx_Queue_t;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

SpinelRx_Queue_t SpinelRxQueue;

UART_HandleTypeDef *OtRcpHuart = &OT_RCP_UART_HANDLER;

char spineldbgout[32];
char spineldbgin[32];

/* stubs for external functions 
   prevent linking errors when not defined in application */
__WEAK void APP_THREAD_ScheduleRcpSpinelRx(void)
{
  /* Need to be implemented by user (os dependant) */
  while(1);
}

otError otPlatUartEnable(void)
{
  otError error = OT_ERROR_NONE;
  HAL_StatusTypeDef HalStatus;
  /*
   * It is assumed that UART MSP Init is done by
   * CubeMX layer. As such, no need to call
   * MX_USART_UART_Init & HAL_UART_MspInit.
   */

  /* Register Uart callbacks */
  OtRcpHuart->TxCpltCallback = otUart_TxCpltCallback;
  OtRcpHuart->RxEventCallback = otUart_RxEvntCpltCallback;
  OtRcpHuart->ErrorCallback   = otUart_ErrorCallback;
  
  SpinelRxQueue.InIdx = 0;
  SpinelRxQueue.OutIdx = 0;
  
  /* Start Reception in DMA IdleMode, Receive until buffer size has been reached or no activity on UART Rx channel 
     When spinel frame is received, callback otUart_RxEvntCpltCallback is triggered
  */
  HalStatus = HAL_UARTEx_ReceiveToIdle_DMA(OtRcpHuart, SpinelRxQueue.Frame[SpinelRxQueue.InIdx].SpinelRxBuffer, BUFFER_SIZE);
  __HAL_DMA_DISABLE_IT(OtRcpHuart->hdmarx, DMA_IT_HT); //Disable Half Transfer IT (not used but always set by HAL_UARTEx_ReceiveToIdle_DMA)
  if (HalStatus != HAL_OK)
  {
    otPlatLog(OT_LOG_LEVEL_WARN, OT_LOG_REGION_PLATFORM,"[SPNL] Uart Start First Reception Error");
    error = OT_ERROR_FAILED;
  }

  return error;
}

otError otPlatUartDisable(void)
{
  return OT_ERROR_NOT_IMPLEMENTED;
}


otError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
  HAL_StatusTypeDef HalStatus;
  if (aBufLength > 0)
  {
    /* Wait for uart to be ready */
    while(OtRcpHuart->gState != HAL_UART_STATE_READY);
    /* aBuf will not be written by OT stack while otPlatUartSendDone() is not called */
    HalStatus = HAL_UART_Transmit_DMA(OtRcpHuart, aBuf, aBufLength);
    if (HalStatus != HAL_OK)
    {
      otPlatLog(OT_LOG_LEVEL_WARN, OT_LOG_REGION_PLATFORM,"[SPNL] Tx UART ERROR tid %d cmd %d prop 0x%x size %d", aBuf[1]&(0x0f), aBuf[2], aBuf[3], aBufLength);
      //while(1);
    }
    else
    {
      otPlatLog(OT_LOG_LEVEL_INFO, OT_LOG_REGION_PLATFORM,"[SPNL] Tx tid %d cmd %d prop 0x%x size %d", aBuf[1]&(0x0f), aBuf[2], aBuf[3], aBufLength);
      if (aBuf[3] == 0x71) //Raw frame
      {
        sprintf(spineldbgout, "%02x, %02x, %02x, %02x, %02x",aBuf[4], aBuf[5], aBuf[6], aBuf[7], aBuf[8]);
        otPlatLog(OT_LOG_LEVEL_INFO, OT_LOG_REGION_PLATFORM,spineldbgout);
      }
    }
  }

  return OT_ERROR_NONE;
}


static void otUart_ErrorCallback(UART_HandleTypeDef *huart)
{
  HAL_StatusTypeDef HalStatus;
  otPlatLog(OT_LOG_LEVEL_WARN, OT_LOG_REGION_PLATFORM,"[SPNL] RX UART ERROR Callback %d", huart->ErrorCode);
  
  /*  Trying restart Uart RX */
  HalStatus = HAL_UARTEx_ReceiveToIdle_DMA(OtRcpHuart, SpinelRxQueue.Frame[SpinelRxQueue.InIdx].SpinelRxBuffer, BUFFER_SIZE);
  __HAL_DMA_DISABLE_IT(OtRcpHuart->hdmarx, DMA_IT_HT); //Disable Half Transfer IT (not used but always set by HAL_UARTEx_ReceiveToIdle_DMA)
  if (HalStatus != HAL_OK)
  {
    otPlatLog(OT_LOG_LEVEL_WARN, OT_LOG_REGION_PLATFORM,"[SPNL] Start RX UART ERROR");
  }
}

static void otUart_TxCpltCallback(UART_HandleTypeDef *huart)
{
  UNUSED(huart);
  /* Prevent OT stack : Uart Tx RCP done */
  otPlatUartSendDone();
}


static void otUart_RxEvntCpltCallback(UART_HandleTypeDef *huart, uint16_t size)
{
  UNUSED(huart);
  HAL_StatusTypeDef HalStatus;
  bool hdlc_frame;
  uint8_t* Ptr_RxBuffer = SpinelRxQueue.Frame[SpinelRxQueue.InIdx].SpinelRxBuffer;
  /* In case spinel packet is split SpinelRxBuffer has already been filled in part
  SpinelRxBuffer size is not equal to 0, set tmp_idx to take into account */
  uint8_t tmp_idx = SpinelRxQueue.Frame[SpinelRxQueue.InIdx].size;

  /* Check is frame is compliant with HDLC formart (spinel encapsulated with HDLC lite) */
  hdlc_frame = ((Ptr_RxBuffer[0] == HDLC_FANION)&&(Ptr_RxBuffer[size+tmp_idx-1] == HDLC_FANION)) ? TRUE : FALSE;

  tmp_idx = 0; /* Reset to 0 */

  if (hdlc_frame == TRUE)
  {
    /* In this case we got complete spinel frame, send it to upper layer in task
    to resume Uart RX activity ASAP */
    SpinelRxQueue.Frame[SpinelRxQueue.InIdx].size += size;
    SpinelRxQueue.InIdx++;
    SpinelRxQueue.InIdx = SpinelRxQueue.InIdx%SPIN_BUFFER_NB;
    APP_THREAD_ScheduleRcpSpinelRx();
  }
  else
  {
    /* check if first byte is HDLC_FANION*/
    if (SpinelRxQueue.Frame[SpinelRxQueue.InIdx].SpinelRxBuffer[0] == HDLC_FANION)
    {
      if (SpinelRxQueue.Frame[SpinelRxQueue.InIdx].size + size >= BUFFER_SIZE)
      {
        /* In this case we do not receive a spinel frame over uart please check
        WBA-Host interface parameters (baudrate etc..)  */
        otPlatLog(OT_LOG_LEVEL_WARN, OT_LOG_REGION_PLATFORM,
              "UART Warning : Frame received NOT HDLC FRAME First B 0x%02X / Last B 0x%02X / size %d"
                , Ptr_RxBuffer[0], Ptr_RxBuffer[BUFFER_SIZE-1], SpinelRxQueue.Frame[SpinelRxQueue.InIdx].size + size);
        SpinelRxQueue.Frame[SpinelRxQueue.InIdx].size = 0;
      }
      else /* We receive part of HDLC frame, restart uart Rx to receive end of the frame*/
      {
        SpinelRxQueue.Frame[SpinelRxQueue.InIdx].size += size;
        tmp_idx = SpinelRxQueue.Frame[SpinelRxQueue.InIdx].size;
      }
    }
    else
    {
        /* In this case we do not receive a spinel frame over uart please check WBA-Host interface parameters (baudrate etc..)  */
        otPlatLog(OT_LOG_LEVEL_WARN, OT_LOG_REGION_PLATFORM,
              "UART Warning : Frame received NOT HDLC FRAME First B 0x%02X / Last B 0x%02X / size %d"
                , Ptr_RxBuffer[0], Ptr_RxBuffer[size-1], SpinelRxQueue.Frame[SpinelRxQueue.InIdx].size + size);
    }
  }

  /* Restart Uart Rx for next spinel frame */
  HalStatus = HAL_UARTEx_ReceiveToIdle_DMA(OtRcpHuart, (uint8_t*)(SpinelRxQueue.Frame[SpinelRxQueue.InIdx].SpinelRxBuffer + tmp_idx), (BUFFER_SIZE-tmp_idx));
  __HAL_DMA_DISABLE_IT(OtRcpHuart->hdmarx, DMA_IT_HT); //Disable Half Transfer IT (not used but always set by HAL_UARTEx_ReceiveToIdle_DMA)
  if (HalStatus != HAL_OK)
  {
    otPlatLog(OT_LOG_LEVEL_WARN, OT_LOG_REGION_PLATFORM,"[SPNL] Start RX UART ERROR %d", HalStatus);
  }
}


void arcRcpSpinelRx(void)
{
    uint8_t* ptr_buffer = SpinelRxQueue.Frame[SpinelRxQueue.OutIdx].SpinelRxBuffer;

    otPlatUartReceived(ptr_buffer, SpinelRxQueue.Frame[SpinelRxQueue.OutIdx].size);
    otPlatLog(OT_LOG_LEVEL_INFO, OT_LOG_REGION_PLATFORM, "[SPNL] Rx tid %d cmd %d prop 0x%x size %d",
              ptr_buffer[1]&(0x0f),ptr_buffer[2], ptr_buffer[3], SpinelRxQueue.Frame[SpinelRxQueue.OutIdx].size);
    if (ptr_buffer[3] == 0x71) /*  RAW FRAME */
    {
      /* print first byte of Raw frame received */
      sprintf(spineldbgout, "%02x, %02x, %02x, %02x, %02x",ptr_buffer[4], ptr_buffer[5], ptr_buffer[6], ptr_buffer[7], ptr_buffer[8]);
      otPlatLog(OT_LOG_LEVEL_INFO, OT_LOG_REGION_PLATFORM, spineldbgout);
    }

    SpinelRxQueue.Frame[SpinelRxQueue.OutIdx].size = 0;
    SpinelRxQueue.OutIdx++;
    SpinelRxQueue.OutIdx = SpinelRxQueue.OutIdx%SPIN_BUFFER_NB;
}

#endif /* OPENTHREAD_RCP */