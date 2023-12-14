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
/* USER CODE END Header */

#include "stm32_adv_trace.h"
#include "hw_if.h"

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

/**
 * Select UART interfaces
 */
#define CFG_DEBUG_TRACE_UART    hw_lpuart1
#define CFG_CONSOLE_MENU
#define CFG_CLI_UART    hw_uart1
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

#define CFG_FULL_LOW_POWER    0

#if (CFG_FULL_LOW_POWER == 1)
#undef CFG_LPM_SUPPORTED
#define CFG_LPM_SUPPORTED   0
#endif /* CFG_FULL_LOW_POWER */

/******************************************************************************
 * RTC
 ******************************************************************************/
#define RTC_N_PREDIV_S 10
#define RTC_PREDIV_S ((1<<RTC_N_PREDIV_S)-1)
#define RTC_PREDIV_A ((1<<(15-RTC_N_PREDIV_S))-1)

#define RTC_ASYNCH_PREDIV       31u   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV        1023u /* LSE as RTC clock */

/******************************************************************************
 * Debug
 ******************************************************************************/
/**
 * When set, this resets some hw resources to set the device in the same state than the power up
 * The FW resets only register that may prevent the FW to run properly
 *
 * This shall be set to 0 in a final product
 *
 */
#define CFG_HW_RESET_BY_FW         1

/**
 * keep debugger enabled while in any low power mode when set to 1
 * should be set to 0 in production
 */
#define CFG_DEBUGGER_SUPPORTED    1

#if (CFG_FULL_LOW_POWER == 1)
#undef CFG_DEBUGGER_SUPPORTED
#define CFG_DEBUGGER_SUPPORTED    0
#endif /* CFG_FULL_LOW_POWER */

#define CFG_RT_DEBUG_GPIO_MODULE            0
#define CFG_RT_DEBUG_DTB                    1

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
#define APP_DBG(...)                                      \
{                                                                 \
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
#define CFG_DEBUG_TRACE_LIGHT     1
#define CFG_DEBUG_TRACE_FULL      0

#if (( CFG_DEBUG_TRACE != 0 ) && ( CFG_DEBUG_TRACE_LIGHT == 0 ) && (CFG_DEBUG_TRACE_FULL == 0))
#undef CFG_DEBUG_TRACE_FULL
#undef CFG_DEBUG_TRACE_LIGHT
#define CFG_DEBUG_TRACE_FULL      0
#define CFG_DEBUG_TRACE_LIGHT     1
#endif

#if ( CFG_DEBUG_TRACE == 0 )
#undef CFG_DEBUG_TRACE_FULL
#undef CFG_DEBUG_TRACE_LIGHT
#define CFG_DEBUG_TRACE_FULL      0
#define CFG_DEBUG_TRACE_LIGHT     0
#endif

/**
 * When not set, the traces is looping on sending the trace over UART
 */
#define DBG_TRACE_USE_CIRCULAR_QUEUE 1

/**
 * max buffer Size to queue data traces and max data trace allowed.
 * Only Used if DBG_TRACE_USE_CIRCULAR_QUEUE is defined
 */
#define DBG_TRACE_MSG_QUEUE_SIZE 4096
#define MAX_DBG_TRACE_MSG_SIZE 1024

/* IP Radio DTB config */
#define RADIO_DTB_TX_MODE 0
#define RADIO_DTB_RX_MODE 0

#if ((CFG_FULL_LOW_POWER == 0) && ((RADIO_DTB_TX_MODE == 1) || \
     (RADIO_DTB_RX_MODE == 1)) )
#define RADIO_DTB_MODE_CONFIG 1
#include "dtb_if.h"
#else
#define RADIO_DTB_MODE_CONFIG 0
#endif
/******************************************************************************
 * Configure Log level for Application
 ******************************************************************************/
#define APPLI_CONFIG_LOG_LEVEL    LOG_LEVEL_INFO
#define APPLI_PRINT_FILE_FUNC_LINE    0

/* USER CODE BEGIN Defines */
/******************************************************************************
 * User interaction
 * When CFG_LED_SUPPORTED is set, LEDS are activated if requested
 * When CFG_BUTTON_SUPPORTED is set, the push button are activated if requested
 ******************************************************************************/
#if (CFG_FULL_LOW_POWER == 1)
#define CFG_LED_SUPPORTED         0
#define CFG_BUTTON_SUPPORTED      0
#else
#define CFG_LED_SUPPORTED         0
#define CFG_BUTTON_SUPPORTED      0
#endif /* CFG_FULL_LOW_POWER */
/* USER CODE END Defines */

/******************************************************************************
 * Scheduler
 ******************************************************************************/
  /**
   * This is the list of task id required by the application
   * Each Id shall be in the range 0..31
   */

typedef enum
{
  CFG_TASK_TRACES,

  /* PHY TASKS */
  CFG_TASK_LINK_LAYER,
  CFG_TASK_PKT_CB,
  CFG_TASK_PHY_CLI_PROCESS,

  CFG_TASK_HW_RNG,

  /* SNPS LL TRACES */
  CFG_TASK_OT_LL_TRACES,

  CFG_TASK_NBR  /**< Shall be last in the list */
} CFG_IdleTask_Id_t;

/* Scheduler types and defines        */
/*------------------------------------*/
#define TASK_LINK_LAYER      (1U << CFG_TASK_LINK_LAYER)
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

#define CFG_SCH_LINK_LAYER      CFG_SEQ_PRIO_0
#define CFG_SCH_PHY_CLI_PROCESS CFG_SEQ_PRIO_1
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

#define EVENT_ACK_FROM_M0_EVT           (1U << CFG_EVT_ACK_FROM_M0_EVT)
#define EVENT_SYNCHRO_BYPASS_IDLE       (1U << CFG_EVT_SYNCHRO_BYPASS_IDLE)
/* USER CODE BEGIN DEFINE_EVENT */

/* USER CODE END DEFINE_EVENT */

/******************************************************************************
 * LOW POWER
 ******************************************************************************/
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

/******************************************************************************
 * OTP manager
 ******************************************************************************/
#define CFG_OTP_BASE_ADDRESS    OTP_AREA_BASE

#define CFG_OTP_END_ADRESS      OTP_AREA_END_ADDR

/******************************************************************************
 * HW RADIO configuration
 ******************************************************************************/
#define USE_RADIO_LOW_ISR                      (1)            /* Link Layer uses radio low interrupt (0 --> NO) 
                                                                                                     (1 --> YES) 
                                                               */

#define NEXT_EVENT_SCHEDULING_FROM_ISR         (1)            /* Link Layer uses radio low interrupt (0 --> NO --> Next event schediling is done at background) 
                                                                                                     (1 --> YES)                                                   
                                                               */

#define RADIO_INTR_NUM            RADIO_IRQn     /* 2.4GHz RADIO global interrupt */
#define RADIO_INTR_PRIO_HIGH      (0)            /* 2.4GHz RADIO interrupt priority when radio is Active */
#define RADIO_INTR_PRIO_LOW       (3)            /* 2.4GHz RADIO interrupt priority when radio is Not Active - Sleep Timer Only */

#if (USE_RADIO_LOW_ISR == 1)
#define RADIO_SW_LOW_INTR_NUM     HASH_IRQn      /* Selected interrupt vector for 2.4GHz RADIO low ISR */
#define RADIO_SW_LOW_INTR_PRIO    (15)           /* 2.4GHz RADIO low ISR priority */
#endif /* USE_RADIO_LOW_ISR */

#define RCC_INTR_PRIO                       (1)           /* HSERDY and PLL1RDY */

#define TOTAL_NUM_IRQ         (HSEM_S_IRQn + 1)

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

/* RF TX power table ID selection:
 *   0 -> RF TX output level from -20 dBm to +10 dBm
 *   1 -> RF TX output level from -20 dBm to +3 dBm
 */
#define CFG_RF_TX_POWER_TABLE_ID            0
                                                 
#endif /*APP_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
