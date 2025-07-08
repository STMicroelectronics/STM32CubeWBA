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
#include "ble_defs.h"
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
#define CFG_BD_ADDRESS                    (0x000000000000)

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

/**
 * Define IO Authentication
 */
#define CFG_BONDING_MODE                  (1)
#define CFG_USED_FIXED_PIN                (1) /* 0->fixed pin is used ; 1->No fixed pin used*/
#define CFG_FIXED_PIN                     (111111)
#define CFG_ENCRYPTION_KEY_SIZE_MAX       (16)
#define CFG_ENCRYPTION_KEY_SIZE_MIN       (8)

/**
 * Define Input Output capabilities
 */
#define CFG_IO_CAPABILITY                 ( IO_CAP_NO_INPUT_NO_OUTPUT)

/**
 * Define Man In The Middle modes
 */
#define CFG_MITM_PROTECTION               (MITM_PROTECTION_NOT_REQUIRED)

/**
 * Define Secure Connections Support
 */
#define CFG_SC_SUPPORT                    (SC_PAIRING_OPTIONAL)

/**
 * Define Keypress Notification Support
 */
#define CFG_KEYPRESS_NOTIFICATION_SUPPORT (KEYPRESS_NOT_SUPPORTED)

/**
*   Identity root key used to derive IRK and DHK(Legacy)
*/
#define CFG_BLE_IR  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

/**
* Encryption root key used to derive LTK(Legacy) and CSRK
*/
#define CFG_BLE_ER  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

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
                                     BLE_OPTIONS_EXTENDED_ADV | \
                                     BLE_OPTIONS_REDUCED_DB_IN_NVM | \
                                     0 | \
                                     0 | \
                                     0 | \
                                     0)

/**
 * Maximum number of simultaneous connections and advertising that the device will support.
 * This setting should not exceed the number of BLE connection supported by BLE host stack.
 */
#define CFG_BLE_NUM_LINK            (1)

/**
 * Maximum number of Services that can be stored in the GATT database.
 * Note that the GAP and GATT services are automatically added so this parameter should be 2 plus the number of user services
 */
#define CFG_BLE_NUM_GATT_SERVICES   (2)

/**
 * Maximum number of Attributes
 * (i.e. the number of characteristic + the number of characteristic values + the number of descriptors, excluding the services)
 * that can be stored in the GATT database.
 * Note that certain characteristics and relative descriptors are added automatically during device initialization
 * so this parameters should be 9 plus the number of user Attributes
 */
#define CFG_BLE_NUM_GATT_ATTRIBUTES (11)

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
#define CFG_BLE_ATT_VALUE_ARRAY_SIZE  (47)

/**
 * Maximum numbers of bearers that can be created for Enhanced ATT per ACL links
 */
#define CFG_BLE_EATT_BEARER_PER_LINK  (0)

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
#include "hap_app_conf.h"
/* GATT Services Configuration */
#undef CFG_BLE_NUM_GATT_SERVICES
#define CFG_BLE_NUM_GATT_SERVICES               BLE_APP_NUM_GATT_SERVICES

/* GATT Attributes Configuration */
#undef CFG_BLE_NUM_GATT_ATTRIBUTES
#define CFG_BLE_NUM_GATT_ATTRIBUTES             BLE_APP_NUM_GATT_ATTRIBUTES

/* Size of the storage area for Attribute values */
#undef CFG_BLE_ATT_VALUE_ARRAY_SIZE
#define CFG_BLE_ATT_VALUE_ARRAY_SIZE            BLE_APP_ATT_VALUE_ARRAY_SIZE

/* USER CODE END BLE_Stack */

/******************************************************************************
 * Low Power
 *
 *  When CFG_LPM_LEVEL is set to:
 *   - 0 : Low Power Mode is not activated, RUN mode will be used.
 *   - 1 : Low power active, mode selected with CFG_LPM_STDBY_SUPPORTED
 *   - 2 : In addition log and debug are disabled to reach lowest power figures.
 *
 * When CFG_LPM_STDBY_SUPPORTED is set to:
 *   - 2 : Stop mode 2 is used as low power mode (if supported by target)
 *   - 1 : Standby is used as low power mode.
 *   - 0 : Stop mode 1 is used as low power mode.
 *
 ******************************************************************************/
#define CFG_LPM_LEVEL            (1)
#define CFG_LPM_STDBY_SUPPORTED  (0)

/**
 * Defines to use dynamic low power wakeup time profilling.
 * With this option at boot wake up time is profiled and then is used.
 */
#define CFG_LPM_WAKEUP_TIME_PROFILING (1)

/**
 * Defines time to wake up from standby before radio event to meet timings
 * This value will be dynamically updated  when using CFG_LPM_WAKEUP_TIME_PROFILING
 */
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
  CFG_LPM_AUDIO,
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
 * Standby low power mode(CFG_LPM_STDBY_SUPPORTED) above 0 will disable LOG.
 */
#define CFG_LOG_SUPPORTED           (0U)

extern UART_HandleTypeDef           huart1;
#define LOG_UART_HANDLER            huart1

/* Configure Log display settings */
#define CFG_LOG_INSERT_COLOR_INSIDE_THE_TRACE       (0U)
#define CFG_LOG_INSERT_TIME_STAMP_INSIDE_THE_TRACE  (0U)
#define CFG_LOG_INSERT_EOL_INSIDE_THE_TRACE         (0U)

#define CFG_LOG_TRACE_FIFO_SIZE     (4096U)
#define CFG_LOG_TRACE_BUF_SIZE      (256U)

/* USER CODE BEGIN Logs */

/* USER CODE END Logs */

/******************************************************************************
 * Configure Log level for Application
 *
 * APPLI_CONFIG_LOG_LEVEL can be any value of the Log_Verbose_Level_t enum.
 *
 * APPLI_CONFIG_LOG_REGION can either be :
 * - LOG_REGION_ALL_REGIONS to enable all regions
 * or
 * - One or several specific regions (any value except LOG_REGION_ALL_REGIONS)
 *   from the Log_Region_t enum and matching the mask value.
 *
 *   For example, to enable both LOG_REGION_BLE and LOG_REGION_APP,
 *   the value assigned to the define is :
 *   (1U << LOG_REGION_BLE | 1U << LOG_REGION_APP)
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
  /* USER CODE BEGIN CFG_Task_Id_t */
  CFG_TASK_AUDIO_ID,
  CFG_TASK_BSP_JOY_SAMPLE,
  CFG_TASK_BSP_JOY_NONE,
  CFG_TASK_BSP_JOY_UP,
  CFG_TASK_BSP_JOY_DOWN,
  CFG_TASK_BSP_JOY_RIGHT,
  CFG_TASK_BSP_JOY_LEFT,
  CFG_TASK_BSP_JOY_SELECT,
  CFG_TASK_MENU_PRINT_ID,
  CFG_TASK_PLL_READY_ID,
  CFG_TASK_APP_ADV_TIMER_ID,
  CFG_TASK_APP_LINKUP_RETRY_TIMER_ID,
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
 * These are the lists of events id registered to the sequencer
 * Each event id shall be in the range [0:31]
 */
typedef enum
{
  /* USER CODE BEGIN CFG_Event_Id_t */

  /* USER CODE END CFG_Event_Id_t */
  CFG_EVENT_NBR                   /* Shall be LAST in the list */
} CFG_Event_Id_t;

/**< Events defines */
/* USER CODE BEGIN EVENT_ID_Define */

/* USER CODE END EVENT_ID_Define */

/******************************************************************************
 * NVM configuration
 ******************************************************************************/

#define CFG_SNVMA_START_SECTOR_ID     ((FLASH_SIZE / FLASH_PAGE_SIZE) - 2u)

#define CFG_SNVMA_START_ADDRESS       (FLASH_BASE + (FLASH_PAGE_SIZE * (CFG_SNVMA_START_SECTOR_ID)))

/* Number of 64-bit words in NVM flash area */
#define CFG_BLE_NVM_SIZE_MAX          ((2048/8)-4)

/* USER CODE BEGIN NVM_Configuration */

/* USER CODE END NVM_Configuration */

/******************************************************************************
 * Debugger
 *
 *  When CFG_DEBUGGER_LEVEL is set to:
 *   - 0 : No Debugger available, SWD/JTAG pins are disabled.
 *   - 1 : Debugger available in RUN mode only.
 *   - 2 : Debugger available in low power mode.
 *
 ******************************************************************************/
#define CFG_DEBUGGER_LEVEL                  (1)

/******************************************************************************
 * RealTime GPIO debug module configuration
 ******************************************************************************/

#define CFG_RT_DEBUG_GPIO_MODULE            (0)
#define CFG_RT_DEBUG_DTB                    (0)

/******************************************************************************
 * System Clock Manager module configuration
 *
 *  When CFG_SCM_SUPPORTED is set to:
 *   - 0 : System Clock Manager is disabled and user must handle himself
 *         all clock management, taking care of radio requirements.
 *         (radio operation requires HSE 32MHz with Voltage Scaling Range 1)
 *   - 1 : System Clock Manager ensures proper clock settings and switchings
 *         according to radio requirements and user preferences
 *
 ******************************************************************************/
#define CFG_SCM_SUPPORTED                   (1)

/******************************************************************************
 * HW RADIO configuration
 ******************************************************************************/
#define RADIO_INTR_NUM                      RADIO_IRQn     /* 2.4GHz RADIO global interrupt */
#define RADIO_INTR_PRIO_HIGH                (0)            /* 2.4GHz RADIO interrupt priority when radio is Active */
#define RADIO_INTR_PRIO_LOW                 (3)            /* 2.4GHz RADIO interrupt priority when radio is Not Active - Sleep Timer Only */

#define RADIO_SW_LOW_INTR_NUM               HASH_IRQn      /* Selected interrupt vector for 2.4GHz RADIO low ISR */
#define RADIO_SW_LOW_INTR_PRIO              (14)           /* 2.4GHz RADIO low ISR priority */

#define RCC_INTR_PRIO                       (1)           /* HSERDY and PLL1RDY */

/* RF TX power table ID selection:
 *   0 -> RF TX output level from -20 dBm to +10 dBm
 *   1 -> RF TX output level from -20 dBm to +3 dBm
 */
#define CFG_RF_TX_POWER_TABLE_ID            (0)

/* Radio sleep clock LSE accuracy configuration */
#define CFG_RADIO_LSE_SLEEP_TIMER_CUSTOM_SCA_RANGE (0x00)

/* USER CODE BEGIN Radio_Configuration */

/* USER CODE END Radio_Configuration */

/******************************************************************************
 * HW_RNG configuration
 ******************************************************************************/

/* Number of 32-bit random numbers stored in internal pool */
#define CFG_HW_RNG_POOL_SIZE                (32)

/* Threshold of random numbers available before triggering pool refill */
#define CFG_HW_RNG_POOL_THRESHOLD           (16)

/* USER CODE BEGIN HW_RNG_Configuration */

/* USER CODE END HW_RNG_Configuration */

/******************************************************************************
 * MEMORY MANAGER
 ******************************************************************************/

#define CFG_MM_POOL_SIZE                                  (2000U)  /* bytes */
#define CFG_AMM_VIRTUAL_MEMORY_NUMBER                     (2U)
#define CFG_AMM_VIRTUAL_STACK_BLE                         (1U)
#define CFG_AMM_VIRTUAL_STACK_BLE_BUFFER_SIZE     (200U)  /* words (32 bits) */
#define CFG_AMM_VIRTUAL_APP_BLE                           (2U)
#define CFG_AMM_VIRTUAL_APP_BLE_BUFFER_SIZE     (200U)  /* words (32 bits) */
#define CFG_AMM_POOL_SIZE                                 ( DIVC(CFG_MM_POOL_SIZE, sizeof (uint32_t)) \
                                                          + (AMM_VIRTUAL_INFO_ELEMENT_SIZE * CFG_AMM_VIRTUAL_MEMORY_NUMBER) )

/* USER CODE BEGIN MEMORY_MANAGER_Configuration */

/* USER CODE END MEMORY_MANAGER_Configuration */

/* USER CODE BEGIN Defines */
#define CFG_BSP_ON_SEQUENCER                    (1)
#define CFG_BSP_ON_DISCOVERY                    (1)

/**
 * User interaction
 * When CFG_LED_SUPPORTED is set, LEDS are activated if requested
 * When CFG_JOYSTICK_SUPPORTED is set, the joystick is activated if requested
 */

#define CFG_LED_SUPPORTED                       (0) /* may be not compatible with LCD, check PCB revision */
#define CFG_JOYSTICK_SUPPORTED                  (1)
#define CFG_LCD_SUPPORTED                       (1)

/**
 *  Joystick Use configuration
 */
#define CFG_JOYSTICK_USE_TYPE                   (JOYSTICK_USE_AS_CHANGE)
#define CFG_JOYSTICK_MODE                       (JOY_MODE_POLLING)

/**
 * Overwrite some configuration imposed by Low Power level selected.
 */
#if (CFG_LPM_LEVEL > 1)
  #if CFG_LED_SUPPORTED
    #undef  CFG_LED_SUPPORTED
    #define CFG_LED_SUPPORTED      (0)
  #endif /* CFG_LED_SUPPORTED */
  #if CFG_LCD_SUPPORTED
    #undef  CFG_LCD_SUPPORTED
    #define CFG_LCD_SUPPORTED      (0)
  #endif /* CFG_LCD_SUPPORTED */
#endif /* CFG_LPM_LEVEL */


/******************************************************************************
 * CODEC MANAGER
 ******************************************************************************/
/* Maximum band used by the codec, used for RAM allocation */
#define CODEC_MAX_BAND                          CODEC_SSWB

#define CODEC_LC3_NUM_SESSION                   (1u)

#define CODEC_LC3_NUM_ENCODER_CHANNEL           (1u)
#define CODEC_LC3_NUM_DECODER_CHANNEL           (2u)

#define CODEC_MNGR_INTR_NUM                     COMP_IRQn      /* Vector used for codec execution */
#define CODEC_MNGR_INTR_PRIO                    (15)

/* Maximum duration in us of interruption of the codec manager over a frame duration */
#define CODEC_PROC_MARGIN_US                    (2000u)

/* Extra audio latency due to maximum radio preparation time */
#if defined(__GNUC__) && defined(DEBUG)
#define CODEC_RF_SETUP_US                       (1250u)
#else
#define CODEC_RF_SETUP_US                       (1100u)
#endif /* defined(__GNUC__) && defined(DEBUG) */
/******************************************************************************
 * Power Table
 ******************************************************************************/
#define CFG_OUTPUT_POWER_TABLE_DEFAULT  (0)
#define CFG_OUTPUT_POWER_TABLE_PS0_0x20 (1)
#define CFG_OUTPUT_POWER_TABLE_PS2_0x10 (2)


#define CFG_OUTPUT_POWER_TABLE_VERSION  (CFG_OUTPUT_POWER_TABLE_PS0_0x20)

/******************************************************************************
 * TEST VALIDATION
 ******************************************************************************/
/* Enable or disable configuration of project to run validation test : only used for project validation test */
#define CFG_TEST_VALIDATION                     (0u)
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

/* USER CODE BEGIN Defines_2 */
#if (CFG_TEST_VALIDATION == 1)
  #undef  CFG_LOG_SUPPORTED
  #define CFG_LOG_SUPPORTED         (1)
  #undef  APPLI_CONFIG_LOG_LEVEL
  #define APPLI_CONFIG_LOG_LEVEL    LOG_VERBOSE_INFO
  #undef  APPLI_CONFIG_LOG_REGION
  #define APPLI_CONFIG_LOG_REGION   (0x04)
  #define JOY1_TEST_ADC_CHANNEL                 ADC_CHANNEL_3
  #define JOY1_TEST_CHANNEL_GPIO_PORT           GPIOA
  #define JOY1_TEST_CHANNEL_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()
  #define JOY1_TEST_CHANNEL_GPIO_PIN            GPIO_PIN_6
#endif /* CFG_TEST_VALIDATION */

/* Set a medium gain for the headset microphone */
#define MIC_GAIN_CONFIG             (1)

/* When the PLL is switched on for audio, Link layer execution is faster and its timings margin can be reduced
   These values have been qualified for LE audio use case and should not be changed by the user */
#define ISO_PLL_DRIFT_TIME                      (7)
#define ISO_PLL_DRIFT_TIME_EXTRA_GCC_DEBUG      (2)

#define ISO_PLL_EXEC_TIME                       (22)
#define ISO_PLL_EXEC_TIME_EXTRA_GCC_DEBUG       (2)

#define ISO_PLL_SCHDL_TIME                      (20)

/* USER CODE END Defines_2 */

#endif /*APP_CONF_H */
