/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    pbp_app.h
  * @author  MCD Application Team
  * @brief   Header for pbp_app.c module
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PBP_APP_H
#define __PBP_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wbaxx_hal.h"
#include "cap.h"
#include "pbp_app_conf.h"
/* Private includes ----------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
#define NUM_LC3_CODEC_CONFIG    16
#define LC3_8_1                 0
#define LC3_8_2                 1
#define LC3_16_1                2
#define LC3_16_2                3
#define LC3_24_1                4
#define LC3_24_2                5
#define LC3_32_1                6
#define LC3_32_2                7
#define LC3_441_1               8
#define LC3_441_2               9
#define LC3_48_1                10
#define LC3_48_2                11
#define LC3_48_3                12
#define LC3_48_4                13
#define LC3_48_5                14
#define LC3_48_6                15

#define NUM_LC3_QOS_CONFIG      32
#define LC3_QOS_8_1_1           0
#define LC3_QOS_8_2_1           1
#define LC3_QOS_16_1_1          2
#define LC3_QOS_16_2_1          3
#define LC3_QOS_24_1_1          4
#define LC3_QOS_24_2_1          5
#define LC3_QOS_32_1_1          6
#define LC3_QOS_32_2_1          7
#define LC3_QOS_441_1_1         8
#define LC3_QOS_441_2_1         9
#define LC3_QOS_48_1_1          10
#define LC3_QOS_48_2_1          11
#define LC3_QOS_48_3_1          12
#define LC3_QOS_48_4_1          13
#define LC3_QOS_48_5_1          14
#define LC3_QOS_48_6_1          15
#define LC3_QOS_8_1_2           16
#define LC3_QOS_8_2_2           17
#define LC3_QOS_16_1_2          18
#define LC3_QOS_16_2_2          19
#define LC3_QOS_24_1_2          20
#define LC3_QOS_24_2_2          21
#define LC3_QOS_32_1_2          22
#define LC3_QOS_32_2_2          23
#define LC3_QOS_441_1_2         24
#define LC3_QOS_441_2_2         25
#define LC3_QOS_48_1_2          26
#define LC3_QOS_48_2_2          27
#define LC3_QOS_48_3_2          28
#define LC3_QOS_48_4_2          29
#define LC3_QOS_48_5_2          30
#define LC3_QOS_48_6_2          31

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  BAP_Role_t                    bap_role;
  BAP_BASE_Group_t              base_group;
  BAP_BASE_Subgroup_t           base_subgroups[1];
  BAP_BASE_BIS_t                base_bis[PBP_MAX_BIS];
  uint8_t                       codec_specific_config_bis[PBP_MAX_BIS][0x06];
  uint8_t                       codec_specific_config_subgroup[PBP_MAX_BIS][0x13];
  uint8_t                       RTN;
  uint16_t                      max_transport_latency;
  uint8_t                       subgroup_metadata[1][MAX_METADATA_LEN];
  Target_Phy_t                  phy;
  uint8_t                       encryption;
  uint32_t                      broadcast_code[4u];
  uint8_t                       audio_driver_config;
  uint16_t                      current_BIS_conn_handles[MAX_NUM_BIS_PER_BIG];
  uint8_t                       current_num_bis;
  BAP_AudioCodecController_t    AudioCodecInController;
  BAP_SupportedStandardCodec_t  aStandardCodec[1];
} PBPAPP_Context_t;

/* External variables --------------------------------------------------------*/

/* Exported macros ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ---------------------------------------------*/
/**
  * @brief Init Audio Stack
  * @retval Status of the operation
  */
tBleStatus APP_AUDIO_STACK_Init(void);
/**
  * @brief Initialize PBP Source and Start Broadcast Audio Procedure.
  */
uint8_t PBPAPP_InitSource(void);

/**
  * @brief   Start PBP Source
  */
uint8_t PBPAPP_StartSource(void);

/**
  * @brief   Stop PBP Source
  */
uint8_t PBPAPP_StopSource(void);

#ifdef __cplusplus
}
#endif

#endif /*__PBP_APP_H */

