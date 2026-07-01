/**
  ******************************************************************************
  * @file    adc_ctrl.c
  * @author  MCD Application Team
  * @brief   Source file for ADC controller module
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

/* Utilities */
#include "utilities_common.h"

/* Real time trace for debug */
#include "RTDebug.h"

/* Own header files */
#include "adc_ctrl.h"
#include "adc_ctrl_conf.h"

/* LL ADC header */
#include "stm32wbaxx_ll_adc.h"

/* Private defines -----------------------------------------------------------*/
/**
 * @brief Initial value define for configuration tracking number
 */
#define ADCCTRL_NO_CONFIG   (uint32_t)(0x00000000u)
#define ADCCTRL_MAX_USER_RANK_NB  (uint32_t)(7u)
#define ADCCTRL_MAX_TOTAL_RANK_NB (uint32_t)(8u)

/**
 * @brief Init variable out of expected ADC conversion data range
 */
#define VAR_CONVERTED_DATA_INIT_VALUE_12B  (__LL_ADC_DIGITAL_SCALE(LL_ADC_RESOLUTION_12B) + 1)
#define VAR_CONVERTED_DATA_INIT_VALUE_10B  (__LL_ADC_DIGITAL_SCALE(LL_ADC_RESOLUTION_10B) + 1)
#define VAR_CONVERTED_DATA_INIT_VALUE_8B  (__LL_ADC_DIGITAL_SCALE(LL_ADC_RESOLUTION_8B) + 1)
#define VAR_CONVERTED_DATA_INIT_VALUE_6B  (__LL_ADC_DIGITAL_SCALE(LL_ADC_RESOLUTION_6B) + 1)

/**
 * @brief Internal temperature sensor, parameter V30 (unit: mV).
 *
 * @details Refer to device datasheet for min/typ/max values.
 *
 */
#define TEMPSENSOR_TYP_CAL1_V          (( int32_t)  760)

/**
 * @brief Internal temperature sensor, parameter Avg_Slope (unit: uV/DegCelsius).
 *
 * @details Refer to device datasheet for min/typ/max values.
 *
 */
#define TEMPSENSOR_TYP_AVGSLOPE        (( int32_t) 2500)

/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/**
 * @brief  ADC IP Client list
 */
static uint32_t ClientList;

/**
 * @brief Tracker of the current applied configuration
 */
static uint32_t CurrentConfig = ADCCTRL_NO_CONFIG;

/**
 * @brief Tracker of the current configured rank count
 */
static uint32_t CurrentConfigRankCount = ADCCTRL_NO_CONFIG;

/**
 * @brief Higher registered handle ID
 */
static uint32_t MaxRegisteredId = ADCCTRL_NO_CONFIG;

/**
 * @brief Handle of the ADC
 */
static ADC_TypeDef * p_ADCHandle = ADCCTRL_HWADDR;

/* Global variables ----------------------------------------------------------*/
/* Error Handler */
extern void Error_Handler(void);

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  Activate ADC.
  *         Activation order is as follow:
  *         - Enable ADC peripharal clock
  *         - Enable ADC internal voltage regulator
  *         - Enable ADC
  * @param  None
  * @retval None
  */
static inline void AdcActivate (void);

/**
  * @brief  Deactivate ADC IP.
  *         Deactivation order is as follow:
  *         - Disable ADC
  *         - Disable ADC internal voltage regulator
  *         - Disable ADC peripharal clock
  * @param  None
  * @retval None
  */
static inline void AdcDeactivate (void);

/**
  * @brief  Configure ADC IP.
  * @param  p_Handle: Handle to work with
  * @retval State of the configuration
  */
static inline ADCCTRL_Cmd_Status_t AdcConfigure (const ADCCTRL_Handle_t * const p_Handle,
                                                 const uint32_t NumberOfRanks);

/**
 * @brief Read the raw value
 * @param  p_Handle: Handle to work with
 * @return Raw value
 */
static inline uint16_t AdcReadRaw (const uint32_t Resolution);

/**
 * @brief Return rank order value from LL rank define
 * @param  Rank: LL rank define
 * @return Rank order in range [1..7], 0 if invalid
 */
static inline uint32_t AdcRankToOrder (const uint32_t Rank);

/**
 * @brief Return LL rank define from rank order value
 * @param  RankOrder: Rank order in range [1..8]
 * @return LL rank define, 0 if invalid
 */
static inline uint32_t AdcOrderToRank (const uint32_t RankOrder);

/**
 * @brief Return rank count from LL sequencer length define
 * @param  SeqLength: LL sequencer length define
 * @return Rank count in range [1..7], 0 if invalid
 */
static inline uint32_t AdcSeqLengthToCount (const uint32_t SeqLength);

/**
 * @brief Return LL sequencer length define from rank count
 * @param  RankCount: Sequencer rank count
 * @return LL sequencer length define, 0 if invalid
 */
static inline uint32_t AdcCountToSeqLength (const uint32_t RankCount);

/**
  * @brief  Perform ADC group regular conversion start, poll for conversion
  *         completion.
  *         (ADCCTRL_HWADDR instance: ADC).
  * @note   This function does not perform ADC group regular conversion stop:
  *         intended to be used with ADC in single mode, trigger SW start
  *         (only 1 ADC conversion done at each trigger, no conversion stop
  *         needed).
  *         In case of continuous mode or conversion trigger set to
  *         external trigger, ADC group regular conversion stop must be added.
  * @param  None
  * @retval None
  */
static inline void ConversionStartPoll_ADC_GrpRegular (void);

/* Functions Definition ------------------------------------------------------*/
__WEAK ADCCTRL_Cmd_Status_t ADCCTRL_Init (void)
{
  ADCCTRL_Cmd_Status_t error = ADCCTRL_UNKNOWN;

  /* Try to take the ADC mutex */
  error = ADCCTRL_MutexTake ();

  if (ADCCTRL_OK == error)
  {
    CurrentConfig = ADCCTRL_NO_CONFIG;
    CurrentConfigRankCount = ADCCTRL_NO_CONFIG;

    p_ADCHandle = ADCCTRL_HWADDR;

    /* Reset ADC Client list */
    ClientList = 0x00u;
    
    /* Deactivate the ADC */
    AdcDeactivate();

    /* Release the mutex */
    ADCCTRL_MutexRelease ();
  }

  return error;
}

__WEAK ADCCTRL_Cmd_Status_t ADCCTRL_RegisterHandle (ADCCTRL_Handle_t * const p_Handle)
{
  ADCCTRL_Cmd_Status_t error = ADCCTRL_UNKNOWN;

  if (NULL == p_Handle)
  {
    error = ADCCTRL_ERROR_NULL_POINTER;
  }
  else if (ADCCTRL_HANDLE_REG == p_Handle->State)
  {
    error = ADCCTRL_HANDLE_ALREADY_REGISTERED;
  }
  else
  {
    /* Try to take the ADC mutex */
    error = ADCCTRL_MutexTake ();

    if (ADCCTRL_OK == error)
    {
      /* Update the maximum registered handle */
      MaxRegisteredId = MaxRegisteredId + 1u;
      /* Update the handle UUID */
      p_Handle->Uid = MaxRegisteredId;
      /* Set handle as initialized */
      p_Handle->State = ADCCTRL_HANDLE_REG;

      /* Release the mutex */
      ADCCTRL_MutexRelease ();
    }
  }

  return error;
}

__WEAK ADCCTRL_Cmd_Status_t ADCCTRL_RequestIpState (const ADCCTRL_Handle_t * const p_Handle,
                                                    const ADCCTRL_Ip_State_t State)
{
  ADCCTRL_Cmd_Status_t error = ADCCTRL_UNKNOWN;

  UTILS_ENTER_CRITICAL_SECTION();

  if (ADC_OFF == State)
  {
    ClientList &= (~(1U << p_Handle->Uid));

    error = ADCCTRL_OK;
  }
  else if (ADC_ON == State)
  {
    ClientList |= (1U << p_Handle->Uid);

    error = ADCCTRL_OK;
  }
  else
  {
    error = ADCCTRL_ERROR_STATE;
  }

  if (0x00u == ClientList)
  {
    /* Disable ADC as there is no request anymore */
    AdcDeactivate();
  }
  else
  {
    /* Enable ADC as there at least one request */
    AdcActivate();
  }

  UTILS_EXIT_CRITICAL_SECTION();

  return error;
}

__WEAK ADCCTRL_Cmd_Status_t ADCCTRL_RequestValues (const ADCCTRL_Handle_t * const p_Handle,
                                                   int16_t * const p_ReadValues)
{
  ADCCTRL_Cmd_Status_t error = ADCCTRL_UNKNOWN;
  uint16_t rawValues[ADCCTRL_MAX_TOTAL_RANK_NB] = {0};
  uint16_t vddValue = 0x00;
  uint32_t maxRank = AdcRankToOrder(LL_ADC_REG_RANK_7);
  uint32_t numberOfUserRanks = 0u;
  uint32_t rankIdx = 0u;

  SYSTEM_DEBUG_SIGNAL_SET(ADC_TEMPERATURE_ACQUISITION);

  if ((NULL == p_Handle) || (NULL == p_ReadValues))
  {
    error = ADCCTRL_ERROR_NULL_POINTER;
  }
  else if (ADCCTRL_HANDLE_NOT_REG == p_Handle->State)
  {
    error = ADCCTRL_HANDLE_NOT_REGISTERED;
  }
  else if ((MaxRegisteredId < p_Handle->Uid) ||
           (ADCCTRL_NO_CONFIG >= p_Handle->Uid))
  {
    error = ADCCTRL_HANDLE_NOT_VALID;
  }
  else if (0x00u == (ClientList & (1U << p_Handle->Uid)))
  {
    error = ADCCTRL_ERROR_STATE;
  }
  else if (LL_ADC_REG_SEQ_DISCONT_1RANK != p_Handle->InitConf.SeqParams.DiscMode)
  {
    error = ADCCTRL_ERROR_CONFIG;
  }
  else
  {
    numberOfUserRanks = AdcSeqLengthToCount(p_Handle->InitConf.SeqParams.Length);

    if ((0u == numberOfUserRanks) || (numberOfUserRanks > maxRank))
    {
      error = ADCCTRL_ERROR_CONFIG;
    }
    else
    {
      for (rankIdx = 0u; 
           ((rankIdx < numberOfUserRanks) && (ADCCTRL_UNKNOWN == error)); 
           rankIdx++)
      {
        if (AdcRankToOrder(p_Handle->ChannelConf[rankIdx].Rank) != (rankIdx + 1u))
        {
          error = ADCCTRL_ERROR_CONFIG;
        }
        else
        {
          /* Do nothing */
        }
      }
    }

    if (ADCCTRL_UNKNOWN == error)
    {
      error = ADCCTRL_MutexTake ();
    }

    if (ADCCTRL_OK == error)
    {
      if ((CurrentConfig != p_Handle->Uid) ||
          (CurrentConfigRankCount != numberOfUserRanks))
      {
        error = AdcConfigure (p_Handle,
                              numberOfUserRanks);

        if (ADCCTRL_OK == error)
        {
          LL_ADC_Enable(p_ADCHandle);
        }
      }

      if (ADCCTRL_OK == error)
      { 
        /* Read the raw ADC values for all ranks */
        for (rankIdx = 0u; 
             rankIdx < (CurrentConfigRankCount + 1u); 
             rankIdx++)
        {
          rawValues[rankIdx] = AdcReadRaw (p_Handle->InitConf.ConvParams.Resolution);
        }

        vddValue = __LL_ADC_CALC_VREFANALOG_VOLTAGE (rawValues[CurrentConfigRankCount],           
                                                     p_Handle->InitConf.ConvParams.Resolution);
        
        for (rankIdx = 0u; 
             rankIdx < numberOfUserRanks; 
             rankIdx++)
        {
          /* Compute VREFInt value */
          if (LL_ADC_CHANNEL_VREFINT == p_Handle->ChannelConf[rankIdx].Channel)
          {
            p_ReadValues[rankIdx] = __LL_ADC_CALC_DATA_TO_VOLTAGE(vddValue, 
                                                                  rawValues[CurrentConfigRankCount], 
                                                                  p_Handle->InitConf.ConvParams.Resolution);
          }
          /* Compute VCore value */
          else if (LL_ADC_CHANNEL_VCORE == p_Handle->ChannelConf[rankIdx].Channel)
          {
            p_ReadValues[rankIdx] = __LL_ADC_CALC_DATA_TO_VOLTAGE(vddValue, 
                                                                  rawValues[rankIdx], 
                                                                  p_Handle->InitConf.ConvParams.Resolution);
          }
          /* Compute Temperature value */
          else if (LL_ADC_CHANNEL_TEMPSENSOR == p_Handle->ChannelConf[rankIdx].Channel)
          {
            if(*TEMPSENSOR_CAL1_ADDR == *TEMPSENSOR_CAL2_ADDR)
            {
              p_ReadValues[rankIdx] = __LL_ADC_CALC_TEMPERATURE_TYP_PARAMS (TEMPSENSOR_TYP_AVGSLOPE,
                                                                            TEMPSENSOR_TYP_CAL1_V,
                                                                            TEMPSENSOR_CAL1_TEMP,
                                                                            vddValue,
                                                                            rawValues[rankIdx],
                                                                            p_Handle->InitConf.ConvParams.Resolution);
            }
            else
            {
              p_ReadValues[rankIdx] = __LL_ADC_CALC_TEMPERATURE (vddValue,
                                                                 rawValues[rankIdx],
                                                                 p_Handle->InitConf.ConvParams.Resolution);
            }
          }
          /* Return raw channel values */
          else
          {
            p_ReadValues[rankIdx] = rawValues[rankIdx];
          }
        }
      }
      else
      {
        error = ADCCTRL_ERROR_CONFIG;
      }

      ADCCTRL_MutexRelease ();
    }
  }

  SYSTEM_DEBUG_SIGNAL_RESET(ADC_TEMPERATURE_ACQUISITION);

  return error;
}

/* Private function Definition -----------------------------------------------*/
void AdcActivate (void)
{
  __IO uint32_t backup_setting_adc_dma_transfer = 0U;

  SYSTEM_DEBUG_SIGNAL_SET(ADC_ACTIVATION);

  UTILS_ENTER_CRITICAL_SECTION();

  /*## Operation on ADC hierarchical scope: ADC instance #####################*/

  /* Note: Hardware constraint (refer to description of the functions         */
  /*       below):                                                            */
  /*       On this STM32 series, setting of these features is conditioned to  */
  /*       ADC state:                                                         */
  /*       ADC must be disabled.                                              */
  /* Note: In this example, all these checks are not necessary but are        */
  /*       implemented anyway to show the best practice usages                */
  /*       corresponding to reference manual procedure.                       */
  /*       Software can be optimized by removing some of these checks, if     */
  /*       they are not relevant considering previous settings and actions    */
  /*       in user application.                                               */
  if (LL_ADC_IsEnabled(p_ADCHandle) == 0)
  {
    /* Select clock source */
    ADCTCTRL_SET_CLOCK_SOURCE ();

    /* Peripheral clock enable */
    ADCCTRL_ENABLE_CLOCK ();

    /* Enable ADC internal voltage regulator */
    LL_ADC_EnableInternalRegulator(p_ADCHandle);

    /* Waiting for ADC internal voltage regulator stabilization. */
    while(LL_ADC_IsInternalRegulatorEnabled(p_ADCHandle) == 0);

    /* Disable ADC DMA transfer request during calibration */
    /* Note: Specificity of this STM32 series: Calibration factor is          */
    /*       available in data register and also transferred by DMA.          */
    /*       To not insert ADC calibration factor among ADC conversion data   */
    /*       in DMA destination address, DMA transfer must be disabled during */
    /*       calibration.                                                     */
    backup_setting_adc_dma_transfer = LL_ADC_REG_GetDMATransfer(p_ADCHandle);
    LL_ADC_REG_SetDMATransfer(p_ADCHandle, LL_ADC_REG_DMA_TRANSFER_NONE);

    /* Run ADC self calibration */
    LL_ADC_StartCalibration(p_ADCHandle);

    /* Poll for ADC effectively calibrated */
    while (LL_ADC_IsCalibrationOnGoing(p_ADCHandle) != 0);

    /* Restore ADC DMA transfer request after calibration */
    LL_ADC_REG_SetDMATransfer(p_ADCHandle, backup_setting_adc_dma_transfer);

    /* Delay required between ADC end of calibration and ADC enable */
    /* LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES --> 2 cycles */
    __asm("mov r0, r0");
    __asm("mov r0, r0");

    /* Enable ADC */
    LL_ADC_Enable(p_ADCHandle);

    /* Poll for ADC ready to convert */
    while (LL_ADC_IsActiveFlag_ADRDY(p_ADCHandle) == 0);

    /* Note: ADC flag ADRDY is not cleared here to be able to check ADC       */
    /*       status afterwards.                                               */
    /*       This flag should be cleared at ADC Deactivation, before a new    */
    /*       ADC activation, using function "LL_ADC_ClearFlag_ADRDY()".       */
  }

  /*## Operation on ADC hierarchical scope: ADC group regular ################*/
  /* Note: No operation on ADC group regular performed here.                  */
  /*       ADC group regular conversions to be performed after this function  */
  /*       using function:                                                    */
  /*       "LL_ADC_REG_StartConversion();"                                    */

  /*## Operation on ADC hierarchical scope: ADC group injected ###############*/
  /* Note: Feature not available on this STM32 series */

  UTILS_EXIT_CRITICAL_SECTION();

  SYSTEM_DEBUG_SIGNAL_RESET(ADC_ACTIVATION);
}

void AdcDeactivate (void)
{
  SYSTEM_DEBUG_SIGNAL_SET(ADC_DEACTIVATION);

  UTILS_ENTER_CRITICAL_SECTION();

  if(LL_ADC_IsEnabled(p_ADCHandle))
  {
    /* Disable ADC */
    LL_ADC_Disable(p_ADCHandle);

    /* Clear flag ADC ready */
    LL_ADC_ClearFlag_ADRDY(p_ADCHandle);

    /* Wait until ADC_CR_ADEN bit is reset before turning off ADC internal regulator */
    while(LL_ADC_IsEnabled(p_ADCHandle) == 1UL);

    /* Wait until ADC_CR_ADSTP bit is reset before turning off ADC internal regulator */
    while(LL_ADC_REG_IsStopConversionOngoing(p_ADCHandle) == 1U);

    /* Disable ADC internal voltage regulator */
    LL_ADC_DisableInternalRegulator(p_ADCHandle);
    while(LL_ADC_IsInternalRegulatorEnabled(p_ADCHandle) == 1U);

    /* Peripharal clock disable */
    ADCCTRL_DISABLE_CLOCK ();
  }

  UTILS_EXIT_CRITICAL_SECTION();

  SYSTEM_DEBUG_SIGNAL_RESET(ADC_DEACTIVATION);
}

ADCCTRL_Cmd_Status_t AdcConfigure (const ADCCTRL_Handle_t * const p_Handle,
                                   const uint32_t NumberOfRanks)
{
  ADCCTRL_Cmd_Status_t error = ADCCTRL_OK;
  
  __IO uint32_t backup_setting_adc_dma_transfer = 0U;

  LL_ADC_InitTypeDef ADC_InitStruct = {0};
  LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};
  uint32_t commonPath = LL_ADC_PATH_INTERNAL_NONE;
  uint32_t totalRankNumber = NumberOfRanks + 1u;
  uint32_t totalSeqLength = 0u;
  uint32_t rankIdx = 0u;

  /* DeInit the ADC module */
  if (SUCCESS != LL_ADC_DeInit (p_ADCHandle))
  {
    error = ADCCTRL_NOK;
  }
  else
  {
    /* Restart a calibration */
    /* Disable ADC DMA transfer request during calibration */
    /* Note: Specificity of this STM32 series: Calibration factor is          */
    /*       available in data register and also transferred by DMA.          */
    /*       To not insert ADC calibration factor among ADC conversion data   */
    /*       in DMA destination address, DMA transfer must be disabled during */
    /*       calibration.                                                     */
    backup_setting_adc_dma_transfer = LL_ADC_REG_GetDMATransfer(p_ADCHandle);
    LL_ADC_REG_SetDMATransfer(p_ADCHandle, LL_ADC_REG_DMA_TRANSFER_NONE);

    /* Run ADC self calibration */
    LL_ADC_StartCalibration(p_ADCHandle);

    /* Poll for ADC effectively calibrated */
    while (LL_ADC_IsCalibrationOnGoing(p_ADCHandle) != 0);

    /* Restore ADC DMA transfer request after calibration */
    LL_ADC_REG_SetDMATransfer(p_ADCHandle, backup_setting_adc_dma_transfer);

    /* Delay required between ADC end of calibration and ADC enable */
    /* LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES --> 2 cycles */
    __asm("mov r0, r0");
    __asm("mov r0, r0");
  }

  /* All OK ? */
  if (ADCCTRL_OK == error)
  {
    totalSeqLength = AdcCountToSeqLength(totalRankNumber);

    if (0u == totalSeqLength)
    {
      error = ADCCTRL_NOK;
    }
  }

  if (ADCCTRL_OK == error)
  {
    /* Update init and configuration parameters with requested values */
    ADC_InitStruct.Resolution = p_Handle->InitConf.ConvParams.Resolution;
    ADC_InitStruct.DataAlignment = p_Handle->InitConf.ConvParams.DataAlign;
    
    ADC_REG_InitStruct.TriggerSource = p_Handle->InitConf.ConvParams.TriggerStart;
    ADC_REG_InitStruct.SequencerLength = totalSeqLength;
    ADC_REG_InitStruct.SequencerDiscont = p_Handle->InitConf.SeqParams.DiscMode;
    ADC_REG_InitStruct.ContinuousMode = p_Handle->InitConf.ConvParams.ConversionMode;
    ADC_REG_InitStruct.DMATransfer = p_Handle->InitConf.ConvParams.DmaTransfer;
    ADC_REG_InitStruct.Overrun = p_Handle->InitConf.ConvParams.Overrun;
    
    for (rankIdx = 0u; 
         rankIdx < NumberOfRanks; 
         rankIdx++)
    {
      if (LL_ADC_CHANNEL_TEMPSENSOR == p_Handle->ChannelConf[rankIdx].Channel)
      {
        commonPath |= LL_ADC_PATH_INTERNAL_TEMPSENSOR;
      }
      else
      {
        /* Do nothing */
      }
    }

    /* VREFINT is always enabled as it is used as reference for better computation */
    commonPath |= LL_ADC_PATH_INTERNAL_VREFINT;

    LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(p_ADCHandle),
                                   commonPath);
    
    /* Set trigger frequency */
    LL_ADC_SetTriggerFrequencyMode(p_ADCHandle, 
                                   p_Handle->InitConf.ConvParams.TriggerFrequencyMode);
    
    /* Apply the requested ADC configuration - Init part */
    if (SUCCESS != LL_ADC_Init(p_ADCHandle, 
                               &ADC_InitStruct))
    {
      error = ADCCTRL_NOK;
    }
    else
    {      
      /* Set Sequencer if configurable */
      LL_ADC_REG_SetSequencerConfigurable(p_ADCHandle, 
                                          p_Handle->InitConf.SeqParams.Setup);
      
      /* Apply the requested ADC configuration - Register init part */
      if (SUCCESS != LL_ADC_REG_Init(p_ADCHandle, 
                                     &ADC_REG_InitStruct))
      {      
        error = ADCCTRL_NOK;
      }
      else 
      {
        /* Set Low power characteristics */
        LL_ADC_SetLPModeAutoPowerOff(p_ADCHandle, 
                                     p_Handle->InitConf.LowPowerParams.AutoPowerOff);
        LL_ADC_SetLPModeAutonomousDPD(p_ADCHandle, 
                                      p_Handle->InitConf.LowPowerParams.AutonomousDPD);

        /* Set Sampling time for channels */
        LL_ADC_SetSamplingTimeCommonChannels(p_ADCHandle, 
                                             LL_ADC_SAMPLINGTIME_COMMON_1, 
                                             p_Handle->InitConf.ConvParams.SamplingTimeCommon1);
        LL_ADC_SetSamplingTimeCommonChannels(p_ADCHandle, 
                                             LL_ADC_SAMPLINGTIME_COMMON_2, 
                                             p_Handle->InitConf.ConvParams.SamplingTimeCommon2);
        
        for (rankIdx = 0u; rankIdx < NumberOfRanks; rankIdx++)
        {
          if (LL_ADC_CHANNEL_VREFINT != p_Handle->ChannelConf[rankIdx].Channel)
          {
            LL_ADC_REG_SetSequencerRanks(p_ADCHandle,
                                         p_Handle->ChannelConf[rankIdx].Rank,
                                         p_Handle->ChannelConf[rankIdx].Channel);
            LL_ADC_SetChannelSamplingTime(p_ADCHandle,
                                          p_Handle->ChannelConf[rankIdx].Channel,
                                          p_Handle->ChannelConf[rankIdx].SamplingTime);
          }
          else 
          {
            /* Do nothing */
          }
        }
        
        /* Configure the VREF channel */
        LL_ADC_REG_SetSequencerRanks(p_ADCHandle,
                                     AdcOrderToRank(NumberOfRanks + 1u),
                                     LL_ADC_CHANNEL_VREFINT);
        LL_ADC_SetChannelSamplingTime(p_ADCHandle,
                                      LL_ADC_CHANNEL_VREFINT,
                                      LL_ADC_SAMPLINGTIME_COMMON_1);
        
        /* Update the current configuration */
        if (ADCCTRL_OK == error)
        {
          CurrentConfig = p_Handle->Uid;
          CurrentConfigRankCount = NumberOfRanks;
        }
      }
    }
  }

  return error;
}

uint16_t AdcReadRaw (const uint32_t Resolution)
{
  /* Variables for ADC conversion data */
  __IO uint16_t uhADCxConvertedData = 0x00;

  /* Perform ADC group regular conversion start, poll for conversion        */
  /* completion.                                                            */
  ConversionStartPoll_ADC_GrpRegular ();

  /* Retrieve ADC conversion data */
  switch (Resolution)
  {
    case LL_ADC_RESOLUTION_12B:
    {
      uhADCxConvertedData = VAR_CONVERTED_DATA_INIT_VALUE_12B;

      uhADCxConvertedData = LL_ADC_REG_ReadConversionData12(p_ADCHandle);

      break;
    }
    case LL_ADC_RESOLUTION_10B:
    {
      uhADCxConvertedData = VAR_CONVERTED_DATA_INIT_VALUE_10B;

      uhADCxConvertedData = LL_ADC_REG_ReadConversionData10(p_ADCHandle);

      break;
    }
    case LL_ADC_RESOLUTION_8B:
    {
      uhADCxConvertedData = VAR_CONVERTED_DATA_INIT_VALUE_8B;

      uhADCxConvertedData = LL_ADC_REG_ReadConversionData8(p_ADCHandle);

      break;
    }
    case LL_ADC_RESOLUTION_6B:
    {
      uhADCxConvertedData = VAR_CONVERTED_DATA_INIT_VALUE_6B;

      uhADCxConvertedData = LL_ADC_REG_ReadConversionData6(p_ADCHandle);

      break;
    }
    default:
    {
      /* Do nothing */
      break;
    }
  }

  return uhADCxConvertedData;
}

uint32_t AdcRankToOrder (const uint32_t Rank)
{
  uint32_t rankOrder = 0u;

  switch (Rank)
  {
    case LL_ADC_REG_RANK_1:
      rankOrder = 1u;
      break;
    case LL_ADC_REG_RANK_2:
      rankOrder = 2u;
      break;
    case LL_ADC_REG_RANK_3:
      rankOrder = 3u;
      break;
    case LL_ADC_REG_RANK_4:
      rankOrder = 4u;
      break;
    case LL_ADC_REG_RANK_5:
      rankOrder = 5u;
      break;
    case LL_ADC_REG_RANK_6:
      rankOrder = 6u;
      break;
    case LL_ADC_REG_RANK_7:
      rankOrder = 7u;
      break;
    default:
      rankOrder = 0u;
      break;
  }

  return rankOrder;
}

uint32_t AdcOrderToRank (const uint32_t RankOrder)
{
  uint32_t rank = 0u;

  switch (RankOrder)
  {
    case 1u:
      rank = LL_ADC_REG_RANK_1;
      break;
    case 2u:
      rank = LL_ADC_REG_RANK_2;
      break;
    case 3u:
      rank = LL_ADC_REG_RANK_3;
      break;
    case 4u:
      rank = LL_ADC_REG_RANK_4;
      break;
    case 5u:
      rank = LL_ADC_REG_RANK_5;
      break;
    case 6u:
      rank = LL_ADC_REG_RANK_6;
      break;
    case 7u:
      rank = LL_ADC_REG_RANK_7;
      break;
#ifdef LL_ADC_REG_RANK_8
    case 8u:
      rank = LL_ADC_REG_RANK_8;
      break;
#endif
    default:
      rank = 0u;
      break;
  }

  return rank;
}

uint32_t AdcSeqLengthToCount (const uint32_t SeqLength)
{
  uint32_t rankCount = 0u;

  switch (SeqLength)
  {
    case LL_ADC_REG_SEQ_SCAN_DISABLE:
      rankCount = 1u;
      break;
#ifdef LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS
    case LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS:
      rankCount = 2u;
      break;
#endif
#ifdef LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS
    case LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS:
      rankCount = 3u;
      break;
#endif
#ifdef LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS
    case LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS:
      rankCount = 4u;
      break;
#endif
#ifdef LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS
    case LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS:
      rankCount = 5u;
      break;
#endif
#ifdef LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS
    case LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS:
      rankCount = 6u;
      break;
#endif
#ifdef LL_ADC_REG_SEQ_SCAN_ENABLE_7RANKS
    case LL_ADC_REG_SEQ_SCAN_ENABLE_7RANKS:
      rankCount = 7u;
      break;
#endif
    default:
      rankCount = 0u;
      break;
  }

  return rankCount;
}

uint32_t AdcCountToSeqLength (const uint32_t RankCount)
{
  uint32_t seqLength = 0u;

  switch (RankCount)
  {
    case 1u:
      seqLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
      break;
#ifdef LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS
    case 2u:
      seqLength = LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS;
      break;
#endif
#ifdef LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS
    case 3u:
      seqLength = LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS;
      break;
#endif
#ifdef LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS
    case 4u:
      seqLength = LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS;
      break;
#endif
#ifdef LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS
    case 5u:
      seqLength = LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS;
      break;
#endif
#ifdef LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS
    case 6u:
      seqLength = LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS;
      break;
#endif
#ifdef LL_ADC_REG_SEQ_SCAN_ENABLE_7RANKS
    case 7u:
      seqLength = LL_ADC_REG_SEQ_SCAN_ENABLE_7RANKS;
      break;
#endif
#ifdef LL_ADC_REG_SEQ_SCAN_ENABLE_8RANKS
    case 8u:
      seqLength = LL_ADC_REG_SEQ_SCAN_ENABLE_8RANKS;
      break;
#endif
    default:
      seqLength = 0u;
      break;
  }

  return seqLength;
}

void ConversionStartPoll_ADC_GrpRegular (void)
{

  /* Start ADC group regular conversion */
  /* Note: Hardware constraint (refer to description of the function          */
  /*       below):                                                            */
  /*       On this STM32 series, setting of this feature is conditioned to    */
  /*       ADC state:                                                         */
  /*       ADC must be enabled without conversion on going on group regular,  */
  /*       without ADC disable command on going.                              */
  /* Note: In this example, all these checks are not necessary but are        */
  /*       implemented anyway to show the best practice usages                */
  /*       corresponding to reference manual procedure.                       */
  /*       Software can be optimized by removing some of these checks, if     */
  /*       they are not relevant considering previous settings and actions    */
  /*       in user application.                                               */
  if ((LL_ADC_IsEnabled(p_ADCHandle) == 1)               &&
      (LL_ADC_IsDisableOngoing(p_ADCHandle) == 0)        &&
      (LL_ADC_REG_IsConversionOngoing(p_ADCHandle) == 0)   )
  {
    LL_ADC_REG_StartConversion(p_ADCHandle);
  }
  else
  {
    /* Error: ADC conversion start could not be performed */
    Error_Handler();
  }

  while (LL_ADC_IsActiveFlag_EOC(p_ADCHandle) == 0);

  /* Clear flag ADC group regular end of unitary conversion */
  /* Note: This action is not needed here, because flag ADC group regular   */
  /*       end of unitary conversion is cleared automatically when          */
  /*       software reads conversion data from ADC data register.           */
  /*       Nevertheless, this action is done anyway to show how to clear    */
  /*       this flag, needed if conversion data is not always read          */
  /*       or if group injected end of unitary conversion is used (for      */
  /*       devices with group injected available).                          */
  LL_ADC_ClearFlag_EOC(p_ADCHandle);
}

/* Weak function Definition --------------------------------------------------*/
__WEAK ADCCTRL_Cmd_Status_t ADCCTRL_MutexTake (void)
{
  return ADCCTRL_OK;
}

__WEAK ADCCTRL_Cmd_Status_t ADCCTRL_MutexRelease (void)
{
  return ADCCTRL_OK;
}
