/**
 ******************************************************************************
  * File Name          : app_tests_commands.h
  * Description        : Header file for CLI Commands to test Concurrents Modes (BLE/Thread/Zigbee)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_TESTS_COMMANDS_H
#define APP_TESTS_COMMANDS_H

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void  AppTestCommand_Init       ( void );
bool  AppTestCommand_Dispatch   (  uint8_t * pRxBuffer, uint16_t iRxBufferSize );

#endif /* APP_TESTS_COMMANDS_H */
