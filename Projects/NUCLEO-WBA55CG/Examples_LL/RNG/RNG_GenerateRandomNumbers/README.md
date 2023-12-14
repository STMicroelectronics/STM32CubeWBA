## <b>RNG_GenerateRandomNumbers Example Description</b>

Configuration of the RNG to generate 32-bit long random numbers.

The peripheral initialization uses LL unitary service
functions for optimization purposes (performance and size).

Example execution:
After startup from reset and system configuration, RNG configuration is performed.
PLL1Q  is selected as RNG clock source.

User is then asked to press USER push-button (LD1 blinking fast).
On USER push-button press, several (8) Random 32bit numbers are generated
(DRDY flag is polled until 1, indicating a random number has been generated and could be retrieved from DR register).
Corresponding generated values are available and stored in a u32 array (aRandom32bit),
whose content could be displayed using debugger (Watch or LiveWatch features).
After successful Random numbers generation, LD1 is turned On.
In case of errors, LD1 is slowly blinking (1sec period).

### <b>Keywords</b>

Analog, RNG, Random, FIPS PUB 140-2, Analog Random number generator, Entropy, Period


### <b>Directory contents</b>

  - RNG/RNG_GenerateRandomNumbers/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - RNG/RNG_GenerateRandomNumbers/Inc/main.h                   Header for main.c module
  - RNG/RNG_GenerateRandomNumbers/Inc/stm32_assert.h           Template file to include assert_failed function
  - RNG/RNG_GenerateRandomNumbers/Src/stm32wbaxx_it.c          Interrupt handlers
  - RNG/RNG_GenerateRandomNumbers/Src/main.c                   Main program
  - RNG/RNG_GenerateRandomNumbers/Src/system_stm32wbaxx.c      STM32WBAxx system source file


### <b>Hardware and Software environment</b> 

  - This example runs on STM32WBA55CGUx devices.

  - This example has been tested with NUCLEO-WBA55CG board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 - Push USER push-button and use Variable watch window from debugger to access to values of generated numbers.
   (A break point could be set on LED_On() call, at end of RandomNumbersGeneration() function).
