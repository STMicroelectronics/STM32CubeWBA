## <b>TIM_DMA Example Description</b>

Use of the DMA with TIMER Update request to transfer data from memory to TIMER Capture Compare Register 3 (TIMx_CCR3).

  The following configuration values are used in this example:

    - TIM1CLK = SystemCoreClock
    - Counter repetition = 3 
    - Prescaler = 0 
    - TIM1 counter clock = SystemCoreClock
    - SystemCoreClock is set to 100 MHz for STM32WBAxx

  The objective is to configure TIM1 channel 3 to generate complementary PWM 
  (Pulse Width Modulation) signal with a frequency equal to 17.57 KHz, and a variable 
  duty cycle that is changed by the DMA after a specific number of Update DMA request.

  The number of this repetitive requests is defined by the TIM1 Repetition counter,
  each 4 Update Requests, the TIM1 Channel 3 Duty Cycle changes to the next new 
  value defined by the aCCValue_Buffer.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock for STM32WBA65RIVx Devices :
The CPU at 100 MHz  

  The PWM waveform can be displayed using an oscilloscope.
 
  In nominal mode (except at start) , it should looks like this :
    
       .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   
        ___________     _______         ___             ___________    __
      _|           |___|       |_______|   |___________|           |__|             
       <----57us -----><----57us -----><----57us -----><----57us ----->


#### <b>Notes</b>  
 1. PWM signal frequency value mentioned above is theoretical (obtained when the system clock frequency 
      is exactly 100 MHz). Since the generated system clock frequency may vary from one board to another observed
      PWM signal frequency might be slightly different.

 2. Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. This
      implies that if HAL_Delay() is called from a peripheral ISR process, then 
      the SysTick interrupt must have higher priority (numerically lower)

 3. This example needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.
  

### <b>Keywords</b>

Timer, DMA, PWM, Frequency, Duty Cycle, Waveform, Oscilloscope, Output, Signal

### <b>Directory contents</b>

    - TIM/TIM_DMA/Inc/stm32wbaxx_nucleo_conf.h     BSP configuration file
    - TIM/TIM_DMA/Inc/stm32wbaxx_hal_conf.h        HAL configuration file
    - TIM/TIM_DMA/Inc/stm32wbaxx_it.h              Interrupt handlers header file
    - TIM/TIM_DMA/Inc/main.h                       Header for main.c module  
    - TIM/TIM_DMA/Src/stm32wbaxx_it.c              Interrupt handlers
    - TIM/TIM_DMA/Src/main.c                       Main program
    - TIM/TIM_DMA/Src/stm32wbaxx_hal_msp.c         HAL MSP file
    - TIM/TIM_DMA/Src/system_stm32wbaxx.c          STM32WBAxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA65RIVx devices.
  - In this example, the clock is set to 100 MHz.
    
  - This example has been tested with STMicroelectronics NUCLEO-WBA65RI 
    board and can be easily tailored to any other supported device 
    and development board.

  - NUCLEO-WBA65RI Set-up
    - Connect the TIM1 pin to an oscilloscope to monitor the different waveforms: 
    - TIM1 CH3 (PB.4 (pin 34 in CN4 connector))

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example