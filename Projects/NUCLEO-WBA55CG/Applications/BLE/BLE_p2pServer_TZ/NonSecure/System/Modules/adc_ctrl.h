/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc_client.h
  * @author  MCD Application Team
  * @brief   Header for ADC client manager module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

#if (USE_TEMPERATURE_BASED_RADIO_CALIBRATION == 1)

#include <stdint.h>

/**
  * @brief  ADC clients definition
  */
typedef enum
{
  SYS_ADC_LL_EVT,   /* ADC request for Link Layer event (depending on radio activity) */
} adc_client_t;

/**
  * @brief  ADC IP state
  */
typedef enum
{
  ADC_OFF = 0x00,
  ADC_ON  = 0x01,
} adc_state_t;

/**
  * @brief  Initialize the adc client list
  * @param  None
  * @retval None
  */
void adc_ctrl_init(void);

/**
  * @brief  Manage ADC enable/disable requests
  * @param  client     client that requests an ADC state change
  * @param  enable     enable or disable request
  * @retval None
  */
void adc_ctrl_req(adc_client_t client, adc_state_t enable);

/**
  * @brief  Read temperature from ADC temperature sensor
  * @param  None
  * @retval Temperature measurement
  */
uint16_t adc_ctrl_request_temperature(void);

#endif /* USE_TEMPERATURE_BASED_RADIO_CALIBRATION */

#endif /* ADC_CTRL_H */
