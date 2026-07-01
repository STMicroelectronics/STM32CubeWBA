/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ots_app.c
  * @author  GPAM Application Team
  * @brief   Implementation of Object Transfer Profile for ESL images
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
#include "app_ble.h"
#include "ll_sys_if.h"
#include "dbg_trace.h"
#include "ots_app.h"
#include "ots.h"
#include "stm32_rtos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32_timer.h"
#include "esl_device.h"
#include "flash_manager.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

#define MAX_OBJ_NAME_LENGTH                                                   10

typedef union
{
  uint32_t  b32[FLASH_PAGE_SIZE/4];
  uint8_t   b8[FLASH_PAGE_SIZE];
}buff_t;
/* USER CODE END PTD */

typedef enum
{
  Oacp_INDICATION_OFF,
  Oacp_INDICATION_ON,
  Olcp_INDICATION_OFF,
  Olcp_INDICATION_ON,
  /* USER CODE BEGIN Service1_APP_SendInformation_t */

  /* USER CODE END Service1_APP_SendInformation_t */
  OTS_APP_SENDINFORMATION_LAST
} OTS_APP_SendInformation_t;

typedef struct
{
  OTS_APP_SendInformation_t     Oacp_Indication_Status;
  OTS_APP_SendInformation_t     Olcp_Indication_Status;
  /* USER CODE BEGIN Service1_APP_Context_t */
  uint8_t   curr_obj_idx;
  uint8_t   curr_obj_id[OBJECT_ID_SIZE];
  char      curr_obj_name[MAX_OBJ_NAME_LENGTH];
  uint16_t  conn_handle;
  uint8_t  l2cap_ch_id;
  UTIL_TIMER_Object_t timer;    /* timer used for timeout */
  bool      timer_expired;
  bool      write_started;
  bool      flash_buff_valid; /* true if content of flash_buff is valid and still needs to be written */
  uint32_t  write_length;
  uint8_t   write_mode;
  uint32_t  curr_address;     /* current address in Flash where data has to be written  */
  uint32_t  end_address;      /* first free address after last data to be written in Flash. */
  buff_t    flash_buff;       /* This is needed to modify the page */
  /* USER CODE END Service1_APP_Context_t */
  uint16_t              ConnectionHandle;
} OTS_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define MAX_OBJ_IDX                                             (NUM_IMAGES - 1)
    
/* Bitmask for Mode Parameter of OACP write */    
#define OACP_WRITE_MODE_TRUNCATE                                            0x02

/* Bitmask for Object Properties */
#define OBJ_PROP_DELETE                                               0x00000001
#define OBJ_PROP_EXEC                                                 0x00000002
#define OBJ_PROP_READ                                                 0x00000004
#define OBJ_PROP_WRITE                                                0x00000008
#define OBJ_PROP_APPEND                                               0x00000010
#define OBJ_PROP_TRUNC                                                0x00000020
#define OBJ_PROP_PATCH                                                0x00000040
#define OBJ_PROP_MARK                                                 0x00000080

/* Object properties: Write & Patch */
#define DEFAULT_OBJ_PROPERTIES    (OBJ_PROP_WRITE|OBJ_PROP_TRUNC|OBJ_PROP_PATCH)

/* OACP Features bitmask */
#define OACP_FEAT_CREATE                                              0x00000001
#define OACP_FEAT_DELETE                                              0x00000002
#define OACP_FEAT_CALC_CHECKSUM                                       0x00000004
#define OACP_FEAT_EXECUTE                                             0x00000008
#define OACP_FEAT_READ                                                0x00000010
#define OACP_FEAT_WRITE                                               0x00000020
#define OACP_FEAT_APPEND                                              0x00000040
#define OACP_FEAT_TRUNCATE                                            0x00000080
#define OACP_FEAT_PATCH                                               0x00000100
#define OACP_FEAT_ABORT                                               0x00000200

/* OLCP Features bitmask */
#define OLCP_FEAT_GOTO                                                0x00000001
#define OLCP_FEAT_ORDER                                               0x00000002
#define OLCP_FEAT_REQ_NUM_OBJ                                         0x00000004
#define OLCP_FEAT_CLEAR_MARK                                          0x00000008

/* OACP Write Op Code and patching Supported */
#define OACP_FEATURES_FIELD   (OACP_FEAT_WRITE|OACP_FEAT_TRUNCATE|OACP_FEAT_PATCH)
/* No optional OLCP features */
#define OLCP_FEATURES_FIELD                                           0x00000000

/* Object type: unspecified */
#define OBJ_TYPE                                                          0x2ACA

#define PAGE_OFFSET_MASK                                   (FLASH_PAGE_SIZE - 1)

#define OBJ_TRANSFER_TIMEOUT_MS                                            30000

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

#define BEGIN_OF_PAGE(a)                             ((a) & (~PAGE_OFFSET_MASK))
#define END_OF_PAGE(a)                            (((a) | PAGE_OFFSET_MASK) + 1)

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static OTS_APP_Context_t OTS_APP_Context;

uint8_t a_OTS_UpdateCharData[247];

/* USER CODE BEGIN PV */

static const uint16_t obj_type = OBJ_TYPE;

static FM_FlashOp_Status_t FlashStatus;

static void FlashCallback(FM_FlashOp_Status_t Status);

static FM_CallbackNode_t FlashCallbackNode = 
{
  /* Header for chained list */
  .NodeList = 
  {
    .next = NULL,
    .prev = NULL
  },
  /* Callback for request status */
  .Callback = FlashCallback
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void OTS_Oacp_SendIndication(void);
static void OTS_Olcp_SendIndication(void);

/* USER CODE BEGIN PFP */
static void loadFlashPage(uint32_t address);
static void writeFlashPage(uint32_t address);
static void ObjTransferTimeout(void *arg);
static void updateObjectSize(uint32_t size);
static void OTS_APP_Process(void);
static void OTS_APP_ProcessRequest(void);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void OTS_Notification(OTS_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_Notification_1 */

  /* USER CODE END Service1_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_Notification_Service1_EvtOpcode */

    /* USER CODE END Service1_Notification_Service1_EvtOpcode */

    case OTS_OTF_READ_EVT:
      /* USER CODE BEGIN Service1Char1_READ_EVT */

      /* USER CODE END Service1Char1_READ_EVT */
      break;

    case OTS_OBN_READ_EVT:
      /* USER CODE BEGIN Service1Char2_READ_EVT */

      /* USER CODE END Service1Char2_READ_EVT */
      break;

    case OTS_OBT_READ_EVT:
      /* USER CODE BEGIN Service1Char3_READ_EVT */

      /* USER CODE END Service1Char3_READ_EVT */
      break;

    case OTS_OBS_READ_EVT:
      /* USER CODE BEGIN Service1Char4_READ_EVT */

      /* USER CODE END Service1Char4_READ_EVT */
      break;

    case OTS_OBI_READ_EVT:
      /* USER CODE BEGIN Service1Char5_READ_EVT */

      /* USER CODE END Service1Char5_READ_EVT */
      break;

    case OTS_OBP_READ_EVT:
      /* USER CODE BEGIN Service1Char6_READ_EVT */

      /* USER CODE END Service1Char6_READ_EVT */
      break;

    case OTS_OACP_WRITE_EVT:
      /* USER CODE BEGIN Service1Char7_WRITE_EVT */

      /* USER CODE END Service1Char7_WRITE_EVT */
      break;

    case OTS_OACP_INDICATE_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char7_INDICATE_ENABLED_EVT */

      /* USER CODE END Service1Char7_INDICATE_ENABLED_EVT */
      break;

    case OTS_OACP_INDICATE_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char7_INDICATE_DISABLED_EVT */

      /* USER CODE END Service1Char7_INDICATE_DISABLED_EVT */
      break;

    case OTS_OLCP_WRITE_EVT:
      /* USER CODE BEGIN Service1Char8_WRITE_EVT */

      /* USER CODE END Service1Char8_WRITE_EVT */
      break;

    case OTS_OLCP_INDICATE_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char8_INDICATE_ENABLED_EVT */

      /* USER CODE END Service1Char8_INDICATE_ENABLED_EVT */
      break;

    case OTS_OLCP_INDICATE_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char8_INDICATE_DISABLED_EVT */

      /* USER CODE END Service1Char8_INDICATE_DISABLED_EVT */
      break;

    default:
      /* USER CODE BEGIN Service1_Notification_default */

      /* USER CODE END Service1_Notification_default */
      break;
  }
  /* USER CODE BEGIN Service1_Notification_2 */

  /* USER CODE END Service1_Notification_2 */
  return;
}

void OTS_APP_EvtRx(OTS_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_APP_EvtRx_1 */

  /* USER CODE END Service1_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_APP_EvtRx_Service1_EvtOpcode */

    /* USER CODE END Service1_APP_EvtRx_Service1_EvtOpcode */
    case OTS_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_CONN_HANDLE_EVT */

      /* USER CODE END Service1_APP_CONN_HANDLE_EVT */
      break;

    case OTS_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_DISCON_HANDLE_EVT */

      /* USER CODE END Service1_APP_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN Service1_APP_EvtRx_default */

      /* USER CODE END Service1_APP_EvtRx_default */
      break;
  }

  /* USER CODE BEGIN Service1_APP_EvtRx_2 */

  /* USER CODE END Service1_APP_EvtRx_2 */

  return;
}

void OTS_APP_Init(void)
{
  UNUSED(OTS_APP_Context);
  OTS_Init();

  /* USER CODE BEGIN Service1_APP_Init */
  uint32_t ots_feature[2] = {OACP_FEATURES_FIELD, OLCP_FEATURES_FIELD};
  
  OTS_APP_Context.l2cap_ch_id = 0xFF;
  
  OTS_Data_t ots_information_data;
  
  ots_information_data.p_Payload = (uint8_t*)ots_feature;
  ots_information_data.Length = sizeof(ots_feature);
  OTS_UpdateValue(OTS_OTF, &ots_information_data);
  
  UTIL_TIMER_Create(&OTS_APP_Context.timer,
                    OBJ_TRANSFER_TIMEOUT_MS,
                    UTIL_TIMER_ONESHOT,
                    ObjTransferTimeout, 0);
  OTS_APP_Context.timer_expired = false;
  
  UTIL_SEQ_RegTask(1<<CFG_TASK_OTS_PROCESS, UTIL_SEQ_RFU, OTS_APP_Process);
  /* USER CODE END Service1_APP_Init */
  return;
}

/* USER CODE BEGIN FD */

void OTS_APP_DeleteImages(void)
{
  uint32_t page_num = (OTS_FLASH_STORAGE_START_ADDRESS - FLASH_BASE) / FLASH_PAGE_SIZE;
  uint32_t num_pages = (OTS_FLASH_STORAGE_END_ADDRESS - OTS_FLASH_STORAGE_START_ADDRESS) / FLASH_PAGE_SIZE;
  uint32_t PageError;
  
  FLASH_EraseInitTypeDef EraseInit = {
    .TypeErase = FLASH_TYPEERASE_PAGES,
    .Page = page_num,
    .NbPages = num_pages,
  };
  
  HAL_FLASHEx_Erase(&EraseInit, &PageError);
  
  LOG_DEBUG_APP("OTP: all images erased.\n");
}

void OTS_APP_GetCurrentObjName(char **name_p)
{
  snprintf(OTS_APP_Context.curr_obj_name, MAX_OBJ_NAME_LENGTH, "Image %d", OTS_APP_Context.curr_obj_idx);
  *name_p = OTS_APP_Context.curr_obj_name;  
}

/*uuid_type: 0 for 16-bit UUIDs, 1 for 128-bit UUIDS */
void OTS_APP_GetCurrentObjType(uint8_t *uuid_type_p, uint8_t **uuid_p)
{
  *uuid_type_p = 0;
  *uuid_p = (uint8_t *)&obj_type;
}

void OTS_APP_GetCurrentObjSize(uint32_t *current_size_p, uint32_t *allocated_size_p)
{
  uint32_t *obj_header_p;
  
  obj_header_p = (uint32_t *)(OTS_FLASH_STORAGE_START_ADDRESS + OTS_APP_Context.curr_obj_idx * (OBJ_ALLOC_SIZE + OBJ_HEADER_SIZE));
  
  if(*obj_header_p == 0xFFFFFFFF)
  {
    /* Special value: empty */
    *current_size_p = 0;
  }
  else
  {
    *current_size_p = *obj_header_p;
  }
  
  *allocated_size_p = OBJ_ALLOC_SIZE;
}

void OTS_APP_GetObjInfo(uint8_t obj_index, OTS_ObjInfo_t *info)
{
  uint32_t *obj_header_p;
  
  if(obj_index > MAX_OBJ_IDX)
  {
    info->obj_p = NULL;
    info->size = 0;
    info->alloc_size = 0;
    
    return;
  }
  
  obj_header_p = (uint32_t *)(OTS_FLASH_STORAGE_START_ADDRESS + obj_index * (OBJ_ALLOC_SIZE + OBJ_HEADER_SIZE));
  
  info->obj_p = ((uint8_t *)obj_header_p) + OBJ_HEADER_SIZE;
  
  if(*obj_header_p == 0xFFFFFFFF)
  {
    /* Special value: empty */
    info->size = 0;
  }
  else
  {
    info->size = *obj_header_p;
  }
  
  info->alloc_size = OBJ_ALLOC_SIZE;
}

void OTS_APP_GetCurrentObjID(uint8_t **id_p)
{
  /* Add the obj_index to the base value 0x000000000100 to obtain the object ID. */  
  OTS_APP_Context.curr_obj_id[1] = 0x01;
  OTS_APP_Context.curr_obj_id[0] = OTS_APP_Context.curr_obj_idx;
  *id_p = OTS_APP_Context.curr_obj_id;
}

void OTS_APP_GetCurrentObjProp(uint32_t *prop_p)
{
  *prop_p = DEFAULT_OBJ_PROPERTIES;
}

uint8_t OTS_APP_OLCPExec(uint8_t op_code)
{
  uint8_t ret = OLCP_RESULT_SUCCESS;
  
  if(OTS_APP_Context.write_started)
  {
    return OLCP_RESULT_OPERATION_FAILED;
  }
    
  switch(op_code)
  {
  case OLCP_OPCODE_FIRST:
    
    OTS_APP_Context.curr_obj_idx = 0;    
    break;
    
  case OLCP_OPCODE_LAST:
    
    OTS_APP_Context.curr_obj_idx = MAX_OBJ_IDX;    
    break;
    
  case OLCP_OPCODE_PREVIOUS:
    
    if(OTS_APP_Context.curr_obj_idx > 0)
    {
      OTS_APP_Context.curr_obj_idx--;      
    }
    else
    {
      ret = OLCP_RESULT_OUT_OF_BOUNDS;
    }
    break;
    
  case OLCP_OPCODE_NEXT:
    
    if(OTS_APP_Context.curr_obj_idx < MAX_OBJ_IDX)
    {
      OTS_APP_Context.curr_obj_idx++;      
    }
    else
    {
      ret = OLCP_RESULT_OUT_OF_BOUNDS;
    }
    break;
    
  default:
    /* Other Op Codes are not supported */
    ret = OLCP_RESULT_NOT_SUPPORTED;
  }
  
  return ret;
}

uint8_t OTS_APP_OACPWrite(uint32_t offset, uint32_t length, uint8_t mode)
{
  uint32_t obj_address;
  uint32_t current_size;
  uint32_t allocated_size;
  uint32_t properties;
  
  LOG_DEBUG_APP("OTS_OACPWrite\n");
  
  OTS_APP_GetCurrentObjProp(&properties);
  if((properties & OBJ_PROP_WRITE) == 0)
  {
    return OACP_RESULT_PROC_NOT_PERMITTED;
  }
  
  if(OTS_APP_Context.l2cap_ch_id == 0xFF)
  {
    return OACP_RESULT_CHANNEL_UNAVAILABLE;
  }
  
  OTS_APP_GetCurrentObjSize(&current_size, &allocated_size);
  
  if(offset > current_size)
  {
    return OACP_RESULT_INVALID_PARAM;
  }
  if(offset + length > OBJ_ALLOC_SIZE)
  {
    return OACP_RESULT_INVALID_PARAM;
  }
  
  OTS_APP_Context.write_length = length;
  OTS_APP_Context.write_mode = mode;
  
  obj_address = OTS_FLASH_STORAGE_START_ADDRESS + OTS_APP_Context.curr_obj_idx * (OBJ_ALLOC_SIZE + OBJ_HEADER_SIZE);
  
  OTS_APP_Context.curr_address = obj_address + OBJ_HEADER_SIZE + offset;
  OTS_APP_Context.end_address = OTS_APP_Context.curr_address + length;
  
  /* This is a check in case MAX_OBJ_IDX is not set accordingly to the reserved space  */
  if(OTS_APP_Context.end_address > OTS_FLASH_STORAGE_END_ADDRESS)
  {
    return OACP_RESULT_OPERATION_FAILED;
  }
  
  if(mode & OACP_WRITE_MODE_TRUNCATE)
  {
    updateObjectSize(offset);
  }
  
  loadFlashPage(OTS_APP_Context.curr_address);
  
  OTS_APP_Context.write_started = true;
  
  UTIL_TIMER_Start(&OTS_APP_Context.timer);
  
  return OACP_RESULT_SUCCESS;  
}

void OTS_APP_L2CAPChannelOpened(uint16_t conn_handle, uint8_t ch_idx)
{
  LOG_DEBUG_APP("OTS_APP_L2CAPChannelOpened, channel index: %d\n", ch_idx);
  
  OTS_APP_Context.conn_handle = conn_handle;
  OTS_APP_Context.l2cap_ch_id = ch_idx;
}

//TODO: update object size with data written so far.
void OTS_APP_L2CAPChannelClosed(void)
{
  LOG_DEBUG_APP("OTS_APP_L2CAPChannelClosed\n");
  
  OTS_APP_Context.l2cap_ch_id = 0xFF;
  OTS_APP_Context.write_started = false;
  
  UTIL_TIMER_Stop(&OTS_APP_Context.timer);
  
  if(OTS_APP_Context.flash_buff_valid == true)
  {
    /* Buffer still has some data to be written */
    writeFlashPage(OTS_APP_Context.curr_address);
    
    OTS_APP_Context.flash_buff_valid = false;
  }
}

void OTS_APP_L2CAPDataReceived(uint16_t sdu_length, uint8_t *sdu_data)
{
  uint32_t page_start = BEGIN_OF_PAGE(OTS_APP_Context.curr_address);
  uint32_t page_end = page_start + FLASH_PAGE_SIZE;
  uint32_t page_offset = OTS_APP_Context.curr_address - page_start;
  uint32_t page_size = sizeof(OTS_APP_Context.flash_buff);
  
  if(OTS_APP_Context.write_started == false)
  {
    return;
  }
  
  UTIL_TIMER_Stop(&OTS_APP_Context.timer);
  UTIL_TIMER_Start(&OTS_APP_Context.timer);
  
  LOG_DEBUG_APP("OTP: Received SDU (length: %d bytes)\n", sdu_length);
  
  if(OTS_APP_Context.curr_address + sdu_length > OTS_APP_Context.end_address)
  {
    /* Attempt to write outside requested space. It should not happen. 
      Limit to requested size. */
    sdu_length = OTS_APP_Context.end_address - OTS_APP_Context.curr_address;
  }
  
  if(page_offset + sdu_length <= page_size)
  {
    OTS_APP_Context.flash_buff_valid = true;
    memcpy(&OTS_APP_Context.flash_buff.b8[page_offset], sdu_data, sdu_length);
    
    OTS_APP_Context.curr_address += sdu_length;
    
    if(OTS_APP_Context.curr_address == page_end &&
       OTS_APP_Context.curr_address != OTS_APP_Context.end_address) /* If OTS_APP_Context.curr_address == OTS_APP_Context.end_address, page is written later. */
    {
      /* End of flash page reached */
      writeFlashPage(page_start);
      OTS_APP_Context.flash_buff_valid = false;
    }    
  }
  else
  {
    /* Data is across two pages. */
    uint32_t first_part_data_length = page_size-page_offset;
    
    OTS_APP_Context.flash_buff_valid = true;
    
    memcpy(&OTS_APP_Context.flash_buff.b8[page_offset], sdu_data, first_part_data_length);
    
    writeFlashPage(page_start);
    
    loadFlashPage(page_end);
    
    memcpy(&OTS_APP_Context.flash_buff.b8[0], sdu_data + first_part_data_length, sdu_length - first_part_data_length);
    
    OTS_APP_Context.curr_address += sdu_length;
  }
  
  if(OTS_APP_Context.curr_address == OTS_APP_Context.end_address)
  {
    LOG_INFO_APP("OTP: Object received\n");
    UTIL_TIMER_Stop(&OTS_APP_Context.timer);
    OTS_APP_Context.write_started = false;
    writeFlashPage(page_start);
    OTS_APP_Context.flash_buff_valid = false;
    
    updateObjectSize(OTS_APP_Context.write_length);
  }
  else if((OTS_APP_Context.curr_address & PAGE_OFFSET_MASK) == 0)
  {
    /* Address is now the beginning of a new page. */
    loadFlashPage(OTS_APP_Context.curr_address);
  }
}

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void OTS_Oacp_SendIndication(void) /* Property Indication */
{
  OTS_APP_SendInformation_t indication_on_off = Oacp_INDICATION_OFF;
  OTS_Data_t ots_indication_data;

  ots_indication_data.p_Payload = (uint8_t*)a_OTS_UpdateCharData;
  ots_indication_data.Length = 0;

  /* USER CODE BEGIN Service1Char7_IS_1 */

  /* USER CODE END Service1Char7_IS_1 */

  if (indication_on_off != Oacp_INDICATION_OFF)
  {
    OTS_UpdateValue(OTS_OACP, &ots_indication_data);
  }

  /* USER CODE BEGIN Service1Char7_IS_Last */

  /* USER CODE END Service1Char7_IS_Last */

  return;
}

__USED void OTS_Olcp_SendIndication(void) /* Property Indication */
{
  OTS_APP_SendInformation_t indication_on_off = Olcp_INDICATION_OFF;
  OTS_Data_t ots_indication_data;

  ots_indication_data.p_Payload = (uint8_t*)a_OTS_UpdateCharData;
  ots_indication_data.Length = 0;

  /* USER CODE BEGIN Service1Char8_IS_1 */

  /* USER CODE END Service1Char8_IS_1 */

  if (indication_on_off != Olcp_INDICATION_OFF)
  {
    OTS_UpdateValue(OTS_OLCP, &ots_indication_data);
  }

  /* USER CODE BEGIN Service1Char8_IS_Last */

  /* USER CODE END Service1Char8_IS_Last */

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

/* A dedicated sector can be used to avoid too many writes. However, the
   size only changes if truncation is active, which should not happen since images
   should have fixed size in our case. */
static void updateObjectSize(uint32_t size)
{
  uint32_t obj_address;
  uint32_t current_size;
  uint32_t allocated_size;
  uint32_t page_start;
  uint32_t page_offset;
  
  OTS_APP_GetCurrentObjSize(&current_size, &allocated_size);
  
  if((size < current_size) && !(OTS_APP_Context.write_mode & OACP_WRITE_MODE_TRUNCATE))
  {
    /* Do not change size if it is less than current and truncation is not enabled.  */
    return;
  }
  
  if(current_size != size)
  {
    LOG_INFO_APP("OTP: Update object size\n");
    
    obj_address = OTS_FLASH_STORAGE_START_ADDRESS + OTS_APP_Context.curr_obj_idx * (OBJ_ALLOC_SIZE + OBJ_HEADER_SIZE);
    page_start = BEGIN_OF_PAGE(obj_address);
    page_offset = obj_address - page_start;  
    loadFlashPage(obj_address);
    memcpy(&OTS_APP_Context.flash_buff.b8[page_offset], &size, 4);  
    writeFlashPage(obj_address);
  }
}

static void loadFlashPage(uint32_t address)
{
  uint32_t *flash_p;
  
  flash_p = (uint32_t *)BEGIN_OF_PAGE(address);
  
  /* Load Flash content in RAM. */
  /* TBR: use memcpy instead? */
  for(uint32_t i = 0; i < FLASH_PAGE_SIZE/4; i ++)
  {
    OTS_APP_Context.flash_buff.b32[i] = flash_p[i];
  }    
}

static void writeFlashPage(uint32_t address)
{
  uint32_t page_address = BEGIN_OF_PAGE(address);
  uint32_t page_num = (page_address - FLASH_BASE) / FLASH_PAGE_SIZE;
  FM_Cmd_Status_t status;
  
  LOG_DEBUG_APP("OTP: Erasing page...\n");
  
  do {
    
    status = FM_Erase(page_num, 1, &FlashCallbackNode);
    
    if(status == FM_ERROR)
    {
      LOG_ERROR_APP("FM_Erase: error\n");
      return;
    }
    
    UTIL_SEQ_WaitEvt ( 1 << CFG_EVENT_FM_CALLBACK_EVT_RSP_ID);
    
    UTIL_SEQ_ClrEvt ( 1 << CFG_EVENT_FM_CALLBACK_EVT_RSP_ID);
    
  }while(FlashStatus == FM_OPERATION_AVAILABLE);
  
  LOG_DEBUG_APP("Erase complete\n");
  
  LOG_DEBUG_APP("OTP: Writing Page...\n");
  
  do {
    
    status = FM_Write (OTS_APP_Context.flash_buff.b32, (uint32_t *)page_address, FLASH_PAGE_SIZE / 4,
                       &FlashCallbackNode);
    
    if(status == FM_ERROR)
    {
      LOG_ERROR_APP("FM_Write: error\n");
      return;
    }
    
    UTIL_SEQ_WaitEvt ( 1 << CFG_EVENT_FM_CALLBACK_EVT_RSP_ID);
    
    UTIL_SEQ_ClrEvt ( 1 << CFG_EVENT_FM_CALLBACK_EVT_RSP_ID);
    
  }while(FlashStatus == FM_OPERATION_AVAILABLE); 
  
  LOG_INFO_APP("OTP: Flash Page Written\n");
  
}

static void ObjTransferTimeout(void *arg)
{  
  OTS_APP_Context.timer_expired = true;
  OTS_APP_ProcessRequest();
}

static void OTS_APP_ProcessRequest(void)
{
  UTIL_SEQ_SetTask(1<<CFG_TASK_OTS_PROCESS, CFG_SEQ_PRIO_0);
}

static void OTS_APP_Process(void)
{
  if(OTS_APP_Context.timer_expired)
  {
    LOG_ERROR_APP("Timeout\n");
    aci_l2cap_coc_disconnect(OTS_APP_Context.l2cap_ch_id);    
    OTS_APP_Context.timer_expired = false;
  }  
}

static void FlashCallback(FM_FlashOp_Status_t Status)
{
    /* Update status */ 
  FlashStatus = Status;

  /* Set event on Process request call */
  UTIL_SEQ_SetEvt ( 1 << CFG_EVENT_FM_CALLBACK_EVT_RSP_ID);
}

/* USER CODE END FD_LOCAL_FUNCTIONS */
