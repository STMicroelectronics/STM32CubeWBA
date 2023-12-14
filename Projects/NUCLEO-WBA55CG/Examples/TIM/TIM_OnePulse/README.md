## <b>TIM_OnePulse Example Description</b>

This example shows how to use the TIMER peripheral to generate a single pulse when
a rising edge of an external signal is received on the TIMER Input pin.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock for STM32WBA55CGUx Devices :
The CPU at 100 MHz 

Clock setup for TIM1 :

  TIM1CLK = SystemCoreClock = 100 MHz.
  
  Prescaler = (TIM1CLK /TIM1 counter clock) - 1
  
  The prescaler value is computed in order to have TIM1 counter clock 
  set at 1000000 Hz.
  
  The Autoreload value is 65535 (TIM1->ARR), so the maximum frequency value to 
  trigger the TIM1 input is 1000000/65535 [Hz] = 15 Hz
 
Configuration of TIM1 in One Pulse Mode:
 
  - The external signal is connected to TIM1_CH2 pin (PA12), 
    and a rising edge on this input is used to trigger the Timer.
  - The One Pulse signal is output on TIM1_CH1 (PA11).

  The delay value is fixed to:

   - Delay = CCR1/TIM1 counter clock 
           = 16383 / 1000000 [sec]
           = 16 ms
           
  The pulse value is fixed to :

   - Pulse value = (TIM_Period - TIM_Pulse)/TIM1 counter clock  
                 = (65535 - 16383) / 1000000 [sec]
                 = 49 ms

  The one pulse waveform can be displayed using an oscilloscope and it looks
  like this.

LD3 is ON when there is an error.

                                ___
                               |   |
  CH2 _________________________|   |__________________________________________
 
                                             ___________________________
                                            |                           |
  CH1 ______________________________________|                           |_____
                               <---Delay----><------Pulse--------------->

 - The delay and pulse values mentioned above are theoretical (obtained when the system clock frequency is exactly 100 MHz).
   They might be slightly different depending on system clock frequency precision.

### <b>Keywords</b>

Timer, Output, signals, One Pulse, PWM, Oscilloscope, External signal, Autoreload, Waveform

### <b>Directory contents</b>

  - TIM/TIM_OnePulse/Inc/stm32wbaxx_nucleo_conf.h  BSP configuration file
  - TIM/TIM_OnePulse/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
  - TIM/TIM_OnePulse/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - TIM/TIM_OnePulse/Inc/main.h                   Header for main.c module  
  - TIM/TIM_OnePulse/Src/stm32wbaxx_it.c          Interrupt handlers
  - TIM/TIM_OnePulse/Src/main.c                   Main program
  - TIM/TIM_OnePulse/Src/stm32wbaxx_hal_msp.c     HAL MSP file
  - TIM/TIM_OnePulse/Src/system_stm32wbaxx.c      STM32WBAxx system source file


### <b>Hardware and Software environment</b>

   - This example runs on STM32WBA55CGUx devices.
   - In this example, the clock is set to 100 MHz.
    
  - This example has been tested with NUCLEO-WBA55CG board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-WBA55CG Set-up
   - Connect the external signal to the TIM1_CH2 pin (PA12) (pin 17 in CN4 connector)
   - Connect the TIM1_CH1 pin(PA11) (pin 24 in CN4 connector) to an oscilloscope to monitor the waveform.  


### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files: Project->Rebuild all
 - Load project image: Project->Download and Debug
 - Run program: Debug->Go(F5) 

