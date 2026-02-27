---
pagetitle: Readme
lang: en
---

::: {.row}
::: {.col-sm-12 .col-lg-8}

## <b>PWR_STOP1_RTC Example Description</b>

How to enter the STOP 1 mode and wake-up from this mode by using the RTC wakeup timer.
It allows to measure the current consumption in STOP 1 mode with RTC enabled.

In the associated software, the system clock is set to 100 MHz and the SysTick is
programmed to generate an interrupt each 1 ms.
The Low Speed Internal (LSI) clock is used as RTC clock source by default.

The system automatically enters STOP 1 mode 5 sec. after start-up.
The RTC wake-up is configured to generate an interrupt on rising edge about 10 sec. afterwards.
Current consumption in STOP 1 mode with RTC feature enabled can be measured during that time.
The software then comes back in run mode for 5 sec. before automatically entering stop 1 mode again.

LD3 is used to monitor the system state as following:

- LD3 toggling: system in RUN mode
- LD3 off : system in STOP 1 mode

These steps are repeated in an infinite loop.

Note: This example can not be used in DEBUG mode due to the fact
      that the Cortex-M33 core is no longer clocked during low power mode
      so debugging features are disabled.

### **Notes**

 1. Care must be taken when HAL_RCCEx_PeriphCLKConfig() is used to select
    the RTC clock source; in this case the Backup domain will be reset in
    order to modify the RTC Clock source, as consequence RTC registers (including
    the backup registers) and RCC_CSR register are set to their reset values.

 2. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 3. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Power, PWR, STOP1 mode, Interrupt, EXTI, Wakeup, Low Power, RTC, LSI,

### **Directory contents**

- PWR/PWR_STOP1_RTC/Inc/stm32wba55g_discovery_conf.h     BSP configuration file
- PWR/PWR_STOP1_RTC/Inc/stm32wbaxx_conf.h         HAL Configuration file
- PWR/PWR_STOP1_RTC/stm32wbaxx_it.h           Header for stm32wbaxx_it.c
- PWR/PWR_STOP1_RTC/Inc/main.h                         Header file for main.c
- PWR/PWR_STOP1_RTC/Src/system_stm32wbaxx.c       STM32WBAxx system clock configuration file
- PWR/PWR_STOP1_RTC/Src/stm32wbaxx_it.c           Interrupt handlers
- PWR/PWR_STOP1_RTC/Src/main.c                         Main program
- PWR/PWR_STOP1_RTC/Src/stm32wbaxx_hal_msp.c      HAL MSP module

### **Hardware and Software environment**

- This example runs on STM32WBA55CGUx devices

- This example has been tested with STMicroelectronics STM32WBA55G-DK1
    board and can be easily tailored to any other supported device
    and development board.

- STM32WBA55G-DK1 Set-up
  - LD3 connected to PA.00 or PB.15 pin (IO used is dependent of board revision)

### **How to use it ?**

In order to make the program work, you must do the following :

- Open your preferred toolchain
- Rebuild all files and load your image into target memory
- Run the example

:::
:::
