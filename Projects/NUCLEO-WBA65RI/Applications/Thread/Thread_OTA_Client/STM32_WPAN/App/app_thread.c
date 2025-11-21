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
#include "udp.h"
#include "app_bsp.h"
#include "logging_stm32wba.h"

void BootModeCheck(void);
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

/* OTA Client information */
struct APP_THREAD_OtaClientInfo_t
{
  uint32_t              otaTagOffset; /* The OTA tag offset from the base address in flash (the address of the magic keyword is located here) */
  uint32_t              magicKeyword; /* The image magic keyword (terminating word) */
};

typedef struct
{
  uint32_t binarySize;
  uint32_t binaryCrcAddress;
  uint32_t baseAddress;
  uint32_t magicKeyword;
} APP_THREAD_OtaContext_t;

typedef void (*fct_t)(void);

/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/
#define C_PANID                 0xBA98U
#define C_CHANNEL_NB            16U
#define C_CCA_THRESHOLD         (-70)

/* USER CODE BEGIN PD */
#define LED_TOGGLE_TIMING_MS     1000u

/* Define the start address where the application shall be located */
#define CFG_APP_START_SECTOR_INDEX          (0x80u) /* 0x80 is the first sector in the second bank*/

/* Each sector is 8 KB, so the downloaded image size will be  means 512 KB*/
#define IMAGE_SECTORS_COUNT                 (0x40u) 

/* Define list of reboot reason */
#define FUOTA_MAGIC_KEYWORD_M33_APP         0xBF806133u       /* Keyword found at the end of Zigbee Ota file for M33 Application Processor binary */

#define HW_FLASH_WIDTH                      (16u)   // Write Lentgh in Bytes (128 bits = 4 words) 

#define C_RESSOURCE_FUOTA_PROVISIONING     "FUOTA_PROVISIONING"
#define C_RESSOURCE_FUOTA_PARAMETERS       "FUOTA_PARAMETERS"
#define C_RESSOURCE_FUOTA_SEND             "FUOTA_SEND"

#define FUOTA_NUMBER_WORDS_64BITS           50
#define FUOTA_PAYLOAD_SIZE                  FUOTA_NUMBER_WORDS_64BITS * 8

#define RAM_FIRMWARE_BUFFER_SIZE            1024u

#define FUOTA_APP_FW_BINARY_ADDRESS         (FLASH_BASE + ( CFG_APP_START_SECTOR_INDEX * FLASH_PAGE_SIZE))     /* Address for Application Processor FW Update */

#define FUOTA_APP_FW_BINARY_MAX_SIZE        (IMAGE_SECTORS_COUNT * FLASH_PAGE_SIZE)

#define FUOTA_M33_APP_OTA_TAG_OFFSET        0x200u            /* Offset of the OTA tag from the base address in flash for M33 Applciation Processor binary */
#define FUOTA_M33_APP_OTA_TAG_ADDRESS       (FUOTA_APP_FW_BINARY_ADDRESS + FUOTA_M33_APP_OTA_TAG_OFFSET)       /* Address of the OTA tag in flash for M33 Application Processor binary */

/**
 * Define list of reboot reason
 */
#define CFG_REBOOT_ON_DOWNLOADED_FW         0x0         /* Reboot on the downloaded Firmware */
#define CFG_REBOOT_ON_OTA_CLIENT_FW         0xaau       /* Reboot on Thread_OTA_Client FW */

#define OTA_RESET_WAIT_TIME                 100

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
static inline void APP_THREAD_DeleteSectors( uint32_t lBaseAddress, uint32_t lBinarySize );
static bool APP_THREAD_OTA_HardwareCalculateCrc( CRC_HandleTypeDef * pstCrcHandle, uint32_t* plBuffer, uint32_t lBufferSize, uint32_t* plCrc );

static void APP_THREAD_CoapReqHandlerFuotaProvisioning(
    void                * pContext,
    otMessage           * pMessage,
    const otMessageInfo * pMessageInfo);
static otError APP_THREAD_ProvisioningRespSend(otMessage    * pMessage,
    const otMessageInfo * pMessageInfo);

static void APP_THREAD_CoapReqHandlerFuota(
    void                * pContext,
    otMessage           * pMessage,
    const otMessageInfo * pMessageInfo);
static void APP_THREAD_CoapSendDataResponseFuota(otMessage    * pMessage,
    const otMessageInfo * pMessageInfo);

static void APP_THREAD_CoapReqHandlerFuotaParameters(
    void                 * pContext,
    otMessage            * pMessage,
    const otMessageInfo  * pMessageInfo);
static void APP_THREAD_CoapSendRespFuotaParameters(otMessage    * pMessage,
    const otMessageInfo * pMessageInfo,
    uint8_t * pData);

static void APP_THREAD_PerformReset( void * arg );
static void APP_THREAD_TimingElapsed(void * args);
static APP_THREAD_StatusTypeDef APP_THREAD_CheckDeviceCapabilities(void);
static bool APP_THREAD_ValidateOta(void);
static void APP_THREAD_FuotaInit(void);

static void JumpFwApp( void );
static bool CheckFwAppValidity( void );
static void JumpSelectionOnPowerUp( void );

/* USER CODE END PFP */

/* Private variables -----------------------------------------------*/
static otInstance * PtOpenThreadInstance;

/* USER CODE BEGIN PV */
static otCoapResource OT_RessourceFuotaProvisioning = {C_RESSOURCE_FUOTA_PROVISIONING, APP_THREAD_CoapReqHandlerFuotaProvisioning,"myCtxProvisioning", NULL};
static otCoapResource OT_RessourceFuotaParameters = {C_RESSOURCE_FUOTA_PARAMETERS, APP_THREAD_CoapReqHandlerFuotaParameters, "myCtxReqFuotaParamHdl", NULL};
static otCoapResource OT_RessourceFuotaSend = {C_RESSOURCE_FUOTA_SEND, APP_THREAD_CoapReqHandlerFuota,"myCtxReqFuotaHdl", NULL};

static uint8_t OT_Command = 0;
static otMessage* pOT_MessageResponse = NULL;

static UTIL_TIMER_Object_t TimerID;

static uint64_t FuotaTransferArray[FUOTA_NUMBER_WORDS_64BITS] = {0};
static APP_THREAD_OtaContext_t OtaContext;

static struct APP_THREAD_OtaClientInfo_t  stOtaClientInfo = 
{
  .otaTagOffset = FUOTA_M33_APP_OTA_TAG_OFFSET,
  .magicKeyword = FUOTA_MAGIC_KEYWORD_M33_APP,
};

UTIL_TIMER_Object_t resetTimerId;
static uint32_t gFlashCurrentOffset = 0;

/* OTA CRC calculation */
CRC_HandleTypeDef    stOtaCrcHandle;
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
  /* process the tasklet */
  otTaskletsProcess(PtOpenThreadInstance);

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
  
  otCoapAddResource(PtOpenThreadInstance, &OT_RessourceFuotaParameters);

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

/**
 * @brief Check if the OTA valid or no.
 * @param  None
 * @retval None
 */
static bool APP_THREAD_ValidateOta(void)
{
  uint32_t       lBinaryMagicKeyword;
  const uint32_t lOtaTagAddress  =  OtaContext.baseAddress + stOtaClientInfo.otaTagOffset;
  const uint32_t lMagicKeywordAddress = *(uint32_t *)lOtaTagAddress;
  
  const uint32_t lCrcAddress = OtaContext.binaryCrcAddress;
  const uint32_t lFlashEndAddress = FLASH_BASE + FLASH_SIZE;
  uint32_t       lCrc;
  uint32_t       lBinaryCrc;
  uint32_t       *lBinaryEndAddress;

    /* Check if the magic keyword address is inside the flash region */
  if ( lMagicKeywordAddress < OtaContext.baseAddress || lMagicKeywordAddress > lFlashEndAddress - sizeof( lBinaryMagicKeyword ) )
  {
    LOG_ERROR_APP( "[OTA] Error, magic keyword address is outside the flash region (0x%08X).", lMagicKeywordAddress );
    return false;
  }

  /* Check if the CRC is inside the flash region */
  if ( lCrcAddress < OtaContext.baseAddress  || lCrcAddress > lFlashEndAddress - sizeof( lBinaryCrc ) )
  {
    LOG_ERROR_APP( "[OTA] Error, CRC address is outside the flash region (0x%08X ).", lCrcAddress );
    return false;
  }

  /* Check if the magic keyword is correct */
  lBinaryMagicKeyword = * ( uint32_t * )( lMagicKeywordAddress );
  if ( lBinaryMagicKeyword != stOtaClientInfo.magicKeyword )
  {
    LOG_ERROR_APP( "[OTA] Error, magic keyword is incorrect (0x%08X != 0x%08X ).", lBinaryMagicKeyword, OtaContext.magicKeyword );
    return false;
  }
 
  
  /* Check if the CRC is correct */
  if ( APP_THREAD_OTA_HardwareCalculateCrc( &stOtaCrcHandle, (uint32_t *)OtaContext.baseAddress, lCrcAddress - OtaContext.baseAddress, &lCrc ) == false )
  {
    LOG_ERROR_APP( "[OTA] Error, CRC calculation failed." );
    return false;
  }
  lBinaryCrc = * ( uint32_t * )( lCrcAddress );
  if ( lBinaryCrc != lCrc )
  {
    LOG_ERROR_APP( "[OTA] Error, CRC is incorrect (0x%08X  != 0x%08X ).", lBinaryCrc, lCrc );
    return false;
  }
  
  lBinaryEndAddress = (uint32_t *)((uint32_t)lCrcAddress + sizeof( lBinaryCrc ));
  /* check binary size and CRC */
  if (OtaContext.binarySize != ((uint32_t)lBinaryEndAddress - OtaContext.baseAddress) )
  {
    LOG_ERROR_APP( "[OTA] Error, Binary size is not correct (0x%08X  != 0x%08X ).", OtaContext.binarySize, ((uint32_t)lBinaryEndAddress - OtaContext.baseAddress) );
    return false;
  }

  return true;
}

static void APP_THREAD_FuotaInit(void)
{
  /* Erase the sectors used to download the image */
  LOG_INFO_APP( "[OTA] Deleting flash sectors from 0x%08X with size 0x%08X", FUOTA_APP_FW_BINARY_ADDRESS, FUOTA_APP_FW_BINARY_MAX_SIZE);

  /* Delete all the sector needed for the image */
  APP_THREAD_DeleteSectors(FUOTA_APP_FW_BINARY_ADDRESS, FUOTA_APP_FW_BINARY_MAX_SIZE);

  UTIL_TIMER_Create( &TimerID, 0, UTIL_TIMER_PERIODIC, APP_THREAD_TimingElapsed, NULL ); 
  
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
  
  if (HAL_CRC_DeInit(pstCrcHandle) != HAL_OK )
  {
    LOG_ERROR_APP( "[OTA] Error, CRC can not be de-initialized.");
    return false;
  }
  
  return true;
}

/**
 * @brief  Deleting sectors helper
 * @param  None
 * @retval None
 */
static inline void APP_THREAD_DeleteSectors( uint32_t lBaseAddress, uint32_t lBinarySize )
{
  FLASH_EraseInitTypeDef        stEraseInit;
  uint32_t                      lPageError;

  /* Determine the page, the number of pages to delete, etc. */
  stEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
  stEraseInit.Banks = FLASH_BANK_2;
  /* for each bank the page numbers goes from 0 to 127*/
  stEraseInit.Page = (( lBaseAddress - FLASH_BASE ) / FLASH_PAGE_SIZE) % FLASH_PAGE_NB;
  stEraseInit.NbPages = lBinarySize / FLASH_PAGE_SIZE;
  stEraseInit.NbPages--;
  if ( lBinarySize % FLASH_PAGE_SIZE != 0 )
  {
    stEraseInit.NbPages++;
  }

  LOG_DEBUG_APP( "stEraseInit" );
  LOG_DEBUG_APP( ".TypeErase     = 0x%08X", stEraseInit.TypeErase );
  LOG_DEBUG_APP( ".Page          = 0x%08X", stEraseInit.Page );
  LOG_DEBUG_APP( ".NbPages       = %u", stEraseInit.NbPages );

  /* Delete the flash sectors */
  HAL_FLASH_Unlock();
  HAL_FLASHEx_Erase(&stEraseInit, &lPageError);
  HAL_FLASH_Lock();
}

/**
 * @brief Handler called when the server receives a COAP request.
 *
 * @param pContext : Context
 * @param pMessage : Message
 * @param pMessageInfo : Message information
 * @retval None
 */
static void APP_THREAD_CoapReqHandlerFuotaProvisioning(
    void                 * pContext,
    otMessage            * pMessage,
    const otMessageInfo  * pMessageInfo)
{
  LOG_INFO_APP(" Received CoAP request on FUOTA_PROVISIONING resource");

  if (otCoapMessageGetType(pMessage) == OT_COAP_TYPE_CONFIRMABLE)
  {
    if (APP_THREAD_ProvisioningRespSend(pMessage, pMessageInfo) != OT_ERROR_NONE)
    {
      APP_THREAD_Error(ERR_THREAD_PROVISIONING_RESP, 0);
    }
  }
}

/**
 * @brief This function acknowledges the data reception by sending an ACK
 *    back to the sender.
 * @param  pRequestHeader coap header
 * @param  pMessageInfo message info pointer
 * @retval None
 */
static otError APP_THREAD_ProvisioningRespSend(otMessage * pMessage,
    const otMessageInfo * pMessageInfo)
{
  otError  error = OT_ERROR_NONE;

  do{
    LOG_INFO_APP("Provisioning: Send CoAP response");

    pOT_MessageResponse = otCoapNewMessage(NULL, NULL);
    if (pOT_MessageResponse == NULL)
    {
      LOG_WARNING_APP("WARNING : pOT_MessageResponse = NULL ! -> exit now");
      break;
    }

    (void)otCoapMessageInitResponse(pOT_MessageResponse,
        pMessage,
        OT_COAP_TYPE_ACKNOWLEDGMENT,
        OT_COAP_CODE_CHANGED);

    (void)otCoapMessageSetPayloadMarker(pOT_MessageResponse);

    error = otMessageAppend(pOT_MessageResponse, &OT_Command, sizeof(OT_Command));
    if (error != OT_ERROR_NONE)
    {
      APP_THREAD_Error(ERR_THREAD_COAP_APPEND_MSG, error);
    }

    error = otMessageAppend(pOT_MessageResponse, otThreadGetMeshLocalEid(PtOpenThreadInstance), sizeof(otIp6Address));
    if (error != OT_ERROR_NONE)
    {
      APP_THREAD_Error(ERR_THREAD_COAP_ALLOC_MSG, error);
      break;
    }

    error = otCoapSendResponse(PtOpenThreadInstance, pOT_MessageResponse, pMessageInfo);
    if (error != OT_ERROR_NONE && pOT_MessageResponse != NULL)
    {
      otMessageFree(pOT_MessageResponse);
      APP_THREAD_Error(ERR_THREAD_COAP_DATA_RESPONSE,error);
    }
  }while(false);

  return error;
}

/**
 * @brief Handler called when the server receives a COAP request.
 *
 * @param pContext : Context
 * @param pHeader : Header
 * @param pMessage : Message
 * @param pMessageInfo : Message information
 * @retval None
 */
static void APP_THREAD_CoapReqHandlerFuotaParameters(
    void                 * pContext,
    otMessage            * pMessage,
    const otMessageInfo  * pMessageInfo)
{
  if (otMessageRead(pMessage, otMessageGetOffset(pMessage), &OtaContext, sizeof(OtaContext)) != sizeof(OtaContext))
  {
    APP_THREAD_Error(ERR_THREAD_MESSAGE_READ, 0);
  }

  /* Display Ota_Context values */
  LOG_INFO_APP("FUOTA_PARAMETERS: Binary Size = 0x%x", OtaContext.binarySize);
  LOG_INFO_APP("FUOTA_PARAMETERS: Address = 0x%x", OtaContext.baseAddress);
  LOG_INFO_APP("FUOTA_PARAMETERS: Magic Keyword = 0x%x", OtaContext.magicKeyword);

  /* Check if Device can be updated with Fuota Server request */
  if (APP_THREAD_CheckDeviceCapabilities() == APP_THREAD_OK)
  {
    OT_Command = APP_THREAD_OK;
    UTIL_TIMER_StartWithPeriod(&TimerID, LED_TOGGLE_TIMING_MS);
  }
  else
  {
    OT_Command = APP_THREAD_ERROR;
    LOG_WARNING_APP("WARNING: Current Device capabilities cannot handle FUOTA. Check memory size available!");
  }
  /* If Message is Confirmable, send response */
  if (otCoapMessageGetType(pMessage) == OT_COAP_TYPE_CONFIRMABLE)
  {
    APP_THREAD_CoapSendRespFuotaParameters(pMessage, pMessageInfo, &OT_Command);
  }
}

static APP_THREAD_StatusTypeDef APP_THREAD_CheckDeviceCapabilities(void)
{
  APP_THREAD_StatusTypeDef status = APP_THREAD_OK;
  
  /* Check that the OTA image can fit in the available Flash space */
  if (OtaContext.baseAddress == FUOTA_APP_FW_BINARY_ADDRESS)
  {
    if (OtaContext.binarySize > FUOTA_APP_FW_BINARY_MAX_SIZE)
    {
      status = APP_THREAD_ERROR;
      LOG_ERROR_APP("WARNING: Not enough Free Flash Memory available to download binary from Server!");
    }
    else
    {
      LOG_DEBUG_APP("Device contains enough Flash Memory to download binary");
    }
  }
  else
  {
    status = APP_THREAD_ERROR;
    LOG_ERROR_APP("WARNING: Invalid OTA Image Base address");
  }

  return status;
}

/**
 * @brief  OTA client writing firmware data from internal RAM cache to flash
 * @param  lFlashAddress: Address in flash
 * @param  pcFirmwareBuffer: RAM buffer to write to flash
 * @param  lFirmwareBufferSize: RAM buffer size
 * @retval Application status code
 */
static inline APP_THREAD_StatusTypeDef APP_THREAD_OTA_ClientWriteFirmwareData( uint32_t lFlashAddress, uint8_t * pcFirmwareBuffer, uint32_t lFirmwareBufferSize )
{
  uint32_t                      lIndex;
  HAL_StatusTypeDef             eHalStatus;
  APP_THREAD_StatusTypeDef      eStatus = APP_THREAD_OK;

  for( lIndex = 0; lIndex < lFirmwareBufferSize; lIndex += HW_FLASH_WIDTH )
  {
    /* Write to Flash Memory */
    uint32_t    lWord1 = * ( ( uint32_t *)( &pcFirmwareBuffer[lIndex + 0 * sizeof( uint32_t )] ) );
    uint32_t    lWord2 = * ( ( uint32_t *)( &pcFirmwareBuffer[lIndex + 1 * sizeof( uint32_t )] ) );
    uint32_t    lWord3 = * ( ( uint32_t *)( &pcFirmwareBuffer[lIndex + 2 * sizeof( uint32_t )] ) );
    uint32_t    lWord4 = * ( ( uint32_t *)( &pcFirmwareBuffer[lIndex + 3 * sizeof( uint32_t )] ) );
    LOG_DEBUG_APP( "0x%08X | 0x%08X | 0x%08X | 0x%08", lWord1, lWord2, lWord3, lWord4 );

    HAL_FLASH_Unlock();

    eHalStatus = HAL_FLASH_Program( FLASH_TYPEPROGRAM_QUADWORD, lFlashAddress + lIndex, ( uint32_t )&pcFirmwareBuffer[lIndex] );
    if ( eHalStatus != HAL_OK )
    {
      LOG_ERROR_APP( "[OTA] Error, HAL_FLASH_Program failed (0x%02X).", eHalStatus );
      eStatus = APP_THREAD_ERROR;
    }
    
    HAL_FLASH_Lock();

    /* Read back the value for verification */
    if ( memcmp( ( void * )( lFlashAddress + lIndex ), &pcFirmwareBuffer[lIndex], HW_FLASH_WIDTH ) != 0x00u )
    {
      LOG_ERROR_APP( "[OTA] Error, flash verifaction failed." );
      eStatus = APP_THREAD_ERROR;
    }

    if (eStatus != APP_THREAD_OK)
    {
      return eStatus;
    }
  }

  return eStatus;
}

static void APP_THREAD_ResetFuotaProcess(void)
{
   gFlashCurrentOffset = 0;
   UTIL_TIMER_Stop(&TimerID);
   BSP_LED_Off(LED_BLUE);
   
   LOG_INFO_APP( "[OTA] Deleting flash sectors from 0x%08X to 0x%08X", FUOTA_APP_FW_BINARY_ADDRESS, FUOTA_APP_FW_BINARY_ADDRESS + FUOTA_APP_FW_BINARY_MAX_SIZE);

   APP_THREAD_DeleteSectors(FUOTA_APP_FW_BINARY_ADDRESS, FUOTA_APP_FW_BINARY_MAX_SIZE);
}

/**
 * @brief Handler called when the server receives a COAP request.
 *
 * @param pContext : Context
 * @param pHeader : Header
 * @param pMessage : Message
 * @param pMessageInfo : Message information
 * @retval None
 */
static void APP_THREAD_CoapReqHandlerFuota(
    void                 *pContext,
    otMessage            * pMessage,
    const otMessageInfo  * pMessageInfo)
{
  bool     lEndFullBinTransfer = FALSE;
  
  if (OtaContext.baseAddress == 0)
  {
    return;
  }
  
  if (otMessageRead(pMessage, otMessageGetOffset(pMessage), &FuotaTransferArray, FUOTA_PAYLOAD_SIZE) != FUOTA_PAYLOAD_SIZE)
  {
    APP_THREAD_Error(ERR_THREAD_MESSAGE_READ, 0);
  }
  
  if(OtaContext.baseAddress + gFlashCurrentOffset > (FLASH_BASE + FLASH_SIZE)) 
  {
    /**
     * The address is not valid
     */
    /* Invalid FUOTA */
    LOG_ERROR_APP("ERROR : Invalid keyword ");
    APP_THREAD_ResetFuotaProcess();

    return ;
  }
  else
  { 
    APP_THREAD_OTA_ClientWriteFirmwareData(OtaContext.baseAddress + gFlashCurrentOffset, (uint8_t*)FuotaTransferArray, FUOTA_PAYLOAD_SIZE);
    gFlashCurrentOffset += FUOTA_PAYLOAD_SIZE;
    
    if (gFlashCurrentOffset >= OtaContext.binarySize)
    {
      lEndFullBinTransfer = TRUE;
    }
  }

  /* If Message is Confirmable, send response */
  if (otCoapMessageGetType(pMessage) == OT_COAP_TYPE_CONFIRMABLE)
  {
    APP_THREAD_CoapSendDataResponseFuota(pMessage, pMessageInfo);
  }

  if(lEndFullBinTransfer == TRUE)
  {
    if (APP_THREAD_ValidateOta() == TRUE)
    {
      /* Wait until the Coap Response is sent to the OTA server */
      UTIL_TIMER_StartWithPeriod( &resetTimerId, OTA_RESET_WAIT_TIME );
    }
    else
    {
      LOG_ERROR_APP("ERROR : OTA Image is not Valid ");
      APP_THREAD_ResetFuotaProcess();
    }
    
  }
}

/**
 * @brief This function acknowledges the data reception by sending an ACK
 *    back to the sender.
 * @param  pRequestHeader coap header
 * @param  pMessageInfo message info pointer
 * @retval None
 */
static void APP_THREAD_CoapSendDataResponseFuota(otMessage    * pMessage,
    const otMessageInfo * pMessageInfo)
{
  otError  error = OT_ERROR_NONE;
  do{
    pOT_MessageResponse = otCoapNewMessage(NULL, NULL);
    if (pOT_MessageResponse == NULL)
    {
      LOG_WARNING_APP("WARNING : pOT_MessageResponse = NULL ! -> exit now");
      break;
    }

    (void)otCoapMessageInitResponse(pOT_MessageResponse,
        pMessage,
        OT_COAP_TYPE_ACKNOWLEDGMENT,
        OT_COAP_CODE_CHANGED);

    error = otCoapSendResponse(PtOpenThreadInstance, pOT_MessageResponse, pMessageInfo);
    if (error != OT_ERROR_NONE && pOT_MessageResponse != NULL)
    {
      otMessageFree(pOT_MessageResponse);
      APP_THREAD_Error(ERR_THREAD_COAP_DATA_RESPONSE,error);
    }
  }while(false);
}

/**
 * @brief This function acknowledges the data reception by sending an ACK
 *    back to the sender.
 * @param  pMessage     message pointer
 * @param  pMessageInfo message info pointer
 * @param  pData        Data pointer
 * @retval None
 */
static void APP_THREAD_CoapSendRespFuotaParameters(otMessage    * pMessage,
    const otMessageInfo * pMessageInfo,
    uint8_t * pData)
{
  otError  error = OT_ERROR_NONE;
  uint8_t data = *pData;
  LOG_DEBUG_APP("APP_THREAD_CoapSendRespFuotaParameters data = %d", data);

  do{
    LOG_DEBUG_APP("FUOTA: Send CoAP response for Fuota Parameters");

    pOT_MessageResponse = otCoapNewMessage(NULL, NULL);
    if (pOT_MessageResponse == NULL)
    {
      LOG_DEBUG_APP("WARNING : pOT_MessageResponse = NULL ! -> exit now");
      break;
    }

    (void)otCoapMessageInitResponse(pOT_MessageResponse,
        pMessage,
        OT_COAP_TYPE_ACKNOWLEDGMENT,
        OT_COAP_CODE_CHANGED);

    (void)otCoapMessageSetPayloadMarker(pOT_MessageResponse);

    error = otMessageAppend(pOT_MessageResponse, &data, sizeof(data));
    if (error != OT_ERROR_NONE)
    {
      APP_THREAD_Error(ERR_THREAD_COAP_APPEND_MSG, error);
    }

    error = otCoapSendResponse(PtOpenThreadInstance, pOT_MessageResponse, pMessageInfo);
    if (error != OT_ERROR_NONE && pOT_MessageResponse != NULL)
    {
      otMessageFree(pOT_MessageResponse);
      APP_THREAD_Error(ERR_THREAD_COAP_DATA_RESPONSE,error);
    }
  }while(false);
}

/**
 * @brief Task responsible for the reset at the end of OTA transfer.
 * @param  None
 * @retval None
 */
static void APP_THREAD_PerformReset( void * arg )
{
  LOG_INFO_APP("*******************************************************");
  LOG_INFO_APP(" FUOTA_CLIENT : END OF TRANSFER COMPLETED");
  /* Stop Toggling of the LED */
  UTIL_TIMER_Stop(&TimerID);
  BSP_LED_On(LED_BLUE);
  
  LOG_INFO_APP("  --> Request to reboot on FW Application");
  LOG_INFO_APP("*******************************************************");
  /**
   * Reboot on FW Application
   */
  *(uint8_t*)SRAM1_BASE = CFG_REBOOT_ON_DOWNLOADED_FW;
  
  NVIC_SystemReset();
}

static void APP_THREAD_AppInit(void)
{
  UTIL_TIMER_Create( &resetTimerId, 0, UTIL_TIMER_ONESHOT, &APP_THREAD_PerformReset, NULL ); 
  
  LOG_INFO_APP("Thread_Ota_Client Application");
  
  /* disable OpenThread Logs */
  setDebugLevel(0);
}

static void APP_THREAD_TimingElapsed(void * args)
{
  UNUSED(args);
  
  BSP_LED_Toggle(LED_BLUE);
}

/**
 * @brief Task associated to the push button 1.
 * @param  None
 * @retval None
 */
void APP_BSP_Button1Action(void)
{
  LOG_INFO_APP("Button 1 pressed");
}

/**
 * @brief Task associated to the push button 2.
 * @param  None
 * @retval None
 */
void APP_BSP_Button3Action(void)
{
  LOG_INFO_APP("Button3 pressed");
  APP_THREAD_ResetFuotaProcess();
}

/**
 * Check the Boot mode request
 * Depending on the result, the CPU may either jump to an existing application in the user flash
 * or keep on running the code to start the OTA loader
 */
void BootModeCheck( void )
{
  if(LL_RCC_IsActiveFlag_SFTRST( ) || LL_RCC_IsActiveFlag_OBLRST( ))
  {
    /**
     * Check Boot Mode from Image Marker
     */
    if((*(uint8_t*)SRAM1_BASE == CFG_REBOOT_ON_DOWNLOADED_FW))
    {
      if (CheckFwAppValidity() == true)
      {
        /**
         * The user has requested to start on the firmware application and it has been checked
         * a valid application is ready
         * Jump now on the application
         */
        JumpFwApp();
      }
    }
    else if(*(uint8_t*)SRAM1_BASE == CFG_REBOOT_ON_OTA_CLIENT_FW)
    {
      /**
       * It has been requested to reboot on Thread_OTA_Client application to download data
       */
    }
    else
    {
      /**
       * There should be no use case to be there because the device already starts from power up
       * and the SRAM1 is then filled with the value define by the user
       * However, it could be that a reset occurs just after a power up and in that case, the Thread_Ota
       * will be running but the sectors to download a new App may not be erased
       */
      JumpSelectionOnPowerUp( );
    }
  }
  else
  {
    /**
     * On Power up, the content of SRAM1 is random
     * The only thing that could be done is to jump on either the firmware application
     * or the Thread_OTA_Client application
     */
    JumpSelectionOnPowerUp( );
  }

  /**
   * Return to the startup file and run the Thread_Ota application
   */
  return;
}

static void JumpSelectionOnPowerUp( void )
{
    /**
     * The SRAM1 is random
     * Initialize SRAM1 to indicate we requested to reboot of Thread_OTA_Client application
     */
    *(uint8_t*)SRAM1_BASE = CFG_REBOOT_ON_OTA_CLIENT_FW;

    /**
     * There is no valid application available
     * Erase all sectors specified by byte1 and byte1 in SRAM1 to download a new App.
     */
    *((uint8_t*)SRAM1_BASE+1) = CFG_APP_START_SECTOR_INDEX;
    *((uint8_t*)SRAM1_BASE+2) = 0xFF;
}

/**
 * Jump to existing FW App in flash
 * It never returns
 */
static void JumpFwApp( void )
{
  fct_t appResetHandler;

  SCB->VTOR = FUOTA_APP_FW_BINARY_ADDRESS;
  __set_MSP( * ( uint32_t * )FUOTA_APP_FW_BINARY_ADDRESS );
  appResetHandler = (fct_t)(*(uint32_t*)(FUOTA_APP_FW_BINARY_ADDRESS + 4u ));
  appResetHandler();

  /**
   * appResetHandler() never returns.
   * However, if for any reason a PUSH instruction is added at the entry of  JumpFwApp(),
   * we need to make sure the POP instruction is not there before appResetHandler() is called
   * The way to ensure this is to add a dummy code after appResetHandler() is called
   * This prevents appResetHandler() to be the last code in the function.
   */
  __WFI();


  return;
}


static bool CheckFwAppValidity( void )
{
  bool status = true;
  uint32_t lMagicKeywordAddress = *(uint32_t*)FUOTA_M33_APP_OTA_TAG_ADDRESS;
  uint32_t lMagicKeywordValue = 0;

  if (lMagicKeywordAddress == 0xFFFFFFFF)
  {
    return false;
  }

  lMagicKeywordValue = *(uint32_t*)lMagicKeywordAddress;

  if( (lMagicKeywordAddress < FLASH_BASE) || (lMagicKeywordAddress > (FLASH_BASE + FLASH_SIZE) ) )
  {
    /**
     * The address is not valid
     */
    status = false;
  }
  else
  {
    if( lMagicKeywordValue != FUOTA_MAGIC_KEYWORD_M33_APP  )
    {
      /**
       * A firmware update procedure did not complete
       */
      status = false;
    }
    else
    {
      /**
       * The firmware application is available
       */
      status = true;

    }
  }
  return status;
}

/* USER CODE END FD_LOCAL_FUNCTIONS */

