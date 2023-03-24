/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    scm.h
  * @author  MCD Application Team
  * @brief   Header for scm.c module
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
#ifndef SCM_H
#define SCM_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32wbaxx_hal.h"
#include "stm32wbaxx_ll_pwr.h"
#include "stm32wbaxx_ll_rcc.h"

/* Exported types ------------------------------------------------------------*/
typedef enum {
  NO_CLOCK_CONFIG = 0,
  HSE_16MHZ,
  HSE_32MHZ,
  SYS_PLL,
} scm_clockconfig_t;

typedef enum {
  LP,
  RUN,
  HSE16,
  HSE32,
  PLL,
} scm_ws_lp_t;

typedef enum {
  SCM_USER_APP,
  SCM_USER_LL_FW,
  TOTAL_CLIENT_NUM, /* To be at the end of the enum */
} scm_user_id_t;

typedef enum {
  NO_PLL,
  PLL_100MHZ,
} scm_pll_config_t;

typedef enum {
  SCM_RADIO_NOT_ACTIVE,
  SCM_RADIO_ACTIVE,
} scm_radio_state_t;

typedef struct{
  scm_clockconfig_t targeted_clock_freq;
  uint32_t flash_ws_cfg;
  uint32_t sram_ws_cfg;
} scm_system_clock_t;

/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
  * @brief  System Clock Manager init code
  * @param  None
  * @retval None
  */
void scm_init(void);

/**
  * @brief  Called at startup or out of Stop Mode.
  *         Enable the HSE at the requested frequency.
  * @param  None
  * @retval None
  */
void scm_setup(void);

/**
  * @brief  Configure the PLLs for when the HSE switch requires a change in the
  *         PLL configuration.
  * @param  scm_pll_config_t This parameter can be one of the following:
  *         @arg NO_PLL
  *         @arg PLL_100MHZ
  * @retval None
  */
void scm_pllconfig(scm_pll_config_t pll_config);

/**
  * @brief  Set the HSE clock to the requested frequency.
  * @param  scm_user_id_t This parameter can be one of the following:
  *         @arg SCM_USER_APP
  *         @arg SCM_USER_LL_FW
  * @param  sysclockconfig This parameter can be one of the following:
  *         @arg HSE_16MHZ
  *         @arg HSE_32MHZ
  *         @arg SYS_PLL
  * @retval None
  */
void scm_setsystemclock (scm_user_id_t user_id, scm_clockconfig_t sysclockconfig);

/**
  * @brief  Called each time the PLL config is ready
  * @param  None
  * @retval None
  */
void scm_pllconfigready(void);

/**
  * @brief  Configure the Flash and SRAMs wait cycle when exit/entry to low power
  * @param  ws_lp_config This parameter can be one of the following:
  *         @arg LP
  *         @arg RUN
  * @retval None
  */
void scm_setwaitstates(const scm_ws_lp_t ws_lp_config);

/**
  * @brief  Notify the state of the Radio
  * @param  scm_radio_state_t This parameter can be one of the following:
  *         @arg SCM_RADIO_ACTIVE
  *         @arg SCM_RADIO_NOT_ACTIVE
  * @retval None
  */
void scm_notifyradiostate(const scm_radio_state_t radio_state);

void scm_hserdy_isr(void);
void scm_pllrdy_isr(void);

#endif /* SCM_H */
