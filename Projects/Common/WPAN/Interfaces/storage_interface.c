/**
  ******************************************************************************
  * @file    storage_interface.c
  * @author  MCD Application Team
  * @brief   Implementation of storage interface
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

/* Includes ------------------------------------------------------------------*/
#include "storage_interface.h"
#include "string.h"
#include <stdio.h>

#if defined(PSA_USE_ITS_ALT)
/* ICache */
#include "stm32wbaxx_ll_icache.h"

/* Uncomment the line below if you want some debug logs */
/* #define FLASH_IF_DBG */
#ifdef FLASH_IF_DBG
#define FLASH_IF_TRACE printf
#else
#define FLASH_IF_TRACE(...)
#endif /* FLASH_IF_DBG */

/* Private typedef -----------------------------------------------------------*/
typedef struct StorageTrackers
{
  uint32_t BaseAddress;   /* Base address of the storage zone */
  uint32_t CurrentBankId; /* Bank 0 or Bank 1 */
  uint32_t UsedSlots;     /* Physical slots written in active bank */
  uint32_t ValidSlots;    /* Non-invalidated slots in active bank */
} StorageTrackers_t;

typedef struct StorageBankStats
{
  uint32_t UsedSlots;
  uint32_t ValidSlots;
} StorageBankStats_t;

/* Private define ------------------------------------------------------------*/
#define FLASH_IF_WRITE_GRANULARITY_BYTES (16U) /* STM32WBA quadword programming */
#define NB_PAGE_SECTOR_PER_ERASE         (1U)  /* Nb page erased per erase */

/*
 * Each storage area uses 2 banks x 16 slots x 512 bytes = 16 KiB.
 * If encrypted ITS is enabled, reserve 32 KiB at the end of a 1 MiB flash.
 * The linker script must reserve the selected pages.
 */
#if defined(PSA_USE_ENCRYPTED_ITS)
#define ITS_LOCATION                     0x080F8000U
#define ITS_ENCRYPTION_KEY_LOCATION      0x080FC000U
#else
#define ITS_LOCATION                     0x080FC000U
#endif /* PSA_USE_ENCRYPTED_ITS */

#define ITS_SLOT_OFFSET_WORDS            (0x00000080U) /* 128 words */
#define ITS_SLOT_MAX_NUMBER              (16U)
#define ITS_BANK_MAX_NUMBER              (2U)
#define ITS_INVALID_BANK_ID              (0xFFU)

#define ITS_SLOT_SIZE_BYTES              (ITS_SLOT_OFFSET_WORDS * sizeof(uint32_t))
#define ITS_BANK_SIZE_BYTES              (ITS_SLOT_SIZE_BYTES * ITS_SLOT_MAX_NUMBER)
#define ITS_MAX_SIZE                     (ITS_BANK_SIZE_BYTES * ITS_BANK_MAX_NUMBER)
#define ITS_SLOT_HEADER_SIZE             (sizeof(its_obj_info_t))
#define ITS_SLOT_PAYLOAD_SIZE            (ITS_SLOT_SIZE_BYTES - ITS_SLOT_HEADER_SIZE)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static StorageTrackers_t StorageTrackers = {ITS_LOCATION,
                                            ITS_INVALID_BANK_ID,
                                            0U,
                                            0U};
#if defined(PSA_USE_ENCRYPTED_ITS)
static StorageTrackers_t StorageTrackersEncrypted = {ITS_ENCRYPTION_KEY_LOCATION,
                                                     ITS_INVALID_BANK_ID,
                                                     0U,
                                                     0U};
#endif /* PSA_USE_ENCRYPTED_ITS */
static uint32_t StorageInitialized = 0U;

/* Private function prototypes -----------------------------------------------*/
static uint32_t FLASH_If_GetPage(uint32_t uAddr);
static HAL_StatusTypeDef FLASH_If_Init(void);
static HAL_StatusTypeDef FLASH_If_Write(void *pDestination, const void *pSource,
                                        uint32_t uLength);
static HAL_StatusTypeDef FLASH_If_Erase_Size(void *pStart, uint32_t uLength);

static StorageTrackers_t *Storage_GetTrackers(uint64_t obj_uid);
static uint32_t *Storage_GetSlotAddress(uint32_t base_address, uint32_t bank_id,
                                        uint32_t slot_id);
static uint32_t Storage_IsSlotErased(const uint32_t *p_slot_address);
static uint32_t Storage_IsSlotInvalid(const uint32_t *p_slot_address);
static uint32_t Storage_IsSlotValid(const uint32_t *p_slot_address);
static uint32_t Storage_IsSlotUidMatching(const uint32_t *p_slot_address,
                                          uint64_t obj_uid);
static void Storage_ScanBank(uint32_t base_address, uint32_t bank_id,
                             StorageBankStats_t *p_stats);
static uint32_t Storage_SelectActiveBank(const StorageBankStats_t *p_bank0,
                                         const StorageBankStats_t *p_bank1);
static psa_status_t Storage_InitTrackers(StorageTrackers_t *p_trackers);
static int32_t Storage_FindLatestSlot(const StorageTrackers_t *p_trackers,
                                      uint64_t obj_uid);
static HAL_StatusTypeDef Storage_InvalidateSlot(uint32_t *p_slot_address);
static HAL_StatusTypeDef Storage_InvalidateMatchingSlots(StorageTrackers_t *p_trackers,
                                                         uint64_t obj_uid,
                                                         uint32_t slots_to_scan,
                                                         int32_t slot_to_keep,
                                                         uint32_t *p_invalidated_count);
static HAL_StatusTypeDef Storage_Swap(StorageTrackers_t *p_trackers);

/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  Gets the page of a given address.
  * @param  uAddr: Address of the FLASH Memory.
  * @retval The page of a given address.
  */
static uint32_t FLASH_If_GetPage(uint32_t uAddr)
{
  uint32_t page = 0U;

  if (uAddr < (FLASH_BASE + FLASH_BANK_SIZE))
  {
    page = (uAddr - FLASH_BASE) / FLASH_PAGE_SIZE;
  }
  else
  {
    page = (uAddr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
  }

  return page;
}

/**
  * @brief  Unlocks Flash for write access and clears error flags.
  * @param  None.
  * @retval HAL Status.
  */
static HAL_StatusTypeDef FLASH_If_Init(void)
{
  HAL_StatusTypeDef ret = HAL_ERROR;

  if (HAL_FLASH_Unlock() == HAL_OK)
  {
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

    if (HAL_FLASH_Lock() == HAL_OK)
    {
      ret = HAL_OK;
    }
#ifdef FLASH_IF_DBG
    else
    {
      FLASH_IF_TRACE("[FLASH_IF] Lock failure\r\n");
    }
#endif /* FLASH_IF_DBG */
  }
#ifdef FLASH_IF_DBG
  else
  {
    FLASH_IF_TRACE("[FLASH_IF] Unlock failure\r\n");
  }
#endif /* FLASH_IF_DBG */

  return ret;
}

/**
  * @brief  This function erases flash pages in user flash area.
  * @param  pStart: Start of user flash area.
  * @param  uLength: Number of bytes.
  * @retval HAL_OK if success, an error code otherwise.
  */
static HAL_StatusTypeDef FLASH_If_Erase_Size(void *pStart, uint32_t uLength)
{
  uint32_t page_error = 0U;
  uint32_t uStart = (uint32_t)pStart;
  FLASH_EraseInitTypeDef EraseInitStruct;
  HAL_StatusTypeDef e_ret_status = HAL_ERROR;
  uint32_t first_page = 0U;
  uint32_t nb_pages = 0U;
  uint32_t chunk_nb_pages;

  if ((pStart == NULL) || (uLength == 0U))
  {
    return HAL_ERROR;
  }

  e_ret_status = FLASH_If_Init();

  if (e_ret_status == HAL_OK)
  {
    if (HAL_FLASH_Unlock() == HAL_OK)
    {
      first_page = FLASH_If_GetPage(uStart);
      nb_pages = FLASH_If_GetPage(uStart + uLength - 1U) - first_page + 1U;

      EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
      EraseInitStruct.Banks = ((uStart - FLASH_BASE) >= FLASH_BANK_SIZE) ? FLASH_BANK_2 : FLASH_BANK_1;

      do
      {
        chunk_nb_pages = (nb_pages >= NB_PAGE_SECTOR_PER_ERASE) ? NB_PAGE_SECTOR_PER_ERASE : nb_pages;
        EraseInitStruct.Page = first_page;
        EraseInitStruct.NbPages = chunk_nb_pages;
        first_page += chunk_nb_pages;
        nb_pages -= chunk_nb_pages;

        if (HAL_FLASHEx_Erase(&EraseInitStruct, &page_error) != HAL_OK)
        {
          (void)HAL_FLASH_GetError();
          e_ret_status = HAL_ERROR;
          break;
        }

        if (1U == LL_ICACHE_IsEnabled())
        {
          do
          {
            LL_ICACHE_Invalidate();
          } while (0U == LL_ICACHE_IsActiveFlag_BUSY());

          while (1U != LL_ICACHE_IsActiveFlag_BSYEND())
          {
          }
        }
      } while (nb_pages > 0U);

      (void)HAL_FLASH_Lock();
    }
    else
    {
      e_ret_status = HAL_ERROR;
    }
  }

  return e_ret_status;
}

/**
  * @brief  This function writes a data buffer in flash.
  * @note   STM32WBA flash is programmed by 128-bit quadword.
  * @retval HAL_OK if success, an error code otherwise.
  */
static HAL_StatusTypeDef FLASH_If_Write(void *pDestination, const void *pSource, uint32_t uLength)
{
  HAL_StatusTypeDef e_ret_status = HAL_ERROR;
  const uint8_t *p_src = (const uint8_t *)pSource;
  uint32_t quadword[4];
  uint32_t i;
  uint32_t remaining;
  uint32_t copy_len;

  if ((pDestination == NULL) || (pSource == NULL) || (uLength == 0U))
  {
    return HAL_ERROR;
  }

  e_ret_status = FLASH_If_Init();

  if (e_ret_status == HAL_OK)
  {
    if (HAL_FLASH_Unlock() != HAL_OK)
    {
      return HAL_ERROR;
    }

    for (i = 0U; i < uLength; i += FLASH_IF_WRITE_GRANULARITY_BYTES)
    {
      remaining = uLength - i;
      copy_len = (remaining >= FLASH_IF_WRITE_GRANULARITY_BYTES) ?
                 FLASH_IF_WRITE_GRANULARITY_BYTES : remaining;

      quadword[0] = 0xFFFFFFFFU;
      quadword[1] = 0xFFFFFFFFU;
      quadword[2] = 0xFFFFFFFFU;
      quadword[3] = 0xFFFFFFFFU;
      (void)memcpy(quadword, &p_src[i], copy_len);

      if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD,
                            (uint32_t)pDestination,
                            (uint32_t)quadword) == HAL_OK)
      {
        if (1U == LL_ICACHE_IsEnabled())
        {
          do
          {
            LL_ICACHE_Invalidate();
          } while (0U == LL_ICACHE_IsActiveFlag_BUSY());

          while (1U != LL_ICACHE_IsActiveFlag_BSYEND())
          {
          }
        }

        if (memcmp(pDestination, quadword, FLASH_IF_WRITE_GRANULARITY_BYTES) != 0)
        {
          e_ret_status = HAL_ERROR;
          break;
        }

        pDestination = (void *)((uint32_t)pDestination + FLASH_IF_WRITE_GRANULARITY_BYTES);
      }
      else
      {
        e_ret_status = HAL_ERROR;
        break;
      }
    }

    (void)HAL_FLASH_Lock();
  }

  return e_ret_status;
}

/**
  * @brief  Return storage tracker according to object zone.
  * @param  obj_uid: Object unique identifier.
  * @retval Pointer to trackers.
  */
static StorageTrackers_t *Storage_GetTrackers(uint64_t obj_uid)
{
#if defined(PSA_USE_ENCRYPTED_ITS)
  if (obj_uid == ITS_ENCRYPTION_SECRET_KEY_ID)
  {
    return &StorageTrackersEncrypted;
  }
#endif /* PSA_USE_ENCRYPTED_ITS */

  return &StorageTrackers;
}

/**
  * @brief  Return slot start address for a given bank and slot index.
  * @param  base_address: Zone base address.
  * @param  bank_id: Logical bank identifier.
  * @param  slot_id: Slot index in bank.
  * @retval Slot start address.
  */
static uint32_t *Storage_GetSlotAddress(uint32_t base_address, uint32_t bank_id,
                                        uint32_t slot_id)
{
  uint32_t slot_address = base_address + (bank_id * ITS_BANK_SIZE_BYTES) +
                          (slot_id * ITS_SLOT_SIZE_BYTES);

  return (uint32_t *)slot_address;
}

static uint32_t Storage_IsSlotErased(const uint32_t *p_slot_address)
{
  const uint32_t erased_quadword[4] = {0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU};

  return (memcmp(p_slot_address, erased_quadword, sizeof(erased_quadword)) == 0) ? 1U : 0U;
}

static uint32_t Storage_IsSlotInvalid(const uint32_t *p_slot_address)
{
  const uint32_t invalid_quadword[4] = {0U, 0U, 0U, 0U};

  return (memcmp(p_slot_address, invalid_quadword, sizeof(invalid_quadword)) == 0) ? 1U : 0U;
}

static uint32_t Storage_IsSlotValid(const uint32_t *p_slot_address)
{
  return ((Storage_IsSlotErased(p_slot_address) == 0U) &&
          (Storage_IsSlotInvalid(p_slot_address) == 0U)) ? 1U : 0U;
}

static uint32_t Storage_IsSlotUidMatching(const uint32_t *p_slot_address, uint64_t obj_uid)
{
  if (Storage_IsSlotValid(p_slot_address) == 0U)
  {
    return 0U;
  }

  return (memcmp(&obj_uid, p_slot_address, sizeof(obj_uid)) == 0) ? 1U : 0U;
}

/**
  * @brief  Scan one bank and compute used/valid slots statistics.
  * @param  base_address: Zone base address.
  * @param  bank_id: Logical bank identifier.
  * @param  p_stats: Output statistics.
  * @retval None.
  */
static void Storage_ScanBank(uint32_t base_address, uint32_t bank_id,
                             StorageBankStats_t *p_stats)
{
  uint32_t slot_id;
  int32_t last_non_erased_slot = -1;

  p_stats->UsedSlots = 0U;
  p_stats->ValidSlots = 0U;

  for (slot_id = 0U; slot_id < ITS_SLOT_MAX_NUMBER; slot_id++)
  {
    uint32_t *p_slot_address = Storage_GetSlotAddress(base_address, bank_id, slot_id);

    if (Storage_IsSlotErased(p_slot_address) != 0U)
    {
      continue;
    }

    last_non_erased_slot = (int32_t)slot_id;

    if (Storage_IsSlotInvalid(p_slot_address) == 0U)
    {
      p_stats->ValidSlots++;
    }
  }

  if (last_non_erased_slot >= 0)
  {
    p_stats->UsedSlots = (uint32_t)last_non_erased_slot + 1U;
  }
}

/**
  * @brief  Select active bank according to scan statistics.
  * @param  p_bank0: Bank 0 stats.
  * @param  p_bank1: Bank 1 stats.
  * @retval Selected bank identifier.
  */
static uint32_t Storage_SelectActiveBank(const StorageBankStats_t *p_bank0,
                                         const StorageBankStats_t *p_bank1)
{
  if (p_bank1->UsedSlots > p_bank0->UsedSlots)
  {
    return 1U;
  }

  if (p_bank0->UsedSlots > p_bank1->UsedSlots)
  {
    return 0U;
  }

  if (p_bank1->ValidSlots > p_bank0->ValidSlots)
  {
    return 1U;
  }

  return 0U;
}

/**
  * @brief  Initialize one zone trackers by scanning both banks.
  * @param  p_trackers: Zone trackers to initialize.
  * @retval PSA_SUCCESS if success, an error code otherwise.
  */
static psa_status_t Storage_InitTrackers(StorageTrackers_t *p_trackers)
{
  uint32_t selected_bank;
  StorageBankStats_t bank_stats[ITS_BANK_MAX_NUMBER];

  Storage_ScanBank(p_trackers->BaseAddress, 0U, &bank_stats[0]);
  Storage_ScanBank(p_trackers->BaseAddress, 1U, &bank_stats[1]);

  selected_bank = Storage_SelectActiveBank(&bank_stats[0], &bank_stats[1]);

  p_trackers->CurrentBankId = selected_bank;
  p_trackers->UsedSlots = bank_stats[selected_bank].UsedSlots;
  p_trackers->ValidSlots = bank_stats[selected_bank].ValidSlots;

  if (FLASH_If_Erase_Size((void *)(p_trackers->BaseAddress +
                                   ((1U - selected_bank) * ITS_BANK_SIZE_BYTES)),
                          ITS_BANK_SIZE_BYTES) != HAL_OK)
  {
    return PSA_ERROR_STORAGE_FAILURE;
  }

  return PSA_SUCCESS;
}

static int32_t Storage_FindLatestSlot(const StorageTrackers_t *p_trackers, uint64_t obj_uid)
{
  uint32_t slot_id;

  for (slot_id = p_trackers->UsedSlots; slot_id > 0U; slot_id--)
  {
    uint32_t *p_slot_address = Storage_GetSlotAddress(p_trackers->BaseAddress,
                                                      p_trackers->CurrentBankId,
                                                      slot_id - 1U);

    if (Storage_IsSlotUidMatching(p_slot_address, obj_uid) != 0U)
    {
      return (int32_t)(slot_id - 1U);
    }
  }

  return -1;
}

/**
  * @brief  Invalidate a slot by programming its first quadword with zeros.
  * @param  p_slot_address: Slot start address.
  * @retval HAL_OK if success, HAL_ERROR otherwise.
  */
static HAL_StatusTypeDef Storage_InvalidateSlot(uint32_t *p_slot_address)
{
  uint32_t invalidation_quadword[4] = {0U, 0U, 0U, 0U};

  if (Storage_IsSlotValid(p_slot_address) == 0U)
  {
    return HAL_OK;
  }

  if (FLASH_If_Write((void *)p_slot_address, invalidation_quadword,
                     sizeof(invalidation_quadword)) != HAL_OK)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

static HAL_StatusTypeDef Storage_InvalidateMatchingSlots(StorageTrackers_t *p_trackers,
                                                         uint64_t obj_uid,
                                                         uint32_t slots_to_scan,
                                                         int32_t slot_to_keep,
                                                         uint32_t *p_invalidated_count)
{
  uint32_t slot_id;

  *p_invalidated_count = 0U;

  for (slot_id = 0U; slot_id < slots_to_scan; slot_id++)
  {
    uint32_t *p_slot_address = Storage_GetSlotAddress(p_trackers->BaseAddress,
                                                      p_trackers->CurrentBankId,
                                                      slot_id);

    if (((int32_t)slot_id != slot_to_keep) &&
        (Storage_IsSlotUidMatching(p_slot_address, obj_uid) != 0U))
    {
      if (Storage_InvalidateSlot(p_slot_address) != HAL_OK)
      {
        return HAL_ERROR;
      }

      if (p_trackers->ValidSlots > 0U)
      {
        p_trackers->ValidSlots--;
      }

      *p_invalidated_count += 1U;
    }
  }

  return HAL_OK;
}

/**
  * @brief  Swap to the alternate bank when current one is full or fragmented.
  * @param  p_trackers: Zone trackers.
  * @retval HAL_OK if success, HAL_ERROR otherwise.
  */
static HAL_StatusTypeDef Storage_Swap(StorageTrackers_t *p_trackers)
{
  uint32_t slot_id;
  uint32_t copied_slots = 0U;
  uint32_t next_bank = (p_trackers->CurrentBankId + 1U) % ITS_BANK_MAX_NUMBER;
  uint32_t next_bank_base = p_trackers->BaseAddress + (next_bank * ITS_BANK_SIZE_BYTES);
  uint32_t current_bank_base = p_trackers->BaseAddress + (p_trackers->CurrentBankId * ITS_BANK_SIZE_BYTES);

  if (FLASH_If_Erase_Size((void *)next_bank_base, ITS_BANK_SIZE_BYTES) != HAL_OK)
  {
    return HAL_ERROR;
  }

  for (slot_id = 0U; slot_id < p_trackers->UsedSlots; slot_id++)
  {
    uint32_t *p_storage = Storage_GetSlotAddress(p_trackers->BaseAddress,
                                                 p_trackers->CurrentBankId,
                                                 slot_id);

    if (Storage_IsSlotValid(p_storage) != 0U)
    {
      uint32_t destination = next_bank_base + (copied_slots * ITS_SLOT_SIZE_BYTES);

      if (FLASH_If_Write((void *)destination, (void *)p_storage, ITS_SLOT_SIZE_BYTES) != HAL_OK)
      {
        return HAL_ERROR;
      }

      copied_slots++;
    }
  }

  if (FLASH_If_Erase_Size((void *)current_bank_base, ITS_BANK_SIZE_BYTES) != HAL_OK)
  {
    return HAL_ERROR;
  }

  p_trackers->CurrentBankId = next_bank;
  p_trackers->UsedSlots = copied_slots;
  p_trackers->ValidSlots = copied_slots;

  return HAL_OK;
}

/**
 * @brief  Initialize the storage module.
 * @retval PSA_SUCCESS if initialization is successful, an error code otherwise.
 */
psa_status_t storage_init(void)
{
  if (Storage_InitTrackers(&StorageTrackers) != PSA_SUCCESS)
  {
    return PSA_ERROR_STORAGE_FAILURE;
  }

#if defined(PSA_USE_ENCRYPTED_ITS)
  if (Storage_InitTrackers(&StorageTrackersEncrypted) != PSA_SUCCESS)
  {
    return PSA_ERROR_STORAGE_FAILURE;
  }
#endif /* PSA_USE_ENCRYPTED_ITS */

  StorageInitialized = 1U;

  return PSA_SUCCESS;
}

/**
  * @brief  A function that stores the object record in storage.
  * @note   p_obj is expected to point to the serialized ITS record whose first
  *         field is the object UID, followed by the object metadata and payload.
  * @param  obj_uid : unique identifier used for identifying the object.
  * @param  obj_length : size of the serialized object record in bytes.
  * @param  p_obj : pointer to the serialized object record.
  * @retval PSA_SUCCESS if success, an error code otherwise.
  */
psa_status_t storage_set(uint64_t obj_uid,
                         uint32_t obj_length,
                         const void *p_obj)
{
  uint32_t *p_storage = NULL;
  StorageTrackers_t *p_trackers = NULL;
  int32_t old_slot;
  uint32_t invalidated_count = 0U;
  uint32_t previous_used_slots;

  if (StorageInitialized != 1U)
  {
    return PSA_ERROR_STORAGE_FAILURE;
  }

  if ((p_obj == NULL) || (obj_length == 0U) || (obj_length > ITS_SLOT_SIZE_BYTES))
  {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  if (memcmp(&obj_uid, p_obj, sizeof(obj_uid)) != 0)
  {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  p_trackers = Storage_GetTrackers(obj_uid);
  old_slot = Storage_FindLatestSlot(p_trackers, obj_uid);

  if (p_trackers->UsedSlots >= ITS_SLOT_MAX_NUMBER)
  {
    if (old_slot >= 0)
    {
      uint32_t *p_old_slot = Storage_GetSlotAddress(p_trackers->BaseAddress,
                                                    p_trackers->CurrentBankId,
                                                    (uint32_t)old_slot);

      if (Storage_InvalidateSlot(p_old_slot) != HAL_OK)
      {
        return PSA_ERROR_STORAGE_FAILURE;
      }

      if (p_trackers->ValidSlots > 0U)
      {
        p_trackers->ValidSlots--;
      }
    }
    else if (p_trackers->ValidSlots >= ITS_SLOT_MAX_NUMBER)
    {
      return PSA_ERROR_INSUFFICIENT_STORAGE;
    }

    if (Storage_Swap(p_trackers) != HAL_OK)
    {
      return PSA_ERROR_STORAGE_FAILURE;
    }

    old_slot = -1;
  }

  if (p_trackers->UsedSlots >= ITS_SLOT_MAX_NUMBER)
  {
    return PSA_ERROR_INSUFFICIENT_STORAGE;
  }

  previous_used_slots = p_trackers->UsedSlots;
  p_storage = Storage_GetSlotAddress(p_trackers->BaseAddress,
                                     p_trackers->CurrentBankId,
                                     p_trackers->UsedSlots);

  if (FLASH_If_Write(p_storage, p_obj, obj_length) != HAL_OK)
  {
    return PSA_ERROR_STORAGE_FAILURE;
  }

  p_trackers->UsedSlots++;
  p_trackers->ValidSlots++;

  if (previous_used_slots > 0U)
  {
    if (Storage_InvalidateMatchingSlots(p_trackers,
                                        obj_uid,
                                        previous_used_slots,
                                        -1,
                                        &invalidated_count) != HAL_OK)
    {
      return PSA_ERROR_STORAGE_FAILURE;
    }
  }

  return PSA_SUCCESS;
}

/**
  * @brief  A function that retrieves the object payload from storage.
  * @param  obj_uid : unique identifier used for identifying the object.
  * @param  obj_offset : The starting byte offset in the object payload.
  * @param  obj_length : size of the object payload to retrieve in bytes.
  * @param  p_obj : pointer to the output object buffer.
  * @retval PSA_SUCCESS if success, an error code otherwise.
  */
psa_status_t storage_get(uint64_t obj_uid,
                         uint32_t obj_offset,
                         uint32_t obj_length,
                         void *p_obj)
{
  int32_t slot;
  uint32_t *p_slot_address;
  uint8_t *p_payload;
  StorageTrackers_t *p_trackers = NULL;

  if (StorageInitialized != 1U)
  {
    return PSA_ERROR_STORAGE_FAILURE;
  }

  if ((p_obj == NULL) ||
      ((obj_offset + obj_length) < obj_offset) ||
      ((obj_offset + obj_length) > ITS_SLOT_PAYLOAD_SIZE))
  {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  p_trackers = Storage_GetTrackers(obj_uid);
  slot = Storage_FindLatestSlot(p_trackers, obj_uid);

  if (slot < 0)
  {
    return PSA_ERROR_DOES_NOT_EXIST;
  }

  p_slot_address = Storage_GetSlotAddress(p_trackers->BaseAddress,
                                          p_trackers->CurrentBankId,
                                          (uint32_t)slot);
  p_payload = ((uint8_t *)p_slot_address) + ITS_SLOT_HEADER_SIZE + obj_offset;

  (void)memcpy(p_obj, p_payload, obj_length);

  return PSA_SUCCESS;
}

/**
  * @brief  A function that retrieves the object info using the object unique
  *         identifier.
  * @param  obj_uid : unique identifier used for identifying data.
  * @param  p_obj_info : pointer to metadata: ID, size, flags.
  * @param  obj_info_size : size of the object info.
  * @retval PSA_SUCCESS if success, an error code otherwise.
  */
psa_status_t storage_get_info(uint64_t obj_uid, void *p_obj_info, uint32_t obj_info_size)
{
  int32_t slot;
  uint32_t *p_slot_address;
  StorageTrackers_t *p_trackers = NULL;

  if (StorageInitialized != 1U)
  {
    return PSA_ERROR_STORAGE_FAILURE;
  }

  if ((p_obj_info == NULL) || (obj_info_size > ITS_SLOT_HEADER_SIZE))
  {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  p_trackers = Storage_GetTrackers(obj_uid);
  slot = Storage_FindLatestSlot(p_trackers, obj_uid);

  if (slot < 0)
  {
    return PSA_ERROR_DOES_NOT_EXIST;
  }

  p_slot_address = Storage_GetSlotAddress(p_trackers->BaseAddress,
                                          p_trackers->CurrentBankId,
                                          (uint32_t)slot);

  (void)memcpy(p_obj_info, p_slot_address, obj_info_size);

  return PSA_SUCCESS;
}

/**
  * @brief  A function that removes the object from storage.
  * @param  obj_uid : unique identifier used for identifying the object.
  * @param  obj_size : size of object to be removed from storage.
  * @retval PSA_SUCCESS if success, an error code otherwise.
  */
psa_status_t storage_remove(uint64_t obj_uid, uint32_t obj_size)
{
  StorageTrackers_t *p_trackers = NULL;
  uint32_t invalidated_count = 0U;

  (void)obj_size;

  if (StorageInitialized != 1U)
  {
    return PSA_ERROR_STORAGE_FAILURE;
  }

  p_trackers = Storage_GetTrackers(obj_uid);

  if (Storage_InvalidateMatchingSlots(p_trackers,
                                      obj_uid,
                                      p_trackers->UsedSlots,
                                      -1,
                                      &invalidated_count) != HAL_OK)
  {
    return PSA_ERROR_STORAGE_FAILURE;
  }

  if (invalidated_count == 0U)
  {
    return PSA_ERROR_DOES_NOT_EXIST;
  }

  return PSA_SUCCESS;
}

void psa_its_init(void);
void psa_its_init(void)
{
  (void)storage_init();
}

psa_status_t storage_reset(void)
{
  if (FLASH_If_Erase_Size((void *)StorageTrackers.BaseAddress, ITS_MAX_SIZE) != HAL_OK)
  {
    return PSA_ERROR_STORAGE_FAILURE;
  }

  StorageTrackers.CurrentBankId = 0U;
  StorageTrackers.UsedSlots = 0U;
  StorageTrackers.ValidSlots = 0U;

#if defined(PSA_USE_ENCRYPTED_ITS)
  if (FLASH_If_Erase_Size((void *)StorageTrackersEncrypted.BaseAddress, ITS_MAX_SIZE) != HAL_OK)
  {
    return PSA_ERROR_STORAGE_FAILURE;
  }

  StorageTrackersEncrypted.CurrentBankId = 0U;
  StorageTrackersEncrypted.UsedSlots = 0U;
  StorageTrackersEncrypted.ValidSlots = 0U;
#endif /* PSA_USE_ENCRYPTED_ITS */

  StorageInitialized = 1U;

  return PSA_SUCCESS;
}

void psa_its_reset_storage(void)
{
  (void)storage_reset();
}

#endif /* PSA_USE_ITS_ALT */
