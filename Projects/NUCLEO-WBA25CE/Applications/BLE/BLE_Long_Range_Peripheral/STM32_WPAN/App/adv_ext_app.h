/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : App/adv_ext_app.h
  * Description        : Header for adv_ext_app.c module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019-2021 STMicroelectronics.
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
#ifndef ADV_EXT_APP_H
#define ADV_EXT_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
  
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define HCI_SECONDARY_ADV_PHY_LE_CODED                  0x03U
#define HCI_PRIMARY_ADV_PHY_OPTIONS                     0x03U // requires S=2
#define HCI_SECONDARY_ADV_PHY_OPTIONS                   0x03U // requires S=2
/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ---------------------------------------------*/
uint8_t ADV_EXT_Config(void);
uint8_t ADV_EXT_Start(void);
/* USER CODE BEGIN EF */

/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /* ADV_EXT_APP_H */
