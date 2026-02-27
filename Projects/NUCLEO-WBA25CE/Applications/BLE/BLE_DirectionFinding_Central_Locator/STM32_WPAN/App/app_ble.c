/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_ble.c
  * @author  MCD Application Team
  * @brief   BLE Application
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "baes.h"
#include "pka_ctrl.h"
#include "ble_timer.h"
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
#include "gatt_client_app.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_bsp.h"
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

  /* Minimum encryption key size requirement */
  uint8_t encryptionKeySizeMin;

  /* Maximum encryption key size requirement */
  uint8_t encryptionKeySizeMax;

  /**
   * this flag indicates whether the host has to initiate
   * the security, wait for pairing or does not have any security
   * requirements.
   * 0x00 : no security required
   * 0x01 : host should initiate security by sending the slave security
   *        request command
   * 0x02 : host need not send the clave security request but it
   * has to wait for pairing to complete before doing any other
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
  uint8_t deviceServerFound;
  uint8_t a_deviceServerBdAddrType;
  uint8_t a_deviceServerBdAddr[BD_ADDR_SIZE];
  /* USER CODE END PTD_1 */
}BleApplicationContext_t;

/* Private defines -----------------------------------------------------------*/
/* GATT buffer size (in bytes)*/
#define BLE_GATT_BUF_SIZE \
          BLE_TOTAL_BUFFER_SIZE_GATT(CFG_BLE_NUM_GATT_ATTRIBUTES, \
                                     CFG_BLE_NUM_GATT_SERVICES, \
                                     CFG_BLE_ATT_VALUE_ARRAY_SIZE)

#define BLE_HOST_EVENT_BUF_SIZE   CFG_BLE_HOST_EVENT_BUF_SIZE

#define MBLOCK_COUNT              (BLE_MBLOCKS_CALC(PREP_WRITE_LIST_SIZE, \
                                                    CFG_BLE_ATT_MTU_MAX, \
                                                    CFG_BLE_NUM_LINK) \
                                   + CFG_BLE_MBLOCK_COUNT_MARGIN)

#define BLE_DYN_ALLOC_SIZE \
        (BLE_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK, MBLOCK_COUNT, (CFG_BLE_EATT_BEARER_PER_LINK * CFG_BLE_NUM_LINK)))

#define BLE_DEFAULT_PIN            (111111) /* Default PIN code for pairing */

/* USER CODE BEGIN PD */
#define CTE_AOA         0x00
#define CTE_AOD_1us     0x01
#define CTE_AOD_2us     0x02

#define CTE_AOA_BIT         0x01
#define CTE_AOD_1us_BIT     0x02
#define CTE_AOD_2us_BIT     0x04

#define CTE_SLOT_1us        0x01
#define CTE_SLOT_2us        0x02

#define SCAN_SWITCH_ON_OFF_LED2_TIMEOUT_MS                                   100

#define PERIPHERAL_PUBLIC_BD_ADDRESS                            (0x0280E10013E1)

/* Parameters for Direction Finding */
#define CTE_SLOT_DURATION           CTE_SLOT_1us        /* Only used for AoA */
#define RX_SWITCHING_PATTERN_LENGTH sizeof(antenna_ids) /* Only used for AoA. */
#define ANTENNA_IDS                 {0,1,2,3}           /* Only used for AoA. */

#define CTE_REQ_INTERVAL            10                  /* Interval for CTE requests, in number of connection intervals */
#define MIN_CTE_LENGTH              10                  /* Minimum CTE length, in units of 8 us */
#define CTE_TYPE                    CTE_AOD_1us         /* Requested CTE type */

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
GATT_CLIENT_APP_ConnHandle_Notif_evt_t clientHandleNotification;

static char a_GapDeviceName[] = {  'D', 'i', 'r', 'e', 'c', 't', 'i', 'o', 'n', ' ', 'F', 'i', 'n', 'd', 'i', 'n', 'g', ' ', 'L', 'o', 'c', 'a', 't', 'o', 'r' }; /* Gap Device Name */

static AMM_VirtualMemoryCallbackFunction_t BLE_EVENTS_ResumeFlowProcessCb;

/* Host stack init variables */
static BleStack_init_t pInitParams;

/* Host stack buffers */
PLACE_IN_SECTION("TAG_HostStack") static uint32_t host_buffer[DIVC(BLE_DYN_ALLOC_SIZE, 4)];
PLACE_IN_SECTION("TAG_HostStack") static uint32_t gatt_buffer[DIVC(BLE_GATT_BUF_SIZE, 4)];
PLACE_IN_SECTION("TAG_HostStack") static uint16_t host_event_buffer[DIVC(BLE_HOST_EVENT_BUF_SIZE, 2)];
PLACE_IN_SECTION("TAG_HostStack") static uint64_t host_nvm_buffer[CFG_BLE_NVM_SIZE_MAX];
PLACE_IN_SECTION("TAG_HostStack") static uint8_t long_write_buffer[CFG_BLE_LONG_WRITE_DATA_BUF_SIZE];
PLACE_IN_SECTION("TAG_HostStack") static uint8_t extra_data_buffer[CFG_BLE_EXTRA_DATA_BUF_SIZE];

/* USER CODE BEGIN PV */

uint8_t antenna_ids[] = ANTENNA_IDS;
/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/

/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private function prototypes -----------------------------------------------*/
static uint8_t HOST_BLE_Init(void);
static void BLE_ResumeFlowProcessCallback(void);
static void BLE_NvmCallback(SNVMA_Callback_Status_t CbkStatus);
static void Ble_Hci_Gap_Gatt_Init(void);
static const uint8_t* BleGenerateIRValue(void);
static const uint8_t* BleGenerateERValue(void);
static void gap_cmd_resp_wait(void);
static void gap_cmd_resp_release(void);
/* USER CODE BEGIN PFP */
tBleStatus hci_le_connection_iq_report_event(uint16_t Connection_Handle,
                                             uint8_t RX_PHY,
                                             uint8_t Data_Channel_Index,
                                             uint16_t RSSI,
                                             uint8_t RSSI_Antenna_ID,
                                             uint8_t CTE_Type,
                                             uint8_t Slot_Durations,
                                             uint8_t Packet_Status,
                                             uint16_t Connection_Event_Counter,
                                             uint8_t Sample_Count,
                                             const IQ_Sample_t* IQ_Sample);
tBleStatus hci_le_cte_request_failed_event( uint8_t Status,
                                           uint16_t Connection_Handle );
static void Connect_Request_Fixed_Addr(void);
static void Connection_Complete_Event(void);
/* USER CODE END PFP */

/* External functions prototypes ---------------------------------------------*/

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Functions Definition ------------------------------------------------------*/
void APP_BLE_Init(void)
{
  /* USER CODE BEGIN APP_BLE_Init_1 */
  LOG_INFO_APP("\n\n\n\n\n\n * * * * * Application name : BLE_DirectionFinding_Central_Locator * * * * * \n\n");
  /* USER CODE END APP_BLE_Init_1 */

  LST_init_head(&BleAsynchEventQueue);

  /* Register BLE Host tasks */
  UTIL_SEQ_RegTask(1U << CFG_TASK_BLE_HOST, UTIL_SEQ_RFU, BleStack_Process_BG);
  UTIL_SEQ_RegTask(1U << CFG_TASK_HCI_ASYNCH_EVT_ID, UTIL_SEQ_RFU, Ble_UserEvtRx);

  /* Initialise NVM RAM buffer, invalidate it's content before restoration */
  host_nvm_buffer[0] = 0;

  /* Register A NVM buffer for BLE Host stack */
  SNVMA_Register(APP_BLE_NvmBuffer,
                  (uint32_t *)host_nvm_buffer,
                  (CFG_BLE_NVM_SIZE_MAX * 2));

  /* Realize a restore */
  SNVMA_Restore(APP_BLE_NvmBuffer);
  /* USER CODE BEGIN APP_BLE_Init_Buffers */

  /* USER CODE END APP_BLE_Init_Buffers */

  /* Initialize BLE related modules */
  BAES_Reset( );
  PKACTRL_Reset();
  BLE_TIMER_Init();

  /* Initialize the BLE Host */
  if (HOST_BLE_Init() == 0u)
  {
    /* Initialization of HCI & GATT & GAP layer */
    Ble_Hci_Gap_Gatt_Init();

    /* Initialization of the BLE Services */
    SVCCTL_Init();

    /**
     * Initialize GATT Client Application
     */
    GATT_CLIENT_APP_Init();
  }
  /* USER CODE BEGIN APP_BLE_Init_2 */

  /* Register a task to handle connection requests with a fixed address */
  UTIL_SEQ_RegTask(1U << CFG_TASK_CONNECT_REQUEST_FIXED_ADDRESS, UTIL_SEQ_RFU, Connect_Request_Fixed_Addr);
  
  UTIL_SEQ_SetTask(1U << CFG_TASK_BSP_BUTTON_B1, CFG_SEQ_PRIO_0);
  
  /* USER CODE END APP_BLE_Init_2 */

  return;
}

/* All BLE activities must be stopped before calling this API */
void APP_BLE_Deinit(void)
{
  /* USER CODE BEGIN APP_BLE_Deinit_1 */

  /* USER CODE END APP_BLE_Deinit_1 */

  aci_reset(0, 0);

  memset(&host_buffer[0], 0, sizeof(host_buffer));
  memset(&gatt_buffer[0], 0, sizeof(gatt_buffer));
  memset(&host_event_buffer[0], 0, sizeof(host_event_buffer));
  memset(&host_nvm_buffer[0], 0, sizeof(host_nvm_buffer));
  memset(&long_write_buffer[0], 0, sizeof(long_write_buffer));
  memset(&extra_data_buffer[0], 0, sizeof(extra_data_buffer));

  /* De-initialize BLE related modules */
  BAES_Reset( );
  PKACTRL_Reset();
  BLE_TIMER_Deinit();

  tListNode *listNodeRemoved;

  /* Free all the Asynchronous Event queue nodes */
  while(LST_is_empty(&BleAsynchEventQueue) != TRUE)
  {
    LST_remove_tail(&BleAsynchEventQueue, &listNodeRemoved);
    (void)AMM_Free((uint32_t *)listNodeRemoved);
  }

  /* USER CODE BEGIN APP_BLE_Deinit_2 */

  /* USER CODE END APP_BLE_Deinit_2 */
  return;
}

SVCCTL_UserEvtFlowStatus_t SVCCTL_App_Notification(void *p_Pckt)
{
  tBleStatus ret = BLE_STATUS_ERROR;
  hci_event_pckt    *p_event_pckt;
  evt_le_meta_event *p_meta_evt;
  evt_blecore_aci   *p_blecore_evt;
  int8_t rssi;

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

        /* Turn off all LEDs */
        APP_BSP_LED_Off(LED_BLUE);
        APP_BSP_LED_Off(LED_GREEN);
        APP_BSP_LED_Off(LED_RED);
        /* Set the task for button 1 to initiate a new connection with a fixed address */
        LOG_INFO_APP("  At any disconnection, central and peripheral will reconnect automatically.\n");
        LOG_INFO_APP("==>> SetTask TASK_BUTTON_1 to create connection with fixed address.\n");
        UTIL_SEQ_SetTask( 1U << CFG_TASK_BSP_BUTTON_B1, CFG_SEQ_PRIO_0);
        /* USER CODE END EVT_DISCONN_COMPLETE_2 */
      }
      gap_cmd_resp_release();

      /* USER CODE BEGIN EVT_DISCONN_COMPLETE_1 */

      /* USER CODE END EVT_DISCONN_COMPLETE_1 */
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

          /* USER CODE BEGIN HCI_EVT_LE_ENHANCED_CONN_COMPLETE */
          GATT_CLIENT_APP_Set_Conn_Handle(0, bleAppContext.connectionHandle);

          Connection_Complete_Event();
          
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

          /* USER CODE BEGIN HCI_EVT_LE_CONN_COMPLETE */

          Connection_Complete_Event();
          
          /* USER CODE END HCI_EVT_LE_CONN_COMPLETE */
          break; /* HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE */
        }
        case HCI_LE_ADVERTISING_REPORT_SUBEVT_CODE:
        {
          hci_le_advertising_report_event_rp0 *p_adv_report;
          p_adv_report = (hci_le_advertising_report_event_rp0 *) p_meta_evt->data;
          UNUSED(p_adv_report);

          rssi = HCI_LE_ADVERTISING_REPORT_RSSI(p_meta_evt->data);
          UNUSED(rssi);

          /* USER CODE BEGIN HCI_EVT_LE_ADVERTISING_REPORT */

          /* USER CODE END HCI_EVT_LE_ADVERTISING_REPORT */
          break; /* HCI_LE_ADVERTISING_REPORT_SUBEVT_CODE */
        }
        case HCI_LE_EXTENDED_ADVERTISING_REPORT_SUBEVT_CODE:
        {
          hci_le_extended_advertising_report_event_rp0 *p_ext_adv_report;
          p_ext_adv_report = (hci_le_extended_advertising_report_event_rp0 *) p_meta_evt->data;
          UNUSED(p_ext_adv_report);
          /* USER CODE BEGIN HCI_LE_EXTENDED_ADVERTISING_REPORT_SUBEVT_CODE */

          /* USER CODE END HCI_LE_EXTENDED_ADVERTISING_REPORT_SUBEVT_CODE */
          break; /* HCI_LE_EXTENDED_ADVERTISING_REPORT_SUBEVT_CODE */
        }
        /* USER CODE BEGIN SUBEVENT */

      case HCI_LE_CONNECTION_IQ_REPORT_SUBEVT_CODE:
        {
          hci_le_connection_iq_report_event_rp0 *p_iq_report;
          p_iq_report = (hci_le_connection_iq_report_event_rp0 *) p_meta_evt->data;
          
          /* The HCI_LE_Connection_IQ_Report event is
          * used to report IQ samples from the Constant
          * Tone Extension field of a received packet containing
          * an LL_CTE_RSP PDU.
          */
          hci_le_connection_iq_report_event(p_iq_report->Connection_Handle,
                                            p_iq_report->RX_PHY,
                                            p_iq_report->Data_Channel_Index,
                                            p_iq_report->RSSI,
                                            p_iq_report->RSSI_Antenna_ID,
                                            p_iq_report->CTE_Type,
                                            p_iq_report->Slot_Durations,
                                            p_iq_report->Packet_Status,
                                            p_iq_report->Connection_Event_Counter,
                                            p_iq_report->Sample_Count,
                                            p_iq_report->IQ_Sample);
          break; /* HCI_LE_CONNECTION_IQ_REPORT_SUBEVT_CODE */
        }
      case HCI_LE_CTE_REQUEST_FAILED_SUBEVT_CODE:
        {
          hci_le_cte_request_failed_event_rp0 *p_cte_failed;
          p_cte_failed = (hci_le_cte_request_failed_event_rp0 *) p_meta_evt->data;
          
          hci_le_cte_request_failed_event(p_cte_failed->Status, p_cte_failed->Connection_Handle);
        break; /* HCI_LE_CTE_REQUEST_FAILED_SUBEVT_CODE */
        }
        
        /* USER CODE END SUBEVENT */
        default:
        {
          LOG_DEBUG_BLE("SVCCTL_App_Notification: unhandled SUBEVENT with opcode: 0x%02X\n", p_meta_evt->subevent);
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
        case ACI_L2CAP_CONNECTION_UPDATE_REQ_VSEVT_CODE:
        {
          aci_l2cap_connection_update_req_event_rp0 *p_l2cap_conn_update_req;
          uint8_t req_resp = 0x01;
          p_l2cap_conn_update_req = (aci_l2cap_connection_update_req_event_rp0 *) p_blecore_evt->data;
          UNUSED(p_l2cap_conn_update_req);

          /* USER CODE BEGIN EVT_L2CAP_CONNECTION_UPDATE_REQ */

          /* USER CODE END EVT_L2CAP_CONNECTION_UPDATE_REQ */

          ret = aci_l2cap_connection_parameter_update_resp(p_l2cap_conn_update_req->Connection_Handle,
                                                           p_l2cap_conn_update_req->Interval_Min,
                                                           p_l2cap_conn_update_req->Interval_Max,
                                                           p_l2cap_conn_update_req->Latency,
                                                           p_l2cap_conn_update_req->Timeout_Multiplier,
                                                           CONN_CE_LENGTH_MS(10),
                                                           CONN_CE_LENGTH_MS(10),
                                                           p_l2cap_conn_update_req->Identifier,
                                                           req_resp);
          if(ret != BLE_STATUS_SUCCESS)
          {
            LOG_INFO_APP("  Fail   : aci_l2cap_connection_parameter_update_resp command\n");
          }
          else
          {
            LOG_INFO_APP("  Success: aci_l2cap_connection_parameter_update_resp command\n");
          }

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

          /* USER CODE END RADIO_ACTIVITY_EVENT */
          break; /* ACI_HAL_END_OF_RADIO_ACTIVITY_VSEVT_CODE */
        }
        case ACI_GAP_PASS_KEY_REQ_VSEVT_CODE:
        {
          uint32_t pin;
          LOG_INFO_APP(">>== ACI_GAP_PASS_KEY_REQ_VSEVT_CODE\n");

          pin = BLE_DEFAULT_PIN;
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
        case ACI_GATT_INDICATION_VSEVT_CODE:
        {
          aci_gatt_indication_event_rp0 *p_gatt_indication_event;
          LOG_INFO_APP(">>== ACI_GATT_INDICATION_VSEVT_CODE\n");

          p_gatt_indication_event = (aci_gatt_indication_event_rp0*)p_blecore_evt->data;

          /* USER CODE BEGIN ACI_GATT_INDICATION_VSEVT_CODE_0 */

          /* USER CODE END ACI_GATT_INDICATION_VSEVT_CODE_0 */

          ret = aci_gatt_confirm_indication(p_gatt_indication_event->Connection_Handle);
          if (ret != BLE_STATUS_SUCCESS)
          {
            LOG_INFO_APP("  Fail   : aci_gatt_confirm_indication command, result: 0x%02X\n", ret);
          }
          else
          {
            LOG_INFO_APP("  Success: aci_gatt_confirm_indication command\n");
          }
          /* USER CODE BEGIN ACI_GATT_INDICATION_VSEVT_CODE_1 */

          /* USER CODE END ACI_GATT_INDICATION_VSEVT_CODE_1 */
          break;
        }
        case ACI_WARNING_VSEVT_CODE:
        {
          aci_warning_event_rp0 *p_fw_warning_event;

          p_fw_warning_event = (aci_warning_event_rp0 *)p_blecore_evt->data;
          UNUSED(p_fw_warning_event);
          LOG_INFO_APP(">>== ACI_WARNING_VSEVT_CODE\n");
          LOG_INFO_APP("FW warning Type = 0x%02X\n", p_fw_warning_event->Warning_Type);
          /* USER CODE BEGIN ACI_HAL_FW_ERROR_VSEVT_CODE */

          /* USER CODE END ACI_HAL_FW_ERROR_VSEVT_CODE */
          break;
        }
        /* USER CODE BEGIN ECODE_1 */

        /* USER CODE END ECODE_1 */
        default:
        {
          LOG_DEBUG_BLE("SVCCTL_App_Notification: unhandled EVT_VENDOR with opcode: 0x%02X\n", p_blecore_evt->ecode);
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
      LOG_DEBUG_BLE("SVCCTL_App_Notification: unhandled EVENT_PCKT with opcode: 0x%02X\n", p_event_pckt->evt);
      /* USER CODE BEGIN EVENT_PCKT_DEFAULT */

      /* USER CODE END EVENT_PCKT_DEFAULT */
      break;
    }
  }
  /* USER CODE BEGIN SVCCTL_App_Notification_1 */

  /* USER CODE END SVCCTL_App_Notification_1 */

  return (SVCCTL_UserEvtFlowEnable);
}

APP_BLE_ConnStatus_t APP_BLE_Get_Client_Connection_Status(uint16_t Connection_Handle)
{
  APP_BLE_ConnStatus_t conn_status;

  if (bleAppContext.connectionHandle == Connection_Handle)
  {
    conn_status = bleAppContext.Device_Connection_Status;
  }
  else
  {
    conn_status = APP_BLE_IDLE;
  }

  return conn_status;
}

void Ble_UserEvtRx( void)
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
  BleStackCB_Process();

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
        gap_cmd_resp_wait();/* waiting for HCI_DISCONNECTION_COMPLETE_EVT_CODE */
      }
      break;
    }/* PROC_GAP_GEN_CONN_TERMINATE */

    /* USER CODE BEGIN GAP_GENERAL */

    /* USER CODE END GAP_GENERAL */
    default:
      break;
  }

  /* USER CODE BEGIN Procedure_Gap_General_2 */

  /* USER CODE END Procedure_Gap_General_2 */
  return;
}

void APP_BLE_Procedure_Gap_Central(ProcGapCentralId_t ProcGapCentralId)
{
  tBleStatus status;
  uint32_t paramA, paramB, paramC, paramD;

  UNUSED(paramA);
  UNUSED(paramB);
  UNUSED(paramC);
  UNUSED(paramD);

  /* USER CODE BEGIN Procedure_Gap_Central_1 */

  /* USER CODE END Procedure_Gap_Central_1 */

  /* First set parameters before calling ACI APIs, only if needed */
  switch(ProcGapCentralId)
  {
    case PROC_GAP_CENTRAL_SCAN_START:
    {
      paramA = SCAN_INT_MS(500);
      paramB = SCAN_WIN_MS(500);
      paramC = APP_BLE_SCANNING;

      break;
    }/* PROC_GAP_CENTRAL_SCAN_START */
    case PROC_GAP_CENTRAL_SCAN_TERMINATE:
    {
      paramA = 1;
      paramB = 1;
      paramC = APP_BLE_IDLE;

      break;
    }/* PROC_GAP_CENTRAL_SCAN_TERMINATE */
    /* USER CODE BEGIN PARAM_UPDATE */

    /* USER CODE END PARAM_UPDATE */
    default:
      break;
  }

  /* USER CODE BEGIN Procedure_Gap_Central_2 */

  /* USER CODE END Procedure_Gap_Central_2 */

  /* Call ACI APIs */
  switch(ProcGapCentralId)
  {
    case PROC_GAP_CENTRAL_SCAN_START:
    {
      status = aci_gap_start_general_discovery_proc(paramA, paramB, CFG_BD_ADDRESS_TYPE, 0);

      if (status != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("aci_gap_start_general_discovery_proc - fail, result: 0x%02X\n", status);
      }
      else
      {
        bleAppContext.Device_Connection_Status = (APP_BLE_ConnStatus_t)paramC;
        LOG_INFO_APP("==>> aci_gap_start_general_discovery_proc - Success\n");
      }

      break;
    }/* PROC_GAP_CENTRAL_SCAN_START */
    case PROC_GAP_CENTRAL_SCAN_TERMINATE:
    {
      status = aci_gap_terminate_gap_proc(GAP_GENERAL_DISCOVERY_PROC);
      if (status != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("aci_gap_terminate_gap_proc - fail, result: 0x%02X\n",status);
      }
      else
      {
        bleAppContext.Device_Connection_Status = (APP_BLE_ConnStatus_t)paramC;
        LOG_INFO_APP("==>> aci_gap_terminate_gap_proc - Success\n");
      }
      break;
    }/* PROC_GAP_CENTRAL_SCAN_TERMINATE */
    /* USER CODE BEGIN ACI_CALL_2 */

    /* USER CODE END ACI_CALL_2 */
    default:
      break;
  }

  /* USER CODE BEGIN Procedure_Gap_Central_3 */

  /* USER CODE END Procedure_Gap_Central_3 */

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
  LOG_INFO_APP("Set appearance 0x%04X in GAP database with status %d\n", appearance, ret);

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

void BleStack_Process_BG(void)
{
  if (BleStack_Process() == 0x0)
  {
    BleStackCB_Process();
  }
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
  pInitParams.extra_data_buffer_size  = CFG_BLE_EXTRA_DATA_BUF_SIZE;
  pInitParams.extra_data_buffer       = (uint8_t*)extra_data_buffer;
  pInitParams.gatt_long_write_buffer  = (uint8_t*)long_write_buffer;
  pInitParams.host_event_fifo_buffer  = host_event_buffer;
  pInitParams.host_event_fifo_buffer_size = DIVC(BLE_HOST_EVENT_BUF_SIZE, 2);
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
  uint32_t p_bd_addr[2];
  uint16_t a_appearance[1] = {CFG_GAP_APPEARANCE};
  const uint8_t *p_ir_value;
  const uint8_t *p_er_value;
  tBleStatus ret;

  /* USER CODE BEGIN Ble_Hci_Gap_Gatt_Init */

  /* USER CODE END Ble_Hci_Gap_Gatt_Init */

  LOG_INFO_APP("==>> Start Ble_Hci_Gap_Gatt_Init function\n");

  /**
   * Static random Address
   * The two upper bits shall be set to 1
   * The lowest 32bits is read from the UDN to differentiate between devices
   * The RNG may be used to provide a random number on each power on
   */
  p_bd_addr[0] = CFG_STATIC_RANDOM_ADDRESS & 0xFFFFFFFF;
  p_bd_addr[1] = (uint32_t)((uint64_t)CFG_STATIC_RANDOM_ADDRESS >> 32);
  p_bd_addr[1] |= 0xC000; /* The two upper bits shall be set to 1 */

  ret = aci_hal_write_config_data(CONFIG_DATA_RANDOM_ADDRESS_OFFSET, CONFIG_DATA_RANDOM_ADDRESS_LEN, (uint8_t*)p_bd_addr);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_hal_write_config_data command - CONFIG_DATA_RANDOM_ADDRESS_OFFSET, result: 0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_hal_write_config_data command - CONFIG_DATA_RANDOM_ADDRESS_OFFSET\n");
    LOG_INFO_APP("  Random Bluetooth Address: %02x:%02x:%02x:%02x:%02x:%02x\n", (uint8_t)(p_bd_addr[1] >> 8),
                                                                               (uint8_t)(p_bd_addr[1]),
                                                                               (uint8_t)(p_bd_addr[0] >> 24),
                                                                               (uint8_t)(p_bd_addr[0] >> 16),
                                                                               (uint8_t)(p_bd_addr[0] >> 8),
                                                                               (uint8_t)(p_bd_addr[0]));
     memcpy(&a_BdAddr[0], (uint8_t*)p_bd_addr, 6);
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
  role |= GAP_CENTRAL_ROLE;

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

    if (((role & GAP_PERIPHERAL_ROLE) != 0) || ((role & GAP_CENTRAL_ROLE) != 0))
    {
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
  bleAppContext.bleSecurityParam.bonding_mode          = CFG_BONDING_MODE;
  /* USER CODE BEGIN Ble_Hci_Gap_Gatt_Init_1 */

    /* Definition of the LE event mask */
    uint8_t LE_Event_Mask[8] = {
                                  0x1F, /* Byte 0: 
                                         * Bit 0: LE Connection Complete event
                                         * Bit 1: LE Advertising Report event
                                         * Bit 2: LE Connection Update Complete event
                                         * Bit 3: LE Read Remote Features Complete event
                                         * Bit 4: LE Long Term Key Request event
                                         */
                                  0x00,  //0x02, 
                                        /* Byte 1: 
                                         * Bit 9: LE Enhanced Connection Complete event
                                         */
                                  0x60, /* Byte 2: 
                                         * Bit 21: LE Connection IQ Report event
                                         * Bit 22: LE CTE Request Failed event
                                         */
                                  0x00, /* Byte 3: No events enabled */
                                  0x00, /* Byte 4: No events enabled */
                                  0x00, /* Byte 5: No events enabled */
                                  0x00, /* Byte 6: No events enabled */
                                  0x00  /* Byte 7: No events enabled */
                              };

  /* Call the function to set the LE event mask */
  ret = hci_le_set_event_mask(LE_Event_Mask);
  if (ret != BLE_STATUS_SUCCESS)
  {
      LOG_INFO_APP("  Fail   : hci_le_set_event_mask command, result: 0x%02X\n", ret);
  }
  else
  {
      LOG_INFO_APP("  Success: hci_le_set_event_mask command\n");
  }
  
  /* USER CODE END Ble_Hci_Gap_Gatt_Init_1 */

  ret = aci_gap_set_authentication_requirement(bleAppContext.bleSecurityParam.bonding_mode,
                                               bleAppContext.bleSecurityParam.mitm_mode,
                                               CFG_SC_SUPPORT,
                                               CFG_KEYPRESS_NOTIFICATION_SUPPORT,
                                               bleAppContext.bleSecurityParam.encryptionKeySizeMin,
                                               bleAppContext.bleSecurityParam.encryptionKeySizeMax,
                                               USE_FIXED_PIN_FOR_PAIRING_FORBIDDEN, /* deprecated feature */
                                               0,                                   /* deprecated feature */
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

  /* USER CODE END Ble_Hci_Gap_Gatt_Init_2 */

  LOG_INFO_APP("==>> End Ble_Hci_Gap_Gatt_Init function\n");

  return;
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

static void gap_cmd_resp_release(void)
{
  UTIL_SEQ_SetEvt(1U << CFG_EVENT_PROC_GAP_COMPLETE);
  return;
}

static void gap_cmd_resp_wait(void)
{
  UTIL_SEQ_WaitEvt(1U << CFG_EVENT_PROC_GAP_COMPLETE);
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


/**
* @brief Initiates a connection request to a fixed public address.
*
* This function demonstrates how to perform a connection request to a device
* with a fixed public address. It is intended for demo purposes and assumes
* that the target device's address is known and fixed.
* CFG_TASK_CONNECT_REQUEST_FIXED_ADDRESS
*
* @note Ensure that the target device with the fixed public address is
*       available and in advertising mode before calling this function.
*
* @return None
*/
static void Connect_Request_Fixed_Addr(void)
{
  tBleStatus result;
  
  
  /* Define the peer address */
  uint8_t peripheral_bd_address[6] = {0};
  peripheral_bd_address[0] = (uint8_t)((0x0280E10013E1 & 0x0000000000FF));
  peripheral_bd_address[1] = (uint8_t)((0x0280E10013E1 & 0x00000000FF00) >> 8);
  peripheral_bd_address[2] = (uint8_t)((0x0280E10013E1 & 0x000000FF0000) >> 16);
  peripheral_bd_address[3] = (uint8_t)((0x0280E10013E1 & 0x0000FF000000) >> 24);
  peripheral_bd_address[4] = (uint8_t)((0x0280E10013E1 & 0x00FF00000000) >> 32);
  peripheral_bd_address[5] = (uint8_t)((0x0280E10013E1 & 0xFF0000000000) >> 40);
  
  
  
  if (1)
  {
    LOG_INFO_APP("aci_gap_create_connection\n");

    result = aci_gap_create_connection(SCAN_INT_MS(500), SCAN_WIN_MS(500),
                                       0x00, 
                                       &peripheral_bd_address[0],
                                       CFG_BD_ADDRESS_TYPE,
                                       CONN_INT_MS(50), CONN_INT_MS(100),
                                       0,
                                       CONN_SUP_TIMEOUT_MS(5000),
                                       CONN_CE_LENGTH_MS(10), CONN_CE_LENGTH_MS(10));
    
    
    if (result == BLE_STATUS_SUCCESS)
    {
      bleAppContext.Device_Connection_Status = APP_BLE_LP_CONNECTING;
      UTIL_SEQ_WaitEvt(1u << CFG_EVENT_CONNECTION_COMPLETE);
      LOG_INFO_APP("     (FIXED ADDRESS)  aci_gap_create_connection success\n");
    }
    else
    {
      LOG_INFO_APP("==>> GAP Create connection Failed , result: 0x%02x\n", result);
      bleAppContext.Device_Connection_Status = APP_BLE_IDLE;
    }
  }
  
  return;
}

static void Connection_Complete_Event(void)
{
  tBleStatus ret = BLE_STATUS_ERROR;
  
  /* Once connected the LED2 is always on, on both devices Central and Peripheral. */
  APP_BSP_LED_On(LED_GREEN);
  
  /* Set the CTE receive parameters */
  ret = hci_le_set_connection_cte_receive_parameters(bleAppContext.connectionHandle,    /* Connection handle that identifies the connection */
                                                     ENABLE,                                /* Enable (0x01) or disable (0x00) receiving Constant Tone Extensions */
                                                     CTE_SLOT_DURATION,                     /* Slot duration (e.g., 0x01 for 1us slots, 0x02 for 2us slots) */
                                                     RX_SWITCHING_PATTERN_LENGTH,           /* Length of the antenna switching pattern */
                                                     antenna_ids                            /* List of antenna IDs in the pattern */
                                                       );
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("Failed to set CTE parameters: reason=0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_APP("==>> hci_le_set_connection_cte_receive_parameters : Success\n");
  }
  
  /* Enable the CTE requests */
  ret = hci_le_connection_cte_request_enable(bleAppContext.connectionHandle, /* Connection handle that identifies the connection */
                                             ENABLE,                             /* Enable (0x01) or disable (0x00) CTE requests */
                                             CTE_REQ_INTERVAL,                   /* Interval between CTE requests in number of connection events */
                                             MIN_CTE_LENGTH,                     /* Minimum length of the CTE in 8 microseconds units */
                                             CTE_TYPE                            /* CTE type support flags (e.g., 0x00 for AoA, 0x01 for AoD with 1us slots, 0x02 for AoD with 2us slots) */
                                               );
  
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("Failed to enable CTE requests: reason=0x%02X\n", ret);
  }
  else
  {
    LOG_INFO_APP("==>> hci_le_connection_cte_request_enable : Success\n");
  }
  
  UTIL_SEQ_SetEvt(1 << CFG_EVENT_CONNECTION_COMPLETE);
}

tBleStatus hci_le_connection_iq_report_event(uint16_t Connection_Handle,
                                             uint8_t RX_PHY,
                                             uint8_t Data_Channel_Index,
                                             uint16_t RSSI,
                                             uint8_t RSSI_Antenna_ID,
                                             uint8_t CTE_Type,
                                             uint8_t Slot_Durations,
                                             uint8_t Packet_Status,
                                             uint16_t Connection_Event_Counter,
                                             uint8_t Sample_Count,
                                             const IQ_Sample_t* IQ_Sample)
{
  LOG_INFO_APP("Connection Handle: 0x%04X\n", Connection_Handle);
  LOG_INFO_APP("RX PHY: 0x%02X\n", RX_PHY);
  LOG_INFO_APP("Data Channel Index: 0x%02X\n", Data_Channel_Index);
  LOG_INFO_APP("RSSI: %d (0.1 dBm)\n", RSSI);
  LOG_INFO_APP("RSSI Antenna ID: 0x%02X\n", RSSI_Antenna_ID);
  LOG_INFO_APP("CTE Type: 0x%02X\n", CTE_Type);
  LOG_INFO_APP("Slot Durations: 0x%02X\n", Slot_Durations);
  LOG_INFO_APP("Packet Status: 0x%02X\n", Packet_Status);
  LOG_INFO_APP("Connection Event Counter: 0x%04X\n", Connection_Event_Counter);
  LOG_INFO_APP("Sample Count: 0x%02X\n", Sample_Count);
  
  LOG_INFO_APP("IQ samples: ");
  for (int i = 0; i < Sample_Count; i++)
  {
    LOG_INFO_APP("(%d,%d)", IQ_Sample[i].I_Sample, IQ_Sample[i].Q_Sample);
  }
  LOG_INFO_APP("\n");
  
  return BLE_STATUS_SUCCESS;
}


tBleStatus hci_le_cte_request_failed_event(uint8_t Status, uint16_t Connection_Handle)
{
  LOG_INFO_APP("CTE Request failed 0x%02X.\n", Status);
  
  return BLE_STATUS_SUCCESS;
}

/* USER CODE END FD_LOCAL_FUNCTION */

/*************************************************************
 *
 * WRAP FUNCTIONS
 *
 *************************************************************/

/**
  * @brief Callback called by the BLE stack (from BleStack_Process() context)
  * to send an indication to the application. The indication is a BLE standard
  * packet that can be either an ACI/HCI event or an ACL data.
  * @param data: pointer to the data of the packet
  * @param length: length of the data of the packet
  * @param ext_data: pointer to the extended data
  * @param ext_length: extended data length
  * @retval Status of the operation
  */

tBleStatus BLECB_Indication( const uint8_t* data,
                          uint16_t length,
                          const uint8_t* ext_data,
                          uint16_t ext_length )
{
  uint8_t status = BLE_STATUS_FAILED;
  BleEvtPacket_t *phcievt = NULL;
  uint16_t total_length = (length+ext_length);

  UNUSED(ext_data);

  /* USER CODE BEGIN BLECB_Indication */

  /* USER CODE END BLECB_Indication */

  if (data[0] == HCI_EVENT_PKT_TYPE)
  {
    BLE_EVENTS_ResumeFlowProcessCb.Callback = BLE_ResumeFlowProcessCallback;
    if (AMM_Alloc (CFG_AMM_VIRTUAL_BLE_EVENTS,
                   DIVC((sizeof(BleEvtPacketHeader_t) + total_length), sizeof (uint32_t)),
                   (uint32_t **)&phcievt,
                   &BLE_EVENTS_ResumeFlowProcessCb) != AMM_ERROR_OK)
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
  if(bleAppContext.Device_Connection_Status != APP_BLE_CONNECTED_CLIENT)
  {
    UTIL_SEQ_SetTask(1u << CFG_TASK_CONNECT_REQUEST_FIXED_ADDRESS, CFG_SEQ_PRIO_0);
  }
  else
  {
    LOG_INFO_APP("==>> Device already connected\n");
  }
  return;
}

void APP_BSP_Button2Action(void)
{
  return;
}

void APP_BSP_Button3Action(void)
{
  if (bleAppContext.Device_Connection_Status == APP_BLE_CONNECTED_CLIENT)
  {
    LOG_INFO_APP("Device connected. The connection is terminated.\n");
    APP_BLE_Procedure_Gap_General(PROC_GAP_GEN_CONN_TERMINATE);
    bleAppContext.Device_Connection_Status = APP_BLE_IDLE;
  }
  return;
}
#endif
/* USER CODE END FD_WRAP_FUNCTIONS */
