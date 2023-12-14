## <b>TIM_PWMInput Example Description</b>

How to use the TIM peripheral to measure the frequency and 
duty cycle of an external signal.

The TIM1CLK frequency is set to SystemCoreClock (Hz), the Prescaler is 0 so the 
counter clock is SystemCoreClock (Hz).
SystemCoreClock is set to 100 MHz for STM32WBA55CGUx Devices.

TIM1 is configured in PWM Input Mode: the external signal is connected to 
TIM1 Channel2 used as input pin.
To measure the frequency and the duty cycle, we use the TIM1 CC2 interrupt request,
so in the timer IRQ routine (via call to function HAL_TIM_IC_CaptureCallback() ),
the frequency and the duty cycle of the external signal are computed.

"uwFrequency" variable contains the external signal frequency:

     TIM1 counter clock = SystemCoreClock
     uwFrequency = TIM1 counter clock / TIM1_CCR2 in Hz

"uwDutyCycle" variable contains the external signal duty cycle:

     uwDutyCycle = (TIM1_CCR1*100)/(TIM1_CCR2) in %.

The minimum frequency value to measure is :
     (TIM1 counter clock / CCR MAX) = (100 MHz)/ 65535

In case of error, LD3 is turned ON.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. This example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Timer, TIM, Input, signals, PWM, External signal, Frequency, Duty cycle, Measure

### <b>Directory contents</b>

  - TIM/TIM_PWMInput/Inc/stm32wbaxx_nucleo_conf.h   BSP configuration file
  - TIM/TIM_PWMInput/Inc/stm32wbaxx_hal_conf.h    	HAL configuration file
  - TIM/TIM_PWMInput/Inc/stm32wbaxx_it.h          	Interrupt handlers header file
  - TIM/TIM_PWMInput/Inc/main.h                  	Header for main.c module  
  - TIM/TIM_PWMInput/Src/stm32wbaxx_it.c          	Interrupt handlers
  - TIM/TIM_PWMInput/Src/main.c                  	Main program
  - TIM/TIM_PWMInput/Src/stm32wbaxx_hal_msp.c     	HAL MSP file
  - TIM/TIM_PWMInput/Src/system_stm32wbaxx.c      	STM32WBAxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices.
    
  - This example has been tested with STMicroelectronics NUCLEO-WBA55CG 
    board and can be easily tailored to any other supported device 
    and development board.

  - NUCLEO-WBA55CG Set-up
    - Connect the external signal to measure to the TIM1 CH2 pin (PA12) (connected to pin 17 in CN4 connector).

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

