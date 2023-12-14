## <b>PWR_EnterStopMode Example Description</b>

How to enter the Stop 0 mode.

After start-up LD1 is toggling during 5 seconds, then the system automatically
enter in Stop 0 mode (Final state).

LD1 is used to monitor the system state as follows:

 - LD1 toggling : system in Run mode
 - LD1 off : system in Stop 0 mode

This example does not implement a wake-up source from any peripheral: to wake-up the device,
press on Reset button.

**Note :** This example can not be used in DEBUG mode due to the fact
that the Cortex-M33 core is no longer clocked during low power mode
so debugging features are disabled.

### <b>Keywords</b>

Power, PWR, Stop mode, Interrupt, Low Power

### <b>Directory contents</b>

  - PWR/PWR_EnterStopMode/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - PWR/PWR_EnterStopMode/Inc/main.h                   Header for main.c module
  - PWR/PWR_EnterStopMode/Inc/stm32_assert.h           Template file to include assert_failed function
  - PWR/PWR_EnterStopMode/Src/stm32wbaxx_it.c          Interrupt handlers
  - PWR/PWR_EnterStopMode/Src/main.c                   Main program
  - PWR/PWR_EnterStopMode/Src/system_stm32wbaxx.c      STM32WBAxx system source file

### <b>Hardware and Software environment</b> 

  - This example runs on STM32WBA55CGUx devices.

  - This example has been tested with STMicroelectronics NUCLEO-WBA55CG
    board and can be easily tailored to any other supported device
    and development board.

  - NUCLEO-WBA55CG Set-up
    - LD1 connected to PB.04 pin

### <b>How to use it ?</b> 

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

