/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_conf.h
  * @author  MCD Application Team
  * @brief   Application configuration file for STM32WPAN Middleware.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_CONF_H
#define APP_CONF_H

/* Includes ------------------------------------------------------------------*/
#include "ble_defs.h"
#include "utilities_conf.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/******************************************************************************
 * Application Config
 ******************************************************************************/
/**< generic parameters ******************************************************/
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
                                     BLE_OPTIONS_REDUCED_DB_IN_NVM | \
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
#define CFG_BLE_NUM_GATT_SERVICES   (3)

/**
 * Maximum number of Attributes
 * (i.e. the number of characteristic + the number of characteristic values + the number of descriptors, excluding the services)
 * that can be stored in the GATT database.
 * Note that certain characteristics and relative descriptors are added automatically during device initialization
 * so this parameters should be 9 plus the number of user Attributes
 */
#define CFG_BLE_NUM_GATT_ATTRIBUTES (20)

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
#define CFG_BLE_ATT_VALUE_ARRAY_SIZE  (1344)

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
#define CFG_LPM_LEVEL            (0)
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

  /* USER CODE END CFG_LPM_Id_t */
} CFG_LPM_Id_t;

/* USER CODE BEGIN Low_Power 1 */

/* USER CODE END Low_Power 1 */

/******************************************************************************
 * RTC
 ******************************************************************************/

/* USER CODE BEGIN RTC */

/* USER CODE END RTC */

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
 * Debugger
 *
 *  When CFG_DEBUGGER_LEVEL is set to:
 *   - 0 : No Debugger available, SWD/JTAG pins are disabled.
 *   - 1 : Debugger available in RUN mode only.
 *   - 2 : Debugger available in low power mode.
 *
 ******************************************************************************/
#define CFG_DEBUGGER_LEVEL                  (0)

/******************************************************************************
 * RealTime GPIO debug module configuration
 ******************************************************************************/

#define CFG_RT_DEBUG_GPIO_MODULE            (0)
#define CFG_RT_DEBUG_DTB                    (0)

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

/* USER CODE BEGIN Defines */

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

/* USER CODE END Defines_2 */

#endif /*APP_CONF_H */
