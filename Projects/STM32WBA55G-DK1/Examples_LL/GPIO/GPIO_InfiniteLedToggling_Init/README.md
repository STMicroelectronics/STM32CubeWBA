## <b>GPIO_InfiniteLedToggling_Init Example Description</b>

How to configure and use GPIOs to toggle the on-board user LEDs
every 250 ms. This example is based on the STM32WBAxx LL API. The peripheral
is initialized with LL initialization function to demonstrate LL init usage.

PB.15 IO (configured in output pushpull mode) toggles in a forever loop.
On STM32WBA55G-DK1 board this IO is connected to LD3.

In this example, HCLK is configured at 100 MHz.

### <b>Keywords</b>

System, GPIO, Output, No pull, Speed, Push-pull, Toggle

### <b>Directory contents</b>

    - GPIO/GPIO_InfiniteLedToggling_Init/Inc/stm32wbaxx_it.h          Interrupt handlers header file
    - GPIO/GPIO_InfiniteLedToggling_Init/Inc/main.h                  Header for main.c module
    - GPIO/GPIO_InfiniteLedToggling_Init/Inc/stm32_assert.h          Template file to include assert_failed function
    - GPIO/GPIO_InfiniteLedToggling_Init/Src/stm32wbaxx_it.c          Interrupt handlers
    - GPIO/GPIO_InfiniteLedToggling_Init/Src/main.c                  Main program
    - GPIO/GPIO_InfiniteLedToggling_Init/Src/system_stm32wbaxx.c      STM32WBAxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices.

  - This example has been tested with STM32WBA55G-DK1 board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


