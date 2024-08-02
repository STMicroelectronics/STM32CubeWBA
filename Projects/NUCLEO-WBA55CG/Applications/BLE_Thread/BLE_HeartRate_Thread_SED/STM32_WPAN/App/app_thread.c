/**
 ******************************************************************************
 * @file    app_thread.c
 * @author  MCD Application Team
 * @version
 * @date
 * @brief   This file contains OpenThread stack Init functions.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2021 STMicroelectronics</center></h2>
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

#include <assert.h>

#include "stm32wbaxx_hal.h"
#include "app_conf.h"
#include "instance.h"
#include "main.h"
#include "openthread-system.h"
#include "cli.h"
#include "radio.h"
#include "platform.h"
#include "tasklet.h"
#include "ll_sys.h"
#include "scm.h"
#include "ll_sys_startup.h"

#include "app_thread.h"
#include "threadplat_pka.h"
#include "common_types.h"
#include "stm32_lpm.h"
#include "stm32_timer.h"

#include "stm32_seq.h"
#include "event_manager.h"
#include "platform_wba.h"
#include "coap.h"
#include "link.h"
#include "thread.h"
#include "coap.h"

extern void otPlatSettingsWipe(otInstance *aInstance);
#include "joiner.h"
#include OPENTHREAD_CONFIG_FILE

#if ((CFG_BUTTON_SUPPORTED ==1)||(CFG_LED_SUPPORTED == 1))
#include "stm32wbaxx_nucleo.h"
#endif

/* Private defines -----------------------------------------------------------*/
#define C_SIZE_CMD_STRING       256U
#define C_PANID                 0xBA98U
#define C_CHANNEL_NB            16U
#define C_CCA_THRESHOLD         -70

/* USER CODE BEGIN PD */
#define C_RESSOURCE             "light"
#define COAP_PAYLOAD_LENGTH      2
#define C_PASSWORD              "PASSED"

otExtAddress ext_addr =
{
  {0x11, 0x11, 0x22, 0x22, 0xb1, 0x2e, 0xc4, 0x1d},
};
static void APP_THREAD_DeviceConfig(void);
static void APP_THREAD_StateNotif(uint32_t NotifFlags, void *pContext);
static void APP_THREAD_Child_Role_Handler(void);

static void ProcessPka(void);

static void APP_THREAD_CoapRequestHandler(void                * pContext,
    otMessage           * pMessage,
    const otMessageInfo * pMessageInfo);
static void APP_THREAD_TraceError(const char * pMess, uint32_t ErrCode);

static void APP_THREAD_SendCoapMulticastRequest(void);
static void APP_THREAD_CoapSendRequest(otCoapResource* aCoapRessource,
    otCoapType aCoapType,
    otCoapCode aCoapCode,
    const char *aStringAddress,
    const otIp6Address* aPeerAddress,
    uint8_t* aPayload,
    uint16_t Size,
    otCoapResponseHandler aHandler,
    void *aContext);

#define COAP_SEND_TIMEOUT  (2000) /**< 2s */
static UTIL_TIMER_Object_t coapSendTimer;
static void coapSendTimerCallback(void *arg);
#define THREAD_LINK_POLL_PERIOD_MS (5000) /**< 5s */

otInstance *PtOpenThreadInstance;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

static otCoapResource OT_Ressource = {C_RESSOURCE, APP_THREAD_CoapRequestHandler, "myContext", NULL};
static otMessageInfo OT_MessageInfo = {0};
static uint8_t PayloadWrite[COAP_PAYLOAD_LENGTH]= {0};
static uint8_t OT_ReceivedCommand = 0;
static otMessage   * pOT_Message = NULL;
bool coapAlreadyStart = FALSE;
static otLinkModeConfig OT_LinkMode = {0};

/* USER CODE END PV */

void APP_THREAD_ScheduleAlarm(void)
{
  UTIL_SEQ_SetTask( 1U<< CFG_TASK_OT_ALARM, CFG_TASK_PRIO_ALARM);
}

void APP_THREAD_ScheduleUsAlarm(void)
{
  UTIL_SEQ_SetTask( 1U<< CFG_TASK_OT_US_ALARM, CFG_TASK_PRIO_US_ALARM);
}

void Thread_Init(void)
{
  otSysInit(0, NULL);

  PtOpenThreadInstance = otInstanceInitSingle();
  assert(PtOpenThreadInstance);
  otDispatch_tbl_init(PtOpenThreadInstance);


  /* Register tasks */
  UTIL_SEQ_RegTask(1 << CFG_TASK_HW_PKA, UTIL_SEQ_RFU, ProcessPka);
  
  UTIL_SEQ_RegTask(1<<CFG_TASK_OT_ALARM, UTIL_SEQ_RFU, ProcessAlarm);
  UTIL_SEQ_RegTask(1<<CFG_TASK_OT_US_ALARM, UTIL_SEQ_RFU, ProcessUsAlarm);
  
  UTIL_SEQ_RegTask(1<<CFG_TASK_SEND_COAP_MSG, UTIL_SEQ_RFU,APP_THREAD_SendCoapMulticastRequest);

  UTIL_SEQ_RegTask(1<<CFG_TASK_OT_TASKLETS, UTIL_SEQ_RFU, ProcessOpenThreadTasklets);

  ll_sys_thread_init();

  /* Run first time */
  UTIL_SEQ_SetTask(1U<<CFG_TASK_OT_ALARM, CFG_TASK_PRIO_ALARM);
}

static void ProcessPka(void)
{
  otPlatPkaProccessLoop();
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
  if (otTaskletsArePending(PtOpenThreadInstance) == TRUE) {
    UTIL_SEQ_SetTask( 1U<< CFG_TASK_OT_TASKLETS, CFG_SEQ_PRIO_0);
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
  /* Process the tasklet */
  otTaskletsProcess(PtOpenThreadInstance);
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
  UTIL_SEQ_SetTask(1U<<CFG_TASK_OT_TASKLETS, CFG_TASK_PRIO_TASKLETS);
}

void APP_THREAD_Init( void )
{
  scm_setsystemclock(SCM_USER_APP, HSE_32MHZ);
  UTIL_LPM_SetStopMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);
  UTIL_LPM_SetOffMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);

  Thread_Init();

  UTIL_TIMER_Status_t tmr_status = UTIL_TIMER_OK;
  /**
   * Create timer to handle COAP request sending
   */
  tmr_status = UTIL_TIMER_Create(&coapSendTimer,
                                 COAP_SEND_TIMEOUT,
                                 UTIL_TIMER_PERIODIC,
                                 &coapSendTimerCallback,
                                 NULL
                                 );
  if (tmr_status != UTIL_TIMER_OK)
    APP_THREAD_TraceError("ERROR : COAP TIMER init failed", -1);

  APP_THREAD_DeviceConfig();
}

void APP_THREAD_SchedulePka(void)
{
  /* Schedule otPlatPkaProccessLoop() */
  UTIL_SEQ_SetTask(1 << CFG_TASK_HW_PKA, CFG_TASK_PRIO_HW_PKA);
}

void APP_THREAD_WaitPkaEndOfOperation(void)
{
  /* Wait for event CFG_IDLEEVT_PKA_END_OF_OPERATION */
  UTIL_SEQ_WaitEvt(1<<CFG_IDLEEVT_PKA_END_OF_OPERATION);
}

void APP_THREAD_PostPkaEndOfOperation(void)
{
  /* Pka operation ended, set CFG_IDLEEVT_PKA_END_OF_OPERATION event */
  UTIL_SEQ_SetEvt(1<<CFG_IDLEEVT_PKA_END_OF_OPERATION);
}

/**
 * @brief Thread initialization.
 * @param  None
 * @retval None
 */
static void APP_THREAD_DeviceConfig(void)
{
  otError error;
  otNetworkKey networkKey = {{0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00}};
  
  //error = otInstanceErasePersistentInfo(PtOpenThreadInstance);
  error = otSetStateChangedCallback(PtOpenThreadInstance, 
                                            APP_THREAD_StateNotif, NULL);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_START,error);
  }
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_SET_STATE_CB,error);
  }
  error = otPlatRadioSetCcaEnergyDetectThreshold(PtOpenThreadInstance, C_CCA_THRESHOLD);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_SET_THRESHOLD,error);
  }
  error = otLinkSetPanId(PtOpenThreadInstance, C_PANID);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_SET_PANID,error);
  }
  error = otThreadSetNetworkKey(PtOpenThreadInstance, &networkKey);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_SET_NETWORKKEY,error);
  }
  error = otLinkSetChannel(PtOpenThreadInstance, C_CHANNEL_NB);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_SET_CHANNEL,error);
  }
  error = otLinkSetExtendedAddress(PtOpenThreadInstance, &ext_addr);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_SET_EXTADDR,error);
  }
  error = otIp6SetEnabled(PtOpenThreadInstance, true);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_IPV6_ENABLE,error);
  }

  /* After reaching the child or router state, the system
   *   a) sets the 'sleepy end device' mode
   *   b) perform a Thread stop
   *   c) perform a Thread start.
   *
   *  NOTE : According to the Thread specification, it is necessary to set the
   *         mode before starting Thread.
   *
   * A Child that has attached to its Parent indicating it is an FTD MUST NOT use Child UpdateRequest
   * to modify its mode to MTD.
   * As a result, you need to first detach from the network before switching from FTD to MTD at runtime,
   * then reattach.
   *
   */
  /* Set the pool period . It means that when the device will enter in 'sleepy
   * end device' mode, it will send an ACK_Request every 
   * THREAD_LINK_POLL_PERIOD_MS.
   * This message will act as keep alive message.
   */
  /* Set the sleepy end device mode */
  OT_LinkMode.mRxOnWhenIdle = 0;
  OT_LinkMode.mDeviceType = 0; /* 0: MTD, 1: FTD */
  OT_LinkMode.mNetworkData = 1U;

  error = otThreadSetLinkMode(PtOpenThreadInstance,OT_LinkMode);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_LINK_MODE,error);
  }
  error = otLinkSetPollPeriod(PtOpenThreadInstance, THREAD_LINK_POLL_PERIOD_MS);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_POLL_MODE,error);
  }
  error = otThreadSetEnabled(PtOpenThreadInstance, true);
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

  /* USER CODE END DEVICECONFIG */
  return;
}

/**
  * @brief Send a COAP multicast request to all the devices which are connected
  *        on the Thread network
  * @param None
  * @retval None
  */
static void APP_THREAD_SendCoapMulticastRequest(void)
{
  PayloadWrite[0] = 0xff;
  PayloadWrite[1] = 0xff;

  /* Send the COAP request */
  APP_THREAD_CoapSendRequest(&OT_Ressource,
      OT_COAP_TYPE_NON_CONFIRMABLE,
      OT_COAP_CODE_PUT,
      MULICAST_FTD_MED,
      NULL,
      &PayloadWrite[0],
      sizeof(PayloadWrite),
      NULL,
      NULL);
}

/**
 * @brief Handler called when the server receives a COAP request.
 *
 * @param pContext : Context
 * @param pMessage : Message
 * @param pMessageInfo : Message information
 * @retval None
 */
static void APP_THREAD_CoapRequestHandler(void                * pContext,
    otMessage           * pMessage,
    const otMessageInfo * pMessageInfo)
{
  do
  {
    if (otCoapMessageGetType(pMessage) != OT_COAP_TYPE_NON_CONFIRMABLE)
    {
      break;
    }

    if (otCoapMessageGetCode(pMessage) != OT_COAP_CODE_PUT)
    {
      break;
    }

    if (otMessageRead(pMessage, otMessageGetOffset(pMessage), &OT_ReceivedCommand, 1U) != 1U)
    {
      APP_THREAD_Error(ERR_THREAD_MESSAGE_READ, 0);
    }
  } while (false);
}

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
static void APP_THREAD_CoapSendRequest(otCoapResource* aCoapRessource,
    otCoapType aCoapType,
    otCoapCode aCoapCode,
    const char *aStringAddress,
    const otIp6Address* aPeerAddress,
    uint8_t* aPayload,
    uint16_t Size,
    otCoapResponseHandler aHandler,
    void* aContext)
{
  otError error = OT_ERROR_NONE;

  do{
    pOT_Message = otCoapNewMessage(PtOpenThreadInstance, NULL);
    if (pOT_Message == NULL)
    {
      APP_THREAD_Error(ERR_THREAD_COAP_NEW_MSG,error);
      break;
    }

    otCoapMessageInit(pOT_Message, aCoapType, aCoapCode);
    otCoapMessageAppendUriPathOptions(pOT_Message, aCoapRessource->mUriPath);
    otCoapMessageSetPayloadMarker(pOT_Message);

    if((aPayload != NULL) && (Size > 0))
    {
      error = otMessageAppend(pOT_Message, aPayload, Size);
      if (error != OT_ERROR_NONE)
      {
        APP_THREAD_Error(ERR_THREAD_COAP_APPEND,error);
        break;
      }
    }

    memset(&OT_MessageInfo, 0, sizeof(OT_MessageInfo));
    memcpy(&OT_MessageInfo.mSockAddr, otThreadGetLinkLocalIp6Address(PtOpenThreadInstance), sizeof(otIp6Address));
    OT_MessageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;

    if((aPeerAddress == NULL) && (aStringAddress != NULL))
    {
      otIp6AddressFromString(aStringAddress, &OT_MessageInfo.mPeerAddr);
    }
    else
    if (aPeerAddress != NULL)
    {
      memcpy(&OT_MessageInfo.mPeerAddr, aPeerAddress, sizeof(OT_MessageInfo.mPeerAddr));
    }
    else
    {
      APP_THREAD_Error(ERR_THREAD_COAP_ADDRESS_NOT_DEFINED, 0);
    }

    if(aCoapType == OT_COAP_TYPE_NON_CONFIRMABLE)
    {
      error = otCoapSendRequest(PtOpenThreadInstance,
          pOT_Message,
          &OT_MessageInfo,
          NULL,
          NULL);
    }
    if(aCoapType == OT_COAP_TYPE_CONFIRMABLE)
    {
      error = otCoapSendRequest(PtOpenThreadInstance,
          pOT_Message,
          &OT_MessageInfo,
          aHandler,
          aContext);
    }
  }while(false);
  if (error != OT_ERROR_NONE && pOT_Message != NULL)
  {
    otMessageFree(pOT_Message);
    APP_THREAD_Error(ERR_THREAD_COAP_SEND_REQUEST,error);
  }
}

static void coapSendTimerCallback(void *arg)
{
  (void)arg;
  UTIL_SEQ_SetTask(1U<<CFG_TASK_SEND_COAP_MSG,CFG_SEQ_PRIO_0);
  return;
}



/**
 * @brief  Warn the user that an error has occurred.In this case,
 *         the LEDs on the Board will start blinking.
 *
 * @param  pMess  : Message associated to the error.
 * @param  ErrCode: Error code associated to the module (OpenThread or other module if any)
 * @retval None
 */
static void APP_THREAD_TraceError(const char * pMess, uint32_t ErrCode)
{
  /* USER CODE BEGIN TRACE_ERROR */
  while(1U == 1U)
  {
  }
  /* USER CODE END TRACE_ERROR */
}

static void APP_THREAD_Child_Role_Handler(void)
{
#if ( CFG_LPM_LEVEL != 0)
  UTIL_LPM_SetStopMode(1 << CFG_LPM_APP, UTIL_LPM_ENABLE);
#if (CFG_LPM_STDBY_SUPPORTED == 1)
  UTIL_LPM_SetOffMode(1 << CFG_LPM_APP, UTIL_LPM_ENABLE);
#endif /* CFG_LPM_STDBY_SUPPORTED */
#endif /* CFG_LPM_LEVEL */

  if (coapAlreadyStart == FALSE)
  {
    UTIL_TIMER_Start(&coapSendTimer);
    coapAlreadyStart = TRUE;
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
  case ERR_THREAD_COAP_APPEND :
    APP_THREAD_TraceError("ERROR : ERR_THREAD_COAP_APPEND ",ErrCode);
    break;
  case ERR_THREAD_COAP_SEND_REQUEST :
    APP_THREAD_TraceError("ERROR : ERR_THREAD_COAP_SEND_REQUEST ",ErrCode);
    break;
    /* USER CODE END APP_THREAD_Error_2 */
  default :
    APP_THREAD_TraceError("ERROR Unknown ", 0);
    break;
  }
}

otDeviceRole current_state = OT_DEVICE_ROLE_DISABLED;
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
    current_state = otThreadGetDeviceRole(PtOpenThreadInstance);
    switch (current_state)
    {
    case OT_DEVICE_ROLE_DISABLED:
      /* USER CODE BEGIN OT_DEVICE_ROLE_DISABLED */
#if (CFG_LED_SUPPORTED == 1)
      BSP_LED_Off(LD2);
      BSP_LED_Off(LD3);
#endif
      /* USER CODE END OT_DEVICE_ROLE_DISABLED */
      break;
    case OT_DEVICE_ROLE_DETACHED:
      /* USER CODE BEGIN OT_DEVICE_ROLE_DETACHED */
      if (coapAlreadyStart == TRUE)
      {
        UTIL_TIMER_Stop(&coapSendTimer);
        coapAlreadyStart = FALSE;
      }
#if (CFG_LED_SUPPORTED == 1)
      BSP_LED_Off(LD2);
      BSP_LED_Off(LD3);
#endif
      /* USER CODE END OT_DEVICE_ROLE_DETACHED */
      break;
    case OT_DEVICE_ROLE_CHILD:
      /* USER CODE BEGIN OT_DEVICE_ROLE_CHILD */
      APP_THREAD_Child_Role_Handler();
#if (CFG_LED_SUPPORTED == 1)
      BSP_LED_Off(LD2);
      BSP_LED_On(LD3);
#endif
      /* USER CODE END OT_DEVICE_ROLE_CHILD */
      break;
    case OT_DEVICE_ROLE_ROUTER:
      /* USER CODE BEGIN OT_DEVICE_ROLE_ROUTER */
#if (CFG_LED_SUPPORTED == 1)
      BSP_LED_Off(LD2);
      BSP_LED_On(LD3);
#endif      
      /* USER CODE END OT_DEVICE_ROLE_ROUTER */
      break;
    case OT_DEVICE_ROLE_LEADER :
      /* USER CODE BEGIN OT_DEVICE_ROLE_LEADER */
#if (CFG_LED_SUPPORTED == 1)
      BSP_LED_On(LD2);
      BSP_LED_Off(LD3);
#endif
      /* USER CODE END OT_DEVICE_ROLE_LEADER */
      break;
    default:
      /* USER CODE BEGIN DEFAULT */
#if (CFG_LED_SUPPORTED == 1)
      BSP_LED_Off(LD2);
      BSP_LED_Off(LD3);
#endif      
      /* USER CODE END DEFAULT */
      break;
    }
  }
}


/* USER CODE END FD_LOCAL_FUNCTIONS */
