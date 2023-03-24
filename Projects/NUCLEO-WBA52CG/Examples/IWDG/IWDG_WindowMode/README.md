## <b>IWDG_WindowMode Example Description</b>

How to periodically update the IWDG reload counter and simulate a software fault that generates
an MCU IWDG reset after a preset laps of time.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 100 MHz.

The IWDG time-out is set to 762 ms (the time-out may varies due to LSI frequency
dispersion).

The Window option is enabled with a window register value set to 400 ms.

To prevent a reset, the down-counter must be reloaded when its value is:

 - Lower than the window register value (400ms)
 - Greater than 0x0
 
The IWDG counter is therefore refreshed each 450 ms in the main program infinite loop to
prevent a IWDG reset (762 - 450 = 312 within the interval).
LD1 is also toggled each 450 ms indicating that the program is running.

LD3 will turn on if any error occurs.

An EXTI Line is connected to a GPIO pin, configured to generate an interrupt
when the USER push-button (PC.13) is pressed.

Once the EXTI Line event occurs by pressing the USER push-button (PC.13),
the refresh period is set to 200 ms.
That will make refresh being outside window value. As a result, when the IWDG counter is reloaded,
the IWDG reset occurs.

In the ISR, a write to invalid address generates a Hard fault exception containing
an infinite loop and preventing to return to main program (the IWDG counter is
not refreshed).
As a result, when the IWDG counter falls to 0, the IWDG reset occurs.
If the IWDG reset is generated, after the system resumes from reset, LD1 turns on for 4 seconds.

If the EXTI Line event does not occur, the IWDG counter is indefinitely refreshed
in the main program infinite loop, and there is no IWDG reset.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate
    delay (in milliseconds) based on variable incremented in SysTick ISR. This
    implies that if HAL_Delay() is called from a peripheral ISR process, then
    the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

System, IWDG, reload counter, MCU Reset, Window mode, Timeout, Software fault

### <b>Directory contents</b>

  - IWDG/IWDG_WindowMode/Inc/stm32wbaxx_nucleo_conf.h BSP configuration file
  - IWDG/IWDG_WindowMode/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
  - IWDG/IWDG_WindowMode/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - IWDG/IWDG_WindowMode/Inc/main.h                   Header for main.c module
  - IWDG/IWDG_WindowMode/Src/stm32wbaxx_it.c          Interrupt handlers
  - IWDG/IWDG_WindowMode/Src/main.c                   Main program
  - IWDG/IWDG_WindowMode/Src/system_stm32wbaxx.c      STM32WBAxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA52CGUx devices.

  - This example has been tested with NUCLEO-WBA52CG board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

