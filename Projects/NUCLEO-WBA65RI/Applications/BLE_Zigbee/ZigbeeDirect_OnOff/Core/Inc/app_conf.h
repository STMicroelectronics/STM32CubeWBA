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
#include "utilities_conf.h"

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
 * Definition of public BD Address,
 * when CFG_BD_ADDRESS = 0x000000000000 the BD address is generated based on Unique Device Number.
 */
#define CFG_BD_ADDRESS                    (0x0008E12A1234)

/**
 * Define BD_ADDR type: define proper address. Can only be GAP_PUBLIC_ADDR (0x00) or GAP_STATIC_RANDOM_ADDR (0x01)
 */
#define CFG_BD_ADDRESS_DEVICE             (GAP_PUBLIC_ADDR)

/**
 * Define privacy: PRIVACY_DISABLED or PRIVACY_ENABLED
 */
#define CFG_PRIVACY                       (PRIVACY_DISABLED)

/**
 * Define BLE Address Type
 * Bluetooth address types defined in ble_legacy.h
 * if CFG_PRIVACY equals PRIVACY_DISABLED, CFG_BLE_ADDRESS_TYPE has 2 allowed values: GAP_PUBLIC_ADDR or GAP_STATIC_RANDOM_ADDR
 * if CFG_PRIVACY equals PRIVACY_ENABLED, CFG_BLE_ADDRESS_TYPE has 2 allowed values: GAP_RESOLVABLE_PRIVATE_ADDR or GAP_NON_RESOLVABLE_PRIVATE_ADDR
 */
#define CFG_BD_ADDRESS_TYPE               (GAP_PUBLIC_ADDR)

#define ADV_INTERVAL_MIN                  (250) /* 156 mS , NOTE was 128 (80 mS) */
#define ADV_INTERVAL_MAX                  (1920) /* 1.2 Sec */
#define ADV_LP_INTERVAL_MIN               (0x0640)
#define ADV_LP_INTERVAL_MAX               (0x0FA0)
#define ADV_TYPE                          ADV_IND
#define ADV_FILTER                        NO_WHITE_LIST_USE

 /* Zigbee Direct Spec recommends a connection interval between 7.5 mS and 100 mS.
 * A shorter connection interval means the BLE side uses more radio resources,
 * where a longer connection interval means BLE messages take longer to be sent.
 * Zigbee tunneling should work fine with a connection interval on the order of
 * 100 mS. */
/* connIntervalmax = Conn_Interval_Max x 1.25ms */
#define ADV_CONN_INTVL_MIN                  16 /* 20 mS */
#define ADV_CONN_INTVL_MAX                  80 /* 100 mS - this is the ZD recommended max */


/**
 * Define IO Authentication
 */
#define CFG_BONDING_MODE                  (1)
#define CFG_USED_FIXED_PIN                (0) /* 0->fixed pin is used ; 1->No fixed pin used*/
#define CFG_FIXED_PIN                     (111111)
#define CFG_ENCRYPTION_KEY_SIZE_MAX       (16)
#define CFG_ENCRYPTION_KEY_SIZE_MIN       (8)

/**
 * Define Input Output capabilities
 */
#define CFG_IO_CAPABILITY                 (IO_CAP_DISPLAY_YES_NO)

/**
 * Define Man In The Middle modes
 */
#define CFG_MITM_PROTECTION               (MITM_PROTECTION_REQUIRED)

/**
 * Define Secure Connections Support
 */
#define CFG_SECURE_NOT_SUPPORTED          (0x00)
#define CFG_SECURE_OPTIONAL               (0x01)
#define CFG_SC_SUPPORT                    (CFG_SECURE_OPTIONAL)

/**
 * Define Keypress Notification Support
 */
#define CFG_KEYPRESS_NOTIFICATION_SUPPORT (KEYPRESS_NOT_SUPPORTED)

/**
*   Identity root key used to derive IRK and DHK(Legacy)
*/
#define CFG_BLE_IR      {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0}

/**
* Encryption root key used to derive LTK(Legacy) and CSRK
*/
#define CFG_BLE_ER      {0xFE, 0xDC, 0xBA, 0x09, 0x87, 0x65, 0x43, 0x21, 0xFE, 0xDC, 0xBA, 0x09, 0x87, 0x65, 0x43, 0x21}

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
                                     0 | \
                                     0)

/**
 * Maximum number of simultaneous connections and advertising that the device will support.
 * This setting should not exceed the number of BLE connection supported by BLE host stack.
 */
#define CFG_BLE_NUM_LINK            (2)

/**
 * Maximum number of Services that can be stored in the GATT database.
 * Note that the GAP and GATT services are automatically added so this parameter should be 2 plus the number of user services
 */
#define CFG_BLE_NUM_GATT_SERVICES   (12)

/**
 * Maximum number of Attributes
 * (i.e. the number of characteristic + the number of characteristic values + the number of descriptors, excluding the services)
 * that can be stored in the GATT database.
 * Note that certain characteristics and relative descriptors are added automatically during device initialization
 * so this parameters should be 9 plus the number of user Attributes
 */
#define CFG_BLE_NUM_GATT_ATTRIBUTES (96)

/**
 * Maximum supported ATT_MTU size
 * This setting should be aligned with ATT_MTU value configured in the ble host
 */
#define CFG_BLE_ATT_MTU_MAX         (247)

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
#define CFG_BLE_ATT_VALUE_ARRAY_SIZE    (2 * 1024)

/**
 * depth of the PREPARE WRITE queue when PREPARE WRITE REQUEST
 */
#define CFG_BLE_ATTR_PREPARE_WRITE_VALUE_SIZE       (30)

#define CFG_BLE_MBLOCK_COUNT_MARGIN                 (0x15)

#define PREP_WRITE_LIST_SIZE                        (BLE_DEFAULT_PREP_WRITE_LIST_SIZE)

/**
 * Number of allocated memory blocks used to transmit and receive data packets
 */
#define CFG_BLE_MBLOCK_COUNT          (BLE_MBLOCKS_CALC(PREP_WRITE_LIST_SIZE, \
                                       CFG_BLE_ATT_MTU_MAX, CFG_BLE_NUM_LINK) \
                                       + CFG_BLE_MBLOCK_COUNT_MARGIN)

/**
 * Appearance of device set into BLE GAP
 */
#define CFG_GAP_APPEARANCE            (GAP_APPEARANCE_UNKNOWN)

/**
 * Connection Oriented Channel parameters
 */
#define CFG_BLE_COC_NBR_MAX           (64)
#define CFG_BLE_COC_MPS_MAX           (248)
#define CFG_BLE_COC_INITIATOR_NBR_MAX (32)

/**
 * PHY preferences
 */
#define CFG_PHY_PREF                  (0)
#define CFG_PHY_PREF_TX               (HCI_TX_PHYS_LE_2M_PREF)
#define CFG_PHY_PREF_RX               (HCI_RX_PHYS_LE_2M_PREF)

/* USER CODE BEGIN BLE_Stack */

/* USER CODE END BLE_Stack */

/******************************************************************************
 * Low Power
 *
 *  When CFG_LPM_LEVEL is set to:
 *   - 0 : Low Power Mode is not activated, RUN mode will be used.
 *   - 1 : Low power active, the one selected with CFG_LPM_STDBY_SUPPORTED
 *   - 2 : In addition, force to disable modules to reach lowest power figures.
 *
 * When CFG_LPM_STDBY_SUPPORTED is set to:
 *   - 1 : Standby is used as low power mode.
 *   - 0 : Standby is not used, so stop mode 1 is used as low power mode.
 *
 ******************************************************************************/
#define CFG_LPM_LEVEL            (0)
#define CFG_LPM_STDBY_SUPPORTED  (0)

/* Defines time to wake up from standby before radio event to meet timings */
#define CFG_LPM_STDBY_WAKEUP_TIME (1500)

/* USER CODE BEGIN Low_Power 0 */

/* USER CODE END Low_Power 0 */

/**
 * Supported requester to the MCU Low Power Manager - can be increased up  to 32
 * It list a bit mapping of all user of the Low Power Manager
 */
typedef enum
{
  CFG_LPM_APP,
  CFG_LPM_LOG,
  CFG_LPM_LL_DEEPSLEEP,
  CFG_LPM_LL_HW_RCO_CLBR,
  /* USER CODE BEGIN CFG_LPM_Id_t */

  /* USER CODE END CFG_LPM_Id_t */
} CFG_LPM_Id_t;

/* USER CODE BEGIN Low_Power 1 */

/* USER CODE END Low_Power 1 */

/******************************************************************************
 * RTC
 ******************************************************************************/

/* USER CODE BEGIN RTC */

/* USER CODE END RTC */

/*****************************************************************************
 * Logs
 *
 * Applications must call LOG_INFO_APP for logs.
 * By default, CFG_LOG_INSERT_TIME_STAMP_INSIDE_THE_TRACE is set to 0.
 * As a result, there is no time stamp insertion inside the logs.
 *
 * For advanced log use cases, see the log_module.h file.
 * This file is customizable, you can create new verbose levels and log regions.
 *****************************************************************************/
/**
 * Enable or disable LOG over UART in the application.
 * Low power level(CFG_LPM_LEVEL) above 1 will disable LOG.
 * Standby low power mode(CFG_LPM_STDBY_SUPPORTED) will disable LOG.
 */
#define CFG_LOG_SUPPORTED           (1U)

/* Usart used by LOG */
extern UART_HandleTypeDef           huart1;
#define LOG_UART_HANDLER            huart1
extern UART_HandleTypeDef           huart1;
#define LOG_UART_HANDLER            huart1
extern UART_HandleTypeDef           huart1;
#define LOG_UART_HANDLER            huart1

/* Configure Log display settings */
#define CFG_LOG_INSERT_COLOR_INSIDE_THE_TRACE       (1U)
#define CFG_LOG_INSERT_TIME_STAMP_INSIDE_THE_TRACE  (0U)
#define CFG_LOG_INSERT_EOL_INSIDE_THE_TRACE         (1U)

#define CFG_LOG_TRACE_FIFO_SIZE     (4096U)
#define CFG_LOG_TRACE_BUF_SIZE      (256U)

/* macro ensuring retrocompatibility with old applications */
#define APP_DBG                     LOG_INFO_APP
#define APP_DBG_MSG                 LOG_INFO_APP

/* Specific defines for Zigbee traces levels */
#define ZB_LOG_MASK_LEVEL_0         0x00000000U
#define ZB_LOG_MASK_LEVEL_1         ( ZB_LOG_MASK_FATAL )
#define ZB_LOG_MASK_LEVEL_2         ( ZB_LOG_MASK_LEVEL_1 | ZB_LOG_MASK_ERROR )
#define ZB_LOG_MASK_LEVEL_3         ( ZB_LOG_MASK_LEVEL_2 | ZB_LOG_MASK_INFO )
#define ZB_LOG_MASK_LEVEL_4         ( ZB_LOG_MASK_LEVEL_3 | ZB_LOG_MASK_DEBUG )
#define ZB_LOG_MASK_LEVEL_5         ( ZB_LOG_MASK_LEVEL_4 | ZB_LOG_MASK_ZCL )
#define ZB_LOG_MASK_LEVEL_ALL       0xFFFFFFFFU

/* Indicate Trace Level for Zigbee Stack (Fatal/Error) */
#define ZIGBEE_CONFIG_LOG_LEVEL     ZB_LOG_MASK_LEVEL_2

/* USER CODE BEGIN Logs */

/* USER CODE END Logs */

/******************************************************************************
 * Configure Log level for Application
 ******************************************************************************/
#define APPLI_CONFIG_LOG_LEVEL      LOG_VERBOSE_INFO
#define APPLI_CONFIG_LOG_REGION     (LOG_REGION_ALL_REGIONS)
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
  CFG_TASK_HW_RNG,
  CFG_TASK_LINK_LAYER,
  CFG_TASK_HCI_ASYNCH_EVT_ID,
  CFG_TASK_BLE_HOST,
  CFG_TASK_AMM,
  CFG_TASK_BPKA,
  CFG_TASK_BLE_TIMER_BCKGND,
  CFG_TASK_FLASH_MANAGER,
  CFG_TASK_MAC_LAYER,
  CFG_TASK_ZIGBEE_LAYER,
  CFG_TASK_AMM_BCKGND,
  CFG_TASK_ZIGBEE_ZDD,
  CFG_TASK_ZIGBEE_NETWORK_FORM,   /* Tasks linked to Zigbee Start. */
  CFG_TASK_ZIGBEE_APP_START,
  CFG_TASK_ZIGBEE_APP1,           /* Tasks linked to Zigbee Application. */
  CFG_TASK_ZIGBEE_APP2,
  CFG_TASK_ZIGBEE_APP3,
  CFG_TASK_ZIGBEE_APP4,
  /* USER CODE BEGIN CFG_Task_Id_t */
  CFG_TASK_BUTTON_B1,
  CFG_TASK_BUTTON_B2,
  CFG_TASK_BUTTON_B3,

  CFG_TASK_MEAS_REQ_ID,
  CFG_TASK_ADV_LP_REQ_ID,
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

/* Sequencer configuration */
#define UTIL_SEQ_CONF_PRIO_NBR              CFG_SEQ_PRIO_NBR

/**
 * This is a bit mapping over 32bits listing all events id supported in the application
 */
typedef enum
{
  CFG_IDLEEVT_PROC_GAP_COMPLETE,
  /* USER CODE BEGIN CFG_IdleEvt_Id_t */

  /* USER CODE END CFG_IdleEvt_Id_t */
} CFG_IdleEvt_Id_t;

/* Sequencer defines */
#define TASK_HW_RNG                         ( 1u << CFG_TASK_HW_RNG )
#define TASK_LINK_LAYER                     ( 1u << CFG_TASK_LINK_LAYER )
#define TASK_FLASH_MNGR                     ( 1u << CFG_TASK_FLASH_MANAGER )
#define TASK_MAC_LAYER                      ( 1u << CFG_TASK_MAC_LAYER )
#define TASK_ZIGBEE_LAYER                   ( 1u << CFG_TASK_ZIGBEE_LAYER )
#define TASK_ZIGBEE_NETWORK_FORM            ( 1u << CFG_TASK_ZIGBEE_NETWORK_FORM )
#define TASK_ZIGBEE_APP_START               ( 1u << CFG_TASK_ZIGBEE_APP_START )
#define TASK_ZIGBEE_APP1                    ( 1u << CFG_TASK_ZIGBEE_APP1 )
#define TASK_ZIGBEE_APP2                    ( 1u << CFG_TASK_ZIGBEE_APP2 )
#define TASK_ZIGBEE_APP3                    ( 1u << CFG_TASK_ZIGBEE_APP3 )
#define TASK_ZIGBEE_APP4                    ( 1u << CFG_TASK_ZIGBEE_APP3 )
/* USER CODE BEGIN TASK_ID_Define */
#define TASK_BUTTON_B1                      ( 1u << CFG_TASK_BUTTON_B1 )
#define TASK_BUTTON_B2                      ( 1u << CFG_TASK_BUTTON_B2 )
#define TASK_BUTTON_B3                      ( 1u << CFG_TASK_BUTTON_B3 )

#define TASK_MEAS_REQ_ID                    ( 1u << CFG_TASK_MEAS_REQ_ID )
#define TASK_ADV_LP_REQ_ID                  ( 1u << CFG_TASK_ADV_LP_REQ_ID )
/* USER CODE END TASK_ID_Define */

/**
 * These are the lists of events id registered to the sequencer
 * Each event id shall be in the range [0:31]
 */
typedef enum
{
  CFG_EVENT_LINK_LAYER,
  CFG_EVENT_MAC_LAYER,
  CFG_EVENT_ZIGBEE_LAYER,
  CFG_EVENT_ZIGBEE_STARTUP_ENDED,
  CFG_EVENT_ZIGBEE_APP1,           /* Events linked to Zigbee Application. */
  CFG_EVENT_ZIGBEE_APP2,
  CFG_EVENT_ZIGBEE_APP3,
  CFG_EVENT_ZIGBEE_APP4,
  /* USER CODE BEGIN CFG_Event_Id_t */

  /* USER CODE END CFG_Event_Id_t */
  CFG_EVENT_NBR                   /* Shall be LAST in the list */
} CFG_Event_Id_t;

/**< Events defines */
#define EVENT_LINK_LAYER                ( 1U << CFG_EVENT_LINK_LAYER )
#define EVENT_MAC_LAYER                 ( 1U << CFG_EVENT_MAC_LAYER )
#define EVENT_ZIGBEE_LAYER              ( 1U << CFG_EVENT_ZIGBEE_LAYER )
#define EVENT_ZIGBEE_STARTUP_ENDED      ( 1U << CFG_EVENT_ZIGBEE_STARTUP_ENDED )
#define EVENT_ZIGBEE_APP1               ( 1U << CFG_EVENT_ZIGBEE_APP1 )
#define EVENT_ZIGBEE_APP2               ( 1U << CFG_EVENT_ZIGBEE_APP2 )
#define EVENT_ZIGBEE_APP3               ( 1U << CFG_EVENT_ZIGBEE_APP3 )
#define EVENT_ZIGBEE_APP4               ( 1U << CFG_EVENT_ZIGBEE_APP4 )
/* USER CODE BEGIN EVENT_ID_Define */

/* USER CODE END EVENT_ID_Define */

/******************************************************************************
 * NVM configuration
 ******************************************************************************/

#define CFG_SNVMA_START_SECTOR_ID     ((FLASH_SIZE / FLASH_PAGE_SIZE) - 2u)

#define CFG_SNVMA_START_ADDRESS       (FLASH_BASE + (FLASH_PAGE_SIZE * (CFG_SNVMA_START_SECTOR_ID)) - FLASH_PAGE_SIZE)

  /**
   * This is the max size of data the THREAD Stack needs to write in NVM
   * This is different to the size allocated in the EEPROM emulator
   * The THREAD Stack shall write all data at an address in the range of [0 : (y-1)]
   * The size is a number of 32bits values
   */
#define CFG_NVMA_THREAD_NVM_SIZE                    ( 0u )

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
 * Debugger
 *
 *  When CFG_DEBUGGER_LEVEL is set to:
 *   - 0 : No Debugger available, SWD/JTAG pins are disabled.
 *   - 1 : Debugger available in RUN mode only.
 *   - 2 : Debugger available in low power mode.
 *
 ******************************************************************************/
#define CFG_DEBUGGER_LEVEL                  (2)

/******************************************************************************
 * RealTime GPIO debug module configuration
 ******************************************************************************/

#define CFG_RT_DEBUG_GPIO_MODULE            (0)
#define CFG_RT_DEBUG_DTB                    (0)

/******************************************************************************
 * System Clock Manager module configuration
 ******************************************************************************/

#define CFG_SCM_SUPPORTED                   (1)

/******************************************************************************
 * HW RADIO configuration
 ******************************************************************************/
#define RADIO_INTR_NUM                      RADIO_IRQn     /* 2.4GHz RADIO global interrupt */
#define RADIO_INTR_PRIO_HIGH                (0)            /* 2.4GHz RADIO interrupt priority when radio is Active */
#define RADIO_INTR_PRIO_LOW                 (5)            /* 2.4GHz RADIO interrupt priority when radio is Not Active - Sleep Timer Only */

#define RADIO_SW_LOW_INTR_NUM               HASH_IRQn      /* Selected interrupt vector for 2.4GHz RADIO low ISR */
#define RADIO_SW_LOW_INTR_PRIO              (15)           /* 2.4GHz RADIO low ISR priority */

#define RCC_INTR_PRIO                       (1)           /* HSERDY and PLL1RDY */

/* RF TX power table ID selection:
 *   0 -> RF TX output level from -20 dBm to +10 dBm
 *   1 -> RF TX output level from -20 dBm to +3 dBm
 */
#define CFG_RF_TX_POWER_TABLE_ID            (1)

/* Custom LSE sleep clock accuracy to use if both conditions are met:
 * - LSE is selected as Link Layer sleep clock source
 * - the LSE used is different from the default one.
 */
#define CFG_RADIO_LSE_SLEEP_TIMER_CUSTOM_SCA_RANGE (0)

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

#define CFG_MM_POOL_SIZE                                  (60000U)  /* bytes */
#define CFG_AMM_VIRTUAL_MEMORY_NUMBER                     (4U)
#define CFG_AMM_VIRTUAL_STACK_BLE                         (1U)
#define CFG_AMM_VIRTUAL_STACK_BLE_BUFFER_SIZE             (400U) /* words (32 bits) */
#define CFG_AMM_VIRTUAL_APP_BLE                           (2U)
#define CFG_AMM_VIRTUAL_APP_BLE_BUFFER_SIZE     (200U)  /* words (32 bits) */
#define CFG_AMM_VIRTUAL_STACK_ZIGBEE_INIT                 (3U)
#define CFG_AMM_VIRTUAL_STACK_ZIGBEE_INIT_BUFFER_SIZE     (10500U)  /* words (32 bits) */
#define CFG_AMM_VIRTUAL_STACK_ZIGBEE_HEAP                 (4U)
#define CFG_AMM_VIRTUAL_STACK_ZIGBEE_HEAP_BUFFER_SIZE     (3000U)  /* words (32 bits) */
#define CFG_AMM_POOL_SIZE                                 ( DIVC(CFG_MM_POOL_SIZE, sizeof (uint32_t)) \
                                                          + (AMM_VIRTUAL_INFO_ELEMENT_SIZE * CFG_AMM_VIRTUAL_MEMORY_NUMBER) )

/* USER CODE BEGIN MEMORY_MANAGER_Configuration */

/* USER CODE END MEMORY_MANAGER_Configuration */

/* USER CODE BEGIN Defines */
#define CFG_BSP_ON_SEQUENCER                    (1)
#define CFG_BSP_ON_NUCLEO                       (1)

/**
 * User interaction
 * When CFG_LED_SUPPORTED is set, LEDS are activated if requested
 * When CFG_BUTTON_SUPPORTED is set, the push button are activated if requested
 */
#define CFG_LED_SUPPORTED                       (1)
#define CFG_BUTTON_SUPPORTED                    (1)

/**
 * Overwrite some configuration imposed by Low Power level selected.
 */
#if (CFG_LPM_LEVEL > 1)
  #if CFG_LED_SUPPORTED
    #undef  CFG_LED_SUPPORTED
    #define CFG_LED_SUPPORTED      (0)
  #endif /* CFG_LED_SUPPORTED */
#endif /* CFG_LPM_LEVEL */

#if ( CFG_LED_SUPPORTED == 1 )
#define APP_LED_ON( LED )           BSP_LED_On( LED )
#define APP_LED_OFF( LED )          BSP_LED_Off( LED )
#define APP_LED_TOGGLE( LED )       BSP_LED_Toggle( LED )
#else /* ( CFG_LED_SUPPORTED == 1 ) */
#define APP_LED_ON( LED )
#define APP_LED_OFF( LED )
#define APP_LED_TOGGLE( LED )
#endif  /* ( CFG_LED_SUPPORTED == 1 ) */

/* USER CODE END Defines */

/**
 * Overwrite some configuration imposed by Low Power level selected.
 */
#if (CFG_LPM_LEVEL > 1)
  #if CFG_LOG_SUPPORTED
    #undef  CFG_LOG_SUPPORTED
    #define CFG_LOG_SUPPORTED       (0)
  #endif /* CFG_LOG_SUPPORTED */
  #if CFG_DEBUGGER_LEVEL
    #undef  CFG_DEBUGGER_LEVEL
    #define CFG_DEBUGGER_LEVEL      (0)
  #endif /* CFG_DEBUGGER_LEVEL */
#endif /* CFG_LPM_LEVEL */

#if (CFG_LPM_STDBY_SUPPORTED == 1)
  #if CFG_LOG_SUPPORTED
    #undef  CFG_LOG_SUPPORTED
    #define CFG_LOG_SUPPORTED       (0)
  #endif /* CFG_LOG_SUPPORTED */
#endif /* CFG_LPM_STDBY_SUPPORTED */

/* USER CODE BEGIN Defines_2 */
/**
 * Switch to enable/disable BLE radio activity shown on green LED
 * 0 : disable / 1 : enable
 */
#define BLE_RADIO_ACTIVITY_ON_LED_SUPPORT               (1)

#if (CFG_LED_SUPPORTED == 0)
#undef BLE_RADIO_ACTIVITY_ON_LED_SUPPORT
#define BLE_RADIO_ACTIVITY_ON_LED_SUPPORT               (0)
#endif 
/* USER CODE END Defines_2 */

#endif /*APP_CONF_H */
