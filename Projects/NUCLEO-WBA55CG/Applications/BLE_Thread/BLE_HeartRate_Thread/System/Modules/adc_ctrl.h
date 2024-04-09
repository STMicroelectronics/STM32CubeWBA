/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc_client.h
  * @author  MCD Application Team
  * @brief   Header for ADC client manager module
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

#ifndef ADC_CTRL_H
#define ADC_CTRL_H

/* Includes ------------------------------------------------------------------*/
/* Utilities */
#include "utilities_common.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/**
 * @brief ADC command status codes
 */
typedef enum ADCCTRL_Cmd_Status
{
  ADCCTRL_OK,
  ADCCTRL_NOK,
  ADCCTRL_BUSY,
  ADCCTRL_HANDLE_ALREADY_REGISTERED,
  ADCCTRL_HANDLE_NOT_REGISTERED,
  ADCCTRL_HANDLE_NOT_VALID,
  ADCCTRL_ERROR_NULL_POINTER,
  ADCCTRL_ERROR_CONFIG,
  ADCCTRL_ERROR_STATE,
  ADCCTRL_UNKNOWN,
} ADCCTRL_Cmd_Status_t;

/**
  * @brief  ADC IP state
  */
typedef enum ADCCTRL_Ip_State
{
  ADC_OFF,
  ADC_ON
} ADCCTRL_Ip_State_t;

/**
 * @brief ADC handle states
 */
typedef enum ADCCTRL_HandleState
{
  ADCCTRL_HANDLE_NOT_REG,
  ADCCTRL_HANDLE_REG,
} ADCCTRL_HandleState_t;

/**
 * @brief ADC Init configuration structure
 *
 */
typedef struct ADCCTRL_InitConfig
{
  uint32_t ClockPrescaler;        /*!< Select ADC clock source (synchronous clock derived from APB clock or asynchronous
                                       clock derived from system clock or PLL (Refer to reference manual for list of
                                       clocks available)) and clock prescaler.
                                       This parameter can be a value of @ref ADC_HAL_EC_COMMON_CLOCK_SOURCE.
                                       Note: The ADC clock configuration is common to all ADC instances.
                                       Note: In case of synchronous clock mode based on HCLK/1, the configuration must
                                             be enabled only if the system clock has a 50% duty clock cycle (APB
                                             prescaler configured inside RCC  must be bypassed and PCLK clock must have
                                             50% duty cycle). Refer to reference manual for details.
                                       Note: In case of usage of asynchronous clock, the selected clock must be
                                             preliminarily enabled at RCC top level.
                                       Note: This parameter can be modified only if all ADC instances are disabled. */

  uint32_t Resolution;            /*!< Configure the ADC resolution.
                                       This parameter can be a value of @ref ADC_HAL_EC_RESOLUTION */

  uint32_t DataAlign;             /*!< Specify ADC data alignment in conversion data register (right or left).
                                       Refer to reference manual for alignments formats versus resolutions.
                                       This parameter can be a value of @ref ADC_HAL_EC_DATA_ALIGN */

  uint32_t ScanConvMode;          /*!< Configure the sequencer of ADC group regular.
                                       On this STM32 series, ADC group regular sequencer both modes "fully configurable"
                                       or "not fully configurable" are available:
                                        - sequencer configured to fully configurable:
                                          sequencer length and each rank affectation to a channel are configurable.
                                           - Sequence length: Set number of ranks in the scan sequence.
                                           - Sequence direction: Unless specified in parameters, sequencer
                                             scan direction is forward (from rank 1 to rank n).
                                        - sequencer configured to not fully configurable:
                                            sequencer length and each rank affectation to a channel are fixed by channel
                                            HW number.
                                           - Sequence length: Number of ranks in the scan sequence is
                                             defined by number of channels set in the sequence,
                                             rank of each channel is fixed by channel HW number.
                                             (channel 0 fixed on rank 0, channel 1 fixed on rank1, ...).
                                           - Sequence direction: Unless specified in parameters, sequencer
                                             scan direction is forward (from lowest channel number to
                                             highest channel number).
                                       This parameter can be associated to parameter 'DiscontinuousConvMode' to have
                                       main sequence subdivided in successive parts. Sequencer is automatically enabled
                                       if several channels are set (sequencer cannot be disabled, as it can be the case
                                       on other STM32 devices):
                                       If only 1 channel is set: Conversion is performed in single mode.
                                       If several channels are set:  Conversions are performed in sequence mode.
                                       This parameter can be a value of @ref ADC_Scan_mode */

  uint32_t EOCSelection;          /*!< Specify which EOC (End Of Conversion) flag is used for conversion by polling and
                                       interruption: end of unitary conversion or end of sequence conversions.
                                       This parameter can be a value of @ref ADC_EOCSelection. */

  FunctionalState LowPowerAutoWait; /*!< Select the dynamic low power Auto Delay: new conversion start only when the
                                       previous conversion (for ADC group regular) has been retrieved by user software,
                                       using function HAL_ADC_GetValue().
                                       This feature automatically adapts the frequency of ADC conversions triggers to
                                       the speed of the system that reads the data. Moreover, this avoids risk of
                                       overrun for low frequency applications.
                                       This parameter can be set to ENABLE or DISABLE.
                                       Note: It is not recommended to use with interruption or DMA (HAL_ADC_Start_IT(),
                                             HAL_ADC_Start_DMA()) since these modes have to clear immediately the EOC
                                             flag (by CPU to free the IRQ pending event or by DMA).
                                             Auto wait will work but fort a very short time, discarding its intended
                                             benefit (except specific case of high load of CPU or DMA transfers which
                                             can justify usage of auto wait).
                                             Do use with polling: 1. Start conversion with HAL_ADC_Start(), 2. Later on,
                                             when ADC conversion data is needed:
                                             use HAL_ADC_PollForConversion() to ensure that conversion is completed and
                                             HAL_ADC_GetValue() to retrieve conversion result and trig another
                                             conversion start. */

  FunctionalState LowPowerAutoPowerOff; /*!< Select the auto-off mode: the ADC automatically powers-off after a
                                             conversion and automatically wakes-up when a new conversion is triggered
                                             (with startup time between trigger and start of sampling).
                                              This feature can be combined with automatic wait mode
                                             (parameter 'LowPowerAutoWait').
                                              This parameter can be set to ENABLE or DISABLE. */

  uint32_t LowPowerAutonomousDPD; /*!< Set ADC low power mode: deep power down in autonomous mode.
                                       This parameter can be a value of
                                       @ref ADC_HAL_EC_AUTONOMOUS_DEEP_POWER_DOWN_MODE. */

  FunctionalState ContinuousConvMode; /*!< Specify whether the conversion is performed in single mode (one conversion)
                                           or continuous mode for ADC group regular, after the first ADC conversion
                                           start trigger occurred (software start or external trigger). This parameter
                                           can be set to ENABLE or DISABLE. */

  uint32_t NbrOfConversion;       /*!< Specify the number of ranks that will be converted within the regular group
                                       sequencer.
                                       This parameter is dependent on ScanConvMode:
                                        - sequencer configured to fully configurable:
                                          Number of ranks in the scan sequence is configurable using this parameter.
                                          Note: After the first call of 'HAL_ADC_Init()', each rank corresponding to
                                                parameter "NbrOfConversion" must be set using 'HAL_ADC_ConfigChannel()'.
                                                Afterwards, when all needed sequencer ranks are set, parameter
                                                'NbrOfConversion' can be updated without modifying configuration of
                                                sequencer ranks (sequencer ranks above 'NbrOfConversion' are discarded).
                                        - sequencer configured to not fully configurable:
                                          Number of ranks in the scan sequence is defined by number of channels set in
                                          the sequence. This parameter is discarded.
                                       This parameter must be a number between Min_Data = 1 and Max_Data = 8.
                                       Note: This parameter must be modified when no conversion is on going on regular
                                             group (ADC disabled, or ADC enabled without continuous mode or external
                                             trigger that could launch a conversion). */

  FunctionalState DiscontinuousConvMode; /*!< Specify whether the conversions sequence of ADC group regular is performed
                                              in Complete-sequence/Discontinuous-sequence (main sequence subdivided in
                                              successive parts).
                                              Discontinuous mode is used only if sequencer is enabled (parameter
                                              'ScanConvMode'). If sequencer is disabled, this parameter is discarded.
                                              Discontinuous mode can be enabled only if continuous mode is disabled.
                                              If continuous mode is enabled, this parameter setting is discarded.
                                              This parameter can be set to ENABLE or DISABLE.
                                              Note: On this STM32 series, ADC group regular number of discontinuous
                                                    ranks increment is fixed to one-by-one. */

  uint32_t ExternalTrigConv;      /*!< Select the external event source used to trigger ADC group regular conversion
                                       start.
                                       If set to ADC_SOFTWARE_START, external triggers are disabled and software trigger
                                       is used instead.
                                       This parameter can be a value of @ref ADC_regular_external_trigger_source.
                                       Caution: external trigger source is common to all ADC instances. */

  uint32_t ExternalTrigConvEdge;  /*!< Select the external event edge used to trigger ADC group regular conversion start
                                       If trigger source is set to ADC_SOFTWARE_START, this parameter is discarded.
                                       This parameter can be a value of @ref ADC_regular_external_trigger_edge */

  FunctionalState DMAContinuousRequests; /*!< Specify whether the DMA requests are performed in one shot mode (DMA
                                              transfer stops when number of conversions is reached) or in continuous
                                              mode (DMA transfer unlimited, whatever number of conversions).
                                              This parameter can be set to ENABLE or DISABLE.
                                              Note: In continuous mode, DMA must be configured in circular mode.
                                                    Otherwise an overrun will be triggered when DMA buffer maximum
                                                    pointer is reached. */

  uint32_t Overrun;               /*!< Select the behavior in case of overrun: data overwritten or preserved (default).
                                       This parameter can be a value of @ref ADC_HAL_EC_REG_OVR_DATA_BEHAVIOR.
                                       Note: In case of overrun set to data preserved and usage with programming model
                                             with interruption (HAL_Start_IT()): ADC IRQ handler has to clear end of
                                             conversion flags, this induces the release of the preserved data. If
                                             needed, this data can be saved in function HAL_ADC_ConvCpltCallback(),
                                             placed in user program code (called before end of conversion flags clear)
                                       Note: Error reporting with respect to the conversion mode:
                                             - Usage with ADC conversion by polling for event or interruption: Error is
                                               reported only if overrun is set to data preserved. If overrun is set to
                                               data overwritten, user can willingly not read all the converted data,
                                               this is not considered as an erroneous case.
                                             - Usage with ADC conversion by DMA: Error is reported whatever overrun
                                               setting (DMA is expected to process all data from data register). */

  uint32_t SamplingTimeCommon1;   /*!< Set sampling time common to a group of channels.
                                       Unit: ADC clock cycles
                                       Conversion time is the addition of sampling time and processing time
                                       (12.5 ADC clock cycles at ADC resolution 12 bits,
                                        10.5 cycles at 10 bits,
                                         8.5 cycles at 8 bits,
                                         6.5 cycles at 6 bits).
                                       Note: On this STM32 family, two different sampling time settings are available,
                                             each channel can use one of these two settings. On some other STM32 devices
                                             this parameter in channel wise and is located into ADC channel
                                             initialization structure.
                                       This parameter can be a value of @ref ADC_HAL_EC_CHANNEL_SAMPLINGTIME
                                       Note: In case of usage of internal measurement channels (VrefInt/Vbat/TempSensor)
                                             sampling time constraints must be respected (sampling time can be adjusted
                                             in function of ADC clock frequency and sampling time setting)
                                             Refer to device datasheet for timings values, parameters TS_vrefint,
                                             TS_vbat, TS_temp (values rough order: few tens of microseconds). */

  uint32_t SamplingTimeCommon2;   /*!< Set sampling time common to a group of channels, second common setting possible.
                                       Unit: ADC clock cycles
                                       Conversion time is the addition of sampling time and processing time
                                       (12.5 ADC clock cycles at ADC resolution 12 bits,
                                        10.5 cycles at 10 bits,
                                         8.5 cycles at 8 bits,
                                         6.5 cycles at 6 bits).
                                       Note: On this STM32 family, two different sampling time settings are available,
                                             each channel can use one of these two settings. On some other STM32 devices
                                             this parameter in channel wise and is located into ADC channel
                                             initialization structure.
                                       This parameter can be a value of @ref ADC_HAL_EC_CHANNEL_SAMPLINGTIME
                                       Note: In case of usage of internal measurement channels (VrefInt/Vbat/TempSensor)
                                             sampling time constraints must be respected (sampling time can be adjusted
                                             in function of ADC clock frequency and sampling time setting)
                                             Refer to device datasheet for timings values, parameters TS_vrefint,
                                             TS_vbat, TS_temp (values rough order: few tens of microseconds). */

  FunctionalState OversamplingMode;       /*!< Specify whether the oversampling feature is enabled or disabled.
                                               This parameter can be set to ENABLE or DISABLE.
                                               Note: This parameter can be modified only if there is no conversion is
                                                     ongoing on ADC group regular. */

  ADC_OversamplingTypeDef Oversampling;   /*!< Specify the Oversampling parameters.
                                               Caution: this setting overwrites the previous oversampling configuration
                                                        if oversampling is already enabled. */

  uint32_t TriggerFrequencyMode;  /*!< Set ADC trigger frequency mode.
                                       This parameter can be a value of @ref ADC_HAL_EC_REG_TRIGGER_FREQ.
                                       Note: ADC trigger frequency mode must be set to low frequency when
                                             a duration is exceeded before ADC conversion start trigger event
                                             (between ADC enable and ADC conversion start trigger event
                                             or between two ADC conversion start trigger event).
                                             Duration value: Refer to device datasheet, parameter "tIdle".
                                       Note: When ADC trigger frequency mode is set to low frequency,
                                             some rearm cycles are inserted before performing ADC conversion
                                             start, inducing a delay of 2 ADC clock cycles. */

} ADCCTRL_InitConfig_t;

/**
 * @brief ADC Channel configuration structure
 *
 */
typedef struct ADCCTRL_ChannelConfig
{
  uint32_t Channel;                /*!< Specify the channel to configure into ADC regular group.
                                        This parameter can be a value of @ref ADC_HAL_EC_CHANNEL
                                        Note: Depending on devices and ADC instances, some channels may not be available
                                              on device package pins. Refer to device datasheet for channels
                                              availability. */

  uint32_t Rank;                   /*!< Add or remove the channel from ADC regular group sequencer and specify its
                                        conversion rank.
                                        This parameter is dependent on ScanConvMode:
                                        - sequencer configured to fully configurable:
                                          Channels ordering into each rank of scan sequence:
                                          whatever channel can be placed into whatever rank.
                                        - sequencer configured to not fully configurable:
                                          rank of each channel is fixed by channel HW number.
                                          (channel 0 fixed on rank 0, channel 1 fixed on rank1, ...).
                                          Despite the channel rank is fixed, this parameter allow an additional
                                          possibility: to remove the selected rank (selected channel) from sequencer.
                                        This parameter can be a value of @ref ADC_HAL_EC_REG_SEQ_RANKS */

  uint32_t SamplingTime;           /*!< Sampling time value to be set for the selected channel.
                                        Unit: ADC clock cycles
                                        Conversion time is the addition of sampling time and processing time
                                        (12.5 ADC clock cycles at ADC resolution 12 bits,
                                         10.5 cycles at 10 bits,
                                         8.5 cycles at 8 bits,
                                         6.5 cycles at 6 bits).
                                        This parameter can be a value of @ref ADC_HAL_EC_SAMPLINGTIME_COMMON
                                        Note: On this STM32 family, two different sampling time settings are available
                                              (refer to parameters "SamplingTimeCommon1" and "SamplingTimeCommon2"),
                                               each channel can use one of these two settings.

                                        Note: In case of usage of internal measurement channels (VrefInt/Vbat/
                                              TempSensor), sampling time constraints must be respected (sampling time
                                              can be adjusted in function of ADC clock frequency and sampling time
                                              setting)
                                              Refer to device datasheet for timings values. */
} ADCCTRL_ChannelConfig_t;

/**
 * @brief ADC handle typedef
 */
typedef struct ADCCTRL_Handle
{
  uint32_t Uid;                           /* Id of the Handle instance */
  ADCCTRL_HandleState_t State;            /* State of the ADC Controller handle */
  ADCCTRL_InitConfig_t InitConf;          /* Init configuration of the ADC */
  ADCCTRL_ChannelConfig_t ChannelConf;    /* Channel configuration of the ADC */
} ADCCTRL_Handle_t;

/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/**
  * @brief  Initialize the adc controller
  *
  * @retval State of the initialization
  */
ADCCTRL_Cmd_Status_t ADCCTRL_Init(void);

/**
  * @brief  Register a ADC handle
  *
  * @param p_Handle: Handle to register
  *
  * @return State of the handle initialization
  */
ADCCTRL_Cmd_Status_t ADCCTRL_RegisterHandle (ADCCTRL_Handle_t * const p_Handle);

/**
 * @brief Request a specific state for the ADC IP - Either On or Off
 *
 * @param p_Handle: ADC handle
 * @param State: Requested state to apply
 *
 * @return State of the operation
 */
ADCCTRL_Cmd_Status_t ADCCTRL_RequestIpState (const ADCCTRL_Handle_t * const p_Handle,
                                             const ADCCTRL_Ip_State_t State);

/**
  * @brief  Read raw value of the ADC
  *
  * @param p_Handle: ADC handle
  * @param p_ReadValue: Raw ADC value
  *
  * @retval Operation state
  */
ADCCTRL_Cmd_Status_t ADCCTRL_RequestRawValue (const ADCCTRL_Handle_t * const p_Handle,
                                              uint16_t * const p_ReadValue);

/**
  * @brief  Read temperature from ADC temperature sensor
  *
  * @details The returned value is actual temperature sensor value
  *
  * @param p_Handle: ADC handle
  * @param p_ReadValue: Temperature measurement
  *
  * @retval Operation state
  */
ADCCTRL_Cmd_Status_t ADCCTRL_RequestTemperature (const ADCCTRL_Handle_t * const p_Handle,
                                                 uint16_t * const p_ReadValue);

/**
  * @brief  Read Voltage from ADC
  *
  * @details The returned value is actual Voltage value in mVolts
  *
  * @param p_Handle: ADC handle
  * @param p_ReadValue: Core Voltage measurement
  *
  * @retval Operation state
  */
ADCCTRL_Cmd_Status_t ADCCTRL_RequestCoreVoltage (const ADCCTRL_Handle_t * const p_Handle,
                                                 uint16_t * const p_ReadValue);

/**
  * @brief  Read reference Voltage from ADC
  *
  * @details The returned value is actual reference Voltage value in mVolts
  *
  * @param p_Handle: ADC handle
  * @param p_ReadValue: Reference Voltage measurement
  *
  * @retval Operation state
  */
ADCCTRL_Cmd_Status_t ADCCTRL_RequestRefVoltage (const ADCCTRL_Handle_t * const p_Handle,
                                                uint16_t * const p_ReadValue);

/* Exported functions to be implemented by the user ------------------------- */
/**
 * @brief  Take ownership on the ADC mutex
 *
 * @details This function shall be implemented by the user
 *
 * @return Status of the command
 * @retval ADCCTRL_Cmd_Status_t::ADCCTRL_OK
 * @retval ADCCTRL_Cmd_Status_t::ADCCTRL_NOK
 */
extern ADCCTRL_Cmd_Status_t ADCCTRL_MutexTake (void);

/**
 * @brief  Release ownership on the ADC mutex
 *
 * @details This function shall be implemented by the user
 *
 * @return Status of the command
 * @retval ADCCTRL_Cmd_Status_t::ADCCTRL_OK
 * @retval ADCCTRL_Cmd_Status_t::ADCCTRL_NOK
 */
extern ADCCTRL_Cmd_Status_t ADCCTRL_MutexRelease (void);

#endif /* ADC_CTRL_H */
