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
#include "lhci.h"
#include "stm32_lpm.h"
#include "stm32_lpm_if.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lhci.h"
#include "os_wrapper.h"
#include "stm32wbaxx_ll_spi.h"
#include "stm32_lpm.h"
#include "stm32_lpm_if.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Maximum size of data buffer (Rx or Tx) */
#define HCI_DATA_MAX_SIZE         315
#define NUM_OF_TX_SYNCHRO         2
#define NUM_OF_TX_ASYNCHRO        50
#define NUM_OF_RX_BUFFER          12
#define NUM_OF_TX_BUFFER           (NUM_OF_TX_ASYNCHRO + NUM_OF_TX_SYNCHRO)
typedef enum {
  SPI_PROT_INIT_STATE = 0,
  SPI_PROT_IDLE,
  SPI_PROT_CS_LOW,
  SPI_PROT_WAITING_HEADER,
  SPI_PROT_WAITING_TX_COMPLETE,
  SPI_PROT_WAITING_RX_COMPLETE,
} Tm_Tl_SpiProtoType;

extern volatile Tm_Tl_SpiProtoType spi_proto_state;

#define TM_TL_SPI_STATE_TRANSACTION(NEWSTATE)        (spi_proto_state = NEWSTATE)
#define TM_TL_SPI_STATE_CHECK(STATE)                (spi_proto_state==STATE)
#define TM_TL_SPI_STATE_FROM(STATE)                 (spi_proto_state>=STATE)

#define TM_TL_SPI_HEADER_LEN  (uint8_t)(5)
#define TM_TL_SPI_CTRL_WRITE  (uint8_t)(0x0A)
#define TM_TL_SPI_CTRL_READ   (uint8_t)(0x0B)

typedef struct
{
  tListNode                 node;  /* Actual node in the list */
  uint16_t                  data_len;
  uint8_t buf[HCI_DATA_MAX_SIZE];  /* Memory buffer */
} HCIBufferNode_t;

/* Global variables structure */
typedef struct
{
  volatile uint8_t tm_tx_on;
  uint8_t  rx_state;
  uint8_t  rxReceivedState;
  uint8_t header_xfer_cmplt;
  HCIBufferNode_t buff_node[NUM_OF_RX_BUFFER+NUM_OF_TX_BUFFER];
} HciTransport_var_t;

uint8_t header_buff_tx[TM_TL_SPI_HEADER_LEN];
uint8_t header_buff_rx[TM_TL_SPI_HEADER_LEN];
volatile Tm_Tl_SpiProtoType spi_proto_state = SPI_PROT_INIT_STATE;

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

/* Definitions for "tm_rx_state" */
#define HCI_RX_STATE_WAIT_TYPE    0
#define HCI_RX_STATE_WAIT_HEADER  1
#define HCI_RX_STATE_WAIT_PAYLOAD 2

/* Definition for "hci_event_type" */
#define HCI_EVENT_SYNCHRO         0
#define HCI_EVENT_ASYNCHRO        1

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

tListNode UART_RX_Pool;
tListNode UART_RX_List;
tListNode UART_TX_Pool;
tListNode UART_TX_List;

HCIBufferNode_t *CurrentBuffNode = NULL;
HciTransport_var_t HCI_var;

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
static void TM_Init(void);
static void TM_SysLocalCmd(uint8_t *data);
static void TM_TxToHost(void);
static void TM_EventNotify(void);
static int HCI_UartSend(uint8_t *data, uint8_t hci_event_type);
static uint16_t HCI_GetDataToSend(HCIBufferNode_t **pNode);
static uint16_t HCI_GetPacketSize(uint8_t *pckt);
static uint8_t* HCI_GetFreeTxBuffer(uint8_t hci_event_type);
static HCIBufferNode_t* HCI_GetDataReceived(void);
static HCIBufferNode_t* HCI_GetFreeRxBuffer(void);
static void transport_layer_send_header(uint16_t tx_data_length);
static void transport_layer_receive_data(uint16_t data_length);
static void transport_layer_send_data(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions prototypes ---------------------------------------------*/

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* External variables --------------------------------------------------------*/
extern SPI_HandleTypeDef hspi1;
#define TM_SPI_HANDLE hspi1

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Functions Definition ------------------------------------------------------*/
void APP_BLE_Init(void)
{
  /* USER CODE BEGIN APP_BLE_Init_1 */
  
  /* USER CODE END APP_BLE_Init_1 */

  /* Register BLE Host tasks */
  UTIL_SEQ_RegTask(1U << CFG_TASK_BLE_HOST, UTIL_SEQ_RFU, BleStack_Process_BG);

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

    /* Initialize Transparent Mode Application */
    TM_Init();
  }
  /* USER CODE BEGIN APP_BLE_Init_2 */

  /* USER CODE END APP_BLE_Init_2 */

  return;
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

/**
  * @brief Initialize Transparent Mode
  * @retval None
  */
static void TM_Init(void)
{
  /* UART init. */
  HCI_var.tm_tx_on = 0;
  HCI_var.rx_state = HCI_RX_STATE_WAIT_TYPE;
  HCI_var.rxReceivedState = 0;

  LST_init_head(&UART_RX_Pool);
  LST_init_head(&UART_RX_List);
  LST_init_head(&UART_TX_Pool);
  LST_init_head(&UART_TX_List);

  for (uint8_t rx_index = 0; rx_index < NUM_OF_RX_BUFFER; rx_index++)
  {
    LST_insert_tail(&UART_RX_Pool, (tListNode *)(&(HCI_var.buff_node[rx_index])));
  }

  for (uint8_t tx_index = 0; tx_index < NUM_OF_TX_BUFFER; tx_index++)
  {
    LST_insert_tail(&UART_TX_Pool, (tListNode *)(&(HCI_var.buff_node[NUM_OF_RX_BUFFER + tx_index])));
  }

  /* USER CODE BEGIN TM_Init */

  /* USER CODE END TM_Init */

  UTIL_LPM_SetMaxMode(1U << CFG_LPM_APP_BLE, UTIL_LPM_MAX_MODE);
  TM_TL_SPI_STATE_TRANSACTION(SPI_PROT_IDLE);

  os_enable_isr();
  /* Register tasks */
  UTIL_SEQ_RegTask(1U << CFG_TASK_TX_TO_HOST_ID, UTIL_SEQ_RFU, TM_TxToHost);
  UTIL_SEQ_RegTask(1U << CFG_TASK_NOTIFY_EVENT_ID, UTIL_SEQ_RFU, TM_EventNotify);

}

/**
  * @brief Process local system commands
  * @param data: pointer to the system command packet data
  * @retval None
  */
static void TM_SysLocalCmd (uint8_t *data)
{
  uint16_t lcmd_opcode =(uint16_t)(data[1] | (data[2]<<8));

  switch(lcmd_opcode)
  {
    case LHCI_OPCODE_C1_WRITE_REG:
      LHCI_C1_Write_Register((BleCmdSerial_t*)data);
      break;

    case LHCI_OPCODE_C1_READ_REG:
      LHCI_C1_Read_Register((BleCmdSerial_t*)data);
      break;

    case LHCI_OPCODE_C1_DEVICE_INF:
      LHCI_C1_Read_Device_Information((BleCmdSerial_t*)data);
      break;

    case LHCI_OPCODE_C1_RF_CONTROL_ANTENNA_SWITCH:
      LHCI_C1_RF_CONTROL_AntennaSwitch((BleCmdSerial_t*)data);
      break;

    default:
      ((TL_CcEvt_t*)(((BleEvtSerial_t*)data)->evt.payload))->cmdcode = lcmd_opcode;
      ((TL_CcEvt_t*)(((BleEvtSerial_t*)data)->evt.payload))->payload[0] = 0x01;
      ((TL_CcEvt_t*)(((BleEvtSerial_t*)data)->evt.payload))->numcmd = 1;
      ((BleEvtSerial_t*)data)->type = TL_LOCRSP_PKT_TYPE;
      ((BleEvtSerial_t*)data)->evt.evtcode = HCI_COMMAND_COMPLETE_EVT_CODE;
      ((BleEvtSerial_t*)data)->evt.plen = TL_EVT_CS_PAYLOAD_SIZE;

      break;
  }

  return;
}

/**
  * @brief  EXTI line rising detection callback.
  * @param  GPIO_Pin Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == TM_SPI_CS_Pin)
  {
    if(TM_TL_SPI_STATE_CHECK(SPI_PROT_WAITING_RX_COMPLETE))
    {
      if(CurrentBuffNode != NULL)
      {
        uint16_t expected_data_length = (header_buff_rx[2] << 8) + header_buff_rx[1];

        CurrentBuffNode->data_len = (expected_data_length - __HAL_DMA_GET_COUNTER(TM_SPI_HANDLE.hdmarx));

        LST_insert_tail(&UART_RX_List, (tListNode *)CurrentBuffNode);
        CurrentBuffNode = NULL;

        /* Schedule task to process received data. */
        UTIL_SEQ_SetTask(1U << CFG_TASK_TX_TO_HOST_ID,CFG_SEQ_PRIO_0);
      }

      UTIL_LPM_SetMaxMode(1U << CFG_LPM_APP_BLE, UTIL_LPM_MAX_MODE);

      TM_TL_SPI_STATE_TRANSACTION(SPI_PROT_IDLE);
    }
    else if(TM_TL_SPI_STATE_CHECK(SPI_PROT_WAITING_TX_COMPLETE))
    {
      LST_insert_tail(&UART_TX_Pool,(tListNode*) CurrentBuffNode);
      CurrentBuffNode = NULL;

      /* Schedule task to check for other data to send */
      UTIL_SEQ_SetTask(1U << CFG_TASK_TX_TO_HOST_ID,CFG_SEQ_PRIO_0);

      UTIL_LPM_SetMaxMode(1U << CFG_LPM_APP_BLE, UTIL_LPM_MAX_MODE);

      TM_TL_SPI_STATE_TRANSACTION(SPI_PROT_IDLE);
    }

    /* Handle special case where external MCU raised CS before all data were transferred. */
    if(HAL_SPI_GetState(&TM_SPI_HANDLE) != HAL_SPI_STATE_READY)
    {
      HAL_SPI_Abort_IT(&TM_SPI_HANDLE);
      if(TM_TL_SPI_STATE_CHECK(SPI_PROT_WAITING_RX_COMPLETE) && CurrentBuffNode != NULL)
      {
        LST_insert_tail(&UART_RX_List, (tListNode *)CurrentBuffNode);
        CurrentBuffNode = NULL;
      }
      else if(TM_TL_SPI_STATE_CHECK(SPI_PROT_WAITING_TX_COMPLETE) && CurrentBuffNode != NULL)
      {
        LST_insert_tail(&UART_TX_Pool,(tListNode*) CurrentBuffNode);
        CurrentBuffNode = NULL;
      }
    }
  }
}

/**
  * @brief  EXTI line falling detection callback.
  * @param  GPIO_Pin Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == TM_SPI_CS_Pin)
  {
    if(TM_TL_SPI_STATE_CHECK(SPI_PROT_IDLE))
    {
      TM_TL_SPI_STATE_TRANSACTION(SPI_PROT_CS_LOW);
      UTIL_SEQ_SetTask(1U << CFG_TASK_TX_TO_HOST_ID,CFG_SEQ_PRIO_0);
    }
    else if(TM_TL_SPI_STATE_CHECK(SPI_PROT_WAITING_HEADER))
    {
      /* Do nothing. Header is already ready to be sent and IRQ is high. */
    }
  }
}

static void TM_TxToHost(void)
{
  if(TM_TL_SPI_STATE_CHECK(SPI_PROT_CS_LOW))
  {
    TM_TL_SPI_STATE_TRANSACTION(SPI_PROT_WAITING_HEADER);

    transport_layer_send_header(0);

    LL_GPIO_SetOutputPin(TM_SPI_IRQ_GPIO_Port, TM_SPI_IRQ_Pin);

    UTIL_LPM_SetMaxMode(1U << CFG_LPM_APP_BLE, UTIL_LPM_SLEEP_MODE);
  }

  if(HCI_var.header_xfer_cmplt)
  {
    HCI_var.header_xfer_cmplt = FALSE;

    if(TM_TL_SPI_STATE_CHECK(SPI_PROT_WAITING_HEADER))
    {
      if(header_buff_rx[0] == TM_TL_SPI_CTRL_WRITE)
      {
        uint16_t rx_data_length = (header_buff_rx[2] << 8) + header_buff_rx[1];

        TM_TL_SPI_STATE_TRANSACTION(SPI_PROT_WAITING_RX_COMPLETE);

        transport_layer_receive_data(rx_data_length);

        LL_GPIO_ResetOutputPin(TM_SPI_IRQ_GPIO_Port, TM_SPI_IRQ_Pin);
      }
      else if(header_buff_rx[0] == TM_TL_SPI_CTRL_READ)
      {
        TM_TL_SPI_STATE_TRANSACTION(SPI_PROT_WAITING_TX_COMPLETE);

        transport_layer_send_data();

        LL_GPIO_ResetOutputPin(TM_SPI_IRQ_GPIO_Port, TM_SPI_IRQ_Pin);
      }
    }
  }

  /* Command FIFO */
  if ( !LST_is_empty(&UART_RX_List) )
  {
    uint16_t size;
    uint8_t packet_type;
    tBleStatus status = BLE_STATUS_SUCCESS;
    HCIBufferNode_t *pNode;
    uint8_t *pData = NULL;

    pNode = HCI_GetDataReceived();
    size = HCI_GetPacketSize(pNode->buf);

    if(size == pNode->data_len)
    {
      /* packet is valid */

      pData = pNode->buf;

      packet_type = *pData;

      if( packet_type == TL_LOCCMD_PKT_TYPE )
      {
        TM_SysLocalCmd(pData);
      }

      BleStack_Request(pData);
      BleStackCB_Process();

      if(( packet_type == 0x01 ) || ( packet_type == TL_LOCCMD_PKT_TYPE ) || ( packet_type == TL_LOCRSP_PKT_TYPE ))
      {
        status = HCI_UartSend(pData, HCI_EVENT_SYNCHRO);
      }

      if( status != BLE_STATUS_SUCCESS )
      {
        /* No more TX buffer available - Synchro event not sent */
        assert_param(0);
      }
    }
    else
    {
      assert_param(0);
    }

    LST_insert_tail(&UART_RX_Pool, (tListNode *)pNode);
  }

  if ( TM_TL_SPI_STATE_CHECK(SPI_PROT_IDLE) && !LST_is_empty(&UART_TX_List) )
  {
    uint16_t size;
    HCIBufferNode_t *pNode = NULL;

    pNode = (HCIBufferNode_t *)UART_TX_List.next;

    size = HCI_GetPacketSize(pNode->buf);

    TM_TL_SPI_STATE_TRANSACTION(SPI_PROT_WAITING_HEADER);

    transport_layer_send_header(size);

    LL_GPIO_SetOutputPin(TM_SPI_IRQ_GPIO_Port, TM_SPI_IRQ_Pin);

    UTIL_LPM_SetMaxMode(1U << CFG_LPM_APP_BLE, UTIL_LPM_SLEEP_MODE);
  }
}

/**
  * @brief Notify to host that a new event is ready to be received
  * @retval None
  */
static void TM_EventNotify(void)
{
  change_state_options_t event_options;

  /* Notify LL that Host is ready */
  event_options.combined_value = 0x0F;
  ll_intf_chng_evnt_hndlr_state(event_options);
}

static int HCI_UartSend(uint8_t *data, uint8_t hci_event_type)
{
  uint16_t size;
  uint8_t *pData = HCI_GetFreeTxBuffer(hci_event_type);

  if ( pData == 0 )
  {
    /* No more TX buffer available */
    return BLE_STATUS_FAILED;
  }

  HCI_var.tm_tx_on |= 1;

  if (data[0] == HCI_ACLDATA_PKT_TYPE)
  {
    size = HCI_ACLDATA_HDR_SIZE + data[3];
  }
  else if (data[0] == HCI_ISODATA_PKT_TYPE)
  {
    size = HCI_ISODATA_HDR_SIZE + (data[3] | ((data[4] &0x3F) << 8) );
  }
  else if (data[0] == TL_LOCRSP_PKT_TYPE)
  {
    size = HCI_EVENT_HDR_SIZE + data[2];
  }
  else
  {
    size = HCI_EVENT_HDR_SIZE + data[2];
  }

  if( size > 255 )
  {
    memcpy( pData, data, 254);
    memcpy( pData + 254, data + 254, size - 254);
  }
  else
  {
    memcpy( pData, data, size);
  }

  return BLE_STATUS_SUCCESS;
}
static uint16_t HCI_GetDataToSend(HCIBufferNode_t **pNode)
{
  uint16_t size;

  if (!LST_is_empty(&UART_TX_List))
  {
    LST_remove_head(&UART_TX_List,(tListNode**) pNode);

    if ( (*pNode)->buf[0] == HCI_ACLDATA_PKT_TYPE )
    {
      size = HCI_ACLDATA_HDR_SIZE + (*pNode)->buf[3];
    }
    else if( (*pNode)->buf[0] == HCI_ISODATA_PKT_TYPE)
    {
      size = HCI_ISODATA_HDR_SIZE +
        ((*pNode)->buf[3] | (((*pNode)->buf[4] &0x3F) << 8));
    }
    else
    {
      size = HCI_EVENT_HDR_SIZE + (*pNode)->buf[2];
    }
  }
  else
  {
    size = 0;
  }

  return size;
}

static uint16_t HCI_GetPacketSize(uint8_t *pckt)
{
  uint16_t size = 0;

  if ( pckt[0] == HCI_ACLDATA_PKT_TYPE )
  {
    size = HCI_ACLDATA_HDR_SIZE + pckt[3];
  }
  else if( pckt[0] == HCI_ISODATA_PKT_TYPE)
  {
    size = HCI_ISODATA_HDR_SIZE +
      (pckt[3] | ((pckt[4] &0x3F) << 8));
  }
  else if( pckt[0] == HCI_EVENT_PKT_TYPE || pckt[0] == TL_LOCRSP_PKT_TYPE)
  {
    size = HCI_EVENT_HDR_SIZE + pckt[2];
  }
  else if( pckt[0] == HCI_COMMAND_PKT_TYPE || pckt[0] == TL_LOCCMD_PKT_TYPE)
  {
    size = HCI_COMMAND_HDR_SIZE + pckt[3];
  }

  return size;
}

static uint8_t* HCI_GetFreeTxBuffer(uint8_t hci_event_type)
{
  HCIBufferNode_t *pNode = NULL;
  uint8_t size = LST_get_size(&UART_TX_Pool);

  if (((hci_event_type == HCI_EVENT_ASYNCHRO) && (size > NUM_OF_TX_SYNCHRO))
      || ((hci_event_type == HCI_EVENT_SYNCHRO) && (size > 0)))
  {
    LST_remove_head(&UART_TX_Pool,(tListNode**) &pNode);

    if (pNode != NULL)
    {
      LST_insert_tail(&UART_TX_List, (tListNode *)pNode);
      return pNode->buf;
    }
    else
    {
      return NULL;
    }
  }
  else
  {
    return NULL;
  }
}

static HCIBufferNode_t* HCI_GetFreeRxBuffer(void)
{
  HCIBufferNode_t *pNode = NULL;

  if (!LST_is_empty(&UART_RX_Pool))
  {
    LST_remove_head(&UART_RX_Pool,(tListNode**) &pNode);
    memset( pNode->buf, 0, HCI_DATA_MAX_SIZE );

    return pNode;
  }
  else
  {
    return NULL;
  }
}

static HCIBufferNode_t* HCI_GetDataReceived(void)
{
  HCIBufferNode_t *pNode = NULL;
  HCI_var.rxReceivedState -= 1;

  if (!LST_is_empty(&UART_RX_List))
  {
    LST_remove_head(&UART_RX_List,(tListNode**) &pNode);

    return pNode;
  }
  else
  {
    return NULL;
  }
}

static void transport_layer_send_header(uint16_t tx_data_length)
{
  uint16_t rx_buffer_size;

  if (!LST_is_empty(&UART_RX_Pool))
  {
    rx_buffer_size = HCI_DATA_MAX_SIZE;
  }
  else
  {
    rx_buffer_size = 0;
  }

  header_buff_tx[0] = 0xFF;
  header_buff_tx[1] = (uint8_t)rx_buffer_size;
  header_buff_tx[2] = (uint8_t)(rx_buffer_size>>8);
  header_buff_tx[3] = (uint8_t)tx_data_length;
  header_buff_tx[4] = (uint8_t)(tx_data_length>>8);

  memset(header_buff_rx, 0, sizeof(header_buff_rx));

  HCI_var.header_xfer_cmplt = FALSE;

  if(HAL_SPI_TransmitReceive_DMA(&TM_SPI_HANDLE, header_buff_tx, header_buff_rx, TM_TL_SPI_HEADER_LEN) != HAL_OK)
  {
    /* Transfer error in transmission process */
    Error_Handler();
  }
}

static void transport_layer_receive_data(uint16_t data_length)
{
  HCIBufferNode_t* rx_buff = HCI_GetFreeRxBuffer();
  assert_param(rx_buff != NULL);

  if(rx_buff == NULL)
  {
    return;
  }

  assert_param(CurrentBuffNode == NULL);
  CurrentBuffNode = rx_buff;

  if(HAL_SPI_Receive_DMA(&TM_SPI_HANDLE, rx_buff->buf, MIN(data_length, HCI_DATA_MAX_SIZE)) != HAL_OK)
  {
    Error_Handler();
  }
}

static void transport_layer_send_data(void)
{
  uint16_t size;

  assert_param(CurrentBuffNode == NULL);

  size = HCI_GetDataToSend(&CurrentBuffNode);

  assert_param(size != 0);

  if(HAL_SPI_Transmit_DMA(&TM_SPI_HANDLE, CurrentBuffNode->buf, size) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  TxRx Transfer completed callback.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report end of DMA TxRx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
  HCI_var.header_xfer_cmplt = TRUE;
  UTIL_SEQ_SetTask(1U << CFG_TASK_TX_TO_HOST_ID,CFG_SEQ_PRIO_0);
}

/**
  * @brief  SPI error callbacks.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
  Error_Handler();
}

/* USER CODE BEGIN FD_LOCAL_FUNCTION */

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
  uint8_t status;
  uint8_t bufferHci[HCI_DATA_MAX_SIZE];

  /* USER CODE BEGIN BLECB_Indication */

  /* USER CODE END BLECB_Indication */

  /* Copy data to buffer */
  MEMCPY( &bufferHci[0], data, length);

  /* Copy exteded data ot the buffer */
  if ( ext_length > 255 )
  {
    MEMCPY( &bufferHci[length], ext_data, 254);
    MEMCPY( &bufferHci[length + 254], ext_data + 254, ext_length - 254 );
  }
  else
  {
    MEMCPY( &bufferHci[length], ext_data, ext_length );
  }

  if (bufferHci[1] == 0xFF) /* ACI events */
  {
    status = HCI_UartSend(&bufferHci[0], HCI_EVENT_SYNCHRO);
  }
  else
  {
    status = HCI_UartSend(&bufferHci[0], HCI_EVENT_ASYNCHRO);
  }

  if(status == BLE_STATUS_SUCCESS)
  {
    /* If packet has been successfully processed, run TX to host task */
    UTIL_SEQ_SetTask(1U << CFG_TASK_TX_TO_HOST_ID,CFG_SEQ_PRIO_0);
  }
  else
  {
    /* If packet hasn't been successfully processed, run Notify Event task */
    UTIL_SEQ_SetTask(1U << CFG_TASK_NOTIFY_EVENT_ID, CFG_SEQ_PRIO_0);
  }
  return status;
}

/* USER CODE BEGIN FD_WRAP_FUNCTIONS */


/* USER CODE END FD_WRAP_FUNCTIONS */
