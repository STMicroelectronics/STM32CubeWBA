/**
  ******************************************************************************
  * @file    b_wba5m_wpan_bus.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for B_WBA5M_WPAN bus.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef B_WBA5M_WPAN_BUS_H
#define B_WBA5M_WPAN_BUS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "b_wba5m_wpan_conf.h"
#include "b_wba5m_wpan_errno.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup B_WBA5M_WPAN
  * @{
  */

/** @addtogroup B_WBA5M_WPAN_BUS
  * @{
  */

/** @defgroup B_WBA5M_WPAN_BUS_Exported_Types B_WBA5M_WPAN BUS Exported Types
  * @{
  */
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
typedef struct
{
  pI2C_CallbackTypeDef  pMspI2cInitCb;
  pI2C_CallbackTypeDef  pMspI2cDeInitCb;
} BSP_I2C_Cb_t;
#endif /* (USE_HAL_I2C_REGISTER_CALLBACKS == 1) */

/**
  * @}
  */

/** @defgroup B_WBA5M_WPAN_BUS_Exported_Constants B_WBA5M_WPAN BUS Exported Constants
  * @{
  */
#define BUS_I2C3                        I2C3
#define BUS_I2C3_CLK_ENABLE()           __HAL_RCC_I2C3_CLK_ENABLE()
#define BUS_I2C3_CLK_DISABLE()          __HAL_RCC_I2C3_CLK_DISABLE()
#define BUS_I2C3_FORCE_RESET()          __HAL_RCC_I2C3_FORCE_RESET()
#define BUS_I2C3_RELEASE_RESET()        __HAL_RCC_I2C3_RELEASE_RESET()
#define BUS_I2C3_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define BUS_I2C3_SCL_GPIO_PIN           GPIO_PIN_6
#define BUS_I2C3_SCL_GPIO_PORT          GPIOA
#define BUS_I2C3_SCL_GPIO_AF            GPIO_AF4_I2C3
#define BUS_I2C3_SDA_GPIO_PIN           GPIO_PIN_7
#define BUS_I2C3_SDA_GPIO_PORT          GPIOA
#define BUS_I2C3_SDA_GPIO_AF            GPIO_AF4_I2C3
#define BUS_I2C3_TIMEOUT                10000U

/**
  * @}
  */

/** @addtogroup B_WBA5M_WPAN_BUS_Exported_Variables
  * @{
  */
extern I2C_HandleTypeDef hbus_i2c3;

/**
  * @}
  */

/** @addtogroup B_WBA5M_WPAN_BUS_Exported_Functions
  * @{
  */
int32_t BSP_I2C3_Init(void);
int32_t BSP_I2C3_DeInit(void);
int32_t BSP_I2C3_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C3_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C3_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C3_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C3_Send(uint16_t DevAddr, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C3_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C3_IsReady(uint16_t DevAddr, uint32_t Trials);
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
int32_t BSP_I2C3_RegisterDefaultMspCallbacks(void);
int32_t BSP_I2C3_RegisterMspCallbacks(BSP_I2C_Cb_t *Callbacks);
#endif /* (USE_HAL_I2C_REGISTER_CALLBACKS == 1) */
HAL_StatusTypeDef MX_I2C3_Init(I2C_HandleTypeDef *hI2c, uint32_t timing);

int32_t BSP_GetTick(void);

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
#endif /* __cplusplus */

#endif /* B_WBA5M_WPAN_BUS_H */
