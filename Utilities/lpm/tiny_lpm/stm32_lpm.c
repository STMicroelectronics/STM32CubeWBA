/**
 ******************************************************************************
 * @file    stm32_lpm.c
 * @author  MCD Application Team
 * @brief   Low Power Manager
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

/* Includes ------------------------------------------------------------------*/
#include "stm32_lpm.h"

/** @addtogroup TINY_LPM
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/** @defgroup TINY_LPM_Private_macros TINY LPM private macros
  * @{
  */

/**
 * @brief macro used to initialized the critical section
 */
#ifndef UTIL_LPM_INIT_CRITICAL_SECTION
  #define UTIL_LPM_INIT_CRITICAL_SECTION( )
#endif

/**
 * @brief macro used to enter the critical section
 */
#ifndef UTIL_LPM_ENTER_CRITICAL_SECTION
  #define UTIL_LPM_ENTER_CRITICAL_SECTION( )    UTILS_ENTER_CRITICAL_SECTION( )
#endif

/**
 * @brief macro used to exit the critical section
 */
#ifndef UTIL_LPM_EXIT_CRITICAL_SECTION
  #define UTIL_LPM_EXIT_CRITICAL_SECTION( )     UTILS_EXIT_CRITICAL_SECTION( )
#endif

/**
 * @brief macro used to enter the critical section when Entering Low Power 
 * @note  this macro is only called inside the function UTIL_LPM_EnterLowPower
 *        and in a basic configuration shall be identcal to the macro 
 *        UTIL_LPM_EXIT_CRITICAL_SECTION. In general, the request to enter the
 *        low power mode is already done under a critical section and 
 *        nesting it is useless (in specific implementations not even possible). 
 *        So the users could define their own macro)
 */
#ifndef UTIL_LPM_ENTER_CRITICAL_SECTION_ELP
  #define UTIL_LPM_ENTER_CRITICAL_SECTION_ELP( )    UTIL_LPM_ENTER_CRITICAL_SECTION( )
#endif

/**
 * @brief macro used to exit the critical section when exiting Low Power mode
 * @note  the behavior of the macro shall be symmetrical with the macro 
 *        UTIL_LPM_ENTER_CRITICAL_SECTION_ELP
 */
#ifndef UTIL_LPM_EXIT_CRITICAL_SECTION_ELP
  #define UTIL_LPM_EXIT_CRITICAL_SECTION_ELP( )     UTIL_LPM_EXIT_CRITICAL_SECTION( )
#endif

/**
 * @}
 */
/* Private function prototypes -----------------------------------------------*/
#if (UTIL_LPM_LEGACY_ENABLED == 1)
/**
 * @brief Forward private functions declaration
 */
static void LPM_SleepMode(uint32_t param);
static void LPM_Stop1Mode(uint32_t param);
static void LPM_StandbyMode(uint32_t param);
#endif /* UTIL_LPM_LEGACY_ENABLED */

/* Private variables ---------------------------------------------------------*/
/** @defgroup TINY_LPM_Private_variables TINY LPM private variables
  * @{
  */

/**
 * @brief Low Power Manager control table.
 *
 * This static array holds the control information for each low power mode.
 */
static UTIL_LPM_bm_t UTIL_LPM_ControlTable[UTIL_LPM_DRIVER_MAX_NUM];    
    
#if (UTIL_LPM_LEGACY_ENABLED == 1)
/**
 * @brief State variable for the off mode.
 * @note  Only used on backward compatibility purpose, for deprecated APIs
 */
static UTIL_LPM_State_t off_state = UTIL_LPM_ENABLE;

/**
 * @brief State variable for the stop mode.
 * @note  Only used on backward compatibility purpose, for deprecated APIs
 */
static UTIL_LPM_State_t stop_state = UTIL_LPM_ENABLE;

/**
 * @brief Array of LPM driver configurations.
 *
 * This array contains the configurations for different low power modes
 * and the corresponding functions to handle those modes.
 *
 *@note It must be ordered from the less efficient (index 0)
 *      to the most efficient low power mode
 */
static const UTIL_LPM_Driver_fp UTIL_LPM_Driver[] =
{
  LPM_SleepMode,
  LPM_Stop1Mode,
  LPM_StandbyMode
};

/**
 * @brief Number of LPM drivers.
 *
 * This constant holds the number of entries in the UTIL_LPM_Driver array.
 */
static const uint32_t UTIL_LPM_Driver_num = sizeof(UTIL_LPM_Driver) / sizeof(UTIL_LPM_Driver_fp);

#endif /* UTIL_LPM_LEGACY_ENABLED */

/**
 * @}
 */

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/** @defgroup TINY_LPM_Private_define TINY LPM private defines
  * @{
  */
/**
 * @brief value used to reset LPM user bitmask field
 */
#define UTIL_LPM_NO_BIT_SET  (0UL)

/**
 * @brief value used to set LPM user bitmask field
 */
#define UTIL_LPM_ALL_BIT_SET (~0UL)

/**
 * @brief value used to reset LPM driver index
 */
#define DRIVER_UNDEFINED     (~0UL)
   

/**
 * @}
 */

/* Private macros ------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

/* Private Functions Definition ----------------------------------------------*/
#if (UTIL_LPM_LEGACY_ENABLED == 1)
/** @defgroup TINY_LPM_Private_Function TINY LPM private functions
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
static void LPM_SleepMode(uint32_t param)
{
	UTIL_PowerDriver.EnterSleepMode();
	UTIL_PowerDriver.ExitSleepMode();
}

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
static void LPM_Stop1Mode(uint32_t param)
{
	UTIL_PowerDriver.EnterStopMode();
	UTIL_PowerDriver.ExitStopMode();
}

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
static void LPM_StandbyMode(uint32_t param)
{
	UTIL_PowerDriver.EnterOffMode();
	UTIL_PowerDriver.ExitOffMode();
}
#endif /* UTIL_LPM_LEGACY_ENABLED */
/**
 * @}
 */

/* Functions Definition ------------------------------------------------------*/

/** @addtogroup TINY_LPM_Exported_function
  * @{
  */

/**
 * @brief Initializes the low power manager (LPM).
 *
 * This function initializes the low power manager.
 *
 * @return None
 */
void UTIL_LPM_Init(void)
{
  uint32_t index;
  
  /* initialize UTIL_LPM_ControlTable */
  for (index = 0; index < UTIL_LPM_Driver_num; index++)
  {
    UTIL_LPM_ControlTable[index] = UTIL_LPM_NO_BIT_SET;
  }
  
  /* by default, enable highest registered mode */ 
  UTIL_LPM_ControlTable[UTIL_LPM_Driver_num - 1] = UTIL_LPM_ALL_BIT_SET;

#if (UTIL_LPM_LEGACY_ENABLED == 1)
  /* reset internal states */
  off_state = UTIL_LPM_ENABLE;
  stop_state = UTIL_LPM_ENABLE;
#endif /* UTIL_LPM_LEGACY_ENABLED */
  
  UTIL_LPM_INIT_CRITICAL_SECTION();
}

/**
 * @brief De-initializes the low power manager (LPM).
 *
 * This function de-initializes the low power manager.
 *
 * @return None
 */
void UTIL_LPM_DeInit(void)
{
}

/**
 * @brief Sets the low power mode for a specific bitmask.
 *
 * This function sets the highest low power mode allowed for the specified 
 * bitmask identifier (client).
 * All modes lower or equal to this one can be reached
 *
 * @param id_bm Bitmask identifier for the client.
 * @param lp_mode The low power mode to set.
 *
 * @return None
 */
void UTIL_LPM_SetMaxMode(UTIL_LPM_bm_t id_bm, uint8_t lp_mode)
{
  uint32_t index = 0;
  
  /* check mode is supported */
  if (lp_mode < UTIL_LPM_Driver_num)
  {
    UTIL_LPM_ENTER_CRITICAL_SECTION();
    
    /* clear user bit for modes lower than the one requested */
    for (index = 0; index < lp_mode ; index++)
    { 
      UTIL_LPM_ControlTable[index] &= (~id_bm);
    }
    /* set user bit for requested mode */
    UTIL_LPM_ControlTable[index] |= id_bm;

    UTIL_LPM_EXIT_CRITICAL_SECTION();
  }
}

/**
 * @brief Gets the current low power mode.
 *
 * This function returns the current low power mode that the system is in.
 *
 * @return The current low power mode
 */
uint8_t UTIL_LPM_GetMaxMode(void)
{
  uint8_t index = 0;

  UTIL_LPM_ENTER_CRITICAL_SECTION();

  /* at least highest mode enabled by default
     (except no driver is registered) */
  while((UTIL_LPM_ControlTable[index] == UTIL_LPM_NO_BIT_SET) &&
        (index < UTIL_LPM_Driver_num))
  {
    index++;
  }
 
  UTIL_LPM_EXIT_CRITICAL_SECTION();

  return index;
}

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
void UTIL_LPM_Enter(uint32_t option)
{
  uint32_t index = 0;
  
  UTIL_LPM_ENTER_CRITICAL_SECTION_ELP();
  
  /* look for first enabled mode, from lowest to highest */
  /* at least highest mode is enabled by default */
  while((UTIL_LPM_ControlTable[index] == UTIL_LPM_NO_BIT_SET) &&
        (index < UTIL_LPM_Driver_num))
  {
    index++;
  }
  
  /* call relevant driver from UTIL_Driver array */
  UTIL_LPM_Driver[index](option);
  
  UTIL_LPM_EXIT_CRITICAL_SECTION_ELP();
}


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
void UTIL_LPM_SetOffMode(UTIL_LPM_bm_t lpm_id_bm, UTIL_LPM_State_t state)
{
  
  UTIL_LPM_ENTER_CRITICAL_SECTION();
  
  off_state = state;
  
  if (stop_state == UTIL_LPM_ENABLE)
  {
    /* if off_state is UTIL_LPM_ENABLE (== 0), sets UTIL_LPM_OFFMODE 
       otherwise sets UTIL_LPM_STOPMODE (UTIL_LPM_OFFMODE - 1) */
    UTIL_LPM_SetMaxMode(lpm_id_bm, UTIL_LPM_OFFMODE - (uint8_t)state);
  }
  else
  {
    UTIL_LPM_SetMaxMode(lpm_id_bm, UTIL_LPM_SLEEPMODE);
  }
  
  UTIL_LPM_EXIT_CRITICAL_SECTION();
}

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
void UTIL_LPM_SetStopMode(UTIL_LPM_bm_t lpm_id_bm, UTIL_LPM_State_t state)   
{
  
  UTIL_LPM_ENTER_CRITICAL_SECTION();
  
  stop_state = state;
  
  if (state == UTIL_LPM_DISABLE)
  {
    UTIL_LPM_SetMaxMode(lpm_id_bm, UTIL_LPM_SLEEPMODE);
  }
  else
  {
    /* if off_state is UTIL_LPM_ENABLE (== 0), sets UTIL_LPM_OFFMODE 
       otherwise sets UTIL_LPM_STOPMODE (UTIL_LPM_OFFMODE - 1) */
    UTIL_LPM_SetMaxMode(lpm_id_bm, UTIL_LPM_OFFMODE - (uint8_t)off_state);
  }
  
  UTIL_LPM_EXIT_CRITICAL_SECTION();
}

#endif /* UTIL_LPM_LEGACY_ENABLED */

/**
 * @}
 */

/**
 * @}
 */
