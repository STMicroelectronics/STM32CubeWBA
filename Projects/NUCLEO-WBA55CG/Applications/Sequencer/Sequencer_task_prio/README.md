## <b>Sequencer_task_prio Example Description</b>

This example is based on the sequencer utility. It shows how the sequencer manages task priority.

Example configuration:

Required a GPIO configuration in output mode.

Example execution:

From the main program execution, the sequencer is initialized and three tasks are registered : TASK0, TASK1 and TASK2.

TASK0 : orders the execution of the TASK0 with low priority, the TASK1 with medium priority and TASK2 with the highest priority.
TASK1 ; toggles GPIO with a frequency of 0.5 Hz for a duration of 5 seconds
TASK2 : toggles GPIO with a frequency of 4 Hz for a duration of 5 secondss

The sequencer orders the execution of the tasks according to their priority.

### <b>Keywords</b>

UTILITY, sequencer

### <b>Directory contents</b>

  - Sequencer/Sequencer_task_prio/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
  - Sequencer/Sequencer_task_prio/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - Sequencer/Sequencer_task_prio/Inc/main.h                   Header for main.c module
  - Sequencer/Sequencer_task_prio/Src/stm32wbaxx_it.c          Interrupt handlers
  - Sequencer/Sequencer_task_prio/Src/stm32wbaxx_hal_msp.c     HAL MSP module
  - Sequencer/Sequencer_task_prio/Src/main.c                   Main program
  - Sequencer/Sequencer_task_prio/Src/system_stm32wbaxx.c      STM32WBAxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices.

  - This example has been tested with NUCLEO-WBA55CG board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

