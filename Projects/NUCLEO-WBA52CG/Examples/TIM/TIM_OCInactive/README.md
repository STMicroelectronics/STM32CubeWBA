## <b>TIM_OCInactive Example Description</b>

Configuration of the TIM peripheral in Output Compare Inactive mode 
with the corresponding Interrupt requests for each channel.

  The TIM2 frequency is set to SystemCoreClock, and the objective is
  to get TIM2 counter clock at 10 kHz so the Prescaler is computed as following:

     - Prescaler = (TIM2CLK /TIM2 counter clock) - 1

  SystemCoreClock is set to 100 MHz for STM32WBAxx Devices.

  The TIM2 CCR1 register value is equal to 10000:

     TIM2_CH1 delay = CCR1_Val/TIM2 counter clock  = 1s

  So the TIM2 Channel 1 generates a signal with a delay equal to 1s.

  The TIM2 CCR2 register value is equal to 5000:

     TIM2_CH2 delay = CCR2_Val/TIM2 counter clock = 500 ms

  So the TIM2 Channel 2 generates a signal with a delay equal to 500 ms.

  The TIM2 CCR3 register value is equal to 2500:

     TIM2_CH3 delay = CCR3_Val/TIM2 counter clock = 250 ms

  So the TIM2 Channel 3 generates a signal with a delay equal to 250 ms.

  The TIM2 CCR4 register value is equal to 1250:

     TIM2_CH4 delay = CCR4_Val/TIM2 counter clock = 125 ms

  So the TIM2 Channel 4 generates a signal with a delay equal to 125 ms.

  While the counter is lower than the Output compare registers values, which 
  determines the Output delay, the PB0, PB1, PB2 and PB3 pin are turned ON. 

  When the counter value reaches the Output compare registers values, the Output 
  Compare interrupts are generated and, in the handler routine, these pins are turned OFF.
  
#### <b>Notes</b>

 1. Delay values mentioned above are theoretical (obtained when the system clock frequency 
    is exactly 100 MHz). Since the generated system clock frequency may vary from one board to another observed
    delay might be slightly different.

 2. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 3. This example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Timer, TIM, Output, Compare, InActive, Signals, Delay, Frequency

### <b>Directory contents</b>

  - TIM/TIM_OCInactive/Inc/stm32wbaxx_nucleo_conf.h     BSP configuration file
  - TIM/TIM_OCInactive/Inc/stm32wbaxx_hal_conf.h    	HAL configuration file
  - TIM/TIM_OCInactive/Inc/stm32wbaxx_it.h          	Interrupt handlers header file
  - TIM/TIM_OCInactive/Inc/main.h                  		Header for main.c module  
  - TIM/TIM_OCInactive/Src/stm32wbaxx_it.c          	Interrupt handlers
  - TIM/TIM_OCInactive/Src/main.c                  		Main program
  - TIM/TIM_OCInactive/Src/stm32wbaxx_hal_msp.c     	HAL MSP file
  - TIM/TIM_OCInactive/Src/system_stm32wbaxx.c      	STM32WBAxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA52CGUx devices.
    
  - This example has been tested with STMicroelectronics NUCLEO-WBA52CG 
    board and can be easily tailored to any other supported device 
    and development board.      

  - NUCLEO-WBA52CG Set-up
   Connect the following pins to an oscilloscope to monitor the different waveforms:
      - PB0: wave form of (TIM2_CH1) ((pin 25 in CN4 connector))
      - PB1: wave form of (TIM2_CH2) ((pin 5 in CN4 connector))
      - PB2: wave form of (TIM2_CH3) ((pin 3 in CN4 connector))
      - PB3: wave form of (TIM2_CH4) ((pin 10 in CN4 connector))

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

  PB0
               _____________________________________________________
 _____________|                                                     |________________
              <-----------------------1sec-------------------------->
  PB1
               ___________________________
 _____________|                           |__________________________________________
              <-----------500ms----------->
  PB2
               _________________
 _____________|                 |____________________________________________________
              <------250ms------>
 
  PB3
               ___________
 _____________|           |___________________________________________________________
              <---125ms--->


