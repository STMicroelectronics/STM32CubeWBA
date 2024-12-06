#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "cmsis_compiler.h"
#include "platform/settings.h"
#include "flash.h"
#include OPENTHREAD_CONFIG_FILE

/******************************************************************************
 * NON VOLATILE STORAGE BUFFER
 *
 * This temporary buffer is located in SRAM and is then copied in FLASH in
 * order to store the non volatile data requested by Thread.
 *
 * This temporary buffer emulates the Flash.
 *
 * Each pages has a size of  SETTINGS_CONFIG_PAGE_SIZE bytes.
 *
 * These two values SETTINGS_CONFIG_PAGE_NUM and SETTINGS_CONFIG_PAGE_SIZE are
 * defined by OpenThread (Refer to stm32wb-openthread-ftd-config.h and
 * stm32wb-openthread-mtd-config.h).
 *
 ******************************************************************************/

/* Private typedef -----------------------------------------------------------*/
OT_TOOL_PACKED_BEGIN
struct settingsBlock
{
    uint16_t key;
    uint16_t length;
} OT_TOOL_PACKED_END;
/* Private define ------------------------------------------------------------*/
#define THREAD_SETTINGS_BUFFER_SIZE 1024
#define THREAD_SETTINGS_RESET_FLAG 0x0784EAD0

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Flag to know whether we are in reset */
__attribute__ ((section (".noinit"))) uint32_t sSettingsIsReset;

/* Pointer to Buf currently used */
__attribute__ ((section (".noinit"))) uint32_t sSettingsBufPos;

//Keep last, aligned 32-bit
__attribute__ ((section (".noinit"))) __attribute__ ((aligned (8))) static uint8_t sSettingBufStart[THREAD_SETTINGS_BUFFER_SIZE];

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief return base address of setting buffer
*/
static uint32_t GetSettingsBuffer_Base(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private functions ---------------------------------------------------------*/

/* stubs for external functions 
   prevent linking errors when not defined in application */
__WEAK void APP_THREAD_SettingsUpdated(settings_type_t SettingType)
{
  /* Can be implemented by user for NVM managment (os dependant) */
  /* empty, by default OT settings are saved in RAM, user responsability to save parameter in ROM */
}

uint32_t GetSettingsBuffer_Base(void)
{
  return (uint32_t)sSettingBufStart;
}

uint32_t GetSettingsBuffer_MaxSize(void)
{
  return (THREAD_SETTINGS_BUFFER_SIZE);
}

otError FillSettingBuffer(uint8_t* buf, uint32_t size)
{
  otError ret = OT_ERROR_NO_BUFS;
  if (size <= THREAD_SETTINGS_BUFFER_SIZE)
  {
    memcpy(sSettingBufStart, buf,size);
    
    sSettingsIsReset = THREAD_SETTINGS_RESET_FLAG;
    
    sSettingsBufPos = GetSettingsBuffer_Base() + size;
    
    ret = OT_ERROR_NONE;
  }
  
  return ret;
}


uint8_t* GetSettingBuffer(uint32_t* size)
{
  if (sSettingsBufPos >= GetSettingsBuffer_Base())
  {
    *size = sSettingsBufPos - GetSettingsBuffer_Base();
  }
  else
  {
    *size = 0;
  }

  return sSettingBufStart;
}


void otPlatSettingsInit(otInstance *aInstance, const uint16_t *aSensitiveKeys, uint16_t aSensitiveKeysLength)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aSensitiveKeys);
    OT_UNUSED_VARIABLE(aSensitiveKeysLength);

    if (sSettingsIsReset != THREAD_SETTINGS_RESET_FLAG)
    {
      sSettingsIsReset = THREAD_SETTINGS_RESET_FLAG;
      sSettingsBufPos = GetSettingsBuffer_Base();
    }
}

void otPlatSettingsDeinit(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
}

otError otPlatSettingsGet(otInstance *aInstance, uint16_t aKey, int aIndex, uint8_t *aValue, uint16_t *aValueLength)
{
  OT_UNUSED_VARIABLE(aInstance);

  const struct settingsBlock *currentBlock;
  uint32_t buf_pos = GetSettingsBuffer_Base();
  int currentIndex = 0;
  uint16_t readLength;
  uint16_t valueLength = 0U;
  otError error = OT_ERROR_NOT_FOUND;

  while (buf_pos < sSettingsBufPos)
  {
    currentBlock = (struct settingsBlock *)(buf_pos);

    if (aKey == currentBlock->key)
    {
      if (currentIndex == aIndex)
      {
        readLength = currentBlock->length;

        // Perform read only if an input buffer was passed in
        if (aValue != NULL && aValueLength != NULL)
        {
          // Adjust read length if input buffer size is smaller
          if (readLength > *aValueLength)
          {
            readLength = *aValueLength;
          }
          memcpy(aValue, (uint8_t *)(buf_pos + sizeof(struct settingsBlock)), readLength);
        }
        valueLength = currentBlock->length;
        error = OT_ERROR_NONE;
        break;
      }
      currentIndex++;
    }
    buf_pos += (sizeof(struct settingsBlock) + currentBlock->length);
  }

  if (aValueLength != NULL)
  {
    *aValueLength = valueLength;
  }

  return error;
}

otError otPlatSettingsAdd(otInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
  OT_UNUSED_VARIABLE(aInstance);
  OT_UNUSED_VARIABLE(aValue);

  otError error;
  struct settingsBlock *currentBlock;
  const uint16_t newBlockLength = sizeof(struct settingsBlock) + aValueLength;

  if ( (sSettingsBufPos +  newBlockLength) <= (GetSettingsBuffer_Base() + GetSettingsBuffer_MaxSize()) )
  {
    currentBlock         = (struct settingsBlock *)sSettingsBufPos;
    currentBlock->key    = aKey;
    currentBlock->length = aValueLength;

    memcpy((uint8_t*) (sSettingsBufPos + sizeof(struct settingsBlock)), aValue, aValueLength);
    
    /* Update current position on Buffer */
    sSettingsBufPos += newBlockLength;
    
    /* Callback to prevent user settings has been added */
    APP_THREAD_SettingsUpdated(SETTINGS_ADDED);

    error = OT_ERROR_NONE;
  }
  else
  {
    error = OT_ERROR_NO_BUFS;
  }

  return error;
}

otError otPlatSettingsDelete(otInstance *aInstance, uint16_t aKey, int aIndex)
{
  OT_UNUSED_VARIABLE(aInstance);

  const struct settingsBlock *currentBlock;
  uint32_t buf_pos = GetSettingsBuffer_Base();
  int currentIndex = 0;
  uint16_t currentBlockLength;
  uint32_t nextBlockStart;
  otError error = OT_ERROR_NOT_FOUND;

  while (buf_pos < sSettingsBufPos)
  {
    currentBlock = (struct settingsBlock *)(buf_pos);
    currentBlockLength = sizeof(struct settingsBlock) + currentBlock->length;
    
    if (aKey == currentBlock->key)
    {
      if ((currentIndex == aIndex)||(aIndex == -1))
      {
        nextBlockStart = buf_pos + currentBlockLength;
        if (nextBlockStart < sSettingsBufPos)
        {
          memmove((uint8_t*)buf_pos, (uint8_t*)nextBlockStart, (sSettingsBufPos - nextBlockStart));
        }

        sSettingsBufPos -= currentBlockLength;
        
        /* Callback to prevent user settings has been added */
        APP_THREAD_SettingsUpdated(SETTINGS_REMOVED);
        error = OT_ERROR_NONE;
        break;
      }
      else
      {
        currentIndex++;
      }
    }
    buf_pos += currentBlockLength;
  }
  return error;
}

otError otPlatSettingsSet(otInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
  OT_UNUSED_VARIABLE(aInstance);

  const struct settingsBlock *currentBlock;
  uint32_t buf_pos = GetSettingsBuffer_Base();
  uint16_t currentBlockLength;
  uint32_t nextBlockStart;

  while (buf_pos < sSettingsBufPos)
  {
    currentBlock = (struct settingsBlock *)(buf_pos);
    currentBlockLength = sizeof(struct settingsBlock) + currentBlock->length;
    if (aKey == currentBlock->key)
    {
      nextBlockStart = buf_pos + currentBlockLength;
      if (nextBlockStart < sSettingsBufPos)
      {
        memmove((uint8_t *)(buf_pos), (uint8_t*)nextBlockStart, (sSettingsBufPos - nextBlockStart));
      }
      sSettingsBufPos -= currentBlockLength;
    }
    else
    {
      buf_pos += currentBlockLength;
    }
  }

  return otPlatSettingsAdd(aInstance, aKey, aValue, aValueLength);
}

void otPlatSettingsWipe(otInstance *aInstance)
{
  /* Reset pos & reset flag like at first init */
  sSettingsIsReset = THREAD_SETTINGS_RESET_FLAG;
  sSettingsBufPos = GetSettingsBuffer_Base();
  APP_THREAD_SettingsUpdated(SETTINGS_MASSERASE);
}