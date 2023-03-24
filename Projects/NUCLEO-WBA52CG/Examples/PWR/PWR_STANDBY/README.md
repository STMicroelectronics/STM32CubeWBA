﻿## <b>PWR_STANDBY Example Description</b>

How to enter the Standby mode and wake up from this mode by using an external
reset or the WKUP pin.

In the associated software, the system clock is set to 100 MHz, an EXTI line
is connected to the USER push-button thrum PC.13 (D4 pin6 CN6) and configured to generate an
interrupt on falling edge.
The SysTick is programmed to generate an interrupt each 1 ms and in the SysTick
interrupt handler, LD1 is toggled in order to indicate whether the MCU is in Standby or Run mode.

When a falling edge is detected on the EXTI line, an interrupt is generated and the system wakes up
the program then checks and clears the standby flag.
After clearing the standby flag, the software enables wake-up pin PWR_WAKEUP_PIN2 connected to PC.13 (D4 pin6 CN6), then
the corresponding flag indicating that a wakeup event was received from the PWR_WAKEUP_PIN2 is cleared.
Finally, the system enters again Standby mode causing LD1 to stop toggling.

A falling edge on WKUP pin will wake-up the system from Standby.
Alternatively, an external RESET of the board will lead to a system wake-up as well.The application needs to ensure that the SysTick

After wake-up from Standby mode, program execution restarts in the same way as after
a RESET and LD1 restarts toggling.

Two leds LD1 and LD3 are used to monitor the system state as follows:

 - LD3 ON: configuration failed (system will go to an infinite loop)
 - LD1 toggling: system in Run mode
 - LD1 off : system in Standby mode

**Note :** To measure the current consumption in Standby mode, remove  jumper
      and connect an amperemeter to  to measure IDD current.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Power, PWR, Standby mode, Interrupt, EXTI, Wakeup, Low Power, External reset

### <b>Directory contents</b>

  - PWR/PWR_STANDBY/Inc/stm32wbaxx_nucleo_conf.h  BSP configuration file
  - PWR/PWR_STANDBY/Inc/stm32wbaxx_conf.h         HAL Configuration file
  - PWR/PWR_STANDBY/Inc/stm32wbaxx_it.h           Header for stm32wbaxx_it.c
  - PWR/PWR_STANDBY/Inc/main.h                    Header file for main.c
  - PWR/PWR_STANDBY/Src/system_stm32wbaxx.c       STM32WBAxx system clock configuration file
  - PWR/PWR_STANDBY/Src/stm32wbaxx_it.c           Interrupt handlers
  - PWR/PWR_STANDBY/Src/main.c                    Main program
  - PWR/PWR_STANDBY/Src/stm32wbaxx_hal_msp.c      HAL MSP module

### <b>Hardware and Software environment</b> 

  - This example runs on STM32WBA52CGUx devices

  - This example has been tested with STMicroelectronics NUCLEO-WBA52CG
    board and can be easily tailored to any other supported device
    and development board.

  - NUCLEO-WBA52CG Set-up
    - Use LD1 and LD3 connected respectively to PB.04 and PB.08 pins
    - USER push-button connected to pin PC.13 (External line 13)
    - WakeUp Pin PWR_WAKEUP_PIN2 connected to PC.13 (D4 pin6 CN6)

### <b>How to use it ?</b> 

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
