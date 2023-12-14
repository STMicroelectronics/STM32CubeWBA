/**
  ******************************************************************************
  * @file    stm32wba55g_discovery_bus.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the stm32wba55g_discovery_bus.c driver.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32WBA55G_DK1_BUS_H
#define STM32WBA55G_DK1_BUS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* Includes ------------------------------------------------------------------*/
#include "stm32wba55g_discovery_conf.h"

#if defined(BSP_USE_CMSIS_OS)
#include "cmsis_os.h"
#endif /* BSP_USE_CMSIS_OS */
/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32WBA55G_DK1
  * @{
  */

/** @addtogroup STM32WBA55G_DK1_BUS
  * @{
  */
/** @defgroup STM32WBA55G_DK1_BUS_Exported_Types BUS Exported Types
  * @{
  */
#if (USE_HAL_I2C_REGISTER_CALLBACKS > 0)
typedef struct
{
  pI2C_CallbackTypeDef  pMspI2cInitCb;
  pI2C_CallbackTypeDef  pMspI2cDeInitCb;
} BSP_I2C_Cb_t;
#endif /* (USE_HAL_I2C_REGISTER_CALLBACKS > 0) */

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
typedef struct
{
  pSPI_CallbackTypeDef  pMspSpiInitCb;
  pSPI_CallbackTypeDef  pMspSpiDeInitCb;
}BSP_SPI3_Cb_t;
#endif /* (USE_HAL_SPI_REGISTER_CALLBACKS == 1) */
/**
  * @}
  */
/** @defgroup STM32WBA55G_DK1_BUS_Exported_Constants BUS Exported Constants
  * @{
  */
/* Definition for I2C3 clock resources */
#define BUS_I2C3                              I2C3

#define BUS_I2C3_CLK_ENABLE()                  __HAL_RCC_I2C3_CLK_ENABLE()
#define BUS_I2C3_CLK_DISABLE()                 __HAL_RCC_I2C3_CLK_DISABLE()

#define BUS_I2C3_SCL_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()
#define BUS_I2C3_SCL_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOA_CLK_DISABLE()

#define BUS_I2C3_SDA_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()
#define BUS_I2C3_SDA_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOA_CLK_DISABLE()

#define BUS_I2C3_FORCE_RESET()                 __HAL_RCC_I2C3_FORCE_RESET()
#define BUS_I2C3_RELEASE_RESET()               __HAL_RCC_I2C3_RELEASE_RESET()

/* Definition for I2C3 Pins */
#define BUS_I2C3_SCL_PIN                       GPIO_PIN_6
#define BUS_I2C3_SCL_GPIO_PORT                 GPIOA
#define BUS_I2C3_SCL_AF                        GPIO_AF4_I2C3

#define BUS_I2C3_SDA_PIN                       GPIO_PIN_7
#define BUS_I2C3_SDA_GPIO_PORT                 GPIOA
#define BUS_I2C3_SDA_AF                        GPIO_AF4_I2C3

#ifndef BUS_I2C3_FREQUENCY
#define BUS_I2C3_FREQUENCY  100000U /* Frequency of I2C3 = 100 KHz*/
#endif /* BUS_I2C3_FREQUENCY */

/*##################### SPI3 ###################################*/
#define BUS_SPI3_INSTANCE                 SPI3
#define BUS_SPI3_CLOCK_ENABLE()           __HAL_RCC_SPI3_CLK_ENABLE()
#define BUS_SPI3_FORCE_RESET()            __HAL_RCC_SPI3_FORCE_RESET()
#define BUS_SPI3_RELEASE_RESET()          __HAL_RCC_SPI3_RELEASE_RESET()
#define BUS_SPI3_CLOCK_DISABLE()          __HAL_RCC_SPI3_CLK_DISABLE()

#define BUS_SPI3_SCK_GPIO_PORT            GPIOA
#define BUS_SPI3_SCK_PIN                  GPIO_PIN_0                 /* PA.00*/
#define BUS_SPI3_SCK_AF                   GPIO_AF6_SPI3
#define BUS_SPI3_SCK_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()
#define BUS_SPI3_SCK_GPIO_CLK_DISABLE()   __HAL_RCC_GPIOA_CLK_DISABLE()

#define BUS_SPI3_MOSI_GPIO_PORT           GPIOB
#define BUS_SPI3_MOSI_PIN                 GPIO_PIN_8                 /* PB.08 */
#define BUS_SPI3_MOSI_AF                  GPIO_AF6_SPI3
#define BUS_SPI3_MOSI_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()
#define BUS_SPI3_MOSI_GPIO_CLK_DISABLE()  __HAL_RCC_GPIOB_CLK_DISABLE()

#define BUS_SPI3_TIMEOUT                  ((uint32_t)0x2)

#ifndef BUS_SPI3_BAUDRATE
   #define BUS_SPI3_BAUDRATE  12500000    /* baud rate of SPIn = 12.5 Mbps*/
#endif

/* SPI3 is a limited instance. TSIZE for transfer is limited to 1023 Bytes */
/* Need to split LCD transfer in 2 parts for size > 1023 Bytes */
#define BUS_SPI3_TSIZE_MAX               0x3FF /* 1023 Bytes */
#define BUS_SPI3_TSIZE_DIV2              0x200 /*  512 Bytes */

/**
  * @}
  */

/** @addtogroup STM32WBA55G_DK1_BUS_Exported_Variables
  * @{
  */
extern I2C_HandleTypeDef hbus_i2c3;
extern SPI_HandleTypeDef hbus_spi3;
/**
  * @}
  */

/** @addtogroup STM32WBA55G_DK1_BUS_Exported_Functions
  * @{
  */
int32_t BSP_I2C3_Init(void);
int32_t BSP_I2C3_DeInit(void);
int32_t BSP_I2C3_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C3_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C3_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C3_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C3_IsReady(uint16_t DevAddr, uint32_t Trials);
int32_t BSP_GetTick(void);

#if (USE_HAL_I2C_REGISTER_CALLBACKS > 0)
int32_t BSP_I2C3_RegisterDefaultMspCallbacks(void);
int32_t BSP_I2C3_RegisterMspCallbacks(BSP_I2C_Cb_t *Callback);
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */
__weak HAL_StatusTypeDef MX_I2C3_Init(I2C_HandleTypeDef *hI2c, uint32_t timing);

int32_t BSP_SPI3_Init(void);
int32_t BSP_SPI3_DeInit(void);
int32_t BSP_SPI3_Send(uint8_t *pData, uint16_t Length);
int32_t BSP_SPI3_Recv(uint8_t *pData, uint16_t Length);
int32_t BSP_SPI3_SendRecv(uint8_t *pTxData, uint8_t *pRxData, uint16_t Length);

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
int32_t BSP_SPI3_RegisterDefaultMspCallbacks (void);
int32_t BSP_SPI3_RegisterMspCallbacks (BSP_SPI3_Cb_t *Callbacks);
#endif /* (USE_HAL_SPI_REGISTER_CALLBACKS == 1) */

HAL_StatusTypeDef MX_SPI3_Init(SPI_HandleTypeDef* phspi, uint32_t BaudratePrescaler);

/**
  * @}
  */

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

#endif /* STM32WBA55G_DK1_BUS_H */
