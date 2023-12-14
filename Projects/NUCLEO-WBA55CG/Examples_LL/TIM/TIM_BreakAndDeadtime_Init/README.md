## <b>TIM_BreakAndDeadtime_Init Example Description</b>

Configuration of the TIM peripheral to
generate three center-aligned PWM and complementary PWM signals,
insert a defined deadtime value,
use the break feature,
and lock the break and dead-time configuration.

This example is based on the STM32WBAxx TIM LL API. The peripheral initialization
uses LL initialization function to demonstrate LL Init.

TIM1CLK is fixed to 100 MHz, the TIM1 Prescaler is set to have
TIM1 counter clock = 10 MHz.

The TIM1 auto-reload is set to generate PWM signal at 100 Hz:

The Three Duty cycles are computed as the following description:

The channel 1 duty cycle is set to 50% so channel 1N is set to 50%.

The channel 2 duty cycle is set to 25% so channel 2N is set to 75%.

The channel 3 duty cycle is set to 12.5% so channel 3N is set to 87.5%.

A dead time equal to 4 us is inserted between
the different complementary signals, and the Lock level 1 is selected.

  - The OCx output signal is the same as the reference signal except for the rising edge,
    which is delayed relative to the reference rising edge.
  - The OCxN output signal is the opposite of the reference signal except for the rising
    edge, which is delayed relative to the reference falling edge

Note that calculated duty cycles apply to the reference signal (OCxREF) from
which outputs OCx and OCxN are generated. As dead time insertion is enabled the
duty cycle measured on OCx will be slightly lower.

The break Polarity is used at High level.

The TIM1 waveforms can be displayed using an oscilloscope.

Note that LD1 and LD3 remained ON because they share the same pins respectively as TIM1_CH3 (PB4) and TIM1_CH1 (PB8). 

### <b>Keywords</b>

Timer, TIM, PWM, Signal, Duty cycle, Dead-time, Break-time, Break polarity, Oscilloscope.

### <b>Directory contents</b>

  - TIM/TIM_BreakAndDeadtime_Init/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - TIM/TIM_BreakAndDeadtime_Init/Inc/main.h                  Header for main.c module
  - TIM/TIM_BreakAndDeadtime_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - TIM/TIM_BreakAndDeadtime_Init/Src/stm32wbaxx_it.c          Interrupt handlers
  - TIM/TIM_BreakAndDeadtime_Init/Src/main.c                  Main program
  - TIM/TIM_BreakAndDeadtime_Init/Src/system_stm32wbaxx.c      STM32WBAxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices.

  - This example has been tested with NUCLEO-WBA55CG board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-WBA55CG Set-up

    - Connect the TIM1 pins to an oscilloscope to monitor the different waveforms:
      - TIM1_CH1  PB8: connected to pin 38 of CN4 connector
      - TIM1_CH1N PB2: connected to pin 3 of CN4 connector
      - TIM1_CH2  PA12: connected to pin 17 of CN4 connector
      - TIM1_CH2N PB1: connected to pin 5 of CN4 connector
      - TIM1_CH3  PB4: connected to pin 6 of CN4 connector
      - TIM1_CH3N PB9: connected to pin 23 of CN4 connector

    - Connect the TIM1 break to the GND. To generate a break event, switch this
      pin level from 0V to 3.3V.
      - TIM1_BKIN  PA2: connected to pin 32 of CN3 connector

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


