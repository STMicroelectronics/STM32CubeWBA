/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    scm.c
  * @author  MCD Application Team
  * @brief   Functions for the System Clock Manager.
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

/* Includes ------------------------------------------------------------------*/
#include "scm.h"
#include "RTDebug.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
RAMCFG_HandleTypeDef sram1_ns =
{
  RAMCFG_SRAM1,           /* Instance */
  HAL_RAMCFG_STATE_READY, /* RAMCFG State */
  0U,                     /* RAMCFG Error Code */
};

RAMCFG_HandleTypeDef sram2_ns =
{
  RAMCFG_SRAM2,           /* Instance */
  HAL_RAMCFG_STATE_READY, /* RAMCFG State */
  0U,                     /* RAMCFG Error Code */
};

static scm_system_clock_t scm_system_clock_config;
static scm_clockconfig_t scm_system_clock_requests[(scm_user_id_t)TOTAL_CLIENT_NUM] = {NO_CLOCK_CONFIG};
static scm_radio_state_t RadioState;
/* Private function prototypes -----------------------------------------------*/
static scm_clockconfig_t scm_getmaxfreq(void);
static void scm_systemclockconfig(void);
static void ConfigStartPll(void);
/* Private functions ---------------------------------------------------------*/
static scm_clockconfig_t scm_getmaxfreq(void)
{
  uint8_t idx = 0;
  scm_clockconfig_t max = NO_CLOCK_CONFIG;

  for(idx = 0; idx < sizeof(scm_system_clock_requests) ; idx++)
  {
    if(scm_system_clock_requests[idx] > max)
    {
      max = scm_system_clock_requests[idx];
    }
  }

  return max;
}

static void scm_systemclockconfig(void)
{
  SYSTEM_DEBUG_SIGNAL_SET(SCM_SYSTEM_CLOCK_CONFIG);
  switch (scm_system_clock_config.targeted_clock_freq)
  {
    case HSE_16MHZ:
    /* Currently supported only HSE32 to HSE16 */

    /* Switch from HSE_32MHz to HSE_16MHz
     *  1. Change RAM/FLASH waitstates
     *  2. Enable prescaler ==> HSE16 enabled
     *  3. AHB5 Div 2
     *  4. Voltage range 2
    **/

    /* First switch to HSE 16 */
    LL_RCC_HSE_EnablePrescaler();

    /* Need to clear HDIV5 before switching to VOS2 */
    LL_RCC_SetAHB5Divider(LL_RCC_AHB5_DIVIDER_2); /* divided by 2 */

    /* Configure flash and SRAMs before switching to VOS2 */
    scm_setwaitstates(HSE16);

    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE2);

    break;

    case HSE_32MHZ:
      /* Currently supported only HSE16 to HSE32 */

      /* Switch from HSE_16MHz to HSE_32MHz
       *  1. Voltage range 1
       *  2. Change RAM/FLASH waitstates
       *  3. AHB5 Div 1
       *  4. Disable prescaler ==> HSE32 enabled
       **/

      /* first switch to VOS1 */
      LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
      while (LL_PWR_IsActiveFlag_VOS() == 0);

      /* Switch to 32Mhz */
      LL_RCC_HSE_DisablePrescaler();

     /* Configure flash and SRAMs */
      scm_setwaitstates(HSE32);

    /* Need to set HDIV5 */
    LL_RCC_SetAHB5Divider(LL_RCC_AHB5_DIVIDER_1); /* divided by 1 */

    break;

  case SYS_PLL:
    scm_pllconfig(PLL_100MHZ);

    /* Configure flash and SRAMs */
    scm_setwaitstates(PLL);
    break;

  default:
    break;
  }
  SYSTEM_DEBUG_SIGNAL_RESET(SCM_SYSTEM_CLOCK_CONFIG);
}

/* Public functions ----------------------------------------------------------*/
void scm_init(void)
{
  /* init scm_system_clock_config with LP config
   * scm_system_clock_config SHALL BE UPDATED BY READING HW CONFIG FROM HAL APIs
   * SHALL BE CALLED AFTER SystemClock_Config()
   **/

  scm_system_clock_config.targeted_clock_freq = HSE_16MHZ;
  scm_system_clock_config.flash_ws_cfg = FLASH_LATENCY_1;
  scm_system_clock_config.sram_ws_cfg = RAMCFG_WAITSTATE_1;

  RadioState = SCM_RADIO_NOT_ACTIVE;

  __HAL_RCC_RAMCFG_CLK_ENABLE();

}

void scm_setup(void)
{
  SYSTEM_DEBUG_SIGNAL_SET(SCM_SETUP);

  /* System clock is now on HSI 16Mhz, as it exits from stop mode */

  /* Start HSE */
  LL_RCC_HSE_Enable();

  if ((LL_RCC_HSE_IsReady() != 0) && (RadioState == SCM_RADIO_ACTIVE))
  {
    /* Switch System Clock on HSE32 */
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSE);

    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSE);

    scm_setwaitstates(HSE32);

    /* As system switched to HSE, disable HSI */
    LL_RCC_HSI_Disable();

    /* Check if the clock system used PLL before low power mode entry */
    if(scm_system_clock_config.targeted_clock_freq == SYS_PLL)
    {
      /* Configure system clock to use PLL */
      ConfigStartPll();

      /* Enable PLL1RDY interrupt */
      __HAL_RCC_ENABLE_IT(RCC_IT_PLL1RDY);
    }
  }
  else
  {
    scm_setwaitstates(HSE16);

    /* Check if the system need to increase VOS range (clock frequency higher than HSE 16Mhz)*/
    if(scm_system_clock_config.targeted_clock_freq != HSE_16MHZ)
    {
      /* Set VOS to range 1 */
      LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
    }

    if (LL_RCC_HSE_IsReady() != 0)
    {
      scm_hserdy_isr();
    }
    else
    {
      /* Enable HSERDY interrupt */
      __HAL_RCC_ENABLE_IT(RCC_IT_HSERDY);
    }
  }
  SYSTEM_DEBUG_SIGNAL_RESET(SCM_SETUP);
}

void scm_pllconfig(scm_pll_config_t pll_config)
{
  (void)pll_config;
  LL_RCC_EnableIT_PLL1RDY();
}

void scm_setsystemclock(scm_user_id_t user_id, scm_clockconfig_t sysclockconfig)
{
  scm_clockconfig_t max_freq_requested;

  UTILS_ENTER_LIMITED_CRITICAL_SECTION(RCC_INTR_PRIO<<4);

  /* Register the request by updating the requested frequency for this user */
  scm_system_clock_requests[user_id] = sysclockconfig;

  /* Get the higher frequency required by the clients */
  max_freq_requested = scm_getmaxfreq();

  /* Check if we need to apply another clock frequency */
  if(scm_system_clock_config.targeted_clock_freq != max_freq_requested)
  {
    scm_system_clock_config.targeted_clock_freq = max_freq_requested;

    /* Check the current system clock source (HSI or HSE) */
    if(LL_RCC_GetSysClkSource() == LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
    {
      /* HSI is still the system clock */

      if(scm_system_clock_config.targeted_clock_freq == HSE_16MHZ)
      {
        /* The system clock target is HSE 16Mhz */

        /* Clear VOS (Range 2) */
        LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE2);
      }
      else
      {
        /* The system clock target is higher than HSE 16Mhz */

        /* Set VOS (Range 1) */
        LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);

        if(RadioState != SCM_RADIO_NOT_ACTIVE)
        {
            /* Wait until VOS has changed */
            while (LL_PWR_IsActiveFlag_VOS() == 0);

            /* Wait until HSE is ready */
            while (LL_RCC_HSE_IsReady() == 0);

            /* Switch to HSE */
            LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSE);
            while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSE);

            LL_RCC_HSE_DisablePrescaler();

            scm_setwaitstates(HSE32);

            LL_RCC_SetAHB5Divider(LL_RCC_AHB5_DIVIDER_1);

            LL_RCC_HSI_Disable();
        }
      }

      /* System clock is going to be configured in RCC HSERDY interrupt */
    }
    else
    {
      /* HSE is already the system clock source */
      /* Configure the system clock */
      scm_systemclockconfig();
    }
  }

  UTILS_EXIT_LIMITED_CRITICAL_SECTION();
}

void scm_pllconfigready(void)
{
  /* shall be defined weak here */
}

void scm_setwaitstates(const scm_ws_lp_t ws_lp_config)
{
  /* Configure flash and SRAMs */
  switch (ws_lp_config) {
  case LP:
    __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_3);
    HAL_RAMCFG_ConfigWaitState(&sram1_ns, RAMCFG_WAITSTATE_1);
    HAL_RAMCFG_ConfigWaitState(&sram2_ns, RAMCFG_WAITSTATE_1);
    break;

  case RUN:
    __HAL_FLASH_SET_LATENCY(scm_system_clock_config.flash_ws_cfg);
    HAL_RAMCFG_ConfigWaitState(&sram1_ns, scm_system_clock_config.sram_ws_cfg);
    HAL_RAMCFG_ConfigWaitState(&sram2_ns, scm_system_clock_config.sram_ws_cfg);
    break;

  case HSE16:
    __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_1);
    HAL_RAMCFG_ConfigWaitState(&sram1_ns, RAMCFG_WAITSTATE_1);
    HAL_RAMCFG_ConfigWaitState(&sram2_ns, RAMCFG_WAITSTATE_1);

    scm_system_clock_config.flash_ws_cfg = FLASH_LATENCY_1;
    scm_system_clock_config.sram_ws_cfg = RAMCFG_WAITSTATE_1;

    break;

  case HSE32:
    __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_0);
    HAL_RAMCFG_ConfigWaitState(&sram1_ns, RAMCFG_WAITSTATE_0);
    HAL_RAMCFG_ConfigWaitState(&sram2_ns, RAMCFG_WAITSTATE_0);

    scm_system_clock_config.flash_ws_cfg = FLASH_LATENCY_0;
    scm_system_clock_config.sram_ws_cfg = RAMCFG_WAITSTATE_0;

    break;

  case PLL:
    /* RAM latencies are alreadey set to 0WS */
    /* Set Flash LATENCY according to PLL configuration */
    /* BELOW CONFIGURATION IS WORST CASE, SHALL BE OPTIMIZED */
    __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_3);
    scm_system_clock_config.flash_ws_cfg = FLASH_LATENCY_3;
    break;

  default:
    break;
  }
}

void scm_hserdy_isr(void)
{
  SYSTEM_DEBUG_SIGNAL_SET(SCM_HSERDY_ISR);

  /* System is on HSI16 */

  /* Wait until VOS has changed */
  while (LL_PWR_IsActiveFlag_VOS() == 0);

  if(scm_system_clock_config.targeted_clock_freq == HSE_16MHZ)
  {
    LL_RCC_HSE_EnablePrescaler();
    /* Switch to HSE */
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSE);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSE);
  }
  else
  {
    /* The system clock target is higher than HSE 16Mhz */

    /* Switch to HSE */
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSE);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSE);

    LL_RCC_HSE_DisablePrescaler();

    scm_setwaitstates(HSE32);

    if(scm_system_clock_config.targeted_clock_freq == SYS_PLL)
    {
      /* The system clock target is based on PLL */

      /* Configure and start PLL */
      ConfigStartPll();

      /* Enable PLL1RDY interrupt */
      __HAL_RCC_ENABLE_IT(RCC_IT_PLL1RDY);
    }

    /* Set HDIV 5 */
    LL_RCC_SetAHB5Divider(LL_RCC_AHB5_DIVIDER_1); /* divided by 1 */
  }

  /* As system switched to HSE, disable HSI */
  LL_RCC_HSI_Disable();

  SYSTEM_DEBUG_SIGNAL_RESET(SCM_HSERDY_ISR);
}

void scm_pllrdy_isr(void)
{
  if(scm_system_clock_config.targeted_clock_freq == SYS_PLL)
  {
    scm_setwaitstates(PLL);

    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1R);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_PLL1R);
  }
}

void scm_notifyradiostate(const scm_radio_state_t radio_state)
{
  if(radio_state != SCM_RADIO_NOT_ACTIVE)
  {
    RadioState = SCM_RADIO_ACTIVE; /* shall be set before calling scm_setsystemclock() */
    scm_setsystemclock(SCM_USER_LL_FW, HSE_32MHZ); /* shall be set before calling scm_setsystemclock() */
  }
  else
  {
    RadioState = SCM_RADIO_NOT_ACTIVE;
    scm_setsystemclock(SCM_USER_LL_FW, HSE_16MHZ);
  }
}

static void ConfigStartPll(void)
{
  /* Configure and start the PLL */
}
