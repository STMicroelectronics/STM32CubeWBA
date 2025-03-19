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
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tmap_app.h"
#include "stm32wba55g_discovery.h"

#if (CFG_LCD_SUPPORTED == 1)
#include "stm32wba55g_discovery_lcd.h"
#include "stm32_lcd.h"
#include "app_menu_cfg.h"
#endif /* CFG_LCD_SUPPORTED */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef uint8_t GattService_LinkupProcState_t;
#define GATT_SERVICE_LINKUP_IDLE 0x00
#define GATT_SERVICE_LINKUP_DISC_SERVICE 0x01
#define GATT_SERVICE_LINKUP_DISC_CHAR 0x02
#define GATT_SERVICE_LINKUP_DISC_CHAR_DESC 0x04
#define GATT_SERVICE_LINKUP_COMPLETE 0x08

typedef struct
{
  GattService_LinkupProcState_t LinkupState;
  uint16_t                      StartHandle;
  uint16_t                      EndHandle;
  uint16_t                      ServiceChangedCharHandle;
  uint16_t                      ServiceChangedCharDescHandle;
  uint16_t                      ServiceChangedCharEndHandle;
}GattService_t;

typedef struct
{
  uint16_t         ConnHandle;
  uint8_t          RemoteAddressType;
  uint8_t          aRemoteAddress[6u];
  GattService_t    GattService;
} BleConn_t;
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

  /* USER CODE BEGIN BleGlobalContext_t */

  /* USER CODE END BleGlobalContext_t */
}BleGlobalContext_t;

typedef struct
{
  BleGlobalContext_t BleApplicationContext_legacy;
  /* USER CODE BEGIN PTD_1 */
  BleConn_t          BleConn[CFG_BLE_NUM_LINK];
  uint16_t           ConnHandle;
  uint16_t           GapServiceHandle;
  uint16_t           GapAppearanceCharHandle;
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
#define GATTSERVICE_GATT_DATABASE_SIZE (8u)
#define BLENVM_GATTSERVICE_HDR_LEN     (12u)
#define GATTSERVICE_NVM_TYPE           (0x0A)
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

static char a_GapDeviceName[] = {  'S', 'T', 'M', '3', '2', 'W', 'B', 'A', '_', '0', '0', '0', '0' }; /* Gap Device Name */

uint64_t buffer_nvm[CFG_BLEPLAT_NVM_MAX_SIZE] = {0};

static AMM_VirtualMemoryCallbackFunction_t APP_BLE_ResumeFlowProcessCb;

/* Host stack init variables */
static uint32_t buffer[DIVC(BLE_DYN_ALLOC_SIZE, 4)];
static uint32_t gatt_buffer[DIVC(BLE_GATT_BUF_SIZE, 4)];
static BleStack_init_t pInitParams;

/* USER CODE BEGIN PV */
extern TMAPAPP_Context_t TMAPAPP_Context;
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
static void BLE_NvmCallback (SNVMA_Callback_Status_t);
static uint8_t HOST_BLE_Init(void);
/* USER CODE BEGIN PFP */
static BleConn_t *Ble_GetConn(uint16_t ConnHandle);
static char Hex_To_Char(uint8_t Hex);
static void GATTService_RestoreDatabase(BleConn_t *pConn);
static void GATTService_StoreDatabase(BleConn_t *pConn);
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

  }
  /* USER CODE BEGIN APP_BLE_Init_2 */

  Menu_Config();

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
      UNUSED(p_disconnection_complete_event);

      /* USER CODE BEGIN EVT_DISCONN_COMPLETE_1 */
      LOG_INFO_APP(">>== HCI_DISCONNECTION_COMPLETE_EVT_CODE\n");
      LOG_INFO_APP("     - Connection Handle:   0x%04X\n     - Reason:    0x%02X\n",
                  p_disconnection_complete_event->Connection_Handle,
                  p_disconnection_complete_event->Reason);
      BleConn_t *p_conn = Ble_GetConn( p_disconnection_complete_event->Connection_Handle);
      if (p_conn != 0)
      {
        p_conn->ConnHandle = 0xFFFFu;
        p_conn->GattService.StartHandle = 0;
        p_conn->GattService.EndHandle = 0;
        p_conn->GattService.LinkupState = GATT_SERVICE_LINKUP_IDLE;
        p_conn->GattService.ServiceChangedCharHandle = 0;
        p_conn->GattService.ServiceChangedCharEndHandle = 0;
        p_conn->GattService.ServiceChangedCharHandle = 0;
      }
      TMAPAPP_LinkDisconnected(p_disconnection_complete_event->Connection_Handle,
                               p_disconnection_complete_event->Reason);

      /* USER CODE END EVT_DISCONN_COMPLETE_1 */
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
          hci_le_connection_update_complete_event_rp0 *p_conn_update_complete;
          p_conn_update_complete = (hci_le_connection_update_complete_event_rp0 *) p_meta_evt->data;
          UNUSED(p_conn_update_complete);

          /* USER CODE BEGIN EVT_LE_CONN_UPDATE_COMPLETE */
          uint16_t conn_interval_us = 0;
          conn_interval_us = p_conn_update_complete->Conn_Interval * 1250;
          LOG_INFO_APP(">>== HCI_LE_CONNECTION_UPDATE_COMPLETE_SUBEVT_CODE\n");
          LOG_INFO_APP("     - Connection Interval:   %d.%02d ms\n     - Connection latency:    %d\n     - Supervision Timeout:   %d ms\n",
                       conn_interval_us / 1000,
                       (conn_interval_us%1000) / 10,
                       p_conn_update_complete->Conn_Latency,
                       p_conn_update_complete->Supervision_Timeout*10);
          UNUSED(conn_interval_us);
          /* USER CODE END EVT_LE_CONN_UPDATE_COMPLETE */
          break;
        }
        case HCI_LE_PHY_UPDATE_COMPLETE_SUBEVT_CODE:
        {
          hci_le_phy_update_complete_event_rp0 *p_le_phy_update_complete;
          p_le_phy_update_complete = (hci_le_phy_update_complete_event_rp0*)p_meta_evt->data;
          UNUSED(p_le_phy_update_complete);

          /* USER CODE BEGIN EVT_LE_PHY_UPDATE_COMPLETE */

          /* USER CODE END EVT_LE_PHY_UPDATE_COMPLETE */
          break;
        }
        case HCI_LE_ENHANCED_CONNECTION_COMPLETE_SUBEVT_CODE:
        {
          hci_le_enhanced_connection_complete_event_rp0 *p_enhanced_conn_complete;
          p_enhanced_conn_complete = (hci_le_enhanced_connection_complete_event_rp0 *) p_meta_evt->data;
          UNUSED(p_enhanced_conn_complete);
          /* USER CODE BEGIN HCI_EVT_LE_ENHANCED_CONN_COMPLETE */
          uint16_t conn_interval_us = 0;
          uint8_t type, a_address[6];
          conn_interval_us = p_enhanced_conn_complete->Conn_Interval * 1250;
          LOG_INFO_APP(">>== HCI_LE_ENHANCED_CONNECTION_COMPLETE_SUBEVT_CODE - Connection handle: 0x%04X\n", p_enhanced_conn_complete->Connection_Handle);
          LOG_INFO_APP("     - Connection established with @:%02x:%02x:%02x:%02x:%02x:%02x\n",
                      p_enhanced_conn_complete->Peer_Address[5],
                      p_enhanced_conn_complete->Peer_Address[4],
                      p_enhanced_conn_complete->Peer_Address[3],
                      p_enhanced_conn_complete->Peer_Address[2],
                      p_enhanced_conn_complete->Peer_Address[1],
                      p_enhanced_conn_complete->Peer_Address[0]);
          LOG_INFO_APP("     - Connection Interval:   %d.%02d ms\n     - Connection latency:    %d\n     - Supervision Timeout:   %d ms\n",
                      conn_interval_us / 1000,
                      (conn_interval_us%1000) / 10,
                      p_enhanced_conn_complete->Conn_Latency,
                      p_enhanced_conn_complete->Supervision_Timeout * 10
                     );
          if (aci_gap_check_bonded_device(p_enhanced_conn_complete->Peer_Address_Type,
                                      &p_enhanced_conn_complete->Peer_Address[0],
                                      &type,
                                      a_address ) == BLE_STATUS_SUCCESS)
          {
            LOG_INFO_APP(">>== device is already bonded\n");
          }
          else
          {
            LOG_INFO_APP(">>== device is not bonded\n");
          }
          if (p_enhanced_conn_complete->Status == 0)
          {
            BleConn_t *p_conn;
            for (uint8_t conn = 0; conn < CFG_BLE_NUM_LINK ; conn++)
            {
              if (bleAppContext.BleConn[conn].ConnHandle == 0xFFFF)
              {
                p_conn = &bleAppContext.BleConn[conn];
                bleAppContext.BleConn[conn].ConnHandle = p_enhanced_conn_complete->Connection_Handle;
                break;
              }
            }
            if (p_enhanced_conn_complete->Role == 0x01)
            {
              uint8_t status;
              status = aci_gap_peripheral_security_req(p_enhanced_conn_complete->Connection_Handle);
              LOG_INFO_APP(">>== aci_gap_peripheral_security_req executed with status %02X\n", status);
              if (status == BLE_STATUS_SUCCESS)
              {
                bleAppContext.ConnHandle = p_enhanced_conn_complete->Connection_Handle;
              }
            }

            p_conn->RemoteAddressType = p_enhanced_conn_complete->Peer_Address_Type;
            MEMCPY(p_conn->aRemoteAddress, p_enhanced_conn_complete->Peer_Address, 6u);
            GATTService_RestoreDatabase(p_conn);

            TMAPAPP_AclConnected(p_enhanced_conn_complete->Connection_Handle,
                                p_enhanced_conn_complete->Peer_Address_Type,
                                p_enhanced_conn_complete->Peer_Address,
                                p_enhanced_conn_complete->Role);

          }

          Menu_SetConnectingPage();
          UNUSED(conn_interval_us);

          /* USER CODE END HCI_EVT_LE_ENHANCED_CONN_COMPLETE */
          break; /* HCI_LE_ENHANCED_CONNECTION_COMPLETE_SUBEVT_CODE */
        }
        case HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE:
        {
          hci_le_connection_complete_event_rp0 *p_conn_complete;
          p_conn_complete = (hci_le_connection_complete_event_rp0 *) p_meta_evt->data;
          UNUSED(p_conn_complete);
          /* USER CODE BEGIN HCI_EVT_LE_CONN_COMPLETE */
          uint16_t conn_interval_us = 0;
          uint8_t type, a_address[6];
          conn_interval_us = p_conn_complete->Conn_Interval * 1250;
          LOG_INFO_APP(">>== HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE - Connection handle: 0x%04X\n", p_conn_complete->Connection_Handle);
          LOG_INFO_APP("     - Connection established with @:%02x:%02x:%02x:%02x:%02x:%02x\n",
                      p_conn_complete->Peer_Address[5],
                      p_conn_complete->Peer_Address[4],
                      p_conn_complete->Peer_Address[3],
                      p_conn_complete->Peer_Address[2],
                      p_conn_complete->Peer_Address[1],
                      p_conn_complete->Peer_Address[0]);
          LOG_INFO_APP("     - Connection Interval:   %d.%02d ms\n     - Connection latency:    %d\n     - Supervision Timeout:   %d ms\n",
                      conn_interval_us / 1000,
                      (conn_interval_us%1000) / 10,
                      p_conn_complete->Conn_Latency,
                      p_conn_complete->Supervision_Timeout * 10
                     );
          if (aci_gap_check_bonded_device(p_conn_complete->Peer_Address_Type,
                                          &p_conn_complete->Peer_Address[0],
                                          &type,
                                          a_address ) == BLE_STATUS_SUCCESS)
          {
            LOG_INFO_APP(">>== device is already bonded\n");
          }
          else
          {
            LOG_INFO_APP(">>== device is not bonded\n");
          }
          if (p_conn_complete->Status == 0)
          {
            BleConn_t *p_conn;
            for (uint8_t conn = 0; conn < CFG_BLE_NUM_LINK ; conn++)
            {
              if (bleAppContext.BleConn[conn].ConnHandle == 0xFFFF)
              {
                p_conn = &bleAppContext.BleConn[conn];
                bleAppContext.BleConn[conn].ConnHandle = p_conn_complete->Connection_Handle;
                break;
              }
            }
            if (p_conn_complete->Role == 0x01)
            {
              uint8_t status;
              status = aci_gap_peripheral_security_req(p_conn_complete->Connection_Handle);
              LOG_INFO_APP(">>== aci_gap_peripheral_security_req executed with status %02X\n", status);
              if (status == BLE_STATUS_SUCCESS)
              {
                bleAppContext.ConnHandle = p_conn_complete->Connection_Handle;
              }
            }

            p_conn->RemoteAddressType = p_conn_complete->Peer_Address_Type;
            MEMCPY(p_conn->aRemoteAddress, p_conn_complete->Peer_Address, 6u);
            GATTService_RestoreDatabase(p_conn);

            TMAPAPP_AclConnected(p_conn_complete->Connection_Handle,
                                p_conn_complete->Peer_Address_Type,
                                p_conn_complete->Peer_Address,
                                p_conn_complete->Role);

          }

          Menu_SetConnectingPage();
          UNUSED(conn_interval_us);

          /* USER CODE END HCI_EVT_LE_CONN_COMPLETE */
          break; /* HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE */
        }
        /* USER CODE BEGIN SUBEVENT */
        case HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_SUBEVT_CODE:
        {
          hci_le_remote_connection_parameter_request_event_rp0 *req_event = (hci_le_remote_connection_parameter_request_event_rp0 *)p_meta_evt->data;
          hci_le_remote_connection_parameter_request_reply(req_event->Connection_Handle,
                                                           req_event->Interval_Min,
                                                           req_event->Interval_Max,
                                                           req_event->Max_Latency,
                                                           req_event->Timeout,
                                                           0,
                                                           0);
          break; /* HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_SUBEVT_CODE */
        }
        case HCI_LE_CIS_ESTABLISHED_SUBEVT_CODE:
        {
            hci_le_cis_established_event_rp0 *p_cis_established_event;
            p_cis_established_event = (hci_le_cis_established_event_rp0 *) p_meta_evt->data;
            LOG_INFO_APP(">>== HCI_LE_CIS_ESTABLISHED_SUBEVT_CODE - CIS Connection handle: 0x%04X  - Status 0x%02X\n",
                         p_cis_established_event->Connection_Handle,
                         p_cis_established_event->Status);
            if (p_cis_established_event->Status == BLE_STATUS_SUCCESS)
            {
              TMAPAPP_CISConnected(p_cis_established_event->Connection_Handle);
            }
        }
        break;
        case HCI_LE_CIS_ESTABLISHED_V2_SUBEVT_CODE:
        {
            hci_le_cis_established_v2_event_rp0 *p_cis_established_event;
            p_cis_established_event = (hci_le_cis_established_v2_event_rp0 *) p_meta_evt->data;
            LOG_INFO_APP(">>== HCI_LE_CIS_ESTABLISHED_V2_SUBEVT_CODE - CIS Connection handle: 0x%04X  - Status 0x%02X\n",
                         p_cis_established_event->Connection_Handle,
                         p_cis_established_event->Status);
            if (p_cis_established_event->Status == BLE_STATUS_SUCCESS)
            {
              TMAPAPP_CISConnected(p_cis_established_event->Connection_Handle);
            }
        }
        break;
#if ((APP_TMAP_ROLE & TMAP_ROLE_BROADCAST_MEDIA_RECEIVER) == TMAP_ROLE_BROADCAST_MEDIA_RECEIVER)
        case HCI_LE_PERIODIC_ADVERTISING_SYNC_TRANSFER_RECEIVED_SUBEVT_CODE:
        {
          hci_le_periodic_advertising_sync_transfer_received_event_rp0 *past_received_event =
          (hci_le_periodic_advertising_sync_transfer_received_event_rp0 *) p_meta_evt->data;
          LOG_INFO_APP(">>== HCI_LE_PERIODIC_ADVERTISING_SYNC_TRANSFER_RECEIVED_SUBEVT_CODE - ACL Connection handle: 0x%04X  - Status 0x%02X\n",
                       past_received_event->Connection_Handle,
                       past_received_event->Status);
          if (past_received_event->Status == BLE_STATUS_SUCCESS)
          {
            TMAPAPP_Context.BSNK.PASyncState = APP_PA_SYNC_STATE_SYNCHRONIZED;
          }
        }
        break;
#endif /* ((APP_TMAP_ROLE & TMAP_ROLE_BROADCAST_MEDIA_RECEIVER) == TMAP_ROLE_BROADCAST_MEDIA_RECEIVER) */

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
        case ACI_GAP_KEYPRESS_NOTIFICATION_VSEVT_CODE:
        {
          LOG_INFO_APP(">>== ACI_GAP_KEYPRESS_NOTIFICATION_VSEVT_CODE\n");
          break;
        }
        case ACI_GAP_PASS_KEY_REQ_VSEVT_CODE:
        {
          aci_gap_pass_key_req_event_rp0 *p_pass_key;
          p_pass_key = (aci_gap_pass_key_req_event_rp0*)p_blecore_evt->data;
          LOG_INFO_APP(">>== ACI_GAP_PASS_KEY_REQ_VSEVT_CODE\n");

          ret = aci_gap_pass_key_resp(p_pass_key->Connection_Handle, CFG_FIXED_PIN);
          if (ret != BLE_STATUS_SUCCESS)
          {
            LOG_INFO_APP("==>> aci_gap_pass_key_resp : Fail, reason: 0x%02X\n", ret);
          }
          else
          {
            LOG_INFO_APP("==>> aci_gap_pass_key_resp : Success\n");
          }
          break;
        }
        case ACI_GAP_NUMERIC_COMPARISON_VALUE_VSEVT_CODE:
        {
          uint8_t confirm_value;
          aci_gap_numeric_comparison_value_event_rp0 *p_numeric_comparison;
          p_numeric_comparison = (aci_gap_numeric_comparison_value_event_rp0*)p_blecore_evt->data;
          LOG_INFO_APP(">>== ACI_GAP_NUMERIC_COMPARISON_VALUE_VSEVT_CODE\n");
          LOG_INFO_APP("     - numeric_value = %ld\n",
                      ((aci_gap_numeric_comparison_value_event_rp0 *)(p_blecore_evt->data))->Numeric_Value);
          LOG_INFO_APP("     - Hex_value = %lx\n",
                      ((aci_gap_numeric_comparison_value_event_rp0 *)(p_blecore_evt->data))->Numeric_Value);

          /* Set confirm value to 1(YES) */
          confirm_value = 1;

          ret = aci_gap_numeric_comparison_value_confirm_yesno(p_numeric_comparison->Connection_Handle, confirm_value);
          if (ret != BLE_STATUS_SUCCESS)
          {
            LOG_INFO_APP("==>> aci_gap_numeric_comparison_value_confirm_yesno : Fail, reason: 0x%02X\n", ret);
          }
          else
          {
            LOG_INFO_APP("==>> aci_gap_numeric_comparison_value_confirm_yesno : Success\n");
          }
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

          if (p_pairing_complete->Status == BLE_STATUS_SUCCESS)
          {
            BleConn_t *p_conn = Ble_GetConn( p_pairing_complete->Connection_Handle);
            const UUID_t uuid = {GENERIC_ATTRIBUTE_SERVICE_UUID};
            tBleStatus status;
            LOG_INFO_APP("Pairing Complete with connection handle 0x%04X\n", p_pairing_complete->Connection_Handle);
            Menu_SetNoStreamPage();

            if (p_conn != 0)
            {
              if (p_conn->GattService.StartHandle == 0x0000)
              {
                /* Start Gatt Service Discovery */
                status = aci_gatt_disc_primary_service_by_uuid(p_pairing_complete->Connection_Handle, 0x01, &uuid);
                LOG_INFO_APP(">>== aci_gatt_disc_primary_service_by_uuid with status %02X\n", status);

                if (status == BLE_STATUS_SUCCESS)
                {
                  p_conn->GattService.LinkupState = GATT_SERVICE_LINKUP_DISC_SERVICE;
                }
                else
                {
                  TMAPAPP_Linkup(p_pairing_complete->Connection_Handle);
                }
              }
              else
              {
                /* Start TMAP App Linkup */
                TMAPAPP_Linkup(p_pairing_complete->Connection_Handle);
              }
            }
          }
          break;
        }
        case ACI_GAP_BOND_LOST_VSEVT_CODE:
        {
          tBleStatus status;
          LOG_INFO_APP(">>== ACI_GAP_BOND_LOST_EVENT\n");

          TMAPAPP_BondLost(bleAppContext.ConnHandle);

          status = aci_gap_allow_rebond(bleAppContext.ConnHandle);
          if (status != BLE_STATUS_SUCCESS)
          {
            LOG_INFO_APP("  Fail   : aci_gap_allow_rebond command, result: 0x%02X\n", status);
          }
          else
          {
            LOG_INFO_APP("  Success: aci_gap_allow_rebond command\n");
          }
          UNUSED(status);
          break;
        }

        /* USER CODE END ECODE */
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
        case ACI_GATT_INDICATION_VSEVT_CODE:
        {
          aci_gatt_indication_event_rp0 *p_indication;
          tBleStatus ret;
          p_indication = (aci_gatt_indication_event_rp0*)p_blecore_evt->data;
          BleConn_t *p_conn = Ble_GetConn( p_indication->Connection_Handle);
          LOG_INFO_APP(">>== ACI_GATT_INDICATION_VSEVT_CODE\n");
          LOG_INFO_APP(">>== Connection_Handle : 0x%04X\n", p_indication->Connection_Handle);
          LOG_INFO_APP(">>== Attribute_Handle : 0x%04X\n", p_indication->Attribute_Handle);
          LOG_INFO_APP(">>== Attribute_Value_Length : 0x%02X\n", p_indication->Attribute_Value_Length);
          LOG_INFO_APP(">>== ACI_GATT_INDICATION_VSEVT_CODE\n");

          if (p_conn != 0)
          {
            if (p_indication->Attribute_Handle == p_conn->GattService.ServiceChangedCharDescHandle)
            {
              LOG_INFO_APP(">>== Service Changed Notified\n");
              /* Remote device notifies Service Changed : perform complete linkup */
              TMAPAPP_BondLost(p_indication->Connection_Handle);
            }
          }

          ret = aci_gatt_confirm_indication(p_indication->Connection_Handle);
          if (ret != BLE_STATUS_SUCCESS)
          {
            /* Impossible to confirm indication due to GATT process ongoing. Notify tmap_app */
            TMAPAPP_ConfirmIndicationRequired(p_indication->Connection_Handle);
            LOG_INFO_APP("  Fail   : aci_gatt_confirm_indication command, result: 0x%02X\n", ret);
          }
          else
          {
            LOG_INFO_APP("  Success: aci_gatt_confirm_indication command\n");
          }
          break;
        }
        case ACI_L2CAP_COC_CONNECT_VSEVT_CODE:
        {
          aci_l2cap_coc_connect_event_rp0 *p_coc_connect;
          p_coc_connect = (aci_l2cap_coc_connect_event_rp0*)p_blecore_evt->data;
          LOG_INFO_APP(">>== ACI_L2CAP_COC_CONNECT_VSEVT_CODE\n");
          LOG_INFO_APP(">>== Connection_Handle : 0x%04X\n", p_coc_connect->Connection_Handle);
          aci_l2cap_coc_connect_confirm(p_coc_connect->Connection_Handle,
                                        p_coc_connect->MTU,
                                        p_coc_connect->MPS,
                                        p_coc_connect->Initial_Credits,
                                        0x01,
                                        0x00,
                                        0x00);
          break;
        }
        case ACI_L2CAP_CONNECTION_UPDATE_REQ_VSEVT_CODE:
        {
          aci_l2cap_connection_update_req_event_rp0 *p_conn_update;
          p_conn_update = (aci_l2cap_connection_update_req_event_rp0*)p_blecore_evt->data;
          LOG_INFO_APP(">>== ACI_L2CAP_CONNECTION_UPDATE_REQ_VSEVT_CODE\n");
          LOG_INFO_APP(">>== Connection_Handle : 0x%04X\n", p_conn_update->Connection_Handle);
          LOG_INFO_APP(">>== Interval_Min : 0x%04X\n", p_conn_update->Interval_Min);
          LOG_INFO_APP(">>== Interval_Max : 0x%04X\n", p_conn_update->Interval_Max);
          LOG_INFO_APP(">>== Latency : 0x%04X\n", p_conn_update->Latency);
          LOG_INFO_APP(">>== Timeout_Multiplier : 0x%04X\n", p_conn_update->Timeout_Multiplier);
          LOG_INFO_APP(">>== Identifier : 0x%02X\n", p_conn_update->Identifier);
          aci_l2cap_connection_parameter_update_resp(p_conn_update->Connection_Handle,
                                                     p_conn_update->Interval_Min,
                                                     p_conn_update->Interval_Max,
                                                     p_conn_update->Latency,
                                                     p_conn_update->Timeout_Multiplier,
                                                     0x00,
                                                     0x00,
                                                     p_conn_update->Identifier,
                                                     0x00);
          break;
        }

        case ACI_ATT_FIND_BY_TYPE_VALUE_RESP_VSEVT_CODE:
        {
          aci_att_find_by_type_value_resp_event_rp0 *pr = (void*)p_blecore_evt->data;
          BleConn_t *p_conn = Ble_GetConn( pr->Connection_Handle);
          LOG_INFO_APP(">>== ACI_ATT_FIND_BY_TYPE_VALUE_RESP_VSEVT_CODE\n");
          if (p_conn != 0)
          {
            if (p_conn->GattService.LinkupState == GATT_SERVICE_LINKUP_DISC_SERVICE && pr->Num_of_Handle_Pair == 1)
            {
              /* Retrieve Gatt Service Handles */
              p_conn->GattService.StartHandle = pr->Attribute_Group_Handle_Pair[0].Found_Attribute_Handle;
              p_conn->GattService.EndHandle = pr->Attribute_Group_Handle_Pair[0].Group_End_Handle;
            }
          }
          break;
        }

        case ACI_GATT_DISC_READ_CHAR_BY_UUID_RESP_VSEVT_CODE:
        {
          aci_gatt_disc_read_char_by_uuid_resp_event_rp0 *pr = (void*)p_blecore_evt->data;
          BleConn_t *p_conn = Ble_GetConn( pr->Connection_Handle);
          LOG_INFO_APP(">>== ACI_GATT_DISC_READ_CHAR_BY_UUID_RESP_VSEVT_CODE\n");
          if (p_conn != 0)
          {
            if (p_conn->GattService.LinkupState == GATT_SERVICE_LINKUP_DISC_CHAR)
            {
              /* Retrieve Service Changed Characteristic Handle */
              p_conn->GattService.ServiceChangedCharHandle = pr->Attribute_Handle;
            }
          }
          break;
        }

        case ACI_ATT_FIND_INFO_RESP_VSEVT_CODE:
        {
          aci_att_find_info_resp_event_rp0 *pr = (void*)p_blecore_evt->data;
          BleConn_t *p_conn = Ble_GetConn( pr->Connection_Handle);
          LOG_INFO_APP(">>== ACI_ATT_FIND_INFO_RESP_VSEVT_CODE\n");

          if (p_conn != 0)
          {
            if ((p_conn->GattService.LinkupState == GATT_SERVICE_LINKUP_DISC_CHAR_DESC) \
                && (pr->Event_Data_Length >= 4) \
                && (pr->Handle_UUID_Pair[2] + (pr->Handle_UUID_Pair[3] << 8) == SERVICE_CHANGED_CHARACTERISTIC_UUID))
            {
              /* Retrieve Service Changed Characteristic Descriptor Handle */
              p_conn->GattService.ServiceChangedCharDescHandle = pr->Handle_UUID_Pair[0] + (pr->Handle_UUID_Pair[1] << 8);
            }
          }
          break;
        }

        case ACI_GATT_PROC_COMPLETE_VSEVT_CODE:
        {
          tBleStatus status;
          aci_gatt_proc_complete_event_rp0 *pr = (void*)p_blecore_evt->data;
          BleConn_t *p_conn = Ble_GetConn( pr->Connection_Handle);
          LOG_INFO_APP(">>== ACI_GATT_PROC_COMPLETE_VSEVT_CODE\n");


          if ((p_conn != 0) && (pr->Error_Code == BLE_STATUS_SUCCESS))
          {
            if (p_conn->GattService.LinkupState == GATT_SERVICE_LINKUP_DISC_SERVICE)
            {
              if (p_conn->GattService.StartHandle != 0)
              {
                const UUID_t uuid = { SERVICE_CHANGED_CHARACTERISTIC_UUID };
                /* Start Service Changed Characteristic Discovery */
                status = aci_gatt_disc_char_by_uuid(p_conn->ConnHandle,
                                                    p_conn->GattService.StartHandle,
                                                    p_conn->GattService.EndHandle,
                                                    0x01, &uuid);
                LOG_INFO_APP(">>== aci_gatt_disc_char_by_uuid with status %02X\n", status);

                if (status == BLE_STATUS_SUCCESS)
                {
                  p_conn->GattService.LinkupState = GATT_SERVICE_LINKUP_DISC_CHAR;
                }
                else
                {
                  TMAPAPP_Linkup(p_conn->ConnHandle);
                }
              }
              else
              {
                /* No Gatt Service found */
                TMAPAPP_Linkup(p_conn->ConnHandle);
              }
            }
            else if (p_conn->GattService.LinkupState == GATT_SERVICE_LINKUP_DISC_CHAR)
            {
              if (p_conn->GattService.ServiceChangedCharHandle != 0)
              {
                tBleStatus status;
                /* Start Service Changed Characteristic Descriptor */
                status  = aci_gatt_disc_all_char_desc(p_conn->ConnHandle,
                                                      p_conn->GattService.ServiceChangedCharHandle,
                                                      p_conn->GattService.EndHandle);
                LOG_INFO_APP(">>== aci_gatt_disc_all_char_desc with status %02X\n", status);

                if (status == BLE_STATUS_SUCCESS)
                {
                  p_conn->GattService.LinkupState = GATT_SERVICE_LINKUP_DISC_CHAR_DESC;
                }
                else
                {
                  /* No Service changed characteristic */
                  TMAPAPP_Linkup(p_conn->ConnHandle);
                }
              }
              else
              {
                TMAPAPP_Linkup(p_conn->ConnHandle);
              }
            }
            else if (p_conn->GattService.LinkupState == GATT_SERVICE_LINKUP_DISC_CHAR_DESC)
            {
              /* End of Gatt Service Linkup: Store database and start TMAP App Linkup */
              if (p_conn->GattService.ServiceChangedCharDescHandle != 0)
              {
                p_conn->GattService.LinkupState = GATT_SERVICE_LINKUP_COMPLETE;
                GATTService_StoreDatabase(p_conn);

                LOG_INFO_APP("Discovered GATT Service\n");
                LOG_INFO_APP("StartHandle=%04d\n", p_conn->GattService.StartHandle);
                LOG_INFO_APP("EndHandle=%04d\n", p_conn->GattService.EndHandle);
                LOG_INFO_APP("ServiceChangedCharHandle=%04d\n", p_conn->GattService.ServiceChangedCharHandle);
                LOG_INFO_APP("ServiceChangedCharDescHandle=%04d\n", p_conn->GattService.ServiceChangedCharDescHandle);
              }
              TMAPAPP_Linkup(p_conn->ConnHandle);
            }
          }

          break;
        }
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

const uint8_t* BleGetBdAddress(void)
{
  const uint8_t *p_bd_addr;

  p_bd_addr = (const uint8_t *)a_BdAddr;

  return p_bd_addr;
}

/* USER CODE BEGIN FD */
tBleStatus SetGapAppearance(uint16_t Appearance)
{
  tBleStatus ret;

  ret = aci_gatt_update_char_value(bleAppContext.GapServiceHandle,
                                   bleAppContext.GapAppearanceCharHandle,
                                   0,
                                   2,
                                   (uint8_t *)&Appearance);
  LOG_INFO_APP("Set Apperance %04X in Gap Database with status %d\n", Appearance, ret);

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
  uint8_t a_hci_commandParams[8] = {0xFFu,0xF7u,0x86u,0xBFu,0x03u,0x00u,0x00u,0x00u};
  /* Mask the HAL Events*/
  aci_hal_set_event_mask(0x00000000);

  /**
   * Set LE Event Mask
   */
  hci_le_set_event_mask(&a_hci_commandParams[0]);
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

  /* USER CODE BEGIN Role_Mngt */
  role |= (GAP_PERIPHERAL_ROLE | GAP_OBSERVER_ROLE);
  a_GapDeviceName[9] = Hex_To_Char((p_bd_addr[1] & 0xF0) >> 4);
  a_GapDeviceName[10] = Hex_To_Char(p_bd_addr[1] & 0x0F);
  a_GapDeviceName[11] = Hex_To_Char((p_bd_addr[0] & 0xF0) >> 4);
  a_GapDeviceName[12] = Hex_To_Char(p_bd_addr[0] & 0x0F);
  a_appearance[0] = GAP_APPEARANCE_EARBUD;
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
  bleAppContext.GapServiceHandle = gap_service_handle;
  bleAppContext.GapAppearanceCharHandle = gap_appearance_char_handle;
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
  for (uint8_t conn = 0; conn < CFG_BLE_NUM_LINK ; conn++)
  {
    bleAppContext.BleConn[conn].ConnHandle = 0xFFFFu;
    bleAppContext.BleConn[conn].GattService.StartHandle = 0;
    bleAppContext.BleConn[conn].GattService.EndHandle = 0;
    bleAppContext.BleConn[conn].GattService.LinkupState = GATT_SERVICE_LINKUP_IDLE;
    bleAppContext.BleConn[conn].GattService.ServiceChangedCharHandle = 0;
    bleAppContext.BleConn[conn].GattService.ServiceChangedCharEndHandle = 0;
    bleAppContext.BleConn[conn].GattService.ServiceChangedCharHandle = 0;
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
static BleConn_t *Ble_GetConn(uint16_t ConnHandle)
{
  for (uint8_t conn = 0; conn < CFG_BLE_NUM_LINK ; conn++)
  {
    if (bleAppContext.BleConn[conn].ConnHandle == ConnHandle)
    {
      return &bleAppContext.BleConn[conn];
    }
  }
  return 0;
}

static char Hex_To_Char(uint8_t Hex)
{
  if (Hex < 0xA)
  {
    return (char) Hex + 48;
  }
  else
  {
    return (char) Hex + 55;
  }
}

static void GATTService_StoreDatabase(BleConn_t *pConn)
{
#if (CFG_LOG_SUPPORTED != 0)
  tBleStatus status;
#endif /*(CFG_LOG_SUPPORTED != 0)*/
  uint8_t addr[6u];
  uint8_t type;
  uint8_t temp_database[GATTSERVICE_GATT_DATABASE_SIZE];
  uint32_t hdr[BLENVM_GATTSERVICE_HDR_LEN / 4];
  uint8_t mode = NVM_FIRST;
  int res;

  if (aci_gap_check_bonded_device(pConn->RemoteAddressType,
                                  &pConn->aRemoteAddress[0],
                                  &type,
                                  addr) == BLE_STATUS_SUCCESS)
  {
    temp_database[0] = pConn->GattService.StartHandle & 0xFF;
    temp_database[1] = (pConn->GattService.StartHandle >> 8) & 0xFF;
    temp_database[2] = pConn->GattService.EndHandle & 0xFF;
    temp_database[3] = (pConn->GattService.EndHandle >> 8) & 0xFF;
    temp_database[4] = pConn->GattService.ServiceChangedCharHandle & 0xFF;
    temp_database[5] = (pConn->GattService.ServiceChangedCharHandle >> 8) & 0xFF;
    temp_database[6] = pConn->GattService.ServiceChangedCharDescHandle & 0xFF;
    temp_database[7] = (pConn->GattService.ServiceChangedCharDescHandle >> 8) & 0xFF;

    while (1)
    {
      /* Get the header part of each record in order to find if there is already in NVM some data corresponding
       * to the same remote device address
       */
      res = NVM_Get(mode, GATTSERVICE_NVM_TYPE, 0, (uint8_t*)hdr, BLENVM_GATTSERVICE_HDR_LEN);
      mode = NVM_NEXT;
      if (res == NVM_EOF)
      {
        /* Reached EOF */
        break;
      }

      if (MEMCMP(&((uint8_t*)hdr)[1], addr, 6u) == 0)
      {
        break;
      }
    }

    if (res != NVM_EOF)
    {
      if (NVM_Compare(BLENVM_GATTSERVICE_HDR_LEN, temp_database, GATTSERVICE_GATT_DATABASE_SIZE) == 0)
      {
        /* No Database change */
        LOG_INFO_APP("No change in Gatt Service database\n");
        return;
      }

      /* Invalidate current record because data has changed */
      NVM_Discard(2);
      LOG_INFO_APP("Discard Gatt Service NVM Record\n");
    }

    ((uint8_t*)(hdr + 0))[0] = 0xFDU;
    ((uint8_t*)(hdr + 0))[1] = addr[0];
    ((uint8_t*)(hdr + 0))[2] = addr[1];
    ((uint8_t*)(hdr + 0))[3] = addr[2];
    ((uint8_t*)(hdr + 1))[0] = addr[3];
    ((uint8_t*)(hdr + 1))[1] = addr[4];
    ((uint8_t*)(hdr + 1))[2] = addr[5];
    ((uint8_t*)(hdr + 1))[3] = 0xFFU;

    hdr[2] = GATTSERVICE_GATT_DATABASE_SIZE;

#if (CFG_LOG_SUPPORTED != 0)
    status =
#endif /*(CFG_LOG_SUPPORTED != 0)*/
    NVM_Add( GATTSERVICE_NVM_TYPE,
            (uint8_t*)hdr,
            BLENVM_GATTSERVICE_HDR_LEN,
            temp_database,
            GATTSERVICE_GATT_DATABASE_SIZE );
    LOG_INFO_APP("Added Gatt Service NVM record with status 0x%02X\n", status);
  }
}

static void GATTService_RestoreDatabase(BleConn_t *pConn)
{
  uint8_t type;
  uint8_t addr[6u];
  uint8_t temp_database[GATTSERVICE_GATT_DATABASE_SIZE] = {0};
  int res;
  uint8_t mode = NVM_FIRST;
  uint32_t hdr[BLENVM_GATTSERVICE_HDR_LEN / 4];

  if (aci_gap_check_bonded_device(pConn->RemoteAddressType,
                                  &pConn->aRemoteAddress[0],
                                  &type,
                                  addr) == BLE_STATUS_SUCCESS)
  {

    while (1)
    {
      /* Get the header part of each record in order to find if there is already in NVM some data corresponding
       * to the same remote device address
       */
      res = NVM_Get( mode, GATTSERVICE_NVM_TYPE,0, (uint8_t*)hdr, BLENVM_GATTSERVICE_HDR_LEN );
      mode = NVM_NEXT;
      if (res == NVM_EOF)
      {
        /* Reached EOF */
        break;
      }

      if (MEMCMP(&((uint8_t*)hdr)[1], addr, 6u) == 0)
      {
        /* Found Record */
        break;
      }
    }

    if (res != NVM_EOF)
    {
      (void) NVM_Get(0, GATTSERVICE_NVM_TYPE, BLENVM_GATTSERVICE_HDR_LEN, temp_database, GATTSERVICE_GATT_DATABASE_SIZE);

      pConn->GattService.StartHandle = temp_database[0] + (temp_database[1] << 8);
      pConn->GattService.EndHandle = temp_database[2] + (temp_database[3] << 8);
      pConn->GattService.ServiceChangedCharHandle = temp_database[4] + (temp_database[5] << 8);
      pConn->GattService.ServiceChangedCharDescHandle = temp_database[6] + (temp_database[7] << 8);

      LOG_INFO_APP("Restored GATT Service\n");
      LOG_INFO_APP("StartHandle=%04d\n", pConn->GattService.StartHandle);
      LOG_INFO_APP("EndHandle=%04d\n", pConn->GattService.EndHandle);
      LOG_INFO_APP("ServiceChangedCharHandle=%04d\n", pConn->GattService.ServiceChangedCharHandle);
      LOG_INFO_APP("ServiceChangedCharDescHandle=%04d\n", pConn->GattService.ServiceChangedCharDescHandle);

      pConn->GattService.LinkupState = GATT_SERVICE_LINKUP_COMPLETE;
    }
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

void NVMCB_Store( const uint32_t* ptr, uint32_t size )
{
  UNUSED(ptr);
  UNUSED(size);

  /* Call SNVMA for storing - Without callback */
  SNVMA_Write (APP_BLE_NvmBuffer,
               BLE_NvmCallback);
}

/* USER CODE BEGIN FD_WRAP_FUNCTIONS */
void APP_NotifyToRun( void )
{
  UTIL_SEQ_SetTask(1 << CFG_TASK_AUDIO_ID, CFG_SEQ_PRIO_0);
  /* Requires to process Host Stack because in Audio Task, some API/HCI commands
   * could be called and so require to run Host Stack
   */
  BleStackCB_Process();
}
/* USER CODE END FD_WRAP_FUNCTIONS */
