/**
  ******************************************************************************
  * @file    pbp_config.h
  * @author  MCD Application Team
  * @brief   This file contains Public Broadcast Profile Configuration
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
#ifndef __PBP_CONFIG_H
#define __PBP_CONFIG_H


/* Private defines -----------------------------------------------------------*/
#define BLE_CFG_PBP_PUBLIC_BROADCAST_SOURCE_ROLE                (1u)    /* Indicate if Public Broadcat Source Role is
                                                                         * supported
                                                                         */
#define BLE_CFG_PBP_PUBLIC_BROADCAST_SINK_ROLE                  (1u)    /* Indicate if Public Broadcast Sink role is
                                                                         * supported
                                                                         */
#define BLE_CFG_PBP_PUBLIC_BROADCAST_ASSISTANT_ROLE             (1u)    /* Indicate if Public Broadcast Assistant role
                                                                         * is supported
                                                                         */

#define BLE_PBP_MAX_ADV_METADATA_SIZE                           (20u)    /* Maximum size of the metadata field of the
                                                                         * public broadcast announcement advertising
                                                                         * data
                                                                         */
#define BLE_PBP_MAX_ADDITIONAL_ADV_DATA                         (50u)   /* Maximum size of the Additional Adv Metadata
                                                                         * Field in the PBP Broadcast Start Audio function
                                                                         */
#endif /* __PBP_CONFIG_H */