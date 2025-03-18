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
#include "stm32wbaxx_nucleo.h"
#include "app_thread_data_transfer.h"
#include "udp.h"
#include "app_bsp.h"
#include "logging_stm32wba.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/**
  * @brief  APP_THREAD Status structures definition
  */
typedef enum
{
  APP_THREAD_OK       = 0x00,
  APP_THREAD_ERROR    = 0x01,
} APP_THREAD_StatusTypeDef;

/* OTA server information */
struct APP_THREAD_OtaServerInfo_t
{
  uint32_t              lOtaTagOffset; /* The OTA tag offset from the base address in flash (the address of the magic keyword is located here) */
  uint32_t              lMagicKeyword; /* The image magic keyword (terminating word) */
};

typedef struct
{
  uint32_t binarySize;
  uint32_t binaryCrcAddress;
  uint32_t baseAddress;
  uint32_t magicKeyword;
} APP_THREAD_OtaContext_t;

/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/
#define C_PANID                 0xBA98U
#define C_CHANNEL_NB            16U
#define C_CCA_THRESHOLD         (-70)

/* USER CODE BEGIN PD */
#define C_RESSOURCE_FUOTA_PROVISIONING     "FUOTA_PROVISIONING"
#define C_RESSOURCE_FUOTA_PARAMETERS       "FUOTA_PARAMETERS"
#define C_RESSOURCE_FUOTA_SEND             "FUOTA_SEND"

#define HW_FLASH_WIDTH                      (16u)   // Write Lentgh in Bytes (128 bits = 4 words) 

/* Following CoAP resource is requested to reboot on Thread_Ota application */
#define C_RESSOURCE_FUOTA_REBOOT           "FUOTA_REBOOT"

/* Keyword found at the end of Thread Ota compatible application binaries */
#define FUOTA_MAGIC_KEYWORD_M33_APP             0xBF806133u       /* Keyword found at the end of Zigbee Ota file for M33 Application Processor binary */


#define CFG_APP_START_SECTOR_INDEX              0x40u             /* Define the start address where the application shall be located */

/* Define Address for Application FW Update */
#define FUOTA_APP_FW_BINARY_ADDRESS             ( FLASH_BASE + ( CFG_APP_START_SECTOR_INDEX * FLASH_PAGE_SIZE ) )     /* Address for Application Processor FW Update */

#define FUOTA_NUMBER_WORDS_64BITS                50
#define FUOTA_PAYLOAD_SIZE                      (FUOTA_NUMBER_WORDS_64BITS * 8)

/**
 * Define list of reboot reason
 */
#define CFG_REBOOT_ON_FW_APP                    (0x00)
#define CFG_REBOOT_ON_THREAD_OTA_APP            (0x01)

#define FUOTA_M33_APP_OTA_TAG_OFFSET            0x160u            /* Offset of the OTA tag from the base address in flash for M33 Applciation Processor binary */
#define FUOTA_M33_APP_OTA_TAG_ADDRESS           ( FUOTA_APP_FW_BINARY_ADDRESS + FUOTA_M33_APP_OTA_TAG_OFFSET )       /* Address of the OTA tag in flash for M33 Application Processor binary */

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

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
static void APP_THREAD_AppInit(void);
static void APP_THREAD_CoapSendRequest( otCoapResource        * aCoapRessource,
                                        otCoapType            aCoapType,
                                        otCoapCode            aCoapCode,
                                        const char            * aStringAddress,
                                        const otIp6Address    * aPeerAddress,
                                        uint8_t               * aPayload,
                                        uint16_t              Size,
                                        otCoapResponseHandler aHandler,
                                        void                  * aContext);

static void APP_THREAD_ProvisioningRespHandler(void                * pContext,
                                               otMessage           * pMessage,
                                               const otMessageInfo * pMessageInfo,
                                               otError               result);

static void APP_THREAD_CoapRespHandlerFuotaSend( void * pContext,
                                                 otMessage           * pMessage,
                                                 const otMessageInfo * pMessageInfo,
                                                 otError               result);

static void APP_THREAD_CoapRespHandlerFuotaReboot( void * pContext,
                                                   otMessage           * pMessage,
                                                   const otMessageInfo * pMessageInfo,
                                                   otError               result);

static void APP_THREAD_CoapRespHandlerFuotaParameters( void * pContext,
                                                       otMessage           * pMessage,
                                                       const otMessageInfo * pMessageInfo,
                                                       otError               result);

static void APP_THREAD_FuotaSetParamApp(void);
static APP_THREAD_StatusTypeDef APP_THREAD_SetOtaContext(void);
static void APP_THREAD_FuotaProvisioning(void);
static void APP_THREAD_FuotaSend(void);
static bool APP_THREAD_GetBinInfo(uint32_t * fuotaBinarySize);
static bool APP_THREAD_SetBinCrc(uint32_t *fuotaBinaryCrcAddress);
static void APP_THREAD_FuotaParameters(void);
static void APP_THREAD_FuotaReboot(void);
static void APP_THREAD_FuotaInit(void);
/* USER CODE END PFP */

/* Private variables -----------------------------------------------*/
static otInstance * PtOpenThreadInstance;

/* USER CODE BEGIN PV */
static otCoapResource OT_RessourceFuotaProvisioning = {C_RESSOURCE_FUOTA_PROVISIONING, NULL, NULL, NULL};
static otCoapResource OT_RessourceFuotaParameters = {C_RESSOURCE_FUOTA_PARAMETERS, NULL, NULL, NULL};
static otCoapResource OT_RessourceFuotaSend = {C_RESSOURCE_FUOTA_SEND, NULL, NULL, NULL};
static otCoapResource OT_RessourceFuotaReboot = {C_RESSOURCE_FUOTA_REBOOT, NULL, NULL, NULL};

static uint8_t        OT_Command = 0;
static otMessageInfo  pOT_MessageInfo = {0};
static otMessage*     pOT_Message = NULL;

static otIp6Address   OT_PeerAddress = { .mFields.m8 = { 0 } };

static uint64_t FuotaTransferArray[FUOTA_NUMBER_WORDS_64BITS] = {0};

static APP_THREAD_OtaContext_t OtaContext;

static const struct APP_THREAD_OtaServerInfo_t  stOtaServerInfo =
{
  .lOtaTagOffset = FUOTA_M33_APP_OTA_TAG_OFFSET,
  .lMagicKeyword = FUOTA_MAGIC_KEYWORD_M33_APP,
};

/* OTA CRC calculation */
CRC_HandleTypeDef                               stOtaCrcHandle;
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
  APP_THREAD_FuotaInit();
  
  APP_THREAD_AppInit();
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
  /* Start the COAP server */
  error = otCoapStart(PtOpenThreadInstance, OT_DEFAULT_COAP_PORT);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_COAP_START,error);
  }

  /* Add COAP resources */
  otCoapAddResource(PtOpenThreadInstance, &OT_RessourceFuotaProvisioning);

  otCoapAddResource(PtOpenThreadInstance, &OT_RessourceFuotaSend);

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
  LOG_ERROR_APP("**** FATAL ERROR = %s (Err = %d)", pMess, ErrCode);
  /* In this case, the LEDs on the Board will start blinking. */

  /* HAL_Delay() requires TIM2 interrupts to work 
     During ThreadX initialization, all interrupts are disabled
     As this function may be called during this phase, 
     interrupts need to be re-enabled to make LEDs blinking    
  */
  if (__get_PRIMASK())
  {
    __enable_irq();
  }
  
  while (1)
  {
    BSP_LED_Toggle(LD1);
    HAL_Delay(500U);
    BSP_LED_Toggle(LD2);
    HAL_Delay(500U);
    BSP_LED_Toggle(LD3);
    HAL_Delay(500U);
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
          BSP_LED_Off(LD2);
          BSP_LED_Off(LD3);
          /* USER CODE END OT_DEVICE_ROLE_DISABLED */
          break;

      case OT_DEVICE_ROLE_DETACHED:
          /* USER CODE BEGIN OT_DEVICE_ROLE_DETACHED */
          BSP_LED_Off(LD2);
          BSP_LED_Off(LD3);
          /* USER CODE END OT_DEVICE_ROLE_DETACHED */
          break;

      case OT_DEVICE_ROLE_CHILD:
          /* USER CODE BEGIN OT_DEVICE_ROLE_CHILD */
          BSP_LED_Off(LD2);
          BSP_LED_On(LD3);
          /* USER CODE END OT_DEVICE_ROLE_CHILD */
          break;

      case OT_DEVICE_ROLE_ROUTER :
          /* USER CODE BEGIN OT_DEVICE_ROLE_ROUTER */
          BSP_LED_Off(LD2);
          BSP_LED_On(LD3);
          /* USER CODE END OT_DEVICE_ROLE_ROUTER */
          break;

      case OT_DEVICE_ROLE_LEADER :
          /* USER CODE BEGIN OT_DEVICE_ROLE_LEADER */
          BSP_LED_On(LD2);
          BSP_LED_Off(LD3);
          /* USER CODE END OT_DEVICE_ROLE_LEADER */
          break;

      default:
          /* USER CODE BEGIN DEFAULT */
          BSP_LED_Off(LD2);
          BSP_LED_Off(LD3);
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

static void APP_THREAD_FuotaInit(void)
{
  UTIL_SEQ_RegTask( 1u << CFG_TASK_FUOTA_SEND, UTIL_SEQ_RFU, APP_THREAD_FuotaSend);
}

/**
 * @brief  OTA compute CRC in hardware
 * @param  pstCrcHandle: CRC handle
 * @param  plBuffer: Buffer to compute CRC over
 * @param  lBufferSize: Buffer size in bytes
 * @param  plCrc: Computed CRC
 * @retval False if HAL_CRC_Init failed
 */
static bool APP_THREAD_OTA_HardwareCalculateCrc( CRC_HandleTypeDef * pstCrcHandle, uint32_t* plBuffer, uint32_t lBufferSize, uint32_t* plCrc )
{
  HAL_StatusTypeDef     eHalStatus;

  if ( HAL_CRC_GetState( pstCrcHandle ) == HAL_CRC_STATE_RESET )
  {
    /* Initialize the CRC */
    LOG_INFO_APP( "[OTA] Initializing the CRC module" );
    pstCrcHandle->Instance = CRC;
    pstCrcHandle->Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
    pstCrcHandle->Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
    pstCrcHandle->Init.CRCLength = CRC_POLYLENGTH_32B;
    pstCrcHandle->Init.InitValue = 0xFFFFFFFF;
    pstCrcHandle->Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_BYTE;
    pstCrcHandle->Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_ENABLE;
    pstCrcHandle->InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
    eHalStatus = HAL_CRC_Init( pstCrcHandle );
    if ( eHalStatus != HAL_OK )
    {
      LOG_ERROR_APP( "[OTA] Error, HAL_CRC_Init failed (0x%08X)", eHalStatus );
      return false;
    }
  }

  /* Compute the CRC */
  *plCrc = ~HAL_CRC_Calculate( pstCrcHandle, plBuffer, lBufferSize );

  return true;
}

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

    memset(&pOT_MessageInfo, 0, sizeof(pOT_MessageInfo));
    pOT_MessageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;

    if((aPeerAddress == NULL) && (aStringAddress != NULL))
    {
      LOG_INFO_APP("Use String Address : %s", aStringAddress);
      (void)otIp6AddressFromString(aStringAddress, &pOT_MessageInfo.mPeerAddr);
    }
    else
    {
      if (aPeerAddress != NULL)
      {
        LOG_DEBUG_APP("Use Peer Address");
        memcpy(&pOT_MessageInfo.mPeerAddr, aPeerAddress, sizeof(pOT_MessageInfo.mPeerAddr));
      }
      else
      {
        LOG_ERROR_APP("ERROR: Address string and Peer Address not defined");
        APP_THREAD_Error(ERR_THREAD_COAP_ADDRESS_NOT_DEFINED, 0);
      }
    }
  
    if(aCoapType == OT_COAP_TYPE_NON_CONFIRMABLE)
    {
      LOG_DEBUG_APP("\r\naCoapType == OT_COAP_TYPE_NON_CONFIRMABLE");
      error = otCoapSendRequest(PtOpenThreadInstance, pOT_Message, &pOT_MessageInfo, NULL, NULL);
    }
    
    if(aCoapType == OT_COAP_TYPE_CONFIRMABLE)
    {
      LOG_DEBUG_APP("\r\naCoapType == OT_COAP_TYPE_CONFIRMABLE");
      error = otCoapSendRequest(PtOpenThreadInstance, pOT_Message, &pOT_MessageInfo, aHandler, aContext);
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
 * @brief This function manages the response handler for provisioning.
 *
 * @param pContext  Context
 * @param pMessage message pointer
 * @param pMessageInfo message info pointer
 * @param Result error code
 * @retval None
 */
static void APP_THREAD_ProvisioningRespHandler(
    void                * pContext,
    otMessage           * pMessage,
    const otMessageInfo * pMessageInfo,
    otError               result)
{
 
  if (result == OT_ERROR_NONE)
  {
    if ((otMessageRead(pMessage, otMessageGetOffset(pMessage), &OT_Command, sizeof(OT_Command)) == sizeof(OT_Command)))
    {
      /* Retrieve the Message */
      if (otMessageRead(pMessage,
          otMessageGetOffset(pMessage) + sizeof(OT_Command),
          &OT_PeerAddress,
          sizeof(OT_PeerAddress)) != sizeof(OT_PeerAddress))
      {
        APP_THREAD_Error(ERR_THREAD_MESSAGE_READ, 0);
      }
      LOG_DEBUG_APP("\r -> Successfully retrieved Remote device address \n");
      }
      /* Notify provisioning OK */
      APP_THREAD_FuotaParameters();
  }
  else
  {
    LOG_ERROR_APP("WARNING: APP_THREAD_CoapDataRespHandler fail with ERROR code = %d",result);
  }
}

/**
 * @brief This function manages the response handler for FUOTA binary data.
 *
 * @param pContext  context
 * @param pMessage message pointer
 * @param pMessageInfo message info pointer
 * @param result error code
 * @retval None
 */
static void APP_THREAD_CoapRespHandlerFuotaSend(
    void * pContext,
    otMessage           * pMessage,
    const otMessageInfo * pMessageInfo,
    otError               result)
{
  if (result == OT_ERROR_NONE)
  {
    UTIL_SEQ_SetEvt(EVENT_TRANSFER_64BITS_DONE);
  }
  else
  {
    LOG_WARNING_APP("APP_THREAD_CoapRespHandlerFuotaSend : WARNING Result %d", result);
  }
}

/**
 * @brief Set FUOTA parameters for APP FW.
 * @param  None
 * @retval None
 */
static void APP_THREAD_FuotaSetParamApp(void)
{
  APP_THREAD_StatusTypeDef status;

  status = APP_THREAD_SetOtaContext();

  if (status == APP_THREAD_OK)
  {
    APP_THREAD_FuotaProvisioning();
  }
  else
  {
    LOG_WARNING_APP("WARNING: Setting Ota Context failed, do not send FUOTA PROVISIONING");
  }
}


/**
 * @brief Set Ota Context.
 * @param  None
 * @retval None
 */
static APP_THREAD_StatusTypeDef APP_THREAD_SetOtaContext(void)
{
  uint32_t binarySize;
  uint32_t binaryCrcAddress;
  bool     result = false;
  
  /* Set base address for FUOTA download */
  OtaContext.baseAddress = FUOTA_APP_FW_BINARY_ADDRESS;
  OtaContext.magicKeyword = FUOTA_MAGIC_KEYWORD_M33_APP;

  /* Set binary size */
  result = APP_THREAD_GetBinInfo(&binarySize);
  if (result == false)
  {
    LOG_ERROR_APP("ERROR : failed to get the binary info");
    return APP_THREAD_ERROR;
  }
  
  result = APP_THREAD_SetBinCrc(&binaryCrcAddress);
  if (result == false)
  {
    LOG_ERROR_APP("ERROR : failed to Set the binary Crc");
    return APP_THREAD_ERROR;
  }
  
  OtaContext.binarySize = binarySize;
  OtaContext.binaryCrcAddress = binaryCrcAddress;
  
  /* Return the function status */
  return APP_THREAD_OK;
}

/**
 * @brief Sends Provisioning request.
 * @param  None
 * @retval None
 */
static void APP_THREAD_FuotaProvisioning(void)
{
  LOG_INFO_APP("Send FUOTA PROVISONING request");

  uint8_t l_provisioning_data = 0x1;

  /* Send a MULTICAST CONFIRMABLE GET Request */
  APP_THREAD_CoapSendRequest(&OT_RessourceFuotaProvisioning,
      OT_COAP_TYPE_CONFIRMABLE,
      OT_COAP_CODE_GET,
      MULTICAST_FTD_MED,
      NULL,
      &l_provisioning_data,
      sizeof(l_provisioning_data),
      &APP_THREAD_ProvisioningRespHandler,
      NULL);
}

static uint32_t APP_THREAD_getCrCAddress(uint32_t binaryLastAddress)
{
  uint32_t reminder = (binaryLastAddress + sizeof(uint32_t)) % HW_FLASH_WIDTH ;
  /* Return the next aligned address*/
  return binaryLastAddress + sizeof(uint32_t) +(HW_FLASH_WIDTH - reminder);
}

/**
 * @brief  writing Word from internal RAM cache to flash
 * @param  lFlashAddress: Address in flash
 * @param  pcFirmwareBuffer: RAM buffer to write to flash

 * @retval Application status code
 */
static inline APP_THREAD_StatusTypeDef APP_THREAD_OTA_WriteWordToFlash( uint32_t lFlashAddress, uint32_t Data)
{
  HAL_StatusTypeDef             eHalStatus;
  APP_THREAD_StatusTypeDef      eStatus = APP_THREAD_OK;
  
  /* Write to Flash Memory one Word only but the FLASH  can not write less than 4 wods   */
  uint32_t DataBuffer[4] = {Data, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
 
  HAL_FLASH_Unlock();

  eHalStatus = HAL_FLASH_Program( FLASH_TYPEPROGRAM_QUADWORD, lFlashAddress , (uint32_t)&DataBuffer[0] );
  if ( eHalStatus != HAL_OK )
  {
    LOG_ERROR_APP( "[OTA] Error, HAL_FLASH_Program failed (0x%02X).", eHalStatus );
    eStatus = APP_THREAD_ERROR;
  }
  
  HAL_FLASH_Lock();

  /* Read back the value for verification */
  if ( memcmp( (void*)(lFlashAddress), DataBuffer, HW_FLASH_WIDTH ) != 0x00u )
  {
    LOG_ERROR_APP( "[OTA] Error, flash verifaction failed." );
    eStatus = APP_THREAD_ERROR;
  }

  if (eStatus != APP_THREAD_OK)
  {
    return eStatus;
  }
  return eStatus;
}

static bool APP_THREAD_GetBinInfo(uint32_t * fuotaBinarySize)
{
  uint32_t       lBinaryMagicKeyword;
  const uint32_t lOtaTagAddress  =  OtaContext.baseAddress + stOtaServerInfo.lOtaTagOffset;
  const uint32_t lMagicKeywordAddress = *(uint32_t *)lOtaTagAddress;
  
  uint32_t lCrcAddress;
  const uint32_t lFlashEndAddress = FLASH_BASE + FLASH_SIZE;
  uint32_t       lCrc;
  uint32_t       *lBinaryEndAddress;

  lCrcAddress = APP_THREAD_getCrCAddress(lMagicKeywordAddress);
  
    /* Check if the magic keyword address is inside the flash region */
  if ( lMagicKeywordAddress < OtaContext.baseAddress || lMagicKeywordAddress > lFlashEndAddress - sizeof( lBinaryMagicKeyword ) )
  {
    LOG_ERROR_APP( "[OTA] Error, magic keyword address is outside the flash region (0x%08X).", lMagicKeywordAddress );
    return false;
  }

  /* Check if the magic keyword is correct */
  lBinaryMagicKeyword = *(uint32_t*)(lMagicKeywordAddress);
  if ( lBinaryMagicKeyword != OtaContext.magicKeyword )
  {
    LOG_ERROR_APP( "[OTA] Error, magic keyword is incorrect (0x%08X != 0x%08X ).", lBinaryMagicKeyword, OtaContext.magicKeyword );
    return false;
  }
 

  lBinaryEndAddress = (uint32_t *)((uint32_t)lCrcAddress + sizeof(lCrc));
  *fuotaBinarySize = (uint32_t)lBinaryEndAddress - OtaContext.baseAddress;


  return true;
}

static bool APP_THREAD_SetBinCrc(uint32_t *fuotaBinaryCrcAddress)
{
  const uint32_t lOtaTagAddress  =  OtaContext.baseAddress + stOtaServerInfo.lOtaTagOffset;
  const uint32_t lMagicKeywordAddress = *(uint32_t *)lOtaTagAddress;
  uint32_t lCrcAddress;
  uint32_t lCrc;
  const uint32_t lFlashEndAddress = FLASH_BASE + FLASH_SIZE;

  lCrcAddress = APP_THREAD_getCrCAddress(lMagicKeywordAddress);
  
  
    /* Check if the CRC is inside the flash region */
  if ( lCrcAddress < OtaContext.baseAddress  || lCrcAddress > lFlashEndAddress - sizeof(lCrc) )
  {
    LOG_ERROR_APP( "[OTA] Error, CRC address is outside the flash region (0x%08X ).", lCrcAddress );
    return false;
  }
  
  if ( *(uint32_t*) lCrcAddress == 0xffffffff )
  {
     /* Calculate the Image CRC and attach it at the end of the image*/
    if ( APP_THREAD_OTA_HardwareCalculateCrc( &stOtaCrcHandle, (uint32_t *)OtaContext.baseAddress, lCrcAddress - OtaContext.baseAddress, &lCrc ) == false )
    {
      LOG_ERROR_APP( "[OTA] Error, CRC calculation failed." );
      return false;
    }
  
    if (APP_THREAD_OTA_WriteWordToFlash(lCrcAddress, lCrc) != APP_THREAD_OK)
    {
      LOG_ERROR_APP( "[OTA] Error, Writting CRC to Flash failed." );
      return false;
    }
    
  }
  else
  {
    lCrc = *(uint32_t*) lCrcAddress;
  }
  
  *fuotaBinaryCrcAddress =  lCrcAddress;
  return true;
}

/**
 * @brief Task associated to FUOTA parameters to be sent.
 * @param  None
 * @retval None
 */
static void APP_THREAD_FuotaParameters(void)
{
  LOG_INFO_APP("FUOTA PROVISIONING OK, Set FUOTA PARAMETERS");

  LOG_INFO_APP("\r Binary size = %d bytes", OtaContext.binarySize);
  LOG_INFO_APP("\r Magic keyword = 0x%x", OtaContext.magicKeyword);
  LOG_INFO_APP("\r Binary CRC Address = 0x%x", OtaContext.binaryCrcAddress);
  LOG_INFO_APP("\r Base address = 0x%x", OtaContext.baseAddress);

  /* Send a CONFIRMABLE PUT Request */
  APP_THREAD_CoapSendRequest(&OT_RessourceFuotaParameters,
      OT_COAP_TYPE_CONFIRMABLE,
      OT_COAP_CODE_PUT,
      NULL,
      &OT_PeerAddress,
      (uint8_t*)&OtaContext,
      sizeof(OtaContext),
      &APP_THREAD_CoapRespHandlerFuotaParameters,
      NULL);
}

/**
 * @brief Task associated to FUOTA binary data sending.
 * @param  None
 * @retval None
 */
static void APP_THREAD_FuotaSend(void)
{
  uint32_t flash_current_offset = 0x0;
  bool binary_transfer_on_going = TRUE;
  uint32_t l_debug_count = 0;
  uint32_t l_current_index_progress = 0;
  uint32_t FuotaProgressArray[100] = {0};
  uint32_t FuotaTransferArraySize = 0;
  uint32_t l_start_transfer_time = 0;
  uint32_t l_end_transfer_time = 0;
  double l_transfer_time = 0;
  double l_transfer_throughput = 0;
  uint8_t l_Fuota_progress_multiplied = 0;

  LOG_INFO_APP("FUOTA PARAMETERS SET, START FUOTA BINARY TRANSFER");

  /* Number of elements in OtaContext.binarySize */
  FuotaTransferArraySize = OtaContext.binarySize / FUOTA_PAYLOAD_SIZE;
  LOG_DEBUG_APP("\r -> %d elements of %d bytes to be transferred", FuotaTransferArraySize, FUOTA_PAYLOAD_SIZE);
  if(FuotaTransferArraySize < 100)
  {
    l_Fuota_progress_multiplied = 1;
    FuotaTransferArraySize = FuotaTransferArraySize * 10;
  }
  for (int index = 1; index < 100; index++)
  {
    FuotaProgressArray[index] = (FuotaTransferArraySize / 100) * index;
  }

  l_start_transfer_time = HAL_GetTick();

  while(binary_transfer_on_going == TRUE){

    /* Get data to be sent */
    /* Read data from flash memory */
    memcpy(FuotaTransferArray, (uint8_t*)(OtaContext.baseAddress + flash_current_offset), FUOTA_PAYLOAD_SIZE);

    /* Send a CONFIRMABLE PUT Request */
    APP_THREAD_CoapSendRequest(&OT_RessourceFuotaSend,
        OT_COAP_TYPE_CONFIRMABLE,
        OT_COAP_CODE_PUT,
        NULL,
        &OT_PeerAddress,
        (uint8_t*)&FuotaTransferArray,
        FUOTA_PAYLOAD_SIZE,
        &APP_THREAD_CoapRespHandlerFuotaSend,
        NULL);

    UTIL_SEQ_WaitEvt(EVENT_TRANSFER_64BITS_DONE);
    
    flash_current_offset += FUOTA_PAYLOAD_SIZE;
    
    if (flash_current_offset >= OtaContext.binarySize)
    {
      binary_transfer_on_going = FALSE;
    }

    /* Display Transfer Progress */
    if(l_debug_count == FuotaProgressArray[l_current_index_progress+1])
    {
      l_current_index_progress += 1;
      if(l_Fuota_progress_multiplied == 1)
      {
        LOG_INFO_APP("FUOTA Transfer %d%...", l_current_index_progress*10);
      }
      else
      {
        LOG_INFO_APP("FUOTA Transfer %d%...", l_current_index_progress);
      }
    }

    l_debug_count++;
  }

  l_end_transfer_time = HAL_GetTick();
  l_transfer_time = (double)(l_end_transfer_time - l_start_transfer_time) / 1000;
  l_transfer_throughput = (((double)OtaContext.binarySize/l_transfer_time) / 1000) * 8;

  LOG_INFO_APP("**************************************************************");
  LOG_INFO_APP(" FUOTA_SERVER : END OF TRANSFER COMPLETED");
  LOG_INFO_APP("  - Payload size = %d bytes", FUOTA_PAYLOAD_SIZE);
  LOG_INFO_APP("  - Transfer time = %.2f seconds", l_transfer_time);
  LOG_INFO_APP("  - Average throughput = %.2f kbit/s", l_transfer_throughput);
  LOG_INFO_APP("**************************************************************");

  /* Notify end of transfer to Thread_Ota ? */
}

/**
 * @brief Task associated to the push button.
 * @param  None
 * @retval None
 */
static void APP_THREAD_FuotaReboot(void)
{
  uint32_t l_data = 0x1;
  LOG_INFO_APP("Send a request to current OTA application to reboot on Thread_OTA_Client");

  /* Send a CONFIRMABLE PUT Request */
  APP_THREAD_CoapSendRequest(&OT_RessourceFuotaReboot,
      OT_COAP_TYPE_CONFIRMABLE,
      OT_COAP_CODE_PUT,
      MULTICAST_FTD_MED,
      NULL,
      (uint8_t*)&l_data,
      sizeof(l_data),
      &APP_THREAD_CoapRespHandlerFuotaReboot,
      NULL);

  UTIL_SEQ_WaitEvt(EVENT_FUOTA_REBOOT_RESP_DONE);

  LOG_INFO_APP("Reboot request ACK from OTA application");
}

/**
 * @brief This function manages the response handler for the request of reboot on Thread_Ota.
 *
 * @param pContext  Context
 * @param pMessage message pointer
 * @param pMessageInfo message info pointer
 * @param result error code
 * @retval None
 */
static void APP_THREAD_CoapRespHandlerFuotaReboot(
    void * pContext,
    otMessage           * pMessage,
    const otMessageInfo * pMessageInfo,
    otError               result)
{
  if (result == OT_ERROR_NONE)
  {
    UTIL_SEQ_SetEvt(EVENT_FUOTA_REBOOT_RESP_DONE);
  }
  else
  {
    LOG_WARNING_APP("APP_THREAD_CoapRespHandlerFuotaReboot : WARNING Result %d", result);
  }
}

/**
 * @brief This function manages the response handler for the request of reboot on Thread_Ota.
 *
 * @param pContext  Context
 * @param pMessage message pointer
 * @param pMessageInfo message info pointer
 * @param result error code
 * @retval None
 */
static void APP_THREAD_CoapRespHandlerFuotaParameters( void * pContext,
                                                        otMessage           * pMessage,
                                                        const otMessageInfo * pMessageInfo,
                                                        otError               result)
{
  if (result == OT_ERROR_NONE)
  {
    /* Read Message */
    if (otMessageRead(pMessage, otMessageGetOffset(pMessage), &OT_Command, sizeof(OT_Command)) == sizeof(OT_Command))
    {
      if (OT_Command == APP_THREAD_OK)
      {
        LOG_DEBUG_APP("FUOTA PARAMETERS: Correct ACK received");
        UTIL_SEQ_SetTask(1u << CFG_TASK_FUOTA_SEND, TASK_PRIO_FUOTA_SEND);
      }
      else if (OT_Command == APP_THREAD_ERROR)
      {
        LOG_WARNING_APP("FUOTA PARAMETERS: Bad ACK received");
        LOG_ERROR_APP("\r -> STOP FUOTA process!");
      }
      else
      {
        LOG_WARNING_APP("FUOTA PARAMETERS: Unrecognized ACK received");
        LOG_ERROR_APP("\r -> STOP FUOTA process!");
      }
    }
    else
    {
      LOG_ERROR_APP("WARNING : APP_THREAD_CoapRespHandlerFuotaParameters otMessageRead failed!");
    }
  }
  else
  {
    LOG_WARNING_APP("APP_THREAD_CoapRespHandlerFuotaReboot : WARNING Result %d", result);
  }
}

static void APP_THREAD_AppInit(void)
{
  LOG_INFO_APP("Thread_Ota_Server Application");
  
  /* disable OpenThread Logs */
  setDebugLevel(0);
}


/**
 * @brief Task associated to the push button 1.
 * @param  None
 * @retval None
 */
void APP_BSP_Button1Action(void)
{
  LOG_INFO_APP("Button 1 pressed");
  APP_THREAD_FuotaSetParamApp();
}

/**
 * @brief Task associated to the push button 2.
 * @param  None
 * @retval None
 */
void APP_BSP_Button3Action(void)
{
  LOG_INFO_APP("Button3 pressed");
  APP_THREAD_FuotaReboot();
}


/* USER CODE END FD_LOCAL_FUNCTIONS */

