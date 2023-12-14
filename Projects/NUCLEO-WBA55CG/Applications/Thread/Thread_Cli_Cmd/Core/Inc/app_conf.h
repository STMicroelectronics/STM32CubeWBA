/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * File Name          : app_conf.h
  * Description        : Application configuration file for STM32WPAN Middleware.
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
#include "log_module.h"

/******************************************************************************
 * Application Config
 ******************************************************************************/




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

  #undef  CFG_LOG_SUPPORTED
  #define CFG_LOG_SUPPORTED      (0)

#else /* CFG_FULL_LOW_POWER */
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
#define RTC_N_PREDIV_S 10
#define RTC_PREDIV_S ((1<<RTC_N_PREDIV_S)-1)
#define RTC_PREDIV_A ((1<<(15-RTC_N_PREDIV_S))-1)



/* USER CODE BEGIN RTC */

/* USER CODE END RTC */

/*****************************************************************************
 * Logs
 *
 * Enable (CFG_LOG_SUPPORTED != 0) or disable the logs (CFG_LOG_SUPPORTED = 0)
 * in the application.
 *
 * APP_DBG_MSG is the macro ensuring retrocompatibility with old applications.
 * It uses the log module.
 * Applications must now call LOG_INFO_APP for logs.
 * By default, CFG_LOG_INSERT_TIME_STAMP_INSIDE_THE_TRACE is set to 0.
 * As a result, there is no time stamp insertion inside the logs.
 *
 * For advanced log use cases, see the log_module.h file.
 * This file is customizable, you can create new verbose levels and log regions.
 *****************************************************************************/

/* Configure Log display settings */
#define CFG_LOG_INSERT_COLOR_INSIDE_THE_TRACE       (1U)
#define CFG_LOG_INSERT_TIME_STAMP_INSIDE_THE_TRACE  (1U)
#define CFG_LOG_INSERT_EOL_INSIDE_THE_TRACE         (1U)

/**
 * Enable or disable logs in the application
 * Enabling low power automatically disables the log module
 */
#ifndef CFG_LOG_SUPPORTED
  #define CFG_LOG_SUPPORTED         (1U)
#endif /* CFG_LOG_SUPPORTED */

#define APP_DBG                     LOG_INFO_APP
#define APP_DBG_MSG                 LOG_INFO_APP

/* USER CODE BEGIN Logs */

/* USER CODE END Logs */

/******************************************************************************
 * Configure Log level for Application
 ******************************************************************************/
#define APPLI_CONFIG_LOG_LEVEL      LOG_VERBOSE_INFO

/* USER CODE BEGIN Log_level */

/* USER CODE END Log_level */



/******************************************************************************
 * Sequencer
 ******************************************************************************/

/**
 * This is the list of task id required by the application
 * Each Id shall be in the range 0..31
 */
typedef enum
{
  CFG_TASK_HW_RNG,                /* Task linked to chip internal peripheral. */
  CFG_TASK_LINK_LAYER,            /* Tasks linked to Communications Layers. */
  CFG_TASK_LINK_LAYER_TEMP_MEAS,
  CFG_TASK_OT_UART,
  CFG_TASK_OT_ALARM,
  CFG_TASK_OT_US_ALARM,

  /* SNPS LL TRACES */
  CFG_TASK_OT_LL_TRACES,

  /* APPLI TASKS */
  CFG_TASK_OT_TASKLETS,
  CFG_TASK_SET_THREAD_MODE,

  CFG_TASK_NBR  /**< Shall be last in the list */
} CFG_IdleTask_Id_t;

/* Scheduler types and defines        */
/*------------------------------------*/
#define TASK_LINK_LAYER      (1U << CFG_TASK_LINK_LAYER)
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
} CFG_SEQ_Prio_Id_t;;

#define CFG_SCH_LINK_LAYER      CFG_SEQ_PRIO_0
#define CFG_SCH_OT_ALARM        CFG_SEQ_PRIO_1
#define CFG_SCH_OT_US_ALARM     CFG_SEQ_PRIO_1
#define CFG_SCH_OT_TASKLETS     CFG_SEQ_PRIO_1
#define CFG_SCH_OT_UART         CFG_SEQ_PRIO_1

/**
 * This is a bit mapping over 32bits listing all events id supported in the application
 */
typedef enum
{
  CFG_EVT_TEST
  /* USER CODE BEGIN CFG_IdleEvt_Id_t */

  /* USER CODE END CFG_IdleEvt_Id_t */
} CFG_IdleEvt_Id_t;

/* Sequencer priorities by default  */
#define CFG_TASK_PRIO_HW_RNG                CFG_SEQ_PRIO_0
#define CFG_TASK_PRIO_LINK_LAYER            CFG_SEQ_PRIO_0

/* USER CODE BEGIN DEFINE_EVENT */

/* USER CODE END DEFINE_EVENT */


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
#define USE_TEMPERATURE_BASED_RADIO_CALIBRATION  (0)

#define RADIO_INTR_NUM                      RADIO_IRQn     /* 2.4GHz RADIO global interrupt */
#define RADIO_INTR_PRIO_HIGH                (0)            /* 2.4GHz RADIO interrupt priority when radio is Active */
#define RADIO_INTR_PRIO_LOW                 (5)            /* 2.4GHz RADIO interrupt priority when radio is Not Active - Sleep Timer Only */

#if (USE_RADIO_LOW_ISR == 1)
#define RADIO_SW_LOW_INTR_NUM               HASH_IRQn      /* Selected interrupt vector for 2.4GHz RADIO low ISR */
#define RADIO_SW_LOW_INTR_PRIO              (15)           /* 2.4GHz RADIO low ISR priority */
#endif /* USE_RADIO_LOW_ISR */

/* Link Layer supported number of antennas */
#define RADIO_NUM_OF_ANTENNAS               (4)

#define RCC_INTR_PRIO                       (1)           /* HSERDY and PLL1RDY */

/* RF TX power table ID selection:
 *   0 -> RF TX output level from -20 dBm to +10 dBm
 *   1 -> RF TX output level from -20 dBm to +3 dBm
 */
#define CFG_RF_TX_POWER_TABLE_ID            (0)

/* USER CODE BEGIN Radio_Configuration */

/* USER CODE END Radio_Configuration */

/******************************************************************************
 * HW_RNG configuration
 ******************************************************************************/

/* Number of 32-bit random values stored in internal pool */
#define CFG_HW_RNG_POOL_SIZE                (32)

/* USER CODE BEGIN HW_RNG_Configuration */

/* USER CODE END HW_RNG_Configuration */


/* USER CODE BEGIN Defines */
/**
 * User interaction
 * When CFG_LED_SUPPORTED is set, LEDS are activated if requested
 * When CFG_BUTTON_SUPPORTED is set, the push button are activated if requested
 * When CFG_DBG_SUPPORTED is set, the debugger is activated
 */

#define CFG_LED_SUPPORTED                       (0)
#define CFG_BUTTON_SUPPORTED                    (0)
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