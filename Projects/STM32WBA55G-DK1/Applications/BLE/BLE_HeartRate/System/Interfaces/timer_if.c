/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    timer_if.c
  * @author  MCD Application Team
  * @brief   Configure RTC Alarm, Tick and Calendar manager
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
#include <math.h>
#include "main.h" /*for Mx generated RTC_N_PREDIV_S and RTC_N_PREDIV_A*/
#include "timer_if.h"
#include "stm32wbaxx_hal_conf.h"
#include "stm32wbaxx_ll_rtc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/**
  * @brief Minimum timeout delay of Alarm in ticks
  */
#define MIN_ALARM_DELAY    3

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

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

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

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/**
  * @brief RTC handle
  */
extern RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private variables ---------------------------------------------------------*/
/**
  * @brief Indicates if the RTC is already Initialized or not
  */
static bool RTC_Initialized = 0;

/**
  * @brief RtcTimerContext
  */
static uint32_t RtcTimerContext = 0;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Exported macro ------------------------------------------------------------*/
#ifdef RTIF_DEBUG
#include "sys_app.h" /*for app_log*/
/**
  * @brief Post the RTC log string format to the circular queue for printing in using the polling mode
  */
#define TIMER_IF_DBG_PRINTF(...) do{ {UTIL_ADV_TRACE_COND_FSend(VLEVEL_ALWAYS, T_REG_OFF, TS_OFF, __VA_ARGS__);} }while(0);
#else
/**
  * @brief not used
  */
#define TIMER_IF_DBG_PRINTF(...)
#endif /* RTIF_DEBUG */

/* USER CODE BEGIN EM */

/* USER CODE END EM */

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

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

UTIL_TIMER_Status_t TIMER_IF_Init(void)
{
  UTIL_TIMER_Status_t ret = UTIL_TIMER_OK;

  /* USER CODE BEGIN TIMER_IF_Init_1 */

  /* USER CODE END TIMER_IF_Init_1 */

  if (RTC_Initialized == false)
  {
    /* Init RTC */
    MX_RTC_Init();

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

  /* USER CODE BEGIN TIMER_IF_Init_2 */

  /* USER CODE END TIMER_IF_Init_2 */

  return ret;

  /* USER CODE BEGIN TIMER_IF_Init_3 */

  /* USER CODE END TIMER_IF_Init_3 */
}

UTIL_TIMER_Status_t TIMER_IF_StartTimer(uint32_t timeout)
{
  UTIL_TIMER_Status_t ret = UTIL_TIMER_OK;

  /* USER CODE BEGIN TIMER_IF_StartTimer_1 */

  /* USER CODE END TIMER_IF_StartTimer_1 */

  RTC_AlarmTypeDef sAlarm = {0};

  /* Stop timer if one is already started */
  TIMER_IF_StopTimer();
  timeout += RtcTimerContext;

  TIMER_IF_DBG_PRINTF("Start timer: time=%d, alarm=%d\n\r",  GetTimerTicks(), timeout);

  /* Starts timer */
  sAlarm.BinaryAutoClr        = RTC_ALARMSUBSECONDBIN_AUTOCLR_NO;
  sAlarm.AlarmTime.SubSeconds = UINT32_MAX - timeout;
  sAlarm.AlarmMask            = RTC_ALARMMASK_NONE;
  sAlarm.AlarmSubSecondMask   = RTC_ALARMSUBSECONDBINMASK_NONE;
  sAlarm.Alarm                = RTC_ALARM_A;

  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* USER CODE BEGIN TIMER_IF_StartTimer_2 */

  /* USER CODE END TIMER_IF_StartTimer_2 */

  return ret;

  /* USER CODE BEGIN TIMER_IF_StartTimer_3 */

  /* USER CODE END TIMER_IF_StartTimer_3 */
}

UTIL_TIMER_Status_t TIMER_IF_StopTimer(void)
{
  UTIL_TIMER_Status_t ret = UTIL_TIMER_OK;

  /* USER CODE BEGIN TIMER_IF_StopTimer_1 */

  /* USER CODE END TIMER_IF_StopTimer_1 */

  /* Clear RTC Alarm Flag */
  __HAL_RTC_ALARM_CLEAR_FLAG(&hrtc, RTC_FLAG_ALRAF);

  /* Disable the Alarm A interrupt */
  HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);

  /* USER CODE BEGIN TIMER_IF_StopTimer_2 */

  /* USER CODE END TIMER_IF_StopTimer_2 */

  return ret;

  /* USER CODE BEGIN TIMER_IF_StopTimer_3 */

  /* USER CODE END TIMER_IF_StopTimer_3 */
}

uint32_t TIMER_IF_SetTimerContext(void)
{
  /* Store time context */
  RtcTimerContext = GetTimerTicks();

  /* USER CODE BEGIN TIMER_IF_SetTimerContext_1 */

  /* USER CODE END TIMER_IF_SetTimerContext_1 */

  TIMER_IF_DBG_PRINTF("TIMER_IF_SetTimerContext=%d\n\r", RtcTimerContext);

  /* Return time context */
  return RtcTimerContext;

  /* USER CODE BEGIN TIMER_IF_SetTimerContext_2 */

  /* USER CODE END TIMER_IF_SetTimerContext_2 */
}

uint32_t TIMER_IF_GetTimerContext(void)
{
  /* USER CODE BEGIN TIMER_IF_GetTimerContext_1 */

  /* USER CODE END TIMER_IF_GetTimerContext_1 */

  TIMER_IF_DBG_PRINTF("TIMER_IF_GetTimerContext=%d\n\r", RtcTimerContext);

  /* Return time context */
  return RtcTimerContext;

  /* USER CODE BEGIN TIMER_IF_GetTimerContext_2 */

  /* USER CODE END TIMER_IF_GetTimerContext_2 */
}

uint32_t TIMER_IF_GetTimerElapsedTime(void)
{
  /* USER CODE BEGIN TIMER_IF_GetTimerElapsedTime_1 */

  /* USER CODE END TIMER_IF_GetTimerElapsedTime_1 */

  return ((uint32_t)(GetTimerTicks() - RtcTimerContext));

  /* USER CODE BEGIN TIMER_IF_GetTimerElapsedTime_2 */

  /* USER CODE END TIMER_IF_GetTimerElapsedTime_2 */
}

uint32_t TIMER_IF_GetTimerValue(void)
{
  /* USER CODE BEGIN TIMER_IF_GetTimerValue_1 */

  /* USER CODE END TIMER_IF_GetTimerValue_1 */

  if (RTC_Initialized == true)
  {
    return GetTimerTicks();
  }
  else
  {
    return 0;
  }

  /* USER CODE BEGIN TIMER_IF_GetTimerValue_2 */

  /* USER CODE END TIMER_IF_GetTimerValue_2 */
}

uint32_t TIMER_IF_GetMinimumTimeout(void)
{
  /* USER CODE BEGIN TIMER_IF_GetMinimumTimeout_1 */

  /* USER CODE END TIMER_IF_GetMinimumTimeout_1 */

  return (MIN_ALARM_DELAY);

  /* USER CODE BEGIN TIMER_IF_GetMinimumTimeout_2 */

  /* USER CODE END TIMER_IF_GetMinimumTimeout_2 */
}

uint32_t TIMER_IF_Convert_ms2Tick(uint32_t timeMilliSec)
{
  /* USER CODE BEGIN TIMER_IF_Convert_ms2Tick_1 */

  /* USER CODE END TIMER_IF_Convert_ms2Tick_1 */

  return ((uint32_t)((((uint64_t) timeMilliSec) << RTC_N_PREDIV_S) / 1000));

  /* USER CODE BEGIN TIMER_IF_Convert_ms2Tick_2 */

  /* USER CODE END TIMER_IF_Convert_ms2Tick_2 */
}

uint32_t TIMER_IF_Convert_Tick2ms(uint32_t tick)
{
  /* USER CODE BEGIN TIMER_IF_Convert_Tick2ms_1 */

  /* USER CODE END TIMER_IF_Convert_Tick2ms_1 */

  return ((uint32_t)((((uint64_t)(tick)) * 1000) >> RTC_N_PREDIV_S));

  /* USER CODE BEGIN TIMER_IF_Convert_Tick2ms_2 */

  /* USER CODE END TIMER_IF_Convert_Tick2ms_2 */
}

void TIMER_IF_DelayMs(uint32_t delay)
{
  /* USER CODE BEGIN TIMER_IF_DelayMs_1 */

  /* USER CODE END TIMER_IF_DelayMs_1 */

  uint32_t delayTicks = TIMER_IF_Convert_ms2Tick(delay);
  uint32_t timeout = GetTimerTicks();

  /* Wait delay ms */
  while (((GetTimerTicks() - timeout)) < delayTicks)
  {
    __NOP();
  }

  /* USER CODE BEGIN TIMER_IF_DelayMs_2 */

  /* USER CODE END TIMER_IF_DelayMs_2 */
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
  /* USER CODE BEGIN HAL_RTC_AlarmAEventCallback_1 */

  /* USER CODE END HAL_RTC_AlarmAEventCallback_1 */

  UTIL_TIMER_IRQ_Handler();

  /* USER CODE BEGIN HAL_RTC_AlarmAEventCallback_2 */

  /* USER CODE END HAL_RTC_AlarmAEventCallback_2 */
}

void HAL_RTCEx_SSRUEventCallback(RTC_HandleTypeDef *hrtc)
{
  /* USER CODE BEGIN HAL_RTCEx_SSRUEventCallback_1 */

  /* USER CODE END HAL_RTCEx_SSRUEventCallback_1 */

  /* Called every 48 days with 1024 ticks per seconds */
  TIMER_IF_DBG_PRINTF(">>Handler SSRUnderflow at %d\n\r", GetTimerTicks());

  /* Increment MSBticks */
  uint32_t MSB_ticks = TIMER_IF_BkUp_Read_MSBticks();
  TIMER_IF_BkUp_Write_MSBticks(MSB_ticks + 1);

  /* USER CODE BEGIN HAL_RTCEx_SSRUEventCallback_2 */

  /* USER CODE END HAL_RTCEx_SSRUEventCallback_2 */
}

uint32_t TIMER_IF_GetTime(uint16_t *mSeconds)
{
  /* USER CODE BEGIN TIMER_IF_GetTime_1 */

  /* USER CODE END TIMER_IF_GetTime_1 */

  uint64_t ticks;
  uint32_t timerValueLsb = GetTimerTicks();
  uint32_t timerValueMSB = TIMER_IF_BkUp_Read_MSBticks();

  ticks = (((uint64_t) timerValueMSB) << 32) + timerValueLsb;

  uint32_t seconds = (uint32_t)(ticks >> RTC_N_PREDIV_S);

  /* USER CODE BEGIN TIMER_IF_GetTime_2 */

  /* USER CODE END TIMER_IF_GetTime_2 */

  ticks = (uint32_t) ticks & RTC_PREDIV_S;

  *mSeconds = TIMER_IF_Convert_Tick2ms(ticks);

  /* USER CODE BEGIN TIMER_IF_GetTime_3 */

  /* USER CODE END TIMER_IF_GetTime_3 */

  return seconds;

  /* USER CODE BEGIN TIMER_IF_GetTime_4 */

  /* USER CODE END TIMER_IF_GetTime_4 */
}

void TIMER_IF_BkUp_Write_Seconds(uint32_t Seconds)
{
  /* USER CODE BEGIN TIMER_IF_BkUp_Write_Seconds_1 */

  /* USER CODE END TIMER_IF_BkUp_Write_Seconds_1 */

  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_SECONDS, Seconds);

  /* USER CODE BEGIN TIMER_IF_BkUp_Write_Seconds_2 */

  /* USER CODE END TIMER_IF_BkUp_Write_Seconds_2 */
}

void TIMER_IF_BkUp_Write_SubSeconds(uint32_t SubSeconds)
{
  /* USER CODE BEGIN TIMER_IF_BkUp_Write_SubSeconds_1 */

  /* USER CODE END TIMER_IF_BkUp_Write_SubSeconds_1 */

  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_SUBSECONDS, SubSeconds);

  /* USER CODE BEGIN TIMER_IF_BkUp_Write_SubSeconds_2 */

  /* USER CODE END TIMER_IF_BkUp_Write_SubSeconds_2 */
}

uint32_t TIMER_IF_BkUp_Read_Seconds(void)
{
  /* USER CODE BEGIN TIMER_IF_BkUp_Read_Seconds_1 */

  /* USER CODE END TIMER_IF_BkUp_Read_Seconds_1 */

  return HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_SECONDS);

  /* USER CODE BEGIN TIMER_IF_BkUp_Read_Seconds_2 */

  /* USER CODE END TIMER_IF_BkUp_Read_Seconds_2 */
}

uint32_t TIMER_IF_BkUp_Read_SubSeconds(void)
{
  /* USER CODE BEGIN TIMER_IF_BkUp_Read_SubSeconds_1 */

  /* USER CODE END TIMER_IF_BkUp_Read_SubSeconds_1 */

  return HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_SUBSECONDS);

  /* USER CODE BEGIN TIMER_IF_BkUp_Read_SubSeconds_2 */

  /* USER CODE END TIMER_IF_BkUp_Read_SubSeconds_2 */
}

static void TIMER_IF_BkUp_Write_MSBticks(uint32_t MSBticks)
{
  /* USER CODE BEGIN TIMER_IF_BkUp_Write_MSBticks_1 */

  /* USER CODE END TIMER_IF_BkUp_Write_MSBticks_1 */

  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_MSBTICKS, MSBticks);

  /* USER CODE BEGIN TIMER_IF_BkUp_Write_MSBticks_2 */

  /* USER CODE END TIMER_IF_BkUp_Write_MSBticks_2 */
}

static uint32_t TIMER_IF_BkUp_Read_MSBticks(void)
{
  /* USER CODE BEGIN TIMER_IF_BkUp_Read_MSBticks_1 */

  /* USER CODE END TIMER_IF_BkUp_Read_MSBticks_1 */

  uint32_t MSBticks;
  MSBticks = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_MSBTICKS);
  return MSBticks;

  /* USER CODE BEGIN TIMER_IF_BkUp_Read_MSBticks_2 */

  /* USER CODE END TIMER_IF_BkUp_Read_MSBticks_2 */
}

static inline uint32_t GetTimerTicks(void)
{
  /* USER CODE BEGIN GetTimerTicks_1 */

  /* USER CODE END GetTimerTicks_1 */

  return (UINT32_MAX - LL_RTC_TIME_GetSubSecond(RTC));

  /* USER CODE BEGIN GetTimerTicks_2 */

  /* USER CODE END GetTimerTicks_2 */
}

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
