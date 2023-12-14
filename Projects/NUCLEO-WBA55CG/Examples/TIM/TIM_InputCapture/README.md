## <b>TIM_InputCapture Example Description</b>

How to use the TIM peripheral to measure an external signal frequency.
 
  At the beginning of the main program the HAL_Init() function is called to reset 
  all the peripherals, initialize the Flash interface and the systick.
  The SystemClock_Config() function is used to configure the system clock for STM32WBA55CGUx Devices :
  The CPU at 100 MHz 

  The TIM1CLK frequency is set to SystemCoreClock (Hz), the Prescaler is 0,
  so the TIM1 counter clock is SystemCoreClock (Hz).

  SystemCoreClock is set to 100 MHz for STM32WBA55CGUx Devices.

  TIM1 is configured in Input Capture Mode: the external signal is connected to 
  TIM1 Channel2 used as input pin.
  To measure the frequency we use the TIM1 CC interrupt request, so in the 
  TIM1 CC IRQHandler routine, the frequency of the external signal is computed.

  The "uwFrequency" variable contains the external signal frequency:

			uwFrequency = TIM1 counter clock / uwDiffCapture (Hz)

  where "uwDiffCapture" is the difference between two consecutive TIM1 captures.

  The minimum frequency value to measure is :

			TIM1 counter clock / CCR MAX = (SystemCoreClock) / 65535

  Due to TIM1 CC IRQHandler processing time (around 4ns), the maximum
  frequency value to measure is around 700kHz.

LD3 is ON when there are an error.
  
#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. This example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.


### <b>Keywords</b>

Timer, TIM, DMA, Frequency, Input, Capture, External Signal, Measurement

### <b>Directory contents</b>

  - TIM/TIM_InputCapture/Inc/stm32wbaxx_nucleo_conf.h   BSP configuration file
  - TIM/TIM_InputCapture/Inc/stm32wbaxx_hal_conf.h    	HAL configuration file
  - TIM/TIM_InputCapture/Inc/stm32wbaxx_it.h          	Interrupt handlers header file
  - TIM/TIM_InputCapture/Inc/main.h                  	Header for main.c module
  - TIM/TIM_InputCapture/Src/stm32wbaxx_it.c          	Interrupt handlers
  - TIM/TIM_InputCapture/Src/main.c                  	Main program
  - TIM/TIM_InputCapture/Src/stm32wbaxx_hal_msp.c     	HAL MSP file
  - TIM/TIM_InputCapture/Src/system_stm32wbaxx.c      	STM32WBAxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices.
  - In this example, the clock is set to 100 MHz.
    
  - This example has been tested with STMicroelectronics NUCLEO-WBA55CG
    board and can be easily tailored to any other supported device 
    and development board.      

  - NUCLEO-WBA55CG Set-up
    - Connect the external signal to measure to the TIM1 CH2 pin (PA.12 (pin 3 in CN6 connector)).

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

