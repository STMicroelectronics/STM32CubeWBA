
---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}

## <b>PWR_STOP2 Example Description</b> 

This example shows how to enter the system in STOP 2 mode and wake-up from this
mode using external RESET or wake-up interrupt.

In the associated software, the system clock is set to 100 MHz, an EXTI line
is connected to the user button through PC.13 and configured to generate an 
interrupt on falling edge upon key press.
The SysTick is programmed to generate an interrupt each 1 ms and in the SysTick 
interrupt handler, LD1 is toggled in order to indicate whether the MCU is in STOP 2 mode 
or RUN mode.

5 seconds after start-up, the system automatically enters STOP 2 mode and 
LD1 stops toggling.

The USER push-button can be pressed at any time to wake-up the system. 
The software then comes back in RUN mode for 5 sec. before automatically entering STOP 2 mode again. 

Two leds LD1 and LD3 are used to monitor the system state as following:

 - LD3 ON: configuration failed (system will go to an infinite loop)
 - LD1 toggling: system in RUN modes
 - LD1 off : system in STOP 2 mode

These steps are repeated in an infinite loop.

#### <b>Notes</b>
	  
 1- This example can not be used in DEBUG mode due to the fact 
      that the Cortex-M33 core is no longer clocked during low power mode 
      so debugging features are disabled.
	  
 2- Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
	  
 3- The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.
      

### <b>Keywords</b>

Power, PWR, Stop mode, Interrupt, EXTI, Wakeup, Low Power,

### <b>Directory contents</b>

    - PWR/PWR_STOP2/Inc/stm32wbaxx_nucleo_conf.h     BSP configuration file
    - PWR/PWR_STOP2/Inc/stm32wbaxx_conf.h            HAL Configuration file
    - PWR/PWR_STOP2/Inc/stm32wbaxx_it.h              Header for stm32wbaxx_it.c
    - PWR/PWR_STOP2/Inc/main.h                       Header file for main.c
    - PWR/PWR_STOP2/Src/system_stm32wbaxx.c          STM32WBAxx system clock configuration file
    - PWR/PWR_STOP2/Src/stm32wbaxx_it.c              Interrupt handlers
    - PWR/PWR_STOP2/Src/main.c                       Main program
    - PWR/PWR_STOP2/Src/stm32wbaxx_hal_msp.c         HAL MSP module

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBAxx devices

  - This example has been tested with STMicroelectronics NUCLEO-WBA65RI
    board and can be easily tailored to any other supported device 
    and development board.

  - NUCLEO-WBA65RI set-up:
    - Use LD1 and LD3 connected respectively to PD.08 and PB.08 pins
    - USER push-button connected to pin PC.13 (External line 13)

        
### <b>How to use it ?</b> 

In order to make the program work, you must do the following :

 - Open your preferred toolchain 
 
 - Rebuild all files and load your image into target memory
 
 - Run the example

 - Unplug then Plug STLINK connection to perform a power-on-reset

:::
:::

