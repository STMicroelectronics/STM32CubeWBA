/**
  ******************************************************************************
  * @file    hap_config.h
  * @author  MCD Application Team
  * @brief   This file contains Hearing Access Profile Configuration
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HAP_CONFIG_H
#define __HAP_CONFIG_H


/* Private defines -----------------------------------------------------------*/
#define BLE_CFG_HAP_HA_ROLE                       (1u)
#define BLE_CFG_HAP_HARC_ROLE                     (1u)
#define BLE_CFG_HAP_HAUC_ROLE                     (1u)
#define BLE_CFG_HAP_IAC_ROLE                      (1u)

#define HAP_HA_IAS_SUPPORT                        (1u)

#define BLE_DBG_HAP_ENABLE                        (1u)
#define HAP_MAX_PRESET_NUM                        (10u)
#define HAP_MAX_PRESET_NAME_LEN                   (40u)
#define MAX_HEARING_AID_PRESET_CONTROL_POINT_SIZE (HAP_MAX_PRESET_NAME_LEN + 6u)

#endif /* __HAP_CONFIG_H */