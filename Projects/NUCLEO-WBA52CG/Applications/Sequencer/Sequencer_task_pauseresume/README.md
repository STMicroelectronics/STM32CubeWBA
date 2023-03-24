## <b>Sequencer_task_pauseresume Example Description</b>

This example is based on the sequencer utilities. It shows how the sequencer handles the task pause/resume mechanism.

Example configuration:

Required a GPIO configuration in output mode.

Example execution:

From the main program execution, the sequencer is initialized and three tasks are registered : TASK0, TASK1 and TASK2.

TASK0 : waits 2 seconds, enables for execution all tasks (TASK0, TASK1, TASK2) and puts TASK2 in pause
(means the task is freeze and the sequencer discard the task execution)
TASK1 : executes a gpio toggle with a frequency of TASK1_FREQ ms and resumes TASK2 (allow sequencer to
execute the task)
TASK2 : executes a gpio toggle with a frequency of TASK2_FREQ ms

The sequencer orders the execution of the tasks according to their priority and the influence of pause/resume.

### <b>Keywords</b>

Utility, sequencer, priority, task

### <b>Directory contents</b>

  - Sequencer/Sequencer_task_pauseresume/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
  - Sequencer/Sequencer_task_pauseresume/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - Sequencer/Sequencer_task_pauseresume/Inc/main.h                   Header for main.c module
  - Sequencer/Sequencer_task_pauseresume/Src/stm32wbaxx_it.c          Interrupt handlers
  - Sequencer/Sequencer_task_pauseresume/Src/stm32wbaxx_hal_msp.c     HAL MSP module
  - Sequencer/Sequencer_task_pauseresume/Src/main.c                   Main program
  - Sequencer/Sequencer_task_pauseresume/Src/system_stm32wbaxx.c      STM32WBAxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA52CGUx devices.

  - This example has been tested with NUCLEO-WBA52CG board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

