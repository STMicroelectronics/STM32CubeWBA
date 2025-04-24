/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_ble.c
  * @author  MCD Application Team
  * @brief   BLE Application
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
#include "main.h"
#include "app_common.h"
#include "log_module.h"
#include "ble.h"
#include "app_ble.h"
#include "host_stack_if.h"
#include "ll_sys_if.h"
#include "stm32_rtos.h"
#include "otp.h"
#include "stm32_timer.h"
#include "stm_list.h"
#include "advanced_memory_manager.h"
#include "blestack.h"
#include "nvm.h"
#include "simple_nvm_arbiter.h"
#include "dis.h"
#include "dis_app.h"
#include "zigbeedirectcomm.h"
#include "zigbeedirectcomm_app.h"
#include "zddsecurity.h"
#include "zddsecurity_app.h"
#include "tunneling.h"
#include "tunneling_app.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bpka.h"
#include "app_bsp.h"
#include "zigbee.zd.h"
#include "zdd_stack_wrapper.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Security parameters structure */
typedef struct
{
  /* IO capability of the device */
  uint8_t ioCapability;

  /**
   * Authentication requirement of the device
   * Man In the Middle protection required?
   */
  uint8_t mitm_mode;

  /* Bonding mode of the device */
  uint8_t bonding_mode;

  /**
   * this variable indicates whether to use a fixed pin
   * during the pairing process or a passkey has to be
   * requested to the application during the pairing process
   * 0 implies use fixed pin and 1 implies request for passkey
   */
  uint8_t Use_Fixed_Pin;

  /* Minimum encryption key size requirement */
  uint8_t encryptionKeySizeMin;

  /* Maximum encryption key size requirement */
  uint8_t encryptionKeySizeMax;

  /**
   * fixed pin to be used in the pairing process if
   * Use_Fixed_Pin is set to 1
   */
  uint32_t Fixed_Pin;

  /**
   * this flag indicates whether the host has to initiate
   * the security, wait for pairing or does not have any security
   * requirements.
   * 0x00 : no security required
   * 0x01 : host should initiate security by sending the slave security
   *        request command
   * 0x02 : host need not send the clave security request but it
   * has to wait for paiirng to complete before doing any other
   * processing
   */
  uint8_t initiateSecurity;
  /* USER CODE BEGIN tSecurityParams */

  /* USER CODE END tSecurityParams */
}SecurityParams_t;

/* Global context contains all BLE common variables. */
typedef struct
{
  /* Security requirements of the host */
  SecurityParams_t bleSecurityParam;

  /* GAP service handle */
  uint16_t gapServiceHandle;

  /* Device name characteristic handle */
  uint16_t devNameCharHandle;

  /* Appearance characteristic handle */
  uint16_t appearanceCharHandle;

  /**
   * connection handle of the current active connection
   * When not in connection, the handle is set to 0xFFFF
   */
  uint16_t connectionHandle;

  /* USER CODE BEGIN BleGlobalContext_t */

  /* USER CODE END BleGlobalContext_t */
}BleGlobalContext_t;

typedef struct
{
  BleGlobalContext_t BleApplicationContext_legacy;
  APP_BLE_ConnStatus_t Device_Connection_Status;
  /* USER CODE BEGIN PTD_1 */
  UTIL_TIMER_Object_t           TimerAdvLowPower_Id;
#if (BLE_RADIO_ACTIVITY_ON_LED_SUPPORT != 0)  
  /* Led Timeout timerID */
  UTIL_TIMER_Object_t SwitchOffLed_timer_Id;
#endif  
  uint8_t connIntervalFlag;
  /* USER CODE END PTD_1 */
}BleApplicationContext_t;

/* Private defines -----------------------------------------------------------*/
/* GATT buffer size (in bytes)*/
#define BLE_GATT_BUF_SIZE \
          BLE_TOTAL_BUFFER_SIZE_GATT(CFG_BLE_NUM_GATT_ATTRIBUTES, \
                                     CFG_BLE_NUM_GATT_SERVICES, \
                                     CFG_BLE_ATT_VALUE_ARRAY_SIZE)

#define MBLOCK_COUNT              (BLE_MBLOCKS_CALC(PREP_WRITE_LIST_SIZE, \
                                                    CFG_BLE_ATT_MTU_MAX, \
                                                    CFG_BLE_NUM_LINK) \
                                   + CFG_BLE_MBLOCK_COUNT_MARGIN)

#define BLE_DYN_ALLOC_SIZE \
        (BLE_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK, MBLOCK_COUNT))

/* USER CODE BEGIN PD */
#define ADV_TIMEOUT_MS                 (60 * 1000)
/* Device Info Characteristic UUID */
#define COPY_UUID_128(uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7, uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0) \
do {\
    uuid_struct[0] = uuid_0; uuid_struct[1] = uuid_1; uuid_struct[2] = uuid_2; uuid_struct[3] = uuid_3; \
    uuid_struct[4] = uuid_4; uuid_struct[5] = uuid_5; uuid_struct[6] = uuid_6; uuid_struct[7] = uuid_7; \
    uuid_struct[8] = uuid_8; uuid_struct[9] = uuid_9; uuid_struct[10] = uuid_10; uuid_struct[11] = uuid_11; \
    uuid_struct[12] = uuid_12; uuid_struct[13] = uuid_13; uuid_struct[14] = uuid_14; uuid_struct[15] = uuid_15; \
}while(0)
#define COPY_DEVINFO_UUID(uuid_struct)       COPY_UUID_128(uuid_struct,0x00,0x00,0xfe,0x31,0x8e,0x22,0x45,0x41,0x9d,0x4c,0x21,0xed,0xae,0x82,0xed,0x19)
#if (BLE_RADIO_ACTIVITY_ON_LED_SUPPORT != 0)
#define LED_ON_TIMEOUT                 (5)          
#endif
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static tListNode BleAsynchEventQueue;

static uint8_t a_BdAddr[BD_ADDR_SIZE];
/* Identity root key used to derive IRK and DHK(Legacy) */
static uint8_t a_BLE_CfgIrValue[16];

/* Encryption root key used to derive LTK(Legacy) and CSRK */
static uint8_t a_BLE_CfgErValue[16];
static BleApplicationContext_t bleAppContext;
DIS_APP_ConnHandleNotEvt_t DISHandleNotification;
ZIGBEEDIRECTCOMM_APP_ConnHandleNotEvt_t ZIGBEEDIRECTCOMMHandleNotification;
ZDDSECURITY_APP_ConnHandleNotEvt_t ZDDSECURITYHandleNotification;
TUNNELING_APP_ConnHandleNotEvt_t TUNNELINGHandleNotification;

static char a_GapDeviceName[] = {  'Z', 'i', 'g', 'b', 'e', 'e', ' ', 'D', 'i', 'r', 'e', 'c', 't' }; /* Gap Device Name */

/* Advertising Data */
uint8_t a_AdvData[9] =
{
  4, AD_TYPE_COMPLETE_LOCAL_NAME, 'Z', 'D', 'D',  /* Complete name */
  3, AD_TYPE_16_BIT_SERV_UUID_CMPLT_LIST, 0xF7, 0xFF,
};
uint64_t buffer_nvm[CFG_BLEPLAT_NVM_MAX_SIZE] = {0};

static AMM_VirtualMemoryCallbackFunction_t APP_BLE_ResumeFlowProcessCb;

/* Host stack init variables */
static uint32_t buffer[DIVC(BLE_DYN_ALLOC_SIZE, 4)];
static uint32_t gatt_buffer[DIVC(BLE_GATT_BUF_SIZE, 4)];
static BleStack_init_t pInitParams;

/* USER CODE BEGIN PV */
uint8_t a_GATT_DevInfoData[22];

/* Extended Advertising Data */
uint8_t a_ExtAdvData[9] =
{
  8, AD_TYPE_SERVICE_DATA, 0xF7, 0xFF, 0x00 /* */, 0x00 /* */, 0x00 /* */, 0x00 /* */, 0x00 /* */,
};
/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/

/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private function prototypes -----------------------------------------------*/
static void BleStack_Process_BG(void);
static void Ble_UserEvtRx(void);
static void BLE_ResumeFlowProcessCallback(void);
static void Ble_Hci_Gap_Gatt_Init(void);
static const uint8_t* BleGenerateBdAddress(void);
static const uint8_t* BleGenerateIRValue(void);
static const uint8_t* BleGenerateERValue(void);
static void gap_cmd_resp_wait(void);
static void gap_cmd_resp_release(void);
static void BLE_NvmCallback (SNVMA_Callback_Status_t);
static uint8_t HOST_BLE_Init(void);
/* USER CODE BEGIN PFP */
#if 0 /* TODO: Should the device enter low power advertising? */
static void APP_BLE_AdvLowPower_timCB(void *arg);
static void APP_BLE_AdvLowPower(void);
#endif
#if (BLE_RADIO_ACTIVITY_ON_LED_SUPPORT != 0)
static void Switch_OFF_Led(void *arg);
#endif
static void APP_BLE_StartAdvertising(uint16_t pan_id, uint16_t nwk_addr, bool pjoin);
/* USER CODE END PFP */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Functions Definition ------------------------------------------------------*/
void APP_BLE_Init(void)
{
  /* USER CODE BEGIN APP_BLE_Init_1 */
#if (BLE_RADIO_ACTIVITY_ON_LED_SUPPORT != 0)
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
#endif
  /* USER CODE END APP_BLE_Init_1 */

  LST_init_head(&BleAsynchEventQueue);

  /* Register BLE Host tasks */
  UTIL_SEQ_RegTask(1U << CFG_TASK_BLE_HOST, UTIL_SEQ_RFU, BleStack_Process_BG);
  UTIL_SEQ_RegTask(1U << CFG_TASK_HCI_ASYNCH_EVT_ID, UTIL_SEQ_RFU, Ble_UserEvtRx);

  /* NVM emulation in RAM initialization */
  NVM_Init(buffer_nvm, 0, CFG_BLEPLAT_NVM_MAX_SIZE);

  /* First register the APP BLE buffer */
  SNVMA_Register (APP_BLE_NvmBuffer,
                  (uint32_t *)buffer_nvm,
                  (CFG_BLEPLAT_NVM_MAX_SIZE * 2));

  /* Realize a restore */
  SNVMA_Restore (APP_BLE_NvmBuffer);
  /* USER CODE BEGIN APP_BLE_Init_Buffers */

  /* USER CODE END APP_BLE_Init_Buffers */

  /* Check consistency */
  if (NVM_Get (NVM_FIRST, 0xFF, 0, 0, 0) != NVM_EOF)
  {
    NVM_Discard (NVM_ALL);
  }

  /* Initialize the BLE Host */
  if (HOST_BLE_Init() == 0u)
  {
    /* Initialization of HCI & GATT & GAP layer */
    Ble_Hci_Gap_Gatt_Init();

    /* Initialization of the BLE Services */
    SVCCTL_Init();

    /* Initialization of the BLE App Context */
    bleAppContext.Device_Connection_Status = APP_BLE_IDLE;
    bleAppContext.BleApplicationContext_legacy.connectionHandle = 0xFFFF;

    /* From here, all initialization are BLE application specific */

    /* USER CODE BEGIN APP_BLE_Init_4 */

    /* USER CODE END APP_BLE_Init_4 */

    /* Initialize Services and Characteristics. */
    LOG_INFO_APP("\n");
    LOG_INFO_APP("Services and Characteristics creation\n");
    DIS_APP_Init();
    ZIGBEEDIRECTCOMM_APP_Init();
    ZDDSECURITY_APP_Init();
    TUNNELING_APP_Init();
    LOG_INFO_APP("End of Services and Characteristics creation\n");
    LOG_INFO_APP("\n");

    /* USER CODE BEGIN APP_BLE_Init_3 */
    UTIL_SEQ_RegTask(1u << CFG_TASK_ZIGBEE_ZDD, 0x00, Zdd_Task);

#if (BLE_RADIO_ACTIVITY_ON_LED_SUPPORT != 0)
    ret = aci_hal_set_radio_activity_mask(0x0006);
    if (ret != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG("  Fail   : aci_hal_set_radio_activity_mask command, result: 0x%2X\n", ret);
    }
    else
    {
      APP_DBG_MSG("  Success: aci_hal_set_radio_activity_mask command\n\r");
    }
    
    /* Create timer to handle the Led Switch OFF */
    UTIL_TIMER_Create(&(bleAppContext.SwitchOffLed_timer_Id),
                      0,
                      UTIL_TIMER_ONESHOT,
                      &Switch_OFF_Led,
                      0);
#endif

#if 0 /* TODO: Should advertising start here? Should the device enter low power advertising? */
    /* Start to Advertise to accept a connection */
    APP_BLE_Procedure_Gap_Peripheral(PROC_GAP_PERIPH_ADVERTISE_START_FAST);

    UTIL_SEQ_RegTask(1<<CFG_TASK_ADV_LP_REQ_ID, UTIL_SEQ_RFU, APP_BLE_AdvLowPower);
    /**
    * Create timer to enter Low Power Advertising
    */
    UTIL_TIMER_Create(&(bleAppContext.TimerAdvLowPower_Id),
                      ADV_TIMEOUT_MS,
                      UTIL_TIMER_ONESHOT,
                      &APP_BLE_AdvLowPower_timCB, 0);
    UTIL_TIMER_Start(&(bleAppContext.TimerAdvLowPower_Id));
#endif
    /* USER CODE END APP_BLE_Init_3 */

  }
  /* USER CODE BEGIN APP_BLE_Init_2 */
  bleAppContext.connIntervalFlag = 0;
  /* USER CODE END APP_BLE_Init_2 */

  return;
}

SVCCTL_UserEvtFlowStatus_t SVCCTL_App_Notification(void *p_Pckt)
{
  tBleStatus ret = BLE_STATUS_ERROR;
  hci_event_pckt    *p_event_pckt;
  evt_le_meta_event *p_meta_evt;
  evt_blecore_aci   *p_blecore_evt;

  p_event_pckt = (hci_event_pckt*) ((hci_uart_pckt *) p_Pckt)->data;
  UNUSED(ret);
  /* USER CODE BEGIN SVCCTL_App_Notification */
  ZDD_Stack_Notification_evt_t            notification_wrapper;
  /* USER CODE END SVCCTL_App_Notification */

  switch (p_event_pckt->evt)
  {
    case HCI_DISCONNECTION_COMPLETE_EVT_CODE:
    {
      hci_disconnection_complete_event_rp0 *p_disconnection_complete_event;
      p_disconnection_complete_event = (hci_disconnection_complete_event_rp0 *) p_event_pckt->data;
      if (p_disconnection_complete_event->Connection_Handle == bleAppContext.BleApplicationContext_legacy.connectionHandle)
      {
        bleAppContext.BleApplicationContext_legacy.connectionHandle = 0xFFFF;
        bleAppContext.Device_Connection_Status = APP_BLE_IDLE;
        LOG_INFO_APP(">>== HCI_DISCONNECTION_COMPLETE_EVT_CODE\n");
        LOG_INFO_APP("     - Connection Handle:   0x%04X\n     - Reason:    0x%02X\n",
                    p_disconnection_complete_event->Connection_Handle,
                    p_disconnection_complete_event->Reason);

        /* USER CODE BEGIN EVT_DISCONN_COMPLETE_2 */

        /* USER CODE END EVT_DISCONN_COMPLETE_2 */
      }
      gap_cmd_resp_release();

      /* USER CODE BEGIN EVT_DISCONN_COMPLETE_1 */
      notification_wrapper.Opcode = BLE_ZDD_WRAP_HCI_DISCONNECTION_COMPLETE_EVT;
      notification_wrapper.ConnectionHandle = p_disconnection_complete_event->Connection_Handle;
      ZDD_Stack_Notification(&notification_wrapper);
      /* USER CODE END EVT_DISCONN_COMPLETE_1 */
      DISHandleNotification.EvtOpcode = DIS_DISCON_HANDLE_EVT;
      ZIGBEEDIRECTCOMMHandleNotification.EvtOpcode = ZIGBEEDIRECTCOMM_DISCON_HANDLE_EVT;
      ZDDSECURITYHandleNotification.EvtOpcode = ZDDSECURITY_DISCON_HANDLE_EVT;
      TUNNELINGHandleNotification.EvtOpcode = TUNNELING_DISCON_HANDLE_EVT;
      DISHandleNotification.ConnectionHandle = p_disconnection_complete_event->Connection_Handle;
      ZIGBEEDIRECTCOMMHandleNotification.ConnectionHandle = p_disconnection_complete_event->Connection_Handle;
      ZDDSECURITYHandleNotification.ConnectionHandle = p_disconnection_complete_event->Connection_Handle;
      TUNNELINGHandleNotification.ConnectionHandle = p_disconnection_complete_event->Connection_Handle;
      DIS_APP_EvtRx(&DISHandleNotification);
      ZIGBEEDIRECTCOMM_APP_EvtRx(&ZIGBEEDIRECTCOMMHandleNotification);
      ZDDSECURITY_APP_EvtRx(&ZDDSECURITYHandleNotification);
      TUNNELING_APP_EvtRx(&TUNNELINGHandleNotification);
      /* USER CODE BEGIN EVT_DISCONN_COMPLETE */

      /* USER CODE END EVT_DISCONN_COMPLETE */
      break; /* HCI_DISCONNECTION_COMPLETE_EVT_CODE */
    }
    case HCI_HARDWARE_ERROR_EVT_CODE:
    {
       hci_hardware_error_event_rp0 *p_hardware_error_event;

       p_hardware_error_event = (hci_hardware_error_event_rp0 *)p_event_pckt->data;
       UNUSED(p_hardware_error_event);
       APP_DBG_MSG(">>== HCI_HARDWARE_ERROR_EVT_CODE\n");
       APP_DBG_MSG("Hardware Code = 0x%02X\n",p_hardware_error_event->Hardware_Code);
       /* USER CODE BEGIN HCI_EVT_LE_HARDWARE_ERROR */

       /* USER CODE END HCI_EVT_LE_HARDWARE_ERROR */
       break; /* HCI_HARDWARE_ERROR_EVT_CODE */
    }
    case HCI_LE_META_EVT_CODE:
    {
      p_meta_evt = (evt_le_meta_event*) p_event_pckt->data;
      /* USER CODE BEGIN EVT_LE_META_EVENT */

      /* USER CODE END EVT_LE_META_EVENT */
      switch (p_meta_evt->subevent)
      {
        case HCI_LE_CONNECTION_UPDATE_COMPLETE_SUBEVT_CODE:
        {
          uint32_t conn_interval_us = 0;
          hci_le_connection_update_complete_event_rp0 *p_conn_update_complete;
          p_conn_update_complete = (hci_le_connection_update_complete_event_rp0 *) p_meta_evt->data;
          conn_interval_us = p_conn_update_complete->Conn_Interval * 1250;
          LOG_INFO_APP(">>== HCI_LE_CONNECTION_UPDATE_COMPLETE_SUBEVT_CODE\n");
          LOG_INFO_APP("     - Connection Interval:   %d.%02d ms\n     - Connection latency:    %d\n     - Supervision Timeout:   %d ms\n",
                       conn_interval_us / 1000,
                       (conn_interval_us%1000) / 10,
                       p_conn_update_complete->Conn_Latency,
                       p_conn_update_complete->Supervision_Timeout*10);
          UNUSED(conn_interval_us);
          UNUSED(p_conn_update_complete);

          /* USER CODE BEGIN EVT_LE_CONN_UPDATE_COMPLETE */

          /* USER CODE END EVT_LE_CONN_UPDATE_COMPLETE */
          break;
        }
        case HCI_LE_PHY_UPDATE_COMPLETE_SUBEVT_CODE:
        {
          hci_le_phy_update_complete_event_rp0 *p_le_phy_update_complete;
          p_le_phy_update_complete = (hci_le_phy_update_complete_event_rp0*)p_meta_evt->data;
          UNUSED(p_le_phy_update_complete);

          gap_cmd_resp_release();

          /* USER CODE BEGIN EVT_LE_PHY_UPDATE_COMPLETE */

          /* USER CODE END EVT_LE_PHY_UPDATE_COMPLETE */
          break;
        }
        case HCI_LE_ENHANCED_CONNECTION_COMPLETE_SUBEVT_CODE:
        {
          uint32_t conn_interval_us = 0;
          hci_le_enhanced_connection_complete_event_rp0 *p_enhanced_conn_complete;
          p_enhanced_conn_complete = (hci_le_enhanced_connection_complete_event_rp0 *) p_meta_evt->data;
          conn_interval_us = p_enhanced_conn_complete->Conn_Interval * 1250;
          LOG_INFO_APP(">>== HCI_LE_ENHANCED_CONNECTION_COMPLETE_SUBEVT_CODE - Connection handle: 0x%04X\n", p_enhanced_conn_complete->Connection_Handle);
          LOG_INFO_APP("     - Connection established with @:%02x:%02x:%02x:%02x:%02x:%02x\n",
                      p_enhanced_conn_complete->Peer_Address[5],
                      p_enhanced_conn_complete->Peer_Address[4],
                      p_enhanced_conn_complete->Peer_Address[3],
                      p_enhanced_conn_complete->Peer_Address[2],
                      p_enhanced_conn_complete->Peer_Address[1],
                      p_enhanced_conn_complete->Peer_Address[0]);
          LOG_INFO_APP("     - Connection Interval:   %d.%02d ms\n     - Connection latency:    %d\n     - Supervision Timeout:   %d ms\n     - Status:              0x%02X\n",
                      conn_interval_us / 1000,
                      (conn_interval_us%1000) / 10,
                      p_enhanced_conn_complete->Conn_Latency,
                      p_enhanced_conn_complete->Supervision_Timeout * 10,
                      p_enhanced_conn_complete->Status
                     );
          UNUSED(conn_interval_us);

          if (bleAppContext.Device_Connection_Status == APP_BLE_LP_CONNECTING)
          {
            /* Connection as client */
            bleAppContext.Device_Connection_Status = APP_BLE_CONNECTED_CLIENT;
          }
          else
          {
            /* Connection as server */
            bleAppContext.Device_Connection_Status = APP_BLE_CONNECTED_SERVER;
          }
          bleAppContext.BleApplicationContext_legacy.connectionHandle = p_enhanced_conn_complete->Connection_Handle;

          DISHandleNotification.EvtOpcode = DIS_CONN_HANDLE_EVT;
          ZIGBEEDIRECTCOMMHandleNotification.EvtOpcode = ZIGBEEDIRECTCOMM_CONN_HANDLE_EVT;
          ZDDSECURITYHandleNotification.EvtOpcode = ZDDSECURITY_CONN_HANDLE_EVT;
          TUNNELINGHandleNotification.EvtOpcode = TUNNELING_CONN_HANDLE_EVT;
          DISHandleNotification.ConnectionHandle = p_enhanced_conn_complete->Connection_Handle;
          ZIGBEEDIRECTCOMMHandleNotification.ConnectionHandle = p_enhanced_conn_complete->Connection_Handle;
          ZDDSECURITYHandleNotification.ConnectionHandle = p_enhanced_conn_complete->Connection_Handle;
          TUNNELINGHandleNotification.ConnectionHandle = p_enhanced_conn_complete->Connection_Handle;
          DIS_APP_EvtRx(&DISHandleNotification);
          ZIGBEEDIRECTCOMM_APP_EvtRx(&ZIGBEEDIRECTCOMMHandleNotification);
          ZDDSECURITY_APP_EvtRx(&ZDDSECURITYHandleNotification);
          TUNNELING_APP_EvtRx(&TUNNELINGHandleNotification);
          /* USER CODE BEGIN HCI_EVT_LE_ENHANCED_CONN_COMPLETE */
          notification_wrapper.Opcode = BLE_ZDD_WRAP_HCI_LE_META_EVT_HCI_LE_ENHANCED_CONNECTION_COMPLETE_SUBEVT;
          notification_wrapper.ConnectionHandle = p_enhanced_conn_complete->Connection_Handle;
          ZDD_Stack_Notification(&notification_wrapper);
          /* USER CODE END HCI_EVT_LE_ENHANCED_CONN_COMPLETE */
          break; /* HCI_LE_ENHANCED_CONNECTION_COMPLETE_SUBEVT_CODE */
        }
        case HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE:
        {
          uint32_t conn_interval_us = 0;
          hci_le_connection_complete_event_rp0 *p_conn_complete;
          p_conn_complete = (hci_le_connection_complete_event_rp0 *) p_meta_evt->data;
          conn_interval_us = p_conn_complete->Conn_Interval * 1250;
          LOG_INFO_APP(">>== HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE - Connection handle: 0x%04X\n", p_conn_complete->Connection_Handle);
          LOG_INFO_APP("     - Connection established with @:%02x:%02x:%02x:%02x:%02x:%02x\n",
                      p_conn_complete->Peer_Address[5],
                      p_conn_complete->Peer_Address[4],
                      p_conn_complete->Peer_Address[3],
                      p_conn_complete->Peer_Address[2],
                      p_conn_complete->Peer_Address[1],
                      p_conn_complete->Peer_Address[0]);
          LOG_INFO_APP("     - Connection Interval:   %d.%02d ms\n     - Connection latency:    %d\n     - Supervision Timeout:   %d ms     - Status:              0x%02X\n",
                      conn_interval_us / 1000,
                      (conn_interval_us%1000) / 10,
                      p_conn_complete->Conn_Latency,
                      p_conn_complete->Supervision_Timeout * 10,
                      p_conn_complete->Status
                     );
          UNUSED(conn_interval_us);

          if (bleAppContext.Device_Connection_Status == APP_BLE_LP_CONNECTING)
          {
            /* Connection as client */
            bleAppContext.Device_Connection_Status = APP_BLE_CONNECTED_CLIENT;
          }
          else
          {
            /* Connection as server */
            bleAppContext.Device_Connection_Status = APP_BLE_CONNECTED_SERVER;
          }
          bleAppContext.BleApplicationContext_legacy.connectionHandle = p_conn_complete->Connection_Handle;

          DISHandleNotification.EvtOpcode = DIS_CONN_HANDLE_EVT;
          ZIGBEEDIRECTCOMMHandleNotification.EvtOpcode = ZIGBEEDIRECTCOMM_CONN_HANDLE_EVT;
          ZDDSECURITYHandleNotification.EvtOpcode = ZDDSECURITY_CONN_HANDLE_EVT;
          TUNNELINGHandleNotification.EvtOpcode = TUNNELING_CONN_HANDLE_EVT;
          DISHandleNotification.ConnectionHandle = p_conn_complete->Connection_Handle;
          ZIGBEEDIRECTCOMMHandleNotification.ConnectionHandle = p_conn_complete->Connection_Handle;
          ZDDSECURITYHandleNotification.ConnectionHandle = p_conn_complete->Connection_Handle;
          TUNNELINGHandleNotification.ConnectionHandle = p_conn_complete->Connection_Handle;
          DIS_APP_EvtRx(&DISHandleNotification);
          ZIGBEEDIRECTCOMM_APP_EvtRx(&ZIGBEEDIRECTCOMMHandleNotification);
          ZDDSECURITY_APP_EvtRx(&ZDDSECURITYHandleNotification);
          TUNNELING_APP_EvtRx(&TUNNELINGHandleNotification);
          /* USER CODE BEGIN HCI_EVT_LE_CONN_COMPLETE */
          notification_wrapper.Opcode = BLE_ZDD_WRAP_HCI_LE_META_EVT_HCI_LE_CONNECTION_COMPLETE_SUBEVT;
          notification_wrapper.ConnectionHandle = p_conn_complete->Connection_Handle;
          notification_wrapper.ConnInterval = p_conn_complete->Conn_Interval;
          ZDD_Stack_Notification(&notification_wrapper);
          /* USER CODE END HCI_EVT_LE_CONN_COMPLETE */
          break; /* HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE */
        }
        /* USER CODE BEGIN SUBEVENT */

        /* USER CODE END SUBEVENT */
        default:
        {
          /* USER CODE BEGIN SUBEVENT_DEFAULT */

          /* USER CODE END SUBEVENT_DEFAULT */
          break;
        }
      }

      /* USER CODE BEGIN META_EVT */

      /* USER CODE END META_EVT */
    }
    break; /* HCI_LE_META_EVT_CODE */

    case HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE:
    {
      p_blecore_evt = (evt_blecore_aci*) p_event_pckt->data;
      /* USER CODE BEGIN EVT_VENDOR */

      /* USER CODE END EVT_VENDOR */
      switch (p_blecore_evt->ecode)
      {
        /* USER CODE BEGIN ECODE */

        /* USER CODE END ECODE */
        case ACI_L2CAP_CONNECTION_UPDATE_RESP_VSEVT_CODE:
        {
          aci_l2cap_connection_update_resp_event_rp0 *p_l2cap_conn_update_resp;
          p_l2cap_conn_update_resp = (aci_l2cap_connection_update_resp_event_rp0 *) p_blecore_evt->data;
          UNUSED(p_l2cap_conn_update_resp);
          /* USER CODE BEGIN EVT_L2CAP_CONNECTION_UPDATE_RESP */

          /* USER CODE END EVT_L2CAP_CONNECTION_UPDATE_RESP */
          break;
        }
        case ACI_GAP_PROC_COMPLETE_VSEVT_CODE:
        {
          aci_gap_proc_complete_event_rp0 *p_gap_proc_complete;
          p_gap_proc_complete = (aci_gap_proc_complete_event_rp0*) p_blecore_evt->data;
          UNUSED(p_gap_proc_complete);

          LOG_INFO_APP(">>== ACI_GAP_PROC_COMPLETE_VSEVT_CODE\n");
          /* USER CODE BEGIN EVT_GAP_PROCEDURE_COMPLETE */

          /* USER CODE END EVT_GAP_PROCEDURE_COMPLETE */
          break; /* ACI_GAP_PROC_COMPLETE_VSEVT_CODE */
        }
        case ACI_HAL_END_OF_RADIO_ACTIVITY_VSEVT_CODE:
        {
          /* USER CODE BEGIN RADIO_ACTIVITY_EVENT */
#if (BLE_RADIO_ACTIVITY_ON_LED_SUPPORT != 0)
          BSP_LED_On(LED_GREEN);
          UTIL_TIMER_StartWithPeriod(&bleAppContext.SwitchOffLed_timer_Id, LED_ON_TIMEOUT);
#endif
          /* USER CODE END RADIO_ACTIVITY_EVENT */
          break; /* ACI_HAL_END_OF_RADIO_ACTIVITY_VSEVT_CODE */
        }
        case ACI_GAP_KEYPRESS_NOTIFICATION_VSEVT_CODE:
        {
          LOG_INFO_APP(">>== ACI_GAP_KEYPRESS_NOTIFICATION_VSEVT_CODE\n");
          /* USER CODE BEGIN ACI_GAP_KEYPRESS_NOTIFICATION_VSEVT_CODE */

          /* USER CODE END ACI_GAP_KEYPRESS_NOTIFICATION_VSEVT_CODE */
          break;
        }
        case ACI_GAP_PASS_KEY_REQ_VSEVT_CODE:
        {
          uint32_t pin;
          LOG_INFO_APP(">>== ACI_GAP_PASS_KEY_REQ_VSEVT_CODE\n");

          pin = CFG_FIXED_PIN;
          /* USER CODE BEGIN ACI_GAP_PASS_KEY_REQ_VSEVT_CODE_0 */
          /* Zigbee Direct does not require Pairing. However, this is here
           * for debugging. */
          aci_gap_pairing_complete_event_rp0 *p_pairing_complete;

          p_pairing_complete = (aci_gap_pairing_complete_event_rp0 *)p_blecore_evt->data;
          if (p_pairing_complete->Status != 0)
          {
            const char *status_str;

            switch (p_pairing_complete->Status)
            {
              case 0x01:
                  status_str = "Timeout";
                  break;

              case 0x02:
                  status_str = "Pairing Failed";
                  break;

              case 0x03:
                  status_str = "Encrypt Failed (local)";
                  break;

              case 0x04:
                  status_str = "Encrypt Failed (peer)";
                  break;

              case 0x05:
                  status_str = "Encrypt not supported";
                  break;

              default:
                  status_str = "Unknown";
                  break;
            }

            LOG_INFO_APP("     - Pairing KO");
            LOG_INFO_APP("     - Status: 0x%02X (%s)", p_pairing_complete->Status, status_str);
            LOG_INFO_APP("     - Reason: 0x%02X", p_pairing_complete->Reason);
          }
          else
          {
            LOG_INFO_APP("     - Pairing Success");
          }
          LOG_INFO_APP("");
          /* USER CODE END ACI_GAP_PASS_KEY_REQ_VSEVT_CODE_0 */

          ret = aci_gap_pass_key_resp(bleAppContext.BleApplicationContext_legacy.connectionHandle, pin);
          if (ret != BLE_STATUS_SUCCESS)
          {
            LOG_INFO_APP("==>> aci_gap_pass_key_resp : Fail, reason: 0x%02X\n", ret);
          }
          else
          {
            LOG_INFO_APP("==>> aci_gap_pass_key_resp : Success\n");
          }
          /* USER CODE BEGIN ACI_GAP_PASS_KEY_REQ_VSEVT_CODE */

          /* USER CODE END ACI_GAP_PASS_KEY_REQ_VSEVT_CODE */
          break;
        }
        case ACI_GAP_NUMERIC_COMPARISON_VALUE_VSEVT_CODE:
        {
          uint8_t confirm_value;
          LOG_INFO_APP(">>== ACI_GAP_NUMERIC_COMPARISON_VALUE_VSEVT_CODE\n");
          LOG_INFO_APP("     - numeric_value = %ld\n",
                      ((aci_gap_numeric_comparison_value_event_rp0 *)(p_blecore_evt->data))->Numeric_Value);
          LOG_INFO_APP("     - Hex_value = %lx\n",
                      ((aci_gap_numeric_comparison_value_event_rp0 *)(p_blecore_evt->data))->Numeric_Value);

          /* Set confirm value to 1(YES) */
          confirm_value = 1;
          /* USER CODE BEGIN ACI_GAP_NUMERIC_COMPARISON_VALUE_VSEVT_CODE_0 */

          /* USER CODE END ACI_GAP_NUMERIC_COMPARISON_VALUE_VSEVT_CODE_0 */

          ret = aci_gap_numeric_comparison_value_confirm_yesno(bleAppContext.BleApplicationContext_legacy.connectionHandle, confirm_value);
          if (ret != BLE_STATUS_SUCCESS)
          {
            LOG_INFO_APP("==>> aci_gap_numeric_comparison_value_confirm_yesno : Fail, reason: 0x%02X\n", ret);
          }
          else
          {
            LOG_INFO_APP("==>> aci_gap_numeric_comparison_value_confirm_yesno : Success\n");
          }
          /* USER CODE BEGIN ACI_GAP_NUMERIC_COMPARISON_VALUE_VSEVT_CODE */

          /* USER CODE END ACI_GAP_NUMERIC_COMPARISON_VALUE_VSEVT_CODE */
          break;
        }
        case ACI_GAP_PAIRING_COMPLETE_VSEVT_CODE:
        {
          LOG_INFO_APP(">>== ACI_GAP_PAIRING_COMPLETE_VSEVT_CODE\n");
          aci_gap_pairing_complete_event_rp0 *p_pairing_complete;
          p_pairing_complete = (aci_gap_pairing_complete_event_rp0*)p_blecore_evt->data;

          if (p_pairing_complete->Status != 0)
          {
            LOG_INFO_APP("     - Pairing KO\n     - Status: 0x%02X\n     - Reason: 0x%02X\n",
                         p_pairing_complete->Status, p_pairing_complete->Reason);
          }
          else
          {
            LOG_INFO_APP("     - Pairing Success\n");
          }
          LOG_INFO_APP("\n");

          /* USER CODE BEGIN ACI_GAP_PAIRING_COMPLETE_VSEVT_CODE */

          /* USER CODE END ACI_GAP_PAIRING_COMPLETE_VSEVT_CODE */
          break;
        }
        case ACI_GAP_BOND_LOST_VSEVT_CODE:
        {
          LOG_INFO_APP(">>== ACI_GAP_BOND_LOST_EVENT\n");
          ret = aci_gap_allow_rebond(bleAppContext.BleApplicationContext_legacy.connectionHandle);
          if (ret != BLE_STATUS_SUCCESS)
          {
            LOG_INFO_APP("==>> aci_gap_allow_rebond : Fail, reason: 0x%02X\n", ret);
          }
          else
          {
            LOG_INFO_APP("==>> aci_gap_allow_rebond : Success\n");
          }
          /* USER CODE BEGIN ACI_GAP_BOND_LOST_VSEVT_CODE */

          /* USER CODE END ACI_GAP_BOND_LOST_VSEVT_CODE */
          break;
        }
        case ACI_HAL_FW_ERROR_VSEVT_CODE:
        {
          aci_hal_fw_error_event_rp0 *p_fw_error_event;

          p_fw_error_event = (aci_hal_fw_error_event_rp0 *)p_blecore_evt->data;
          UNUSED(p_fw_error_event);
          APP_DBG_MSG(">>== ACI_HAL_FW_ERROR_VSEVT_CODE\n");
          APP_DBG_MSG("FW Error Type = 0x%02X\n", p_fw_error_event->FW_Error_Type);
          /* USER CODE BEGIN ACI_HAL_FW_ERROR_VSEVT_CODE */

          /* USER CODE END ACI_HAL_FW_ERROR_VSEVT_CODE */
          break;
        }
        /* USER CODE BEGIN ECODE_1 */
        case ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE: /* 0x0c01 */
          break;

        case ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE: /* 0x0c03 */
          break;

        case ACI_GAP_LIMITED_DISCOVERABLE_VSEVT_CODE: /* 0x0400 */
          break;
        /* USER CODE END ECODE_1 */
        default:
        {
          /* USER CODE BEGIN ECODE_DEFAULT */

          /* USER CODE END ECODE_DEFAULT */
          break;
        }
      }
      /* USER CODE BEGIN EVT_VENDOR_1 */

      /* USER CODE END EVT_VENDOR_1 */
    }
    break; /* HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE */
    /* USER CODE BEGIN EVENT_PCKT */

    /* USER CODE END EVENT_PCKT */
    default:
    {
      /* USER CODE BEGIN EVENT_PCKT_DEFAULT */

      /* USER CODE END EVENT_PCKT_DEFAULT */
      break;
    }
  }
  /* USER CODE BEGIN SVCCTL_App_Notification_1 */

  /* USER CODE END SVCCTL_App_Notification_1 */

  return (SVCCTL_UserEvtFlowEnable);
}

APP_BLE_ConnStatus_t APP_BLE_Get_Server_Connection_Status(void)
{
  return bleAppContext.Device_Connection_Status;
}

void APP_BLE_Procedure_Gap_General(ProcGapGeneralId_t ProcGapGeneralId)
{
  tBleStatus status;

  /* USER CODE BEGIN Procedure_Gap_General_1 */

  /* USER CODE END Procedure_Gap_General_1 */

  switch(ProcGapGeneralId)
  {
    case PROC_GAP_GEN_PHY_TOGGLE:
    {
      uint8_t phy_tx = 0U, phy_rx = 0U;

      status = hci_le_read_phy(bleAppContext.BleApplicationContext_legacy.connectionHandle, &phy_tx, &phy_rx);

      if (status != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("hci_le_read_phy failure: reason=0x%02X\n",status);
      }
      else
      {
        LOG_INFO_APP("==>> hci_le_read_phy - Success\n");
        LOG_INFO_APP("==>> PHY Param  TX= %d, RX= %d\n", phy_tx, phy_rx);
        if ((phy_tx == HCI_TX_PHY_LE_2M) && (phy_rx == HCI_RX_PHY_LE_2M))
        {
          LOG_INFO_APP("==>> hci_le_set_phy PHY Param  TX= %d, RX= %d - ", HCI_TX_PHY_LE_1M, HCI_RX_PHY_LE_1M);
          status = hci_le_set_phy(bleAppContext.BleApplicationContext_legacy.connectionHandle, 0, HCI_TX_PHYS_LE_1M_PREF, HCI_RX_PHYS_LE_1M_PREF, 0);
          if (status != BLE_STATUS_SUCCESS)
          {
            LOG_INFO_APP("Fail\n");
          }
          else
          {
            LOG_INFO_APP("Success\n");
            gap_cmd_resp_wait();/* waiting for HCI_LE_PHY_UPDATE_COMPLETE_SUBEVT_CODE */
          }
        }
        else
        {
          LOG_INFO_APP("==>> hci_le_set_phy PHY Param  TX= %d, RX= %d - ", HCI_TX_PHYS_LE_2M_PREF, HCI_RX_PHYS_LE_2M_PREF);
          status = hci_le_set_phy(bleAppContext.BleApplicationContext_legacy.connectionHandle, 0, HCI_TX_PHYS_LE_2M_PREF, HCI_RX_PHYS_LE_2M_PREF, 0);
          if (status != BLE_STATUS_SUCCESS)
          {
            LOG_INFO_APP("Fail\n");
          }
          else
          {
            LOG_INFO_APP("Success\n");
            gap_cmd_resp_wait();/* waiting for HCI_LE_PHY_UPDATE_COMPLETE_SUBEVT_CODE */
          }
        }
      }
      break;
    }/* PROC_GAP_GEN_PHY_TOGGLE */
    case PROC_GAP_GEN_CONN_TERMINATE:
    {
      status = aci_gap_terminate(bleAppContext.BleApplicationContext_legacy.connectionHandle, HCI_REMOTE_USER_TERMINATED_CONNECTION_ERR_CODE);
      if (status != BLE_STATUS_SUCCESS)
      {
         LOG_INFO_APP("aci_gap_terminate failure: reason=0x%02X\n", status);
      }
      else
      {
        LOG_INFO_APP("==>> aci_gap_terminate : Success\n");
      }
      gap_cmd_resp_wait();/* waiting for HCI_DISCONNECTION_COMPLETE_EVT_CODE */
      break;
    }/* PROC_GAP_GEN_CONN_TERMINATE */
    case PROC_GATT_EXCHANGE_CONFIG:
    {
      status = aci_gatt_exchange_config(bleAppContext.BleApplicationContext_legacy.connectionHandle);
      if (status != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("aci_gatt_exchange_config failure: reason=0x%02X\n", status);
      }
      else
      {
        LOG_INFO_APP("==>> aci_gatt_exchange_config : Success\n");
      }
      break;
    }
    /* USER CODE BEGIN GAP_GENERAL */

    /* USER CODE END GAP_GENERAL */
    default:
      break;
  }

  /* USER CODE BEGIN Procedure_Gap_General_2 */

  /* USER CODE END Procedure_Gap_General_2 */
  return;
}

void APP_BLE_Procedure_Gap_Peripheral(ProcGapPeripheralId_t ProcGapPeripheralId)
{
  tBleStatus status;
  uint32_t paramA = 0U;
  uint32_t paramB = 0U;
  uint32_t paramC = 0U;
  uint32_t paramD = 0U;

  /* USER CODE BEGIN Procedure_Gap_Peripheral_1 */
  uint8_t adv_data[18];
  /* USER CODE END Procedure_Gap_Peripheral_1 */

  /* First set parameters before calling ACI APIs, only if needed */
  switch(ProcGapPeripheralId)
  {
    case PROC_GAP_PERIPH_ADVERTISE_START_FAST:
    {
      paramA = ADV_INTERVAL_MIN;
      paramB = ADV_INTERVAL_MAX;
      paramC = APP_BLE_ADV_FAST;
      paramD = ADV_IND;

      break;
    }
    case PROC_GAP_PERIPH_ADVERTISE_NON_CONN_START_FAST:
    {
      paramA = ADV_INTERVAL_MIN;
      paramB = ADV_INTERVAL_MAX;
      paramC = APP_BLE_ADV_NON_CONN_FAST;
      paramD = ADV_NONCONN_IND;

      break;
    }
    case PROC_GAP_PERIPH_ADVERTISE_START_LP:
    {
      paramA = ADV_LP_INTERVAL_MIN;
      paramB = ADV_LP_INTERVAL_MAX;
      paramC = APP_BLE_ADV_LP;
      paramD = ADV_IND;

      break;
    }
    case PROC_GAP_PERIPH_ADVERTISE_NON_CONN_START_LP:
    {
      paramA = ADV_LP_INTERVAL_MIN;
      paramB = ADV_LP_INTERVAL_MAX;
      paramC = APP_BLE_ADV_NON_CONN_LP;
      paramD = ADV_NONCONN_IND;

      break;
    }
    case PROC_GAP_PERIPH_ADVERTISE_STOP:
    {
      paramC = APP_BLE_IDLE;

      break;
    }/* PROC_GAP_PERIPH_ADVERTISE_STOP */
    case PROC_GAP_PERIPH_CONN_PARAM_UPDATE:
    {
      paramA = CONN_INT_MS(1000);
      paramB = CONN_INT_MS(1000);
      paramC = 0x0000;
      paramD = 0x01F4;

      /* USER CODE BEGIN CONN_PARAM_UPDATE */
      if (bleAppContext.connIntervalFlag != 0)
      {
        bleAppContext.connIntervalFlag = 0;
        paramA = CONN_INT_MS(50);
        paramB = CONN_INT_MS(50);
      }
      else
      {
        bleAppContext.connIntervalFlag = 1;
        paramA = CONN_INT_MS(1000);
        paramB = CONN_INT_MS(1000);
      }
      /* USER CODE END CONN_PARAM_UPDATE */
      break;
    }/* PROC_GAP_PERIPH_CONN_PARAM_UPDATE */
    /* USER CODE BEGIN PARAM_UPDATE_1 */

    /* USER CODE END PARAM_UPDATE_1 */
    default:
      break;
  }

  /* USER CODE BEGIN Procedure_Gap_Peripheral_2 */

  /* USER CODE END Procedure_Gap_Peripheral_2 */

  /* Call ACI APIs */
  switch(ProcGapPeripheralId)
  {
    case PROC_GAP_PERIPH_ADVERTISE_START_FAST:
    case PROC_GAP_PERIPH_ADVERTISE_START_LP:
    case PROC_GAP_PERIPH_ADVERTISE_NON_CONN_START_FAST:
    case PROC_GAP_PERIPH_ADVERTISE_NON_CONN_START_LP:
    {
      /* Start Advertising */
      status = aci_gap_set_discoverable(paramD,
                                        paramA,
                                        paramB,
                                        CFG_BD_ADDRESS_TYPE,
                                        ADV_FILTER,
                                        0, 0, 0, 0, 0, 0);
      if (status != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("==>> aci_gap_set_discoverable - fail, result: 0x%02X\n", status);
      }
      else
      {
        bleAppContext.Device_Connection_Status = (APP_BLE_ConnStatus_t)paramC;
        LOG_INFO_APP("==>> aci_gap_set_discoverable - Success\n");
      }

      status = aci_gap_delete_ad_type(AD_TYPE_TX_POWER_LEVEL);
      if (status != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("==>> delete tx power level - fail, result: 0x%02X\n", status);
      }

      /* Update Advertising data */
      uint8_t *adv_data_p;
      uint8_t adv_data_len;

      adv_data_p = &a_AdvData[0];
      adv_data_len = sizeof(a_AdvData);
      /* USER CODE BEGIN ADV_DATA_UPDATE_1 */
      memcpy(&adv_data[0], &a_AdvData[0], sizeof(a_AdvData));
      memcpy(&adv_data[sizeof(a_AdvData)], &a_ExtAdvData[0], sizeof(a_ExtAdvData));
      adv_data_p = adv_data; /* Override */
      adv_data_len = sizeof(adv_data);
      /* USER CODE END ADV_DATA_UPDATE_1 */
      status = aci_gap_update_adv_data(adv_data_len, adv_data_p);
      if (status != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("==>> Start Advertising Failed, result: 0x%02X\n", status);
      }
      else
      {
        LOG_INFO_APP("==>> Success: Start Advertising\n");
      }
      break;
    }
    case PROC_GAP_PERIPH_ADVERTISE_STOP:
    {
      status = aci_gap_set_non_discoverable();
      if (status != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("aci_gap_set_non_discoverable - fail, result: 0x%02X\n",status);
      }
      else
      {
        LOG_INFO_APP("==>> aci_gap_set_non_discoverable - Success\n");
        if (bleAppContext.BleApplicationContext_legacy.connectionHandle != 0xFFFF)
        {
          bleAppContext.Device_Connection_Status = APP_BLE_CONNECTED_SERVER;
        }
        else
        {
          bleAppContext.Device_Connection_Status = APP_BLE_IDLE;
        }
      }
      break;
    }/* PROC_GAP_PERIPH_ADVERTISE_STOP */
    case PROC_GAP_PERIPH_ADVERTISE_DATA_UPDATE:
    {
      uint8_t *adv_data_p;
      uint8_t adv_data_len;

      adv_data_p = &a_AdvData[0];
      adv_data_len = sizeof(a_AdvData);
      /* USER CODE BEGIN ADV_DATA_UPDATE_2 */

      /* USER CODE END ADV_DATA_UPDATE_2 */
      status = aci_gap_update_adv_data(adv_data_len, adv_data_p);
      if (status != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("aci_gap_update_adv_data - fail, result: 0x%02X\n",status);
      }
      else
      {
        LOG_INFO_APP("==>> aci_gap_update_adv_data - Success\n");
      }

      break;
    }/* PROC_GAP_PERIPH_ADVERTISE_DATA_UPDATE */
    case PROC_GAP_PERIPH_CONN_PARAM_UPDATE:
    {
       status = aci_l2cap_connection_parameter_update_req(
                                                       bleAppContext.BleApplicationContext_legacy.connectionHandle,
                                                       paramA,
                                                       paramB,
                                                       paramC,
                                                       paramD);
      if (status != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("aci_l2cap_connection_parameter_update_req - fail, result: 0x%02X\n",status);
      }
      else
      {
        LOG_INFO_APP("==>> aci_l2cap_connection_parameter_update_req - Success\n");
      }

      break;
    }/* PROC_GAP_PERIPH_CONN_PARAM_UPDATE */
    /* USER CODE BEGIN ACI_CALL */

    /* USER CODE END ACI_CALL */
    default:
      break;
  }

  /* USER CODE BEGIN Procedure_Gap_Peripheral_3 */

  /* USER CODE END Procedure_Gap_Peripheral_3 */
  return;
}

const uint8_t* BleGetBdAddress(void)
{
  const uint8_t *p_bd_addr;

  p_bd_addr = (const uint8_t *)a_BdAddr;

  return p_bd_addr;
}

/* USER CODE BEGIN FD */

static void APP_BLE_StartAdvertising(uint16_t pan_id, uint16_t nwk_addr, bool pjoin)
{
  bool is_service_data_present = false;
  uint8_t *adv_data_p;
  uint8_t adv_data_len;
  uint8_t i;
  uint8_t ad_length;
  uint8_t ad_type;

  adv_data_p = &a_ExtAdvData[0];
  adv_data_len = sizeof(a_ExtAdvData);
  i = 0;

  /* Find the Service Data AD Type and update the contents */
  while (i <adv_data_len)
  {
    ad_length = adv_data_p[i++];
    ad_type = adv_data_p[i];
    if (ad_type != AD_TYPE_SERVICE_DATA)
    {
      i += ad_length; /* Skip */
      continue;
    }
    else
    {
      is_service_data_present = true;
      i++; /* Enter */
    }
    i += 2; /* Skip UUID */

    /* Flags */
    /* version = 0x01
     * tunnel support = 0x10
     * permit join = 0x20 */
    adv_data_p[i] = 0x11U;
    if (pjoin) {
        adv_data_p[i] |= 0x20U;
    }
    i++;
    /* PAN ID */
    putle16(&adv_data_p[i], pan_id);
    i += 2;
    /* Short Address */
    putle16(&adv_data_p[i], nwk_addr);
    i += 2;
  }

  if (is_service_data_present)
  {
    APP_BLE_Procedure_Gap_Peripheral(PROC_GAP_PERIPH_ADVERTISE_START_FAST);
  }
  else
  {
    LOG_ERROR_APP("Error, AD_TYPE_SERVICE_DATA missing from BLE advertisement data.");
  }
}

bool BLE_App_Notification(BLE_App_Notification_evt_t *pNotification)
{
  bool ret = true;
  TUNNELING_Data_t tunneling_data;
  ZDDSECURITY_Data_t zddsecurity_data;
  ZIGBEEDIRECTCOMM_Data_t zigbeedirectcomm_data;

  switch(pNotification->Opcode)
  {
    case ZDD_BLE_WRAP_TUNNELING_TUNNZDTSNPDU_UPDATEVALUE_EVT:
      tunneling_data.Length = pNotification->Data.Length;
      tunneling_data.p_Payload = pNotification->Data.pPayload;
      ret = TUNNELING_UpdateValue(TUNNELING_TUNNZDTSNPDU, &tunneling_data);
      break;
    case ZDD_BLE_WRAP_ZDDSECURITY_SECURITY25519AES_UPDATEVALUE_EVT:
    case ZDD_BLE_WRAP_ZDDSECURITY_SECURITY25519SHA_UPDATEVALUE_EVT:
    case ZDD_BLE_WRAP_ZDDSECURITY_SECURITY_P256SHA_UPDATEVALUE_EVT:
      zddsecurity_data.Length = pNotification->Data.Length;
      zddsecurity_data.p_Payload = pNotification->Data.pPayload;
      if (pNotification->Opcode == ZDD_BLE_WRAP_ZDDSECURITY_SECURITY25519AES_UPDATEVALUE_EVT)
      {
        ret = ZDDSECURITY_UpdateValue(ZDDSECURITY_SECURITY25519AES, &zddsecurity_data);
      }
      else if (pNotification->Opcode == ZDD_BLE_WRAP_ZDDSECURITY_SECURITY25519SHA_UPDATEVALUE_EVT)
      {
        ret = ZDDSECURITY_UpdateValue(ZDDSECURITY_SECURITY25519SHA, &zddsecurity_data);
      }
      else if (pNotification->Opcode == ZDD_BLE_WRAP_ZDDSECURITY_SECURITY_P256SHA_UPDATEVALUE_EVT)
      {
        ret = ZDDSECURITY_UpdateValue(ZDDSECURITY_P256SHA, &zddsecurity_data);
      }
      break;
    case ZDD_BLE_WRAP_ZIGBEEDIRECTCOMM_COMMSTATUS_UPDATEVALUE_EVT:
      zigbeedirectcomm_data.Length = pNotification->Data.Length;
      zigbeedirectcomm_data.p_Payload = pNotification->Data.pPayload;
      ret = ZIGBEEDIRECTCOMM_UpdateValue(ZIGBEEDIRECTCOMM_COMMSTATUS, &zigbeedirectcomm_data);
      break;
    case ZDD_BLE_WRAP_BLE_START_ADVERTISING_EVT:
      APP_BLE_StartAdvertising(pNotification->AdvData.PanId,
                               pNotification->AdvData.NwkAddr,
                               pNotification->AdvData.PJoin);
      break;
    case ZDD_BLE_WRAP_BLE_STOP_ADVERTISING_EVT:
      APP_BLE_Procedure_Gap_Peripheral(PROC_GAP_PERIPH_ADVERTISE_STOP);
      break;
    case ZDD_BLE_WRAP_OS_SHCEDULE_ZDD_TASK_EVT:
      UTIL_SEQ_SetTask(1u << CFG_TASK_ZIGBEE_ZDD, CFG_SEQ_PRIO_1);
      break;
    default:
      ret = false;
      break;
  }

  return ret;
}

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
static uint8_t HOST_BLE_Init(void)
{
  tBleStatus return_status;

  pInitParams.numAttrRecord           = CFG_BLE_NUM_GATT_ATTRIBUTES;
  pInitParams.numAttrServ             = CFG_BLE_NUM_GATT_SERVICES;
  pInitParams.attrValueArrSize        = CFG_BLE_ATT_VALUE_ARRAY_SIZE;
  pInitParams.prWriteListSize         = CFG_BLE_ATTR_PREPARE_WRITE_VALUE_SIZE;
  pInitParams.attMtu                  = CFG_BLE_ATT_MTU_MAX;
  pInitParams.max_coc_nbr             = CFG_BLE_COC_NBR_MAX;
  pInitParams.max_coc_mps             = CFG_BLE_COC_MPS_MAX;
  pInitParams.max_coc_initiator_nbr   = CFG_BLE_COC_INITIATOR_NBR_MAX;
  pInitParams.numOfLinks              = CFG_BLE_NUM_LINK;
  pInitParams.mblockCount             = CFG_BLE_MBLOCK_COUNT;
  pInitParams.bleStartRamAddress      = (uint8_t*)buffer;
  pInitParams.total_buffer_size       = BLE_DYN_ALLOC_SIZE;
  pInitParams.bleStartRamAddress_GATT = (uint8_t*)gatt_buffer;
  pInitParams.total_buffer_size_GATT  = BLE_GATT_BUF_SIZE;
  pInitParams.options                 = CFG_BLE_OPTIONS;
  pInitParams.debug                   = 0U;
/* USER CODE BEGIN HOST_BLE_Init_Params */

/* USER CODE END HOST_BLE_Init_Params */
  return_status = BleStack_Init(&pInitParams);
/* USER CODE BEGIN HOST_BLE_Init */

/* USER CODE END HOST_BLE_Init */
  return ((uint8_t)return_status);
}

static void Ble_Hci_Gap_Gatt_Init(void)
{
  uint8_t role;
  uint16_t gap_service_handle = 0U, gap_dev_name_char_handle = 0U, gap_appearance_char_handle = 0U;
  const uint8_t *p_bd_addr;
  uint16_t a_appearance[1] = {CFG_GAP_APPEARANCE};
  const uint8_t *p_ir_value;
  const uint8_t *p_er_value;
  tBleStatus ret;

  /* USER CODE BEGIN Ble_Hci_Gap_Gatt_Init */
/* Add number of record for Device Info Characteristic */
  static const uint8_t p_additional_svc_record[1] = {0x03};

  ret = aci_hal_write_config_data(CONFIG_DATA_GAP_ADD_REC_NBR_OFFSET,
                                  CONFIG_DATA_GAP_ADD_REC_NBR_LEN,
                                  (uint8_t*) p_additional_svc_record);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_hal_write_config_data command - CONFIG_DATA_GAP_ADD_REC_NBR_OFFSET, result: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_hal_write_config_data command - CONFIG_DATA_GAP_ADD_REC_NBR_OFFSET\n");
  }
  /* USER CODE END Ble_Hci_Gap_Gatt_Init */

  LOG_INFO_APP("==>> Start Ble_Hci_Gap_Gatt_Init function\n");

  /* Write the BD Address */
  p_bd_addr = BleGenerateBdAddress();

  /* USER CODE BEGIN BD_Address_Mngt */

  /* USER CODE END BD_Address_Mngt */

  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET,
                                  CONFIG_DATA_PUBADDR_LEN,
                                  (uint8_t*) p_bd_addr);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_hal_write_config_data command - CONFIG_DATA_PUBADDR_OFFSET, result: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_hal_write_config_data command - CONFIG_DATA_PUBADDR_OFFSET\n");
    LOG_INFO_APP("  Public Bluetooth Address: %02x:%02x:%02x:%02x:%02x:%02x\n",p_bd_addr[5],p_bd_addr[4],p_bd_addr[3],p_bd_addr[2],p_bd_addr[1],p_bd_addr[0]);
  }

  /* Generate Identity root key Value */
  p_ir_value = BleGenerateIRValue();

  /* Write Identity root key used to derive IRK and DHK(Legacy) */
  ret = aci_hal_write_config_data(CONFIG_DATA_IR_OFFSET, CONFIG_DATA_IR_LEN, p_ir_value);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_hal_write_config_data command - CONFIG_DATA_IR_OFFSET, result: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_hal_write_config_data command - CONFIG_DATA_IR_OFFSET\n");
  }

  /* Generate Encryption root key Value */
  p_er_value = BleGenerateERValue();

  /* Write Encryption root key used to derive LTK and CSRK */
  ret = aci_hal_write_config_data(CONFIG_DATA_ER_OFFSET, CONFIG_DATA_ER_LEN, p_er_value);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_hal_write_config_data command - CONFIG_DATA_ER_OFFSET, result: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_hal_write_config_data command - CONFIG_DATA_ER_OFFSET\n");
  }

  /* Set Transmission RF Power. */
  ret = aci_hal_set_tx_power_level(1, CFG_TX_POWER);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_hal_set_tx_power_level command, result: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_hal_set_tx_power_level command\n");
  }

  /* Initialize GATT interface */
  ret = aci_gatt_init();
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_init command, result: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_init command\n");
  }

  /* Initialize GAP interface */
  role = 0U;
  role |= GAP_PERIPHERAL_ROLE;

  /* USER CODE BEGIN Role_Mngt */

  /* USER CODE END Role_Mngt */

  if (role > 0)
  {
    ret = aci_gap_init(role,
                       CFG_PRIVACY,
                       sizeof(a_GapDeviceName),
                       &gap_service_handle,
                       &gap_dev_name_char_handle,
                       &gap_appearance_char_handle);

    if (ret != BLE_STATUS_SUCCESS)
    {
      LOG_INFO_APP("  Fail   : aci_gap_init command, result: 0x%02X\n", ret);
    }
    else
    {
      LOG_INFO_APP("  Success: aci_gap_init command\n");
    }

    ret = aci_gatt_update_char_value(gap_service_handle,
                                     gap_dev_name_char_handle,
                                     0,
                                     sizeof(a_GapDeviceName),
                                     (uint8_t *) a_GapDeviceName);
    if (ret != BLE_STATUS_SUCCESS)
    {
      LOG_INFO_APP("  Fail   : aci_gatt_update_char_value - Device Name, result: 0x%02X\n", ret);
    }
    else
    {
      LOG_INFO_APP("  Success: aci_gatt_update_char_value - Device Name\n");
    }

    ret = aci_gatt_update_char_value(gap_service_handle,
                                     gap_appearance_char_handle,
                                     0,
                                     sizeof(a_appearance),
                                     (uint8_t *)&a_appearance);
    if (ret != BLE_STATUS_SUCCESS)
    {
      LOG_INFO_APP("  Fail   : aci_gatt_update_char_value - Appearance, result: 0x%02X\n", ret);
    }
    else
    {
      LOG_INFO_APP("  Success: aci_gatt_update_char_value - Appearance\n");
    }
  }
  else
  {
    LOG_ERROR_APP("GAP role cannot be null\n");
  }

  /* Initialize Default PHY */
  ret = hci_le_set_default_phy(CFG_PHY_PREF, CFG_PHY_PREF_TX, CFG_PHY_PREF_RX);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : hci_le_set_default_phy command, result: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: hci_le_set_default_phy command\n");
  }

  /* Initialize IO capability */
  bleAppContext.BleApplicationContext_legacy.bleSecurityParam.ioCapability = CFG_IO_CAPABILITY;
  ret = aci_gap_set_io_capability(bleAppContext.BleApplicationContext_legacy.bleSecurityParam.ioCapability);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gap_set_io_capability command, result: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gap_set_io_capability command\n");
  }

  /* Initialize authentication */
  bleAppContext.BleApplicationContext_legacy.bleSecurityParam.mitm_mode             = CFG_MITM_PROTECTION;
  bleAppContext.BleApplicationContext_legacy.bleSecurityParam.encryptionKeySizeMin  = CFG_ENCRYPTION_KEY_SIZE_MIN;
  bleAppContext.BleApplicationContext_legacy.bleSecurityParam.encryptionKeySizeMax  = CFG_ENCRYPTION_KEY_SIZE_MAX;
  bleAppContext.BleApplicationContext_legacy.bleSecurityParam.Use_Fixed_Pin         = CFG_USED_FIXED_PIN;
  bleAppContext.BleApplicationContext_legacy.bleSecurityParam.Fixed_Pin             = CFG_FIXED_PIN;
  bleAppContext.BleApplicationContext_legacy.bleSecurityParam.bonding_mode          = CFG_BONDING_MODE;
  /* USER CODE BEGIN Ble_Hci_Gap_Gatt_Init_1 */

  /* USER CODE END Ble_Hci_Gap_Gatt_Init_1 */

  ret = aci_gap_set_authentication_requirement(bleAppContext.BleApplicationContext_legacy.bleSecurityParam.bonding_mode,
                                               bleAppContext.BleApplicationContext_legacy.bleSecurityParam.mitm_mode,
                                               CFG_SC_SUPPORT,
                                               CFG_KEYPRESS_NOTIFICATION_SUPPORT,
                                               bleAppContext.BleApplicationContext_legacy.bleSecurityParam.encryptionKeySizeMin,
                                               bleAppContext.BleApplicationContext_legacy.bleSecurityParam.encryptionKeySizeMax,
                                               bleAppContext.BleApplicationContext_legacy.bleSecurityParam.Use_Fixed_Pin,
                                               bleAppContext.BleApplicationContext_legacy.bleSecurityParam.Fixed_Pin,
                                               CFG_BD_ADDRESS_DEVICE);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gap_set_authentication_requirement command, result: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gap_set_authentication_requirement command\n");
  }

  /* Initialize whitelist */
  if (bleAppContext.BleApplicationContext_legacy.bleSecurityParam.bonding_mode)
  {
    ret = aci_gap_configure_whitelist();
    if (ret != BLE_STATUS_SUCCESS)
    {
      LOG_INFO_APP("  Fail   : aci_gap_configure_whitelist command, result: 0x%02X\n", ret);
    }
    else
    {
      LOG_INFO_APP("  Success: aci_gap_configure_whitelist command\n");
    }
  }

  /* USER CODE BEGIN Ble_Hci_Gap_Gatt_Init_2 */
  /** Device Info Characteristic **/
  /* Add a new characterisitc */
  Char_UUID_t  uuid;
  uint16_t gap_DevInfoChar_handle = 0U;

  /* Add new characteristic to GAP service */
  uint16_t SizeDeviceInfoChar = 22;
  COPY_DEVINFO_UUID(uuid.Char_UUID_128);

  ret = aci_gatt_add_char(gap_service_handle,
                          UUID_TYPE_128,
                          (Char_UUID_t *) &uuid,
                          SizeDeviceInfoChar,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_NONE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &gap_DevInfoChar_handle);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command : Device Info Char, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command : Device Info Char\n");
  }

  /**
  * Initialize Device Info Characteristic
  */
  uint8_t * p_device_info_payload = (uint8_t*)a_GATT_DevInfoData;

  LOG_INFO_APP("---------------------------------------------\n");
  /* Device ID: WBA5x, WBA6x... */
  a_GATT_DevInfoData[0] = (uint8_t)(LL_DBGMCU_GetDeviceID() & 0xff);
  a_GATT_DevInfoData[1] = (uint8_t)((LL_DBGMCU_GetDeviceID() & 0xff00)>>8);
  LOG_INFO_APP("-- DEVICE INFO CHAR : Device ID = 0x%02X %02X\n",a_GATT_DevInfoData[1],a_GATT_DevInfoData[0]);

  /* Rev ID: RevA, RevB... */
  a_GATT_DevInfoData[2] = (uint8_t)(LL_DBGMCU_GetRevisionID() & 0xff);
  a_GATT_DevInfoData[3] = (uint8_t)((LL_DBGMCU_GetRevisionID() & 0xff00)>>8);
  LOG_INFO_APP("-- DEVICE INFO CHAR : Revision ID = 0x%02X %02X\n",a_GATT_DevInfoData[3],a_GATT_DevInfoData[2]);

  /* Board ID: Nucleo WBA, DK1 WBA... */
  a_GATT_DevInfoData[4] = BOARD_ID_NUCLEO_WBA5X;
  LOG_INFO_APP("-- DEVICE INFO CHAR : Board ID = 0x%02X\n",a_GATT_DevInfoData[4]);

  /* HW Package: QFN32, QFN48... */
  a_GATT_DevInfoData[5] = (uint8_t)LL_GetPackageType();
  LOG_INFO_APP("-- DEVICE INFO CHAR : HW Package = 0x%02X\n",a_GATT_DevInfoData[5]);

  /* FW version: v1.3.0, v1.4.0... */
  a_GATT_DevInfoData[6] = CFG_FW_MAJOR_VERSION;
  a_GATT_DevInfoData[7] = CFG_FW_MINOR_VERSION;
  a_GATT_DevInfoData[8] = CFG_FW_SUBVERSION;
  a_GATT_DevInfoData[9] = CFG_FW_BRANCH;
  a_GATT_DevInfoData[10] = CFG_FW_BUILD;
  LOG_INFO_APP("-- DEVICE INFO CHAR : FW Version = v%d.%d.%d - branch %d - build %d\n",a_GATT_DevInfoData[6],a_GATT_DevInfoData[7],a_GATT_DevInfoData[8],a_GATT_DevInfoData[9],a_GATT_DevInfoData[10]);

  /* Application ID: p2pServer, HeartRate... */
  a_GATT_DevInfoData[11] = FW_ID_HEART_RATE;
  LOG_INFO_APP("-- DEVICE INFO CHAR : Application ID = 0x%02X\n",a_GATT_DevInfoData[11]);

  /* Host Stack Version: 0.15, 0.16... */
  uint8_t HCI_Version = 0;
  uint16_t HCI_Subversion = 0;
  uint8_t LMP_Version = 0;
  uint16_t Company_Identifier = 0;
  uint16_t LMP_Subversion = 0;
  hci_read_local_version_information(&HCI_Version, &HCI_Subversion, &LMP_Version, &Company_Identifier, &LMP_Subversion);
  a_GATT_DevInfoData[12] = (uint8_t)((uint16_t)HCI_Subversion & 0xff);
  LOG_INFO_APP("-- DEVICE INFO CHAR : Host Stack version = 0x%02X\n",a_GATT_DevInfoData[12]);

  /* Host Stack Type: Full, Basic, Basic Plus... */
  a_GATT_DevInfoData[13] = (uint8_t)(((uint16_t)HCI_Subversion & 0xff00)>>8);
  LOG_INFO_APP("-- DEVICE INFO CHAR : Host Stack Type = 0x%02X\n",a_GATT_DevInfoData[13]);

  /* RESERVED */
  a_GATT_DevInfoData[14] = 0xFF; /* reserved */
  a_GATT_DevInfoData[15] = 0xFF; /* reserved */
  a_GATT_DevInfoData[16] = 0xFF; /* reserved */
  a_GATT_DevInfoData[17] = 0xFF; /* reserved */

  /* Audio Lib */
  a_GATT_DevInfoData[18] = 0xFF; /* NA */
  a_GATT_DevInfoData[19] = 0xFF; /* NA */

  /* Audio Codec */
  a_GATT_DevInfoData[20] = 0xFF; /* NA */
  a_GATT_DevInfoData[21] = 0xFF; /* NA */
  LOG_INFO_APP("---------------------------------------------\n");

  ret = aci_gatt_update_char_value(gap_service_handle,
                             gap_DevInfoChar_handle,
                             0, /* charValOffset */
                             SizeDeviceInfoChar, /* charValueLen */
                             p_device_info_payload);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_update_char_value DEVINFO command, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_update_char_value DEVINFO command\n");
  }
  /* USER CODE END Ble_Hci_Gap_Gatt_Init_2 */

  LOG_INFO_APP("==>> End Ble_Hci_Gap_Gatt_Init function\n");

  return;
}

static void Ble_UserEvtRx( void)
{
  SVCCTL_UserEvtFlowStatus_t svctl_return_status;
  BleEvtPacket_t *phcievt = NULL;

  LST_remove_head ( &BleAsynchEventQueue, (tListNode **)&phcievt );

  svctl_return_status = SVCCTL_UserEvtRx((void *)&(phcievt->evtserial));

  if (svctl_return_status != SVCCTL_UserEvtFlowDisable)
  {
    AMM_Free((uint32_t *)phcievt);
  }
  else
  {
    LST_insert_head ( &BleAsynchEventQueue, (tListNode *)phcievt );
  }

  if ((LST_is_empty(&BleAsynchEventQueue) == FALSE) && (svctl_return_status != SVCCTL_UserEvtFlowDisable) )
  {
    UTIL_SEQ_SetTask(1U << CFG_TASK_HCI_ASYNCH_EVT_ID, CFG_SEQ_PRIO_0);
  }

  /* Trigger BLE Host stack to process */
  UTIL_SEQ_SetTask(1U << CFG_TASK_BLE_HOST, CFG_SEQ_PRIO_0);

}

static const uint8_t* BleGenerateBdAddress(void)
{
  OTP_Data_s *p_otp_addr = NULL;
  const uint8_t *p_bd_addr;
  uint32_t udn;
  uint32_t company_id;
  uint32_t device_id;
  uint8_t a_BdAddrDefault[BD_ADDR_SIZE] ={0x65, 0x43, 0x21, 0x1E, 0x08, 0x00};
  uint8_t a_BDAddrNull[BD_ADDR_SIZE];
  memset(&a_BDAddrNull[0], 0x00, sizeof(a_BDAddrNull));

  a_BdAddr[0] = (uint8_t)(CFG_BD_ADDRESS & 0x0000000000FF);
  a_BdAddr[1] = (uint8_t)((CFG_BD_ADDRESS & 0x00000000FF00) >> 8);
  a_BdAddr[2] = (uint8_t)((CFG_BD_ADDRESS & 0x000000FF0000) >> 16);
  a_BdAddr[3] = (uint8_t)((CFG_BD_ADDRESS & 0x0000FF000000) >> 24);
  a_BdAddr[4] = (uint8_t)((CFG_BD_ADDRESS & 0x00FF00000000) >> 32);
  a_BdAddr[5] = (uint8_t)((CFG_BD_ADDRESS & 0xFF0000000000) >> 40);

  if(memcmp(&a_BdAddr[0], &a_BDAddrNull[0], BD_ADDR_SIZE) != 0)
  {
    p_bd_addr = (const uint8_t *)a_BdAddr;
  }
  else
  {
    udn = LL_FLASH_GetUDN();

    /* USER CODE BEGIN BleGenerateBdAddress */

    /* USER CODE END BleGenerateBdAddress */

    if (udn != 0xFFFFFFFF)
    {
      company_id = LL_FLASH_GetSTCompanyID();
      device_id = LL_FLASH_GetDeviceID();

    /**
     * Public Address with the ST company ID
     * bit[47:24] : 24bits (OUI) equal to the company ID
     * bit[23:16] : Device ID.
     * bit[15:0] : The last 16bits from the UDN
     * Note: In order to use the Public Address in a final product, a dedicated
     * 24bits company ID (OUI) shall be bought.
     */
      a_BdAddr[0] = (uint8_t)(udn & 0x000000FF);
      a_BdAddr[1] = (uint8_t)((udn & 0x0000FF00) >> 8);
      a_BdAddr[2] = (uint8_t)device_id;
      a_BdAddr[3] = (uint8_t)(company_id & 0x000000FF);
      a_BdAddr[4] = (uint8_t)((company_id & 0x0000FF00) >> 8);
      a_BdAddr[5] = (uint8_t)((company_id & 0x00FF0000) >> 16);
      p_bd_addr = (const uint8_t *)a_BdAddr;
    }
    else
    {
      if (OTP_Read(0, &p_otp_addr) == HAL_OK)
      {
        a_BdAddr[0] = p_otp_addr->bd_address[0];
        a_BdAddr[1] = p_otp_addr->bd_address[1];
        a_BdAddr[2] = p_otp_addr->bd_address[2];
        a_BdAddr[3] = p_otp_addr->bd_address[3];
        a_BdAddr[4] = p_otp_addr->bd_address[4];
        a_BdAddr[5] = p_otp_addr->bd_address[5];
        p_bd_addr = (const uint8_t *)a_BdAddr;
      }
      else
      {
        memcpy(&a_BdAddr[0], a_BdAddrDefault,BD_ADDR_SIZE);
        p_bd_addr = (const uint8_t *)a_BdAddr;
      }
    }
  }

  return p_bd_addr;
}

static const uint8_t* BleGenerateIRValue(void)
{
  uint32_t uid_word0;
  uint32_t uid_word1;
  const uint8_t *p_ir_value;
  uint8_t a_BLE_CfgIrValueNull[16];
  uint8_t a_cfg_ir_value[16] = CFG_BLE_IR;
  uint8_t a_BLE_CfgIrValueDefault[16] =
  {
    0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0
  };

  /* USER CODE BEGIN BleGenerateIRValue_1 */

  /* USER CODE END BleGenerateIRValue_1 */

  memset(&a_BLE_CfgIrValueNull[0], 0x00, sizeof(a_BLE_CfgIrValueNull));

  memcpy(&a_BLE_CfgIrValue[0], a_cfg_ir_value,16);

  if(memcmp(&a_BLE_CfgIrValue[0], &a_BLE_CfgIrValueNull[0], 16) != 0)
  {
    p_ir_value = (const uint8_t *)a_BLE_CfgIrValue;
  }
  else
  {
    uid_word0 = LL_GetUID_Word0();
    uid_word1 = LL_GetUID_Word1();
    /* USER CODE BEGIN BleGenerateIRValue_2 */

    /* USER CODE END BleGenerateIRValue_2 */

    if ((uid_word0 != 0xFFFFFFFF) && (uid_word1 != 0xFFFFFFFF))
    {

    /**
     * Identity root key is built from bits of the UDN.
     */
      a_BLE_CfgIrValue[0] = a_BLE_CfgIrValue[8] = (uint8_t)(uid_word0 & 0x000000FF);
      a_BLE_CfgIrValue[1] = a_BLE_CfgIrValue[9] = (uint8_t)((uid_word0 & 0x0000FF00) >> 8);
      a_BLE_CfgIrValue[2] = a_BLE_CfgIrValue[10] = (uint8_t)((uid_word0 & 0x00FF0000) >> 16);
      a_BLE_CfgIrValue[3] = a_BLE_CfgIrValue[11] = (uint8_t)((uid_word0 & 0xFF000000) >> 24);
      a_BLE_CfgIrValue[4] = a_BLE_CfgIrValue[12] = (uint8_t)(uid_word1 & 0x000000FF);
      a_BLE_CfgIrValue[5] = a_BLE_CfgIrValue[13] = (uint8_t)((uid_word1 & 0x0000FF00) >> 8);
      a_BLE_CfgIrValue[6] = a_BLE_CfgIrValue[14] = (uint8_t)((uid_word1 & 0x00FF0000) >> 16);
      a_BLE_CfgIrValue[7] = a_BLE_CfgIrValue[15] = (uint8_t)((uid_word1 & 0xFF000000) >> 24);
      p_ir_value = (const uint8_t *)a_BLE_CfgIrValue;
    }
    else
    {
      memcpy(&a_BLE_CfgIrValue[0], a_BLE_CfgIrValueDefault,16);
      p_ir_value = (const uint8_t *)a_BLE_CfgIrValue;
    }
  }

  /* USER CODE BEGIN BleGenerateIRValue_3 */

  /* USER CODE END BleGenerateIRValue_3 */
  return p_ir_value;
}

static const uint8_t* BleGenerateERValue(void)
{
  const uint8_t *p_er_value;
  uint32_t uid_word1;
  uint32_t uid_word2;
  uint8_t a_BLE_CfgErValueNull[16];
  uint8_t a_cfg_er_value[16] = CFG_BLE_ER;
  uint8_t a_BLE_CfgErValueDefault[16] =
  {
    0xFE, 0xDC, 0xBA, 0x09, 0x87, 0x65, 0x43, 0x21, 0xFE, 0xDC, 0xBA, 0x09, 0x87, 0x65, 0x43, 0x21
  };

  /* USER CODE BEGIN BleGenerateERValue_1 */

  /* USER CODE END BleGenerateERValue_1 */

  memset(&a_BLE_CfgErValueNull[0], 0x00, sizeof(a_BLE_CfgErValueNull));

  memcpy(&a_BLE_CfgErValue[0], a_cfg_er_value,16);

  if(memcmp(&a_BLE_CfgErValue[0], &a_BLE_CfgErValueNull[0], 16) != 0)
  {
    p_er_value = (const uint8_t *)a_BLE_CfgErValue;
  }
  else
  {
    uid_word1 = LL_GetUID_Word1();
    uid_word2 = LL_GetUID_Word2();

    /* USER CODE BEGIN BleGenerateERValue_2 */

    /* USER CODE END BleGenerateERValue_2 */

   if ((uid_word1 != 0xFFFFFFFF) && (uid_word2 != 0xFFFFFFFF))
    {

    /**
     * Encryption root key is built from bits of the UDN.
     */
      a_BLE_CfgErValue[0] = a_BLE_CfgErValue[8] = (uint8_t)(uid_word2 & 0x000000FF);
      a_BLE_CfgErValue[1] = a_BLE_CfgErValue[9] = (uint8_t)((uid_word2 & 0x0000FF00) >> 8);
      a_BLE_CfgErValue[2] = a_BLE_CfgErValue[10] = (uint8_t)((uid_word2 & 0x00FF0000) >> 16);
      a_BLE_CfgErValue[3] = a_BLE_CfgErValue[11] = (uint8_t)((uid_word2 & 0xFF000000) >> 24);
      a_BLE_CfgErValue[4] = a_BLE_CfgErValue[12] = (uint8_t)~(uid_word1 & 0x000000FF);
      a_BLE_CfgErValue[5] = a_BLE_CfgErValue[13] = (uint8_t)~((uid_word1 & 0x0000FF00) >> 8);
      a_BLE_CfgErValue[6] = a_BLE_CfgErValue[14] = (uint8_t)~((uid_word1 & 0x00FF0000) >> 16);
      a_BLE_CfgErValue[7] = a_BLE_CfgErValue[15] = (uint8_t)~((uid_word1 & 0xFF000000) >> 24);
      p_er_value = (const uint8_t *)a_BLE_CfgErValue;
    }
    else
    {
      memcpy(&a_BLE_CfgErValue[0], a_BLE_CfgErValueDefault,16);
      p_er_value = (const uint8_t *)a_BLE_CfgErValue;
    }
  }

  /* USER CODE BEGIN BleGenerateERValue_3 */

  /* USER CODE END BleGenerateERValue_3 */
  return p_er_value;
}

static void BleStack_Process_BG(void)
{
  if (BleStack_Process( ) == 0x0)
  {
    BleStackCB_Process( );
  }
}

static void gap_cmd_resp_release(void)
{
  UTIL_SEQ_SetEvt(1U << CFG_IDLEEVT_PROC_GAP_COMPLETE);
  return;
}

static void gap_cmd_resp_wait(void)
{
  UTIL_SEQ_WaitEvt(1U << CFG_IDLEEVT_PROC_GAP_COMPLETE);
  return;
}

/**
  * @brief  Notify the LL to resume the flow process
  * @param  None
  * @retval None
  */
static void BLE_ResumeFlowProcessCallback(void)
{
  /* Receive any events from the LL. */
  change_state_options_t notify_options;

  notify_options.combined_value = 0x0F;

  ll_intf_chng_evnt_hndlr_state( notify_options );
}

static void BLE_NvmCallback (SNVMA_Callback_Status_t CbkStatus)
{
  if (CbkStatus != SNVMA_OPERATION_COMPLETE)
  {
    /* Retry the write operation */
    SNVMA_Write (APP_BLE_NvmBuffer,
                 BLE_NvmCallback);
  }
}

/* USER CODE BEGIN FD_LOCAL_FUNCTION */

#if 0 /* TODO: Should the device enter low power advertising? */
static void APP_BLE_AdvLowPower_timCB(void *arg)
{
  /**
   * The code shall be executed in the background as aci command may be sent
   * The background is the only place where the application can make sure a new aci command
   * is not sent if there is a pending one
   */
  UTIL_SEQ_SetTask(1<<CFG_TASK_ADV_LP_REQ_ID, CFG_SEQ_PRIO_0);

  return;
}

static void APP_BLE_AdvLowPower(void)
{
  UTIL_TIMER_Stop(&(bleAppContext.TimerAdvLowPower_Id));
  APP_BLE_Procedure_Gap_Peripheral(PROC_GAP_PERIPH_ADVERTISE_STOP);
  APP_BLE_Procedure_Gap_Peripheral(PROC_GAP_PERIPH_ADVERTISE_START_LP);
}
#endif
/* USER CODE END FD_LOCAL_FUNCTION */

/*************************************************************
 *
 * WRAP FUNCTIONS
 *
 *************************************************************/

tBleStatus BLECB_Indication( const uint8_t* data,
                          uint16_t length,
                          const uint8_t* ext_data,
                          uint16_t ext_length )
{
  uint8_t status = BLE_STATUS_FAILED;
  BleEvtPacket_t *phcievt = NULL;
  uint16_t total_length = (length+ext_length);

  UNUSED(ext_data);

  if (data[0] == HCI_EVENT_PKT_TYPE)
  {
    APP_BLE_ResumeFlowProcessCb.Callback = BLE_ResumeFlowProcessCallback;
    if (AMM_Alloc (CFG_AMM_VIRTUAL_APP_BLE,
                   DIVC((sizeof(BleEvtPacketHeader_t) + total_length), sizeof (uint32_t)),
                   (uint32_t **)&phcievt,
                   &APP_BLE_ResumeFlowProcessCb) != AMM_ERROR_OK)
    {
      LOG_INFO_APP("Alloc failed\n");
      status = BLE_STATUS_FAILED;
    }
    else if (phcievt != (BleEvtPacket_t *)0 )
    {
      phcievt->evtserial.type = HCI_EVENT_PKT_TYPE;
      phcievt->evtserial.evt.evtcode = data[1];
      phcievt->evtserial.evt.plen  = data[2];
      MEMCPY( (void*)&phcievt->evtserial.evt.payload, &data[3], data[2]);
      LST_insert_tail(&BleAsynchEventQueue, (tListNode *)phcievt);
      UTIL_SEQ_SetTask(1U << CFG_TASK_HCI_ASYNCH_EVT_ID, CFG_SEQ_PRIO_0);
      status = BLE_STATUS_SUCCESS;
    }
  }
  else if (data[0] == HCI_ACLDATA_PKT_TYPE)
  {
    status = BLE_STATUS_SUCCESS;
  }
  return status;
}

void NVMCB_Store( const uint32_t* ptr, uint32_t size )
{
  UNUSED(ptr);
  UNUSED(size);

  /* Call SNVMA for storing - Without callback */
  SNVMA_Write (APP_BLE_NvmBuffer,
               BLE_NvmCallback);
}

/* USER CODE BEGIN FD_WRAP_FUNCTIONS */

#if (BLE_RADIO_ACTIVITY_ON_LED_SUPPORT != 0)
static void Switch_OFF_Led(void *arg)
{
  BSP_LED_Off(LED_GREEN);
  return;
}
#endif

/* USER CODE END FD_WRAP_FUNCTIONS */
