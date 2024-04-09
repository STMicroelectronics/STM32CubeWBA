/**
  ******************************************************************************
  * @file    pbp_alloc.h
  * @author  MCD Application Team
  * @brief   This file contains definitions used for Public Broadcast Profile
  *          Allocation context
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
#ifndef __PBP_ALLOC_H
#define __PBP_ALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "cmsis_compiler.h"
#include "pbp.h"
#include "pbp_config.h"

/* Types ---------------------------------------------------------------------*/

typedef struct
{
  PBP_Role_t             Role;
  PBP_PBK_Scan_State     PBKScanState;
  PBP_PBK_PA_Sync_State  PBKPASyncState;
  PBP_PBK_BIG_Sync_State PBKBIGSyncState;
  PBP_PBA_Scan_State     PBAScanState;
  PBP_PBA_PA_Sync_State  PBAPASyncState;
  PBP_PBS_State_t        PBSState;
  uint8_t                WaitForAudioDown;
}PBP_Context_t;

extern PBP_Context_t PBP_Context;

#ifdef __cplusplus
}
#endif

#endif /* __PBP_ALLOC_H */

