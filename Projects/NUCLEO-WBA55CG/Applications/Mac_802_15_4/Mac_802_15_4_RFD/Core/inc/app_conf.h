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
#include "stm32wbaxx_nucleo.h"
#include "stm32_adv_trace.h"
#include "log_module.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/******************************************************************************
 * Application Config
 ******************************************************************************/
/******************************************************************************
 * Transport Layer
 ******************************************************************************/
/**
 * Queue length of BLE Event
 * This parameter defines the number of asynchronous events that can be stored in the HCI layer before
 * being reported to the application. When a command is sent to the BLE core coprocessor, the HCI layer
 * is waiting for the event with the Num_HCI_Command_Packets set to 1. The receive queue shall be large
 * enough to store all asynchronous events received in between.
 * When CFG_TLBLE_MOST_EVENT_PAYLOAD_SIZE is set to 27, this allow to store three 255 bytes long asynchronous events
 * between the HCI command and its event.
 * This parameter depends on the value given to CFG_TLBLE_MOST_EVENT_PAYLOAD_SIZE. When the queue size is to small,
 * the system may hang if the queue is full with asynchronous events and the HCI layer is still waiting
 * for a CC/CS event, In that case, the notification TL_BLE_HCI_ToNot() is called to indicate
 * to the application a HCI command did not receive its command event within 30s (Default HCI Timeout).
 */
#define CFG_TL_EVT_QUEUE_LENGTH 5
/**
 * This parameter should be set to fit most events received by the HCI layer. It defines the buffer size of each element
 * allocated in the queue of received events and can be used to optimize the amount of RAM allocated by the Memory Manager.
 * It should not exceed 255 which is the maximum HCI packet payload size (a greater value is a lost of memory as it will
 * never be used)
 * With the current wireless firmware implementation, this parameter shall be kept to 255
 */
#define CFG_TL_MOST_EVENT_PAYLOAD_SIZE 255   /**< Set to 255 with the memory manager and the mailbox */

#define TL_EVENT_FRAME_SIZE ( TL_EVT_HDR_SIZE + CFG_TL_MOST_EVENT_PAYLOAD_SIZE )
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
#define IF_USART_TX          hlpuart1
#define IF_USART_RX          huart1

/******************************************************************************
 * USB interface
 ******************************************************************************/

/**
 * Enable/Disable USB interface
 */
#define CFG_USB_INTERFACE_ENABLE    0

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
#define CFG_LOG_INSERT_COLOR_INSIDE_THE_TRACE       (0U)
#define CFG_LOG_INSERT_TIME_STAMP_INSIDE_THE_TRACE  (0U)
#define CFG_LOG_INSERT_EOL_INSIDE_THE_TRACE         (0U)

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

/* USER CODE BEGIN Traces */

/* USER CODE END Traces */

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
  CFG_TASK_HCI_ASYNCH_EVT_ID,
  CFG_TASK_LINK_LAYER_TEMP_MEAS,
  CFG_TASK_BPKA,
  CFG_TASK_MAC_LAYER,
  CFG_TASK_RFD,
  CFG_TASK_AMM_BCKGND,
  CFG_TASK_FLASH_MANAGER_BCKGND,
  /* USER CODE BEGIN CFG_Task_Id_t */
  TASK_BUTTON_1,
  TASK_BUTTON_2,
  TASK_BUTTON_3,
  CFG_TASK_ADV_CANCEL_ID,
  CFG_TASK_SEND_NOTIF_ID,
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
 * Each Id shall be in the range 0..31
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

#define LL_IN_HIGH_PRIO 0x01

#if ( LL_IN_HIGH_PRIO == 0x01)
#define TASK_LINK_LAYER_PRIO  CFG_SEQ_PRIO_0
#define TASK_MAC_LAYER_PRIO   CFG_SEQ_PRIO_1
#define TASK_MAC_APP_PRIO     CFG_SEQ_PRIO_1
#define TASK_BPKA_PRIO        CFG_SEQ_PRIO_1
#define TASK_RNG_PRIO         CFG_SEQ_PRIO_1
#else
#define TASK_LINK_LAYER_PRIO  CFG_SEQ_PRIO_0
#define TASK_MAC_LAYER_PRIO   CFG_SEQ_PRIO_0
#define TASK_MAC_APP_PRIO     CFG_SEQ_PRIO_0
#define TASK_BPKA_PRIO        CFG_SEQ_PRIO_0
#define TASK_RNG_PRIO         CFG_SEQ_PRIO_0
#endif

/* USER CODE END DEFINE_EVENT */

/* Sequencer priorities by default  */
#define CFG_TASK_PRIO_HW_RNG                CFG_SEQ_PRIO_0
#define CFG_TASK_PRIO_LINK_LAYER            CFG_SEQ_PRIO_0

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

#define CFG_RT_DEBUG_GPIO_MODULE         (0)
#define CFG_RT_DEBUG_DTB                 (0)

/******************************************************************************
 * HW RADIO configuration
 ******************************************************************************/
/* Link Layer uses radio low interrupt (0 --> NO ; 1 --> YES) */
#define USE_RADIO_LOW_ISR                   (1)

/* Link Layer event scheduling (0 --> NO, next event schediling is done at background ; 1 --> YES) */
#define NEXT_EVENT_SCHEDULING_FROM_ISR      (1)

/* Link Layer uses temperature based calibration (0 --> NO ; 1 --> YES) */
#define USE_TEMPERATURE_BASED_RADIO_CALIBRATION  (1)

#if (DEFAULT_PHY_CALIBRATION_PERIOD == 0)
#undef USE_TEMPERATURE_BASED_RADIO_CALIBRATION
#define USE_TEMPERATURE_BASED_RADIO_CALIBRATION  (0)
#endif

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

/* RF TX power table ID selection:
 *   0 -> RF TX output level from -20 dBm to +10 dBm
 *   1 -> RF TX output level from -20 dBm to +3 dBm
 */
#define CFG_RF_TX_POWER_TABLE_ID            0

/* USER CODE END Defines */

#endif /*APP_CONF_H */
