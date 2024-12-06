/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32_rtos.h
  * @author  MCD Application Team
  * @brief   Include file for all RTOS/Sequencer can be used on WBA
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
#ifndef ZIGBEE_STM32WBA_SYS_H
#define ZIGBEE_STM32WBA_SYS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
extern void ZigbeeSys_Process       ( void );
extern void ZigbeeSys_Init          ( void );
extern void ZigbeeSys_Resume        ( void );
extern void ZigbeeSys_SemaphoreSet  ( void );
extern void ZigbeeSys_SemaphoreWait ( void );
extern void ZigbeeSys_EventSet      ( void );
extern void ZigbeeSys_EventWait     ( void );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // ZIGBEE_STM32WBA_SYS_H
