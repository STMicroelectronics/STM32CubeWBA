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
#include "ble.h"
#include "app_ble.h"
#include "host_stack_if.h"
#include "ll_sys_if.h"
#include "stm32_seq.h"
#include "otp.h"
#include "stm32_timer.h"
#include "stm_list.h"
#include "nvm.h"
#include "advanced_memory_manager.h"
#include "blestack.h"
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
  LOW_POWER_MODE_ENABLE,
}LowPowerModeStatus_t;
/* USER CODE END PTD */

/* Definitions for "uart_rx_state" */
enum
{
  HCI_RX_STATE_WAIT_TYPE = 0,
  HCI_RX_STATE_WAIT_HEADER,
  HCI_RX_STATE_WAIT_PAYLOAD
};

/* Maximum size of data buffer (Rx or Tx) */
#define HCI_DATA_MAX_SIZE         313
#define NUM_OF_TX_BUFFER           60
#define NUM_OF_RX_BUFFER           60

/* Global variables structure */
typedef struct
{
  uint8_t   uart_state;
  volatile uint8_t uart_tx_on;
  uint16_t uart_rx_size;
  uint16_t rx_total_size;
  uint8_t  rx_state;
  uint8_t  rx_idx;
  uint8_t  index_rx_free;
  uint8_t  index_received;
  uint8_t  rx_buf[NUM_OF_RX_BUFFER][HCI_DATA_MAX_SIZE];
  uint8_t  index_free;
  uint8_t  index_to_send;
  uint8_t  tx_buf[NUM_OF_TX_BUFFER][HCI_DATA_MAX_SIZE];
  uint8_t  rxReceivedState;
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

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

uint64_t buffer_nvm[CFG_BLEPLAT_NVM_MAX_SIZE] = {0};

HciTransport_var_t HCI_var;
uint8_t bufferHci[HCI_DATA_MAX_SIZE] = {0};

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
static void TM_Init( void );
static void TM_SysLocalCmd(uint8_t *data);
static void TM_TxToHost( void );
static void TM_UART_TxComplete(uint8_t *buffer);
static void TM_UART_RxComplete( uint8_t *buffer );

static void BLEUART_Write(UART_HandleTypeDef *huart, uint8_t *buffer, uint16_t size);
static void BLEUART_Read(UART_HandleTypeDef *huart, uint8_t *buffer, uint16_t size);

static int HCI_UartSend( uint8_t *data );
static uint16_t HCI_GetDataToSend( uint8_t **dataToSend );
static uint8_t* HCI_GetFreeTxBuffer( void );
static uint8_t* HCI_GetDataReceived( void );
static uint8_t* HCI_GetFreeRxBuffer( void );
static void BLE_NvmCallback (SNVMA_Callback_Status_t);
static uint8_t  HOST_BLE_Init(void);
/* USER CODE BEGIN PFP */
static void TM_SetLowPowerMode( void );
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

/* USER CODE BEGIN FD*/

/* USER CODE END FD*/

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

static void TM_Init( void )
{
  /* UART init. */
  HCI_var.uart_state = 0;
  HCI_var.uart_tx_on = 0;
  HCI_var.rx_state = HCI_RX_STATE_WAIT_TYPE;
  HCI_var.rx_idx = 0;
  HCI_var.rxReceivedState = 0;

  os_disable_isr();
  HCI_var.uart_state |= 1;

  BLEUART_Read(&huart1, HCI_GetFreeRxBuffer(), 1 /*IDENTIFIER_OFFSET*/);

/* USER CODE BEGIN TM_Init*/
  UTIL_LPM_SetOffMode(1 << CFG_LPM_APP_BLE, UTIL_LPM_DISABLE);
  UTIL_LPM_SetStopMode(1<<CFG_LPM_APP_BLE, UTIL_LPM_DISABLE);
  LowPowerModeStatus = LOW_POWER_MODE_DISABLE;
/* USER CODE END TM_Init*/

  os_enable_isr();
  UTIL_SEQ_RegTask(1U << CFG_TASK_TX_TO_HOST_ID, UTIL_SEQ_RFU, TM_TxToHost);

}

static void TM_SysLocalCmd (uint8_t *data)
{
  data = &HCI_var.rx_buf[HCI_var.rx_idx-1][0];
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

static void TM_TxToHost( void )
{
  if ( HCI_var.rxReceivedState != 0 )
  {
    uint8_t *pData = 0;
    uint8_t packet_type;

    pData = HCI_GetDataReceived();

    if ( pData != NULL )
    {
      packet_type = *pData;
      HCI_var.uart_rx_size = 0;
      HCI_var.rx_total_size = 0;
      HCI_var.uart_state &= ~2;
      BleStack_Request( pData );
      HostStack_Process();

      if( packet_type == 0x01 )
      {
        HCI_UartSend( pData );
      }
      else if( packet_type == TL_LOCCMD_PKT_TYPE )
      {
        TM_SysLocalCmd(pData);
        HCI_UartSend( pData );
      }
      else if( packet_type == TL_LOCRSP_PKT_TYPE )
      {
        HCI_UartSend( pData );
      }
    }
  }

  if( HCI_var.uart_tx_on == 1 )
  {
    uint8_t *pData = 0;
    uint16_t size;

    HCI_var.uart_tx_on |= 2;
    size = HCI_GetDataToSend( &pData );

    if( pData != 0 )
    {
      HCI_var.uart_state |= 8;

      os_disable_isr();
      BLEUART_Write(&huart1, pData, size );
      os_enable_isr();
    }
    else
    {}
  }
  else
  {}

}

static void TM_UART_TxComplete(uint8_t *buffer)
{
  change_state_options_t event_options;
  HCI_var.index_to_send++;

  /* Notify LL that Host is ready */
  event_options.combined_value = 0x0F;
  ll_intf_chng_evnt_hndlr_state(event_options);

  if ( HCI_var.index_to_send == NUM_OF_TX_BUFFER )
  {
    HCI_var.index_to_send = 0;
  }
  if ( HCI_var.index_free == HCI_var.index_to_send)
  {
    HCI_var.uart_tx_on = 0; /* No more data to send */
  }
  else
  {
    HCI_var.uart_tx_on = 1; /* More data to send */
    UTIL_SEQ_SetTask(1U << CFG_TASK_TX_TO_HOST_ID,CFG_SEQ_PRIO_0);
  }

  HCI_var.uart_state  &= ~8;
}

static void TM_UART_RxComplete( uint8_t *buffer )
{
  uint8_t *data = HCI_var.rx_buf[HCI_var.rx_idx];
  uint16_t size_to_receive = 1, header_size, payload_size;

  HCI_var.uart_state &= ~1;

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
      HCI_var.rx_total_size = header_size + payload_size;

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
      HCI_var.uart_rx_size = HCI_var.rx_total_size;
      HCI_var.uart_state |= 2;
      HCI_var.rxReceivedState += 1;
      HCI_var.rx_idx++;
      if( HCI_var.rx_idx == NUM_OF_RX_BUFFER )
      {
        HCI_var.rx_idx = 0;
      }
      HCI_var.rx_state = HCI_RX_STATE_WAIT_TYPE;
      data = HCI_GetFreeRxBuffer();
    }
  }

  HCI_var.uart_state |= 1;

  os_disable_isr();
  BLEUART_Read(&huart1, data, size_to_receive );
  os_enable_isr();

  UTIL_SEQ_SetTask(1U << CFG_TASK_TX_TO_HOST_ID,CFG_SEQ_PRIO_0);
}

static void BLEUART_Write(UART_HandleTypeDef *huart, uint8_t *buffer, uint16_t size)
{
  writeBusBuffer = buffer;
#if defined(OPENTHREAD_FTD) || defined(OPENTHREAD_MTD)
  HAL_UART_Transmit_IT(huart, buffer, size);
#else
  HAL_UART_Transmit_DMA(huart, buffer, size);
#endif
  return;
}

static void BLEUART_Read(UART_HandleTypeDef *huart, uint8_t *buffer, uint16_t size)
{
  readBusBuffer = buffer;
#if defined(OPENTHREAD_FTD) || defined(OPENTHREAD_MTD)
  HAL_UART_Receive_IT(huart, buffer, size);
#else
  HAL_UART_Receive_DMA(huart, buffer, size);
#endif
  return;
}

static int HCI_UartSend( uint8_t *data )
{
  uint16_t size;
  uint8_t *pData = HCI_GetFreeTxBuffer();

  if ( pData == 0 )
    return 1;

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

  return 0u;
}

static uint16_t HCI_GetDataToSend( uint8_t **dataToSend )
{
  uint16_t size;

  if ( HCI_var.tx_buf[HCI_var.index_to_send][0] == HCI_ACLDATA_PKT_TYPE )
  {
    size = HCI_ACLDATA_HDR_SIZE + HCI_var.tx_buf[HCI_var.index_to_send][3];
  }
  else if( HCI_var.tx_buf[HCI_var.index_to_send][0] == HCI_ISODATA_PKT_TYPE)
  {
    size = HCI_ISODATA_HDR_SIZE +
      (HCI_var.tx_buf[HCI_var.index_to_send][3] |
       ((HCI_var.tx_buf[HCI_var.index_to_send][4] &0x3F) << 8));
  }
  else /* TL_LOCCMD_PKT_TYPE, TL_LOCRSP_PKT_TYPE and other*/
  {
    size = HCI_EVENT_HDR_SIZE + HCI_var.tx_buf[HCI_var.index_to_send][2];
  }

  *dataToSend = &HCI_var.tx_buf[HCI_var.index_to_send][0];

  return size;
}

uint8_t* HCI_GetFreeTxBuffer( void )
{
  uint8_t *pBuffer = 0;

  pBuffer = &HCI_var.tx_buf[HCI_var.index_free][0];

  if( (HCI_var.index_free + 1) == HCI_var.index_to_send)
  {
    //No more data free.
    pBuffer = NULL;
  }
  else if( (HCI_var.index_free + 1) == NUM_OF_TX_BUFFER )
  {
    if( HCI_var.index_to_send == 0)
    {
      // No more free buffer: index_free = index_to_send = 0
      pBuffer = NULL;
    }
    else
    {
      HCI_var.index_free = 0;
    }
  }
  else
  {
    HCI_var.index_free++;
  }

  return pBuffer;
}

static uint8_t* HCI_GetFreeRxBuffer( void )
{
  uint8_t *pBuffer = 0;

  pBuffer = &HCI_var.rx_buf[HCI_var.index_rx_free][0];

  memset( pBuffer, 0, HCI_DATA_MAX_SIZE );
  HCI_var.index_rx_free++;
  if ( HCI_var.index_rx_free == NUM_OF_RX_BUFFER )
  {
    HCI_var.index_rx_free = 0;
  }

  if ( HCI_var.index_rx_free == HCI_var.index_received)
  {
    /* No more data free */
  }

  return pBuffer;
}

static uint8_t* HCI_GetDataReceived( void )
{
  uint8_t *pBuffer = 0;

  HCI_var.rxReceivedState -= 1;

  pBuffer = &HCI_var.rx_buf[HCI_var.index_received][0];

  /* Increase the index of received data */
  HCI_var.index_received++;
  if ( HCI_var.index_received == NUM_OF_RX_BUFFER )
  {
    HCI_var.index_received = 0;
  }

  return pBuffer;
}

static void BleStack_Process_BG(void)
{
  if (BleStack_Process( ) == 0x0)
  {
    BleStackCB_Process( );
  }
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
static void TM_SetLowPowerMode(void)
{
  if(LowPowerModeStatus == LOW_POWER_MODE_DISABLE)
  {
    BSP_LED_Off(LED_GREEN);
    LowPowerModeStatus = LOW_POWER_MODE_ENABLE;
    UTIL_LPM_SetStopMode(1<<CFG_LPM_APP_BLE, UTIL_LPM_ENABLE);
  }
  else
  {
    BSP_LED_On(LED_GREEN);
    LowPowerModeStatus = LOW_POWER_MODE_DISABLE;
    UTIL_LPM_SetStopMode(1<<CFG_LPM_APP_BLE, UTIL_LPM_DISABLE);
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
  uint8_t status = BLE_STATUS_FAILED;
  memcpy( &bufferHci[0], data, length);

  if ( ext_length > 255 )
  {
    memcpy( &bufferHci[length], ext_data, 254);
    memcpy( &bufferHci[length + 254], ext_data + 254, ext_length - 254 );
  }
  else
  {
    memcpy( &bufferHci[length], ext_data, ext_length );
  }

  if(HCI_UartSend( &bufferHci[0] ) == 0){
    UTIL_SEQ_SetTask(1U << CFG_TASK_TX_TO_HOST_ID,CFG_SEQ_PRIO_0);
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
#if (CFG_BUTTON_SUPPORTED == 1)
void APPE_Button1Action(void)
{
   TM_SetLowPowerMode();
}
#endif
/* USER CODE END FD_WRAP_FUNCTIONS */
