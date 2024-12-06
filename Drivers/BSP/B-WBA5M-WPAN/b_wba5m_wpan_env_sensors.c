/**
  ******************************************************************************
  * @file    b_wba5m_wpan_env_sensors.c
  * @author  MCD Application Team
  * @brief   This file provides a set of functions needed to manage the environmental sensors
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
#include "b_wba5m_wpan_bus.h"
#include "b_wba5m_wpan_env_sensors.h"

/** @addtogroup BSP BSP
  * @{
  */

/** @addtogroup B_WBA5M_WPAN B_WBA5M_WPAN
  * @{
  */

/** @defgroup B_WBA5M_WPAN_ENV_SENSORS B_WBA5M_WPAN ENV SENSORS
  * @{
  */

/** @defgroup B_WBA5M_WPAN_ENV_SENSORS_Exported_Variables B_WBA5M_WPAN ENV SENSORS Exported Variables
  * @{
  */

extern void *EnvCompObj[ENV_SENSOR_INSTANCES_NBR]; /* This "redundant" line is here to fulfil MISRA C-2012 rule 8.4 */
void *EnvCompObj[ENV_SENSOR_INSTANCES_NBR];

/**
  * @}
  */

/** @defgroup B_WBA5M_WPAN_ENV_SENSORS_Private_Variables B_WBA5M_WPAN ENV SENSORS Private Variables
  * @{
  */

/* We define a jump table in order to get the correct index from the desired function. */
/* This table should have a size equal to the maximum value of a function plus 1.      */
/* But due to MISRA it has to be increased to 15 + 1. */
static uint32_t FunctionIndex[] = {0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3};
static ENV_SENSOR_FuncDrv_t *EnvFuncDrv[ENV_SENSOR_INSTANCES_NBR][ENV_SENSOR_FUNCTIONS_NBR];
static ENV_SENSOR_CommonDrv_t *EnvDrv[ENV_SENSOR_INSTANCES_NBR];
static ENV_SENSOR_Ctx_t EnvCtx[ENV_SENSOR_INSTANCES_NBR];

/**
  * @}
  */

/** @defgroup B_WBA5M_WPAN_ENV_SENSORS_Private_Function_Prototypes B_WBA5M_WPAN ENV SENSORS Private Function Prototypes
  * @{
  */

#if (USE_ENV_SENSOR_SHT40AD1B_0 == 1U)
static int32_t SHT40AD1B_0_Probe(uint32_t Functions);
#endif /* USE_ENV_SENSOR_SHT40AD1B_0 == 1U */

/**
  * @}
  */

/** @defgroup B_WBA5M_WPAN_ENV_SENSORS_Exported_Functions B_WBA5M_WPAN ENV SENSOR Exported Functions
  * @{
  */

/**
  * @brief  Initializes the environmental sensor
  * @param  Instance environmental sensor instance to be used
  * @param  Functions Environmental sensor functions. Could be :
  *         - ENV_TEMPERATURE
  *         - ENV_PRESSURE
  *         - ENV_HUMIDITY
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_Init(uint32_t Instance, uint32_t Functions)
{
  int32_t ret = BSP_ERROR_NONE;
  uint32_t function = ENV_TEMPERATURE;
  uint32_t i;
  uint32_t component_functions = 0;
  ENV_SENSOR_Capabilities_t cap;

  switch (Instance)
  {
#if (USE_ENV_SENSOR_SHT40AD1B_0 == 1U)
    case ENV_SENSOR_SHT40AD1B_0:
      if (SHT40AD1B_0_Probe(Functions) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_NO_INIT;
      }
      if (EnvDrv[Instance]->GetCapabilities(EnvCompObj[Instance], (void *)&cap) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_UNKNOWN_COMPONENT;
      }
      if (cap.Temperature == 1U)
      {
        component_functions |= ENV_TEMPERATURE;
      }
      if (cap.Humidity == 1U)
      {
        component_functions |= ENV_HUMIDITY;
      }
      if (cap.Pressure == 1U)
      {
        component_functions |= ENV_PRESSURE;
      }
      break;
#endif /* USE_ENV_SENSOR_SHT40AD1B_0 == 1U */

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  if (ret != BSP_ERROR_NONE)
  {
    return ret;
  }

  for (i = 0; i < ENV_SENSOR_FUNCTIONS_NBR; i++)
  {
    if (((Functions & function) == function) && ((component_functions & function) == function))
    {
      if (EnvFuncDrv[Instance][FunctionIndex[function]]->Enable(EnvCompObj[Instance]) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_COMPONENT_FAILURE;
      }
    }
    function = function << 1;
  }

  return ret;
}

/**
  * @brief  Deinitialize environmental sensor
  * @param  Instance environmental sensor instance to be used
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_DeInit(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= ENV_SENSOR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (EnvDrv[Instance]->DeInit(EnvCompObj[Instance]) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
  * @brief  Get environmental sensor instance capabilities
  * @param  Instance Environmental sensor instance
  * @param  Capabilities pointer to Environmental sensor capabilities
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_GetCapabilities(uint32_t Instance, ENV_SENSOR_Capabilities_t *Capabilities)
{
  int32_t ret;

  if (Instance >= ENV_SENSOR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (EnvDrv[Instance]->GetCapabilities(EnvCompObj[Instance], Capabilities) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
  * @brief  Get WHOAMI value
  * @param  Instance environmental sensor instance to be used
  * @param  Id WHOAMI value
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_ReadID(uint32_t Instance, uint8_t *Id)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= ENV_SENSOR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (EnvDrv[Instance]->ReadID(EnvCompObj[Instance], Id) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
  * @brief  Enable environmental sensor
  * @param  Instance environmental sensor instance to be used
  * @param  Function Environmental sensor function. Could be :
  *         - ENV_TEMPERATURE
  *         - ENV_PRESSURE
  *         - ENV_HUMIDITY
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_Enable(uint32_t Instance, uint32_t Function)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= ENV_SENSOR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((EnvCtx[Instance].Functions & Function) == Function)
    {
      if (EnvFuncDrv[Instance][FunctionIndex[Function]]->Enable(EnvCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

/**
  * @brief  Disable environmental sensor
  * @param  Instance environmental sensor instance to be used
  * @param  Function Environmental sensor function. Could be :
  *         - ENV_TEMPERATURE
  *         - ENV_PRESSURE
  *         - ENV_HUMIDITY
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_Disable(uint32_t Instance, uint32_t Function)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= ENV_SENSOR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((EnvCtx[Instance].Functions & Function) == Function)
    {
      if (EnvFuncDrv[Instance][FunctionIndex[Function]]->Disable(EnvCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

/**
  * @brief  Get environmental sensor Output Data Rate
  * @param  Instance environmental sensor instance to be used
  * @param  Function Environmental sensor function. Could be :
  *         - ENV_TEMPERATURE
  *         - ENV_PRESSURE
  *         - ENV_HUMIDITY
  * @param  Odr pointer to Output Data Rate read value
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float *Odr)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= ENV_SENSOR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((EnvCtx[Instance].Functions & Function) == Function)
    {
      if (EnvFuncDrv[Instance][FunctionIndex[Function]]->GetOutputDataRate(EnvCompObj[Instance], Odr) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

/**
  * @brief  Set environmental sensor Output Data Rate
  * @param  Instance environmental sensor instance to be used
  * @param  Function Environmental sensor function. Could be :
  *         - ENV_TEMPERATURE
  *         - ENV_PRESSURE
  *         - ENV_HUMIDITY
  * @param  Odr Output Data Rate value to be set
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float Odr)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= ENV_SENSOR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((EnvCtx[Instance].Functions & Function) == Function)
    {
      if (EnvFuncDrv[Instance][FunctionIndex[Function]]->SetOutputDataRate(EnvCompObj[Instance], Odr) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

/**
  * @brief  Get environmental sensor value
  * @param  Instance environmental sensor instance to be used
  * @param  Function Environmental sensor function. Could be :
  *         - ENV_TEMPERATURE
  *         - ENV_PRESSURE
  *         - ENV_HUMIDITY
  * @param  Value pointer to environmental sensor value
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_GetValue(uint32_t Instance, uint32_t Function, float *Value)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= ENV_SENSOR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((EnvCtx[Instance].Functions & Function) == Function)
    {
      if (EnvFuncDrv[Instance][FunctionIndex[Function]]->GetValue(EnvCompObj[Instance], Value) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

/**
  * @}
  */

/** @defgroup B_WBA5M_WPAN_ENV_SENSORS_Private_Functions B_WBA5M_WPAN ENV SENSORS Private Functions
  * @{
  */

#if (USE_ENV_SENSOR_SHT40AD1B_0 == 1U)
/**
  * @brief  Register Bus IOs for SHT40AD1B instance
  * @param  Functions Environmental sensor functions. Could be :
  *         - ENV_TEMPERATURE and/or ENV_HUMIDITY
  * @retval BSP status
  */
static int32_t SHT40AD1B_0_Probe(uint32_t Functions)
{
  SHT40AD1B_IO_t            io_ctx;
  uint8_t                   id;
  int32_t                   ret = BSP_ERROR_NONE;
  static SHT40AD1B_Object_t sht40ad1b_obj_0;
  SHT40AD1B_Capabilities_t  cap;

  /* Configure the driver */
  io_ctx.BusType     = SHT40AD1B_I2C_BUS; /* I2C */
  io_ctx.Address     = SHT40AD1B_I2C_ADDRESS;
  io_ctx.Init        = B_WBA5M_WPAN_I2C_Init;
  io_ctx.DeInit      = B_WBA5M_WPAN_I2C_DeInit;
  io_ctx.Read        = B_WBA5M_WPAN_I2C_Read;
  io_ctx.Write       = B_WBA5M_WPAN_I2C_Write;
  io_ctx.GetTick     = B_WBA5M_WPAN_GetTick;
  io_ctx.Delay       = HAL_Delay;

  if (SHT40AD1B_RegisterBusIO(&sht40ad1b_obj_0, &io_ctx) != SHT40AD1B_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (SHT40AD1B_ReadID(&sht40ad1b_obj_0, &id) != SHT40AD1B_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (id != SHT40AD1B_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void)SHT40AD1B_GetCapabilities(&sht40ad1b_obj_0, &cap);

    EnvCtx[ENV_SENSOR_SHT40AD1B_0].Functions = ((uint32_t)cap.Temperature)
                                         | ((uint32_t)cap.Pressure << 1)
                                         | ((uint32_t)cap.Humidity << 2);

    EnvCompObj[ENV_SENSOR_SHT40AD1B_0] = &sht40ad1b_obj_0;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    EnvDrv[ENV_SENSOR_SHT40AD1B_0] = (ENV_SENSOR_CommonDrv_t *)(void *)&SHT40AD1B_COMMON_Driver;

    if ((ret == BSP_ERROR_NONE) && ((Functions & ENV_TEMPERATURE) == ENV_TEMPERATURE) && (cap.Temperature == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      EnvFuncDrv[ENV_SENSOR_SHT40AD1B_0][FunctionIndex[ENV_TEMPERATURE]] = (ENV_SENSOR_FuncDrv_t *)(void *)&SHT40AD1B_TEMP_Driver;

      if (EnvDrv[ENV_SENSOR_SHT40AD1B_0]->Init(EnvCompObj[ENV_SENSOR_SHT40AD1B_0]) != SHT40AD1B_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if ((ret == BSP_ERROR_NONE) && ((Functions & ENV_HUMIDITY) == ENV_HUMIDITY) && (cap.Humidity == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      EnvFuncDrv[ENV_SENSOR_SHT40AD1B_0][FunctionIndex[ENV_HUMIDITY]] = (ENV_SENSOR_FuncDrv_t *)(void *)&SHT40AD1B_HUM_Driver;

      if (EnvDrv[ENV_SENSOR_SHT40AD1B_0]->Init(EnvCompObj[ENV_SENSOR_SHT40AD1B_0]) != SHT40AD1B_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if ((ret == BSP_ERROR_NONE) && ((Functions & ENV_PRESSURE) == ENV_PRESSURE))
    {
      /* Return an error if the application try to initialize a function not supported by the component */
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}
#endif /* USE_ENV_SENSOR_SHT40AD1B_0 == 1U */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
