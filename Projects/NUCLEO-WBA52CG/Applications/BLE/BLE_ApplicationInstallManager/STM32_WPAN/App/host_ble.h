/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    host_ble.h
  * @author  MCD Application Team
  * @brief   Header for App/host_ble.c
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
#ifndef HOST_BLE_H
#define HOST_BLE_H

#ifdef __cplusplus
extern "C" {
#endif

  /* Includes ------------------------------------------------------------------*/

  /* Exported types ------------------------------------------------------------*/

  /* Exported constants --------------------------------------------------------*/
  /* External variables --------------------------------------------------------*/
  /* Exported macros -----------------------------------------------------------*/
  /* Exported functions ------------------------------------------------------- */
/**
 * @brief  BLE Host initialization
 *
 * @param  None
 * @retval None
 */
uint8_t  HOST_BLE_Init(void);

#ifdef __cplusplus
}
#endif

#endif /*HOST_BLE_H */
