## <b>CORTEXM_SysTick_LSE Example Description</b>

How to use the LSE as SysTick clock source with a 1 ms timebase to toggle LEDs.

  A "HAL_Delay" function is implemented based on the SysTick end-of-count event.
  Three LEDs are toggled with a timing defined by the HAL_Delay function.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate
    delay (in milliseconds) based on variable incremented in SysTick ISR. This
    implies that if HAL_Delay() is called from a peripheral ISR process, then 
    the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application need to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

System, Cortex, Time base, Systick, LSE 

### <b>Directory contents</b>

  - CORTEX/CORTEXM_SysTick_LSE/Inc/stm32wbaxx_nucleo_conf.h     BSP configuration file
  - CORTEX/CORTEXM_SysTick_LSE/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
  - CORTEX/CORTEXM_SysTick_LSE/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - CORTEX/CORTEXM_SysTick_LSE/Inc/main.h                  Header for main.c module
  - CORTEX/CORTEXM_SysTick_LSE/Src/stm32wbaxx_it.c          Interrupt handlers
  - CORTEX/CORTEXM_SysTick_LSE/Src//stm32wbaxx_hal_msp.c     HAL MSP file
  - CORTEX/CORTEXM_SysTick_LSE/Src/main.c                  Main program
  - CORTEX/CORTEXM_SysTick_LSE/Src/system_stm32wbaxx.c      STM32WBAxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices.
    
  - This example has been tested with NUCLEO-WBA55CG board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

- Open your preferred toolchain
- Rebuild all files and load your image into target memory
- Run the example
