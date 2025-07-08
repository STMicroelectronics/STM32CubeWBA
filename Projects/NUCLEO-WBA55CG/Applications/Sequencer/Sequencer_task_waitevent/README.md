## <b>Sequencer_task_waitevent Example Description</b>

This example is based on the sequencer utility. It shows how the sequencer handles the task waiting for an event.

Example configuration:

Requires a GPIO configuration in output mode.

Example execution:

From the main program execution, the sequencer is initialized and three tasks are registered : TASK0,
TASK1 and TASK2.

TASK0 : waits 2 seconds, enables for execution all the tasks (TASK0, TASK1, TASK2)
TASK1 : executes a gpio toggle at a frequency TASK1_FREQ and generates an EVENT0
TASK2 : waits an EVENT0 and executes a gpio toggle at frequency TASK2_FREQ

The sequencer orders the execution of the tasks according to their priority. The TASK2 with the
highest priority starts its execution and waits for EVENT0. the TASK1 takes over and gives the
hand back to TASK2 by generating an EVENT0

### <b>Keywords</b>

Utility, sequencer, priority, task, event

### <b>Directory contents</b>

  - Sequencer/Sequencer_task_waitevent/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
  - Sequencer/Sequencer_task_waitevent/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - Sequencer/Sequencer_task_waitevent/Inc/main.h                   Header for main.c module
  - Sequencer/Sequencer_task_waitevent/Src/stm32wbaxx_it.c          Interrupt handlers
  - Sequencer/Sequencer_task_waitevent/Src/stm32wbaxx_hal_msp.c     HAL MSP module
  - Sequencer/Sequencer_task_waitevent/Src/main.c                   Main program
  - Sequencer/Sequencer_task_waitevent/Src/system_stm32wbaxx.c      STM32WBAxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices.

  - This example has been tested with NUCLEO-WBA55CG board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

