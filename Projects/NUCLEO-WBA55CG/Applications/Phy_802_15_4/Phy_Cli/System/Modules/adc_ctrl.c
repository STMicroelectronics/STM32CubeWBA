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

#include "adc_ctrl.h"

#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)
#include "stm32wbaxx_ll_adc.h"
#include "utilities_common.h"
#include "RTDebug.h"
#include <stdint.h>

/* Init variable out of expected ADC conversion data range */
#define VAR_CONVERTED_DATA_INIT_VALUE  (__LL_ADC_DIGITAL_SCALE(LL_ADC_RESOLUTION_12B) + 1)

/* Temperature sensor characteristics from datasheet */
#define TEMPSENSOR_TYP_CAL1_V          (( int32_t)  760)        /* Internal temperature sensor, parameter V30 (unit: mV). Refer to device datasheet for min/typ/max values. */
#define TEMPSENSOR_TYP_AVGSLOPE        (( int32_t) 2500)        /* Internal temperature sensor, parameter Avg_Slope (unit: uV/DegCelsius). Refer to device datasheet for min/typ/max values. */

/* Definitions of environment analog values */
/* Value of analog reference voltage (Vref+), connected to analog voltage   */
/* supply Vdda (unit: mV).                                                  */
#define VDDA_APPLI                     (3300UL)

extern void Error_Handler(void);

/* ADC client list */
static uint32_t adc_client_list = 0;

static void adc_ctrl_AdcActivate(void);
static void adc_ctrl_AdcDeactivate(void);
static void adc_ctrl_ConversionStartPoll_ADC_GrpRegular(void);

/**
  * @brief  Activate ADC.
  *         Activation order is as follow:
  *         - Enable ADC peripharal clock
  *         - Enable ADC internal voltage regulator
  *         - Enable ADC
  * @param  None
  * @retval None
  */
static void adc_ctrl_AdcActivate(void)
{
  __IO uint32_t backup_setting_adc_dma_transfer = 0U;

  SYSTEM_DEBUG_SIGNAL_SET(ADC_ACTIVATION);

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
  if (LL_ADC_IsEnabled(ADC4) == 0)
  {
    /* Peripheral clock enable */
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_ADC4);

    /* Enable ADC internal voltage regulator */
    LL_ADC_EnableInternalRegulator(ADC4);

    /* Waiting for ADC internal voltage regulator stabilization. */
    while(LL_ADC_IsInternalRegulatorEnabled(ADC4) == 0);

    /* Disable ADC DMA transfer request during calibration */
    /* Note: Specificity of this STM32 series: Calibration factor is          */
    /*       available in data register and also transferred by DMA.          */
    /*       To not insert ADC calibration factor among ADC conversion data   */
    /*       in DMA destination address, DMA transfer must be disabled during */
    /*       calibration.                                                     */
    backup_setting_adc_dma_transfer = LL_ADC_REG_GetDMATransfer(ADC4);
    LL_ADC_REG_SetDMATransfer(ADC4, LL_ADC_REG_DMA_TRANSFER_NONE);

    /* Run ADC self calibration */
    LL_ADC_StartCalibration(ADC4);

    /* Poll for ADC effectively calibrated */

    while (LL_ADC_IsCalibrationOnGoing(ADC4) != 0);

    /* Restore ADC DMA transfer request after calibration */
    LL_ADC_REG_SetDMATransfer(ADC4, backup_setting_adc_dma_transfer);

    /* Delay required between ADC end of calibration and ADC enable */
    /* LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES --> 2 cycles */
    __asm("mov r0, r0");
    __asm("mov r0, r0");

    /* Enable ADC */
    LL_ADC_Enable(ADC4);

    /* Poll for ADC ready to convert */
    while (LL_ADC_IsActiveFlag_ADRDY(ADC4) == 0);

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

  SYSTEM_DEBUG_SIGNAL_RESET(ADC_ACTIVATION);
}

/**
  * @brief  Perform ADC group regular conversion start, poll for conversion
  *         completion.
  *         (ADC4 instance: ADC).
  * @note   This function does not perform ADC group regular conversion stop:
  *         intended to be used with ADC in single mode, trigger SW start
  *         (only 1 ADC conversion done at each trigger, no conversion stop
  *         needed).
  *         In case of continuous mode or conversion trigger set to
  *         external trigger, ADC group regular conversion stop must be added.
  * @param  None
  * @retval None
  */
static void adc_ctrl_ConversionStartPoll_ADC_GrpRegular(void)
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
  if ((LL_ADC_IsEnabled(ADC4) == 1)               &&
      (LL_ADC_IsDisableOngoing(ADC4) == 0)        &&
      (LL_ADC_REG_IsConversionOngoing(ADC4) == 0)   )
  {
    LL_ADC_REG_StartConversion(ADC4);
  }
  else
  {
    /* Error: ADC conversion start could not be performed */
    Error_Handler();
  }

  while (LL_ADC_IsActiveFlag_EOC(ADC4) == 0);

  /* Clear flag ADC group regular end of unitary conversion */
  /* Note: This action is not needed here, because flag ADC group regular   */
  /*       end of unitary conversion is cleared automatically when          */
  /*       software reads conversion data from ADC data register.           */
  /*       Nevertheless, this action is done anyway to show how to clear    */
  /*       this flag, needed if conversion data is not always read          */
  /*       or if group injected end of unitary conversion is used (for      */
  /*       devices with group injected available).                          */
  LL_ADC_ClearFlag_EOC(ADC4);

}

/**
  * @brief  Deactivate ADC IP.
  *         Deactivation order is as follow:
  *         - Disable ADC
  *         - Disable ADC internal voltage regulator
  *         - Disable ADC peripharal clock
  * @param  None
  * @retval None
  */
static void adc_ctrl_AdcDeactivate(void)
{
  SYSTEM_DEBUG_SIGNAL_SET(ADC_DEACTIVATION);
  if(LL_ADC_IsEnabled(ADC4))
  {
    /* Disable ADC */
    LL_ADC_Disable(ADC4);

    /* Wait until ADC_CR_ADEN bit is reset before turning off ADC internal regulator */
    while(LL_ADC_IsEnabled(ADC4) == 1UL);

    /* Wait until ADC_CR_ADSTP bit is reset before turning off ADC internal regulator */
    while(LL_ADC_REG_IsStopConversionOngoing(ADC4) == 1U);

    /* Disable ADC internal voltage regulator */
    LL_ADC_DisableInternalRegulator(ADC4);
    while(LL_ADC_IsInternalRegulatorEnabled(ADC4) == 1U);

    /* Peripharal clock disable */
    LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_ADC4);
  }
  SYSTEM_DEBUG_SIGNAL_RESET(ADC_DEACTIVATION);
}

/**
  * @brief  Initialize the adc client list
  * @param  None
  * @retval None
  */
void adc_ctrl_init(void)
{
  adc_client_list = 0;
  adc_ctrl_AdcDeactivate();
}

/**
  * @brief  Manage ADC enable/disable requests
  * @param  client     client that requests an ADC state change
  * @param  enable     enable or disable request
  * @retval None
  */
void adc_ctrl_req(adc_client_t client, adc_state_t enable)
{
  UTILS_ENTER_CRITICAL_SECTION();

  if(enable == ADC_OFF)
  {
    adc_client_list &= (~(1U << client));
  }

  else
  {
    adc_client_list |= (1U << client);
  }

  if(adc_client_list == 0)
  {
    /* Disable ADC as there is no request anymore */
    adc_ctrl_AdcDeactivate();
  }

  else{
    /* Enable ADC as there at least one request */
    adc_ctrl_AdcActivate();
  }

  UTILS_EXIT_CRITICAL_SECTION();
}

uint16_t adc_ctrl_request_temperature(void)
{
  /* Variables for ADC conversion data */
  __IO uint16_t uhADCxConvertedData = VAR_CONVERTED_DATA_INIT_VALUE;

  /* Variables for ADC conversion data computation to physical values
   * Value of temperature calculated from ADC conversion data (unit: degree Celsius)
   */
  __IO  int16_t hADCxConvertedData_Temperature_DegreeCelsius = 0UL;

  SYSTEM_DEBUG_SIGNAL_SET(ADC_TEMPERATURE_ACQUISITION);

  /* Perform ADC group regular conversion start, poll for conversion        */
  /* completion.                                                            */
  adc_ctrl_ConversionStartPoll_ADC_GrpRegular();

  /* Retrieve ADC conversion data */
  /* (data scale corresponds to ADC resolution: 12 bits) */
  uhADCxConvertedData = LL_ADC_REG_ReadConversionData12(ADC4);

  /* Computation of ADC conversions raw data to physical values             */
  /* using LL ADC driver helper macro.                                      */
  if(*TEMPSENSOR_CAL1_ADDR == *TEMPSENSOR_CAL2_ADDR)
  {
    /* Case of samples not calibrated in production */
    hADCxConvertedData_Temperature_DegreeCelsius = __LL_ADC_CALC_TEMPERATURE_TYP_PARAMS(
                                                       TEMPSENSOR_TYP_AVGSLOPE,
                                                       TEMPSENSOR_TYP_CAL1_V,
                                                       TEMPSENSOR_CAL1_TEMP,
                                                       VDDA_APPLI,
                                                       uhADCxConvertedData,
                                                       LL_ADC_RESOLUTION_12B);
  }
  else
  {
    /* Case of samples calibrated in production */
    hADCxConvertedData_Temperature_DegreeCelsius = __LL_ADC_CALC_TEMPERATURE(VDDA_APPLI, uhADCxConvertedData, LL_ADC_RESOLUTION_12B);
  }

  SYSTEM_DEBUG_SIGNAL_RESET(ADC_TEMPERATURE_ACQUISITION);

  return hADCxConvertedData_Temperature_DegreeCelsius;
}
#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */
