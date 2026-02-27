## <b>TIM_PWMOutput Example Description</b>

This example shows how to configure the TIM peripheral in PWM (Pulse Width Modulation)
mode.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.

The SystemClock_Config() function is used to configure the system clock for STM32WBA25CEUx devices :
The CPU at 64 MHz.

    In this example TIM2 input clock (TIM2CLK) is set to APB1 clock (PCLK1),
    since APB2 prescaler is equal to 1.
      TIM1CLK = PCLK1
      PCLK1 = HCLK
      => TIM2CLK = HCLK = SystemCoreClock

    To get TIM2 counter clock at 1 MHz, the prescaler is computed as follows:
       Prescaler = (TIM2CLK / TIM2 counter clock) - 1
       Prescaler = ((SystemCoreClock) /1 MHz) - 1

    To get TIM2 output clock at 25 KHz, the period (ARR)) is computed as follows:
       ARR = (TIM2 counter clock / TIM2 output clock) - 1
           = 39

    TIM2 Channel1 duty cycle = (TIM2_CCR1/ TIM1_ARR + 1)* 100 = 50%
    TIM2 Channel2 duty cycle = (TIM2_CCR2/ TIM1_ARR + 1)* 100 = 37.5%
    TIM2 Channel3 duty cycle = (TIM2_CCR3/ TIM1_ARR + 1)* 100 = 25%
    TIM2 Channel4 duty cycle = (TIM2_CCR4/ TIM1_ARR + 1)* 100 = 12.5%

LD3 is ON when there are an error.

The PWM waveforms can be displayed using an oscilloscope.

#### <b>Notes</b>

 1. The duty cycles values mentioned above are theoretical (obtained when the system clock frequency is exactly 64 MHz).
    They might be slightly different depending on system clock frequency precision.

 2. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 3. This example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.
	
 4. To be able to display PWM waveform using an oscilloscope please make sure to close the SB23, SB24, S29 and SB33

### <b>Keywords</b>

Timer, TIM, Output, signal, PWM, Oscilloscope, Frequency, Duty cycle, Waveform

### <b>Directory contents</b>

  - TIM/TIM_PWMOutput/Inc/stm32wbaxx_nucleo_conf.h     BSP configuration file
  - TIM/TIM_PWMOutput/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
  - TIM/TIM_PWMOutput/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - TIM/TIM_PWMOutput/Inc/main.h                  Header for main.c module  
  - TIM/TIM_PWMOutput/Src/stm32wbaxx_it.c          Interrupt handlers
  - TIM/TIM_PWMOutput/Src/main.c                  Main program
  - TIM/TIM_PWMOutput/Src/stm32wbaxx_hal_msp.c     HAL MSP file
  - TIM/TIM_PWMOutput/Src/system_stm32wbaxx.c      STM32WBAxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA25CEUx devices.
  - In this example, the clock is set to 64 MHz.

  - This example has been tested with STMicroelectronics NUCLEO-WBA25CE 
    board and can be easily tailored to any other supported device 
    and development board.

  - NUCLEO-WBA25CE Set-up
   Connect the following pins to an oscilloscope to monitor the different waveforms:

       - TIM2_CH1 : PB.12 (pin 4 in CN8 connector)
       - TIM2_CH2 : PA.08 (pin 6 in CN8 connector)
       - TIM2_CH3 : PA.07 (pin 7 in CN8 connector)
       - TIM2_CH4 : PA.06 (pin 2 in CN6 connector)


### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

