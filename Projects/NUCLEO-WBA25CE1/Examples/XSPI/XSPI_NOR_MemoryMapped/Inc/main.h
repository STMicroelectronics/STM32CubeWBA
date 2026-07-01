/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wbaxx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32wbaxx_nucleo.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define XSPI1_BASE_MM              0x90000000U

/* MX25R3235F Macronix memory */
/* Size of the flash */
#define XSPI_FLASH_SIZE             12U
#define XSPI_PAGE_SIZE              256U

/* Flash commands */
#define FOUR_IO_READ_CMD           0xEBU
#define QUAD_PAGE_PROG_CMD         0x38U
#define READ_STATUS_REG_CMD        0x05U
#define WRITE_STATUS_REGISTER      0x01U
#define READ_CONF_REG_CMD          0x15U
#define WRITE_ENABLE_CMD           0x06U
#define SECTOR_ERASE_CMD           0x20U


/* Dummy clocks cycles */
#define DUMMY_CLOCK_CYCLES_READ_REG 0U
#define DUMMY_CLOCK_CYCLES_4READ    6U

/* Memory registers bit definition */
#define SR_QE_BIT              0x40U
#define SR_WIP_BIT             0x01U
#define SR_WEL_BIT             0x02U

/* Memory delay */
#define MEMORY_PAGE_PROG_DELAY      10U

/* End address of the XSPI memory */
#define XSPI_END_ADDR               (1U << XSPI_FLASH_SIZE)

/* Size of buffers */
#define BUFFERSIZE                  (COUNTOF(aTxBuffer) - 1U)
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define COUNTOF(__BUFFER__)         (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
