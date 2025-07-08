/*******************************************************************************
 * @file    stm32_lpm_if.c
 * @author  MCD Application Team
 * @brief   Low layer function to enter/exit low power modes (stop, sleep)
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
#include "stm32_lpm_if.h"
#include "stm32_lpm.h"

/** @addtogroup TINY_LPM_IF
  * @{
  */

/* USER CODE BEGIN include */

/* USER CODE END include */

/* Exported variables --------------------------------------------------------*/
/** @defgroup TINY_LPM_IF_Exported_variables TINY LPM IF exported variables
  * @{
  */

/**
 * @brief Array of LPM driver configurations.
 *
 * This array contains the configurations for different low power modes
 * and the corresponding functions to handle those modes.
 *
 *@note It must be ordered from the less efficient (index 0) 
 *      to the most efficient low power mode 
 */
const UTIL_LPM_Driver_fp UTIL_LPM_Driver[] =
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
const uint32_t UTIL_LPM_Driver_num = sizeof(UTIL_LPM_Driver) / sizeof(UTIL_LPM_Driver_fp);

/**
 * @brief Assertion to ensure at least one driver is registered in interface file.
 */
static_assert(sizeof(UTIL_LPM_Driver) != 0, "at least one LPM driver is required");
/* Check not too many drivers are registered in interface file */

/**
 * @brief Assertion to ensure registered drivers are within boundaries.
 */
static_assert((sizeof(UTIL_LPM_Driver) / sizeof(UTIL_LPM_Driver_fp)) <= UTIL_LPM_DRIVER_MAX_NUM, 
              "too many LPM drivers registered");

/**
 * @brief Assertion to ensure drivers and associated enum type are aligned
 */
static_assert((sizeof(UTIL_LPM_Driver) / sizeof(UTIL_LPM_Driver_fp)) == UTIL_LPM_NUM_MODES, 
              "UTIL_LPM_Mode_t enum not aligned with UTIL_LPM_Driver size");
              
/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN Private_Function_Prototypes */

/* USER CODE END Private_Function_Prototypes */
/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN Private_Typedef */

/* USER CODE END Private_Typedef */
/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Private_Define */

/* USER CODE END Private_Define */
/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Private_Macro */

/* USER CODE END Private_Macro */
/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Private_Variables */

/* USER CODE END Private_Variables */

/** @addtogroup TINY_LPM_IF_Exported_functions
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
void LPM_SleepMode(uint32_t param)
{
/* USER CODE BEGIN LPM_SleepMode */

/* USER CODE END LPM_SleepMode */
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
void LPM_Stop1Mode(uint32_t param)
{
/* USER CODE BEGIN LPM_Stop1Mode */

/* USER CODE END LPM_Stop1Mode */
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
void LPM_StandbyMode(uint32_t param)
{
/* USER CODE BEGIN LPM_StandbyMode */

/* USER CODE END LPM_StandbyMode */
}

/* USER CODE BEGIN Private_Functions */

/* USER CODE END Private_Functions */

/**
 * @}
 */

/**
 * @}
 */

