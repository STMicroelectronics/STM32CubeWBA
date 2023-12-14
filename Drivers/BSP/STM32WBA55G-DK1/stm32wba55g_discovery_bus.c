/**
  ******************************************************************************
  * @file    stm32wba55g_discovery_bus.c
  * @author  MCD Application Team
  * @brief   This file provides a set of firmware functions to communicate
  *          with  external devices available on STM32WBA55G_DK1 board
  *          from STMicroelectronics.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32wba55g_discovery_bus.h"
#include "stm32wba55g_discovery_errno.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32WBA55G_DK1
  * @{
  */

/** @defgroup STM32WBA55G_DK1_BUS BUS
  * @{
  */

/** @defgroup STM32WBA55G_DK1_BUS_Private_Constants BUS Private Constants
  * @{
  */
#ifndef I2C_VALID_TIMING_NBR
#define I2C_VALID_TIMING_NBR                 128U
#endif /* I2C_VALID_TIMING_NBR */

#define I2C_SPEED_FREQ_STANDARD                0U    /* 100 kHz */
#define I2C_SPEED_FREQ_FAST                    1U    /* 400 kHz */
#define I2C_SPEED_FREQ_FAST_PLUS               2U    /* 1 MHz */
#define I2C_ANALOG_FILTER_DELAY_MIN            50U   /* ns */
#define I2C_ANALOG_FILTER_DELAY_MAX            260U  /* ns */
#define I2C_USE_ANALOG_FILTER                  1U
#define I2C_DIGITAL_FILTER_COEF                0U
#define I2C_PRESC_MAX                          16U
#define I2C_SCLDEL_MAX                         16U
#define I2C_SDADEL_MAX                         16U
#define I2C_SCLH_MAX                           256U
#define I2C_SCLL_MAX                           256U
#define SEC2NSEC                               1000000000UL
/**
  * @}
  */

/** @defgroup STM32WBA55G_DK1_BUS_Private_Types BUS Private Types
  * @{
  */
typedef struct
{
  uint32_t freq;       /* Frequency in Hz */
  uint32_t freq_min;   /* Minimum frequency in Hz */
  uint32_t freq_max;   /* Maximum frequency in Hz */
  uint32_t hddat_min;  /* Minimum data hold time in ns */
  uint32_t vddat_max;  /* Maximum data valid time in ns */
  uint32_t sudat_min;  /* Minimum data setup time in ns */
  uint32_t lscl_min;   /* Minimum low period of the SCL clock in ns */
  uint32_t hscl_min;   /* Minimum high period of SCL clock in ns */
  uint32_t trise;      /* Rise time in ns */
  uint32_t tfall;      /* Fall time in ns */
  uint32_t dnf;        /* Digital noise filter coefficient */
} I2C_Charac_t;

typedef struct
{
  uint32_t presc;      /* Timing prescaler */
  uint32_t tscldel;    /* SCL delay */
  uint32_t tsdadel;    /* SDA delay */
  uint32_t sclh;       /* SCL high period */
  uint32_t scll;       /* SCL low period */
} I2C_Timings_t;
/**
  * @}
  */

/** @defgroup STM32WBA55G_DK1_BUS_Private_Constants BUS Private Constants
  * @{
  */
static const I2C_Charac_t I2C_Charac[] =
{
  [I2C_SPEED_FREQ_STANDARD] =
  {
    .freq = 100000,
    .freq_min = 80000,
    .freq_max = 120000,
    .hddat_min = 0,
    .vddat_max = 3450,
    .sudat_min = 250,
    .lscl_min = 4700,
    .hscl_min = 4000,
    .trise = 640,
    .tfall = 20,
    .dnf = I2C_DIGITAL_FILTER_COEF,
  },
  [I2C_SPEED_FREQ_FAST] =
  {
    .freq = 400000,
    .freq_min = 320000,
    .freq_max = 480000,
    .hddat_min = 0,
    .vddat_max = 900,
    .sudat_min = 100,
    .lscl_min = 1300,
    .hscl_min = 600,
    .trise = 250,
    .tfall = 100,
    .dnf = I2C_DIGITAL_FILTER_COEF,
  },
  [I2C_SPEED_FREQ_FAST_PLUS] =
  {
    .freq = 1000000,
    .freq_min = 800000,
    .freq_max = 1200000,
    .hddat_min = 0,
    .vddat_max = 450,
    .sudat_min = 50,
    .lscl_min = 500,
    .hscl_min = 260,
    .trise = 60,
    .tfall = 100,
    .dnf = I2C_DIGITAL_FILTER_COEF,
  },
};
/**
  * @}
  */

/** @defgroup STM32WBA55G_DK1_BUS_Private_Variables BUS Private Variables
  * @{
  */
#if (USE_HAL_I2C_REGISTER_CALLBACKS > 0)
static uint32_t IsI2c3MspCbValid = 0;
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */

static uint32_t      I2c3InitCounter = 0;
static I2C_Timings_t I2c_valid_timing[I2C_VALID_TIMING_NBR];
static uint32_t      I2c_valid_timing_nbr;
#if defined(BSP_USE_CMSIS_OS)
static osSemaphoreId BspI2cSemaphore = 0;
#endif /* BSP_USE_CMSIS_OS */
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
static uint32_t IsSpi1MspCbValid = 0;
#endif
/**
  * @}
  */

/** @defgroup STM32WBA55G_DK1_BUS_Exported_Variables BUS Exported Variables
  * @{
  */
I2C_HandleTypeDef hbus_i2c3;
SPI_HandleTypeDef hbus_spi3;

/**
  * @}
  */

/** @defgroup STM32WBA55G_DK1_BUS_Private_FunctionPrototypes BUS Private FunctionPrototypes
  * @{
  */
static void     I2C3_MspInit(I2C_HandleTypeDef *hI2c);
static void     I2C3_MspDeInit(I2C_HandleTypeDef *hI2c);
static int32_t  I2C3_WriteReg(uint16_t DevAddr, uint16_t MemAddSize, uint16_t Reg, uint8_t *pData, uint16_t Length);
static int32_t  I2C3_ReadReg(uint16_t DevAddr, uint16_t MemAddSize, uint16_t Reg, uint8_t *pData, uint16_t Length);

static uint32_t I2C_GetTiming(uint32_t clock_src_freq, uint32_t i2c_freq);
static uint32_t I2C_Compute_SCLL_SCLH(uint32_t clock_src_freq, uint32_t I2C_speed);
static void     I2C_Compute_PRESC_SCLDEL_SDADEL(uint32_t clock_src_freq, uint32_t I2C_speed);

static void SPI3_MspInit(SPI_HandleTypeDef* hspi);
static void SPI3_MspDeInit(SPI_HandleTypeDef* hspi);
static uint32_t SPI_GetPrescaler( uint32_t clk_src_hz, uint32_t baudfreq_mbps );
/**
  * @}
  */

/** @defgroup STM32WBA55G_DK1_BUS_Exported_Functions BUS Exported Functions
  * @{
  */

/**
  * @brief  Initializes I2C3 HAL.
  * @retval BSP status
  */
int32_t BSP_I2C3_Init(void)
{
  int32_t ret = BSP_ERROR_NONE;

  hbus_i2c3.Instance = BUS_I2C3;

  if (I2c3InitCounter == 0U)
  {
    I2c3InitCounter++;

    if (HAL_I2C_GetState(&hbus_i2c3) == HAL_I2C_STATE_RESET)
    {
#if defined(BSP_USE_CMSIS_OS)
      if (BspI2cSemaphore == NULL)
      {
        /* Create semaphore to prevent multiple I2C access */
        osSemaphoreDef(BSP_I2C_SEM);
        BspI2cSemaphore = osSemaphoreCreate(osSemaphore(BSP_I2C_SEM), 1);
      }
#endif /* BSP_USE_CMSIS_OS */
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 0)
      /* Init the I2C3 Msp */
      I2C3_MspInit(&hbus_i2c3);
#else
      if (IsI2c3MspCbValid == 0U)
      {
        if (BSP_I2C3_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
        {
          ret = BSP_ERROR_MSP_FAILURE;
        }
      }
      if (ret == BSP_ERROR_NONE)
      {
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */
      if (MX_I2C3_Init(&hbus_i2c3, I2C_GetTiming(HAL_RCC_GetPCLK1Freq(), BUS_I2C3_FREQUENCY)) != HAL_OK)
      {
        ret = BSP_ERROR_BUS_FAILURE;
      }
#if (USE_HAL_I2C_REGISTER_CALLBACKS > 0)
    }
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */
  }
}
return ret;
}

/**
  * @brief  DeInitializes I2C HAL.
  * @retval BSP status
  */
int32_t BSP_I2C3_DeInit(void)
{
  int32_t ret  = BSP_ERROR_NONE;

  if (I2c3InitCounter > 0U)
  {
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 0)
    I2C3_MspDeInit(&hbus_i2c3);
#endif /* (USE_HAL_I2C_REGISTER_CALLBACKS == 0) */

    /* Init the I2C */
    if (HAL_I2C_DeInit(&hbus_i2c3) != HAL_OK)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }

    I2c3InitCounter--;
  }

  return ret;
}

/**
  * @brief  MX I2C3 initialization.
  * @param  hI2c I2C handle
  * @param  timing I2C timing
  * @retval HAL status
  */
__weak HAL_StatusTypeDef MX_I2C3_Init(I2C_HandleTypeDef *hI2c, uint32_t timing)
{
  HAL_StatusTypeDef status = HAL_OK;

  hI2c->Init.Timing           = timing;
  hI2c->Init.OwnAddress1      = 0;
  hI2c->Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
  hI2c->Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
  hI2c->Init.OwnAddress2      = 0;
  hI2c->Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hI2c->Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
  hI2c->Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;

  if (HAL_I2C_Init(hI2c) != HAL_OK)
  {
    status = HAL_ERROR;
  }
  else
  {
    uint32_t analog_filter;

    analog_filter = I2C_ANALOGFILTER_ENABLE;
    if (HAL_I2CEx_ConfigAnalogFilter(hI2c, analog_filter) != HAL_OK)
    {
      status = HAL_ERROR;
    }
    else
    {
      if (HAL_I2CEx_ConfigDigitalFilter(hI2c, I2C_DIGITAL_FILTER_COEF) != HAL_OK)
      {
        status = HAL_ERROR;
      }
    }
  }

  return status;
}

/**
  * @brief  Write a 8bit value in a register of the device through BUS.
  * @param  DevAddr Device address on Bus.
  * @param  Reg    The target register address to write
  * @param  pData  The target register value to be written
  * @param  Length buffer size to be written
  * @retval BSP status
  */
int32_t BSP_I2C3_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  int32_t ret;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (I2C3_WriteReg(DevAddr, Reg, I2C_MEMADD_SIZE_8BIT, pData, Length) == 0)
  {
    ret = BSP_ERROR_NONE;
  }
  else
  {
    if (HAL_I2C_GetError(&hbus_i2c3) == HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret =  BSP_ERROR_PERIPH_FAILURE;
    }
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Read a 8bit register of the device through BUS
  * @param  DevAddr Device address on BUS
  * @param  Reg     The target register address to read
  * @param  pData   Pointer to data buffer
  * @param  Length  Length of the data
  * @retval BSP status
  */
int32_t BSP_I2C3_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  int32_t ret;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (I2C3_ReadReg(DevAddr, Reg, I2C_MEMADD_SIZE_8BIT, pData, Length) == 0)
  {
    ret = BSP_ERROR_NONE;
  }
  else
  {
    if (HAL_I2C_GetError(&hbus_i2c3) == HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret =  BSP_ERROR_PERIPH_FAILURE;
    }
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Write a 16bit value in a register of the device through BUS.
  * @param  DevAddr Device address on Bus.
  * @param  Reg    The target register address to write
  * @param  pData  The target register value to be written
  * @param  Length buffer size to be written
  * @retval BSP status
  */
int32_t BSP_I2C3_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  int32_t ret;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (I2C3_WriteReg(DevAddr, Reg, I2C_MEMADD_SIZE_16BIT, pData, Length) == 0)
  {
    ret = BSP_ERROR_NONE;
  }
  else
  {
    if (HAL_I2C_GetError(&hbus_i2c3) == HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret =  BSP_ERROR_PERIPH_FAILURE;
    }
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Read a 16bit register of the device through BUS
  * @param  DevAddr Device address on BUS
  * @param  Reg     The target register address to read
  * @param  pData   Pointer to data buffer
  * @param  Length  Length of the data
  * @retval BSP status
  */
int32_t BSP_I2C3_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  int32_t ret;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (I2C3_ReadReg(DevAddr, Reg, I2C_MEMADD_SIZE_16BIT, pData, Length) == 0)
  {
    ret = BSP_ERROR_NONE;
  }
  else
  {
    if (HAL_I2C_GetError(&hbus_i2c3) == HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret =  BSP_ERROR_PERIPH_FAILURE;
    }
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Checks if target device is ready for communication.
  * @note   This function is used with Memory devices
  * @param  DevAddr  Target device address
  * @param  Trials      Number of trials
  * @retval BSP status
  */
int32_t BSP_I2C3_IsReady(uint16_t DevAddr, uint32_t Trials)
{
  int32_t ret = BSP_ERROR_NONE;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (HAL_I2C_IsDeviceReady(&hbus_i2c3, DevAddr, Trials, 1000) != HAL_OK)
  {
    ret = BSP_ERROR_BUSY;
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Initializes SPI HAL.
  * @retval BSP status
  */
int32_t BSP_SPI3_Init(void)
{
  int32_t ret = BSP_ERROR_NONE;

  hbus_spi3.Instance  = BUS_SPI3_INSTANCE;

  if (HAL_SPI_GetState(&hbus_spi3) == HAL_SPI_STATE_RESET)
  {
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 0)
    /* Init the SPI Msp */
    SPI3_MspInit(&hbus_spi3);
#else
    if(IsSpi1MspCbValid == 0U)
    {
      if(BSP_SPI3_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
      {
        return BSP_ERROR_MSP_FAILURE;
      }
    }
#endif
    /* Init the SPI */
    if (MX_SPI3_Init(&hbus_spi3, SPI_GetPrescaler( HAL_RCC_GetPCLK7Freq(), BUS_SPI3_BAUDRATE)) != HAL_OK)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
  }

  return ret;
}

/**
  * @brief  DeInitializes SPI HAL.
  * @retval BSP status
  */
int32_t BSP_SPI3_DeInit(void)
{
  int32_t ret  = BSP_ERROR_NONE;

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 0)
  SPI3_MspDeInit(&hbus_spi3);
#endif
  /* DeInit the SPI*/
  if(HAL_SPI_DeInit(&hbus_spi3) == HAL_OK)
  {
    ret = BSP_ERROR_BUS_FAILURE;
  }

  return ret;
}

/**
  * @brief  Initializes MX SPI3 HAL.
  * @param  phspi             SPI handler
  * @param  BaudratePrescaler prsecaler to set for SPI baudrate
  * @retval HAL status
  */
__weak HAL_StatusTypeDef MX_SPI3_Init(SPI_HandleTypeDef* phspi, uint32_t BaudratePrescaler)
{
  HAL_StatusTypeDef ret = HAL_OK;

  phspi->Init.Mode                    = SPI_MODE_MASTER;
  phspi->Init.Direction               = SPI_DIRECTION_1LINE;
  phspi->Init.DataSize                = SPI_DATASIZE_8BIT;
  phspi->Init.CLKPolarity             = SPI_POLARITY_LOW;
  phspi->Init.CLKPhase                = SPI_PHASE_1EDGE;
  phspi->Init.NSS                     = SPI_NSS_SOFT;
  phspi->Init.BaudRatePrescaler       = SPI_BAUDRATEPRESCALER_8;
  phspi->Init.FirstBit                = SPI_FIRSTBIT_MSB;
  phspi->Init.TIMode                  = SPI_TIMODE_DISABLE;
  phspi->Init.CRCCalculation          = SPI_CRCCALCULATION_DISABLE;
  phspi->Init.CRCPolynomial           = 7;
  phspi->Init.NSSPMode                = SPI_NSS_PULSE_DISABLE;
  phspi->Init.NSSPolarity             = SPI_NSS_POLARITY_LOW;
  phspi->Init.FifoThreshold           = SPI_FIFO_THRESHOLD_01DATA;
  phspi->Init.MasterSSIdleness        = SPI_MASTER_SS_IDLENESS_00CYCLE;
  phspi->Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  phspi->Init.MasterReceiverAutoSusp  = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  phspi->Init.MasterKeepIOState       = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  phspi->Init.IOSwap                  = SPI_IO_SWAP_DISABLE;
  phspi->Init.ReadyMasterManagement   = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
  phspi->Init.ReadyPolarity           = SPI_RDY_POLARITY_HIGH;

  if(HAL_SPI_Init(phspi) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  return ret;
}

/**
  * @brief  Write Data through SPI BUS.
  * @param  pData  Pointer to data buffer to send
  * @param  Length Length of data in byte
  * @retval BSP status
  */
int32_t BSP_SPI3_Send(uint8_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_NONE;

  if(HAL_SPI_Transmit(&hbus_spi3, pData, Length, BUS_SPI3_TIMEOUT) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }
  return ret;
}

/**
  * @brief  Receive Data from SPI BUS
  * @param  pData  Pointer to data buffer to receive
  * @param  Length Length of data in byte
  * @retval BSP status
  */
int32_t  BSP_SPI3_Recv(uint8_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;
  uint32_t tx_data = 0xFFFFFFFFU;

  if(HAL_SPI_TransmitReceive(&hbus_spi3, (uint8_t*)&tx_data, pData, Length, BUS_SPI3_TIMEOUT) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }
  return ret;
}

/**
  * @brief  Send and Receive data to/from SPI BUS (Full duplex)
  * @param  pTxData  Pointer to data buffer to send
  * @param  pRxData  Pointer to data buffer to receive
  * @param  Length   Length of data in byte
  * @retval BSP status
  */
int32_t BSP_SPI3_SendRecv(uint8_t *pTxData, uint8_t *pRxData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(HAL_SPI_TransmitReceive(&hbus_spi3, pTxData, pRxData, Length, BUS_SPI3_TIMEOUT) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
  * @brief  Delay function
  * @retval Tick value
  */
int32_t BSP_GetTick(void)
{
  return (int32_t)HAL_GetTick();
}

#if (USE_HAL_I2C_REGISTER_CALLBACKS > 0)
/**
  * @brief Register Default I2C3 Bus Msp Callbacks
  * @retval BSP status
  */
int32_t BSP_I2C3_RegisterDefaultMspCallbacks(void)
{
  int32_t ret = BSP_ERROR_NONE;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  __HAL_I2C_RESET_HANDLE_STATE(&hbus_i2c3);

  /* Register default MspInit/MspDeInit Callback */
  if (HAL_I2C_RegisterCallback(&hbus_i2c3, HAL_I2C_MSPINIT_CB_ID, I2C3_MspInit) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else if (HAL_I2C_RegisterCallback(&hbus_i2c3, HAL_I2C_MSPDEINIT_CB_ID, I2C3_MspDeInit) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    IsI2c3MspCbValid = 1U;
  }

#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */
  /* BSP status */
  return ret;
}

/**
  * @brief Register I2C3 Bus Msp Callback registering
  * @param Callbacks     pointer to I2C3 MspInit/MspDeInit callback functions
  * @retval BSP status
  */
int32_t BSP_I2C3_RegisterMspCallbacks(BSP_I2C_Cb_t *Callback)
{
  int32_t ret = BSP_ERROR_NONE;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  __HAL_I2C_RESET_HANDLE_STATE(&hbus_i2c3);

  /* Register MspInit/MspDeInit Callbacks */
  if (HAL_I2C_RegisterCallback(&hbus_i2c3, HAL_I2C_MSPINIT_CB_ID, Callback->pMspI2cInitCb) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else if (HAL_I2C_RegisterCallback(&hbus_i2c3, HAL_I2C_MSPDEINIT_CB_ID, Callback->pMspI2cDeInitCb) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    IsI2c3MspCbValid = 1U;
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  /* BSP status */
  return ret;
}
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
/**
  * @brief Register Default SPI3 Bus Msp Callbacks
  * @retval BSP status
  */
int32_t BSP_SPI3_RegisterDefaultMspCallbacks (void)
{
  __HAL_SPI_RESET_HANDLE_STATE(&hbus_spi3);

  /* Register MspInit Callback */
  if(HAL_SPI_RegisterCallback(&hbus_spi3, HAL_SPI_MSPINIT_CB_ID, SPI3_MspInit) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }

  /* Register MspDeInit Callback */
  if(HAL_SPI_RegisterCallback(&hbus_spi3, HAL_SPI_MSPDEINIT_CB_ID, SPI3_MspDeInit) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }

  IsSpi1MspCbValid = 1;

  return BSP_ERROR_NONE;
}

/**
  * @brief Register SPI3 Bus Msp Callback registering
  * @param Callbacks     pointer to SPI3 MspInit/MspDeInit callback functions
  * @retval BSP status
  */
int32_t BSP_SPI3_RegisterMspCallbacks(BSP_SPI3_Cb_t *Callbacks)
{
  __HAL_SPI_RESET_HANDLE_STATE(&hbus_spi3);

  /* Register MspInit Callback */
  if(HAL_SPI_RegisterCallback(&hbus_spi3, HAL_SPI_MSPINIT_CB_ID, Callbacks->pMspSpiInitCb) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }

  /* Register MspDeInit Callback */
  if(HAL_SPI_RegisterCallback(&hbus_spi3, HAL_SPI_MSPDEINIT_CB_ID, Callbacks->pMspSpiDeInitCb) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }

  IsSpi1MspCbValid = 1;

  return BSP_ERROR_NONE;
}

#endif /* (USE_HAL_SPI_REGISTER_CALLBACKS == 1) */

/**
  * @brief  Initializes SPI MSP.
  * @param  hspi  SPI handler
  * @retval None
  */
static void SPI3_MspInit(SPI_HandleTypeDef* hspi)
{
  GPIO_InitTypeDef   GPIO_InitStructure;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(hspi);

  /* enable SCK GPIO clock */
  BUS_SPI3_SCK_GPIO_CLK_ENABLE();

  /* Enable MOSI GPIO clock */
  BUS_SPI3_MOSI_GPIO_CLK_ENABLE();

  /* configure SPIx SCK, MOSI */
  GPIO_InitStructure.Pin       = BUS_SPI3_SCK_PIN;
  GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull      = GPIO_PULLDOWN;
  GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_MEDIUM;
  GPIO_InitStructure.Alternate = BUS_SPI3_SCK_AF;
  HAL_GPIO_Init(BUS_SPI3_SCK_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.Pin       = BUS_SPI3_MOSI_PIN;
  GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull      = GPIO_PULLDOWN;
  GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_MEDIUM;
  GPIO_InitStructure.Alternate = BUS_SPI3_MOSI_AF;
  HAL_GPIO_Init(BUS_SPI3_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*** Configure the SPI peripheral ***/
  /* Enable SPI clock */
  BUS_SPI3_CLOCK_ENABLE();

  /* Force the SPI peripheral clock reset */
  BUS_SPI3_FORCE_RESET();

  /* Release the SPI peripheral clock reset */
  BUS_SPI3_RELEASE_RESET();
}

/**
  * @brief  DeInitializes SPI MSP.
  * @param  hspi  SPI handler
  * @retval None
  */
static void SPI3_MspDeInit(SPI_HandleTypeDef* hspi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hspi);

  GPIO_InitTypeDef gpio_init_structure;

  /* DeInitialize SPI SCK and MOSI  */
  gpio_init_structure.Pin = BUS_SPI3_SCK_PIN;
  HAL_GPIO_DeInit(BUS_SPI3_SCK_GPIO_PORT, gpio_init_structure.Pin);
  gpio_init_structure.Pin = BUS_SPI3_MOSI_PIN;
  HAL_GPIO_DeInit(BUS_SPI3_MOSI_GPIO_PORT, gpio_init_structure.Pin);

  /* Peripheral clock disable */
  __HAL_RCC_SPI3_CLK_DISABLE();
}

/**
  * @}
  */

/** @defgroup STM32WBA55G_DK1_BUS_Private_Functions BUS Private Functions
  * @{
  */
/**
  * @brief  Compute I2C timing according current I2C clock source and required I2C clock.
  * @param  clock_src_freq I2C clock source in Hz.
  * @param  i2c_freq Required I2C clock in Hz.
  * @retval I2C timing or 0 in case of error.
  */
static uint32_t I2C_GetTiming(uint32_t clock_src_freq, uint32_t i2c_freq)
{
  uint32_t ret = 0;
  uint32_t speed;
  uint32_t idx;

  I2c_valid_timing_nbr = 0;

  if ((clock_src_freq != 0U) && (i2c_freq != 0U))
  {
    for (speed = 0 ; speed <= (uint32_t)I2C_SPEED_FREQ_FAST_PLUS ; speed++)
    {
      if ((i2c_freq >= I2C_Charac[speed].freq_min) &&
          (i2c_freq <= I2C_Charac[speed].freq_max))
      {
        I2C_Compute_PRESC_SCLDEL_SDADEL(clock_src_freq, speed);
        idx = I2C_Compute_SCLL_SCLH(clock_src_freq, speed);

        if (idx < I2C_VALID_TIMING_NBR)
        {
          ret = ((I2c_valid_timing[idx].presc  & 0x0FU) << 28) | \
                ((I2c_valid_timing[idx].tscldel & 0x0FU) << 20) | \
                ((I2c_valid_timing[idx].tsdadel & 0x0FU) << 16) | \
                ((I2c_valid_timing[idx].sclh & 0xFFU) << 8) | \
                ((I2c_valid_timing[idx].scll & 0xFFU) << 0);
        }
        break;
      }
    }
  }

  return ret;
}

/**
  * @brief  Compute PRESC, SCLDEL and SDADEL.
  * @param  clock_src_freq I2C source clock in HZ.
  * @param  I2C_speed I2C frequency (index).
  * @retval None.
  */
static void I2C_Compute_PRESC_SCLDEL_SDADEL(uint32_t clock_src_freq, uint32_t I2C_speed)
{
  uint32_t prev_presc = I2C_PRESC_MAX;
  uint32_t ti2cclk;
  int32_t  tsdadel_min;
  int32_t  tsdadel_max;
  int32_t  tscldel_min;
  uint32_t presc;
  uint32_t scldel;
  uint32_t sdadel;
  uint32_t tafdel_min;
  uint32_t tafdel_max;

  ti2cclk   = (SEC2NSEC + (clock_src_freq / 2U)) / clock_src_freq;

  tafdel_min = I2C_ANALOG_FILTER_DELAY_MIN;
  tafdel_max = I2C_ANALOG_FILTER_DELAY_MAX;

  /* tDNF = DNF x tI2CCLK
     tPRESC = (PRESC+1) x tI2CCLK
     SDADEL >= {tf +tHD;DAT(min) - tAF(min) - tDNF - [3 x tI2CCLK]} / {tPRESC}
     SDADEL <= {tVD;DAT(max) - tr - tAF(max) - tDNF- [4 x tI2CCLK]} / {tPRESC} */

  tsdadel_min = (int32_t)I2C_Charac[I2C_speed].tfall + (int32_t)I2C_Charac[I2C_speed].hddat_min -
                (int32_t)tafdel_min - (int32_t)(((int32_t)I2C_Charac[I2C_speed].dnf + 3) * (int32_t)ti2cclk);

  tsdadel_max = (int32_t)I2C_Charac[I2C_speed].vddat_max - (int32_t)I2C_Charac[I2C_speed].trise -
                (int32_t)tafdel_max - (int32_t)(((int32_t)I2C_Charac[I2C_speed].dnf + 4) * (int32_t)ti2cclk);


  /* {[tr+ tSU;DAT(min)] / [tPRESC]} - 1 <= SCLDEL */
  tscldel_min = (int32_t)I2C_Charac[I2C_speed].trise + (int32_t)I2C_Charac[I2C_speed].sudat_min;

  if (tsdadel_min <= 0)
  {
    tsdadel_min = 0;
  }

  if (tsdadel_max <= 0)
  {
    tsdadel_max = 0;
  }

  for (presc = 0; presc < I2C_PRESC_MAX; presc++)
  {
    for (scldel = 0; scldel < I2C_SCLDEL_MAX; scldel++)
    {
      /* TSCLDEL = (SCLDEL+1) * (PRESC+1) * TI2CCLK */
      uint32_t tscldel = (scldel + 1U) * (presc + 1U) * ti2cclk;

      if (tscldel >= (uint32_t)tscldel_min)
      {
        for (sdadel = 0; sdadel < I2C_SDADEL_MAX; sdadel++)
        {
          /* TSDADEL = SDADEL * (PRESC+1) * TI2CCLK */
          uint32_t tsdadel = (sdadel * (presc + 1U)) * ti2cclk;

          if ((tsdadel >= (uint32_t)tsdadel_min) && (tsdadel <= (uint32_t)tsdadel_max))
          {
            if ((presc != prev_presc) && (I2c_valid_timing_nbr <= I2C_VALID_TIMING_NBR))
            {
              I2c_valid_timing[I2c_valid_timing_nbr].presc = presc;
              I2c_valid_timing[I2c_valid_timing_nbr].tscldel = scldel;
              I2c_valid_timing[I2c_valid_timing_nbr].tsdadel = sdadel;
              prev_presc = presc;
              I2c_valid_timing_nbr ++;

              if (I2c_valid_timing_nbr >= I2C_VALID_TIMING_NBR)
              {
                return;
              }
            }
          }
        }
      }
    }
  }
}

/**
  * @brief  Calculate SCLL and SCLH and find best configuration.
  * @param  clock_src_freq I2C source clock in HZ.
  * @param  I2C_speed I2C frequency (index).
  * @retval config index (0 to I2C_VALID_TIMING_NBR], 0xFFFFFFFF for no valid config.
  */
static uint32_t I2C_Compute_SCLL_SCLH(uint32_t clock_src_freq, uint32_t I2C_speed)
{
  uint32_t ret = 0xFFFFFFFFU;
  uint32_t ti2cclk;
  uint32_t ti2cspeed;
  uint32_t prev_error;
  uint32_t dnf_delay;
  uint32_t clk_min;
  uint32_t clk_max;
  uint32_t scll;
  uint32_t sclh;
  uint32_t tafdel_min;

  ti2cclk   = (SEC2NSEC + (clock_src_freq / 2U)) / clock_src_freq;
  ti2cspeed   = (SEC2NSEC + (I2C_Charac[I2C_speed].freq / 2U)) / I2C_Charac[I2C_speed].freq;

  tafdel_min = I2C_ANALOG_FILTER_DELAY_MIN;

  /* tDNF = DNF x tI2CCLK */
  dnf_delay = I2C_Charac[I2C_speed].dnf * ti2cclk;

  clk_max = SEC2NSEC / I2C_Charac[I2C_speed].freq_min;
  clk_min = SEC2NSEC / I2C_Charac[I2C_speed].freq_max;

  prev_error = ti2cspeed;

  for (uint32_t count = 0; count < I2c_valid_timing_nbr; count++)
  {
    /* tPRESC = (PRESC+1) x tI2CCLK*/
    uint32_t tpresc = (I2c_valid_timing[count].presc + 1U) * ti2cclk;

    for (scll = 0; scll < I2C_SCLL_MAX; scll++)
    {
      /* tLOW(min) <= tAF(min) + tDNF + 2 x tI2CCLK + [(SCLL+1) x tPRESC ] */
      uint32_t tscl_l = tafdel_min + dnf_delay + (2U * ti2cclk) + ((scll + 1U) * tpresc);


      /* The I2CCLK period tI2CCLK must respect the following conditions:
      tI2CCLK < (tLOW - tfilters) / 4 and tI2CCLK < tHIGH */
      if ((tscl_l > I2C_Charac[I2C_speed].lscl_min) && (ti2cclk < ((tscl_l - tafdel_min - dnf_delay) / 4U)))
      {
        for (sclh = 0; sclh < I2C_SCLH_MAX; sclh++)
        {
          /* tHIGH(min) <= tAF(min) + tDNF + 2 x tI2CCLK + [(SCLH+1) x tPRESC] */
          uint32_t tscl_h = tafdel_min + dnf_delay + (2U * ti2cclk) + ((sclh + 1U) * tpresc);

          /* tSCL = tf + tLOW + tr + tHIGH */
          uint32_t tscl = tscl_l + tscl_h + I2C_Charac[I2C_speed].trise + I2C_Charac[I2C_speed].tfall;

          if ((tscl >= clk_min) && (tscl <= clk_max) && (tscl_h >= I2C_Charac[I2C_speed].hscl_min)
              && (ti2cclk < tscl_h))
          {
            int32_t error = (int32_t)tscl - (int32_t)ti2cspeed;

            if (error < 0)
            {
              error = -error;
            }

            /* look for the timings with the lowest clock error */
            if ((uint32_t)error < prev_error)
            {
              prev_error = (uint32_t)error;
              I2c_valid_timing[count].scll = scll;
              I2c_valid_timing[count].sclh = sclh;
              ret = count;
            }
          }
        }
      }
    }
  }

  return ret;
}

/**
  * @brief  Initializes I2C MSP.
  * @param  hI2c  I2C handler
  * @retval None
  */
static void I2C3_MspInit(I2C_HandleTypeDef *hI2c)
{
  GPIO_InitTypeDef  gpio_init_structure;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(hI2c);

  /*** Configure the GPIOs ***/
  /* Enable SCL GPIO clock */
  BUS_I2C3_SCL_GPIO_CLK_ENABLE();
  /* Enable SDA GPIO clock */
  BUS_I2C3_SDA_GPIO_CLK_ENABLE();

  /* Configure I2C Tx as alternate function */
  gpio_init_structure.Pin               = BUS_I2C3_SCL_PIN;
  gpio_init_structure.Mode              = GPIO_MODE_AF_OD;
  gpio_init_structure.Pull              = GPIO_PULLUP;
  gpio_init_structure.Speed             = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Alternate         = BUS_I2C3_SCL_AF;
  HAL_GPIO_Init(BUS_I2C3_SCL_GPIO_PORT, &gpio_init_structure);

  /* Configure I2C Rx as alternate function */
  gpio_init_structure.Pin               = BUS_I2C3_SDA_PIN;
  gpio_init_structure.Mode              = GPIO_MODE_AF_OD;
  gpio_init_structure.Pull              = GPIO_PULLUP;
  gpio_init_structure.Speed             = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Alternate         = BUS_I2C3_SDA_AF;
  HAL_GPIO_Init(BUS_I2C3_SDA_GPIO_PORT, &gpio_init_structure);

  /*** Configure the I2C peripheral ***/
  /* Enable I2C clock */
  BUS_I2C3_CLK_ENABLE();

  /* Force the I2C peripheral clock reset */
  BUS_I2C3_FORCE_RESET();

  /* Release the I2C peripheral clock reset */
  BUS_I2C3_RELEASE_RESET();
}

/**
  * @brief  DeInitializes I2C MSP.
  * @param  hI2c  I2C handler
  * @retval None
  */
static void I2C3_MspDeInit(I2C_HandleTypeDef *hI2c)
{
  GPIO_InitTypeDef  gpio_init_structure;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(hI2c);

  /* Configure I2C Tx, Rx as alternate function */
  gpio_init_structure.Pin = BUS_I2C3_SCL_PIN;
  HAL_GPIO_DeInit(BUS_I2C3_SCL_GPIO_PORT, gpio_init_structure.Pin);
  gpio_init_structure.Pin = BUS_I2C3_SDA_PIN;
  HAL_GPIO_DeInit(BUS_I2C3_SDA_GPIO_PORT, gpio_init_structure.Pin);

  /* Disable I2C clock */
  BUS_I2C3_CLK_DISABLE();
}

/**
  * @brief  Write a value in a register of the device through BUS.
  * @param  DevAddr    Device address on Bus.
  * @param  Reg        The target register address to write
  * @param  MemAddSize Size of internal memory address
  * @param  pData      The target register value to be written
  * @param  Length     data length in bytes
  * @retval BSP status
  */
static int32_t I2C3_WriteReg(uint16_t DevAddr, uint16_t Reg, uint16_t MemAddSize, uint8_t *pData, uint16_t Length)
{
  if (HAL_I2C_Mem_Write(&hbus_i2c3, DevAddr, Reg, MemAddSize, pData, Length, 10000) == HAL_OK)
  {
    return BSP_ERROR_NONE;
  }

  return BSP_ERROR_BUS_FAILURE;
}

/**
  * @brief  Read a register of the device through BUS
  * @param  DevAddr    Device address on BUS
  * @param  Reg        The target register address to read
  * @param  MemAddSize Size of internal memory address
  * @param  pData      The target register value to be written
  * @param  Length     data length in bytes
  * @retval BSP status
  */
static int32_t I2C3_ReadReg(uint16_t DevAddr, uint16_t Reg, uint16_t MemAddSize, uint8_t *pData, uint16_t Length)
{
  if (HAL_I2C_Mem_Read(&hbus_i2c3, DevAddr, Reg, MemAddSize, pData, Length, 10000) == HAL_OK)
  {
    return BSP_ERROR_NONE;
  }

  return BSP_ERROR_BUS_FAILURE;
}

/**
  * @brief  Convert the SPI baudfreq into tpresc.
  * @param  clock_src_freq : SPI source clock in HZ.
  * @param  baudfreq_mbps : SPI baud freq in mbps.
  * @retval Prescaler divisor
  */
static uint32_t SPI_GetPrescaler( uint32_t clock_src_freq, uint32_t baudfreq_mbps )
{
  uint32_t divisor = 0;
  uint32_t spi_clk = clock_src_freq;
  uint32_t presc = 0;

  static const uint32_t baudfreq[]=
  {
    SPI_BAUDRATEPRESCALER_2,
    SPI_BAUDRATEPRESCALER_4,
    SPI_BAUDRATEPRESCALER_8,
    SPI_BAUDRATEPRESCALER_16,
    SPI_BAUDRATEPRESCALER_32,
    SPI_BAUDRATEPRESCALER_64,
    SPI_BAUDRATEPRESCALER_128,
    SPI_BAUDRATEPRESCALER_256,
  };

  while( spi_clk > baudfreq_mbps)
  {
    presc = baudfreq[divisor];
    if (++divisor > 7U)
      break;

    spi_clk= ( spi_clk >> 1);
  }

  return presc;
}

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
