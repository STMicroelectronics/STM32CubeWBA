## <b>RCC_OutputSystemClockOnMCO Example Description</b>

Configuration of MCO pin (PA8) to output the system clock.

At start-up, USER push-button and MCO pin are configured. The program configures SYSCLK
to the max frequency using the PLL with HSE as clock source.

The signal on PA8 (pin 35 of CN3 connector) can be monitored with an oscilloscope
to check the different MCO configuration set at each USER push-button press.
Different configuration will be observed :

 - SYSCLK frequency with frequency value around @100MHZ.
 - HSI frequency with frequency value around @16MHz.
 - HSE frequency value , hence around @32MHZ.

When user press User push-button, a LD1 toggle is done to indicate a change in MCO config.

### <b>Keywords</b>

System, RCC, PLL, HSI, PLLCLK, SYSCLK, HSE, Clock, Oscillator


### <b>Directory contents</b>

  - RCC/RCC_OutputSystemClockOnMCO/Inc/stm32wbaxx_it.h         Interrupt handlers header file
  - RCC/RCC_OutputSystemClockOnMCO/Inc/main.h                  Header for main.c module
  - RCC/RCC_OutputSystemClockOnMCO/Inc/stm32_assert.h          Template file to include assert_failed function
  - RCC/RCC_OutputSystemClockOnMCO/Src/stm32wbaxx_it.c         Interrupt handlers
  - RCC/RCC_OutputSystemClockOnMCO/Src/main.c                  Main program
  - RCC/RCC_OutputSystemClockOnMCO/Src/system_stm32wbaxx.c     STM32WBAxx system source file


### <b>Hardware and Software environment</b> 

  - This example runs on STM32WBA52CGUx devices.

  - This example has been tested with NUCLEO-WBA52CG board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-WBA52CG Set-up
    - Connect the MCO pin to an oscilloscope to monitor the different waveforms:
      - PA.08: connected to pin 35 of CN3 connector

### <b>How to use it ?</b> 

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
