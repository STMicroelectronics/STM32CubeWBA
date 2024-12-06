/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * File Name          : flash.h
  * Description        : OpenThread platform flash header
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FLASH_H
#define FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <openthread/instance.h>
  
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef enum{
  SETTINGS_ADDED,
  SETTINGS_REMOVED,
  SETTINGS_MASSERASE
}settings_type_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Exported types ------------------------------------------------------------*/

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief this callback will be called each time settings have been added or removed
 * depending of setting update types user can define action
 *
 * @param SettingType
 * SETTINGS_ADDED : Advice to add settings in ROM by task in low prio to keep better ot performances
 * SETTINGS_REMOVED : Advice to remove settings in ROM by task in low prio to keep better ot performances
 * SETTINGS_MASSERASE : ROM need to be erased right now in blocking way, device will reset just after otPlatSettingsWipe
 *
 * Please use GetSettingBuffer() to get OT settings buffer & current size to copy in ROM
*/
void APP_THREAD_SettingsUpdated(settings_type_t SettingType);


/**
 * @brief return maximal size of setting buffer
*/
uint32_t GetSettingsBuffer_MaxSize(void);

/**
 * @brief fill setting buffer
 * 
 * @param buf : input buffer to be copied in OT settings
 * @param size : input size of buffer to be copied in OT settings
 * size < GetSettingsBuffer_MaxSize() else OT_ERROR_NO_BUFS
 *
 * FillSettingBuffer() can be used
*/
otError FillSettingBuffer(uint8_t* buf, uint32_t size);

/**
 * @brief get setting buffer
 * 
 * @param *size : output current OT settings buffer size
 * @return *uint8_t: output OT settings buffer
 *
 *
*/
uint8_t* GetSettingBuffer(uint32_t* size);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* FLASH_H */



