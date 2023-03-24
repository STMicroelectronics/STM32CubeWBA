## <b>Tiny_lpm_3modes Example Description</b>

This example is based on the tiny lpm utility. It shows how the sequencer handles the task waiting for an event.

Example configuration:

Requires an RTC to exit lowpower mode.

Example execution:

From the main program execution, the tiny lpm is initialized and order the execution of one of the three lowpower 
modes : Sleep, Stop and OFF. The actions to enter the different power mode are managed in the file stm32_lpm_if.c, 
which is defined by the application and used by the utility to enter the allowed low power mode. The UTIL_LPM_SetOffMode 
and UTIL_LPM_SetStopMode functions are used to allow/prohibit modes and tiny lpm enter the authorized mode based 
on this information.

### <b>Keywords</b>

UTILITY, power, sleep, stop, standby, low power, WFI

### <b>Directory contents</b>

  - LPM/Tiny_lpm_3modes/Inc/main.h                   Header for main.c module
  - LPM/Tiny_lpm_3modes/Src/stm32_lpm_if.h           Header lowpower if
  - LPM/Tiny_lpm_3modes/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
  - LPM/Tiny_lpm_3modes/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - LPM/Tiny_lpm_3modes/Inc/stm32wbaxx_nucleo_conf.h header for the board configuration
  - LPM/Tiny_lpm_3modes/Inc/utilities_conf.h         header for utility configuration
  - LPM/Tiny_lpm_3modes/Src/main.c                   Main program
  - LPM/Tiny_lpm_3modes/Src/stm32_lpm_if.c           function to manage the enter/exti of the lowpwer mode
  - LPM/Tiny_lpm_3modes/Src/stm32wbaxx_hal_msp.c     HAL MSP module
  - LPM/Tiny_lpm_3modes/Src/stm32wbaxx_it.c          Interrupt handlers
  - LPM/Tiny_lpm_3modes/Src/system_stm32wbaxx.c      STM32WBAxx system source file
  

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA52CGUx devices.

  - This example has been tested with NUCLEO-WBA52CG board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

