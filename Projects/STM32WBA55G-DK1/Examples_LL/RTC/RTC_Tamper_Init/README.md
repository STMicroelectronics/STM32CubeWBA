## <b>RTC_Tamper_Init Example Description</b>

Configuration of the Tamper using the RTC LL API. The peripheral initialization
uses LL unitary service functions for optimization purposes (performance and size).

In this example, after start-up, SYSCLK is configured to the max frequency using the PLL with
HSE as clock source.

The RTC peripheral configuration is ensured by the Configure_RTC() function
(configure of the needed RTC resources according to the used hardware CLOCK,
PWR, RTC clock source and BackUp). You may update this function to change RTC configuration.

The associated firmware performs the following:

1. It configures the Tamper pin to be rising edge, and enables the Tamper
   interrupt.

2. It writes the data to all RTC Backup data registers, then check whether the
   data were correctly written. If yes, LD3 toggles with a fast period,
   otherwise LD3 toggles with a period of 1s.

3. Applying a low level on the tamper 4 (pin PC13 (pin5 CN10))
   the RTC backup data registers are reset and the Tamper interrupt is generated.
   The corresponding ISR then checks whether the RTC Backup data registers are cleared.
   If yes LD3 turns on, otherwise LD3 toggles with a period of 1s.

**Note :** On this board, the tamper 4 pin is connected to User push-button (connection to pin PC.13 (pin5 CN10)).

### <b>Keywords</b>

System, RTC, Tamper, Reset, LSE, LSI

### <b>Directory contents</b>

  - RTC/RTC_Tamper_Init/Inc/stm32wbaxx_it.h     Interrupt handlers header file
  - RTC/RTC_Tamper_Init/Inc/main.h             Header for main.c module
  - RTC/RTC_Tamper_Init/Inc/stm32_assert.h     Template file to include assert_failed function
  - RTC/RTC_Tamper_Init/Src/stm32wbaxx_it.c     Interrupt handlers
  - RTC/RTC_Tamper_Init/Src/main.c             Main program
  - RTC/RTC_Tamper_Init/Src/system_stm32wbaxx.c STM32WBAxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices.

  - This example has been tested with STM32WBA55G-DK1 board and can be
    easily tailored to any other supported device and development board.


### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


