/**
  ******************************************************************************
  * @file    Applications\802.15.4\MAC_802.15.4\Inc
  * @author  MCD Application Team
  * @brief   Header for app_mac_user.h module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_MAC_USER_H
#define APP_MAC_USER_H


/* Includes ------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
/* Exported types and defines ------------------------------------------------*/
/* ---------------------------------------------------------------------------*/


/* FFD MAC_802_15_4 application generic defines */
/*------------------------------------*/


/* Exported types ------------------------------------------------------------*/


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void APP_MAC_Init(void);
void APP_FFD_MAC_802_15_4_SetupTask(void);

#endif /* APP_MAC_USER_H */
