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
#include "lhci.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lhci.h"
#include "os_wrapper.h"
#include "stm32wbaxx_nucleo.h"
#include "stm32_lpm.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* Definitions for "uart_rx_state" */
typedef enum
{
  LOW_POWER_MODE_DISABLE,
  LOW_POWER_MODE_STOP,
  LOW_POWER_MODE_STDBY,
}LowPowerModeStatus_t;
/* USER CODE END PTD */

/* Maximum size of data buffer (Rx or Tx) */
#define HCI_DATA_MAX_SIZE         313
#define NUM_OF_TX_SYNCHRO          2
#define NUM_OF_TX_ASYNCHRO        50
#define NUM_OF_RX_BUFFER          12
#define NUM_OF_TX_BUFFER           (NUM_OF_TX_ASYNCHRO + NUM_OF_TX_SYNCHRO)

typedef struct
{
  tListNode                 node;  /* Actual node in the list */
  uint8_t buf[HCI_DATA_MAX_SIZE];  /* Memory buffer */
} UART_node;

/* Global variables structure */
typedef struct
{
  volatile uint8_t uart_tx_on;
  uint8_t  rx_state;
  uint8_t  rxReceivedState;
  UART_node buff_node[NUM_OF_RX_BUFFER+NUM_OF_TX_BUFFER];
} HciTransport_var_t;

extern RNG_HandleTypeDef hrng;

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

/* Definitions for "uart_rx_state" */
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

uint64_t buffer_nvm[CFG_BLEPLAT_NVM_MAX_SIZE] = {0};

tListNode UART_RX_Pool;
tListNode UART_RX_List;
tListNode UART_TX_Pool;
tListNode UART_TX_List;

UART_node *ongoing_TX_node;
HciTransport_var_t HCI_var;

static uint8_t *readBusBuffer;
static uint8_t *writeBusBuffer;

/* Host stack init variables */
static uint32_t buffer[DIVC(BLE_DYN_ALLOC_SIZE, 4)];
static uint32_t gatt_buffer[DIVC(BLE_GATT_BUF_SIZE, 4)];
static BleStack_init_t pInitParams;

/* USER CODE BEGIN PV */
static LowPowerModeStatus_t LowPowerModeStatus;
/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/

/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private function prototypes -----------------------------------------------*/
static void BleStack_Process_BG(void);
static void TM_Init(void);
static void TM_SysLocalCmd(uint8_t *data);
static void TM_TxToHost(void);
static void TM_EventNotify(void);
static void TM_UART_TxComplete(uint8_t *buffer);
static void TM_UART_RxComplete(uint8_t *buffer);

static void BLEUART_Write(UART_HandleTypeDef *huart, uint8_t *buffer, uint16_t size);
static void BLEUART_Read(UART_HandleTypeDef *huart, uint8_t *buffer, uint16_t size);

static int HCI_UartSend(uint8_t *data, uint8_t hci_event_type);
static uint16_t HCI_GetDataToSend(uint8_t **dataToSend);
static uint8_t* HCI_GetFreeTxBuffer(uint8_t hci_event_type);
static uint8_t* HCI_GetDataReceived(void);
static uint8_t* HCI_GetFreeRxBuffer(void);
static uint8_t HOST_BLE_Init(void);
/* USER CODE BEGIN PFP */
static void TM_SetLowPowerMode( LowPowerModeStatus_t low_power_mode_status );
/* USER CODE END PFP */

/* External variables --------------------------------------------------------*/
extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Functions Definition ------------------------------------------------------*/
void APP_BLE_Init(void)
{
  /* USER CODE BEGIN APP_BLE_Init_1 */
  
  /* USER CODE END APP_BLE_Init_1 */

  /* Register BLE Host tasks */
  UTIL_SEQ_RegTask(1U << CFG_TASK_BLE_HOST, UTIL_SEQ_RFU, BleStack_Process_BG);

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

    /* Initialize Transparent Mode Application */
    TM_Init();
  }
  /* USER CODE BEGIN APP_BLE_Init_2 */

  /* USER CODE END APP_BLE_Init_2 */

  return;
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

static void TM_Init(void)
{
  /* UART init. */
  HCI_var.uart_tx_on = 0;
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

  BLEUART_Read(&huart1, HCI_GetFreeRxBuffer(), 1 /*IDENTIFIER_OFFSET*/);

/* USER CODE BEGIN TM_Init */
  UTIL_LPM_SetOffMode(1 << CFG_LPM_APP_BLE, UTIL_LPM_DISABLE);
  UTIL_LPM_SetStopMode(1<<CFG_LPM_APP_BLE, UTIL_LPM_DISABLE);
  LowPowerModeStatus = LOW_POWER_MODE_DISABLE;
/* USER CODE END TM_Init */

  os_enable_isr();
  UTIL_SEQ_RegTask(1U << CFG_TASK_TX_TO_HOST_ID, UTIL_SEQ_RFU, TM_TxToHost);
  UTIL_SEQ_RegTask(1U << CFG_TASK_NOTIFY_EVENT_ID, UTIL_SEQ_RFU, TM_EventNotify);

}

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

static void TM_TxToHost(void)
{
  if ( HCI_var.rxReceivedState != 0 )
  {
    uint8_t *pData = 0;
    uint8_t packet_type;
    tBleStatus status = BLE_STATUS_SUCCESS;

    pData = HCI_GetDataReceived();

    if ( pData != NULL )
    {
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
  }

  if( HCI_var.uart_tx_on == 1 )
  {
    uint8_t *pData = 0;
    uint16_t size;

    HCI_var.uart_tx_on |= 2;
    size = HCI_GetDataToSend(&pData);

    if( pData != 0 )
    {
      os_disable_isr();
      BLEUART_Write(&huart1, pData, size);
      os_enable_isr();
    }
    else
    {}
  }
  else
  {}
}

static void TM_EventNotify(void)
{
  change_state_options_t event_options;

  /* Notify LL that Host is ready */
  event_options.combined_value = 0x0F;
  ll_intf_chng_evnt_hndlr_state(event_options);
}

static void TM_UART_TxComplete(uint8_t *buffer)
{
  memset(ongoing_TX_node->buf, 0, HCI_DATA_MAX_SIZE);
  LST_insert_tail(&UART_TX_Pool,(tListNode*) ongoing_TX_node);

  if ( LST_get_size(&UART_TX_List) == 0)
  {
    HCI_var.uart_tx_on = 0; /* No more data to send */
  }
  else
  {
    HCI_var.uart_tx_on = 1; /* More data to send */
    UTIL_SEQ_SetTask(1U << CFG_TASK_TX_TO_HOST_ID,CFG_SEQ_PRIO_0);
  }
}

static void TM_UART_RxComplete( uint8_t *buffer )
{
  UART_node *pNode = ((UART_node*)(UART_RX_List.prev));
  uint8_t *data = pNode->buf;
  uint16_t size_to_receive = 1, header_size, payload_size;

  switch ( HCI_var.rx_state )
  {
  case HCI_RX_STATE_WAIT_TYPE:
    {
      if ( data[0] == HCI_ACLDATA_PKT_TYPE )
      {
        HCI_var.rx_state = HCI_RX_STATE_WAIT_HEADER;
        data += 1;
        size_to_receive = HCI_ACLDATA_HDR_SIZE - 1;
      }
      else if ( (data[0] == HCI_COMMAND_PKT_TYPE) ||
                (data[0] == 0x20) )
      {
        HCI_var.rx_state = HCI_RX_STATE_WAIT_HEADER;
        data += 1;
        size_to_receive = HCI_COMMAND_HDR_SIZE - 1;
      }
      else if (data[0] == HCI_ISODATA_PKT_TYPE)
      {
        HCI_var.rx_state = HCI_RX_STATE_WAIT_HEADER;
        data += 1;
        size_to_receive = HCI_ISODATA_HDR_SIZE - 1;
      }
      else
      {
        /* Received unknown packet type: silently ignore */
      }
      break;
    }

  case HCI_RX_STATE_WAIT_HEADER:
    {
      header_size = ((data[0] == HCI_ACLDATA_PKT_TYPE) ?
                     HCI_ACLDATA_HDR_SIZE : HCI_COMMAND_HDR_SIZE);
      payload_size = data[3];
      if (data[0] == HCI_ISODATA_PKT_TYPE)
      {
        header_size = HCI_ISODATA_HDR_SIZE;
        payload_size = data[3] | ((data[4] &0x3F) << 8);
      }

      if ( payload_size > 0 )
      {
        HCI_var.rx_state = HCI_RX_STATE_WAIT_PAYLOAD;
        data += header_size;
        size_to_receive = payload_size;
        break;
      }
      /* else continue with next case */
    }

  default:
  case HCI_RX_STATE_WAIT_PAYLOAD:
    {
      HCI_var.rxReceivedState += 1;
      HCI_var.rx_state = HCI_RX_STATE_WAIT_TYPE;
      data = HCI_GetFreeRxBuffer();
    }
  }

  os_disable_isr();
  BLEUART_Read(&huart1, data, size_to_receive );
  os_enable_isr();

  UTIL_SEQ_SetTask(1U << CFG_TASK_TX_TO_HOST_ID,CFG_SEQ_PRIO_0);
}

static void BLEUART_Write(UART_HandleTypeDef *huart, uint8_t *buffer, uint16_t size)
{
  writeBusBuffer = buffer;
  HAL_UART_Transmit_DMA(huart, buffer, size);
}

static void BLEUART_Read(UART_HandleTypeDef *huart, uint8_t *buffer, uint16_t size)
{
  HAL_StatusTypeDef uart_status;
  readBusBuffer = buffer;

  uart_status = HAL_UART_Receive_DMA(huart, buffer, size);
  if ( uart_status != HAL_OK )
  {
    /* No more RX buffer available - UART blocked in idle mode */
    assert_param(0);
  }
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

  HCI_var.uart_tx_on |= 1;

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

static uint16_t HCI_GetDataToSend(uint8_t **dataToSend)
{
  uint16_t size;
  UART_node *pNode = NULL;

  if (LST_get_size(&UART_TX_List) != 0)
  {
    LST_remove_head(&UART_TX_List,(tListNode**) &pNode);

    if ( pNode->buf[0] == HCI_ACLDATA_PKT_TYPE )
    {
      size = HCI_ACLDATA_HDR_SIZE + pNode->buf[3];
    }
    else if( pNode->buf[0] == HCI_ISODATA_PKT_TYPE)
    {
      size = HCI_ISODATA_HDR_SIZE +
        (pNode->buf[3] | ((pNode->buf[4] &0x3F) << 8));
    }
    else
    {
      size = HCI_EVENT_HDR_SIZE + pNode->buf[2];
    }

    *dataToSend = &pNode->buf[0];

    ongoing_TX_node = pNode;
  }
  else
  {
    size = 0;
  }
  return size;
}

static uint8_t* HCI_GetFreeTxBuffer(uint8_t hci_event_type)
{
  UART_node *pNode = NULL;
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

static uint8_t* HCI_GetFreeRxBuffer(void)
{
  UART_node *pNode = NULL;

  if (LST_get_size(&UART_RX_Pool) != 0)
  {
    LST_remove_head(&UART_RX_Pool,(tListNode**) &pNode);
    memset( pNode->buf, 0, HCI_DATA_MAX_SIZE );
    LST_insert_tail(&UART_RX_List, (tListNode *)pNode);

    return pNode->buf;
  }
  else
  {
    return NULL;
  }
}

static uint8_t* HCI_GetDataReceived(void)
{
  UART_node *pNode = NULL;
  HCI_var.rxReceivedState -= 1;

  if (LST_get_size(&UART_RX_List) != 0)
  {
    LST_remove_head(&UART_RX_List,(tListNode**) &pNode);
    LST_insert_tail(&UART_RX_Pool, (tListNode *)pNode);
    return pNode->buf;
  }
  else
  {
    return NULL;
  }
}

static void BleStack_Process_BG(void)
{
  if (BleStack_Process( ) == 0x0)
  {
    BleStackCB_Process( );
  }
}

/* USER CODE BEGIN FD_LOCAL_FUNCTION */
static void TM_SetLowPowerMode( LowPowerModeStatus_t low_power_mode_status )
{
  if(LowPowerModeStatus == LOW_POWER_MODE_DISABLE)
  {
    #if (CFG_LED_SUPPORTED == 1)
    BSP_LED_Off(LED_GREEN);
    #endif
    UTIL_LPM_SetStopMode(1<<CFG_LPM_APP_BLE, UTIL_LPM_ENABLE);
    if(low_power_mode_status == LOW_POWER_MODE_STOP)
    {
      LowPowerModeStatus = LOW_POWER_MODE_STOP;
    }
    else
    {
      UTIL_LPM_SetOffMode(1<<CFG_LPM_APP_BLE, UTIL_LPM_ENABLE);
      LowPowerModeStatus = LOW_POWER_MODE_STDBY;
    }
  }
  else if(LowPowerModeStatus == LOW_POWER_MODE_STOP)
  {
    if(low_power_mode_status == LOW_POWER_MODE_STOP)
    {
      #if (CFG_LED_SUPPORTED == 1)
      BSP_LED_On(LED_GREEN);
      #endif
      LowPowerModeStatus = LOW_POWER_MODE_DISABLE;
      UTIL_LPM_SetStopMode(1<<CFG_LPM_APP_BLE, UTIL_LPM_DISABLE);
      UTIL_LPM_SetOffMode(1<<CFG_LPM_APP_BLE, UTIL_LPM_DISABLE);
    }
    else
    {
      LowPowerModeStatus = LOW_POWER_MODE_STDBY;
      UTIL_LPM_SetOffMode(1<<CFG_LPM_APP_BLE, UTIL_LPM_ENABLE);
    }
  }
  else /* LOW_POWER_MODE_STDBY */
  {
    /* cannot exit from standby in current implementation */
    /* missing wakeup pin */
  }
  return;
}
/* USER CODE END FD_LOCAL_FUNCTION */

/*************************************************************
 *
 * WRAP FUNCTIONS
 *
 *************************************************************/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  UNUSED(huart);
  TM_UART_RxComplete(readBusBuffer);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  TM_UART_TxComplete(writeBusBuffer);
}

tBleStatus BLECB_Indication( const uint8_t* data,
                          uint16_t length,
                          const uint8_t* ext_data,
                          uint16_t ext_length )
{
  uint8_t status;
  uint8_t bufferHci[HCI_DATA_MAX_SIZE];

  MEMCPY( &bufferHci[0], data, length);

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
    UTIL_SEQ_SetTask(1U << CFG_TASK_TX_TO_HOST_ID,CFG_SEQ_PRIO_0);
  }
  else
  {
    UTIL_SEQ_SetTask(1U << CFG_TASK_NOTIFY_EVENT_ID, CFG_SEQ_PRIO_0);
  }
  return status;
}

/* USER CODE BEGIN FD_WRAP_FUNCTIONS */
#if (CFG_BUTTON_SUPPORTED == 1)
void APPE_Button1Action(void)
{
  /* Enter/Exit Stop Mode */
  TM_SetLowPowerMode(LOW_POWER_MODE_STOP);
}
void APPE_Button2Action(void)
{
  /* Enter Standby Mode, exit must be performed by reset */
  TM_SetLowPowerMode(LOW_POWER_MODE_STDBY);
}
#endif
/* USER CODE END FD_WRAP_FUNCTIONS */
