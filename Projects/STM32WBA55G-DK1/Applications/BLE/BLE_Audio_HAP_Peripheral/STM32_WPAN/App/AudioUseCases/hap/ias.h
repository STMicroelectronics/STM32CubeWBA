/**
  ******************************************************************************
  * @file    ias.h
  * @author  MCD Application Team
  * @brief   Header for ias.c module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2018-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */



/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IAS_H
#define __IAS_H

#ifdef __cplusplus
extern "C"
{
#endif


/* Includes ------------------------------------------------------------------*/
#include "cmsis_compiler.h"
#include "ble_core.h"
#include "svc_ctl.h"

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  IAS_NO_ALERT_EVT,
  IAS_MID_ALERT_EVT,
  IAS_HIGH_ALERT_EVT
} IAS_NotCode_t;

typedef struct
{
  IAS_NotCode_t  EvtOpcode;
  uint16_t        ConnHandle;     /* ACL Connection Handle associated to the event */
} IAS_Notification_Evt_t;


/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
tBleStatus IAS_Init(void);
void IAS_Notification(IAS_Notification_Evt_t *pNotification);
SVCCTL_EvtAckStatus_t IAS_Event_Handler(void *Event);


#ifdef __cplusplus
}
#endif

#endif /*__IAS_H */


