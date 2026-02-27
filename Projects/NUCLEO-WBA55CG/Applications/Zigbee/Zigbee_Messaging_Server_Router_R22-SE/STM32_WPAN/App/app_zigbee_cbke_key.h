/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_zigbee_cbke_key.h
  * Description        : Header that define CBKE Key using by application.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_ZIGBEE_CBKE_KEY_H
#define APP_ZIGBEE_CBKE_KEY_H

/* Includes ------------------------------------------------------------------*/

/* Private includes -----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* These CBKE Keys are valid with an Extended Address. So we perhaps need to change Extended Address of Device for tests */
extern const uint64_t   dlMyExtendedAdress;


/* Constants for CBKE Elliptic Crypto for Server */
extern const uint8_t    szZibgeeCbkeCert[];
extern const uint8_t    szZibgeeCbkeCaPublic[];
extern const uint8_t    szZibgeeCbkePrivate[];


#endif /* APP_ZIGBEE_CBKE_KEY_H */
