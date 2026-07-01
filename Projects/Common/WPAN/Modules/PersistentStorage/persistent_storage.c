/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    persistent_storage.c
  * @author  MCD Application Team
  * @brief   Source for Persistent Storage module
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

/* Utilities */
#include "utilities_common.h"

/* Own header files */
#include "persistent_storage.h"
#include "persistent_storage_conf.h"

/* Private defines -----------------------------------------------------------*/
/**
 * @brief Number of bytes in a 128 bits flash line
 */
#define PS_FLASH_LINE_128B  0x10u

/**
 * @brief Number of bytes in a 64 bits flash line
 */
#define PS_FLASH_LINE_64B  0x8u

/* -- 128bits flash line operations and definitions-------------------------- */
#if (PS_FLASH_LINE_LENGTH == PS_FLASH_LINE_128B)
/**
 * @brief Number of data present in the header of a data packet - In bytes
 */
#define PS_HEADER_DATA_NUMBER   0x08u

/**
 * @brief Alignment 128bits
 */
#define PS_ALIGNMENT_MASK  0x0000000Fu

/* -- 64bits flash line operations and definitions--------------------------- */
#elif (PS_FLASH_LINE_LENGTH == PS_FLASH_LINE_64B)
/**
 * @brief Number of data present in the header of a data packet - In bytes
 */
#define PS_HEADER_DATA_NUMBER   0x00u

/**
 * @brief Alignment 128bits
 */
#define PS_ALIGNMENT_MASK  0x00000007u

/* -- Unsupported flash line ------------------------------------------------ */
#else
#error "Flash line length not supported"
#endif /* PS_FLASH_LINE_LENGTH */

/**
 * @brief Polynomial value for CRC 8 bits computation
 */
#define PS_CRC8_POLY 0x07

/**
 * @brief Reset value for CRC 8 bits computation
 */
#define PS_CRC8_RESET_VALUE 0xFF

/**
 * @brief Mask of the Data ID
 */
#define PS_HEADER_DATA_ID_MASK   0xFF000000u

/**
 * @brief Position of the Data ID
 */
#define PS_HEADER_DATA_ID_POS   24u

/**
 * @brief Mask of the Data Length
 */
#define PS_HEADER_DATA_LENGTH_MASK   0x00FFFFFFu

/**
 * @brief Position of the Data Length
 */
#define PS_HEADER_DATA_LENGTH_POS   0u

/**
 * @brief Mask of the Bank erase counter
 */
#define PS_HEADER_BANK_ERASE_COUNTER_MASK   0x00FFFFFFu

/**
 * @brief Position of the Bank erase counter
 */
#define PS_HEADER_BANK_ERASE_COUNTER_POS 0u

/**
 * @brief Mask of the Bank header CRC
 */
#define PS_HEADER_BANK_CRC_MASK   0xFF000000u

/**
 * @brief Position of the Bank header CRC
 */
#define PS_HEADER_BANK_CRC_POS   24u

/**
 * @brief Length of the information in the header without the CRC
 */
#define PS_HEADER_BANK_SIZE_WO_CRC  7u

/**
 * @brief Size of Data information field in a data header in bytes
 */
#define PS_DATA_HEADER_DATA_INFO_SIZE 4u

/**
 * @brief Maximum amount of write failures allowed in one write operation
 */
#define PS_WRITE_FAILURE_LIMIT 20u

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_0_MASK   (1u << PS_DATA_ID_0)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_1_MASK   (1u << PS_DATA_ID_1)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_2_MASK   (1u << PS_DATA_ID_2)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_3_MASK   (1u << PS_DATA_ID_3)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_4_MASK   (1u << PS_DATA_ID_4)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_5_MASK   (1u << PS_DATA_ID_5)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_6_MASK   (1u << PS_DATA_ID_6)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_7_MASK   (1u << PS_DATA_ID_7)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_8_MASK   (1u << PS_DATA_ID_8)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_9_MASK   (1u << PS_DATA_ID_9)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_10_MASK  (1u << PS_DATA_ID_10)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_11_MASK  (1u << PS_DATA_ID_11)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_12_MASK  (1u << PS_DATA_ID_12)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_13_MASK  (1u << PS_DATA_ID_13)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_14_MASK  (1u << PS_DATA_ID_14)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_15_MASK  (1u << PS_DATA_ID_15)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_16_MASK  (1u << PS_DATA_ID_16)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_17_MASK  (1u << PS_DATA_ID_17)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_18_MASK  (1u << PS_DATA_ID_18)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_19_MASK  (1u << PS_DATA_ID_19)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_20_MASK  (1u << PS_DATA_ID_20)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_21_MASK  (1u << PS_DATA_ID_21)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_22_MASK  (1u << PS_DATA_ID_22)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_23_MASK  (1u << PS_DATA_ID_23)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_24_MASK  (1u << PS_DATA_ID_24)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_25_MASK  (1u << PS_DATA_ID_25)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_26_MASK  (1u << PS_DATA_ID_26)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_27_MASK  (1u << PS_DATA_ID_27)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_28_MASK  (1u << PS_DATA_ID_28)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_29_MASK  (1u << PS_DATA_ID_29)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_30_MASK  (1u << PS_DATA_ID_30)

/**
 * @brief Mask for Presence bitmask checking
 */
#define PS_DATA_ID_31_MASK  (1u << PS_DATA_ID_31)

/* Private macros ------------------------------------------------------------*/
/**
 * @brief Compute a modulo
 */
#define PS_COMPUTE_MODULO(Value, Modulo)   \
  (Value & (Modulo - 1u))

/**
 * @brief Align an address on the flash line boundary
 */
#define PS_ALIGN_FLASH_LINE(p_Addr)  \
  (uint32_t *)(((uint32_t)p_Addr + PS_ALIGNMENT_MASK) & ~PS_ALIGNMENT_MASK)

/**
 * @brief Macro to get the Data ID from a PS_DataHeader_t::DataInfo field
 */
#define PS_GET_DATA_ID(DataInfo) \
  ((DataInfo & PS_HEADER_DATA_ID_MASK) >> PS_HEADER_DATA_ID_POS)

/**
 * @brief Macro to get the Data Length from a PS_DataHeader_t::DataInfo field
 */
#define PS_GET_DATA_LENGTH(DataInfo) \
  ((DataInfo & PS_HEADER_DATA_LENGTH_MASK) >> PS_HEADER_DATA_LENGTH_POS)

/**
 * @brief Macro to set the Data Information of the PS_DataHeader_t::DataInfo field
 */
#define PS_MERGE_DATA_INFO(DataId, DataLength)                                        \
  (((DataId << PS_HEADER_DATA_ID_POS) & PS_HEADER_DATA_ID_MASK) |             \
   ((DataLength << PS_HEADER_DATA_LENGTH_POS) & PS_HEADER_DATA_LENGTH_MASK))

/**
 * @brief Macro to get the Data ID from a PS_BankHeader_t::BankInfo field
 */
#define PS_GET_CRC(BankInfo) \
  ((BankInfo & PS_HEADER_BANK_CRC_MASK) >> PS_HEADER_BANK_CRC_POS)

/**
 * @brief Macro to get the Data Length from a PS_BankHeader_t::BankInfo field
 */
#define PS_GET_ERASE_COUNTER(BankInfo) \
  ((BankInfo & PS_HEADER_BANK_ERASE_COUNTER_MASK) >> PS_HEADER_BANK_ERASE_COUNTER_POS)

/**
 * @brief Macro to set the Bank information of the PS_BankHeader_t::BankInfo field
 */
#define PS_MERGE_BANK_INFO(EraseCounter, Crc)                                                         \
  (((EraseCounter << PS_HEADER_BANK_ERASE_COUNTER_POS) & PS_HEADER_BANK_ERASE_COUNTER_MASK) | \
   ((Crc << PS_HEADER_BANK_CRC_POS) & PS_HEADER_BANK_CRC_MASK))

/**
 * @brief Get the sector ID of a given address
 */
#define PS_GET_SECTOR_ID(p_Addr, p_StartAddr)  \
  ((p_Addr - p_StartAddr) / PS_SECTOR_SIZE)

/**
 * @brief Check if source data has been written correctly
 */
/* -- 128bits flash line operations and definitions-------------------------- */
#if (PS_FLASH_LINE_LENGTH == PS_FLASH_LINE_128B)
#define PS_IS_FLASH_LINE_OK(p_DestAddr, p_SrcAddr)                        \
  ((*((uint32_t *)p_DestAddr)) == (*((uint32_t *)(p_SrcAddr)))                &&  \
   ((*(((uint32_t *)p_DestAddr) + 1u)) == (*(((uint32_t *)p_SrcAddr) + 1u)))  &&  \
   ((*(((uint32_t *)p_DestAddr) + 2u)) == (*(((uint32_t *)p_SrcAddr) + 2u)))  &&  \
   ((*(((uint32_t *)p_DestAddr) + 3u)) == (*(((uint32_t *)p_SrcAddr) + 3u))))
#else
#define PS_IS_FLASH_LINE_OK(p_DestAddr, p_SrcAddr)                        \
  ((*((uint32_t *)p_DestAddr)) == (*((uint32_t *)(p_SrcAddr)))                &&  \
   ((*(((uint32_t *)p_DestAddr) + 1u)) == (*(((uint32_t *)p_SrcAddr) + 1u))))
#endif /* (PS_FLASH_LINE_LENGTH == PS_FLASH_LINE_128B) */

/* Private typedef -----------------------------------------------------------*/
/**
 * @brief Enumeration of the different states possible for the module
 */
typedef enum PS_Module_State
{
  PS_INITIAL_STATE    = 0u, /* First state of the module, before any initialization */
  PS_WAITING_STATE    = 1u, /* Module is initialized and waiting for a command */
  PS_EXEC_STATE       = 2u, /* Module is executing a command */
  PS_PENDING_STATE    = 3u, /* Module has been interrupted during a command and waits for resume */
  PS_CLEAN_REQ_STATE  = 4u, /* Persistent Storage waits for a clean up operation */
} PS_Module_State_e;

/**
 * @brief Identifier for each module function
 *
 * @details This is used for resume mechanism
 */
typedef enum PS_FunctionId
{
  PS_INITOP_ID            = 0u,
  PS_WRITEOP_ID           = 1u,
  PS_RESTOREOP_ID         = 2u,
  PS_CLEANUPOP_ID         = 3u,
  PS_SEARCHDATA_ID        = 4u,
  PS_RECOVERVALIDDATA_ID  = 5u,
  PS_CONFIRMVALIDDATA_ID  = 6u,
  PS_ERASEBANK_ID         = 7u,
  PS_WRITEBANKHEADER_ID   = 8u,
  PS_WRITEDATA_ID         = 9u,
  PS_ORGANIZEBANKLIST_ID  = 10u,
  PS_CREATEBANKLIST_ID    = 11u,
  PS_SWAPBANK_ID          = 12u,
  PS_COPYDATA_ID          = 13u,
} PS_FunctionId_e;

/**
 * @brief Enumeration of the possible running operation
 */
typedef enum PS_RunningOperation
{
  PS_INIT_OP    = 0u,
  PS_WRITE_OP   = 1u,
  PS_RESTORE_OP = 2u,
  PS_CLEANUP_OP = 3u,
  PS_UNKNOWN_OP
} PS_RunningOperation_e;

/**
 * @brief Enumeration of the possible writing target
 */
typedef enum PS_WritingTarget
{
  PS_NO_BANK      = 0u,
  PS_NORMAL_BANK  = 1u,
  PS_SPARE_BANK   = 3u
} PS_WritingTarget_e;

/**
 * @brief Init operation states
 */
typedef enum PS_InitStates
{
  PS_INIT_FIND_VALID_HEADER   = 0u,
  PS_INIT_BANK_LIST_CREATION  = 1u,
  PS_INIT_DATA_MAPPING        = 2u,
  PS_INIT_CLEANUP             = 3u,
  PS_INIT_OP_OVER             = 4u,
} PS_InitStates_e;

/**
 * @brief Bank list creation states
 */
typedef enum PS_CreateBankListStates
{
  PS_CREATEBANKLIST_GATHER_INFORMATION                 = 0u,
  PS_CREATEBANKLIST_CLEAN_CORRUPTED_BANKS              = 1u,
  PS_CREATEBANKLIST_DETECT_SPARE_BANK_AND_WRITE_HEADER = 2u,
  PS_CREATEBANKLIST_SORT_BANK_LIST                     = 3u,
  PS_CREATEBANKLIST_OP_OVER                            = 4u,
} PS_CreateBankListStates_e;

/**
 * @brief Enumeration of the different states possible during a Write Data operation
 */
typedef enum PS_WriteDataStates
{
  PS_WRITEDATA_HEADER_WRITE     = 0u,
  PS_WRITEDATA_KERNEL_WRITE     = 1u,
  PS_WRITEDATA_TAIL_WRITE       = 2u,
  PS_WRITEDATA_OP_OVER          = 3u,
} PS_WriteDataStates_e;

/**
 * @brief Enumeration of the different states possible during a Write operation
 */
typedef enum PS_WriteStates
{
  PS_WRITE_GET_WRITE_BANK                   = 0u,
  PS_WRITE_DATA                             = 1u,
  PS_WRITE_ERROR_NORMAL_BANK                = 2u,
  PS_WRITE_ERROR_SPARE_BANK_ERASE           = 3u,
  PS_WRITE_ERROR_SPARE_BANK_WRITE_HEADER    = 4u,
  PS_WRITE_OP_OVER                          = 5u,
} PS_WriteStates_e;

/**
 * @brief Enumeration of the different states possible during a Restore operation
 */
typedef enum PS_RestoreStates
{
  PS_RESTORE_GET_BANK             = 0u,
  PS_RESTORE_SEARCH_HIT           = 1u,
  PS_RESTORE_CONFIRM_HIT          = 2u,
  PS_RESTORE_SEARCH_MISSING_DATA  = 3u,
} PS_RestoreStates_e;

/**
 * @brief Enumeration of the different states possible during a Cleanup operation
 */
typedef enum PS_CleanupStates
{
  PS_CLEANUP_BANK_SWAP                   = 0u,
  PS_CLEANUP_ERASE_FUTURE_SPARE_BANK     = 1u,
  PS_CLEANUP_WRITE_NEW_SPARE_BANK_HEADER = 2u,
  PS_CLEANUP_UPDATE_TRACKERS             = 3u,
  PS_CLEANUP_CHECK_AGING_LIMIT           = 4u,
  PS_CLEANUP_CHECK_OP_OVER               = 5u,
} PS_CleanupStates_e;

/**
 * @brief Enumeration of the different states possible during a Bank swap
 */
 typedef enum PS_BankSwapStates
 {
  PS_SWAPBANK_DATA_SEARCH                   = 0u,
  PS_SWAPBANK_DATA_WRITE                    = 1u,
  PS_SWAPBANK_SEARCH_MISSING_DATA           = 2u,
  PS_SWAPBANK_ERROR_SPARE_BANK_ERASE        = 3u,
  PS_SWAPBANK_ERROR_SPARE_BANK_WRITE_HEADER = 4u,
  PS_SWAPBANK_OP_OVER                       = 5u,
 }PS_BankSwapStates_e;

/**
 * @brief Header of a Data packet
 */
typedef struct __ALIGNED(8u) PS_DataHeader
{
  uint32_t DataInfo;                             /* Data information is composed like this:
                                                  *   + Data ID:      8 bits | Mask: 0xFF000000
                                                  *   + Data Length: 24 bits | Mask: 0x00FFFFFF
                                                  */
  uint32_t Crc;                                  /* CRC32 integrity check */
/* - 128 bits flash line header case --- */
#if (PS_FLASH_LINE_LENGTH == PS_FLASH_LINE_128B)
  uint8_t Data [PS_HEADER_DATA_NUMBER];  /* First data of the packet */
#endif /* (PS_FLASH_LINE_LENGTH == PS_FLASH_LINE_128B) */
} PS_DataHeader_t;

/**
 * @brief Header of a Bank
 */
typedef __PACKED_STRUCT __ALIGNED(8u) PS_BankHeader
{
  uint8_t Freshness;      /* Freshness Tracker of the bank - It is based on the global amount of erases performed */
  uint8_t BankId;         /* Id of the bank */
  uint8_t BankNumber;     /* Number of banks presents in this Persistent Storage */
  uint8_t SectorNumber;   /* Number of sectors presents in one bank */
  uint32_t BankInfo;      /* Bank information is composed like this
                           *   + Erase counter: 24 bits | Mask: 0xFFFFFF00
                           *   + CRC:            8 bits | Mask: 0x000000FF
                           */
/* - 128 bits flash line header case --- */
#if (PS_FLASH_LINE_LENGTH == PS_FLASH_LINE_128B)
  uint32_t Padding[4u];   /* Padding memory */
#endif /* (PS_FLASH_LINE_LENGTH == PS_FLASH_LINE_128B) */
} PS_BankHeader_t;

/**
 * @brief Structure to track evolutive information of a Bank
 */
typedef __PACKED_STRUCT PS_BankInformation
{
  uint32_t PresenceBitmask;       /* Bitmask to track the presence of a valid data - Each bit represent a data ID */
  uint32_t RemainingSpace;        /* Remaining space available in the bank - Bank header excluded */
  uint32_t * p_StartAddress;      /* Start address of the bank */
  uint32_t * p_NextWriteAddress;  /* Address of the next flash line to write */
} PS_BankInformation_t;

/* Private variables ---------------------------------------------------------*/
/**
 * @brief Number of erases performed all over the banks
 */
static uint8_t FreshnessValue = 0x00u;

/**
 * @brief Track the current state of context keeping in functions
 *
 * @details This mask is checked in each functions capable of being paused and resumed
 */
static uint32_t ContextKeepingMask = 0x00;

/**
 * @brief Track the current state of the module
 */
static PS_Module_State_e ModuleState = PS_INITIAL_STATE;

/**
 * @brief Track the current operation performed by the module
 *
 * @details Useful for Resume operation
 *
 */
static PS_RunningOperation_e CurrentOperation = PS_UNKNOWN_OP;

/**
 * @brief Current configuration of the Persistent Storage
 */
static PS_Configuration_t CurrentConfiguration = {0};

/**
 * @brief Valid data sizes table
 */
static uint32_t a_OccupationTable [PS_DATA_ID_MAX];

/**
 * @brief Bank information - For each bank
 */
static PS_BankInformation_t a_PSBanksInfo [PS_BANK_NUMBER_MAX] = {0};

/**
 * @brief Bank list to track freshness of banks
 *
 * @details The first element is the fresher bank.
 *          The last element is the spare bank.
 *          In between, the banks are classified in ascending order.
 */
static PS_BankInformation_t * ap_BankList [PS_BANK_NUMBER_MAX] = {0};

/**
 * @brief Empty flash line for comparisons
 */
#if (PS_FLASH_LINE_LENGTH == PS_FLASH_LINE_128B)
static const uint32_t a_EmptyFlashLine [(PS_FLASH_LINE_128B >> 2u)] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
#else
static const uint32_t a_EmptyFlashLine [(PS_FLASH_LINE_64B >> 2u)] = {0xFFFFFFFF, 0xFFFFFFFF};
#endif

/**
 * @brief Invalidated flash line for comparisons
 */
#if (PS_FLASH_LINE_LENGTH == PS_FLASH_LINE_128B)
static const uint32_t a_InvalidatedFlashLine [(PS_FLASH_LINE_128B >> 2u)] = {0x00000000, 0x00000000, 0x00000000, 0x00000000};
#else
static const uint32_t a_InvalidatedFlashLine [(PS_FLASH_LINE_64B >> 2u)] = {0x00000000, 0x00000000};
#endif

/* Global variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief Set the Context Keeping
 *
 * @param FunctionId: Id of the entered function
 *
 * @return None
 */
static inline void SetContextKeeping (const PS_FunctionId_e FunctionId);

/**
 * @brief Reset the Context Keeping
 *
 * @param FunctionId: Id of the entered function
 */
static inline void ResetContextKeeping (const PS_FunctionId_e FunctionId);

/**
 * @brief Indicate if context has to be kept or not
 *
 * @param FunctionId: Id of the entered function
 *
 * @return True if the context has to be kept
 */
static inline uint8_t IsContextToBeKept (const PS_FunctionId_e FunctionId);

/**
 * @brief Indicate if the amount of valid data is too much
 *
 * @param DataID: ID of the data to write
 * @param potentialDataSize: Size of the potential data to write
 *
 * @return True if the there are too many data in Persistent Storage
 */
static inline uint8_t IsValidDataOccupationTooMuch (const PS_Data_ID_e DataID,
                                                    const uint32_t potentialDataSize);

/**
 * @brief Compute 8 bits checksum on a buffer
 *
 * @param a_Buffer: Input buffer
 * @param BufferSize: Input buffer size
 * @param p_CrcEightValue: Computed value of the CRC8
 *
 * @return None
 */
static inline void ComputeCrcEight (const uint8_t a_Buffer [],
                                    const uint32_t BufferSize,
                                    uint8_t * const p_CrcEightValue);

/**
 * @brief Update Bank information
 *
 * @param DataID: ID of the new written data
 * @param NewBankIdx: Index of the new impacted bank
 * @param DataOccupation: Size of the data occupation in bytes - Header and padding included
 *
 * @return None
 */
static inline void UpdateBankInformation (const PS_Data_ID_e DataID,
                                          const uint32_t NewBankIdx,
                                          const uint32_t DataOccupation);

/**
 * @brief Get the index of the hosting bank of the given data ID
 *
 * @param DataID: ID of the data to search the hosting bank
 * @param StartingIdx: Starting index of the research
 * @param p_HostingBankIdx: Index of the bank hosting the data
 *
 * @return None
 */
static inline void GetHostingBank (const PS_Data_ID_e DataID,
                                   const uint32_t StartingIdx,
                                   uint32_t * const p_HostingBankIdx);

/**
 * @brief Get the Future Spare Bank index
 *
 * @param p_FutureSpareBankIdx: Future spare bank index in the bank list
 *
 * @return None
 */
static inline void GetFutureSpareBank (uint32_t * const p_FutureSpareBankIdx);

/**
 * @brief Get the Write bank index
 *
 * @param CallContext: Operation from where this function has been called
 * @param DataID: ID of the data to write
 * @param DataSize: Size of the data to write in bank
 * @param p_BankIdx: [In]: Index of the bank to start the research from.
 *                   [Out]: Index of the bank to write in.
 *
 * @return Type of the future target bank
 */
static inline PS_WritingTarget_e GetWriteBank (PS_RunningOperation_e CallContext,
                                                       const PS_Data_ID_e DataID,
                                                       const uint32_t DataSize,
                                                       uint32_t * const p_BankIdx);

/**
 * @brief Check if aging limit has been exceed
 *
 * @param p_AgingLimitTriggered: Aging limit flag. True: Aging limit triggered
 *
 * @return True if aging limit has been exceed
 */
static inline PS_Cmd_Status_e IsAgingLimitExceed (uint8_t * const p_AgingLimitTriggered);

/**
 * @brief Get the Newest Bank index
 *
 * @param p_NewestBankIdx: Newest bank index in the bank list
 *
 * @return State of the operation
 */
static inline PS_Cmd_Status_e GetNewestBank (uint32_t * const p_NewestBankIdx);

/**
 * @brief Invalidate a flash line
 *
 * @param p_InvalidAddr: Address of the flash line to invalidate
 *
 * @return State of the operation
 */
static inline PS_Cmd_Status_e InvalidateLine (uint32_t * const p_InvalidAddr);

/**
 * @brief Read a flash line
 *
 * @param p_ReadAddr: Flash address to read
 * @param p_FlashLineContent: Content of the read flash line
 *
 * @return State of the operation
 */
static inline PS_Cmd_Status_e ReadFlashLine (const uint32_t * const p_ReadAddr,
                                                     uint32_t * const p_FlashLineContent);

/**
 * @brief Check data integrity of a given data - Inside user memory
 *
 * @param p_DataHeader: Address of the data header to check integrity
 * @param p_DataContent: Address of the data content to check integrity
 *
 * @return State of the operation
 */
static inline  PS_Cmd_Status_e CheckDataIntegrity (const PS_DataHeader_t * const p_DataHeader,
                                                           const uint32_t * const p_DataContent);

/**
 * @brief Try to localize an existing Persistent Storage
 *
 * @param p_PSStartAddress: Start address of the research
 * @param p_PSLastAddress: End address of the research
 * @param p_FoundIdx: Index of the valid bank header - 0xFF if none found
 *
 * @return State of the operation
 */
static PS_Cmd_Status_e FindLocation (const uint32_t * const p_PSStartAddress,
                                             const uint32_t * const p_PSLastAddress,
                                             uint32_t * const p_FoundIdx);

/**
 * @brief Search from the indicated bank to the first, the presence of the missing data
 *
 * @param StartingIdx: Starting bank index
 * @param MissingDataBitmask: Bitmask of the missing data
 *
 * @return State of the operation
 */
static PS_Cmd_Status_e RecoverValidData (const uint32_t StartingIdx,
                                                 const uint32_t MissingDataBitmask);

/**
 * @brief Confirm the validity of a potential valid data
 *
 * @param p_PotentialValidData: Address of the valid data to confirm
 * @param p_DataDestination: Address to store the data
 * @param DataDestinationSize: Size available of the data destination
 * @param p_OccupationSize: Amount of bytes occupied by the data in memory
 *
 * @return PS_Cmd_Status_e
 */
static PS_Cmd_Status_e ConfirmValidData (const uint32_t * const p_PotentialValidData,
                                                 uint8_t * const p_DataDestination,
                                                 const uint32_t DataDestinationSize,
                                                 uint32_t * const p_OccupationSize);

/**
 * @brief Search in memory all the possible valid data and their hosting bank
 */
static PS_Cmd_Status_e BuildDataMap (void);

/**
 * @brief Sort the list in ascending order
 *
 * @details Spare bank is still placed at the end
 *
 * @param NewSpareBankIdx: Index of the new spare bank in the list
 *
 * @return None
 */
static PS_Cmd_Status_e OrganizeBankList (const uint32_t NewSpareBankIdx);

/**
 * @brief Provides a list of possibles data addresses
 *
 * @param DataID: ID of the data to search
 * @param BankListIdx: Index of the bank to search
 * @param p_LastPossibleAddress: Last possible address - Can be NULL if last possible address is the end of the bank
 * @param ap_DataAddress: Possible data addresses from oldest [0] to newest [2]
 *
 * @return State of the operation
 */
static PS_Cmd_Status_e SearchData (const PS_Data_ID_e DataID,
                                           const uint32_t BankListIdx,
                                           const uint32_t * const p_LastPossibleAddress,
                                           uint32_t * ap_DataAddress [3u]);

/**
 * @brief Create the Bank List
 *
 * @param StartingIndex: Starting index for research - Bank behind are supposed corrupted
 *
 * @return State of the operation
 */
static PS_Cmd_Status_e CreateBankList (const uint32_t StartingIndex);

/**
 * @brief Format and write the bank header of the given bank
 *
 * @param BankId: Id of the bank
 * @param EraseCounter: Erase counter value of the bank
 *
 * @return State of the operation
 */
static PS_Cmd_Status_e WriteBankHeader (const uint8_t BankId,
                                                const uint32_t EraseCounter);

/**
 * @brief Format and write the data in flash
 *
 * @param p_WriteAddress: Address of the memory area to write the data
 * @param DataID: ID of the data to write
 * @param p_DataSrc: Pointer onto the data source buffer
 * @param DataSize: Size of the data to write - in bytes
 * @param p_WrittenBytes: Number of bytes written
 *
 * @return State of the operation
 */
static PS_Cmd_Status_e WriteData (uint32_t * const p_WriteAddress,
                                          const PS_Data_ID_e DataID,
                                          const uint8_t * const p_DataSrc,
                                          const uint32_t DataSize,
                                          uint32_t * const p_WrittenBytes);

/**
 * @brief Copy to the valid data from the new spare bank to the current one
 *
 * @param NewSpareBankIdx: Index of the new spare bank in the bank list
 *
 * @return State of the operation
 */
static PS_Cmd_Status_e SwapBank (const uint32_t NewSpareBankIdx);

/**
 * @brief Erase the bank
 *
 * @param BankId: ID of the bank to erase - The given ID is the one of the ap_BankList
 *
 * @return State of the operation
 */
static PS_Cmd_Status_e EraseBank (const uint32_t BankId);

/**
  * @brief  Init operation
  *
  * @param p_Config: Configuration to apply for Persistent Storage
  *
  * @return State of the initialization
  */
static PS_Cmd_Status_e InitOp (const PS_Configuration_t * const p_Config);

/**
 * @brief Write operation
 *
 * @param DataID: ID of the data to write
 * @param p_DataSrc: Pointer onto the data source buffer
 * @param DataSize: Size of the data to write - in bytes
 *
 * @return State of the operation
 */
static PS_Cmd_Status_e WriteOp (const PS_Data_ID_e DataID,
                                        const uint8_t * const p_DataSrc,
                                        const uint32_t DataSize);

/**
 * @brief Restore operation
 *
 * @param DataID: ID of the data to restore
 * @param p_DataDest: Pointer onto the data destination buffer
 * @param DataSize: Maximum size of available data
 *
 * @return State of the operation
 */
static PS_Cmd_Status_e RestoreOp (const PS_Data_ID_e DataID,
                                          uint8_t * const p_DataDest,
                                          const uint32_t DataSize);

/**
 * @brief Cleanup operation
 *
 * @param AgingLimitTriggerFlag: Flag for Aging limit trigger - True: Aging limit has been overlapped
 *
 * @return State of the operation
 */
static PS_Cmd_Status_e CleanUpOp (const uint8_t AgingLimitTriggerFlag);

/**
 * @brief Check data validity of a given data - Inside Persistent Storage
 *
 * @param p_DataAddress: Address of the data to check validity
 *
 * @return State of the operation
 */
PS_Cmd_Status_e CheckDataValidity (const uint32_t * const p_DataAddress);

/**
 * @brief Copy a data from a memory area to another
 *
 * @param p_WriteAddress: Address of the memory area to write the data
 * @param p_DataAddr: Address of the data to copy
 * @param DataSize: Size of the data to write - in bytes
 * @param p_WrittenBytes: Number of bytes written
 *
 * @return State of the operation
 */
PS_Cmd_Status_e CopyData (uint32_t * const p_WriteAddress,
                                  const uint32_t * const p_DataAddr,
                                  const uint32_t DataSize,
                                  uint32_t * const p_WrittenBytes);

/* Functions Definition ------------------------------------------------------*/
PS_Cmd_Status_e PS_Init (const PS_Configuration_t * const p_Config)
{
  PS_Cmd_Status_e error = PS_UNKNOWN;

  /* Parameters checks */
  if (PS_PENDING_STATE == ModuleState)
  {
    error = PS_OPERATION_PAUSED;
  }
  else if (PS_CLEAN_REQ_STATE == ModuleState)
  {
    error = PS_CLEANUP_REQUIRED;
  }
  else if (PS_INITIAL_STATE != ModuleState)
  {
    error = PS_ERROR_ALREADY_INIT;
  }
  else if (NULL == p_Config)
  {
    error = PS_ERROR_NULL_POINTER;
  }
  else if ((PS_BANK_NUMBER_MIN > p_Config->BankNumber) ||
           (PS_BANK_NUMBER_MAX < p_Config->BankNumber))
  {
    error = PS_ERROR_CONFIG;
  }
  else if (0u == p_Config->BankSize)
  {
    error = PS_ERROR_CONFIG;
  }
  else if (0u == p_Config->AgingLimit)
  {
    error = PS_ERROR_CONFIG;
  }
  else if (NULL == p_Config->p_StartingAddress)
  {
    error = PS_ERROR_CONFIG;
  }
  else if (NULL == p_Config->p_SearchLimit)
  {
    error = PS_ERROR_CONFIG;
  }
  else if ((uint32_t *)((uint32_t)p_Config->p_StartingAddress + (p_Config->BankNumber *
                                                                 p_Config->BankSize *
                                                                 PS_SECTOR_SIZE)) > p_Config->p_SearchLimit)
  {
    error = PS_ERROR_CONFIG;
  }
  else
  {
    /* Initialize low level drivers: CRC */
    error = PS_CrcDriver.Init ();

    if (PS_OK == error)
    {
      /* Initialize low level drivers: Flash */
      error = PS_FlashDriver.Init ();

      if (PS_OK == error)
      {
        /* Setup state flags */
        CurrentOperation = PS_INIT_OP;

        /* Call private operation function */
        error = InitOp (p_Config);

        if (PS_OK == error)
        {
          /* Module is initialized, update current state */
          ModuleState = PS_WAITING_STATE;

          /* Update state flag */
          CurrentOperation = PS_UNKNOWN_OP;
        }
        /* Aging limit triggered */
        else if (PS_CLEANUP_REQUIRED == error)
        {
          /* Setup state flags */
          CurrentOperation = PS_CLEANUP_OP;

          /* A new clean operation is needed */
          error = CleanUpOp (TRUE);

          if (PS_OK == error)
          {
            /* Module is initialized, update current state */
            ModuleState = PS_WAITING_STATE;

            /* Update state flag */
            CurrentOperation = PS_UNKNOWN_OP;
          }
          else if (PS_OPERATION_PAUSED == error)
          {
            /* Setup context keeping mask */
            SetContextKeeping (PS_CLEANUPOP_ID);

            /* Module has been interrupted from operation */
            ModuleState = PS_PENDING_STATE;
          }
        }
        else if (PS_OPERATION_PAUSED == error)
        {
          /* Setup context keeping mask */
          SetContextKeeping (PS_INITOP_ID);

          /* Module has been interrupted from operation */
          ModuleState = PS_PENDING_STATE;
        }
      }
    }
  }

  return error;
}

PS_Cmd_Status_e PS_Write (const PS_Data_ID_e DataID,
                                          const uint8_t * const p_DataSrc,
                                          const uint32_t DataSize)
{
  PS_Cmd_Status_e error = PS_UNKNOWN;

  /* Parameters checks */
  if (PS_INITIAL_STATE == ModuleState)
  {
    error = PS_ERROR_NOT_INIT;
  }
  else if (PS_PENDING_STATE == ModuleState)
  {
    error = PS_OPERATION_PAUSED;
  }
  else if (PS_CLEAN_REQ_STATE == ModuleState)
  {
    error = PS_CLEANUP_REQUIRED;
  }
  else if (NULL == p_DataSrc)
  {
    error = PS_ERROR_NULL_POINTER;
  }
  else if (0u == DataSize)
  {
    error = PS_ERROR_DATA_SIZE;
  }
  else if (PS_DATA_ID_MAX <= DataID)
  {
    error = PS_ERROR_CONFIG;
  }
  else if (TRUE == IsValidDataOccupationTooMuch (DataID,
                                                 DataSize))
  {
    error = PS_ERROR_CONFIG;
  }
  else
  {
    /* Setup current module state */
    ModuleState = PS_EXEC_STATE;

    /* Setup state flags */
    CurrentOperation = PS_WRITE_OP;

    /* Call private operation function */
    error = WriteOp (DataID,
                     p_DataSrc,
                     DataSize);

    if (PS_OK == error)
    {
      /* Update current state */
      ModuleState = PS_WAITING_STATE;

      /* Update state flag */
      CurrentOperation = PS_UNKNOWN_OP;
    }
    else if (PS_CLEANUP_REQUIRED == error)
    {
      /* Update current state */
      ModuleState = PS_CLEAN_REQ_STATE;

      /* Update state flag */
      CurrentOperation = PS_UNKNOWN_OP;
    }
    else if (PS_OPERATION_PAUSED == error)
    {
      /* Setup context keeping mask */
      SetContextKeeping (PS_WRITEOP_ID);

      /* Module has been interrupted from operation */
      ModuleState = PS_PENDING_STATE;
    }
    else
    {
      /* Do nothing, error code should already be set */
    }
  }

  return error;
}

PS_Cmd_Status_e PS_Restore (const PS_Data_ID_e DataID,
                                            uint8_t * const p_DataDest,
                                            const uint32_t DataSize)
{
  PS_Cmd_Status_e error = PS_UNKNOWN;

  /* Parameters checks */
  if (PS_INITIAL_STATE == ModuleState)
  {
    error = PS_ERROR_NOT_INIT;
  }
  else if (PS_PENDING_STATE == ModuleState)
  {
    error = PS_OPERATION_PAUSED;
  }
  else if (PS_CLEAN_REQ_STATE == ModuleState)
  {
    error = PS_CLEANUP_REQUIRED;
  }
  else if (NULL == p_DataDest)
  {
    error = PS_ERROR_NULL_POINTER;
  }
  else if (PS_DATA_ID_MAX <= DataID)
  {
    error = PS_ERROR_CONFIG;
  }
  else if (0u == DataSize)
  {
    error = PS_ERROR_DATA_SIZE;
  }
  else
  {
    /* Setup current module state */
    ModuleState = PS_EXEC_STATE;

    /* Setup state flags */
    CurrentOperation = PS_RESTORE_OP;

    /* Call private operation function */
    error = RestoreOp (DataID,
                       p_DataDest,
                       DataSize);

    if (PS_OK == error)
    {
      /* Update current state */
      ModuleState = PS_WAITING_STATE;

      /* Update state flag */
      CurrentOperation = PS_UNKNOWN_OP;
    }
    else if (PS_OPERATION_PAUSED == error)
    {
      /* Setup context keeping mask */
      SetContextKeeping (PS_RESTOREOP_ID);

      /* Module has been interrupted from operation */
      ModuleState = PS_PENDING_STATE;
    }
    else
    {
      /* Do nothing, error code shall already be set */
    }
  }

  return error;
}

PS_Cmd_Status_e PS_CleanUp (void)
{
  PS_Cmd_Status_e error = PS_UNKNOWN;

  /* Parameters checks */
  if (PS_INITIAL_STATE == ModuleState)
  {
    error = PS_ERROR_NOT_INIT;
  }
  else if (PS_PENDING_STATE == ModuleState)
  {
    error = PS_OPERATION_PAUSED;
  }
  else if (PS_CLEAN_REQ_STATE != ModuleState)
  {
    error = PS_NOK;
  }
  else
  {
    /* Setup current module state */
    ModuleState = PS_EXEC_STATE;

    /* Setup state flags */
    CurrentOperation = PS_CLEANUP_OP;

    /* Call private operation function */
    error = CleanUpOp (FALSE);

    if (PS_OK == error)
    {
      /* Update current state */
      ModuleState = PS_WAITING_STATE;

      /* Update state flag */
      CurrentOperation = PS_UNKNOWN_OP;
    }
    /* Aging limit triggered */
    else if (PS_CLEANUP_REQUIRED == error)
    {
      /* A new clean operation is needed */
      error = CleanUpOp (TRUE);

      if (PS_OK == error)
      {
        /* Module is initialized, update current state */
        ModuleState = PS_WAITING_STATE;

        /* Update state flag */
        CurrentOperation = PS_UNKNOWN_OP;
      }
      else if (PS_OPERATION_PAUSED == error)
      {
        /* Setup context keeping mask */
        SetContextKeeping (PS_CLEANUPOP_ID);

        /* Module has been interrupted from operation */
        ModuleState = PS_PENDING_STATE;
      }
    }
    else if (PS_OPERATION_PAUSED == error)
    {
      /* Setup context keeping mask */
      SetContextKeeping (PS_CLEANUPOP_ID);

      /* Module has been interrupted from operation */
      ModuleState = PS_PENDING_STATE;
    }
    else
    {
      /* Do nothing, error code shall already be set */
    }
  }

  return error;
}

PS_Cmd_Status_e PS_Resume (void)
{
  PS_Cmd_Status_e error = PS_UNKNOWN;

  /* Parameters checks */
  if (PS_INITIAL_STATE == ModuleState)
  {
    error = PS_ERROR_NOT_INIT;
  }
  else if (PS_CLEAN_REQ_STATE == ModuleState)
  {
    error = PS_CLEANUP_REQUIRED;
  }
  else if (PS_PENDING_STATE != ModuleState)
  {
    error = PS_NOK;
  }
  else
  {
    /* Resume the right operation - With dummy parameters */
    switch (CurrentOperation)
    {
      case PS_INIT_OP:
      {
        /* Resume init operation - With dummy parameters */
        error = InitOp (NULL);

        if (PS_OK == error)
        {
          /* Module is initialized, update current state */
          ModuleState = PS_WAITING_STATE;

          /* Update state flag */
          CurrentOperation = PS_UNKNOWN_OP;
        }
        /* Aging limit triggered */
        else if (PS_CLEANUP_REQUIRED == error)
        {
          /* Setup state flags */
          CurrentOperation = PS_CLEANUP_OP;

          /* A new clean operation is needed */
          error = CleanUpOp (TRUE);

          if (PS_OK == error)
          {
            /* Module is initialized, update current state */
            ModuleState = PS_WAITING_STATE;

            /* Update state flag */
            CurrentOperation = PS_UNKNOWN_OP;
          }
          else if (PS_OPERATION_PAUSED == error)
          {
            /* Setup context keeping mask */
            SetContextKeeping (PS_CLEANUPOP_ID);
          }
          else
          {
            /* Do nothing, error code should already be set */
          }
        }
        else if (PS_OPERATION_PAUSED == error)
        {
          /* Setup context keeping mask */
          SetContextKeeping (PS_INITOP_ID);
        }
        else
        {
          /* Do nothing, error code should already be set */
        }

        break;
      }

      case PS_WRITE_OP:
      {
        /* Resume the right operation - With dummy parameters */
        error = WriteOp (PS_DATA_ID_MAX,
                         NULL,
                         0x00);

        if (PS_OK == error)
        {
          /* Module is initialized, update current state */
          ModuleState = PS_WAITING_STATE;

          /* Update state flag */
          CurrentOperation = PS_UNKNOWN_OP;
        }
        else if (PS_CLEANUP_REQUIRED == error)
        {
          /* Update current state */
          ModuleState = PS_CLEAN_REQ_STATE;

          /* Update state flag */
          CurrentOperation = PS_UNKNOWN_OP;
        }
        else if (PS_OPERATION_PAUSED == error)
        {
          /* Setup context keeping mask */
          SetContextKeeping (PS_WRITEOP_ID);
        }
        else
        {
          /* Do nothing, error code should already be set */
        }

        break;
      }

      case PS_RESTORE_OP:
      {
        /* Resume the right operation - With dummy parameters */
        error = RestoreOp (PS_DATA_ID_MAX,
                           NULL,
                           0x00);

        if (PS_OK == error)
        {
          /* Module is initialized, update current state */
          ModuleState = PS_WAITING_STATE;

          /* Update state flag */
          CurrentOperation = PS_UNKNOWN_OP;
        }
        else if (PS_OPERATION_PAUSED == error)
        {
          /* Setup context keeping mask */
          SetContextKeeping (PS_RESTOREOP_ID);

          /* Module has been interrupted from operation */
          ModuleState = PS_PENDING_STATE;
        }
        else
        {
          /* Do nothing, error code should already be set */
        }

        break;
      }

      case PS_CLEANUP_OP:
      {
        /* Resume the right operation - With dummy parameters */
        error = CleanUpOp (FALSE);

        if (PS_OK == error)
        {
          /* Module is initialized, update current state */
          ModuleState = PS_WAITING_STATE;

          /* Update state flag */
          CurrentOperation = PS_UNKNOWN_OP;
        }
        /* Aging limit triggered */
        else if (PS_CLEANUP_REQUIRED == error)
        {
          /* A new clean operation is needed */
          error = CleanUpOp (TRUE);

          if (PS_OK == error)
          {
            /* Module is initialized, update current state */
            ModuleState = PS_WAITING_STATE;

            /* Update state flag */
            CurrentOperation = PS_UNKNOWN_OP;
          }
          else if (PS_OPERATION_PAUSED == error)
          {
            /* Setup context keeping mask */
            SetContextKeeping (PS_CLEANUPOP_ID);
          }
          else
          {
            /* Do nothing, error code should already be set */
          }
        }
        else if (PS_OPERATION_PAUSED == error)
        {
          /* Setup context keeping mask */
          SetContextKeeping (PS_CLEANUPOP_ID);
        }
        else
        {
          /* Do nothing, error code should already be set */
        }

        break;
      }

      default:
      {
        break;
      }
    }
  }

  return error;
}

void PS_EccCorrectionCallback (uint32_t * const p_ErrorAddr)
{
  /* TODO: Determine if really needed since Correction should already be done */
}

void PS_EccDetectionCallback (uint32_t * const p_ErrorAddr)
{
  /* Invalidate the flash line */
  (void)InvalidateLine (p_ErrorAddr);
}

/* Private function Definition -----------------------------------------------*/

void SetContextKeeping (const PS_FunctionId_e FunctionId)
{
  ContextKeepingMask |= (1u << (uint8_t)FunctionId);
}

void ResetContextKeeping (const PS_FunctionId_e FunctionId)
{
  ContextKeepingMask &= ~(1u << (uint8_t)FunctionId);
}

uint8_t IsContextToBeKept (const PS_FunctionId_e FunctionId)
{
  uint8_t error = FALSE;

  if (0u != (ContextKeepingMask & (1u << (uint8_t)FunctionId)))
  {
    error = TRUE;
  }

  return error;
}

uint8_t IsValidDataOccupationTooMuch (const PS_Data_ID_e DataID,
                                      const uint32_t potentialDataSize)
{
  uint8_t error = FALSE;

  uint32_t validDataAmount = 0x00;
  uint32_t realDataSize = 0x00;

  /* Compute real data size occupation */
  realDataSize = potentialDataSize +
                 sizeof (PS_DataHeader_t) -
                 PS_HEADER_DATA_NUMBER;

  realDataSize = (uint32_t)PS_ALIGN_FLASH_LINE (realDataSize);

  /* For each bank, compute the space occupied by valid data */
  for (uint32_t bankIdx = 0x00;
       (CurrentConfiguration.BankNumber - 1u) > bankIdx;
       bankIdx++)
  {
    /* Bank header size occupation */
    validDataAmount = sizeof (PS_BankHeader_t);

    /* Data size occupation */
    for (uint32_t dataIdx = 0x00;
         PS_DATA_ID_MAX > dataIdx;
         dataIdx++)
    {
      if ((ap_BankList[bankIdx]->PresenceBitmask & (1u << dataIdx)) &&
          (DataID != dataIdx))
      {
        validDataAmount += a_OccupationTable[dataIdx];
      }
    }

    /* Check if the potential data can fit in the bank */
    if (((CurrentConfiguration.BankSize * PS_SECTOR_SIZE) - validDataAmount) >= realDataSize)
    {
      error = FALSE;

      /* Data can fit - Leave the loop */
      break;
    }
    else
    {
      /* Data would not fit this bank - Keep going */
      error = TRUE;
    }
  }

  return error;
}

void ComputeCrcEight (const uint8_t a_Buffer [],
                      const uint32_t BufferSize,
                      uint8_t * const p_CrcEightValue)
{
  *p_CrcEightValue = 0x00;

  for (uint32_t idx = 0x00;
       BufferSize > idx;
       idx++)
  {
    *p_CrcEightValue ^= a_Buffer[idx];

    for (uint32_t cnt = 0x00;
         8u > cnt;
         cnt++)
    {
      if (*p_CrcEightValue & 0x80)
      {
        *p_CrcEightValue = (*p_CrcEightValue << 1u) ^ PS_CRC8_POLY;
      }
      else
      {
        *p_CrcEightValue <<= 1u;
      }
    }
  }
}

void UpdateBankInformation (const PS_Data_ID_e DataID,
                            const uint32_t NewBankIdx,
                            const uint32_t DataOccupation)
{
  uint32_t hostingBankIdx = 0xFF;

  /* Now search the old hosting bank */
  GetHostingBank (DataID,
                  NewBankIdx,
                  &hostingBankIdx);

  /* Check if the last hosting bank was different from the new one */
  if ((NewBankIdx != hostingBankIdx) && (0xFF != hostingBankIdx))
  {
    /* Clear the old hosting bank presence bitmask */
    ap_BankList[hostingBankIdx]->PresenceBitmask = ap_BankList[hostingBankIdx]->PresenceBitmask & ~(1u << DataID);
  }

  /* Update presence bitmask */
  ap_BankList[NewBankIdx]->PresenceBitmask = ap_BankList[NewBankIdx]->PresenceBitmask | (1u << DataID);

  /* Update bank remaining space */
  ap_BankList[NewBankIdx]->RemainingSpace = ap_BankList[NewBankIdx]->RemainingSpace - DataOccupation;

  /* Update next write address */
  ap_BankList[NewBankIdx]->p_NextWriteAddress = (uint32_t *)((uint32_t)(ap_BankList[NewBankIdx]->p_NextWriteAddress) +
                                                                        DataOccupation);
}

void GetHostingBank (const PS_Data_ID_e DataID,
                     const uint32_t StartingIdx,
                     uint32_t * const p_HostingBankIdx)
{
  uint32_t bankIdx = StartingIdx;

  for (uint32_t cnt = 0x00;
       CurrentConfiguration.BankNumber > cnt;
       cnt++)
  {
    /* Check if last data was here before */
    if (0x00 != (ap_BankList[bankIdx]->PresenceBitmask & (1u << DataID)))
    {
      *p_HostingBankIdx = bankIdx;

      /* Leave the loop */
      break;
    }

    /* A wrap has been performed, restart from the beginning of the list */
    if (0x00 == bankIdx)
    {
      bankIdx = (CurrentConfiguration.BankNumber - 1u);
    }
    else
    {
      bankIdx--;
    }
  }
}

void GetFutureSpareBank (uint32_t * const p_FutureSpareBankIdx)
{
  uint32_t validDataAmount = 0x00;
  uint32_t currentFutureBankAmount = 0xFFFFFFFF;

  /* Loop over the banks and determines which bank can be the new spare bank */
  for (uint32_t cnt = 0x00;
       (CurrentConfiguration.BankNumber - 1u) > cnt;
       cnt++)
  {
    /* Reset amount before computation */
    validDataAmount = 0x00;

    /* Data ID 0-9 */
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_0_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_0];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_1_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_1];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_2_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_2];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_3_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_3];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_4_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_4];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_5_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_5];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_6_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_6];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_7_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_7];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_8_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_8];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_9_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_9];
    }

    /* Data ID 10-19 */
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_10_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_10];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_11_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_11];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_12_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_12];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_13_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_13];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_14_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_14];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_15_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_15];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_16_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_16];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_17_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_17];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_18_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_18];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_19_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_19];
    }

    /* Data ID 20-29 */
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_20_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_20];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_21_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_21];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_22_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_22];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_23_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_23];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_24_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_24];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_25_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_25];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_26_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_26];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_27_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_27];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_28_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_28];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_29_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_29];
    }

    /* Data ID 30-31 */
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_30_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_30];
    }
    if (ap_BankList[cnt]->PresenceBitmask & PS_DATA_ID_31_MASK)
    {
      validDataAmount += a_OccupationTable[PS_DATA_ID_31];
    }

    /* Check if there is less or more valid data inside this bank */
    if (currentFutureBankAmount > validDataAmount)
    {
      currentFutureBankAmount = validDataAmount;

      *p_FutureSpareBankIdx = cnt;
    }
  }
}

PS_WritingTarget_e GetWriteBank (PS_RunningOperation_e CallContext,
                                         const PS_Data_ID_e DataID,
                                         const uint32_t DataSize,
                                         uint32_t * const p_BankIdx)
{
  PS_WritingTarget_e error = PS_NORMAL_BANK;

  uint32_t cnt = 0x00;
  uint32_t bankIdx = 0x00;
  uint32_t dataOccupation = 0x00;

  /* ---- Starting search point setting --------------------------------------*/
  /* Starting research point depending on the calling operation */
  switch (CallContext)
  {
    case PS_WRITE_OP:
    {
      /* Check if first time write by searching a previous version of the data */
      (void)GetHostingBank (DataID,
                            (CurrentConfiguration.BankNumber - 2u),
                            &bankIdx);

      break;
    }

    case PS_CLEANUP_OP:
    {
      /* Set the starting point */
      bankIdx = *p_BankIdx;
      break;
    }

    default:
    {
      error = PS_NO_BANK;
      break;
    }
  }

  /* ---- Searching space in Normal banks ------------------------------------*/
  /* Compute data occupation */
  dataOccupation = ((DataSize - PS_HEADER_DATA_NUMBER) +
                    sizeof (PS_DataHeader_t) +
                    (PS_FLASH_LINE_LENGTH - PS_COMPUTE_MODULO(DataSize,
                                                                              PS_FLASH_LINE_LENGTH)));

  dataOccupation = (uint32_t)PS_ALIGN_FLASH_LINE((DataSize + sizeof (PS_DataHeader_t) - PS_HEADER_DATA_NUMBER));

  /* Loop across all the banks to find a possible spot - Loop over all the Normal banks */
  while ((CurrentConfiguration.BankNumber - 1u) > cnt)
  {
    /* Is there enough space inside this bank ? */
    if (ap_BankList[bankIdx]->RemainingSpace >= dataOccupation)
    {
      /* Update the write address */
      *p_BankIdx = bankIdx;

      /* Leave the loop */
      break;
    }
    /* Keep going */
    else
    {
      /* End of the normal banks ? */
      if ((CurrentConfiguration.BankNumber - 1u) == (bankIdx + 1u))
      {
        bankIdx = 0x00;
      }
      else
      {
        bankIdx++;
      }
    }

    /* Update counter */
    cnt++;
  }

  /* ---- Spare bank attribution ---------------------------------------------*/
  /* Check if a write in SpareBank is required - All normal banks do not have enough space */
  if ((CurrentConfiguration.BankNumber - 1u) <= cnt)
  {
    /* Update the write address */
    *p_BankIdx = (CurrentConfiguration.BankNumber - 1u);

    /* Update error code */
    error = PS_SPARE_BANK;
  }

  return error;
}

PS_Cmd_Status_e GetNewestBank (uint32_t * const p_NewestBankIdx)
{
  PS_Cmd_Status_e error = PS_OK;

  uint32_t tmpEraseCounter = 0xFFFFFFFF;

  PS_BankHeader_t tmpBankHeader = {0xFFu};

  /* Loop over the banks and determines which bank is the newest */
  for (uint32_t cnt = 0x00;
       ((CurrentConfiguration.BankNumber - 1u) > cnt) &&
       (PS_OK == error);
       cnt++)
  {
    error = ReadFlashLine (ap_BankList[cnt]->p_StartAddress,
                           (uint32_t *)&tmpBankHeader);

    if (PS_OK == error)
    {
      if (tmpEraseCounter > PS_GET_ERASE_COUNTER (tmpBankHeader.BankInfo))
      {
        tmpEraseCounter = PS_GET_ERASE_COUNTER (tmpBankHeader.BankInfo);

        *p_NewestBankIdx = cnt;
      }
    }
  }

  return error;
}

PS_Cmd_Status_e IsAgingLimitExceed (uint8_t * const p_AgingLimitTriggered)
{
  PS_Cmd_Status_e error = PS_OK;

  uint32_t minIdx = 0x00;
  uint32_t spareBankErase = 0x00;

  PS_BankHeader_t tmpBankHeader = {0xFF};

  /* Get the newest bank */
  error = GetNewestBank (&minIdx);

  if (PS_OK == error)
  {
    error = ReadFlashLine (ap_BankList[(CurrentConfiguration.BankNumber - 1u)]->p_StartAddress,
                           (uint32_t *)&tmpBankHeader);

    if (PS_OK == error)
    {
      spareBankErase = PS_GET_ERASE_COUNTER (tmpBankHeader.BankInfo);

      error = ReadFlashLine (ap_BankList[minIdx]->p_StartAddress,
                             (uint32_t *)&tmpBankHeader);

      if (PS_OK == error)
      {
        uint32_t minBankErase = PS_GET_ERASE_COUNTER (tmpBankHeader.BankInfo);

        /* Check if Aging limit has been triggered */
        if ((spareBankErase >= minBankErase) &&
            (CurrentConfiguration.AgingLimit <= (spareBankErase - minBankErase)))
        {
          *p_AgingLimitTriggered = TRUE;
        }
      }
    }
  }

  return error;
}

PS_Cmd_Status_e ReadFlashLine (const uint32_t * const p_ReadAddr,
                                       uint32_t * const p_FlashLineContent)
{
  /* Read address is directly accessible */
  return PS_FlashDriver.Read (p_ReadAddr,
                                      p_FlashLineContent,
                                      PS_FLASH_LINE_LENGTH);
}

PS_Cmd_Status_e InvalidateLine (uint32_t * const p_InvalidAddr)
{
  PS_Cmd_Status_e error = PS_UNKNOWN;

  /* Invalidate the invalid flash line */
  error = PS_FlashDriver.Write (p_InvalidAddr, a_InvalidatedFlashLine);

  return error;
}

PS_Cmd_Status_e CheckDataIntegrity (const PS_DataHeader_t * const p_DataHeader,
                                            const uint32_t * const p_DataContent)
{
  PS_Cmd_Status_e error = PS_UNKNOWN;

  uint32_t crcValue = 0x00;

  /* Stat crc computation with Data Info */
  error = PS_CrcDriver.Calculate (&(p_DataHeader->DataInfo),
                                          PS_DATA_HEADER_DATA_INFO_SIZE,
                                          &crcValue);

  if ((PS_OK == error))
  {
    /* Compute the CRC value */
    error = PS_CrcDriver.Accumulate (p_DataContent,
                                             PS_GET_DATA_LENGTH (p_DataHeader->DataInfo),
                                             &crcValue);

    /* Check CRC operation result */
    if (PS_OK == error)
    {
      if (p_DataHeader->Crc != crcValue)
      {
        error = PS_CRC_ERROR;
      }
    }
  }

  return error;
}

PS_Cmd_Status_e FindLocation (const uint32_t * const p_PSStartAddress,
                                      const uint32_t * const p_PSLastAddress,
                                      uint32_t * const p_FoundIdx)
{
  PS_Cmd_Status_e error = PS_OK;

  uint8_t headerCrc = PS_CRC8_RESET_VALUE;
  int32_t bankIdx = 0xFF;

  PS_BankHeader_t tmpBankHeader = {0xFFu};

  /* Search for a header */
  for (uint32_t * p_tmpAddr = PS_ALIGN_FLASH_LINE (p_PSStartAddress);
       ((PS_ALIGN_FLASH_LINE (p_PSLastAddress) > p_tmpAddr) &&
        (PS_OK == error));
       p_tmpAddr = (uint32_t *)((uint32_t)p_tmpAddr + PS_SECTOR_SIZE))
  {
    /* Get the bank header */
    error = ReadFlashLine (p_tmpAddr,
                           (uint32_t *)&tmpBankHeader);

    if (PS_OK == error)
    {
      /* Compute the crc of the supposed header */
      ComputeCrcEight ((uint8_t *)&tmpBankHeader,
                       PS_HEADER_BANK_SIZE_WO_CRC,
                       &headerCrc);

      /* Check if the crc does match the header value */
      if (headerCrc == PS_GET_CRC (tmpBankHeader.BankInfo))
      {
        /* A header has been found, update the Persistent Storage configuration */
        CurrentConfiguration.BankNumber = tmpBankHeader.BankNumber;
        CurrentConfiguration.BankSize = tmpBankHeader.SectorNumber;
        FreshnessValue = tmpBankHeader.Freshness;

        /* Update the starting address of each banks - Upper part */
        bankIdx = (tmpBankHeader.BankId + 0x01);

        while (CurrentConfiguration.BankNumber > bankIdx)
        {
          /* Update bank info */
          a_PSBanksInfo[bankIdx].p_StartAddress = (uint32_t *)((uint32_t)p_tmpAddr +
                                                                            (bankIdx - tmpBankHeader.BankId) *
                                                                            (CurrentConfiguration.BankSize *
                                                                             PS_SECTOR_SIZE));

          /* Initialize also the bank list */
          ap_BankList[bankIdx] = &a_PSBanksInfo[bankIdx];

          bankIdx++;
        }

        /* Update the starting address of each banks - Lower part */
        bankIdx = tmpBankHeader.BankId;

        while (0x00 <= bankIdx)
        {
          /* Update bank info */
          a_PSBanksInfo[bankIdx].p_StartAddress = (uint32_t *)((uint32_t)p_tmpAddr -
                                                                            (tmpBankHeader.BankId - bankIdx) *
                                                                            (CurrentConfiguration.BankSize *
                                                                             PS_SECTOR_SIZE));

          /* Initialize also the bank list */
          ap_BankList[bankIdx] = &a_PSBanksInfo[bankIdx];

          bankIdx--;
        }

        /* Return found index ... */
        *p_FoundIdx = tmpBankHeader.BankId;

        /* ... and leave the loop */
        break;
      }
    }
  }

  return error;
}

PS_Cmd_Status_e SearchData (const PS_Data_ID_e DataID,
                                    const uint32_t BankListIdx,
                                    const uint32_t * const p_LastPossibleAddress,
                                    uint32_t * ap_DataAddress [3u])
{
  PS_Cmd_Status_e error = PS_DATA_NOT_FOUND;

  PS_DataHeader_t readHeader = {0u};

  static uint32_t * a_possibleDataAddr [3u] = {NULL, NULL, NULL};
  static uint32_t * p_searchAddr = NULL;
  static const uint32_t * p_endAddr = NULL;

  if (FALSE == IsContextToBeKept (PS_SEARCHDATA_ID))
  {
    /* Reset possible data addresses */
    a_possibleDataAddr[0u] = NULL;
    a_possibleDataAddr[1u] = NULL;
    a_possibleDataAddr[2u] = NULL;

    ap_DataAddress[0u] = NULL;
    ap_DataAddress[1u] = NULL;
    ap_DataAddress[2u] = NULL;

    /* Set the starting point */
    p_searchAddr = (uint32_t *)((uint32_t)ap_BankList[BankListIdx]->p_StartAddress +
                                          PS_FLASH_LINE_LENGTH);

    /* Set the ending point */
    if (NULL != p_LastPossibleAddress)
    {
      p_endAddr = p_LastPossibleAddress;
    }
    else
    {
      p_endAddr = ap_BankList[BankListIdx]->p_NextWriteAddress;
    }
  }
  else
  {
    /* Reset context keeping mask */
    ResetContextKeeping (PS_SEARCHDATA_ID);
  }

  /* Search the data */
  while ((p_endAddr > p_searchAddr) &&
         (PS_DATA_NOT_FOUND == error))
  {
    /* Reset before read the header */
    memset ((void *)&readHeader,
            0xFF,
            sizeof (PS_DataHeader_t));

    /* Read the flash line */
    error = ReadFlashLine (p_searchAddr,
                           (uint32_t *)&readHeader);

    if (PS_OK == error)
    {
      /* Check if the data header is empty */
      if (PS_IS_FLASH_LINE_OK (&readHeader, a_EmptyFlashLine))
      {
        /* Restrict bank scope as it might be corrupted */
        ap_BankList[BankListIdx]->p_NextWriteAddress = p_searchAddr;
        ap_BankList[BankListIdx]->RemainingSpace = 0x00;

        /* Invalid Data found */
        error = PS_ERROR_INVALID;
      }
      /* Check if the data header is invalid */
      else if (PS_IS_FLASH_LINE_OK (&readHeader, a_InvalidatedFlashLine))
      {
        /* Restrict bank scope as it might be corrupted */
        ap_BankList[BankListIdx]->p_NextWriteAddress = p_searchAddr;
        ap_BankList[BankListIdx]->RemainingSpace = 0x00;

        /* Invalid Data found */
        error = PS_ERROR_INVALID;
      }
      /* Check the data ID */
      else if (PS_DATA_ID_MAX <= PS_GET_DATA_ID (readHeader.DataInfo))
      {
        /* Restrict bank scope as it might be corrupted */
        ap_BankList[BankListIdx]->p_NextWriteAddress = p_searchAddr;
        ap_BankList[BankListIdx]->RemainingSpace = 0x00;

        /* Invalid Data found */
        error = PS_ERROR_INVALID;
      }
      else
      {
        if (PS_GET_DATA_ID (readHeader.DataInfo) == DataID)
        {
          /* A data with the requested data ID has been found, store up to three candidates */
          a_possibleDataAddr[0u] = a_possibleDataAddr[1u];
          a_possibleDataAddr[1u] = a_possibleDataAddr[2u];
          a_possibleDataAddr[2u] = p_searchAddr;
        }

        /* Jump to the next data */
        p_searchAddr = (uint32_t *)((uint32_t)p_searchAddr +
                                              PS_GET_DATA_LENGTH (readHeader.DataInfo) -
                                              PS_HEADER_DATA_NUMBER +
                                              PS_FLASH_LINE_LENGTH);

        /* Align jump address */
        p_searchAddr = PS_ALIGN_FLASH_LINE (p_searchAddr);

        /* Keep searching */
        error = PS_DATA_NOT_FOUND;
      }
    }
    else
    {
      /* Do nothing, error code should already be set */
    }
  }

  /* Once research is over, return possible addresses from oldest to newest */
  if ((PS_DATA_NOT_FOUND == error) &&
      ((NULL != a_possibleDataAddr[0u]) ||
       (NULL != a_possibleDataAddr[1u]) ||
       (NULL != a_possibleDataAddr[2u])))
  {
    ap_DataAddress[0u] = a_possibleDataAddr[0u];
    ap_DataAddress[1u] = a_possibleDataAddr[1u];
    ap_DataAddress[2u] = a_possibleDataAddr[2u];
    error = PS_OK;
  }

  return error;
}

PS_Cmd_Status_e RecoverValidData (const uint32_t StartingIdx,
                                          const uint32_t MissingDataBitmask)
{
  PS_Cmd_Status_e error = PS_OK;

  uint8_t currentDataId = 0xFF;
  uint32_t currentDataSize = 0x00;

  uint32_t * p_tmpAddress = NULL;
  uint32_t * p_endAddress = NULL;

  PS_DataHeader_t tmpDataHeader = {0xFF};

  static int32_t bankCnt = 0x00;
  static uint32_t dataFoundMask = 0x00;

  if (FALSE == IsContextToBeKept (PS_RECOVERVALIDDATA_ID))
  {
    /* Reset found flags */
    dataFoundMask = 0x00;

    /* Starting from the most recent bank */
    bankCnt = StartingIdx;
  }
  else
  {
    /* Reset context keeping mask */
    ResetContextKeeping (PS_RECOVERVALIDDATA_ID);
  }

  while ((0x00 <= bankCnt) && (MissingDataBitmask != dataFoundMask) && (PS_OK == error))
  {
    /* Set bonding pointers */
    p_tmpAddress = (uint32_t *)((uint32_t)ap_BankList[bankCnt]->p_StartAddress +
                   PS_FLASH_LINE_LENGTH);
    p_endAddress = (uint32_t *)((uint32_t)ap_BankList[bankCnt]->p_StartAddress +
                   (CurrentConfiguration.BankSize * PS_SECTOR_SIZE));

    /* Loop while there is data present */
    while ((p_endAddress > p_tmpAddress) &&
           (PS_OK == error))
    {
      error = ReadFlashLine (p_tmpAddress,
                             (uint32_t *)&tmpDataHeader);

      if (PS_OK == error)
      {
        /* Check if the data header is empty */
        if (PS_IS_FLASH_LINE_OK (&tmpDataHeader, a_EmptyFlashLine))
        {
          /* Update next write pointer */
          ap_BankList[bankCnt]->p_NextWriteAddress = p_tmpAddress;

          /* Update remaining space */
          ap_BankList[bankCnt]->RemainingSpace = ((uint32_t)p_endAddress - (uint32_t)p_tmpAddress);

          /* No more data, leave the loop */
          p_endAddress = p_tmpAddress;
        }
        /* Check if the data header is invalid */
        else if (PS_IS_FLASH_LINE_OK (&tmpDataHeader, a_InvalidatedFlashLine))
        {
          /* Restrict bank scope as it might be corrupted */
          ap_BankList[bankCnt]->p_NextWriteAddress = p_tmpAddress;

          /* Update remaining space */
          ap_BankList[bankCnt]->RemainingSpace = 0x00;

          /* Update loop leaving condition */
          p_endAddress = p_tmpAddress;
        }
        /* Check if header data ID is NOK */
        else if (PS_DATA_ID_MAX <= PS_GET_DATA_ID (tmpDataHeader.DataInfo))
        {
          /* Restrict bank scope as it might be corrupted */
          ap_BankList[bankCnt]->p_NextWriteAddress = p_tmpAddress;

          /* Update remaining space */
          ap_BankList[bankCnt]->RemainingSpace = 0x00;

          /* Update loop leaving condition */
          p_endAddress = p_tmpAddress;
        }
        /* Header should be OK */
        else
        {
          /* Get data information */
          currentDataId = PS_GET_DATA_ID (tmpDataHeader.DataInfo);

          currentDataSize = PS_GET_DATA_LENGTH (tmpDataHeader.DataInfo) +
                            sizeof (PS_DataHeader_t) -
                            PS_HEADER_DATA_NUMBER;

          currentDataSize = (uint32_t)PS_ALIGN_FLASH_LINE (currentDataSize);

          /* Compute data crc */
          error = CheckDataValidity (p_tmpAddress);

          if (PS_OK == error)
          {
            /* Data valid - Check if not already found */
            if (0x00 == (dataFoundMask & (1u << currentDataId)))
            {
              /* New valid data found */
              dataFoundMask |= (1u << currentDataId);

              /* Set the bank presence bitmask */
              ap_BankList[bankCnt]->PresenceBitmask |= (1u << currentDataId);
            }

            /* Update occupation table - header and padding included */
            a_OccupationTable[currentDataId] = currentDataSize;

            /* Jump to the next data */
            p_tmpAddress = PS_ALIGN_FLASH_LINE ((uint32_t *)((uint32_t)p_tmpAddress + currentDataSize));

            /* Update remaining space */
            ap_BankList[bankCnt]->RemainingSpace = ((uint32_t)p_endAddress - (uint32_t)p_tmpAddress);
          }
          else if (PS_CRC_ERROR == error)
          {
            /* Not sure if validity is NOK because of a corrupted header or a corrupted data */
            /* Data shall be invalidated */
            /* TODO: Determine whether it shall be blocking */
            (void)InvalidateLine (p_tmpAddress);

            /* Restrict bank scope as it might be corrupted */
            ap_BankList[bankCnt]->p_NextWriteAddress = p_tmpAddress;

            /* Restrict the bank action range */
            ap_BankList[bankCnt]->RemainingSpace = 0x00;

            /* Update leaving loop condition */
            p_endAddress = p_tmpAddress;

            /* Reset error code */
            error = PS_OK;
          }
          else
          {
            /* Do nothing, error code should already be meaningful */
          }

          /* Update next write pointer */
          ap_BankList[bankCnt]->p_NextWriteAddress = p_tmpAddress;
        }
      }
    }

    if (PS_OK == error)
    {
      bankCnt--;
    }
  }

  return error;
}

PS_Cmd_Status_e ConfirmValidData (const uint32_t * const p_PotentialValidData,
                                          uint8_t * const p_DataDestination,
                                          const uint32_t DataDestinationSize,
                                          uint32_t * const p_OccupationSize)
{
  PS_Cmd_Status_e error = PS_OK;

  PS_DataHeader_t currentDataHeader = {0u};

  if (FALSE == IsContextToBeKept (PS_CONFIRMVALIDDATA_ID))
  {
    /* Nothing to reset since operation is always restarted from the beginning */
  }
  else
  {
    /* Reset context keeping mask */
    ResetContextKeeping (PS_CONFIRMVALIDDATA_ID);
  }

  /* Information shall be updated */
  error = ReadFlashLine (p_PotentialValidData,
                         (uint32_t *)&currentDataHeader);

  if (PS_OK == error)
  {
    /* Compute the new occupation of the valid data */
    *p_OccupationSize = PS_GET_DATA_LENGTH (currentDataHeader.DataInfo);

    if (*p_OccupationSize <= DataDestinationSize)
    {
      /* Copy the valid data inside the destination buffer */
      error = PS_FlashDriver.Read ((uint32_t *)((uint32_t)p_PotentialValidData +
                                                                  sizeof (PS_DataHeader_t) -
                                                                  PS_HEADER_DATA_NUMBER),
                                            (uint32_t *)p_DataDestination,
                                            *p_OccupationSize);

      if (PS_OK == error)
      {
        /* Check integrity */
        error = CheckDataIntegrity (&currentDataHeader,
                                    (uint32_t *)p_DataDestination);
      }
    }
    /* Either user data size is not enough or header is corrupted */
    else
    {
      /* First check validity of data */
      error = CheckDataValidity (p_PotentialValidData);

      if (PS_OK == error)
      {
        /* Not enough memory given by user */
        if (sizeof (uint32_t) <= DataDestinationSize)
        {
          /* Return read data size */
          memcpy ((void *)p_DataDestination,
                  (void *)p_OccupationSize,
                  sizeof (uint32_t));
        }

        /* Update error code */
        error = PS_ERROR_DATA_SIZE;
      }
    }
  }

  return error;
}

PS_Cmd_Status_e EraseBank (const uint32_t BankId)
{
  PS_Cmd_Status_e error = PS_OK;

  static uint32_t eraseSectorIdx = 0x00;
  static uint32_t lastSectorIdx = 0x00;

  if (FALSE == IsContextToBeKept (PS_ERASEBANK_ID))
  {
    /* Reset the index */
    eraseSectorIdx = (((uint32_t)ap_BankList[BankId]->p_StartAddress - PS_FLASH_BASE_ADDRESS) /
                       PS_SECTOR_SIZE);

    lastSectorIdx = eraseSectorIdx + CurrentConfiguration.BankSize;
  }
  else
  {
    /* Reset context keeping mask */
    ResetContextKeeping (PS_ERASEBANK_ID);
  }

  /* Erase all the sectors */
  while ((lastSectorIdx > eraseSectorIdx) && (PS_OK == error))
  {
    /* Erase the sector */
    error = PS_FlashDriver.Erase (eraseSectorIdx);

    if (PS_OK == error)
    {
      eraseSectorIdx++;
    }
  }

  return error;
}

PS_Cmd_Status_e WriteBankHeader (const uint8_t BankId,
                                         const uint32_t EraseCounter)
{
  PS_Cmd_Status_e error = PS_UNKNOWN;

  uint8_t bankHeaderCrc = PS_CRC8_RESET_VALUE;

  PS_BankHeader_t emptyBankHeader = {0xFF};

  static PS_BankHeader_t tmpBankHeader = {0xFF};

  if (FALSE == IsContextToBeKept (PS_WRITEBANKHEADER_ID))
  {
    /* Reset the header */
    memset ((void *)&tmpBankHeader,
            0xFF,
            sizeof (PS_BankHeader_t));

    /* Update the freshness tracker before writing the header - Avoid hitting 0xFF */
    if (0xFE > FreshnessValue)
    {
      FreshnessValue++;
    }
    else
    {
      FreshnessValue = 0x00u;
    }

    /* Fulfil the bank header with information */
    tmpBankHeader.Freshness = FreshnessValue;
    tmpBankHeader.BankId = BankId;
    tmpBankHeader.BankNumber = CurrentConfiguration.BankNumber;
    tmpBankHeader.SectorNumber = CurrentConfiguration.BankSize;
    tmpBankHeader.BankInfo = PS_MERGE_BANK_INFO (EraseCounter, 0xFFu);

    /* Compute CRC8 of the bank header */
    ComputeCrcEight ((uint8_t *)&tmpBankHeader,
                     PS_HEADER_BANK_SIZE_WO_CRC,
                     &bankHeaderCrc);

    /* Update bank info with CRC8 */
    tmpBankHeader.BankInfo = PS_MERGE_BANK_INFO (EraseCounter, bankHeaderCrc);
  }
  else
  {
    /* Reset context keeping mask */
    ResetContextKeeping (PS_WRITEBANKHEADER_ID);
  }

  /* Execute the write operation */
  error = PS_FlashDriver.Write (a_PSBanksInfo[BankId].p_StartAddress,
                                        (uint32_t *)(&tmpBankHeader));

  /* Write OK ? */
  if (PS_OK == error)
  {
    /* Read it back to check if all is OK */
    error = ReadFlashLine (a_PSBanksInfo[BankId].p_StartAddress,
                           (uint32_t *)&emptyBankHeader);

    if (PS_OK == error)
    {
      if (PS_IS_FLASH_LINE_OK (&emptyBankHeader,
                                       &tmpBankHeader))
      {
        /* Update next write address */
        a_PSBanksInfo[BankId].p_NextWriteAddress = (uint32_t *)((uint32_t)a_PSBanksInfo[BankId].p_StartAddress +
                                                                              PS_FLASH_LINE_LENGTH);

        /* Update presence bitmask */
        a_PSBanksInfo[BankId].PresenceBitmask = 0x00;

        /* Update remaining space */
        a_PSBanksInfo[BankId].RemainingSpace = (CurrentConfiguration.BankSize * PS_SECTOR_SIZE) -
                                                    PS_FLASH_LINE_LENGTH;
      }
      else
      {
        error = PS_ERROR_INVALID;
      }
    }
  }

  return error;
}

PS_Cmd_Status_e WriteData (uint32_t * const p_WriteAddress,
                                   const PS_Data_ID_e DataID,
                                   const uint8_t * const p_DataSrc,
                                   const uint32_t DataSize,
                                   uint32_t * const p_WrittenBytes)
{
  PS_Cmd_Status_e error = PS_OK;

  /* Last flash line write to be aligned with a flash line */
  uint8_t lastWriteLine [PS_FLASH_LINE_LENGTH] = {0};

#if (PS_FLASH_LINE_LENGTH == PS_FLASH_LINE_128B)
  uint8_t a_flashLineRead [PS_FLASH_LINE_128B] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                                           0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
#else
  uint8_t a_flashLineRead [PS_FLASH_LINE_64B] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
#endif

  /* Current state of the operation */
  static PS_WriteDataStates_e currentState = PS_WRITEDATA_HEADER_WRITE;

  /* Number of bytes written */
  static uint32_t writtenBytes = 0x00;
  /* Amount of bytes inside the main data write loop */
  static uint32_t kernelDataSize = 0x00;

  /* Pointer onto the address to write the data */
  static uint32_t * p_writingAddr = NULL;
  /* Pointer onto the main data write loop source */
  static uint32_t * p_kernelDataAddr = NULL;

  /* Data header variable */
  static PS_DataHeader_t dataHeader = {0};

  /* Start from scratch ? */
  if (FALSE == IsContextToBeKept (PS_WRITEDATA_ID))
  {
    /* Reset static variables */
    writtenBytes = 0x00;
    kernelDataSize = 0x00;

    p_writingAddr = NULL;
    p_kernelDataAddr = NULL;

    memset ((void *)(&dataHeader),
            0x00,
            sizeof (PS_DataHeader_t));

    /* ---- Data header creation ---------------------------------------------*/
    /* Fulfil the header - Data information: Data ID + Data Length */
    dataHeader.DataInfo = PS_MERGE_DATA_INFO (DataID,
                                                      DataSize);

    /* Start computation of CRC - Data Information */
    error = PS_CrcDriver.Calculate (&(dataHeader.DataInfo),
                                            PS_DATA_HEADER_DATA_INFO_SIZE,
                                            &dataHeader.Crc);

    if (PS_OK == error)
    {
      /* Compute integrity checksum on the data */
      error = PS_CrcDriver.Accumulate ((uint32_t *)(p_DataSrc),
                                               DataSize,
                                               &dataHeader.Crc);

      if (PS_OK == error)
      {
/* - 128 bits flash line header case --- */
#if (PS_FLASH_LINE_LENGTH == PS_FLASH_LINE_128B)
        uint32_t headerDataSize = DataSize;

        if (PS_HEADER_DATA_NUMBER < headerDataSize)
        {
          headerDataSize = PS_HEADER_DATA_NUMBER;
        }

        memset ((void *)dataHeader.Data,
          0xFF,
          PS_HEADER_DATA_NUMBER);

        /* Fulfil the header - Data start */
        memcpy ((void *)dataHeader.Data,
                (void *)p_DataSrc,
                headerDataSize);
#endif /* (PS_FLASH_LINE_LENGTH == PS_FLASH_LINE_128B) */

        /* Setup the write address */
        p_writingAddr = p_WriteAddress;

        /* Update kernel data information */
#if (PS_FLASH_LINE_LENGTH == PS_FLASH_LINE_128B)
        p_kernelDataAddr = (uint32_t *)((uint32_t)p_DataSrc + headerDataSize);
        kernelDataSize = DataSize - headerDataSize;
#else
        p_kernelDataAddr = (uint32_t *)p_DataSrc;
        kernelDataSize = DataSize;
#endif

        /* Update state */
        currentState = PS_WRITEDATA_HEADER_WRITE;
      }
    }
  }
  else
  {
    /* Reset context keeping mask */
    ResetContextKeeping (PS_WRITEDATA_ID);
  }

  /* ---- State machine for operations ---------------------------------------*/
  while ((PS_OK == error) && (PS_WRITEDATA_OP_OVER != currentState))
  {
    switch (currentState)
    {
      case PS_WRITEDATA_HEADER_WRITE:
      {
        /* Write the header */
        error = PS_FlashDriver.Write (p_writingAddr,
                                              (uint32_t *)(&dataHeader));

        /* Write OK ? */
        if (PS_OK == error)
        {
          /* Read it back to check if all is OK */
          error = ReadFlashLine(p_writingAddr,
                                (uint32_t *)a_flashLineRead);

          if (PS_OK == error)
          {
            if (PS_IS_FLASH_LINE_OK (a_flashLineRead, ((uint32_t *)&dataHeader)))
            {
              /* Update written bytes count */
              writtenBytes += PS_FLASH_LINE_LENGTH;

              /* Update written bytes count */
              *p_WrittenBytes += writtenBytes;

              /* Update write address */
              p_writingAddr = (uint32_t *)((uint32_t)p_writingAddr + PS_FLASH_LINE_LENGTH);

              /* Update state */
              currentState = PS_WRITEDATA_KERNEL_WRITE;
            }
            else
            {
              error = PS_ERROR_INVALID;
            }
          }
        }
        break;
      }

      case PS_WRITEDATA_KERNEL_WRITE:
      {
        /* Execute flash write operation - Kernel */
        while ((PS_FLASH_LINE_LENGTH <= kernelDataSize) &&
               (PS_OK == error))
        {
          /* Write the flash line */
          error = PS_FlashDriver.Write (p_writingAddr,
                                                p_kernelDataAddr);

          /* Write OK ? */
          if (PS_OK == error)
          {
            /* Read it back to check if all is OK */
            error = ReadFlashLine (p_writingAddr,
                                   (uint32_t *)a_flashLineRead);

            if (PS_OK == error)
            {
              if (PS_IS_FLASH_LINE_OK (a_flashLineRead, p_kernelDataAddr))
              {
                /* Update written bytes count */
                writtenBytes += PS_FLASH_LINE_LENGTH;

                /* Update written bytes count */
                *p_WrittenBytes += writtenBytes;

                /* Update the kernel size */
                kernelDataSize -= PS_FLASH_LINE_LENGTH;

                /* Update write address */
                p_writingAddr = (uint32_t *)((uint32_t)p_writingAddr + PS_FLASH_LINE_LENGTH);

                /* Update kernel data address */
                p_kernelDataAddr = (uint32_t *)((uint32_t)p_kernelDataAddr + PS_FLASH_LINE_LENGTH);
              }
              else
              {
                error = PS_ERROR_INVALID;
              }
            }
          }
        }

        if ((PS_FLASH_LINE_LENGTH > kernelDataSize) &&
            (PS_OK == error))
        {
          /* Update state */
          currentState = PS_WRITEDATA_TAIL_WRITE;
        }
        break;
      }

      case PS_WRITEDATA_TAIL_WRITE:
      {
        /* Remaining bytes to write down ? */
        if (0x00 < kernelDataSize)
        {
          /* Copy the data to write in the end */
          memcpy (lastWriteLine,
                  p_kernelDataAddr,
                  kernelDataSize);

          /* Setup the last possible line with 0xFF */
          memset (&lastWriteLine[kernelDataSize],
                  0xFF,
                  (PS_FLASH_LINE_LENGTH - kernelDataSize));

          /* Write the flash line */
          error = PS_FlashDriver.Write (p_writingAddr,
                                                (uint32_t *)&lastWriteLine);

          /* Write OK ? */
          if (PS_OK == error)
          {
            /* Read it back to check if all is OK */
            error = ReadFlashLine (p_writingAddr,
                                   (uint32_t *)a_flashLineRead);

            if (PS_OK == error)
            {
              if (PS_IS_FLASH_LINE_OK (a_flashLineRead, lastWriteLine))
              {
                /* Update written bytes count */
                *p_WrittenBytes = writtenBytes + PS_FLASH_LINE_LENGTH;

                /* Update size */
                kernelDataSize = 0x00;

                /* Update current state */
                currentState = PS_WRITEDATA_OP_OVER;
              }
              else
              {
                error = PS_ERROR_INVALID;
              }
            }
          }
        }
        /* Write operation is over, leave */
        else
        {
          /* Update written bytes count */
          *p_WrittenBytes = writtenBytes;

          /* Update current state */
          currentState = PS_WRITEDATA_OP_OVER;
        }
        break;
      }

      case PS_WRITEDATA_OP_OVER:
      {
        /* Do nothing, should never be reached */
        break;
      }

      default:
      {
        error = PS_NOK;
        break;
      }
    }
  }

  return error;
}

PS_Cmd_Status_e OrganizeBankList (const uint32_t NewSpareBankIdx)
{
  PS_Cmd_Status_e error = PS_OK;

  uint8_t maxFreshness = 0x00;
  uint32_t maxIdx = 0x00;

  PS_BankHeader_t tmpBankHeader = {0xFF};

  PS_BankInformation_t * tmpPointer = NULL;

  static uint32_t searchIteration = 0x00;

  if (FALSE == IsContextToBeKept (PS_ORGANIZEBANKLIST_ID))
  {
    /* First invert position of new and old spare banks */
    tmpPointer = ap_BankList[(CurrentConfiguration.BankNumber - 1u)];
    ap_BankList[(CurrentConfiguration.BankNumber - 1u)] = ap_BankList[NewSpareBankIdx];
    ap_BankList[NewSpareBankIdx] = tmpPointer;

    /* Spare Bank is always the last one, no need to hit it */
    searchIteration = (CurrentConfiguration.BankNumber - 2u);
  }
  else
  {
    /* Reset context keeping mask */
    ResetContextKeeping (PS_ORGANIZEBANKLIST_ID);
  }

  while ((0x00 < searchIteration) && (PS_OK == error))
  {
    maxIdx = searchIteration;

    error = ReadFlashLine (ap_BankList[maxIdx]->p_StartAddress,
                           (uint32_t *)&tmpBankHeader);

    if (PS_OK == error)
    {
      maxFreshness = tmpBankHeader.Freshness;

      /* Determine the greatest */
      for (uint32_t cnt = searchIteration;
           ((0x00 < cnt) &&
            (PS_OK == error));
           cnt--)
      {
        error = ReadFlashLine(ap_BankList[cnt]->p_StartAddress,
                              (uint32_t *)&tmpBankHeader);

        if (PS_OK == error)
        {
          /* Is this max ? */
          if (maxFreshness < tmpBankHeader.Freshness)
          {
            maxIdx = cnt;
          }
        }
      }

      if (PS_OK == error)
      {
        /* Execute a swap between the max and the current search iteration */
        if (searchIteration != maxIdx)
        {
          tmpPointer = ap_BankList[maxIdx];
          ap_BankList[maxIdx] = ap_BankList[searchIteration];
          ap_BankList[searchIteration] = tmpPointer;
        }

        searchIteration--;
      }
    }
  }

  return error;
}

PS_Cmd_Status_e CreateBankList (const uint32_t StartingIndex)
{
  PS_Cmd_Status_e error = PS_OK;

  uint8_t headerCrc = PS_CRC8_RESET_VALUE;
  int16_t tmpDiff = 0x00;

#if (PS_FLASH_LINE_LENGTH == PS_FLASH_LINE_128B)
  uint8_t a_flashLineRead [PS_FLASH_LINE_128B] = {0xFF};
#else
  uint8_t a_flashLineRead [PS_FLASH_LINE_64B] = {0xFF};
#endif

  PS_BankHeader_t tmpBankHeader = {0xFFu};

  static uint32_t bankInfoIdx = 0x00;
  static uint32_t spareBankIdx = 0xFF;
  static uint32_t fresherBankIdx = 0xFF;
  static uint32_t maxEraseCounter = 0x00;
  static PS_CreateBankListStates_e currentState = PS_CREATEBANKLIST_GATHER_INFORMATION;

  if (FALSE == IsContextToBeKept (PS_CREATEBANKLIST_ID))
  {
    /* Reset the index */
    bankInfoIdx = StartingIndex;

    /* Reset the index */
    spareBankIdx = 0xFF;

    /* Reset the index */
    fresherBankIdx = StartingIndex;

    /* Reset max erase counter */
    maxEraseCounter = 0x00;

    /* Reset state */
    currentState = PS_CREATEBANKLIST_GATHER_INFORMATION;
  }
  else
  {
    /* Reset context keeping mask */
    ResetContextKeeping (PS_CREATEBANKLIST_ID);
  }

  while ((PS_OK == error) && (PS_CREATEBANKLIST_OP_OVER != currentState))
  {
    switch (currentState)
    {
      case PS_CREATEBANKLIST_GATHER_INFORMATION:
      {
        /* Scroll the whole banks to get information */
        while ((CurrentConfiguration.BankNumber > bankInfoIdx) && (PS_OK == error))
        {
          /* Get the bank header */
          error = ReadFlashLine (a_PSBanksInfo[bankInfoIdx].p_StartAddress,
                                 (uint32_t *)&tmpBankHeader);

          if (PS_OK == error)
          {
            /* Compute the crc of the supposed header */
            ComputeCrcEight ((uint8_t *)&tmpBankHeader,
                             PS_HEADER_BANK_SIZE_WO_CRC,
                             &headerCrc);

            /* Check if the crc does match the header value - Valid header ? */
            if (headerCrc == PS_GET_CRC (tmpBankHeader.BankInfo))
            {
              /* --- Update the freshness value ----- */
              tmpDiff = (int16_t)(FreshnessValue) - tmpBankHeader.Freshness;

              if (0 > tmpDiff)
              {
                if (tmpDiff >= ((int32_t)CurrentConfiguration.AgingLimit * -1))
                {
                  FreshnessValue = tmpBankHeader.Freshness;

                  fresherBankIdx = bankInfoIdx;
                }
              }
              else
              {
                if (tmpDiff >= (255u - CurrentConfiguration.AgingLimit))
                {
                  FreshnessValue = tmpBankHeader.Freshness;

                  fresherBankIdx = bankInfoIdx;
                }
              }

              /* --- Update the erase counter ----- */
              if (maxEraseCounter < PS_GET_ERASE_COUNTER (tmpBankHeader.BankInfo))
              {
                maxEraseCounter = PS_GET_ERASE_COUNTER (tmpBankHeader.BankInfo);
              }
            }
            /* Crc error */
            else
            {
              /* Bank shall be cleared */
              error = EraseBank (bankInfoIdx);

              if (PS_OPERATION_PAUSED == error)
              {
                /* Setup context keeping mask */
                SetContextKeeping (PS_ERASEBANK_ID);
              }
              else
              {
                /* Do nothing, error code shall already be set */
              }
            }

            /* Update the index */
            if (PS_OK == error)
            {
              bankInfoIdx++;
            }
          }
        }

        /* Is research over ? */
        if (CurrentConfiguration.BankNumber == bankInfoIdx)
        {
          /* If there is/are corrupted bank(s) before our starting index, clean is required */
          if (0x00 < StartingIndex)
          {
            /* Reset bank index */
            bankInfoIdx = StartingIndex;

            currentState = PS_CREATEBANKLIST_CLEAN_CORRUPTED_BANKS;
          }
          else
          {
            bankInfoIdx = 0x00;

            /* Update the state */
            currentState = PS_CREATEBANKLIST_DETECT_SPARE_BANK_AND_WRITE_HEADER;
          }
        }
        break;
      }

      case PS_CREATEBANKLIST_CLEAN_CORRUPTED_BANKS:
      {
        while ((0x00 < bankInfoIdx) && (PS_OK == error))
        {
          error = EraseBank ((bankInfoIdx - 1u));

          if (PS_OK == error)
          {
            bankInfoIdx--;
          }
          else if (PS_OPERATION_PAUSED == error)
          {
            /* Setup context keeping mask */
            SetContextKeeping (PS_ERASEBANK_ID);
          }
          else
          {
            /* Do nothing, error code shall already be set */
          }
        }

        if (0x00 == bankInfoIdx)
        {
          /* Update the state */
          currentState = PS_CREATEBANKLIST_DETECT_SPARE_BANK_AND_WRITE_HEADER;
        }
        break;
      }

      case PS_CREATEBANKLIST_DETECT_SPARE_BANK_AND_WRITE_HEADER:
      {
        while ((CurrentConfiguration.BankNumber > bankInfoIdx) && (PS_OK == error))
        {
          error = ReadFlashLine (a_PSBanksInfo[bankInfoIdx].p_StartAddress,
                                 (uint32_t *)a_flashLineRead);

          if (PS_OK == error)
          {
            /* Check if the first line is empty - No header present, should be a newly erased bank */
            if (PS_IS_FLASH_LINE_OK (a_flashLineRead,
                                             a_EmptyFlashLine))
            {
              /* Bank header shall be written */
              error = WriteBankHeader (bankInfoIdx,
                                       maxEraseCounter);

              if (PS_OK == error)
              {
                /* Possible spare Bank found */
                spareBankIdx = bankInfoIdx;

                bankInfoIdx++;
              }
              else if (PS_ERROR_INVALID == error)
              {
                /* Bank shall be erased before trying to re-write the bank header */
                error = EraseBank (bankInfoIdx);

                if (PS_OPERATION_PAUSED == error)
                {
                  /* Setup context keeping mask */
                  SetContextKeeping (PS_ERASEBANK_ID);
                }
                else
                {
                  /* Do nothing, error code shall already be set */
                }
              }
              else if (PS_OPERATION_PAUSED == error)
              {
                /* Setup context keeping mask */
                SetContextKeeping (PS_WRITEBANKHEADER_ID);
              }
              else
              {
                /* Do nothing, error code should already be set */
              }
            }
            else
            {
              /* Read the second line to check if there is a data header */
              error = ReadFlashLine (((uint32_t *)((uint32_t)a_PSBanksInfo[bankInfoIdx].p_StartAddress +
                                                             PS_FLASH_LINE_LENGTH)),
                                     (uint32_t *)a_flashLineRead);

              if (PS_OK == error)
              {
                /* Check if it is a possible spare bank - Its second flash line shall be full of 0xFF */
                if (PS_IS_FLASH_LINE_OK (a_flashLineRead,
                                                 a_EmptyFlashLine))
                {
                  /* Possible spare Bank found */
                  spareBankIdx = bankInfoIdx;
                }

                bankInfoIdx++;
              }
            }
          }
        }

        if (CurrentConfiguration.BankNumber == bankInfoIdx)
        {
          /* Update the state */
          currentState = PS_CREATEBANKLIST_SORT_BANK_LIST;
        }
        break;
      }

      case PS_CREATEBANKLIST_SORT_BANK_LIST:
      {
        /* Sorting depends on the presence of a SpareBank */
        if (0xFF == spareBankIdx)
        {
          /* No spare bank found, the fresher bank is the last spare bank. Target has been reset before bank swap */
          error = OrganizeBankList (fresherBankIdx);
        }
        else
        {
          /* Update bank list */
          error = OrganizeBankList (spareBankIdx);
        }

        if (PS_OK == error)
        {
          if (0xFF == spareBankIdx)
          {
            /* Specify that a cleanup is required right after since no active spare bank is present */
            error = PS_CLEANUP_REQUIRED;
          }

          /* Update the state */
          currentState = PS_CREATEBANKLIST_OP_OVER;
        }
        else if (PS_OPERATION_PAUSED == error)
        {
          /* Setup context keeping mask */
          SetContextKeeping (PS_ORGANIZEBANKLIST_ID);
        }
        else
        {
          /* Do nothing, error code should already be set */
        }

        break;
      }

      case PS_CREATEBANKLIST_OP_OVER:
      {
        /* Do nothing, should never be reached */
        break;
      }

      default:
      {
        /* Nothing to do here, leave */
        error = PS_NOK;
        break;
      }
    }
  }

  return error;
}

PS_Cmd_Status_e BuildDataMap (void)
{
  PS_Cmd_Status_e error = PS_OK;

  /* Recover all the data on all the whole Persistent Storage memory */
  error = RecoverValidData ((CurrentConfiguration.BankNumber - 1u),
                            0xFFFFFFFF);

  if (PS_OPERATION_PAUSED == error)
  {
    /* Setup context keeping mask */
    SetContextKeeping (PS_RECOVERVALIDDATA_ID);
  }
  else
  {
    /* Do nothing, error code should already be set */
  }

  return error;
}

PS_Cmd_Status_e SwapBank (const uint32_t NewSpareBankIdx)
{
  PS_Cmd_Status_e error = PS_OK;
  PS_Cmd_Status_e candidateError = PS_DATA_NOT_FOUND;

  uint32_t occupationSize = 0x00;
  uint32_t writtenBytes = 0x00;

  PS_DataHeader_t currentDataHeader = {0xFFu};
  PS_BankHeader_t currentBankHeader = {0xFFu};

  static uint8_t bankId = 0xFF;
  static uint32_t bitmaskIdx = 0x00;
  static uint32_t eraseCounterValue = 0x00;
  static uint32_t * p_validDataAddr = NULL;
  static uint32_t * a_possibleDataAddr [3u] = {NULL, NULL, NULL};
  static PS_BankSwapStates_e currentState = PS_SWAPBANK_DATA_SEARCH;

  /* Check if it is a start from scratch */
  if (FALSE == IsContextToBeKept (PS_SWAPBANK_ID))
  {
    /* Reset the static variables */
    bankId = 0xFF;
    bitmaskIdx = 0x00;
    eraseCounterValue = 0x00;
    p_validDataAddr = NULL;
    a_possibleDataAddr[0u] = NULL;
    a_possibleDataAddr[1u] = NULL;
    a_possibleDataAddr[2u] = NULL;
    currentState = PS_SWAPBANK_DATA_SEARCH;
  }
  else
  {
    /* Reset context keeping mask */
    ResetContextKeeping (PS_SWAPBANK_ID);
  }

  /* Loop over all the data IDs */
  while ((PS_DATA_ID_MAX > bitmaskIdx) &&
         ((PS_OK == error) || (PS_DATA_NOT_FOUND == error)))
  {
    /* Check which valid data is present in the future spare bank */
    if (ap_BankList[NewSpareBankIdx]->PresenceBitmask & (1u << bitmaskIdx))
    {
      while ((PS_SWAPBANK_OP_OVER != currentState) &&
             ((PS_OK == error) || (PS_DATA_NOT_FOUND == error)))
      {
        switch (currentState)
        {
          case PS_SWAPBANK_DATA_SEARCH:
          {
            uint8_t validDataFound = FALSE;

            /* Get the data position if any */
            error = SearchData ((PS_Data_ID_e)bitmaskIdx,
                                NewSpareBankIdx,
                                p_validDataAddr,
                                a_possibleDataAddr);

            if (PS_OK == error)
            {
              /* Check candidates from newest to oldest */
              for (int8_t idx = 2;
                   (0 <= idx) && (FALSE == validDataFound) && (PS_OK == error);
                   idx--)
              {
                if (NULL != a_possibleDataAddr[idx])
                {
                  candidateError = CheckDataValidity (a_possibleDataAddr[idx]);

                  if (PS_OK == candidateError)
                  {
                    p_validDataAddr = a_possibleDataAddr[idx];
                    validDataFound = TRUE;
                  }
                  else if ((PS_CRC_ERROR == candidateError) ||
                           (PS_ERROR_INVALID == candidateError) ||
                           (PS_DATA_NOT_FOUND == candidateError))
                  {
                    /* Candidate is not valid, continue with older one */
                    error = PS_OK;
                  }
                  else
                  {
                    error = candidateError;
                  }
                }
              }

              if ((PS_OK == error) && (TRUE == validDataFound))
              {
                /* Read header value */
                error = ReadFlashLine (p_validDataAddr,
                                       (uint32_t *)&currentDataHeader);

                if (PS_OK == error)
                {
                  occupationSize = PS_GET_DATA_LENGTH (currentDataHeader.DataInfo) +
                                   sizeof (PS_DataHeader_t) -
                                   PS_HEADER_DATA_NUMBER;

                  occupationSize = (uint32_t)PS_ALIGN_FLASH_LINE (occupationSize);

                  /* Update occupation table */
                  a_OccupationTable[bitmaskIdx] = occupationSize;

                  /* Update current state */
                  currentState = PS_SWAPBANK_DATA_WRITE;
                }
                else
                {
                  /* Do nothing, error code should already be set */
                }
              }
              else if (PS_OK == error)
              {
                /* No valid candidate found */
                error = PS_DATA_NOT_FOUND;
              }
              else
              {
                /* Do nothing, error code should already be set */
              }
            }
            else if (PS_DATA_NOT_FOUND == error)
            {
              /* Update presence bitmask */
              ap_BankList[NewSpareBankIdx]->PresenceBitmask &= ~(1u << bitmaskIdx);

              /* Update current state to search the missing data */
              currentState = PS_SWAPBANK_SEARCH_MISSING_DATA;
            }
            else if (PS_ERROR_INVALID == error)
            {
              /* Invalid data found, keep searching for another version of this data ... */
              /* ... Update error code for loop purpose */
              error = PS_DATA_NOT_FOUND;
            }
            else if (PS_OPERATION_PAUSED == error)
            {
              /* Setup context keeping mask */
              SetContextKeeping (PS_SEARCHDATA_ID);
            }
            else
            {
              /* Error code should already be set */
            }

            break;
          }

          case PS_SWAPBANK_DATA_WRITE:
          {
            /* Reset written bytes */
            writtenBytes = 0x00;

            /* Transfer the data from New Spare Bank to Old Spare Bank */
            error = CopyData (ap_BankList[(CurrentConfiguration.BankNumber - 1u)]->p_NextWriteAddress,
                              p_validDataAddr,
                              a_OccupationTable[bitmaskIdx],
                              &writtenBytes);

            if (PS_OK == error)
            {
              /* Clear the old hosting bank presence bitmask */
              ap_BankList[NewSpareBankIdx]->PresenceBitmask &= ~(1u << bitmaskIdx);

              /* Update presence bitmask */
              ap_BankList[(CurrentConfiguration.BankNumber - 1u)]->PresenceBitmask |= (1u << bitmaskIdx);

              /* Update bank remaining space */
              ap_BankList[(CurrentConfiguration.BankNumber - 1u)]->RemainingSpace -= writtenBytes;

              /* Update next write address */
              ap_BankList[(CurrentConfiguration.BankNumber - 1u)]->p_NextWriteAddress =
                (uint32_t *)((uint32_t)ap_BankList[(CurrentConfiguration.BankNumber - 1u)]->p_NextWriteAddress +
                                                                                            writtenBytes);

              /* Search for a new possible data */
              currentState = PS_SWAPBANK_OP_OVER;
            }
            else if (PS_ERROR_INVALID == error)
            {
              /* A spare bank clean is required */
              currentState = PS_SWAPBANK_ERROR_SPARE_BANK_ERASE;

              /* Update error code for loop purpose */
              error = PS_DATA_NOT_FOUND;
            }
            else if (PS_OPERATION_PAUSED == error)
            {
              /* Setup context keeping mask */
              SetContextKeeping (PS_COPYDATA_ID);
            }
            else
            {
              /* Do nothing, error code should already be set */
            }

            break;
          }

          case PS_SWAPBANK_SEARCH_MISSING_DATA:
          {
            /* Search for the missing data - In others banks */
            error = RecoverValidData (NewSpareBankIdx,
                                      (1u << bitmaskIdx));

            if (PS_OK == error)
            {
              /* Keep searching for other valid data in the New Spare Bank */
              currentState = PS_SWAPBANK_OP_OVER;
            }
            else if (PS_OPERATION_PAUSED == error)
            {
              /* Setup context keeping mask */
              SetContextKeeping (PS_RECOVERVALIDDATA_ID);
            }
            else
            {
              /* Error code should already be set */
            }

            break;
          }

          case PS_SWAPBANK_ERROR_SPARE_BANK_ERASE:
          {
            /* Check if erase counter is known */
            if ((0xFF == bankId) &&
                (0x00 == eraseCounterValue))
            {
              /* First get the erase counter */
              error = ReadFlashLine (ap_BankList[(CurrentConfiguration.BankNumber - 1u)]->p_StartAddress,
                                     (uint32_t *)&currentBankHeader);

              if (PS_OK == error)
              {
                /* Before erasing the bank, recover the bank id ... */
                bankId = currentBankHeader.BankId;

                /* ... and the erase counter value */
                eraseCounterValue = PS_GET_ERASE_COUNTER (currentBankHeader.BankInfo);
              }
            }

            if (PS_OK == error)
            {
              /* Erase the spare bank before any new write operation */
              error = EraseBank ((CurrentConfiguration.BankNumber - 1u));

              if (PS_OK == error)
              {
                /* Update erase counter */
                eraseCounterValue++;

                /* Update trackers */
                ap_BankList[(CurrentConfiguration.BankNumber - 1u)]->PresenceBitmask = 0x00;

                /* Invalid write happened, ask user to restart write process */
                currentState = PS_SWAPBANK_ERROR_SPARE_BANK_WRITE_HEADER;
              }
              else if (PS_OPERATION_PAUSED == error)
              {
                /* Setup context keeping mask */
                SetContextKeeping (PS_ERASEBANK_ID);
              }
              else
              {
                /* Do nothing, error code shall already be set */
              }
            }

            break;
          }

          case PS_SWAPBANK_ERROR_SPARE_BANK_WRITE_HEADER:
          {
            error = WriteBankHeader (bankId,
                                     eraseCounterValue);

            if (PS_OK == error)
            {
              /* Update the state */
              currentState = PS_SWAPBANK_OP_OVER;

              /* Update error code, it was an invalid write */
              error = PS_ERROR_INVALID;
            }
            else if (PS_ERROR_INVALID == error)
            {
              /* Let's try again; Erase the bank */
              currentState = PS_SWAPBANK_ERROR_SPARE_BANK_ERASE;

              /* Reset the error code */
              error = PS_OK;
            }
            else if (PS_OPERATION_PAUSED == error)
            {
              /* Setup context keeping mask */
              SetContextKeeping (PS_WRITEBANKHEADER_ID);
            }
            else
            {
              /* Do nothing, error code should already be set */
            }
            break;
          }

          case PS_SWAPBANK_OP_OVER:
          {
            /* Should never be reached */
            break;
          }

          default:
          {
            /* Should never be reached */
            break;
          }
        }
      }
    }

    /* Updates trackers */
    if (PS_OK == error)
    {
      /* Reset current state */
      currentState = PS_SWAPBANK_DATA_SEARCH;

      /* Reset variable for next iteration */
      p_validDataAddr = NULL;

      /* Increment data ID */
      bitmaskIdx++;
    }
  }

  return error;
}

PS_Cmd_Status_e InitOp (const PS_Configuration_t * const p_Config)
{
  PS_Cmd_Status_e error = PS_OK;

  /* Flag for Clean Up need */
  static uint8_t isCleanUpNeeded = FALSE;

  /* Index of a valid bank */
  static uint32_t validFoundIdx = 0xFF;

  /* Current state of the operation */
  static PS_InitStates_e currentState = PS_INIT_FIND_VALID_HEADER;

  if (FALSE == IsContextToBeKept (PS_INITOP_ID))
  {
    /* Reset clean up flag */
    isCleanUpNeeded = FALSE;

    /* Reset index */
    validFoundIdx = 0xFF;

    /* Reset the operation state */
    currentState = PS_INIT_FIND_VALID_HEADER;

    /* Copy config */
    memcpy (&CurrentConfiguration,
            p_Config,
            sizeof (PS_Configuration_t));
  }
  else
  {
    /* Reset context keeping mask */
    ResetContextKeeping (PS_INITOP_ID);
  }

  while ((PS_OK == error) && (PS_INIT_OP_OVER != currentState))
  {
    switch (currentState)
    {
      case PS_INIT_FIND_VALID_HEADER:
      {
        /* Find if there is an already existing Persistent Storage present */
        error = FindLocation (CurrentConfiguration.p_StartingAddress,
                              CurrentConfiguration.p_SearchLimit,
                              &validFoundIdx);

        if (PS_OK == error)
        {
          /* Has a location been found ? */
          if (0xFF == validFoundIdx)
          {
            /* No implementation found, start a new one */
            validFoundIdx = 0x00;

            for (uint32_t cnt = 0x00;
                 CurrentConfiguration.BankNumber > cnt;
                 cnt++)
            {
              /* Init the bank information */
              a_PSBanksInfo[cnt].p_StartAddress = (uint32_t *)((uint32_t)CurrentConfiguration.p_StartingAddress +
                                                      (cnt * (CurrentConfiguration.BankSize * PS_SECTOR_SIZE)));

              /* Initialize also the bank list */
              ap_BankList[cnt] = &a_PSBanksInfo[cnt];
            }
          }
          else
          {
            /* An old implementation has been found, do nothing */
          }

          /* Pursue recovering: Bank list creation */
          currentState = PS_INIT_BANK_LIST_CREATION;
        }

        break;
      }

      case PS_INIT_BANK_LIST_CREATION:
      {
        /* Create the bank list */
        error = CreateBankList (validFoundIdx);

        if (PS_OK == error)
        {
          /* Pursue recovering: Data mapping */
          currentState = PS_INIT_DATA_MAPPING;
        }
        else if (PS_CLEANUP_REQUIRED == error)
        {
          /* Update flag */
          isCleanUpNeeded = TRUE;

          /* Pursue recovering: Data mapping */
          currentState = PS_INIT_DATA_MAPPING;

          /* Reset error code */
          error = PS_OK;
        }
        else if (PS_OPERATION_PAUSED == error)
        {
          /* Setup context keeping mask */
          SetContextKeeping (PS_CREATEBANKLIST_ID);
        }
        else
        {
          /* Do nothing, error code should already be set */
        }

        break;
      }

      case PS_INIT_DATA_MAPPING:
      {
        /* Search for all the valid data */
        error = BuildDataMap ();

        if (PS_OK == error)
        {
          if (TRUE == isCleanUpNeeded)
          {
            currentState = PS_INIT_CLEANUP;
          }
          else
          {
            /* Update state, operation is over */
            currentState = PS_INIT_OP_OVER;
          }
        }

        break;
      }

      case PS_INIT_CLEANUP:
      {
        /* Before finishing the init, a cleanup is required */
        error = CleanUpOp (FALSE);

        if (PS_OK == error)
        {
          /* Update state, operation is over */
          currentState = PS_INIT_OP_OVER;
        }
        else if (PS_OPERATION_PAUSED == error)
        {
          /* Setup context keeping mask */
          SetContextKeeping (PS_CLEANUPOP_ID);
        }
        else
        {
          /* Do nothing, error code shall already be set */
        }

        break;
      }

      case PS_INIT_OP_OVER:
      {
        /* Do nothing, should never be reached */
        break;
      }

      default:
      {
        /* Do nothing, leave */
        break;
      }
    }
  }

  return error;
}

PS_Cmd_Status_e WriteOp (const PS_Data_ID_e DataID,
                                 const uint8_t * const p_DataSrc,
                                 const uint32_t DataSize)
{
  PS_Cmd_Status_e error = PS_OK;

  uint32_t writtenDataSz = 0x00;
  uint32_t * writeAddress = NULL;

  PS_BankHeader_t currentBankHeader = {0xFFu};

  static const uint8_t * p_dataSrcAddress = NULL;
  static uint32_t dataSize = 0x00;
  static PS_Data_ID_e dataId = PS_DATA_ID_MAX;

  static uint8_t bankId = 0xFF;
  static uint32_t writeBankIndex = 0x00;
  static uint32_t eraseCounterValue = 0x00;
  static uint32_t writeFailureCounter = 0x00;
  static uint32_t * p_failedDataStartAddress = NULL;
  static PS_WritingTarget_e bankType = PS_NO_BANK;

  static PS_WriteStates_e currentState = PS_WRITE_GET_WRITE_BANK;

  /* ---- Starting setup -----------------------------------------------------*/
  /* Is it start from scratch ? */
  if (FALSE == IsContextToBeKept (PS_WRITEOP_ID))
  {
    /* Store parameters */
    p_dataSrcAddress = p_DataSrc;
    dataSize = DataSize;
    dataId = DataID;

    /* Reset static variables */
    bankId = 0xFF;
    writeBankIndex = 0x00;
    eraseCounterValue = 0x00;
    writeFailureCounter = 0x00;
    p_failedDataStartAddress = NULL;
    bankType = PS_NO_BANK;
    currentState = PS_WRITE_GET_WRITE_BANK;
  }
  else
  {
    /* Reset context keeping mask */
    ResetContextKeeping (PS_WRITEOP_ID);
  }

  /* ---- Writing operation --------------------------------------------------*/
  while ((PS_OK == error) && (PS_WRITE_OP_OVER != currentState))
  {
    switch (currentState)
    {
      case PS_WRITE_GET_WRITE_BANK:
      {
        writeAddress = NULL;

        /* Get the future address for data writing */
        bankType = GetWriteBank (PS_WRITE_OP,
                                 dataId,
                                 dataSize,
                                 &writeBankIndex);

        /* Setup the write address */
        writeAddress = ap_BankList[writeBankIndex]->p_NextWriteAddress;
        p_failedDataStartAddress = writeAddress;

        /* Check if a write spot has been found */
        if ((NULL == writeAddress) || (PS_NO_BANK == bankType))
        {
          /* No write target available */
          error = PS_ERROR_NO_WRITE_TARGET;
        }
        else
        {
          currentState = PS_WRITE_DATA;
        }

        break;
      }

      case PS_WRITE_DATA:
      {
        /* Try to write the data */
        error = WriteData (writeAddress,
                           dataId,
                           p_dataSrcAddress,
                           dataSize,
                           &writtenDataSz);

        /* No error encountered */
        if (PS_OK == error)
        {
          /* Update occupation table with the written size - Header and padding included */
          a_OccupationTable[dataId] = writtenDataSz;

          /* Update bank information */
          UpdateBankInformation (dataId,
                                 writeBankIndex,
                                 writtenDataSz);

          if (PS_SPARE_BANK == bankType)
          {
            /* Inform user that a cleanup is required */
            error = PS_CLEANUP_REQUIRED;
          }

          /* Leave the loop */
          currentState = PS_WRITE_OP_OVER;
        }
        /* Check if an invalid write has happened */
        else if (PS_ERROR_INVALID == error)
        {
          writeFailureCounter++;

          if (PS_WRITE_FAILURE_LIMIT <= writeFailureCounter)
          {
            currentState = PS_WRITE_OP_OVER;
          }
          else if (PS_NORMAL_BANK == bankType)
          {
            currentState = PS_WRITE_ERROR_NORMAL_BANK;
          }
          else if (PS_SPARE_BANK == bankType)
          {
            currentState = PS_WRITE_ERROR_SPARE_BANK_ERASE;
          }

          /* Reset error code */
          error = PS_OK;
        }
        else if (PS_OPERATION_PAUSED == error)
        {
          /* Setup context keeping mask */
          SetContextKeeping (PS_WRITEDATA_ID);
        }
        else
        {
          /* Do nothing, error code should already be set */
        }

        break;
      }

      case PS_WRITE_ERROR_NORMAL_BANK:
      {
        /* Invalidate first line of the failed data packet (header line). */
        /* TODO: Determine whether or not this call shall be blocking or not */
        if (NULL != p_failedDataStartAddress)
        {
          (void)InvalidateLine (p_failedDataStartAddress);
        }
        else
        {
          (void)InvalidateLine (writeAddress);
        }

        /* Update bank remaining space */
        ap_BankList[writeBankIndex]->RemainingSpace = 0x00;

        /* Restart a new write operation on another bank */
        currentState = PS_WRITE_GET_WRITE_BANK;

        /* Reset error code to keep state machine running */
        error = PS_OK;

        break;
      }

      case PS_WRITE_ERROR_SPARE_BANK_ERASE:
      {
        /* Check if erase counter is known */
        if ((0xFF == bankId) &&
            (0x00 == eraseCounterValue))
        {
          /* First get the erase counter */
          error = ReadFlashLine (ap_BankList[writeBankIndex]->p_StartAddress,
                                (uint32_t *)&currentBankHeader);

          if (PS_OK == error)
          {
            /* Before erasing the bank, recover the bank id ... */
            bankId = currentBankHeader.BankId;

            /* ... and the erase counter value */
            eraseCounterValue = PS_GET_ERASE_COUNTER (currentBankHeader.BankInfo);
          }
        }

        if (PS_OK == error)
        {
          /* Erase the spare bank before any new write operation */
          error = EraseBank (writeBankIndex);

          if (PS_OK == error)
          {
            /* Update the erase counter */
            eraseCounterValue++;

            /* Re-write the bank header */
            currentState = PS_WRITE_ERROR_SPARE_BANK_WRITE_HEADER;
          }
          else if (PS_OPERATION_PAUSED == error)
          {
            /* Setup context keeping mask */
            SetContextKeeping (PS_ERASEBANK_ID);
          }
          else
          {
            /* Do nothing, error code shall already be set */
          }
        }

        break;
      }

      case PS_WRITE_ERROR_SPARE_BANK_WRITE_HEADER:
      {
        error = WriteBankHeader (bankId,
                                 eraseCounterValue);

        if (PS_OK == error)
        {
          /* Update the state */
          currentState = PS_WRITE_OP_OVER;

          /* Update error code, it was an invalid write */
          error = PS_ERROR_INVALID;
        }
        else if (PS_ERROR_INVALID == error)
        {
          writeFailureCounter++;

          if (PS_WRITE_FAILURE_LIMIT <= writeFailureCounter)
          {
            currentState = PS_WRITE_OP_OVER;
          }
          else
          {
            /* Let's try again; Erase the bank */
            currentState = PS_WRITE_ERROR_SPARE_BANK_ERASE;

            /* Reset the error code */
            error = PS_OK;
          }
        }
        else if (PS_OPERATION_PAUSED == error)
        {
          /* Setup context keeping mask */
          SetContextKeeping (PS_WRITEBANKHEADER_ID);
        }
        else
        {
          /* Do nothing, error code should already be set */
        }
        break;
      }

      case PS_WRITE_OP_OVER:
      {
        /* Do nothing, should never be reached */
        break;
      }

      default:
      {
        /* Do nothing, should never be reached */
        break;
      }
    }
  }

  return error;
}

PS_Cmd_Status_e RestoreOp (const PS_Data_ID_e DataID,
                                   uint8_t * const p_DataDest,
                                   const uint32_t DataSize)
{
  PS_Cmd_Status_e error = PS_DATA_NOT_FOUND;
  PS_Cmd_Status_e candidateError = PS_DATA_NOT_FOUND;

  uint32_t occupationSize = 0x00;

  /* Parameters */
  static uint8_t * p_dataDestAddress = NULL;
  static uint32_t dataDestSize = 0x00;
  static PS_Data_ID_e dataId = PS_DATA_ID_MAX;

  static int32_t currentBankIdx = 0xFF;
  static uint32_t hostingBankIndex = 0xFF;
  static uint32_t missingDataMask = 0x00;
  static uint32_t * p_dataAddr = NULL;
  static uint32_t * a_possibleDataAddr [3u] = {NULL, NULL, NULL};
  static PS_RestoreStates_e currentState = PS_RESTORE_GET_BANK;

  /* Not used for the time being */
  if (FALSE == IsContextToBeKept (PS_RESTOREOP_ID))
  {
    /* Store parameters */
    p_dataDestAddress = p_DataDest;
    dataDestSize = DataSize;
    dataId = DataID;

    /* Reset potential hosting bank */
    hostingBankIndex = 0xFF;

    /* Reset missing data mask */
    missingDataMask = 0x00;

    /* Reset valid data address */
    p_dataAddr = NULL;

    /* Reset possible data addresses */
    a_possibleDataAddr[0u] = NULL;
    a_possibleDataAddr[1u] = NULL;
    a_possibleDataAddr[2u] = NULL;

    /* Reset state */
    currentState = PS_RESTORE_GET_BANK;

    /* Reset bank loop index */
    currentBankIdx = (CurrentConfiguration.BankNumber - 2u);
  }
  else
  {
    /* Reset context keeping mask */
    ResetContextKeeping (PS_RESTOREOP_ID);
  }

  /* Search for a valid version of the data inside the others banks */
  while ((0x00 <= currentBankIdx) && (PS_DATA_NOT_FOUND == error))
  {
    switch (currentState)
    {
      case PS_RESTORE_GET_BANK:
      {
        /* First get the hosting bank */
        GetHostingBank (dataId,
                        currentBankIdx,
                        &hostingBankIndex);

        /* Data bank found ? */
        if (0xFF == hostingBankIndex)
        {
          /* Leave the loop */
          currentBankIdx = -1;
        }
        else
        {
          /* Starting point is the hosting bank */
          currentBankIdx = hostingBankIndex;

          /* Update state */
          currentState = PS_RESTORE_SEARCH_HIT;
        }

        break;
      }

      case PS_RESTORE_SEARCH_HIT:
      {
        uint8_t validDataFound = FALSE;

        /* Search if there is a valid data inside the indicated bank */
        error = SearchData (dataId,
                            currentBankIdx,
                            p_dataAddr,
                            a_possibleDataAddr);

        if (PS_OK == error)
        {
          /* Check candidates from newest to oldest */
          for (int8_t idx = 2;
               (0 <= idx) && (FALSE == validDataFound) && (PS_OK == error);
               idx--)
          {
            if (NULL != a_possibleDataAddr[idx])
            {
              candidateError = CheckDataValidity (a_possibleDataAddr[idx]);

              if (PS_OK == candidateError)
              {
                p_dataAddr = a_possibleDataAddr[idx];
                validDataFound = TRUE;
              }
              else if ((PS_CRC_ERROR == candidateError) ||
                       (PS_ERROR_INVALID == candidateError) ||
                       (PS_DATA_NOT_FOUND == candidateError))
              {
                /* Candidate is not valid, continue with older one */
                error = PS_OK;
              }
              else
              {
                error = candidateError;
              }
            }
          }

          if ((PS_OK == error) && (TRUE == validDataFound))
          {
            /* Update state */
            currentState = PS_RESTORE_CONFIRM_HIT;

            /* Reset error code for loop purposes */
            error = PS_DATA_NOT_FOUND;
          }
          else if (PS_OK == error)
          {
            /* No valid candidate found */
            error = PS_DATA_NOT_FOUND;
          }
          else
          {
            /* Error code should already be set */
          }
        }

        if ((PS_DATA_NOT_FOUND == error) && (FALSE == validDataFound))
        {
          /* Clean the presence bitmask */
          ap_BankList[currentBankIdx]->PresenceBitmask &= ~(1u << dataId);

          /* Search the data in another bank */
          currentBankIdx--;

          /* Reset the data address */
          p_dataAddr = NULL;
        }
        else if (PS_ERROR_INVALID == error)
        {
          /* Update state to search for missing data */
          currentState = PS_RESTORE_SEARCH_MISSING_DATA;

          /* Reset error code for loop purposes */
          error = PS_DATA_NOT_FOUND;
        }
        else if (PS_OPERATION_PAUSED == error)
        {
          /* Keep context in RestoreOp itself */
        }
        else
        {
          /* Error code should already be set */
        }

        break;
      }

      case PS_RESTORE_CONFIRM_HIT:
      {
        error = ConfirmValidData (p_dataAddr,
                                  p_dataDestAddress,
                                  dataDestSize,
                                  &occupationSize);

        if (PS_OK == error)
        {
          /* Update occupation table */
          a_OccupationTable[dataId] = (uint32_t)PS_ALIGN_FLASH_LINE (occupationSize +
                                                                              sizeof (PS_DataHeader_t) -
                                                                              PS_HEADER_DATA_NUMBER);

          /* Update the information regarding the valid data location */
          ap_BankList[currentBankIdx]->PresenceBitmask |= (1u << dataId);
        }
        else if (PS_CRC_ERROR == error)
        {
          /* Not sure if validity is NOK due to a corrupted header or a corrupted data */
          /* Data shall be invalidated */
          /* TODO: Check if it should be blocking */
          (void)InvalidateLine (p_dataAddr);

          /* Limit the bank capacity */
          ap_BankList[currentBankIdx]->p_NextWriteAddress = p_dataAddr;
          ap_BankList[currentBankIdx]->RemainingSpace = 0x00;

          /* Update state */
          currentState = PS_RESTORE_SEARCH_MISSING_DATA;

          /* Reset error code for loop purposes */
          error = PS_DATA_NOT_FOUND;
        }
        else if (PS_OPERATION_PAUSED == error)
        {
          /* Setup context keeping mask */
          SetContextKeeping (PS_CONFIRMVALIDDATA_ID);
        }
        else
        {
          /* Error code should already be set */
        }

        break;
      }

      case PS_RESTORE_SEARCH_MISSING_DATA:
      {
        /* Setup the missing data mask */
        missingDataMask = ap_BankList[currentBankIdx]->PresenceBitmask;

        /* Reset the presence bitmask as we might have lost some data */
        ap_BankList[currentBankIdx]->PresenceBitmask = 0x00;

        /* Search for missing data */
        error = RecoverValidData (currentBankIdx,
                                  missingDataMask);

        if (PS_OK == error)
        {
          /* Reset possible hosting bank */
          hostingBankIndex = 0xFF;

          /* Reset missing data bitmask */
          missingDataMask = 0x00;

          /* Keep on searching */
          currentState = PS_RESTORE_GET_BANK;

          /* Reset error code for loop purposes */
          error = PS_DATA_NOT_FOUND;
        }
        else if (PS_OPERATION_PAUSED == error)
        {
          /* Setup context keeping mask */
          SetContextKeeping (PS_RECOVERVALIDDATA_ID);
        }
        else
        {
          /* Error code should already be set */
        }

        break;
      }

      default:
      {
        /* Shall not be reached */
        error = PS_NOK;
        break;
      }
    }
  }

  return error;
}

PS_Cmd_Status_e CleanUpOp (const uint8_t AgingLimitTriggerFlag)
{
  PS_Cmd_Status_e error = PS_OK;

  uint8_t agingFlag = FALSE;

  PS_BankHeader_t currentBankHeader = {0xFF};

  /* Parameter */
  static uint8_t agingLimitFlag = FALSE;

  static uint8_t bankId = 0xFF;
  static uint32_t futureSpareBankIdx = 0xFF;
  static uint32_t eraseCounterValue = 0x00;
  static PS_CleanupStates_e currentState = PS_CLEANUP_BANK_SWAP;

  /* ---- Starting setup -----------------------------------------------------*/
  /* Is it start from scratch ? */
  if (FALSE == IsContextToBeKept (PS_CLEANUPOP_ID))
  {
    /* Store parameter */
    agingLimitFlag = AgingLimitTriggerFlag;

    /* Reset the index */
    futureSpareBankIdx = 0xFF;

    /* Reset the bank id */
    bankId = 0xFF;

    /* Reset the erase counter */
    eraseCounterValue = 0x00;

    /* Aging limit has been triggered ? */
    if (FALSE == agingLimitFlag)
    {
      /* Get the future spare bank */
      GetFutureSpareBank (&futureSpareBankIdx);
    }
    /* (TRUE == agingLimitFlag) */
    else
    {
      /* Get the newest bank of the list */
      GetNewestBank (&futureSpareBankIdx);
    }

    /* Set the future operation state */
    currentState = PS_CLEANUP_BANK_SWAP;
  }
  else
  {
    /* Reset context keeping mask */
    ResetContextKeeping (PS_CLEANUPOP_ID);
  }

  /* ---- State machine for operations ---------------------------------------*/
  while ((PS_OK == error) && (PS_CLEANUP_CHECK_OP_OVER != currentState))
  {
    /* State checker */
    switch (currentState)
    {
      case PS_CLEANUP_BANK_SWAP:
      {
        error = SwapBank (futureSpareBankIdx);

        if (PS_OK == error)
        {
          /* Update the state */
          currentState = PS_CLEANUP_ERASE_FUTURE_SPARE_BANK;
        }
        else if (PS_OPERATION_PAUSED == error)
        {
          /* Setup context keeping mask */
          SetContextKeeping (PS_SWAPBANK_ID);
        }
        else
        {
          /* Do nothing, error code shall already be set */
        }

        break;
      }

      case PS_CLEANUP_ERASE_FUTURE_SPARE_BANK:
      {
        if ((0xFF == bankId) &&
            (0x00 == eraseCounterValue))
        {
          /* Information shall be updated, read the bank header */
          error = ReadFlashLine(ap_BankList[futureSpareBankIdx]->p_StartAddress,
                                (uint32_t *)&currentBankHeader);

          if (PS_OK == error)
          {
            /* Before erasing the bank, recover the bank id ... */
            bankId = currentBankHeader.BankId;

            /* ... and the erase counter value */
            eraseCounterValue = PS_GET_ERASE_COUNTER (currentBankHeader.BankInfo);
          }
        }

        if (PS_OK == error)
        {
          error = EraseBank (futureSpareBankIdx);

          if (PS_OK == error)
          {
            /* Update erase counter before bank header write */
            eraseCounterValue++;

            /* Update the state */
            currentState = PS_CLEANUP_WRITE_NEW_SPARE_BANK_HEADER;
          }
          else if (PS_OPERATION_PAUSED == error)
          {
            /* Setup context keeping mask */
            SetContextKeeping (PS_ERASEBANK_ID);
          }
          else
          {
            /* Do nothing, error code shall already be set */
          }
        }

        break;
      }

      case PS_CLEANUP_WRITE_NEW_SPARE_BANK_HEADER:
      {
        error = WriteBankHeader (bankId,
                                 eraseCounterValue);

        if (PS_OK == error)
        {
          /* Update the state */
          currentState = PS_CLEANUP_UPDATE_TRACKERS;
        }
        else if (PS_ERROR_INVALID == error)
        {
          /* Let's try again; Erase the bank */
          currentState = PS_CLEANUP_ERASE_FUTURE_SPARE_BANK;

          /* Reset the error code */
          error = PS_OK;
        }
        else if (PS_OPERATION_PAUSED == error)
        {
          /* Setup context keeping mask */
          SetContextKeeping (PS_WRITEBANKHEADER_ID);
        }
        else
        {
          /* Do nothing, error code should already be set */
        }
        break;
      }

      case PS_CLEANUP_UPDATE_TRACKERS:
      {
        /* Update all the trackers information */
        error = OrganizeBankList (futureSpareBankIdx);

        if (PS_OK == error)
        {
          /* Update the state */
          currentState = PS_CLEANUP_CHECK_AGING_LIMIT;
        }
        else if (PS_OPERATION_PAUSED == error)
        {
          /* Setup context keeping mask */
          SetContextKeeping (PS_ORGANIZEBANKLIST_ID);
        }
        else
        {
          /* Do nothing, error code should already be set */
        }

        break;
      }

      case PS_CLEANUP_CHECK_AGING_LIMIT:
      {
        /* Cleanup has been performed, check if the aging limit has been overlapped */
        error = IsAgingLimitExceed (&agingFlag);

        if (PS_OK == error)
        {
          if (TRUE == agingFlag)
          {
            error = PS_CLEANUP_REQUIRED;
          }

          /* Update the state */
          currentState = PS_CLEANUP_CHECK_OP_OVER;
        }

        break;
      }

      case PS_CLEANUP_CHECK_OP_OVER:
      {
        /* Do nothing, should never be reached */
        break;
      }

      default:
      {
        /* Do nothing, just leave */
        break;
      }
    }
  }

  return error;
}

/* Weak function Definition -----------------------------------------------*/
__WEAK PS_Cmd_Status_e CheckDataValidity (const uint32_t * const p_DataAddress)
{
  PS_Cmd_Status_e error = PS_OK;

  PS_DataHeader_t currentDataHeader = {0u};

  /* Information shall be updated */
  error = ReadFlashLine (p_DataAddress,
                         (uint32_t *)&currentDataHeader);

  if (PS_OK == error)
  {
    /* Since flash is internal, just call the above function */
    error = CheckDataIntegrity (&currentDataHeader,
                                (uint32_t *)((uint32_t)p_DataAddress +
                                              PS_FLASH_LINE_LENGTH -
                                              PS_HEADER_DATA_NUMBER));
  }

  return error;
}

__WEAK PS_Cmd_Status_e CopyData (uint32_t * const p_WriteAddress,
                                         const uint32_t * const p_DataAddr,
                                         const uint32_t DataSize,
                                         uint32_t * const p_WrittenBytes)
{
  PS_Cmd_Status_e error = PS_OK;

#if (PS_FLASH_LINE_LENGTH == PS_FLASH_LINE_128B)
  uint8_t a_writtenFlashLine [PS_FLASH_LINE_128B] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                                             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  uint8_t a_readFlashLine [PS_FLASH_LINE_128B] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                                          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
#else
  uint8_t a_writtenFlashLine [PS_FLASH_LINE_64B] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  uint8_t a_readFlashLine [PS_FLASH_LINE_64B] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
#endif

  static uint32_t writeIdx = 0x00;

  /* Shall context be kept ? */
  if (FALSE == IsContextToBeKept (PS_COPYDATA_ID))
  {
    writeIdx = 0x00;
  }
  else
  {
    /* Reset context keeping mask */
    ResetContextKeeping (PS_COPYDATA_ID);
  }

  /* Copy from one bank to another */
  while ((DataSize > writeIdx) && (PS_OK == error))
  {
    /* Write one line */
    error = PS_FlashDriver.Write ((uint32_t *)((uint32_t)p_WriteAddress + writeIdx),
                                          (uint32_t *)((uint32_t)p_DataAddr + writeIdx));

    /* Write OK ? */
    if (PS_OK == error)
    {
      /* Read it back to check if all is OK */
      error = ReadFlashLine ((uint32_t *)((uint32_t)p_WriteAddress + writeIdx),
                             (uint32_t *)a_writtenFlashLine);

      if (PS_OK == error)
      {
        /* Read content that should have been written */
        error = ReadFlashLine ((uint32_t *)((uint32_t)p_DataAddr + writeIdx),
                               (uint32_t *)a_readFlashLine);

        if (PS_OK == error)
        {
          if (PS_IS_FLASH_LINE_OK (a_writtenFlashLine,
                                           a_readFlashLine))
          {
            /* Update written bytes count */
            *p_WrittenBytes += PS_FLASH_LINE_LENGTH;

            /* Update write index */
            writeIdx += PS_FLASH_LINE_LENGTH;
          }
          else
          {
            error = PS_ERROR_INVALID;
          }
        }
      }
    }
  }

  return  error;
}
