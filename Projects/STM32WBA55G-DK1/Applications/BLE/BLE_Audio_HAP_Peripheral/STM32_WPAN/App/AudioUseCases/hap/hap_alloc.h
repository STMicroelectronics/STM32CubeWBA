/**
  ******************************************************************************
  * @file    hap_alloc.h
  * @author  MCD Application Team
  * @brief   This file contains definitions used for Hearing Access Profile
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
#ifndef __HAP_ALLOC_H
#define __HAP_ALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "cmsis_compiler.h"
#include "hap.h"
#include "hap_config.h"
#include "has.h"
#include "hap_harc.h"
#include "hap_iac.h"
#include "hap_ha.h"
#include "usecase_dev_mgmt.h"

/* Types ---------------------------------------------------------------------*/

typedef struct
{
  HAP_Role_t             Role;

#if ((BLE_CFG_HAP_HARC_ROLE == 1u) && (BLE_CFG_HAP_IAC_ROLE == 1u))
  HAP_LinkupMode_t       LinkupMode;
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */

#if (BLE_CFG_HAP_HA_ROLE == 1u)
  HAP_HA_Context_t       HA;
#endif /* (BLE_CFG_HAP_HA_ROLE == 1u) */

#if (BLE_CFG_HAP_HARC_ROLE == 1u)
  HAP_HARC_Context_t     HARC;
#endif /* (BLE_CFG_HAP_HARC_ROLE == 1u) */

#if (BLE_CFG_HAP_IAC_ROLE == 1u)
  HAP_IAC_Inst_t         aIACInst[USECASE_DEV_MGMT_MAX_CONNECTION];             /* Array on IAC Instances */
#endif /* (BLE_CFG_HAP_IAC_ROLE == 1u) */
} HAP_Context_t;

extern HAP_Context_t HAP_Context;

#ifdef __cplusplus
}
#endif

#endif /* __HAP_ALLOC_H */

