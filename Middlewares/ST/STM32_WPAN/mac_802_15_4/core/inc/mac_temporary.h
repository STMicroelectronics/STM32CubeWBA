/******************************************************************************
 * @file    mac_temporary.h
 * @author
 * @brief   Header for mac interface.
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

#ifndef MAC_TEMPORARY_H
#define MAC_TEMPORARY_H


//#include "cmsis_iccarm.h" // Must be remove
#include "cmsis_compiler.h"
#include "mem_intf.h"
#include "string.h"
#include "bsp.h"
//#include "log_module.h" // MAC log

#ifndef ASSERT
/*
 * Define Two assertion severity levels HIGH for conditions that requires system reset and LOW for conditions that can be bypassed
 * */
#define SEVERITY_LOW		0
#define SEVERITY_HIGH		1

#if LOGGER_ENABLE
	#define ASSERT(condition, severity)			bsp_assert_log(condition, severity, __func__, __LINE__)
#else
        #define ASSERT(condition, severity)			bsp_assert(condition, severity)
#endif /* LOGGER_ENABLE */
#endif /* ASSERT*/

/* Debug Buffer Management */
#define ST_MAC_HANDLE_INCOMING_MAC_CMD 0x00
#define ST_MAC_HANDLE_OUTGOING_MAC_MSG 0x01

/* For Keil compilateur need to used ble_memcpy, otherwise issue with 32-bit alignment */
#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#define MAC_MEMSET  ble_memset
#define MAC_MEMCPY  ble_memcpy
#else
#define MAC_MEMSET  memset
#define MAC_MEMCPY  memcpy
#endif

// MUST BE REMOVED WHEN FULL INTEGRATION IS SCHEDULED
#define COMMON_RF_M_BEGIN     do {

#define COMMON_RF_M_END       } while(0)

#ifndef ENTER_CRITICAL_REGION
#define ENTER_CRITICAL_REGION()                   COMMON_RF_M_BEGIN uint32_t _primask = __get_PRIMASK( ); \
                                                                    __disable_irq( )
#endif /* ENTER_CRITICAL_REGION */

#ifndef LEAVE_CRITICAL_REGION
#define LEAVE_CRITICAL_REGION()                   __set_PRIMASK( _primask ); COMMON_RF_M_END
#endif /* LEAVE_CRITICAL_REGION */
                                                                      
                                                                      
 // BYTES UTILS 
                                                                      /* Write bytes to memory macro like function */
#define MAC_WRITE_2_BYTES(pckt,pos,var)     \
	do {\
		((uint8_t *)pckt)[pos] = (uint8_t)(var);\
		((uint8_t *)pckt)[pos + 1] = (uint8_t)(((var) & 0xff00) >> 8);\
	} while(0)

#define MAC_WRITE_3_BYTES(pckt,pos,var)     \
	do {\
		((uint8_t *)pckt)[pos] = (uint8_t)(var);\
		((uint8_t *)pckt)[pos + 1] = (uint8_t) ((var) >> 8);\
                ((uint8_t *)pckt)[pos + 2] = (uint8_t) ((var) >> 16);\
	} while(0)

#define MAC_WRITE_4_BYTES(pckt,pos,var)     \
	do {\
		((uint8_t *)pckt)[pos] = (uint8_t)(var);\
		((uint8_t *)pckt)[pos + 1] = (uint8_t) ((var) >> 8);\
                ((uint8_t *)pckt)[pos + 2] = (uint8_t) ((var) >> 16);\
                ((uint8_t *)pckt)[pos + 3] = (uint8_t) ((var) >> 24);\
	} while(0)
    
          
/** @brief  For log, fill even if 0x00 */
#define MAC_READ_8_BYTES(pckt, pos) (((uint64_t) (pckt)[pos]) | \
		                      (((uint32_t) (pckt)[pos+1])<< 8) | \
				       (((uint32_t) (pckt)[pos+2])<< 16) | \
                                       (((uint32_t) (pckt)[pos+3])<< 24) | \
                                       (((uint32_t) (pckt)[pos+4])<< 32) | \
                                       (((uint32_t) (pckt)[pos+5])<< 40) | \
                                       (((uint32_t) (pckt)[pos+6])<< 48) | \
                                       (((uint32_t) (pckt)[pos+7])<< 56))
          
/* LOG_REGION_MAC */
#ifdef CFG_LOG_SUPPORTED
	#define LOG_INFO_MAC(...)         Log_Module_Print( LOG_VERBOSE_INFO, LOG_REGION_MAC, __VA_ARGS__)
	#define LOG_ERROR_MAC(...)        Log_Module_Print( LOG_VERBOSE_ERROR, LOG_REGION_MAC, __VA_ARGS__)
	#define LOG_WARNING_MAC(...)      Log_Module_Print( LOG_VERBOSE_WARNING, LOG_REGION_MAC, __VA_ARGS__)
	#define LOG_DEBUG_MAC(...)        Log_Module_Print( LOG_VERBOSE_DEBUG, LOG_REGION_MAC, __VA_ARGS__)
#else
	#define LOG_INFO_MAC(...)
	#define LOG_ERROR_MAC(...)
	#define LOG_WARNING_MAC(...)
	#define LOG_DEBUG_MAC(...)
#endif

/* USER CODE BEGIN LOG_REGION_MAC */
/**
 * Add inside this user section your defines to match the new verbose levels you
 * created into Log_Verbose_Level_t.
 * Example :
 * #define LOG_CUSTOM_MAC(...)         Log_Module_Print( LOG_VERBOSE_CUSTOM, LOG_REGION_APP, __VA_ARGS__);
 *
 * You don't need to update all regions with your custom values.
 * Do it accordingly to your needs. E.g you might not need LOG_VERBOSE_CUSTOM
 * for a System region.
 */
          
#endif /* MAC_TEMPORARY_H */