/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_ble.c
  * @author  MCD Application Team
  * @brief   BLE Application
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
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "gmap_app.h"
#include "stm32wba65i_discovery.h"

#if (CFG_LCD_SUPPORTED == 1)
#include "stm32wba65i_discovery_lcd.h"
#include "stm32_lcd.h"
#include "app_menu_cfg.h"
#endif /* CFG_LCD_SUPPORTED */

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

  /* USER CODE BEGIN PTD_1 */

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
#define ADV_TIMEOUT_MS                 (60 * 1000)

#if ((CFG_BLE_OPTIONS & BLE_OPTIONS_ENHANCED_ATT) == BLE_OPTIONS_ENHANCED_ATT)

/*MTU Maximum Transmission Unit for EATT*/
#define CFG_BLE_EATT_MTU_MAX            (CFG_BLE_COC_MPS_MAX - 2u) /* Maximum supported by LE Host*/

/* Number of K-frames that can be received on the created channel(s) by the L2CAP layer entity sending this packet.
 * cf 'Initial_Credits' parameter of the aci_l2cap_coc_connect() and aci_l2cap_coc_connect_confirm() functions
 */
#define INITIAL_RX_CREDITS              (1u)

/*Number of channels to be created thanks to aci_l2cap_coc_connect() call (Maximum value is 5)*/
#define MAX_NUM_CLIENT_EATT_BEARERS     MIN(CFG_BLE_EATT_BEARER_PER_LINK,5u)

#endif /* ((CFG_BLE_OPTIONS & BLE_OPTIONS_ENHANCED_ATT) == BLE_OPTIONS_ENHANCED_ATT)*/

#define GATT_EAB_PREFIX                 0xEAU

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

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/

/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private function prototypes -----------------------------------------------*/
static uint8_t HOST_BLE_Init(void);
static void BLE_ResumeFlowProcessCallback(void);
static void BLE_NvmCallback(SNVMA_Callback_Status_t CbkStatus);
static void Ble_Hci_Gap_Gatt_Init(void);
static const uint8_t* BleGenerateBdAddress(void);
static const uint8_t* BleGenerateIRValue(void);
static const uint8_t* BleGenerateERValue(void);
/* USER CODE BEGIN PFP */
static tBleStatus APP_COCConnect(uint16_t ConnHandle);
static char Hex_To_Char(uint8_t Hex);

/* USER CODE END PFP */

/* External functions prototypes ---------------------------------------------*/

/* USER CODE BEGIN EFP */
extern void BLE_CodecReset(void);
extern void BLE_CodecEvent(const uint8_t* buffer);

/* USER CODE END EFP */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */
extern GMAPAPP_Context_t GMAPAPP_Context;

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

  }
  /* USER CODE BEGIN APP_BLE_Init_2 */

  /* Initialize codec module */
  BLE_CodecReset();

  /* Initialize the Audio Stack */
  APP_AUDIO_STACK_Init();

  GMAPAPP_Init();

#if (CFG_LCD_SUPPORTED == 1)
  Menu_Config();
#endif /* (CFG_LCD_SUPPORTED == 1) */

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
      GMAPAPP_LinkDisconnected(p_disconnection_complete_event->Connection_Handle,
                               p_disconnection_complete_event->Reason);

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
          hci_le_connection_update_complete_event_rp0 *p_conn_update_complete;
          p_conn_update_complete = (hci_le_connection_update_complete_event_rp0 *) p_meta_evt->data;
          UNUSED(p_conn_update_complete);

          /* USER CODE BEGIN EVT_LE_CONN_UPDATE_COMPLETE */
          uint32_t conn_interval_us = 0;
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
          LOG_INFO_APP(">>== HCI_LE_PHY_UPDATE_COMPLETE_SUBEVT_CODE - ACL Connection handle: 0x%04X  - Status 0x%02X\n",
                       p_le_phy_update_complete->Connection_Handle,
                       p_le_phy_update_complete->Status);
          LOG_INFO_APP(">>== RX_PHY : 0x%02X  - TX_PHY : 0x%02X\n",
                       p_le_phy_update_complete->RX_PHY,
                       p_le_phy_update_complete->TX_PHY);

          /* USER CODE END EVT_LE_PHY_UPDATE_COMPLETE */
          break;
        }
        case HCI_LE_ENHANCED_CONNECTION_COMPLETE_SUBEVT_CODE:
        {
          hci_le_enhanced_connection_complete_event_rp0 *p_enhanced_conn_complete;
          p_enhanced_conn_complete = (hci_le_enhanced_connection_complete_event_rp0 *) p_meta_evt->data;
          UNUSED(p_enhanced_conn_complete);
          /* USER CODE BEGIN HCI_EVT_LE_ENHANCED_CONN_COMPLETE */
          uint32_t conn_interval_us = 0;
          uint8_t type, a_address[6];
          conn_interval_us = p_enhanced_conn_complete->Conn_Interval * 1250;
          LOG_INFO_APP(">>== HCI_LE_ENHANCED_CONNECTION_COMPLETE_SUBEVT_CODE - Connection handle: 0x%04X\n",
                       p_enhanced_conn_complete->Connection_Handle);
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
          UNUSED(conn_interval_us);
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
            uint8_t status;
            GMAPAPP_AclConnected(p_enhanced_conn_complete->Connection_Handle,
                                p_enhanced_conn_complete->Peer_Address_Type,
                                p_enhanced_conn_complete->Peer_Address,
                                p_enhanced_conn_complete->Role);

            status = APP_COCConnect(p_enhanced_conn_complete->Connection_Handle);
            if (status != BLE_STATUS_SUCCESS)
            {
              status = aci_gap_send_pairing_req(p_enhanced_conn_complete->Connection_Handle, 0x00);
              if (status != BLE_STATUS_SUCCESS)
              {
                LOG_INFO_APP("==>> aci_gap_send_pairing_req : Fail, reason: 0x%02X\n", status);
              }
              else
              {
                LOG_INFO_APP("==>> aci_gap_send_pairing_req : Success\n");
              }
            }
          }

#if (CFG_LCD_SUPPORTED == 1)
          Menu_SetConnectingPage(p_enhanced_conn_complete->Connection_Handle);
#endif /* (CFG_LCD_SUPPORTED == 1) */

          /* USER CODE END HCI_EVT_LE_ENHANCED_CONN_COMPLETE */
          break; /* HCI_LE_ENHANCED_CONNECTION_COMPLETE_SUBEVT_CODE */
        }
        case HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE:
        {
          hci_le_connection_complete_event_rp0 *p_conn_complete;
          p_conn_complete = (hci_le_connection_complete_event_rp0 *) p_meta_evt->data;
          UNUSED(p_conn_complete);
          /* USER CODE BEGIN HCI_EVT_LE_CONN_COMPLETE */
          uint32_t conn_interval_us = 0;
          uint8_t type, a_address[6];
          conn_interval_us = p_conn_complete->Conn_Interval * 1250;
          LOG_INFO_APP(">>== HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE - Connection handle: 0x%04X\n",
                       p_conn_complete->Connection_Handle);
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
          UNUSED(conn_interval_us);
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
            uint8_t status;
            GMAPAPP_AclConnected(p_conn_complete->Connection_Handle,
                                p_conn_complete->Peer_Address_Type,
                                p_conn_complete->Peer_Address,
                                p_conn_complete->Role);

            status = APP_COCConnect(p_conn_complete->Connection_Handle);
            if (status != BLE_STATUS_SUCCESS)
            {
              status = aci_gap_send_pairing_req(p_conn_complete->Connection_Handle, 0x00);
              if (status != BLE_STATUS_SUCCESS)
              {
                LOG_INFO_APP("==>> aci_gap_send_pairing_req : Fail, reason: 0x%02X\n", status);
              }
              else
              {
                LOG_INFO_APP("==>> aci_gap_send_pairing_req : Success\n");
              }
            }
          }

#if (CFG_LCD_SUPPORTED == 1)
          Menu_SetConnectingPage(p_conn_complete->Connection_Handle);
#endif /* (CFG_LCD_SUPPORTED == 1) */

          /* USER CODE END HCI_EVT_LE_CONN_COMPLETE */
          break; /* HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE */
        }
        /* USER CODE BEGIN SUBEVENT */
        case HCI_LE_CIS_ESTABLISHED_SUBEVT_CODE:
        {
            hci_le_cis_established_event_rp0 *p_cis_established_event;
            p_cis_established_event = (hci_le_cis_established_event_rp0 *) p_meta_evt->data;
            LOG_INFO_APP(">>== HCI_LE_CIS_ESTABLISHED_SUBEVT_CODE - CIS Connection handle: 0x%04X  - Status 0x%02X\n",
                         p_cis_established_event->Connection_Handle,
                         p_cis_established_event->Status);
            if (p_cis_established_event->Status == BLE_STATUS_SUCCESS)
            {
              GMAPAPP_CISConnected(p_cis_established_event->Connection_Handle);
              LOG_INFO_APP("CIG_Sync_Delay: %d us, CIS_Sync_Delay: %d us\n",
                         p_cis_established_event->CIG_Sync_Delay[0]  \
                         + (p_cis_established_event->CIG_Sync_Delay[1]<<8) \
                         + (p_cis_established_event->CIG_Sync_Delay[2]<<16),
                         p_cis_established_event->CIS_Sync_Delay[0]  \
                         + (p_cis_established_event->CIS_Sync_Delay[1]<<8) \
                         + (p_cis_established_event->CIS_Sync_Delay[2]<<16));
              LOG_INFO_APP("Transport_Latency_C_To_P: %d us, Transport_Latency_P_To_C: %d us\n",
                         p_cis_established_event->Transport_Latency_C_To_P[0]  \
                         + (p_cis_established_event->Transport_Latency_C_To_P[1]<<8) \
                         + (p_cis_established_event->Transport_Latency_C_To_P[2]<<16),
                         p_cis_established_event->Transport_Latency_P_To_C[0]  \
                         + (p_cis_established_event->Transport_Latency_P_To_C[1]<<8) \
                         + (p_cis_established_event->Transport_Latency_P_To_C[2]<<16));
              LOG_INFO_APP("PHY_C_To_P: 0x%02X, PHY_P_To_C: 0x%02X\n",
                         p_cis_established_event->PHY_C_To_P,
                         p_cis_established_event->PHY_P_To_C);
              LOG_INFO_APP("BN_C_To_P: 0x%02X, BN_P_To_C: 0x%02X\n",
                         p_cis_established_event->BN_C_To_P,
                         p_cis_established_event->BN_P_To_C);
              LOG_INFO_APP("FT_C_To_P: 0x%02X, FT_P_To_C: 0x%02X\n",
                         p_cis_established_event->FT_C_To_P,
                         p_cis_established_event->FT_P_To_C);
              LOG_INFO_APP("Max_PDU_C_To_P: 0x%04X, Max_PDU_P_To_C: 0x%04X\n",
                         p_cis_established_event->Max_PDU_C_To_P,
                         p_cis_established_event->Max_PDU_P_To_C);
              LOG_INFO_APP("NSE: 0x%02X, ISO_Interval: 0x%04X\n",
                         p_cis_established_event->NSE,
                         p_cis_established_event->ISO_Interval);
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
              GMAPAPP_CISConnected(p_cis_established_event->Connection_Handle);
              LOG_INFO_APP("CIG_Sync_Delay: %d us, CIS_Sync_Delay: %d us\n",
                         p_cis_established_event->CIG_Sync_Delay[0]  \
                         + (p_cis_established_event->CIG_Sync_Delay[1]<<8) \
                         + (p_cis_established_event->CIG_Sync_Delay[2]<<16),
                         p_cis_established_event->CIS_Sync_Delay[0]  \
                         + (p_cis_established_event->CIS_Sync_Delay[1]<<8) \
                         + (p_cis_established_event->CIS_Sync_Delay[2]<<16));
              LOG_INFO_APP("Transport_Latency_C_To_P: %d us, Transport_Latency_P_To_C: %d us\n",
                         p_cis_established_event->Transport_Latency_C_To_P[0]  \
                         + (p_cis_established_event->Transport_Latency_C_To_P[1]<<8) \
                         + (p_cis_established_event->Transport_Latency_C_To_P[2]<<16),
                         p_cis_established_event->Transport_Latency_P_To_C[0]  \
                         + (p_cis_established_event->Transport_Latency_P_To_C[1]<<8) \
                         + (p_cis_established_event->Transport_Latency_P_To_C[2]<<16));
              LOG_INFO_APP("PHY_C_To_P: 0x%02X, PHY_P_To_C: 0x%02X\n",
                         p_cis_established_event->PHY_C_To_P,
                         p_cis_established_event->PHY_P_To_C);
              LOG_INFO_APP("BN_C_To_P: 0x%02X, BN_P_To_C: 0x%02X\n",
                         p_cis_established_event->BN_C_To_P,
                         p_cis_established_event->BN_P_To_C);
              LOG_INFO_APP("FT_C_To_P: 0x%02X, FT_P_To_C: 0x%02X\n",
                         p_cis_established_event->FT_C_To_P,
                         p_cis_established_event->FT_P_To_C);
              LOG_INFO_APP("Max_PDU_C_To_P: 0x%04X, Max_PDU_P_To_C: 0x%04X\n",
                         p_cis_established_event->Max_PDU_C_To_P,
                         p_cis_established_event->Max_PDU_P_To_C);
              LOG_INFO_APP("NSE: 0x%02X, ISO_Interval: 0x%04X\n",
                         p_cis_established_event->NSE,
                         p_cis_established_event->ISO_Interval);
            }
        }
        break;

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

        case HCI_LE_EXTENDED_ADVERTISING_REPORT_SUBEVT_CODE:
        {
          uint8_t i;
          uint8_t audio_device = 0;
          uint8_t* name_ptr = 0;
          uint8_t name_len = 0;

          hci_le_extended_advertising_report_event_rp0 *report = (hci_le_extended_advertising_report_event_rp0*) p_meta_evt->data;

          for (i = 0; i < report->Num_Reports; i++)
          {
            uint16_t parse_index = 0;
            while (parse_index + 3 < report->Data_Length)
            {
              if (report->Data[parse_index + 1] == AD_TYPE_SERVICE_DATA
                  && report->Data[parse_index + 2] == 0x4E
                    && report->Data[parse_index + 3] == 0x18)
              {
                audio_device = 1u;
              }
              if ((report->Data[parse_index + 1] == AD_TYPE_16_BIT_SERV_UUID_CMPLT_LIST)\
                 || (report->Data[parse_index + 1] == AD_TYPE_16_BIT_SERV_UUID))
              {
                  for (uint16_t j = 0; j< (report->Data[parse_index] - 1);j++)
                  {
                    if ((report->Data[parse_index + 2 + j ] == 0x4E)
                      && (report->Data[parse_index + 3 + j] == 0x18))
                    {
                      audio_device = 1u;
                    }
                    j+=1;
                  }
              }
              if (report->Data[parse_index + 1] == AD_TYPE_COMPLETE_LOCAL_NAME)
              {
                name_ptr = &report->Data[parse_index + 2];
                name_len = report->Data[parse_index] - 1;
              }
              parse_index += report->Data[parse_index] + 1;
            }
          }

          if (audio_device == 1u)
          {
            char name[30] = "Unknown";
            if (name_ptr != 0)
            {
              UTIL_MEM_cpy_8(&name[0], name_ptr, name_len);
              name[name_len] = '\0';
            }

#if (CFG_TEST_VALIDATION == 1u)
            if (memcmp(&name[0], "GMAP_WBA", 8u) != 0)
              return SVCCTL_UserEvtFlowEnable;
#endif /*(CFG_TEST_VALIDATION == 1u)*/

            LOG_INFO_APP("%s: %02x:%02x:%02x:%02x:%02x:%02x\n",
                         name,
                         report->Address[5],
                         report->Address[4],
                         report->Address[3],
                         report->Address[2],
                         report->Address[1],
                         report->Address[0]);
#if (CFG_LCD_SUPPORTED == 1)
            Menu_AddUnicastServer(&report->Address[0], report->Address_Type, &name[0]);
#endif /* (CFG_LCD_SUPPORTED == 1) */
          }
        }
        break;

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

          /*Set PHY preferences for the connection*/
          hci_le_set_phy(p_pairing_complete->Connection_Handle, CFG_PHY_PREF, CFG_PHY_PREF_TX, CFG_PHY_PREF_RX, 0);

          if (p_pairing_complete->Status == BLE_STATUS_SUCCESS)
          {
            LOG_INFO_APP("Pairing Complete with connection handle 0x%04X\n", p_pairing_complete->Connection_Handle);

#if (CFG_LCD_SUPPORTED == 1)
            Menu_SetLinkupPage(p_pairing_complete->Connection_Handle);
#endif /* (CFG_LCD_SUPPORTED == 1) */
            (void)GMAPAPP_Linkup(p_pairing_complete->Connection_Handle);
          }
          else if (p_pairing_complete->Reason == HCI_PIN_OR_KEY_MISSING_ERR_CODE)
          {
            uint8_t status;
            LOG_INFO_APP("Pairing Failed with connection handle 0x%04X because of Key Mismatch : Force Bonding\n",
                         p_pairing_complete->Connection_Handle);
            GMAPAPP_BondLost(p_pairing_complete->Connection_Handle);
            status = aci_gap_send_pairing_req(p_pairing_complete->Connection_Handle, 1);
            if (status != BLE_STATUS_SUCCESS)
            {
              LOG_INFO_APP("==>> aci_gap_send_pairing_req : Fail, reason: 0x%02X\n", status);
            }
            else
            {
              LOG_INFO_APP("==>> aci_gap_send_pairing_req : Success\n");
            }
          }
          else
          {
            LOG_INFO_APP("Pairing Failed with connection handle 0x%04X\n", p_pairing_complete->Connection_Handle);
            GMAPAPP_Disconnect();
#if (CFG_LCD_SUPPORTED == 1)
            Menu_SetScanningPage();
#endif /* (CFG_LCD_SUPPORTED == 1) */
          }
          break;
        }
        case ACI_GAP_BOND_LOST_VSEVT_CODE:
        {
          LOG_INFO_APP(">>== ACI_GAP_BOND_LOST_EVENT\n");
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
        case ACI_GATT_INDICATION_VSEVT_CODE:
        {
          aci_gatt_indication_event_rp0 *p_indication;
          tBleStatus ret;
          p_indication = (aci_gatt_indication_event_rp0*)p_blecore_evt->data;
          LOG_INFO_APP(">>== ACI_GATT_INDICATION_VSEVT_CODE\n");
          LOG_INFO_APP(">>== Connection_Handle : 0x%04X\n", p_indication->Connection_Handle);
          LOG_INFO_APP(">>== Attribute_Handle : 0x%04X\n", p_indication->Attribute_Handle);
          LOG_INFO_APP(">>== Attribute_Value_Length : 0x%02X\n", p_indication->Attribute_Value_Length);
          LOG_INFO_APP(">>== ACI_GATT_INDICATION_VSEVT_CODE\n");

          ret = aci_gatt_confirm_indication(p_indication->Connection_Handle);
          if (ret != BLE_STATUS_SUCCESS)
          {
            /* Impossible to confirm indication due to GATT process ongoing. Notify gmap_app */
            GMAPAPP_ConfirmIndicationRequired(p_indication->Connection_Handle);
            LOG_INFO_APP("  Fail   : aci_gatt_confirm_indication command, result: 0x%02X\n", ret);
          }
          else
          {
            LOG_INFO_APP("  Success: aci_gatt_confirm_indication command\n");
          }
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

#if ((CFG_BLE_OPTIONS & BLE_OPTIONS_ENHANCED_ATT) == BLE_OPTIONS_ENHANCED_ATT)
        case ACI_L2CAP_COC_CONNECT_VSEVT_CODE:
        {
          aci_l2cap_coc_connect_event_rp0 *p_coc_connect;
          tBleStatus ret;
          uint16_t Result = 0x0001;
          uint8_t Channel_Number;
          uint8_t Channel_Index_List[5u];
          p_coc_connect = (aci_l2cap_coc_connect_event_rp0*)p_blecore_evt->data;
          uint8_t Max_Channel_Number = p_coc_connect->Channel_Number;
          LOG_INFO_APP(">>== ACI_L2CAP_COC_CONNECT_VSEVT_CODE\n");
          LOG_INFO_APP(">>==  Credit Based Connection Request :\n");
          LOG_INFO_APP("         Connection_Handle : 0x%04X\n", p_coc_connect->Connection_Handle);
          LOG_INFO_APP("         SPSM : 0x%04X\n", p_coc_connect->SPSM);
          LOG_INFO_APP("         MTU : %d\n", p_coc_connect->MTU);
          LOG_INFO_APP("         MPS : %d\n", p_coc_connect->MPS);
          LOG_INFO_APP("         Initial_Credits : %d\n", p_coc_connect->Initial_Credits);
          LOG_INFO_APP("         Channel_Number : %d\n", p_coc_connect->Channel_Number);
          if (p_coc_connect->SPSM == 0x0027)
          {
            Result = 0x0000u;
          }
          if (Max_Channel_Number > (CFG_BLE_EATT_BEARER_PER_LINK - MAX_NUM_CLIENT_EATT_BEARERS))
          {
            Max_Channel_Number = (CFG_BLE_EATT_BEARER_PER_LINK - MAX_NUM_CLIENT_EATT_BEARERS);
          }
          if (Max_Channel_Number == 0)
          {
            Result = 0x00004u; /*Some connections refused - insufficient resources available*/
          }
          ret = aci_l2cap_coc_connect_confirm(p_coc_connect->Connection_Handle,
                                              CFG_BLE_EATT_MTU_MAX,
                                              CFG_BLE_COC_MPS_MAX,
                                              INITIAL_RX_CREDITS,
                                              Result,
                                              Max_Channel_Number,
                                              &Channel_Number,
                                              Channel_Index_List);
          if (ret != BLE_STATUS_SUCCESS)
          {
            LOG_INFO_APP("==>> aci_l2cap_coc_connect_confirm() : Fail, reason: 0x%02X\n", ret);
          }
          else
          {
            LOG_INFO_APP("==>> aci_l2cap_coc_connect_confirm() : Success\n");
            if (Result == 0x000u)
            {
              LOG_INFO_APP("         Number of created channels issued from remote request : %d\n", p_coc_connect->Channel_Number);
              for (uint8_t i = 0u; i < Channel_Number ; i++)
              {
                LOG_INFO_APP("            Channel_Index : 0x%04X\n", Channel_Index_List[i]);
              }
            }
          }
          break;
        }

        case ACI_L2CAP_COC_CONNECT_CONFIRM_VSEVT_CODE:
        {
          aci_l2cap_coc_connect_confirm_event_rp0 *coc_connect_confirm_event;
          tBleStatus status;
          coc_connect_confirm_event = (aci_l2cap_coc_connect_confirm_event_rp0*)p_blecore_evt->data;

          LOG_INFO_APP(">>== ACI_L2CAP_COC_CONNECT_CONFIRM_VSEVT_CODE\n");
          LOG_INFO_APP(">>==  Credit Based Connection Response :\n");
          LOG_INFO_APP("         Connection_Handle : 0x%04X\n", coc_connect_confirm_event->Connection_Handle);
          LOG_INFO_APP("         Result : 0x%04X\n", coc_connect_confirm_event->Result);
          LOG_INFO_APP("         MTU : %d\n", coc_connect_confirm_event->MTU);
          LOG_INFO_APP("         MPS : %d\n", coc_connect_confirm_event->MPS);
          LOG_INFO_APP("         Initial_Credits : %d\n", coc_connect_confirm_event->Initial_Credits);
          LOG_INFO_APP("         Number of created channels issued from local request : %d\n", coc_connect_confirm_event->Channel_Number);
          for (uint8_t i = 0u; i < coc_connect_confirm_event->Channel_Number ; i++)
          {
            LOG_INFO_APP(">>== Channel_Index : 0x%04X\n", coc_connect_confirm_event->Channel_Index_List[i]);
          }
          if (coc_connect_confirm_event->Result == 0x0000u)
          {
            LOG_INFO_APP("==>> Connection Oriented Channel established with success\n");
          }
          else
          {
            LOG_INFO_APP("==>> Connection Oriented Channel failed for reason 0x%02X\n");
          }

          status = aci_gap_send_pairing_req(coc_connect_confirm_event->Connection_Handle, 0x00);
          if (status != BLE_STATUS_SUCCESS)
          {
            LOG_INFO_APP("==>> aci_gap_send_pairing_req : Fail, reason: 0x%02X\n", status);
          }
          else
          {
            LOG_INFO_APP("==>> aci_gap_send_pairing_req : Success\n");
          }
          UNUSED(status);
          break;
        }

        case ACI_L2CAP_COC_DISCONNECT_VSEVT_CODE:
        {
#if (CFG_LOG_SUPPORTED != 0)
          aci_l2cap_coc_disconnect_event_rp0 *coc_disconnect_event;
          coc_disconnect_event = (aci_l2cap_coc_disconnect_event_rp0*)p_blecore_evt->data;
          LOG_INFO_APP(">>== ACI_L2CAP_COC_DISCONNECT_VSEVT_CODE\n");
          LOG_INFO_APP(">>==  Credit Based Connection Disconnected :\n");
          LOG_INFO_APP("         COC ConnHandle = 0x%04X\n",(GATT_EAB_PREFIX << 8) | coc_disconnect_event->Channel_Index);
          LOG_INFO_APP("         Channel_Index = 0x%04X\n",coc_disconnect_event->Channel_Index );
          break;
#endif /*(CFG_LOG_SUPPORTED != 0)*/
        }

        case ACI_L2CAP_COMMAND_REJECT_VSEVT_CODE:
        {
#if (CFG_LOG_SUPPORTED != 0)
          aci_l2cap_command_reject_event_rp0 *aci_l2cap_command_reject_event;
          aci_l2cap_command_reject_event = (aci_l2cap_command_reject_event_rp0*)p_blecore_evt->data;
          LOG_INFO_APP(">>== ACI_L2CAP_COMMAND_REJECT_VSEVT_CODE\n");
          LOG_INFO_APP("         Reason = 0x%04X\n",aci_l2cap_command_reject_event->Reason);
          LOG_INFO_APP("         Connection Handle = 0x%04X\n",aci_l2cap_command_reject_event->Connection_Handle);
          LOG_INFO_APP("         Identifier = %d\n",aci_l2cap_command_reject_event->Identifier);
          break;
#endif /*(CFG_LOG_SUPPORTED != 0)*/
        }

        case ACI_GATT_EATT_BEARER_VSEVT_CODE:
        {
          aci_gatt_eatt_bearer_event_rp0 *eatt_bearer_event;
          eatt_bearer_event = (aci_gatt_eatt_bearer_event_rp0*)p_blecore_evt->data;
          LOG_INFO_APP(">>== ACI_GATT_EATT_BEARER_VSEVT_CODE\n");
          LOG_INFO_APP("EATT Bearer associated to :\n");
          LOG_INFO_APP("        Channel_Handle = 0x%04X\n",eatt_bearer_event->Connection_Handle );
          LOG_INFO_APP("        Channel_Index = 0x%02X\n",eatt_bearer_event->Channel_Index );
          LOG_INFO_APP("        State = 0x%02X\n",eatt_bearer_event->EAB_State );
          LOG_INFO_APP("        MTU = %d\n",eatt_bearer_event->MTU );
          LOG_INFO_APP("        COC ConnHandle = 0x%04X\n",(GATT_EAB_PREFIX << 8) | eatt_bearer_event->Channel_Index);
          if (eatt_bearer_event->EAB_State == 0u)
          {
            uint8_t aChannel_Index[1];
            aChannel_Index[0] = eatt_bearer_event->Channel_Index;
            tBleStatus status = BLE_AUDIO_STACK_EATT_SubscribeBearers(aChannel_Index,1);
            LOG_INFO_APP("BLE_AUDIO_STACK_EATT_SubscribeBearers() for ChannelIdx 0x%02X returns status 0x%02X\n",
                         eatt_bearer_event->Channel_Index,
                         status);
          }
          break;
        }
#endif /*((CFG_BLE_OPTIONS & BLE_OPTIONS_ENHANCED_ATT) == BLE_OPTIONS_ENHANCED_ATT)*/

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
  const uint8_t *p_bd_addr;
  uint16_t a_appearance[1] = {CFG_GAP_APPEARANCE};
  const uint8_t *p_ir_value;
  const uint8_t *p_er_value;
  tBleStatus ret;

  /* USER CODE BEGIN Ble_Hci_Gap_Gatt_Init */
  uint8_t a_hci_commandParams[8] = {0xFFu,0xFFu,0x86u,0xBFu,0x03u,0x00u,0x00u,0x00u};

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
  role |= GAP_CENTRAL_ROLE | GAP_PERIPHERAL_ROLE;
  a_GapDeviceName[9] = Hex_To_Char((p_bd_addr[1] & 0xF0) >> 4);
  a_GapDeviceName[10] = Hex_To_Char(p_bd_addr[1] & 0x0F);
  a_GapDeviceName[11] = Hex_To_Char((p_bd_addr[0] & 0xF0) >> 4);
  a_GapDeviceName[12] = Hex_To_Char(p_bd_addr[0] & 0x0F);

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

static const uint8_t* BleGenerateBdAddress(void)
{
  OTP_Data_s *p_otp_addr = NULL;
  const uint8_t *p_bd_addr;
  uint32_t udn;
  uint32_t company_id;
  uint32_t device_id;
  uint8_t a_BdAddrDefault[BD_ADDR_SIZE] ={0x00, 0x00, 0x00, 0xE1, 0x80, 0x00};
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
        memcpy(&a_BdAddr[0], a_BdAddrDefault, BD_ADDR_SIZE);
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

static tBleStatus APP_COCConnect(uint16_t ConnHandle)
{
#if ((CFG_BLE_OPTIONS & BLE_OPTIONS_ENHANCED_ATT) == BLE_OPTIONS_ENHANCED_ATT)
  tBleStatus status;

  LOG_INFO_APP("==>> Start Connection Oriented Channel\n");
  LOG_INFO_APP("  SPSM : 0x%04X\n",0x0027);
  LOG_INFO_APP("  MTU : %d\n",CFG_BLE_EATT_MTU_MAX);
  LOG_INFO_APP("  MPS : %d\n",CFG_BLE_COC_MPS_MAX);
  LOG_INFO_APP("  Initial_Credits (Rx) : %d\n",INITIAL_RX_CREDITS);
  LOG_INFO_APP("  Channel_Number : %d\n",MAX_NUM_CLIENT_EATT_BEARERS);
  status = aci_l2cap_coc_connect(ConnHandle,
                                 0x0027,
                                 CFG_BLE_EATT_MTU_MAX,
                                 CFG_BLE_COC_MPS_MAX,
                                 INITIAL_RX_CREDITS,
                                 MAX_NUM_CLIENT_EATT_BEARERS);
  if (status != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_l2cap_coc_connect command, result: 0x%02X\n", status);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_l2cap_coc_connect command\n");
  }
  return status;
#else /*((CFG_BLE_OPTIONS & BLE_OPTIONS_ENHANCED_ATT) == 0)*/
  return HCI_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE_ERR_CODE;
#endif /*((CFG_BLE_OPTIONS & BLE_OPTIONS_ENHANCED_ATT) == BLE_OPTIONS_ENHANCED_ATT)*/
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
  /* Notify the event to the codec */
  BLE_CodecEvent(data);

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

void APP_NotifyToRun( void )
{
  UTIL_SEQ_SetTask(1 << CFG_TASK_AUDIO_ID, CFG_SEQ_PRIO_0);
  /* Requires to process Host Stack because in Audio Task, some API/HCI commands
   * could be called and so require to run Host Stack
   */
  BleStackCB_Process();
}

/* USER CODE END FD_WRAP_FUNCTIONS */
