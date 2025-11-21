/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * File Name          : app_thread.c
  * Description        : Thread Application.
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
#include <assert.h>

#include "app_conf.h"
#include "app_common.h"
#include "app_entry.h"
#include "app_thread.h"
#include "dbg_trace.h"
#include "stm32_rtos.h"
#include "stm32_timer.h"
#include "common_types.h"
#include "instance.h"
#include "cli.h"
#include "radio.h"
#include "platform.h"
#include "tasklet.h"
#include "ll_sys_startup.h"
#include "event_manager.h"
#include "platform_wba.h"
#include "coap.h"
#include "link.h"
#include "thread.h"
#include "coap.h"
#include "log_module.h"
#include "joiner.h"
#include "alarm.h"
#include "stm32_lpm.h"
#include "ll_sys_if.h"
#include OPENTHREAD_CONFIG_FILE
#include "scm.h"
#include "app_bsp.h"
/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#ifdef OT_CLI_USE
#include "uart.h"
#endif /* OT_CLI_USE */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/
#define C_SIZE_CMD_STRING       256U
#define C_PANID                 0xBA98U
#define C_CHANNEL_NB            16U
#define C_CCA_THRESHOLD         -70

/* USER CODE BEGIN PD */
#define C_RESSOURCE             "light"
#define COAP_PAYLOAD_LENGTH      2

/* Enable periodic CoAp message transmission to Thread Leader */
#define APP_THREAD_PERIODIC_TRANSMIT

#ifdef APP_THREAD_PERIODIC_TRANSMIT
#define APP_THREAD_TRANSMIT_PERIOD      (1*1000)        /**< 1000ms */
#endif
/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private function prototypes -----------------------------------------------*/
static void APP_THREAD_DeviceConfig(void);
static void APP_THREAD_StateNotif(uint32_t NotifFlags, void *pContext);
static void APP_THREAD_TraceError(const char * pMess, uint32_t ErrCode);

/* USER CODE BEGIN PFP */
static void APP_THREAD_CoapSendRequest( otCoapResource        * aCoapRessource,
                                        otCoapType            aCoapType,
                                        otCoapCode            aCoapCode,
                                        const char            * aStringAddress,
                                        const otIp6Address    * aPeerAddress,
                                        uint8_t               * aPayload,
                                        uint16_t              Size,
                                        otCoapResponseHandler aHandler,
                                        void                  * aContext);

static void APP_THREAD_CoapRequestHandler(  void                * pContext,
                                            otMessage           * pMessage,
                                            const otMessageInfo * pMessageInfo);

static void APP_THREAD_CoapSendDataResponse(otMessage           * pMessage,
                                            const otMessageInfo * pMessageInfo);

#if (CFG_JOYSTICK_SUPPORTED == 1)
static void APP_THREAD_CoapDataRespHandler( void                * aContext,
                                            otMessage           * pMessage,
                                            const otMessageInfo * pMessageInfo,
                                            otError             result);
#endif

static void APP_THREAD_InitPayloadWrite(void);
static void APP_THREAD_SendCoapMsgWithNoConf(void);
#if (CFG_JOYSTICK_SUPPORTED == 1)
static void APP_THREAD_SendCoapMsgWithConf(void);
#endif
static bool APP_THREAD_CheckMsgValidity(void);
#ifdef APP_THREAD_PERIODIC_TRANSMIT
static void APP_THREAD_TransmitRequest(void *arg);
#endif

#if (OT_CLI_USE == 1)
static void APP_THREAD_CliInit(otInstance *aInstance);
static void APP_THREAD_ProcessUart(void);
#endif /* OT_CLI_USE */

/* USER CODE END PFP */

/* Private variables -----------------------------------------------*/
static otInstance * PtOpenThreadInstance;

/* USER CODE BEGIN PV */
static otCoapResource OT_Ressource = {C_RESSOURCE, APP_THREAD_CoapRequestHandler, "MyOwnContext", NULL};
static otMessageInfo OT_MessageInfo = {0};
static otMessage* pOT_Message = NULL;
static otMessage* pOT_MessageResponse = NULL;

static uint8_t PayloadWrite[COAP_PAYLOAD_LENGTH]= {0};
static uint8_t PayloadRead[COAP_PAYLOAD_LENGTH]= {0};
static uint8_t CounterRole= {0};

#ifdef APP_THREAD_PERIODIC_TRANSMIT
osTimerId_t APP_Thread_transmitTimerHandle;
#endif

const osThreadAttr_t stAlarmTaskAttributes = 
{
  .name = "Alarm Task",
  .priority = CFG_TASK_PRIO_ALARM,
  .stack_size = TASK_ALARM_STACK_SIZE
};

const osThreadAttr_t stUsAlarmTaskAttributes = 
{
  .name = "UsAlarm Task",
  .priority = CFG_TASK_PRIO_US_ALARM,
  .stack_size = TASK_ALARM_US_STACK_SIZE
};

const osThreadAttr_t stTaskletsTaskAttributes = 
{
  .name = "Tasklets Task",
  .priority = CFG_TASK_PRIO_TASKLETS,
  .stack_size = TASK_TASKLETS_STACK_SIZE
};

const osThreadAttr_t stCliUartTaskAttributes = 
{
  .name = "CliUart Task",
  .priority = CFG_TASK_PRIO_CLI_UART,
  .stack_size = TASK_CLI_UART_STACK_SIZE
};

const osThreadAttr_t stSendCoapMsgTaskAttributes = 
{
  .name = "SendCoapMsg Task",
  .priority = CFG_TASK_PRIO_SEND_COAP_MSG,
  .stack_size = TASK_SEND_COAP_MSG_STACK_SIZE
};

osSemaphoreId_t       AlarmSemaphore, UsAlarmSemaphore;
osThreadId_t          AlarmTaskId, UsAlarmTaskId;

osSemaphoreId_t       CliUartSemaphore, TaskletsSemaphore, SendCoapMsgSemaphore;
os_thread_id          CliUartTaskId, TaskletsTaskId, SendCoapMsgTaskId; 

/* USER CODE END PV */

/* Functions Definition ------------------------------------------------------*/
#if (OT_CLI_USE == 1)
/**
 * @brief   Background Task for Cli Uart.
 */
static void Cli_Uart_Task( void * argument )
{
  UNUSED(argument);  
  while(1)
  {
    /* Wait for task semaphore to be released */
    osSemaphoreAcquire( CliUartSemaphore, osWaitForever );
    APP_THREAD_ProcessUart();
    osThreadYield();
  }
}
#endif // (OT_CLI_USE == 1)


#ifdef GRL_TEST
osSemaphoreId_t       TaskletsMutex;
#endif

void APP_THREAD_ScheduleAlarm(void)
{
  osSemaphoreRelease(AlarmSemaphore);
}

void APP_THREAD_ScheduleUsAlarm(void)
{
  osSemaphoreRelease(UsAlarmSemaphore);
}

/**
 * @brief   Background Task for Alarm.
 */
static void Alarm_Task( void * argument )
{
  UNUSED(argument);  
  while(1)
  {
    /* Wait for task semaphore to be released */
    osSemaphoreAcquire( AlarmSemaphore, osWaitForever );
    osMutexAcquire(LinkLayerMutex, osWaitForever);
    ProcessAlarm();
    osMutexRelease(LinkLayerMutex);
    osThreadYield();
  }
}

/**
 * @brief   Background Task for US Alarm.
 */
static void Us_Alarm_Task( void * argument )
{
  UNUSED(argument);  
  while(1)
  {
    /* Wait for task semaphore to be released */
    osSemaphoreAcquire( UsAlarmSemaphore, osWaitForever );
    osMutexAcquire(LinkLayerMutex, osWaitForever);
    ProcessUsAlarm();
    osMutexRelease(LinkLayerMutex);
    osThreadYield();
  }
}

/**
 * @brief   Background Task for Tasklets.
 */
static void Tasklets_Task( void * argument )
{
  UNUSED(argument);
  while(1)
  {
    /* Wait for task semaphore to be released */
    osSemaphoreAcquire( TaskletsSemaphore, osWaitForever );
    osMutexAcquire(LinkLayerMutex, osWaitForever);
    ProcessOpenThreadTasklets();
    osMutexRelease(LinkLayerMutex);
    osThreadYield();
  }
}

/**
 * @brief   Background Task for Send Coap Messages.
 */
static void Send_Coap_Msg_Task( void * argument )
{
  UNUSED(argument);  
  while(1)
  {
    /* Wait for task semaphore to be released */
    osSemaphoreAcquire( SendCoapMsgSemaphore, osWaitForever );
    APP_THREAD_SendCoapMsgWithNoConf();
    osThreadYield();
  }
}

void Thread_Init(void)
{
#if OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
  size_t otInstanceBufferLength = 0;
  uint8_t *otInstanceBuffer = NULL;
#endif

  otSysInit(0, NULL);

#if OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
  // Call to query the buffer size
  (void)otInstanceInit(NULL, &otInstanceBufferLength);

  // Call to allocate the buffer
  otInstanceBuffer = (uint8_t *)malloc(otInstanceBufferLength);
  assert(otInstanceBuffer);

  // Initialize OpenThread with the buffer
  PtOpenThreadInstance = otInstanceInit(otInstanceBuffer, &otInstanceBufferLength);
#else
  PtOpenThreadInstance = otInstanceInitSingle();
#endif

  assert(PtOpenThreadInstance);

#if (OT_CLI_USE == 1)
  APP_THREAD_CliInit(PtOpenThreadInstance);
#endif

  otDispatch_tbl_init(PtOpenThreadInstance);

  /* Register tasks */ 
  
#if (OT_CLI_USE == 1)
  /* Create thread for cli uart and semaphore to control it*/
  CliUartSemaphore = osSemaphoreNew( 1, 0, NULL );
  if ( CliUartSemaphore == NULL )
  { 
    APP_DBG( "ERROR FREERTOS : CLI UART SEMAPHORE CREATION FAILED" );
    while(1);
  }  

  CliUartTaskId = osThreadNew( Cli_Uart_Task, NULL, &stCliUartTaskAttributes );
  if ( CliUartTaskId == NULL )
  { 
    APP_DBG( "ERROR FREERTOS : CLI UART TASK CREATION FAILED" );
    while(1);
  }  
#endif
  /* Create thread for Alarm and semaphore to control it*/
  AlarmSemaphore = osSemaphoreNew( 1, 0, NULL );
  if ( AlarmSemaphore == NULL )
  { 
    APP_DBG( "ERROR FREERTOS : ALARM SEMAPHORE CREATION FAILED" );
    while(1);
  }  

  AlarmTaskId = osThreadNew( Alarm_Task, NULL, &stAlarmTaskAttributes );
  if ( AlarmTaskId == NULL )
  { 
    APP_DBG( "ERROR FREERTOS : ALARM TASK CREATION FAILED" );
    while(1);
  }  
  
  /* Create thread for US Alarm and semaphore to control it*/
  UsAlarmSemaphore = osSemaphoreNew( 1, 0, NULL );
  if ( UsAlarmSemaphore == NULL )
  { 
    APP_DBG( "ERROR FREERTOS : US ALARM SEMAPHORE CREATION FAILED" );
    while(1);
  }  

  UsAlarmTaskId = osThreadNew( Us_Alarm_Task, NULL, &stUsAlarmTaskAttributes );
  if ( UsAlarmTaskId == NULL )
  { 
    APP_DBG( "ERROR FREERTOS : US ALARM TASK CREATION FAILED" );
    while(1);
  } 

  /* Create thread for Tasklets and semaphore to control it*/
  TaskletsSemaphore = osSemaphoreNew( 1, 0, NULL );
  if ( TaskletsSemaphore == NULL )
  { 
    APP_DBG( "ERROR FREERTOS : TASKLETS SEMAPHORE CREATION FAILED" );
    while(1);
  }  

  TaskletsTaskId = osThreadNew( Tasklets_Task, NULL, &stTaskletsTaskAttributes );
  if ( TaskletsTaskId == NULL )
  { 
    APP_DBG( "ERROR FREERTOS : TASKLETS TASK CREATION FAILED" );
    while(1);
  }

#ifdef GRL_TEST
  /* Create Mutex to protect OT thread */
  TaskletsMutex = osSemaphoreNew( 1, 1, NULL );
  if ( TaskletsMutex == NULL )
  {
    APP_DBG( "ERROR FREERTOS : TASKLETS SEMAPHORE CREATION FAILED" );
    while(1);
  }
#endif

  ll_sys_thread_init();

  /* Run first time */
  osSemaphoreRelease(AlarmSemaphore);
  osSemaphoreRelease(TaskletsSemaphore);
#if (OT_CLI_USE == 1)
  osSemaphoreRelease(CliUartSemaphore);
#endif

  /* USER CODE BEGIN INIT TASKS */
#ifdef APP_THREAD_PERIODIC_TRANSMIT
  /* Create thread for periodic CoAp msg transmission and semaphore to control it*/
  SendCoapMsgSemaphore = osSemaphoreNew( 1, 0, NULL );
  if ( SendCoapMsgSemaphore == NULL )
  { 
    APP_DBG( "ERROR FREERTOS : SEND COAP MESSAGES SEMAPHORE CREATION FAILED" );
    while(1);
  }  

  SendCoapMsgTaskId = osThreadNew( Send_Coap_Msg_Task, NULL, &stSendCoapMsgTaskAttributes );
  if ( SendCoapMsgTaskId == NULL )
  { 
    APP_DBG( "ERROR FREERTOS : SEND COAP MESSAGES TASK CREATION FAILED" );
    while(1);
  }

  /* Create timer to handle periodic CoAp msg transmission */
  APP_Thread_transmitTimerHandle = osTimerNew(APP_THREAD_TransmitRequest, osTimerPeriodic, NULL, NULL);
#endif
  /* USER CODE END INIT TASKS */

}

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
    osSemaphoreRelease(TaskletsSemaphore);
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

  /* Put the IP802_15_4 back to sleep mode */
  //ll_sys_radio_hclk_ctrl_req(LL_SYS_RADIO_HCLK_LL_BG, LL_SYS_RADIO_HCLK_OFF);

  /* Reschedule the tasklets if any */
  ProcessTasklets();
}

/**
 * OpenThread calls this function when the tasklet queue transitions from empty to non-empty.
 *
 * @param[in] aInstance A pointer to an OpenThread instance.
 */
void otTaskletsSignalPending(otInstance *aInstance)
{
  osSemaphoreRelease(TaskletsSemaphore);
}

void APP_THREAD_Init( void )
{
  UTIL_LPM_SetStopMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);
  UTIL_LPM_SetOffMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);

  Thread_Init();

  APP_THREAD_DeviceConfig();
}

/**
 * @brief Thread initialization.
 * @param  None
 * @retval None
 */
static void APP_THREAD_DeviceConfig(void)
{
  otError error = OT_ERROR_NONE;
  
  #ifndef GRL_TEST
  otNetworkKey networkKey = {{0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00}};
  #endif

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
  
#ifndef GRL_TEST
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
#endif
  otPlatRadioEnableSrcMatch(PtOpenThreadInstance, true);

  error = otIp6SetEnabled(PtOpenThreadInstance, true);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_IPV6_ENABLE,error);
  }
  
#ifndef GRL_TEST
#ifdef OPENTHREAD_MTD
  /* Set MTD device type */
  static otLinkModeConfig OT_LinkMode = {
  .mRxOnWhenIdle = 1,
  .mDeviceType   = 0, /* 0: MTD, 1: FTD */
  .mNetworkData  = 1
  };

  error = otThreadSetLinkMode(PtOpenThreadInstance,OT_LinkMode);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_LINK_MODE,error);
  }
#endif
#endif 
  
#ifdef GRL_TEST
  error = otThreadSetEnabled(PtOpenThreadInstance, false);
#else
  error = otThreadSetEnabled(PtOpenThreadInstance, true);
#endif 
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_START,error);
  }

  /* USER CODE BEGIN DEVICECONFIG */
  /* Start the COAP server */
  error = otCoapStart(PtOpenThreadInstance, OT_DEFAULT_COAP_PORT);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_COAP_START,error);
  }

  /* Add COAP resources */
  otCoapAddResource(PtOpenThreadInstance, &OT_Ressource);

  APP_THREAD_InitPayloadWrite();

  /* USER CODE END DEVICECONFIG */
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
  LOG_ERROR_APP("**** FATAL ERROR = %s (Err = %d)", pMess, ErrCode);

  while (1)
  {
#if CFG_LED_SUPPORTED
    /* In this case, the LEDs on the Board will start blinking. */
    BSP_LED_Toggle(LED_GREEN);
    HAL_Delay(500U);
    BSP_LED_Toggle(LED_RED);
    HAL_Delay(500U);
#endif
  }

  /* USER CODE END TRACE_ERROR */
}

/**
 * @brief Send a non-confirmable CoAp message.
 * @param  None
 * @retval None
 */
static void APP_THREAD_SendCoapMsgWithNoConf(void)
{
  LOG_INFO_APP("Send a CoAP NON-CONFIRMABLE PUT Request");
  
  /* Send a NON-CONFIRMABLE PUT Request */
  APP_THREAD_CoapSendRequest(&OT_Ressource, OT_COAP_TYPE_NON_CONFIRMABLE, OT_COAP_CODE_PUT, MULICAST_FTD_MED,
                              NULL, PayloadWrite, sizeof(PayloadWrite), NULL, NULL);

}

#if (CFG_JOYSTICK_SUPPORTED == 1)
/**
 * @brief Send a confirmable CoAp message.
 * @param  None
 * @retval None
 */
static void APP_THREAD_SendCoapMsgWithConf(void)
{
  LOG_INFO_APP("Send a CoAP CONFIRMABLE PUT Request");
  
  /* Send a CONFIRMABLE PUT Request */
  APP_THREAD_CoapSendRequest(&OT_Ressource, OT_COAP_TYPE_CONFIRMABLE, OT_COAP_CODE_PUT, MULICAST_FTD_MED,
                             NULL, PayloadWrite, sizeof(PayloadWrite), APP_THREAD_CoapDataRespHandler, NULL);
}
#endif

/**
 * @brief Handler called when the server receives a COAP request.
 *
 * @param pContext : Context
 * @param pMessage : Message
 * @param pMessageInfo : Message information
 * @retval None
 */
static void APP_THREAD_CoapRequestHandler(void                 * pContext,
                                          otMessage            * pMessage,
                                          const otMessageInfo  * pMessageInfo)

{
  LOG_INFO_APP("Received CoAP request (context = %s)", pContext);
  
  if (otMessageRead(pMessage, otMessageGetOffset(pMessage), &PayloadRead, sizeof(PayloadRead)) != sizeof(PayloadRead))
  {
    APP_THREAD_Error(ERR_THREAD_MESSAGE_READ, 0);
  }

  if (APP_THREAD_CheckMsgValidity() == true)
  {
    BSP_LED_Toggle(LED_GREEN);
  }
  
  /* If Message is Confirmable, send response */
  if (otCoapMessageGetType(pMessage) == OT_COAP_TYPE_CONFIRMABLE)
  {
    APP_THREAD_CoapSendDataResponse(pMessage, pMessageInfo);
  }
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

    /* USER CODE BEGIN APP_THREAD_Error_2 */
    case ERR_THREAD_COAP_START :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_COAP_START ",ErrCode);
        break;

    case ERR_THREAD_COAP_ADD_RESSOURCE :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_COAP_ADD_RESSOURCE ",ErrCode);
        break;

    case ERR_THREAD_MESSAGE_READ :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_MESSAGE_READ ",ErrCode);
        break;

    case ERR_THREAD_COAP_SEND_RESPONSE :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_COAP_SEND_RESPONSE ",ErrCode);
        break;

    case ERR_THREAD_COAP_APPEND :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_COAP_APPEND ",ErrCode);
        break;

    case ERR_THREAD_COAP_SEND_REQUEST :
        APP_THREAD_TraceError("ERROR : ERR_THREAD_COAP_SEND_REQUEST ",ErrCode);
        break;

    case ERR_THREAD_MSG_COMPARE_FAILED:
        APP_THREAD_TraceError("ERROR : ERR_THREAD_MSG_COMPARE_FAILED ",ErrCode);
        break;

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
#if (CFG_LED_SUPPORTED == 1)
          BSP_LED_Off(LED_GREEN);
          BSP_LED_Off(LED_RED);
#endif
#ifdef APP_THREAD_PERIODIC_TRANSMIT
          /* Stop timer for periodic CoAp msg transmission */
          osTimerStop(APP_Thread_transmitTimerHandle);
          LOG_INFO_APP("INFO: PERIODIC_TRANSMIT timer stopped");
          CounterRole++;
          LOG_INFO_APP("INFO: Role is DISABLED. CounterRole value = %d",CounterRole);

#endif
          /* USER CODE END OT_DEVICE_ROLE_DISABLED */
          break;

      case OT_DEVICE_ROLE_DETACHED:
          /* USER CODE BEGIN OT_DEVICE_ROLE_DETACHED */
#if (CFG_LED_SUPPORTED == 1)
          BSP_LED_Off(LED_GREEN);
          BSP_LED_Off(LED_RED);
#endif
#ifdef APP_THREAD_PERIODIC_TRANSMIT
          /* Stop timer for periodic CoAp msg transmission */
          osTimerStop(APP_Thread_transmitTimerHandle);
          LOG_INFO_APP("INFO: PERIODIC_TRANSMIT timer stopped");
          CounterRole++;
          LOG_INFO_APP("INFO: Role is DETACHED. CounterRole value = %d",CounterRole);
#endif
          /* USER CODE END OT_DEVICE_ROLE_DETACHED */
          break;

      case OT_DEVICE_ROLE_CHILD:
          /* USER CODE BEGIN OT_DEVICE_ROLE_CHILD */
#if (CFG_LED_SUPPORTED == 1)
          BSP_LED_Off(LED_GREEN);
          BSP_LED_On(LED_RED);
#endif
#ifdef APP_THREAD_PERIODIC_TRANSMIT  
          /* If timer for periodic CoAp msg transmission NOT running */
          if (osTimerIsRunning(APP_Thread_transmitTimerHandle) == 0U)
          {
            /* Start timer for periodic CoAp msg transmission */
            osTimerStart(APP_Thread_transmitTimerHandle, pdMS_TO_TICKS(APP_THREAD_TRANSMIT_PERIOD));
            LOG_INFO_APP("INFO: PERIODIC_TRANSMIT timer started");
          }
          CounterRole++;
          LOG_INFO_APP("INFO: Role is CHILD. CounterRole value = %d",CounterRole);
#endif
          /* USER CODE END OT_DEVICE_ROLE_CHILD */
          break;

      case OT_DEVICE_ROLE_ROUTER :
          /* USER CODE BEGIN OT_DEVICE_ROLE_ROUTER */
#if (CFG_LED_SUPPORTED == 1)
          BSP_LED_Off(LED_GREEN);
          BSP_LED_On(LED_RED);
#endif
#ifdef APP_THREAD_PERIODIC_TRANSMIT  
          /* If timer for periodic CoAp msg transmission NOT running */
          if (osTimerIsRunning(APP_Thread_transmitTimerHandle) == 0U)
          {
            /* Start timer for periodic CoAp msg transmission */
            osTimerStart(APP_Thread_transmitTimerHandle, pdMS_TO_TICKS(APP_THREAD_TRANSMIT_PERIOD));
            LOG_INFO_APP("INFO: PERIODIC_TRANSMIT timer started");
          }
          CounterRole++;
          LOG_INFO_APP("INFO: Role is ROUTER. CounterRole value = %d",CounterRole);
#endif
          /* USER CODE END OT_DEVICE_ROLE_ROUTER */
          break;

      case OT_DEVICE_ROLE_LEADER :
          /* USER CODE BEGIN OT_DEVICE_ROLE_LEADER */
#if (CFG_LED_SUPPORTED == 1)
          BSP_LED_On(LED_GREEN);
          BSP_LED_Off(LED_RED);
#endif
#ifdef APP_THREAD_PERIODIC_TRANSMIT  
          /* If timer for periodic CoAp msg transmission NOT running */
          if (osTimerIsRunning(APP_Thread_transmitTimerHandle) == 0U)
          {
            /* Start timer for periodic CoAp msg transmission */
            osTimerStart(APP_Thread_transmitTimerHandle, pdMS_TO_TICKS(APP_THREAD_TRANSMIT_PERIOD));
            LOG_INFO_APP("INFO: PERIODIC_TRANSMIT timer started");
          }
          CounterRole++;
          LOG_INFO_APP("INFO: Role is LEADER. CounterRole value = %d",CounterRole);
#endif
          /* USER CODE END OT_DEVICE_ROLE_LEADER */
          break;

      default:
          /* USER CODE BEGIN DEFAULT */
#if (CFG_LED_SUPPORTED == 1)
          BSP_LED_Off(LED_GREEN);
          BSP_LED_Off(LED_RED);
#endif
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
  osSemaphoreRelease(CliUartSemaphore);
}

static void APP_THREAD_CliInit(otInstance *aInstance)
{

  (void) otPlatUartEnable();
  otCliInit(aInstance, CliUartOutput, aInstance);
}
#endif /* OT_CLI_USE */

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */
/**
 * @brief Send a CoAP request with defined parameters.
 *
 * @param[in]  aCoapRessource   A pointer to a otCoapResource.
 * @param[in]  aCoapType        otCoapType.
 * @param[in]  aCoapCode        otCoapCode.
 * @param[in]  aStringAddress   A pointer to a NULL-terminated string representing the address. Example: "FF03::1" for Multicast.
 * @param[in]  aPeerAddress     A pointer to otIp6Address Peer Address.
 * @param[in]  aPayload         A pointer to payload.
 * @param[in]  aHandler         A pointer to CoAP response handler.
 * @param[in]  aContext         A pointer to application specific context.
 *
 * @retval none.
 */
static void APP_THREAD_CoapSendRequest( otCoapResource        * aCoapRessource,
                                        otCoapType            aCoapType,
                                        otCoapCode            aCoapCode,
                                        const char            * aStringAddress,
                                        const otIp6Address    * aPeerAddress,
                                        uint8_t               * aPayload,
                                        uint16_t              Size,
                                        otCoapResponseHandler aHandler,
                                        void                  * aContext)
{
  otError error = OT_ERROR_NONE;

  do
  {
    pOT_Message = otCoapNewMessage(PtOpenThreadInstance, NULL);
    if (pOT_Message == NULL)
    {
      APP_THREAD_Error(ERR_THREAD_COAP_NEW_MSG,error);
      break;
    }

    otCoapMessageInit(pOT_Message, aCoapType, aCoapCode);
    (void)otCoapMessageAppendUriPathOptions(pOT_Message, aCoapRessource->mUriPath);
    (void)otCoapMessageSetPayloadMarker(pOT_Message);

    if((aPayload != NULL) && (Size > 0))
    {
      error = otMessageAppend(pOT_Message, aPayload, Size);
      if (error != OT_ERROR_NONE)
      {
        APP_THREAD_Error(ERR_THREAD_COAP_APPEND,error);
        break;
      }
    }
    else
    {
      LOG_ERROR_APP("APP_THREAD_CoapSendRequest: No payload passed");
    }

    memset(&OT_MessageInfo, 0, sizeof(OT_MessageInfo));
    OT_MessageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;

    if((aPeerAddress == NULL) && (aStringAddress != NULL))
    {
      LOG_INFO_APP("Use String Address : %s", aStringAddress);
      (void)otIp6AddressFromString(aStringAddress, &OT_MessageInfo.mPeerAddr);
    }
    else
    {
      if (aPeerAddress != NULL)
      {
        LOG_INFO_APP("Use Peer Address");
        memcpy(&OT_MessageInfo.mPeerAddr, aPeerAddress, sizeof(OT_MessageInfo.mPeerAddr));
      }
      else
      {
        LOG_ERROR_APP("ERROR: Address string and Peer Address not defined");
        APP_THREAD_Error(ERR_THREAD_COAP_ADDRESS_NOT_DEFINED, 0);
      }
    }

    if(aCoapType == OT_COAP_TYPE_NON_CONFIRMABLE)
    {
      LOG_INFO_APP("\r\naCoapType == OT_COAP_TYPE_NON_CONFIRMABLE");
      error = otCoapSendRequest(PtOpenThreadInstance, pOT_Message, &OT_MessageInfo, NULL, NULL);
    }
    else if(aCoapType == OT_COAP_TYPE_CONFIRMABLE)
    {
      LOG_INFO_APP("\r\naCoapType == OT_COAP_TYPE_CONFIRMABLE");
      error = otCoapSendRequest(PtOpenThreadInstance, pOT_Message, &OT_MessageInfo, aHandler, aContext);
    }
  }
  while(false);
  
  if (error != OT_ERROR_NONE && pOT_Message != NULL)
  {
    otMessageFree(pOT_Message);
    APP_THREAD_Error(ERR_THREAD_COAP_SEND_REQUEST,error);
  }
}

/**
 * @brief This function acknowledges the data reception by sending an ACK
 *        back to the sender.
 * @param  pMessage coap message
 * @param  pMessageInfo message info pointer
 * @retval None
 */
static void APP_THREAD_CoapSendDataResponse(otMessage  * pMessage, const otMessageInfo * pMessageInfo)
{
  otError  error = OT_ERROR_NONE;

  do
  {
    LOG_INFO_APP("APP_THREAD_CoapSendDataResponse");

    pOT_MessageResponse = otCoapNewMessage(PtOpenThreadInstance, NULL);
    if (pOT_MessageResponse == NULL)
    {
      LOG_WARNING_APP("WARNING : pOT_MessageResponse = NULL ! -> exit now");
      break;
    }

    error = otCoapMessageInitResponse(pOT_MessageResponse, pMessage, OT_COAP_TYPE_ACKNOWLEDGMENT, OT_COAP_CODE_VALID);

    error = otCoapSendResponse(PtOpenThreadInstance, pOT_MessageResponse, pMessageInfo);
    if (error != OT_ERROR_NONE && pOT_MessageResponse != NULL)
    {
      otMessageFree(pOT_MessageResponse);
      APP_THREAD_Error(ERR_THREAD_COAP_DATA_RESPONSE,error);
    }
  }
  while(false);
}

#if (CFG_JOYSTICK_SUPPORTED == 1)
/**
 * @brief This function manages the data response handler.
 *
 * @param pHeader  context
 * @param pMessage message pointer
 * @param pMessageInfo message info pointer
 * @param result error code
 * @retval None
 */
static void APP_THREAD_CoapDataRespHandler( void                * pContext,
                                            otMessage           * pMessage,
                                            const otMessageInfo * pMessageInfo,
                                            otError               result)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(pMessage);
  UNUSED(pMessageInfo);

  if (result == OT_ERROR_NONE)
  {
    LOG_INFO_APP("APP_THREAD_CoapDataRespHandler : NO ERROR");
  }
  else
  {
    LOG_WARNING_APP("APP_THREAD_CoapDataRespHandler : WARNING. Result = %d",result);
  }
}
#endif

/**
 * @brief Initialize CoAP write buffer.
 * @param  None
 * @retval None
 */
static void APP_THREAD_InitPayloadWrite(void)
{
  uint8_t i;
  
  for(i = 0; i < COAP_PAYLOAD_LENGTH; i++)
  {
    PayloadWrite[i] = 0xFF;
  }
}

/**
 * @brief  Compare the message received versus the original message.
 * @param  None
 * @retval None
 */
static bool APP_THREAD_CheckMsgValidity(void)
{
  bool valid = true;
  uint32_t i;

  for(i = 0; i < COAP_PAYLOAD_LENGTH; i++)
  {
    if(PayloadRead[i] != PayloadWrite[i])
    {
      valid = false;
    }
  }

  if(valid == true)
  {
    LOG_INFO_APP("PAYLOAD Comparison OK!");
  }
  else
  {
    APP_THREAD_Error(ERR_THREAD_MSG_COMPARE_FAILED, 0);
  }
  
  return valid;
}

#ifdef APP_THREAD_PERIODIC_TRANSMIT
/**
 * @brief  Trigger a CoAp message transmission.
 * @param  arg
 * @retval None
 */
static void APP_THREAD_TransmitRequest(void *arg)
{
  osSemaphoreRelease(SendCoapMsgSemaphore);
}
#endif

#if (CFG_JOYSTICK_SUPPORTED == 1)
/**
 * @brief  Management of the right arrow of the JoyStick: Send non-confirmable CoAp message 
 * @param  None
 * @retval None
 */
void APP_BSP_JoystickRightAction(void)
{
  APP_THREAD_SendCoapMsgWithNoConf();
  /* USER CODE BEGIN Button1Action */
  #ifdef APP_THREAD_PERIODIC_TRANSMIT
    /* If timer for periodic CoAp msg transmission running */
    if (osTimerIsRunning(APP_Thread_transmitTimerHandle) == 0U)
    {
    /* Stop timer for periodic CoAp msg transmission */
    osTimerStart(APP_Thread_transmitTimerHandle,100);
    LOG_INFO_APP("INFO: PERIODIC_TRANSMIT timer started");
  }
  #endif
  /* USER CODE END Button1Action */
}

/**
 * @brief  Management of the left arrow of the JoyStick : Send confirmable CoAp message 
 * @param  None
 * @retval None
 */
void APP_BSP_JoystickLeftAction(void)
{
  APP_THREAD_SendCoapMsgWithConf();
  /* USER CODE BEGIN Button2Action */
  osTimerStop(APP_Thread_transmitTimerHandle);
  LOG_INFO_APP("INFO: PERIODIC_TRANSMIT timer stopped");
  /* USER CODE END Button2Action */
}
#endif

/* USER CODE END FD_LOCAL_FUNCTIONS */

