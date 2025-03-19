/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_usbx_device.h
  * @author  MCD Application Team
  * @brief   USBX Device applicative header file
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
#ifndef __APP_USBX_DEVICE_H__
#define __APP_USBX_DEVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ux_api.h"
#include "ux_device_class_hid.h"
#include "ux_device_mouse.h"
#include "ux_device_descriptors.h"
#include "ux_dcd_stm32.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "ux_api.h"
#include "ux_system.h"
#include "ux_utility.h"
#include "ux_device_stack.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
UINT MX_USBX_Device_Init(VOID);
UINT MX_USBX_Device_Stack_Init(void);
UINT MX_USBX_Device_Stack_DeInit(void);

/* USER CODE BEGIN EFP */
VOID USBX_APP_Device_Init(VOID);
VOID USBX_Device_Process(VOID *arg);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define APP_QUEUE_SIZE                      5
/* USER CODE END PD */

/* USER CODE BEGIN 1 */
typedef enum
{
  STOP_USB_DEVICE = 1,
  START_USB_DEVICE,
} USB_MODE_STATE;
/* USER CODE END 1 */

#ifdef __cplusplus
}
#endif
#endif /* __APP_USBX_DEVICE_H__ */
