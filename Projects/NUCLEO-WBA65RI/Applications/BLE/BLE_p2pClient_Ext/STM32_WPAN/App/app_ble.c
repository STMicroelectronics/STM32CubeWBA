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
#include "gatt_client_app.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_bsp.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct
{
  uint8_t  Identifier;
  uint16_t L2CAP_Length;
  uint16_t Interval_Min;
  uint16_t Interval_Max;
  uint16_t Latency;
  uint16_t Timeout_Multiplier;
} APP_BLE_p2p_Conn_Update_req_t;
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
  uint8_t deviceServerFound;
  uint8_t a_deviceServerBdAddr[BD_ADDR_SIZE];
  uint8_t a_deviceServerExtendedBdAddr[BD_ADDR_SIZE];
  uint8_t deviceServerExtendedAddressType;
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
#define L_BUFF_SIZE 256
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

static char a_GapDeviceName[] = {  'P', 'e', 'e', 'r', ' ', 't', 'o', ' ', 'P', 'e', 'e', 'r', ' ', 'C', 'l', 'i', 'e', 'n', 't', ' ', 'E', 'x', 't', 'e', 'n', 'd', 'e', 'd' }; /* Gap Device Name */

static AMM_VirtualMemoryCallbackFunction_t APP_BLE_ResumeFlowProcessCb;

/* Host stack init variables */
static BleStack_init_t pInitParams;

/* Host stack buffers */
static uint32_t host_buffer[DIVC(BLE_DYN_ALLOC_SIZE, 4)];
static uint32_t gatt_buffer[DIVC(BLE_GATT_BUF_SIZE, 4)];
static uint64_t host_nvm_buffer[CFG_BLE_NVM_SIZE_MAX];

/* USER CODE BEGIN PV */
APP_BLE_p2p_Conn_Update_req_t APP_BLE_p2p_Conn_Update_req;
char l_buff[L_BUFF_SIZE];
uint16_t l_buff_pos;
volatile uint8_t display_filter = 0;
volatile uint8_t scan_running = 0;
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
static uint8_t analyse_ext_adv_report(hci_le_extended_advertising_report_event_rp0 *p_ext_adv_report);
static void connectRequestExt(void);
/* USER CODE END PFP */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Functions Definition ------------------------------------------------------*/
void APP_BLE_Init(void)
{
  /* USER CODE BEGIN APP_BLE_Init_1 */

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

    /**
     * Initialize GATT Client Application
     */
    GATT_CLIENT_APP_Init();
  }
  /* USER CODE BEGIN APP_BLE_Init_2 */
  UTIL_SEQ_RegTask( 1u << CFG_TASK_CONN_REQ_EXT_DEV_ID, UTIL_SEQ_RFU, connectRequestExt);
  /* USER CODE END APP_BLE_Init_2 */

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
          GATT_CLIENT_APP_Set_Conn_Handle(0, p_enhanced_conn_complete->Connection_Handle);
          
          /* Discover services */
          if (bleAppContext.Device_Connection_Status == APP_BLE_CONNECTED_CLIENT)
          {
            UTIL_SEQ_SetTask(1U << CFG_TASK_DISCOVER_SERVICES_ID, CFG_SEQ_PRIO_0);
          }
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

          GATT_CLIENT_APP_Set_Conn_Handle(0, p_conn_complete->Connection_Handle);

          /* USER CODE BEGIN HCI_EVT_LE_CONN_COMPLETE */
          /* Discover services */
          if (bleAppContext.Device_Connection_Status == APP_BLE_CONNECTED_CLIENT)
          {
            UTIL_SEQ_SetTask(1U << CFG_TASK_DISCOVER_SERVICES_ID, CFG_SEQ_PRIO_0);
          }
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

          analyse_ext_adv_report(p_ext_adv_report);

          /* USER CODE END HCI_LE_EXTENDED_ADVERTISING_REPORT_SUBEVT_CODE */
          break; /* HCI_LE_EXTENDED_ADVERTISING_REPORT_SUBEVT_CODE */
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
          if ((p_gap_proc_complete->Procedure_Code == GAP_GENERAL_DISCOVERY_PROC) &&
              (p_gap_proc_complete->Status == 0x00))
          {
            LOG_INFO_APP("-- GAP_GENERAL_DISCOVERY_PROC completed\n");

            #if (CFG_LED_SUPPORTED == 1)
            BSP_LED_Off(LED_BLUE);
            #endif
            if (scan_running != 0)
            {
              APP_BLE_Procedure_Gap_Central(PROC_GAP_CENTRAL_SCAN_START);
            }

          }
          /* USER CODE END EVT_GAP_PROCEDURE_COMPLETE */
          break; /* ACI_GAP_PROC_COMPLETE_VSEVT_CODE */
        }
        case ACI_HAL_END_OF_RADIO_ACTIVITY_VSEVT_CODE:
        {
          /* USER CODE BEGIN RADIO_ACTIVITY_EVENT */

          /* USER CODE END RADIO_ACTIVITY_EVENT */
          break; /* ACI_HAL_END_OF_RADIO_ACTIVITY_VSEVT_CODE */
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
    /* USER CODE BEGIN ACI_CALL */

    /* USER CODE END ACI_CALL */
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
static uint8_t analyse_ext_adv_report(hci_le_extended_advertising_report_event_rp0 *p_ext_adv_report)
{
  uint8_t status;
  
  if ((display_filter != 0) && HAL_IS_BIT_SET(p_ext_adv_report->Event_Type, 0x0010))
  {
    status = 1;
  }
  else
  {
    status = 0;
    
    l_buff_pos = 0;
    memset(&l_buff[l_buff_pos], 0, L_BUFF_SIZE);          

    l_buff_pos += snprintf(&l_buff[l_buff_pos], L_BUFF_SIZE - l_buff_pos, 
                            "%02X:%02X:%02X:%02X:%02X:%02X | "
                            ,p_ext_adv_report->Address[5], p_ext_adv_report->Address[4]
                            ,p_ext_adv_report->Address[3] ,p_ext_adv_report->Address[2]
                            ,p_ext_adv_report->Address[1] ,p_ext_adv_report->Address[0]);
   
    if (HAL_IS_BIT_SET(p_ext_adv_report->Event_Type, 0x0010))
    {
      l_buff_pos += snprintf(&l_buff[l_buff_pos], L_BUFF_SIZE, "LEGACY   |");
    }
    else
    {
      l_buff_pos += snprintf(&l_buff[l_buff_pos], L_BUFF_SIZE, "EXTENDED |");
    }
    
    l_buff_pos += snprintf(&l_buff[l_buff_pos], L_BUFF_SIZE - l_buff_pos, "%4d | ",(int8_t)p_ext_adv_report->RSSI );
              
    if (HAL_IS_BIT_SET(p_ext_adv_report->Event_Type, 0x0001))
    {
      l_buff_pos += snprintf(&l_buff[l_buff_pos], L_BUFF_SIZE - l_buff_pos, "CONN |");
    }
    else
    {
      l_buff_pos += snprintf(&l_buff[l_buff_pos], L_BUFF_SIZE - l_buff_pos, "     |");
    }
    
    if (HAL_IS_BIT_SET(p_ext_adv_report->Event_Type, 0x0002))
    {
      if (HAL_IS_BIT_SET(p_ext_adv_report->Event_Type, 0x0008))
      {
        l_buff_pos += snprintf(&l_buff[l_buff_pos], L_BUFF_SIZE - l_buff_pos, "SCAN RSP |");
      }
      else
      {
        l_buff_pos += snprintf(&l_buff[l_buff_pos], L_BUFF_SIZE - l_buff_pos, "SCAN     |");
      }
    }
    else
    {
      l_buff_pos += snprintf(&l_buff[l_buff_pos], L_BUFF_SIZE - l_buff_pos, "         |");
    }
    
    l_buff_pos += snprintf(&l_buff[l_buff_pos], L_BUFF_SIZE - l_buff_pos, "%3d | ",p_ext_adv_report->Data_Length );
    
    if (p_ext_adv_report->Advertising_SID != 0xFF)
    {
      l_buff_pos += snprintf(&l_buff[l_buff_pos], L_BUFF_SIZE - l_buff_pos, "%2d | ",p_ext_adv_report->Advertising_SID );
    }
    
    if (HAL_IS_BIT_SET(p_ext_adv_report->Event_Type, 0x0020))
    {
      l_buff_pos += snprintf(&l_buff[l_buff_pos], L_BUFF_SIZE - l_buff_pos, "more data   | ");
    }
    else if (HAL_IS_BIT_SET(p_ext_adv_report->Event_Type, 0x0040))
    {
      l_buff_pos += snprintf(&l_buff[l_buff_pos], L_BUFF_SIZE - l_buff_pos, "trunc data | ");
    }
    else
    {
      /* Nothing to do */
    }          

    if( !(HAL_IS_BIT_SET(p_ext_adv_report->Event_Type, 0x0010)) && /* extended */
        (HAL_IS_BIT_SET(p_ext_adv_report->Event_Type, 0x0001)) )   /* connectable */
    {
      uint8_t i, adtype, adlength;
      
      i = 0;
      while ( i < p_ext_adv_report->Data_Length )
      {
        adlength = p_ext_adv_report->Data[i];
        adtype = p_ext_adv_report->Data[i + 1];
        
        if ( adtype == AD_TYPE_MANUFACTURER_SPECIFIC_DATA )
        {
          if ( (adlength >= 7) && 
               (p_ext_adv_report->Data[i + 4] == 0x02) && /* blueST v2*/
               (p_ext_adv_report->Data[i + 6] == 0x83))   /* p2pServer application */
          {
            l_buff_pos += snprintf(&l_buff[l_buff_pos], L_BUFF_SIZE - l_buff_pos, "p2pServer detected, address recorded to connect. | ");

            bleAppContext.a_deviceServerExtendedBdAddr[0] = p_ext_adv_report->Address[0];
            bleAppContext.a_deviceServerExtendedBdAddr[1] = p_ext_adv_report->Address[1];
            bleAppContext.a_deviceServerExtendedBdAddr[2] = p_ext_adv_report->Address[2];
            bleAppContext.a_deviceServerExtendedBdAddr[3] = p_ext_adv_report->Address[3];
            bleAppContext.a_deviceServerExtendedBdAddr[4] = p_ext_adv_report->Address[4];
            bleAppContext.a_deviceServerExtendedBdAddr[5] = p_ext_adv_report->Address[5];
            bleAppContext.deviceServerExtendedAddressType = p_ext_adv_report->Address_Type;

            bleAppContext.deviceServerFound = 0x01;
          }
        }
        i += adlength + 1;
      }
    }

    l_buff_pos += snprintf(&l_buff[l_buff_pos], L_BUFF_SIZE - l_buff_pos, "\n");
   
    LOG_INFO_APP("%s",l_buff);
  }
    
  return status;
}

static void connectRequestExt(void)
{
  tBleStatus result;

  if (bleAppContext.deviceServerFound != 0x00)
  {
    LOG_INFO_APP("aci_gap_create_connection\n");
    result = aci_gap_create_connection(SCAN_INT_MS(500), SCAN_WIN_MS(500),
                                       bleAppContext.deviceServerExtendedAddressType, 
                                       &bleAppContext.a_deviceServerExtendedBdAddr[0],
                                       CFG_BD_ADDRESS_TYPE,
                                       CONN_INT_MS(50), CONN_INT_MS(100),
                                       0,
                                       CONN_SUP_TIMEOUT_MS(5000),
                                       CONN_CE_LENGTH_MS(10), CONN_CE_LENGTH_MS(10));

    if (result == BLE_STATUS_SUCCESS)
    {
      bleAppContext.Device_Connection_Status = APP_BLE_LP_CONNECTING;
    }
    else
    {
      LOG_INFO_APP("==>> GAP Create connection Failed , result: 0x%02x\n", result);
      bleAppContext.Device_Connection_Status = APP_BLE_IDLE;
    }
  }

  return;
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
  if (bleAppContext.Device_Connection_Status != APP_BLE_CONNECTED_CLIENT)
  {
    if (scan_running == 1)
    {
      APP_BLE_Procedure_Gap_Central(PROC_GAP_CENTRAL_SCAN_TERMINATE);
      scan_running = 0;
    }
    else
    {
      APP_BLE_Procedure_Gap_Central(PROC_GAP_CENTRAL_SCAN_START);
      scan_running = 1;
    }
  }
  else
  {
    UTIL_SEQ_SetTask( 1u << CFG_TASK_P2PC_WRITE_CHAR_ID, CFG_SEQ_PRIO_0);
  }

  return;
}

void APP_BSP_Button2Action(void)
{
  display_filter = !display_filter;

  return;
}

void APP_BSP_Button3Action(void)
{
  if(bleAppContext.Device_Connection_Status != APP_BLE_CONNECTED_CLIENT)
  {
    UTIL_SEQ_SetTask(1u << CFG_TASK_CONN_REQ_EXT_DEV_ID, CFG_SEQ_PRIO_0);
  }
  else
  {
    LOG_INFO_APP("==>> Device already connected\n");
  }

  return;
}
#endif
/* USER CODE END FD_WRAP_FUNCTIONS */
