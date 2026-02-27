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
#include "lhci.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lhci.h"
#include "os_wrapper.h"
#include "stm32wbaxx_nucleo.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

#define NUM_OF_RX_BUFFER          12
#define NUM_OF_TX_BUFFER          50

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

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* Pools for USB packets */
tListNode USB_RX_Pool;
tListNode USB_RX_List;
tListNode USB_TX_Pool;
tListNode USB_TX_List;

/* USB Packets */
USBNode_t USB_Node[ NUM_OF_RX_BUFFER + NUM_OF_TX_BUFFER];

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
static tBleStatus TM_AddUSBTXPacket(uint8_t *pData);
/* USER CODE BEGIN PFP */

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

/**
  * @brief Retrieve a USB packet from the USB RX pool
  * @retval a USB packet from the USB RX pool
  */
USBNode_t* TM_GetUSBRXPacket(void)
{
  USBNode_t *pNode = NULL;
  if (LST_get_size(&USB_RX_Pool) != 0)
  {
  /* Retrieve and reset packet from USB RX Pool */
    LST_remove_head(&USB_RX_Pool,(tListNode**) &pNode);
    memset(pNode->buf, 0, HCI_DATA_MAX_SIZE);
    pNode->len = 0;
  }

  return pNode;
}

/**
  * @brief Add a USB packet to USB RX list and run the TX to Host task to process it
  * @param pNode: a pointer to the USB packet to add to the USB RX list
  * @retval status of the operation
  */
tBleStatus TM_AddUSBRXPacket(USBNode_t *pNode)
{
  /* Insert packet in RX List */
  LST_insert_tail(&USB_RX_List, (tListNode *)pNode);

  /* Run TX_TO_HOST task */
  UTIL_SEQ_SetTask(1U << CFG_TASK_TX_TO_HOST_ID,CFG_SEQ_PRIO_0);
  return BLE_STATUS_SUCCESS;
}

/**
  * @brief Retrieve a USB packet from the USB TX list
  * @retval a USB packet from the USB TX List
  */
USBNode_t* TM_GetUSBTXPacket(void)
{
  USBNode_t *pNode = NULL;
  if (LST_get_size(&USB_TX_List) != 0)
  {
    /* Retrieve packet from USB TX List */
    LST_remove_head(&USB_TX_List,(tListNode**) &pNode);
  }

  return pNode;
}

/**
  * @brief Returns USB packet to the USB TX Pool
  * @param pNode: pointer to the packet to return to the TX Pool
  * @retval None
  */
void TM_FreeUSBTXPacket(USBNode_t *pNode)
{
  /* Insert packet in USB TX Pool */
  LST_insert_tail(&USB_TX_Pool, (tListNode *)pNode);
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

  LOG_INFO_APP("[TM] Transparent Mode Init\n");

  /* Init USB packets lists */
  LST_init_head(&USB_RX_Pool);
  LST_init_head(&USB_RX_List);
  LST_init_head(&USB_TX_Pool);
  LST_init_head(&USB_TX_List);

  /* Fill USB RX Pool list */
  for (uint8_t rx_index = 0; rx_index < NUM_OF_RX_BUFFER; rx_index++)
  {
    LST_insert_tail(&USB_RX_Pool, (tListNode *)(&(USB_Node[rx_index])));
  }

  /* Fill USB TX Pool list */
  for (uint8_t tx_index = 0; tx_index < NUM_OF_TX_BUFFER; tx_index++)
  {
    LST_insert_tail(&USB_TX_Pool, (tListNode *)(&(USB_Node[NUM_OF_RX_BUFFER + tx_index])));
  }

  /* USER CODE BEGIN TM_Init */

  /* USER CODE END TM_Init */

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
  * @brief Retrieves packets from the USB RX list to send them to the Host Stack
  * @retval None
  */
static void TM_TxToHost(void)
{
  USBNode_t *pNode = NULL;
  while (LST_get_size(&USB_RX_List) != 0)
  {
    uint8_t packet_type;

    /* Retrieve packet from the USB RX list */
    LST_remove_head(&USB_RX_List,(tListNode**) &pNode);
    packet_type = pNode->buf[0];
    LOG_INFO_APP("[TM] Sending packet of type %02X and size %d to Host\n", packet_type, pNode->len);

    if( packet_type == TL_LOCCMD_PKT_TYPE )
    {
      /* Local System Command */
      TM_SysLocalCmd(pNode->buf);
    }

    /* Send packet and run host */
    BleStack_Request(pNode->buf);
    BleStackCB_Process();

    /* If packet doesn't expect asynchronous event, send it to USB directly */
    if(( packet_type == HCI_COMMAND_PKT_TYPE ) || ( packet_type == TL_LOCCMD_PKT_TYPE ) || ( packet_type == TL_LOCRSP_PKT_TYPE ))
    {
      tBleStatus status;
      status = TM_AddUSBTXPacket(pNode->buf);

      if( status != BLE_STATUS_SUCCESS )
      {
        /* No more TX buffer available - Synchro event not sent */
        assert_param(0);
      }
    }

    /* Return packet to USB RX Pool */
    LST_insert_tail(&USB_RX_Pool, (tListNode *)pNode);
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

/**
  * @brief Retrieve a packet from the USB TX Pool and insert it to the USB TX List to be sent through USB
  * @param pData: pointer to the data of the packet to send through USB
  * @retval None
  */
static tBleStatus TM_AddUSBTXPacket(uint8_t *pData)
{
  /* Retrieve packet from TX Pool */
  if (LST_get_size(&USB_TX_Pool) != 0)
  {
    USBNode_t *pNode = NULL;
    uint16_t size;

    /* Retrieve and reset packet from the USB TX Pool */
    LST_remove_head(&USB_TX_Pool,(tListNode**) &pNode);
    memset( pNode->buf, 0, HCI_DATA_MAX_SIZE );

    /* Process HCI packet length */
    if (pData[0] == HCI_ACLDATA_PKT_TYPE)
    {
      size = HCI_ACLDATA_HDR_SIZE + pData[3];
    }
    else if (pData[0] == HCI_ISODATA_PKT_TYPE)
    {
      size = HCI_ISODATA_HDR_SIZE + (pData[3] | ((pData[4] &0x3F) << 8) );
    }
    else if (pData[0] == TL_LOCRSP_PKT_TYPE)
    {
      size = HCI_EVENT_HDR_SIZE + pData[2];
    }
    else
    {
      size = HCI_EVENT_HDR_SIZE + pData[2];
    }

    /* Copy data to buffer */
    if( size > 255 )
    {
      memcpy( pNode->buf, pData, 254);
      memcpy( pNode->buf + 254, pData + 254, size - 254);
    }
    else
    {
      memcpy( pNode->buf, pData, size);
    }
    pNode->len = size;

    /* Insert packet to the USB TX List and run the USB task */
    LST_insert_tail(&USB_TX_List, (tListNode *)pNode);
    UTIL_SEQ_SetTask(1U << CFG_TASK_USBX, CFG_SEQ_PRIO_0);

    return BLE_STATUS_SUCCESS;
  }
  else
  {
    return BLE_STATUS_OUT_OF_MEMORY;
  }
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

  LOG_INFO_APP("[TM] Received %d bytes packet from host\n", length + ext_length);

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

  status = TM_AddUSBTXPacket(bufferHci);

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
