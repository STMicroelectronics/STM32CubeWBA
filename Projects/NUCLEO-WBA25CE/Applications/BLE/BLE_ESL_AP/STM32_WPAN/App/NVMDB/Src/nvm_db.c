/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    nvm_db.c
  * @author  GPM WBL Application Team
  * @brief   This file provides functions to implement sequential databases.
  *
  *  This file module uses Flash Manager module for Flash operation and STM32
  *  Sequencer. All the NVMDB operations which require a write or erase of the
  * Flash need to be called inside a task.
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

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "nvm_db.h"
#include "nvm_db_conf.h"
#include "flash_manager.h"
#include "log_module.h"
#include "stm32_seq.h"

#include "flash_driver.h"

/** @defgroup NVM_Manager  NVM Manager
 * @{
 */

/** @defgroup NVM_Manager_TypesDefinitions Private Type Definitions
 * @{
 */

#undef FLASH_DOUBLEWORD_SUPPORT

typedef struct
{
  uint8_t valid_flag;
  uint8_t record_id;
  uint16_t padding[6];
  NVMDB_RecordSizeType length;
} NVMDB_RecordHeaderType;

#define RECORD_HEADER_SIZE  sizeof(NVMDB_RecordHeaderType)

typedef struct
{
  NVMDB_RecordHeaderType header;
  uint8_t data[];
} NVMDB_RecordType, *NVMDB_RecordType_ptr;

typedef struct
{
  uint32_t start_address;
  uint32_t end_address;
  uint16_t valid_records;
  uint16_t invalid_records;
  uint16_t free_space;  // Free space at the end of last record. It is a real free space, not virtual. After a clean, the free space may increase. It takes also into account all the records in cache.
  bool locked;
  uint16_t clean_threshold;
} NVMDB_info;

typedef struct
{
  uint8_t move_to_next_record;
  NVMDB_RecordSizeType record_offset;
  NVMDB_RecordSizeType record_length;
}ReadStateType;

// Generic structure
typedef struct
{
  uint16_t length;
  uint8_t op;
  NVMDB_IdType id;
}CacheOperationType;

typedef struct
{
  uint16_t length;
  uint8_t op;
  NVMDB_IdType id;
}CacheNoOperationType;

typedef struct
{
  uint16_t length;
  uint8_t op;
  NVMDB_IdType id;
  uint8_t record_type;
}CacheWriteOperationType;

typedef struct
{
  uint16_t length;
  uint8_t op;
  NVMDB_IdType id;
  uint32_t address;
}CacheDeleteOperationType;

typedef struct
{
  uint16_t length;
  uint8_t op;
  NVMDB_HandleType handle;  // NVMDB_IdType id is first field of NVMDB_HandleType structure
  uint32_t flash_address;
  ReadStateType read_state;
}CacheCleanLargeOperationType;

typedef struct
{
  uint16_t length;
  uint8_t op;
  NVMDB_IdType id;
  uint8_t page_num_start;
  uint8_t num_pages;
}CachePageEraseOperationType;

typedef struct
{
  uint16_t length;
  uint8_t op;
  NVMDB_IdType id;
  const NVMDB_SmallDBContainerType *smallDBContainer_p;
}CacheSmallDBEraseOperationType;

/**
 * @}
 */

/** @defgroup NVM_Manager_Private_Defines Private Defines
 * @{
 */
#define DEBUG_GPIO_HIGH()
#define DEBUG_GPIO_LOW()

#define NO_RECORD           0xFF
#define VALID_RECORD        0xFE
#define INVALID_RECORD      0x00

/* If 1 the buffer used to temporarily store data is static, otherwise it is allocated in CSTACK. */
#define NVMDB_STATIC_BUFF       1

#define SMALL_DB 1
#define LARGE_DB 2

#ifdef DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define ATOMIC_SECTION_BEGIN() uint32_t uwPRIMASK_Bit = __get_PRIMASK(); \
                                __disable_irq(); \
/* Must be called in the same or in a lower scope of ATOMIC_SECTION_BEGIN */
#define ATOMIC_SECTION_END() __set_PRIMASK(uwPRIMASK_Bit)

/**
 * @}
 */

/** @defgroup NVM_Manager_Private_Macros Private Macros
 * @{
 */

#define PAGE_OFFSET_MASK    (PAGE_SIZE - 1)

#define MIN_RECORD_SIZE     (RECORD_HEADER_SIZE + 1)
#define MAX_RECORD_SIZE     ((NVMDB_RecordSizeType)(-1))

#ifndef MIN
#define MIN(a, b)            (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b)            (((a) > (b)) ? (a) : (b))
#endif

#define ROUND(a, b)           ((((a) - 1) | (b - 1)) + 1)  // Round 'a' to next multiple of 'b', being 'b' a power of 2.
#define ROUND8(a)             ROUND(a, 8) // Round to next multiple of 8
#define ROUND16(a)            ROUND(a, 16) // Round to next multiple of 16
#define ROUNDPAGE(a)          ROUND(a, PAGE_SIZE) // Round to next multiple of page size
#define BEGIN_OF_PAGE(a)      ((a) & (~PAGE_OFFSET_MASK))
                                  
#ifdef FLASH_DOUBLEWORD_SUPPORT
#define ROUND_ADDRESS(a)      ROUND8(a)
#define ROUND_SIZE(a)         ROUND8(a)
#else
#define ROUND_ADDRESS(a)      ROUND16(a)
#define ROUND_SIZE(a)         ROUND16(a)
#endif
                                  

/**
 * @}
 */

/** @defgroup NVM_Manager_Private_Variables Private Variables
 * @{
 */
#if NVMDB_STATIC_BUFF
ALIGN(4) static uint8_t NVM_buffer[PAGE_SIZE];
#endif
static NVMDB_info DBInfo[NUM_DB];

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

static FM_FlashOp_Status_t FlashStatus;

/**
 * @}
 */

/** @defgroup NVM_Manager_External_Variables External Variables
 * @{
 */

extern const NVMDB_SmallDBContainerType NVM_SMALL_DB_STATIC_INFO[];
extern const NVMDB_StaticInfoType NVM_LARGE_DB_STATIC_INFO[];

/**
 * @}
 */

/** @defgroup NVM_Manager_Private_FunctionPrototypes Private Function Prototypes
 * @{
 */

#if AUTO_CLEAN
static int8_t NVMDB_CleanCheck(void);
#endif

/**
 * @}
 */

/** @defgroup NVM_Manager_Private_Functions Private Functions
 * @{
 */

static void FlashCallback(FM_FlashOp_Status_t Status)
{
    /* Update status */ 
  FlashStatus = Status;

  /* Set event on Process request call */
  UTIL_SEQ_SetEvt ( 1 << CFG_EVENT_FM_CALLBACK_EVT_RSP_ID);
}

static NVMDB_status_t NVMDB_get_info(NVMDB_info *info)
{
  uint32_t address = info->start_address;
  NVMDB_RecordType_ptr record_p;

  info->valid_records = 0;
  info->invalid_records = 0;
  info->free_space = 0;
  info->locked = false;

  while(1)
  {

    record_p = (NVMDB_RecordType_ptr)address;

    if(record_p->header.valid_flag == NO_RECORD)
    {
      info->free_space = info->end_address - address - RECORD_HEADER_SIZE;
      return NVMDB_STATUS_OK;
    }
    else if(record_p->header.valid_flag == VALID_RECORD)
    {
      info->valid_records++;
    }
    else if(record_p->header.valid_flag == INVALID_RECORD)
    {
      info->invalid_records++;
    }
    else
    {
      // Wrong flag
      return NVMDB_STATUS_CORRUPTED_DB;
    }

    address += ROUND_ADDRESS(record_p->header.length + RECORD_HEADER_SIZE);

    if(address + MIN_RECORD_SIZE >= info->end_address)  // End of DB reached
    {
      return NVMDB_STATUS_OK;
    }
  }
}

/* Current_record_length is used to read the next record when the current record
   is no more present (because, for example, a clean operation has canceled it). */
static NVMDB_status_t NextRecordNoLock(NVMDB_HandleType *handle_p, uint8_t type, uint8_t **data_p, NVMDB_RecordSizeType *data_len, NVMDB_RecordSizeType current_record_length, uint8_t *record_type)
{
  NVMDB_RecordHeaderType header;
  NVMDB_RecordType_ptr record_p;

  if(current_record_length)
  {
    header.length = current_record_length;
    header.valid_flag = VALID_RECORD;
    record_p = (NVMDB_RecordType_ptr) & header;
  }
  else
  {
    record_p = (NVMDB_RecordType_ptr)handle_p->address;
  }

  if(handle_p->address >= handle_p->end_address || record_p->header.valid_flag == NO_RECORD)
  {
    return NVMDB_STATUS_END_OF_DB;
  }

  // If we are not at the end we can move to next record.

  while(1)
  {

    // If it is the first read, the pointer already points to first record.
    // If it is not the first read, advance the pointer to the next record (word aligned).
    if(handle_p->first_read)
    {
      handle_p->first_read = false;
    }
    else
    {
      handle_p->address += ROUND_ADDRESS(record_p->header.length + RECORD_HEADER_SIZE);
      record_p = (NVMDB_RecordType_ptr)handle_p->address;

      if(handle_p->address >= handle_p->end_address || record_p->header.valid_flag == NO_RECORD)
      {
        return NVMDB_STATUS_END_OF_DB;
      }
    }

    if(record_p->header.valid_flag == INVALID_RECORD)
    {
      // If record is invalidated, address is updated in next cycle.
      continue;
    }

    if(record_p->header.valid_flag != VALID_RECORD)
    {
      return NVMDB_STATUS_CORRUPTED_DB;
    }

    if(type == ALL_TYPES || record_p->header.record_id == type)
    {
      *data_p = (uint8_t *)handle_p->address + RECORD_HEADER_SIZE;
      *data_len = record_p->header.length;

      if(record_type != NULL)
      {
        *record_type = record_p->header.record_id;
      }

      //memcpy(data_p, (uint8_t*)handle_p->address + RECORD_HEADER_SIZE + offset, MIN(record_p->header.length - offset, max_size));
      //*size_p = record_p->header.length;

      return NVMDB_STATUS_OK;
    }
  }
}

static NVMDB_status_t ReadNextRecordNoLock(NVMDB_HandleType *handle_p, uint8_t record_type, NVMDB_RecordSizeType offset, uint8_t *data_p, NVMDB_RecordSizeType max_size, NVMDB_RecordSizeType *size_p, NVMDB_RecordSizeType current_record_length, uint8_t *record_type_out)
{
  NVMDB_status_t status;

  uint8_t *data;
  NVMDB_RecordSizeType data_len;

  *size_p = 0;

  status = NextRecordNoLock(handle_p, record_type, &data, &data_len, current_record_length, record_type_out);

  if(status != NVMDB_STATUS_OK)
  {
    return status;
  }

  if(offset >= data_len)
  {
    return NVMDB_STATUS_INVALID_OFFSET;
  }

  memcpy(data_p, data + offset, MIN(data_len - offset, max_size));
  *size_p = data_len;

  return NVMDB_STATUS_OK;
}

/* flash_address must be double word aligned. */
static void write_data(uint32_t flash_address, uint16_t data_length, const uint32_t *data)
{
  FM_Cmd_Status_t status;
  
  if(data_length == 0)
    return;

  //TODO: write FF at the end if data_length is not multiple of 8 bytes.
  
  data_length = ROUND8(data_length);
  
  do {
    
    status = FM_Write ((uint32_t *)data, (uint32_t *)flash_address, data_length / 4,
                       &FlashCallbackNode);
    
    if(status == FM_ERROR)
    {
      LOG_ERROR_APP("FM_Write: error\n");
      return;
    }
    
    UTIL_SEQ_WaitEvt ( 1 << CFG_EVENT_FM_CALLBACK_EVT_RSP_ID);
    
    UTIL_SEQ_ClrEvt ( 1 << CFG_EVENT_FM_CALLBACK_EVT_RSP_ID);
    
  }while(FlashStatus == FM_OPERATION_AVAILABLE);
}

static void write_data_unaligned(uint32_t flash_address, uint16_t data_length, const void *data)
{
  FM_Cmd_Status_t status;
  uint32_t * datasrc_p;
  
  if(data_length == 0)
    return;
  
  if((uint32_t)data & 0x03)
  {
    /* Not aligned */
    memcpy(NVM_buffer, data, data_length);
    datasrc_p = (uint32_t *)NVM_buffer;
  }
  else
  {
    /* data already aligned */
    datasrc_p = (uint32_t *)data;
  }

  //TODO: write FF at the end if data_length is not multiple of 8 bytes.
  
  data_length = ROUND_SIZE(data_length);
  
  do {
    
    status = FM_Write (datasrc_p, (uint32_t *)flash_address, data_length / 4,
                       &FlashCallbackNode);
    
    if(status == FM_ERROR)
    {
      LOG_ERROR_APP("FM_Write: error\n");
      return;
    }
    
    UTIL_SEQ_WaitEvt ( 1 << CFG_EVENT_FM_CALLBACK_EVT_RSP_ID);
    
    UTIL_SEQ_ClrEvt ( 1 << CFG_EVENT_FM_CALLBACK_EVT_RSP_ID);
    
  }while(FlashStatus == FM_OPERATION_AVAILABLE);
}

static NVMDB_status_t WriteRecord(uint32_t flash_address, uint8_t record_id, uint16_t data1_length, const void *data1, uint16_t data2_length, const void *data2)
{
  ALIGN(4) NVMDB_RecordHeaderType header;

  data1_length = ROUND_ADDRESS(data1_length);

  header.valid_flag = VALID_RECORD;
  header.record_id = record_id;
  header.length = data1_length + data2_length;
  memset(&header.padding, 0xFF, sizeof(header.padding));

  DEBUG_GPIO_HIGH();
  
  write_data(flash_address, sizeof(header), (uint32_t *)&header);
  write_data_unaligned(flash_address + sizeof(header), data1_length, data1);
  write_data_unaligned(flash_address + sizeof(header) + data1_length, data2_length, data2);

  DEBUG_GPIO_LOW();

  return NVMDB_STATUS_OK;
}

int NVMDB_CompareCurrentRecord(NVMDB_HandleType *handle_p, NVMDB_RecordSizeType offset, const uint8_t *data_p, NVMDB_RecordSizeType size)
{
  NVMDB_RecordType_ptr record_p;

  record_p = (NVMDB_RecordType_ptr)handle_p->address;

  if(handle_p->address >= handle_p->end_address || record_p->header.valid_flag == NO_RECORD)
  {
    return NVMDB_STATUS_END_OF_DB;
  }

  if(handle_p->first_read || record_p->header.valid_flag != VALID_RECORD)
  {
    return NVMDB_STATUS_INVALID_RECORD;
  }

  if(offset >= record_p->header.length)
  {
    return NVMDB_STATUS_INVALID_OFFSET;
  }

  handle_p->first_read = false;

  if(memcmp(data_p, (uint8_t *)handle_p->address + RECORD_HEADER_SIZE + offset, size) == 0)
  {
    return NVMDB_STATUS_OK;
  }
  else
  {
    return -1;
  }
}

static NVMDB_status_t ReadCurrentRecordNoLock(NVMDB_HandleType *handle_p, NVMDB_RecordSizeType offset, uint8_t *data_p, NVMDB_RecordSizeType max_size, NVMDB_RecordSizeType *size_p, NVMDB_RecordSizeType current_record_length)
{
  NVMDB_RecordHeaderType header;
  NVMDB_RecordType_ptr record_p;

  *size_p = 0;

  if(current_record_length)
  {
    header.length = current_record_length;
    header.valid_flag = VALID_RECORD;
    record_p = (NVMDB_RecordType_ptr) & header;
  }
  else
  {
    record_p = (NVMDB_RecordType_ptr)handle_p->address;
  }

  if(handle_p->address >= handle_p->end_address || record_p->header.valid_flag == NO_RECORD)
  {
    return NVMDB_STATUS_END_OF_DB;
  }

  if(handle_p->first_read || record_p->header.valid_flag != VALID_RECORD)
  {
    return NVMDB_STATUS_INVALID_RECORD;
  }

  if(offset >= record_p->header.length)
  {
    return NVMDB_STATUS_INVALID_OFFSET;
  }

  handle_p->first_read = false;

  memcpy(data_p, (uint8_t *)handle_p->address + RECORD_HEADER_SIZE + offset, MIN(record_p->header.length - offset, max_size));
  *size_p = record_p->header.length;

  return NVMDB_STATUS_OK;
}

static NVMDB_status_t InvalidateRecord(uint32_t address)
{
  uint32_t words[2] = {0x00000000, 0x00000000};

  DEBUG_GPIO_HIGH();
  FD_SetStatus (FD_FLASHACCESS_RFTS_BYPASS, LL_FLASH_ENABLE);
  write_data(address, sizeof(words), words);
  FD_SetStatus (FD_FLASHACCESS_RFTS_BYPASS, LL_FLASH_DISABLE);
  DEBUG_GPIO_LOW();

  return NVMDB_STATUS_OK;
}

static NVMDB_status_t NVMDB_DeleteRecordNoCache(const NVMDB_HandleType *handle_p)
{
  NVMDB_RecordType *record_p;
  NVMDB_status_t status;

  record_p = (NVMDB_RecordType_ptr)handle_p->address;

  if(handle_p->address >= handle_p->end_address || record_p->header.valid_flag == NO_RECORD)
  {
    return NVMDB_STATUS_END_OF_DB;
  }
  if(record_p->header.valid_flag == VALID_RECORD)
  {
    status = InvalidateRecord((uint32_t)record_p);
    if(status)
    {
      return status;
    }

    DBInfo[handle_p->id].valid_records--;
    DBInfo[handle_p->id].invalid_records++;
    
    NVMDB_ProcessRequest();

    return NVMDB_STATUS_OK;
  }
  else
  {
    return NVMDB_STATUS_INVALID_RECORD;
  }
}

static void ErasePage(uint32_t address, uint8_t num_pages)
{
  FM_Cmd_Status_t status;
  int page_num = (address - FLASH_BASE) / PAGE_SIZE;
  
  DEBUG_GPIO_HIGH();
  
  do {
    
    status = FM_Erase(page_num, 1, &FlashCallbackNode);
    
    if(status == FM_ERROR)
    {
      LOG_ERROR_APP("FM_Write: error\n");
      return;
    }
    
    UTIL_SEQ_WaitEvt ( 1 << CFG_EVENT_FM_CALLBACK_EVT_RSP_ID);
    
    UTIL_SEQ_ClrEvt ( 1 << CFG_EVENT_FM_CALLBACK_EVT_RSP_ID);
    
  }while(FlashStatus == FM_OPERATION_AVAILABLE);
  
  DEBUG_GPIO_LOW();
}

/* Size of data must be multiple of 4. This function also erases the page if needed. */
static void WriteBufferToFlash(uint32_t address, uint32_t *data, uint32_t size)
{
  /* Check if we are writing the same data in entire pages.
     If size is less than a page size, we need to erase the page to clean it. */
  if((size % PAGE_SIZE) == 0 && memcmp((uint8_t *)address, data, size) == 0)
  {
    return;
  }

  ErasePage(address, ROUNDPAGE(size) / PAGE_SIZE);

  DEBUG_GPIO_HIGH();
  
  write_data(address, size, data);
  
  DEBUG_GPIO_LOW();
}

static void InitReadState(ReadStateType *state_p)
{
  state_p->move_to_next_record = true;
  state_p->record_offset = 0;
  state_p->record_length = 0;
}

/* Call InitReadState() to initialize read_state before passing it to the function to start loading data from the database.
   This function returns NVMDB_STATUS_OK if other calls must be done to the same function to continue reading the database.
   Pass the same read_state variable to the function if calling it again to continue reading from the same database. */

static NVMDB_status_t LoadDBToRAM(NVMDB_HandleType *handle_p, uint8_t *buff, uint16_t buff_size, uint16_t *num_read_bytes_p, ReadStateType *read_state)
{
  NVMDB_status_t status;
  uint16_t free_buffer_size;
  NVMDB_RecordSizeType record_size, remaining_record_bytes;
  NVMDB_RecordHeaderType header;
  uint16_t index = 0;

  memset(buff, 0xFF, buff_size);

  header.valid_flag = VALID_RECORD;

  while(1)
  {

    free_buffer_size = buff_size - index - RECORD_HEADER_SIZE;

    // Read the record
    if(read_state->move_to_next_record)
    {
      /* Passing read_state->record_length to the function because we do not want
         the function to retrieve the info from the header of the current record,
         since it may not be anymore in the same position in flash. */
      status = ReadNextRecordNoLock(handle_p, ALL_TYPES, 0, buff + index + RECORD_HEADER_SIZE, free_buffer_size, &record_size, read_state->record_length, &header.record_id);
      read_state->record_length = record_size;

      if(status != NVMDB_STATUS_OK)
      {
        *num_read_bytes_p = index;
        return status;
      }

      // Prepare the header and copy it to buffer
      header.length = record_size;
      memcpy(buff + index, &header, RECORD_HEADER_SIZE);
      index += ROUND_ADDRESS(header.length + RECORD_HEADER_SIZE);
    }
    else
    {

      status = ReadCurrentRecordNoLock(handle_p, read_state->record_offset, buff + index, free_buffer_size, &record_size, read_state->record_length); // It will not read record in cache because this operation is not allowed if there are any.

      if(status != NVMDB_STATUS_OK)
      {
        // This should not happen
        return status;
      }
      index = ROUND_ADDRESS(record_size - read_state->record_offset);
    }

    // The number of remaining bytes for the current record that were to be read before calling last NVMDB_ReadNextRecord().
    // Rounded to the next multiple to find out if buffer can now be written inside the page or not.
    remaining_record_bytes = ROUND_ADDRESS(record_size - read_state->record_offset);

    if(remaining_record_bytes > free_buffer_size)
    {
      // Not able to read the entire record.
      read_state->move_to_next_record = false;
      read_state->record_offset += free_buffer_size;
      *num_read_bytes_p = buff_size;
      return NVMDB_STATUS_OK;
    }
    else if(remaining_record_bytes == free_buffer_size)
    {
      read_state->move_to_next_record = true;
      read_state->record_offset = 0;
      *num_read_bytes_p = buff_size;
      return NVMDB_STATUS_OK;
    }
    else if(remaining_record_bytes < free_buffer_size)
    {
      read_state->move_to_next_record = true;
      read_state->record_offset = 0;
    }
  }
}
// No inline to avoid allocating NVM_buffer multiple times at the same time.
__NOINLINE static NVMDB_status_t CleanLargeDB(NVMDB_IdType NVMDB_id)
{
  NVMDB_status_t status;
  uint32_t flash_write_address;
  uint16_t num_read_bytes;
  NVMDB_HandleType handle;
  ReadStateType state;

  if(!DBInfo[NVMDB_id].invalid_records)
  {
    return NVMDB_STATUS_OK;
  }

  status = NVMDB_HandleInit(NVMDB_id, &handle);
  flash_write_address = (uint32_t)handle.address;

  InitReadState(&state);

  while(1)
  {
#if !NVMDB_STATIC_BUFF
    uint8_t NVM_buffer[PAGE_SIZE];
#endif

    status = LoadDBToRAM(&handle, NVM_buffer, sizeof(NVM_buffer), &num_read_bytes, &state);
    if(status != NVMDB_STATUS_END_OF_DB && status != NVMDB_STATUS_OK) // No other error codes are expected. This should not happen.
    {
      return status;
    }

    if(num_read_bytes == sizeof(NVM_buffer) || status != NVMDB_STATUS_OK)
    {
      if(num_read_bytes == 0)
      {
        // This can happen if the end of the database is reached at the beginning of the LoadDBToRAM() function.
        break;
      }

      // Write buffer to flash
      WriteBufferToFlash(flash_write_address, (uint32_t *)NVM_buffer, num_read_bytes);
      flash_write_address += sizeof(NVM_buffer);
      if(status != NVMDB_STATUS_OK)
      {
        break;
      }
    }
  }
  // Erase remaining pages. A possible optimization could be to erase the page only if it is not already erased.
  uint8_t num_pages = (ROUNDPAGE(handle.end_address) - flash_write_address) / PAGE_SIZE;
  //uint8_t page_num_start = (flash_write_address - FLASH_BASE) / PAGE_SIZE;
  
  ErasePage(flash_write_address, num_pages);

  // Update free space.
  return NVMDB_get_info(&DBInfo[NVMDB_id]);
}

__NOINLINE static NVMDB_status_t CleanPage(const NVMDB_SmallDBContainerType *smallDBContainer_p)
{
  NVMDB_status_t status;
  NVMDB_status_t ret = NVMDB_STATUS_OK;
  uint8_t NVMDB_id;
  NVMDB_HandleType handle;
  uint32_t page_offset;
  uint16_t num_bytes;
  ReadStateType state;
  int i;
  uint8_t found = false;
#if !NVMDB_STATIC_BUFF
  uint8_t NVM_buffer[PAGE_SIZE];
#endif

  /* Check if there are invalids record in the page and if there are pending operations on the databases. */
  for(i = 0; i < smallDBContainer_p->num_db; i++)
  {
    NVMDB_id = smallDBContainer_p->dbs[i].id;
    if(DBInfo[NVMDB_id].invalid_records)
    {
      found = true;
    }
  }
  if(!found)
  {
    /* No invalid records found. */
    return NVMDB_STATUS_OK;
  }

  memset(NVM_buffer, 0xFF, sizeof(NVM_buffer));

  for(i = 0; i < smallDBContainer_p->num_db; i++)
  {
    NVMDB_id = smallDBContainer_p->dbs[i].id;
    page_offset = smallDBContainer_p->dbs[i].offset;

    status = NVMDB_HandleInit(NVMDB_id, &handle);
    if(status) // This should not happen
    {
      return NVMDB_STATUS_OK;
    }

    InitReadState(&state);
    // It cannot happen that the database does not fill inside the buffer. So we call LoadDBToRAM() just once for each db.
    LoadDBToRAM(&handle, NVM_buffer + page_offset, sizeof(NVM_buffer) - page_offset, &num_bytes, &state);
  }

  WriteBufferToFlash(smallDBContainer_p->page_address, (uint32_t *)NVM_buffer, sizeof(NVM_buffer));

  for(i = 0; i < smallDBContainer_p->num_db; i++)
  {
    NVMDB_id = smallDBContainer_p->dbs[i].id;
    // Update free space.
    status = NVMDB_get_info(&DBInfo[NVMDB_id]);
    if(status)
    {
      ret = status; // Return last error if any.
    }
  }

  return ret;
}

static uint8_t GetDBType(NVMDB_IdType NVMDB_id, const NVMDB_SmallDBContainerType **smallDBContainer)
{
  uint8_t id;

  /* Check if it is a large DB. */

  for(int i = 0; i < NUM_LARGE_DBS; i++)
  {

    id = NVM_LARGE_DB_STATIC_INFO[i].id;

    if(id == NVMDB_id)
    {
      return LARGE_DB;
    }
  }

  for(int i = 0; i < NUM_SMALL_DB_PAGES; i++)
  {

    for(int j = 0; j < NVM_SMALL_DB_STATIC_INFO[i].num_db; j++)
    {
      id = NVM_SMALL_DB_STATIC_INFO[i].dbs[j].id;

      if(id == NVMDB_id)
      {
        // DB found inside this small db container.
        *smallDBContainer = &NVM_SMALL_DB_STATIC_INFO[i];
        return SMALL_DB;
      }
    }
  }

  return 0;
}

__NOINLINE static NVMDB_status_t EraseSmallDB(NVMDB_IdType NVMDB_id, const NVMDB_SmallDBContainerType *smallDBContainer_p)
{
#if !NVMDB_STATIC_BUFF
  uint8_t NVM_buffer[PAGE_SIZE];
#endif

  memset(NVM_buffer, 0xFF, sizeof(NVM_buffer));

  // Search other databases in the same page
  for(int i = 0; i < smallDBContainer_p->num_db; i++)
  {
    NVMDB_HandleType handle;
    uint16_t num_read_bytes;
    ReadStateType state;
    NVMDB_IdType id = smallDBContainer_p->dbs[i].id;
    if(id != NVMDB_id)
    {
      // This is not the db to be erased. Save it.
      NVMDB_HandleInit(id, &handle);
      uint32_t offset = smallDBContainer_p->dbs[i].offset;
      InitReadState(&state);
      LoadDBToRAM(&handle, NVM_buffer + offset, sizeof(NVM_buffer) - offset, &num_read_bytes, &state);
    }
  }

  // Write buffer to flash
  WriteBufferToFlash(smallDBContainer_p->page_address, (uint32_t *)NVM_buffer, sizeof(NVM_buffer));

  return NVMDB_STATUS_OK;
}

static NVMDB_status_t NVMDB_AppendRecordNoCache(NVMDB_HandleType *handle_p, uint8_t record_id, uint16_t data1_length, const void *data1, uint16_t data2_length, const void *data2)
{
  NVMDB_RecordType *record_p;
  NVMDB_RecordSizeType available_size;
  NVMDB_status_t status;

  data1_length = ROUND_ADDRESS(data1_length);

  /* Check if there is space in the db. Not strictly needed. But this check is faster in case the db is full (instead of parsing every records).  */
//  if(length > DBInfo[handle_p->id].free_space){
//    if(DBInfo[handle_p->id].invalid_records)
//      return NVMDB_STATUS_CLEAN_NEEDED;
//    return NVMDB_STATUS_FULL_DB;
//  }

  /* Search for first available entry. */

  while(1)
  {

    if(handle_p->address + MIN_RECORD_SIZE >= handle_p->end_address)
    {
      /* End of DB reached. This should not happen since the free space has been checked before.
         Should we remove this code or keep it to be safer?  */
      if(DBInfo[handle_p->id].invalid_records)
      {
        return NVMDB_STATUS_CLEAN_NEEDED;
      }
      return NVMDB_STATUS_FULL_DB;
    }

    record_p = (NVMDB_RecordType_ptr)handle_p->address;

    if(record_p->header.valid_flag == NO_RECORD)
    {
      available_size = MIN(handle_p->end_address - handle_p->address, MAX_RECORD_SIZE);
      break;
    }
    else if(record_p->header.valid_flag != VALID_RECORD && record_p->header.valid_flag != INVALID_RECORD)
    {
      return NVMDB_STATUS_CORRUPTED_DB;
    }

    handle_p->address += ROUND_ADDRESS(record_p->header.length + RECORD_HEADER_SIZE);
  }

  if(available_size < data1_length + data2_length + RECORD_HEADER_SIZE)
  {
    /* End of DB reached. This should not happen since the free space has been checked before.
       Should we remove this code or keep it to be safer?  */
    if(DBInfo[handle_p->id].invalid_records)
    {
      return NVMDB_STATUS_CLEAN_NEEDED;
    }
    return NVMDB_STATUS_FULL_DB;
  }

  status = WriteRecord(handle_p->address, record_id, data1_length, data1, data2_length, data2);
  if(status)
  {
    return status;
  }

  DBInfo[handle_p->id].valid_records++;

  return NVMDB_STATUS_OK;
}

/**
 * @}
 */

/** @defgroup NVM_Manager_Public_Functions Public Functions
 * @{
 */

/**
 * @brief  Initialize the NVM Manager.
 *
 *         Function to be called before using the library.
 *
 * @param  None
 * @retval Indicates if the function executed successfully.
 */
NVMDB_status_t NVMDB_Init(void)
{
  // Checks DB consistency. Reads number of records.

  NVMDB_status_t status;
  uint32_t page_address, offset;
  uint16_t clean_threshold;
  uint8_t id;
  
  NVMDB_InitConf();

  /* Parse small DBs. */
  for(int i = 0; i < NUM_SMALL_DB_PAGES; i++)
  {

    page_address = NVM_SMALL_DB_STATIC_INFO[i].page_address;

    for(int j = 0; j < NVM_SMALL_DB_STATIC_INFO[i].num_db; j++)
    {
      id = NVM_SMALL_DB_STATIC_INFO[i].dbs[j].id;
      offset = NVM_SMALL_DB_STATIC_INFO[i].dbs[j].offset;
#if AUTO_CLEAN
      clean_threshold = NVM_SMALL_DB_STATIC_INFO[i].dbs[j].clean_threshold;
#endif
      if(DBInfo[id].start_address || id >= NUM_DB)
      {
        // ID already used.
        return NVMDB_STATUS_INVALID_ID;
      }
      DBInfo[id].start_address = page_address + offset;

      if(j == NVM_SMALL_DB_STATIC_INFO[i].num_db - 1)
      {
        // If it is the last db of the page, end address is the last address of the page
        DBInfo[id].end_address = page_address + PAGE_SIZE;
      }
      else
      {
        // If it is not the last db of the page, end address is the start address of the next db
        DBInfo[id].end_address = page_address + NVM_SMALL_DB_STATIC_INFO[i].dbs[j + 1].offset;
      }
      DBInfo[id].clean_threshold = clean_threshold;

      status = NVMDB_get_info(&DBInfo[id]);
      if(status)
      {
        return status;
      }
    }
  }

  /* Parse large DBs. */
  for(int i = 0; i < NUM_LARGE_DBS; i++)
  {

    id = NVM_LARGE_DB_STATIC_INFO[i].id;

    if(DBInfo[id].start_address || id >= NUM_DB)
    {
      // ID already used.
      return NVMDB_STATUS_INVALID_ID;
    }

    DBInfo[id].start_address = NVM_LARGE_DB_STATIC_INFO[i].address;
    DBInfo[id].end_address = NVM_LARGE_DB_STATIC_INFO[i].address + NVM_LARGE_DB_STATIC_INFO[i].size;
#if AUTO_CLEAN
    DBInfo[id].clean_threshold = NVM_LARGE_DB_STATIC_INFO[i].clean_threshold;
#endif

    status = NVMDB_get_info(&DBInfo[id]);
    if(status)
    {
      return status;
    }
  }
  
  /* Schedule an initial NVMDB_Tick() in case there is a DB to be cleaned. */
  NVMDB_ProcessRequest();

  return NVMDB_STATUS_OK;
}

/**
 * @brief      Initialize the handle that points to the given database.
 *
 *             NVMDB_HandleInit has to be called before using functions that
 *             need an handle.
 *
 * @param      NVMDB_id ID of the database. This is a number that uniquely identifies
 *             the database. The IDs are decided when declaring the database structure
 *             through NVM_SMALL_DB_STATIC_INFO and NVM_LARGE_DB_STATIC_INFO variables.
 * @param[out] handle_p Pointer to the handle to be initialized.
 * @retval Indicates if the function executed successfully.
 */
NVMDB_status_t NVMDB_HandleInit(NVMDB_IdType NVMDB_id, NVMDB_HandleType *handle_p)
{
  if(NVMDB_id >= NUM_DB)
  {
    return NVMDB_STATUS_INVALID_ID;
  }

  handle_p->address = DBInfo[NVMDB_id].start_address;
  handle_p->end_address = DBInfo[NVMDB_id].end_address;
  handle_p->first_read = true;
  handle_p->id = NVMDB_id;

  return NVMDB_STATUS_OK;
}

/**
 * @brief      Read data from the next record in the database.
 *
 *             Retrieve data from the next valid record in the database.
 *             Each time NVMDB_ReadNextRecord is called, the handle is firstly
 *             moved forward in order to point to the next valid record in the database.
 *             Record data is copied into the provided buffer.
 *
 * @param[in,out] handle_p Handle pointing to the database. It must have been previously
 *             initialized with NVMDB_HandleInit. After calling the function, the
 *             handle can be used again to read the next record.
 * @param      record_type Type of the record. This value is application-specific. Use
 *             ALL_TYPES to read every records, regardless of the type.
 * @param      data_offset Offset inside the record from which data has to be copied.
 *             Normally it is set to 0.
 * @param[out] data_p Array in which data will be copied.
 * @param      max_size Size of the buffer pointed by data_p.
 * @param[out] size_p Size of the read record.
 * @retval     Indicates if the function executed successfully.
 */
NVMDB_status_t NVMDB_ReadNextRecord(NVMDB_HandleType *handle_p, uint8_t record_type, NVMDB_RecordSizeType data_offset, uint8_t *data_p, NVMDB_RecordSizeType max_size, NVMDB_RecordSizeType *size_p)
{
  if(DBInfo[handle_p->id].locked)
  {
    return NVMDB_STATUS_LOCKED;
  }

  return ReadNextRecordNoLock(handle_p, record_type, data_offset, data_p, max_size, size_p, 0, NULL);
}

/**
 * @brief      Read data from the current record in the database.
 *
 *             Retrieve data from the current valid record in the database.
 *             Handle is not moved before retrieving the data.
 *
 * @param[in,out] handle_p Handle pointing to the database. It must have been previously
 *             initialized with NVMDB_HandleInit.
 * @param      data_offset Offset inside the record from which data has to be copied.
 *             Normally it is set to 0.
 * @param[out] data_p Array in which data will be copied.
 * @param      max_size Size of the buffer pointed by data_p.
 * @param[out] size_p Size of the read record.
 * @retval     Indicates if the function executed successfully.
 */
NVMDB_status_t NVMDB_ReadCurrentRecord(NVMDB_HandleType *handle_p, NVMDB_RecordSizeType data_offset, uint8_t *data_p, NVMDB_RecordSizeType max_size, NVMDB_RecordSizeType *size_p)
{
  if(DBInfo[handle_p->id].locked)
  {
    return NVMDB_STATUS_LOCKED;
  }

  return ReadCurrentRecordNoLock(handle_p, data_offset, data_p, max_size, size_p, 0);
}

/**
 * @brief      Find next record in the database that matches the given pattern.
 *
 *             NVMDB_FindNextRecord searches inside the database starting from the
 *             record pointed by the given handle. If the pattern is found starting
 *             from pattern_offset, the function returns NVMDB_STATUS_OK and the
 *             handle points to the found record. The value of the record can be
 *             retrieved by providing a buffer where data will be copied.
 *
 * @note The handle is moved while searching inside the database.
 *
 * @param[in,out] handle_p Handle pointing to the database. It must have been previously
 *             initialized with NVMDB_HandleInit. After calling the function, the
 *             handle can be used again to search the pattern, starting from the next record.
 * @param      record_type Type of the record to search for. This value is application-specific. Use
 *             ALL_TYPES to read every records, regardless of the type.
 * @param      pattern_offset Offset at which the pattern has to be located inside the record.
 * @param[in]  pattern_p The pattern that has to be found inside the database.
 * @param      pattern_length Length of the pattern.
 * @param      data_offset Offset inside the record from which data has to be copied.
 *             Normally it is set to 0.
 * @param      data_offset Length of the pattern.
 * @param[out] data_p Array in which data will be copied. If NULL, no data is copied.
 * @param      max_size Size of the buffer pointed by data_p.
 * @param[out] size_p Size of the read record.
 * @retval     Indicates if the function executed successfully.
 */
NVMDB_status_t NVMDB_FindNextRecord(NVMDB_HandleType *handle_p, uint8_t record_type, NVMDB_RecordSizeType pattern_offset, const uint8_t *pattern_p, NVMDB_RecordSizeType pattern_length, NVMDB_RecordSizeType data_offset, uint8_t *data_p, NVMDB_RecordSizeType max_size, NVMDB_RecordSizeType *size_p)
{
  NVMDB_status_t status;
  uint8_t *data;
  NVMDB_RecordSizeType record_len;

  if(DBInfo[handle_p->id].locked)
  {
    return NVMDB_STATUS_CACHE_OP_PENDING;
  }

  while(1)
  {

    status = NextRecordNoLock(handle_p, record_type, &data, &record_len, 0, NULL);

    if(status != NVMDB_STATUS_OK)
    {
      return status;
    }

    if(pattern_offset >= record_len || pattern_length > record_len - pattern_offset)
    {
      continue;
    }

    if(memcmp(data + pattern_offset, pattern_p, pattern_length) == 0)
    {
      // Record has been found
      if(data_p != NULL)
      {
        *size_p = record_len;
        if(data_offset >= record_len)
        {
          return NVMDB_STATUS_INVALID_OFFSET;
        }
        memcpy(data_p, data + data_offset, MIN(record_len - data_offset, max_size));
      }

      return NVMDB_STATUS_OK;
    }
  }
}

/**
 * @brief      Add a record in the database.
 *
 *             The record is added at the end of the database pointed by the given handle.
 * @note       If the handle points to the last record in the database, the append
 *             operation is faster.
 * @note       A write or erase operation on the Flash prevents access to the
 *             Flash for a certain amount of time, potentially delaying execution
 *             of the time-critical radio interrupt service routine. Thus, it is checked
 *             that enough time is present before the next scheduled radio activity
 *             will start. If there is no enough time, this operation is scheduled. A
 *             cache is used to temporarily store the record.
 *
 * @param[in,out] handle_p Handle pointing to the database. It must have been previously
 *             initialized with NVMDB_HandleInit. If the function returns NVMDB_STATUS_OK,
 *             the handle points to the added record.
 *             @note If the returned value is not NVMDB_STATUS_OK, the handle my points to
 *             any record in the database.
 * @param      record_type One byte that can be used by the application to identify the type
 *             of record.
 * @param      header_length Length of bytes in header. It must be a multiple of 4. Set it to 0
 *             if header is NULL.
 * @param[in]  header Data to be stored at the beginning of the record. It can be used by the
 *             application to add an header to the record. The number of bytes must be a
 *             multiple of 4. Set it to NULL if no header has to be added to the record.
 * @param      data_length Length of bytes in data.
 * @param[in]  data Data to be stored inside the record.
 * @retval     Indicates if the function executed successfully.
 */
NVMDB_status_t NVMDB_AppendRecord(NVMDB_HandleType *handle_p, uint8_t record_type, uint16_t header_length, const void *header, uint16_t data_length, const void *data)
{
  NVMDB_status_t status;

  status = NVMDB_AppendRecordNoCache(handle_p, record_type, header_length, header, data_length, data);

  if(status != NVMDB_STATUS_OK)
  {
    return status;
  }

  /* Take into account that the free space is reduced. */
  DBInfo[handle_p->id].free_space -= ROUND_ADDRESS(header_length + data_length) + RECORD_HEADER_SIZE;
  
  NVMDB_ProcessRequest();

  return NVMDB_STATUS_OK;
}

/**
 * @brief      Delete a record.
 *
 *             NVMDB_DeleteRecord deletes the record pointed by the given handle.
 *             Since the database is in Flash, the record is actually invalidated
 *             and cannot be read by other functions.
 *             A clean operation of the database definitively removes the record.
 *
 * @note       A write operation on the Flash prevents access to the
 *             Flash for a certain amount of time, potentially delaying execution
 *             of the time-critical radio interrupt service routine. Thus, it is checked
 *             that enough time is present before the next scheduled radio activity
 *             will start. If there is no enough time, this operation is scheduled. A
 *             cache is used to temporarily store the operation.
 *
 * @param[in,out] handle_p Handle pointing to the record to be deleted.
 * @retval     Indicates if the function executed successfully.
 */
NVMDB_status_t NVMDB_DeleteRecord(const NVMDB_HandleType *handle_p)
{
  return NVMDB_DeleteRecordNoCache(handle_p);
}

/**
 * @brief      Erase a database.
 *
 *             NVMDB_Erase removes all the records from the database, bringing it
 *             back to the original state when no records have been written.
 *
 * @note       An erase operation on the Flash prevents access to the
 *             Flash for a certain amount of time, potentially delaying execution
 *             of the time-critical radio interrupt service routine. Thus, it is checked
 *             that enough time is present before the next scheduled radio activity
 *             will start. If there is no enough time, this operation is scheduled. A
 *             cache is used to temporarily store the operation.
 *
 * @param      NVMDB_id The ID of the record to be erased.
 * @retval     Indicates if the function executed successfully.
 */
NVMDB_status_t NVMDB_Erase(NVMDB_IdType NVMDB_id)
{
  /* This function schedules a flash erase. Another possible implementation can
   * invalidate all the records of the database and let a clean operation erase the page.
   */
  const NVMDB_SmallDBContainerType *smallDBContainer_p;
  uint8_t type;
  NVMDB_status_t status;

  type = GetDBType(NVMDB_id, &smallDBContainer_p);

  if(type == LARGE_DB)
  {
    uint8_t num_pages;
    uint32_t start_address = DBInfo[NVMDB_id].start_address;
    uint32_t end_address = DBInfo[NVMDB_id].end_address;
    
    num_pages = (ROUNDPAGE(end_address) - start_address) / PAGE_SIZE;
    
    ErasePage(start_address, num_pages);

    NVMDB_get_info(&DBInfo[NVMDB_id]);

    return NVMDB_STATUS_OK;
  }

  if(type == SMALL_DB)
  {

    status = EraseSmallDB(NVMDB_id, smallDBContainer_p);

    if(status == NVMDB_STATUS_OK)
    {
      // Remove any operations in cache for all the DBs in the page. This means write and delete operations.
      // Cache operations for all the DBs in the same page have been executed by LoadDBToRAM().
      for(int i = 0; i < smallDBContainer_p->num_db; i++)
      {
        NVMDB_IdType id = smallDBContainer_p->dbs[i].id;
        NVMDB_get_info(&DBInfo[id]);
      }
    }

    return status;
  }

  return NVMDB_STATUS_INVALID_ID;
}

/**
 * @brief      Clean the database.
 *
 *             It reads the entire database and writes back only valid records.
 *
 * @note       Erase and write operations on the Flash prevents access to the
 *             Flash for a certain amount of time, potentially delaying execution
 *             of the time-critical radio interrupt service routine. Thus, it is checked
 *             that enough time is present before the next scheduled radio activity
 *             will start. If there is no enough time to clean a small DB,
 *             NVMDB_STATUS_NOT_ENOUGH_TIME is returned. In this case the operation
 *             should be retried again later. In case a clean operation is requested
 *             for a large DB, NVMDB_STATUS_NOT_ENOUGH_TIME is returned only if there is
 *             no time to write the first Flash page. If there is time to write this
 *             page, the other write an erase operations are scheduled. Scheduled
 *             operations are temporarily stored in cache. While a clean operation
 *             is scheduled in cache, no other operations are allowed.
 *
 * @param      NVMDB_id The ID of the record to be cleaned.
 * @retval     Indicates if the function executed successfully.
 */
NVMDB_status_t NVMDB_CleanDB(NVMDB_IdType NVMDB_id)
{
  const NVMDB_SmallDBContainerType *smallDBContainer;
  uint8_t type;
  
  LOG_DEBUG_APP("NVMDB_CleanDB(%d)\n", NVMDB_id);

  type = GetDBType(NVMDB_id, &smallDBContainer);
  if(type == LARGE_DB)
  {
    return CleanLargeDB(NVMDB_id);
  }
  if(type == SMALL_DB)
  {
    return CleanPage(smallDBContainer);
  }

  return NVMDB_STATUS_INVALID_ID;
}

/**
 * @brief      Function performing maintenance operations.
 *
 *             This function must be called periodically. Its main task is to
 *             perform scheduled operations.
 *
 * @retval     Returned values is NVMDB_STATUS_OK if no other operations are scheduled.
 *             The value NVMDB_STATUS_NOT_ENOUGH_TIME indicates that some operations
 *             cannot be performed because there is not enough time. Other values
 *             indicates unexpected conditions of the database.
 */
void NVMDB_Tick(void)
{
#if AUTO_CLEAN
  int8_t dirty_db_id;
  
  dirty_db_id = NVMDB_CleanCheck();
  if(dirty_db_id >= 0)
  {
    NVMDB_CleanDB((NVMDB_IdType)dirty_db_id);
    PRINTF("Handle possibly not valid anymore!\r\n");
  }
#endif
}

#if AUTO_CLEAN
// Checks if it is a good time to perform a clean operation
static int8_t NVMDB_CleanCheck(void)
{
  static int8_t db_start_index = 0;
  uint8_t i = db_start_index;

  do
  {
    if(DBInfo[i].invalid_records)
    {
      if(DBInfo[i].free_space < DBInfo[i].clean_threshold)
      {
        // Next time do not start from this db.
        // There may be time to clean another one if this one has not been successfully cleaned.
        db_start_index = i + 1;
        if(db_start_index == NUM_DB)
        {
          db_start_index = 0;
        }
        return i;
      }
    }

    i++;
    if(i == NUM_DB)
    {
      i = 0;
    }
  }
  while(i != db_start_index);

  // All databases have been checked. We can start from first position next time (but not strictly needed).
  db_start_index = 0;

  return -1; // No db to clean
}

#endif

/**
 * @brief Function called by NVMDB module when it is requested to run NVMDB_Tick().
 *
 *        The function must be implemented by application. NVMDB_Tick() should
 *        not be called directly. It is recommended to call it as a background task.
 */
__weak void NVMDB_ProcessRequest(void)
{
}

/**
 * @}
 */

/**
 * @}
 */
