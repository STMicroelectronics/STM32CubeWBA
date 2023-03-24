## <b>CORTEX_MPU example Description</b>

Presentation of the MPU features. This example configures MPU attributes of different
MPU regions then configures a memory area as privileged read-only, and attempts to
perform read and write operations in different modes.

If the access is permitted LD1 is toggling. If the access is not permitted,
a memory management fault is generated and LD1 is ON.

To generate an MPU memory fault exception due to an access right error, press
user button.

### <b>Notes</b>

  - Using STM32CubeIDE debugger, when hard fault is generated user shall press the debugger "Run" button to continue the execution

### <b>Keywords</b>

MPU, interrupt, MPU memory fault exception,

### <b>Directory contents</b>

  - CORTEX/CORTEX_MPU/Inc/stm32wbaxx_it.h         Interrupt handlers header file
  - CORTEX/CORTEX_MPU/Inc/main.h                  Header for main.c module
  - CORTEX/CORTEX_MPU/Inc/stm32_assert.h          Template file to include assert_failed function
  - CORTEX/CORTEX_MPU/Src/stm32wbaxx_it.c         Interrupt handlers
  - CORTEX/CORTEX_MPU/Src/main.c                  Main program
  - CORTEX/CORTEX_MPU/Src/system_stm32wbaxx.c     STM32WBAxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA52CGUx devices.

  - This example has been tested with a STM32WBA52CGUx embedded on an
    NUCLEO-WBA52CG board and can be easily tailored to any other supported
    device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

