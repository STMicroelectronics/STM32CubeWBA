/**
  ******************************************************************************
  * @file    gmap_alloc.h
  * @author  MCD Application Team
  * @brief   This file contains definitions used for Gaming Audio Profile
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
#ifndef __GMAP_ALLOC_H
#define __GMAP_ALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "cmsis_compiler.h"
#include "gmap.h"
#include "gmap_config.h"
#include "gmas.h"
#include "usecase_dev_mgmt.h"

/* Types ---------------------------------------------------------------------*/

typedef struct
{
  GMAP_Role_t             Role;
  UGGFeatures_t           UGGFeatures;
  UGTFeatures_t           UGTFeatures;
  BGSFeatures_t           BGSFeatures;
  BGRFeatures_t           BGRFeatures;
  GMAS_ServiceContext_t   GMASSvc;
  GMAP_CltInst_t          CltInst[USECASE_DEV_MGMT_MAX_CONNECTION]; /* Pointer on Connection Instances */
}GMAP_Context_t;

extern GMAP_Context_t GMAP_Context;

#ifdef __cplusplus
}
#endif

#endif /* __GMAP_ALLOC_H */

