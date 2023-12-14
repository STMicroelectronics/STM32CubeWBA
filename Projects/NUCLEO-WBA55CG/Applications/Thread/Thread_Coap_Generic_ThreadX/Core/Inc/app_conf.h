/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_conf.h
  * @author  MCD Application Team
  * @brief   Application configuration file for STM32WPAN Middleware.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
 *  When CFG_FULL_LOW_POWER is set to:
 *   - 0 : the low Power Mode is not activated
 *   - 1, 2 or 3 : the system is configured in low Power mode.
 *   - 2 : Communication for Debug are removed.
 *   - 3 ; Communication for Debug and LED are removed. It's assumed as Full Low Power Mode.
 *
 ******************************************************************************/
#define CFG_FULL_LOW_POWER       (0)
#define CFG_LPM_STDBY_SUPPORTED  (0)

/**
 * Low Power configuration
 */
#if (CFG_FULL_LOW_POWER > 0)
#undef CFG_LPM_SUPPORTED
#define CFG_LPM_SUPPORTED        (1)
#else /* CFG_FULL_LOW_POWER > 0 */
#undef CFG_LPM_SUPPORTED
#define CFG_LPM_SUPPORTED        (0)
#endif /* CFG_FULL_LOW_POWER > 0 */

#if (CFG_FULL_LOW_POWER > 1)
#undef  CFG_LOG_SUPPORTED
#define CFG_LOG_SUPPORTED        (0)
#endif /* CFG_LPM_STDBY_SUPPORTED > 0 */

/* USER CODE BEGIN Low_Power 0 */

/* USER CODE END Low_Power 0 */

/**
 * Supported requester to the MCU Low Power Manager - can be increased up  to 32
 * It list a bit mapping of all user of the Low Power Manager
 */
typedef enum
{
  CFG_LPM_APP,
  CFG_LPM_APP_THREAD,
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
#define CFG_LOG_INSERT_TIME_STAMP_INSIDE_THE_TRACE  (0U)
#define CFG_LOG_INSERT_EOL_INSIDE_THE_TRACE         (1U)

/**
 * Enable or disable logs in the application
 * Enabling low power automatically disables the log module
 */
#ifndef CFG_LOG_SUPPORTED
#define CFG_LOG_SUPPORTED           (1U)
#endif /* CFG_LOG_SUPPORTED */

#define APP_DBG                     LOG_INFO_APP
#define APP_DBG_MSG                 LOG_INFO_APP

/* USER CODE BEGIN Logs */

/* USER CODE END Logs */

/******************************************************************************
 * Configure Log level for Application
 ******************************************************************************/
#define APPLI_CONFIG_LOG_LEVEL      LOG_VERBOSE_WARNING

/* USER CODE BEGIN Log_level */

/* USER CODE END Log_level */

/******************************************************************************
 * Configure Serial Link used for Thread Command Line
 ******************************************************************************/
#define OT_CLI_USE                  (1U)
extern UART_HandleTypeDef           hlpuart1;
#define OT_CLI_UART_HANDLER         hlpuart1

/******************************************************************************
 * Sequencer
 ******************************************************************************/

/**
 * These are the lists of task id registered to the sequencer
 * Each task id shall be in the range [0:31]
 */
typedef enum
{
  CFG_TASK_HW_RNG,                /* Task linked to chip internal peripheral. */
  CFG_TASK_LINK_LAYER,            /* Tasks linked to Communications Layers. */
  CFG_TASK_OT_UART,
  CFG_TASK_OT_ALARM,
  CFG_TASK_OT_US_ALARM,

  /* APPLI TASKS */
  CFG_TASK_OT_TASKLETS,
  CFG_TASK_SET_THREAD_MODE,
  /* USER CODE BEGIN CFG_Task_Id_t */
  CFG_TASK_BUTTON_SW1,
  CFG_TASK_BUTTON_SW2,
  CFG_TASK_BUTTON_SW3,

  /* USER CODE END CFG_Task_Id_t */
  CFG_TASK_NBR /* Shall be LAST in the list */
} CFG_Task_Id_t;

/* USER CODE BEGIN DEFINE_TASK */

/* USER CODE END DEFINE_TASK */


/* USER CODE BEGIN TASK_ID_Define */

/* USER CODE END TASK_ID_Define */

/* Sequencer priorities by default  */
#define CFG_TASK_PRIO_HW_RNG                6u 
#define CFG_TASK_PRIO_ALARM                 6u 
#define CFG_TASK_PRIO_ALARM_US              CFG_TASK_PRIO_ALARM
#define CFG_TASK_PRIO_TASKLETS              7u 
#define CFG_TASK_PRIO_LINK_LAYER            8u  
#define CFG_TASK_PRIO_UART                  12u 

/* USER CODE BEGIN TASK_Priority_Define */
#define CFG_TASK_PRIO_BUTTON_SWx            13u 

/* USER CODE END TASK_Priority_Define */

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

#define RADIO_INTR_NUM                      RADIO_IRQn     /* 2.4GHz RADIO global interrupt */
#define RADIO_INTR_PRIO_HIGH                (0)            /* 2.4GHz RADIO interrupt priority when radio is Active */
#define RADIO_INTR_PRIO_LOW                 (5)            /* 2.4GHz RADIO interrupt priority when radio is Not Active - Sleep Timer Only */

#if (USE_RADIO_LOW_ISR == 1)
#define RADIO_SW_LOW_INTR_NUM               HASH_IRQn      /* Selected interrupt vector for 2.4GHz RADIO low ISR */
#define RADIO_SW_LOW_INTR_PRIO              (14)           /* 2.4GHz RADIO low ISR priority */
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

/******************************************************************************
 * User interaction
 * When CFG_LED_SUPPORTED is set, LEDS are activated if requested
 * When CFG_BUTTON_SUPPORTED is set, the push button are activated if requested
 ******************************************************************************/
#if (CFG_FULL_LOW_POWER == 1)
#define CFG_LED_SUPPORTED         (0)
#define CFG_BUTTON_SUPPORTED      (0)
#else
#define CFG_LED_SUPPORTED         (1)
#define CFG_BUTTON_SUPPORTED      (1)
#endif /* CFG_FULL_LOW_POWER */

/* USER CODE END Defines */

#endif /*APP_CONF_H */
