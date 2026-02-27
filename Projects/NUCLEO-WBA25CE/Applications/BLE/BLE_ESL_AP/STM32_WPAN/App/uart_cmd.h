/**
  ******************************************************************************
  * @file    uart_cmd.h
  * @author  MCD Application Team
  * @brief   Header file for uart commands for ESL.
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
#ifndef UART_CMD_H
#define UART_CMD_H

void UART_CMD_Init(void);

void UART_CMD_ProcessRequestCB(void);

void UART_CMD_Process(void);


#endif /* UART_CMD_H */
