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
  CFG_LPM_LOG,
  CFG_LPM_APP_THREAD,
  /* USER CODE BEGIN CFG_LPM_Id_t */

  /* USER CODE END CFG_LPM_Id_t */
} CFG_LPM_Id_t;

/* Core voltage supply selection, it can be PWR_LDO_SUPPLY or PWR_SMPS_SUPPLY */
#define CFG_CORE_SUPPLY          (PWR_LDO_SUPPLY)


/**
 * EEPROM emulation defines
 *  
 */
/*
   CFG_SNVMA_START_ADDRESS : address of the beginning of the NVM
                            (shall be within allocation range of scatterfile)

   CFG_NVM_CACHE_SIZE : max size of the RAM cache in bytes

   CFG_NVM_NB_OF_PAGE : Number of page of flash to use

   CFG_PERSISTENCE_ENABLED: enables persistence in flash with EEPROM emulation
                            ("RAM persistence" is always active)

*/

#define CFG_PERSISTENCE_ENABLED                 (1U)
#define CFG_NVM_NB_OF_PAGE                      (7U)
#define CFG_NVM_SIZE                            (CFG_NVM_NB_OF_PAGE * FLASH_PAGE_SIZE)
#define CFG_NVM_CACHE_SIZE                      (0xC00U) /* Max data in bytes (3K) */

#if defined(FLASH_DBANK_SUPPORT)
#define CFG_SNVMA_START_ADDRESS       (FLASH_BASE + (FLASH_PAGE_SIZE * ((FLASH_SIZE / FLASH_PAGE_SIZE) - CFG_NVM_NB_OF_PAGE)) - FLASH_PAGE_SIZE)
#else
#define CFG_SNVMA_START_ADDRESS       (FLASH_BASE + (FLASH_PAGE_SIZE * ((FLASH_SIZE / FLASH_PAGE_SIZE) - CFG_NVM_NB_OF_PAGE)))
#endif

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
#define CFG_DEBUG_APP_TRACE         (1)
/* Configure Log display settings */
#define CFG_LOG_INSERT_COLOR_INSIDE_THE_TRACE       (1U)
#define CFG_LOG_INSERT_TIME_STAMP_INSIDE_THE_TRACE  (0U)
#define CFG_LOG_INSERT_EOL_INSIDE_THE_TRACE         (1U)

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

#define APP_TRACE_REGION        ( LOG_REGION_APP | LOG_REGION_ZIGBEE )    /* Indicate Trace Code 'Region' for Application */
#define APP_TRACE_LEVEL         ( VLEVEL_L )                              /* Indicate Trace Level for Application ( Essential ) */


/* Indicate Trace Level for Zigbee Stack (Fatal/Error) */
#define ZIGBEE_CONFIG_LOG_LEVEL     ZB_LOG_MASK_LEVEL_2

/* USER CODE BEGIN Logs */

/* USER CODE END Logs */

/******************************************************************************
 * Configure Log level for Application
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
  CFG_TASK_HW_RNG,                /* Task linked to chip internal peripheral. */
  CFG_TASK_LINK_LAYER,            /* Tasks linked to Communications Layers. */
  CFG_TASK_MAC_LAYER,
  CFG_TASK_AMM_BCKGND,            /* AMM background task */
  CFG_TASK_FLASH_MANAGER_BCKGND,  /* Tasks linked to flash manager */
  CFG_TASK_ZIGBEE_LAYER,
  CFG_TASK_ZIGBEE_NETWORK_FORM,   /* Tasks linked to Zigbee Start. */
  CFG_TASK_ZIGBEE_APP_START,
  CFG_TASK_ZIGBEE_APP1,           /* Tasks linked to Zigbee Application. */
  CFG_TASK_ZIGBEE_APP2,
  CFG_TASK_ZIGBEE_APP3,
  CFG_TASK_ZIGBEE_APP4,
  /* USER CODE BEGIN CFG_Task_Id_t */
  CFG_TASK_BUTTON_SW1,		        /* Task linked to push-button. */
  CFG_TASK_BUTTON_SW2,
  CFG_TASK_BUTTON_SW3,
  CFG_TASK_ZIGBEE_ZBCLI_APP,
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

/* Sequencer defines */
#define TASK_HW_RNG                         ( 1u << CFG_TASK_HW_RNG )
#define TASK_LINK_LAYER                     ( 1u << CFG_TASK_LINK_LAYER )
#define TASK_MAC_LAYER                      ( 1u << CFG_TASK_MAC_LAYER )
#define TASK_ZIGBEE_LAYER                   ( 1u << CFG_TASK_ZIGBEE_LAYER )
#define TASK_FLASH_MNGR                     ( 1u << CFG_TASK_FLASH_MANAGER_BCKGND )
#define TASK_ZIGBEE_NETWORK_FORM            ( 1u << CFG_TASK_ZIGBEE_NETWORK_FORM )
#define TASK_ZIGBEE_APP_START               ( 1u << CFG_TASK_ZIGBEE_APP_START )
#define TASK_ZIGBEE_APP1                    ( 1u << CFG_TASK_ZIGBEE_APP1 )
#define TASK_ZIGBEE_APP2                    ( 1u << CFG_TASK_ZIGBEE_APP2 )
#define TASK_ZIGBEE_APP3                    ( 1u << CFG_TASK_ZIGBEE_APP3 )
#define TASK_ZIGBEE_APP4                    ( 1u << CFG_TASK_ZIGBEE_APP3 )
/* USER CODE BEGIN TASK_ID_Define */
#define TASK_BUTTON_SW1                     ( 1u << CFG_TASK_BUTTON_SW1 )
#define TASK_BUTTON_SW2                     ( 1u << CFG_TASK_BUTTON_SW2 )
#define TASK_BUTTON_SW3                     ( 1u << CFG_TASK_BUTTON_SW3 )
#define TASK_ZIGBEE_ZBCLI_APP               ( 1u << CFG_TASK_ZIGBEE_ZBCLI_APP )
/* USER CODE END TASK_ID_Define */

/* Used by Sequencer */
#define UTIL_SEQ_CONF_PRIO_NBR              CFG_SEQ_PRIO_NBR

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
  CFG_EVENT_ZBCLI_TIMER,
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
#define EVENT_ZBCLI_TIMER               ( 1U << CFG_EVENT_ZBCLI_TIMER )

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

#define CFG_RT_DEBUG_GPIO_MODULE         (0)
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
#define RADIO_SW_LOW_INTR_PRIO              (13)           /* 2.4GHz RADIO low ISR priority */
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

/******************************************************************************
 * MEMORY MANAGER
 ******************************************************************************/
#ifdef APPLICATION_USE_BAREMETAL

#define CFG_MM_POOL_SIZE                          (64000)  /* bytes */
#define CFG_AMM_VIRTUAL_MEMORY_NUMBER             (2u)
#define CFG_AMM_VIRTUAL_APP_ZB_INIT               (1U)
#define CFG_AMM_VIRTUAL_APP_ZB_INIT_BUFFER_SIZE   (10000U) /* words (32 bits) */
#define CFG_AMM_VIRTUAL_APP_ZB_HEAP               (2U)
#define CFG_AMM_VIRTUAL_APP_ZB_HEAP_BUFFER_SIZE   (2000U)  /* words (32 bits) */
#define CFG_AMM_POOL_SIZE                         DIVC(CFG_MM_POOL_SIZE, sizeof (uint32_t)) \
                                                  + (AMM_VIRTUAL_INFO_ELEMENT_SIZE * CFG_AMM_VIRTUAL_MEMORY_NUMBER)
#endif
                                                    
#ifdef APPLICATION_USE_THREADX

#define CFG_MM_POOL_SIZE                          (56000u)  /* bytes */
#define CFG_AMM_VIRTUAL_MEMORY_NUMBER             (2u)
#define CFG_AMM_VIRTUAL_APP_ZB_INIT               (1U)
#define CFG_AMM_VIRTUAL_APP_ZB_INIT_BUFFER_SIZE   (10000u) /* words (32 bits) */
#define CFG_AMM_VIRTUAL_APP_ZB_HEAP               (2U)
#define CFG_AMM_VIRTUAL_APP_ZB_HEAP_BUFFER_SIZE   (2000U)  /* words (32 bits) */
#define CFG_AMM_POOL_SIZE                         DIVC(CFG_MM_POOL_SIZE, sizeof (uint32_t)) \
                                                  + (AMM_VIRTUAL_INFO_ELEMENT_SIZE * CFG_AMM_VIRTUAL_MEMORY_NUMBER)                                                    
#endif
                                                    
#ifdef APPLICATION_USE_FREERTOS

#define CFG_MM_POOL_SIZE                          (56000u)  /* bytes */
#define CFG_AMM_VIRTUAL_MEMORY_NUMBER             (2u)
#define CFG_AMM_VIRTUAL_APP_ZB_INIT               (1U)
#define CFG_AMM_VIRTUAL_APP_ZB_INIT_BUFFER_SIZE   (10000U) /* words (32 bits) */
#define CFG_AMM_VIRTUAL_APP_ZB_HEAP               (2U)
#define CFG_AMM_VIRTUAL_APP_ZB_HEAP_BUFFER_SIZE   (2000U)  /* words (32 bits) */
#define CFG_AMM_POOL_SIZE                         DIVC(CFG_MM_POOL_SIZE, sizeof (uint32_t)) \
                                                  + (AMM_VIRTUAL_INFO_ELEMENT_SIZE * CFG_AMM_VIRTUAL_MEMORY_NUMBER)                                                    
#endif

/* USER CODE BEGIN MEMORY_MANAGER_Configuration */

/* USER CODE END MEMORY_MANAGER_Configuration */
                                                    
/* USER CODE BEGIN Defines */
#ifdef APPLICATION_USE_BAREMETAL
/**
 * User interaction
 * When CFG_LED_SUPPORTED is set, LEDS are activated if requested
 * When CFG_BUTTON_SUPPORTED is set, the push button are activated if requested
 */
#define CFG_LED_SUPPORTED           (1)
#define CFG_BUTTON_SUPPORTED        (1)
#endif
#ifdef APPLICATION_USE_THREADX
/**
 * User interaction
 * When CFG_LED_SUPPORTED is set, LEDS are activated if requested
 * When CFG_BUTTON_SUPPORTED is set, the push button are activated if requested
 */
#define CFG_LED_SUPPORTED           (1)
#define CFG_BUTTON_SUPPORTED        (0)
#endif
#ifdef APPLICATION_USE_FREERTOS
/**
 * User interaction
 * When CFG_LED_SUPPORTED is set, LEDS are activated if requested
 * When CFG_BUTTON_SUPPORTED is set, the push button are activated if requested
 */
#define CFG_LED_SUPPORTED           (1)
#define CFG_BUTTON_SUPPORTED        (1)
#endif

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

/* USER CODE END Defines_2 */

#endif /*APP_CONF_H */
