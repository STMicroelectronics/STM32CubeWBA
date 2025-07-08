## <b>Sequencer_gpio_toggle_lowpower Example Description</b>

This example is based on the sequencer utilities.
It shows how to use the sequecer IDLE task to manage application low power mode.

Example configuration:

Require a GPIO configuration in output mode.

Example execution:

From the main program execution, the sequencer is initialized and a TASK0 is registered with the
function function_Task0 (this function toggles the GPIO). Additionally, initialized an LPTIM to
manage the enabling of the TASK0 so when an LPTIM interrupt occurs a TASK0 execution is required.
The function UTIL_SEQ_Idle enters STOP mode when the sequencer has nothing to do.

### <b>Keywords</b>

UTILITY, sequencer, low power, PWR

### <b>Directory contents</b>

  - Sequencer/Sequencer_gpio_toggle_lowpower/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
  - Sequencer/Sequencer_gpio_toggle_lowpower/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - Sequencer/Sequencer_gpio_toggle_lowpower/Inc/main.h                  Header for main.c module
  - Sequencer/Sequencer_gpio_toggle_lowpower/Src/stm32wbaxx_it.c          Interrupt handlers
  - Sequencer/Sequencer_gpio_toggle_lowpower/Src/stm32wbaxx_hal_msp.c     HAL MSP module
  - Sequencer/Sequencer_gpio_toggle_lowpower/Src/main.c                  Main program
  - Sequencer/Sequencer_gpio_toggle_lowpower/Src/system_stm32wbaxx.c      STM32WBAxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices.

  - This example has been tested with NUCLEO-WBA55CG board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

