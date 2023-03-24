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
#include "main.h"
#include "scm.h"
#include "stm32_lpm_if.h"
#include "stm32_lpm.h"
#include "stm32wbaxx_hal_pwr.h"
#include "RTDebug.h"
#include "stm32wbaxx_ll_icache.h"
#include "stm32wbaxx.h"
#include "utilities_common.h"
#include "cmsis_compiler.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables -----------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
const struct UTIL_LPM_Driver_s UTIL_PowerDriver =
{
  PWR_EnterSleepMode,
  PWR_ExitSleepMode,

  PWR_EnterStopMode,
  PWR_ExitStopMode,

  PWR_EnterOffMode,
  PWR_ExitOffMode,
};

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* Variable to store the MainStackPointer before entering standby wfi */
uint32_t backup_MSP;
static uint32_t boot_after_standby;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Private function prototypes -----------------------------------------------*/
static void Standby_Restore_GPIO(void);
static void Enter_Stop_Standby_Mode(void);
static void Exit_Stop_Standby_Mode(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

static void Standby_Restore_GPIO(void)
{
  uint32_t temp;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  // ---------------------------------------------------------------------------

  *(uint32_t *)0x42020000 = 0xA8000000;  // Configure GPIOA_MODER 15:13 AF (JTAG), 12:0 In  STANDBY MESURMENTS OK

  __ASM("mov r0, r0"); // Delay to allow GPIOx_IDR.IDy to be updated after GPIOx_MODER is set to Input
  __ASM("mov r0, r0"); // Delay to allow GPIOx_IDR.IDy to be updated after GPIOx_MODER is set to Input

  temp = LL_GPIO_ReadInputPort(GPIOA);
  LL_GPIO_WriteOutputPort(GPIOA, temp); // Restore Port A output drive levels

  /* GPIOA_MODER set to reset value */
  *(uint32_t *)0x42020000 = 0xABFFFFFF;

  // ---------------------------------------------------------------------------

  *(uint32_t *)0x42020400 = 0x00000280;  // Configure GPIOB_MODER 4:3 AF (JTAG), 15:5, 2:0 In

  __ASM("mov r0, r0"); // Delay to allow GPIOx_IDR.IDy to be updated after GPIOx_MODER is set to Input
  __ASM("mov r0, r0"); // Delay to allow GPIOx_IDR.IDy to be updated after GPIOx_MODER is set to Input

  temp = LL_GPIO_ReadInputPort(GPIOB);
  LL_GPIO_WriteOutputPort(GPIOB, temp); // Restore Port B output drive levels

  /* GPIOB_MODER set to reset value */
  *(uint32_t *)0x42020400 = 0xFFFFFEBF;

  // ---------------------------------------------------------------------------

  *(uint32_t *)0x42020800 = 0x00000000;  // Configure GPIOC_MODER 15:13 In

  __ASM("mov r0, r0"); // Delay to allow GPIOx_IDR.IDy to be updated after GPIOx_MODER is set to Input
  __ASM("mov r0, r0"); // Delay to allow GPIOx_IDR.IDy to be updated after GPIOx_MODER is set to Input

  temp = LL_GPIO_ReadInputPort(GPIOC);
  LL_GPIO_WriteOutputPort(GPIOC, temp); // Restore Port C output drive levels

  /* GPIOC_MODER set to reset value */
  *(uint32_t *)0x42020800 = 0xFC000000;

  // ---------------------------------------------------------------------------

  *(uint32_t *)0x42021C00 = 0x00000000;  // Configure GPIOH_MODER 3 In

  __ASM("mov r0, r0"); // Delay to allow GPIOx_IDR.IDy to be updated after GPIOx_MODER is set to Input
  __ASM("mov r0, r0"); // Delay to allow GPIOx_IDR.IDy to be updated after GPIOx_MODER is set to Input

  temp = LL_GPIO_ReadInputPort(GPIOH);
  LL_GPIO_WriteOutputPort(GPIOH, temp); // Restore Port H output drive levels

  /* GPIOH_MODER set to reset value */
  *(uint32_t *)0x42021C00 = 0x0000C000;

  // ---------------------------------------------------------------------------
}

static void Enter_Stop_Standby_Mode(void)
{
      /* Disabling ICACHE */
  LL_ICACHE_Disable();

  /* Wait until ICACHE_SR.BUSYF is cleared */
  while(LL_ICACHE_IsActiveFlag_BUSY() == 1U);

  /* Wait until ICACHE_SR.BSYENDF is set */
  while(LL_ICACHE_IsActiveFlag_BSYEND() == 0U);

  scm_setwaitstates(LP);

  LL_LPM_EnableDeepSleep();

  while(LL_PWR_IsActiveFlag_ACTVOS( ) == 0);
}

static void Exit_Stop_Standby_Mode(void)
{
    LL_ICACHE_Enable();
    while(LL_ICACHE_IsEnabled() == 0U);

    if (LL_PWR_IsActiveFlag_STOP() == 1U)
    {
      scm_setup();
    }
    else
    {
      scm_setwaitstates( RUN );
    }
}

void PWR_EnterOffMode( void )
{
  SYSTEM_DEBUG_SIGNAL_SET(LOW_POWER_STANDBY_MODE_ENTER);

  /* USER CODE BEGIN PWR_EnterOffMode_1 */

  /* USER CODE END PWR_EnterOffMode_1 */

  /*
   * There is no risk to clear all the WUF here because in the current implementation, this API is called
   * in critical section. If an interrupt occurs while in that critical section before that point,
   * the flag is set and will be cleared here but the system will not enter Off Mode
   * because an interrupt is pending in the NVIC. The ISR will be executed when moving out
   * of this critical section
   */

  Enter_Stop_Standby_Mode();

  /* Set low power mode to standby */
  LL_PWR_SetPowerMode( LL_PWR_MODE_STANDBY );

  /* This option is used to ensure that store operations are completed */
#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  __force_stores();
#endif /*(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) */

  /* Save all Cortex registers on stack and call WFI instruction */
  CPUcontextSave();
  SYS_WAITING_CYCLES_25();
  /* USER CODE BEGIN PWR_EnterOffMode_2 */

  /* USER CODE END PWR_EnterOffMode_2 */

  SYSTEM_DEBUG_SIGNAL_RESET(LOW_POWER_STANDBY_MODE_ENTER);
  SYSTEM_DEBUG_SIGNAL_SET(LOW_POWER_STANDBY_MODE_ACTIVE);
}

void PWR_ExitOffMode( void )
{
  SYSTEM_DEBUG_SIGNAL_RESET(LOW_POWER_STANDBY_MODE_ACTIVE);
  SYSTEM_DEBUG_SIGNAL_SET(LOW_POWER_STANDBY_MODE_EXIT);

  /* USER CODE BEGIN PWR_ExitOffMode_1 */

  /* USER CODE END PWR_ExitOffMode_1 */

  if( 1UL == boot_after_standby )
  {
    boot_after_standby = 0;

    HAL_NVIC_SetPriority(RADIO_INTR_NUM, RADIO_INTR_PRIO_LOW, 0);
    HAL_NVIC_EnableIRQ(RADIO_INTR_NUM);
    HAL_NVIC_SetPriority(RADIO_SW_LOW_INTR_NUM, RADIO_SW_LOW_INTR_PRIO, 0);
    HAL_NVIC_EnableIRQ(RADIO_SW_LOW_INTR_NUM);
    HAL_NVIC_SetPriority(RCC_IRQn, RCC_INTR_PRIO, 0);
    HAL_NVIC_EnableIRQ(RCC_IRQn);
    HAL_NVIC_SetPriority(RTC_IRQn, 0x07, 0);
    HAL_NVIC_EnableIRQ(RTC_IRQn);

    /*
     ***********************************
     * Restore SoC HW configuration
     ***********************************
     */
    Standby_Restore_GPIO();
    MX_GPIO_Init();

    /* RNG */
    MX_RNG_Init();

    /* Restore ICACHE */
    MX_ICACHE_Init();

    /* Restore system clock configuration */
    scm_standbyexit();

    /* Enable RTC peripheral clock */
    LL_PWR_EnableBkUpAccess();
    __HAL_RCC_RTCAPB_CLK_ENABLE();
  }
  else
  {
    Exit_Stop_Standby_Mode();
  }

  /* USER CODE BEGIN PWR_ExitOffMode_2 */

  /* USER CODE END PWR_ExitOffMode_2 */

  /* Important note: at this point, all the IOs configuration is done */

  /* Clear all IOs retention status  */
  HAL_PWREx_DisableStandbyRetainedIOState(PWR_GPIO_A, PWR_GPIO_PIN_MASK);
  HAL_PWREx_DisableStandbyRetainedIOState(PWR_GPIO_B, PWR_GPIO_PIN_MASK);
  HAL_PWREx_DisableStandbyRetainedIOState(PWR_GPIO_C, PWR_GPIO_PIN_MASK);
  HAL_PWREx_DisableStandbyRetainedIOState(PWR_GPIO_H, PWR_GPIO_PIN_MASK);

  SYSTEM_DEBUG_SIGNAL_RESET(LOW_POWER_STANDBY_MODE_EXIT);
}

void PWR_EnterStopMode( void )
{
  SYSTEM_DEBUG_SIGNAL_SET(LOW_POWER_STOP_MODE_ENTER);

  /* USER CODE BEGIN PWR_EnterStopMode_1 */

  /* USER CODE END PWR_EnterStopMode_1 */

  Enter_Stop_Standby_Mode();

  LL_PWR_SetPowerMode(LL_PWR_MODE_STOP1);

  __WFI( );
  SYS_WAITING_CYCLES_25();

  /* USER CODE BEGIN PWR_EnterStopMode_2 */

  /* USER CODE END PWR_EnterStopMode_2 */

  SYSTEM_DEBUG_SIGNAL_RESET(LOW_POWER_STOP_MODE_ENTER);
  SYSTEM_DEBUG_SIGNAL_SET(LOW_POWER_STOP_MODE_ACTIVE);
}

void PWR_ExitStopMode( void )
{
  SYSTEM_DEBUG_SIGNAL_RESET(LOW_POWER_STOP_MODE_ACTIVE);
  SYSTEM_DEBUG_SIGNAL_SET(LOW_POWER_STOP_MODE_EXIT);

  /* USER CODE BEGIN PWR_ExitStopMode_1 */

  /* USER CODE END PWR_ExitStopMode_1 */

  Exit_Stop_Standby_Mode();

  /* USER CODE BEGIN PWR_ExitStopMode_2 */

  /* USER CODE END PWR_ExitStopMode_2 */

  SYSTEM_DEBUG_SIGNAL_RESET(LOW_POWER_STOP_MODE_EXIT);
}

void PWR_EnterSleepMode( void )
{
  /* USER CODE BEGIN PWR_EnterSleepMode_1 */

  /* USER CODE END PWR_EnterSleepMode_1 */

  LL_LPM_EnableSleep();
  __WFI();

  /* USER CODE BEGIN PWR_EnterSleepMode_2 */

  /* USER CODE END PWR_EnterSleepMode_2 */
}

void PWR_ExitSleepMode( void )
{
  /* USER CODE BEGIN PWR_ExitSleepMode */

  /* USER CODE END PWR_ExitSleepMode */
}

static uint32_t standby_cnt = 0; // Debug only
uint32_t is_boot_from_standby(void)
{
#if (CFG_DBG_SUPPORTED == 0)
  LL_DBGMCU_DisableDBGStopMode();
  LL_DBGMCU_DisableDBGStandbyMode();
#endif

  __HAL_RCC_PWR_CLK_ENABLE();

  LL_PWR_EnableUltraLowPowerMode();
  __HAL_FLASH_SLEEP_POWERDOWN_ENABLE();

  standby_cnt++;

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

__weak void enter_standby_notification(void)
{
  /* User code here */
}

__weak void exit_standby_notification(void)
{
  /* User code here */
}

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

