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
#include "ble_core.h"
#include "uuid.h"
#include "svc_ctl.h"
#include "app_ble.h"
#include "host_stack_if.h"
#include "ll_sys_if.h"
#include "stm32_rtos.h"
#include "otp.h"
#include "stm32_timer.h"
#include "stm_list.h"
#include "advanced_memory_manager.h"
#include "blestack.h"
#include "simple_nvm_arbiter.h"
#include "p2p_server.h"
#include "p2p_server_app.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_bsp.h"
#include "adv_ext_app.h"
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

typedef struct
{
  /* GAP service handle */
  uint16_t gapServiceHandle;

  /* GAP device name characteristic handle */
  uint16_t gapDevNameCharHandle;

  /* GAP appearance characteristic handle */
  uint16_t gapAppearanceCharHandle;

  /**
   * connection handle of the current active connection
   * When not in connection, the handle is set to 0xFFFF
   */
  uint16_t connectionHandle;

  /* Security requirements of the host */
  SecurityParams_t bleSecurityParam;

  APP_BLE_ConnStatus_t Device_Connection_Status;
  /* USER CODE BEGIN PTD_1 */
  /* Advertising timeout timerID*/
  UTIL_TIMER_Object_t Advertising_mgr_timer_Id;
  /* Led Timeout timerID */
  UTIL_TIMER_Object_t SwitchOffGPIO_timer_Id;

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
        (BLE_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK, MBLOCK_COUNT, (CFG_BLE_EATT_BEARER_PER_LINK * CFG_BLE_NUM_LINK)))

/* USER CODE BEGIN PD */
#define LED_ON_TIMEOUT_MS              (5)
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
P2P_SERVER_APP_ConnHandleNotEvt_t P2P_SERVERHandleNotification;

static char a_GapDeviceName[] = {  'P', 'e', 'e', 'r', ' ', 't', 'o', ' ', 'P', 'e', 'e', 'r', ' ', 'S', 'e', 'r', 'v', 'e', 'r', ' ', 'E', 'x', 't', 'e', 'n', 'd', 'e', 'd' }; /* Gap Device Name */

/* Advertising Data */
uint8_t a_AdvData[28] =
{
  11, AD_TYPE_COMPLETE_LOCAL_NAME, 'p', '2', 'p', 'S', 'e', 'x', 't', '_', 'X', 'X',  /* Complete name */
  15, AD_TYPE_MANUFACTURER_SPECIFIC_DATA, 0x30, 0x00, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */,
};

static AMM_VirtualMemoryCallbackFunction_t APP_BLE_ResumeFlowProcessCb;

/* Host stack init variables */
static BleStack_init_t pInitParams;

/* Host stack buffers */
static uint32_t host_buffer[DIVC(BLE_DYN_ALLOC_SIZE, 4)];
static uint32_t gatt_buffer[DIVC(BLE_GATT_BUF_SIZE, 4)];
static uint64_t host_nvm_buffer[CFG_BLE_NVM_SIZE_MAX];

/* USER CODE BEGIN PV */
uint8_t a_GATT_DevInfoData[22];
/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/

/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private function prototypes -----------------------------------------------*/
static uint8_t HOST_BLE_Init(void);
static void BleStack_Process_BG(void);
static void Ble_UserEvtRx(void);
static void BLE_ResumeFlowProcessCallback(void);
static void BLE_NvmCallback(SNVMA_Callback_Status_t CbkStatus);
static void Ble_Hci_Gap_Gatt_Init(void);
static const uint8_t* BleGenerateBdAddress(void);
static const uint8_t* BleGenerateIRValue(void);
static const uint8_t* BleGenerateERValue(void);
static void gap_cmd_resp_wait(void);
static void gap_cmd_resp_release(void);
/* USER CODE BEGIN PFP */
static void Adv_Cancel_Req(void *arg);
static void Adv_Cancel(void);
static void Switch_OFF_GPIO(void *arg);
static void fill_advData(uint8_t *p_adv_data, uint8_t tab_size, const uint8_t*p_bd_addr);
/* USER CODE END PFP */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Functions Definition ------------------------------------------------------*/
void APP_BLE_Init(void)
{
  /* USER CODE BEGIN APP_BLE_Init_1 */
  tBleStatus ret;
  /* USER CODE END APP_BLE_Init_1 */

  LST_init_head(&BleAsynchEventQueue);

  /* Register BLE Host tasks */
  UTIL_SEQ_RegTask(1U << CFG_TASK_BLE_HOST, UTIL_SEQ_RFU, BleStack_Process_BG);
  UTIL_SEQ_RegTask(1U << CFG_TASK_HCI_ASYNCH_EVT_ID, UTIL_SEQ_RFU, Ble_UserEvtRx);

  /* Initialise NVM RAM buffer, invalidate it's content before restauration */
  host_nvm_buffer[0] = 0;

  /* Register A NVM buffer for BLE Host stack */
  SNVMA_Register(APP_BLE_NvmBuffer,
                  (uint32_t *)host_nvm_buffer,
                  (CFG_BLE_NVM_SIZE_MAX * 2));

  /* Realize a restore */
  SNVMA_Restore(APP_BLE_NvmBuffer);
  /* USER CODE BEGIN APP_BLE_Init_Buffers */

  /* USER CODE END APP_BLE_Init_Buffers */

  /* Initialize the BLE Host */
  if (HOST_BLE_Init() == 0u)
  {
    /* Initialization of HCI & GATT & GAP layer */
    Ble_Hci_Gap_Gatt_Init();

    /* Initialization of the BLE Services */
    SVCCTL_Init();

    /* Initialization of the BLE App Context */
    bleAppContext.Device_Connection_Status = APP_BLE_IDLE;
    bleAppContext.connectionHandle = 0xFFFF;

    /* From here, all initialization are BLE application specific */

    /* USER CODE BEGIN APP_BLE_Init_4 */
    UTIL_SEQ_RegTask(1<<CFG_TASK_ADV_CANCEL_ID, UTIL_SEQ_RFU, Adv_Cancel);

    /* Create timer to handle the Advertising Stop */
    UTIL_TIMER_Create(&(bleAppContext.Advertising_mgr_timer_Id),
                      0,
                      UTIL_TIMER_ONESHOT,
                      &Adv_Cancel_Req,
                      0);
    /* Create timer to handle the Led Switch OFF */
    UTIL_TIMER_Create(&(bleAppContext.SwitchOffGPIO_timer_Id),
                      0,
                      UTIL_TIMER_ONESHOT,
                      &Switch_OFF_GPIO,
                      0);
    /* USER CODE END APP_BLE_Init_4 */

    /* Initialize Services and Characteristics. */
    LOG_INFO_APP("\n");
    LOG_INFO_APP("Services and Characteristics creation\n");
    P2P_SERVER_APP_Init();
    LOG_INFO_APP("End of Services and Characteristics creation\n");
    LOG_INFO_APP("\n");

    /* USER CODE BEGIN APP_BLE_Init_3 */
    ret = aci_hal_set_radio_activity_mask(0x0006);
    if (ret != BLE_STATUS_SUCCESS)
    {
      LOG_INFO_APP("  Fail   : aci_hal_set_radio_activity_mask command, result: 0x%2X\n", ret);
    }
    else
    {
      LOG_INFO_APP("  Success: aci_hal_set_radio_activity_mask command\n\r");
    }

    ADV_EXT_Config();

    ADV_EXT_Start();

    /* Start a timer to stop advertising after a while */
    UTIL_TIMER_StartWithPeriod(&bleAppContext.Advertising_mgr_timer_Id, ADV_TIMEOUT_MS);

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

  /* USER CODE END SVCCTL_App_Notification */

  switch (p_event_pckt->evt)
  {
    case HCI_DISCONNECTION_COMPLETE_EVT_CODE:
    {
      hci_disconnection_complete_event_rp0 *p_disconnection_complete_event;
      p_disconnection_complete_event = (hci_disconnection_complete_event_rp0 *) p_event_pckt->data;
      if (p_disconnection_complete_event->Connection_Handle == bleAppContext.connectionHandle)
      {
        bleAppContext.connectionHandle = 0xFFFF;
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

      /* USER CODE END EVT_DISCONN_COMPLETE_1 */
      P2P_SERVERHandleNotification.EvtOpcode = P2P_SERVER_DISCON_HANDLE_EVT;
      P2P_SERVERHandleNotification.ConnectionHandle = p_disconnection_complete_event->Connection_Handle;
      P2P_SERVER_APP_EvtRx(&P2P_SERVERHandleNotification);
      /* USER CODE BEGIN EVT_DISCONN_COMPLETE */

      /* USER CODE END EVT_DISCONN_COMPLETE */
      break; /* HCI_DISCONNECTION_COMPLETE_EVT_CODE */
    }
    case HCI_HARDWARE_ERROR_EVT_CODE:
    {
       hci_hardware_error_event_rp0 *p_hardware_error_event;

       p_hardware_error_event = (hci_hardware_error_event_rp0 *)p_event_pckt->data;
       UNUSED(p_hardware_error_event);
       LOG_INFO_APP(">>== HCI_HARDWARE_ERROR_EVT_CODE\n");
       LOG_INFO_APP("Hardware Code = 0x%02X\n",p_hardware_error_event->Hardware_Code);
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
          LOG_INFO_APP("     - Connection Interval:   %d.%02d ms\n     - Connection latency:    %d\n     - Supervision Timeout:   %d ms\n     - Status:                0x%02X\n",
                       conn_interval_us / 1000,
                       (conn_interval_us%1000) / 10,
                       p_conn_update_complete->Conn_Latency,
                       p_conn_update_complete->Supervision_Timeout*10,
                       p_conn_update_complete->Status);
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
          LOG_INFO_APP("     - Connection Interval:   %d.%02d ms\n     - Connection latency:    %d\n     - Supervision Timeout:   %d ms\n     - Status:               0x%02X\n",
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
          bleAppContext.connectionHandle = p_enhanced_conn_complete->Connection_Handle;

          P2P_SERVERHandleNotification.EvtOpcode = P2P_SERVER_CONN_HANDLE_EVT;
          P2P_SERVERHandleNotification.ConnectionHandle = p_enhanced_conn_complete->Connection_Handle;
          P2P_SERVER_APP_EvtRx(&P2P_SERVERHandleNotification);
          /* USER CODE BEGIN HCI_EVT_LE_ENHANCED_CONN_COMPLETE */
          /* The connection is done, there is no need anymore to schedule the LP ADV */
          UTIL_TIMER_Stop(&(bleAppContext.Advertising_mgr_timer_Id));
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
          LOG_INFO_APP("     - Connection Interval:   %d.%02d ms\n     - Connection latency:    %d\n     - Supervision Timeout:   %d ms\n     - Status:               0x%02X\n",
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
          bleAppContext.connectionHandle = p_conn_complete->Connection_Handle;

          P2P_SERVERHandleNotification.EvtOpcode = P2P_SERVER_CONN_HANDLE_EVT;
          P2P_SERVERHandleNotification.ConnectionHandle = p_conn_complete->Connection_Handle;
          P2P_SERVER_APP_EvtRx(&P2P_SERVERHandleNotification);
          /* USER CODE BEGIN HCI_EVT_LE_CONN_COMPLETE */
          /* The connection is done, there is no need anymore to schedule the LP ADV */
          UTIL_TIMER_Stop(&(bleAppContext.Advertising_mgr_timer_Id));
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
          #if (CFG_LED_SUPPORTED == 1)
          BSP_LED_On(LED_GREEN);
          #endif
          UTIL_TIMER_StartWithPeriod(&bleAppContext.SwitchOffGPIO_timer_Id, LED_ON_TIMEOUT_MS);
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

          /* USER CODE END ACI_GAP_PASS_KEY_REQ_VSEVT_CODE_0 */

          ret = aci_gap_pass_key_resp(bleAppContext.connectionHandle, pin);
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

          ret = aci_gap_numeric_comparison_value_confirm_yesno(bleAppContext.connectionHandle, confirm_value);
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
          ret = aci_gap_allow_rebond(bleAppContext.connectionHandle);
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
          LOG_INFO_APP(">>== ACI_HAL_FW_ERROR_VSEVT_CODE\n");
          LOG_INFO_APP("FW Error Type = 0x%02X\n", p_fw_error_event->FW_Error_Type);
          /* USER CODE BEGIN ACI_HAL_FW_ERROR_VSEVT_CODE */

          /* USER CODE END ACI_HAL_FW_ERROR_VSEVT_CODE */
          break;
        }
        /* USER CODE BEGIN ECODE_1 */

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

      status = hci_le_read_phy(bleAppContext.connectionHandle, &phy_tx, &phy_rx);

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
          status = hci_le_set_phy(bleAppContext.connectionHandle, 0, HCI_TX_PHYS_LE_1M_PREF, HCI_RX_PHYS_LE_1M_PREF, 0);
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
          status = hci_le_set_phy(bleAppContext.connectionHandle, 0, HCI_TX_PHYS_LE_2M_PREF, HCI_RX_PHYS_LE_2M_PREF, 0);
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
      status = aci_gap_terminate(bleAppContext.connectionHandle, HCI_REMOTE_USER_TERMINATED_CONNECTION_ERR_CODE);
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
      status = aci_gatt_exchange_config(bleAppContext.connectionHandle);
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
        if (bleAppContext.connectionHandle != 0xFFFF)
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
                                                       bleAppContext.connectionHandle,
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

tBleStatus SetGapAppearance(uint16_t appearance)
{
  tBleStatus ret;

  ret = aci_gatt_update_char_value(bleAppContext.gapServiceHandle,
                                   bleAppContext.gapAppearanceCharHandle,
                                   0,
                                   2,
                                   (uint8_t *)&appearance);
  LOG_INFO_APP("Set apperance 0x%04X in GAP database with status %d\n", appearance, ret);

  return ret;
}

tBleStatus SetGapDeviceName(uint8_t *devicename, uint8_t devicename_len)
{
  tBleStatus ret;

  ret = aci_gatt_update_char_value(bleAppContext.gapServiceHandle,
                                   bleAppContext.gapDevNameCharHandle,
                                   0,
                                   devicename_len,
                                   devicename);
  LOG_INFO_APP("Set device name in GAP database with status %d\n", ret);

  return ret;
}

void APP_BLE_HostNvmStore(void)
{
  /* Start SNVMA write procedure */
  SNVMA_Write(APP_BLE_NvmBuffer, BLE_NvmCallback);
}

/* USER CODE BEGIN FD */

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
  pInitParams.max_add_eatt_bearers    = CFG_BLE_EATT_BEARER_PER_LINK * CFG_BLE_NUM_LINK;
  pInitParams.numOfLinks              = CFG_BLE_NUM_LINK;
  pInitParams.mblockCount             = CFG_BLE_MBLOCK_COUNT;
  pInitParams.bleStartRamAddress      = (uint8_t*)host_buffer;
  pInitParams.total_buffer_size       = BLE_DYN_ALLOC_SIZE;
  pInitParams.bleStartRamAddress_GATT = (uint8_t*)gatt_buffer;
  pInitParams.total_buffer_size_GATT  = BLE_GATT_BUF_SIZE;
  pInitParams.nvm_cache_buffer        = host_nvm_buffer;
  pInitParams.nvm_cache_max_size      = CFG_BLE_NVM_SIZE_MAX;
  pInitParams.nvm_cache_size          = CFG_BLE_NVM_SIZE_MAX - 1;
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
      bleAppContext.gapServiceHandle        = gap_service_handle;
      bleAppContext.gapDevNameCharHandle    = gap_dev_name_char_handle;
      bleAppContext.gapAppearanceCharHandle = gap_appearance_char_handle;
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
  bleAppContext.bleSecurityParam.ioCapability = CFG_IO_CAPABILITY;
  ret = aci_gap_set_io_capability(bleAppContext.bleSecurityParam.ioCapability);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gap_set_io_capability command, result: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gap_set_io_capability command\n");
  }

  /* Initialize authentication */
  bleAppContext.bleSecurityParam.mitm_mode             = CFG_MITM_PROTECTION;
  bleAppContext.bleSecurityParam.encryptionKeySizeMin  = CFG_ENCRYPTION_KEY_SIZE_MIN;
  bleAppContext.bleSecurityParam.encryptionKeySizeMax  = CFG_ENCRYPTION_KEY_SIZE_MAX;
  bleAppContext.bleSecurityParam.Use_Fixed_Pin         = CFG_USED_FIXED_PIN;
  bleAppContext.bleSecurityParam.Fixed_Pin             = CFG_FIXED_PIN;
  bleAppContext.bleSecurityParam.bonding_mode          = CFG_BONDING_MODE;
  /* USER CODE BEGIN Ble_Hci_Gap_Gatt_Init_1 */
  fill_advData(&a_AdvData[0], sizeof(a_AdvData), (uint8_t*) p_bd_addr);
  /* USER CODE END Ble_Hci_Gap_Gatt_Init_1 */

  ret = aci_gap_set_authentication_requirement(bleAppContext.bleSecurityParam.bonding_mode,
                                               bleAppContext.bleSecurityParam.mitm_mode,
                                               CFG_SC_SUPPORT,
                                               CFG_KEYPRESS_NOTIFICATION_SUPPORT,
                                               bleAppContext.bleSecurityParam.encryptionKeySizeMin,
                                               bleAppContext.bleSecurityParam.encryptionKeySizeMax,
                                               bleAppContext.bleSecurityParam.Use_Fixed_Pin,
                                               bleAppContext.bleSecurityParam.Fixed_Pin,
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
  if (bleAppContext.bleSecurityParam.bonding_mode)
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
  a_GATT_DevInfoData[4] = BOARD_ID_NUCLEO_WBA6X;
  LOG_INFO_APP("-- DEVICE INFO CHAR : Board ID = 0x%02X\n",a_GATT_DevInfoData[4]);

  /* HW Package: QFN32, QFN48... */
  a_GATT_DevInfoData[5] = (uint8_t)LL_GetPackageType();
  LOG_INFO_APP("-- DEVICE INFO CHAR : HW Package = 0x%02X\n",a_GATT_DevInfoData[5]);

  /* FW version: v1.3.0, v1.4.0... */
  a_GATT_DevInfoData[6] = WPAN_FW_MAJOR_VERSION;
  a_GATT_DevInfoData[7] = WPAN_FW_MINOR_VERSION;
  a_GATT_DevInfoData[8] = WPAN_FW_SUBVERSION;
  a_GATT_DevInfoData[9] = WPAN_FW_BRANCH;
  a_GATT_DevInfoData[10] = WPAN_FW_BUILD;
  LOG_INFO_APP("-- DEVICE INFO CHAR : FW Version = v%d.%d.%d - branch %d - build %d\n",a_GATT_DevInfoData[6],a_GATT_DevInfoData[7],a_GATT_DevInfoData[8],a_GATT_DevInfoData[9],a_GATT_DevInfoData[10]);

  /* Application ID: p2pServer, HeartRate... */
  a_GATT_DevInfoData[11] = FW_ID_P2P_SERVER;
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

static void BLE_NvmCallback(SNVMA_Callback_Status_t CbkStatus)
{
  if (CbkStatus != SNVMA_OPERATION_COMPLETE)
  {
    /* Retry the write operation */
    SNVMA_Write (APP_BLE_NvmBuffer, BLE_NvmCallback);
  }
}

/* USER CODE BEGIN FD_LOCAL_FUNCTION */

static void Adv_Cancel_Req(void *arg)
{
  UTIL_SEQ_SetTask(1 << CFG_TASK_ADV_CANCEL_ID, CFG_SEQ_PRIO_0);
  return;
}

static void Switch_OFF_GPIO(void *arg)
{
  #if (CFG_LED_SUPPORTED == 1)
  BSP_LED_Off(LED_GREEN);
  #endif
  return;
}

static void Adv_Cancel(void)
{
  #if (CFG_LED_SUPPORTED == 1)
  BSP_LED_Off(LED_GREEN);
  #endif

  bleAppContext.Device_Connection_Status = APP_BLE_IDLE;

  return;
}

static void fill_advData(uint8_t *p_adv_data, uint8_t tab_size, const uint8_t* p_bd_addr)
{
  uint16_t i =0;
  uint8_t bd_addr_1, bd_addr_0;
  uint8_t ad_length, ad_type;  
  
  while(i < tab_size)
  {
    ad_length = p_adv_data[i];
    ad_type = p_adv_data[i + 1];

    if( (i + ad_length) >= tab_size )
    {
      break;
    }
      
    switch (ad_type)
    {
    case AD_TYPE_FLAGS:
      break;
    case AD_TYPE_TX_POWER_LEVEL:
      break;
    case AD_TYPE_COMPLETE_LOCAL_NAME:
      {
        if((p_adv_data[i + ad_length] == 'X') && (p_adv_data[i + ad_length - 1] == 'X'))
        {
          bd_addr_1 = ((p_bd_addr[0] & 0xF0)>>4);
          bd_addr_0 = (p_bd_addr[0] & 0xF);
          
          /* Convert hex value into ascii */
          if(bd_addr_1 > 0x09)
          {
            p_adv_data[i + ad_length - 1] = bd_addr_1 + '7';
          }
          else
          {
            p_adv_data[i + ad_length - 1] = bd_addr_1 + '0';
          }
          
          if(bd_addr_0 > 0x09)
          {
            p_adv_data[i + ad_length] = bd_addr_0 + '7';
          }
          else
          {
            p_adv_data[i + ad_length] = bd_addr_0 + '0';
          }
        }
        break;
      }
    case AD_TYPE_MANUFACTURER_SPECIFIC_DATA:
      {
        p_adv_data[i+2] = ST_MANUF_ID;
        p_adv_data[i+3] = 0x00;
        p_adv_data[i+4] = BLUESTSDK_V2; /* blueST SDK version */
        p_adv_data[i+5] = BOARD_ID_NUCLEO_WBA6X; /* Board ID */
        p_adv_data[i+6] = FW_ID_P2P_SERVER; /* FW ID */
        p_adv_data[i+7] = 0x00; /* FW data 1 */
        p_adv_data[i+8] = 0x00; /* FW data 2 */
        p_adv_data[i+9] = 0x00; /* FW data 3 */
        p_adv_data[i+10] = p_bd_addr[5]; /* MSB BD address */
        p_adv_data[i+11] = p_bd_addr[4];
        p_adv_data[i+12] = p_bd_addr[3];
        p_adv_data[i+13] = p_bd_addr[2];
        p_adv_data[i+14] = p_bd_addr[1];
        p_adv_data[i+15] = p_bd_addr[0]; /* LSB BD address */
        break;
      }
    default:
      break;
    }
    i += ad_length + 1; /* increment the iterator to go on next element*/
  }
}
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

/* USER CODE BEGIN FD_WRAP_FUNCTIONS */
#if (CFG_BUTTON_SUPPORTED == 1)
void APP_BSP_Button1Action(void)
{
  if (bleAppContext.Device_Connection_Status == APP_BLE_IDLE)
  {
  }
  else
  {
    UTIL_SEQ_SetTask( 1<<CFG_TASK_SEND_NOTIF_ID, CFG_SEQ_PRIO_0);
  }

  return;
}

void APP_BSP_Button2Action(void)
{
  tBleStatus ret;
  
  if (bleAppContext.Device_Connection_Status != APP_BLE_CONNECTED_SERVER)
  {
    /* Clear Security Database */
    ret = aci_gap_clear_security_db();
    if (ret != BLE_STATUS_SUCCESS)
    {
      LOG_INFO_APP("==>> aci_gap_clear_security_db - Fail, result: 0x%02X\n", ret);
    }
    else
    {
      LOG_INFO_APP("==>> aci_gap_clear_security_db - Success\n");
    }
  }
  else
  {
  }

  return;
}

void APP_BSP_Button3Action(void)
{
  if (bleAppContext.Device_Connection_Status != APP_BLE_CONNECTED_SERVER)
  {

  }
  else
  {
    APP_BLE_Procedure_Gap_Peripheral(PROC_GAP_PERIPH_CONN_PARAM_UPDATE);
  }

  return;
}

#endif

/* USER CODE END FD_WRAP_FUNCTIONS */
