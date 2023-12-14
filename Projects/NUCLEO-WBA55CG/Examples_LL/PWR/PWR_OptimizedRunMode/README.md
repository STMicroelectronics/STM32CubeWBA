## <b>PWR_OptimizedRunMode Example Description</b>

How to increase/decrease frequency and VCORE and how to enter/exit the
Low-power run mode.

In the associated software, the system clock is set to 100MHz, an EXTI line
is connected to the USER push-button through PC.13 and configured to generate an
interrupt on falling edge upon key press.

After start-up LD1 is toggling FAST (100ms blinking period), indicates that device
is running at 100MHz.

LD1 toggling speed is controlled by variable "uhLedBlinkSpeed".

The USER push-button can be pressed at any time to change Frequency, VCore(VOS)
and Low Power Run mode.

Initial STATE:
--> Freq: 100MHz, VCore 1.2V, Core in Run Mode
--> LD1 toggling FAST (100ms)   - wait USER push-button action

STATE 2:
USER push-button pressed:
--> Freq: 32MHz, VCore 1.0V, Core in Run Mode
--> LD1 toggling MEDIUM (200ms) - wait USER push-button action

STATE 3:
USER push-button pressed:
--> Freq:  1000KHz, VCore 1.0V, Core in Low Power Run Mode
--> LD1 toggling SLOW (400ms)   - wait USER push-button action

STATE 4:
USER push-button pressed:
--> Freq: 32MHz, VCore 1.0V, Core in Run Mode
--> LD1 toggling MEDIUM (200ms) - wait USER push-button action

Final STATE:
USER push-button pressed:
--> Freq: 100MHz, VCore 1.2V, Core in Run Mode
--> LD1 toggling FAST (100ms) in infinite loop


      NB: LD1 has an impact on power consumption.
          Remove LD1 blinking to have a constant power consumption,
          comment line  "#define USE_LED" in main.c file

**Note:** This example may not be used in debug mode depending on IDE and debugger
          configuration selected, due to stsem low frequency and low power mode
          constraints.

### <b>Keywords</b>

Power, PWR, Low-power run mode, Interrupt, VCORE, Low Power

### <b>Directory contents</b>

  - PWR/PWR_OptimizedRunMode/Inc/stm32wbaxx_it.h         Interrupt handlers header file
  - PWR/PWR_OptimizedRunMode/Inc/main.h                  Header for main.c module
  - PWR/PWR_OptimizedRunMode/Inc/stm32_assert.h          Template file to include assert_failed function
  - PWR/PWR_OptimizedRunMode/Src/stm32wbaxx_it.c         Interrupt handlers
  - PWR/PWR_OptimizedRunMode/Src/main.c                  Main program
  - PWR/PWR_OptimizedRunMode/Src/system_stm32wbaxx.c     STM32WBAxx system source file


### <b>Hardware and Software environment</b>
  - This example runs on STM32WBA55CGUx devices.

  - This example has been tested with STMicroelectronics NUCLEO-WBA55CG
    board and can be easily tailored to any other supported device
    and development board.

  - NUCLEO-WBA55CG Set-up
    - LD1 connected to pin PB.04
    - User push-button connected to pin PC.13

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

