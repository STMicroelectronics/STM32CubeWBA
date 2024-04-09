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

/* Definitions of environment analog values */
/**
 * @brief Value of analog reference voltage (Vref+), connected to analog voltage
 *
 * @details supply Vdda (unit: mV).
 *
 */
#define VDDA_APPLI                     (3300UL)

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
 * @brief Higher registered handle ID
 */
static uint32_t MaxRegisteredId = ADCCTRL_NO_CONFIG;

/**
 * @brief Handle of the HAL ADC
 */
static ADC_HandleTypeDef ADCHandle =
{
  .Instance = ADCCTRL_HWADDR,
};

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
static inline HAL_StatusTypeDef AdcConfigure (const ADCCTRL_Handle_t * const p_Handle);

/**
 * @brief Read the raw value
 * @param  p_Handle: Handle to work with
 * @return Raw value
 */
static inline uint16_t AdcReadRaw (const ADCCTRL_Handle_t * const p_Handle);

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
__WEAK ADCCTRL_Cmd_Status_t ADCCTRL_Init(void)
{
  ADCCTRL_Cmd_Status_t error = ADCCTRL_UNKNOWN;

  /* Try to take the ADC mutex */
  error = ADCCTRL_MutexTake ();

  if (ADCCTRL_OK == error)
  {
    CurrentConfig = ADCCTRL_NO_CONFIG;

    ADCHandle.State = HAL_ADC_STATE_RESET;

    ADCHandle.Instance = ADCCTRL_HWADDR;

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

__WEAK ADCCTRL_Cmd_Status_t ADCCTRL_RequestRawValue (const ADCCTRL_Handle_t * const p_Handle,
                                                     uint16_t * const p_ReadValue)
{
  ADCCTRL_Cmd_Status_t error = ADCCTRL_UNKNOWN;
  HAL_StatusTypeDef eReturn = HAL_OK;

  /* Null pointer for handle or payload */
  if ((NULL == p_Handle) || (NULL == p_ReadValue))
  {
    error = ADCCTRL_ERROR_NULL_POINTER;
  }
  /* Handle not init */
  else if (ADCCTRL_HANDLE_NOT_REG == p_Handle->State)
  {
    error = ADCCTRL_HANDLE_NOT_REGISTERED;
  }
  /* Handle not in the range */
  else if ((MaxRegisteredId < p_Handle->Uid) ||
           (ADCCTRL_NO_CONFIG >= p_Handle->Uid))
  {
    error = ADCCTRL_HANDLE_NOT_VALID;
  }
  /* Check ADC state */
  else if (0x00u == (ClientList & (1U << p_Handle->Uid)))
  {
    error = ADCCTRL_ERROR_STATE;
  }
  else
  {
    /* Try to take the ADC mutex */
    error = ADCCTRL_MutexTake ();

    if (ADCCTRL_OK == error)
    {
      /* Is the current config IS NOT the same as the one requested ? */
      if (CurrentConfig != p_Handle->Uid)
      {
        /* Configure the ADC before use */
        eReturn = AdcConfigure (p_Handle);

        /* Enable ADC */
        LL_ADC_Enable(ADCCTRL_HWADDR);
      }

      if (eReturn == HAL_OK)
      {
        /* Return the read value */
        *p_ReadValue = AdcReadRaw (p_Handle);
      }
      else
      {
        error = ADCCTRL_ERROR_CONFIG;
      }

      /* Release the mutex */
      ADCCTRL_MutexRelease ();
    }
  }

  return error;
}

__WEAK ADCCTRL_Cmd_Status_t ADCCTRL_RequestTemperature (const ADCCTRL_Handle_t * const p_Handle,
                                                        uint16_t * const p_ReadValue)
{
  ADCCTRL_Cmd_Status_t error = ADCCTRL_UNKNOWN;
  HAL_StatusTypeDef eReturn = HAL_OK;

  /* Variables for ADC conversion data */
  __IO uint16_t uhADCxConvertedData = 0x00;

  SYSTEM_DEBUG_SIGNAL_SET(ADC_TEMPERATURE_ACQUISITION);

  /* Null pointer for handle or payload */
  if ((NULL == p_Handle) || (NULL == p_ReadValue))
  {
    error = ADCCTRL_ERROR_NULL_POINTER;
  }
  /* Handle not init */
  else if (ADCCTRL_HANDLE_NOT_REG == p_Handle->State)
  {
    error = ADCCTRL_HANDLE_NOT_REGISTERED;
  }
  /* Handle not in the range */
  else if ((MaxRegisteredId < p_Handle->Uid) ||
           (ADCCTRL_NO_CONFIG >= p_Handle->Uid))
  {
    error = ADCCTRL_HANDLE_NOT_VALID;
  }
  /* Check ADC state */
  else if (0x00u == (ClientList & (1U << p_Handle->Uid)))
  {
    error = ADCCTRL_ERROR_STATE;
  }
  else
  {
    /* Try to take the ADC mutex */
    error = ADCCTRL_MutexTake ();

    if (ADCCTRL_OK == error)
    {
      /* Is the current config IS NOT the same as the one requested ? */
      if (CurrentConfig != p_Handle->Uid)
      {
        /* Configure the ADC before use */
        eReturn = AdcConfigure (p_Handle);

        /* Enable ADC */
        LL_ADC_Enable(ADCCTRL_HWADDR);
      }

      if (eReturn == HAL_OK)
      {
        /* Return the read value */
        uhADCxConvertedData = AdcReadRaw (p_Handle);

        /* Computation of ADC conversions raw data to physical values             */
        /* using LL ADC driver helper macro.                                      */
        if(*TEMPSENSOR_CAL1_ADDR == *TEMPSENSOR_CAL2_ADDR)
        {
          /* Case of samples not calibrated in production */
          *p_ReadValue = __LL_ADC_CALC_TEMPERATURE_TYP_PARAMS (TEMPSENSOR_TYP_AVGSLOPE,
                                                               TEMPSENSOR_TYP_CAL1_V,
                                                               TEMPSENSOR_CAL1_TEMP,
                                                               VDDA_APPLI,
                                                               uhADCxConvertedData,
                                                               p_Handle->InitConf.Resolution);
        }
        else
        {
          /* Case of samples calibrated in production */
          *p_ReadValue = __LL_ADC_CALC_TEMPERATURE (VDDA_APPLI,
                                                    uhADCxConvertedData,
                                                    p_Handle->InitConf.Resolution);
        }
      }
      else
      {
        error = ADCCTRL_ERROR_CONFIG;
      }

      /* Release the mutex */
      ADCCTRL_MutexRelease ();
    }
  }

  return error;
}

__WEAK ADCCTRL_Cmd_Status_t ADCCTRL_RequestCoreVoltage (const ADCCTRL_Handle_t * const p_Handle,
                                                        uint16_t * const p_ReadValue)
{
  ADCCTRL_Cmd_Status_t error = ADCCTRL_UNKNOWN;
  HAL_StatusTypeDef eReturn = HAL_OK;

  /* Variables for ADC conversion data */
  __IO uint16_t uhADCxConvertedData = 0x00;

  SYSTEM_DEBUG_SIGNAL_SET(ADC_TEMPERATURE_ACQUISITION);

  /* Null pointer for handle or payload */
  if ((NULL == p_Handle) || (NULL == p_ReadValue))
  {
    error = ADCCTRL_ERROR_NULL_POINTER;
  }
  /* Handle not init */
  else if (ADCCTRL_HANDLE_NOT_REG == p_Handle->State)
  {
    error = ADCCTRL_HANDLE_NOT_REGISTERED;
  }
  /* Handle not in the range */
  else if ((MaxRegisteredId < p_Handle->Uid) ||
           (ADCCTRL_NO_CONFIG >= p_Handle->Uid))
  {
    error = ADCCTRL_HANDLE_NOT_VALID;
  }
  /* Check ADC state */
  else if (0x00u == (ClientList & (1U << p_Handle->Uid)))
  {
    error = ADCCTRL_ERROR_STATE;
  }
  else
  {
    /* Try to take the ADC mutex */
    error = ADCCTRL_MutexTake ();

    if (ADCCTRL_OK == error)
    {
      /* Is the current config IS NOT the same as the one requested ? */
      if (CurrentConfig != p_Handle->Uid)
      {
        /* Configure the ADC before use */
        eReturn = AdcConfigure (p_Handle);

        /* Enable ADC */
        LL_ADC_Enable(ADCCTRL_HWADDR);
      }

      if (eReturn == HAL_OK)
      {
        /* Return the read value */
        uhADCxConvertedData = AdcReadRaw (p_Handle);

        /* Computation of ADC conversions raw data to physical values             */
        /* using LL ADC driver helper macro.                                      */
        *p_ReadValue = __LL_ADC_CALC_DATA_TO_VOLTAGE (VDDA_APPLI,
                                                      uhADCxConvertedData,
                                                      p_Handle->InitConf.Resolution);
      }
      else
      {
        error = ADCCTRL_ERROR_CONFIG;
      }

      /* Release the mutex */
      ADCCTRL_MutexRelease ();
    }
  }

  return error;
}

__WEAK ADCCTRL_Cmd_Status_t ADCCTRL_RequestRefVoltage (const ADCCTRL_Handle_t * const p_Handle,
                                                       uint16_t * const p_ReadValue)
{
  ADCCTRL_Cmd_Status_t error = ADCCTRL_UNKNOWN;
  HAL_StatusTypeDef eReturn = HAL_OK;

  /* Variables for ADC conversion data */
  __IO uint16_t uhADCxConvertedData = 0x00;

  SYSTEM_DEBUG_SIGNAL_SET(ADC_TEMPERATURE_ACQUISITION);

  /* Null pointer for handle or payload */
  if ((NULL == p_Handle) || (NULL == p_ReadValue))
  {
    error = ADCCTRL_ERROR_NULL_POINTER;
  }
  /* Handle not init */
  else if (ADCCTRL_HANDLE_NOT_REG == p_Handle->State)
  {
    error = ADCCTRL_HANDLE_NOT_REGISTERED;
  }
  /* Handle not in the range */
  else if ((MaxRegisteredId < p_Handle->Uid) ||
           (ADCCTRL_NO_CONFIG >= p_Handle->Uid))
  {
    error = ADCCTRL_HANDLE_NOT_VALID;
  }
  /* Check ADC state */
  else if (0x00u == (ClientList & (1U << p_Handle->Uid)))
  {
    error = ADCCTRL_ERROR_STATE;
  }
  else
  {
    /* Try to take the ADC mutex */
    error = ADCCTRL_MutexTake ();

    if (ADCCTRL_OK == error)
    {
      /* Is the current config IS NOT the same as the one requested ? */
      if (CurrentConfig != p_Handle->Uid)
      {
        /* Configure the ADC before use */
        eReturn = AdcConfigure (p_Handle);

        /* Enable ADC */
        LL_ADC_Enable(ADCCTRL_HWADDR);
      }

      if (eReturn == HAL_OK)
      {
        /* Return the read value */
        uhADCxConvertedData = AdcReadRaw (p_Handle);

        /* Computation of ADC conversions raw data to physical values             */
        /* using LL ADC driver helper macro.                                      */
        *p_ReadValue = __LL_ADC_CALC_VREFANALOG_VOLTAGE (uhADCxConvertedData,
                                                         p_Handle->InitConf.Resolution);
      }
      else
      {
        error = ADCCTRL_ERROR_CONFIG;
      }

      /* Release the mutex */
      ADCCTRL_MutexRelease ();
    }
  }

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
  if (LL_ADC_IsEnabled(ADCCTRL_HWADDR) == 0)
  {
    /* Peripheral clock enable */
    ADCCTRL_ENABLE_CLOCK ();

    /* Enable ADC internal voltage regulator */
    LL_ADC_EnableInternalRegulator(ADCCTRL_HWADDR);

    /* Waiting for ADC internal voltage regulator stabilization. */
    while(LL_ADC_IsInternalRegulatorEnabled(ADCCTRL_HWADDR) == 0);

    /* Disable ADC DMA transfer request during calibration */
    /* Note: Specificity of this STM32 series: Calibration factor is          */
    /*       available in data register and also transferred by DMA.          */
    /*       To not insert ADC calibration factor among ADC conversion data   */
    /*       in DMA destination address, DMA transfer must be disabled during */
    /*       calibration.                                                     */
    backup_setting_adc_dma_transfer = LL_ADC_REG_GetDMATransfer(ADCCTRL_HWADDR);
    LL_ADC_REG_SetDMATransfer(ADCCTRL_HWADDR, LL_ADC_REG_DMA_TRANSFER_NONE);

    /* Run ADC self calibration */
    LL_ADC_StartCalibration(ADCCTRL_HWADDR);

    /* Poll for ADC effectively calibrated */
    while (LL_ADC_IsCalibrationOnGoing(ADCCTRL_HWADDR) != 0);

    /* Restore ADC DMA transfer request after calibration */
    LL_ADC_REG_SetDMATransfer(ADCCTRL_HWADDR, backup_setting_adc_dma_transfer);

    /* Delay required between ADC end of calibration and ADC enable */
    /* LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES --> 2 cycles */
    __asm("mov r0, r0");
    __asm("mov r0, r0");

    /* Enable ADC */
    LL_ADC_Enable(ADCCTRL_HWADDR);

    /* Poll for ADC ready to convert */
    while (LL_ADC_IsActiveFlag_ADRDY(ADCCTRL_HWADDR) == 0);

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

  if(LL_ADC_IsEnabled(ADCCTRL_HWADDR))
  {
    /* Disable ADC */
    LL_ADC_Disable(ADCCTRL_HWADDR);

    /* Clear flag ADC ready */
    LL_ADC_ClearFlag_ADRDY(ADCCTRL_HWADDR);

    /* Wait until ADC_CR_ADEN bit is reset before turning off ADC internal regulator */
    while(LL_ADC_IsEnabled(ADCCTRL_HWADDR) == 1UL);

    /* Wait until ADC_CR_ADSTP bit is reset before turning off ADC internal regulator */
    while(LL_ADC_REG_IsStopConversionOngoing(ADCCTRL_HWADDR) == 1U);

    /* Disable ADC internal voltage regulator */
    LL_ADC_DisableInternalRegulator(ADCCTRL_HWADDR);
    while(LL_ADC_IsInternalRegulatorEnabled(ADCCTRL_HWADDR) == 1U);

    /* Peripharal clock disable */
    ADCCTRL_DISABLE_CLOCK ();
  }

  UTILS_EXIT_CRITICAL_SECTION();

  SYSTEM_DEBUG_SIGNAL_RESET(ADC_DEACTIVATION);
}

HAL_StatusTypeDef AdcConfigure (const ADCCTRL_Handle_t * const p_Handle)
{
  HAL_StatusTypeDef error = HAL_OK;

  /* No need to DeInit if the ADC if it is not yet initialized */
  if (HAL_ADC_STATE_RESET != ADCHandle.State)
  {
    /* DeInit the ADC module */
    error = HAL_ADC_DeInit(&ADCHandle);
  }

  /* All OK ? */
  if (HAL_OK == error)
  {
    /* Fulfill the configuration part */
    memcpy ((void *)&ADCHandle.Init,
            (void *)&p_Handle->InitConf,
            sizeof (ADC_InitTypeDef));

    /* Apply the requested ADC configuration */
    error = HAL_ADC_Init(&ADCHandle);

    if (HAL_OK == error)
    {
      /* Apply channel configuration */
      error = HAL_ADC_ConfigChannel(&ADCHandle,
                                    (ADC_ChannelConfTypeDef *)&p_Handle->ChannelConf);

      if (HAL_OK == error)
      {
        /* Update the current configuration */
        CurrentConfig = p_Handle->Uid;
      }
      else
      {
        /* There must be an issue with configuration, clean the configuration */
        memset ((void *)(&ADCHandle.Init),
                0x00,
                sizeof (ADC_InitTypeDef));
      }
    }
    else
    {
      /* There must be an issue with configuration, clean the configuration */
      memset ((void *)(&ADCHandle.Init),
              0x00,
              sizeof (ADC_InitTypeDef));
    }
  }

  return error;
}

uint16_t AdcReadRaw (const ADCCTRL_Handle_t * const p_Handle)
{
  /* Variables for ADC conversion data */
  __IO uint16_t uhADCxConvertedData = 0x00;

  /* Perform ADC group regular conversion start, poll for conversion        */
  /* completion.                                                            */
  ConversionStartPoll_ADC_GrpRegular ();

  /* Retrieve ADC conversion data */
  switch (p_Handle->InitConf.Resolution)
  {
    case LL_ADC_RESOLUTION_12B:
    {
      uhADCxConvertedData = VAR_CONVERTED_DATA_INIT_VALUE_12B;

      uhADCxConvertedData = LL_ADC_REG_ReadConversionData12(ADCCTRL_HWADDR);

      break;
    }
    case LL_ADC_RESOLUTION_10B:
    {
      uhADCxConvertedData = VAR_CONVERTED_DATA_INIT_VALUE_10B;

      uhADCxConvertedData = LL_ADC_REG_ReadConversionData10(ADCCTRL_HWADDR);

      break;
    }
    case LL_ADC_RESOLUTION_8B:
    {
      uhADCxConvertedData = VAR_CONVERTED_DATA_INIT_VALUE_8B;

      uhADCxConvertedData = LL_ADC_REG_ReadConversionData8(ADCCTRL_HWADDR);

      break;
    }
    case LL_ADC_RESOLUTION_6B:
    {
      uhADCxConvertedData = VAR_CONVERTED_DATA_INIT_VALUE_6B;

      uhADCxConvertedData = LL_ADC_REG_ReadConversionData6(ADCCTRL_HWADDR);

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

void ConversionStartPoll_ADC_GrpRegular(void)
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
  if ((LL_ADC_IsEnabled(ADCCTRL_HWADDR) == 1)               &&
      (LL_ADC_IsDisableOngoing(ADCCTRL_HWADDR) == 0)        &&
      (LL_ADC_REG_IsConversionOngoing(ADCCTRL_HWADDR) == 0)   )
  {
    LL_ADC_REG_StartConversion(ADCCTRL_HWADDR);
  }
  else
  {
    /* Error: ADC conversion start could not be performed */
    Error_Handler();
  }

  while (LL_ADC_IsActiveFlag_EOC(ADCCTRL_HWADDR) == 0);

  /* Clear flag ADC group regular end of unitary conversion */
  /* Note: This action is not needed here, because flag ADC group regular   */
  /*       end of unitary conversion is cleared automatically when          */
  /*       software reads conversion data from ADC data register.           */
  /*       Nevertheless, this action is done anyway to show how to clear    */
  /*       this flag, needed if conversion data is not always read          */
  /*       or if group injected end of unitary conversion is used (for      */
  /*       devices with group injected available).                          */
  LL_ADC_ClearFlag_EOC(ADCCTRL_HWADDR);
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
