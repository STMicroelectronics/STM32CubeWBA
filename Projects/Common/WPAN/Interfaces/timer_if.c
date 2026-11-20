/**
  ******************************************************************************
  * @file    timer_if.c
  * @author  MCD Application Team
  * @brief   Configure RTC Alarm, Tick and Calendar manager
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <math.h>

#include "stm32wbaxx_hal.h"
#include "stm32wbaxx_hal_conf.h"
#include "stm32wbaxx_ll_rtc.h"

#include "timer_if.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/**
  * @brief Minimum timeout delay of Alarm in ticks
  */
#define MIN_ALARM_DELAY    1

/**
  * @brief Backup seconds register
  */
#define RTC_BKP_SECONDS    RTC_BKP_DR0

/**
  * @brief Backup subseconds register
  */
#define RTC_BKP_SUBSECONDS RTC_BKP_DR1

/**
  * @brief Backup msbticks register
  */
#define RTC_BKP_MSBTICKS   RTC_BKP_DR2

/* #define RTIF_DEBUG */

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/**
  * @brief Timer driver callbacks handler
  */
const UTIL_TIMER_Driver_s UTIL_TimerDriver =
{
  TIMER_IF_Init,
  NULL,

  TIMER_IF_StartTimer,
  TIMER_IF_StopTimer,

  TIMER_IF_SetTimerContext,
  TIMER_IF_GetTimerContext,

  TIMER_IF_GetTimerElapsedTime,
  TIMER_IF_GetTimerValue,
  TIMER_IF_GetMinimumTimeout,

  TIMER_IF_Convert_ms2Tick,
  TIMER_IF_Convert_Tick2ms,
};

/**
  * @brief SysTime driver callbacks handler
  */
const UTIL_SYSTIM_Driver_s UTIL_SYSTIMDriver =
{
  TIMER_IF_BkUp_Write_Seconds,
  TIMER_IF_BkUp_Read_Seconds,
  TIMER_IF_BkUp_Write_SubSeconds,
  TIMER_IF_BkUp_Read_SubSeconds,
  TIMER_IF_GetTime,
};

/* External variables --------------------------------------------------------*/
/**
  * @brief RTC handle
  */
extern RTC_HandleTypeDef hrtc;

/**
  * @brief Runtime RTC clock source hook.
  * @note  Can be overridden by the application to provide a measured runtime
  *        RTC clock frequency. Default implementation uses static config.
  * @retval RTC kernel clock in Hz
  */
__WEAK uint32_t TIMER_IF_GetRuntimeRtcClockHz(void)
{
  return TIMER_IF_RTCCLK_FREQ_HZ;
}

/* Private variables ---------------------------------------------------------*/
/**
  * @brief Indicates if the RTC is already Initialized or not
  */
static bool RTC_Initialized = 0;

/**
  * @brief RtcTimerContext
  */
static uint32_t RtcTimerContext = 0;

/* Exported macro ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief Get rtc timer Value in rtc tick
  * @return val the rtc timer value (upcounting)
  */
static inline uint32_t GetTimerTicks(void);

/**
  * @brief Writes MSBticks to backup register
  * Absolute RTC time in tick is (MSBticks)<<32 + (32bits binary counter)
  * @note MSBticks incremented every time the 32bits RTC timer wraps around (~44days)
  * @param[in] MSBticks
  * @return none
  */
static void TIMER_IF_BkUp_Write_MSBticks(uint32_t MSBticks);

/**
  * @brief Reads MSBticks from backup register
  * Absolute RTC time in tick is (MSBticks)<<32 + (32bits binary counter)
  * @note MSBticks incremented every time the 32bits RTC timer wraps around (~44days)
  * @retval MSBticks
  */
static uint32_t TIMER_IF_BkUp_Read_MSBticks(void);

/* Private user code ---------------------------------------------------------*/

UTIL_TIMER_Status_t TIMER_IF_Init(void)
{
  UTIL_TIMER_Status_t ret = UTIL_TIMER_OK;

  if (RTC_Initialized == false)
  {
    /* Stop Timer */
    TIMER_IF_StopTimer();

    /* DeActivate the Alarm A enabled by MX during MX_RTC_Init() */
    HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);

    /* Enable Direct Read of the calendar registers (not through Shadow) */
    HAL_RTCEx_EnableBypassShadow(&hrtc);

    /* Initialise MSB ticks */
    TIMER_IF_BkUp_Write_MSBticks(0);

    TIMER_IF_SetTimerContext();

    RTC_Initialized = true;
  }

  return ret;
}

UTIL_TIMER_Status_t TIMER_IF_StartTimer(uint32_t timeout)
{
  UTIL_TIMER_Status_t ret = UTIL_TIMER_OK;

  RTC_AlarmTypeDef sAlarm = {0};

  /* Stop timer if one is already started */
  TIMER_IF_StopTimer();
  timeout += RtcTimerContext;

  TIMER_IF_DBG_PRINTF("Start timer: time=%d, alarm=%d\n\r",  GetTimerTicks(), timeout);

  /* Starts timer */
  sAlarm.BinaryAutoClr        = RTC_ALARMSUBSECONDBIN_AUTOCLR_NO;
  sAlarm.AlarmTime.SubSeconds = UINT32_MAX - timeout;
  sAlarm.AlarmMask            = RTC_ALARMMASK_ALL;
  sAlarm.AlarmSubSecondMask   = RTC_ALARMSUBSECONDBINMASK_NONE;
  sAlarm.Alarm                = RTC_ALARM_A;

  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  return ret;
}

UTIL_TIMER_Status_t TIMER_IF_StopTimer(void)
{
  UTIL_TIMER_Status_t ret = UTIL_TIMER_OK;

  /* Clear RTC Alarm Flag */
  __HAL_RTC_ALARM_CLEAR_FLAG(&hrtc, RTC_FLAG_ALRAF);

  /* Disable the Alarm A interrupt */
  HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);

  return ret;
}

uint32_t TIMER_IF_SetTimerContext(void)
{
  /* Store time context */
  RtcTimerContext = GetTimerTicks();

  TIMER_IF_DBG_PRINTF("TIMER_IF_SetTimerContext=%d\n\r", RtcTimerContext);

  /* Return time context */
  return RtcTimerContext;
}

uint32_t TIMER_IF_GetTimerContext(void)
{
  TIMER_IF_DBG_PRINTF("TIMER_IF_GetTimerContext=%d\n\r", RtcTimerContext);

  /* Return time context */
  return RtcTimerContext;
}

uint32_t TIMER_IF_GetTimerElapsedTime(void)
{
  return ((uint32_t)(GetTimerTicks() - RtcTimerContext));
}

uint32_t TIMER_IF_GetTimerValue(void)
{
  if (RTC_Initialized == true)
  {
    return GetTimerTicks();
  }
  else
  {
    return 0;
  }
}

uint32_t TIMER_IF_GetMinimumTimeout(void)
{
  return (MIN_ALARM_DELAY);
}

uint32_t TIMER_IF_Convert_ms2Tick(uint32_t timeMilliSec)
{
  uint32_t rtcClkHz = TIMER_IF_GetRuntimeRtcClockHz();
  if (rtcClkHz == 0U)
  {
    rtcClkHz = TIMER_IF_RTCCLK_FREQ_HZ;
  }

  return (uint32_t)(((uint64_t)timeMilliSec * rtcClkHz) / (1000U * (hrtc.Init.AsynchPrediv + 1U)));
}

uint32_t TIMER_IF_Convert_Tick2ms(uint32_t tick)
{
  uint32_t rtcClkHz = TIMER_IF_GetRuntimeRtcClockHz();
  if (rtcClkHz == 0U)
  {
    rtcClkHz = TIMER_IF_RTCCLK_FREQ_HZ;
  }

  return (uint32_t)(((uint64_t)tick * 1000U * (hrtc.Init.AsynchPrediv + 1U)) / rtcClkHz);
}

void TIMER_IF_DelayMs(uint32_t delay)
{
  uint32_t delayTicks = TIMER_IF_Convert_ms2Tick(delay);
  uint32_t timeout = GetTimerTicks();

  /* Wait delay ms */
  while (((GetTimerTicks() - timeout)) < delayTicks)
  {
    __NOP();
  }
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
  UTIL_TIMER_IRQ_Handler();
}

void HAL_RTCEx_SSRUEventCallback(RTC_HandleTypeDef *hrtc)
{
  /* Called every 48 days with 1024 ticks per seconds */
  TIMER_IF_DBG_PRINTF(">>Handler SSRUnderflow at %d\n\r", GetTimerTicks());

  /* Increment MSBticks */
  uint32_t MSB_ticks = TIMER_IF_BkUp_Read_MSBticks();
  TIMER_IF_BkUp_Write_MSBticks(MSB_ticks + 1);
}

uint32_t TIMER_IF_GetTime(uint16_t *mSeconds)
{
  uint64_t ticks;
  uint32_t timerValueLsb = GetTimerTicks();
  uint32_t timerValueMSB = TIMER_IF_BkUp_Read_MSBticks();

  ticks = (((uint64_t) timerValueMSB) << 32) + timerValueLsb;

  uint32_t rtcClkHz = TIMER_IF_GetRuntimeRtcClockHz();
  if (rtcClkHz == 0U)
  {
    rtcClkHz = TIMER_IF_RTCCLK_FREQ_HZ;
  }

  uint32_t tickRate = rtcClkHz / (hrtc.Init.AsynchPrediv + 1U);
  if (tickRate == 0U)
  {
    tickRate = 1U;
  }
  uint32_t seconds  = (uint32_t)(ticks / tickRate);
  uint32_t subTicks = (uint32_t)(ticks % tickRate);

  *mSeconds = TIMER_IF_Convert_Tick2ms(subTicks);

  return seconds;
}

void TIMER_IF_BkUp_Write_Seconds(uint32_t Seconds)
{
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_SECONDS, Seconds);
}

void TIMER_IF_BkUp_Write_SubSeconds(uint32_t SubSeconds)
{
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_SUBSECONDS, SubSeconds);
}

uint32_t TIMER_IF_BkUp_Read_Seconds(void)
{
  return HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_SECONDS);
}

uint32_t TIMER_IF_BkUp_Read_SubSeconds(void)
{
  return HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_SUBSECONDS);
}

static void TIMER_IF_BkUp_Write_MSBticks(uint32_t MSBticks)
{
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_MSBTICKS, MSBticks);
}

static uint32_t TIMER_IF_BkUp_Read_MSBticks(void)
{
  uint32_t MSBticks;
  MSBticks = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_MSBTICKS);
  return MSBticks;
}

static inline uint32_t GetTimerTicks(void)
{
  uint32_t ssr = LL_RTC_TIME_GetSubSecond(RTC);
  /* read twice to make sure value it valid*/
  while (ssr != LL_RTC_TIME_GetSubSecond(RTC))
  {
    ssr = LL_RTC_TIME_GetSubSecond(RTC);
  }
  return UINT32_MAX - ssr;
}
