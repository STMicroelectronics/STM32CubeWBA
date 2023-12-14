## <b>LPTIM_Timeout Example Description</b>

How to implement, through the HAL LPTIM API, a timeout with the LPTIMER peripheral, to wake up 
the system from a low-power mode.

The main() function configures the LPTIMER and goes in STOP mode.
In order to start the timer a first trigger is needed on (PB8)(LPTIM_ETR).
Any successive trigger event on (PB8) will reset the counter and the timer 
will restart. The timeout value corresponds to the compare value (32000).
If after the first trigger no other trigger occurs within the expected time frame, 
the MCU is woken-up by the compare match event and LD1 toggles.

In this example the LPTIM is clocked by the LSI 

     Timeout = (Compare + 1) / LPTIM_Clock
             = (32000 + 1) / LSI_Clock_Frequency

Once the system is woken up it remains in run mode. The led keeps toggling each time the timer expires.

**Note:** that the LD3 keeps toggling since it shares the same pin with LPTIM1_ETR (PB8).

#### <b>Notes</b>

 1. This example can not be used in DEBUG mode, this is due to the fact
    that the Cortex-M33 core is no longer clocked during low power mode
    so debugging features are disabled.

 2. Care must be taken when using HAL_Delay(), this function provides accurate
    delay (in milliseconds) based on variable incremented in SysTick ISR. This
    implies that if HAL_Delay() is called from a peripheral ISR process, then 
    the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
 3. This example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.
	  
### <b>Keywords</b>

Timer, Low Power Timer, Wake up, Stop mode, LSI, Run mode

### <b>Directory contents</b>

  - LPTIM/LPTIM_Timeout/Inc/stm32wbaxx_nucleo_conf.h BSP configuration file
  - LPTIM/LPTIM_Timeout/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
  - LPTIM/LPTIM_Timeout/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - LPTIM/LPTIM_Timeout/Inc/main.h                   Header for main.c module  
  - LPTIM/LPTIM_Timeout/Src/stm32wbaxx_it.c          Interrupt handlers
  - LPTIM/LPTIM_Timeout/Src/main.c                   Main program
  - LPTIM/LPTIM_Timeout/Src/stm32wbaxx_hal_msp.c     HAL MSP module
  - LPTIM/LPTIM_Timeout/Src/system_stm32wbaxx.c      STM32WBAxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices.
    
  - This example has been tested with STMicroelectronics NUCLEO-WBA55CG
    board and can be easily tailored to any other supported device
    and development board.   
	
  - Connect an external trigger (ETR) to PB8(pin 38 in CN4 connector). 
    If the trigger is higher than 1Hz, the counter is regularly reset, the system stays in STOP mode.
    If the trigger is lower than 1Hz, the counter expires and the system is woken up.


### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred tool chain 
 - Rebuild all files and load your image into target memory
 - Run the example

