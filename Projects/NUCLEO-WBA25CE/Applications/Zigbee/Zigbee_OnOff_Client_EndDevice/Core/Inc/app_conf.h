/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_conf.h
  * @author  MCD Application Team
  * @brief   Application configuration file for STM32WPAN Middleware.
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
/* USER CODE BEGIN Generic_Parameters */

/* USER CODE END Generic_Parameters */

/**< specific parameters */
/*****************************************************/

/* USER CODE BEGIN Specific_Parameters */

/* USER CODE END Specific_Parameters */

/******************************************************************************
 * Low Power
 *
 *  When CFG_LPM_LEVEL is set to:
 *   - 0 : Low Power Mode is not activated, RUN mode will be used.
 *   - 1 : Low power active, mode(s) selected with CFG_LPM_mode_SUPPORTED
 *   - 2 : In addition log and debug are disabled to reach lowest power figures.
 ******************************************************************************/
#define CFG_LPM_LEVEL               (0U)

#define CFG_LPM_STOP1_SUPPORTED     (0U)
#define CFG_LPM_STANDBY_SUPPORTED   (0U)

/**
 * Defines to use dynamic low power wakeup time profilling.
 * With this option at boot wake up time is profiled and then is used.
 */
#define CFG_LPM_WAKEUP_TIME_PROFILING (1U)

/**
 * Defines time to wake up from standby before radio event to meet timings
 * This value will be dynamically updated when using CFG_LPM_WAKEUP_TIME_PROFILING
 */
#define CFG_LPM_STDBY_WAKEUP_TIME (1500U)

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
 * Enabled low power modes above STOP1 (STOP2 or STANDBY) will disable LOG.
 */
#define CFG_LOG_SUPPORTED           (1U)

/* Usart used by LOG */
extern UART_HandleTypeDef           huart1;
#define LOG_UART_HANDLER            huart1

/* Configure Log display settings */
#define CFG_LOG_INSERT_COLOR_INSIDE_THE_TRACE       (1U)
#define CFG_LOG_INSERT_TIME_STAMP_INSIDE_THE_TRACE  (0U)
#define CFG_LOG_INSERT_EOL_INSIDE_THE_TRACE         (1U)

#define CFG_LOG_TRACE_FIFO_SIZE     (4096U)
#define CFG_LOG_TRACE_BUF_SIZE      (256U)

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
#define APPLI_CONFIG_LOG_LEVEL      LOG_VERBOSE_WARNING
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
  CFG_TASK_MAC_LAYER,
  CFG_TASK_ZIGBEE_LAYER,
  CFG_TASK_ZIGBEE_NETWORK_FORM,   /* Tasks linked to Zigbee Start. */
  CFG_TASK_ZIGBEE_APP_START,
  CFG_TASK_ZIGBEE_PERSISTENCE,
  CFG_TASK_ZIGBEE_OTA_REQUEST_UPGRADE,
  CFG_TASK_ZIGBEE_OTA_START_DOWNLOAD,
  CFG_TASK_ZIGBEE_APP1,           /* Tasks linked to Zigbee Application. */
  CFG_TASK_ZIGBEE_APP2,
  CFG_TASK_ZIGBEE_APP3,
  CFG_TASK_ZIGBEE_APP4,
  /* USER CODE BEGIN CFG_Task_Id_t */
  CFG_TASK_BSP_BUTTON_B1,         /* Task linked to push-button. */
  CFG_TASK_BSP_BUTTON_B2,
  CFG_TASK_BSP_BUTTON_B3,

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

/* Sequencer defines */
#define TASK_HW_RNG                         ( 1u << CFG_TASK_HW_RNG )
#define TASK_LINK_LAYER                     ( 1u << CFG_TASK_LINK_LAYER )
#define TASK_MAC_LAYER                      ( 1u << CFG_TASK_MAC_LAYER )
#define TASK_ZIGBEE_LAYER                   ( 1u << CFG_TASK_ZIGBEE_LAYER )
#define TASK_ZIGBEE_NETWORK_FORM            ( 1u << CFG_TASK_ZIGBEE_NETWORK_FORM )
#define TASK_ZIGBEE_APP_START               ( 1u << CFG_TASK_ZIGBEE_APP_START )
#define TASK_ZIGBEE_PERSISTENCE             ( 1u << CFG_TASK_ZIGBEE_PERSISTENCE )
#define TASK_ZIGBEE_OTA_REQUEST_UPGRADE     ( 1u << CFG_TASK_ZIGBEE_OTA_REQUEST_UPGRADE )
#define TASK_ZIGBEE_OTA_START_DOWNLOAD      ( 1u << CFG_TASK_ZIGBEE_OTA_START_DOWNLOAD )
#define TASK_ZIGBEE_APP1                    ( 1u << CFG_TASK_ZIGBEE_APP1 )
#define TASK_ZIGBEE_APP2                    ( 1u << CFG_TASK_ZIGBEE_APP2 )
#define TASK_ZIGBEE_APP3                    ( 1u << CFG_TASK_ZIGBEE_APP3 )
#define TASK_ZIGBEE_APP4                    ( 1u << CFG_TASK_ZIGBEE_APP3 )
/* USER CODE BEGIN TASK_ID_Define */
#define TASK_BSP_BUTTON_B1                  ( 1u << CFG_TASK_BSP_BUTTON_B1 )
#define TASK_BSP_BUTTON_B2                  ( 1u << CFG_TASK_BSP_BUTTON_B2 )
#define TASK_BSP_BUTTON_B3                  ( 1u << CFG_TASK_BSP_BUTTON_B3 )

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
  CFG_EVENT_ZIGBEE_CALLBACK_DONE,
  CFG_EVENT_ZIGBEE_RESTART_WAIT,
  CFG_EVENT_ZIGBEE_STARTUP_ENDED,
  CFG_EVENT_ZIGBEE_STARTUP_PERSISTENCE_ENDED,
  CFG_EVENT_ZIGBEE_OTA_SERVER_FOUND,
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
#define EVENT_ZIGBEE_CALLBACK_DONE      ( 1U << CFG_EVENT_ZIGBEE_CALLBACK_DONE )
#define EVENT_ZIGBEE_RESTART_WAIT       ( 1U << CFG_EVENT_ZIGBEE_RESTART_WAIT )
#define EVENT_ZIGBEE_STARTUP_ENDED      ( 1U << CFG_EVENT_ZIGBEE_STARTUP_ENDED )
#define EVENT_ZIGBEE_STARTUP_PERSISTENCE_ENDED  ( 1U << CFG_EVENT_ZIGBEE_STARTUP_PERSISTENCE_ENDED )
#define EVENT_ZIGBEE_OTA_SERVER_FOUND   ( 1U << CFG_EVENT_ZIGBEE_OTA_SERVER_FOUND )
#define EVENT_ZIGBEE_APP1               ( 1U << CFG_EVENT_ZIGBEE_APP1 )
#define EVENT_ZIGBEE_APP2               ( 1U << CFG_EVENT_ZIGBEE_APP2 )
#define EVENT_ZIGBEE_APP3               ( 1U << CFG_EVENT_ZIGBEE_APP3 )
#define EVENT_ZIGBEE_APP4               ( 1U << CFG_EVENT_ZIGBEE_APP4 )
/* USER CODE BEGIN EVENT_ID_Define */

/* USER CODE END EVENT_ID_Define */

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
/* Link Layer CTE degradation switch from FCC (0 --> NO ; 1 --> YES) */
#define USE_CTE_DEGRADATION                 (0)

#define RADIO_INTR_NUM                      RADIO_IRQn     /* 2.4GHz RADIO global interrupt */
#define RADIO_INTR_PRIO_HIGH                (0)            /* 2.4GHz RADIO interrupt priority when radio is Active */
#define RADIO_INTR_PRIO_LOW                 (5)            /* 2.4GHz RADIO interrupt priority when radio is Not Active - Sleep Timer Only */

#define RADIO_SW_LOW_INTR_NUM               HASH_IRQn      /* Selected interrupt vector for 2.4GHz RADIO low ISR */
#define RADIO_SW_LOW_INTR_PRIO              (15)           /* 2.4GHz RADIO low ISR priority */

#define RCC_INTR_PRIO                       (1)           /* HSERDY and PLL1RDY */

/* RF TX power table ID selection:
 *   0 -> RF TX output level from -20 dBm to +10 dBm. VDDRFPA at VDD level.
 *   1 -> RF TX output level from -20 dBm to +3 dBm. VDDRFPA at VDD11 level like on ST MB1803 and MB2130 boards.
 *   3 -> RF TX output level from -20 dBm to +3 dBm, optimised with VOS_1_5. VDDRFPA at VDD11 level like on ST MB1803 and MB2130 boards
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
/**
 * User interaction
 * When CFG_LED_SUPPORTED is set, LEDS are activated if requested
 * When CFG_BUTTON_SUPPORTED is set, the push button are activated if requested
 */

#define CFG_BSP_ON_NUCLEO           (1)
#define CFG_BSP_ON_SEQUENCER        (1)

#define CFG_LED_SUPPORTED           (1)
#define CFG_BUTTON_SUPPORTED        (1)

/**
 * If CFG_LPM_LEVEL at 2, make sure LED are disabled
 */
#if (CFG_LPM_LEVEL > 1)
  #undef  CFG_LED_SUPPORTED
  #define CFG_LED_SUPPORTED         (0)
#endif /* CFG_FULL_LOW_POWER */

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
    #define CFG_LOG_SUPPORTED       (0U)
  #endif /* CFG_LOG_SUPPORTED */
  #if CFG_DEBUGGER_LEVEL
    #undef  CFG_DEBUGGER_LEVEL
    #define CFG_DEBUGGER_LEVEL      (0U)
  #endif /* CFG_DEBUGGER_LEVEL */
#endif /* CFG_LPM_LEVEL */

#if (CFG_LPM_LEVEL == 0)
  #undef CFG_LPM_STOP1_SUPPORTED
  #define CFG_LPM_STOP1_SUPPORTED   (0U)
  #undef CFG_LPM_STANDBY_SUPPORTED
  #define CFG_LPM_STANDBY_SUPPORTED (0U)
#endif

/*********************************************************************
 * CAUTION: CFG_LPM_STDBY_SUPPORTED is deprecated and must be removed
 * Please use a combination of previous defines instead
 * Temporary define for backward compatibility
 *********************************************************************/
 #if (CFG_LPM_STANDBY_SUPPORTED == 1U)
 #define CFG_LPM_STDBY_SUPPORTED (1U)
 #else
 #define CFG_LPM_STDBY_SUPPORTED (0U)
 #endif

/* USER CODE BEGIN Defines_2 */

/* USER CODE END Defines_2 */

#endif /*APP_CONF_H */
