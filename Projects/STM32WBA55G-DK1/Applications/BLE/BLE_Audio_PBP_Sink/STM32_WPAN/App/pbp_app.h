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
#include "pbp_app_conf.h"
#include "cap.h"
/* Private includes ----------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef uint8_t PBPAPP_PASyncState_t;
#define PBPAPP_PA_SYNC_STATE_IDLE           (0x00)
#define PBPAPP_PA_SYNC_STATE_SYNCHRONIZING  (0x01)
#define PBPAPP_PA_SYNC_STATE_SYNCHRONIZED   (0x02)
#define PBPAPP_PA_SYNC_STATE_BASE_RECEIVED  (0x03)

typedef uint8_t PBPAPP_BIGSyncState_t;
#define PBPAPP_BIG_SYNC_STATE_IDLE          (0x00)
#define PBPAPP_BIG_SYNC_STATE_SYNCHRONIZING (0x01)
#define PBPAPP_BIG_SYNC_STATE_SYNCHRONIZED  (0x02)

typedef uint8_t PBPAPP_ScanState_t;
#define PBPAPP_SCAN_STATE_IDLE              (0x00)
#define PBPAPP_SCAN_STATE_SCANNING          (0x01)

typedef struct
{
  BAP_Role_t                    bap_role;
  BAP_BASE_Group_t              base_group;
  BAP_BASE_Subgroup_t           base_subgroups[2];
  BAP_BASE_BIS_t                base_bis[2];
  uint8_t                       codec_specific_config_bis[2][0x06];
  uint8_t                       codec_specific_config_subgroup[2][0x13];
  uint8_t                       RTN;
  uint16_t                      max_transport_latency;
  uint8_t                       subgroup_metadata[2][MAX_METADATA_LEN];
  Target_Phy_t                  phy;
  uint8_t                       encryption;
  uint32_t                      broadcast_code[4u];
  uint8_t                       audio_driver_config;
  uint16_t                      current_BIS_conn_handles[MAX_NUM_BIS_PER_BIG];
  uint8_t                       current_num_bis;
  BAP_AudioCodecController_t    AudioCodecInController;
  BAP_SupportedStandardCodec_t  aStandardCodec[1];
  PBPAPP_PASyncState_t          PASyncState;
  PBPAPP_BIGSyncState_t         BIGSyncState;
  PBPAPP_ScanState_t            ScanState;
  uint8_t                       PASyncHandle;
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
  * @brief Init PBP Sink Application
  * @retval Status of the operation
  */
uint8_t PBPAPP_InitSink(void);

/**
  * @brief Start PBP Sink
  * @retval Status of the operation
  */
uint8_t PBPAPP_StartSink(void);

/**
  * @brief Stop PBP Sink
  * @retval Status of the operation
  */
uint8_t PBPAPP_StopSink(void);

/**
  * @brief Switch to the following Broadcast Source of the list
  * @param next: 1 to switch to next source, 0 to switch to previous one
  * @retval Name of the new Broadcast Source
  */
char* PBPAPP_SwitchBrdSource(uint8_t next);

/**
  * @brief Returns the name of the current Broadcast Source
  * @retval Name of the current Broadcast Source
  */
char* PBPAPP_GetBrdSourceName(void);

#ifdef __cplusplus
}
#endif

#endif /*__PBP_APP_H */

