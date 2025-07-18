/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * File Name          : app_thread.c
  * Description        : Thread Application.
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
#include <assert.h>
#include <stdint.h>

#include "app_conf.h"
#include "app_common.h"
#include "app_entry.h"
#include "log_module.h"
#include "app_thread.h"
#include "dbg_trace.h"
#include "stm32_rtos.h"
#include "stm32_timer.h"
#if (CFG_LPM_LEVEL != 0)
#include "stm32_lpm.h"
#endif // CFG_LPM_LEVEL
#include "common_types.h"
#include "instance.h"
#include "radio.h"
#include "platform.h"
#include "ll_sys_startup.h"
#include "event_manager.h"
#include "platform_wba.h"
#include "link.h"
#include "cli.h"
#include "coap.h"
#include "tasklet.h"
#include "thread.h"
#if (OT_CLI_USE == 1)
#include "uart.h"
#endif
#include "joiner.h"
#include "alarm.h"
#include OPENTHREAD_CONFIG_FILE

/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "udp.h"
#include "app_bsp.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/
#define C_PANID                 0xBA98U
#define C_CHANNEL_NB            16U
#define C_CCA_THRESHOLD         (-70)

/* USER CODE BEGIN PD */
#define UDP_PORT                1234
/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/**
 * This macro checks for the specified status, which is expected to commonly be successful, and branches to the local
 * label 'exit' if the status is unsuccessful.
 *
 *  @param[in]  aStatus     A scalar status to be evaluated against zero (0).
 *
 */
#define SuccessOrExit(aStatus) \
    do                         \
    {                          \
        if ((aStatus) != 0)    \
        {                      \
            goto exit;         \
        }                      \
    } while (false)

/**
 * This macro checks for the specified condition, which is expected to commonly be true, and both executes @a ... and
 * branches to the local label 'exit' if the condition is false.
 *
 *  @param[in]  aCondition  A Boolean expression to be evaluated.
 *  @param[in]  ...         An expression or block to execute when the assertion fails.
 *
 */
#define VerifyOrExit(aCondition, ...) \
    do                                \
    {                                 \
        if (!(aCondition))            \
        {                             \
            __VA_ARGS__;              \
            goto exit;                \
        }                             \
    } while (false)
/* USER CODE END PM */

/* Private function prototypes -----------------------------------------------*/
static void APP_THREAD_DeviceConfig(void);
static void APP_THREAD_StateNotif(uint32_t NotifFlags, void *pContext);
static void APP_THREAD_TraceError(const char * pMess, uint32_t ErrCode);

#if (OT_CLI_USE == 1)
static void APP_THREAD_CliInit(otInstance *aInstance);
static void APP_THREAD_ProcessUart(void);
#endif // OT_CLI_USE

static void APP_THREAD_PersistenceStartup(void);

/* USER CODE BEGIN PFP */
static otError UdpBind(uint16_t aPort);
static void HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
static void APP_THREAD_SW1_Process(void); /* UdpSend */
static void APP_THREAD_SW2_Process(void); /* UdpSend */
static otError UdpSend(void);
/* USER CODE END PFP */

/* Private variables -----------------------------------------------*/
static otInstance * PtOpenThreadInstance;

/* USER CODE BEGIN PV */

otUdpSocket mSocket;
uint8_t udpBufffer[256] = "HELLO UDP!";

uint16_t udpPort = UDP_PORT;

/* USER CODE END PV */

/* Functions Definition ------------------------------------------------------*/

void ProcessAlarm(void)
{
  arcAlarmProcess(PtOpenThreadInstance);
}

void ProcessUsAlarm(void)
{
  arcUsAlarmProcess(PtOpenThreadInstance);
}

void ProcessTasklets(void)
{
  if (otTaskletsArePending(PtOpenThreadInstance) == TRUE)
  {
    UTIL_SEQ_SetTask(1U << CFG_TASK_OT_TASKLETS, TASK_PRIO_TASKLETS);
  }
}

/**
 * @brief  ProcessOpenThreadTasklets.
 * @param  None
 * @param  None
 * @retval None
 */
void ProcessOpenThreadTasklets(void)
{
  /* wakeUp the system */
  //ll_sys_radio_hclk_ctrl_req(LL_SYS_RADIO_HCLK_LL_BG, LL_SYS_RADIO_HCLK_ON);
  //ll_sys_dp_slp_exit();

  /* process the tasklet */
  otTaskletsProcess(PtOpenThreadInstance);

  /* put the IP802_15_4 back to sleep mode */
  //ll_sys_radio_hclk_ctrl_req(LL_SYS_RADIO_HCLK_LL_BG, LL_SYS_RADIO_HCLK_OFF);

  /* reschedule the tasklets if any */
  ProcessTasklets();
}

/**
 * OpenThread calls this function when the tasklet queue transitions from empty to non-empty.
 *
 * @param[in] aInstance A pointer to an OpenThread instance.
 */
void otTaskletsSignalPending(otInstance *aInstance)
{
  UTIL_SEQ_SetTask(1U << CFG_TASK_OT_TASKLETS, TASK_PRIO_TASKLETS);
}

void APP_THREAD_ScheduleAlarm(void)
{
  UTIL_SEQ_SetTask(1U << CFG_TASK_OT_ALARM, TASK_PRIO_ALARM);
}

void APP_THREAD_ScheduleUsAlarm(void)
{
  UTIL_SEQ_SetTask(1U << CFG_TASK_OT_US_ALARM, TASK_PRIO_US_ALARM);
}

static void APP_THREAD_AlarmsInit(void)
{
  UTIL_SEQ_RegTask(1U << CFG_TASK_OT_ALARM, UTIL_SEQ_RFU, ProcessAlarm);
  UTIL_SEQ_RegTask(1U << CFG_TASK_OT_US_ALARM, UTIL_SEQ_RFU, ProcessUsAlarm);

  /* Run first time */
  UTIL_SEQ_SetTask(1U << CFG_TASK_OT_ALARM, TASK_PRIO_ALARM);
}

static void APP_THREAD_TaskletsInit(void)
{
  UTIL_SEQ_RegTask(1U << CFG_TASK_OT_TASKLETS, UTIL_SEQ_RFU, ProcessOpenThreadTasklets);
}

/**
 *
 */
void Thread_Init(void)
{
#if OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
  size_t otInstanceBufferLength = 0;
  uint8_t *otInstanceBuffer = NULL;
#endif // OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE

  APP_THREAD_PersistenceStartup();

  otSysInit(0, NULL);

#if OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
  // Call to query the buffer size
  (void)otInstanceInit(NULL, &otInstanceBufferLength);

  // Call to allocate the buffer
  otInstanceBuffer = (uint8_t *)malloc(otInstanceBufferLength);
  assert(otInstanceBuffer);

  // Initialize OpenThread with the buffer
  PtOpenThreadInstance = otInstanceInit(otInstanceBuffer, &otInstanceBufferLength);
#else // OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
  PtOpenThreadInstance = otInstanceInitSingle();
#endif // OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE

  assert(PtOpenThreadInstance);

#if (OT_CLI_USE == 1)
  APP_THREAD_CliInit(PtOpenThreadInstance);
#endif // OT_CLI_USE
  otDispatch_tbl_init(PtOpenThreadInstance);

  /* Register tasks */
  APP_THREAD_AlarmsInit();
  APP_THREAD_TaskletsInit();

  ll_sys_thread_init();

  /* USER CODE BEGIN INIT TASKS */

  /* USER CODE END INIT TASKS */
}

/**
 * @brief Thread initialization.
 * @param  None
 * @retval None
 */
static void APP_THREAD_DeviceConfig(void)
{
  otError error = OT_ERROR_NONE;
  otNetworkKey networkKey = {{0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00}};

  error = otSetStateChangedCallback(PtOpenThreadInstance, APP_THREAD_StateNotif, NULL);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_SET_STATE_CB,error);
  }

  error = otPlatRadioSetCcaEnergyDetectThreshold(PtOpenThreadInstance, C_CCA_THRESHOLD);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_SET_THRESHOLD,error);
  }

  error = otLinkSetChannel(PtOpenThreadInstance, C_CHANNEL_NB);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_SET_CHANNEL,error);
  }

  error = otLinkSetPanId(PtOpenThreadInstance, C_PANID);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_SET_PANID,error);
  }

  error = otThreadSetNetworkKey(PtOpenThreadInstance, &networkKey);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_SET_NETWORK_KEY,error);
  }

  otPlatRadioEnableSrcMatch(PtOpenThreadInstance, true);

  error = otIp6SetEnabled(PtOpenThreadInstance, true);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_IPV6_ENABLE,error);
  }
  error = otThreadSetEnabled(PtOpenThreadInstance, true);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_START,error);
  }
  /* USER CODE BEGIN DEVICECONFIG */
  /* Initialize socket */
  memset(&mSocket, 0, sizeof(mSocket));

  /* Open socket */
  (void)otUdpOpen(PtOpenThreadInstance, &mSocket, HandleUdpReceive, NULL);
  (void)UdpBind(UDP_PORT);

  /* USER CODE END DEVICECONFIG */
}

void APP_THREAD_Init( void )
{
#if (CFG_LPM_LEVEL != 0)
  UTIL_LPM_SetStopMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);
  UTIL_LPM_SetOffMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);
#endif // CFG_LPM_LEVEL

  Thread_Init();

  APP_THREAD_DeviceConfig();
}

/**
 * @brief  Warn the user that an error has occurred.
 *
 * @param  pMess  : Message associated to the error.
 * @param  ErrCode: Error code associated to the module (OpenThread or other module if any)
 * @retval None
 */
static void APP_THREAD_TraceError(const char * pMess, uint32_t ErrCode)
{
  /* USER CODE BEGIN TRACE_ERROR */
  //APP_DBG("**** Fatal error = %s (Err = %d)", pMess, ErrCode);
  while (1)
  {
  }
  /* USER CODE END TRACE_ERROR */
}

/**
 * @brief  Trace the error or the warning reported.
 * @param  ErrId :
 * @param  ErrCode
 * @retval None
 */
void APP_THREAD_Error(uint32_t ErrId, uint32_t ErrCode)
{
  /* USER CODE BEGIN APP_THREAD_Error_1 */

  /* USER CODE END APP_THREAD_Error_1 */

  switch(ErrId)
  {
    case ERR_THREAD_SET_STATE_CB :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_SET_STATE_CB ",ErrCode);
        break;

    case ERR_THREAD_SET_CHANNEL :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_SET_CHANNEL ",ErrCode);
        break;

    case ERR_THREAD_SET_PANID :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_SET_PANID ",ErrCode);
        break;

    case ERR_THREAD_IPV6_ENABLE :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_IPV6_ENABLE ",ErrCode);
        break;

    case ERR_THREAD_START :
        APP_THREAD_TraceError("ERROR: ERR_THREAD_START ", ErrCode);
        break;

    case ERR_THREAD_ERASE_PERSISTENT_INFO :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_ERASE_PERSISTENT_INFO ",ErrCode);
        break;

    case ERR_THREAD_SET_NETWORK_KEY :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_SET_NETWORK_KEY ",ErrCode);
        break;

    case ERR_THREAD_CHECK_WIRELESS :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_CHECK_WIRELESS ",ErrCode);
        break;

    case ERR_THREAD_SET_THRESHOLD:
        APP_THREAD_TraceError("ERROR : ERR_THREAD_SET_THRESHOLD", ErrCode);
        break;

    /* USER CODE BEGIN APP_THREAD_Error_2 */

    /* USER CODE END APP_THREAD_Error_2 */
    default :
        APP_THREAD_TraceError("ERROR Unknown ", 0);
        break;
  }
}

/**
 * @brief Thread notification when the state changes.
 * @param  aFlags  : Define the item that has been modified
 *         aContext: Context
 *
 * @retval None
 */
static void APP_THREAD_StateNotif(uint32_t NotifFlags, void *pContext)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(pContext);

  /* USER CODE BEGIN APP_THREAD_STATENOTIF */
  
  /* USER CODE END APP_THREAD_STATENOTIF */

  if ((NotifFlags & (uint32_t)OT_CHANGED_THREAD_ROLE) == (uint32_t)OT_CHANGED_THREAD_ROLE)
  {
    switch (otThreadGetDeviceRole(PtOpenThreadInstance))
    {
      case OT_DEVICE_ROLE_DISABLED:
          /* USER CODE BEGIN OT_DEVICE_ROLE_DISABLED */
      APP_LED_OFF(LD2);
      APP_LED_OFF(LD3);
          /* USER CODE END OT_DEVICE_ROLE_DISABLED */
          break;

      case OT_DEVICE_ROLE_DETACHED:
          /* USER CODE BEGIN OT_DEVICE_ROLE_DETACHED */
      APP_LED_OFF(LD2);
      APP_LED_OFF(LD3);
          /* USER CODE END OT_DEVICE_ROLE_DETACHED */
          break;

      case OT_DEVICE_ROLE_CHILD:
          /* USER CODE BEGIN OT_DEVICE_ROLE_CHILD */
      APP_LED_OFF(LD2);
      APP_LED_ON(LD3);
          /* USER CODE END OT_DEVICE_ROLE_CHILD */
          break;

      case OT_DEVICE_ROLE_ROUTER :
          /* USER CODE BEGIN OT_DEVICE_ROLE_ROUTER */
      APP_LED_OFF(LD2);
      APP_LED_ON(LD3);
          /* USER CODE END OT_DEVICE_ROLE_ROUTER */
          break;

      case OT_DEVICE_ROLE_LEADER :
          /* USER CODE BEGIN OT_DEVICE_ROLE_LEADER */
      APP_LED_ON(LD2);
      APP_LED_OFF(LD3);
          /* USER CODE END OT_DEVICE_ROLE_LEADER */
          break;

      default:
          /* USER CODE BEGIN DEFAULT */
      APP_LED_OFF(LD2);
      APP_LED_OFF(LD3);
          /* USER CODE END DEFAULT */
          break;
    }
  }
}

#if (OT_CLI_USE == 1)
/* OT CLI UART functions */
static void APP_THREAD_ProcessUart(void)
{
  arcUartProcess();
}

void APP_THREAD_ScheduleUART(void)
{
  UTIL_SEQ_SetTask(1U << CFG_TASK_OT_UART, TASK_PRIO_UART);
}

static void APP_THREAD_CliInit(otInstance *aInstance)
{
  UTIL_SEQ_RegTask(1 << CFG_TASK_OT_UART, UTIL_SEQ_RFU, APP_THREAD_ProcessUart);
  /* run first time */
  UTIL_SEQ_SetTask(1U << CFG_TASK_OT_UART, TASK_PRIO_UART);

  (void)otPlatUartEnable();
  otCliInit(aInstance, CliUartOutput, aInstance);
}
#endif /* OT_CLI_USE */

/**
 * @brief  Thread persistence startup
 * @param  None
 * @retval None
 */
static void APP_THREAD_PersistenceStartup(void)
{
  /* USER CODE BEGIN APP_THREAD_PersistenceStartup */

  /* USER CODE END APP_THREAD_PersistenceStartup */
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */
static uint16_t Swap16(uint16_t v)
{
    return (((v & 0x00ffU) << 8) & 0xff00) | (((v & 0xff00U) >> 8) & 0x00ff);
}

static uint16_t HostSwap16(uint16_t v)
{
    return Swap16(v);
}

static otError UdpBind(uint16_t aPort)
{
    otError    error;
    otSockAddr sockaddr;
    otNetifIdentifier netif = OT_NETIF_THREAD_HOST;

    memset(&sockaddr, 0, sizeof(sockaddr));

    /* "::" specifies the IPv6 Unspecified Address */
    error = otIp6AddressFromString("::", &sockaddr.mAddress);
    SuccessOrExit(error);

    sockaddr.mPort    = aPort;

    error = otUdpBind(PtOpenThreadInstance, &mSocket, &sockaddr, netif);

exit:
    return error;
}

static void HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
  int     length;
  static  uint32_t l_receive_count = 0;
  uint8_t udpBuffferReceived[256];

  LOG_INFO_APP("\r\nl_receive_count = %d", l_receive_count++);
  LOG_INFO_APP("\r\nReceived %d bytes from ", otMessageGetLength(aMessage) - otMessageGetOffset(aMessage));
  LOG_INFO_APP(
         "%x:%x:%x:%x:%x:%x:%x:%x %d ", HostSwap16(aMessageInfo->mPeerAddr.mFields.m16[0]),
         HostSwap16(aMessageInfo->mPeerAddr.mFields.m16[1]), HostSwap16(aMessageInfo->mPeerAddr.mFields.m16[2]),
         HostSwap16(aMessageInfo->mPeerAddr.mFields.m16[3]), HostSwap16(aMessageInfo->mPeerAddr.mFields.m16[4]),
         HostSwap16(aMessageInfo->mPeerAddr.mFields.m16[5]), HostSwap16(aMessageInfo->mPeerAddr.mFields.m16[6]),
         HostSwap16(aMessageInfo->mPeerAddr.mFields.m16[7]), aMessageInfo->mPeerPort);

  length = otMessageRead(aMessage, otMessageGetOffset(aMessage), udpBuffferReceived, sizeof(udpBuffferReceived) - 1);
  udpBuffferReceived[length] = '\0';

  LOG_INFO_APP("\r\nReceived %s\r\n", udpBuffferReceived);

  if(strcmp((char const*)udpBuffferReceived, (char const*)udpBufffer) == 0)
  {
    LOG_INFO_APP("\r\nComparison OK!\r\n");
    APP_LED_TOGGLE(LED_BLUE);
  }
}

/**
 * @brief This function initiates after pushing SW1 the UdpSend procedure
 *
 * @param None
 * @retval None
 */
static void APP_THREAD_SW1_Process(void)
{
  otDeviceRole eDeviceRole = otThreadGetDeviceRole(PtOpenThreadInstance);
  
  if (eDeviceRole == OT_DEVICE_ROLE_LEADER)
  {
    /* Send Udp request */
    (void)UdpSend();
  }
  else if ((eDeviceRole == OT_DEVICE_ROLE_ROUTER)||(eDeviceRole == OT_DEVICE_ROLE_CHILD))
  {
    LOG_INFO_APP("\r\nDevice is Router or child press SW2 to send Udp request\r\n");
  }
  else
  {
    LOG_INFO_APP("\r\nDevice must be attached before send Udp request\r\n");
  }
}


/**
 * @brief This function initiates after pushing SW2 the UdpSend procedure
 *
 * @param None
 * @retval None
 */
static void APP_THREAD_SW2_Process(void)
{
  otDeviceRole eDeviceRole = otThreadGetDeviceRole(PtOpenThreadInstance);
  
  if (eDeviceRole == OT_DEVICE_ROLE_LEADER)
  {
    LOG_INFO_APP("\r\nDevice is Leader press SW1 to send Udp request\r\n");
  }
  else if ((eDeviceRole == OT_DEVICE_ROLE_ROUTER)||(eDeviceRole == OT_DEVICE_ROLE_CHILD))
  {
    /* Send Udp request */
    (void)UdpSend();
  }
  else
  {
    LOG_INFO_APP("\r\nDevice must be attached before send Udp request\r\n");
  }

}

static otError UdpSend(void)
{
    otError       error;
    otMessageInfo messageInfo;
    otMessage *   message = NULL;

    memset(&messageInfo, 0, sizeof(messageInfo));

    error = otIp6AddressFromString("ff02::1", &messageInfo.mPeerAddr);
    SuccessOrExit(error);

    messageInfo.mPeerPort    = udpPort;

    message = otUdpNewMessage(NULL, NULL);
    VerifyOrExit(message != NULL, error = OT_ERROR_NO_BUFS);

    error = otMessageAppend(message, udpBufffer, (uint16_t)strlen((const char*)udpBufffer));
    SuccessOrExit(error);

    LOG_INFO_APP("\r\nSending UDP message %s\r\n", udpBufffer);
    error = otUdpSend(PtOpenThreadInstance, &mSocket, message, &messageInfo);

exit:

    if (error != OT_ERROR_NONE && message != NULL)
    {
        LOG_INFO_APP("\r\nUdpSend failed with error : %s", error);
        otMessageFree(message);
    }

    return error;
}

void APP_BSP_Button1Action(void)
{
  APP_THREAD_SW1_Process();
}

void APP_BSP_Button2Action(void)
{
  APP_THREAD_SW2_Process();
}

/* USER CODE END FD_LOCAL_FUNCTIONS */

