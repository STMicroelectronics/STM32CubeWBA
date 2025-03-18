/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    hw_conf.h
  * @author  MCD Application Team
  * @brief   Hardware configuration file for STM32WPAN Middleware.
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
#ifndef HW_CONF_H
#define HW_CONF_H

/******************************************************************************
 * Semaphores
 * THIS SHALL NO BE CHANGED AS THESE SEMAPHORES ARE USED AS WELL ON THE CM0+
 *****************************************************************************/
/**
*  The CPU2 may be configured to store the Thread persistent data either in internal NVM storage on CPU2 or in
*  SRAM2 buffer provided by the user application. This can be configured with the system command SHCI_C2_Config()
*  When the CPU2 is requested to store persistent data in SRAM2, it can write data in this buffer at any time when needed.
*  In order to read consistent data with the CPU1 from the SRAM2 buffer, the flow should be:
*  + CPU1 takes CFG_HW_THREAD_NVM_SRAM_SEMID semaphore
*  + CPU1 reads all persistent data from SRAM2 (most of the time, the goal is to write these data into an NVM managed by CPU1)
*  + CPU1 releases CFG_HW_THREAD_NVM_SRAM_SEMID semaphore
*  CFG_HW_THREAD_NVM_SRAM_SEMID semaphore makes sure CPU2 does not update the persistent data in SRAM2 at the same time CPU1 is reading them.
*  There is no timing constraint on how long this semaphore can be kept.
*/
#define CFG_HW_THREAD_NVM_SRAM_SEMID          9

/**
*  The CPU2 may be configured to store the BLE persistent data either in internal NVM storage on CPU2 or in
*  SRAM2 buffer provided by the user application. This can be configured with the system command SHCI_C2_Config()
*  When the CPU2 is requested to store persistent data in SRAM2, it can write data in this buffer at any time when needed.
*  In order to read consistent data with the CPU1 from the SRAM2 buffer, the flow should be:
*  + CPU1 takes CFG_HW_BLE_NVM_SRAM_SEMID semaphore
*  + CPU1 reads all persistent data from SRAM2 (most of the time, the goal is to write these data into an NVM managed by CPU1)
*  + CPU1 releases CFG_HW_BLE_NVM_SRAM_SEMID semaphore
*  CFG_HW_BLE_NVM_SRAM_SEMID semaphore makes sure CPU2 does not update the persistent data in SRAM2 at the same time CPU1 is reading them.
*  There is no timing constraint on how long this semaphore can be kept.
*/
#define CFG_HW_BLE_NVM_SRAM_SEMID             8

/**
  *  Index of the semaphore used to  access the FLASH 
  */
#define CFG_HW_FLASH_SEMID                    2



/******************************************************************************
 * HW UART
 *****************************************************************************/
#define CFG_HW_LPUART1_ENABLED                0
#define CFG_HW_LPUART1_DMA_TX_SUPPORTED       0

#define CFG_HW_USART1_ENABLED                 1
#define CFG_HW_USART1_DMA_TX_SUPPORTED        1
#define CFG_HW_USART1_DMA_RX_SUPPORTED        1
   
/**
 * LPUART1
 */
#define CFG_HW_LPUART1_PREEMPTPRIORITY         0x0F
#define CFG_HW_LPUART1_SUBPRIORITY             0

/** < The application shall check the selected source clock is enable */
#define CFG_HW_LPUART1_SOURCE_CLOCK             RCC_LPUART1CLKSOURCE_SYSCLK

#define CFG_HW_LPUART1_BAUDRATE                115200u
#define CFG_HW_LPUART1_WORDLENGTH              UART_WORDLENGTH_8B
#define CFG_HW_LPUART1_STOPBITS                UART_STOPBITS_1
#define CFG_HW_LPUART1_PARITY                  UART_PARITY_NONE
#define CFG_HW_LPUART1_HWFLOWCTL               UART_HWCONTROL_NONE
#define CFG_HW_LPUART1_MODE                    UART_MODE_TX_RX
#define CFG_HW_LPUART1_ADVFEATUREINIT          UART_ADVFEATURE_NO_INIT
#define CFG_HW_LPUART1_OVERSAMPLING            UART_OVERSAMPLING_8

#define CFG_HW_LPUART1_TX_PORT_CLK_ENABLE      __HAL_RCC_GPIOA_CLK_ENABLE
#define CFG_HW_LPUART1_TX_PORT                 GPIOA
#define CFG_HW_LPUART1_TX_PIN                  GPIO_PIN_2
#define CFG_HW_LPUART1_TX_MODE                 GPIO_MODE_AF_PP
#define CFG_HW_LPUART1_TX_PULL                 GPIO_NOPULL
#define CFG_HW_LPUART1_TX_SPEED                GPIO_SPEED_FREQ_HIGH
#define CFG_HW_LPUART1_TX_ALTERNATE            GPIO_AF8_LPUART1

#define CFG_HW_LPUART1_RX_PORT_CLK_ENABLE      __HAL_RCC_GPIOA_CLK_ENABLE
#define CFG_HW_LPUART1_RX_PORT                 GPIOA
#define CFG_HW_LPUART1_RX_PIN                  GPIO_PIN_3
#define CFG_HW_LPUART1_RX_MODE                 GPIO_MODE_AF_PP
#define CFG_HW_LPUART1_RX_PULL                 GPIO_NOPULL
#define CFG_HW_LPUART1_RX_SPEED                GPIO_SPEED_FREQ_HIGH
#define CFG_HW_LPUART1_RX_ALTERNATE            GPIO_AF8_LPUART1

#define CFG_HW_LPUART1_DMA_TX_PREEMPTPRIORITY  0x0F
#define CFG_HW_LPUART1_DMA_TX_SUBPRIORITY      0

#define CFG_HW_LPUART1_DMAMUX_CLK_ENABLE       __HAL_RCC_DMAMUX1_CLK_ENABLE
#define CFG_HW_LPUART1_DMA_CLK_ENABLE          __HAL_RCC_DMA1_CLK_ENABLE
#define CFG_HW_LPUART1_TX_DMA_REQ              DMA_REQUEST_LPUART1_TX
#define CFG_HW_LPUART1_TX_DMA_CHANNEL          DMA1_Channel1
#define CFG_HW_LPUART1_TX_DMA_IRQn             DMA1_Channel1_IRQn
#define CFG_HW_LPUART1_DMA_TX_IRQHandler       DMA1_Channel1_IRQHandler

/**
 * UART1
 */
#define CFG_HW_USART1_PREEMPTPRIORITY         0x07
#define CFG_HW_USART1_SUBPRIORITY             0

/** < The application shall check the selected source clock is enable */
#define CFG_HW_USART1_SOURCE_CLOCK            RCC_USART1CLKSOURCE_PCLK2

#define CFG_HW_USART1_BAUDRATE                115200
#define CFG_HW_USART1_WORDLENGTH              UART_WORDLENGTH_8B
#define CFG_HW_USART1_STOPBITS                UART_STOPBITS_1
#define CFG_HW_USART1_PARITY                  UART_PARITY_NONE
#define CFG_HW_USART1_HWFLOWCTL               UART_HWCONTROL_NONE
#define CFG_HW_USART1_MODE                    UART_MODE_TX_RX
#define CFG_HW_USART1_OVERSAMPLING            UART_OVERSAMPLING_8
#define CFG_HW_USART1_ONEBITSAMPLING          UART_ONE_BIT_SAMPLE_DISABLE
#define CFG_HW_USART1_PRESCALER               UART_PRESCALER_DIV1

#define CFG_HW_USART1_TX_PORT_CLK_ENABLE      __HAL_RCC_GPIOB_CLK_ENABLE
#define CFG_HW_USART1_TX_PORT                 GPIOB
#define CFG_HW_USART1_TX_PIN                  GPIO_PIN_12
#define CFG_HW_USART1_TX_MODE                 GPIO_MODE_AF_PP
#define CFG_HW_USART1_TX_PULL                 GPIO_NOPULL
#define CFG_HW_USART1_TX_SPEED                GPIO_SPEED_FREQ_HIGH
#define CFG_HW_USART1_TX_ALTERNATE            0x07

#define CFG_HW_USART1_RX_PORT_CLK_ENABLE      __HAL_RCC_GPIOA_CLK_ENABLE
#define CFG_HW_USART1_RX_PORT                 GPIOA
#define CFG_HW_USART1_RX_PIN                  GPIO_PIN_8
#define CFG_HW_USART1_RX_MODE                 GPIO_MODE_AF_PP
#define CFG_HW_USART1_RX_PULL                 GPIO_NOPULL
#define CFG_HW_USART1_RX_SPEED                GPIO_SPEED_FREQ_HIGH
#define CFG_HW_USART1_RX_ALTERNATE            0x07

#define CFG_HW_USART1_DMA_TX_PREEMPTPRIORITY  0x06
#define CFG_HW_USART1_DMA_TX_SUBPRIORITY      0

#define CFG_HW_USART1_DMA_CLK_ENABLE          __HAL_RCC_GPDMA1_CLK_ENABLE
#define CFG_HW_USART1_TX_DMA_REQ	          GPDMA1_REQUEST_USART1_TX
#define CFG_HW_USART1_TX_DMA_CHANNEL          GPDMA1_Channel0_NS
#define CFG_HW_USART1_TX_DMA_IRQn             GPDMA1_Channel0_IRQn
#define CFG_HW_USART1_DMA_TX_IRQHandler       GPDMA1_Channel0_IRQHandler
    
#define CFG_HW_USART1_DMA_RX_PREEMPTPRIORITY  0x05
#define CFG_HW_USART1_DMA_RX_SUBPRIORITY      0

#define CFG_HW_USART1_RX_DMA_REQ	          GPDMA1_REQUEST_USART1_RX
#define CFG_HW_USART1_RX_DMA_CHANNEL          GPDMA1_Channel1_NS
#define CFG_HW_USART1_RX_DMA_IRQn             GPDMA1_Channel1_IRQn
#define CFG_HW_USART1_DMA_RX_IRQHandler       GPDMA1_Channel1_IRQHandler
   
/**
* UART2
*/
#define CFG_HW_USART2_PREEMPTPRIORITY         0x07
#define CFG_HW_USART2_SUBPRIORITY             0   
   
#define CFG_HW_USART2_SOURCE_CLOCK            RCC_USART2CLKSOURCE_PCLK1
   
#define CFG_HW_USART2_BAUDRATE                115200
#define CFG_HW_USART2_WORDLENGTH              UART_WORDLENGTH_8B
#define CFG_HW_USART2_STOPBITS                UART_STOPBITS_1
#define CFG_HW_USART2_PARITY                  UART_PARITY_NONE
#define CFG_HW_USART2_HWFLOWCTL               UART_HWCONTROL_NONE
#define CFG_HW_USART2_MODE                    UART_MODE_TX_RX
#define CFG_HW_USART2_OVERSAMPLING            UART_OVERSAMPLING_8
#define CFG_HW_USART2_PRESCALER               UART_PRESCALER_DIV1  
   
#define CFG_HW_USART2_TX_PORT_CLK_ENABLE      __HAL_RCC_GPIOA_CLK_ENABLE
#define CFG_HW_USART2_TX_PORT                 GPIOA
#define CFG_HW_USART2_TX_PIN                  GPIO_PIN_12
#define CFG_HW_USART2_TX_MODE                 GPIO_MODE_AF_PP
#define CFG_HW_USART2_TX_PULL                 GPIO_NOPULL
#define CFG_HW_USART2_TX_SPEED                GPIO_SPEED_FREQ_HIGH
#define CFG_HW_USART2_TX_ALTERNATE            0x03

#define CFG_HW_USART2_RX_PORT_CLK_ENABLE      __HAL_RCC_GPIOA_CLK_ENABLE
#define CFG_HW_USART2_RX_PORT                 GPIOA
#define CFG_HW_USART2_RX_PIN                  GPIO_PIN_11
#define CFG_HW_USART2_RX_MODE                 GPIO_MODE_AF_PP
#define CFG_HW_USART2_RX_PULL                 GPIO_NOPULL
#define CFG_HW_USART2_RX_SPEED                GPIO_SPEED_FREQ_HIGH
#define CFG_HW_USART2_RX_ALTERNATE            0x03
   
#define CFG_HW_USART2_DMA_TX_PREEMPTPRIORITY  0x07
#define CFG_HW_USART2_DMA_TX_SUBPRIORITY      0

#define CFG_HW_USART2_DMA_CLK_ENABLE          __HAL_RCC_GPDMA1_CLK_ENABLE
#define CFG_HW_USART2_TX_DMA_REQ	      	  GPDMA1_REQUEST_USART2_TX
#define CFG_HW_USART2_TX_DMA_CHANNEL          GPDMA1_Channel7_NS
#define CFG_HW_USART2_TX_DMA_IRQn             GPDMA1_Channel7_IRQn
#define CFG_HW_USART2_DMA_TX_IRQHandler       GPDMA1_Channel7_IRQHandler   

#endif /*HW_CONF_H */
