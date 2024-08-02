/**
  ******************************************************************************
  * @file    app_cli_runner.c
  * @author  MCD Application Team
  * @brief   Entry point for the CLI Runner
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2023 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include "main.h"

#include "app_cli_runner.h"
#include "ll_sys_startup.h"
#include "stm32_seq.h"
#include "stm32wbaxx_hal_uart.h"
#include "st_mac_802_15_4_raw_svc.h"

uint8_t cli_cmd_process = 0;

/* Private typedef -----------------------------------------------------------*/
// CLI commands list
enum {
  CMD_SET_CHANNEL,
  CMD_SET_POWER,
  CMD_TX_START_CONTINUOUS,
  CMD_TX_STOP_CONTINUOUS,
  CMD_TX_START,
  CMD_TX_STOP,
  CMD_RX_START,
  CMD_RX_STOP,
  CMD_CW_START,
  CMD_CW_STOP,
  CMD_GET_CCA,
  CMD_GET_ED,
  CMD_GET_LQI,
  CMD_GET_RSSI,
  CMD_SET_CCA_THRESHOLD,
  CMD_GET_CCA_THRESHOLD,
  // common to all protocols (even if content changes */
  CMD_HELP,
  CMD_VERSION,
  CMD_INFO,
  CMD_EXAMPLE,
};

/* Private define ------------------------------------------------------------*/
#define PLACE_IN_SECTION( __x__ )  __attribute__((section (__x__)))
#define PHY_CLI_UART_BUFFER_SIZE     650U

#define PHY_CLI_FRAME_BUFFER_NB_FRAME 512
// max frame size is 127 for 802.15.4
#define PHY_CLI_MAX_FRAME_SIZE 127
// each frame is prefixed by its size that is one byte, so +1
#define PHY_CLI_FRAME_BUFFER_SIZE (PHY_CLI_FRAME_BUFFER_NB_FRAME * (PHY_CLI_MAX_FRAME_SIZE + 1))
// arbitrary size, must be edefined to a relevant value for other protocol
#define PHY_CLI_FRAME_SIZE 127
// each frame is prefixed by its size that is one byte, so +1
#define PHY_CLI_FRAME_BUFFER_SIZE (PHY_CLI_FRAME_BUFFER_NB_FRAME * (PHY_CLI_MAX_FRAME_SIZE + 1))
#define PHY_CLI_FRAME_BUFFER_INCR_IDX(i) {i = (i == (PHY_CLI_FRAME_BUFFER_SIZE - 1)) ? 0 : i+1 ;}

#define RTT_COLOR_CODE_DEFAULT "\x1b[0m"
#define RTT_COLOR_CODE_RED     "\x1b[0;91m"
#define RTT_COLOR_CODE_GREEN   "\x1b[0;92m"
#define RTT_COLOR_CODE_YELLOW  "\x1b[0;93m"
#define RTT_COLOR_CODE_CYAN    "\x1b[0;96m"

/* Private macro -------------------------------------------------------------*/

/* External variables --------------------------------------------------------*/
extern uint8_t phy_transmitPacket[];
extern UART_HandleTypeDef huart1;

/* Public variables ----------------------------------------------------------*/
uint8_t g_phy_cli_frame_buffer[PHY_CLI_FRAME_BUFFER_SIZE];
uint32_t g_phy_cli_frame_rd_idx = 0;   // index of the next byte to be read in the buffer
uint32_t g_phy_cli_frame_wr_idx = 0;   // index of the next byte to be written in the buffer
uint32_t g_phy_cli_frame_to_print = 0; // number of frames to print (in the buffer)
uint32_t g_phy_cli_frame_idx = 0;      // num of frame since last RX_start


/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef *cli_uart;
uint8_t cliUartRX;
volatile uint8_t is_rx_pending = FALSE;
volatile uint8_t cliUart_DMA_TX_ongoing = FALSE;
uint8_t verifRx;

static cliCmd_t SetChannel          = { "Set_channel", 1, 1, {{0, "-"}}};
static cliCmd_t SetPower            = { "Set_power", 1, 1, {{0, "-"}}};
static cliCmd_t TX_start_continuous = { "TX_start_continuous", 0, 0, {{0, "-"}}};
static cliCmd_t TX_stop_continuous  = { "TX_stop_continuous", 0, 0, {{0, "-"}}};
static cliCmd_t TX_start            = { "TX_start", 1, 5, {{0, "-"}}};
static cliCmd_t TX_stop             = { "TX_stop", 0, 0, {{0, "-"}}};
static cliCmd_t RX_start            = { "RX_start", 0, 2, {{0, "-"}}};
static cliCmd_t RX_stop             = { "RX_stop", 0, 0, {{0, "-"}}};
static cliCmd_t CW_start            = { "CW_start", 1, 1, {{0, "-"}}};
static cliCmd_t CW_stop             = { "CW_stop", 0, 0, {{0, "-"}}};
static cliCmd_t Get_CCA             = { "RX_get_CCA", 0, 0, {{0, "-"}}};
static cliCmd_t Get_ED              = { "RX_get_ED", 0, 0, {{0, "-"}}};
static cliCmd_t Get_LQI             = { "RX_get_LQI", 0, 0, {{0, "-"}}};
static cliCmd_t Get_RSSI            = { "RX_get_RSSI", 0, 0, {{0, "-"}}};
static cliCmd_t Set_CCA_threshold   = { "Set_CCA_threshold", 1, 1, {{0, "-"}}};
static cliCmd_t Get_CCA_threshold   = { "Get_CCA_threshold", 0, 0, {{0, "-"}}};
static cliCmd_t Help                = { "Help", 0, 0, {{0, "-"}}};
static cliCmd_t Version             = { "Version", 0, 0, {{0, "-"}}};
static cliCmd_t Info                = { "Info", 0, 0, {{0, "-"}}};
static cliCmd_t Example             = { "Example", 0, 0, {{0, "-"}}};

static const cliCmd_t* const cmdList[] = {
  &SetChannel,
  &SetPower,
  &TX_start_continuous,
  &TX_stop_continuous,
  &TX_start,
  &TX_stop,
  &RX_start,
  &RX_stop,
  &CW_start,
  &CW_stop,
  &Get_CCA,
  &Get_ED,
  &Get_LQI,
  &Get_RSSI,
  &Set_CCA_threshold,
  &Get_CCA_threshold,
  &Help,
  &Version,
  &Info,
  &Example
};

/* Command archives (for debug purpose) */
#define CMD_ARCHIVE_SIZE 10 /* Number of commands in history */
typedef struct {
  int archives[CMD_ARCHIVE_SIZE];
  int index;
} cliCmdArchives;
cliCmdArchives cmds_history;

static char *cliCmdArray[10]; // Command + 9 parameters max
static char uartRxBuffer[PHY_CLI_UART_BUFFER_SIZE] = {0}; /* Store UART command */
static uint8_t uartTxBuffer[PHY_CLI_UART_BUFFER_SIZE] = {0}; /* Store UART cli answer */
static uint16_t uartRxBuffer_length;

/* Private function prototypes -----------------------------------------------*/
/* UART RX/TX */
static void app_cli_uartTransmit(uint8_t *pData, uint16_t size );

/* Callbacks for printing infos from PHY dependent CLI */
void app_phy_cli_print_tx_results_cb();
void app_phy_cli_print_rx_results_cb(); 

/* CLI Process */
static void APP_PHY_CLI_Uart_Init(void);
static int  app_phy_cli_cmdLookup(const char *cmdStr);
static void app_cli_print_result(const uint8_t result);
static inline void app_cli_printPrompt(void);
static void app_phy_cli_Runner(char *pCommand, uint16_t commandLength);
static void app_cli_Process(void);

uint32_t app_phy_cli_strtoul(char *s) {
  uint32_t r;
  char *p, *e;
  uint8_t len,b;
  b = 10;
  len = 0;
  p = s;
  while(s[len] != 0) {len += 1;}
  if (len == 0)
    return(0L);
  if ((len > 2) && (s[0] == '0') && (s[1] == 'x')) {
    p = s+2;
    b = 16;
  }
  r = strtoul( p, &e, b);
  return(r);
}

/* Callback to handle the payload from UART */
static void uartRxCpltCallback(UART_HandleTypeDef *huart)
{
  UNUSED(huart);
  HAL_UART_Receive_IT(cli_uart, &cliUartRX, 1);

  /*
   * If UART is plugged after the board is powered, spurious character are "received".
   * Detect and remove these
   */
  if ( (cliUartRX == 0) && (uartRxBuffer_length == 0) )
  {
    return;
  }

  /* If ENTER, forward the command if non-empty*/
  if ( ((cliUartRX == '\r') || (cliUartRX == '\n')) &&
     (uartRxBuffer_length != 0) )
  {
    uartRxBuffer[uartRxBuffer_length] = 0; /* Add a \0 for possible use (e.g. strlen) */
    /* Add echo */
    UTIL_SEQ_SetTask( 1U << CFG_TASK_PHY_CLI_PROCESS, CFG_SCH_PHY_CLI_PROCESS);
  }
  else
  {
    /* Add char to current buffer (only if enough room) */
    if (uartRxBuffer_length < PHY_CLI_UART_BUFFER_SIZE)
    {
      uartRxBuffer[uartRxBuffer_length++] = cliUartRX;
    }
  }
}

static void uartTxCpltCallback(UART_HandleTypeDef *huart)
{
  cliUart_DMA_TX_ongoing = FALSE;
  UNUSED(huart);
}

static void APP_PHY_CLI_Uart_Init(void)
{
  /*
   * Note: DMA is not required, but strongly advised to reduce as much CPU occupation
   *       by UART transmission.
   */
  cli_uart = &huart1;

  uartRxBuffer_length = 0U;

  /* register callbacks */
  cli_uart->TxCpltCallback = uartTxCpltCallback;
  cli_uart->RxCpltCallback = uartRxCpltCallback;
  HAL_UART_Receive_IT(cli_uart, &cliUartRX, 1);
}

/**
 * @brief  Entrypoint to the whole application
 * @param  None
 *
 * @retval None
 */
void APP_PHY_CLI_Init()
{
  APP_PHY_CLI_Uart_Init();
  app_cli_ex_init();
  
  cmds_history.index = -1;
  for (uint8_t i = 0; i < CMD_ARCHIVE_SIZE; i++)
  {
    cmds_history.archives[i] = 0;
  }

  // register callbacks

  // ED cb is always a direct callback

  /* Register tasks */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_PHY_CLI_PROCESS, UTIL_SEQ_RFU, app_cli_Process);

  ll_sys_thread_init();

  app_cli_print("=====================\r\n");
  app_cli_print(CLI_NAME);
  app_cli_print("=====================\r\n");
  app_cli_printPrompt();
}

//####################################
//
// CLI Print help and example
//
//####################################
static void app_phy_cli_help(void)
{
  app_cli_print("    %s\r\n      Print this message\r\n", Help.cmd);
  app_cli_print("    %s\r\n      Get the version\r\n", Version.cmd);
  app_cli_print("    %s <channel>\r\n      Set channel\r\n", SetChannel.cmd);
  app_cli_print("    %s <power>\r\n      Set radio power in dbm\r\n", SetPower.cmd);
  app_cli_print("    %s <threshold> \r\n      Set CCA threshold in dbm\r\n", Set_CCA_threshold.cmd);
  app_cli_print("    %s\r\n      Return current CCA threshold in dbm\r\n", Get_CCA_threshold.cmd);
  app_cli_print("    %s <length>,<header bytes>,[<data bytes>]* [<nb_frame>],[<delay_packets>],[<Stop if error>]\r\n      Send a frame\r\n", TX_start.cmd);
  app_cli_print("    %s\r\n      Stop an ongoing transmission, if any\r\n", TX_stop.cmd);
  app_cli_print("    %s\r\n      Start a continuous transmission \r\n", TX_start_continuous.cmd);
  app_cli_print("    %s\r\n      Stop a continuous transmission \r\n", TX_stop_continuous.cmd);
  app_cli_print("    %s <duration>\r\n      Radio in RX mode\r\n", RX_start.cmd);
  app_cli_print("    %s\r\n      Stop Radio in RX mode\r\n", RX_stop.cmd);
  app_cli_print("    %s <channel>\r\n      Start a continuous wave for a channel\r\n", CW_start.cmd);
  app_cli_print("    %s\r\n      Stop the continuous wave\r\n", CW_stop.cmd);
  app_cli_print("    %s\r\n      Print examples\r\n", Example.cmd);
  app_cli_print("    %s\r\n      Get the Clear Channel Assessment on current channel\r\n", Get_CCA.cmd);
  app_cli_print("    %s\r\n      Get the Energy Detection on current channel\r\n", Get_ED.cmd);
}

static void app_phy_cli_example(void)
{
  app_cli_print("Not yet implemented\r\n");
#if 0
#define PHY_CLI_PRINT_EXAMPLE(...) PHY_CLI_PRINT(__VA_ARGS__)

  PHY_CLI_PRINT_EXAMPLE("\n ==> To get Version and create Connection with CubeMonitorRF\r\n");
  PHY_CLI_PRINT_EXAMPLE("cli > %s\r\n", Version.cmd);
  PHY_CLI_PRINT_EXAMPLE("\n ==> To set the Channel used in TX and RX between [11:26]\r\n");
  PHY_CLI_PRINT_EXAMPLE("cli > %s 11\r\n", SetChannel.cmd);
  PHY_CLI_PRINT_EXAMPLE("\n ==> To set the Power in dbm used in TX [-21:6]r\n");
  PHY_CLI_PRINT_EXAMPLE("cli > %s -21\r\n", SetPower.cmd);
  PHY_CLI_PRINT_EXAMPLE("\n ==> To start a transmission (TX)\r\n");
  PHY_CLI_PRINT_EXAMPLE("cli > %s <param(s)>\r\n",TX_start.cmd);
  HAL_Delay(1);
  PHY_CLI_PRINT_EXAMPLE("Command transmitting a MAC frame\r\n");
  HAL_Delay(1);
  PHY_CLI_PRINT_EXAMPLE("  @param  1 The frame to be transmitted without CRC (automatically calculated)\r\n");
  HAL_Delay(1);
  PHY_CLI_PRINT_EXAMPLE("  @param  2 [Optional] The number of time the frame is sent (default = 1)\r\n");
  HAL_Delay(1);
  PHY_CLI_PRINT_EXAMPLE("  @param  3 [Optional] The delay between frames in ms (default = 0)\r\n");
  HAL_Delay(1);
  PHY_CLI_PRINT_EXAMPLE("  @param  4 [Optional] A boolean to stop transmission when TX result is not 0x00 (default = 0)\r\n");
  HAL_Delay(1);
  PHY_CLI_PRINT_EXAMPLE("  @return 0 if command succeed, or error code\r\n");
  HAL_Delay(1);
  PHY_CLI_PRINT_EXAMPLE("Frame is a comma separated hexadecimal string with the following format:\r\n");
  PHY_CLI_PRINT_EXAMPLE("- Byte 1:     Frame length CRC included\r\n");
  PHY_CLI_PRINT_EXAMPLE("- Byte 2-3:   MAC Frame control (FC)\r\n");
  PHY_CLI_PRINT_EXAMPLE("- Byte 4:     MAC Sequence number (SN)\r\n");
  PHY_CLI_PRINT_EXAMPLE("- Byte 5-x:   MAC data (including addressing field if used)\r\n");
  HAL_Delay(1);
  PHY_CLI_PRINT_EXAMPLE("NO ACK required:\r\n");
  PHY_CLI_PRINT_EXAMPLE("cli > %s 0x0B,0x00,0x00,0x01,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6\r\n", TX_start.cmd);
  HAL_Delay(1);
  PHY_CLI_PRINT_EXAMPLE("cli > %s 0x0B,0x01,0x00,0x01,0xFF,0xFF,0xd3,0xd4,0xd5,0xd6 4\r\n", TX_start.cmd);
  HAL_Delay(1);
  PHY_CLI_PRINT_EXAMPLE("cli > %s 0x0B,0x63,0x98,0x01,0x11,0x22,0xd3,0xd4,0xd5,0xd6 5 10\r\n", TX_start.cmd);
  HAL_Delay(1);
  PHY_CLI_PRINT_EXAMPLE("cli > %s 0x0A,0x01,0x08,0x01,0x22,0x11,0xFF,0xFF,0xFF 6 20\r\n", TX_start.cmd);
  HAL_Delay(1);
  PHY_CLI_PRINT_EXAMPLE("cli > %s 0x0B,0x01,0x80,0x01,0x11,0x22,0xd3,0xd4,0xd5,0xd6 7 30\r\n", TX_start.cmd);
  HAL_Delay(1);
  PHY_CLI_PRINT_EXAMPLE("Note: a special frame has been defined and the name `STD' can be used instead of the frame byte definition.\r\n" );
  PHY_CLI_PRINT_EXAMPLE("cli > %s STD\r\n", TX_start.cmd);
  PHY_CLI_PRINT_EXAMPLE("is equivalent to\r\n");
  PHY_CLI_PRINT_EXAMPLE("cli > %s %s\r\n", TX_start.cmd, CLI_802_15_4_STD_PACKET);
  HAL_Delay(1);
  PHY_CLI_PRINT_EXAMPLE("ACK required:\r\n");
  PHY_CLI_PRINT_EXAMPLE("cli > %s 0x0B,0x63,0x98,0x01,0x22,0x11,0xBB,0xAA,0xd5,0xd6 5 40\r\n", TX_start.cmd);
  HAL_Delay(1);
  PHY_CLI_PRINT_EXAMPLE("cli > %s 0x0A,0x21,0x08,0x01,0x22,0x11,0xFF,0xFF,0xFF 6 50 1\r\n", TX_start.cmd);
  HAL_Delay(1);
  PHY_CLI_PRINT_EXAMPLE("cli > %s 0x0B,0x21,0x08,0x01,0xFF,0xFF,0xd3,0xd4,0xd5,0xd6 7 60\r\n", TX_start.cmd);
  HAL_Delay(1);
  PHY_CLI_PRINT_EXAMPLE("Note: a special frame has been defined and the name `RACK' can be used instead of the frame byte definition.\r\n" );
  PHY_CLI_PRINT_EXAMPLE("cli > %s RACK\r\n", TX_start.cmd);
  PHY_CLI_PRINT_EXAMPLE("is equivalent to\r\n");
  PHY_CLI_PRINT_EXAMPLE("cli > %s %s\r\n", TX_start.cmd, CLI_802_15_4_RACK_PACKET);
  HAL_Delay(1);
  PHY_CLI_PRINT_EXAMPLE("\n ==> To start a reception (RX)\r\n");
  PHY_CLI_PRINT_EXAMPLE("cli > %s\r\n",RX_start.cmd);
  HAL_Delay(1);
  PHY_CLI_PRINT_EXAMPLE("Command receiving a MAC frame\r\n");
  HAL_Delay(1);
  //PHY_CLI_PRINT_EXAMPLE("- DEFAULT PAN ID :   0x%4x > 0x%2x,0x%2x,\r\n",PHY_CLI_DEFAULT_PANID, PHY_CLI_DEFAULT_PANID&0xFF, PHY_CLI_DEFAULT_PANID>>8);
  //PHY_CLI_PRINT_EXAMPLE("- DEFAULT ADDRESS:   0x%4x > 0x%2x,0x%2x,\r\n",PHY_CLI_DEFAULT_ADDRESS, PHY_CLI_DEFAULT_ADDRESS&0xFF, PHY_CLI_DEFAULT_ADDRESS>>8);
  //HAL_Delay(1);
  PHY_CLI_PRINT_EXAMPLE("\n ==> To stop a reception \r\n");
  PHY_CLI_PRINT_EXAMPLE("cli > %s\r\n",RX_stop.cmd);
  HAL_Delay(1);
  PHY_CLI_PRINT_EXAMPLE("Command stopping radio and printing received results\r\n");
  HAL_Delay(1);
  PHY_CLI_PRINT_EXAMPLE("\n ==> To set the specific TM [0:1] on signal [1:63]\r\n");
  PHY_CLI_PRINT_EXAMPLE("cli > %s 0 4\r\n", DTB_TM_select.cmd);
  PHY_CLI_PRINT_EXAMPLE("Command setting the TM 0 on signal 4 (i_zclk)\r\n");
  HAL_Delay(1);
#undef PHY_CLI_PRINT_EXAMPLE
#endif
  app_cli_printPrompt();
}

//####################################
//
// CubeMonitorRF Transmission Process (UART)
//
//####################################
void app_cli_print(const char *aFormat, ...)
{
  uint16_t ret;
  
  va_list aArguments;
  va_start(aArguments, aFormat);

  while(cliUart_DMA_TX_ongoing == TRUE);
  cliUart_DMA_TX_ongoing = TRUE;

  ret = (uint16_t)vsnprintf((char *) &uartTxBuffer[0], PHY_CLI_UART_BUFFER_SIZE
                                     , aFormat, aArguments);
  if (ret > 0)
    app_cli_uartTransmit(&uartTxBuffer[0], ret);
}

/**
 * @brief  Call the UART API to send data to UART
 *
 * @note  This API is blocking until previous transmission is done.
 *
 * @param[in] pData pointer to data
 * @param[in] size of message to send (in number of bytes)
 * @retval None
 */
static void app_cli_uartTransmit(uint8_t *pData, uint16_t size )
{
  HAL_StatusTypeDef result = HAL_UART_Transmit_DMA(cli_uart, pData, size);

  if (result != HAL_OK) {
    /* UART should return in Receiver mode */
      HAL_UART_Receive_IT(&huart1, &verifRx, 1);
  }
}

#define RX_BUFFER_MAX_SIZE_STR RX_BUFFER_MAX_SIZE << 2
static char payload_str[RX_BUFFER_MAX_SIZE_STR];
static char payload_str_buffer[10];

void app_cli_print_payload(const app_cli_single_RX_t *frame)
{
  sprintf(payload_str, "\r\n[");
  for (uint16_t i = 0U; i< frame->payload_len; i++)
  {
	  sprintf(payload_str_buffer,"0x%02X,", (unsigned int)frame->payload[i]);
	  strcat(payload_str, payload_str_buffer);
  }
  strcat(payload_str, "]");
  app_cli_print(payload_str);
}

//####################################
//
// CLI specific tasks
// These tasks are scheduled when there is an "asynchronous" result to print from the underlying CLI.
// As he printing takes too much time to be executed in the link layer task, it cannot be only a callback so it is
// implemented as a task (but this task can takes too many time to execute ? let's try again a simple callback like for ED)?
// Each task has an "schedule" associated function so that the the CLI can schedule it without knowing details about the scheduler 
//####################################

void app_cli_print_tx_results(const uint32_t frm_success, const uint32_t frm_failed,
                              const uint32_t frm_nack, const uint32_t frm_ack)
{
  app_cli_print("Nb of TX complete ..........: [%d]\r\n", frm_success);
  app_cli_print("Nb of TX complete but FAILED: [%d]\r\n", frm_failed);
  app_cli_print("Nb of ACK timeout ..........: [%d]\r\n", frm_nack);
  app_cli_print("Nb ACK seen ................: [%d]\r\n", frm_ack);
  app_cli_print("Error found ................: [NA]\r\n");
  app_cli_print("Last Tx result .............: [NA]\r\n");
  app_cli_print_result(0);
  app_cli_printPrompt();
}

void app_cli_print_rx_results(const uint16_t packets_received, const uint16_t packets_rejected,
                              const app_cli_single_RX_t *p_last_received_frame)
{
  app_cli_print("PHY pdu(s)\r\n");
  //Keep SFD for legacy
  app_cli_print("nb SFD received:[%d]\r\n", packets_received);
  app_cli_print("\r\nMAC Frame(s)\r\n");
  app_cli_print("nb packet:[%d]\r\n", packets_received);
  app_cli_print("nb rejected packet:[%d]\r\n", packets_rejected);
  app_cli_print("nb filtered packet:[N/A]\r\n");

  // Last frame received:
  if (p_last_received_frame != NULL)
  {
    app_cli_print("Last good Frame received:");
    app_cli_print_payload(p_last_received_frame);
    app_cli_print("\r\n");
    
    app_cli_print("Length:[%d]\r\n", p_last_received_frame->payload_len);

    app_cli_print("\r\nIn last good Frame received:\r\n");
    app_cli_print("RSSI:[%d]\r\n", p_last_received_frame->rssi);
    app_cli_print("LQI:[%d]\r\n", p_last_received_frame->lqi);
  } else
  {
    app_cli_print("No frame received\r\n");
    app_cli_print("RSSI:[%d]\r\nLQI:[%d]\r\n", RSSI_DEFAULT_VALUE, LQI_DEFAULT_VALUE);
  }
  app_cli_print_result(0);
  app_cli_printPrompt();
}

void app_cli_print_ed_cb(const uint8_t ed) {
  app_cli_print("ED:[%d]\r\n", ed);
  app_cli_printPrompt();
}

//####################################
//
// CLI Process
//
//####################################
static void app_cli_print_result(const uint8_t result)
{
  app_cli_print("Result:[%d]\r\n", result);
}

static inline void app_cli_printPrompt(void)
{
  app_cli_print("%s", CLI_PROMPT);
}

static int app_phy_cli_cmdLookup(const char *cmdStr) {
    for (int i = 0; i < (sizeof(cmdList)/sizeof(cmdList[0])); i++)
    {
      uint8_t cmp = strcasecmp(cmdList[i]->cmd, cmdStr);
      if (cmp <= 0)
        return i;
    }
    return 255;
}

static void app_phy_cli_Runner(char *pCommand, uint16_t commandLength)
{
  int  argIndex = 0;
  int  commandNb = 0;
  char * cmdToken, * dataPacket;
  uint8_t res, channel, pkt_size, cca_result, lqi, rx_frame_printing;
  int8_t power, cca_threshold, rssi;
  uint32_t packetNb;
  uint16_t rx_duration, tx_delay, stop_tx;

  if (commandLength == 0)
  {
    app_cli_print("Command length must not be null\r\n");
    return;
  }

  /* Fill a pointer table with addr of the different arguments of the command */
  cmdToken = strtok(pCommand, " ");
  while (cmdToken != NULL) {
    cliCmdArray[argIndex++] = cmdToken;
    cmdToken = strtok(NULL, " ");
    /* Echo command written (e.g. 'TX_start 0x0C,0x01,0x08,0x01,0x22,0x11,0xFF,0xFF,0xB5,0xB6,0xB7') */
    app_cli_print(" %s", cliCmdArray[argIndex-1]);
  }
  app_cli_print("\r\n");

  /* Check param number */
  commandNb = app_phy_cli_cmdLookup(cliCmdArray[0]);
  if ( (commandNb >= (sizeof(cmdList)/sizeof(cmdList[0]) )) ||
        ((argIndex-1) < cmdList[commandNb]->nArgsMin) ||
         ((argIndex-1) > cmdList[commandNb]->nArgsMax) ) {
    app_cli_print("Unknown command or incorrect number of param received: %s\r\n", cliCmdArray[0] );
    app_cli_printPrompt();
    return;
  }
  
  /* Save command in archives */
  cmds_history.index++;
  cmds_history.index %= CMD_ARCHIVE_SIZE;
  cmds_history.archives[cmds_history.index] = commandNb;

  switch (commandNb) {

  case CMD_HELP:
    app_phy_cli_help();
    app_cli_printPrompt();
    break;

  case CMD_EXAMPLE: 
    app_phy_cli_example();
    break;

  case CMD_VERSION:
    res = app_cli_ex_get_version();
    app_cli_print_result(res);
    app_cli_printPrompt();
    break;

  case CMD_INFO:
    res = app_cli_ex_get_info();
    app_cli_print_result(res);
    app_cli_printPrompt();
    break;

  case CMD_SET_CHANNEL:
    channel = (uint8_t)app_phy_cli_strtoul(cliCmdArray[1]);

    /* execute app_802_15_4 CLI */
    if ( (channel < 11) || (channel > 26))
    {
      app_cli_print( "Specified channel %d is out of range\r\n", channel);
      res = 1;
    } else
    {
      res = app_cli_ex_set_channel(channel);
    }
    app_cli_print_result(res);
    app_cli_printPrompt();
    break;

  case CMD_SET_POWER:
    power = (int8_t)atol(cliCmdArray[1]);
    res = app_cli_ex_set_power(power);
    app_cli_print_result(res);
    app_cli_printPrompt();
    break;

  case CMD_TX_START_CONTINUOUS:
    res = app_cli_ex_tx_continuous_start();
    if (res == 0)
    {
      app_cli_print("Tx started. Waiting `%s' cmd\r\n", TX_stop_continuous.cmd);
    }
    app_cli_print_result(res);
    app_cli_printPrompt();
    break;

  case CMD_TX_STOP_CONTINUOUS:
    res = app_cli_ex_tx_continuous_stop();
    app_cli_print_result(res);
    app_cli_printPrompt();

    break;

  case CMD_TX_START:
    dataPacket = strtok(cliCmdArray[1], ",");
    pkt_size = strtoul(dataPacket+2, NULL, 16);

    // second parameter (optional) : number of packet to be sent
    if (argIndex > 2) {
      packetNb = (uint32_t)atol(cliCmdArray[2]);
    }
    else {
      packetNb = 1;
    }

    // third parameter (optional) : delay ms between each packet
    tx_delay = (argIndex > 3) ? (uint16_t)atol(cliCmdArray[3]) : 0;

    // fourth parameter (optional) : stop TX if error
    stop_tx = (argIndex > 4) ? (uint16_t)atol(cliCmdArray[4]) : 0;

    res = app_cli_ex_start_tx((uint8_t*) dataPacket+5, pkt_size, packetNb,
                              tx_delay, stop_tx);
    /* If failure or infinite TX (packetNb = 0) print result & prompt */
    if ( (res) || (packetNb == 0)) {
      app_cli_print_result(res);
      app_cli_printPrompt();
    }
    break;

  case CMD_TX_STOP:
    res = app_cli_ex_stop_tx();
    if (res == MAC_UNEXPECTED_RADIO_STATE)
    {
      app_cli_print_result(res);
      app_cli_printPrompt();
    }
    break;

  case CMD_RX_START:
    // parameter (optional) : Dynamic frame printing
    rx_frame_printing = (argIndex > 1) ? (uint16_t)atol(cliCmdArray[1]) : 0;
    // parameter (optional) : RX duration
    rx_duration = (argIndex > 2) ? (uint16_t)atol(cliCmdArray[2]) : 0;
    
    /* execute app_802_15_4 CLI */
    res = app_cli_ex_start_rx(rx_frame_printing, rx_duration);
    is_rx_pending = TRUE;

    app_cli_print_result(res);
    /* If failure print prompt */
    app_cli_printPrompt();
    break;

  case CMD_RX_STOP: 
    app_cli_ex_stop_rx();
    /* If no RX pending, print prompt */
    if (is_rx_pending == FALSE)
    {
      app_cli_print_result(0);
      app_cli_printPrompt();
    }
    break;

  case CMD_GET_CCA:
    /* execute app_802_15_4 CLI */
    res = app_cli_ex_get_cca(&cca_result);
    if (res == 0)
      app_cli_print("ChannelIsClear:[%d]\r\n", cca_result);
    else
      app_cli_print_result(res);
    app_cli_printPrompt();
    break;  

  case CMD_SET_CCA_THRESHOLD:
    cca_threshold = (int8_t)atol(cliCmdArray[1]);
    /* execute app_802_15_4 CLI */
    res = app_cli_ex_set_cca_threshold(cca_threshold);
    /* print the result */
    app_cli_print_result(res);
    app_cli_printPrompt();
    break;

  case CMD_GET_CCA_THRESHOLD:
    res = app_cli_ex_get_cca_threshold(&cca_threshold);
    app_cli_print("CCA threshold is %d dbm\r\n", cca_threshold);
    app_cli_print_result(res);
    app_cli_printPrompt();
    break;

  case CMD_GET_ED:
    /* execute app_802_15_4 CLI */
    res= app_cli_ex_start_ed_scan();
    if (res) {
      //error, print prompt
      app_cli_print_result(res);
      app_cli_printPrompt();
    }
    /* prompt will be issued by the callback, after printing the result */
    break;

  case CMD_GET_LQI:
    res = app_cli_ex_get_lqi(&lqi);
    if (res == 0)
    {
      // LQI was available
      app_cli_print("LQI: [%d]\r\n", lqi);
    }
    app_cli_print_result(res);
    app_cli_printPrompt();
    break;    

  case CMD_GET_RSSI:
    res = app_cli_ex_get_rssi(&rssi);
    if (res == 0)
    {
      // RSSI was available
      app_cli_print("RSSI: [%d]\r\n", rssi);
    }
    app_cli_print_result(res);
    app_cli_printPrompt();
    break;    

  case CMD_CW_START:
    channel = (uint8_t)app_phy_cli_strtoul(cliCmdArray[1]);
    if ( (channel < 11) || (channel > 26))
    {
      app_cli_print( "Specified channel %d is out of range\r\n", channel);
      res = 1;
    } else
    {
      res = app_cli_ex_cw_start(channel);
    }
    app_cli_print_result(res);
    app_cli_printPrompt();   
    break;

  case CMD_CW_STOP:
    res = app_cli_ex_cw_stop();
    app_cli_print_result(res);
    app_cli_printPrompt();     
    break;
  }
}

static void app_cli_Process(void)
{
  app_phy_cli_Runner(&uartRxBuffer[0], uartRxBuffer_length);
  uartRxBuffer_length = 0U;
  cli_cmd_process = 0U;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
