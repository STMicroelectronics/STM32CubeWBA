/**
  ******************************************************************************
  * @file    tmap_alloc.h
  * @author  MCD Application Team
  * @brief   This file contains definitions used for Telephony and Media Audio
  *          Profile Allocation context
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
#ifndef __TMAP_ALLOC_H
#define __TMAP_ALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "cmsis_compiler.h"
#include "tmap.h"
#include "tmap_config.h"
#include "tmas.h"

/* Types ---------------------------------------------------------------------*/

typedef struct
{
  TMAP_Role_t             Role;
  TMAS_ServiceContext_t   TMASSvc;
  uint8_t                 MaxNumBleLinks;         /* Maximum Number of Ble Links */
  TMAP_CltInst_t          *pConnInst;             /* Pointer on Connection Instances */
}TMAP_Context_t;

extern TMAP_Context_t TMAP_Context;

#ifdef __cplusplus
}
#endif

#endif /* __TMAP_ALLOC_H */

