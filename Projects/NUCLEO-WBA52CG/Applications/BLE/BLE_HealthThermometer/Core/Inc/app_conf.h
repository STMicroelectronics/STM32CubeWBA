/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_conf.h
  * @author  MCD Application Team
  * @brief   Application configuration file for STM32WPAN Middleware.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_CONF_H
#define APP_CONF_H

/* Includes ------------------------------------------------------------------*/
#include "hw_if.h"
#include "stm32_adv_trace.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/******************************************************************************
 * Application Config
 ******************************************************************************/
/**< generic parameters ******************************************************/

/**
 * Define Tx Power
 */
#define CFG_TX_POWER                      (0x19) /* 0x19 <=> -0.3 dBm */

/**
 * Define Advertising parameters
 */
#define CFG_BD_ADDRESS                    (0x0008E12A1234)
#define CFG_BD_ADDRESS_TYPE               (GAP_PUBLIC_ADDR)

#define ADV_INTERVAL_MIN                  (0x0080)
#define ADV_INTERVAL_MAX                  (0x00A0)
#define ADV_LP_INTERVAL_MIN               (0x0640)
#define ADV_LP_INTERVAL_MAX               (0x0FA0)
#define ADV_TYPE                          ADV_IND
#define ADV_FILTER                        NO_WHITE_LIST_USE

/**
 * Define IO Authentication
 */
#define CFG_BONDING_MODE                 (1)
#define CFG_FIXED_PIN                    (111111)
#define CFG_USED_FIXED_PIN               (0)
#define CFG_ENCRYPTION_KEY_SIZE_MAX      (16)
#define CFG_ENCRYPTION_KEY_SIZE_MIN      (8)

/**
 * Define Input Output capabilities
 */
#define CFG_IO_CAPABILITY                (IO_CAP_DISPLAY_YES_NO)

/**
 * Define Man In The Middle modes
 */
#define CFG_MITM_PROTECTION              (MITM_PROTECTION_REQUIRED)

/**
 * Define Secure Connections Support
 */
#define CFG_SECURE_NOT_SUPPORTED              (0x00)
#define CFG_SECURE_OPTIONAL                   (0x01)
#define CFG_SECURE_MANDATORY                  (0x02)

#define CFG_SC_SUPPORT                        CFG_SECURE_OPTIONAL

/**
 * Define Keypress Notification Support
 */
#define CFG_KEYPRESS_NOTIFICATION_SUPPORT     (KEYPRESS_NOT_SUPPORTED)

/**
*   Identity root key used to derive LTK and CSRK
*/
#define CFG_BLE_IRK     {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0}

/**
* Encryption root key used to derive LTK and CSRK
*/
#define CFG_BLE_ERK     {0xFE, 0xDC, 0xBA, 0x09, 0x87, 0x65, 0x43, 0x21, 0xFE, 0xDC, 0xBA, 0x09, 0x87, 0x65, 0x43, 0x21}

/* USER CODE BEGIN Generic_Parameters */

/* USER CODE END Generic_Parameters */

/**< specific parameters */
/*****************************************************/

/* USER CODE BEGIN Specific_Parameters */

/* USER CODE END Specific_Parameters */

/******************************************************************************
 * BLE Stack
 ******************************************************************************/
/**
 * BLE stack options, bitmap to active or not some features at BleStack_Init() function call.
 */
#define CFG_BLE_OPTIONS             (0 | \
                                     0 | \
                                     0 | \
                                     0 | \
                                     0 | \
                                     0 | \
                                     0 | \
                                     0)

/**
 * Maximum number of simultaneous connections that the device will support.
 * Valid values are from 1 to 8
 * This setting should not exceed the number of BLE connection supported by
 * the ble host
 */
#define CFG_BLE_NUM_LINK            (2)

/**
 * Maximum number of Services that can be stored in the GATT database.
 * Note that the GAP and GATT services are automatically added so this parameter should be 2 plus the number of user services
 */
#define CFG_BLE_NUM_GATT_SERVICES   (8)

/**
 * Maximum number of Attributes
 * (i.e. the number of characteristic + the number of characteristic values + the number of descriptors, excluding the services)
 * that can be stored in the GATT database.
 * Note that certain characteristics and relative descriptors are added automatically during device initialization
 * so this parameters should be 9 plus the number of user Attributes
 */
#define CFG_BLE_NUM_GATT_ATTRIBUTES (68)

/**
 * Maximum supported ATT_MTU size
 * This setting should be aligned with ATT_MTU value configured in the ble host
 */
#define CFG_BLE_ATT_MTU_MAX         (251)

/**
 * Size of the storage area for Attribute values
 *  This value depends on the number of attributes used by application. In particular the sum of the following quantities (in octets) should be made for each attribute:
 *  - attribute value length
 *  - 5, if UUID is 16 bit; 19, if UUID is 128 bit
 *  - 2, if server configuration descriptor is used
 *  - 2*DTM_NUM_LINK, if client configuration descriptor is used
 *  - 2, if extended properties is used
 *  The total amount of memory needed is the sum of the above quantities for each attribute.
 */
#define CFG_BLE_ATT_VALUE_ARRAY_SIZE    (1344)

/**
 * depth of the PREPARE WRITE queue when PREPARE WRITE REQUEST
 */
#define CFG_BLE_ATTR_PREPARE_WRITE_VALUE_SIZE       (30)

#define CFG_BLE_MBLOCK_COUNT_MARGIN                 (0x15)

#define PREP_WRITE_LIST_SIZE                        (BLE_DEFAULT_PREP_WRITE_LIST_SIZE)

/**
 * Number of allocated memory blocks used to transmit and receive data packets
 */
#define CFG_BLE_MBLOCK_COUNT (BLE_MBLOCKS_CALC(PREP_WRITE_LIST_SIZE, \
                                       CFG_BLE_ATT_MTU_MAX, CFG_BLE_NUM_LINK) \
                                   + CFG_BLE_MBLOCK_COUNT_MARGIN)

/**
 * Maximum supported Devices in BLE Database
 */
#define CFG_BLE_MAX_DDB_ENTRIES     (20)

/**
 * Appearance of device set into BLE GAP
 */
#define CFG_GAP_APPEARANCE          (GAP_APPEARANCE_UNKNOWN)

/**
 * Connection Oriented Channel parameters
 */
#define CFG_BLE_COC_NBR_MAX                         (64)
#define CFG_BLE_COC_MPS_MAX                         (248)
#define CFG_BLE_COC_INITIATOR_NBR_MAX               (32)

/* USER CODE BEGIN BLE_Stack */

/* USER CODE END BLE_Stack */

/******************************************************************************
 * Low Power
 *
 *  When CFG_FULL_LOW_POWER is set to 1, the system is configured in full
 *  low power mode. It means that all what can have an impact on the consumptions
 *  are powered down.(For instance LED, Access to Debugger, Etc.)
 *
 *  When CFG_FULL_LOW_POWER is set to 0, the low power mode is not activated
 *
 ******************************************************************************/
#define CFG_FULL_LOW_POWER       (0)

#define CFG_LPM_SUPPORTED        (0)
#define CFG_LPM_STDBY_SUPPORTED  (0)

/**
 * Low Power configuration
 */
#if (CFG_FULL_LOW_POWER == 1)
  #undef CFG_LPM_SUPPORTED
  #define CFG_LPM_SUPPORTED      (1)

  #undef  CFG_DBG_SUPPORTED
  #define CFG_DBG_SUPPORTED      (0)

#else
  #undef CFG_LPM_SUPPORTED
  #define CFG_LPM_SUPPORTED      (0)
#endif /* CFG_FULL_LOW_POWER */

/* USER CODE BEGIN Low_Power 0 */

/* USER CODE END Low_Power 0 */

/**
 * Supported requester to the MCU Low Power Manager - can be increased up  to 32
 * It list a bit mapping of all user of the Low Power Manager
 */
typedef enum
{
  CFG_LPM_APP,
  /* USER CODE BEGIN CFG_LPM_Id_t */

  /* USER CODE END CFG_LPM_Id_t */
} CFG_LPM_Id_t;

/* USER CODE BEGIN Low_Power 1 */

/* USER CODE END Low_Power 1 */

/******************************************************************************
 * RTC
 ******************************************************************************/
#define RTC_N_PREDIV_S (10)
#define RTC_PREDIV_S ((1<<RTC_N_PREDIV_S)-1)
#define RTC_PREDIV_A ((1<<(15-RTC_N_PREDIV_S))-1)

/* USER CODE BEGIN RTC */

/* USER CODE END RTC */

/*****************************************************************************
 * Traces
 * Enable or Disable traces in application
 * When CFG_DEBUG_TRACE is set, traces are activated
 *
 * Note : Refer to utilities_conf.h file in order to details
 *        the level of traces : CFG_DEBUG_TRACE_FULL or CFG_DEBUG_TRACE_LIGHT
 *****************************************************************************/

#define ADV_TRACE_TIMESTAMP_ENABLE  (0U)

/**
 * Enable or Disable traces in application
 */
#define CFG_DEBUG_APP_TRACE         (1)

/* New implementation using stm32_adv_trace */
#define APP_DBG(...)                                                                  \
{                                                                                     \
  UTIL_ADV_TRACE_COND_FSend(VLEVEL_L, ~0x0, ADV_TRACE_TIMESTAMP_ENABLE, __VA_ARGS__); \
}

#if (CFG_DEBUG_APP_TRACE != 0)
#define APP_DBG_MSG                 APP_DBG
#else
#define APP_DBG_MSG(...)
#endif

#if (CFG_DEBUG_APP_TRACE != 0)
#define CFG_DEBUG_TRACE             (1)
#endif

#if (CFG_DEBUG_TRACE != 0)
#undef CFG_LPM_SUPPORTED
#define CFG_LPM_SUPPORTED           (0)
#endif

/**
 * When CFG_DEBUG_TRACE_FULL is set to 1, the trace are output with the API name, the file name and the line number
 * When CFG_DEBUG_TRACE_LIGHT is set to 1, only the debug message is output
 *
 * When both are set to 0, no trace are output
 * When both are set to 1,  CFG_DEBUG_TRACE_FULL is selected
 */
#define CFG_DEBUG_TRACE_LIGHT       (0)
#define CFG_DEBUG_TRACE_FULL        (1)

#if (( CFG_DEBUG_TRACE != 0 ) && ( CFG_DEBUG_TRACE_LIGHT == 0 ) && (CFG_DEBUG_TRACE_FULL == 0))
#undef CFG_DEBUG_TRACE_FULL
#undef CFG_DEBUG_TRACE_LIGHT
#define CFG_DEBUG_TRACE_FULL        (0)
#define CFG_DEBUG_TRACE_LIGHT       (1)
#endif

#if ( CFG_DEBUG_TRACE == 0 )
#undef CFG_DEBUG_TRACE_FULL
#undef CFG_DEBUG_TRACE_LIGHT
#define CFG_DEBUG_TRACE_FULL        (0)
#define CFG_DEBUG_TRACE_LIGHT       (0)
#endif

/**
 * When not set, the traces is looping on sending the trace over UART
 */
#define DBG_TRACE_USE_CIRCULAR_QUEUE  (1)

/**
 * Max buffer size to queue data traces and max data trace allowed.
 * Only Used if DBG_TRACE_USE_CIRCULAR_QUEUE is defined
 */
#define DBG_TRACE_MSG_QUEUE_SIZE      (4096)
#define MAX_DBG_TRACE_MSG_SIZE        (1024)

/**
 * Max message size for debug logging service
 */
#define SYS_MAX_MSG                 (200U)

/* USER CODE BEGIN Traces */

/* USER CODE END Traces */

/******************************************************************************
 * Configure Log level for Application
 ******************************************************************************/
#define APPLI_CONFIG_LOG_LEVEL      (LOG_LEVEL_INFO)
#define APPLI_PRINT_FILE_FUNC_LINE  (0)

/* USER CODE BEGIN Log_level */

/* USER CODE END Log_level */

/******************************************************************************
 * Sequencer
 ******************************************************************************/

/**
 * These are the lists of task id registered to the sequencer
 * Each task id shall be in the range [0:31]
 */
typedef enum
{
  CFG_TASK_HCI_ASYNCH_EVT_ID,
  CFG_TASK_LINK_LAYER,
  CFG_TASK_LINK_LAYER_TEMP_MEAS,
  CFG_TASK_BLE_HOST,
  CFG_TASK_BPKA,
  CFG_TASK_HW_RNG,
  CFG_TASK_AMM_BCKGND,
  CFG_TASK_FLASH_MANAGER_BCKGND,
  CFG_TASK_BLE_TIMER_BCKGND,
  /* USER CODE BEGIN CFG_Task_Id_t */
  TASK_BUTTON_1,
  TASK_BUTTON_2,
  TASK_BUTTON_3,
  CFG_TASK_HTS_MEAS_REQ_ID,
  CFG_TASK_HTS_INTERMEDIATE_TEMPERATURE_REQ_ID,
  CFG_TASK_HTS_MEAS_INTERVAL_REQ_ID,
  /* USER CODE END CFG_Task_Id_t */
  CFG_TASK_NBR /* Shall be LAST in the list */
} CFG_Task_Id_t;

/* USER CODE BEGIN DEFINE_TASK */

/* USER CODE END DEFINE_TASK */

/**
 * This is the list of priority required by the application
 * Shall be in the range 0..31
 */
typedef enum
{
  CFG_SEQ_PRIO_0 = 0,
  CFG_SEQ_PRIO_1,
  /* USER CODE BEGIN CFG_SEQ_Prio_Id_t */

  /* USER CODE END CFG_SEQ_Prio_Id_t */
  CFG_SEQ_PRIO_NBR /* Shall be LAST in the list */
} CFG_SEQ_Prio_Id_t;

/**
 * This is a bit mapping over 32bits listing all events id supported in the application
 */
typedef enum
{
  CFG_IDLEEVT_PROC_GAP_COMPLETE,
  /* USER CODE BEGIN CFG_IdleEvt_Id_t */

  /* USER CODE END CFG_IdleEvt_Id_t */
} CFG_IdleEvt_Id_t;

/******************************************************************************
 * NVM configuration
 ******************************************************************************/

#define CFG_SNVMA_START_SECTOR_ID     (FLASH_PAGE_NB - 2u)

#define CFG_SNVMA_START_ADDRESS       (FLASH_BASE + (FLASH_PAGE_SIZE * (CFG_SNVMA_START_SECTOR_ID)))

/* USER CODE BEGIN NVM_Configuration */

/* USER CODE END NVM_Configuration */

/******************************************************************************
 * BLEPLAT configuration
 ******************************************************************************/

/* Number of 64-bit words in NVM flash area */
#define CFG_BLEPLAT_NVM_MAX_SIZE            ((2048/8)-4)

/* USER CODE BEGIN BLEPLAT_Configuration */

/* USER CODE END BLEPLAT_Configuration */

/******************************************************************************
 * RT GPIO debug module configuration
 ******************************************************************************/

#define RT_DEBUG_GPIO_MODULE         (0)
#define RT_DEBUG_DTB                 (0)

/******************************************************************************
 * HW RADIO configuration
 ******************************************************************************/
/* Link Layer uses radio low interrupt (0 --> NO ; 1 --> YES) */
#define USE_RADIO_LOW_ISR                   (1)

/* Link Layer event scheduling (0 --> NO, next event schediling is done at background ; 1 --> YES) */
#define NEXT_EVENT_SCHEDULING_FROM_ISR      (1)

/* Link Layer uses temperature based calibration (0 --> NO ; 1 --> YES) */
#define USE_TEMPERATURE_BASED_RADIO_CALIBRATION  (1)

#define RADIO_INTR_NUM                      RADIO_IRQn     /* 2.4GHz RADIO global interrupt */
#define RADIO_INTR_PRIO_HIGH                (0)            /* 2.4GHz RADIO interrupt priority when radio is Active */
#define RADIO_INTR_PRIO_LOW                 (5)            /* 2.4GHz RADIO interrupt priority when radio is Not Active - Sleep Timer Only */

#if (USE_RADIO_LOW_ISR == 1)
#define RADIO_SW_LOW_INTR_NUM               HASH_IRQn      /* Selected interrupt vector for 2.4GHz RADIO low ISR */
#define RADIO_SW_LOW_INTR_PRIO              (15)           /* 2.4GHz RADIO low ISR priority */
#endif /* USE_RADIO_LOW_ISR */

#define RCC_INTR_PRIO                       (1)           /* HSERDY and PLL1RDY */

/* USER CODE BEGIN Radio_Configuration */

/* USER CODE END Radio_Configuration */

/******************************************************************************
 * HW_RNG configuration
 ******************************************************************************/

/* Number of 32-bit random values stored in internal pool */
#define CFG_HW_RNG_POOL_SIZE                (32)

/* USER CODE BEGIN HW_RNG_Configuration */

/* USER CODE END HW_RNG_Configuration */

/******************************************************************************
 * MEMORY MANAGER
 ******************************************************************************/

#define CFG_MM_POOL_SIZE                          (4000)
#define CFG_PWR_VOS2_SUPPORTED                    (0)   /* VOS2 power configuration not currently supported with radio activity */
#define CFG_AMM_VIRTUAL_MEMORY_NUMBER             (2u)
#define CFG_AMM_VIRTUAL_STACK_BLE                   (1U)
#define CFG_AMM_VIRTUAL_STACK_BLE_BUFFER_SIZE       (400U)
#define CFG_AMM_VIRTUAL_APP_BLE                   (2U)
#define CFG_AMM_VIRTUAL_APP_BLE_BUFFER_SIZE       (200U)
#define CFG_AMM_POOL_SIZE                      DIVC(CFG_MM_POOL_SIZE, sizeof (uint32_t)) \
                                               + (AMM_VIRTUAL_INFO_ELEMENT_SIZE * CFG_AMM_VIRTUAL_MEMORY_NUMBER)

/* USER CODE BEGIN MEMORY_MANAGER_Configuration */

/* USER CODE END MEMORY_MANAGER_Configuration */

/* USER CODE BEGIN Defines */
/**
 * User interaction
 * When CFG_LED_SUPPORTED is set, LEDS are activated if requested
 * When CFG_BUTTON_SUPPORTED is set, the push button are activated if requested
 * When CFG_DBG_SUPPORTED is set, the debugger is activated
 */

#define CFG_LED_SUPPORTED                       (1)
#define CFG_BUTTON_SUPPORTED                    (1)
#define CFG_DBG_SUPPORTED                       (1)

/**
 * If CFG_FULL_LOW_POWER is requested, make sure LED and debugger are disabled
 */
#if (CFG_FULL_LOW_POWER == 1)
  #undef  CFG_LED_SUPPORTED
  #define CFG_LED_SUPPORTED      (0)
  #undef  CFG_DBG_SUPPORTED
  #define CFG_DBG_SUPPORTED      (0)
#endif /* CFG_FULL_LOW_POWER */

/* USER CODE END Defines */

#endif /*APP_CONF_H */
