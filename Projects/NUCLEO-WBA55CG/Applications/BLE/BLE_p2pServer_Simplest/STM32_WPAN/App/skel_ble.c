/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ble_if.c
  * @author  MCD Application Team
  * @brief   BLE Application
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
#include "main.h"
#include "app_common.h"
#include "ble_core.h"
#include "svc_ctl.h"
#include "skel_ble.h"
#include "ll_sys_if.h"
#include "stm_list.h"
#include "blestack.h"
#include "host_stack_if.h"
/* Private includes ----------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */
typedef struct{
  uint16_t MyBLEServiceHandle;              /**< Service handle */
  uint16_t MyBLEWriteCharacteristicHandle;  /**< Write Characteristic handle */
  uint16_t MyBLENotifyCharacteristicHandle; /**< Notify Characteristic handle */
} MyBLEServiceContext_t;
static MyBLEServiceContext_t myBLEServiceContext;

/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/
/* GATT buffer size (in bytes)*/
#define BLE_GATT_BUF_SIZE \
          BLE_TOTAL_BUFFER_SIZE_GATT(CFG_BLE_NUM_GATT_ATTRIBUTES, \
                                     CFG_BLE_NUM_GATT_SERVICES, \
                                     CFG_BLE_ATT_VALUE_ARRAY_SIZE)

#define BLE_DYN_ALLOC_SIZE \
        (BLE_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_LINK, CFG_BLE_MBLOCK_COUNT, 0))

/* Dummy value for NVM buffer in RAM for BLE Host stack */
#define CFG_BLE_NVM_SIZE_MAX            4

/* Flag define */
#define APP_FLAG_BLE_HOST               1
#define APP_FLAG_HCI_ASYNCH_EVT_ID      2
#define APP_FLAG_LINK_LAYER             3
/* USER CODE BEGIN Flag */
#define APP_FLAG_BT_PUSH                4
/* USER CODE END Flag */
#define APP_FLAG_GET(flag)                  VariableBit_Get_BB(((uint32_t)&APP_State), flag)
#define APP_FLAG_SET(flag)                  VariableBit_Set_BB(((uint32_t)&APP_State), flag)
#define APP_FLAG_RESET(flag)                VariableBit_Reset_BB(((uint32_t)&APP_State), flag)

#define CFG_BD_ADDRESS                    (0x0080E12A1234)

/* USER CODE BEGIN PD */
/**
*  Service 128bits UUID
*/
const uint8_t BLEservice_UUID[16] =
{0x8f, 0xe5, 0xb3, 0xd5, 0x2e, 0x7f, 0x4a, 0x98, 0x2a, 0x48, 0x7a, 0xcc, 0x40, 0xfe, 0x00, 0x00};
/**
*  Write characteristic 128bits UUID 
*/
const uint8_t BLEWriteCharacteristic_UUID[16] =
{0x19, 0xed, 0x82, 0xae, 0xed, 0x21, 0x4c, 0x9d, 0x41, 0x45, 0x22, 0x8e, 0x41, 0xfe, 0x00, 0x00};
/**
*  Notify  characteristic 128bits UUID
*/
const uint8_t BLENotifyCharacteristic_UUID[16] =
{0x19, 0xed, 0x82, 0xae, 0xed, 0x21, 0x4c, 0x9d, 0x41, 0x45, 0x22, 0x8e, 0x42, 0xfe, 0x00, 0x00};

static SVCCTL_EvtAckStatus_t BLEService_EventHandler(void *pckt);

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static volatile uint32_t APP_State = 0x00000000;

static tListNode BleAsynchEventQueue;

static const uint8_t a_MBdAddr[BD_ADDR_SIZE] =
{
  (uint8_t)((CFG_BD_ADDRESS & 0x0000000000FF)),
  (uint8_t)((CFG_BD_ADDRESS & 0x00000000FF00) >> 8),
  (uint8_t)((CFG_BD_ADDRESS & 0x000000FF0000) >> 16),
  (uint8_t)((CFG_BD_ADDRESS & 0x0000FF000000) >> 24),
  (uint8_t)((CFG_BD_ADDRESS & 0x00FF00000000) >> 32),
  (uint8_t)((CFG_BD_ADDRESS & 0xFF0000000000) >> 40)
};

static const char a_GapDeviceName[] = {  'S', 'T', 'M', '3', '2', 'W', 'B', 'A' }; /* Gap Device Name */

/* Host stack init variables */
static BleStack_init_t pInitParams;

/* Host stack buffers */
static uint32_t host_buffer[DIVC(BLE_DYN_ALLOC_SIZE, 4)];
static uint32_t gatt_buffer[DIVC(BLE_GATT_BUF_SIZE, 4)];
static uint64_t host_nvm_buffer[CFG_BLE_NVM_SIZE_MAX];

/* USER CODE BEGIN PV */
Service_UUID_t service_uuid;
Char_UUID_t char_uuid;

/* Notification status */
uint8_t Notif_Status;
uint8_t Notif_Value[2];

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/

/* USER CODE BEGIN GV */
static const char gap_ad_local_name[] = { AD_TYPE_COMPLETE_LOCAL_NAME, 'P', '2', 'P', '_', 'S', 'I', 'M', 'P', 'L', 'E', 'S', 'T' };

uint8_t exData[25] ={
                       8, AD_TYPE_COMPLETE_LOCAL_NAME, 'p', '2', 'p', 'S', '_', 'S', 'I',  /* Complete name */
                       15, AD_TYPE_MANUFACTURER_SPECIFIC_DATA, 0x30, 0x00, 0x00 , 0x00, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00, 
                       };
/* USER CODE END GV */

/* Private function prototypes -----------------------------------------------*/
static void BleStack_Process_BG(void);
static void Ble_UserEvtRx(void);
static void Ble_Hci_Gap_Gatt_Init(void);
static uint8_t  HOST_BLE_Init(void);
void LINKLAYER_DEBUG_SIGNAL_SET(void* signal);
void LINKLAYER_DEBUG_SIGNAL_RESET(void* signal);
void LINKLAYER_DEBUG_SIGNAL_TOGGLE(void* signal);

/* USER CODE BEGIN PFP */
static void BLE_GATT_MyBLENotifyCharacteristic_Update(void);
/* USER CODE END PFP */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Functions Definition ------------------------------------------------------*/
void BLE_Init(void)
{
  /* USER CODE BEGIN APP_BLE_Init_1 */

  /* USER CODE END APP_BLE_Init_1 */

  LST_init_head(&BleAsynchEventQueue);

  /* USER CODE BEGIN APP_BLE_Init_2 */

  /* USER CODE END APP_BLE_Init_2 */

  /* Initialize the BLE Host */
  if (HOST_BLE_Init() == 0u)
  {
    /* Initialization of HCI & GATT & GAP layer */
    Ble_Hci_Gap_Gatt_Init();

    /* Initialization of the BLE Services */
    SVCCTL_Init();

    /* USER CODE BEGIN APP_BLE_Init_3 */
    BLE_Service_Init();
    /* USER CODE END APP_BLE_Init_3 */
  }
  /* USER CODE BEGIN APP_BLE_Init_4 */
  Notif_Status = 0;
  
  tBleStatus ret = aci_gap_set_discoverable(ADV_IND, 100, 100, GAP_PUBLIC_ADDR,
                               NO_WHITE_LIST_USE,
                               sizeof(gap_ad_local_name),
                               (const uint8_t *)gap_ad_local_name,
                               0, NULL, 0, 0);
  
  if (ret != BLE_STATUS_SUCCESS)
  {
    Error_Handler();
  }
  
  ret = aci_gap_update_adv_data(25, exData);
  if (ret != BLE_STATUS_SUCCESS)
  {
    Error_Handler();
  }
  /* USER CODE END APP_BLE_Init_4 */

  return;
}

SVCCTL_UserEvtFlowStatus_t SVCCTL_App_Notification(void *p_Pckt)
{
  hci_event_pckt    *p_event_pckt;
  evt_le_meta_event *p_meta_evt;

  p_event_pckt = (hci_event_pckt*) ((hci_uart_pckt *) p_Pckt)->data;

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
      tBleStatus ret = aci_gap_set_discoverable(ADV_IND, 100, 100, GAP_PUBLIC_ADDR,
                               NO_WHITE_LIST_USE,
                               sizeof(gap_ad_local_name),
                               (const uint8_t *)gap_ad_local_name,
                               0, NULL, 0, 0);
  
      if (ret != BLE_STATUS_SUCCESS)
      {
        Error_Handler();
      }
      
      ret = aci_gap_update_adv_data(25, exData);
      if (ret != BLE_STATUS_SUCCESS)
      {
        Error_Handler();
      }
      /* USER CODE END EVT_DISCONN_COMPLETE_1 */
      break; /* HCI_DISCONNECTION_COMPLETE_EVT_CODE */
    }

    case HCI_LE_META_EVT_CODE:
    {
      p_meta_evt = (evt_le_meta_event*) p_event_pckt->data;
      /* USER CODE BEGIN EVT_LE_META_EVENT */

      /* USER CODE END EVT_LE_META_EVENT */

      switch (p_meta_evt->subevent)
      {
        case HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE:
        {
          hci_le_connection_complete_event_rp0 *p_conn_complete;
          p_conn_complete = (hci_le_connection_complete_event_rp0 *) p_meta_evt->data;
          UNUSED(p_conn_complete);
          /* USER CODE BEGIN HCI_EVT_LE_CONN_COMPLETE */

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
      break; /* HCI_LE_META_EVT_CODE */
    }
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

/**
  * @brief  loop function calling process request.
  * @param  None
  * @retval None
  */
void BLE_Process(void){
  if (APP_FLAG_GET(APP_FLAG_LINK_LAYER) == 1){
    APP_FLAG_RESET(APP_FLAG_LINK_LAYER);
    ll_sys_bg_process();
  }

  if (APP_FLAG_GET(APP_FLAG_BLE_HOST) == 1)
  {
    APP_FLAG_RESET(APP_FLAG_BLE_HOST);
    BleStack_Process_BG();
  }

  if (APP_FLAG_GET(APP_FLAG_HCI_ASYNCH_EVT_ID) == 1)
  {
    APP_FLAG_RESET(APP_FLAG_HCI_ASYNCH_EVT_ID);
    Ble_UserEvtRx();
  }
  /* USER CODE BEGIN BLE_Process */
  if (APP_FLAG_GET(APP_FLAG_BT_PUSH) == 1)
  {
    APP_FLAG_RESET(APP_FLAG_BT_PUSH);
    BLE_GATT_MyBLENotifyCharacteristic_Update(); 
  }
  /* USER CODE END BLE_Process */

}

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
uint8_t HOST_BLE_Init(void)
{
  tBleStatus return_status = BLE_STATUS_FAILED;

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
  uint16_t gap_service_handle, gap_dev_name_char_handle, gap_appearance_char_handle;
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;

  /* USER CODE BEGIN Ble_Hci_Gap_Gatt_Init */

  /* USER CODE END Ble_Hci_Gap_Gatt_Init */

  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET,
                                  CONFIG_DATA_PUBADDR_LEN,
                                  (uint8_t*) a_MBdAddr);
  if (ret != BLE_STATUS_SUCCESS)
  {
    Error_Handler();
  }

  /* Initialize GATT interface */
  ret = aci_gatt_init();
  if (ret != BLE_STATUS_SUCCESS)
  {
    Error_Handler();
  }

  /* Initialize GAP interface */
  role = 0x01;

  /* USER CODE BEGIN Role_Mngt */

  /* USER CODE END Role_Mngt */

  if (role > 0)
  {
    ret = aci_gap_init(role,
                       PRIVACY_DISABLED,
                       sizeof(a_GapDeviceName),
                       &gap_service_handle,
                       &gap_dev_name_char_handle,
                       &gap_appearance_char_handle);

    if (ret != BLE_STATUS_SUCCESS)
    {
      Error_Handler();
    }
  }

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTION */
/**
* @brief This function init and register BLE service and its services
*
* @param  None
* @retval None
*/
void BLE_Service_Init(void)
{
 tBleStatus ret = BLE_STATUS_SUCCESS;
  
  /**
  *	Register the event handler to the BLE controller
  */
  SVCCTL_RegisterSvcHandler(BLEService_EventHandler);

  /**
  *  Add My Very Own Service
  */
  memcpy(&service_uuid.Service_UUID_128, BLEservice_UUID, 16);
  ret = aci_gatt_add_service(UUID_TYPE_128,
                       (Service_UUID_t *) &service_uuid,
                       PRIMARY_SERVICE,
                       MY_BLE_SERVICE_MAX_ATT_RECORDS,
                       &(myBLEServiceContext.MyBLEServiceHandle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    Error_Handler(); /* UNEXPECTED */
  }

  /**
  *  Add My Very Own Write Characteristic
  */
  memcpy(&char_uuid.Char_UUID_128, BLEWriteCharacteristic_UUID, 16);
  ret = aci_gatt_add_char(myBLEServiceContext.MyBLEServiceHandle,
                    UUID_TYPE_128, &char_uuid,
                    MY_BLE_WRITE_CHARACTERISTIC_VALUE_LENGTH,
                    CHAR_PROP_WRITE_WITHOUT_RESP|CHAR_PROP_READ,
                    ATTR_PERMISSION_NONE,
                    GATT_NOTIFY_ATTRIBUTE_WRITE, /* gattEvtMask */
                    10, /* encryKeySize */
                    1, /* isVariable */
                    &(myBLEServiceContext.MyBLEWriteCharacteristicHandle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    Error_Handler(); /* UNEXPECTED */
  }

  /**
  *   Add My Very Own Notify Characteristic
  */
  memcpy(&char_uuid.Char_UUID_128, BLENotifyCharacteristic_UUID, 16);
  ret = aci_gatt_add_char(myBLEServiceContext.MyBLEServiceHandle,
                    UUID_TYPE_128, &char_uuid,
                    MY_BLE_NOTIFY_CHARACTERISTIC_VALUE_LENGTH,
                    CHAR_PROP_NOTIFY,
                    ATTR_PERMISSION_NONE,
                    GATT_NOTIFY_ATTRIBUTE_WRITE, /* gattEvtMask */
                    10, /* encryKeySize */
                    1, /* isVariable: 1 */
                    &(myBLEServiceContext.MyBLENotifyCharacteristicHandle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    Error_Handler(); /* UNEXPECTED */
  }
  
  /* Device nb 1 */
  Notif_Value[0] = 1;
}


/**
* @brief  Event Handler of BLE Service
* @param  Event: Address of the buffer holding the Event
* @retval Ack: Return whether the Event has been managed or not
*/
static SVCCTL_EvtAckStatus_t BLEService_EventHandler(void *Event)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *event_pckt;
  evt_blecore_aci *blecore_evt;
  aci_gatt_attribute_modified_event_rp0 *attribute_modified;
  return_value = SVCCTL_EvtNotAck;

  event_pckt = (hci_event_pckt *)(((hci_uart_pckt*)Event)->data);

  switch(event_pckt->evt)
  {
  case HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE:
    {
      blecore_evt = (evt_blecore_aci*)event_pckt->data;
      switch(blecore_evt->ecode)
      {
      case ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE:
        /**
        *  Identify for which service and characteristic event was detected
        */
        attribute_modified = (aci_gatt_attribute_modified_event_rp0*)blecore_evt->data;
        if(attribute_modified->Attr_Handle == (myBLEServiceContext.MyBLEWriteCharacteristicHandle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
        {
          if (attribute_modified->Attr_Data[1] == 0x00)
          {
            HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
          }else{
            HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
          }
        }
        
        if(attribute_modified->Attr_Handle == (myBLEServiceContext.MyBLENotifyCharacteristicHandle + CHARACTERISTIC_CONFIGURATION_DESCRIPTOR_OFFSET))
        {
          if (attribute_modified->Attr_Data[0] == 0x01)
          {
            // user action if notification enabled
            Notif_Status=attribute_modified->Attr_Data[0];
          }
        }

        break;

      default:
        break;
      }
    }
    break; /* HCI_HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE_SPECIFIC */

  default:
    break;
  }

  return(return_value);
}

static void BLE_GATT_MyBLENotifyCharacteristic_Update()
{
  tBleStatus ret = BLE_STATUS_SUCCESS;
  
  if(Notif_Value[1] == 0){
    Notif_Value[1] = 1;
  }else{
    Notif_Value[1] = 0;
  }
  
  if(Notif_Status==1){
    ret = aci_gatt_update_char_value(myBLEServiceContext.MyBLEServiceHandle,
                                     myBLEServiceContext.MyBLENotifyCharacteristicHandle,
                                     0, /* charValOffset */
                                     2, /* charValueLen */
                                     Notif_Value);
    if (ret != BLE_STATUS_SUCCESS)
    {
      Error_Handler(); /* UNEXPECTED */
    }
  }
}

/* USER CODE END FD_LOCAL_FUNCTION */

/*************************************************************
 *
 * WRAP FUNCTIONS
 *
 *************************************************************/

static void Ble_UserEvtRx( void)
{
  SVCCTL_UserEvtFlowStatus_t svctl_return_status;
  BleEvtPacket_t *phcievt;

  LST_remove_head ( &BleAsynchEventQueue, (tListNode **)&phcievt );

  svctl_return_status = SVCCTL_UserEvtRx((void *)&(phcievt->evtserial));

  if (svctl_return_status != SVCCTL_UserEvtFlowDisable)
  {
    free(phcievt);
  }
  else
  {
    LST_insert_head ( &BleAsynchEventQueue, (tListNode *)phcievt );
  }

  if ((LST_is_empty(&BleAsynchEventQueue) == FALSE) && (svctl_return_status != SVCCTL_UserEvtFlowDisable) )
  {
    APP_FLAG_SET(APP_FLAG_HCI_ASYNCH_EVT_ID);
  }

  /* set the BG_BleStack_Process task for scheduling */
  BleStackCB_Process();
}

static void BleStack_Process_BG(void)
{
  if (BleStack_Process( ) == 0x0)
  {
    BleStackCB_Process( );
  }
}

tBleStatus BLECB_Indication( const uint8_t* data,
                          uint16_t length,
                          const uint8_t* ext_data,
                          uint16_t ext_length )
{
  uint8_t status = BLE_STATUS_FAILED;
  BleEvtPacket_t *phcievt;
  uint16_t total_length = (length+ext_length);

  UNUSED(ext_data);

  if (data[0] == HCI_EVENT_PKT_TYPE)
  {
    phcievt = malloc(sizeof(BleEvtPacketHeader_t) + total_length);
    phcievt->evtserial.type = HCI_EVENT_PKT_TYPE;
    phcievt->evtserial.evt.evtcode = data[1];
    phcievt->evtserial.evt.plen  = data[2];
    memcpy( (void*)&phcievt->evtserial.evt.payload, &data[3], data[2]);
    LST_insert_tail(&BleAsynchEventQueue, (tListNode *)phcievt);

    APP_FLAG_SET(APP_FLAG_HCI_ASYNCH_EVT_ID);
    status = BLE_STATUS_SUCCESS;
  }
  else if (data[0] == HCI_ACLDATA_PKT_TYPE)
  {
    status = BLE_STATUS_SUCCESS;
  }
  return status;
}

/**
  * @brief  LL processing callback.
  * @param  None
  * @retval None
  */
void ll_process(void)
{
  APP_FLAG_SET(APP_FLAG_LINK_LAYER);
}

/* USER CODE BEGIN FD_WRAP_FUNCTIONS */
void APP_BLE_Key_Button1_Action(void)
{
  APP_FLAG_SET(APP_FLAG_BT_PUSH);
}

/* USER CODE END FD_WRAP_FUNCTIONS */

/*************************************************************
 *
 * HOST_STACK
 *
 *************************************************************/
/**
  * @brief  BLE Host stack processing callback.
  * @param  None
  * @retval None
  */
void Ble_HostStack_Process(void)
{
  APP_FLAG_SET(APP_FLAG_BLE_HOST);
}

/*************************************************************/

/* Link Layer debug API definition */
void LINKLAYER_DEBUG_SIGNAL_SET(void* signal)
{
  /* USER CODE BEGIN LINKLAYER_DEBUG_SIGNAL_SET */

  /* USER CODE END LINKLAYER_DEBUG_SIGNAL_SET */
  return;
}

void LINKLAYER_DEBUG_SIGNAL_RESET(void* signal)
{
  /* USER CODE BEGIN LINKLAYER_DEBUG_SIGNAL_RESET */

  /* USER CODE END LINKLAYER_DEBUG_SIGNAL_RESET */
  return;
}

void LINKLAYER_DEBUG_SIGNAL_TOGGLE(void* signal)
{
  /* USER CODE BEGIN LINKLAYER_DEBUG_SIGNAL_TOGGLE */

  /* USER CODE END LINKLAYER_DEBUG_SIGNAL_TOGGLE */
  return;
}

