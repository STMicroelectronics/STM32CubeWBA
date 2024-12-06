/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32_lpm_if.c
  * @author  MCD Application Team
  * @brief   Low layer function to enter/exit low power modes (stop, sleep)
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
#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#include "arm_compat.h"
#endif /*(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) */
#include "stm32_lpm_if.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* Variable to store the MainStackPointer before entering standby wfi */
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

uint32_t is_boot_from_standby(void)
{
  uint32_t boot_after_standby;
  
  __HAL_RCC_PWR_CLK_ENABLE();

  LL_PWR_EnableUltraLowPowerMode();
  __HAL_FLASH_SLEEP_POWERDOWN_ENABLE();

  /* Ensure this is a return from Standby, and not a reset */
  if( (LL_PWR_IsActiveFlag_SB() == 1UL ) &&
      (READ_REG(RCC->CSR) == 0U)
    )
  {
    /* When exit from standby, disable IRQ so that restore and PWR_ExitOffMode are in critical section */
    __disable_irq( );

    boot_after_standby = 1;
  }
  else
  {
    boot_after_standby = 0;
  }

  return boot_after_standby;
}

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

