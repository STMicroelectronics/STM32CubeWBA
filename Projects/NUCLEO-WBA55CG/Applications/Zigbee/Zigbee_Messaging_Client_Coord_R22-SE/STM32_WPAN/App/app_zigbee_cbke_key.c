/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_zigbee_cbkekey.c
  * Description        : Define CBKE Key using by application.
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

/* Includes ------------------------------------------------------------------*/
#include <assert.h>
#include <stdint.h>

/* Private includes -----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
/* To obtains these keys, you need to create an account on Certicom site (www.certicom.com) and request a 'ZigBee 1.2 Test Certificate' for each devices. */

#define APPLICATION_KEY_SUITE           ZCL_KEY_SUITE_CBKE2_ECMQV

/* These CBKE Keys are valid with an Extended Address. So we perhaps need to change Extended Address of Device for tests */
const uint64_t dlMyExtendedAdress       = 0x00u;

/* Constants for CBKE Elliptic Crypto for Client */
const uint8_t szZibgeeCbkeCert[] =      { 0x00 };

const uint8_t szZibgeeCbkeCaPublic[] =  { 0x00 };

const uint8_t szZibgeeCbkePrivate[] =   { 0x00 };

