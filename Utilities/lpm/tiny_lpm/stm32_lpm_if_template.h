/**
 ******************************************************************************
 * @file    stm32_lpm_if.h
 * @brief   Header for stm32_lpm_f.c module (device specific LP management)
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics. 
 * All rights reserved.</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32_TINY_LPM_IF_H
#define STM32_TINY_LPM_IF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/** @defgroup TINY_LPM_IF TINY LPM IF
  * @{
  */

/* Exported Definitions ------------------------------------------------------------------*/

/** @defgroup TINY_LPM_IF_Exported_definitions TINY LPM IF Exported definitions
 * @{
 */

/* Exported typedef ---------------------------------------------------------*/
/** @defgroup TINY_LPM_Exported_typedef TINY LPM exported typedef
  * @{
  */
  
/**
 * @brief Enumeration for low power modes.
 *
 * This enumeration defines the various low power modes that the system can enter.
 *
 * @note It must be consistent with UTIL_LPM_Driver array definition
 */
typedef enum
{
  UTIL_LPM_SLEEP_MODE,           /**< Sleep mode */
  UTIL_LPM_STOP_MODE,            /**< Stop 1 mode */
  UTIL_LPM_STANDBY_MODE,         /**< Standby mode */
  UTIL_LPM_NUM_MODES             /**< Number of supported modes */
} UTIL_LPM_Mode_t;

/* Exported Functions ------------------------------------------------------------------*/

/** @defgroup TINY_LPM_IF_Exported_functions TINY LPM IF Exported functions
 * @{
 */

/**
 * @brief Puts the system into Sleep mode.
 *
 * This function configures the system to enter and exit Sleep mode. 
 * Any specific behavior can be controlled by the `param` parameter.
 *
 * @param param Configuration parameter for Sleep mode.        
 *
 * @return None
 */
void LPM_SleepMode(uint32_t param);

/**
 * @brief Puts the system into Stop1 mode.
 *
 * This function configures the system to enter and exit Stop1 mode. 
 * Any specific behavior can be controlled by the `param` parameter.
 *
 * @param param Configuration parameter for Stop1 mode.        
 *
 * @return None
 */
void LPM_Stop1Mode(uint32_t param);

/**
 * @brief Puts the system into Standby mode.
 *
 * This function configures the system to enter and exit Standby mode. 
 * Any specific behavior can be controlled by the `param` parameter.
 *
 * @param param Configuration parameter for Standby mode.        
 *
 * @return None
 */
void LPM_StandbyMode(uint32_t param);


/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* STM32_TINY_LPM_IF_H */

