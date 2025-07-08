## <b>Sequencer_gpio_toggle Example Description</b>

This example is based on the sequencer utilities. It shows how to use a sequencer task to toggle a GPIO.

Example configuration:

Required a GPIO configuration in output mode.

Example execution:

From the main program execution, the sequencer is initialized and a TASK0 is registered with the
function function_Task0 (this function toggle the GPIO). When a systick interrupt is generated,
the HAL_GetTick function is used to generate every 400ms a TASK0 event.

LD1 is used to show the gpio toggle

### <b>Keywords</b>

UTILITY, sequencer

### <b>Directory contents</b>

  - Sequencer/Sequencer_gpio_toggle/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
  - Sequencer/Sequencer_gpio_toggle/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - Sequencer/Sequencer_gpio_toggle/Inc/main.h                        Header for main.c module
  - Sequencer/Sequencer_gpio_toggle/Src/stm32wbaxx_it.c          Interrupt handlers
  - Sequencer/Sequencer_gpio_toggle/Src/stm32wbaxx_hal_msp.c     HAL MSP module
  - Sequencer/Sequencer_gpio_toggle/Src/main.c                        Main program
  - Sequencer/Sequencer_gpio_toggle/Src/system_stm32wbaxx.c      STM32WBAxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices.

  - This example has been tested with NUCLEO-WBA55CG board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

