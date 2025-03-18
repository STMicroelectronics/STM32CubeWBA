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
#include "zigbee.zd.h"
#include "zdd_stack_wrapper.h"
#include "bpka.h"

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

/* Global structures and defines used  to handle Zigbee Direct. */
/****************************************************************/

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

static uint8_t a_BdAddr[BD_ADDR_SIZE];
/* Identity root key used to derive IRK and DHK(Legacy) */
static uint8_t a_BLE_CfgIrValue[16];

/* Encryption root key used to derive LTK(Legacy) and CSRK */
static uint8_t a_BLE_CfgErValue[16];
static BleApplicationContext_t bleAppContext;
/* Identity root key used to derive IRK and DHK(Legacy) */
static const uint8_t zdd_BLE_CfgIrValue[16] = CFG_BLE_IR;

/* Encryption root key used to derive LTK(Legacy) and CSRK */
static const uint8_t zdd_BLE_CfgErValue[16] = CFG_BLE_ER;
static uint32_t zdd_ble_ram[DIVC(BLE_DYN_ALLOC_SIZE, 4)];
static uint32_t zdd_ble_gatt_ram[DIVC(BLE_GATT_BUF_SIZE, 4)];


static char a_GapDeviceName[] = {  'H', 'e', 'a', 'r', 't', ' ', 'R', 'a', 't', 'e', ' ', 'S', 'e', 'n', 's', 'o', 'r' }; /* Gap Device Name */

/* Advertising Data */
uint8_t a_AdvData[27] =
{
  6, AD_TYPE_COMPLETE_LOCAL_NAME, 'H', 'R', '_', 'X', 'X',  /* Complete name */
  3, AD_TYPE_16_BIT_SERV_UUID_CMPLT_LIST, 0x0D, 0x18,
  15, AD_TYPE_MANUFACTURER_SPECIFIC_DATA, 0x30, 0x00, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */,
};

static const BleStack_init_t zdd_ble_stack_init_params = {
    .numAttrRecord = CFG_BLE_NUM_GATT_ATTRIBUTES,
    .numAttrServ = CFG_BLE_NUM_GATT_SERVICES,
    .attrValueArrSize = CFG_BLE_ATT_VALUE_ARRAY_SIZE,
    .prWriteListSize = CFG_BLE_ATTR_PREPARE_WRITE_VALUE_SIZE,
    .attMtu = CFG_BLE_ATT_MTU_MAX,
    .max_coc_nbr = CFG_BLE_COC_NBR_MAX,
    .max_coc_mps = CFG_BLE_COC_MPS_MAX,
    .max_coc_initiator_nbr = CFG_BLE_COC_INITIATOR_NBR_MAX,
    .numOfLinks = CFG_BLE_NUM_LINK,
    .mblockCount = CFG_BLE_MBLOCK_COUNT,
    .bleStartRamAddress = (uint8_t *)zdd_ble_ram,
    .total_buffer_size = BLE_DYN_ALLOC_SIZE,
    .bleStartRamAddress_GATT = (uint8_t *)zdd_ble_gatt_ram,
    .total_buffer_size_GATT = BLE_GATT_BUF_SIZE,
    .options = CFG_BLE_OPTIONS,
    .debug = 0U
};
uint64_t buffer_nvm[CFG_BLEPLAT_NVM_MAX_SIZE] = {0};

/* USER CODE BEGIN PV */
uint8_t a_GATT_DevInfoData[22];
/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/

/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Global function prototypes -----------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/

static void Ble_Hci_Gap_Gatt_Init(void);
static const uint8_t* BleGenerateBdAddress(void);
static const uint8_t* BleGenerateIRValue(void);
static const uint8_t* BleGenerateERValue(void);
static void gap_cmd_resp_wait(void);
static void BleStackProcess(void);
static void BleUserEvtRx(void);
static void BLE_NvmCallback (SNVMA_Callback_Status_t);
static uint8_t HOST_BLE_Init(void);
static bool initialize_ble_stack(void);
/* USER CODE BEGIN PFP */
static void fill_advData(uint8_t *p_adv_data, uint8_t tab_size, const uint8_t*p_bd_addr);

#if (BLE_RADIO_ACTIVITY_ON_LED_SUPPORT != 0)
static void Switch_OFF_Led(void *arg);
#endif
/* USER CODE END PFP */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Functions Definition ------------------------------------------------------*/

tListNode* get_ble_asynch_event_queue(void); // TODO 

void APP_BLE_Init(void)
{
  
#if (BLE_RADIO_ACTIVITY_ON_LED_SUPPORT != 0)
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
#endif

  /* NVM emulation in RAM initialization */
   NVM_Init(buffer_nvm, 0, CFG_BLEPLAT_NVM_MAX_SIZE);

  /* First register the APP BLE buffer */
  SNVMA_Register (SNVMA_BufferId_1,
                  (uint32_t *)buffer_nvm,
                  (CFG_BLEPLAT_NVM_MAX_SIZE * 2));

  /* Realize a restore */
  SNVMA_Restore (SNVMA_BufferId_1);
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

    /* Initialize Servic+es and Characteristics. */
    LOG_INFO_APP("\n");
    LOG_INFO_APP("Services and Characteristics creation\n");
    LOG_INFO_APP("End of Services and Characteristics creation\n");
    LOG_INFO_APP("\n");

    /* USER CODE BEGIN APP_BLE_Init_3 */
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
    
  }
  /* USER CODE BEGIN APP_BLE_Init_2 */
  bleAppContext.connIntervalFlag = 0;
  /* USER CODE END APP_BLE_Init_2 */

  return;
}

/**
 * @brief  Event handler
 * @param  Event: Address of the buffer holding the Event
 * @retval Ack: Return whether the Event has been managed or not
 */


static SVCCTL_EvtAckStatus_t
zdd_svc_event_handler(void *Event)
{
    SVCCTL_EvtAckStatus_t return_value;
    hci_event_pckt *event_pckt;
    evt_blecore_aci *blecore_evt;

    return_value = SVCCTL_EvtNotAck;
    event_pckt = (hci_event_pckt *)(((hci_uart_pckt *)Event)->data);

    /* ZDD_DEBUG("evt = 0x%02x", event_pckt->evt); */

    switch (event_pckt->evt) {
        case HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE:
            blecore_evt = (evt_blecore_aci *)event_pckt->data;

            /* ZDD_DEBUG("vendor ecode = 0x%04x", blecore_evt->ecode); */

            switch (blecore_evt->ecode) {
                /*-------------------------------------------------------------
                 * Handle Read request from GATT Client
                 *-------------------------------------------------------------
                 */
                case ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE:
                {
                    aci_gatt_read_permit_req_event_rp0 *attribute_read;

                    attribute_read = (aci_gatt_read_permit_req_event_rp0 *)blecore_evt->data;


                    if (!check_zdd_connection_handle(attribute_read->Connection_Handle)) {
                        break;
                    }

                    if (handle_zdd_read_request(attribute_read->Attribute_Handle) == false) {
                    	LOG_INFO_APP("Dropping unknown Read request(handle = 0x%04x)", attribute_read->Attribute_Handle);
                        break;
                    }

                    aci_gatt_allow_read(attribute_read->Connection_Handle);
                    LOG_INFO_APP("Allow read (handle = 0x%04x)", attribute_read->Attribute_Handle);
                    break;
                }

                case ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE:
                {
                    aci_gatt_write_permit_req_event_rp0 *write_perm_req;

                    return_value = SVCCTL_EvtAckFlowEnable;

                    write_perm_req = (aci_gatt_write_permit_req_event_rp0 *)blecore_evt->data;

                    if (processWriteForZdd(write_perm_req) == false) {
                        /* Unknown Charateristic / Attribute. */
                        break;
                    }

                    /* Allowed to process this Write Request */
                    /* ZDD_DEBUG("Write allowed"); */
                    aci_gatt_write_resp(write_perm_req->Connection_Handle,
                        write_perm_req->Attribute_Handle,
                        0x00, /* write_status = 0 (no error))*/
                        0x00, /* err_code */
                        write_perm_req->Data_Length,
                        (uint8_t *)&write_perm_req->Data[0]);

                    UTIL_SEQ_SetTask(1u << CFG_TASK_ZIGBEE_ZDD, CFG_SEQ_PRIO_1);
                    break;
                }

                /*-------------------------------------------------------------
                 * Handle Write request or Notification enabling from GATT Client
                 *-------------------------------------------------------------
                 */
                case ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE:
                {
                    aci_gatt_attribute_modified_event_rp0 *attribute_modified;
                    
                    attribute_modified = (aci_gatt_attribute_modified_event_rp0 *)blecore_evt->data;

                    if (!check_zdd_connection_handle(attribute_modified->Connection_Handle)) {
                         break;
                    }
                    return_value = handle_zdd_aci_gatt_atribute_modif(attribute_modified,return_value);
                    break;
                }

                case ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE:
                {
                    aci_att_exchange_mtu_resp_event_rp0 *p_exchange_mtu;

                    p_exchange_mtu = (aci_att_exchange_mtu_resp_event_rp0 *)blecore_evt->data;

                    LOG_INFO_APP("p_exchange_mtu = %d", p_exchange_mtu->Server_RX_MTU);

                    /* Let the BLE stack handle it. */
                    /* return_value = SVCCTL_EvtAckFlowEnable; */
                    break;
                }

                case ACI_GATT_SERVER_CONFIRMATION_VSEVT_CODE:

                	if (check_zdd_write_active() == true) {
                		aci_gatt_server_confirmation_event_rp0 *conf_event;

                		conf_event = (aci_gatt_server_confirmation_event_rp0 *)blecore_evt->data;
                		(void)conf_event;
                		update_zdd_write_active();
                	}
                	else {
                		LOG_INFO_APP("ACI_GATT_SERVER_CONFIRMATION_VSEVT_CODE (no blocking write active)");
                	}
                	return_value = SVCCTL_EvtAckFlowEnable;
                	break;

                default:
                	LOG_INFO_APP("unhandled vendor ecode");
                          handle_unhandled_event(__LINE__);
                    break;

            }
            break;

        default:
            LOG_INFO_APP("unhandled evt");
            handle_unhandled_event(__LINE__);
            break;
    }

    /* ZDD_DEBUG("Done (return_value = %d)", return_value); */
    return return_value;
}


SVCCTL_UserEvtFlowStatus_t SVCCTL_App_Notification(void *p_Pckt)
{
    tBleStatus ret = BLE_STATUS_ERROR;
    hci_event_pckt *p_event_pckt;
    evt_le_meta_event *p_meta_evt;
    evt_blecore_aci *p_blecore_evt;

    p_event_pckt = (hci_event_pckt *)((hci_uart_pckt *)p_Pckt)->data;
    UNUSED(ret);

    switch (p_event_pckt->evt) {
        case HCI_DISCONNECTION_COMPLETE_EVT_CODE:
        {
            hci_disconnection_complete_event_rp0 *p_disconnection_complete_event;
            p_disconnection_complete_event = (hci_disconnection_complete_event_rp0 *)p_event_pckt->data;
            LOG_INFO_APP(">>== HCI_DISCONNECTION_COMPLETE_EVT_CODE");
            LOG_INFO_APP("     - Connection Handle:   0x%02X", p_disconnection_complete_event->Connection_Handle);
            LOG_INFO_APP("     - Reason:    0x%02X", p_disconnection_complete_event->Reason);

            if (!check_zdd_connection_handle(p_disconnection_complete_event->Connection_Handle)) {
                LOG_INFO_APP("Ignoring, handle mismatch");
                break;
            }
            if (check_zdd_write_active()) {
            	LOG_INFO_APP("Warning, Characteristic Write is still active");
            }
            disconnect_zdd_port_zvd();
            break;
        }

        case HCI_LE_META_EVT_CODE:
        {
            p_meta_evt = (evt_le_meta_event *)p_event_pckt->data;

            switch (p_meta_evt->subevent) {
                case HCI_LE_CONNECTION_UPDATE_COMPLETE_SUBEVT_CODE:
                {
                    uint16_t conn_interval_us = 0;
                    hci_le_connection_update_complete_event_rp0 *p_conn_update_complete;
                    p_conn_update_complete = (hci_le_connection_update_complete_event_rp0 *)p_meta_evt->data;
                    conn_interval_us = p_conn_update_complete->Conn_Interval * 1250;
                    LOG_INFO_APP(">>== HCI_LE_CONNECTION_UPDATE_COMPLETE_SUBEVT_CODE");
                    LOG_INFO_APP("     - Connection Interval:   %d.%02d ms     "
                        "- Connection latency:    %d",
                        conn_interval_us / 1000,
                        (conn_interval_us % 1000) / 10,
                        p_conn_update_complete->Conn_Latency);
                    LOG_INFO_APP("     - Supervision Timeout:   %d ms",
                        p_conn_update_complete->Supervision_Timeout * 10);
                    UNUSED(p_conn_update_complete);
                    break;
                }

                case HCI_LE_PHY_UPDATE_COMPLETE_SUBEVT_CODE:
                {
                    break;
                }

                case HCI_LE_ENHANCED_CONNECTION_COMPLETE_SUBEVT_CODE:
                {
                    uint16_t conn_interval_us = 0;
                    hci_le_enhanced_connection_complete_event_rp0 *p_enhanced_conn_complete;
                    p_enhanced_conn_complete = (hci_le_enhanced_connection_complete_event_rp0 *)p_meta_evt->data;
                    conn_interval_us = p_enhanced_conn_complete->Conn_Interval * 1250;

                    LOG_INFO_APP("");
                    LOG_INFO_APP(">>== HCI_LE_ENHANCED_CONNECTION_COMPLETE_SUBEVT_CODE");
                    LOG_INFO_APP("     - Connection handle: 0x%04X", p_enhanced_conn_complete->Connection_Handle);
                    LOG_INFO_APP("     - Connection established with %02x:%02x:%02x:%02x:%02x:%02x",
                        p_enhanced_conn_complete->Peer_Address[5],
                        p_enhanced_conn_complete->Peer_Address[4],
                        p_enhanced_conn_complete->Peer_Address[3],
                        p_enhanced_conn_complete->Peer_Address[2],
                        p_enhanced_conn_complete->Peer_Address[1],
                        p_enhanced_conn_complete->Peer_Address[0]);
                    LOG_INFO_APP("     - Connection Interval:   %d.%02d ms", conn_interval_us / 1000, (conn_interval_us % 1000) / 10);
                    LOG_INFO_APP("     - Connection latency:    %d", p_enhanced_conn_complete->Conn_Latency);
                    LOG_INFO_APP("     - Supervision Timeout:   %d ms", p_enhanced_conn_complete->Supervision_Timeout * 10);
                    LOG_INFO_APP("");

                    if (!handle_new_zdd_connection()) {
                        LOG_INFO_APP("Error, cannot accept new connection.");
                        hci_disconnect(p_enhanced_conn_complete->Connection_Handle, 0x13);
                        break;
                    }

                    update_zdd_port_handle(p_enhanced_conn_complete->Connection_Handle);
                    break;
                }

                case HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE:
                {
                    uint16_t conn_interval_us = 0;
                    uint8_t result;
                    
                    hci_le_connection_complete_event_rp0 *p_conn_complete;
                    p_conn_complete = (hci_le_connection_complete_event_rp0 *)p_meta_evt->data;
                    conn_interval_us = p_conn_complete->Conn_Interval * 1250;
                    LOG_INFO_APP(">>== HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE - Connection handle: 0x%04X",
                        p_conn_complete->Connection_Handle);
                    LOG_INFO_APP("     - Connection established with %02x:%02x:%02x:%02x:%02x:%02x",
                        p_conn_complete->Peer_Address[5],
                        p_conn_complete->Peer_Address[4],
                        p_conn_complete->Peer_Address[3],
                        p_conn_complete->Peer_Address[2],
                        p_conn_complete->Peer_Address[1],
                        p_conn_complete->Peer_Address[0]);

                    LOG_INFO_APP("     - Connection Interval:   %d.%02d ms", conn_interval_us / 1000, (conn_interval_us % 1000) / 10);
                    LOG_INFO_APP("     - Connection latency:    %d", p_conn_complete->Conn_Latency);
                    LOG_INFO_APP("     - Supervision Timeout:   %d ms", p_conn_complete->Supervision_Timeout * 10);

                    if (!handle_new_zdd_connection()) {
                        LOG_INFO_APP("Error, cannot accept new connection.");
                        hci_disconnect(p_conn_complete->Connection_Handle, 0x13);
                        break;
                    }
                    update_zdd_port_handle(p_conn_complete->Connection_Handle);

                    /* initiate a connection update */
                    result = aci_l2cap_connection_parameter_update_req(p_conn_complete->Connection_Handle,
                    p_conn_complete->Conn_Interval, p_conn_complete->Conn_Interval, 0x00, 0x64);
                    if (result != 0) {
                    	LOG_INFO_APP("Error, aci_l2cap_connection_parameter_update_req fail (status = 0x%x)", result);
                    }
                    break;
                }

                default:
                    LOG_INFO_APP("Error, unhandled HCI_LE_META_EVT_CODE subevt = 0x%04x", p_meta_evt->subevent);
                    handle_unhandled_event(__LINE__);
                    break;
            }
        }
        break;

        case HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE:
        {
            p_blecore_evt = (evt_blecore_aci *)p_event_pckt->data;
            switch (p_blecore_evt->ecode) {
                case ACI_L2CAP_CONNECTION_UPDATE_RESP_VSEVT_CODE:
                {
                    aci_l2cap_connection_update_resp_event_rp0 *p_l2cap_conn_update_resp;
                    p_l2cap_conn_update_resp = (aci_l2cap_connection_update_resp_event_rp0 *)p_blecore_evt->data;
                    UNUSED(p_l2cap_conn_update_resp);
                    break;
                }

                case ACI_GAP_PROC_COMPLETE_VSEVT_CODE:
                {
                    aci_gap_proc_complete_event_rp0 *p_gap_proc_complete;
                    p_gap_proc_complete = (aci_gap_proc_complete_event_rp0 *)p_blecore_evt->data;
                    UNUSED(p_gap_proc_complete);

                    LOG_INFO_APP(">>== ACI_GAP_PROC_COMPLETE_VSEVT_CODE");
                    break;
                }

                case ACI_HAL_END_OF_RADIO_ACTIVITY_VSEVT_CODE:
                {
#if (BLE_RADIO_ACTIVITY_ON_LED_SUPPORT != 0)
                //PATCH TODO    APP_LED_ON(LED_GREEN);
                //PATCH TODO    UTIL_TIMER_StartWithPeriod(&bleAppContext.SwitchOffLed_timer_Id, LED_ON_TIMEOUT);
#endif
                    break;
                }

                case ACI_GAP_KEYPRESS_NOTIFICATION_VSEVT_CODE:
                {
                    LOG_INFO_APP(">>== ACI_GAP_KEYPRESS_NOTIFICATION_VSEVT_CODE");
                    break;
                }

#if 0 /* TODO  */
                case ACI_GAP_PASS_KEY_REQ_VSEVT_CODE:
                {
                    uint32_t pin;
                    LOG_INFO_APP(">>== ACI_GAP_PASS_KEY_REQ_VSEVT_CODE");

                    pin = CFG_FIXED_PIN;

                    ret = aci_gap_pass_key_resp(bleAppContext.BleApplicationContext_legacy.connectionHandle, pin);

                    if (ret != BLE_STATUS_SUCCESS) {
                        LOG_INFO_APP("==>> aci_gap_pass_key_resp : Fail, reason: 0x%02X", ret);
                    }
                    else {
                        ZDD_DEBUG("==>> aci_gap_pass_key_resp : Success");
                    }
                    break;
                }

                case ACI_GAP_NUMERIC_COMPARISON_VALUE_VSEVT_CODE:
                {
                    uint8_t confirm_value;
                    ZDD_DEBUG(">>== ACI_GAP_NUMERIC_COMPARISON_VALUE_VSEVT_CODE");
                    ZDD_DEBUG("     - numeric_value = %ld",
                        ((aci_gap_numeric_comparison_value_event_rp0 *)(p_blecore_evt->data))->Numeric_Value);
                    ZDD_DEBUG("     - Hex_value = %lx",
                        ((aci_gap_numeric_comparison_value_event_rp0 *)(p_blecore_evt->data))->Numeric_Value);

                    /* Set confirm value to 1(YES) */
                    confirm_value = 1;

                    ret = aci_gap_numeric_comparison_value_confirm_yesno(
                            bleAppContext.BleApplicationContext_legacy.connectionHandle, confirm_value);
                    if (ret != BLE_STATUS_SUCCESS) {
                        ZDD_DEBUG("==>> aci_gap_numeric_comparison_value_confirm_yesno : Fail, reason: 0x%02X", ret);
                    }
                    else {
                        ZDD_DEBUG("==>> aci_gap_numeric_comparison_value_confirm_yesno : Success");
                    }
                    break;
                }
#endif
                /* Zigbee Direct does not require Pairing. However, this is here
                 * for debugging. */
                case ACI_GAP_PAIRING_COMPLETE_VSEVT_CODE:
                {
                    aci_gap_pairing_complete_event_rp0 *p_pairing_complete;

                    LOG_INFO_APP(">>== ACI_GAP_PAIRING_COMPLETE_VSEVT_CODE");
                    p_pairing_complete = (aci_gap_pairing_complete_event_rp0 *)p_blecore_evt->data;

                    if (p_pairing_complete->Status != 0) {
                        const char *status_str;

                        switch (p_pairing_complete->Status) {
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
                    else {
                        LOG_INFO_APP("     - Pairing Success");
                    }
                    LOG_INFO_APP("");
                    break;
                }

                case ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE: /* 0x0c01 */
                    break;

                case ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE: /* 0x0c03 */
                    break;

                case ACI_GAP_LIMITED_DISCOVERABLE_VSEVT_CODE: /* 0x0400 */
                    break;

                default:
                    LOG_INFO_APP("Unhandled vendor event code (0x%04x)", p_blecore_evt->ecode);
                    handle_unhandled_event(__LINE__);
                    break;
            }
            break;
        }

        default:
            LOG_INFO_APP("Error, unhandled evt = 0x%02x", p_event_pckt->evt);
            handle_unhandled_event(__LINE__);
            break;
    }

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
        bleAppContext.Device_Connection_Status = (APP_BLE_ConnStatus_t)paramC;
        LOG_INFO_APP("==>> aci_gap_set_non_discoverable - Success\n");
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


bool zdd_port_ble_init(void) {
  
    BPKA_Reset();

#ifdef COND_USE_NVM
    /* Disable flash before any use - RFTS */
    FD_SetStatus(FD_FLASHACCESS_RFTS, LL_FLASH_DISABLE);
    /* Enable RFTS Bypass for flash operation - Since LL has not started yet */
    FD_SetStatus(FD_FLASHACCESS_RFTS_BYPASS, LL_FLASH_ENABLE);
    /* Enable flash system flag */
    FD_SetStatus(FD_FLASHACCESS_SYSTEM, LL_FLASH_ENABLE);

    /* NVM emulation in RAM initialization */
    // NVM_Init(buffer_nvm, 0, CFG_BLEPLAT_NVM_MAX_SIZE);
#endif

    zdd_generate_bd_addr();

    // Initialize zdd_port_info using the wrapper function
    initialize_zdd_port_info();

    // Initialize BleAsynchEventQueue using the wrapper function
    initialize_zdd_ble_asynch_event_queue();

    UTIL_SEQ_RegTask(1U << CFG_TASK_BLE_HOST, UTIL_SEQ_RFU, BleStackProcess);
    UTIL_SEQ_RegTask(1u << CFG_TASK_HCI_ASYNCH_EVT_ID, UTIL_SEQ_RFU, BleUserEvtRx);

    // Initialize the BLE stack using the wrapper function
    if (!initialize_ble_stack()) {
        return false;
    }

    /* Initialization of the BLE Services */
    SVCCTL_Init();

    /**
     *  Register the event handler to the BLE controller
     */
    SVCCTL_RegisterSvcHandler(zdd_svc_event_handler);

    /* Init ZDD Services */
    zdd_gatt_services_init();

    UTIL_SEQ_RegTask(1u << CFG_TASK_ZIGBEE_ZDD, 0x00, zdd_port_task);

#ifdef COND_USE_NVM
    /* Disable RFTS Bypass for flash operation - Since LL has not started yet */
    FD_SetStatus(FD_FLASHACCESS_RFTS_BYPASS, LL_FLASH_DISABLE);
#endif

    return true;
}

void ZddStackProcess(void) {

     UTIL_SEQ_SetTask(1u << CFG_TASK_ZIGBEE_ZDD, CFG_SEQ_PRIO_1);

}



tBleStatus ZddSetDiscoverable()
{
 return aci_gap_set_limited_discoverable(ADV_TYPE,
        ADV_INTERVAL_MIN, ADV_INTERVAL_MAX,
        CFG_BD_ADDRESS_TYPE, ADV_FILTER, 0, NULL, 0, NULL,
        ADV_CONN_INTVL_MIN, ADV_CONN_INTVL_MAX);

}

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/


// Wrapper function to initialize the BLE stack
static bool initialize_ble_stack(void) {
    tBleStatus return_status;
	uint32_t srd_bd_addr[2];

    return_status = BleStack_Init(&zdd_ble_stack_init_params);
    if (return_status != BLE_STATUS_SUCCESS) {
        return false;
    }

    hci_reset();

    return_status = write_zdd_bd_address();
    if (return_status != BLE_STATUS_SUCCESS) {
        return false;
    }


    srd_bd_addr[1] = 0x0000ED6E;
    srd_bd_addr[0] = LL_FLASH_GetUDN();
    aci_hal_write_config_data(CONFIG_DATA_RANDOM_ADDRESS_OFFSET, CONFIG_DATA_RANDOM_ADDRESS_LEN, (uint8_t *)srd_bd_addr);

    /* Write Identity root key used to derive IRK and DHK(Legacy) */
    return_status = aci_hal_write_config_data(CONFIG_DATA_IR_OFFSET, CONFIG_DATA_IR_LEN, (uint8_t *)zdd_BLE_CfgIrValue);
    if (return_status != BLE_STATUS_SUCCESS) {
        return false;
    }

    /* Write Encryption root key used to derive LTK and CSRK */
    return_status = aci_hal_write_config_data(CONFIG_DATA_ER_OFFSET, CONFIG_DATA_ER_LEN, (uint8_t *)zdd_BLE_CfgErValue);
    if (return_status != BLE_STATUS_SUCCESS) {
        return false;
    }

    /* Set Transmission RF Power. */
    return_status = aci_hal_set_tx_power_level(1, CFG_TX_POWER);
    if (return_status != BLE_STATUS_SUCCESS) {
        return false;
    }

    /* Initialize GATT interface */
    return_status = aci_gatt_init();
    if (return_status != BLE_STATUS_SUCCESS) {
        return false;
    }

    if (!initialize_gap_gatt_interface()) {
        return false;
    }

    /* Initialize Default PHY */
    return_status = hci_le_set_default_phy(CFG_PHY_PREF, CFG_PHY_PREF_TX, CFG_PHY_PREF_RX);
    if (return_status != BLE_STATUS_SUCCESS) {
        return false;
    }

    /* Initialize IO capability */
    return_status = aci_gap_set_io_capability(CFG_IO_CAPABILITY);
    if (return_status != BLE_STATUS_SUCCESS) {
        return false;
    }

    /* Initialize authentication */
    return_status = aci_gap_set_authentication_requirement(
        CFG_BONDING_MODE,
        CFG_MITM_PROTECTION,
        CFG_SC_SUPPORT,
        CFG_KEYPRESS_NOTIFICATION_SUPPORT,
        CFG_ENCRYPTION_KEY_SIZE_MIN,
        CFG_ENCRYPTION_KEY_SIZE_MAX,
        CFG_USED_FIXED_PIN,
        CFG_FIXED_PIN,
        CFG_BD_ADDRESS_TYPE);
    if (return_status != BLE_STATUS_SUCCESS) {
        return false;
    }

    return true;
}

static uint8_t HOST_BLE_Init(void)
{
  tBleStatus return_status;

  return_status = BleStack_Init(&zdd_ble_stack_init_params);
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
  fill_advData(&a_AdvData[0], sizeof(a_AdvData), (uint8_t*) p_bd_addr);
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

static void BLE_NvmCallback (SNVMA_Callback_Status_t CbkStatus)
{
  if (CbkStatus != SNVMA_OPERATION_COMPLETE)
  {
    /* Retry the write operation */
    SNVMA_Write (SNVMA_BufferId_1,
                 BLE_NvmCallback);
  }
}

/* USER CODE BEGIN FD_LOCAL_FUNCTION */

static void fill_advData(uint8_t *p_adv_data, uint8_t tab_size, const uint8_t* p_bd_addr)
{
  uint16_t i =0;
  uint8_t bd_addr_1, bd_addr_0;
  uint8_t ad_length, ad_type;  
  
  while(i < tab_size)
  {
    ad_length = p_adv_data[i];
    ad_type = p_adv_data[i + 1];
      
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
        p_adv_data[i+5] = BOARD_ID_NUCLEO_WBA5X; /* Board ID */
        p_adv_data[i+6] = FW_ID_HEART_RATE; /* FW ID */
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


static void BleStackProcess(void) {
    set_active_thread_name(__func__);

    if (BleStack_Process() == 0x0) {
        UTIL_SEQ_SetTask(1U << CFG_TASK_BLE_HOST, CFG_SEQ_PRIO_0);
    }

    clear_active_thread_name();
}


static void BleUserEvtRx(void) {
    SVCCTL_UserEvtFlowStatus_t svctl_return_status;
    BleEvtPacket_t *phcievt;

    set_active_thread_name(__func__);

    LST_remove_head(get_ble_asynch_event_queue(), (tListNode **)&phcievt);

    svctl_return_status = SVCCTL_UserEvtRx((void *)&(phcievt->evtserial));

    if (svctl_return_status != SVCCTL_UserEvtFlowDisable) {
       free(phcievt);
    }
    else {
        LST_insert_head(get_ble_asynch_event_queue(), (tListNode *)phcievt);
    }

    if ((LST_is_empty(get_ble_asynch_event_queue()) == FALSE) && (svctl_return_status != SVCCTL_UserEvtFlowDisable)) {
        /* LOG_INFO_APP("Kicking task: BleUserEvtRx"); */
        UTIL_SEQ_SetTask(1u << CFG_TASK_HCI_ASYNCH_EVT_ID, CFG_SEQ_PRIO_0);
    }

     /* set the BG_BleStack_Process task for scheduling */
     /* LOG_INFO_APP("Kicking task: BleStackProcess"); */
    UTIL_SEQ_SetTask(1U << CFG_TASK_BLE_HOST, CFG_SEQ_PRIO_0);

    clear_active_thread_name();
}

                                  
/*************************************************************
 *
 * WRAP FUNCTIONS
 *
 *************************************************************/

void NVMCB_Store( const uint32_t* ptr, uint32_t size )
{
  UNUSED(ptr);
  UNUSED(size);

  /* Call SNVMA for storing - Without callback */
  SNVMA_Write (SNVMA_BufferId_1,
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


tBleStatus BLECB_Indication(const uint8_t *data, uint16_t length, const uint8_t *ext_data, uint16_t ext_length) {
    UNUSED(ext_data);

    if (data[0] == HCI_EVENT_PKT_TYPE) {
       BleEvtPacket_t *phcievt;
       uint16_t total_length = (length + ext_length);

       phcievt = malloc(sizeof(BleEvtPacketHeader_t) + total_length);
       if (phcievt == NULL) {
             LOG_INFO_APP("Error, failed to allocate BleEvtPacket_t");
             handle_unhandled_event(__LINE__);
             return BLE_STATUS_FAILED;
        }

        phcievt->evtserial.type = HCI_EVENT_PKT_TYPE;
        phcievt->evtserial.evt.evtcode = data[1];
        phcievt->evtserial.evt.plen = data[2];
        memcpy((void *)&phcievt->evtserial.evt.payload, &data[3], data[2]);
        LST_insert_tail(get_ble_asynch_event_queue(), (tListNode *)phcievt);

        /* LOG_INFO_APP("Kicking task: BleUserEvtRx"); */
        UTIL_SEQ_SetTask(1u << CFG_TASK_HCI_ASYNCH_EVT_ID, CFG_SEQ_PRIO_0);
        return BLE_STATUS_SUCCESS;
     }

     if (data[0] == HCI_ACLDATA_PKT_TYPE) {
        return BLE_STATUS_SUCCESS;
     }

    handle_unhandled_event(__LINE__);
    return BLE_STATUS_FAILED;
}


/* USER CODE END FD_WRAP_FUNCTIONS */
