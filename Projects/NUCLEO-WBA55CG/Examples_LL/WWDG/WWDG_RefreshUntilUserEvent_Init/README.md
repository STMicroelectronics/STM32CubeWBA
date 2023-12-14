## <b>WWDG_RefreshUntilUserEvent_Init Example Description</b>

Configuration of the WWDG to periodically update the counter and
generate an MCU WWDG reset when a user button is pressed. The peripheral initialization
uses the LL unitary service functions for optimization purposes (performance and size).

Example Configuration:

Configure the WWDG (Window, Prescaler & Counter) and enable it.
Refresh the WWDG downcounter in the main loop - LD1 is blinking fastly & continuously

Example Execution:

When USER push-button is pressed, the Downcounter automatic refresh mechanism is disable and thus, reset will occur.
After a reset when re-entering in the main, RCC WWDG Reset Flag will be checked and if we are back from a WWDG reset the LD1 will be switch ON.

Waiting a new USER push-button pressed to re-activate the WWDG

### <b>Keywords</b>

System, WWDG, Timeout, Refresh, Counter update, MCU Reset, Downcounter, Event, Window

### <b>Directory contents</b>

  - WWDG/WWDG_RefreshUntilUserEvent_Init/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - WWDG/WWDG_RefreshUntilUserEvent_Init/Inc/main.h                  Header for main.c module
  - WWDG/WWDG_RefreshUntilUserEvent_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - WWDG/WWDG_RefreshUntilUserEvent_Init/Src/stm32wbaxx_it.c          Interrupt handlers
  - WWDG/WWDG_RefreshUntilUserEvent_Init/Src/main.c                  Main program
  - WWDG/WWDG_RefreshUntilUserEvent_Init/Src/system_stm32wbaxx.c      STM32WBAxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices.

  - This example has been tested with NUCLEO-WBA55CG board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

