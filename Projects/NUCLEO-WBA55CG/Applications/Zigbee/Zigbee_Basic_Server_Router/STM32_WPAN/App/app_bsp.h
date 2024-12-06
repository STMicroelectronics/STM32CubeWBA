/**
  ******************************************************************************
  * @file    app_bsp.h
  * @author  MCD Application Team
  * @brief   Interface to manage BSP.
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
#ifndef APP_BSP_H
#define APP_BSP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#ifdef CFG_BSP_ON_DISCOVERY
#include "stm32wba55g_discovery.h"
#if (CFG_LCD_SUPPORTED == 1)
#include "stm32wba55g_discovery_lcd.h"
#include "stm32_lcd.h"
#endif /* (CFG_LCD_SUPPORTED == 1) */
#endif /* CFG_BSP_ON_DISCOVERY */
#ifdef CFG_BSP_ON_CEB
#include "b_wba5m_wpan.h"
#endif /* CFG_BSP_ON_CEB */
#ifdef CFG_BSP_ON_NUCLEO
#include "stm32wbaxx_nucleo.h"
#endif /* CFG_BSP_ON_CEB */

/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported macros ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void      APP_BSP_Init                    ( void );
void      APP_BSP_StandbyExit             ( void );
uint8_t   APP_BSP_SerialCmdExecute        ( uint8_t * pRxBuffer, uint16_t iRxBufferSize );

#if (CFG_LED_SUPPORTED == 1)
void      APP_BSP_LedInit                 ( void );

#endif /* (CFG_LED_SUPPORTED == 1) */
#if (CFG_LCD_SUPPORTED == 1)
void      APP_BSP_DisplayInit             ( void );
#endif /* (CFG_LCD_SUPPORTED == 1) */
#if ( CFG_BUTTON_SUPPORTED == 1 )
void      APP_BSP_ButtonInit              ( void );

uint8_t   APP_BSP_ButtonIsLongPressed     ( uint16_t btnIdx );
void      APP_BSP_SetButtonIsLongPressed  ( uint16_t btnIdx );
void      APP_BSP_Button1Action           ( void );
void      APP_BSP_Button2Action           ( void );
void      APP_BSP_Button3Action           ( void );

void      BSP_PB_Callback                 ( Button_TypeDef button );
#ifdef CFG_BSP_ON_DISCOVERY
void      BSP_JOY_Callback                ( JOY_TypeDef joyNb, JOYPin_TypeDef joyPin );
#endif /* CFG_BSP_ON_DISCOVERY */

#endif /* ( CFG_BUTTON_SUPPORTED == 1 )  */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*APP_BSP_H */
