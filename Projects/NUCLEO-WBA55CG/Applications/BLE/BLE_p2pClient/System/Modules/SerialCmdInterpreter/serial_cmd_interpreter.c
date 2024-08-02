/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    serial_cmd_interpreter.c
  * @author  MCD Application Team
  * @brief   Source file for the serial commands interpreter module.
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
#include "log_module.h"
#include "app_conf.h"
#include "stm32_adv_trace.h"
#include "serial_cmd_interpreter.h"

/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN PI */
#include "stm32wbaxx_nucleo.h"

/* USER CODE END PI */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/
#define RX_BUFF_SIZE       (256U)

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static uint8_t RxBuffer[RX_BUFF_SIZE];
static uint16_t indexRxBuffer = 0;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/
/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private functions prototypes ----------------------------------------------*/
static void UART_Rx_Callback(uint8_t *PData, uint16_t Size, uint8_t Error);
static void Uart_Cmd_Execute(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */
extern EXTI_HandleTypeDef hpb_exti[BUTTONn];

/* USER CODE END EV */

/* Functions Definition ------------------------------------------------------*/
void Serial_CMD_Interpreter_Init(void)
{
  /* Init Communication reception. Need that Log/Traces are activated */
  UTIL_ADV_TRACE_StartRxProcess(UART_Rx_Callback);
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/* Private functions definition ----------------------------------------------*/
static void UART_Rx_Callback(uint8_t *PData, uint16_t Size, uint8_t Error)
{
  /* USER CODE BEGIN UART_Rx_Callback_0 */

  /* USER CODE END UART_Rx_Callback_0 */

  /* Filling buffer and wait for '\r' charactere to execute actions */
  if (indexRxBuffer < RX_BUFF_SIZE)
  {
    if (*PData == '\r')
    {
      Uart_Cmd_Execute();

      /* Clear receive buffer and character counter*/
      indexRxBuffer = 0;
      memset( &RxBuffer[0], 0, RX_BUFF_SIZE );
    }
    else
    {
      if ( ( *PData == '\n' ) && ( indexRxBuffer == 0 ) )
      {
        /* discard this first charactere if it's a delimiter  */
      }
      else
      {
        RxBuffer[indexRxBuffer++] = *PData;
      }
    }
  }
  else
  {
    indexRxBuffer = 0;
    memset(&RxBuffer[0], 0, RX_BUFF_SIZE);
  }
  return;

  /* USER CODE BEGIN UART_Rx_Callback_1 */

  /* USER CODE END UART_Rx_Callback_1 */
}

static void Uart_Cmd_Execute(void)
{
  /*
    This function is in charge of interpreting the received data.
    The data are located within the RxBuffer variable.

    Here is an example of how to use them.
    In this simple case, we'll generate a SW IT on GPIO14 if the received data is a string "TEST".
    Add the following code into the user code section :

    // Extended line handler on which we will generate the IT
    EXTI_HandleTypeDef exti_handle;

    // Check RxBuffer's content to know if we're matching our case
    if(strcmp((char const*)RxBuffer, "TEST") == 0)
    {
      LOG_INFO_APP("TEST has been received in Uart_Cmd_Execute.\n");
      exti_handle.Line = EXTI_LINE_14;
      HAL_EXTI_GenerateSWI(&exti_handle);
    }
  */

  /* USER CODE BEGIN Uart_Cmd_Execute */
  Button_TypeDef      eButton;

  /* Parse received frame */
  if ( strcmp((char const*)RxBuffer, "SW1") == 0 )
  {
    eButton = B1;
  }
  else if ( strcmp( (char const*)RxBuffer, "SW2" ) == 0 )
  {
    eButton = B2;
  }
  else if ( strcmp( (char const*)RxBuffer, "SW3" ) == 0 )
  {
    eButton = B3;
  }
  else
  {
    LOG_INFO_APP( "NOT RECOGNIZED COMMAND : %s\n", RxBuffer );
    return;
  }

  /* Launch SW Command */
  LOG_INFO_APP( "%s pressed by Command.\n", RxBuffer );
  BSP_PB_Callback( eButton );

  /* USER CODE END Uart_Cmd_Execute */
}

/* USER CODE BEGIN PFD */

/* USER CODE END PFD */

