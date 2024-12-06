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
#include "stm32wbaxx_nucleo.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/******************************************************************************
 * Application Config
 ******************************************************************************/
/**< generic parameters ******************************************************/
/* USER CODE BEGIN Generic_Parameters */

/******************************************************************************
 * UART interfaces
 ******************************************************************************/

#define CFG_HW_LPUART1_ENABLED           1
#define CFG_HW_LPUART1_DMA_TX_SUPPORTED  1

#define CFG_HW_USART1_ENABLED           1
#define CFG_HW_USART1_DMA_TX_SUPPORTED  1

#define CFG_HW_USART2_ENABLED           0
#define CFG_HW_USART2_DMA_TX_SUPPORTED  0
#define CFG_HW_USART2_DMA_RX_SUPPORTED  0

/**
 * Define to 1 enable usart_if.c, you must link uart handler with function UartIf_Init()
 */
#define IF_USART_USE         1
extern UART_HandleTypeDef    huart1;
extern UART_HandleTypeDef    hlpuart1;
#define IF_USART_TX          huart1
#define IF_USART_RX          huart1

/* USER CODE BEGIN Specific_Parameters */

/* USER CODE END Specific_Parameters */

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

/* Core voltage supply selection, it can be PWR_LDO_SUPPLY or PWR_SMPS_SUPPLY */
#define CFG_CORE_SUPPLY          (PWR_LDO_SUPPLY)

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

/* Configure Log display settings */
#define CFG_LOG_INSERT_COLOR_INSIDE_THE_TRACE       (0U)
#define CFG_LOG_INSERT_TIME_STAMP_INSIDE_THE_TRACE  (0U)
#define CFG_LOG_INSERT_EOL_INSIDE_THE_TRACE         (0U)

/* macro ensuring retrocompatibility with old applications */
#define APP_DBG                     LOG_INFO_APP
#define APP_DBG_MSG                 LOG_INFO_APP

/* USER CODE BEGIN Logs */

/* USER CODE END Logs */

/******************************************************************************
 * Configure Log level for Application
 ******************************************************************************/
#define APPLI_CONFIG_LOG_LEVEL      (LOG_LEVEL_INFO)

/* USER CODE BEGIN Log_level */

/* USER CODE END Log_level */


/**
 * User button Interrupt Handlers
 */
#define PUSH_BUTTON1_EXTI_IRQHandler     EXTI13_IRQHandler
#define PUSH_BUTTON2_EXTI_IRQHandler     EXTI6_IRQHandler
#define PUSH_BUTTON3_EXTI_IRQHandler     EXTI7_IRQHandler

/******************************************************************************
 * Configure Log level for Application
 ******************************************************************************/
#define APPLI_PRINT_FILE_FUNC_LINE  (0)

/* USER CODE BEGIN Defines */

/* USER CODE END Defines */
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
  CFG_TASK_LINK_LAYER_TEMP_MEAS,
  CFG_TASK_MAC_LAYER,
  CFG_TASK_RFD,
  CFG_TASK_DATA_POOL,
  /* USER CODE BEGIN CFG_Task_Id_t */
  CFG_TASK_BUTTON_1,
  CFG_TASK_BUTTON_2,
  CFG_TASK_BUTTON_3,
  /* USER CODE END CFG_Task_Id_t */
  CFG_TASK_NBR /* Shall be LAST in the list */
} CFG_Task_Id_t;

/* Scheduler types and defines        */
/*------------------------------------*/
#define TASK_LINK_LAYER         (1U << CFG_TASK_LINK_LAYER)
#define TASK_MAC_LAYER          (1U << CFG_TASK_MAC_LAYER)     
#define TASK_RFD                (1U << CFG_TASK_RFD)

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

/* Sequencer priorities by default  */
#define CFG_TASK_PRIO_HW_RNG                CFG_SEQ_PRIO_0
#define CFG_TASK_PRIO_LINK_LAYER            CFG_SEQ_PRIO_0
#define TASK_MAC_LAYER_PRIO                 CFG_SEQ_PRIO_0
#define TASK_MAC_APP_PRIO					CFG_SEQ_PRIO_1
/* USER CODE BEGIN TASK_Priority_Define */

/* USER CODE END TASK_Priority_Define */

/* Used by Sequencer */
#define UTIL_SEQ_CONF_PRIO_NBR              CFG_SEQ_PRIO_NBR

/**
 * This is a bit mapping over 32bits listing all events id supported in the application
 */
typedef enum
{
  /* USER CODE BEGIN CFG_IdleEvt_Id_t */
  CFG_EVT_RESET_CNF,              /* RESET CNF */
  CFG_EVT_SET_CNF,                /* SET PIB CNF */
  CFG_EVT_GET_CNF,                /* GET PIB CNF */
  CFG_EVT_START_CNF,              /* START CNF */
  CFG_EVT_RX_ON_WHEN_IDLE_CNF,    /* RX ON WHEN IDLE CNF */
  CFG_EVT_ASSOCIATE_CNF,          /* ASSOCIATE CONF */
  CFG_EVT_DATA_CNF,               /* DATA CNF */
  CFG_EVT_SCAN_CNF,               /* SCAN CNF */
  CFG_EVT_POLL_CNF,               /* POLL CNF */
  CFG_EVT_DISASSOCIATE_CNF,       /* DISASSOCIATE CNF */
  CFG_EVT_PURGE_CNF,              /* PURGE CNF */
  CFG_EVT_BEACON_CNF,             /* BEACON CNF */
  CFG_EVT_GET_PWR_INFO_TABLE_CNF, /* GET PWR INFO TABLE CNF */
  CFG_EVT_SET_PWR_INFO_TABLE_CNF, /* SET PWR INFO TABLE CNF */
  /* USER CODE END CFG_IdleEvt_Id_t */
  CFG_EVENT_NBR                   /* Shall be LAST in the list */
} CFG_IdleEvt_Id_t;

/* USER CODE BEGIN DEFINE_EVENT */
/*Events managed by Setup Task*/
#define EVENT_RESET_CNF                 (1U << CFG_EVT_RESET_CNF)
#define EVENT_SET_CNF                   (1U << CFG_EVT_SET_CNF)
#define EVENT_GET_CNF                   (1U << CFG_EVT_GET_CNF)
#define EVENT_ASSOCIATE_CNF             (1U << CFG_EVT_ASSOCIATE_CNF)
#define EVENT_START_CNF                 (1U << CFG_EVT_START_CNF)
#define EVENT_RX_ON_WHEN_IDLE_CNF       (1U << CFG_EVT_RX_ON_WHEN_IDLE_CNF)
#define EVENT_SCAN_CNF                  (1U << CFG_EVT_SCAN_CNF)
#define EVENT_POLL_CNF                  (1U << CFG_EVT_POLL_CNF)
#define EVENT_DISASSOCIATE_CNF          (1U << CFG_EVT_DISASSOCIATE_CNF)
#define EVENT_DATA_CNF                  (1U << CFG_EVT_DATA_CNF)
#define EVENT_PURGE_CNF                 (1U << CFG_EVT_PURGE_CNF)
#define EVENT_BEACON_CNF                (1U << CFG_EVT_BEACON_CNF)
#define EVENT_GET_PWR_INFO_TABLE_CNF    (1U << CFG_EVT_GET_PWR_INFO_TABLE_CNF)
#define EVENT_SET_PWR_INFO_TABLE_CNF    (1U << CFG_EVT_SET_PWR_INFO_TABLE_CNF)

/* USER CODE END DEFINE_EVENT */


/******************************************************************************
 * Debugger
 *
 *  When CFG_DEBUGGER_LEVEL is set to:
 *   - 0 : No Debugger available, SWD/JTAG pins are disabled.
 *   - 1 : Debugger available in RUN mode only.
 *   - 2 : Debugger available in low power mode.
 *
 ******************************************************************************/
#define CFG_DEBUGGER_LEVEL           (2)

/******************************************************************************
 * RealTime GPIO debug module configuration
 ******************************************************************************/

#define CFG_RT_DEBUG_GPIO_MODULE         (1)
#define CFG_RT_DEBUG_DTB                 (0)

/******************************************************************************
 * System Clock Manager module configuration
 ******************************************************************************/

#define CFG_SCM_SUPPORTED            (1)

/******************************************************************************
 * HW RADIO configuration
 ******************************************************************************/
/* Do not modify - must be 1 */
#define USE_RADIO_LOW_ISR                   (1)

/* Do not modify - must be 1 */
#define NEXT_EVENT_SCHEDULING_FROM_ISR      (1)

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
 */

#define CFG_LED_SUPPORTED           (1)
#define CFG_BUTTON_SUPPORTED        (1)

/**
 * If CFG_LPM_LEVEL at 2, make sure LED are disabled
 */
#if (CFG_LPM_LEVEL > 1)
  #undef  CFG_LED_SUPPORTED
  #define CFG_LED_SUPPORTED         (0)
#endif /* CFG_FULL_LOW_POWER */


/* 802.15.4 parameters */
#define MAC_RETRY_SCAN           0x03
#define BASE_SCAN_DURATION       0x03
#define BEACON_PAYLOAD           "BZH"
#define BEACON_PAYLOAD_SIZE      0x03

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

/******************************************************************************
 * HW TimerServer
 ******************************************************************************/
/**
 * This setting is used when standby mode is supported.
 * hw_ts_InitMode_Limited should be used when the device restarts from Standby Mode. In that case, the Timer Server does
 * not re-initialized its context. Only the Hardware register which content has been lost is reconfigured
 * Otherwise, hw_ts_InitMode_Full should be requested (Start from Power ON) and everything is re-initialized.
 */
typedef enum
{
  hw_ts_InitMode_Full,
  hw_ts_InitMode_Limited,
} HW_TS_InitMode_t;

/**
 * When a Timer is created as a SingleShot timer, it is not automatically restarted when the timeout occurs. However,
 * the timer is kept reserved in the list and could be restarted at anytime with HW_TS_Start()
 *
 * When a Timer is created as a Repeated timer, it is automatically restarted when the timeout occurs.
 */
typedef enum
{
  hw_ts_SingleShot,
  hw_ts_Repeated
} HW_TS_Mode_t;

/**
 * hw_ts_Successful is returned when a Timer has been successfully created with HW_TS_Create(). Otherwise, hw_ts_Failed
 * is returned. When hw_ts_Failed is returned, that means there are not enough free slots in the list to create a
 * Timer. In that case, CFG_HW_TS_MAX_NBR_CONCURRENT_TIMER should be increased
 */
typedef enum
{
  hw_ts_Successful,
  hw_ts_Failed,
}HW_TS_ReturnStatus_t;

/* USER CODE BEGIN Defines_2 */

/* USER CODE END Defines_2 */

#endif /*APP_CONF_H */
