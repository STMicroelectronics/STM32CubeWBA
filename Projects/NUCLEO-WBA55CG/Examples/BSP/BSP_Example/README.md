## <b>BSP_Example Description</b>

This example describes how to use the bsp API. 

The 3 LEDs toggle one after the other in a forever loop. 
This shows the usage of BSP_LED_Toggle.

When the buttons SW1, SW2 or SW3 are pressed, the toggle delay is changed.
  - SW1 set the delay to 100ms.
  - SW2 set the delay to 500ms.
  - SW3 set the delay to 1000ms.

In this example:

    - LED1, LED2 and LED3 toggle in the same loop. Toggle frequency 
      is change by using SW1, SW2 or SW3 push buttons.

    - HCLK is configured at 100 MHz.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

System, BSP, EXTI, Output, Alternate function, Push-pull, Toggle

### <b>Directory contents</b>

  - BSP/BSP_Example/Inc/stm32wbaxx_nucleo_conf.h     BSP configuration file
  - BSP/BSP_Example/Inc/stm32wbaxx_hal_conf.h        HAL configuration file
  - BSP/BSP_Example/Inc/stm32wbaxx_it.h              Interrupt handlers header file
  - BSP/BSP_Example/Inc/main.h                       Header for main.c module  
  - BSP/BSP_Example/Src/stm32wbaxx_it.c              Interrupt handlers
  - BSP/BSP_Example/Src/main.c                       Main program
  - BSP/BSP_Example/Src/system_stm32wbaxx.c          STM32WBAxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices.

  - This example has been tested with NUCLEO-WBA55CG board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


