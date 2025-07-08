/**
 ******************************************************************************
 * @file    stm32_lpm.h
 * @author  MCD Application Team
 * @brief   Header for stm32_lpm.c module
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
#ifndef STM32_TINY_LPM_H
#define STM32_TINY_LPM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "utilities_conf.h"

/** @defgroup TINY_LPM TINY LPM
  * @{
  */

/* Exported typedef ---------------------------------------------------------*/
/** @defgroup TINY_LPM_Exported_typedef TINY LPM exported typedef
  * @{
  */

/**
 * @brief Bitmask type definition for low power manager (LPM) operations.
 *
 * This typedef defines a bitmask type used for representing different clients
 * and their respective low power mode states.
 *
 * @note As a bitmap, each client must be represented by a single bit
 */
typedef uint32_t UTIL_LPM_bm_t;

/**
 * @brief Structure for low power mode driver configuration.
 *
 * This structure holds the configuration for a low power mode driver,
 * including the mode and the function pointer to the driver function.
 */
typedef void (* const UTIL_LPM_Driver_fp)(uint32_t);

#if (UTIL_LPM_LEGACY_ENABLED == 1)
/**
 * @brief Enumeration to represent the state of a low power mode.
 *
 * This enumeration defines the possible states for enabling or disabling
 * a low power mode.
 *
 * @note Deprecated, this is only used for backward compatibility.
 */
typedef enum
{
  UTIL_LPM_ENABLE,     /**< Enable low power mode */
  UTIL_LPM_DISABLE,    /**< Disable low power mode */
} UTIL_LPM_State_t;

/**
 * @brief Enumeration to represent low power modes.
 *
 * This enumeration defines the various low power modes that the system can enter.
 *
 * @note Deprecated, this is only used for backward compatibility.
 */
typedef enum
{
  UTIL_LPM_SLEEPMODE,
  UTIL_LPM_STOPMODE,
  UTIL_LPM_OFFMODE,
} UTIL_LPM_LegacyMode_t;

/**
 * @brief LPM driver structure definition
 *
 * @note Deprecated, this is only used for backward compatibility.
 */
struct UTIL_LPM_Driver_s
{
  void (*EnterSleepMode) ( void ); /*!<function to enter the sleep mode */
  void (*ExitSleepMode) ( void );  /*!<function to exit the sleep mode  */
  void (*EnterStopMode) ( void );  /*!<function to enter the stop mode  */
  void (*ExitStopMode) ( void );   /*!<function to exit the stop mode   */
  void (*EnterOffMode) ( void );   /*!<function to enter the off mode   */
  void (*ExitOffMode) ( void );    /*!<function to exit the off mode    */
};

/**
 * @brief LPM driver
 *
 * This structure is defined and initialized in the specific platform
 * power implementation.
 *
 * @note Deprecated, this is only used for backward compatibility.
 */
extern const struct UTIL_LPM_Driver_s UTIL_PowerDriver;

#endif /* UTIL_LPM_LEGACY_ENABLED */

/**
 * @}
 */

/* Exported macros  ------------------------------------------------------*/
/** @defgroup TINY_LPM_Exported_macro TINY LPM exported definitions
  * @{
  */   

/* Exported macros -----------------------------------------------------------*/

#if (UTIL_LPM_LEGACY_ENABLED == 1)
/**
 * @brief Enters the most efficient low power mode.
 *
 * This function configures the system to enter the most efficient low power mode
 * allowed based on client settings.
 *
 * @note This macro is deprecated, UTIL_LPM_EnterMode() should be used instead.
 */    
#define UTIL_LPM_EnterLowPower()    UTIL_LPM_Enter(0UL) 

/**
 * @brief Returns mode that will be applied when the system will enter low power.
 *
 * This API returns the Low Power Mode selected that will be applied 
 * when the system will enter low power mode.
 *         
 * @note This macro is deprecated, UTIL_LPM_GetMaxMode() should be used instead.
 */    
#define UTIL_LPM_GetMode()    UTIL_LPM_GetMaxMode() 

#endif /* UTIL_LPM_LEGACY_ENABLED */

/**
 * @}
 */
   
/** @defgroup TINY_LPM_Exported_macro TINY LPM exported struct
  * @{
  */

/**
 * @}
 */

/* External variables --------------------------------------------------------*/

#if (UTIL_LPM_LEGACY_ENABLED == 0)
/** @defgroup TINY_LPM_Exported_struct TINY LPM exported struct
  * @{
  */

/**
 * @brief Array of LPM driver configurations.
 *
 * This array contains the configurations for different low power modes
 * and the corresponding functions to handle those modes.
 *
 * @note This array is defined and initialized in the specific platform
 *       interface file
 */
extern const UTIL_LPM_Driver_fp UTIL_LPM_Driver[];

/**
 * @brief Number of LPM driver configurations.
 *
 * This constant holds the number of entries in the UTIL_LPM_PowerDriver array.
 *
 * @note This array is defined and initialized in the specific platform
 *       interface file.
 */
extern const uint32_t UTIL_LPM_Driver_num;

/**
 * @}
 */
#endif /* UTIL_LPM_LEGACY_ENABLED */

/* Exported functions ------------------------------------------------------- */

/** @defgroup TINY_LPM_Exported_function TINY LPM exported functions
 * @{
 */

/**
 * @brief Initializes the low power manager (LPM).
 *
 * This function initializes the low power manager.
 *
 * @return None
 */
void UTIL_LPM_Init(void);

/**
 * @brief De-initializes the low power manager (LPM).
 *
 * This function de-initializes the low power manager.
 *
 * @return None
 */
void UTIL_LPM_DeInit(void);

/**
 * @brief Gets the current highest low power mode allowed.
 *
 * This function returns the current highest low power mode that the system can go
 *
 * @note It actually returns the index of the highest allowed LPM driver
 *
 * @return The current low power mode
 */
uint8_t UTIL_LPM_GetMaxMode(void);

/**
 * @brief Sets the highest low power mode for a specific bitmask.
 *
 * This function sets the highest low power mode allowed for the specified 
 * bitmask identifier (client).
 * All modes lower or equal to this one can be reached
 *
 * @param lpm_id_bm Bitmask identifier for the client.
 * @param lp_mode The highest low power mode to set.
 *
 * @return None
 */
void UTIL_LPM_SetMaxMode(UTIL_LPM_bm_t lpm_id_bm, uint8_t lp_mode);

/**
 * @brief Enters the most efficient low power mode.
 *
 * This function configures the system to enter the most efficient low power mode
 * allowed based on client settings.
 *
 * @param option Optional configuration parameter for low power driver.
 *
 * @note This API is called in a critical section for baremetal implementation
 *
 * @return None
 */
void UTIL_LPM_Enter(uint32_t option);

#if (UTIL_LPM_LEGACY_ENABLED == 1)
/**
 * @brief Enable or disable Standby mode
 *
 * This API notifies the low power manager if the specified user allows
 * Standby mode or not.
 *
 * @param lpm_id_bm user bitmask
 * @param state (enable/disable)
 *
 * @note Deprecated, UTIL_LPM_SetMaxMode() should be used instead.
 */ 
void UTIL_LPM_SetOffMode(UTIL_LPM_bm_t lpm_id_bm, UTIL_LPM_State_t state);

/**
 * @brief Enable or disable Stop1 mode
 *
 * This API notifies the low power manager if the specified user allows
 * Stop1 mode or not.
 *
 * @param lpm_id_bm user bitmask
 * @param state (enable/disable)
 *
 * @note Deprecated, UTIL_LPM_SetMaxMode() should be used instead.
 */
void UTIL_LPM_SetStopMode(UTIL_LPM_bm_t lpm_id_bm, UTIL_LPM_State_t state);

#endif /* UTIL_LPM_LEGACY_ENABLED */

/**
 *@}
 */


#ifdef __cplusplus
}
#endif

#endif /* STM32_TINY_LPM_H */

