/**
  ******************************************************************************
  * @file    p2p_peripheral_app.h
  * @author  MCD Application Team
  * @brief   PHeader for p2p_peripheral_app.c module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BLE_COC_APP_H
#define BLE_COC_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#define BUTTON_PRESSED                                            GPIO_PIN_RESET
/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* External variables --------------------------------------------------------*/

/* Exported macros ------------------------------------------------------------*/

/* Exported functions ---------------------------------------------*/
  void COC_PERIPH_APP_Init( void );
//  void COC_PERIPH_APP_Notification( COC_APP_ConnHandle_Not_evt_t *pNotification );

  void COC_App_KeyButton1Action( void );
  void COC_App_KeyButton2Action( void );
  void COC_App_KeyButton3Action( void );
  void Resume_DataSend(void);


#ifdef __cplusplus
}
#endif

#endif /*BLE_COC_APP_H */
