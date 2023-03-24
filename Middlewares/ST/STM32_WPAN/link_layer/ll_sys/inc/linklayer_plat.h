/**
  ******************************************************************************
  * @file    hw_radio.h
  * @author  MCD Application Team
  * @brief   Header for linklayer_plat.c interface module
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

#ifndef LINKLAYER_PLAT_H
#define LINKLAYER_PLAT_H

#include <stdint.h>

extern void LINKLAYER_PLAT_ClockInit(void);
extern void LINKLAYER_PLAT_DelayUs(uint32_t delay);
extern void LINKLAYER_PLAT_Assert(uint8_t condition);
extern void LINKLAYER_PLAT_AclkCtrl(uint8_t enable);
extern void LINKLAYER_PLAT_HclkEnable(void);
extern void LINKLAYER_PLAT_HclkDisable(void);
extern void LINKLAYER_PLAT_WaitHclkRdy(void);
extern void LINKLAYER_PLAT_GetRNG(uint8_t *ptr_rnd, uint32_t len);
extern void LINKLAYER_PLAT_SetupRadioIT(void (*intr_cb)());
extern void LINKLAYER_PLAT_SetupSwLowIT(void (*intr_cb)());
extern void LINKLAYER_PLAT_TriggerSwLowIT(uint8_t priority);
extern void LINKLAYER_PLAT_EnableIRQ(void);
extern void LINKLAYER_PLAT_DisableIRQ(void);
extern void LINKLAYER_PLAT_EnableSpecificIRQ(uint8_t isr_type);
extern void LINKLAYER_PLAT_DisableSpecificIRQ(uint8_t isr_type);
extern void LINKLAYER_PLAT_EnableRadioIT(void);
extern void LINKLAYER_PLAT_DisableRadioIT(void);
extern void LINKLAYER_PLAT_StartRadioEvt(void);
extern void LINKLAYER_PLAT_StopRadioEvt(void);
extern void LINKLAYER_PLAT_RequestTemperature(void);
extern void LINKLAYER_PLAT_EnableOSContextSwitch(void);
extern void LINKLAYER_PLAT_DisableOSContextSwitch(void);

#endif /* LINKLAYER_PLAT_H */