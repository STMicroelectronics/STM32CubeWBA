## <b>ICACHE_Memory_Remap Example Description</b>
How to execute code from a remapped region configured through the ICACHE HAL driver.

This example describes how to remap the MX25R3235F external Flash to C-bus and execute code located in this memory.

First, a piece of code which blinks once the **Green led** is executed from external Flash before remapping.
After remapping to C-Bus, the **Green led** will blink infinitely.

This project is targeted to run on STM32WBA25CE device on NUCLEO-WBA25CE board from STMicroelectronics.
At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals and initialize the systick used as 1ms HAL timebase.
The SystemClock_Config() function is used to configure the system clock (SYSCLK) to run at 64 MHz.

NUCLEO-WBA25CE board's LED can be used to monitor the transfers status:

- **Green LED is blinking** when code has been correctly executed before and after remapping.
- **Red LED is ON** when any error occurred.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

 3. The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
    from Flash memory, and reach the maximum performance.

### <b>Keywords</b>

System, ICACHE, XSPI, FLASH, Memory Remap, C-bus

### <b>Directory contents</b>

File | Description
 --- | ---
  ICACHE/ICACHE_Memory_Remap/Src/main.c                      | Main program
  ICACHE/ICACHE_Memory_Remap/Src/system_stm32wbaxx.c         | STM32WBAxx system source file
  ICACHE/ICACHE_Memory_Remap/Src/stm32wbaxx_it.c             | Interrupt handlers
  ICACHE/ICACHE_Memory_Remap/Src/stm32wbaxx_hal_msp.c        | HAL MSP module
  ICACHE/ICACHE_Memory_Remap/Inc/main.h                      | Main program header file
  ICACHE/ICACHE_Memory_Remap/Inc/stm32wbaxx_nucleo_conf.h    | BSP Configuration file
  ICACHE/ICACHE_Memory_Remap/Inc/stm32wbaxx_hal_conf.h       | HAL Configuration file
  ICACHE/ICACHE_Memory_Remap/Inc/stm32wbaxx_it.h             | Interrupt handlers header file
  ICACHE/ICACHE_Memory_Remap/Inc/mx25r3235f_conf             | BSP component configuration file

### <b>Hardware and Software environment</b>

- This example runs on STM32WBA25CE devices without security enabled (TZEN=0).
- This example has been tested with STMicroelectronics NUCLEO-WBA25CE (MB1801 and MB2293)
  board and can be easily tailored to any other supported device
  and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

- Open your preferred toolchain
- Rebuild all files and load your image into target memory
- Run the example
