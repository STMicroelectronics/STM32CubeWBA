/**
  ******************************************************************************
  * @file    PWR/PWR_ModesSelection/Src/system_config.c
  * @author  MCD Application Team
  * @brief   WBA25CE-compatible low power configuration backend.
  ******************************************************************************
  */

/* USER CODE BEGIN 1 */
/* Includes ------------------------------------------------------------------*/
#include "system_config.h"

/* LSI1 ready flag typically sets within a few hundred us, ample margin in ms */
#define LSI1_STARTUP_TIMEOUT (10U)

/* RTC handler declaration */
static RTC_HandleTypeDef RTCHandle;
static IWDG_HandleTypeDef hiwdg;

static System_StatusTypeDef RTC_Enable(void);
static System_StatusTypeDef RTC_Disable(void);
static System_StatusTypeDef SystemClock_32MHz(void);
static System_StatusTypeDef SystemClock_64MHz(void);
static System_StatusTypeDef IWDG_Enable(void);

void system_config(System_LowPowerModeScenarioTypeDef scenario)
{
  /* *** Common pre-setup for all modes *** */
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  if ((scenario == SYSTEM_LOWPOWER_S9) || (scenario == SYSTEM_LOWPOWER_S10) || (scenario == SYSTEM_LOWPOWER_S11))
  {
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
    RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
  }
  else if ((scenario == SYSTEM_LOWPOWER_S0) || (scenario == SYSTEM_LOWPOWER_S1))
  {
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
    RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
  }
  else
  {
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
    RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
    RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
  }
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while (1);
  }

  /* Enforcing true STOP and STANDBY modes, by disabling their emulation. */
  HAL_DBGMCU_DisableDBGStopMode();
  HAL_DBGMCU_DisableDBGStandbyMode();

  HAL_PWREx_DisableVddUSB();

  /* *** End of Common pre-setup *** */

  /* *** Specific setup for each mode *** */

  if ((scenario >= SYSTEM_LOWPOWER_S2) && (scenario <= SYSTEM_LOWPOWER_S11))
  {
    CLEAR_REG(RCC->AHB1SMENR);
    CLEAR_REG(RCC->AHB2SMENR);
    CLEAR_REG(RCC->AHB4SMENR);
    CLEAR_REG(RCC->AHB5SMENR);
    CLEAR_REG(RCC->APB1SMENR1);
    CLEAR_REG(RCC->APB1SMENR2);
    CLEAR_REG(RCC->APB2SMENR);
    CLEAR_REG(RCC->APB7SMENR);
  }
  else
  {
    /* Do nothing */
  }

  /* *** End of specific setup for each mode *** */
}

/**
  * @brief  Configure rtc state.
  * @param  scenario : Specifies system low-power mode scenario from
  *                    System_LowPowerModeScenarioTypeDef enumeration.
  * @retval System Status.
  */
System_StatusTypeDef system_rtc_config(System_LowPowerModeScenarioTypeDef scenario)
{
  System_StatusTypeDef status = SYSTEM_OK;

  switch (scenario)
  {
    /* STANDBY - with RTC but no IWDG (S9) */
    case SYSTEM_LOWPOWER_S9:
    {
      if (RTC_Enable() != SYSTEM_OK)
      {
        status = SYSTEM_ERROR;
      }
      break;
    }

    /* STANDBY - SRAM2(32K) + RTC + LSI1 + IWDG + ULPMEN=1 */
    case SYSTEM_LOWPOWER_S10:
    {
      if (RTC_Enable() != SYSTEM_OK)
      {
        status = SYSTEM_ERROR;
      }
      /* Enable IWDG for watchdog capability in STANDBY mode */
      if (status == SYSTEM_OK && IWDG_Enable() != SYSTEM_OK)
      {
        status = SYSTEM_ERROR;
      }
      break;
    }

    /* STANDBY - IWDG + LSI1 + ULPMEN=0 */
    case SYSTEM_LOWPOWER_S11:
    {
      /* Enable IWDG only, no RTC */
      if (IWDG_Enable() != SYSTEM_OK)
      {
        status = SYSTEM_ERROR;
      }
      break;
    }

    /* Disable RTC for all other scenarios */
    default:
    {
      if (RTC_Disable() != SYSTEM_OK)
      {
        status = SYSTEM_ERROR;
      }
      break;
    }
  }

  return status;
}

/**
  * @brief  Configure system clock frequency.
  * @param  scenario : Specifies system low-power mode scenario from
  *                    System_LowPowerModeScenarioTypeDef enumeration.
  *
  * @retval System Status.
  */
System_StatusTypeDef system_clock_config(System_LowPowerModeScenarioTypeDef scenario)
{
  System_StatusTypeDef status = SYSTEM_OK;

  /* Preset RTC APB access for RTC-backed standby modes. */
  if ((scenario == SYSTEM_LOWPOWER_S9) || (scenario == SYSTEM_LOWPOWER_S10))
  {
    SET_BIT(RCC->APB7ENR, RCC_APB7ENR_RTCAPBEN);
  }

  switch (scenario)
  {
    /* Run 32 MHz */
    case SYSTEM_LOWPOWER_S0:
    {
      if (SystemClock_32MHz() != SYSTEM_OK)
      {
        return SYSTEM_ERROR;
      }
      break;
    }

    /* Run 64 MHz */
    case SYSTEM_LOWPOWER_S1:
    {
      if (SystemClock_64MHz() != SYSTEM_OK)
      {
        return SYSTEM_ERROR;
      }
      break;
    }

    default:
    {
      break;
    }
  }

  return status;
}

/**
  * @brief  Configure SRAM retention.
  * @param  scenario : Specifies system low-power mode scenario from
  *                    System_LowPowerModeScenarioTypeDef enumeration.
  * @retval System Status.
  */
System_StatusTypeDef system_ram_retention_config(System_LowPowerModeScenarioTypeDef scenario)
{
  System_StatusTypeDef status = SYSTEM_OK;

  /* Enable Power Clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* Disable cache and clear flags for low power modes */
  if (scenario >= SYSTEM_LOWPOWER_S2)
  {
    HAL_ICACHE_Disable();
    HAL_ICACHE_DeInit();
    __HAL_ICACHE_CLEAR_FLAG(ICACHE_FLAG_BUSYEND | ICACHE_FLAG_ERROR);
  }

  __HAL_RCC_SRAM2_CLK_DISABLE();
  HAL_PWREx_DisableSRAM1ContentStandbyRetention();
  HAL_PWREx_DisableSRAM2ContentStandbyRetention();
  HAL_PWREx_DisableRadioSRAMClockStandbyRetention();
  HAL_PWREx_DisableRAMsContentStopRetention(PWR_SRAM1_FULL_STOP_RETENTION);
  HAL_PWREx_DisableRAMsContentStopRetention(PWR_SRAM2_FULL_STOP_RETENTION);
  HAL_PWREx_DisableRAMsContentStopRetention(PWR_USB_SRAM_STOP_RETENTION);
  HAL_PWREx_DisableRAMsContentStopRetention(PWR_PKA_SRAM_STOP_RETENTION);
  HAL_PWREx_DisableFlashFastWakeUp();
  /* Wait that default config is effective */
  __DSB();

  /* Set specific configurations */
  switch (scenario)
  {
    /* STOP0 - SRAM1(64K) + SRAM2(32K) + PKA + USB */
    case SYSTEM_LOWPOWER_S3:
    {
      __HAL_RCC_SRAM2_CLK_ENABLE();
      HAL_PWREx_EnableRAMsContentStopRetention(PWR_SRAM1_FULL_STOP_RETENTION);
      HAL_PWREx_EnableRAMsContentStopRetention(PWR_SRAM2_FULL_STOP_RETENTION);
      HAL_PWREx_EnableRAMsContentStopRetention(PWR_USB_SRAM_STOP_RETENTION);
      HAL_PWREx_EnableRAMsContentStopRetention(PWR_PKA_SRAM_STOP_RETENTION);
      break;
    }

    /* STOP0 - SRAM1(64K) */
    case SYSTEM_LOWPOWER_S4:
    {
      HAL_PWREx_EnableRAMsContentStopRetention(PWR_SRAM1_FULL_STOP_RETENTION);
      break;
    }

    /* STOP1 - SRAM1(64K) + SRAM2(32K) + PKA + USB + ULPMEN=1 */
    case SYSTEM_LOWPOWER_S5:
    {
      __HAL_RCC_SRAM2_CLK_ENABLE();
      HAL_PWREx_EnableRAMsContentStopRetention(PWR_SRAM1_FULL_STOP_RETENTION);
      HAL_PWREx_EnableRAMsContentStopRetention(PWR_SRAM2_FULL_STOP_RETENTION);
      HAL_PWREx_EnableRAMsContentStopRetention(PWR_USB_SRAM_STOP_RETENTION);
      HAL_PWREx_EnableRAMsContentStopRetention(PWR_PKA_SRAM_STOP_RETENTION);
      break;
    }

    /* STOP1 - SRAM1(64K) + ULPMEN=1 */
    case SYSTEM_LOWPOWER_S6:
    {
      HAL_PWREx_EnableRAMsContentStopRetention(PWR_SRAM1_FULL_STOP_RETENTION);
      break;
    }

    /* STANDBY - SRAM1(64K) + SRAM2(32K) + Radio + RTC + ULPMEN=1 */
    case SYSTEM_LOWPOWER_S7:
    {
      __HAL_RCC_SRAM2_CLK_ENABLE();
      __HAL_RCC_RADIO_CLK_ENABLE();
      SET_BIT(PWR->CR1, PWR_CR1_RTCRSB);
      HAL_PWREx_EnableSRAM1ContentStandbyRetention(PWR_SRAM1_FULL_STANDBY_RETENTION);
      HAL_PWREx_EnableSRAM2ContentStandbyRetention(PWR_SRAM2_FULL_STANDBY_RETENTION);
      HAL_PWREx_EnableRadioSRAMClockStandbyRetention(PWR_RADIOSRAM_FULL_STANDBY_RETENTION);
      break;
    }

    /* STANDBY - SRAM1(64K) + Radio + RTC + ULPMEN=1 */
    case SYSTEM_LOWPOWER_S8:
    {
      __HAL_RCC_RADIO_CLK_ENABLE();
      SET_BIT(PWR->CR1, PWR_CR1_RTCRSB);
      HAL_PWREx_EnableSRAM1ContentStandbyRetention(PWR_SRAM1_FULL_STANDBY_RETENTION);
      HAL_PWREx_EnableRadioSRAMClockStandbyRetention(PWR_RADIOSRAM_FULL_STANDBY_RETENTION);
      break;
    }

    /* STANDBY - SRAM2(32K) + RTC + LSI1 + ULPMEN=1 */
    case SYSTEM_LOWPOWER_S9:
    {
      __HAL_RCC_SRAM2_CLK_ENABLE();
      SET_BIT(PWR->CR1, PWR_CR1_RTCRSB);
      HAL_PWREx_EnableSRAM2ContentStandbyRetention(PWR_SRAM2_FULL_STANDBY_RETENTION);
      break;
    }

    /* STANDBY - SRAM2(32K) + RTC + LSI1 + IWDG + ULPMEN=1 */
    case SYSTEM_LOWPOWER_S10:
    {
      __HAL_RCC_SRAM2_CLK_ENABLE();
      SET_BIT(PWR->CR1, PWR_CR1_RTCRSB);
      HAL_PWREx_EnableSRAM2ContentStandbyRetention(PWR_SRAM2_FULL_STANDBY_RETENTION);
      break;
    }

    /* STANDBY - IWDG + LSI1 + ULPMEN=0 */
    case SYSTEM_LOWPOWER_S11:
    {
      /* No SRAM retention needed, only IWDG monitoring */
      break;
    }

    default:
    {
      break;
    }
  }

  /* Wait that default config is effective */
  __DSB();

  return status;
}

static System_StatusTypeDef SystemClock_64MHz(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();

  HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY);

  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_5) != HAL_OK)
  {
    return SYSTEM_ERROR;
  }

  /* Activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEDiv = RCC_HSE_DIV1;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL1.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL1.PLLFractional = 0U;
  RCC_OscInitStruct.PLL1.PLLM = 2;
  RCC_OscInitStruct.PLL1.PLLN = 8;    /* VCO = HSE/2 * 8 = 32 / 2 * 8 = 128 MHz */
  RCC_OscInitStruct.PLL1.PLLR = 2;    /* PLLSYS = 128 MHz / 2 = 64 MHz */
  RCC_OscInitStruct.PLL1.PLLP = 2;
  RCC_OscInitStruct.PLL1.PLLQ = 2;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    return SYSTEM_ERROR;
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 |
                                RCC_CLOCKTYPE_PCLK7 | RCC_CLOCKTYPE_HCLK5;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB7CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHB5_PLL1_CLKDivider = RCC_SYSCLK_PLL1_DIV2;
  RCC_ClkInitStruct.AHB5_HSEHSI_CLKDivider = RCC_SYSCLK_HSEHSI_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    return SYSTEM_ERROR;
  }

  return SYSTEM_OK;
}

static System_StatusTypeDef RTC_Enable(void)
{
  uint32_t tickstart;

  RTCHandle.Instance = RTC;

  HAL_PWR_EnableBkUpAccess();

  __HAL_RCC_LSI1_ENABLE();

  tickstart = HAL_GetTick();
  while (__HAL_RCC_GET_FLAG(RCC_FLAG_LSI1RDY) == 0U)
  {
    if ((HAL_GetTick() - tickstart) > LSI1_STARTUP_TIMEOUT)
    {
      HAL_PWR_DisableBkUpAccess();
      return SYSTEM_ERROR;
    }
  }

  __HAL_RCC_RTCAPB_CLK_ENABLE();
  __HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSI);

  RTCHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RTCHandle.Init.AsynchPrediv = 0x7FU;
  RTCHandle.Init.SynchPrediv = 0xFFU;
  RTCHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RTCHandle.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  RTCHandle.Init.OutPutPolarity = RTC_OUTPUT_DISABLE;
  RTCHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  RTCHandle.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;

  if (HAL_RTC_Init(&RTCHandle) != HAL_OK)
  {
    return SYSTEM_ERROR;
  }

  HAL_PWR_DisableBkUpAccess();
  return SYSTEM_OK;
}

static System_StatusTypeDef RTC_Disable(void)
{
  RTCHandle.Instance = RTC;

  if (HAL_RTC_GetState(&RTCHandle) != HAL_RTC_STATE_RESET)
  {
    if (HAL_RTC_DeInit(&RTCHandle) != HAL_OK)
    {
      return SYSTEM_ERROR;
    }
  }

  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_RTCAPB_CLK_DISABLE();
  __HAL_RCC_LSI1_DISABLE();
  HAL_PWR_DisableBkUpAccess();

  return SYSTEM_OK;
}

static System_StatusTypeDef IWDG_Enable(void)
{
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_256;  /* Prescaler = 256 for 30 second timeout */
  hiwdg.Init.Window = 1875;                   /* Safe refresh window (1875-3749) */
  hiwdg.Init.Reload = 3749;                   /* Reload value for ~30 seconds */
  hiwdg.Init.EWI = 0;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    return SYSTEM_ERROR;
  }

  return SYSTEM_OK;
}

/**
  * @brief  Configure system clock to 32 MHz.
  * @param  None
  * @retval System Status.
  */
static System_StatusTypeDef SystemClock_32MHz(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();

  HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY);

  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_5) != HAL_OK)
  {
    return SYSTEM_ERROR;
  }

  /* Use HSE directly as system clock at 32 MHz */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_OFF;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    return SYSTEM_ERROR;
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 |
                                RCC_CLOCKTYPE_PCLK7 | RCC_CLOCKTYPE_HCLK5;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB7CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHB5_PLL1_CLKDivider = RCC_SYSCLK_PLL1_DIV4;
  RCC_ClkInitStruct.AHB5_HSEHSI_CLKDivider = RCC_SYSCLK_HSEHSI_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    return SYSTEM_ERROR;
  }

  return SYSTEM_OK;
}

/**
  * @brief  Configure system low power mode.
  * @param  scenario : Specifies system low-power mode scenario from
  *                    System_LowPowerModeScenarioTypeDef enumeration.
  * @retval System Status.
  */
System_StatusTypeDef system_lowpower_config(System_LowPowerModeScenarioTypeDef scenario)
{
  System_StatusTypeDef status = SYSTEM_OK;

  /* Suspend Systick */
  HAL_SuspendTick();

  /* Stop and Standby entry is ignored while WUFx or SBF are set, and these are not cleared by system reset */
  __HAL_PWR_CLEAR_FLAG(PWR_WAKEUP_ALL_FLAG);
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SBF);

  /* Empty buffers before reaching the final mode */
  __DSB();

  switch (scenario)
  {
    /* SLEEP - 16 MHz */
    case SYSTEM_LOWPOWER_S2:
    {
      HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
      break;
    }

    /* STOP0 */
    case SYSTEM_LOWPOWER_S3:
    case SYSTEM_LOWPOWER_S4:
    {
      HAL_PWR_EnterSTOPMode(PWR_LOWPOWERMODE_STOP0, PWR_SLEEPENTRY_WFI);
      break;
    }

    /* STOP1 + ULPMEN=1 */
    case SYSTEM_LOWPOWER_S5:
    case SYSTEM_LOWPOWER_S6:
    {
      HAL_PWR_EnterSTOPMode(PWR_LOWPOWERMODE_STOP1, PWR_SLEEPENTRY_WFI);
      break;
    }

    /* STANDBY */
    case SYSTEM_LOWPOWER_S7:
    case SYSTEM_LOWPOWER_S8:
    case SYSTEM_LOWPOWER_S9:
    case SYSTEM_LOWPOWER_S10:
    case SYSTEM_LOWPOWER_S11:
    {
      HAL_PWR_EnterSTANDBYMode();
      break;
    }

    default:
    {
      /* Run mode */
      break;
    }
  }

  return status;
}

/**
  * @brief  Configure system regulator.
  * @param  scenario : Specifies system low-power mode scenario from
  *                    System_LowPowerModeScenarioTypeDef enumeration.
  *
  * @retval System Status.
  */
System_StatusTypeDef system_regulator_config(System_LowPowerModeScenarioTypeDef scenario)
{
  System_StatusTypeDef status = SYSTEM_OK;

  /* Enable Power Clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  switch (scenario)
  {
    /* STOP1 and STANDBY with ULPMEN=1 (S5-S10) */
    case SYSTEM_LOWPOWER_S5:
    case SYSTEM_LOWPOWER_S6:
    case SYSTEM_LOWPOWER_S7:
    case SYSTEM_LOWPOWER_S8:
    case SYSTEM_LOWPOWER_S9:
    case SYSTEM_LOWPOWER_S10:
    {
      HAL_PWREx_EnableUltraLowPowerMode();
      break;
    }

    /* STANDBY with ULPMEN=0 (S11) and RUN modes (S0-S4) */
    default:
    {
      HAL_PWREx_DisableUltraLowPowerMode();
      break;
    }
  }

  return status;
}
/* USER CODE END 1 */