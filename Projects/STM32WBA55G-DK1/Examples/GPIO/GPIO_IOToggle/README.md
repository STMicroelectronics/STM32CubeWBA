
---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}

## <b>GPIO_IOToggle Example Description</b>

How to configure and use GPIOs through the HAL API.

PA.00 or PB.15 IO (configured in output pushpull mode) toggles in a forever loop.
On STM32WBA55G-DK1 board this IO is connected to LD3 (IO used is linked to board revision).

In this example, HCLK is configured at 100 MHz.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

System, GPIO, Input, Output, Alternate function, Push-pull, Toggle

### <b>Directory contents</b>

    - GPIO/GPIO_IOToggle/Inc/stm32wba55g_discovery_conf.h     BSP configuration file
    - GPIO/GPIO_IOToggle/Inc/stm32wbaxx_hal_conf.h            HAL configuration file
    - GPIO/GPIO_IOToggle/Inc/stm32wbaxx_it.h                  Interrupt handlers header file
    - GPIO/GPIO_IOToggle/Inc/main.h                           Header for main.c module  
    - GPIO/GPIO_IOToggle/Src/stm32wbaxx_it.c                  Interrupt handlers
    - GPIO/GPIO_IOToggle/Src/stm32wbaxx_hal_msp.c             HAL MSP file
    - GPIO/GPIO_IOToggle/Src/main.c                           Main program
    - GPIO/GPIO_IOToggle/Src/system_stm32wbaxx.c              STM32WBAxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices.

  - This example has been tested with STM32WBA55G-DK1 board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


:::
:::

